#include "static_draw_assets50dw.h"
#include "scene_asset48d.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Static draw preparation requires strict finite IEEE arithmetic"
#endif

namespace rr::assets {
namespace {
static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559,
    "IEEE binary64 required");
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
    "IEEE binary32 required");
constexpr std::uint32_t maxSourceSurfaces = 16384;

bool reject(StaticDrawAssetDiagnostic50DW& d, StaticDrawAssetError50DW error) noexcept {
    d.error = error;
    return false;
}
bool addBounded(std::uint32_t& total, std::uint32_t count, std::uint32_t capacity) noexcept {
    if (total > capacity || count > capacity - total) return false;
    total += count;
    return true;
}
bool budgetsValid(const StaticDrawBudgets50DW& b) noexcept {
    return b.assetCapacity && b.assetCapacity <= maxStaticDrawAssets50DW &&
        b.instanceCapacity && b.instanceCapacity <= maxStaticDrawInstances50DW &&
        b.sourceByteCapacity && b.sourceByteCapacity <= max_scene_bytes48d &&
        b.sourceVertexCapacity && b.sourceVertexCapacity <= max_mesh_vertices &&
        b.sourceIndexCapacity && b.sourceIndexCapacity <= max_mesh_indices &&
        b.sourceSurfaceCapacity && b.sourceSurfaceCapacity <= maxSourceSurfaces &&
        b.vertexCapacity && b.vertexCapacity <= max_mesh_vertices &&
        b.indexCapacity && b.indexCapacity <= max_mesh_indices &&
        b.spanCapacity && b.spanCapacity <= maxStaticDrawSpans50DW;
}

// A nonallocating framing scan. This does not replace parseSceneAsset48D's
// semantic checks: it bounds every serialized count that causes allocations,
// including counts the parser would otherwise reject only after allocation.
struct Scan {
    const std::vector<std::uint8_t>& bytes;
    std::size_t at = 0;
    bool good = true;
    bool skip(std::uint64_t count) noexcept {
        if (!good || at > bytes.size() || count > bytes.size() - at) {
            good = false;
            return false;
        }
        at += static_cast<std::size_t>(count);
        return true;
    }
    std::uint32_t integer(unsigned width = 4) noexcept {
        const auto start = at;
        if (!skip(width)) return 0;
        std::uint32_t value = 0;
        for (unsigned n = 0; n < width; ++n) value |= std::uint32_t(bytes[start + n]) << (8 * n);
        return value;
    }
    bool name() noexcept {
        const auto count = integer();
        return good && count && count <= 128 && skip(count);
    }
};
struct SourceCounts {
    std::uint32_t vertices = 0, indices = 0, surfaces = 0;
    std::uint32_t lod0Vertices = 0, lod0Indices = 0, lod0Spans = 0;
    bool worldSpace = false;
};
bool scanScene(const std::vector<std::uint8_t>& bytes, SourceCounts& output,
    StaticDrawAssetDiagnostic50DW& d) noexcept {
    Scan r{bytes};
    const auto malformed = [&]() noexcept {
        d.byteOffset = r.at;
        return reject(d, StaticDrawAssetError50DW::serializedScene);
    };
    if (bytes.size() < 96 || bytes.size() > max_scene_bytes48d ||
        std::memcmp(bytes.data(), "RRSCENE1", 8) != 0) return malformed();
    r.at = 8;
    if (r.integer() != 1) return malformed();
    const auto world = r.integer(), palette = r.integer();
    const auto nodes = r.integer(), materials = r.integer(), lods = r.integer();
    const auto declaredVertices = r.integer(), declaredSurfaces = r.integer();
    if (!r.good || world > 1 || palette > 1 || !materials || materials > 4096 ||
        (lods != 1 && lods != 4) || !declaredVertices || declaredVertices > max_mesh_vertices ||
        !declaredSurfaces || declaredSurfaces > maxSourceSurfaces) return malformed();
    if (nodes != 1 || palette != 0)
        return reject(d, StaticDrawAssetError50DW::unsupportedScene);
    if (!r.skip(7 * 8) || !r.name()) return malformed();
    if (r.integer() != 0xffffffffu || !r.skip((16 + 16 + 7) * 8)) return malformed();
    for (std::uint32_t n = 0; n < materials; ++n) if (!r.name()) return malformed();

    SourceCounts counts{};
    counts.worldSpace = world != 0;
    for (std::uint32_t lod = 0; lod < lods; ++lod) {
        if (!r.skip(8)) return malformed();
        const auto surfaces = r.integer();
        if (!r.good || !surfaces || !addBounded(counts.surfaces, surfaces, maxSourceSurfaces))
            return malformed();
        if (lod == 0) counts.lod0Spans = surfaces;
        for (std::uint32_t surface = 0; surface < surfaces; ++surface) {
            d.surface = surface;
            const auto material = r.integer(), color = r.integer();
            if (!r.skip(8)) return malformed();
            const auto vertices = r.integer(), indices = r.integer();
            if (!r.good || material >= materials || color > 1 || !vertices || !indices || indices % 3 ||
                !addBounded(counts.vertices, vertices, max_mesh_vertices) ||
                !addBounded(counts.indices, indices, max_mesh_indices)) return malformed();
            if (lod == 0) {
                counts.lod0Vertices += vertices;
                counts.lod0Indices += indices;
            }
            for (std::uint32_t vertex = 0; vertex < vertices; ++vertex) {
                if (!r.skip(8 * 8 + 4)) return malformed();
                const auto bone = r.integer(2);
                if (!r.good) return malformed();
                if (bone != 0) {
                    d.vertex = vertex;
                    d.byteOffset = r.at - 2;
                    return reject(d, StaticDrawAssetError50DW::unsupportedScene);
                }
            }
            if (!r.skip(std::uint64_t(indices) * 4)) return malformed();
        }
    }
    const auto spans = r.integer();
    if (!r.good || spans != counts.lod0Spans || !r.skip(std::uint64_t(spans) * 24)) return malformed();
    const auto meshBytes = r.integer();
    if (!r.good || meshBytes < 16 || meshBytes > max_asset_file_bytes ||
        meshBytes != bytes.size() - r.at) return malformed();
    if (std::memcmp(bytes.data() + r.at, "RRMESH01", 8) != 0) return malformed();
    r.skip(8);
    const auto meshVertices = r.integer(), meshIndices = r.integer();
    if (!r.good || meshVertices != counts.lod0Vertices || meshIndices != counts.lod0Indices ||
        16ull + 24ull * meshVertices + 4ull * meshIndices != meshBytes ||
        counts.vertices != declaredVertices || counts.surfaces != declaredSurfaces ||
        !r.skip(24ull * meshVertices + 4ull * meshIndices) || r.at != bytes.size()) return malformed();
    output = counts;
    d.surface = d.vertex = 0;
    d.byteOffset = 0;
    return true;
}

bool checkMatrix(const std::array<double, 16>& matrix, bool worldSpace,
    StaticDrawAssetDiagnostic50DW& d) noexcept {
    for (std::uint32_t lane = 0; lane < 16; ++lane) {
        d.lane = lane;
        if (!std::isfinite(matrix[lane])) return reject(d, StaticDrawAssetError50DW::matrix);
        if (lane % 4 == 3 && matrix[lane] != (lane == 15 ? 1.0 : 0.0))
            return reject(d, StaticDrawAssetError50DW::matrix);
        // Quantization origin is already in semantic positions. The observed
        // static world-quantized subset has explicit identity instance pose.
        if (worldSpace && matrix[lane] != (lane % 5 == 0 ? 1.0 : 0.0))
            return reject(d, StaticDrawAssetError50DW::unsupportedScene);
    }
    d.lane = 0;
    return true;
}
bool project(const SceneVertex48D& source, const std::array<double, 16>& matrix,
    Vertex48B& target, StaticDrawAssetDiagnostic50DW& d) noexcept {
    for (std::uint32_t axis = 0; axis < 3; ++axis) {
        d.lane = axis;
        double sum = 0;
        for (std::uint32_t k = 0; k < 3; ++k) {
            // Separate stored operations preserve the existing static-world
            // scalar contract even on a target with fused multiply-add.
            volatile double product = source.position_local[k] * matrix[k * 4 + axis];
            if (!std::isfinite(product)) return reject(d, StaticDrawAssetError50DW::arithmetic);
            volatile double next = sum + product;
            if (!std::isfinite(next)) return reject(d, StaticDrawAssetError50DW::arithmetic);
            sum = next;
        }
        volatile double value = sum + matrix[12 + axis];
        const double maximum = static_cast<double>(std::numeric_limits<float>::max());
        if (!std::isfinite(value) || value < -maximum || value > maximum)
            return reject(d, StaticDrawAssetError50DW::arithmetic);
        target.position[axis] = static_cast<float>(value);
    }
    d.lane = 0;
    return true;
}
}

struct StaticDrawAssets50DW::Storage {
    struct Asset { Scene48D scene; Mesh48B mesh; };
    std::vector<Asset> assets;
    std::vector<StaticDrawInstance50DW> instances;
    std::vector<Vertex48B> vertices;
    std::vector<std::uint32_t> indices;
    std::vector<SceneSpan48D> spans;
    std::vector<StaticDrawSpanSource50DW> spanSources;
    StaticDrawView50DW view{};
};

StaticDrawAssets50DW::StaticDrawAssets50DW() noexcept = default;
StaticDrawAssets50DW::~StaticDrawAssets50DW() = default;

bool StaticDrawAssets50DW::load(const StaticDrawAssetsInput50DW& input,
    StaticDrawAssetDiagnostic50DW& d) noexcept {
    d.error = StaticDrawAssetError50DW::none;
    d.asset = d.instance = d.surface = d.vertex = d.lane = 0;
    d.byteOffset = 0;
    d.parserMessage.clear();
    if (storage_) return reject(d, StaticDrawAssetError50DW::alreadyLoaded);
    if (!input.assets || !input.instances || !input.assetCount || !input.instanceCount)
        return reject(d, StaticDrawAssetError50DW::input);
    const auto& b = input.budgets;
    if (!budgetsValid(b) || input.assetCount > b.assetCapacity || input.instanceCount > b.instanceCapacity)
        return reject(d, StaticDrawAssetError50DW::budget);
    if (std::fegetround() != FE_TONEAREST) return reject(d, StaticDrawAssetError50DW::roundingMode);
    std::size_t sourceBytes = 0;
    for (std::uint32_t asset = 0; asset < input.assetCount; ++asset) {
        d.asset = asset;
        if (!input.assets[asset].sceneBytes) return reject(d, StaticDrawAssetError50DW::input);
        const auto bytes = input.assets[asset].sceneBytes->size();
        if (bytes > b.sourceByteCapacity - sourceBytes) return reject(d, StaticDrawAssetError50DW::budget);
        sourceBytes += bytes;
    }
    try {
        // Only compact per-asset count records allocate before all geometry
        // budgets pass. Their extent is bounded by the explicit asset budget.
        std::vector<SourceCounts> counts(input.assetCount);
        std::uint32_t sourceVertices = 0, sourceIndices = 0, sourceSurfaces = 0;
        for (std::uint32_t asset = 0; asset < input.assetCount; ++asset) {
            d.asset = asset;
            if (!scanScene(*input.assets[asset].sceneBytes, counts[asset], d)) return false;
            const auto& c = counts[asset];
            if (!addBounded(sourceVertices, c.vertices, b.sourceVertexCapacity) ||
                !addBounded(sourceIndices, c.indices, b.sourceIndexCapacity) ||
                !addBounded(sourceSurfaces, c.surfaces, b.sourceSurfaceCapacity))
                return reject(d, StaticDrawAssetError50DW::budget);
        }
        std::uint32_t vertices = 0, indices = 0, spans = 0;
        for (std::uint32_t instance = 0; instance < input.instanceCount; ++instance) {
            d.instance = instance;
            const auto& source = input.instances[instance];
            d.asset = source.asset;
            if (source.asset >= input.assetCount) return reject(d, StaticDrawAssetError50DW::input);
            const auto& c = counts[source.asset];
            if (!checkMatrix(source.matrix, c.worldSpace, d)) return false;
            if (!addBounded(vertices, c.lod0Vertices, b.vertexCapacity) ||
                !addBounded(indices, c.lod0Indices, b.indexCapacity) ||
                !addBounded(spans, c.lod0Spans, b.spanCapacity))
                return reject(d, StaticDrawAssetError50DW::budget);
        }
        d.instance = 0;
        std::unique_ptr<Storage> next(new Storage);
        next->assets.resize(input.assetCount);
        for (std::uint32_t asset = 0; asset < input.assetCount; ++asset) {
            d.asset = asset;
            auto& target = next->assets[asset];
            if (!parseSceneAsset48D(*input.assets[asset].sceneBytes, target.scene, d.parserMessage))
                return reject(d, StaticDrawAssetError50DW::scene);
            if (!parseMesh48B(target.scene.neutral_lod0.data(), target.scene.neutral_lod0.size(),
                target.mesh, d.parserMessage))
                return reject(d, StaticDrawAssetError50DW::compatibilityMesh);
            const auto& c = counts[asset];
            if (target.scene.nodes.size() != 1 || target.scene.nodes[0].parent != -1 ||
                target.scene.identity_matrix_palette || target.scene.world_space != c.worldSpace ||
                target.scene.vertices_all_lods != c.vertices || target.scene.surfaces_all_lods != c.surfaces ||
                target.scene.lods.empty() || target.scene.lods[0].surfaces.size() != c.lod0Spans ||
                target.scene.spans_lod0.size() != c.lod0Spans ||
                target.mesh.vertices.size() != c.lod0Vertices || target.mesh.indices.size() != c.lod0Indices)
                return reject(d, StaticDrawAssetError50DW::scene);
        }
        next->instances.assign(input.instances, input.instances + input.instanceCount);
        next->vertices.resize(vertices);
        next->indices.resize(indices);
        next->spans.resize(spans);
        next->spanSources.resize(spans);
        std::uint32_t vertexAt = 0, indexAt = 0, spanAt = 0;
        Bounds48B bounds{};
        for (std::uint32_t instance = 0; instance < input.instanceCount; ++instance) {
            d.instance = instance;
            const auto& placement = next->instances[instance];
            d.asset = placement.asset;
            const auto& asset = next->assets[placement.asset];
            const auto& surfaces = asset.scene.lods[0].surfaces;
            for (std::uint32_t surface = 0; surface < surfaces.size(); ++surface) {
                d.surface = surface;
                const auto& source = surfaces[surface];
                const auto& originalSpan = asset.scene.spans_lod0[surface];
                SceneSpan48D span = originalSpan;
                span.vertex_start = vertexAt;
                span.index_start = indexAt;
                for (std::uint32_t vertex = 0; vertex < span.vertex_count; ++vertex) {
                    d.vertex = vertex;
                    auto& target = next->vertices[vertexAt + vertex];
                    std::memcpy(&target, &asset.mesh.vertices[originalSpan.vertex_start + vertex], sizeof(target));
                    if (!project(source.vertices[vertex], placement.matrix, target, d)) return false;
                    for (std::uint32_t axis = 0; axis < 3; ++axis) {
                        const auto value = target.position[axis];
                        if (vertexAt + vertex == 0 || value < bounds.minimum[axis]) bounds.minimum[axis] = value;
                        if (vertexAt + vertex == 0 || value > bounds.maximum[axis]) bounds.maximum[axis] = value;
                    }
                }
                for (std::uint32_t index = 0; index < span.index_count; ++index)
                    next->indices[indexAt + index] = vertexAt + source.indices[index];
                next->spans[spanAt] = span;
                next->spanSources[spanAt] = {placement.asset, instance, surface};
                vertexAt += span.vertex_count;
                indexAt += span.index_count;
                ++spanAt;
            }
        }
        next->view = {next->vertices.data(), vertices, next->indices.data(), indices,
            next->spans.data(), next->spanSources.data(), spans, bounds};
        storage_.swap(next);
        d.asset = d.instance = d.surface = d.vertex = d.lane = 0;
        return true;
    } catch (const std::bad_alloc&) {
        return reject(d, StaticDrawAssetError50DW::allocation);
    } catch (const std::length_error&) {
        return reject(d, StaticDrawAssetError50DW::allocation);
    } catch (...) {
        return reject(d, StaticDrawAssetError50DW::unexpectedFailure);
    }
}

bool StaticDrawAssets50DW::loaded() const noexcept { return bool(storage_); }
const StaticDrawView50DW* StaticDrawAssets50DW::draw() const noexcept {
    return storage_ ? &storage_->view : nullptr;
}
const Scene48D* StaticDrawAssets50DW::scene(std::uint32_t asset) const noexcept {
    return storage_ && asset < storage_->assets.size() ? &storage_->assets[asset].scene : nullptr;
}
const Mesh48B* StaticDrawAssets50DW::compatibilityMesh(std::uint32_t asset) const noexcept {
    return storage_ && asset < storage_->assets.size() ? &storage_->assets[asset].mesh : nullptr;
}
const StaticDrawInstance50DW* StaticDrawAssets50DW::instance(std::uint32_t instance) const noexcept {
    return storage_ && instance < storage_->instances.size() ? &storage_->instances[instance] : nullptr;
}
std::uint32_t StaticDrawAssets50DW::assetCount() const noexcept {
    return storage_ ? static_cast<std::uint32_t>(storage_->assets.size()) : 0;
}
std::uint32_t StaticDrawAssets50DW::instanceCount() const noexcept {
    return storage_ ? static_cast<std::uint32_t>(storage_->instances.size()) : 0;
}
}

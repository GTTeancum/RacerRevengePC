#include "import_service48f.h"
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace rr { namespace windows { namespace {
using Clock = std::chrono::steady_clock;
constexpr std::size_t texture_limit = 256u * 1024u * 1024u;
void demand(bool ok, const std::string& message) { if (!ok) throw std::runtime_error(message); }
double milliseconds(Clock::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}
std::string normalized(std::string text) {
    for (auto& c : text) {
        if (c == '/') c = '\\';
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
    }
    return text;
}
bool sameTexture(const RuntimeTexture48E& a, const RuntimeTexture48E& b) {
    if (a.levels.size() != b.levels.size()) return false;
    for (std::size_t i = 0; i < a.levels.size(); ++i)
        if (a.levels[i].width != b.levels[i].width || a.levels[i].height != b.levels[i].height ||
            a.levels[i].rgba != b.levels[i].rgba) return false;
    return true;
}
bool sameMaterial(const RuntimeMaterial48E& a, const RuntimeMaterial48E& b) {
    if (a.passes.size() != b.passes.size()) return false;
    for (std::size_t p = 0; p < a.passes.size(); ++p) {
        const auto& x = a.passes[p]; const auto& y = b.passes[p];
        if (x.program != y.program || x.textures.size() != y.textures.size()) return false;
        for (std::size_t t = 0; t < x.textures.size(); ++t)
            if (x.textures[t].slot != y.textures[t].slot || x.textures[t].texture != y.textures[t].texture ||
                normalized(x.textures[t].name) != normalized(y.textures[t].name)) return false;
    }
    return true;
}
float finiteFloat(double value) {
    demand(std::isfinite(value) && std::abs(value) <= std::numeric_limits<float>::max(),
        "World placement exceeds finite native float range");
    return static_cast<float>(value);
}
bool nonidentity(const std::array<double, 16>& matrix) {
    for (std::size_t i = 0; i < matrix.size(); ++i)
        if (matrix[i] != (i % 5 == 0 ? 1.0 : 0.0)) return true;
    return false;
}
std::string placedMeshHash(const assets::Mesh48B& mesh) {
    // Hash the explicit RRMESH01 byte contract for independent comparison with
    // the frozen offline placement evidence; this does not write a mesh file.
    std::vector<std::uint8_t> bytes{'R','R','M','E','S','H','0','1'};
    bytes.reserve(16 + mesh.vertices.size() * 24 + mesh.indices.size() * 4);
    auto integer = [&](std::uint32_t n) { for (unsigned c = 0; c < 4; ++c) { bytes.push_back(static_cast<std::uint8_t>(n)); n >>= 8; } };
    auto number = [&](float value) {
        std::uint32_t bits; static_assert(sizeof(bits) == sizeof(value), "binary32 required");
        std::memcpy(&bits, &value, sizeof(bits)); integer(bits);
    };
    integer(static_cast<std::uint32_t>(mesh.vertices.size())); integer(static_cast<std::uint32_t>(mesh.indices.size()));
    for (const auto& vertex : mesh.vertices) {
        for (auto value : vertex.position) number(value);
        for (auto value : vertex.uv) number(value);
        bytes.insert(bytes.end(), std::begin(vertex.color), std::end(vertex.color));
    }
    for (auto index : mesh.indices) integer(index);
    struct Handles {
        BCRYPT_ALG_HANDLE algorithm = nullptr; BCRYPT_HASH_HANDLE hash = nullptr;
        std::vector<std::uint8_t> object;
        ~Handles() { if (hash) BCryptDestroyHash(hash); if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0); }
    } handles;
    demand(BCryptOpenAlgorithmProvider(&handles.algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) >= 0,
        "Cannot open world evidence SHA256");
    DWORD objectSize = 0, returned = 0;
    demand(BCryptGetProperty(handles.algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectSize),
        sizeof(objectSize), &returned, 0) >= 0, "Cannot query world evidence SHA256");
    handles.object.resize(objectSize);
    demand(BCryptCreateHash(handles.algorithm, &handles.hash, handles.object.data(), objectSize, nullptr, 0, 0) >= 0,
        "Cannot initialize world evidence SHA256");
    std::array<std::uint8_t, 32> digest{};
    demand(BCryptHashData(handles.hash, bytes.data(), static_cast<ULONG>(bytes.size()), 0) >= 0 &&
        BCryptFinishHash(handles.hash, digest.data(), static_cast<ULONG>(digest.size()), 0) >= 0,
        "Cannot finish world evidence SHA256");
    std::ostringstream text; text << std::hex << std::setfill('0');
    for (auto byte : digest) text << std::setw(2) << static_cast<unsigned>(byte);
    return text.str();
}
std::size_t scenePayload(const assets::Scene48D& scene) {
    std::size_t bytes = scene.neutral_lod0.size() + scene.nodes.size() * sizeof(assets::SceneNode48D)
        + scene.spans_lod0.size() * sizeof(assets::SceneSpan48D);
    for (const auto& node : scene.nodes) bytes += node.name.size();
    for (const auto& material : scene.materials) bytes += material.size();
    for (const auto& lod : scene.lods) for (const auto& surface : lod.surfaces)
        bytes += sizeof(surface) + surface.vertices.size() * sizeof(assets::SceneVertex48D)
            + surface.indices.size() * sizeof(std::uint32_t)
            + surface.strips.size() * sizeof(assets::SceneStrip48D)
            + surface.continuations.size() * sizeof(assets::SceneContinuation48D);
    return bytes;
}
struct CacheReadLock {
    HANDLE value = INVALID_HANDLE_VALUE;
    explicit CacheReadLock(const std::filesystem::path& cache) {
        // All cache writers take the same lock with share=0. Keep both world
        // and geometry selection on one generation without writing any file.
        value = CreateFileW((cache / "import.lock").c_str(), GENERIC_READ, FILE_SHARE_READ,
            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        demand(value != INVALID_HANDLE_VALUE, "Cannot lock cache for world inspection; an import may be active");
    }
    ~CacheReadLock() { if (value != INVALID_HANDLE_VALUE) CloseHandle(value); }
    CacheReadLock(const CacheReadLock&) = delete;
    CacheReadLock& operator=(const CacheReadLock&) = delete;
};
} // namespace

bool assembleCachedWorld48F(const std::filesystem::path& cache, const std::string& container,
    const std::string& member, RuntimeWorld48F& output, std::string& error) {
    try {
        const auto loadStart = Clock::now();
        CacheReadLock cacheLock(cache);
        assets::World48F world; std::vector<bool> collisionOnly;
        demand(loadCachedWorld48F(cache, container, member, world, collisionOnly, error), error);
        demand(collisionOnly.size() == world.instances.size(), "World classification count mismatch");
        RuntimeWorld48F assembled;
        demand(assets::buildWorldIndex49B(world, assembled.world_index, error), error);
        assembled.source_instances = static_cast<std::uint32_t>(world.instances.size());
        assembled.zones = static_cast<std::uint32_t>(world.zones.size());
        assembled.portals = static_cast<std::uint32_t>(world.portals.size());
        std::vector<std::string> names;
        std::map<std::string, std::size_t> geometry;
        for (std::size_t i = 0; i < world.instances.size(); ++i) {
            if (collisionOnly[i]) { ++assembled.collision_only_instances; continue; }
            ++assembled.visual_instances;
            const auto key = normalized(world.instances[i].geometry_member);
            if (geometry.emplace(key, names.size()).second) names.push_back(world.instances[i].geometry_member);
        }
        demand(!names.empty(), "World contains no supported visual geometry");
        demand(names.size() <= 4096, "World unique geometry limit");
        std::vector<RuntimeScene48E> sources;
        demand(loadCachedScenes48F(cache, container, names, sources, error), error);
        demand(sources.size() == names.size(), "World geometry selection count mismatch");
        assembled.unique_geometry = static_cast<std::uint32_t>(names.size());
        assembled.cache_load_ms = milliseconds(loadStart);
        const auto assemblyStart = Clock::now();
        std::map<std::string, int> textureIndices;
        std::map<std::string, std::uint32_t> materialIndices;
        std::vector<std::vector<std::uint32_t>> materialMaps(sources.size());
        std::set<std::string> warnings;
        for (std::size_t n = 0; n < sources.size(); ++n) {
            auto& source = sources[n]; const auto& scene = source.scene;
            demand(scene.nodes.size() == 1 && scene.nodes[0].parent == -1 && !scene.lods.empty(),
                "World static hierarchy is outside recovered single-root subset: " + names[n]);
            demand(source.materials.size() == scene.materials.size(), "World material binding count mismatch");
            assembled.semantic_payload_bytes += scenePayload(scene);
            demand(assembled.semantic_payload_bytes <= 512u * 1024u * 1024u, "World semantic payload limit");
            for (const auto& surface : scene.lods[0].surfaces) {
                demand(surface.vertices.size() <= assets::max_mesh_vertices - assembled.unique_vertices &&
                    surface.indices.size() <= assets::max_mesh_indices - assembled.unique_indices,
                    "World unique geometry payload limit");
                assembled.unique_vertices += static_cast<std::uint32_t>(surface.vertices.size());
                assembled.unique_indices += static_cast<std::uint32_t>(surface.indices.size());
            }
            std::vector<int> textureMap;
            for (auto& texture : source.textures) {
                const auto key = normalized(texture.source_name);
                auto found = textureIndices.find(key);
                if (found != textureIndices.end()) {
                    demand(sameTexture(assembled.textures.at(found->second), texture), "Conflicting world texture identity: " + key);
                    textureMap.push_back(found->second);
                } else {
                    demand(!texture.levels.empty(), "World texture has no native mip levels");
                    for (const auto& mip : texture.levels) {
                        demand(mip.rgba.size() <= texture_limit - assembled.texture_payload_bytes,
                            "World texture payload exceeds 256 MiB inspection limit");
                        assembled.texture_payload_bytes += mip.rgba.size();
                    }
                    const int index = static_cast<int>(assembled.textures.size());
                    textureIndices.emplace(key, index); textureMap.push_back(index);
                    assembled.textures.push_back(std::move(texture));
                }
            }
            for (auto& material : source.materials) {
                for (auto& pass : material.passes) for (auto& slot : pass.textures) {
                    demand(slot.texture >= -1 && (slot.texture < 0 || static_cast<std::size_t>(slot.texture) < textureMap.size()),
                        "Invalid world texture slot binding");
                    if (slot.texture >= 0) slot.texture = textureMap[slot.texture];
                }
                const auto key = normalized(material.name); auto found = materialIndices.find(key);
                if (found != materialIndices.end()) {
                    demand(sameMaterial(assembled.materials.at(found->second), material), "Conflicting world material identity: " + key);
                    materialMaps[n].push_back(found->second);
                } else {
                    const auto index = static_cast<std::uint32_t>(assembled.materials.size());
                    materialIndices.emplace(key, index); materialMaps[n].push_back(index);
                    assembled.materials.push_back(std::move(material));
                }
            }
            warnings.insert(source.warnings.begin(), source.warnings.end());
            // Duplicate texture payloads no longer need to survive assembly.
            source.textures.clear(); source.materials.clear(); source.warnings.clear();
        }
        std::size_t totalVertices = 0, totalIndices = 0;
        for (std::size_t i = 0; i < world.instances.size(); ++i) if (!collisionOnly[i]) {
            const auto& instance = world.instances[i];
            const auto& scene = sources[geometry.at(normalized(instance.geometry_member))].scene;
            if (scene.world_space)
                demand(instance.position == std::array<double, 3>{0, 0, 0} &&
                    instance.look == std::array<double, 3>{0, 0, 1} && instance.up == std::array<double, 3>{0, 1, 0},
                    "World-quantized instance pose is outside recovered subset");
            for (auto value : instance.matrix) demand(std::isfinite(value), "Nonfinite world placement matrix");
            if (nonidentity(scene.nodes[0].local_matrix)) ++assembled.replaced_root_matrices;
            for (const auto& surface : scene.lods[0].surfaces) {
                demand(surface.vertices.size() <= assets::max_mesh_vertices - totalVertices &&
                    surface.indices.size() <= assets::max_mesh_indices - totalIndices, "Placed world geometry limit");
                totalVertices += surface.vertices.size(); totalIndices += surface.indices.size();
            }
        }
        demand(totalVertices && totalIndices, "World contains no drawable triangles");
        assembled.mesh.vertices.reserve(totalVertices); assembled.mesh.indices.reserve(totalIndices);
        for (std::size_t i = 0; i < world.instances.size(); ++i) if (!collisionOnly[i]) {
            const auto& instance = world.instances[i];
            const auto sceneIndex = geometry.at(normalized(instance.geometry_member));
            const auto& scene = sources[sceneIndex].scene;
            for (const auto& surface : scene.lods[0].surfaces) {
                assets::SceneSpan48D span;
                span.material_index = materialMaps[sceneIndex].at(surface.material_index);
                span.vertex_start = static_cast<std::uint32_t>(assembled.mesh.vertices.size());
                span.index_start = static_cast<std::uint32_t>(assembled.mesh.indices.size());
                span.vertex_count = static_cast<std::uint32_t>(surface.vertices.size());
                span.index_count = static_cast<std::uint32_t>(surface.indices.size());
                span.has_original_colors = surface.has_original_colors;
                for (const auto& vertex : surface.vertices) {
                    demand(vertex.bone == 0, "World static vertex references an unproven bone");
                    assets::Vertex48B placed{};
                    for (unsigned c = 0; c < 3; ++c) {
                        // Ordered binary64 math, then one float32 conversion;
                        // source PSG root bind is intentionally REPLACED.
                        double value = 0;
                        for (unsigned k = 0; k < 3; ++k) value += vertex.position_local[k] * instance.matrix[k * 4 + c];
                        placed.position[c] = finiteFloat(value + instance.matrix[12 + c]);
                    }
                    for (unsigned c = 0; c < 2; ++c) placed.uv[c] = finiteFloat(vertex.uv[c]);
                    for (unsigned c = 0; c < 4; ++c) placed.color[c] = surface.has_original_colors ? vertex.color[c] : 255;
                    for (unsigned c = 0; c < 3; ++c) {
                        if (assembled.mesh.vertices.empty()) assembled.mesh.bounds.minimum[c] = assembled.mesh.bounds.maximum[c] = placed.position[c];
                        else {
                            assembled.mesh.bounds.minimum[c] = std::min(assembled.mesh.bounds.minimum[c], placed.position[c]);
                            assembled.mesh.bounds.maximum[c] = std::max(assembled.mesh.bounds.maximum[c], placed.position[c]);
                        }
                    }
                    assembled.mesh.vertices.push_back(placed);
                }
                for (auto index : surface.indices) {
                    demand(index < span.vertex_count, "Invalid world surface index");
                    assembled.mesh.indices.push_back(span.vertex_start + index);
                }
                assembled.spans.push_back(span);
            }
        }
        assembled.placed_buffer_bytes = assembled.mesh.vertices.size() * sizeof(assets::Vertex48B)
            + assembled.mesh.indices.size() * sizeof(std::uint32_t);
        assembled.placed_mesh_sha256 = placedMeshHash(assembled.mesh);
        warnings.insert("All original static instances drawn together; portal and exclusion visibility is not executed");
        warnings.insert("Original collision-only instances retained in counts; collision meshes and moving actors are not drawn");
        assembled.warnings.assign(warnings.begin(), warnings.end());
        assembled.assembly_ms = milliseconds(assemblyStart);
        output = std::move(assembled); error.clear(); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
} }

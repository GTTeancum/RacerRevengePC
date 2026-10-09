#include "scene_asset48d.h"
#include "native_asset48b.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace rr { namespace assets { namespace {
void demand(bool ok, const char* text) { if (!ok) throw std::runtime_error(text); }
struct Writer {
    std::vector<std::uint8_t> data;
    void integer(std::uint64_t n, unsigned bytes = 4) {
        demand(data.size() <= max_scene_bytes48d - bytes, "Scene size limit");
        for (unsigned i = 0; i < bytes; ++i) { data.push_back(static_cast<std::uint8_t>(n)); n >>= 8; }
    }
    void number(double n) {
        demand(std::isfinite(n), "Nonfinite scene value");
        std::uint64_t bits; static_assert(sizeof(bits) == sizeof(n), "binary64 required");
        std::memcpy(&bits, &n, sizeof(bits)); integer(bits, 8);
    }
    template<std::size_t N> void numbers(const std::array<double, N>& a) { for (auto n : a) number(n); }
    void string(const std::string& s) {
        demand(!s.empty() && s.size() <= 128 && s.find('\0') == std::string::npos, "Invalid scene name");
        integer(s.size()); for (unsigned char c : s) integer(c, 1);
    }
};
struct Reader {
    const std::vector<std::uint8_t>& data; std::size_t at = 0;
    std::uint64_t integer(unsigned bytes = 4) {
        demand(at <= data.size() && bytes <= data.size() - at, "Truncated scene");
        std::uint64_t n = 0;
        for (unsigned i = 0; i < bytes; ++i) n |= std::uint64_t(data[at++]) << (8 * i);
        return n;
    }
    std::uint32_t count(std::uint32_t limit, std::size_t minBytes = 1) {
        auto n = static_cast<std::uint32_t>(integer());
        demand(n <= limit && n <= (data.size() - at) / minBytes, "Scene count exceeds bounds"); return n;
    }
    bool flag() { auto n = integer(); demand(n <= 1, "Invalid scene boolean"); return n != 0; }
    double number() {
        const auto bits = integer(8); double n; std::memcpy(&n, &bits, sizeof(n));
        demand(std::isfinite(n), "Nonfinite scene value"); return n;
    }
    template<std::size_t N> void numbers(std::array<double, N>& a) { for (auto& n : a) n = number(); }
    std::string string() {
        const auto n = count(128); demand(n != 0, "Empty scene name");
        std::string s(reinterpret_cast<const char*>(data.data() + at), n); at += n;
        demand(s.find('\0') == std::string::npos, "Invalid scene name"); return s;
    }
};
void validate(const Scene48D& s) {
    demand(!s.nodes.empty() && s.nodes.size() <= 256, "Invalid scene node count");
    demand(!s.materials.empty() && s.materials.size() <= 4096, "Invalid scene material count");
    demand(s.lods.size() == 1 || s.lods.size() == 4, "Invalid scene LOD count");
    for (unsigned i = 0; i < 4; ++i) demand(s.quantization[i] > 0, "Invalid scene quantization");
    demand(s.world_space || (s.quantization[4] == 0 && s.quantization[5] == 0 && s.quantization[6] == 0), "Invalid scene local origin");
    for (std::size_t i = 0; i < s.nodes.size(); ++i) {
        const auto& n = s.nodes[i];
        demand(n.parent >= -1 && n.parent < static_cast<std::int32_t>(i), "Invalid scene hierarchy");
        for (const auto* m : {&n.local_matrix, &n.bind_matrix})
            demand((*m)[3] == 0 && (*m)[7] == 0 && (*m)[11] == 0 && (*m)[15] == 1, "Nonaffine scene matrix");
        auto expected = n.local_matrix;
        if (n.parent >= 0) {
            const auto& parent = s.nodes[n.parent].bind_matrix;
            for (unsigned row = 0; row < 4; ++row) for (unsigned col = 0; col < 4; ++col) {
                double sum = 0;
                for (unsigned k = 0; k < 4; ++k) sum += n.local_matrix[row * 4 + k] * parent[k * 4 + col];
                expected[row * 4 + col] = sum;
            }
        }
        demand(expected == n.bind_matrix, "Scene hierarchy bind mismatch");
        for (unsigned k = 3; k < 7; ++k) demand(n.stored_bounds[k] >= 0, "Negative scene bound");
    }
    std::uint64_t vertices = 0, surfaces = 0, indices = 0;
    std::vector<std::array<double, 6>> nodeBounds(s.nodes.size());
    std::vector<bool> nodeUsed(s.nodes.size());
    std::uint32_t v0 = 0, i0 = 0;
    for (std::size_t l = 0; l < s.lods.size(); ++l) {
        const auto& lod = s.lods[l]; surfaces += lod.surfaces.size();
        demand(lod.cutoff > 0, "Invalid scene LOD cutoff");
        demand(!lod.surfaces.empty() && surfaces <= 16384, "Invalid scene surface count");
        if (l == 0) demand(s.spans_lod0.size() == lod.surfaces.size(), "Scene span count mismatch");
        for (std::size_t j = 0; j < lod.surfaces.size(); ++j) {
            const auto& surf = lod.surfaces[j];
            demand(surf.uv_size_uninterpreted > 0, "Invalid scene UV metadata");
            vertices += surf.vertices.size(); indices += surf.indices.size();
            demand(vertices <= 1000000 && indices <= 3000000, "Scene geometry limit");
            demand(surf.material_index < s.materials.size() && !surf.vertices.empty() &&
                !surf.indices.empty() && surf.indices.size() % 3 == 0, "Invalid scene surface");
            for (const auto& v : surf.vertices) {
                demand(v.bone < s.nodes.size(), "Scene bone out of range");
                auto& bounds = nodeBounds[v.bone];
                for (unsigned c = 0; c < 3; ++c) {
                    demand(std::isfinite(v.position_local[c]), "Nonfinite scene position");
                    bounds[c] = nodeUsed[v.bone] ? std::min(bounds[c], v.position_local[c]) : v.position_local[c];
                    bounds[c + 3] = nodeUsed[v.bone] ? std::max(bounds[c + 3], v.position_local[c]) : v.position_local[c];
                }
                nodeUsed[v.bone] = true;
            }
            for (auto ix : surf.indices) demand(ix < surf.vertices.size(), "Scene index out of range");
            if (l == 0) {
                const auto& span = s.spans_lod0[j];
                demand(span.material_index == surf.material_index && span.has_original_colors == surf.has_original_colors &&
                    span.vertex_start == v0 && span.index_start == i0 && span.vertex_count == surf.vertices.size() &&
                    span.index_count == surf.indices.size(), "Scene compatibility span mismatch");
                v0 += span.vertex_count; i0 += span.index_count;
            }
        }
    }
    demand(vertices == s.vertices_all_lods && surfaces == s.surfaces_all_lods, "Scene totals mismatch");
    for (std::size_t n = 0; n < s.nodes.size(); ++n) if (nodeUsed[n]) {
        const auto& stored = s.nodes[n].stored_bounds;
        for (unsigned c = 0; c < 3; ++c) {
            const double limit = s.quantization[0] * 1.01 + 0.002;
            demand(std::abs(nodeBounds[n][c] - (stored[c] - stored[c + 3])) <= limit &&
                std::abs(nodeBounds[n][c + 3] - (stored[c] + stored[c + 3])) <= limit, "Scene node bounds mismatch");
        }
    }
    Mesh48B mesh; std::string error;
    demand(parseMesh48B(s.neutral_lod0.data(), s.neutral_lod0.size(), mesh, error), "Invalid scene compatibility mesh");
    demand(mesh.vertices.size() == v0 && mesh.indices.size() == i0, "Scene compatibility geometry mismatch");
    // The legacy inspection mesh must describe this scene, not merely have matching counts.
    for (std::size_t j = 0; j < s.spans_lod0.size(); ++j) {
        const auto& surf = s.lods[0].surfaces[j]; const auto& span = s.spans_lod0[j];
        for (std::size_t k = 0; k < surf.indices.size(); ++k)
            demand(mesh.indices[span.index_start + k] == span.vertex_start + surf.indices[k], "Scene compatibility index mismatch");
        for (std::size_t k = 0; k < surf.vertices.size(); ++k) {
            const auto& v = surf.vertices[k]; const auto& mv = mesh.vertices[span.vertex_start + k];
            const auto& m = s.nodes[v.bone].bind_matrix;
            for (unsigned c = 0; c < 3; ++c) {
                const double p = v.position_local[0] * m[c] + v.position_local[1] * m[4 + c] +
                    v.position_local[2] * m[8 + c] + m[12 + c];
                demand(std::isfinite(p) && mv.position[c] == static_cast<float>(p), "Scene compatibility position mismatch");
            }
            for (unsigned c = 0; c < 2; ++c) demand(mv.uv[c] == static_cast<float>(v.uv[c]), "Scene compatibility UV mismatch");
            for (unsigned c = 0; c < 4; ++c)
                demand(mv.color[c] == (surf.has_original_colors ? v.color[c] : 255), "Scene compatibility color mismatch");
        }
    }
}
} // namespace
bool encodeSceneAsset48D(const Scene48D& s, std::vector<std::uint8_t>& output, std::string& error) {
    try {
        validate(s); Writer w;
        for (unsigned char c : std::string("RRSCENE1")) w.integer(c, 1);
        w.integer(1); w.integer(s.world_space); w.integer(s.identity_matrix_palette);
        w.integer(s.nodes.size()); w.integer(s.materials.size()); w.integer(s.lods.size());
        w.integer(s.vertices_all_lods); w.integer(s.surfaces_all_lods); w.numbers(s.quantization);
        for (const auto& n : s.nodes) {
            w.string(n.name); w.integer(static_cast<std::uint32_t>(n.parent));
            w.numbers(n.local_matrix); w.numbers(n.bind_matrix); w.numbers(n.stored_bounds);
        }
        for (const auto& m : s.materials) w.string(m);
        for (const auto& lod : s.lods) {
            w.number(lod.cutoff); w.integer(lod.surfaces.size());
            for (const auto& surf : lod.surfaces) {
                w.integer(surf.material_index); w.integer(surf.has_original_colors);
                w.number(surf.uv_size_uninterpreted); w.integer(surf.vertices.size()); w.integer(surf.indices.size());
                for (const auto& v : surf.vertices) {
                    w.numbers(v.position_local); w.numbers(v.normal_local); w.numbers(v.uv);
                    for (auto c : v.color) w.integer(c, 1); w.integer(v.bone, 2);
                }
                for (auto ix : surf.indices) w.integer(ix);
            }
        }
        w.integer(s.spans_lod0.size());
        for (const auto& span : s.spans_lod0) {
            w.integer(span.material_index); w.integer(span.vertex_start); w.integer(span.vertex_count);
            w.integer(span.index_start); w.integer(span.index_count); w.integer(span.has_original_colors);
        }
        w.integer(s.neutral_lod0.size());
        demand(s.neutral_lod0.size() <= max_scene_bytes48d - w.data.size(), "Scene size limit");
        w.data.insert(w.data.end(), s.neutral_lod0.begin(), s.neutral_lod0.end());
        output = std::move(w.data); error.clear(); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
bool parseSceneAsset48D(const std::vector<std::uint8_t>& bytes, Scene48D& output, std::string& error) {
    try {
        demand(bytes.size() <= max_scene_bytes48d && bytes.size() >= 96, "Invalid scene size");
        demand(std::memcmp(bytes.data(), "RRSCENE1", 8) == 0, "Invalid scene magic");
        Reader r{bytes, 8}; demand(r.integer() == 1, "Unsupported scene version"); Scene48D s;
        s.world_space = r.flag(); s.identity_matrix_palette = r.flag();
        const auto nodes = r.count(256, 316), materials = r.count(4096), lods = r.count(4, 12);
        s.vertices_all_lods = r.count(1000000); s.surfaces_all_lods = r.count(16384);
        r.numbers(s.quantization); s.nodes.resize(nodes);
        for (auto& n : s.nodes) {
            n.name = r.string(); const auto parent = static_cast<std::uint32_t>(r.integer());
            demand(parent == 0xffffffffu || parent < nodes, "Invalid parent number");
            n.parent = parent == 0xffffffffu ? -1 : static_cast<std::int32_t>(parent);
            r.numbers(n.local_matrix); r.numbers(n.bind_matrix); r.numbers(n.stored_bounds);
        }
        for (unsigned i = 0; i < materials; ++i) s.materials.push_back(r.string());
        s.lods.resize(lods); std::uint64_t totalV = 0, totalI = 0, totalS = 0;
        for (auto& lod : s.lods) {
            lod.cutoff = r.number(); const auto count = r.count(16384, 24);
            totalS += count; demand(totalS <= 16384, "Scene surface limit"); lod.surfaces.resize(count);
            for (auto& surf : lod.surfaces) {
                surf.material_index = static_cast<std::uint32_t>(r.integer()); surf.has_original_colors = r.flag();
                surf.uv_size_uninterpreted = r.number(); const auto vc = r.count(1000000, 70), ic = r.count(3000000, 4);
                totalV += vc; totalI += ic; demand(totalV <= 1000000 && totalI <= 3000000, "Scene geometry limit");
                surf.vertices.resize(vc); surf.indices.resize(ic);
                for (auto& v : surf.vertices) {
                    r.numbers(v.position_local); r.numbers(v.normal_local); r.numbers(v.uv);
                    for (auto& c : v.color) c = static_cast<std::uint8_t>(r.integer(1));
                    v.bone = static_cast<std::uint16_t>(r.integer(2));
                }
                for (auto& ix : surf.indices) ix = static_cast<std::uint32_t>(r.integer());
            }
        }
        s.spans_lod0.resize(r.count(16384, 24));
        for (auto& span : s.spans_lod0) {
            span.material_index = static_cast<std::uint32_t>(r.integer()); span.vertex_start = static_cast<std::uint32_t>(r.integer());
            span.vertex_count = static_cast<std::uint32_t>(r.integer()); span.index_start = static_cast<std::uint32_t>(r.integer());
            span.index_count = static_cast<std::uint32_t>(r.integer()); span.has_original_colors = r.flag();
        }
        const auto meshBytes = r.count(static_cast<std::uint32_t>(max_asset_file_bytes));
        demand(meshBytes == bytes.size() - r.at, "Trailing or truncated scene data");
        s.neutral_lod0.assign(bytes.begin() + r.at, bytes.end()); validate(s);
        output = std::move(s); error.clear(); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
} }

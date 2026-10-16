#include "import_scene48d.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <utility>

namespace rr { namespace assets {
namespace {
struct Failure48D { DecodeError48C error; };
void require(bool value, DecodeKind48C kind, const char* code, std::size_t offset, const char* detail) {
    if (!value) throw Failure48D{{kind, code, offset, detail}};
}
struct Read48D {
    const std::vector<std::uint8_t>& data;
    void range(std::size_t at, std::size_t size) const {
        require(at <= data.size() && size <= data.size() - at, DecodeKind48C::malformed,
            "scene.truncated", at, "Read exceeds PSG extent");
    }
    std::uint32_t u32(std::size_t at) const {
        range(at, 4);
        return std::uint32_t(data[at]) | (std::uint32_t(data[at + 1]) << 8) |
            (std::uint32_t(data[at + 2]) << 16) | (std::uint32_t(data[at + 3]) << 24);
    }
    std::uint16_t u16(std::size_t at) const {
        range(at, 2); return std::uint16_t(unsigned(data[at]) | (unsigned(data[at + 1]) << 8));
    }
    int i16(std::size_t at) const { const unsigned x = u16(at); return x < 32768 ? int(x) : int(x) - 65536; }
    int i32(std::size_t at) const {
        const auto bits = u32(at);
        return static_cast<int>(bits < 0x80000000u ? std::int64_t(bits) : std::int64_t(bits) - 0x100000000LL);
    }
    int i8(std::size_t at) const { range(at, 1); return data[at] < 128 ? int(data[at]) : int(data[at]) - 256; }
    double f32(std::size_t at) const {
        static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559, "IEEE binary32 required");
        const auto bits = u32(at); float value; std::memcpy(&value, &bits, 4); return value;
    }
    bool zero(std::size_t at, std::size_t size) const {
        range(at, size); for (std::size_t i = 0; i < size; ++i) if (data[at + i]) return false; return true;
    }
    std::string string(std::size_t at, std::size_t size) const {
        range(at, size); std::size_t length = 0;
        while (length < size && data[at + length]) {
            require(data[at + length] < 128, DecodeKind48C::unsupported, "scene.string_encoding", at + length, "Fixed name is not ASCII");
            ++length;
        }
        require(length < size && zero(at + length, size - length), DecodeKind48C::malformed,
            "scene.string_padding", at, "Fixed name is unterminated or has nonzero suffix");
        return std::string(data.begin() + at, data.begin() + at + length);
    }
};
void cancelled(const DecodeHooks48C* hooks, std::size_t at) {
    require(!hooks || !hooks->cancelled || !hooks->cancelled(hooks->context), DecodeKind48C::cancelled,
        "scene.cancelled", at, "Cancelled before scene output commit");
}
void progress(const DecodeHooks48C* hooks, const char* stage, std::uint32_t done, std::uint32_t total) {
    if (hooks && hooks->progress) hooks->progress(hooks->context, stage, done, total);
}
std::array<double, 16> compose(const std::array<double, 16>& local, const std::array<double, 16>& parent,
    std::size_t at) {
    std::array<double, 16> result{};
    for (unsigned row = 0; row < 4; ++row) for (unsigned column = 0; column < 4; ++column) {
        double sum = 0;
        for (unsigned k = 0; k < 4; ++k) sum += local[row * 4 + k] * parent[k * 4 + column];
        require(std::isfinite(sum), DecodeKind48C::malformed, "scene.bind_nonfinite", at, "Composed bind matrix is nonfinite");
        result[row * 4 + column] = sum;
    }
    return result;
}
void appendU32(std::vector<std::uint8_t>& bytes, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) bytes.push_back(static_cast<std::uint8_t>(value >> (8 * i)));
}
void appendF32(std::vector<std::uint8_t>& bytes, double value) {
    const float stored = static_cast<float>(value);
    require(std::isfinite(stored), DecodeKind48C::unsupported, "scene.native_float_range", 0,
        "Neutral native export exceeds finite binary32");
    std::uint32_t bits; std::memcpy(&bits, &stored, 4); appendU32(bytes, bits);
}
struct RawVertex48D {
    std::array<int, 4> xyz{};
    std::array<int, 3> normal{};
    std::array<int, 2> uv{};
    std::array<std::uint8_t, 4> color{};
};
bool sameEndpoint(const RawVertex48D& a, const RawVertex48D& b) {
    return a.xyz[0] == b.xyz[0] && a.xyz[1] == b.xyz[1] && a.xyz[2] == b.xyz[2] &&
        std::abs(a.xyz[3]) == std::abs(b.xyz[3]) && a.normal == b.normal && a.uv == b.uv && a.color == b.color;
}
void accumulateBounds(SceneNode48D& node, const std::array<double, 3>& position) {
    for (unsigned axis = 0; axis < 3; ++axis) {
        if (!node.decoded_points || position[axis] < node.decoded_min[axis]) node.decoded_min[axis] = position[axis];
        if (!node.decoded_points || position[axis] > node.decoded_max[axis]) node.decoded_max[axis] = position[axis];
    }
    ++node.decoded_points;
}
void decodeSurface(const Read48D& read, std::size_t begin, std::size_t end, std::uint32_t expectedVertices,
    Scene48D& scene, SceneSurface48D& surface, const DecodeHooks48C* hooks,
    bool allowSigned32QuantizedPositions) {
    std::size_t at = begin;
    surface.vertices.reserve(expectedVertices);
    surface.indices.reserve(static_cast<std::size_t>(expectedVertices) * 3);
    std::array<RawVertex48D, 2> previousEndpoints{};
    auto take = [&](std::uint32_t command, std::uint32_t count, std::uint32_t address, std::size_t elementBytes) {
        require(at <= end && end - at >= 4, DecodeKind48C::malformed, "scene.command_extent", at, "Command crosses surface extent");
        const std::uint32_t expected = (command << 24) | (count << 16) | address;
        require(read.u32(at) == expected, DecodeKind48C::unsupported, "scene.attribute_command", at,
            "Unsupported original attribute command shape");
        at += 4;
        const std::size_t bytes = elementBytes * count, padded = (bytes + 3) & ~std::size_t(3);
        require(at <= end && padded <= end - at, DecodeKind48C::malformed, "scene.attribute_extent", at, "Attribute crosses surface extent");
        require(read.zero(at + bytes, padded - bytes), DecodeKind48C::unsupported, "scene.attribute_padding", at + bytes, "Nonzero attribute padding");
        const std::size_t data = at; at += padded; return data;
    };
    while (at < end) {
        cancelled(hooks, at);
        require(end - at >= 4, DecodeKind48C::malformed, "scene.command_truncated", at, "Truncated surface command");
        if (read.u32(at) == 0) break;
        const std::size_t tag = take(0x6c, 1, 0x8000, 16);
        const std::uint32_t count = read.u32(tag) & 0x7fff;
        require(count >= 3 && count <= 16 && read.u32(tag) == (0x8000 | count) &&
            read.u32(tag + 4) == 0x30024000 && read.u32(tag + 8) == 0x412 && read.u32(tag + 12) == 0,
            DecodeKind48C::unsupported, "scene.group_template", tag, "Only observed original groups of 3..16 vertices supported");
        require(surface.vertices.size() + count <= expectedVertices, DecodeKind48C::malformed,
            "scene.vertex_count", tag, "Surface groups exceed declared vertex count");
        const bool widePositions = allowSigned32QuantizedPositions &&
            read.u32(at) == ((0x6cu << 24) | (count << 16) | 0x8001u);
        const std::size_t xyzElementBytes = widePositions ? 16 : 8;
        const auto xyz = take(widePositions ? 0x6c : 0x6d, count, 0x8001, xyzElementBytes);
        const auto normals = take(0x6a, count, 0x8001 + count, 3);
        const auto uv = take(0x65, count, 0x8001 + 2 * count, 4);
        const auto colors = surface.has_original_colors ? take(0x6e, count, 0xc001 + 3 * count, 4) : 0;
        require(at <= end && end - at >= 4 && read.u32(at) == 0x17000000, DecodeKind48C::unsupported,
            "scene.group_end", at, "Missing original surface group end");
        at += 4;
        std::array<RawVertex48D, 16> raw{};
        for (unsigned i = 0; i < count; ++i) {
            for (unsigned axis = 0; axis < 4; ++axis)
                raw[i].xyz[axis] = widePositions ? read.i32(xyz + i * 16 + axis * 4) :
                    read.i16(xyz + i * 8 + axis * 2);
            if (widePositions) for (unsigned axis = 0; axis < 3; ++axis)
                require(raw[i].xyz[axis] >= -8388608 && raw[i].xyz[axis] <= 8388607,
                    DecodeKind48C::unsupported, "scene.signed24_position", xyz + i * 16 + axis * 4,
                    "V4_32 position is outside the authored signed24 quantized subset");
            const auto boneOffset = xyz + i * xyzElementBytes + (widePositions ? 12 : 6);
            // Bound signed bone/ADC before abs, including INT_MIN rejection.
            const int maximumBoneWord = static_cast<int>(scene.nodes.size()) * 16;
            require(raw[i].xyz[3] >= -maximumBoneWord && raw[i].xyz[3] <= maximumBoneWord,
                DecodeKind48C::malformed, "scene.bone_index", boneOffset,
                "Rigid bone/ADC word is outside hierarchy");
            const int boneWord = std::abs(raw[i].xyz[3]);
            require(boneWord != 0 && boneWord % 16 == 0 && boneWord / 16 >= 1 &&
                static_cast<unsigned>(boneWord / 16) <= scene.nodes.size(), DecodeKind48C::malformed,
                "scene.bone_index", boneOffset, "Rigid bone/ADC word is outside hierarchy");
            for (unsigned axis = 0; axis < 3; ++axis) raw[i].normal[axis] = read.i8(normals + i * 3 + axis);
            for (unsigned axis = 0; axis < 2; ++axis) raw[i].uv[axis] = read.i16(uv + i * 4 + axis * 2);
            if (surface.has_original_colors)
                for (unsigned channel = 0; channel < 4; ++channel) raw[i].color[channel] = read.data[colors + i * 4 + channel];
        }
        const std::uint32_t base = static_cast<std::uint32_t>(surface.vertices.size());
        std::uint32_t start = 0;
        while (start < count) {
            require(start + 2 < count && raw[start].xyz[3] < 0 && raw[start + 1].xyz[3] < 0,
                DecodeKind48C::unsupported, "scene.strip_start", xyz + start * xyzElementBytes, "Strip needs two suppressed initial vertices");
            std::uint32_t finish = start + 2;
            while (finish < count && raw[finish].xyz[3] > 0) {
                std::uint32_t a = base + finish - 2, b = base + finish - 1;
                if ((finish - start) & 1) std::swap(a, b);
                surface.indices.push_back(a); surface.indices.push_back(b); surface.indices.push_back(base + finish);
                ++finish;
            }
            require(finish > start + 2, DecodeKind48C::unsupported, "scene.empty_strip", xyz + start * xyzElementBytes, "Source strip emits no triangle");
            if (!surface.strips.empty()) {
                const auto& previous = surface.strips.back();
                if (previous.end - previous.start == 16 && previous.start == 0 &&
                    previous.group + 1 == surface.groups && start == 0 &&
                    sameEndpoint(previousEndpoints[0], raw[start]) && sameEndpoint(previousEndpoints[1], raw[start + 1])) {
                    const auto next = static_cast<std::uint32_t>(surface.strips.size());
                    surface.continuations.push_back({next - 1, next, previous.group, surface.groups});
                }
            }
            surface.strips.push_back({surface.groups, start, finish, base + start});
            previousEndpoints = {{raw[finish - 2], raw[finish - 1]}};
            start = finish;
        }
        for (unsigned i = 0; i < count; ++i) {
            SceneVertex48D vertex;
            vertex.bone = static_cast<std::uint16_t>(std::abs(raw[i].xyz[3]) / 16 - 1);
            for (unsigned axis = 0; axis < 3; ++axis) {
                const double product = raw[i].xyz[axis] * scene.quantization[0];
                vertex.position_local[axis] = product + scene.quantization[4 + axis];
                vertex.normal_local[axis] = raw[i].normal[axis] * scene.quantization[1];
            }
            for (unsigned axis = 0; axis < 2; ++axis) vertex.uv[axis] = raw[i].uv[axis] * scene.quantization[2 + axis];
            vertex.color = raw[i].color;
            accumulateBounds(scene.nodes[vertex.bone], vertex.position_local);
            surface.vertices.push_back(vertex);
        }
        ++surface.groups;
        progress(hooks, "scene_surface_vertices", static_cast<std::uint32_t>(surface.vertices.size()), expectedVertices);
    }
    require(end - at < 16 && read.zero(at, end - at), DecodeKind48C::unsupported,
        "scene.surface_trailer", at, "Unexpected surface trailing bytes");
    require(surface.vertices.size() == expectedVertices, DecodeKind48C::malformed,
        "scene.vertex_header", surface.source_offset, "Surface vertex header count mismatch");
    require(surface.strips.size() >= surface.continuations.size() &&
        surface.strips.size() - surface.continuations.size() == surface.source_strips, DecodeKind48C::unsupported,
        "scene.strip_continuations", surface.source_offset,
        "Source strip count is not explained by exact 16-vertex endpoint continuations");
    require(surface.indices.size() == 3 * (surface.vertices.size() - 2 * surface.strips.size()), DecodeKind48C::malformed,
        "scene.triangle_accounting", surface.source_offset, "Triangle emission does not reconcile");
    for (std::size_t i = 0; i < surface.indices.size(); i += 3) {
        const auto& a = surface.vertices[surface.indices[i]];
        const auto& b = surface.vertices[surface.indices[i + 1]];
        const auto& c = surface.vertices[surface.indices[i + 2]];
        if (a.bone != b.bone || a.bone != c.bone) { ++surface.mixed_bone_unchecked; continue; }
        std::array<double, 3> u{}, v{}, normal{};
        for (unsigned axis = 0; axis < 3; ++axis) {
            u[axis] = b.position_local[axis] - a.position_local[axis];
            v[axis] = c.position_local[axis] - a.position_local[axis];
            normal[axis] = (a.normal_local[axis] + b.normal_local[axis]) + c.normal_local[axis];
        }
        const double dot = ((u[1] * v[2] - u[2] * v[1]) * normal[0] +
            (u[2] * v[0] - u[0] * v[2]) * normal[1]) + (u[0] * v[1] - u[1] * v[0]) * normal[2];
        require(std::isfinite(dot), DecodeKind48C::malformed, "scene.normal_nonfinite", surface.source_offset, "Normal validation overflow");
        if (dot > 0) ++surface.normal_positive;
        else if (dot < 0) ++surface.normal_negative;
        else ++surface.normal_zero;
    }
}
void exportNeutral(Scene48D& scene, const DecodeHooks48C* hooks) {
    const auto& lod = scene.lods[0];
    std::uint32_t vertexCount = 0, indexCount = 0;
    for (const auto& surface : lod.surfaces) {
        vertexCount += static_cast<std::uint32_t>(surface.vertices.size());
        indexCount += static_cast<std::uint32_t>(surface.indices.size());
    }
    scene.neutral_lod0.reserve(16 + std::size_t(vertexCount) * 24 + std::size_t(indexCount) * 4);
    const char magic[] = "RRMESH01";
    scene.neutral_lod0.insert(scene.neutral_lod0.end(), magic, magic + 8);
    appendU32(scene.neutral_lod0, vertexCount); appendU32(scene.neutral_lod0, indexCount);
    std::uint32_t vertexBase = 0, indexBase = 0;
    for (const auto& surface : lod.surfaces) {
        cancelled(hooks, surface.source_offset);
        scene.spans_lod0.push_back({surface.material_index, vertexBase, static_cast<std::uint32_t>(surface.vertices.size()),
            indexBase, static_cast<std::uint32_t>(surface.indices.size()), surface.has_original_colors});
        for (const auto& vertex : surface.vertices) {
            const auto& matrix = scene.nodes[vertex.bone].bind_matrix;
            for (unsigned axis = 0; axis < 3; ++axis) {
                double value = 0;
                for (unsigned k = 0; k < 3; ++k) value += vertex.position_local[k] * matrix[k * 4 + axis];
                appendF32(scene.neutral_lod0, value + matrix[12 + axis]);
            }
            for (double value : vertex.uv) appendF32(scene.neutral_lod0, value);
            if (surface.has_original_colors)
                scene.neutral_lod0.insert(scene.neutral_lod0.end(), vertex.color.begin(), vertex.color.end());
            else scene.neutral_lod0.insert(scene.neutral_lod0.end(), 4, 255);
        }
        vertexBase += static_cast<std::uint32_t>(surface.vertices.size());
        indexBase += static_cast<std::uint32_t>(surface.indices.size());
    }
    vertexBase = 0;
    for (const auto& surface : lod.surfaces) {
        for (std::uint32_t index : surface.indices) appendU32(scene.neutral_lod0, vertexBase + index);
        vertexBase += static_cast<std::uint32_t>(surface.vertices.size());
    }
}
}

bool decodeScene48D(const std::vector<std::uint8_t>& input, Scene48D& output,
    DecodeError48C& error, const DecodeHooks48C* hooks, const SceneLimits48D& limits) {
    error = {};
    try {
        cancelled(hooks, 0);
        require(input.size() <= std::min<std::size_t>(8u * 1024u * 1024u, limits.input_bytes), DecodeKind48C::limit,
            "scene.file_limit", 0, "PSG exceeds input size limit");
        require(input.size() >= 12, DecodeKind48C::malformed, "scene.header_truncated", 0, "PSG header is truncated");
        const Read48D read{input};
        require(read.u32(0) == 0x00677370 && read.u32(4) == 3, DecodeKind48C::unsupported,
            "scene.version", 0, "Only original PSG version3 supported");
        const auto nodeCount = read.u32(8);
        require(nodeCount >= 1 && nodeCount <= 256, DecodeKind48C::limit, "scene.node_count", 8, "Hierarchy count outside 1..256");
        Scene48D parsed;
        parsed.nodes.reserve(nodeCount);
        std::size_t at = 12;
        for (unsigned i = 0; i < nodeCount; ++i) {
            cancelled(hooks, at); read.range(at, 224);
            SceneNode48D node; node.source_offset = at; node.name = read.string(at, 128);
            for (unsigned j = 0; j < 16; ++j) {
                node.local_matrix[j] = read.f32(at + 128 + j * 4);
                require(std::isfinite(node.local_matrix[j]), DecodeKind48C::malformed,
                    "scene.matrix_nonfinite", at + 128 + j * 4, "Hierarchy matrix must be finite");
            }
            require(node.local_matrix[3] == 0 && node.local_matrix[7] == 0 && node.local_matrix[11] == 0 && node.local_matrix[15] == 1,
                DecodeKind48C::unsupported, "scene.matrix_nonaffine", at + 128, "Hierarchy matrix is not affine");
            for (unsigned j = 0; j < 7; ++j) {
                node.stored_bounds[j] = read.f32(at + 192 + j * 4);
                require(std::isfinite(node.stored_bounds[j]) && (j < 3 || node.stored_bounds[j] >= 0),
                    DecodeKind48C::malformed, "scene.hierarchy_bounds", at + 192 + j * 4, "Invalid hierarchy bounds");
            }
            const auto parent = read.u32(at + 220);
            require(parent == 0xffffffffu || parent < i, DecodeKind48C::unsupported,
                "scene.parent_order", at + 220, "Parent must precede child; cyclic/forward hierarchy unsupported");
            node.parent = parent == 0xffffffffu ? -1 : static_cast<std::int32_t>(parent);
            node.bind_matrix = node.parent < 0 ? node.local_matrix : compose(node.local_matrix, parsed.nodes[parent].bind_matrix, at);
            parsed.nodes.push_back(std::move(node)); at += 224;
            progress(hooks, "scene_nodes", i + 1, nodeCount);
        }
        const auto materialCount = read.u32(at); at += 4;
        require(materialCount >= 1 && materialCount <= 4096, DecodeKind48C::limit,
            "scene.material_count", at - 4, "Material count outside 1..4096");
        read.range(at, std::size_t(materialCount) * 64);
        parsed.materials.reserve(materialCount);
        for (unsigned i = 0; i < materialCount; ++i) parsed.materials.push_back(read.string(at + i * 64, 64));
        at += std::size_t(materialCount) * 64;
        for (unsigned i = 0; i < 7; ++i) {
            parsed.quantization[i] = read.f32(at + i * 4);
            require(std::isfinite(parsed.quantization[i]) && (i >= 4 || parsed.quantization[i] > 0),
                DecodeKind48C::malformed, "scene.quantization", at + i * 4, "Invalid finite quantization/origin");
        }
        const auto world = read.u32(at + 28);
        require(world <= 1, DecodeKind48C::unsupported, "scene.world_flag", at + 28, "Unknown world-space flag");
        parsed.world_space = world != 0;
        require(parsed.world_space || (parsed.quantization[4] == 0 && parsed.quantization[5] == 0 && parsed.quantization[6] == 0),
            DecodeKind48C::unsupported, "scene.local_origin", at + 16, "Local geometry has unproven nonzero origin");
        at += 32;
        const auto lodCount = read.u32(at); at += 4;
        require(lodCount == 1 || lodCount == 4, DecodeKind48C::unsupported, "scene.lod_layout", at - 4, "Only observed one/four LOD layouts supported");
        parsed.lods.reserve(lodCount);
        for (unsigned level = 0; level < lodCount; ++level) {
            cancelled(hooks, at);
            SceneLod48D lod; lod.cutoff = read.f32(at);
            const auto surfaceCount = read.u32(at + 4); at += 8;
            require(std::isfinite(lod.cutoff) && lod.cutoff > 0, DecodeKind48C::malformed,
                "scene.lod_cutoff", at - 8, "LOD cutoff must be positive/finite");
            require(surfaceCount >= 1 && surfaceCount <= 4096, DecodeKind48C::limit,
                "scene.surface_count", at - 4, "Surface count outside 1..4096");
            lod.surfaces.reserve(surfaceCount);
            for (unsigned surfaceIndex = 0; surfaceIndex < surfaceCount; ++surfaceIndex) {
                cancelled(hooks, at); read.range(at, 20);
                SceneSurface48D surface; surface.source_offset = at;
                surface.material_index = read.u32(at); surface.component_word = read.u32(at + 4);
                surface.uv_size_uninterpreted = read.f32(at + 8);
                const auto vertexCount = read.u16(at + 12);
                surface.source_strips = read.u16(at + 14);
                const auto size = read.u32(at + 16); at += 20;
                require(surface.material_index < materialCount, DecodeKind48C::malformed,
                    "scene.material_index", surface.source_offset, "Surface material index outside table");
                // Original 246344..2463A4 reads separate attribute/UV-count/
                // palette/reserved bytes. 00ff0107 is the observed single-node,
                // no-palette form of position/normal/UV without stored colors.
                // Original 23c.psg also stores 0000010f: palette zero with
                // position/normal/UV and authored V4_8 colors. Its palette
                // trailer is validated below, as for the uncolored 00000107.
                // Keep exact admission: other palettes/attributes are unproven.
                require(surface.component_word == 0x00ff010f || surface.component_word == 0x0000010f ||
                    surface.component_word == 0x00000107 ||
                    (surface.component_word == 0x00ff0107 && nodeCount == 1),
                    DecodeKind48C::unsupported, "scene.attribute_variant", surface.source_offset + 4, "Unknown original surface attribute variant");
                // Absence is retained semantically; it is not an authored white
                // color or a recovered material/lighting default.
                surface.has_original_colors = surface.component_word == 0x00ff010f ||
                    surface.component_word == 0x0000010f;
                require(std::isfinite(surface.uv_size_uninterpreted) && surface.uv_size_uninterpreted > 0 &&
                    vertexCount >= 3 && surface.source_strips >= 1 && surface.source_strips <= std::uint32_t(vertexCount) / 3u,
                    DecodeKind48C::malformed, "scene.surface_metadata", surface.source_offset + 8, "Invalid UV-size/vertex/strip metadata");
                require(size > 0 && size % 16 == 0 && at <= input.size() && size <= input.size() - at,
                    DecodeKind48C::malformed, "scene.surface_extent", surface.source_offset + 16, "Surface payload extent/alignment invalid");
                const auto vertexLimit = std::min(std::uint32_t(1000000), limits.vertices_all_lods);
                require(parsed.vertices_all_lods <= vertexLimit && vertexCount <= vertexLimit - parsed.vertices_all_lods,
                    DecodeKind48C::limit, "scene.total_vertices", surface.source_offset, "Total vertices across LODs exceed limit");
                parsed.vertices_all_lods += vertexCount;
                decodeSurface(read, at, at + size, vertexCount, parsed, surface, hooks,
                    limits.allow_signed32_quantized_positions); at += size;
                lod.surfaces.push_back(std::move(surface)); ++parsed.surfaces_all_lods;
            }
            parsed.lods.push_back(std::move(lod));
            progress(hooks, "scene_lods", level + 1, lodCount);
        }
        if (nodeCount == 1) {
            require(at == input.size(), DecodeKind48C::unsupported, "scene.single_object_trailer", at, "Unexpected single-object trailer");
        } else {
            const auto palettes = read.u32(at), paddedBytes = read.u32(at + 4); at += 8;
            require(palettes == 1 && paddedBytes == ((nodeCount + 3) & ~3u), DecodeKind48C::unsupported,
                "scene.palette_header", at - 8, "Unknown original rigid matrix palette header");
            read.range(at, std::size_t(paddedBytes) + 4);
            require(at + paddedBytes + 4 == input.size(), DecodeKind48C::malformed,
                "scene.palette_extent", at, "Matrix palette does not consume exact suffix");
            for (unsigned i = 0; i < nodeCount; ++i)
                require(read.data[at + i] == i, DecodeKind48C::unsupported, "scene.palette_order", at + i, "Only identity rigid palette supported");
            require(read.zero(at + nodeCount, paddedBytes - nodeCount) && read.u32(at + paddedBytes) == nodeCount,
                DecodeKind48C::malformed, "scene.palette_count", at + nodeCount, "Invalid matrix palette padding/count");
            parsed.identity_matrix_palette = true; at += paddedBytes + 4;
        }
        for (auto& node : parsed.nodes) {
            cancelled(hooks, node.source_offset);
            for (unsigned axis = 0; axis < 3; ++axis) {
                node.max_bounds_error = std::max(node.max_bounds_error,
                    std::max(std::abs(node.decoded_min[axis] - (node.stored_bounds[axis] - node.stored_bounds[axis + 3])),
                             std::abs(node.decoded_max[axis] - (node.stored_bounds[axis] + node.stored_bounds[axis + 3]))));
            }
            require(node.max_bounds_error <= parsed.quantization[0] * 1.01 + 0.002, DecodeKind48C::unsupported,
                "scene.node_bounds", node.source_offset + 192, "Decoded local points disagree with original node bounds");
        }
        require(at == input.size(), DecodeKind48C::malformed, "scene.trailing_bytes", at, "Unconsumed PSG bytes");
        exportNeutral(parsed, hooks);
        cancelled(hooks, at);
        output = std::move(parsed);
        return true;
    } catch (const Failure48D& failure) { error = failure.error; }
    catch (const std::bad_alloc&) { error = {DecodeKind48C::limit, "scene.allocation", 0, "Scene allocation failed"}; }
    return false;
}
} }

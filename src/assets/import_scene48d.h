#pragma once
#include "import_decode48c.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr { namespace assets {
struct SceneLimits48D {
    std::size_t input_bytes = 8u * 1024u * 1024u;
    std::uint32_t vertices_all_lods = 1000000;
};
struct SceneNode48D {
    std::string name;
    std::int32_t parent = -1;
    std::array<double, 16> local_matrix{}, bind_matrix{};
    std::array<double, 7> stored_bounds{}; // center xyz, half extent xyz, radius
    std::array<double, 3> decoded_min{}, decoded_max{};
    double max_bounds_error = 0;
    std::uint32_t decoded_points = 0;
    std::size_t source_offset = 0;
};
struct SceneVertex48D {
    std::array<double, 3> position_local{}, normal_local{};
    std::array<double, 2> uv{};
    // Meaningful only when the containing surface has_original_colors is true.
    std::array<std::uint8_t, 4> color{};
    std::uint16_t bone = 0;
};
struct SceneStrip48D {
    std::uint32_t group = 0, start = 0, end = 0, vertex_start = 0;
};
struct SceneContinuation48D {
    std::uint32_t previous_strip = 0, next_strip = 0;
    std::uint32_t previous_group = 0, next_group = 0;
};
struct SceneSurface48D {
    std::uint32_t material_index = 0, component_word = 0;
    double uv_size_uninterpreted = 0;
    bool has_original_colors = false;
    std::size_t source_offset = 0;
    std::vector<SceneVertex48D> vertices;
    std::vector<std::uint32_t> indices;
    std::vector<SceneStrip48D> strips;
    std::vector<SceneContinuation48D> continuations;
    std::uint32_t source_strips = 0, groups = 0;
    std::uint32_t normal_positive = 0, normal_negative = 0, normal_zero = 0, mixed_bone_unchecked = 0;
};
struct SceneLod48D {
    double cutoff = 0;
    std::vector<SceneSurface48D> surfaces;
};
struct SceneSpan48D {
    std::uint32_t material_index = 0;
    std::uint32_t vertex_start = 0, vertex_count = 0, index_start = 0, index_count = 0;
    bool has_original_colors = false;
};
struct Scene48D {
    std::vector<SceneNode48D> nodes;
    std::vector<std::string> materials;
    std::array<double, 7> quantization{};
    bool world_space = false, identity_matrix_palette = false;
    std::vector<SceneLod48D> lods;
    std::uint32_t vertices_all_lods = 0, surfaces_all_lods = 0;
    // Diagnostic LOD0 with original bind matrices applied. Original positions
    // remain in the semantic surfaces. Missing colors become explicit neutral
    // white only in this compatibility export, never in semantic source data.
    std::vector<std::uint8_t> neutral_lod0;
    std::vector<SceneSpan48D> spans_lod0;
};

// Bounded port of the preserved decode_scene48c.py subset. Transactional output;
// no guest execution, files, JSON dependency, guessed placement or shading.
// Retains negative-normal diagnostics without silently flipping triangles.
bool decodeScene48D(const std::vector<std::uint8_t>& input, Scene48D& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    const SceneLimits48D& limits = SceneLimits48D{});
} }

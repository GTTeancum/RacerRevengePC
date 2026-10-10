#pragma once
#include "import_service48e.h"
#include "import_world48f.h"
#include "world_index49b.h"

namespace rr { namespace windows {
// Reads one verified active cache generation. Collision-only classifications
// are parallel to the original instances and require a present original COL
// member; a damaged/unconverted PSG must never be hidden as collision-only.
bool loadCachedWorld48F(const std::filesystem::path& cache, const std::string& container,
    const std::string& member, assets::World48F& output, std::vector<bool>& collisionOnly,
    std::string& error);
// Output ordering matches requested unique member names. The implementation
// validates the manifest once and shares selected-object verification work.
bool loadCachedScenes48F(const std::filesystem::path& cache, const std::string& container,
    const std::vector<std::string>& members, std::vector<RuntimeScene48E>& output,
    std::string& error);

struct RuntimeWorld48F {
    // Authored metadata retained independently of visual buffers. Views preserve
    // exact placement/portal values and literal references; no visibility policy.
    assets::OwnedWorldIndex49B world_index;
    // These immutable buffers are assembled once at load. The render loop never
    // parses source files or converts/places vertices.
    assets::Mesh48B mesh;
    std::vector<assets::SceneSpan48D> spans;
    std::vector<RuntimeMaterial48E> materials;
    std::vector<RuntimeTexture48E> textures;
    std::vector<std::string> warnings;
    std::string placed_mesh_sha256;
    std::uint32_t source_instances = 0, visual_instances = 0, collision_only_instances = 0;
    std::uint32_t unique_geometry = 0, zones = 0, portals = 0, replaced_root_matrices = 0;
    std::uint32_t unique_vertices = 0, unique_indices = 0;
    std::size_t semantic_payload_bytes = 0, placed_buffer_bytes = 0, texture_payload_bytes = 0;
    double cache_load_ms = 0, assembly_ms = 0;
};
// Transactional bounded assembly using original local vertices and replacing
// each static root matrix with the authored world placement. No visibility or
// animation is guessed. Only the recovered single-root static subset is used.
bool assembleCachedWorld48F(const std::filesystem::path& cache, const std::string& container,
    const std::string& member, RuntimeWorld48F& output, std::string& error);
} }

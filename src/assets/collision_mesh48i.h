#pragma once
#include "import_collision48g.h"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace rr::assets {
// Same ordered tree as RRCOL001. A high child bit identifies a face index.
constexpr std::uint32_t collision_mesh_leaf48i = 0x80000000u;
struct CollisionMeshNode48I {
    float volume = 0;
    std::array<float, 3> center{}, half{};
    std::array<std::uint32_t, 2> children{};
};
struct CollisionMeshFace48I {
    float d = 0;
    std::array<std::int16_t, 3> normal{};
    std::array<std::uint16_t, 3> vertices{};
};
struct CollisionMesh48I {
    std::array<float, 16> local_matrix{};
    std::uint32_t reverse_mode = 0;
    std::vector<std::array<float, 3>> vertices, vertex_normals;
    std::vector<CollisionMeshNode48I> nodes;
    std::vector<CollisionMeshFace48I> faces;
};
// Validates a native cache before constructing compact runtime data. All original
// ordering is retained; source bytes and offsets stay in the cache, not the live
// mesh. No original COL decoder or guest execution. Failure leaves output intact.
bool loadCollisionMesh48I(const std::vector<std::uint8_t>& cache,
                         CollisionMesh48I& output, std::string& error);
// Compact runtime projection of original type1, retaining member source order.
// This storage is cold-owned: freeze members before binding their mesh addresses.
struct CollisionCompoundMeshMember50IJ {
    std::uint32_t scene_node_index=0,source_offset=0,body_offset=0,source_end=0;
    CollisionMesh48I mesh;
};
struct CollisionCompoundMesh50IJ {
    std::uint32_t reverse_mode=0;
    std::vector<CollisionCompoundMeshMember50IJ> members;
};
// Decode original COL1 directly, then apply the same source face-normal packing
// as loadCollisionMesh48I. No synthesized COL0 header/cache or geometry fallback.
// Caller retains raw source; failure leaves runtime output unchanged.
bool loadCollisionCompoundMesh50IJ(const std::vector<std::uint8_t>& original,
    CollisionCompoundMesh50IJ& output, std::string& error,
    const CollisionCompoundLimits50IJ& limits={});
// Original static creator27B1FC: separately normalize Up/Look, then Up x Look.
// Ordinary finite binary32 domain, strict arithmetic; no orthogonalization.
bool placementCollision48I(const std::array<float, 3>& position,
                          const std::array<float, 3>& look,
                          const std::array<float, 3>& up,
                          std::array<float, 16>& output) noexcept;
// Original11F730 ordered local x placement composition (row-vector storage).
// Only finite affine inputs are accepted. Output may alias either input.
bool composeCollisionWorld48I(const std::array<float, 16>& local,
                             const std::array<float, 16>& placement,
                             std::array<float, 16>& output) noexcept;
}

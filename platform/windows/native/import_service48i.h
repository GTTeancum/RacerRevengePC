#pragma once
#include "import_service48f.h"
#include "collision_mesh48i.h"

namespace rr::windows {
struct PlacedCollision48I {
    std::uint32_t source_instance = 0, asset = 0;
    std::array<float, 16> matrix{};
};
struct RuntimeCollisionWorld48I {
    std::vector<assets::CollisionMesh48I> assets;
    std::vector<PlacedCollision48I> instances;
    std::uint32_t original_instances = 0, missing_original_members = 0;
    std::size_t retained_payload_bytes = 0;
};
struct RuntimeWorld48I { RuntimeWorld48F visuals; RuntimeCollisionWorld48I collisions; };
// Same evidenced default static-member policy as48G; compact native records and
// original-order float placements. Candidate eligibility/category remains separate.
// Caller holds the import read lock throughout the load.
bool loadCachedCollisionWorld48I(const std::filesystem::path&, const std::string& container,
    const std::string& member, RuntimeCollisionWorld48I&, std::string& error);
bool assembleCachedWorld48I(const std::filesystem::path&, const std::string& container,
    const std::string& member, RuntimeWorld48I&, std::string& error);
}

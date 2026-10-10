#pragma once
#include "import_service48f.h"
#include "import_collision48g.h"

namespace rr { namespace windows {
struct PlacedCollision48G {
    std::uint32_t source_instance = 0, asset = 0;
    std::array<double,16> matrix{};
};
struct RuntimeCollisionWorld48G {
    std::vector<assets::Collision48G> assets;
    std::vector<PlacedCollision48G> instances;
    std::uint32_t original_instances = 0, missing_original_members = 0;
    std::size_t retained_payload_bytes = 0;
};
struct RuntimeWorld48G { RuntimeWorld48F visuals; RuntimeCollisionWorld48G collisions; };
// Caller holds the cache import.lock read handle across this operation.
// Supports the recovered default static collision policy; authored overrides
// receive explicit errors. Missing original COL files follow the original
// optional load route; present unconverted or damaged COL files are errors.
bool loadCachedCollisionWorld48G(const std::filesystem::path&, const std::string& container,
    const std::string& member, RuntimeCollisionWorld48G&, std::string& error);
// One read lock spans visual and collision loading; retained data owns its memory.
bool assembleCachedWorld48G(const std::filesystem::path&, const std::string& container,
    const std::string& member, RuntimeWorld48G&, std::string& error);
} }

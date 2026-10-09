#pragma once
#include "vehicle_forces48d.h"
#include <cstdint>

namespace rr::game {
struct TerrainFollowState48G {
    std::uint32_t active104, pending10c, detached11c;
    Vector3_48C previousPoint180, previousNormal190, largestVelocityChange1d0;
};
struct TerrainFollowParameters48G {
    Vector3_48C localOffset1c0;
    float airborneTorque204, detachAngle208, detachDistance20c;
};
struct TerrainSurface48G {
    std::uint32_t forceAngleTest108, hit110, hit114;
    Vector3_48C point150, normal170;
};
struct TerrainScene48G {
    ReadyBodyBasis48D basis;
    Vector3_48C position;
};
struct TerrainFollowBody48G {
    Vector3_48C position60, worldVelocity70, cachedLocalVelocity80;
    float angleE8;
    BodyLoads48C loads;
    TerrainScene48G scene;
};
struct TerrainFollowResult48G {
    TerrainFollowState48G state;
    TerrainFollowBody48G body;
};

// Original vehicle+348 response 0x1B1A30 and numeric root-scene operations.
// Runs after aerodynamic/collision loads, before rigid-body scene readback.
// Surface query/detection, SpringCollisionModel impulses and scene child
// notifications are separate. This function does NOT replace them with rays.
// Original function reads no elapsed time. All floats/intermediates finite;
// failure leaves out unchanged. /fp:strict, no FMA/reassociation required.
// Original-word oracle checks ordinary host binary32 plus supplied PCSX2's
// finite VU RSQRT normalization policy, not every exceptional PS2 FPU mode.
bool applyTerrainFollowing48G(const TerrainFollowState48G& previous,
    const TerrainFollowParameters48G& parameters, const TerrainSurface48G& surface,
    const TerrainFollowBody48G& body, TerrainFollowResult48G& out) noexcept;
}

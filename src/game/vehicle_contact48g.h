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

// Pure, separate1B1A30 numeric phases; success is NOT following/scene completion.
// Caller owns history copies before each phase and the later conditional reads.
struct TerrainOrientationPrefix48G {
    Vector3_48C side,normalAxis;
    float angle;
};
bool prepareTerrainHistoryDistance48G(const Vector3_48C& previousPoint,
    const Vector3_48C& currentPoint,const Vector3_48C& normal,float& distance) noexcept;
bool prepareTerrainOrientationPrefix48G(const Vector3_48C& forward,const Vector3_48C& up,
    const Vector3_48C& previousNormal,const Vector3_48C& currentNormal,
    TerrainOrientationPrefix48G& output) noexcept;
bool prepareTerrainOrientationAlignment48G(const TerrainOrientationPrefix48G& input,
    float& alignment) noexcept;

// Separate active1B1A30 numeric scratch, before real stores/getter/translation.
bool prepareTerrainNormalForce48G(const Vector3_48C& force,
    const Vector3_48C& normal,Vector3_48C& extra) noexcept;
bool prepareTerrainTranslation48G(const Vector3_48C& surfacePoint,
    const Vector3_48C& offsetPoint,const Vector3_48C& normal,
    float actualScale,Vector3_48C& displacement) noexcept;

//Source1B2014 phases; scratch only, no velocity/history/scene completion.
bool prepareTerrainVelocityFraction48G(const Vector3_48C& velocity,
    const Vector3_48C& normal,float& fraction) noexcept;
bool prepareTerrainSteepNormal48G(const Vector3_48C& velocity,
    const Vector3_48C& normal,Vector3_48C& amount) noexcept;
bool prepareTerrainTangentVelocity48G(const Vector3_48C& velocity,
    const Vector3_48C& normal,Vector3_48C& tangentVelocity) noexcept;
bool prepareTerrainVelocityChange48G(const Vector3_48C& current,
    const Vector3_48C& previous,const Vector3_48C& largest,
    Vector3_48C& change,bool& replace) noexcept;

//Separate source caller cross and reached28A090 unnormalized basis scratch.
bool prepareTerrainForward48G(const Vector3_48C& forward,
    const Vector3_48C& normal,Vector3_48C& newForward) noexcept;
bool prepareTerrainBasis48G(const Vector3_48C& forward,
    const Vector3_48C& up,ReadyBodyBasis48D& basis) noexcept;

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

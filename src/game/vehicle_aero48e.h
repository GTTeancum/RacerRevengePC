#pragma once
#include "vehicle_forces48d.h"

namespace rr::game {
// Names retain field offsets where a physical/tuning-key meaning is not yet
// proven. These are selected runtime coefficients, not raw PHY defaults.
struct AeroParameters48E {
    float offset68, lift6c, drag78, pitch84, pitchDamping88, lateral90;
    float rollDampingB0, timerYawB8, yawDampingC8;
    float driveDragF8, brakeDragFc, steering100, steeringRamp104, timerSteering108;
    float timerLateral10c, timerYaw110, timerDrag114;
    float tiltedLateral118, tiltedSteering11c, tiltedYaw120;
    Vector3_48C velocityDamping130;
};
struct AeroInput48E {
    // Body +80: previous wrapper's resolved local velocity, not freshly inferred
    // from BodyState48C.velocity. Body +E0 provides the low-forward-speed angle.
    Vector3_48C bodyLocalVelocity, angularVelocity;
    float bodyAngleE0;
    bool subtractWorldFlow;                    // Model +34.
    Vector3_48C worldFlow40;
    bool triggerD0, triggerD8, fullSteeringE0; // Source actions not inferred.
    float driveE4, driveE8, brakeEc;
};
struct AeroState48E {
    float timer140, timer144;
    bool lowSpeedDc;
};
struct AeroResult48E {
    AeroState48E state;
    Vector3_48C localForce, directTorque;
    BodyLoads48C accumulatedLoads;
};
// Full original 0x16CD90 numerical producer/application for a linked body with
// ready cached scene basis. Dirty scene traversal is the caller's dependency.
// Preserves timer, branch and binary32 operation order. Finite domain; dt>=0.
// Failure leaves out untouched. No guest memory/instruction runtime required.
bool applyAerodynamics48E(const AeroState48E&, const AeroParameters48E&,
                         const AeroInput48E&, const ReadyBodyBasis48D&,
                         const BodyLoads48C&, float elapsedFactor, AeroResult48E& out) noexcept;

struct VehiclePartialStep48E {
    EngineState48D engine;
    AeroResult48E aero;
    BodyState48C body;
};
// Original recovered order: engine generation -> gravity -> optional engine
// application -> aero -> body core. Contact/repulsor/scene wrapper still absent.
bool stepEngineAeroBody48E(const BodyState48C&, const BodyParameters48C&, const BodyLoads48C&,
                          const EngineState48D&, const EngineParameters48D&, const EngineInput48D&,
                          const AeroState48E&, const AeroParameters48E&, const AeroInput48E&,
                          const ReadyBodyBasis48D&, GravityParameters48D, bool applyEngine,
                          float elapsedFactor, VehiclePartialStep48E& out) noexcept;
// Exact scalar writer 0x167BC0..0x167C18 (RSQRT then reciprocal), deliberately
// separate: invocation ordering relative to RunSimulation remains unproven.
bool refreshCachedSpeed48E(const Vector3_48C& velocity, float& out) noexcept;
}

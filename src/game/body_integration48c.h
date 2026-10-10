#pragma once

namespace rr::game {

struct Vector3_48C { float v[3]; };
// The original quaternion is scalar first: [w,x,y,z].
struct Quaternion48C { float v[4]; };

struct BodyParameters48C {
    float mass;
    float inverseMass;
    Vector3_48C inverseInertiaColumns[3];
    float linearAccelerationLimitSquared;
    float angularAccelerationLimitSquared;
    float linearVelocityLimitSquared;
    float angularVelocityLimitSquared;
    bool translationEnabled[3];
    bool rotationEnabled[3];
};

struct BodyLoads48C { Vector3_48C force, torque; };

struct BodyState48C {
    Vector3_48C position, velocity, angularVelocity;
    Quaternion48C orientation;
    // These are actual persistent Adams-Bashforth histories, not optional caches.
    Vector3_48C previousPositionDerivative;
    Vector3_48C previousLinearAcceleration;
    Vector3_48C previousAngularAcceleration;
    Quaternion48C previousOrientationDerivative;
    // Current derivatives are exposed for semantic comparisons/next systems.
    Vector3_48C linearAcceleration, angularAcceleration;
    Quaternion48C orientationDerivative;
};

// Numerical core of PhysicsRigidBody::Update, ELF 0x167CD4..0x1680F0,
// with the observed 0x14EE40 integrator and original quaternion helpers.
// Caller supplies the original pre-integration position/orientation (the full
// function obtains them from its associated scene transform).
//
// This is NOT a whole vehicle update: it does not produce forces/collisions,
// update the scene transform, clear force accumulators, or run callbacks.
// elapsedFactor retains original f12 units; it must be finite and >=0.
// Consumed inputs must be finite, squared limits >=0; overflowing intermediates
// fail. The three current derivative fields in input are ignored/overwritten.
// Failure leaves output unchanged, including when output aliases input.
// Strict binary32 operation order required (/fp:strict, no fast math/FMA).
// Equivalence is to the generated reference's host arithmetic, not all PS2
// floating-point exceptional modes. The caller owns the floating-point mode.
bool integrateBody48C(const BodyState48C& input,
                      const BodyParameters48C& parameters,
                      const BodyLoads48C& loads, float elapsedFactor,
                      BodyState48C& output) noexcept;

}

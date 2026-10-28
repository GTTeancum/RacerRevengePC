#pragma once
#include "body_scene48g.h"

namespace rr::game {
struct SpringPointForceParameters48J {
    Vector3_48C primaryLocalCenter, customLocalCenter;
    float torqueScale;
    bool useCustomCenter, yawTorqueOnly;
};
struct SpringFrictionBody48J {
    BodyMatrix48G scene;
    BodyLoads48C loads;
    float mass;
};
struct SpringFrictionResult48J {
    Vector3_48C worldForce;
    BodyLoads48C primary, other;
};
//Pure296CDC..296ED0 torque delta only, after the caller's already committed
//forceXYZ and actual point/global sentinel branch. No loads or source return.
bool prepareSpringTorqueDelta48J(const BodyMatrix48G&,
    const SpringPointForceParameters48J&,const Vector3_48C& force,
    const Vector3_48C& point,Vector3_48C&)noexcept;
// Original296C10: add world force; transform point/force into the recipient's
// frame and accumulate scaled local torque. A zero world point is the original
// no-torque sentinel. The default lever-arm center comes from the PRIMARY body,
// including when applying the opposite force to a second body.
bool addSpringPointForce48J(const BodyMatrix48G&, const BodyLoads48C&,
    const SpringPointForceParameters48J&, const Vector3_48C& worldForce,
    const Vector3_48C& worldPoint, BodyLoads48C&) noexcept;
//Pure297390..297598 friction world-force prefix only; no296C10/body
//store or whole297390 return. Uses the retained pre-response cached120.
bool prepareSpringFrictionVector48J(const Vector3_48C& normal,
    const Vector3_48C& cachedVelocity,float coefficient,float primaryMass,
    Vector3_48C&)noexcept;
// Original297390, including both original296C10 calls when other!=nullptr.
// contactVelocity is SpringCollisionModel+120, not recomputed body velocity.
// All consumed inputs/intermediates finite; errors leave output untouched.
bool applySpringFriction48J(const Vector3_48C& normal, const Vector3_48C& point,
    const Vector3_48C& contactVelocity, float coefficient,
    const SpringPointForceParameters48J&, const SpringFrictionBody48J& primary,
    const SpringFrictionBody48J* other, SpringFrictionResult48J&) noexcept;
}

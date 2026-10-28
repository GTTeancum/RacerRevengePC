#pragma once
#include "body_wrapper48h.h"

namespace rr::game {
struct SpringImpactParameters48K {
    Vector3_48C worldCenter110, contactVelocity120, customLocalCenter100;
    float normalForceScale98, angularImpulseScaleB0, forceTorqueScaleB4;
    float restitutionB8, restitutionBC, restitutionC0;
    float positionCorrectionScaleCC, angularCorrectionScaleD0;
    bool disableLeverArms5C, correctPenetration64, yawOnly6C, customCenter70;
};
struct SpringImpactBody48K {
    RootBodyState48H state;
    BodyParameters48C parameters;
    Vector3_48C localCenter;
};
struct SpringImpactResult48K { RootBodyState48H primary, other; };
//Distinct pure297610 raw-arm/effective-mass/normal-speed/impulse prefix.
//No body stores, scene getters or whole297610 return.
struct SpringImpactImpulsePrefix48K {
    Vector3_48C arm,otherArm,impulse;
    float effectiveMass,normalSpeed;
    bool inward;
};
bool prepareSpringImpactImpulsePrefix48K(const Vector3_48C& normal,const Vector3_48C& point,
    const SpringImpactParameters48K&,const SpringImpactBody48K& primary,
    const SpringImpactBody48K* other,SpringImpactImpulsePrefix48K&)noexcept;
//Pure post-getter arm cross/inertia/yaw/scale, not any1D0/B0 stores.
bool prepareSpringImpactAngularPrefix48K(const Vector3_48C& arm,
    const Vector3_48C& actualInverseGetterResult,const BodyParameters48C&,
    float sourceScale,bool yawOnly,Vector3_48C&)noexcept;
//Pure297FF8 inertia(torque) cross arm, before the actual direction getter.
bool prepareSpringImpactTorqueArm48K(const BodyParameters48C&,const Vector3_48C& torque,
    const Vector3_48C& arm,Vector3_48C&)noexcept;
//Pure165580/165740 additional delta only; actual accumulatorXYZ adds belong
//to the same-owner caller and must not be replaced by an atomic root result.
bool prepareSpringCorrectionDelta48K(const Vector3_48C& current,
    const Vector3_48C& incoming,Vector3_48C&)noexcept;
// Native original297610, including force accumulation and165580/165740 correction
// aggregation. Scenes must be current world matrices before this numerical phase;
// scene graph refresh/notifications belong to the surrounding native scheduler.
// Other body must be distinct. No PS2 state or addresses are consumed.
// Strict binary32, finite consumed arithmetic, caller-owned rounding mode.
// Failure leaves output unchanged. No-other result.other is value-initialized.
bool applySpringImpact48K(const Vector3_48C& normal,const Vector3_48C& point,
    float penetration,const SpringImpactParameters48K&,const SpringImpactBody48K&,
    const SpringImpactBody48K* other,SpringImpactResult48K&) noexcept;
}

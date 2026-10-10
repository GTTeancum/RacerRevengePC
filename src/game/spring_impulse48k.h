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

#pragma once
#include "spring_contact48j.h"
#include "spring_impulse48k.h"

namespace rr::game {
struct SpringForceParameters48L {
    Vector3_48C contactVelocity120;
    SpringPointForceParameters48J pointForce;
    float maxPenetration90, maxSpeed94;
    float stiffness9C, dampingA0, impactStiffnessA4, impactDampingA8;
    bool impact54, otherSpring58;
};
// Original297190, including both original296C10 point-force calls. The cached
// relative contact velocity is consumed directly, without recomputing bodies.
// Impact coefficients are selected iff impact54 && (!other || !otherSpring58).
// With another body, the smaller mass is multiplied by the larger body's
// stored inverse mass; neither reciprocal is recomputed. Equal masses consume
// the other's inverse mass. Only that inverse mass and selected coefficients
// must be finite. No-other ignores both mass fields entirely.
// pointForce.primaryLocalCenter must describe the primary body, also for the
// equal/opposite recipient, as required by original296C10. The zero point is
// its no-torque sentinel. Other must be distinct. Unconsumed state is ignored.
// Strict individually rounded binary32; caller owns the rounding mode. No
// allocation, PS2 state or global mutation. Failure leaves output unchanged;
// successful no-other output.other is value-initialized.
bool applySpringForce48L(const Vector3_48C& normal,const Vector3_48C& point,
    float penetration,const SpringForceParameters48L&,const SpringImpactBody48K& primary,
    const SpringImpactBody48K* other,SpringFrictionResult48J&) noexcept;
// Original2983C0: remove the inward cached normal-speed component from the
// primary velocity only. Does not update torque, correction accumulators or
// any other body. Finite consumed arithmetic, transactional output.
bool removeSpringInwardVelocity48L(const Vector3_48C& normal,
    const Vector3_48C& cachedContactVelocity,const Vector3_48C& primaryVelocity,
    Vector3_48C& output) noexcept;
}

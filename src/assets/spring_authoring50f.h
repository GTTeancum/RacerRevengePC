#pragma once
#include "impact_authoring49y.h"
#include "../game/spring_model48k.h"
namespace rr::assets {
enum class SpringKey50F : unsigned {MaxCompression,Spring,Damping,ImpulseSpring,
    ImpulseDamping,Friction,WallRestitution,PodRestitution,MomentScale,
    AngularImpulseScale,FrictionTorqueScale,Count};
constexpr std::size_t springKeyCount50F=static_cast<unsigned>(SpringKey50F::Count);
struct SpringCollisionAuthoring50F {float values[springKeyCount50F];};
struct SpringCollisionProvenance50F {ImpactAuthoringToken49Y fields[springKeyCount50F];};
// Bounded cold PHY reader, same lexical/duplicate rules as49N/49Y. All11
// Collisions keys required; unknown values ignored, no authored defaults.
// Atomic output/provenance. Diagnostic written unless it aliases another span.
bool readSpringCollision50F(const char*,std::size_t,SpringCollisionAuthoring50F&,
    PodPhysicsDiagnostic49N&,SpringCollisionProvenance50F* =nullptr) noexcept;
// Original2159A0..215A6C/295C90/295DD0 for a present primary body.
// Applies authored values to supplied initialized parameters; leaves runtime
// toggles, caches, normalForce98, correction scales and materials1..9 unchanged.
// Uses the explicit original body.mass26C. Separate rounded multiply/sqrt order;
// ordinary finite arithmetic only. Error preserves both destinations.
bool applySpringCollision50F(const SpringCollisionAuthoring50F&,float mass26C,
    rr::game::SpringGeneratedParameters48K&,rr::game::SpringResponseParameters48L&) noexcept;
}

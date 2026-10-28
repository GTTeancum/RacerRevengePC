#pragma once
#include "spring_impulse48k.h"
#include <array>
#include <cstddef>
#include <cstdint>
namespace rr::game {
struct SpringGeneratedParameters48K {
    SpringImpactParameters48K impact;
    float frictionTorqueScaleC4;
    std::array<float,10> frictionByMaterial;
    bool pointContacts48,springForce4C,friction50,impact54,otherSpring58,secondary68;
};
struct SpringGeneratedState48K { std::uint32_t pending38,contact3C,upward40,other44,point74,generated78,field130; };
struct SpringGeneratedRecord48K {
    Vector3_48C localPoint10,normal20,surfacePoint30;
    BodyMatrix48G scene40;
    bool hasScene40;
    std::uint32_t otherBodyIdentity44,material4C;
    std::uint8_t flags50;
};
//Pure296884..296974 prefix only. No contact flags,296F10, responses or
//whole2964C0 completion. Caller authenticates actual record/scene owner first.
struct SpringRecordGeometryPrefix48K {Vector3_48C worldPoint;float depth;};
bool prepareSpringRecordGeometry48K(const BodyMatrix48G&,const Vector3_48C& localPoint,
    const Vector3_48C& normal,const Vector3_48C& surfacePoint,
    SpringRecordGeometryPrefix48K&)noexcept;
struct SpringModelBody48K {std::uint32_t identity;SpringImpactBody48K body;};
// Caller-owned storage, normally reserved once per simulation world. The
// coordinator never allocates, resizes, frees or retains these buffers.
struct SpringGeneratedBuffers48K {
    SpringModelBody48K* bodies;
    std::size_t bodyCapacity;
    std::uint8_t* recordFlags50;
    std::size_t recordCapacity;
};
struct SpringGeneratedResult48K {
    SpringGeneratedState48K state;
    Vector3_48C worldCenter110,contactVelocity120;
    std::size_t bodyCount,recordCount;
};
enum class SpringGeneratedStatus48K { Complete,InvalidInput,UnsupportedScope,InsufficientCapacity };
// Original2964C0 generated-contact phase and296F10 preparation, composing
// original297610/297390 numerical responses. bodies[0] is the primary body;
// identities are nonzero/unique; record other identity0 means static world.
// Point contacts and spring-force/secondary response branches are unsupported.
// Scenes are caller-refreshed world matrices. Strict binary32 arithmetic and
// the caller's rounding mode are retained; records execute serially in order.
// Scratch must be disjoint from inputs, result metadata and output storage.
// Result metadata/buffer descriptors must not overlap any writable storage.
// Output bodies may exactly alias input bodies; other overlaps are rejected.
// Both buffers need bodyCount bodies and count flag bytes. On Complete the
// output buffers and metadata are committed together; on error only scratch
// may change. There is no imposed gameplay contact limit or per-call allocation.
SpringGeneratedStatus48K updateGeneratedSpringContacts48K(
    const SpringGeneratedParameters48K&,const SpringGeneratedState48K&,
    const SpringModelBody48K*,std::size_t,const SpringGeneratedRecord48K*,std::size_t,
    const SpringGeneratedBuffers48K& scratch,const SpringGeneratedBuffers48K& destination,
    SpringGeneratedResult48K&) noexcept;
// Additional original response tuning. No inferred units or desktop defaults.
struct SpringResponseParameters48L {
    float maxPenetration90,maxSpeed94,stiffness9C,dampingA0,impactStiffnessA4,impactDampingA8;
    bool secondaryAtLimit60;
};
struct SpringPointRecord48L {
    SpringGeneratedRecord48K contact;
    float penetrationOffset48;
};
// Full numerical2964C0 coordinator over caller-prepared point/generated lists.
// Point records run first when pointContacts48 is enabled. Their explicit scene
// is required; their other identity is ignored; friction additionally needs
// flags50 bit0. Flag output is all point records followed by all generated ones.
// Disabled point records preserve flags without consuming scene/geometry data.
// The remaining buffer/transaction/strict-arithmetic contracts above apply.
// Scene refresh/query generation are caller-owned. Original process-local
// sentinel initialization is not part of this typed numerical interface.
SpringGeneratedStatus48K updateSpringContacts48L(
    const SpringGeneratedParameters48K&,const SpringResponseParameters48L&,
    const SpringGeneratedState48K&,const SpringModelBody48K*,std::size_t,
    const SpringPointRecord48L*,std::size_t,const SpringGeneratedRecord48K*,std::size_t,
    const SpringGeneratedBuffers48K& scratch,const SpringGeneratedBuffers48K& destination,
    SpringGeneratedResult48K&) noexcept;

}

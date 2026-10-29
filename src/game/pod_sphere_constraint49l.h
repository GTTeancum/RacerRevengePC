#pragma once
#include <cstdint>

namespace rr::game {
// Source 128-bit vectors and column-contiguous 4x4 matrices. All lanes are
// retained, including the homogeneous lanes written/read by the VU helpers.
struct PodSphereVector49L { float v[4]; };
struct PodSphereMatrix49L { float v[16]; };
struct PodSphereBody49L {
    bool scenePresent; // body+288 presence; supplied scene snapshot follows
    std::uint32_t sceneFlags190;
    PodSphereMatrix49L sceneWorld100;
    float inverseMass; // body+270
    PodSphereMatrix49L inverseInertia; // body+140
    PodSphereVector49L linearVelocity, angularVelocity; // body+70/+B0
    PodSphereVector49L worldForce, localTorque, localCenter; // +180/+190/+1E0
};
struct PodSphereJoint49L {
    float stiffness30, damping34;
    std::uint32_t enabled38, unilateral3C, suppressed40;
    // Native pointer-presence facts; the corresponding bodies are supplied to
    // each stage. No object addresses, ownership, or source RAM are retained.
    bool bodyAPresent44, bodyBPresent48;
    float length4C, lengthSquared50, speedSquared54;
    PodSphereVector49L localAnchor60, localAnchor70;
    PodSphereVector49L worldAnchor80, worldAnchor90, fixedAnchorA0, fieldB0;
    PodSphereVector49L centripetalC0, separationD0, directionE0, preparedForceF0;
    PodSphereMatrix49L cached100, cached140, inverse180;
};
// Source-ordered numeric construction/setup; unlike legacy transactional APIs,
// these retain all earlier stores on false. stoppedPC is the reached source
// phase/store admission boundary, not a guest CPU cursor. No base constructor,
// dispatch/owner identity, allocation, append, scene getter, or readiness proof.
struct PodSphereSourcePresenceHooks49L {
    void* context=nullptr;
    void (*publish)(void* context,bool isBodyB,bool present) noexcept=nullptr;
};
// Optional non-failing hooks publish only the matching typed owner alias just
// after44/48 numeric stores. They must not mutate joint/inputs/global words or
// reenter. ActualA must be non-null and proven by caller before setupA entry.
// Inputs are stable, distinct from joint storage. Actual434390 points to four
// source words, or null to stop at its first load after initialVF0/flag stores.
// Word copies admit arbitrary payloads; only consumed arithmetic is restricted
// to finite FE_TONEAREST binary32, with no fast math/FMA. No rollback/resume.
bool initializePodSphereSourceOrdered49L(PodSphereJoint49L&,
    const std::uint32_t* actual434390Words,std::uint32_t& stoppedPC,
    const PodSphereSourcePresenceHooks49L* hooks=nullptr) noexcept;
bool setupPodSphereASourceOrdered49L(PodSphereJoint49L&,float inverseMass270,
    const PodSphereMatrix49L& inverseInertia140,const PodSphereVector49L& anchor,
    std::uint32_t& stoppedPC,const PodSphereSourcePresenceHooks49L* hooks=nullptr) noexcept;
bool setupPodSphereBSourceOrdered49L(PodSphereJoint49L&,bool bodyBPresent,
    const PodSphereVector49L& anchor,float length,std::uint32_t& stoppedPC,
    const PodSphereSourcePresenceHooks49L* hooks=nullptr) noexcept;
// Represented numeric constructor-suffix writes1B6214..1B6468, after the
// excluded base initializer1276E0. This is not full object construction.
// Caller initializes excluded base-owned state and the incoming native object.
// Every represented flag/reference is explicitly written by the source suffix.
bool initializePodSphere49L(PodSphereJoint49L&) noexcept;
// Setup1B64D0 admits a non-null A. It freezes its then-current inverse mass
// and inertia into J140/J100. Subsequent stages use those caches unchanged.
bool setupPodSphereA49L(PodSphereJoint49L&, const PodSphereBody49L&,
                       const PodSphereVector49L& localAnchor) noexcept;
// Setup1B6670: null B selects fixedAnchorA0 during preparation. The local
// anchor is still stored. Source MAX clamps length against bits3DCCCCCD.
bool setupPodSphereB49L(PodSphereJoint49L&, const PodSphereBody49L* bodyB,
                       const PodSphereVector49L& localAnchor, float length) noexcept;
// Call in original order:1B66D0,1B6BA0,1B6FF0. These are independent numeric
// leaves, not the vehicle2188B0 wrapper. They stop before368 contacts and any
// secondary-body integration. B is read-only; all force/torque writes target A.
bool preparePodSphere49L(PodSphereJoint49L&, const PodSphereBody49L* bodyA,
                        const PodSphereBody49L* bodyB) noexcept;
bool correctPodSphere49L(PodSphereJoint49L&, PodSphereBody49L* bodyA,
                        const PodSphereBody49L* bodyB, float& magnitude) noexcept;
bool applyPodSphere49L(const PodSphereJoint49L&, PodSphereBody49L* bodyA) noexcept;
//Pure1B6784..1B67F0 scratch for the retained source-ordered caller. This
//neither mutates a joint/body nor attests a getter or slot74 completion.
//False preserves numeric outputs; stoppedPC identifies the reached phase.
//lastPointVf6W is the actual last point getter's sceneWorld100[11], NOT its
//constant point-result W. Zero distance is admitted here: the source reaches
//C0 before its later reciprocal. Same finite FE_TONEAREST domain as below.
//angularVelocityXYZ references three stable floats; numeric outputs are
//distinct scratch storage. It is not an owner/scene or body-presence receipt.
bool preparePodSphereCentripetalPrefix49L(const PodSphereVector49L& separation,
    const float* angularVelocityXYZ,const PodSphereVector49L& localAnchor,
    float lastPointVf6W,float& distanceSquared,PodSphereVector49L& tangential,
    PodSphereVector49L& centripetal,std::uint32_t& stoppedPC) noexcept;
// Legacy functions and the centripetal prefix require FE_TONEAREST and strict binary32/no fast math/FMA.
// Each function is serial, fixed-size, allocation-free, and preserves numeric
// outputs on false. The prefix helper's diagnostic stoppedPC still advances.
// Stages with enabled38==0 do nothing; correction returns +0 while preserving
// suppressed40. Enabled stages require A and its scene ready (190 bit0); B,
// when present, requires a ready scene in preparation. Pointer presence must
// agree with J44/J48. No scene hierarchy refresh or callbacks are substituted.
// Consumed values and arithmetic intermediates must be finite. Preparation
// admits strictly positive squared separation; this is a native boundary, NOT
// an invented source epsilon or coincident-anchor fallback. Source inverse's
// exact-zero determinant branch is supported and writes all16 zero lanes.
// Global434390 is admitted in its source-initialized (0,0,0,1) state.
// Arithmetic equivalence is to finite binary32 source-word execution, not all
// physical EE/VU exceptional floating-point modes. The magnitude output must
// be distinct from joint/body storage.
// The caller owns stage ordering and must retain caches between stages. Live
// mass/inertia during correction are distinct from setup-cached mass/inertia.
}

#pragma once
#include "body_advancement49k.h"
#include "secondary_body49m.h"

namespace rr::game {
// Supplied constructed/registered/root facts. No constructor, registration,
// hierarchy preparation, contact world, or hardware setup is claimed here.
struct SecondaryDomain49M {
    std::uint32_t body320, registrationMask, bodyChildCount;
    std::uint32_t sceneChildCount, sceneObserverCount;
    bool sceneHasParent;
};
struct MainSphereLanes49M {
    std::uint32_t sceneFlags190;
    float velocity70W, angularVelocityB0W, force180W, torque190W;
};
struct MainSphereParameters49M {
    float centerOffsetW;
    float inverseInertiaExtra[7]; // lanes3,7,11,12,13,14,15
};
struct CoupledBodyParameters49M {
    MainBodyParameters49K main;
    MainSphereParameters49M mainSphere;
    SecondaryBodyParameters49M secondary;
};
enum class CoupledBodyPhase49M {
    FrameReady, AwaitingPostPlayer, AwaitingPostBase, SubstepReady,
    AwaitingWorldPreparation, AwaitingResponse354, AwaitingSecondary,
    AwaitingResponse368, AwaitingMainIntegration, AwaitingQuery219120,
    AwaitingSceneConsumer
};
struct CoupledBodyState49M {
    // secondBodyPresent is permanently true for every admitted phase. The49K
    // public API retains its rejection and is never called by this composition.
    MainBodyState49K main;
    SecondaryDomain49M secondaryDomain;
    SecondaryBodyState49M secondary;
    MainSphereLanes49M mainSphere;
    PodSphereJoint49L joint;
    std::uint32_t vehicle838;
    float lastConstraintMagnitude;
    CoupledBodyPhase49M phase;
};
// One caller-owned shared simulation record, reused in original serial vehicle
// order. Noise initialization, seed choice and external users remain explicit.
struct CoupledNoiseContext49M { GradientNoiseState49M noise; std::uint32_t sharedRandomState; };
// Represented BODY/JOINT external snapshot. Contacts may change either
// body, the joint and the supplemental full-vector lanes. Main is read-only
// ONLY while49L is running. Internal phase tags must retain their current value.
// Admission requires these external stages not to mutate main frame caches,
// parameters, domains, controls/scheduler or shared noise/RNG. No unrestricted
// external effects, whole219810 implementation or complete world are claimed.
struct CoupledComponents49M {
    MainBodyComponents49K main;
    SecondaryBodyState49M secondary;
    MainSphereLanes49M mainSphere;
    PodSphereJoint49L joint;
};
CoupledComponents49M coupledComponents49M(const CoupledBodyState49M&) noexcept;
struct PostBase49M {
    CoupledComponents49M components;
    float liveSubstepScale;
    std::uint32_t liveVehicleState40, applicationActive11C, liveVehicle838;
};
struct PostWorld49M {
    WorldPreparationPoint49K point;
    CoupledComponents49M components;
    bool aeroSubtractWorldFlow;
    Vector3_48C aeroWorldFlow40;
    std::uint8_t liveApplicationGateByte58;
};
struct PostResponse354_49M {
    CoupledComponents49M components;
    std::uint32_t response3C, response44, liveVehicle838;
};
struct SecondaryInputs49M {
    float gravity378, gravity37C;
    PodSphereVector49L targetRelative2F0;
    float effective4346C8, effective4346D0, effective4346D8;
    float effective4346E0, effective4346E8, effective4346F0;
    float effective4346F8, effective434700, effective434708;
};
// Current main scene/flags, current348+104, state40 and FRAME-CACHED main+34
// are derived from canonical state at this stage, never supplied as duplicates.
bool beginCoupledBodyFrame49M(const CoupledBodyState49M&, const FrameReader49K&,
                              CoupledBodyState49M&) noexcept;
bool bindCoupledBodyFrame49M(const CoupledBodyState49M&, const PostPlayerAt217DE0_49K&,
    ControlRateLimits48B, const ControlBindingParameters48W&, CoupledBodyState49M&) noexcept;
bool refreshCoupledBodyFrame49M(const CoupledBodyState49M&, const PostBase49M&,
                                CoupledBodyState49M&) noexcept;
bool beginCoupledBodySubstep49M(const CoupledBodyState49M&, const SubstepSelectionContext49K&,
                                CoupledBodyState49M&) noexcept;
bool produceCoupledBodyForces49M(const CoupledBodyState49M&, const CoupledBodyParameters49M&,
    const PostWorld49M&, CoupledBodyState49M&) noexcept;
// Accept354 -> disabled348 response. No48H integration yet.
bool completeCoupledResponse354_49M(const CoupledBodyState49M&, const CoupledBodyParameters49M&,
    const PostResponse354_49M&, CoupledBodyState49M&) noexcept;
// live838==0 skips the ENTIRE secondary wrapper and advances to main integration.
// Otherwise wrapper forces ->49L prepare/correct/apply -> required368 boundary.
bool advanceCoupledSecondary49M(const CoupledBodyState49M&, const CoupledBodyParameters49M&,
    const SecondaryInputs49M&, CoupledNoiseContext49M&, CoupledBodyState49M&) noexcept;
// Supplied368 ->secondary48H.20C stays live;48H clears238. Root-ready scene100
// is represented by the supplied no-parent sceneC0 identity; no scene refresh
// callbacks or hierarchy traversal are invented.
bool completeCoupledResponse368_49M(const CoupledBodyState49M&, const CoupledBodyParameters49M&,
    const CoupledComponents49M&, CoupledBodyState49M&) noexcept;
// Current main1C0 impact gate AFTER368 ->main48H ->main20C clear.
bool integrateCoupledMain49M(const CoupledBodyState49M&, const CoupledBodyParameters49M&,
                             CoupledBodyState49M&) noexcept;
bool completeCoupledQuery49M(const CoupledBodyState49M&, const SuppliedQuery219120_49K&,
                             CoupledBodyState49M&) noexcept;
bool completeCoupledSceneConsumer49M(const CoupledBodyState49M&, const SceneConsumerObservation49K&,
                                     CoupledBodyState49M&) noexcept;
// Strict binary32/FE_TONEAREST, fixed storage, serial and allocation-free.
// Every bool transition is atomic on false, supports input/output aliasing,
// and checks its dedicated phase plus the retained49K schedule. External action
// queries and already-applied supplied effects cannot be rolled back.
}

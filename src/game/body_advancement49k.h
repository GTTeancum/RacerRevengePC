#pragma once
#include "advancement_selectors49k.h"
#include "body_wrapper48h.h"
#include "player_input48x.h"

namespace rr::game {
// Bounded main-body route only. These are explicit context observations, not
// defaults inferred from a no-contact fixture. The initialized body was stored
// at vehicle+340, registered with mask2, and has body+320=1.
struct MainBodyDomain49K {
    std::uint32_t body320, bodyChildCount, sceneChildCount, sceneObserverCount;
    bool sceneAttached, sceneHasParent, bodyEnabled;
    bool secondBodyPresent, override3C, resetRequested;
    std::uint32_t probe100;
    bool probe120Present, probe124Present;
};
struct MainBodyComponents49K {
    RootBodyState48H root;
    EngineState48D engine;
    AeroState48E aero;
    ControlModelBindings48W bindings;
    DisabledProbeState49K probe348;
    LateFeedback49K feedback;
    std::uint32_t body20C;
};
struct MainBodyParameters49K {
    BodyParameters48C body;
    EngineParameters48D engine;
    AeroParameters48E aero;
    Vector3_48C centerOffset;
    float probeLocal140[4], effective434260[4];
    float probe204, effective4342C8;
    Vector3_48C effective434290;
    // Exact values against which the original compares body+1C0 before223030.
    Vector3_48C effectiveImpactZero434630;
    float gravityForceX, gravityForceZ;
};
enum class MainBodyPhase49K {
    FrameReady, AwaitingPostPlayer, AwaitingPostBase, SubstepReady,
    AwaitingWorldPreparation, AwaitingResponse354, AwaitingQuery219120,
    AwaitingSceneConsumer
};
struct MainBodyState49K {
    MainBodyDomain49K domain;
    MainBodyComponents49K components;
    PlayerControlStep48X controls;
    float cachedSpeed34;
    Vector3_48C cachedAnglesE0;
    float frameFactor, liveSubstepScale;
    Schedule49K schedule;
    std::uint32_t substepIndex;
    float selectedGravity;
    EngineFlags49K selectedEngineFlags;
    std::uint32_t response354_3C, response354_44, vehicle380;
    std::uint32_t vehicleState40, applicationActive11C;
    float priorPoint7B0[4], currentPoint7C0[4];
    MainBodyPhase49K phase;
};
struct FrameReader49K {
    DualShockAxisBytes48X packet;
    PlayerReadContext48X context;
    PlayerActionCodes48X codes;
    PlayerActionProvider48X actions;
    // Prepared49I Ready factor, after clock/manager/root gates; not host seconds
    // silently substituted for49I. No timing callbacks are performed here.
    float preparedFactor;
};
// Reader1A1A20 called by player1A11E0 at1A15C0. Stops BEFORE opaque player
// stages and base228620. Device/action reads may already have occurred on failure.
bool beginMainBodyFrame49K(const MainBodyState49K&, const FrameReader49K&,
                           MainBodyState49K&) noexcept;
struct PostPlayerAt217DE0_49K {
    // Explicit post-player-stage values. Copying controls.player is a fixture
    // assumption, never a claim those intervening player calls were executed.
    PlayerReadResult48X player;
    ControlBindingInput48W bindingContext;
};
// Base228620 calls217DE0 at22877C only when vehicle+40 is neither3 nor4.
// Other states are rejected, rather than forcing the skipped binding call.
// Uses supplied post-player values, retained
// filter/D28 histories and OLD cachedSpeed34. Exactly once per admitted frame.
bool bindMainBodyFrame49K(const MainBodyState49K&, const PostPlayerAt217DE0_49K&,
    ControlRateLimits48B, const ControlBindingParameters48W&, MainBodyState49K&) noexcept;
struct PostBaseAtChildPhase49K {
    // Supplied effects after intervening base stages and at the scheduler's
    // child phase. Every represented external effect is explicit in this state.
    MainBodyComponents49K components;
    float liveSubstepScale;
    std::uint32_t liveVehicleState40, applicationActive11C;
};
// Body167B80 with+320=1: quaternion→E0/E4/E8, speed refresh, empty child router.
// NO integration. Scheduler childphase1A8304 precedes substep selection.
// Admits the advancing route only: app+11C=0 and vehicle+40 neither3 nor4.
bool refreshMainBodyFrame49K(const MainBodyState49K&, const PostBaseAtChildPhase49K&,
                            MainBodyState49K&) noexcept;
struct SubstepSelectionContext49K {
    float vehicle378, vehicle37C, effective434730, effective434738;
    std::uint32_t vehicle64;
    std::uint8_t vehicleB9C;
};
// Reads INCOMING348+104, frame cached speed and scene up.y. Flags follow gravity.
// Stops before first-substep +358/219420, whose side effects remain external.
bool beginMainBodySubstep49K(const MainBodyState49K&, const SubstepSelectionContext49K&,
                            MainBodyState49K&) noexcept;
enum class WorldPreparationPoint49K {
    FirstSubstep358And219420, LaterRequired358And219420, LaterSkipped358And219420
};
struct PostWorldPreparation49K {
    WorldPreparationPoint49K point;
    MainBodyComponents49K components;
    // Live force-stage inputs that are not established by48W or latefeedback.
    bool aeroSubtractWorldFlow;
    Vector3_48C aeroWorldFlow40;
    std::uint8_t liveApplicationGateByte58;
};
// Supplied post-world state→disabled348prepare→engine219C84→gravity219C90→
// conditional engine219CC0→aero219CD4. Pauses BEFORE mandatory354response219CE8.
// The caller must retain native-owned histories unless its external stage
// actually changes them. The library never substitutes a fake world callback.
bool produceMainBodyForces49K(const MainBodyState49K&, const MainBodyParameters49K&,
                             const PostWorldPreparation49K&, MainBodyState49K&) noexcept;
struct PostResponse354_49K {
    // Full post354 response snapshot, including body/load/correction state.
    // This call does not implement354, infer no contacts, or add fake impulses.
    MainBodyComponents49K components;
    std::uint32_t response3C, response44;
};
// Native disabled348response219CF4→reject unsupported impact223030 branch→
// root48H219E80→20Cclear219E94. Stops BEFORE mandatory219120/121920 world query.
bool completeMainBodyResponse49K(const MainBodyState49K&, const MainBodyParameters49K&,
                                const PostResponse354_49K&, MainBodyState49K&) noexcept;
struct SuppliedQuery219120_49K {
    std::uint8_t resultFlagsF8;
    float currentScenePoint7C0[4];
};
// Explicit completed external query result is mandatory. OnlyF8 bit5 clear is
// supported: original219400 copies current7C0 to prior7B0. Hits are rejected.
// Then vehicle+380 is set from354+44 and219F90..FCC feedback becomes the
// NEXT producer's mode flags. Auxiliary point BA0→BB0 at219F78..F8C is
// unrepresented; this is not a complete219810 implementation.
bool completeMainBodyQuery49K(const MainBodyState49K&, const SuppliedQuery219120_49K&,
                             MainBodyState49K&) noexcept;
struct SceneConsumerObservation49K { BodyMatrix48G consumedScene; };
// Caller supplies the transform actually delivered to the external scene
// consumer. No consumer, child graph or observer is invoked. This boundary only
// advances the diagnostic phase; opaque consumer mutation is unsupported.
bool completeMainBodySceneConsumer49K(const MainBodyState49K&,
    const SceneConsumerObservation49K&, MainBodyState49K&) noexcept;
// All public transitions require FE_TONEAREST, consistent with49I/J timing.
// The supplied state must be initialized and kept between calls. Resume gates
// check factor/count/index against the retained frame factor and live scale.
// All native transitions are fixed-size, serial, allocation-free and atomic on
// false; aliased state/output is supported. Use strict binary32/no fast math/FMA.
// External effects and completed read-only action queries cannot be rolled back.
}

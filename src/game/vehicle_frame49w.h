#pragma once
#include "plain_effect_tail50q.h"
#include "packed_vehicle_tail50r.h"
#include "vehicle_animation49t.h"
#include "chariot_lifecycle49v.h"
#include "graph_body49w.h"
#include "pod_shape49w.h"
#include "static_impact49y.h"
#include "impact_consumers49z.h"
#include "impact_aftermath50a.h"
#include "impact_effects50b.h"
#include "optional_probe_effects50p.h"
#include "impact_transitions50c.h"
#include "terrain_query_workspace48m.h"

namespace rr::game {
constexpr std::uint32_t vehicleFrameNodeCount49W=maxSceneNodes49P;
enum class VehicleFramePhase49W : std::uint32_t {
    Ready, PostPlayer, BeforeGraphics, PostGraphics, PostImpactConsumers, PostImpactRepair, AfterImpactBlock, BetweenBodyChildren,
    AfterBodyChildren, SubstepReady, PostWorld, Post354, Post368,
    MainIntegration, Query219120, PostImpactEffects50O, PostOptionalProbeEffects50P, PostPlainEffects50Q, PostPackedBytes50S, PostEngineBase50X
};
// These are supplied source-completed stages, never callbacks invented by the
// native implementation. All unrepresented effects are outside this bridge;
// represented graph/body/history/control/noise effects MUST be preserving.
// BB0.W is unknown and absent: external stages may not consume that word.
enum class PreservingCut49W : std::uint32_t {
    PlayerAndBaseBefore217DE0, Base2176E0BeforeGraphics,
    BaseAfterGraphicsThroughBA0ResetBeforeChildren,
    OtherChildrenBetween340And35C, OtherChildrenAfter35C, OtherChildrenBeforeMain340
};
struct PreservingStage49W {
    PreservingCut49W point;
    bool representedStatePreserved;
    bool omittedBB0WNotConsumed;
};
struct SceneTopology49W {
    SceneNodeIndex49P parent180=SceneNodeIndex49P::none;
    SceneNodeIndex49P firstChild184=SceneNodeIndex49P::none;
    SceneNodeIndex49P nextSibling188=SceneNodeIndex49P::none;
};
struct VehicleFrameBindings49W {
    VehicleAnimationBindings49T animation;
    VisualSceneBindings49R visual;
    ChariotLifecycleBindings49V lifecycle;
    SceneTopology49W topology[vehicleFrameNodeCount49W];
    GraphBodyDomain49W mainDomain,secondaryDomain;
    std::uint32_t sceneNodeCount=53; // Legacy Anakin default; capacity is larger.
};
struct VehicleFrameParameters49W {
    CoupledBodyParameters49M bodies;
    VehicleVisualParameters49R visual;
    ChariotLifecycleParameters49V lifecycle;
    SecondaryActivationGlobals49V activation;
    ControlRateLimits48B rates;
    ControlBindingParameters48W control;
    PodShapeSource49W shape;
    // Source-owned current track EnvDamage. Required only on an unskipped
    // impact call; constructor zeros are not a substitute for authored tuning.
    StaticImpactTuning49Y impactTuning;
    bool impactTuningBound;
};
// Supplemental live fields which have no owner in the retained body records.
// State is initialized once; no phase accepts a replacement body or scene pose.
struct VehicleFrameFields49W {
    std::uint32_t vehicle58,vehicle3C,vehicle834,vehicle844,vehicleCFC,vehicleD00;
    std::uint8_t applicationByte58,chariot191,chariot1B0,left191,right191;
    float objectD34_10,damage7D0,damage7D8,damage7D4,damage7DC;
    float gravity378,gravity37C; // sole live pair, observed at substep entry
    VisualVector49R impactBA0;
    Vector3_48C inverseImpactBB0XYZ; // BB0.W (sourceBBC) deliberately unknown/unrepresented
    float impactDirectionalD08[8]; // sole D08..D24 maxima owner; constructor zeros
    float defenseCF0; // current authored/profile-selected Defense, not a default
    float repairCF4; // current authored/profile-selected BaseRepairAmount
    std::uint8_t impactFlagsCD8;
};
// Only previously unrepresented consumer state lives here. Damage7D0..7DC,
// CFC/D00, partnerDuration10 and directional maxima keep their existing owners.
struct VehicleImpactSupplement49Z {
    float timers820[5],cooldown7E0,periodic7E8;
    std::uint32_t fieldD04,partnerId,partnerState;
    float partnerTimer08,partnerAmount0C,environmentalD38,environmentalD3C;
    float repairTimer7E4;
};
struct VehicleImpactEvents49Z {
    ImpactConsumerEvent49Z values[2];
    std::size_t count;
};
// Explicit enabled348 extension. Shared flags/150/1D0 remain solely in
// main.components.probe348. Only previously absent fields live here.
// stage:0 awaiting1B1800,1 prepared awaiting query,2 query committed,3 responded.
struct VehicleTerrainProbe50FP {
    bool enabled=false;
    std::uint8_t stage=0;
    Vector3_48C normal170{},previousPoint180{},previousNormal190{};
    std::array<std::uint32_t,16> collectedIdentities{};
    std::uint32_t currentCandidateIdentity=0;
    TerrainFollowParameters48G response{};
};
struct VehicleFrameState49W {
    SceneNode49P nodes[vehicleFrameNodeCount49W];
    CoupledBodyState49M bodies; // sole main,secondary,838,864,history/cache/joint owner
    SecondaryActivationSupplement49V mainSupplement,secondarySupplement;
    GraphBodyExtraLanes49W mainExtra,secondaryExtra;
    VehicleAnimationState49T animation;
    VehicleAnimationShared49T animationShared;
    VehicleVisualState49R visual;
    VisualShared49R visualShared;
    ChariotLifecycleState49V lifecycle;
    ChariotLifecycleShared49V lifecycleShared;
    CoupledNoiseContext49M noise; // exactly one serial49R->49V->physics owner
    PodShapeState49W shape;
    VehicleFrameFields49W fields;
    VehicleFramePhase49W phase;
    // Native coverage marker, not an original game field. A recorded impact
    // requires the actual postgraphics consumer before the next cache/reset.
    // The legacy preserving cut cannot clear this marker or erase maxima.
    std::uint32_t impactConsumerPending49Y;
    VehicleImpactSupplement49Z impactSupplement;
    std::uint32_t impactConsumersActive49Z;
    // Sole packed C00..C06 owner. Written completely by2291D0 before use.
    std::uint8_t packedC00[7];
    VehicleTerrainProbe50FP terrainProbe;
    std::uint32_t sceneNodeCount=53; // Active forest only; unused capacity is not consumed.
};
inline bool validVehicleSceneCount49W(const VehicleFrameState49W& state,
    const VehicleFrameBindings49W& bindings) noexcept {
    return state.sceneNodeCount&&state.sceneNodeCount<=vehicleFrameNodeCount49W&&
        state.sceneNodeCount==bindings.sceneNodeCount;
}
struct VehicleFrameWorkspace49W {
    VehicleFrameState49W next;
    VehicleAnimationWorkspace49T animation;
    VehicleVisualWorkspace49R visual;
    ChariotLifecycleWorkspace49V lifecycle;
    GraphBodyWorkspace49W body;
};
enum class VehicleFrameError49W : std::uint32_t {
    None,Storage,Phase,Admission,Graph,Projection,ExternalCut,Input,
    Animation,Visual,Lifecycle,Arithmetic,Body,Query,Shape
};
struct VehicleFrameDiagnostic49W {
    VehicleFrameError49W error;
    std::uint32_t sourceStage;
    GraphBodyDiagnostic49W body;
};
// Narrow scene/history-preserving external contact projection. XYZ velocities,
// loads/corrections/accumulators and their represented W lanes are permitted;
// all poses, caches, time864, derivatives, phases and topology remain native.
// All PostWorld/354/368 external stages must exclude consumption of omitted
// BB0.W, preserve graph/cache/timing/noise and all fields absent from this record.
struct BodyContactEffects49W {
    Vector3_48C velocity70,angularVelocityB0;
    BodyLoads48C loads;
    Vector3_48C positionCorrection1A0,rotationCorrection1B0,accumulator1C0,accumulator1D0;
    float velocity70W,angularVelocityB0W,force180W,torque190W;
    float positionCorrection1A0W,rotationCorrection1B0W,accumulator1C0W,accumulator1D0W;
    std::uint32_t body20C;
};
struct ContactEffects49W {
    BodyContactEffects49W main,secondary;
    PodSphereJoint49L joint;
    VisualVector49R impactBA0;
};
ContactEffects49W contactEffects49W(const VehicleFrameState49W&) noexcept;
enum class WorldCut49W : std::uint32_t {
    First358And219420,First358Secondary36CAnd219420,
    LaterRequired358And219420,LaterSkipped
};
struct PostWorld49W {
    WorldCut49W point;
    ContactEffects49W effects;
    bool aeroSubtractWorldFlow;
    Vector3_48C aeroWorldFlow40;
    std::uint8_t applicationGateByte58; // live observation updates sole fields.applicationByte58 owner
};
struct Post354_49W {
    ContactEffects49W effects;
    std::uint32_t response3C,response44,vehicle838;
};
// No arbitrary query point: position7C0 is derived from current main190 before
// the caller executes11CA80/11FF60/121920. Caller returns only its actual F8.
struct NoHitQuery49W { std::uint8_t resultFlagsF8; bool representedStatePreserved,omittedBB0WNotConsumed; };
// Source-completed movement query. Query/callback owners are retained outside
// this frame; only the selected point and actual F8 enter the native hit tail.
// No parent/observer guard is removed. The live source push distance is explicit.
struct MovementQueryCompletion50M {
 std::uint8_t resultFlagsF8;
 VisualVector49R selectedPoint;
 float pushDistance434710;
 bool representedStatePreserved,omittedBB0WNotConsumed;
};

// FrameReader49K is reused for packet/codes/provider/preparedFactor only. Its
// context values are derived from the canonical fields/player owner. Binding
// likewise derives source7D0/4 and cached34; substep B9C comes from player flags.
// Already-initialized B9C bit7 is required before basebinding (no217440).
// SubstepSelectionContext observes378/37C into the sole fields owner; the
// duplicate SecondaryInputs49M gravity members are ignored by49W. Immutable
// parameter/binding descriptors and source-effective constants must remain
// unchanged through a staged frame, including all external cuts.
bool readVehicleFrame49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const FrameReader49K&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool bindVehicleFrame49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const PreservingStage49W&,
    const PostPlayerAt217DE0_49K&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool graphicsVehicleFrame49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const PreservingStage49W&,
    VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
// Executes the real manager dispatch and222B20/2228D0 damage/timer consumers.
// Input manager/profile/player/vehicle64/tuning values must be authentic live
// owners. Factor, CD8,3C,40,58 and component presence are derived canonically;
// duplicated input members cannot override them. Events preserve source order
// for the real controller backend; this function never invokes that backend.
// PartnerId/timers and all other supplement state require source initialization
// (notably partnerId=-1 in an empty initialized partner record).
// Outer manager bit1 clear skips to AfterImpactBlock at228B40. Otherwise success
// stops at PostImpactConsumers before2288C8 repair; CD8's inner skip does not
// bypass repair. Both paths still need the remaining real base operations
// before BA0 reset/cache refresh. The legacy refresh cannot bypass either phase.
bool consumeVehicleImpacts49Z(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const ImpactConsumerInput49Z&,VehicleImpactEvents49Z&,
    VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
// Executes actual virtual+80 ->224370 immediately after the consumers. Reads
// canonical player.flagsB9C, factor, repairCF4 and bound component presence.
// Stops before the subsequent223540 effect and225C50 transition operations;
// no legacy preserving-cut bypass is admitted from PostImpactRepair.
bool repairVehicleImpacts50A(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool refreshMainVehicleFrame49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const PreservingStage49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool refreshSecondaryVehicleFrame49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const PreservingStage49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool scheduleVehicleFrame49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const PreservingStage49W&,float liveSubstepScale,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool beginVehicleSubstep49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const SubstepSelectionContext49K&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
// Cold opt-in only, from the ordinary Ready disabled topology. Parameters must
// come from actual348 setup; no default tuning or fabricated support is supplied.
bool enableVehicleTerrain50FP(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const TerrainPrepareState48M& initial,const TerrainFollowParameters48G&,
    VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
// PostWorld AFTER spring detection, BEFORE forces. Publishes actual48M numeric
// preparation to caller for348's own live membership update and limited query.
// Segment endpoints/mode are actual cold query ownership; old numeric world is
// ignored by48M. Output must be disjoint from all inputs/state/workspace.
bool prepareVehicleTerrain50FP(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const LocalSegment48I&,TerrainPrepareState48M&,
    VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
// Supply only a completed48M terrain result for that same prepared348 query.
// Caller owns genuine candidate order, filters, live-registration receipt and
// candidate-effect commit. This seam never substitutes a surface or does queries.
bool commitVehicleTerrainQuery50FP(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const TerrainQueryResult48M&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool produceVehicleForces49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const PostWorld49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool respondVehicle354_49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const Post354_49W&,const SecondaryInputs49M&,
    VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool respondVehicle368_49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const ContactEffects49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool integrateVehicleMain49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool finishVehicleQuery49W(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const NoHitQuery49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
// Bounded single-vehicle staged advancing diagnostic, not a complete original
// frame. Ordinary58=40=3C=0/app11C=0, body320=1, actual empty body component
// routes, null physical scene observers and current694.E0=0. Fixed53-node
// topology retains190->194. Initial main cache is ready; every external cut
// preserves scenes. Secondary may be dirty after49V. Each native call stages
// all state in disjoint caller workspace and is atomic on false; external reads
// and completed caller stages cannot be rolled back as a whole frame. Fixed
// storage, allocation-free, strict binary32 FE_TONEAREST, serial access and
// inherited finite/source-scratch domains. Workspace, diagnostics, descriptors,
// input records and persistent state are distinct initialized live storage.
bool finishVehicleMovementQuery50M(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 const VehicleFrameParameters49W&,const MovementQueryCompletion50M&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;

// Executes actual223540 after repair. Caller supplies live model/effect owner
// bindings and authored name metadata only; damage/timers/velocity/RNG derive
// from this frame. Ordered backend events are explicit outputs, never gameplay
// transition substitutes. Success stops before2179D0 and225C50; backend event
// semantics and the remaining base operations must be completed before reuse.
// All declared spans must be disjoint; failures leave frame/events/count intact.
bool effectsVehicleImpacts50O(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 const ImpactEffectsInput50B& ownerBindings,ImpactEffectsWorkspace50B&,
 ImpactEffectEvent50B*,std::size_t capacity,std::size_t& count,
 VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;

// Original2179D0 early-return/stop-only projection. Active probe work rejects
// before its first B9C write; no enabled path is replaced by a preserving cut.
bool optionalProbeVehicleImpacts50P(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 const OptionalProbeOwners50P&,OptionalProbeResult50P&,VehicleFrameWorkspace49W&,
 VehicleFrameDiagnostic49W&) noexcept;
// Executes ordinary225C50. Damage and BA8 derive from canonical frame owners.
// B9D is a separate byte, absent from the legacy frame; its sole current owner
// is explicit here. It must not be inferred from the eight-bit B9C input field.
// Threshold50 is an explicit live raw word, not a guessed default. Diagnostic
// decision may change on rejection; frame does not. A selected gameplay handler
// remains unsupported and is never emitted as a deferred effect/backend event.
bool decideVehicleImpacts50P(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 std::uint32_t liveThresholdBits50,std::uint8_t liveFlagsB9D,ImpactTransitionResult50C&,VehicleFrameWorkspace49W&,
 VehicleFrameDiagnostic49W&) noexcept;

// Ordinary228B40 attachment-null branch. Current effects flags must be refreshed
// after earlier backend events. Success stops before2291D0 and remaining base work.
bool plainEffectsVehicleTail50Q(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 const PlainEffectOwners50Q&,PlainEffectResult50Q&,VehicleFrameWorkspace49W&,
 VehicleFrameDiagnostic49W&) noexcept;
// Source2291D0 bound to canonical current owners. Configuration is the current
// resolved global entry; packet writes are atomic and stop before BA0 reset.
bool packVehicleFrame50S(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 bool configurationEntryNonzero,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
}

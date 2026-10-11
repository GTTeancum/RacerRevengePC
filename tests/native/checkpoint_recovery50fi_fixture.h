#pragma once
// Development HOST recovery policy. A prepared checkpoint creates a NEW native
// session; it never completes2255B0, resumes a partially executed frame, or
// changes the retained228BBC, damage-transition or50EN link-removal guards.
// Geometry is50ET/50CP.
#include "authored_runtime_ground50cp_fixture.h"
#include "track_navigation50eu.h"
#include "vehicle_frame49w.h"
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace checkpoint_recovery50fi_fixture {
using namespace rr::game;

enum class Error {
    None, Input, NativeCut, PlayerDomain, Navigation, Scene, Selection, Basis,
    Damage, GroundStorage, GroundProbe, GroundCandidate, GroundMiss,
    DamageCut, DamageDecision, PortalCut, PortalDecision, ManualBoundary
};
struct Diagnostic {
    Error error=Error::None;
    UndertrackDiagnostic50ET source{};
    ResetTreeStatus50AK groundStatus=ResetTreeStatus50AK::Complete;
    ResetProbePhase50AG groundPhase=ResetProbePhase50AG::AwaitingQuery;
    std::size_t owner=0,candidates=0;
    std::uint32_t attempts=0;
};
struct Damage {
    float damage7D0=0.f,damage7D4=0.f,damage7D8=0.f,damage7DC=0.f;
};
enum class Policy { Undertrack, DamageFreshVehicle, PortalLinkRemoval, ManualCheckpoint };
struct Plan {
    // Anchor+authored lift and the source2255B0/216AF0 basis, BEFORE the
    // independent actual terrain query. No current-position/nearest fallback.
    BodyMatrix48G sourcePose{};
    UndertrackSelection50ET selection{};
    TrackNavigationProgress50EU progress{};
    Damage damage{};
    float lift=0.f;
    std::size_t zoneCount=0;
    CollectionRef50BX primaryZone=0;
    std::uint8_t nodeIndex78=0;
    bool prepared=false;
    Policy policy=Policy::Undertrack;
    // Actual unsupported50C receipt, retained only for DamageFreshVehicle.
    // It is never interpreted as permission to execute the destructive target.
    ImpactTransitionResult50C damageDecision{ImpactTransitionStatus50C::noTransition,0,0,0};
    // Actual failed50EN receipt, retained only for PortalLinkRemoval. This is
    // an explicit HOST restart reason, never a hit/miss or completed prefix.
    LiveZoneResult50EN portalDecision{};
};

namespace detail {
inline bool fail(Diagnostic& d,Error error) noexcept {d.error=error;return false;}
inline bool validIndex(SceneNodeIndex49P index) noexcept {
    const auto i=static_cast<std::int32_t>(index);
    return i>=0&&i<static_cast<std::int32_t>(vehicleFrameNodeCount49W);
}
inline bool validDamage(const Damage& damage) noexcept {
    // Same finite, non-subnormal input domain as the existing repair/consumer
    // leaves. Validate actual damage before every explicit recovery policy.
    const float values[]={damage.damage7D0,damage.damage7D4,
                          damage.damage7D8,damage.damage7DC};
    for(float value:values) {
        const auto bits=std::bit_cast<std::uint32_t>(value);
        const auto exponent=bits&0x7f800000u;
        if(exponent==0x7f800000u||(!exponent&&(bits&0x7fffffu)))return false;
    }
    return true;
}
inline bool validPose(const BodyMatrix48G& pose) noexcept {
    for(float value:pose.v)if(!std::isfinite(value))return false;
    return std::bit_cast<std::uint32_t>(pose.v[3])==0&&
           std::bit_cast<std::uint32_t>(pose.v[7])==0&&
           std::bit_cast<std::uint32_t>(pose.v[11])==0&&
           std::bit_cast<std::uint32_t>(pose.v[15])==0x3f800000u;
}
inline bool selectedDamageTransition(const ImpactTransitionResult50C& decision) noexcept {
    if(decision.status!=ImpactTransitionStatus50C::unsupportedTransition)return false;
    switch(decision.sourcePC) {
    case 0x225d10:return decision.vtableSlot==0x88&&decision.originalTarget==0x2261b0;
    case 0x225d9c:case 0x225e28:
        return decision.vtableSlot==0x8c&&decision.originalTarget==0x226750;
    case 0x225e7c:return decision.vtableSlot==0x90&&decision.originalTarget==0x227370;
    case 0x225ec0:return decision.vtableSlot==0x94&&decision.originalTarget==0x227a40;
    default:return false;
    }
}
inline bool selectedPortalBoundary(const LiveZoneResult50EN& decision) noexcept {
    // The source281320 guard is reached only after these operations completed.
    // Other failures must not be relabeled as a recoverable portal boundary.
    return decision.status==LiveZoneStatus50EN::UnsupportedLinkRemoval&&
        decision.boundsStatus==CollectionBoundsStatus48S::Complete&&
        decision.geometryStatus==PortalGeometryStatus50EO::Complete&&
        decision.registryStatus==SearchStorageStatus50BS::Complete;
}
inline bool validPolicy(const Plan& plan) noexcept {
    if(plan.policy==Policy::Undertrack||plan.policy==Policy::ManualCheckpoint)return true;
    if(plan.policy==Policy::PortalLinkRemoval)return selectedPortalBoundary(plan.portalDecision);
    return plan.policy==Policy::DamageFreshVehicle&&selectedDamageTransition(plan.damageDecision)&&
        std::bit_cast<std::uint32_t>(plan.damage.damage7D0)==0&&
        std::bit_cast<std::uint32_t>(plan.damage.damage7D4)==0&&
        std::bit_cast<std::uint32_t>(plan.damage.damage7D8)==0&&
        std::bit_cast<std::uint32_t>(plan.damage.damage7DC)==0;
}
inline bool validPlan(const Plan& plan) noexcept {
    return validPolicy(plan)&&plan.prepared&&plan.zoneCount&&plan.zoneCount<=256&&
        static_cast<std::size_t>(plan.nodeIndex78)<plan.zoneCount&&
        plan.primaryZone==static_cast<CollectionRef50BX>(plan.nodeIndex78)+1u&&
        !plan.selection.writesC80&&
        plan.selection.anchorNode==plan.progress.reset.vehicleC8C&&
        std::isfinite(plan.lift)&&plan.lift>0.f&&
        validPose(plan.sourcePose)&&validDamage(plan.damage);
}
// Common source geometry path; each public entry point separately authenticates
// its own reached native failure or explicit Ready-boundary host request before
// entering. Never forge a cut or advance navigation for a host reset.
inline bool prepareCheckpoint(const TrackNavigation50EU& map,
    const TrackNavigationProgress50EU& progress,const VehicleFrameState49W& state,
    const VehicleFrameBindings49W& bindings,float lift,const LiveZoneWorld50EN& world,
    Policy policy,const ImpactTransitionResult50C* damageDecision,
    const LiveZoneResult50EN* portalDecision,Plan& out,Diagnostic& d) noexcept {
    if(state.fields.vehicle58||state.fields.vehicle3C||
       !state.bodies.main.domain.sceneAttached||state.bodies.main.domain.sceneHasParent)
        return detail::fail(d,Error::PlayerDomain);
    if(!map.gatesPrepared||!map.indicesPrepared||!map.track.count||
       map.track.count>maxUndertrackNodes50ET||!world.portalHeads||
       !world.zoneCount||world.zoneCount>256)
        return detail::fail(d,Error::Navigation);
    const auto index=bindings.lifecycle.main190;
    if(!detail::validIndex(index))return detail::fail(d,Error::Scene);
    const auto& main=state.nodes[static_cast<unsigned>(index)];
    if(main.parent180!=SceneNodeIndex49P::none||!std::isfinite(main.localC0.v[13])||
       (policy==Policy::Undertrack&&!(main.localC0.v[13]<0.f)))
        return detail::fail(d,policy==Policy::Undertrack?Error::NativeCut:Error::Scene);
    std::array<SceneNode49P,vehicleFrameNodeCount49W> nodes{};
    std::memcpy(nodes.data(),state.nodes,sizeof state.nodes);
    const SceneGraph49P graph{nodes.data(),vehicleFrameNodeCount49W};
    if(!validateSceneGraph49P(graph)||!refreshSceneWorld49P(graph,index))
        return detail::fail(d,Error::Scene);
    UndertrackInput50ET input{};
    std::memcpy(input.worldPosition,nodes[static_cast<unsigned>(index)].world100.v+12,12);
    std::memcpy(input.localPosition,main.localC0.v+12,12);
    input.worldPosition[3]=input.localPosition[3]=1.f;
    input.lift7EC=lift;
    Plan candidate{};
    if(!selectUndertrackReposition50ET(map.track,progress.reset,input,
                                      candidate.selection,d.source))
        return detail::fail(d,Error::Selection);
    if(candidate.selection.writesC80||candidate.selection.anchorNode>=map.track.count)
        return detail::fail(d,Error::Navigation);
    candidate.nodeIndex78=map.nodeIndex78[candidate.selection.anchorNode];
    if(static_cast<std::size_t>(candidate.nodeIndex78)>=world.zoneCount)
        return detail::fail(d,Error::Navigation);
    // This is50EN's established native ordinal+1 ref translation, not a guest
    // pointer, guessed point-in-zone, or replacement index producer.
    candidate.primaryZone=static_cast<CollectionRef50BX>(candidate.nodeIndex78)+1u;
    candidate.zoneCount=world.zoneCount;
    if(!prepareUndertrackResetPose50ET(candidate.selection.positionBC0,
        candidate.selection.headingBD0,state.lifecycleShared.up434650.v,
        candidate.sourcePose,d.source))return detail::fail(d,Error::Basis);
    candidate.progress=progress;
    candidate.damage={state.fields.damage7D0,state.fields.damage7D4,
                      state.fields.damage7D8,state.fields.damage7DC};
    if(!detail::validDamage(candidate.damage))return detail::fail(d,Error::Damage);
    candidate.policy=policy;
    if(policy==Policy::DamageFreshVehicle) {
        if(!damageDecision||!selectedDamageTransition(*damageDecision))
            return detail::fail(d,Error::DamageDecision);
        candidate.damageDecision=*damageDecision;
        // Explicit host fresh-vehicle policy. This does not mutate/repair the
        // damaged native frame or claim its destructive handler completed.
        candidate.damage={};
    }else if(policy==Policy::PortalLinkRemoval) {
        if(!portalDecision||!selectedPortalBoundary(*portalDecision))
            return detail::fail(d,Error::PortalDecision);
        candidate.portalDecision=*portalDecision;
    }
    candidate.lift=lift;candidate.prepared=true;
    if(!detail::validPlan(candidate))return detail::fail(d,Error::Input);
    out=candidate;return true;
}

} // namespace detail

// Explicit development-HOST request at an ordinary completed-frame boundary.
// This is not a native failure or the original2255B0 action: no cut receipt,
// negative-Y requirement, or partially executed frame is admitted. Selection,
// current damage/navigation retention, and subsequent actual ground preflight
// are shared with recovery. A held input must not generate repeated requests.
// The source state is read-only and false preserves out.
inline bool prepareManualCheckpoint(const TrackNavigation50EU& map,
    const TrackNavigationProgress50EU& progress,const VehicleFrameState49W& state,
    const VehicleFrameBindings49W& bindings,float lift,const LiveZoneWorld50EN& world,
    Plan& out,Diagnostic& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(lift)||lift<=0.f)
        return detail::fail(d,Error::Input);
    if(state.phase!=VehicleFramePhase49W::Ready)
        return detail::fail(d,Error::ManualBoundary);
    return detail::prepareCheckpoint(map,progress,state,bindings,lift,world,
                                     Policy::ManualCheckpoint,nullptr,nullptr,out,d);
}

// Call only after the genuine native ExternalCut at228BBC. The original frame
// stays byte-for-byte unchanged:28B2B0's required world-cache refresh is applied
// to a bounded copy of its scene graph. Source input uses refreshed world190
// and original local190, plus the existing lifecycle shared434650 up vector.
// All records/spans must be separately backed; output is unchanged on false.
inline bool prepare(const TrackNavigation50EU& map,
    const TrackNavigationProgress50EU& progress,const VehicleFrameState49W& state,
    const VehicleFrameBindings49W& bindings,const VehicleFrameDiagnostic49W& cut,
    float lift,const LiveZoneWorld50EN& world,Plan& out,Diagnostic& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(lift)||lift<=0.f)
        return detail::fail(d,Error::Input);
    if(cut.error!=VehicleFrameError49W::ExternalCut||cut.sourceStage!=0x228bbcu||
       state.phase!=VehicleFramePhase49W::PostPackedBytes50S)
        return detail::fail(d,Error::NativeCut);
    return detail::prepareCheckpoint(map,progress,state,bindings,lift,world,
                                     Policy::Undertrack,nullptr,nullptr,out,d);
}

// Separate HOST policy for a failed decideVehicleImpacts50P. Admit only an
// actual selected destructive50C target at its real reached frame phase. The
// thresholdBits must be exactly that supplied to the native call; reinspection
// reads the unchanged canonical damage, BA8 and B9D bit0 and reproduces its receipt.
// Source checkpoint/zone/ground semantics are identical to undertrack recovery,
// but the NEW vehicle intentionally starts with four zero damage lanes. The old
// damaged frame is untouched. This is not native wreck/detachment continuation.
inline bool prepareDamageRecovery(const TrackNavigation50EU& map,
    const TrackNavigationProgress50EU& progress,const VehicleFrameState49W& state,
    const VehicleFrameBindings49W& bindings,const ImpactTransitionResult50C& decision,
    const VehicleFrameDiagnostic49W& cut,std::uint32_t thresholdBits,
    float lift,const LiveZoneWorld50EN& world,Plan& out,Diagnostic& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(lift)||lift<=0.f)
        return detail::fail(d,Error::Input);
    if(state.phase!=VehicleFramePhase49W::PostOptionalProbeEffects50P||
       cut.error!=VehicleFrameError49W::ExternalCut||cut.sourceStage!=decision.sourcePC)
        return detail::fail(d,Error::DamageCut);
    if(!detail::selectedDamageTransition(decision))return detail::fail(d,Error::DamageDecision);
    ImpactTransitionInput50C input{};
    const float* damage[]={&state.fields.damage7D0,&state.fields.damage7D4,
                           &state.fields.damage7D8,&state.fields.damage7DC};
    for(unsigned i=0;i<4;++i)std::memcpy(input.damage7D0+i,damage[i],4);
    std::memcpy(&input.impactBA8,&state.fields.impactBA0.v[2],4);
    std::memcpy(&input.threshold50,&thresholdBits,4);
    //225CF8 consumes bit0 only, represented by this same canonical frame owner.
    input.flagsB9D=static_cast<std::uint32_t>(state.fields.flagB9D0);
    const auto actual=inspectImpactTransition50C(input);
    if(actual.status!=decision.status||actual.sourcePC!=decision.sourcePC||
       actual.vtableSlot!=decision.vtableSlot||actual.originalTarget!=decision.originalTarget)
        return detail::fail(d,Error::DamageDecision);
    return detail::prepareCheckpoint(map,progress,state,bindings,lift,world,
                                     Policy::DamageFreshVehicle,&decision,nullptr,out,d);
}

// Separate HOST policy for the actual failed50EN prefix in the current
// PostWorld spring/terrain query paths. Pass the returned receipt immediately;
// do not fabricate it from a generic query failure or retry the failed frame.
// No negative-Y requirement or invented228BBC diagnostic: the genuine current
// checkpoint selector, copied scene and damage/navigation retention are shared.
//50EN already preserves its live membership on this failure; this routine never
// touches those owners or supplies unlink semantics. Ground must still succeed
// before the host may discard the old session. False preserves out.
inline bool preparePortalRecovery(const TrackNavigation50EU& map,
    const TrackNavigationProgress50EU& progress,const VehicleFrameState49W& state,
    const VehicleFrameBindings49W& bindings,const LiveZoneResult50EN& decision,
    float lift,const LiveZoneWorld50EN& world,Plan& out,Diagnostic& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(lift)||lift<=0.f)
        return detail::fail(d,Error::Input);
    if(state.phase!=VehicleFramePhase49W::PostWorld)
        return detail::fail(d,Error::PortalCut);
    if(!detail::selectedPortalBoundary(decision))
        return detail::fail(d,Error::PortalDecision);
    return detail::prepareCheckpoint(map,progress,state,bindings,lift,world,
                                     Policy::PortalLinkRemoval,nullptr,&decision,out,d);
}

// Fallible equivalent of the50CP cold probe, with a supplied SOURCE checkpoint
// pose rather than the grid pose. Owners, spheres, mesh placements, category
// masks, candidate order, endpoint shortening and all hit heights are real.
// Unlike50CP's diagnostic need(), no reject exits the process. False preserves
// out; only caller-owned stack scratch and diagnostics may change. At most20
// source probe requests and64 actual owners are admitted. No heap allocation.
inline bool resolveGround(const authored_runtime_ground50cp_fixture::ColdScene& scene,
    const Plan& plan,std::uint32_t floorMask,std::uint32_t mode,
    std::uint32_t* stack,std::size_t capacity,BodyMatrix48G& out,
    Diagnostic& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!detail::validPlan(plan)||!floorMask||mode>1||
       std::bit_cast<std::uint32_t>(scene.lift)!=std::bit_cast<std::uint32_t>(plan.lift)||
       scene.world.zones.size()!=plan.zoneCount)return detail::fail(d,Error::Input);
    if(!stack||!scene.stackCount||capacity<scene.stackCount||
       reinterpret_cast<std::uintptr_t>(stack)%alignof(std::uint32_t)||
       scene.owners.empty()||scene.owners.size()>64)
        return detail::fail(d,Error::GroundStorage);
    std::uint32_t position[4]{};
    for(unsigned i=0;i<4;++i)position[i]=std::bit_cast<std::uint32_t>(plan.sourcePose.v[12+i]);
    ResetProbe50AG probe{};
    if(!beginResetProbe50AG(position,std::bit_cast<std::uint32_t>(plan.lift),probe))
        return detail::fail(d,Error::GroundProbe);
    while(probe.phase==ResetProbePhase50AG::AwaitingQuery) {
        if(d.attempts>=20)return detail::fail(d,Error::GroundProbe);
        ++d.attempts;
        SpringVector4_48N start{},end{};
        std::memcpy(start.v,probe.start,16);std::memcpy(end.v,probe.end,16);
        bool hit=false;std::uint32_t height=0;
        for(std::size_t i=0;i<scene.owners.size();++i) {
            const auto& owner=scene.owners[i];
            if(!(owner.instance.category&floorMask))continue;
            d.owner=i;++d.candidates;
            ResetCandidateResult50AM result{};
            d.groundStatus=queryResetCandidate50AM(start,end,owner.instance.world,
                {{owner.instance.sphere.center.v[0],owner.instance.sphere.center.v[1],
                  owner.instance.sphere.center.v[2]}},owner.instance.sphere.radius,
                1,owner.mesh,mode,stack,capacity,result);
            if(d.groundStatus!=ResetTreeStatus50AK::Complete)
                return detail::fail(d,Error::GroundCandidate);
            if(result.hit) {
                hit=true;end=result.point;height=std::bit_cast<std::uint32_t>(end.v[1]);
            }
        }
        if(!acceptResetProbe50AG(probe,hit,height))return detail::fail(d,Error::GroundProbe);
        d.groundPhase=probe.phase;
    }
    //50AG exposes an exhausted staged value for its original caller. A host
    // checkpoint must never convert that unsupported result into fake ground.
    if(probe.phase!=ResetProbePhase50AG::Hit)return detail::fail(d,Error::GroundMiss);
    BodyMatrix48G grounded=plan.sourcePose;
    for(unsigned i=0;i<4;++i)grounded.v[12+i]=std::bit_cast<float>(probe.staged[i]);
    if(!detail::validPose(grounded))return detail::fail(d,Error::GroundProbe);
    out=grounded;return true;
}

// Cold continuation only, AFTER new50EU map/AI owners are prepared and AFTER
// resolveGround succeeded. Caller has not entered a native frame. The fresh
// session keeps new input/clock/noise/physics/effect owners. Its damage lanes are
// preserved for Undertrack/PortalLinkRemoval/ManualCheckpoint, zero for
// DamageFreshVehicle.
// No stale body/query/
// cache pointers or pending events are copied.
// CA0 is intentionally rebased to the new main190 local pose: the host teleport
// must not be presented to1A0DA0 as an actual gate-crossing movement segment.
// False preserves fields/progress/CA0. Grounded pose and plan are read-only.
inline bool restoreContinuity(const Plan& plan,VehicleFrameFields49W& fields,
    TrackNavigationProgress50EU& progress,float (&ca0)[4],
    const BodyMatrix48G& grounded,Diagnostic& d) noexcept {
    d={};
    if(!detail::validPlan(plan)||!detail::validPose(grounded)||fields.vehicle58||fields.vehicle3C)
        return detail::fail(d,Error::Input);
    // The accepted ground query changes translation Y only. Do not accept a
    // mismatched pose as though it had been resolved for this checkpoint.
    for(unsigned i=0;i<16;++i)if(i!=13&&
        std::bit_cast<std::uint32_t>(grounded.v[i])!=
        std::bit_cast<std::uint32_t>(plan.sourcePose.v[i]))return detail::fail(d,Error::Input);
    fields.damage7D0=plan.damage.damage7D0;fields.damage7D4=plan.damage.damage7D4;
    fields.damage7D8=plan.damage.damage7D8;fields.damage7DC=plan.damage.damage7DC;
    progress=plan.progress;
    std::memcpy(ca0,grounded.v+12,16);
    return true;
}

inline const char* errorText(Error error) noexcept {
    switch(error) {
    case Error::None:return "none";
    case Error::Input:return "invalid checkpoint input";
    case Error::NativeCut:return "not the retained native undertrack cut";
    case Error::PlayerDomain:return "unsupported player scene owner";
    case Error::Navigation:return "source navigation or zone index unavailable";
    case Error::Scene:return "source scene refresh rejected";
    case Error::Selection:return "source checkpoint selector rejected";
    case Error::Basis:return "source checkpoint basis rejected";
    case Error::Damage:return "damage outside native consumer domain";
    case Error::GroundStorage:return "ground query storage unavailable";
    case Error::GroundProbe:return "source ground probe rejected";
    case Error::GroundCandidate:return "actual terrain candidate query rejected";
    case Error::GroundMiss:return "no actual terrain ground hit";
    case Error::DamageCut:return "not a reached native damage-transition cut";
    case Error::DamageDecision:return "unsupported or mismatched damage-transition receipt";
    case Error::PortalCut:return "not a reached live portal-query phase";
    case Error::PortalDecision:return "not the native unsupported link-removal receipt";
    case Error::ManualBoundary:return "manual checkpoint requires a normal Ready boundary";
    }
    return "unknown checkpoint failure";
}
}

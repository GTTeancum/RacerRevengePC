#pragma once
// Source LostAI E8 ->2255B0 ->virtual78/216A30 continuation at real PostWorld.
// Distinct from original-AI16EAA0 dispatch, with no FE8/dispatch-counter writes.
#include "../../src/game/ai_runtime50iv.h"
#include "../../src/game/undertrack_reset50ew.h"
#include "../../src/game/pod_live_query50iz.h"
#include "../../src/game/pod_scene_rebase50fb.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "original_ai_reset_ground_bridge.h"
#include "original_ai_physical_recovery.h"
#include "../../src/game/source_trigger_entry50mb.h"
#include "../../src/game/trigger_actions50in.h"
#include "../../src/game/track_navigation_view50mb.h"
#include <array>
#include <bit>
#include <cstdio>
#include <cstring>
#include <limits>

namespace native_source_trigger_reset50MB {
using namespace rr::game;
namespace visibility=original_gc_gameplay_lighting_candidate;
enum class Stage : std::uint8_t {
    Idle,Select,Placement,BasePrefix,RemoveVisibility,RemoveCollision,
    InitialPhysics,PreparePod,AttachVisibility,AttachCollision,BeginGround,
    Ground,GroundDiagnostic,FinishPhysics,FinishVisibility,Complete
};
struct Invocation {
    SourceTriggerActionCall50MB call{};
    LostAiAction50IN action{};
    std::uint32_t rosterSlot=0,racer60=0,collider690=0;
    const void* sourceOwner=nullptr;
    std::uint64_t sourceGeneration=0;
};
inline bool sameInvocation(const Invocation& a,const Invocation& b) noexcept {
    return a.call.querySerial==b.call.querySerial&&a.call.candidateOrdinal==b.call.candidateOrdinal&&
        a.call.source.triggerCollider==b.call.source.triggerCollider&&a.call.source.otherCollider==b.call.source.otherCollider&&
        a.call.source.actionIndex==b.call.source.actionIndex&&a.call.source.entering==b.call.source.entering&&
        a.call.source.sourceSite==b.call.source.sourceSite&&
        a.rosterSlot==b.rosterSlot&&a.racer60==b.racer60&&a.collider690==b.collider690&&
        a.sourceOwner==b.sourceOwner&&a.sourceGeneration==b.sourceGeneration&&
        a.action.targetNodeIndex==b.action.targetNodeIndex&&a.action.reset==b.action.reset;
}
struct State {
    Stage stage=Stage::Idle;
    Invocation invocation{};
    bool eventBound=false;
    std::uint64_t completed=0;
    std::uint32_t rosterSlot=0,racer60=0;
    // Continuation's originalC80 input survives its source225640 store. This
    // is not an alternate progress authority; all other progression is live.
    UndertrackProgress50ET selectionProgress{};
    UndertrackSelection50ET selection{};
    // Sole represented BC0/BD0/C30/C34 produced by this2255B0, including the
    // earlier actual water-clamped BC4 and later actual ground-result BC4.
    // No spawn/grid values seed these fields.
    UndertrackPlacement50ET placement{};
    UndertrackRegistryProgress50EW registry{};
    ResetProbe50AG probe{};
    ResetGroundResult50IZ ground{};
    //21725C is the first represented850 writer. Prior value is not consumed.
    std::uint32_t vehicle850=0;
    bool vehicle850Known=false;
};
using Models=native_original_ai_recovery::Models;
struct Diagnostic {
    std::uint32_t sourcePC=0;
    const char* message="original-AI physical reset has not been requested";
    UndertrackDiagnostic50ET placement{};
    UndertrackResetResult50EW physics{};
    visibility::Diagnostic visibility{};
    ResetGroundDiagnostic50IZ ground{};
};
struct Owners {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Real sourceD40 presence is required for this admitted ALL8/native-audio
    // path. No controller1A0240/player prefix is called. D44 belongs to the
    // actual session PodRaceAudioManager row, not a bank-local temporary.
    std::uint8_t& aiEmitterD40Flags22C;
    std::uint32_t& materialD44Field24C;
    TrackNavigationView50EU aiTrack;
    CollisionResultBindingOwner50MB& resultBinding;
    const SourceOuterResultLease50MB& outerResult;
    const UndertrackPhysicsDomain50EW& height;
    const SecondaryActivationGlobals49V& activation;
    const ResetLifecycleGlobals50AT& lifecycle;
    const VisualVector49R& targetRelative2F0;
    const UndertrackQueryOwners50EW& queries;
    ZoneLifecycle50CL& collisionLifecycle;
    LiveZoneWorld50EN& collisionWorld;
    LiveZoneWorkspace50EN& collisionWorkspace;
    original_ai_reset_ground_bridge::GroundBridge& ground;
    Models& models;
    const ResetEffectBindings50BK& effects;
    EffectOwner50BE* effectOwners=nullptr;
    std::size_t effectOwnerCount=0;
};
inline bool fail(Diagnostic& d,std::uint32_t pc,const char* message) noexcept {
    d.sourcePC=pc;d.message=message;return false;
}
// Serial within the selected racer's real bank visit. All views are concrete
// live owners; this function performs the operations itself. Source-completed
// slices/cursors survive failures. No earlier AI navigation/packing/repair/
//16E830 or completed reset slice is replayed. No allocation or guest execution.
inline bool resumePhysicalAfterTarget50MB(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const Invocation& invocation,State& state,Owners& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    diagnostic={};
    constexpr auto domain=UndertrackResetDomain50EW::SourceTriggerAiPostWorld;
    if(!invocation.sourceOwner||!invocation.sourceGeneration||!invocation.call.querySerial||
       !invocation.call.source.entering||!invocation.action.reset||!invocation.racer60||!invocation.collider690||
       invocation.call.source.otherCollider!=invocation.collider690||
       frame.phase!=VehicleFramePhase49W::PostWorld||frame.fields.vehicle58!=1u||
       !frame.fields.nativeAiOwnersBound50IV||frame.fields.vehicle3C||
       frame.bodies.main.phase!=MainBodyPhase49K::AwaitingWorldPreparation||
       frame.bodies.phase!=CoupledBodyPhase49M::AwaitingWorldPreparation||
       !owners.queries.query[0].type2||!owners.queries.query[0].type2->filter||
       owners.queries.query[0].type2->filter->identity!=invocation.collider690||
       !owners.resultBinding.outerQueryCurrent(owners.outerResult,*owners.queries.query[0].type2))
        return fail(diagnostic,0x1b283cu,"source LostAI reset is not at the actual main690 E8 PostWorld boundary");
    if(state.eventBound){
        if(!sameInvocation(state.invocation,invocation))
            return fail(diagnostic,0x1b283cu,"different source event cannot replace retained reset state");
        if(state.stage==Stage::Complete)return true;
    }else{
        if(state.stage!=Stage::Idle||progress.reset.vehicleC80!=invocation.action.targetNodeIndex)
            return fail(diagnostic,0x1b282cu,"actual once-only target C80 write is absent");
        state.invocation=invocation;state.eventBound=true;state.rosterSlot=invocation.rosterSlot;state.racer60=invocation.racer60;
    }
    if(!bindings.podLiveQuery50IZ||bindings.podLiveQuery50IZ->source!=&parameters.shape||
       bindings.podLiveQuery50IZ->live.expectedObjectRef!=owners.queries.query[1].object||
       &bindings.podLiveQuery50IZ->live.registrationE0!=owners.queries.query[1].membership||
       &bindings.podLiveQuery50IZ->live.lifecycle!=&owners.collisionLifecycle||
       &bindings.podLiveQuery50IZ->live.world!=&owners.collisionWorld||
       &bindings.podLiveQuery50IZ->live.workspace!=&owners.collisionWorkspace)
        return fail(diagnostic,0x21699cu,"actual registered694 continuation binding is missing");
    visibility::ResetVisibilityLease lease;
    const visibility::SourceTriggerVisibilityIdentity50MB visibilityIdentity{
        invocation.sourceOwner,invocation.sourceGeneration,invocation.call.querySerial,
        invocation.call.candidateOrdinal,invocation.call.source.actionIndex,invocation.racer60};
    if(!owners.visibility.beginSourceTriggerResetVisibility50MB(state.rosterSlot,frame,bindings,parameters,
        &owners.cables254Field50,visibilityIdentity,lease,diagnostic.visibility))
        return fail(diagnostic,diagnostic.visibility.sourcePC,"actual source racer7F0/PodCables reset lease rejected");
    auto slice=[&](UndertrackResetResult50EW result,const char* message) noexcept {
        diagnostic.physics=result;
        return result.status==UndertrackResetStatus50EW::SourceSliceComplete||
            fail(diagnostic,result.sourcePC,message);
    };
    for(;;)switch(state.stage){
    case Stage::Idle: {
        if(!owners.aiTrack.runtimeValid()||!owners.aiTrack.indicesPrepared()||!owners.aiTrack.gatesPrepared()||
           owners.aiTrack.role()!=NavigationRole50MB::Ai||!owners.aiTrack.sameWorld(owners.collisionWorld)||
           progress.reset.vehicleC80>=owners.aiTrack.count())
            return fail(diagnostic,0x225620u,"actual AI spline/currentC80 is unavailable");
        const auto& node=owners.aiTrack.track().nodes()[progress.reset.vehicleC80];
        if(!node.nextCount||node.next[0]>=owners.aiTrack.count())
            return fail(diagnostic,0x225634u,"actual AI Next0 is unavailable");
        state.selectionProgress=progress.reset;
        progress.reset.vehicleC80=node.next[0]; //225640 BEFORE geometry/callbacks.
        state.stage=Stage::Select;break;
    }
    case Stage::Select: {
        const auto main=static_cast<std::int32_t>(bindings.lifecycle.main190);
        if(main<0||static_cast<std::uint32_t>(main)>=frame.sceneNodeCount||
           !refreshSceneWorld49P({frame.nodes,frame.sceneNodeCount},bindings.lifecycle.main190))
            return fail(diagnostic,0x225694u,"source reset main190 world refresh rejected");
        UndertrackInput50ET input{};input.vehicle58=frame.fields.vehicle58;
        std::memcpy(input.worldPosition,frame.nodes[main].world100.v+12,16);
        std::memcpy(input.localPosition,frame.nodes[main].localC0.v+12,16);
        input.lift7EC=parameters.visual.verticalLimit7EC;
        if(!selectUndertrackReposition50ET(owners.aiTrack.track(),state.selectionProgress,input,
            state.selection,diagnostic.placement))
            return fail(diagnostic,diagnostic.placement.sourcePC,"source2255B0 geometry rejected");
        state.stage=Stage::Placement;break;
    }
    case Stage::Placement: {
        std::array<std::uint32_t,256> visibilityRows{},collisionRows{};
        const auto count=lease.zoneCount();
        if(!count||count>visibilityRows.size()||count!=owners.collisionLifecycle.zoneCount||
           count!=owners.collisionWorld.zoneCount)
            return fail(diagnostic,0x225a74u,"actual application4C0/4C4 table extents differ");
        for(std::size_t ordinal=0;ordinal<count;++ordinal){
            visibilityRows[ordinal]=lease.zoneRef4C0(lease.zoneKey4C0(ordinal));
            //50CL/50EN real collision rows retain authored ordinal+1. Verify
            //both actual method refs before using that table entry.
            const auto& zone=owners.collisionLifecycle.storage.views.zones[ordinal];
            if(!visibilityRows[ordinal]||!zone.firstMethod||!zone.secondMethod)
                return fail(diagnostic,0x225a74u,"actual application zone table row is unbound");
            collisionRows[ordinal]=static_cast<std::uint32_t>(ordinal+1u);
        }
        if(!bindUndertrackPlacement50ET(state.selection,owners.aiTrack,visibilityRows.data(),
            collisionRows.data(),count,state.placement,diagnostic.placement))
            return fail(diagnostic,diagnostic.placement.sourcePC?diagnostic.placement.sourcePC:0x225a38u,
                "source AI node78/application placement binding rejected");
        state.stage=Stage::BasePrefix;break;
    }
    case Stage::BasePrefix:
        if(!resetUndertrackBasePrefix50ET(&owners.aiEmitterD40Flags22C,&owners.materialD44Field24C))
            return fail(diagnostic,0x216a80u,"actual AI D40/D44 base reset rejected");
        state.stage=Stage::RemoveVisibility;break;
    case Stage::RemoveVisibility:
        if(!owners.visibility.removeAiResetVisibility(lease,diagnostic.visibility))
            return fail(diagnostic,diagnostic.visibility.sourcePC,"source7F0 removal rejected");
        state.stage=Stage::RemoveCollision;break;
    case Stage::RemoveCollision:
        if(!slice(removeUndertrackCollisionOwners50EW(frame,owners.queries,owners.collisionLifecycle,
            owners.collisionWorld,owners.collisionWorkspace,state.registry,domain),"source collision removal rejected"))return false;
        state.stage=Stage::InitialPhysics;break;
    case Stage::InitialPhysics:
        // Mutable placement overload publishes source216BCC with the initial
        // scenes atomically. BeginGround must consume this SAME BC0 owner.
        if(!slice(placeInitialUndertrackPhysics50EW(frame,bindings,state.placement,owners.height,
            owners.activation,owners.lifecycle,workspace,domain),"source initial reset physics/height owner rejected"))return false;
        state.stage=Stage::PreparePod;break;
    case Stage::PreparePod: {
        LiveZoneBounds50EN prepared{};
        if(!slice(prepareUndertrackPodQuery50EW(frame,bindings,parameters,prepared,domain),
            "source694 preparation rejected"))return false;
        state.stage=Stage::AttachVisibility;break;
    }
    case Stage::AttachVisibility:
        if(!owners.visibility.attachAiResetVisibility(lease,state.placement.vehicleC30,diagnostic.visibility))
            return fail(diagnostic,diagnostic.visibility.sourcePC,"source7F0 attachment rejected");
        state.stage=Stage::AttachCollision;break;
    case Stage::AttachCollision:
        if(!slice(attachUndertrackCollisionOwners50EW(frame,bindings,parameters,state.placement,
            owners.queries,owners.collisionLifecycle,owners.collisionWorld,owners.collisionWorkspace,
            state.registry,domain),"source collision attachment rejected"))return false;
        state.stage=Stage::BeginGround;break;
    case Stage::BeginGround: {
        std::uint32_t words[4];std::memcpy(words,state.placement.positionBC0,sizeof words);
        if(!beginResetProbe50AG(words,std::bit_cast<std::uint32_t>(parameters.visual.verticalLimit7EC),state.probe))
            return fail(diagnostic,0x216e8cu,"source reset probe initialization rejected");
        state.ground={};state.ground.point={{0.f,0.f,0.f,1.f}};
        state.ground.normal={{0.f,0.f,0.f,1.f}};
        state.stage=Stage::Ground;break;
    }
    case Stage::Ground: {
        if(state.probe.phase!=ResetProbePhase50AG::AwaitingQuery){
            state.stage=Stage::GroundDiagnostic;break;
        }
        SpringVector4_48N start{},end{};std::memcpy(start.v,state.probe.start,16);
        std::memcpy(end.v,state.probe.end,16);
        if(owners.ground.querySourceBound50MB(state.placement.vehicleC34,start,end,state.ground,owners.resultBinding,owners.outerResult)!=ResetGroundStatus50IZ::Complete){
            diagnostic.ground=owners.ground.diagnostic().query;
            return fail(diagnostic,diagnostic.ground.sourcePC,"actual reset124CB0 ground query rejected");
        }
        if(!acceptResetProbe50AG(state.probe,state.ground.hit,
            std::bit_cast<std::uint32_t>(state.ground.point.v[1])))
            return fail(diagnostic,0x216f28u,"source reset ground continuation rejected");
        break;
    }
    case Stage::GroundDiagnostic:
        if(state.probe.phase==ResetProbePhase50AG::Exhausted){
            // Exact original3B2C50 diagnostic at216F48. Exhaustion keeps its
            //source lift result; it is not reported as a hit or a y-clamp.
            std::fputs("WARNING! PodVehicle::Reset: Could not reset vehicle above the ground!\n",stderr);
        }
        //216F50..216F5C publish BC4 only AFTER the exhaustion diagnostic.
        std::memcpy(state.placement.positionBC0,state.probe.staged,16);
        state.stage=Stage::FinishPhysics;break;
    case Stage::FinishPhysics:
        if(!slice(finishUndertrackPhysics50EW(frame,bindings,parameters,state.placement,state.probe,
            owners.targetRelative2F0,owners.models.bindings.data(),owners.models.count,
            owners.effects,owners.effectOwners,owners.effectOwnerCount,state.vehicle850,workspace,domain),
            "source final reset scenes/bounds/effects rejected"))return false;
        state.vehicle850Known=true;state.stage=Stage::FinishVisibility;break;
    case Stage::FinishVisibility:
        if(!coherentPodLiveQuery50IZ(frame.shape,*bindings.podLiveQuery50IZ)||
           state.registry.removed!=4u||state.registry.attached!=4u)
            return fail(diagnostic,0x2169a4u,"actual694/690/7A4/348 reset ownership is incomplete");
        if(!owners.visibility.finishAiResetVisibility(lease,diagnostic.visibility))
            return fail(diagnostic,diagnostic.visibility.sourcePC,"actual reset visibility publication rejected");
        state.stage=Stage::Complete;break;
    case Stage::Complete:
        ++state.completed;diagnostic.sourcePC=0;
        diagnostic.message="source LostAI physical reset returned in the same PostWorld collision query";return true;
    }
}
inline SourceTriggerActionResult50MB resumeAfterLostAiTarget50MB(VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& bindings,const VehicleFrameParameters49W& parameters,
    TrackNavigationProgress50EU& progress,const Invocation& invocation,State& state,
    Owners& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    const bool complete=resumePhysicalAfterTarget50MB(frame,bindings,parameters,progress,invocation,state,owners,workspace,diagnostic);
    return {complete?SourceTriggerActionStatus50MB::Complete:SourceTriggerActionStatus50MB::Blocked,
        diagnostic.sourcePC,diagnostic.physics.ownerOffset};
}
} // namespace native_source_trigger_reset50MB

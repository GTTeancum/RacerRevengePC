#pragma once
// Genuine Player2255B0 ->1A24F0 ->216A30 from228BBC or controller1A17A4.
// No AI role/phase substitution, host checkpoint or grid placement occurs.
#include "original_ai_physical_recovery.h"
#include "../../src/game/original_human_engine_reset50lb.h"
#include "../../src/game/vehicle_base_tail50x.h"

namespace native_original_human_recovery {
using namespace rr::game;
namespace visibility=original_gc_gameplay_lighting_candidate;
// These three/five model-bound descriptors are the SAME physical-model
// projection shared by both216A30 callers, not AI state or an AI controller.
using Models=native_original_ai_recovery::Models;
enum class Stage : std::uint8_t {
    Idle,Select,Placement,PlayerPrefix,Controller,BasePrefix,RemoveVisibility,
    RemoveCollision,InitialPhysics,PreparePod,AttachVisibility,AttachCollision,
    BeginGround,Ground,GroundDiagnostic,FinishPhysics,FinishVisibility,Complete
};
struct State {
    Stage stage=Stage::Idle;
    UndertrackResetDomain50EW domain=UndertrackResetDomain50EW::PlayerPostPackedBytes50S;
    std::uint64_t serial=0,physicalCompleted=0;
    UndertrackProgress50ET selectionProgress{};
    UndertrackSelection50ET selection{};
    UndertrackPlacement50ET placement{};
    UndertrackRegistryProgress50EW registry{};
    ResetProbe50AG probe{};
    ResetGroundResult50IZ ground{};
    std::uint32_t vehicle850=0;
    bool vehicle850Known=false;
};
struct Diagnostic {
    std::uint32_t sourcePC=0;
    const char* message="human physical reset has not been requested";
    UndertrackDiagnostic50ET placement{};
    UndertrackResetResult50EW physics{};
    visibility::Diagnostic visibility{};
    ResetGroundDiagnostic50IZ ground{};
};
struct Owners {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const TrackNavigation50EU& mapTrack;
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
// Serial inside the actual human arena. Source-completed slices survive
// failure; callers must not replay Player input/navigation/packing. The camera
// tail runs separately only AFTER this lease has released the visibility world.
// No allocation, guest execution or replacement controls are used.
inline bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    diagnostic={};
    const bool controller=domain==UndertrackResetDomain50EW::PlayerControllerReady50LC;
    if((!controller&&domain!=UndertrackResetDomain50EW::PlayerPostPackedBytes50S)||!serial||
       frame.phase!=(controller?VehicleFramePhase49W::Ready:VehicleFramePhase49W::PostPackedBytes50S)||
       frame.fields.vehicle58!=0u||frame.fields.vehicle3C||!owners.controllerD40.bound())
        return fail(diagnostic,controller?0x1a17a4u:0x228bbcu,"human reset caller/phase ownership is unavailable");
    if(!bindings.podLiveQuery50IZ||bindings.podLiveQuery50IZ->source!=&parameters.shape||
       bindings.podLiveQuery50IZ->live.expectedObjectRef!=owners.queries.query[1].object||
       &bindings.podLiveQuery50IZ->live.registrationE0!=owners.queries.query[1].membership||
       &bindings.podLiveQuery50IZ->live.lifecycle!=&owners.collisionLifecycle||
       &bindings.podLiveQuery50IZ->live.world!=&owners.collisionWorld||
       &bindings.podLiveQuery50IZ->live.workspace!=&owners.collisionWorkspace)
        return fail(diagnostic,0x21699cu,"actual registered694 continuation binding is missing");
    if(state.serial!=serial||state.domain!=domain){
        if(state.stage!=Stage::Idle&&state.stage!=Stage::Complete)
            return fail(diagnostic,0x228bbcu,"a different human reset cannot replace this unfinished call");
        const auto completed=state.physicalCompleted;const auto vehicle850=state.vehicle850;
        const auto known=state.vehicle850Known;state={};state.physicalCompleted=completed;
        state.vehicle850=vehicle850;state.vehicle850Known=known;state.serial=serial;state.domain=domain;
    }
    if(state.stage==Stage::Complete)return true;
    visibility::ResetVisibilityLease lease;
    const bool borrowed=controller?
        owners.visibility.beginPlayerInputResetVisibility(0u,frame,bindings,parameters,
            &owners.cables254Field50,lease,diagnostic.visibility):
        owners.visibility.beginPlayerResetVisibility(0u,frame,bindings,parameters,
            &owners.cables254Field50,lease,diagnostic.visibility);
    if(!borrowed)
        return fail(diagnostic,diagnostic.visibility.sourcePC,"actual racer7F0/PodCables reset lease rejected");
    auto slice=[&](UndertrackResetResult50EW result,const char* message) noexcept {
        diagnostic.physics=result;
        return result.status==UndertrackResetStatus50EW::SourceSliceComplete||
            fail(diagnostic,result.sourcePC,message);
    };
    for(;;)switch(state.stage){
    case Stage::Idle:
        if(!owners.mapTrack.indicesPrepared||!owners.mapTrack.gatesPrepared)
            return fail(diagnostic,0x2255e0u,"actual human map progression/indices are unavailable");
        state.selectionProgress=progress.reset;state.stage=Stage::Select;break;
    case Stage::Select: {
        const auto main=static_cast<std::int32_t>(bindings.lifecycle.main190);
        if(main<0||static_cast<std::uint32_t>(main)>=frame.sceneNodeCount||
           !refreshSceneWorld49P({frame.nodes,frame.sceneNodeCount},bindings.lifecycle.main190))
            return fail(diagnostic,0x225694u,"source reset main190 world refresh rejected");
        UndertrackInput50ET input{};input.vehicle58=frame.fields.vehicle58;
        std::memcpy(input.worldPosition,frame.nodes[main].world100.v+12,16);
        std::memcpy(input.localPosition,frame.nodes[main].localC0.v+12,16);
        input.lift7EC=parameters.visual.verticalLimit7EC;
        if(!selectUndertrackReposition50ET(owners.mapTrack.track,state.selectionProgress,input,
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
        UndertrackTableBindings50ET tables{};
        if(!undertrackBindings50EU(owners.mapTrack,visibilityRows.data(),collisionRows.data(),count,tables)||
           !bindUndertrackPlacement50ET(state.selection,tables,state.placement,diagnostic.placement))
            return fail(diagnostic,diagnostic.placement.sourcePC?diagnostic.placement.sourcePC:0x225a38u,
                "source human map node78/application placement binding rejected");
        state.stage=Stage::PlayerPrefix;break;
    }
    case Stage::PlayerPrefix:
        resetUndertrackPlayerPrefix50ET(owners.playerPrefix);state.stage=Stage::Controller;break;
    case Stage::Controller:
        if(!owners.controllerD40.reset1A0240())
            return fail(diagnostic,0x1a259cu,"actual human D40 controller reset rejected");
        state.stage=Stage::BasePrefix;break;
    case Stage::BasePrefix:
        if(!resetUndertrackBasePrefix50ET(&owners.controllerD40.flags22C(),&owners.materialD44Field24C))
            return fail(diagnostic,0x216a80u,"actual human D40/D44 base reset rejected");
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
        if(owners.ground.query(state.placement.vehicleC34,start,end,state.ground)!=ResetGroundStatus50IZ::Complete){
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
        ++state.physicalCompleted;diagnostic.sourcePC=0;
        diagnostic.message="human1A24F0/216A30 returned; actual225B0C camera tail remains required";
        return true;
    }
}
// The actual228BBC continuation supplies its own saved pre-reset base pose.
inline bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    if(!base.pending||base.frame!=&frame||base.bindings!=&bindings)
        return fail(diagnostic,0x228bbcu,"human undertrack reset has no actual base continuation");
    return resume2255B0(frame,bindings,parameters,progress,base.serial,
        UndertrackResetDomain50EW::PlayerPostPackedBytes50S,state,owners,workspace,diagnostic);
}
// Distinct1A17A4 event call. It never manufactures a base-frame cursor.
inline bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    return resume2255B0(frame,bindings,parameters,progress,serial,
        UndertrackResetDomain50EW::PlayerControllerReady50LC,state,owners,workspace,diagnostic);
}
} // namespace native_original_human_recovery

#pragma once
// Concrete original-AI16EAA0 ->2255B0 ->virtual78/216A30 continuation.
// No player override, camera reset, checkpoint, crash or damage-reset route.
#include "../../src/game/ai_runtime50iv.h"
#include "../../src/game/undertrack_reset50ew.h"
#include "../../src/game/pod_live_query50iz.h"
#include "../../src/game/pod_scene_rebase50fb.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "original_ai_reset_ground_bridge.h"
#include <array>
#include <bit>
#include <cstdio>
#include <cstring>
#include <limits>
#include <type_traits>
#include "../../src/game/track_navigation_view50mb.h"

namespace native_original_ai_recovery {
using namespace rr::game;
namespace visibility=original_gc_gameplay_lighting_candidate;
enum class Stage : std::uint8_t {
    Idle,Select,Placement,BasePrefix,RemoveVisibility,RemoveCollision,
    InitialPhysics,PreparePod,AttachVisibility,AttachCollision,BeginGround,
    Ground,GroundDiagnostic,FinishPhysics,FinishVisibility,Complete
};
struct State {
    Stage stage=Stage::Idle;
    std::uint64_t scalarResetSerial=0,completed=0;
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
struct Models {
    std::array<std::array<ResetBoundNode50BA,maxSceneNodes49P>,5> nodes{};
    std::array<ModelBoundsResult49O,5> results{};
    std::array<UndertrackModelBounds50EW,5> bindings{};
    std::uint32_t count=0;
};
// Cold once per actual selected rig, using its complete existing source-node
// map. Three or five graphics owners share the same two physical bodies.
inline bool bindColdModels(const VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& binding,const PodSceneRebaseModel50FB* input,
    std::uint32_t count,Models& output) noexcept {
    if(frame.phase!=VehicleFramePhase49W::Ready||frame.bodies.main.frameFactor!=0.f||
       !input||count!=(binding.visual.extra1C0Present?5u:3u))return false;
    const SceneNodeIndex49P roots[]{binding.lifecycle.model1B4,binding.lifecycle.right1B8,
        binding.lifecycle.left1BC,binding.visual.bottomRight1C0,binding.visual.bottomLeft1C4};
    for(unsigned model=0;model<count;++model){const auto& source=input[model];
        if(source.root!=roots[model]||!source.sourceNodes||
           source.sourceNodeCount!=source.hierarchy.nodeCount||
           !source.sourceNodeCount||source.sourceNodeCount>maxSceneNodes49P)return false;
        for(unsigned index=0;index<source.sourceNodeCount;++index){
            rr::assets::PsgNode49O node{};
            if(!rr::assets::readPsgNode49O(source.hierarchy,index,node))return false;
            const auto actual=source.sourceNodes[index];const auto ni=static_cast<std::int32_t>(actual);
            if(ni<0||static_cast<std::uint32_t>(ni)>=frame.sceneNodeCount||
               node.parent>=static_cast<std::int32_t>(index))return false;
            const auto parent=node.parent<0?source.root:source.sourceNodes[node.parent];
            if(frame.nodes[ni].parent180!=parent)return false;
            auto& bound=output.nodes[model][index];bound.node=actual;
            for(unsigned axis=0;axis<3;++axis){bound.center140.v[axis]=node.bounds[axis];
                bound.halfExtent150.v[axis]=node.bounds[axis+3];}
        }
        output.bindings[model]={output.nodes[model].data(),source.sourceNodeCount,&output.results[model]};
    }
    output.count=count;return true;
}
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
    const TrackNavigation50EU& aiTrack;
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
// The selected final view is retained by value and cannot be reseated inside
// this owner. Keep the same immutable selection across an unfinished State;
// State itself remains the original bank-safe scalar continuation.
struct Owners50MB {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Real sourceD40 presence is required for this admitted ALL8/native-audio
    // path. No controller1A0240/player prefix is called. D44 belongs to the
    // actual session PodRaceAudioManager row, not a bank-local temporary.
    std::uint8_t& aiEmitterD40Flags22C;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50EU aiTrack;
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
// TC-CONSUMER-BEGIN 0
struct Owners50TC {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Real sourceD40 presence is required for this admitted ALL8/native-audio
    // path. No controller1A0240/player prefix is called. D44 belongs to the
    // actual session PodRaceAudioManager row, not a bank-local temporary.
    std::uint8_t& aiEmitterD40Flags22C;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50TC aiTrack;
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
// TC-CONSUMER-END 0

// TB-CONSUMER-BEGIN 0
struct Owners50TB {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Real sourceD40 presence is required for this admitted ALL8/native-audio
    // path. No controller1A0240/player prefix is called. D44 belongs to the
    // actual session PodRaceAudioManager row, not a bank-local temporary.
    std::uint8_t& aiEmitterD40Flags22C;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50TB aiTrack;
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
// TB-CONSUMER-END 0

// Exact-type navigation access adapters. Legacy calls retain their original
// fixed owner and table-binding order; MB calls use only issued strong views.
namespace ordinary_navigation_detail50MB {
inline bool indicesPrepared(const TrackNavigation50EU& t) noexcept {return t.indicesPrepared;}
inline bool indicesPrepared(const TrackNavigationView50EU& t) noexcept {return t.indicesPrepared();}
// TC-CONSUMER-BEGIN 1
inline bool indicesPrepared(const TrackNavigationView50TC& t) noexcept {return t.indicesPrepared();}
// TC-CONSUMER-END 1

// TB-CONSUMER-BEGIN 1
inline bool indicesPrepared(const TrackNavigationView50TB& t) noexcept {return t.indicesPrepared();}
// TB-CONSUMER-END 1

inline bool gatesPrepared(const TrackNavigation50EU& t) noexcept {return t.gatesPrepared;}
inline bool gatesPrepared(const TrackNavigationView50EU& t) noexcept {return t.gatesPrepared();}
// TC-CONSUMER-BEGIN 2
inline bool gatesPrepared(const TrackNavigationView50TC& t) noexcept {return t.gatesPrepared();}
// TC-CONSUMER-END 2

// TB-CONSUMER-BEGIN 2
inline bool gatesPrepared(const TrackNavigationView50TB& t) noexcept {return t.gatesPrepared();}
// TB-CONSUMER-END 2

inline std::uint32_t count(const TrackNavigation50EU& t) noexcept {return t.track.count;}
inline std::uint32_t count(const TrackNavigationView50EU& t) noexcept {return t.count();}
// TC-CONSUMER-BEGIN 3
inline std::uint32_t count(const TrackNavigationView50TC& t) noexcept {return t.count();}
// TC-CONSUMER-END 3

// TB-CONSUMER-BEGIN 3
inline std::uint32_t count(const TrackNavigationView50TB& t) noexcept {return t.count();}
// TB-CONSUMER-END 3

inline const UndertrackNode50ET& node(const TrackNavigation50EU& t,std::uint32_t index) noexcept {
    return t.track.nodes[index];
}
inline const UndertrackNode50ET& node(const TrackNavigationView50EU& t,std::uint32_t index) noexcept {
    return t.track().nodes()[index];
}
// TC-CONSUMER-BEGIN 4
inline const UndertrackNode50ET& node(const TrackNavigationView50TC& t,std::uint32_t index) noexcept {
    return t.track().nodes()[index];
}
// TC-CONSUMER-END 4

// TB-CONSUMER-BEGIN 4
inline const UndertrackNode50ET& node(const TrackNavigationView50TB& t,std::uint32_t index) noexcept {
    return t.track().nodes()[index];
}
// TB-CONSUMER-END 4

inline const UndertrackTrack50ET& track(const TrackNavigation50EU& t) noexcept {return t.track;}
inline UndertrackTrackView50ET track(const TrackNavigationView50EU& t) noexcept {return t.track();}
// TC-CONSUMER-BEGIN 5
inline UndertrackTrackView50TC track(const TrackNavigationView50TC& t) noexcept {return t.track();}
// TC-CONSUMER-END 5

// TB-CONSUMER-BEGIN 5
inline UndertrackTrackView50TB track(const TrackNavigationView50TB& t) noexcept {return t.track();}
// TB-CONSUMER-END 5

inline bool bindPlacement(const UndertrackSelection50ET& selection,const TrackNavigation50EU& navigation,
    const std::uint32_t* visibilityRows,const std::uint32_t* collisionRows,std::size_t count,
    UndertrackPlacement50ET& placement,UndertrackDiagnostic50ET& diagnostic) noexcept {
    UndertrackTableBindings50ET tables{};
    return undertrackBindings50EU(navigation,visibilityRows,collisionRows,count,tables)&&
        bindUndertrackPlacement50ET(selection,tables,placement,diagnostic);
}
inline bool bindPlacement(const UndertrackSelection50ET& selection,const TrackNavigationView50EU& navigation,
    const std::uint32_t* visibilityRows,const std::uint32_t* collisionRows,std::size_t count,
    UndertrackPlacement50ET& placement,UndertrackDiagnostic50ET& diagnostic) noexcept {
    return bindUndertrackPlacement50ET(selection,navigation,visibilityRows,collisionRows,count,placement,diagnostic);
}
// TC-CONSUMER-BEGIN 6
inline bool bindPlacement(const UndertrackSelection50ET& selection,const TrackNavigationView50TC& navigation,
    const std::uint32_t* visibilityRows,const std::uint32_t* collisionRows,std::size_t count,
    UndertrackPlacement50ET& placement,UndertrackDiagnostic50ET& diagnostic) noexcept {
    return bindUndertrackPlacement50ET(selection,navigation,visibilityRows,collisionRows,count,placement,diagnostic);
}
// TC-CONSUMER-END 6

// TB-CONSUMER-BEGIN 6
inline bool bindPlacement(const UndertrackSelection50ET& selection,const TrackNavigationView50TB& navigation,
    const std::uint32_t* visibilityRows,const std::uint32_t* collisionRows,std::size_t count,
    UndertrackPlacement50ET& placement,UndertrackDiagnostic50ET& diagnostic) noexcept {
    return bindUndertrackPlacement50ET(selection,navigation,visibilityRows,collisionRows,count,placement,diagnostic);
}
// TB-CONSUMER-END 6

} // namespace ordinary_navigation_detail50MB
// Constant-true legacy admission leaves every original guard/effect ordered.
inline bool navigationReadyOrdinaryShared50MB(const Owners&) noexcept {return true;}
inline bool navigationReadyOrdinaryShared50MB(const Owners50MB& owners) noexcept {
    return owners.aiTrack.runtimeValid()&&owners.aiTrack.role()==NavigationRole50MB::Ai&&
        owners.aiTrack.sameWorld(owners.collisionWorld);
}
// TC-CONSUMER-BEGIN 7
inline bool navigationReadyOrdinaryShared50MB(const Owners50TC& owners) noexcept {
    return owners.aiTrack.runtimeValid()&&owners.aiTrack.role()==NavigationRole50TC::Ai&&
        owners.aiTrack.sameWorld(owners.collisionWorld);
}
// TC-CONSUMER-END 7

// TB-CONSUMER-BEGIN 7
inline bool navigationReadyOrdinaryShared50MB(const Owners50TB& owners) noexcept {
    return owners.aiTrack.runtimeValid()&&owners.aiTrack.role()==NavigationRole50TB::Ai&&
        owners.aiTrack.sameWorld(owners.collisionWorld);
}
// TB-CONSUMER-END 7


inline bool fail(Diagnostic& d,std::uint32_t pc,const char* message) noexcept {
    d.sourcePC=pc;d.message=message;return false;
}
// Serial within the selected racer's real bank visit. All views are concrete
// live owners; this function performs the operations itself. Source-completed
// slices/cursors survive failures. No earlier AI navigation/packing/repair/
//16E830 or completed reset slice is replayed. No allocation or guest execution.
template<class OwnersT>
    requires (std::is_same_v<OwnersT,Owners>||std::is_same_v<OwnersT,Owners50MB>||std::is_same_v<OwnersT,Owners50TC>||std::is_same_v<OwnersT,Owners50TB>)
inline bool resumeOrdinaryShared50MB(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    AiRuntimeState50IV& ai,AiRuntimeReceipt50IV& receipt,State& state,
    OwnersT& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    diagnostic={};
    // Validate the final source role/world before any continuation or visibility
    // effect, including when resuming after selection/placement already ran.
    if(!navigationReadyOrdinaryShared50MB(owners))
        return fail(diagnostic,0x225620u,"ordinary ai reset requires its final same-world MB navigation view");
    constexpr auto domain=UndertrackResetDomain50EW::OriginalAiPostPlayer;
    if(!ai.initialized||!ai.physicalReset2255B0Pending||!ai.scalarResets16E830||
       ai.state.aiStateFE8!=0u||frame.phase!=VehicleFramePhase49W::PostPlayer||
       frame.fields.vehicle58!=1u||!frame.fields.nativeAiOwnersBound50IV||frame.fields.vehicle3C)
        return fail(diagnostic,0x16eaa0u,"AI physical-reset continuation is not at its real PostPlayer boundary");
    if(!bindings.podLiveQuery50IZ||bindings.podLiveQuery50IZ->source!=&parameters.shape||
       bindings.podLiveQuery50IZ->live.expectedObjectRef!=owners.queries.query[1].object||
       &bindings.podLiveQuery50IZ->live.registrationE0!=owners.queries.query[1].membership||
       &bindings.podLiveQuery50IZ->live.lifecycle!=&owners.collisionLifecycle||
       &bindings.podLiveQuery50IZ->live.world!=&owners.collisionWorld||
       &bindings.podLiveQuery50IZ->live.workspace!=&owners.collisionWorkspace)
        return fail(diagnostic,0x21699cu,"actual registered694 continuation binding is missing");
    if(state.scalarResetSerial!=ai.scalarResets16E830){
        if(state.stage!=Stage::Idle&&state.stage!=Stage::Complete)
            return fail(diagnostic,0x16eaa0u,"a different scalar reset cannot replace an unfinished physical reset");
        const auto completed=state.completed;const auto vehicle850=state.vehicle850;
        const auto known=state.vehicle850Known;state={};state.completed=completed;
        state.vehicle850=vehicle850;state.vehicle850Known=known;
        state.scalarResetSerial=ai.scalarResets16E830;state.rosterSlot=ai.managerIndexFE0;
        state.racer60=ai.racer60;
    }
    if(state.rosterSlot!=ai.managerIndexFE0||state.racer60!=ai.racer60)
        return fail(diagnostic,0x16eaa0u,"physical reset belongs to another racer");
    visibility::ResetVisibilityLease lease;
    if(!owners.visibility.beginAiResetVisibility(state.rosterSlot,frame,bindings,parameters,
        &owners.cables254Field50,lease,diagnostic.visibility))
        return fail(diagnostic,diagnostic.visibility.sourcePC,"actual racer7F0/PodCables reset lease rejected");
    auto slice=[&](UndertrackResetResult50EW result,const char* message) noexcept {
        diagnostic.physics=result;
        return result.status==UndertrackResetStatus50EW::SourceSliceComplete||
            fail(diagnostic,result.sourcePC,message);
    };
    for(;;)switch(state.stage){
    case Stage::Idle: {
        if(!ordinary_navigation_detail50MB::indicesPrepared(owners.aiTrack)||!ordinary_navigation_detail50MB::gatesPrepared(owners.aiTrack)||
           progress.reset.vehicleC80>=ordinary_navigation_detail50MB::count(owners.aiTrack))
            return fail(diagnostic,0x225620u,"actual AI spline/currentC80 is unavailable");
        const auto& node=ordinary_navigation_detail50MB::node(owners.aiTrack,progress.reset.vehicleC80);
        if(!node.nextCount||node.next[0]>=ordinary_navigation_detail50MB::count(owners.aiTrack))
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
        if(!selectUndertrackReposition50ET(ordinary_navigation_detail50MB::track(owners.aiTrack),state.selectionProgress,input,
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
        if(!ordinary_navigation_detail50MB::bindPlacement(state.selection,owners.aiTrack,
            visibilityRows.data(),collisionRows.data(),count,state.placement,diagnostic.placement))
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
        //16EAA8 branches directly to16EAE8..16EAF8. FE8 is already0 from
        //16E830, but this dispatch DOES NOT enter ordinary state0 driving.
        ai.physicalReset2255B0Pending=false;++state.completed;++ai.dispatchedFrames;
        receipt.physicalResetPending=false;receipt.dispatched=true;receipt.stateFE8=ai.state.aiStateFE8;
        receipt.sourceBoundary=0;diagnostic.sourcePC=0;diagnostic.message="original-AI physical reset returned in the same PostPlayer frame";
        return true;
    }
}
inline bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    AiRuntimeState50IV& ai,AiRuntimeReceipt50IV& receipt,State& state,
    Owners& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    return resumeOrdinaryShared50MB(frame,bindings,parameters,progress,ai,receipt,state,owners,workspace,diagnostic);
}
inline bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    AiRuntimeState50IV& ai,AiRuntimeReceipt50IV& receipt,State& state,
    Owners50MB& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    return resumeOrdinaryShared50MB(frame,bindings,parameters,progress,ai,receipt,state,owners,workspace,diagnostic);
}
// TC-CONSUMER-BEGIN 8
inline bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    AiRuntimeState50IV& ai,AiRuntimeReceipt50IV& receipt,State& state,
    Owners50TC& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    return resumeOrdinaryShared50MB(frame,bindings,parameters,progress,ai,receipt,state,owners,workspace,diagnostic);
}
// TC-CONSUMER-END 8

// TB-CONSUMER-BEGIN 8
inline bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    AiRuntimeState50IV& ai,AiRuntimeReceipt50IV& receipt,State& state,
    Owners50TB& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept {
    return resumeOrdinaryShared50MB(frame,bindings,parameters,progress,ai,receipt,state,owners,workspace,diagnostic);
}
// TB-CONSUMER-END 8

} // namespace native_original_ai_recovery

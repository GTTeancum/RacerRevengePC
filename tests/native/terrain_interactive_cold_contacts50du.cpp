// Cold stage bodies retain their original order and borrowed execution arena.
#include "native_last_failure50pd.h"
#include "terrain_interactive_cold_contacts50du.h"
#include "runtime_startup50fl_fixture.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "native_racer_draw50hn_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "gc_trigger_actions50kg_fixture.h"
#include "ga_trigger_actions_fixture.h"
#include "ma_trigger_actions_fixture.h"
#include "sa_trigger_actions_fixture.h"
#include "sc_trigger_actions_fixture.h"
#include "mb_source_trigger50mb.h"
#include "gb_query50gb.h"
#include "mc_query50mc.h"
#include "sc_runtime_ground50la.h"
#include "gc_gate_audio50in_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "source_announcer50ie_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../src/game/original_racer_selection50ij.h"
#include "../../src/game/vehicle_draw_batch50co.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace interactive_cold50DU {
namespace {
void require50DU(bool ok,const char* message) {
    if(!ok){native_last_failure50PD::record(message);std::cerr<<"require_failed "<<(message?message:"")<<std::endl;throw std::runtime_error(message);}
}
template<class Operation>
decltype(auto) coldRacerStage50DU(const std::string& podId,const char* stage,
    Operation&& operation) {
    try { return operation(); }
    catch(const std::exception& error) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: "+error.what()));
    }
    catch(...) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: non-standard exception"));
    }
}
constexpr std::uint32_t raceRacerCapacity=rr::game::defaultHostRaceRacers50HT;
} // namespace

void ContactsContext::run() {
  // The neutral setup tick below performs genuine live trigger queries.
  // Bind the shared action owner only after every actual rig and final grid
  // pose exists, but before any tick can acquire its synchronous C80 lease.
  // Material/GPU preparation remains at the later renderer boundary.
  if(gcTriggerActions){
   require50DU(nativeOriginalFleet&&originalAiEnabled&&
       originalAiHumanCount==(humanCourseEnabled?1u:0u)&&racerBank.count()==raceRacerCount&&
       navigationAI->track.count==195,"GC triggers require the actual selected roster and195-node AI spline");
   std::array<gc_trigger_actions50KG::RacerIdentities,gc_trigger_actions50KG::racers> identities{};
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    identities[racer]={qfilter.identity,secondaryFilter.identity};
   });
   require50DU(gcTriggerActions->prepare(*gcTriggerAuthoring,*gcAnimatedWorld,identities,
       navigationAI->track.count,gcGateAudio.get(),!audioEnabled,originalAiHumanCount,raceRacerCount),
       "GC actual trigger/action destinations rejected");
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    require50DU(gcTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        gcTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "GC trigger cold actual COL poses unavailable");
   });
   std::cerr<<"GC NATIVE TRIGGERS:9 authored spheres, method1 zones,16 actual COL identities; "
       "source PLAYER-prefix-filtered LostAI branches, once-only Animation1 then BIGDOOR/World4D8; "
       "source entry/exit lifecycle, native endpoint/swept-surface geometry; reverse108E90 parity unresolved.\n";
   if(audioEnabled)std::cerr<<"GC BIGDOOR borrows the accepted native camera listener in XZ; original listener-object cadence parity remains unclaimed.\n";
  }
  if(isMbTrack){
   require50DU(originalAiEnabled&&racerBank.count()==raceRacerCount&&mbAiView.runtimeValid()&&mbAiView.count()==292&&
       mbOriginalDisc&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount,
       "MB requires actual human and seven AI owners");
   std::array<std::array<std::uint32_t,2>,8> identities{};
   for(std::size_t slot=0;slot<raceRacerCount;++slot)racerBank.visit(slot,[&](){identities[slot]={qfilter.identity,secondaryFilter.identity};});
   mbSourceTriggers=std::make_unique<mb_source_trigger50MB::Owner>();
   require50DU(mbSourceTriggers->bind(racerBank,mbCollisionWorld,identities,originalAiHumanCount),
       "MB genuine late trigger identity binding rejected");
   mbColdBound=true;
   for(std::size_t slot=0;slot<raceRacerCount;++slot)racerBank.visit(slot,[&](){
    require50DU(mbSourceTriggers->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        mbSourceTriggers->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "MB actual cold query geometry observation rejected");
   });
  }
  if(isMcTrack){
   require50DU(originalAiEnabled&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&
       originalAiOpponentCount==raceOpponentCount&&racerBank.count()==raceRacerCount,"MC actual human/seven-AI Bank roster required");
   mcQueryBinding=std::make_unique<mc_query50MC::Binding>();
   require50DU(mcQueryBinding->enroll(racerBank,mcCollisionWorld,mcNavigationOwner,qfilter.identity,
       secondaryFilter.identity,navigationProgress.reset.vehicleC80,originalAiHumanCount,s,sessionRandom,applicationClock),"MC typed current Bank/C80 enrollment rejected");
   for(std::size_t slot=0;slot<raceRacerCount;++slot)racerBank.visit(slot,[&](){
    require50DU(mcQueryBinding->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        mcQueryBinding->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "MC actual cold Bank geometry observation rejected");
   });
  }else if(isGbTrack){
   require50DU(originalAiEnabled&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&
       originalAiOpponentCount==raceOpponentCount&&racerBank.count()==raceRacerCount,"GB actual human/seven-AI Bank roster required");
   gbQueryBinding=std::make_unique<gb_query50GB::Binding>();
   require50DU(gbQueryBinding->enroll(racerBank,gbCollisionWorld,gbNavigationOwner,qfilter.identity,
       secondaryFilter.identity,navigationProgress.reset.vehicleC80,originalAiHumanCount),"GB typed current Bank/C80 enrollment rejected");
   for(std::size_t slot=0;slot<raceRacerCount;++slot)racerBank.visit(slot,[&](){
    require50DU(gbQueryBinding->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        gbQueryBinding->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "GB actual cold Bank geometry observation rejected");
   });
  }
  if(tbTriggerActions){
   require50DU(tbColdSources&&tbCollisionWorld->coldSource()==tbColdSources.get()&&
       tbWorldBorrow&&tbWorldBorrow->world()==&liveWorld&&originalAiEnabled&&racerBank.count()==raceRacerCount&&
       tbAiView.runtimeValid()&&tbAiView.count()==269&&tbMapView.runtimeValid()&&tbMapView.count()==270&&
       tbOriginalDisc&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount,
       "TB triggers require actual source/world/navigation and human/seven-AI ownership");
   std::array<tb_trigger_actions50TB::RacerIdentities,tb_trigger_actions50TB::racers> identities{};
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    identities[slot]={qfilter.identity,secondaryFilter.identity};
   });
   const bool bound=tbTriggerActions->bindRacerIdentities(identities,originalAiHumanCount,raceRacerCount);
   require50DU(bound,"TB genuine late trigger identity/action binding rejected");
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(tbTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        tbTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "TB actual cold query geometry unavailable");
   });
  }
  if(tcTriggerActions){
   require50DU(tcColdSources&&tcCollisionWorld->coldSource()==tcColdSources.get()&&
       tcWorldBorrow&&tcWorldBorrow->world()==&liveWorld&&originalAiEnabled&&racerBank.count()==raceRacerCount&&
       tcAiView.runtimeValid()&&tcAiView.count()==293&&tcMapView.runtimeValid()&&tcMapView.count()==268&&
       tcOriginalDisc&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount,
       "TC triggers require actual source/world/navigation and human/seven-AI ownership");
   std::array<tc_trigger_actions50TC::RacerIdentities,tc_trigger_actions50TC::racers> identities{};
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    identities[slot]={qfilter.identity,secondaryFilter.identity};
   });
   const bool bound=tcTriggerActions->bindRacerIdentities(identities,originalAiHumanCount,raceRacerCount);
   require50DU(bound,"TC genuine late trigger identity/action binding rejected");
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(tcTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        tcTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "TC actual cold query geometry unavailable");
   });
  }
  if(isRcTrack){
   requireMbRuntimeNavigation50MB();
   require50DU(rcTriggerActions&&rcTriggerAuthoring&&rcTriggerAuthoring->ready()&&
       rcTriggerAuthoring->coldSource()==rcColdSources.get()&&rcWorldBorrow&&
       rcWorldBorrow->world()==&liveWorld&&rcWorldBorrow->actions()==rcTriggerActions&&
       originalAiEnabled&&racerBank.count()==raceRacerCount&&rcNavigationBorrow.ai().track.count==209&&
       rcOriginalDisc&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount,
       "RC triggers require actual source/world/navigation and human/seven-AI ownership");
   std::array<rc_trigger_actions::RacerIdentities,rc_trigger_actions::racers> identities{};
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    identities[slot]={qfilter.identity,secondaryFilter.identity};
   });
   require50DU(rcTriggerActions->prepare(rcTriggerAuthoring,identities,rcNavigationBorrow.ai().track.count,originalAiHumanCount,raceRacerCount),
       "RC genuine late trigger identity/action binding rejected");
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(rcTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        rcTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "RC actual cold query geometry unavailable");
   });
  }
  if(scTriggerActions){
   require50DU(scColdSources&&scGround->coldSource()==scColdSources.get()&&originalAiEnabled&&racerBank.count()==raceRacerCount&&
       navigationAI->track.count==180&&navigationAI->indicesPrepared&&navigationMap->track.count==179&&navigationMap->indicesPrepared&&
       scOriginalDisc&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount,
       "SC triggers require actual source/navigation and human/seven-AI manager ownership");
   std::array<sc_trigger_actions::RacerIdentities,sc_trigger_actions::racers> identities{};
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    identities[slot]={qfilter.identity,secondaryFilter.identity};
   });
   require50DU(scTriggerActions->prepare(scTriggerAuthoring,identities,navigationAI->track.count,originalAiHumanCount,raceRacerCount),
       "SC genuine late trigger identity/action binding rejected");
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(scTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        scTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "SC actual cold query geometry unavailable");
   });
  }
  if(maTriggerActions){
   require50DU(originalAiEnabled&&racerBank.count()==raceRacerCount&&navigationAI->track.count==213&&
       ((allAiEnabled&&originalAiHumanCount==0&&originalAiOpponentCount==8)||
        (maOriginalDisc&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount)),
       "MA triggers require actual human/AI manager ownership and their original AI spline");
   std::array<ma_trigger_actions::RacerIdentities,ma_trigger_actions::racers> identities{};
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    identities[racer]={qfilter.identity,secondaryFilter.identity};
   });
   require50DU(maTriggerActions->prepare(*maTriggerAuthoring,identities,navigationAI->track.count,originalAiHumanCount,raceRacerCount),
       "MA actual trigger/action destinations rejected");
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    require50DU(maTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        maTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "MA trigger cold actual COL poses unavailable");
   });
  }
  if(saTriggerActions){
   require50DU(originalAiEnabled&&racerBank.count()==raceRacerCount&&navigationAI->track.count==121&&
       ((allAiEnabled&&originalAiHumanCount==0&&originalAiOpponentCount==8)||
        (saOriginalDisc&&humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount)),
       "SA triggers require actual human/AI manager ownership and their original AI spline");
   std::array<sa_trigger_actions::RacerIdentities,sa_trigger_actions::racers> identities{};
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    identities[racer]={qfilter.identity,secondaryFilter.identity};
   });
   require50DU(saTriggerActions->prepare(*saTriggerAuthoring,identities,originalAiHumanCount,navigationAI->track.count,raceRacerCount),
       "SA actual trigger/action destinations rejected");
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    require50DU(saTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        saTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "SA trigger cold actual COL poses unavailable");
   });
  }
  if(gaAnimatedWorld){
   require50DU(humanCourseEnabled&&originalAiEnabled&&originalAiHumanCount==1&&
       originalAiOpponentCount==raceOpponentCount&&racerBank.count()==raceRacerCount&&navigationAI->track.count==107,
       "GA triggers require real Player0, seven original AI and canonical107-node navigation");
   std::array<ga_trigger_actions::RacerIdentities,ga_trigger_actions::racers> triggerIdentities{};
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    triggerIdentities[racer]={qfilter.identity,secondaryFilter.identity};
   });
   require50DU(gaTriggerActions->prepare(*gaTriggerAuthoring,*gaAnimatedWorld,triggerIdentities,
       navigationAI->track.count,gaGateAudio.get(),!audioEnabled,originalAiHumanCount,raceRacerCount),"GA actual trigger/action destinations unavailable");
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    require50DU(gaTriggerActions->observePrepared(qfilter.identity,*query.resource,mainMembership.lastShapeWorld)&&
        gaTriggerActions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,secondaryMembership.lastShapeWorld),
        "GA trigger lifecycle cold actual COL poses unavailable");
   });
   std::cerr<<"GA NATIVE TRIGGERS:10 authored spheres, method1 zones,16 actual racer COL identities; "
       "unchanged GC endpoint/swept-surface geometry policy, original action/lifecycle semantics; reverse108E90 parity unresolved.\n";
   if(audioEnabled)std::cerr<<"GA BIGDOOR uses the existing shared accepted-camera native listener in XZ and real World4D8 PCM; original listener-object parity is not claimed.\n";
  }
  if(nativeSelectedLights&&!isRaTrack)for(std::size_t racer=0;racer<racerBank.count();++racer)
   racerBank.visit(racer,[&](){
    // Only this genuine unadvanced cold generation owns source C30=4C0[0].
    // Rig installation and final per-racer grid placement already completed.
    require50DU(completedFrames==0&&m.frameFactor==0.f&&gcGraphicsSequence[racer]==0,
        "GC visibility creation requires final fresh grid before first graphics");
    require50DU(gcLightOwner.attachFreshVehicle(static_cast<std::uint32_t>(racer),s,
        x.bindings,x.parameters,gcLightDiagnostic),"actual cold GC racer7F0 registration failed");
    if(originalRecoveryEnabled){
     // Only the admitted direct-GA Player0 uses the human constructor role.
     // Geometry comes from the same selected pod/probe owners as every AI;
     // neither source vehicle58 nor native AI ownership is rewritten.
     const auto queryRole=directOriginalHuman&&humanCourseEnabled&&racer==0?
         undertrack_owners50ew_fixture::ColdQueryRole50LH::humanPlayer:
         undertrack_owners50ew_fixture::ColdQueryRole50LH::originalAi;
     const bool attached=undertrack_owners50ew_fixture::attachFreshQueries(recoveryQueryOwners,s,x.bindings,x.parameters,
         recoveryBorrowedQueries(),{trackCounts.podQuery,trackCounts.movementQuery},zoneLifecycle,liveWorld,
         membershipWork,initialCollisionZone,queryRole);
     if(!attached)std::cerr<<"native_cold_query_rejected slot="<<racer
         <<" role="<<static_cast<unsigned>(queryRole)<<" vehicle58="<<s.fields.vehicle58
         <<" ai_owners="<<s.fields.nativeAiOwnersBound50IV<<'\n';
     require50DU(attached,"actual selected694/7A4 cold registrations rejected");
     require50DU(publishRecoveryQueries(),"actual selected694/7A4 cold publication rejected");
    }
   });
  if(racerContactsEnabled){
   racerContactOwner=std::make_unique<native_racer_contact50HW::Owner>();
   std::string error;
   const bool contactsReady=racerContactOwner->prepare(contactBindings.data(),raceRacerCount,error);
   require50DU(contactsReady,error.empty()?"original racer contact world preparation failed":error.c_str());
  }
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   const auto index=racer-1;auto& visual=*racerVisuals[racer];
   coldRacerStage50DU(racerRigs[racer]->podId,"draw storage preparation",[&](){
    VehicleDrawBatchInput50CO source{};
    require50DU(visual.drawAssets.batchInput(0,ModelPalettePhase49S::selectedRenderPreparation,
        ModelPaletteSceneState49S::completeCurrentScene,source),"original opponent draw descriptors unavailable");
    require50DU(opponentDraw[index].initialize(source),"original opponent draw storage initialization failed");
    for(unsigned component=1;component<visual.drawAssets.componentCount();++component)
     for(const auto material:visual.flameMaterialIndices(component)){
      platform::VehicleWindowMaterialUv50DS uv{};
      uv.component=static_cast<VehicleDrawComponent50CM>(component);uv.materialIndex=material;
      opponentUvs[index].push_back(uv);
     }
   });
  }
  if(dynamicContactEnabled){
   std::array<native_dynamic_contact50IW::ManagerBinding,raceRacerCapacity> bindings{};
   for(std::size_t slot=0;slot<raceRacerCount;++slot){
    bindings[slot]={slot,{racerContactIds[slot]}};
    // Default camera POD01 may use the retained fixture in the mixed path;
    // selectedOriginal is already authenticated before any cold installation.
    require50DU(slot==0||racerRigs[slot]!=nullptr,"actual contact actor rig unavailable");
    const auto* selection=slot==0?selectedOriginal:
        original_racer_selection50IJ::findByPodId(racerRigs[slot]->podId.c_str());
    require50DU(selection,"actual contact actor original racer60 unavailable");
    require50DU(dynamicRacer60[slot]==selection->racer60&&
        sourceAnnouncer->collisionRequestsEnabled()&&sourceAnnouncer->hasNamedBank(selection->racer60),
        "actual dynamic actor and shared announcer bank identity differ");
   }
   std::string error;
   require50DU(dynamicRegistry.prepare(bindings.data(),raceRacerCount,originalAiHumanCount,error),
       error.empty()?"original dynamic manager registry unavailable":error.c_str());
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(s.phase==VehicleFramePhase49W::Ready&&m.components.body20C==0&&
        s.impactSupplement.partnerId==0xffffffffu&&s.impactSupplement.partnerState==0,
        "dynamic owners require original fresh body/D34 state");
    s.fields.dynamicOwnersBound50IW=true;
   });
  }
}
} // namespace interactive_cold50DU

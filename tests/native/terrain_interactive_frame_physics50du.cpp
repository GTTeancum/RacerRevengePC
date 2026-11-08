// Source-identical Physics suffix; all queries retain the original live Context.
#include "terrain_interactive_frame_calls50du.h"
#include "terrain_interactive_storage50du.h"
#include "runtime_startup50fl_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "original_ai_audio_owner.h"
#include "ra_cold_aggregate50la.h"
#include "mc_query50mc.h"
#include "gb_query50gb.h"
#include "mb_source_trigger50mb.h"
#include "ga_trigger_actions_fixture.h"
#include "gc_trigger_actions50kg_fixture.h"
#include "ma_trigger_actions_fixture.h"
#include "sa_trigger_actions_fixture.h"
#include "sc_trigger_actions_fixture.h"
#include "lap_drive50gj_fixture.h"
#include "live_audio50hf_fixture.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "../../src/game/vehicle_secondary_contact50j.h"
#include "../../src/game/vehicle_live_query50eq.h"
#include "../../src/game/vehicle_static_query50h.h"
#include "../../src/game/movement_static_query50l.h"
#include "../../src/game/collision_normal_mode50ja.h"
#include "../../src/game/engine_damage_audio50ia.h"
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include "ra_cold_roster_operation50la.h"

namespace interactive_frame50DU {
namespace f=runtime_startup50fl;
using namespace rr::game;
using NativeRacerFramePart=Part;
using detail::require50DU;
using detail::addCounter50DU;

bool Context::advancePhysics(std::size_t actualRacerSlot,Part part,float liveSubstepScale,
    bool sourceAiAudio,NativeCalls& runNative) {
 require50DU(part==NativeRacerFramePart::Physics&&
     s.phase==VehicleFramePhase49W::AfterBodyChildren,
     "physics suffix requires completed racer children");
 // A destroyed racer (state3/4) has no body integration: its intact body is
 // hidden and frozen; only the phase closes for the shared schedule.
 if(s.bodies.main.vehicleState40==3u||s.bodies.main.vehicleState40==4u){
  // Shared schedule contract: one consumed substep at this frame's factor.
  s.bodies.main.schedule.count=1;s.bodies.main.substepIndex=1;
  s.phase=VehicleFramePhase49W::Ready;return true;
 }
 runNative([&]{return scheduleVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenAfter35C),liveSubstepScale,x.workspace,x.diagnostic);},"schedule");
 require50DU(s.bodies.main.schedule.count==1,"16ms diagnostic step requires one native substep");

 runNative([&]{return beginVehicleSubstep49W(s,x.bindings,x.selectors,x.workspace,x.diagnostic);},"native selector");
 // The C80 lease spans the actual synchronous query only. It is released
 // on both normal and exceptional exits before any Bank switch or observation.
 auto withRaTriggerLease=[&](auto&& queryOperation){
  if(!isRaTrack)return queryOperation();
  require50DU(raWorld->beginRuntimeTriggerQuery(racerBank,actualRacerSlot,raDiagnostic),
      "RA actual racer C80 query lease rejected");
  bool active=true;
  try {
   auto result=queryOperation();
   const bool ended=raWorld->endRuntimeTriggerQuery(racerBank,raDiagnostic);active=false;
   require50DU(ended,"RA original trigger action failed after query commit");return result;
  }catch(...){
   if(active&&!raWorld->endRuntimeTriggerQuery(racerBank,raDiagnostic))std::terminate();
   throw;
  }
 };
 auto withRcTriggerLease=[&](auto&& queryOperation){
  if(!isRcTrack)return queryOperation();
  requireMbRuntimeNavigation50MB();
  require50DU(rcTriggerActions&&rcTriggerActions->ready()&&rcWorldBorrow&&
      rcWorldBorrow->world()==&liveWorld&&rcWorldBorrow->actions()==rcTriggerActions&&
      rcTriggerActions->sourceOwner()==rcTriggerAuthoring.get()&&
      racerBank.count()==raceRacerCount&&actualRacerSlot<racerBank.count(),
      "RC query requires its actual same-source world and current Bank arena");
  const rc_trigger_actions::ActionLease lease{qfilter.identity,&navigationProgress.reset.vehicleC80};
  require50DU(rcTriggerActions->beginQuery(lease),"RC genuine main690/C80 query lease rejected");
  bool active=true;
  try{
   auto result=queryOperation();const bool ended=rcTriggerActions->endQuery();active=false;
   require50DU(ended,"RC source trigger action failed after committed query");return result;
  }catch(...){
   // endQuery always clears the named borrow, even after action rejection.
   // Preserve the actual exception and every already-committed source prefix.
   if(active){const bool ended=rcTriggerActions->endQuery();active=false;
    if(!ended)std::fputs("RC exceptional query ended with retained action failure\n",stderr);}
   throw;
  }
 };
 auto withScTriggerLease=[&](auto&& queryOperation){
  if(!scTriggerActions)return queryOperation();
  require50DU(racerBank.count()==raceRacerCount&&actualRacerSlot<racerBank.count(),"SC query requires actual current racer slot");
  const sc_trigger_actions::ActionLease lease{qfilter.identity,&navigationProgress.reset.vehicleC80};
  require50DU(scTriggerActions->beginQuery(lease),"SC genuine main690/C80 query lease rejected");
  bool active=true;
  try{
   auto result=queryOperation();const bool ended=scTriggerActions->endQuery();active=false;
   require50DU(ended,"SC source trigger action failed after committed query");return result;
  }catch(...){
   // endQuery clears the actual borrow even when a source-action rejection
   // makes its result false. Preserve the exception and committed prefixes.
   if(active){const bool ended=scTriggerActions->endQuery();active=false;
    if(!ended)std::fputs("SC exceptional query ended with retained action failure\n",stderr);}
   throw;
  }
 };
 // The named C80 lease outlives QueryBorrow; both expire before Bank restore.
 // Failed numerical/prefix work is terminal, even when no action was reached.
 auto withTbTriggerLease=[&](std::uint32_t actualCollider,auto&& queryOperation){
  if(!tbTriggerActions)return queryOperation();
  require50DU(tbWorldBorrow&&tbWorldBorrow->world()==&liveWorld&&racerBank.count()==raceRacerCount&&
      actualRacerSlot<racerBank.count(),"TB query requires its actual world/Bank arena");
  Vector3_48C listener{{audioListener.x,0.f,audioListener.z}};
  const tb_trigger_actions50TB::ActionLease lease{qfilter.identity,actualCollider,&navigationProgress.reset.vehicleC80,
      audioListenerReady?&listener:nullptr,audioListenerReady?1u:0u};
  tb_trigger_actions50TB::Owner::QueryBorrow borrow(*tbTriggerActions,lease);
  require50DU(bool(borrow),"TB genuine collider/C80 query borrow rejected");
  auto result=queryOperation();
  const bool complete=result.status==VehicleLiveQueryStatus50EQ::Complete;
  const bool ended=borrow.finish(complete);
  require50DU(!complete||ended,"TB successful query did not release its actual action cursor");
  return result;
 };
 auto withMcTriggerLease=[&](bool secondary,auto&& queryOperation){
  if(!isMcTrack)return queryOperation();
  require50DU(mcQueryBinding&&mcQueryBinding->enrolled(),"MC current typed Bank query binding absent");
  VehicleLiveQueryResult50EQ result{};bool entered=false;
  const bool accepted=mcQueryBinding->query(actualRacerSlot,secondary,[&](track_runtime_storage50IG::Storage& actual){
   if(&actual!=&trackStorage)return false;entered=true;result=queryOperation();
   if(result.status==VehicleLiveQueryStatus50EQ::Complete&&result.prefixCommitted){
    const auto& source=secondary?secondaryQuery:query;const auto& member=secondary?secondaryMembership:mainMembership;
    if(!mcQueryBinding->observePrepared(source.filter->identity,*source.resource,member.lastShapeWorld))return false;
   }
   return result.status==VehicleLiveQueryStatus50EQ::Complete;
  });
  require50DU(entered,"MC genuine query admission rejected before indexed work");
  require50DU(result.status!=VehicleLiveQueryStatus50EQ::Complete||accepted,"MC completed indexed query failed actual owner publication");
  return result;
 };
 auto withGbTriggerLease=[&](bool secondary,auto&& queryOperation){
  if(!isGbTrack)return queryOperation();
  require50DU(gbQueryBinding&&gbQueryBinding->enrolled(),"GB current typed Bank query binding absent");
  VehicleLiveQueryResult50EQ result{};bool entered=false;
  const bool accepted=gbQueryBinding->query(actualRacerSlot,secondary,[&](track_runtime_storage50IG::Storage& actual){
   if(&actual!=&trackStorage)return false;entered=true;result=queryOperation();
   if(result.status==VehicleLiveQueryStatus50EQ::Complete&&result.prefixCommitted){
    const auto& source=secondary?secondaryQuery:query;const auto& member=secondary?secondaryMembership:mainMembership;
    if(!gbQueryBinding->observePrepared(source.filter->identity,*source.resource,member.lastShapeWorld))return false;
   }
   return result.status==VehicleLiveQueryStatus50EQ::Complete;
  });
  require50DU(entered,"GB genuine query admission rejected before indexed work");
  require50DU(result.status!=VehicleLiveQueryStatus50EQ::Complete||accepted,"GB completed indexed query failed actual owner publication");
  return result;
 };
 auto withTcTriggerLease=[&](std::uint32_t actualCollider,auto&& queryOperation){
  if(!tcTriggerActions)return queryOperation();
  require50DU(tcWorldBorrow&&tcWorldBorrow->world()==&liveWorld&&racerBank.count()==raceRacerCount&&
      actualRacerSlot<racerBank.count(),"TC query requires its actual world/Bank arena");
  const tc_trigger_actions50TC::ActionLease lease{qfilter.identity,actualCollider,&navigationProgress.reset.vehicleC80};
  tc_trigger_actions50TC::Owner::QueryBorrow borrow(*tcTriggerActions,lease);
  require50DU(bool(borrow),"TC genuine collider/C80 query borrow rejected");
  auto result=queryOperation();
  const bool complete=result.status==VehicleLiveQueryStatus50EQ::Complete;
  const bool ended=borrow.finish(complete);
  require50DU(!complete||ended,"TC successful query did not release its actual action cursor");
  return result;
 };
 MainStaticDetectorAdmission50H admission{};
 const auto* detectorPolicy=trackStorage.nativeDetectorPolicy();
 require50DU(detectorPolicy&&detectorPolicy->admission(false,s,x.bindings,query,spring,settings,
     mainMembership,mainQueryRef,admission),"actual registered native main detector policy unavailable");
 runNative(collectRegistered,"ordered prepared-list vehicle candidates");
 if(isMbTrack){
#include "mb_main_terrain_query50mb.inc"
 }else{
 Vector3_48C mainTriggerListener{{audioListener.x,0.f,audioListener.z}};
 ga_trigger_actions::ActionLease mainTriggerLease{qfilter.identity,&navigationProgress.reset.vehicleC80,
     audioListenerReady?&mainTriggerListener:nullptr,audioListenerReady?1u:0u};
 gc_trigger_actions50KG::ActionLease mainGcLease{qfilter.identity,&navigationProgress.reset.vehicleC80,
     audioListenerReady?&mainTriggerListener:nullptr,audioListenerReady?1u:0u};
 if(gcTriggerActions)require50DU(gcTriggerActions->beginQuery(mainGcLease),"GC main trigger arena lease unavailable");
 if(gaTriggerActions)require50DU(gaTriggerActions->beginQuery(mainTriggerLease),"GA main trigger arena lease unavailable");
 ma_trigger_actions::ActionLease mainMaLease{qfilter.identity,&navigationProgress.reset.vehicleC80};
 sa_trigger_actions::ActionLease mainSaLease{qfilter.identity,&navigationProgress.reset.vehicleC80};
 if(maTriggerActions)require50DU(maTriggerActions->beginQuery(mainMaLease),"MA main trigger arena lease unavailable");
 if(saTriggerActions)require50DU(saTriggerActions->beginQuery(mainSaLease),"SA main trigger arena lease unavailable");
 const auto mainQueryAttempt=withMcTriggerLease(false,[&](){return withGbTriggerLease(false,[&](){return withTbTriggerLease(qfilter.identity,[&](){return withRcTriggerLease([&](){return withTcTriggerLease(qfilter.identity,[&](){return withScTriggerLease([&](){return withRaTriggerLease([&](){return queryMainLiveIndexedStaticVehicle50EQ(s,x.bindings,admission,mainLiveQuery,query,candidates.data(),candidates.size(),registeredIndices,registeredCount,initial,workspace,destination,queryResult);});});});});});});});
 if(isRaTrack&&mainQueryAttempt.prefixCommitted)
  require50DU(raWorld->observeRuntimeTriggerPoses(racerBank,actualRacerSlot,raDiagnostic),
      "RA committed query pose observation rejected");
 if(gcTriggerActions)require50DU(gcTriggerActions->endQuery(),"GC main source trigger action failed after contact commit");
 if(gaTriggerActions)require50DU(gaTriggerActions->endQuery(),"GA main source trigger action failed after contact commit");
 if(maTriggerActions)require50DU(maTriggerActions->endQuery(),"MA main trigger action failed after contact commit");
 if(saTriggerActions)require50DU(saTriggerActions->endQuery(),"SA main trigger action failed after contact commit");
 if(!isTcTrack&&!isGbTrack&&!isMcTrack&&mainQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
    recoverPortalBoundary(mainQueryAttempt.prefix,"main")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{return acceptLiveQuery(mainQueryAttempt,query,mainMembership,mainCrossedPortals,"main");},
     "registered main terrain query");
 require50DU(collisionNormalMode.acceptCompleted(queryResult.normalModeWrite),"source collision normal-mode binding unavailable");
 runNative([&]{return commitCandidateEffects(effects);},"commit main query candidate effects");qfilter.flagsF8=queryResult.queryFlagsF8;query.initialQueryCurrentIdentity=queryResult.currentCandidateIdentity;initial.selected=queryResult.selected;
 spring.records=queryResult.records;std::memcpy(records,queryRecords,sizeof records);
 }

 if(s.bodies.vehicle838){
 MainStaticDetectorAdmission50H secondaryAdmission{};
 require50DU(detectorPolicy->admission(true,s,x.bindings,secondaryQuery,secondarySpring,secondarySettings,
     secondaryMembership,secondaryQueryRef,secondaryAdmission),"actual registered native secondary detector policy unavailable");
 runNative([&]{return prepareSecondaryLiveQueryScene50EQ(s,x.bindings,secondaryAdmission,secondaryLiveQuery,x.workspace)==SecondaryQuerySceneStatus50J::Ready;},"registered original secondary scene getter");
 // Secondary uses the same first-substep merged list, without recollection.

 if(isMbTrack){
#include "mb_secondary_terrain_query50mb.inc"
 }else{
 Vector3_48C secondaryTriggerListener{{audioListener.x,0.f,audioListener.z}};
 ga_trigger_actions::ActionLease secondaryTriggerLease{qfilter.identity,&navigationProgress.reset.vehicleC80,
     audioListenerReady?&secondaryTriggerListener:nullptr,audioListenerReady?1u:0u};
 gc_trigger_actions50KG::ActionLease secondaryGcLease{qfilter.identity,&navigationProgress.reset.vehicleC80,
     audioListenerReady?&secondaryTriggerListener:nullptr,audioListenerReady?1u:0u};
 if(gcTriggerActions)require50DU(gcTriggerActions->beginQuery(secondaryGcLease),"GC secondary trigger arena lease unavailable");
 if(gaTriggerActions)require50DU(gaTriggerActions->beginQuery(secondaryTriggerLease),"GA secondary trigger arena lease unavailable");
 ma_trigger_actions::ActionLease secondaryMaLease{qfilter.identity,&navigationProgress.reset.vehicleC80};
 sa_trigger_actions::ActionLease secondarySaLease{qfilter.identity,&navigationProgress.reset.vehicleC80};
 if(maTriggerActions)require50DU(maTriggerActions->beginQuery(secondaryMaLease),"MA secondary trigger arena lease unavailable");
 if(saTriggerActions)require50DU(saTriggerActions->beginQuery(secondarySaLease),"SA secondary trigger arena lease unavailable");
 const auto secondaryQueryAttempt=withMcTriggerLease(true,[&](){return withGbTriggerLease(true,[&](){return withTbTriggerLease(secondaryFilter.identity,[&](){return withRcTriggerLease([&](){return withTcTriggerLease(secondaryFilter.identity,[&](){return withScTriggerLease([&](){return withRaTriggerLease([&](){return querySecondaryLiveIndexedStaticVehicle50EQ(s,x.bindings,secondaryAdmission,secondaryLiveQuery,secondaryQuery,candidates.data(),candidates.size(),registeredIndices,registeredCount,si,sw,sd,secondaryResult);});});});});});});});
 if(isRaTrack&&secondaryQueryAttempt.prefixCommitted)
  require50DU(raWorld->observeRuntimeTriggerPoses(racerBank,actualRacerSlot,raDiagnostic),
      "RA committed query pose observation rejected");
 if(gcTriggerActions)require50DU(gcTriggerActions->endQuery(),"GC secondary source trigger action failed after contact commit");
 if(gaTriggerActions)require50DU(gaTriggerActions->endQuery(),"GA secondary source trigger action failed after contact commit");
 if(maTriggerActions)require50DU(maTriggerActions->endQuery(),"MA secondary trigger action failed after contact commit");
 if(saTriggerActions)require50DU(saTriggerActions->endQuery(),"SA secondary trigger action failed after contact commit");
 if(!isTcTrack&&!isGbTrack&&!isMcTrack&&secondaryQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
    recoverPortalBoundary(secondaryQueryAttempt.prefix,"secondary")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{return acceptLiveQuery(secondaryQueryAttempt,secondaryQuery,secondaryMembership,secondaryCrossedPortals,"secondary");},
     "registered secondary terrain query");
 require50DU(collisionNormalMode.acceptCompleted(secondaryResult.normalModeWrite),"source collision normal-mode binding unavailable");
 runNative([&]{return commitCandidateEffects(sde);},"commit secondary query candidate effects");secondaryFilter.flagsF8=secondaryResult.queryFlagsF8;secondaryQuery.initialQueryCurrentIdentity=secondaryResult.currentCandidateIdentity;si.selected=secondaryResult.selected;
 secondarySpring.records=secondaryResult.records;std::memcpy(secondaryRecords,secondaryQueryRecords,sizeof secondaryRecords);
 }
 
 }
 TerrainPrepareState48M terrainPrepared{};TerrainQueryResult48M terrainResult{};
 runNative([&]{return prepareVehicleTerrain50FP(s,x.bindings,x.parameters,terrainOwner.initial.segment,
     terrainPrepared,x.workspace,x.diagnostic);},"prepare enabled native terrain probe");
 const auto terrainAttempt=live_terrain_probe50fr::query(terrainLiveOwner,terrainPrepared,
     terrainOwner.metadata,terrainContext,probeIndices,probeCount,terrainResult);
 if(terrainAttempt.prefixCommitted&&originalRecoveryEnabled){
  terrainOwner.initial.segment.worldMatrix=terrainPrepared.segment.worldMatrix;
  terrainOwner.initial.sphere=terrainPrepared.sphere;
 }
 if(terrainAttempt.status==live_terrain_probe50fr::Status::PrefixRejected&&
    recoverPortalBoundary(terrainAttempt.prefix,"terrain")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{
   if(terrainAttempt.status!=live_terrain_probe50fr::Status::Complete)
    std::cerr<<"terrain query status "<<unsigned(terrainAttempt.status)<<" prefix "<<unsigned(terrainAttempt.prefix.status)
       <<" query "<<unsigned(terrainAttempt.queryStatus)<<" prefix_committed "<<terrainAttempt.prefixCommitted<<'\n';
   return terrainAttempt.status==live_terrain_probe50fr::Status::Complete;
 },"actual registered terrain limited-list query");
 require50DU(collisionNormalMode.acceptCompleted(terrainResult.normalModeWrite),"source collision normal-mode binding unavailable");
 for(std::size_t i=0;i<probeCount;++i){const auto index=probeIndices[i];
  const auto& source=candidates[index].candidate.collision;auto& mirror=movementCandidates[index].collision;
  mirror.filter.flagsF8=source.filter.flagsF8;mirror.copied40=source.copied40;mirror.copied50=source.copied50;
 }
 runNative([&]{return commitVehicleTerrainQuery50FP(s,x.bindings,terrainResult,x.workspace,x.diagnostic);},
     "commit actual terrain surface");
 ++terrainQueries;if(terrainResult.terrain.surface.hit110||terrainResult.terrain.surface.hit114)++terrainHitFrames;
 PostWorld49W world{};world.point=s.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420;world.effects=contactEffects49W(s);world.applicationGateByte58=s.fields.applicationByte58;
 runNative([&]{return produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic);},"native main forces");
 ResponseStorage50K responseStorage;auto responseWorkspace=responseStorage.view(x.workspace);
 runNative([&]{return respondMainStaticVehicle50G(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,query.queryScene,1,spring,records,40,responseWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual main contact response");
 require50DU(s.phase==(s.bodies.vehicle838?VehicleFramePhase49W::Post368:VehicleFramePhase49W::MainIntegration),"source-selected response phase");

 if(s.bodies.vehicle838){
 ResponseStorage50K secondaryStorage;auto secondaryWorkspace=secondaryStorage.view(x.workspace);
 runNative([&]{return respondSecondaryStaticVehicle50J(s,x.bindings,x.parameters,secondarySettings,secondaryResponse,{78},2,secondarySpring,secondaryRecords,60,secondaryWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual secondary contact and integration");
 require50DU(s.phase==VehicleFramePhase49W::MainIntegration,"secondary response integration phase");
 }
 if(dynamicContactEnabled){
  native_dynamic_contact50IW::Diagnostic peerDiagnostic{};
  if(!dynamicPeers.refreshSelfAtImpact(s,peerDiagnostic))
   dynamicBoundary(peerDiagnostic.sourcePC,"dynamic impact live self projection unavailable");
  DynamicVehicleImpactCursor50IW impact{};
  auto status=beginVehicleDynamicImpact50IW(s,x.bindings,x.parameters,poseHistory,
      dynamicPeers.managerView(),dynamicTuning,dynamicProfile,impact,x.diagnostic);
  if(status==DynamicVehicleImpactStatus50IW::NeedFirstContact){
   fulfillDynamicMusic();
   status=resumeVehicleDynamicImpact50IW(s,poseHistory,impact,x.diagnostic);
  }
  if(status!=DynamicVehicleImpactStatus50IW::Complete)
   dynamicBoundary(x.diagnostic.sourceStage,"original dynamic impact continuation rejected");
 }
 runNative([&]{return integrateVehicleMain49W(s,x.bindings,x.parameters,x.workspace,x.diagnostic);},"contact-driven body integration");
 require50DU(s.phase==VehicleFramePhase49W::Query219120,"real late-query boundary");
 std::memcpy(&endpoints[0],s.bodies.main.priorPoint7B0,16);std::memcpy(&endpoints[1],s.bodies.main.currentPoint7C0,16);
 if(originalRecoveryEnabled){
  //2191FC ->11CA80 copies both pairs/zero metadata;219204 ->11FF60
  // reaches120230 for the actual registered7A4. No second ray owner exists.
  endpointScenes[0]=endpointScenes[1]=0;recoveryQueryOwners.movementTag=3;
  LiveZoneBounds50EN bounds{};bounds.bounds.shapeTag=3;bounds.bounds.pairCount=1;
  bounds.bounds.endpoints=endpoints;bounds.bounds.endpointCount=2;
  bounds.bounds.endpointScenes=endpointScenes;bounds.bounds.endpointSceneCount=2;
  runNative([&]{return updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,
      recoveryQueryOwners.memberships[1],bounds,membershipWork).status==LiveZoneStatus50EN::Complete;},
      "source registered7A4 movement update");
  const auto& cached=recoveryQueryOwners.memberships[1].bounds;
  for(unsigned axis=0;axis<3;++axis)recoveryQueryOwners.movementSphere.center.v[axis]=cached.cachedCenter.v[axis];
  recoveryQueryOwners.movementSphere.radius=cached.cachedRadius;
 }
 runNative(gatherRegistered,"registered movement candidates");
 runNative([&]{const auto outcome=queryMovementStaticCandidates50L({true,true},movement,movementSource,registeredMovement,registeredCount,movementWorkspace,movementOutput,movementResult,movementFlag);movementFlag=outcome.vehicle7A0;return outcome.status==Type3QueryStatus48P::Complete;},"actual movement segment against terrain");
 require50DU(collisionNormalMode.acceptCompleted(movementResult.normalModeWrite),"source collision normal-mode binding unavailable");
 runNative([&]{return commitCandidateEffects(movementEffects);},"commit movement query candidate effects");movement.filter.flagsF8=movementResult.queryFlags;movement.initial=movementResult.state;movement.initialQueryCurrent=movementResult.currentCandidate;movementInitial=movementDestination;
 MovementQueryCompletion50M completion{};completion.resultFlagsF8=movementResult.queryFlags;std::memcpy(&completion.selectedPoint,&movementResult.state.selectedPoint,16);completion.pushDistance434710=10;completion.representedStatePreserved=completion.omittedBB0WNotConsumed=true;
 runNative([&]{return finishVehicleMovementQuery50M(s,x.bindings,x.parameters,completion,x.workspace,x.diagnostic);},"native actual movement result and frame tail");
 require50DU(s.phase==VehicleFramePhase49W::Ready,"one-substep diagnostic frame complete");
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame&&!sourceAiAudio){
  const auto& engineParameters=liveAudio->engineParameters(); // Selected original PHY tuning.
  // Original19EEA0 order: cooldown, active curve, damage selection, then
  // engine pitch controls. The same player RNG already used by effects and
  // material audio must also receive the source's two healthy-state draws.
  const EngineDamageInput50IA damageInput{m.frameFactor,s.fields.damage7D0,
      s.fields.damage7D4,s.fields.damage7DC};
  const auto damageReceipt=updateEngineDamageAudio50IA(damageInput,engineDamage,
      engineDamage,s.noiseOwner().sharedRandomState);
  require50DU(damageReceipt.okay(),"original engine damage modulation rejected");
  ++engineDamageUpdates;engineDamageDraws+=damageReceipt.randomDraws;
  if(damageReceipt.damageDipStarted)++engineDamageDips;
  if(damageReceipt.returnStarted)++engineDamageReturns;
  if(damageReceipt.dipFinished)++engineDamageCompletions;
  PodEngineInput50HE engineInput{boostState.speedRatio70,engineDamage.scale484,liveAudio->boostPitch()};
  require50DU(liveAudio->engine(engineParameters,engineInput).okay(),"native engine audio controls rejected");
  PodWarningInput50HE warning{};
  warning.damage7D0=s.fields.damage7D0;warning.damage7D4=s.fields.damage7D4;
  warning.damage7DC=s.fields.damage7DC;warning.speedRatio70=boostState.speedRatio70;
  warning.vehicle64=boostState.vehicle64;warning.flagsCD8=s.fields.impactFlagsCD8;
  warning.flags10CC=lapContinuity.prefix.flags10CC;
  warning.emitter22C=liveAudio->boostPitch()?2u:0u;
  warning.sprayInputKnown=false; // Water probe118 is not represented by this client.
  require50DU(liveAudio->warnings(warning).okay(),"native warning audio projection rejected");
 }
 addCounter50DU(completedFrames);
 if(m.components.probe348.field104)++terrainSupportedFrames;
 if(m.controls.player.flagsB9C&2u)addCounter50DU(repairActiveTicks);
 if(spring.records.contact3C)addCounter50DU(mainContactFrames);
 if(s.bodies.vehicle838&&secondarySpring.records.contact3C)addCounter50DU(secondaryContactFrames);
 return true;
 
}
} // namespace interactive_frame50DU

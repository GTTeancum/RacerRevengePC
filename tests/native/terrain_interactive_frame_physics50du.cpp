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
#include "../../src/game/native_test_undertrack50ut.h"
#include "../../src/game/vehicle_pod_response50pc.h"
#include "roster_pod_contact50pc.h"
#include "../../src/game/native_surface_traction50st.h"
#include "../../src/game/native_zero_gravity50zg.h"
#include "../../src/game/vehicle_frame_surface50st.h"
#include <algorithm>
#include <limits>
#include <optional>
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
 // Unattended test only (RR_TEST_UNDERTRACK, native_test_undertrack50ut.h):
 // drop this racer below Y=0 before its physics step, as a pod that fell
 // through the track. 167CA0 rereads the body position from main190's world
 // (133g: writing the body field alone is overwritten), so the pod's two
 // parentless body scenes main190/19C (and the parentless hidden 198) move by
 // the same vertical step through the game's own 28AF90 translation setter and
 // 2897B0 refresh, and the bodies' scene caches are refreshed from them as the
 // frame's own projection does. The 219120 sweep runs 7B0 -> 7C0; a sweep from
 // above the track to below it hits the track and puts the pod back, so the
 // prior point is cleared to the shared sentinel as 216A30 does after it
 // places a pod (219D90 then copies the current point: zero-length sweep).
 // Writes stay inside this racer's own frame state (s); nothing else moves.
 if(actualRacerSlot<rr::game::nativeTestUndertrackPending50UT.size()&&
    rr::game::nativeTestUndertrackPending50UT[actualRacerSlot]){
  rr::game::nativeTestUndertrackPending50UT[actualRacerSlot]=false;
  const auto& life=x.bindings.lifecycle;
  const SceneGraph49P graph{s.nodes,s.sceneNodeCount};
  require50DU(validVehicleSceneCount49W(s,x.bindings)&&validateSceneGraph49P(graph)&&
      static_cast<std::uint32_t>(life.main190)<s.sceneNodeCount&&
      static_cast<std::uint32_t>(life.physical19C)<s.sceneNodeCount&&
      refreshSceneWorld49P(graph,life.main190),"test undertrack drop requires the racer's scene forest");
  const float fromY=s.nodes[static_cast<std::uint32_t>(life.main190)].world100.v[13];
  const float step=rr::game::nativeTestUndertrackY50UT-99.f-fromY;
  std::cerr<<"test_undertrack armed slot="<<actualRacerSlot<<" frame="<<completedFrames<<" from_y="<<fromY<<'\n';
  const SceneDisplacement49P down{{0.f,step,0.f,0.f}};
  for(const auto node:{life.main190,life.physical19C,life.hidden198}){
   if(node==SceneNodeIndex49P::none||static_cast<std::uint32_t>(node)>=s.sceneNodeCount||
      s.nodes[static_cast<std::uint32_t>(node)].parent180!=SceneNodeIndex49P::none)continue;
   require50DU(addParentlessTranslation49P(graph,node,down)&&refreshSceneWorld49P(graph,node),
       "test undertrack drop scene setter rejected");
  }
  auto& bodies=s.bodies;
  bodies.main.components.root.scene=s.nodes[static_cast<std::uint32_t>(life.main190)].world100;
  bodies.mainSphere.sceneFlags190=(bodies.mainSphere.sceneFlags190&0xffffff00u)|s.nodes[static_cast<std::uint32_t>(life.main190)].sceneFlags190;
  bodies.secondary.root.scene=s.nodes[static_cast<std::uint32_t>(life.physical19C)].world100;
  bodies.secondary.sceneFlags190=(bodies.secondary.sceneFlags190&0xffffff00u)|s.nodes[static_cast<std::uint32_t>(life.physical19C)].sceneFlags190;
  std::memcpy(bodies.main.priorPoint7B0,s.visualShared.zero434630.v,16);
 }
 runNative([&]{return scheduleVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenAfter35C),liveSubstepScale,x.workspace,x.diagnostic);},"schedule");
 require50DU(s.bodies.main.schedule.count==1,"16ms diagnostic step requires one native substep");
 // PC ENHANCEMENT, NOT ORIGINAL (native_zero_gravity50zg.h): N64 zero gravity
 // for every racer through the one step. Inactive (no ZeroGravity data, or
 // switched off) -> no context, nothing below changes. The progress words are
 // the ones 2255B0 itself picks by +58: player C8C->C88 on the map line, AI
 // C84->C80 on the AI line. The context lives for this racer's step only.
 std::optional<rr::game::ScopedZeroGravityStep50ZG> zeroGravityScope;
 if(rr::game::zeroGravity50ZG.active()){
  rr::game::ZeroGravityStepInput50ZG zg{};
  zg.human=s.fields.vehicle58==0u;
  zg.from=zg.human?navigationProgress.reset.vehicleC8C:navigationProgress.vehicleC84;
  zg.to=zg.human?navigationProgress.reset.vehicleC88:navigationProgress.reset.vehicleC80;
  for(unsigned i=0;i<3;++i)zg.position[i]=s.bodies.main.components.root.body.position.v[i];
  zg.steer=s.bodies.main.controls.player.raw.values[0];        // both: the racer's own steering channel
  zg.pitchStick=s.bodies.main.components.bindings.aeroAux50;    // stick Y as 48W binds it (human only read)
  zg.speed=s.bodies.main.cachedSpeed34;
  zg.finished=(s.fields.impactFlagsCD8&1u)!=0u;
  zg.dt=s.bodies.main.schedule.factorPerSubstep;
  zeroGravityScope.emplace(rr::game::zeroGravity50ZG.prepareStep(actualRacerSlot,zg));
 }

 runNative([&]{return beginVehicleSubstep49W(s,x.bindings,x.selectors,x.workspace,x.diagnostic);},"native selector");
 // Stage S (consumers.md B4): this racer's own 690/364/348 rows come from the
 // one storage accessor at the point of use, never from a reference bound once
 // at cold. Legacy layout: the same members and refs as the former bindings.
 using QueryRoleS=track_runtime_storage50IG::RosterRole;
 const auto mainRowNow=trackStorage.currentRow(QueryRoleS::main690);
 const auto secondaryRowNow=trackStorage.currentRow(QueryRoleS::secondary364);
 const auto terrainRowNow=trackStorage.currentRow(QueryRoleS::terrain348);
 require50DU(mainRowNow&&secondaryRowNow&&terrainRowNow,"actual racer 690/364/348 query rows unavailable");
 auto& mainRowMember=*mainRowNow.state;auto& secondaryRowMember=*secondaryRowNow.state;
 const VehicleLiveQueryContext50EQ mainRowLive{mainLiveQuery.lifecycle,mainLiveQuery.world,
     mainRowMember,mainLiveQuery.workspace,mainRowNow.ref};
 const VehicleLiveQueryContext50EQ secondaryRowLive{secondaryLiveQuery.lifecycle,secondaryLiveQuery.world,
     secondaryRowMember,secondaryLiveQuery.workspace,secondaryRowNow.ref};
 live_terrain_probe50fr::Context terrainRowContext{terrainContext.lifecycle,terrainContext.world,
     *terrainRowNow.state,terrainContext.membershipWork,terrainRowNow.ref,terrainContext.targets,
     terrainContext.targetCount,terrainContext.selected,terrainContext.selectedCapacity,terrainContext.queryWork,
     terrainContext.effects,terrainContext.effectCapacity};
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
    const auto& source=secondary?secondaryQuery:query;const auto& member=secondary?secondaryRowMember:mainRowMember;
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
    const auto& source=secondary?secondaryQuery:query;const auto& member=secondary?secondaryRowMember:mainRowMember;
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
     mainRowMember,mainRowNow.ref,admission),"actual registered native main detector policy unavailable");
 runNative(collectRegistered,"ordered prepared-list vehicle candidates");
 // Step R2 (inert): type the merged 690 list the original's 358 detector walks.
 // A pod694 entry needs the in-list-order 294BB0 dispatch inside the indexed
 // query, which is not wired yet; until step R5 the PORT STAND-IN exclusion
 // list keeps every pod out, so this only counts (and refuses a surprise).
 {roster_pod_contact50PC::SelectedList selected;
  require50DU(roster_pod_contact50PC::classify(trackStorage,racerBank.count(),candidates.data(),candidates.size(),
      trackStorage.mergedIds.data(),registeredCount,registeredIndices,registeredCount,selected),
      "pod contact: collected 690 list classification rejected");
  auto& podTotals=roster_pod_contact50PC::totals();++podTotals.classified;
  podTotals.mergedPods+=selected.pods;podTotals.mergedOtherRows+=selected.otherRows;
  require50DU(!selected.pods&&!selected.otherRows,
      "pod contact: roster row in the 690 list before step R5 (294BB0 in-order dispatch not wired)");
  if(actualRacerSlot==0&&completedFrames%3600u==0u)roster_pod_contact50PC::reportSummary(std::cerr,completedFrames);}
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
 const auto mainQueryAttempt=withMcTriggerLease(false,[&](){return withGbTriggerLease(false,[&](){return withTbTriggerLease(qfilter.identity,[&](){return withRcTriggerLease([&](){return withTcTriggerLease(qfilter.identity,[&](){return withScTriggerLease([&](){return withRaTriggerLease([&](){return queryMainLiveIndexedStaticVehicle50EQ(s,x.bindings,admission,mainRowLive,query,candidates.data(),candidates.size(),registeredIndices,registeredCount,initial,workspace,destination,queryResult);});});});});});});});
 if(isRaTrack&&mainQueryAttempt.prefixCommitted)
  require50DU(raWorld->observeRuntimeTriggerPoses(racerBank,actualRacerSlot,raDiagnostic),
      "RA committed query pose observation rejected");
 if(gcTriggerActions)require50DU(gcTriggerActions->endQuery(),"GC main source trigger action failed after contact commit");
 if(gaTriggerActions)require50DU(gaTriggerActions->endQuery(),"GA main source trigger action failed after contact commit");
 if(maTriggerActions)require50DU(maTriggerActions->endQuery(),"MA main trigger action failed after contact commit");
 if(saTriggerActions)require50DU(saTriggerActions->endQuery(),"SA main trigger action failed after contact commit");
 if(!isTcTrack&&!isGbTrack&&!isMcTrack&&mainQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
    recoverPortalBoundary(mainQueryAttempt.prefix,"main")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{return acceptLiveQuery(mainQueryAttempt,query,mainRowMember,mainCrossedPortals,"main");},
     "registered main terrain query");
 require50DU(collisionNormalMode.acceptCompleted(queryResult.normalModeWrite),"source collision normal-mode binding unavailable");
 runNative([&]{return commitCandidateEffects(effects);},"commit main query candidate effects");qfilter.flagsF8=queryResult.queryFlagsF8;query.initialQueryCurrentIdentity=queryResult.currentCandidateIdentity;initial.selected=queryResult.selected;
 spring.records=queryResult.records;std::memcpy(records,queryRecords,sizeof records);
 }

 if(s.bodies.vehicle838){
 MainStaticDetectorAdmission50H secondaryAdmission{};
 require50DU(detectorPolicy->admission(true,s,x.bindings,secondaryQuery,secondarySpring,secondarySettings,
     secondaryRowMember,secondaryRowNow.ref,secondaryAdmission),"actual registered native secondary detector policy unavailable");
 runNative([&]{return prepareSecondaryLiveQueryScene50EQ(s,x.bindings,secondaryAdmission,secondaryRowLive,x.workspace)==SecondaryQuerySceneStatus50J::Ready;},"registered original secondary scene getter");
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
 const auto secondaryQueryAttempt=withMcTriggerLease(true,[&](){return withGbTriggerLease(true,[&](){return withTbTriggerLease(secondaryFilter.identity,[&](){return withRcTriggerLease([&](){return withTcTriggerLease(secondaryFilter.identity,[&](){return withScTriggerLease([&](){return withRaTriggerLease([&](){return querySecondaryLiveIndexedStaticVehicle50EQ(s,x.bindings,secondaryAdmission,secondaryRowLive,secondaryQuery,candidates.data(),candidates.size(),registeredIndices,registeredCount,si,sw,sd,secondaryResult);});});});});});});});
 if(isRaTrack&&secondaryQueryAttempt.prefixCommitted)
  require50DU(raWorld->observeRuntimeTriggerPoses(racerBank,actualRacerSlot,raDiagnostic),
      "RA committed query pose observation rejected");
 if(gcTriggerActions)require50DU(gcTriggerActions->endQuery(),"GC secondary source trigger action failed after contact commit");
 if(gaTriggerActions)require50DU(gaTriggerActions->endQuery(),"GA secondary source trigger action failed after contact commit");
 if(maTriggerActions)require50DU(maTriggerActions->endQuery(),"MA secondary trigger action failed after contact commit");
 if(saTriggerActions)require50DU(saTriggerActions->endQuery(),"SA secondary trigger action failed after contact commit");
 if(!isTcTrack&&!isGbTrack&&!isMcTrack&&secondaryQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
    recoverPortalBoundary(secondaryQueryAttempt.prefix,"secondary")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{return acceptLiveQuery(secondaryQueryAttempt,secondaryQuery,secondaryRowMember,secondaryCrossedPortals,"secondary");},
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
     terrainOwner.metadata,terrainRowContext,probeIndices,probeCount,terrainResult);
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
 // PC ENHANCEMENT, NOT ORIGINAL (native_surface_traction50st.h,
 // native_zero_gravity50zg.h): N64 surface grip, speed factor and speed bonus
 // from the course's TerrainZone data, plus the zero-g speed floor and aero,
 // every racer. All inputs are at their defaults on stock data and with the
 // switches off, and the original entry point then runs unchanged.
 rr::game::SurfaceForceInputs50ST surfaceForces{};
 surfaceForces.lateralScale=rr::game::surfaceRules50ST.lateralScale(actualRacerSlot);
 surfaceForces.speedFactor=rr::game::surfaceRules50ST.speedFactor(actualRacerSlot,
     s.bodies.main.components.probe348.field104!=0u);
 surfaceForces.speedBonus=rr::game::surfaceRules50ST.speedBonus(actualRacerSlot);
 {
  // N64 target speed at zero throttle: the bonus, and in zero-g at least 75
  // (ZON bonus 75-speed, ZOFF floor). Revenge has no thrust without drive,
  // so the floor is a drive floor while the pod is below it (NOTES.md 4.3).
  const bool zeroG=rr::game::zeroGravityStepAny50ZG();
  const float floor=zeroG?std::max(rr::game::zeroGravityMinimumSpeed50ST,surfaceForces.speedBonus):surfaceForces.speedBonus;
  const bool driving=s.bodies.main.vehicleState40==0u&&!(s.fields.impactFlagsCD8&1u);
  surfaceForces.minimumDrive=driving&&floor>0.f&&s.bodies.main.cachedSpeed34<floor?1.f:0.f;
  surfaceForces.untiltedAero=rr::game::zeroGravityStepIs50ZG(rr::game::ZeroGravityMode50ZG::Flight);
 }
 runNative([&]{return surfaceForces.isOriginal()?
     produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic):
     produceVehicleForcesSurface50ST(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic,
         surfaceForces);},"native main forces");
 ResponseStorage50K responseStorage;auto responseWorkspace=responseStorage.view(x.workspace);
 // Step R4: a generated record naming another body (other44, from 294BB0 or
 // 219420) runs the two-body 2964C0; the other bodies are read from and
 // committed to their own saved slots, bounds-checked, only after the frame
 // commit succeeded. With no other44 record this is exactly the static call.
 bool podRecords=false;
 for(std::size_t i=0;i<40u&&static_cast<std::int32_t>(i)<spring.records.generatedCount84;++i)
  if(records[i].other44.value)podRecords=true;
 if(!podRecords)
 runNative([&]{return respondMainStaticVehicle50G(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,query.queryScene,1,spring,records,40,responseWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual main contact response");
 else{
  require50DU(dynamicRegistry.prepared()&&dynamicRegistry.count()==racerBank.count()&&actualRacerSlot<racerBank.count(),
      "pod contact: body identity registry unavailable for a two-body response");
  roster_pod_contact50PC::SlotAccess access(racerBank,s,x.parameters,actualRacerSlot);
  require50DU(access.ready(),"pod contact: visited racer slot lease rejected");
  const auto primary=dynamicRegistry.bodyIdentityForSlot(actualRacerSlot).value;
  std::array<SpringModelBody48K,roster_pod_contact50PC::maximumOthers> others{},othersOut{};
  std::array<std::size_t,roster_pod_contact50PC::maximumOthers> otherSlots{};std::size_t otherCount=0;
  for(std::size_t i=0;i<40u&&static_cast<std::int32_t>(i)<spring.records.generatedCount84;++i){
   const auto id=records[i].other44.value;if(!id)continue;
   bool seen=false;for(std::size_t n=0;n<otherCount;++n)if(others[n].identity==id)seen=true;
   if(seen)continue;
   std::size_t slot=racerBank.count();
   for(std::size_t k=0;k<racerBank.count();++k)if(dynamicRegistry.bodyIdentityForSlot(k).value==id){
    require50DU(slot==racerBank.count(),"pod contact: duplicate body identity");slot=k;}
   require50DU(slot<racerBank.count()&&slot!=actualRacerSlot&&otherCount<others.size(),
       "pod contact: record names no other racer body");
   require50DU(access.gatherBody(slot,id,others[otherCount])&&access.fitsBody(slot),
       "pod contact: other racer body unavailable or its response would not fit");
   otherSlots[otherCount++]=slot;
  }
  struct PodResponseStorage {
   SpringGeneratedRecord48K projected[60]{};SpringRecord48N staged[60]{};std::uint8_t flags[60]{},nextFlags[60]{};
   SpringModelBody48K scratch[podResponseBodyCapacity50PC]{},prepared[podResponseBodyCapacity50PC]{};
  } podStorage;
  const VehiclePodResponseWorkspace50PC podWorkspace{{podStorage.projected,60,
      {podStorage.scratch,podResponseBodyCapacity50PC,podStorage.flags,60},
      {podStorage.prepared,podResponseBodyCapacity50PC,podStorage.nextFlags,60}},podStorage.staged,60,&x.workspace};
  runNative([&]{return respondMainVehicleWithPods50PC(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,
      query.queryScene,primary,others.data(),otherCount,othersOut.data(),spring,records,40,podWorkspace,x.diagnostic)==
      VehicleStaticResponseStatus50G::Complete;},"original two-body main contact response");
  for(std::size_t n=0;n<otherCount;++n)
   require50DU(access.commitBody(otherSlots[n],othersOut[n]),"pod contact: other racer response write refused");
  auto& podTotals=roster_pod_contact50PC::totals();podTotals.twoBodyResponses+=otherCount;
  for(std::size_t n=0;n<otherCount;++n)roster_pod_contact50PC::reportPair("2964C0","main",actualRacerSlot,otherSlots[n],0,0);
 }

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
  // Stage S: this racer's own 7A4 row through the one storage accessor.
  const auto movementRowNow=trackStorage.currentRow(QueryRoleS::movement7A4);
  require50DU(bool(movementRowNow),"actual racer 7A4 query row unavailable");
  runNative([&]{return updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,
      *movementRowNow.state,bounds,membershipWork).status==LiveZoneStatus50EN::Complete;},
      "source registered7A4 movement update");
  const auto& cached=movementRowNow.state->bounds;
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
  require50DU(damageReceipt.okay(),damageReceipt.status==EngineDamageStatus50IA::RandomRange?
      "original engine damage modulation rejected (random range)":
      "original engine damage modulation rejected (input or curve state)");
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
 // Surface enhancement: targets from this completed step's probe list, then
 // the 0.5/s easing (inert and silent when off or on stock data).
 // Zero-g: in Flight the surface is the floor under the racing line.
 const rr::game::SurfaceZeroGravityStep50ST zeroGravitySurface{
     rr::game::zeroGravityStepIs50ZG(rr::game::ZeroGravityMode50ZG::Flight),
     rr::game::zeroGravity50ZG.stepLineOwner(actualRacerSlot),s.bodies.main.cachedSpeed34};
 rr::game::surfaceRules50ST.update(actualRacerSlot,terrainResult.terrain,
     (s.fields.impactFlagsCD8&1u)!=0u,s.bodies.main.schedule.factorPerSubstep,zeroGravitySurface);
 if(rr::game::zeroGravity50ZG.active()){
  // Ground distance for the ZOFF landing test (< 12 ft): the probe's own
  // contact point (body + basis x 1C0 offset, as 48G measures its gap) to the
  // committed surface point along its normal. +inf when the probe saw nothing.
  float groundDistance=std::numeric_limits<float>::infinity();
  const auto& surface=terrainResult.terrain.surface;
  const auto main190=static_cast<std::uint32_t>(x.bindings.lifecycle.main190);
  if((surface.hit110||surface.hit114)&&main190<s.sceneNodeCount){
   const auto& w=s.nodes[main190].world100.v;const auto& o=s.terrainProbe.response.localOffset1c0.v;
   float gap=0.f;
   for(unsigned i=0;i<3;++i){
    const float probe=s.bodies.main.components.root.body.position.v[i]+w[i]*o[0]+w[4+i]*o[1]+w[8+i]*o[2];
    gap+=(probe-surface.point150.v[i])*surface.normal170.v[i];
   }
   if(std::isfinite(gap))groundDistance=gap;
  }
  rr::game::zeroGravity50ZG.afterStep(actualRacerSlot,terrainResult.terrain,groundDistance,
      s.bodies.main.cachedSpeed34,rr::game::surfaceRules50ST,rr::game::zeroGravityStep50ZG);
  // TEST ONLY (forced zero-g runs): the player's attitude in flight, so the
  // stick/turn senses can be read from logs (nose = body Z axis, wing = X).
  static const bool zeroGAttitudeLog=std::getenv("RR_TEST_FORCE_ZERO_G")!=nullptr;
  static std::uint64_t zeroGAttitudeSteps=0;
  if(zeroGAttitudeLog&&actualRacerSlot==0&&main190<s.sceneNodeCount&&
     rr::game::zeroGravity50ZG.mode(0)==rr::game::ZeroGravityMode50ZG::Flight&&(zeroGAttitudeSteps++%15u)==0u){
   const auto& w=s.nodes[main190].world100.v;
   std::fprintf(stderr,"zero_g_attitude step=%llu stick=%.3f steer=%.3f wing_x=(%.3f,%.3f,%.3f) nose_z=(%.3f,%.3f,%.3f) speed=%.1f\n",
       (unsigned long long)zeroGAttitudeSteps,s.bodies.main.components.bindings.aeroAux50,
       s.bodies.main.controls.player.raw.values[0],w[0],w[1],w[2],w[8],w[9],w[10],s.bodies.main.cachedSpeed34);
  }
 }
 zeroGravityScope.reset();
 addCounter50DU(completedFrames);
 if(m.components.probe348.field104)++terrainSupportedFrames;
 if(m.controls.player.flagsB9C&2u)addCounter50DU(repairActiveTicks);
 if(spring.records.contact3C)addCounter50DU(mainContactFrames);
 if(s.bodies.vehicle838&&secondarySpring.records.contact3C)addCounter50DU(secondaryContactFrames);
 return true;
 
}
} // namespace interactive_frame50DU

#include "player_ai_pilot50du.h"
// Source-identical Physics suffix; all queries retain the original live Context.
#include "terrain_interactive_frame_calls50du.h"
#include "terrain_interactive_storage50du.h"
#include "runtime_startup50fl_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "original_ai_audio_owner.h"
#include "original_ai_physical_recovery.h"     // 1B27C0 -> 2255B0 trigger reset (LostAI Reset)
#include "course_trigger_actions50ct.h"        // the one LostAI Reset sink slot
#include "native_gc_water_racer50kg_fixture.h" // 216B78 reset height domain (A9)
#include "../../src/game/ai_runtime50iv.h"
#include "../../src/game/collision_result_binding50mb.h"
#include "track_collision_world50cr.h"   // R3: the one collision world and its per-racer query bracket
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
#include "../../src/game/n64_track_hazards_runtime50nh.h"
#include "../../src/game/native_destroyed_racers50pd.h"
#include "../../src/game/vehicle_frame_surface50st.h"
#include <algorithm>
#include <limits>
#include <optional>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>

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
  // The frame closes as every frame does: the body phase markers (host bookkeeping of
  // the frame transaction, no original field) return to their between-frames values
  // with the frame phase. The destroy commit (commitOriginalDestroyed) jumps the frame
  // to AfterBodyChildren from wherever the read/bind left the bodies (PostPlayer /
  // PostBase); without this the bodies kept that stale phase for the whole wreck, so
  // after 228110 the next ordinary frame entry (readAiVehicleFrame50IV /
  // readVehicleFrame49W start(): phases()) refused with error=2 (Phase) - cr17 TA slot 7
  // at the Pause RESTART setup tick. Same for the human and every AI, every course.
  s.bodies.main.phase=MainBodyPhase49K::FrameReady;s.bodies.phase=CoupledBodyPhase49M::FrameReady;
  s.bodies.secondary.phase=SecondaryPhase49M::Ready;
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
 // PORT EXTENSION, NOT ORIGINAL (n64_track_hazards_runtime50nh.h): the N64
 // respawn after a column wreck places the restored racer on the line through
 // the same 228BBC -> 2255B0 under-track reset the original runs for every
 // racer, reached by this same drop. Only when this racer's reset owner exists
 // (human: the course's human owner; AI: original recovery + original AI);
 // otherwise the racer stays restored where it was (logged).
 bool n64RespawnDrop=rr::game::n64HazardRuntime50NH.takePlacement(actualRacerSlot);
 if(n64RespawnDrop){
  const bool human=s.fields.vehicle58==0u;
  const bool owned=human?(humanCourseEnabled&&actualRacerSlot==0u):(originalRecoveryEnabled&&originalAiEnabled);
  rr::game::n64Log50NH("n64_respawn placement slot=%u human=%d via=%s frame=%llu\n",unsigned(actualRacerSlot),int(human),
      owned?"228BBC_2255B0":"none",static_cast<unsigned long long>(completedFrames));
  if(!owned)n64RespawnDrop=false;
 }
 const bool testUndertrackDrop=actualRacerSlot<rr::game::nativeTestUndertrackPending50UT.size()&&
     rr::game::nativeTestUndertrackPending50UT[actualRacerSlot];
 if(testUndertrackDrop||n64RespawnDrop){
  if(testUndertrackDrop)rr::game::nativeTestUndertrackPending50UT[actualRacerSlot]=false;
  const auto& life=x.bindings.lifecycle;
  const SceneGraph49P graph{s.nodes,s.sceneNodeCount};
  require50DU(validVehicleSceneCount49W(s,x.bindings)&&validateSceneGraph49P(graph)&&
      static_cast<std::uint32_t>(life.main190)<s.sceneNodeCount&&
      static_cast<std::uint32_t>(life.physical19C)<s.sceneNodeCount&&
      refreshSceneWorld49P(graph,life.main190),"test undertrack drop requires the racer's scene forest");
  const float fromY=s.nodes[static_cast<std::uint32_t>(life.main190)].world100.v[13];
  const float step=rr::game::nativeTestUndertrackY50UT+rr::game::undertrackPlane50ZG-99.f-fromY;
  std::cerr<<(testUndertrackDrop?"test_undertrack armed":"n64_respawn drop")<<" slot="<<actualRacerSlot<<" frame="<<completedFrames<<" from_y="<<fromY<<'\n';
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
  const bool pilot=player_ai_pilot50DU::drives(actualRacerSlot)&&s.fields.vehicle58==0u;
  zg.human=s.fields.vehicle58==0u&&!pilot;
  const auto& route=pilot?originalAi.pilotNavigation:navigationProgress;
  zg.from=zg.human?route.reset.vehicleC8C:route.vehicleC84;
  zg.to=zg.human?route.reset.vehicleC88:route.reset.vehicleC80;
  for(unsigned i=0;i<3;++i)zg.position[i]=s.bodies.main.components.root.body.position.v[i];
  zg.steer=s.bodies.main.controls.player.raw.values[0];        // both: the racer's own steering channel
  zg.pitchStick=s.bodies.main.components.bindings.aeroAux50;    // stick Y as 48W binds it (human only read)
  // Lean: channel 2 = conditioned (255 - packet byte 4), so right stick right /
  // D gives -1; negate so + = right. Default preset only (input binding sets it).
  zg.lean=zg.human&&rr::game::zeroGravity50ZG.humanLeanBound()?-s.bodies.main.controls.player.raw.values[2]:0.f;
  zg.speed=s.bodies.main.cachedSpeed34;
  zg.finished=(s.fields.impactFlagsCD8&1u)!=0u;
  zg.dt=s.bodies.main.schedule.factorPerSubstep;
  {// Logs only (zero_gravity sample): travel against the pod's own axes.
   const auto main190=static_cast<std::uint32_t>(x.bindings.lifecycle.main190);
   if(main190<s.sceneNodeCount){const auto& w=s.nodes[main190].world100.v;zg.haveMotion=true;
    for(unsigned i=0;i<3;++i){zg.velocity[i]=s.bodies.main.components.root.body.velocity.v[i];
     zg.right[i]=w[i];zg.up[i]=w[4+i];zg.forward[i]=w[8+i];}}
  }
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
 // Common rewrite (I4, B-C11/B-C13): ONE query bracket for every course, over
 // the track's one collision world (track.roles.collision) and its one trigger
 // group (274E90, 2A00A0/2A0E20/2A0B30). The trigger lease names this racer's
 // main690 identity, this query's collider, the racer's C80 node and the
 // accepted camera listener; it spans the actual synchronous indexed query
 // only and ends on every exit before any Bank switch or observation. A
 // committed prefix is published to the trigger actions (observePrepared).
 // Retires the RA C80 lease, the RC/SC/TB/TC/GA/GC/MA/common-route leases and
 // the MC/GB typed Bank bindings.
 require50DU(bool(track.roles.collision),"track collision world (roles.collision, 05.04) absent");
 auto& collisionWorld=*track.roles.collision;
 const Vector3_48C queryListener{{audioListener.x,0.f,audioListener.z}};
 auto withTrackQuery=[&](bool secondary,std::uint32_t actualCollider,auto&& queryOperation){
  const track_collision_world50CR::QueryLease lease{actualRacerSlot,qfilter.identity,actualCollider,
      &navigationProgress.reset.vehicleC80,audioListenerReady?&queryListener:nullptr,audioListenerReady?1u:0u,
      player_ai_pilot50DU::drives(actualRacerSlot)?&originalAi.pilotNavigation.reset.vehicleC80:nullptr};
  VehicleLiveQueryResult50EQ result{};bool entered=false;
  const bool accepted=collisionWorld.queryRacer(racerBank,lease,[&](track_runtime_storage50IG::Storage& actual){
   if(&actual!=&trackStorage)return false;entered=true;result=queryOperation();
   if(result.status==VehicleLiveQueryStatus50EQ::Complete&&result.prefixCommitted){
    const auto& source=secondary?secondaryQuery:query;const auto& member=secondary?secondaryRowMember:mainRowMember;
    if(!collisionWorld.observePrepared(source.filter->identity,*source.resource,member.lastShapeWorld))return false;
   }
   return result.status==VehicleLiveQueryStatus50EQ::Complete;
  });
  require50DU(entered,"track query admission rejected before indexed work");
  require50DU(result.status!=VehicleLiveQueryStatus50EQ::Complete||accepted,
      "track completed indexed query failed its trigger owner publication");
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
 {
 // LostAI Reset TRUE (1B5A60 -> 1B27C0: C80 store at 1B282C, then 2255B0 on
 // the matching racer at 1B283C), one path for every course. 2255B0 runs inside
 // this racer's own main690 E8 query at the PostWorld cut, so only this query
 // can lend it: the outer-result binding of exactly this query (124CB0's ground
 // probe nests under it), and the racer's reset owners (the PostWorld D40/D44
 // borrow sourceTriggerResetOwners50MB, cables, lights, AI view, world). Bound
 // for an ORIGINAL AI racer only: 2255B0's player branch from a trigger is not
 // carried (PORT STAND-IN flagged in course_trigger_actions50ct.h; the
 // original branches on +58 here, A7). Unbound -> the action owner's preflight
 // refuses a reached Reset action (logged); nothing else changes.
 namespace air=native_original_ai_recovery;
 std::uint8_t* lostAiFlags22C=nullptr;std::uint32_t* lostAiField24C=nullptr;
 auto lostAiHeightDomain=selectedRuntimeHeightDomain;
 const auto& lostAiRelative=x.secondaryInputs.targetRelative2F0;
 const VisualVector49R lostAiTarget{{lostAiRelative.v[0],lostAiRelative.v[1],lostAiRelative.v[2],lostAiRelative.v[3]}};
 std::optional<UndertrackQueryOwners50EW> lostAiQueries;
 std::optional<air::Owners> lostAiOwners;
 if(track.roles.triggerActions&&track.roles.triggers&&track.roles.lights&&
    s.fields.vehicle58==1u&&sourceAiAudio&&originalRecoveryEnabled&&originalAiEnabled&&
    originalAiAudio&&originalResetGround&&originalRecoveryWorkspace&&
    actualRacerSlot<racerCables.size()&&racerCables[actualRacerSlot]&&
    originalAiAudio->sourceTriggerResetOwners50MB(actualRacerSlot,originalAi.racer60,s,lostAiFlags22C,lostAiField24C)&&
    lostAiFlags22C&&lostAiField24C){
  // 216B78: the actual terrain330/water334 owners of this racer's world (the
  // same rule as the frame children's resets; no water -> the selected domain).
  const bool heightBound=!track.hasWater()||(actualRacerSlot<nativeWaterRacers.size()&&
      nativeWaterRacers[actualRacerSlot]&&nativeWaterRacers[actualRacerSlot]->bindResetDomain(
          selectedRuntimeHeightDomain,terrainOwner.lift1F0,lostAiHeightDomain));
  if(heightBound){
   lostAiQueries.emplace(recoveryQueries());
   // Physical query[0] must be THIS main690 query (checked again at 1B283C).
   if(lostAiQueries->query[0].type2==&query)
    lostAiOwners.emplace(air::Owners{*track.roles.lights,racerCables[actualRacerSlot]->visibility50(),
        *lostAiFlags22C,*lostAiField24C,aiView,lostAiHeightDomain,resetGlobals,resetLifecycle,
        lostAiTarget,*lostAiQueries,zoneLifecycle,liveWorld,membershipWork,*originalResetGround,
        originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount});
  }
 }
 struct LostAiResetBinding {
  Context* self=nullptr;std::size_t slot=0;std::uint32_t main690=0;
  rr::game::CollisionResultBindingOwner50MB* resultBinding=nullptr;
  const rr::game::SourceOuterResultLease50MB* outer=nullptr;
  const void* issuer=nullptr;std::uint64_t generation=0;
  air::Owners* owners=nullptr;
 };
 const auto runMainQuery=[&]{return queryMainLiveIndexedStaticVehicle50EQ(s,x.bindings,admission,mainRowLive,query,candidates.data(),candidates.size(),registeredIndices,registeredCount,initial,workspace,destination,queryResult);};
 const auto mainQueryAttempt=withTrackQuery(false,qfilter.identity,[&](){
  if(!lostAiOwners)return runMainQuery();
  // Inside queryRacer: the action owner's QueryBorrow has opened this query's
  // serial (2A00A0 bracket). Anything else unbinds the reset (refused if reached).
  const auto& group=*track.roles.triggers;
  const std::uint64_t generation=aiView.generation();
  if(!group.queryActive||group.queryCollider!=qfilter.identity||!group.querySerial||!generation||
     !query.filter||query.filter->identity!=qfilter.identity||!query.resource||initialMetadata.size()!=initialPairs.size()||
     initialPairs.size()<query.resource->rayCount||
     (registeredCount&&!registeredIndices)||initial.recordCount>std::size(records)){
   std::cerr<<"lostai_reset unbound slot="<<actualRacerSlot<<" reason=query_binding_inputs\n";
   return runMainQuery();
  }
  // Host-side outer-result binding for exactly this query. Declaration order:
  // the lease ends before its owner on every exit (the owner terminates if it
  // would die borrowed).
  rr::game::CollisionResultBindingOwner50MB resultBinding;
  rr::game::SourceOuterResultLease50MB outer;
  const rr::game::SourceQueryIdentity50MB identity{track.roles.triggerActions.get(),generation,
      group.querySerial,actualRacerSlot,qfilter.identity};
  if(!resultBinding.beginOuter(identity,query,queryResult,initialPairs.data(),initialMetadata.data(),
         initialPairs.size(),outer)||
     !resultBinding.bindOuterScratch(outer,workspace,destination,registeredIndices,registeredCount,
         initial.recordSlots,initial.recordCount)){
   if(resultBinding.borrowed()&&!resultBinding.endOuter(outer))std::terminate();
   std::cerr<<"lostai_reset unbound slot="<<actualRacerSlot<<" reason=outer_result_binding\n";
   return runMainQuery();
  }
  VehicleLiveQueryResult50EQ attempt{};
  {
   LostAiResetBinding context{this,actualRacerSlot,qfilter.identity,&resultBinding,&outer,
       track.roles.triggerActions.get(),generation,&*lostAiOwners};
   const course_trigger_actions50CT::LostAiResetSink::Binding sinkBinding{&context,actualRacerSlot,qfilter.identity,
    // Pure: this racer's reset owners are bound and the outer result is this query's.
    [](void* p,const course_trigger_actions50CT::ActionEvent& e)noexcept{
     const auto& c=*static_cast<const LostAiResetBinding*>(p);
     return c.owners&&c.resultBinding&&c.outer&&e.authored&&e.lease&&e.querySerial&&
         e.racerSlot==c.slot&&e.lease->racerMain690==c.main690&&e.otherCollider==c.main690&&
         c.resultBinding->outerQueryCurrent(*c.outer,c.self->query);
    },
    // 1B283C: 2255B0 for this racer (the action owner already wrote C80).
    [](void* p,const course_trigger_actions50CT::ActionEvent& e)noexcept{
     auto& c=*static_cast<LostAiResetBinding*>(p);auto& self=*c.self;
     if(!c.owners||!e.authored)return false;
     const air::TriggerQuery trigger{*c.resultBinding,*c.outer,c.issuer,c.generation,e.querySerial,
         e.candidateOrdinal,e.action,c.main690,e.authored->targetNodeIndex};
     const bool ok=air::resumeTrigger1B27C0(self.s,self.x.bindings,self.x.parameters,self.navigationProgress,
         self.originalAi,trigger,self.originalRecovery,*c.owners,*self.originalRecoveryWorkspace,
         self.originalRecoveryDiagnostic);
     std::cerr<<"lostai_reset slot="<<c.slot<<" trigger="<<(e.trigger+1)<<" action="<<e.action
         <<" c80="<<e.authored->targetNodeIndex<<" ok="<<ok<<" stage="<<static_cast<unsigned>(self.originalRecovery.stage)
         <<" pc="<<std::hex<<self.originalRecoveryDiagnostic.sourcePC<<std::dec
         <<" msg="<<(self.originalRecoveryDiagnostic.message?self.originalRecoveryDiagnostic.message:"")<<'\n';
     return ok;
    }};
   course_trigger_actions50CT::LostAiResetSink::Scope scope(track.roles.triggerActions->lostAiResetSink(),sinkBinding);
   if(!scope)std::cerr<<"lostai_reset unbound slot="<<actualRacerSlot<<" reason=sink_scope\n";
   attempt=runMainQuery();
  }
  if(!resultBinding.endOuter(outer))std::terminate();   // a nested ground result left borrowed
  return attempt;
 });
 // PORT STAND-IN (flagged, I2's host boundary): a PrefixRejected portal
 // boundary abandons the frame through recoverPortalBoundary, every course.
 if(mainQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
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

 {
 const auto secondaryQueryAttempt=withTrackQuery(true,secondaryFilter.identity,[&](){return querySecondaryLiveIndexedStaticVehicle50EQ(s,x.bindings,secondaryAdmission,secondaryRowLive,secondaryQuery,candidates.data(),candidates.size(),registeredIndices,registeredCount,si,sw,sd,secondaryResult);});
 if(secondaryQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
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
 // TEST ONLY: RR_TEST_SPEED_LOG=<every> logs the player's body speed, the
 // surface speed bonus and the boost words every <every> steps, with the
 // heading change since the last sample, so a straight can be picked from a
 // muted logs-only run and the boost gain read on it. Reads only.
 {
  static const long speedLogEvery=[]{const char* v=std::getenv("RR_TEST_SPEED_LOG");
      const long n=v?std::atol(v):0;return n>=1&&n<=100000?n:0L;}();
  if(speedLogEvery&&actualRacerSlot==0){
   static std::uint64_t speedLogSteps=0;static float speedLogYaw=0.f;static bool speedLogHaveYaw=false;
   const auto main190=static_cast<std::uint32_t>(x.bindings.lifecycle.main190);
   if(++speedLogSteps%static_cast<std::uint64_t>(speedLogEvery)==0&&main190<s.sceneNodeCount){
    const auto& w=s.nodes[main190].world100.v;
    const float yaw=std::atan2(w[8],w[10])*57.29578f;
    float turn=speedLogHaveYaw?yaw-speedLogYaw:0.f;
    if(turn>180.f)turn-=360.f;else if(turn<-180.f)turn+=360.f;
    speedLogYaw=yaw;speedLogHaveYaw=true;
    const auto& p=s.bodies.main.components.root.body.position.v;
    std::fprintf(stderr,"speed_log step=%llu speed=%.2f bonus=%.2f boost64=%u ratio70=%.3f heat7C=%.3f yaw=%.1f turn=%.2f pos=%.1f,%.1f,%.1f\n",
        static_cast<unsigned long long>(speedLogSteps),s.bodies.main.cachedSpeed34,
        rr::game::surfaceRules50ST.speedBonus(0),boostState.vehicle64,boostState.speedRatio70,boostState.heat7C,
        yaw,turn,p[0],p[1],p[2]);
   }
  }
 }
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
   std::fprintf(stderr,"zero_g_attitude step=%llu stick=%.3f steer=%.3f lean=%.3f tilt=%.3f wing_x=(%.3f,%.3f,%.3f) nose_z=(%.3f,%.3f,%.3f) speed=%.1f\n",
       (unsigned long long)zeroGAttitudeSteps,s.bodies.main.components.bindings.aeroAux50,
       s.bodies.main.controls.player.raw.values[0],
       rr::game::zeroGravity50ZG.humanLeanBound()?-s.bodies.main.controls.player.raw.values[2]:0.f,
       rr::game::zeroGravity50ZG.tilt(0),w[0],w[1],w[2],w[8],w[9],w[10],s.bodies.main.cachedSpeed34);
  }
 }
 // PORT EXTENSION, NOT ORIGINAL (n64_track_hazards_runtime50nh.h): N64 track
 // hazards after this racer's completed step. Inert (no call cost beyond the
 // test) unless the course's TOB carried N64 rows or the test switch is set.
 if(rr::game::n64HazardRuntime50NH.active()){
  rr::game::N64HazardRuntime50NH::StepInput hz{};
  auto& st=hz.step;st.slot=actualRacerSlot;st.human=s.fields.vehicle58==0u;
  const auto state40=s.bodies.main.vehicleState40;
  const bool limping=actualRacerSlot<rr::game::nativeDestroyedRacers50PD.size()&&rr::game::nativeDestroyedRacers50PD[actualRacerSlot].limping;
  st.alive=state40==0u&&!limping;
  st.notAliveReason=(state40==1u||state40==2u||limping)?"limping":"dead";
  for(unsigned i=0;i<3;++i)st.current[i]=s.bodies.main.components.root.body.position.v[i];
  hz.frameSeconds=s.bodies.main.frameFactor;
  st.lap=lapContinuity.state.vehicleCC8; // [I] CC8 = laps completed (N64 +0x78 lap 0/1/2)
  // Carrier owners (N64 0x800665A4: the surface under the pod). Flight: the
  // floor under the line (every owner of the .nft surface, else the zero-g
  // line owner); otherwise this step's collected static owners.
  std::array<std::uint32_t,64> carriers{};std::size_t carrierCount=0;
  hz.flight=rr::game::zeroGravityStepIs50ZG(rr::game::ZeroGravityMode50ZG::Flight);
  hz.recovering=rr::game::zeroGravityStepIs50ZG(rr::game::ZeroGravityMode50ZG::Recover);
  if(hz.flight){
   const auto& floor=rr::game::zeroGravity50ZG.floorTable();
   const auto* list=floor.lastValid(actualRacerSlot)?floor.owners(floor.lastSurface(actualRacerSlot)):nullptr;
   if(floor.lastValid(actualRacerSlot)){if(list)for(const auto o:*list)if(carrierCount<carriers.size())carriers[carrierCount++]=o;}
   else if(const auto o=rr::game::zeroGravity50ZG.stepLineOwner(actualRacerSlot);o!=rr::game::noSurfaceOwner50ST)carriers[carrierCount++]=o;
  }else{
   const auto& terrain=terrainResult.terrain;
   const auto count=std::min<std::uint32_t>(terrain.collectedCount,static_cast<std::uint32_t>(terrain.collectedIdentities.size()));
   for(std::uint32_t i=0;i<count&&carrierCount<carriers.size();++i){
    const auto identity=terrain.collectedIdentities[i];
    auto o=rr::game::surfaceRules50ST.ownerOfIdentity(identity);
    if(o==rr::game::noSurfaceOwner50ST)o=rr::game::n64HazardRuntime50NH.animatedCarrier(identity);
    if(o!=rr::game::noSurfaceOwner50ST)carriers[carrierCount++]=o;}
  }
  st.groundOwners=carriers.data();st.groundOwnerCount=carrierCount;
  hz.boosting=boostState.vehicle64!=0u; // [I] Revenge boost word for N64 WAS_BOOSTING
  const auto main190=static_cast<std::uint32_t>(x.bindings.lifecycle.main190);
  if(main190<s.sceneNodeCount){const auto& w=s.nodes[main190].world100.v;hz.forward[0]=w[8];hz.forward[1]=w[9];hz.forward[2]=w[10];}
  rr::game::n64HazardRuntime50NH.afterStep(hz);
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

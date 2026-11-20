// Cold stage bodies retain their original order and borrowed execution arena.
#include "native_last_failure50pd.h"
#include "terrain_interactive_cold_contacts50du.h"
#include "runtime_startup50fl_fixture.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "native_racer_draw50hn_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "track_collision_world50cr.h"            // track.roles.collision: Borrow::actions()
#include "course_trigger_actions50ct.h"           // the one trigger action owner (2A00A0/2A0E20)
#include "positional_sound_rows50sr.h"            // track.roles.sounds: Sound action sink
#include "track_particle_emitter_sink50cr.h"     // track.roles.emitters: Particles action sink (15A500)
#include "gc_gate_audio50in_fixture.h"            // triggerAudio: the one PodSound voice (shared kernel)
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
  // The visited racer's own 690/364 row pose (one storage accessor; shared
  // world: that slot's row, never slot 0's).
  auto rowShape=[&](track_runtime_storage50IG::RosterRole role)->const auto& {
   const auto row=trackStorage.currentRow(role);
   require50DU(bool(row),"actual visited racer 690/364 row unavailable");
   return row.state->lastShapeWorld;
  };
  // The neutral setup tick below performs genuine live trigger queries.
  // Bind the track's one trigger action owner (274E90 rows of
  // track.roles.triggerSource over the one 2A00A0/2A0E20/2A0B30 group
  // track.roles.triggers, lent by the one collision world) only after every
  // actual rig and final grid pose exists, but before any tick can acquire its
  // synchronous C80 lease. Material/GPU preparation remains at the later
  // renderer boundary. One path for every course: a track that authors no
  // trigger rows prepares an empty list, as 274E90 does.
  {
   require50DU(track.roles.collision&&track.roles.collision->geometryReady(),
       "the track's collision world (05.04) is not ready for trigger binding");
   require50DU(track.roles.triggerSource&&track.roles.triggers,
       "the track's trigger list/group (05.09) is unavailable");
   // Step 08 (2A1260/1ACF70) ran: LostAI targets are checked per action
   // against the live AI spline's own node count (no stock count).
   requireRuntimeNavigation();
   require50DU(aiView.runtimeValid()&&mapView.runtimeValid()&&aiView.sameOwner(mapView)&&aiView.sameWorld(liveWorld),
       "trigger LostAI targets require the step-08 map/AI views over the live world");
   require50DU(originalAiEnabled&&racerBank.count()==raceRacerCount&&
       ((allAiEnabled&&originalAiHumanCount==0&&originalAiOpponentCount==raceRacerCount)||
        (humanCourseEnabled&&!allAiEnabled&&originalAiHumanCount==1&&originalAiOpponentCount==raceOpponentCount)),
       "triggers require the actual human/AI manager roster and the live AI spline");
   auto borrow=track.roles.collision->borrow();
   require50DU(bool(borrow)&&borrow.world()==&liveWorld,"the track's collision world borrow is unavailable");
   auto* actions=borrow.actions();
   require50DU(actions!=nullptr,"the track's trigger action owner is unavailable");
   // PodSound destination: the one gate/pod voice (triggerAudio, World4D8
   // 1B5B00), present only when the trigger list authors a PodSound row.
   // Muted audio admits the action as a no-op, as the original voice does.
   course_trigger_actions50CT::ActionSink podSound{};
   if(triggerAudio){
    if(audioEnabled)podSound={.context=triggerAudio.get(),
        .preflight=[](void* p,const course_trigger_actions50CT::ActionEvent&)noexcept->bool{
         const auto* voice=static_cast<const gc_gate_audio50IN::Owner*>(p);return voice->available()&&voice->ready();},
        .apply=[](void* p,const course_trigger_actions50CT::ActionEvent& e)noexcept->bool{
         auto* voice=static_cast<gc_gate_audio50IN::Owner*>(p);
         if(e.authored)voice->start(e.authored->filename);else voice->start();return !voice->failed();},
        .sourcePc=0x1b5b00u};
    else podSound={.context=triggerAudio.get(),
        .preflight=[](void*,const course_trigger_actions50CT::ActionEvent&)noexcept->bool{return true;},
        .apply=[](void*,const course_trigger_actions50CT::ActionEvent&)noexcept->bool{return true;},
        .sourcePc=0x1b5b00u};
   }
   const course_trigger_actions50CT::Destinations destinations{
       .animations=track.roles.animation,
       .podSound=podSound,
       .sound=track.roles.sounds?track.roles.sounds->triggerSink():course_trigger_actions50CT::ActionSink{},
       // Particles: the ground-effects owner's own adapter (15A500/15A560, area E).
       .particles=track.roles.emitters?track_particle_emitters50CR::triggerSink(*track.roles.emitters):course_trigger_actions50CT::ActionSink{},
       // LostAI Reset TRUE (1B5A60 -> 2255B0 pose reset): the action owner's one
       // slot sink; the frame lends it per original-AI main690 query (frame_physics).
       .lostAiReset=actions->lostAiResetSink().sink()};
   // The source half (rows, group, LostAI targets) was prepared before the
   // collision world took the owner (world.cpp); bind the destinations now.
   require50DU(actions==track.roles.triggerActions.get(),"the collision world lends a different trigger action owner");
   require50DU(actions->bindDestinations(destinations),"trigger/action destinations rejected");
   // 2A00A0's chance draw: the one application RNG (308030) with its lazy
   // seed/range slots and the application clock; IgnoreTrigger PLAYER/AI
   // lists (2DC630) from the manager+34 human prefix.
   std::array<course_trigger_actions50CT::RacerIdentities,raceRacerCapacity> identities{};
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    identities[racer]={qfilter.identity,secondaryFilter.identity};
   });
   require50DU(actions->bindRacerIdentities(identities,originalAiHumanCount,
       {&s.noiseOwner().sharedRandomState,&sessionRandom,&applicationClock},racerBank.count()),
       "trigger racer identity/chance binding rejected");
   // The collision world publishes query readiness (the action owner's bindQuery)
   // once the identities are bound; observePrepared below needs a ready owner.
   require50DU(track.roles.collision->bindQuery(),"the track's collision world refused its trigger query binding");
   // Bound from here: the cold closer (I3) unbinds the action owner on failure.
   coldNavigationBound=true;
   for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
    require50DU(actions->observePrepared(qfilter.identity,*query.resource,rowShape(track_runtime_storage50IG::RosterRole::main690))&&
        actions->observePrepared(secondaryFilter.identity,*secondaryQuery.resource,rowShape(track_runtime_storage50IG::RosterRole::secondary364)),
        "trigger cold actual COL poses unavailable");
   });
   std::cerr<<"NATIVE TRIGGERS:"<<track.roles.triggerSource->liveCount()<<" authored rows, method1 zones, "
       <<2*racerBank.count()<<" actual racer COL identities; one 274E90 list, 2A00A0/2A0E20 actions; reverse108E90 parity unresolved.\n";
   if(audioEnabled&&triggerAudio)std::cerr<<"trigger PodSound uses the shared accepted-camera native listener in XZ and real World4D8 PCM; original listener-object parity is not claimed.\n";
  }
  // The track's one lighting owner (SCB Lights, prepared once at step 05.13).
  if(nativeSelectedLights)require50DU(track.roles.lights!=nullptr,"the track's lighting owner (05.13) is unavailable");
  if(nativeSelectedLights)for(std::size_t racer=0;racer<racerBank.count();++racer)
   racerBank.visit(racer,[&](){
    // Only this genuine unadvanced cold generation owns source C30=4C0[0].
    // Rig installation and final per-racer grid placement already completed.
    require50DU(completedFrames==0&&m.frameFactor==0.f&&graphicsSequence[racer]==0,
        "visibility creation requires final fresh grid before first graphics");
    require50DU(track.roles.lights->attachFreshVehicle(static_cast<std::uint32_t>(racer),s,
        x.bindings,x.parameters,lightDiagnostic),"actual cold racer7F0 registration failed");
    if(originalRecoveryEnabled){
     // Only the rooted course race's Player0 uses the human constructor role.
     // Geometry comes from the same selected pod/probe owners as every AI;
     // neither source vehicle58 nor native AI ownership is rewritten.
     const auto queryRole=rootedCourseRace&&humanCourseEnabled&&racer==0?
         undertrack_owners50ew_fixture::ColdQueryRole50LH::humanPlayer:
         undertrack_owners50ew_fixture::ColdQueryRole50LH::originalAi;
     //This racer's own 694/7A4 rows (shared world: that slot's pair).
     const auto podRow=trackStorage.currentRow(track_runtime_storage50IG::RosterRole::pod694);
     const auto movementRow=trackStorage.currentRow(track_runtime_storage50IG::RosterRole::movement7A4);
     const bool attached=undertrack_owners50ew_fixture::attachFreshQueries(recoveryQueryOwners,s,x.bindings,x.parameters,
         recoveryBorrowedQueries(),{podRow.ref,movementRow.ref},{podRow.state,movementRow.state},
         {podRow.storage,movementRow.storage},zoneLifecycle,liveWorld,
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
        original_racer_selection50IJ::findAnyByPodId(racerRigs[slot]->podId.c_str());
    require50DU(selection,"actual contact actor original racer60 unavailable");
    // An add-on row (PORT EXTENSION) has no named bank: generic lines only.
    require50DU(dynamicRacer60[slot]==selection->racer60&&
        sourceAnnouncer->collisionRequestsEnabled()&&
        (sourceAnnouncer->hasNamedBank(selection->racer60)||selection->addon),
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

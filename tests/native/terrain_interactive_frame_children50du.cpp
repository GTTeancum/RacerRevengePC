// Source-identical Children prefix; no owners are moved out of runSession.
#include "native_wreck_debris_sim50pd.h"
#include "native_wreck_effects50pd.h"
#include <cfenv>
#include "terrain_interactive_frame_calls50du.h"
#include "runtime_startup50fl_fixture.h"
#include "native_ai_session50iv_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "original_ai_audio_owner.h"
#include "original_ai_physical_recovery.h"
#include "original_human_physical_recovery.h"
#include "../../src/game/native_destroyed_racers50pd.h"
#include <bit>
#include "../../src/game/original_vehicle_damage50la.h"
#include <cstring>
#include "native_gc_water_session50kg_fixture.h"
#include "mc_particle_vehicle50mc.h"
#include "lap_drive50gj_fixture.h"
#include "live_audio50hf_fixture.h"
#include "race_mode1_host50id_fixture.h"
#include "material_audio_provider50hi_fixture.h"
#include "checkpoint_recovery50fi_fixture.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "../../src/game/dynamic_impact_events50iw.h"
#include "../../src/game/original_human_reset_timers50lc.h"
#include "../../src/game/controller_actions50ec.h"
#include "../../src/game/controller_ingress50cx.h"
#include "../../src/game/wrong_way_progress50go.h"
#include "../../src/game/player_finish50ia.h"
#include "../../src/game/vehicle_base_finish50y.h"
#include "../../src/game/boost_controls50fk.h"
#include "../../src/assets/effect_names50bl.h"
#include "../../src/assets/impact_authoring49y.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <variant>

namespace interactive_frame50DU {
namespace recovery50FI=checkpoint_recovery50fi_fixture;
namespace gcLighting=rr::game::original_gc_gameplay_lighting_candidate;
namespace f=runtime_startup50fl;
using namespace rr::game;
using detail::require50DU;
using detail::addCounter50DU;

bool Context::advanceChildren(std::size_t actualRacerSlot,float preparedOuterFactor,
    bool sourceAiAudio,NativeCalls& runNative) {
 require50DU(!originalUiActive,"Main-only frontend cannot advance race physics");
 require50DU(!nativeSelectedLights||actualRacerSlot<gcGraphicsSequence.size(),
     "GC light update requires the actual native racer slot");
 finishDriverAppliedThisFrame=false;
 // A destroyed racer (original state3/4) runs only its destroyed frame.
 if(s.bodies.main.vehicleState40==3u||s.bodies.main.vehicleState40==4u)
  return advanceDestroyedChildren(actualRacerSlot,preparedOuterFactor);
 if(actualRacerSlot<rr::game::nativeTestDestroyPending50PD.size()&&
    rr::game::nativeTestDestroyPending50PD[actualRacerSlot]){
  // Unattended test only (RR_TEST_DESTROY): the canonical B9D bit0 that
  // 222714 sets for a deadly hit; 225C50 then selects 2261B0.
  rr::game::nativeTestDestroyPending50PD[actualRacerSlot]=false;
  s.fields.flagB9D0=true;
  const int attacker=rr::game::nativeTestDestroyAttacker50PD[actualRacerSlot];
  if(attacker>=0){poseHistory.markerD30=static_cast<std::uint32_t>(attacker);poseHistory.timerD2C=0x40000000u;}
  std::cerr<<"test_destroy armed slot="<<actualRacerSlot<<" frame="<<completedFrames
      <<" wreck="<<rr::game::nativeTestDestroyWreck50PD[actualRacerSlot]<<'\n';
  if(rr::game::nativeTestDestroyWreck50PD[actualRacerSlot]&&originalAiEnabled){
   // Test-only: the 225D9C dispatch of 225C50 (slot 8C, 226750) in place of
   // the B9D explode bit; no damage word is written.
   rr::game::nativeTestDestroyWreck50PD[actualRacerSlot]=false;s.fields.flagB9D0=false;
   const ImpactTransitionResult50C forced{ImpactTransitionStatus50C::unsupportedTransition,0x225d9cu,0x8cu,0x226750u};
   return commitOriginalDestroyed(actualRacerSlot,forced,preparedOuterFactor);
  }
  // ":left"/":right" stay pending until this frame's 225C50 point below.
  if(rr::game::nativeTestDestroyLimp50PD[actualRacerSlot])s.fields.flagB9D0=false;
 }
 if(audioEnabled&&!sourceAiAudio&&!opponentFrame&&!raceSetupFrame)require50DU(advanceSpeechTimers50HG(.016f,lapContinuity.prefix,speechState),
     "native speech countdown failed");
 // The live path does not call fixture/oracle callbacks; retain a bounded
 // diagnostic frame label for any future cold-fixture diagnostics.
 x.frame=static_cast<unsigned>(std::min<std::uint64_t>(completedFrames,
     std::numeric_limits<unsigned>::max()));
 FrameReader49K read{};read.preparedFactor=preparedOuterFactor;read.codes=x.actionCodes;
 read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
 if(s.fields.vehicle58==1u){
  // A retained physical-reset continuation already owns this PostPlayer
  // frame. Never read/replay its navigation/packing/repair/scalar prefix.
  if(!originalAi.physicalReset2255B0Pending)
   runNative([&]{return readAiVehicleFrame50IV(s,x.bindings,read.preparedFactor,x.workspace,x.diagnostic);},"original AI frame entry");
  const auto driveSelectedOriginalAi50MB=[&]<class Input>()
      requires (std::is_same_v<Input,AiRuntimeInput50IV>||std::is_same_v<Input,AiRuntimeInput50MB>||std::is_same_v<Input,AiRuntimeInput50TC>||std::is_same_v<Input,AiRuntimeInput50TB>||std::is_same_v<Input,AiRuntimeInput50GB>||std::is_same_v<Input,AiRuntimeInput50MC>){
  Input aiInput{};aiInput.applicationGlobals=&native_ai50IV::applicationNoise().ai;
  if constexpr(std::is_same_v<Input,AiRuntimeInput50MC>){
   requireMbRuntimeNavigation50MB();
   aiInput.mapFF0=mcMapView;aiInput.aiFF4=mcAiView;
   aiInput.orientationTrackBC=mcAiView;aiInput.aiMetadata=mcAiMetadata;aiInput.actualWorld=&liveWorld;
  }else if constexpr(std::is_same_v<Input,AiRuntimeInput50GB>){
   requireMbRuntimeNavigation50MB();
   aiInput.mapFF0=gbMapView;aiInput.aiFF4=gbAiView;
   aiInput.orientationTrackBC=gbAiView;aiInput.aiMetadata=gbAiMetadata;aiInput.actualWorld=&liveWorld;
  }else if constexpr(std::is_same_v<Input,AiRuntimeInput50TB>){
   requireMbRuntimeNavigation50MB();
   aiInput.mapFF0=tbMapView;aiInput.aiFF4=tbAiView;
   aiInput.orientationTrackBC=tbAiView;aiInput.aiMetadata=tbAiMetadata;aiInput.actualWorld=&liveWorld;
  }else if constexpr(std::is_same_v<Input,AiRuntimeInput50TC>){
   requireMbRuntimeNavigation50MB();
   aiInput.mapFF0=tcMapView;aiInput.aiFF4=tcAiView;
   aiInput.orientationTrackBC=tcAiView;aiInput.aiMetadata=tcAiMetadata;aiInput.actualWorld=&liveWorld;
  }else if constexpr(std::is_same_v<Input,AiRuntimeInput50MB>){
   aiInput.mapFF0=mbMapView;aiInput.aiFF4=mbAiView;
   aiInput.orientationTrackBC=mbAiView;aiInput.aiMetadata=mbAiMetadata;
  }else{
   aiInput.mapFF0=navigationMap;aiInput.aiFF4=navigationAI;
   aiInput.orientationTrackBC=navigationAI;aiInput.aiMetadata=&selectedOriginalAiMetadata;
  }
  aiInput.tuning=&originalAiTuning;
  aiInput.racers=originalAiSession.observed;aiInput.racerCount=raceRacerCount;
  aiInput.combatDiagnostic=[](const char* message,void*) noexcept {std::fputs(message,stderr);};
  aiInput.body340Context=&dynamicRegistry;
  aiInput.resolveBody340=[](const void* context,std::uint32_t slot) noexcept {
   return static_cast<const native_dynamic_contact50IW::Registry*>(context)->bodyIdentityForManagerIndex(slot);
  };
  aiInput.raceFields=&originalAiSession.fields[originalAi.managerIndexFE0];aiInput.timing=&lapContinuity.timing;
  aiInput.actualZoneOrdinal48=mainMembership.storage.links[0].zone?mainMembership.storage.links[0].zone-1u:0u;
  aiInput.managerHumanCount34=originalAiHumanCount;
  aiInput.raceCountdown94=humanCourseEnabled?&humanResultsCountdown:nullptr;
  // Consumers here read1125C low byte; the native roster owns the complete
  // packed count word, with the actual admitted low5 lap selection retained.
  aiInput.settings1125C=static_cast<std::uint8_t>(originalAiSettings1125C);
  if constexpr(std::is_same_v<Input,AiRuntimeInput50MC>)aiInput.aiTrackLength3C=mcAiMetadata.trackLength3C();
  else if constexpr(std::is_same_v<Input,AiRuntimeInput50GB>)aiInput.aiTrackLength3C=gbAiMetadata.trackLength3C();
  else if constexpr(std::is_same_v<Input,AiRuntimeInput50TB>)aiInput.aiTrackLength3C=tbAiMetadata.trackLength3C();
  else if constexpr(std::is_same_v<Input,AiRuntimeInput50TC>)aiInput.aiTrackLength3C=tcAiMetadata.trackLength3C();
  else if constexpr(std::is_same_v<Input,AiRuntimeInput50MB>)aiInput.aiTrackLength3C=mbAiMetadata.trackLength3C();
  else aiInput.aiTrackLength3C=selectedOriginalAiMetadata.trackLength3C;
  aiInput.raceOwnersBound=originalAiSession.rankBound&&mainMembership.storage.links[0].zone!=0;
  return driveOriginalAi50IV(s,x.bindings,x.parameters,navigationProgress,navigationCA0,
      lapContinuity.state.vehicleCC8,boostState,originalAi,aiInput,originalAiReceipt);
  };
  bool aiReturned=isMcTrack?driveSelectedOriginalAi50MB.template operator()<AiRuntimeInput50MC>():isGbTrack?driveSelectedOriginalAi50MB.template operator()<AiRuntimeInput50GB>():isTbTrack?driveSelectedOriginalAi50MB.template operator()<AiRuntimeInput50TB>():
      isTcTrack?driveSelectedOriginalAi50MB.template operator()<AiRuntimeInput50TC>():
      isMbTrack?driveSelectedOriginalAi50MB.template operator()<AiRuntimeInput50MB>():
      driveSelectedOriginalAi50MB.template operator()<AiRuntimeInput50IV>();
  if(!aiReturned&&originalAi.physicalReset2255B0Pending&&originalRecoveryEnabled){
   std::uint8_t* flags22C=nullptr;std::uint32_t* field24C=nullptr;
   if(originalResetGround&&racerCables[actualRacerSlot]&&sourceAiAudio&&
      originalAiAudio->physicalResetOwners(actualRacerSlot,originalAi.racer60,s,flags22C,field24C)&&
      refreshAnimatedCandidates()){
    const auto drained=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,trackCounts.events);
    if(drained.status==ZoneQueueStatus50CQ::Complete&&!drained.remaining){
     const auto queries=recoveryQueries();
     auto actualHeightDomain=selectedRuntimeHeightDomain;
     if(isMcTrack)require50DU(mcWaterOwner&&mcWaterOwner->bindResetDomain(selectedRuntimeHeightDomain,
         terrainOwner.lift1F0,actualHeightDomain),"MC AI reset requires actual same-runtime water334 and live348 lift");
     if(nativeGcWater)require50DU(nativeWaterRacers[actualRacerSlot]&&
         nativeWaterRacers[actualRacerSlot]->bindResetDomain(selectedRuntimeHeightDomain,
             terrainOwner.lift1F0,actualHeightDomain),"actual native water reset domain unavailable");
     if(isMcTrack)require50DU(mcParticleVehicles->racer(actualRacerSlot)&&
         mcParticleVehicles->racer(actualRacerSlot)->bindResetDomain(selectedRuntimeHeightDomain,
             terrainOwner.lift1F0,actualHeightDomain),"actual native water reset domain unavailable");
     // Both native types represent the same four scalar target-relative
     // components. Copy explicitly; unrelated struct references cannot alias.
     const auto& relative=x.secondaryInputs.targetRelative2F0;
     const VisualVector49R recoveryTargetRelative{{relative.v[0],relative.v[1],relative.v[2],relative.v[3]}};
     using RecoveryOwners50MB=std::variant<native_original_ai_recovery::Owners,native_original_ai_recovery::Owners50MB,native_original_ai_recovery::Owners50TC,native_original_ai_recovery::Owners50TB,native_original_ai_recovery::Owners50GB,native_original_ai_recovery::Owners50MC>;
     RecoveryOwners50MB owners=isMcTrack?
         RecoveryOwners50MB{native_original_ai_recovery::Owners50MC{gcLightOwner,racerCables[actualRacerSlot]->visibility50(),
             *flags22C,*field24C,mcAiView,actualHeightDomain,resetGlobals,resetLifecycle,
             recoveryTargetRelative,queries,zoneLifecycle,liveWorld,membershipWork,*originalResetGround,
             originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:isGbTrack?
         RecoveryOwners50MB{native_original_ai_recovery::Owners50GB{gcLightOwner,racerCables[actualRacerSlot]->visibility50(),
             *flags22C,*field24C,gbAiView,actualHeightDomain,resetGlobals,resetLifecycle,
             recoveryTargetRelative,queries,zoneLifecycle,liveWorld,membershipWork,*originalResetGround,
             originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:isTbTrack?
         RecoveryOwners50MB{native_original_ai_recovery::Owners50TB{gcLightOwner,racerCables[actualRacerSlot]->visibility50(),
             *flags22C,*field24C,tbAiView,actualHeightDomain,resetGlobals,resetLifecycle,
             recoveryTargetRelative,queries,zoneLifecycle,liveWorld,membershipWork,*originalResetGround,
             originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:
         isTcTrack?
         RecoveryOwners50MB{native_original_ai_recovery::Owners50TC{gcLightOwner,racerCables[actualRacerSlot]->visibility50(),
             *flags22C,*field24C,tcAiView,actualHeightDomain,resetGlobals,resetLifecycle,
             recoveryTargetRelative,queries,zoneLifecycle,liveWorld,membershipWork,*originalResetGround,
             originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:
         isMbTrack?
         RecoveryOwners50MB{native_original_ai_recovery::Owners50MB{gcLightOwner,racerCables[actualRacerSlot]->visibility50(),
             *flags22C,*field24C,mbAiView,actualHeightDomain,resetGlobals,resetLifecycle,
             recoveryTargetRelative,queries,zoneLifecycle,liveWorld,membershipWork,*originalResetGround,
             originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:
         RecoveryOwners50MB{native_original_ai_recovery::Owners{gcLightOwner,racerCables[actualRacerSlot]->visibility50(),
             *flags22C,*field24C,*navigationAI,actualHeightDomain,resetGlobals,resetLifecycle,
             recoveryTargetRelative,queries,zoneLifecycle,liveWorld,membershipWork,*originalResetGround,
             originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}};
     aiReturned=std::visit([&](auto& actual){return native_original_ai_recovery::resume(s,x.bindings,x.parameters,navigationProgress,
         originalAi,originalAiReceipt,originalRecovery,actual,*originalRecoveryWorkspace,originalRecoveryDiagnostic);},owners);
    }else {originalRecoveryDiagnostic.sourcePC=0x124cb0u;
     originalRecoveryDiagnostic.message="actual world registration queue could not publish before reset ground query";}
   }else {originalRecoveryDiagnostic.sourcePC=0x216a80u;
    originalRecoveryDiagnostic.message="actual AI D40/D44/cables or world publication is unavailable";}
   if(!aiReturned){originalAiReceipt.sourceBoundary=originalRecoveryDiagnostic.sourcePC;
    std::cerr<<"ORIGINAL AI physical reset: "<<originalRecoveryDiagnostic.message
        <<" stage="<<static_cast<unsigned>(originalRecovery.stage)<<'\n';}
  }
  if(!aiReturned){
   std::cerr<<"ORIGINAL AI source boundary: slot="<<originalAi.managerIndexFE0
       <<" state="<<originalAiReceipt.stateFE8<<" pc="<<std::hex<<originalAiReceipt.sourceBoundary<<std::dec
       <<" ai_dispatches="<<originalAi.dispatchedFrames<<" brakes="<<originalAi.brakingFrames
       <<" terrain330="<<static_cast<unsigned>(selectedRuntimeHeightDomain.terrain330)
       <<" water334="<<static_cast<unsigned>(selectedRuntimeHeightDomain.water334)
       <<" height_source_bound="<<actualHeight.sourceBound
       <<" water_native_query="<<actualHeight.waterNativeQueryBound<<'\n';
   recoveryPaused=true;return false;
  }
  lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
 }else{
 if(humanCourseEnabled&&!raceSetupFrame){
  std::uint32_t timerPC=0;
  const bool timers=advanceOriginalHumanResetTimers50LC(playerEvents,s.fields.vehicle3C,
      s.fields.applicationByte58,preparedOuterFactor,sourceBlur,timerPC);
  if(!timers)std::cerr<<"original human reset timer source boundary pc="<<std::hex<<timerPC<<std::dec<<'\n';
  require50DU(timers,"original human reset eligibility/blur producer unavailable");
 }
 const DeviceQueryContext48Y queryContext{eventContext.present,applicationEvents.rootFlags228,
     playerEnvironment.held.context518,eventContext.mask1898};
 runNative([&]{return controller.prepareFrameReader(queryContext,read.context,
     read.codes,read.preparedFactor,read);},"native held provider");
 runNative([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"persistent reader");
 //1A15E4/1A160C follows1A1A20 and precedes1A25D0/228620. Consume the
 // prior outer frame's CA0 before228724 copies this frame's local body pose.
 // This owns progression words only; the existing reset cut remains in place.
 require50DU(s.phase==VehicleFramePhase49W::PostPlayer&&!s.fields.vehicle58,
     "source player navigation phase unavailable");
 if((s.fields.applicationByte58&3u)==2u||(s.fields.impactFlagsCD8&1u)){
  TrackNavigationInput50EU navigationInput{};
  std::memcpy(navigationInput.previousCA0,navigationCA0,sizeof navigationCA0);
  std::memcpy(navigationInput.currentLocal190,
      s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].localC0.v+12,12);
  navigationInput.currentLocal190[3]=1.f;
  navigationInput.mainBody104=m.components.probe348.field104;
  TrackNavigationReceipt50EU receipt{};
  const TrackNavigationMapEffects50GH lapEffects{&lapOwner,&lap_drive50GJ::Owner::accepted};
  const bool advanced=isMcTrack?
      advancePlayerTrackNavigationEffects50GH(mcMapView,mcAiView,navigationInput,lapEffects,navigationProgress,receipt,navigationDiagnostic):isGbTrack?
      advancePlayerTrackNavigationEffects50GH(gbMapView,gbAiView,navigationInput,lapEffects,navigationProgress,receipt,navigationDiagnostic):isTbTrack?
      advancePlayerTrackNavigationEffects50GH(tbMapView,tbAiView,navigationInput,lapEffects,navigationProgress,receipt,navigationDiagnostic):
      isTcTrack?
      advancePlayerTrackNavigationEffects50GH(tcMapView,tcAiView,navigationInput,lapEffects,navigationProgress,receipt,navigationDiagnostic):
      isMbTrack?
      advancePlayerTrackNavigationEffects50GH(mbMapView,mbAiView,navigationInput,lapEffects,navigationProgress,receipt,navigationDiagnostic):
      advancePlayerTrackNavigationEffects50GH(*navigationMap,*navigationAI,
          navigationInput,lapEffects,navigationProgress,receipt,navigationDiagnostic);
  addCounter50DU(nativeCalls);
  require50DU(reportNavigation(advanced,"live1A0DA0"),"source player navigation rejected");
  addCounter50DU(navigationUpdates);
  addCounter50DU(navigationAISequential,receipt.aiSequential);
  addCounter50DU(navigationMapSequential,receipt.mapSequential);
  addCounter50DU(navigationAIScan,receipt.aiScan);
  addCounter50DU(navigationMapScan,receipt.mapScan);
 }else addCounter50DU(navigationGateSkips);
 //1A15EC packing-reference projection is joined at the serial arena boundary
 // immediately after this player update, before any AI update.
 // Original1A1614: once the human manager gate ends, a finished player runs
 //1A36A0 after navigation and before wrong-way/base processing. Opponents still
 // use the explicitly host-authored course adapter, not this player leaf.
 if(!opponentFrame&&!raceSetupFrame&&humanResultsActive&&(s.fields.impactFlagsCD8&1u)){
  require50DU((s.fields.applicationByte58&3u)!=2u&&(s.fields.impactFlagsCD8&1u),
      "native finish driver requires ended human gate and source CD8");
  const auto current=navigationProgress.reset.vehicleC88;
  require50DU(current<selectedMapCount50MB(),"finished player map target unavailable");
  const auto& scene=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)];
  PlayerFinishInput50IA finishInput{};
  finishInput.resultsCountdown94=humanResultsCountdown;
  finishInput.authoredSteering10A4=authoredFinishSteering10A4;
  finishInput.bodyCachedSpeed34=m.cachedSpeed34;
  finishInput.bodyWorldAngularA4=m.components.root.worldAngularVelocityA0.v[1];
  finishInput.engineDifference5C=authored.engineDifference5C;
  for(unsigned axis=0;axis<3;++axis){
   finishInput.bodySceneWorldPosition[axis]=scene.world100.v[12+axis];
   finishInput.mapCurrentNodePosition[axis]=selectedMapNode50MB(current).position[axis];
   finishInput.mainSceneLocalForward[axis]=scene.localC0.v[8+axis];
  }
  finishInput.mainSceneLocalUpY=scene.localC0.v[5];
  finishInput.previousFlagsB9C=m.controls.player.flagsB9C;
  PlayerFinishResult50IA finishResult{};
  runNative([&](){return driveFinishedPlayer50IA(finishInput,finishResult);},
      "original finished-player controls rejected");
  m.controls.player=finishResult.player;
  // This source model write precedes the first frame use of parameters in
  //217DE0; keep the resulting descriptor unchanged for the rest of this frame.
  x.parameters.bodies.main.aero.steering100=finishResult.aeroSteering100;
  finishDriverAppliedThisFrame=true;
 }
 //1A1620: after the entire navigation branch, before undertrack processing.
 // Navigation above consumed the previous update's actual flags10CC bit1.
 WrongWayProgressInput50GO wrongWayInput{};
 wrongWayInput.application34C=applicationWrongWayBypass34C;
 wrongWayInput.vehicle40=m.vehicleState40;
 wrongWayInput.vehicleC8C=navigationProgress.reset.vehicleC8C;
 wrongWayInput.vehicleC88=navigationProgress.reset.vehicleC88;
 wrongWayInput.config58=s.fields.applicationByte58;
 wrongWayInput.vehicleCD8=s.fields.impactFlagsCD8;
 std::memcpy(wrongWayInput.localForwardE0,
     s.nodes[static_cast<unsigned>(x.bindings.animation.bodyScene288)].localC0.v+8,12);
 // The original native HUD owner is genuinely absent; the separate HOST overlay
 // below observes state and is not substituted as a completed original UI call.
 wrongWayInput.warningOwner=nullptr;
 WrongWayProgressState50GO wrongWayState{lapContinuity.state.vehicleFF8,
     lapContinuity.prefix.flags10CC};
 require50DU(isMcTrack?
     beginWrongWayProgress50GO(mcMapView,lapOwner.mcTrack(),wrongWayInput,
         wrongWayState,wrongWayContinuation,wrongWayReceipt,wrongWayDiagnostic):isGbTrack?
     beginWrongWayProgress50GO(gbMapView,lapOwner.gbTrack(),wrongWayInput,
         wrongWayState,wrongWayContinuation,wrongWayReceipt,wrongWayDiagnostic):isTbTrack?
     beginWrongWayProgress50GO(tbMapView,lapOwner.tbTrack(),wrongWayInput,
         wrongWayState,wrongWayContinuation,wrongWayReceipt,wrongWayDiagnostic):
     isTcTrack?
     beginWrongWayProgress50GO(tcMapView,lapOwner.tcTrack(),wrongWayInput,
         wrongWayState,wrongWayContinuation,wrongWayReceipt,wrongWayDiagnostic):
     isMbTrack?
     beginWrongWayProgress50GO(mbMapView,lapOwner.mbTrack(),wrongWayInput,
         wrongWayState,wrongWayContinuation,wrongWayReceipt,wrongWayDiagnostic):
     beginWrongWayProgress50GO(navigationMap->track,lapOwner.track,wrongWayInput,
         wrongWayState,wrongWayContinuation,wrongWayReceipt,wrongWayDiagnostic),
     "native wrong-way progression rejected");
 require50DU(wrongWayContinuation.pending.operation==WrongWayHudOperation50GO::None,
     "unowned original warning callback pending");
 lapContinuity.state.vehicleFF8=wrongWayState.vehicleFF8;
 lapContinuity.prefix.flags10CC=wrongWayState.flags10CC;
 addCounter50DU(wrongWayUpdates);
 if(wrongWayReceipt.wrongWay)addCounter50DU(wrongWayActiveTicks);
 }
 //228724..228730:28B180 copies only local XYZ from the actual340->288
 // binding, without world-cache refresh, immediately before217DE0 here.
 // The supplied preserving cut covers intervening unrelated source effects.
 require50DU(!m.applicationActive11C&&!s.fields.vehicle3C&&m.domain.sceneAttached,
     "source CA0 capture owner unavailable");
 std::memcpy(navigationCA0,
     s.nodes[static_cast<unsigned>(x.bindings.animation.bodyScene288)].localC0.v+12,12);
 addCounter50DU(navigationCA0Copies);
 PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=x.configurationEntryNonzero;
 if(s.fields.vehicle58==1u){
  runNative([&]{return bindAiVehicleFrame50IV(s,x.bindings,x.parameters,x.workspace,x.diagnostic);},"original AI native binding");
  if(m.components.bindings.aeroBrakeEC==1.f&&m.controls.player.raw.values[0]!=0.f)
   ++originalAi.boundBrakeSteerFrames;
 }
 else runNative([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"persistent binding");
 const BoostInput50FK boostInput{m.frameFactor,m.components.engine.smoothedDrive[0],
     m.components.engine.smoothedDrive[1],m.cachedSpeed34,m.controls.player.flagsB9C,
     s.fields.vehicle58,dynamicContactEnabled?dynamicProfile.game34:2u,0,sourceAiAudio};
 // Fresh186EF0 index5 explicitly clears10FC0. ALL8 source audio binds its
 // actual D40 owner; other routes retain the existing native boost subscriber.
 if(audioEnabled&&!raceSetupFrame&&!sourceAiAudio){
  const bool heldBoost=(m.controls.player.flagsB9C&1u)&&!(m.controls.player.flagsB9C&2u);
  const float heating=boostParameters.heatRate88*(boostInput.engine70+boostInput.engine74);
  if(opponentFrame)require50DU(opponent_audio50HX::boostAtBoundary(opponentBoostAudio,
      heldBoost,heating,boostState.vehicle64),"source opponent boost audio event rejected");
  else require50DU(liveAudio->boostAtBoundary(heldBoost,heating,boostState.vehicle64).okay(),
      "source boost audio event rejected");
 }
 runNative([&]{
     const BoostState50FK boostBefore=boostState;
     const bool boostAccepted=sourceAiAudio?
     originalAiAudio->thermal(actualRacerSlot,s,x.bindings,boostInput,boostParameters,boostState,boostDiagnostic):
     s.fields.vehicle58==1u?
     advanceAiThermal50IT(boostInput,boostParameters,boostState,boostState,boostDiagnostic):
     advancePlayerBoost50FK(boostInput,boostParameters,boostState,boostState,boostDiagnostic);
     // The shared vehicle diagnostic does not carry the boost leaf's own
     // reason; record it, its inputs and the FP rounding mode before failing.
     if(!boostAccepted)std::cerr<<"boost_failure slot="<<actualRacerSlot<<" error="<<static_cast<unsigned>(boostDiagnostic.error)
         <<" stage="<<std::hex<<boostDiagnostic.sourceStage<<std::dec<<" rounding="<<std::fegetround()
         <<" source_ai_audio="<<sourceAiAudio<<" vehicle58="<<s.fields.vehicle58
         <<" frame_factor="<<boostInput.frameFactor<<" engine70="<<boostInput.engine70<<" engine74="<<boostInput.engine74
         <<" speed34="<<boostInput.cachedSpeed34<<" flagsB9C="<<static_cast<unsigned>(boostInput.flagsB9C)
         <<" profile34="<<boostInput.profile34<<" v64="<<boostBefore.vehicle64<<" v68="<<boostBefore.vehicle68
         <<" duration6C="<<boostBefore.duration6C<<" ratio70="<<boostBefore.speedRatio70<<" heat7C="<<boostBefore.heat7C
         <<" heat88="<<boostParameters.heatRate88<<" cool80="<<boostParameters.baseCooling80
         <<" cool84="<<boostParameters.speedCooling84<<" diff5C="<<boostParameters.engineDifference5C<<'\n';
     return boostAccepted;},"native boost controller");
 x.selectors.vehicle64=boostState.vehicle64;
 runNative([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"persistent graphics");
 if(nativeSelectedLights){
  auto& sequence=gcGraphicsSequence[actualRacerSlot];
  require50DU(sequence!=std::numeric_limits<std::uint64_t>::max(),"GC graphics publication overflow");
  ++sequence;
  //228844..228884 consumes EXISTING694 immediately after graphics. Neither
  // later physics nor the current draw model substitutes for that cache.
  require50DU(gcLightOwner.updateAfterGraphics(static_cast<std::uint32_t>(actualRacerSlot),
      s,x.bindings,x.parameters,sequence,gcLightDiagnostic),
      "actual GC visibility update failed; requested selected-light frame cannot publish");
  if(originalRecoveryEnabled){
   require50DU(racerCables[actualRacerSlot]&&gcLightOwner.writeCablesVisibilityAfterGraphics(
       static_cast<std::uint32_t>(actualRacerSlot),s,x.bindings,x.parameters,sequence,
       racerCables[actualRacerSlot]->visibility50(),gcLightDiagnostic),
       "actual PodCables254 cached visibility write rejected");
  }
 }
 ImpactConsumerInput49Z consumer{};consumer.vehicle64=boostState.vehicle64;consumer.managerFlags58=s.fields.applicationByte58;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 if(dynamicContactEnabled){
  consumer.profile34=dynamicProfile.game34;consumer.profile10FAC=0;
  // Consume the same committed profile vibration bit as Game Options.
  consumer.profileFlags11050=dynamicConsumerFlags11050;
  //222D34 skips feedback for every nonzero vehicle58, including all8 AI.
  //Keep the real human-only boundary intact; camera slot0 is no human owner.
  consumer.stopBeforeControllerFeedback50IW=true;
  consumer.player5CBound50IW=!allAiEnabled&&actualRacerSlot==0u;
  if(consumer.player5CBound50IW)consumer.player5C=playerSource5C;
  consumer.finalDamageThreshold64=dynamicFinalTuning.threshold64;
  consumer.finalDamageIncrement68=dynamicFinalTuning.increment68;
  consumer.dynamicPartnerEventsBound=true;consumer.partnerEventThresholdBound=dynamicTuning.bound;
  consumer.partnerEventThreshold48=dynamicTuning.points3C[3];
 }
 bool consumersCompleted=consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);
 if(dynamicContactEnabled&&(consumersCompleted||
     (x.diagnostic.error==VehicleFrameError49W::ExternalCut&&
      (x.diagnostic.sourceStage==0x222d70u||x.diagnostic.sourceStage==0x222d8cu)))){
  auto appendConsumerPartners=[&](){
   refreshDynamicSelfActor();
   for(std::size_t index=0;index<consumerEvents.partners.count;++index){
    const auto& event=consumerEvents.partners.values[index];
    if(event.partnerId>=raceRacerCount)dynamicBoundary(event.sourcePC,"original partner actor index unavailable");
    const auto appended=appendDynamicContactEvent1_50IW(dynamicEvents,dynamicActorResolver,
        static_cast<RaceActorId50ID>(dynamicSelfSlot+1),event.partnerId+1,event.word8,
        dynamicEventBindings.eventOwner33CBound);
    if(!appended.helperReturned())dynamicBoundary(event.sourcePC,"original shared contact event enqueue pending");
   }
  };
  //222CFC event0 precedes both controller helpers. Commit/enqueue that exact
  // prefix before resolving actual source5C; never run222B20 again.
  appendConsumerPartners();
  if(!consumersCompleted&&s.phase==VehicleFramePhase49W::ImpactFeedbackPending50KZ){
   if(!consumer.player5CBound50IW)dynamicBoundary(0x222d70u,"actual human source5C not bound");
   consumersCompleted=resumeVehicleImpactsWithControllers50KZ(s,x.bindings,
       feedbackDevices,2u,consumerEvents,x.workspace,x.diagnostic);
   if(!consumersCompleted)dynamicBoundary(x.diagnostic.sourceStage,"original controller helper continuation pending");
   feedbackHelpers+=consumerEvents.count;
   // Only2228D0's later events are now returned. The prefix event0 is not
   // duplicated, and actual helper timers survive any later gameplay failure.
   appendConsumerPartners();
  }
 }
 runNative([&]{return consumersCompleted;},"persistent impact consumers");
 if(s.phase==VehicleFramePhase49W::PostImpactConsumers){
 runNative([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"persistent repair");
 //1A2404/1A2300 are human callback overrides. The followed original AI
 //receives its own base callbacks; host slot0 must not invent human event5.
 if(audioEnabled&&!sourceAiAudio&&!opponentFrame&&!raceSetupFrame){
  const float damage[]={s.fields.damage7D0,s.fields.damage7D4,s.fields.damage7D8,s.fields.damage7DC};
  require50DU(liveAudio->repairCompletion(m.controls.player.flagsB9C,s.fields.vehicle3C,damage).okay(),
      "source repair-completion audio rejected");
  require50DU(afterRepairSpeechState50HG(s.fields.damage7D0,s.fields.damage7D4,
      s.fields.damage7DC,lapContinuity.prefix),"native repair speech state rejected");
  if(dynamicContactEnabled)refreshDynamicSelfActor();
  const auto produced=dynamicContactEnabled?
      produceQueuedDamageSpeech50ID(speechInput(),lapContinuity.prefix,1u,dynamicActorResolver,dynamicEvents):
      (mode1Enabled?mode1->produceDamage(speechInput()):
      produceDamageSpeechEvent50HG(speechInput(),lapContinuity.prefix,pendingSpeech));
  require50DU(produced.status==PodSpeechStatus50HG::Complete,"native damage speech enqueue rejected");
  if(produced.eventProduced)++speechEvents;
 }
 ImpactEffectsInput50B owners{};
 owners.models[0].node=x.bindings.visual.left1BC;owners.models[1].node=x.bindings.visual.right1B8;
 owners.models[2].node=x.bindings.visual.bottomLeft1C4;owners.models[3].node=x.bindings.visual.bottomRight1C0;
 std::size_t count=0;owners.damage00Matches=namesForEffects.damage00;owners.nameCount=s.sceneNodeCount;
 for(unsigned engine=0;engine<4;++engine){
  auto&model=owners.models[engine];if(model.node==SceneNodeIndex49P::none)continue;
  require50DU(rr::assets::effectModelRoot50BL(namesForEffects,model.node),"bound model name scope");
  // Preserve the existing fresh, unhidden model admission. 22355C consumes
  // only disabled bit2; fresh289410 clears it. This is not sceneFlags190/191.
  model.flags03=3;
  const auto smoke=effectOwnerIndex[0][engine],sparks=effectOwnerIndex[1][engine],fire=effectOwnerIndex[2][engine];
  require50DU(smoke<nativeEffectCount&&sparks<nativeEffectCount&&fire<nativeEffectCount,
      "effect owner bank does not match the current racer");
  model.effect800={nativeEffects[smoke].id,nativeEffects[smoke].state.flags1D4};
  model.effect804={nativeEffects[sparks].id,nativeEffects[sparks].state.flags1D4};
  model.effect80C={nativeEffects[fire].id,nativeEffects[fire].state.flags1D4};
 }
 const bool originalMaterialDispatch=originalAiAudioEnabled&&!raceSetupFrame;
 const auto materialRacer60=sourceAiAudio?originalAi.racer60:playerRacer60;
 if(originalMaterialDispatch)require50DU((sourceAiAudio?
     originalAi.managerIndexFE0==actualRacerSlot:
     humanCourseEnabled&&actualRacerSlot==0u&&!opponentFrame)&&
     originalAiAudio->materialEventTarget(actualRacerSlot,materialRacer60,s,owners.effectD40),
     "actual current human/AI D40 material event owner unavailable");
 runNative([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,effectsWorkspace,generatedEffects,maxImpactEffectEvents50B,count,x.workspace,x.diagnostic);},"actual effect control bindings");
 //224130/224148 is the source-final event, after all particle commands and
 // their shared RNG. Consume the concrete particles first, then this same
 // racer's real D40/D44; never send the channel event to a particle surrogate.
 const auto particleCount=originalMaterialDispatch?(count?count-1u:0u):count;
 if(originalMaterialDispatch)require50DU(count&&generatedEffects[count-1].kind==ImpactEffectKind50B::ChannelA8&&
     generatedEffects[count-1].target==owners.effectD40,"actual D40 channel tail missing");
 runNative([&]{return consumeEffects(generatedEffects,particleCount);},"consume impact effect controls");
 if(originalMaterialDispatch){
  const auto& event=generatedEffects[count-1];std::int32_t contactA9=-1;
  // The source stop channel3 never reads current contact. Preserve that lazy
  // branch rather than inventing a contact dependency during its stop-all.
  if(event.argument==2u)require50DU(resolveContactMaterial50HI(spring.records.currentCandidate130,
      audioContactMaterials,terrainOwnerCount,contactA9),"actual material contact A9 unresolved");
  if(!sourceAiAudio){
   require50DU(dynamicContactEnabled&&dynamicSelfSlot==actualRacerSlot,
       "actual human material event FIFO/actor owner unavailable");
   refreshDynamicSelfActor();
  }
  const auto receipt=originalAiAudio->materialChannel(actualRacerSlot,materialRacer60,
      s,x.bindings,event,contactA9,boostState.speedRatio70,s.noiseOwner(),
      dynamicEvents,dynamicActorResolver,dynamicEventBindings.eventOwner33CBound);
  if(receipt.status!=OriginalPodMaterialAudioStatus::complete){
   std::cerr<<"Original material source boundary slot="<<actualRacerSlot<<" pc="<<std::hex
       <<receipt.sourcePC<<std::dec<<" status="<<static_cast<unsigned>(receipt.status)
       <<"; committed fields and shared RNG retained.\n";
   if(humanCourseEnabled&&actualRacerSlot==0u){
    // Read existing observations only. A zero/nonzero reset serial does not
    // establish which host key was delivered or caused this failure.
    std::cerr<<"original_material_human_context manual_reset_serial="<<originalManualResetSerial
        <<" manual_reset_graphics_due="<<originalManualResetGraphicsDue
        <<" recovery_stage="<<static_cast<unsigned>(originalHumanRecovery.stage)
        <<" manual_reset_pc="<<std::hex<<originalManualRecoveryDiagnostic.sourcePC
        <<" undertrack_reset_pc="<<originalHumanRecoveryDiagnostic.sourcePC<<std::dec
        <<" undertrack_pending="<<humanBaseUndertrack.pending
        <<" controller_phase="<<static_cast<unsigned>(controllerActions.phase())
        <<" player_source5c="<<playerEvents.source5C<<" player_action_index="<<playerEvents.indexFFC
        <<" player_field40="<<playerEvents.field40<<" player_countcd0="<<playerEvents.countCD0
        <<" player_timer1098_bits="<<std::hex<<original_material_input_diagnostics50LN::bits(playerEvents.value1098)
        <<" player_timer1094_bits="<<original_material_input_diagnostics50LN::bits(playerEvents.value1094)
        <<" player_timer1090_bits="<<original_material_input_diagnostics50LN::bits(playerEvents.value1090)<<std::dec
        <<" input_flags29="<<static_cast<unsigned>(playerEnvironment.flags29)
        <<" input_gate58="<<static_cast<unsigned>(playerEnvironment.gate58)
        <<" held_root_flags="<<static_cast<unsigned>(playerEnvironment.held.rootFlags228)
        <<" frame_phase="<<static_cast<unsigned>(s.phase)<<" vehicle3c="<<s.fields.vehicle3C
        <<" vehicle58="<<s.fields.vehicle58<<" body_state40="<<m.vehicleState40<<'\n';
    std::cerr.flush();
   }
   require50DU(false,"actual human/AI material dispatch failed");
  }
  addCounter50DU(effectCommands,1);
 }
 if(audioEnabled&&!originalMaterialDispatch&&!sourceAiAudio&&!opponentFrame&&!raceSetupFrame){
  MaterialAudioInput50HI materialInput{};
  materialInput.channelArgument=s.impactSupplement.timers820[4]>0.f?2u:3u;
  require50DU(resolveContactMaterial50HI(spring.records.currentCandidate130,
      audioContactMaterials,terrainOwnerCount,materialInput.contactMaterialA9),"actual audio contact identity unresolved");
  materialInput.speedRatio70=boostState.speedRatio70;
  if(audioListenerReady){
   const auto& p=m.components.root.body.position;
   materialInput.listenerDistance=std::hypot(p.v[0]-audioListener.x,p.v[2]-audioListener.z);
  }
  require50DU(materialAudio->update(materialInput,s.noiseOwner().sharedRandomState).okay(),
      "native material audio subscriber rejected");
 }
 OptionalProbeOwners50P optionalOwners{}; // Development topology has no optional dust owner.
 OptionalProbeResult50P optional{};
 runNative([&]{return optionalProbeVehicleImpacts50P(s,x.bindings,optionalOwners,optional,x.workspace,x.diagnostic);},"explicit absent optional dust-owner context");runNative([&]{return consumeEffects(optional.events,optional.count);},"consume dust stop controls");ImpactTransitionResult50C decision{};
 // racer+50 "ExplosiveWreckSpeed" (2159E8..215A28), read at race
 // activation from this racer's own loaded PHY. Used by 225C50
 // (225CCC..225CEC) and the 2289BC limp countdown.
 require50DU(actualRacerSlot<rr::game::nativeExplosiveWreckSpeeds50PD.size()&&
     rr::game::nativeExplosiveWreckSpeeds50PD[actualRacerSlot].loaded,"racer +50 explosive wreck speed not loaded for this racer");
 const std::uint32_t wreckSpeed50Bits=rr::game::nativeExplosiveWreckSpeeds50PD[actualRacerSlot].speed50Bits;
 const bool damageReady=decideVehicleImpacts50P(s,x.bindings,wreckSpeed50Bits,
     decision,x.workspace,x.diagnostic);
 bool limpContinued=false;
 const auto isLimpEntry=[&](const ImpactTransitionResult50C& d){
  return originalAiEnabled&&(d.sourcePC==0x225e7cu||d.sourcePC==0x225ec0u)&&!s.fields.vehicle58&&!s.fields.vehicle3C;};
 if(damageReady&&actualRacerSlot<rr::game::nativeTestDestroyLimp50PD.size()&&
    rr::game::nativeTestDestroyLimp50PD[actualRacerSlot]&&originalAiEnabled&&!s.bodies.main.vehicleState40){
  // Test-only: this frame's 225C50 takes its 225E7C (slot90, 227370, left
  // lost) or 225EC0 (slot94, 227A40, right lost) site, so the real suffix
  // runs. Provenance: the original's own debug input 1A17EC calls 90/94.
  const int side=rr::game::nativeTestDestroyLimp50PD[actualRacerSlot];
  rr::game::nativeTestDestroyLimp50PD[actualRacerSlot]=0;
  const ImpactTransitionResult50C forced=side==1?
      ImpactTransitionResult50C{ImpactTransitionStatus50C::unsupportedTransition,0x225e7cu,0x90u,0x227370u}:
      ImpactTransitionResult50C{ImpactTransitionStatus50C::unsupportedTransition,0x225ec0u,0x94u,0x227a40u};
  require50DU(commitOriginalDestroyed(actualRacerSlot,forced,preparedOuterFactor),"test limp entry rejected");
  limpContinued=true;
 }
 if(!damageReady&&x.diagnostic.error==VehicleFrameError49W::ExternalCut&&
    decision.status==ImpactTransitionStatus50C::unsupportedTransition){
  addCounter50DU(nativeCalls);
  // Original 225C50 dispatches the racer's destroy handler; the racer is
  // retired for the rest of the race (no respawn). Original-AI sessions run
  // that transition; the legacy host checkpoint path below is unchanged.
  if(isLimpEntry(decision)){
   // 227370/227A40 return to 225C50, whose suffix runs; 228620 then goes on
   // with this same frame (2289BC countdown onward). The impact block is
   // completed again at state1/2, where 2288EC no longer runs 225C50.
   require50DU(commitOriginalDestroyed(actualRacerSlot,decision,preparedOuterFactor),"limp entry rejected");
   ImpactTransitionResult50C after{};
   limpContinued=decideVehicleImpacts50P(s,x.bindings,wreckSpeed50Bits,after,x.workspace,x.diagnostic)&&
       after.status==ImpactTransitionStatus50C::noTransition;
   require50DU(limpContinued,"limp entry impact block did not complete at state1/2");
  }else{
  if(originalAiEnabled)return commitOriginalDestroyed(actualRacerSlot,decision,preparedOuterFactor);
  recoveryPaused=true;
  if(opponentFrame)return false; // A failed AI never restarts the human session.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack,"MB/TC/RC/TB/GB/MC native session cannot enter legacy HOST checkpoint prepareDamageRecovery");
  const bool prepared=recovery50FI::prepareDamageRecovery(*navigationMap,navigationProgress,s,
      x.bindings,decision,x.diagnostic,0x3f800000u,x.parameters.visual.verticalLimit7EC,liveWorld,
      nextCheckpoint,recoveryDiagnostic);
  const bool grounded=prepared&&recovery50FI::resolveGround(ground,nextCheckpoint,16,0,
      groundStack.data(),groundStack.size(),groundedCheckpoint,recoveryDiagnostic);
  if(grounded){
   checkpointOutput=nextCheckpoint;checkpointRequested=true;
   std::cerr<<"Damage checkpoint recovery after "<<completedFrames<<" completed ticks: source=0x"
            <<std::hex<<decision.sourcePC<<std::dec<<" anchor="<<nextCheckpoint.selection.anchorNode
            <<" zone="<<nextCheckpoint.primaryZone<<". Fresh vehicle; progress retained.\n";
  }else{
   std::cerr<<"Damage recovery unavailable: error="<<static_cast<unsigned>(recoveryDiagnostic.error)
            <<". Last committed image retained; F5/restart returns to grid.\n";
  }
  return false;
  }
 }
 runNative([&]{return damageReady||limpContinued;},"ordinary damage decision");
 // 2289BC..228A90 (human, 3C==0, states1/2): 4C -= dt; |BA8| > threshold
 // -> 2261B0, > threshold/2 or 4C <= 0 -> 226750, both without 225C50's
 // suffix. The threshold is the same racer value the state-0 decision uses.
 if((s.bodies.main.vehicleState40==1u||s.bodies.main.vehicleState40==2u)&&!s.fields.vehicle58&&!s.fields.vehicle3C){
  require50DU(actualRacerSlot<rr::game::nativeDestroyedRacers50PD.size()&&
      rr::game::nativeDestroyedRacers50PD[actualRacerSlot].limping,"limping racer has no native limp record");
  auto& limp=rr::game::nativeDestroyedRacers50PD[actualRacerSlot];++limp.limpFrames;
  float impact=0.f;std::memcpy(&impact,&s.fields.impactBA0.v[2],4);
  const auto countdown=rr::game::advanceOriginalDetachedDamage50LA(s.bodies.main.vehicleState40,s.fields.vehicle3C,
      preparedOuterFactor,impact,std::bit_cast<float>(wreckSpeed50Bits),limp.limp4C);
  const auto& p=s.bodies.main.components.root.body.position;
  // The cockpit tether: 1B66D0's world anchors from this frame's physics
  // (A on the cockpit at the surviving hitch, B on the main body at the
  // surviving engine) against the 1B6670 length.
  const auto& joint=s.bodies.joint;
  const float tx=joint.worldAnchor80.v[0]-joint.worldAnchor90.v[0],ty=joint.worldAnchor80.v[1]-joint.worldAnchor90.v[1],
      tz=joint.worldAnchor80.v[2]-joint.worldAnchor90.v[2];
  std::cerr<<"pod_limp frame="<<limp.limpFrames<<" state40="<<s.bodies.main.vehicleState40<<" t4C="<<limp.limp4C
      <<" ba8="<<impact<<" speed="<<s.bodies.main.cachedSpeed34<<" pos="<<p.v[0]<<','<<p.v[1]<<','<<p.v[2]
      <<" tether="<<std::sqrt(tx*tx+ty*ty+tz*tz)<<" tether_len="<<joint.length4C
      <<" status="<<static_cast<unsigned>(countdown.status)<<'\n';
  if(countdown.status==rr::game::OriginalVehicleDamageStatus50LA::destructionRequired){
   std::cerr<<"pod_limp_exit reason="<<(countdown.sourcePC==0x228a30u?"impact88":limp.limp4C<=0.f?"timer":"impact8C")
       <<" handler=0x"<<std::hex<<countdown.originalTarget<<std::dec<<" after_frames="<<limp.limpFrames<<'\n';
   const ImpactTransitionResult50C escalation{ImpactTransitionStatus50C::unsupportedTransition,countdown.sourcePC,
       countdown.vtableSlot,countdown.originalTarget};
   return commitOriginalDestroyed(actualRacerSlot,escalation,preparedOuterFactor);
  }
  require50DU(countdown.status==rr::game::OriginalVehicleDamageStatus50LA::complete,"original 2289BC limp countdown rejected");
 }
 }
 require50DU(s.phase==VehicleFramePhase49W::AfterImpactBlock,"source impact outer skip continuation unavailable");
 if(isMcTrack){
  require50DU(mcParticleVehicles&&mcParticleVehicles->racer(actualRacerSlot)&&mcParticleVehicles->racer(actualRacerSlot)->attachedTail(s,x.bindings).okay(),"MC actual22A620 water tail rejected");
 }else if(nativeGcWater){
  require50DU(nativeWaterRacers[actualRacerSlot]&&
      nativeWaterRacers[actualRacerSlot]->attachedTail(s,x.bindings).okay(),
      "actual22A620 native water tail rejected");
 }else{
 PlainEffectOwners50Q plainOwners{};
 const SceneNodeIndex49P plainModels[]={x.bindings.visual.left1BC,x.bindings.visual.right1B8,
     x.bindings.visual.bottomLeft1C4,x.bindings.visual.bottomRight1C0};
 for(unsigned engine=0;engine<4;++engine)if(plainModels[engine]!=SceneNodeIndex49P::none)
  for(unsigned array=0;array<3;++array){
   const auto index=effectOwnerIndex[array][engine];
   require50DU(index<nativeEffectCount,"plain effect owner bank does not match the current racer");
   const auto&owner=nativeEffects[index];plainOwners.slots[array*4+engine]={owner.id,owner.state.flags1D4};
  }
 PlainEffectResult50Q plain{};
 runNative([&]{return plainEffectsVehicleTail50Q(s,x.bindings,plainOwners,plain,x.workspace,x.diagnostic);},"plain effects");runNative([&]{return consumeEffects(plain.events,plain.count);},"consume plain effect controls");
 }
 runNative([&]{return packVehicleFrame50S(s,x.bindings,x.configurationEntryNonzero,x.workspace,x.diagnostic);},"packet");BaseTailBindings50X baseBinding{77,Probe128Binding50X::Null};
 bool baseAdvanced=advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,
     baseBinding,baseTail,baseGlobals,coldFlame,baseWorkspace,x.workspace,x.diagnostic,
     humanCourseEnabled&&actualRacerSlot==0u?&humanBaseUndertrack:nullptr);
 if(!baseAdvanced&&humanCourseEnabled&&actualRacerSlot==0u&&
    x.diagnostic.error==VehicleFrameError49W::ExternalCut&&x.diagnostic.sourceStage==0x228BBC){
  addCounter50DU(nativeCalls);
  bool physicalReturned=false;
  std::uint8_t* flags22C=nullptr;std::uint32_t* field24C=nullptr;
  auto& controllerD40=liveAudio->humanController();
  if(originalRecoveryEnabled&&originalResetGround&&racerCables[0]&&resetHumanCamera&&
     originalAiAudio->humanPhysicalResetOwners(playerRacer60,s,controllerD40,flags22C,field24C)&&
     flags22C==&controllerD40.flags22C()&&field24C&&refreshAnimatedCandidates()){
   const auto drained=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,trackCounts.events);
   if(drained.status==ZoneQueueStatus50CQ::Complete&&!drained.remaining){
    const auto queries=recoveryQueries();const auto& relative=x.secondaryInputs.targetRelative2F0;
    const VisualVector49R target{{relative.v[0],relative.v[1],relative.v[2],relative.v[3]}};
    auto mcActualHeightDomain=selectedRuntimeHeightDomain;
    if(isMcTrack)require50DU(mcWaterOwner&&mcWaterOwner->bindResetDomain(selectedRuntimeHeightDomain,
        terrainOwner.lift1F0,mcActualHeightDomain),"MC human reset requires actual same-runtime water334 and live348 lift");
    using RecoveryOwners50MB=std::variant<native_original_human_recovery::Owners,native_original_human_recovery::Owners50MB,native_original_human_recovery::Owners50TC,native_original_human_recovery::Owners50TB,native_original_human_recovery::Owners50GB,native_original_human_recovery::Owners50MC>;
    RecoveryOwners50MB owners=isMcTrack?
        RecoveryOwners50MB{native_original_human_recovery::Owners50MC{gcLightOwner,racerCables[0]->visibility50(),
        controllerD40,lapContinuity.prefix,*field24C,mcMapView,mcActualHeightDomain,
        resetGlobals,resetLifecycle,target,queries,zoneLifecycle,liveWorld,membershipWork,
        *originalResetGround,originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:isGbTrack?
        RecoveryOwners50MB{native_original_human_recovery::Owners50GB{gcLightOwner,racerCables[0]->visibility50(),
        controllerD40,lapContinuity.prefix,*field24C,gbMapView,selectedRuntimeHeightDomain,
        resetGlobals,resetLifecycle,target,queries,zoneLifecycle,liveWorld,membershipWork,
        *originalResetGround,originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:isTbTrack?
        RecoveryOwners50MB{native_original_human_recovery::Owners50TB{gcLightOwner,racerCables[0]->visibility50(),
        controllerD40,lapContinuity.prefix,*field24C,tbMapView,selectedRuntimeHeightDomain,
        resetGlobals,resetLifecycle,target,queries,zoneLifecycle,liveWorld,membershipWork,
        *originalResetGround,originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:
        isTcTrack?
        RecoveryOwners50MB{native_original_human_recovery::Owners50TC{gcLightOwner,racerCables[0]->visibility50(),
        controllerD40,lapContinuity.prefix,*field24C,tcMapView,selectedRuntimeHeightDomain,
        resetGlobals,resetLifecycle,target,queries,zoneLifecycle,liveWorld,membershipWork,
        *originalResetGround,originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:
        isMbTrack?
        RecoveryOwners50MB{native_original_human_recovery::Owners50MB{gcLightOwner,racerCables[0]->visibility50(),
        controllerD40,lapContinuity.prefix,*field24C,mbMapView,selectedRuntimeHeightDomain,
        resetGlobals,resetLifecycle,target,queries,zoneLifecycle,liveWorld,membershipWork,
        *originalResetGround,originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}}:
        RecoveryOwners50MB{native_original_human_recovery::Owners{gcLightOwner,racerCables[0]->visibility50(),
        controllerD40,lapContinuity.prefix,*field24C,*navigationMap,selectedRuntimeHeightDomain,
        resetGlobals,resetLifecycle,target,queries,zoneLifecycle,liveWorld,membershipWork,
        *originalResetGround,originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount}};
    physicalReturned=std::visit([&](auto& actual){return native_original_human_recovery::resume(s,x.bindings,x.parameters,navigationProgress,
        humanBaseUndertrack,originalHumanRecovery,actual,*originalRecoveryWorkspace,originalHumanRecoveryDiagnostic);},owners);
   }else{originalHumanRecoveryDiagnostic.sourcePC=0x124cb0u;
    originalHumanRecoveryDiagnostic.message="actual human reset world registration queue is pending";}
  }else{originalHumanRecoveryDiagnostic.sourcePC=0x1a259cu;
   originalHumanRecoveryDiagnostic.message="actual human D40/D44/camera/cables reset ownership is unavailable";}
  // The physical lease has ended. Camera3C can now borrow the SAME4C0 world.
  // Only after the complete camera tail returns may228BC4 resume. The saved
  // pre-reset stack position survives; damage, packed controls and type stay.
  if(physicalReturned&&resetHumanCamera(originalHumanRecovery.selection,
      originalHumanRecovery.placement,humanBaseUndertrack.serial)){
   baseAdvanced=resumeVehicleBaseAfterUndertrack50X(s,x.bindings,clock,baseBinding,
       baseTail,baseGlobals,coldFlame,baseWorkspace,x.workspace,x.diagnostic,humanBaseUndertrack);
   if(!baseAdvanced){originalHumanRecoveryDiagnostic.sourcePC=x.diagnostic.sourceStage;
    originalHumanRecoveryDiagnostic.message="actual228BC4 base continuation rejected after complete human reset";}
  }
  if(!baseAdvanced){
   recoveryPaused=true;
   std::cerr<<"ORIGINAL HUMAN undertrack continuation: "<<originalHumanRecoveryDiagnostic.message
       <<" physical_stage="<<static_cast<unsigned>(originalHumanRecovery.stage)
       <<" pc="<<std::hex<<originalHumanRecoveryDiagnostic.sourcePC<<std::dec
       <<"; actual completed prefixes retained, no HOST checkpoint.\n";
   return false;
  }
 }
 if(!baseAdvanced&&x.diagnostic.error==VehicleFrameError49W::ExternalCut&&
    x.diagnostic.sourceStage==0x228BBC){
  addCounter50DU(nativeCalls);recoveryPaused=true;
  if(originalAiEnabled){
   std::cerr<<"ORIGINAL AI source-recovery boundary: kind=undertrack pc="<<std::hex
       <<x.diagnostic.sourceStage<<std::dec<<" phase="<<static_cast<unsigned>(s.phase)
       <<"; no checkpoint prepared or session rebuilt.\n";
   require50DU(false,"original AI single session reached native undertrack boundary");
  }
  if(opponentFrame)return false; // A failed AI never restarts the human session.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack,"MB/TC/RC/TB/GB/MC native session cannot enter legacy HOST checkpoint prepare");
  const bool prepared=recovery50FI::prepare(*navigationMap,navigationProgress,s,x.bindings,
      x.diagnostic,x.parameters.visual.verticalLimit7EC,liveWorld,nextCheckpoint,recoveryDiagnostic);
  const bool grounded=prepared&&recovery50FI::resolveGround(ground,nextCheckpoint,16,0,
      groundStack.data(),groundStack.size(),groundedCheckpoint,recoveryDiagnostic);
  if(grounded){
   checkpointOutput=nextCheckpoint;checkpointRequested=true;
   std::cerr<<"Automatic checkpoint recovery after "<<completedFrames<<" completed ticks: anchor="
            <<nextCheckpoint.selection.anchorNode<<" zone="<<nextCheckpoint.primaryZone
            <<". Rebuilding native vehicle session; damaged values and progress retained.\n";
  }else{
   std::cerr<<"Checkpoint recovery unavailable: error="<<static_cast<unsigned>(recoveryDiagnostic.error)
            <<" source="<<static_cast<unsigned>(recoveryDiagnostic.source.error)
            <<" ground="<<static_cast<unsigned>(recoveryDiagnostic.groundPhase)
            <<". Last committed image retained. F5/restart returns to grid; quit exits.\n";
  }
  return false;
 }
 runNative([&]{return baseAdvanced;},"persistent base engine");
 runNative([&]{return finishVehicleBaseAndMainCache50Y(s,x.bindings,baseTail,true,poseHistory,f::cut(PreservingCut49W::OtherChildrenBeforeMain340),x.workspace,x.diagnostic);},"persistent base pose history");
 runNative([&]{return refreshSecondaryVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenBetween340And35C),x.workspace,x.diagnostic);},"secondary cache");
 require50DU(s.phase==VehicleFramePhase49W::AfterBodyChildren,
     "racer child prefix did not reach the source camera cut");
 if(isMcTrack){
  auto& sequence=nativeParticleChildSequence[actualRacerSlot];require50DU(sequence!=std::numeric_limits<std::uint64_t>::max(),"MC child sequence exhausted");
  require50DU(mcParticleVehicles&&mcParticleVehicles->advanceChildren(actualRacerSlot,nativeEffects,nativeEffectCount,++sequence,preparedOuterFactor,false,
    s.noiseOwner().sharedRandomState,sessionRandom,applicationClock,nativeFireSerial,nativeWaterApplication.sparksGlobals),"MC actual prior dust/spark/fire then spray children rejected");
 }
 if(nativeGcWater){
  auto& sequence=nativeParticleChildSequence[actualRacerSlot];
  require50DU(sequence!=std::numeric_limits<std::uint64_t>::max(),"native particle child sequence exhausted");
  ++sequence;
  const auto common=nativeParticleChildren[actualRacerSlot].advanceChildren(nativeParticleAuthoring,
      nativeEffects,nativeEffectCount,sequence,preparedOuterFactor,false,s.noiseOwner().sharedRandomState,
      sessionRandom,applicationClock,nativeFireSerial,nativeWaterApplication.sparksGlobals);
  if(!common.okay()){
   std::cerr<<"GC native particle boundary: owner="<<common.ownerIndex<<" pc="<<std::hex
       <<common.sourcePC<<std::dec<<'\n';
   require50DU(false,"actual canonical particle child update rejected");
  }
  require50DU(nativeWaterRacers[actualRacerSlot]->sprayChildren(sequence,preparedOuterFactor,false,
      s.noiseOwner().sharedRandomState,sessionRandom,applicationClock).okay(),"actual water spray child update rejected");
  require50DU(nativeParticleChildren[actualRacerSlot].lateChildrenGate().okay(),
      "actual formerly disabled late particle child reached unsupported update");
 }
 if(sourceAiAudio)require50DU(originalAiAudio->children(actualRacerSlot,s,x.bindings,boostState),
     "native D48/D40 audio child service rejected");
 return true;
}

// Original 228390 (plain effect tail): the same owner walk the ordinary frame
// runs at 228B64, which 228764/228770 call directly for states 3/4.
bool Context::stopDestroyedPlainEffects(const char* label) {
 PlainEffectOwners50Q plainOwners{};
 const SceneNodeIndex49P plainModels[]={x.bindings.visual.left1BC,x.bindings.visual.right1B8,
     x.bindings.visual.bottomLeft1C4,x.bindings.visual.bottomRight1C0};
 for(unsigned engine=0;engine<4;++engine)if(plainModels[engine]!=SceneNodeIndex49P::none)
  for(unsigned array=0;array<3;++array){
   const auto index=effectOwnerIndex[array][engine];
   require50DU(index<nativeEffectCount,"destroyed plain effect owner bank does not match the racer");
   const auto& owner=nativeEffects[index];plainOwners.slots[array*4+engine]={owner.id,owner.state.flags1D4};
  }
 const auto plain=inspectPlainEffectTail50Q(plainOwners);
 require50DU(plain.status==PlainEffectStatus50Q::Complete,label);
 return consumeEffects(plain.events,plain.count);
}

bool Context::commitOriginalDestroyed(std::size_t actualRacerSlot,
    const ImpactTransitionResult50C& decision,float preparedOuterFactor) {
 using namespace rr::game;
 (void)preparedOuterFactor;
 require50DU(actualRacerSlot<nativeDestroyedRacers50PD.size(),"destroyed racer slot outside the race field");
 auto& record=nativeDestroyedRacers50PD[actualRacerSlot];
 auto& main=s.bodies.main;
 const bool human=s.fields.vehicle58==0u;
 // 228A30/228A84: the limp countdown's escalation (no 225C50 suffix).
 const bool limpEscalation=decision.sourcePC==0x228a30u||decision.sourcePC==0x228a84u;
 if(limpEscalation)require50DU(record.limping&&!record.destroyed&&(main.vehicleState40==1u||main.vehicleState40==2u),
     "limp escalation reached a racer that is not limping");
 else require50DU(!record.destroyed&&!record.limping&&main.vehicleState40==0u,"destroy transition reached a racer already out");
 // 225EC8..226184: the 225C50 suffix after a dispatched handler.
 auto applySuffix=[&](){
  // 225EC8..226034 kill credit: a dispatched handler, D30 != -1 and 188FB0
  // (profile,1)==0, i.e. two-player game34==2 or the one-hit option 10FE0==0.
  // Race1B0 bit4 is not represented. Applied at the next world boundary.
  record.killCreditPending=record.attackerD30!=0xffffffffu&&record.attackerD30<racerBank.count()&&
      record.attackerD30!=actualRacerSlot&&(dynamicProfile.game34==2u||dynamicProfile.game10FE0==0u);
  // The human's player-event projection of its CD0 (consumed by the tournament
  // result, 1875C0) is a host object, credited here; the arena CD0 at the
  // next world boundary.
  if(record.killCreditPending&&record.attackerD30==0u&&humanCourseEnabled)++playerEvents.countCD0;
  // 226054..226184: when the victim's D2C timer is positive (recently hit),
  // the shared 308030 stream (lazy 3D6B30 seed, 3D6B38 guard) picks music
  // song 17+int(3*u) for 1A46E0. Requested at the next world boundary.
  {std::uint32_t timerBits=poseHistory.timerD2C;float timer=0.f;std::memcpy(&timer,&timerBits,4);
   if(timer>0.f){
    auto& random=sessionRandom;auto& raw=s.noiseOwner().sharedRandomState;
    if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
    if(!random.seed.seed3D6B30){std::uint32_t stamp=0;
     require50DU(sampleHostTimestamp50ID(applicationClock,stamp),"destroy music seed clock unavailable");
     random.seed.seed3D6B30=stamp;raw=stamp;}
    raw=raw*0x41c64e6du+0x3039u;
    const auto drawn=static_cast<std::int32_t>(raw&0x7fffffffu);
    volatile float unit=std::bit_cast<float>(0x30000000u)*static_cast<float>(drawn);
    volatile float scaled=3.f*unit;
    const auto song=static_cast<std::int32_t>(scaled)+17;
    if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
    require50DU(!random.range.rangeCheck3D6B38||(song>=17&&song<=19),"destroy music song outside 17..19 (226140 assertion)");
    record.musicSong=static_cast<std::uint32_t>(song);
   }}
 };
 // 227370 (225E7C, slot90) / 227A40 (225EC0, slot94) for the local human:
 // the pod loses one side and keeps racing (states1/2). The AI (58 != 0)
 // takes 226750 directly (2273A0/227A70).
 if((decision.sourcePC==0x225e7cu||decision.sourcePC==0x225ec0u)&&human&&!s.fields.vehicle3C){
  const bool left=decision.sourcePC==0x225e7cu;
  const std::uint32_t side=left?1u:2u;
  record={};record.limping=true;record.limpState40=side;record.frame=completedFrames;record.limpFrame=completedFrames;
  record.decisionPC=decision.sourcePC;record.handler=left?0x227370u:0x227a40u;
  record.attackerD30=poseHistory.markerD30;
  // 2273C4/2273C8 (227A94/227A98): 40 = 1/2, 4C = 2.0f.
  main.vehicleState40=side;record.state40=side;record.limp4C=std::bit_cast<float>(0x40000000u);
  // 2273EC..227430 (227ABC..227B00): Stop84 on the LOST side's 808 and 81C only.
  {ImpactEffectEvent50B stops[2]{};std::size_t count=0;const unsigned i=left?0u:1u;
   if(resetEffectBindings.owners808[i]){stops[count].kind=ImpactEffectKind50B::Stop84;
    stops[count].sourcePC=left?0x227400u:0x227ad0u;stops[count].target=resetEffectBindings.owners808[i];++count;}
   if(resetEffectBindings.table81CPresent){
    if(resetEffectBindings.controls81C[i])stopEffect50BC(*resetEffectBindings.controls81C[i]);
    else if(resetEffectBindings.owners81C[i]){stops[count].kind=ImpactEffectKind50B::Stop84;
     stops[count].sourcePC=left?0x227428u:0x227af8u;stops[count].target=resetEffectBindings.owners81C[i];++count;}}
   require50DU(consumeEffects(stops,count),"limp entry lost-side effect stops rejected");}
  // 227434..2274CC: human, 3C==0, vibration option: 0.5 s on both motors.
  if(!s.fields.vehicle3C&&(dynamicConsumerFlags11050&4u)&&playerSource5C<2u){
   auto& device=feedbackDevices[playerSource5C];clearControllerFeedback50KZ(device,2);
   require50DU(requestControllerFeedback50KZ(device,2,0u,.5f),"limp entry vibration request rejected");}
  // 2274D0 (227BA0): 48 = 0 (no Binder, no detached-cable path). The port's
  // only writer of 48 is its 20F6B4 constructor, which stores 0, so the field
  // already holds 0 here; an unconstructed 48 has no owner, so refuse.
  require50DU(s.fields.cableMode48.sourceConstructed(),"limp entry racer field 48 is not constructed");
  // 227920..2279BC (227FF4..228090): the cockpit becomes the physical body on
  // one tether, from the surviving side's hitch to its engine.
  {using namespace rr::game::detail;
   const SceneGraph49P g{s.nodes,s.sceneNodeCount};const auto& l=x.bindings.lifecycle;
   const SceneNodeIndex49P engineNode=left?l.right1B8:l.left1BC,hitchNode=left?l.rightCable1E4:l.leftCable1E0;
   for(const auto n:{engineNode,hitchNode,l.main190,l.model1B4,l.physical19C})
    require50DU(static_cast<std::int32_t>(n)>=0&&static_cast<std::uint32_t>(n)<s.sceneNodeCount,"limp entry scene node outside the racer graph");
   s.bodies.vehicle838=1u;s.lifecycle.vehicle83C=0u;                 // 227920/227928
   SceneVectorContext49U scratch{0};VisualVector49R engine{},hitch{};
   require50DU(position49U(g,engineNode,l.main190,scratch,engine)&&     // 22793C: surviving engine rel 190
       position49U(g,hitchNode,l.model1B4,scratch,hitch),"limp entry tether anchors unavailable"); // 22794C: hitch rel 1B4
   const float* length=s.fields.length858.value();
   require50DU(length&&std::isfinite(*length),"limp entry tether length 858 unavailable");
   PodSphereBody49L cockpit{};makeSecondarySphereBody49M(s.bodies.secondary,x.parameters.bodies.secondary,cockpit);
   PodSphereBody49L body{};
   {const auto& m=s.bodies.main;const auto& lanes=s.bodies.mainSphere;SecondaryBodyState49M view{};
    view.root=m.components.root;view.scenePresent=m.domain.sceneAttached;view.sceneFlags190=lanes.sceneFlags190;
    view.velocity70W=lanes.velocity70W;view.angularVelocityB0W=lanes.angularVelocityB0W;view.force180W=lanes.force180W;
    view.torque190W=lanes.torque190W;SecondaryBodyParameters49M par{};par.body=x.parameters.bodies.main.body;
    par.centerOffset=x.parameters.bodies.main.centerOffset;par.centerOffsetW=x.parameters.bodies.mainSphere.centerOffsetW;
    std::memcpy(par.inverseInertiaExtra,x.parameters.bodies.mainSphere.inverseInertiaExtra,sizeof par.inverseInertiaExtra);
    makeSecondarySphereBody49M(view,par,body);}
   PodSphereVector49L a{},b{};std::memcpy(a.v,hitch.v,sizeof a.v);std::memcpy(b.v,engine.v,sizeof b.v);
   require50DU(setupPodSphereA49L(s.bodies.joint,cockpit,a)&&setupPodSphereB49L(s.bodies.joint,&body,b,*length),
       "limp entry 1B64D0/1B6670 tether rebind rejected");                 // 22795C/227970
   require50DU(copyLocal49U(g,l.physical19C,s.nodes[static_cast<std::uint32_t>(l.model1B4)].localC0),
       "limp entry 28B3E0 cockpit pose copy rejected");                    // 227980
   // The secondary body's scene view is the 19C node itself (body+288):
   // republish it as the frame's own projection does after a scene write.
   {const auto& node=s.nodes[static_cast<std::uint32_t>(l.physical19C)];
    s.bodies.secondary.root.scene=node.world100;
    s.bodies.secondary.sceneFlags190=(s.bodies.secondary.sceneFlags190&0xffffff00u)|node.sceneFlags190;}
   resetSecondaryActivation49V(s.bodies.secondary,s.secondarySupplement,x.parameters.activation); // 227988 (35C vAC)
   copySecondaryMainVelocities49V(s.bodies.main.components.root,s.bodies.mainSphere.velocity70W,
       s.bodies.mainSphere.angularVelocityB0W,s.bodies.secondary);         // 22799C..2279BC
   std::cerr<<"pod_limp_joint side="<<(left?"left":"right")<<" hitch="<<hitch.v[0]<<','<<hitch.v[1]<<','<<hitch.v[2]
       <<" engine="<<engine.v[0]<<','<<engine.v[1]<<','<<engine.v[2]<<" length858="<<*length<<'\n';}
  // Rows 11..20: the lost side's [Wreckage] collections fly off from the
  // engine models (374[1]/374[3] left, 374[2]/374[4] right; 227D88 negates
  // the kick for 374[2] only).
  {const auto& v=x.bindings.visual;const auto& l=x.bindings.lifecycle;
   const SceneNodeIndex49P models[2]={left?v.left1BC:v.right1B8,left?v.bottomLeft1C4:v.bottomRight1C0};
   const std::size_t collections[2]={left?1u:2u,left?3u:4u};const float signs[2]={left?1.f:-1.f,1.f};
   native_wreck_debris50PD::LimpCollection list[2]{};std::size_t count=0;
   for(unsigned k=0;k<2;++k){const auto n=static_cast<std::int32_t>(models[k]);
    if(models[k]==SceneNodeIndex49P::none)continue;
    require50DU(n>=0&&static_cast<std::uint32_t>(n)<s.sceneNodeCount,"limp engine model outside the racer graph");
    list[count++]={collections[k],s.nodes[static_cast<std::uint32_t>(n)].world100.v,signs[k]};}
   const auto body=static_cast<std::uint32_t>(x.bindings.animation.bodyScene288);
   const auto container=static_cast<std::uint32_t>(l.container194);
   require50DU(body<s.sceneNodeCount&&container<s.sceneNodeCount,"limp body/container node unavailable");
   std::size_t first=0;
   const auto spawned=native_wreck_debris50PD::spawnLimp(actualRacerSlot,list,count,s.nodes[body].world100.v,
       main.components.root.body.velocity.v,main.components.root.body.angularVelocity.v,
       main.components.root.worldAngularVelocityA0.v,s.nodes[container].world100.v+12,s.noiseOwner().sharedRandomState,first);
   const auto* w=s.nodes[body].world100.v;
   native_wreck_effects50PD::destroyed(actualRacerSlot,false,{w[12],w[13],w[14]},{w[12],w[13],w[14]});
   const auto& live=native_wreck_debris50PD::sim().pieces;
   std::cerr<<"debris_spawn_limp slot="<<actualRacerSlot<<" collections=0x"<<std::hex
       <<native_wreck_debris50PD::sim().activeCollections[actualRacerSlot%8u]<<std::dec<<" pieces="<<spawned
       <<" groups="<<native_wreck_debris50PD::sim().groupCount;
   for(std::size_t i=first;i<live.size();++i){const auto& q=live[i];
    std::cerr<<" ["<<q.index<<" at="<<q.position.x<<','<<q.position.y<<','<<q.position.z
        <<" v="<<q.velocity.x<<','<<q.velocity.y<<','<<q.velocity.z<<" wz="<<q.angular.z<<']';}
   std::cerr<<'\n';}
  // 2279C4..227A24: the 79C refresh. NOT REPRESENTED: its outputs 698/69C
  // (122350) are the racer's zone records that the racer constructor hands
  // to each wreck collection (212A40..212A68 -> 22BFD0) for the pieces'
  // world queries; the native debris contact queries the whole track's
  // collision candidates instead, so nothing in the port reads them.
  std::cerr<<"pod_limp_query79c slot="<<actualRacerSlot<<" represented=0 consumer=22BFD0_wreck_zone\n";
  // The suffix runs at limp entry (s1 = 1 after the handler, 225EC8 onward).
  applySuffix();
  std::cerr<<"pod_limp_enter slot="<<actualRacerSlot<<" side="<<(left?"left":"right")<<" state40="<<side
      <<" t4C="<<record.limp4C<<" frame="<<completedFrames<<" attacker="<<static_cast<std::int32_t>(record.attackerD30)
      <<" kill_credit="<<record.killCreditPending<<" music_song="<<record.musicSong<<'\n';
  // The caller continues 228620 with this same frame.
  return true;
 }
 const NativeDestroyedRacer50PD limp=record;
 std::uint32_t handler=0;
 switch(decision.sourcePC){
  case 0x228a30u:handler=0x2261b0u;break;                    // limp 4C/BA8 -> slot88
  case 0x228a84u:handler=0x226750u;break;                    // limp 4C/BA8 -> slot8C
  case 0x225d10u:handler=0x2261b0u;break;                    // slot88 explode
  case 0x225d9cu:case 0x225e28u:handler=0x226750u;break;     // slot8C wreck
  // slot90/94: AI 227370/227A40 dispatch 226750 directly (the human
  // limps instead, handled above before this switch).

  case 0x225e7cu:case 0x225ec0u:handler=0x226750u;break;
  default:
   std::cerr<<"original destroy dispatch unknown pc="<<std::hex<<decision.sourcePC<<std::dec<<'\n';
   require50DU(false,"original destroy dispatch site is not one of 225D10/225D9C/225E28/225E7C/225EC0");
 }
 const bool explode=handler==0x2261b0u;
 // 2261C4/2272F0 state, 2261CC/22675C 4C=0, 2261D8 268=0.6f (0x3F19999A).
 main.vehicleState40=explode?4u:3u;
 record={};record.destroyed=true;record.handler=handler;record.decisionPC=decision.sourcePC;
 if(limpEscalation){record.limpState40=limp.limpState40;record.limpFrame=limp.limpFrame;record.limpFrames=limp.limpFrames;
  record.attackerD30=limp.attackerD30;record.suffixApplied=true;}
 record.state40=main.vehicleState40;record.timer4C=0.f;
 {std::uint32_t bits=explode?0x3f19999au:0u;std::memcpy(&record.timer268,&bits,4);}
 record.frame=completedFrames;
 // 225F04: the victim's D30 names the racer that last hit it (2 s window
 // via D2C). Its CD0 kill count and career/hall effects are not yet joined.
 if(!limpEscalation)record.attackerD30=poseHistory.markerD30;
 // 226288..2262A4: explosion point = body288 local XYZ (28B180) with Y+5.0f.
 const auto& p=main.components.root.body.position;
 {const auto node=static_cast<std::int32_t>(x.bindings.animation.bodyScene288);
  require50DU(node>=0&&static_cast<std::uint32_t>(node)<s.sceneNodeCount,"destroyed racer body288 unavailable");
  const auto* local=s.nodes[static_cast<std::uint32_t>(node)].localC0.v+12;
  volatile float raised=local[1]+5.f;
  record.explosion270[0]=local[0];record.explosion270[1]=raised;record.explosion270[2]=local[2];record.explosion270[3]=1.f;}
 {
  // 2261F8..226278 / 226780..226800: Stop84 on 808[0], 808[1], then 81C[0],
  // 81C[1] when present. Both handlers do this before D40 virtual9C.
  ImpactEffectEvent50B stops[4]{};std::size_t count=0;
  const std::uint32_t pcs808[2]={0x226210u,0x226230u},pcs81C[2]={0x226258u,0x226278u};
  for(unsigned i=0;i<2;++i)if(resetEffectBindings.owners808[i]){
   stops[count].kind=ImpactEffectKind50B::Stop84;stops[count].sourcePC=pcs808[i];
   stops[count].target=resetEffectBindings.owners808[i];++count;}
  if(resetEffectBindings.table81CPresent)for(unsigned i=0;i<2;++i){
   if(resetEffectBindings.controls81C[i])stopEffect50BC(*resetEffectBindings.controls81C[i]);
   else if(resetEffectBindings.owners81C[i]){
    stops[count].kind=ImpactEffectKind50B::Stop84;stops[count].sourcePC=pcs81C[i];
    stops[count].target=resetEffectBindings.owners81C[i];++count;}
  }
  require50DU(consumeEffects(stops,count),"original destroy handler effect stops rejected");
  // 2263F8 / 226808: D40 virtual9C. AI: 16DF70 wreck cue and engine/binder stop.
  if(originalAiAudioEnabled&&s.fields.vehicle58==1u){
   std::uint32_t cue=0;
   require50DU(originalAiAudio->destroyed16DF70(actualRacerSlot,s,x.bindings,sessionRandom,
       s.noiseOwner().sharedRandomState,applicationClock,cue),"original AI D40 16DF70 destroy audio rejected");
   std::cerr<<"pod_destroyed_audio slot="<<actualRacerSlot<<" wreck_cue="<<cue<<'\n';
  }else if(audioEnabled&&human&&liveAudio->outputOpen()){
   // Human D40 virtual9C = 1A0060: v88/v84 stop the binder/engine loops,
   // 2924A0(488) stops the p_event emitter, 292360(48C,1) plays p_debris cue1.
   // Its D44 virtualA8, 4A8 random delay and 178B80 call are not joined.
   using rr::game::AudioBank50HE;using rr::game::AudioOperation50HE;
   auto apply=[&](AudioOperation50HE operation,AudioBank50HE bank,std::uint32_t key,std::uint32_t site){
    rr::game::AudioCommand50HE command{};command.operation=operation;command.cue={bank,key};command.sourceSite=site;
    return liveAudio->applySourceCommand(command);
   };
   apply(AudioOperation50HE::Stop,AudioBank50HE::PodEngine,2u,0x1a0088u);
   apply(AudioOperation50HE::Stop,AudioBank50HE::PodEngine,1u,0x1a0098u);
   apply(AudioOperation50HE::Stop,AudioBank50HE::PodEngine,3u,0x1a0098u);
   for(std::uint32_t key=1;key<=6;++key)apply(AudioOperation50HE::Stop,AudioBank50HE::PodEvent,key,0x1a00b8u);
   apply(AudioOperation50HE::Start,AudioBank50HE::PodDebris,1u,0x1a00ccu);
   std::cerr<<"pod_destroyed_audio slot=0 human_debris_cue=1\n";
  }
 }
 // 2265A0..2266C8 (22CA60 per collection, 22CB9C launch draws) for 2261B0;
 // 226750's in-place activation (22C6A0) has no launch tail.
 {const auto node=static_cast<std::uint32_t>(x.bindings.animation.bodyScene288);
  std::size_t skippedActive=0;
  const auto spawned=native_wreck_debris50PD::spawn(actualRacerSlot,explode,s.nodes[node].world100.v,
      main.components.root.body.velocity.v,main.components.root.body.angularVelocity.v,
      main.components.root.worldAngularVelocityA0.v,main.components.root.body.position.v,s.noiseOwner().sharedRandomState,
      limpEscalation&&!explode?s.bodies.secondary.root.body.velocity.v:nullptr,
      limpEscalation&&!explode?s.bodies.secondary.root.body.angularVelocity.v:nullptr,&skippedActive);
  {const auto& d=native_wreck_debris50PD::sim();
   std::cerr<<"debris_spawn slot="<<actualRacerSlot<<" pieces="<<spawned<<" explode="<<explode<<" skipped_active="<<skippedActive
       <<" groups="<<d.groupCount
       <<" track_floor="<<d.floor<<" track_ceiling="<<d.ceiling<<" mesh_candidates="<<d.meshCandidates
       <<" compound="<<d.compoundCandidates<<" callback="<<d.callbackCandidates<<" other="<<d.otherCandidates;}
  const auto& live=native_wreck_debris50PD::sim().pieces;const auto* w=s.nodes[node].world100.v;
  for(std::size_t i=live.size()-spawned;i<live.size();++i){const auto& q=live[i];
   std::cerr<<" ["<<q.index<<" off="<<q.position.x-w[12]<<','<<q.position.y-w[13]<<','<<q.position.z-w[14]
       <<" r="<<q.radius<<" m="<<q.mass<<" i="<<q.inertia<<" drag="<<q.drag
       <<" v="<<q.velocity.x<<','<<q.velocity.y<<','<<q.velocity.z<<']';}
  std::cerr<<'\n';
  native_wreck_effects50PD::destroyed(actualRacerSlot,explode,{w[12],w[13],w[14]},
      {record.explosion270[0],record.explosion270[1],record.explosion270[2]});
  const auto& effects=native_wreck_effects50PD::state();
  std::cerr<<"wreck_effects_start slot="<<actualRacerSlot<<" emitters="<<effects.emitters.size()
      <<" embers_launched="<<effects.embersLaunched
      <<" ready="<<effects.cold<<'\n';}
 // The same 79C refresh tail ends 2261B0/226750; not represented (see the
 // limp entry above).
 std::cerr<<"pod_destroyed_query79c slot="<<actualRacerSlot<<" represented=0\n";
 if(!limpEscalation)applySuffix();
 // 226414..2264AC / 226824..2268BC: human (58==0, 3C==0) with the vibration
 // option (11050 bit2): device virtual40 (238870, clear both) then virtual44
 // (2388B0, both motors) for 2.0 s (explode) or 1.0 s (wreck in place).
 if(human&&!s.fields.vehicle3C&&(dynamicConsumerFlags11050&4u)&&playerSource5C<2u){
  auto& device=feedbackDevices[playerSource5C];
  clearControllerFeedback50KZ(device,2);
  require50DU(requestControllerFeedback50KZ(device,2,0u,explode?2.f:1.f),"destroyed racer vibration request rejected");
  std::cerr<<"pod_destroyed_vibration seconds="<<(explode?2:1)<<" capable="<<device.capable<<'\n';
 }
 std::cerr<<"pod_destroyed slot="<<actualRacerSlot<<" human="<<human<<" handler=0x"<<std::hex<<handler
     <<" dispatch=0x"<<decision.sourcePC<<std::dec<<" state40="<<main.vehicleState40
     <<" frame="<<completedFrames<<" attacker="<<static_cast<std::int32_t>(record.attackerD30)
     <<" kill_credit="<<record.killCreditPending<<" music_song="<<record.musicSong<<" position="<<p.v[0]<<','<<p.v[1]<<','<<p.v[2]<<'\n';
 // The rest of this tick's racer frame does not run for a destroyed racer;
 // the next physics part only closes the phase.
 s.phase=VehicleFramePhase49W::AfterBodyChildren;
 return true;
}

bool Context::advanceDestroyedChildren(std::size_t actualRacerSlot,float preparedOuterFactor) {
 using namespace rr::game;
 require50DU(s.phase==VehicleFramePhase49W::Ready,"destroyed racer frame must begin Ready");
 require50DU(actualRacerSlot<nativeDestroyedRacers50PD.size()&&nativeDestroyedRacers50PD[actualRacerSlot].destroyed,
     "racer in state3/4 has no native destroy record");
 auto& record=nativeDestroyedRacers50PD[actualRacerSlot];++record.destroyedFrames;
 // 228764/228770 -> 228390, skipping controls, boost, animation, visual
 // response, chariot, impacts, repair/effect/transition work and water.
 require50DU(stopDestroyedPlainEffects("destroyed racer 228390 effect tail rejected"),
     "destroyed racer effect stops rejected");
 // 228BC4..228BE0: only state4 decrements 268 by the dispatched factor.
 if(s.bodies.main.vehicleState40==4u)
  require50DU(advanceOriginalExplosionTimer50LA(4u,preparedOuterFactor,record.timer268).okay(),
      "destroyed racer 268 timer rejected");
 // The schedule pairs each racer frame with this tick's dispatched factor.
 s.bodies.main.frameFactor=preparedOuterFactor;
 s.phase=VehicleFramePhase49W::AfterBodyChildren;
 return true;
}
} // namespace interactive_frame50DU

#include "native_wreck_debris_sim50pd.h"
#include "native_wreck_debris_gpu50pd.h"
#include "../../src/game/native_destroyed_racers50pd.h"
#include "original_manager_fields.h"
#include <functional>
// Complete original lexical suffix; caller prefix outlives every owner below.
#include "terrain_interactive_continuation50du.h"
#include "../../src/game/original_human_reset_timers50lc.h"
#include "original_camera_assets50kx.h"
#include "terrain_interactive_loop50du.h" // Implementation-only complete live providers.
#include "terrain_interactive_frame50du.h"
#include "terrain_interactive_publication50du.h"
#include "terrain_interactive_cold_owners50du.h"
#include "../../src/game/original_opponent_selection50kg.h"
#include "../../src/game/original_racer_selection50ij.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "terrain_interactive_cold_contacts50du.h"
#include "player_loading_session50du.h"
#include "cold_load_diagnostics50du.h"
#include "pod_cables50fd_fixture.h"
#include "original_consumed_ra_stage50du.h"
#include "original_results_feedback_scope50la.h"
#include "original_race_hud_scope50la.h"
#include "original_position_stats_adapter50la.h"
#include "original_consumed_ga_stage50du.h"
#include "original_consumed_gc_stage50du.h"
#include "original_consumed_ma_stage50du.h"
#include "original_consumed_sa_stage50du.h"
#include "original_consumed_ta_stage50du.h"
#include "original_native_ta_root_audio50la.h"
#include "rc_positional_sound50rc.h"
#include "tb_positional_sound50tb.h"
#include "gb_positional_sound50gb.h"
#include "mc_positional_sound50mc.h"
#include "mc_camera_assets50mc.h"
#include "mc_static_model_params50mc.h"
#include "mc_cold_visibility50mc.h"
#include "mc_query50mc.h"
#include "gb_cold_visibility50gb.h"
#include "gb_static_model_params50gb.h"
#include "source_grid50gb_fixture.h"
#include "source_grid50tc_fixture.h"
#include "source_grid50tb_fixture.h"
#include "tb_static_model_params50tb.h"
#include "rc_crystal50rc.h"
#include "rc_static_model_params50rc.h"
#include "tc_static_model_params50tc.h"
#include "sc_static_model_params_fixture.h"
#include "mb_static_model_params_fixture.h"
#include "mb_collision_world50mb.h"
#include "mb_source_trigger50mb.h"
#include "tb_door_binding50tb.h"
#include "tb_cold_visibility50tb.h"
#include "tc_gate_visibility50tc.h"
#include "tc_cold_visibility50tc.h"
#include "native_cold_model_retirement50lh.h"
#include "ra_native_session50la.h"
#include "original_ai_physical_recovery.h"
#include "original_human_physical_recovery.h"
#include "gc_particle_materials50kg_fixture.h"
#include "track_normals50hk_fixture.h"
#include "sc_runtime_ground50la.h"
#include "ma_static_model_params_fixture.h"
#include "original_position_stats_prestage50la.h"
#include "gc_static_model_params50kg_fixture.h"
#include "ga_trigger_actions_fixture.h"
#include "sa_trigger_actions_fixture.h"
#include "ma_trigger_actions_fixture.h"
#include "sc_trigger_actions_fixture.h"
#include <fstream>
#include <iomanip>

namespace interactive_continuation50DU {
namespace platform=rr::platform;
namespace gcLighting=rr::game::original_gc_gameplay_lighting_candidate;
namespace f=runtime_startup50fl;
namespace recovery50FI=checkpoint_recovery50fi_fixture;
namespace podTextures=pod_materials50ed_fixture;
using namespace rr::game;
using interactive_frame50DU::BorrowedCall;
using interactive_loop50DU::RacerContactTotals50HV;
using interactive_loop50DU::RacePresentation50HZ;
using interactive_loop50DU::GameplayDisplayWallTotals50DU;
using StartMusicContext=interactive_publication50DU::StartMusicContext;
using J=nlohmann::json;
namespace {
void require50DU(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
// Cold startup only: add resource identity/stage without changing successful
// operations. Keep the original exception nested and its detail in what().
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
void addCounter50DU(std::uint64_t& counter, std::uint64_t amount = 1) noexcept {
    const auto room = std::numeric_limits<std::uint64_t>::max() - counter;
    counter += std::min(room, amount);
}
// Caller-owned wall-only aggregates. These are nested completion_wait
// components, never additional disjoint phases or GPU-only measurements.
using interactive_loop50DU::GameplayDisplayWallTotals50DU;
// Independent of periodic telemetry resets. No clocks or FPS decisions here.
constexpr std::uint32_t raceOpponentCapacity=raceRacerCapacity-1;
} // namespace

SessionExit50DU Context::run() {
 const auto& acceptLiveQuery=this->acceptLiveQueryCall;
 auto& actualHeight=this->actualHeight;
 auto& actualWorld=this->actualWorld;
 auto& allAiControllerSamples=this->allAiControllerSamples;
 auto& allAiEnabled=this->allAiEnabled;
 auto& allAiHostDriverCalls=this->allAiHostDriverCalls;
 auto& allAiKeyboardPackets=this->allAiKeyboardPackets;
 auto& announcerRequestsUnjoined=this->announcerRequestsUnjoined;
 auto& applicationChildren=this->applicationChildren;
 auto& applicationClock=this->applicationClock;
 auto& applicationEvents=this->applicationEvents;
 auto& applicationWrongWayBypass34C=this->applicationWrongWayBypass34C;
 const auto& applyCourseInput=this->applyCourseInputCall;
 auto& audioContactMaterials=this->audioContactMaterials;
 auto& audioDiagnostic=this->audioDiagnostic;
 auto& audioEnabled=this->audioEnabled;
 auto& audioListener=this->audioListener;
 auto& audioListenerReady=this->audioListenerReady;
 auto& audioOutputRequested=this->audioOutputRequested;
 auto& authored=this->authored;
 auto& authoredFinishSteering10A4=this->authoredFinishSteering10A4;
 auto& authoredTrackName=this->authoredTrackName;
 auto& autoDriveEnabled=this->autoDriveEnabled;
 auto& autoStopReported=this->autoStopReported;
 auto& baseGlobals=this->baseGlobals;
 auto& baseTail=this->baseTail;
 auto& baseWorkspace=this->baseWorkspace;
 const auto& beginNativeLightDraw=this->beginNativeLightDrawCall;
 const auto& bindColdNativeEffects=this->bindColdNativeEffectsCall;
 auto& boostButtonCode=this->boostButtonCode;
 auto& boostCoolingCF8=this->boostCoolingCF8;
 auto& boostDiagnostic=this->boostDiagnostic;
 auto& boostParameters=this->boostParameters;
 auto& boostState=this->boostState;
 auto& cableBindings=this->cableBindings;
 auto& cableDiagnostic=this->cableDiagnostic;
 auto& cableMesh=this->cableMesh;
 auto& cableOwner=this->cableOwner;
 auto& cableWorkspace=this->cableWorkspace;
 auto& cameraStartup350=this->cameraStartup350;
 auto& candidates=this->candidates;
 auto& captureFirstWrongWay=this->captureFirstWrongWay;
 auto& capturePath=this->capturePath;
 auto& captureRun=this->captureRun;
 auto& capturedFirstWrongWay=this->capturedFirstWrongWay;
 auto& checkpointInput=this->checkpointInput;
 auto& checkpointOutput=this->checkpointOutput;
 auto& checkpointRequested=this->checkpointRequested;
 auto& clock=this->clock;
 const auto& closeGbCurrentCollision=this->closeGbCurrentCollisionCall;
 const auto& closeMcCurrentCollision=this->closeMcCurrentCollisionCall;
 const auto& closeRcCurrentCollision=this->closeRcCurrentCollisionCall;
 const auto& closeScCurrentMemberships=this->closeScCurrentMembershipsCall;
 const auto& closeTbCurrentCollision=this->closeTbCurrentCollisionCall;
 const auto& closeTcCurrentCollision=this->closeTcCurrentCollisionCall;
 auto& coldFlame=this->coldFlame;
 auto& coldLoadDiagnostics=this->coldLoadDiagnostics;
 const auto& collectRegistered=this->collectRegisteredCall;
 auto& collectedCalls=this->collectedCalls;
 auto& collisionNormalMode=this->collisionNormalMode;
 const auto& commitCandidateEffects=this->commitCandidateEffectsCall;
 auto& completedFrames=this->completedFrames;
 const auto& composePacket=this->composePacketCall;
 const auto& consumeEffects=this->consumeEffectsCall;
 auto& consumedGaStage=this->consumedGaStage;
 auto& consumedGcStage=this->consumedGcStage;
 auto& consumedMaStage=this->consumedMaStage;
 auto& consumedRaStage=this->consumedRaStage;
 auto& consumedSaStage=this->consumedSaStage;
 auto& consumedTaStage=this->consumedTaStage;
 auto& controlPresets=this->controlPresets;
 auto& controller=this->controller;
 auto& controllerActions=this->controllerActions;
 auto& controllerEventStorage=this->controllerEventStorage;
 auto& controllerEvents=this->controllerEvents;
 auto& controllerPresses=this->controllerPresses;
 auto& controllerReleases=this->controllerReleases;
 auto& courseColdLoad=this->courseColdLoad;
 auto& courseDiagnostic=this->courseDiagnostic;
 auto& courseOutput=this->courseOutput;
 auto& courseRaceCold=this->courseRaceCold;
 auto& courseRaceLaunched=this->courseRaceLaunched;
 auto& courseState=this->courseState;
 auto& courseTuning=this->courseTuning;
 auto& destination=this->destination;
 auto& directOriginalHuman=this->directOriginalHuman;
 auto& discardedHostNs=this->discardedHostNs;
 auto& displayWallTotals=this->displayWallTotals;
 auto& drawAssets=this->drawAssets;
 auto& drawWorkspace=this->drawWorkspace;
 auto& dynamicActorResolver=this->dynamicActorResolver;
 auto& dynamicActors=this->dynamicActors;
 const auto& dynamicBoundary=this->dynamicBoundaryCall;
 auto& dynamicConsumerFlags11050=this->dynamicConsumerFlags11050;
 auto& dynamicContactEnabled=this->dynamicContactEnabled;
 auto& dynamicDamageCursor=this->dynamicDamageCursor;
 auto& dynamicEventBindings=this->dynamicEventBindings;
 auto& dynamicEventCursor=this->dynamicEventCursor;
 auto& dynamicEvents=this->dynamicEvents;
 auto& dynamicFinalTuning=this->dynamicFinalTuning;
 auto& dynamicLapCursor=this->dynamicLapCursor;
 auto& dynamicMusicProfile=this->dynamicMusicProfile;
 auto& dynamicPeers=this->dynamicPeers;
 auto& dynamicPodEffect=this->dynamicPodEffect;
 auto& dynamicProfile=this->dynamicProfile;
 auto& dynamicRacer60=this->dynamicRacer60;
 auto& dynamicRegistry=this->dynamicRegistry;
 auto& dynamicSelfSlot=this->dynamicSelfSlot;
 auto& dynamicTuning=this->dynamicTuning;
 auto& effectCommands=this->effectCommands;
 auto& effectOwnerIndex=this->effectOwnerIndex;
 auto& effects=this->effects;
 auto& effectsWorkspace=this->effectsWorkspace;
 auto& endpointScenes=this->endpointScenes;
 auto& endpoints=this->endpoints;
 auto& engineDamage=this->engineDamage;
 auto& engineDamageCompletions=this->engineDamageCompletions;
 auto& engineDamageDips=this->engineDamageDips;
 auto& engineDamageDraws=this->engineDamageDraws;
 auto& engineDamageReturns=this->engineDamageReturns;
 auto& engineDamageUpdates=this->engineDamageUpdates;
 auto& eventContext=this->eventContext;
 auto& executeOriginalManualReset=this->executeOriginalManualReset;
 auto& feedbackDevices=this->feedbackDevices;
 auto& feedbackHelpers=this->feedbackHelpers;
 auto& feedbackHost=this->feedbackHost;
 auto& feedbackLeaseRefreshes=this->feedbackLeaseRefreshes;
 auto& feedbackNoDevice=this->feedbackNoDevice;
 auto& feedbackPhysicalDevice=this->feedbackPhysicalDevice;
 auto& feedbackSubmissions=this->feedbackSubmissions;
 auto& finishDriverAppliedThisFrame=this->finishDriverAppliedThisFrame;
 auto& finishDriverTicks=this->finishDriverTicks;
 auto& finishManualInputTicks=this->finishManualInputTicks;
 auto& freshMainProbe=this->freshMainProbe;
 auto& freshProbe100=this->freshProbe100;
 auto& freshTerrainProbe=this->freshTerrainProbe;
 const auto& fulfillDynamicMusic=this->fulfillDynamicMusicCall;
 auto& gaAnimatedMembership=this->gaAnimatedMembership;
 auto& gaAnimatedWorld=this->gaAnimatedWorld;
 auto& gaAnimationDiagnostic=this->gaAnimationDiagnostic;
 auto& gaGateAudio=this->gaGateAudio;
 auto& gaOriginalDisc=this->gaOriginalDisc;
 auto& gaOriginalWorld=this->gaOriginalWorld;
 auto& gaTriggerActions=this->gaTriggerActions;
 auto& gaTriggerAuthoring=this->gaTriggerAuthoring;
 auto& gaTriggerMembership=this->gaTriggerMembership;
 const auto& gatherRegistered=this->gatherRegisteredCall;
 auto& gbAiMetadata=this->gbAiMetadata;
 auto& gbAiView=this->gbAiView;
 auto& gbAnimationBinding=this->gbAnimationBinding;
 auto& gbAnimationBindingDiagnostic=this->gbAnimationBindingDiagnostic;
 auto& gbAnimationRuntime=this->gbAnimationRuntime;
 auto& gbColdSources=this->gbColdSources;
 auto& gbCollisionCleanupDone=this->gbCollisionCleanupDone;
 auto& gbCollisionWorld=this->gbCollisionWorld;
 auto& gbFogOwner=this->gbFogOwner;
 auto& gbLightOwner=this->gbLightOwner;
 auto& gbMapMetadata=this->gbMapMetadata;
 auto& gbMapView=this->gbMapView;
 auto& gbNavigation=this->gbNavigation;
 auto& gbNavigationOwner=this->gbNavigationOwner;
 auto& gbOriginalWorld=this->gbOriginalWorld;
 auto& gbParticleAttachments=this->gbParticleAttachments;
 auto& gbParticleAuthoring=this->gbParticleAuthoring;
 auto& gbParticleRender=this->gbParticleRender;
 auto& gbParticleRuntime=this->gbParticleRuntime;
 auto& gbParticleRuntimeDiagnostic=this->gbParticleRuntimeDiagnostic;
 auto& gbParticleWindow=this->gbParticleWindow;
 auto& gbPodSound=this->gbPodSound;
 auto& gbPositionalAudio=this->gbPositionalAudio;
 auto& gbQueryBinding=this->gbQueryBinding;
 const auto& gbRemainingPayloadBytes=this->gbRemainingPayloadBytesCall;
 auto& gbTrackParticles=this->gbTrackParticles;
 auto& gbTriggerActions=this->gbTriggerActions;
 auto& gbWorldBorrow=this->gbWorldBorrow;
 auto& gcAnimatedMembership=this->gcAnimatedMembership;
 auto& gcAnimatedWorld=this->gcAnimatedWorld;
 auto& gcAnimationDiagnostic=this->gcAnimationDiagnostic;
 auto& gcDrawLights=this->gcDrawLights;
 auto& gcGateAudio=this->gcGateAudio;
 auto& gcGraphicsSequence=this->gcGraphicsSequence;
 auto& gcLightDiagnostic=this->gcLightDiagnostic;
 auto& gcLightOwner=this->gcLightOwner;
 auto& gcNativeGate=this->gcNativeGate;
 auto& gcNativeWorldSelected=this->gcNativeWorldSelected;
 auto& gcOriginalDisc=this->gcOriginalDisc;
 auto& gcOriginalWorld=this->gcOriginalWorld;
 auto& gcTriggerActions=this->gcTriggerActions;
 auto& gcTriggerAuthoring=this->gcTriggerAuthoring;
 auto& gcTriggerMembership=this->gcTriggerMembership;
 auto& generatedEffects=this->generatedEffects;
 auto& ground=this->ground;
 auto& groundStack=this->groundStack;
 auto& headlessLive=this->headlessLive;
 auto& host=this->host;
 auto& hostDiagnostic=this->hostDiagnostic;
 auto& humanBaseUndertrack=this->humanBaseUndertrack;
 auto& humanCameraInputRoute=this->humanCameraInputRoute;
 auto& humanControlInput=this->humanControlInput;
 auto& humanCourseEnabled=this->humanCourseEnabled;
 auto& humanResultsActive=this->humanResultsActive;
 // The exact consumed human-RA Race already owns constructor1A6660's4.f.
 // Bind existing manager/camera/ending-clock users to THAT scalar; never copy
 // the Setup timer or reset94 here. Other routes retain their previous owner.
 // Keep physical storage alive through all nested borrowers and close/unwind;
 // this native shared reference does not retain its source counted authority.
 const auto humanEndRace94=!humanCourseEnabled?std::shared_ptr<OriginalRaceConstructor50LA>{}:
     consumedRaStage.race()?consumedRaStage.race():consumedTaStage.race();
 if(humanEndRace94){
  require50DU(humanEndRace94->resultsFeedbackThread_==std::this_thread::get_id()&&
      this->nativeOriginalFleet&&(this->isRaTrack||this->isTaTrack)&&
      this->originalMainApplication.race28C()==humanEndRace94.get()&&
      humanEndRace94->application_==this->originalMainApplication.entities()&&
      humanEndRace94->profile_==this->originalMainApplication.riderProfile&&
      humanEndRace94->counted_&&!humanEndRace94->rolledBack_&&humanEndRace94->constructorSuffix_&&
      humanEndRace94->entity_.sessionGeneration==this->sessionIndex+1&&
      (this->isRaTrack?humanEndRace94->world74_&&humanEndRace94->world74_==this->raWorld:
          !humanEndRace94->world74_&&humanEndRace94->taWorld74_&&humanEndRace94->taWorld74_==this->taOriginalWorld),
      "human ending94 requires its actual current consumed Race/Application/world");
 }
 auto& humanResultsCountdown=humanEndRace94?humanEndRace94->field94_:this->humanResultsCountdown;
 auto& humanResultsReady=this->humanResultsReady;
 auto& initial=this->initial;
 auto& initialCollisionZone=this->initialCollisionZone;
 auto& initialMetadata=this->initialMetadata;
 auto& initialPairs=this->initialPairs;
 auto& isGaTrack=this->isGaTrack;
 auto& isGbTrack=this->isGbTrack;
 auto& isGcTrack=this->isGcTrack;
 auto& isMaSaTrack=this->isMaSaTrack;
 auto& isMaTrack=this->isMaTrack;
 auto& isMbTrack=this->isMbTrack;
 auto& isMcTrack=this->isMcTrack;
 auto& isRaTrack=this->isRaTrack;
 auto& isRcTrack=this->isRcTrack;
 auto& isSaTrack=this->isSaTrack;
 auto& isScTrack=this->isScTrack;
 auto& isTaTrack=this->isTaTrack;
 auto& isTbTrack=this->isTbTrack;
 auto& isTcTrack=this->isTcTrack;
 auto& keyboardDiagnostic=this->keyboardDiagnostic;
 auto& lapContinuity=this->lapContinuity;
 auto& lapOwner=this->lapOwner;
 auto& lastCommittedWrongWay=this->lastCommittedWrongWay;
 auto& legacyGround=this->legacyGround;
 auto& liveAudio=this->liveAudio;
 auto& liveWorld=this->liveWorld;
 auto& loadedAudioSettings=this->loadedAudioSettings;
 auto& loadedTrack=this->loadedTrack;
 auto& m=this->m;
 auto& maOriginalDisc=this->maOriginalDisc;
 auto& maOriginalWorld=this->maOriginalWorld;
 auto& maTriggerActions=this->maTriggerActions;
 auto& maTriggerAuthoring=this->maTriggerAuthoring;
 auto& maTriggerMembership=this->maTriggerMembership;
 auto& mainContactFrames=this->mainContactFrames;
 auto& mainCrossedPortals=this->mainCrossedPortals;
 auto& mainLiveQuery=this->mainLiveQuery;
 auto& mainMembership=this->mainMembership;
 auto& mainMembershipStorage=this->mainMembershipStorage;
 auto& mainQueryRef=this->mainQueryRef;
 auto& managerRuntimeSelection=this->managerRuntimeSelection;
 auto& materialAudio=this->materialAudio;
 auto& materialOwner=this->materialOwner;
 auto& mbAiMetadata=this->mbAiMetadata;
 auto& mbAiView=this->mbAiView;
 auto& mbCollisionWorld=this->mbCollisionWorld;
 auto& mbMapMetadata=this->mbMapMetadata;
 auto& mbMapView=this->mbMapView;
 auto& mbOriginalDisc=this->mbOriginalDisc;
 auto& mbOriginalWorld=this->mbOriginalWorld;
 auto& mbSourceTriggers=this->mbSourceTriggers;
 auto& mcAdditionalPayloadReserved=this->mcAdditionalPayloadReserved;
 auto& mcAiMetadata=this->mcAiMetadata;
 auto& mcAiView=this->mcAiView;
 auto& mcAnimationBinding=this->mcAnimationBinding;
 auto& mcAnimationBindingDiagnostic=this->mcAnimationBindingDiagnostic;
 auto& mcAnimationRuntime=this->mcAnimationRuntime;
 auto& mcColdSources=this->mcColdSources;
 auto& mcCollisionCleanupDone=this->mcCollisionCleanupDone;
 auto& mcCollisionWorld=this->mcCollisionWorld;
 auto& mcFogOwner=this->mcFogOwner;
 auto& mcLightOwner=this->mcLightOwner;
 auto& mcMapMetadata=this->mcMapMetadata;
 auto& mcMapView=this->mcMapView;
 auto& mcNavigation=this->mcNavigation;
 auto& mcNavigationOwner=this->mcNavigationOwner;
 auto& mcOriginalWorld=this->mcOriginalWorld;
 auto& mcParticleAttachments=this->mcParticleAttachments;
 auto& mcParticleRender=this->mcParticleRender;
 auto& mcParticleRuntime=this->mcParticleRuntime;
 auto& mcParticleRuntimeDiagnostic=this->mcParticleRuntimeDiagnostic;
 auto& mcParticleSource=this->mcParticleSource;
 auto& mcParticleVehicles=this->mcParticleVehicles;
 auto& mcParticleWindow=this->mcParticleWindow;
 auto& mcPositionalAudio=this->mcPositionalAudio;
 auto& mcQueryBinding=this->mcQueryBinding;
 const auto& mcRemainingPayloadBytes=this->mcRemainingPayloadBytesCall;
 auto& mcTrackParticles=this->mcTrackParticles;
 auto& mcTriggerActions=this->mcTriggerActions;
 auto& mcWaterOwner=this->mcWaterOwner;
 auto& mcWaterUpdateSequence=this->mcWaterUpdateSequence;
 auto& mcWorldBorrow=this->mcWorldBorrow;
 auto& membershipWork=this->membershipWork;
 auto& mode1=this->mode1;
 auto& mode1Effects=this->mode1Effects;
 auto& mode1Enabled=this->mode1Enabled;
 auto& monotonicNow50DU=*this->monotonicNow50DU;
 auto& movement=this->movement;
 auto& movementCandidates=this->movementCandidates;
 auto& movementDestination=this->movementDestination;
 auto& movementEffects=this->movementEffects;
 auto& movementFlag=this->movementFlag;
 auto& movementInitial=this->movementInitial;
 auto& movementOutput=this->movementOutput;
 auto& movementResult=this->movementResult;
 auto& movementSource=this->movementSource;
 auto& movementWorkspace=this->movementWorkspace;
 auto& namesForEffects=this->namesForEffects;
 auto& nativeCalls=this->nativeCalls;
 auto& nativeEffectCount=this->nativeEffectCount;
 auto& nativeEffects=this->nativeEffects;
 auto& nativeFireSerial=this->nativeFireSerial;
 auto& nativeGbSelection=this->nativeGbSelection;
 auto& nativeGcWater=this->nativeGcWater;
 const auto& nativeLightFailure=this->nativeLightFailureCall;
 auto& nativeLightingPolicy=this->nativeLightingPolicy;
 auto& nativeMaSelection=this->nativeMaSelection;
 auto& nativeMbSelection=this->nativeMbSelection;
 auto& nativeMcSelection=this->nativeMcSelection;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
 auto& nativeParticleAuthoring=this->nativeParticleAuthoring;
 auto& nativeParticleChildSequence=this->nativeParticleChildSequence;
 auto& nativeParticleChildren=this->nativeParticleChildren;
 auto& nativeRaSelection=this->nativeRaSelection;
 auto& nativeRacerManager=this->nativeRacerManager;
 auto& nativeRcSelection=this->nativeRcSelection;
 auto& nativeRecordActor=this->nativeRecordActor;
 auto& nativeRosterOwners=this->nativeRosterOwners;
 auto& nativeSaSelection=this->nativeSaSelection;
 auto& nativeScSelection=this->nativeScSelection;
 auto& nativeSelectedLights=this->nativeSelectedLights;
 auto& nativeTaSelection=this->nativeTaSelection;
 auto& nativeTbSelection=this->nativeTbSelection;
 auto& nativeTcSelection=this->nativeTcSelection;
 auto& nativeVideo=this->nativeVideo;
 auto& nativeWaterApplication=this->nativeWaterApplication;
 auto& nativeWaterRacers=this->nativeWaterRacers;
 auto& navigationAI=this->navigationAI;
 auto& navigationAIScan=this->navigationAIScan;
 auto& navigationAISequential=this->navigationAISequential;
 auto& navigationCA0=this->navigationCA0;
 auto& navigationCA0Copies=this->navigationCA0Copies;
 auto& navigationDiagnostic=this->navigationDiagnostic;
 auto& navigationGateSkips=this->navigationGateSkips;
 auto& navigationMap=this->navigationMap;
 auto& navigationMapScan=this->navigationMapScan;
 auto& navigationMapSequential=this->navigationMapSequential;
 auto& navigationProgress=this->navigationProgress;
 auto& navigationUpdates=this->navigationUpdates;
 const auto& openNativeVideo=this->openNativeVideoCall;
 auto& opponentAudio=this->opponentAudio;
 auto& opponentBoostAudio=this->opponentBoostAudio;
 auto& opponentFrame=this->opponentFrame;
 auto& originalAi=this->originalAi;
 auto& originalAiAudio=this->originalAiAudio;
 auto& originalAiAudioEnabled=this->originalAiAudioEnabled;
 auto& originalAiAuthoringDiagnostic=this->originalAiAuthoringDiagnostic;
 auto& originalAiCreateSelection=this->originalAiCreateSelection;
 auto& originalAiEnabled=this->originalAiEnabled;
 auto& originalAiHumanCount=this->originalAiHumanCount;
 auto& originalAiOpponentCount=this->originalAiOpponentCount;
 const auto& originalAiOwnsSlot=this->originalAiOwnsSlotCall;
 auto& originalAiReceipt=this->originalAiReceipt;
 auto& originalAiSession=this->originalAiSession;
 auto& originalAiSettings1125C=this->originalAiSettings1125C;
 auto& originalAiTuning=this->originalAiTuning;
 auto& originalCameraEnabled=this->originalCameraEnabled;
 auto& originalHumanRecovery=this->originalHumanRecovery;
 auto& originalHumanRecoveryDiagnostic=this->originalHumanRecoveryDiagnostic;
 auto& originalMainApplication=this->originalMainApplication;
 auto& originalManualRecoveryDiagnostic=this->originalManualRecoveryDiagnostic;
 auto& originalManualResetGraphicsDue=this->originalManualResetGraphicsDue;
 auto& originalManualResetSerial=this->originalManualResetSerial;
 auto& originalRecovery=this->originalRecovery;
 auto& originalRecoveryDiagnostic=this->originalRecoveryDiagnostic;
 auto& originalRecoveryEnabled=this->originalRecoveryEnabled;
 auto& originalRecoveryModels=this->originalRecoveryModels;
 auto& originalRecoveryWorkspace=this->originalRecoveryWorkspace;
 auto& originalResetGround=this->originalResetGround;
 auto& originalUiActive=this->originalUiActive;
 auto& originalUiStarted=this->originalUiStarted;
 auto& packet=this->packet;
 auto& packetLive=this->packetLive;
 auto& peakCollected=this->peakCollected;
 auto& pendingSpeech=this->pendingSpeech;
 auto& phaseTiming=this->phaseTiming;
 auto& physicalControllerReceipt=this->physicalControllerReceipt;
 auto& playerActionIndexFFC=this->playerActionIndexFFC;
 auto& playerArchive=this->playerArchive;
 auto& playerEnvironment=this->playerEnvironment;
 auto& playerEventChild=this->playerEventChild;
 auto& playerEvents=this->playerEvents;
 auto& playerPodId=this->playerPodId;
 auto& playerRacer60=this->playerRacer60;
 auto& playerSource5C=this->playerSource5C;
 auto& playerUsesRosterRig=this->playerUsesRosterRig;
 auto& poseHistory=this->poseHistory;
 auto& powerslideButtonCode=this->powerslideButtonCode;
 const auto& prepareQueryCache=this->prepareQueryCacheCall;
 auto& probeCount=this->probeCount;
 auto& probeIndices=this->probeIndices;
 const auto& publishRecoveryQueries=this->publishRecoveryQueriesCall;
 auto& qfilter=this->qfilter;
 auto& query=this->query;
 auto& queryRecords=this->queryRecords;
 auto& queryResult=this->queryResult;
 auto& raDiagnostic=this->raDiagnostic;
 auto& raFreshProbe=this->raFreshProbe;
 auto& raRuntime=this->raRuntime;
 auto& raWorld=this->raWorld;
 auto& raceAudio=this->raceAudio;
 auto& raceEnabled=this->raceEnabled;
 auto& raceMusic=this->raceMusic;
 auto& raceOpponentCount=this->raceOpponentCount;
 auto& raceRacerCount=this->raceRacerCount;
 auto& raceSetupFrame=this->raceSetupFrame;
 auto& racerCables=this->racerCables;
 auto& racerCollisions=this->racerCollisions;
 auto& racerContactsEnabled=this->racerContactsEnabled;
 auto& racerHudNames=this->racerHudNames;
 auto& racerNames=this->racerNames;
 auto& racerRigs=this->racerRigs;
 auto& racerVisuals=this->racerVisuals;
 auto& rcColdSources=this->rcColdSources;
 auto& rcCollisionCleanupDone=this->rcCollisionCleanupDone;
 auto& rcCollisionWorld=this->rcCollisionWorld;
 auto& rcLightOwner=this->rcLightOwner;
 auto& rcNavigationBorrow=this->rcNavigationBorrow;
 auto& rcOriginalDisc=this->rcOriginalDisc;
 auto& rcPositionalAudio=this->rcPositionalAudio;
 const auto& rcRemainingPayloadBytes=this->rcRemainingPayloadBytesCall;
 auto& rcTriggerActions=this->rcTriggerActions;
 auto& rcTriggerAuthoring=this->rcTriggerAuthoring;
 auto& rcWorldBorrow=this->rcWorldBorrow;
 auto& rebaseBounds=this->rebaseBounds;
 auto& rebaseDiagnostic=this->rebaseDiagnostic;
 auto& rebaseModelCount=this->rebaseModelCount;
 auto& rebaseModels=this->rebaseModels;
 auto& rebaseNodes=this->rebaseNodes;
 auto& rebaseWorkspace=this->rebaseWorkspace;
 auto& records=this->records;
 const auto& recoverPortalBoundary=this->recoverPortalBoundaryCall;
 const auto& recoveryBorrowedQueries=this->recoveryBorrowedQueriesCall;
 auto& recoveryPaused=this->recoveryPaused;
 auto& recoveryPodLive=this->recoveryPodLive;
 const auto& recoveryQueries=this->recoveryQueriesCall;
 auto& recoveryQueryOwners=this->recoveryQueryOwners;
 const auto& refreshAnimatedCandidates=this->refreshAnimatedCandidatesCall;
 const auto& refreshDynamicSelfActor=this->refreshDynamicSelfActorCall;
 auto& registeredCount=this->registeredCount;
 auto& registeredIndices=this->registeredIndices;
 auto& registeredMovement=this->registeredMovement;
 auto& renderDiagnostics=this->renderDiagnostics;
 auto& repairActiveTicks=this->repairActiveTicks;
 auto& repairButtonCode=this->repairButtonCode;
 auto& reportHost50DU=*this->reportHost50DU;
 const auto& reportNavigation=this->reportNavigationCall;
 auto& reportRenderer50DU=*this->reportRenderer50DU;
 auto& requestIntroMusicFade=this->requestIntroMusicFade;
 auto& requestStartMusic=this->requestStartMusic;
 auto& requestedWindowSamples=this->requestedWindowSamples;
 const auto& requireMbRuntimeNavigation50MB=this->requireMbRuntimeNavigation50MBCall;
 const auto& reserveGbAdditionalPayload=this->reserveGbAdditionalPayloadCall;
 const auto& reserveMcAdditionalPayload=this->reserveMcAdditionalPayloadCall;
 const auto& reserveRcAdditionalPayload=this->reserveRcAdditionalPayloadCall;
 const auto& reserveTbAdditionalPayload=this->reserveTbAdditionalPayloadCall;
 auto& resetEffectBindings=this->resetEffectBindings;
 auto& resetEffectState=this->resetEffectState;
 auto& resetEffectWorkspace=this->resetEffectWorkspace;
 auto& resetGlobals=this->resetGlobals;
 auto& resetHumanCamera=this->resetHumanCamera;
 auto& resetLifecycle=this->resetLifecycle;
 auto& response=this->response;
 const auto& retireGbAudioOwners=this->retireGbAudioOwnersCall;
 const auto& retireMcAudioOwners=this->retireMcAudioOwnersCall;
 const auto& retireRcAudioOwners=this->retireRcAudioOwnersCall;
 const auto& retireTbAudioOwners=this->retireTbAudioOwnersCall;
 auto& s=this->s;
 auto& saAnimatedWorld=this->saAnimatedWorld;
 auto& saAnimationDiagnostic=this->saAnimationDiagnostic;
 auto& saOriginalDisc=this->saOriginalDisc;
 auto& saOriginalWorld=this->saOriginalWorld;
 auto& saTriggerActions=this->saTriggerActions;
 auto& saTriggerAuthoring=this->saTriggerAuthoring;
 auto& saTriggerMembership=this->saTriggerMembership;
 const auto& sampleControllerView=this->sampleControllerCall;
 auto& scColdSources=this->scColdSources;
 auto& scGround=this->scGround;
 auto& scMembershipCleanupDone=this->scMembershipCleanupDone;
 auto& scOriginalDisc=this->scOriginalDisc;
 auto& scTriggerActions=this->scTriggerActions;
 auto& scTriggerAuthoring=this->scTriggerAuthoring;
 auto& scTriggerMembership=this->scTriggerMembership;
 auto& sd=this->sd;
 auto& sde=this->sde;
 auto& secondaryContactFrames=this->secondaryContactFrames;
 auto& secondaryCrossedPortals=this->secondaryCrossedPortals;
 auto& secondaryFilter=this->secondaryFilter;
 auto& secondaryLiveQuery=this->secondaryLiveQuery;
 auto& secondaryMembership=this->secondaryMembership;
 auto& secondaryMembershipStorage=this->secondaryMembershipStorage;
 auto& secondaryQuery=this->secondaryQuery;
 auto& secondaryQueryRecords=this->secondaryQueryRecords;
 auto& secondaryQueryRef=this->secondaryQueryRef;
 auto& secondaryRecords=this->secondaryRecords;
 auto& secondaryResponse=this->secondaryResponse;
 auto& secondaryResult=this->secondaryResult;
 auto& secondarySettings=this->secondarySettings;
 auto& secondarySpring=this->secondarySpring;
 const auto& selectedAiCount50MB=this->selectedAiCount50MBCall;
 auto& selectedAudioSettings=this->selectedAudioSettings;
 auto& selectedControls=this->selectedControls;
 const auto& selectedMapCount50MB=this->selectedMapCount50MBCall;
 const auto& selectedMapIndex50MB=this->selectedMapIndex50MBCall;
 const auto& selectedMapNode50MB=this->selectedMapNode50MBCall;
 auto& selectedOriginal=this->selectedOriginal;
 auto& selectedOriginalAiMapMetadata=this->selectedOriginalAiMapMetadata;
 auto& selectedOriginalAiMetadata=this->selectedOriginalAiMetadata;
 auto& selectedRacer60=this->selectedRacer60;
 auto& selectedRuntimeHeightDomain=this->selectedRuntimeHeightDomain;
 auto& selectedTrack=this->selectedTrack;
 auto& selection=this->selection;
 auto& sessionIndex=this->sessionIndex;
 auto& sessionRandom=this->sessionRandom;
 auto& settings=this->settings;
 auto& si=this->si;
 auto& sim=this->sim;
 auto& sip=this->sip;
 auto& sourceAnnouncer=this->sourceAnnouncer;
 auto& sourceBlur=this->sourceBlur;
 auto& sourceCameraCycleAudio=this->sourceCameraCycleAudio;
 auto& sourceConsumedIntroActivation=this->sourceConsumedIntroActivation;
 auto& sourceConsumedPauseActivation=this->sourceConsumedPauseActivation;
 auto& sourceDisplayLifetime=this->sourceDisplayLifetime;
 auto& sourceFollowQueries=this->sourceFollowQueries;
 auto& sourceModelRenderGlobals=this->sourceModelRenderGlobals;
 auto& sourcePause=this->sourcePause;
 auto& sourcePauseObservation=this->sourcePauseObservation;
 auto& sourcePauseParentAttached=this->sourcePauseParentAttached;
 auto& sourcePauseTrack=this->sourcePauseTrack;
 auto& sourcePositionStatsPrestage=this->sourcePositionStatsPrestage;
 auto& sourcePositionStatsPrestageDiagnostic=this->sourcePositionStatsPrestageDiagnostic;
 auto& sourceRaceHud=this->sourceRaceHud;
 auto& sourceRaceHudDisplayReported=this->sourceRaceHudDisplayReported;
 auto& sourceRaceHudRejectionReported=this->sourceRaceHudRejectionReported;
 auto& sourceRaceHudRendering=this->sourceRaceHudRendering;
 auto& speechCounts=this->speechCounts;
 auto& speechCues=this->speechCues;
 auto& speechEvents=this->speechEvents;
 const auto& speechInput=this->speechInputCall;
 auto& speechState=this->speechState;
 auto& spring=this->spring;
 auto& startAtMenu=this->startAtMenu;
 auto& startMusicContext=this->startMusicContext;
 const auto& stopFeedbackTransport=this->stopFeedbackTransportCall;
 auto& submittedDrawFrames=this->submittedDrawFrames;
 auto& sw=this->sw;
 auto& taOriginalDisc=this->taOriginalDisc;
 auto& taOriginalWorld=this->taOriginalWorld;
 auto& tbAiMetadata=this->tbAiMetadata;
 auto& tbAiView=this->tbAiView;
 auto& tbColdSources=this->tbColdSources;
 auto& tbCollisionCleanupDone=this->tbCollisionCleanupDone;
 auto& tbCollisionWorld=this->tbCollisionWorld;
 auto& tbDoorBinding=this->tbDoorBinding;
 auto& tbDoorBindingDiagnostic=this->tbDoorBindingDiagnostic;
 auto& tbDoorDiagnostic=this->tbDoorDiagnostic;
 auto& tbDoorPrefixArmed=this->tbDoorPrefixArmed;
 auto& tbDoorRuntime=this->tbDoorRuntime;
 auto& tbLightOwner=this->tbLightOwner;
 auto& tbMapMetadata=this->tbMapMetadata;
 auto& tbMapView=this->tbMapView;
 auto& tbOriginalDisc=this->tbOriginalDisc;
 auto& tbPodSound=this->tbPodSound;
 auto& tbPositionalAudio=this->tbPositionalAudio;
 const auto& tbRemainingPayloadBytes=this->tbRemainingPayloadBytesCall;
 auto& tbTriggerActions=this->tbTriggerActions;
 auto& tbWorldBorrow=this->tbWorldBorrow;
 auto& tcAiMetadata=this->tcAiMetadata;
 auto& tcAiView=this->tcAiView;
 auto& tcColdSources=this->tcColdSources;
 auto& tcCollisionCleanupDone=this->tcCollisionCleanupDone;
 auto& tcCollisionWorld=this->tcCollisionWorld;
 auto& tcGateDiagnostic=this->tcGateDiagnostic;
 auto& tcGatePrefixArmed=this->tcGatePrefixArmed;
 auto& tcGateRuntime=this->tcGateRuntime;
 auto& tcGateVisibility=this->tcGateVisibility;
 auto& tcGateVisibilityDiagnostic=this->tcGateVisibilityDiagnostic;
 auto& tcGlobalSound=this->tcGlobalSound;
 auto& tcLightOwner=this->tcLightOwner;
 auto& tcMapMetadata=this->tcMapMetadata;
 auto& tcMapView=this->tcMapView;
 auto& tcOriginalDisc=this->tcOriginalDisc;
 auto& tcTriggerActions=this->tcTriggerActions;
 auto& tcWorldBorrow=this->tcWorldBorrow;
 auto& terrainBindings=this->terrainBindings;
 auto& terrainContext=this->terrainContext;
 auto& terrainHitFrames=this->terrainHitFrames;
 auto& terrainLiveOwner=this->terrainLiveOwner;
 auto& terrainMembership=this->terrainMembership;
 auto& terrainMembershipStorage=this->terrainMembershipStorage;
 auto& terrainOwner=this->terrainOwner;
 auto& terrainOwnerCount=this->terrainOwnerCount;
 auto& terrainQueries=this->terrainQueries;
 auto& terrainQueryRef=this->terrainQueryRef;
 auto& terrainSupportedFrames=this->terrainSupportedFrames;
 auto& tournamentLaunchSelection=this->tournamentLaunchSelection;
 auto& tournamentLaunchStamp=this->tournamentLaunchStamp;
 auto& tournamentResultArmed=this->tournamentResultArmed;
 auto& tournamentResultCommitted=this->tournamentResultCommitted;
 auto& trackAdmission=this->trackAdmission;
 auto& trackCounts=this->trackCounts;
 auto& trackSession=this->trackSession;
 auto& trackStorage=this->trackStorage;
 auto& unavailableEffect=this->unavailableEffect;
 auto& usePhysicalGameController=this->usePhysicalGameController;
 auto& windowSwaps=this->windowSwaps;
 auto& workspace=this->workspace;
 auto& wrongWayActiveTicks=this->wrongWayActiveTicks;
 auto& wrongWayContinuation=this->wrongWayContinuation;
 auto& wrongWayDiagnostic=this->wrongWayDiagnostic;
 auto& wrongWayReceipt=this->wrongWayReceipt;
 auto& wrongWayUpdates=this->wrongWayUpdates;
 auto& x=this->x;
 auto& zoneCount=this->zoneCount;
 auto& zoneLifecycle=this->zoneLifecycle;
 auto& zoneQueue=this->zoneQueue;
 auto& zoneScratch=this->zoneScratch;
 auto sampleController=[&](bool advanceFeedback=false){sampleControllerView(advanceFeedback);};
 // Views of original prefix-guard flags only; original guard destructors remain in caller.
 struct ArmedFlagView { bool& armed; };
 ArmedFlagView tbDoorPrefixGuard{tbDoorPrefixArmed},tcGatePrefixGuard{tcGatePrefixArmed};
 // Isolated two-phase seam. Prefix stops at the real body-child cut; suffix
 // retains the existing one-substep physical/query owners. The legacy wrapper
 // below remains for unchanged diagnostics until the root joins the real race
 // child traversal through original_all8_frame::Schedule.
 using NativeRacerFramePart=interactive_frame50DU::Part;
 // The earlier same display/audio stage outlives Bank/Join and their retained
 // native model owners on unwind. RA opens that same Video context below.
 rr::platform::loading_progress50DU::pulse("Preparing race owners");
 original_main_entry50KG::SpeedBoostColdResult50LA sourceSpeedBoost;
 original_main_entry50KG::TimeStatsColdResult50LA sourceTimeStats;
 original_main_entry50KG::DamageColdResult50LA sourceDamage;
 coldLoadDiagnostics.begin("roster_runtime");
 native_racer50HN::Bank racerBank;
 auto closeRcBankCollision=[&]() noexcept -> bool {try{
  if(rcCollisionCleanupDone)return true;
  if(racerBank.count()){
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(!racerBank.visit(slot,closeRcCurrentCollision))return false;
  }else if(!closeRcCurrentCollision())return false;
  rcCollisionCleanupDone=true;return true;
 }catch(...){return false;}};
 struct RcBankCollisionGuard {
  decltype(closeRcBankCollision)& close;
  ~RcBankCollisionGuard()noexcept{if(!close())std::terminate();}
 } rcBankCollisionGuard{closeRcBankCollision};
 auto closeScTriggerMemberships=[&]() noexcept -> bool {try{
  if(scMembershipCleanupDone)return true;
  if(racerBank.count()){
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(!racerBank.visit(slot,closeScCurrentMemberships))return false;
  }else if(!closeScCurrentMemberships())return false;
  scMembershipCleanupDone=true;return true;
 }catch(...){return false;}};
 auto closeTbBankCollision=[&]() noexcept -> bool {try{
  if(tbCollisionCleanupDone)return true;
  if(racerBank.count()){
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(!racerBank.visit(slot,closeTbCurrentCollision))return false;
  }else if(!closeTbCurrentCollision())return false;
  tbCollisionCleanupDone=true;return true;
 }catch(...){return false;}};
 struct TbBankCollisionGuard {
  decltype(closeTbBankCollision)& close;
  ~TbBankCollisionGuard()noexcept{if(!close())std::terminate();}
 } tbBankCollisionGuard{closeTbBankCollision};
 auto closeTcBankCollision=[&]() noexcept -> bool {try{
  if(tcCollisionCleanupDone)return true;
  if(racerBank.count()){
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(!racerBank.visit(slot,closeTcCurrentCollision))return false;
  }else if(!closeTcCurrentCollision())return false;
  tcCollisionCleanupDone=true;return true;
 }catch(...){return false;}};
 struct TcBankCollisionGuard {
  decltype(closeTcBankCollision)& close;
  ~TcBankCollisionGuard()noexcept{if(!close())std::terminate();}
 } tcBankCollisionGuard{closeTcBankCollision};
 // Destroyed after the later model retirement guard, before the actual Bank.
 struct ScBankMembershipGuard {
  decltype(closeScTriggerMemberships)& close;
  ~ScBankMembershipGuard()noexcept{if(!close())std::terminate();}
 } scBankMembershipGuard{closeScTriggerMemberships};
 // Canonical models borrow this Bank, while their shared RuntimeOwners was
 // allocated much earlier. Retire them before Bank destruction even if cold
 // construction throws before the later renderer/closeWindow try boundary.
 // RA keeps its separate Session/aggregate retirement obligation unchanged.
 native_racer50HN::BankBoundModelRetirement50LH<ra_cold_roster50LA::RuntimeOwners>
     nativeColdModelRetirement(*nativeRosterOwners,!isRaTrack);
 // This guard begins before any Bank enrollment and captures only owners
 // already declared. The later full-session guard retires contacts first and
 // marks this predecessor complete; an early failure has no live query yet.
 bool gbBankPrefixClosed=!isGbTrack;
 auto closeGbBankRegistries=[&]()noexcept->bool {try{
  if(!gbLightOwner||&gcLightOwner!=gbLightOwner.get())return false;
  if(gbParticleRender&&!gbParticleRender->closed()&&!gbParticleRender->close())return false;
  if(gbAnimationBinding&&gbAnimationBinding->needsClose()&&!gbAnimationBinding->close(gbAnimationBindingDiagnostic))return false;
  if(!nativeColdModelRetirement.close())return false;
  for(std::size_t slot=0;slot<racerBank.count();++slot)if(!racerBank.visit(slot,[&](){
   const bool visibilityRetired=racerRigs[slot]&&racerCables[slot]?
       gb_cold_visibility50GB::Retirement::retireSlot(racerBank,slot,s,x.bindings,x.parameters,
           *racerRigs[slot],*racerCables[slot],gcLightOwner,gcLightDiagnostic):
       gb_cold_visibility50GB::Retirement::untouchedSlot(racerBank,slot,gcLightOwner,gcLightDiagnostic);
   return visibilityRetired&&closeGbCurrentCollision();
  }))return false;
  if(!racerBank.count()&&!closeGbCurrentCollision())return false;
  gbCollisionCleanupDone=true;return true;
 }catch(...){return false;}};
 auto closeGbBankPrefix=[&]()noexcept->bool {try{
  if(gbBankPrefixClosed)return true;
  if(gbQueryBinding&&gbQueryBinding->enrolled())return false;
  if(!closeGbBankRegistries())return false;
  gbQueryBinding.reset();
  if(!racerBank.retireNativeBacking())return false;
  gbBankPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct GbBankPrefixGuard {
  decltype(closeGbBankPrefix)& close;
  ~GbBankPrefixGuard()noexcept{if(!close()){
   std::fputs("GB Bank prefix retirement rejected; backing retained until fail-stop\n",stderr);std::terminate();}}
 } gbBankPrefixGuard{closeGbBankPrefix};
 bool mcBankPrefixClosed=!isMcTrack;
 auto closeMcBankRegistries=[&]()noexcept->bool {try{
  if(!mcLightOwner||&gcLightOwner!=mcLightOwner.get())return false;
  if(mcParticleRender&&!mcParticleRender->closed()&&!mcParticleRender->close())return false;
  if(mcAnimationBinding&&mcAnimationBinding->needsClose()&&!mcAnimationBinding->close(mcAnimationBindingDiagnostic))return false;
  if(!nativeColdModelRetirement.close())return false;
  for(std::size_t slot=0;slot<racerBank.count();++slot)if(!racerBank.visit(slot,[&](){
   const bool visibilityRetired=racerRigs[slot]&&racerCables[slot]?
       mc_cold_visibility50MC::Retirement::retireSlot(racerBank,slot,s,x.bindings,x.parameters,
           *racerRigs[slot],*racerCables[slot],gcLightOwner,gcLightDiagnostic):
       mc_cold_visibility50MC::Retirement::untouchedSlot(racerBank,slot,gcLightOwner,gcLightDiagnostic);
   return visibilityRetired&&closeMcCurrentCollision();
  }))return false;
  if(!racerBank.count()&&!closeMcCurrentCollision())return false;
  mcCollisionCleanupDone=true;return true;
 }catch(...){return false;}};
 auto closeMcBankPrefix=[&]()noexcept->bool {try{
  if(mcBankPrefixClosed)return true;
  if(mcQueryBinding&&mcQueryBinding->enrolled())return false;
  if(!closeMcBankRegistries())return false;
  mcQueryBinding.reset();
  if(!racerBank.retireNativeBacking())return false;
  mcBankPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct McBankPrefixGuard {
  decltype(closeMcBankPrefix)& close;
  ~McBankPrefixGuard()noexcept{if(!close()){
   std::fputs("MC Bank prefix retirement rejected; backing retained until fail-stop\n",stderr);std::terminate();}}
 } mcBankPrefixGuard{closeMcBankPrefix};
 bool tbColdPrefixClosed=!isTbTrack;
 auto closeTbColdPrefix=[&]() noexcept -> bool {try{
  if(tbColdPrefixClosed)return true;
  if(!tbLightOwner||&gcLightOwner!=tbLightOwner.get())return false;
  // Failed door work holds the lighting static-busy prefix. Before any7F0
  // retirement, remove that exact registration while its backing still lives.
  // Here the early path has no GPU/camera; normal close has retired them above.
  if(tbDoorBinding&&tbDoorBinding->needsClose()&&!tbDoorBinding->ready()&&
     !tbDoorBinding->close(tbDoorBindingDiagnostic))return false;
  if(!nativeColdModelRetirement.close())return false;
  // Slot count remains zero until seal succeeds. No racer lighting prefix can
  // exist before the actual serial installation loop below.
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   if(!racerBank.visit(slot,[&](){
    const bool visibilityRetired=racerRigs[slot]&&racerCables[slot]?
        tb_cold_visibility50TB::Retirement::retireSlot(racerBank,slot,s,x.bindings,x.parameters,
            *racerRigs[slot],*racerCables[slot],gcLightOwner,gcLightDiagnostic):
        tb_cold_visibility50TB::Retirement::untouchedSlot(racerBank,slot,gcLightOwner,gcLightDiagnostic);
    return visibilityRetired&&closeTbCurrentCollision();
   }))return false;
  }
  if(!racerBank.count()&&!closeTbCurrentCollision())return false;
  tbCollisionCleanupDone=true;tbColdPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct TbColdPrefixGuard {
  decltype(closeTbColdPrefix)& close;bool armed=true;
  ~TbColdPrefixGuard()noexcept{if(armed&&!close()){
   std::fputs("TB cold racer retirement rejected; owner backing retained\n",stderr);
   std::terminate();
  }}
 } tbColdPrefixGuard{closeTbColdPrefix};
 bool tcColdPrefixClosed=!isTcTrack;
 auto closeTcColdPrefix=[&]() noexcept -> bool {try{
  if(tcColdPrefixClosed)return true;
  if(!tcLightOwner||&gcLightOwner!=tcLightOwner.get())return false;
  // Failed gate work holds the lighting static-busy prefix. Before any7F0
  // retirement, remove that exact registration while its backing still lives.
  // Here the early path has no GPU/camera; normal close has retired them above.
  if(tcGateVisibility&&tcGateVisibility->needsClose()&&!tcGateVisibility->ready()&&
     !tcGateVisibility->close(tcGateVisibilityDiagnostic))return false;
  if(!nativeColdModelRetirement.close())return false;
  // Slot count remains zero until seal succeeds. No racer lighting prefix can
  // exist before the actual serial installation loop below.
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   if(!racerBank.visit(slot,[&](){
    const bool visibilityRetired=racerRigs[slot]&&racerCables[slot]?
        tc_cold_visibility50TC::Retirement::retireSlot(racerBank,slot,s,x.bindings,x.parameters,
            *racerRigs[slot],*racerCables[slot],gcLightOwner,gcLightDiagnostic):
        tc_cold_visibility50TC::Retirement::untouchedSlot(racerBank,slot,gcLightOwner,gcLightDiagnostic);
    return visibilityRetired&&closeTcCurrentCollision();
   }))return false;
  }
  if(!racerBank.count()&&!closeTcCurrentCollision())return false;
  tcCollisionCleanupDone=true;tcColdPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct TcColdPrefixGuard {
  decltype(closeTcColdPrefix)& close;bool armed=true;
  ~TcColdPrefixGuard()noexcept{if(armed&&!close()){
   std::fputs("TC cold racer retirement rejected; owner backing retained\n",stderr);
   std::terminate();
  }}
 } tcColdPrefixGuard{closeTcColdPrefix};

 std::unique_ptr<ra_native_session50LA::Join> raSession;
 ra_native_session50LA::Diagnostic raSessionDiagnostic;
 // Non-owning frame view: retain references to the same locals, including
 // owner/callback variables that are populated later in this cold session.
 interactive_frame50DU::Context frameContext50DU{
  .originalAiAudioEnabled=originalAiAudioEnabled,
  .originalUiActive=originalUiActive,
  .nativeSelectedLights=nativeSelectedLights,
  .gcGraphicsSequence=gcGraphicsSequence,
  .finishDriverAppliedThisFrame=finishDriverAppliedThisFrame,
  .audioEnabled=audioEnabled,
  .opponentFrame=opponentFrame,
  .raceSetupFrame=raceSetupFrame,
  .lapContinuity=lapContinuity,
  .speechState=speechState,
  .x=x,
  .s=s,
  .m=m,
  .completedFrames=completedFrames,
  .originalAi=originalAi,
  .mcMapView=mcMapView,
  .mcAiView=mcAiView,
  .mcAiMetadata=mcAiMetadata,
  .gbMapView=gbMapView,
  .gbAiView=gbAiView,
  .gbAiMetadata=gbAiMetadata,
  .tbMapView=tbMapView,
  .tbAiView=tbAiView,
  .tbAiMetadata=tbAiMetadata,
  .tcMapView=tcMapView,
  .tcAiView=tcAiView,
  .tcAiMetadata=tcAiMetadata,
  .mbMapView=mbMapView,
  .mbAiView=mbAiView,
  .mbAiMetadata=mbAiMetadata,
  .liveWorld=liveWorld,
  .navigationMap=navigationMap,
  .navigationAI=navigationAI,
  .selectedOriginalAiMetadata=selectedOriginalAiMetadata,
  .originalAiTuning=originalAiTuning,
  .originalAiSession=originalAiSession,
  .raceRacerCount=raceRacerCount,
  .dynamicRegistry=dynamicRegistry,
  .mainMembership=mainMembership,
  .originalAiHumanCount=originalAiHumanCount,
  .humanCourseEnabled=humanCourseEnabled,
  .humanResultsCountdown=humanResultsCountdown,
  .originalAiSettings1125C=originalAiSettings1125C,
  .navigationProgress=navigationProgress,
  .navigationCA0=navigationCA0,
  .boostState=boostState,
  .originalAiReceipt=originalAiReceipt,
  .isMcTrack=isMcTrack,
  .isGbTrack=isGbTrack,
  .isTbTrack=isTbTrack,
  .isTcTrack=isTcTrack,
  .isMbTrack=isMbTrack,
  .originalRecoveryEnabled=originalRecoveryEnabled,
  .originalResetGround=originalResetGround,
  .racerCables=racerCables,
  .originalAiAudio=originalAiAudio,
  .zoneQueue=zoneQueue,
  .zoneLifecycle=zoneLifecycle,
  .zoneScratch=zoneScratch,
  .trackCounts=trackCounts,
  .selectedRuntimeHeightDomain=selectedRuntimeHeightDomain,
  .mcWaterOwner=mcWaterOwner,
  .terrainOwner=terrainOwner,
  .nativeGcWater=nativeGcWater,
  .nativeWaterRacers=nativeWaterRacers,
  .mcParticleVehicles=mcParticleVehicles,
  .gcLightOwner=gcLightOwner,
  .resetGlobals=resetGlobals,
  .resetLifecycle=resetLifecycle,
  .originalRecoveryModels=originalRecoveryModels,
  .resetEffectBindings=resetEffectBindings,
  .nativeEffects=nativeEffects,
  .nativeEffectCount=nativeEffectCount,
  .originalRecovery=originalRecovery,
  .originalRecoveryWorkspace=originalRecoveryWorkspace,
  .originalRecoveryDiagnostic=originalRecoveryDiagnostic,
  .actualHeight=actualHeight,
  .recoveryPaused=recoveryPaused,
  .playerEvents=playerEvents,
  .sourceBlur=sourceBlur,
  .eventContext=eventContext,
  .applicationEvents=applicationEvents,
  .playerEnvironment=playerEnvironment,
  .controller=controller,
  .lapOwner=lapOwner,
  .navigationDiagnostic=navigationDiagnostic,
  .nativeCalls=nativeCalls,
  .navigationUpdates=navigationUpdates,
  .navigationGateSkips=navigationGateSkips,
  .navigationCA0Copies=navigationCA0Copies,
  .navigationAISequential=navigationAISequential,
  .navigationMapSequential=navigationMapSequential,
  .navigationAIScan=navigationAIScan,
  .navigationMapScan=navigationMapScan,
  .humanResultsActive=humanResultsActive,
  .authoredFinishSteering10A4=authoredFinishSteering10A4,
  .authored=authored,
  .applicationWrongWayBypass34C=applicationWrongWayBypass34C,
  .wrongWayContinuation=wrongWayContinuation,
  .wrongWayReceipt=wrongWayReceipt,
  .wrongWayDiagnostic=wrongWayDiagnostic,
  .wrongWayUpdates=wrongWayUpdates,
  .wrongWayActiveTicks=wrongWayActiveTicks,
  .dynamicContactEnabled=dynamicContactEnabled,
  .dynamicProfile=dynamicProfile,
  .boostParameters=boostParameters,
  .opponentBoostAudio=opponentBoostAudio,
  .liveAudio=liveAudio,
  .boostDiagnostic=boostDiagnostic,
  .gcLightDiagnostic=gcLightDiagnostic,
  .dynamicConsumerFlags11050=dynamicConsumerFlags11050,
  .allAiEnabled=allAiEnabled,
  .playerSource5C=playerSource5C,
  .dynamicFinalTuning=dynamicFinalTuning,
  .dynamicTuning=dynamicTuning,
  .dynamicEvents=dynamicEvents,
  .dynamicActorResolver=dynamicActorResolver,
  .dynamicSelfSlot=dynamicSelfSlot,
  .dynamicEventBindings=dynamicEventBindings,
  .feedbackDevices=feedbackDevices,
  .feedbackHelpers=feedbackHelpers,
  .mode1Enabled=mode1Enabled,
  .mode1=mode1,
  .pendingSpeech=pendingSpeech,
  .speechEvents=speechEvents,
  .namesForEffects=namesForEffects,
  .effectOwnerIndex=effectOwnerIndex,
  .playerRacer60=playerRacer60,
  .effectsWorkspace=effectsWorkspace,
  .generatedEffects=generatedEffects,
  .spring=spring,
  .audioContactMaterials=audioContactMaterials,
  .terrainOwnerCount=terrainOwnerCount,
  .originalManualResetSerial=originalManualResetSerial,
  .originalManualResetGraphicsDue=originalManualResetGraphicsDue,
  .originalHumanRecovery=originalHumanRecovery,
  .originalManualRecoveryDiagnostic=originalManualRecoveryDiagnostic,
  .originalHumanRecoveryDiagnostic=originalHumanRecoveryDiagnostic,
  .humanBaseUndertrack=humanBaseUndertrack,
  .controllerActions=controllerActions,
  .effectCommands=effectCommands,
  .audioListenerReady=audioListenerReady,
  .audioListener=audioListener,
  .materialAudio=materialAudio,
  .membershipWork=membershipWork,
  .requireMbRuntimeNavigation50MB=interactive_frame50DU::BorrowedCall<void()>{requireMbRuntimeNavigation50MB},
  .refreshAnimatedCandidates=interactive_frame50DU::BorrowedCall<bool()>{refreshAnimatedCandidates},
  .recoveryQueries=interactive_frame50DU::BorrowedCall<rr::game::UndertrackQueryOwners50EW()>{recoveryQueries},
  .reportNavigation=interactive_frame50DU::BorrowedCall<bool(bool,const char*)>{reportNavigation},
  .selectedMapCount50MB=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount50MB},
  .selectedMapNode50MB=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode50MB},
  .refreshDynamicSelfActor=interactive_frame50DU::BorrowedCall<void()>{refreshDynamicSelfActor},
  .dynamicBoundary=interactive_frame50DU::BorrowedCall<void(std::uint32_t,const char*)>{dynamicBoundary},
  .speechInput=interactive_frame50DU::BorrowedCall<rr::game::DamageSpeechInput50HG()>{speechInput},
  .consumeEffects=interactive_frame50DU::BorrowedCall<bool(const rr::game::ImpactEffectEvent50B*,std::size_t)>{consumeEffects},
  .originalAiEnabled=originalAiEnabled,
  .isRaTrack=isRaTrack,
  .isRcTrack=isRcTrack,
  .checkpointRequested=checkpointRequested,
  .checkpointOutput=checkpointOutput,
  .ground=ground,
  .groundStack=groundStack,
  .nativeWaterApplication=nativeWaterApplication,
  .nativeFireSerial=nativeFireSerial,
  .sessionRandom=sessionRandom,
  .applicationClock=applicationClock,
  .nativeParticleAuthoring=nativeParticleAuthoring,
  .nativeParticleChildren=nativeParticleChildren,
  .nativeParticleChildSequence=nativeParticleChildSequence,
  .clock=clock,
  .baseTail=baseTail,
  .baseGlobals=baseGlobals,
  .baseWorkspace=baseWorkspace,
  .poseHistory=poseHistory,
  .coldFlame=coldFlame,
  .raWorld=raWorld,
  .raDiagnostic=raDiagnostic,
  .rcTriggerActions=rcTriggerActions,
  .rcTriggerAuthoring=rcTriggerAuthoring,
  .rcWorldBorrow=rcWorldBorrow,
  .tbTriggerActions=tbTriggerActions,
  .tbWorldBorrow=tbWorldBorrow,
  .tcTriggerActions=tcTriggerActions,
  .tcWorldBorrow=tcWorldBorrow,
  .scTriggerActions=scTriggerActions,
  .gaTriggerActions=gaTriggerActions,
  .gcTriggerActions=gcTriggerActions,
  .maTriggerActions=maTriggerActions,
  .saTriggerActions=saTriggerActions,
  .mcQueryBinding=mcQueryBinding,
  .gbQueryBinding=gbQueryBinding,
  .trackStorage=trackStorage,
  .secondaryMembership=secondaryMembership,
  .qfilter=qfilter,
  .query=query,
  .secondaryFilter=secondaryFilter,
  .secondaryQuery=secondaryQuery,
  .mainQueryRef=mainQueryRef,
  .secondaryQueryRef=secondaryQueryRef,
  .mainLiveQuery=mainLiveQuery,
  .secondaryLiveQuery=secondaryLiveQuery,
  .candidates=candidates,
  .initial=initial,
  .workspace=workspace,
  .destination=destination,
  .queryResult=queryResult,
  .records=records,
  .queryRecords=queryRecords,
  .effects=effects,
  .si=si,
  .sw=sw,
  .sd=sd,
  .secondaryResult=secondaryResult,
  .secondarySpring=secondarySpring,
  .secondaryRecords=secondaryRecords,
  .secondaryQueryRecords=secondaryQueryRecords,
  .sde=sde,
  .settings=settings,
  .response=response,
  .secondarySettings=secondarySettings,
  .secondaryResponse=secondaryResponse,
  .movementCandidates=movementCandidates,
  .movement=movement,
  .movementInitial=movementInitial,
  .movementDestination=movementDestination,
  .endpoints=endpoints,
  .endpointScenes=endpointScenes,
  .movementEffects=movementEffects,
  .movementSource=movementSource,
  .movementWorkspace=movementWorkspace,
  .movementOutput=movementOutput,
  .movementResult=movementResult,
  .movementFlag=movementFlag,
  .registeredMovement=registeredMovement,
  .registeredIndices=registeredIndices,
  .registeredCount=registeredCount,
  .probeIndices=probeIndices,
  .probeCount=probeCount,
  .terrainLiveOwner=terrainLiveOwner,
  .terrainContext=terrainContext,
  .recoveryQueryOwners=recoveryQueryOwners,
  .collisionNormalMode=collisionNormalMode,
  .dynamicPeers=dynamicPeers,
  .engineDamage=engineDamage,
  .engineDamageUpdates=engineDamageUpdates,
  .engineDamageDraws=engineDamageDraws,
  .engineDamageDips=engineDamageDips,
  .engineDamageReturns=engineDamageReturns,
  .engineDamageCompletions=engineDamageCompletions,
  .mainContactFrames=mainContactFrames,
  .secondaryContactFrames=secondaryContactFrames,
  .repairActiveTicks=repairActiveTicks,
  .terrainQueries=terrainQueries,
  .terrainHitFrames=terrainHitFrames,
  .terrainSupportedFrames=terrainSupportedFrames,
  .mainCrossedPortals=mainCrossedPortals,
  .secondaryCrossedPortals=secondaryCrossedPortals,
  .mbSourceTriggers=mbSourceTriggers,
  .mbCollisionWorld=mbCollisionWorld,
  .initialPairs=initialPairs,
  .initialMetadata=initialMetadata,
  .sip=sip,
  .sim=sim,
  .resetHumanCamera=resetHumanCamera,
  .collectRegistered=interactive_frame50DU::BorrowedCall<bool()>{collectRegistered},
  .gatherRegistered=interactive_frame50DU::BorrowedCall<bool()>{gatherRegistered},
  .commitCandidateEffects=interactive_frame50DU::BorrowedCall<bool(const std::vector<rr::game::TerrainCandidateEffects48J>&)>{commitCandidateEffects},
  .recoverPortalBoundary=interactive_frame50DU::BorrowedCall<bool(const rr::game::LiveZoneResult50EN&,const char*)>{recoverPortalBoundary},
  .acceptLiveQuery=interactive_frame50DU::BorrowedCall<bool(const rr::game::VehicleLiveQueryResult50EQ&,rr::game::Type2StaticQuery49X&,const rr::game::LiveZoneMembership50EN&,std::uint64_t&,const char*)>{acceptLiveQuery},
  .fulfillDynamicMusic=interactive_frame50DU::BorrowedCall<void()>{fulfillDynamicMusic},
  .racerBank=racerBank,
 };
 auto advanceFramePart=[&](std::size_t actualRacerSlot,NativeRacerFramePart part,
     float preparedOuterFactor,float liveSubstepScale){
  return frameContext50DU.advance(actualRacerSlot,part,preparedOuterFactor,liveSubstepScale);
 };

 auto advanceFrame=[&](std::size_t actualRacerSlot){
  return advanceFramePart(actualRacerSlot,NativeRacerFramePart::Children,.016f,60.f)&&
      advanceFramePart(actualRacerSlot,NativeRacerFramePart::Physics,.016f,60.f);
 };

 // Original racers have distinct immutable resource owners and share only the
 // serial execution arena, never tuning, poses, contacts or progression histories.

 // These stable source owners close through the SAME bank before it dies.
 // Ordinary host/other-course paths do not acquire this bounded TA lifecycle.
 auto& nativeRightModels=nativeRosterOwners->right;
 auto& nativeLeftModels=nativeRosterOwners->left;
 auto& nativeBottomRightModels=nativeRosterOwners->bottomRight;
 auto& nativeBottomLeftModels=nativeRosterOwners->bottomLeft;
 // Declaration order also preserves C/BL/BR/L/R release during an exception
 // before the explicit Session/window cleanup has been installed. The lower
 // arrays stay empty for the ordinary three-model roster members.
 auto& nativeChariotModels=nativeRosterOwners->chariot;
 std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> nativeModelDomain;
 // Course can reuse this exact cold roster after leaving the frontend.
 // Construct its owners now at the real Rig.install, including UI-active
 // startup; never replay construction over the installed graph at Course.
 if((isTaTrack||isSaTrack||isRaTrack||directOriginalHuman)&&originalCameraEnabled&&originalMainApplication.enabled){
  originalMainApplication.appStart(originalMainApplication.assetRoot);
  nativeModelDomain=originalMainApplication.standardMaterialDomain();
 }
 native_racer50IZ::FrameReads racerFrameReads;
 native_ai50IV::ObservationReads originalAiReads;
 auto projectDynamicActorsForSlot=[&](std::size_t slot){
  dynamicSelfSlot=slot;
  for(std::size_t actor=0;actor<racerBank.count();++actor){
   dynamicActors.values[actor]={racerBank.read(actor,racerFrameReads.vehicle3C),
       racerBank.read(actor,racerFrameReads.vehicle58),dynamicRacer60[actor],
       racerBank.read(actor,originalAiReads.lapCC8),
       static_cast<std::uint32_t>(originalAiSession.rank[actor].placeCCC),
       racerBank.read(actor,racerFrameReads.flagsCD8)};
   dynamicActors.bound[actor]=true;
  }
 };
 auto captureDynamicBeforeFrame=[&](std::size_t slot){
  if(!dynamicContactEnabled)return;
  native_dynamic_contact50IW::Diagnostic diagnostic{};
  if(!dynamicRegistry.captureBeforeFrame(racerBank,racerFrameReads,slot,dynamicPeers,diagnostic))
   dynamicBoundary(diagnostic.sourcePC,"every dynamic peer must be active and Ready");
  projectDynamicActorsForSlot(slot);
 };
 // Resolve the event callback receiver, not the camera/followed slot. The
 // bank lease ends before dispatch; only actual scalar inputs escape it. D48
 // state/PCM remain in their stable native per-racer owners outside snapshots.
 auto resolveDynamicSpeech=[&](RaceActorId50ID actor,DamageSpeechInput50HG& input,
     const PodSpeechCounts50HG*& counts,PodSpeechState50HG*& state){
  counts=nullptr;state=nullptr;RaceActorSnapshot50ID identity{};
  if(!actor||actor>racerBank.count()||!dynamicActorResolver.read||
     !dynamicActorResolver.read(dynamicActorResolver.context,actor,identity))return false;
  const auto slot=static_cast<std::size_t>(actor-1u);
  if(originalAiAudioEnabled&&originalAiOwnsSlot(slot)){
   if(!originalAiAudio->speechOwner(slot,identity.vehicle60,counts,state))return false;
  }else{
   if(actor!=1u)return false;
   counts=&speechCounts;state=&speechState;
  }
  input=racerBank.visit(slot,[&](){
   auto live=speechInput();live.vehicle60=identity.vehicle60;live.vehicle60Known=true;return live;
  });
  return input.vehicle3C==identity.vehicle3C&&input.vehicle58==identity.vehicle58;
 };
 auto applyDynamicSpeech=[&](RaceActorId50ID actor,const AudioCommand50HE& command){
  if(!liveAudio->outputOpen())return false;
  if(!originalAiAudioEnabled||(humanCourseEnabled&&actor==1u))return actor==1u&&liveAudio->applySourceCommand(command).okay();
  RaceActorSnapshot50ID identity{};
  return actor&&actor<=racerBank.count()&&dynamicActorResolver.read&&
      dynamicActorResolver.read(dynamicActorResolver.context,actor,identity)&&
      originalAiAudio->applySpeech(static_cast<std::size_t>(actor-1u),identity.vehicle60,command);
 };
 auto captureDynamicBeforeScheduledPhysics=[&](const original_all8_frame::Schedule& schedule,std::size_t slot){
  if(!dynamicContactEnabled)return;
  native_dynamic_contact50IW::Diagnostic diagnostic{};
  if(!dynamicRegistry.captureBeforeScheduledPhysics(schedule,racerBank,racerFrameReads,slot,dynamicPeers,diagnostic))
   dynamicBoundary(diagnostic.sourcePC,"dynamic peers missed the exact two-phase physics cut");
  projectDynamicActorsForSlot(slot);
 };
 auto requireOriginalAiFleetAtBoundary=[&](const original_all8_frame::Schedule* schedule){
  if(isRcTrack)requireMbRuntimeNavigation50MB();
  // Every actual racer is an input to the joined source manager. A failed
  // native frame is retained by Bank::visit, but is not a manager observation.
  // Check the entire fleet before any observation, timing or rank mutation;
  // never synthesize Ready or silently remove a failed racer from the field.
  const bool scheduledPattern=schedule&&
      (schedule->admitsChildrenObservation(racerBank,racerFrameReads,schedule->nextBankSlot())||
       schedule->admitsMiddleObservation(racerBank,racerFrameReads));
  if(schedule)require50DU(scheduledPattern,"AI observation missed its exact two-phase child boundary");
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   const bool active=racerBank.active(slot);
   bool selectedAiMetadataReady=true;
   if(originalAiOwnsSlot(slot)){
    const auto sourceRacer60=racerBank.read(slot,originalAiReads.racer60);
    const auto expectedRecordMatch=sourceRacer60==static_cast<std::uint32_t>(
        originalAiCreateSelection.freshCourseRecordPod11280)?1u:0u;
    selectedAiMetadataReady=originalAiCreateSelection.bound&&
        racerBank.read(slot,originalAiReads.fieldFE4)==expectedRecordMatch&&
        (originalAiCreateSelection.actualRaceClass!=AuthoredClass49N::A||
         racerBank.read(slot,originalAiReads.repairTime1064)==2.f);
   }
   if(active&&(schedule?scheduledPattern:racerBank.read(slot,racerFrameReads.phase)==VehicleFramePhase49W::Ready)&&
      (!originalAiOwnsSlot(slot)||(racerBank.read(slot,racerFrameReads.vehicle58)==1u&&
         racerBank.read(slot,racerFrameReads.aiOwnersBound)&&
         racerBank.read(slot,originalAiReads.initialized)&&
         racerBank.read(slot,originalAiReads.managerIndexFE0)==slot&&
         selectedAiMetadataReady))&&
      (!humanCourseEnabled||slot!=0||(racerBank.read(slot,racerFrameReads.vehicle58)==0u&&
         !racerBank.read(slot,racerFrameReads.aiOwnersBound))))continue;
   std::cerr<<"ORIGINAL AI world boundary: slot="<<slot<<" active="<<active
       <<" phase="<<static_cast<unsigned>(racerBank.read(slot,racerFrameReads.phase))
       <<" vehicle58="<<racerBank.read(slot,racerFrameReads.vehicle58)
       <<" ai_source_boundary="<<std::hex<<racerBank.read(slot,originalAiReads.sourceBoundary)
       <<" frame_source_stage="<<racerBank.read(slot,originalAiReads.frameSourceStage)<<std::dec
       <<"; native partial state retained; manager was not advanced.\n";
   require50DU(false,"original AI requires every actual racer active and Ready");
  }
 };
 auto requireOriginalAiFleetReady=[&](){requireOriginalAiFleetAtBoundary(nullptr);};
 auto observeOriginalAiFleet=[&](const original_all8_frame::Schedule* schedule=nullptr){
  requireOriginalAiFleetAtBoundary(schedule);
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   if(isMcTrack)originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,mcMapView,mcMapMetadata);
   else if(isGbTrack)originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,gbMapView,gbMapMetadata);
   else if(isTbTrack)originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,tbMapView,tbMapMetadata);
   else if(isTcTrack)originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,tcMapView,tcMapMetadata);
   else if(isMbTrack)originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,mbMapView,mbMapMetadata);
   else originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,*navigationMap,selectedOriginalAiMapMetadata);
  }
 };
 // Manager passes only consume their current arena and stable shared owners.
 // Preserve slot order and the boundaries between passes while avoiding the
 // whole-racer snapshots. No callback may borrow another
 // slot or retain an arena reference after it returns; the field lease restores on throw.
 original_manager_fields::Access managerFields;
 bool managerFieldsReported=false;
 auto visitManagerRacers=[&](auto&& operation){
  managerFields.visitAll(racerBank,s,lapContinuity,std::forward<decltype(operation)>(operation));
  if(!managerFieldsReported){
   std::fprintf(stderr,"native_manager_fields bytes_per_racer=%zu full_snapshot_bytes=%zu slots=%zu\n",
       managerFields.bytesPerRacer(),racerBank.bytesPerRacer(),racerBank.count());
   managerFieldsReported=true;
  }
 };
 auto rankOriginalAiFleet=[&](const original_all8_frame::Schedule* schedule=nullptr){
  observeOriginalAiFleet(schedule);
  require50DU(native_ai50IV::rank(originalAiSession,racerBank.count()),"original AI source rank/remaining boundary");
  visitManagerRacers([&](std::size_t slot){
   //1B0700 is a write to the same canonical racer+C90+44 used by camera,
   //manager and later AI observations. A transient rank scratch is no owner.
   lapContinuity.timing.wordCD4=originalAiSession.rank[slot].retirementCD4;
  });
 };
 auto updateOriginalAiPacking=[&](){
  //1A15EC belongs to the human update. With no humans it is never called;
  // every actual AI still executes171C60 and its authentic FF8<0 return.
  if(allAiEnabled)return;
  // Original1A15EC follows the sole human's navigation. The serial arena
  // transports its source writes before any AI frame, without nested visits.
  if((s.fields.applicationByte58&3u)!=2u)return;
  AiPackingRacerObservation50IU rows[aiRaceRankCapacity50IV]{};
  for(std::size_t slot=0;slot<racerBank.count();++slot){const auto&o=originalAiSession.observed[slot];
   rows[slot]={o.vehicle58,o.flagsCD8,originalAiSession.rank[slot].placeCCC,
       o.vehicle58==1u?&originalAiSession.packingReferences[slot]:nullptr};
  }
  AiPackingReferenceDiagnostic50IU diagnostic{};
  require50DU(updateAiPackingReferences50IU(0,originalAiSession.rank[0].placeCCC,1,
      originalAiTuning,{},rows,racerBank.count(),diagnostic),"original AI packing-reference boundary");
  for(std::size_t slot=0;slot<racerBank.count();++slot)if(originalAiOwnsSlot(slot))
   racerBank.visit(slot,[&](){originalAi.referenceFF8=originalAiSession.packingReferences[slot];});
 };
 auto reportOriginalAiFleet=[&](const char* reason){
  if(!originalAiEnabled)return;
  std::cerr<<"original_ai_selection track="<<trackSession->world->code
      <<" track_id1125F="<<static_cast<unsigned>(originalAiCreateSelection.selectedTrackId1125F)
      <<" course_order187B10="<<originalAiCreateSelection.selectedOrder187B10
      <<" create_class="<<static_cast<unsigned>(originalAiCreateSelection.actualRaceClass)
      <<" course_record_pod11280="<<originalAiCreateSelection.freshCourseRecordPod11280
      <<" fresh_profile="<<originalAiCreateSelection.bound<<" loaded_save=0 reason="<<reason<<'\n';
  std::cerr<<"original_ai_all enabled="<<allAiEnabled<<" racer_count="<<racerBank.count()
      <<" human_count="<<originalAiHumanCount<<" ai_count="<<(nativeOriginalFleet?originalAiOpponentCount:1u)
      <<" camera_slot=0 camera_racer60="<<playerRacer60
      <<" host_driver_calls="<<allAiHostDriverCalls<<" controller_samples="<<allAiControllerSamples
      <<" keyboard_packets="<<allAiKeyboardPackets<<" feedback_helpers="<<feedbackHelpers
      <<" feedback_submissions="<<feedbackSubmissions<<" reason="<<reason<<'\n';
  std::cerr<<"original_ai_manager flags58="<<static_cast<unsigned>(originalAiSession.manager.flags58)
      <<" time30="<<originalAiSession.manager.time30<<" track_length38="<<originalAiSession.trackLength38
      <<" finish_count54="<<originalAiSession.finished44.count54
      <<" active_count50="<<originalAiSession.retired40.activeCount50
      <<" all8_finish_awaiting_display="<<originalAiSession.all8FinishAwaitingDisplay
      <<" all8_results_complete="<<originalAiSession.all8ResultsComplete
      <<" end_policy="<<(allAiEnabled?"all_actual_source_finishes":"retained_human_client")<<'\n';
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   if(!originalAiOwnsSlot(slot))continue;
   const bool active=racerBank.active(slot);
   racerBank.visit(slot,[&](){
    const auto& fields=originalAiSession.fields[originalAi.managerIndexFE0];
    std::cerr<<"original_ai slot="<<slot<<" source_index="<<originalAi.managerIndexFE0
        <<" racer60="<<originalAi.racer60<<" vehicle58="<<s.fields.vehicle58
        <<" record_matchFE4="<<originalAi.fieldFE4<<" repair_time1064="<<originalAi.profile.repairTime1064
        <<" initialized="<<originalAi.initialized<<" owners_bound="<<s.fields.nativeAiOwnersBound50IV
        <<" active="<<active<<" phase="<<static_cast<unsigned>(s.phase)
        <<" native_ticks="<<completedFrames<<" dispatched="<<originalAi.dispatchedFrames
        <<" state="<<originalAi.state.aiStateFE8<<" CB8="<<lapContinuity.timing.timeCB8
        <<" stall1090="<<originalAi.state.stallTimer1090
        <<" wrong_way1094="<<originalAi.state.wrongWayTimer1094
        <<" orientation10A0="<<originalAi.state.orientation10A0
        <<" startup="<<originalAi.startupFrames<<" ordinary="<<originalAi.ordinaryFrames
        <<" combat_scans="<<originalAi.combatScans<<" combat_ai_rejections="<<originalAi.combatAiRejections
        <<" target1008="<<(originalAi.target1008Present?static_cast<std::int64_t>(originalAi.target1008ManagerIndex):-1)
        <<" brake_commands="<<originalAi.brakingFrames<<" bound_brake_steer="<<originalAi.boundBrakeSteerFrames
        <<" cached_speed34="<<m.cachedSpeed34
        <<" engineDrive="<<m.components.bindings.engineDrive[0]<<'/'<<m.components.bindings.engineDrive[1]
        <<" brakeDragFc="<<x.parameters.bodies.main.aero.brakeDragFc
        <<" impact_flagsCD8="<<static_cast<unsigned>(s.fields.impactFlagsCD8)
        <<" steer="<<m.controls.player.raw.values[0]<<" aeroBrakeEC="<<m.components.bindings.aeroBrakeEC
        <<" aeroDrive="<<m.components.bindings.aeroDrive[0]<<'/'<<m.components.bindings.aeroDrive[1]
        <<" target="<<originalAiReceipt.desiredSpeed
        <<" B7C/B80="<<m.controls.player.actionB7C<<'/'<<m.controls.player.actionB80
        <<" B94="<<x.parameters.rates.x<<" C80="<<navigationProgress.reset.vehicleC80
        <<" CC4="<<fields.remainingCC4<<" CC8="<<lapContinuity.state.vehicleCC8
        <<" CCC="<<fields.placeCCC
        <<" FF8="<<originalAi.referenceFF8<<" FFC="<<originalAi.packing.scaleFFC
        <<" 1038/103C="<<originalAi.lists.counts[0]<<'/'<<originalAi.lists.counts[1]
        <<" cooldown109C="<<originalAi.boost.cooldown109C
        <<" route_selections="<<originalAi.navigation.routeSelections
        <<" route_rng_draws="<<originalAi.navigation.routeRandomDraws
        <<" route1080/1084="<<originalAi.navigation.choice1080<<'/'<<originalAi.navigation.choice1084
        <<" source_boundary="<<std::hex<<originalAiReceipt.sourceBoundary<<std::dec<<'\n';
   });
  }
 };
 auto advanceOriginalAiManager=[&](float factor,const original_all8_frame::Schedule* schedule=nullptr){
  // Arm only after the actual source manager has started, before its first
  // genuine timing pass. Never arm from a UI proposal, HOST finish or fixture.
  if(humanCourseEnabled&&originalUiStarted&&originalMainApplication.profile&&
     originalMainApplication.profile->mode34==2&&!tournamentResultArmed&&!tournamentResultCommitted&&
     (originalAiSession.manager.flags58&3u)==2u){
   auto& profile=*originalMainApplication.riderProfile;
   require50DU(profile.courseRaceSnapshot(tournamentLaunchSelection),"tournament launch selection unavailable");
   tournamentLaunchStamp=profile.stamp();
   require50DU(profile.armTournamentResultForCommittedRace(tournamentLaunchSelection,tournamentLaunchStamp,
       originalAiSession.manager,*originalMainApplication.state),"genuine tournament launch arming rejected");
   tournamentResultArmed=true;
  }
  // Genuine1AF9C0 clock and the complete1AFF50 list/timing order. Root
  //placement remains the declared native ALL8 schedule. No HOST race clock,
  //distance interpolation, rank or finish bit feeds this source manager.
  // beginLapManagerClock mutates shared time, so validate before that call.
  requireOriginalAiFleetAtBoundary(schedule);
  const auto managerClock=beginLapManagerClock50ID(originalAiSession.manager,factor,false);
  if(humanCourseEnabled&&humanResultsActive){
   require50DU(managerClock==LapManagerClockStatus50ID::OtherManagerPath&&
       (originalAiSession.manager.flags58&1u),"ended human source manager must not run another timing/rank pass");
   return;
  }
  require50DU(managerClock==LapManagerClockStatus50ID::NeedRacerPass,"original AI manager clock boundary");
  visitManagerRacers([&](std::size_t slot){
   LapTimingInput50ID input{factor,dynamicContactEnabled?dynamicMusicProfile.mode11258:2u,s.fields.vehicle58,static_cast<std::uint32_t>(slot),
       lapContinuity.state.vehicleCC8,s.fields.impactFlagsCD8};
   auto result=beginRacerLapTiming50ID(lapContinuity.timing,originalAiSession.manager,input,lapContinuity.timingCursor);
   if(result.status==LapTimingStatus50ID::NeedLapEvent){
    struct LiveLapActor {RaceActorId50ID identity;RaceActorSnapshot50ID snapshot;};
    LiveLapActor actor{static_cast<RaceActorId50ID>(slot+1u),{s.fields.vehicle3C,s.fields.vehicle58,
        dynamicRacer60[slot],lapContinuity.state.vehicleCC8,
        static_cast<std::uint32_t>(originalAiSession.rank[slot].placeCCC),s.fields.impactFlagsCD8}};
    const RaceActorResolver50ID resolver{&actor,[](void* opaque,RaceActorId50ID identity,RaceActorSnapshot50ID& value)noexcept{
     const auto& live=*static_cast<const LiveLapActor*>(opaque);
     if(identity!=live.identity)return false;value=live.snapshot;return true;
    }};
    require50DU(dynamicContactEnabled&&appendRaceEvent50ID(dynamicEvents,
        {0,6,0,actor.identity,0},resolver).helperReturned(),"original1A39B0/1A64B0 lap queue helper pending");
    //Only queue construction/append has returned here. The common FIFO's
    //event6 subscriber runs later using live post-rank/post-finish actors.
    result=resumeRacerLapTimingAfterEvent50ID(lapContinuity.timing,originalAiSession.manager,
        lapContinuity.timingCursor,result.request.sequence);
   }
   require50DU(result.status==LapTimingStatus50ID::Complete,"original AI timing continuation boundary");
  });
  rankOriginalAiFleet(schedule);
  visitManagerRacers([&](std::size_t slot){
   if((s.fields.impactFlagsCD8&1u)&&(originalAiSession.manager.flags58&0x20u))return;
   if(!(s.fields.impactFlagsCD8&2u))return;
   require50DU(finishOriginalAiManagerRow(originalAiSession.manager.flags58,originalAiSettings1125C,
       static_cast<std::uint32_t>(slot),lapContinuity.state.vehicleCC8,s.fields.impactFlagsCD8,
       lapContinuity.timing,originalAiSession.finished44)==OriginalAiFinishStatus::Complete,
       "original1B02F0 finish list/count/time continuation pending");
   s.fields.impactFlagsCD8=static_cast<std::uint8_t>(s.fields.impactFlagsCD8&~2u); //1B02A8
   lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
  });
  std::array<OriginalAiGapRow,aiRaceRankCapacity50IV> rows{};
  std::array<float,aiRaceRankCapacity50IV> gaps{};
  visitManagerRacers([&](std::size_t slot){
   rows[slot]={originalAiSession.rank[slot].placeCCC,static_cast<std::int32_t>(lapContinuity.state.vehicleCC8),
       s.fields.impactFlagsCD8,originalAiSession.rank[slot].remainingCC4,lapContinuity.timing.timeCB8,
       lapContinuity.timing.valueCBC};
  });
  require50DU(computeOriginalAiManagerGaps(originalAiSession.trackLength38,rows.data(),racerBank.count(),
      gaps.data())==OriginalAiFinishStatus::Complete,"original1B0920 gap continuation pending");
  //1B0CB0's exact zero-human path only sets manager bit6. The separately
  //declared ALL8 outer-end policy is applied after completed physics below;
  //retail1B0A70 would otherwise end a zero-human field on its first frame.
  if(allAiEnabled)originalAiSession.manager.flags58=static_cast<std::uint8_t>(originalAiSession.manager.flags58|0x40u);
  std::array<OriginalHumanEndRow50KG,8> humanEndRows{};
  visitManagerRacers([&](std::size_t slot){
   lapContinuity.timing.valueCBC=gaps[slot];
   if(humanCourseEnabled)humanEndRows[slot]={s.fields.vehicle58,lapContinuity.timing.wordCD4,
       s.fields.impactFlagsCD8,originalAiSession.rank[slot].placeCCC,lapContinuity.timing.valueCBC};
  });
  if(humanCourseEnabled){
   const auto ended=finishOriginalHumanManagerPass50KG(originalAiSession.manager,humanEndRows.data(),
       racerBank.count(),originalAiHumanCount,originalAiTuning,humanResultsCountdown);
   require50DU(ended.status==OriginalHumanEndStatus50KG::complete,"actual human source manager ending rejected");
   humanResultsActive=ended.ended;
  }
  visitManagerRacers([&](std::size_t slot){
   s.fields.applicationByte58=originalAiSession.manager.flags58;
  });
 };
 // Native ALL8 root schedule. No old whole-frame/player-first loop may also
 // run for this frame. The operation invokes recovered scene/controller and
 // actual native world/timing services between real racer prefixes/physics.
 // It does not claim the absent retail HUD/audio child list was traversed.
 // Audio ownership is explicit: ALL8 can use genuine AI D40 at the native
 // audio phase. Full original application/child RNG chronology is unclaimed.

 race_session50HM::Session legacyRace{};
 race_session50HM::MbSession mbRace{};
 race_session50HM::TcSession tcRace{};
 race_session50HM::GbSession gbRace{};
 race_session50HM::McSession mcRace{};
 race_session50HM::TbSession tbRace{};
 auto& race=isMcTrack?static_cast<race_session50HM::SessionRows&>(mcRace):isGbTrack?static_cast<race_session50HM::SessionRows&>(gbRace):isTbTrack?static_cast<race_session50HM::SessionRows&>(tbRace):isTcTrack?static_cast<race_session50HM::SessionRows&>(tcRace):isMbTrack?static_cast<race_session50HM::SessionRows&>(mbRace):static_cast<race_session50HM::SessionRows&>(legacyRace);
 const race_session50HM::SessionBorrow raceBorrow=isMcTrack?race_session50HM::SessionBorrow(mcRace):isGbTrack?race_session50HM::SessionBorrow(gbRace):isTbTrack?race_session50HM::SessionBorrow(tbRace):isTcTrack?race_session50HM::SessionBorrow(tcRace):isMbTrack?race_session50HM::SessionBorrow(mbRace):race_session50HM::SessionBorrow(legacyRace);
 const auto withRace=[&](auto&& operation)->decltype(auto){
  if(isRcTrack)requireMbRuntimeNavigation50MB();
  if(isMcTrack){requireMbRuntimeNavigation50MB();return operation(mcRace);}
  if(isGbTrack){requireMbRuntimeNavigation50MB();return operation(gbRace);}
  if(isTbTrack){requireMbRuntimeNavigation50MB();return operation(tbRace);}
  if(isTcTrack)return operation(tcRace);
  if(isMbTrack)return operation(mbRace);return operation(legacyRace);
 };
 // One cold-owned contact world, outside every racer snapshot. Geometry and
 // scratch never borrow an opponent arena pointer across a bank visit.
 std::unique_ptr<native_racer_contact50HW::Owner> racerContactOwner;
 std::array<std::uint32_t,native_racer_contact50HW::maxRacers> racerContactIds{};
 std::array<std::uint32_t,native_racer_contact50HW::maxRacers> racerContactMaterials{};
 RacerContactTotals50HV racerContactTotals;
 const auto reportRacerContacts=[&](){
  std::cerr<<"racer_contacts enabled="<<racerContactsEnabled
      <<" passes="<<racerContactTotals.passes<<" no_pairs="<<racerContactTotals.noPairs
      <<" tested_pairs="<<racerContactTotals.testedPairs<<" geometry_pairs="<<racerContactTotals.geometryPairs
      <<" suppressed_pairs="<<racerContactTotals.suppressedPairs<<" responded_pairs="<<racerContactTotals.respondedPairs
      <<" generated_records="<<racerContactTotals.generatedRecords<<" active_records="<<racerContactTotals.activeRecords
      <<" failures="<<racerContactTotals.failures<<" last_status="<<static_cast<unsigned>(racerContactTotals.lastStatus)<<'\n';
 };
 std::array<native_racer50HN::DrawOwner,maxHostRaceOpponents50HT> opponentDraw;
 std::array<std::vector<platform::VehicleWindowMaterialUv50DS>,maxHostRaceOpponents50HT> opponentUvs;
 EngineFlareOutput50GV opponentFlames[maxHostRaceOpponents50HT]{};
 bool opponentFlamePrepared[maxHostRaceOpponents50HT]{};
 // Last accepted render transaction, separate from the full race/standings
 // field. These host presentation counters never change a native racer.
 RacePresentation50HZ acceptedRacePresentation;
 auto observeLiveRacer=[&](){
  race_session50HM::Observation observed{};
  observed.completedLaps=lapContinuity.state.vehicleCC8;
  observed.checkpointCursor=lapContinuity.state.vehicleFF8;
  observed.previousMapNode=navigationProgress.reset.vehicleC8C;
  observed.currentMapNode=navigationProgress.reset.vehicleC88;
  for(unsigned axis=0;axis<3;++axis)observed.position[axis]=m.components.root.body.position.v[axis];
  observed.wrongWay=(lapContinuity.prefix.flags10CC&2u)!=0;
  observed.lapCallbackPending=lapOwner.pending.pending.requested;
  return observed;
 };
 // Publish trivial engine inputs during the existing race observation visits;
 // no extra full-state bank visits, ownership copies, or per-mix allocations.
 std::array<opponent_audio50HX::Input,opponent_audio50HX::maxOpponents> opponentEngineInputs{};
 std::array<std::uint64_t,maxHostRaceRacers50HT> observedNativeTicks{};
 std::array<std::uint8_t,maxHostRaceRacers50HT> observedNativeFinishFlags{};
 auto sampleRaceField=[&](){
  observedNativeTicks[0]=completedFrames;
  observedNativeFinishFlags[0]=s.fields.impactFlagsCD8;
  require50DU(withRace([&](auto& actualRace){return race_session50HM::sampleRacer(actualRace,0,observeLiveRacer());}),"player race observation rejected");
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   // An abandoned partial frame cannot publish progression or display geometry.
   // Keep the last completed observation for a visibly stopped opponent.
   opponentEngineInputs[racer-1]={};
   const auto observed=racerBank.active(racer)?racerBank.visit(racer,[&](){
    observedNativeTicks[racer]=completedFrames;
    observedNativeFinishFlags[racer]=s.fields.impactFlagsCD8;
    if(audioEnabled){
     require50DU(s.phase==VehicleFramePhase49W::Ready,"opponent audio needs a completed native frame");
     auto& input=opponentEngineInputs[racer-1];const auto& p=m.components.root.body.position;
     input.active=true;input.position={p.v[0],p.v[1],p.v[2]};
     input.speedRatio70=boostState.speedRatio70;
     // Preserve the source boost decision made before this racer's native heat
     // update. Damage-dip modulation remains unjoined; player state is never borrowed.
     input.enginePitchScale484=1.f;input.boostPitch=opponentBoostAudio.source.pitchBoost();
     input.boostCommand=opponentBoostAudio.command;input.boostSerial=opponentBoostAudio.serial;
    }
    return observeLiveRacer();
   }):race.racers[racer].live;
   require50DU(withRace([&](auto& actualRace){return race_session50HM::sampleRacer(actualRace,static_cast<std::uint32_t>(racer),observed);}),
       "opponent race observation rejected");
  }
 };
 // One-way presentation adapter. The existing window/audio/menu carrier can
 //display native manager data; it cannot rank, interpolate a finish, write a
 //source flag, or feed its clock/distance back to any original AI owner.
 auto publishOriginalAiStandings=[&](){
  require50DU(nativeOriginalFleet&&originalAiSession.managerBound&&originalAiSession.rankBound&&
      race.racerCount==racerBank.count()&&race.racerCount==raceRacerCount,
      "native presentation requires the complete actual source manager");
  std::array<std::uint32_t,aiRaceRankCapacity50IV> finishPlaces{};
  for(std::uint32_t n=0;n<originalAiSession.finished44.count54;++n){
   const auto slot=originalAiSession.finished44.managerRows[n];
   require50DU(slot<race.racerCount&&!finishPlaces[slot],"source finish list identity is invalid");
   finishPlaces[slot]=n+1;
  }
  for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
   auto& row=race.racers[slot];
   require50DU(row.sampled&&s.fields.vehicle58==(humanCourseEnabled&&slot==0?0u:1u)&&
       static_cast<bool>(s.fields.impactFlagsCD8&1u)==static_cast<bool>(finishPlaces[slot]),
       "native presentation cannot invent or discard a source finish");
   row.live=row.pending;row.observed=row.pending;row.observedValid=true;row.sampled=false;
   row.place=static_cast<std::uint32_t>(originalAiSession.rank[slot].placeCCC);
   row.remainingDistance=originalAiSession.rank[slot].remainingCC4;
   row.finished=finishPlaces[slot]!=0;row.finishPlace=finishPlaces[slot];
   require50DU(lapContinuity.timing.wordCD4==originalAiSession.rank[slot].retirementCD4,
       "native presentation requires the actual committed retirement marker");
   row.retirementCD4=lapContinuity.timing.wordCD4;
   if(row.finished)row.finishSeconds=lapContinuity.timing.timeCB8;
   race.ordered[slot]=static_cast<std::uint32_t>(slot);
  });
  race.raceSeconds=originalAiSession.manager.time30;race.finishCount=originalAiSession.finished44.count54;
  //Presentation ordering only. CCC values are preserved, including any source
  //ties; there is no distance tie-break or new rank written into a native row.
  for(std::uint32_t i=1;i<race.racerCount;++i){const auto slot=race.ordered[i];auto j=i;
   while(j&&race.racers[slot].place<race.racers[race.ordered[j-1]].place){
    race.ordered[j]=race.ordered[j-1];--j;
   }
   race.ordered[j]=slot;
  }
  if(allAiEnabled&&race.finishCount==race.racerCount){
   //Explicit native ALL8 outer-end policy:1B0A70's retail human-only predicate
   //would close at frame1 with no humans. Here closure requires EVERY real
   //1B02F0 list entry/flag, after the actual final camera and physics frame.
   //The final native scene is retained for Results; no unsupported retail
   //state6 driver or positive-time ending-camera call is claimed.
   for(std::uint32_t n=0;n<race.racerCount;++n){
    const auto slot=originalAiSession.finished44.managerRows[n];
    race.ordered[n]=slot;race.racers[slot].place=n+1; //Display actual list44 ordinal.
   }
   //Do not end the camera's manager or offer Results before this final real
   //scene reaches actual GPU/display completion. The root holds simulation
   //while awaiting that receipt, then seals the presentation below.
   originalAiSession.all8FinishAwaitingDisplay=true;
   std::cerr<<"ORIGINAL ALL8 FINISH: source finish list sealed "<<race.finishCount
       <<" racers; awaiting actual final-scene display before Results.\n";
  }
  if(humanCourseEnabled&&humanResultsActive){
   // One-way view of the genuine human1B0A70 ending. Source CCC/list44/time30
   // already sealed; no HOST finish interpolation, rank, or source flag write.
   race.resultsClosed=true;race.phase=race_session50HM::Phase::Complete;
  }
  race.error=race_session50HM::Error::None;
 };
 // Bound below once the camera session local exists (declared later for its
 // teardown order); null until then, which keeps the body listener fallback.
 const std::shared_ptr<original_camera_session50KX::Session>* audioListenerCamera50DU=nullptr;
 auto updateOpponentAudio=[&](){
  if(!audioEnabled||!raceEnabled)return;
  if(originalAiAudioEnabled){
   // Explicit native listener: actual committed selected body position and
   // its actual orientation. Never synthesize a camera/chase matrix.
   rr::game::OriginalAiAudioListener listener{};
   const auto& p=m.components.root.body.position;
   rr::game::BodyMatrix48G orientation;
   require50DU(rr::game::quaternionToMatrix48G(m.components.root.body.orientation,orientation),
       "AI native listener orientation unavailable");
   for(unsigned axis=0;axis<3;++axis){listener.position[axis]=p.v[axis];listener.forward[axis]=orientation.v[8+axis];}
   listener.position[3]=listener.forward[3]=1.f;
   {
    // Original 1A4558..1A4588: the listener is the CAMERA (position/look),
    // so the intro camera far from the grid hears distant engines quietly.
    // The body listener above remains only when no camera view exists yet.
    float cameraPosition[3],cameraLook[3];
    const bool fromCamera=audioListenerCamera50DU&&*audioListenerCamera50DU&&(*audioListenerCamera50DU)->nativeAudioListenerPose(cameraPosition,cameraLook);
    if(fromCamera)for(unsigned axis=0;axis<3;++axis){
     listener.position[axis]=cameraPosition[axis];listener.forward[axis]=cameraLook[axis];}
    static int listenerSource50DU=-1;
    if(listenerSource50DU!=(fromCamera?1:0)){listenerSource50DU=fromCamera?1:0;
     std::cerr<<"audio_listener source="<<(fromCamera?"camera":"player_body")<<" position="<<listener.position[0]<<','
         <<listener.position[1]<<','<<listener.position[2]<<" player="<<p.v[0]<<','<<p.v[1]<<','<<p.v[2]<<"\n";}
    static std::uint64_t listenerSamples50DU=0;
    if(++listenerSamples50DU%312==0){
     const double dx=double(listener.position[0])-p.v[0],dz=double(listener.position[2])-p.v[2];
     std::cerr<<"audio_listener_stats source="<<(fromCamera?"camera":"player_body")
         <<" listener_to_player_xz="<<std::sqrt(dx*dx+dz*dz)<<"\n";
    }
   }
   if(isRcTrack){
    require50DU(rcPositionalAudio&&rcPositionalAudio->ready()&&
        rcPositionalAudio->sourceIdentity()==rcColdSources.get(),
        "RC positional audio lost its actual retained source");
    // Actual committed selected-body listener, shared World4D8 and process RNG.
    // Runs once at the existing countdown/world audio boundary before shared mix.
    const auto positional=rcPositionalAudio->service(&listener,1,s.noiseOwner());
    require50DU(positional.complete,"RC positional audio service rejected");
   }
   if(isTbTrack){
    require50DU(tbPositionalAudio&&tbPositionalAudio->ready()&&
        tbPositionalAudio->sourceIdentity()==tbColdSources.get(),
        "TB positional audio lost its actual retained source");
    // Existing countdown/committed-world boundary, once before shared mix.
    // Pause exits before this boundary; the adapter also respects World4D8.
    const auto positional=tbPositionalAudio->service(&listener,1,s.noiseOwner());
    require50DU(positional.complete,"TB positional audio service rejected");
   }
   if(isGbTrack){
    require50DU(gbPositionalAudio&&gbPositionalAudio->ready()&&
        gbPositionalAudio->sourceIdentity()==gbColdSources.get(),
        "GB positional audio lost its actual retained source");
    // Existing countdown/committed-world boundary, once before shared mix.
    // Pause exits before this boundary; the adapter also respects World4D8.
    const auto positional=gbPositionalAudio->service(&listener,1,s.noiseOwner());
    require50DU(positional.complete,"GB positional audio service rejected");
   }
   if(isMcTrack){
    require50DU(mcPositionalAudio&&mcPositionalAudio->ready()&&
        mcPositionalAudio->sourceIdentity()==mcColdSources.get(),
        "MC positional audio lost its actual retained source");
    // Existing countdown/committed-world boundary, once before shared mix.
    // Pause exits before this boundary; the adapter also respects World4D8.
    const auto positional=mcPositionalAudio->service(&listener,1,s.noiseOwner());
    require50DU(positional.complete,"MC positional audio service rejected");
   }
   require50DU(originalAiAudio->setListener(listener)&&originalAiAudio->mixBoundary(s.noiseOwner()),
       "native AI Aural service rejected");return;
  }
  const auto beforeOpponentAudio=::allocations;
  auto inputs=opponentEngineInputs;
  for(std::size_t i=0;i<inputs.size();++i)
   inputs[i].active=inputs[i].active&&racerBank.active(i+1)&&
       withRace([&](auto& actualRace){return race_session50HM::simulationAllowed(actualRace,static_cast<std::uint32_t>(i+1));});
  // The listener follows the committed player body, so distance is independent
  // of render cadence, resize, and uncommitted chase-camera preparation.
  const auto& p=m.components.root.body.position;
  const auto receipt=opponentAudio->update({p.v[0],p.v[1],p.v[2]},inputs);
  require50DU(receipt.okay(),"original opponent engine audio world update rejected");
  require50DU(::allocations==beforeOpponentAudio,"opponent engine audio update allocated C++ storage");
 };
 std::function<void()> activateColdRace;
 bool gbColdPrefixClosed=!isGbTrack;
 auto closeGbColdPrefix=[&]()noexcept->bool {try{
  if(gbColdPrefixClosed)return true;
  if(gbQueryBinding&&gbQueryBinding->enrolled()&&!gbQueryBinding->fence())return false;
  if(!closeGbBankRegistries())return false;
  if(!lapOwner.releaseGbTrack())return false;
  gbRace={};gbMapView={};gbAiView={};gbMapMetadata={};gbAiMetadata={};gbWorldBorrow.reset();
  if(gbQueryBinding&&gbQueryBinding->enrolled()){
   if(!gbQueryBinding->retireContacts()||!gbQueryBinding->close())return false;
  }else if(!gbNavigation.close())return false;
  gbQueryBinding.reset();
  if(gbTriggerActions&&!gbTriggerActions->close())return false;
  if(gbParticleRuntime&&!gbParticleRuntime->quiesce())return false;
  if(gbTrackParticles&&!gbTrackParticles->close())return false;
  if(gbParticleRuntime&&!gbParticleRuntime->closeAfterPool())return false;
  if(!racerBank.retireNativeBacking())return false;
  gbBankPrefixClosed=true;gbColdPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct GbColdPrefixGuard {
  decltype(closeGbColdPrefix)& close;bool armed=true;
  ~GbColdPrefixGuard()noexcept {if(armed&&!close()){
   std::fputs("GB actual Bank cold-prefix retirement rejected; retained until fail-stop\n",stderr);std::terminate();}}
 } gbColdPrefixGuard{closeGbColdPrefix};
 bool mcColdPrefixClosed=!isMcTrack;
 auto closeMcColdPrefix=[&]()noexcept->bool {try{
  if(mcColdPrefixClosed)return true;
  if(mcQueryBinding&&mcQueryBinding->enrolled()&&!mcQueryBinding->fence())return false;
  if(!closeMcBankRegistries())return false;
  if(!lapOwner.releaseMcTrack())return false;
  mcRace={};mcMapView={};mcAiView={};mcMapMetadata={};mcAiMetadata={};mcWorldBorrow.reset();
  if(mcQueryBinding&&mcQueryBinding->enrolled()){
   if(!mcQueryBinding->retireContacts()||!mcQueryBinding->close())return false;
  }else if(!mcNavigation.close())return false;
  mcQueryBinding.reset();
  if(mcTriggerActions&&!mcTriggerActions->close())return false;
  if(mcParticleRuntime&&!mcParticleRuntime->quiesce())return false;
  if(mcTrackParticles&&!mcTrackParticles->close())return false;
  if(mcParticleRuntime&&!mcParticleRuntime->closeAfterPool())return false;
  mcParticleVehicles.reset();
  if(!racerBank.retireNativeBacking())return false;
  mcBankPrefixClosed=true;mcColdPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct McColdPrefixGuard {
  decltype(closeMcColdPrefix)& close;bool armed=true;
  ~McColdPrefixGuard()noexcept {if(armed&&!close()){
   std::fputs("MC actual Bank cold-prefix retirement rejected; retained until fail-stop\n",stderr);std::terminate();}}
 } mcColdPrefixGuard{closeMcColdPrefix};

 // This interval precedes renderer/camera construction and closeWindow's
 // exception boundary. All captured owner backing predates this guard.
 bool mbColdBound=false;
 auto closeMbColdPrefix=[&]() noexcept -> bool {try {
  if(!mbColdBound)return true;
  if(!mbSourceTriggers||!mbCollisionWorld)return false;
  if(mbSourceTriggers->pending()&&!mbSourceTriggers->retireSuspended([&](std::size_t slot){
   require50DU(slot<racerCables.size()&&racerCables[slot]&&racerRigs[slot],
       "MB cold pending cleanup requires actual slot owners");
   return mb_source_trigger50MB::RetirementBorrow{s,x.bindings,x.parameters,gcLightOwner,
       racerCables[slot]->visibility50(),recoveryQueries(),zoneLifecycle,liveWorld,membershipWork,
       slot==0?playerRacer60:originalAi.racer60};
  }))return false;
  // Pending source borrows have closed before model retirement can visit Bank.
  if(!nativeColdModelRetirement.close())return false;
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   if(!racerBank.visit(slot,[&](){
    if(!racerCables[slot]||!racerRigs[slot])return false;
    if(!mbSourceTriggers->retireColdVisibility(slot,s,x.bindings,x.parameters,gcLightOwner,
        racerCables[slot]->visibility50(),gcLightDiagnostic))return false;
    const auto queries=recoveryQueries();
    for(std::size_t index=0;index<4;++index){const auto& q=queries.query[index];
     if(!q.membership||!q.object||(q.membership->attached&&q.membership->object!=q.object))return false;
     const auto removed=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,*q.membership,membershipWork);
     if(removed.status!=LiveZoneStatus50EN::Complete||q.membership->attached||q.membership->object)return false;
     if(index==1)s.shape.registrationE0=0;
    }
    auto removed=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,secondaryMembership,membershipWork);
    if(removed.status!=LiveZoneStatus50EN::Complete||secondaryMembership.attached||secondaryMembership.object)return false;
    for(std::size_t i=0;i<3;++i){auto* member=mbCollisionWorld->triggerMembership(i);if(!member)return false;
     removed=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,member->state,membershipWork);
     if(removed.status!=LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;
    }
    return true;
   }))return false;
  }
  if(!mbSourceTriggers->unbind())return false;
  mbSourceTriggers.reset();mbColdBound=false;return true;
 }catch(...){return false;}};
 struct MbColdPrefixGuard {
  decltype(closeMbColdPrefix)& close;bool armed=true;
  ~MbColdPrefixGuard() noexcept {if(armed&&!close()){
   std::fputs("MB cold prefix retirement rejected; owners retained until fail-stop\n",stderr);
   std::terminate();
  }}
 } mbColdPrefixGuard{closeMbColdPrefix};
 if(raceEnabled){
  std::array<native_racer_contact50HW::Binding,raceRacerCapacity> contactBindings{};
  // The actual Tournament profile and authenticated field authorize ranks;
  // manager/Bank slot identity is never reordered to obtain a start position.
  const bool canonicalTournamentGrid=[&](){
   if(!originalUiStarted||originalUiActive||!humanCourseEnabled||
      !(directOriginalHuman||isRaTrack))return false;
   using Profile=rr::platform::OriginalUiRiderProfileOwner50KG;
   const auto* profile=originalMainApplication.riderProfile.get();
   Profile::CourseRaceSelection selected{};
   require50DU(profile&&profile->courseRaceSnapshot(selected),
       "Tournament grid requires the actual retained Course profile");
   if(selected.mode34!=2)return false;
   const auto stamp=profile->stamp();
   Profile::TournamentCareerSnapshot career{};
   OriginalUiCourseRememberedProfile50KG course{};
   require50DU(courseRaceCold&&profile->selectedTournamentCourseSnapshot(career,course)&&
       stamp.mode34==2&&career.stamp.mode34==2&&course.mode34==2&&
       stamp.generation==career.stamp.generation&&
       selected.subtype==1&&selected.rider==playerRacer60&&career.career100C0==playerRacer60&&
       selected.entrants==8u&&raceRacerCount==8u&&raceOpponentCount==7u&&
       originalAiHumanCount==1u&&playerUsesRosterRig&&
       ((selected.packed1125C>>5)&3u)==1u&&((selected.packed1125C>>7)&15u)==7u&&
       (career.progress219C&31u)<course.courseOrder100C4.size()&&
       selected.course==course.courseOrder100C4[career.progress219C&31u],
       "Tournament grid lost selected career/Course identity");
   namespace original=rr::game::original_opponent_selection50KG;
   const auto order=original::courseIndex187B10(course.courseOrder100C4,
       static_cast<std::uint8_t>(selected.course))+1u;
   std::array<original::RacerId,7> opponents{};
   require50DU(original::deterministic188470(order<5u?1u:(order<9u?2u:3u),
       static_cast<original::RacerId>(playerRacer60),opponents),
       "Tournament grid deterministic field rejected");
   for(std::size_t slot=0;slot<8u;++slot){
    const auto id=slot?static_cast<std::uint32_t>(opponents[slot-1]):playerRacer60;
    const auto* identity=original_racer_selection50IJ::findByRacer60(id);
    require50DU(identity&&racerRigs[slot]&&racerRigs[slot]->podId==identity->podId,
        "Tournament grid differs from the admitted authenticated field");
   }
   require50DU(profile->stamp().generation==stamp.generation&&profile->stamp().mode34==2,
       "Tournament grid profile changed during cold admission");
   return true;
  }();
  // Execute the original cold owners stage synchronously against these same locals.
  interactive_cold50DU::OwnersContext{
   .canonicalTournamentGrid=canonicalTournamentGrid,
   .actualRacer60ForResults=dynamicRacer60,
   .allAiEnabled=allAiEnabled,
   .announcerRequestsUnjoined=announcerRequestsUnjoined,
   .applicationEvents=applicationEvents,
   .authored=authored,
   .authoredFinishSteering10A4=authoredFinishSteering10A4,
   .authoredTrackName=authoredTrackName,
   .autoDriveEnabled=autoDriveEnabled,
   .autoStopReported=autoStopReported,
   .baseGlobals=baseGlobals,
   .baseTail=baseTail,
   .bindColdNativeEffects=interactive_frame50DU::BorrowedCall<void()>{bindColdNativeEffects},
   .boostCoolingCF8=boostCoolingCF8,
   .boostDiagnostic=boostDiagnostic,
   .boostParameters=boostParameters,
   .boostState=boostState,
   .candidates=candidates,
   .checkpointOutput=checkpointOutput,
   .checkpointRequested=checkpointRequested,
   .clock=clock,
   .coldFlame=coldFlame,
   .collectedCalls=collectedCalls,
   .completedFrames=completedFrames,
   .consumedRaStage=consumedRaStage,
   .contactBindings=contactBindings,
   .controller=controller,
   .controllerActions=controllerActions,
   .controllerEventStorage=controllerEventStorage,
   .controllerEvents=controllerEvents,
   .controllerPresses=controllerPresses,
   .controllerReleases=controllerReleases,
   .courseDiagnostic=courseDiagnostic,
   .courseOutput=courseOutput,
   .courseState=courseState,
   .courseTuning=courseTuning,
   .dynamicContactEnabled=dynamicContactEnabled,
   .dynamicMusicProfile=dynamicMusicProfile,
   .effectCommands=effectCommands,
   .endpointScenes=endpointScenes,
   .endpoints=endpoints,
   .freshMainProbe=freshMainProbe,
   .freshProbe100=freshProbe100,
   .freshTerrainProbe=freshTerrainProbe,
   .gaAnimatedMembership=gaAnimatedMembership,
   .gaTriggerMembership=gaTriggerMembership,
   .gbAiMetadata=gbAiMetadata,
   .gbAiView=gbAiView,
   .gbAnimationBinding=gbAnimationBinding,
   .gbAnimationBindingDiagnostic=gbAnimationBindingDiagnostic,
   .gbCollisionWorld=gbCollisionWorld,
   .gbMapMetadata=gbMapMetadata,
   .gbMapView=gbMapView,
   .gbWorldBorrow=gbWorldBorrow,
   .gcAnimatedMembership=gcAnimatedMembership,
   .gcTriggerActions=gcTriggerActions,
   .gcTriggerMembership=gcTriggerMembership,
   .humanBaseUndertrack=humanBaseUndertrack,
   .humanCourseEnabled=humanCourseEnabled,
   .initial=initial,
   .initialMetadata=initialMetadata,
   .initialPairs=initialPairs,
   .isGaTrack=isGaTrack,
   .isGbTrack=isGbTrack,
   .isMaSaTrack=isMaSaTrack,
   .isMbTrack=isMbTrack,
   .isMcTrack=isMcTrack,
   .isRaTrack=isRaTrack,
   .isRcTrack=isRcTrack,
   .isScTrack=isScTrack,
   .isTbTrack=isTbTrack,
   .isTcTrack=isTcTrack,
   .keyboardDiagnostic=keyboardDiagnostic,
   .lapContinuity=lapContinuity,
   .lapOwner=lapOwner,
   .legacyGround=legacyGround,
   .m=m,
   .maTriggerMembership=maTriggerMembership,
   .mainContactFrames=mainContactFrames,
   .mainCrossedPortals=mainCrossedPortals,
   .mbAiMetadata=mbAiMetadata,
   .mbAiView=mbAiView,
   .mbCollisionWorld=mbCollisionWorld,
   .mbMapMetadata=mbMapMetadata,
   .mbMapView=mbMapView,
   .mcAiMetadata=mcAiMetadata,
   .mcAiView=mcAiView,
   .mcAnimationBinding=mcAnimationBinding,
   .mcAnimationBindingDiagnostic=mcAnimationBindingDiagnostic,
   .mcCollisionWorld=mcCollisionWorld,
   .mcMapMetadata=mcMapMetadata,
   .mcMapView=mcMapView,
   .mcWorldBorrow=mcWorldBorrow,
   .movement=movement,
   .movementFlag=movementFlag,
   .movementInitial=movementInitial,
   .movementResult=movementResult,
   .movementSource=movementSource,
   .namesForEffects=namesForEffects,
   .nativeBottomLeftModels=nativeBottomLeftModels,
   .nativeBottomRightModels=nativeBottomRightModels,
   .nativeCalls=nativeCalls,
   .nativeChariotModels=nativeChariotModels,
   .nativeEffectCount=nativeEffectCount,
   .nativeEffects=nativeEffects,
   .nativeLeftModels=nativeLeftModels,
   .nativeModelDomain=nativeModelDomain,
   .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
   .nativeRightModels=nativeRightModels,
   .nativeRosterOwners=nativeRosterOwners,
   .nativeVideo=nativeVideo,
   .navigationAI=navigationAI,
   .navigationAIScan=navigationAIScan,
   .navigationAISequential=navigationAISequential,
   .navigationCA0=navigationCA0,
   .navigationCA0Copies=navigationCA0Copies,
   .navigationDiagnostic=navigationDiagnostic,
   .navigationGateSkips=navigationGateSkips,
   .navigationMap=navigationMap,
   .navigationMapScan=navigationMapScan,
   .navigationMapSequential=navigationMapSequential,
   .navigationProgress=navigationProgress,
   .navigationUpdates=navigationUpdates,
   .openNativeVideo=interactive_frame50DU::BorrowedCall<void()>{openNativeVideo},
   .opponentBoostAudio=opponentBoostAudio,
   .originalAi=originalAi,
   .originalAiAuthoringDiagnostic=originalAiAuthoringDiagnostic,
   .originalAiCreateSelection=originalAiCreateSelection,
   .originalAiEnabled=originalAiEnabled,
   .originalAiHumanCount=originalAiHumanCount,
   .originalAiOwnsSlot=interactive_frame50DU::BorrowedCall<bool(std::size_t) noexcept>{originalAiOwnsSlot},
   .originalAiReads=originalAiReads,
   .originalAiReceipt=originalAiReceipt,
   .originalAiSession=originalAiSession,
   .originalHumanRecovery=originalHumanRecovery,
   .originalMainApplication=originalMainApplication,
   .originalRecovery=originalRecovery,
   .originalRecoveryEnabled=originalRecoveryEnabled,
   .originalRecoveryModels=originalRecoveryModels,
   .packet=packet,
   .packetLive=packetLive,
   .peakCollected=peakCollected,
   .pendingSpeech=pendingSpeech,
   .playerEnvironment=playerEnvironment,
   .playerEventChild=playerEventChild,
   .playerEvents=playerEvents,
   .playerUsesRosterRig=playerUsesRosterRig,
   .poseHistory=poseHistory,
   .qfilter=qfilter,
   .query=query,
   .queryResult=queryResult,
   .raDiagnostic=raDiagnostic,
   .raFreshProbe=raFreshProbe,
   .raSession=raSession,
   .raSessionDiagnostic=raSessionDiagnostic,
   .raWorld=raWorld,
   .raceRacerCount=raceRacerCount,
   .racerBank=racerBank,
   .racerCables=racerCables,
   .racerCollisions=racerCollisions,
   .racerContactIds=racerContactIds,
   .racerContactMaterials=racerContactMaterials,
   .racerFrameReads=racerFrameReads,
   .racerHudNames=racerHudNames,
   .racerNames=racerNames,
   .racerRigs=racerRigs,
   .racerVisuals=racerVisuals,
   .rcCollisionWorld=rcCollisionWorld,
   .rcWorldBorrow=rcWorldBorrow,
   .rebaseBounds=rebaseBounds,
   .rebaseDiagnostic=rebaseDiagnostic,
   .rebaseModelCount=rebaseModelCount,
   .rebaseModels=rebaseModels,
   .rebaseNodes=rebaseNodes,
   .rebaseWorkspace=rebaseWorkspace,
   .records=records,
   .recoveryPaused=recoveryPaused,
   .recoveryPodLive=recoveryPodLive,
   .recoveryQueryOwners=recoveryQueryOwners,
   .repairActiveTicks=repairActiveTicks,
   .requireMbRuntimeNavigation50MB=interactive_frame50DU::BorrowedCall<void()>{requireMbRuntimeNavigation50MB},
   .resetEffectBindings=resetEffectBindings,
   .resetEffectState=resetEffectState,
   .resetEffectWorkspace=resetEffectWorkspace,
   .response=response,
   .s=s,
   .saTriggerMembership=saTriggerMembership,
   .scColdSources=scColdSources,
   .scGround=scGround,
   .scTriggerMembership=scTriggerMembership,
   .secondaryContactFrames=secondaryContactFrames,
   .secondaryCrossedPortals=secondaryCrossedPortals,
   .secondaryFilter=secondaryFilter,
   .secondaryQuery=secondaryQuery,
   .secondaryRecords=secondaryRecords,
   .secondaryResponse=secondaryResponse,
   .secondaryResult=secondaryResult,
   .secondarySettings=secondarySettings,
   .secondarySpring=secondarySpring,
   .selectedOriginalAiMapMetadata=selectedOriginalAiMapMetadata,
   .selectedOriginalAiMetadata=selectedOriginalAiMetadata,
   .selection=selection,
   .sessionIndex=sessionIndex,
   .settings=settings,
   .si=si,
   .sim=sim,
   .sip=sip,
   .sourceConsumedPauseActivation=sourceConsumedPauseActivation,
   .sourceDamage=sourceDamage,
   .sourcePause=sourcePause,
   .sourcePositionStatsPrestage=sourcePositionStatsPrestage,
   .sourcePositionStatsPrestageDiagnostic=sourcePositionStatsPrestageDiagnostic,
   .sourceSpeedBoost=sourceSpeedBoost,
   .sourceTimeStats=sourceTimeStats,
   .speechCues=speechCues,
   .speechEvents=speechEvents,
   .speechState=speechState,
   .spring=spring,
   .tbAiMetadata=tbAiMetadata,
   .tbAiView=tbAiView,
   .tbCollisionWorld=tbCollisionWorld,
   .tbMapMetadata=tbMapMetadata,
   .tbMapView=tbMapView,
   .tbWorldBorrow=tbWorldBorrow,
   .tcAiMetadata=tcAiMetadata,
   .tcAiView=tcAiView,
   .tcCollisionWorld=tcCollisionWorld,
   .tcMapMetadata=tcMapMetadata,
   .tcMapView=tcMapView,
   .tcWorldBorrow=tcWorldBorrow,
   .terrainBindings=terrainBindings,
   .terrainHitFrames=terrainHitFrames,
   .terrainLiveOwner=terrainLiveOwner,
   .terrainOwner=terrainOwner,
   .terrainQueries=terrainQueries,
   .terrainSupportedFrames=terrainSupportedFrames,
   .trackStorage=trackStorage,
   .unavailableEffect=unavailableEffect,
   .wrongWayActiveTicks=wrongWayActiveTicks,
   .wrongWayContinuation=wrongWayContinuation,
   .wrongWayDiagnostic=wrongWayDiagnostic,
   .wrongWayReceipt=wrongWayReceipt,
   .wrongWayUpdates=wrongWayUpdates,
   .x=x,
   .nativeHumanAudio=audioEnabled&&originalUiStarted&&consumedRaStage.race()?liveAudio.get():nullptr,
   .nativeHumanSpeechCounts=&speechCounts,
  }.run();
 // Bind once to the selected authenticated map representation. Placement is
 // shared across human/opponents; no per-racer track dispatch or null fallback.
 source_grid50IG::SelectedMapGrid selectedGridNavigation;
 if(trackSession->originalStaticCandidates||canonicalTournamentGrid){
  requireMbRuntimeNavigation50MB();
  const bool bound=isMcTrack?selectedGridNavigation.bindView(mcMapView):
      isGbTrack?selectedGridNavigation.bindView(gbMapView):
      isTbTrack?selectedGridNavigation.bindView(tbMapView):
      isTcTrack?selectedGridNavigation.bindView(tcMapView):
      isMbTrack?selectedGridNavigation.bindView(mbMapView):
      selectedGridNavigation.bindLegacy(navigationMap);
  require50DU(bound,"selected authenticated map has no cold grid capability");
 }
 //1AE9EC requests rank0 from the same authenticated course, not the
 //player's rank4 transform. Non-Tournament TA HOST placement stays unchanged.
 if(nativeRacerManager){
  require50DU(nativeRacerManager->count30()==racerBank.count()&&racerBank.count()==1u&&
      managerRuntimeSelection.rider==playerRacer60&&
      (managerRuntimeSelection.packed1125C>>11u)==playerRacer60,
      "record actor requires its actual selected rider and singleton bank");
  source_grid50IG::SelectedMapGrid recordGrid=selectedGridNavigation;
  if(!trackSession->originalStaticCandidates)
   require50DU(recordGrid.bindLegacy(navigationMap),"record actor original rank0 map unavailable");
  racerBank.visit(0,[&](){
   BodyMatrix48G rankZero{},groundedRankZero{};UndertrackDiagnostic50ET diagnostic{};
   const float selectedLift=x.parameters.visual.verticalLimit7EC;
   require50DU(original_record_grid50LA::prepareRankZeroPose(recordGrid,selectedLift,
       s.lifecycleShared.up434650.v,rankZero,diagnostic),
       "record actor original rank0 pose construction failed");
   require50DU(native_racer50HN::groundGridPose(ground,rankZero,selectedLift,
       groundStack.data(),groundStack.size(),groundedRankZero),
       "record actor rank0 has no actual ground hit");
   nativeRecordActor=std::make_unique<original_subtype3_record::Owner>();
   std::string error;
   require50DU(nativeRecordActor->constructFreshNoRecord(sessionIndex+1,
       managerRuntimeSelection.packed1125C,playerArchive,groundedRankZero,
       originalMainApplication.entities(),s,*nativeRacerManager,error),error.c_str());
  });
 }
#include "native_racer_grid50hn.inc"
  if(!isRaTrack&&playerUsesRosterRig)coldRacerStage50DU(playerPodId,"human grounded grid placement",[&](){
   placeOpponentAtGrid50HN(0);
   if(humanCourseEnabled)initializeOriginalHumanResetTimers50LC(playerEvents);
   require50DU(query.resource==&racerCollisions[0]->main()&&
       secondaryQuery.resource==&racerCollisions[0]->secondary()&&
       mainMembership.lastType2Resource==query.resource&&
       secondaryMembership.lastType2Resource==secondaryQuery.resource,
       "selected human retains another racer's static contact resource");
  });
  if(!isRaTrack)for(std::size_t racer=1;racer<racerBank.count();++racer)
   coldRacerStage50DU(racerRigs[racer]->podId,"grounded grid placement",[&](){placeOpponentAtGrid50HN(racer);});
  // Execute the original cold contacts stage synchronously against these same locals.
  interactive_cold50DU::ContactsContext{
   .allAiEnabled=allAiEnabled,
   .applicationClock=applicationClock,
   .audioEnabled=audioEnabled,
   .completedFrames=completedFrames,
   .contactBindings=contactBindings,
   .directOriginalHuman=directOriginalHuman,
   .dynamicContactEnabled=dynamicContactEnabled,
   .dynamicRacer60=dynamicRacer60,
   .dynamicRegistry=dynamicRegistry,
   .gaAnimatedWorld=gaAnimatedWorld,
   .gaGateAudio=gaGateAudio,
   .gaTriggerActions=gaTriggerActions,
   .gaTriggerAuthoring=gaTriggerAuthoring,
   .gbCollisionWorld=gbCollisionWorld,
   .gbNavigationOwner=gbNavigationOwner,
   .gbQueryBinding=gbQueryBinding,
   .gcAnimatedWorld=gcAnimatedWorld,
   .gcGateAudio=gcGateAudio,
   .gcGraphicsSequence=gcGraphicsSequence,
   .gcLightDiagnostic=gcLightDiagnostic,
   .gcLightOwner=gcLightOwner,
   .gcTriggerActions=gcTriggerActions,
   .gcTriggerAuthoring=gcTriggerAuthoring,
   .humanCourseEnabled=humanCourseEnabled,
   .initialCollisionZone=initialCollisionZone,
   .isGbTrack=isGbTrack,
   .isMbTrack=isMbTrack,
   .isMcTrack=isMcTrack,
   .isRaTrack=isRaTrack,
   .isRcTrack=isRcTrack,
   .liveWorld=liveWorld,
   .m=m,
   .maOriginalDisc=maOriginalDisc,
   .maTriggerActions=maTriggerActions,
   .maTriggerAuthoring=maTriggerAuthoring,
   .mainMembership=mainMembership,
   .mbAiView=mbAiView,
   .mbColdBound=mbColdBound,
   .mbCollisionWorld=mbCollisionWorld,
   .mbOriginalDisc=mbOriginalDisc,
   .mbSourceTriggers=mbSourceTriggers,
   .mcCollisionWorld=mcCollisionWorld,
   .mcNavigationOwner=mcNavigationOwner,
   .mcQueryBinding=mcQueryBinding,
   .membershipWork=membershipWork,
   .nativeOriginalFleet=nativeOriginalFleet,
   .nativeSelectedLights=nativeSelectedLights,
   .navigationAI=navigationAI,
   .navigationMap=navigationMap,
   .navigationProgress=navigationProgress,
   .opponentDraw=opponentDraw,
   .opponentUvs=opponentUvs,
   .originalAiEnabled=originalAiEnabled,
   .originalAiHumanCount=originalAiHumanCount,
   .originalAiOpponentCount=originalAiOpponentCount,
   .originalRecoveryEnabled=originalRecoveryEnabled,
   .publishRecoveryQueries=interactive_frame50DU::BorrowedCall<bool()>{publishRecoveryQueries},
   .qfilter=qfilter,
   .query=query,
   .raceOpponentCount=raceOpponentCount,
   .raceRacerCount=raceRacerCount,
   .racerBank=racerBank,
   .racerContactIds=racerContactIds,
   .racerContactOwner=racerContactOwner,
   .racerContactsEnabled=racerContactsEnabled,
   .racerRigs=racerRigs,
   .racerVisuals=racerVisuals,
   .rcColdSources=rcColdSources,
   .rcNavigationBorrow=rcNavigationBorrow,
   .rcOriginalDisc=rcOriginalDisc,
   .rcTriggerActions=rcTriggerActions,
   .rcTriggerAuthoring=rcTriggerAuthoring,
   .rcWorldBorrow=rcWorldBorrow,
   .recoveryBorrowedQueries=interactive_frame50DU::BorrowedCall<undertrack_owners50ew_fixture::BorrowedQueries()>{recoveryBorrowedQueries},
   .recoveryQueryOwners=recoveryQueryOwners,
   .requireMbRuntimeNavigation50MB=interactive_frame50DU::BorrowedCall<void()>{requireMbRuntimeNavigation50MB},
   .s=s,
   .saOriginalDisc=saOriginalDisc,
   .saTriggerActions=saTriggerActions,
   .saTriggerAuthoring=saTriggerAuthoring,
   .scColdSources=scColdSources,
   .scGround=scGround,
   .scOriginalDisc=scOriginalDisc,
   .scTriggerActions=scTriggerActions,
   .scTriggerAuthoring=scTriggerAuthoring,
   .secondaryFilter=secondaryFilter,
   .secondaryMembership=secondaryMembership,
   .secondaryQuery=secondaryQuery,
   .selectedOriginal=selectedOriginal,
   .sessionRandom=sessionRandom,
   .sourceAnnouncer=sourceAnnouncer,
   .tbAiView=tbAiView,
   .tbColdSources=tbColdSources,
   .tbCollisionWorld=tbCollisionWorld,
   .tbMapView=tbMapView,
   .tbOriginalDisc=tbOriginalDisc,
   .tbTriggerActions=tbTriggerActions,
   .tbWorldBorrow=tbWorldBorrow,
   .tcAiView=tcAiView,
   .tcColdSources=tcColdSources,
   .tcCollisionWorld=tcCollisionWorld,
   .tcMapView=tcMapView,
   .tcOriginalDisc=tcOriginalDisc,
   .tcTriggerActions=tcTriggerActions,
   .tcWorldBorrow=tcWorldBorrow,
   .trackCounts=trackCounts,
   .x=x,
   .zoneLifecycle=zoneLifecycle,
  }.run();
  // Race activation begins here, AFTER genuine cold rig/grid/contact/draw
  // owners are loaded. The neutral setup tick is real native physics, not
  // asset preparation. Main-only retains these assets but cannot activate a
  // race, publish fabricated ranks, or sample a neutral driving packet.
  activateColdRace=[&](){
  require50DU(!originalUiActive&&race.phase==race_session50HM::Phase::Uninitialized,
      "native cold race activation is one-shot after Course input ownership ends");
  // Every activated race starts with its racers intact (Restart rebuilds them).
  rr::game::resetNativeDestroyedRacers50PD();
  native_wreck_debris50PD::resetSim();
  native_wreck_effects50PD::reset();
  rr::game::nativeTestDestroyPending50PD={};
  rr::game::nativeTestDestroyAttacker50PD.fill(-1);
  for(auto& test:rr::game::nativeTestDestroys50PD)test.fired=false;
  if(!allAiEnabled){
   platform::SelectedControlInput50KY current{};
   require50DU(platform::bindSelectedControlInput50KY(controlPresets,playerActionIndexFFC,playerSource5C,current)&&
       current.preset==humanControlInput.preset&&current.twinEngine==humanControlInput.twinEngine,
       "human Controls changed after cold handling; a fresh cold world is required before activation");
  }
  if(originalUiStarted){
   OriginalUiLightManagerLifetimeDiagnostic50KG lightDiagnostic;
   require50DU(originalCameraEnabled&&originalMainApplication.lightManager&&
       originalMainApplication.lightManager->beginNativeGameplay(sessionIndex+1,lightDiagnostic),
       "native race must borrow the dormant application lighting lifetime after real UI release");
  }
  if(isMcTrack){
   require50DU(mcParticleVehicles&&mcParticleVehicles->ready(),"MC actual retained racer producers absent");
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    std::string error;require50DU(mcParticleVehicles->initializeRacer(slot,nativeEffects,nativeEffectCount,x.bindings.visual.bottomLeft1C4!=SceneNodeIndex49P::none,
      s,x.bindings,x.diagnostic,nativeWaterApplication.initialStep434630,s.noiseOwner().sharedRandomState,error),error.c_str());
    auto* racer=mcParticleVehicles->racer(slot);require50DU(racer,"MC completed racer water owner absent");resetEffectBindings.table81CPresent=true;
    for(unsigned side=0;side<2;++side){resetEffectBindings.owners81C[side]=0;resetEffectBindings.controls81C[side]=&racer->spray(side).controls();}
   });
   require50DU(mcParticleVehicles->constructorsComplete()&&mcParticleVehicles->expectedRacers()==raceRacerCount&&
       mcParticleVehicles->initializedRacers()==raceRacerCount,"MC actual eight-racer constructor coverage");
  }
  if(nativeGcWater){
   require50DU(nativeGcWater->activate(s.noiseOwner().sharedRandomState,sessionRandom,applicationClock),
       "actual GC gameplay fog population rejected");
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(!nativeWaterRacers[slot],"native water racer construction repeated");
    nativeWaterRacers[slot]=std::make_unique<native_gc_water_racer50KG::Owner>();
    // Register the actual14/8 source children before the two sprays, in the
    // same constructor/list order used by every later component visit.
    require50DU(nativeParticleChildren[slot].bindCold(nativeParticleAuthoring,nativeEffects,nativeEffectCount,
        x.bindings.visual.bottomLeft1C4!=SceneNodeIndex49P::none,nativeGcWater->particles()).okay(),
        "actual stopped particle child ownership rejected");
    // Explicit native source-owner namespace backed by these actual objects.
    require50DU(nativeWaterRacers[slot]->constructSprays(nativeGcWater->water(),nativeGcWater->particles(),
        0xf5000001u+static_cast<std::uint32_t>(slot),0xf3000001u,
        0xf4000001u+static_cast<std::uint32_t>(slot),
        nativeWaterApplication.initialStep434630,nativeGcWater->sprayBytes(),s.noiseOwner().sharedRandomState).okay(),
        "actual two-spray constructor rejected");
    require50DU(nativeWaterRacers[slot]->finishConstructor(s,x.bindings,x.diagnostic).okay(),
        "actual BE0/terrain water constructor join rejected");
    //2172C0/2172E0 target the canonical19CC50 controls, not the separate
    //50BE ID registry (whose integer IDs can overlap manager owner handles).
    resetEffectBindings.table81CPresent=true;
    for(unsigned side=0;side<2;++side){resetEffectBindings.owners81C[side]=0;
     resetEffectBindings.controls81C[side]=&nativeWaterRacers[slot]->spray(side).controls();}

   });
   std::cerr<<"GC NATIVE WATER: shared lattice, exact height/reset/force/spray owners; "
       "native post-prefix/pre-lap scheduling and completed-track-span fire visibility. "
       "NativeUndefinedCarryPolicy enabled; actual dust/smoke, spark and fire children share the scene pool.\n";
  }
  const auto retainActualSpeechOwners=[&](){
   // Any consumed original race (RA, TA, MA, SA, GA, GC) publishes the same
   // Application race28C and session generation the speech owners bind to.
   const auto actualRace=consumedRaStage.race()?consumedRaStage.race():consumedTaStage.race()?consumedTaStage.race():
       consumedMaStage.race()?consumedMaStage.race():consumedSaStage.race()?consumedSaStage.race():
       consumedGaStage.race()?consumedGaStage.race():consumedGcStage.race();
   if(!audioEnabled||!originalUiStarted||!humanCourseEnabled||!actualRace)return;
   // RA keeps its fail-closed contract. Other consumed races join when their
   // identities match and otherwise keep the existing speech path (logged).
   if(actualRace!=consumedRaStage.race()&&!(originalMainApplication.race28C()==actualRace.get()&&
      originalMainApplication.state&&&s.noiseOwner()==&originalMainApplication.state->sharedNoise308030)){
    std::cerr<<"native_human_speech_join_unavailable session="<<actualRace->entity_.sessionGeneration<<'\n';
    return;
   }
   //This is the actual successful native speech producer, not Scope capture
   //or a future event's claim. All initialization draws already happened once.
   require50DU(originalMainApplication.race28C()==actualRace.get()&&originalMainApplication.state&&
       &s.noiseOwner()==&originalMainApplication.state->sharedNoise308030&&
       liveAudio->retainNativeHumanSpeechInitialization(originalMainApplication,racerBank,
           actualRace->entity_.sessionGeneration,playerRacer60,speechCounts,speechState,
           originalMainApplication.state->rawRandom308030),
       "actual human speech initializer/provider/App/Bank identity unavailable");
   if(originalAiAudioEnabled)require50DU(originalAiAudio->captureNativeSpeechReceipt(
       originalMainApplication,racerBank,actualRace->entity_.sessionGeneration,
       originalMainApplication.state->rawRandom308030,racerBank.count()),
       "actual AI speech initializer/provider/App/Bank identity unavailable");
  };
  if(originalAiAudioEnabled){
   if(originalUiStarted){require50DU(originalAiAudio->initializeSpeech(s.noiseOwner().sharedRandomState,
       humanCourseEnabled?&speechCounts:nullptr,humanCourseEnabled?&speechState:nullptr),
       "actual all-eight D48 shared-RNG initialization failed");
    retainActualSpeechOwners();
   }
   rr::game::OriginalAiAudioListener listener{};rr::game::BodyMatrix48G orientation;
   require50DU(rr::game::quaternionToMatrix48G(m.components.root.body.orientation,orientation),
       "cold AI listener orientation unavailable");
   for(unsigned axis=0;axis<3;++axis){listener.position[axis]=m.components.root.body.position.v[axis];
       listener.forward[axis]=orientation.v[8+axis];}
   listener.position[3]=listener.forward[3]=1.f;
   require50DU(originalAiAudio->setListener(listener)&&originalAiAudio->activate(),
       "AI source audio lifecycle activation rejected");
  }
  if(originalUiStarted&&audioEnabled&&!originalAiAudioEnabled){
   require50DU(initializePodSpeech50HG(speechCounts,
      s.noiseOwner().sharedRandomState,speechState)==PodSpeechStatus50HG::Complete,
      "selected original speech cursor initialization failed");
   retainActualSpeechOwners();
  }
  // One neutral, unscored native setup tick establishes actual graphics/ground
  // before countdown rendering. After this all physics freezes until GO.
  const platform::WindowInput50DQ noKeys{};
  raceSetupFrame=true;
  if(originalAiEnabled){
   requireMbRuntimeNavigation50MB();
   require50DU(!originalAiSession.managerBound&&selectedMapCount50MB()>1&&
       (isMcTrack?mcMapMetadata.matches(mcMapView):isGbTrack?gbMapMetadata.matches(gbMapView):isTbTrack?tbMapMetadata.matches(tbMapView):isTcTrack?tcMapMetadata.matches(tcMapView):isMbTrack?mbMapMetadata.matches(mbMapView):selectedOriginalAiMapMetadata.bound),
       "source manager create/reset requires the actual cold MAP owner");
   //Only the actual consumed RA uses the counted B4/native same-state join.
   //A rejected/partial canonical prefix never falls through to legacy reset.
   if(isRaTrack&&consumedRaStage.race()){
    // Native roster backing already exists. Give its B0 role one counted
    // owner before the B4 association, except the actual subtype3 record
    // route which already owns a distinct source manager. No second roster.
    if(!nativeRacerManager){
     std::uint32_t b0PC=0;
     require50DU(consumedRaStage.race()->initializeNativeRacerB0(*this,racerBank,b0PC),
         "actual counted B0/native roster cold join rejected");
    }
    std::uint32_t b4PC=0;
    require50DU(consumedRaStage.race()->initializeNativeTimingB4(*this,racerBank,b4PC),
        "actual counted B4/native manager cold join rejected");
   }else require50DU(initializeOriginalAiManager(originalAiSession.manager,originalAiSession.finished44,
       originalAiSession.retired40,racerBank.count(),dynamicContactEnabled?dynamicMusicProfile.mode11258:2u,
       isMcTrack?mcMapMetadata.nodes()[1].distanceToFinish60:isGbTrack?gbMapMetadata.nodes()[1].distanceToFinish60:isTbTrack?tbMapMetadata.nodes()[1].distanceToFinish60:isTcTrack?tcMapMetadata.nodes()[1].distanceToFinish60:isMbTrack?mbMapMetadata.nodes()[1].distanceToFinish60:selectedOriginalAiMapMetadata.nodes[1].distanceToFinish60,
       selectedMapNode50MB(0).position,selectedMapNode50MB(1).position,originalAiSession.trackLength38)==OriginalAiFinishStatus::Complete,
       "original1AF7F0/1AF960/1AFC70 manager lifecycle rejected");
   originalAiSession.managerBound=true;
  }
  if(originalAiEnabled)for(std::size_t slot=0;slot<racerBank.count();++slot)
   racerBank.visit(slot,[&](){s.fields.applicationByte58=originalAiSession.manager.flags58;});
  auto initializeGraphics=[&](std::size_t actualRacerSlot){
   if(s.fields.vehicle58==0u){composePacket(false,noKeys,actualRacerSlot);sampleController();}
   require50DU(advanceFrame(actualRacerSlot),"native race grid setup tick rejected");
   require50DU(lapContinuity.state.vehicleCC8==0,"cold race setup unexpectedly completed a lap");
  };
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   opponentFrame=true;
   try{
    captureDynamicBeforeFrame(racer);
    if(originalAiEnabled)observeOriginalAiFleet();
    racerBank.visit(racer,[&](){initializeGraphics(racer);});
   }
   catch(const std::exception& error){
    opponentFrame=false;
    std::throw_with_nested(std::runtime_error("Original racer "+racerRigs[racer]->podId+
        " [initial native graphics tick, "+racerNames[racer]+"]: "+error.what()));
   }
   opponentFrame=false;
  }
  captureDynamicBeforeFrame(0);
  if(originalAiEnabled)observeOriginalAiFleet();
  if(isRaTrack||isMbTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack)racerBank.visit(0,[&](){initializeGraphics(0);});
  else initializeGraphics(0);
  raceSetupFrame=false;
  if(originalAiEnabled){
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    initializeLapTimingFields50ID(lapContinuity.timing,dynamicContactEnabled?dynamicMusicProfile.mode11258:2u);lapContinuity.timingInitialized=true;
   });
   rankOriginalAiFleet();
  }
  race_session50HM::Config racePresentationConfig;
  racePresentationConfig.lapTarget=originalAiSettings1125C&31u;
  if(isRcTrack)requireMbRuntimeNavigation50MB();
  require50DU((isMcTrack?race_session50HM::initialize(mcRace,mcMapView,lapOwner.mcTrack(),raceRacerCount,racePresentationConfig):isGbTrack?race_session50HM::initialize(gbRace,gbMapView,lapOwner.gbTrack(),raceRacerCount,racePresentationConfig):isTbTrack?race_session50HM::initialize(tbRace,tbMapView,lapOwner.tbTrack(),raceRacerCount,racePresentationConfig):isTcTrack?race_session50HM::initialize(tcRace,tcMapView,lapOwner.tcTrack(),raceRacerCount,racePresentationConfig):isMbTrack?race_session50HM::initialize(mbRace,mbMapView,lapOwner.mbTrack(),raceRacerCount,racePresentationConfig):race_session50HM::initialize(legacyRace,*navigationMap,lapOwner.track,raceRacerCount,racePresentationConfig)),
      "authored race route/markers rejected");
  std::uint32_t gridOrder[maxHostRaceRacers50HT]{};
  for(std::uint32_t index=0;index<raceOpponentCount;++index)gridOrder[index]=index+1;
  gridOrder[raceOpponentCount]=0;
  require50DU(withRace([&](auto& actualRace){return race_session50HM::seedGridOrder(actualRace,gridOrder,raceRacerCount);}),
      "explicit host starting-grid order rejected");
  sampleRaceField();
  if(nativeOriginalFleet)publishOriginalAiStandings();
  else require50DU(withRace([&](auto& actualRace){return race_session50HM::commitFrame(actualRace,0);}),"race grid observations rejected");
  if(mode1Enabled){
   mode1=withRace([&](auto& actualRace){return std::make_unique<race_mode1_host50ID::Owner>(racerBank,lapOwner,s.fields,actualRace);});
   std::array<race_mode1_host50ID::RosterEntry,raceRacerCapacity> roles{};
   roles[0]={playerRacer60,0,true,true}; // Authenticated selected identity, one human.
   // Bot IDs are not synthesized. Category6 sees HOST AI role and returns
   // before identity-dependent speech; arena vehicle58 remains unchanged.
   require50DU(mode1->initialize(roles.data(),raceRacerCount),"cold mode1 lap/record/FIFO binding failed");
  }
  if(allAiEnabled)std::cerr<<"ORIGINAL ALL8 MANAGER: native1AFF50 timing/event6,1B0400 rank, "
      "1B02F0 finish list/count/time and1B0920 gaps. Explicit zero-human end policy waits for all "
      "eight genuine finishes, then retains the actual completed scene in native Results.\n";
  else if(humanCourseEnabled)std::cerr<<"ORIGINAL HUMAN MANAGER: one Player, "<<raceOpponentCount<<" original AI; source timing/rank/finish/end predicate, "
      "ending camera/audio/fade; bounded native Results presentation.\n";
  else std::cerr<<"RACE MODE "<<(mode1Enabled?1:2)
      <<(mode1Enabled?": source lap/records/event FIFO; explicit HOST world/rank/finish and distance projection.\n":
         ": existing HOST observer/audio fallback explicitly selected.\n");
  std::cerr<<"NATIVE RACE: "<<raceRacerCount<<" distinct original racers, "<<race.config.lapTarget<<" laps, 3-second countdown. "
      "Field count matches the original normal race; starting-grid placement and world scheduling are host-owned. ";
  if(allAiEnabled)std::cerr<<"Every racer, including the camera-followed racer, uses original in-game AI. ";
  else if(humanCourseEnabled)std::cerr<<"The selected human uses actual host input; "<<raceOpponentCount<<" opponents retain their original AI. ";
  else if(originalAiEnabled)std::cerr<<"One designated racer uses original in-game AI; other control paths remain separate. ";
  else std::cerr<<"This session has no original AI controller enabled. ";
  std::cerr<<"Each racer owns "<<racerBank.bytesPerRacer()<<" bytes of native state.\n";
  if(!nativeOriginalFleet)std::cerr<<"NATIVE FINISH CONTINUATION: source CD8 finish state retains native physics, "
      "engine audio, all original racer draws and sealed named standings. "
      "Original player finish controls retain the selected twin-engine config; "
      "RESULTS after the normal four-second countdown retains the live scene "
      "as HOST presentation until restart/quit.\n";
  std::cerr<<"RACER CONTACTS: "<<(racerContactsEnabled?"enabled":"disabled")
      <<"; each original main-body geometry and authored response, host world-frame scheduling. "
        "Empty-root material0 is a deterministic host policy; original padding parity is unclaimed. "
        "Secondary dynamic collision, damage attribution and opponent speech/collision/boost-event audio remain unjoined.\n";
  if(dynamicContactEnabled)std::cerr<<"ORIGINAL AI DYNAMIC CONTACTS: ordered body20C/210 and live D34 peers, "
      "source severity/common maxima, authentic profile11258=1 music, shared event1/event5/event6 FIFO. "
      "Actual roster Dax banks service event1 types1/2 and event5 type12 through one shared episode. "
      "Collision music uses native linear shared-mix fade to source target0 in1500ms, "
      "retirement at source1.5+0.35s; original IOP interpolation parity is unknown. "
      "Human source5C resolves actual native feedback objects; absent keyboard hardware preserves genuine no-device returns; "
      "ALL8 vehicle58=1 follows the original222D34 skip. "
      "Native serial world timing is explicit; no full original scheduler claim.\n";
  };
  if(!originalUiActive&&!isGbTrack&&!isMcTrack)activateColdRace();
 }
 if(originalUiActive){
  // Read actual retained owners without changing a counter or fabricating
  // Ready/rank/field state. Renderer.create consumes immutable source meshes,
  // never a VehicleDrawBatch or a completed vehicle graphics tick.
  require50DU(race.phase==race_session50HM::Phase::Uninitialized && race.racerCount==0 &&
      !originalAiSession.rankBound && !mode1 && !raceSetupFrame && !opponentFrame,
      "Main-only startup unexpectedly activated a race manager");
  require50DU(allAiControllerSamples==0 && allAiKeyboardPackets==0 && allAiHostDriverCalls==0 &&
      feedbackHelpers==0 && feedbackSubmissions==0 && feedbackLeaseRefreshes==0,
      "Main-only startup consumed driving input");
  const auto requireUnadvancedColdRacer=[&](){
   require50DU(completedFrames==0 && m.frameFactor==0.f && nativeCalls==0 &&
       s.phase==VehicleFramePhase49W::Ready && originalAi.dispatchedFrames==0 &&
       controllerPresses==0 && controllerReleases==0,
       "Main-only startup advanced an actual cold racer");
  };
  requireUnadvancedColdRacer();
  if(raceEnabled)for(std::size_t slot=0;slot<racerBank.count();++slot){
   require50DU(!originalAiSession.observed[slot].bound && !originalAiSession.fields[slot].bound &&
       !race.racers[slot].observedValid && !race.racers[slot].sampled,
       "Main-only startup published a race observation");
   racerBank.visit(slot,requireUnadvancedColdRacer);
  }
  std::cerr<<"ORIGINAL MAIN COLD: genuine loaded racer assets="<<(raceEnabled?racerBank.count():1u)
      <<"; all retained native frame counts/factors and driving samples are zero; "
      "race manager, source rank and field observations remain uninitialized.\n";
 }

 // Cold-selected player publications borrow stable owners through shutdown.
 // POD01 fixture resources never supply another original racer's draw or cable.
 coldLoadDiagnostics.begin("render_source_preparation");
 auto& playerDrawAssets=playerUsesRosterRig?racerVisuals[0]->drawAssets:drawAssets;
 const auto playerCableBindings=playerUsesRosterRig?racerCables[0]->bindings():cableBindings;
 std::vector<platform::VehicleWindowMaterialUv50DS> playerFlameUvs;
 for(unsigned component=1;component<playerDrawAssets.componentCount();++component){
  if(playerUsesRosterRig){
   for(const auto material:racerVisuals[0]->flameMaterialIndices(component)){
    platform::VehicleWindowMaterialUv50DS uv{};
    uv.component=static_cast<VehicleDrawComponent50CM>(component);uv.materialIndex=material;
    playerFlameUvs.push_back(uv);
   }
  }else{
   platform::VehicleWindowMaterialUv50DS uv{};
   uv.component=static_cast<VehicleDrawComponent50CM>(component);uv.materialIndex=1;
   playerFlameUvs.push_back(uv);
  }
 }

 std::function<bool(std::uint64_t,bool,std::uint32_t&)> advanceOwnedListener;
 // HOST audio/world scheduling: once after each actual common16ms mix,
 // including countdown. Never rendered-frame, paused or per-racer updates.
 auto advanceSourceAudioWorld=[&](std::uint64_t beforeMix,bool heldSourceScene){
  if(!mode1Enabled&&!dynamicContactEnabled)return;
  // Dynamic update performs its own source activity read after the timer.
  // Legacy category6-only mode keeps the existing explicit completion poll.
  if(!dynamicContactEnabled)require50DU(raceMusic->pollCompletion(),
      "mode1 music transport ownership failed after shared mix");
  const auto afterMix=liveAudio->mixedFrames();
  require50DU(afterMix>=beforeMix&&(afterMix-beforeMix==0u||afterMix-beforeMix==768u),
      "mode1 shared mix must advance one actual768-frame step or none");
  if(afterMix==beforeMix)return;
  // Output may fail AFTER the actual mix. Source completion/update still runs
  // exactly once for those consumed frames before surfacing the output error.
  if(dynamicContactEnabled){
   //234440/235060 supplies the real target/duration. The explicitly selected
   // native envelope advances by shared-mixer consumed frames, separately
   // from original call acceptance; neither a wall timer nor a second mix.
   std::uint32_t listenerPC=0;
   if(!advanceOwnedListener||!advanceOwnedListener(beforeMix,heldSourceScene,listenerPC))
    dynamicBoundary(listenerPC,"actual native listener consumed-step prefix is terminal");
   const auto musicUpdate=raceMusic->update(.016f);
   if(!musicUpdate.completed())dynamicBoundary(musicUpdate.sourcePC,
       "collision music source/native transport continuation pending");
  }
  AnnouncerLeader50IE leader{};
  if(dynamicContactEnabled){
   require50DU(originalAiSession.rankBound,"dynamic audio needs actual original race rank");
   // Original 2C4DA0 walks every manager row and keeps the LAST row whose
   // place is 1 (no break, no uniqueness test). With no such row it keeps
   // its FLT_MAX/1e-6 seeds, so the <10 s leader tail is simply false. The
   // ranking 1B0814 can legitimately leave no place-1 row (e.g. a racer
   // driving the wrong way); neither case stops the race.
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(originalAiSession.rank[slot].placeCCC==1){
     leader.exists=true;leader.remainingDistance34=originalAiSession.rank[slot].remainingCC4;
     racerBank.visit(slot,[&](){leader.vehicle58=s.fields.vehicle58;leader.cachedSpeed34=m.cachedSpeed34;});
    }
   static bool leaderAbsentLogged50DU=false;
   if(!leader.exists&&!leaderAbsentLogged50DU){leaderAbsentLogged50DU=true;
    std::cerr<<"announcer_leader absent=1 (no place-1 row; original 2C4DA0 tail is false)\n";}
  }else if(race.racerCount){
   const auto slot=race.ordered[0];leader.exists=true;
   leader.vehicle58=slot==0?0u:1u; // HOST role, not arena player-physics gate.
   leader.remainingDistance34=static_cast<float>(race.racers[slot].remainingDistance);
   leader.cachedSpeed34=racerBank.visit(slot,[&](){return m.cachedSpeed34;});
   leader.hostRemainingProjection=true;
  }
  // Exact source remaining/speed<10 priority formula; remaining distance is
  // HOST interpolation, not original cross-section or ranking-tie arithmetic.
  require50DU(sourceAnnouncer->update(.016f,leader),"mode1 shared announcer update unresolved");
 };

 // Actual selected visual mesh, authenticated by its selected source profile.
 // Visual and collision member closures remain separate admissions.
 rr::assets::Mesh48B trackMesh;std::string trackLoadError;
 if(taOriginalDisc){
  require50DU(taOriginalWorld&&taOriginalWorld->mesh(),"retained TA original world unavailable");
  trackMesh=*taOriginalWorld->mesh();
 }
 if(gcOriginalDisc){
  require50DU(gcOriginalWorld&&gcOriginalWorld->mesh(),"retained GC original mesh publication missing");
  trackMesh=*gcOriginalWorld->mesh();
 }
 if(gaOriginalDisc){
  require50DU(gaOriginalWorld&&gaOriginalWorld->mesh(),"retained GA original mesh publication missing");
  trackMesh=*gaOriginalWorld->mesh();
 }
 if(maOriginalDisc){
  require50DU(maOriginalWorld&&maOriginalWorld->mesh(),"retained MA original mesh publication missing");
  trackMesh=*maOriginalWorld->mesh();
 }
 if(saOriginalDisc){
  require50DU(saOriginalWorld&&saOriginalWorld->mesh(),"retained SA original mesh publication missing");
  trackMesh=*saOriginalWorld->mesh();
 }
 std::unique_ptr<sc_original_world50LM::Owner> scOriginalWorld;
 if(scOriginalDisc){
  scOriginalWorld=std::make_unique<sc_original_world50LM::Owner>();
  sc_original_world50LM::Diagnostic diagnostic;
  const bool loaded=scOriginalWorld->load(runtime_startup50fl::readBytes(trackSession->archive),actualWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"SC original static world rejected":diagnostic.message.c_str());
  require50DU(scOriginalWorld->mesh(),"SC original mesh publication missing");
  trackMesh=*scOriginalWorld->mesh();
 }
 std::shared_ptr<tb_original_world50LM::Owner> tbOriginalWorld;
 if(isTbTrack){
  require50DU(tbColdSources&&tbColdSources->ready()&&tbColdSources->world().parameters.original_bytes==actualWorld.parameters.original_bytes,
      "TB actual compact source missing or world source differs");
  tbOriginalWorld=std::make_shared<tb_original_world50LM::Owner>();tb_original_world50LM::Diagnostic diagnostic;
  const bool loaded=tbOriginalWorld->load(tbColdSources,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"TB original world rejected":diagnostic.message.c_str());
  require50DU(tbOriginalWorld->mesh(),"TB original mesh publication missing");
  trackMesh=*tbOriginalWorld->mesh();
 }
 std::shared_ptr<rc_original_world50LM::Owner> rcOriginalWorld;
 if(isRcTrack){
  require50DU(rcColdSources&&rcColdSources->ready()&&rcColdSources->world().parameters.original_bytes==actualWorld.parameters.original_bytes,
      "RC visual source/world identity differs");
  const auto rawReservation=rc_original_world50LM::Owner::additionalRawReservationBytes();
  reserveRcAdditionalPayload(rawReservation,"RC crystal raw reservation exceeds remaining payload");
  rcOriginalWorld=std::make_shared<rc_original_world50LM::Owner>();
  rc_original_world50LM::Diagnostic diagnostic;
  const bool loaded=rcOriginalWorld->load(rcColdSources,rawReservation,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"RC original world rejected":diagnostic.message.c_str());
  require50DU(rcOriginalWorld->mesh(),"RC original mesh publication missing");
  trackMesh=*rcOriginalWorld->mesh();
 }
 std::unique_ptr<tc_original_world50LM::Owner> tcOriginalWorld;
 if(tcOriginalDisc){
  tcOriginalWorld=std::make_unique<tc_original_world50LM::Owner>();
  tc_original_world50LM::Diagnostic diagnostic;
  const bool loaded=tcOriginalWorld->load(runtime_startup50fl::readBytes(trackSession->archive),actualWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"TC original static world rejected":diagnostic.message.c_str());
  require50DU(tcOriginalWorld->mesh(),"TC original mesh publication missing");
  trackMesh=*tcOriginalWorld->mesh();
 }
 if(mbOriginalDisc){
  require50DU(mbOriginalWorld&&mbOriginalWorld->loaded()&&mbOriginalWorld->mesh(),
      "MB retained original visual owner missing");
  trackMesh=*mbOriginalWorld->mesh();
 }
 const bool trackLoaded=isRaTrack||directOriginalHuman||rr::assets::loadMesh48B(
     trackSession->visualMesh,trackMesh,trackLoadError);
 require50DU(trackLoaded,trackLoadError.empty()?"selected-track visual mesh load failed":trackLoadError.c_str());
 namespace trackTextures=track_materials50fa_fixture;
 auto trackMaterialOwner=std::make_unique<trackTextures::Owner>();
 const auto trackSource=isMcTrack?track_source_fixture::Track::mc:isGbTrack?track_source_fixture::Track::gb:isTbTrack?track_source_fixture::Track::tb:isRcTrack?track_source_fixture::Track::rc:isTcTrack?track_source_fixture::Track::tc:isScTrack?track_source_fixture::Track::sc:isMbTrack?track_source_fixture::Track::mb:isRaTrack?track_source_fixture::Track::ra:isMaTrack?track_source_fixture::Track::ma:isSaTrack?track_source_fixture::Track::sa:
     isGaTrack?track_source_fixture::Track::ga:
     isGcTrack?track_source_fixture::Track::gc:track_source_fixture::Track::ta;
 trackTextures::Request trackMaterialRequest{};
 trackMaterialRequest.track=trackSource;
 trackMaterialRequest.root=trackSession->visualRoot;
 trackMaterialRequest.exportReport=trackSession->visualReport;
 trackMaterialRequest.choice={1,1,trackSession->originalStaticCandidates?rr::assets::MaterialTextureRequirement50EA::required:
     rr::assets::MaterialTextureRequirement50EA::optionalUntextured,
     trackTextures::MipMode::largestRetained,0};
 trackMaterialRequest.coldSourceByteCapacity=isMaSaTrack?64u*1024u*1024u:32u*1024u*1024u;
 trackMaterialRequest.sourceArchive=trackSession->archive;
 trackMaterialRequest.decodeSourceTexture=isMcTrack?&mc_original_world50LM::decodeSourceTexture:isGbTrack?&gb_original_world50LM::decodeSourceTexture:isTbTrack?&tb_original_world50LM::decodeSourceTexture:isRcTrack?&rc_original_world50LM::decodeSourceTexture:tcOriginalDisc?&tc_original_world50LM::decodeSourceTexture:scOriginalDisc?&sc_original_world50LM::decodeSourceTexture:mbOriginalDisc?&mb_original_world50LM::decodeSourceTexture:taOriginalDisc?&ta_original_world50LE::decodeSourceTexture:gaOriginalDisc?&ga_original_world50LH::decodeSourceTexture:
     maOriginalDisc?&ma_original_world50LM::decodeSourceTexture:saOriginalDisc?&sa_original_world50LM::decodeSourceTexture:isRaTrack?nullptr:isMaTrack?&source_texture50fc_fixture::decodeSourceTextureMa:
     isSaTrack?&source_texture50fc_fixture::decodeSourceTextureSa:isGaTrack?&source_texture50fc_fixture::decodeSourceTextureGa:
     isGcTrack?&source_texture50fc_fixture::decodeSourceTextureGc:&source_texture50fc_fixture::decodeSourceTexture;
 trackMaterialRequest.includeAllAuthoredPasses=trackSession->originalStaticCandidates;
 trackMaterialRequest.allowStaticAnimationFrame=true;
 const std::size_t trackTextureBytes=isMaSaTrack?64u*1024u*1024u:isGaTrack?32u*1024u*1024u:8u*1024u*1024u;
 trackMaterialRequest.textureBudgets={isMaSaTrack?256u:isGaTrack?128u:trackSession->originalStaticCandidates?64u:32u,1024,
     trackTextureBytes,trackTextureBytes,1024};
 trackTextures::Diagnostic trackMaterialDiagnostic{};
 //27A208/27A234 initializes each existing static-model +148 to true.
 //271A00 applies numeric TOB overrides by the actual loaded model name.
 //Per-instance native projections preserve repeated placements of that model;
 //unmatched source names create neither render geometry nor source actors.
 std::vector<std::uint32_t> staticShadowReceiving(actualWorld.instances.size(),1u);
 if(isMaTrack){
  ma_static_model_params::Owner overrides;ma_static_model_params::Receipt receipt;
  std::string error;
  require50DU(maTriggerAuthoring&&overrides.load(maTriggerAuthoring->originalTob(),error),
      error.empty()?"MA original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<ma_static_model_params::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"MA actual static-model property application rejected");
 }
 if(isMbTrack){
  mb_static_model_params::Owner overrides;mb_static_model_params::Receipt receipt;
  std::string error;
  require50DU(mbCollisionWorld&&overrides.load(mbCollisionWorld->originalTob(),error),
      error.empty()?"MB original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<mb_static_model_params::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"MB actual static-model property application rejected");
 }
 if(isScTrack){
  sc_static_model_params::Owner overrides;sc_static_model_params::Receipt receipt;
  std::string error;
  const bool loaded=scTriggerAuthoring&&overrides.load(scTriggerAuthoring->originalTob(),error);
  require50DU(loaded,error.empty()?"SC original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<sc_static_model_params::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"SC actual static-model property application rejected");
 }
 if(isTbTrack){
  reserveTbAdditionalPayload(tb_static_model_params50TB::Owner::maximumAdditionalPayloadBytes(),"TB static property reservation exceeds remaining payload");
  tb_static_model_params50TB::Owner overrides;tb_static_model_params50TB::Receipt receipt;
  std::string error;
  const bool loaded=tbColdSources&&overrides.load(tbColdSources,error);
  require50DU(loaded,error.empty()?"TB original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<tb_static_model_params50TB::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"TB actual static-model property application rejected");
 }
 if(isGbTrack){
  reserveGbAdditionalPayload(gb_static_model_params50GB::Owner::maximumAdditionalPayloadBytes(),"GB static property reservation exceeds remaining payload");
  gb_static_model_params50GB::Owner overrides;gb_static_model_params50GB::Receipt receipt;
  std::string error;
  const bool loaded=gbColdSources&&overrides.load(gbColdSources,error);
  require50DU(loaded,error.empty()?"GB original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<gb_static_model_params50GB::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"GB actual static-model property application rejected");
 }
 if(isMcTrack){
  reserveMcAdditionalPayload(mc_static_model_params50MC::Owner::maximumAdditionalPayloadBytes(),"MC static property reservation exceeds remaining payload");
  mc_static_model_params50MC::Owner overrides;mc_static_model_params50MC::Receipt receipt;
  std::string error;
  const bool loaded=mcColdSources&&overrides.load(mcColdSources,mc_static_model_params50MC::Owner::maximumAdditionalPayloadBytes(),error);
  require50DU(loaded,error.empty()?"MC original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<mc_static_model_params50MC::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"MC actual static-model property application rejected");
 }
 if(isMcTrack){ // All override/name/ModelLease temporaries above are destroyed.
  const auto released=mc_static_model_params50MC::Owner::maximumAdditionalPayloadBytes();
  require50DU(mcAdditionalPayloadReserved>=released,"MC static property scratch release accounting");
  mcAdditionalPayloadReserved-=released;
 }
 if(isRcTrack){
  reserveRcAdditionalPayload(rc_static_model_params50RC::Owner::maximumAdditionalPayloadBytes(),"RC static property reservation exceeds remaining payload");
  rc_static_model_params50RC::Owner overrides;rc_static_model_params50RC::Receipt receipt;
  std::string error;
  const bool loaded=rcColdSources&&overrides.load(rcColdSources,error);
  require50DU(loaded,error.empty()?"RC original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<rc_static_model_params50RC::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"RC actual static-model property application rejected");
 }
 if(isTcTrack){
  tc_static_model_params50TC::Owner overrides;tc_static_model_params50TC::Receipt receipt;
  std::string error;
  const bool loaded=tcColdSources&&overrides.decode(tcColdSources,error);
  require50DU(loaded,error.empty()?"TC original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<tc_static_model_params50TC::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"TC actual static-model property application rejected");
 }
 if(isGcTrack){
  gc_static_model_params50KG::Owner overrides;gc_static_model_params50KG::Receipt receipt;
  std::string error;
  require50DU(gcTriggerAuthoring&&overrides.load(gcTriggerAuthoring->originalTob(),error),
      error.empty()?"GC original static-model parameters rejected":error.c_str());
  std::vector<std::string> names(actualWorld.instances.size());
  std::vector<gc_static_model_params50KG::ModelLease> models(actualWorld.instances.size());
  for(std::size_t i=0;i<actualWorld.instances.size();++i){
   const auto& member=actualWorld.instances[i].geometry_member;
   const auto slash=member.find_last_of("/\\");
   names[i]=member.substr(slash==std::string::npos?0:slash+1);
   const auto dot=names[i].find_last_of('.');if(dot!=std::string::npos)names[i].resize(dot);
   models[i]={&names[i],&staticShadowReceiving[i],1u};
  }
  require50DU(overrides.apply(models.data(),models.size(),receipt),"GC actual static-model property application rejected");
 }
 std::vector<rr::assets::WorldNormal50HK> legacyTrackNormals;
 const auto& trackNormals=mcOriginalWorld?*mcOriginalWorld->normals():gbOriginalWorld?*gbOriginalWorld->normals():tbOriginalWorld?*tbOriginalWorld->normals():rcOriginalWorld?*rcOriginalWorld->normals():tcOriginalWorld?*tcOriginalWorld->normals():scOriginalWorld?*scOriginalWorld->normals():mbOriginalWorld?*mbOriginalWorld->normals():taOriginalWorld?*taOriginalWorld->normals():gcOriginalWorld?*gcOriginalWorld->normals():gaOriginalWorld?*gaOriginalWorld->normals():
     maOriginalWorld?*maOriginalWorld->normals():saOriginalWorld?*saOriginalWorld->normals():legacyTrackNormals;
 track_normals50hk_fixture::Diagnostic trackNormalDiagnostic{};
 require50DU(isRaTrack||directOriginalHuman||track_normals50hk_fixture::load(trackMaterialRequest.root,
     trackMaterialRequest.exportReport,trackMesh,legacyTrackNormals,trackNormalDiagnostic,trackSource),
     trackNormalDiagnostic.message.empty()?"selected-track authored normal load failed":trackNormalDiagnostic.message.c_str());
 std::cerr<<trackSession->world->code<<" source normals "<<trackNormalDiagnostic.normals
     <<" zero/fallback "<<trackNormalDiagnostic.zeroNormals<<'\n';
 auto advanceNativeGcWater=[&](){
  if(mcWaterOwner){
   require50DU(mcWaterOwner->ready()&&mcWaterOwner->sourceIdentity()==mcColdSources.get()&&
       mcWaterUpdateSequence<std::numeric_limits<std::uint64_t>::max(),"MC actual water update owner rejected");
   const auto node=static_cast<std::uint32_t>(x.bindings.animation.bodyScene288);
   require50DU(node<s.sceneNodeCount,"MC followed water anchor scene missing");
   float position[3];std::memcpy(position,s.nodes[node].localC0.v+12,12);
   const WaterPresentationView50JC view{position,0,true,true};
   require50DU(mcWaterOwner->runtime().anchor(view).status==native_water_runtime50JC::Status::Complete&&
       mcWaterOwner->runtime().advance(++mcWaterUpdateSequence,.016f,0u).status==native_water_runtime50JC::Status::Complete,
       "MC actual water root update rejected");
  }
  if(!nativeGcWater)return;
  // Outside every bank lease, the canonical live arena is actual followed
  // racer0. Copy the lanes before lending them to shared world owners.
  const auto node=static_cast<std::uint32_t>(x.bindings.animation.bodyScene288);
  require50DU(node<s.sceneNodeCount,"followed water anchor scene missing");
  float position[3];std::memcpy(position,s.nodes[node].localC0.v+12,12);
  require50DU(nativeGcWater->advance(.016f,0u,position,s.noiseOwner().sharedRandomState,
      nativeFireSerial,sessionRandom,applicationClock),"native GC particle/water root update rejected");
 };
 std::vector<std::uint32_t> nativeFireSpanIds;
 std::vector<std::uint8_t> nativeFireSpanDrawn,nativeFireSpanOwner;
 sc_zero_sky50KX::Owner scZeroSky;
 if(isScTrack){
  std::string error;const bool ready=scZeroSky.prepare(scColdSources,error);
  require50DU(ready,error.empty()?"SC zero-sky authoring rejected":error.c_str());
 }
 // Late environment storage participates in the parent's same MC payload ledger.
 const auto reserveMcEnvironment=[&](std::size_t bytes,const char* reason){
  require50DU(isMcTrack&&bytes<=mcRemainingPayloadBytes(),reason);
  mcAdditionalPayloadReserved+=bytes;
 };
 std::unique_ptr<mc_sky50MC::Owner> mcSkyOwner;
 if(isMcTrack){
  reserveMcEnvironment(sizeof(mc_sky50MC::Owner),"MC sky owner exceeds remaining payload");
  mcSkyOwner=std::make_unique<mc_sky50MC::Owner>();
 }
 auto skyOwner=std::make_unique<sky50gw_fixture::Owner>();
 sky50gw_fixture::Request skyRequest{};skyRequest.track=trackSource;
 skyRequest.archive=trackSession->archive;
 skyRequest.textureRoot=trackSession->skyTextureRoot;
 skyRequest.decodeOriginalImages=directOriginalHuman;
 skyRequest.retainedMbWorld=isMbTrack?mbOriginalWorld.get():nullptr;
 skyRequest.retainedTcWorld=isTcTrack?tcOriginalWorld.get():nullptr;
 skyRequest.retainedRcSource=isRcTrack?rcColdSources.get():nullptr;
 skyRequest.retainedTbSource=isTbTrack?tbColdSources.get():nullptr;
 skyRequest.retainedGbSource=isGbTrack?gbColdSources:nullptr;
 sky50gw_fixture::Diagnostic skyDiagnostic{};
 if(isRaTrack){
  ra_cold_aggregate50LA::Diagnostic raSkyDiagnostic;
  require50DU(raWorld&&raWorld->prepareSkyImages(
      runtime_asset_paths50LB::resolve("evidence/skyRA"),raSkyDiagnostic),
      "actual retained RA authored sky image preparation failed");
 }else if(isMcTrack){
  // Bounded native texture load; all decoded images remain with this owner
  // through the existing renderer/GPU-texture shutdown fence.
  require50DU(mcRemainingPayloadBytes()>=48u*(65552u+65536u)+194342u,
      "MC sky decoded/source bytes exceed remaining payload");
  const bool loaded=mcSkyOwner->prepare(mcColdSources,skyDiagnostic);
  require50DU(loaded,skyDiagnostic.message.c_str());
  reserveMcEnvironment(mcSkyOwner->retainedPayloadBytes(),"MC retained sky exceeds remaining payload");
 }else if(isTcTrack||isRcTrack||isTbTrack||isGbTrack){
  const bool loaded=skyOwner->load(skyRequest,skyDiagnostic);
  require50DU(loaded,skyDiagnostic.message.empty()?"TC own sky images rejected":skyDiagnostic.message.c_str());
 }else if(!isScTrack)require50DU(skyOwner->load(skyRequest,skyDiagnostic),
     skyDiagnostic.message.empty()?"actual selected-track sky load failed":skyDiagnostic.message.c_str());
 original_track_sky50KX::Owner sourceSkySelection;
 if(isMaSaTrack||isMbTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack){
  std::string skySelectionError;
  const bool skySelected=isMcTrack?sourceSkySelection.prepareMcSource(*mcColdSources,skySelectionError):isGbTrack?sourceSkySelection.prepareGbSource(*gbColdSources,skySelectionError):isTbTrack?sourceSkySelection.prepareTbSource(*tbColdSources,skySelectionError):isRcTrack?sourceSkySelection.prepareRcSource(*rcColdSources,skySelectionError):isTcTrack?sourceSkySelection.prepareTcSource(*tcOriginalWorld,skySelectionError):
      (isMbTrack?sourceSkySelection.prepareMbSource(*mbOriginalWorld,skySelectionError):
      sourceSkySelection.prepare(actualWorld,trackSource,skySelectionError));
  require50DU(skySelected,skySelectionError.empty()?"source zone-selected sky authoring rejected":skySelectionError.c_str());
 }
 // Borrow the same application Video on each cold race. Local renderer/GPU
 // owners still close before its real host; a later session reopens that host
 // only after cleanup, without replacing the application's Video identity.

 gc_dynamic_window50IG::Owner gcDynamicWindow;
 ga_dynamic_window::Owner gaDynamicWindow;
 tc_dynamic_window50TC::Owner tcDynamicWindow;
 tb_dynamic_window50TB::Owner tbDynamicWindow;
 gb_dynamic_window50GB::Owner gbDynamicWindow;
 mc_dynamic_window50MC::Owner mcDynamicWindow;
 sa_dynamic_window::Owner saDynamicWindow;
 gc_atm_window_fixture::Owner gcAtm;
 ga_atm_window_fixture::Owner gaAtm;
 ma_sa_atm_window_fixture::Owner maSaAtm;
 ma_shield_uv_fixture::Owner maShieldUv;
 tb_atm_window_fixture::Owner tbAtm;
 gb_atm_window_fixture::Owner gbAtm;
 mc_atm_window_fixture::Owner mcAtm;
 mc_shield_uv_fixture::Owner mcShieldUv;
 tb_shield_uv_fixture::Owner tbShieldUv;
 rc_atm_window_fixture::Owner rcAtm;
 std::shared_ptr<rc_crystal50RC::Owner> rcCrystal;
 tc_atm_window_fixture::Owner tcAtm;
 tc_shield_uv_fixture::Owner tcShieldUv;
 sc_atm_window_fixture::Owner scAtm;
 sc_lava_uv_fixture::Owner scLavaUv;
 mb_atm_window_fixture::Owner mbAtm;
 mb_shield_uv_fixture::Owner mbShieldUv;
 gameplay_binder_window_candidate_fixture::Owner gameplayBinder;
 gc_particle_materials50kg_fixture::Owner gcParticleMaterials;
 OriginalGcParticleRenderOwner50KG gcParticleRender;
 // Same process normal-material mode AND retained APPSTART template objects
 // as Main. Direct gameplay prepares them on first use from its admitted disc
 // tree. These local handles borrow immutable application-lifetime backing;
 // they never create a replacement ScreenMT or reset it on race restart.
 const bool gameplayBinderEnabled=raceEnabled&&playerUsesRosterRig&&nativeSelectedLights&&
     !checkpointInput&&(!originalUiActive||courseRaceCold);
 rr::assets::OriginalUiMaterialTemplates50KG gameplayBinderTemplates;
 std::shared_ptr<const rr::assets::OriginalUiArchive50KG> gameplayBinderAppStart;
 if(gameplayBinderEnabled){
  require50DU(((isGaTrack&&gaAnimatedWorld)||(isGcTrack&&gcAnimatedWorld)||
      (isTaTrack&&nativeTaSelection)||(isMaTrack&&nativeMaSelection)||(isSaTrack&&nativeSaSelection)||(isMbTrack&&nativeMbSelection)||(isScTrack&&nativeScSelection)||(isTcTrack&&nativeTcSelection)||(isRcTrack&&nativeRcSelection)||(isTbTrack&&nativeTbSelection)||(isGbTrack&&nativeGbSelection)||(isMcTrack&&nativeMcSelection)||
      (isRaTrack&&nativeRaSelection&&originalCameraEnabled))&&racerBank.count()==raceRacerCount,
      "Binder requires an actual eight-racer authenticated ordinary session");
  gameplayBinderAppStart=originalMainApplication.appStart(
      runtime_asset_paths50LB::resolve("work/native/project/disc"));
  rr::assets::OriginalUiMaterialDiagnostic50KG materialDiagnostic;
  require50DU(gameplayBinderTemplates.load(*gameplayBinderAppStart,materialDiagnostic),
      "Binder authentic DefaultMT owner rejected");
  const gameplay_binder_window_candidate_fixture::SourceDependencies gameplayBinderDependencies{
      gameplayBinderTemplates,originalMainApplication.materialRuntime};
  gameplay_binder_candidate_fixture::Diagnostic diagnostic;
  for(std::size_t racer=0;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
   const bool bound=gameplayBinder.bindRacer(static_cast<std::uint32_t>(racer),*racerRigs[racer],s,x.bindings,
       gameplayBinderDependencies,diagnostic);
   require50DU(bound,diagnostic.message.empty()?"actual racer Binder source join failed":diagnostic.message.c_str());
  });
 }
 if(nativeGcWater){
  require50DU(gameplayBinderEnabled&&gameplayBinder.loaded(),
      "GC particles require the actual gameplay DefaultMT/runtime owners");
  gc_particle_materials50kg_fixture::Diagnostic diagnostic;
  require50DU(gcParticleMaterials.load(nativeGcWater->particlePsm(),nativeGcWater->particlePst(),
      gameplayBinderTemplates,originalMainApplication.materialRuntime,diagnostic),
      diagnostic.message.empty()?"original GC particle material/atlas rejected":diagnostic.message.c_str());
  require50DU(gcParticleRender.prepare(nativeGcWater->particles())==OriginalGcParticleRenderStatus50KG::complete,
      "actual GC shared-manager render allocation rejected");
 }

 if(gcAnimatedWorld){
  gc_dynamic_materials_fixture::Diagnostic materialDiagnostic{};
  // Finish each cold call before reading its diagnostic: function argument
  // evaluation order must not select the fallback from a still-empty message.
  const bool gcMaterialsLoaded=gcDynamicWindow.loadSource(trackSession->archive,*gcAnimatedWorld,materialDiagnostic);
  if(!gcMaterialsLoaded)std::cerr<<"GC dynamic material load diagnostic material="<<materialDiagnostic.material
      <<" source_code="<<materialDiagnostic.source.code<<" source_offset="<<materialDiagnostic.source.offset
      <<" source_detail="<<materialDiagnostic.source.detail
      <<" bank_error="<<static_cast<unsigned>(materialDiagnostic.bank.error)
      <<" bank_texture="<<materialDiagnostic.bank.texture<<" bank_slot="<<materialDiagnostic.bank.slot
      <<" bank_offset="<<materialDiagnostic.bank.byteOffset<<" bank_detail="<<materialDiagnostic.bank.parserMessage<<'\n';
  require50DU(gcMaterialsLoaded,
      materialDiagnostic.message.empty()?"original GC dynamic materials rejected":materialDiagnostic.message.c_str());
 }
 if(gaAnimatedWorld){
  ga_dynamic_materials::Diagnostic materialDiagnostic{};
  const bool loaded=gaDynamicWindow.loadSource(trackSession->archive,*gaAnimatedWorld,
      ga_original_world50LH::decodeSourceTexture,materialDiagnostic);
  if(!loaded)std::cerr<<"GA dynamic material load diagnostic material="<<materialDiagnostic.material
      <<" source_code="<<materialDiagnostic.source.code<<" source_offset="<<materialDiagnostic.source.offset
      <<" source_detail="<<materialDiagnostic.source.detail
      <<" bank_error="<<static_cast<unsigned>(materialDiagnostic.bank.error)<<'\n';
  require50DU(loaded,materialDiagnostic.message.empty()?"original GA dynamic materials rejected":materialDiagnostic.message.c_str());

 }
 if(saAnimatedWorld){
  sa_dynamic_materials::Diagnostic diagnostic;
  const bool loaded=saOriginalDisc?
      saDynamicWindow.loadSource(trackSession->archive,*saAnimatedWorld,sa_original_world50LM::decodeSourceTexture,diagnostic):
      saDynamicWindow.loadSource(trackSession->archive,*saAnimatedWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"SA actual dynamic materials rejected":diagnostic.message.c_str());
 }
 if(isTcTrack){
  require50DU(tcGateRuntime&&tcGateRuntime->ready(),"TC actual gate runtime absent before material preparation");
  tc_dynamic_materials50TC::Diagnostic diagnostic;
  const bool loaded=tcDynamicWindow.loadSource(tcGateRuntime,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"TC actual gate materials rejected":diagnostic.message.c_str());
 }
 if(isTbTrack){
  require50DU(tbDoorRuntime&&tbDoorRuntime->ready(),"TB actual door runtime absent before material preparation");
  tb_dynamic_materials50TB::Diagnostic diagnostic;
  const bool loaded=tbDynamicWindow.loadSource(tbDoorRuntime,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"TB actual door materials rejected":diagnostic.message.c_str());
 }
 if(isGbTrack){
  require50DU(gbAnimationRuntime&&gbAnimationRuntime->ready(),"GB actual four-animation runtime absent before material preparation");
  gb_dynamic_materials50GB::Diagnostic diagnostic;
  const bool loaded=gbDynamicWindow.loadSource(gbAnimationRuntime,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"GB actual four-animation materials rejected":diagnostic.message.c_str());
 }
 if(isMcTrack){
  reserveMcAdditionalPayload(mc_dynamic_materials50MC::Owner::maximumPayloadReservation(),"MC dynamic source/material scratch exceeds remaining payload");
  require50DU(mcAnimationRuntime&&mcAnimationRuntime->ready(),"MC actual fourteen-animation runtime absent before material preparation");
  mc_dynamic_materials50MC::Diagnostic diagnostic;
  const bool loaded=mcDynamicWindow.loadSource(mcAnimationRuntime,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"MC actual fourteen-animation materials rejected":diagnostic.message.c_str());
 }
 // One application publication survives frontend/race destruction. These
 // consumers only borrow its live view; no runSession-local clock authority.
 const auto sourceApplicationClock=originalMainApplication.animationClock();
 std::uint64_t gcAnimatedWorldSteps=0,gaAnimatedWorldSteps=0,saAnimatedWorldSteps=0,gbWorldServiceStep=0,mcWorldServiceStep=0;
 auto advanceWorldAnimations=[&](){
  if(!isGbTrack&&!isMcTrack&&!tcGateRuntime&&!isRcTrack&&!tbDoorRuntime){
  if(!gcAnimatedWorld&&!gaAnimatedWorld&&!saAnimatedWorld&&!gaAtm.loaded()&&!maSaAtm.loaded()&&!mbAtm.loaded()&&!scAtm.loaded()&&!tcAtm.loaded()&&!rcAtm.loaded()&&!tbAtm.loaded()&&!gameplayBinder.loaded())return;
  }
  // Source1AAD98 registers the complete racer group before1B4A58's animation
  // service. Current HOST participant order stays explicit; world service runs
  // once after that group, including enabled countdown world traversal.
  // TA has no owner in these animated-world collections; its real Binder
  // still consumes this shared timestamp publication at render preparation.
  const auto before=::allocations;
  std::uint32_t sourceApplicationTimestamp=0;
  require50DU(originalMainApplication.sampleAnimationTimestamp(sourceApplicationTimestamp),
      "shared source application timestamp rejected");
  if(isMcTrack){
   const bool published=mcAnimationBinding&&mcAnimationBinding->service(.016f,sourceApplicationTimestamp,
       s.noiseOwner().sharedRandomState,sessionRandom,s.animationShared.motion,mcAnimationBindingDiagnostic);
   require50DU(published,mcAnimationBindingDiagnostic.message.c_str());
  }
  if(isGbTrack){
   std::uint32_t actualPause=0;
   require50DU(sourceApplicationClock.readPause11C(actualPause),"GB actual world pause unavailable");
   require50DU(gbLightOwner&&gbLightOwner->serviceGbTimedLights(.016f,s.noiseOwner().sharedRandomState,
       actualPause!=0,gcLightDiagnostic),"GB actual timed light service rejected");
   const bool animationPublished=gbAnimationBinding&&gbAnimationBinding->service(.016f,sourceApplicationTimestamp,
       s.noiseOwner().sharedRandomState,sessionRandom,s.animationShared.motion,gbAnimationBindingDiagnostic);
   require50DU(animationPublished,gbAnimationBindingDiagnostic.message.empty()?"GB actual four-animation world publication rejected":gbAnimationBindingDiagnostic.message.c_str());
   const bool particlesAdvanced=gbParticleRender&&gbParticleRender->advance(.016f,gbParticleRuntimeDiagnostic);
   require50DU(particlesAdvanced,gbParticleRuntimeDiagnostic.message.empty()?"GB actual shared manager and source-order emitter service rejected":gbParticleRuntimeDiagnostic.message.c_str());
   require50DU(gbWorldServiceStep!=std::numeric_limits<std::uint64_t>::max()&&gbQueryBinding&&
       gbQueryBinding->serviceWorld(++gbWorldServiceStep),"GB actual trigger exit service rejected");
  }
  if(isMcTrack){
   const bool particlesAdvanced=mcParticleRender&&mcParticleRender->advance(.016f,mcParticleRuntimeDiagnostic);
   require50DU(particlesAdvanced,mcParticleRuntimeDiagnostic.message.empty()?"MC actual shared manager/source-order40 emitter service rejected":mcParticleRuntimeDiagnostic.message.c_str());
  }
  if(tbDoorRuntime){
   const bool advanced=tbDoorRuntime->service(.016f,sourceApplicationTimestamp,s.noiseOwner().sharedRandomState,
       sessionRandom,s.animationShared.motion,tbDoorDiagnostic);
   require50DU(advanced,tbDoorDiagnostic.message.empty()?"TB original door world service rejected":tbDoorDiagnostic.message.c_str());
   const bool registered=tbDoorBinding&&tbDoorBinding->update(tbDoorBindingDiagnostic);
   require50DU(registered,"TB actual visual/collision door update rejected");
  }
  if(tcGateRuntime){
   const bool advanced=tcGateRuntime->service(.016f,sourceApplicationTimestamp,s.noiseOwner().sharedRandomState,
       sessionRandom,s.animationShared.motion,tcGateDiagnostic);
   require50DU(advanced,tcGateDiagnostic.message.empty()?"TC original gate world service rejected":tcGateDiagnostic.message.c_str());
   const bool registered=tcGateVisibility&&tcGateVisibility->update(tcGateVisibilityDiagnostic);
   require50DU(registered,"TC actual101788 gate observer update rejected");
  }
  if(gcAnimatedWorld)require50DU(gcAnimatedWorld->service(.016f,sourceApplicationTimestamp,s.noiseOwner().sharedRandomState,
      sessionRandom,s.animationShared.motion,gcAnimationDiagnostic),
      gcAnimationDiagnostic.message.empty()?"original GC world animation rejected":gcAnimationDiagnostic.message.c_str());
  if(gaAnimatedWorld){
   const bool advanced=gaAnimatedWorld->service(.016f,sourceApplicationTimestamp,s.noiseOwner().sharedRandomState,
       sessionRandom,s.animationShared.motion,gaAnimationDiagnostic);
   require50DU(advanced,gaAnimationDiagnostic.message.empty()?"original GA world animation rejected":gaAnimationDiagnostic.message.c_str());
  }
  if(saAnimatedWorld){
   const bool advanced=saAnimatedWorld->service(.016f,sourceApplicationTimestamp,s.noiseOwner().sharedRandomState,
       sessionRandom,s.animationShared.motion,saAnimationDiagnostic);
   require50DU(advanced,saAnimationDiagnostic.message.empty()?"original SA world animation rejected":saAnimationDiagnostic.message.c_str());
  }
  if(gcAtm.loaded())require50DU(gcAtm.advance(sourceApplicationTimestamp),"original GC ATM time update rejected");
  if(gaAtm.loaded())require50DU(gaAtm.advance(sourceApplicationTimestamp),"original GA ATM time update rejected");
  if(maSaAtm.loaded())require50DU(maSaAtm.advance(sourceApplicationTimestamp),"original MA/SA ATM time update rejected");
  if(mbAtm.loaded())require50DU(mbAtm.advance(sourceApplicationTimestamp),"original MB ATM time update rejected");
  if(scAtm.loaded())require50DU(scAtm.advance(sourceApplicationTimestamp),"original SC ATM time update rejected");
  if(tbAtm.loaded())require50DU(tbAtm.advance(sourceApplicationTimestamp),"original TB ATM time update rejected");
  if(mcAtm.loaded())require50DU(mcAtm.advance(sourceApplicationTimestamp),"MC actual ATM time update rejected");
  if(isMcTrack)require50DU(mcWorldServiceStep!=std::numeric_limits<std::uint64_t>::max()&&mcQueryBinding&&mcQueryBinding->serviceWorld(++mcWorldServiceStep),"MC actual trigger exit service rejected");
  if(gbAtm.loaded())require50DU(gbAtm.advance(sourceApplicationTimestamp),"original GB ATM time update rejected");
  if(rcAtm.loaded())require50DU(rcAtm.advance(sourceApplicationTimestamp),"original RC ATM time update rejected");
  if(tcAtm.loaded())require50DU(tcAtm.advance(sourceApplicationTimestamp),"original TC ATM time update rejected");
  if(isRaTrack)require50DU(raWorld->serviceRuntimeTriggers(racerBank,raDiagnostic),
      "RA original trigger exit service rejected actual committed COL poses");
  if(gcTriggerActions)require50DU(gcTriggerActions->serviceWorld()==gc_trigger_actions50KG::Status::Complete,
      "GC original coarse trigger exit service rejected actual COL poses");
  if(mbSourceTriggers)require50DU(mbSourceTriggers->serviceWorld(),"MB actual source trigger exit service rejected");
  if(tbTriggerActions)require50DU(tbTriggerActions->serviceWorld()==tb_trigger_actions50TB::Status::Complete,
      "TB actual trigger exit service rejected; terminal action prefixes retained");
  if(tcTriggerActions)require50DU(tcTriggerActions->serviceWorld()==tc_trigger_actions50TC::Status::Complete,
      "TC actual trigger exit service rejected; terminal action prefixes retained");
  if(isRcTrack){
   requireMbRuntimeNavigation50MB();
   require50DU(rcTriggerActions&&rcTriggerActions->serviceWorld()==rc_trigger_actions::Status::Complete,
       "RC actual trigger exit service rejected prepared COL poses");
  }
  if(scTriggerActions)require50DU(scTriggerActions->serviceWorld()==sc_trigger_actions::Status::Complete,
      "SC actual trigger exit service rejected prepared COL poses");
  if(maTriggerActions)require50DU(maTriggerActions->serviceWorld()==ma_trigger_actions::Status::Complete,
      "MA actual trigger exit service rejected COL poses");
  if(saTriggerActions)require50DU(saTriggerActions->serviceWorld()==sa_trigger_actions::Status::Complete,
      "SA actual trigger exit service rejected COL poses");
  if(gaTriggerActions)require50DU(gaTriggerActions->serviceWorld()==ga_trigger_actions::Status::Complete,
      "GA original coarse trigger exit service rejected actual COL poses");
  require50DU(::allocations==before,"successful original world animation allocated C++ storage");
  if(gcAnimatedWorld)++gcAnimatedWorldSteps;
  if(gaAnimatedWorld)++gaAnimatedWorldSteps;
  if(saAnimatedWorld)++saAnimatedWorldSteps;
 };
 platform::MaterialTextureGl50EB gpuTextures,trackGpuTextures,cableGpuTextures,skyGpuTextures;
 platform::MaterialTextureGl50EB specularGpuTextures50LA;
 std::array<platform::MaterialTextureGl50EB,raceOpponentCapacity> opponentGpuTextures,opponentCableGpuTextures;
 std::array<platform::MaterialTextureGl50EB,raceOpponentCapacity> opponentSpecularGpuTextures50LA;
 platform::MaterialTextureGlDiagnostic50EB textureDiagnostic{};
 auto& renderer=sourceDisplayLifetime->renderer();
 // Borrowed by Session cold teardown, including partial-construction unwind.
 platform::VehicleWindowDiagnostic50DS rendererDiagnostic{},completionDiagnostic{};
 // The canonical TA4A0 provider outlives every Session racer borrowing it.
 const bool humanOriginalHud=directOriginalHuman&&humanCourseEnabled&&originalCameraEnabled&&bool(nativeModelDomain);
 require50DU(!directOriginalHuman||humanOriginalHud,"direct human world requires the real canonical HUD/material domain");
 std::shared_ptr<rc_particle_visual50RC::Window> rcParticleWindow;
 OriginalTrackParticleCold50LA nativeTaParticles;
 OriginalTrackParticleColdDiagnostic50LA nativeTaParticleDiagnostic;
 // TB has no declared ambient emitter list. The real Session racers borrow
 // this one canonical track pool; its existing fixed5000 atlas/pool domain is unchanged.
 std::unique_ptr<OriginalTrackParticleCold50LA> tbTrackParticles;
 std::size_t tbParticleRegistryPayloadCharged=0;
 bool tbParticleRegistryFinalAccounted=false;
 const auto accountTbParticleRegistry=[&](){
  require50DU(isTbTrack&&tbTrackParticles&&tbTrackParticles->prepared()&&tbTrackParticles->particles(),
      "TB registry accounting requires the actual canonical pool");
  const auto actual=tbTrackParticles->particles()->emitterStoragePayloadBytes();
  require50DU(actual>=tbParticleRegistryPayloadCharged,"TB shared registry capacity regressed before retirement");
  reserveTbAdditionalPayload(actual-tbParticleRegistryPayloadCharged,"TB actual shared registry exceeds remaining payload");
  tbParticleRegistryPayloadCharged=actual;
 };
 const auto advanceTbTrackParticles=[&](){
  if(!isTbTrack)return;
  requireMbRuntimeNavigation50MB();
  require50DU(tbTrackParticles&&tbTrackParticles->prepared()&&tbTrackParticles->particles()&&
      tbParticleRegistryFinalAccounted&&
      tbTrackParticles->particles()->emitterStoragePayloadBytes()==tbParticleRegistryPayloadCharged,
      "TB actual canonical particle pool service is unavailable");
  std::uint32_t actualPause11C=0;
  require50DU(sourceApplicationClock.readPause11C(actualPause11C),"TB actual Application pause owner unavailable");
  const auto advanced=tbTrackParticles->particles()->advance(.016f,actualPause11C!=0);
  require50DU(advanced.okay(),"TB actual canonical particle manager advance rejected");
 };
 // RC uses one actual canonical pool for Session racer borrows and all
 // source emitters. These handles precede service/cleanup lambdas; preparation
 // happens only inside the existing closeWindow-protected setup interval.
 std::shared_ptr<OriginalTrackParticleCold50LA> rcTrackParticles;
 std::shared_ptr<rc_particle_authoring50RC::Owner> rcParticleAuthoring;
 std::shared_ptr<rc_particle_attachments50RC::Owner> rcParticleAttachments;
 std::shared_ptr<rc_particle_runtime50RC::Owner> rcParticleRuntime;
 std::shared_ptr<rc_particle_render50RC::Owner> rcParticleRender;
 rc_particle_authoring50RC::Diagnostic rcParticleAuthoringDiagnostic;
 rc_particle_attachments50RC::Diagnostic rcParticleAttachmentDiagnostic;
 rc_particle_runtime50RC::Diagnostic rcParticleRuntimeDiagnostic;
 rc_particle_render50RC::Diagnostic rcParticleRenderDiagnostic;
 gb_particle_render50GB::Diagnostic gbParticleRenderDiagnostic;
 mc_particle_render50MC::Diagnostic mcParticleRenderDiagnostic;
 bool mcParticleFrame152Notice=false;
 std::size_t rcParticleWindowPayloadReservation=0;
 std::size_t rcParticleRegistryPayloadCharged=0;
 bool rcParticleRegistryFinalAccounted=false;
 const auto accountRcParticleRegistry=[&](){
  require50DU(isRcTrack&&rcTrackParticles&&rcTrackParticles->prepared()&&rcTrackParticles->particles(),
      "RC registry accounting requires the actual shared pool");
  const auto actual=rcTrackParticles->particles()->emitterStoragePayloadBytes();
  require50DU(actual>=rcParticleRegistryPayloadCharged,"RC shared registry capacity regressed before retirement");
  reserveRcAdditionalPayload(actual-rcParticleRegistryPayloadCharged,"RC actual shared registry exceeds remaining payload");
  rcParticleRegistryPayloadCharged=actual;
 };
 const auto advanceRcTrackParticles=[&](){
  if(!isRcTrack)return;
  require50DU(rcParticleRuntime&&rcParticleRuntime->active()&&rcParticleWindow&&rcParticleWindow->ready()&&
      rcParticleRegistryFinalAccounted,"RC actual emitter/Window/pool service is unavailable");
  const bool advanced=rcParticleRuntime->advance(.016f,rcParticleRuntimeDiagnostic);
  if(!advanced)std::cerr<<"rc_emitter_update_rejected child="<<rcParticleRuntimeDiagnostic.child
      <<" source_pc="<<rcParticleRuntimeDiagnostic.source.sourcePC
      <<" status="<<static_cast<unsigned>(rcParticleRuntimeDiagnostic.source.status)
      <<" message="<<rcParticleRuntimeDiagnostic.message<<'\n';
  require50DU(advanced,"RC actual source emitter update failed; prefixes retained for cleanup");
 };
 namespace sourceCamera=original_camera_session50KX;
 // Explicit native root policy: genuine ALL8 controls, recovered single-view
 // camera lifecycle, existing world/audio/HUD services. Other admitted clients
 // keep the existing camera route; no full retail root chronology is claimed.
 OriginalVideoFadeState sourceVideo;
 auto& sourceRendererDispatch=sourceDisplayLifetime->dispatch();
 //Root-owned only; staging/drawing remains inactive. Never retain via renderer.
 original_main_entry50KG::PositionStatsPrimitiveResult50LA sourcePositionStatsPrimitives;
 original_main_entry50KG::PositionStatsResourceResult50LA sourcePositionStatsResources;
 std::shared_ptr<OriginalPositionStatsHud50LA> sourcePositionStatsHud;
 bool sourcePositionStatsRendering=false,sourcePositionStatsDisplayReported=false;
 // Native GL has no guest DMA packet queues/callbacks. Its real submissions
 // are retired by the context API, never certified by these empty source lists.
 std::int32_t sourceFadeManagerA8=0,sourceFadeDuration98=0;
 // Preserve legacy construction after actual native launch subtype binding.
 // A reached counted prefix already owns and populated this same draw owner.
 if(!sourceModelRenderGlobals)sourceModelRenderGlobals=
     std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
 OriginalAttachedGlobals sourceAttachedGlobals;
 OriginalRaceFogFields legacySourceTrackFog;
 require50DU(!isGbTrack||(gbFogOwner&&gbFogOwner->ready()&&gbFogOwner->sourceOwner()==gbColdSources.get()&&gbFogOwner->cameraFields()),
     "GB actual source-bound fog owner unavailable");
 require50DU(!isMcTrack||(mcFogOwner&&mcFogOwner->ready()&&mcFogOwner->sourceOwner()==mcColdSources.get()&&mcFogOwner->cameraFields()),
     "MC actual source-bound fog owner unavailable");
 auto& sourceTrackFog=isMcTrack?*mcFogOwner->cameraFields():isGbTrack?*gbFogOwner->cameraFields():legacySourceTrackFog;
 std::unique_ptr<mc_camera_assets50MC::Owner> mcSourceCameraAssets;
 if(isMcTrack){
  reserveMcEnvironment(sizeof(mc_camera_assets50MC::Owner),"MC camera owner exceeds remaining payload");
  mcSourceCameraAssets=std::make_unique<mc_camera_assets50MC::Owner>();
 }
 sourceCamera::AuthoredStorage legacySourceCameraAssets;
 std::unique_ptr<sourceCamera::AuthoredStorage> gbSourceCameraAssets;
 if(isGbTrack){
  reserveGbAdditionalPayload(sizeof(sourceCamera::AuthoredStorage),"GB camera descriptor exceeds remaining payload");
  gbSourceCameraAssets=std::make_unique<sourceCamera::AuthoredStorage>();
 }
 std::unique_ptr<sourceCamera::AuthoredStorage> tbSourceCameraAssets;
 if(isTbTrack){
  reserveTbAdditionalPayload(sizeof(sourceCamera::AuthoredStorage),"TB camera descriptor exceeds remaining payload");
  tbSourceCameraAssets=std::make_unique<sourceCamera::AuthoredStorage>();
 }
 std::unique_ptr<sourceCamera::AuthoredStorage> rcSourceCameraAssets;
 if(isRcTrack)rcSourceCameraAssets=std::make_unique<sourceCamera::AuthoredStorage>();
 auto& sourceCameraAssets=isGbTrack?*gbSourceCameraAssets:isTbTrack?*tbSourceCameraAssets:isRcTrack?*rcSourceCameraAssets:legacySourceCameraAssets;
 std::unique_ptr<sourceCamera::DisplayInputs> sourceCameraDisplay;
 std::shared_ptr<sourceCamera::Session> sourceCameraSession;
 audioListenerCamera50DU=&sourceCameraSession;
 bool ownedListenerCameraObserved=false;
 advanceOwnedListener=[&](std::uint64_t before,bool held,std::uint32_t& pc){
  // Preserve original lexical Camera/assets destruction order. This closure
  // supplies no pose/provider: it calls the retained actual Session directly.
  if(!sourceCameraSession)return !ownedListenerCameraObserved; //No fallback after actual owner.
  ownedListenerCameraObserved=true;
  // The exact consumed ALL8 Pause route has no human/TA issuance. Keep its
  // listener on this real shared-mix callback without widening that issuance.
  if(sourceConsumedPauseActivation)
   return originalMainApplication.advanceConsumedRaAll8MusicListenerNative(*sourceCameraSession,before,held,pc);
  return sourceCameraSession->advanceNativeMusicListenerConsumedStep(before,held,pc);
 };
 sourceCamera::Session::GameplayFogRenderPhase sourceFogRenderPhase;
 std::array<native_racer50HW::Cables*,8> sourceCameraCables{};
 original_speed_boost_adapter50LA::Owner sourceSpeedBoostAdapter;
 bool sourceSpeedBoostRendering=false,sourceSpeedBoostDisplayReported=false;
 original_time_stats_adapter50LA::Owner sourceTimeStatsAdapter;
 bool sourceTimeStatsRendering=false,sourceTimeStatsDisplayReported=false;
 OriginalDamageObservation50LA sourceDamageObservation;
 bool sourceDamageRendering=false,sourceDamageDisplayReported=false;
 original_camera_input50KX::Owner sourceCameraInput;
 original_all8_frame::Schedule sourceFrameSchedule;
 std::unique_ptr<platform::OriginalVideoBlockingNativeBackend> sourceFadeOut,sourceFadeIn,sourceEndingFade;
 std::uint64_t sourceCameraTick=0,renderedCameraPublication=0;
 // A synchronous source skip can suspend inside a real camera child call.
 // Resume that same call, never manufacture another scene update or tick.
 std::optional<std::uint64_t> suspendedTaCameraFrame;
 std::shared_ptr<const rr::assets::OriginalTaIntroAuthoringCandidate> taIntroAuthoring;
 std::shared_ptr<const rr::assets::AudioBank50HC> taIntroPcm;
 original_ui_source_timing50LH::Owner taRootTiming;
 original_ui_source_timing50LH::PublicationView taRootPublication;
 std::uint64_t taRootTimelineNs=0;
 bool taRootTimingInitialized=false,taRootActive=false;
 auto nativeIntroRace=[&]() -> rr::game::OriginalRaceConstructor50LA* {
  return consumedTaStage.race()?consumedTaStage.race().get():
      consumedRaStage.race()?consumedRaStage.race().get():
      consumedMaStage.race()?consumedMaStage.race().get():
      consumedSaStage.race()?consumedSaStage.race().get():
      consumedGaStage.race()?consumedGaStage.race().get():consumedGcStage.race().get();
 };
 auto nativeIntroController=[&]() -> const rr::game::OriginalWreckComponent50LA* {
  // Slot custody survives a failed camera prepare; do not ask a live-camera
  // readiness getter to authorize teardown of an already retained controller.
  if(const auto* race=nativeIntroRace())if(const auto* controller=race->nativeIntroControllerSlot48())return controller;
  return sourceCameraSession?sourceCameraSession->taControllerEntity174A30():nullptr;
 };
 auto finishTaRoot=[&](){
  if(!taRootActive)return true;
  std::string error;
  if(!nativeIntroRace()||!sourceCameraSession||!nativeIntroController()||
     !originalMainApplication.finishNativeTaRaceRoot140CFC(*nativeIntroRace(),
       *nativeIntroController(),taRootPublication,error)||!taRootTiming.commit())return false;
  taRootActive=false;return true;
 };
 auto beginTaRoot=[&](){
  if(!taIntroAuthoring)return true;
  if(taRootActive||!nativeIntroRace()||!sourceCameraSession||!nativeIntroController())return false;
  original_ui_source_timing50LH::Sample sample;
  if(!taRootTimingInitialized){
   if(!taRootTiming.retainPublication(taRootPublication)||!taRootTiming.prepare(0,true,sample)||
      !taRootTiming.commit())return false;
   taRootTimingInitialized=true;
  }
  taRootTimelineNs+=16000000u;
  std::string error;
  if(!taRootTiming.prepare(taRootTimelineNs,false,sample)||
     !originalMainApplication.beginNativeTaRaceRoot140CE0(*nativeIntroRace(),
       *nativeIntroController(),taRootPublication,error))return false;
  taRootActive=true;
  if(!liveAudio->groupsForProviders()->serviceTaIntro235720(taRootPublication))return false;
  // Same accepted Root23C, once before Camera175D70. The original child list
  // places F8 before controller48. A suspended skip resumes that same Camera
  // call without beginTaRoot, so this timer cannot replay during fade polls.
  // This native cut does not certify the absent full141ED0 renderer dispatcher.
  return originalMainApplication.updateNativeTaRaceF8BeforeCamera50LA(*nativeIntroRace(),
      *nativeIntroController(),taRootPublication,error);
 };
 // Called ONLY from the two successful ordered native callback-return cuts,
 // never by the generic normal/early-return RAII guard.
 auto finishTaRootWithResults=[&](){
  if(!taRootActive)return true;
  std::string error;
  if(!nativeIntroRace()||!sourceCameraSession||!nativeIntroController()||
     !originalMainApplication.finishNativeTaRaceRoot140CFC(*nativeIntroRace(),
       *nativeIntroController(),taRootPublication,error))return false;
  rr::game::OriginalResults104Diagnostic50LA results;
  const bool captured=originalMainApplication.captureNativeResultsAfterRoot50LA(*sourceCameraSession,results);
  // Commit the same real prepared sample even when a bounded capture rejects;
  // it must not remain available for a replay or a later fabricated Frame.
  const bool committed=taRootTiming.commit();taRootActive=false;
  return captured&&committed;
 };
 bool sourceMainBaseReady=false;
 original_course_targets50KX::Catalog courseTargets;
 original_main_entry50KG::Frontend originalMain;

 bool restartRequested=false,returnOriginalMainRequested=false;
 // Private same-thread borrow of the actual setup devices and this Bank.
 // Declared after RA/camera owners: fallback revokes before their destruction.
 original_results_feedback50LA::Scope sourceResultsFeedback{*this,racerBank};
 //Same lexical lifetime, distinct actual HUD authority; no end-flow caller.
 original_race_hud_scope50LA::Scope sourceEndHud{*this,racerBank};
 auto closeWindow=[&](){
  if(!originalMainApplication.cancelNativePauseReturn50LA(sourceFadeOut.get())){
   std::cerr<<"native_close_rejected gate=Pause_retained_input_cancel\n";return false;
  }
  if(!originalMainApplication.nativePauseMenuDeparting50LA()&&
     !originalMainApplication.cancelNativePauseMenuTransfer50LA()){
   std::cerr<<"native_close_rejected gate=Pause_menu_request_cancel\n";return false;
  }
  // The pending input owns Camera/World/Display pins and a live fade capture.
  // Retire it before revoking any registered source callback or GPU owner.
  // A failed drain retains ALL custody for the existing close retry.
  if(originalMainApplication.resultsDonePending50LA()){
   if(!sourceFadeOut||!sourceFadeOut->cancelAndRetire()||
      !originalMainApplication.cancelResultsDone50LA(sourceFadeOut.get())){
    std::cerr<<"native_close_rejected gate=Results_retained_input_cancel\n";return false;
   }
  }
  if(!originalMainApplication.nativeResultsHallDeparting50LA()&&
     !originalMainApplication.cancelNativeResultsHallTransfer50LA()){
   std::cerr<<"native_close_rejected gate=Results_Hall_request_cancel\n";return false;
  }
  sourceResultsFeedback.revoke(); // Terminal even if a later close gate fails.
  sourceEndHud.revoke(); //Before the first fallible camera/world/renderer close.
  const auto closeRejected=[&](const char* gate){
   std::cerr<<"native_close_rejected gate="<<gate
       <<" renderer_error="<<static_cast<unsigned>(rendererDiagnostic.error)
       <<" renderer_operation="<<(rendererDiagnostic.operation?rendererDiagnostic.operation:"")
       <<" renderer_cleanup_incomplete="<<rendererDiagnostic.cleanupIncomplete
       <<" position_error="<<static_cast<unsigned>(sourcePositionStatsResources.diagnostic.error)
       <<" position_material_status="<<static_cast<unsigned>(sourcePositionStatsResources.diagnostic.material.status)
       <<" position_material_pc="<<sourcePositionStatsResources.diagnostic.material.sourcePC
       <<" consumed_material_status="<<static_cast<unsigned>(consumedRaStage.closeMaterialDiagnostic.status)
       <<" consumed_material_pc="<<consumedRaStage.closeMaterialDiagnostic.sourcePC
       <<" consumed_material_field="<<(consumedRaStage.closeMaterialDiagnostic.field?consumedRaStage.closeMaterialDiagnostic.field:"")
       <<" consumed_audio_error="<<static_cast<unsigned>(consumedRaStage.closeAudioDiagnostic.error)
       <<" position_pc="<<sourcePositionStatsResources.diagnostic.sourcePC
       <<" position_renderer_operation="<<(sourcePositionStatsResources.diagnostic.renderer.operation?
           sourcePositionStatsResources.diagnostic.renderer.operation:"")
       <<" ra_failure="<<(raSessionDiagnostic.failure?raSessionDiagnostic.failure:"")<<'\n';
   if(raWorld){const auto& world=raWorld->nativeCloseDiagnostic();
    std::cerr<<"native_world_close_rejected gate="<<(world.gate?world.gate:"")
        <<" counted="<<world.counted<<" retire_counted="<<world.retireCounted
        <<" topology_attempted="<<world.topologyAttempted<<" topology_closed="<<world.topologyClosed
        <<" camera_borrows="<<world.cameraBorrows<<" world_borrows="<<world.worldBorrows
        <<" runtime_owners="<<world.runtimeOwners<<" navigation_owners="<<world.navigationOwners
        <<" lighting_owners="<<world.lightingOwners<<" expected_lighting_owners="<<world.expectedLightingOwners
        <<" particle_owners="<<world.particleOwners<<" pc="<<world.sourcePC
        <<" world_refs="<<world.worldReferences<<" world_flags="<<world.worldFlags<<" world_mask="<<world.worldMask<<'\n';
   }
   return false;
  };
  const auto closeAccumulated=[](const char* gate){
   std::cerr<<"native_close_accumulated gate="<<gate<<'\n';return false;
  };
  // Host no longer closes between player sessions. Retire any real motor
  // lease explicitly rather than depending on SDL window destruction.
  if(originalMainApplication.retainsPlayerWindow50DU()&&feedbackHost&&
     usePhysicalGameController&&feedbackPhysicalDevice.connected){
   platform::ControllerRumbleReceipt50KZ stopped;
   platform::GameHostDiagnostic50ER stopDiagnostic;
   if(!feedbackHost->rumbleController50KZ(playerSource5C,feedbackPhysicalDevice.instanceId,
       0,0,stopped,stopDiagnostic))return closeRejected("retained_window_rumble_stop");
  }
  if(gbQueryBinding&&gbQueryBinding->enrolled()&&!gbQueryBinding->fence())return closeRejected("gb_query_terminal_fence");
  if(gbParticleRender&&!gbParticleRender->closed()&&!gbParticleRender->close())return closeRejected("gb_real_renderer_borrow_retirement");
  if(mcParticleRender&&!mcParticleRender->closed()&&!mcParticleRender->close())return closeRejected("mc_real_renderer_borrow_retirement");
  if(mcAnimationBinding&&mcAnimationBinding->needsClose()&&!mcAnimationBinding->close(mcAnimationBindingDiagnostic))
   return closeRejected("mc_animation_membership_retirement");
  if(gbAnimationBinding&&gbAnimationBinding->needsClose()&&!gbAnimationBinding->close(gbAnimationBindingDiagnostic))
   return closeRejected("gb_actual_animation_membership_retirement");
  // Stop new RC numerical/scheduling borrows on normal and setup-failure
  // cleanup, before Session racer prefixes or renderer/Window owners retire.
  if(rcParticleRuntime&&!rcParticleRuntime->quiesce())return closeRejected("rc_emitter_quiesce");
  // A reached gate failure blocks ordinary lighting, including the camera's
  // removal lease. Retire only that incomplete CPU observer before camera
  // cleanup; actual root/window/runtime/lighting and GPU backing remain live.
  // Binding.close rejects any still-active draw/camera/reset lease. No further
  // renderer traversal is admitted once this terminal close has started.
  if(tbDoorBinding&&tbDoorBinding->needsClose()&&!tbDoorBinding->ready()&&
      !tbDoorBinding->close(tbDoorBindingDiagnostic))
   return closeRejected("tb_incomplete_door_before_camera_retirement");
  if(tcGateVisibility&&tcGateVisibility->needsClose()&&!tcGateVisibility->ready()&&
      !tcGateVisibility->close(tcGateVisibilityDiagnostic))
   return closeRejected("tc_incomplete_gate_before_camera_retirement");
  if(sourcePauseParentAttached){
   original_pause_observation50LA::Owner::Diagnostic pauseCloseDiagnostic;
   if(!sourcePauseObservation.closeNativeSpectatorMenuForShutdown(pauseCloseDiagnostic)){
    std::cerr<<"native_pause_shutdown_rejected pc="<<pauseCloseDiagnostic.sourcePC<<'\n';
    return closeRejected("pause_shutdown_callback");
   }
   sourcePauseParentAttached=false; // Actual close completed; native graph cleanup follows.
  }
  // Every-track original Pause left open by Restart/Main/Quit: close its
  // entity activity before the shared Pause retirement below.
  if(sourcePauseObservation.hostOpen()&&!sourcePauseObservation.closeHost())
   return closeRejected("original_pause_host_shutdown");
  // Retire the actual Pause GPU borrow and FINAL pin before any HUD material
  // releases. Keep the renderer/context live for PositionStats' own drains.
  if(!renderer.retireOriginalPauseDerived(rendererDiagnostic))return closeRejected("if(!renderer.retireOriginalPauseDerived(rendererDiagnostic))");
  if(!sourcePause.retireDrawing())return closeRejected("if(!sourcePause.retireDrawing())");
  if(sourcePositionStatsResources.owner&&
     !sourcePositionStatsResources.owner->close(sourcePositionStatsResources.diagnostic))return closeRejected("!sourcePositionStatsResources.owner->close(sourcePositionStatsResources.diagnostic))");
  if(sourcePositionStatsPrimitives.owner&&
     !sourcePositionStatsPrimitives.owner->close(sourcePositionStatsPrimitives.diagnostic))return closeRejected("!sourcePositionStatsPrimitives.owner->close(sourcePositionStatsPrimitives.diagnostic))");
  // Counted race/FIFO/world owners also retain the display. Its final close
  // admission follows their real retirement below, while GL remains current.
  sourcePositionStatsResources.owner.reset();sourcePositionStatsResources.primitive.owner.reset();
  sourcePositionStatsResources.coldResourcesLoaded=false;
  sourcePositionStatsPrimitives.owner.reset();sourcePositionStatsPrimitives.resourceFragmentLoaded=false;
  bool clean=true;
  // A failed source E8 owns the selected Bank slot. Retire its genuine
  // partial reset before camera/model close performs any ordinary Bank visit.
  if(mbSourceTriggers&&mbSourceTriggers->pending()){
   const bool retired=mbSourceTriggers->retireSuspended([&](std::size_t slot){
    require50DU(slot<racerCables.size()&&racerCables[slot]&&racerRigs[slot],
        "MB failed-slot cleanup requires its actual cables and Rig");
    return mb_source_trigger50MB::RetirementBorrow{s,x.bindings,x.parameters,gcLightOwner,
        racerCables[slot]->visibility50(),recoveryQueries(),zoneLifecycle,liveWorld,membershipWork,
        slot==0?playerRacer60:originalAi.racer60};
   });
   if(!retired)return closeRejected("mb_source_pending_retirement");
  }
  if(originalCameraEnabled){
   if(taIntroAuthoring&&liveAudio->groupsForProviders()&&
      !liveAudio->groupsForProviders()->retireTaIntro142590())return closeRejected("TA intro audio retirement");
   if(taRootActive){
    std::string error;
    if(!originalMainApplication.abandonNativeTaRaceRootForTeardown50LA(*nativeIntroRace(),
       *nativeIntroController(),taRootPublication,error))return closeRejected("TA pending root abandonment");
    taRootActive=false;
   }
   if(nativeIntroRace()&&nativeIntroController()&&
      !nativeIntroRace()->clearNativeIntroControllerSlot48(*nativeIntroController()))
    return closeRejected("actual intro race controller slot48 retirement");
   if(sourceEndingFade&&!sourceEndingFade->cancelAndRetire())return closeRejected("if(sourceEndingFade&&!sourceEndingFade->cancelAndRetire())");
   if(sourceFadeIn&&!sourceFadeIn->cancelAndRetire())return closeRejected("if(sourceFadeIn&&!sourceFadeIn->cancelAndRetire())");
   if(sourceFadeOut&&!sourceFadeOut->cancelAndRetire())return closeRejected("if(sourceFadeOut&&!sourceFadeOut->cancelAndRetire())");
   if(sourceCameraSession){
    if(const auto* race=originalMainApplication.race28C()){
     // Read-only Camera34 custody observation before abandonment.
     std::cerr<<"camera34_close_state race28C=1 camera34="<<bool(race->camera34_)
         <<" adapter_bound="<<(race->cameraNativeAdapter34_==sourceCameraSession.get())
         <<" adapter_any="<<bool(race->cameraNativeAdapter34_)
         <<" lifetime_current="<<race->nativeCameraAdapter34LifetimeCurrent(*sourceCameraSession)
         <<" issued_current="<<(originalMainApplication.issuedCourseCameraSession50LA()==sourceCameraSession)
         <<" step="<<static_cast<unsigned>(race->cameraInitStep_)<<"\n";
    }else std::cerr<<"camera34_close_state race28C=0\n";
    const auto abandoned=sourceCameraSession->abandonAndCloseSpatial();
    if(abandoned.status!=sourceCamera::Status::abandoned){
     std::cerr<<"camera_abandon_rejected status="<<static_cast<unsigned>(abandoned.status)
         <<" source_pc=0x"<<std::hex<<abandoned.sourcePC<<std::dec<<"\n";
     return closeRejected("if(sourceCameraSession&&sourceCameraSession->abandonAndCloseSpatial().status!=sourceCamera::Status::abandoned)");
    }
   }
   if(isRaTrack){
    sourceCameraSession.reset();
    if(raSession&&!raSession->retireBeforeBankDestruction(raSessionDiagnostic))return closeRejected("if(raSession&&!raSession->retireBeforeBankDestruction(raSessionDiagnostic))");
   }
   if(!isRcTrack&&!nativeTaParticles.close())return closeRejected("if(!nativeTaParticles.close())");
   if(renderer.initialized()&&!renderer.clearGameplayBlurStrengths(rendererDiagnostic))return closeRejected("if(renderer.initialized()&&!renderer.clearGameplayBlurStrengths(rendererDiagnostic))");
  }
  // The coordinator retires Rider source owners through its live renderer,
  // then closes the renderer before releasing session/texture owners. A host
  // pre-close would make source retirement impossible during exception cleanup.
  if(originalUiStarted){
   // Before activation, renderer.create may retain a bank borrow even on
   // failure. With no coordinator borrow, preserve renderer-first cleanup.
   if(!originalMain.coordinator.borrowsRenderer(renderer)&&
      renderer.ownsResources()&&!renderer.close(rendererDiagnostic)){
    clean=closeAccumulated("coordinator_renderer_close");
    // Retry cleanup while its context and both texture owners are still live.
    // Never unwind into bank destructors while the renderer retains a borrow.
    if(!renderer.close(rendererDiagnostic)){
     std::cerr<<"original UI renderer cleanup remains unresolved; terminating before owner teardown\n";
     std::terminate();
    }
   }
   if(!originalMain.closeUi()){
    clean=closeAccumulated("original_main_close_ui");
    if(!originalMain.closeUi()){
     std::cerr<<"original UI bank cleanup remains unresolved; terminating with context retained\n";
     std::terminate();
    }
   }
  }
  if(!originalMain.closeAudio())clean=closeAccumulated("if(!originalMain.closeAudio())clean=false;");
  if(!sourceAnnouncer->reset())clean=closeAccumulated("if(!sourceAnnouncer->reset())clean=false;raceMusic->reset();");raceMusic->reset();
  raceAudio->reset(); // Stop shared Dax and restore groups before mixer closure.
  if(tcGlobalSound&&tcGlobalSound->prepared()&&!tcGlobalSound->close())
   return closeRejected("tc_actual_global_sound_shutdown");
  if(gbPodSound&&!gbPodSound->close())return closeRejected("gb_actual_pod_sound_shutdown");
  if(!retireGbAudioOwners())return closeRejected("gb_positional_material_audio_retirement");
  if(!retireMcAudioOwners())return closeRejected("mc_positional_material_audio_retirement");
  if(tbPodSound&&!tbPodSound->close())return closeRejected("tb_actual_pod_sound_shutdown");
  gcGateAudio.reset(); // Stop exclusive GC voice126 before its real mixer/groups.
  gaGateAudio.reset(); // Stop voice126 while its real mixer/groups still live.
  opponentAudio->close(); // Release96..116 before the live mixer/shared PCM closes.
  originalAiAudio->close();
  if(!retireRcAudioOwners())return closeRejected("rc_positional_material_audio_retirement");
  if(!retireTbAudioOwners())return closeRejected("tb_positional_material_audio_retirement");
  sourceCameraCycleAudio.close(); //Retire exclusive voice159 before its actual mixer/output.
  if(!liveAudio->close(audioDiagnostic)){
   std::cerr<<"audio close failed "<<static_cast<unsigned>(audioDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"audio close failed \"<<static_cast<unsigned>(audioDiagnostic.error)<<'\n';clean=false;");
  }
  if(renderer.ownsResources()&&!renderer.close(rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);clean=closeAccumulated("reportRenderer50DU(rendererDiagnostic);clean=false;");
  }
  // Never destroy a context beneath unresolved GPU ownership. Destructors may
  // retry cleanup in the same correct order; neither silently leaks resources.
  if(renderer.ownsResources())return closeRejected("if(renderer.ownsResources())");
  // Session abandonAndCloseSpatial already retired actual racer Entity
  // prefixes. Renderer retirement above revoked Window publication/borrows.
  // Keep the real context alive for the atlas's checked GPU close.
  if(rcParticleRender&&!rcParticleRender->close())return closeRejected("rc_particle_numeric_retirement");
  if(gbParticleWindow&&!gbParticleWindow->close(textureDiagnostic))return closeRejected("gb_particle_window_retirement");
  if(mcParticleWindow&&!mcParticleWindow->close(textureDiagnostic))return closeRejected("mc_particle_window_retirement");
  if(rcParticleWindow&&!rcParticleWindow->close(textureDiagnostic))return closeRejected("rc_particle_window_retirement");
  if(rcTrackParticles&&!rcTrackParticles->close())return closeRejected("rc_actual_shared_pool_retirement");
  if(rcParticleRuntime&&!rcParticleRuntime->closeAfterPool())return closeRejected("rc_emitter_release_after_pool");
  if(!sourcePause.retireDrawing())return closeRejected("if(!sourcePause.retireDrawing())");
  // Renderer has retired its strong HUD owner and every pixel/GPU borrow.
  // Release canonical UI refs before later model retirement invalidates leaves.
  sourceTimeStatsRendering=false;
  sourceTimeStats.owner.reset();sourceTimeStats.pixels.reset();
  sourceDamageRendering=false;
  sourceDamage.owner.reset();sourceDamage.pixels.reset();
  sourcePositionStatsRendering=false;sourcePositionStatsHud.reset();
  sourceSpeedBoostRendering=false;sourceSpeedBoost.owner.reset();sourceSpeedBoost.pixels.reset();
  if(gcDynamicWindow.ownsResources()&&!gcDynamicWindow.close(textureDiagnostic)){
   std::cerr<<"GC dynamic texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"GC dynamic texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(gcDynamicWindow.ownsResources())return closeRejected("if(gcDynamicWindow.ownsResources())");
  if(gaDynamicWindow.ownsResources()&&!gaDynamicWindow.close(textureDiagnostic)){
   std::cerr<<"GA dynamic texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"GA dynamic texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(gaDynamicWindow.ownsResources())return closeRejected("if(gaDynamicWindow.ownsResources())");
  if(tcDynamicWindow.ownsResources()&&!tcDynamicWindow.close(textureDiagnostic)){
   std::cerr<<"TC gate texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';
   clean=closeAccumulated("TC gate texture cleanup failed");
  }
  if(tcDynamicWindow.ownsResources())return closeRejected("TC gate texture resources remain");
  if(tbDynamicWindow.ownsResources()&&!tbDynamicWindow.close(textureDiagnostic)){
   std::cerr<<"TB door texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';
   clean=closeAccumulated("TB door texture cleanup failed");
  }
  if(tbDynamicWindow.ownsResources())return closeRejected("TB door texture resources remain");
  if(mcDynamicWindow.ownsResources()&&!mcDynamicWindow.close(textureDiagnostic))return closeRejected("mc_dynamic_texture_retirement");
  if(mcAtm.ownsResources()&&!mcAtm.close(textureDiagnostic))return closeRejected("mc_atm_texture_retirement");
  if(gbDynamicWindow.ownsResources()&&!gbDynamicWindow.close(textureDiagnostic))return closeRejected("gb_dynamic_texture_retirement");
  if(gbAtm.ownsResources()&&!gbAtm.close(textureDiagnostic))return closeRejected("gb_atm_texture_retirement");
  if(gcAtm.ownsResources()&&!gcAtm.close(textureDiagnostic)){
   std::cerr<<"GC ATM texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"GC ATM texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(gcAtm.ownsResources())return closeRejected("if(gcAtm.ownsResources())");
  if(saDynamicWindow.ownsResources()&&!saDynamicWindow.close(textureDiagnostic)){
   std::cerr<<"SA dynamic texture cleanup rejected\n";clean=closeAccumulated("std::cerr<<\"SA dynamic texture cleanup rejected\n\";clean=false;");
  }
  if(saDynamicWindow.ownsResources())return closeRejected("if(saDynamicWindow.ownsResources())");
  if(maSaAtm.ownsResources()&&!maSaAtm.close(textureDiagnostic)){
   std::cerr<<"MA/SA ATM texture cleanup rejected\n";clean=closeAccumulated("std::cerr<<\"MA/SA ATM texture cleanup rejected\n\";clean=false;");
  }
  if(maSaAtm.ownsResources())return closeRejected("if(maSaAtm.ownsResources())");
  if(mbAtm.ownsResources()&&!mbAtm.close(textureDiagnostic)){
   std::cerr<<"MB ATM texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';
   clean=closeAccumulated("MB ATM texture cleanup failed");
  }
  if(mbAtm.ownsResources())return closeRejected("if(mbAtm.ownsResources())");
  if(scAtm.ownsResources()&&!scAtm.close(textureDiagnostic)){
   std::cerr<<"SC ATM texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';
   clean=closeAccumulated("SC ATM texture cleanup failed");
  }
  if(scAtm.ownsResources())return closeRejected("if(scAtm.ownsResources())");
  if(tbAtm.ownsResources()&&!tbAtm.close(textureDiagnostic))return closeRejected("TB ATM GPU close rejected");
  if(tbAtm.ownsResources())return closeRejected("TB ATM resources remain");
  if(rcAtm.ownsResources()&&!rcAtm.close(textureDiagnostic))return closeRejected("RC ATM GPU close rejected");
  if(rcAtm.ownsResources())return closeRejected("RC ATM resources remain");
  if(tcAtm.ownsResources()&&!tcAtm.close(textureDiagnostic)){
   std::cerr<<"TC ATM texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';
   clean=closeAccumulated("TC ATM texture cleanup failed");
  }
  if(tcAtm.ownsResources())return closeRejected("if(tcAtm.ownsResources())");
  if(gaAtm.ownsResources()&&!gaAtm.close(textureDiagnostic)){
   std::cerr<<"GA ATM texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"GA ATM texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(gaAtm.ownsResources())return closeRejected("if(gaAtm.ownsResources())");
  if(gcParticleMaterials.ownsResources()&&!gcParticleMaterials.closeGpu(textureDiagnostic))clean=closeAccumulated("if(gcParticleMaterials.ownsResources()&&!gcParticleMaterials.closeGpu(textureDiagnostic))clean=false;");
  if(gcParticleMaterials.ownsResources())return closeRejected("if(gcParticleMaterials.ownsResources())");
  if(gameplayBinder.ownsResources()&&!gameplayBinder.close(textureDiagnostic))clean=closeAccumulated("if(gameplayBinder.ownsResources()&&!gameplayBinder.close(textureDiagnostic))clean=false;");
  if(gameplayBinder.ownsResources())return closeRejected("if(gameplayBinder.ownsResources())");
  for(std::size_t index=0;index<raceOpponentCount;++index){
   for(auto* texture:{&opponentGpuTextures[index],&opponentCableGpuTextures[index],
       &opponentSpecularGpuTextures50LA[index]}){
    if(texture->ownsResources()&&!texture->close(textureDiagnostic)){
     std::cerr<<"original racer texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"original racer texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
    }
    if(texture->ownsResources())return closeRejected("if(texture->ownsResources())");
   }
  }
  if(!native_wreck_debris50PD::closeGpu(textureDiagnostic)){
   std::cerr<<"debris texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("debris texture cleanup");
  }
  if(gpuTextures.ownsResources()&&!gpuTextures.close(textureDiagnostic)){
   std::cerr<<"texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(gpuTextures.ownsResources())return closeRejected("if(gpuTextures.ownsResources())");
  if(specularGpuTextures50LA.ownsResources()&&!specularGpuTextures50LA.close(textureDiagnostic)){
   std::cerr<<"canonical specular texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"canonical specular texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(specularGpuTextures50LA.ownsResources())return closeRejected("if(specularGpuTextures50LA.ownsResources())");
  if(trackGpuTextures.ownsResources()&&!trackGpuTextures.close(textureDiagnostic)){
   std::cerr<<"track texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"track texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(trackGpuTextures.ownsResources())return closeRejected("if(trackGpuTextures.ownsResources())");
  if(cableGpuTextures.ownsResources()&&!cableGpuTextures.close(textureDiagnostic)){
   std::cerr<<"cable texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"cable texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(cableGpuTextures.ownsResources())return closeRejected("if(cableGpuTextures.ownsResources())");
  if(skyGpuTextures.ownsResources()&&!skyGpuTextures.close(textureDiagnostic)){
   std::cerr<<"sky texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("std::cerr<<\"sky texture cleanup error \"<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;");
  }
  if(skyGpuTextures.ownsResources())return closeRejected("if(skyGpuTextures.ownsResources())");
  // Session retires its attached racers above. Frontend-only exit and a
  // rejected cold material population have no Session, but still own real
  // installed models and may already have canonical flame borrows. Close
  // those same bank slots while the actual Video host remains available;
  // later local destruction is an idempotent no-op, never the first release.
  for(std::size_t slot=nativeRightModels.size();slot>0;--slot){
   // Later aliases retire in reverse source order on Session close,
   // Course/restart and frontend/partial-load/exception unwind alike.
   for(auto* model:{nativeChariotModels[slot-1].get(),nativeBottomLeftModels[slot-1].get(),
       nativeBottomRightModels[slot-1].get(),nativeLeftModels[slot-1].get(),
       nativeRightModels[slot-1].get()}){
    if(model&&!model->close()){
     std::cerr<<"original model cleanup rejected slot="<<slot-1
         <<" role="<<model->modelRole()<<" source_pc="<<model->failurePC()<<'\n';
     return closeRejected("original_model_close");
    }
   }
  }
  if(isTbTrack){
   // abandonAndCloseSpatial above retired actual racer Entity/particle borrows.
   // Renderer, HUD and model callbacks have now closed; reject external Session
   // ownership before destroying its borrowed pool or authoring inputs.
   if(sourceCameraSession&&sourceCameraSession.use_count()!=1)return closeRejected("tb_camera_owner_still_borrowed");
   sourceCameraSession.reset();
   if(tbTrackParticles&&!tbTrackParticles->close())return closeRejected("tb_actual_track_pool_retirement");
   tbTrackParticles.reset();
   tbSourceCameraAssets.reset();
   // Keep all descriptor/source/registry charges until this runSession ends.
  }
  if(mbSourceTriggers){
   // All renderer/camera/model callbacks above have retired. Remove the real
   // serial query memberships while each original Bank slot is installed.
   sourceCameraSession.reset();
   for(std::size_t slot=0;slot<racerBank.count();++slot){
    const bool retired=racerBank.visit(slot,[&](){
     const auto queries=recoveryQueries();
     for(const auto& q:queries.query){
      if(!q.membership||!q.object||(q.membership->attached&&q.membership->object!=q.object))return false;
      const auto r=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,*q.membership,membershipWork);
      if(r.status!=LiveZoneStatus50EN::Complete||q.membership->attached||q.membership->object)return false;
     }
     s.shape.registrationE0=0;
     auto r=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,secondaryMembership,membershipWork);
     if(r.status!=LiveZoneStatus50EN::Complete||secondaryMembership.attached||secondaryMembership.object)return false;
     for(std::size_t i=0;i<3;++i){auto* membership=mbCollisionWorld->triggerMembership(i);
      if(!membership)return false;
      r=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership->state,membershipWork);
      if(r.status!=LiveZoneStatus50EN::Complete||membership->state.attached||membership->state.object)return false;
     }
     return true;
    });
    if(!retired)return closeRejected("mb_serial_query_retirement");
   }
   if(!mbSourceTriggers->unbind())return closeRejected("mb_action_owner_unbind");
   mbSourceTriggers.reset();
  }
  if(isGbTrack){
   if(sourceCameraSession&&sourceCameraSession.use_count()!=1)return closeRejected("gb_camera_owner_still_borrowed");
   sourceCameraSession.reset();gbSourceCameraAssets.reset();
   if(!closeGbColdPrefix())return closeRejected("gb_all_peer_terminal_contact_retirement");
  }
  if(isMcTrack){
   if(sourceCameraSession&&sourceCameraSession.use_count()!=1)return closeRejected("mc_camera_owner_still_borrowed");
   sourceCameraSession.reset();mcSourceCameraAssets.reset();
   if(!closeMcColdPrefix())return closeRejected("mc_all_peer_terminal_contact_retirement");
  }
  if(isTbTrack){
   // Camera/Follow, pool and model/GPU borrowers have actually retired above.
   // Binding owns door337 retirement; this path removes only real query and
   // trigger321..336 memberships, never a second removal of the door.
   if(tbDoorBinding&&!tbDoorBinding->close(tbDoorBindingDiagnostic))
    return closeRejected("tb_door_dual_membership_retirement");
   if(!closeTbColdPrefix())return closeRejected("tb_serial_visibility_collision_retirement");
  }
  if(isTcTrack){
   // Camera/Follow and every model/GPU borrower have actually closed above.
   if(!closeTcColdPrefix())return closeRejected("tc_serial_visibility_collision_retirement");
   if(tcGateVisibility&&!tcGateVisibility->close(tcGateVisibilityDiagnostic))
    return closeRejected("tc_gate_observer_retirement");
  }
  if(!closeRcBankCollision())return closeRejected("rc_serial_collision_retirement");
  if(!closeScTriggerMemberships())return closeRejected("sc_serial_trigger_membership_retirement");
  if(consumedRaStage.race()){
   // No frame, reader or model callback is invoked beyond this point. Destroy
   // actual snapshots before releasing the aggregate's runtime lifetime pin.
   if(!racerBank.retireNativeBacking())return closeRejected("if(!racerBank.retireNativeBacking())");
   sourcePauseTrack.reset();raRuntime.reset();
   if(!consumedRaStage.close())return closeRejected("if(!consumedRaStage.close())");
  }
  if(consumedTaStage.race()&&!consumedTaStage.close())return closeRejected("TA consumed race/world retirement");
  if(consumedGcStage.race()&&!consumedGcStage.close())return closeRejected("GC consumed race/world retirement");
  if(consumedGaStage.race()&&!consumedGaStage.close())return closeRejected("GA consumed race/world retirement");
  if(consumedSaStage.race()&&!consumedSaStage.close())return closeRejected("SA consumed race/world retirement");
  if(consumedMaStage.race()&&!consumedMaStage.close())return closeRejected("MA consumed race/world retirement");
  if(!sourcePause.close())return closeRejected("if(!sourcePause.close())");
  if(!sourceDisplayLifetime->beginCloseAfterDependents())return closeRejected("if(!sourceDisplayLifetime->beginCloseAfterDependents())");
  if(!originalMainApplication.retainsPlayerWindow50DU()&&host.ownsResources()&&!host.close(hostDiagnostic)){
   reportHost50DU(hostDiagnostic);clean=closeAccumulated("reportHost50DU(hostDiagnostic);clean=false;");
  }
  if(hostDiagnostic.window.cleanupError==platform::WindowHostError50DQ::ControllerFailure)
   std::cerr<<"SDL final rumble stop was not submitted; native resources were released: "
       <<hostDiagnostic.window.cleanupDetail.data()<<'\n';
  feedbackHost=nullptr;
  if(!clean)std::cerr<<"native_close_rejected gate=accumulated_cleanup_error\n";
  return clean;
 };
 // Late explicit closeWindow now owns every MB retirement path.
 mcColdPrefixGuard.armed=false;
 gbColdPrefixGuard.armed=false;
 mbColdPrefixGuard.armed=false;
 tcColdPrefixGuard.armed=false;tcGatePrefixGuard.armed=false;
 tbColdPrefixGuard.armed=false;tbDoorPrefixGuard.armed=false;
 try {
  // Root-owned callback installation stays at its original publication point.
  const auto installHumanCameraInput50DU=[&](){
   if(humanCourseEnabled)resetHumanCamera=[&,requireMbRuntimeNavigation50MB](const UndertrackSelection50ET& selected,
       const UndertrackPlacement50ET& placed,std::uint64_t serial){
    if(isRcTrack)requireMbRuntimeNavigation50MB();
    const auto result=sourceCameraSession->resetHumanUndertrack225B0C(selected,placed,serial);
    if(result.status!=sourceCamera::Status::complete){
     originalHumanRecoveryDiagnostic.sourcePC=result.sourcePC;
     originalHumanRecoveryDiagnostic.message="actual human camera reset continuation rejected";
     return false;
    }
    return true;
   };
   if(humanCourseEnabled){
    require50DU(controllerActions.phase()==ControllerActionPhase50EC::Idle&&controllerEvents.count==0&&
        sourceCameraSession->ready(),"human camera requires the same idle acknowledged input owner");
    humanCameraInputRoute={&playerEventChild,sourceCameraSession.get(),&playerEnvironment.held};
    applicationChildren[0].next=1;
    applicationEvents.root={0x600u,0,applicationChildren,2};applicationEvents.visitBudget=2;
    applicationEvents.childContext=&humanCameraInputRoute;
    applicationEvents.childCallback=[](void* opaque,std::uint32_t child,bool pressed,
        InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept -> EventCallbackResult48Z {
     auto* route=static_cast<HumanCameraInputRoute*>(opaque);
     if(!route||!route->player||!route->camera||!route->held)return {false,0};
     if(child==0)return playerEventChild49A(route->player,0,pressed,event,record);
     if(child==1){
      const auto routed=pressed?route->camera->press(*route->held,event,record):route->camera->release(*route->held,event,record);
      if(pressed){const auto& in=route->camera->lastInput(); // Diagnostic: view-button outcome.
       std::fprintf(stderr,"camera_view_press available=%u handled=%u status=%u operation=%u pc=0x%x mode=%d\n",
           unsigned(routed.available),unsigned(routed.originalReturn),unsigned(in.status),unsigned(in.operation),
           in.sourcePC,int(route->camera->cameraMode()));}
      return routed;
     }
     return {false,0};
    };
    require50DU(platform::bindSelectedCameraCycle50KY(controlPresets,0,humanControlInput),
        "actual human camera cycle action cannot bind the accepted preset");
    // The existing49A handler owns gates and its two following timer stores.
    // Only its genuine synchronous2255B0 effect is installed here, after all
    // physical owners and this concrete camera Session have been prepared.
    executeOriginalManualReset=[&,requireMbRuntimeNavigation50MB,refreshAnimatedCandidates,recoveryQueries]() noexcept {
     auto failManual=[&](std::uint32_t pc,const char* message) noexcept {
      originalManualRecoveryDiagnostic.sourcePC=pc;originalManualRecoveryDiagnostic.message=message;
      std::cerr<<"ORIGINAL HUMAN manual reset unavailable: "<<message<<" pc="
          <<std::hex<<pc<<std::dec<<"; completed prefixes retained.\n";
      if(originalResetGround){
       const auto& ground=originalResetGround->diagnostic();
       std::cerr<<"original_manual_reset_ground_context message="<<(ground.message?ground.message:"")
           <<" status="<<static_cast<unsigned>(ground.query.status)
           <<" pc="<<std::hex<<ground.query.sourcePC<<std::dec
           <<" object="<<ground.query.object<<" method="<<ground.query.method
           <<" zone="<<ground.query.zone
           <<" narrowphase="<<static_cast<unsigned>(ground.query.narrowphase)
           <<" active_objects="<<trackStorage.collectionInput.objectCount
           <<" reserved_objects="<<trackStorage.count.objects
           <<" camera_death="<<trackStorage.count.cameraDeath
           <<" wreck_first="<<trackStorage.count.wreckFirstQuery
           <<" wreck_count="<<trackStorage.count.wreckReferences<<'\n';
      }
      return false;
     };
     if(!originalRecoveryEnabled||!originalResetGround||!originalAiAudio||!racerCables[0]||
        !sourceCameraSession||!sourceCameraSession->ready()||humanBaseUndertrack.pending||
        s.phase!=VehicleFramePhase49W::Ready||s.fields.vehicle58||s.fields.vehicle3C||m.vehicleState40||
        originalManualResetSerial==std::numeric_limits<std::uint64_t>::max()||
        gcGraphicsSequence[0]==std::numeric_limits<std::uint64_t>::max()||
        (originalHumanRecovery.stage!=native_original_human_recovery::Stage::Idle&&
         originalHumanRecovery.stage!=native_original_human_recovery::Stage::Complete))
      return failManual(0x1a17a4u,"actual human Ready/normal-state reset owner is unavailable");
     std::uint8_t* flags22C=nullptr;std::uint32_t* field24C=nullptr;
     auto& controllerD40=liveAudio->humanController();
     requireMbRuntimeNavigation50MB();
     if(!originalAiAudio->humanPhysicalResetOwners(playerRacer60,s,controllerD40,flags22C,field24C)||
        flags22C!=&controllerD40.flags22C()||!field24C||!refreshAnimatedCandidates())
      return failManual(0x1a259cu,"actual human D40/D44 or world publication is unavailable");
     const auto drained=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,trackCounts.events);
     if(drained.status!=ZoneQueueStatus50CQ::Complete||drained.remaining)
      return failManual(0x124cb0u,"actual reset world registration queue is pending");
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
     const std::uint32_t resetBeforePositionBits[3]={
         original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[0]),
         original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[1]),
         original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[2])};
     ++originalManualResetSerial;
     if(!std::visit([&](auto& actual){return native_original_human_recovery::resetController2255B0(s,x.bindings,x.parameters,navigationProgress,
         originalManualResetSerial,originalHumanRecovery,actual,*originalRecoveryWorkspace,originalManualRecoveryDiagnostic);},owners))
      return failManual(originalManualRecoveryDiagnostic.sourcePC,originalManualRecoveryDiagnostic.message);
     // Physical visibility lease has ended before camera3C borrows4C0.
     const auto camera=sourceCameraSession->resetHumanController225B0C(originalHumanRecovery.selection,
         originalHumanRecovery.placement,originalManualResetSerial);
     if(camera.status!=sourceCamera::Status::complete)
      return failManual(camera.sourcePC,"actual human225B0C camera reset is incomplete");
     // Input samples may precede a zero-step host poll. Keep capture/resize
     // redraw pending until the next real graphics publication, never redraw
     // reset models against the previous sequence or force an extra update.
     originalManualResetGraphicsDue=gcGraphicsSequence[0]+1u;
     // Both reset owners returned complete; this does not claim graphics publication.
     std::cerr<<"original_manual_reset_complete serial="<<originalManualResetSerial
         <<" physical_completed="<<originalHumanRecovery.physicalCompleted
         <<" anchor_node="<<originalHumanRecovery.selection.anchorNode
         <<" selected_next_node="<<originalHumanRecovery.selection.selectedNextNode
         <<" before_position_bits="<<std::hex<<resetBeforePositionBits[0]<<','
         <<resetBeforePositionBits[1]<<','<<resetBeforePositionBits[2]
         <<" after_position_bits="<<original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[0])<<','
         <<original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[1])<<','
         <<original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[2])<<std::dec
         <<" graphics_sequence="<<gcGraphicsSequence[0]
         <<" graphics_due="<<originalManualResetGraphicsDue<<'\n';
     return true;
    };
    require50DU(originalRecoveryEnabled&&originalResetGround&&racerCables[0]&&
        platform::bindSelectedManualReset50LC(controlPresets,0,humanControlInput),
        "actual human manual reset cannot bind the accepted preset");
    const auto& resetMapping=humanControlInput.keyboard.buttons[static_cast<std::size_t>(humanControlInput.manualResetCode)];
    std::cerr<<"original_manual_reset_binding bound="<<humanControlInput.manualResetBound
        <<" descriptor="<<humanControlInput.manualResetCode
        <<" mapping_source="<<static_cast<unsigned>(resetMapping.source)
        <<" scancode="<<resetMapping.scancode<<" preset="<<humanControlInput.preset
        <<" human_course="<<humanCourseEnabled<<" physical_controller="<<usePhysicalGameController<<'\n';
   }else require50DU(sourceCameraInput.bind(*sourceCameraSession,controlPresets),"original camera input bind rejected");
  };
  interactive_publication50DU::Context publicationContext50DU{
   .acceptedRacePresentation=acceptedRacePresentation,
   .accountRcParticleRegistryCall=interactive_frame50DU::BorrowedCall<void()>{accountRcParticleRegistry},
   .accountTbParticleRegistryCall=interactive_frame50DU::BorrowedCall<void()>{accountTbParticleRegistry},
   .activateColdRace=activateColdRace,
   .actualWorld=actualWorld,
   .advanceFrameCall=interactive_frame50DU::BorrowedCall<bool(std::size_t)>{advanceFrame},
   .advanceFramePartCall=interactive_frame50DU::BorrowedCall<bool(std::size_t,interactive_frame50DU::Part,float,float)>{advanceFramePart},
   .advanceNativeGcWaterCall=interactive_frame50DU::BorrowedCall<void()>{advanceNativeGcWater},
   .advanceOriginalAiManagerCall=interactive_frame50DU::BorrowedCall<void(float,const original_all8_frame::Schedule*)>{advanceOriginalAiManager},
   .advanceRcTrackParticlesCall=interactive_frame50DU::BorrowedCall<void()>{advanceRcTrackParticles},
   .advanceSourceAudioWorldCall=interactive_frame50DU::BorrowedCall<void(std::uint64_t,bool)>{advanceSourceAudioWorld},
   .advanceTbTrackParticlesCall=interactive_frame50DU::BorrowedCall<void()>{advanceTbTrackParticles},
   .advanceWorldAnimationsCall=interactive_frame50DU::BorrowedCall<void()>{advanceWorldAnimations},
   .allAiControllerSamples=allAiControllerSamples,
   .allAiEnabled=allAiEnabled,
   .allAiHostDriverCalls=allAiHostDriverCalls,
   .allAiKeyboardPackets=allAiKeyboardPackets,
   .announcerRequestsUnjoined=announcerRequestsUnjoined,
   .applicationClock=applicationClock,
   .applicationEvents=applicationEvents,
   .applyCourseInputCall=interactive_frame50DU::BorrowedCall<void(std::size_t)>{applyCourseInput},
   .applyDynamicSpeechCall=interactive_frame50DU::BorrowedCall<bool(rr::game::RaceActorId50ID,const rr::game::AudioCommand50HE&)>{applyDynamicSpeech},
   .audioDiagnostic=audioDiagnostic,
   .audioEnabled=audioEnabled,
   .audioListener=audioListener,
   .audioListenerReady=audioListenerReady,
   .audioOutputRequested=audioOutputRequested,
   .authored=authored,
   .autoDriveEnabled=autoDriveEnabled,
   .baseTail=baseTail,
   .beginNativeLightDrawCall=interactive_frame50DU::BorrowedCall<void(std::uint32_t,rr::game::original_gc_gameplay_lighting_candidate::DrawLease&)>{beginNativeLightDraw},
   .beginTaRootCall=interactive_frame50DU::BorrowedCall<bool()>{beginTaRoot},
   .boostButtonCode=boostButtonCode,
   .boostState=boostState,
   .cableDiagnostic=cableDiagnostic,
   .cableGpuTextures=cableGpuTextures,
   .cableMesh=cableMesh,
   .cableOwner=cableOwner,
   .cableWorkspace=cableWorkspace,
   .cameraStartup350=cameraStartup350,
   .captureDynamicBeforeFrameCall=interactive_frame50DU::BorrowedCall<void(std::size_t)>{captureDynamicBeforeFrame},
   .captureDynamicBeforeScheduledPhysicsCall=interactive_frame50DU::BorrowedCall<void(const original_all8_frame::Schedule&,std::size_t)>{captureDynamicBeforeScheduledPhysics},
   .captureFirstWrongWay=captureFirstWrongWay,
   .capturePath=capturePath,
   .captureRun=captureRun,
   .capturedFirstWrongWay=capturedFirstWrongWay,
   .checkpointOutput=checkpointOutput,
   .checkpointRequested=checkpointRequested,
   .coldFlame=coldFlame,
   .coldLoadDiagnostics=coldLoadDiagnostics,
   .collisionNormalMode=collisionNormalMode,
   .completedFrames=completedFrames,
   .completionDiagnostic=completionDiagnostic,
   .composePacketCall=interactive_frame50DU::BorrowedCall<void(bool,const rr::platform::WindowInput50DQ&,std::size_t)>{composePacket},
   .consumedGaStage=consumedGaStage,
   .consumedGcStage=consumedGcStage,
   .consumedMaStage=consumedMaStage,
   .consumedRaStage=consumedRaStage,
   .consumedSaStage=consumedSaStage,
   .consumedTaStage=consumedTaStage,
   .controlPresets=controlPresets,
   .controller=controller,
   .courseColdLoad=courseColdLoad,
   .courseDiagnostic=courseDiagnostic,
   .courseOutput=courseOutput,
   .courseRaceCold=courseRaceCold,
   .courseRaceLaunched=courseRaceLaunched,
   .courseTargets=courseTargets,
   .courseTuning=courseTuning,
   .destination=destination,
   .directOriginalHuman=directOriginalHuman,
   .discardedHostNs=discardedHostNs,
   .displayWallTotals=displayWallTotals,
   .drawWorkspace=drawWorkspace,
   .dynamicActorResolver=dynamicActorResolver,
   .dynamicBoundaryCall=interactive_frame50DU::BorrowedCall<void(std::uint32_t,const char*)>{dynamicBoundary},
   .dynamicConsumerFlags11050=dynamicConsumerFlags11050,
   .dynamicContactEnabled=dynamicContactEnabled,
   .dynamicDamageCursor=dynamicDamageCursor,
   .dynamicEventBindings=dynamicEventBindings,
   .dynamicEventCursor=dynamicEventCursor,
   .dynamicEvents=dynamicEvents,
   .dynamicLapCursor=dynamicLapCursor,
   .dynamicMusicProfile=dynamicMusicProfile,
   .dynamicPodEffect=dynamicPodEffect,
   .dynamicProfile=dynamicProfile,
   .dynamicRacer60=dynamicRacer60,
   .dynamicRegistry=dynamicRegistry,
   .engineDamage=engineDamage,
   .engineDamageCompletions=engineDamageCompletions,
   .engineDamageDips=engineDamageDips,
   .engineDamageDraws=engineDamageDraws,
   .engineDamageReturns=engineDamageReturns,
   .engineDamageUpdates=engineDamageUpdates,
   .feedbackHost=feedbackHost,
   .finishDriverAppliedThisFrame=finishDriverAppliedThisFrame,
   .finishDriverTicks=finishDriverTicks,
   .finishManualInputTicks=finishManualInputTicks,
   .finishTaRootCall=interactive_frame50DU::BorrowedCall<bool()>{finishTaRoot},
   .finishTaRootWithResultsCall=interactive_frame50DU::BorrowedCall<bool()>{finishTaRootWithResults},
   .gaAnimatedWorld=gaAnimatedWorld,
   .gaAtm=gaAtm,
   .gaDynamicWindow=gaDynamicWindow,
   .gaOriginalWorld=gaOriginalWorld,
   .gaTriggerActions=gaTriggerActions,
   .gameplayBinder=gameplayBinder,
   .gameplayBinderEnabled=gameplayBinderEnabled,
   .gameplayBinderTemplates=gameplayBinderTemplates,
   .gbAnimationRuntime=gbAnimationRuntime,
   .gbAtm=gbAtm,
   .gbColdSources=gbColdSources,
   .gbDynamicWindow=gbDynamicWindow,
   .gbFogOwner=gbFogOwner,
   .gbLightOwner=gbLightOwner,
   .gbMapView=gbMapView,
   .gbOriginalWorld=gbOriginalWorld,
   .gbParticleAttachments=gbParticleAttachments,
   .gbParticleAuthoring=gbParticleAuthoring,
   .gbParticleRender=gbParticleRender,
   .gbParticleRenderDiagnostic=gbParticleRenderDiagnostic,
   .gbParticleRuntime=gbParticleRuntime,
   .gbParticleWindow=gbParticleWindow,
   .gbQueryBinding=gbQueryBinding,
   .gbRace=gbRace,
   .gbRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{gbRemainingPayloadBytes},
   .gbSourceCameraAssets=gbSourceCameraAssets,
   .gbTrackParticles=gbTrackParticles,
   .gcAnimatedWorld=gcAnimatedWorld,
   .gcAnimatedWorldSteps=gcAnimatedWorldSteps,
   .gcAtm=gcAtm,
   .gcDrawLights=gcDrawLights,
   .gcDynamicWindow=gcDynamicWindow,
   .gcGateAudio=gcGateAudio,
   .gcGraphicsSequence=gcGraphicsSequence,
   .gcLightDiagnostic=gcLightDiagnostic,
   .gcLightOwner=gcLightOwner,
   .gcNativeGate=gcNativeGate,
   .gcNativeWorldSelected=gcNativeWorldSelected,
   .gcOriginalWorld=gcOriginalWorld,
   .gcParticleMaterials=gcParticleMaterials,
   .gcParticleRender=gcParticleRender,
   .gcTriggerActions=gcTriggerActions,
   .gpuTextures=gpuTextures,
   .ground=ground,
   .groundStack=groundStack,
   .headlessLive=headlessLive,
   .host=host,
   .hostDiagnostic=hostDiagnostic,
   .humanControlInput=humanControlInput,
   .humanCourseEnabled=humanCourseEnabled,
   .humanOriginalHud=humanOriginalHud,
   .humanResultsActive=humanResultsActive,
   .humanResultsCountdown=humanResultsCountdown,
   .humanResultsReady=humanResultsReady,
   .installHumanCameraInput50DUCall=interactive_frame50DU::BorrowedCall<void()>{installHumanCameraInput50DU},
   .isGaTrack=isGaTrack,
   .isGbTrack=isGbTrack,
   .isGcTrack=isGcTrack,
   .isMaSaTrack=isMaSaTrack,
   .isMaTrack=isMaTrack,
   .isMbTrack=isMbTrack,
   .isMcTrack=isMcTrack,
   .isRaTrack=isRaTrack,
   .isRcTrack=isRcTrack,
   .isSaTrack=isSaTrack,
   .isScTrack=isScTrack,
   .isTaTrack=isTaTrack,
   .isTbTrack=isTbTrack,
   .isTcTrack=isTcTrack,
   .lapContinuity=lapContinuity,
   .lapOwner=lapOwner,
   .lastCommittedWrongWay=lastCommittedWrongWay,
   .legacyRace=legacyRace,
   .liveAudio=liveAudio,
   .liveWorld=liveWorld,
   .loadedAudioSettings=loadedAudioSettings,
   .loadedTrack=loadedTrack,
   .m=m,
   .maOriginalDisc=maOriginalDisc,
   .maOriginalWorld=maOriginalWorld,
   .maSaAtm=maSaAtm,
   .maShieldUv=maShieldUv,
   .maTriggerActions=maTriggerActions,
   .mainContactFrames=mainContactFrames,
   .mainCrossedPortals=mainCrossedPortals,
   .mainMembership=mainMembership,
   .materialAudio=materialAudio,
   .materialOwner=materialOwner,
   .mbAtm=mbAtm,
   .mbMapView=mbMapView,
   .mbOriginalWorld=mbOriginalWorld,
   .mbRace=mbRace,
   .mbShieldUv=mbShieldUv,
   .mcAnimationRuntime=mcAnimationRuntime,
   .mcAtm=mcAtm,
   .mcColdSources=mcColdSources,
   .mcDynamicWindow=mcDynamicWindow,
   .mcFogOwner=mcFogOwner,
   .mcLightOwner=mcLightOwner,
   .mcMapView=mcMapView,
   .mcOriginalWorld=mcOriginalWorld,
   .mcParticleAttachments=mcParticleAttachments,
   .mcParticleFrame152Notice=mcParticleFrame152Notice,
   .mcParticleRender=mcParticleRender,
   .mcParticleRenderDiagnostic=mcParticleRenderDiagnostic,
   .mcParticleRuntime=mcParticleRuntime,
   .mcParticleSource=mcParticleSource,
   .mcParticleVehicles=mcParticleVehicles,
   .mcParticleWindow=mcParticleWindow,
   .mcQueryBinding=mcQueryBinding,
   .mcRace=mcRace,
   .mcRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{mcRemainingPayloadBytes},
   .mcShieldUv=mcShieldUv,
   .mcSkyOwner=mcSkyOwner,
   .mcSourceCameraAssets=mcSourceCameraAssets,
   .mcTrackParticles=mcTrackParticles,
   .mcWaterOwner=mcWaterOwner,
   .mode1=mode1,
   .mode1Effects=mode1Effects,
   .mode1Enabled=mode1Enabled,
   .monotonicNow50DU=monotonicNow50DU,
   .nativeBottomLeftModels=nativeBottomLeftModels,
   .nativeBottomRightModels=nativeBottomRightModels,
   .nativeCalls=nativeCalls,
   .nativeChariotModels=nativeChariotModels,
   .nativeFireSerial=nativeFireSerial,
   .nativeFireSpanDrawn=nativeFireSpanDrawn,
   .nativeFireSpanIds=nativeFireSpanIds,
   .nativeFireSpanOwner=nativeFireSpanOwner,
   .nativeGcWater=nativeGcWater,
   .nativeIntroRaceCall=interactive_frame50DU::BorrowedCall<rr::game::OriginalRaceConstructor50LA*()>{nativeIntroRace},
   .nativeLeftModels=nativeLeftModels,
   .nativeLightFailureCall=interactive_frame50DU::BorrowedCall<void(const char*,std::uint32_t,const rr::game::original_gc_gameplay_lighting_candidate::DrawLease&)>{nativeLightFailure},
   .nativeLightingPolicy=nativeLightingPolicy,
   .nativeModelDomain=nativeModelDomain,
   .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
   .nativeOriginalFleet=nativeOriginalFleet,
   .nativeRecordActor=nativeRecordActor,
   .nativeRightModels=nativeRightModels,
   .nativeSelectedLights=nativeSelectedLights,
   .nativeTaParticleDiagnostic=nativeTaParticleDiagnostic,
   .nativeTaParticles=nativeTaParticles,
   .nativeVideo=nativeVideo,
   .navigationAI=navigationAI,
   .navigationAIScan=navigationAIScan,
   .navigationAISequential=navigationAISequential,
   .navigationMap=navigationMap,
   .navigationMapScan=navigationMapScan,
   .navigationMapSequential=navigationMapSequential,
   .navigationProgress=navigationProgress,
   .observeOriginalAiFleetCall=interactive_frame50DU::BorrowedCall<void(const original_all8_frame::Schedule*)>{observeOriginalAiFleet},
   .observedNativeFinishFlags=observedNativeFinishFlags,
   .observedNativeTicks=observedNativeTicks,
   .openNativeVideoCall=interactive_frame50DU::BorrowedCall<void()>{openNativeVideo},
   .opponentAudio=opponentAudio,
   .opponentCableGpuTextures=opponentCableGpuTextures,
   .opponentDraw=opponentDraw,
   .opponentFlamePrepared=opponentFlamePrepared,
   .opponentFlames=opponentFlames,
   .opponentFrame=opponentFrame,
   .opponentGpuTextures=opponentGpuTextures,
   .opponentSpecularGpuTextures50LA=opponentSpecularGpuTextures50LA,
   .opponentUvs=opponentUvs,
   .originalAi=originalAi,
   .originalAiAudio=originalAiAudio,
   .originalAiAudioEnabled=originalAiAudioEnabled,
   .originalAiEnabled=originalAiEnabled,
   .originalAiHumanCount=originalAiHumanCount,
   .originalAiOpponentCount=originalAiOpponentCount,
   .originalAiOwnsSlotCall=interactive_frame50DU::BorrowedCall<bool(std::size_t) noexcept>{originalAiOwnsSlot},
   .originalAiReads=originalAiReads,
   .originalAiSession=originalAiSession,
   .originalAiSettings1125C=originalAiSettings1125C,
   .originalCameraEnabled=originalCameraEnabled,
   .originalMain=originalMain,
   .originalMainApplication=originalMainApplication,
   .originalManualResetGraphicsDue=originalManualResetGraphicsDue,
   .originalUiActive=originalUiActive,
   .originalUiStarted=originalUiStarted,
   .pendingSpeech=pendingSpeech,
   .phaseTiming=phaseTiming,
   .physicalControllerReceipt=physicalControllerReceipt,
   .playerCableBindings=playerCableBindings,
   .playerDrawAssets=playerDrawAssets,
   .playerEnvironment=playerEnvironment,
   .playerEvents=playerEvents,
   .playerFlameUvs=playerFlameUvs,
   .playerPodId=playerPodId,
   .playerRacer60=playerRacer60,
   .playerUsesRosterRig=playerUsesRosterRig,
   .poseHistory=poseHistory,
   .powerslideButtonCode=powerslideButtonCode,
   .projectDynamicActorsForSlotCall=interactive_frame50DU::BorrowedCall<void(std::size_t)>{projectDynamicActorsForSlot},
   .publishOriginalAiStandingsCall=interactive_frame50DU::BorrowedCall<void()>{publishOriginalAiStandings},
   .raDiagnostic=raDiagnostic,
   .raSession=raSession,
   .raSessionDiagnostic=raSessionDiagnostic,
   .raWorld=raWorld,
   .race=race,
   .raceAudio=raceAudio,
   .raceBorrow=raceBorrow,
   .raceEnabled=raceEnabled,
   .raceMusic=raceMusic,
   .raceOpponentCount=raceOpponentCount,
   .raceRacerCount=raceRacerCount,
   .racerBank=racerBank,
   .racerCables=racerCables,
   .racerCollisions=racerCollisions,
   .racerContactIds=racerContactIds,
   .racerContactMaterials=racerContactMaterials,
   .racerContactOwner=racerContactOwner,
   .racerContactTotals=racerContactTotals,
   .racerContactsEnabled=racerContactsEnabled,
   .racerFrameReads=racerFrameReads,
   .racerHudNames=racerHudNames,
   .racerNames=racerNames,
   .racerRigs=racerRigs,
   .racerVisuals=racerVisuals,
   .rcAtm=rcAtm,
   .rcColdSources=rcColdSources,
   .rcCollisionWorld=rcCollisionWorld,
   .rcCrystal=rcCrystal,
   .rcLightOwner=rcLightOwner,
   .rcNavigationBorrow=rcNavigationBorrow,
   .rcOriginalDisc=rcOriginalDisc,
   .rcOriginalWorld=rcOriginalWorld,
   .rcParticleAttachmentDiagnostic=rcParticleAttachmentDiagnostic,
   .rcParticleAttachments=rcParticleAttachments,
   .rcParticleAuthoring=rcParticleAuthoring,
   .rcParticleAuthoringDiagnostic=rcParticleAuthoringDiagnostic,
   .rcParticleRegistryFinalAccounted=rcParticleRegistryFinalAccounted,
   .rcParticleRegistryPayloadCharged=rcParticleRegistryPayloadCharged,
   .rcParticleRender=rcParticleRender,
   .rcParticleRenderDiagnostic=rcParticleRenderDiagnostic,
   .rcParticleRuntime=rcParticleRuntime,
   .rcParticleRuntimeDiagnostic=rcParticleRuntimeDiagnostic,
   .rcParticleWindow=rcParticleWindow,
   .rcParticleWindowPayloadReservation=rcParticleWindowPayloadReservation,
   .rcRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{rcRemainingPayloadBytes},
   .rcTrackParticles=rcTrackParticles,
   .rcTriggerActions=rcTriggerActions,
   .rcWorldBorrow=rcWorldBorrow,
   .recoveryPaused=recoveryPaused,
   .refreshAnimatedCandidatesCall=interactive_frame50DU::BorrowedCall<bool()>{refreshAnimatedCandidates},
   .renderDiagnostics=renderDiagnostics,
   .renderedCameraPublication=renderedCameraPublication,
   .renderer=renderer,
   .rendererDiagnostic=rendererDiagnostic,
   .repairActiveTicks=repairActiveTicks,
   .repairButtonCode=repairButtonCode,
   .reportHost50DU=reportHost50DU,
   .reportOriginalAiFleetCall=interactive_frame50DU::BorrowedCall<void(const char*)>{reportOriginalAiFleet},
   .reportRacerContactsCall=interactive_frame50DU::BorrowedCall<void()>{reportRacerContacts},
   .reportRenderer50DU=reportRenderer50DU,
   .requestIntroMusicFade=requestIntroMusicFade,
   .requestStartMusic=requestStartMusic,
   .requestedWindowSamples=requestedWindowSamples,
   .requireMbRuntimeNavigation50MBCall=interactive_frame50DU::BorrowedCall<void()>{requireMbRuntimeNavigation50MB},
   .requireOriginalAiFleetReadyCall=interactive_frame50DU::BorrowedCall<void()>{requireOriginalAiFleetReady},
   .reserveGbAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveGbAdditionalPayload},
   .reserveMcEnvironmentCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveMcEnvironment},
   .reserveRcAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveRcAdditionalPayload},
   .reserveTbAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveTbAdditionalPayload},
   .resolveDynamicSpeechCall=interactive_frame50DU::BorrowedCall<bool(rr::game::RaceActorId50ID,rr::game::DamageSpeechInput50HG&,const rr::game::PodSpeechCounts50HG*&,rr::game::PodSpeechState50HG*&)>{resolveDynamicSpeech},
   .restartRequested=restartRequested,
   .returnOriginalMainRequested=returnOriginalMainRequested,
   .s=s,
   .saAnimatedWorld=saAnimatedWorld,
   .saDynamicWindow=saDynamicWindow,
   .saOriginalDisc=saOriginalDisc,
   .saOriginalWorld=saOriginalWorld,
   .saTriggerActions=saTriggerActions,
   .sampleControllerCall=interactive_frame50DU::BorrowedCall<void(bool)>{sampleController},
   .sampleRaceFieldCall=interactive_frame50DU::BorrowedCall<void()>{sampleRaceField},
   .scAtm=scAtm,
   .scColdSources=scColdSources,
   .scGround=scGround,
   .scLavaUv=scLavaUv,
   .scOriginalDisc=scOriginalDisc,
   .scOriginalWorld=scOriginalWorld,
   .scTriggerActions=scTriggerActions,
   .scZeroSky=scZeroSky,
   .secondaryContactFrames=secondaryContactFrames,
   .secondaryCrossedPortals=secondaryCrossedPortals,
   .secondaryMembership=secondaryMembership,
   .selectedAiCount50MBCall=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedAiCount50MB},
   .selectedAudioSettings=selectedAudioSettings,
   .selectedControls=selectedControls,
   .selectedMapCount50MBCall=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount50MB},
   .selectedMapIndex50MBCall=interactive_frame50DU::BorrowedCall<std::uint8_t(std::uint32_t)>{selectedMapIndex50MB},
   .selectedMapNode50MBCall=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode50MB},
   .selectedRacer60=selectedRacer60,
   .selectedRuntimeHeightDomain=selectedRuntimeHeightDomain,
   .selectedTrack=selectedTrack,
   .sessionIndex=sessionIndex,
   .sessionRandom=sessionRandom,
   .skyGpuTextures=skyGpuTextures,
   .skyOwner=skyOwner,
   .sourceAnnouncer=sourceAnnouncer,
   .sourceApplicationClock=sourceApplicationClock,
   .sourceAttachedGlobals=sourceAttachedGlobals,
   .sourceBlur=sourceBlur,
   .sourceCameraAssets=sourceCameraAssets,
   .sourceCameraCables=sourceCameraCables,
   .sourceCameraCycleAudio=sourceCameraCycleAudio,
   .sourceCameraDisplay=sourceCameraDisplay,
   .sourceCameraInput=sourceCameraInput,
   .sourceCameraSession=sourceCameraSession,
   .sourceCameraTick=sourceCameraTick,
   .sourceConsumedIntroActivation=sourceConsumedIntroActivation,
   .sourceConsumedPauseActivation=sourceConsumedPauseActivation,
   .sourceDamage=sourceDamage,
   .sourceDamageDisplayReported=sourceDamageDisplayReported,
   .sourceDamageObservation=sourceDamageObservation,
   .sourceDamageRendering=sourceDamageRendering,
   .sourceDisplayLifetime=sourceDisplayLifetime,
   .sourceEndingFade=sourceEndingFade,
   .sourceFadeDuration98=sourceFadeDuration98,
   .sourceFadeIn=sourceFadeIn,
   .sourceFadeManagerA8=sourceFadeManagerA8,
   .sourceFadeOut=sourceFadeOut,
   .sourceFogRenderPhase=sourceFogRenderPhase,
   .sourceFollowQueries=sourceFollowQueries,
   .sourceFrameSchedule=sourceFrameSchedule,
   .sourceMainBaseReady=sourceMainBaseReady,
   .sourceModelRenderGlobals=sourceModelRenderGlobals,
   .sourcePause=sourcePause,
   .sourcePauseObservation=sourcePauseObservation,
   .sourcePauseParentAttached=sourcePauseParentAttached,
   .sourcePauseTrack=sourcePauseTrack,
   .sourcePositionStatsDisplayReported=sourcePositionStatsDisplayReported,
   .sourcePositionStatsHud=sourcePositionStatsHud,
   .sourcePositionStatsPrestage=sourcePositionStatsPrestage,
   .sourcePositionStatsPrestageDiagnostic=sourcePositionStatsPrestageDiagnostic,
   .sourcePositionStatsRendering=sourcePositionStatsRendering,
   .sourcePositionStatsResources=sourcePositionStatsResources,
   .sourceRaceHud=sourceRaceHud,
   .sourceRaceHudDisplayReported=sourceRaceHudDisplayReported,
   .sourceRaceHudRejectionReported=sourceRaceHudRejectionReported,
   .sourceRaceHudRendering=sourceRaceHudRendering,
   .sourceRendererDispatch=sourceRendererDispatch,
   .sourceSkySelection=sourceSkySelection,
   .sourceSpeedBoost=sourceSpeedBoost,
   .sourceSpeedBoostAdapter=sourceSpeedBoostAdapter,
   .sourceSpeedBoostDisplayReported=sourceSpeedBoostDisplayReported,
   .sourceSpeedBoostRendering=sourceSpeedBoostRendering,
   .sourceTimeStats=sourceTimeStats,
   .sourceTimeStatsAdapter=sourceTimeStatsAdapter,
   .sourceTimeStatsDisplayReported=sourceTimeStatsDisplayReported,
   .sourceTimeStatsRendering=sourceTimeStatsRendering,
   .sourceTrackFog=sourceTrackFog,
   .sourceVideo=sourceVideo,
   .specularGpuTextures50LA=specularGpuTextures50LA,
   .speechCounts=speechCounts,
   .speechCues=speechCues,
   .speechEvents=speechEvents,
   .speechInputCall=interactive_frame50DU::BorrowedCall<rr::game::DamageSpeechInput50HG()>{speechInput},
   .speechState=speechState,
   .startAtMenu=startAtMenu,
   .startMusicContext=startMusicContext,
   .staticShadowReceiving=staticShadowReceiving,
   .stopFeedbackTransportCall=interactive_frame50DU::BorrowedCall<void()>{stopFeedbackTransport},
   .submittedDrawFrames=submittedDrawFrames,
   .suspendedTaCameraFrame=suspendedTaCameraFrame,
   .taIntroAuthoring=taIntroAuthoring,
   .taIntroPcm=taIntroPcm,
   .taOriginalDisc=taOriginalDisc,
   .taOriginalWorld=taOriginalWorld,
   .tbAiView=tbAiView,
   .tbAtm=tbAtm,
   .tbColdSources=tbColdSources,
   .tbCollisionWorld=tbCollisionWorld,
   .tbDoorBinding=tbDoorBinding,
   .tbDoorRuntime=tbDoorRuntime,
   .tbDynamicWindow=tbDynamicWindow,
   .tbLightOwner=tbLightOwner,
   .tbMapView=tbMapView,
   .tbOriginalDisc=tbOriginalDisc,
   .tbOriginalWorld=tbOriginalWorld,
   .tbParticleRegistryFinalAccounted=tbParticleRegistryFinalAccounted,
   .tbPodSound=tbPodSound,
   .tbRace=tbRace,
   .tbRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{tbRemainingPayloadBytes},
   .tbShieldUv=tbShieldUv,
   .tbSourceCameraAssets=tbSourceCameraAssets,
   .tbTrackParticles=tbTrackParticles,
   .tbTriggerActions=tbTriggerActions,
   .tbWorldBorrow=tbWorldBorrow,
   .tcAiView=tcAiView,
   .tcAtm=tcAtm,
   .tcColdSources=tcColdSources,
   .tcCollisionWorld=tcCollisionWorld,
   .tcDynamicWindow=tcDynamicWindow,
   .tcGateRuntime=tcGateRuntime,
   .tcGateVisibility=tcGateVisibility,
   .tcGlobalSound=tcGlobalSound,
   .tcMapView=tcMapView,
   .tcOriginalDisc=tcOriginalDisc,
   .tcOriginalWorld=tcOriginalWorld,
   .tcRace=tcRace,
   .tcShieldUv=tcShieldUv,
   .tcTriggerActions=tcTriggerActions,
   .tcWorldBorrow=tcWorldBorrow,
   .terrainHitFrames=terrainHitFrames,
   .terrainOwnerCount=terrainOwnerCount,
   .terrainQueries=terrainQueries,
   .terrainSupportedFrames=terrainSupportedFrames,
   .textureDiagnostic=textureDiagnostic,
   .tournamentLaunchSelection=tournamentLaunchSelection,
   .tournamentLaunchStamp=tournamentLaunchStamp,
   .tournamentResultArmed=tournamentResultArmed,
   .tournamentResultCommitted=tournamentResultCommitted,
   .trackAdmission=trackAdmission,
   .trackGpuTextures=trackGpuTextures,
   .trackMaterialDiagnostic=trackMaterialDiagnostic,
   .trackMaterialOwner=trackMaterialOwner,
   .trackMaterialRequest=trackMaterialRequest,
   .trackMesh=trackMesh,
   .trackNormals=trackNormals,
   .trackSession=trackSession,
   .trackSource=trackSource,
   .trackTextureBytes=trackTextureBytes,
   .updateOpponentAudioCall=interactive_frame50DU::BorrowedCall<void()>{updateOpponentAudio},
   .updateOriginalAiPackingCall=interactive_frame50DU::BorrowedCall<void()>{updateOriginalAiPacking},
   .usePhysicalGameController=usePhysicalGameController,
   .windowSwaps=windowSwaps,
   .wrongWayActiveTicks=wrongWayActiveTicks,
   .wrongWayReceipt=wrongWayReceipt,
   .wrongWayUpdates=wrongWayUpdates,
   .x=x,
   .zoneCount=zoneCount,
   .zoneLifecycle=zoneLifecycle,
  };
  publicationContext50DU.run();
 }catch(...){
  const auto failure=std::current_exception();
  const bool closed=closeWindow();
  if(originalCameraEnabled&&!closed&&!closeWindow()){
   std::cerr<<"camera/display cleanup remains unresolved; preserving owners by terminating\n";
   std::terminate();
  }
  try{std::rethrow_exception(failure);}
  catch(const PlayerLoadingCancelled50DU&){return SessionExit50DU::Quit;}
  throw;
 }
 const auto closedNavigationAnchor=originalUiActive?0u:static_cast<unsigned>(
     selectedMapIndex50MB(navigationProgress.reset.vehicleC8C));
 const auto closedMainZone=originalUiActive?0:mainMembership.storage.links[0].zone;
 const auto closedSecondaryZone=originalUiActive?0:secondaryMembership.storage.links[0].zone;
 bool windowClosed=closeWindow();
 if(originalCameraEnabled&&!windowClosed){
  windowClosed=closeWindow();
  if(!windowClosed){
   std::cerr<<"camera/display cleanup remains unresolved; preserving owners by terminating\n";
   std::terminate();
  }
 }
 require50DU(windowClosed,"window/renderer cleanup reported a failure");
 if(originalUiActive){
  const auto mainAudioInfo=originalMain.audioOutput.info();
  J closed={{"native_frames",originalMain.frameCount},{"captures",originalMain.captureSequence},
      {"world_ticks",completedFrames},{"window_swaps",windowSwaps},{"cleanup_complete",true},
      {"audio_mixed_frames",mainAudioInfo.mixedFrames},
        {"audio_source_frame_origin",originalMain.audioOutput.sourceFrameOrigin()},
        {"audio_output_source_mixed_frames",originalMain.audioOutput.outputSourceMixedFrames()},
        {"audio_output_serial",originalMain.audioOutput.retainedOutputSerial()},
      {"audio_source_mixed_frames",originalMain.audioOutput.sourceMixedFrames()},
      {"audio_captured_frames",mainAudioInfo.capturedFrames},{"audio_rate",mainAudioInfo.sampleRate},
      {"audio_clock_microseconds",mainAudioInfo.simulationMicroseconds},
      {"audio_accepted_nanoseconds",originalMain.audioOutput.acceptedNanoseconds()},
      {"audio_device_status",platform::audioDeviceStatusName50HD(mainAudioInfo.deviceStatus)},
      {"audio_device_consumed_frames",mainAudioInfo.deviceConsumedFrames},
      {"audio_device_underrun_frames",mainAudioInfo.deviceUnderrunFrames},
      {"audio_device_overflow_frames",mainAudioInfo.deviceOverflowFrames}};
  std::cout<<"original_main_closed "<<closed.dump()<<'\n';
  return courseColdLoad.pending()?SessionExit50DU::CourseColdLoad:SessionExit50DU::Quit;
 }
 if(raceEnabled)reportRacerContacts();
 std::cout<<(restartRequested?"Development session restarting: ticks=":"Development drive stopped: ticks=")<<completedFrames
          <<" render_commits="<<submittedDrawFrames<<" window_swaps="<<windowSwaps<<" native_calls="<<nativeCalls
          <<" collected_calls="<<collectedCalls<<" peak_collected="<<peakCollected
          <<" main_contact_ticks="<<mainContactFrames<<" secondary_contact_ticks="<<secondaryContactFrames
          <<" repair_active_ticks="<<repairActiveTicks
          <<" main_primary_zone="<<closedMainZone
          <<" secondary_primary_zone="<<closedSecondaryZone
          <<" main_crossed_portals="<<mainCrossedPortals<<" secondary_crossed_portals="<<secondaryCrossedPortals
          <<" nav_C80="<<navigationProgress.reset.vehicleC80<<" nav_C84="<<navigationProgress.vehicleC84
          <<" nav_C88="<<navigationProgress.reset.vehicleC88<<" nav_C8C="<<navigationProgress.reset.vehicleC8C
          <<" nav_anchor_index78="<<closedNavigationAnchor
          <<" nav_updates="<<navigationUpdates<<" nav_gate_skips="<<navigationGateSkips
          <<" nav_CA0_copies="<<navigationCA0Copies
          <<" nav_map_sequential="<<navigationMapSequential<<" nav_AI_sequential="<<navigationAISequential
          <<" nav_map_scan="<<navigationMapScan<<" nav_AI_scan="<<navigationAIScan
          <<" controller_presses="<<controllerPresses<<" controller_releases="<<controllerReleases
          <<" feedback_helpers="<<feedbackHelpers<<" feedback_submissions="<<feedbackSubmissions
          <<" feedback_no_device="<<feedbackNoDevice<<" feedback_lease_refreshes="<<feedbackLeaseRefreshes
          <<" effect_commands="<<effectCommands<<" discarded_host_ns="<<discardedHostNs<<'\n';
 lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
 std::cout<<"Host lap state: completed="<<lapContinuity.state.vehicleCC8<<" marker_cursor="
          <<lapContinuity.state.vehicleFF8<<" callbacks="<<lapOwner.completedCallbacks
          <<" unavailable_sound_calls="<<lapOwner.unavailableSounds<<'\n';
 return originalMainApplication.nativePauseMenuDeparting50LA()?SessionExit50DU::PauseMenuReturn:
        originalMainApplication.nativeResultsHallDeparting50LA()?SessionExit50DU::ResultsHallReturn:
     returnOriginalMainRequested?SessionExit50DU::ReturnOriginalMain:
     checkpointRequested?SessionExit50DU::Checkpoint:(restartRequested?SessionExit50DU::Restart:SessionExit50DU::Quit);
}
} // namespace interactive_continuation50DU

//Definitions stay at the existing lexical Context owner; no new source admission.
namespace rr::game {
bool OriginalRaceConstructor50LA::initializeNativeRacerB0(
    ::interactive_continuation50DU::Context& context,::native_racer50HN::Bank& bank,
    std::uint32_t& pc)noexcept {
    pc=0x1aa548;
    if(resultsFeedbackThread_!=std::this_thread::get_id())return false;
    // Existing lexical native producer only, after its real registered roster
    // and grounded grid creation. This does not enter source1AA580/1AD520.
    if(!context.isRaTrack||!context.originalAiEnabled||context.nativeRacerManager||
       context.consumedRaStage.race().get()!=this||context.originalMainApplication.race28C()!=this||
       application_!=context.originalMainApplication.entities()||!counted_||rolledBack_||
       !world74_||world74_!=context.raWorld||world74_->terminal_||world74_->closed_||
       !world74_->runtimeQueries_||!world74_->source_||!profile_||
       profile_!=context.originalMainApplication.riderProfile||
       context.originalAiHumanCount>1u||context.originalAiHumanCount>bank.count()||
       context.originalAiOpponentCount!=bank.count()-context.originalAiHumanCount)return false;
    const auto& runtime=*world74_->runtimeQueries_;
    if(runtime.terminal_||runtime.closed_||!runtime.cameraBankCurrent()||runtime.bankIdentity_!=&bank)return false;
    try {
        // Original1A6F4C calls2A1160,1A6F54 publishes B8, then1A6F5C
        // loads it. Here the existing native cold producer adopts only the
        // same retained RA projection, BEFORE constructing native B0.
        pc=0x1a6f48;
        if(!mapB8_){
            ra_cold_aggregate50LA::Diagnostic diagnostic;
            auto navigation=world74_->borrowColdNavigation(diagnostic);
            if(!navigation||&navigation->map()!=context.navigationMap)return false;
            auto next=context.originalMainApplication.constructTunnelTrackCold2A1160(
                entity_.sessionGeneration,OriginalTunnelTrackRole50LA::mapB8,std::move(navigation));
            if(!next)return false;
            mapB8_=std::move(next); //Retain actual count if later binding fails.
        }
        pc=0x1a6f5c;
        if(!mapB8_->navigation()&&
           !context.originalMainApplication.bindTunnelTrackColdProjection2A1260(*mapB8_))return false;
        if(mapB8_->navigation()!=context.navigationMap||mapB8_->application_!=application_||
           mapB8_->entity_.sessionGeneration!=entity_.sessionGeneration)return false;
        pc=0x1aa548;
        if(!racerB0_){
            std::unique_ptr<OriginalRaceRacerManager50LA> next(new OriginalRaceRacerManager50LA(
                application_,entity_.sessionGeneration,bank,context.s,context.originalAi,context.originalAiHumanCount));
            if(!next->constructed()){pc=0x1ad440;return false;}
            // Explicit native typed B0 publication after actual counted ctor.
            // No guest malloc result, loaded44/3C or full Race load is inferred.
            racerB0_=std::move(next);
        }
        return racerB0_->associateNativeRoster(*this,bank,context.s,context.originalAi,
            context.originalAiHumanCount,pc);
    }catch(...){return false;} // Retain published count and every reached effect.
}
bool OriginalRaceConstructor50LA::initializeNativeTimingB4(
    ::interactive_continuation50DU::Context& context,::native_racer50HN::Bank& bank,
    std::uint32_t& pc)noexcept {
    pc=0x1a7710;
    if(resultsFeedbackThread_!=std::this_thread::get_id())return false;
    //This is the actual EXISTING native cold manager producer, not permission
    //to skip the incomplete original1A6BF0/world/controller/attachment tails.
    if(!context.isRaTrack||!context.originalAiEnabled||context.consumedRaStage.race().get()!=this||
       context.originalMainApplication.race28C()!=this||application_!=context.originalMainApplication.entities()||
       !counted_||rolledBack_||!world74_||world74_!=context.raWorld||
       world74_->terminal_||world74_->closed_||!world74_->runtimeQueries_||!world74_->source_)return false;
    const auto& runtime=*world74_->runtimeQueries_;
    if(runtime.terminal_||runtime.closed_||!runtime.cameraBankCurrent()||runtime.bankIdentity_!=&bank)return false;
    const auto* source=world74_->source_->sourcesDecoded();
    if(!source||context.navigationMap!=&source->map||!source->mapMetadata.bound||
       source->map.track.count<2u||!profile_||
       profile_!=context.originalMainApplication.riderProfile)return false;
    try {
        if(!timingB4_){
            std::unique_ptr<OriginalRaceTimingManager50LA> next(new OriginalRaceTimingManager50LA(
                application_,entity_.sessionGeneration,context.originalAiSession,bank));
            if(!next->constructed()){pc=0x1af7f0;return false;}
            timingB4_=std::move(next); //Actual1A7730 publication BEFORE subtype/load.
        }
        return timingB4_->continueNativeAssociation(*this,context.originalAiSession,bank,
            source->map,source->mapMetadata,pc);
    }catch(...){return false;} //Retain published B4/count and any reached effects.
}
}

namespace original_race_hud_scope50LA {
void Registration::continuePreAck1AB520(rr::game::OriginalRaceConstructor50LA& race,
    rr::game::OriginalRaceEndEntryDiagnostic50LA& d)noexcept {
    using S=rr::game::OriginalRaceEndEntryDiagnostic50LA::Status;
    d.status=S::owner;d.sourcePC=0x1ab520;
    d.operation="Actual same-Race lexical Camera/Bank/manager lifetime unavailable";
    if(std::this_thread::get_id()!=thread_)return; //Before mutable scope/weak reads.
    if(!active_||!published_||!hud_||!manager_||!bank_||!camera_)return;
    const auto retained=race_.lock();
    if(retained.get()!=&race||application_->race28C()!=&race||application_->entities()!=race.application_||
       !race.counted_||race.rolledBack_||race.entity_.sessionGeneration!=generation_||
       race.endHud_.lock().get()!=this||!race.world74_)return;
    try {
        const auto world=race.world74_;ra_cold_aggregate50LA::Diagnostic wd;
        const auto camera=world->borrowRuntimeSourceCamera(world,wd);
        if(!camera||camera.get()!=camera_||&camera->actual_.bank!=bank_||
           camera->actual_.raceHud!=hud_||camera->generation_!=generation_||
           camera->actual_.applicationEntities!=race.application_)return;
        //No new permanent observer or shadow rows. The same existing private
        //read facet binds synchronously to these actual lifetime-pinned owners.
        original_position_stats_adapter50LA::Owner observer;
        original_position_stats_adapter50LA::Owner::Diagnostic observed;
        if(!observer.bind(*camera,*manager_,observed))return;
        race.continuePreAck1AB520(observer,d);
    }catch(...){/*Borrow/bind failures precede the noexcept source continuation.*/}
}
}

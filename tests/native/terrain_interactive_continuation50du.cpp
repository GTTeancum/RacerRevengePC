#include "native_last_failure50pd.h"
#include "native_course_archive_release50pd.h"
#include "native_wreck_debris_sim50pd.h"
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "native_common_results50pd.h"
#include "native_common_race50pd.h"
#include "native_race_replay50pd.h"
#include "native_wreck_debris_gpu50pd.h"
#include "native_water_surface50ws.h"
#include "../../src/game/native_destroyed_racers50pd.h"
#include "../../src/game/n64_track_hazards_runtime50nh.h"
#include "../../src/game/native_chariot_rigid_mount50rm.h"
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
#include "../../src/game/dlc_rider_entries50dl.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "terrain_interactive_cold_contacts50du.h"
#include "player_loading_session50du.h"
#include "cold_load_diagnostics50du.h"
#include "pod_cables50fd_fixture.h"
// Common rewrite: the one stage, track, roster, lap tracker, race start/end.
#include "original_consumed_stage50du.h"
#include "common_track50cr.h"
#include "common_race_world50cr.h"
#include "common_roster50cr.h"
#include "common_lap50cr.h"
#include "common_race_start50cr.h"
#include "common_race_end50cr.h"
#include "common_race_log50cr.h"
// NEW types of the common rewrite (R3/R5/R7; names frozen by W0) and the one
// racer cold-visibility retirement (W0 request i3: generalised tb/tc/gb/mc_cold_visibility).
#include "track_collision_world50cr.h"
#include "single_pass_world50lm.h"
#include "track_atm50cr.h"
#include "positional_sound_rows50sr.h"
#include "racer_cold_visibility50cr.h"
#include "gc_gate_audio50in_fixture.h" // triggerAudio (the PodSound voice) is reset here
#include "track_source_fixture.h" // trackSource (publication context member)
#include "selected_grid_navigation50ig.h"
#include "native_racer_grid50hn_fixture.h" // groundGridPose/placeColdGrid (record actor rank0, native_racer_grid50hn.inc)
#include "source_grid50ig_fixture.h"
#include "native_water_runtime50jc_fixture.h"
#include "water_resources50jc_fixture.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_track_fog50cr.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "original_track_sky50kx_fixture.h"
#include "native_racer_model_lifetime50la_fixture.h"
#include "original_results_feedback_scope50la.h"
#include "original_race_hud_scope50la.h"
#include "original_position_stats_adapter50la.h"
#include "original_native_ta_root_audio50la.h"
#include "track_animation_binding50an.h"
#include "native_cold_model_retirement50lh.h"
#include "original_ai_physical_recovery.h"
#include "original_human_physical_recovery.h"
#include "gc_particle_materials50kg_fixture.h"
#include "native_gc_other_particle_children50kg_fixture.h" // the water racer's particle children (bindCold)
#include "original_position_stats_prestage50la.h"
#include "../../src/game/original_static_model_params50ps.h"
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
    if(!ok){native_last_failure50PD::record(message);std::cerr<<"require_failed "<<(message?message:"")<<std::endl;throw std::runtime_error(message);}
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
 auto& track=this->track;
 auto& laps=this->laps;
 auto& roster=this->roster;
 auto& raceStart=this->raceStart;
 auto& raceEnd=this->raceEnd;
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
 const auto& closeCurrentCollision=this->closeCurrentCollisionCall;
 const auto& closeCurrentMemberships=this->closeCurrentMembershipsCall;
 auto& coldFlame=this->coldFlame;
 auto& coldLoadDiagnostics=this->coldLoadDiagnostics;
 const auto& collectRegistered=this->collectRegisteredCall;
 auto& collectedCalls=this->collectedCalls;
 auto& collisionNormalMode=this->collisionNormalMode;
 const auto& commitCandidateEffects=this->commitCandidateEffectsCall;
 auto& completedFrames=this->completedFrames;
 const auto& composePacket=this->composePacketCall;
 const auto& consumeEffects=this->consumeEffectsCall;
 auto& consumedStage=this->consumedStage;
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
 auto& rootedCourseRace=this->rootedCourseRace;
 auto& courseRaceLaunched=this->courseRaceLaunched;
 auto& courseState=this->courseState;
 auto& courseTuning=this->courseTuning;
 auto& destination=this->destination;
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
 auto& trackAnimationMembership=this->trackAnimationMembership;
 auto& trackAnimations=this->trackAnimations;
 auto& trackAnimationBinding=this->trackAnimationBinding;
 auto& trackAnimationBindingDiagnostic=this->trackAnimationBindingDiagnostic;
 auto& trackAnimationPrefixArmed=this->trackAnimationPrefixArmed;
 auto& trackAnimationDiagnostic=this->trackAnimationDiagnostic;
 auto& triggerAudio=this->triggerAudio;
 auto& triggerMembership=this->triggerMembership;
 const auto& gatherRegistered=this->gatherRegisteredCall;
 auto& aiView=this->aiView;
 auto& collisionCleanupDone=this->collisionCleanupDone;
 auto& mapView=this->mapView;
 const auto& remainingPayloadBytes=this->remainingPayloadBytesCall;
 auto& drawLights=this->drawLights;
 auto& graphicsSequence=this->graphicsSequence;
 auto& lightDiagnostic=this->lightDiagnostic;
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
 // The consumed Race of every course owns constructor1A6660's +94 ending
 // clock. Bind the manager/camera/ending-clock users to THAT scalar; never copy
 // the Setup timer or reset94 here. One fail-closed contract for every course:
 // the race must be the current counted race whose World at race+74 holds this
 // session's Track (1A6BC0 -> 1B2870).
 const auto humanEndRace94Candidate=!humanCourseEnabled?std::shared_ptr<OriginalRaceConstructor50LA>{}:
     consumedStage.race();
 const auto humanEndRace94Current=[&](const OriginalRaceConstructor50LA& race){
  return race.resultsFeedbackThread_==std::this_thread::get_id()&&this->nativeOriginalFleet&&
      this->originalMainApplication.race28C()==&race&&
      race.application_==this->originalMainApplication.entities()&&
      race.profile_==this->originalMainApplication.riderProfile&&
      race.counted_&&!race.rolledBack_&&race.constructorSuffix_&&
      race.entity_.sessionGeneration==this->sessionIndex+1&&
      race.world74_&&race.world74_->trackData()==&this->track;
 };
 if(humanEndRace94Candidate)
  require50DU(humanEndRace94Current(*humanEndRace94Candidate),
      "human ending94 requires its actual current consumed Race/Application/world");
 const auto& humanEndRace94=humanEndRace94Candidate;
 auto& humanResultsCountdown=humanEndRace94?humanEndRace94->field94_:this->humanResultsCountdown;
 auto& humanResultsReady=this->humanResultsReady;
 auto& initial=this->initial;
 auto& initialCollisionZone=this->initialCollisionZone;
 auto& initialMetadata=this->initialMetadata;
 auto& initialPairs=this->initialPairs;
 auto& keyboardDiagnostic=this->keyboardDiagnostic;
 auto& lapContinuity=this->lapContinuity;
 auto& lapOwner=this->lapOwner;
 auto& lastCommittedWrongWay=this->lastCommittedWrongWay;
 auto& liveAudio=this->liveAudio;
 auto& liveWorld=this->liveWorld;
 auto& loadedAudioSettings=this->loadedAudioSettings;
 auto& loadedTrack=this->loadedTrack;
 auto& m=this->m;
 auto& mainContactFrames=this->mainContactFrames;
 auto& mainCrossedPortals=this->mainCrossedPortals;
 auto& mainLiveQuery=this->mainLiveQuery;
 auto& mainMembership=this->mainMembership;
 auto& mainMembershipStorage=this->mainMembershipStorage;
 auto& mainQueryRef=this->mainQueryRef;
 auto& managerRuntimeSelection=this->managerRuntimeSelection;
 auto& materialAudio=this->materialAudio;
 auto& materialOwner=this->materialOwner;
 auto& waterUpdateSequence=this->waterUpdateSequence;
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
 const auto& nativeLightFailure=this->nativeLightFailureCall;
 auto& nativeLightingPolicy=this->nativeLightingPolicy;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
 auto& nativeParticleAuthoring=this->nativeParticleAuthoring;
 auto& nativeParticleChildSequence=this->nativeParticleChildSequence;
 auto& nativeParticleChildren=this->nativeParticleChildren;
 auto& nativeRacerManager=this->nativeRacerManager;
 auto& nativeRecordActor=this->nativeRecordActor;
 auto& nativeSelectedLights=this->nativeSelectedLights;
 auto& nativeVideo=this->nativeVideo;
 auto& nativeWaterApplication=this->nativeWaterApplication;
 auto& nativeWaterRacers=this->nativeWaterRacers;
 auto& navigationAIScan=this->navigationAIScan;
 auto& navigationAISequential=this->navigationAISequential;
 auto& navigationCA0=this->navigationCA0;
 auto& navigationCA0Copies=this->navigationCA0Copies;
 auto& navigationDiagnostic=this->navigationDiagnostic;
 auto& navigationGateSkips=this->navigationGateSkips;
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
 auto& retireEndCandidateProducer=this->retireEndCandidateProducer;
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
 const auto& requireRuntimeNavigation=this->requireRuntimeNavigationCall;
 const auto& reserveAdditionalPayload=this->reserveAdditionalPayloadCall;
 auto& resetEffectBindings=this->resetEffectBindings;
 auto& resetEffectState=this->resetEffectState;
 auto& resetEffectWorkspace=this->resetEffectWorkspace;
 auto& resetGlobals=this->resetGlobals;
 auto& resetHumanCamera=this->resetHumanCamera;
 auto& resetLifecycle=this->resetLifecycle;
 auto& response=this->response;
 const auto& retireAudioOwners=this->retireAudioOwnersCall;
 auto& s=this->s;
 const auto& sampleControllerView=this->sampleControllerCall;
 auto& membershipCleanupDone=this->membershipCleanupDone;
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
 const auto& selectedMapCount=this->selectedMapCountCall;
 const auto& selectedMapIndex=this->selectedMapIndexCall;
 const auto& selectedMapNode=this->selectedMapNodeCall;
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
 ArmedFlagView trackAnimationPrefixGuard{trackAnimationPrefixArmed};
 // Isolated two-phase seam. Prefix stops at the real body-child cut; suffix
 // retains the existing one-substep physical/query owners. The legacy wrapper
 // below remains for unchanged diagnostics until the root joins the real race
 // child traversal through original_all8_frame::Schedule.
 using NativeRacerFramePart=interactive_frame50DU::Part;
 // The earlier same display/audio stage outlives Bank/Join and their retained
 // native model owners on unwind. RA opens that same Video context below.
 // Original 1A6EC4 follows the SimpleVCR replay setup (287430/287710/2878F0). The
 // port records replays natively with no load-time owner; stand-in point.
 rr::platform::loading_progress50DU::step(.1f,0x1a6ec4);
 rr::platform::loading_progress50DU::pulse("Preparing race owners");
 original_main_entry50KG::SpeedBoostColdResult50LA sourceSpeedBoost;
 original_main_entry50KG::TimeStatsColdResult50LA sourceTimeStats;
 original_main_entry50KG::DamageColdResult50LA sourceDamage;
 coldLoadDiagnostics.begin("roster_runtime");
 // The original renderer (442F70) is up from boot, before any course or racer
 // loads. Every course opens the same Video host and brings the race display
 // up here; the race load's camera start (1A9590 cold) may drain it before the
 // racers exist. Content is created later on this same renderer.
 openNativeVideo();
 {
  rr::platform::VehicleWindowDiagnostic50DS upDiagnostic;
  if(!sourceDisplayLifetime->bringUp(headlessLive?rr::platform::VehicleWindowOutput50DS::headless:
      rr::platform::VehicleWindowOutput50DS::defaultBackBuffer,upDiagnostic)){
   reportRenderer50DU(upDiagnostic);require50DU(false,"race display bring-up rejected");
  }
  std::cerr<<"race_display_bring_up course_cold=1\n";
 }
 native_racer50HN::Bank racerBank;
 // The one lighting owner of this race's track (05.13 SCB Lights, R6).
 require50DU(static_cast<bool>(track.roles.lights),"race track lighting owner missing");
 auto& lightOwner=*track.roles.lights;
 // One bank collision close and one trigger-membership close for every course
 // (the one collision world, track.roles.collision; R3).
 auto closeBankCollision=[&]() noexcept -> bool {try{
  if(collisionCleanupDone)return true;
  if(racerBank.count()){
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(!racerBank.visit(slot,closeCurrentCollision))return false;
  }else if(!closeCurrentCollision())return false;
  collisionCleanupDone=true;return true;
 }catch(...){return false;}};
 struct BankCollisionGuard {
  decltype(closeBankCollision)& close;
  ~BankCollisionGuard()noexcept{if(!close())std::terminate();}
 } bankCollisionGuard{closeBankCollision};
 auto closeTriggerMemberships=[&]() noexcept -> bool {try{
  if(membershipCleanupDone)return true;
  if(racerBank.count()){
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(!racerBank.visit(slot,closeCurrentMemberships))return false;
  }else if(!closeCurrentMemberships())return false;
  membershipCleanupDone=true;return true;
 }catch(...){return false;}};
 // Destroyed after the later model retirement guard, before the actual Bank.
 struct BankMembershipGuard {
  decltype(closeTriggerMemberships)& close;
  ~BankMembershipGuard()noexcept{if(!close())std::terminate();}
 } bankMembershipGuard{closeTriggerMemberships};
 // The racers' native model owners (chariot, bottom-left/right, left, right per
 // slot), filled by the cold owners at the real Rig.install for every course.
 // Canonical models borrow this Bank; retire them before Bank destruction even if
 // cold construction throws before the later renderer/closeWindow boundary.
 struct NativeRosterModels50DU {
  std::array<std::unique_ptr<native_racer_model50LA::Owner>,8> right,left,bottomRight,bottomLeft,chariot;
 } nativeRosterModels;
 native_racer50HN::BankBoundModelRetirement50LH<NativeRosterModels50DU>
     nativeColdModelRetirement(nativeRosterModels,true);
 // One cold racer prefix for every course (the MC/GB aggregate shape). This
 // guard begins before any Bank enrollment and captures only owners already
 // declared. The later full-session guard retires contacts first and marks this
 // predecessor complete; an early failure has no live query yet.
 bool bankPrefixClosed=false;
 auto closeBankRegistries=[&]()noexcept->bool {try{
  if(track.roles.lights.get()!=&lightOwner)return false;
  // Track-animation rows hold the lighting static-busy prefix. Before any 7F0
  // retirement, remove those registrations while their backing still lives.
  if(trackAnimationBinding&&trackAnimationBinding->needsClose()&&!trackAnimationBinding->close(trackAnimationBindingDiagnostic))return false;
  if(!nativeColdModelRetirement.close())return false;
  for(std::size_t slot=0;slot<racerBank.count();++slot)if(!racerBank.visit(slot,[&](){
   const bool visibilityRetired=racerRigs[slot]&&racerCables[slot]?
       racer_cold_visibility50CR::Retirement::retireSlot(racerBank,slot,s,x.bindings,x.parameters,
           *racerRigs[slot],*racerCables[slot],lightOwner,lightDiagnostic):
       racer_cold_visibility50CR::Retirement::untouchedSlot(racerBank,slot,lightOwner,lightDiagnostic);
   return visibilityRetired&&closeCurrentCollision();
  }))return false;
  if(!racerBank.count()&&!closeCurrentCollision())return false;
  collisionCleanupDone=true;return true;
 }catch(...){return false;}};
 auto closeBankPrefix=[&]()noexcept->bool {try{
  if(bankPrefixClosed)return true;
  if(!closeBankRegistries())return false;
  if(!racerBank.retireNativeBacking())return false;
  bankPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct BankPrefixGuard {
  decltype(closeBankPrefix)& close;
  ~BankPrefixGuard()noexcept{if(!close()){
   std::fputs("race Bank prefix retirement rejected; backing retained until fail-stop\n",stderr);std::terminate();}}
 } bankPrefixGuard{closeBankPrefix};

 // Non-owning frame view: retain references to the same locals, including
 // owner/callback variables that are populated later in this cold session.
 interactive_frame50DU::Context frameContext50DU{
  .track=track,
  .laps=laps,
  .originalAiAudioEnabled=originalAiAudioEnabled,
  .originalUiActive=originalUiActive,
  .nativeSelectedLights=nativeSelectedLights,
  .graphicsSequence=graphicsSequence,
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
  .mapView=mapView,
  .aiView=aiView,
  .liveWorld=liveWorld,
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
  .originalRecoveryEnabled=originalRecoveryEnabled,
  .originalResetGround=originalResetGround,
  .racerCables=racerCables,
  .originalAiAudio=originalAiAudio,
  .zoneQueue=zoneQueue,
  .zoneLifecycle=zoneLifecycle,
  .zoneScratch=zoneScratch,
  .trackCounts=trackCounts,
  .selectedRuntimeHeightDomain=selectedRuntimeHeightDomain,
  .terrainOwner=terrainOwner,
  .nativeWaterRacers=nativeWaterRacers,
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
  .sourceRaceHud=sourceRaceHud,
  .dynamicContactEnabled=dynamicContactEnabled,
  .dynamicProfile=dynamicProfile,
  .boostParameters=boostParameters,
  .opponentBoostAudio=opponentBoostAudio,
  .liveAudio=liveAudio,
  .boostDiagnostic=boostDiagnostic,
  .lightDiagnostic=lightDiagnostic,
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
  .requireRuntimeNavigation=interactive_frame50DU::BorrowedCall<void()>{requireRuntimeNavigation},
  .refreshAnimatedCandidates=interactive_frame50DU::BorrowedCall<bool()>{refreshAnimatedCandidates},
  .recoveryQueries=interactive_frame50DU::BorrowedCall<rr::game::UndertrackQueryOwners50EW()>{recoveryQueries},
  .reportNavigation=interactive_frame50DU::BorrowedCall<bool(bool,const char*)>{reportNavigation},
  .selectedMapCount=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount},
  .selectedMapNode=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode},
  .refreshDynamicSelfActor=interactive_frame50DU::BorrowedCall<void()>{refreshDynamicSelfActor},
  .dynamicBoundary=interactive_frame50DU::BorrowedCall<void(std::uint32_t,const char*)>{dynamicBoundary},
  .speechInput=interactive_frame50DU::BorrowedCall<rr::game::DamageSpeechInput50HG()>{speechInput},
  .consumeEffects=interactive_frame50DU::BorrowedCall<bool(const rr::game::ImpactEffectEvent50B*,std::size_t)>{consumeEffects},
  .originalAiEnabled=originalAiEnabled,
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
 // The lower arrays stay empty for the ordinary three-model roster members.
 auto& nativeRightModels=nativeRosterModels.right;
 auto& nativeLeftModels=nativeRosterModels.left;
 auto& nativeBottomRightModels=nativeRosterModels.bottomRight;
 auto& nativeBottomLeftModels=nativeRosterModels.bottomLeft;
 auto& nativeChariotModels=nativeRosterModels.chariot;
 std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> nativeModelDomain;
 // Course can reuse this exact cold roster after leaving the frontend.
 // Construct its owners now at the real Rig.install, including UI-active
 // startup; never replay construction over the installed graph at Course.
 // Every course (body694.md 4.6, the former course list is retired): the
 // racer340 bodies exist whenever the original camera and Application run.
 // On the shared world a missing body refuses at cold.
 if(originalCameraEnabled&&originalMainApplication.enabled){
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
  // The one navigation store must be runtime-valid for every course (2A1260/1ACF70).
  requireRuntimeNavigation();
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
   // One map view for every course (common_track50CR::NavigationView, sized from m<code>.spl);
   // its AI metadata is a view over the same navigation storage (R4), not a second store.
   originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,mapView,rr::game::metadataView50CR(mapView));
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
  // PORT EXTENSION, NOT ORIGINAL (n64_track_hazards_runtime50nh.h): a racer
  // waiting for its N64 respawn (column wreck, AfterDestroy = Respawn) is not
  // retired by 1B06F4 (Revenge has no respawn; its retirement is permanent).
  if(rr::game::n64HazardRuntime50NH.anyRespawnArmed())
   for(std::size_t slot=0;slot<racerBank.count()&&slot<std::size(originalAiSession.rankInput);++slot)
    if(rr::game::n64HazardRuntime50NH.respawnArmed(slot)&&!originalAiSession.rankInput[slot].retirementCD4)
     originalAiSession.rankInput[slot].vehicleState40=0u;
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
  std::cerr<<"original_ai_selection track="<<track.code()
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

 // One race session for every course, its node arrays sized from the map
 // view's node count (race_session50HM::CourseSession, R4).
 race_session50HM::CourseSession raceSession{};
 auto& race=raceSession;
 const race_session50HM::SessionBorrow raceBorrow(raceSession);
 const auto withRace=[&](auto&& operation)->decltype(auto){
  requireRuntimeNavigation();
  return operation(raceSession);
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
   // One positional-sound owner (AUB/SCB sound rows, 271600; R7). It exists
   // exactly when the track's files author positional sounds. Actual committed
   // listener, shared World4D8 and process RNG; runs once at the existing
   // countdown/committed-world boundary before the shared mix. Pause exits
   // before this boundary.
   if(track.roles.sounds){
    require50DU(track.roles.sounds->ready(),"track positional audio lost its actual retained source");
    const auto positional=track.roles.sounds->service(&listener,1,s.noiseOwner());
    require50DU(positional.complete,"track positional audio service rejected");
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
 // One cold prefix for every course (the MC/GB aggregate shape): the
 // collision world's query fence, the Bank registries, the lap tracker's view
 // of the track, then the Bank's native backing. Role owners themselves are
 // released once, by the Track (common_track50CR::Track::release, R3).
 // Set by the cold contacts once the navigation store is bound to the live
 // collision world (1ACF70); its membership close is the one bank guard above; closeColdPrefix
 // below unbinds the trigger action owner it bound.
 bool coldNavigationBound=false;
 bool coldPrefixClosed=false,coldRegistriesClosed=false;
 auto closeColdPrefix=[&]()noexcept->bool {try{
  if(coldPrefixClosed)return true;
  // Fence the one collision world (its trigger/query service stops; 1B2870 retire order).
  if(track.roles.collision&&!track.roles.collision->beginRetirement())return false;
  // Once the cold contacts bound the one trigger action owner (destinations,
  // racer identities), it is unbound here on every exit, failure included (the
  // old MB cold closer). Its query bracket belongs to the collision world, so
  // beginRetirement above is the unbind (unbindQuery); a direct unbindQuery would
  // desynchronise that bracket. Confirm the owner left service before the Bank
  // registries go; Track::release closes it after retireContacts.
  if(coldNavigationBound){
   const auto* actions=track.roles.triggerActions.get();
   if(!track.roles.collision||!actions||actions->ready()||!actions->queryIdle())return false;
   coldNavigationBound=false;
  }
  // The Bank registries visit every slot, so they close before the terminal sweep
  // below raises the Bank's terminal fence (which stays set and refuses visits).
  if(!coldRegistriesClosed){if(!closeBankRegistries())return false;coldRegistriesClosed=true;}
  // One terminal all-peer contact sweep of the one collision world over the racer
  // Bank (the retired MC/GB query owners' retireContacts, every course): detaches
  // every racer query row, removes every trigger membership, clears the contact
  // pairs. Without a sealed field there is no peer to sweep; Track::release runs
  // the Bank-less retireContacts.
  if(track.roles.collision&&racerBank.count()){
   if(!track.roles.collision->retireContacts(racerBank))return false;
   // The sweep removed (and verified) every trigger membership of every peer; the
   // Bank now refuses visits (terminal fence) and its backing retires below, so the
   // serial membership closer has nothing left to visit.
   membershipCleanupDone=true;
  }
  if(!lapOwner.releaseTrack())return false;
  laps.release();
  if(!racerBank.retireNativeBacking())return false;
  bankPrefixClosed=true;coldPrefixClosed=true;return true;
 }catch(...){return false;}};
 struct ColdPrefixGuard {
  decltype(closeColdPrefix)& close;bool armed=true;
  ~ColdPrefixGuard()noexcept {if(armed&&!close()){
   std::fputs("race Bank cold-prefix retirement rejected; retained until fail-stop\n",stderr);std::terminate();}}
 } coldPrefixGuard{closeColdPrefix};
 if(raceEnabled){
  std::array<native_racer_contact50HW::Binding,raceRacerCapacity> contactBindings{};
  // The actual Tournament profile and authenticated field authorize ranks;
  // manager/Bank slot identity is never reordered to obtain a start position.
  const bool canonicalTournamentGrid=[&](){
   if(!originalUiStarted||originalUiActive||!humanCourseEnabled||
      !rootedCourseRace)return false;
   using Profile=rr::platform::OriginalUiRiderProfileOwner50KG;
   const auto* profile=originalMainApplication.riderProfile.get();
   Profile::CourseRaceSelection selected{};
   require50DU(profile&&profile->courseRaceSnapshot(selected),
       "Tournament grid requires the actual retained Course profile");
   if(selected.mode34!=2)return false;
   const auto stamp=profile->stamp();
   Profile::TournamentCareerSnapshot career{};
   OriginalUiCourseRememberedProfile50KG course{};
   require50DU(rootedCourseRace&&profile->selectedTournamentCourseSnapshot(career,course)&&
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
  .track=track,
  .roster=roster,
  .laps=laps,
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
  .consumedStage=consumedStage,
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
  .trackAnimationMembership=trackAnimationMembership,
  .trackAnimationBinding=trackAnimationBinding,
  .trackAnimationBindingDiagnostic=trackAnimationBindingDiagnostic,
  .triggerMembership=triggerMembership,
  .aiView=aiView,
  .mapView=mapView,
  .humanBaseUndertrack=humanBaseUndertrack,
  .humanCourseEnabled=humanCourseEnabled,
  .initial=initial,
  .initialMetadata=initialMetadata,
  .initialPairs=initialPairs,
  .keyboardDiagnostic=keyboardDiagnostic,
  .lapContinuity=lapContinuity,
  .lapOwner=lapOwner,
  .m=m,
  .mainContactFrames=mainContactFrames,
  .mainCrossedPortals=mainCrossedPortals,
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
  .nativeVideo=nativeVideo,
  .navigationAIScan=navigationAIScan,
  .navigationAISequential=navigationAISequential,
  .navigationCA0=navigationCA0,
  .navigationCA0Copies=navigationCA0Copies,
  .navigationDiagnostic=navigationDiagnostic,
  .navigationGateSkips=navigationGateSkips,
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
  .requireRuntimeNavigation=interactive_frame50DU::BorrowedCall<void()>{requireRuntimeNavigation},
  .resetEffectBindings=resetEffectBindings,
  .resetEffectState=resetEffectState,
  .resetEffectWorkspace=resetEffectWorkspace,
  .response=response,
  .s=s,
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
  // B-C06: the counted race of every course carries the human race audio.
  .nativeHumanAudio=audioEnabled&&originalUiStarted&&consumedStage.race()?liveAudio.get():nullptr,
  .nativeHumanSpeechCounts=&speechCounts,
 }.run();
 // Original 1A7450: after racers/pods (1750A0/1752D0) and audio\announce.res.
 rr::platform::loading_progress50DU::step(.1f,0x1a7450);
 // Bind once to the selected authenticated map representation. Placement is
 // shared across human/opponents; no per-racer track dispatch or null fallback.
 source_grid50IG::SelectedMapGrid selectedGridNavigation;
 if(trackSession->originalStaticCandidates||canonicalTournamentGrid){
  requireRuntimeNavigation();
  // One map view for every course (m<code>.spl through 2A1260).
  const bool bound=selectedGridNavigation.bindView(mapView);
  require50DU(bound,"selected authenticated map has no cold grid capability");
 }
 //1AE9EC requests rank0 from the same authenticated course, not the
 //player's rank4 transform. Non-Tournament TA HOST placement stays unchanged.
 if(nativeRacerManager){
  require50DU(nativeRacerManager->count30()==racerBank.count()&&racerBank.count()==1u&&
      managerRuntimeSelection.rider==rr::game::dlc50DL::profileRider(playerRacer60)&&
      (managerRuntimeSelection.packed1125C>>11u)==rr::game::dlc50DL::profileRider(playerRacer60),
      "record actor requires its actual selected rider and singleton bank");
  source_grid50IG::SelectedMapGrid recordGrid=selectedGridNavigation;
  if(!trackSession->originalStaticCandidates){
   requireRuntimeNavigation();
   require50DU(recordGrid.bindView(mapView),"record actor original rank0 map unavailable");
  }
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
  // One grounded grid placement for every course and every slot (B-C18).
  if(playerUsesRosterRig)coldRacerStage50DU(playerPodId,"human grounded grid placement",[&](){
   placeOpponentAtGrid50HN(0);
   if(humanCourseEnabled)initializeOriginalHumanResetTimers50LC(playerEvents);
   require50DU(query.resource==&racerCollisions[0]->main()&&
       secondaryQuery.resource==&racerCollisions[0]->secondary()&&
       mainMembership.lastType2Resource==query.resource&&
       secondaryMembership.lastType2Resource==secondaryQuery.resource,
       "selected human retains another racer's static contact resource");
  });
  for(std::size_t racer=1;racer<racerBank.count();++racer)
   coldRacerStage50DU(racerRigs[racer]->podId,"grounded grid placement",[&](){placeOpponentAtGrid50HN(racer);});
  // Execute the original cold contacts stage synchronously against these same locals.
  interactive_cold50DU::ContactsContext{
  .track=track,
  .roster=roster,
  .allAiEnabled=allAiEnabled,
  .applicationClock=applicationClock,
  .audioEnabled=audioEnabled,
  .completedFrames=completedFrames,
  .contactBindings=contactBindings,
  .rootedCourseRace=rootedCourseRace,
  .dynamicContactEnabled=dynamicContactEnabled,
  .dynamicRacer60=dynamicRacer60,
  .dynamicRegistry=dynamicRegistry,
  .trackAnimations=trackAnimations,
  .triggerAudio=triggerAudio,
  .graphicsSequence=graphicsSequence,
  .lightDiagnostic=lightDiagnostic,
  .humanCourseEnabled=humanCourseEnabled,
  .initialCollisionZone=initialCollisionZone,
  .liveWorld=liveWorld,
  .m=m,
  .mainMembership=mainMembership,
  .aiView=aiView,
  .coldNavigationBound=coldNavigationBound,
  .membershipWork=membershipWork,
  .nativeOriginalFleet=nativeOriginalFleet,
  .nativeSelectedLights=nativeSelectedLights,
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
  .recoveryBorrowedQueries=interactive_frame50DU::BorrowedCall<undertrack_owners50ew_fixture::BorrowedQueries()>{recoveryBorrowedQueries},
  .recoveryQueryOwners=recoveryQueryOwners,
  .requireRuntimeNavigation=interactive_frame50DU::BorrowedCall<void()>{requireRuntimeNavigation},
  .s=s,
  .secondaryFilter=secondaryFilter,
  .secondaryMembership=secondaryMembership,
  .secondaryQuery=secondaryQuery,
  .selectedOriginal=selectedOriginal,
  .sessionRandom=sessionRandom,
  .sourceAnnouncer=sourceAnnouncer,
  .mapView=mapView,
  .trackCounts=trackCounts,
  .trackStorage=trackStorage,
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
  native_common_results50PD::reset();native_race_replay50PD::owner().reset();
  rr::game::nativeTestDestroyPending50PD={};
  rr::game::nativeTestLimpResetArmed50PD=false;
  // PORT EXTENSION (n64_track_hazards_runtime50nh.h): N64 'Load'/'RSet' - every
  // hazard row as authored, nothing fired, no pending explode/damage/respawn.
  rr::game::n64HazardRuntime50NH.raceReset();
  // Racer +50 (215A28) from each racer's own loaded PHY, and the 228110
  // restart restore check: a fresh race session must carry the cold-built
  // cockpit tether (anchors and length), not the one a limp rebound.
  rr::game::resetNativeExplosiveWreckSpeeds50PD();
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   require50DU(slot<racerRigs.size()&&racerRigs[slot],"racer +50 explosive wreck speed: racer PHY rig absent");
   require50DU(rr::game::loadNativeExplosiveWreckSpeed50PD(slot,racerRigs[slot]->phy),
       "racer +50 explosive wreck speed: PHY [Collisions] ExplosiveWreckSpeed rejected");
   const auto& w=rr::game::nativeExplosiveWreckSpeeds50PD[slot];
   std::cerr<<"racer_wreck_speed50 slot="<<slot<<" pod="<<racerRigs[slot]->podId<<" key="<<w.keyPresent
       <<" authored="<<std::bit_cast<float>(w.authoredBits)<<" speed50="<<std::bit_cast<float>(w.speed50Bits)
       <<" bits=0x"<<std::hex<<w.speed50Bits<<std::dec<<'\n';
  }
  // PC EXTENSION, NOT ORIGINAL (flagged): [Pod Animation] ChariotRigidMount,
  // asked of every racer's own PHY with the 158920 rule (absent = 0 = the
  // original chariot). The one place that fills the table.
  rr::game::resetNativeChariotRigidMounts50RM();
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   require50DU(slot<racerRigs.size()&&racerRigs[slot],"chariot rigid mount: racer PHY rig absent");
   require50DU(rr::game::loadNativeChariotRigidMount50RM(slot,racerRigs[slot]->phy),
       "chariot rigid mount: PHY [Pod Animation] ChariotRigidMount rejected");
   const auto& mount=rr::game::nativeChariotRigidMounts50RM[slot];
   std::cerr<<"chariot_rigid_mount slot="<<slot<<" pod="<<racerRigs[slot]->podId<<" key="<<mount.keyPresent
       <<" value="<<mount.authored<<" forced="<<mount.forced<<" active="<<mount.active
       <<" enabled="<<rr::game::chariotRigidMountSwitches50RM().enabled<<'\n';
  }
  racerBank.visit(0,[&](){
   const auto& j=s.bodies.joint;
   std::cerr<<"joint_restore slot=0 state40="<<s.bodies.main.vehicleState40<<" anchorA="<<j.localAnchor60.v[0]<<','
       <<j.localAnchor60.v[1]<<','<<j.localAnchor60.v[2]<<" anchorB="<<j.localAnchor70.v[0]<<','<<j.localAnchor70.v[1]<<','
       <<j.localAnchor70.v[2]<<" bodyB="<<j.bodyBPresent48<<" length="<<j.length4C<<" cockpit838="<<s.bodies.vehicle838<<'\n';
  });
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
  // A9: the original branches on the track's own data. With water named in the
  // SCB Environment (2745D4 water factory) every racer gets its two sprays and
  // its 14/8 particle children over the one water runtime and the one 1B4DC0
  // particle manager; without water (274570) nothing is constructed.
  if(track.hasWater()){
   require50DU(track.roles.water&&track.roles.waterResources&&track.roles.particles&&track.roles.particles->particles(),
       "track water named but its water/particle owners are absent");
   auto& water=*track.roles.water;
   auto& particles=*track.roles.particles->particles();
   // No water activation: the fog population the retired GC water session ran here
   // (populateColdFog, 160520) is done at cold inside 05.06 by the ground-effects owner.
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(!nativeWaterRacers[slot],"native water racer construction repeated");
    nativeWaterRacers[slot]=std::make_unique<native_gc_water_racer50KG::Owner>();
    // Register the actual14/8 source children before the two sprays, in the
    // same constructor/list order used by every later component visit.
    require50DU(nativeParticleChildren[slot].bindCold(nativeParticleAuthoring,nativeEffects,nativeEffectCount,
        x.bindings.visual.bottomLeft1C4!=SceneNodeIndex49P::none,particles).okay(),
        "actual stopped particle child ownership rejected");
    // The racer's own particle\podSpray.emt, read from that racer's POD archive
    // (the archive the roster loaded through 2129C0); a missing member refuses.
    require50DU(racerRigs[slot]!=nullptr,"water racer spray requires the slot's loaded pod");
    const auto podSprayEmt=coldRacerStage50DU(racerRigs[slot]->podId,"particle\\podSpray.emt",
        [&](){return racerRigs[slot]->originalMember("particle\\podSpray.emt");});
    require50DU(!podSprayEmt.empty(),"racer pod particle\\podSpray.emt is empty");
    require50DU(nativeWaterRacers[slot]->constructSprays(water,particles,
        0xf5000001u+static_cast<std::uint32_t>(slot),0xf3000001u,
        0xf4000001u+static_cast<std::uint32_t>(slot),
        nativeWaterApplication.initialStep434630,podSprayEmt,s.noiseOwner().sharedRandomState).okay(),
        "actual two-spray constructor rejected");
    require50DU(nativeWaterRacers[slot]->finishConstructor(s,x.bindings,x.diagnostic).okay(),
        "actual BE0/terrain water constructor join rejected");
    //2172C0/2172E0 target the canonical19CC50 controls, not the separate
    //50BE ID registry (whose integer IDs can overlap manager owner handles).
    resetEffectBindings.table81CPresent=true;
    for(unsigned side=0;side<2;++side){resetEffectBindings.owners81C[side]=0;
     resetEffectBindings.controls81C[side]=&nativeWaterRacers[slot]->spray(side).controls();}
   });
   std::cerr<<"track_water racers="<<racerBank.count()<<" course="<<track.code()<<'\n';
  }
  const auto retainActualSpeechOwners=[&](){
   // The consumed race of every course publishes the same Application race28C
   // and session generation the speech owners bind to.
   const auto actualRace=consumedStage.race();
   if(!audioEnabled||!originalUiStarted||!humanCourseEnabled||!actualRace)return;
   // Consumed races join when their identities match and otherwise keep the
   // existing speech path (logged).
   if(!(originalMainApplication.race28C()==actualRace.get()&&
      originalMainApplication.state&&&s.noiseOwner()==&originalMainApplication.state->sharedNoise308030)){
    std::cerr<<"native_human_speech_join_unavailable session="<<actualRace->entity_.sessionGeneration
        <<" race28C="<<(originalMainApplication.race28C()==actualRace.get())
        <<" state="<<bool(originalMainApplication.state)
        <<" noise="<<(originalMainApplication.state&&&s.noiseOwner()==&originalMainApplication.state->sharedNoise308030)<<'\n';
    return;
   }
   //This is the actual successful native speech producer, not Scope capture
   //or a future event's claim. All initialization draws already happened once.
   // The human pod load of every course (1ADF80, common roster) already bound
   // the native player speech forward (22A210) and ran 2C1000 through the same
   // live audio owner; that owner refuses a second retain.
   const bool forwardBound=!allAiEnabled;
   if(forwardBound)std::cerr<<"native_human_speech forward_bound_by_human_load=1\n";
   require50DU(forwardBound||liveAudio->retainNativeHumanSpeechInitialization(originalMainApplication,racerBank,
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
   requireRuntimeNavigation();
   require50DU(!originalAiSession.managerBound&&selectedMapCount()>1&&
       mapView.runtimeValid()&&mapView.metadata(),
       "source manager create/reset requires the actual cold MAP owner");
   // B-C06: the counted race of every course uses the B0 (1AA548) / B4
   // (1A7710) native same-state join. A rejected/partial canonical prefix never
   // falls through to legacy reset.
   if(consumedStage.race()){
    // Native roster backing already exists. Give its B0 role one counted
    // owner before the B4 association, except the actual subtype3 record
    // route which already owns a distinct source manager. No second roster.
    if(!nativeRacerManager){
     std::uint32_t b0PC=0;
     require50DU(consumedStage.race()->initializeNativeRacerB0(*this,racerBank,b0PC),
         "actual counted B0/native roster cold join rejected");
    }
    std::uint32_t b4PC=0;
    require50DU(consumedStage.race()->initializeNativeTimingB4(*this,racerBank,b4PC),
        "actual counted B4/native manager cold join rejected");
   }else require50DU(initializeOriginalAiManager(originalAiSession.manager,originalAiSession.finished44,
       originalAiSession.retired40,racerBank.count(),dynamicContactEnabled?dynamicMusicProfile.mode11258:2u,
       mapView.metadata()[1].distanceToFinish60,
       selectedMapNode(0).position,selectedMapNode(1).position,originalAiSession.trackLength38)==OriginalAiFinishStatus::Complete,
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
  // Slot 0 runs inside its Bank visit on every course (one shared arena).
  racerBank.visit(0,[&](){initializeGraphics(0);});
  raceSetupFrame=false;
  if(originalAiEnabled){
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    initializeLapTimingFields50ID(lapContinuity.timing,dynamicContactEnabled?dynamicMusicProfile.mode11258:2u);lapContinuity.timingInitialized=true;
   });
   rankOriginalAiFleet();
  }
  race_session50HM::Config racePresentationConfig;
  racePresentationConfig.lapTarget=originalAiSettings1125C&31u;
  requireRuntimeNavigation();
  require50DU(race_session50HM::initialize(raceSession,mapView,laps,raceRacerCount,racePresentationConfig),
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
  // One activation point for every course, after the cold rig/grid/contact/draw
  // owners above (publication keeps only its phase-guarded no-op re-check).
  // PORT STAND-IN: the host's neutral activation tick; GB/MC used to defer it
  // until after renderer/query admission, now one point for every course.
  if(!originalUiActive)activateColdRace();
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

 // The one static world of this race's track (05.07/05.08: instances, standard
 // models and the single-pass material decoder, R3/R7). Its mesh, normals,
 // bank, spans and selections are read from the track's own files for every
 // course; no per-course world owner, decoder or texture budget remains.
 require50DU(track.loaded()&&track.roles.staticWorld&&track.roles.staticWorld->loaded()&&
     track.roles.staticWorld->mesh()&&track.roles.staticWorld->normals(),
     "race track static world (mesh/normals) unavailable");
 rr::assets::Mesh48B trackMesh=*track.roles.staticWorld->mesh();
 namespace trackTextures=track_materials50fa_fixture;
 // Course data route for every course: the stock source rows are receipts in the
 // readers (stock_course_receipts50cr.h), never a selector.
 // PORT STAND-IN: trackSource stays only because the W0 publication context
 // carries it; it is the data route (Track::course) for every course.
 const auto trackSource=track_source_fixture::Track::course;
 trackTextures::Request trackMaterialRequest{};
 trackMaterialRequest.root=trackSession->visualRoot;
 trackMaterialRequest.exportReport=trackSession->visualReport;
 trackMaterialRequest.choice={1,1,trackSession->originalStaticCandidates?rr::assets::MaterialTextureRequirement50EA::required:
     rr::assets::MaterialTextureRequirement50EA::optionalUntextured,
     trackTextures::MipMode::largestRetained,0};
 // One cold source budget, sized from the loaded archive: no texture source can
 // be larger than the course archive that holds it.
 trackMaterialRequest.coldSourceByteCapacity=static_cast<std::size_t>(track.archive.bytes());
 trackMaterialRequest.sourceArchive=trackSession->archive;
 // The one decoder is the static world's own single-pass decoder (05.08).
 trackMaterialRequest.decodeSourceTexture=nullptr;
 trackMaterialRequest.includeAllAuthoredPasses=trackSession->originalStaticCandidates;
 trackMaterialRequest.allowStaticAnimationFrame=true;
 // One texture budget for every course. The original sizes its material table (382B38) and
 // texture table from the course's own data: the shared string table grows 16 rows at a time
 // on the heap (298900, reached from the name request at 2993A8) and 29E340 loads every
 // requested texture row; neither has a count limit. The only fixed count on this chain is the
 // 4096-slot shader-pass registry (29C490, assert at 29C4E8), owned by the material table.
 // The 50EA hard ceilings are therefore the single definition here; the track bank checks every
 // texture, slot and byte against them before storing it, and the GPU upload re-checks bytes.
 // 1024 stays the per-texture dimension: the PS2 GS cannot sample a larger texture.
 const std::size_t trackTextureBytes=rr::assets::maxMaterialTextureRGBABytes50EA;
 trackMaterialRequest.textureBudgets={rr::assets::maxMaterialTextureAssets50EA,rr::assets::maxMaterialTextureSlots50EA,
     rr::assets::maxMaterialTextureSourceBytes50EA,rr::assets::maxMaterialTextureRGBABytes50EA,1024};
 trackTextures::Diagnostic trackMaterialDiagnostic{};
 //27A208/27A234 initializes each existing static-model +148 to true.
 //271A00 applies the TOB staticmodelparams rows by the actual loaded model
 //name. One reader for every course: courses differ only in the rows their
 //own TOB carries (src/game/original_static_model_params50ps.h). Replaces
 //the per-course MA/MB/SC/TB/GB/MC/RC/TC/GC owners for this flag (port
 //stand-ins, retired) and applies TA/GA/SA rows that were never applied.
 std::vector<std::uint32_t> staticShadowReceiving;
 {
  rr::game::StaticModelParamsReceipt50PS receipt;std::string error;
  require50DU(rr::game::applyStaticModelParams50PS(actualWorld,staticShadowReceiving,receipt,error),
      error.empty()?"original static-model parameters rejected":error.c_str());
  require50DU(staticShadowReceiving.size()==actualWorld.instances.size(),"static shadow receiver table extent");
  std::cerr<<"shadow_receivers course="<<track.code()<<" instances="<<receipt.instances
      <<" standard="<<receipt.standardInstances<<" receiving="<<receipt.receiving
      <<" rows="<<receipt.declaredRows<<" applied="<<receipt.rowsApplied<<" stores="<<receipt.stores
      <<" unmatched="<<receipt.rowsUnmatched<<" missing_section="<<receipt.rowsMissingSection
      <<" missing_fields="<<receipt.rowsMissingFields<<" unknown_value="<<receipt.rowsUnknownValue
      <<" name_rule_differs="<<receipt.nameRuleDiffers<<'\n';
 }
 const auto& trackNormals=*track.roles.staticWorld->normals();
 std::cerr<<track.code()<<" source normals "<<trackNormals.size()<<'\n';
 auto advanceWater=[&](){
  // A9: the water runtime exists exactly when the SCB names water (2745D4).
  if(!track.roles.water)return;
  require50DU(waterUpdateSequence<std::numeric_limits<std::uint64_t>::max(),"track water update sequence exhausted");
  // Outside every bank lease, the canonical live arena is actual followed
  // racer0. Copy the lanes before lending them to shared world owners.
  const auto node=static_cast<std::uint32_t>(x.bindings.animation.bodyScene288);
  require50DU(node<s.sceneNodeCount,"followed water anchor scene missing");
  float position[3];std::memcpy(position,s.nodes[node].localC0.v+12,12);
  const WaterPresentationView50JC view{position,0,true,true};
  std::uint32_t actualPause11C=0;
  require50DU(originalMainApplication.animationClock().readPause11C(actualPause11C),"water Application pause owner unavailable");
  require50DU(track.roles.water->anchor(view).status==native_water_runtime50JC::Status::Complete&&
      track.roles.water->advance(++waterUpdateSequence,.016f,actualPause11C).status==native_water_runtime50JC::Status::Complete,
      "track water root update rejected");
 };
 std::vector<std::uint32_t> nativeFireSpanIds;
 std::vector<std::uint8_t> nativeFireSpanDrawn,nativeFireSpanOwner;
 // The original branches on [Environment].NumberOfSkyCubes (276C4C/276C60), not
 // the course: zero cubes -> no sky cube is drawn; one or more -> the course-data
 // sky loader below. The cube (the CUB's own base cube), its tiles and the clear
 // colour come from the data (sky_course_data50gw.inl); the zone rows
 // (CubeFileForZoneN) are the one zone->cube selection the track load prepared
 // (05.11 274AE0, track.roles.sky, R6).
 const rr::assets::ResArchive48C* courseArchive=track.archive.decoded();
 require50DU(courseArchive,"race track archive is not decoded");
 long courseSkyCubes=1;
 {
  const rr::assets::ParameterBlock48F* environment=nullptr;
  for(const auto& b:actualWorld.parameters.blocks)if(b.name=="environment")environment=&b;
  if(environment)for(const auto& f:environment->fields){
   if(f.name=="numberofskycubes"){char* end=nullptr;courseSkyCubes=std::strtol(f.value.c_str(),&end,10);
    require50DU(!f.value.empty()&&end&&!*end&&courseSkyCubes>=0,"course NumberOfSkyCubes unreadable");}
  }
 }
 // Late environment storage joins the one race payload ledger.
 // PORT STAND-IN: the race payload ledger (remainingPayloadBytes/
 // reserveAdditionalPayload, formerly MC/GB only) is a host allocation guard for
 // every course; the original has no such budget.
 const auto reserveEnvironment=[&](std::size_t bytes,const char* reason){
  require50DU(bytes<=remainingPayloadBytes(),reason);
  reserveAdditionalPayload(bytes,reason);
 };
 auto skyOwner=std::make_unique<sky50gw_fixture::Owner>();
 sky50gw_fixture::Diagnostic skyDiagnostic{};
 if(courseSkyCubes>0){
  const bool loaded=skyOwner->loadFromCourseData(*courseArchive,actualWorld,skyDiagnostic);
  require50DU(loaded,skyDiagnostic.message.empty()?"course-data sky load failed":skyDiagnostic.message.c_str());
  std::cerr<<"course_sky_data track="<<track.code()<<" cubes="<<courseSkyCubes<<" tiles="<<skyOwner->spans()->size()
      <<" textures="<<skyOwner->bank()->textureCount()<<"\n";
 }else std::cerr<<"course_sky_data track="<<track.code()<<" cubes=0\n";
 require50DU(static_cast<bool>(track.roles.sky),"race track zone sky selection (274AE0) missing");
 auto& sourceSkySelection=*track.roles.sky;
 // Borrow the same application Video on each cold race. Local renderer/GPU
 // owners still close before its real host; a later session reopens that host
 // only after cleanup, without replacing the application's Video identity.

 track_animation_window50AN::Owner trackAnimationWindow; // the common track-animation window
 // The track's one UV-animation owner (05.14 29B1D0, 29A310/29A490 data rule, every course).
 require50DU(static_cast<bool>(track.roles.uvAnimation),"race track uv animation role (29B1D0) missing");
 gameplay_binder_window_candidate_fixture::Owner gameplayBinder;
 // One particle material/render owner over the one 1B4DC0 particle manager.
 gc_particle_materials50kg_fixture::Owner particleMaterials;
 OriginalGcParticleRenderOwner50KG particleRender;
 // Same process normal-material mode AND retained APPSTART template objects
 // as Main. Direct gameplay prepares them on first use from its admitted disc
 // tree. These local handles borrow immutable application-lifetime backing;
 // they never create a replacement ScreenMT or reset it on race restart.
 const bool gameplayBinderEnabled=raceEnabled&&playerUsesRosterRig&&nativeSelectedLights&&
     !checkpointInput&&(!originalUiActive||rootedCourseRace);
 rr::assets::OriginalUiMaterialTemplates50KG gameplayBinderTemplates;
 std::shared_ptr<const rr::assets::OriginalUiArchive50KG> gameplayBinderAppStart;
 if(gameplayBinderEnabled){
  // One admission for every course: the loaded track and the full field.
  require50DU(track.loaded()&&racerBank.count()==raceRacerCount,
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
 // The one 1B4DC0 particle manager of the track (05.06, R7) draws through one
 // material/render owner on every course that prepared it.
 if(track.roles.particles&&track.roles.particles->prepared()&&track.roles.particles->particles()){
  require50DU(gameplayBinderEnabled&&gameplayBinder.loaded(),
      "track particles require the actual gameplay DefaultMT/runtime owners");
  gc_particle_materials50kg_fixture::Diagnostic diagnostic;
  require50DU(particleMaterials.load(track.roles.particles->particlePsm(),track.roles.particles->particlePst(),
      gameplayBinderTemplates,originalMainApplication.materialRuntime,diagnostic),
      diagnostic.message.empty()?"original track particle material/atlas rejected":diagnostic.message.c_str());
  require50DU(particleRender.prepare(*track.roles.particles->particles())==OriginalGcParticleRenderStatus50KG::complete,
      "actual track shared-manager render allocation rejected");
 }

 // Common track-animation window: the course's own PSM/PST for every span (absent owner = none).
 // Its alpha policy needs the APPSTART DefaultMT owner even where the gameplay Binder is off.
 if(trackAnimations){
  if(!gameplayBinderEnabled){
   gameplayBinderAppStart=originalMainApplication.appStart(runtime_asset_paths50LB::resolve("work/native/project/disc"));
   rr::assets::OriginalUiMaterialDiagnostic50KG materialDiagnostic;
   require50DU(gameplayBinderTemplates.load(*gameplayBinderAppStart,materialDiagnostic),
       "track animation window: authentic DefaultMT owner rejected");
  }
  track_animation_materials50AN::Diagnostic materialDiagnostic{};
  const bool loaded=trackAnimationWindow.loadSource(trackAnimations,trackAnimations->extract(),materialDiagnostic);
  if(!loaded)std::cerr<<"track_animation_window course="<<trackAnimations->label()<<" refused: "<<materialDiagnostic.message
      <<" span="<<materialDiagnostic.span<<" member="<<materialDiagnostic.member<<" source_code="<<materialDiagnostic.source.code<<'\n';
  require50DU(loaded,materialDiagnostic.message.empty()?"track animation materials rejected":materialDiagnostic.message.c_str());
  trackAnimations->releaseExtract();
  std::cerr<<"track_animation_window course="<<trackAnimations->label()<<" spans="<<trackAnimations->spans().size()
      <<" materials="<<trackAnimationWindow.materials().materialCount()<<" textures="<<trackAnimationWindow.materials().textureCount()<<'\n';
 }
 // One application publication survives frontend/race destruction. These
 // consumers only borrow its live view; no runSession-local clock authority.
 const auto sourceApplicationClock=originalMainApplication.animationClock();
 std::uint64_t trackAnimationSteps=0,worldServiceStep=0;
 auto advanceWorldAnimations=[&](){
  // Source1AAD98 registers the complete racer group before1B4A58's animation
  // service. Current HOST participant order stays explicit; world service runs
  // once after that group, including enabled countdown world traversal.
  const auto before=::allocations;
  std::uint32_t sourceApplicationTimestamp=0;
  require50DU(originalMainApplication.sampleAnimationTimestamp(sourceApplicationTimestamp),
      "shared source application timestamp rejected");
  std::uint32_t actualPause=0;
  require50DU(sourceApplicationClock.readPause11C(actualPause),"track world pause unavailable");
  // The SCB's timed lights and flashers (one lighting owner, every light type).
  require50DU(lightOwner.serviceTimedLights(.016f,s.noiseOwner().sharedRandomState,actualPause!=0,lightDiagnostic),
      "track timed light service rejected");
  // The ONE track-animation service (101860): no course test; absent owner = nothing to do.
  // dt = the step's frame time (.016, the same object-system frame time every world child gets);
  // after the timed lights and before every emitter service. The lane route publishes the moved
  // rows' visual and collision memberships through the binding; the static route's collision
  // refresh is the setup's.
  if(trackAnimations){
   const bool advanced=trackAnimationBinding?
     trackAnimationBinding->service(.016f,actualPause!=0,sourceApplicationTimestamp,s.noiseOwner().sharedRandomState,
       sessionRandom,s.animationShared.motion,trackAnimationBindingDiagnostic):
     trackAnimations->tick(.016f,actualPause!=0,sourceApplicationTimestamp,s.noiseOwner().sharedRandomState,
       sessionRandom,s.animationShared.motion,trackAnimationDiagnostic);
   const auto& message=trackAnimationBinding?trackAnimationBindingDiagnostic.message:trackAnimationDiagnostic.message;
   require50DU(advanced,message.empty()?"track animation world service rejected":message.c_str());
  }
  // One authored animated-texture owner (29B1D0 family, R5).
  if(track.roles.atm&&track.roles.atm->loaded())
   require50DU(track.roles.atm->advance(sourceApplicationTimestamp),"track ATM time update rejected");
  // The one 2A0B30 trigger exit service over committed COL poses (one trigger
  // group, one collision world; R3/R5).
  if(track.roles.collision){
   requireRuntimeNavigation();
   require50DU(worldServiceStep!=std::numeric_limits<std::uint64_t>::max()&&
       track.roles.collision->serviceWorld(++worldServiceStep),"track trigger exit service rejected");
  }
  require50DU(::allocations==before,"successful original world animation allocated C++ storage");
  if(trackAnimations)++trackAnimationSteps;
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
 const bool humanOriginalHud=rootedCourseRace&&humanCourseEnabled&&originalCameraEnabled&&bool(nativeModelDomain);
 require50DU(!rootedCourseRace||humanOriginalHud,"direct human world requires the real canonical HUD/material domain");
 // The one particle manager (1B4DC0) of the track; its emitter registry payload
 // is accounted once into the race payload ledger and serviced once per world step.
 OriginalTrackParticleColdDiagnostic50LA particleDiagnostic;
 std::size_t particleRegistryPayloadCharged=0;
 const auto accountParticleRegistry=[&](){
  if(!track.roles.particles)return;
  require50DU(track.roles.particles->prepared()&&track.roles.particles->particles(),
      "track particle registry accounting requires the actual shared pool");
  const auto actual=track.roles.particles->particles()->emitterStoragePayloadBytes();
  require50DU(actual>=particleRegistryPayloadCharged,"track particle registry capacity regressed before retirement");
  reserveAdditionalPayload(actual-particleRegistryPayloadCharged,"track particle registry exceeds remaining payload");
  particleRegistryPayloadCharged=actual;
 };
 const auto advanceTrackParticles=[&](){
  if(!track.roles.particles)return;
  require50DU(track.roles.particles->prepared()&&track.roles.particles->particles()&&
      track.roles.particles->particles()->emitterStoragePayloadBytes()==particleRegistryPayloadCharged,
      "track particle pool service is unavailable");
  std::uint32_t actualPause11C=0;
  require50DU(sourceApplicationClock.readPause11C(actualPause11C),"track particle Application pause owner unavailable");
  // With the ground-effects owner published, its advance (run by the loop right
  // after this call, never while paused) runs the manager's 161180 advance and
  // each fog row's 160160 phase, so 161180 runs exactly once per world step.
  if(track.roles.emitters)return;
  auto& particles=*track.roles.particles->particles();
  require50DU(particles.advance(.016f,actualPause11C!=0).okay(),"track particle manager advance rejected");
  require50DU(particles.advanceFogPhases(.016f)==OriginalGcParticleStatus50KG::complete,
      "track particle fog phase advance rejected");
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
 // Camera fog fields; the camera session fills them from the one 273C30 fog
 // runtime of the track (track.roles.fog, R6) for every course.
 OriginalRaceFogFields sourceTrackFog;
 require50DU(static_cast<bool>(track.roles.fog),"race track fog runtime (273C30) missing");
 // One authored camera storage (TOB from the track, CAM from the selected pod; R2).
 reserveAdditionalPayload(sizeof(sourceCamera::AuthoredStorage),"camera descriptor exceeds remaining payload");
 auto cameraAssets=std::make_unique<sourceCamera::AuthoredStorage>();
 auto& sourceCameraAssets=*cameraAssets;
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
 std::optional<std::uint64_t> suspendedCameraFrame;
 std::shared_ptr<const rr::assets::OriginalTaIntroAuthoringCandidate> introAuthoring;
 std::shared_ptr<const rr::assets::AudioBank50HC> introPcm;
 original_ui_source_timing50LH::Owner raceRootTiming;
 original_ui_source_timing50LH::PublicationView raceRootPublication;
 std::uint64_t raceRootTimelineNs=0;
 bool raceRootTimingInitialized=false,raceRootActive=false;
 // The counted race of every course owns the intro/root controller slot48.
 auto nativeIntroRace=[&]() -> rr::game::OriginalRaceConstructor50LA* {
  return consumedStage.race().get();
 };
 auto nativeIntroController=[&]() -> const rr::game::OriginalWreckComponent50LA* {
  // Slot custody survives a failed camera prepare; do not ask a live-camera
  // readiness getter to authorize teardown of an already retained controller.
  if(const auto* race=nativeIntroRace())if(const auto* controller=race->nativeIntroControllerSlot48())return controller;
  return sourceCameraSession?sourceCameraSession->taControllerEntity174A30():nullptr;
 };
 auto finishRaceRoot=[&](){
  if(!raceRootActive)return true;
  std::string error;
  if(!nativeIntroRace()||!sourceCameraSession||!nativeIntroController()||
     !originalMainApplication.finishNativeTaRaceRoot140CFC(*nativeIntroRace(),
       *nativeIntroController(),raceRootPublication,error)||!raceRootTiming.commit())return false;
  raceRootActive=false;return true;
 };
 auto beginRaceRoot=[&](){
  // Root 140CE0 runs on the race slot48 controller of EVERY counted race
  // (authored intro or zero cuts, 174E58/174A30). A race without that
  // controller (ALL8 spectator before its controller binds) has no root.
  if(!introAuthoring&&!nativeIntroController())return true;
  if(raceRootActive||!nativeIntroRace()||!sourceCameraSession||!nativeIntroController())return false;
  original_ui_source_timing50LH::Sample sample;
  if(!raceRootTimingInitialized){
   if(!raceRootTiming.retainPublication(raceRootPublication)||!raceRootTiming.prepare(0,true,sample)||
      !raceRootTiming.commit())return false;
   raceRootTimingInitialized=true;
  }
  raceRootTimelineNs+=16000000u;
  std::string error;
  if(!raceRootTiming.prepare(raceRootTimelineNs,false,sample)||
     !originalMainApplication.beginNativeTaRaceRoot140CE0(*nativeIntroRace(),
       *nativeIntroController(),raceRootPublication,error))return false;
  raceRootActive=true;
  if(!liveAudio->groupsForProviders()->serviceTaIntro235720(raceRootPublication))return false;
  // Same accepted Root23C, once before Camera175D70. The original child list
  // places F8 before controller48. A suspended skip resumes that same Camera
  // call without beginRaceRoot, so this timer cannot replay during fade polls.
  // This native cut does not certify the absent full141ED0 renderer dispatcher.
  return originalMainApplication.updateNativeTaRaceF8BeforeCamera50LA(*nativeIntroRace(),
      *nativeIntroController(),raceRootPublication,error);
 };
 // Called ONLY from the two successful ordered native callback-return cuts,
 // never by the generic normal/early-return RAII guard.
 auto finishRaceRootWithResults=[&](){
  if(!raceRootActive)return true;
  std::string error;
  if(!nativeIntroRace()||!sourceCameraSession||!nativeIntroController()||
     !originalMainApplication.finishNativeTaRaceRoot140CFC(*nativeIntroRace(),
       *nativeIntroController(),raceRootPublication,error)){
   static bool rootReported=false;
   if(native_common_results50PD::state().replayActive&&!rootReported&&(rootReported=true))
    std::cerr<<"race_replay root_finish_rejected error="<<error<<'\n';
   return false;
  }
  rr::game::OriginalResults104Diagnostic50LA results;
  const bool captured=originalMainApplication.captureNativeResultsAfterRoot50LA(*sourceCameraSession,results);
  if(native_common_results50PD::state().replayActive&&(!captured||results.operation)){
   static const char* reported=nullptr;
   if(results.operation!=reported){reported=results.operation;
    std::cerr<<"race_replay results_capture captured="<<captured<<" pc="<<std::hex<<results.sourcePC<<std::dec
        <<" status="<<static_cast<unsigned>(results.status)<<" operation="<<(results.operation?results.operation:"")<<'\n';}
  }
  // Commit the same real prepared sample even when a bounded capture rejects;
  // it must not remain available for a replay or a later fabricated Frame.
  const bool committed=raceRootTiming.commit();raceRootActive=false;
  return captured&&committed;
 };
 bool sourceMainBaseReady=false;
 original_course_targets50KX::Catalog courseTargets;
 original_main_entry50KG::Frontend originalMain;

 bool returnOriginalMainRequested=false;
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
       <<" consumed_material_status="<<static_cast<unsigned>(consumedStage.closeMaterialDiagnostic.status)
       <<" consumed_material_pc="<<consumedStage.closeMaterialDiagnostic.sourcePC
       <<" consumed_material_field="<<(consumedStage.closeMaterialDiagnostic.field?consumedStage.closeMaterialDiagnostic.field:"")
       <<" consumed_audio_error="<<static_cast<unsigned>(consumedStage.closeAudioDiagnostic.error)
       <<" position_pc="<<sourcePositionStatsResources.diagnostic.sourcePC
       <<" position_renderer_operation="<<(sourcePositionStatsResources.diagnostic.renderer.operation?
           sourcePositionStatsResources.diagnostic.renderer.operation:"")
       <<'\n';
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
  // The track-animation binding retires its visual and collision memberships (every
  // course on the lane route) before camera and racer cleanup; backing stays live.
  if(trackAnimationBinding&&trackAnimationBinding->needsClose()&&!trackAnimationBinding->close(trackAnimationBindingDiagnostic))
   return closeRejected("track_animation_membership_retirement");
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
  // A display fade (blocking 262C70/262D80, or the async underlay) holds the
  // renderer exclusively until it is retired, and every renderer retirement
  // below (Pause, PositionStats' 244D60 drains) is refused while it does.
  // A close that arrives mid-fade (race-start fade-in, Results fade-in)
  // cancels it first; the later camera-tail cancelAndRetire calls then see
  // it already retired. Host shutdown order only: no source state changes.
  for(auto* fade:{sourceEndingFade.get(),sourceFadeIn.get(),sourceFadeOut.get()})
   if(fade&&!fade->cancelAndRetire())return closeRejected("display fade cancel before renderer retirement");
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
  if(originalCameraEnabled){
   // The zero-line counted race retires its empty Announcer120 transport here too.
   if((introAuthoring||(liveAudio->groupsForProviders()&&liveAudio->groupsForProviders()->zeroLineRaceIntroOpen()))&&
      liveAudio->groupsForProviders()&&
      !liveAudio->groupsForProviders()->retireTaIntro142590())return closeRejected("TA intro audio retirement");
   if(raceRootActive){
    std::string error;
    if(!originalMainApplication.abandonNativeTaRaceRootForTeardown50LA(*nativeIntroRace(),
       *nativeIntroController(),raceRootPublication,error))return closeRejected("TA pending root abandonment");
    raceRootActive=false;
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
   // The track's particle manager closes once with the Track's own release
   // (common_track50CR::Track::release, R3), after the renderer below.
   if(renderer.initialized()&&!renderer.clearGameplayBlurStrengths(rendererDiagnostic))return closeRejected("if(renderer.initialized()&&!renderer.clearGameplayBlurStrengths(rendererDiagnostic))");
  }
  // Every course's counted race: its Results104/F8 renderer leases pin the
  // renderer. Retire the race's root, F8 and Pause/Results here (idempotent in
  // the later closeNativeConsumedRace), while the renderer and FIFO are still live.
  if(const auto& race=originalMainApplication.sourceRace28COwner_;
     race&&native_race_replay50PD::Owner::commonRace(*race)&&race->results104_){
   if(originalMainApplication.resultsInputContinuation50LA_)return closeRejected("common race Results input still pending");
   const char* gate=nullptr;
   if(!race->detachNativeHumanResultsRoot50LA(gate))return closeRejected(gate?gate:"common race Results root detach");
   if(!race->retireF8Cold(gate))return closeRejected(gate?gate:"common race F8 renderer retirement");
   if(!race->retireNativePauseParent(gate))return closeRejected(gate?gate:"common race Results renderer retirement");
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
  // One positional-sound/material-audio owner of the track (AUB 271600, R7).
  if(!retireAudioOwners())return closeRejected("track_positional_material_audio_retirement");
  triggerAudio.reset(); // Stop the trigger voice while its real mixer/groups still live.
  opponentAudio->close(); // Release96..116 before the live mixer/shared PCM closes.
  originalAiAudio->close();
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
  if(!sourcePause.retireDrawing())return closeRejected("if(!sourcePause.retireDrawing())");
  // Renderer has retired its strong HUD owner and every pixel/GPU borrow.
  // Release canonical UI refs before later model retirement invalidates leaves.
  sourceTimeStatsRendering=false;
  sourceTimeStats.owner.reset();sourceTimeStats.pixels.reset();
  sourceDamageRendering=false;
  sourceDamage.owner.reset();sourceDamage.pixels.reset();
  sourcePositionStatsRendering=false;sourcePositionStatsHud.reset();
  sourceSpeedBoostRendering=false;sourceSpeedBoost.owner.reset();sourceSpeedBoost.pixels.reset();
  if(trackAnimationWindow.ownsResources()&&!trackAnimationWindow.close(textureDiagnostic)){
   std::cerr<<"track animation texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("track animation texture cleanup failed");
  }
  if(trackAnimationWindow.ownsResources())return closeRejected("track animation texture resources remain");
  // The one authored animated-texture owner of the track (R5).
  if(track.roles.atm&&track.roles.atm->ownsResources()&&!track.roles.atm->close(textureDiagnostic)){
   std::cerr<<"track ATM texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';
   clean=closeAccumulated("track ATM texture cleanup failed");
  }
  if(track.roles.atm&&track.roles.atm->ownsResources())return closeRejected("track ATM resources remain");
  if(particleMaterials.ownsResources()&&!particleMaterials.closeGpu(textureDiagnostic))clean=closeAccumulated("track particle materials GPU close");
  if(particleMaterials.ownsResources())return closeRejected("track particle materials resources remain");
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
  if(!native_water_surface50WS::closeGpu(textureDiagnostic)){
   std::cerr<<"water surface texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=closeAccumulated("water surface texture cleanup");
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
  // Stage S-3 (body694.md 4.3): every course withdraws each slot's roster rows
  // (racer340 borrows) before any model closes its body; a row still holding a
  // body is refused here. Legacy layout: no rows, no effect.
  for(std::size_t slot=0;slot<nativeChariotModels.size()&&slot<track_runtime_storage50IG::Storage::maximumRoster;++slot){
   trackStorage.revokeRowColdOwners(slot);
   const auto* model=nativeChariotModels[slot].get();
   if(model&&trackStorage.rowsHoldBody(model->coldMainBody340())){
    std::cerr<<"roster row still holds racer340 slot="<<slot<<'\n';
    return closeRejected("roster_row_body_retirement");
   }
  }
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
  // One cold retirement for every course (the MC/GB aggregate shape).
  // abandonAndCloseSpatial above retired actual racer Entity/particle borrows;
  // renderer, HUD and model callbacks have now closed. Reject external Session
  // ownership before the collision world fence and the Bank's serial
  // visibility/collision/trigger-membership retirement.
  if(sourceCameraSession&&sourceCameraSession.use_count()!=1)return closeRejected("camera_owner_still_borrowed");
  sourceCameraSession.reset();cameraAssets.reset();
  if(!closeColdPrefix())return closeRejected("all_peer_terminal_contact_retirement");
  if(!closeBankCollision())return closeRejected("serial_collision_retirement");
  if(!closeTriggerMemberships())return closeRejected("serial_trigger_membership_retirement");
  if(consumedStage.race()){
   // No frame, reader or model callback is invoked beyond this point. Destroy
   // actual snapshots before releasing the aggregate's runtime lifetime pin.
   if(!racerBank.retireNativeBacking())return closeRejected("if(!racerBank.retireNativeBacking())");
   // 1A6840 order: race-owned services before the World (see setup).
   if(retireEndCandidateProducer&&!retireEndCandidateProducer())
    return closeRejected("end_candidate_producer_retirement");
   // One stage close for every course: audio, the counted race and its World
   // at race+74 (1B2870 retire), then the Track's own release (roles in
   // reverse order, then the archive 1316A0; R1/R3).
   if(!consumedStage.close()){
    const auto* field=consumedStage.closeMaterialDiagnostic.field;
    std::cerr<<"consumed_close_rejected field="<<(field?field:"")<<" pc="<<std::hex
        <<consumedStage.closeMaterialDiagnostic.sourcePC<<std::dec<<'\n';
    return closeRejected("consumed race/world retirement");
   }
  }
  // The course's track bank (1316A0) is released by the Track itself inside the
  // stage close above (course_archive_release is printed there).
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
 // Late explicit closeWindow now owns the cold retirement path.
 coldPrefixGuard.armed=false;trackAnimationPrefixGuard.armed=false;
 try {
  // Root-owned callback installation stays at its original publication point.
  const auto installHumanCameraInput50DU=[&](){
   if(humanCourseEnabled)resetHumanCamera=[&,requireRuntimeNavigation](const UndertrackSelection50ET& selected,
       const UndertrackPlacement50ET& placed,std::uint64_t serial){
    requireRuntimeNavigation();
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
    executeOriginalManualReset=[&,requireRuntimeNavigation,refreshAnimatedCandidates,recoveryQueries]() noexcept {
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
        s.phase!=VehicleFramePhase49W::Ready||s.fields.vehicle58||s.fields.vehicle3C||
        // 1A1750..1A17B8 calls 2255B0 with no state test: a limping pod
        // (states 1/2) resets and stays limping. States 3/4 keep the guard:
        // the destroyed racer frame (228390) is not a reset owner here, and
        // whether the source input routine still reaches 1A1750 then is not
        // established.
        (m.vehicleState40!=0u&&m.vehicleState40!=1u&&m.vehicleState40!=2u)||
        originalManualResetSerial==std::numeric_limits<std::uint64_t>::max()||
        graphicsSequence[0]==std::numeric_limits<std::uint64_t>::max()||
        (originalHumanRecovery.stage!=native_original_human_recovery::Stage::Idle&&
         originalHumanRecovery.stage!=native_original_human_recovery::Stage::Complete))
      return failManual(0x1a17a4u,"actual human Ready/normal-state reset owner is unavailable");
     std::uint8_t* flags22C=nullptr;std::uint32_t* field24C=nullptr;
     auto& controllerD40=liveAudio->humanController();
     requireRuntimeNavigation();
     if(!originalAiAudio->humanPhysicalResetOwners(playerRacer60,s,controllerD40,flags22C,field24C)||
        flags22C!=&controllerD40.flags22C()||!field24C||!refreshAnimatedCandidates())
      return failManual(0x1a259cu,"actual human D40/D44 or world publication is unavailable");
     const auto drained=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,trackCounts.events);
     if(drained.status!=ZoneQueueStatus50CQ::Complete||drained.remaining)
      return failManual(0x124cb0u,"actual reset world registration queue is pending");
     const auto queries=recoveryQueries();const auto& relative=x.secondaryInputs.targetRelative2F0;
     const VisualVector49R target{{relative.v[0],relative.v[1],relative.v[2],relative.v[3]}};
     // A9: with water named in the SCB, the human reset reads the same-runtime
     // water334 and live348 lift height domain of the one water runtime.
     auto actualHeightDomain=selectedRuntimeHeightDomain;
     if(track.hasWater())require50DU(track.roles.water&&track.roles.water->bindResetDomain(selectedRuntimeHeightDomain,
         terrainOwner.lift1F0,actualHeightDomain),"human reset requires actual same-runtime water334 and live348 lift");
     // One recovery owner set for every course, over the one map view.
     native_original_human_recovery::Owners owners{lightOwner,racerCables[0]->visibility50(),
         controllerD40,lapContinuity.prefix,*field24C,mapView,actualHeightDomain,
         resetGlobals,resetLifecycle,target,queries,zoneLifecycle,liveWorld,membershipWork,
         *originalResetGround,originalRecoveryModels,resetEffectBindings,nativeEffects,nativeEffectCount};
     const std::uint32_t resetBeforePositionBits[3]={
         original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[0]),
         original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[1]),
         original_material_input_diagnostics50LN::bits(m.components.root.body.position.v[2])};
     ++originalManualResetSerial;
     if(!native_original_human_recovery::resetController2255B0(s,x.bindings,x.parameters,navigationProgress,
         originalManualResetSerial,originalHumanRecovery,owners,*originalRecoveryWorkspace,originalManualRecoveryDiagnostic))
      return failManual(originalManualRecoveryDiagnostic.sourcePC,originalManualRecoveryDiagnostic.message);
     // Physical visibility lease has ended before camera3C borrows4C0.
     const auto camera=sourceCameraSession->resetHumanController225B0C(originalHumanRecovery.selection,
         originalHumanRecovery.placement,originalManualResetSerial);
     if(camera.status!=sourceCamera::Status::complete)
      return failManual(camera.sourcePC,"actual human225B0C camera reset is incomplete");
     // Input samples may precede a zero-step host poll. Keep capture/resize
     // redraw pending until the next real graphics publication, never redraw
     // reset models against the previous sequence or force an extra update.
     originalManualResetGraphicsDue=graphicsSequence[0]+1u;
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
         <<" graphics_sequence="<<graphicsSequence[0]
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
  auto bindResultsFeedback=[&](){sourceResultsFeedback.bindAfterPublication(*this,racerBank);};
  interactive_publication50DU::Context publicationContext50DU{
  .track=track,
  .laps=laps,
  .roster=roster,
  .raceStart=raceStart,
  .raceEnd=raceEnd,
  .raceStartOwner=this->raceStartOwner,
  .acceptedRacePresentation=acceptedRacePresentation,
  .accountParticleRegistryCall=interactive_frame50DU::BorrowedCall<void()>{accountParticleRegistry},
  .activateColdRace=activateColdRace,
  .actualWorld=actualWorld,
  .advanceFrameCall=interactive_frame50DU::BorrowedCall<bool(std::size_t)>{advanceFrame},
  .advanceFramePartCall=interactive_frame50DU::BorrowedCall<bool(std::size_t,interactive_frame50DU::Part,float,float)>{advanceFramePart},
  .advanceWaterCall=interactive_frame50DU::BorrowedCall<void()>{advanceWater},
  .advanceOriginalAiManagerCall=interactive_frame50DU::BorrowedCall<void(float,const original_all8_frame::Schedule*)>{advanceOriginalAiManager},
  .advanceTrackParticlesCall=interactive_frame50DU::BorrowedCall<void()>{advanceTrackParticles},
  .advanceSourceAudioWorldCall=interactive_frame50DU::BorrowedCall<void(std::uint64_t,bool)>{advanceSourceAudioWorld},
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
  .beginRaceRootCall=interactive_frame50DU::BorrowedCall<bool()>{beginRaceRoot},
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
  .consumedStage=consumedStage,
  .controlPresets=controlPresets,
  .controller=controller,
  .courseColdLoad=courseColdLoad,
  .courseDiagnostic=courseDiagnostic,
  .courseOutput=courseOutput,
  .rootedCourseRace=rootedCourseRace,
  .courseRaceLaunched=courseRaceLaunched,
  .courseTargets=courseTargets,
  .courseTuning=courseTuning,
  .destination=destination,
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
  .finishRaceRootCall=interactive_frame50DU::BorrowedCall<bool()>{finishRaceRoot},
  .finishRaceRootWithResultsCall=interactive_frame50DU::BorrowedCall<bool()>{finishRaceRootWithResults},
  .trackAnimations=trackAnimations,
  .trackAnimationBinding=trackAnimationBinding,
  .trackAnimationWindow=trackAnimationWindow,
  .gameplayBinder=gameplayBinder,
  .gameplayBinderEnabled=gameplayBinderEnabled,
  .gameplayBinderTemplates=gameplayBinderTemplates,
  .mapView=mapView,
  .raceSession=raceSession,
  .remainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{remainingPayloadBytes},
  .cameraAssets=cameraAssets,
  .trackAnimationSteps=trackAnimationSteps,
  .drawLights=drawLights,
  .triggerAudio=triggerAudio,
  .graphicsSequence=graphicsSequence,
  .lightDiagnostic=lightDiagnostic,
  .particleMaterials=particleMaterials,
  .particleRender=particleRender,
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
  .lapContinuity=lapContinuity,
  .lapOwner=lapOwner,
  .lastCommittedWrongWay=lastCommittedWrongWay,
  .liveAudio=liveAudio,
  .liveWorld=liveWorld,
  .loadedAudioSettings=loadedAudioSettings,
  .loadedTrack=loadedTrack,
  .m=m,
  .mainContactFrames=mainContactFrames,
  .mainCrossedPortals=mainCrossedPortals,
  .mainMembership=mainMembership,
  .materialAudio=track.roles.audio,
  .materialOwner=materialOwner,
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
  .particleDiagnostic=particleDiagnostic,
  .nativeVideo=nativeVideo,
  .navigationAIScan=navigationAIScan,
  .navigationAISequential=navigationAISequential,
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
  .requireRuntimeNavigationCall=interactive_frame50DU::BorrowedCall<void()>{requireRuntimeNavigation},
  .requireOriginalAiFleetReadyCall=interactive_frame50DU::BorrowedCall<void()>{requireOriginalAiFleetReady},
  .reserveAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveAdditionalPayload},
  .reserveEnvironmentCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveEnvironment},
  .resolveDynamicSpeechCall=interactive_frame50DU::BorrowedCall<bool(rr::game::RaceActorId50ID,rr::game::DamageSpeechInput50HG&,const rr::game::PodSpeechCounts50HG*&,rr::game::PodSpeechState50HG*&)>{resolveDynamicSpeech},
  .returnOriginalMainRequested=returnOriginalMainRequested,
  .s=s,
  .sampleControllerCall=interactive_frame50DU::BorrowedCall<void(bool)>{sampleController},
  .sampleRaceFieldCall=interactive_frame50DU::BorrowedCall<void()>{sampleRaceField},
  .secondaryContactFrames=secondaryContactFrames,
  .secondaryCrossedPortals=secondaryCrossedPortals,
  .secondaryMembership=secondaryMembership,
  .selectedAiCount50MBCall=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedAiCount50MB},
  .selectedAudioSettings=selectedAudioSettings,
  .selectedControls=selectedControls,
  .selectedMapCountCall=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount},
  .selectedMapIndexCall=interactive_frame50DU::BorrowedCall<std::uint8_t(std::uint32_t)>{selectedMapIndex},
  .selectedMapNodeCall=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode},
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
  .suspendedCameraFrame=suspendedCameraFrame,
  .introAuthoring=introAuthoring,
  .introPcm=introPcm,
  .aiView=aiView,
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
  .trackMaterialRequest=trackMaterialRequest,
  .trackMesh=trackMesh,
  .trackNormals=trackNormals,
  .trackSession=trackSession,
  .trackSource=trackSource,
  .trackTextureBytes=trackTextureBytes,
  .trackUvAnimation=*track.roles.uvAnimation,
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
  .bindResultsFeedbackCall=interactive_frame50DU::BorrowedCall<void()>{bindResultsFeedback},
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
     selectedMapIndex(navigationProgress.reset.vehicleC8C));
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
 std::cout<<"Development drive stopped: ticks="<<completedFrames
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
     // No Restart exit: Pause RESTART, the Time Trial finish and Practice restart
     // the race in place through 1AC8E0 (common_race_start50CR, R9).
     checkpointRequested?SessionExit50DU::Checkpoint:SessionExit50DU::Quit;
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
    // B-C06: one rule for every course. The counted race's World at race+74
    // (1B2870) holds this session's Track and its collision world answers queries.
    const auto& track=context.track;
    if(!context.originalAiEnabled||context.nativeRacerManager||
       context.consumedStage.race().get()!=this||context.originalMainApplication.race28C()!=this||
       application_!=context.originalMainApplication.entities()||!counted_||rolledBack_||
       !world74_||world74_->trackData()!=&track||!world74_->track||
       !track.roles.collision||!track.roles.collision->queryReady()||!profile_||
       profile_!=context.originalMainApplication.riderProfile||
       context.originalAiHumanCount>1u||context.originalAiHumanCount>bank.count()||
       context.originalAiOpponentCount!=bank.count()-context.originalAiHumanCount)return false;
    try {
        // Original1A6F4C calls2A1160,1A6F54 publishes B8, then1A6F5C
        // loads it over the race's one track navigation, BEFORE constructing native B0.
        pc=0x1a6f48;
        if(!mapB8_){
            auto next=context.originalMainApplication.constructTunnelTrackCold2A1160(
                entity_.sessionGeneration,OriginalTunnelTrackRole50LA::mapB8,
                std::shared_ptr<const common_track50CR::Track>(world74_->track));
            if(!next)return false;
            mapB8_=std::move(next); //Retain actual count if later binding fails.
        }
        pc=0x1a6f5c;
        if(!mapB8_->navigation()&&
           !context.originalMainApplication.bindTunnelTrackColdProjection2A1260(*mapB8_))return false;
        if(!mapB8_->navigation()||!context.mapView.runtimeValid()||mapB8_->application_!=application_||
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
    // B-C06: one rule for every course (World at race+74 holds this Track).
    const auto& track=context.track;
    if(!context.originalAiEnabled||context.consumedStage.race().get()!=this||
       context.originalMainApplication.race28C()!=this||application_!=context.originalMainApplication.entities()||
       !counted_||rolledBack_||!world74_||world74_->trackData()!=&track||
       !track.roles.collision||!track.roles.collision->queryReady())return false;
    // The map view over m<code>.spl (2A1260) with its bound AI metadata.
    if(!context.mapView.runtimeValid()||!context.mapView.metadata()||context.mapView.count()<2u||!profile_||
       profile_!=context.originalMainApplication.riderProfile)return false;
    try {
        if(!timingB4_){
            std::unique_ptr<OriginalRaceTimingManager50LA> next(new OriginalRaceTimingManager50LA(
                application_,entity_.sessionGeneration,context.originalAiSession,bank));
            if(!next->constructed()){pc=0x1af7f0;return false;}
            timingB4_=std::move(next); //Actual1A7730 publication BEFORE subtype/load.
        }
        return timingB4_->continueNativeAssociation(*this,context.originalAiSession,bank,
            context.mapView,pc);
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
       race.endHud_.lock().get()!=this||!race.anyWorld())return;
    try {
        // Every course's race reaches 1AB520 through its own camera link.
        const original_camera_session50KX::Session* camera=race.cameraNativeAdapter34_;
        if(!camera||camera!=camera_||&camera->actual_.bank!=bank_||
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

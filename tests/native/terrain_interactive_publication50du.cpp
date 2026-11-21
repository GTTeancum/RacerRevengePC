// Original publication scope and its local owners; no root-only hooks/readers.
#include "../../src/assets/dlc_files50dl.h"
#include "native_last_failure50pd.h"
#include "course_world_texture50st.h"
#include "native_course_archive_release50pd.h"
#include "native_wreck_debris_gpu50pd.h"
#include "native_water_surface50ws.h"
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "native_wreck_debris_sim50pd.h"
#include "native_wreck_effects50pd.h"
#include "native_common_results50pd.h"
#include "native_common_race50pd.h"
#include "terrain_interactive_publication50du.h"
#include "native_display_capacity50du.h"
#include "original_race_hud_scope50la.h"
#include "terrain_interactive_loop50du.h"
#include "player_loading_session50du.h"
#include "cold_load_diagnostics50du.h"
#include "../../platform/linux/native/material_texture_functions50ee.h"
#include "../../platform/linux/native/original_ta_intro_preload50la.h"
#include "pod_cables50fd_fixture.h"
#include "gc_particle_materials50kg_fixture.h"
#include "track_animation_binding50an.h"
#include "n64_track_animation_port50nh.h"   // N64 hazard rows: race-reset restore (inert on stock)
#include "../../src/game/camera_shake_relay50cs.h" // CameraShake trigger action -> this race's camera session
#include "original_camera_assets50kx.h"
#include "original_ta_camera_issuer_candidate.h"
// W0 common rewrite: the one stage and the one track's role owners (complete types).
#include "original_consumed_stage50du.h"
#include "single_pass_world50lm.h"        // R3 NEW: single_pass_world50LM::Owner (roles.staticWorld)
#include "track_collision_world50cr.h"    // R3 NEW: track_collision_world50CR::Owner (roles.collision)
#include "track_atm50cr.h"                // R5 NEW: track_atm50CR::Owner (roles.atm)
#include "native_water_runtime50jc_fixture.h"
#include "../../src/game/native_trigger_group50mc.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../src/game/lap_timing50id.h"
#include "gc_gate_audio50in_fixture.h"
#include "original_native_ta_root_audio50la.h"
#include "original_position_stats_prestage50la.h"
#include "source_grid50ig_fixture.h"           // 2A2880 grid geometry over the one map view (race start hook)
#include "native_racer_grid50hn_fixture.h"     // groundGridPose
#include "original_subtype3_record_owner50la.h" // 1ACB18 record actor (race start hook)
#include "../../src/game/lap_progress50gf.h"
#include "track_environment_uv50cr.h"           // track.environmentUv producer (area E)
#include "track_particle_emitters50cr.h"       // roles.emitters placements (A9)
#include "../../src/game/original_track_fog50cr.h"
#include <fstream>
#include <iomanip>

namespace interactive_publication50DU {
namespace platform=rr::platform;
namespace sourceCamera=original_camera_session50KX;
namespace trackTextures=track_materials50fa_fixture;
namespace gcLighting=rr::game::original_gc_gameplay_lighting_candidate;
using namespace rr::game;
using interactive_frame50DU::BorrowedCall;
using interactive_loop50DU::RacerContactTotals50HV;
using interactive_loop50DU::RacePresentation50HZ;
using interactive_loop50DU::GameplayDisplayWallTotals50DU;
using J=nlohmann::json;
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
constexpr std::uint32_t raceOpponentCapacity=raceRacerCapacity-1;
struct BaselineDisplayWall50DU {
    using Boundary=track_fps_baseline50LA::Boundary;
    using Transition=track_fps_baseline50LA::Transition;
    using Observation=track_fps_baseline50LA::Observation;
    bool enabled=false,active=false,sealed=false,identityFailure=false,overflow=false;
    Boundary begin{},end{},last{};
    std::uint64_t samples=0;
    GameplayDisplayWallTotals50DU::Total totals[3]{};
    static bool same(const Boundary& a,const Boundary& b) noexcept {
        return a.ns==b.ns&&a.swaps==b.swaps&&a.tick==b.tick;
    }
    void consume(const Transition& t,const rr::platform::VehicleWindowDiagnostic50DS* d=nullptr) noexcept {
        if(!enabled||sealed)return;
        if(t.discarded){
            active=false;identityFailure=false;overflow=false;samples=0;
            begin=end=last={};for(auto& total:totals)total={};
        }
        if(t.observation==Observation::measurementStart){
            active=true;identityFailure=false;overflow=false;samples=0;
            begin=t.begin;last=t.current;end={};for(auto& total:totals)total={};
            if(!same(begin,last))identityFailure=true;
            return; // The boundary swap is excluded, including its timings.
        }
        if(t.observation!=Observation::measurementSample&&t.observation!=Observation::completed)return;
        if(!active||!same(begin,t.begin)||t.current.ns<last.ns||t.current.tick<last.tick||
            t.current.swaps<=last.swaps||t.current.swaps-last.swaps!=1)identityFailure=true;
        if(samples==(std::numeric_limits<std::uint64_t>::max)())overflow=true;else ++samples;
        if(d){
            if(!d->submittedWorkCompleted||!d->gameplayWindowSwapSubmitted||
                !d->gameplayDisplayHistoryAccepted)identityFailure=true;
            totals[0].record(d->gameplayHistoryStageWall);
            totals[1].record(d->gameplayFinishWall);
            totals[2].record(d->gameplaySwapWall);
        }else identityFailure=true;
        last=t.current;
        if(t.observation==Observation::completed){end=t.current;sealed=true;active=false;}
    }
    J receipt(const track_fps_baseline50LA::Receipt& fps,std::uint64_t session) const {
        const bool identities=sealed&&!identityFailure&&same(begin,{fps.beginNs,fps.beginSwaps,fps.beginTick})&&
            same(end,{fps.endNs,fps.endSwaps,fps.endTick});
        const bool countAgreement=identities&&!overflow&&fps.endSwaps>=fps.beginSwaps&&
            samples==fps.endSwaps-fps.beginSwaps;
        bool timingComplete=countAgreement,timingOverflow=overflow;
        J components=J::object();
        const char* names[]{"history_stage","finish_wait","host_swap"};
        for(unsigned i=0;i<3;++i){const auto& total=totals[i];
            timingOverflow=timingOverflow||total.overflow;
            timingComplete=timingComplete&&!total.overflow&&total.attempted==samples&&total.valid==samples;
            components[names[i]]={{"attempted",total.attempted},{"valid",total.valid},
                {"wall_ns",total.wallNs},{"max_wall_ns",total.maxWallNs},{"overflow",total.overflow}};
        }
        return {{"schema","baseline-display-wall-v1"},{"session",session},{"parent","completion_wait"},
            {"nested",true},{"additive_to_parent",false},{"parent_wall_time_measured",false},
            {"scope","gameplay"},{"baseline_aligned",countAgreement},
            {"boundary_identity_match",identities},{"count_agreement",countAgreement},
            {"timing_complete",timingComplete},{"overflow",timingOverflow},
            {"interval_begin_ns",begin.ns},{"interval_end_ns",end.ns},
            {"begin_window_swaps",begin.swaps},{"end_window_swaps",end.swaps},
            {"begin_native_tick",begin.tick},{"end_native_tick",end.tick},
            {"accepted_sample_swaps",samples},{"discarded_intervals",fps.discardedIntervals},
            {"components",std::move(components)}};
    }
};
} // namespace

void Context::run() {
 auto& track=this->track;
 auto& laps=this->laps;
 auto& roster=this->roster;
 auto& raceStart=this->raceStart;
 auto& raceEnd=this->raceEnd;
 auto& acceptedRacePresentation=this->acceptedRacePresentation;
 const auto& accountParticleRegistry=this->accountParticleRegistryCall;
 auto& activateColdRace=this->activateColdRace;
 auto& actualWorld=this->actualWorld;
 const auto& advanceFrame=this->advanceFrameCall;
 const auto& advanceFramePart=this->advanceFramePartCall;
 const auto& advanceWater=this->advanceWaterCall;
 const auto& advanceOriginalAiManager=this->advanceOriginalAiManagerCall;
 const auto& advanceTrackParticles=this->advanceTrackParticlesCall;
 const auto& advanceSourceAudioWorld=this->advanceSourceAudioWorldCall;
 const auto& advanceWorldAnimations=this->advanceWorldAnimationsCall;
 auto& allAiControllerSamples=this->allAiControllerSamples;
 auto& allAiEnabled=this->allAiEnabled;
 auto& allAiHostDriverCalls=this->allAiHostDriverCalls;
 auto& allAiKeyboardPackets=this->allAiKeyboardPackets;
 auto& announcerRequestsUnjoined=this->announcerRequestsUnjoined;
 auto& applicationClock=this->applicationClock;
 auto& applicationEvents=this->applicationEvents;
 const auto& applyCourseInput=this->applyCourseInputCall;
 const auto& applyDynamicSpeech=this->applyDynamicSpeechCall;
 auto& audioDiagnostic=this->audioDiagnostic;
 auto& audioEnabled=this->audioEnabled;
 auto& audioListener=this->audioListener;
 auto& audioListenerReady=this->audioListenerReady;
 auto& audioOutputRequested=this->audioOutputRequested;
 auto& authored=this->authored;
 auto& autoDriveEnabled=this->autoDriveEnabled;
 auto& baseTail=this->baseTail;
 const auto& beginNativeLightDraw=this->beginNativeLightDrawCall;
 const auto& beginRaceRoot=this->beginRaceRootCall;
 auto& boostButtonCode=this->boostButtonCode;
 auto& boostState=this->boostState;
 auto& cableDiagnostic=this->cableDiagnostic;
 auto& cableGpuTextures=this->cableGpuTextures;
 auto& cableMesh=this->cableMesh;
 auto& cableOwner=this->cableOwner;
 auto& cableWorkspace=this->cableWorkspace;
 auto& cameraStartup350=this->cameraStartup350;
 const auto& captureDynamicBeforeFrame=this->captureDynamicBeforeFrameCall;
 const auto& captureDynamicBeforeScheduledPhysics=this->captureDynamicBeforeScheduledPhysicsCall;
 auto& captureFirstWrongWay=this->captureFirstWrongWay;
 auto& capturePath=this->capturePath;
 auto& captureRun=this->captureRun;
 auto& capturedFirstWrongWay=this->capturedFirstWrongWay;
 auto& checkpointOutput=this->checkpointOutput;
 auto& checkpointRequested=this->checkpointRequested;
 auto& coldFlame=this->coldFlame;
 auto& coldLoadDiagnostics=this->coldLoadDiagnostics;
 auto& collisionNormalMode=this->collisionNormalMode;
 auto& completedFrames=this->completedFrames;
 auto& completionDiagnostic=this->completionDiagnostic;
 const auto& composePacket=this->composePacketCall;
 auto& consumedStage=this->consumedStage;
 auto& controlPresets=this->controlPresets;
 auto& controller=this->controller;
 auto& courseColdLoad=this->courseColdLoad;
 auto& courseDiagnostic=this->courseDiagnostic;
 auto& courseOutput=this->courseOutput;
 auto& rootedCourseRace=this->rootedCourseRace;
 auto& courseRaceLaunched=this->courseRaceLaunched;
 auto& courseTargets=this->courseTargets;
 auto& courseTuning=this->courseTuning;
 auto& destination=this->destination;
 auto& discardedHostNs=this->discardedHostNs;
 auto& displayWallTotals=this->displayWallTotals;
 auto& drawWorkspace=this->drawWorkspace;
 auto& dynamicActorResolver=this->dynamicActorResolver;
 const auto& dynamicBoundary=this->dynamicBoundaryCall;
 auto& dynamicConsumerFlags11050=this->dynamicConsumerFlags11050;
 auto& dynamicContactEnabled=this->dynamicContactEnabled;
 auto& dynamicDamageCursor=this->dynamicDamageCursor;
 auto& dynamicEventBindings=this->dynamicEventBindings;
 auto& dynamicEventCursor=this->dynamicEventCursor;
 auto& dynamicEvents=this->dynamicEvents;
 auto& dynamicLapCursor=this->dynamicLapCursor;
 auto& dynamicMusicProfile=this->dynamicMusicProfile;
 auto& dynamicPodEffect=this->dynamicPodEffect;
 auto& dynamicProfile=this->dynamicProfile;
 auto& dynamicRacer60=this->dynamicRacer60;
 auto& dynamicRegistry=this->dynamicRegistry;
 auto& engineDamage=this->engineDamage;
 auto& engineDamageCompletions=this->engineDamageCompletions;
 auto& engineDamageDips=this->engineDamageDips;
 auto& engineDamageDraws=this->engineDamageDraws;
 auto& engineDamageReturns=this->engineDamageReturns;
 auto& engineDamageUpdates=this->engineDamageUpdates;
 auto& feedbackHost=this->feedbackHost;
 auto& finishDriverAppliedThisFrame=this->finishDriverAppliedThisFrame;
 auto& finishDriverTicks=this->finishDriverTicks;
 auto& finishManualInputTicks=this->finishManualInputTicks;
 const auto& finishRaceRoot=this->finishRaceRootCall;
 const auto& finishRaceRootWithResults=this->finishRaceRootWithResultsCall;
 auto& trackAnimations=this->trackAnimations;
 auto& trackAnimationBinding=this->trackAnimationBinding;
 auto& trackAnimationWindow=this->trackAnimationWindow;
 auto& gameplayBinder=this->gameplayBinder;
 auto& gameplayBinderEnabled=this->gameplayBinderEnabled;
 auto& gameplayBinderTemplates=this->gameplayBinderTemplates;
 auto& mapView=this->mapView;
 auto& raceSession=this->raceSession;
 const auto& remainingPayloadBytes=this->remainingPayloadBytesCall;
 auto& cameraAssets=this->cameraAssets;
 auto& trackAnimationSteps=this->trackAnimationSteps;
 auto& drawLights=this->drawLights;
 auto& triggerAudio=this->triggerAudio;
 auto& graphicsSequence=this->graphicsSequence;
 auto& lightDiagnostic=this->lightDiagnostic;
 auto& particleMaterials=this->particleMaterials;
 auto& particleRender=this->particleRender;
 auto& gpuTextures=this->gpuTextures;
 auto& ground=this->ground;
 auto& groundStack=this->groundStack;
 auto& headlessLive=this->headlessLive;
 auto& host=this->host;
 auto& hostDiagnostic=this->hostDiagnostic;
 auto& humanControlInput=this->humanControlInput;
 auto& humanCourseEnabled=this->humanCourseEnabled;
 auto& humanOriginalHud=this->humanOriginalHud;
 auto& humanResultsActive=this->humanResultsActive;
 auto& humanResultsCountdown=this->humanResultsCountdown;
 auto& humanResultsReady=this->humanResultsReady;
 const auto& installHumanCameraInput50DU=this->installHumanCameraInput50DUCall;
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
 auto& mainMembership=this->mainMembership;
 auto& materialOwner=this->materialOwner;
 auto& mode1=this->mode1;
 auto& mode1Effects=this->mode1Effects;
 auto& mode1Enabled=this->mode1Enabled;
 auto& monotonicNow50DU=*this->monotonicNow50DU;
 auto& nativeBottomLeftModels=this->nativeBottomLeftModels;
 auto& nativeBottomRightModels=this->nativeBottomRightModels;
 auto& nativeCalls=this->nativeCalls;
 auto& nativeChariotModels=this->nativeChariotModels;
 auto& nativeFireSerial=this->nativeFireSerial;
 auto& nativeFireSpanDrawn=this->nativeFireSpanDrawn;
 auto& nativeFireSpanIds=this->nativeFireSpanIds;
 auto& nativeFireSpanOwner=this->nativeFireSpanOwner;
 const auto& nativeIntroRace=this->nativeIntroRaceCall;
 auto& nativeLeftModels=this->nativeLeftModels;
 const auto& nativeLightFailure=this->nativeLightFailureCall;
 auto& nativeLightingPolicy=this->nativeLightingPolicy;
 auto& nativeModelDomain=this->nativeModelDomain;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
 auto& nativeRecordActor=this->nativeRecordActor;
 auto& nativeRightModels=this->nativeRightModels;
 auto& nativeSelectedLights=this->nativeSelectedLights;
 auto& particleDiagnostic=this->particleDiagnostic;
 auto& nativeVideo=this->nativeVideo;
 auto& navigationAIScan=this->navigationAIScan;
 auto& navigationAISequential=this->navigationAISequential;
 auto& navigationMapScan=this->navigationMapScan;
 auto& navigationMapSequential=this->navigationMapSequential;
 auto& navigationProgress=this->navigationProgress;
 const auto& observeOriginalAiFleet=this->observeOriginalAiFleetCall;
 auto& observedNativeFinishFlags=this->observedNativeFinishFlags;
 auto& observedNativeTicks=this->observedNativeTicks;
 const auto& openNativeVideo=this->openNativeVideoCall;
 auto& opponentAudio=this->opponentAudio;
 auto& opponentCableGpuTextures=this->opponentCableGpuTextures;
 auto& opponentDraw=this->opponentDraw;
 auto& opponentFlamePrepared=this->opponentFlamePrepared;
 auto& opponentFlames=this->opponentFlames;
 auto& opponentFrame=this->opponentFrame;
 auto& opponentGpuTextures=this->opponentGpuTextures;
 auto& opponentSpecularGpuTextures50LA=this->opponentSpecularGpuTextures50LA;
 auto& opponentUvs=this->opponentUvs;
 auto& originalAi=this->originalAi;
 auto& originalAiAudio=this->originalAiAudio;
 auto& originalAiAudioEnabled=this->originalAiAudioEnabled;
 auto& originalAiEnabled=this->originalAiEnabled;
 auto& originalAiHumanCount=this->originalAiHumanCount;
 auto& originalAiOpponentCount=this->originalAiOpponentCount;
 const auto& originalAiOwnsSlot=this->originalAiOwnsSlotCall;
 auto& originalAiReads=this->originalAiReads;
 auto& originalAiSession=this->originalAiSession;
 auto& originalAiSettings1125C=this->originalAiSettings1125C;
 auto& originalCameraEnabled=this->originalCameraEnabled;
 auto& originalMain=this->originalMain;
 auto& originalMainApplication=this->originalMainApplication;
 auto& originalManualResetGraphicsDue=this->originalManualResetGraphicsDue;
 auto& originalUiActive=this->originalUiActive;
 auto& originalUiStarted=this->originalUiStarted;
 auto& pendingSpeech=this->pendingSpeech;
 auto& phaseTiming=this->phaseTiming;
 auto& physicalControllerReceipt=this->physicalControllerReceipt;
 auto& playerCableBindings=this->playerCableBindings;
 auto& playerDrawAssets=this->playerDrawAssets;
 auto& playerEnvironment=this->playerEnvironment;
 auto& playerEvents=this->playerEvents;
 auto& playerFlameUvs=this->playerFlameUvs;
 auto& playerPodId=this->playerPodId;
 auto& playerRacer60=this->playerRacer60;
 auto& playerUsesRosterRig=this->playerUsesRosterRig;
 auto& poseHistory=this->poseHistory;
 auto& powerslideButtonCode=this->powerslideButtonCode;
 const auto& projectDynamicActorsForSlot=this->projectDynamicActorsForSlotCall;
 const auto& publishOriginalAiStandings=this->publishOriginalAiStandingsCall;
 auto& race=this->race;
 auto& raceAudio=this->raceAudio;
 auto& raceBorrow=this->raceBorrow;
 auto& raceEnabled=this->raceEnabled;
 auto& raceMusic=this->raceMusic;
 auto& raceOpponentCount=this->raceOpponentCount;
 auto& raceRacerCount=this->raceRacerCount;
 auto& racerBank=this->racerBank;
 auto& racerCables=this->racerCables;
 auto& racerCollisions=this->racerCollisions;
 auto& racerContactIds=this->racerContactIds;
 auto& racerContactMaterials=this->racerContactMaterials;
 auto& racerContactOwner=this->racerContactOwner;
 auto& racerContactTotals=this->racerContactTotals;
 auto& racerContactsEnabled=this->racerContactsEnabled;
 auto& racerFrameReads=this->racerFrameReads;
 auto& racerHudNames=this->racerHudNames;
 auto& racerNames=this->racerNames;
 auto& racerRigs=this->racerRigs;
 auto& racerVisuals=this->racerVisuals;
 auto& recoveryPaused=this->recoveryPaused;
 const auto& refreshAnimatedCandidates=this->refreshAnimatedCandidatesCall;
 const auto& restartGridPlacementCall=this->restartGridPlacementCall;
 const auto& restartGridPreflightCall=this->restartGridPreflightCall;
 const auto& restartSetupTickCall=this->restartSetupTickCall;
 auto& renderDiagnostics=this->renderDiagnostics;
 auto& renderedCameraPublication=this->renderedCameraPublication;
 auto& renderer=this->renderer;
 auto& rendererDiagnostic=this->rendererDiagnostic;
 auto& repairActiveTicks=this->repairActiveTicks;
 auto& repairButtonCode=this->repairButtonCode;
 auto& reportHost50DU=*this->reportHost50DU;
 const auto& reportOriginalAiFleet=this->reportOriginalAiFleetCall;
 const auto& reportRacerContacts=this->reportRacerContactsCall;
 auto& reportRenderer50DU=*this->reportRenderer50DU;
 auto& requestIntroMusicFade=this->requestIntroMusicFade;
 auto& requestStartMusic=this->requestStartMusic;
 auto& requestedWindowSamples=this->requestedWindowSamples;
 const auto& requireRuntimeNavigation=this->requireRuntimeNavigationCall;
 const auto& requireOriginalAiFleetReady=this->requireOriginalAiFleetReadyCall;
 const auto& reserveAdditionalPayload=this->reserveAdditionalPayloadCall;
 const auto& reserveEnvironment=this->reserveEnvironmentCall;
 const auto& resolveDynamicSpeech=this->resolveDynamicSpeechCall;
 auto& returnOriginalMainRequested=this->returnOriginalMainRequested;
 auto& s=this->s;
 const auto& sampleController=this->sampleControllerCall;
 const auto& sampleRaceField=this->sampleRaceFieldCall;
 auto& secondaryContactFrames=this->secondaryContactFrames;
 auto& secondaryCrossedPortals=this->secondaryCrossedPortals;
 auto& secondaryMembership=this->secondaryMembership;
 const auto& selectedAiCount50MB=this->selectedAiCount50MBCall;
 auto& selectedAudioSettings=this->selectedAudioSettings;
 auto& selectedControls=this->selectedControls;
 const auto& selectedMapCount=this->selectedMapCountCall;
 const auto& selectedMapIndex=this->selectedMapIndexCall;
 const auto& selectedMapNode=this->selectedMapNodeCall;
 auto& selectedRacer60=this->selectedRacer60;
 auto& selectedRuntimeHeightDomain=this->selectedRuntimeHeightDomain;
 auto& selectedTrack=this->selectedTrack;
 auto& sessionIndex=this->sessionIndex;
 auto& sessionRandom=this->sessionRandom;
 auto& skyGpuTextures=this->skyGpuTextures;
 auto& skyOwner=this->skyOwner;
 auto& sourceAnnouncer=this->sourceAnnouncer;
 auto& sourceApplicationClock=this->sourceApplicationClock;
 auto& sourceAttachedGlobals=this->sourceAttachedGlobals;
 auto& sourceBlur=this->sourceBlur;
 auto& sourceCameraAssets=this->sourceCameraAssets;
 auto& sourceCameraCables=this->sourceCameraCables;
 auto& sourceCameraCycleAudio=this->sourceCameraCycleAudio;
 auto& sourceCameraDisplay=this->sourceCameraDisplay;
 auto& sourceCameraInput=this->sourceCameraInput;
 auto& sourceCameraSession=this->sourceCameraSession;
 auto& sourceCameraTick=this->sourceCameraTick;
 auto& sourceConsumedIntroActivation=this->sourceConsumedIntroActivation;
 auto& sourceConsumedPauseActivation=this->sourceConsumedPauseActivation;
 auto& sourceDamage=this->sourceDamage;
 auto& sourceDamageDisplayReported=this->sourceDamageDisplayReported;
 auto& sourceDamageObservation=this->sourceDamageObservation;
 auto& sourceDamageRendering=this->sourceDamageRendering;
 auto& sourceDisplayLifetime=this->sourceDisplayLifetime;
 auto& sourceEndingFade=this->sourceEndingFade;
 auto& sourceFadeDuration98=this->sourceFadeDuration98;
 auto& sourceFadeIn=this->sourceFadeIn;
 auto& sourceFadeManagerA8=this->sourceFadeManagerA8;
 auto& sourceFadeOut=this->sourceFadeOut;
 auto& sourceFogRenderPhase=this->sourceFogRenderPhase;
 auto& sourceFollowQueries=this->sourceFollowQueries;
 auto& sourceFrameSchedule=this->sourceFrameSchedule;
 auto& sourceMainBaseReady=this->sourceMainBaseReady;
 auto& sourceModelRenderGlobals=this->sourceModelRenderGlobals;
 auto& sourcePause=this->sourcePause;
 auto& sourcePauseObservation=this->sourcePauseObservation;
 auto& sourcePauseParentAttached=this->sourcePauseParentAttached;
 auto& sourcePositionStatsDisplayReported=this->sourcePositionStatsDisplayReported;
 auto& sourcePositionStatsHud=this->sourcePositionStatsHud;
 auto& sourcePositionStatsPrestage=this->sourcePositionStatsPrestage;
 auto& sourcePositionStatsPrestageDiagnostic=this->sourcePositionStatsPrestageDiagnostic;
 auto& sourcePositionStatsRendering=this->sourcePositionStatsRendering;
 auto& sourcePositionStatsResources=this->sourcePositionStatsResources;
 auto& sourceRaceHud=this->sourceRaceHud;
 auto& sourceRaceHudDisplayReported=this->sourceRaceHudDisplayReported;
 auto& sourceRaceHudRejectionReported=this->sourceRaceHudRejectionReported;
 auto& sourceRaceHudRendering=this->sourceRaceHudRendering;
 auto& sourceRendererDispatch=this->sourceRendererDispatch;
 auto& sourceSkySelection=this->sourceSkySelection;
 auto& sourceSpeedBoost=this->sourceSpeedBoost;
 auto& sourceSpeedBoostAdapter=this->sourceSpeedBoostAdapter;
 auto& sourceSpeedBoostDisplayReported=this->sourceSpeedBoostDisplayReported;
 auto& sourceSpeedBoostRendering=this->sourceSpeedBoostRendering;
 auto& sourceTimeStats=this->sourceTimeStats;
 auto& sourceTimeStatsAdapter=this->sourceTimeStatsAdapter;
 auto& sourceTimeStatsDisplayReported=this->sourceTimeStatsDisplayReported;
 auto& sourceTimeStatsRendering=this->sourceTimeStatsRendering;
 // W0 area B item 4: the camera session (DisplayInputs::trackFog3EC), the loop and
 // the renderer borrow the track's one fog role (World3EC, 05.05), not a session copy.
 require50DU(track.roles.fog&&track.roles.fog->cameraFields3EC(),"track fog role (05.05 273C30) not prepared");
 auto& sourceTrackFog=*track.roles.fog->cameraFields3EC();
 auto& sourceVideo=this->sourceVideo;
 auto& specularGpuTextures50LA=this->specularGpuTextures50LA;
 auto& speechCounts=this->speechCounts;
 auto& speechCues=this->speechCues;
 auto& speechEvents=this->speechEvents;
 const auto& speechInput=this->speechInputCall;
 auto& speechState=this->speechState;
 auto& startAtMenu=this->startAtMenu;
 auto& startMusicContext=this->startMusicContext;
 auto& staticShadowReceiving=this->staticShadowReceiving;
 const auto& stopFeedbackTransport=this->stopFeedbackTransportCall;
 auto& submittedDrawFrames=this->submittedDrawFrames;
 auto& suspendedCameraFrame=this->suspendedCameraFrame;
 auto& introAuthoring=this->introAuthoring;
 auto& introPcm=this->introPcm;
 auto& aiView=this->aiView;
 auto& terrainHitFrames=this->terrainHitFrames;
 auto& terrainOwnerCount=this->terrainOwnerCount;
 auto& terrainQueries=this->terrainQueries;
 auto& terrainSupportedFrames=this->terrainSupportedFrames;
 auto& textureDiagnostic=this->textureDiagnostic;
 auto& tournamentLaunchSelection=this->tournamentLaunchSelection;
 auto& tournamentLaunchStamp=this->tournamentLaunchStamp;
 auto& tournamentResultArmed=this->tournamentResultArmed;
 auto& tournamentResultCommitted=this->tournamentResultCommitted;
 auto& trackAdmission=this->trackAdmission;
 auto& trackGpuTextures=this->trackGpuTextures;
 auto& trackMaterialDiagnostic=this->trackMaterialDiagnostic;
 auto& trackMaterialRequest=this->trackMaterialRequest;
 auto& trackMesh=this->trackMesh;
 auto& trackNormals=this->trackNormals;
 auto& trackSession=this->trackSession;
 auto& trackSource=this->trackSource;
 auto& trackTextureBytes=this->trackTextureBytes;
 auto& trackUvAnimation=this->trackUvAnimation;
 const auto& updateOpponentAudio=this->updateOpponentAudioCall;
 const auto& updateOriginalAiPacking=this->updateOriginalAiPackingCall;
 auto& usePhysicalGameController=this->usePhysicalGameController;
 auto& windowSwaps=this->windowSwaps;
 auto& wrongWayActiveTicks=this->wrongWayActiveTicks;
 auto& wrongWayReceipt=this->wrongWayReceipt;
 auto& wrongWayUpdates=this->wrongWayUpdates;
 auto& x=this->x;
 auto& zoneCount=this->zoneCount;
 auto& zoneLifecycle=this->zoneLifecycle;
 const auto& bindResultsFeedback=this->bindResultsFeedbackCall;
 // One race session row type for every course (common_lap50CR / race_session50HM::CourseSession, R4).
 const auto withRace=[&](auto&& operation)->decltype(auto){
  requireRuntimeNavigation();
  return operation(raceSession);
 };
  rr::platform::loading_progress50DU::pulse("Preparing graphics");
  PlayerLoadingSession50DU::checkStop();
 coldLoadDiagnostics.begin("video_and_renderer_publication");
  openNativeVideo();
  feedbackHost=&host;
  // Publish the track bank only after this application's real Video host is
  // current. Keep every dependent cold bank/span consumer within the same
  // cleanup-protected lifetime, before GPU upload and renderer publication.
  // Original Main's TA/SA sessions enter the sealed original640x448 domain.
  // Direct640x480 retains the existing host route without a canonical context.
  if(originalMainApplication.enabled&&!originalUiActive)
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
  // One canonical material domain for every course (14D370 population over the
  // Application's 382B38 table); no course term (B-C07).
  if(originalMainApplication.enabled){
   originalMainApplication.appStart(originalMainApplication.assetRoot);
   trackMaterialRequest.canonicalDomain=originalMainApplication.standardMaterialDomain();
   trackMaterialRequest.canonicalContext=nativeVideo->standardMaterialConstructionContext(
       nativeVideo,originalMainApplication);
   trackMaterialRequest.canonicalMaterialPath={&originalMainApplication.materialDirectory36B970()};
  }
  // The one track's role owners (common_track50cr.h). The 1A9EE0 load (I2 cold,
  // R3) already mounted the course archive (12CEA0/132A20/132E40) and staged the
  // 1B4DC0 particle manager, ATM, fog, sky, lights and water from its data.
  const auto& roles=track.roles;
  auto* const staticWorld=roles.staticWorld.get();
  require50DU(track.loaded()&&staticWorld&&staticWorld->loaded(),
      "common track: the 1A9EE0 static world (05.07) is not loaded");
  bool commonRacePausePrewarmed=false;
  if(humanOriginalHud){
   // Acquire every actual HUD and particle reference before the single final
   // whole-table population. All borrowers share the same Application domain.
   require50DU(originalMainApplication.stageSpeedBoostNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceSpeedBoost),"direct human Speed/Boost source staging rejected");
   require50DU(originalMainApplication.stageTimeStatsNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceTimeStats,true),"direct human TimeStats source staging rejected");
   require50DU(originalMainApplication.stageDamageNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceDamage),"direct human Damage source staging rejected");
   require50DU(sourcePositionStatsPrestage.stage(originalMainApplication,originalMainApplication.assetRoot,
       trackMaterialRequest.canonicalContext,sourcePositionStatsPrestageDiagnostic),"direct human PositionStats source staging rejected");
   // Original Pause on every race (1A8A40 Pause100). A rejection is logged
   // through pause_refused; Start is then ignored (no host Pause fallback).
   if(!originalMainApplication.prewarmPauseCold50LA(sourcePause))
    common_race_log50CR::pauseRefused(0x1BA130u,"Pause100 cold prewarm rejected");
   else commonRacePausePrewarmed=true;
  }
  if(nativeModelDomain){
   require50DU(trackMaterialRequest.canonicalDomain==nativeModelDomain,
       "actual racer and track must share the Application material domain");
   // Finish each real installed model's resource/ref ownership once, before
   // the existing whole-table14D370 scan. Do not replay authored transforms
   // over the graph or postpone positive racer rows until after population.
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    const auto* rig=racerRigs[slot].get();
    require50DU(rig&&(rig->modelCount==3||rig->modelCount==5)&&
        bool(nativeBottomRightModels[slot])==(rig->modelCount==5)&&
        bool(nativeBottomLeftModels[slot])==(rig->modelCount==5),
        "actual lower model pair differs from selected Rig topology");
    for(const auto role:{native_racer50HW::Rig::right,native_racer50HW::Rig::left,
        native_racer50HW::Rig::bottomRight,native_racer50HW::Rig::bottomLeft,native_racer50HW::Rig::chariot}){
     if(role>=rig->modelCount)continue;
     auto* model=(role==native_racer50HW::Rig::right?nativeRightModels:
         role==native_racer50HW::Rig::left?nativeLeftModels:
         role==native_racer50HW::Rig::bottomRight?nativeBottomRightModels:
         role==native_racer50HW::Rig::bottomLeft?nativeBottomLeftModels:nativeChariotModels)[slot].get();
     require50DU(model&&racerVisuals[slot]&&model->actualRig()==racerRigs[slot].get()&&
         model->modelRole()==role&&model->materialDomain()==nativeModelDomain.get(),
         "actual original model owner missing before material population");
     model->completeResources1538B0(originalMainApplication.materialDirectory36B970());
     const auto* geometry=model->modelGeometry();
     require50DU(geometry&&geometry->payload()&&model->materialReferences(),
         "actual model material source owner missing");
     trackMaterialRequest.canonicalRacerModels.push_back({geometry->payload(),model->materialReferences()});
    }
   });
  }
  // The original race load 1A8A40 runs for EVERY race (one race class,
  // vtable 3CEE80): Pause100, then Results104 and F8, inside the racer
  // population window (racer sources admitted, before the one combined
  // population/collector), through the one static world's population hook.
  if(commonRacePausePrewarmed&&humanCourseEnabled&&!allAiEnabled){
   const auto commonRace=native_common_race50PD::race(*this);
   if(commonRace)staticWorld->beforeRacerPopulation=[&,commonRace]{
    OriginalResults104Diagnostic50LA loadDiagnostic;
    const bool loaded=originalMainApplication.loadHumanTournamentPauseResultsBeforeCollector50LA(
        *commonRace,sourcePause,roster,dynamicRacer60,loadDiagnostic);
    std::cerr<<"common_race_results_load loaded="<<loaded<<" pc="<<std::hex<<loadDiagnostic.sourcePC<<std::dec
        <<" status="<<static_cast<unsigned>(loadDiagnostic.status)
        <<" operation="<<(loadDiagnostic.operation?loadDiagnostic.operation:"")
        <<" pause_storage="<<bool(sourcePause.storage())<<" pause_pc="<<std::hex<<sourcePause.storageDiagnostic.sourcePC
        <<" pause_material_pc="<<sourcePause.storageDiagnostic.material.sourcePC<<std::dec
        <<" pause_material_status="<<static_cast<unsigned>(sourcePause.storageDiagnostic.material.status)
        <<" pause_object="<<sourcePause.storageDiagnostic.object<<'\n';
    return true;
   };
   else std::cerr<<"common_race_results_load loaded=0 operation=no counted race\n";
  }
  // Camera inputs shared by the camera preparation below (every course); each
  // is made once.
  auto makeSourceCameraDisplay=[&](){
   if(sourceCameraDisplay)return;
   sourceCameraDisplay=std::make_unique<sourceCamera::DisplayInputs>(sourceCamera::DisplayInputs{
       sourceVideo,sourceRendererDispatch,renderer,rendererDiagnostic,2,640,448,
       sourceFadeManagerA8,sourceFadeDuration98,0,0,
       static_cast<std::uint8_t>(originalUiStarted?originalMainApplication.state->globalFlags228:applicationEvents.rootFlags228),
       OriginalCameraDrainPolicy::initializedContext,
       &sourceTrackFog,
       consumedStage.race()?&originalMainApplication.applicationMode22C():nullptr});
  };
  auto prepareIntroAuthoring=[&](){
   // 174E6C..174EA8: Single Race keeps its authored cut count only in
   // Tournament (P+34 == 2, 174E90) or with App+350 set (174E9C); otherwise
   // the count is zeroed and no cut child is built. A Free Play counted race
   // therefore has no authored intro: no cut/controller authoring here.
   // 174EA8: subtype 2/3 always zero the cut count (Time Trial is counted, 148r).
   // A course whose TOB authors no cut (World+3E4 == 0) has no intro either (fix-B).
   const bool introCutsAuthored=nativeOrdinaryLaunch11258==1u&&(dynamicProfile.game34==2u||cameraStartup350!=0)&&
       consumedStage.race()&&consumedStage.race()->world74()&&consumedStage.race()->world74()->cutCount3E4()!=0u;
   if(!introAuthoring&&introCutsAuthored&&(consumedStage.race()||sourceConsumedIntroActivation)){
    introAuthoring=originalMainApplication.retainNativeTaIntroAuthoring50LA();
    require50DU(bool(introAuthoring),"actual selected-track intro authoring custody rejected");
    const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
    const auto audioRoot=suppliedAudioRoot?std::filesystem::path(suppliedAudioRoot):
        runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO");
    std::string error;
    require50DU(platform::prepareOriginalTaIntroPcm50LA(introAuthoring,audioRoot,introPcm,error),
        error.empty()?"original selected-track intro PCM preparation rejected":error.c_str());
   }
  };
  // One population for every course: the one static world (05.07) admits the
  // racer sources and runs the single 14D370 population and collector.
  const bool trackMaterialsLoaded=staticWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic);
  if(!trackMaterialsLoaded){
   const auto& failure=trackMaterialDiagnostic;const auto& canonical=failure.canonical;
   std::cerr<<"selected-track material diagnostic stage="<<failure.stage
       <<" source_member="<<failure.sourceMember<<" instance="<<failure.instance
       <<" surface="<<failure.surface<<" material="<<failure.material
       <<" message="<<failure.message
       <<" canonical_status="<<static_cast<unsigned>(canonical.status)
       <<" canonical_pc="<<canonical.sourcePC<<" canonical_row="<<canonical.row
       <<" canonical_slot="<<canonical.slot<<" canonical_pass="<<canonical.pass
       <<" canonical_map="<<canonical.map<<" canonical_field="<<(canonical.field?canonical.field:"")
       <<" document_error="<<static_cast<unsigned>(canonical.document.error)
       <<" document_offset="<<canonical.document.byteOffset<<" document_pc="<<canonical.document.sourcePC
       <<" program_status="<<static_cast<unsigned>(canonical.program.status)
       <<" program_pc="<<canonical.program.sourcePC<<" program_row="<<canonical.program.row
       <<" texture_status="<<static_cast<unsigned>(canonical.texture.status)
       <<" texture_pc="<<canonical.texture.sourcePC<<" texture_row="<<canonical.texture.row
       <<" texture_source="<<canonical.texture.source<<" texture_offset="<<canonical.texture.byteOffset
       <<" racer_source_status="<<static_cast<unsigned>(failure.racerSource.status)
       // Status names (original_racer_material_sources50la_fixture.h:13). "unsealed" at 132820
       // on a member means the provider lookup answered unknown: two active mounts serve that
       // name with different bytes (or the namespace is not sealed).
       <<" racer_source_reason="<<[&]{
           static const char* const names[]={"complete","input","lookup_unknown_ambiguous_or_unsealed","model",
               "reference","missing_material","document","unsupported","missing_texture","conflict","allocation"};
           const auto i=static_cast<unsigned>(failure.racerSource.status);
           return i<sizeof(names)/sizeof(names[0])?names[i]:"unknown";}()
       <<" racer_source_pc="<<failure.racerSource.sourcePC<<" racer_source_member="<<failure.racerSource.member
       <<" racer_source_slot="<<failure.racerSource.modelSlot<<" racer_source_pass="<<failure.racerSource.pass
       <<" racer_source_map="<<failure.racerSource.map
       <<" bank_error="<<static_cast<unsigned>(failure.bank.error)
       <<" bank_texture="<<failure.bank.texture<<" bank_slot="<<failure.bank.slot
       <<" bank_offset="<<failure.bank.byteOffset<<" bank_detail="<<failure.bank.parserMessage<<'\n';
  }
  staticWorld->beforeRacerPopulation=nullptr;
  require50DU(trackMaterialsLoaded,
      trackMaterialDiagnostic.message.empty()?"selected-track material load failed":trackMaterialDiagnostic.message.c_str());
  if(humanOriginalHud){
   require50DU(originalMainApplication.finishTimeStatsNativeCold50LA(sourceTimeStats),"direct human TimeStats final verification rejected");
   require50DU(originalMainApplication.finishDamageNativeCold50LA(sourceDamage),"direct human Damage final verification rejected");
   require50DU(sourcePositionStatsPrestage.verify(sourcePositionStatsPrestageDiagnostic),"direct human PositionStats final verification rejected");
   require50DU(originalMainApplication.finishSpeedBoostNativeCold50LA(sourceSpeedBoost),"direct human Speed/Boost final verification rejected");
  }
  if(allAiEnabled&&originalCameraEnabled&&trackMaterialRequest.canonicalDomain){
   const bool speedStaged=originalMainApplication.stageSpeedBoostNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceSpeedBoost);
   if(!speedStaged)std::cerr<<"original_speed_boost_cold_rejected phase="
       <<static_cast<unsigned>(sourceSpeedBoost.failedAt)<<" source_pc="<<sourceSpeedBoost.object.sourcePC
       <<" material_pc="<<sourceSpeedBoost.object.material.sourcePC<<" message="<<sourceSpeedBoost.failure<<'\n';
   require50DU(speedStaged,"original Speed/Boost cold source join rejected");
   const bool prepared=originalMainApplication.stageTimeStatsNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceTimeStats);
   if(!prepared)std::cerr<<"original_time_stats_cold_rejected phase="
       <<static_cast<unsigned>(sourceTimeStats.failedAt)<<" source_pc="<<sourceTimeStats.object.sourcePC
       <<" material_pc="<<sourceTimeStats.material.sourcePC<<" message="<<sourceTimeStats.failure<<'\n';
   require50DU(prepared,"original TimeStats cold source join rejected");
   // Both owners observe this SAME final collector. No later GPU-bound
   // material registration is smuggled in through either completion hook.
   const bool speedFinished=originalMainApplication.finishSpeedBoostNativeCold50LA(sourceSpeedBoost);
   if(!speedFinished)std::cerr<<"original_speed_boost_final_rejected source_pc="<<sourceSpeedBoost.object.sourcePC
       <<" material_pc="<<sourceSpeedBoost.object.material.sourcePC<<" message="<<sourceSpeedBoost.failure<<'\n';
   require50DU(speedFinished,"original Speed/Boost final source join rejected");
  }
  if(nativeModelDomain){
   // Every racer's visual consumers bind these same final canonical references.
   // Resolve the SAME1C4/pass/map/context into the already-authenticated
   // host base bank and the independently owned context-2 projection before
   // windowMaterials or GPU publication. The paired consumer stays restricted
   // to the actual canonical body material and its recovered source groups.
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    auto& model=*nativeRightModels[slot];
    auto& leftModel=*nativeLeftModels[slot];
    using EngineInput=native_racer50HW::Visual::CanonicalEngineModelInput50LA;
    const auto engineInput=[](const native_racer_model50LA::Owner& owner)->EngineInput{
     return {owner.modelScene(),owner.modelGeometry(),owner.materialReferences()};
    };
    std::array<EngineInput,4> engines{{engineInput(leftModel),engineInput(model),{}, {}}};
    if(racerRigs[slot]->modelCount==5){
     engines[2]=engineInput(*nativeBottomLeftModels[slot]);
     engines[3]=engineInput(*nativeBottomRightModels[slot]);
    }
    // Chariot base maps first; its own pair/single materials then join the
    // same load-once map-2 bank as the engines (one rule for every model).
    auto& chariotModel=*nativeChariotModels[slot];
    racerVisuals[slot]->bindCanonicalChariot(*chariotModel.modelScene(),*chariotModel.modelGeometry(),
        *chariotModel.materialReferences(),*nativeModelDomain);
    racerVisuals[slot]->bindCanonicalEngineModels(engines,engineInput(chariotModel),
        *nativeModelDomain,nativeSelectedLights);
    model.bindRendererMaterialFlame(coldFlame);
    leftModel.bindRendererMaterialFlame(coldFlame);
    if(racerRigs[slot]->modelCount==5){
     auto& bottomRightModel=*nativeBottomRightModels[slot];
     bottomRightModel.bindRendererMaterialFlame(coldFlame);
     auto& bottomLeftModel=*nativeBottomLeftModels[slot];
     bottomLeftModel.bindRendererMaterialFlame(coldFlame);
    }
   });
  }
  // A9: the ground-effects owner's attached placements (roles.emitters, 05.06),
  // every course: the renderer spans that carry each placement's world instance
  // are matched once from the one static world's spans. Unattached placements
  // (worldInstance == nullptr) have no span and are skipped.
  if(roles.emitters){
   const auto* spans=staticWorld->spans();require50DU(spans,"common track: static world spans missing");
   const std::size_t placements=roles.emitters->attachedCount();
   std::vector<bool> matched(placements,false),attached(placements,false);
   for(std::size_t fire=0;fire<placements;++fire){
    const auto* a=roles.emitters->attachment(fire);
    if(!a||!a->worldInstance)continue;
    attached[fire]=true;
    for(std::size_t index=0;index<spans->size();++index)if((*spans)[index].worldInstanceIndex==a->worldIndex){
     require50DU(index<=std::numeric_limits<std::uint32_t>::max()&&fire<=std::numeric_limits<std::uint8_t>::max(),
         "ground-effect attachment span index overflow");
     nativeFireSpanIds.push_back(static_cast<std::uint32_t>(index));nativeFireSpanOwner.push_back(static_cast<std::uint8_t>(fire));
     matched[fire]=true;
    }
   }
   for(std::size_t fire=0;fire<placements;++fire)
    require50DU(!attached[fire]||matched[fire],"ground-effect attachment has no actual renderer spans");
   nativeFireSpanDrawn.resize(nativeFireSpanIds.size());
  }
  // ATM (authored animated textures), particle Window and numeric renderer,
  // fog: one owner each in track.roles (05.05 fog, 05.06 particles, 05.14 ATM),
  // read by the renderer through request.commonTrack (R8). No per-course owner.
  // Original Main owns a separate mixer/device/WAV. The retained cold race
  // bank cannot open output during Main, even if RR_AUDIO is enabled.
  auto activateRaceAudio=[&](){
   require50DU(!originalUiActive,"race output cannot open while Course owns input");
  if(audioEnabled){
   if(!audioOutputRequested)std::cerr<<"ALL8 audio output muted: real logical owners and software sample clock retained; no device or capture requested.\n";
   const char* audioWav=audioOutputRequested?std::getenv("RR_AUDIO_WAV"):nullptr;
   std::string sessionAudioPath;
   if(audioWav&&(sessionIndex||originalUiStarted)){
    const std::filesystem::path base(audioWav);
    // The frontend finalizes the base WAV before this new race output opens.
    // Keep both real captures: never reopen, overwrite or append that file.
    const auto suffix=std::string(originalUiStarted?"-race":"")+
        (sessionIndex?"-session-"+std::to_string(sessionIndex+1):std::string());
    sessionAudioPath=(base.parent_path()/(base.stem().string()+suffix+base.extension().string())).string();
    audioWav=sessionAudioPath.c_str();
   }
   if(liveAudio->bank().ready){
    platform::AudioOutputRequest50HD audioRequest{};
    audioRequest.wavPath=audioWav;audioRequest.tryDevice=audioOutputRequested&&!headlessLive;
    const char* deviceSetting=std::getenv("RR_AUDIO_DEVICE");
    if(deviceSetting&&audioOutputRequested)audioRequest.tryDevice=std::string(deviceSetting)=="1";
    if(!liveAudio->openOutput(audioRequest,audioDiagnostic)){
     std::cerr<<"Audio output open error="<<static_cast<unsigned>(audioDiagnostic.error)
         <<" detail="<<audioDiagnostic.detail.data()<<'\n';
     require50DU(false,"native audio output open failed");
    }
    const auto audioInfo=liveAudio->info();
    std::cerr<<"Audio output device="<<platform::audioDeviceStatusName50HD(audioInfo.deviceStatus)
        <<" capture="<<audioInfo.captureActive<<" rate="<<audioInfo.sampleRate
        <<" detail="<<audioInfo.deviceDetail.data()<<'\n';
    if(audioWav)std::cerr<<"Audio recording session="<<sessionIndex+1<<" path="<<audioWav<<'\n';
   }else{
    require50DU(!audioWav,"original audio samples missing; refusing a silent proof recording");
    std::cerr<<"Audio unavailable: no original samples decoded; no device or WAV opened.\n";
   }
  }
  if((mode1Enabled||dynamicContactEnabled))
   require50DU(liveAudio->outputOpen(),"source events need the real shared output backend");
  // Main retained cold race PCM owners, but did not initialize/observe a race.
  // Do not seed countdown or standings audio from an unstarted world.
  if(audioEnabled&&raceEnabled){
   if(!originalCameraEnabled)require50DU(withRace([&](auto& actualRace){return raceAudio->observe(actualRace);}).accepted,"initial race audio observation rejected");
   // Recovered intro has not started the one countdown yet. Its real audio
   // observations begin only after source1AFD80 installs the actual duration.
   std::cerr<<"Race audio ready/missing/unsupported/invalid "<<raceAudio->bank().ready<<'/'
       <<raceAudio->bank().missing<<'/'<<raceAudio->bank().unsupported<<'/'
       <<raceAudio->bank().invalid<<" of "<<raceAudio->bank().clips.size()<<'\n';
  }
   if(audioEnabled){liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);}
  };
  if(!originalUiActive)activateRaceAudio();
  if(originalUiActive && audioEnabled){
   liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);
  }
  using GlStringFunction=const unsigned char*(*)(unsigned int);
  const auto glString=reinterpret_cast<GlStringFunction>(host.procAddress("glGetString"));
  if(glString){
   for(const auto field:{0x1f00u,0x1f01u,0x1f02u}){
    const auto* value=glString(field);
    std::cerr<<"GL "<<field<<": "<<(value?reinterpret_cast<const char*>(value):"unavailable")<<'\n';
   }
  }
  // Resizable/maximizable window: renderer capacity covers any attached
  // monitor (bounded 4K). Larger drawables keep the existing oversize pause.
  std::int32_t maximumWidth=1280,maximumHeight=896;
  native_display_capacity50DU::capacity(1280,896,maximumWidth,maximumHeight);
  std::cerr<<"native_render_capacity width="<<maximumWidth<<" height="<<maximumHeight<<"\n";
  rr::platform::loading_progress50DU::pulse("Allocating graphics targets");
  PlayerLoadingSession50DU::checkStop();
  platform::VehicleWindowRequest50DS renderRequest{};
  sourceDisplayLifetime->bindCreationRequest(renderRequest);
  renderRequest.maximumWidth=maximumWidth;renderRequest.maximumHeight=maximumHeight;
  renderRequest.enableHostHud=true;
  renderRequest.enableOriginalGameplayBlur=originalCameraEnabled;
  renderRequest.enableGameplayDisplayWallTiming=phaseTiming.enabled;
  // Original projected pod shadows, one path for every course: the renderer
  // copies the per-instance AllowShadowReceiving table (TOB 271A00, applied
  // by original_static_model_params50ps.h) and the instance zones at create.
  // Retires enableNativePodShadows, its course gate and
  // RR_DISABLE_NATIVE_SHADOWS (port stand-ins).
  std::vector<std::uint32_t> staticInstanceZones(actualWorld.instances.size(),0u);
  for(std::size_t i=0;i<actualWorld.instances.size();++i)staticInstanceZones[i]=actualWorld.instances[i].zone;
  require50DU(staticShadowReceiving.size()==actualWorld.instances.size()&&
      staticShadowReceiving.size()<=std::numeric_limits<std::uint32_t>::max(),"original shadow receiver table extent");
  renderRequest.staticShadowReceiving=staticShadowReceiving.empty()?nullptr:staticShadowReceiving.data();
  renderRequest.staticInstanceZone=staticInstanceZones.empty()?nullptr:staticInstanceZones.data();
  renderRequest.staticInstanceCount=static_cast<std::uint32_t>(staticShadowReceiving.size());
  // 2DEBC0/2DE7C0 portal walk: the world's own portals (zones are 1-based).
  std::vector<std::uint32_t> staticPortalZones;std::vector<float> staticPortalCorners;
  require50DU(actualWorld.portals.size()<=4096u,"original shadow portal table extent");
  staticPortalZones.reserve(actualWorld.portals.size()*2u);staticPortalCorners.reserve(actualWorld.portals.size()*12u);
  for(const auto& portal:actualWorld.portals){
   staticPortalZones.push_back(portal.zones[0]);staticPortalZones.push_back(portal.zones[1]);
   for(const auto& corner:portal.vertices)for(double value:corner)staticPortalCorners.push_back(static_cast<float>(value));
  }
  renderRequest.staticPortalZones=staticPortalZones.empty()?nullptr:staticPortalZones.data();
  renderRequest.staticPortalCorners=staticPortalCorners.empty()?nullptr:staticPortalCorners.data();
  renderRequest.staticPortalCount=static_cast<std::uint32_t>(actualWorld.portals.size());
  renderRequest.output=headlessLive?platform::VehicleWindowOutput50DS::headless:platform::VehicleWindowOutput50DS::defaultBackBuffer;
  // Explicit diagnostic material policy: authored pass1/slot1, largest retained
  // mip, linear filtering and repeat. This does not reproduce original shaders.
  platform::MaterialTextureGlRequest50EB textureRequest{};
  textureRequest.context=&host;textureRequest.isCurrent=renderRequest.isCurrent;
  require50DU(sourceDisplayLifetime->bindTextureUploadRequest(textureRequest),
      "actual Application direct-upload lifetime binding rejected");
  const char* missingTextureProc=nullptr;
  require50DU(platform::loadMaterialTextureFunctions50EE(&host,renderRequest.procAddress,
      textureRequest.gl,missingTextureProc),"required texture GL entry point unavailable");
  if(originalUiActive){
   courseTargets.prepare(trackAdmission);
   auto destination=original_main_entry50KG::courseRaceDestination([&,originalAiOwnsSlot](const platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection& selected){
    const auto* target=courseTargets.admits(selected);
    if(!target||!rootedCourseRace||!originalUiActive||courseRaceLaunched||courseColdLoad.pending()||
       !raceEnabled||!nativeOriginalFleet||!originalAiEnabled||!originalCameraEnabled||
       race.phase!=race_session50HM::Phase::Uninitialized||racerBank.count()!=raceRacerCount||
       !renderer.initialized()||!trackGpuTextures.initialized()||!skyGpuTextures.initialized()||
       !gameplayBinder.loaded()||!gameplayBinder.ownsResources()||!activateColdRace||
       &s.noiseOwner()!=&originalMainApplication.state->sharedNoise308030||
       &sessionRandom!=&originalMainApplication.state->sharedRandomSelectors)return false;
    // Match the dedicated frontend: a rooted target cannot queue an ALL8
    // replacement, even from this retained embedded branch. One rule for
    // every course (B-C02): the target is rooted when the original asset root is.
    const bool targetNeedsHuman=!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
    if(targetNeedsHuman&&(!humanCourseEnabled||allAiEnabled))return false;
    if(selected.laps!=3&&!humanCourseEnabled)return false;
    // The current world is real and unadvanced. A different target is a
    // queued authenticated cold-load plan; it is never marked world-ready.
    // One readiness rule for every course: each role owner of the one track is
    // required exactly when the track's own data authored it (SC's rule).
    if(!track.loaded()||!staticWorld->loaded())return false;
    if(track.counts.triggers!=0u&&(!roles.triggers||!roles.triggers->ready))return false;
    if(roles.atm&&(!roles.atm->loaded()||!roles.atm->ownsResources()))return false;
    if(roles.particles&&!roles.particles->prepared())return false;
    if(roles.fog&&!roles.fog->prepared())return false;
    if(track.hasWater()!=bool(roles.water))return false;                       // A9
    if(roles.collision&&(!roles.collision->geometryReady()||!roles.collision->queryReady()))return false;
    if(!mapView.runtimeValid()||!aiView.runtimeValid()||!mapView.sameOwner(aiView)||
       !mapView.sameWorld(liveWorld)||!aiView.sameWorld(liveWorld))return false;
    if(triggerAudio&&(!triggerAudio->available()||triggerAudio->failed()))return false;
    if(particleMaterials.loaded()&&!particleMaterials.ownsResources())return false;
    // Every course: the common track-animation owner is ready with its window when present.
    if(trackAnimations&&(!trackAnimations->ready()||!trackAnimationWindow.loaded()||!trackAnimationWindow.ownsResources()))return false;
    if(trackAnimations&&(!trackAnimationBinding||!trackAnimationBinding->ready()||
       trackAnimationBinding->runtimeOwner()!=trackAnimations.get()))return false;
    if(humanCourseEnabled&&!humanOriginalHud)return false;
    if(!sourceCameraSession||!sourceCameraSession->ready())return false;
    for(std::size_t slot=0;slot<racerBank.count();++slot){
     if(!racerBank.active(slot)||!racerRigs[slot]||!racerCollisions[slot]||!racerVisuals[slot]||!racerCables[slot]||
        (originalAiOwnsSlot(slot)&&(!racerBank.read(slot,originalAiReads.initialized)||
         racerBank.read(slot,racerFrameReads.vehicle58)!=1u))||
        (humanCourseEnabled&&slot==0&&(racerBank.read(slot,racerFrameReads.vehicle58)!=0u||
         racerBank.read(slot,racerFrameReads.aiOwnersBound))))return false;
    }
    return true;
   },[&](const original_course_targets50KX::Selection& selected){return courseTargets.availability(selected);});
   originalMain.prepare(originalMainApplication,nativeVideo,textureRequest,renderRequest,std::move(destination),&monotonicNow50DU,sessionIndex);
  }
  std::vector<platform::MaterialTextureSampler50EB> samplers;
  std::vector<platform::VehicleWindowMaterialBinding50DS> materialBindings[3];
  if(playerUsesRosterRig){
   const auto& visual=*racerVisuals[0];samplers=visual.samplers();
   auto playerTextureRequest=textureRequest;
   playerTextureRequest.samplers=samplers.data();
   playerTextureRequest.samplerCount=static_cast<std::uint32_t>(samplers.size());
   playerTextureRequest.maximumGpuBytes=8u*1024u*1024u;
   require50DU(gpuTextures.create(playerTextureRequest,visual.materials,textureDiagnostic),
       "selected original player texture upload failed");
   const auto& secondBank=visual.canonicalEngineSpecularMaterials50LA();
   if(secondBank.loaded()){
    const auto& secondSamplers=visual.canonicalEngineSpecularSamplers50LA();
    auto secondRequest=textureRequest;
    secondRequest.samplers=secondSamplers.data();
    secondRequest.samplerCount=static_cast<std::uint32_t>(secondSamplers.size());
    secondRequest.maximumGpuBytes=8u*1024u*1024u;
    require50DU(specularGpuTextures50LA.create(secondRequest,secondBank,textureDiagnostic),
        "canonical player context-2 texture upload failed");
   }
   renderRequest.materials=visual.windowMaterials(&gpuTextures,
       secondBank.loaded()?&specularGpuTextures50LA:nullptr);
  }else{
  const auto* textureBank=materialOwner->bank();require50DU(textureBank,"cold texture bank missing");
  samplers.assign(textureBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat});
  // Both actual Flame PSMs specify vertical clamp. Repeating V wraps the
  // bright tail row into the nearly-zero tip UV, producing a detached ring.
  for(unsigned component=1;component<3;++component){
   const auto* selected=materialOwner->component(component);
   require50DU(selected,"engine sampler material selection missing");
   for(const auto& entry:*selected)if(entry.materialIndex==1){
    require50DU(entry.textureIndex<samplers.size(),"flame sampler bank index");
    samplers[entry.textureIndex].t=platform::MaterialTextureWrap50EB::clampToEdge;
   }
  }
  textureRequest.samplers=samplers.data();textureRequest.samplerCount=static_cast<std::uint32_t>(samplers.size());
  textureRequest.maximumGpuBytes=1024u*1024u;
  require50DU(gpuTextures.create(textureRequest,*textureBank,textureDiagnostic),"actual Pod texture upload failed");
  for(unsigned component=0;component<3;++component){
   const auto* selected=materialOwner->component(component);require50DU(selected,"dense material selection missing");
   for(const auto& entry:*selected){
    platform::VehicleWindowMaterialBinding50DS binding{
        platform::VehicleWindowMaterialMode50DS::requiredTexture,entry.textureIndex};
    if(component>0&&entry.materialIndex==1){
     auto name=entry.materialName;
     for(char& c:name)if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');
     require50DU(name==(component==1?"flame01l":"flame01r"),"actual engine flame material identity");
     binding.blend=platform::VehicleWindowBlend50DS::additive;
     binding.fixedBlendAlpha=true;binding.alphaScale=255.f/128.f;
     binding.depthWrite=false;
    }
    materialBindings[component].push_back(binding);
   }
   renderRequest.materials.component[component]={materialBindings[component].data(),
       static_cast<std::uint32_t>(materialBindings[component].size())};
  }
  renderRequest.materials.textures=&gpuTextures;
  }
  renderRequest.mode=platform::VehicleWindowMode50DS::callerMaterialTextures;
  std::vector<platform::VehicleWindowTrackSpan50DS> trackBindings;
  // The loaded course's own Fog fractions (278570; defaults Visibility 1,
  // Haziness 0.5): the one fog owner the 1A9EE0 load prepared (05.05, 273C30),
  // borrowed by the camera session and the renderer, for every course.
  rr::game::OriginalTrackFogDiagnostic50CR trackFogDiagnostic;
  rr::game::OriginalRaceFogFields loadedTrackFog{};
  if(!roles.fog||!roles.fog->loadFields(loadedTrackFog,trackFogDiagnostic)){
   std::cerr<<"track_fog_rejected status="<<rr::game::originalTrackFogStatusName50CR(trackFogDiagnostic.status)
       <<" pc=0x"<<std::hex<<trackFogDiagnostic.sourcePC<<std::dec<<" offset="<<trackFogDiagnostic.sourceOffset
       <<" zone="<<trackFogDiagnostic.zone<<'\n';
   require50DU(false,"track Fog authoring rejected");
  }
  std::cerr<<"track_fog_fractions course="<<track.code()<<" visibility="<<loadedTrackFog.authoredFraction3C
      <<" haziness="<<loadedTrackFog.authoredFraction40<<'\n';
  // Far range proof line: far=4000 authored=0 on every stock course.
  std::cerr<<"track_fog_far course="<<track.code()<<" far="<<loadedTrackFog.far50
      <<" authored="<<(roles.fog->fog().farAuthored?1:0)<<" near="<<loadedTrackFog.near54
      <<" start="<<loadedTrackFog.start34<<" end="<<loadedTrackFog.end38<<'\n';
  // ONE renderer request for every course (B-C20): the renderer reads the
  // track's role owners (static world, fog, sky, particles, water, ATM, UV and
  // track animation) from this one Track. The consumed stage owns the Track;
  // without a counted race the caller prefix does (outlives this scope).
  // PORT STAND-IN: the non-counted fallback is a non-owning alias of the
  // caller-owned Track (no control block), for direct starts only.
  {
   std::shared_ptr<const common_track50CR::Track> requestTrack=consumedStage.track();
   if(requestTrack)require50DU(requestTrack.get()==&track,"consumed stage holds a different Track than this race");
   else requestTrack=std::shared_ptr<const common_track50CR::Track>(std::shared_ptr<const void>{},&track);
   renderRequest.commonTrack=std::move(requestTrack);
  }
  // Environment-mapped passes (EnvironmentMapCompressed): the track's one
  // producer over roles.staticWorld + roles.lights, every course (null = none).
  {
   std::string environmentError;
   renderRequest.track.environmentUv=track_environment_uv50CR::make(track,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,environmentError);
   require50DU(renderRequest.track.environmentUv||environmentError.empty(),environmentError.c_str());
  }
  renderRequest.track.mesh=staticWorld->mesh();
  // Authored fog RGB and gameplay ranges are source-backed. The native shader
  // policy is enabled by default; explicit 0 is a diagnostic opt-out (no
  // course forces it, B-C03). This does not claim original material/
  // interpolation/quantization parity.
  const char* authoredFogSetting=std::getenv("RR_AUTHORED_WORLD_FOG");
  require50DU(!authoredFogSetting||std::string(authoredFogSetting)=="0"||
      std::string(authoredFogSetting)=="1","RR_AUTHORED_WORLD_FOG must be 0 or 1");
  // The sky's clear RGB (274AE0; a track authoring no sky cube still has one).
  // 13E830 World3EC, from the 05.11 sky step (track.roles.sky): present with zero cubes (SC).
  const float* trackClearRgba=sourceSkySelection.clearRgba();
  if(!authoredFogSetting||std::string(authoredFogSetting)=="1"){
   auto& fog=renderRequest.track.worldFog;
   const float* color=trackClearRgba;
   require50DU(color,"authenticated world fog RGB missing");
   for(unsigned i=0;i<3;++i)fog.color[i]=color[i];
   // 273C30: end = near + visibility x (far - near); start = (end - base) x
   // (1 - haziness), from this course's own [Fog] block (one reader, every course).
   // near/far/base are the 13E920 range 1A98DC left in World3EC: 1333.3 / 4000 / 64
   // on every stock course; far is the N64-canonical FarDistance on a DLC course
   // that authors it (original_race_fog_far50nf.h).
   const float minimum=loadedTrackFog.near54,maximum=loadedTrackFog.far50;
   const float haziness=loadedTrackFog.authoredFraction40;
   const float visibility=loadedTrackFog.authoredFraction3C;
   fog.end=minimum+visibility*(maximum-minimum);
   fog.start=(fog.end-loadedTrackFog.minimum58)*(1.f-haziness);fog.enabled=true;
   std::cerr<<"Authored native world fog: start "<<fog.start
       <<" end "<<fog.end<<"; source ranges established, native world shader parity pending\n";
  }
  if(originalCameraEnabled){
   const float* color=trackClearRgba;
   require50DU(color,"actual source camera fog RGB missing");
   const auto channel=[&](unsigned axis){return static_cast<std::uint32_t>(std::lround(color[axis]*255.f));};
   sourceTrackFog.color30=0xff000000u|(channel(0)<<16)|(channel(1)<<8)|channel(2);
   sourceTrackFog.authoredFraction3C=loadedTrackFog.authoredFraction3C;
   sourceTrackFog.authoredFraction40=loadedTrackFog.authoredFraction40;
  }
  const auto* worldNormals=staticWorld->normals();
  require50DU(worldNormals,"common track: static world normals missing");
  renderRequest.track.sourceNormals=worldNormals->data();
  renderRequest.track.sourceNormalCount=static_cast<std::uint32_t>(worldNormals->size());
  // One shared ceiling for every course world. The renderer validates the mesh
  // it actually builds against it; it allocates the mesh's own size.
  constexpr std::uint32_t courseWorldVertexCeiling=32u*1024u*1024u,courseWorldIndexCeiling=16u*1024u*1024u,
      courseWorldNormalCeiling=16u*1024u*1024u;
  renderRequest.track.maximumNormalBytes=courseWorldNormalCeiling;
  // The authored projection (ProjectedPrelit) and the environment-map UVs share the renderer's
  // one float2 stream. Which one a track supplies is read from its data: projection UVs only
  // when a static-world span draws a projected pass (GA/GB/GC/TB author them), environment UVs
  // only when a pass is EnvironmentMapCompressed (RC's crystals). Both at once is not
  // representable by that stream and is refused here with its reason (no stock course does).
  // Replaces the retired `!isRcTrack` branch. The environment budgets are sized from the
  // producer and the mesh (they were left 0 when R8 retired maximumRcProjection/StagingBytes,
  // so every environment-mapped track was refused at "environment-map UV producer
  // association/projection budget").
  std::size_t projectedSpans=0;
  if(const auto* spans=staticWorld->spans())for(const auto& span:*spans)projectedSpans+=span.projectionPass!=0;
  if(renderRequest.track.environmentUv){
   require50DU(!projectedSpans,"track authors both projected (ProjectedPrelit) and environment-mapped passes: one shared UV stream");
   const auto* mesh=staticWorld->mesh();
   require50DU(mesh&&!mesh->vertices.empty(),"environment-map UVs need the static world mesh");
   renderRequest.track.maximumEnvironmentUvBytes=mesh->vertices.size()*sizeof(std::array<float,2>);
   renderRequest.track.maximumEnvironmentStagingBytes=renderRequest.track.environmentUv->uvCount()*sizeof(std::array<float,2>);
   std::cerr<<"track_uv_stream course="<<track.code()<<" stream=environment uvs="<<renderRequest.track.environmentUv->uvCount()
       <<" ranges="<<renderRequest.track.environmentUv->rangeCount()<<" mesh_vertices="<<mesh->vertices.size()<<'\n';
  }else if(trackSession->originalStaticCandidates&&projectedSpans){
   const auto* projectionUvs=staticWorld->projectionUvs();
   require50DU(projectionUvs,"original projected UV publication missing");
   renderRequest.track.sourceProjectionUvs=projectionUvs->data();
   renderRequest.track.sourceProjectionCount=projectionUvs->size();
   renderRequest.track.maximumProjectionBytes=projectionUvs->size()*sizeof(rr::assets::TrackProjectedUv);
   std::cerr<<"track_uv_stream course="<<track.code()<<" stream=projection projected_spans="<<projectedSpans<<'\n';
  }else std::cerr<<"track_uv_stream course="<<track.code()<<" stream=none\n";
  // Bounded host rendering optimization; an explicit debugging opt-out keeps
  // the same scene and physics available for comparisons.
  const char* disableCulling=std::getenv("RR_DISABLE_TRACK_CULLING");
  renderRequest.track.enableFrustumCulling=!(disableCulling&&std::string(disableCulling)=="1");
  renderRequest.track.zoneCount=static_cast<std::uint32_t>(actualWorld.zones.size());
  renderRequest.track.maximumVertexBytes=courseWorldVertexCeiling;
  renderRequest.track.maximumIndexBytes=courseWorldIndexCeiling;
  const auto* trackBank=staticWorld->bank();
  const auto* trackSpans=staticWorld->spans();
  const auto* trackSelections=staticWorld->selections();
  require50DU(trackBank&&trackSpans&&trackSelections,"selected-track material publication missing");
  std::vector<platform::MaterialTextureSampler50EB> trackSamplers(trackBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat});
  for(const auto& entry:*trackSelections){
   require50DU(entry.textureIndex<trackSamplers.size(),"original sampler texture unresolved");
   auto& sampler=trackSamplers[entry.textureIndex];
   sampler.s=entry.clampU?platform::MaterialTextureWrap50EB::clampToEdge:platform::MaterialTextureWrap50EB::repeat;
   sampler.t=entry.clampV?platform::MaterialTextureWrap50EB::clampToEdge:platform::MaterialTextureWrap50EB::repeat;
  }
  auto trackTextureRequest=textureRequest;
  trackTextureRequest.samplers=trackSamplers.data();
  trackTextureRequest.samplerCount=static_cast<std::uint32_t>(trackSamplers.size());
  trackTextureRequest.maximumGpuBytes=trackTextureBytes;
  require50DU(trackGpuTextures.create(trackTextureRequest,*trackBank,textureDiagnostic),
      "actual selected-track texture upload failed");
  // One texture-scroll owner for every course (29B1D0/29B050), loaded by the
  // 1A9EE0 animation step (05.14) from the track's own authored pass fields.
  auto* const uvAnimation=roles.uvAnimation.get();
  if(uvAnimation)uvAnimation->log(std::cerr);
  std::size_t authoredAa1Spans=0; // spans drawn with the authored AA1 coverage
  trackBindings.reserve(trackSpans->size());
  for(const auto& span:*trackSpans){
   platform::VehicleWindowTrackSpan50DS binding{};
   binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.sourcePass=span.sourcePass;binding.sourcePassCount=span.sourcePassCount;
   binding.projectionPass=span.projectionPass;
   binding.material.mode=span.textureIndex==trackTextures::unresolved?
       platform::VehicleWindowMaterialMode50DS::untextured:
       platform::VehicleWindowMaterialMode50DS::requiredTexture;
   binding.material.textureIndex=span.textureIndex;
   require50DU(span.materialSelection<trackSelections->size(),"selected-track material selection missing");
   material_alpha50gx_fixture::Policy alpha{};
   const auto& sourceSelection=(*trackSelections)[span.materialSelection];
   // One alpha rule for every course (selectMaSa through the one static world).
   const bool alphaSelected=staticWorld->alpha(span.materialSelection,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,alpha);
   require50DU(alphaSelected,"selected-track material alpha policy mismatch");
   // Native AA1 coverage from the pass's own authored onePassAntialiasingOnOff
   // field (OnePassAntiAliasingOnOff), on every course where it is authored.
   {
    bool aa1Valid=true;
    const bool aa1=course_world_texture50ST::authoredOnePassAa1(sourceSelection.sourcePassFields,aa1Valid);
    require50DU(aa1Valid,"selected-track authored onePassAntialiasingOnOff value unsupported");
    if(aa1){binding.nativeCoverage=platform::NativeTrackCoverage50DS::fourSampleAa1;++authoredAa1Spans;}
   }
   using AlphaBlend=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==AlphaBlend::additive?
       platform::VehicleWindowBlend50DS::additive:alpha.blend==AlphaBlend::sourceAlpha?
       platform::VehicleWindowBlend50DS::sourceAlpha:platform::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
   binding.material.alphaScale=alpha.alphaScale;
   binding.material.fixedBlendAlpha=alpha.fixedBlendAlpha;
   // These authenticated beam placements explicitly disable source lighting.
   binding.material.unlit=alpha.unlit;
   binding.material.sourcePrelit=alpha.sourcePrelit;
   // Geometry data, not a course or material name: a surface stored without a
   // colour stream draws with the original program's light-row bytes, not the
   // export's diagnostic white (renderer: sourceColorStreamAbsent).
   binding.material.sourceColorStreamAbsent=!span.hasOriginalColors;
   binding.discardZeroAlpha=alpha.discardZeroAlpha;
   binding.sourceAlphaTest=alpha.sourceAlphaTest;
   binding.sourceAlphaReference=alpha.sourceAlphaReference;
   binding.sourceFogKnown=alpha.sourceFogKnown;binding.sourceFogEnabled=alpha.sourceFogEnabled;
   // Common GS ALPHA lowering (every course): the destination-alpha pair and
   // the environment-map program come from the pass data, not the course.
   binding.gsDestinationAlphaPass=alpha.gsDestinationAlphaPass;
   binding.sourceEnvironmentMap=alpha.sourceEnvironmentMap;
   if(uvAnimation&&uvAnimation->animated(static_cast<std::uint32_t>(trackBindings.size())))
    require50DU(uvAnimation->bindRenderSpan(static_cast<std::uint32_t>(trackBindings.size()),binding),
        "track UV-animated span lowering rejected");
   require50DU(span.worldInstanceIndex<staticShadowReceiving.size(),"static shadow receiver source instance rejected");
   binding.allowPodShadow=staticShadowReceiving[span.worldInstanceIndex]!=0;
   binding.worldInstance=static_cast<std::uint32_t>(span.worldInstanceIndex);
   // Authored zone of the span's static model (SCB ZoneN-ModelM), every course.
   require50DU(span.worldInstanceIndex<actualWorld.instances.size()&&
       actualWorld.instances[span.worldInstanceIndex].zone<=actualWorld.zones.size(),
       "static model zone outside the world's authored zones");
   binding.sourceZone=actualWorld.instances[span.worldInstanceIndex].zone;
   // An EnvironmentMapCompressed pass draws its projected UVs from the track load's
   // environment-UV producer (renderRequest.track.environmentUv), not from the ATM owner: a
   // course with environment maps and no ATM is drawn (C8).
   require50DU(!binding.sourceEnvironmentMap||renderRequest.track.environmentUv,"environment-map pass lacks its projection owner");
   trackBindings.push_back(binding);
  }
  {
   std::size_t absent=0,absentBlended=0;
   for(const auto& b:trackBindings)if(b.material.sourceColorStreamAbsent){
    ++absent;if(b.material.blend!=platform::VehicleWindowBlend50DS::opaque)++absentBlended;}
   std::cerr<<"track_source_color_absent spans="<<absent<<" blended="<<absentBlended
       <<" of="<<trackBindings.size()<<"\n";
   std::cerr<<"track_authored_aa1 course="<<track.code()<<" spans="<<authoredAa1Spans
       <<" of="<<trackBindings.size()<<"\n";
  }
  const auto* cableBank=playerUsesRosterRig?racerCables[0]->bank():cableOwner->bank();
  require50DU(cableBank,"selected player cable bank missing");
  const platform::MaterialTextureSampler50EB cableSampler{
      platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
      platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat};
  auto cableTextureRequest=textureRequest;
  cableTextureRequest.samplers=&cableSampler;cableTextureRequest.samplerCount=1;
  cableTextureRequest.maximumGpuBytes=64u*64u*4u;
  require50DU(cableGpuTextures.create(cableTextureRequest,*cableBank,textureDiagnostic),
      "actual cable texture upload failed");
  renderRequest.cables.textures=&cableGpuTextures;
  renderRequest.cables.textureIndex=0;
  if(gameplayBinder.loaded()){
   require50DU(gameplayBinder.createGpu(textureRequest,textureDiagnostic),"original Binder GPU bank rejected");
   require50DU(gameplayBinder.bindRequest(renderRequest),"original Binder cold renderer join rejected");
  }
  renderRequest.track.textures=&trackGpuTextures;
  renderRequest.track.spans=trackBindings.data();
  renderRequest.track.spanCount=static_cast<std::uint32_t>(trackBindings.size());
  renderRequest.track.mode=platform::VehicleWindowMode50DS::callerMaterialTextures;
  // One source RGB scale for every course world (GS 0x80 = 1.0).
  renderRequest.track.vertexColorScale=trackSession->originalStaticCandidates?255.f/128.f:2.f;
  std::vector<platform::VehicleWindowTrackSpan50DS> skyBindings;
  // 274AE0: a track that authors no sky cube publishes no sky (data, A9-style).
  if(track.counts.skyCubes&&skyOwner){
  const auto* skyBank=skyOwner->bank();
  const auto* skyMesh=skyOwner->mesh();
  const auto* skySpans=skyOwner->spans();
  require50DU(skyBank&&skyMesh&&skySpans,"actual sky publication missing");
  std::vector<platform::MaterialTextureSampler50EB> skySamplers(skyBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::clampToEdge,platform::MaterialTextureWrap50EB::clampToEdge});
  auto skyTextureRequest=textureRequest;
  skyTextureRequest.samplers=skySamplers.data();
  skyTextureRequest.samplerCount=static_cast<std::uint32_t>(skySamplers.size());
  // The sky bank's own texture count (one rule for every course).
  skyTextureRequest.maximumGpuBytes=static_cast<std::uint32_t>(skyBank->textureCount())*128u*128u*4u;
  require50DU(skyGpuTextures.create(skyTextureRequest,*skyBank,textureDiagnostic),
      "actual selected-track sky texture upload failed");
  skyBindings.reserve(skySpans->size());
  for(const auto& span:*skySpans){
   platform::VehicleWindowTrackSpan50DS binding{};
   binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.material.mode=platform::VehicleWindowMaterialMode50DS::requiredTexture;
   binding.material.textureIndex=span.textureIndex;
   binding.material.depthWrite=false;binding.material.unlit=true;
   skyBindings.push_back(binding);
  }
  renderRequest.sky.mesh=skyMesh;renderRequest.sky.textures=&skyGpuTextures;
  renderRequest.sky.maximumVertexBytes=192u*sizeof(rr::assets::Vertex48B);
  renderRequest.sky.maximumIndexBytes=288u*sizeof(std::uint32_t);
  renderRequest.sky.spans=skyBindings.data();
  renderRequest.sky.spanCount=static_cast<std::uint32_t>(skyBindings.size());
  // 1299A0 cube sequences of the same sky (data: absent unless the sky authors an .acb).
  if(track.roles.sky)if(auto* sequences=track.roles.sky->sequences()){
   require50DU(sequences->createGpu(textureRequest,textureDiagnostic),"sky cube sequence texture upload failed");
   require50DU(sequences->bindRequest(renderRequest),"sky cube sequence renderer binding rejected");
  }
  }
  std::array<platform::VehicleWindowRacerArchetype50DS,raceOpponentCapacity> racerArchetypes{};
  if(raceEnabled){
   for(std::size_t racer=1;racer<racerBank.count();++racer){
    const auto index=racer-1;const auto& visual=*racerVisuals[racer];
    coldRacerStage50DU(racerRigs[racer]->podId,"material GPU upload",[&](){
     const auto ownSamplers=visual.samplers();
     auto ownRequest=textureRequest;
     ownRequest.samplers=ownSamplers.data();ownRequest.samplerCount=static_cast<std::uint32_t>(ownSamplers.size());
     ownRequest.maximumGpuBytes=8u*1024u*1024u;
     require50DU(opponentGpuTextures[index].create(ownRequest,visual.materials,textureDiagnostic),
         "original opponent texture upload failed");
     const auto& secondBank=visual.canonicalEngineSpecularMaterials50LA();
     if(secondBank.loaded()){
      const auto& secondSamplers=visual.canonicalEngineSpecularSamplers50LA();
      auto secondRequest=textureRequest;
      secondRequest.samplers=secondSamplers.data();
      secondRequest.samplerCount=static_cast<std::uint32_t>(secondSamplers.size());
      secondRequest.maximumGpuBytes=8u*1024u*1024u;
      require50DU(opponentSpecularGpuTextures50LA[index].create(secondRequest,secondBank,textureDiagnostic),
          "canonical opponent context-2 texture upload failed");
     }
    });
    coldRacerStage50DU(racerRigs[racer]->podId,"cable GPU upload",[&](){
     const auto* ownCableBank=racerCables[racer]->bank();
     require50DU(ownCableBank&&opponentCableGpuTextures[index].create(cableTextureRequest,
         *ownCableBank,textureDiagnostic),"original opponent cable texture upload failed");
    });
    coldRacerStage50DU(racerRigs[racer]->podId,"renderer archetype publication",[&](){
     auto& archetype=racerArchetypes[index];archetype.assets=&visual.drawAssets;
     archetype.materials=visual.windowMaterials(&opponentGpuTextures[index],
         visual.canonicalEngineSpecularMaterials50LA().loaded()?&opponentSpecularGpuTextures50LA[index]:nullptr);
     archetype.cables.textures=&opponentCableGpuTextures[index];archetype.cables.textureIndex=0;
    });
   }
   renderRequest.racerArchetypes=raceOpponentCount?racerArchetypes.data():nullptr;renderRequest.racerArchetypeCount=raceOpponentCount;
   // Wreckage pieces of every racer for the destroyed-racer debris pass.
   std::string debrisLog;
   {std::string effectsLog;
    // Wreck particle effects: pod effect files and the selected track's
    // sprite atlas. Optional: a rejected atlas only disables the effects.
    // inherit.particles (146r): a registry course without particle\demoprt.* reads
    // them from the named stock course's archive (members in the archive win).
    const std::string effectsArchive=[&]()->std::string{
     rr::game::dlc50DL::AddonCourseLoad row;
     if(trackSession->registry&&rr::game::dlc50DL::addonCourseLoadByCode(trackSession->world->code,row)&&row.particlesInherited)
      return runtime_asset_paths50LB::resolve("work/native/project/disc/"+row.particleArchive).string();
     return trackSession->archive;
    }();
    if(trackSession->registry)std::cerr<<"course_route_wreck_effects code="<<trackSession->world->code
        <<" source="<<(effectsArchive==trackSession->archive?"own":"inherited")<<'\n';
    const bool effects=racerRigs[0]&&native_wreck_debris50PD::loadEffects(effectsArchive.c_str(),*racerRigs[0],effectsLog)&&
        native_wreck_debris50PD::prepareEffectsGpu(textureRequest,textureDiagnostic);
    std::cerr<<"wreck_effects_cold ready="<<effects<<' '<<effectsLog<<'\n';}
   require50DU(native_wreck_debris50PD::prepareGpu(textureRequest,textureDiagnostic,debrisLog),debrisLog.c_str());
   native_wreck_debris50PD::bindRequest(renderRequest);
   std::cerr<<"wreck_debris_gpu "<<debrisLog<<'\n';
   // Original water surface (A9): one owner for every course whose SCB
   // Environment names water (2745D4); none when it does not (274570).
   {const native_water_runtime50JC::Owner* waterRuntime=track.hasWater()?roles.water.get():nullptr;
    if(waterRuntime&&waterRuntime->resources()){
     std::string waterLog;
     const bool water=native_water_surface50WS::loadCold(trackSession->archive,*waterRuntime->resources(),
         track.identity.code.text.data(),originalAiHumanCount,waterLog)&&
         native_water_surface50WS::prepareGpu(textureRequest,textureDiagnostic);
     native_water_surface50WS::bindRequest(renderRequest);
     std::cerr<<"water_surface_cold ready="<<water<<' '<<waterLog<<'\n';
    }else if(waterRuntime)std::cerr<<"water_surface_cold ready=0 runtime_resources_unbound\n";}
  }
  // The one track-animation window (every course with an owner).
  if(trackAnimations){
   require50DU(trackAnimationWindow.createGpu(textureRequest,gameplayBinderTemplates,originalMainApplication.materialRuntime,textureDiagnostic),
       "track animation dynamic texture upload failed");
   require50DU(trackAnimationWindow.bindRequest(renderRequest),"track animation dynamic renderer publication failed");
  }
  // One ATM owner (05.14), present exactly when the track authors animated
  // textures (B-C08/B-C13: no course term).
  if(roles.atm&&roles.atm->loaded()){
   require50DU(roles.atm->createGpu(textureRequest,textureDiagnostic),"track ATM GPU bank rejected");
   require50DU(roles.atm->bindRequest(renderRequest),"track ATM renderer binding rejected");
  }
  // The particle atlas/material bank of the race's particle children, when
  // the track's particle data loaded it (one rule for every course).
  if(particleMaterials.loaded()){
   require50DU(particleMaterials.createGpu(textureRequest,textureDiagnostic),
       "original particle atlas GPU publication rejected");
   renderRequest.particleCpuTextures=particleMaterials.bank();
   renderRequest.particleTextures=particleMaterials.gpu();
   renderRequest.particleMaterial=particleMaterials.material();
  }
  if(!sourceDisplayLifetime->create(renderRequest,playerDrawAssets,rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);throw std::runtime_error("window vehicle renderer creation failed");
  }
  // One activation point for every course (continuation); this is its
  // phase-guarded no-op re-check after the renderer exists.
  if(!originalUiActive&&race.phase==race_session50HM::Phase::Uninitialized)activateColdRace();
  // Actual cold owner/GPU state after successful renderer creation, every
  // course. No fabricated trigger hits or rendered-frame counts.
  std::cerr<<"course_native_ready course="<<track.code()
      <<" dynamic_ready="<<(trackAnimations&&trackAnimations->ready())
      <<" dynamic_loaded="<<trackAnimationWindow.loaded()<<" dynamic_gpu="<<trackAnimationWindow.ownsResources()
      <<" triggers="<<track.counts.triggers<<" trigger_ready="<<(roles.triggers&&roles.triggers->ready)
      <<" atm_loaded="<<(roles.atm&&roles.atm->loaded())<<" atm_gpu="<<(roles.atm&&roles.atm->ownsResources())
      <<" atm_bindings="<<renderRequest.trackAnimationCount
      <<" water="<<track.hasWater()<<" particles="<<(roles.particles&&roles.particles->prepared())
      <<" binder_enabled="<<gameplayBinderEnabled<<" binder_loaded="<<gameplayBinder.loaded()
      <<" binder_gpu="<<gameplayBinder.ownsResources()
      <<" binder_bound="<<(renderRequest.gameplayBinderTextures!=nullptr)
      <<" camera_policy="<<(originalCameraEnabled?"recovered_original_attached_follow":"host_chase_framePod")
      <<" listener_policy=accepted_camera_xz\n";
  auto publishSourceCameraFog=[&](){
   if(!originalCameraEnabled||!renderRequest.track.worldFog.enabled)return;
   auto fog=renderRequest.track.worldFog;
   fog.start=sourceTrackFog.start34;fog.end=sourceTrackFog.end38;
   require50DU(renderer.updateWorldFog(fog,rendererDiagnostic),"original camera fog publication rejected");
  };
  auto prepareSourceCamera=[&](){
   if(!originalCameraEnabled)return;
   require50DU(!sourceCameraSession&&racerRigs[0]&&nativeOriginalFleet&&
       originalAiHumanCount==(humanCourseEnabled?1u:0u),
       "original camera cold owner requires the actual fresh native roster");
   // New native display extension, initialized once. Existing UI aperture is
   // a separate effect and has finished/retired before this race destination.
   require50DU(sourceCamera::initializeFreshNativeCameraVideoAlpha0(sourceVideo),
       "fresh native video alpha policy rejected");
   // Explicit existing native shader interpretation. These choices are NOT
   // recovered source COLCLAMP/PABE/FBA state or a packet-completion receipt.
   using FadePolicies=platform::NativeVideoPacketOverlayPolicies50LA;
   const FadePolicies fadePolicies{FadePolicies::Rounding::signedFloor128,
       FadePolicies::ColorRange::clamp8,FadePolicies::Pabe::zero,
       FadePolicies::AlphaStore::packedSourceByte};
   sourceFadeOut=std::make_unique<platform::OriginalVideoBlockingNativeBackend>(renderer,host,
       platform::OriginalVideoNativeUse::blockingSourceLoops,fadePolicies);
   sourceFadeIn=std::make_unique<platform::OriginalVideoBlockingNativeBackend>(renderer,host,
       platform::OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0,fadePolicies);
   if(humanCourseEnabled)sourceEndingFade=std::make_unique<platform::OriginalVideoBlockingNativeBackend>(renderer,host,
       platform::OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0,fadePolicies);
   // One camera asset load for every course: the selected pod's CAM and the
   // track's own TOB (track.documents.tob), with the pod's authenticated PHY.
   sourceCameraAssets.load(track,playerPodId,racerRigs[0]->phy);
   require50DU(!sourceCameraAssets.cam.empty(),"original camera assets: selected pod CAM missing");
   makeSourceCameraDisplay();
   if(humanOriginalHud||sourceConsumedPauseActivation){
    const auto domain=originalMainApplication.standardMaterialDomain();
    const auto collector=domain->coldCollector14D440();
    const auto provider=domain->archiveProvider3E1A70();
    const auto revision=provider->revision();
    const bool staged=originalMainApplication.stagePositionStatsResources50LA(
        originalMainApplication.assetRoot,sourcePositionStatsResources);
    if(!staged)std::cerr<<"original_position_stats_stage_rejected message="
        <<sourcePositionStatsResources.failure<<" source_pc="<<sourcePositionStatsResources.diagnostic.sourcePC
        <<" material_pc="<<sourcePositionStatsResources.diagnostic.material.sourcePC
        <<" material_status="<<static_cast<unsigned>(sourcePositionStatsResources.diagnostic.material.status)<<'\n';
    require50DU(staged,"original PositionStats full resource construction failed");
    require50DU(provider->sealed()&&provider->revision()==revision&&
        domain->materials382B38()->coldCollectorCurrent(collector)&&
        sourcePositionStatsPrestage.verify(sourcePositionStatsPrestageDiagnostic),
        "PositionStats construction changed the retained canonical publication");
    sourcePositionStatsHud=std::make_shared<OriginalPositionStatsHud50LA>();
    const bool initialized=sourcePositionStatsHud->initialize(sourcePositionStatsResources.owner,
        sourceTimeStats.pixels,sourcePositionStatsResources.diagnostic);
    if(!initialized)std::cerr<<"original_position_stats_adapter_rejected source_pc="
        <<sourcePositionStatsResources.diagnostic.sourcePC<<" material_pc="
        <<sourcePositionStatsResources.diagnostic.material.sourcePC<<'\n';
    require50DU(initialized,"original PositionStats native adapter rejected");
    sourcePositionStatsRendering=renderer.bindOriginalPositionStats(sourcePositionStatsHud,rendererDiagnostic);
    if(!sourcePositionStatsRendering)reportRenderer50DU(rendererDiagnostic);
    require50DU(sourcePositionStatsRendering,"original PositionStats GPU source join rejected");
    // A race that already loaded Pause100 (the common race load above)
    // shares that storage with the Pause binding below.
    if(!sourcePause.storage()){
     rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
     const bool selected=originalMainApplication.riderProfile&&
         originalMainApplication.riderProfile->courseRaceSnapshot(course);
     if(!selected||!originalMainApplication.loadHostRacePauseCold50LA(sourcePause,sessionIndex+1,
         static_cast<std::int32_t>(course.subtype)))
      std::cerr<<"original_pause_host load_rejected selected="<<selected<<"\n";
    }
   }
   if(humanCourseEnabled){
    OriginalRaceHudDiagnostic50KG hudDiagnostic;
    require50DU(sourceRaceHud.load(
        originalMainApplication.appStart(originalMainApplication.assetRoot),
        original_main_entry50KG::readArchive(originalMainApplication.assetRoot,rr::assets::OriginalUiArchiveId50KG::overlays),
        original_main_entry50KG::readArchive(originalMainApplication.assetRoot,rr::assets::OriginalUiArchiveId50KG::localization),
        originalMainApplication.materialRuntime,hudDiagnostic),"actual original race HUD resources rejected");
   }
   for(std::size_t slot=0;slot<sourceCameraCables.size();++slot)sourceCameraCables[slot]=racerCables[slot].get();
   prepareIntroAuthoring();
   requireRuntimeNavigation();
   require50DU(bool(roles.lights),"common track: the one lighting owner (05.13) is missing");
   sourceCamera::ActualOwners sourceCameraActual{
       racerBank,s,x.bindings,baseTail,originalAi,lapContinuity.timing,originalAiSession.manager,raceBorrow,
       sessionRandom,sourceAttachedGlobals,applicationClock,controlPresets,nativeOrdinaryLaunch11258,
       dynamicProfile.game34,cameraStartup350,
       *roles.lights,zoneLifecycle,mainMembership,sourceBlur,*sourceFollowQueries,poseHistory,boostState,sourceCameraCycleAudio,
       dynamicRacer60,originalAiHumanCount,humanResultsCountdown,playerEnvironment.held,
       humanCourseEnabled?&sourceRaceHud:nullptr,&native_wreck_debris50PD::cameraView().view,
       trackMaterialRequest.canonicalDomain?originalMainApplication.entities():nullptr,
       trackMaterialRequest.canonicalDomain?trackMaterialRequest.canonicalDomain->materials382B38():nullptr,
       trackMaterialRequest.canonicalDomain?&originalMainApplication.materialDirectory36B970():nullptr,
       trackMaterialRequest.canonicalDomain?&racerRigs:nullptr,
       trackMaterialRequest.canonicalDomain?roles.particles.get():nullptr,
       trackMaterialRequest.canonicalDomain?&selectedRuntimeHeightDomain:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeRightModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeLeftModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeChariotModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeBottomRightModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeBottomLeftModels:nullptr,
       originalMainApplication.animationClock(),&sourceCameraCables,
       // B8: the one navigation map view (radar / start map), every course.
       mapView,
       &startMusicContext,requestStartMusic,nativeRecordActor.get(),
       introAuthoring?liveAudio->groupsForProviders():nullptr,introAuthoring,requestIntroMusicFade};
   // 1A6DC0 -> 1A95B0 -> 174DF0/174E58 -> 174A30 (+174FA4 when the cut count
   // World+3E4 is nonzero; 174EA8 zeroes it from launch data): the ONE camera
   // factory, for the consumed counted race of every course. Any other launch
   // (no counted race) takes the ordinary camera session.
   const auto& countedRace=consumedStage.race();
   const auto* world74=countedRace?countedRace->world74():nullptr;
   if(countedRace&&world74&&trackMaterialRequest.canonicalDomain){
    sourceCameraSession=originalMainApplication.createNativeCourseCameraSession50LA(sourceCameraActual,*countedRace,*world74,
        std::span<const std::uint8_t>(track.documents.tob.data(),track.documents.tob.size()),mapView,sourceCameraAssets);
    require50DU(bool(sourceCameraSession),"same-Application native course camera custody rejected");
    // 1A95B0 -> 174A30 for EVERY counted race, once per human (race+1B0 low2;
    // zero humans build none). The factory built the controller (and the cut
    // owners 174FA4 when 174E48..174EC4 kept a nonzero cut count); its race
    // slot48 store (1A95DC) is the same with or without cuts.
    if(introAuthoring){
     auto* controller=sourceCameraSession->taControllerEntity174A30();
     require50DU(controller&&nativeIntroRace()&&nativeIntroRace()->retainNativeIntroControllerSlot48(*controller),
         "actual race slot48 intro controller publication rejected");
     require50DU(sourceCameraSession->constructTaIntro177750(
         introPcm,platform::originalTaIntroFirstVoice50HD,platform::originalTaIntroVoices50HD),
         "actual intro audio177750 construction rejected");
    }else if(nativeIntroRace()&&humanCourseEnabled){
     // Zero cuts: 174F9C skips the cut owners 174FA4 and the intro audio
     // entity 177750; 235720 (the Announcer120 group update) runs every race
     // frame with an empty event set.
     auto* controller=sourceCameraSession->taControllerEntity174A30();
     require50DU(controller&&nativeIntroRace()->retainNativeIntroControllerSlot48(*controller),
         "actual counted-race slot48 controller publication rejected");
     auto* groups=liveAudio->groupsForProviders();
     const auto domain=originalMainApplication.standardMaterialDomain();
     const auto& aural=originalMainApplication.sourceAural00Owner_;
     require50DU(groups&&aural&&domain&&groups->beginZeroLineRaceIntro235720(*controller,
         aural->retainedStreamMetadata(),domain->archiveProvider3E1A70(),
         platform::originalTaIntroFirstVoice50HD,platform::originalTaIntroVoices50HD),
         "actual counted-race Announcer120 empty intro transport rejected");
     std::cerr<<"counted_controller course="<<track.code()<<" cuts=0 slot48=1 intro_lines=0\n";
    }
   }else sourceCameraSession=std::make_shared<sourceCamera::Session>(sourceCameraActual);
   // CameraShake trigger enters (1B2780) reach this session's camera through the relay
   // (camera_shake_relay50cs.h); unbound before the session is released (teardown).
   rr::game::cameraShakeRelay50CS.raceReset();
   rr::game::cameraShakeRelay50CS.bind(sourceCameraSession.get(),
       [](void* p,const rr::game::OriginalCameraShakeAction& a)noexcept->bool{
        return static_cast<sourceCamera::Session*>(p)->applyAuthoredShakeEnter(true,a);});
   const auto prepared=sourceCameraSession->prepare(sessionIndex+1,sourceCameraAssets.inputs,*sourceCameraDisplay);
   if(prepared.status!=sourceCamera::Status::complete)std::cerr<<"original_camera_cold_rejected status="
       <<static_cast<unsigned>(prepared.status)<<" source_pc="<<prepared.sourcePC
       <<" visibility_join_status="<<static_cast<unsigned>(prepared.visibilityJoin.status)
       <<" visibility_owner_status="<<static_cast<unsigned>(prepared.visibilityJoin.owner.status)
       <<" visibility_owner_pc="<<prepared.visibilityJoin.owner.sourcePC
       <<" visibility_registration_status="<<static_cast<unsigned>(prepared.visibilityJoin.registration.status)
       <<" visibility_scene_attached="<<prepared.visibilityJoin.sceneAttached
       <<" entity_cold_status="<<static_cast<unsigned>(prepared.entityCold.status)
       <<" entity_cold_pc="<<prepared.entityCold.sourcePC
       <<" follow_query_pc="<<sourceFollowQueries->diagnostic().sourcePC
       <<" follow_query_diagnostic_may_be_unreached="<<sourceFollowQueries->diagnostic().message<<'\n';
   require50DU(prepared.status==sourceCamera::Status::complete,"original camera cold source join rejected");
   // The one particle registry's final accounting, after the camera session
   // took its particle borrows (every course whose track data has particles).
   if(roles.particles)accountParticleRegistry();
   // 175650: the course TV camera from the course's own archive. ONE call for
   // every course, after the session is prepared. A missing/unloadable file
   // leaves no TV camera, as race48+78 = 0, and the Results replay keeps the
   // current view.
   {const auto tvArchive=runtime_startup50fl::readBytes(trackSession->archive);
    const bool tv=sourceCameraSession&&sourceCameraSession->loadReplayTv175650(tvArchive,trackSession->archiveBytes,trackSession->world->code);
    std::cerr<<"original_tv_camera loaded="<<tv<<" error="<<(sourceCameraSession?unsigned(sourceCameraSession->replayTvLoadError().code):0u)<<'\n';}
   // Demand comes from actual retained consumers, not a course-name list: the
   // track's water and particle owners (its data), and a racer body drawn
   // through the single-diffuse map program (for example Darth Vader's pod).
   const bool singleDiffuseRacer=std::any_of(racerVisuals.begin(),racerVisuals.end(),
       [](const auto& visual){return visual&&visual->hasCanonicalSingleDiffuse50LA();});
   if(track.hasWater()||roles.particles||singleDiffuseRacer)
    require50DU(sourceCameraSession&&sourceCameraSession->bindGameplayFogRenderPhase(sourceFogRenderPhase),
        "actual selected render consumer fog phase binding rejected");
   // ALL8 spectator (humans34 == 0, Pause activation from launch data) on the
   // launch's own course: the counted music/native children, the Pause FINAL
   // draw and the consumed-race Pause parent, for every course.
   if(sourceConsumedPauseActivation&&consumedStage.race()){
    // Separate ALL8 native contract on this actual prepared Camera/provider
    // scope. Unavailable preparation acquires no source owner; row9 stays
    // unsupported. Successful native children have the existing actual shared-
    // mix listener consumer. A reached failure retains its owner for cleanup.
    std::uint32_t musicPC=0;
    const auto music=originalMainApplication.stageConsumedRaAll8MusicNative10C(
        *sourceCameraSession,musicPC);
    using Music=original_main_entry50KG::Application::All8MusicStatus;
    if(music==Music::failed){
     std::cerr<<"all8_music_children_failed pc="<<musicPC<<'\n';
     require50DU(false,"actual ALL8 counted music/native children stopped; partial owner retained");
    }
    if(music==Music::unavailable)
     std::cerr<<"all8_music_children_unjoined pc="<<musicPC<<'\n';
   }
   // Pause draw admission pins FINAL: finish only after every real HUD/camera
   // resource retain and population call above has completed.
   if(sourceConsumedPauseActivation&&consumedStage.race()&&!originalMainApplication.finishPauseDrawCold50LA(sourcePause)){
    const auto& d=sourcePause.nativeDiagnostic;
    std::cerr<<"pause_draw_diagnostic pc="<<d.sourcePC<<" reference="<<d.reference<<" pass="<<d.pass
        <<" canonical_status="<<static_cast<unsigned>(d.canonical.status)<<" canonical_pc="<<d.canonical.sourcePC
        <<" canonical_row="<<d.canonical.row<<" canonical_field="<<(d.canonical.field?d.canonical.field:"")
        <<" material_error="<<static_cast<unsigned>(d.material.error)<<" material_pc="<<d.material.sourcePC
        <<" material_field="<<d.material.field<<" texture_code="<<d.texture.code<<" texture_detail="<<d.texture.detail<<'\n';
    require50DU(false,"actual Pause FINAL native draw preparation rejected");
   }
   if(sourceConsumedPauseActivation&&consumedStage.race()){
    // The spectator Pause observes the launch's own Track (R9: bindRace takes
    // the launch's course and profile, no fixed identity). Its title is the
    // launch's own track-ID title (constructor 17EA08 order, as the loader).
    constexpr std::size_t spectatorTitles[]={106,111,115,117,110,114,118,108,107,112,116,109,119};
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection spectatorCourse;
    // A registry (add-on) course has a named title, not a track-ID one: the
    // spectator bind has no named form yet (fix-H.txt, area A).
    rr::game::dlc50DL::AddonCourseLoad spectatorRow;
    const bool spectatorRegistry=trackSession->registry&&rr::game::dlc50DL::addonCourseLoadByCode(trackSession->world->code,spectatorRow);
    const bool spectatorTitled=!spectatorRegistry&&originalMainApplication.riderProfile&&
        originalMainApplication.riderProfile->courseRaceSnapshot(spectatorCourse)&&
        spectatorCourse.course<std::size(spectatorTitles);
    require50DU(spectatorTitled,"ALL8 spectator Pause title: the launch's course has no track-ID title");
    require50DU(sourcePauseObservation.bind(originalMainApplication,sourcePause.storage())&&
        sourcePauseObservation.bindRace(sourceCameraSession,consumedStage.track(),spectatorTitles[spectatorCourse.course]),
        "actual Pause camera/source observation bind rejected");
    require50DU(renderer.bindOriginalPauseDerived(sourcePause.drawing(),rendererDiagnostic),
        "actual Pause GPU binding rejected");
    require50DU(originalMainApplication.attachNativeConsumedRacePause(*consumedStage.race(),sourceCameraSession),
        "actual consumed race/Pause parent attachment rejected");
    sourcePauseParentAttached=true;
   }
   // A human race binds the Pause storage it loaded above to the counted race;
   // the all-AI spectator binds its consumed-race Pause above.
   if(humanOriginalHud&&!sourceConsumedPauseActivation&&sourcePause.storage()&&sourceCameraSession){
    // Constructor17EA08 title IDs in actual track-ID order (as the loader).
    constexpr std::size_t trackTitles[]={106,111,115,117,110,114,118,108,107,112,116,109,119};
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
    // Registry course (146r): the title is the row's name; the profile's stock
    // course field is not this race's course and is never used as an index here.
    rr::game::dlc50DL::AddonCourseLoad titleRow;
    const bool registryTitle=trackSession->registry&&rr::game::dlc50DL::addonCourseLoadByCode(trackSession->world->code,titleRow);
    bool bound=originalMainApplication.riderProfile&&
        originalMainApplication.riderProfile->courseRaceSnapshot(course)&&(registryTitle||course.course<13)&&
        originalMainApplication.finishPauseDrawCold50LA(sourcePause)&&
        renderer.bindOriginalPauseDerived(sourcePause.drawing(),rendererDiagnostic);
    bound=bound&&(registryTitle?
        sourcePauseObservation.bindHostNamed(originalMainApplication,sourcePause.storage(),sourceCameraSession,titleRow.name):
        sourcePauseObservation.bindHost(originalMainApplication,sourcePause.storage(),sourceCameraSession,trackTitles[course.course]));
    if(registryTitle)std::cerr<<"course_route_pause_title code="<<titleRow.code<<" name=\""<<titleRow.name<<"\" stock=0\n";
    std::cerr<<"original_pause_host bound="<<bound<<" course="<<course.course<<" subtype="<<course.subtype<<"\n";
    // 149p: the counted race that owns this SAME Pause100 (every course,
    // add-on courses included) takes the original Pause owner: 1A87A0 Start,
    // 1BE8A0 open, 1BD7C0/1BD8CC rows. Without it the refusal is logged
    // (pause_refused) and Start is ignored; there is no host Pause fallback.
    const bool counted=bound&&sourcePauseObservation.attachCountedRace();
    if(!counted)common_race_log50CR::pauseRefused(0x1A87A0u,"counted race Pause owner not attached");
    std::cerr<<"original_pause_owner counted="<<counted<<" course="<<course.course<<" addon="<<registryTitle
        <<" subtype="<<course.subtype<<" mode34="<<(originalMainApplication.riderProfile?originalMainApplication.riderProfile->stamp().mode34:99u)<<"\n";
   }
   if(sourceSpeedBoost.owner){
    original_speed_boost_adapter50LA::Owner::Diagnostic adapterDiagnostic;
    require50DU(sourceSpeedBoostAdapter.bind(*sourceCameraSession,adapterDiagnostic),
        "actual Speed/Boost racer-owner bind rejected");
    platform::VehicleWindowDiagnostic50DS speedDiagnostic;
    sourceSpeedBoostRendering=renderer.bindOriginalSpeedBoost(sourceSpeedBoost.owner,speedDiagnostic);
    if(!sourceSpeedBoostRendering)std::cerr<<"original_speed_boost_gpu_rejected operation="
        <<(speedDiagnostic.operation?speedDiagnostic.operation:"unknown")<<'\n';
    require50DU(sourceSpeedBoostRendering,"original Speed/Boost GPU source join rejected");
   }
   if(sourceDamage.owner){
    OriginalDamageObservation50LA::Diagnostic observation;
    require50DU(sourceDamageObservation.bind(*sourceCameraSession,sourceDamage.owner,observation),
        "actual ALL8 Damage observation bind rejected");
    platform::VehicleWindowDiagnostic50DS damageDiagnostic;
    sourceDamageRendering=renderer.bindOriginalDamage(sourceDamage.owner,damageDiagnostic);
    if(!sourceDamageRendering)std::cerr<<"original_damage_gpu_rejected operation="
        <<(damageDiagnostic.operation?damageDiagnostic.operation:"unknown")<<'\n';
    require50DU(sourceDamageRendering,"original Damage GPU source join rejected");
   }
   if(sourceTimeStats.owner){
    original_time_stats_adapter50LA::Owner::Diagnostic adapterDiagnostic;
    require50DU(sourceTimeStatsAdapter.bind(*sourceCameraSession,originalAiSession,lapContinuity,
        originalAiSettings1125C,adapterDiagnostic),"actual TimeStats timing-owner bind rejected");
    platform::VehicleWindowDiagnostic50DS timeStatsDiagnostic;
    sourceTimeStatsRendering=renderer.bindOriginalTimeStats(sourceTimeStats.owner,timeStatsDiagnostic);
    if(!sourceTimeStatsRendering)std::cerr<<"original_time_stats_gpu_rejected operation="
        <<(timeStatsDiagnostic.operation?timeStatsDiagnostic.operation:"unknown")<<'\n';
    require50DU(sourceTimeStatsRendering,"original TimeStats GPU source join rejected");
   }
   if(humanCourseEnabled){
    const std::array<const OriginalCameraViewportFields*,2> hudViews{{&sourceCameraSession->viewport(),nullptr}};
    require50DU(sourceRaceHud.construct1A8CC4(1,hudViews,sourceCameraDisplay->rendererWidth4,sourceCameraDisplay->rendererHeight8),
        "actual source HUD TextOverlay/view construction rejected");
    //Additional same-owner source-slot publication only. Legacy or
    //absent lexical registration gets no end-HUD authority; startup unchanged.
    original_race_hud_scope50LA::Registration::observeConstructed1A8CC4(*this);
    platform::VehicleWindowDiagnostic50DS hudRenderDiagnostic;
    sourceRaceHudRendering=renderer.bindOriginalRaceHud(sourceRaceHud,hudRenderDiagnostic);
    require50DU(!hudRenderDiagnostic.cleanupIncomplete,"failed original HUD GPU setup needs renderer retirement");
    if(!sourceRaceHudRendering)std::cerr<<"Original race lap HUD unavailable; native HUD retained: "
        <<(hudRenderDiagnostic.operation?hudRenderDiagnostic.operation:"cold resource join rejected")<<'\n';
    require50DU(racerBank.visit(0,[&](){return lapOwner.bindOriginalHud(sourceRaceHud,0,
        originalAiHumanCount,dynamicConsumerFlags11050);}),"actual human nonnull lap HUD binding rejected");
   }
   // 1AED30 start lock and the 197040 PodRaceStartOverlay load run in the one
   // race start (raceStart.begin(fresh) below, hooks lockStart1AED30 /
   // showStartOverlay197040), not here.

   // The human Results104 root of the counted race, every course (the original
   // has one race class; 1A7DD0..1AB520 read no track id). A rejection is
   // logged; the end dispatch then departs through common_race_end refused.
   if(native_common_race50PD::race(*this)&&humanCourseEnabled&&!allAiEnabled&&originalCameraEnabled&&sourcePause.storage()){
    OriginalResults104Diagnostic50LA rootDiagnostic;
    const bool published=originalMainApplication.publishNativeHumanResultsRoot50LA(*this,rootDiagnostic);
    native_common_results50PD::state().rootPublished=published;
    if(published)bindResultsFeedbackCall();
    std::cerr<<"common_race_results_root published="<<published<<" pc="<<std::hex<<rootDiagnostic.sourcePC<<std::dec
        <<" operation="<<(rootDiagnostic.operation?rootDiagnostic.operation:"")<<'\n';
   }
   installHumanCameraInput50DU();
   std::array<original_all8_frame::RacerBinding,8> cameraRoster{};
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    cameraRoster[slot]={slot,sourceCameraSession->racerKey(slot)};
   require50DU(sourceFrameSchedule.bind(racerBank,s,racerFrameReads,cameraRoster,originalAiHumanCount,
       sourceCameraSession->sessionGeneration()),"original camera ALL8 scheduler binding rejected");
   publishSourceCameraFog();
   std::cerr<<"ORIGINAL CAMERA: source mode4 intro then saved Attached/Follow mode; "
       <<(humanCourseEnabled?"one actual Player; ":"ALL8 original AI; ")
       <<"actual live roster="<<raceRacerCount<<"; "
       <<"native root/world/audio policy, one actual RNG owner, source640x448 projection; selected camera action cycles Attached/Follow presets. "
       <<"track_cuts3E4="<<sourceCameraSession->authoredSceneCutCount3E4()
       <<" controller_cutsA8="<<sourceCameraSession->effectiveControllerCutCountA8()<<'\n';
  };
  if(!originalUiActive)prepareSourceCamera();
  if(originalUiActive)originalMain.activate(renderer);
  // Original 1A77A4: after overlays.res (race HUD) and the race managers.
  rr::platform::loading_progress50DU::step(.1f,0x1a77a4);
  // Renderer owns its cold GPU copy; release CPU-only track staging promptly.
  if(!headlessLive)trackMesh=rr::assets::Mesh48B{}; // Capture-only VM geometry evidence.
  materialOwner.reset();
  samplers.clear();
  for(auto& bindings:materialBindings)std::vector<platform::VehicleWindowMaterialBinding50DS>{}.swap(bindings);
  VehiclePresentState50DA presenter{};
  platform::FixedStepClock50DT hostClock;
  constexpr std::uint64_t stepNs=16000000,maximumSleepNs=4000000;
  require50DU(hostClock.initialize(monotonicNow50DU(),stepNs,4),"fixed-step host clock setup failed");
  platform::GameHostInput50ER hostInput{};auto& input=hostInput.keys;
  // Root enables this only together with complete selected-original loading.
  // A menu-only merge must never advertise a racer that still loads POD01.
  constexpr bool alternateHumanReady50IF=true;
  platform::HostSessionMenu50IF sessionMenu(startAtMenu,raceEnabled,playerRacer60,
      alternateHumanReady50IF,audioOutputRequested,loadedAudioSettings,selectedAudioSettings,
      loadedTrack,selectedTrack,trackAdmission,originalCameraEnabled,originalUiStarted&&originalCameraEnabled,humanCourseEnabled);
  sessionMenu.setHumanControls(humanControlInput.preset,humanControlInput.twinEngine,usePhysicalGameController,
      humanControlInput.manualResetBound);
  std::uint64_t drawnMenuRevision=0;
  std::int32_t drawnMenuWidth=0,drawnMenuHeight=0;
  bool menuImagePublished=false,suppressResumeKeys=originalMainApplication.retainsPlayerWindow50DU(),forceSceneRender=false;
  bool capturePending=false;int terminalInputError=0;
  windows_capture50LD::Session windowCapture;
  windowCapture.prepare(captureRun,sessionIndex,maximumWidth,maximumHeight,"race");
  std::vector<std::uint8_t> captureBuffer(headlessLive?
      static_cast<std::size_t>(nativeVideo->width)*nativeVideo->height*4u:0u);
  std::vector<std::uint8_t> captureRgbRow(headlessLive?static_cast<std::size_t>(nativeVideo->width)*3u:0u);
  const char* visibilitySetting=std::getenv("RR_CAPTURE_VISIBILITY");
  const bool captureVisibility=visibilitySetting&&std::string(visibilitySetting)=="1";
  platform::OriginalUiControllerNavigation50LA originalUiControllerNavigation,originalUiSecondControllerNavigation;
  platform::OriginalSpectatorMenuInput50LA spectatorMenuInput;
  // Destruction order matters: retire the real fade BEFORE the publication-
  // local producer drops its event/record arrays, then outer closeWindow may
  // revoke Scope/Camera/World/Display. Never abandon an in-flight GPU borrow.
  const auto cancelPauseInput=[&]()noexcept {
   return originalMainApplication.cancelNativePauseReturn50LA(sourceFadeOut.get());
  };
  struct PauseInputGuard {const decltype(cancelPauseInput)& cancel;
   ~PauseInputGuard()noexcept{if(!cancel())std::terminate();}} pauseInputGuard{cancelPauseInput};
  bool spectatorInputActive=false,spectatorClockResetRequired=true;
  std::uint64_t spectatorUnderlayPublication=0;
  std::uint64_t spectatorAudioLastNs=0,spectatorAudioCarryNs=0;
  std::int32_t spectatorUnderlayWidth=0,spectatorUnderlayHeight=0;

  bool wasPaused=true,oversizeReported=false;
  const char* baselineSetting=std::getenv("RR_TRACK_FPS_BASELINE");
  require50DU(!baselineSetting||std::string(baselineSetting)=="0"||std::string(baselineSetting)=="1",
      "RR_TRACK_FPS_BASELINE must be 0 or 1");
  const bool baselineRequested=baselineSetting&&std::string(baselineSetting)=="1";
  require50DU(!baselineRequested||(!headlessLive&&host.hasWindow()&&raceEnabled&&
      (allAiEnabled||humanCourseEnabled)&&originalAiEnabled&&originalCameraEnabled&&
      racerBank.count()==raceRacerCount&&originalAiHumanCount==(allAiEnabled?0u:1u)&&!autoDriveEnabled),
      "track FPS measurement requires the admitted original fleet and camera window, without a host auto-driver");
  track_fps_baseline50LA::Baseline trackBaseline{baselineRequested};
  BaselineDisplayWall50DU baselineDisplayWall{phaseTiming.enabled&&baselineRequested};
  const auto discardTrackBaseline=[&]() noexcept {
   track_fps_baseline50LA::Transition transition;
   trackBaseline.discard(&transition);
   baselineDisplayWall.consume(transition);
  };
  const auto trackBaselineEligible=[&](){
   const auto& observed=host.info();
   return trackBaseline.enabled()&&!originalUiActive&&!sessionMenu.open()&&!recoveryPaused&&
       race.phase==race_session50HM::Phase::Racing&&!humanResultsReady&&
       !originalAiSession.all8FinishAwaitingDisplay&&!originalAiSession.all8ResultsComplete&&
       sourceCameraSession&&!sourceCameraSession->pendingDisplayWork()&&
       !input.minimized&&!input.eventBudgetExhausted&&
       (input.focused||captureRun.unattended())&&
       observed.drawableWidth>0&&observed.drawableHeight>0&&
       observed.drawableWidth<=maximumWidth&&observed.drawableHeight<=maximumHeight&&
       !autoDriveEnabled&&allAiHostDriverCalls==0&&
       (!allAiEnabled||(allAiControllerSamples==0&&allAiKeyboardPackets==0));
  };
  const auto observeTrackBaseline=[&](const platform::VehicleWindowDiagnostic50DS& display){
   if(!trackBaseline.enabled()||trackBaseline.complete())return;
   if(!trackBaselineEligible()){discardTrackBaseline();return;}
   const auto& observed=host.info();
   track_fps_baseline50LA::Receipt measured{};
   track_fps_baseline50LA::Transition transition;
   const bool accepted=trackBaseline.observe(monotonicNow50DU(),windowSwaps,completedFrames,
       {observed.drawableWidth,observed.drawableHeight,observed.swapInterval,
        input.focused,captureRun.unattended()},measured,&transition);
   baselineDisplayWall.consume(transition,&display);
   if(!accepted)return;
   requireOriginalAiFleetReady();
   const auto elapsed=measured.endNs-measured.beginNs;
   const auto frames=measured.endSwaps-measured.beginSwaps;
   J receipt={{"schema",windows_capture50LD::baselineSchema},{"session",sessionIndex+1},
       {"track",trackSession->world->code},{"track_archive_sha256",trackSession->archiveSha256},
       {"source_revision",nullptr},{"source_identity_scope","correlate external build receipt and executable SHA256"},
       {"mode",measured.settings.unattendedWindow?windows_capture50LD::unattendedMode:windows_capture50LD::focusedMode},
       {"actual_focus",measured.settings.focused},{"drawable_width",measured.settings.width},
       {"drawable_height",measured.settings.height},{"swap_interval",measured.settings.swapInterval},
       {"clock","std::chrono::steady_clock; process-relative nanoseconds"},
       {"interval_begin_ns",measured.beginNs},{"interval_end_ns",measured.endNs},{"elapsed_ns",elapsed},
       {"actual_completed_swap_submissions",frames},{"fps",double(frames)*1e9/double(elapsed)},
       {"begin_window_swaps",measured.beginSwaps},{"end_window_swaps",measured.endSwaps},
       {"begin_native_tick",measured.beginTick},{"end_native_tick",measured.endTick},
       {"warmup_required_ns",track_fps_baseline50LA::warmupNanoseconds},
       {"warmup_excluded_ns",measured.warmupNs},{"warmup_excluded_swap_submissions",measured.warmupSwaps},
       {"measurement_required_ns",track_fps_baseline50LA::measurementNanoseconds},
       {"discarded_intervals",measured.discardedIntervals},
       {"paused_menu_fade_intervals_discarded",true},{"capture_readback_and_disk_intervals_discarded",true},
       {"racer_count",racerBank.count()},{"original_ai_count",originalAiOpponentCount},
       {"human_count",originalAiHumanCount},{"followed_racer_slot",0},{"followed_racer60",playerRacer60},
       {"all_original_ai",allAiEnabled},
       {"race_configuration",humanCourseEnabled?"original_human_and_ai":"original_all_ai"},
       {"input_counter_scope","controller dispatch calls and composed keyboard packets; zero required only for all-AI"},
       {"host_driver_calls",allAiHostDriverCalls},
       {"controller_samples",allAiControllerSamples},{"keyboard_packets",allAiKeyboardPackets},
       {"original_shadows",renderRequest.staticInstanceCount!=0},
       // One track draw path for every course (B-C20).
       {"world_renderer","common_track"},
       {"track_adapter_settings_apply",true},
       {"track_frustum_culling",renderRequest.track.enableFrustumCulling},
       {"authored_world_fog",renderRequest.track.worldFog.enabled},
       {"original_gameplay_blur",renderRequest.enableOriginalGameplayBlur},
       {"logical_audio_enabled",audioEnabled},{"audio_output_requested",audioOutputRequested},
       {"native_step_ns",stepNs},{"maximum_catchup_steps",4},{"msaa_samples_requested",requestedWindowSamples},
       {"includes_existing_host_pacing_and_telemetry",true},
       {"measurement_scope",windows_capture50LD::measurementScope}};
   if(baselineDisplayWall.enabled)
    receipt["display_wall_diagnostics"]=baselineDisplayWall.receipt(measured,sessionIndex+1);
   captureRun.noteBaseline(receipt);
   std::cerr<<"track_fps_baseline "<<receipt.dump()<<'\n';
  };
  std::uint64_t lastCommittedTick=0;
  host_chase_camera50fn_fixture::State cameraFraming{};
  std::uint64_t cameraAdjustedFrames=0,cameraLimitedFrames=0;
  float viewForwardX=0.f,viewForwardZ=1.f; // Host chase view, preserved at vertical attitudes.
  auto reportUiState=[&](const char* reason){
   if(originalUiActive){
    originalMain.report(reason,originalMainApplication,completedFrames,liveAudio->mixedFrames());return;
   }
   // Evidence from the actual immutable bank owner and clocks, not a synthetic
   // pause counter. Capture may request this repeatedly without any world step.
   std::cerr<<"host_ui_state {\"reason\":\""<<reason<<"\",\"session\":"<<(sessionIndex+1)
       <<",\"audio_bank_id\":"<<reinterpret_cast<std::uintptr_t>(&liveAudio->bank())
       <<",\"menu\":"<<(sessionMenu.open()?"true":"false")<<",\"page\":"<<unsigned(sessionMenu.page())
       <<",\"loaded_racer\":"<<playerRacer60<<",\"selected_racer\":"<<selectedRacer60
       <<",\"loaded_track\":\""<<platform::hostTrackCode50IM(loadedTrack)
       <<"\",\"selected_track\":\""<<platform::hostTrackCode50IM(selectedTrack)<<'"'
       <<",\"track_pending\":"<<(sessionMenu.trackChangesPending()?"true":"false")
       // Data terms only (B-C08): the one track's load state, no course list.
       <<",\"loaded_track_incomplete\":"<<(track.loaded()?"false":"true")
       <<",\"results_visible\":"<<(humanResultsReady&&!sessionMenu.open()?"true":"false")
       <<",\"audio_enabled\":"<<(audioOutputRequested?"true":"false")
       <<",\"logical_audio_enabled\":"<<(audioEnabled?"true":"false")
       <<",\"audio_pending\":"<<(sessionMenu.audioChangesPending()?"true":"false")
       <<",\"audio_effective\":["<<loadedAudioSettings.effects11020<<','<<loadedAudioSettings.music11024
       <<','<<loadedAudioSettings.character11028<<','<<loadedAudioSettings.announcer1102C<<']'
       <<",\"audio_requested\":["<<selectedAudioSettings.effects11020<<','<<selectedAudioSettings.music11024
       <<','<<selectedAudioSettings.character11028<<','<<selectedAudioSettings.announcer1102C<<']'
       <<",\"tick\":"<<completedFrames<<",\"mixed_frames\":"<<liveAudio->mixedFrames()<<"}\n";
  };
  auto captureCommitted=[&](){
   // Readback cannot borrow the renderer during an owned fade transaction.
   // Keep the real request pending until retirement; never capture the private
   // unswapped restored-camera underlay as if it were the displayed image.
   if(sourceCameraSession&&(sourceCameraSession->blockingVideoPending()||sourceCameraSession->asyncVideoPending()))return;
   if(!capturePending||(!presenter.hasPublished&&!menuImagePublished&&!originalMain.imagePublished))return;
   require50DU(headlessLive,"capture path not configured");
   phase_timing50GK::Scope captureTiming(phaseTiming,phase_timing50GK::Phase::Capture);
   platform::VehicleWindowDiagnostic50DS readbackDiagnostic{};
   const auto readbackStart=originalMain.stageStart();
   const bool readbackOkay=renderer.readbackRgba(captureBuffer.data(),captureBuffer.size(),readbackDiagnostic);
   if(originalUiActive)originalMain.reportStage("capture_readback",readbackStart,readbackOkay);
   require50DU(readbackOkay,"live frame readback failed");
   const auto fileStart=originalMain.stageStart();
   const auto width=readbackDiagnostic.readbackWidth,height=readbackDiagnostic.readbackHeight;
   require50DU(width>0&&static_cast<std::size_t>(width)*3<=captureRgbRow.size(),"capture RGB row capacity");
   const auto captureFile=std::filesystem::path(capturePath);
   const auto writingFile=originalUiActive?
       std::filesystem::path(captureFile.string()+".pending"):captureFile;
   std::ofstream image(writingFile,std::ios::binary);
   require50DU(bool(image),"capture file open failed");
   image<<"P6\n"<<width<<' '<<height<<"\n255\n";
   for(int row=height;row>0;--row){
    const auto rowOffset=static_cast<std::size_t>(row-1)*width*4;
    for(int column=0;column<width;++column){
     const auto source=rowOffset+static_cast<std::size_t>(column)*4;
     const auto target=static_cast<std::size_t>(column)*3;
     for(unsigned channel=0;channel<3;++channel)captureRgbRow[target+channel]=captureBuffer[source+channel];
    }
    image.write(reinterpret_cast<const char*>(captureRgbRow.data()),static_cast<std::streamsize>(width)*3);
   }
   image.close(); // Publish the complete file before the capture acknowledgment.
   require50DU(bool(image),"capture file write failed");
   if(originalUiActive){
    // Same-directory rename atomically publishes the completed PPM before ACK.
    std::filesystem::rename(writingFile,captureFile);
    originalMain.reportStage("capture_file_publication",fileStart);
    const auto mainAudioInfo=originalMain.audioOutput.info();
    const auto capturedUi=originalMain.coordinator.snapshot();
    J capturedGroup=nullptr;
    std::int32_t capturedSlot=0;
    const bool capturedRider=capturedUi.route.screen==5&&capturedUi.route.page==1;
    const bool capturedCourse=capturedUi.route.screen==6&&capturedUi.route.page==1;
    const bool capturedHall=capturedUi.route.screen==11&&capturedUi.route.page==1;
    const bool capturedTournament=capturedUi.route.screen==9&&capturedUi.route.page==1;
    const bool capturedGarage=capturedUi.route.screen==13&&capturedUi.route.page==1;
    if(capturedRider){
     require50DU(originalMain.coordinator.riderSession(),"original Rider capture session missing");
     capturedSlot=capturedUi.rider.slot4C8;
    }else if(!capturedGarage){
     const auto* capturedNavigation=originalMain.navigation();
     require50DU(capturedNavigation,"original UI capture navigation missing");
     const auto group=capturedNavigation->controllerGroup[0];capturedGroup=group;
     capturedSlot=group>0&&static_cast<std::size_t>(group)<capturedNavigation->groups.size()?
         capturedNavigation->groups[group].selectedSlot:0;
    }
    const auto capturedAperture=capturedUi.route.screen==4?capturedUi.main.aperture:
        capturedRider?capturedUi.rider.aperture:capturedCourse?capturedUi.course.aperture:capturedHall?capturedUi.hall.aperture:capturedTournament?capturedUi.tournament.aperture:capturedGarage?capturedUi.garageAperture:capturedUi.options.aperture;
    J receipt={{"sequence",++originalMain.captureSequence},{"native_frame",originalMain.frameCount},
        {"screen",capturedUi.route.screen},{"page",capturedUi.route.page},{"switches",capturedUi.switches},
        {"requested_screen",capturedGarage?J(nullptr):J(capturedUi.transition.requested.screen)},{"requested_page",capturedGarage?J(nullptr):J(capturedUi.transition.requested.page)},
        {"selected_group",capturedGroup},{"selected_slot",capturedGarage?J(nullptr):J(capturedSlot)},
        {"saved_group",originalMainApplication.gameSettingsProfile->savedGroup6E8},
        {"lifecycle",capturedGarage?capturedUi.garageManagerLifecycle:static_cast<unsigned>(capturedUi.transition.lifecycle)},
        {"phase",capturedGarage?capturedUi.garageManagerTransition:static_cast<unsigned>(capturedUi.transition.phase)},
        {"aperture_state",static_cast<unsigned>(capturedAperture.state)},
        {"aperture_input_enabled",capturedAperture.inputEnabled268},
        {"gpu_publication",capturedGarage?J(nullptr):J(originalMain.coordinator.gpu()->publicationToken())},
        {"accepted_update_sequence",originalMain.lastAudioSequence},
        {"file",captureFile.string()},{"width",width},{"height",height},
        {"audio_mixed_frames",mainAudioInfo.mixedFrames},
        {"audio_source_frame_origin",originalMain.audioOutput.sourceFrameOrigin()},
        {"audio_output_source_mixed_frames",originalMain.audioOutput.outputSourceMixedFrames()},
        {"audio_output_serial",originalMain.audioOutput.retainedOutputSerial()},
        {"audio_captured_frames",mainAudioInfo.capturedFrames},{"audio_rate",mainAudioInfo.sampleRate},
        {"audio_clock_microseconds",mainAudioInfo.simulationMicroseconds},
        {"audio_accepted_nanoseconds",originalMain.audioOutput.acceptedNanoseconds()}};
    if(capturedRider){
     const bool sourceAccepted=originalMain.submittedScreen==5&&originalMain.sourceFrameGeneration!=0&&
         originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
     receipt["selected_rider"]=capturedUi.rider.rider4C4;
     receipt["selection_attached"]=capturedUi.rider.selectionAttached;
     receipt["accepted_source_frame"]=sourceAccepted;
     receipt["source_frame_generation"]=sourceAccepted?originalMain.sourceFrameGeneration:0;
     receipt["source_target_generation"]=sourceAccepted?originalMain.sourceTargetGeneration:0;
     receipt["source_quads"]=sourceAccepted?originalMain.submittedQuads:0;
     receipt["source_draws"]=sourceAccepted?originalMain.submittedDraws:0;
     receipt["source_triangles"]=sourceAccepted?originalMain.submittedTriangles:0;
     receipt["source_coherent_framebuffer_fetch"]=sourceAccepted&&originalMain.submittedCoherentFramebufferFetch;
     receipt["source_texture_barriers"]=sourceAccepted?originalMain.submittedTextureBarriers:0;
    }
    if(capturedGarage){
     // Real Garage is a completed Root target, not the legacy immutable
     // SourceFrame/GPU bank or a copied Options navigation object.
     const bool accepted=capturedUi.frameSubmitted&&capturedUi.garageTargetPublished&&
         capturedUi.garageRootReturned&&capturedUi.garageTimingCommitted&&
         originalMain.submittedScreen==13&&capturedUi.garagePublication!=0&&
         originalMain.submittedGaragePublication==capturedUi.garagePublication;
     receipt["presentation"]="native_source_rgba_target";
     receipt["garage_loaded"]=capturedUi.garageLoaded;
     receipt["selected_stat"]=capturedUi.garageLoaded?J(capturedUi.garageSelectedStat):J(nullptr);
     receipt["source_publication"]=capturedUi.garagePublication;
     receipt["root_returned"]=capturedUi.garageRootReturned;
     receipt["timing_committed"]=capturedUi.garageTimingCommitted;
     receipt["target_published"]=capturedUi.garageTargetPublished;
     receipt["accepted_native_target"]=accepted;
     receipt["aperture_bound"]=capturedAperture.bound;
     receipt["aperture_top"]=capturedAperture.top284;
     receipt["aperture_bottom"]=capturedAperture.bottom28C;
    }
    if(capturedTournament){
     const bool sourceAccepted=capturedUi.frameSubmitted&&originalMain.submittedScreen==9&&originalMain.sourceFrameGeneration!=0&&
         originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
     receipt["career"]=capturedUi.tournament.career;receipt["prompt_mask"]=capturedUi.tournament.promptMask42C;
     receipt["camera_active"]=capturedUi.tournament.cameraActive;receipt["accepted_source_frame"]=sourceAccepted;
     receipt["source_frame_generation"]=sourceAccepted?originalMain.sourceFrameGeneration:0;
     receipt["source_target_generation"]=sourceAccepted?originalMain.sourceTargetGeneration:0;
     receipt["source_quads"]=sourceAccepted?originalMain.submittedQuads:0;
     receipt["source_draws"]=sourceAccepted?originalMain.submittedDraws:0;
     receipt["source_triangles"]=sourceAccepted?originalMain.submittedTriangles:0;
    }
    if(capturedCourse){
     const bool nativeAccepted=originalMain.submittedScreen==6&&capturedUi.frameSubmitted&&
         originalMain.submittedPublication==originalMain.coordinator.gpu()->publicationToken();
     receipt["presentation"]="native_host_rgb";
     receipt["course_slot"]=capturedUi.course.courseSlot14E4;receipt["subtype_slot"]=capturedUi.course.subtypeSlot14F4;
     receipt["laps"]=capturedUi.course.laps14EC;receipt["entrants"]=capturedUi.course.entrants14F0;
     receipt["popup_enabled"]=capturedUi.course.popupEnabled438;receipt["accepted_native_frame"]=nativeAccepted;
     receipt["native_quads"]=nativeAccepted?originalMain.submittedQuads:0;
     receipt["native_draws"]=nativeAccepted?originalMain.submittedDraws:0;
    }
    std::cerr<<"original_main_capture "<<receipt.dump()<<'\n';
    reportUiState("capture");capturePending=false;return;
   }
   std::cerr<<"live capture committed at tick "<<lastCommittedTick
            <<(recoveryPaused?" (retained before undertrack pause)":"")<<'\n';
   std::cerr<<"capture_state wrong_way="<<lastCommittedWrongWay<<" tick="<<lastCommittedTick
       <<" host_menu="<<menuImagePublished<<'\n';
   reportUiState("capture");
   if(!menuImagePublished&&captureFirstWrongWay&&lastCommittedWrongWay)capturedFirstWrongWay=true;
   capturePending=false;
  };
  std::uint64_t lastStatusNs=monotonicNow50DU();
  std::uint64_t lastAllAiStatusNs=lastStatusNs;
  std::uint64_t lastStatusRenderCount=0;
  if(!originalUiActive&&!allAiEnabled){
  std::cerr<<"Native human input: source="<<(usePhysicalGameController?"SDL mapped controller":"native keyboard")
      <<" original_preset="<<humanControlInput.preset<<" twin_engine="<<humanControlInput.twinEngine
      <<" throttle/brake="<<selectedControls.actions.action694<<'/'<<selectedControls.actions.action690
      <<" repair/boost/powerslide="<<repairButtonCode<<'/'<<boostButtonCode<<'/'<<powerslideButtonCode<<'\n';
  if(usePhysicalGameController)std::cerr<<"The selected original button layout is preserved on the actual SDL pad. "
      "Left/right stick Y remain independent in advanced mode; normal mode uses its selected throttle/brake buttons and left X steering. "
      "Pad Start or keyboard Escape opens Pause or resumes from its main page. In Pause: D-pad selects, A confirms, Y backs out/cancels; Restart and Main return use their confirmation menu. Keyboard arrows/Enter/Escape remain available; F5/Backspace retain restart actions. SELECT/Back uses the original manual reset when bound; unjoined look actions stop explicitly.\n";
  else std::cerr<<(humanControlInput.twinEngine?
      "W/S left engine; Up/Down right engine; unequal input steers. ":
      "W throttle; S brake; Left/Right steer; Up/Down feed original left-stick Y. ")
      <<"Space boost; R repair; Left Shift powerslide; T original manual reset when bound. Opposite axis keys center. "
      "The selected original row supplies each action code. Escape opens Pause or resumes from its main page; arrows select, Enter confirms, Escape backs out/cancels; F5 restarts.\n";
  std::cerr<<"16ms host ticks, maximum4 catch-up ticks. Focus loss/minimize pauses.\n"
      <<"Actual selected original colliders="<<terrainOwnerCount<<" zones="<<zoneCount
      <<" navigation map/AI="<<selectedMapCount()<<'/'<<selectedAiCount50MB()<<'\n'
      <<"Repair CF4 "<<s.fields.repairCF4<<" from actual "<<playerPodId
      <<" selected authoring; profile overwrite remains absent.\n";
  if(autoDriveEnabled)std::cerr<<"HOST course-check autopilot enabled; pace="
      <<(courseTuning.authoredSpeedOnly?"authored-limits":"conservative-host")
      <<" speed-limit-to-body-scale="<<courseTuning.speedLimitToBodyScale
      <<" steering-preview-seconds="<<courseTuning.authoredSteeringLookaheadSeconds
      <<" full-throttle-steering="<<courseTuning.fullThrottleSteeringRange
      <<"; not original AI. Manual W/S/Up/Down takes over. No automatic reset.\n";
  if(headlessLive)std::cerr<<"Stdin host commands include capture, checkpoint, restart, pause, resume, menu, and quit. "
      "Menus use down/up Up, Down, Left, Right, Enter, Escape. Audio edits apply on Start/Restart only. "
      "Restart runs the in-place 1AC8E0 race start (no session reload).\n";
  }
  reportUiState(originalUiActive?"bound":"ready");
  if(allAiEnabled){
   requireOriginalAiFleetReady();
   if(!originalUiActive)reportOriginalAiFleet("ready");
  }
  if(originalUiActive){
   std::cerr<<"Original UI controls: Main/Options menu Up/Left/W previous(2), Down/Right/S next(3); "
       "Game Options Up/W previous group(2), Down/S next group(3), Left/Right change value(0/1), R defaults(5). Enter/Space confirm(4), "
       "Controls Left/Right changes preset(0/1), R rotates the actual controller(5), Tab selects Normal/Advanced(7). "
       "Escape/menu back(6; source Main does not handle back). Window Close, quit, or stdin EOF exits. "
       "Game Options Accept requires the live enabled audio owner and commits process settings; Cancel remembers the group without applying local edits. "
       "Rider Confirm opens its prepared destination with original enabled audio or explicit mute. Course uses original selection/unlock rules; Confirm on a supported native destination performs a full cold race load. The accepted pad A or keyboard Confirm chooses driving input unless RR_HUMAN_INPUT_DEVICE explicitly overrides it. "
       "F5/restart and checkpoint have no original UI destination. capture writes the existing live PPM target.\n";
  }
  // Persist genuine completed progression outside the allocation-free world
  // pass, before accepting Restart/Main input. Every operand comes from the
  // same manager and bank, not the display overlay or synthetic finish fields.
  auto commitCompletedTournament=[&](){
   if(!tournamentResultArmed||tournamentResultCommitted||
      (originalAiSession.manager.flags58&3u)!=3u)return;
   // 1AB5BC: behind the common race's original Results, 1875C0/187D80 run
   // inside the acknowledgement (after Results is confirmed), not at the
   // finish. The host-panel fallback keeps committing at the finish.
   {const auto& common=native_common_results50PD::state();
    if(common.rootPublished&&!common.fallback&&!common.acknowledged)return;}
   // 1875C0 walks the manager's own count (count4C = the bank's racer count), not a
   // fixed eight; the rows array is the 1AD520 field limit and is bounds-checked.
   std::array<rr::game::OriginalTournamentResultRacer50KG,8> rows{};
   require50DU(humanCourseEnabled&&originalAiHumanCount==1&&racerBank.count()>0&&racerBank.count()<=rows.size(),
       "tournament result lost its original manager roster");
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    auto& row=rows[slot];row.pod60=dynamicRacer60[slot];row.class58=s.fields.vehicle58;
    row.retirementCD4=lapContinuity.timing.wordCD4;
    row.placeCCC=originalAiSession.rank[slot].placeCCC;
    // Only the sole actual human's kill count is consumed by1875C0.
    // AI rows do not substitute a copied human counter.
    row.killsCD0=slot==0?static_cast<std::int32_t>(playerEvents.countCD0):0;
   });
   rr::game::OriginalTournamentResultReceipt50KG receipt;
   require50DU(originalMainApplication.riderProfile->commitTournamentRaceResult(
       originalAiSession.manager,rows.data(),racerBank.count(),originalAiHumanCount,
       tournamentLaunchSelection,tournamentLaunchStamp,*originalMainApplication.state,receipt),
       "genuine tournament result transaction rejected");
   tournamentResultCommitted=true;tournamentResultArmed=false;
   originalMainApplication.showBetaPersistenceStatus();
   std::cerr<<"tournament_progress_committed course_order="<<receipt.courseOrder
       <<" advanced="<<receipt.careerAdvanced<<" retired="<<receipt.retired
       <<" prize="<<receipt.prize<<" bonus="<<receipt.killBonus<<'\n';
  };
  // ---- One race start (1A6AA0 fresh tail / 1AC8E0 restart) and one end
  // dispatch (1A7DD0) for every counted race (PLAN 2.6/2.7). The port-side
  // owners of the per-step actions live here (W0 round 2, RACE-START STEPS);
  // each hook performs exactly one original step for every racer and reports
  // refusal, which R9 logs. Nothing here names a course or a pod.
  const auto resetRacerTiming216690=[&](std::size_t slot,std::uint32_t mode11258)noexcept->bool{
   // 216690 (from 1AEA70 per racer), whole body:
   //  21669C..2166F4 CC8 = 0, CD8 bits 1/0/2 = 0, CCC = 0, CB0/CB8/CBC/CD0/CD4 = 0;
   //  216708..216724 CB4 = 0 unless App2DC+11258 == 3; 216730..216754 CC0[100] = 0;
   //  216758 217420: +7D0/+7D4/+7D8/+7DC = 0, +D3C/+D38 = 0.
   // PORT STAND-IN: 216760..216780 (+74 = 0, +7C = 0, racer+240 vtable+20) are
   // not ported: no port field of the racer state carries those offsets.
   try{
    if(slot>=racerBank.count()||!racerBank.active(slot)||slot>=std::size(originalAiSession.rank))return false;
    common_lap50CR::Diagnostic lapDiagnostic{};
    // The lap owner's own copy of CC8/CD8 (R4: its 1AEA70 per-racer reset).
    if(laps.prepared()&&!laps.resetRacer(slot,lapDiagnostic)){
     std::cerr<<"race_start_216690 slot="<<slot<<" lap_reset_refused="<<(lapDiagnostic.reason?lapDiagnostic.reason:"unknown")<<'\n';
     return false;
    }
    racerBank.visit(slot,[&](){
     lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
     rr::game::initializePlayerLapRaceFields50GF(lapContinuity.state);              // CC8, CD8 bits 0..2
     s.fields.impactFlagsCD8=static_cast<std::uint8_t>((s.fields.impactFlagsCD8&~7u)|(lapContinuity.state.vehicleCD8&7u));
     rr::game::initializeLapTimingFields50ID(lapContinuity.timing,mode11258);       // CB0..CD4, CC0, CB4 rule
     s.fields.damage7D0=0.f;s.fields.damage7D4=0.f;s.fields.damage7D8=0.f;s.fields.damage7DC=0.f; // 217420
     s.impactSupplement.environmentalD38=0.f;s.impactSupplement.environmentalD3C=0.f;
     // The human's PlayerEventState49A carries the same racer fields (CB0/CB4/CB8 and the
     // kill counter CD0, ++ at 225F34) for the player event child: 216690 resets them too.
     if(s.fields.vehicle58==0u){
      playerEvents.countCD0=0;playerEvents.valueCB0=0.f;playerEvents.valueCB8=0.f;
      if(mode11258!=3u)playerEvents.valueCB4=0.f;
     }
    });
    // CCC (place) and CD4 (retirement) as the AI manager's rank rows mirror them.
    originalAiSession.rank[slot].placeCCC=0;originalAiSession.rank[slot].retirementCD4=0;
    // The original racer has ONE CCC word; the port keeps three host mirrors of it (rank,
    // fields, observed: native_ai_session50iv_fixture.h rank() writes all three together).
    // 216690 stores CCC = 0 once, so all three mirrors take that one store; leaving
    // fields/observed at the previous race's place made every view0 reader that checks
    // fields == rank (PositionStats 191DB0, the 1A7DD0 end dispatch) refuse until the next
    // rank pass (cr15 TA: "original PositionStats display preparation rejected" right after
    // the Pause RESTART). Host arrays, slot < rank capacity checked above.
    if(slot<std::size(originalAiSession.fields))originalAiSession.fields[slot].placeCCC=0;
    if(slot<std::size(originalAiSession.observed))originalAiSession.observed[slot].placeCCC=0;
    return true;
   }catch(...){return false;}
  };
  const auto placeOnGrid2A2880=[&](std::size_t slot,std::uint32_t a1,bool replay1B0bit4)noexcept->bool{
   // 1AEA70 per racer: rank = slot - humans34 for an AI racer (racer+58 == 1),
   // slot + manager38 (the AI count) for a human; 2A2880(raceB8 map, rank) over
   // the one map view (source_grid50IG). 2169C0 stores BC0 = position with Y +
   // racer+7EC (this racer's PHY lift), W = 1.0, and BD0 = the 2A2880 heading;
   // the continuation's racer owners then run 7F4/C84 = 0, virtual78
   // (1A24F0 / 216A30, which grounds the pose with its own reset probe) and
   // the 844 / 3C stores (restartGridPlacementCall). No host grounding here.
   // PORT STAND-IN (restart.txt): the Time Trial branch 1AEC40..1AECE0
   // (11258 == 3: manager+40, the record racer, 2A2880 rank 0 + 2169C0 +
   // virtual78(1)) has no racer in the port's bank; the record actor's own
   // restart is the 1ACB18 hook.
   try{
    if(slot>=racerBank.count()||!racerBank.active(slot))return false;
    bool ok=false;std::uint32_t rank=0;rr::game::UndertrackPlacement50ET start2169C0{};
    racerBank.visit(slot,[&](){
     const auto humans=static_cast<std::size_t>(originalAiHumanCount);
     const bool ai=s.fields.vehicle58==1u;
     if(ai?slot<humans:slot>=humans)return;
     rank=static_cast<std::uint32_t>(ai?slot-humans:slot+(racerBank.count()-humans));
     rr::game::GridCenterResult50AC grid{};
     if(rank>7u||!source_grid50IG::prepare(mapView,rank,grid))return;
     const float lift=x.parameters.visual.verticalLimit7EC; // 2169F8 racer+7EC
     for(unsigned axis=0;axis<3;++axis){
      const float at=std::bit_cast<float>(grid.position[axis]);
      start2169C0.positionBC0[axis]=axis==1?at+lift:at+0.f;     // 2169F0..216A08
      start2169C0.headingBD0[axis]=std::bit_cast<float>(grid.heading[axis]);
     }
     start2169C0.positionBC0[3]=1.f;                                // 216A0C W = 1.0
     start2169C0.headingBD0[3]=std::bit_cast<float>(grid.heading[3]);
     for(float v:start2169C0.positionBC0)if(!std::isfinite(v))return;
     for(float v:start2169C0.headingBD0)if(!std::isfinite(v))return;
     ok=true;
    });
    std::cerr<<"race_start_grid pc=2A2880 slot="<<slot<<" rank="<<rank<<" pose_ok="<<ok;
    if(ok)std::cerr<<" x="<<start2169C0.positionBC0[0]<<" y="<<start2169C0.positionBC0[1]
        <<" z="<<start2169C0.positionBC0[2];
    std::cerr<<'\n';
    if(!ok)return false;
    // Outside the publication's bank visit: the owner visits the slot itself.
    return restartGridPlacementCall(slot,start2169C0,a1,replay1B0bit4);
   }catch(...){return false;}
  };
  const auto lockStart1AED30=[&]()noexcept->bool{
   // 1AED30: every racer's body may only move vertically and pitch/roll until
   // GO: 167A30(racer340, 0,1,0) translation, 167A40(racer340, 1,0,1) rotation.
   try{
    for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
     auto& body=x.parameters.bodies.main.body;
     body.translationEnabled[0]=false;body.translationEnabled[1]=true;body.translationEnabled[2]=false;
     body.rotationEnabled[0]=true;body.rotationEnabled[1]=false;body.rotationEnabled[2]=true;});
    std::cerr<<"race_start_lock 1AED30 racers="<<racerBank.count()<<'\n';
    return true;
   }catch(...){return false;}
  };
  const auto showStartOverlay197040=[&]()noexcept->bool{
   // Fresh: 1A8C40 / 197040 / 1A8C78, the race's PodRaceStartOverlay (raceD4),
   // human and all-AI alike (1A8A40 builds it unconditionally). Restart
   // (1ACDF0): 1983A0(raceD4), age 0.75 and once-flags cleared on that overlay.
   if(!sourceCameraSession)return false;
   if(sourceCameraSession->startOverlay())return sourceCameraSession->resetStartOverlay1983A0();
   try{
    OriginalRaceStartOverlayDiagnostic50HU startDiagnostic;
    const bool startLoaded=sourceCameraSession->loadStartOverlay(
        originalMainApplication.appStart(originalMainApplication.assetRoot),
        original_main_entry50KG::readArchive(originalMainApplication.assetRoot,rr::assets::OriginalUiArchiveId50KG::overlays),
        originalMainApplication.materialRuntime,startDiagnostic);
    if(!startLoaded){
     std::cerr<<"race_start_overlay load_rejected error="<<static_cast<unsigned>(startDiagnostic.error)
         <<" pc="<<std::hex<<startDiagnostic.sourcePC<<" material_pc="<<startDiagnostic.material.sourcePC<<std::dec
         <<" texture="<<startDiagnostic.texture<<'\n';
     return false;
    }
    platform::VehicleWindowDiagnostic50DS startRenderDiagnostic;
    const bool startBound=renderer.bindOriginalRaceStartOverlay(sourceCameraSession->startOverlay(),startRenderDiagnostic);
    std::cerr<<"race_start_overlay gpu_bind="<<startBound<<" operation="
        <<(startRenderDiagnostic.operation?startRenderDiagnostic.operation:"none")
        <<" cleanup_incomplete="<<startRenderDiagnostic.cleanupIncomplete<<'\n';
    return !startRenderDiagnostic.cleanupIncomplete;
   }catch(...){return false;}
  };
  const auto restartRecordActor=[&]()noexcept->bool{
   // 1ACB18 (11258 == 3, race78, race1B0 bit4 clear; R9 checks): B0+40 vtable+18
   // = 141520 on the Time Trial record actor (flags29 bits 0/1 clear, vtable+28).
   return nativeRecordActor&&nativeRecordActor->restart1ACB18();
  };
  // 1AFC70(B4) rows on the native AI session that owns them (race1B0 bit4 clear, 1ACC7C):
  // per racer (count4C = the bank's racer count) 40[i] = 0 and 44[i] = 0, then +54 = 0 and
  // +50 = +4C. flags58 bits 0..4, +30 and +34 are the camera Session's (same manager word).
  // PORT STAND-IN (flagged): 3C[i] virtual74 has no port racer virtual.
  const auto managerRowsFit1AFC70=[&]()noexcept->bool{
   const std::size_t count4C=racerBank.count();
   return originalAiSession.managerBound&&originalAiSession.retired40.bound&&
       count4C<=originalAiSession.retired40.managerRows.size()&&count4C<=originalAiSession.finished44.managerRows.size();
  };
  const auto restartManager1AFC70=[&]()noexcept->bool{
   if(!managerRowsFit1AFC70())return false;
   const std::size_t count4C=racerBank.count();
   for(std::size_t i=0;i<count4C;++i){originalAiSession.retired40.managerRows[i]=0;originalAiSession.finished44.managerRows[i]=0;}
   originalAiSession.finished44.count54=0;
   originalAiSession.retired40.activeCount50=static_cast<decltype(originalAiSession.retired40.activeCount50)>(count4C);
   return true;
  };
  const auto resetCamera=[&](std::uint32_t a1,bool replay1B0bit4)noexcept->bool{
   // 1ACBA0 per human 1AF330 / 1750A0, 1AFC70(B4) and 1ACD44 (area B's Session).
   if(!sourceCameraSession)return false;
   if(!replay1B0bit4&&!restartManager1AFC70()){
    try{std::cerr<<"race_start_camera pc=1AFC70 refused manager rows unavailable\n";}catch(...){}
    return false;}
   if(sourceCameraSession->rebindRacerCameras1ACBA0(a1,replay1B0bit4))return true;
   const auto& d=sourceCameraSession->diagnostic();
   try{std::cerr<<"race_start_camera pc=1ACBA0 refused_pc="<<std::hex<<d.sourcePC<<std::dec<<'\n';}catch(...){}
   return false;
  };
  const auto readBestLapCB4=[&](std::size_t slot,float& bestLapCB4)noexcept->bool{
   try{
    if(slot>=racerBank.count()||!racerBank.active(slot))return false;
    racerBank.visit(slot,[&](){bestLapCB4=lapContinuity.timing.bestLapCB4;});
    return true;
   }catch(...){return false;}
  };
  // The 1AC8E0 preflight (common_race_start50CR::Hooks::preflightRestart1AC8E0): every
  // racer's 216690 / 2A2880 / 2169C0 / 1AEA70 owner tests and the camera Session's 1ACBA0
  // tests, all before any effect and with no store. Poses are computed (pure) and checked.
  const auto preflightRestart1AC8E0=[&](std::uint32_t a1,bool replay1B0bit4,std::uint32_t mode11258,
      std::uint32_t& pc,const char*& why)noexcept->bool{
   (void)mode11258;pc=0;why=nullptr;
   const auto refuse=[&](std::uint32_t at,const char* reason)noexcept{
    pc=at;why=reason;
    try{std::cerr<<"race_start_preflight refused_pc="<<std::hex<<at<<std::dec<<" reason="<<reason<<'\n';}catch(...){}
    return false;};
   try{
    if(!sourceCameraSession)return refuse(0x1acba0u,"camera session absent");
    const std::size_t racers=racerBank.count();
    if(!racers||racers>std::size(originalAiSession.rank))return refuse(0x1aea70u,"race field outside the manager rows");
    if(!replay1B0bit4&&!managerRowsFit1AFC70())return refuse(0x1afc70u,"1AFC70 manager rows unavailable");
    for(std::size_t slot=0;slot<racers;++slot){
     if(!racerBank.active(slot))return refuse(0x216690u,"216690 racer slot inactive");
     bool posed=false;
     racerBank.visit(slot,[&](){
      const auto humans=static_cast<std::size_t>(originalAiHumanCount);
      const bool ai=s.fields.vehicle58==1u;
      if(ai?slot<humans:slot>=humans)return;
      const auto rank=static_cast<std::uint32_t>(ai?slot-humans:slot+(racers-humans));
      rr::game::GridCenterResult50AC grid{};
      if(rank>7u||!source_grid50IG::prepare(mapView,rank,grid))return;
      const float lift=x.parameters.visual.verticalLimit7EC;
      for(unsigned axis=0;axis<3;++axis){
       const float at=std::bit_cast<float>(grid.position[axis]);
       if(!std::isfinite(axis==1?at+lift:at+0.f))return;
      }
      for(unsigned axis=0;axis<4;++axis)if(!std::isfinite(std::bit_cast<float>(grid.heading[axis])))return;
      posed=true;
     });
     if(!posed)return refuse(0x2a2880u,"2A2880 grid pose refused");
     std::uint32_t at=0;const char* reason=nullptr;
     if(!restartGridPreflightCall(slot,at,reason))return refuse(at?at:0x1aea70u,reason?reason:"1AEA70 racer owners unavailable");
    }
    if(const auto at=sourceCameraSession->preflightRebindRacerCameras1ACBA0(a1,replay1B0bit4))
     return refuse(at,"1ACBA0 camera session not ready for the restart");
    return true;
   }catch(...){return refuse(0x1ac8e0u,"1AC8E0 preflight exception");}
  };
  common_race_start50CR::Hooks raceStartHooks{};
  raceStartHooks.resetRacerTiming216690.emplace(resetRacerTiming216690);
  raceStartHooks.placeOnGrid2A2880.emplace(placeOnGrid2A2880);
  raceStartHooks.lockStart1AED30.emplace(lockStart1AED30);
  raceStartHooks.showStartOverlay197040.emplace(showStartOverlay197040);
  raceStartHooks.restartRecordActor.emplace(restartRecordActor);
  raceStartHooks.resetCamera.emplace(resetCamera);
  raceStartHooks.readBestLapCB4.emplace(readBestLapCB4);
  // After a whole 1AC8E0 restart (race1B0 bit4 clear): the fresh race's one neutral setup
  // tick for every racer, through the continuation that owns it (PORT STAND-IN, flagged:
  // see common_race_start50CR::Hooks::restartSetupTick).
  const auto restartSetupTick=[&]()noexcept->bool{return restartSetupTickCall();};
  raceStartHooks.restartSetupTick.emplace(restartSetupTick);
  raceStartHooks.preflightRestart1AC8E0.emplace(preflightRestart1AC8E0);
  struct RaceFlowBinding final {
   common_race_start50CR::Owner& start;common_race_end50CR::Owner& end;bool bound=false;
   ~RaceFlowBinding(){
    // The world hook captures this publication's track owners: it never outlives them.
    native_common_results50PD::state().restartWorld=nullptr;
    if(bound){end.unbind();start.unbind();}}
  } raceFlowBinding{raceStart,raceEnd};
  if(const auto& countedRace=consumedStage.race()){
   require50DU(raceStart.bind(countedRace,roster,laps),"common race start bind rejected");
   common_race_end50CR::Launch endLaunch{};
   endLaunch.subtype11258=nativeOrdinaryLaunch11258;
   endLaunch.p34=dynamicProfile.game34;
   endLaunch.humans34=originalAiHumanCount;
   endLaunch.mode34=originalMainApplication.riderProfile?originalMainApplication.riderProfile->stamp().mode34:0u;
   endLaunch.app2E0=originalMainApplication.useProfile2E0();
   require50DU(raceEnd.bind(countedRace,endLaunch,raceStart),"common race end bind rejected");
   raceFlowBinding.bound=true;
   // 1AC8E0's World74 leaves, reached directly as in the original (1AC900 / 1AC978 /
   // 1AC98C), every course: no dependency on the Results root, race78 or a racing step.
   //  161F60 (World+4A0 particles, when present): the one trackside emitter owner runs
   //   the manager's own 161F60 removal, then puts every placement back to its authored
   //   start state (null = no manager); with it the native wreck effects clear.
   //  Scene::Reset 2702E0 (Scene vtable +0x74): 101D10 ResetAll of every track animation +
   //   101B20 re-stop of every Animation-action target, then trigger manager +0x80 (2DCC70:
   //   every trigger's fired bit 0x8 cleared) over the one trigger group of the track.
   //   The lane route republishes every row's visual/collision membership after the
   //   reset; the static route's next world service does.
   //   PORT STAND-IN (flagged, existing): 27C330 (Scene+0x3D4) and the Particles-action
   //   emitter reset of 2702E0 are not carried; the three leaves run together.
   native_common_results50PD::state().restartWorld=[&](const char*& reason)->bool{
    reason=nullptr;native_wreck_effects50PD::reset();
    if(track.roles.emitters){
     common_track50CR::Diagnostic d;
     if(!track.roles.emitters->restart161F60(d)){static thread_local std::string why;
      why="161F60 track particle emitter restart rejected: "+d.reason;reason=why.c_str();return false;}
    }
    std::size_t rows=0,triggers=0;
    // PORT EXTENSION (N64 data only): 'RSet' reloads the N64 track fresh - every hazard row
    // as authored and the animation rows they changed restored, before 2702E0 re-poses them.
    rr::game::n64HazardRuntime50NH.raceReset();
    n64_track_animation_port50NH::n64TrackAnimationPort50NH.raceReset();
    rr::game::cameraShakeRelay50CS.raceReset();
    if(trackAnimationBinding){track_animation_binding50AN::Diagnostic resetDiagnostic;
     if(!trackAnimationBinding->raceReset(resetDiagnostic)){reason="2702E0 track animation ResetAll (101D10/101B20) rejected";return false;}
     rows=trackAnimations?trackAnimations->rowCount():0;}
    else if(trackAnimations){track_animation_runtime50AN::Diagnostic resetDiagnostic;
     if(!trackAnimations->raceReset(resetDiagnostic)){reason="2702E0 track animation ResetAll (101D10/101B20) rejected";return false;}
     rows=trackAnimations->rowCount();}
    if(track.roles.triggers)triggers=rr::game::raceResetNativeTriggerGroup2DCC70(*track.roles.triggers);
    std::cerr<<"track_animation_race_reset course="<<(trackAnimations?trackAnimations->label():std::string("-"))
        <<" rows="<<rows<<" common_triggers="<<triggers<<" at=1AC8E0\n";
    return true;};
   require50DU(originalMainApplication.bindRaceStart50CR(this->raceStartOwner),"Application race start binding rejected");
   // Fresh race: only the shared tail (1AED30, 197040, race94; the cold load
   // placed every racer, so no 2A2880 here). Every fresh hook is filled: a
   // refusal is a failed load step (the 197040 overlay load), as before.
   common_race_start50CR::Receipt freshReceipt{};
   const common_race_start50CR::Request fresh{common_race_start50CR::Reason::fresh,0u,
       common_race_start50CR::callerFresh1A6AA0,nativeOrdinaryLaunch11258};
   const bool freshStarted=raceStart.begin(fresh,raceStartHooks,freshReceipt);
   require50DU(freshStarted||!sourceCameraSession,
       freshReceipt.reason?freshReceipt.reason:"common race start (fresh 1A6AA0 tail) refused");
  }else if(sourceCameraSession){
   // PORT STAND-IN: a launch with no counted race (direct start) has no
   // race start owner to bind; the same two fresh hooks run directly.
   require50DU(lockStart1AED30(),"1AED30 start lock refused");
   require50DU(showStartOverlay197040(),"original PodRaceStartOverlay 197040 load rejected");
  }
  // Original 1A78DC: last step, after 2AED60 camera fade and 232EC0 flush.
  rr::platform::loading_progress50DU::step(.1f,0x1a78dc);
  rr::platform::loading_progress50DU::finish();
  PlayerLoadingSession50DU::checkStop();
  coldLoadDiagnostics.complete();
  // Opt-in host observations only; each receipt is bounded to this session.
  bool firstMainPollReported=false,firstMainActivityGateReported=false;
  bool secondMainPollReported=false,firstMainInactiveReported=false;
  // Borrow existing locals for the synchronous host loop only. Original
  // resources and final cleanup remain in this runSession scope.
  interactive_loop50DU::Context loopContext50DU{
   .track=track,
   .laps=laps,
   .raceStart=raceStart,
   .raceEnd=raceEnd,
   .raceStartHooks=raceStartHooks,
   .originalUiActive=originalUiActive,
   .courseRaceLaunched=courseRaceLaunched,
   .firstMainPollReported=firstMainPollReported,
   .firstMainActivityGateReported=firstMainActivityGateReported,
   .secondMainPollReported=secondMainPollReported,
   .firstMainInactiveReported=firstMainInactiveReported,
   .phaseTiming=phaseTiming,
   .originalMain=originalMain,
   .host=host,
   .hostInput=hostInput,
   .hostDiagnostic=hostDiagnostic,
   .input=input,
   .captureRun=captureRun,
   .windowCapture=windowCapture,
   .sessionMenu=sessionMenu,
   .sourcePauseParentAttached=sourcePauseParentAttached,
   .usePhysicalGameController=usePhysicalGameController,
   .suppressResumeKeys=suppressResumeKeys,
   .forceSceneRender=forceSceneRender,
   .menuImagePublished=menuImagePublished,
   .capturePending=capturePending,
   .terminalInputError=terminalInputError,
   .maximumWidth=maximumWidth,
   .maximumHeight=maximumHeight,
   .originalUiControllerNavigation=originalUiControllerNavigation,
   .originalUiSecondControllerNavigation=originalUiSecondControllerNavigation,
   .applicationClock=applicationClock,
   .originalMainApplication=originalMainApplication,
   .controlPresets=controlPresets,
   .renderer=renderer,
   .windowSwaps=windowSwaps,
   .submittedDrawFrames=submittedDrawFrames,
   .discardedHostNs=discardedHostNs,
   .courseTargets=courseTargets,
   .rootedCourseRace=rootedCourseRace,
   .courseColdLoad=courseColdLoad,
   .sessionIndex=sessionIndex,
   .completedFrames=completedFrames,
   .sourceCameraSession=sourceCameraSession,
   .sourceConsumedIntroActivation=sourceConsumedIntroActivation,
   .spectatorUnderlayPublication=spectatorUnderlayPublication,
   .spectatorMenuInput=spectatorMenuInput,
   .spectatorClockResetRequired=spectatorClockResetRequired,
   .spectatorInputActive=spectatorInputActive,
   .sourcePauseObservation=sourcePauseObservation,
   .spectatorAudioLastNs=spectatorAudioLastNs,
   .spectatorAudioCarryNs=spectatorAudioCarryNs,
   .hostClock=hostClock,
   .lastStatusNs=lastStatusNs,
   .lastStatusRenderCount=lastStatusRenderCount,
   .displayWallTotals=displayWallTotals,
   .renderDiagnostics=renderDiagnostics,
   .sourceCameraInput=sourceCameraInput,
   .sourceCameraDisplay=sourceCameraDisplay,
   .wasPaused=wasPaused,
   .oversizeReported=oversizeReported,
   .audioEnabled=audioEnabled,
   .liveAudio=liveAudio,
   .audioDiagnostic=audioDiagnostic,
   .spectatorUnderlayWidth=spectatorUnderlayWidth,
   .spectatorUnderlayHeight=spectatorUnderlayHeight,
   .sourcePause=sourcePause,
   .rendererDiagnostic=rendererDiagnostic,
   .completionDiagnostic=completionDiagnostic,
   .allAiEnabled=allAiEnabled,
   .originalAiOpponentCount=originalAiOpponentCount,
   .originalAiHumanCount=originalAiHumanCount,
   .dynamicConsumerFlags11050=dynamicConsumerFlags11050,
   .humanCourseEnabled=humanCourseEnabled,
   .raceMusic=raceMusic,
   .mode1Enabled=mode1Enabled,
   .dynamicContactEnabled=dynamicContactEnabled,
   .sourceEndingFade=sourceEndingFade,
   .sourceFadeIn=sourceFadeIn,
   .sourceFadeOut=sourceFadeOut,
   .suspendedCameraFrame=suspendedCameraFrame,
   .originalCameraEnabled=originalCameraEnabled,
   .returnOriginalMainRequested=returnOriginalMainRequested,
   .humanResultsReady=humanResultsReady,
   .humanResultsActive=humanResultsActive,
   .recoveryPaused=recoveryPaused,
   .checkpointRequested=checkpointRequested,
   .s=s,
   .originalUiStarted=originalUiStarted,
   .humanControlInput=humanControlInput,
   .selectedRacer60=selectedRacer60,
   .selectedAudioSettings=selectedAudioSettings,
   .selectedTrack=selectedTrack,
   .raceEnabled=raceEnabled,
   .navigationProgress=navigationProgress,
   .x=x,
   .liveWorld=liveWorld,
   .ground=ground,
   .groundStack=groundStack,
   .checkpointOutput=checkpointOutput,
   .drawnMenuRevision=drawnMenuRevision,
   .drawnMenuWidth=drawnMenuWidth,
   .drawnMenuHeight=drawnMenuHeight,
   .physicalControllerReceipt=physicalControllerReceipt,
   .autoDriveEnabled=autoDriveEnabled,
   .originalAiSession=originalAiSession,
   .sourceAnnouncer=sourceAnnouncer,
   .sourceCameraTick=sourceCameraTick,
   .renderedCameraPublication=renderedCameraPublication,
   .racerBank=racerBank,
   .race=race,
   .raceAudio=raceAudio,
   .stepNs=stepNs,
   .originalAiEnabled=originalAiEnabled,
   .raceSession=raceSession,
   .commitCompletedTournament=interactive_frame50DU::BorrowedCall<void()>{commitCompletedTournament},
   .trackBaselineEligible=interactive_frame50DU::BorrowedCall<bool()>{trackBaselineEligible},
   .discardTrackBaseline=interactive_frame50DU::BorrowedCall<void() noexcept>{discardTrackBaseline},
   .captureCommitted=interactive_frame50DU::BorrowedCall<void()>{captureCommitted},
   .reportUiState=interactive_frame50DU::BorrowedCall<void(const char*)>{reportUiState},
   .stopFeedbackTransport=interactive_frame50DU::BorrowedCall<void()>{stopFeedbackTransport},
   .finishRaceRoot=interactive_frame50DU::BorrowedCall<bool()>{finishRaceRoot},
   .finishRaceRootWithResults=interactive_frame50DU::BorrowedCall<bool()>{finishRaceRootWithResults},
   .beginRaceRoot=interactive_frame50DU::BorrowedCall<bool()>{beginRaceRoot},
   .publishSourceCameraFog=interactive_frame50DU::BorrowedCall<void()>{publishSourceCameraFog},
   .requireRuntimeNavigation=interactive_frame50DU::BorrowedCall<void()>{requireRuntimeNavigation},
   .advanceWorldAnimations=interactive_frame50DU::BorrowedCall<void()>{advanceWorldAnimations},
   .advanceWater=interactive_frame50DU::BorrowedCall<void()>{advanceWater},
   .advanceTrackParticles=interactive_frame50DU::BorrowedCall<void()>{advanceTrackParticles},
   .refreshAnimatedCandidates=interactive_frame50DU::BorrowedCall<bool()>{refreshAnimatedCandidates},
   .advanceSourceAudioWorld=interactive_frame50DU::BorrowedCall<void(std::uint64_t,bool)>{advanceSourceAudioWorld},
   .updateOpponentAudio=interactive_frame50DU::BorrowedCall<void()>{updateOpponentAudio},
   .sampleControllerCall=interactive_frame50DU::BorrowedCall<void(bool)>{sampleController},
   .composePacketCall=interactive_frame50DU::BorrowedCall<void(bool,const rr::platform::WindowInput50DQ&,std::size_t)>{composePacket},
   .applyCourseInputCall=interactive_frame50DU::BorrowedCall<void(std::size_t)>{applyCourseInput},
   .monotonicNow50DU=&monotonicNow50DU,
   .reportHost50DU=&reportHost50DU,
   .reportRenderer50DU=&reportRenderer50DU,
   .originalAiAudioEnabled=originalAiAudioEnabled,
   .finishDriverAppliedThisFrame=finishDriverAppliedThisFrame,
   .opponentFrame=opponentFrame,
   .dynamicRegistry=dynamicRegistry,
   .collisionNormalMode=collisionNormalMode,
   .mode1=mode1,
   .nativeOriginalFleet=nativeOriginalFleet,
   .lapContinuity=lapContinuity,
   .humanResultsCountdown=humanResultsCountdown,
   .sessionRandom=sessionRandom,
   .speechState=speechState,
   .dynamicEvents=dynamicEvents,
   .dynamicActorResolver=dynamicActorResolver,
   .dynamicEventBindings=dynamicEventBindings,
   .originalAiSettings1125C=originalAiSettings1125C,
   .pendingSpeech=pendingSpeech,
   .originalManualResetGraphicsDue=originalManualResetGraphicsDue,
   .graphicsSequence=graphicsSequence,
   .nativeSelectedLights=nativeSelectedLights,
   .lightDiagnostic=lightDiagnostic,
   .m=m,
   .raceRacerCount=raceRacerCount,
   .nativeCalls=nativeCalls,
   .sourceRaceHud=sourceRaceHud,
   .sourceFrameSchedule=sourceFrameSchedule,
   .sourceSpeedBoost=sourceSpeedBoost,
   .sourceDamage=sourceDamage,
   .sourceTimeStats=sourceTimeStats,
   .finishDriverTicks=finishDriverTicks,
   .finishManualInputTicks=finishManualInputTicks,
   .racerNames=racerNames,
   .racerContactsEnabled=racerContactsEnabled,
   .racerContactOwner=racerContactOwner,
   .racerContactIds=racerContactIds,
   .racerContactMaterials=racerContactMaterials,
   .racerContactTotals=racerContactTotals,
   .observedNativeFinishFlags=observedNativeFinishFlags,
   .speechCounts=speechCounts,
   .mode1Effects=mode1Effects,
   .dynamicPodEffect=dynamicPodEffect,
   .speechCues=speechCues,
   .dynamicMusicProfile=dynamicMusicProfile,
   .dynamicDamageCursor=dynamicDamageCursor,
   .dynamicLapCursor=dynamicLapCursor,
   .announcerRequestsUnjoined=announcerRequestsUnjoined,
   .captureFirstWrongWay=captureFirstWrongWay,
   .capturedFirstWrongWay=capturedFirstWrongWay,
   .presenter=presenter,
   .playerDrawAssets=playerDrawAssets,
   .nativeModelDomain=nativeModelDomain,
   .nativeLightingPolicy=nativeLightingPolicy,
   .drawLights=drawLights,
   .racerVisuals=racerVisuals,
   .nativeLeftModels=nativeLeftModels,
   .nativeRightModels=nativeRightModels,
   .racerRigs=racerRigs,
   .nativeBottomLeftModels=nativeBottomLeftModels,
   .nativeBottomRightModels=nativeBottomRightModels,
   .nativeChariotModels=nativeChariotModels,
   .drawWorkspace=drawWorkspace,
   .sourceFogRenderPhase=sourceFogRenderPhase,
   .dynamicEventCursor=dynamicEventCursor,
   .captureDynamicBeforeFrame=interactive_frame50DU::BorrowedCall<void(std::size_t)>{captureDynamicBeforeFrame},
   .observeOriginalAiFleetCall=interactive_frame50DU::BorrowedCall<void(const original_all8_frame::Schedule*)>{observeOriginalAiFleet},
   .advanceOriginalAiManagerCall=interactive_frame50DU::BorrowedCall<void(float,const original_all8_frame::Schedule*)>{advanceOriginalAiManager},
   .advanceFrame=interactive_frame50DU::BorrowedCall<bool(std::size_t)>{advanceFrame},
   .updateOriginalAiPacking=interactive_frame50DU::BorrowedCall<void()>{updateOriginalAiPacking},
   .requireOriginalAiFleetReady=interactive_frame50DU::BorrowedCall<void()>{requireOriginalAiFleetReady},
   .reportRacerContacts=interactive_frame50DU::BorrowedCall<void()>{reportRacerContacts},
   .sampleRaceField=interactive_frame50DU::BorrowedCall<void()>{sampleRaceField},
   .publishOriginalAiStandings=interactive_frame50DU::BorrowedCall<void()>{publishOriginalAiStandings},
   .dynamicBoundary=interactive_frame50DU::BorrowedCall<void(std::uint32_t,const char*)>{dynamicBoundary},
   .resolveDynamicSpeech=interactive_frame50DU::BorrowedCall<bool(rr::game::RaceActorId50ID,rr::game::DamageSpeechInput50HG&,const rr::game::PodSpeechCounts50HG*&,rr::game::PodSpeechState50HG*&)>{resolveDynamicSpeech},
   .applyDynamicSpeech=interactive_frame50DU::BorrowedCall<bool(rr::game::RaceActorId50ID,const rr::game::AudioCommand50HE&)>{applyDynamicSpeech},
   .speechInput=interactive_frame50DU::BorrowedCall<rr::game::DamageSpeechInput50HG()>{speechInput},
   .beginNativeLightDraw=interactive_frame50DU::BorrowedCall<void(std::uint32_t,rr::game::original_gc_gameplay_lighting_candidate::DrawLease&)>{beginNativeLightDraw},
   .nativeLightFailure=interactive_frame50DU::BorrowedCall<void(const char*,std::uint32_t,const rr::game::original_gc_gameplay_lighting_candidate::DrawLease&)>{nativeLightFailure},
   .projectDynamicActorsForSlot=interactive_frame50DU::BorrowedCall<void(std::size_t)>{projectDynamicActorsForSlot},
   .advanceFramePart=interactive_frame50DU::BorrowedCall<bool(std::size_t,interactive_frame50DU::Part,float,float)>{advanceFramePart},
   .captureDynamicBeforeScheduledPhysics=interactive_frame50DU::BorrowedCall<void(const original_all8_frame::Schedule&,std::size_t)>{captureDynamicBeforeScheduledPhysics},
   .audioListener=audioListener,
   .audioListenerReady=audioListenerReady,
   .boostState=boostState,
   .selectedMapCount=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount},
   .selectedMapNode=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode},
   .baseTail=baseTail,
   .coldFlame=coldFlame,
   .sourceBlur=sourceBlur,
   .racerCables=racerCables,
   .mainContactFrames=mainContactFrames,
   .secondaryContactFrames=secondaryContactFrames,
   .playerRacer60=playerRacer60,
   .terrainOwnerCount=terrainOwnerCount,
   .terrainQueries=terrainQueries,
   .terrainHitFrames=terrainHitFrames,
   .terrainSupportedFrames=terrainSupportedFrames,
   .controller=controller,
   .repairActiveTicks=repairActiveTicks,
   .mainMembership=mainMembership,
   .secondaryMembership=secondaryMembership,
   .mainCrossedPortals=mainCrossedPortals,
   .secondaryCrossedPortals=secondaryCrossedPortals,
   .wrongWayUpdates=wrongWayUpdates,
   .wrongWayActiveTicks=wrongWayActiveTicks,
   .wrongWayReceipt=wrongWayReceipt,
   .lapOwner=lapOwner,
   .navigationMapSequential=navigationMapSequential,
   .navigationAISequential=navigationAISequential,
   .navigationMapScan=navigationMapScan,
   .navigationAIScan=navigationAIScan,
   .engineDamageUpdates=engineDamageUpdates,
   .engineDamageDraws=engineDamageDraws,
   .engineDamageDips=engineDamageDips,
   .engineDamageReturns=engineDamageReturns,
   .engineDamageCompletions=engineDamageCompletions,
   .engineDamage=engineDamage,
   .speechEvents=speechEvents,
   .materialAudio=track.roles.audio,
   .originalAiAudio=originalAiAudio,
   .sourceTrackFog=sourceTrackFog,
   .cameraFraming=cameraFraming,
   .cameraAdjustedFrames=cameraAdjustedFrames,
   .cameraLimitedFrames=cameraLimitedFrames,
   .playerCableBindings=playerCableBindings,
   .cableMesh=cableMesh,
   .cableWorkspace=cableWorkspace,
   .cableDiagnostic=cableDiagnostic,
   .gameplayBinder=gameplayBinder,
   .racerHudNames=racerHudNames,
   .skyOwner=skyOwner,
   .playerFlameUvs=playerFlameUvs,
   .humanOriginalHud=humanOriginalHud,
   .renderRequest=renderRequest,
   .sourceModelRenderGlobals=sourceModelRenderGlobals,
   .sourceRaceHudRendering=sourceRaceHudRendering,
   .sourceSpeedBoostRendering=sourceSpeedBoostRendering,
   .sourceSpeedBoostAdapter=sourceSpeedBoostAdapter,
   .sourceTimeStatsRendering=sourceTimeStatsRendering,
   .sourceTimeStatsAdapter=sourceTimeStatsAdapter,
   .sourceDamageRendering=sourceDamageRendering,
   .sourceDamageObservation=sourceDamageObservation,
   .sourcePositionStatsRendering=sourcePositionStatsRendering,
   .sourcePositionStatsHud=sourcePositionStatsHud,
   .sourcePositionStatsResources=sourcePositionStatsResources,
   .particleRender=particleRender,
   .trackAnimations=trackAnimations,
   .trackAnimationBinding=trackAnimationBinding,
   .trackAnimationWindow=trackAnimationWindow,
   .sourceSkySelection=sourceSkySelection,
   .opponentFlamePrepared=opponentFlamePrepared,
   .opponentDraw=opponentDraw,
   .opponentFlames=opponentFlames,
   .opponentUvs=opponentUvs,
   .raceOpponentCount=raceOpponentCount,
   .sourceApplicationClock=sourceApplicationClock,
   .trackUvAnimation=trackUvAnimation,
   .sourceRaceHudRejectionReported=sourceRaceHudRejectionReported,
   .acceptedRacePresentation=acceptedRacePresentation,
   .headlessLive=headlessLive,
   .captureVisibility=captureVisibility,
   .trackMesh=trackMesh,
   .sourceMainBaseReady=sourceMainBaseReady,
   .observeTrackBaseline=interactive_frame50DU::BorrowedCall<void(const rr::platform::VehicleWindowDiagnostic50DS&)>{observeTrackBaseline},
   .sourceRaceHudDisplayReported=sourceRaceHudDisplayReported,
   .sourceSpeedBoostDisplayReported=sourceSpeedBoostDisplayReported,
   .sourceTimeStatsDisplayReported=sourceTimeStatsDisplayReported,
   .sourcePositionStatsDisplayReported=sourcePositionStatsDisplayReported,
   .sourceDamageDisplayReported=sourceDamageDisplayReported,
   .nativeFireSpanIds=nativeFireSpanIds,
   .nativeFireSpanDrawn=nativeFireSpanDrawn,
   .nativeFireSpanOwner=nativeFireSpanOwner,
   .lastCommittedTick=lastCommittedTick,
   .lastCommittedWrongWay=lastCommittedWrongWay,
   .loadedTrack=loadedTrack,
   .trackBaseline=trackBaseline,
   .lastAllAiStatusNs=lastAllAiStatusNs,
   .reportOriginalAiFleet=interactive_frame50DU::BorrowedCall<void(const char*)>{reportOriginalAiFleet},
   .trackAnimationSteps=trackAnimationSteps,
   .boostButtonCode=boostButtonCode,
   .powerslideButtonCode=powerslideButtonCode,
   .repairButtonCode=repairButtonCode,
   .courseDiagnostic=courseDiagnostic,
   .courseOutput=courseOutput,
   .selectedMapIndex=interactive_frame50DU::BorrowedCall<std::uint8_t(std::uint32_t)>{selectedMapIndex},
   .opponentAudio=opponentAudio,
   .observedNativeTicks=observedNativeTicks,
   .maximumSleepNs=maximumSleepNs,
   .viewForwardX=viewForwardX,
   .viewForwardZ=viewForwardZ
  };
  loopContext50DU.run();
  commitCompletedTournament();
  displayWallTotals.reportAndReset("session_end");
  if(trackBaseline.enabled()&&!trackBaseline.complete())
   std::cerr<<"track_fps_baseline_unavailable: no uninterrupted five-second warmup plus five-second measurement completed\n";
  require50DU(terminalInputError==0,"live input stream failed");
}
} // namespace interactive_publication50DU

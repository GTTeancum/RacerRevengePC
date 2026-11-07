// Original publication scope and its local owners; no root-only hooks/readers.
#include "terrain_interactive_publication50du.h"
#include "original_race_hud_scope50la.h"
#include "terrain_interactive_loop50du.h"
#include "player_loading_session50du.h"
#include "cold_load_diagnostics50du.h"
#include "../../platform/linux/native/material_texture_functions50ee.h"
#include "../../platform/linux/native/original_ta_intro_preload50la.h"
#include "pod_cables50fd_fixture.h"
#include "gc_particle_materials50kg_fixture.h"
#include "mc_camera_assets50mc.h"
#include "mc_particle_cold50mc.h"
#include "mc_shield_render50mc.h"
#include "mc_query50mc.h"
#include "rc_crystal50rc.h"
#include "tb_door_binding50tb.h"
#include "tc_gate_visibility50tc.h"
#include "ra_source_camera_factory50la.h"
#include "ra_native_session50la.h"
#include "native_ta_particle_cold50la_fixture.h"
#include "original_camera_assets50kx.h"
#include "original_consumed_ra_stage50du.h"
#include "original_consumed_ga_stage50du.h"
#include "original_consumed_gc_stage50du.h"
#include "original_consumed_ma_stage50du.h"
#include "original_consumed_sa_stage50du.h"
#include "original_consumed_ta_stage50du.h"
#include "original_ta_camera_issuer_candidate.h"
#include "original_native_ta_root_audio50la.h"
#include "original_position_stats_prestage50la.h"
#include "sc_runtime_ground50la.h"
#include "ga_trigger_actions_fixture.h"
#include "sa_trigger_actions_fixture.h"
#include "ma_trigger_actions_fixture.h"
#include "sc_trigger_actions_fixture.h"
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
    if(!ok)throw std::runtime_error(message);
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
 auto& acceptedRacePresentation=this->acceptedRacePresentation;
 const auto& accountRcParticleRegistry=this->accountRcParticleRegistryCall;
 const auto& accountTbParticleRegistry=this->accountTbParticleRegistryCall;
 auto& activateColdRace=this->activateColdRace;
 auto& actualWorld=this->actualWorld;
 const auto& advanceFrame=this->advanceFrameCall;
 const auto& advanceFramePart=this->advanceFramePartCall;
 const auto& advanceNativeGcWater=this->advanceNativeGcWaterCall;
 const auto& advanceOriginalAiManager=this->advanceOriginalAiManagerCall;
 const auto& advanceRcTrackParticles=this->advanceRcTrackParticlesCall;
 const auto& advanceSourceAudioWorld=this->advanceSourceAudioWorldCall;
 const auto& advanceTbTrackParticles=this->advanceTbTrackParticlesCall;
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
 const auto& beginTaRoot=this->beginTaRootCall;
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
 auto& consumedGaStage=this->consumedGaStage;
 auto& consumedGcStage=this->consumedGcStage;
 auto& consumedMaStage=this->consumedMaStage;
 auto& consumedRaStage=this->consumedRaStage;
 auto& consumedSaStage=this->consumedSaStage;
 auto& consumedTaStage=this->consumedTaStage;
 auto& controlPresets=this->controlPresets;
 auto& controller=this->controller;
 auto& courseColdLoad=this->courseColdLoad;
 auto& courseDiagnostic=this->courseDiagnostic;
 auto& courseOutput=this->courseOutput;
 auto& courseRaceCold=this->courseRaceCold;
 auto& courseRaceLaunched=this->courseRaceLaunched;
 auto& courseTargets=this->courseTargets;
 auto& courseTuning=this->courseTuning;
 auto& destination=this->destination;
 auto& directOriginalHuman=this->directOriginalHuman;
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
 const auto& finishTaRoot=this->finishTaRootCall;
 const auto& finishTaRootWithResults=this->finishTaRootWithResultsCall;
 auto& gaAnimatedWorld=this->gaAnimatedWorld;
 auto& gaAtm=this->gaAtm;
 auto& gaDynamicWindow=this->gaDynamicWindow;
 auto& gaOriginalWorld=this->gaOriginalWorld;
 auto& gaTriggerActions=this->gaTriggerActions;
 auto& gameplayBinder=this->gameplayBinder;
 auto& gameplayBinderEnabled=this->gameplayBinderEnabled;
 auto& gameplayBinderTemplates=this->gameplayBinderTemplates;
 auto& gbAnimationRuntime=this->gbAnimationRuntime;
 auto& gbAtm=this->gbAtm;
 auto& gbColdSources=this->gbColdSources;
 auto& gbDynamicWindow=this->gbDynamicWindow;
 auto& gbFogOwner=this->gbFogOwner;
 auto& gbLightOwner=this->gbLightOwner;
 auto& gbMapView=this->gbMapView;
 auto& gbOriginalWorld=this->gbOriginalWorld;
 auto& gbParticleAttachments=this->gbParticleAttachments;
 auto& gbParticleAuthoring=this->gbParticleAuthoring;
 auto& gbParticleRender=this->gbParticleRender;
 auto& gbParticleRenderDiagnostic=this->gbParticleRenderDiagnostic;
 auto& gbParticleRuntime=this->gbParticleRuntime;
 auto& gbParticleWindow=this->gbParticleWindow;
 auto& gbQueryBinding=this->gbQueryBinding;
 auto& gbRace=this->gbRace;
 const auto& gbRemainingPayloadBytes=this->gbRemainingPayloadBytesCall;
 auto& gbSourceCameraAssets=this->gbSourceCameraAssets;
 auto& gbTrackParticles=this->gbTrackParticles;
 auto& gcAnimatedWorld=this->gcAnimatedWorld;
 auto& gcAnimatedWorldSteps=this->gcAnimatedWorldSteps;
 auto& gcAtm=this->gcAtm;
 auto& gcDrawLights=this->gcDrawLights;
 auto& gcDynamicWindow=this->gcDynamicWindow;
 auto& gcGateAudio=this->gcGateAudio;
 auto& gcGraphicsSequence=this->gcGraphicsSequence;
 auto& gcLightDiagnostic=this->gcLightDiagnostic;
 auto& gcLightOwner=this->gcLightOwner;
 auto& gcNativeGate=this->gcNativeGate;
 auto& gcNativeWorldSelected=this->gcNativeWorldSelected;
 auto& gcOriginalWorld=this->gcOriginalWorld;
 auto& gcParticleMaterials=this->gcParticleMaterials;
 auto& gcParticleRender=this->gcParticleRender;
 auto& gcTriggerActions=this->gcTriggerActions;
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
 auto& lapContinuity=this->lapContinuity;
 auto& lapOwner=this->lapOwner;
 auto& lastCommittedWrongWay=this->lastCommittedWrongWay;
 auto& legacyRace=this->legacyRace;
 auto& liveAudio=this->liveAudio;
 auto& liveWorld=this->liveWorld;
 auto& loadedAudioSettings=this->loadedAudioSettings;
 auto& loadedTrack=this->loadedTrack;
 auto& m=this->m;
 auto& maOriginalDisc=this->maOriginalDisc;
 auto& maOriginalWorld=this->maOriginalWorld;
 auto& maSaAtm=this->maSaAtm;
 auto& maShieldUv=this->maShieldUv;
 auto& maTriggerActions=this->maTriggerActions;
 auto& mainContactFrames=this->mainContactFrames;
 auto& mainCrossedPortals=this->mainCrossedPortals;
 auto& mainMembership=this->mainMembership;
 auto& materialAudio=this->materialAudio;
 auto& materialOwner=this->materialOwner;
 auto& mbAtm=this->mbAtm;
 auto& mbMapView=this->mbMapView;
 auto& mbOriginalWorld=this->mbOriginalWorld;
 auto& mbRace=this->mbRace;
 auto& mbShieldUv=this->mbShieldUv;
 auto& mcAnimationRuntime=this->mcAnimationRuntime;
 auto& mcAtm=this->mcAtm;
 auto& mcColdSources=this->mcColdSources;
 auto& mcDynamicWindow=this->mcDynamicWindow;
 auto& mcFogOwner=this->mcFogOwner;
 auto& mcLightOwner=this->mcLightOwner;
 auto& mcMapView=this->mcMapView;
 auto& mcOriginalWorld=this->mcOriginalWorld;
 auto& mcParticleAttachments=this->mcParticleAttachments;
 auto& mcParticleFrame152Notice=this->mcParticleFrame152Notice;
 auto& mcParticleRender=this->mcParticleRender;
 auto& mcParticleRenderDiagnostic=this->mcParticleRenderDiagnostic;
 auto& mcParticleRuntime=this->mcParticleRuntime;
 auto& mcParticleSource=this->mcParticleSource;
 auto& mcParticleVehicles=this->mcParticleVehicles;
 auto& mcParticleWindow=this->mcParticleWindow;
 auto& mcQueryBinding=this->mcQueryBinding;
 auto& mcRace=this->mcRace;
 const auto& mcRemainingPayloadBytes=this->mcRemainingPayloadBytesCall;
 auto& mcShieldUv=this->mcShieldUv;
 auto& mcSkyOwner=this->mcSkyOwner;
 auto& mcSourceCameraAssets=this->mcSourceCameraAssets;
 auto& mcTrackParticles=this->mcTrackParticles;
 auto& mcWaterOwner=this->mcWaterOwner;
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
 auto& nativeGcWater=this->nativeGcWater;
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
 auto& nativeTaParticleDiagnostic=this->nativeTaParticleDiagnostic;
 auto& nativeTaParticles=this->nativeTaParticles;
 auto& nativeVideo=this->nativeVideo;
 auto& navigationAI=this->navigationAI;
 auto& navigationAIScan=this->navigationAIScan;
 auto& navigationAISequential=this->navigationAISequential;
 auto& navigationMap=this->navigationMap;
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
 auto& raDiagnostic=this->raDiagnostic;
 auto& raSession=this->raSession;
 auto& raSessionDiagnostic=this->raSessionDiagnostic;
 auto& raWorld=this->raWorld;
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
 auto& rcAtm=this->rcAtm;
 auto& rcColdSources=this->rcColdSources;
 auto& rcCollisionWorld=this->rcCollisionWorld;
 auto& rcCrystal=this->rcCrystal;
 auto& rcLightOwner=this->rcLightOwner;
 auto& rcNavigationBorrow=this->rcNavigationBorrow;
 auto& rcOriginalDisc=this->rcOriginalDisc;
 auto& rcOriginalWorld=this->rcOriginalWorld;
 auto& rcParticleAttachmentDiagnostic=this->rcParticleAttachmentDiagnostic;
 auto& rcParticleAttachments=this->rcParticleAttachments;
 auto& rcParticleAuthoring=this->rcParticleAuthoring;
 auto& rcParticleAuthoringDiagnostic=this->rcParticleAuthoringDiagnostic;
 auto& rcParticleRegistryFinalAccounted=this->rcParticleRegistryFinalAccounted;
 auto& rcParticleRegistryPayloadCharged=this->rcParticleRegistryPayloadCharged;
 auto& rcParticleRender=this->rcParticleRender;
 auto& rcParticleRenderDiagnostic=this->rcParticleRenderDiagnostic;
 auto& rcParticleRuntime=this->rcParticleRuntime;
 auto& rcParticleRuntimeDiagnostic=this->rcParticleRuntimeDiagnostic;
 auto& rcParticleWindow=this->rcParticleWindow;
 auto& rcParticleWindowPayloadReservation=this->rcParticleWindowPayloadReservation;
 const auto& rcRemainingPayloadBytes=this->rcRemainingPayloadBytesCall;
 auto& rcTrackParticles=this->rcTrackParticles;
 auto& rcTriggerActions=this->rcTriggerActions;
 auto& rcWorldBorrow=this->rcWorldBorrow;
 auto& recoveryPaused=this->recoveryPaused;
 const auto& refreshAnimatedCandidates=this->refreshAnimatedCandidatesCall;
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
 const auto& requireMbRuntimeNavigation50MB=this->requireMbRuntimeNavigation50MBCall;
 const auto& requireOriginalAiFleetReady=this->requireOriginalAiFleetReadyCall;
 const auto& reserveGbAdditionalPayload=this->reserveGbAdditionalPayloadCall;
 const auto& reserveMcEnvironment=this->reserveMcEnvironmentCall;
 const auto& reserveRcAdditionalPayload=this->reserveRcAdditionalPayloadCall;
 const auto& reserveTbAdditionalPayload=this->reserveTbAdditionalPayloadCall;
 const auto& resolveDynamicSpeech=this->resolveDynamicSpeechCall;
 auto& restartRequested=this->restartRequested;
 auto& returnOriginalMainRequested=this->returnOriginalMainRequested;
 auto& s=this->s;
 auto& saAnimatedWorld=this->saAnimatedWorld;
 auto& saDynamicWindow=this->saDynamicWindow;
 auto& saOriginalDisc=this->saOriginalDisc;
 auto& saOriginalWorld=this->saOriginalWorld;
 auto& saTriggerActions=this->saTriggerActions;
 const auto& sampleController=this->sampleControllerCall;
 const auto& sampleRaceField=this->sampleRaceFieldCall;
 auto& scAtm=this->scAtm;
 auto& scColdSources=this->scColdSources;
 auto& scGround=this->scGround;
 auto& scLavaUv=this->scLavaUv;
 auto& scOriginalDisc=this->scOriginalDisc;
 auto& scOriginalWorld=this->scOriginalWorld;
 auto& scTriggerActions=this->scTriggerActions;
 auto& scZeroSky=this->scZeroSky;
 auto& secondaryContactFrames=this->secondaryContactFrames;
 auto& secondaryCrossedPortals=this->secondaryCrossedPortals;
 auto& secondaryMembership=this->secondaryMembership;
 const auto& selectedAiCount50MB=this->selectedAiCount50MBCall;
 auto& selectedAudioSettings=this->selectedAudioSettings;
 auto& selectedControls=this->selectedControls;
 const auto& selectedMapCount50MB=this->selectedMapCount50MBCall;
 const auto& selectedMapIndex50MB=this->selectedMapIndex50MBCall;
 const auto& selectedMapNode50MB=this->selectedMapNode50MBCall;
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
 auto& sourcePauseTrack=this->sourcePauseTrack;
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
 auto& sourceTrackFog=this->sourceTrackFog;
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
 auto& suspendedTaCameraFrame=this->suspendedTaCameraFrame;
 auto& taIntroAuthoring=this->taIntroAuthoring;
 auto& taIntroPcm=this->taIntroPcm;
 auto& taOriginalDisc=this->taOriginalDisc;
 auto& taOriginalWorld=this->taOriginalWorld;
 auto& tbAiView=this->tbAiView;
 auto& tbAtm=this->tbAtm;
 auto& tbColdSources=this->tbColdSources;
 auto& tbCollisionWorld=this->tbCollisionWorld;
 auto& tbDoorBinding=this->tbDoorBinding;
 auto& tbDoorRuntime=this->tbDoorRuntime;
 auto& tbDynamicWindow=this->tbDynamicWindow;
 auto& tbLightOwner=this->tbLightOwner;
 auto& tbMapView=this->tbMapView;
 auto& tbOriginalDisc=this->tbOriginalDisc;
 auto& tbOriginalWorld=this->tbOriginalWorld;
 auto& tbParticleRegistryFinalAccounted=this->tbParticleRegistryFinalAccounted;
 auto& tbPodSound=this->tbPodSound;
 auto& tbRace=this->tbRace;
 const auto& tbRemainingPayloadBytes=this->tbRemainingPayloadBytesCall;
 auto& tbShieldUv=this->tbShieldUv;
 auto& tbSourceCameraAssets=this->tbSourceCameraAssets;
 auto& tbTrackParticles=this->tbTrackParticles;
 auto& tbTriggerActions=this->tbTriggerActions;
 auto& tbWorldBorrow=this->tbWorldBorrow;
 auto& tcAiView=this->tcAiView;
 auto& tcAtm=this->tcAtm;
 auto& tcColdSources=this->tcColdSources;
 auto& tcCollisionWorld=this->tcCollisionWorld;
 auto& tcDynamicWindow=this->tcDynamicWindow;
 auto& tcGateRuntime=this->tcGateRuntime;
 auto& tcGateVisibility=this->tcGateVisibility;
 auto& tcGlobalSound=this->tcGlobalSound;
 auto& tcMapView=this->tcMapView;
 auto& tcOriginalDisc=this->tcOriginalDisc;
 auto& tcOriginalWorld=this->tcOriginalWorld;
 auto& tcRace=this->tcRace;
 auto& tcShieldUv=this->tcShieldUv;
 auto& tcTriggerActions=this->tcTriggerActions;
 auto& tcWorldBorrow=this->tcWorldBorrow;
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
 auto& trackMaterialOwner=this->trackMaterialOwner;
 auto& trackMaterialRequest=this->trackMaterialRequest;
 auto& trackMesh=this->trackMesh;
 auto& trackNormals=this->trackNormals;
 auto& trackSession=this->trackSession;
 auto& trackSource=this->trackSource;
 auto& trackTextureBytes=this->trackTextureBytes;
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
 const auto withRace=[&](auto&& operation)->decltype(auto){
  if(isRcTrack)requireMbRuntimeNavigation50MB();
  if(isMcTrack){requireMbRuntimeNavigation50MB();return operation(mcRace);}
  if(isGbTrack){requireMbRuntimeNavigation50MB();return operation(gbRace);}
  if(isTbTrack){requireMbRuntimeNavigation50MB();return operation(tbRace);}
  if(isTcTrack)return operation(tcRace);
  if(isMbTrack)return operation(mbRace);return operation(legacyRace);
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
  if((isTaTrack||isSaTrack||isRaTrack||directOriginalHuman)&&originalMainApplication.enabled){
   originalMainApplication.appStart(originalMainApplication.assetRoot);
   trackMaterialRequest.canonicalDomain=originalMainApplication.standardMaterialDomain();
   trackMaterialRequest.canonicalContext=nativeVideo->standardMaterialConstructionContext(
       nativeVideo,originalMainApplication);
   trackMaterialRequest.canonicalMaterialPath={&originalMainApplication.materialDirectory36B970()};
  }
  if(humanOriginalHud&&!isRaTrack){
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
   // Original Pause on every track: same cold sources as the RA route. A
   // rejection keeps the host menu (logged), never stops the race.
   if(!originalMainApplication.prewarmPauseCold50LA(sourcePause))
    std::cerr<<"original_pause_host prewarm_rejected\n";
  }
  if(nativeModelDomain&&!isRaTrack){
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
  // Observe the result before inspecting its diagnostic. Function arguments
  // have no left-to-right evaluation guarantee, including on MSVC.
  if(isTbTrack){
   require50DU(tbColdSources&&trackMaterialRequest.canonicalDomain,"TB canonical domain/source missing");
   const auto provider=trackMaterialRequest.canonicalDomain->archiveProvider3E1A70();
   const auto reservation=tbColdSources->canonicalMountReservationBytes();
   reserveTbAdditionalPayload(reservation,"TB canonical mount reservation exceeds remaining payload");
   rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
   require50DU(provider&&tbColdSources->mountCanonicalArchive(*provider,reservation,mounted)&&
       provider->sealNativeMounts(mounted)&&
       trackMaterialRequest.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,trackMaterialDiagnostic.canonical.texture),
       "TB same-source canonical mount/seal/bind rejected");
  }
  if(isRcTrack){
   require50DU(rcColdSources&&trackMaterialRequest.canonicalDomain,"RC canonical domain/source missing");
   const auto provider=trackMaterialRequest.canonicalDomain->archiveProvider3E1A70();
   const auto reservation=rcColdSources->canonicalMountReservationBytes();
   reserveRcAdditionalPayload(reservation,"RC canonical mount reservation exceeds remaining payload");
   rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
   require50DU(provider&&rcColdSources->mountCanonicalArchive(*provider,reservation,mounted)&&
       provider->sealNativeMounts(mounted)&&
       trackMaterialRequest.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,trackMaterialDiagnostic.canonical.texture),
       "RC same-source canonical mount/seal/bind rejected");
  }
  if(originalCameraEnabled&&!isRaTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack&&trackMaterialRequest.canonicalDomain){
   // Existing TA/SA canonical callers also stage particles before population.
   // Authenticate and mount the complete selected archive, never a subset or
   // a guessed negative namespace. GA already retains this exact decoded owner.
   rr::assets::ResArchive48C retainedTrackArchive;
   const auto* archive=tcOriginalWorld?tcOriginalWorld->archive():scOriginalWorld?scOriginalWorld->archive():mbOriginalWorld?mbOriginalWorld->archive():taOriginalWorld?taOriginalWorld->archive():gcOriginalWorld?gcOriginalWorld->archive():gaOriginalWorld?gaOriginalWorld->archive():
       maOriginalWorld?maOriginalWorld->archive():saOriginalWorld?saOriginalWorld->archive():&retainedTrackArchive;
   if(!tcOriginalWorld&&!scOriginalWorld&&!mbOriginalWorld&&!taOriginalWorld&&!gcOriginalWorld&&!gaOriginalWorld&&!maOriginalWorld&&!saOriginalWorld){
    const auto bytes=runtime_startup50fl::readBytes(trackSession->archive);
    rr::assets::DecodeError48C diagnostic;rr::assets::DecodeLimits48C limits;
    limits.resource_file_bytes=trackSession->archiveBytes;
    limits.resource_decoded_bytes=64u*1024u*1024u;limits.resource_entries=4096;
    require50DU(bytes.size()==trackSession->archiveBytes&&
        pod_materials50ed_fixture::detail::sha256(bytes)==trackSession->archiveSha256&&
        rr::assets::decodeResource48C(bytes,retainedTrackArchive,diagnostic,nullptr,limits),
        "canonical particle selected-track archive authentication rejected");
   }
   const auto provider=trackMaterialRequest.canonicalDomain->archiveProvider3E1A70();
   rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
   require50DU(provider&&archive&&
       (taOriginalWorld?taOriginalWorld->mountCanonicalArchive(*provider,mounted):
           provider->mountAuthenticatedRes(trackSession->world->container,trackSession->archiveSha256,trackSession->archiveBytes,
               *archive,mounted))&&provider->sealNativeMounts(mounted)&&
       trackMaterialRequest.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,trackMaterialDiagnostic.canonical.texture),
       "canonical selected-track archive mount rejected");
   require50DU(native_ta_particle_cold50LA::prepare(nativeTaParticles,*trackSession,actualWorld,
       originalMainApplication,trackMaterialRequest.canonicalDomain->materials382B38(),
       sessionIndex+1,nativeTaParticleDiagnostic),"canonical particle source staging rejected");
  }
  if(isTbTrack){
   requireMbRuntimeNavigation50MB();
   require50DU(originalCameraEnabled&&trackMaterialRequest.canonicalDomain&&tbColdSources&&tbColdSources->ready(),
       "TB particle setup requires its actual source and cold Application domain");
   // Incremental root descriptor plus conservative sequential source-copy/hash
   // scratch. Fixed pool/atlas and canonical material storage keep their existing domains.
   reserveTbAdditionalPayload(sizeof(OriginalTrackParticleCold50LA)+4u*(9210u+5022u+132124u),
       "TB canonical particle owner/source staging exceeds remaining payload");
   tbTrackParticles=std::make_unique<OriginalTrackParticleCold50LA>();
   const bool poolPrepared=native_ta_particle_cold50LA::prepareTb(*tbTrackParticles,*trackSession,tbColdSources,
       originalMainApplication,trackMaterialRequest.canonicalDomain->materials382B38(),sessionIndex+1,nativeTaParticleDiagnostic);
   if(!poolPrepared)std::cerr<<"tb_particle_cold_rejected status="<<static_cast<unsigned>(nativeTaParticleDiagnostic.status)
       <<" source_pc="<<nativeTaParticleDiagnostic.sourcePC<<'\n';
   require50DU(poolPrepared,"TB actual canonical particle pool rejected");
   accountTbParticleRegistry();
  }
  if(isRcTrack){
   require50DU(originalCameraEnabled&&trackMaterialRequest.canonicalDomain&&rcColdSources&&rcColdSources->ready()&&
       rcLightOwner&&rcLightOwner->world(),"RC emitter setup requires the actual source, Application and lighting");
   // Reserve all later numerical/Window storage BEFORE the shared registry
   // can grow. Window's conservative declared peak remains charged through
   // close; neither later prepare spends the shared remainder a second time.
   reserveRcAdditionalPayload(rc_particle_render50RC::Owner::retainedPayloadBytes(),
       "RC numerical particle renderer exceeds remaining payload");
   reserveRcAdditionalPayload(rc_particle_visual50RC::Window::maximumAdditionalPayloadBytes(),
       "RC particle Window reservation exceeds remaining payload");
   rcParticleWindowPayloadReservation=rc_particle_visual50RC::Window::maximumAdditionalPayloadBytes();
   rcParticleAuthoring=std::make_shared<rc_particle_authoring50RC::Owner>();
   const bool authored=rcParticleAuthoring->decode(rcColdSources,rcRemainingPayloadBytes(),rcParticleAuthoringDiagnostic);
   require50DU(authored,rcParticleAuthoringDiagnostic.message.empty()?"RC original emitter source rejected":rcParticleAuthoringDiagnostic.message.c_str());
   reserveRcAdditionalPayload(rcParticleAuthoring->retainedPayloadBytes(),"RC retained emitter descriptors exceed remaining payload");
   rcParticleAttachments=std::make_shared<rc_particle_attachments50RC::Owner>();
   const bool attached=rcParticleAttachments->prepare(rcParticleAuthoring,rcRemainingPayloadBytes(),rcParticleAttachmentDiagnostic);
   require50DU(attached,rcParticleAttachmentDiagnostic.message.empty()?"RC actual attachment graphs rejected":rcParticleAttachmentDiagnostic.message.c_str());
   reserveRcAdditionalPayload(rcParticleAttachments->retainedPayloadBytes(),"RC actual attachment graphs exceed remaining payload");
   rcTrackParticles=std::make_shared<OriginalTrackParticleCold50LA>();
   const bool poolPrepared=native_ta_particle_cold50LA::prepareRc(*rcTrackParticles,*trackSession,rcColdSources,
       originalMainApplication,trackMaterialRequest.canonicalDomain->materials382B38(),sessionIndex+1,nativeTaParticleDiagnostic);
   if(!poolPrepared)std::cerr<<"rc_particle_cold_rejected status="<<static_cast<unsigned>(nativeTaParticleDiagnostic.status)
       <<" source_pc="<<nativeTaParticleDiagnostic.sourcePC<<'\n';
   require50DU(poolPrepared,"RC actual canonical particle pool rejected");
   // Fixed pool/atlas remain in their existing particle domain. Charge this
   // shared registry's actual retained capacity exactly once in the RC ledger.
   accountRcParticleRegistry();
   reserveRcAdditionalPayload(rc_particle_runtime50RC::Owner::retainedPayloadBytes(),
       "RC emitter runtime exceeds remaining payload");
   rcParticleRuntime=std::make_shared<rc_particle_runtime50RC::Owner>();
   const bool runtimePrepared=rcParticleRuntime->prepare(rcParticleAuthoring,rcParticleAttachments,rcTrackParticles,
       s.noiseOwner().sharedRandomState,sessionRandom,applicationClock,nativeFireSerial,
       originalMainApplication.animationClock(),rc_particle_runtime50RC::Owner::retainedPayloadBytes()+rcRemainingPayloadBytes(),
       rcParticleRuntimeDiagnostic);
   require50DU(runtimePrepared,rcParticleRuntimeDiagnostic.message.empty()?"RC actual emitter runtime rejected":rcParticleRuntimeDiagnostic.message.c_str());
   const bool activated=rcParticleRuntime->activate(rcParticleRuntimeDiagnostic);
   require50DU(activated,rcParticleRuntimeDiagnostic.message.empty()?"RC actual emitter activation rejected":rcParticleRuntimeDiagnostic.message.c_str());
   accountRcParticleRegistry();
   rcParticleRender=std::make_shared<rc_particle_render50RC::Owner>();
   const bool renderPrepared=rcParticleRender->prepare(rcParticleRuntime,rcLightOwner,
       rc_particle_render50RC::Owner::retainedPayloadBytes(),rcParticleRenderDiagnostic);
   require50DU(renderPrepared,"RC actual numerical particle renderer rejected");
  }
  const bool trackMaterialsLoaded=isRaTrack||(mcOriginalWorld?mcOriginalWorld->populateCanonicalRacers(trackMaterialRequest,mcRemainingPayloadBytes(),trackMaterialDiagnostic):gbOriginalWorld?
      gbOriginalWorld->populateCanonicalRacers(trackMaterialRequest,gbRemainingPayloadBytes(),trackMaterialDiagnostic):tbOriginalWorld?
      tbOriginalWorld->populateCanonicalRacers(trackMaterialRequest,tbRemainingPayloadBytes(),trackMaterialDiagnostic):rcOriginalWorld?
      rcOriginalWorld->populateCanonicalRacers(trackMaterialRequest,rcRemainingPayloadBytes(),trackMaterialDiagnostic):tcOriginalWorld?
      tcOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):scOriginalWorld?
      scOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):mbOriginalWorld?
      mbOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):taOriginalWorld?
      taOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):gcOriginalWorld?
      gcOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):gaOriginalWorld?
      gaOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):maOriginalWorld?
      maOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):saOriginalWorld?
      saOriginalWorld->populateCanonicalRacers(trackMaterialRequest,trackMaterialDiagnostic):
      trackMaterialOwner->load(trackMaterialRequest,trackMesh,trackMaterialDiagnostic));
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
       <<" racer_source_pc="<<failure.racerSource.sourcePC<<" racer_source_member="<<failure.racerSource.member
       <<" racer_source_slot="<<failure.racerSource.modelSlot<<" racer_source_pass="<<failure.racerSource.pass
       <<" racer_source_map="<<failure.racerSource.map
       <<" bank_error="<<static_cast<unsigned>(failure.bank.error)
       <<" bank_texture="<<failure.bank.texture<<" bank_slot="<<failure.bank.slot
       <<" bank_offset="<<failure.bank.byteOffset<<" bank_detail="<<failure.bank.parserMessage<<'\n';
  }
  require50DU(trackMaterialsLoaded,
      trackMaterialDiagnostic.message.empty()?"selected-track material load failed":trackMaterialDiagnostic.message.c_str());
  if(humanOriginalHud&&!isRaTrack){
   require50DU(originalMainApplication.finishTimeStatsNativeCold50LA(sourceTimeStats),"direct human TimeStats final verification rejected");
   require50DU(originalMainApplication.finishDamageNativeCold50LA(sourceDamage),"direct human Damage final verification rejected");
   require50DU(sourcePositionStatsPrestage.verify(sourcePositionStatsPrestageDiagnostic),"direct human PositionStats final verification rejected");
   require50DU(originalMainApplication.finishSpeedBoostNativeCold50LA(sourceSpeedBoost),"direct human Speed/Boost final verification rejected");
  }
  if(allAiEnabled&&originalCameraEnabled&&trackMaterialRequest.canonicalDomain&&!isRaTrack){
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
   // RA resources are already complete through the retained roster owner;
   // its visual consumers still bind these same final canonical references.
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
    racerVisuals[slot]->bindCanonicalEngineModels(engines,*nativeModelDomain,nativeSelectedLights);
    model.bindRendererMaterialFlame(coldFlame);
    leftModel.bindRendererMaterialFlame(coldFlame);
    if(racerRigs[slot]->modelCount==5){
     auto& bottomRightModel=*nativeBottomRightModels[slot];
     bottomRightModel.bindRendererMaterialFlame(coldFlame);
     auto& bottomLeftModel=*nativeBottomLeftModels[slot];
     bottomLeftModel.bindRendererMaterialFlame(coldFlame);
    }
    auto& chariotModel=*nativeChariotModels[slot];
    racerVisuals[slot]->bindCanonicalChariot(*chariotModel.modelScene(),*chariotModel.modelGeometry(),
        *chariotModel.materialReferences(),*nativeModelDomain);
   });
  }
  if(nativeGcWater){
   const auto* spans=taOriginalWorld?taOriginalWorld->spans():gcOriginalWorld?gcOriginalWorld->spans():gaOriginalWorld?gaOriginalWorld->spans():maOriginalWorld?maOriginalWorld->spans():
       saOriginalWorld?saOriginalWorld->spans():trackMaterialOwner->spans();require50DU(spans,"GC source track spans missing");
   std::array<bool,6> matched{};
   for(std::size_t index=0;index<spans->size();++index)for(unsigned fire=0;fire<6;++fire)
    if((*spans)[index].worldInstanceIndex==nativeGcWater->attachmentWorldInstances()[fire]){
     require50DU(index<=std::numeric_limits<std::uint32_t>::max(),"GC native span index overflow");
     nativeFireSpanIds.push_back(static_cast<std::uint32_t>(index));nativeFireSpanOwner.push_back(static_cast<std::uint8_t>(fire));
     matched[fire]=true;
    }
   for(bool actual:matched)require50DU(actual,"GC generator has no actual renderer spans");
   nativeFireSpanDrawn.resize(nativeFireSpanIds.size());
  }
  if(isMaTrack){
   ma_shield_uv_fixture::Diagnostic diagnostic;
   const bool loaded=maOriginalWorld?maShieldUv.load(*maOriginalWorld,diagnostic):
       maShieldUv.load(*trackMaterialOwner,diagnostic);
   require50DU(loaded,
       diagnostic.message.empty()?"MA actual shield UV owner rejected":diagnostic.message.c_str());
  }
  if(isMbTrack){
   mb_shield_uv_fixture::Diagnostic diagnostic;
   require50DU(mbOriginalWorld&&mbShieldUv.load(*mbOriginalWorld,diagnostic),
       diagnostic.message.empty()?"MB actual shield UV owner rejected":diagnostic.message.c_str());
   mb_atm_animation_fixture::Diagnostic animationDiagnostic;
   require50DU(mbAtm.loadSource(trackSession->archive,trackSource,*mbOriginalWorld,animationDiagnostic),
       animationDiagnostic.message.empty()?"MB actual animated textures rejected":animationDiagnostic.message.c_str());
  }
  if(isScTrack){
   sc_lava_uv_fixture::Diagnostic diagnostic;
   const bool uvLoaded=scOriginalWorld&&scLavaUv.load(*scOriginalWorld,diagnostic);
   require50DU(uvLoaded,diagnostic.message.empty()?"SC actual lava UV owner rejected":diagnostic.message.c_str());
   sc_atm_animation_fixture::Diagnostic animationDiagnostic;
   const bool atmLoaded=scAtm.loadSource(trackSession->archive,trackSource,*scOriginalWorld,animationDiagnostic);
   require50DU(atmLoaded,animationDiagnostic.message.empty()?"SC actual animated textures rejected":animationDiagnostic.message.c_str());
  }
  if(isGbTrack){
   require50DU(gbOriginalWorld&&gbParticleRuntime&&gbParticleRuntime->sourceStaged()&&gbParticleAttachments&&
       gbTrackParticles&&gbTrackParticles->prepared()&&gbFogOwner&&gbFogOwner->ready()&&trackMaterialRequest.canonicalDomain,
       "GB actual canonical source, 67 emitters and fog missing before Window preparation");
   // Reserve both fixed owners and their shared-owner control-block policy
   // allowances before any visual/material or numerical storage allocation.
   const auto visualBytes=gb_particle_visual50GB::Window::maximumAdditionalPayloadBytes();
   const auto numericBytes=gb_particle_render50GB::Owner::retainedPayloadBytes();
   reserveGbAdditionalPayload(visualBytes+numericBytes+256u,
       "GB particle Window/numeric/control-block reservation exceeds shared payload");
   gbParticleWindow=std::make_shared<gb_particle_visual50GB::Window>();
   gb_particle_visual50GB::Diagnostic materialDiagnostic;
   const bool materialReady=gbParticleWindow->prepare(gbOriginalWorld,gbParticleAttachments,gbTrackParticles,
       trackMaterialRequest.canonicalDomain,sessionIndex+1,visualBytes,materialDiagnostic);
   if(!materialReady)std::cerr<<"gb_particle_window_rejected message="<<materialDiagnostic.message
       <<" material_status="<<static_cast<unsigned>(materialDiagnostic.material.status)
       <<" material_pc="<<materialDiagnostic.material.sourcePC<<'\n';
   require50DU(materialReady,"GB actual 130-frame canonical particle Window rejected");
   gbParticleRender=std::make_shared<gb_particle_render50GB::Owner>();
   const bool numericReady=gbParticleRender->prepare(gbParticleRuntime,gbLightOwner,gbParticleWindow,gbFogOwner,
       numericBytes,gbParticleRenderDiagnostic);
   if(!numericReady)std::cerr<<"gb_particle_numeric_rejected status="<<static_cast<unsigned>(gbParticleRenderDiagnostic.status)
       <<" source_pc="<<gbParticleRenderDiagnostic.sourcePC<<'\n';
   require50DU(numericReady,"GB actual numeric/material/light/fog destination binding rejected");
  }
  if(isMcTrack){
   require50DU(mcOriginalWorld&&mcParticleRuntime&&mcParticleRuntime->sourceStaged()&&mcParticleAttachments&&
       mcTrackParticles&&mcTrackParticles->prepared()&&mcFogOwner&&mcFogOwner->ready()&&trackMaterialRequest.canonicalDomain,
       "MC actual canonical source, 40 emitters and fog missing before Window preparation");
   // Reserve both fixed owners and their shared-owner control-block policy
   // allowances before any visual/material or numerical storage allocation.
   const auto visualBytes=mc_particle_visual50MC::Window::maximumAdditionalPayloadBytes();
   const auto numericBytes=mc_particle_render50MC::Owner::retainedPayloadBytes();
   reserveMcEnvironment(visualBytes+numericBytes+256u,
       "MC particle Window/numeric/control-block reservation exceeds shared payload");
   mcParticleWindow=std::make_shared<mc_particle_visual50MC::Window>();
   mc_particle_visual50MC::Diagnostic materialDiagnostic;
   const bool materialReady=mcParticleWindow->prepare(mcOriginalWorld,mcParticleAttachments,mcTrackParticles,
       trackMaterialRequest.canonicalDomain,sessionIndex+1,visualBytes,materialDiagnostic);
   if(!materialReady)std::cerr<<"mc_particle_window_rejected message="<<materialDiagnostic.message
       <<" material_status="<<static_cast<unsigned>(materialDiagnostic.material.status)
       <<" material_pc="<<materialDiagnostic.material.sourcePC<<'\n';
   require50DU(materialReady,"MC actual 152-frame canonical particle Window rejected");
   mcParticleRender=std::make_shared<mc_particle_render50MC::Owner>();
   const bool numericReady=mcParticleRender->prepare(mcParticleRuntime,mcLightOwner,mcParticleWindow,mcFogOwner,
       numericBytes,mcParticleRenderDiagnostic);
   if(!numericReady)std::cerr<<"mc_particle_numeric_rejected status="<<static_cast<unsigned>(mcParticleRenderDiagnostic.status)
       <<" source_pc="<<mcParticleRenderDiagnostic.sourcePC<<'\n';
   require50DU(numericReady,"MC actual numeric/material/light/fog destination binding rejected");
   require50DU(mcParticleRender->bindVehicles(mcParticleVehicles),"MC actual retained racer producer/render join rejected");
  }
  if(isRcTrack){
   reserveRcAdditionalPayload(sizeof(rc_crystal50RC::Owner)+rc_crystal50RC::Owner::uvCount*sizeof(std::array<float,2>),"RC crystal bridge and CPU UV scratch exceed remaining payload");
   rcCrystal=std::make_shared<rc_crystal50RC::Owner>();std::string error;
   const bool crystalReady=rcCrystal->prepare(rcOriginalWorld,rcLightOwner,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,sizeof(rc_crystal50RC::Owner),error);
   require50DU(crystalReady,error.empty()?"RC crystal bridge rejected":error.c_str());
   rc_atm_animation_fixture::Diagnostic animationDiagnostic;
   const bool atmLoaded=rcAtm.loadSource(trackSource,rcOriginalWorld,animationDiagnostic);
   require50DU(atmLoaded,animationDiagnostic.message.empty()?"RC actual animated textures rejected":animationDiagnostic.message.c_str());
   rcParticleWindow=std::make_shared<rc_particle_visual50RC::Window>();
   rc_particle_visual50RC::Diagnostic particleDiagnostic;
   const bool particleReady=rcParticleWindow->prepare(rcOriginalWorld,rcParticleAttachments,rcTrackParticles,
       trackMaterialRequest.canonicalDomain,sessionIndex+1,
       rcParticleWindowPayloadReservation,particleDiagnostic);
   require50DU(particleReady,particleDiagnostic.message.empty()?"RC canonical particle Window rejected":particleDiagnostic.message.c_str());
  }
  if(isGbTrack){
   gb_atm_animation_fixture::Diagnostic animationDiagnostic;
   const bool atmLoaded=gbAtm.loadSource(trackSource,gbOriginalWorld,animationDiagnostic);
   require50DU(atmLoaded,animationDiagnostic.message.empty()?"GB actual animated textures rejected":animationDiagnostic.message.c_str());
  }
  if(isMcTrack){
   mc_shield_uv_fixture::Diagnostic uvDiagnostic;
   require50DU(mcShieldUv.load(*mcOriginalWorld,uvDiagnostic),uvDiagnostic.message.c_str());
   mc_atm_animation_fixture::Diagnostic animationDiagnostic;
   const bool atmLoaded=mcAtm.loadSource(trackSource,mcOriginalWorld,animationDiagnostic);
   require50DU(atmLoaded,animationDiagnostic.message.empty()?"MC actual animated textures rejected":animationDiagnostic.message.c_str());
  }
  if(isTbTrack){
   tb_shield_uv_fixture::Diagnostic diagnostic;
   const bool uvLoaded=tbShieldUv.load(tbOriginalWorld,diagnostic);
   require50DU(uvLoaded,diagnostic.message.empty()?"TB actual shield UV owner rejected":diagnostic.message.c_str());
   tb_atm_animation_fixture::Diagnostic animationDiagnostic;
   const bool atmLoaded=tbAtm.loadSource(trackSource,tbOriginalWorld,animationDiagnostic);
   require50DU(atmLoaded,animationDiagnostic.message.empty()?"TB actual animated textures rejected":animationDiagnostic.message.c_str());
  }
  if(isTcTrack){
   tc_shield_uv_fixture::Diagnostic diagnostic;
   const bool uvLoaded=tcOriginalWorld&&tcShieldUv.load(*tcOriginalWorld,diagnostic);
   require50DU(uvLoaded,diagnostic.message.empty()?"TC actual shield UV owner rejected":diagnostic.message.c_str());
   tc_atm_animation_fixture::Diagnostic animationDiagnostic;
   const bool atmLoaded=tcAtm.loadSource(trackSource,*tcOriginalWorld,animationDiagnostic);
   require50DU(atmLoaded,animationDiagnostic.message.empty()?"TC actual animated textures rejected":animationDiagnostic.message.c_str());
  }
  if(gcAnimatedWorld){
   gc_atm_animation_fixture::Diagnostic atmDiagnostic{};
   const bool gcAtmLoaded=gcOriginalWorld?gcAtm.loadSource(trackSession->archive,*gcOriginalWorld,atmDiagnostic):
       gcAtm.loadSource(trackSession->archive,*trackMaterialOwner,atmDiagnostic);
   if(!gcAtmLoaded)std::cerr<<"GC ATM load diagnostic animation="<<atmDiagnostic.animation<<" frame="<<atmDiagnostic.frame
       <<" source_code="<<atmDiagnostic.source.code<<" source_offset="<<atmDiagnostic.source.offset
       <<" source_detail="<<atmDiagnostic.source.detail
       <<" bank_error="<<static_cast<unsigned>(atmDiagnostic.bank.error)
       <<" bank_texture="<<atmDiagnostic.bank.texture<<" bank_slot="<<atmDiagnostic.bank.slot
       <<" bank_offset="<<atmDiagnostic.bank.byteOffset<<" bank_detail="<<atmDiagnostic.bank.parserMessage<<'\n';
   require50DU(gcAtmLoaded,
       atmDiagnostic.message.empty()?"original GC ATM source bank rejected":atmDiagnostic.message.c_str());
  }
  if(isGaTrack){
   ga_atm_animation_fixture::Diagnostic atmDiagnostic{};
   const bool loaded=gaOriginalWorld&&gaAtm.loadSource(trackSession->archive,*gaOriginalWorld,atmDiagnostic);
   if(!loaded)std::cerr<<"GA ATM load diagnostic animation="<<atmDiagnostic.animation<<" frame="<<atmDiagnostic.frame
       <<" source_code="<<atmDiagnostic.source.code<<" source_offset="<<atmDiagnostic.source.offset
       <<" source_detail="<<atmDiagnostic.source.detail<<'\n';
   require50DU(loaded,atmDiagnostic.message.empty()?"original GA ATM source bank rejected":atmDiagnostic.message.c_str());
  }
  if(isMaSaTrack){
   ma_sa_atm_animation_fixture::Diagnostic diagnostic;
   const bool loaded=maOriginalWorld?maSaAtm.loadSource(trackSession->archive,trackSource,*maOriginalWorld,diagnostic):
       saOriginalWorld?maSaAtm.loadSource(trackSession->archive,trackSource,*saOriginalWorld,diagnostic):
       maSaAtm.loadSource(trackSession->archive,trackSource,*trackMaterialOwner,diagnostic);
   require50DU(loaded,diagnostic.message.empty()?"MA/SA actual animated textures rejected":diagnostic.message.c_str());
  }
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
  #if defined(_WIN32)
  constexpr std::int32_t maximumWidth=1280,maximumHeight=896;
#else
  constexpr std::int32_t maximumWidth=1280,maximumHeight=720;
#endif
  rr::platform::loading_progress50DU::pulse("Allocating graphics targets");
  PlayerLoadingSession50DU::checkStop();
  platform::VehicleWindowRequest50DS renderRequest{};
  sourceDisplayLifetime->bindCreationRequest(renderRequest);
  renderRequest.maximumWidth=maximumWidth;renderRequest.maximumHeight=maximumHeight;
  renderRequest.enableHostHud=true;
  renderRequest.enableOriginalGameplayBlur=originalCameraEnabled;
  renderRequest.enableGameplayDisplayWallTiming=phaseTiming.enabled;
  const char* disableNativeShadows=std::getenv("RR_DISABLE_NATIVE_SHADOWS");
  // Native analytic pod-shadow shader currently belongs to the legacy world.
  renderRequest.enableNativePodShadows=!isRaTrack&&!(disableNativeShadows&&std::string(disableNativeShadows)=="1");
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
    if(!target||!courseRaceCold||!originalUiActive||courseRaceLaunched||courseColdLoad.pending()||
       !raceEnabled||!nativeOriginalFleet||!originalAiEnabled||!originalCameraEnabled||
       race.phase!=race_session50HM::Phase::Uninitialized||racerBank.count()!=raceRacerCount||
       !renderer.initialized()||!trackGpuTextures.initialized()||!skyGpuTextures.initialized()||
       !gameplayBinder.loaded()||!gameplayBinder.ownsResources()||!activateColdRace||
       &s.noiseOwner()!=&originalMainApplication.state->sharedNoise308030||
       &sessionRandom!=&originalMainApplication.state->sharedRandomSelectors)return false;
    if(humanCourseEnabled&&!original_course_targets50KX::directHumanTrack(target->track))return false;
    // Match the dedicated frontend: a rooted direct-human target cannot
    // queue an ALL8 replacement, even from this retained embedded branch.
    const bool targetNeedsHuman=target->track==platform::HostTrack50IM::GA||
        (!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty()&&
         original_course_targets50KX::directHumanTrack(target->track));
    if(targetNeedsHuman&&(!humanCourseEnabled||allAiEnabled))return false;
    if(selected.laps!=3&&!humanCourseEnabled)return false;
    // The current world is real and unadvanced. A different target is a
    // queued authenticated cold-load plan; it is never marked world-ready.
    if(isGcTrack&&(!gcNativeWorldSelected||!gcTriggerActions||!gcTriggerActions->ready()||
       !gcGateAudio||!gcGateAudio->available()||gcGateAudio->failed()||
       !nativeGcWater||!nativeGcWater->prepared()||!gcAnimatedWorld||!gcAnimatedWorld->ready()||
       !gcDynamicWindow.loaded()||!gcDynamicWindow.ownsResources()||!gcAtm.loaded()||!gcAtm.ownsResources()||
       !gcParticleMaterials.loaded()||!gcParticleMaterials.ownsResources()))return false;
    if(isRaTrack&&(!raWorld||raWorld->terminalFailure()||!raWorld->coldStaticWorld()||
       !raWorld->skyAssets()||!raSession||!sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(isSaTrack&&(!saAnimatedWorld||!saAnimatedWorld->ready()||!saDynamicWindow.loaded()||
       !saDynamicWindow.ownsResources()||!saTriggerActions||!saTriggerActions->ready()||
       !maSaAtm.loaded()||!maSaAtm.ownsResources()))return false;
    if(isMaTrack&&(!maTriggerActions||!maTriggerActions->ready()||!maShieldUv.loaded()||!maSaAtm.loaded()||
       !maSaAtm.ownsResources()))return false;
    if(isMbTrack&&(!mbOriginalWorld||!mbOriginalWorld->loaded()||!mbShieldUv.loaded()||
       !mbAtm.loaded()||!mbAtm.ownsResources()))return false;
    if(rcOriginalDisc&&(!humanOriginalHud||!rcColdSources||!rcColdSources->ready()||
       !rcCollisionWorld||!rcWorldBorrow||rcCollisionWorld->coldSource()!=rcColdSources.get()||
       rcWorldBorrow->world()!=&liveWorld||!rcNavigationBorrow.valid()||
       !rcNavigationBorrow.sameWorld(liveWorld)||rcNavigationBorrow.sourceIdentity()!=rcColdSources.get()||
       !rcTriggerActions||!rcTriggerActions->ready()||
       rcTriggerActions->status()!=rc_trigger_actions::Status::Complete||
       !sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(tbOriginalDisc&&(!humanOriginalHud||!tbColdSources||!tbColdSources->ready()||!tbCollisionWorld||!tbWorldBorrow||
       tbCollisionWorld->coldSource()!=tbColdSources.get()||tbWorldBorrow->world()!=&liveWorld||
       !tbTriggerActions||tbWorldBorrow->actions()!=tbTriggerActions.get()||!tbTriggerActions->ready()||
       tbTriggerActions->status()!=tb_trigger_actions50TB::Status::Complete||
       !tbPodSound||!tbPodSound->prepared()||!tbDoorRuntime||!tbDoorRuntime->ready()||
       !tbDoorBinding||!tbDoorBinding->ready()||!tbDoorBinding->ownsRegistration()||
       tbDoorBinding->runtimeOwner()!=tbDoorRuntime.get()||tbDoorBinding->lightingOwner()!=tbLightOwner.get()||
       !tbMapView.runtimeValid()||!tbAiView.runtimeValid()||tbMapView.count()!=270||tbAiView.count()!=269||
       !tbMapView.sameOwner(tbAiView)||!tbMapView.sameWorld(liveWorld)||!tbAiView.sameWorld(liveWorld)||
       !sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(tcOriginalDisc&&(!humanOriginalHud||!tcColdSources||!tcCollisionWorld||!tcWorldBorrow||
       tcCollisionWorld->coldSource()!=tcColdSources.get()||tcWorldBorrow->world()!=&liveWorld||
       !tcTriggerActions||!tcTriggerActions->ready()||tcTriggerActions->status()!=tc_trigger_actions50TC::Status::Complete||
       !tcGlobalSound||!tcGlobalSound->prepared()||!tcGateRuntime||!tcGateRuntime->ready()||
       !tcGateVisibility||!tcGateVisibility->ready()||!tcGateVisibility->ownsRegistration()||
       !tcMapView.runtimeValid()||!tcAiView.runtimeValid()||tcMapView.count()!=268||tcAiView.count()!=293||
       !tcMapView.sameWorld(liveWorld)||!tcAiView.sameWorld(liveWorld)||
       !sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(scOriginalDisc&&(!humanOriginalHud||!scColdSources||!scGround||
       scGround->coldSource()!=scColdSources.get()||!scTriggerActions||!scTriggerActions->ready()||
       scTriggerActions->status()!=sc_trigger_actions::Status::Complete||
       !navigationMap||!navigationAI||!navigationMap->indicesPrepared||!navigationAI->indicesPrepared||
       navigationMap->track.count!=179||navigationAI->track.count!=180||
       !sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(isGbTrack&&(!gbParticleAuthoring||!gbParticleAuthoring->ready()||!gbParticleAttachments||!gbParticleAttachments->sourceGraphReady()||
       !gbTrackParticles||!gbTrackParticles->prepared()||!gbParticleRuntime||!gbParticleRuntime->sourceStaged()||
       !gbParticleRender||!gbParticleRender->ready()||gbParticleRender->runtimeOwner()!=gbParticleRuntime.get()||
       gbParticleRender->windowOwner()!=gbParticleWindow.get()||!gbParticleWindow||!gbParticleWindow->ready()||
       !gbFogOwner||!gbFogOwner->ready()||gbFogOwner->sourceOwner()!=gbColdSources.get()||
       !gbTrackParticles->particles()||gbTrackParticles->particles()->emitterRegistryReservation().registeredEmitters!=67u))return false;
    if(isMcTrack&&(!mcParticleSource||!mcParticleSource->ready()||!mcParticleAttachments||!mcParticleAttachments->sourceGraphReady()||
       !mcTrackParticles||!mcTrackParticles->prepared()||!mcParticleRuntime||!mcParticleRuntime->sourceStaged()||
       !mcParticleRender||!mcParticleRender->ready()||mcParticleRender->runtimeOwner()!=mcParticleRuntime.get()||
       mcParticleRender->windowOwner()!=mcParticleWindow.get()||!mcParticleWindow||!mcParticleWindow->ready()||
       !mcFogOwner||!mcFogOwner->ready()||mcFogOwner->sourceOwner()!=mcColdSources.get()||
       !mcTrackParticles->particles()||mcTrackParticles->particles()->emitterRegistryReservation().registeredEmitters!=40u+(mcParticleVehicles?mcParticleVehicles->registeredProducerCount():0u)))return false;
    if(isRcTrack&&(!rcParticleAuthoring||!rcParticleAuthoring->ready()||!rcParticleAttachments||!rcParticleAttachments->ready()||
       !rcTrackParticles||!rcTrackParticles->prepared()||!rcParticleRuntime||!rcParticleRuntime->active()||!rcParticleRender||
       rcParticleRuntime->sourceOwner()!=rcParticleAuthoring.get()||rcParticleRuntime->attachmentOwner()!=rcParticleAttachments.get()||
       rcParticleRuntime->coldOwner()!=rcTrackParticles.get()||!rcParticleWindow||!rcParticleWindow->ready()||
       !rcParticleRegistryFinalAccounted||!rcTrackParticles->particles()||
       rcTrackParticles->particles()->emitterStoragePayloadBytes()!=rcParticleRegistryPayloadCharged))return false;
    if(isScTrack&&(!scOriginalWorld||!scOriginalWorld->loaded()||!scLavaUv.loaded()||
       !scAtm.loaded()||!scAtm.ownsResources()||!scZeroSky.ready()))return false;
    if(maOriginalDisc&&(!humanOriginalHud||!maOriginalWorld||!maOriginalWorld->loaded()||
       !sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(saOriginalDisc&&(!humanOriginalHud||!saOriginalWorld||!saOriginalWorld->loaded()||
       !sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(taOriginalDisc&&(!humanOriginalHud||!taOriginalWorld||!taOriginalWorld->loaded()||
       !sourceCameraSession||!sourceCameraSession->ready()))return false;
    if(isGaTrack&&(!humanOriginalHud||!gaOriginalWorld||!gaOriginalWorld->loaded()||
       !gaAnimatedWorld||!gaAnimatedWorld->ready()||!gaDynamicWindow.loaded()||
       !gaDynamicWindow.ownsResources()||!gaTriggerActions||!gaTriggerActions->ready()||
       !gaAtm.loaded()||!gaAtm.ownsResources()))return false;
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
  if(!isRaTrack){
  renderRequest.track.mesh=isMcTrack?mcOriginalWorld->mesh():isGbTrack?gbOriginalWorld->mesh():isTbTrack?tbOriginalWorld->mesh():isRcTrack?rcOriginalWorld->mesh():&trackMesh;
  renderRequest.track.rcCrystal=rcCrystal;
  renderRequest.track.tbWorld=tbOriginalWorld;
  renderRequest.track.maximumRcProjectionBytes=isRcTrack?rcOriginalWorld->mesh()->vertices.size()*sizeof(std::array<float,2>):0;
  renderRequest.track.maximumRcStagingBytes=isRcTrack?rc_crystal50RC::Owner::uvCount*sizeof(std::array<float,2>):0;
  renderRequest.track.rcParticleWindow=rcParticleWindow;
  renderRequest.track.gbParticleWindow=gbParticleWindow;
  renderRequest.track.mcParticleWindow=mcParticleWindow;
  // Authored fog RGB and gameplay ranges are source-backed. The native shader
  // policy is enabled by default; explicit 0 is a diagnostic opt-out. This does
  // not claim original material/interpolation/quantization parity.
  const char* authoredFogSetting=std::getenv("RR_AUTHORED_WORLD_FOG");
  require50DU(!authoredFogSetting||std::string(authoredFogSetting)=="0"||
      std::string(authoredFogSetting)=="1","RR_AUTHORED_WORLD_FOG must be 0 or 1");
  require50DU(!(isGbTrack||isMcTrack)||!authoredFogSetting||std::string(authoredFogSetting)=="1",
      "GB actual particle/camera rendering requires authored world fog");
  if(!authoredFogSetting||std::string(authoredFogSetting)=="1"){
   require50DU(isTaTrack||isGcTrack||isGaTrack||isMaSaTrack||isMbTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack,"unsupported reconstructed fog track");
   auto& fog=renderRequest.track.worldFog;
   const float* color=isScTrack?scZeroSky.clearRgba():isMcTrack?mcSkyOwner->clearRgba():skyOwner->clearRgba();
   require50DU(color,"authenticated world fog RGB missing");
   for(unsigned i=0;i<3;++i)fog.color[i]=color[i];
   constexpr float minimum=1333.300048828125f,maximum=4000.f;
   // TC omitted Visibility/Haziness resolve to source defaults1/0.5.
   // MB and SC own authenticated global Fog fractions are 1/1.
   // SC zone RGB varies; this existing global native policy does not
   // claim camera-zone color/interpolation parity.
   const float haziness=(isGbTrack||isMcTrack)?sourceTrackFog.authoredFraction40:(isTbTrack||isRcTrack||isTcTrack)?.5f:(isScTrack||isMbTrack)?1.f:isMaSaTrack?1.f:.5f;
   // GA source Fog.Visibility0.4 is authenticated with its own SCB.
   const float visibility=(isGbTrack||isMcTrack)?sourceTrackFog.authoredFraction3C:(isTbTrack||isRcTrack||isTcTrack)?1.f:(isScTrack||isMbTrack)?1.f:(isTaTrack||isMaSaTrack)?1.f:isGaTrack?.4f:.5f;
   fog.end=minimum+visibility*(maximum-minimum);
   fog.start=(fog.end-64.f)*(1.f-haziness);fog.enabled=true;
   std::cerr<<"Authored native world fog: start "<<fog.start
       <<" end "<<fog.end<<"; source ranges established, native world shader parity pending\n";
  }
  if(originalCameraEnabled&&!isGbTrack&&!isMcTrack){
   const float* color=isScTrack?scZeroSky.clearRgba():skyOwner->clearRgba();
   require50DU(color,"actual source camera fog RGB missing");
   const auto channel=[&](unsigned axis){return static_cast<std::uint32_t>(std::lround(color[axis]*255.f));};
   sourceTrackFog.color30=0xff000000u|(channel(0)<<16)|(channel(1)<<8)|channel(2);
   sourceTrackFog.authoredFraction3C=(isTbTrack||isRcTrack||isTcTrack)?1.f:(isScTrack||isMbTrack)?1.f:(isTaTrack||isMaSaTrack)?1.f:isGaTrack?.4f:.5f;
   sourceTrackFog.authoredFraction40=(isTbTrack||isRcTrack||isTcTrack)?.5f:(isScTrack||isMbTrack)?1.f:isMaSaTrack?1.f:.5f;
  }
  renderRequest.track.sourceNormals=trackNormals.data();
  renderRequest.track.sourceNormalCount=static_cast<std::uint32_t>(trackNormals.size());
  renderRequest.track.maximumNormalBytes=isMaSaTrack?16u*1024u*1024u:isGaTrack?8u*1024u*1024u:
      trackSession->originalStaticCandidates?4u*1024u*1024u:2u*1024u*1024u;
  if(trackSession->originalStaticCandidates&&!isRcTrack){
   const auto* projectionUvs=mcOriginalWorld?mcOriginalWorld->projectionUvs():gbOriginalWorld?gbOriginalWorld->projectionUvs():tbOriginalWorld?tbOriginalWorld->projectionUvs():tcOriginalWorld?tcOriginalWorld->projectionUvs():scOriginalWorld?scOriginalWorld->projectionUvs():mbOriginalWorld?mbOriginalWorld->projectionUvs():gcOriginalWorld?gcOriginalWorld->projectionUvs():gaOriginalWorld?gaOriginalWorld->projectionUvs():maOriginalWorld?maOriginalWorld->projectionUvs():
       saOriginalWorld?saOriginalWorld->projectionUvs():trackMaterialOwner->projectionUvs();
   require50DU(projectionUvs,"original GC projected UV publication missing");
   renderRequest.track.sourceProjectionUvs=projectionUvs->data();
   renderRequest.track.sourceProjectionCount=projectionUvs->size();
   renderRequest.track.maximumProjectionBytes=projectionUvs->size()*sizeof(rr::assets::TrackProjectedUv);
  }
  // Bounded host rendering optimization; an explicit debugging opt-out keeps
  // the same scene and physics available for comparisons.
  const char* disableCulling=std::getenv("RR_DISABLE_TRACK_CULLING");
  renderRequest.track.enableFrustumCulling=!(disableCulling&&std::string(disableCulling)=="1");
  renderRequest.track.maximumVertexBytes=isMaSaTrack?32u*1024u*1024u:isGaTrack?16u*1024u*1024u:
      trackSession->originalStaticCandidates?8u*1024u*1024u:4u*1024u*1024u;
  renderRequest.track.maximumIndexBytes=isMaSaTrack?16u*1024u*1024u:isGaTrack?8u*1024u*1024u:2u*1024u*1024u;
  const auto* trackBank=mcOriginalWorld?mcOriginalWorld->bank():gbOriginalWorld?gbOriginalWorld->bank():tbOriginalWorld?tbOriginalWorld->bank():rcOriginalWorld?rcOriginalWorld->bank():tcOriginalWorld?tcOriginalWorld->bank():scOriginalWorld?scOriginalWorld->bank():mbOriginalWorld?mbOriginalWorld->bank():taOriginalWorld?taOriginalWorld->bank():gcOriginalWorld?gcOriginalWorld->bank():gaOriginalWorld?gaOriginalWorld->bank():maOriginalWorld?maOriginalWorld->bank():
       saOriginalWorld?saOriginalWorld->bank():trackMaterialOwner->bank();
  const auto* trackSpans=mcOriginalWorld?mcOriginalWorld->spans():gbOriginalWorld?gbOriginalWorld->spans():tbOriginalWorld?tbOriginalWorld->spans():rcOriginalWorld?rcOriginalWorld->spans():tcOriginalWorld?tcOriginalWorld->spans():scOriginalWorld?scOriginalWorld->spans():mbOriginalWorld?mbOriginalWorld->spans():taOriginalWorld?taOriginalWorld->spans():gcOriginalWorld?gcOriginalWorld->spans():gaOriginalWorld?gaOriginalWorld->spans():maOriginalWorld?maOriginalWorld->spans():
       saOriginalWorld?saOriginalWorld->spans():trackMaterialOwner->spans();
  require50DU(trackBank&&trackSpans,"selected-track material publication missing");
  std::vector<platform::MaterialTextureSampler50EB> trackSamplers(trackBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat});
  if(trackSession->originalStaticCandidates||taOriginalDisc){
   const auto* selected=mcOriginalWorld?mcOriginalWorld->selections():gbOriginalWorld?gbOriginalWorld->selections():tbOriginalWorld?tbOriginalWorld->selections():rcOriginalWorld?rcOriginalWorld->selections():tcOriginalWorld?tcOriginalWorld->selections():scOriginalWorld?scOriginalWorld->selections():mbOriginalWorld?mbOriginalWorld->selections():taOriginalWorld?taOriginalWorld->selections():gcOriginalWorld?gcOriginalWorld->selections():gaOriginalWorld?gaOriginalWorld->selections():maOriginalWorld?maOriginalWorld->selections():
       saOriginalWorld?saOriginalWorld->selections():trackMaterialOwner->selections();
   require50DU(selected,"original GC sampler selections missing");
   for(const auto& entry:*selected){
    require50DU(entry.textureIndex<trackSamplers.size(),"original GC sampler texture unresolved");
    auto& sampler=trackSamplers[entry.textureIndex];
    sampler.s=entry.clampU?platform::MaterialTextureWrap50EB::clampToEdge:platform::MaterialTextureWrap50EB::repeat;
    sampler.t=entry.clampV?platform::MaterialTextureWrap50EB::clampToEdge:platform::MaterialTextureWrap50EB::repeat;
   }
  }
  auto trackTextureRequest=textureRequest;
  trackTextureRequest.samplers=trackSamplers.data();
  trackTextureRequest.samplerCount=static_cast<std::uint32_t>(trackSamplers.size());
  trackTextureRequest.maximumGpuBytes=trackTextureBytes;
  require50DU(trackGpuTextures.create(trackTextureRequest,*trackBank,textureDiagnostic),
      "actual selected-track texture upload failed");
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
   const auto* trackSelections=mcOriginalWorld?mcOriginalWorld->selections():gbOriginalWorld?gbOriginalWorld->selections():tbOriginalWorld?tbOriginalWorld->selections():rcOriginalWorld?rcOriginalWorld->selections():tcOriginalWorld?tcOriginalWorld->selections():scOriginalWorld?scOriginalWorld->selections():mbOriginalWorld?mbOriginalWorld->selections():taOriginalWorld?taOriginalWorld->selections():gcOriginalWorld?gcOriginalWorld->selections():gaOriginalWorld?gaOriginalWorld->selections():maOriginalWorld?maOriginalWorld->selections():
       saOriginalWorld?saOriginalWorld->selections():trackMaterialOwner->selections();
   require50DU(trackSelections&&span.materialSelection<trackSelections->size(),"selected-track material selection missing");
   material_alpha50gx_fixture::Policy alpha{};
   const auto& sourceSelection=(*trackSelections)[span.materialSelection];
   const bool alphaSelected=mcOriginalWorld?mcOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):gbOriginalWorld?
       gbOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):tbOriginalWorld?
       tbOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):rcOriginalWorld?
       rcOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):tcOriginalWorld?
       tcOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):scOriginalWorld?
       scOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):mbOriginalWorld?
       mbOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):taOriginalWorld?
       taOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):gcOriginalWorld?
       gcOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):gaOriginalWorld?
       gaOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):maOriginalWorld?
       maOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):saOriginalWorld?
       saOriginalWorld->alpha(span.materialSelection,gameplayBinderTemplates,originalMainApplication.materialRuntime,alpha):isGaTrack?
       material_alpha50gx_fixture::selectGa(sourceSelection,gameplayBinderTemplates,
           originalMainApplication.materialRuntime,alpha):(isMaTrack||isSaTrack)?
       material_alpha50gx_fixture::selectMaSa(sourceSelection,gameplayBinderTemplates,
           originalMainApplication.materialRuntime,alpha):
       material_alpha50gx_fixture::select(sourceSelection,alpha);
   require50DU(alphaSelected,"selected-track material alpha policy mismatch");
   // Preserve the exact owned TB CROWD AA1 demand in the native span.
   // The authenticated source key and explicit field gate this one material.
   if(isTbTrack&&pod_materials50ed_fixture::detail::normalized(sourceSelection.materialName)=="CROWD"){
    require50DU(sourceSelection.track==track_source_fixture::Track::tb&&sourceSelection.assetIdentity==0x5442ull&&
        sourceSelection.slot==1&&sourceSelection.pass==1&&sourceSelection.passCount==1&&
        sourceSelection.materialKey=="2367f573cb30ca975777a7f4ac5473f2006d091c63e9e0b3346bbdbfec01d5b8"&&
        pod_materials50ed_fixture::detail::normalized(sourceSelection.program)=="PRELITCOMPRESSED"&&
        pod_materials50ed_fixture::detail::normalized(sourceSelection.textureName)=="CROWD",
        "TB CROWD coverage source identity differs");
    bool sourceAa1=false;
    for(const auto& field:sourceSelection.sourcePassFields.at("authored"))
     if(pod_materials50ed_fixture::detail::normalized(field.at("name").get<std::string>())=="ONEPASSANTIALIASINGONOFF"){
      require50DU(!sourceAa1&&field.at("value").is_string()&&field.at("value").get<std::string>()=="1",
          "TB CROWD explicit AA1 field differs");
      sourceAa1=true;
     }
    require50DU(sourceAa1,"TB CROWD authored AA1 field missing");
    binding.nativeCoverage=platform::NativeTrackCoverage50DS::fourSampleAa1;
   }
   // Preserve the exact owned RC CROWD AA1 demand in the native span.
   // The authenticated source key and explicit field gate this one material.
   if(isRcTrack&&pod_materials50ed_fixture::detail::normalized(sourceSelection.materialName)=="CROWD"){
    require50DU(sourceSelection.track==track_source_fixture::Track::rc&&sourceSelection.assetIdentity==0x5243ull&&
        sourceSelection.slot==1&&sourceSelection.pass==1&&sourceSelection.passCount==1&&
        sourceSelection.materialKey=="2367f573cb30ca975777a7f4ac5473f2006d091c63e9e0b3346bbdbfec01d5b8"&&
        pod_materials50ed_fixture::detail::normalized(sourceSelection.program)=="PRELITCOMPRESSED"&&
        pod_materials50ed_fixture::detail::normalized(sourceSelection.textureName)=="CROWD",
        "RC CROWD coverage source identity differs");
    bool sourceAa1=false;
    for(const auto& field:sourceSelection.sourcePassFields.at("authored"))
     if(pod_materials50ed_fixture::detail::normalized(field.at("name").get<std::string>())=="ONEPASSANTIALIASINGONOFF"){
      require50DU(!sourceAa1&&field.at("value").is_string()&&field.at("value").get<std::string>()=="1",
          "RC CROWD explicit AA1 field differs");
      sourceAa1=true;
     }
    require50DU(sourceAa1,"RC CROWD authored AA1 field missing");
    binding.nativeCoverage=platform::NativeTrackCoverage50DS::fourSampleAa1;
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
   binding.discardZeroAlpha=alpha.discardZeroAlpha;
   binding.sourceAlphaTest=alpha.sourceAlphaTest;
   binding.sourceAlphaReference=alpha.sourceAlphaReference;
   binding.sourceFogKnown=alpha.sourceFogKnown;binding.sourceFogEnabled=alpha.sourceFogEnabled;
   binding.sourceUvAnimation=(maShieldUv.loaded()&&maShieldUv.rows(static_cast<std::uint32_t>(trackBindings.size())))||
       (mbShieldUv.loaded()&&mbShieldUv.rows(static_cast<std::uint32_t>(trackBindings.size())));
   if(isScTrack&&scLavaUv.rows(static_cast<std::uint32_t>(trackBindings.size())))
    require50DU(scLavaUv.bindRenderSpan(static_cast<std::uint32_t>(trackBindings.size()),binding),
        "SC exact two-pass lava span lowering rejected");
   if(isMcTrack&&mcShieldUv.rows(static_cast<std::uint32_t>(trackBindings.size())))
    require50DU(mc_shield_render50MC::bind(mcShieldUv,*mcOriginalWorld,static_cast<std::uint32_t>(trackBindings.size()),binding),"MC actual unique shield UV span rejected");
   if(isTbTrack&&tbShieldUv.rows(static_cast<std::uint32_t>(trackBindings.size())))
    require50DU(tbShieldUv.bindRenderSpan(static_cast<std::uint32_t>(trackBindings.size()),binding),
        "TB actual shield span lowering rejected");
   if(isTcTrack&&tcShieldUv.rows(static_cast<std::uint32_t>(trackBindings.size())))
    require50DU(tcShieldUv.bindRenderSpan(static_cast<std::uint32_t>(trackBindings.size()),binding),
        "TC exact single-pass shield span lowering rejected");
   require50DU(span.worldInstanceIndex<staticShadowReceiving.size(),"static shadow receiver source instance rejected");
   binding.allowPodShadow=staticShadowReceiving[span.worldInstanceIndex]!=0;
   if(isRcTrack)require50DU(rcCrystal&&rcCrystal->bindSpan(trackBindings.size(),binding),"RC crystal exact span lowering rejected");
   trackBindings.push_back(binding);
  }
  }else{
   // The aggregate supplies its exact 44-family/49-image cold publication.
   // Never reinterpret RA through the legacy track mesh/material adapter.
   require50DU(raWorld&&raWorld->coldStaticWorld()&&
       raWorld->coldStaticWorld()->coldPixels()&&
       raWorld->coldStaticWorld()->coldPixels()->textureCount()==49,
       "actual complete RA cold pixel bank is unavailable");
   auto raTextureRequest=textureRequest;
   raTextureRequest.maximumGpuBytes=64u*1024u*1024u;
   require50DU(trackGpuTextures.createRaStaticWorld(raTextureRequest,*raWorld,textureDiagnostic),
       "actual RA full-world texture publication failed");
   renderRequest.raStaticWorld=raWorld;
   renderRequest.raStaticWorldTextures=&trackGpuTextures;
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
  if(!isRaTrack){
  renderRequest.track.textures=&trackGpuTextures;
  renderRequest.track.spans=trackBindings.data();
  renderRequest.track.spanCount=static_cast<std::uint32_t>(trackBindings.size());
  renderRequest.track.mode=platform::VehicleWindowMode50DS::callerMaterialTextures;
  renderRequest.track.vertexColorScale=isRcTrack?1.f:trackSession->originalStaticCandidates?255.f/128.f:2.f; // GC source RGB scale; TA host exposure unchanged.
  }
  const auto* raSky=isRaTrack?raWorld->skyAssets():nullptr;
  require50DU(!isRaTrack||raSky,"actual RA authored sky publication missing");
  std::vector<platform::VehicleWindowTrackSpan50DS> skyBindings;
  if(!isScTrack){
  const auto* skyBank=isRaTrack?raSky->bank():isMcTrack?mcSkyOwner->bank():skyOwner->bank();
  const auto* skyMesh=isRaTrack?raSky->mesh():isMcTrack?mcSkyOwner->mesh():skyOwner->mesh();
  const auto* skySpans=isRaTrack?raSky->spans():isMcTrack?mcSkyOwner->spans():skyOwner->spans();
  require50DU(skyBank&&skyMesh&&skySpans,"actual sky publication missing");
  std::vector<platform::MaterialTextureSampler50EB> skySamplers(skyBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::clampToEdge,platform::MaterialTextureWrap50EB::clampToEdge});
  auto skyTextureRequest=textureRequest;
  skyTextureRequest.samplers=skySamplers.data();
  skyTextureRequest.samplerCount=static_cast<std::uint32_t>(skySamplers.size());
  skyTextureRequest.maximumGpuBytes=(isGbTrack?90u:48u)*128u*128u*4u;
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
  }
  if(gcAnimatedWorld){
   require50DU(gcDynamicWindow.createGpu(textureRequest,*gcAnimatedWorld,textureDiagnostic),"original GC dynamic texture upload failed");
   require50DU(gcDynamicWindow.bindRequest(renderRequest,*gcAnimatedWorld),"original GC dynamic renderer publication failed");
   require50DU(gcAtm.createGpu(textureRequest,textureDiagnostic),"original GC ATM GPU bank rejected");
   require50DU(gcAtm.bindRequest(renderRequest),"original GC ATM render binding rejected");
  }
  if(gaAnimatedWorld){
   require50DU(gaDynamicWindow.createGpu(textureRequest,*gaAnimatedWorld,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,textureDiagnostic),"original GA dynamic texture upload failed");
   require50DU(gaDynamicWindow.bindRequest(renderRequest,*gaAnimatedWorld),"original GA dynamic renderer publication failed");
  }
  if(isTcTrack){
   require50DU(tcGateRuntime&&tcGateRuntime->ready()&&tcDynamicWindow.loaded(),"TC actual gate source unavailable before GPU creation");
   require50DU(tcDynamicWindow.createGpu(textureRequest,*tcGateRuntime,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,textureDiagnostic),"TC actual gate texture upload failed");
   require50DU(tcDynamicWindow.bindRequest(renderRequest,*tcGateRuntime),"TC actual gate renderer publication failed");
  }
  if(isTbTrack){
   require50DU(tbDoorRuntime&&tbDoorRuntime->ready()&&tbDynamicWindow.loaded(),"TB actual door source unavailable before GPU creation");
   require50DU(tbDynamicWindow.createGpu(textureRequest,*tbDoorRuntime,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,textureDiagnostic),"TB actual door texture upload failed");
   require50DU(tbDynamicWindow.bindRequest(renderRequest,*tbDoorRuntime),"TB actual door renderer publication failed");
  }
  if(isGbTrack){
   require50DU(gbAnimationRuntime&&gbAnimationRuntime->ready()&&gbDynamicWindow.loaded(),"GB actual four-animation source unavailable before GPU creation");
   require50DU(gbDynamicWindow.createGpu(textureRequest,*gbAnimationRuntime,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,textureDiagnostic),"GB actual four-animation texture upload failed");
   require50DU(gbDynamicWindow.bindRequest(renderRequest,*gbAnimationRuntime),"GB actual four-animation renderer publication failed");
  }
  if(isMcTrack){
   require50DU(mcAnimationRuntime&&mcAnimationRuntime->ready()&&mcDynamicWindow.loaded(),"MC actual fourteen-animation source unavailable before GPU creation");
   require50DU(mcDynamicWindow.createGpu(textureRequest,*mcAnimationRuntime,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,textureDiagnostic),"MC actual fourteen-animation texture upload failed");
   require50DU(mcDynamicWindow.bindRequest(renderRequest,*mcAnimationRuntime),"MC actual fourteen-animation renderer publication failed");
  }
  if(saAnimatedWorld){
   require50DU(saDynamicWindow.createGpu(textureRequest,*saAnimatedWorld,gameplayBinderTemplates,
       originalMainApplication.materialRuntime,textureDiagnostic),"SA actual dynamic texture upload failed");
   require50DU(saDynamicWindow.bindRequest(renderRequest,*saAnimatedWorld),"SA actual dynamic renderer binding failed");
  }
  if(maSaAtm.loaded()){
   require50DU(maSaAtm.createGpu(textureRequest,textureDiagnostic),"MA/SA actual ATM GPU bank rejected");
   require50DU(maSaAtm.bindRequest(renderRequest),"MA/SA actual ATM renderer binding rejected");
  }
  if(mbAtm.loaded()){
   require50DU(mbAtm.createGpu(textureRequest,textureDiagnostic),"MB actual ATM GPU bank rejected");
   require50DU(mbAtm.bindRequest(renderRequest),"MB actual ATM renderer binding rejected");
  }
  if(scAtm.loaded()){
   require50DU(scAtm.createGpu(textureRequest,textureDiagnostic),"SC actual ATM GPU bank rejected");
   require50DU(scAtm.bindRequest(renderRequest),"SC actual ATM renderer binding rejected");
  }
  if(tbAtm.loaded()){
   require50DU(tbAtm.createGpu(textureRequest,textureDiagnostic),"TB actual ATM GPU bank rejected");
   require50DU(tbAtm.bindRequest(renderRequest),"TB actual ATM renderer binding rejected");
  }
  if(gbAtm.loaded()){
   require50DU(gbAtm.createGpu(textureRequest,textureDiagnostic),"GB actual ATM GPU bank rejected");
   require50DU(gbAtm.bindRequest(renderRequest),"GB actual ATM renderer binding rejected");
  }
  if(mcAtm.loaded()){
   require50DU(mcAtm.createGpu(textureRequest,textureDiagnostic),"MC actual ATM GPU bank rejected");
   require50DU(mcAtm.bindRequest(renderRequest),"MC actual ATM renderer binding rejected");
  }
  if(rcAtm.loaded()){
   require50DU(rcAtm.createGpu(textureRequest,textureDiagnostic),"RC actual ATM GPU bank rejected");
   require50DU(rcAtm.bindRequest(renderRequest),"RC actual ATM renderer binding rejected");
  }
  if(isGbTrack){
   const platform::MaterialTextureSampler50EB sampler{platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat};
   auto request=textureRequest;request.samplers=&sampler;request.samplerCount=1;
   require50DU(gbParticleWindow&&gbParticleWindow->createGpu(request,textureDiagnostic),
       "GB exact canonical particle atlas upload rejected");
  }
  if(isMcTrack){
   const platform::MaterialTextureSampler50EB sampler{platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat};
   auto request=textureRequest;request.samplers=&sampler;request.samplerCount=1;
   require50DU(mcParticleWindow&&mcParticleWindow->createGpu(request,textureDiagnostic),
       "MC exact canonical particle atlas upload rejected");
  }
  if(isRcTrack){
   const platform::MaterialTextureSampler50EB sampler{platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat};
   auto request=textureRequest;request.samplers=&sampler;request.samplerCount=1;
   require50DU(rcParticleWindow&&rcParticleWindow->createGpu(request,textureDiagnostic),"RC actual particle atlas upload rejected");
  }
  if(tcAtm.loaded()){
   require50DU(tcAtm.createGpu(textureRequest,textureDiagnostic),"TC actual ATM GPU bank rejected");
   require50DU(tcAtm.bindRequest(renderRequest),"TC actual ATM renderer binding rejected");
  }
  if(gaAtm.loaded()){
   require50DU(gaAtm.createGpu(textureRequest,textureDiagnostic),"original GA ATM GPU bank rejected");
   require50DU(gaAtm.bindRequest(renderRequest),"original GA ATM render binding rejected");
  }
  if(nativeGcWater){
   require50DU(gcParticleMaterials.createGpu(textureRequest,textureDiagnostic),
       "original GC particle atlas GPU publication rejected");
   renderRequest.gcParticleCpuTextures=gcParticleMaterials.bank();
   renderRequest.gcParticleTextures=gcParticleMaterials.gpu();
   renderRequest.gcParticleMaterial=gcParticleMaterials.material();
  }
  if(!sourceDisplayLifetime->create(renderRequest,playerDrawAssets,rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);throw std::runtime_error("window vehicle renderer creation failed");
  }
  if(isMcTrack){require50DU(mcQueryBinding&&mcQueryBinding->activateQueries(),"MC actual final renderer/query admission rejected");if(!originalUiActive&&race.phase==race_session50HM::Phase::Uninitialized)activateColdRace();}
  if(isGbTrack){
   // Only real Window creation may publish the renderer association. The
   // aggregate/Bank issuer can now open genuine queries, without requiring
   // an earlier positive particle span or fabricated first-draw receipt.
   require50DU(gbParticleRender&&gbParticleRender->ready()&&gbQueryBinding&&
       gbQueryBinding->activateRenderer(gbParticleRender),"GB actual final renderer/query admission rejected");
   require50DU(gbQueryBinding->commitListeners(nullptr,0),"GB initial empty listener commit rejected");
   if(!originalUiActive&&race.phase==race_session50HM::Phase::Uninitialized)activateColdRace();
  }
  if(isGaTrack){
   // Actual cold owner/GPU state after successful renderer creation. No
   // fabricated trigger hits or rendered-frame counts enter this receipt.
   std::cerr<<"ga_native_ready dynamic_ready="<<(gaAnimatedWorld&&gaAnimatedWorld->ready())
       <<" dynamic_loaded="<<gaDynamicWindow.loaded()<<" dynamic_gpu="<<gaDynamicWindow.ownsResources()
       <<" trigger_ready="<<(gaTriggerActions&&gaTriggerActions->ready())
       <<" atm_loaded="<<gaAtm.loaded()<<" atm_gpu="<<gaAtm.ownsResources()
       <<" atm_bindings="<<renderRequest.trackAnimationCount
       <<" binder_enabled="<<gameplayBinderEnabled<<" binder_loaded="<<gameplayBinder.loaded()
       <<" binder_gpu="<<gameplayBinder.ownsResources()
       <<" binder_bound="<<(renderRequest.gameplayBinderTextures!=nullptr)
       <<" camera_policy="<<(originalCameraEnabled?"recovered_original_attached_follow":"host_chase_framePod")
       <<" listener_policy=accepted_camera_xz\n";
  }
  auto publishSourceCameraFog=[&](){
   if(!originalCameraEnabled||!renderRequest.track.worldFog.enabled)return;
   require50DU(!isMcTrack||(mcFogOwner&&mcFogOwner->ready()&& &sourceTrackFog==mcFogOwner->cameraFields()),
       "MC source camera lost its actual same-owner fog fields");
   require50DU(!isGbTrack||(gbFogOwner&&gbFogOwner->ready()&& &sourceTrackFog==gbFogOwner->cameraFields()),
       "GB current camera fog storage identity differs");
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
   if(isMcTrack){
    requireMbRuntimeNavigation50MB();
    const auto pods=runtime_asset_paths50LB::resolve("work/native/project/disc/PODS");
    const auto camBytes=std::filesystem::file_size(pods/playerPodId/(playerPodId+".CAM"));
    reserveMcEnvironment(static_cast<std::size_t>(camBytes),"MC retained CAM exceeds remaining payload");
    reserveMcEnvironment(4u*186642u,"MC TOB authentication scratch exceeds remaining payload");
    std::string error;
    require50DU(mcSourceCameraAssets&&mcSourceCameraAssets->prepare(mcColdSources,pods,playerPodId,racerRigs[0]->phy,error),error.c_str());
    require50DU(mcSourceCameraAssets->sourceIdentity()==mcColdSources.get()&&
        mcSourceCameraAssets->camBytes()==camBytes&&mcSourceCameraAssets->camCapacity()<=camBytes&&
        mcSourceCameraAssets->inputs()->tob==mcColdSources->triggerParameters().original_bytes.data(),
        "MC selected CAM/retained TOB differs from same-source reservation");
   }
   else if(isGbTrack){
    requireMbRuntimeNavigation50MB();
    require50DU(gbSourceCameraAssets&&gbColdSources&&gbColdSources->ready(),"GB scoped camera source unavailable");
    const auto path=runtime_asset_paths50LB::resolve(
        std::filesystem::path("work/native/project/disc/PODS")/playerPodId/(playerPodId+".CAM"));
    const auto camBytes=std::filesystem::file_size(path);
    require50DU(camBytes&&camBytes<=gbRemainingPayloadBytes(),"GB selected CAM exceeds remaining payload");
    reserveGbAdditionalPayload(static_cast<std::size_t>(camBytes),"GB retained CAM exceeds remaining payload");
    reserveGbAdditionalPayload(4u*20617u,"GB retained TOB authentication scratch exceeds remaining payload");
    sourceCameraAssets.loadGb(*trackSession,playerPodId,racerRigs[0]->phy,gbColdSources);
    require50DU(sourceCameraAssets.cam.size()==camBytes&&sourceCameraAssets.cam.capacity()<=camBytes&&
        sourceCameraAssets.tob.empty()&&sourceCameraAssets.inputs.tob==gbColdSources->originalTob().data(),
        "GB actual CAM allocation/retained TOB differs from its reserved source");
   }
   else if(isTbTrack){
    requireMbRuntimeNavigation50MB();
    require50DU(tbSourceCameraAssets&&tbColdSources&&tbColdSources->ready(),"TB scoped camera source unavailable");
    const auto path=runtime_asset_paths50LB::resolve(
        std::filesystem::path("work/native/project/disc/PODS")/playerPodId/(playerPodId+".CAM"));
    const auto camBytes=std::filesystem::file_size(path);
    require50DU(camBytes&&camBytes<=tbRemainingPayloadBytes(),"TB selected CAM exceeds remaining payload");
    reserveTbAdditionalPayload(static_cast<std::size_t>(camBytes),"TB retained CAM exceeds remaining payload");
    reserveTbAdditionalPayload(4u*106139u,"TB retained TOB authentication scratch exceeds remaining payload");
    sourceCameraAssets.loadTb(*trackSession,playerPodId,racerRigs[0]->phy,tbColdSources);
    require50DU(sourceCameraAssets.cam.size()==camBytes&&sourceCameraAssets.cam.capacity()<=camBytes&&
        sourceCameraAssets.tob.empty()&&sourceCameraAssets.inputs.tob==tbColdSources->originalTob().data(),
        "TB actual CAM allocation/retained TOB differs from its reserved source");
   }
   else if(isRcTrack)sourceCameraAssets.loadRc(*trackSession,playerPodId,racerRigs[0]->phy,rcColdSources);
   else if(!isRaTrack)sourceCameraAssets.load(*trackSession,playerPodId,racerRigs[0]->phy);
   else sourceCameraAssets.cam=runtime_startup50fl::readBytes(
       runtime_asset_paths50LB::resolve("work/native/project/disc/PODS")/playerPodId/(playerPodId+".CAM"));
   sourceCameraDisplay=std::make_unique<sourceCamera::DisplayInputs>(sourceCamera::DisplayInputs{
       sourceVideo,sourceRendererDispatch,renderer,rendererDiagnostic,2,640,448,
       sourceFadeManagerA8,sourceFadeDuration98,0,0,
       static_cast<std::uint8_t>(originalUiStarted?originalMainApplication.state->globalFlags228:applicationEvents.rootFlags228),
       OriginalCameraDrainPolicy::initializedContext,
       isRaTrack?nullptr:&sourceTrackFog,
       consumedRaStage.race()?&originalMainApplication.applicationMode22C():nullptr});
   if(isRaTrack||humanOriginalHud){
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
        "PositionStats construction changed the retained RA canonical publication");
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
    if(!isRaTrack){
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
   if(consumedTaStage.race()||consumedMaStage.race()||consumedSaStage.race()||consumedGaStage.race()||consumedGcStage.race()||sourceConsumedIntroActivation){
    taIntroAuthoring=originalMainApplication.retainNativeTaIntroAuthoring50LA();
    require50DU(bool(taIntroAuthoring),"actual selected-track intro authoring custody rejected");
    const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
    const auto audioRoot=suppliedAudioRoot?std::filesystem::path(suppliedAudioRoot):
        runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO");
    std::string error;
    require50DU(platform::prepareOriginalTaIntroPcm50LA(taIntroAuthoring,audioRoot,taIntroPcm,error),
        error.empty()?"original selected-track intro PCM preparation rejected":error.c_str());
   }
   if(isRaTrack){
    const ra_source_camera50LA::CameraRuntimeOwners runtime{racerBank,s,x.bindings,baseTail,originalAi,
        lapContinuity.timing,originalAiSession.manager,legacyRace,sessionRandom,sourceAttachedGlobals,
        applicationClock,controlPresets,nativeOrdinaryLaunch11258,dynamicProfile.game34,cameraStartup350,
        sourceBlur,poseHistory,boostState,sourceCameraCycleAudio,dynamicRacer60,originalAiHumanCount,
        humanResultsCountdown,playerEnvironment.held,humanCourseEnabled?&sourceRaceHud:nullptr,nullptr,
        &startMusicContext,requestStartMusic,nativeRecordActor.get(),
        taIntroAuthoring?liveAudio->groupsForProviders():nullptr,taIntroAuthoring,requestIntroMusicFade,
        humanCourseEnabled?&playerEvents:nullptr};
    const bool cameraConstructed=raSession&&raSession->constructCamera(runtime,sourceCameraAssets.cam,*sourceCameraDisplay,raSessionDiagnostic);
    if(!cameraConstructed){
     std::cerr<<"RA camera failure="<<(raSessionDiagnostic.aggregate.failure?raSessionDiagnostic.aggregate.failure:"")
       <<" detail="<<(raSessionDiagnostic.aggregate.runtimeQueries.message?raSessionDiagnostic.aggregate.runtimeQueries.message:"")<<'\n';
     require50DU(false,"same-owner RA camera construction rejected");
    }
    if(taIntroAuthoring){
     auto* controller=raSession->introController174A30(raSessionDiagnostic);
     require50DU(controller&&consumedRaStage.race()&&
         consumedRaStage.race()->retainNativeIntroControllerSlot48(*controller),
         "actual RA race slot48 intro controller publication rejected");
     require50DU(raSession->constructIntroAudio177750(taIntroPcm,
         platform::originalTaIntroFirstVoice50HD,platform::originalTaIntroVoices50HD,raSessionDiagnostic),
         "actual RA intro audio177750 construction rejected");
    }
    sourceCameraSession=raSession->prepareAndBorrowCamera(raSessionDiagnostic);
    require50DU(bool(sourceCameraSession),"same-owner RA camera preparation rejected");
   }else{
   requireMbRuntimeNavigation50MB();

   sourceCamera::ActualOwners sourceCameraActual{
       racerBank,s,x.bindings,baseTail,originalAi,lapContinuity.timing,originalAiSession.manager,raceBorrow,
       sessionRandom,sourceAttachedGlobals,applicationClock,controlPresets,nativeOrdinaryLaunch11258,
       dynamicProfile.game34,cameraStartup350,
       gcLightOwner,zoneLifecycle,mainMembership,sourceBlur,*sourceFollowQueries,poseHistory,boostState,sourceCameraCycleAudio,
       dynamicRacer60,originalAiHumanCount,humanResultsCountdown,playerEnvironment.held,
       humanCourseEnabled?&sourceRaceHud:nullptr,nullptr,
       trackMaterialRequest.canonicalDomain?originalMainApplication.entities():nullptr,
       trackMaterialRequest.canonicalDomain?trackMaterialRequest.canonicalDomain->materials382B38():nullptr,
       trackMaterialRequest.canonicalDomain?&originalMainApplication.materialDirectory36B970():nullptr,
       trackMaterialRequest.canonicalDomain?&racerRigs:nullptr,
       trackMaterialRequest.canonicalDomain?(isMcTrack?mcTrackParticles.get():isGbTrack?gbTrackParticles.get():isTbTrack?tbTrackParticles.get():isRcTrack?rcTrackParticles.get():&nativeTaParticles):nullptr,
       trackMaterialRequest.canonicalDomain?&selectedRuntimeHeightDomain:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeRightModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeLeftModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeChariotModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeBottomRightModels:nullptr,
       trackMaterialRequest.canonicalDomain?&nativeBottomLeftModels:nullptr,
       originalMainApplication.animationClock(),&sourceCameraCables,
       directOriginalHuman&&!isMbTrack&&!isTcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack?navigationMap:nullptr,
       directOriginalHuman&&isMbTrack?mbMapView:TrackNavigationView50EU{},
       directOriginalHuman&&isTcTrack?tcMapView:TrackNavigationView50TC{},
       directOriginalHuman&&isTbTrack?tbMapView:TrackNavigationView50TB{},
       directOriginalHuman&&isGbTrack?gbMapView:TrackNavigationView50GB{},
       directOriginalHuman&&isMcTrack?mcMapView:TrackNavigationView50MC{},
       &startMusicContext,requestStartMusic,nativeRecordActor.get(),
       taIntroAuthoring?liveAudio->groupsForProviders():nullptr,taIntroAuthoring,requestIntroMusicFade};
   if((isTaTrack||consumedMaStage.race()||consumedSaStage.race()||consumedGaStage.race()||consumedGcStage.race())&&trackMaterialRequest.canonicalDomain){
    sourceCameraSession=isTaTrack?originalMainApplication.createNativeTaCameraSession50LA(sourceCameraActual,sourceCameraAssets):
        isMaTrack?originalMainApplication.createNativeMaCameraSession50LA(sourceCameraActual,sourceCameraAssets):
        isSaTrack?originalMainApplication.createNativeSaCameraSession50LA(sourceCameraActual,sourceCameraAssets):
        isGaTrack?originalMainApplication.createNativeGaCameraSession50LA(sourceCameraActual,sourceCameraAssets):
        originalMainApplication.createNativeGcCameraSession50LA(sourceCameraActual,sourceCameraAssets);
    require50DU(bool(sourceCameraSession),"same-Application TA native camera custody rejected");
    if(taIntroAuthoring){
     require50DU(originalMainApplication.constructNativeTaIntroCamera50LA(),"actual TA cut/controller construction rejected");
     auto* controller=sourceCameraSession->taControllerEntity174A30();
     require50DU(controller&&nativeIntroRace()&&nativeIntroRace()->retainNativeIntroControllerSlot48(*controller),
         "actual TA race slot48 controller publication rejected");
     require50DU(sourceCameraSession->constructTaIntro177750(
         taIntroPcm,platform::originalTaIntroFirstVoice50HD,platform::originalTaIntroVoices50HD),
         "actual TA intro audio177750 construction rejected");
    }
   }else sourceCameraSession=std::make_shared<sourceCamera::Session>(sourceCameraActual);
   const auto prepared=sourceCameraSession->prepare(sessionIndex+1,isMcTrack?*mcSourceCameraAssets->inputs():sourceCameraAssets.inputs,*sourceCameraDisplay);
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
   if(isTbTrack){accountTbParticleRegistry();tbParticleRegistryFinalAccounted=true;}
   if(isRcTrack){accountRcParticleRegistry();rcParticleRegistryFinalAccounted=true;}
   }
   // Demand comes from actual retained consumers, not a course-name list.
   // TB owns the canonical pool without a separate numeric renderer. RA has
   // its separate static-fog observation phase and has none of these owners.
   if(nativeGcWater||rcParticleRender||tbTrackParticles||gbParticleRender||mcParticleRender)
    require50DU(sourceCameraSession&&sourceCameraSession->bindGameplayFogRenderPhase(sourceFogRenderPhase),
        "actual selected render consumer fog phase binding rejected");
   if(sourceConsumedPauseActivation&&consumedRaStage.race()){
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
   if(sourceConsumedPauseActivation&&consumedRaStage.race()&&!originalMainApplication.finishPauseDrawCold50LA(sourcePause)){
    const auto& d=sourcePause.nativeDiagnostic;
    std::cerr<<"pause_draw_diagnostic pc="<<d.sourcePC<<" reference="<<d.reference<<" pass="<<d.pass
        <<" canonical_status="<<static_cast<unsigned>(d.canonical.status)<<" canonical_pc="<<d.canonical.sourcePC
        <<" canonical_row="<<d.canonical.row<<" canonical_field="<<(d.canonical.field?d.canonical.field:"")
        <<" material_error="<<static_cast<unsigned>(d.material.error)<<" material_pc="<<d.material.sourcePC
        <<" material_field="<<d.material.field<<" texture_code="<<d.texture.code<<" texture_detail="<<d.texture.detail<<'\n';
    require50DU(false,"actual Pause FINAL native draw preparation rejected");
   }
   if(sourceConsumedPauseActivation&&consumedRaStage.race()){
    sourcePauseTrack=raWorld->borrowNativeSource(raWorld,raDiagnostic);
    require50DU(sourcePauseTrack&&sourcePauseObservation.bind(originalMainApplication,sourcePause.storage())&&
        sourcePauseObservation.bindRace(sourceCameraSession,sourcePauseTrack),
        "actual Pause camera/source observation bind rejected");
    require50DU(renderer.bindOriginalPauseDerived(sourcePause.drawing(),rendererDiagnostic),
        "actual Pause GPU binding rejected");
    require50DU(originalMainApplication.attachNativeConsumedRacePause(*consumedRaStage.race(),sourceCameraSession),
        "actual consumed race/Pause parent attachment rejected");
    sourcePauseParentAttached=true;
   }
   // RA human tournaments load the same Pause storage for Results (but never
   // its draw); other tracks load it above. The all-AI spectator route keeps
   // its own consumed-race Pause binding.
   if(humanOriginalHud&&!sourceConsumedPauseActivation&&sourcePause.storage()&&sourceCameraSession){
    // Constructor17EA08 title IDs in actual track-ID order (as the loader).
    constexpr std::size_t trackTitles[]={106,111,115,117,110,114,118,108,107,112,116,109,119};
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
    bool bound=originalMainApplication.riderProfile&&
        originalMainApplication.riderProfile->courseRaceSnapshot(course)&&course.course<13&&
        originalMainApplication.finishPauseDrawCold50LA(sourcePause)&&
        renderer.bindOriginalPauseDerived(sourcePause.drawing(),rendererDiagnostic);
    bound=bound&&sourcePauseObservation.bindHost(originalMainApplication,sourcePause.storage(),
        sourceCameraSession,trackTitles[course.course]);
    std::cerr<<"original_pause_host bound="<<bound<<" course="<<course.course<<" subtype="<<course.subtype<<"\n";
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
    //Additional same-owner source-slot publication only. Legacy/non-RA or
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

   if(sourceConsumedIntroActivation&&consumedRaStage.race()){
    OriginalResults104Diagnostic50LA rootDiagnostic;
    if(!originalMainApplication.publishNativeHumanResultsRoot50LA(*this,rootDiagnostic)){
     std::cerr<<"native_human_results_root_rejected pc="<<rootDiagnostic.sourcePC
         <<" operation="<<(rootDiagnostic.operation?rootDiagnostic.operation:"owner")<<'\n';
     require50DU(false,"actual accepted-world human Results root publication rejected");
    }
   }
   installHumanCameraInput50DU();
   if(gcNativeWorldSelected){
    require50DU(gcTriggerActions&&gcTriggerActions->ready()&&gcGateAudio&&gcGateAudio->available()&&
        !gcGateAudio->failed()&&originalAiAudioEnabled&&nativeGcWater&&nativeGcWater->prepared()&&
        gcAnimatedWorld&&gcAnimatedWorld->ready()&&gcDynamicWindow.loaded()&&gcDynamicWindow.ownsResources()&&
        gcAtm.loaded()&&gcAtm.ownsResources()&&gcParticleMaterials.loaded()&&gcParticleMaterials.ownsResources()&&
        sourceCameraSession->ready(),"GC actual native-world provider ownership is incomplete");
    std::cerr<<"gc_native_ready trigger_count="<<gcTriggerActions->nativeGroup().triggers.size()
        <<" trigger_colliders="<<gcTriggerActions->nativeGroup().colliders.size()
        <<" gate_pcm="<<gcGateAudio->available()<<" water_effects="<<nativeGcWater->prepared()
        <<" particle_gpu="<<gcParticleMaterials.ownsResources()
        <<" camera_ready="<<sourceCameraSession->ready()
        <<" native_geometry_policy=type2_endpoint_swept_surface\n";
   }
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
  platform::HostRaceResults50IL resultsUi;
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
       {"native_pod_shadows",renderRequest.enableNativePodShadows},
       {"world_renderer",renderRequest.raStaticWorld?"native_ra_aggregate":"legacy_track_adapter"},
       // Keep existing keys stable for receipt readers, but identify their
       // scope: RA uses its own camera fog and conservative clip rejection.
       {"track_adapter_settings_apply",!bool(renderRequest.raStaticWorld)},
       {"track_frustum_culling",renderRequest.track.enableFrustumCulling},
       {"authored_world_fog",renderRequest.track.worldFog.enabled},
       {"ra_authored_material_fog",bool(renderRequest.raStaticWorld)},
       {"ra_conservative_clip_rejection",bool(renderRequest.raStaticWorld)},
       {"ra_internal_coverage_samples",renderRequest.raStaticWorld?4:0},
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
       <<",\"loaded_track_incomplete\":"<<(((isGcTrack&&!gcNativeWorldSelected)||isGaTrack||isMaSaTrack)?"true":"false")
       <<",\"gc_incomplete_opt_in\":"<<(trackAdmission.gcIncompleteWorld?"true":"false")
       <<",\"gc_native_world_selected\":"<<(gcNativeWorldSelected?"true":"false")
       <<",\"gc_mode2_admitted\":"<<(trackAdmission.gcMode2?"true":"false")
       <<",\"ga_candidate_admitted\":"<<(isGaTrack&&trackAdmission.gaOriginalAiCandidate?"true":"false")
       <<",\"results_visible\":"<<((humanCourseEnabled||allAiEnabled||!isGaTrack)&&humanResultsReady&&!sessionMenu.open()?"true":"false")
       <<",\"results_action\":"<<resultsUi.selectedAction()
       <<",\"results_confirming\":"<<(resultsUi.confirming()?"true":"false")
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
      <<" navigation map/AI="<<selectedMapCount50MB()<<'/'<<selectedAiCount50MB()<<'\n'
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
      "Restart destroys this session and repeats the authored-grid cold setup.\n";
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
   require50DU(humanCourseEnabled&&originalAiHumanCount==1&&racerBank.count()==8,
       "tournament result lost its original manager roster");
   std::array<rr::game::OriginalTournamentResultRacer50KG,8> rows{};
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
  rr::platform::loading_progress50DU::finish();
  PlayerLoadingSession50DU::checkStop();
  coldLoadDiagnostics.complete();
  // Opt-in host observations only; each receipt is bounded to this session.
  bool firstMainPollReported=false,firstMainActivityGateReported=false;
  bool secondMainPollReported=false,firstMainInactiveReported=false;
  // Borrow existing locals for the synchronous host loop only. Original
  // resources and final cleanup remain in this runSession scope.
  interactive_loop50DU::Context loopContext50DU{
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
   .courseRaceCold=courseRaceCold,
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
   .humanCourseEnabled=humanCourseEnabled,
   .raceMusic=raceMusic,
   .mode1Enabled=mode1Enabled,
   .dynamicContactEnabled=dynamicContactEnabled,
   .sourceEndingFade=sourceEndingFade,
   .sourceFadeIn=sourceFadeIn,
   .sourceFadeOut=sourceFadeOut,
   .suspendedTaCameraFrame=suspendedTaCameraFrame,
   .originalCameraEnabled=originalCameraEnabled,
   .isGaTrack=isGaTrack,
   .restartRequested=restartRequested,
   .returnOriginalMainRequested=returnOriginalMainRequested,
   .resultsUi=resultsUi,
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
   .isMbTrack=isMbTrack,
   .isTcTrack=isTcTrack,
   .isRcTrack=isRcTrack,
   .isTbTrack=isTbTrack,
   .isGbTrack=isGbTrack,
   .isMcTrack=isMcTrack,
   .navigationMap=navigationMap,
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
   .legacyRace=legacyRace,
   .mbRace=mbRace,
   .tcRace=tcRace,
   .gbRace=gbRace,
   .mcRace=mcRace,
   .tbRace=tbRace,
   .commitCompletedTournament=interactive_frame50DU::BorrowedCall<void()>{commitCompletedTournament},
   .trackBaselineEligible=interactive_frame50DU::BorrowedCall<bool()>{trackBaselineEligible},
   .discardTrackBaseline=interactive_frame50DU::BorrowedCall<void() noexcept>{discardTrackBaseline},
   .captureCommitted=interactive_frame50DU::BorrowedCall<void()>{captureCommitted},
   .reportUiState=interactive_frame50DU::BorrowedCall<void(const char*)>{reportUiState},
   .stopFeedbackTransport=interactive_frame50DU::BorrowedCall<void()>{stopFeedbackTransport},
   .finishTaRoot=interactive_frame50DU::BorrowedCall<bool()>{finishTaRoot},
   .finishTaRootWithResults=interactive_frame50DU::BorrowedCall<bool()>{finishTaRootWithResults},
   .beginTaRoot=interactive_frame50DU::BorrowedCall<bool()>{beginTaRoot},
   .publishSourceCameraFog=interactive_frame50DU::BorrowedCall<void()>{publishSourceCameraFog},
   .requireMbRuntimeNavigation50MB=interactive_frame50DU::BorrowedCall<void()>{requireMbRuntimeNavigation50MB},
   .advanceWorldAnimations=interactive_frame50DU::BorrowedCall<void()>{advanceWorldAnimations},
   .advanceNativeGcWater=interactive_frame50DU::BorrowedCall<void()>{advanceNativeGcWater},
   .advanceRcTrackParticles=interactive_frame50DU::BorrowedCall<void()>{advanceRcTrackParticles},
   .advanceTbTrackParticles=interactive_frame50DU::BorrowedCall<void()>{advanceTbTrackParticles},
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
   .gcGraphicsSequence=gcGraphicsSequence,
   .nativeSelectedLights=nativeSelectedLights,
   .gcLightOwner=gcLightOwner,
   .gcLightDiagnostic=gcLightDiagnostic,
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
   .gcDrawLights=gcDrawLights,
   .racerVisuals=racerVisuals,
   .nativeLeftModels=nativeLeftModels,
   .nativeRightModels=nativeRightModels,
   .racerRigs=racerRigs,
   .nativeBottomLeftModels=nativeBottomLeftModels,
   .nativeBottomRightModels=nativeBottomRightModels,
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
   .isRaTrack=isRaTrack,
   .raWorld=raWorld,
   .gbQueryBinding=gbQueryBinding,
   .audioListener=audioListener,
   .audioListenerReady=audioListenerReady,
   .boostState=boostState,
   .selectedMapCount50MB=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount50MB},
   .selectedMapNode50MB=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode50MB},
   .baseTail=baseTail,
   .coldFlame=coldFlame,
   .nativeGcWater=nativeGcWater,
   .sourceBlur=sourceBlur,
   .racerCables=racerCables,
   .mainContactFrames=mainContactFrames,
   .secondaryContactFrames=secondaryContactFrames,
   .mcWaterOwner=mcWaterOwner,
   .playerRacer60=playerRacer60,
   .gcTriggerActions=gcTriggerActions,
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
   .materialAudio=materialAudio,
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
   .isScTrack=isScTrack,
   .scZeroSky=scZeroSky,
   .raSky=raSky,
   .mcSkyOwner=mcSkyOwner,
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
   .gcParticleRender=gcParticleRender,
   .gcAnimatedWorld=gcAnimatedWorld,
   .gcDynamicWindow=gcDynamicWindow,
   .gcAtm=gcAtm,
   .gaAnimatedWorld=gaAnimatedWorld,
   .gaDynamicWindow=gaDynamicWindow,
   .tcGateRuntime=tcGateRuntime,
   .tcDynamicWindow=tcDynamicWindow,
   .tbDoorRuntime=tbDoorRuntime,
   .tbDynamicWindow=tbDynamicWindow,
   .mcAnimationRuntime=mcAnimationRuntime,
   .mcDynamicWindow=mcDynamicWindow,
   .gbAnimationRuntime=gbAnimationRuntime,
   .gbDynamicWindow=gbDynamicWindow,
   .gaAtm=gaAtm,
   .saAnimatedWorld=saAnimatedWorld,
   .saDynamicWindow=saDynamicWindow,
   .maSaAtm=maSaAtm,
   .mbAtm=mbAtm,
   .scAtm=scAtm,
   .tbAtm=tbAtm,
   .mcAtm=mcAtm,
   .gbAtm=gbAtm,
   .rcAtm=rcAtm,
   .tcAtm=tcAtm,
   .isMaSaTrack=isMaSaTrack,
   .sourceSkySelection=sourceSkySelection,
   .opponentFlamePrepared=opponentFlamePrepared,
   .opponentDraw=opponentDraw,
   .opponentFlames=opponentFlames,
   .opponentUvs=opponentUvs,
   .raceOpponentCount=raceOpponentCount,
   .sourceApplicationClock=sourceApplicationClock,
   .maShieldUv=maShieldUv,
   .mbShieldUv=mbShieldUv,
   .tbShieldUv=tbShieldUv,
   .mcShieldUv=mcShieldUv,
   .tcShieldUv=tcShieldUv,
   .scLavaUv=scLavaUv,
   .mcParticleWindow=mcParticleWindow,
   .mcParticleRender=mcParticleRender,
   .mcParticleRenderDiagnostic=mcParticleRenderDiagnostic,
   .mcParticleFrame152Notice=mcParticleFrame152Notice,
   .gbParticleWindow=gbParticleWindow,
   .gbParticleRender=gbParticleRender,
   .gbParticleRenderDiagnostic=gbParticleRenderDiagnostic,
   .rcParticleWindow=rcParticleWindow,
   .rcParticleRender=rcParticleRender,
   .rcParticleRenderDiagnostic=rcParticleRenderDiagnostic,
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
   .gcAnimatedWorldSteps=gcAnimatedWorldSteps,
   .gcNativeGate=gcNativeGate,
   .boostButtonCode=boostButtonCode,
   .powerslideButtonCode=powerslideButtonCode,
   .repairButtonCode=repairButtonCode,
   .courseDiagnostic=courseDiagnostic,
   .courseOutput=courseOutput,
   .selectedMapIndex50MB=interactive_frame50DU::BorrowedCall<std::uint8_t(std::uint32_t)>{selectedMapIndex50MB},
   .opponentAudio=opponentAudio,
   .observedNativeTicks=observedNativeTicks,
   .maximumSleepNs=maximumSleepNs,
   .viewForwardX=viewForwardX,
   .viewForwardZ=viewForwardZ,
  };
  loopContext50DU.run();
  commitCompletedTournament();
  displayWallTotals.reportAndReset("session_end");
  if(trackBaseline.enabled()&&!trackBaseline.complete())
   std::cerr<<"track_fps_baseline_unavailable: no uninterrupted five-second warmup plus five-second measurement completed\n";
  require50DU(terminalInputError==0,"live input stream failed");
}
} // namespace interactive_publication50DU

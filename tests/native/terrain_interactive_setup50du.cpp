#include "native_last_failure50pd.h"
#include <cfenv>
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "../../platform/linux/native/stall_watchdog50du.h"
#include "../../src/game/ps2_float_mode50la.h"
#include "terrain_interactive_setup50du.h"
#include "original_native_world_water50la.h"
#include <cstring>
#include "../../src/game/indexed_type2_contact50cc.h"
#include "../../src/game/compound_query50ij.h"
#include "terrain_interactive_continuation50du.h"
#include "terrain_interactive_frame50du.h"
#include "terrain_interactive_publication50du.h"
#include "terrain_interactive_cold_owners50du.h"
#include "terrain_interactive_cold_contacts50du.h"
#include "terrain_interactive_loop_providers50du.h" // Complete Application before inline loading helpers.
#include "player_loading_session50du.h"
#include "cold_load_diagnostics50du.h"
#include "pod_cables50fd_fixture.h"
#include "original_consumed_ra_stage50du.h"
#include "original_consumed_ga_stage50du.h"
#include "original_consumed_gc_stage50du.h"
#include "original_consumed_ma_stage50du.h"
#include "original_consumed_mb_stage50du.h"
#include "original_consumed_sa_stage50du.h"
#include "original_consumed_ta_stage50du.h"
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

#include "pod_speech_authoring50hg_fixture.h"
#include "ma_trigger_candidates_fixture.h"
#include "sc_trigger_candidates_fixture.h"
#include "sa_trigger_candidates_fixture.h"
#include "gc_animated_candidates50ig_fixture.h"
#include "gc_trigger_candidates50kg_fixture.h"
#include "ga_animated_candidates_fixture.h"
#include "ga_trigger_candidates_fixture.h"
#include "terrain_interactive_storage50du.h"
#include "runtime_allocation_counter50fl.h"
#include "native_ai_session50iv_fixture.h"
#include "../../src/game/original_human_finish50kg.h"
#include "player_loading_session50du.h"
#include "original_racer_manager_entity50la.h"
#include "original_subtype3_record_owner50la.h"
#include "original_record_grid_pose50la.h"
// Native-only water selection lease. This is not a guest World400 store or a
// terrain3FC absence inference. The source height was pinned at World construction.
namespace ra_cold_aggregate50LA {
native_world_water50LA::Domain::Lease Owner::bindNativeWaterContext(
    const interactive_setup50DU::Context& context) noexcept {
    if(!nativeWaterDomain_||!source_||!source_->sourcesDecoded()||
       !prefix_.runtimeQueryOwners||!runtimeQueries_)return {};
    const auto current=[](const interactive_setup50DU::Context& c,
        const track_height_cold50JB::Owner& height,
        const native_world_water50LA::Domain& domain) noexcept -> bool {
        if(!c.isRaTrack||!c.raWorld||!c.consumedRaStage.race()||
           !c.raWorld->nativeWaterDomainMatches(domain)||
           c.originalMainApplication.entities()!=c.raWorld->worldApplication_||
           !c.raWorld->source_||!c.raWorld->source_->sourcesDecoded()||
           !c.raWorld->prefix_.runtimeQueryOwners||!c.raWorld->runtimeQueries_)return false;
        const auto& decoded=*c.raWorld->source_->sourcesDecoded();
        if(&c.actualHeight!=&height||&decoded.height!=&height||
           &c.actualWorld!=&decoded.world||!height.sourceBound||
           !height.descriptor.bound||!height.descriptor.waterFile110.defaulted||
           std::strcmp(height.descriptor.waterFile110.value.data(),"none")!=0||
           height.nativeWaterResources||height.waterNativeQueryBound||
           c.nativeGcWater||c.mcWaterOwner)return false;
        const auto noProvider=[](const rr::game::UndertrackPhysicsDomain50EW& selected) noexcept {
            return selected.water334==rr::game::UndertrackHeightOwner50EW::Absent&&
                !selected.waterProvider334&&!selected.waterGlobals&&!selected.terrain348Lift1F0;
        };
        // Enum absence is merely a consistency check; both actual owning slots
        // and both actual selected/source provider/global borrows are read above/below.
        return noProvider(height.sourceDomain)&&noProvider(c.selectedRuntimeHeightDomain);
    };
    return nativeWaterDomain_->bind(nativeWaterDomain_,context,
        source_->sourcesDecoded()->height,current);
}
}
namespace interactive_setup50DU {
namespace platform=rr::platform;
namespace gcLighting=rr::game::original_gc_gameplay_lighting_candidate;
namespace f=runtime_startup50fl;
namespace recovery50FI=checkpoint_recovery50fi_fixture;
namespace podTextures=pod_materials50ed_fixture;
using namespace rr::game;
using interactive_frame50DU::BorrowedCall;
using interactive_continuation50DU::DynamicActors50IW;
using interactive_loop50DU::RacerContactTotals50HV;
using interactive_loop50DU::RacePresentation50HZ;
using interactive_loop50DU::GameplayDisplayWallTotals50DU;
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
constexpr std::uint32_t raceRacerCapacity=rr::game::defaultHostRaceRacers50HT;
constexpr std::uint32_t raceOpponentCapacity=raceRacerCapacity-1;
}
SessionExit50DU Context::run() {
 auto& actualHeight=this->actualHeight;
 auto& actualWorld=this->actualWorld;
 auto& allAiControllerSamples=this->allAiControllerSamples;
 auto& allAiEnabled=this->allAiEnabled;
 auto& allAiHostDriverCalls=this->allAiHostDriverCalls;
 auto& allAiKeyboardPackets=this->allAiKeyboardPackets;
 auto& animatedCandidateFirst=this->animatedCandidateFirst;
 auto& applicationClock=this->applicationClock;
 auto& audioContactMaterials=this->audioContactMaterials;
 auto& authored=this->authored;
 auto& authoredFinishSteering10A4=this->authoredFinishSteering10A4;
 auto& authoredTrackName=this->authoredTrackName;
 auto& boostCoolingCF8=this->boostCoolingCF8;
 auto& boostDiagnostic=this->boostDiagnostic;
 auto& boostParameters=this->boostParameters;
 auto& boostState=this->boostState;
 auto& cameraStartup350=this->cameraStartup350;
 auto& candidates=this->candidates;
 auto& captureFirstWrongWay=this->captureFirstWrongWay;
 auto& capturePath=this->capturePath;
 auto& captureRun=this->captureRun;
 auto& capturedFirstWrongWay=this->capturedFirstWrongWay;
 auto& checkpointInput=this->checkpointInput;
 auto& checkpointOutput=this->checkpointOutput;
 auto& coldLoadDiagnostics=this->coldLoadDiagnostics;
 auto& collision=this->collision;
 auto& collisionNormalMode=this->collisionNormalMode;
 auto& consumedGaStage=this->consumedGaStage;
 auto& consumedGcStage=this->consumedGcStage;
 auto& consumedMaStage=this->consumedMaStage;
 auto& consumedMbStage=this->consumedMbStage;
 auto& consumedRaStage=this->consumedRaStage;
 auto& consumedSaStage=this->consumedSaStage;
 auto& consumedTaStage=this->consumedTaStage;
 auto& controlPresets=this->controlPresets;
 auto& courseColdLoad=this->courseColdLoad;
 auto& courseRaceCold=this->courseRaceCold;
 auto& courseRaceLaunched=this->courseRaceLaunched;
 auto& deviceIndex=this->deviceIndex;
 auto& directOriginalHuman=this->directOriginalHuman;
 auto& directOriginalInputs=this->directOriginalInputs;
 auto& displayWallTotals=this->displayWallTotals;
 auto& dynamicActorResolver=this->dynamicActorResolver;
 auto& dynamicActors=this->dynamicActors;
 auto& dynamicConsumerFlags11050=this->dynamicConsumerFlags11050;
 auto& dynamicContactEnabled=this->dynamicContactEnabled;
 auto& dynamicDamageCursor=this->dynamicDamageCursor;
 auto& dynamicEventBindings=this->dynamicEventBindings;
 auto& dynamicEventCursor=this->dynamicEventCursor;
 auto& dynamicEvents=this->dynamicEvents;
 auto& dynamicFinalTuning=this->dynamicFinalTuning;
 auto& dynamicLapCursor=this->dynamicLapCursor;
 auto& dynamicMusicCursor=this->dynamicMusicCursor;
 auto& dynamicMusicProfile=this->dynamicMusicProfile;
 auto& dynamicPeers=this->dynamicPeers;
 auto& dynamicPodEffect=this->dynamicPodEffect;
 auto& dynamicProfile=this->dynamicProfile;
 auto& dynamicRacer60=this->dynamicRacer60;
 auto& dynamicRegistry=this->dynamicRegistry;
 auto& dynamicSelfSlot=this->dynamicSelfSlot;
 auto& dynamicTuning=this->dynamicTuning;
 auto& freshMainProbe=this->freshMainProbe;
 auto& freshProbe100=this->freshProbe100;
 auto& freshTerrainProbe=this->freshTerrainProbe;
 auto& gaAnimatedMembership=this->gaAnimatedMembership;
 auto& gaAnimatedWorld=this->gaAnimatedWorld;
 auto& gaAnimationDiagnostic=this->gaAnimationDiagnostic;
 auto& gaOriginalDisc=this->gaOriginalDisc;
 auto& gaOriginalWorld=this->gaOriginalWorld;
 auto& gaTriggerActions=this->gaTriggerActions;
 auto& gaTriggerAuthoring=this->gaTriggerAuthoring;
 auto& gaTriggerCandidateFirst=this->gaTriggerCandidateFirst;
 auto& gaTriggerMembership=this->gaTriggerMembership;
 auto& gbAiMetadata=this->gbAiMetadata;
 auto& gbAiView=this->gbAiView;
 auto& gbAnimationBinding=this->gbAnimationBinding;
 auto& gbAnimationBindingDiagnostic=this->gbAnimationBindingDiagnostic;
 auto& gbAnimationRuntime=this->gbAnimationRuntime;
 auto& gbColdSources=this->gbColdSources;
 auto& gbCollisionWorld=this->gbCollisionWorld;
 auto& gbFogOwner=this->gbFogOwner;
 auto& gbLightOwner=this->gbLightOwner;
 auto& gbLiveAudioPrepared=this->gbLiveAudioPrepared;
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
 auto& gbPreparedAudioRoot=this->gbPreparedAudioRoot;
 auto& gbQueryBinding=this->gbQueryBinding;
 const auto& gbRemainingPayloadBytes=this->gbRemainingPayloadBytesCall;
 auto& gbSoundMode=this->gbSoundMode;
 auto& gbTrackParticles=this->gbTrackParticles;
 auto& gbTriggerActions=this->gbTriggerActions;
 auto& gbTriggerAuthoring=this->gbTriggerAuthoring;
 auto& gbWorldBorrow=this->gbWorldBorrow;
 auto& gcAnimatedMembership=this->gcAnimatedMembership;
 auto& gcAnimatedWorld=this->gcAnimatedWorld;
 auto& gcAnimationDiagnostic=this->gcAnimationDiagnostic;
 auto& gcNativeGate=this->gcNativeGate;
 auto& gcNativeWorldSelected=this->gcNativeWorldSelected;
 auto& gcOriginalDisc=this->gcOriginalDisc;
 auto& gcOriginalWorld=this->gcOriginalWorld;
 auto& gcTriggerActions=this->gcTriggerActions;
 auto& gcTriggerAuthoring=this->gcTriggerAuthoring;
 auto& gcTriggerCandidateFirst=this->gcTriggerCandidateFirst;
 auto& gcTriggerMembership=this->gcTriggerMembership;
 auto& ground=this->ground;
 auto& groundStack=this->groundStack;
 auto& headlessLive=this->headlessLive;
 auto& host=this->host;
 auto& hostDiagnostic=this->hostDiagnostic;
 auto& humanControlInput=this->humanControlInput;
 auto& humanCourseEnabled=this->humanCourseEnabled;
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
 auto& lastCommittedWrongWay=this->lastCommittedWrongWay;
 auto& legacyGround=this->legacyGround;
 auto& liveAudio=this->liveAudio;
 auto& loadedAudioSettings=this->loadedAudioSettings;
 auto& loadedTrack=this->loadedTrack;
 auto& m=this->m;
 auto& maOriginalDisc=this->maOriginalDisc;
 auto& maOriginalWorld=this->maOriginalWorld;
 auto& maTriggerActions=this->maTriggerActions;
 auto& maTriggerAuthoring=this->maTriggerAuthoring;
 auto& maTriggerCandidateFirst=this->maTriggerCandidateFirst;
 auto& maTriggerMembership=this->maTriggerMembership;
 auto& mainPairCapacity=this->mainPairCapacity;
 auto& managerRuntimeSelection=this->managerRuntimeSelection;
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
 auto& mcParticleWindow=this->mcParticleWindow;
 auto& mcQueryBinding=this->mcQueryBinding;
 const auto& mcRemainingPayloadBytes=this->mcRemainingPayloadBytesCall;
 auto& mcTrackParticles=this->mcTrackParticles;
 auto& mcTriggerActions=this->mcTriggerActions;
 auto& mcWaterOwner=this->mcWaterOwner;
 auto& mcWaterUpdateSequence=this->mcWaterUpdateSequence;
 auto& mcWorldBorrow=this->mcWorldBorrow;
 auto& mode1Enabled=this->mode1Enabled;
 auto& mode1Requested=this->mode1Requested;
 auto& monotonicNow50DU=*this->monotonicNow50DU;
 auto& movement=this->movement;
 auto& nativeFireSerial=this->nativeFireSerial;
 auto& nativeGaSelection=this->nativeGaSelection;
 auto& nativeGbSelection=this->nativeGbSelection;
 auto& nativeGcWater=this->nativeGcWater;
 auto& nativeMaSelection=this->nativeMaSelection;
 auto& nativeMbSelection=this->nativeMbSelection;
 auto& nativeMcSelection=this->nativeMcSelection;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
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
 const auto& openNativeVideo=this->openNativeVideoCall;
 auto& originalAi=this->originalAi;
 auto& originalAiAuthoringDiagnostic=this->originalAiAuthoringDiagnostic;
 auto& originalAiCreateSelection=this->originalAiCreateSelection;
 auto& originalAiEnabled=this->originalAiEnabled;
 auto& originalAiHumanCount=this->originalAiHumanCount;
 auto& originalAiMapMetadata=this->originalAiMapMetadata;
 auto& originalAiMetadata=this->originalAiMetadata;
 auto& originalAiOpponentCount=this->originalAiOpponentCount;
 const auto& originalAiOwnsSlot=this->originalAiOwnsSlotCall;
 auto& originalAiReceipt=this->originalAiReceipt;
 auto& originalAiSession=this->originalAiSession;
 auto& originalAiSettings1125C=this->originalAiSettings1125C;
 auto& originalAiTuning=this->originalAiTuning;
 auto& originalCameraEnabled=this->originalCameraEnabled;
 auto& originalMainApplication=this->originalMainApplication;
 auto& originalRecoveryEnabled=this->originalRecoveryEnabled;
 auto& originalUiActive=this->originalUiActive;
 auto& originalUiStarted=this->originalUiStarted;
 auto& phaseTiming=this->phaseTiming;
 auto& playerActionIndexFFC=this->playerActionIndexFFC;
 auto& playerArchive=this->playerArchive;
 auto& playerPodId=this->playerPodId;
 auto& playerRacer60=this->playerRacer60;
 auto& playerSource5C=this->playerSource5C;
 auto& playerUsesRosterRig=this->playerUsesRosterRig;
 auto& pose=this->pose;
 auto& qfilter=this->qfilter;
 auto& query=this->query;
 auto& queryRecords=this->queryRecords;
 auto& raDiagnostic=this->raDiagnostic;
 auto& raFreshProbe=this->raFreshProbe;
 auto& raRuntime=this->raRuntime;
 auto& raWorld=this->raWorld;
 auto& raceEnabled=this->raceEnabled;
 auto& raceOpponentCount=this->raceOpponentCount;
 auto& raceRacerCount=this->raceRacerCount;
 auto& racerCables=this->racerCables;
 auto& racerCollisions=this->racerCollisions;
 auto& racerContactsEnabled=this->racerContactsEnabled;
 auto& racerHudNames=this->racerHudNames;
 auto& racerNames=this->racerNames;
 auto& racerRigs=this->racerRigs;
 auto& racerVisuals=this->racerVisuals;
 auto& rcColdSources=this->rcColdSources;
 auto& rcCollisionWorld=this->rcCollisionWorld;
 auto& rcLightOwner=this->rcLightOwner;
 auto& rcNavigationBorrow=this->rcNavigationBorrow;
 auto& rcOriginalDisc=this->rcOriginalDisc;
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
 auto& recoveryGlobals=this->recoveryGlobals;
 auto& recoveryQueryOwners=this->recoveryQueryOwners;
 auto& renderDiagnostics=this->renderDiagnostics;
 auto& reportHost50DU=*this->reportHost50DU;
 auto& reportRenderer50DU=*this->reportRenderer50DU;
 auto& requestedWindowSamples=this->requestedWindowSamples;
 const auto& reserveGbAdditionalPayload=this->reserveGbAdditionalPayloadCall;
 const auto& reserveMcAdditionalPayload=this->reserveMcAdditionalPayloadCall;
 const auto& reserveRcAdditionalPayload=this->reserveRcAdditionalPayloadCall;
 const auto& reserveTbAdditionalPayload=this->reserveTbAdditionalPayloadCall;
 auto& resetGlobals=this->resetGlobals;
 auto& resetLifecycle=this->resetLifecycle;
 auto& response=this->response;
 auto& s=this->s;
 auto& saAnimatedWorld=this->saAnimatedWorld;
 auto& saAnimationDiagnostic=this->saAnimationDiagnostic;
 auto& saOriginalDisc=this->saOriginalDisc;
 auto& saOriginalWorld=this->saOriginalWorld;
 auto& saTriggerActions=this->saTriggerActions;
 auto& saTriggerAuthoring=this->saTriggerAuthoring;
 auto& saTriggerCandidateFirst=this->saTriggerCandidateFirst;
 auto& saTriggerMembership=this->saTriggerMembership;
 auto& scColdSources=this->scColdSources;
 auto& scGround=this->scGround;
 auto& scOriginalDisc=this->scOriginalDisc;
 auto& scTriggerActions=this->scTriggerActions;
 auto& scTriggerAuthoring=this->scTriggerAuthoring;
 auto& scTriggerCandidateFirst=this->scTriggerCandidateFirst;
 auto& scTriggerMembership=this->scTriggerMembership;
 auto& scratchRecords=this->scratchRecords;
 auto& secondaryFilter=this->secondaryFilter;
 auto& secondaryPairCapacity=this->secondaryPairCapacity;
 auto& secondaryQuery=this->secondaryQuery;
 auto& secondaryQueryRecords=this->secondaryQueryRecords;
 auto& secondaryQueryResource=this->secondaryQueryResource;
 auto& secondaryRecords=this->secondaryRecords;
 auto& secondaryScratchRecords=this->secondaryScratchRecords;
 auto& secondarySpring=this->secondarySpring;
 auto& selectedAudioSettings=this->selectedAudioSettings;
 auto& selectedOriginal=this->selectedOriginal;
 auto& selectedRacer60=this->selectedRacer60;
 auto& selectedRuntimeHeightDomain=this->selectedRuntimeHeightDomain;
 auto& selectedTrack=this->selectedTrack;
 auto& selection=this->selection;
 auto& sessionIndex=this->sessionIndex;
 auto& sessionRandom=this->sessionRandom;
 auto& settings=this->settings;
 auto& sourceConsumedIntroActivation=this->sourceConsumedIntroActivation;
 auto& sourceConsumedPauseActivation=this->sourceConsumedPauseActivation;
 auto& sourceDisplayLifetime=this->sourceDisplayLifetime;
 auto& sourceModelRenderGlobals=this->sourceModelRenderGlobals;
 auto& sourcePause=this->sourcePause;
 auto& sourcePauseObservation=this->sourcePauseObservation;
 auto& sourcePauseParentAttached=this->sourcePauseParentAttached;
 auto& sourcePauseTrack=this->sourcePauseTrack;
 auto& sourcePositionStatsPrestage=this->sourcePositionStatsPrestage;
 auto& sourcePositionStatsPrestageDiagnostic=this->sourcePositionStatsPrestageDiagnostic;
 auto& sourceStaticCandidates=this->sourceStaticCandidates;
 auto& spring=this->spring;
 auto& stackCount=this->stackCount;
 auto& startAtMenu=this->startAtMenu;
 auto& taOriginalDisc=this->taOriginalDisc;
 auto& taOriginalWorld=this->taOriginalWorld;
 auto& tbAiMetadata=this->tbAiMetadata;
 auto& tbAiView=this->tbAiView;
 auto& tbColdSources=this->tbColdSources;
 auto& tbCollisionWorld=this->tbCollisionWorld;
 auto& tbDoorBinding=this->tbDoorBinding;
 auto& tbDoorBindingDiagnostic=this->tbDoorBindingDiagnostic;
 auto& tbDoorDiagnostic=this->tbDoorDiagnostic;
 auto& tbDoorPrefixArmed=this->tbDoorPrefixArmed;
 auto& tbDoorRuntime=this->tbDoorRuntime;
 auto& tbLightOwner=this->tbLightOwner;
 auto& tbLiveAudioPrepared=this->tbLiveAudioPrepared;
 auto& tbMapMetadata=this->tbMapMetadata;
 auto& tbMapView=this->tbMapView;
 auto& tbOriginalDisc=this->tbOriginalDisc;
 auto& tbPodSound=this->tbPodSound;
 auto& tbPreparedAudioRoot=this->tbPreparedAudioRoot;
 const auto& tbRemainingPayloadBytes=this->tbRemainingPayloadBytesCall;
 auto& tbSoundMode=this->tbSoundMode;
 auto& tbTriggerActions=this->tbTriggerActions;
 auto& tbTriggerAuthoring=this->tbTriggerAuthoring;
 auto& tbWorldBorrow=this->tbWorldBorrow;
 auto& tcAiMetadata=this->tcAiMetadata;
 auto& tcAiView=this->tcAiView;
 auto& tcColdSources=this->tcColdSources;
 auto& tcCollisionWorld=this->tcCollisionWorld;
 auto& tcGateDiagnostic=this->tcGateDiagnostic;
 auto& tcGatePrefixArmed=this->tcGatePrefixArmed;
 auto& tcGateRuntime=this->tcGateRuntime;
 auto& tcGateVisibility=this->tcGateVisibility;
 auto& tcGateVisibilityDiagnostic=this->tcGateVisibilityDiagnostic;
 auto& tcGlobalSound=this->tcGlobalSound;
 auto& tcLightOwner=this->tcLightOwner;
 auto& tcLiveAudioPrepared=this->tcLiveAudioPrepared;
 auto& tcMapMetadata=this->tcMapMetadata;
 auto& tcMapView=this->tcMapView;
 auto& tcOriginalDisc=this->tcOriginalDisc;
 auto& tcTriggerActions=this->tcTriggerActions;
 auto& tcWorldBorrow=this->tcWorldBorrow;
 auto& terrainBindings=this->terrainBindings;
 auto& terrainOwner=this->terrainOwner;
 auto& terrainOwnerCount=this->terrainOwnerCount;
 auto& tournamentLaunchSelection=this->tournamentLaunchSelection;
 auto& tournamentLaunchStamp=this->tournamentLaunchStamp;
 auto& tournamentResultArmed=this->tournamentResultArmed;
 auto& tournamentResultCommitted=this->tournamentResultCommitted;
 auto& trackAdmission=this->trackAdmission;
 auto& trackBindings=this->trackBindings;
 auto& trackCounts=this->trackCounts;
 auto& trackSession=this->trackSession;
 auto& trackStorage=this->trackStorage;
 auto& tuning=this->tuning;
 auto& usePhysicalGameController=this->usePhysicalGameController;
 auto& x=this->x;
 // The actual guards remain early; these views have no destructor action.
 struct ArmedFlagView { bool& armed; };
 ArmedFlagView tbDoorPrefixGuard{tbDoorPrefixArmed},tcGatePrefixGuard{tcGatePrefixArmed};
 const auto srays=secondaryPairCapacity;
 std::vector<SpringCumulativePair48N> sip(srays),ssp(srays),sdp(srays);std::vector<Type2PairMetadata49X> sim(srays),ssm(srays),sdm(srays);
 std::vector<TerrainCandidateEffects48J> sse(candidates.size()),sde(candidates.size());std::vector<Type2WorkItem49X> ss(stackCount+secondaryQueryResource.maxDepth);
 Type2StaticInitial49X si{};si.records=secondarySpring.records;si.pairs=sip.data();si.metadata=sim.data();si.pairSlots=srays;si.recordSlots=secondaryRecords;si.recordCount=60;
 Type2StaticWorkspace49X sw{ssp.data(),srays,ssm.data(),srays,secondaryScratchRecords,60,sse.data(),sse.size(),ss.data(),ss.size(),nullptr,0};
 Type2StaticDestination49X sd{sdp.data(),srays,sdm.data(),srays,secondaryQueryRecords,60,sde.data(),sde.size()};Type2StaticResult49X secondaryResult{};
 SpringGeneratedParameters48K secondarySettings{};secondarySettings.impact54=true;secondarySettings.impact.disableLeverArms5C=false;secondarySettings.impact.correctPenetration64=true;secondarySettings.impact.normalForceScale98=1;secondarySettings.impact.positionCorrectionScaleCC=.025f;
 SpringResponseParameters48L secondaryResponse{};secondaryResponse.maxSpeed94=10000;
 require50DU(rr::assets::applySpringCollision50F(collision,x.parameters.bodies.secondary.body.mass,secondarySettings,secondaryResponse),"authored secondary response settings");
 const auto rays=mainPairCapacity;
 std::vector<SpringCumulativePair48N> initialPairs(rays),scratchPairs(rays),pairs(rays);std::vector<Type2PairMetadata49X> initialMetadata(rays),scratchMetadata(rays),metadata(rays);
 std::vector<TerrainCandidateEffects48J> scratchEffects(candidates.size()),effects(candidates.size());std::vector<Type2WorkItem49X> stack(stackCount);
 Type2StaticInitial49X initial{};initial.records=spring.records;initial.pairs=initialPairs.data();initial.metadata=initialMetadata.data();initial.pairSlots=rays;initial.recordSlots=records;initial.recordCount=40;
 Type2StaticWorkspace49X workspace{scratchPairs.data(),rays,scratchMetadata.data(),rays,scratchRecords,40,scratchEffects.data(),scratchEffects.size(),stack.data(),stack.size(),nullptr,0};
 Type2StaticDestination49X destination{pairs.data(),rays,metadata.data(),rays,queryRecords,40,effects.data(),effects.size()};Type2StaticResult49X queryResult{};

 auto& movementCandidates=trackStorage.movement;
 MovementRays50N movementInitial,movementScratch,movementDestination;SpringVector4_48N endpoints[2]{};std::uint32_t endpointScenes[2]{};
 std::vector<TerrainCandidateEffects48J> movementScratchEffects(candidates.size()),movementEffects(candidates.size());std::vector<std::uint32_t> movementStack(stackCount);
 Type3QuerySource48P movementSource{movementInitial.source(),endpoints,2,endpointScenes,2,nullptr,0};
 Type3QueryWorkspace48P movementWorkspace{{movementScratch.view(),nullptr,0,movementScratchEffects.data(),movementScratchEffects.size()},movementStack.data(),movementStack.size()};
 Type3QueryBuffers48P movementOutput{movementDestination.view(),nullptr,0,movementEffects.data(),movementEffects.size()};Type3QueryResult48P movementResult{};std::uint32_t movementFlag=0;
 // Preserve the separate ordered movement registry, prepared once before
 // publication. Hot gather copies only through capacity-checked native views.
 auto& registry=trackStorage.movementRegistry;
 auto* registeredMovement=trackStorage.registeredMovement.data();
 auto* registeredIndices=trackStorage.registeredIndices.data();
 auto& registeredCount=trackStorage.registeredCount;
 auto* probeIndices=trackStorage.probeIndices.data();auto& probeCount=trackStorage.probeCount;
 auto& registrationBlocks=trackStorage.movementBlocks;
 auto& registrationView=trackStorage.movementView;
 auto gatherRegistered=[&](){return trackStorage.gatherMovement();};

 // Source spring constructor295424 ->11B570(query,2): enable querying,
 // disable target participation and temporary-registration bits. Both spring
 // query objects may be registered without becoming their own static targets.
 qfilter.flagsF8=static_cast<std::uint8_t>((qfilter.flagsF8&~0x0eu)|2u);
 secondaryFilter.flagsF8=static_cast<std::uint8_t>((secondaryFilter.flagsF8&~0x0eu)|2u);
 // The isolated corpus cases both used identity100. These are now two live
 // objects in one collector table, so assign a distinct opaque native identity
 // to secondary; this is not a recovered guest address or a collision filter.
 constexpr std::uint32_t secondaryNativeIdentity=native_query_identities50KX::secondary364;
 require50DU(qfilter.identity!=secondaryNativeIdentity&&movement.filter.identity!=secondaryNativeIdentity,
     "reserved secondary query identity conflicts with another native query");
 for(const auto& candidate:candidates)require50DU(candidate.candidate.collision.filter.identity!=secondaryNativeIdentity,
     "reserved secondary query identity conflicts with terrain");
 secondaryFilter.identity=secondaryNativeIdentity;
 if(originalRecoveryEnabled){
  using Categories=OriginalNativeCollisionCategories;
  require50DU(terrainBindings.physicsBodyCategory==Categories::physicsBody&&
      terrainBindings.staticObjectCategory==Categories::staticObject&&
      terrainBindings.floorCategory==Categories::floor&&terrainBindings.tubeCategory==Categories::tube&&
      query.skipMask==Categories::trigger&&query.materialMask==Categories::staticObject,
      "actual native collision category namespace differs before binding source POD");
  const auto& globals=recoveryGlobals;
  std::string why;
  require50DU(undertrack_owners50ew_fixture::prepareStorage(recoveryQueryOwners,globals,
      trackCounts.membership,why),why.empty()?"actual694/7A4 cold storage rejected":why.c_str());
  // Source212918..21299C reciprocal first-list exclusions. Sizes/backing are
  // fixed before bank sealing; selected identities are filled once per racer.
  qfilter.identityLists[0].mode=secondaryFilter.identityLists[0].mode=1;
  qfilter.identityLists[0].identities.resize(4);
  secondaryFilter.identityLists[0].identities.resize(3);
  qfilter.identityLists[1].mode=secondaryFilter.identityLists[1].mode=1;
  qfilter.identityLists[1].identities.clear();secondaryFilter.identityLists[1].identities.clear();
  // Bind existing7A4 source constructor fields, not a second movement query.
  // Its pre-existing limited-list callback policy is unchanged here.
  movement.filter.categoryA4=Categories::physicsBody;movement.filter.acceptMaskB8=0xffffffffu;
  movement.filter.flags31=static_cast<std::uint8_t>((movement.filter.flags31&0xf0u)|3u);
  movement.filter.flagsF8=static_cast<std::uint8_t>((movement.filter.flagsF8&0xc0u)|2u);
  movement.filter.categoryModeD4=0;movement.filter.categoryMasks={Categories::staticObject};
  for(auto& list:movement.filter.identityLists){list.mode=1;list.identities.clear();}
  movement.copied40=movement.copied50={{0.f,0.f,0.f,1.f}};
 }

 // Static references preserve authored candidate order; three real query
 // owners follow them. No fixed TA zone/collider dimensions remain here.
 const CollectionRef50BX mainQueryRef=trackCounts.mainQuery,
     secondaryQueryRef=trackCounts.secondaryQuery,terrainQueryRef=trackCounts.terrainQuery;
 const auto collectionObjectCount=trackCounts.objects,zoneCount=trackCounts.zones;
 auto* collectionObjects=trackStorage.objects.data();
 auto* collectionSource=trackStorage.source.data();
 auto* collectionSecond=trackStorage.second.data();
 auto* treeSource=trackStorage.treeSource.data();auto* treeSecond=trackStorage.treeSecond.data();
 auto& zoneScratch=trackStorage.registrationScratch;
 auto& zoneLifecycle=trackStorage.lifecycle;auto& zoneQueue=trackStorage.queue;
 auto& liveWorld=trackStorage.world;
 bool rcCollisionCleanupDone=!isRcTrack;
 auto closeRcCurrentCollision=[&]() noexcept -> bool {
  if(!isRcTrack)return true;
  if(!rcWorldBorrow||rcWorldBorrow->world()!=&liveWorld||!rcTriggerActions||
     rcWorldBorrow->actions()!=rcTriggerActions||!rcTriggerActions->queryIdle())return false;
  auto remove=[&](LiveZoneMembership50EN& state,CollectionRef50BX expected){
   if(state.attached&&state.object!=expected)return false;
   const auto result=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,state,trackStorage.membershipWork);
   return result.status==LiveZoneStatus50EN::Complete&&!state.attached&&!state.object;
  };
  if(!remove(trackStorage.mainMembership.state,trackCounts.mainQuery)||
     !remove(trackStorage.secondaryMembership.state,trackCounts.secondaryQuery)||
     !remove(trackStorage.terrainMembership.state,trackCounts.terrainQuery)||
     !remove(recoveryQueryOwners.memberships[0],trackCounts.podQuery)||
     !remove(recoveryQueryOwners.memberships[1],trackCounts.movementQuery))return false;
  s.shape.registrationE0=0;
  for(std::size_t i=0;i<rc_trigger_authoring::count;++i){
   auto* membership=rcWorldBorrow->triggerMembership(i);
   if(!membership||!remove(membership->state,static_cast<CollectionRef50BX>(rc_collision_world50RC::sourceCandidates+i+1)))return false;
  }
  return true;
 };
 struct RcCurrentCollisionGuard {
  bool& done;decltype(closeRcCurrentCollision)& close;
  ~RcCurrentCollisionGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } rcCurrentCollisionGuard{rcCollisionCleanupDone,closeRcCurrentCollision};
 bool mcCollisionCleanupDone=!isMcTrack;
 auto closeMcCurrentCollision=[&]()noexcept->bool {
  if(!isMcTrack)return true;
  if(!mcWorldBorrow||mcWorldBorrow->world()!=&liveWorld||!mcTriggerActions||!mcTriggerActions->queryIdle()||zoneQueue.count)return false;
  auto remove=[&](LiveZoneMembership50EN& state,CollectionRef50BX expected){
   if(state.attached&&state.object!=expected)return false;
   const auto r=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,state,trackStorage.membershipWork);
   return r.status==LiveZoneStatus50EN::Complete&&!state.attached&&!state.object;
  };
  if(!remove(trackStorage.mainMembership.state,trackCounts.mainQuery)||
     !remove(trackStorage.secondaryMembership.state,trackCounts.secondaryQuery)||
     !remove(trackStorage.terrainMembership.state,trackCounts.terrainQuery)||
     !remove(recoveryQueryOwners.memberships[0],trackCounts.podQuery)||
     !remove(recoveryQueryOwners.memberships[1],trackCounts.movementQuery))return false;
  s.shape.registrationE0=0;
  for(std::size_t i=0;i<21;++i){auto* member=mcWorldBorrow->triggerMembership(i);
   if(!member||!remove(member->state,static_cast<CollectionRef50BX>(90+i)))return false;}
  return true;
 };
 struct McCurrentCollisionGuard {
  bool& done;decltype(closeMcCurrentCollision)& close;
  ~McCurrentCollisionGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } mcCurrentCollisionGuard{mcCollisionCleanupDone,closeMcCurrentCollision};
 bool gbCollisionCleanupDone=!isGbTrack;
 auto closeGbCurrentCollision=[&]()noexcept->bool {
  if(!isGbTrack)return true;
  if(!gbWorldBorrow||gbWorldBorrow->world()!=&liveWorld||!gbTriggerActions||!gbTriggerActions->queryIdle()||zoneQueue.count)return false;
  auto remove=[&](LiveZoneMembership50EN& state,CollectionRef50BX expected){
   if(state.attached&&state.object!=expected)return false;
   const auto r=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,state,trackStorage.membershipWork);
   return r.status==LiveZoneStatus50EN::Complete&&!state.attached&&!state.object;
  };
  if(!remove(trackStorage.mainMembership.state,trackCounts.mainQuery)||
     !remove(trackStorage.secondaryMembership.state,trackCounts.secondaryQuery)||
     !remove(trackStorage.terrainMembership.state,trackCounts.terrainQuery)||
     !remove(recoveryQueryOwners.memberships[0],trackCounts.podQuery)||
     !remove(recoveryQueryOwners.memberships[1],trackCounts.movementQuery))return false;
  s.shape.registrationE0=0;
  for(std::size_t i=0;i<18;++i){auto* member=gbWorldBorrow->triggerMembership(i);
   if(!member||!remove(member->state,static_cast<CollectionRef50BX>(616+i)))return false;}
  return true;
 };
 struct GbCurrentCollisionGuard {
  bool& done;decltype(closeGbCurrentCollision)& close;
  ~GbCurrentCollisionGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } gbCurrentCollisionGuard{gbCollisionCleanupDone,closeGbCurrentCollision};
 bool tbCollisionCleanupDone=!isTbTrack;
 auto closeTbCurrentCollision=[&]() noexcept -> bool {
  if(!isTbTrack)return true;
  if(!tbWorldBorrow||tbWorldBorrow->world()!=&liveWorld||!tbTriggerActions||
     tbWorldBorrow->actions()!=tbTriggerActions.get()||!tbTriggerActions->queryIdle())return false;
  auto remove=[&](LiveZoneMembership50EN& state,CollectionRef50BX expected){
   if(state.attached&&state.object!=expected)return false;
   const auto result=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,state,trackStorage.membershipWork);
   return result.status==LiveZoneStatus50EN::Complete&&!state.attached&&!state.object;
  };
  if(!remove(trackStorage.mainMembership.state,trackCounts.mainQuery)||
     !remove(trackStorage.secondaryMembership.state,trackCounts.secondaryQuery)||
     !remove(trackStorage.terrainMembership.state,trackCounts.terrainQuery)||
     !remove(recoveryQueryOwners.memberships[0],trackCounts.podQuery)||
     !remove(recoveryQueryOwners.memberships[1],trackCounts.movementQuery))return false;
  s.shape.registrationE0=0;
  for(std::size_t i=0;i<tb_trigger_authoring50TB::count;++i){
   auto* membership=tbWorldBorrow->triggerMembership(i);
   if(!membership||!remove(membership->state,static_cast<CollectionRef50BX>(tb_collision_world50TB::sourceCandidates+i+1)))return false;
  }
  return true;
 };
 struct TbCurrentCollisionGuard {
  bool& done;decltype(closeTbCurrentCollision)& close;
  ~TbCurrentCollisionGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } tbCurrentCollisionGuard{tbCollisionCleanupDone,closeTbCurrentCollision};
 bool tcCollisionCleanupDone=!isTcTrack;
 auto closeTcCurrentCollision=[&]() noexcept -> bool {
  if(!isTcTrack)return true;
  if(!tcWorldBorrow||tcWorldBorrow->world()!=&liveWorld||tcTriggerActions->nativeGroup().queryActive)return false;
  auto remove=[&](LiveZoneMembership50EN& state,CollectionRef50BX expected){
   if(state.attached&&state.object!=expected)return false;
   const auto result=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,state,trackStorage.membershipWork);
   return result.status==LiveZoneStatus50EN::Complete&&!state.attached&&!state.object;
  };
  if(!remove(trackStorage.mainMembership.state,trackCounts.mainQuery)||
     !remove(trackStorage.secondaryMembership.state,trackCounts.secondaryQuery)||
     !remove(trackStorage.terrainMembership.state,trackCounts.terrainQuery)||
     !remove(recoveryQueryOwners.memberships[0],trackCounts.podQuery)||
     !remove(recoveryQueryOwners.memberships[1],trackCounts.movementQuery))return false;
  s.shape.registrationE0=0;
  for(std::size_t i=0;i<tc_trigger_authoring50TC::count;++i){
   auto* membership=tcWorldBorrow->triggerMembership(i);
   if(!membership||!remove(membership->state,static_cast<CollectionRef50BX>(tc_collision_world50TC::sourceCandidates+i+1)))return false;
  }
  return true;
 };
 struct TcCurrentCollisionGuard {
  bool& done;decltype(closeTcCurrentCollision)& close;
  ~TcCurrentCollisionGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } tcCurrentCollisionGuard{tcCollisionCleanupDone,closeTcCurrentCollision};
 bool scMembershipCleanupDone=!isScTrack;
 auto closeScCurrentMemberships=[&]() noexcept -> bool {
  if(!isScTrack)return true;
  for(std::size_t i=0;i<scTriggerMembership.size();++i){auto* membership=scTriggerMembership[i].get();
   if(!membership)return false;
   const auto expected=static_cast<CollectionRef50BX>(scTriggerCandidateFirst+i+1);
   if(membership->state.attached&&membership->state.object!=expected)return false;
   const auto result=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership->state,trackStorage.membershipWork);
   if(result.status!=LiveZoneStatus50EN::Complete||membership->state.attached||membership->state.object)return false;
  }return true;
 };
 // Handles partial attachment before a Bank exists. The later Bank guard
 // closes every real saved slot and marks this same completion flag.
 struct ScCurrentMembershipGuard {
  bool& done;decltype(closeScCurrentMemberships)& close;
  ~ScCurrentMembershipGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } scCurrentMembershipGuard{scMembershipCleanupDone,closeScCurrentMemberships};
 // Cold original TunnelTrack owners, separate from the grid-start projection.
 // Share the existing serial source-libm owner with49T; keep every node/NextN.
 auto legacyNavigationMap=(isRaTrack||isMbTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack)?nullptr:std::make_unique<TrackNavigation50EU>();
 auto legacyNavigationAI=(isRaTrack||isMbTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack)?nullptr:std::make_unique<TrackNavigation50EU>();
 const auto* navigationMap=(isMbTrack||isTcTrack||isTbTrack||isGbTrack||isMcTrack)?nullptr:isRcTrack?&rcNavigationBorrow.map():isRaTrack?&raRuntime->source.map:legacyNavigationMap.get();
 const auto* navigationAI=(isMbTrack||isTcTrack||isTbTrack||isGbTrack||isMcTrack)?nullptr:isRcTrack?&rcNavigationBorrow.ai():isRaTrack?&raRuntime->source.ai:legacyNavigationAI.get();
 // The root's final const views were issued once against this actual liveWorld,
 // outside Bank. All suspended ordinary recovery calls keep this same selection.
 const auto requireMbRuntimeNavigation50MB=[&](){
  if(isMcTrack){
   require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!navigationMap&&!navigationAI&&mcColdSources&&
       mcMapView.runtimeValid()&&mcAiView.runtimeValid()&&
       mcMapView.role()==NavigationRole50MC::Map&&mcAiView.role()==NavigationRole50MC::Ai&&
       mcMapView.sourceIdentity()==mcColdSources.get()&&mcAiView.sourceIdentity()==mcColdSources.get()&&
       mcMapView.sameOwner(mcAiView)&&mcMapView.sameWorld(liveWorld)&&mcAiView.sameWorld(liveWorld)&&
       mcMapMetadata.matches(mcMapView)&&mcAiMetadata.matches(mcAiView),
       "MC navigation requires its retained same-source final Map/Ai/metadata and actual liveWorld");
   return;
  }
  if(isGbTrack){
   require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!navigationMap&&!navigationAI&&gbColdSources&&
       gbMapView.runtimeValid()&&gbAiView.runtimeValid()&&
       gbMapView.role()==NavigationRole50GB::Map&&gbAiView.role()==NavigationRole50GB::Ai&&
       gbMapView.sourceIdentity()==gbColdSources.get()&&gbAiView.sourceIdentity()==gbColdSources.get()&&
       gbMapView.sameOwner(gbAiView)&&gbMapView.sameWorld(liveWorld)&&gbAiView.sameWorld(liveWorld)&&
       gbMapMetadata.matches(gbMapView)&&gbAiMetadata.matches(gbAiView),
       "GB navigation requires its retained same-source final Map/Ai/metadata and actual liveWorld");
   return;
  }
  if(isTbTrack){
   require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!navigationMap&&!navigationAI&&tbColdSources&&
       tbMapView.runtimeValid()&&tbAiView.runtimeValid()&&
       tbMapView.role()==NavigationRole50TB::Map&&tbAiView.role()==NavigationRole50TB::Ai&&
       tbMapView.sourceIdentity()==tbColdSources.get()&&tbAiView.sourceIdentity()==tbColdSources.get()&&
       tbMapView.sameOwner(tbAiView)&&tbMapView.sameWorld(liveWorld)&&tbAiView.sameWorld(liveWorld)&&
       tbMapMetadata.matches(tbMapView)&&tbAiMetadata.matches(tbAiView),
       "TB navigation requires its retained same-source final Map/Ai/metadata and actual liveWorld");
   return;
  }
  if(isRcTrack){
   require50DU(!isMbTrack&&!isTcTrack&&rcColdSources&&rcCollisionWorld&&rcWorldBorrow&&
       rcNavigationBorrow.valid()&&rcNavigationBorrow.sameWorld(liveWorld)&&
       rcNavigationBorrow.sourceIdentity()==rcColdSources.get()&&
       rcCollisionWorld->coldSource()==rcColdSources.get()&&rcWorldBorrow->world()==&liveWorld&&
       navigationMap==&rcNavigationBorrow.map()&&navigationAI==&rcNavigationBorrow.ai(),
       "RC navigation requires its retained final same-source borrow and actual liveWorld");
   return;
  }
  if(isTcTrack){
   require50DU(!isMbTrack&&!navigationMap&&!navigationAI&&tcMapView.runtimeValid()&&tcAiView.runtimeValid()&&
       tcMapView.role()==NavigationRole50TC::Map&&tcAiView.role()==NavigationRole50TC::Ai&&
       tcMapView.sameOwner(tcAiView)&&tcMapView.sameWorld(liveWorld)&&tcAiView.sameWorld(liveWorld)&&
       tcMapMetadata.matches(tcMapView)&&tcAiMetadata.matches(tcAiView),
       "TC navigation requires its same-source final Map/Ai/metadata and actual liveWorld");
   return;
  }
  if(!isMbTrack)return;
  require50DU(!navigationMap&&!navigationAI&&mbMapView.runtimeValid()&&mbAiView.runtimeValid()&&
      mbMapView.role()==NavigationRole50MB::Map&&mbAiView.role()==NavigationRole50MB::Ai&&
      mbMapView.sameOwner(mbAiView)&&mbMapView.sameWorld(liveWorld)&&mbAiView.sameWorld(liveWorld)&&
      mbMapMetadata.matches(mbMapView)&&mbAiMetadata.matches(mbAiView),
      "MB navigation requires its same-source final Map/Ai/metadata and actual liveWorld");
 };
 // RC's references remain bounded by the same const final borrow, outside Bank.
 if(isRcTrack)requireMbRuntimeNavigation50MB();
 const auto& selectedOriginalAiMapMetadata=isRcTrack?rcNavigationBorrow.mapMetadata():originalAiMapMetadata;
 const auto& selectedOriginalAiMetadata=isRcTrack?rcNavigationBorrow.aiMetadata():originalAiMetadata;
 const auto selectedMapCount50MB=[&](){
  requireMbRuntimeNavigation50MB();return isMcTrack?mcMapView.count():isGbTrack?gbMapView.count():isTbTrack?tbMapView.count():isTcTrack?tcMapView.count():isMbTrack?mbMapView.count():navigationMap->track.count;
 };
 const auto selectedAiCount50MB=[&](){
  requireMbRuntimeNavigation50MB();return isMcTrack?mcAiView.count():isGbTrack?gbAiView.count():isTbTrack?tbAiView.count():isTcTrack?tcAiView.count():isMbTrack?mbAiView.count():navigationAI->track.count;
 };
 const auto selectedMapNode50MB=[&](std::uint32_t index)->const UndertrackNode50ET&{
  if(isMcTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<mcMapView.count(),"MC map node is outside the actual source");
   return mcMapView.track().nodes()[index];}
  if(isGbTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<gbMapView.count(),"GB map node is outside the actual source");
   return gbMapView.track().nodes()[index];}
  if(isTbTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<tbMapView.count(),"TB map node is outside the actual source");
   return tbMapView.track().nodes()[index];}
  if(isTcTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<tcMapView.count(),"TC map node is outside the actual source");
   return tcMapView.track().nodes()[index];}
  if(isMbTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<mbMapView.count(),"MB map node is outside the actual source");
   return mbMapView.track().nodes()[index];}
  if(isRcTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<navigationMap->track.count,"RC map node is outside the actual source");}
  return navigationMap->track.nodes[index];
 };
 const auto selectedMapIndex50MB=[&](std::uint32_t index){
  if(isMcTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<mcMapView.count(),"MC map index is outside the actual source");
   return mcMapView.nodeIndex78()[index];}
  if(isGbTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<gbMapView.count(),"GB map index is outside the actual source");
   return gbMapView.nodeIndex78()[index];}
  if(isTbTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<tbMapView.count(),"TB map index is outside the actual source");
   return tbMapView.nodeIndex78()[index];}
  if(isTcTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<tcMapView.count(),"TC map index is outside the actual source");
   return tcMapView.nodeIndex78()[index];}
  if(isMbTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<mbMapView.count(),"MB map index is outside the actual source");
   return mbMapView.nodeIndex78()[index];}
  if(isRcTrack){requireMbRuntimeNavigation50MB();
   require50DU(index<navigationMap->track.count,"RC map index is outside the actual source");}
  return navigationMap->nodeIndex78[index];
 };
 const char* autoSetting=std::getenv("RR_AUTODRIVE");
 bool autoDriveEnabled=autoSetting&&std::string(autoSetting)=="1";
 require50DU(!autoDriveEnabled||allAiEnabled||humanControlInput.twinEngine,
     "legacy HOST course driver has no normal-preset input owner; use real human controls");
 require50DU(!nativeOriginalFleet||!autoDriveEnabled,
     "native Course ownership forbids HOST autodrive; only the real Player and original AI may drive");
 bool autoStopReported=false;
 rr::host::CourseSpeedLimits50GK courseLimits{};
 rr::host::CourseDriverTuning50GK courseTuning{};
 // Default automated demonstrations use authored speed limits. The previous
 // conservative host policy remains an explicit replay option, never a change
 // to native physics or the simulation clock.
 courseTuning.authoredSpeedOnly=true;
 const char* autoPace=std::getenv("RR_AUTODRIVE_PACE");
 if(autoPace){
  require50DU(std::string(autoPace)=="authored"||std::string(autoPace)=="conservative",
      "autodrive pace must be authored or conservative");
  courseTuning.authoredSpeedOnly=std::string(autoPace)=="authored";
 }
 // Original AI leaf173798..1737A4 compares desired speed against this
 // conversion times cached body speed. Reuse its units, not its unjoined
 // race/vehicle target selection. This host driver measures planar speed.
 if(courseTuning.authoredSpeedOnly)
  courseTuning.speedLimitToBodyScale=1.f/std::bit_cast<float>(std::uint32_t{0x3f2e8bc1u});
 rr::host::CourseDriverState50GK courseState{};
 rr::host::CourseDriverDiagnostic50GK courseDiagnostic{};
 rr::host::CourseDriverOutput50GK courseOutput{};
 if(track_session_assets50IG::codeEquals(trackSession->world->code,"GC")){
  require50DU(rr::host::readCourseSteeringLookahead50GK(
      reinterpret_cast<const char*>(tuning.data()),tuning.size(),
      courseTuning.authoredSteeringLookaheadSeconds,courseDiagnostic),
      "actual GC AIDriving.Lookahead unavailable for host steering preview");
  courseTuning.fullThrottleSteeringRange=true;
 }
 const char* autoPolarity=std::getenv("RR_AUTODRIVE_POLARITY");
 if(autoPolarity){
  require50DU(std::string(autoPolarity)=="1"||std::string(autoPolarity)=="-1","autodrive polarity must be 1 or -1");
  courseTuning.steeringPolarity=std::string(autoPolarity)=="-1"?-1.f:1.f;
 }
 WrongWayProgressContinuation50GO wrongWayContinuation{};
 WrongWayProgressReceipt50GO wrongWayReceipt{};
 WrongWayProgressDiagnostic50GO wrongWayDiagnostic{};
 std::uint64_t wrongWayUpdates=0,wrongWayActiveTicks=0;
 // Explicit native-host normal mode; no original app34C constructor is claimed.
 constexpr std::uint32_t applicationWrongWayBypass34C=0;
 lap_drive50GJ::Owner lapOwner;lapOwner.continuity=&lapContinuity;lapOwner.fields=&s.fields;
 OriginalRaceHud50KG sourceRaceHud; //Actual source text/gleam ownership; native gauges remain until joined.
 bool sourceRaceHudRendering=false;
 bool sourceRaceHudDisplayReported=false,sourceRaceHudRejectionReported=false;
 original_camera_cycle_audio50KX::Owner sourceCameraCycleAudio;
 auto raceAudio=std::make_unique<race_audio50HQ::Owner>();
 auto sourceAnnouncer=std::make_unique<source_announcer50IE::Owner>();
 auto raceMusic=std::make_unique<race_music50IE::Owner>();
 using StartMusicContext=interactive_publication50DU::StartMusicContext;
 StartMusicContext startMusicContext{raceMusic.get(),liveAudio.get(),&nativeOrdinaryLaunch11258};
 const auto requestStartMusic=+[](void* opaque,std::int32_t song,std::int32_t argument)noexcept {
  auto* context=static_cast<StartMusicContext*>(opaque);
  return context&&context->music&&context->audio&&context->sourceEventType&&
      (((song==44||song==45)&&argument==1)||(song==25&&argument==0))&&context->audio->outputOpen()&&
      context->music->request(static_cast<std::uint32_t>(song),static_cast<std::uint32_t>(argument),*context->sourceEventType).completed();
 };
 const auto requestIntroMusicFade=+[](void* opaque,float seconds)noexcept {
  auto* context=static_cast<StartMusicContext*>(opaque);
  return context&&context->music&&context->audio&&context->audio->outputOpen()&&seconds==0.5f&&
      context->music->requestFade1A4AB0(seconds).completed();
 };
 std::unique_ptr<race_mode1_host50ID::Owner> mode1;
 auto materialAudio=std::make_unique<material_audio50HI::Provider>();
 // This owner dies before the live mixer/shared boost PCM and owns96..116.
 auto opponentAudio=std::make_unique<opponent_audio50HX::Owner>();
 auto originalAiAudio=std::make_unique<native_original_ai_audio::Owner>();
 std::shared_ptr<rr::platform::OriginalSoundMode50KG> gameplaySoundMode;
 std::unique_ptr<rc_positional_sound50RC::Owner> rcPositionalAudio;
 bool rcAudioOwnersRetired=false;
 // D44 borrows the material PCM bank. Retire actual borrowers before either
 // RC PCM/source-retaining owner; the same mixer and canonical source outlive
 // this guard. Later root terminal cleanup may invoke this idempotent join.
 const auto retireRcAudioOwners=[&]()noexcept{
  if(!isRcTrack||rcAudioOwnersRetired)return true;
  originalAiAudio->close();
  if(rcPositionalAudio&&!rcPositionalAudio->close())return false;
  rcPositionalAudio.reset();
  if(materialAudio){materialAudio->reset();materialAudio.reset();}
  rcAudioOwnersRetired=true;return true;
 };
 struct RcAudioOwnerGuard {
  const decltype(retireRcAudioOwners)& retire;
  ~RcAudioOwnerGuard(){if(!retire())std::terminate();}
 } rcAudioOwnerGuard{retireRcAudioOwners};
 std::unique_ptr<tb_positional_sound50TB::Owner> tbPositionalAudio;
 bool tbAudioOwnersRetired=false;
 const auto retireTbAudioOwners=[&]()noexcept{
  if(!isTbTrack||tbAudioOwnersRetired)return true;
  // Every D44 row borrows materialAudio PCM. End those admissions first,
  // then the distinct positional transports, then the host material bank.
  originalAiAudio->close();
  if(tbPositionalAudio&&!tbPositionalAudio->close())return false;
  tbPositionalAudio.reset();
  if(materialAudio){materialAudio->reset();materialAudio.reset();}
  tbAudioOwnersRetired=true;return true;
 };
 struct TbAudioOwnerGuard {
  const decltype(retireTbAudioOwners)& retire;
  ~TbAudioOwnerGuard(){if(!retire())std::terminate();}
 } tbAudioOwnerGuard{retireTbAudioOwners};
 std::unique_ptr<gb_positional_sound50GB::Owner> gbPositionalAudio;
 bool gbAudioOwnersRetired=false;
 const auto retireGbAudioOwners=[&]()noexcept{
  if(!isGbTrack||gbAudioOwnersRetired)return true;
  // Every D44 row borrows materialAudio PCM. End those admissions first,
  // then the distinct positional transports, then the host material bank.
  originalAiAudio->close();
  if(gbPositionalAudio&&!gbPositionalAudio->close())return false;
  gbPositionalAudio.reset();
  if(materialAudio){materialAudio->reset();materialAudio.reset();}
  gbAudioOwnersRetired=true;return true;
 };
 struct GbAudioOwnerGuard {
  const decltype(retireGbAudioOwners)& retire;
  ~GbAudioOwnerGuard(){if(!retire())std::terminate();}
 } gbAudioOwnerGuard{retireGbAudioOwners};
 const auto reserveMcAudioPayload=[&](std::size_t bytes,const char* reason){
  require50DU(isMcTrack&&bytes<=mcRemainingPayloadBytes(),reason);
  mcAdditionalPayloadReserved+=bytes;
 };
 std::unique_ptr<mc_positional_sound50MC::Owner> mcPositionalAudio;
 bool mcAudioOwnersRetired=false;
 const auto retireMcAudioOwners=[&]()noexcept{
  if(!isMcTrack||mcAudioOwnersRetired)return true;
  // Every D44 row borrows materialAudio PCM. End those admissions first,
  // then the distinct positional transports, then the host material bank.
  originalAiAudio->close();
  if(mcPositionalAudio&&!mcPositionalAudio->close())return false;
  mcPositionalAudio.reset();
  if(materialAudio){materialAudio->reset();materialAudio.reset();}
  mcAudioOwnersRetired=true;return true;
 };
 struct McAudioOwnerGuard {
  const decltype(retireMcAudioOwners)& retire;
  ~McAudioOwnerGuard(){if(!retire())std::terminate();}
 } mcAudioOwnerGuard{retireMcAudioOwners};
 // Only fixed source state enters the racer bank; mixer/PCM ownership never does.
 opponent_audio50HX::BoostState opponentBoostAudio{};
 const char* audioSetting=std::getenv("RR_AUDIO");
 const bool audioOutputRequested=!(audioSetting&&std::string(audioSetting)=="0");
 // ALL8 still owns its real logical D40/D44/event/sample state when host
 // output is muted. RR_AUDIO=0 requests neither a device nor a WAV; the
 // same software mix advances real playback lifetime and source chronology.
 const bool audioEnabled=audioOutputRequested||nativeOriginalFleet;
 const bool originalAiAudioEnabled=audioEnabled&&nativeOriginalFleet;
 require50DU(!audioEnabled||trackSession->materialAudio,
     "Selected track material audio provider is unavailable; ALL8 requires logical owners even with muted host output");
 require50DU(!dynamicContactEnabled||audioEnabled,"dynamic source contacts require genuine audio providers");
 require50DU(!mode1Requested||audioEnabled,
     "mode1 requires original audio providers; use RR_RACE_MODE=2 explicitly for RR_AUDIO=0");
 rr::platform::AudioOutputDiagnostic50HD audioDiagnostic{};
 PodSpeechCounts50HG speechCounts{};PodSpeechState50HG speechState{};
 // Selected-player emitter state only; opponent bank visits never mutate it.
 // Fresh session reconstructs original19EBA0 values. Pause retains its timers.
 EngineDamageState50IA& engineDamage=liveAudio->humanController().damage;
 std::uint64_t engineDamageUpdates=0,engineDamageDraws=0,engineDamageDips=0;
 std::uint64_t engineDamageReturns=0,engineDamageCompletions=0;
 DamageSpeechEvent50HG pendingSpeech{};
 float playerSpeechChance=0.f;
 std::unique_ptr<gc_gate_audio50IN::Owner> gaGateAudio,gcGateAudio;
 SpeechPosition50HG audioListener{};bool audioListenerReady=false;
 std::uint64_t speechEvents=0,speechCues=0,announcerRequestsUnjoined=0;
 if((consumedRaStage.race()||consumedTaStage.race()||consumedMaStage.race()||consumedMbStage.race()||consumedSaStage.race()||consumedGaStage.race()||consumedGcStage.race())&&!audioEnabled)require50DU(liveAudio->finishCountedMutedGroups(loadedAudioSettings),
     "actual counted muted audio group settings rejected");
 if(originalCameraEnabled&&!audioEnabled)require50DU(sourceCameraCycleAudio.prepareMuted(),
     "explicit muted camera cycle owner rejected");
 if(audioEnabled){
  const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
  const std::filesystem::path audioRoot=suppliedAudioRoot?std::filesystem::path(suppliedAudioRoot):
      runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO");
  std::string audioError;
  rr::platform::loading_progress50DU::pulse("Loading audio");
 coldLoadDiagnostics.begin("audio_native_bank");
  if(!tcLiveAudioPrepared&&!tbLiveAudioPrepared&&!gbLiveAudioPrepared){
   const bool prepared=liveAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),audioRoot,audioError,playerRacer60,loadedAudioSettings);
   require50DU(prepared,audioError.empty()?"native audio bank preparation failed":audioError.c_str());
  }else require50DU(((isTcTrack&&tcLiveAudioPrepared&&!tbLiveAudioPrepared&&!gbLiveAudioPrepared)||
      (isTbTrack&&tbLiveAudioPrepared&&!tcLiveAudioPrepared&&!gbLiveAudioPrepared&&audioRoot==tbPreparedAudioRoot)||
      (isGbTrack&&gbLiveAudioPrepared&&!tcLiveAudioPrepared&&!tbLiveAudioPrepared&&audioRoot==gbPreparedAudioRoot))&&
      liveAudio->bank().ready&&liveAudio->mixerForMaterialOwner()&&liveAudio->groupsForProviders(),
      "TC/TB/GB early audio owner no longer matches actual later consumers");
 coldLoadDiagnostics.begin("audio_camera_and_race");
  auto* materialMixer=liveAudio->mixerForMaterialOwner();
  if(originalAiAudioEnabled){
   if(isGbTrack){
    require50DU(gbLiveAudioPrepared&&gbSoundMode&&gbSoundMode->ready()&&materialMixer&&
        materialMixer->originalSoundMode50KG()==gbSoundMode.get()&&
        (!originalMainApplication.soundMode||originalMainApplication.soundMode==gbSoundMode),
        "GB actual early source-mode owner differs at later audio join");
    gameplaySoundMode=gbSoundMode;
   }else if(isTbTrack){
    require50DU(tbLiveAudioPrepared&&tbSoundMode&&tbSoundMode->ready()&&materialMixer&&
        materialMixer->originalSoundMode50KG()==tbSoundMode.get()&&
        (!originalMainApplication.soundMode||originalMainApplication.soundMode==tbSoundMode),
        "TB actual early source-mode owner differs at later audio join");
    gameplaySoundMode=tbSoundMode;
   }else{
   gameplaySoundMode=originalMainApplication.soundMode;
   if(!gameplaySoundMode){
    gameplaySoundMode=std::make_shared<rr::platform::OriginalSoundMode50KG>();
    require50DU(gameplaySoundMode->initializeFromProfile1101C(1),
        "direct native race stereo-mode construction rejected");
   }
   require50DU(materialMixer&&materialMixer->bindOriginalSoundMode50KG(gameplaySoundMode),
       "actual gameplay source-mode binding rejected");
   }
  }
  auto* audioGroups=liveAudio->groupsForProviders();
  auto* sharedAnnouncer=liveAudio->sharedAnnouncer();
  require50DU(audioGroups&&sharedAnnouncer,"shared native audio group/episode owner unavailable");
  // The race UI emitter (raceUI.ini keys 1..6) is common to every course; a
  // human race's Results menu (common race or RA) dispatches all six.
  if(originalCameraEnabled)require50DU(materialMixer&&((sourceConsumedPauseActivation&&consumedRaStage.race())||humanCourseEnabled?
      sourceCameraCycleAudio.prepareResultsFeedback(runtime_asset_paths50LB::resolve("work/native/project/disc"),
          audioRoot,*materialMixer,*audioGroups,audioError):
      sourceCameraCycleAudio.prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),
          audioRoot,*materialMixer,*audioGroups,audioError)),
      audioError.empty()?"original camera cycle audio owner unavailable":audioError.c_str());
  if(isGaTrack){
   gaGateAudio=std::make_unique<gc_gate_audio50IN::Owner>();
   const bool prepared=materialMixer&&audioGroups&&gaGateAudio->prepare(*materialMixer,*audioGroups,audioRoot,audioError);
   require50DU(prepared&&gaGateAudio->available(),audioError.empty()?"GA original BIGDOOR PCM/World4D8 voice unavailable":audioError.c_str());
  }
  if(gcTriggerActions){
   gcGateAudio=std::make_unique<gc_gate_audio50IN::Owner>();
   const bool prepared=materialMixer&&audioGroups&&gcGateAudio->prepare(*materialMixer,*audioGroups,audioRoot,audioError);
   require50DU(prepared&&gcGateAudio->available(),audioError.empty()?"GC original BIGDOOR PCM/World4D8 voice unavailable":audioError.c_str());
  }
  if(raceEnabled){
   const bool raceAudioPrepared=materialMixer&&raceAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),
       audioRoot,*materialMixer,*audioGroups,*sharedAnnouncer,audioError,playerRacer60);
   require50DU(raceAudioPrepared,
       audioError.empty()?"original race audio preparation failed":audioError.c_str());
  }
 coldLoadDiagnostics.begin("audio_announcer_music");
  if(mode1Requested||dynamicContactEnabled){
   if(dynamicContactEnabled)for(std::size_t slot=0;slot<raceRacerCount;++slot){
    //2C4B30 binds Dax named banks from the same actual manager racer60 as
    // the dynamic registry. Camera position and AI/human state are not IDs.
    require50DU(slot==0||racerRigs[slot]!=nullptr,"actual announcer actor rig unavailable");
    const auto* selection=slot==0?selectedOriginal:
        original_racer_selection50IJ::findByPodId(racerRigs[slot]->podId.c_str());
    require50DU(selection,"actual announcer original racer60 unavailable");
    dynamicRacer60[slot]=selection->racer60;
   }
   require50DU(materialMixer&&sourceAnnouncer->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),audioRoot,
       *materialMixer,*audioGroups,*sharedAnnouncer,sessionRandom.seed,applicationClock,
       s.noiseOwner().sharedRandomState,audioError,playerRacer60,
       dynamicContactEnabled?dynamicRacer60.data():nullptr,
       dynamicContactEnabled?raceRacerCount:0,&sessionRandom.range),
       audioError.empty()?"source Dax cold preparation failed; explicit fallback RR_RACE_MODE=2":audioError.c_str());
   const bool practiceMusicOnly=nativeOrdinaryLaunch11258==2u||nativeOrdinaryLaunch11258==3u;
   require50DU(materialMixer&&raceMusic->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),audioRoot,
       *materialMixer,*audioGroups,audioError,dynamicContactEnabled&&!practiceMusicOnly,
       dynamicContactEnabled?race_music50IE::TimedVolumePolicy50IW::NativeLinearSharedMix:
           race_music50IE::TimedVolumePolicy50IW::StrictSourceBoundary,practiceMusicOnly,
       // Song 25 is the intro camera's 1756D0 -> 1A46E0(25,0) on every
       // course that runs the tournament intro, not a TA-only track song.
       originalCameraEnabled&&nativeOrdinaryLaunch11258==1u&&
           (dynamicProfile.game34==2u||cameraStartup350!=0)),
       audioError.empty()?"source music cold preparation failed; explicit fallback RR_RACE_MODE=2":audioError.c_str());
   require50DU(sourceAnnouncer->prepared()&&raceMusic->bank().ready==
       raceMusic->expectedReadyClips(),
       "mode1 source providers/music PCM incomplete; no silent fallback (RR_RACE_MODE=2 is explicit)");
   require50DU(raceAudio->setHostSpeechEnabled(false),"mode1 HOST Dax suppression binding failed");
   mode1Enabled=mode1Requested;
   if(dynamicContactEnabled){
    const auto bytes=runtime_startup50fl::readBytes(audioRoot/"SPCHPCT.INI");
    require50DU(bindOriginalDynamicContactChances50IW(bytes.size(),
        pod_materials50ed_fixture::detail::sha256(bytes).c_str(),dynamicEventBindings),
        "authentic original collision speech chances unavailable");
    //21080C..210818 vehicle33C is the same application28C->10C event owner.
    dynamicEventBindings.eventOwner33CBound=true;
   }
  }
  rr::platform::loading_progress50DU::pulse("Loading track sounds");
 coldLoadDiagnostics.begin("audio_materials");
  const bool materialAudioReady=materialMixer&&(isMcTrack?
      materialAudio->prepareMc(mcColdSources,audioRoot,*materialMixer,64,audioError):isGbTrack?
      materialAudio->prepareGb(gbColdSources,audioRoot,*materialMixer,64,audioError):isTbTrack?
      materialAudio->prepareTb(tbColdSources,audioRoot,*materialMixer,64,audioError):isRcTrack?
      materialAudio->prepareRc(rcColdSources,audioRoot,*materialMixer,64,audioError):
      materialAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),audioRoot,
      *materialMixer,64,audioError,track_session_assets50IG::codeEquals(trackSession->world->code,"GA")?
      material_audio50HI::Track::ga:isRaTrack?material_audio50HI::Track::ra:isTcTrack?material_audio50HI::Track::tc:isScTrack?material_audio50HI::Track::sc:isMbTrack?material_audio50HI::Track::mb:isMaTrack?material_audio50HI::Track::ma:isSaTrack?material_audio50HI::Track::sa:
      trackSession->originalStaticCandidates?material_audio50HI::Track::gc:material_audio50HI::Track::ta));
  require50DU(materialAudioReady,audioError.empty()?"native material audio preparation failed":audioError.c_str());
  require50DU(!isTbTrack||(materialAudio->tbSourceIdentity()==tbColdSources.get()&&
      materialAudio->collisionRowCount()==24&&materialAudio->driveRowCount()==20&&
      materialAudio->extraVoiceFirst()==192&&materialAudio->extraVoiceCount()==28),
      "TB actual material rows or exclusive host extras192..219 differ");
  require50DU(!isGbTrack||(materialAudio->gbSourceIdentity()==gbColdSources.get()&&
      materialAudio->collisionRowCount()==20&&materialAudio->driveRowCount()==20&&
      materialAudio->extraVoiceFirst()==192&&materialAudio->extraVoiceCount()==24),
      "GB actual material rows or exclusive host extras192..215 differ");
  require50DU(!isMcTrack||(materialAudio->mcSourceIdentity()==mcColdSources.get()&&
      materialAudio->collisionRowCount()==12&&materialAudio->driveRowCount()==8&&
      materialAudio->extraVoiceFirst()==192&&materialAudio->extraVoiceCount()==4),
      "MC actual material rows or exclusive host extras192..195 differ");
  if(isMcTrack)reserveMcAudioPayload(materialAudio->retainedPayloadBytes(),
      "MC actual material audio retained payload exceeds remaining ledger");
  if(isRaTrack){
   const auto& originals=raRuntime->source.collisions;
   for(std::size_t i=0;i<trackBindings.size();++i){const auto& binding=trackBindings[i];
    if(binding.animated||binding.trigger)continue;
    require50DU(binding.originalOwner<originals.size(),"RA material owner outside actual retained collisions");
    const auto& original=originals[binding.originalOwner];
    require50DU(rr::assets::applyTrackMaterialAssignments50HI(materialAudio->catalog(),actualWorld,
        original.zone,original.ordinal,trackStorage.contactMaterials[i],audioError),
        audioError.empty()?"original RA contact material assignment failed":audioError.c_str());
   }
  }
  if(sourceStaticCandidates&&!isRaTrack){
   //271600 applies actual AUB assignments before the creator publishes A9.
   for(std::size_t i=0;i<trackBindings.size();++i){
    const auto& binding=trackBindings[i];
    // Animated CHARACTER owners retain constructor materialA9=0; AUB's
    // static SCB assignments do not index their sentinel originalOwner.
    if(binding.animated||binding.trigger)continue;
    require50DU(binding.originalOwner<ground.owners.size(),"GC static material owner index outside authored ground");
    const auto& original=ground.owners[binding.originalOwner];
    require50DU(rr::assets::applyTrackMaterialAssignments50HI(materialAudio->catalog(),actualWorld,
        original.zone,original.ordinal,trackStorage.contactMaterials[i],audioError),
        audioError.empty()?"original GC contact material assignment failed":audioError.c_str());}
  }
  require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
      race_audio_groups50IC::ProviderRole::MaterialCollision,64,material_audio50HI::soundCount),
      "material group ownership rejected");
  if(materialAudio->extraVoiceCount())require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
      race_audio_groups50IC::ProviderRole::MaterialCollision,materialAudio->extraVoiceFirst(),
      materialAudio->extraVoiceCount()),"extra original material sound group ownership rejected");
  if(isRcTrack){
   require50DU(nativeOriginalFleet&&humanCourseEnabled&&originalAiAudioEnabled&&
       rcColdSources&&rcColdSources->ready()&&materialAudio->rcSourceIdentity()==rcColdSources.get(),
       "RC positional audio requires the actual retained human source and material provider");
   rcPositionalAudio=std::make_unique<rc_positional_sound50RC::Owner>();
   const bool prepared=rcPositionalAudio->prepare(rcColdSources,*materialAudio,audioRoot,
       *materialMixer,*audioGroups,audioError);
   require50DU(prepared,audioError.empty()?"RC positional audio preparation rejected":audioError.c_str());
  }
  if(isTbTrack){
   require50DU(nativeOriginalFleet&&humanCourseEnabled&&originalAiAudioEnabled&&tbLiveAudioPrepared&&
       tbColdSources&&tbColdSources->ready()&&tbPodSound&&tbPodSound->prepared()&&
       tbPodSound->sourceIdentity()==tbTriggerAuthoring.get()&&
       tbPodSound->sourceIdentity()->coldSource()==tbColdSources.get()&&
       materialAudio->tbSourceIdentity()==tbColdSources.get(),
       "TB positional audio requires the same retained trigger/material/cold source");
   tbPositionalAudio=std::make_unique<tb_positional_sound50TB::Owner>();
   // prepare checks every actual128..151 slot is inactive and unbound before
   // registering all97 descriptors with this same mixer and World4D8 owner.
   const bool prepared=tbPositionalAudio->prepare(tbColdSources,*materialAudio,audioRoot,
       *materialMixer,*audioGroups,audioError);
   require50DU(prepared,audioError.empty()?"TB positional audio preparation rejected":audioError.c_str());
  }
  if(isMcTrack){
   require50DU(nativeOriginalFleet&&humanCourseEnabled&&originalAiAudioEnabled&&
       mcColdSources&&mcColdSources->ready()&&materialAudio->mcSourceIdentity()==mcColdSources.get()&&
       materialMixer&&audioGroups&&audioGroups->uses(*materialMixer)&&materialMixer->originalSoundMode50KG(),
       "MC positional audio requires actual same-source material/mixer/groups/mode owners");
   mcPositionalAudio=std::make_unique<mc_positional_sound50MC::Owner>();
   const bool prepared=mcPositionalAudio->prepare(mcColdSources,audioRoot,*materialMixer,*audioGroups,audioError);
   require50DU(prepared&&mcPositionalAudio->uses(*materialMixer,*audioGroups),
       audioError.empty()?"MC positional audio preparation rejected":audioError.c_str());
   reserveMcAudioPayload(mcPositionalAudio->retainedPayloadBytes(),
       "MC actual positional audio retained payload exceeds remaining ledger");
  }
  if(isGbTrack){
   require50DU(nativeOriginalFleet&&humanCourseEnabled&&originalAiAudioEnabled&&gbLiveAudioPrepared&&
       gbColdSources&&gbColdSources->ready()&&gbPodSound&&gbPodSound->prepared()&&
       gbPodSound->sourceIdentity()==gbTriggerAuthoring.get()&&
       gbPodSound->sourceIdentity()->coldSource()==gbColdSources.get()&&
       materialAudio->gbSourceIdentity()==gbColdSources.get(),
       "GB positional audio requires the same retained trigger/material/cold source");
   gbPositionalAudio=std::make_unique<gb_positional_sound50GB::Owner>();
   // prepare checks every actual128..151 slot is inactive and unbound before
   // registering all49 descriptors with this same mixer and World4D8 owner.
   const bool prepared=gbPositionalAudio->prepare(gbColdSources,*materialAudio,audioRoot,
       *materialMixer,*audioGroups,audioError);
   require50DU(prepared,audioError.empty()?"GB positional audio preparation rejected":audioError.c_str());
   std::cerr<<"gb_native_audio ambient_rows=4 ambient_scheduling=unsupported positional_rows=49 native_transport_slots=24\n";
  }
 coldLoadDiagnostics.begin("audio_roster_banks");
  if(raceEnabled&&!originalAiAudioEnabled){
   std::array<opponent_audio50HX::Source,opponent_audio50HX::maxOpponents> sources{};
   for(std::size_t racer=1;racer<raceRacerCount;++racer)
    sources[racer-1]={racerRigs[racer]->podId,&racerRigs[racer]->phy};
   const bool prepared=opponentAudio->prepare(sources,audioRoot,liveAudio->bank(),*materialMixer,audioError);
   require50DU(prepared,audioError.empty()?"original opponent engine audio preparation failed":audioError.c_str());
   require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
       race_audio_groups50IC::ProviderRole::EngineBinderBoost,
       opponent_audio50HX::firstVoice,opponent_audio50HX::ownedVoiceCount),
       "opponent engine group ownership rejected");
   const auto& opponentBank=opponentAudio->bank();
   std::cerr<<"Opponent engine audio ready/missing/unsupported/invalid "<<opponentBank.ready<<'/'
       <<opponentBank.missing<<'/'<<opponentBank.unsupported<<'/'<<opponentBank.invalid
       <<" unique original engine samples; shared original boost; exclusive voices96..116; host distance attenuation.\n";
   for(const auto& mapping:opponentAudio->mappings())
    std::cerr<<"OPPONENT AUDIO "<<mapping.podId<<" engine="<<mapping.engineName
        <<" binder="<<mapping.binderName<<'\n';
  }
  if(originalAiAudioEnabled){
   std::array<opponent_audio50HX::Source,8> sources{};
   for(std::size_t racer=0;racer<raceRacerCount;++racer){
    require50DU(racerRigs[racer]!=nullptr,"ALL8 audio requires every actual original rig");
    sources[racer]={racerRigs[racer]->podId,&racerRigs[racer]->phy};
   }
   require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
       race_audio_groups50IC::ProviderRole::EngineBinderBoost,96,21)&&
       race_audio_groups50IC::bindProviderRange(*audioGroups,
       race_audio_groups50IC::ProviderRole::EngineBinderBoost,160,32),
       "AI source sound group binding rejected");
   // Evaluate before reading the message (argument order is unspecified).
   const bool aiAudioPrepared=originalAiAudio->prepare(sources,audioRoot,liveAudio->bank(),*materialMixer,
       sessionIndex+1,originalAiSettings1125C,audioError);
   require50DU(aiAudioPrepared,audioError.empty()?"AI audio preparation rejected":audioError.c_str());
   if(humanCourseEnabled){
    // Same stable native slot0 identity convention as the real seven AI rows;
    // this binds the human reset/material fields and its genuine D44 row0.
    auto& controller=liveAudio->humanController();
    require50DU(controller.bind({sessionIndex+1,17,18,19,0,playerRacer60})&&
        originalAiAudio->prepareHumanResetOwner(controller),
        "actual human controller/material reset ownership rejected");
   }
   require50DU(originalAiAudio->prepareSpeech(runtime_asset_paths50LB::resolve("work/native/project/disc"),audioRoot,
       *materialMixer,*audioGroups,audioError),audioError.empty()?"actual all-eight D48 banks unavailable":audioError.c_str());
   const bool materialRows=originalAiAudio->prepareMaterialArrays(*materialMixer,*audioGroups,
       materialAudio->catalog(),materialAudio->bank(),audioError);
   require50DU(materialRows,audioError.empty()?"actual ALL8 material rows unavailable":audioError.c_str());
   require50DU(originalAiAudio->selectNativeMaterialPitchPolicy(),"native material pitch policy admission rejected");
   std::cerr<<"Original D44 authored arrays bound for all eight actual racers; native decoded-rate unity4096 "
       "pitch quantization/rendering selected; original233E90/RPC16 metadata remains unknown.\n";
  }
  speechCounts=liveAudio->speechCounts();
  require50DU(pod_speech50HG::readPlayerSpeechChance(playerSpeechChance,audioError,
      audioRoot/"SPCHPCT.INI"),audioError.empty()?"actual speech probability missing":audioError.c_str());
  if(!originalUiStarted){
   if(originalAiAudioEnabled)require50DU(originalAiAudio->initializeSpeech(s.noiseOwner().sharedRandomState,
       humanCourseEnabled?&speechCounts:nullptr,humanCourseEnabled?&speechState:nullptr),
       "actual all-eight D48 shared-RNG initialization failed");
   if(!originalAiAudioEnabled)require50DU(initializePodSpeech50HG(speechCounts,s.noiseOwner().sharedRandomState,speechState)==
       PodSpeechStatus50HG::Complete,"native human speech cursor initialization failed");
  }
  const auto& bank=liveAudio->bank();
  std::cerr<<"Audio bank ready/missing/unsupported/invalid "<<bank.ready<<'/'<<bank.missing
      <<'/'<<bank.unsupported<<'/'<<bank.invalid<<" of "<<bank.clips.size()<<'\n';
  const auto& audioConfig=liveAudio->audioSettings();
  std::cerr<<"Audio configured original levels effects/music/character/Dax "
      <<audioConfig.effects11020<<'/'<<audioConfig.music11024<<'/'
      <<audioConfig.character11028<<'/'<<audioConfig.announcer1102C
      <<"; no saved audio preferences loaded; "<<(mode1Enabled?"source mode1":"HOST mode2")
      <<" Dax policy, shared voice84 completion.\n";
  lapOwner.audioContext=liveAudio.get();
  lapOwner.audioDispatch=[](void* context,std::uint32_t key,std::uint32_t flags,
                           std::uint32_t a3)noexcept{
   const auto result=static_cast<live_audio50HF::Owner*>(context)->dispatchLap(key,flags,a3);
   return lap_drive50GJ::AudioDispatchResult{result.completed(),
       result.status==pod_audio50HE::DispatchStatus::CompletedStarted,
       result.status==pod_audio50HE::DispatchStatus::CompletedUnavailable,result.originalV0};
  };
 }
 // Original 1A6CEC follows the race music manager (1A39F0/1A3C00). Here after the
 // race audio/music block, which a muted run skips; the original always loads it.
 rr::platform::loading_progress50DU::step(.1f,0x1a6cec);
 rr::platform::loading_progress50DU::pulse("Preparing navigation");
 coldLoadDiagnostics.begin("navigation_runtime");
 TrackNavigationDiagnostic50EU navigationDiagnostic{};
 auto reportNavigation=[&](bool ok,const char* operation){
  if(!ok)std::cerr<<operation<<" navigation error "
      <<static_cast<unsigned>(navigationDiagnostic.error)<<" source-stage 0x"
      <<std::hex<<navigationDiagnostic.sourcePC<<std::dec
      <<" node "<<navigationDiagnostic.node<<'\n';
  return ok;
 };
 if(isMcTrack){
  requireMbRuntimeNavigation50MB();
  LapProgressTrackView50MC markers;
  require50DU(prepareLapProgressTrack50GF(mcMapView,mcMapMetadata,markers,lapOwner.diagnostic)&&
      lapOwner.bindMcTrack(markers),"actual MC lap markers or shared lap owner rejected");
 }else if(isGbTrack){
  requireMbRuntimeNavigation50MB();
  LapProgressTrackView50GB markers;
  require50DU(prepareLapProgressTrack50GF(gbMapView,gbMapMetadata,markers,lapOwner.diagnostic)&&
      lapOwner.bindGbTrack(markers),"actual GB lap markers or shared lap owner rejected");
 }else if(isTbTrack){
  requireMbRuntimeNavigation50MB();
  LapProgressTrackView50TB markers;
  require50DU(prepareLapProgressTrack50GF(tbMapView,tbMapMetadata,markers,lapOwner.diagnostic)&&
      lapOwner.bindTbTrack(markers),"actual TB lap markers or shared lap owner rejected");
 }else if(isRcTrack){
  requireMbRuntimeNavigation50MB();
  const auto& markers=rcNavigationBorrow.mapLap();
  require50DU(markers.prepared&&markers.nodeCount==186&&
      selectedOriginalAiMapMetadata.bound&&selectedOriginalAiMetadata.bound,
      "RC same-read lap and metadata owners unavailable");
  lapOwner.track=markers;
 }else if(isTcTrack){
  requireMbRuntimeNavigation50MB();
  LapProgressTrackView50TC markers;
  require50DU(prepareLapProgressTrack50GF(tcMapView,tcMapMetadata,markers,lapOwner.diagnostic)&&
      lapOwner.bindTcTrack(markers),"actual TC lap markers or shared lap owner rejected");
 }else if(isMbTrack){
  requireMbRuntimeNavigation50MB();
  LapProgressTrackView50MB markers;
  require50DU(prepareLapProgressTrack50GF(mbMapView,mbMapMetadata,markers,lapOwner.diagnostic)&&
      lapOwner.bindMbTrack(markers),"actual MB lap markers or shared lap owner rejected");
 }else if(isScTrack){
  const auto* source=scColdSources->sourcesDecoded();
  require50DU(source&&source->mapLap.prepared&&source->mapLap.nodeCount==179&&
      source->map.gatesPrepared&&!source->map.indicesPrepared&&source->ai.gatesPrepared&&!source->ai.indicesPrepared,
      "SC same-read gate/lap source unavailable");
  *legacyNavigationMap=source->map;*legacyNavigationAI=source->ai;
  originalAiMapMetadata=source->mapMetadata;originalAiMetadata=source->aiMetadata;
  lapOwner.track=source->mapLap;
  require50DU(reportNavigation(bindTrackNavigationZones50EU(liveWorld,*legacyNavigationMap,navigationDiagnostic),"SC map"),
      "SC map actual world index binding rejected");
  require50DU(reportNavigation(bindTrackNavigationZones50EU(liveWorld,*legacyNavigationAI,navigationDiagnostic),"SC AI"),
      "SC AI actual world index binding rejected");
  const auto& bytes=source->memberBytes[2];
  require50DU(rr::host::readCourseSpeedLimits50GK(reinterpret_cast<const char*>(bytes.data()),bytes.size(),
      navigationAI->track.count,courseLimits,courseDiagnostic),"SC actual AI speed metadata rejected");
 }else if(isRaTrack){
  originalAiMapMetadata=raRuntime->source.mapMetadata;
  originalAiMetadata=raRuntime->source.aiMetadata;
  lapOwner.track=raRuntime->source.mapLap;
  const auto& bytes=raRuntime->source.memberBytes[2];
  require50DU(rr::host::readCourseSpeedLimits50GK(reinterpret_cast<const char*>(bytes.data()),
      bytes.size(),navigationAI->track.count,courseLimits,courseDiagnostic),
      "actual retained RA A spline speed metadata rejected");
 }else{
  rr::assets::ResArchive48C navigationArchive;rr::assets::DecodeError48C error;
  const auto bytes=runtime_startup50fl::readBytes(trackSession->archive);
  require50DU(rr::assets::decodeResource48C(bytes,navigationArchive,error),
      "decode actual selected-track navigation archive");
  auto scratch=std::make_unique<TrackNavigationScratch50EU>();
  auto readNavigation=[&](const char* member,TrackNavigation50EU& destination){
   std::vector<std::uint8_t> text;
   require50DU(rr::assets::extractResMember48C(navigationArchive,member,text,error),
       "extract actual M/A navigation spline");
   require50DU(reportNavigation(readTrackNavigation50EU(
       reinterpret_cast<const char*>(text.data()),text.size(),s.animationShared.libm,
       *scratch,destination,navigationDiagnostic),member),
       "prepare source navigation gates");
   if(originalAiEnabled)require50DU(rr::assets::readAiTrackMetadata50IU(
       reinterpret_cast<const char*>(text.data()),text.size(),destination,
       &destination==navigationAI?originalAiMetadata:originalAiMapMetadata,
       originalAiAuthoringDiagnostic),"original AI/map metadata unavailable");
   if(&destination==navigationAI)
    require50DU(rr::host::readCourseSpeedLimits50GK(reinterpret_cast<const char*>(text.data()),
        text.size(),destination.track.count,courseLimits,courseDiagnostic),"actual selected A spline speed metadata rejected");
   require50DU(reportNavigation(bindTrackNavigationZones50EU(
       liveWorld,destination,navigationDiagnostic),member),
       "bind source navigation portal indices");
  };
  readNavigation(trackSession->mapSpline,*legacyNavigationMap);
  require50DU(prepareLapProgressTrack50GF(*scratch,lapOwner.track,lapOwner.diagnostic),
      "actual authored lap progress markers rejected");
  readNavigation(trackSession->aiSpline,*legacyNavigationAI);
 }
 if(!lapContinuity.initialized){
  lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
  initializePlayerLapRaceFields50GF(lapContinuity.state);
  initializePlayerLapCursor50GF(lapContinuity.state);
  lapContinuity.initialized=true;
 }
 // Actual owned reset-prefix projection; dynamic wrong-way1A2E30 is not joined.
 resetUndertrackPlayerPrefix50ET(lapContinuity.prefix);
 s.fields.impactFlagsCD8=static_cast<std::uint8_t>((s.fields.impactFlagsCD8&~7u)|
     (lapContinuity.state.vehicleCD8&7u));
 std::string lapSoundError;
 require50DU(lap_sound50GH::prepareDisabledFromPod01(lapOwner.sound,lapOwner.provider,lapSoundError,
     playerArchive,playerRacer60),
     "actual event sound owner preparation failed");
 std::cerr<<"HOST LAPS uses authored markers and explicit config mode2; lap audio provider="
     <<(audioEnabled?"native PCM (see asset availability)":"explicitly disabled")
     <<"; source wrong-way progression enabled; race-finish rules remain unjoined.\n";
 TrackNavigationProgress50EU navigationProgress{};
 initializeTrackNavigationProgress50EU(navigationProgress);
 //20F5C0..20F5C8 initializes CA0=(0,0,0,1). The first gate query therefore
 // takes1A0940's explicit zero-XZ miss; do not seed this with a guessed pose.
 float navigationCA0[4]{0.f,0.f,0.f,1.f};
 const CollectionRef50BX initialCollisionZone=checkpointInput?checkpointInput->primaryZone:trackSession->initialCollisionZone;
 if(checkpointInput){
  require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack,"MB/TC/RC/TB/GB/MC native session does not consume a HOST checkpoint reconstruction");
  require50DU(checkpointInput->zoneCount==liveWorld.zoneCount,"checkpoint world namespace changed");
  require50DU(checkpointInput->progress.reset.vehicleC80<navigationAI->track.count&&
      checkpointInput->progress.vehicleC84<navigationAI->track.count&&
      checkpointInput->progress.reset.vehicleC88<navigationMap->track.count&&
      checkpointInput->progress.reset.vehicleC8C<navigationMap->track.count&&
      checkpointInput->selection.anchorNode<navigationMap->track.count&&
      navigationMap->nodeIndex78[checkpointInput->selection.anchorNode]==checkpointInput->nodeIndex78,
      "checkpoint navigation identity changed");
  recovery50FI::Diagnostic recoveryDiagnostic{};
  require50DU(recovery50FI::restoreContinuity(*checkpointInput,s.fields,navigationProgress,
      navigationCA0,pose,recoveryDiagnostic),"checkpoint continuity restore failed");
  std::cerr<<"Checkpoint restored: anchor="<<checkpointInput->selection.anchorNode
           <<" zone="<<initialCollisionZone<<" position="<<pose.v[12]<<','<<pose.v[13]<<','<<pose.v[14]
           <<" damage="<<s.fields.damage7D4<<'/'<<s.fields.damage7D0<<'\n';
 }
 const auto validNavigationScene=[&](SceneNodeIndex49P node){
  const auto index=static_cast<std::int32_t>(node);
  return index>=0&&index<static_cast<std::int32_t>(s.sceneNodeCount);
 };
 require50DU(validNavigationScene(x.bindings.animation.bodyScene288)&&validNavigationScene(x.bindings.lifecycle.main190)&&
     m.domain.sceneAttached&&!m.domain.sceneHasParent&&!s.fields.vehicle58,
     "source player navigation scene ownership unavailable");
 std::uint64_t navigationUpdates=0,navigationGateSkips=0,navigationCA0Copies=0;
 std::uint64_t navigationAISequential=0,navigationMapSequential=0;
 std::uint64_t navigationAIScan=0,navigationMapScan=0;
 auto& mainMembership=trackStorage.mainMembership.state;
 auto& secondaryMembership=trackStorage.secondaryMembership.state;
 auto& terrainMembership=trackStorage.terrainMembership.state;
 auto& membershipWork=trackStorage.membershipWork;
 const auto mainMembershipStorage=trackStorage.mainMembership.storage();
 const auto secondaryMembershipStorage=trackStorage.secondaryMembership.storage();
 const auto terrainMembershipStorage=trackStorage.terrainMembership.storage();
 live_terrain_probe50fr::Owner terrainLiveOwner{};
 require50DU(live_terrain_probe50fr::initialize(terrainLiveOwner,terrainOwner.initial.segment),"initialize actual single-pair terrain owner");
 const PodLiveQueryContext50IZ recoveryPodLive{{zoneLifecycle,liveWorld,
     recoveryQueryOwners.memberships[0],membershipWork,trackCounts.podQuery},&x.parameters.shape};
 auto recoveryBorrowedQueries=[&](){
  undertrack_owners50ew_fixture::BorrowedQueries q{};
  q.main690=&query;q.mainMembership=&mainMembership;q.mainStorage=mainMembershipStorage;q.mainRef=mainQueryRef;
  q.movement7A4=&movement;q.movementEndpoints=endpoints;q.movementMetadata=endpointScenes;q.movementSource=&movementSource;
  q.terrain348=&terrainOwner.initial;q.terrainResource=&terrainLiveOwner.resource;q.terrainMetadata=&terrainOwner.metadata;
  q.terrainMembership=&terrainMembership;q.terrainStorage=terrainMembershipStorage;q.terrainRef=terrainQueryRef;
  return q;
 };
 auto recoveryQueries=[&](){return recoveryQueryOwners.binding(recoveryBorrowedQueries(),
     {trackCounts.podQuery,trackCounts.movementQuery});};
 auto publishRecoveryQueries=[&](){
  if(!originalRecoveryEnabled)return true;
  if(!recoveryQueryOwners.sourcePrepared||!recoveryQueryOwners.memberships[0].attached||
     !recoveryQueryOwners.memberships[1].attached||s.shape.registrationE0!=trackCounts.podQuery)return false;
  CollisionSphere48J sphere{};std::memcpy(sphere.center.v,s.shape.center60.v,sizeof sphere.center.v);sphere.radius=s.shape.radius80;
  const auto pod=projectCollectionObject50CF(recoveryQueryOwners.podFilter,sphere,query.collisionTypeTag,0,
      std::bit_cast<std::uint32_t>(s.shape.center60.v[3]));
  const auto ray=projectCollectionObject50CF(movement.filter,recoveryQueryOwners.movementSphere,
      movement.collisionTypeTag,recoveryQueryOwners.movementTag,
      std::bit_cast<std::uint32_t>(recoveryQueryOwners.memberships[1].bounds.cachedCenter.v[3]));
  if(!pod.valid||!ray.valid)return false;
  const CollectionProjection50CF* projection[]{&pod,&ray};
  const CollectionRef50BX refs[]{trackCounts.podQuery,trackCounts.movementQuery};
  for(unsigned i=0;i<2;++i){const auto index=refs[i]-1u;collectionObjects[index]=projection[i]->object;
   collectionSource[index].flagsF8=projection[i]->state.flagsF8;
   collectionSource[index].cachedCenter=projection[i]->state.cachedCenter;
   collectionSource[index].cachedRadius=projection[i]->state.cachedRadius;
  }
  return true;
 };

 auto prepareQueryCache=[&](Type2StaticQuery49X& source,const BodyMatrix48G& scene,
                            LiveZoneBounds50EN& bounds){
  if(!source.resource||!source.resource->nodeCount||(source.resource->root&0x8000u)||
     source.resource->root>=source.resource->nodeCount)return false;
  const auto& root=source.resource->nodes[source.resource->root];
  QuerySpherePrepareInput48O input{};input.preparedScene=scene;input.shapeType=2;input.hasPairTree=true;
  for(unsigned i=0;i<3;++i){input.treeCenter.v[i]=root.center[i];input.treeHalf.v[i]=root.half[i];}
  QuerySpherePrepareOutput48O prepared{};
  if(prepareQuerySphere48O(input,prepared)!=QuerySpherePrepareStatus48O::Ready)return false;
  source.worldMatrix=prepared.world;
  for(unsigned i=0;i<3;++i)source.sphere.center.v[i]=prepared.center.v[i];
  source.sphere.radius=prepared.radius;
  bounds={};bounds.bounds.shapeTag=2;bounds.bounds.shapeWorld=prepared.world;
  bounds.bounds.cachedCenter=prepared.center;bounds.bounds.cachedRadius=prepared.radius;
  bounds.type2Resource=source.resource;return true;
 };
 auto reportMembership=[&](const LiveZoneResult50EN& result,const char* operation){
  if(result.status==LiveZoneStatus50EN::Complete)return true;
  std::cerr<<operation<<" membership status "<<static_cast<unsigned>(result.status)
           <<" bounds "<<static_cast<unsigned>(result.boundsStatus)
           <<" geometry "<<static_cast<unsigned>(result.geometryStatus)
           <<" registry "<<static_cast<unsigned>(result.registryStatus);
  if(result.status==LiveZoneStatus50EN::UnsupportedLinkRemoval)
   std::cerr<<" UnsupportedLinkRemoval";
  std::cerr<<'\n';return false;
 };
 if(gcAnimatedWorld)for(std::size_t i=0;i<gcAnimatedMembership.size();++i){
  auto& membership=*gcAnimatedMembership[i];const auto bounds=gc_animated_candidates50IG::bounds(*gcAnimatedWorld,i);
  const auto ref=static_cast<CollectionRef50BX>(animatedCandidateFirst+i+1);
  //2D93F0 ->120070(method2,radius0), then2D94B0 ->120230 after cold pose.
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,gcAnimatedWorld->source(i).startingZone,ZoneMethod50CL::Second,0.f,
      bounds,membershipWork),"GC original animation initial attachment"),"GC original animation zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"GC original animation cold update"),"GC original animation cold membership failed");
 }
 if(gaAnimatedWorld)for(std::size_t i=0;i<gaAnimatedMembership.size();++i){
  auto& membership=*gaAnimatedMembership[i];const auto bounds=ga_animated_candidates::bounds(*gaAnimatedWorld,i);
  const auto ref=static_cast<CollectionRef50BX>(animatedCandidateFirst+i+1);
  //2D93F0 ->120070(method2,radius0), then2D94B0 ->120230 after cold pose.
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,gaAnimatedWorld->source(i).startingZone,ZoneMethod50CL::Second,0.f,
      bounds,membershipWork),"GA original animation initial attachment"),"GA original animation zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"GA original animation cold update"),"GA original animation cold membership failed");
 }
 if(gaAnimatedWorld)for(std::size_t i=0;i<gaTriggerMembership.size();++i){
  auto& membership=*gaTriggerMembership[i];const auto& authored=gaTriggerAuthoring->source(i);
  const auto bounds=ga_trigger_candidates::bounds(authored);
  const auto ref=static_cast<CollectionRef50BX>(gaTriggerCandidateFirst+i+1);
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,authored.zone,ZoneMethod50CL::First,0.f,bounds,membershipWork),
      "GA authored trigger initial attachment"),"GA trigger zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"GA authored trigger cold update"),"GA trigger membership failed");
 }
 if(scTriggerAuthoring)for(std::size_t i=0;i<scTriggerMembership.size();++i){
  auto& membership=*scTriggerMembership[i];const auto& authored=scTriggerAuthoring->source(i);
  const auto bounds=sc_trigger_candidates::bounds(authored);
  const auto ref=static_cast<CollectionRef50BX>(scTriggerCandidateFirst+i+1);
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,authored.zone,ZoneMethod50CL::First,0.f,bounds,membershipWork),
      "SC actual trigger attachment"),"SC trigger zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"SC actual trigger cold update"),"SC trigger cold membership failed");
 }
 if(maTriggerAuthoring)for(std::size_t i=0;i<maTriggerMembership.size();++i){
  auto& membership=*maTriggerMembership[i];const auto& authored=maTriggerAuthoring->source(i);
  const auto bounds=ma_trigger_candidates::bounds(authored);
  const auto ref=static_cast<CollectionRef50BX>(maTriggerCandidateFirst+i+1);
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,authored.zone,ZoneMethod50CL::First,0.f,bounds,membershipWork),
      "MA authored trigger attachment"),"MA trigger zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"MA authored trigger cold update"),"MA trigger cold membership failed");
 }
 if(saTriggerAuthoring)for(std::size_t i=0;i<saTriggerMembership.size();++i){
  auto& membership=*saTriggerMembership[i];const auto& authored=saTriggerAuthoring->source(i);
  const auto bounds=sa_trigger_candidates::bounds(authored);
  const auto ref=static_cast<CollectionRef50BX>(saTriggerCandidateFirst+i+1);
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,authored.zone,ZoneMethod50CL::First,0.f,bounds,membershipWork),
      "SA authored trigger attachment"),"SA trigger zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"SA authored trigger cold update"),"SA trigger cold membership failed");
 }
 if(gcTriggerActions)for(std::size_t i=0;i<gcTriggerMembership.size();++i){
  auto& membership=*gcTriggerMembership[i];const auto& authored=gcTriggerAuthoring->source(i);
  const auto bounds=gc_trigger_candidates50KG::bounds(authored);
  const auto ref=static_cast<CollectionRef50BX>(gcTriggerCandidateFirst+i+1);
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,authored.zone,ZoneMethod50CL::First,0.f,bounds,membershipWork),
      "GC authored trigger initial attachment"),"GC trigger zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"GC authored trigger cold update"),"GC trigger cold membership failed");
 }
 auto refreshAnimatedCandidates=[&](){
  if(gcAnimatedWorld){
  gc_animated_candidates50IG::sync(*gcAnimatedWorld,candidates.data(),candidates.size(),animatedCandidateFirst);
  for(std::size_t i=0;i<gcAnimatedMembership.size();++i){
   const auto bounds=gc_animated_candidates50IG::bounds(*gcAnimatedWorld,i);
   const auto result=updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,gcAnimatedMembership[i]->state,bounds,membershipWork);
   if(!reportMembership(result,"GC original animation live update"))return false;
   const auto index=animatedCandidateFirst+i;auto& mirror=movementCandidates[index].collision;
   mirror.geometry.worldMatrix=candidates[index].candidate.collision.geometry.worldMatrix;
   mirror.sphere=candidates[index].candidate.collision.sphere;
  }
  }
  if(gaAnimatedWorld){
  ga_animated_candidates::sync(*gaAnimatedWorld,candidates.data(),candidates.size(),animatedCandidateFirst);
  for(std::size_t i=0;i<gaAnimatedMembership.size();++i){
   const auto bounds=ga_animated_candidates::bounds(*gaAnimatedWorld,i);
   const auto result=updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,gaAnimatedMembership[i]->state,bounds,membershipWork);
   if(!reportMembership(result,"GA original animation live update"))return false;
   const auto index=animatedCandidateFirst+i;auto& mirror=movementCandidates[index].collision;
   mirror.geometry.worldMatrix=candidates[index].candidate.collision.geometry.worldMatrix;
   mirror.sphere=candidates[index].candidate.collision.sphere;
  }
  }
  return true;
 };
 LiveZoneBounds50EN mainInitialBounds{},secondaryInitialBounds{};
 require50DU(prepareQueryCache(query,s.nodes[static_cast<int>(x.bindings.lifecycle.main190)].world100,mainInitialBounds)&&
     prepareQueryCache(secondaryQuery,s.nodes[static_cast<int>(x.bindings.lifecycle.physical19C)].world100,secondaryInitialBounds),
     "source initial spring matrix and sphere preparation");
 // Source grid-start1AE578..1AE594 uses collision4C4[0], the first actual zone.
 // Collision registration120070 passes overlap radius0, independently of cache.
 if(!isRaTrack)require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,mainMembership,
     mainMembershipStorage,mainQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.0f,mainInitialBounds,membershipWork),
     "main initial attachment"),"attach main live spring membership");
 if(!isRaTrack)require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,secondaryMembership,
     secondaryMembershipStorage,secondaryQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.0f,secondaryInitialBounds,membershipWork),
     "secondary initial attachment"),"attach secondary live spring membership");

 TerrainPrepareInput48M coldTerrainInput{};
 coldTerrainInput.state=terrainOwner.initial;
 coldTerrainInput.scene=s.nodes[static_cast<int>(x.bindings.lifecycle.main190)].world100;
 coldTerrainInput.lowerEndpoint140=terrainOwner.lowerEndpoint140;
 TerrainPrepareState48M coldTerrainPrepared{};
 require50DU(prepareTerrainSinglePair48M(coldTerrainInput,coldTerrainPrepared)==TerrainPrepareStatus48M::QueryReady,
     "prepare initial authored terrain query bounds");
 const auto terrainInitialBounds=live_terrain_probe50fr::bounds(terrainLiveOwner,coldTerrainPrepared);
 if(!isRaTrack)require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,terrainMembership,
     terrainMembershipStorage,terrainQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.f,
     terrainInitialBounds,membershipWork),"terrain initial attachment"),"attach terrain probe membership");
 std::vector<std::uint32_t> terrainStack(stackCount);
 live_terrain_probe50fr::Context terrainContext{zoneLifecycle,liveWorld,terrainMembership,membershipWork,
     terrainQueryRef,trackStorage.terrainTargets.data(),terrainOwnerCount,
     trackStorage.terrainSelected.data(),trackCounts.selected,
     {trackStorage.terrainScratchEffects.data(),trackCounts.selected,terrainStack.data(),terrainStack.size()},
     trackStorage.terrainEffects.data(),trackCounts.selected};
 std::uint64_t terrainQueries=0,terrainHitFrames=0,terrainSupportedFrames=0;
 const VehicleLiveQueryContext50EQ mainLiveQuery{zoneLifecycle,liveWorld,mainMembership,membershipWork,mainQueryRef};
 const VehicleLiveQueryContext50EQ secondaryLiveQuery{zoneLifecycle,liveWorld,secondaryMembership,membershipWork,secondaryQueryRef};
 // Original 1A7164: after the .spl AI/map splines (2A1260) and portal index 1ACF70.
 rr::platform::loading_progress50DU::step(.1f,0x1a7164);
 std::uint64_t mainCrossedPortals=0,secondaryCrossedPortals=0;
 std::uint64_t reportedGcTriggerDispatches=0; // shared world observations, outside racer bank
 auto acceptLiveQuery=[&](const VehicleLiveQueryResult50EQ& attempt,
                         Type2StaticQuery49X& source,const LiveZoneMembership50EN& membership,
                         std::uint64_t& crossedPortals,const char* operation){
  // Prefix writes precede narrowphase in the source. Keep accepted cache/E0
  // changes even if the later indexed query rejects and the client stops.
  if(attempt.prefixCommitted){
   addCounter50DU(crossedPortals,static_cast<std::uint64_t>(attempt.prefix.crossedPortals));
   source.worldMatrix=membership.lastShapeWorld;
   for(unsigned i=0;i<3;++i)source.sphere.center.v[i]=membership.bounds.cachedCenter.v[i];
   source.sphere.radius=membership.bounds.cachedRadius;
   if(gcTriggerActions&&gcTriggerActions->ready())require50DU(gcTriggerActions->observePrepared(
       source.filter->identity,*source.resource,source.worldMatrix),"GC trigger lifecycle query pose unavailable");
   if(gaTriggerActions&&gaTriggerActions->ready())require50DU(gaTriggerActions->observePrepared(
       source.filter->identity,*source.resource,source.worldMatrix),"GA trigger lifecycle query pose unavailable");
   if(tbTriggerActions&&attempt.status==VehicleLiveQueryStatus50EQ::Complete)
    require50DU(tbTriggerActions->observePrepared(source.filter->identity,*source.resource,source.worldMatrix),
        "TB actual completed query pose observation rejected");
   if(tcTriggerActions&&attempt.status==VehicleLiveQueryStatus50EQ::Complete)
    require50DU(tcTriggerActions->observePrepared(source.filter->identity,*source.resource,source.worldMatrix),
        "TC actual completed query pose observation rejected");
   if(isRcTrack){
    requireMbRuntimeNavigation50MB();
    require50DU(rcTriggerActions&&rcTriggerActions->ready()&&
        rcTriggerActions->observePrepared(source.filter->identity,*source.resource,source.worldMatrix),
        "RC trigger actual prepared COL pose unavailable");
   }
   if(scTriggerActions&&scTriggerActions->ready())require50DU(scTriggerActions->observePrepared(
       source.filter->identity,*source.resource,source.worldMatrix),"SC trigger actual prepared COL pose unavailable");
   if(maTriggerActions&&maTriggerActions->ready())require50DU(maTriggerActions->observePrepared(
       source.filter->identity,*source.resource,source.worldMatrix),"MA trigger lifecycle query pose unavailable");
   if(saTriggerActions&&saTriggerActions->ready())require50DU(saTriggerActions->observePrepared(
       source.filter->identity,*source.resource,source.worldMatrix),"SA trigger lifecycle query pose unavailable");
  }
  if(gcTriggerActions&&gcTriggerActions->nativeGroup().dispatches!=reportedGcTriggerDispatches){
   const auto& group=gcTriggerActions->nativeGroup();const auto& actions=gcTriggerActions->counters();
   reportedGcTriggerDispatches=group.dispatches;
   std::cerr<<"GC WORLD ACTION: dispatches="<<group.dispatches<<" lost_ai_writes="<<actions.lostAiWrites
       <<" opening_active="<<gcAnimatedWorld->state(0).sequence.active90
       <<" sound_starts="<<actions.soundStarts<<" sound_culled="<<actions.soundCulled
       <<" policy=native_swept_type2_segments\n";
  }
  if(attempt.status==VehicleLiveQueryStatus50EQ::Complete)return true;
  std::cerr<<operation<<" live-query status "<<static_cast<unsigned>(attempt.status)
           <<" query "<<static_cast<unsigned>(attempt.queryStatus)
           <<" prefix-committed "<<attempt.prefixCommitted
           <<" revision "<<attempt.membershipRevision<<'\n';
  if(attempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected)
   reportMembership(attempt.prefix,operation);
  return false;
 };

 auto* mergedIds=trackStorage.mergedIds.data();auto* secondIds=trackStorage.secondIds.data();
 auto* resolveScratch=trackStorage.resolveScratch.data();auto* ownerHandles=trackStorage.handles.data();
 auto& collectionInput=trackStorage.collectionInput;
 collectionInput.zoneLinks=mainMembership.storage.links;collectionInput.zoneLinkCount=mainMembership.storage.capacity;collectionInput.zoneHead=1;collectionInput.query=mainQueryRef;
 collectionInput.bounds.shapeTag=2;collectionInput.type2Resource=query.resource;collectionInput.directTypeTag=query.collisionTypeTag;collectionInput.groupTypeTag=query.groupTypeTag;
 auto collectionInitial=trackStorage.initialCollection();
 auto collectionWork=trackStorage.collectionWorkspace();
 std::uint64_t collectedCalls=0;std::size_t peakCollected=0;
 native_original_ai_recovery::State originalRecovery;
 native_original_human_recovery::State originalHumanRecovery;
 native_original_human_recovery::Diagnostic originalManualRecoveryDiagnostic;
 std::uint64_t originalManualResetSerial=0,originalManualResetGraphicsDue=0;
 GameplayBlurPending sourceBlur;
 native_original_human_recovery::Diagnostic originalHumanRecoveryDiagnostic;
 BaseUndertrackContinuation50X humanBaseUndertrack;
 // Installed only by the concrete prepared Session below; this callback
 // executes its actual17BCE0/178860/179A80 operations in the current bank0 visit.
 std::function<bool(const UndertrackSelection50ET&,const UndertrackPlacement50ET&,std::uint64_t)> resetHumanCamera;
 native_original_ai_recovery::Models originalRecoveryModels;
 native_original_ai_recovery::Diagnostic originalRecoveryDiagnostic;
 auto originalRecoveryWorkspace=std::make_unique<UndertrackPhysicsWorkspace50EW>();
 if(isRaTrack){
  query.resource=&racerCollisions[0]->main();secondaryQuery.resource=&racerCollisions[0]->secondary();
  query.callbackEnabled=secondaryQuery.callbackEnabled=true;
  query.skipMask=secondaryQuery.skipMask=8u;
  terrain_probe_authoring50fq_fixture::Owner selectedProbe;std::string error;
  require50DU(terrain_probe_authoring50fq_fixture::prepare(selectedProbe,query,x.parameters,
      racerRigs[0]->phy,terrainBindings,error,*racerCollisions[0])&&
      terrain_probe_authoring50fq_fixture::installProbeCold(terrainOwner,terrainLiveOwner,selectedProbe,error),
      "actual RA selected terrain constructor rejected");
  require50DU(raWorld->bindRuntimeQueryOwners({s,collisionNormalMode,query,qfilter,secondaryQuery,
      secondaryFilter,*racerCollisions[0],terrainOwner,terrainLiveOwner,&recoveryQueryOwners,&movement},raDiagnostic),
      raDiagnostic.failure?raDiagnostic.failure:"actual RA query owners rejected");
 }
 // The counted World/domain already exists; bind only the actual Context's
 // selection here, after its genuine runtime query owners. Scope withdrawal is
 // before Context references die, including exceptions and continuation return.
 native_world_water50LA::Domain::Lease nativeWaterContextLease;
 if(isRaTrack&&consumedRaStage.race()){
  nativeWaterContextLease=raWorld->bindNativeWaterContext(*this);
  require50DU(static_cast<bool>(nativeWaterContextLease),
      "actual RA native water owner selection rejected");
 }
 //One concrete native policy replaces the frame driver's former literal
 //detector admissions. Its native lanes are registered by actual Storage;
 //the guard revokes borrows before this Context's settings/callbacks disappear.
 require50DU(!trackStorage.nativeDetectorPolicy_,"native detector policy already owned");
 auto nativeDetectorPolicy=std::shared_ptr<native_spring_detector_policy50LA::Owner>(
     new native_spring_detector_policy50LA::Owner(s,x.bindings,query,spring,settings,
         mainMembership,mainQueryRef,secondaryQuery,secondarySpring,secondarySettings,
         secondaryMembership,secondaryQueryRef));
 native_spring_detector_policy50LA::Owner::Lease nativeDetectorLease{nativeDetectorPolicy};
 trackStorage.nativeDetectorPolicy_=nativeDetectorPolicy;
 std::unique_ptr<original_ai_reset_ground_bridge::GroundBridge> originalResetGround;
 if(originalRecoveryEnabled)originalResetGround=std::make_unique<original_ai_reset_ground_bridge::GroundBridge>(
     trackStorage,actualWorld,collisionNormalMode,query,qfilter,secondaryQuery,secondaryFilter,
     terrainOwner.metadata,s,recoveryQueryOwners,movement,terrainBindings.floorCategory);
 std::size_t maximumRecoveryMeshStack=0;
 if(originalRecoveryEnabled)for(const auto& collision:racerCollisions)if(collision){
  const auto& mesh=collision->mainTarget().mesh;
  maximumRecoveryMeshStack=std::max(maximumRecoveryMeshStack,mesh.nodes.size()+mesh.faces.size()+1u);
 }
 std::unique_ptr<original_follow_query_bridge::QueryBridge> legacyFollowQueries;
 if(originalCameraEnabled&&!isRaTrack)legacyFollowQueries=std::make_unique<original_follow_query_bridge::QueryBridge>(
     trackStorage,ground,collisionNormalMode,query,qfilter,secondaryQuery,secondaryFilter,terrainOwner.metadata,
     originalRecoveryEnabled?&recoveryQueryOwners:nullptr,originalRecoveryEnabled?&s:nullptr,
     originalRecoveryEnabled?&movement:nullptr,maximumRecoveryMeshStack);
 auto* sourceFollowQueries=isRaTrack?raWorld->nativeFollowQueries():legacyFollowQueries.get();
 if(originalResetGround&&sourceFollowQueries)
  require50DU(originalResetGround->bindCanonicalTailOwner(*sourceFollowQueries),
      "actual reset camera/wreck tail owner binding rejected");
 using EndQueryBridge=ra_source_camera50LA::OwnershipBridge;
 EndQueryBridge::EndCandidateProducer* endCandidateProducer=nullptr;
 auto projectRegistered=[&](){
  //50CL keeps method descriptors current after each explicit registry change.
  for(std::size_t i=0;i<terrainOwnerCount;++i){const auto&owner=candidates[i].candidate.collision;auto projected=projectCollectionObject50CF(owner.filter,owner.sphere,owner.typeTag,candidates[i].shapeType,0);if(!projected.valid)return false;collectionObjects[i]=projected.object;collectionSource[i]=projected.state;}
  if(!mainMembership.attached||!secondaryMembership.attached||
     mainMembership.object!=mainQueryRef||secondaryMembership.object!=secondaryQueryRef||
     mainMembership.lastShapeTag!=2||secondaryMembership.lastShapeTag!=2||
     mainMembership.lastType2Resource!=query.resource||
     secondaryMembership.lastType2Resource!=secondaryQuery.resource)return false;
  // Outer219AF0/219B14 consumes the prior accepted matrix/sphere and E0.
  //1223E4 calls1202E0 directly; fresh11FE50/11F680/120230 belongs only to
  // the later spring prefix. Secondary will reuse this exact collected list.
  LiveZoneBounds50EN collectionBounds{};collectionBounds.bounds.shapeTag=2;
  collectionBounds.bounds.shapeWorld=mainMembership.lastShapeWorld;
  collectionBounds.bounds.cachedCenter=mainMembership.bounds.cachedCenter;
  collectionBounds.bounds.cachedRadius=mainMembership.bounds.cachedRadius;
  collectionBounds.type2Resource=query.resource;
  CollisionSphere48J mainCollectionSphere{},secondaryCollectionSphere{};
  for(unsigned i=0;i<3;++i){
   mainCollectionSphere.center.v[i]=mainMembership.bounds.cachedCenter.v[i];
   secondaryCollectionSphere.center.v[i]=secondaryMembership.bounds.cachedCenter.v[i];
  }
  mainCollectionSphere.radius=mainMembership.bounds.cachedRadius;
  secondaryCollectionSphere.radius=secondaryMembership.bounds.cachedRadius;
  const auto mainProjection=projectCollectionObject50CF(qfilter,mainCollectionSphere,query.collisionTypeTag,2,
      std::bit_cast<std::uint32_t>(collectionBounds.bounds.cachedCenter.v[3]));
  const auto secondaryProjection=projectCollectionObject50CF(secondaryFilter,secondaryCollectionSphere,
      secondaryQuery.collisionTypeTag,2,std::bit_cast<std::uint32_t>(secondaryMembership.bounds.cachedCenter.v[3]));
  CollisionSphere48J terrainCollectionSphere{};
  for(unsigned i=0;i<3;++i)terrainCollectionSphere.center.v[i]=terrainMembership.bounds.cachedCenter.v[i];
  terrainCollectionSphere.radius=terrainMembership.bounds.cachedRadius;
  const auto terrainProjection=projectCollectionObject50CF(terrainOwner.metadata.filter,
      terrainCollectionSphere,terrainOwner.metadata.collisionTypeTag,2,
      std::bit_cast<std::uint32_t>(terrainMembership.bounds.cachedCenter.v[3]));
  if(!mainProjection.valid||!secondaryProjection.valid||!terrainProjection.valid)return false;
  collectionObjects[terrainQueryRef-1]=terrainProjection.object;
  collectionSource[terrainQueryRef-1]=terrainProjection.state;
  collectionObjects[mainQueryRef-1]=mainProjection.object;collectionSource[mainQueryRef-1]=mainProjection.state;
  collectionObjects[secondaryQueryRef-1]=secondaryProjection.object;collectionSource[secondaryQueryRef-1]=secondaryProjection.state;
  if(!publishRecoveryQueries())return false;
  collectionInput.zoneLinks=mainMembership.storage.links;collectionInput.zoneLinkCount=mainMembership.storage.capacity;
  collectionInput.zoneHead=1;collectionInput.bounds=collectionBounds.bounds;
  collectionInput.initialCurrentIdentity=query.initialQueryCurrentIdentity;
  //Native representation publication, not an original query effect. Only
  //the actual materializer completion may advance this producer's revision.
  return !endCandidateProducer||endCandidateProducer->projected();
 };
 auto collectRegistered=[&](){
  // Each HOST racer bank restores its own collection cache. Before it queries,
  // publish the one shared world's latest accepted compound pose/membership.
  if(!refreshAnimatedCandidates())return false;
  const auto applied=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,trackCounts.events);
  if(applied.status!=ZoneQueueStatus50CQ::Complete||applied.remaining)return false;
  if(prepareSearchView50BT(registry,registrationBlocks,2,registrationView)!=SearchStorageStatus50BS::Complete)return false;
  if(!projectRegistered())return false;
  const auto result=collectVehicleCandidates50CA(collectionInput,collectionInitial,collectionWork);
  if(result.phase!=VehicleCandidateResult50CA::Phase::Complete){std::cerr<<"collection phase "<<int(result.phase)<<" status "<<int(result.collectionStatus)<<'\n';return false;}
  if(resolveCandidateHandles50CB(mergedIds,result.count,ownerHandles,terrainOwnerCount,resolveScratch,track_runtime_storage50IG::sourceSelectionCapacity,registeredIndices,trackCounts.selected)!=CandidateHandleStatus50CB::Complete)return false;
  if(resolveCandidateHandles50CB(secondIds,result.second.count,ownerHandles,terrainOwnerCount,resolveScratch,track_runtime_storage50IG::sourceSelectionCapacity,probeIndices,trackCounts.selected)!=CandidateHandleStatus50CB::Complete)return false;
  probeCount=static_cast<std::size_t>(result.second.count);
  terrainOwner.metadata.filter.flagsF8=collectionSecond[terrainQueryRef-1].flagsF8;
  registeredCount=static_cast<std::size_t>(result.count);peakCollected=std::max(peakCollected,registeredCount);addCounter50DU(collectedCalls);
  for(std::size_t i=0;i<terrainOwnerCount;++i)candidates[i].candidate.collision.filter.flagsF8=collectionSecond[i].flagsF8;
  qfilter.flagsF8=collectionSecond[mainQueryRef-1].flagsF8;
  secondaryFilter.flagsF8=collectionSecond[secondaryQueryRef-1].flagsF8;query.initialQueryCurrentIdentity=result.second.currentIdentity;
  if(originalRecoveryEnabled){recoveryQueryOwners.podFilter.flagsF8=collectionSecond[trackCounts.podQuery-1u].flagsF8;
   movement.filter.flagsF8=collectionSecond[trackCounts.movementQuery-1u].flagsF8;}
  if(sourceFollowQueries&&!sourceFollowQueries->publishCollection(collectionSecond,collectionInput.objectCount))return false;
  std::memcpy(treeSource,treeSecond,trackCounts.methods*sizeof(*treeSource));return true;
 };
 //Private RA source continuation borrows ONLY this real materializer and
 //the actual publication owners. It never calls collectRegistered or refresh
 //geometry/animation, and refuses pending registrations rather than draining
 //them at a guessed source point. RA has no GC/GA animated candidate producer.
 std::unique_ptr<EndQueryBridge::EndCandidateProducer> ownedEndCandidateProducer;
 if(isRaTrack){
  auto runtime=raWorld->runtimeQueries_;
  require50DU(runtime&&runtime->storage_.get()==&trackStorage&&runtime->actual_&&
      &runtime->actual_->frame==&s && &runtime->actual_->main==&query &&
      &runtime->actual_->secondary==&secondaryQuery && &runtime->actual_->mainFilter==&qfilter &&
      &runtime->actual_->secondaryFilter==&secondaryFilter && &runtime->actual_->terrain==&terrainOwner &&
      runtime->actual_->recovery==&recoveryQueryOwners && runtime->actual_->movement==&movement &&
      runtime->follow_.get()==sourceFollowQueries && !runtime->endCandidateProducer_,
      "RA actual compact projection producer identity rejected");
  auto producer=std::make_unique<EndQueryBridge::EndCandidateProducer>();
  producer->owner=runtime;producer->storage=&trackStorage;
  producer->bodyRegistry=&dynamicRegistry; //Actual Context lifetime; no fresh identity namespace.
  producer->run=[&](EndQueryBridge::EndCandidateResult& out){
   if(!endCandidateProducer||!endCandidateProducer->inCall||gcAnimatedWorld||gaAnimatedWorld||
      zoneQueue.count||!sourceFollowQueries||
      prepareSearchView50BT(registry,registrationBlocks,2,registrationView)!=SearchStorageStatus50BS::Complete||
      !projectRegistered())return;
   out.projectionRevision=endCandidateProducer->activeRevision;
   if(!out.projectionRevision)return;
   if(trackStorage.candidates.size()!=terrainOwnerCount)return;
   for(std::size_t i=0;i<terrainOwnerCount;++i)
    if(trackStorage.candidates[i]!=&candidates[i])return;
   auto input=collectionInput;
   //Actual219AD4/219AE4 dereference, not a stale cold collectionInput resource.
   input.query=mainQueryRef;input.type2Resource=query.resource;
   input.directTypeTag=query.collisionTypeTag;input.groupTypeTag=query.groupTypeTag;
   const auto publish=[&](const CandidateCollectionBuffers50BX& accepted,
                          const CandidateCollectionResult50BX& result){
    //Each returned122350 publishes independently, before the next source call.
    //No late merge transaction can roll back the first query's F8/current/tree.
    for(std::size_t i=0;i<terrainOwnerCount;++i)
     candidates[i].candidate.collision.filter.flagsF8=accepted.objects[i].flagsF8;
    terrainOwner.metadata.filter.flagsF8=accepted.objects[terrainQueryRef-1].flagsF8;
    qfilter.flagsF8=accepted.objects[mainQueryRef-1].flagsF8;
    secondaryFilter.flagsF8=accepted.objects[secondaryQueryRef-1].flagsF8;
    query.initialQueryCurrentIdentity=result.currentIdentity;
    if(originalRecoveryEnabled){
     recoveryQueryOwners.podFilter.flagsF8=accepted.objects[trackCounts.podQuery-1u].flagsF8;
     movement.filter.flagsF8=accepted.objects[trackCounts.movementQuery-1u].flagsF8;
    }
    //Keep actual compact source aliases in step with accepted native effects;
    //no geometry callback, registration or query is replayed to rebuild them.
    std::memcpy(collectionSource,accepted.objects,input.objectCount*sizeof(*collectionSource));
    std::memcpy(treeSource,accepted.trees,input.treeCount*sizeof(*treeSource));
    return sourceFollowQueries->publishCollection(accepted.objects,input.objectCount);
   };
   input.methodMask=2;input.skipSphere=true;input.ignoreGroups=true;input.nominalCapacity=64;
   CandidateCollectionResult50BX first{};
   out.stoppedPC=0x219af0;
   if(collectCandidatesBounded50BX(input,collectionInitial,collectionWork.collector,
       collectionWork.first,first)!=CandidateCollectionStatus50BX::Complete)return;
   out.firstReturned=true;out.firstCount=first.count;out.stoppedPC=0x219af8;
   if(!publish(collectionWork.first,first))return;
   //219AF8 re-reads the same main358/query38 owner. No fresh spring/cache step.
   out.stoppedPC=0x219b14;
   if(query.resource!=input.type2Resource||!mainMembership.attached||
      mainMembership.object!=mainQueryRef||mainMembership.lastType2Resource!=query.resource)return;
   input.methodMask=1;input.skipSphere=false;input.ignoreGroups=false;
   input.initialCurrentIdentity=first.currentIdentity;input.initialActiveTree=first.activeTree;
   CandidateCollectionState50BX afterFirst{};
   afterFirst.objects=collectionWork.first.objects;afterFirst.objectCapacity=collectionWork.first.objectCapacity;
   afterFirst.trees=collectionWork.first.trees;afterFirst.treeCapacity=collectionWork.first.treeCapacity;
   afterFirst.pools=collectionWork.first.pools;afterFirst.poolCapacity=collectionWork.first.poolCapacity;
   afterFirst.poolBlocks=collectionWork.first.poolBlocks;afterFirst.poolBlockCapacity=collectionWork.first.poolBlockCapacity;
   afterFirst.poolEntries=collectionWork.first.poolEntries;afterFirst.poolEntryCapacity=collectionWork.first.poolEntryCapacity;
   afterFirst.shape0States=collectionWork.first.shape0States;afterFirst.shape0Capacity=collectionWork.first.shape0Capacity;
   CandidateCollectionResult50BX second{};
   if(collectCandidatesBounded50BX(input,afterFirst,collectionWork.collector,
       collectionWork.second,second)!=CandidateCollectionStatus50BX::Complete)return;
   out.secondReturned=true;out.secondCount=second.count;out.stoppedPC=0x219b1c;
   if(!publish(collectionWork.second,second))return;
   out.stoppedPC=0x219b1c;
   CandidateMergeResult48S merged{};
   const CandidateMergeInput48S merge{
       {first.count,collectionWork.first.identities,collectionWork.first.identityCapacity},
       {second.count,collectionWork.second.identities,collectionWork.second.identityCapacity}};
   if(mergeCandidates48S(merge,collectionWork.mergeScratch,collectionWork.merged,merged)!=
       CandidateMergeStatus48S::Complete)return;
   //Existing native consumer mapping; never guest pointer words or fake358.
   if(resolveCandidateHandles50CB(mergedIds,merged.count,ownerHandles,terrainOwnerCount,
       resolveScratch,track_runtime_storage50IG::sourceSelectionCapacity,registeredIndices,
       trackCounts.selected)!=CandidateHandleStatus50CB::Complete)return;
   registeredCount=static_cast<std::size_t>(merged.count);
   if(resolveCandidateHandles50CB(secondIds,second.count,ownerHandles,terrainOwnerCount,
       resolveScratch,track_runtime_storage50IG::sourceSelectionCapacity,probeIndices,
       trackCounts.selected)!=CandidateHandleStatus50CB::Complete)return;
   probeCount=static_cast<std::size_t>(second.count);
   out.merged=true;out.mergedCount=merged.count;out.stoppedPC=0x219bd0;return;
  };
#include "ra_shared_collection_context50la.inl"
  // Capture only real Context-backed main/secondary owners. Both calls reuse
  // the same source384/388 list wired by2128E0..F4, never recollect/refresh.
  auto& mainQueryForEnd=query;auto& mainFilterForEnd=qfilter;auto& mainSpringForEnd=spring;
  auto& mainInitialForEnd=initial;auto& mainPairsForEnd=initialPairs;auto& mainMetadataForEnd=initialMetadata;
  auto& mainResultForEnd=queryResult;auto* mainRecordsForEnd=records;
  auto& mainWorkspaceForEnd=workspace;auto& mainDestinationForEnd=destination;
#include "ra_shared_direct_pod_context50la.inl"
#include "ra_shared_terrain_context50la.inl"
#include "ra_shared_spring_response_context50la.inl"
  producer->limited=[&,runtime,mainRecordsForEnd](bool secondary,const SourceQueryIdentity50MB& identity,
      const SpringVector4_48N& sentinel,EndQueryBridge::EndCandidateResult& out){
   auto& query=secondary?secondaryQuery:mainQueryForEnd;
   auto& qfilter=secondary?secondaryFilter:mainFilterForEnd;
   auto& spring=secondary?secondarySpring:mainSpringForEnd;
   auto& initial=secondary?si:mainInitialForEnd;
   auto& initialPairs=secondary?sip:mainPairsForEnd;
   auto& initialMetadata=secondary?sim:mainMetadataForEnd;
   auto& queryResult=secondary?secondaryResult:mainResultForEnd;
   auto* records=secondary?secondaryRecords:mainRecordsForEnd;
   const std::size_t recordCount=secondary?60u:40u;
   const auto& workspace=secondary?sw:mainWorkspaceForEnd;
   const auto& destination=secondary?sd:mainDestinationForEnd;
   const bool shared=bool(trackStorage.sharedRoster_);
   const auto& selected=runtime->sharedMergedSelection_;
   auto& initialized=secondary?out.nativeSecondaryLimitedInitialized:out.nativeLimitedInitialized;
   auto& returned=secondary?out.nativeSecondaryLimitedReturned:out.nativeLimitedReturned;
   auto& ordinal=secondary?out.nativeSecondaryLimitedOrdinal:out.nativeLimitedOrdinal;
   out.stoppedPC=0x295a00;
   auto* producer=endCandidateProducer;
   if(!producer||identity.issuer!=producer||!producer->inCall||
      producer->activeRevision!=identity.serial||out.projectionRevision!=identity.serial||
      !(secondary?out.nativeSecondaryQueryPrefixReturned:out.nativeQueryPrefixReturned)||!out.merged||out.mergedCount<0||
      registeredCount!=static_cast<std::size_t>(out.mergedCount)||registeredCount>trackCounts.selected||
      (shared&&(selected.projection!=identity.serial||selected.count!=registeredCount||
          selected.count>selected.entries.size()))||
      query.filter!=&qfilter||identity.queryCollider!=qfilter.identity||
      initial.pairs!=initialPairs.data()||initial.metadata!=initialMetadata.data()||
      initial.pairSlots!=initialPairs.size()||initialMetadata.size()!=initialPairs.size()||
      initial.recordSlots!=records||initial.recordCount!=recordCount||
      !runtime->registeredRange(records,initial.recordCount*sizeof(*records))||
      !runtime->registeredRange(&queryResult,sizeof(queryResult))||
      !runtime->registeredRange(&initial,sizeof(initial))||
      !runtime->registeredRange(&registeredCount,sizeof(registeredCount))||
      (registeredCount&&!shared&&!runtime->registeredRange(registeredIndices,registeredCount*sizeof(*registeredIndices)))||
      !runtime->registeredRange(&qfilter.flagsF8,sizeof(qfilter.flagsF8))||
      !runtime->registeredRange(initialPairs.data(),initialPairs.size()*sizeof(initialPairs[0]))||
      !runtime->registeredRange(initialMetadata.data(),initialMetadata.size()*sizeof(initialMetadata[0])))return;
   // Snapshot actual384 count once at295A04. No fresh collection or empty-list
   // assumption. The same materialization/Bank visit owns every retained index.
   const auto count=registeredCount;
   for(std::size_t i=0;i<count;++i){
    if(shared&&selected.entries[i].identity!=mergedIds[i])return;
    if(shared&&selected.entries[i].kind!=EndQueryBridge::SharedSelectedKind::staticCandidate)continue;
    const auto index=shared?selected.entries[i].index:registeredIndices[i];
    if(index>=terrainOwnerCount||trackStorage.candidates[index]!=&candidates[index]||
       movementCandidates[index].collision.filter.identity!=candidates[index].candidate.collision.filter.identity||
       (shared&&selected.entries[i].identity!=candidates[index].candidate.collision.filter.identity))return;
   }
   auto& binding=producer->nativeLimitedResultBorrow;
   SourceOuterResultLease50MB lease;
   // Native preflight only; this neither writes guest3DF5B8 nor infers its
   // previous value. Lifetime release revokes only the native borrow.
   if(!binding.beginOuter(identity,query,queryResult,initialPairs.data(),
       initialMetadata.data(),initialPairs.size(),lease))return;
   if(!binding.bindOuterScratch(lease,workspace,destination,shared?nullptr:registeredIndices,shared?0u:count,
       records,initial.recordCount))return;
   qfilter.flagsF8=static_cast<std::uint8_t>(qfilter.flagsF8|0x10u); //1226A4
   qfilter.flagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&~0x20u); //11DD60
   if(shared&&!runtime->publishSharedActiveQueryFlags(secondary,out))return;
   runtime->actual_->normalMode.acceptReachedLimitedQueryCounts11DD00();
   //124F70/11E2A4 type2 selected-result adaptation. Preserve actual current
   //selected bytes and stale pairs; neither spring reset nor global sentinel init.
   std::memcpy(&queryResult.selected,&initial.selected,sizeof(initial.selected));
   std::memcpy(&queryResult.selected.first,&sentinel,sizeof(sentinel));
   std::memcpy(&queryResult.selected.second,&sentinel,sizeof(sentinel));
   queryResult.selected.fraction=1.f;queryResult.pairCount=0;
   std::memcpy(&initial.selected,&queryResult.selected,sizeof(initial.selected));
   queryResult.queryFlagsF8=qfilter.flagsF8;queryResult.records=spring.records;
   queryResult.currentCandidateIdentity=query.initialQueryCurrentIdentity;
   initialized=true;
   // Native diagnostic counters are not purported source stores.
   queryResult.candidateCount=0;queryResult.geometryHits=0;queryResult.hasGeometryHit=false;
   queryResult.leafHits=0;queryResult.callbacks=0;queryResult.appended=0;queryResult.saturated=0;
   queryResult.visits=0;queryResult.traceUsed=0;queryResult.stackHighWater=0;
   queryResult.normalModeWrite={};queryResult.normalModeWrite.bound=query.normalMode54Bound;
   out.stoppedPC=0x1226b0;
   // The callback reads category through actual spring.currentCandidate130,
   // not a default query.initialCurrentCategory. Zero does not read category.
   std::uint32_t currentCategory=0;bool currentCategoryKnown=false;
   const auto readCurrentCategory=[&](){
    currentCategory=0;currentCategoryKnown=!spring.records.currentCandidate130;
    if(currentCategoryKnown)return;
    const auto extent=shared?collectionInput.objectCount:terrainOwnerCount;
    for(std::size_t n=0;n<extent;++n){
     const auto id=shared?collectionObjects[n].identity:candidates[n].candidate.collision.filter.identity;
     if(id!=spring.records.currentCandidate130)continue;
     if(currentCategoryKnown){currentCategoryKnown=false;return;}
     currentCategory=shared?collectionObjects[n].filter.categoryA4:candidates[n].candidate.collision.filter.categoryA4;
     currentCategoryKnown=true;
    }
   };
   if(shared)readCurrentCategory();
   else {
    //Retain the exact legacy early-return/ambiguity behavior.
    currentCategoryKnown=!spring.records.currentCandidate130;
    if(!currentCategoryKnown){
     for(std::size_t n=0;n<terrainOwnerCount;++n){
      const auto& current=candidates[n].candidate.collision.filter;
      if(current.identity==spring.records.currentCandidate130){
       if(currentCategoryKnown)return;
       currentCategory=current.categoryA4;currentCategoryKnown=true;
      }
     }
    }
   }
   for(std::size_t i=0;i<count;++i){
    ordinal=i;out.stoppedPC=0x1226b0;
    if(!binding.outerCurrent(lease,identity,query,queryResult,initialPairs.data(),
        initialMetadata.data(),initialPairs.size()))return;
    if(shared&&selected.entries[i].kind!=EndQueryBridge::SharedSelectedKind::staticCandidate){
     if(selected.entries[i].kind!=EndQueryBridge::SharedSelectedKind::pod694||!producer->directPod)return;
     const auto row=selected.entries[i].index;
     if(row%track_runtime_storage50IG::Storage::rosterRoles!=3u||
        !producer->directPod(secondary,row/track_runtime_storage50IG::Storage::rosterRoles,identity,lease,out))return;
     readCurrentCategory();continue;
    }
    const auto candidateIndex=shared?selected.entries[i].index:registeredIndices[i];
    auto& candidate=candidates[candidateIndex].candidate.collision;
    ++queryResult.candidateCount; //Native diagnostic only.
    if(candidate.typeTag!=query.collisionTypeTag){
     //122898 reads bit0 before GP8014/type comparison. Only the reached
     //actual group branch remains unsupported; ordinary mismatches do skip.
     if(!(qfilter.flagsF8&1u)&&candidate.typeTag==query.groupTypeTag){
      out.stoppedPC=0x1228b4;return;
     }
     continue;
    }
    query.initialQueryCurrentIdentity=candidate.filter.identity; //1226CC
    queryResult.currentCandidateIdentity=candidate.filter.identity;
    candidate.filter.flagsF8=static_cast<std::uint8_t>(candidate.filter.flagsF8|0x10u); //1226E8
    auto& mirror=movementCandidates[candidateIndex].collision;
    mirror.filter.flagsF8=candidate.filter.flagsF8;
    if(shared)collectionSource[candidateIndex].flagsF8=candidate.filter.flagsF8;
    out.stoppedPC=0x126eb0;
    bool overlaps=false;
    if(!evaluateType2CandidateSphere50CC(query.sphere,candidate.sphere,overlaps))return;
    if(!overlaps)continue;
    out.stoppedPC=0x120e70;
    if(!passType2CandidateFilter50CC(qfilter,candidate.filter))continue;
    out.stoppedPC=0x121434;
    if(!runtime->actual_->normalMode.acceptReachedSourceDispatch50MB(
        query.normalMode54,query.normalMode54Bound))return;
    recordCollisionNormalMode50JA(queryResult.normalModeWrite,query.normalMode54);
    out.stoppedPC=0x12143c;
    const auto& owner=candidates[candidateIndex];
    // Native atomic numerical admission. Unrepresented target/E8/dynamic E4
    // cannot be claimed as a completed geometry miss, nor replayed later.
    if(query.normalMode54!=0u||(owner.shapeType!=0&&owner.shapeType!=1)||
       candidate.hasResponseCallbackE8||
       (query.callbackEnabled&&!(candidate.filter.categoryA4&query.skipMask)&&
        (!currentCategoryKnown||(candidate.filter.categoryA4&query.dynamicMask))))return;
    TerrainCandidateEffects48J currentEffect{};currentEffect.identity=candidate.filter.identity;
    currentEffect.flagsF8=candidate.filter.flagsF8;
    std::memcpy(currentEffect.copied40.data(),candidate.copied40.data(),sizeof(currentEffect.copied40));
    std::memcpy(currentEffect.copied50.data(),candidate.copied50.data(),sizeof(currentEffect.copied50));
    Type2StaticResult49X previous{};std::memcpy(&previous,&queryResult,sizeof(previous));
    previous.records=spring.records;previous.queryFlagsF8=qfilter.flagsF8;
    --previous.candidateCount; //Single-candidate primitive owns this increment.
    initial.records=spring.records;
    const Type2WorldContinuation50CC continuation{&previous,currentCategory,nullptr,&currentEffect};
    Type2StaticResult49X next{};auto nextCategory=currentCategory;
    if(continueType2WorldCandidate50CC(query,owner,initial,workspace,destination,
        continuation,next,nextCategory)!=Type2StaticStatus49X::Complete)return;
    // Publish each completed actual candidate before reading the next one.
    // Prior candidate writes survive every later stopped admission.
    std::memcpy(initialPairs.data(),destination.pairs,initialPairs.size()*sizeof(initialPairs[0]));
    std::memcpy(initialMetadata.data(),destination.metadata,initialMetadata.size()*sizeof(initialMetadata[0]));
    std::memcpy(records,destination.records,initial.recordCount*sizeof(*records));
    std::memcpy(&queryResult,&next,sizeof(next));spring.records=next.records;initial.records=next.records;
    std::memcpy(&initial.selected,&next.selected,sizeof(initial.selected));
    qfilter.flagsF8=next.queryFlagsF8;query.initialQueryCurrentIdentity=next.currentCandidateIdentity;
    candidate.filter.flagsF8=destination.effects[0].flagsF8;
    mirror.filter.flagsF8=candidate.filter.flagsF8;
    if(shared)collectionSource[candidateIndex].flagsF8=candidate.filter.flagsF8;
    std::memcpy(candidate.copied40.data(),destination.effects[0].copied40.data(),sizeof(candidate.copied40));
    std::memcpy(candidate.copied50.data(),destination.effects[0].copied50.data(),sizeof(candidate.copied50));
    std::memcpy(mirror.copied40.data(),candidate.copied40.data(),sizeof(mirror.copied40));
    std::memcpy(mirror.copied50.data(),candidate.copied50.data(),sizeof(mirror.copied50));
    if(shared&&!runtime->publishSharedActiveQueryFlags(secondary,out))return;
    currentCategory=nextCategory;
   }
   //122B54 tests actual bit5. On shape2 the reached11E470 finalizer is an
   //immediate epilogue (11E490->11E61C); no vector-buffer reduction is omitted.
   //295630 ignores the122660 Boolean and returns1 after this actual loop.
   ordinal=count;returned=true;
   (secondary?out.nativeSecondaryDetectorReturned:out.nativeDetectorReturned)=true;
   out.stoppedPC=secondary?0x219c10:0x219be4;
  };
  producer->contact=[&,runtime,mainRecordsForEnd](const OriginalApplicationEntities50LA& app,
      EndQueryBridge::EndCandidateResult& out){
   if(!endCandidateProducer||!endCandidateProducer->inCall||!out.firstReturned||
      out.projectionRevision!=endCandidateProducer->activeRevision||
      !out.nativeContactScenePrepared)return;
   if(trackStorage.sharedRoster_){
#include "ra_shared_contact_context50la.inl"
    return;
   }
   //219420 reads the FIRST collector58C/590, not merged384 or probe48C.
   //No new selection, sort, deduplication or synthetic empty list is supplied.
   for(std::int32_t i=0;i<out.firstCount;++i){
    out.nativeContactOrdinal=static_cast<std::size_t>(i);out.stoppedPC=0x219500;
    if(out.firstCount<0||static_cast<std::size_t>(out.firstCount)>collectionWork.first.identityCapacity||
       !collectionWork.first.identities)return;
    const auto id=collectionWork.first.identities[static_cast<std::size_t>(i)];
    const auto* found=std::lower_bound(ownerHandles,ownerHandles+terrainOwnerCount,id,
        [](const CandidateHandle50CB& handle,std::uint32_t key){return handle.identity<key;});
    if(found==ownerHandles+terrainOwnerCount||found->identity!=id||found->index>=terrainOwnerCount||
       trackStorage.candidates[found->index]!=&candidates[found->index])return;
    const auto& candidate=candidates[found->index].candidate.collision;
    if(candidate.filter.identity!=id)return;
    out.stoppedPC=0x219508;
    std::uint32_t mask=0;if(!runtime->readNativeContactPodCategory(app,mask))return;
    if(!(candidate.filter.categoryA4&mask))continue;
    out.nativeContactMatchedIdentity=id;out.stoppedPC=0x1202e0;
    //Actual candidate+A0 body is not owner9C. Unknown bounds/body callback
    //custody cannot be replaced by a terrain body or nested Bank frame replay.
    return;
   }
   out.nativeContactOrdinal=out.firstCount>0?static_cast<std::size_t>(out.firstCount):0u;
   out.nativeContactReturned=true;out.stoppedPC=0x219c74;
  };
  ownedEndCandidateProducer=std::move(producer);
  endCandidateProducer=ownedEndCandidateProducer.get();
  runtime->endCandidateProducer_=endCandidateProducer;
 }
 auto commitCandidateEffects=[&](const std::vector<TerrainCandidateEffects48J>&changed){
  if(!registeredCount)return true;
  if(changed.size()<registeredCount)return false;
  for(std::size_t i=0;i<registeredCount;++i)if(changed[i].identity!=candidates[registeredIndices[i]].candidate.collision.filter.identity)return false;
  for(std::size_t i=0;i<registeredCount;++i){auto&primary=candidates[registeredIndices[i]].candidate.collision;auto&movementView=movementCandidates[registeredIndices[i]].collision;primary.filter.flagsF8=changed[i].flagsF8;primary.copied40=changed[i].copied40;primary.copied50=changed[i].copied50;movementView.filter.flagsF8=changed[i].flagsF8;movementView.copied40=changed[i].copied40;movementView.copied50=changed[i].copied50;}
  return true;
 };
 rr::assets::PodFlameOwners50W coldFlame{};
 {using namespace rr::assets;std::ifstream input(runtime_asset_paths50LB::resolve("work/native/project/disc/PODS/POD01/POD01.RES"),std::ios::binary);std::vector<std::uint8_t> raw((std::istreambuf_iterator<char>(input)),{}),member;ResArchive48C res;DecodeError48C error;Scene48D left,right;Material48E shared,fl,fr;
 require50DU(decodeResource48C(raw,res,error),"decode actual Pod01");
 auto scene=[&](const char*name,Scene48D&out){return extractResMember48C(res,name,member,error)&&decodeScene48D(member,out,error);};
 auto material=[&](const char*name,Material48E&out){return extractResMember48C(res,name,member,error)&&decodeMaterial48E(member,out,error);};
 require50DU(scene("pods\\pod01\\englt1.psg",left)&&scene("pods\\pod01\\engrt1.psg",right),"actual engine scenes");
 require50DU(material("textures\\01_1.psm",shared)&&material("textures\\flame01l.psm",fl)&&material("textures\\flame01r.psm",fr),"actual flame materials");PodFlameError50W diag{};
 require50DU(buildPodFlameOwners50W(left,right,shared,fl,fr,static_cast<unsigned>(x.bindings.visual.left1BC)+1,static_cast<unsigned>(x.bindings.visual.right1B8)+1,coldFlame,diag),"cold flame owners");
 }


 BaseTailState50X baseTail=coldBaseTailState50X(0x3f800000u,0x3f800000u);BaseTailGlobals50X baseGlobals{};BaseTailWorkspace50X baseWorkspace{};BasePoseHistoryState50Y poseHistory{};poseHistory.nextFD4=19;
 // Explicit diagnostic cold history: all positions start at supplied pod pose.
 for(auto&slot:poseHistory.historyD50){std::memcpy(slot,pose.v+12,12);slot[3]=0x3f800000u;}
 s.impactSupplement.partnerId=0xffffffffu;FrameTimingState49I clock{};clock.lastDispatched23C=.016f;

 // Fresh unrenamed native model roots; later renames/clones are not admitted.
 rr::assets::ModelRootName50BL rootNames[2]{};SceneNodeIndex49P namedIndices[2][14];rr::assets::EffectNameModel50BL namedModels[2]{};rr::assets::EffectNames50BL namesForEffects{};
 for(unsigned side=0;side<2;++side){rr::assets::initializeFreshModelRootName50BL(rootNames[side]);const auto root=side?x.bindings.visual.right1B8:x.bindings.visual.left1BC;require50DU(x.assets.hierarchy[side].nodeCount==14,"actual engine node count");for(unsigned i=0;i<14;++i)namedIndices[side][i]=static_cast<SceneNodeIndex49P>(static_cast<int>(root)+1+i);namedModels[side]={root,&rootNames[side],x.assets.hierarchy[side],namedIndices[side],14};}
 require50DU(rr::assets::buildEffectNames50BL({s.nodes,s.sceneNodeCount},namedModels,2,namesForEffects),"actual effect attachment names");
 // Existing primary IDs1..8 are stable. Optional bottom smoke/sparks/fire
 // own IDs9..14; their source800/804/80C order is bottom left then bottom right.
 // There are still only two source808 dust owners.
 EffectOwner50BE nativeEffects[14]{};std::size_t nativeEffectCount=0;
 const char*effectFiles[]={"podLeftEngineDust.emt","podRightEngineDust.emt",
     "podLeftEngineSmoke.emt","podRightEngineSmoke.emt","podSparks.emt","podFire.emt"};
 const char*effectHashes[]={
     "0e18ac3a94b8d4abaec38f9288a96b39b800ac58b17ba499ffc33f247995bb66",
     "9e5b26cf362e261239a0c6f7ff499f4dea285862286aa8dae65611529e2b5f7f",
     "03f835faf0501dd2a0761f5d1e1996d44bb634fa636242ff94341beaec7008c0",
     "2e89d1d1c6d44804dc25687d837699ab3207e8a800ee3756875e5760226957e2",
     "0a8e6c18e9f98992770f7046eea11a7541f286c387977aedcbbf277026d180d8",
     "b122aca0eb7779d48f5f90c93f2ab2d624ee38d2f824b1843ae62e790a5e44f8"};
 std::array<std::vector<std::uint8_t>,6> effectSources;
 if(directOriginalInputs)effectSources=original_disc_effects50LB::load();
 for(unsigned i=0;i<6;++i){
  if(!directOriginalInputs)effectSources[i]=runtime_startup50fl::readBytes(std::string("evidence/effectAuthoring50BF/members/")+effectFiles[i]);
  require50DU(pod_materials50ed_fixture::detail::sha256(effectSources[i])==effectHashes[i],
      "original shared effect authoring identity differs");
 }
 native_gc_other_particle_children50KG::SharedAuthoring nativeParticleAuthoring;
 std::array<native_gc_other_particle_children50KG::Runtime,raceRacerCapacity> nativeParticleChildren;
 std::shared_ptr<mc_particle_vehicle50MC::Owner> mcParticleVehicles;
 if(isMcTrack){
  require50DU(mcWaterOwner&&mcWaterOwner->ready()&&mcParticleSource&&mcTrackParticles,"MC real water/source pool before racer producers");
  constexpr std::size_t selectedPodScratch=4u*1024u*1024u;
  reserveMcAdditionalPayload(selectedPodScratch+mc_particle_vehicle50MC::Owner::maximumAdditionalPayloadBytes(),"MC actual selected-POD spray decode and eight-racer owner exceed shared budget");
  { // Actual selected-POD decoder scratch scope, not a lifetime reservation.
  const auto raw=runtime_startup50fl::readBytes(playerArchive.string());
  require50DU(raw.size()<=1024u*1024u,"MC selected POD source exceeds cold decode bound");
  rr::assets::ResArchive48C pod;rr::assets::DecodeError48C decoded;rr::assets::DecodeLimits48C limits;
  limits.resource_file_bytes=1024u*1024u;limits.resource_decoded_bytes=3u*1024u*1024u;limits.resource_entries=512;
  std::vector<std::uint8_t> spray;
  require50DU(rr::assets::decodeResource48C(raw,pod,decoded,nullptr,limits)&&rr::assets::extractResMember48C(pod,"particle\\podSpray.emt",spray,decoded,1027),"MC actual selected-POD spray member rejected");
  mcParticleVehicles=std::make_shared<mc_particle_vehicle50MC::Owner>();std::string error;
  require50DU(mcParticleVehicles->prepare(mcParticleSource,mcWaterOwner,mcTrackParticles,effectSources,std::move(spray),mc_particle_vehicle50MC::Owner::maximumAdditionalPayloadBytes(),error,raceRacerCount),error.c_str());
  } // raw RES, decoded directory/data and local source diagnostics are destroyed.
  require50DU(mcAdditionalPayloadReserved>=selectedPodScratch,"MC scratch release accounting");
  mcAdditionalPayloadReserved-=selectedPodScratch;
 }

 std::array<std::uint64_t,raceRacerCapacity> nativeParticleChildSequence{};
 if(nativeGcWater)require50DU(nativeParticleAuthoring.parse(effectSources).okay(),
     "actual native particle derived/common authoring rejected");
 ResetEffectBindings50BK resetEffectBindings{};ResetEffectState50BK resetEffectState{};ResetEffectWorkspace50BK resetEffectWorkspace{};
 auto bindColdNativeEffects=[&](){
  require50DU(s.phase==VehicleFramePhase49W::Ready&&m.frameFactor==0.f,
      "effect owner construction requires a fresh racer");
  const bool bottomRight=x.bindings.visual.bottomRight1C0!=SceneNodeIndex49P::none;
  const bool bottomLeft=x.bindings.visual.bottomLeft1C4!=SceneNodeIndex49P::none;
  require50DU(bottomRight==bottomLeft&&bottomRight==x.bindings.visual.extra1C0Present&&
      bottomLeft==x.bindings.visual.extra1C4Present,"optional effect model presence differs");
  for(auto&owner:nativeEffects)owner={};
  nativeEffectCount=bottomRight?14u:8u;
  constexpr unsigned source[]={0,1,2,3,4,4,5,5,2,3,4,4,5,5};
  auto make=[&](unsigned i){
   const auto&raw=effectSources[source[i]];
   const auto profile=source[i]<4?EffectProfile50BE::Particle19A5E0:
       source[i]==4?EffectProfile50BE::Ordinary15F100:EffectProfile50BE::Fire15DE00;
   require50DU(rr::assets::buildEffectControlOwner50BJ(i+1,profile,
       reinterpret_cast<const char*>(raw.data()),raw.size(),{0,0},nativeEffects[i]),
       "actual effect control construction");
  };
  for(unsigned i=0;i<8;++i)make(i);
  if(bottomRight){
   //2112C8..211748 allocates bottom right then bottom left. Smoke reads its
   // side's EMT; sparks/fire copy the same primary-right authored parameters.
   // These fresh control projections use those exact authenticated EMT bytes
   // and independent records/IDs, retaining the immediate source Stop84.
   for(unsigned i:{9u,11u,13u,8u,10u,12u})make(i);
  }
  resetEffectBindings={{1,2},{0,0},{5,6,bottomLeft?11u:0u,bottomRight?12u:0u},false};
  std::memcpy(resetEffectState.timers820,s.impactSupplement.timers820,16);
  const auto before=::allocations;
  require50DU(resetEffectStops50BK(resetEffectBindings,nativeEffects,nativeEffectCount,
      resetEffectState,resetEffectWorkspace).status==EffectDispatchStatus50BE::Complete&&
      ::allocations==before,"native reset effect stop sequence");
  std::memcpy(s.impactSupplement.timers820,resetEffectState.timers820,16);
 };
 bindColdNativeEffects();
 // Source order is 1BC,1B8,1C4,1C0 for all three effect-pointer arrays.
 constexpr unsigned effectOwnerIndex[3][4]={{2,3,8,9},{4,5,10,11},{6,7,12,13}};
 ImpactEffectsWorkspace50B effectsWorkspace{};ImpactEffectEvent50B generatedEffects[maxImpactEffectEvents50B]{};EffectDispatchWorkspace50BE effectDispatch{};
 std::uint64_t effectCommands=0;
 auto consumeEffects=[&](const ImpactEffectEvent50B*events,std::size_t count){
  const auto result=consumeEffectControls50BE(events,count,nativeEffects,nativeEffectCount,effectDispatch);
  if(result.status!=EffectDispatchStatus50BE::Complete)return false;
  addCounter50DU(effectCommands,count);return true;
 };
 std::uint64_t completedFrames=0,mainContactFrames=0,secondaryContactFrames=0,repairActiveTicks=0;
 namespace gcLighting=rr::game::original_gc_gameplay_lighting_candidate;
 gcLighting::Owner legacyLightOwner;
 require50DU(!isTcTrack||(tcLightOwner&&tcLightOwner->world()),"TC actual early lighting owner absent");
 require50DU(!isRcTrack||(rcColdSources&&rcColdSources->ready()&&rcLightOwner&&rcLightOwner->world()&&
     rcLightOwner->world()->parameters.original_bytes==rcColdSources->world().parameters.original_bytes),
     "RC actual shared source lighting owner absent");
 require50DU(!isTbTrack||(tbColdSources&&tbColdSources->ready()&&tbLightOwner&&tbLightOwner->world()&&
     tbLightOwner->world()->parameters.original_bytes==tbColdSources->world().parameters.original_bytes),
     "TB actual shared source lighting owner absent");
 auto& gcLightOwner=isMcTrack?*mcLightOwner:isGbTrack?*gbLightOwner:isTbTrack?*tbLightOwner:isRcTrack?*rcLightOwner:isTcTrack?*tcLightOwner:isRaTrack?raRuntime->lighting:legacyLightOwner;
 gcLighting::Diagnostic gcLightDiagnostic{};
 // Session owners are deliberately OUTSIDE the transient native racer bank.
 // This count advances at actual graphics success, before later failure cuts.
 std::array<std::uint64_t,raceRacerCapacity> gcGraphicsSequence{};
 std::array<NativeGcSelectedLightsCandidate,raceRacerCapacity> gcDrawLights{};
 const auto nativeLightingPolicy=nativeMcSelection?gcLighting::CachePolicy::nativeMcAuthoredZoneLightSelection:nativeGbSelection?
     gcLighting::CachePolicy::nativeGbAuthoredZoneLightSelection:nativeTbSelection?
     gcLighting::CachePolicy::nativeTbAuthoredZoneLightSelection:nativeRcSelection?
     gcLighting::CachePolicy::nativeRcAuthoredZoneLightSelection:nativeTcSelection?
     gcLighting::CachePolicy::nativeTcAuthoredZoneLightSelection:nativeScSelection?
     gcLighting::CachePolicy::nativeScAuthoredZoneLightSelection:nativeMbSelection?
     gcLighting::CachePolicy::nativeMbAuthoredZoneLightSelection:nativeRaSelection?
     gcLighting::CachePolicy::nativeRaAuthoredZoneLightSelection:nativeMaSelection?
     gcLighting::CachePolicy::nativeMaAuthoredZoneLightSelection:nativeSaSelection?
     gcLighting::CachePolicy::nativeSaAuthoredZoneLightSelection:nativeGaSelection?
     gcLighting::CachePolicy::nativeGaAuthoredZoneSpotSelection:nativeTaSelection?
     gcLighting::CachePolicy::nativeTaAuthoredZoneSpotSelection:
     gcLighting::CachePolicy::inactiveCandidateNativePerPublicationRecompute;
 // Geometry preparation intentionally refreshes only its private scene copy.
 // Native light selection instead reads the canonical frame borrowed by the
 // draw lease, so execute the real2412E4..2412FC root getter on that same frame.
 auto nativeLightFailure=[&](const char* operation,std::uint32_t racer,
     const gcLighting::DrawLease& lease){
  const auto statusName=[](gcLighting::Status status){
   switch(status){
   case gcLighting::Status::complete:return "complete";
   case gcLighting::Status::unprepared:return "unprepared";
   case gcLighting::Status::alreadyPrepared:return "alreadyPrepared";
   case gcLighting::Status::sourceIdentity:return "sourceIdentity";
   case gcLighting::Status::sourceShape:return "sourceShape";
   case gcLighting::Status::sourceFields:return "sourceFields";
   case gcLighting::Status::unsupportedScene:return "unsupportedScene";
   case gcLighting::Status::phase:return "phase";
   case gcLighting::Status::identity:return "identity";
   case gcLighting::Status::bounds:return "bounds";
   case gcLighting::Status::registration:return "registration";
   case gcLighting::Status::activeDraw:return "activeDraw";
   case gcLighting::Status::stalePublication:return "stalePublication";
   case gcLighting::Status::unsupportedPolicy:return "unsupportedPolicy";
   case gcLighting::Status::arithmetic:return "arithmetic";
   case gcLighting::Status::allocation:return "allocation";
   case gcLighting::Status::missingResetOwner:return "missingResetOwner";
   }
   return "unknown";
  };
  std::ostringstream message;
  message<<"native selected-light "<<operation<<" track="<<trackSession->world->code
      <<" racer="<<racer<<" diagnostic-racer="<<gcLightDiagnostic.racer
      <<" frame="<<completedFrames<<" frame-phase="<<static_cast<unsigned>(s.phase)
      <<" owner-status="<<statusName(gcLightDiagnostic.status)
      <<'('<<static_cast<unsigned>(gcLightDiagnostic.status)<<')'
      <<" source=0x"<<std::hex<<gcLightDiagnostic.sourcePC<<std::dec
      <<" policy="<<static_cast<unsigned>(nativeLightingPolicy)
      <<" graphics-sequence="<<(racer<gcGraphicsSequence.size()?gcGraphicsSequence[racer]:0u)
      <<" draw-generation="<<lease.generation()
      <<" membership-status="<<static_cast<unsigned>(gcLightDiagnostic.membership.status)
      <<" membership-bounds="<<static_cast<unsigned>(gcLightDiagnostic.membership.boundsStatus)
      <<" membership-geometry="<<static_cast<unsigned>(gcLightDiagnostic.membership.geometryStatus)
      <<" membership-registry="<<static_cast<unsigned>(gcLightDiagnostic.membership.registryStatus)
      <<" crossed-portals="<<gcLightDiagnostic.membership.crossedPortals
      <<" reset-status="<<static_cast<unsigned>(gcLightDiagnostic.resetVisibility.status)
      <<" reset-source=0x"<<std::hex<<gcLightDiagnostic.resetVisibility.sourcePC<<std::dec
      <<" decode-kind="<<static_cast<unsigned>(gcLightDiagnostic.decode.kind)
      <<" decode-code="<<gcLightDiagnostic.decode.code
      <<" decode-offset="<<gcLightDiagnostic.decode.offset
      <<" decode-detail="<<gcLightDiagnostic.decode.detail;
  const SceneNodeIndex49P roots[]={x.bindings.lifecycle.model1B4,x.bindings.visual.left1BC,
      x.bindings.visual.right1B8,x.bindings.visual.bottomLeft1C4,x.bindings.visual.bottomRight1C0};
  for(unsigned component=0;component<maxVehicleDrawComponents50CM;++component){
   const auto root=static_cast<std::int32_t>(roots[component]);
   message<<" component"<<component<<"-root="<<root;
   if(root>=0&&static_cast<std::uint32_t>(root)<s.sceneNodeCount&&
       static_cast<std::uint32_t>(root)<vehicleFrameNodeCount49W)
    message<<"/flags=0x"<<std::hex<<static_cast<unsigned>(s.nodes[root].sceneFlags190)<<std::dec;
  }
  throw std::runtime_error(message.str());
 };
 auto beginNativeLightDraw=[&](std::uint32_t racer,gcLighting::DrawLease& lease){
  if(racer>=gcGraphicsSequence.size()||!gcLightOwner.beginDraw(racer,s,x.bindings,
      gcGraphicsSequence[racer],lease,gcLightDiagnostic))
   nativeLightFailure("beginDraw rejected",racer,lease);
  const SceneGraph49P graph{s.nodes,s.sceneNodeCount};
  const SceneNodeIndex49P roots[]={x.bindings.lifecycle.model1B4,x.bindings.visual.left1BC,
      x.bindings.visual.right1B8,x.bindings.visual.bottomLeft1C4,x.bindings.visual.bottomRight1C0};
  const unsigned count=x.bindings.visual.bottomRight1C0!=SceneNodeIndex49P::none?5u:
      x.bindings.visual.bottomLeft1C4!=SceneNodeIndex49P::none?4u:defaultVehicleDrawComponents50CM;
  if((s.phase!=VehicleFramePhase49W::Ready&&s.phase!=VehicleFramePhase49W::PostGraphics)||
      !validVehicleDrawComponentCount50CM(count)||!graph.nodes||!validateSceneGraph49P(graph))
   nativeLightFailure("canonical root admission at0x2412E4 rejected",racer,lease);
  for(unsigned component=0;component<count;++component){
   const auto root=static_cast<std::int32_t>(roots[component]);
   if(root<0||static_cast<std::uint32_t>(root)>=graph.count)
    nativeLightFailure("canonical root binding at0x2412E4 rejected",racer,lease);
   // Preserve ready opaque caches. Dirty roots use the existing original getter,
   // never a ready-bit write, copied physics pose or replacement light position.
   if(!(graph.nodes[root].sceneFlags190&1u)&&!refreshSceneWorld49P(graph,roots[component]))
    nativeLightFailure("canonical root getter0x2897B0 at0x2412E4 rejected",racer,lease);
  }
 };
 if(nativeSelectedLights){
  const auto& bytes=isMcTrack?mcColdSources->world().parameters.original_bytes:isGbTrack?gbColdSources->world().parameters.original_bytes:isTbTrack?tbColdSources->world().parameters.original_bytes:actualWorld.parameters.original_bytes;
  // RA borrows the already-prepared lighting owner from its actual cold aggregate.
  // Never reconstruct a second world owner or fall through to GC preparation.
  if(isTcTrack)require50DU(nativeTcSelection&&tcLightOwner&&tcLightOwner->world()&&
      tcLightOwner->world()->parameters.original_bytes==bytes,
      "TC early gate visibility lighting lost its actual authenticated source");
  if(isRcTrack)require50DU(nativeRcSelection&&rcColdSources&&rcLightOwner&&rcLightOwner->world()&&
      &gcLightOwner==rcLightOwner.get()&&rcLightOwner->world()->parameters.original_bytes==bytes&&
      bytes==rcColdSources->world().parameters.original_bytes,
      "RC shared lighting lost its actual authenticated source");
  if(isMcTrack)require50DU(nativeMcSelection&&mcColdSources&&mcLightOwner&&
      mcLightOwner->mcSourceIdentity()==mcColdSources.get()&&mcLightOwner->world()==&mcColdSources->world()&&
      &gcLightOwner==mcLightOwner.get(),"MC same-source lighting owner lost");
  if(isGbTrack)require50DU(nativeGbSelection&&gbColdSources&&gbLightOwner&&gbLightOwner->world()&&
      &gcLightOwner==gbLightOwner.get()&&gbLightOwner->world()->parameters.original_bytes==bytes,
      "GB shared lighting lost its actual authenticated source");
  if(isTbTrack)require50DU(nativeTbSelection&&tbColdSources&&tbLightOwner&&tbLightOwner->world()&&
      &gcLightOwner==tbLightOwner.get()&&tbLightOwner->world()->parameters.original_bytes==bytes&&
      bytes==tbColdSources->world().parameters.original_bytes,
      "TB shared lighting lost its actual authenticated source");
  if(!isRaTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack)require50DU(nativeScSelection?gcLightOwner.prepareScNative(bytes,raceRacerCount,gcLightDiagnostic):nativeMbSelection?gcLightOwner.prepareMbNative(bytes,raceRacerCount,gcLightDiagnostic):
      nativeMaSelection?gcLightOwner.prepareMaNative(bytes,raceRacerCount,gcLightDiagnostic):
      nativeSaSelection?gcLightOwner.prepareSaNative(bytes,raceRacerCount,gcLightDiagnostic):
      nativeGaSelection?gcLightOwner.prepareGaNative(bytes,raceRacerCount,gcLightDiagnostic):
      nativeTaSelection?gcLightOwner.prepareTaNative(bytes,raceRacerCount,gcLightDiagnostic):
      gcLightOwner.prepare(bytes,raceRacerCount,gcLightDiagnostic),
      "authenticated track light/visibility owner preparation failed");
  std::cerr<<(nativeMcSelection?"NATIVE MC SELECTED LIGHTS: actual28-zone/28-portal topology,28 ambient/one directional source, no timed flash; ":nativeGbSelection?
      "NATIVE GB SELECTED LIGHTS: genuine source ambient/directional/timed light registration and original Light+A0 particle semantics; ":nativeTbSelection?
      "NATIVE TB SELECTED LIGHTS: actual10-zone/11-portal source topology and authenticated retained-section light registration; ":nativeRcSelection?
      "NATIVE RC SELECTED LIGHTS: actual15-zone/15-portal source topology,15 ambient and one directional light; ":nativeTcSelection?
      "NATIVE TC SELECTED LIGHTS: actual25-zone/26-portal topology and authored ambient/directional lights; ":nativeScSelection?
      "NATIVE SC SELECTED LIGHTS: actual30-zone/34-portal source topology and134 emitting lights; ":nativeMbSelection?
      "NATIVE MB SELECTED LIGHTS: authenticated emitting source lights and actual racer/camera visibility; ":nativeRaSelection?
      "NATIVE RA SELECTED LIGHTS: same-world authenticated cold aggregate and actual racer/camera visibility; ":nativeMaSelection?
      "NATIVE MA SELECTED LIGHTS: source numeric ambient/directional/point/spot registration, actual camera/racer visibility; ":nativeSaSelection?
      "NATIVE SA SELECTED LIGHTS: source numeric ambient reset ordering, directional overrides and74 spots, actual25-zone camera/racer visibility; ":nativeGaSelection?
      "NATIVE GA SELECTED LIGHTS: authentic olive ambient/sun and all36 authored spotlights including original Light9 defaults; actual racer visibility; explicit native zone scan/top-two selection; ":nativeTaSelection?
      "NATIVE TA SELECTED LIGHTS: authored zone/portal colors and 17 spotlights; actual racer visibility; explicit native zone scan/top-two selection; ":
      "NATIVE GC SELECTED LIGHTS: actual registered ambient/directional colors and real racer visibility; ")
      <<"native per-publication selection and host world-normal Lambert; existing material/alpha/fog/specular limits retained.\n";
 }
 std::uint64_t submittedDrawFrames=0,windowSwaps=0,nativeCalls=0,discardedHostNs=0;
 auto runNative=[&](auto&&fn,const char*label){
  // Native arithmetic requires round-to-nearest (set once at session start).
  // Host drivers/libraries may change the thread's FP mode between native
  // calls; restore it and record each repair instead of failing a later leaf.
  if(const int rounding=std::fegetround();rounding!=FE_TONEAREST){
   std::cerr<<"fp_rounding_repaired before="<<label<<" mode="<<rounding<<'\n';
   require50DU(std::fesetround(FE_TONEAREST)==0,"round-to-nearest restore failed");
  }
  if(rr::game::enforcePs2FloatMode50LA())
   std::cerr<<"fp_flush_mode_repaired before="<<label<<'\n';
  stall_watchdog50DU::mark(label);
  const bool ok=fn();addCounter50DU(nativeCalls);
  if(!ok){std::ostringstream message;message<<"native frame "<<completedFrames
   <<" "<<label<<" error="<<static_cast<unsigned>(x.diagnostic.error)
   <<" source-stage="<<std::hex<<x.diagnostic.sourceStage
   <<" body="<<static_cast<unsigned>(x.diagnostic.body);
   throw std::runtime_error(message.str());}
 };

 // Actual retained chariot and engine meshes, loaded once before creating GL.
 rr::assets::VehicleDrawAssets50CR drawAssets;
 namespace podTextures=pod_materials50ed_fixture;
 std::unique_ptr<podTextures::Owner> materialOwner;
 std::unique_ptr<pod_cables50fd_fixture::Owner> cableOwner;
 PodCableBindings50FD cableBindings{};
 // Direct original per-racer owners construct all meshes/materials/cables.
 // Never open an unrelated generated POD01 render cache for these routes.
 if(!directOriginalInputs){
 rr::assets::VehicleDrawAssetsInput50CR drawInput{};
 rr::assets::VehicleDrawAssetDiagnostic50CR drawLoadDiagnostic{};
 std::vector<std::uint8_t> drawBytes[3];SceneNodeIndex49P drawNodes[3][18]{};std::uint8_t drawPalette[3][18]{};
 const char*drawKeys[]={"cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5","210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3","f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60"};
 const unsigned drawCounts[]={18,14,14},drawBases[]={31,1,16},drawMapped[]={2,0,1};
 for(unsigned component=0;component<3;++component){
  drawBytes[component]=runtime_startup50fl::readBytes(std::string("evidence/visual49G/cacheobjects/")+drawKeys[component]+".rrscene");
  const auto&psg=x.assets.psg[drawMapped[component]];const auto padded=(drawCounts[component]+3)&~3u;
  require50DU(psg.size()>=12+padded,"actual palette suffix bounds");const auto suffix=psg.size()-(12+padded);
  auto word=[&](std::size_t at){return std::uint32_t(psg[at])|(std::uint32_t(psg[at+1])<<8)|(std::uint32_t(psg[at+2])<<16)|(std::uint32_t(psg[at+3])<<24);};
  require50DU(word(suffix)==1&&word(suffix+4)==padded&&word(suffix+8+padded)==drawCounts[component],"actual single palette record");
  for(unsigned n=0;n<drawCounts[component];++n){drawPalette[component][n]=psg[suffix+8+n];drawNodes[component][n]=static_cast<SceneNodeIndex49P>(drawBases[component]+n);}
  drawInput.component[component]={&drawBytes[component],drawNodes[component],drawCounts[component],drawPalette[component],drawCounts[component]};
 }
 require50DU(drawAssets.load(drawInput,drawLoadDiagnostic),"cold native draw owners");
 materialOwner=std::make_unique<podTextures::Owner>();
 podTextures::Request materialRequest{};materialRequest.root=runtime_asset_paths50LB::resolve("evidence/visual49G");
 materialRequest.coldSourceByteCapacity=8u*1024u*1024u;
 materialRequest.textureBudgets={5,26,1024u*1024u,1024u*1024u,256};
 for(unsigned component=0;component<3;++component){
  const auto* scene=drawAssets.scene(static_cast<VehicleDrawComponent50CM>(component));
  require50DU(scene,"material scene missing");
  materialRequest.choices[component].assign(scene->materials.size(),
      podTextures::Choice{1,1,rr::assets::MaterialTextureRequirement50EA::required,
                         podTextures::MipMode::largestRetained,0});
 }
 podTextures::Diagnostic materialDiagnostic{};
 const bool materialsLoaded=materialOwner->load(materialRequest,drawAssets,materialDiagnostic);
 require50DU(materialsLoaded,materialDiagnostic.message.empty()?"Pod material load failed":materialDiagnostic.message.c_str());
 cableOwner=std::make_unique<pod_cables50fd_fixture::Owner>();
 pod_cables50fd_fixture::Diagnostic cableAssetDiagnostic{};
 require50DU(cableOwner->load(drawAssets,x.bindings,s,cableAssetDiagnostic),
     cableAssetDiagnostic.message.empty()?"actual cable asset load failed":cableAssetDiagnostic.message.c_str());
 cableBindings=cableOwner->bindings();
 }
 PodCableMesh50FD cableMesh{};PodCableWorkspace50FD cableWorkspace{};
 PodCableDiagnostic50FD cableDiagnostic{};
 VehicleDrawWorkspace50CM drawWorkspace{};
 // Read the same persistent original49D selection used by cold handling.
 // Human host adapters target its actual numeric codes; no preset4 identity
 // assumption applies. ALL8 never validates or samples a human host binding.
 SelectedControlPreset49D selectedControls{};
 require50DU(resolveControlPreset49D(controlPresets,playerActionIndexFFC,playerSource5C,selectedControls),"separate explicit action/configuration slots");
 x.actionCodes=selectedControls.actions;x.configurationEntryNonzero=selectedControls.configurationEntryNonzero;
 DeviceButtonBank48Y initialControllerButtons{};ControllerIngress50CX controller(initialControllerButtons);
 const auto deviceDescriptors=initialDualShockDescriptors48Y();
 const auto repairButtonCode=static_cast<std::uint32_t>(selectedControls.actions.flag69C);
 const auto boostButtonCode=static_cast<std::uint32_t>(selectedControls.actions.flag698);
 const auto powerslideButtonCode=static_cast<std::uint32_t>(selectedControls.actions.flag6B4);
 // Explicit development-host topology: one enabled player under the real48Z
 // application router, with no event-handling descendants under that player.
 // The supplemental49A fields are persistent cold storage, not another frame
 // or a claim to reproduce the original player/application constructors.
 PlayerEventState49A playerEvents{};
 playerEvents.source5C=playerSource5C;playerEvents.indexFFC=playerActionIndexFFC;
 const auto playerActionRows=playerEventPresetRows49D(controlPresets);
 PlayerEventEnvironment49A playerEnvironment{};
 playerEnvironment.actionRows=playerActionRows.rows;
 playerEnvironment.actionRowCount=controlPresetSlots49D;
 playerEnvironment.flags29=1; // Supplied enabled application->28C game object.
 // Category0 context/debug and category1 auxiliary sources do not exist in
 // this host. Leave them absent, without fabricated successful-miss callbacks.
 EventContext48Z eventContext{false,0,{nullptr,0}};
 EventRecordSources48Z eventRecords{};
 PlayerEffectCall49A unavailableEffect{};
 std::function<bool()> executeOriginalManualReset;
 struct HumanActionEffects50LC {
  PlayerEffectCall49A* reached;std::function<bool()>* reset;
 } humanActionEffects{&unavailableEffect,&executeOriginalManualReset};
 PlayerEventBindings49A playerEventBindings{};
 playerEventBindings.effects={&humanActionEffects,
     [](void* opaque,const PlayerEffectCall49A& call) noexcept {
         auto& owner=*static_cast<HumanActionEffects50LC*>(opaque);*owner.reached=call;
         if(call.kind==PlayerEffectKind49A::Function2255B0&&*owner.reset)
             return (*owner.reset)(); // Synchronous actual physics AND camera.
         return false; // All other reached unsupported effects remain blocked.
     }};
 playerEventBindings.children={0x600u,noEventChild48Z,nullptr,0};
 playerEventBindings.childVisitBudget=0;
 // Same actual Player input device/held records can feed the subsequent
 // camera child. The second child is linked only after genuine cold camera
 // preparation; ALL8 retains its independent camera-only transport.
 EventChild48Z applicationChildren[2]{{noEventChild48Z,1},{noEventChild48Z,1}};
 using HumanCameraInputRoute=interactive_continuation50DU::HumanCameraInputRoute;
 HumanCameraInputRoute humanCameraInputRoute;
 JoystickApplicationBridge48Z applicationEvents{};
 applicationEvents.rootFlags228=0;
 applicationEvents.root={0x600u,0,applicationChildren,1};
 applicationEvents.visitBudget=1;
 PlayerEventChildBinding49A playerEventChild{};
 playerEventChild.childIndex=0;playerEventChild.player=&playerEvents;
 playerEventChild.environment=&playerEnvironment;playerEventChild.bindings=&playerEventBindings;
 ManagerEventCallbacks48Z eventCallbacks{};
 require50DU(bindControllerPlayerActions50EC(controller,deviceIndex,eventRecords,
     applicationEvents,playerEventChild,eventCallbacks),"live one-player action binding");
 InputEvent48Z controllerEventStorage[32]{};
 InputEventQueue48Z controllerEvents{controllerEventStorage,32,0};
 ControllerActions50EC controllerActions(controller,controllerEvents,deviceIndex);
 std::uint64_t controllerPresses=0,controllerReleases=0;
 bool recoveryPaused=false,checkpointRequested=false;
 namespace platform=rr::platform;
 // Human-only binding was validated before any configuration-sensitive
 // cold physics. Keyboard semantics and physical pad positions are explicit
 // native transport policies; neither is a generated driving controller.
 const auto& keyMapping=humanControlInput.keyboard;
 DevicePacket48Y packetTemplate{};packetTemplate.bytes[2]=packetTemplate.bytes[3]=0xff;
 DevicePacket48Y packet{};platform::KeyboardPacketDiagnostic50DR keyboardDiagnostic{};
 platform::SelectedControllerLatch50KY physicalControllerLatch{};
 platform::SelectedControllerReceipt50KY physicalControllerReceipt{};
 std::int32_t reportedPhysicalController=-2;
 // Actual native application device0/1 objects are retained even for a
 // keyboard-only session, whose fresh capability remains genuinely absent.
 // These are physical feedback owners, separate from keyboard input packets.
 ControllerFeedbackState50KZ feedbackDevices[2]{};
 platform::WindowGameController50DQ feedbackPhysicalDevice{};
 bool feedbackDeviceReady=false;
 platform::GameHost50ER* feedbackHost=nullptr;
 std::uint64_t feedbackHelpers=0,feedbackSubmissions=0,feedbackNoDevice=0,feedbackLeaseRefreshes=0;
 auto serviceFeedback=[&](float elapsed){
  if(allAiEnabled)return;
  const bool ready=usePhysicalGameController&&feedbackDeviceReady;
  ControllerFeedbackCommands50KZ commands{};
  require50DU(serviceControllerFeedback50KZ(feedbackDevices[playerSource5C],ready,ready,
      elapsed,commands),"original native controller feedback service arithmetic rejected");
  auto submit=[&](const std::array<std::uint8_t,6>& bytes){
   require50DU(feedbackHost,"actual SDL controller feedback transport not bound");
   platform::GameHostDiagnostic50ER diagnostic{};platform::ControllerRumbleReceipt50KZ receipt{};
   // Source SDK send return values did not gate timer/counter writes. Keep
   // those committed prefixes on a native transport failure; never replay.
   stall_watchdog50DU::mark("controller_rumble_submit");
   if(!feedbackHost->rumbleController50KZ(playerSource5C,feedbackPhysicalDevice.instanceId,
       bytes[0],bytes[1],receipt,diagnostic)){
    reportHost50DU(diagnostic);
    throw std::runtime_error("actual SDL controller feedback submission failed; native timer prefix retained");
   }
   if(receipt.status==platform::ControllerRumbleStatus50KZ::Submitted)++feedbackSubmissions;
   else ++feedbackNoDevice; // Explicit loss/unsupported receipt, never submitted.
  };
  for(std::size_t n=0;n<commands.count;++n)submit(commands.values[n].actuator714);
  // SDL effects expire, whereas the source actuator packet remains active
  // until a stop. Renew this bounded native100ms lease on active device ticks,
  // including after a pause. This is platform transport, not a source send or
  // a fabricated helper return; it cannot alter source timers/counters.
  const auto& actuator=feedbackDevices[playerSource5C].actuator714;
  if(ready&&!commands.count&&(actuator[0]||actuator[1])){
   submit(actuator);++feedbackLeaseRefreshes;
  }
 };
 auto stopFeedbackTransport=[&](){
  if(allAiEnabled||!feedbackHost||!usePhysicalGameController||!feedbackPhysicalDevice.connected)return;
  platform::GameHostDiagnostic50ER diagnostic{};platform::ControllerRumbleReceipt50KZ receipt{};
  stall_watchdog50DU::mark("controller_rumble_stop");
  if(!feedbackHost->rumbleController50KZ(playerSource5C,feedbackPhysicalDevice.instanceId,
      0,0,receipt,diagnostic)){
   reportHost50DU(diagnostic);throw std::runtime_error("actual SDL controller pause stop failed");
  }
  // Host pause stops physical motors only. Source durations remain frozen.
 };
 bool packetLive=false;
 // Diagnostic-only change detector; never feeds native input or action state.
 std::array<std::uint32_t,13> lastManualResetInputObservation{};
 bool manualResetInputObservationValid=false;
 auto requireActionStatus=[&](ControllerActionStatus50EC status,const char* operation){
  addCounter50DU(nativeCalls);
  if(status!=ControllerActionStatus50EC::Ok){
   std::cerr<<"controller action "<<operation<<" status "<<static_cast<unsigned>(status)
            <<" phase "<<static_cast<unsigned>(controllerActions.phase())<<'\n';
   throw std::runtime_error("native controller action lease failed");
  }
 };
 // Every accepted sample is appended, genuinely dispatched and acknowledged
 // before another poll can overwrite its transitions, including pause releases.
 auto sampleController=[&](bool advanceFeedback=false){
  require50DU(!originalUiActive,"Main-only frontend cannot sample driving input");
  // UI capture/quit/pause paths may reach this call site, but there is no
  // player device owner to sample in the explicitly selected all-AI roster.
  if(allAiEnabled)return;
  ++allAiControllerSamples;
  // The one declared source-cut pause only samples release-all packets. Keep
  // its partial native phase intact; input release is not frame continuation.
  require50DU(s.phase==VehicleFramePhase49W::Ready||(recoveryPaused&&!packetLive),
              "controller dispatch requires frame boundary or source-cut release");
  playerEvents.field40=m.vehicleState40;
  playerEnvironment.gate58=s.fields.applicationByte58;
  playerEnvironment.held.rootFlags228=applicationEvents.rootFlags228;
  playerEnvironment.held.context1898=eventContext.mask1898;
  requireActionStatus(controllerActions.samplePacket(true,true,packet,deviceDescriptors,
      dualShockActiveButtons48Y),"sample");
  require50DU(controller.deviceSample().status==DevicePacketStatus48Y::Accepted,
              "controller did not accept the composed native host packet");
  // Input manager1280E4..128140 invokes237DF0 before event dispatch. This
  // native scheduler assigns that device poll once per16ms tick; other host
  // ingress/release samples only service its event bridge. Original outer
  // polling cadence is not recovered, and no PS2 scheduler parity is claimed.
  if(advanceFeedback)serviceFeedback(.016f);
  requireActionStatus(controllerActions.appendSample(),"append");
  // Observe existing sampled transitions only; never repeat a held query or dispatch.
  const auto logManualResetTransition=[&](const char* phase){
   if(!humanControlInput.manualResetBound)return;
   const auto& sampled=controller.deviceSample();
   for(std::uint32_t i=0;i<sampled.transitionCount;++i){
    const auto& transition=sampled.transitions[i];
    if(transition.buttonCode!=static_cast<std::uint32_t>(humanControlInput.manualResetCode))continue;
    std::cerr<<"original_manual_reset_transition phase="<<phase
        <<" sample="<<allAiControllerSamples<<" transition="<<i
        <<" code="<<transition.buttonCode<<" held="<<transition.held
        <<" device="<<transition.deviceId<<" context_mask="<<transition.contextMask
        <<" packet_live="<<packetLive
        <<" flags29="<<static_cast<unsigned>(playerEnvironment.flags29)
        <<" gate58="<<static_cast<unsigned>(playerEnvironment.gate58)
        <<" root_flags="<<static_cast<unsigned>(playerEnvironment.held.rootFlags228)
        <<" context518="<<playerEnvironment.held.context518
        <<" context1898="<<playerEnvironment.held.context1898
        <<" timer1098_bits="<<std::hex<<original_material_input_diagnostics50LN::bits(playerEvents.value1098)<<std::dec
        <<" manual_reset_serial="<<originalManualResetSerial
        <<" physical_completed="<<originalHumanRecovery.physicalCompleted<<'\n';
   }
  };
  logManualResetTransition("before_dispatch");
  const auto result=controllerActions.dispatch(eventContext,eventRecords,eventCallbacks);
  logManualResetTransition("after_dispatch");
  if(humanControlInput.cameraCycleBound){ // Diagnostic: view-button delivery.
   const auto& sampled=controller.deviceSample();
   for(std::uint32_t i=0;i<sampled.transitionCount;++i){
    const auto& transition=sampled.transitions[i];
    if(transition.buttonCode!=static_cast<std::uint32_t>(humanControlInput.cameraCycleCode))continue;
    std::cerr<<"camera_cycle_transition held="<<transition.held<<" context_mask="<<transition.contextMask
        <<" dispatch_status="<<static_cast<unsigned>(result.dispatch.status)
        <<" callbacks="<<result.dispatch.callbacksInvoked
        <<" app_route="<<static_cast<unsigned>(applicationEvents.lastRoute.status)
        <<" app_callbacks="<<applicationEvents.lastRoute.callbacksInvoked
        <<" context1898="<<eventContext.mask1898<<" root_flags="<<static_cast<unsigned>(applicationEvents.rootFlags228)<<'\n';
   }
  }
  addCounter50DU(nativeCalls);
  if(result.status!=ControllerActionStatus50EC::Ok||!result.dispatchAttempted||
     result.dispatch.status!=EventDispatchStatus48Z::Complete){
   std::cerr<<"controller dispatch unavailable: status "<<static_cast<unsigned>(result.status)
            <<" manager "<<static_cast<unsigned>(result.dispatch.status)
            <<" completed "<<result.dispatch.eventsCompleted
            <<" callbacks "<<result.dispatch.callbacksInvoked;
   if(result.dispatchAttempted&&result.dispatch.callbacksInvoked){
    std::cerr<<" application route "<<static_cast<unsigned>(applicationEvents.lastRoute.status);
    if(applicationEvents.lastRoute.callbacksInvoked){
     const auto& playerResult=playerEventChild.lastResult;
     std::cerr<<" player "<<static_cast<unsigned>(playerResult.status)
              <<" held query "<<static_cast<unsigned>(playerResult.lastQuery);
     if(playerResult.effectsInvoked)
      std::cerr<<" unavailable effect "<<static_cast<unsigned>(unavailableEffect.kind);
    }
   }
   std::cerr<<'\n';
   throw std::runtime_error("native action unavailable; pending batch retained without replay");
  }
  const auto& sample=controller.deviceSample();
  require50DU(result.dispatch.eventsCompleted==sample.transitionCount&&
      result.dispatch.callbacksInvoked==sample.transitionCount,
      "controller transition batch was not fully dispatched");
  for(std::uint32_t i=0;i<sample.transitionCount;++i)
   addCounter50DU(sample.transitions[i].held?controllerPresses:controllerReleases);
  requireActionStatus(controllerActions.acknowledgeComplete(),"acknowledge");
 };
 auto composePacket=[&](bool live,const platform::WindowInput50DQ& input,std::size_t actualRacerSlot=0){
  require50DU(!originalUiActive,"Main-only frontend cannot compose a driving packet");
  if(allAiEnabled)return;
  const bool physicalHuman=usePhysicalGameController&&actualRacerSlot==0;
  const bool manualHeld=physicalHuman?platform::selectedControllerHeld50KY(input.controllers[0]):
      platform::selectedKeyboardHeld50KY(input,humanControlInput);
  if(live&&autoDriveEnabled&&manualHeld){
   autoDriveEnabled=false;courseState={};courseOutput={};
   std::cerr<<"Host course driver disabled by actual manual driving input.\n";
  }
  if(physicalHuman){
   const auto& physical=input.controllers[0];
   feedbackPhysicalDevice=physical;
   // Hardware readiness is independent of human control permission (e.g.
   // the genuine post-finish driver); SDL still sampled this attached device.
   feedbackDeviceReady=physical.connected&&input.focused&&!input.minimized&&
       !input.activityLost&&!input.eventBudgetExhausted;
   if(physical.connected)feedbackDevices[playerSource5C].capable=physical.rumbleSupported;
   // Original disconnect leaves capability/timers unchanged; real readiness
   // still prevents any transfer while the physical device is absent.
   if(reportedPhysicalController!=(physical.connected?physical.instanceId:-1)){
    reportedPhysicalController=physical.connected?physical.instanceId:-1;
    std::cerr<<"native human SDL controller instance="<<reportedPhysicalController
        <<"; disconnected/inactive input releases, reconnect requires neutral controls.\n";
   }
   const bool composed=platform::composeSelectedControllerPacket50KY(humanControlInput,physical,
       live&&input.focused&&!input.minimized&&!input.activityLost,physicalControllerLatch,packet,physicalControllerReceipt,
       !input.focused||input.minimized||input.activityLost,platform::selectedControllerCountdownHold50KY);
   if(physicalControllerReceipt.status==platform::SelectedControllerStatus50KY::unsupportedOriginalAction)
    std::cerr<<"native controller input unavailable: status="<<unsigned(physicalControllerReceipt.status)
       <<" original_descriptor="<<physicalControllerReceipt.unsupportedCode
       <<"; selected original action owner is unavailable; whole packet released, release all controls to resume; no action remapped.\n";
   require50DU(composed,"selected physical controller input snapshot is invalid");
   packetLive=physicalControllerReceipt.status==platform::SelectedControllerStatus50KY::live;
  }else{
   // Observe the caller's already-sampled, policy-filtered input before composition.
   // An invalid/unbound descriptor is reported, never dereferenced or repaired.
   const auto resetCode=humanControlInput.manualResetCode;
   const bool resetCodeValid=resetCode>=0&&resetCode<static_cast<std::int32_t>(dualShockActiveButtons48Y);
   const auto resetMapping=resetCodeValid?keyMapping.buttons[static_cast<std::size_t>(resetCode)]:
       platform::KeyboardButtonMapping50DR{};
   const std::array<std::uint32_t,13> observation{{
       input.down[23],input.pressed[23],input.released[23],
       static_cast<std::uint32_t>(live),static_cast<std::uint32_t>(packetLive),
       static_cast<std::uint32_t>(input.focused),static_cast<std::uint32_t>(input.minimized),
       static_cast<std::uint32_t>(input.activityLost),static_cast<std::uint32_t>(input.eventBudgetExhausted),
       static_cast<std::uint32_t>(humanControlInput.manualResetBound),static_cast<std::uint32_t>(resetCode),
       static_cast<std::uint32_t>(resetMapping.source),resetMapping.scancode}};
   const bool changed=!manualResetInputObservationValid||observation!=lastManualResetInputObservation;
   if(changed){
    std::cerr<<"original_manual_reset_input phase=before_compose keyboard_packet="<<(allAiKeyboardPackets+1u)
        <<" sampled_scancode=23 down="<<unsigned(input.down[23])
        <<" pressed="<<unsigned(input.pressed[23])<<" released="<<unsigned(input.released[23])
        <<" requested_live="<<live<<" prior_packet_live="<<packetLive
        <<" focused="<<input.focused<<" minimized="<<input.minimized
        <<" activity_lost="<<input.activityLost<<" event_budget_exhausted="<<input.eventBudgetExhausted
        <<" bound="<<humanControlInput.manualResetBound<<" descriptor="<<resetCode
        <<" mapping_valid="<<resetCodeValid<<" mapping_source="<<static_cast<unsigned>(resetMapping.source)
        <<" mapped_scancode="<<resetMapping.scancode<<'\n';
    lastManualResetInputObservation=observation;manualResetInputObservationValid=true;
   }
   ++allAiKeyboardPackets;
   require50DU(platform::composeKeyboardPacket50DR({true,live},
       {input.down.data(),input.down.size()},keyMapping,
       platform::KeyboardUnavailablePolicy50DR::ReleaseAll,packetTemplate,packet,
       keyboardDiagnostic),"selected keyboard packet composition failed");
   packetLive=live;
   if(changed)std::cerr<<"original_manual_reset_input phase=after_compose keyboard_packet="<<allAiKeyboardPackets
       <<" packet_live="<<packetLive<<" disposition="<<static_cast<unsigned>(keyboardDiagnostic.disposition)
       <<" packet_byte2="<<unsigned(packet.bytes[2])<<" packet_byte3="<<unsigned(packet.bytes[3])<<'\n';
  }
 };

 auto applyCourseInput=[&](std::size_t racerSlot=0){
  require50DU(!originalUiActive,"Main-only frontend cannot invoke a driving controller");
  require50DU(!nativeOriginalFleet,"native Course path reached forbidden HOST course driver");
  require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack,"MB/TC/RC/TB/GB/MC native session cannot enter the legacy HOST course driver");
  ++allAiHostDriverCalls;
  rr::host::CourseDriverInput50GK command{};
  command.enabled=autoDriveEnabled;command.elapsedSeconds=.016f;
  command.pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
  command.worldVelocity=m.components.root.body.velocity;
  command.yawRate=m.components.root.worldAngularVelocityA0.v[1];
  command.previousNode=navigationProgress.vehicleC84;
  command.currentNode=navigationProgress.reset.vehicleC80;
  const bool newNode=!courseState.initialized||courseState.observedNode!=command.currentNode;
  const bool driving=rr::host::driveCourse50GK(*navigationAI,courseLimits,courseTuning,
      command,courseState,courseOutput,courseDiagnostic);
  require50DU(rr::host::applyCourseThrottlePacket50GK(courseOutput,packet),"host course packet rejected");
  if(driving&&newNode&&courseTuning.authoredSteeringLookaheadSeconds>0.f)
   std::cerr<<"Host course node: slot="<<racerSlot<<" tick="<<completedFrames
       <<" previous="<<command.previousNode<<" node="<<command.currentNode
       <<" position="<<command.pose.v[12]<<','<<command.pose.v[13]<<','<<command.pose.v[14]
       <<" speed="<<courseDiagnostic.bodySpeed<<" target="<<courseDiagnostic.targetSpeed
       <<" cross_track="<<courseDiagnostic.crossTrack<<" half_width="<<courseDiagnostic.halfWidth
       <<" speed_preview="<<courseDiagnostic.speedLookahead
       <<" steering_preview="<<courseDiagnostic.steeringLookahead
       <<" steering_segments="<<courseDiagnostic.steeringSegments
       <<" aim="<<courseDiagnostic.aim[0]<<','<<courseDiagnostic.aim[1]<<','<<courseDiagnostic.aim[2]
       <<" error="<<courseDiagnostic.headingError<<" turn="<<courseDiagnostic.turn
       <<" demand="<<courseDiagnostic.speedDemand<<" mixed_demand="<<courseDiagnostic.mixedDemand
       <<" left/right="<<courseOutput.left<<'/'<<courseOutput.right
       <<" packet7/5="<<unsigned(packet.bytes[7])<<'/'<<unsigned(packet.bytes[5])<<'\n';
  if(!driving&&!autoStopReported){
   autoStopReported=true;
   std::cerr<<"Host course driver stopped: status="<<unsigned(courseDiagnostic.status)
       <<" node="<<courseDiagnostic.node<<" cross_track="<<courseDiagnostic.crossTrack
       <<" stalled_s="<<courseDiagnostic.stalledSeconds
       <<" slot="<<racerSlot<<" tick="<<completedFrames<<" previous="<<command.previousNode
       <<" position="<<command.pose.v[12]<<','<<command.pose.v[13]<<','<<command.pose.v[14]
       <<" speed="<<courseDiagnostic.bodySpeed<<" half_width="<<courseDiagnostic.halfWidth
       <<". Neutral input; manual takeover remains available.\n";
  }
 };

 // Human Course uses the actual1B0A70 predicate and1A7DA0 clock below.
 // Legacy direct-play retains its separately labeled HOST end presentation.
 bool humanResultsActive=false,humanResultsReady=false;
 // Actual race94 for the owned human Course route (1A6660), distinct from
 // the source manager30 race timer. Legacy diagnostics retain their timer.
 float humanResultsCountdown=humanCourseEnabled?4.f:0.f;
 std::uint64_t finishDriverTicks=0,finishManualInputTicks=0;
 bool finishDriverAppliedThisFrame=false;
 bool opponentFrame=false,raceSetupFrame=false;
 // This host fallback abandons the failed frame; it never supplies a miss
 // or repairs source list topology. Only the exact retained boundary qualifies.
 auto recoverPortalBoundary=[&](const LiveZoneResult50EN& failure,const char* owner){
  if(failure.status!=LiveZoneStatus50EN::UnsupportedLinkRemoval)return false;
  recoveryPaused=true;
  if(originalAiEnabled){
   std::cerr<<"ORIGINAL AI source-recovery boundary: kind=portal owner="<<owner
       <<" phase="<<static_cast<unsigned>(s.phase)
       <<" status="<<static_cast<unsigned>(failure.status)<<"; no checkpoint prepared or session rebuilt.\n";
   require50DU(false,"original AI single session reached native portal boundary");
  }
  if(opponentFrame)return true; // Keep the unsupported prefix in this AI slot only.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack,"MB/TC/RC/TB/GB/MC native session cannot enter legacy HOST checkpoint preparePortalRecovery");
  const bool prepared=recovery50FI::preparePortalRecovery(*navigationMap,navigationProgress,s,
      x.bindings,failure,x.parameters.visual.verticalLimit7EC,liveWorld,nextCheckpoint,recoveryDiagnostic);
  const bool repeated=prepared&&checkpointInput&&
      checkpointInput->policy==recovery50FI::Policy::PortalLinkRemoval&&
      checkpointInput->primaryZone==nextCheckpoint.primaryZone&&
      checkpointInput->selection.anchorNode==nextCheckpoint.selection.anchorNode&&
      checkpointInput->progress.reset.vehicleC80==nextCheckpoint.progress.reset.vehicleC80&&
      checkpointInput->progress.vehicleC84==nextCheckpoint.progress.vehicleC84&&
      checkpointInput->progress.reset.vehicleC88==nextCheckpoint.progress.reset.vehicleC88&&
      checkpointInput->progress.reset.vehicleC8C==nextCheckpoint.progress.reset.vehicleC8C;
  const bool grounded=prepared&&!repeated&&recovery50FI::resolveGround(ground,nextCheckpoint,16,0,
      groundStack.data(),groundStack.size(),groundedCheckpoint,recoveryDiagnostic);
  if(grounded){
   checkpointOutput=nextCheckpoint;checkpointRequested=true;
   std::cerr<<"Host portal-boundary checkpoint recovery: owner="<<owner
      <<" completed_ticks="<<completedFrames<<" anchor="<<nextCheckpoint.selection.anchorNode
      <<" zone="<<nextCheckpoint.primaryZone<<". Failed frame abandoned; damage/progress retained.\n";
  }else{
   std::cerr<<"Portal boundary paused: owner="<<owner<<" repeated_without_progress="<<repeated
      <<" recovery_error="<<unsigned(recoveryDiagnostic.error)
      <<". Last committed image retained; F5/restart returns to grid.\n";
  }
  return true;
 };
 auto speechInput=[&](){
  DamageSpeechInput50HG in{};
  in.damage7D0=s.fields.damage7D0;in.damage7D4=s.fields.damage7D4;in.damage7DC=s.fields.damage7DC;
  in.vehicle3C=s.fields.vehicle3C;in.vehicle40=m.vehicleState40;in.vehicle58=s.fields.vehicle58;
  in.vehicle60=playerRacer60;
  // Source21081C/210820 authenticates the resource identity, not native bank slot0.
  in.vehicle60Known=true;in.playerSpeechChance138=playerSpeechChance;
  in.config58=s.fields.applicationByte58;
  in.position={m.components.root.body.position.v[0],m.components.root.body.position.v[2]};
  in.listeners={audioListenerReady?&audioListener:nullptr,audioListenerReady?1u:0u};
  return in;
 };
 struct Mode1EffectsContext50ID {
  std::unique_ptr<race_mode1_host50ID::Owner>* mode=nullptr;
  source_announcer50IE::Owner* dax=nullptr;race_music50IE::Owner* music=nullptr;
  live_audio50HF::Owner* audio=nullptr;decltype(speechInput)* input=nullptr;
  std::uint64_t* speechCues=nullptr;
 } mode1Context{&mode1,sourceAnnouncer.get(),raceMusic.get(),liveAudio.get(),&speechInput,&speechCues};
 race_mode1_host50ID::Effects mode1Effects{};mode1Effects.context=&mode1Context;
 mode1Effects.announcer=[](void* opaque,std::uint32_t type,std::uint32_t racer,std::uint32_t variant)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);
  return *c.mode&&c.dax->request(type,racer,variant,
      (*c.mode)->config.mode11258,(*c.mode)->manager.flags58);
 };
 mode1Effects.music=[](void* opaque,std::uint32_t key,std::uint32_t argumentA2)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);
  return *c.mode&&c.audio->outputOpen()&&c.music->request(key,argumentA2,
      (*c.mode)->config.mode11258).completed();
 };
 mode1Effects.audio=[](void* opaque,const AudioCommand50HE& command)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);
  if(!c.audio->outputOpen()||!c.audio->applySourceCommand(command).okay())return false;
  ++*c.speechCues;return true;
 };
 mode1Effects.damageInput=[](void* opaque,DamageSpeechInput50HG& input)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);input=(*c.input)();return true;
 };
 auto refreshDynamicSelfActor=[&](){
  require50DU(dynamicSelfSlot<raceRacerCount,"dynamic actor slot unavailable");
  dynamicActors.values[dynamicSelfSlot]={s.fields.vehicle3C,s.fields.vehicle58,
      dynamicRacer60[dynamicSelfSlot],lapContinuity.state.vehicleCC8,
      static_cast<std::uint32_t>(originalAiSession.rank[dynamicSelfSlot].placeCCC),s.fields.impactFlagsCD8};
  dynamicActors.bound[dynamicSelfSlot]=true;
 };
 auto dynamicBoundary=[&](std::uint32_t pc,const char* owner){
  std::cerr<<"ORIGINAL AI dynamic-contact boundary: slot="<<dynamicSelfSlot
      <<" pc="<<std::hex<<pc<<std::dec<<" owner="<<owner
      <<" phase="<<static_cast<unsigned>(s.phase)<<" contacts20C="<<m.components.body20C
      <<" partner="<<s.impactSupplement.partnerId<<" partner_state="<<s.impactSupplement.partnerState
      <<" partner_amount="<<s.impactSupplement.partnerAmount0C
      <<" CFC="<<s.fields.vehicleCFC<<" D00="<<s.fields.vehicleD00
      <<" event_count="<<dynamicEvents.count150<<"; reached native state retained.\n";
  require50DU(false,owner);
 };
 auto fulfillDynamicMusic=[&](){
  using Status=DynamicContactEffectStatus50IW;
  if(beginDynamicContactMusic50IW(dynamicMusicCursor).status!=Status::Continue)
   dynamicBoundary(0x22a400u,"collision music continuation already pending");
  for(;;){
   const auto step=advanceDynamicContactMusic50IW(dynamicMusicCursor,sessionRandom.seed,sessionRandom.range);
   if(step.status==Status::Complete)return;
   if(step.status!=Status::NeedHelper)dynamicBoundary(step.sourcePC,"collision music source continuation");
   std::uint32_t actualV0=0;bool returned=false;
   if(step.request.kind==LapEventCall50ID::Music1A46E0){
    returned=dynamicMusicProfile.bound&&liveAudio->outputOpen()&&
        raceMusic->request(step.request.argumentA1,step.request.argumentA2,
            dynamicMusicProfile.mode11258).completed();
   }else returned=applyHostLapClockOrRandom50ID(step.request,applicationClock,
       s.noiseOwner().sharedRandomState,actualV0);
   if(!returned)dynamicBoundary(step.sourcePC,"collision music actual helper pending");
   if(!acceptDynamicContactMusicReturn50IW(dynamicMusicCursor,step.request.sequence,
       step.request.kind,actualV0))dynamicBoundary(step.sourcePC,"collision music return mismatch");
  }
 };
 // Continue synchronously: suffix-owned Bank/window/gameplay resources retire
 // before these prefix locals, including on exceptional unwind.
 interactive_continuation50DU::Context continuation50DU{
  .acceptLiveQueryCall=interactive_frame50DU::BorrowedCall<bool(const rr::game::VehicleLiveQueryResult50EQ&,rr::game::Type2StaticQuery49X&,const rr::game::LiveZoneMembership50EN&,std::uint64_t&,const char*)>{acceptLiveQuery},
  .actualHeight=actualHeight,
  .actualWorld=actualWorld,
  .allAiControllerSamples=allAiControllerSamples,
  .allAiEnabled=allAiEnabled,
  .allAiHostDriverCalls=allAiHostDriverCalls,
  .allAiKeyboardPackets=allAiKeyboardPackets,
  .announcerRequestsUnjoined=announcerRequestsUnjoined,
  .applicationChildren=applicationChildren,
  .applicationClock=applicationClock,
  .applicationEvents=applicationEvents,
  .applicationWrongWayBypass34C=applicationWrongWayBypass34C,
  .applyCourseInputCall=interactive_frame50DU::BorrowedCall<void(std::size_t)>{applyCourseInput},
  .audioContactMaterials=audioContactMaterials,
  .audioDiagnostic=audioDiagnostic,
  .audioEnabled=audioEnabled,
  .audioListener=audioListener,
  .audioListenerReady=audioListenerReady,
  .audioOutputRequested=audioOutputRequested,
  .authored=authored,
  .authoredFinishSteering10A4=authoredFinishSteering10A4,
  .authoredTrackName=authoredTrackName,
  .autoDriveEnabled=autoDriveEnabled,
  .autoStopReported=autoStopReported,
  .baseGlobals=baseGlobals,
  .baseTail=baseTail,
  .baseWorkspace=baseWorkspace,
  .beginNativeLightDrawCall=interactive_frame50DU::BorrowedCall<void(std::uint32_t,rr::game::original_gc_gameplay_lighting_candidate::DrawLease&)>{beginNativeLightDraw},
  .bindColdNativeEffectsCall=interactive_frame50DU::BorrowedCall<void()>{bindColdNativeEffects},
  .boostButtonCode=boostButtonCode,
  .boostCoolingCF8=boostCoolingCF8,
  .boostDiagnostic=boostDiagnostic,
  .boostParameters=boostParameters,
  .boostState=boostState,
  .cableBindings=cableBindings,
  .cableDiagnostic=cableDiagnostic,
  .cableMesh=cableMesh,
  .cableOwner=cableOwner,
  .cableWorkspace=cableWorkspace,
  .cameraStartup350=cameraStartup350,
  .candidates=candidates,
  .captureFirstWrongWay=captureFirstWrongWay,
  .capturePath=capturePath,
  .captureRun=captureRun,
  .capturedFirstWrongWay=capturedFirstWrongWay,
  .checkpointInput=checkpointInput,
  .checkpointOutput=checkpointOutput,
  .checkpointRequested=checkpointRequested,
  .clock=clock,
  .closeGbCurrentCollisionCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeGbCurrentCollision},
  .closeMcCurrentCollisionCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeMcCurrentCollision},
  .closeRcCurrentCollisionCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeRcCurrentCollision},
  .closeScCurrentMembershipsCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeScCurrentMemberships},
  .closeTbCurrentCollisionCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeTbCurrentCollision},
  .closeTcCurrentCollisionCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeTcCurrentCollision},
  .coldFlame=coldFlame,
  .coldLoadDiagnostics=coldLoadDiagnostics,
  .collectRegisteredCall=interactive_frame50DU::BorrowedCall<bool()>{collectRegistered},
  .collectedCalls=collectedCalls,
  .collisionNormalMode=collisionNormalMode,
  .commitCandidateEffectsCall=interactive_frame50DU::BorrowedCall<bool(const std::vector<rr::game::TerrainCandidateEffects48J>&)>{commitCandidateEffects},
  .completedFrames=completedFrames,
  .composePacketCall=interactive_frame50DU::BorrowedCall<void(bool,const rr::platform::WindowInput50DQ&,std::size_t)>{composePacket},
  .consumeEffectsCall=interactive_frame50DU::BorrowedCall<bool(const rr::game::ImpactEffectEvent50B*,std::size_t)>{consumeEffects},
  .consumedGaStage=consumedGaStage,
  .consumedGcStage=consumedGcStage,
  .consumedMaStage=consumedMaStage,
  .consumedMbStage=consumedMbStage,
  .consumedRaStage=consumedRaStage,
  .consumedSaStage=consumedSaStage,
  .consumedTaStage=consumedTaStage,
  .controlPresets=controlPresets,
  .controller=controller,
  .controllerActions=controllerActions,
  .controllerEventStorage=controllerEventStorage,
  .controllerEvents=controllerEvents,
  .controllerPresses=controllerPresses,
  .controllerReleases=controllerReleases,
  .courseColdLoad=courseColdLoad,
  .courseDiagnostic=courseDiagnostic,
  .courseOutput=courseOutput,
  .courseRaceCold=courseRaceCold,
  .courseRaceLaunched=courseRaceLaunched,
  .courseState=courseState,
  .courseTuning=courseTuning,
  .destination=destination,
  .directOriginalHuman=directOriginalHuman,
  .discardedHostNs=discardedHostNs,
  .displayWallTotals=displayWallTotals,
  .drawAssets=drawAssets,
  .drawWorkspace=drawWorkspace,
  .dynamicActorResolver=dynamicActorResolver,
  .dynamicActors=dynamicActors,
  .dynamicBoundaryCall=interactive_frame50DU::BorrowedCall<void(std::uint32_t,const char*)>{dynamicBoundary},
  .dynamicConsumerFlags11050=dynamicConsumerFlags11050,
  .dynamicContactEnabled=dynamicContactEnabled,
  .dynamicDamageCursor=dynamicDamageCursor,
  .dynamicEventBindings=dynamicEventBindings,
  .dynamicEventCursor=dynamicEventCursor,
  .dynamicEvents=dynamicEvents,
  .dynamicFinalTuning=dynamicFinalTuning,
  .dynamicLapCursor=dynamicLapCursor,
  .dynamicMusicProfile=dynamicMusicProfile,
  .dynamicPeers=dynamicPeers,
  .dynamicPodEffect=dynamicPodEffect,
  .dynamicProfile=dynamicProfile,
  .dynamicRacer60=dynamicRacer60,
  .dynamicRegistry=dynamicRegistry,
  .dynamicSelfSlot=dynamicSelfSlot,
  .dynamicTuning=dynamicTuning,
  .effectCommands=effectCommands,
  .effectOwnerIndex=effectOwnerIndex,
  .effects=effects,
  .effectsWorkspace=effectsWorkspace,
  .endpointScenes=endpointScenes,
  .endpoints=endpoints,
  .engineDamage=engineDamage,
  .engineDamageCompletions=engineDamageCompletions,
  .engineDamageDips=engineDamageDips,
  .engineDamageDraws=engineDamageDraws,
  .engineDamageReturns=engineDamageReturns,
  .engineDamageUpdates=engineDamageUpdates,
  .eventContext=eventContext,
  .executeOriginalManualReset=executeOriginalManualReset,
  .feedbackDevices=feedbackDevices,
  .feedbackHelpers=feedbackHelpers,
  .feedbackHost=feedbackHost,
  .feedbackLeaseRefreshes=feedbackLeaseRefreshes,
  .feedbackNoDevice=feedbackNoDevice,
  .feedbackPhysicalDevice=feedbackPhysicalDevice,
  .feedbackSubmissions=feedbackSubmissions,
  .finishDriverAppliedThisFrame=finishDriverAppliedThisFrame,
  .finishDriverTicks=finishDriverTicks,
  .finishManualInputTicks=finishManualInputTicks,
  .freshMainProbe=freshMainProbe,
  .freshProbe100=freshProbe100,
  .freshTerrainProbe=freshTerrainProbe,
  .fulfillDynamicMusicCall=interactive_frame50DU::BorrowedCall<void()>{fulfillDynamicMusic},
  .gaAnimatedMembership=gaAnimatedMembership,
  .gaAnimatedWorld=gaAnimatedWorld,
  .gaAnimationDiagnostic=gaAnimationDiagnostic,
  .gaGateAudio=gaGateAudio,
  .gaOriginalDisc=gaOriginalDisc,
  .gaOriginalWorld=gaOriginalWorld,
  .gaTriggerActions=gaTriggerActions,
  .gaTriggerAuthoring=gaTriggerAuthoring,
  .gaTriggerMembership=gaTriggerMembership,
  .gatherRegisteredCall=interactive_frame50DU::BorrowedCall<bool()>{gatherRegistered},
  .gbAiMetadata=gbAiMetadata,
  .gbAiView=gbAiView,
  .gbAnimationBinding=gbAnimationBinding,
  .gbAnimationBindingDiagnostic=gbAnimationBindingDiagnostic,
  .gbAnimationRuntime=gbAnimationRuntime,
  .gbColdSources=gbColdSources,
  .gbCollisionCleanupDone=gbCollisionCleanupDone,
  .gbCollisionWorld=gbCollisionWorld,
  .gbFogOwner=gbFogOwner,
  .gbLightOwner=gbLightOwner,
  .gbMapMetadata=gbMapMetadata,
  .gbMapView=gbMapView,
  .gbNavigation=gbNavigation,
  .gbNavigationOwner=gbNavigationOwner,
  .gbOriginalWorld=gbOriginalWorld,
  .gbParticleAttachments=gbParticleAttachments,
  .gbParticleAuthoring=gbParticleAuthoring,
  .gbParticleRender=gbParticleRender,
  .gbParticleRuntime=gbParticleRuntime,
  .gbParticleRuntimeDiagnostic=gbParticleRuntimeDiagnostic,
  .gbParticleWindow=gbParticleWindow,
  .gbPodSound=gbPodSound,
  .gbPositionalAudio=gbPositionalAudio,
  .gbQueryBinding=gbQueryBinding,
  .gbRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{gbRemainingPayloadBytes},
  .gbTrackParticles=gbTrackParticles,
  .gbTriggerActions=gbTriggerActions,
  .gbWorldBorrow=gbWorldBorrow,
  .gcAnimatedMembership=gcAnimatedMembership,
  .gcAnimatedWorld=gcAnimatedWorld,
  .gcAnimationDiagnostic=gcAnimationDiagnostic,
  .gcDrawLights=gcDrawLights,
  .gcGateAudio=gcGateAudio,
  .gcGraphicsSequence=gcGraphicsSequence,
  .gcLightDiagnostic=gcLightDiagnostic,
  .gcLightOwner=gcLightOwner,
  .gcNativeGate=gcNativeGate,
  .gcNativeWorldSelected=gcNativeWorldSelected,
  .gcOriginalDisc=gcOriginalDisc,
  .gcOriginalWorld=gcOriginalWorld,
  .gcTriggerActions=gcTriggerActions,
  .gcTriggerAuthoring=gcTriggerAuthoring,
  .gcTriggerMembership=gcTriggerMembership,
  .generatedEffects=generatedEffects,
  .ground=ground,
  .groundStack=groundStack,
  .headlessLive=headlessLive,
  .host=host,
  .hostDiagnostic=hostDiagnostic,
  .humanBaseUndertrack=humanBaseUndertrack,
  .humanCameraInputRoute=humanCameraInputRoute,
  .humanControlInput=humanControlInput,
  .humanCourseEnabled=humanCourseEnabled,
  .humanResultsActive=humanResultsActive,
  .humanResultsCountdown=humanResultsCountdown,
  .humanResultsReady=humanResultsReady,
  .initial=initial,
  .initialCollisionZone=initialCollisionZone,
  .initialMetadata=initialMetadata,
  .initialPairs=initialPairs,
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
  .keyboardDiagnostic=keyboardDiagnostic,
  .lapContinuity=lapContinuity,
  .lapOwner=lapOwner,
  .lastCommittedWrongWay=lastCommittedWrongWay,
  .legacyGround=legacyGround,
  .liveAudio=liveAudio,
  .liveWorld=liveWorld,
  .loadedAudioSettings=loadedAudioSettings,
  .loadedTrack=loadedTrack,
  .m=m,
  .maOriginalDisc=maOriginalDisc,
  .maOriginalWorld=maOriginalWorld,
  .maTriggerActions=maTriggerActions,
  .maTriggerAuthoring=maTriggerAuthoring,
  .maTriggerMembership=maTriggerMembership,
  .mainContactFrames=mainContactFrames,
  .mainCrossedPortals=mainCrossedPortals,
  .mainLiveQuery=mainLiveQuery,
  .mainMembership=mainMembership,
  .mainMembershipStorage=mainMembershipStorage,
  .mainQueryRef=mainQueryRef,
  .managerRuntimeSelection=managerRuntimeSelection,
  .materialAudio=materialAudio,
  .materialOwner=materialOwner,
  .mbAiMetadata=mbAiMetadata,
  .mbAiView=mbAiView,
  .mbCollisionWorld=mbCollisionWorld,
  .mbMapMetadata=mbMapMetadata,
  .mbMapView=mbMapView,
  .mbOriginalDisc=mbOriginalDisc,
  .mbOriginalWorld=mbOriginalWorld,
  .mbSourceTriggers=mbSourceTriggers,
  .mcAdditionalPayloadReserved=mcAdditionalPayloadReserved,
  .mcAiMetadata=mcAiMetadata,
  .mcAiView=mcAiView,
  .mcAnimationBinding=mcAnimationBinding,
  .mcAnimationBindingDiagnostic=mcAnimationBindingDiagnostic,
  .mcAnimationRuntime=mcAnimationRuntime,
  .mcColdSources=mcColdSources,
  .mcCollisionCleanupDone=mcCollisionCleanupDone,
  .mcCollisionWorld=mcCollisionWorld,
  .mcFogOwner=mcFogOwner,
  .mcLightOwner=mcLightOwner,
  .mcMapMetadata=mcMapMetadata,
  .mcMapView=mcMapView,
  .mcNavigation=mcNavigation,
  .mcNavigationOwner=mcNavigationOwner,
  .mcOriginalWorld=mcOriginalWorld,
  .mcParticleAttachments=mcParticleAttachments,
  .mcParticleRender=mcParticleRender,
  .mcParticleRuntime=mcParticleRuntime,
  .mcParticleRuntimeDiagnostic=mcParticleRuntimeDiagnostic,
  .mcParticleSource=mcParticleSource,
  .mcParticleVehicles=mcParticleVehicles,
  .mcParticleWindow=mcParticleWindow,
  .mcPositionalAudio=mcPositionalAudio,
  .mcQueryBinding=mcQueryBinding,
  .mcRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{mcRemainingPayloadBytes},
  .mcTrackParticles=mcTrackParticles,
  .mcTriggerActions=mcTriggerActions,
  .mcWaterOwner=mcWaterOwner,
  .mcWaterUpdateSequence=mcWaterUpdateSequence,
  .mcWorldBorrow=mcWorldBorrow,
  .membershipWork=membershipWork,
  .mode1=mode1,
  .mode1Effects=mode1Effects,
  .mode1Enabled=mode1Enabled,
  .monotonicNow50DU=monotonicNow50DU,
  .movement=movement,
  .movementCandidates=movementCandidates,
  .movementDestination=movementDestination,
  .movementEffects=movementEffects,
  .movementFlag=movementFlag,
  .movementInitial=movementInitial,
  .movementOutput=movementOutput,
  .movementResult=movementResult,
  .movementSource=movementSource,
  .movementWorkspace=movementWorkspace,
  .namesForEffects=namesForEffects,
  .nativeCalls=nativeCalls,
  .nativeEffectCount=nativeEffectCount,
  .nativeEffects=nativeEffects,
  .nativeFireSerial=nativeFireSerial,
  .nativeGbSelection=nativeGbSelection,
  .nativeGcWater=nativeGcWater,
  .nativeLightFailureCall=interactive_frame50DU::BorrowedCall<void(const char*,std::uint32_t,const rr::game::original_gc_gameplay_lighting_candidate::DrawLease&)>{nativeLightFailure},
  .nativeLightingPolicy=nativeLightingPolicy,
  .nativeMaSelection=nativeMaSelection,
  .nativeMbSelection=nativeMbSelection,
  .nativeMcSelection=nativeMcSelection,
  .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
  .nativeOriginalFleet=nativeOriginalFleet,
  .nativeParticleAuthoring=nativeParticleAuthoring,
  .nativeParticleChildSequence=nativeParticleChildSequence,
  .nativeParticleChildren=nativeParticleChildren,
  .nativeRaSelection=nativeRaSelection,
  .nativeRacerManager=nativeRacerManager,
  .nativeRcSelection=nativeRcSelection,
  .nativeRecordActor=nativeRecordActor,
  .nativeRosterOwners=nativeRosterOwners,
  .nativeSaSelection=nativeSaSelection,
  .nativeScSelection=nativeScSelection,
  .nativeSelectedLights=nativeSelectedLights,
  .nativeTaSelection=nativeTaSelection,
  .nativeTbSelection=nativeTbSelection,
  .nativeTcSelection=nativeTcSelection,
  .nativeVideo=nativeVideo,
  .nativeWaterApplication=nativeWaterApplication,
  .nativeWaterRacers=nativeWaterRacers,
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
  .openNativeVideoCall=interactive_frame50DU::BorrowedCall<void()>{openNativeVideo},
  .opponentAudio=opponentAudio,
  .opponentBoostAudio=opponentBoostAudio,
  .opponentFrame=opponentFrame,
  .originalAi=originalAi,
  .originalAiAudio=originalAiAudio,
  .originalAiAudioEnabled=originalAiAudioEnabled,
  .originalAiAuthoringDiagnostic=originalAiAuthoringDiagnostic,
  .originalAiCreateSelection=originalAiCreateSelection,
  .originalAiEnabled=originalAiEnabled,
  .originalAiHumanCount=originalAiHumanCount,
  .originalAiOpponentCount=originalAiOpponentCount,
  .originalAiOwnsSlotCall=interactive_frame50DU::BorrowedCall<bool(std::size_t) noexcept>{originalAiOwnsSlot},
  .originalAiReceipt=originalAiReceipt,
  .originalAiSession=originalAiSession,
  .originalAiSettings1125C=originalAiSettings1125C,
  .originalAiTuning=originalAiTuning,
  .originalCameraEnabled=originalCameraEnabled,
  .originalHumanRecovery=originalHumanRecovery,
  .originalHumanRecoveryDiagnostic=originalHumanRecoveryDiagnostic,
  .originalMainApplication=originalMainApplication,
  .originalManualRecoveryDiagnostic=originalManualRecoveryDiagnostic,
  .originalManualResetGraphicsDue=originalManualResetGraphicsDue,
  .originalManualResetSerial=originalManualResetSerial,
  .originalRecovery=originalRecovery,
  .originalRecoveryDiagnostic=originalRecoveryDiagnostic,
  .originalRecoveryEnabled=originalRecoveryEnabled,
  .originalRecoveryModels=originalRecoveryModels,
  .originalRecoveryWorkspace=originalRecoveryWorkspace,
  .originalResetGround=originalResetGround,
  .originalUiActive=originalUiActive,
  .originalUiStarted=originalUiStarted,
  .packet=packet,
  .packetLive=packetLive,
  .peakCollected=peakCollected,
  .pendingSpeech=pendingSpeech,
  .phaseTiming=phaseTiming,
  .physicalControllerReceipt=physicalControllerReceipt,
  .playerActionIndexFFC=playerActionIndexFFC,
  .playerArchive=playerArchive,
  .playerEnvironment=playerEnvironment,
  .playerEventChild=playerEventChild,
  .playerEvents=playerEvents,
  .playerPodId=playerPodId,
  .playerRacer60=playerRacer60,
  .playerSource5C=playerSource5C,
  .playerUsesRosterRig=playerUsesRosterRig,
  .poseHistory=poseHistory,
  .powerslideButtonCode=powerslideButtonCode,
  .prepareQueryCacheCall=interactive_frame50DU::BorrowedCall<bool(rr::game::Type2StaticQuery49X&,const rr::game::BodyMatrix48G&,rr::game::LiveZoneBounds50EN&)>{prepareQueryCache},
  .probeCount=probeCount,
  .probeIndices=probeIndices,
  .publishRecoveryQueriesCall=interactive_frame50DU::BorrowedCall<bool()>{publishRecoveryQueries},
  .qfilter=qfilter,
  .query=query,
  .queryRecords=queryRecords,
  .queryResult=queryResult,
  .raDiagnostic=raDiagnostic,
  .raFreshProbe=raFreshProbe,
  .raRuntime=raRuntime,
  .raWorld=raWorld,
  .raceAudio=raceAudio,
  .raceEnabled=raceEnabled,
  .raceMusic=raceMusic,
  .raceOpponentCount=raceOpponentCount,
  .raceRacerCount=raceRacerCount,
  .raceSetupFrame=raceSetupFrame,
  .racerCables=racerCables,
  .racerCollisions=racerCollisions,
  .racerContactsEnabled=racerContactsEnabled,
  .racerHudNames=racerHudNames,
  .racerNames=racerNames,
  .racerRigs=racerRigs,
  .racerVisuals=racerVisuals,
  .rcColdSources=rcColdSources,
  .rcCollisionCleanupDone=rcCollisionCleanupDone,
  .rcCollisionWorld=rcCollisionWorld,
  .rcLightOwner=rcLightOwner,
  .rcNavigationBorrow=rcNavigationBorrow,
  .rcOriginalDisc=rcOriginalDisc,
  .rcPositionalAudio=rcPositionalAudio,
  .rcRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{rcRemainingPayloadBytes},
  .rcTriggerActions=rcTriggerActions,
  .rcTriggerAuthoring=rcTriggerAuthoring,
  .rcWorldBorrow=rcWorldBorrow,
  .rebaseBounds=rebaseBounds,
  .rebaseDiagnostic=rebaseDiagnostic,
  .rebaseModelCount=rebaseModelCount,
  .rebaseModels=rebaseModels,
  .rebaseNodes=rebaseNodes,
  .rebaseWorkspace=rebaseWorkspace,
  .records=records,
  .recoverPortalBoundaryCall=interactive_frame50DU::BorrowedCall<bool(const rr::game::LiveZoneResult50EN&,const char*)>{recoverPortalBoundary},
  .recoveryBorrowedQueriesCall=interactive_frame50DU::BorrowedCall<undertrack_owners50ew_fixture::BorrowedQueries()>{recoveryBorrowedQueries},
  .recoveryPaused=recoveryPaused,
  .recoveryPodLive=recoveryPodLive,
  .recoveryQueriesCall=interactive_frame50DU::BorrowedCall<rr::game::UndertrackQueryOwners50EW()>{recoveryQueries},
  .recoveryQueryOwners=recoveryQueryOwners,
  .refreshAnimatedCandidatesCall=interactive_frame50DU::BorrowedCall<bool()>{refreshAnimatedCandidates},
  .refreshDynamicSelfActorCall=interactive_frame50DU::BorrowedCall<void()>{refreshDynamicSelfActor},
  .registeredCount=registeredCount,
  .registeredIndices=registeredIndices,
  .registeredMovement=registeredMovement,
  .renderDiagnostics=renderDiagnostics,
  .repairActiveTicks=repairActiveTicks,
  .repairButtonCode=repairButtonCode,
  .reportHost50DU=reportHost50DU,
  .reportNavigationCall=interactive_frame50DU::BorrowedCall<bool(bool,const char*)>{reportNavigation},
  .reportRenderer50DU=reportRenderer50DU,
  .requestIntroMusicFade=requestIntroMusicFade,
  .requestStartMusic=requestStartMusic,
  .requestedWindowSamples=requestedWindowSamples,
  .requireMbRuntimeNavigation50MBCall=interactive_frame50DU::BorrowedCall<void()>{requireMbRuntimeNavigation50MB},
  .reserveGbAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveGbAdditionalPayload},
  .reserveMcAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveMcAdditionalPayload},
  .reserveRcAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveRcAdditionalPayload},
  .reserveTbAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveTbAdditionalPayload},
  .resetEffectBindings=resetEffectBindings,
  .resetEffectState=resetEffectState,
  .resetEffectWorkspace=resetEffectWorkspace,
  .resetGlobals=resetGlobals,
  .resetHumanCamera=resetHumanCamera,
  .resetLifecycle=resetLifecycle,
  .response=response,
  .retireGbAudioOwnersCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{retireGbAudioOwners},
  .retireMcAudioOwnersCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{retireMcAudioOwners},
  .retireRcAudioOwnersCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{retireRcAudioOwners},
  .retireTbAudioOwnersCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{retireTbAudioOwners},
  .s=s,
  .saAnimatedWorld=saAnimatedWorld,
  .saAnimationDiagnostic=saAnimationDiagnostic,
  .saOriginalDisc=saOriginalDisc,
  .saOriginalWorld=saOriginalWorld,
  .saTriggerActions=saTriggerActions,
  .saTriggerAuthoring=saTriggerAuthoring,
  .saTriggerMembership=saTriggerMembership,
  .sampleControllerCall=interactive_frame50DU::BorrowedCall<void(bool)>{sampleController},
  .scColdSources=scColdSources,
  .scGround=scGround,
  .scMembershipCleanupDone=scMembershipCleanupDone,
  .scOriginalDisc=scOriginalDisc,
  .scTriggerActions=scTriggerActions,
  .scTriggerAuthoring=scTriggerAuthoring,
  .scTriggerMembership=scTriggerMembership,
  .sd=sd,
  .sde=sde,
  .secondaryContactFrames=secondaryContactFrames,
  .secondaryCrossedPortals=secondaryCrossedPortals,
  .secondaryFilter=secondaryFilter,
  .secondaryLiveQuery=secondaryLiveQuery,
  .secondaryMembership=secondaryMembership,
  .secondaryMembershipStorage=secondaryMembershipStorage,
  .secondaryQuery=secondaryQuery,
  .secondaryQueryRecords=secondaryQueryRecords,
  .secondaryQueryRef=secondaryQueryRef,
  .secondaryRecords=secondaryRecords,
  .secondaryResponse=secondaryResponse,
  .secondaryResult=secondaryResult,
  .secondarySettings=secondarySettings,
  .secondarySpring=secondarySpring,
  .selectedAiCount50MBCall=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedAiCount50MB},
  .selectedAudioSettings=selectedAudioSettings,
  .selectedControls=selectedControls,
  .selectedMapCount50MBCall=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount50MB},
  .selectedMapIndex50MBCall=interactive_frame50DU::BorrowedCall<std::uint8_t(std::uint32_t)>{selectedMapIndex50MB},
  .selectedMapNode50MBCall=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode50MB},
  .selectedOriginal=selectedOriginal,
  .selectedOriginalAiMapMetadata=selectedOriginalAiMapMetadata,
  .selectedOriginalAiMetadata=selectedOriginalAiMetadata,
  .selectedRacer60=selectedRacer60,
  .selectedRuntimeHeightDomain=selectedRuntimeHeightDomain,
  .selectedTrack=selectedTrack,
  .selection=selection,
  .sessionIndex=sessionIndex,
  .sessionRandom=sessionRandom,
  .settings=settings,
  .si=si,
  .sim=sim,
  .sip=sip,
  .sourceAnnouncer=sourceAnnouncer,
  .sourceBlur=sourceBlur,
  .sourceCameraCycleAudio=sourceCameraCycleAudio,
  .sourceConsumedIntroActivation=sourceConsumedIntroActivation,
  .sourceConsumedPauseActivation=sourceConsumedPauseActivation,
  .sourceDisplayLifetime=sourceDisplayLifetime,
  .sourceFollowQueries=sourceFollowQueries,
  .sourceModelRenderGlobals=sourceModelRenderGlobals,
  .sourcePause=sourcePause,
  .sourcePauseObservation=sourcePauseObservation,
  .sourcePauseParentAttached=sourcePauseParentAttached,
  .sourcePauseTrack=sourcePauseTrack,
  .sourcePositionStatsPrestage=sourcePositionStatsPrestage,
  .sourcePositionStatsPrestageDiagnostic=sourcePositionStatsPrestageDiagnostic,
  .sourceRaceHud=sourceRaceHud,
  .sourceRaceHudDisplayReported=sourceRaceHudDisplayReported,
  .sourceRaceHudRejectionReported=sourceRaceHudRejectionReported,
  .sourceRaceHudRendering=sourceRaceHudRendering,
  .speechCounts=speechCounts,
  .speechCues=speechCues,
  .speechEvents=speechEvents,
  .speechInputCall=interactive_frame50DU::BorrowedCall<rr::game::DamageSpeechInput50HG()>{speechInput},
  .speechState=speechState,
  .spring=spring,
  .startAtMenu=startAtMenu,
  .startMusicContext=startMusicContext,
  .stopFeedbackTransportCall=interactive_frame50DU::BorrowedCall<void()>{stopFeedbackTransport},
  .submittedDrawFrames=submittedDrawFrames,
  .sw=sw,
  .taOriginalDisc=taOriginalDisc,
  .taOriginalWorld=taOriginalWorld,
  .tbAiMetadata=tbAiMetadata,
  .tbAiView=tbAiView,
  .tbColdSources=tbColdSources,
  .tbCollisionCleanupDone=tbCollisionCleanupDone,
  .tbCollisionWorld=tbCollisionWorld,
  .tbDoorBinding=tbDoorBinding,
  .tbDoorBindingDiagnostic=tbDoorBindingDiagnostic,
  .tbDoorDiagnostic=tbDoorDiagnostic,
  .tbDoorPrefixArmed=tbDoorPrefixGuard.armed,
  .tbDoorRuntime=tbDoorRuntime,
  .tbLightOwner=tbLightOwner,
  .tbMapMetadata=tbMapMetadata,
  .tbMapView=tbMapView,
  .tbOriginalDisc=tbOriginalDisc,
  .tbPodSound=tbPodSound,
  .tbPositionalAudio=tbPositionalAudio,
  .tbRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{tbRemainingPayloadBytes},
  .tbTriggerActions=tbTriggerActions,
  .tbWorldBorrow=tbWorldBorrow,
  .tcAiMetadata=tcAiMetadata,
  .tcAiView=tcAiView,
  .tcColdSources=tcColdSources,
  .tcCollisionCleanupDone=tcCollisionCleanupDone,
  .tcCollisionWorld=tcCollisionWorld,
  .tcGateDiagnostic=tcGateDiagnostic,
  .tcGatePrefixArmed=tcGatePrefixGuard.armed,
  .tcGateRuntime=tcGateRuntime,
  .tcGateVisibility=tcGateVisibility,
  .tcGateVisibilityDiagnostic=tcGateVisibilityDiagnostic,
  .tcGlobalSound=tcGlobalSound,
  .tcLightOwner=tcLightOwner,
  .tcMapMetadata=tcMapMetadata,
  .tcMapView=tcMapView,
  .tcOriginalDisc=tcOriginalDisc,
  .tcTriggerActions=tcTriggerActions,
  .tcWorldBorrow=tcWorldBorrow,
  .terrainBindings=terrainBindings,
  .terrainContext=terrainContext,
  .terrainHitFrames=terrainHitFrames,
  .terrainLiveOwner=terrainLiveOwner,
  .terrainMembership=terrainMembership,
  .terrainMembershipStorage=terrainMembershipStorage,
  .terrainOwner=terrainOwner,
  .terrainOwnerCount=terrainOwnerCount,
  .terrainQueries=terrainQueries,
  .terrainQueryRef=terrainQueryRef,
  .terrainSupportedFrames=terrainSupportedFrames,
  .tournamentLaunchSelection=tournamentLaunchSelection,
  .tournamentLaunchStamp=tournamentLaunchStamp,
  .tournamentResultArmed=tournamentResultArmed,
  .tournamentResultCommitted=tournamentResultCommitted,
  .trackAdmission=trackAdmission,
  .trackCounts=trackCounts,
  .trackSession=trackSession,
  .trackStorage=trackStorage,
  .unavailableEffect=unavailableEffect,
  .usePhysicalGameController=usePhysicalGameController,
  .windowSwaps=windowSwaps,
  .workspace=workspace,
  .wrongWayActiveTicks=wrongWayActiveTicks,
  .wrongWayContinuation=wrongWayContinuation,
  .wrongWayDiagnostic=wrongWayDiagnostic,
  .wrongWayReceipt=wrongWayReceipt,
  .wrongWayUpdates=wrongWayUpdates,
  .x=x,
  .zoneCount=zoneCount,
  .zoneLifecycle=zoneLifecycle,
  .zoneQueue=zoneQueue,
  .zoneScratch=zoneScratch,
 };
 return continuation50DU.run();
}
}

// Exact original suffix: world owners retire before the original early prefix.
#include "native_last_failure50pd.h"
#include "native_wreck_debris_sim50pd.h"
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "terrain_interactive_world50du.h"
#include "terrain_interactive_roster_cold50du.h"
#include "ra_application_cold50la.h"
#include "gb_application_cold50gb.h"
#include "mc_application_cold50mc.h"
#include "terrain_interactive_setup50du.h"
#include "terrain_interactive_world_dependencies50du.h"
#include "terrain_interactive_loop_types50du.h"
#include "runtime_allocation_counter50fl.h"
#include "type2_fixture_io50i_declarations.h"
#include "native_ai_session50iv_fixture.h"
#include "../../src/game/original_human_finish50kg.h"
#include "player_loading_session50du.h"
#include "original_racer_manager_entity50la.h"
#include "original_subtype3_record_owner50la.h"
#include "original_record_grid_pose50la.h"
namespace cold=cold50i;
namespace interactive_world50DU {
namespace f=runtime_startup50fl;
namespace recovery50FI=checkpoint_recovery50fi_fixture;
using namespace rr::game;
using interactive_frame50DU::BorrowedCall;
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
 static_assert(raceRacerCapacity<=rr::platform::maxVehicleWindowStandings50DS);
 static_assert(maxHostRaceOpponents50HT==rr::platform::maxVehicleWindowOpponents50DS);
 static_assert(maxHostRaceRacers50HT<=native_racer_contact50HW::maxRacers);
 static_assert(live_audio50HF::lapFirstVoice+live_audio50HF::lapVoiceCount<=64);
 static_assert(gameplay_audio50HE::extraFirstVoice+gameplay_audio50HE::cueCount-
     gameplay_audio50HE::legacyCueCount==original_camera_cycle_audio50KX::Owner::voice);
 static_assert(original_camera_cycle_audio50KX::Owner::voice<live_audio50HF::voiceCount);
 static_assert(64+material_audio50HI::soundCount<=race_audio50HQ::firstVoice);
 static_assert(race_audio50HQ::firstVoice+race_audio50HQ::voiceCount<=opponent_audio50HX::firstVoice);
 static_assert(opponent_audio50HX::firstVoice+opponent_audio50HX::voiceCount<=opponent_audio50HX::boostFirstVoice);
 static_assert(opponent_audio50HX::boostFirstVoice+opponent_audio50HX::boostVoiceCount<=race_music50IE::voice);
 static_assert(race_music50IE::voice<material_audio50HI::extraFirstVoice);
 static_assert(material_audio50HI::extraFirstVoice+material_audio50HI::maximumExtraVoices<=live_audio50HF::voiceCount);
 static_assert(material_audio50HI::gaExtraFirstVoice+material_audio50HI::gaMaximumExtraVoices<=live_audio50HF::voiceCount);
 static_assert(material_audio50HI::gaExtraFirstVoice+material_audio50HI::gaMaximumExtraVoices<=gameplay_audio50HE::extraFirstVoice);
 static_assert(160u+32u<=material_audio50HI::maSaExtraFirstVoice);
 static_assert(material_audio50HI::maSaExtraFirstVoice+material_audio50HI::saMaximumExtraVoices<=live_audio50HF::voiceCount);
 static_assert(tb_pod_sound50TB::voice==126&&tb_positional_sound50TB::firstVoice==128);
 static_assert(tb_positional_sound50TB::firstVoice+tb_positional_sound50TB::voiceCount==152);
 static_assert(material_audio50HI::tbExtraFirstVoice==192&&material_audio50HI::tbMaximumExtraVoices==28);
 static_assert(material_audio50HI::tbExtraFirstVoice+material_audio50HI::tbMaximumExtraVoices==
     rr::platform::originalMaterialFirstVoice50HD);
 static_assert(rr::platform::originalMaterialFirstVoice50HD+8u*44u==rr::platform::originalSpeechFirstVoice50HD);
 static_assert(rr::platform::originalSpeechFirstVoice50HD+8u*47u==rr::platform::originalTaIntroFirstVoice50HD);
 static_assert(rr::platform::originalTaIntroFirstVoice50HD+rr::platform::originalTaIntroVoices50HD==live_audio50HF::voiceCount);
 auto& allAiControllerSamples=this->allAiControllerSamples;
 auto& allAiEnabled=this->allAiEnabled;
 auto& allAiHostDriverCalls=this->allAiHostDriverCalls;
 auto& allAiKeyboardPackets=this->allAiKeyboardPackets;
 auto& applicationClock=this->applicationClock;
 auto& cameraStartup350=this->cameraStartup350;
 auto& captureFirstWrongWay=this->captureFirstWrongWay;
 auto& capturePath=this->capturePath;
 auto& captureRun=this->captureRun;
 auto& capturedFirstWrongWay=this->capturedFirstWrongWay;
 auto& checkpointInput=this->checkpointInput;
 auto& checkpointOutput=this->checkpointOutput;
 auto& chosen=this->chosen;
 auto& coldLoadDiagnostics=this->coldLoadDiagnostics;
 auto& collisionNormalMode=this->collisionNormalMode;
 auto& consumedLaunch=this->consumedLaunch;
 auto& controlPresets=this->controlPresets;
 auto& corpus=this->corpus;
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
 auto& endpointPolicy=this->endpointPolicy;
 auto& gaOriginalDisc=this->gaOriginalDisc;
 auto& gbLiveAudioPrepared=this->gbLiveAudioPrepared;
 auto& gbOriginalDisc=this->gbOriginalDisc;
 auto& gbPodSound=this->gbPodSound;
 auto& gbPreparedAudioRoot=this->gbPreparedAudioRoot;
 auto& gbSoundMode=this->gbSoundMode;
 auto& gcNativeGate=this->gcNativeGate;
 auto& gcNativeWorldSelected=this->gcNativeWorldSelected;
 auto& gcOriginalDisc=this->gcOriginalDisc;
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
 auto& legacyCandidates=this->legacyCandidates;
 auto& liveAudio=this->liveAudio;
 auto& loadedAudioSettings=this->loadedAudioSettings;
 auto& loadedTrack=this->loadedTrack;
 auto& m=this->m;
 auto& maOriginalDisc=this->maOriginalDisc;
 auto& mainQueryResource=this->mainQueryResource;
 auto& managerRuntimeSelection=this->managerRuntimeSelection;
 auto& mbOriginalDisc=this->mbOriginalDisc;
 auto& mcOriginalDisc=this->mcOriginalDisc;
 auto& meshes=this->meshes;
 auto& mode1Enabled=this->mode1Enabled;
 auto& mode1Requested=this->mode1Requested;
 auto& monotonicNow50DU=*this->monotonicNow50DU;
 auto& names=this->names;
 auto& nativeGaSelection=this->nativeGaSelection;
 auto& nativeGbSelection=this->nativeGbSelection;
 auto& nativeMaSelection=this->nativeMaSelection;
 auto& nativeMbSelection=this->nativeMbSelection;
 auto& nativeMcSelection=this->nativeMcSelection;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
 auto& nativeRaSelection=this->nativeRaSelection;
 auto& nativeRacerManager=this->nativeRacerManager;
 auto& nativeRcSelection=this->nativeRcSelection;
 auto& nativeRecordActor=this->nativeRecordActor;
 auto& nativeSaSelection=this->nativeSaSelection;
 auto& nativeScSelection=this->nativeScSelection;
 auto& nativeSelectedLights=this->nativeSelectedLights;
 auto& nativeTaSelection=this->nativeTaSelection;
 auto& nativeTbSelection=this->nativeTbSelection;
 auto& nativeTcSelection=this->nativeTcSelection;
 auto& nativeVideo=this->nativeVideo;
 auto& nativeWaterApplication=this->nativeWaterApplication;
 auto& nativeWaterEnabled=this->nativeWaterEnabled;
 auto& nativeWaterGlobals=this->nativeWaterGlobals;
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
 auto& originalBootstrapCollision=this->originalBootstrapCollision;
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
 const auto& prepareGbPodSound=this->prepareGbPodSoundCall;
 const auto& prepareTbPodSound=this->prepareTbPodSoundCall;
 auto& qfilter=this->qfilter;
 auto& query=this->query;
 auto& raceEnabled=this->raceEnabled;
 auto& raceOpponentCount=this->raceOpponentCount;
 auto& raceRacerCount=this->raceRacerCount;
 auto& racerContactsEnabled=this->racerContactsEnabled;
 auto& rcOriginalDisc=this->rcOriginalDisc;
 auto& renderDiagnostics=this->renderDiagnostics;
 auto& reportHost50DU=*this->reportHost50DU;
 auto& reportRenderer50DU=*this->reportRenderer50DU;
 auto& requestedWindowSamples=this->requestedWindowSamples;
 auto& s=this->s;
 auto& saOriginalDisc=this->saOriginalDisc;
 auto& scOriginalDisc=this->scOriginalDisc;
 auto& selectedAudioSettings=this->selectedAudioSettings;
 auto& selectedOriginal=this->selectedOriginal;
 auto& selectedRacer60=this->selectedRacer60;
 auto& selectedTrack=this->selectedTrack;
 auto& sessionIndex=this->sessionIndex;
 auto& sessionRandom=this->sessionRandom;
 auto& sourceConsumedIntroActivation=this->sourceConsumedIntroActivation;
 auto& sourceConsumedPauseActivation=this->sourceConsumedPauseActivation;
 auto& sourceDisplayLifetime=this->sourceDisplayLifetime;
 auto& sourceModelRenderGlobals=this->sourceModelRenderGlobals;
 auto& sourceStaticCandidates=this->sourceStaticCandidates;
 auto& stackCount=this->stackCount;
 auto& startAtMenu=this->startAtMenu;
 auto& taOriginalDisc=this->taOriginalDisc;
 auto& tbLiveAudioPrepared=this->tbLiveAudioPrepared;
 auto& tbOriginalDisc=this->tbOriginalDisc;
 auto& tbPodSound=this->tbPodSound;
 auto& tbPreparedAudioRoot=this->tbPreparedAudioRoot;
 auto& tbSoundMode=this->tbSoundMode;
 auto& tcOriginalDisc=this->tcOriginalDisc;
 auto& tournamentLaunchSelection=this->tournamentLaunchSelection;
 auto& tournamentLaunchStamp=this->tournamentLaunchStamp;
 auto& tournamentResultArmed=this->tournamentResultArmed;
 auto& tournamentResultCommitted=this->tournamentResultCommitted;
 auto& trackAdmission=this->trackAdmission;
 auto& trackSession=this->trackSession;
 auto& usePhysicalGameController=this->usePhysicalGameController;
 auto& x=this->x;
 original_consumed_ra_stage50DU::Owner consumedRaStage(originalMainApplication,*liveAudio);
 original_consumed_ta_stage50DU::Owner consumedTaStage(originalMainApplication,*liveAudio);
 original_consumed_gc_stage50DU::Owner consumedGcStage(originalMainApplication,*liveAudio);
 original_consumed_ga_stage50DU::Owner consumedGaStage(originalMainApplication,*liveAudio);
 original_consumed_sa_stage50DU::Owner consumedSaStage(originalMainApplication,*liveAudio);
 original_consumed_ma_stage50DU::Owner consumedMaStage(originalMainApplication,*liveAudio);
 std::shared_ptr<ta_original_world50LE::Owner> taOriginalWorld;
 std::shared_ptr<gc_original_world50LM::Owner> gcOriginalWorld;
 std::shared_ptr<ga_original_world50LH::Owner> gaOriginalWorld;
 std::shared_ptr<sa_original_world50LM::Owner> saOriginalWorld;
 std::shared_ptr<ma_original_world50LM::Owner> maOriginalWorld;
 original_pause_cold50LA::Owner sourcePause;
 original_pause_observation50LA::Owner sourcePauseObservation;
 std::shared_ptr<const ra_cold_sources50LA::DecodedSources> sourcePauseTrack;
 bool sourcePauseParentAttached=false;
 // Permanent original-resource owners precede the snapshot bank and all draw
 // publications. Only their immutable borrowed views enter the live arena.
 original_position_stats_prestage50LA::Owner sourcePositionStatsPrestage;
 original_position_stats_prestage50LA::Diagnostic sourcePositionStatsPrestageDiagnostic;
 auto raRosterOwners=interactive_roster_cold50DU::makeSharedOwners();
 auto& racerRigs=raRosterOwners->rigs;
 auto& racerCollisions=raRosterOwners->collisions;
 auto nativeRosterOwners=interactive_roster_cold50DU::makeRuntimeOwners();
 auto& racerVisuals=nativeRosterOwners->visuals;
 auto& racerCables=nativeRosterOwners->cables;
 std::array<std::string,raceRacerCapacity> racerNames;
 std::array<std::array<char,rr::platform::maxVehicleWindowRacerName50DS+1>,raceRacerCapacity> racerHudNames{};
 std::size_t mainPairCapacity=mainQueryResource.rayCount,secondaryPairCapacity=0;
 const auto matchesEndpointPolicy=[&](const Type2ResourceView49X& resource){
  for(std::size_t index=0;index<resource.rayCount;++index){
   Type2OpaqueEndpointWords49X words{};
   if(!endpointPolicy.words(nullptr,native_racer_collision50HW::QueryBody::Main,index,words)||
      std::bit_cast<std::uint32_t>(resource.rays[index].first.v[3])!=words.first||
      std::bit_cast<std::uint32_t>(resource.rays[index].second.v[3])!=words.second)return false;
  }
  return true;
 };
 if(raceEnabled)interactive_roster_cold50DU::Context{
  .mainQueryResource=mainQueryResource,
  .directOriginalInputs=directOriginalInputs,
  .playerRacer60=playerRacer60,
  .courseRaceCold=courseRaceCold,
  .sourceUiHumanCourse=originalUiStarted&&!originalUiActive&&humanCourseEnabled,
  .courseProfile=originalMainApplication.riderProfile.get(),
  .playerPodId=playerPodId,
  .raceRacerCount=raceRacerCount,
  .playerUsesRosterRig=playerUsesRosterRig,
  .racerRigs=racerRigs,
  .racerCollisions=racerCollisions,
  .endpointPolicy=endpointPolicy,
  .racerNames=racerNames,
  .mainPairCapacity=mainPairCapacity,
  .secondaryPairCapacity=secondaryPairCapacity,
  .matchesEndpointPolicy=interactive_frame50DU::BorrowedCall<bool(const rr::game::Type2ResourceView49X&)>(matchesEndpointPolicy),
 }.run();
 // Cold diagnostic setup only: replace retained generic0x84 fixture flags with
 // authenticated fresh graphics-model/source-node flags (49O creation proof).
 // This is before reset setters; never used to coerce a live cache into admission.
 const SceneNodeIndex49P rebaseRoots[3]={x.bindings.lifecycle.model1B4,
     x.bindings.visual.right1B8,x.bindings.visual.left1BC};
 const unsigned rebaseAsset[3]={2,1,0};
 SceneNodeIndex49P rebaseNodes[3][18]{};
 PodSceneRebaseModel50FB rebaseModels[5]{};std::uint32_t rebaseModelCount=3;
 for(unsigned component=0;component<3;++component){
  const auto root=static_cast<int>(rebaseRoots[component]);
  const auto& hierarchy=x.assets.hierarchy[rebaseAsset[component]];
  require50DU(hierarchy.nodeCount<=18,"source model rebase capacity");
  s.nodes[root].sceneFlags190=0x80;
  for(unsigned i=0;i<hierarchy.nodeCount;++i){
   rebaseNodes[component][i]=static_cast<SceneNodeIndex49P>(root+1+i);
   s.nodes[root+1+i].sceneFlags190=0xa4;
  }
  rebaseModels[component]={rebaseRoots[component],hierarchy,rebaseNodes[component],hierarchy.nodeCount};
 }


 FreshPodGeometrySources49O sources{};sources.chariot=x.assets.hierarchy[2];sources.rightEngine=x.assets.hierarchy[1];sources.forward=x.assets.motions[4];sources.back=x.assets.motions[5];rr::assets::MotionBindingDiagnostic49O bindingDiagnostic{};
 require50DU(rr::assets::readMotionNodeBindings49O(x.assets.hierarchy[2],reinterpret_cast<const char*>(x.assets.vut[2].data()),x.assets.vut[2].size(),sources.chariotBindings,bindingDiagnostic),"actual motion bindings");
 FreshPodGeometryWorkspace49O geometryWorkspace{};FreshPodGeometryResult49O geometry{};FreshPodGeometryDiagnostic49O geometryDiagnostic{};
 require50DU(buildFreshPodGeometry49O(sources,geometryWorkspace,geometry,geometryDiagnostic),"actual pod geometry");
 const auto phy=directOriginalInputs?x.assets.phy:
     runtime_startup50fl::readBytes("work/impactAuthoring49Y/Pod01.phy");
 // Original214954..214AA0 selects initial PHY class from the loaded track's
 // lowercase two-letter identity. Use the existing source selector, including
 // its sb/ra/sc exceptions; never carry the old TA-only class into a GC session.
 const auto sourceTrackLetter=[](char c){return c>='A'&&c<='Z'?char(c+'a'-'A'):c;};
 const char authoredTrackName[]={sourceTrackLetter(trackSession->world->code[0]),
     sourceTrackLetter(trackSession->world->code[1]),'\0'};
 AuthoredClass49N trackClass=AuthoredClass49N::A;
 require50DU(selectAuthoredClass49N(authoredTrackName,2,trackClass),
     "loaded original track's authored handling class unavailable");
 // The actual human's selected mode controls source215408..21549C cold
 // steering/engine torque binding as well as its later48W input branch.
 // ALL8 keeps the existing bootstrap and per-AI native mode58 rebind intact.
 const AuthoredSelectionContext49N selection{trackClass,false,
     allAiEnabled||humanControlInput.twinEngine,false,false,{}};
 auto authored=runtime_startup50fl::bind(phy,geometry,selection,x.parameters,s,x.selectors,x.secondaryInputs);
 // Player1A0858..5C preserves the fully authored aero350+100 as10A4.
 // Capture once, before the finish leaf changes the live steering coefficient.
 float authoredFinishSteering10A4=x.parameters.bodies.main.aero.steering100;
 // Explicit native namespace shared with the retained collider/query owners.
 // These IDs/masks are host bindings, not reconstructed guest addresses.
 terrain_probe_authoring50fq_fixture::Owner terrainOwner{};
 terrain_probe_authoring50fq_fixture::Bindings terrainBindings{};
 terrainBindings.identity=native_query_identities50KX::terrain348;
 terrainBindings.physicsBodyCategory=2;terrainBindings.staticObjectCategory=4;
 terrainBindings.floorCategory=16;terrainBindings.tubeCategory=64;
 terrainBindings.collisionTypeTag=query.collisionTypeTag;
 terrainBindings.groupTypeTag=query.groupTypeTag;
 terrainBindings.flags31=3;terrainBindings.preservedFlagsF8=0;
 std::string terrainAuthoringError;
 const bool terrainAuthored=terrain_probe_authoring50fq_fixture::prepare(terrainOwner,query,
     x.parameters,phy,terrainBindings,terrainAuthoringError);
 require50DU(terrainAuthored,terrainAuthoringError.c_str());
 for(unsigned axis=0;axis<3;++axis)x.parameters.bodies.main.probeLocal140[axis]=terrainOwner.lowerEndpoint140.v[axis];
 x.parameters.bodies.main.probeLocal140[3]=1.f;
 // Explicit diagnostic movement-query state and complete configured static order.
 // The late movement detector retains its separate explicit terrain order.
 Type3QueryInput48P movement{};movement.filter=qfilter;movement.filter.identity=native_query_identities50KX::movement7A4;movement.filter.categoryA4=2;movement.filter.acceptMaskB8=4;movement.filter.flagsF8=6;movement.filter.flags31=1;
 //212444 constructs movement7A4 through11C710;11C898 stores shape+44=0.
 movement.normalModeBound50JA=true;
 movement.pairCount=1;movement.normalMode=0;movement.callbackEnabled=false;movement.collisionTypeTag=query.collisionTypeTag;movement.groupTypeTag=query.groupTypeTag;movement.dynamicMask=2;movement.sentinel={{0,0,0,1}};
 movement.copied40=query.copied40;movement.copied50=query.copied50;for(unsigned i:{0u,5u,10u,15u})movement.bodyScene.v[i]=1;
 undertrack_owners50ew_fixture::Owners recoveryQueryOwners;
 undertrack_owners50ew_fixture::SourceGlobals recoveryGlobals{};
 recoveryGlobals.podCategory3E0718=terrainBindings.physicsBodyCategory;
 recoveryGlobals.extraPodCategory3B14B0=OriginalNativeCollisionCategories::pod;
 recoveryGlobals.movementCategory3E0720=query.materialMask;
 recoveryGlobals.collisionTypeTag=query.collisionTypeTag;recoveryGlobals.podFlags31=3;
 recoveryGlobals.identity[0]=native_query_identities50KX::pod694;
 recoveryGlobals.identity[1]=movement.filter.identity;
 recoveryGlobals.terrainIdentity348=terrainOwner.metadata.filter.identity;
 std::shared_ptr<ra_cold_aggregate50LA::Owner> raWorld;
 std::shared_ptr<ra_cold_aggregate50LA::Owner::NativeRuntimeView> raRuntime;
 ra_cold_aggregate50LA::Diagnostic raDiagnostic{};
 if(isRaTrack){
  // Keep the nested NativeRuntimeView identity and exact final publication in
  // this owning scope. The synchronous provider calls it before its locals die.
  const auto publishNativeRuntime=[&](){
  raRuntime=raWorld->borrowNativeRuntime(raWorld,raDiagnostic);
  require50DU(bool(raRuntime),raDiagnostic.failure?raDiagnostic.failure:"RA runtime world borrow rejected");
  };
  interactive_ra_cold50DU::Context{
  .raWorld=raWorld,
  .raDiagnostic=raDiagnostic,
  .query=query,
  .qfilter=qfilter,
  .raRosterOwners=raRosterOwners,
  .originalAiHumanCount=originalAiHumanCount,
  .raceRacerCount=raceRacerCount,
  .s=s,
  .trackSession=trackSession,
  .consumedLaunch=consumedLaunch,
  .allAiEnabled=allAiEnabled,
  .sourceConsumedIntroActivation=sourceConsumedIntroActivation,
  .originalCameraEnabled=originalCameraEnabled,
  .openNativeVideo=openNativeVideo,
  .originalMainApplication=originalMainApplication,
  .nativeVideo=nativeVideo,
  .sourceModelRenderGlobals=sourceModelRenderGlobals,
  .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
  .consumedRaStage=consumedRaStage,
  .sessionIndex=sessionIndex,
  .playerRacer60=playerRacer60,
  .sourceConsumedPauseActivation=sourceConsumedPauseActivation,
  .recoveryQueryOwners=recoveryQueryOwners,
  .recoveryGlobals=recoveryGlobals,
  .terrainOwner=terrainOwner,
  .movement=movement,
  .publishNativeRuntime=interactive_frame50DU::BorrowedCall<void()>(publishNativeRuntime),
 }.run();
 }
 authored_runtime_ground50cp_fixture::Request groundRequest{};
 groundRequest.archive=trackSession->archive;groundRequest.worldMember=trackSession->world->worldMember;
 groundRequest.mapSpline=trackSession->mapSpline;groundRequest.gridRank=trackSession->gridRank;
 groundRequest.archiveSha256=trackSession->archiveSha256;groundRequest.archiveBytes=trackSession->archiveBytes;
 groundRequest.bindFreshGameTrackHeight=originalAiEnabled;
 if(directOriginalHuman)groundRequest.originalPodPhysics=&x.assets.phy;
 track_runtime_storage50IG::Limits trackStorageLimits;trackStorageLimits.originalCameraQueries=originalCameraEnabled;
 // Explicit MC PC-native48MiB aggregate budget; all other tracks retain32MiB.
 if(isMcTrack)trackStorageLimits.coldPayloadBytes=48u*1024u*1024u;
 trackStorageLimits.originalRecoveryQueries=originalRecoveryEnabled;
 // Conservative precharge for GB fields added to shared VehicleWindow metadata, on every route.
 // This reduces each route's bounded allowance; MC alone uses the measured48MiB policy above.
 require50DU(trackStorageLimits.coldPayloadBytes>=rr::platform::gbParticleRendererMetadataReservation50DS,"shared renderer metadata reservation overflow");
 trackStorageLimits.coldPayloadBytes-=rr::platform::gbParticleRendererMetadataReservation50DS;
 if(isMcTrack){require50DU(trackStorageLimits.coldPayloadBytes>=rr::platform::mcParticleRendererMetadataReservation50DS,"MC renderer metadata reservation overflow");trackStorageLimits.coldPayloadBytes-=rr::platform::mcParticleRendererMetadataReservation50DS;}
 auto mbStorageLimits=trackStorageLimits;
 // These source/mesh owners precede every candidate, Storage and Bank borrower.
 // TC source/Gate/Sound/Action are independent strong owners; none retains
 // the aggregate. The final views and typed Borrow below retain actual Storage.
 // Immutable RC source is retained by the real canonical Application mount
 // across serial races. Each race owns fresh final collision/navigation leases.
#include "gb_application_cold50gb.inl"
#include "mc_application_cold50mc.inl"
 std::shared_ptr<const tb_cold_sources50LA::Owner> tbColdSources;
 std::shared_ptr<rr::game::original_gc_gameplay_lighting_candidate::Owner> tbLightOwner;
 std::shared_ptr<tb_trigger_authoring50TB::Owner> tbTriggerAuthoring;
 std::shared_ptr<tb_collision_world50TB::Owner> tbCollisionWorld;
 std::optional<tb_collision_world50TB::Owner::Borrow> tbWorldBorrow;
 tb_navigation50TB::Owner tbNavigation;
 tb_navigation50TB::Diagnostic tbNavigationDiagnostic;
 std::shared_ptr<tb_trigger_actions50TB::Owner> tbTriggerActions;
 std::shared_ptr<tb_door_motion50TB::SourceOwner> tbDoorSource;
 std::shared_ptr<tb_door_runtime50TB::Owner> tbDoorRuntime;
 std::unique_ptr<tb_door_binding50TB::Binding> tbDoorBinding;
 tb_door_runtime50TB::Diagnostic tbDoorDiagnostic;
 tb_door_binding50TB::Diagnostic tbDoorBindingDiagnostic;
 std::size_t tbExternalPayloadReservation=0;
 struct TbDoorPrefixGuard {
  decltype(tbDoorBinding)& binding;tb_door_binding50TB::Diagnostic& diagnostic;bool armed=true;
  ~TbDoorPrefixGuard()noexcept{if(armed&&binding&&!binding->close(diagnostic)){
   std::fputs("TB reached door registration retirement rejected; backing retained\n",stderr);std::terminate();
  }}
 } tbDoorPrefixGuard{tbDoorBinding,tbDoorBindingDiagnostic};
 if(isTbTrack){
  require50DU(tbOriginalDisc&&directOriginalHuman&&originalAiEnabled&&nativeOriginalFleet,
      "TB requires actual original human/seven-AI source ownership");
  // A fresh Application has not reached APPSTART yet. Only a previously
  // prepared real Application can carry a canonical source from an earlier race.
  if(originalMainApplication.materialApplication&&originalMainApplication.materialApplication->prepared()){
   const auto tbDomain=originalMainApplication.standardMaterialDomain();
   const auto tbProvider=tbDomain?tbDomain->archiveProvider3E1A70():nullptr;
   require50DU(bool(tbProvider),"TB actual Application archive provider absent");
   rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic reusedDiagnostic;
   const bool reused=tb_cold_sources50LA::Owner::reuseCanonicalSource(*tbProvider,tbColdSources,reusedDiagnostic);
   require50DU(reused,"TB retained canonical source identity/thread reuse rejected");
  }
  if(!tbColdSources){
   auto source=std::make_shared<tb_cold_sources50LA::Owner>();
   tb_cold_sources50LA::Diagnostic sourceDiagnostic;
   auto original=runtime_startup50fl::readBytes(trackSession->archive);
   const bool decoded=source->decodeOwned(std::move(original),s.animationShared.libm,
       trackStorageLimits.coldPayloadBytes,sourceDiagnostic);
   require50DU(decoded,sourceDiagnostic.message.empty()?"TB complete compact source rejected":sourceDiagnostic.message.c_str());
   tbColdSources=std::move(source);
  }
  require50DU(tbColdSources->ready()&&tbColdSources->retainedPayloadBytes()<=trackStorageLimits.coldPayloadBytes,
      "TB retained source exceeds existing payload allowance");
  const auto reserveEarly=[&](std::size_t bytes){
   require50DU(bytes<=trackStorageLimits.coldPayloadBytes&&tbExternalPayloadReservation<=trackStorageLimits.coldPayloadBytes-bytes,
       "TB early external reservation overflow");tbExternalPayloadReservation+=bytes;
  };
  reserveEarly(tb_trigger_authoring50TB::Owner::maximumAdditionalPayloadBytes());
  reserveEarly(tb_door_motion50TB::SourceOwner::maximumAdditionalPayloadBytes());
  reserveEarly(tb_door_runtime50TB::Owner::requiredPayloadBytes());
  reserveEarly(tb_door_binding50TB::Binding::requiredPayloadBytes());
  reserveEarly(sizeof(tb_trigger_actions50TB::Owner));
  reserveEarly(rr::game::original_gc_gameplay_lighting_candidate::Owner::tbDoorStorageReservationBytes());
  require50DU(tb_collision_world50TB::Owner::requiredPreStoragePayloadBytes(*tbColdSources,tbExternalPayloadReservation)<trackStorageLimits.coldPayloadBytes,
      "TB complete early source/ground/door descriptor reservation exceeds existing allowance");
  tbLightOwner=std::make_shared<rr::game::original_gc_gameplay_lighting_candidate::Owner>();
  rr::game::original_gc_gameplay_lighting_candidate::Diagnostic lightDiagnostic;
  const bool lightsReady=tbLightOwner->prepareTbNative(tbColdSources->world().parameters.original_bytes,
      raceRacerCount,lightDiagnostic);
  require50DU(lightsReady&&tbLightOwner->world()&&
      tbLightOwner->world()->parameters.original_bytes==tbColdSources->world().parameters.original_bytes,
      "TB actual same-source lighting preparation rejected");
  const bool navigationPrepared=tbNavigation.prepare(tbColdSources,tbNavigationDiagnostic);
  require50DU(navigationPrepared,tbNavigationDiagnostic.message.empty()?"TB navigation source rejected":tbNavigationDiagnostic.message.c_str());
  tbCollisionWorld=std::make_shared<tb_collision_world50TB::Owner>();
  std::string error;
  const bool groundReady=tbCollisionWorld->prepareGround(tbColdSources,{4,16,32,64},x.assets.phy,trackSession->gridRank,tbExternalPayloadReservation,trackStorageLimits,error);
  require50DU(groundReady,error.empty()?"TB genuine collision Ground rejected":error.c_str());
  tbTriggerAuthoring=std::make_shared<tb_trigger_authoring50TB::Owner>();
  const bool triggerReady=tbTriggerAuthoring->decode(tbColdSources,error);
  require50DU(triggerReady,error.empty()?"TB authenticated trigger source rejected":error.c_str());
  prepareTbPodSound(tbTriggerAuthoring);
  tbDoorSource=std::make_shared<tb_door_motion50TB::SourceOwner>();tb_door_motion50TB::Diagnostic doorSourceDiagnostic;
  const bool sourceReady=tbDoorSource->decode(tbColdSources,s.animationShared.libm,
      tb_door_motion50TB::SourceOwner::maximumAdditionalPayloadBytes(),doorSourceDiagnostic);
  require50DU(sourceReady,doorSourceDiagnostic.message.empty()?"TB actual door source rejected":doorSourceDiagnostic.message.c_str());
  tbDoorRuntime=std::make_shared<tb_door_runtime50TB::Owner>();
  const bool begun=tbDoorRuntime->beginPrepare(tbDoorSource,s.animationShared.motion,
      tb_door_runtime50TB::Owner::requiredPayloadBytes(),tbDoorDiagnostic);
  require50DU(begun,tbDoorDiagnostic.message.empty()?"TB actual root graph rejected":tbDoorDiagnostic.message.c_str());
  tbDoorBinding=std::make_unique<tb_door_binding50TB::Binding>();
  const bool visualAttached=tbDoorBinding->beginVisual(tbDoorRuntime,tbLightOwner,tbDoorBindingDiagnostic);
  require50DU(visualAttached,tbDoorBindingDiagnostic.message.empty()?"TB actual door visual prefix rejected":tbDoorBindingDiagnostic.message.c_str());
  tbTriggerActions=std::make_shared<tb_trigger_actions50TB::Owner>();
  require50DU(tbTriggerActions->prepare(tbTriggerAuthoring,tbDoorRuntime,tbPodSound),"TB actual ordered action owner preparation rejected");
 }
 std::shared_ptr<const rc_cold_sources50LA::Owner> rcColdSources;
 std::shared_ptr<rr::game::original_gc_gameplay_lighting_candidate::Owner> rcLightOwner;
 std::shared_ptr<rc_trigger_authoring::Owner> rcTriggerAuthoring;
 std::shared_ptr<rc_collision_world50RC::Owner> rcCollisionWorld;
 std::optional<rc_collision_world50RC::Owner::Borrow> rcWorldBorrow;
 rc_navigation50RC::Owner rcNavigation;
 rc_navigation50RC::Diagnostic rcNavigationDiagnostic;
 rc_trigger_actions::Owner* rcTriggerActions=nullptr;
 if(isRcTrack){
  require50DU(rcOriginalDisc&&directOriginalHuman&&originalAiEnabled&&nativeOriginalFleet,
      "RC requires actual original human/seven-AI source ownership");
  // A fresh Application has not reached APPSTART yet. Only a previously
  // prepared real Application can carry a canonical source from an earlier race.
  if(originalMainApplication.materialApplication&&originalMainApplication.materialApplication->prepared()){
   const auto rcDomain=originalMainApplication.standardMaterialDomain();
   const auto rcProvider=rcDomain?rcDomain->archiveProvider3E1A70():nullptr;
   require50DU(bool(rcProvider),"RC actual Application archive provider absent");
   rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic reusedDiagnostic;
   const bool reused=rc_cold_sources50LA::Owner::reuseCanonicalSource(*rcProvider,rcColdSources,reusedDiagnostic);
   require50DU(reused,"RC retained canonical source identity/thread reuse rejected");
  }
  if(!rcColdSources){
   auto source=std::make_shared<rc_cold_sources50LA::Owner>();
   rc_cold_sources50LA::Diagnostic sourceDiagnostic;
   auto original=runtime_startup50fl::readBytes(trackSession->archive);
   const bool decoded=source->decodeOwned(std::move(original),s.animationShared.libm,
       trackStorageLimits.coldPayloadBytes,sourceDiagnostic);
   require50DU(decoded,sourceDiagnostic.message.empty()?"RC complete compact source rejected":sourceDiagnostic.message.c_str());
   rcColdSources=std::move(source);
  }
  require50DU(rcColdSources->ready()&&rcColdSources->retainedPayloadBytes()<=trackStorageLimits.coldPayloadBytes,
      "RC retained source exceeds existing payload allowance");
  rcLightOwner=std::make_shared<rr::game::original_gc_gameplay_lighting_candidate::Owner>();
  rr::game::original_gc_gameplay_lighting_candidate::Diagnostic lightDiagnostic;
  const bool lightsReady=rcLightOwner->prepareRcNative(rcColdSources->world().parameters.original_bytes,
      raceRacerCount,lightDiagnostic);
  require50DU(lightsReady&&rcLightOwner->world()&&
      rcLightOwner->world()->parameters.original_bytes==rcColdSources->world().parameters.original_bytes,
      "RC actual same-source lighting preparation rejected");
  const bool navigationPrepared=rcNavigation.prepare(rcColdSources,rcNavigationDiagnostic);
  require50DU(navigationPrepared,rcNavigationDiagnostic.message.empty()?"RC navigation source rejected":rcNavigationDiagnostic.message.c_str());
  rcCollisionWorld=std::make_shared<rc_collision_world50RC::Owner>();
  std::string error;
  const bool groundReady=rcCollisionWorld->prepareGround(rcColdSources,{4,16,32,64},x.assets.phy,trackSession->gridRank,error);
  require50DU(groundReady,error.empty()?"RC genuine collision Ground rejected":error.c_str());
  rcTriggerAuthoring=std::make_shared<rc_trigger_authoring::Owner>();
  const bool triggerReady=rcTriggerAuthoring->decode(rcColdSources,error);
  require50DU(triggerReady,error.empty()?"RC authenticated trigger source rejected":error.c_str());
 }
 std::shared_ptr<const tc_cold_sources50LA::Owner> tcColdSources;
 std::shared_ptr<tc_trigger_authoring50TC::Owner> tcTriggerAuthoring;
 std::shared_ptr<tc_gate_motion50TC::SourceOwner> tcGateSource;
 std::shared_ptr<rr::game::original_gc_gameplay_lighting_candidate::Owner> tcLightOwner;
 rr::game::original_gc_gameplay_lighting_candidate::Diagnostic tcLightDiagnostic{};
 std::shared_ptr<tc_gate_runtime50TC::Owner> tcGateRuntime;
 std::unique_ptr<tc_gate_visibility50TC::Binding> tcGateVisibility;
 tc_gate_visibility50TC::Diagnostic tcGateVisibilityDiagnostic{};
 // This boundary exists before the first real gate observer can attach.
 // Later GPU/camera cleanup takes over only once closeWindow is established.
 struct TcGatePrefixGuard {
  decltype(tcGateVisibility)& binding;tc_gate_visibility50TC::Diagnostic& diagnostic;
  bool armed=true;
  ~TcGatePrefixGuard()noexcept{if(armed&&binding&&!binding->close(diagnostic)){
   std::fputs("TC initial gate visibility retirement rejected; owner backing retained\n",stderr);
   std::terminate();
  }}
 } tcGatePrefixGuard{tcGateVisibility,tcGateVisibilityDiagnostic};
 std::shared_ptr<tc_global_sound50TC::Owner> tcGlobalSound;
 std::shared_ptr<tc_trigger_actions50TC::Owner> tcTriggerActions;
 std::shared_ptr<tc_collision_world50TC::Owner> tcCollisionWorld;
 std::optional<tc_collision_world50TC::Owner::Borrow> tcWorldBorrow;
 tc_navigation50TC::Owner tcNavigation;
 tc_navigation50TC::Diagnostic tcNavigationDiagnostic;
 tc_gate_runtime50TC::Diagnostic tcGateDiagnostic;
 bool tcLiveAudioPrepared=false;
 if(isTcTrack){
  require50DU(tcOriginalDisc&&directOriginalHuman&&originalAiEnabled&&nativeOriginalFleet,
      "TC requires actual original human/AI and ordinary Sound ownership");
  auto source=std::make_shared<tc_cold_sources50LA::Owner>();
  tc_cold_sources50LA::Diagnostic diagnostic;
  const auto original=runtime_startup50fl::readBytes(trackSession->archive);
  const bool decoded=source->decode(original,s.animationShared.libm,trackStorageLimits.coldPayloadBytes,diagnostic);
  require50DU(decoded,diagnostic.message.empty()?"TC genuine cold source rejected":diagnostic.message.c_str());
  tcColdSources=std::move(source);
  tcLightOwner=std::make_shared<rr::game::original_gc_gameplay_lighting_candidate::Owner>();
  const bool lightsReady=tcLightOwner->prepareTcNative(tcColdSources->world().parameters.original_bytes,
      raceRacerCount,tcLightDiagnostic);
  require50DU(lightsReady&&tcLightOwner->world()&&
      tcLightOwner->world()->parameters.original_bytes==tcColdSources->world().parameters.original_bytes,
      "TC genuine early lighting source preparation rejected");
  const bool navigationPrepared=tcNavigation.prepare(tcColdSources,tcNavigationDiagnostic);
  require50DU(navigationPrepared,tcNavigationDiagnostic.message.empty()?"TC actual navigation source rejected":tcNavigationDiagnostic.message.c_str());
  tcCollisionWorld=std::make_shared<tc_collision_world50TC::Owner>();
  std::string error;
  const bool groundReady=tcCollisionWorld->prepareGround(tcColdSources,{4,16,32,64},x.assets.phy,trackSession->gridRank,error);
  require50DU(groundReady,error.empty()?"TC same-source original collision Ground rejected":error.c_str());
  tcTriggerAuthoring=std::make_shared<tc_trigger_authoring50TC::Owner>();
  const bool triggersReady=tcTriggerAuthoring->decode(tcColdSources,error);
  require50DU(triggersReady,error.empty()?"TC authentic trigger descriptors rejected":error.c_str());
  tcGateSource=std::make_shared<tc_gate_motion50TC::SourceOwner>();
  tc_gate_motion50TC::Diagnostic gateSourceDiagnostic;
  const bool gateSourceReady=tcGateSource->decode(tcColdSources,gateSourceDiagnostic);
  require50DU(gateSourceReady,gateSourceDiagnostic.message.empty()?"TC actual gate resources rejected":gateSourceDiagnostic.message.c_str());
  tcGateRuntime=std::make_shared<tc_gate_runtime50TC::Owner>();
  const bool gateReady=tcGateRuntime->beginPrepare(tcGateSource,s.animationShared.motion,
      trackStorageLimits.coldPayloadBytes,tcGateDiagnostic);
  require50DU(gateReady,tcGateDiagnostic.message.empty()?"TC original gate source placement rejected":tcGateDiagnostic.message.c_str());
  tcGateVisibility=std::make_unique<tc_gate_visibility50TC::Binding>();
  const bool gateRegistered=tcGateVisibility->prepare(tcGateRuntime,tcLightOwner,
      s.animationShared.motion,tcGateVisibilityDiagnostic);
  require50DU(gateRegistered&&tcGateVisibility->ready()&&tcGateRuntime->ready(),
      "TC actual method2 gate registration/initial sample rejected");
  // Same live audio owner/settings as the later material/engine/camera joins.
  // TC's three-action group must own both actual voices before Storage publish.
  const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
  const std::filesystem::path audioRoot=suppliedAudioRoot?std::filesystem::path(suppliedAudioRoot):
      runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO");
  const bool audioPrepared=liveAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),
      audioRoot,error,playerRacer60,loadedAudioSettings);
  require50DU(audioPrepared,error.empty()?"TC real shared audio preparation rejected":error.c_str());
  tcLiveAudioPrepared=true;
  auto* mixer=liveAudio->mixerForMaterialOwner();auto* groups=liveAudio->groupsForProviders();
  require50DU(mixer&&groups&&groups->uses(*mixer),"TC actual shared mixer/groups unavailable");
  tcGlobalSound=std::make_shared<tc_global_sound50TC::Owner>();
  const bool soundReady=tcGlobalSound->prepare(tcTriggerAuthoring,*mixer,*groups,audioRoot,error);
  require50DU(soundReady,error.empty()?"TC genuine ordinary Sound sources rejected":error.c_str());
  tcTriggerActions=std::make_shared<tc_trigger_actions50TC::Owner>();
  const bool actionsReady=tcTriggerActions->prepare(tcTriggerAuthoring,tcGateRuntime,tcGlobalSound);
  require50DU(actionsReady,"TC actual ordered action owners rejected");
 }
 std::shared_ptr<const sc_cold_sources50LA::Owner> scColdSources;
 std::unique_ptr<sc_runtime_ground50LA::Owner> scGround;
 if(isScTrack){
  require50DU(scOriginalDisc&&directOriginalHuman&&originalAiEnabled,
      "SC requires actual original-disc fresh human/AI ownership");
  auto source=std::make_shared<sc_cold_sources50LA::Owner>();
  sc_cold_sources50LA::Diagnostic diagnostic;
  const auto original=runtime_startup50fl::readBytes(trackSession->archive);
  const bool decoded=source->decode(original,s.animationShared.libm,diagnostic);
  require50DU(decoded,diagnostic.failure?diagnostic.failure:"SC genuine cold source rejected");
  scColdSources=std::move(source);scGround=std::make_unique<sc_runtime_ground50LA::Owner>();
  std::string error;
  const bool prepared=scGround->prepare(scColdSources,{4,16,32,64},x.assets.phy,trackSession->gridRank,error);
  require50DU(prepared,error.empty()?"SC same-source collision owners rejected":error.c_str());
 }
 std::unique_ptr<mb_original_world50LM::Owner> mbOriginalWorld;
 std::shared_ptr<mb_collision_world50MB::Owner> mbCollisionWorld;
 mb_navigation50MB::Owner mbNavigation;
 std::unique_ptr<mb_source_trigger50MB::Owner> mbSourceTriggers;
 mb_navigation50MB::Diagnostic mbNavigationDiagnostic;
 std::size_t mbNavigationPayload=0;
 if(isMbTrack){
  require50DU(mbOriginalDisc&&directOriginalHuman,"MB requires its original-disc human route");
  constexpr auto navigationPayload=mb_navigation50MB::Owner::persistentPayloadBytes()+mb_navigation50MB::Owner::ownerBytes();
  constexpr auto gateScratch=mb_navigation50MB::Owner::coldScratchBytes();
  require50DU(navigationPayload<=trackStorageLimits.coldPayloadBytes&&
      gateScratch<=trackStorageLimits.coldPayloadBytes-navigationPayload,
      "MB navigation exceeds existing native cold payload allowance");
  mbNavigationPayload=navigationPayload;
  mbStorageLimits.coldPayloadBytes-=navigationPayload;
  mbOriginalWorld=std::make_unique<mb_original_world50LM::Owner>();
  {
   auto raw=runtime_startup50fl::readBytes(trackSession->archive);
   require50DU(raw.size()==mb_original_world50LM::archiveBytes&&
       pod_materials50ed_fixture::detail::sha256(raw)==mb_original_world50LM::archiveSha,
       "MB visual bootstrap original archive identity");
   rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;rr::assets::World48F sourceWorld;
   std::vector<std::uint8_t> sourceScb;
   require50DU(rr::assets::decodeResource48C(raw,archive,error)&&
       rr::assets::extractResMember48C(archive,"tracks\\mb.scb",sourceScb,error)&&
       sourceScb.size()==mb_original_world50LM::scbBytesCount&&
       pod_materials50ed_fixture::detail::sha256(sourceScb)==mb_original_world50LM::scbSha&&
       rr::assets::decodeWorld48F(sourceScb,sourceWorld,error),"MB authentic visual bootstrap SCB");
   mb_original_world50LM::Diagnostic d;
   const bool loaded=mbOriginalWorld->load(raw,sourceWorld,d);
   require50DU(loaded,d.message.empty()?"MB visual cold owner rejected":d.message.c_str());
  } // Bootstrap archive/world retire; the visual owner retains its own source.
  const bool navigationReady=mbNavigation.prepareGates(*mbOriginalWorld,s.animationShared.libm,
      mbNavigationPayload,mbNavigationDiagnostic);
  require50DU(navigationReady,mbNavigationDiagnostic.message.empty()?"MB navigation gates rejected":mbNavigationDiagnostic.message.c_str());
  mbCollisionWorld=std::make_shared<mb_collision_world50MB::Owner>();
  track_static_candidates50IG::Bindings names{};names.firstIdentity=0x10000000u;
  names.collisionTypeTag=query.collisionTypeTag;names.groupTypeTag=query.groupTypeTag;
  names.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  names.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  std::string error;const bool groundReady=mbCollisionWorld->prepareGround(groundRequest,mbNavigation.map(),names,error);
  require50DU(groundReady,error.empty()?"MB actual collision owner rejected":error.c_str());
 }
 authored_runtime_ground50cp_fixture::ColdScene legacyGround{};
 if(!isRaTrack&&!isMbTrack&&!isScTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack)legacyGround=authored_runtime_ground50cp_fixture::load({4,16,32,64},groundRequest);
 require50DU(!isTbTrack||(tbCollisionWorld&&tbCollisionWorld->coldSource()==tbColdSources.get()&&tbCollisionWorld->ground()),
     "TB ground alias requires the actual same compact source owner");
 const auto& ground=isMcTrack?*mcCollisionWorld->ground():isGbTrack?*gbCollisionWorld->ground():isTbTrack?*tbCollisionWorld->ground():isRcTrack?*rcCollisionWorld->ground():isTcTrack?*tcCollisionWorld->ground():isScTrack?*scGround->ground():isMbTrack?mbCollisionWorld->ground():legacyGround;
 auto& preparedCandidates=isRaTrack?raRuntime->candidates:legacyCandidates;
 const auto& actualWorld=isRaTrack?raRuntime->source.world:ground.world;
 // Decode the same real TA owner before counted construction/audio loading.
 // It is later used directly for scene materials/mesh; no second world is made.
 if(taOriginalDisc){
  taOriginalWorld=std::make_shared<ta_original_world50LE::Owner>();
  ta_original_world50LE::Diagnostic diagnostic;
  const bool loaded=taOriginalWorld->loadOwned(runtime_startup50fl::readBytes(trackSession->archive),actualWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"TA original static world rejected":diagnostic.message.c_str());
  require50DU(taOriginalWorld->mesh(),"TA original mesh publication missing");
  rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection taSelected{};
  const bool taAuthoredIntro=originalMainApplication.riderProfile&&
      originalMainApplication.riderProfile->courseRaceSnapshot(taSelected)&&
      taSelected.mode34==2&&taSelected.subtype==1;
  // Actual174E48 gates: startup350 is zero; subtype2/3 never use authored cuts.
  // Keep ordinary Single Event and record/practice owners unchanged.
  std::cerr<<"consumed_ta_gate launch="<<bool(consumedLaunch)<<" camera="<<originalCameraEnabled
      <<" mode34="<<taSelected.mode34<<" subtype="<<taSelected.subtype<<" admitted="<<taAuthoredIntro<<"\n";
  if(consumedLaunch&&originalCameraEnabled&&taAuthoredIntro){
   openNativeVideo();
   originalMainApplication.appStart(originalMainApplication.assetRoot);
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
   const auto context=nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication);
   const auto domain=originalMainApplication.standardMaterialDomain();
   require50DU(context&&domain&&domain->prepared(),"TA counted race material context unavailable");
   if(!sourceModelRenderGlobals)sourceModelRenderGlobals=
       std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
   rr::assets::OriginalStandardMaterialDiagnostic50LA d;std::uint32_t pc=0;
   const bool ready=consumedTaStage.begin(*consumedLaunch,sessionIndex+1,playerRacer60,
       *sourceModelRenderGlobals,taOriginalWorld,d,pc);
   if(!ready)std::cerr<<"consumed_ta_prefix_rejected source_pc="<<pc
       <<" material_pc="<<d.sourcePC<<" material_status="<<static_cast<unsigned>(d.status)<<'\n';
   require50DU(ready,"actual consumed TA race/world constructor prefix rejected");
  }
 }

 // Decode GC once before counted audio construction; renderer later borrows
 // this same typed original archive/mesh, not a replacement world.
 if(gcOriginalDisc){
  gcOriginalWorld=std::make_shared<gc_original_world50LM::Owner>();
  gc_original_world50LM::Diagnostic diagnostic;
  const bool loaded=gcOriginalWorld->load(runtime_startup50fl::readBytes(trackSession->archive),actualWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"GC original all-pass world rejected":diagnostic.message.c_str());
  require50DU(gcOriginalWorld->mesh(),"GC original mesh publication missing");
  if(consumedLaunch&&originalCameraEnabled&&humanCourseEnabled&&dynamicProfile.game34==2&&nativeOrdinaryLaunch11258==1){
   openNativeVideo();originalMainApplication.appStart(originalMainApplication.assetRoot);
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
   const auto context=nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication);
   const auto domain=originalMainApplication.standardMaterialDomain();
   require50DU(context&&domain&&domain->prepared(),"GC counted race material context unavailable");
   if(!sourceModelRenderGlobals)sourceModelRenderGlobals=std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
   rr::assets::OriginalStandardMaterialDiagnostic50LA d;std::uint32_t pc=0;
   const bool ready=consumedGcStage.begin(*consumedLaunch,sessionIndex+1,playerRacer60,
       *sourceModelRenderGlobals,gcOriginalWorld,d,pc);
   if(!ready)std::cerr<<"consumed_gc_prefix_rejected source_pc="<<pc<<" material_pc="<<d.sourcePC<<'\n';
   require50DU(ready,"actual consumed GC race/world constructor prefix rejected");
  }
 }

 // Preserve the one actual MA static owner across source prefix and rendering.
 if(maOriginalDisc){
  maOriginalWorld=std::make_shared<ma_original_world50LM::Owner>();
  ma_original_world50LM::Diagnostic diagnostic;
  const bool loaded=maOriginalWorld->load(runtime_startup50fl::readBytes(trackSession->archive),actualWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"MA original static world rejected":diagnostic.message.c_str());
  require50DU(maOriginalWorld->mesh(),"MA original mesh publication missing");
  if(consumedLaunch&&originalCameraEnabled&&humanCourseEnabled&&dynamicProfile.game34==2&&nativeOrdinaryLaunch11258==1){
   openNativeVideo();originalMainApplication.appStart(originalMainApplication.assetRoot);
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
   const auto context=nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication);
   const auto domain=originalMainApplication.standardMaterialDomain();
   require50DU(context&&domain&&domain->prepared(),"MA counted race material context unavailable");
   if(!sourceModelRenderGlobals)sourceModelRenderGlobals=std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
   rr::assets::OriginalStandardMaterialDiagnostic50LA d;std::uint32_t pc=0;
   const bool ready=consumedMaStage.begin(*consumedLaunch,sessionIndex+1,playerRacer60,
       *sourceModelRenderGlobals,maOriginalWorld,d,pc);
   if(!ready)std::cerr<<"consumed_ma_prefix_rejected source_pc="<<pc<<" material_pc="<<d.sourcePC<<'\n';
   require50DU(ready,"actual consumed MA race/world constructor prefix rejected");
  }
 }

 // Preserve the one actual SA static owner across source prefix and rendering.
 if(saOriginalDisc){
  saOriginalWorld=std::make_shared<sa_original_world50LM::Owner>();
  sa_original_world50LM::Diagnostic diagnostic;
  const bool loaded=saOriginalWorld->load(runtime_startup50fl::readBytes(trackSession->archive),actualWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"SA original static world rejected":diagnostic.message.c_str());
  require50DU(saOriginalWorld->mesh(),"SA original mesh publication missing");
  if(consumedLaunch&&originalCameraEnabled&&humanCourseEnabled&&dynamicProfile.game34==2&&nativeOrdinaryLaunch11258==1){
   openNativeVideo();originalMainApplication.appStart(originalMainApplication.assetRoot);
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
   const auto context=nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication);
   const auto domain=originalMainApplication.standardMaterialDomain();
   require50DU(context&&domain&&domain->prepared(),"SA counted race material context unavailable");
   if(!sourceModelRenderGlobals)sourceModelRenderGlobals=std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
   rr::assets::OriginalStandardMaterialDiagnostic50LA d;std::uint32_t pc=0;
   const bool ready=consumedSaStage.begin(*consumedLaunch,sessionIndex+1,playerRacer60,
       *sourceModelRenderGlobals,saOriginalWorld,d,pc);
   if(!ready)std::cerr<<"consumed_sa_prefix_rejected source_pc="<<pc<<" material_pc="<<d.sourcePC<<'\n';
   require50DU(ready,"actual consumed SA race/world constructor prefix rejected");
  }
 }

 // Preserve the same actual GA decoded static owner across counted source
 // prefix and later rendering. Query/water owners remain separate and real.
 if(gaOriginalDisc){
  gaOriginalWorld=std::make_shared<ga_original_world50LH::Owner>();
  ga_original_world50LH::Diagnostic diagnostic;
  const bool loaded=gaOriginalWorld->load(runtime_startup50fl::readBytes(trackSession->archive),actualWorld,diagnostic);
  require50DU(loaded,diagnostic.message.empty()?"GA original static world rejected":diagnostic.message.c_str());
  require50DU(gaOriginalWorld->mesh(),"GA original mesh publication missing");
  if(consumedLaunch&&originalCameraEnabled&&humanCourseEnabled&&dynamicProfile.game34==2&&nativeOrdinaryLaunch11258==1){
   openNativeVideo();originalMainApplication.appStart(originalMainApplication.assetRoot);
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
   const auto context=nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication);
   const auto domain=originalMainApplication.standardMaterialDomain();
   require50DU(context&&domain&&domain->prepared(),"GA counted race material context unavailable");
   if(!sourceModelRenderGlobals)sourceModelRenderGlobals=std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
   rr::assets::OriginalStandardMaterialDiagnostic50LA d;std::uint32_t pc=0;
   const bool ready=consumedGaStage.begin(*consumedLaunch,sessionIndex+1,playerRacer60,
       *sourceModelRenderGlobals,gaOriginalWorld,d,pc);
   if(!ready)std::cerr<<"consumed_ga_prefix_rejected source_pc="<<pc<<" material_pc="<<d.sourcePC<<'\n';
   require50DU(ready,"actual consumed GA race/world constructor prefix rejected");
  }
 }

 // Original 1AA1B8: after 1A9EE0 loads tracks\<track> (world, objects, lights).
 rr::platform::loading_progress50DU::step(.1f,0x1aa1b8);
 rr::platform::loading_progress50DU::pulse("Preparing terrain");
 coldLoadDiagnostics.begin("ground_query");
 const auto& actualHeight=isRaTrack?raRuntime->source.height:ground.height;
 if(originalAiEnabled){
  require50DU(actualHeight.sourceBound&&actualHeight.descriptor.bound,
      "original AI track330/334 source ownership unavailable");
  std::cerr<<"ORIGINAL AI height owners: terrain330="<<static_cast<unsigned>(actualHeight.sourceDomain.terrain330)
      <<" water334="<<static_cast<unsigned>(actualHeight.sourceDomain.water334)
      <<" source_bound="<<actualHeight.sourceBound
      <<" water_native_query="<<actualHeight.waterNativeQueryBound<<'\n';
 }
 require50DU(isRaTrack||ground.lift==x.parameters.visual.verticalLimit7EC,"same actual Pod01 lift");
 std::vector<std::uint32_t> groundStack(ground.stackCount);
 std::shared_ptr<mc_water50MC::Owner> mcWaterOwner;
 std::uint64_t mcWaterUpdateSequence=0;
 std::unique_ptr<native_gc_water_session50KG::Owner> nativeGcWater;
 std::array<std::unique_ptr<native_gc_water_racer50KG::Owner>,raceRacerCapacity> nativeWaterRacers;
 auto& nativeFireSerial=originalUiStarted?originalMainApplication.state->fireSerial433970:
     nativeWaterApplication.directFireSerial433970;
 if(nativeWaterEnabled){
  nativeGcWater=std::make_unique<native_gc_water_session50KG::Owner>();
  nativeGcWater->prepare(nativeWaterApplication,ground,trackSession->archive,playerArchive.string().c_str());
 }
 const auto groundAllocations=::allocations;
 BodyMatrix48G pose=isRaTrack?s.nodes[static_cast<std::int32_t>(x.bindings.lifecycle.main190)].world100:ground.pose;
 if(!isRaTrack){
 if(checkpointInput){
  recovery50FI::Diagnostic recoveryDiagnostic{};
  require50DU(recovery50FI::resolveGround(ground,*checkpointInput,16,0,
      groundStack.data(),groundStack.size(),pose,recoveryDiagnostic),
      "checkpoint actual ground resolution failed");
 }else{
  const auto groundProbe=authored_runtime_ground50cp_fixture::resolve(ground,16,0,groundStack.data(),groundStack.size());
  require50DU(groundProbe.phase==ResetProbePhase50AG::Hit,"actual grid ground height");
  for(unsigned i=0;i<4;++i)pose.v[12+i]=std::bit_cast<float>(groundProbe.staged[i]);
 }
 } // RA retained roster operation owns its actual M-spline/grid-ground placement.
 require50DU(::allocations==groundAllocations,"actual ground query without allocations");
 rr::platform::loading_progress50DU::pulse("Preparing collision");
 coldLoadDiagnostics.begin("static_candidates");
 std::vector<track_runtime_storage50IG::Binding> legacyTrackBindings;
 auto& preparedBindings=legacyTrackBindings;
 std::vector<track_static_candidates50IG::Radii> originalStaticRadii;
 if(!isRaTrack&&!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack){
 if(sourceStaticCandidates){
  track_static_candidates50IG::Bindings staticBindings{};
  staticBindings.firstIdentity=0x10000000u; // Explicit collision-object native namespace.
  staticBindings.collisionTypeTag=query.collisionTypeTag;staticBindings.groupTypeTag=query.groupTypeTag;
  staticBindings.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  staticBindings.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  track_static_candidates50IG::prepare(ground,staticBindings,preparedCandidates,originalStaticRadii,preparedBindings);
  for(const auto& candidate:preparedCandidates)stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }else{
 preparedBindings.resize(preparedCandidates.size());
 const auto& row=*chosen;
 for(std::size_t k=0;k<preparedCandidates.size();++k){const auto&j=row["candidates"][row["order"][k].get<std::size_t>()];const auto name=authored_runtime_ground50cp_fixture::lower(corpus["meshes"][j["mesh"].get<std::size_t>()]["name"].get<std::string>());const authored_runtime_ground50cp_fixture::Owner*match=nullptr;for(const auto&o:ground.owners)if(authored_runtime_ground50cp_fixture::lower(o.name)==name){bool equal=true;for(unsigned n=12;n<15;++n)if(o.instance.world.v[n]!=preparedCandidates[k].candidate.collision.geometry.worldMatrix.v[n])equal=false;if(equal){match=&o;break;}}require50DU(match,"actual collider placement matches explicit candidate");preparedBindings[k]={static_cast<std::size_t>(match-ground.owners.data()),&preparedCandidates[k]};auto&radiusOwner=meshes[j["mesh"].get<std::size_t>()];require50DU(radiusOwner.radii.size()==match->mesh.nodes.size(),"actual mesh radius extent");std::vector<std::uint32_t> topology(match->mesh.nodes.size()+match->mesh.faces.size());require50DU(prepareStaticTargetRadii49X(match->mesh,radiusOwner.radii.data(),radiusOwner.radii.size(),{topology.data(),topology.size()},radiusOwner.maxDepth)==Type2ResourceStatus49X::Complete,"radii prepared from owned actual mesh");preparedCandidates[k].maxDepth=radiusOwner.maxDepth;stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+radiusOwner.maxDepth);auto&v=preparedCandidates[k].candidate.collision;v.geometry.mesh=&match->mesh;v.geometry.worldMatrix=match->instance.world;v.filter.categoryA4=match->instance.category;v.sphere={{{match->instance.sphere.center.v[0],match->instance.sphere.center.v[1],match->instance.sphere.center.v[2]}},match->instance.sphere.radius};}
 }
 require50DU(preparedCandidates.size()==trackSession->terrainOwners&&ground.owners.size()==preparedCandidates.size(),
     "original track binding must cover every retained terrain owner");
 }else if(isRaTrack){
  require50DU(raRuntime->source.collisions.size()==trackSession->terrainOwners&&
      preparedCandidates.size()==raRuntime->bindings.size(),"RA same-world candidate extent differs");
  for(const auto& candidate:preparedCandidates)stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }
 if(isMbTrack){
  require50DU(ground.owners.size()==trackSession->terrainOwners&&mbCollisionWorld->candidateStorage().size()==131&&mbCollisionWorld->bindingStorage().size()==131,
      "MB actual terrain and trigger closure changed");
  for(const auto& candidate:mbCollisionWorld->candidateStorage())stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }
 std::unique_ptr<sa_animated_world::Owner> saAnimatedWorld;
 sa_animated_world::Diagnostic saAnimationDiagnostic{};
 std::shared_ptr<sc_trigger_authoring::Owner> scTriggerAuthoring;
 std::unique_ptr<sc_trigger_actions::Owner> scTriggerActions;
 std::size_t scTriggerCandidateFirst=std::numeric_limits<std::size_t>::max();
 std::unique_ptr<ma_trigger_authoring::Owner> maTriggerAuthoring;
 std::unique_ptr<ma_trigger_actions::Owner> maTriggerActions;
 std::unique_ptr<sa_trigger_authoring::Owner> saTriggerAuthoring;
 std::unique_ptr<sa_trigger_actions::Owner> saTriggerActions;
 std::size_t maTriggerCandidateFirst=std::numeric_limits<std::size_t>::max();
 std::size_t saTriggerCandidateFirst=std::numeric_limits<std::size_t>::max();
 std::unique_ptr<ga_animated_world::Owner> gaAnimatedWorld;
 ga_animated_world::Diagnostic gaAnimationDiagnostic{};
 std::unique_ptr<ga_trigger_authoring::Owner> gaTriggerAuthoring;
 std::unique_ptr<ga_trigger_actions::Owner> gaTriggerActions;
 std::size_t gaTriggerCandidateFirst=std::numeric_limits<std::size_t>::max();
 std::unique_ptr<gc_animated_world50IG::Owner> gcAnimatedWorld;
 gc_animated_world50IG::Diagnostic gcAnimationDiagnostic{};
 const std::size_t animatedCandidateFirst=isMbTrack?mbCollisionWorld->candidateStorage().size():preparedCandidates.size();
 std::unique_ptr<gc_trigger_authoring50IN::Owner> gcTriggerAuthoring;
 std::unique_ptr<gc_trigger_actions50KG::Owner> gcTriggerActions;
 std::size_t gcTriggerCandidateFirst=std::numeric_limits<std::size_t>::max();
 if(isGcTrack){
  gcAnimatedWorld=std::make_unique<gc_animated_world50IG::Owner>();
  require50DU(gcAnimatedWorld->load(trackSession->archive,s.animationShared.libm,
      s.animationShared.motion,gcAnimationDiagnostic),
      gcAnimationDiagnostic.message.empty()?"original GC animated resource load failed":gcAnimationDiagnostic.message.c_str());
  gc_animated_candidates50IG::prepare(*gcAnimatedWorld,preparedCandidates,preparedBindings,
      query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
  // Keep the one authenticated TOB alive for its triggers and actual static
  // model overrides, including when trigger geometry is diagnostically off.
  gcTriggerAuthoring=std::make_unique<gc_trigger_authoring50IN::Owner>();std::string error;
  // Exact original tracks/agc.spl has195 nodes; require the live owner too
  // before binding any action destinations below. Twelve authored zones.
  const bool authored=gcTriggerAuthoring->load(trackSession->archive,195u,12u,error);
  require50DU(authored,error.empty()?"GC original trigger authoring rejected":error.c_str());
  if(gcNativeGate){
   gcTriggerActions=std::make_unique<gc_trigger_actions50KG::Owner>();
   gcTriggerCandidateFirst=gc_trigger_candidates50KG::prepare(*gcTriggerAuthoring,*gcTriggerActions,
       preparedCandidates,preparedBindings,query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
  }
  for(const auto& candidate:preparedCandidates)stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
  std::cerr<<"GC ANIMATED WORLD: original opening/windmill owners=2, vertices=1195, indices=2337, surfaces=6, "
      "COL1 children=4, CHARACTER native mask=1, source node mappings=1/2 and 0/1; opening inactive, windmill loop15s.\n";
 }
 if(isGaTrack){
  gaAnimatedWorld=std::make_unique<ga_animated_world::Owner>();
  const bool loaded=gaAnimatedWorld->load(trackSession->archive,s.animationShared.libm,
      s.animationShared.motion,gaAnimationDiagnostic);
  require50DU(loaded,gaAnimationDiagnostic.message.empty()?"original GA animated resource load failed":gaAnimationDiagnostic.message.c_str());
  ga_animated_candidates::prepare(*gaAnimatedWorld,preparedCandidates,preparedBindings,
      query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
  for(const auto& candidate:preparedCandidates)stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
  gaTriggerAuthoring=std::make_unique<ga_trigger_authoring::Owner>();std::string triggerError;
  // Exact authenticated tracks/aga.spl header: NumNodes107. Recheck against
  // the canonical live navigation owner before actions become ready below.
  const bool triggersLoaded=gaTriggerAuthoring->load(trackSession->archive,107u,12u,triggerError);
  require50DU(triggersLoaded,triggerError.empty()?"GA original trigger authoring failed":triggerError.c_str());
  gaTriggerActions=std::make_unique<ga_trigger_actions::Owner>();
  gaTriggerCandidateFirst=ga_trigger_candidates::prepare(*gaTriggerAuthoring,*gaTriggerActions,
      preparedCandidates,preparedBindings,query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
  std::cerr<<"GA ANIMATED WORLD: original Defensegate/opening,2 owners,605 vertices,6 surfaces,5 COL children; "
      "root-inclusive mappings1/2/3 and1/2, CHARACTER1, zones11/7, both authored inactive/nonlooping.\n";
 }
 if(isSaTrack){
  saAnimatedWorld=std::make_unique<sa_animated_world::Owner>();
  const bool loaded=saAnimatedWorld->load(trackSession->archive,s.animationShared.libm,
      s.animationShared.motion,saAnimationDiagnostic);
  require50DU(loaded,saAnimationDiagnostic.message.empty()?"original SA animated resources rejected":saAnimationDiagnostic.message.c_str());
  // All ten actual TOB animations use UseCollision=F. No collider registration
  // or fabricated static actor is added for these render/service owners.
  saTriggerAuthoring=std::make_unique<sa_trigger_authoring::Owner>();std::string error;
  const bool authored=saTriggerAuthoring->load(trackSession->archive,121u,25u,error);
  require50DU(authored,error.empty()?"SA original trigger authoring rejected":error.c_str());
  saTriggerActions=std::make_unique<sa_trigger_actions::Owner>();
  saTriggerCandidateFirst=sa_trigger_candidates::prepare(*saTriggerAuthoring,*saTriggerActions,
      preparedCandidates,preparedBindings,query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
 }
 if(isMaTrack){
  maTriggerAuthoring=std::make_unique<ma_trigger_authoring::Owner>();std::string error;
  const bool authored=maTriggerAuthoring->load(trackSession->archive,213u,9u,error);
  require50DU(authored,error.empty()?"MA original trigger authoring rejected":error.c_str());
  maTriggerActions=std::make_unique<ma_trigger_actions::Owner>();
  maTriggerCandidateFirst=ma_trigger_candidates::prepare(*maTriggerAuthoring,*maTriggerActions,
      preparedCandidates,preparedBindings,query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
 }
 if(isScTrack){
  scTriggerAuthoring=std::make_shared<sc_trigger_authoring::Owner>();std::string error;
  const bool decoded=scTriggerAuthoring->decode(scColdSources,error);
  require50DU(decoded,error.empty()?"SC same-source trigger authoring rejected":error.c_str());
  scTriggerActions=std::make_unique<sc_trigger_actions::Owner>();
  scTriggerCandidateFirst=sc_trigger_candidates::prepare(*scTriggerAuthoring,*scTriggerActions,
      preparedCandidates,preparedBindings,query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
  require50DU(scTriggerCandidateFirst==252&&preparedCandidates.size()==261,"SC actual COL/trigger closure changed");
 }
 if(isTcTrack){
  track_static_candidates50IG::Bindings names{};names.firstIdentity=0x10000000u;
  names.collisionTypeTag=query.collisionTypeTag;names.groupTypeTag=query.groupTypeTag;
  names.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  names.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  tc_collision_world50TC::Diagnostic diagnostic;
  const bool prepared=tcCollisionWorld->prepareStorage(originalMainApplication.entities(),
      tcTriggerActions,tcGlobalSound,names,trackStorageLimits,diagnostic);
  require50DU(prepared,diagnostic.message.empty()?"TC actual aggregate Storage rejected":diagnostic.message.c_str());
  tcWorldBorrow.emplace(tcCollisionWorld->borrow());
  require50DU(bool(*tcWorldBorrow)&&tcWorldBorrow->storage()&&
      tcWorldBorrow->candidateStorage().size()==91&&tcWorldBorrow->bindingStorage().size()==91&&
      ground.owners.size()==69&&trackSession->terrainOwners==69,"TC actual69 COL plus22 trigger owner closure");
  const bool indexed=tcNavigation.bindWorld(tcCollisionWorld->lease(),tcNavigationDiagnostic);
  require50DU(indexed,tcNavigationDiagnostic.message.empty()?"TC actual same-world navigation rejected":tcNavigationDiagnostic.message.c_str());
  for(const auto& candidate:tcWorldBorrow->candidateStorage())
   stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }
 if(isMcTrack){
  track_static_candidates50IG::Bindings names{};names.firstIdentity=0x10000000u;
  names.collisionTypeTag=query.collisionTypeTag;names.groupTypeTag=query.groupTypeTag;
  names.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  names.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  mc_collision_world50MC::Diagnostic diagnostic;
  const bool prepared=mcCollisionWorld->prepareStorage(originalMainApplication.entities(),
      mcTriggerAuthoring,mcTriggerActions,mcAnimationRuntime,names,trackStorageLimits,diagnostic);
  require50DU(prepared,diagnostic.message.empty()?"MC actual111 aggregate Storage rejected":diagnostic.message.c_str());
  require50DU(mcAnimationBinding->completePreparation(mcCollisionWorld,s.animationShared.motion,mcAnimationBindingDiagnostic),
      "MC actual fourteen-animation dual membership publication rejected");
  mcWorldBorrow.emplace(mcCollisionWorld->borrow());
  require50DU(bool(*mcWorldBorrow)&&mcWorldBorrow->registrationStorage()&&
      mcWorldBorrow->candidateStorage().size()==111&&ground.owners.size()==89,"MC complete89+21+1 closure rejected");
  require50DU(mcNavigation.bindWorld(mcCollisionWorld->lease(),mcNavigationDiagnostic),"MC actual same-world navigation rejected");
  for(const auto& candidate:mcWorldBorrow->candidateStorage())stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }else if(isGbTrack){
  track_static_candidates50IG::Bindings names{};names.firstIdentity=0x10000000u;
  names.collisionTypeTag=query.collisionTypeTag;names.groupTypeTag=query.groupTypeTag;
  names.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  names.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  gb_collision_world50GB::Diagnostic diagnostic;
  const bool prepared=gbCollisionWorld->prepareStorage(originalMainApplication.entities(),
      gbTriggerAuthoring,gbTriggerActions,gbAnimationRuntime,names,trackStorageLimits,diagnostic);
  require50DU(prepared,diagnostic.message.empty()?"GB actual637 aggregate Storage rejected":diagnostic.message.c_str());
  require50DU(gbAnimationBinding->completePreparation(gbCollisionWorld,s.animationShared.motion,gbAnimationBindingDiagnostic),
      "GB actual four-animation dual membership publication rejected");
  gbWorldBorrow.emplace(gbCollisionWorld->borrow());
  require50DU(bool(*gbWorldBorrow)&&gbWorldBorrow->registrationStorage()&&
      gbWorldBorrow->candidateStorage().size()==637&&ground.owners.size()==615,"GB complete615+18+4 closure rejected");
  require50DU(gbNavigation.bindWorld(gbCollisionWorld->lease(),gbNavigationDiagnostic),"GB actual same-world navigation rejected");
  for(const auto& candidate:gbWorldBorrow->candidateStorage())stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }
 if(isTbTrack){
  track_static_candidates50IG::Bindings names{};names.firstIdentity=0x10000000u;
  names.collisionTypeTag=query.collisionTypeTag;names.groupTypeTag=query.groupTypeTag;
  names.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  names.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  tb_collision_world50TB::Diagnostic diagnostic;
  const bool prepared=tbCollisionWorld->prepareStorage(originalMainApplication.entities(),
      tbTriggerAuthoring,tbTriggerActions,tbDoorRuntime,names,trackStorageLimits,diagnostic);
  require50DU(prepared,diagnostic.message.empty()?"TB actual aggregate Storage rejected":diagnostic.message.c_str());
  const bool completed=tbDoorBinding&&tbDoorBinding->completePreparation(tbCollisionWorld,s.animationShared.motion,tbDoorBindingDiagnostic);
  require50DU(completed&&tbDoorBinding->ready()&&tbDoorRuntime->ready(),
      tbDoorBindingDiagnostic.message.empty()?"TB actual door dual membership publication rejected":tbDoorBindingDiagnostic.message.c_str());
  tbWorldBorrow.emplace(tbCollisionWorld->borrow());
  require50DU(bool(*tbWorldBorrow)&&tbWorldBorrow->storage()&&
      tbWorldBorrow->candidateStorage().size()==337&&tbWorldBorrow->bindingStorage().size()==337&&
      ground.owners.size()==320&&trackSession->terrainOwners==320,"TB actual320 static plus16 trigger plus1 root door closure");
  require50DU(tbWorldBorrow->actions()==tbTriggerActions.get(),"TB actual shared aggregate action identity differs");
  const auto reservation=tbCollisionWorld->payload().navigationReservation;
  require50DU(reservation>=tb_navigation50TB::Owner::ownerBytes(),"TB final navigation reservation underflow");
  require50DU(reservation>=tb_navigation50TB::Owner::ownerBytes()+tb_navigation50TB::Owner::persistentPayloadBytes()+tb_navigation50TB::Owner::topologyScratchBytes(),
      "TB final navigation reservation incomplete");
  const bool indexed=tbNavigation.bindWorld(tbCollisionWorld->lease(),tbNavigationDiagnostic);
  require50DU(indexed,tbNavigationDiagnostic.message.empty()?"TB actual same-world navigation rejected":tbNavigationDiagnostic.message.c_str());
  for(const auto& candidate:tbWorldBorrow->candidateStorage())
   stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }
 if(isRcTrack){
  track_static_candidates50IG::Bindings names{};names.firstIdentity=0x10000000u;
  names.collisionTypeTag=query.collisionTypeTag;names.groupTypeTag=query.groupTypeTag;
  names.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  names.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  rc_collision_world50RC::Diagnostic diagnostic;
  const bool prepared=rcCollisionWorld->prepareStorage(originalMainApplication.entities(),
      rcTriggerAuthoring,names,trackStorageLimits,diagnostic);
  require50DU(prepared,diagnostic.message.empty()?"RC actual aggregate Storage rejected":diagnostic.message.c_str());
  rcWorldBorrow.emplace(rcCollisionWorld->borrow());
  require50DU(bool(*rcWorldBorrow)&&rcWorldBorrow->storage()&&
      rcWorldBorrow->candidateStorage().size()==123&&rcWorldBorrow->bindingStorage().size()==123&&
      ground.owners.size()==112&&trackSession->terrainOwners==112,"RC actual112 COL plus11 trigger closure");
  rcTriggerActions=rcWorldBorrow->actions();
  require50DU(rcTriggerActions,"RC actual aggregate action owner absent");
  const auto reservation=rcCollisionWorld->payload().navigationReservation;
  require50DU(reservation>=rc_navigation50RC::Owner::ownerBytes(),"RC final navigation reservation underflow");
  const auto reserved=reservation-rc_navigation50RC::Owner::ownerBytes();
  const bool indexed=rcNavigation.bindWorld(rcCollisionWorld->lease(),reserved,rcNavigationDiagnostic);
  require50DU(indexed,rcNavigationDiagnostic.message.empty()?"RC actual same-world navigation rejected":rcNavigationDiagnostic.message.c_str());
  for(const auto& candidate:rcWorldBorrow->candidateStorage())
   stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 }
 // This ledger extends the existing cold descriptor/source reservation only.
 // Ordinary visual mesh/image allocations retain their own established limits.
 const auto mcRemainingPayloadBytes=[&](){
  if(!isMcTrack)return std::size_t{0};
  const auto used=mcCollisionWorld->payload().totalRequested;
  require50DU(used<=trackStorageLimits.coldPayloadBytes&&
      mcAdditionalPayloadReserved<=trackStorageLimits.coldPayloadBytes-used,
      "MC additional cold payload accounting overflow");
  return trackStorageLimits.coldPayloadBytes-used-mcAdditionalPayloadReserved;
 };
 const auto reserveMcAdditionalPayload=[&](std::size_t bytes,const char* reason){
  require50DU(isMcTrack&&bytes<=mcRemainingPayloadBytes(),reason);
  mcAdditionalPayloadReserved+=bytes;
 };
 if(isMcTrack){
  mcWaterOwner=std::make_shared<mc_water50MC::Owner>();std::string error;
  require50DU(actualHeight.sourceBound&&actualHeight.sourceDomain.terrain330==UndertrackHeightOwner50EW::Absent&&
      actualHeight.sourceDomain.water334==UndertrackHeightOwner50EW::Present&&
      mcWaterOwner->prepare(mcColdSources,nativeWaterApplication.water,error),
      error.empty()?"MC actual water334 source/runtime preparation rejected":error.c_str());
  reserveMcAdditionalPayload(mcWaterOwner->retainedPayloadBytes(),"MC actual water runtime/resources exceed remaining payload");
 }
 // Retain the selected runtime provider domain independently of its authored
 // presence descriptor. This scope outlives the camera Session that borrows it.
 auto selectedRuntimeHeightDomain=actualHeight.sourceDomain;
 require50DU(!(mcWaterOwner&&nativeGcWater),"selected runtime water ownership is ambiguous");
 const native_water_runtime50JC::Owner* selectedRuntimeWater=nullptr;
 if(mcWaterOwner){require50DU(mcWaterOwner->ready(),"selected MC water owner is not ready");
  selectedRuntimeWater=&mcWaterOwner->runtime();}
 if(nativeGcWater){require50DU(nativeGcWater->prepared(),"selected GC water owner is not prepared");
  selectedRuntimeWater=&nativeGcWater->water();}
 if(selectedRuntimeWater)require50DU(selectedRuntimeWater->bindRuntimeHeightDomain(actualHeight.sourceDomain,
     terrainOwner.lift1F0,selectedRuntimeHeightDomain),
     "selected actual water owner cannot publish its runtime height domain");
 auto mcMapView=mcNavigation.map();
 auto mcAiView=mcNavigation.ai();
 auto mcMapMetadata=mcNavigation.mapMetadata();
 auto mcAiMetadata=mcNavigation.aiMetadata();
 require50DU(!isMcTrack||(mcWorldBorrow&&mcWorldBorrow->world()&&mcMapView.runtimeValid()&&mcAiView.runtimeValid()&&
     mcMapView.sameOwner(mcAiView)&&mcMapView.sameWorld(*mcWorldBorrow->world())&&mcAiView.sameWorld(*mcWorldBorrow->world())&&
     mcMapView.sourceIdentity()==mcColdSources.get()&&mcMapMetadata.matches(mcMapView)&&mcAiMetadata.matches(mcAiView)),
     "MC final typed navigation/world identity differs");
 const auto gbRemainingPayloadBytes=[&](){
  if(!isGbTrack)return std::size_t{0};
  const auto used=gbCollisionWorld->payload().totalRequested;
  require50DU(used<=trackStorageLimits.coldPayloadBytes&&
      gbAdditionalPayloadReserved<=trackStorageLimits.coldPayloadBytes-used,
      "GB additional cold payload accounting overflow");
  return trackStorageLimits.coldPayloadBytes-used-gbAdditionalPayloadReserved;
 };
 const auto reserveGbAdditionalPayload=[&](std::size_t bytes,const char* reason){
  require50DU(isGbTrack&&bytes<=gbRemainingPayloadBytes(),reason);
  gbAdditionalPayloadReserved+=bytes;
 };
 auto gbMapView=gbNavigation.map();
 auto gbAiView=gbNavigation.ai();
 auto gbMapMetadata=gbNavigation.mapMetadata();
 auto gbAiMetadata=gbNavigation.aiMetadata();
 require50DU(!isGbTrack||(gbWorldBorrow&&gbWorldBorrow->world()&&gbMapView.runtimeValid()&&gbAiView.runtimeValid()&&
     gbMapView.sameOwner(gbAiView)&&gbMapView.sameWorld(*gbWorldBorrow->world())&&gbAiView.sameWorld(*gbWorldBorrow->world())&&
     gbMapView.sourceIdentity()==gbColdSources.get()&&gbMapMetadata.matches(gbMapView)&&gbAiMetadata.matches(gbAiView)),
     "GB final typed navigation/world identity differs");
 std::size_t tbAdditionalPayloadReserved=0;
 const auto tbRemainingPayloadBytes=[&](){
  if(!isTbTrack)return std::size_t{0};
  const auto used=tbCollisionWorld->payload().totalRequested;
  require50DU(used<=trackStorageLimits.coldPayloadBytes&&
      tbAdditionalPayloadReserved<=trackStorageLimits.coldPayloadBytes-used,
      "TB additional cold payload accounting overflow");
  return trackStorageLimits.coldPayloadBytes-used-tbAdditionalPayloadReserved;
 };
 const auto reserveTbAdditionalPayload=[&](std::size_t bytes,const char* reason){
  require50DU(isTbTrack&&bytes<=tbRemainingPayloadBytes(),reason);
  tbAdditionalPayloadReserved+=bytes;
 };
 const auto tbMapView=tbNavigation.map();
 const auto tbAiView=tbNavigation.ai();
 const auto tbMapMetadata=tbNavigation.mapMetadata();
 const auto tbAiMetadata=tbNavigation.aiMetadata();
 require50DU(!isTbTrack||(tbWorldBorrow&&tbWorldBorrow->world()&&tbMapView.runtimeValid()&&tbAiView.runtimeValid()&&
     tbMapView.sameOwner(tbAiView)&&tbMapView.sameWorld(*tbWorldBorrow->world())&&tbAiView.sameWorld(*tbWorldBorrow->world())&&
     tbMapView.sourceIdentity()==tbColdSources.get()&&tbMapMetadata.matches(tbMapView)&&tbAiMetadata.matches(tbAiView)),
     "TB final typed navigation/world identity differs");
 std::size_t rcAdditionalPayloadReserved=0;
 const auto rcRemainingPayloadBytes=[&](){
  if(!isRcTrack)return std::size_t{0};
  const auto used=rcCollisionWorld->payload().totalRequested;
  require50DU(used<=trackStorageLimits.coldPayloadBytes&&
      rcAdditionalPayloadReserved<=trackStorageLimits.coldPayloadBytes-used,
      "RC additional cold payload accounting overflow");
  return trackStorageLimits.coldPayloadBytes-used-rcAdditionalPayloadReserved;
 };
 const auto reserveRcAdditionalPayload=[&](std::size_t bytes,const char* reason){
  require50DU(isRcTrack&&bytes<=rcRemainingPayloadBytes(),reason);
  rcAdditionalPayloadReserved+=bytes;
 };
 const auto rcNavigationBorrow=rcNavigation.borrow();
 require50DU(!isRcTrack||(rcNavigationBorrow.valid()&&rcWorldBorrow&&
     rcWorldBorrow->world()&&rcNavigationBorrow.sameWorld(*rcWorldBorrow->world())&&
     rcNavigationBorrow.sourceIdentity()==rcColdSources.get()),"RC final navigation/world borrow mismatch");
 if(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack)legacyGround.audit=J{};
 std::span<Type2StaticCandidate49X> candidates=isMcTrack?mcWorldBorrow->candidateStorage():isGbTrack?gbWorldBorrow->candidateStorage():isTbTrack?tbWorldBorrow->candidateStorage():isRcTrack?rcWorldBorrow->candidateStorage():isTcTrack?tcWorldBorrow->candidateStorage():isMbTrack?mbCollisionWorld->candidateStorage():std::span<Type2StaticCandidate49X>(preparedCandidates);
 native_wreck_debris50PD::sim().track=candidates; // debris segment collision (borrowed for the race)
 std::span<const track_runtime_storage50IG::Binding> trackBindings=isMcTrack?mcWorldBorrow->bindingStorage():isGbTrack?gbWorldBorrow->bindingStorage():isTbTrack?tbWorldBorrow->bindingStorage():isRcTrack?rcWorldBorrow->bindingStorage():isTcTrack?tcWorldBorrow->bindingStorage():isMbTrack?mbCollisionWorld->bindingStorage():std::span<const track_runtime_storage50IG::Binding>(preparedBindings);
 coldLoadDiagnostics.begin("track_storage");
 // Cold, immovable, count-derived storage. Candidate order and every borrowed
 // source mesh/radius remain owned above; publication checks complete coverage.
 // Original 1AA23C follows the geometry\ buoy/finish-line load; stand-in: after static collision.
 rr::platform::loading_progress50DU::step(.1f,0x1aa23c);
 rr::platform::loading_progress50DU::pulse("Preparing track storage");
 track_runtime_storage50IG::Owner trackStorageOwner;
 track_runtime_storage50IG::Diagnostic trackStorageDiagnostic{};
 if(isMbTrack){
  const bool storageReady=mbCollisionWorld->prepareStorage(originalMainApplication.entities(),mbStorageLimits,trackStorageDiagnostic);
  require50DU(storageReady,trackStorageDiagnostic.message.empty()?"MB actual world storage rejected":trackStorageDiagnostic.message.c_str());
  require50DU(trackStorageDiagnostic.requestedPayloadBytes<=mbStorageLimits.coldPayloadBytes&&
      mbNavigationPayload<=trackStorageLimits.coldPayloadBytes-trackStorageDiagnostic.requestedPayloadBytes,
      "MB combined navigation/storage native payload exceeded existing allowance");
  const bool indexed=mbNavigation.bindWorld(mbCollisionWorld->lease(),mbNavigationDiagnostic);
  require50DU(indexed,mbNavigationDiagnostic.message.empty()?"MB same-world navigation indices rejected":mbNavigationDiagnostic.message.c_str());
  require50DU(mbCollisionWorld->attachTriggers(),"MB authentic three-trigger memberships rejected");
 }
 if(!isRaTrack&&!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack)require50DU(trackStorageOwner.prepare(originalMainApplication.entities(),ground,trackBindings.data(),trackBindings.size(),trackStorageLimits,
     trackStorageDiagnostic),trackStorageDiagnostic.message.empty()?"original track storage preparation failed":
     trackStorageDiagnostic.message.c_str());
 auto& trackStorage=isMcTrack?*mcWorldBorrow->storage():isGbTrack?*gbWorldBorrow->storage():isTbTrack?*tbWorldBorrow->storage():isRcTrack?*rcWorldBorrow->storage():isTcTrack?*tcWorldBorrow->storage():isMbTrack?*mbCollisionWorld->storage():isRaTrack?raRuntime->storage:*trackStorageOwner.get();
 const auto& trackCounts=trackStorage.count;
 const auto tcMapView=tcNavigation.map();
 const auto tcAiView=tcNavigation.ai();
 const auto tcMapMetadata=tcNavigation.mapMetadata();
 const auto tcAiMetadata=tcNavigation.aiMetadata();
 const auto mbMapView=mbNavigation.map();
 const auto mbAiView=mbNavigation.ai();
 const auto mbMapMetadata=mbNavigation.mapMetadata();
 const auto mbAiMetadata=mbNavigation.aiMetadata();
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,gc_animated_world50IG::ownerCount> gcAnimatedMembership;
 if(gcAnimatedWorld)for(auto& membership:gcAnimatedMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,ga_animated_world::ownerCount> gaAnimatedMembership;
 if(gaAnimatedWorld)for(auto& membership:gaAnimatedMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,ga_trigger_authoring::count> gaTriggerMembership;
 if(gaAnimatedWorld)for(auto& membership:gaTriggerMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,sc_trigger_authoring::count> scTriggerMembership;
 if(scTriggerAuthoring)for(auto& membership:scTriggerMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,ma_trigger_authoring::count> maTriggerMembership;
 if(maTriggerAuthoring)for(auto& membership:maTriggerMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,sa_trigger_authoring::count> saTriggerMembership;
 if(saTriggerAuthoring)for(auto& membership:saTriggerMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,gc_trigger_authoring50IN::count> gcTriggerMembership;
 if(gcTriggerActions)for(auto& membership:gcTriggerMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 // Original 1AA27C (after 1AA250, before anim\); stand-in: after track storage.
 rr::platform::loading_progress50DU::step(.1f,0x1aa27c);
 rr::platform::loading_progress50DU::pulse("Preparing physics");
 coldLoadDiagnostics.begin("physics_navigation_setup");
 const auto terrainOwnerCount=trackCounts.terrain;
 auto* audioContactMaterials=trackStorage.contactMaterials.data();
 require50DU(setLocalMatrix49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.main190,pose)&&refreshSceneWorld49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.main190),"supplied initial pose");f::project(s,x.bindings);
 auto secondaryPose=pose;for(unsigned i=0;i<3;++i){volatile float a=pose.v[i]*x.secondaryInputs.targetRelative2F0.v[0];volatile float b=pose.v[4+i]*x.secondaryInputs.targetRelative2F0.v[1];volatile float c=pose.v[8+i]*x.secondaryInputs.targetRelative2F0.v[2];volatile float ab=a+b;volatile float abc=ab+c;volatile float position=pose.v[12+i]+abc;secondaryPose.v[12+i]=position;}
 require50DU(setLocalMatrix49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.physical19C,secondaryPose)&&refreshSceneWorld49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.physical19C),"supplied secondary initial pose");
 require50DU(setLocalMatrix49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.model1B4,secondaryPose),"supplied initial displayed pose");f::project(s,x.bindings);
 ResetBoundNode50BA resetModelNodes[18]{};ResetBoundsWorkspace50BA resetBoundsWorkspace{};ModelBoundsResult49O resetModelBounds{};
 require50DU(x.assets.hierarchy[2].nodeCount==18,"actual chariot source node count");
 for(unsigned i=0;i<18;++i){rr::assets::PsgNode49O authoredNode{};require50DU(rr::assets::readPsgNode49O(x.assets.hierarchy[2],i,authoredNode),"actual reset model bound");const auto index=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+static_cast<std::int32_t>(i);resetModelNodes[i].node=static_cast<SceneNodeIndex49P>(index);const auto expectedParent=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+authoredNode.parent;require50DU(static_cast<std::int32_t>(s.nodes[index].parent180)==expectedParent,"source flat model parent mapping");for(unsigned j=0;j<3;++j){resetModelNodes[i].center140.v[j]=authoredNode.bounds[j];resetModelNodes[i].halfExtent150.v[j]=authoredNode.bounds[3+j];}}
 const auto beforeResetBounds=::allocations;
 if(!refreshResetModelBounds50BA({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.model1B4,resetModelNodes,18,resetBoundsWorkspace,resetModelBounds)){std::cerr<<"reset root flags "<<unsigned(s.nodes[static_cast<int>(x.bindings.lifecycle.model1B4)].sceneFlags190)<<" descendants";for(const auto&n:resetModelNodes)std::cerr<<' '<<unsigned(s.nodes[static_cast<int>(n.node)].sceneFlags190);std::cerr<<'\n';require50DU(false,"reset model cache admission");}
 require50DU(::allocations==beforeResetBounds,"reset model bounds no allocation");
 PodSceneRebaseWorkspace50FB rebaseWorkspace{};
 ModelBoundsResult49O rebaseBounds[5]{};PodSceneRebaseDiagnostic50FB rebaseDiagnostic{};
 if(!rebaseColdPodScene50FB({s.nodes,s.sceneNodeCount},rebaseModels,rebaseModelCount,rebaseWorkspace,rebaseBounds,rebaseDiagnostic)){
  std::cerr<<"cold pod rebase error "<<unsigned(rebaseDiagnostic.error)
           <<" model "<<rebaseDiagnostic.model<<" node "<<rebaseDiagnostic.sourceNode<<'\n';
  require50DU(false,"source first-base pod scene rebase failed");
 }


 SecondaryActivationGlobals49V resetGlobals{};initializedSecondaryActivationGlobals49V(resetGlobals);resetMainRigidBody50AR(m,s.bodies.mainSphere,s.mainSupplement,resetGlobals);resetSecondaryActivation49V(s.bodies.secondary,s.secondarySupplement,resetGlobals);
 for(auto*body:{&m.components.root,&s.bodies.secondary.root}){require50DU(matrixToQuaternion48G(body->scene,body->body.orientation),"prepared initial orientation");std::memcpy(body->body.position.v,body->scene.v+12,12);}
 ResetLifecycleGlobals50AT resetLifecycle{};initializedResetLifecycleGlobals50AT(resetLifecycle);require50DU(resetLifecycleRecords50AT(s.bodies.vehicle838,s.lifecycle,resetLifecycle),"source reset lifecycle records");s.fields.applicationByte58=2;m.controls.player.flagsB9C=0x80;
 SpringGeneratedParameters48K settings{};settings.impact54=true;settings.friction50=true;settings.impact.disableLeverArms5C=true;settings.impact.correctPenetration64=true;settings.impact.normalForceScale98=.97f;settings.impact.positionCorrectionScaleCC=.2f;
 SpringResponseParameters48L response{};response.maxSpeed94=10000;rr::assets::SpringCollisionAuthoring50F collision{};rr::assets::PodPhysicsDiagnostic49N diagnostic{};
 require50DU(rr::assets::readSpringCollision50F(reinterpret_cast<const char*>(phy.data()),phy.size(),collision,diagnostic)&&rr::assets::applySpringCollision50F(collision,x.parameters.bodies.main.body.mass,settings,response),"authored response settings");
 const auto tuning=[&](){
  if(isMcTrack){
   rr::assets::DecodeError48C error;std::vector<std::uint8_t> bytes;
   const auto& pin=mc_cold_sources50LA::sources[4];
   const bool extracted=mcColdSources->extractMember(pin.name,bytes,error,pin.bytes);
   require50DU(extracted&&bytes.size()==pin.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==pin.sha,
       "MC retained original TUN member unavailable");return bytes;
  }else if(isGbTrack){
   rr::assets::DecodeError48C error;std::vector<std::uint8_t> bytes;
   const auto& pin=gb_cold_sources50LA::sources[4];
   const bool extracted=gbColdSources->extractMember(pin.name,bytes,error,pin.bytes);
   require50DU(extracted&&bytes.size()==pin.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==pin.sha,
       "GB retained original TUN member unavailable");return bytes;
  }
  if(isTbTrack){
   rr::assets::DecodeError48C error;std::vector<std::uint8_t> bytes;
   const auto& pin=tb_cold_sources50LA::sources[4];
   const bool extracted=tbColdSources->extractMember(pin.name,bytes,error,pin.bytes);
   require50DU(extracted&&bytes.size()==pin.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==pin.sha,
       "TB retained original TUN member unavailable");return bytes;
  }
  if(isRcTrack){
   rr::assets::DecodeError48C error;std::vector<std::uint8_t> bytes;
   const auto& pin=rc_cold_sources50LA::sources[4];
   const bool extracted=rcColdSources->extractMember(pin.name,bytes,error,pin.bytes);
   require50DU(extracted&&bytes.size()==pin.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==pin.sha,
       "RC retained original TUN member unavailable");return bytes;
  }
  if(isTcTrack){
   rr::assets::DecodeError48C error;std::vector<std::uint8_t> bytes;
   const auto& pin=tc_cold_sources50LA::sources[4];
   const bool extracted=rr::assets::extractResMember48C(tcColdSources->archive(),pin.name,bytes,error,pin.bytes);
   require50DU(extracted&&bytes.size()==pin.bytes&&pod_materials50ed_fixture::detail::sha256(bytes)==pin.sha,
       "TC retained original TUN member unavailable");return bytes;
  }
  if(isScTrack)return scColdSources->sourcesDecoded()->memberBytes[4];
  if(isRaTrack)return raRuntime->source.memberBytes[3];
  if(!trackSession->tuningInArchive)return runtime_startup50fl::readBytes(trackSession->impactTuning);
  rr::assets::ResArchive48C source;rr::assets::DecodeError48C error;std::vector<std::uint8_t> bytes;
  require50DU(rr::assets::decodeResource48C(runtime_startup50fl::readBytes(trackSession->archive),source,error)&&
      rr::assets::extractResMember48C(source,trackSession->impactTuning,bytes,error),
      "actual selected original track tuning unavailable");return bytes;
 }();require50DU(rr::assets::readEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),tuning.size(),x.parameters.impactTuning,diagnostic),"actual selected-track impact tuning");x.parameters.impactTuningBound=true;
 if(originalAiEnabled){
  require50DU(rr::assets::readAIEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),x.parameters.impactTuning,diagnostic),"original AI environment tuning unavailable");
  if(isMcTrack)originalAiTuning=mcColdSources->aiTuning();
  else if(isGbTrack)originalAiTuning=gbColdSources->aiTuning();
  else if(isTbTrack)originalAiTuning=tbColdSources->aiTuning();
  else if(isRcTrack)originalAiTuning=rcColdSources->aiTuning();
  else if(isTcTrack)originalAiTuning=tcColdSources->aiTuning();
  else if(isScTrack)originalAiTuning=scColdSources->sourcesDecoded()->aiTuning;
  else require50DU(rr::assets::readAiTrackTuning50IU(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),originalAiTuning,originalAiAuthoringDiagnostic),"original AI track tuning unavailable");
 }
 if(dynamicContactEnabled){
  require50DU(rr::assets::readDynamicImpactAuthoring50IW(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),dynamicTuning,diagnostic),"original dynamic contact tuning unavailable");
  require50DU(rr::assets::readFinalImpactAuthoring49Y(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),dynamicFinalTuning,diagnostic),"original final impact tuning unavailable");
 }
 rr::assets::DefenseAuthoring49Y defense{};require50DU(rr::assets::readDefenseAuthoring49Y(reinterpret_cast<const char*>(phy.data()),phy.size(),defense,diagnostic)&&defense.baseDefense.present,"explicit base-profile defense");s.fields.defenseCF0=defense.baseDefense.value;
 // Profile overwrite and authored class selection are separate source gates.
 // Use the same UseValues/class decision as the already-bound physics.
 rr::assets::RepairAuthoring50A repairAuthoring{};
 require50DU(!selection.applyProfileOverwrite&&rr::assets::readRepairAuthoring50A(
     reinterpret_cast<const char*>(phy.data()),phy.size(),repairAuthoring,diagnostic),
     "explicit no-profile repair authoring unavailable");
 using RepairKey=rr::assets::RepairAuthoringKey50A;
 float currentRepair=0.f;
 if(authored.usedAuthoredClassValues){
  RepairKey key=RepairKey::Count;
  switch(authored.selectedClass){
   case AuthoredClass49N::A:key=RepairKey::RepairA;break;
   case AuthoredClass49N::B:key=RepairKey::RepairB;break;
   case AuthoredClass49N::C:key=RepairKey::RepairC;break;
  }
  require50DU(key!=RepairKey::Count,"unknown authored repair class");
  const auto& amount=repairAuthoring.values[static_cast<unsigned>(key)];
  require50DU(amount.present,"selected authored repair class missing");
  currentRepair=amount.value;
 }else{
  const auto& minimum=repairAuthoring.values[static_cast<unsigned>(RepairKey::MinRepair)];
  const auto& maximum=repairAuthoring.values[static_cast<unsigned>(RepairKey::MaxRepair)];
  const auto& baseAbility=repairAuthoring.values[static_cast<unsigned>(RepairKey::BaseAbilityRepair)];
  const auto& maxAbility=repairAuthoring.values[static_cast<unsigned>(RepairKey::MaxAbilityRepair)];
  require50DU(minimum.present&&maximum.present&&baseAbility.present&&maxAbility.present&&
      baseAbility.value==1.0f&&maxAbility.value==1.0f,
      "interpolated repair requires retained equal-unit ability endpoints");
  volatile float difference=maxAbility.value-baseAbility.value;
  volatile float delta=0.0f*difference;
  volatile float ability=baseAbility.value+delta;
  volatile float range=maximum.value-minimum.value;
  volatile float scaled=ability*range;
  currentRepair=minimum.value+scaled;
 }
 require50DU(std::isfinite(currentRepair)&&currentRepair>0.f,"authored repair amount unavailable");
 s.fields.repairCF4=currentRepair;
 BoostParameters50FK boostParameters{};float boostCoolingCF8=0.f;
 require50DU(boost50fk_fixture::bind(reinterpret_cast<const char*>(phy.data()),phy.size(),
     selection,authored,boostParameters,boostCoolingCF8),"actual boost authoring unavailable");
 BoostState50FK boostState{};BoostDiagnostic50FK boostDiagnostic{};
 if(!playerUsesRosterRig)std::cerr<<"ORIGINAL HANDLING: slot=0 track="<<authoredTrackName
     <<" class="<<static_cast<unsigned>(authored.selectedClass)
     <<" class_values="<<authored.usedAuthoredClassValues
     <<" max_thrust="<<x.parameters.bodies.main.engine.maxThrust
     <<" top_speed_body="<<x.parameters.bodies.main.engine.theoreticalTopSpeed
     <<" steering100="<<x.parameters.bodies.main.aero.steering100
     <<" repair="<<s.fields.repairCF4<<" cooling="<<boostCoolingCF8<<'\n';

 // Preserve the actual fresh constructor records before the player's probe
 // is enabled. Fresh opponent slots install these cold records before enabling
 // their own authored probe; no already-running state is reset this way.
 require50DU(s.phase==VehicleFramePhase49W::Ready&&m.frameFactor==0.f&&
     !s.terrainProbe.enabled&&!s.bodies.main.domain.probe100,
     "source pre-enable terrain constructor state unavailable");
 const auto freshTerrainProbe=s.terrainProbe;
 const auto freshMainProbe=s.bodies.main.components.probe348;
 const auto freshProbe100=s.bodies.main.domain.probe100;
 auto raFreshProbe=isRaTrack?ra_cold_roster50LA::FreshProbe::capture(s):nullptr;
 require50DU(enableVehicleTerrain50FP(s,x.bindings,terrainOwner.initial,
     terrainOwner.follow,x.workspace,x.diagnostic),"enable source-authored terrain probe");

 StaticSpringState50G spring{};spring.records.generatedCapacity88=40;SpringRecord48N records[60]{},queryRecords[60]{},scratchRecords[60]{};

 const J*secondaryCase=nullptr;cold::Shape secondaryShape{};
 CollisionFilter48J secondaryFilter{};Type2StaticQuery49X secondaryQuery{};
 if(directOriginalInputs){
  original_disc_query_bootstrap50LE::initialize(originalBootstrapCollision.secondary(),secondaryFilter,secondaryQuery,true);
 }else{
  for(const auto& item:corpus["cases"])if(item["name"]==trackSession->secondaryCollisionCase)secondaryCase=&item;
  require50DU(secondaryCase,"actual chariot query present");const auto& sqj=(*secondaryCase)["query"];
  secondaryShape=cold::shape(corpus["shapes"][sqj["shape"].get<std::size_t>()]);secondaryFilter=cold::filter(sqj["filter"]);
  secondaryQuery.resource=&secondaryShape.view;secondaryQuery.filter=&secondaryFilter;secondaryQuery.queryScene.value=78;
  secondaryQuery.normalMode54=0u;secondaryQuery.normalMode54Bound=true;
  secondaryQuery.collisionTypeTag=sqj["collision_tag"];secondaryQuery.groupTypeTag=sqj["group_tag"];secondaryQuery.callbackEnabled=sqj["callback"];
  cold::words(secondaryQuery.copied40.data(),sqj["copied40"]);cold::words(secondaryQuery.copied50.data(),sqj["copied50"]);
  secondaryQuery.skipMask=(*secondaryCase)["masks"][0];secondaryQuery.dynamicMask=(*secondaryCase)["masks"][1];secondaryQuery.materialMask=(*secondaryCase)["masks"][2];
 }
 const auto& secondaryQueryResource=*secondaryQuery.resource;
 // All these actual authored groups use the same declared native shape5/6
 // backend. MA/SA were already registered, so omitting them here rejected
 // their genuine trigger owners before any geometric/action dispatch.
 query.nativeGatePolicy=secondaryQuery.nativeGatePolicy=gcNativeGate||isGaTrack||isMaSaTrack||isRaTrack||isMbTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack;
 if(gcAnimatedWorld)require50DU(gc_animated_candidates50IG::admitNonBodyResponse(*gcAnimatedWorld,
     candidates.data(),candidates.size(),animatedCandidateFirst,query,secondaryQuery),
     "GC original CHARACTER non-body spring response routing differs");
 if(gaAnimatedWorld)require50DU(ga_animated_candidates::admitNonBodyResponse(*gaAnimatedWorld,
     candidates.data(),candidates.size(),animatedCandidateFirst,query,secondaryQuery),
     "GA original CHARACTER non-body spring response routing differs");
 if(!sourceStaticCandidates)require50DU((*secondaryCase)["order"]==(*chosen)["order"]&&(*secondaryCase)["candidates"]==(*chosen)["candidates"],"same explicit ordered terrain inputs");
 StaticSpringState50G secondarySpring{};secondarySpring.records.generatedCapacity88=60;
 SpringRecord48N secondaryRecords[60]{},secondaryQueryRecords[60]{},secondaryScratchRecords[60]{};
 if(raceEnabled)require50DU(matchesEndpointPolicy(secondaryQueryResource),
     "retained secondary endpoint-W host policy differs");
 secondaryPairCapacity=std::max(secondaryPairCapacity,secondaryQueryResource.rayCount);
 std::size_t targetDepth=0;
 for(const auto& candidate:candidates)targetDepth=std::max(targetDepth,candidate.maxDepth);
 for(std::size_t racer=0;racer<racerCollisions.size();++racer)if(racerCollisions[racer]){
  coldRacerStage50DU(racerRigs[racer]?racerRigs[racer]->podId:playerPodId,"collision scratch preparation",[&](){
   native_racer_collision50HW::ScratchRequirements required{};
   require50DU(racerCollisions[racer]->requirements(targetDepth,required),"original query scratch extent rejected");
   stackCount=std::max({stackCount,required.mainStack,required.secondaryStack});
  });
 }
 // Original 1A6DEC: after 1A9EE0 returns and 114770/1A95B0; stand-in: end of world/collision.
 rr::platform::loading_progress50DU::step(.1f,0x1a6dec);
 // This complete lexical suffix owns query/navigation/audio/input resources.
 // It retires synchronously before the unchanged early cold owners below unwind.
 interactive_setup50DU::Context setup50DU{
  .actualHeight=actualHeight,
  .actualWorld=actualWorld,
  .allAiControllerSamples=allAiControllerSamples,
  .allAiEnabled=allAiEnabled,
  .allAiHostDriverCalls=allAiHostDriverCalls,
  .allAiKeyboardPackets=allAiKeyboardPackets,
  .animatedCandidateFirst=animatedCandidateFirst,
  .applicationClock=applicationClock,
  .audioContactMaterials=audioContactMaterials,
  .authored=authored,
  .authoredFinishSteering10A4=authoredFinishSteering10A4,
  .authoredTrackName=authoredTrackName,
  .boostCoolingCF8=boostCoolingCF8,
  .boostDiagnostic=boostDiagnostic,
  .boostParameters=boostParameters,
  .boostState=boostState,
  .cameraStartup350=cameraStartup350,
  .candidates=candidates,
  .captureFirstWrongWay=captureFirstWrongWay,
  .capturePath=capturePath,
  .captureRun=captureRun,
  .capturedFirstWrongWay=capturedFirstWrongWay,
  .checkpointInput=checkpointInput,
  .checkpointOutput=checkpointOutput,
  .coldLoadDiagnostics=coldLoadDiagnostics,
  .collision=collision,
  .collisionNormalMode=collisionNormalMode,
  .consumedGaStage=consumedGaStage,
  .consumedGcStage=consumedGcStage,
  .consumedMaStage=consumedMaStage,
  .consumedRaStage=consumedRaStage,
  .consumedSaStage=consumedSaStage,
  .consumedTaStage=consumedTaStage,
  .controlPresets=controlPresets,
  .courseColdLoad=courseColdLoad,
  .courseRaceCold=courseRaceCold,
  .courseRaceLaunched=courseRaceLaunched,
  .deviceIndex=deviceIndex,
  .directOriginalHuman=directOriginalHuman,
  .directOriginalInputs=directOriginalInputs,
  .displayWallTotals=displayWallTotals,
  .dynamicActorResolver=dynamicActorResolver,
  .dynamicActors=dynamicActors,
  .dynamicConsumerFlags11050=dynamicConsumerFlags11050,
  .dynamicContactEnabled=dynamicContactEnabled,
  .dynamicDamageCursor=dynamicDamageCursor,
  .dynamicEventBindings=dynamicEventBindings,
  .dynamicEventCursor=dynamicEventCursor,
  .dynamicEvents=dynamicEvents,
  .dynamicFinalTuning=dynamicFinalTuning,
  .dynamicLapCursor=dynamicLapCursor,
  .dynamicMusicCursor=dynamicMusicCursor,
  .dynamicMusicProfile=dynamicMusicProfile,
  .dynamicPeers=dynamicPeers,
  .dynamicPodEffect=dynamicPodEffect,
  .dynamicProfile=dynamicProfile,
  .dynamicRacer60=dynamicRacer60,
  .dynamicRegistry=dynamicRegistry,
  .dynamicSelfSlot=dynamicSelfSlot,
  .dynamicTuning=dynamicTuning,
  .freshMainProbe=freshMainProbe,
  .freshProbe100=freshProbe100,
  .freshTerrainProbe=freshTerrainProbe,
  .gaAnimatedMembership=gaAnimatedMembership,
  .gaAnimatedWorld=gaAnimatedWorld,
  .gaAnimationDiagnostic=gaAnimationDiagnostic,
  .gaOriginalDisc=gaOriginalDisc,
  .gaOriginalWorld=gaOriginalWorld,
  .gaTriggerActions=gaTriggerActions,
  .gaTriggerAuthoring=gaTriggerAuthoring,
  .gaTriggerCandidateFirst=gaTriggerCandidateFirst,
  .gaTriggerMembership=gaTriggerMembership,
  .gbAiMetadata=gbAiMetadata,
  .gbAiView=gbAiView,
  .gbAnimationBinding=gbAnimationBinding,
  .gbAnimationBindingDiagnostic=gbAnimationBindingDiagnostic,
  .gbAnimationRuntime=gbAnimationRuntime,
  .gbColdSources=gbColdSources,
  .gbCollisionWorld=gbCollisionWorld,
  .gbFogOwner=gbFogOwner,
  .gbLightOwner=gbLightOwner,
  .gbLiveAudioPrepared=gbLiveAudioPrepared,
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
  .gbPreparedAudioRoot=gbPreparedAudioRoot,
  .gbQueryBinding=gbQueryBinding,
  .gbRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{gbRemainingPayloadBytes},
  .gbSoundMode=gbSoundMode,
  .gbTrackParticles=gbTrackParticles,
  .gbTriggerActions=gbTriggerActions,
  .gbTriggerAuthoring=gbTriggerAuthoring,
  .gbWorldBorrow=gbWorldBorrow,
  .gcAnimatedMembership=gcAnimatedMembership,
  .gcAnimatedWorld=gcAnimatedWorld,
  .gcAnimationDiagnostic=gcAnimationDiagnostic,
  .gcNativeGate=gcNativeGate,
  .gcNativeWorldSelected=gcNativeWorldSelected,
  .gcOriginalDisc=gcOriginalDisc,
  .gcOriginalWorld=gcOriginalWorld,
  .gcTriggerActions=gcTriggerActions,
  .gcTriggerAuthoring=gcTriggerAuthoring,
  .gcTriggerCandidateFirst=gcTriggerCandidateFirst,
  .gcTriggerMembership=gcTriggerMembership,
  .ground=ground,
  .groundStack=groundStack,
  .headlessLive=headlessLive,
  .host=host,
  .hostDiagnostic=hostDiagnostic,
  .humanControlInput=humanControlInput,
  .humanCourseEnabled=humanCourseEnabled,
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
  .lastCommittedWrongWay=lastCommittedWrongWay,
  .legacyGround=legacyGround,
  .liveAudio=liveAudio,
  .loadedAudioSettings=loadedAudioSettings,
  .loadedTrack=loadedTrack,
  .m=m,
  .maOriginalDisc=maOriginalDisc,
  .maOriginalWorld=maOriginalWorld,
  .maTriggerActions=maTriggerActions,
  .maTriggerAuthoring=maTriggerAuthoring,
  .maTriggerCandidateFirst=maTriggerCandidateFirst,
  .maTriggerMembership=maTriggerMembership,
  .mainPairCapacity=mainPairCapacity,
  .managerRuntimeSelection=managerRuntimeSelection,
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
  .mcParticleWindow=mcParticleWindow,
  .mcQueryBinding=mcQueryBinding,
  .mcRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{mcRemainingPayloadBytes},
  .mcTrackParticles=mcTrackParticles,
  .mcTriggerActions=mcTriggerActions,
  .mcWaterOwner=mcWaterOwner,
  .mcWaterUpdateSequence=mcWaterUpdateSequence,
  .mcWorldBorrow=mcWorldBorrow,
  .mode1Enabled=mode1Enabled,
  .mode1Requested=mode1Requested,
  .monotonicNow50DU=monotonicNow50DU,
  .movement=movement,
  .nativeFireSerial=nativeFireSerial,
  .nativeGaSelection=nativeGaSelection,
  .nativeGbSelection=nativeGbSelection,
  .nativeGcWater=nativeGcWater,
  .nativeMaSelection=nativeMaSelection,
  .nativeMbSelection=nativeMbSelection,
  .nativeMcSelection=nativeMcSelection,
  .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
  .nativeOriginalFleet=nativeOriginalFleet,
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
  .openNativeVideoCall=interactive_frame50DU::BorrowedCall<void()>{openNativeVideo},
  .originalAi=originalAi,
  .originalAiAuthoringDiagnostic=originalAiAuthoringDiagnostic,
  .originalAiCreateSelection=originalAiCreateSelection,
  .originalAiEnabled=originalAiEnabled,
  .originalAiHumanCount=originalAiHumanCount,
  .originalAiMapMetadata=originalAiMapMetadata,
  .originalAiMetadata=originalAiMetadata,
  .originalAiOpponentCount=originalAiOpponentCount,
  .originalAiOwnsSlotCall=interactive_frame50DU::BorrowedCall<bool(std::size_t) noexcept>{originalAiOwnsSlot},
  .originalAiReceipt=originalAiReceipt,
  .originalAiSession=originalAiSession,
  .originalAiSettings1125C=originalAiSettings1125C,
  .originalAiTuning=originalAiTuning,
  .originalCameraEnabled=originalCameraEnabled,
  .originalMainApplication=originalMainApplication,
  .originalRecoveryEnabled=originalRecoveryEnabled,
  .originalUiActive=originalUiActive,
  .originalUiStarted=originalUiStarted,
  .phaseTiming=phaseTiming,
  .playerActionIndexFFC=playerActionIndexFFC,
  .playerArchive=playerArchive,
  .playerPodId=playerPodId,
  .playerRacer60=playerRacer60,
  .playerSource5C=playerSource5C,
  .playerUsesRosterRig=playerUsesRosterRig,
  .pose=pose,
  .qfilter=qfilter,
  .query=query,
  .queryRecords=queryRecords,
  .raDiagnostic=raDiagnostic,
  .raFreshProbe=raFreshProbe,
  .raRuntime=raRuntime,
  .raWorld=raWorld,
  .raceEnabled=raceEnabled,
  .raceOpponentCount=raceOpponentCount,
  .raceRacerCount=raceRacerCount,
  .racerCables=racerCables,
  .racerCollisions=racerCollisions,
  .racerContactsEnabled=racerContactsEnabled,
  .racerHudNames=racerHudNames,
  .racerNames=racerNames,
  .racerRigs=racerRigs,
  .racerVisuals=racerVisuals,
  .rcColdSources=rcColdSources,
  .rcCollisionWorld=rcCollisionWorld,
  .rcLightOwner=rcLightOwner,
  .rcNavigationBorrow=rcNavigationBorrow,
  .rcOriginalDisc=rcOriginalDisc,
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
  .recoveryGlobals=recoveryGlobals,
  .recoveryQueryOwners=recoveryQueryOwners,
  .renderDiagnostics=renderDiagnostics,
  .reportHost50DU=reportHost50DU,
  .reportRenderer50DU=reportRenderer50DU,
  .requestedWindowSamples=requestedWindowSamples,
  .reserveGbAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveGbAdditionalPayload},
  .reserveMcAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveMcAdditionalPayload},
  .reserveRcAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveRcAdditionalPayload},
  .reserveTbAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveTbAdditionalPayload},
  .resetGlobals=resetGlobals,
  .resetLifecycle=resetLifecycle,
  .response=response,
  .s=s,
  .saAnimatedWorld=saAnimatedWorld,
  .saAnimationDiagnostic=saAnimationDiagnostic,
  .saOriginalDisc=saOriginalDisc,
  .saOriginalWorld=saOriginalWorld,
  .saTriggerActions=saTriggerActions,
  .saTriggerAuthoring=saTriggerAuthoring,
  .saTriggerCandidateFirst=saTriggerCandidateFirst,
  .saTriggerMembership=saTriggerMembership,
  .scColdSources=scColdSources,
  .scGround=scGround,
  .scOriginalDisc=scOriginalDisc,
  .scTriggerActions=scTriggerActions,
  .scTriggerAuthoring=scTriggerAuthoring,
  .scTriggerCandidateFirst=scTriggerCandidateFirst,
  .scTriggerMembership=scTriggerMembership,
  .scratchRecords=scratchRecords,
  .secondaryFilter=secondaryFilter,
  .secondaryPairCapacity=secondaryPairCapacity,
  .secondaryQuery=secondaryQuery,
  .secondaryQueryRecords=secondaryQueryRecords,
  .secondaryQueryResource=secondaryQueryResource,
  .secondaryRecords=secondaryRecords,
  .secondaryScratchRecords=secondaryScratchRecords,
  .secondarySpring=secondarySpring,
  .selectedAudioSettings=selectedAudioSettings,
  .selectedOriginal=selectedOriginal,
  .selectedRacer60=selectedRacer60,
  .selectedRuntimeHeightDomain=selectedRuntimeHeightDomain,
  .selectedTrack=selectedTrack,
  .selection=selection,
  .sessionIndex=sessionIndex,
  .sessionRandom=sessionRandom,
  .settings=settings,
  .sourceConsumedIntroActivation=sourceConsumedIntroActivation,
  .sourceConsumedPauseActivation=sourceConsumedPauseActivation,
  .sourceDisplayLifetime=sourceDisplayLifetime,
  .sourceModelRenderGlobals=sourceModelRenderGlobals,
  .sourcePause=sourcePause,
  .sourcePauseObservation=sourcePauseObservation,
  .sourcePauseParentAttached=sourcePauseParentAttached,
  .sourcePauseTrack=sourcePauseTrack,
  .sourcePositionStatsPrestage=sourcePositionStatsPrestage,
  .sourcePositionStatsPrestageDiagnostic=sourcePositionStatsPrestageDiagnostic,
  .sourceStaticCandidates=sourceStaticCandidates,
  .spring=spring,
  .stackCount=stackCount,
  .startAtMenu=startAtMenu,
  .taOriginalDisc=taOriginalDisc,
  .taOriginalWorld=taOriginalWorld,
  .tbAiMetadata=tbAiMetadata,
  .tbAiView=tbAiView,
  .tbColdSources=tbColdSources,
  .tbCollisionWorld=tbCollisionWorld,
  .tbDoorBinding=tbDoorBinding,
  .tbDoorBindingDiagnostic=tbDoorBindingDiagnostic,
  .tbDoorDiagnostic=tbDoorDiagnostic,
  .tbDoorPrefixArmed=tbDoorPrefixGuard.armed,
  .tbDoorRuntime=tbDoorRuntime,
  .tbLightOwner=tbLightOwner,
  .tbLiveAudioPrepared=tbLiveAudioPrepared,
  .tbMapMetadata=tbMapMetadata,
  .tbMapView=tbMapView,
  .tbOriginalDisc=tbOriginalDisc,
  .tbPodSound=tbPodSound,
  .tbPreparedAudioRoot=tbPreparedAudioRoot,
  .tbRemainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{tbRemainingPayloadBytes},
  .tbSoundMode=tbSoundMode,
  .tbTriggerActions=tbTriggerActions,
  .tbTriggerAuthoring=tbTriggerAuthoring,
  .tbWorldBorrow=tbWorldBorrow,
  .tcAiMetadata=tcAiMetadata,
  .tcAiView=tcAiView,
  .tcColdSources=tcColdSources,
  .tcCollisionWorld=tcCollisionWorld,
  .tcGateDiagnostic=tcGateDiagnostic,
  .tcGatePrefixArmed=tcGatePrefixGuard.armed,
  .tcGateRuntime=tcGateRuntime,
  .tcGateVisibility=tcGateVisibility,
  .tcGateVisibilityDiagnostic=tcGateVisibilityDiagnostic,
  .tcGlobalSound=tcGlobalSound,
  .tcLightOwner=tcLightOwner,
  .tcLiveAudioPrepared=tcLiveAudioPrepared,
  .tcMapMetadata=tcMapMetadata,
  .tcMapView=tcMapView,
  .tcOriginalDisc=tcOriginalDisc,
  .tcTriggerActions=tcTriggerActions,
  .tcWorldBorrow=tcWorldBorrow,
  .terrainBindings=terrainBindings,
  .terrainOwner=terrainOwner,
  .terrainOwnerCount=terrainOwnerCount,
  .tournamentLaunchSelection=tournamentLaunchSelection,
  .tournamentLaunchStamp=tournamentLaunchStamp,
  .tournamentResultArmed=tournamentResultArmed,
  .tournamentResultCommitted=tournamentResultCommitted,
  .trackAdmission=trackAdmission,
  .trackBindings=trackBindings,
  .trackCounts=trackCounts,
  .trackSession=trackSession,
  .trackStorage=trackStorage,
  .tuning=tuning,
  .usePhysicalGameController=usePhysicalGameController,
  .x=x,
 };
 return setup50DU.run();
}
}

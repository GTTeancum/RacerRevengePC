// Exact original suffix: world owners retire before the original early prefix.
// Common rewrite (I2): the ONE cold load for every course and every pod. The race
// (1A65C0) and its World (1A6BC0) are built by the one consumed stage, the track
// (1A6D80 -> 1A9EE0) by the one load into that World, then the navigation (08), the
// roster (10) and the one collision world's storage. Courses differ only in the files
// the Track read; nothing below names a course or a pod (PLAN 2.1, 2.2, B-C01..B-C18).
#include "native_last_failure50pd.h"
#include "../../src/game/native_surface_traction50st.h"
#include "../../src/game/native_zero_gravity50zg.h"
#include "../../src/game/n64_track_hazards_runtime50nh.h"
#include "native_wreck_debris_sim50pd.h"
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "terrain_interactive_world50du.h"
#include "terrain_interactive_roster_cold50du.h"
#include "terrain_interactive_setup50du.h"
#include "terrain_interactive_world_dependencies50du.h"
#include "terrain_interactive_loop_types50du.h"
#include "track_collision_world50cr.h"
#include "course_trigger_actions50ct.h"
#include "runtime_allocation_counter50fl.h"
#include "type2_fixture_io50i_declarations.h"
#include "native_ai_session50iv_fixture.h"
#include "../../src/game/original_human_finish50kg.h"
#include "player_loading_session50du.h"
#include "original_racer_manager_entity50la.h"
#include "original_subtype3_record_owner50la.h"
#include "original_record_grid_pose50la.h"
#include "../../src/game/dlc_course_entries50dl.h"
namespace cold=cold50i;
namespace interactive_world50DU {
namespace f=runtime_startup50fl;
namespace recovery50FI=checkpoint_recovery50fi_fixture;
namespace ct=common_track50CR;
namespace clog=common_race_log50CR;
using namespace rr::game;
using interactive_frame50DU::BorrowedCall;
using interactive_loop50DU::GameplayDisplayWallTotals50DU;
using J=nlohmann::json;
namespace {
// The one counted-race rule: the original builds the race 1A65C0 for every launch
// (Single Race 11258 1, Practice 2, Time Trial 3), on every course; the end dispatch
// (common_race_end50CR, 1A7DD0 -> 1AB520 -> 1ABBC0/1ABC1C/1AB5BC) branches on 11258.
bool countedRaceLaunch(std::uint32_t launch11258) noexcept {return launch11258>=1u&&launch11258<=3u;}
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
// A refused common step: one common_load line (first refusal of the run), then the
// existing require_failed throw so the session unwinds exactly as before.
void refuseStep50DU(clog::Step step,const ct::Diagnostic& d,const char* fallback) {
    const std::string reason=d.reason.empty()?std::string(fallback):d.reason;
    clog::load(step,false,d.value,reason,nullptr,d.pc);
    require50DU(false,reason.c_str());
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
 // The one material provider's extra voices (R7: 192..219, one range for every course).
 static_assert(material_audio50HI::extraFirstVoice+material_audio50HI::maximumExtraVoices<=live_audio50HF::voiceCount);
 static_assert(material_audio50HI::extraFirstVoice+material_audio50HI::maximumExtraVoices<=
     rr::platform::originalMaterialFirstVoice50HD);
 static_assert(rr::platform::originalMaterialFirstVoice50HD+8u*44u==rr::platform::originalSpeechFirstVoice50HD);
 static_assert(rr::platform::originalSpeechFirstVoice50HD+8u*47u==rr::platform::originalTaIntroFirstVoice50HD);
 static_assert(rr::platform::originalTaIntroFirstVoice50HD+rr::platform::originalTaIntroVoices50HD==live_audio50HF::voiceCount);
 auto& track=this->track;
 auto& roster=this->roster;
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
 auto& rootedCourseRace=this->rootedCourseRace;
 auto& courseRaceLaunched=this->courseRaceLaunched;
 auto& deviceIndex=this->deviceIndex;
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
 auto& liveAudioPrepared=this->liveAudioPrepared;
 auto& preparedAudioRoot=this->preparedAudioRoot;
 auto& soundMode=this->soundMode;
 auto& headlessLive=this->headlessLive;
 auto& host=this->host;
 auto& hostDiagnostic=this->hostDiagnostic;
 auto& humanControlInput=this->humanControlInput;
 auto& humanCourseEnabled=this->humanCourseEnabled;
 auto& lapContinuity=this->lapContinuity;
 auto& lastCommittedWrongWay=this->lastCommittedWrongWay;
 auto& liveAudio=this->liveAudio;
 auto& loadedAudioSettings=this->loadedAudioSettings;
 auto& loadedTrack=this->loadedTrack;
 auto& m=this->m;
 auto& mainQueryResource=this->mainQueryResource;
 auto& managerRuntimeSelection=this->managerRuntimeSelection;
 auto& meshes=this->meshes;
 auto& mode1Enabled=this->mode1Enabled;
 auto& mode1Requested=this->mode1Requested;
 auto& monotonicNow50DU=*this->monotonicNow50DU;
 auto& names=this->names;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
 auto& nativeRacerManager=this->nativeRacerManager;
 auto& nativeRecordActor=this->nativeRecordActor;
 auto& nativeSelectedLights=this->nativeSelectedLights;
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
 auto& qfilter=this->qfilter;
 auto& query=this->query;
 auto& raceEnabled=this->raceEnabled;
 auto& raceOpponentCount=this->raceOpponentCount;
 auto& raceRacerCount=this->raceRacerCount;
 auto& racerContactsEnabled=this->racerContactsEnabled;
 auto& renderDiagnostics=this->renderDiagnostics;
 auto& reportHost50DU=*this->reportHost50DU;
 auto& reportRenderer50DU=*this->reportRenderer50DU;
 auto& requestedWindowSamples=this->requestedWindowSamples;
 auto& s=this->s;
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
 auto& tournamentLaunchSelection=this->tournamentLaunchSelection;
 auto& tournamentLaunchStamp=this->tournamentLaunchStamp;
 auto& tournamentResultArmed=this->tournamentResultArmed;
 auto& tournamentResultCommitted=this->tournamentResultCommitted;
 auto& trackAdmission=this->trackAdmission;
 auto& trackSession=this->trackSession;
 auto& usePhysicalGameController=this->usePhysicalGameController;
 auto& x=this->x;
 // The race's one Track: the session built it with only its identity (code, source,
 // file names); the stage begins on it and the 1A9EE0 load fills it (W0 request i1:
 // trackOwner is the owning handle of `track`).
 const auto& trackOwner=this->trackOwner;
 require50DU(trackOwner&&trackOwner.get()==&track&&!track.loaded()&&track.identity.code.valid(),
     "the race's one track identity is unavailable");
 const auto courseCode=track.code();
 original_consumed_stage50DU::Owner consumedStage(originalMainApplication,*liveAudio);
 common_lap50CR::Tracker laps;
 original_pause_cold50LA::Owner sourcePause;
 original_pause_observation50LA::Owner sourcePauseObservation;
 bool sourcePauseParentAttached=false;
 // Permanent original-resource owners precede the snapshot bank and all draw
 // publications. Only their immutable borrowed views enter the live arena.
 original_position_stats_prestage50LA::Owner sourcePositionStatsPrestage;
 original_position_stats_prestage50LA::Diagnostic sourcePositionStatsPrestageDiagnostic;
 // The race's per-slot rig/collision and runtime (visual, cable, model) arrays: every race.
 auto rosterOwners=interactive_roster_cold50DU::makeSharedOwners();
 auto& racerRigs=rosterOwners->rigs;
 auto& racerCollisions=rosterOwners->collisions;
 auto runtimeOwners=interactive_roster_cold50DU::makeRuntimeOwners();
 auto& racerVisuals=runtimeOwners->visuals;
 auto& racerCables=runtimeOwners->cables;
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
 // The selected racer's own pod PHY (B-P04: the development Pod01.phy input is gone;
 // every slot reads its own pod's files).
 const auto& phy=x.assets.phy;
 require50DU(!phy.empty(),"selected racer PHY unavailable");
 // A1 (original 214954..214AA0): the initial PHY class comes from the loaded track's
 // lowercase two-letter code, with the source selector's sb/ra/sc exceptions. This is
 // the original's own course-string compare (kept, PLAN 1.1 A1).
 const auto sourceTrackLetter=[](char c){return c>='A'&&c<='Z'?char(c+'a'-'A'):c;};
 const char authoredTrackName[]={sourceTrackLetter(courseCode[0]),sourceTrackLetter(courseCode[1]),'\0'};
 AuthoredClass49N trackClass=AuthoredClass49N::A;
 require50DU(selectAuthoredClass49N(authoredTrackName,2,trackClass),
     "loaded original track's authored handling class unavailable");
 // PORT EXTENSION, NOT ORIGINAL (A12, DLC add-on course): a registry code takes the
 // phy_class of its registry row; the code-letter rule above answers for stock codes.
 if(char addonClass=0;rr::game::dlc50DL::addonHandlingClass(authoredTrackName,addonClass))
     trackClass=addonClass=='A'?AuthoredClass49N::A:addonClass=='B'?AuthoredClass49N::B:AuthoredClass49N::C;
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

 // ---- Steps 01-02: the one race (1A65C0) and its one World (1A6BC0), every course.
 // The launch is the rooted original Course entry (I1); its 11258 subtype decides
 // only the end dispatch, never whether the race exists.
 require50DU(consumedLaunch&&originalCameraEnabled&&rootedCourseRace&&countedRaceLaunch(nativeOrdinaryLaunch11258)&&
     (dynamicProfile.game34==2u||dynamicProfile.game34==0u),
     "the counted race requires the rooted original Course launch (11258 1..3, game34 0 or 2)");
 openNativeVideo();
 originalMainApplication.appStart(originalMainApplication.assetRoot);
 originalMainApplication.enterGameplayMaterialDirectory17D7A0();
 const auto materialContext=nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication);
 const auto materialDomain=originalMainApplication.standardMaterialDomain();
 require50DU(materialContext&&materialDomain&&materialDomain->prepared(),"counted race material context unavailable");
 if(!sourceModelRenderGlobals)sourceModelRenderGlobals=
     std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
 {
  rr::assets::OriginalStandardMaterialDiagnostic50LA d;std::uint32_t pc=0;
  const bool ready=consumedStage.begin(*consumedLaunch,sessionIndex+1,playerRacer60,
      *sourceModelRenderGlobals,trackOwner,d,pc);
  if(!ready)std::cerr<<"consumed_prefix_rejected course="<<courseCode<<" source_pc="<<std::hex<<pc<<std::dec
      <<" material_pc="<<d.sourcePC<<" material_status="<<static_cast<unsigned>(d.status)<<'\n';
  require50DU(ready,"consumed race/world constructor prefix rejected");
 }
 const auto& race=consumedStage.race();
 require50DU(race&&race->world74()&&race->world74()->track==trackOwner,"the race's one World does not hold the race's track");

 // The shared live audio owner before the AUB reader (05.12) binds its material and
 // positional voices to its mixer/groups (the same owner/settings as every later join).
 const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
 const std::filesystem::path audioRoot=suppliedAudioRoot?std::filesystem::path(suppliedAudioRoot):
     runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO");
 if(!liveAudioPrepared){
  std::string error;
  const bool audioPrepared=liveAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),
      audioRoot,error,playerRacer60,loadedAudioSettings);
  require50DU(audioPrepared,error.empty()?"shared audio preparation rejected":error.c_str());
  liveAudioPrepared=true;preparedAudioRoot=audioRoot;
 }
 // Step 11 (1AF330, 2C2B50, 2C4B30): the speech banks are loaded by the shared live
 // audio prepare above (once per process; a retained prepare keeps its banks).
 clog::load(clog::Step::speech1AF330,liveAudioPrepared,liveAudioPrepared?1u:0u,
     liveAudioPrepared?std::string_view{}:std::string_view{"live audio not prepared"});
 auto* audioMixer=liveAudio->mixerForMaterialOwner();auto* audioGroups=liveAudio->groupsForProviders();
 require50DU(audioMixer&&audioGroups&&audioGroups->uses(*audioMixer),"shared mixer/groups unavailable");
 // 1101C: the Application's one sound-mode owner (application lifetime, set from the profile
 // at start) is on the mixer before 05.12, because the AUB material and positional voices bind
 // to it. The original has it from application start; the port binds it here if no earlier
 // audio join did, and keeps it as the session's mode so the later audio join reuses it.
 if(!audioMixer->originalSoundMode50KG()){
  if(!soundMode)soundMode=originalMainApplication.soundMode;
  if(!soundMode){
   auto mode=std::make_shared<rr::platform::OriginalSoundMode50KG>();
   require50DU(mode->initializeFromProfile1101C(1),"race stereo-mode construction (1101C) rejected");
   soundMode=std::move(mode);
  }
  require50DU(audioMixer->bindOriginalSoundMode50KG(soundMode),
      "sound-mode binding before the AUB reader rejected");
 }

 // ---- Steps 04-05: the one track load (1A6D80 -> 1A9EE0) into the race's World:
 // archive, SCB, TOB, physics index, collision ground, fog, particles, instances,
 // materials, triggers, water (A9), sky, AUB, lights, animation, tail.
 rr::platform::loading_progress50DU::pulse("Loading track");
 coldLoadDiagnostics.begin("track_load");
 {
  const ct::LoadInputs inputs{
   .application=&originalMainApplication,
   .race=race.get(),
   .world=race->world74(),
   .entities=originalMainApplication.entities(),
   .materials382B38=materialDomain->materials382B38(),
   .mixer=audioMixer,
   .audioGroups=audioGroups,
   .racers=raceRacerCount,
   .generation=sessionIndex+1,
   .audioRoot=audioRoot.string(),
   .animationShared=&s.animationShared,   // W0 request r5 #1
  };
  ct::Diagnostic d;
  // The load logs its own common_load line per step and stops at the first refusal.
  const bool loaded=originalMainApplication.loadConsumedRaceTrack1A9EE0(*race,*trackOwner,track.identity,inputs,d);
  require50DU(loaded,d.reason.empty()?"track load 1A9EE0 refused":d.reason.c_str());
 }
 require50DU(track.loaded()&&track.roles.collision&&track.roles.collision->ground(),
     "track load finished without its collision ground");
 // Original 1AA1B8: after 1A9EE0 loads tracks\<track> (world, objects, lights).
 rr::platform::loading_progress50DU::step(.1f,0x1aa1b8);
 rr::platform::loading_progress50DU::pulse("Preparing terrain");
 coldLoadDiagnostics.begin("ground_query");

 // ---- Step 08: both splines (2A1160/2A1260), node counts from the files.
 {
  ct::Diagnostic d;
  const bool read=ct::readNavigation2A1260(*trackOwner,s.animationShared.libm,d); // logs its own common_load step=08
  require50DU(read,d.reason.empty()?"navigation 2A1260 refused":d.reason.c_str());
 }
 const auto trackCollision=track.roles.collision;
 // 2A2880 grid input on the one collision world: the map spline's first node and the
 // selected racer's own PHY lift (session row grid rank, data).
 {
  std::string error;
  require50DU(trackCollision->placeGrid(ct::navigationView(track,ct::NavigationRole::map),trackSession->gridRank,phy,error),
      error.empty()?"grid placement on the track's collision ground rejected":error.c_str());
 }
 // The one trigger action owner (R5, W0 request r5 #2): it needs the AI spline (08)
 // and the field (10) before it binds; created here, bound in the cold contacts.
 if(track.roles.triggerSource&&track.roles.triggers)
  trackOwner->roles.triggerActions=std::make_shared<course_trigger_actions50CT::Owner>();

 // ---- Step 10: the roster 1AA510 -> 1AD520 -> 1AE280 (humans 1ADF80, AI 1AE3CC ->
 // 2129C0, wreck chain 165E90), then each slot's own collision owner.
 if(raceEnabled)interactive_roster_cold50DU::Context{
  .mainQueryResource=mainQueryResource,
  .directOriginalInputs=directOriginalInputs,
  .playerRacer60=playerRacer60,
  .rootedCourseRace=rootedCourseRace,
  .sourceUiHumanCourse=originalUiStarted&&!originalUiActive&&humanCourseEnabled,
  .courseProfile=originalMainApplication.riderProfile.get(),
  .raceRacerCount=raceRacerCount,
  .track=track,
  .roster=roster,
  .racerRigs=racerRigs,
  .racerCollisions=racerCollisions,
  .endpointPolicy=endpointPolicy,
  .racerNames=racerNames,
  .mainPairCapacity=mainPairCapacity,
  .secondaryPairCapacity=secondaryPairCapacity,
  .matchesEndpointPolicy=interactive_frame50DU::BorrowedCall<bool(const rr::game::Type2ResourceView49X&)>(matchesEndpointPolicy),
 }.run();
 {
  std::array<std::uint32_t,common_roster50CR::maxSlots> pods{};
  for(std::size_t slot=0;slot<roster.count()&&slot<pods.size();++slot)pods[slot]=roster.row(slot)->racer60;
  clog::raceBegin({.run=sessionIndex+1,.course=courseCode,.source=track.source(),.mode34=dynamicProfile.game34,
      .subtype11258=nativeOrdinaryLaunch11258,.p34=managerRuntimeSelection.mode34,.humans34=roster.humans34(),
      .field=static_cast<std::uint32_t>(roster.count()),.pods=pods.data(),.podCount=roster.count()});
 }

 // ---- The one collision world's storage (B-C11): static candidates of every placed
 // COL owner, one candidate per live trigger, the track-animation lane, and every
 // racer's query rows (shared layout), sized from those counts.
 rr::platform::loading_progress50DU::pulse("Preparing collision");
 coldLoadDiagnostics.begin("static_candidates");
 require50DU(sourceStaticCandidates,"every course takes its static candidates from the track's collision world");
 track_runtime_storage50IG::Limits trackStorageLimits;trackStorageLimits.originalCameraQueries=originalCameraEnabled;
 trackStorageLimits.originalRecoveryQueries=originalRecoveryEnabled;
 std::array<const native_racer_collision50HW::Owner*,track_runtime_storage50IG::Storage::maximumRoster> trackRosterOwners{};
 track_runtime_storage50IG::RosterPlan trackRosterPlan{};
 if(raceEnabled&&originalRecoveryEnabled){
  require50DU(raceRacerCount&&raceRacerCount<=trackRosterOwners.size()&&raceRacerCount<=racerCollisions.size(),
      "shared roster plan exceeds the storage roster ceiling");
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   require50DU(bool(racerCollisions[racer]),"shared roster plan requires every racer's collision owner");
   trackRosterOwners[racer]=racerCollisions[racer].get();
  }
  trackRosterPlan={trackRosterOwners.data(),raceRacerCount};
  trackStorageLimits.roster=&trackRosterPlan;
 }
 // ONE common track-animation owner (05.14, read from the course's own TOB; absent
 // [Animations] = none). Its visual lane joins the one lighting owner, its collision
 // rows the one collision world (track_animation_binding50an.h).
 std::shared_ptr<track_animation_runtime50AN::Owner> trackAnimations=track.roles.animation;
 track_animation_runtime50AN::Diagnostic trackAnimationDiagnostic{};
 std::unique_ptr<track_animation_binding50AN::Binding> trackAnimationBinding;
 track_animation_binding50AN::Diagnostic trackAnimationBindingDiagnostic{};
 struct TrackAnimationPrefixGuard {
  decltype(trackAnimationBinding)& binding;track_animation_binding50AN::Diagnostic& diagnostic;bool armed=true;
  ~TrackAnimationPrefixGuard()noexcept{if(armed&&binding&&binding->needsClose()&&!binding->close(diagnostic)){
   std::fputs("track animation reached registration retirement rejected; backing retained\n",stderr);std::terminate();
  }}
 } trackAnimationPrefixGuard{trackAnimationBinding,trackAnimationBindingDiagnostic};
 if(trackAnimations){
  require50DU(bool(track.roles.lights),"track animation visual lane requires the track's lighting owner");
  trackAnimationBinding=std::make_unique<track_animation_binding50AN::Binding>();
  require50DU(trackAnimationBinding->beginVisual(trackAnimations,track.roles.lights,trackAnimationBindingDiagnostic),
      trackAnimationBindingDiagnostic.message.empty()?"track animation visual prefix rejected":trackAnimationBindingDiagnostic.message.c_str());
 }
 {
  // The one trigger action owner's source half (274E90 rows over the one group,
  // LostAI targets against the step-08 AI spline's node count) is prepared before
  // the collision world takes it: prepareStorage reads its live rows and bounds.
  // Its destinations (PodSound voice, Sound, Particles, LostAI Reset) are bound in
  // the cold contacts once their owners exist, before the racer identities.
  if(const auto& actions=track.roles.triggerActions){
   const ct::NavigationView splineAi=ct::navigationView(track,ct::NavigationRole::ai);
   require50DU(actions->prepareSource(track.roles.triggerSource,track.roles.triggers,
       static_cast<std::uint32_t>(splineAi.count()),trackAnimations),"trigger action source preparation rejected");
  }
  track_collision_world50CR::StorageRequest request{};
  request.application=originalMainApplication.entities();
  // Same pointer as Track::roles.triggerActions (W0 round 2, I2 item 2): the
  // collision world holds it and closes it at retirement.
  request.actions=track.roles.triggerActions;
  request.animations=trackAnimations;
  request.names.firstIdentity=0x10000000u; // Explicit collision-object native namespace.
  request.names.collisionTypeTag=query.collisionTypeTag;request.names.groupTypeTag=query.groupTypeTag;
  request.names.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  request.names.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  request.limits=trackStorageLimits;
  track_collision_world50CR::Diagnostic d;
  const bool prepared=trackCollision->prepareStorage(std::move(request),d);
  if(!prepared){
   ct::Diagnostic refused;refused.pc=0x271230u;refused.value=d.payload.totalRequested;
   refused.reason=d.message.empty()?"collision world storage rejected":d.message;
   refuseStep50DU(clog::Step::collision271230,refused,"collision world storage rejected");
  }
 }
 // Animation memberships publish exactly when the TOB authors animations.
 require50DU(bool(trackAnimationBinding)==bool(trackAnimations)&&
     (!trackAnimationBinding||trackAnimationBinding->completePreparation(trackCollision,trackAnimationBindingDiagnostic)),
     trackAnimationBindingDiagnostic.message.empty()?"track animation visual/collision publication rejected":trackAnimationBindingDiagnostic.message.c_str());
 require50DU(track_collision_world50CR::publishStorageRole(*trackOwner)&&track.roles.storage,
     "the collision world's storage role was not published");

 // ---- Step 08 (zone index): 1ACF70 against the one collision world's live world.
 {
  ct::Diagnostic d;
  const bool bound=ct::bindNavigation1ACF70(*trackOwner,d); // logs its own common_load step=08
  require50DU(bound,d.reason.empty()?"navigation 1ACF70 zone index refused":d.reason.c_str());
 }
 const ct::NavigationView mapView=ct::navigationView(track,ct::NavigationRole::map);
 const ct::NavigationView aiView=ct::navigationView(track,ct::NavigationRole::ai);
 require50DU(mapView.runtimeValid()&&aiView.runtimeValid()&&mapView.sameOwner(aiView),
     "the track's map/AI navigation views are not runtime-valid on one store");
 {
  common_lap50CR::Diagnostic d;
  require50DU(laps.prepare(mapView,aiView,raceRacerCount,d),d.reason?d.reason:"lap tracker 2A1980 preparation refused");
 }
 // One common_track_data line per role (receipts for a stock course, none for an add-on).
 track.logTrackData();

 const auto* groundScene=trackCollision->ground();
 require50DU(groundScene,"the track's collision ground is unavailable");
 const auto& ground=*groundScene;
 const auto& actualWorld=track.documents.world;
 const auto& actualHeight=ground.height;
 if(originalAiEnabled){
  require50DU(actualHeight.sourceBound&&actualHeight.descriptor.bound,
      "original AI track330/334 source ownership unavailable");
  std::cerr<<"ORIGINAL AI height owners: terrain330="<<static_cast<unsigned>(actualHeight.sourceDomain.terrain330)
      <<" water334="<<static_cast<unsigned>(actualHeight.sourceDomain.water334)
      <<" source_bound="<<actualHeight.sourceBound
      <<" water_native_query="<<actualHeight.waterNativeQueryBound<<'\n';
 }
 std::vector<std::uint32_t> groundStack(ground.stackCount);
 std::uint64_t waterUpdateSequence=0;
 std::array<std::unique_ptr<native_gc_water_racer50KG::Owner>,raceRacerCapacity> nativeWaterRacers;
 auto& nativeFireSerial=originalUiStarted?originalMainApplication.state->fireSerial433970:
     nativeWaterApplication.directFireSerial433970;
 // A9: the original branches on whether the track's data names water (274570 none /
 // 2745D4 water factory); the 05.10 reader built roles.water exactly then.
 require50DU(track.hasWater()==bool(track.roles.water)&&
     (actualHeight.sourceDomain.water334==UndertrackHeightOwner50EW::Present)==track.hasWater(),
     "the track's water owner differs from its water data");
 const native_water_runtime50JC::Owner* selectedRuntimeWater=track.roles.water.get();
 // Retain the selected runtime provider domain independently of its authored
 // presence descriptor. This scope outlives the camera Session that borrows it.
 auto selectedRuntimeHeightDomain=actualHeight.sourceDomain;
 if(selectedRuntimeWater)require50DU(selectedRuntimeWater->bindRuntimeHeightDomain(actualHeight.sourceDomain,
     terrainOwner.lift1F0,selectedRuntimeHeightDomain),
     "the track's water owner cannot publish its runtime height domain");
 const auto groundAllocations=::allocations;
 // One placement rule for every course (B-C18): the grid pose from the ground probe.
 BodyMatrix48G pose=ground.pose;
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
 require50DU(::allocations==groundAllocations,"actual ground query without allocations");

 // The collision world's candidates, bindings and storage (one Storage for every course).
 std::span<Type2StaticCandidate49X> candidates;
 std::span<const track_runtime_storage50IG::Binding> trackBindings;
 std::size_t triggerCount=0;
 {
  auto borrow=trackCollision->borrow();
  require50DU(bool(borrow)&&borrow.storage()&&borrow.registrationStorage()&&borrow.world(),
      "the track's collision world is not geometry-ready");
  candidates=borrow.candidateStorage();trackBindings=borrow.bindingStorage();triggerCount=borrow.triggerCount();
  require50DU(candidates.size()==trackCollision->candidateCount()&&trackBindings.size()==candidates.size()&&
      ground.owners.size()==trackCollision->sourceCount(),
      "the collision world must cover every placed owner, live trigger and animation row");
 }
 for(const auto& candidate:candidates)stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+candidate.maxDepth);
 // Candidate order of the one collision world (GB aggregate shape): placed COL owners,
 // then one candidate per live trigger (274E90), then the animation lane.
 const std::size_t triggerCandidateFirst=ground.owners.size();
 const std::size_t animatedCandidateFirst=triggerCandidateFirst+triggerCount;
 // The contexts' one trigger-membership store, sized by the collision world from the
 // live trigger count (BlockId::triggerMemberships, bounds-checked).
 auto& triggerMembership=trackCollision->triggerMemberships();
 require50DU(triggerMembership.sized()&&triggerMembership.size()==triggerCount,
     "trigger membership store differs from the live trigger count");
 // Animation memberships live in the collision world's lane (no second set here).
 std::vector<std::unique_ptr<track_runtime_storage50IG::Membership>> trackAnimationMembership;

 native_wreck_debris50PD::sim().track=candidates; // debris segment collision (borrowed for the race)
 // F5 wreck E0 detectors use the same static-query type tags and 3E0728/
 // 3E0718/3E0720 masks as the live pod query (one-time filter allocation).
 native_wreck_debris50PD::f5::bindWorld(query.collisionTypeTag,query.groupTypeTag,query.skipMask,query.dynamicMask,query.materialMask);
 {const bool e8=native_wreck_debris50PD::f5::bindMoveQuery(candidates);
  std::cerr<<"debris_f5_e8 bound="<<e8<<" candidates="<<native_wreck_debris50PD::f5::moveQuery().mirror.size()<<'\n';}
 // 167250 water: debris pieces borrow the racer's vehicle334 provider, i.e.
 // this race's runtime water owner (null when the track names no water, A9).
 {auto& w=native_wreck_debris50PD::f5::world();w.waterContext=selectedRuntimeWater;
  w.waterQuery=selectedRuntimeWater?+[](const void* c,float (&p)[3])noexcept{
      return static_cast<const native_water_runtime50JC::Owner*>(c)->query(p);}:nullptr;
  std::cerr<<"debris_f5_water bound="<<(selectedRuntimeWater!=nullptr)<<" source334="
      <<(actualHeight.sourceDomain.water334==UndertrackHeightOwner50EW::Present)<<'\n';}
 std::cerr<<"debris_f5_world type="<<unsigned(query.collisionTypeTag)<<" group="<<unsigned(query.groupTypeTag)
     <<" skip="<<query.skipMask<<" dynamic="<<query.dynamicMask<<" material="<<query.materialMask<<" candidates="<<candidates.size()<<'\n';
 coldLoadDiagnostics.begin("track_storage");
 // Original 1AA23C follows the geometry\ buoy/finish-line load; stand-in: after static collision.
 rr::platform::loading_progress50DU::step(.1f,0x1aa23c);
 rr::platform::loading_progress50DU::pulse("Preparing track storage");
 auto& trackStorage=*track.roles.storage;
 const auto& trackCounts=trackStorage.count;
 // Debris item a: the wreck pieces' 121920 zone walk reads this storage's
 // zone SearchLists (read-only) through a private shadow membership. Bound
 // only when every storage candidate is exactly the debris track span entry
 // (ref - 1 -> sim().track index).
 {bool same=trackCounts.terrain<=candidates.size();
  for(std::size_t i=0;same&&i<trackCounts.terrain;++i)same=trackStorage.candidates[i]==&candidates[i];
  const bool zoneWalk=same&&native_wreck_debris50PD::zones::bind(trackStorage.zones.data(),trackCounts.zones,
      trackStorage.trees.data(),trackCounts.methods,trackCounts.terrain,trackStorage.world,trackCounts.membership);
  if(!zoneWalk)native_wreck_debris50PD::zones::walk()=native_wreck_debris50PD::zones::Walk{};
  std::cerr<<"debris_zone_walk bind="<<zoneWalk<<" same_candidates="<<same<<" zones="<<trackCounts.zones
      <<" methods="<<trackCounts.methods<<" terrain="<<trackCounts.terrain<<" membership="<<trackCounts.membership<<'\n';}
 // PC ENHANCEMENT, NOT ORIGINAL (native_surface_traction50st.h): the original
 // 276880/272520 TerrainZone rows and piece->row mapping, read on every course
 // from that course's own TOB, over the static owners numbered from 0x10000000u.
 // Host-side only; nothing when the switch is off.
 if(rr::game::surfaceRules50ST.enabled()){
  const auto surfaceArchive=runtime_startup50fl::readBytes(trackSession->archive);
  rr::game::surfaceRules50ST.loadCourse(surfaceArchive,track.identity.scbMember.c_str(),
      actualWorld,ground.owners,0x10000000u);
  // PC ENHANCEMENT, NOT ORIGINAL (native_zero_gravity50zg.h): the racing-line
  // table (floor under the line by progress) from this course's own map and
  // AI splines and the same static owners. Built only when the course has a
  // ZeroGravity "on" row (no stock course has one) and the switch is on;
  // otherwise this call only clears the previous course's table. PORT EXTENSION:
  // the course's own N64 floor table tracks\<code>.nft (named from the world
  // member) is the table source when present; absent = the line sampling.
  const auto floorMember=rr::game::n64FloorMember50NH(track.identity.scbMember.c_str());
  rr::game::zeroGravity50ZG.loadCourse(surfaceArchive,track.identity.mapSpline.c_str(),track.identity.aiSpline.c_str(),
      ground.owners,rr::game::surfaceRules50ST,floorMember.c_str());
 }
 // PORT EXTENSION, NOT ORIGINAL (n64_track_hazards_runtime50nh.h): the N64
 // hazard rows of this course's TOB (energy column, breakable asteroids, door),
 // SurfaceObject names resolved to the static owners by the 27A524 instance
 // name rule (SurfaceRules50ST's names). Every course; no rows (every stock
 // course) = nothing built and no log line. Test rows only with
 // RR_TEST_N64_HAZARD (across the course's own map line).
 {
  static const std::vector<rr::game::N64HazardRow50NH> noRows;
  const auto& courseN64Rows=track.roles.triggerSource?track.roles.triggerSource->n64Rows():noRows;
  std::vector<std::uint8_t> hazardArchive;const std::vector<std::uint8_t>* hazardBytes=nullptr;
  if(const char* t=std::getenv("RR_TEST_N64_HAZARD");t&&*t){
   hazardArchive=runtime_startup50fl::readBytes(trackSession->archive);hazardBytes=&hazardArchive;}
  rr::game::n64HazardRuntime50NH.loadCourse(courseN64Rows,rr::game::surfaceRules50ST,hazardBytes,
      track.identity.mapSpline.c_str(),track.identity.code.text.data());
 }
 // The cold payload ledger of the one collision world: its own descriptors and
 // storage plus later cold owners' reservations, inside the race's one limit.
 // PORT STAND-IN: the limit is the shared track_runtime_storage50IG ceiling (one value
 // for every course), not a size; the original has no such ledger.
 std::size_t additionalPayloadReserved=0;
 const auto remainingPayloadBytes=[&](){
  const auto used=trackCollision->payload().totalRequested;
  require50DU(used<=trackStorageLimits.coldPayloadBytes&&
      additionalPayloadReserved<=trackStorageLimits.coldPayloadBytes-used,
      "additional cold payload accounting overflow");
  return trackStorageLimits.coldPayloadBytes-used-additionalPayloadReserved;
 };
 const auto reserveAdditionalPayload=[&](std::size_t bytes,const char* reason){
  require50DU(bytes<=remainingPayloadBytes(),reason);
  additionalPayloadReserved+=bytes;
 };
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
 // The course TUN, read by the track load (in-archive, or the registry row's inherited
 // tuning archive, PORT EXTENSION A12): one member, the same decoders for every course.
 const auto& tuning=track.documents.tuning;
 require50DU(!tuning.empty(),"the track's tuning (TUN) is unavailable");
 require50DU(rr::assets::readEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),tuning.size(),x.parameters.impactTuning,diagnostic),"actual selected-track impact tuning");x.parameters.impactTuningBound=true;
 if(originalAiEnabled){
  require50DU(rr::assets::readAIEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),x.parameters.impactTuning,diagnostic),"original AI environment tuning unavailable");
  require50DU(rr::assets::readAiTrackTuning50IU(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),originalAiTuning,originalAiAuthoringDiagnostic),"original AI track tuning unavailable");
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

 // Preserve the actual fresh constructor records before the player's probe
 // is enabled. Fresh opponent slots install these cold records before enabling
 // their own authored probe; no already-running state is reset this way.
 require50DU(s.phase==VehicleFramePhase49W::Ready&&m.frameFactor==0.f&&
     !s.terrainProbe.enabled&&!s.bodies.main.domain.probe100,
     "source pre-enable terrain constructor state unavailable");
 const auto freshTerrainProbe=s.terrainProbe;
 const auto freshMainProbe=s.bodies.main.components.probe348;
 const auto freshProbe100=s.bodies.main.domain.probe100;
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
 // Every authored trigger/animation group of the one collision world uses the same
 // declared native shape5/6 backend, on every course.
 query.nativeGatePolicy=secondaryQuery.nativeGatePolicy=true;
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
  .track=track,
  .roster=roster,
  .laps=laps,
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
  .consumedStage=consumedStage,
  .controlPresets=controlPresets,
  .courseColdLoad=courseColdLoad,
  .rootedCourseRace=rootedCourseRace,
  .courseRaceLaunched=courseRaceLaunched,
  .deviceIndex=deviceIndex,
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
  .trackAnimationMembership=trackAnimationMembership,
  .trackAnimations=trackAnimations,
  .trackAnimationBinding=trackAnimationBinding,
  .trackAnimationBindingDiagnostic=trackAnimationBindingDiagnostic,
  .trackAnimationPrefixArmed=trackAnimationPrefixGuard.armed,
  .trackAnimationDiagnostic=trackAnimationDiagnostic,
  .triggerCandidateFirst=triggerCandidateFirst,
  .triggerMembership=triggerMembership,
  .aiView=aiView,
  .liveAudioPrepared=liveAudioPrepared,
  .mapView=mapView,
  .preparedAudioRoot=preparedAudioRoot,
  .remainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{remainingPayloadBytes},
  .soundMode=soundMode,
  .ground=ground,
  .groundStack=groundStack,
  .headlessLive=headlessLive,
  .host=host,
  .hostDiagnostic=hostDiagnostic,
  .humanControlInput=humanControlInput,
  .humanCourseEnabled=humanCourseEnabled,
  .lapContinuity=lapContinuity,
  .lastCommittedWrongWay=lastCommittedWrongWay,
  .liveAudio=liveAudio,
  .loadedAudioSettings=loadedAudioSettings,
  .loadedTrack=loadedTrack,
  .m=m,
  .mainPairCapacity=mainPairCapacity,
  .managerRuntimeSelection=managerRuntimeSelection,
  .waterUpdateSequence=waterUpdateSequence,
  .mode1Enabled=mode1Enabled,
  .mode1Requested=mode1Requested,
  .monotonicNow50DU=monotonicNow50DU,
  .movement=movement,
  .nativeFireSerial=nativeFireSerial,
  .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
  .nativeOriginalFleet=nativeOriginalFleet,
  .nativeRacerManager=nativeRacerManager,
  .nativeRecordActor=nativeRecordActor,
  .nativeSelectedLights=nativeSelectedLights,
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
  .reserveAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveAdditionalPayload},
  .resetGlobals=resetGlobals,
  .resetLifecycle=resetLifecycle,
  .response=response,
  .s=s,
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
  .sourcePositionStatsPrestage=sourcePositionStatsPrestage,
  .sourcePositionStatsPrestageDiagnostic=sourcePositionStatsPrestageDiagnostic,
  .sourceStaticCandidates=sourceStaticCandidates,
  .spring=spring,
  .stackCount=stackCount,
  .startAtMenu=startAtMenu,
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

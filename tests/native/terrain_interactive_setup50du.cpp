#include "original_second_human_input50sv.h"
#include "player_ai_pilot50du.h"
#include "../../src/assets/dlc_files50dl.h"
#include "../../src/game/original_ui_game_options_controller50kg.h" // C15: 1865B4 profile reset (soundMode1101C)
#include "native_last_failure50pd.h"
#include <cfenv>
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "../../platform/linux/native/stall_watchdog50du.h"
#include "../../src/game/ps2_float_mode50la.h"
#include "terrain_interactive_setup50du.h"
#include <cstring>
#include "../../src/game/indexed_type2_contact50cc.h"
#include "../../src/game/collision_mesh_admission50cc.h"
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
// Common rewrite: the one owner of each track role (common_track50cr.h Roles).
#include "track_collision_world50cr.h"            // R3: track.roles.collision (one closer, one Borrow)
#include "positional_sound_rows50sr.h"            // R7: track.roles.sounds
#include "material_audio_provider50hi_fixture.h"  // R7: track.roles.audio
#include "native_results_world_stop26d070.h"
#include "original_disc_effects50lb.h"            // the six shared pod effect files (EMT)
#include "effect_cold50bj.h"                      // buildEffectControlOwner50BJ
#include "gc_gate_audio50in_fixture.h"            // triggerAudio: the one PodSound voice (shared kernel)
#include "original_follow_query_bridge50kx.h"     // the one Follow bridge (B-C18)
#include "native_spring_detector_policy50la.h"
#include "original_ai_reset_ground_bridge.h"
#include "native_racer_cables50hw_fixture.h"      // native_racer50HW::buildColdFlames (each slot's own pod)
#include "track_animation_binding50an.h"
#include "../../src/assets/spring_authoring50f.h"
#include "native_gc_other_particle_children50kg_fixture.h"
#include "native_cold_model_retirement50lh.h"
#include "original_ai_physical_recovery.h"
#include "original_human_physical_recovery.h"
#include "gc_particle_materials50kg_fixture.h"
#include "original_position_stats_prestage50la.h"
#include <fstream>
#include <iomanip>

#include "pod_speech_authoring50hg_fixture.h"
#include "track_animation_candidates50an.h"
#include "terrain_interactive_storage50du.h"
#include "runtime_allocation_counter50fl.h"
#include "native_ai_session50iv_fixture.h"
#include "../../src/game/original_human_finish50kg.h"
#include "player_loading_session50du.h"
#include "original_racer_manager_entity50la.h"
#include "original_subtype3_record_owner50la.h"
#include "original_record_grid_pose50la.h"
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
// One trigger shape bound for every course: the 274E90 row's own sphere
// (shape5) or box (shape6) from the track's trigger source, as the cold
// 2D93F0 ->120070 registration reads it. No course table.
rr::game::LiveZoneBounds50EN triggerBounds50DU(const course_trigger_sources50CT::Source& source)noexcept {
 rr::game::LiveZoneBounds50EN out{};out.bounds.shapeTag=source.shapeType;
 for(unsigned n=0;n<16;n+=5)out.bounds.shapeWorld.v[n]=1.f;
 if(source.shapeType==6){out.bounds.shapeWorld=source.box.world;
  for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=source.box.center.v[n];out.bounds.localHalf.v[n]=source.box.half.v[n];}
 }else for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=source.sphere.center.v[n];out.bounds.localHalf.v[n]=source.sphere.radius;}
 for(unsigned n=0;n<3;++n)out.bounds.cachedCenter.v[n]=source.sphere.center.v[n];
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=source.sphere.radius;return out;
}
// Caller-owned wall-only aggregates. These are nested completion_wait
// components, never additional disjoint phases or GPU-only measurements.
using interactive_loop50DU::GameplayDisplayWallTotals50DU;
// Independent of periodic telemetry resets. No clocks or FPS decisions here.
constexpr std::uint32_t raceRacerCapacity=rr::game::defaultHostRaceRacers50HT;
constexpr std::uint32_t raceOpponentCapacity=raceRacerCapacity-1;
}
SessionExit50DU Context::run() {
 auto& track=this->track;
 auto& roster=this->roster;
 auto& laps=this->laps;
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
 auto& consumedStage=this->consumedStage;
 auto& controlPresets=this->controlPresets;
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
 auto& freshMainProbe=this->freshMainProbe;
 auto& freshProbe100=this->freshProbe100;
 auto& freshTerrainProbe=this->freshTerrainProbe;
 auto& trackAnimationMembership=this->trackAnimationMembership;
 auto& trackAnimations=this->trackAnimations;
 auto& trackAnimationBinding=this->trackAnimationBinding;
 auto& trackAnimationBindingDiagnostic=this->trackAnimationBindingDiagnostic;
 auto& trackAnimationPrefixArmed=this->trackAnimationPrefixArmed;
 auto& trackAnimationDiagnostic=this->trackAnimationDiagnostic;
 auto& triggerCandidateFirst=this->triggerCandidateFirst;
 auto& triggerMembership=this->triggerMembership;
 auto& aiView=this->aiView;
 auto& liveAudioPrepared=this->liveAudioPrepared;
 auto& mapView=this->mapView;
 auto& preparedAudioRoot=this->preparedAudioRoot;
 const auto& remainingPayloadBytes=this->remainingPayloadBytesCall;
 auto& soundMode=this->soundMode;
 auto& ground=this->ground;
 auto& groundStack=this->groundStack;
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
 auto& mainPairCapacity=this->mainPairCapacity;
 auto& managerRuntimeSelection=this->managerRuntimeSelection;
 auto& waterUpdateSequence=this->waterUpdateSequence;
 auto& mode1Enabled=this->mode1Enabled;
 auto& mode1Requested=this->mode1Requested;
 auto& monotonicNow50DU=*this->monotonicNow50DU;
 auto& movement=this->movement;
 auto& nativeFireSerial=this->nativeFireSerial;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
 auto& nativeRacerManager=this->nativeRacerManager;
 auto& nativeRecordActor=this->nativeRecordActor;
 auto& nativeSelectedLights=this->nativeSelectedLights;
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
 const auto& reserveAdditionalPayload=this->reserveAdditionalPayloadCall;
 auto& resetGlobals=this->resetGlobals;
 auto& resetLifecycle=this->resetLifecycle;
 auto& response=this->response;
 auto& s=this->s;
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
 auto& sourcePositionStatsPrestage=this->sourcePositionStatsPrestage;
 auto& sourcePositionStatsPrestageDiagnostic=this->sourcePositionStatsPrestageDiagnostic;
 auto& sourceStaticCandidates=this->sourceStaticCandidates;
 auto& spring=this->spring;
 auto& stackCount=this->stackCount;
 auto& startAtMenu=this->startAtMenu;
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
 // Collision mesh admission cache lives exactly as long as this race session:
 // declared before every session-local owner, so it closes after they are
 // destroyed. Track/world candidates are owned outside and outlive it.
 // Host memory only; see collision_mesh_admission50cc.h.
 rr::game::CollisionAdmissionScope50CC collisionAdmissionScope50CC;
 // The actual guards remain early; these views have no destructor action.
 struct ArmedFlagView { bool& armed; };
 ArmedFlagView trackAnimationPrefixGuard{trackAnimationPrefixArmed};
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
  // Legacy layout: the storage accessor lends these two memberships as rows 3/4.
  require50DU(trackStorage.sharedLayout()||!trackCounts.legacyPodQuery||trackStorage.bindLegacyRecoveryRows(recoveryQueryOwners.memberships[0],
      recoveryQueryOwners.storage(0),recoveryQueryOwners.memberships[1],recoveryQueryOwners.storage(1)),
      "actual694/7A4 legacy query rows rejected");
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
  // Port stand-in (track_runtime_storage50IG::sizeRosterPodExclusionStandIn):
  // the other-pod694 exclusion list of 690/364/7A4, sized here before the
  // Bank seals; unbanked (same for every racer), filled once at the cold roster.
  require50DU(track_runtime_storage50IG::sizeRosterPodExclusionStandIn(qfilter)&&
      track_runtime_storage50IG::sizeRosterPodExclusionStandIn(secondaryFilter)&&
      track_runtime_storage50IG::sizeRosterPodExclusionStandIn(movement.filter),
      "roster pod694 exclusion stand-in sizing rejected");
  movement.copied40=movement.copied50={{0.f,0.f,0.f,1.f}};
 }

 // Static references preserve authored candidate order; three real query
 // owners follow them. No fixed TA zone/collider dimensions remain here.
 // Slot 0's 690/364/348 refs (human/camera subject). Per-racer consumers read
 // trackStorage.currentRow(role).ref at use; the shared world has no fixed ref.
 const CollectionRef50BX mainQueryRef=trackStorage.row(0,track_runtime_storage50IG::RosterRole::main690).ref,
     secondaryQueryRef=trackStorage.row(0,track_runtime_storage50IG::RosterRole::secondary364).ref,
     terrainQueryRef=trackStorage.row(0,track_runtime_storage50IG::RosterRole::terrain348).ref;
 const auto collectionObjectCount=trackCounts.objects,zoneCount=trackCounts.zones;
 auto* collectionObjects=trackStorage.objects.data();
 auto* collectionSource=trackStorage.source.data();
 auto* collectionSecond=trackStorage.second.data();
 auto* treeSource=trackStorage.treeSource.data();auto* treeSecond=trackStorage.treeSecond.data();
 auto& zoneScratch=trackStorage.registrationScratch;
 auto& zoneLifecycle=trackStorage.lifecycle;auto& zoneQueue=trackStorage.queue;
 auto& liveWorld=trackStorage.world;
 // The live racer's 690/364/348/694/7A4 rows, in role order, through the one
 // storage accessor.
 auto removeCurrentQueryRows=[&](auto& remove){
  return trackStorage.forEachPresentRow([&](std::size_t,track_runtime_storage50IG::RosterRole,
      track_runtime_storage50IG::RosterRowRef& queryRow){return bool(remove(*queryRow.state,queryRow.ref));});
 };
 // The track's one collision world (track.roles.collision, 271230/27BB40) must
 // be lending its geometry before any membership attaches; refused here, before
 // the closers below exist, rather than at their unwinding.
 require50DU(track.roles.collision&&track.roles.collision->geometryReady(),
     "the track's collision world (05.04) is not ready");
 auto removeZoneMembership=[&](LiveZoneMembership50EN& state,CollectionRef50BX expected){
  if(state.attached&&state.object!=expected)return false;
  const auto result=removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,state,trackStorage.membershipWork);
  return result.status==LiveZoneStatus50EN::Complete&&!state.attached&&!state.object;
 };
 // Live trigger memberships of the track's 274E90 rows (refs
 // triggerCandidateFirst+1.., the live prefix of the one Block; none: nothing).
 auto removeTriggerMemberships=[&]()noexcept->bool{
  for(std::size_t i=0;i<triggerMembership.size();++i){
   auto* slot=triggerMembership.at(i);
   if(!slot||!*slot||!removeZoneMembership((*slot)->state,static_cast<CollectionRef50BX>(triggerCandidateFirst+i+1)))return false;
  }
  return true;
 };
 // ONE closer for every course over the one collision world: the current
 // racer's query rows, then every live trigger membership, before the World
 // at race+74 retires (1A6840 releases every race-owned registration before
 // 1B2870). Replaces the five per-course closers (common-path rule).
 bool collisionCleanupDone=false;
 auto closeCurrentCollision=[&]()noexcept->bool {
  auto& world=*track.roles.collision;
  auto borrow=world.geometryReady()?world.borrow():world.retirementBorrow();
  if(!borrow||borrow.world()!=&liveWorld||zoneQueue.count)return false;
  if(const auto* actions=borrow.actions();actions&&!actions->queryIdle())return false;
  if(!removeCurrentQueryRows(removeZoneMembership))return false;
  s.shape.registrationE0=0;
  return removeTriggerMemberships();
 };
 struct CurrentCollisionGuard {
  bool& done;decltype(closeCurrentCollision)& close;
  ~CurrentCollisionGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } currentCollisionGuard{collisionCleanupDone,closeCurrentCollision};
 // Trigger memberships alone (idempotent with the closer above: a detached
 // membership removes as Complete).
 bool membershipCleanupDone=false;
 auto closeCurrentMemberships=[&]() noexcept -> bool {return removeTriggerMemberships();};
 // Handles partial attachment before a Bank exists. The later Bank guard
 // closes every real saved slot and marks this same completion flag.
 struct CurrentMembershipGuard {
  bool& done;decltype(closeCurrentMemberships)& close;
  ~CurrentMembershipGuard()noexcept{if(!done){if(!close())std::terminate();done=true;}}
 } currentMembershipGuard{membershipCleanupDone,closeCurrentMemberships};
 // ONE navigation for every course: the map and AI views of the track's one
 // store (2A1160/2A1260 splines, 1ACF70 zone index bound to the one collision
 // world, step 08). All suspended ordinary recovery calls keep this selection.
 const auto requireRuntimeNavigation=[&](){
  require50DU(mapView.runtimeValid()&&aiView.runtimeValid()&&
      mapView.role()==common_track50CR::NavigationRole::map&&aiView.role()==common_track50CR::NavigationRole::ai&&
      mapView.sameOwner(aiView)&&mapView.sameWorld(liveWorld)&&aiView.sameWorld(liveWorld)&&
      mapView.storage()==track.roles.navigation.get(),
      "navigation requires the track's one map/AI store bound to the actual liveWorld");
 };
 const auto& selectedOriginalAiMapMetadata=originalAiMapMetadata;
 const auto& selectedOriginalAiMetadata=originalAiMetadata;
 const auto selectedMapCount=[&](){requireRuntimeNavigation();return mapView.count();};
 const auto selectedAiCount50MB=[&](){requireRuntimeNavigation();return aiView.count();};
 const auto selectedMapNode=[&](std::uint32_t index)->const UndertrackNode50ET&{
  requireRuntimeNavigation();
  require50DU(index<mapView.count(),"map node is outside the track's map spline");
  return mapView.nodes()[index];
 };
 const auto selectedMapIndex=[&](std::uint32_t index){
  requireRuntimeNavigation();
  require50DU(index<mapView.count(),"map index is outside the track's map spline");
  return mapView.nodeIndex78()[index];
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
 // The track's own tuning AIDriving.Lookahead steers the host preview when the
 // course authors it (data, every course; host driver only).
 if(rr::host::readCourseSteeringLookahead50GK(
     reinterpret_cast<const char*>(tuning.data()),tuning.size(),
     courseTuning.authoredSteeringLookaheadSeconds,courseDiagnostic))
  courseTuning.fullThrottleSteeringRange=true;
 else courseDiagnostic={};
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
  if(!context||!context->music||!context->audio||!context->sourceEventType||!context->audio->outputOpen()||
      !(((song==44||song==45)&&argument==1)||(song==25&&argument==0)))return false;
  auto request=[&](){return context->music->request(song,argument,*context->sourceEventType);};
  auto receipt=request();
  using Result=race_music50IE::Result;
  // Pause Restart can leave 1AC960's stream envelope queued on the paused mixer.
  // A zero-cut start requests its song before the first regular mixer update.
  if(receipt.result==Result::PendingTimedVolume26C8F0||receipt.result==Result::PendingFade234440){
   if(context->music->update(0.f).completed())receipt=request();
  }
  if(!receipt.completed())std::fprintf(stderr,"race_start_music_refused song=%d status=%u pc=%x\n",
      song,unsigned(receipt.result),receipt.sourcePlayPC);
  return receipt.completed();
 };
 const auto requestIntroMusicFade=+[](void* opaque,float seconds)noexcept {
  auto* context=static_cast<StartMusicContext*>(opaque);
  return context&&context->music&&context->audio&&context->audio->outputOpen()&&seconds==0.5f&&
      context->music->requestFade1A4AB0(seconds).completed();
 };
 std::unique_ptr<race_mode1_host50ID::Owner> mode1;
 // This owner dies before the live mixer/shared boost PCM and owns96..116.
 auto opponentAudio=std::make_unique<opponent_audio50HX::Owner>();
 auto originalAiAudio=std::make_unique<native_original_ai_audio::Owner>();
 std::shared_ptr<rr::platform::OriginalSoundMode50KG> gameplaySoundMode;
 // D44 borrows the track's material PCM bank (track.roles.audio) and the
 // track's positional rows (track.roles.sounds) play on the same mixer. Retire
 // the actual borrowers first, then the positional transports, then the
 // material voices; the same mixer and canonical source outlive this guard.
 // Later root terminal cleanup may invoke this idempotent join. One rule for
 // every course.
 bool audioOwnersRetired=false;
 const auto retireAudioOwners=[&]()noexcept{
  if(audioOwnersRetired)return true;
  originalAiAudio->close();
  if(track.roles.sounds&&!track.roles.sounds->close())return false;
  if(track.roles.audio)track.roles.audio->reset();
  audioOwnersRetired=true;return true;
 };
 struct AudioOwnerGuard {
  const decltype(retireAudioOwners)& retire;
  ~AudioOwnerGuard(){if(!retire())std::terminate();}
 } audioOwnerGuard{retireAudioOwners};
 // Only fixed source state enters the racer bank; mixer/PCM ownership never does.
 opponent_audio50HX::BoostState opponentBoostAudio{};
 const char* audioSetting=std::getenv("RR_AUDIO");
 const bool audioOutputRequested=!(audioSetting&&std::string(audioSetting)=="0");
 // ALL8 still owns its real logical D40/D44/event/sample state when host
 // output is muted. RR_AUDIO=0 requests neither a device nor a WAV; the
 // same software mix advances real playback lifetime and source chronology.
 const bool audioEnabled=audioOutputRequested||nativeOriginalFleet;
 const bool originalAiAudioEnabled=audioEnabled&&nativeOriginalFleet;
 require50DU(!audioEnabled||track.roles.audio,
     "the track's material audio provider is unavailable; ALL8 requires logical owners even with muted host output");
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
 std::unique_ptr<gc_gate_audio50IN::Owner> triggerAudio;
 SpeechPosition50HG audioListener{};bool audioListenerReady=false;
 std::uint64_t speechEvents=0,speechCues=0,announcerRequestsUnjoined=0;
 if(consumedStage.race()&&!audioEnabled)require50DU(liveAudio->finishCountedMutedGroups(loadedAudioSettings),
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
  if(!liveAudioPrepared){
   const bool prepared=liveAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),audioRoot,audioError,playerRacer60,loadedAudioSettings);
   require50DU(prepared,audioError.empty()?"native audio bank preparation failed":audioError.c_str());
  }else require50DU((preparedAudioRoot.empty()||audioRoot==preparedAudioRoot)&&
      liveAudio->bank().ready&&liveAudio->mixerForMaterialOwner()&&liveAudio->groupsForProviders(),
      "early audio owner no longer matches actual later consumers");
 coldLoadDiagnostics.begin("audio_camera_and_race");
  auto* materialMixer=liveAudio->mixerForMaterialOwner();
  if(originalAiAudioEnabled){
   if(liveAudioPrepared&&soundMode){
    // The early audio owner already bound its source-mode owner to this mixer.
    require50DU(soundMode->ready()&&materialMixer&&
        materialMixer->originalSoundMode50KG()==soundMode.get()&&
        (!originalMainApplication.soundMode||originalMainApplication.soundMode==soundMode),
        "actual early source-mode owner differs at later audio join");
    gameplaySoundMode=soundMode;
   }else{
   gameplaySoundMode=originalMainApplication.soundMode;
   if(!gameplaySoundMode){
    gameplaySoundMode=std::make_shared<rr::platform::OriginalSoundMode50KG>();
    // C15: the profile's own 1101C value; with no profile, the original reset 1865B4 value.
    rr::game::OriginalUiGameSettingsProfile50KG resetProfile{};
    rr::game::resetOriginalUiGameSettingsProfileFields50KG(resetProfile);
    const auto& profile1101C=originalMainApplication.gameSettingsProfile?*originalMainApplication.gameSettingsProfile:resetProfile;
    require50DU(gameplaySoundMode->initializeFromProfile1101C(profile1101C.soundMode1101C),
        "direct native race stereo-mode construction from the profile rejected");
   }
   // The cold load binds the Application's one mode before 05.12 (world); reuse it.
   if(!(materialMixer&&materialMixer->originalSoundMode50KG()==gameplaySoundMode.get()))
   require50DU(materialMixer&&materialMixer->bindOriginalSoundMode50KG(gameplaySoundMode),
       "actual gameplay source-mode binding rejected");
   }
  }
  auto* audioGroups=liveAudio->groupsForProviders();
  auto* sharedAnnouncer=liveAudio->sharedAnnouncer();
  require50DU(audioGroups&&sharedAnnouncer,"shared native audio group/episode owner unavailable");
  // The race UI emitter (raceUI.ini keys 1..6) is common to every course; a
  // human race's Results menu (common race or RA) dispatches all six.
  if(originalCameraEnabled)require50DU(materialMixer&&((sourceConsumedPauseActivation&&consumedStage.race())||humanCourseEnabled?
      sourceCameraCycleAudio.prepareResultsFeedback(runtime_asset_paths50LB::resolve("work/native/project/disc"),
          audioRoot,*materialMixer,*audioGroups,audioError):
      sourceCameraCycleAudio.prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),
          audioRoot,*materialMixer,*audioGroups,audioError)),
      audioError.empty()?"original camera cycle audio owner unavailable":audioError.c_str());
  // A PodSound voice exists exactly when the trigger data authors a PodSound
  // action; the voice file is the action's own Filename (stock GA: BIGDOOR).
  // A PodSound voice exists exactly when the track's trigger data authors a
  // PodSound action; the voice file is the action's own Filename (one rule for
  // every course; stock GA/GC: BIGDOOR).
  if(const auto* podSound=track.roles.triggerSource?track.roles.triggerSource->firstPodSound():nullptr){
   // C12: every PodSound action's own file is loaded (first-use order); each action plays its own.
   std::vector<std::string> podSoundFiles{podSound->filename};
   for(const auto& source:track.roles.triggerSource->sources())for(const auto& action:source.actions)
    if(action.kind==course_trigger_sources50CT::ActionKind::PodSound)podSoundFiles.push_back(action.filename);
   triggerAudio=std::make_unique<gc_gate_audio50IN::Owner>();
   const bool prepared=materialMixer&&audioGroups&&triggerAudio->prepare(*materialMixer,*audioGroups,audioRoot,audioError,podSoundFiles);
   require50DU(prepared&&triggerAudio->available(),audioError.empty()?"trigger PodSound PCM/World4D8 voice unavailable":audioError.c_str());
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
        original_racer_selection50IJ::findAnyByPodId(racerRigs[slot]->podId.c_str());
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
  // The track's one AUB catalog and material provider (271600/271674..271804)
  // and its positional rows were prepared by the track load (step 05.12) on
  // this same mixer; borrow them (one rule for every course; stock row counts
  // are receipts there).
  require50DU(materialMixer&&track.roles.audio,"the track's material audio provider (05.12) is unavailable");
  auto& trackAudio=*track.roles.audio;
  //271600 applies actual AUB assignments before the creator publishes A9, for
  //every static owner of the track's one ground scene.
  for(std::size_t i=0;i<trackBindings.size();++i){
   const auto& binding=trackBindings[i];
   // Animated CHARACTER owners retain constructor materialA9=0; AUB's
   // static SCB assignments do not index their sentinel originalOwner.
   if(binding.animated||binding.trigger)continue;
   require50DU(binding.originalOwner<ground.owners.size(),"static material owner index outside the track's ground");
   require50DU(i<trackStorage.contactMaterials.size(),"contact material row outside the track's storage");
   const auto& original=ground.owners[binding.originalOwner];
   require50DU(rr::assets::applyTrackMaterialAssignments50HI(trackAudio.catalog(),actualWorld,
       original.zone,original.ordinal,trackStorage.contactMaterials[i],audioError),
       audioError.empty()?"original contact material assignment failed":audioError.c_str());
  }
  require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
      race_audio_groups50IC::ProviderRole::MaterialCollision,64,material_audio50HI::soundCount),
      "material group ownership rejected");
  if(trackAudio.extraVoiceCount())require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
      race_audio_groups50IC::ProviderRole::MaterialCollision,trackAudio.extraVoiceFirst(),
      trackAudio.extraVoiceCount()),"extra original material sound group ownership rejected");
  // The track's one positional-sound owner (track.roles.sounds): present
  // exactly when the AUB authors SoulTree/ambient rows or the triggers author
  // world sounds (common reader). Its transports join the Results 26D070 stop.
  // An AUB whose [Sounds] lists author no SoulTree row, on a track whose triggers author no
  // Sound action, builds nothing: the 279910/2787B0 row loop runs zero times and no
  // transport exists (the owner's own contract: ready()==false, authored()==false). The
  // ready() demand applies only when there is something to play; a zero-row track (the
  // NV add-on: 0 SoulTree, 0 ambient, 0 trigger sounds) races without positional sound.
  if(auto* sounds=track.roles.sounds.get();sounds&&originalAiAudioEnabled&&!sounds->authored()){
   std::cerr<<"positional_sounds site=common course="<<track.code()<<" transport=none reason=no_authored_rows\n";
  }else if(sounds&&originalAiAudioEnabled){
   require50DU(sounds->ready(),"the track's positional sound rows (05.12) are not ready");
   // One registration: the owner's one logical transport carries every voice it plays (the
   // SoulTree/ambient rows AND the trigger world one-shots, playTrigger on the same logical_).
   // Registering the owner as well listed each active voice twice, and the Results stop
   // refused its duplicate claim (cr16: aural_stop26D070_refused step=15 on TA and RA, the
   // Results input never ran). The old per-course code had two separate owners with
   // separate voices; the merged common owner must be listed once.
   require50DU(originalAiAudio->registerResultsWorldSound26D070(sounds->resultsWorldTransport26D070()),
       "positional audio Results stop registration rejected");
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
    for(std::uint32_t slot=0;slot<originalAiHumanCount;++slot){
     auto& controller=slot?liveAudio->secondHumanController():liveAudio->humanController();
     const auto base=std::uint64_t(slot+1)*16;
     const auto* selection=slot?original_racer_selection50IJ::findAnyByPodId(racerRigs[slot]->podId.c_str()):selectedOriginal;
     require50DU(selection&&controller.bind({sessionIndex+1,base+1,base+2,base+3,slot,selection->racer60})&&
         originalAiAudio->prepareHumanResetOwner(controller),
         "actual human controller/material reset ownership rejected");
    }
   }
   require50DU(originalAiAudio->prepareSpeech(runtime_asset_paths50LB::resolve("work/native/project/disc"),audioRoot,
       *materialMixer,*audioGroups,audioError),audioError.empty()?"actual all-eight D48 banks unavailable":audioError.c_str());
   const bool materialRows=originalAiAudio->prepareMaterialArrays(*materialMixer,*audioGroups,
       trackAudio.catalog(),trackAudio.bank(),audioError);
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
 // ONE navigation and lap rule for every course: the map/AI views of the
 // track's one store (2A1160/2A1260 + 1ACF70, step 08) and the one lap tracker
 // (ProgressNode markers 2A1980/2A273C/2A26E0). Counts are the spline files'.
 requireRuntimeNavigation();
 if(!laps.prepared()){
  common_lap50CR::Diagnostic lapDiagnostic{};
  require50DU(laps.prepare(mapView,aiView,raceRacerCount,lapDiagnostic),
      lapDiagnostic.reason?lapDiagnostic.reason:"the track's lap markers (2A1980) were rejected");
 }
 require50DU(laps.map().sameOwner(mapView)&&laps.ai().sameOwner(aiView)&&laps.field()==raceRacerCount,
     "lap tracker views differ from the track's navigation");
 // The human lap owner reads the one map view (its own spline count, bounded by the view's
 // capacity), never the legacy 256-row LapProgressTrack50GF path: a map spline longer than
 // 256 points (TC 268, and the 257-293 courses) refused its first scan crossing at the start
 // line (live1A0DA0 navigation error 9 at 1A10D0 node 0). Every course, one binding.
 require50DU(lapOwner.bindTrack(mapView,laps.markers())&&lapOwner.trackBound(),
     "the human lap owner refused the track's map view and lap markers (2A1980)");
 std::cerr<<"common_lap_bind owner=human map_nodes="<<mapView.count()<<" capacity="<<mapView.capacity()
     <<" markers="<<laps.markers().progressCount48<<" bound="<<lapOwner.trackBound()<<'\n';
 // AI/map metadata: the AI consumers read the one shared store through
 // AiTrackMetadataView50IU (navigation_view50cr.h; 1AAB30 reads the loaded spline), sized from
 // each spline's own count (TB/TC/MB/GB 257-293 points). No fixed-size copy is made: the
 // legacy AiTrackMetadata50IU rows (256) stay unbound and nothing reads them.
 require50DU(!originalAiEnabled||(mapView.metadata()&&aiView.metadata()),"map/AI spline AI metadata unavailable");
 (void)originalAiMapMetadata;(void)originalAiMetadata;
 require50DU(rr::host::readCourseSpeedLimits50GK(reinterpret_cast<const char*>(track.documents.aiSpline.data()),
     track.documents.aiSpline.size(),aiView.count(),courseLimits,courseDiagnostic),
     "the track's A spline speed metadata was rejected");
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
  require50DU(checkpointInput->zoneCount==liveWorld.zoneCount,"checkpoint world namespace changed");
  requireRuntimeNavigation();
  require50DU(checkpointInput->progress.reset.vehicleC80<aiView.count()&&
      checkpointInput->progress.vehicleC84<aiView.count()&&
      checkpointInput->progress.reset.vehicleC88<mapView.count()&&
      checkpointInput->progress.reset.vehicleC8C<mapView.count()&&
      checkpointInput->selection.anchorNode<mapView.count()&&
      mapView.nodeIndex78()[checkpointInput->selection.anchorNode]==checkpointInput->nodeIndex78,
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
 // Slot 0's rows (the human/camera subject). Row objects are stable,
 // unbanked heap objects in the shared world, so these references stay valid;
 // every per-racer consumer reads trackStorage.currentRow(role) at use.
 auto& membershipWork=trackStorage.membershipWork;
 const auto slot0Main=trackStorage.row(0,track_runtime_storage50IG::RosterRole::main690);
 const auto slot0Secondary=trackStorage.row(0,track_runtime_storage50IG::RosterRole::secondary364);
 const auto slot0Terrain=trackStorage.row(0,track_runtime_storage50IG::RosterRole::terrain348);
 require50DU(bool(slot0Main)&&bool(slot0Secondary)&&bool(slot0Terrain),"actual slot-0 690/364/348 query rows unavailable");
 auto& mainMembership=*slot0Main.state;
 auto& secondaryMembership=*slot0Secondary.state;
 auto& terrainMembership=*slot0Terrain.state;
 const auto mainMembershipStorage=slot0Main.storage;
 const auto secondaryMembershipStorage=slot0Secondary.storage;
 const auto terrainMembershipStorage=slot0Terrain.storage;
 live_terrain_probe50fr::Owner terrainLiveOwner{};
 require50DU(live_terrain_probe50fr::initialize(terrainLiveOwner,terrainOwner.initial.segment),"initialize actual single-pair terrain owner");
 // Legacy layout only: the one saved 694 context over the lent membership.
 // Shared world: each racer's own context lives in its 694 row
 // (Storage::bindRowPodContext, bound in that racer's cold visit).
 const auto legacyPodRow=trackStorage.sharedLayout()?track_runtime_storage50IG::RosterRowRef{}:
     trackStorage.row(0,track_runtime_storage50IG::RosterRole::pod694);
 const PodLiveQueryContext50IZ recoveryPodLive{{zoneLifecycle,liveWorld,
     legacyPodRow?*legacyPodRow.state:recoveryQueryOwners.memberships[0],membershipWork,legacyPodRow.ref},&x.parameters.shape};
 using QueryRole50DU=track_runtime_storage50IG::RosterRole;
 auto recoveryBorrowedQueries=[&](){
  //Current racer's own 690/348 rows from the one storage accessor. An empty
  //row leaves the field null and the undertrack join refuses it.
  const auto mainRow=trackStorage.currentRow(QueryRole50DU::main690);
  const auto terrainRow=trackStorage.currentRow(QueryRole50DU::terrain348);
  undertrack_owners50ew_fixture::BorrowedQueries q{};
  q.main690=&query;q.mainMembership=mainRow.state;q.mainStorage=mainRow.storage;q.mainRef=mainRow.ref;
  q.movement7A4=&movement;q.movementEndpoints=endpoints;q.movementMetadata=endpointScenes;q.movementSource=&movementSource;
  q.terrain348=&terrainOwner.initial;q.terrainResource=&terrainLiveOwner.resource;q.terrainMetadata=&terrainOwner.metadata;
  q.terrainMembership=terrainRow.state;q.terrainStorage=terrainRow.storage;q.terrainRef=terrainRow.ref;
  return q;
 };
 //The current racer's own 694/7A4 rows (shared world: that slot's pair;
 //legacy layout: the lent undertrack memberships).
 auto recoveryQueries=[&](){
  const auto podRow=trackStorage.currentRow(QueryRole50DU::pod694);
  const auto movementRow=trackStorage.currentRow(QueryRole50DU::movement7A4);
  return recoveryQueryOwners.binding(recoveryBorrowedQueries(),{podRow.ref,movementRow.ref},
      {podRow.state,movementRow.state},{podRow.storage,movementRow.storage});};
 auto publishRecoveryQueries=[&](){
  if(!originalRecoveryEnabled)return true;
  const auto podRow=trackStorage.currentRow(QueryRole50DU::pod694);
  const auto movementRow=trackStorage.currentRow(QueryRole50DU::movement7A4);
  if(!podRow||!movementRow)return false;
  if(!recoveryQueryOwners.sourcePrepared||!podRow.state->attached||
     !movementRow.state->attached||s.shape.registrationE0!=podRow.ref)return false;
  CollisionSphere48J sphere{};std::memcpy(sphere.center.v,s.shape.center60.v,sizeof sphere.center.v);sphere.radius=s.shape.radius80;
  const auto pod=projectCollectionObject50CF(recoveryQueryOwners.podFilter,sphere,query.collisionTypeTag,0,
      std::bit_cast<std::uint32_t>(s.shape.center60.v[3]));
  const auto ray=projectCollectionObject50CF(movement.filter,recoveryQueryOwners.movementSphere,
      movement.collisionTypeTag,recoveryQueryOwners.movementTag,
      std::bit_cast<std::uint32_t>(movementRow.state->bounds.cachedCenter.v[3]));
  if(!pod.valid||!ray.valid)return false;
  const CollectionProjection50CF* projection[]{&pod,&ray};
  const CollectionRef50BX refs[]{podRow.ref,movementRow.ref}; //row() proves 1..count.objects
  const track_runtime_storage50IG::RosterRowRef* rowRefs[]{&podRow,&movementRow};
  for(unsigned i=0;i<2;++i){const auto index=refs[i]-1u;collectionObjects[index]=projection[i]->object;
   if(!track_runtime_storage50IG::Storage::useRowFilter(*rowRefs[i],collectionObjects[index]))return false;
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
 // Common track-animation rows with collision (candidate k = collisionRows()[k]).
 // Static-candidate route only: the lane route (TB/TC/GB/MC worlds) publishes its rows through
 // the common binding (track_animation_binding50an.h), not through this storage.
 const auto trackAnimationRows=trackAnimations&&!trackAnimationBinding?track_animation_candidates50AN::collisionRows(*trackAnimations):std::vector<std::size_t>{};
 require50DU(trackAnimationRows.size()==trackAnimationMembership.size(),"track animation membership count differs from its collision rows");
 for(std::size_t k=0;k<trackAnimationRows.size();++k){const auto row=trackAnimationRows[k];
  auto& membership=*trackAnimationMembership[k];const auto bounds=track_animation_candidates50AN::bounds(*trackAnimations,row);
  const auto ref=static_cast<CollectionRef50BX>(animatedCandidateFirst+k+1);
  //2D93F0 ->120070(method2,radius0), then2D94B0 ->120230 after cold pose.
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,track_animation_candidates50AN::authoredZone(*trackAnimations,row),ZoneMethod50CL::Second,0.f,
      bounds,membershipWork),"track animation initial attachment"),"track animation zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"track animation cold update"),"track animation cold membership failed");
  trackAnimations->mutableState(row).collisionDirty=false;
  std::cerr<<"track_animation_collision course="<<trackAnimations->label()<<" row="<<(row+1)
      <<" registered=1 candidate="<<(animatedCandidateFirst+k)<<" zone="<<track_animation_candidates50AN::authoredZone(*trackAnimations,row)<<'\n';
 }
 // The track's trigger memberships (274E90 rows of its one trigger source,
 // refs triggerCandidateFirst+1..): 2D93F0 ->120070(method1,radius0), then the
 // cold update. One loop for every course. A row the collision world already
 // registered (same object) is only updated.
 if(const auto* triggers=track.roles.triggerSource.get()){
  require50DU(triggers->liveCount()==triggerMembership.size(),
      "trigger membership count differs from the track's trigger rows");
  for(std::size_t i=0;i<triggers->liveCount();++i){
   auto* slot=triggerMembership.at(i);
   require50DU(slot&&*slot,"trigger membership row unavailable");
   auto& membership=**slot;const auto& authored=triggers->source(i);
   const auto bounds=triggerBounds50DU(authored);
   const auto ref=static_cast<CollectionRef50BX>(triggerCandidateFirst+i+1);
   if(!membership.state.attached)require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
       membership.storage(),ref,authored.zone,ZoneMethod50CL::First,0.f,bounds,membershipWork),
       "authored trigger initial attachment"),"trigger zone attachment failed");
   else require50DU(membership.state.object==ref,"trigger membership attached to another object");
   require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
       bounds,membershipWork),"authored trigger cold update"),"trigger cold membership failed");
  }
 }
 auto refreshAnimatedCandidates=[&](){
  if(trackAnimations&&!trackAnimationBinding){
  // 11AD80 / 120230 refresh of every collision row; a row hidden with its collision off
  // (PORT EXTENSION (b), N64 data only) leaves the zone registry until it is shown again.
  track_animation_candidates50AN::sync(*trackAnimations,candidates.data(),candidates.size(),animatedCandidateFirst);
  for(std::size_t k=0;k<trackAnimationRows.size();++k){const auto row=trackAnimationRows[k];
   auto& state=trackAnimations->mutableState(row);auto& membership=*trackAnimationMembership[k];
   const auto bounds=track_animation_candidates50AN::bounds(*trackAnimations,row);
   if(!state.collisionWanted){
    if(membership.state.attached&&!reportMembership(removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,membershipWork),
       "track animation collision unregister"))return false;
   }else if(!membership.state.attached){
    if(!reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,membership.storage(),
       static_cast<CollectionRef50BX>(animatedCandidateFirst+k+1),track_animation_candidates50AN::authoredZone(*trackAnimations,row),
       ZoneMethod50CL::Second,0.f,bounds,membershipWork),"track animation collision re-register"))return false;
   }else{
    // PORT STAND-IN (flagged): 11ADC0/11AD20 re-home a wrapped looping row that left its
    // StartingZone; the ordinary membership update carries it (no stock row leaves its zone).
    const auto result=updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,bounds,membershipWork);
    if(!reportMembership(result,"track animation live update"))return false;
   }
   state.collisionDirty=false;state.rehomePending=false;
   const auto index=animatedCandidateFirst+k;auto& mirror=movementCandidates[index].collision;
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
 // Cold, live racer (slot 0) rows through the one storage accessor.
 const auto coldMainRow=trackStorage.row(0,QueryRole50DU::main690);
 const auto coldSecondaryRow=trackStorage.row(0,QueryRole50DU::secondary364);
 const auto coldTerrainRow=trackStorage.row(0,QueryRole50DU::terrain348);
 require50DU(bool(coldMainRow)&&bool(coldSecondaryRow)&&bool(coldTerrainRow),"actual cold 690/364/348 query rows unavailable");
 // Shared world: every slot's rows (slot 0 included) attach once, at that
 // slot's grid placement after its cold binding; nothing is inherited from a
 // pre-seal copy. Legacy layout (or no roster rig for slot 0): attach here.
 const bool coldInitialAttach=!(trackStorage.sharedLayout()&&playerUsesRosterRig);
 if(coldInitialAttach)require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,*coldMainRow.state,
     coldMainRow.storage,coldMainRow.ref,initialCollisionZone,ZoneMethod50CL::Second,0.0f,mainInitialBounds,membershipWork),
     "main initial attachment"),"attach main live spring membership");
 if(coldInitialAttach)require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,*coldSecondaryRow.state,
     coldSecondaryRow.storage,coldSecondaryRow.ref,initialCollisionZone,ZoneMethod50CL::Second,0.0f,secondaryInitialBounds,membershipWork),
     "secondary initial attachment"),"attach secondary live spring membership");

 TerrainPrepareInput48M coldTerrainInput{};
 coldTerrainInput.state=terrainOwner.initial;
 coldTerrainInput.scene=s.nodes[static_cast<int>(x.bindings.lifecycle.main190)].world100;
 coldTerrainInput.lowerEndpoint140=terrainOwner.lowerEndpoint140;
 TerrainPrepareState48M coldTerrainPrepared{};
 require50DU(prepareTerrainSinglePair48M(coldTerrainInput,coldTerrainPrepared)==TerrainPrepareStatus48M::QueryReady,
     "prepare initial authored terrain query bounds");
 const auto terrainInitialBounds=live_terrain_probe50fr::bounds(terrainLiveOwner,coldTerrainPrepared);
 if(coldInitialAttach)require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,*coldTerrainRow.state,
     coldTerrainRow.storage,coldTerrainRow.ref,initialCollisionZone,ZoneMethod50CL::Second,0.f,
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
 std::uint64_t reportedTriggerDispatches=0; // shared world observations, outside racer bank
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
   // The track's one trigger action owner (the collision world's, over
   // track.roles.triggers) records each prepared query pose for its shapes.
   if(auto borrow=track.roles.collision->borrow()){
    if(auto* actions=borrow.actions();actions&&actions->ready())require50DU(actions->observePrepared(
        source.filter->identity,*source.resource,source.worldMatrix),"trigger lifecycle query pose unavailable");
   }
  }
  if(auto borrow=track.roles.collision->borrow()){
   if(const auto* actions=borrow.actions();actions&&actions->nativeGroup()&&actions->nativeGroup()->dispatches!=reportedTriggerDispatches){
    const auto& group=*actions->nativeGroup();
    reportedTriggerDispatches=group.dispatches;
    // cr-18: name the trigger(s) this query's collider is now a member of (the dispatched
    // contact adds the pair after every action ran) instead of reading row 1 of the animation
    // table for every course: through cr-16 "opening_active" was GC's door row whatever
    // trigger dispatched (a LostAI entry by the secondary collider read as a failed door).
    // Every course, every trigger kind. Streams only (no allocation in the player frame).
    const std::uint32_t collider=source.filter?source.filter->identity:0u;
    const auto* rows=actions->sourceOwner();
    std::cerr<<"TRIGGER WORLD ACTION: dispatches="<<reportedTriggerDispatches
        <<" collider="<<(collider==secondaryFilter.identity?"secondary":"main")
        <<" animation_activations="<<actions->counters().animationActivations
        <<" policy=native_swept_type2_segments\n";
    for(std::size_t t=0;rows&&collider&&t<group.triggerCount&&t<group.triggers.size()&&t<rows->liveCount();++t){
     const auto& entry=group.triggers[t].collider;bool member=false;
     for(std::size_t k=0;k<entry.highWaterD0&&k<entry.membersCC.size();++k)member=member||entry.membersCC[k]==collider;
     if(!member)continue;
     const auto& authored=rows->source(t);
     std::cerr<<"trigger_dispatch course="<<track.code()<<" trigger="<<(t+1)<<" actions="<<authored.actions.size()<<" kinds=";
     for(std::size_t a=0;a<authored.actions.size();++a)
      std::cerr<<(a?"+":"")<<course_trigger_sources50CT::actionKindName(authored.actions[a].kind);
     std::cerr<<'\n';
     // Animation 101170: the action's target row, set active by fire(); the runtime's next
     // 101860 tick commits it and prints track_animation_started ... by=trigger<N>.
     for(const auto& action:authored.actions){
      if(action.kind!=course_trigger_sources50CT::ActionKind::Animation)continue;
      const auto row=static_cast<std::size_t>(action.animationIndex);
      const bool bound=trackAnimations&&row<trackAnimations->rowCount();
      const auto* state=bound?&trackAnimations->state(row):nullptr;
      std::cerr<<"trigger_animation_action course="<<track.code()<<" trigger="<<(t+1)<<" row="<<(row+1)
          <<" applied="<<(state&&!state->active90.empty()&&state->active90[0]==1?1:0)
          <<" pending_trigger="<<(state?state->pendingTrigger:0u)
          <<" stopped="<<(state?state->stopped10:1u)
          <<" same_runtime="<<(bound&&actions->animationOwner()==trackAnimations.get()?1:0)<<'\n';
     }
    }
   }
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
 collectionInput.zoneLinks=coldMainRow.state->storage.links;collectionInput.zoneLinkCount=coldMainRow.state->storage.capacity;collectionInput.zoneHead=1;collectionInput.query=coldMainRow.ref;
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
 //One concrete native policy replaces the frame driver's former literal
 //detector admissions. Its native lanes are registered by actual Storage;
 //the guard revokes borrows before this Context's settings/callbacks disappear.
 require50DU(!trackStorage.nativeDetectorPolicy_,"native detector policy already owned");
 auto nativeDetectorPolicy=std::shared_ptr<native_spring_detector_policy50LA::Owner>(
     new native_spring_detector_policy50LA::Owner(s,x.bindings,query,spring,settings,
         mainMembership,mainQueryRef,secondaryQuery,secondarySpring,secondarySettings,
         secondaryMembership,secondaryQueryRef,trackStorage.sharedLayout()));
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
 // ONE Follow bridge for every course (B-C18), over the track's storage and
 // its one ground scene.
 std::unique_ptr<original_follow_query_bridge::QueryBridge> followQueries;
 if(originalCameraEnabled)followQueries=std::make_unique<original_follow_query_bridge::QueryBridge>(
     trackStorage,ground,collisionNormalMode,query,qfilter,secondaryQuery,secondaryFilter,terrainOwner.metadata,
     originalRecoveryEnabled?&recoveryQueryOwners:nullptr,originalRecoveryEnabled?&s:nullptr,
     originalRecoveryEnabled?&movement:nullptr,maximumRecoveryMeshStack);
 auto* sourceFollowQueries=followQueries.get();
 if(originalResetGround&&sourceFollowQueries)
  require50DU(originalResetGround->bindCanonicalTailOwner(*sourceFollowQueries),
      "actual reset camera/wreck tail owner binding rejected");
 auto projectRegistered=[&](){
  //50CL keeps method descriptors current after each explicit registry change.
  for(std::size_t i=0;i<terrainOwnerCount;++i){const auto&owner=candidates[i].candidate.collision;auto projected=projectCollectionObject50CF(owner.filter,owner.sphere,owner.typeTag,candidates[i].shapeType,0);if(!projected.valid)return false;collectionObjects[i]=projected.object;collectionSource[i]=projected.state;}
  //Current racer's 690/364/348 rows through the one storage accessor.
  const auto mainRow=trackStorage.currentRow(QueryRole50DU::main690);
  const auto secondaryRow=trackStorage.currentRow(QueryRole50DU::secondary364);
  const auto terrainRow=trackStorage.currentRow(QueryRole50DU::terrain348);
  if(!mainRow||!secondaryRow||!terrainRow)return false;
  const auto& mainLive=*mainRow.state;const auto& secondaryLive=*secondaryRow.state;const auto& terrainLive=*terrainRow.state;
  if(!mainLive.attached||!secondaryLive.attached||
     mainLive.object!=mainRow.ref||secondaryLive.object!=secondaryRow.ref||
     mainLive.lastShapeTag!=2||secondaryLive.lastShapeTag!=2||
     mainLive.lastType2Resource!=query.resource||
     secondaryLive.lastType2Resource!=secondaryQuery.resource)return false;
  // Outer219AF0/219B14 consumes the prior accepted matrix/sphere and E0.
  //1223E4 calls1202E0 directly; fresh11FE50/11F680/120230 belongs only to
  // the later spring prefix. Secondary will reuse this exact collected list.
  LiveZoneBounds50EN collectionBounds{};collectionBounds.bounds.shapeTag=2;
  collectionBounds.bounds.shapeWorld=mainLive.lastShapeWorld;
  collectionBounds.bounds.cachedCenter=mainLive.bounds.cachedCenter;
  collectionBounds.bounds.cachedRadius=mainLive.bounds.cachedRadius;
  collectionBounds.type2Resource=query.resource;
  CollisionSphere48J mainCollectionSphere{},secondaryCollectionSphere{};
  for(unsigned i=0;i<3;++i){
   mainCollectionSphere.center.v[i]=mainLive.bounds.cachedCenter.v[i];
   secondaryCollectionSphere.center.v[i]=secondaryLive.bounds.cachedCenter.v[i];
  }
  mainCollectionSphere.radius=mainLive.bounds.cachedRadius;
  secondaryCollectionSphere.radius=secondaryLive.bounds.cachedRadius;
  const auto mainProjection=projectCollectionObject50CF(qfilter,mainCollectionSphere,query.collisionTypeTag,2,
      std::bit_cast<std::uint32_t>(collectionBounds.bounds.cachedCenter.v[3]));
  const auto secondaryProjection=projectCollectionObject50CF(secondaryFilter,secondaryCollectionSphere,
      secondaryQuery.collisionTypeTag,2,std::bit_cast<std::uint32_t>(secondaryLive.bounds.cachedCenter.v[3]));
  CollisionSphere48J terrainCollectionSphere{};
  for(unsigned i=0;i<3;++i)terrainCollectionSphere.center.v[i]=terrainLive.bounds.cachedCenter.v[i];
  terrainCollectionSphere.radius=terrainLive.bounds.cachedRadius;
  const auto terrainProjection=projectCollectionObject50CF(terrainOwner.metadata.filter,
      terrainCollectionSphere,terrainOwner.metadata.collisionTypeTag,2,
      std::bit_cast<std::uint32_t>(terrainLive.bounds.cachedCenter.v[3]));
  if(!mainProjection.valid||!secondaryProjection.valid||!terrainProjection.valid)return false;
  //Row refs are proven 1..count.objects by row(); these index collector backing of that size.
  collectionObjects[terrainRow.ref-1]=terrainProjection.object;
  collectionSource[terrainRow.ref-1]=terrainProjection.state;
  collectionObjects[mainRow.ref-1]=mainProjection.object;collectionSource[mainRow.ref-1]=mainProjection.state;
  collectionObjects[secondaryRow.ref-1]=secondaryProjection.object;collectionSource[secondaryRow.ref-1]=secondaryProjection.state;
  using StorageS=track_runtime_storage50IG::Storage;
  if(!StorageS::useRowFilter(terrainRow,collectionObjects[terrainRow.ref-1])||
     !StorageS::useRowFilter(mainRow,collectionObjects[mainRow.ref-1])||
     !StorageS::useRowFilter(secondaryRow,collectionObjects[secondaryRow.ref-1]))return false;
  if(!publishRecoveryQueries())return false;
  collectionInput.zoneLinks=mainLive.storage.links;collectionInput.zoneLinkCount=mainLive.storage.capacity;
  collectionInput.zoneHead=1;collectionInput.bounds=collectionBounds.bounds;
  //The visited racer's own 690 object and main resource (shared world: the
  //descriptor is not banked; legacy: the same values its bank copy holds).
  collectionInput.query=mainRow.ref;collectionInput.type2Resource=query.resource;
  collectionInput.initialCurrentIdentity=query.initialQueryCurrentIdentity;
  //No end-candidate producer publishes a projection (see retireEndCandidateProducer).
  return true;
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
  //F8 copy-back through the one storage accessor; projectRegistered above
  //already refused an empty 690/364/348 row and publishRecoveryQueries 694/7A4.
  const auto mainRow=trackStorage.currentRow(QueryRole50DU::main690);
  const auto secondaryRow=trackStorage.currentRow(QueryRole50DU::secondary364);
  const auto terrainRow=trackStorage.currentRow(QueryRole50DU::terrain348);
  if(!mainRow||!secondaryRow||!terrainRow)return false;
  terrainOwner.metadata.filter.flagsF8=collectionSecond[terrainRow.ref-1].flagsF8;
  registeredCount=static_cast<std::size_t>(result.count);peakCollected=std::max(peakCollected,registeredCount);addCounter50DU(collectedCalls);
  for(std::size_t i=0;i<terrainOwnerCount;++i)candidates[i].candidate.collision.filter.flagsF8=collectionSecond[i].flagsF8;
  qfilter.flagsF8=collectionSecond[mainRow.ref-1].flagsF8;
  secondaryFilter.flagsF8=collectionSecond[secondaryRow.ref-1].flagsF8;query.initialQueryCurrentIdentity=result.second.currentIdentity;
  if(originalRecoveryEnabled){
   const auto podRow=trackStorage.currentRow(QueryRole50DU::pod694);
   const auto movementRow=trackStorage.currentRow(QueryRole50DU::movement7A4);
   if(!podRow||!movementRow)return false;
   recoveryQueryOwners.podFilter.flagsF8=collectionSecond[podRow.ref-1u].flagsF8;
   movement.filter.flagsF8=collectionSecond[movementRow.ref-1u].flagsF8;}
  if(sourceFollowQueries&&!sourceFollowQueries->publishCollection(collectionSecond,collectionInput.objectCount))return false;
  std::memcpy(treeSource,treeSecond,trackCounts.methods*sizeof(*treeSource));return true;
 };
 // PORT STAND-IN (common-path rule): the 219AF0..219C74 end-candidate producer
 // (shared roster projection, ra_shared_*_context50la.inl) ran only inside the
 // retired RA aggregate's runtime query owner (OwnershipBridge). No common
 // owner of that runtime exists yet, so no course builds the producer and the
 // race end has nothing to retire here. Original: 219AF0/219B14/219B1C on
 // every race, released before the World at race+74 (1A69A8..1A69C4).
 std::function<bool()> retireEndCandidateProducer=[]()->bool{
  std::cerr<<"end_producer_retired present=0\n";
  return true;
 };
 auto commitCandidateEffects=[&](const std::vector<TerrainCandidateEffects48J>&changed){
  if(!registeredCount)return true;
  if(changed.size()<registeredCount)return false;
  for(std::size_t i=0;i<registeredCount;++i)if(changed[i].identity!=candidates[registeredIndices[i]].candidate.collision.filter.identity)return false;
  for(std::size_t i=0;i<registeredCount;++i){auto&primary=candidates[registeredIndices[i]].candidate.collision;auto&movementView=movementCandidates[registeredIndices[i]].collision;primary.filter.flagsF8=changed[i].flagsF8;primary.copied40=changed[i].copied40;primary.copied50=changed[i].copied50;movementView.filter.flagsF8=changed[i].flagsF8;movementView.copied40=changed[i].copied40;movementView.copied50=changed[i].copied50;}
  return true;
 };
 // Each racer's engine-flame owners come from that racer's own pod files
 // (native_racer50HW::buildColdFlames over its own rig and draw assets); the
 // cold roster visit rebuilds them per slot. Slot 0's own owners here when its
 // rig is already built (B-P04: no POD01 default).
 rr::assets::PodFlameOwners50W coldFlame{};
 if(racerRigs[0]&&racerVisuals[0])
  // Name the pod in a refusal, as the per-slot roster visit does.
  coldRacerStage50DU(racerRigs[0]->podId,"flame construction",[&](){
   coldFlame=native_racer50HW::buildColdFlames(*racerRigs[0],racerVisuals[0]->drawAssets,x.bindings);});


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

 std::array<std::uint64_t,raceRacerCapacity> nativeParticleChildSequence{};
 // A9: the water racer particles exist exactly when the track's SCB names water.
 if(track.hasWater())require50DU(nativeParticleAuthoring.parse(effectSources).okay(),
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
 // The track's one lighting owner (SCB Lights/LightN, all light types and
 // flashers), prepared once by the track load at step 05.13 for every course.
 require50DU(track.roles.lights!=nullptr,"the track's lighting owner (05.13) is unavailable");
 auto& lightOwner=*track.roles.lights;
 gcLighting::Diagnostic lightDiagnostic{};
 // Session owners are deliberately OUTSIDE the transient native racer bank.
 // This count advances at actual graphics success, before later failure cuts.
 std::array<std::uint64_t,raceRacerCapacity> graphicsSequence{};
 std::array<NativeGcSelectedLightsCandidate,raceRacerCapacity> drawLights{};
 const auto nativeLightingPolicy=nativeSelectedLights?
     gcLighting::CachePolicy::nativeAuthoredZoneLightSelection:
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
  message<<"native selected-light "<<operation<<" track="<<track.code()
      <<" racer="<<racer<<" diagnostic-racer="<<lightDiagnostic.racer
      <<" frame="<<completedFrames<<" frame-phase="<<static_cast<unsigned>(s.phase)
      <<" owner-status="<<statusName(lightDiagnostic.status)
      <<'('<<static_cast<unsigned>(lightDiagnostic.status)<<')'
      <<" source=0x"<<std::hex<<lightDiagnostic.sourcePC<<std::dec
      <<" policy="<<static_cast<unsigned>(nativeLightingPolicy)
      <<" graphics-sequence="<<(racer<graphicsSequence.size()?graphicsSequence[racer]:0u)
      <<" draw-generation="<<lease.generation()
      <<" membership-status="<<static_cast<unsigned>(lightDiagnostic.membership.status)
      <<" membership-bounds="<<static_cast<unsigned>(lightDiagnostic.membership.boundsStatus)
      <<" membership-geometry="<<static_cast<unsigned>(lightDiagnostic.membership.geometryStatus)
      <<" membership-registry="<<static_cast<unsigned>(lightDiagnostic.membership.registryStatus)
      <<" crossed-portals="<<lightDiagnostic.membership.crossedPortals
      <<" reset-status="<<static_cast<unsigned>(lightDiagnostic.resetVisibility.status)
      <<" reset-source=0x"<<std::hex<<lightDiagnostic.resetVisibility.sourcePC<<std::dec
      <<" decode-kind="<<static_cast<unsigned>(lightDiagnostic.decode.kind)
      <<" decode-code="<<lightDiagnostic.decode.code
      <<" decode-offset="<<lightDiagnostic.decode.offset
      <<" decode-detail="<<lightDiagnostic.decode.detail;
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
  if(racer>=graphicsSequence.size()||!lightOwner.beginDraw(racer,s,x.bindings,
      graphicsSequence[racer],lease,lightDiagnostic))
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
 if(nativeSelectedLights)
  std::cerr<<"NATIVE SELECTED LIGHTS: track="<<track.code()
      <<" authored zone/portal topology and lights read from the course SCB (one prepare, step 05.13);"
      " native per-publication selection and host world-normal Lambert.\n";
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
 std::unique_ptr<SecondHumanInput50SV> secondHuman;
 if(originalAiHumanCount==2u){
  secondHuman=std::make_unique<SecondHumanInput50SV>();
  require50DU(secondHuman->bind(controlPresets),"player 2 control preset and event binding rejected");
 }
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
  const auto tracePad=[&](const rr::game::DevicePacket48Y& data,const platform::SelectedControlInput50KY& binding){
   static const bool enabled=[](){const char* v=std::getenv("RR_TEST_PAD_REBIND");return v&&!std::strcmp(v,"1");}();
   if(!enabled||!captureRun.unattended()||!live)return;
   const auto desc=initialDualShockDescriptors48Y();
   const int throttle=binding.sourceActions.action694;
   const std::array<int,4> sample{throttle<0?-1:data.bytes[desc.records[throttle].packetOffset],data.bytes[6],data.bytes[7],data.bytes[5]};
   static std::array<int,4> previous[2]={{-2},{-2}};
   if(sample!=previous[binding.player]){
    previous[binding.player]=sample;
    std::cerr<<"pad_binding_sample player="<<binding.player+1<<" throttle="<<sample[0]
        <<" steer="<<sample[1]<<" left_y="<<sample[2]<<" right_y="<<sample[3]<<'\n';
   }
  };
  if(allAiEnabled)return;
  if(secondHuman && actualRacerSlot==1u){
   const bool released=!input.focused||input.minimized||input.activityLost;
   require50DU(secondHuman->service(input.controllers[1],live&&!released,released,
       platform::selectedControllerCountdownHold50KY,secondHuman->state40,secondHuman->gate58),
       "player 2 controller sample and dispatch rejected");
   tracePad(secondHuman->packet,secondHuman->binding);
   return;
  }
  if(live&&player_ai_pilot50DU::enabled&&actualRacerSlot==0&&originalAi.physicalReset2255B0Pending){
   require50DU(executeOriginalManualReset&&executeOriginalManualReset(),"AI pilot player reset unavailable");
   originalAi.pilotNavigation=navigationProgress;
   originalAi.physicalReset2255B0Pending=false;
   std::cerr<<"player_ai_pilot reset slot=0 node="<<originalAi.pilotNavigation.reset.vehicleC80<<'\n';
  }
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
   tracePad(packet,humanControlInput);
  }else{
   // Observe the caller's already-sampled, policy-filtered input before composition.
   // An invalid/unbound descriptor is reported, never dereferenced or repaired.
   const auto resetCode=humanControlInput.manualResetCode;
   const bool resetCodeValid=resetCode>=0&&resetCode<static_cast<std::int32_t>(dualShockActiveButtons48Y);
   const auto resetMapping=resetCodeValid?keyMapping.buttons[static_cast<std::size_t>(resetCode)]:
       platform::KeyboardButtonMapping50DR{};
   const auto resetKey=platform::keyboard_bindings::physical(humanControlInput.twinEngine,23);
   const std::array<std::uint32_t,13> observation{{
       input.down[resetKey],input.pressed[resetKey],input.released[resetKey],
       static_cast<std::uint32_t>(live),static_cast<std::uint32_t>(packetLive),
       static_cast<std::uint32_t>(input.focused),static_cast<std::uint32_t>(input.minimized),
       static_cast<std::uint32_t>(input.activityLost),static_cast<std::uint32_t>(input.eventBudgetExhausted),
       static_cast<std::uint32_t>(humanControlInput.manualResetBound),static_cast<std::uint32_t>(resetCode),
       static_cast<std::uint32_t>(resetMapping.source),resetMapping.scancode}};
   const bool changed=!manualResetInputObservationValid||observation!=lastManualResetInputObservation;
   if(changed){
    std::cerr<<"original_manual_reset_input phase=before_compose keyboard_packet="<<(allAiKeyboardPackets+1u)
        <<" sampled_scancode="<<resetKey<<" down="<<unsigned(input.down[resetKey])
        <<" pressed="<<unsigned(input.pressed[resetKey])<<" released="<<unsigned(input.released[resetKey])
        <<" requested_live="<<live<<" prior_packet_live="<<packetLive
        <<" focused="<<input.focused<<" minimized="<<input.minimized
        <<" activity_lost="<<input.activityLost<<" event_budget_exhausted="<<input.eventBudgetExhausted
        <<" bound="<<humanControlInput.manualResetBound<<" descriptor="<<resetCode
        <<" mapping_valid="<<resetCodeValid<<" mapping_source="<<static_cast<unsigned>(resetMapping.source)
        <<" mapped_scancode="<<resetMapping.scancode<<'\n';
    lastManualResetInputObservation=observation;manualResetInputObservationValid=true;
   }
   ++allAiKeyboardPackets;
   // Opt-in input fixture, contained in this race's keyboard packet path.
   static const bool testBindings=[](){const char* v=std::getenv("RR_TEST_KEYBOARD_REBIND");return v&&std::strcmp(v,"1")==0;}();
   const auto* physicalKeys=&input.down;
   std::array<std::uint8_t,512> fixtureKeys;
   int phase=-1;
   static unsigned fixtureTicks=0;
   if(testBindings&&captureRun.unattended()&&live&&fixtureTicks<120){
    phase=static_cast<int>(fixtureTicks++/30);
    fixtureKeys.fill(0);
    if(phase==0)fixtureKeys[26]=1; // old W
    if(phase==1)fixtureKeys[12]=1; // new I
    if(phase==2)fixtureKeys[14]=1; // K, right engine on twin layout
    physicalKeys=&fixtureKeys;
   }
   std::array<std::uint8_t,512> drivingKeys;
   require50DU(platform::composeKeyboardPacket50DR({true,live},
       {platform::keyboard_bindings::drivingKeys(humanControlInput.twinEngine,*physicalKeys,drivingKeys),input.down.size()},keyMapping,
       platform::KeyboardUnavailablePolicy50DR::ReleaseAll,packetTemplate,packet,
       keyboardDiagnostic),"selected keyboard packet composition failed");
   packetLive=live;
   static int loggedPhase=-1;
   if(phase>=0&&phase!=loggedPhase){
    loggedPhase=phase;
    const auto descriptors=initialDualShockDescriptors48Y();
    const auto offset=humanControlInput.twinEngine?0:descriptors.records[humanControlInput.sourceActions.action694].packetOffset;
    std::cerr<<"keyboard_rebind_sample phase="<<phase<<" throttle_pressure="<<unsigned(packet.bytes[offset])
        <<" left_y="<<unsigned(packet.bytes[7])<<" right_y="<<unsigned(packet.bytes[5])<<'\n';
   }
   if(changed)std::cerr<<"original_manual_reset_input phase=after_compose keyboard_packet="<<allAiKeyboardPackets
       <<" packet_live="<<packetLive<<" disposition="<<static_cast<unsigned>(keyboardDiagnostic.disposition)
       <<" packet_byte2="<<unsigned(packet.bytes[2])<<" packet_byte3="<<unsigned(packet.bytes[3])<<'\n';
  }
 };

 auto applyCourseInput=[&](std::size_t racerSlot=0){
  require50DU(!originalUiActive,"Main-only frontend cannot invoke a driving controller");
  require50DU(!nativeOriginalFleet,"native Course path reached forbidden HOST course driver");
  ++allAiHostDriverCalls;
  rr::host::CourseDriverInput50GK command{};
  command.enabled=autoDriveEnabled;command.elapsedSeconds=.016f;
  command.pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
  command.worldVelocity=m.components.root.body.velocity;
  command.yawRate=m.components.root.worldAngularVelocityA0.v[1];
  command.previousNode=navigationProgress.vehicleC84;
  command.currentNode=navigationProgress.reset.vehicleC80;
  const bool newNode=!courseState.initialized||courseState.observedNode!=command.currentNode;
  const bool driving=rr::host::driveCourse50GK(aiView,courseLimits,courseTuning,
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
  const bool prepared=recovery50FI::preparePortalRecovery(mapView,navigationProgress,s,
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
 // The one race start (1A6AA0 fresh, 1AC8E0 restart) and the one end dispatch
 // (1A7DD0) of this race, for every course; publication binds them to the
 // counted race (common_race_start50cr.h, common_race_end50cr.h).
 // The Application holds a weak handle (bindRaceStart50CR), so the owner is shared.
 const auto raceStartOwner=std::make_shared<common_race_start50CR::Owner>(originalMainApplication);
 auto& raceStart=*raceStartOwner;
 common_race_end50CR::Owner raceEnd{originalMainApplication};
 // Continue synchronously: suffix-owned Bank/window/gameplay resources retire
 // before these prefix locals, including on exceptional unwind.
 interactive_continuation50DU::Context continuation50DU{
  .track=track,
  .laps=laps,
  .roster=roster,
  .raceStart=raceStart,
  .raceEnd=raceEnd,
  .raceStartOwner=raceStartOwner,
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
  .closeCurrentCollisionCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeCurrentCollision},
  .closeCurrentMembershipsCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{closeCurrentMemberships},
  .coldFlame=coldFlame,
  .coldLoadDiagnostics=coldLoadDiagnostics,
  .collectRegisteredCall=interactive_frame50DU::BorrowedCall<bool()>{collectRegistered},
  .collectedCalls=collectedCalls,
  .collisionNormalMode=collisionNormalMode,
  .commitCandidateEffectsCall=interactive_frame50DU::BorrowedCall<bool(const std::vector<rr::game::TerrainCandidateEffects48J>&)>{commitCandidateEffects},
  .completedFrames=completedFrames,
  .composePacketCall=interactive_frame50DU::BorrowedCall<void(bool,const rr::platform::WindowInput50DQ&,std::size_t)>{composePacket},
  .consumeEffectsCall=interactive_frame50DU::BorrowedCall<bool(const rr::game::ImpactEffectEvent50B*,std::size_t)>{consumeEffects},
  .consumedStage=consumedStage,
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
  .rootedCourseRace=rootedCourseRace,
  .courseRaceLaunched=courseRaceLaunched,
  .courseState=courseState,
  .courseTuning=courseTuning,
  .destination=destination,
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
  .trackAnimationMembership=trackAnimationMembership,
  .trackAnimations=trackAnimations,
  .trackAnimationBinding=trackAnimationBinding,
  .trackAnimationBindingDiagnostic=trackAnimationBindingDiagnostic,
  .trackAnimationPrefixArmed=trackAnimationPrefixGuard.armed,
  .trackAnimationDiagnostic=trackAnimationDiagnostic,
  .triggerAudio=triggerAudio,
  .triggerMembership=triggerMembership,
  .gatherRegisteredCall=interactive_frame50DU::BorrowedCall<bool()>{gatherRegistered},
  .aiView=aiView,
  .collisionCleanupDone=collisionCleanupDone,
  .mapView=mapView,
  .remainingPayloadBytesCall=interactive_frame50DU::BorrowedCall<std::size_t()>{remainingPayloadBytes},
  .drawLights=drawLights,
  .graphicsSequence=graphicsSequence,
  .lightDiagnostic=lightDiagnostic,
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
  .keyboardDiagnostic=keyboardDiagnostic,
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
  .mainLiveQuery=mainLiveQuery,
  .mainMembership=mainMembership,
  .mainMembershipStorage=mainMembershipStorage,
  .mainQueryRef=mainQueryRef,
  .managerRuntimeSelection=managerRuntimeSelection,
  .materialAudio=track.roles.audio,  // the one material provider (05.12)
  .materialOwner=materialOwner,
  .waterUpdateSequence=waterUpdateSequence,
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
  .nativeLightFailureCall=interactive_frame50DU::BorrowedCall<void(const char*,std::uint32_t,const rr::game::original_gc_gameplay_lighting_candidate::DrawLease&)>{nativeLightFailure},
  .nativeLightingPolicy=nativeLightingPolicy,
  .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
  .nativeOriginalFleet=nativeOriginalFleet,
  .nativeParticleAuthoring=nativeParticleAuthoring,
  .nativeParticleChildSequence=nativeParticleChildSequence,
  .nativeParticleChildren=nativeParticleChildren,
  .nativeRacerManager=nativeRacerManager,
  .nativeRecordActor=nativeRecordActor,
  .nativeSelectedLights=nativeSelectedLights,
  .nativeVideo=nativeVideo,
  .nativeWaterApplication=nativeWaterApplication,
  .nativeWaterRacers=nativeWaterRacers,
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
  .retireEndCandidateProducer=retireEndCandidateProducer,
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
  .requireRuntimeNavigationCall=interactive_frame50DU::BorrowedCall<void()>{requireRuntimeNavigation},
  .reserveAdditionalPayloadCall=interactive_frame50DU::BorrowedCall<void(std::size_t,const char*)>{reserveAdditionalPayload},
  .resetEffectBindings=resetEffectBindings,
  .resetEffectState=resetEffectState,
  .resetEffectWorkspace=resetEffectWorkspace,
  .resetGlobals=resetGlobals,
  .resetHumanCamera=resetHumanCamera,
  .resetLifecycle=resetLifecycle,
  .response=response,
  .retireAudioOwnersCall=interactive_frame50DU::BorrowedCall<bool() noexcept>{retireAudioOwners},
  .s=s,
  .sampleControllerCall=interactive_frame50DU::BorrowedCall<void(bool)>{sampleController},
  .membershipCleanupDone=membershipCleanupDone,
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
  .selectedMapCountCall=interactive_frame50DU::BorrowedCall<std::uint32_t()>{selectedMapCount},
  .selectedMapIndexCall=interactive_frame50DU::BorrowedCall<std::uint8_t(std::uint32_t)>{selectedMapIndex},
  .selectedMapNodeCall=interactive_frame50DU::BorrowedCall<const rr::game::UndertrackNode50ET&(std::uint32_t)>{selectedMapNode},
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
  .secondHumanInput50SV=secondHuman.get(),
 };
 return continuation50DU.run();
}
}

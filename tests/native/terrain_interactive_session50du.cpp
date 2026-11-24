// Initial fixture/corpus/audio prefix only; world owners live in the next stage.
#include "native_last_failure50pd.h"
#include "terrain_interactive_session50du.h"
#include "terrain_interactive_world50du.h"
#include "terrain_interactive_loop_types50du.h"
#include "runtime_allocation_counter50fl.h"
#include "../../src/game/native_zero_gravity50zg.h"
#include "type2_fixture_io50i.h" // Sole non-inline reader implementation provider.
#include "runtime_startup50fl_fixture.h"
#include "runtime_asset_paths50lb.h"
#include "cold_load_diagnostics50du.h"
#include "track_session_assets50ig_fixture.h"
#include "original_ui_main_entry50kg.h"
#include "player_loading_session50du.h"
#include "original_course_race_targets50kx.h"
#include "original_racer_manager_entity50la.h"
#include "original_subtype3_record_owner50la.h"
#include "native_ai_session50iv_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "original_disc_query_bootstrap50le.h"
#include "live_audio50hf_fixture.h"
#include "lap_drive50gj_fixture.h"
#include "phase_timing50gk_fixture.h"
#include "render_diagnostics50gq_fixture.h"
#include "windows_framebuffer_capture50ld.h"
#include "../../src/game/ai_cold_owners50iu.h"
#include "../../src/game/original_racer_selection50ij.h"
#include "../../src/game/dlc_rider_entries50dl.h"
#include "../../src/game/original_ui_audio_options50kg.h"
#include "../../src/game/dynamic_impact_events50iw.h"
#include "../../src/assets/impact_authoring49y.h"
#include "../../src/game/damage_event5_subscriber50id.h"
#include "../../src/game/original_human_finish50kg.h"
#include "../../src/game/original_racer_model_view50la.h"
#include "../../platform/linux/native/selected_control_input50ky.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include "../../platform/linux/native/host_track_selection50im.h"
#include "../../platform/linux/native/game_host50er.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
namespace f=runtime_startup50fl;
using namespace rr::game;
using J=nlohmann::json;
namespace cold=cold50i;
namespace recovery50FI=checkpoint_recovery50fi_fixture;
using interactive_loop50DU::RacerContactTotals50HV;
using interactive_loop50DU::RacePresentation50HZ;
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
}
SessionExit50DU runSession50DU(const char* corpusPath,bool headlessLive,const char* capturePath,
    const recovery50FI::Plan* checkpointInput,recovery50FI::Plan& checkpointOutput,
    lap_drive50GJ::Continuity& lapContinuity,std::uint64_t sessionIndex,
    HostMonotonicClock50ID& applicationClock,
    rr::game::CollisionNormalModeOwner50JA& collisionNormalMode,bool startAtMenu,std::uint32_t& selectedRacer60,
    const rr::game::RaceAudioSettings50IC& effectiveAudioSettings,
    rr::game::RaceAudioSettings50IC& selectedAudioSettings,
    rr::platform::HostTrack50IM effectiveTrack,rr::platform::HostTrack50IM& selectedTrack,
    const rr::platform::HostTrackAdmission50IM& trackAdmission,
    const original_main_entry50KG::Application& originalMainApplication,
    bool enterOriginalUi,ControlPresets49D& controlPresets,SharedRandomOwner50ID& directApplicationRandom,
    const std::int32_t& cameraStartup350,original_course_targets50KX::ColdLoadRequest& courseColdLoad,
    const std::shared_ptr<native_gc_water_session50KG::Application>& nativeWaterGlobals,windows_capture50LD::Run& captureRun,
    original_course_targets50KX::ConsumedLaunch* consumedLaunch,const SessionServices50DU& services){
 auto& monotonicNow50DU=*services.monotonicNow50DU;
 auto& reportHost50DU=*services.reportHost50DU;
 auto& reportRenderer50DU=*services.reportRenderer50DU;
 auto& nativeWaterApplication=*nativeWaterGlobals;
 // Declare before every race/camera/radar borrower. Reverse destruction
 // retires the distinct record child before its actual racer-manager parent.
 std::unique_ptr<original_racer_manager_entity::Owner> nativeRacerManager;
 std::unique_ptr<original_subtype3_record::Owner> nativeRecordActor;
 rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection managerRuntimeSelection{};
 const bool originalUiStarted=originalMainApplication.enabled;
 bool originalUiActive=originalUiStarted&&enterOriginalUi,courseRaceLaunched=false;
 // Capture the immutable loaded identity before any front-menu choice changes
 // the next-session selection. The alternate-player integration consumes it.
 // PORT EXTENSION, NOT ORIGINAL (DLC phase 2): at a consumed free-play Course
 // launch the port-owned add-on rider selection (never in packed1125D or the
 // save) names the racer that races; the profile keeps its stock rider, and
 // every profile comparison below reads it back through profileRider().
 rr::game::dlc50DL::applyTestAddonRider();
 const std::uint32_t playerRacer60=[&]()->std::uint32_t{
  rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection launched{};
  if(!consumedLaunch||!originalMainApplication.riderProfile||
     !originalMainApplication.riderProfile->courseRaceSnapshot(launched)){
   rr::game::dlc50DL::clearRaceRider();return selectedRacer60;
  }
  return rr::game::dlc50DL::latchRaceRider(selectedRacer60,launched.mode34==0||launched.mode34==1,
      rr::platform::hostTrackCode50IM(effectiveTrack));
 }();
 const auto loadedTrack=effectiveTrack;
 require50DU(rr::platform::validHostTrack50IM(loadedTrack),"unsupported loaded track identity");
 ColdLoadDiagnostics50DU coldLoadDiagnostics(originalUiStarted&&!enterOriginalUi,
     rr::platform::hostTrackCode50IM(loadedTrack),playerRacer60,sessionIndex+1);
 coldLoadDiagnostics.begin("source_bootstrap");
 const char* trackSelectionError=nullptr;
 rr::platform::loading_progress50DU::pulse("Reading original track");
 const auto* selectedTrackAssets=track_session_assets50IG::select(rr::platform::hostTrackCode50IM(loadedTrack),trackSelectionError);
 require50DU(selectedTrackAssets,trackSelectionError?trackSelectionError:"original track admission failed");
 track_session_assets50IG::ResolvedSessionAssets resolvedTrackAssets(*selectedTrackAssets);
 const auto* trackSession=&resolvedTrackAssets.assets;
 // Common rewrite (I1, B-C01/B-C02): ONE entry for every course. The course code
 // is data (file names, receipts, the A1 PHY-class compare); the source says only
 // where the files come from (stock disc or DLC registry row, A12). No flag here
 // names a course: the twelve per-course track flags, the "common route" pair,
 // the thirteen per-course original-disc flags and the direct-human OR are deleted.
 const std::string_view courseCode(trackSession->world->code);
 const bool registryCourse=trackSession->registry;
 require50DU(!registryCourse||loadedTrack==rr::platform::HostTrack50IM::Course,
     "registry course row reached the session without its registry launch identity");
 // The race's one Track identity (187330 name): where this course's files are.
 // R3 fills it from the course row, the same fields for a stock or registry row.
 common_track50CR::Identity raceIdentity;
 {
  std::string identityError;
  require50DU(common_track50CR::identityForCourse(courseCode,registryCourse,raceIdentity,identityError),
      identityError.empty()?"course code has no track identity (stock or registry row)":identityError.c_str());
 }
 // A real Course consumption may reach only this next cold session, matched by
 // its course code as data (stock or registry row). Direct startup has no such
 // owner; this check does not activate a counted race.
 if(consumedLaunch)require50DU(originalMainApplication.enabled&&originalMainApplication.riderProfile&&
     consumedLaunch->registryCourse()==registryCourse&&
     consumedLaunch->matchesCourse(*originalMainApplication.riderProfile,sessionIndex+1,courseCode,playerRacer60),
     "consumed Course launch lost its same-profile next-session identity");
 const bool originalDisc=!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 // One rule for every course (B-C02; original 17C16C / 17D678 -> 1A65C0): the
 // race is launched through the rooted original Course entry, reading the
 // original disc. A human Player0 field and the retained ALL8 spectator field
 // (humans34 == 0) take the same entry; only the field differs.
 const bool rootedCourseRace=originalUiStarted&&consumedLaunch!=nullptr&&originalDisc;
 const bool directOriginalInputs=rootedCourseRace;
 // Collision ownership is separate from the legacy all-pass/render-policy flag.
 // An original-disc race takes its static candidates from the track's own files,
 // every course; the legacy corpus candidates are retired (W0 deleted them).
 const bool sourceStaticCandidates=trackSession->originalStaticCandidates||directOriginalInputs;
 // A direct original race reads the course archive only. A row that names a
 // development tuning file outside the archive reads the archive's own
 // tracks\<code>.tun instead, as the original does for every course (keyed on the
 // row's tuningInArchive data, never on the course).
 const std::string archiveTuningMember="tracks\\"+raceIdentity.code.lower()+".tun";
 if(directOriginalInputs&&!resolvedTrackAssets.assets.tuningInArchive&&!resolvedTrackAssets.assets.tuningArchive){
  resolvedTrackAssets.assets.tuningInArchive=true;
  resolvedTrackAssets.assets.impactTuning=archiveTuningMember.c_str();
 }
 // Clearing legacy path descriptors prevents accidental generated visual/sky fallback.
 if(directOriginalInputs){
  resolvedTrackAssets.assets.visualRoot="";resolvedTrackAssets.assets.visualMesh="";
  resolvedTrackAssets.assets.visualReport="";resolvedTrackAssets.assets.skyTextureRoot="";
 }
 std::cerr<<"course_entry code="<<courseCode<<" source="<<common_race_log50CR::sourceName(raceIdentity.source)
     <<" archive="<<trackSession->archive<<" listed_bytes="<<trackSession->archiveBytes
     <<" rooted="<<(rootedCourseRace?1:0)<<" session="<<sessionIndex+1<<'\n';
 require50DU(!rootedCourseRace||(!checkpointInput&&!startAtMenu),
     "original Course race requires a fresh ordinary cold race (RR_START_MENU=0, no checkpoint)");
 // B-C03: the GC- and RA-named environment switches (host triggers, native
 // gate, native water, incomplete world, RA native world) are deleted; no
 // course-independent replacement is needed (water and triggers follow data).

 // Original Course enters a fresh race after the frontend has committed
 // Options and retired its local owners. Read the sole application profile
 // here, before liveAudio->prepare initializes the actual shared groups.
 // Host requested/effective settings are only the separate direct-host path.
 const auto loadedAudioSettings=[&](){
  if(!originalUiStarted)return effectiveAudioSettings;
  rr::game::OriginalUiAudioOptions50KG committed;
  require50DU(originalMainApplication.gameSettingsProfile&&
      rr::game::readOriginalUiAudioOptions50KG(*originalMainApplication.gameSettingsProfile,committed),
      "original race entry requires known committed audio profile fields");
  const auto& mode=originalMainApplication.soundMode;
  require50DU(mode&&mode->ready()&&rr::game::validOriginalSoundModeFields50KG(mode->fields())&&
      static_cast<std::uint32_t>(mode->fields().profile1101C)==committed.soundMode1101C,
      "original race entry lost its committed application sound-mode owner");
  return committed.volumes;
 }();
 const auto* selectedOriginal=original_racer_selection50IJ::findAnyByRacer60(playerRacer60);
 require50DU(selectedOriginal&&selectedOriginal->humanJoinImplemented,"unsupported selected racer identity");
 require50DU(std::fesetround(FE_TONEAREST)==0,"round-to-nearest setup failed");
 const char* raceSetting=std::getenv("RR_RACE");
 const bool raceEnabled=!(raceSetting&&std::string(raceSetting)=="0");
 // B-C02: every race launches from the rooted Course entry. The former direct
 // host races (host start menu, legacy corpus TA) had no consumed launch; the
 // one stage (original_consumed_stage50DU::Owner::begin) needs that launch.
 require50DU(!raceEnabled||rootedCourseRace,
     "a race launches only from the rooted original Course entry (original disc, Main/Course, consumed launch)");
 // One authored level-illumination rule for every course: on for a fresh race,
 // RR_NATIVE_SELECTED_LIGHTS=0 is the single diagnostic opt-out (the former
 // per-course RR_NATIVE_xx_SELECTED_LIGHTS switches and the 13 nativeXxSelection
 // aliases are retired; the lighting owner keeps one storage layout, R6).
 const char* selectedLightingSetting=std::getenv("RR_NATIVE_SELECTED_LIGHTS");
 require50DU(!selectedLightingSetting||std::string(selectedLightingSetting)=="0"||
     std::string(selectedLightingSetting)=="1","RR_NATIVE_SELECTED_LIGHTS must be 0 or 1");
 const bool freshLevelLighting=raceEnabled&&!checkpointInput&&(!originalUiActive||rootedCourseRace);
 const bool nativeSelectedLights=selectedLightingSetting?std::string(selectedLightingSetting)=="1":freshLevelLighting;
 require50DU(!nativeSelectedLights||freshLevelLighting,
     "native selected lights require a fresh authenticated race; checkpoint/Main unsupported");
 const char* allAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool allAiEnabled=allAiSetting&&std::string(allAiSetting)=="1";
 // Native device selection never changes original player/AI classes. ALL8
 // bypasses both human packet paths even if a physical device is attached.
 const char* inputDeviceSetting=std::getenv("RR_HUMAN_INPUT_DEVICE");
 require50DU(!inputDeviceSetting||std::string(inputDeviceSetting)=="keyboard"||
     std::string(inputDeviceSetting)=="controller","RR_HUMAN_INPUT_DEVICE must be keyboard or controller");
 const bool usePhysicalGameController=!allAiEnabled&&(inputDeviceSetting?
     std::string(inputDeviceSetting)=="controller":originalMainApplication.courseHumanController);
 require50DU(!usePhysicalGameController||!headlessLive,"physical game controller requires the native SDL window");
#if defined(_WIN32)
 std::cerr<<"selected_input_mode="<<(allAiEnabled?"all_ai":usePhysicalGameController?"controller":"keyboard")
     <<" session="<<sessionIndex+1<<" explicit_override="<<(inputDeviceSetting?1:0)<<'\n';
#endif
 constexpr std::uint32_t deviceIndex=0,playerSource5C=0,playerActionIndexFFC=0;
 rr::platform::SelectedControlInput50KY humanControlInput{};
 // Apply the selected player's original action row and engine configuration together.
 if(!enterOriginalUi){
  for(unsigned slot=0;slot<2;++slot){
   const char* scheme=std::getenv(slot?"RR_PLAYER2_SCHEME":"RR_PLAYER1_SCHEME");
   if(scheme&&scheme[0]>='0'&&scheme[0]<='7'&&!scheme[1])
    require50DU(selectControlPreset49D(controlPresets,slot,scheme[0]-'0'),"launcher control scheme refused");
  }
 }
 require50DU(allAiEnabled||rr::platform::bindSelectedControlInput50KY(controlPresets,
     playerActionIndexFFC,playerSource5C,humanControlInput),"selected human preset/configuration is outside the exposed original rows");
 // PC enhancement (native_zero_gravity50zg.h): the player's zero-g lean is
 // bound in the default control preset only (right stick X; keyboard A/D).
 rr::game::zeroGravity50ZG.setHumanLeanBound(!allAiEnabled&&humanControlInput.zeroGLean);
 std::cerr<<"zero_g_lean bound="<<(!allAiEnabled&&humanControlInput.zeroGLean?1:0)
     <<" preset="<<humanControlInput.preset<<'\n';
 const char* originalAiSetting=std::getenv("RR_ORIGINAL_AI_FIRST_SLICE");
 // Product Course owns one real Player and seven original AI (or the ALL8
 // field). Every accepted human Course request cold-reloads, even the same selection.
 const bool humanCourseEnabled=rootedCourseRace&&!allAiEnabled;
 require50DU(!captureRun.enabled()||(!headlessLive&&raceEnabled&&(allAiEnabled||humanCourseEnabled)),
     "Window track capture requires a real window and an admitted original human or all-AI race");
 require50DU(!rootedCourseRace||!originalAiSetting||std::string(originalAiSetting)!="1",
     "rooted Course race forbids the legacy first-slice AI override");
 const bool nativeOriginalFleet=allAiEnabled||humanCourseEnabled;
 const bool originalAiEnabled=nativeOriginalFleet||(originalAiSetting&&std::string(originalAiSetting)=="1");
 const char* dynamicContactSetting=std::getenv("RR_ORIGINAL_AI_DYNAMIC_CONTACT");
 const bool dynamicContactEnabled=nativeOriginalFleet||(dynamicContactSetting&&std::string(dynamicContactSetting)=="1");
 require50DU(!dynamicContactEnabled||originalAiEnabled,
     "dynamic source contacts require the bounded original AI session");
 const std::uint32_t originalAiHumanCount=allAiEnabled?0u:
     (originalMainApplication.profile&&originalMainApplication.profile->mode34==1?2u:1u);
 constexpr std::size_t originalAiSlot=1;
 const auto originalAiOwnsSlot=[&](std::size_t slot) noexcept {
  return originalAiEnabled&&(allAiEnabled||(humanCourseEnabled?slot>=originalAiHumanCount:slot==originalAiSlot));
 };
 // One admission for every course (B-C02), replacing the per-course require50DU
 // admissions: a fresh rooted race in mode2 with the one native cold request.
 require50DU(!rootedCourseRace||(raceEnabled&&!checkpointInput&&!startAtMenu&&
     trackAdmission.gcMode2&&trackAdmission.nativeWorldRequested&&(humanCourseEnabled||allAiEnabled)),
     "the rooted Course race requires a fresh cold race in mode2 (Player0 plus seven AI, or the ALL8 field)");
 const auto originalAiCreateSelection=[&](){
  AiFreshCreateSelection50IU selected{};
  require50DU(!originalAiEnabled||(raceEnabled&&bindFreshAiCreateSelection50IU(
      trackSession->world->code,2,selected)),
      "original AI requires an authenticated owned fresh course/class/record selection");
  return selected;
 }();
 // Source21079C retains constructor a1;21081C stores vehicle60 and210820/24
 // formats the original Pod%02d resource prefix. Native bank slot0 is a role:
 // it always takes its own roster rig, whatever pod the player selected.
 const bool playerUsesRosterRig=true;
 const bool originalCameraEnabled=nativeOriginalFleet&&originalAiEnabled&&raceEnabled&&
     playerUsesRosterRig&&nativeSelectedLights&&!checkpointInput;
 // This bounded source2255B0 join requires the actual original-AI roster,
 // canonical7F0 visibility and selected original rig owners. Player recovery
 // and reset=false authored LostAI actions retain their separate behavior.
 const bool originalRecoveryEnabled=originalCameraEnabled;
 // A9: water is the track's own data. The original branches on whether the
 // SCB names water (274570 none / 2745D4 water factory, R7 prepareWater274570);
 // the counted race admits the water owner on every course and the data decides.
 // (B-C03/B-C16: the GC-named water and gate switches, the GC-only gate/water
 // selection and the GC incomplete-world block are deleted.)
 const bool nativeWaterEnabled=originalCameraEnabled;
 require50DU(!nativeWaterEnabled||(originalCameraEnabled&&rootedCourseRace),
     "track water/particles require the rooted fleet/source-camera world");
 require50DU(!rootedCourseRace||!raceEnabled||originalCameraEnabled,
     "the rooted Course race requires the actual recovered camera/visibility owner");
 require50DU(raceEnabled||!playerUsesRosterRig,"alternate original selection requires the native race");
 const std::string playerPodId(selectedOriginal->podId);
 const auto playerArchive=original_player_identity50IK::podArchive(
     runtime_asset_paths50LB::resolve("work/native/project/disc/PODS"),playerPodId);
 const char* raceModeSetting=std::getenv("RR_RACE_MODE");
 require50DU(!raceModeSetting||std::string(raceModeSetting)=="1"||std::string(raceModeSetting)=="2",
     "RR_RACE_MODE must be 1 (source events) or 2 (HOST fallback)");
 const bool mode1Requested=raceEnabled&&(!raceModeSetting||std::string(raceModeSetting)=="1");
 require50DU(!mode1Requested||trackSession->nativeMode1,
     "Selected original track record-row/selection configuration is unjoined; choose RR_RACE_MODE=2 explicitly");
 require50DU(!raceEnabled||rr::platform::hostTrackAdmitted50IM(loadedTrack,trackAdmission),
     "a race requires a supported course selection and RR_RACE_MODE=2");
 // Default mode1 is committed only after ALL required providers cold-prepare.
 // Failure is explicit; RR_RACE_MODE=2 is the selectable existing fallback.
 bool mode1Enabled=false;
 require50DU(!originalAiEnabled||!mode1Requested,
     "original AI uses its native manager; RR_RACE_MODE=1 selects the separate one-human adapter");
 const char* disableRacerContacts=std::getenv("RR_DISABLE_RACER_CONTACTS");
 const bool racerContactsEnabled=raceEnabled&&
     !(disableRacerContacts&&std::string(disableRacerContacts)=="1");
 require50DU(!originalAiEnabled||racerContactsEnabled,
     "original AI first slice requires actual racer contacts and their source boundary");
 // Course's immutable consumption was checked at entry, before1AD520
 // mutates the actual profile. (A restart is in place, 1AC8E0, and never
 // re-enters this cold entry.)
 if(humanCourseEnabled&&!originalUiActive){
  rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection before;
  require50DU(originalMainApplication.riderProfile&&
      originalMainApplication.riderProfile->courseRaceSnapshot(before),
      "racer manager requires the actual Course profile");
  if(before.subtype==3u){
   std::string error;
   nativeRacerManager=std::make_unique<original_racer_manager_entity::Owner>();
   require50DU(nativeRacerManager->construct(originalMainApplication,sessionIndex+1,error),error.c_str());
   require50DU(nativeRacerManager->initializeSelection(*originalMainApplication.riderProfile,
       before,managerRuntimeSelection),"actual racer manager1AD520 profile transition failed");
   require50DU(managerRuntimeSelection.rider==rr::game::dlc50DL::profileRider(playerRacer60)&&
       managerRuntimeSelection.entrants==1u&&nativeRacerManager->count30()==1u&&
       nativeRacerManager->humans34()==1u&&nativeRacerManager->opponents38()==0u,
       "time-trial manager did not produce the actual singleton roster");
  }
 }
 constexpr std::uint32_t raceRacerCapacity=rr::game::defaultHostRaceRacers50HT;
 constexpr std::uint32_t raceOpponentCapacity=raceRacerCapacity-1;
 // Capacity is not a fabricated live roster. Native Course uses its actual
 // committed human/opponent lanes; direct diagnostic clients keep eight.
 const std::uint32_t raceRacerCount=[&](){
  if(humanCourseEnabled&&!originalUiActive){
   rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
   require50DU(originalMainApplication.profile&&originalMainApplication.profile->known&&
       originalMainApplication.riderProfile&&originalMainApplication.riderProfile->courseRaceSnapshot(selected)&&
       selected.mode34==originalMainApplication.profile->mode34&&
       rr::game::nativeCourseMode50KG(selected)&&selected.entrants<=raceRacerCapacity&&
       (selected.subtype!=3||(nativeRacerManager&&selected.entrants==1u)),
       "native Course requires its actual committed live roster extent");
   return selected.entrants;
  }
  return raceRacerCapacity;
 }();
 const std::uint32_t raceOpponentCount=raceRacerCount-1;
 // Native selected roster, not the retail human-dependent boot root1AE620.
 // Counts use the original1125C layout; every AI retains its real pod/profile.
 const std::uint32_t originalAiOpponentCount=raceRacerCount-originalAiHumanCount;
 const std::uint16_t originalAiSettings1125C=[&]() -> std::uint16_t {
  // Actual mode0/mode2 human Course launch consumes the retained
  // source lap choice. Tournament mode2 has committed fixed three laps;
  // direct/ALL8 diagnostic clients retain their separate default3.
  if(humanCourseEnabled&&!originalUiActive){
   rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
   require50DU(originalMainApplication.profile&&originalMainApplication.profile->known&&
       originalMainApplication.riderProfile&&
       originalMainApplication.riderProfile->courseRaceSnapshot(selected)&&
       original_course_targets50KX::target(selected)&&
       original_course_targets50KX::target(selected)->track==loadedTrack&&
       selected.mode34==originalMainApplication.profile->mode34&&
       rr::game::nativeCourseMode50KG(selected)&&
       selected.rider==rr::game::dlc50DL::profileRider(playerRacer60)&&selected.subtype>=1&&selected.subtype<=3&&selected.laps>=1&&selected.laps<=25&&
       selected.entrants==raceRacerCount&&((selected.packed1125C>>5)&3u)==originalAiHumanCount&&
       ((selected.packed1125C>>7)&15u)==originalAiOpponentCount,
       "human Course lap configuration requires its actual retained profile");
   return static_cast<std::uint16_t>(selected.packed1125C&0x7ffu);
  }
  return static_cast<std::uint16_t>(3u|(originalAiHumanCount<<5u)|(originalAiOpponentCount<<7u));
 }();
 std::uint64_t allAiHostDriverCalls=0,allAiControllerSamples=0,allAiKeyboardPackets=0;
 if(raceEnabled){
  require50DU(!checkpointInput,"a race must restart every racer together from the grid");
  lapContinuity={};
 }
 const char* tracePhases=std::getenv("RR_TRACE_PHASES");
 phase_timing50GK::Interval phaseTiming{tracePhases&&std::string(tracePhases)=="1"};
 GameplayDisplayWallTotals50DU displayWallTotals{phaseTiming.enabled};
 const char* traceRender=std::getenv("RR_TRACE_RENDER");
 render_diagnostics50GQ::Interval renderDiagnostics{traceRender&&std::string(traceRender)=="1"};
 const char* captureWrongSetting=std::getenv("RR_CAPTURE_WRONG_WAY");
 const bool captureFirstWrongWay=headlessLive&&captureWrongSetting&&std::string(captureWrongSetting)=="1";
 bool capturedFirstWrongWay=false,lastCommittedWrongWay=false;
 // A rooted race constructs its bootstrap from authenticated original resources
 // in memory, every course. A non-race diagnostic session keeps its explicit
 // corpus query; no replacement JSON case or generated-file fallback is created.
 f::Fixture x(0,".",directOriginalInputs?f::AssetInput::OriginalDisc:f::AssetInput::RetainedDevelopment);
 auto&s=x.state;auto&m=s.bodies.main;
 const auto endpointPolicy=original_disc_query_bootstrap50LE::endpointPolicy();
 native_racer_collision50HW::Owner originalBootstrapCollision;
 J corpus;const J* chosen=nullptr;cold::Shape shape{};
 CollisionFilter48J qfilter{};Type2StaticQuery49X query{};
 std::vector<cold::Mesh> meshes;
 if(directOriginalInputs){
  // C2 (B-P04): the query bootstrap collision is slot 0's own pod, read from that
  // pod's own archive and member folder (Pods\PodNN\ for a stock pod, A5 Pod%02d;
  // the registry row's package and stem for an add-on pod). No pod is named here.
  std::string error;
  const auto bootstrapPodBytes=runtime_startup50fl::readBytes(playerArchive);
  const bool prepared=originalBootstrapCollision.prepare(
      {playerArchive.string(),original_player_identity50IK::podFolderMember(playerPodId)},
      bootstrapPodBytes,endpointPolicy,error);
  require50DU(prepared,error.empty()?"original slot-0 pod collision bootstrap rejected":error.c_str());
  original_disc_query_bootstrap50LE::initialize(originalBootstrapCollision.main(),qfilter,query,false);
 }else{
  std::ifstream file(corpusPath);require50DU(bool(file),"cannot open collision corpus");file>>corpus;
  for(const auto& item:corpus["cases"])if(item["name"]==trackSession->mainCollisionCase)chosen=&item;
  require50DU(chosen,"authored placement case present");const auto& row=*chosen;const auto& qj=row["query"];
  shape=cold::shape(corpus["shapes"][qj["shape"].get<std::size_t>()]);
  qfilter=cold::filter(qj["filter"]);query.resource=&shape.view;query.filter=&qfilter;query.queryScene.value=77;
  query.normalMode54=0u;query.normalMode54Bound=true;
  query.collisionTypeTag=qj["collision_tag"];query.groupTypeTag=qj["group_tag"];query.callbackEnabled=qj["callback"];
  cold::words(query.copied40.data(),qj["copied40"]);cold::words(query.copied50.data(),qj["copied50"]);
  query.skipMask=row["masks"][0];query.dynamicMask=row["masks"][1];query.materialMask=row["masks"][2];
 }
 const auto& mainQueryResource=*query.resource;
 std::vector<std::string> names;
 std::size_t stackCount=1;
 // The legacy corpus static candidates are retired (W0 removed them from the
 // world context): a race takes its candidates from the track's own files.
 require50DU(!raceEnabled||sourceStaticCandidates,
     "legacy corpus static candidates are retired; a race reads its track's own candidates");
 if(originalUiStarted){
  require50DU(originalMainApplication.state&&originalMainApplication.state->known,
      "native Course entry requires the actual application RNG owner");
  s.borrowedNoise50KG=&originalMainApplication.state->sharedNoise308030;
 }
 native_ai50IV::NoiseLifetime originalNoiseLifetime(s.noiseOwner(),originalAiEnabled&&!originalUiStarted);
 require50DU(originalNoiseLifetime.admitted,"Application noise has a terminal incomplete initialization prefix");
 AiRuntimeState50IV originalAi{};AiRuntimeReceipt50IV originalAiReceipt{};
 native_ai50IV::Session originalAiSession{};
 // The exact genuine mode-2 race owns its arm until commit or session teardown.
 // Main return and exceptions cannot leave a dangling manager pointer.
 struct TournamentResultLifetime50KG {
  rr::platform::OriginalUiRiderProfileOwner50KG* profile=nullptr;
  const LapTimingManager50ID* manager=nullptr;
  ~TournamentResultLifetime50KG(){if(profile&&manager)profile->cancelTournamentResultForRace(*manager);}
 } tournamentResultLifetime{originalMainApplication.riderProfile.get(),&originalAiSession.manager};
 rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection tournamentLaunchSelection{};
 rr::platform::OriginalUiRiderProfileStamp50KG tournamentLaunchStamp{};
 bool tournamentResultArmed=false,tournamentResultCommitted=false;
 AiTrackMetadata50IU originalAiMapMetadata{},originalAiMetadata{};
 AiTrackTuning50IU originalAiTuning{};
 rr::assets::AiAuthoringDiagnostic50IU originalAiAuthoringDiagnostic{};
 // Application-owned RNG selectors survive native cold-session rebuilds.
 // The direct AI stream is retained by NoiseLifetime; UI-launched sessions
 // continue borrowing the same real application stream and selector owners.
 // No restart, capture or menu entry reseeds or resets their lazy caches.
 SharedRandomOwner50ID legacySessionRandom{}; //Unchanged per-session owner for retained non-AI clients.
 auto& sessionRandom=originalUiStarted?originalMainApplication.state->sharedRandomSelectors:
     originalAiEnabled?directApplicationRandom:legacySessionRandom;
 // Shared source owners: never bank these along with the reused racer arena.
 // Fresh direct profile defaults; a Course restart projects its real retained
 // selection below. No imported save/cheat profile is manufactured.
 // The consumed track bits stay cold-clear only before the retained destruction
 // and menu boundaries. They are not reset on each contact.
 DynamicImpactProfile50IW dynamicProfile{};
 dynamicProfile.game34=3;dynamicProfile.game10FC4=0;dynamicProfile.game10FE0=0;
 dynamicProfile.track1B1=0;dynamicProfile.bound=dynamicContactEnabled;
 DynamicContactProfile50IW dynamicMusicProfile{1u,dynamicContactEnabled};
 if(originalUiStarted&&!originalUiActive){
  rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
  require50DU(rootedCourseRace&&originalMainApplication.riderProfile&&
      originalMainApplication.riderProfile->courseRaceSnapshot(selected)&&
      original_course_targets50KX::target(selected)&&
      original_course_targets50KX::target(selected)->track==loadedTrack&&
      selected.rider==rr::game::dlc50DL::profileRider(playerRacer60)&&originalMainApplication.profile&&
      selected.mode34==originalMainApplication.profile->mode34&&
      rr::game::nativeCourseMode50KG(selected)&&
      selected.subtype>=1&&selected.subtype<=3&&selected.laps==(humanCourseEnabled?(originalAiSettings1125C&31u):3u)&&selected.entrants==raceRacerCount&&
      ((selected.packed1125C>>5)&3)==originalAiHumanCount&&((selected.packed1125C>>7)&15)==originalAiOpponentCount&&
      (!humanCourseEnabled||(selected.packed1125C&0x7ffu)==originalAiSettings1125C),
      "native Course cold load requires its actual retained track/racer profile");
  dynamicProfile.game34=originalMainApplication.profile->mode34;
  dynamicProfile.game1125F=static_cast<std::uint8_t>(selected.course);
  dynamicMusicProfile.mode11258=selected.subtype;
 }

 // The original application settings owner supplies the actual vibration
 // bit to the existing impact-feedback/controller backend. Direct-host mode
 // retains its explicit fresh default.
 const std::uint32_t dynamicConsumerFlags11050=originalUiStarted?
     originalMainApplication.gameSettingsProfile->flags11050:4u;
 DynamicImpactTuning50IW dynamicTuning{};
 rr::assets::FinalImpactAuthoring49Y dynamicFinalTuning{};
 DynamicContactEventBindings50IW dynamicEventBindings{};
 DynamicContactMusicCursor50IW dynamicMusicCursor{};
 DynamicContactEventCursor50IW dynamicEventCursor{};
 DynamicContactPodEffect50IW dynamicPodEffect{};
 DamageEventCursor50ID dynamicDamageCursor{};
 LapEventCursor50ID dynamicLapCursor{};
 RaceEventQueue50ID<128> dynamicEvents{};
 native_dynamic_contact50IW::Registry dynamicRegistry;
 native_dynamic_contact50IW::Snapshot dynamicPeers{};
 std::size_t dynamicSelfSlot=0;
 using DynamicActors50IW=interactive_continuation50DU::DynamicActors50IW;
 DynamicActors50IW dynamicActors;
 std::array<std::uint32_t,raceRacerCapacity> dynamicRacer60{};
 const RaceActorResolver50ID dynamicActorResolver{&dynamicActors,
  [](void* opaque,RaceActorId50ID id,RaceActorSnapshot50ID& out)noexcept{
   const auto& actors=*static_cast<const DynamicActors50IW*>(opaque);
   if(!id||id>actors.values.size()||!actors.bound[id-1])return false;
   out=actors.values[id-1];return true;
  }};
 // Same owners are available before any RA counted Storage construction.
 // Merely declaring the open lambda does not create a window or activate a race.
 rr::platform::loading_progress50DU::pulse("Preparing source owners");
 auto nativeVideo=originalMainApplication.videoForSession();
 auto sourceDisplayLifetime=originalMainApplication.raceDisplayForSession50LA();
 auto liveAudio=std::make_unique<live_audio50HF::Owner>();
 // One shared-audio preparation state for every course (B-C17). The former
 // TB/GB early PodSound owners are retired: the track's trigger-driven sounds
 // are the one positional-sound owner over the track's sound rows (R7,
 // positional_sound_rows50SR, prepared in the 05.12 AUB step). The world stage
 // records here whether it already prepared the shared audio, its AUDIO root
 // and the bound sound mode, so later material/engine setup reuses them.
 bool liveAudioPrepared=false;
 std::filesystem::path preparedAudioRoot;
 std::shared_ptr<rr::platform::OriginalSoundMode50KG> soundMode;
 // Retain the actual accepted source event type; never relabel it as a normal race.
 std::uint32_t nativeOrdinaryLaunch11258=dynamicMusicProfile.mode11258;
 std::unique_ptr<OriginalRacerModelRenderGlobals50LA> sourceModelRenderGlobals;
 auto& host=nativeVideo->host;
 rr::platform::GameHostDiagnostic50ER hostDiagnostic{};
 constexpr int requestedWindowSamples=0;
 auto openNativeVideo=[&](){
  if(host.created()){require50DU(host.current(),"same native Video lost its current context");return;}
  rr::platform::GameHostRequest50ER hostRequest{};
  hostRequest.mode=headlessLive?rr::platform::GameHostMode50ER::HeadlessLive:rr::platform::GameHostMode50ER::Window;
  auto& windowRequest=hostRequest.window;
  windowRequest.title=raceEnabled?"Star Wars: Racer Revenge":
      "Star Wars: Racer Revenge";
  if(originalUiActive)windowRequest.title="Star Wars: Racer Revenge";
  windowRequest.width=nativeVideo->width;windowRequest.height=nativeVideo->height;
#if defined(_WIN32)
  // Presentation only: the retained source Video stays 640x448.
  if(!headlessLive&&nativeVideo->width==640&&nativeVideo->height==448){
   windowRequest.width=1280;windowRequest.height=896;
  }
#endif
  // GL4.0 core first: the UI AA1 coverage pass (Garage models) uses
  // gl_SampleID. Drivers that return exactly the requested version (AMD)
  // would otherwise give 3.2. 3.2 core remains the fallback for all races.
  windowRequest.major=4;windowRequest.minor=0;
  windowRequest.profile=rr::platform::WindowGlProfile50DQ::Core;
  windowRequest.samples=requestedWindowSamples;windowRequest.swapInterval=0;windowRequest.resizable=true;
  // Frontend navigation borrows actual SDL button edges independently of
  // human driving. ALL8 keeps usePhysicalGameController false. No second-pad
  // popup/presence capability or source racing action is admitted here.
  windowRequest.gameControllers=usePhysicalGameController||(originalUiStarted&&!headlessLive);
  if(originalUiActive){
   windowRequest.redBits=windowRequest.greenBits=windowRequest.blueBits=windowRequest.alphaBits=8;
   windowRequest.depthBits=24;windowRequest.stencilBits=0;
  }
  hostRequest.headless={rr::platform::OffscreenGlPlatform50DC::MesaSurfaceless,nativeVideo->width,nativeVideo->height,3,2,rr::platform::OffscreenGlProfile50DC::Core,8,8,8,8,24,0,0};
  if(!host.create(hostRequest,hostDiagnostic)){
   std::cerr<<"native_window_gl4_unavailable retrying=3.2\n";
   windowRequest.major=3;windowRequest.minor=2;hostDiagnostic={};
   if(!host.create(hostRequest,hostDiagnostic)){
    reportHost50DU(hostDiagnostic);throw std::runtime_error("window/context creation failed");
   }
  }
 };
 // B-C06: activation from launch data only, every course. Pause100 is built for every race
 // (1A8A40); a human race binds it through attachCountedRace (publication), and this flag is
 // the ALL8 spectator binding only: the original branches on humans34 == 0 (the observation
 // requires it), never on the course.
 const bool sourceConsumedPauseActivation=rootedCourseRace&&originalCameraEnabled&&allAiEnabled;
 // Intro cuts are decided by 174EA8 from launch data: subtype 2 and 3 have no
 // cuts, subtype 1 has them only when P+34 == 2 (tournament) and App+350 == 0.
 // The cut count itself is the TOB's (World+3E4, 174E58), read by the camera
 // session (R2); a course whose TOB authors no cameras takes the zero-cut path.
 const bool sourceConsumedIntroActivation=rootedCourseRace&&originalCameraEnabled&&
     nativeOrdinaryLaunch11258==1u&&dynamicProfile.game34==2u&&cameraStartup350==0;
 // The race's one Track (common_track50cr.h) and one roster (common_roster50cr.h).
 // The Track carries only the identity here; the world stage (I2) begins the
 // stage with it (01-02), loads it through 1A9EE0 (04-05), builds and loads the
 // roster (10). Declared after the audio/video owners so it retires before them.
 auto raceTrack=std::make_shared<common_track50CR::Track>();
 raceTrack->identity=std::move(raceIdentity);
 common_roster50CR::Owner raceRoster;
 // Actual world/collision ownership remains in the next lexical stage.
 // It retires before this original early fixture/rig/audio prefix, on all exits.
 interactive_world50DU::Context world50DU{
  .track=*raceTrack,
  .trackOwner=raceTrack,
  .roster=raceRoster,
  .allAiControllerSamples=allAiControllerSamples,
  .allAiEnabled=allAiEnabled,
  .allAiHostDriverCalls=allAiHostDriverCalls,
  .allAiKeyboardPackets=allAiKeyboardPackets,
  .applicationClock=applicationClock,
  .cameraStartup350=cameraStartup350,
  .captureFirstWrongWay=captureFirstWrongWay,
  .capturePath=capturePath,
  .captureRun=captureRun,
  .capturedFirstWrongWay=capturedFirstWrongWay,
  .checkpointInput=checkpointInput,
  .checkpointOutput=checkpointOutput,
  .chosen=chosen,
  .coldLoadDiagnostics=coldLoadDiagnostics,
  .collisionNormalMode=collisionNormalMode,
  .consumedLaunch=consumedLaunch,
  .controlPresets=controlPresets,
  .corpus=corpus,
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
  .endpointPolicy=endpointPolicy,
  .liveAudioPrepared=liveAudioPrepared,
  .preparedAudioRoot=preparedAudioRoot,
  .soundMode=soundMode,
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
  .mainQueryResource=mainQueryResource,
  .managerRuntimeSelection=managerRuntimeSelection,
  .meshes=meshes,
  .mode1Enabled=mode1Enabled,
  .mode1Requested=mode1Requested,
  .monotonicNow50DU=monotonicNow50DU,
  .names=names,
  .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
  .nativeOriginalFleet=nativeOriginalFleet,
  .nativeRacerManager=nativeRacerManager,
  .nativeRecordActor=nativeRecordActor,
  .nativeSelectedLights=nativeSelectedLights,
  .nativeVideo=nativeVideo,
  .nativeWaterApplication=nativeWaterApplication,
  .nativeWaterEnabled=nativeWaterEnabled,
  .nativeWaterGlobals=nativeWaterGlobals,
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
  .originalBootstrapCollision=originalBootstrapCollision,
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
  .qfilter=qfilter,
  .query=query,
  .raceEnabled=raceEnabled,
  .raceOpponentCount=raceOpponentCount,
  .raceRacerCount=raceRacerCount,
  .racerContactsEnabled=racerContactsEnabled,
  .renderDiagnostics=renderDiagnostics,
  .reportHost50DU=reportHost50DU,
  .reportRenderer50DU=reportRenderer50DU,
  .requestedWindowSamples=requestedWindowSamples,
  .s=s,
  .selectedAudioSettings=selectedAudioSettings,
  .selectedOriginal=selectedOriginal,
  .selectedRacer60=selectedRacer60,
  .selectedTrack=selectedTrack,
  .sessionIndex=sessionIndex,
  .sessionRandom=sessionRandom,
  .sourceConsumedIntroActivation=sourceConsumedIntroActivation,
  .sourceConsumedPauseActivation=sourceConsumedPauseActivation,
  .sourceDisplayLifetime=sourceDisplayLifetime,
  .sourceModelRenderGlobals=sourceModelRenderGlobals,
  .sourceStaticCandidates=sourceStaticCandidates,
  .stackCount=stackCount,
  .startAtMenu=startAtMenu,
  .tournamentLaunchSelection=tournamentLaunchSelection,
  .tournamentLaunchStamp=tournamentLaunchStamp,
  .tournamentResultArmed=tournamentResultArmed,
  .tournamentResultCommitted=tournamentResultCommitted,
  .trackAdmission=trackAdmission,
  .trackSession=trackSession,
  .usePhysicalGameController=usePhysicalGameController,
  .x=x,
 };
 return world50DU.run();
}

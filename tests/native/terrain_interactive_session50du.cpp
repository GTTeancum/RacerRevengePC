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
#include "tb_pod_sound50tb.h"
#include "gb_pod_sound50gb.h"
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
  return rr::game::dlc50DL::latchRaceRider(selectedRacer60,launched.mode34==0,
      rr::platform::hostTrackCode50IM(effectiveTrack));
 }();
 const auto loadedTrack=effectiveTrack;
 // A real Course consumption may reach only this next cold session. Direct
 // startup has no such owner; this check does not activate a counted race.
 if(consumedLaunch)require50DU(originalMainApplication.enabled&&originalMainApplication.riderProfile&&
     consumedLaunch->matches(*originalMainApplication.riderProfile,sessionIndex+1,loadedTrack,playerRacer60),
     "consumed Course launch lost its same-profile next-session identity");
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
 const bool isGcTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"GC");
 const bool isGaTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"GA");
 const bool isTaTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"TA");
 const bool isMaTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"MA");
 const bool isSaTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"SA");
 const bool isRaTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"RA");
 const bool isMbTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"MB");
 const bool isScTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"SC");
 const bool isTcTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"TC");
 const bool isRcTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"RC");
 const bool isGbTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"GB");
 const bool isMcTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"MC");
 const bool isTbTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"TB");
 const bool isMaSaTrack=isMaTrack||isSaTrack;
 // Direct human worlds use only authenticated original archives. Clearing
 // legacy path descriptors prevents accidental generated visual/sky fallback.
 // GC direct-human capability is entered by the rooted frontend. The
 // retained standalone ALL8 diagnostic remains a distinct legacy route.
 const bool gcOriginalDisc=isGcTrack&&originalUiStarted&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool gaOriginalDisc=isGaTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool taOriginalDisc=isTaTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool maOriginalDisc=isMaTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool saOriginalDisc=isSaTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool mbOriginalDisc=isMbTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool scOriginalDisc=isScTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool tcOriginalDisc=isTcTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool rcOriginalDisc=isRcTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool gbOriginalDisc=isGbTrack&&!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool mcOriginalDisc=isMcTrack&&!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const bool tbOriginalDisc=isTbTrack&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const char* raAllAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool raOriginalDisc=isRaTrack&&originalUiStarted&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty()&&
     !(raAllAiSetting&&std::string(raAllAiSetting)=="1");
 const bool directOriginalHuman=raOriginalDisc||gcOriginalDisc||gaOriginalDisc||taOriginalDisc||maOriginalDisc||saOriginalDisc||mbOriginalDisc||scOriginalDisc||tcOriginalDisc||rcOriginalDisc||tbOriginalDisc||gbOriginalDisc||mcOriginalDisc;
 const bool directOriginalInputs=isRaTrack||directOriginalHuman;
 // Collision ownership is separate from the legacy all-pass/render-policy flag.
 const bool sourceStaticCandidates=trackSession->originalStaticCandidates||taOriginalDisc;
 if(taOriginalDisc){
  resolvedTrackAssets.assets.tuningInArchive=true;
  resolvedTrackAssets.assets.impactTuning="tracks\\ta.tun";
 }
 if(directOriginalHuman){
  resolvedTrackAssets.assets.visualRoot="";resolvedTrackAssets.assets.visualMesh="";
  resolvedTrackAssets.assets.visualReport="";resolvedTrackAssets.assets.skyTextureRoot="";
 }
 const bool courseRaceCold=(isGcTrack||isGaTrack||isTaTrack||isMaSaTrack||isRaTrack||isMbTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack)&&originalUiStarted;
 require50DU(!(isMaSaTrack||isRaTrack||isMbTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack)||(!checkpointInput&&!startAtMenu),
     "MA/SA/RA require a fresh ordinary cold race; host startup menu/checkpoint unsupported");
 require50DU(!courseRaceCold||!startAtMenu,"original Course race entry requires RR_START_MENU=0");
 // GA follows the common direct-disc Course rule (its former GA-only host
 // admission and WatchtowerRun gate were port stand-ins, retired).
 require50DU(!(gaOriginalDisc||gcOriginalDisc||taOriginalDisc||maOriginalDisc||saOriginalDisc||mbOriginalDisc||scOriginalDisc||tcOriginalDisc||rcOriginalDisc||tbOriginalDisc)||
     (courseRaceCold&&!checkpointInput&&!startAtMenu),
     "direct-disc human route requires actual Main/Course ownership");
 // Retained HOST trigger source is dormant; native Gate has independent ownership.
 const char* hostTriggerSetting=std::getenv("RR_GC_HOST_TRIGGERS");
 require50DU(!hostTriggerSetting||std::string(hostTriggerSetting)=="0",
     "RR_GC_HOST_TRIGGERS is dormant and cannot replace original-AI/native Gate ownership");
 const char* gateSetting=std::getenv("RR_GC_NATIVE_GATE");
 require50DU(!gateSetting||std::string(gateSetting)=="0"||std::string(gateSetting)=="1",
     "RR_GC_NATIVE_GATE must be 0 or 1");

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
 // One authored level-illumination rule for every course: on for a fresh race,
 // RR_NATIVE_SELECTED_LIGHTS=0 is the single diagnostic opt-out (the former
 // per-course RR_NATIVE_xx_SELECTED_LIGHTS switches were retired).
 const char* selectedLightingSetting=std::getenv("RR_NATIVE_SELECTED_LIGHTS");
 require50DU(!selectedLightingSetting||std::string(selectedLightingSetting)=="0"||
     std::string(selectedLightingSetting)=="1","RR_NATIVE_SELECTED_LIGHTS must be 0 or 1");
 const bool freshLevelLighting=raceEnabled&&!checkpointInput&&(!originalUiActive||courseRaceCold);
 const bool nativeSelectedLights=selectedLightingSetting?std::string(selectedLightingSetting)=="1":freshLevelLighting;
 require50DU(!nativeSelectedLights||freshLevelLighting,
     "native selected lights require a fresh authenticated race; checkpoint/Main unsupported");
 // PORT STAND-IN (flagged): the lighting owner still keeps one storage layout
 // per course scene (topology capacities), so the scene is named by the loaded
 // course here. These are aliases of the one rule above, not separate switches.
 const bool nativeGcSelection=nativeSelectedLights&&isGcTrack;
 const bool nativeTaSelection=nativeSelectedLights&&isTaTrack;
 const bool nativeGaSelection=nativeSelectedLights&&isGaTrack;
 const bool nativeMaSelection=nativeSelectedLights&&isMaTrack;
 const bool nativeSaSelection=nativeSelectedLights&&isSaTrack;
 const bool nativeMbSelection=nativeSelectedLights&&isMbTrack;
 const bool nativeScSelection=nativeSelectedLights&&isScTrack;
 const bool nativeTcSelection=nativeSelectedLights&&isTcTrack;
 const bool nativeRcSelection=nativeSelectedLights&&isRcTrack;
 const bool nativeGbSelection=nativeSelectedLights&&isGbTrack;
 const bool nativeMcSelection=nativeSelectedLights&&isMcTrack;
 const bool nativeTbSelection=nativeSelectedLights&&isTbTrack;
 const bool nativeRaSelection=nativeSelectedLights&&isRaTrack;
 const char* allAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool allAiEnabled=allAiSetting&&std::string(allAiSetting)=="1";
 require50DU(!originalUiStarted||allAiEnabled||directOriginalHuman,
     "recovered original profile entry requires its owned direct human or retained all-AI route");
 require50DU(!directOriginalHuman||!allAiEnabled,"direct human Course must not replace Player0 with AI");
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
 require50DU(allAiEnabled||rr::platform::bindSelectedControlInput50KY(controlPresets,
     playerActionIndexFFC,playerSource5C,humanControlInput),"selected human preset/configuration is outside the exposed original rows");
 // PC enhancement (native_zero_gravity50zg.h): the player's zero-g lean is
 // bound in the default control preset only (right stick X; keyboard A/D).
 rr::game::zeroGravity50ZG.setHumanLeanBound(!allAiEnabled&&humanControlInput.zeroGLean);
 std::cerr<<"zero_g_lean bound="<<(!allAiEnabled&&humanControlInput.zeroGLean?1:0)
     <<" preset="<<humanControlInput.preset<<'\n';
 const char* originalAiSetting=std::getenv("RR_ORIGINAL_AI_FIRST_SLICE");
 // Product Course owns one real Player and seven original AI. This is a
 // separate native roster, never the legacy one-AI diagnostic slice.
 // Every accepted human Course request cold-reloads, even the same selection.
 const bool humanCourseEnabled=courseRaceCold&&(isTaTrack||directOriginalHuman)&&!allAiEnabled;
 require50DU(!captureRun.enabled()||(!headlessLive&&raceEnabled&&(allAiEnabled||humanCourseEnabled)),
     "Window track capture requires a real window and an admitted original human or all-AI race");
 require50DU(!directOriginalHuman||!originalAiSetting||std::string(originalAiSetting)!="1",
     "direct human Course forbids the legacy first-slice AI override");
 const bool nativeOriginalFleet=allAiEnabled||humanCourseEnabled;
 const bool originalAiEnabled=nativeOriginalFleet||(originalAiSetting&&std::string(originalAiSetting)=="1");
 const char* dynamicContactSetting=std::getenv("RR_ORIGINAL_AI_DYNAMIC_CONTACT");
 const bool dynamicContactEnabled=nativeOriginalFleet||(dynamicContactSetting&&std::string(dynamicContactSetting)=="1");
 require50DU(!dynamicContactEnabled||originalAiEnabled,
     "dynamic source contacts require the bounded original AI session");
 constexpr std::size_t originalAiSlot=1;
 const auto originalAiOwnsSlot=[&](std::size_t slot) noexcept {
  return originalAiEnabled&&(allAiEnabled||(humanCourseEnabled?slot!=0:slot==originalAiSlot));
 };
 require50DU(!isMaSaTrack||(raceEnabled&&!checkpointInput&&!startAtMenu&&
     ((directOriginalHuman&&humanCourseEnabled&&!allAiEnabled)||(!directOriginalHuman&&allAiEnabled))),
     "MA/SA require either actual Main/Course Player0 plus seven AI or their retained ALL8 route");
 require50DU(!isMbTrack||(raceEnabled&&humanCourseEnabled&&!allAiEnabled&&!checkpointInput&&!startAtMenu&&trackAdmission.mbNativeWorldRequested),
     "MB requires the exact original Main/Course Player0 plus seven AI request");
 require50DU(!isScTrack||(raceEnabled&&humanCourseEnabled&&!allAiEnabled&&!checkpointInput&&!startAtMenu&&trackAdmission.scNativeWorldRequested),
     "SC requires the exact original Main/Course Player0 plus seven AI request");
 require50DU(!isTcTrack||(raceEnabled&&humanCourseEnabled&&!allAiEnabled&&!checkpointInput&&!startAtMenu&&trackAdmission.tcNativeWorldRequested),
     "TC requires the exact original Main/Course Player0 plus seven AI request");
 require50DU(!isRcTrack||(raceEnabled&&humanCourseEnabled&&!allAiEnabled&&!checkpointInput&&!startAtMenu&&
     trackAdmission.rcNativeWorldRequested&&trackAdmission.gcMode2),
     "RC requires the shared Main/Course Player0 plus seven AI request and mode2");
 require50DU(!isTbTrack||(raceEnabled&&humanCourseEnabled&&!allAiEnabled&&!checkpointInput&&!startAtMenu&&
     trackAdmission.tbNativeWorldRequested&&trackAdmission.gcMode2),
     "TB requires the shared Main/Course Player0 plus seven AI request and mode2");
 require50DU(!isGbTrack||(raceEnabled&&humanCourseEnabled&&!allAiEnabled&&!checkpointInput&&!startAtMenu&&
     trackAdmission.gbNativeWorldRequested&&trackAdmission.gcMode2),
     "GB requires shared Main/Course Player0 plus seven AI request and mode2");
 require50DU(!isMcTrack||(raceEnabled&&humanCourseEnabled&&!allAiEnabled&&!checkpointInput&&!startAtMenu&&
     trackAdmission.mcNativeWorldRequested&&trackAdmission.gcMode2),
     "MC requires genuine rooted human Course mode2 and actual runtime owners");
 require50DU(!isRaTrack||(raceEnabled&&(allAiEnabled||humanCourseEnabled)&&!checkpointInput&&!startAtMenu),
     "RA requires a fresh original Course human roster or explicit legacy ALL8 roster");
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
 // The concrete nine-trigger owner is part of the genuine GC world. Zero is
 // an explicit diagnostic omission; the admitted native fleet needs no opt-in.
 const bool gcNativeGate=isGcTrack&&(gateSetting?std::string(gateSetting)=="1":originalCameraEnabled);
 const char* nativeWaterSetting=std::getenv("RR_GC_NATIVE_WATER");
 require50DU(!nativeWaterSetting||std::string(nativeWaterSetting)=="0"||std::string(nativeWaterSetting)=="1",
     "RR_GC_NATIVE_WATER must be 0 or 1");
 // Original water and local fog are on for the genuinely admitted GC
 // single-view source-camera world. Explicit0 remains diagnostic only.
 const bool nativeWaterEnabled=nativeWaterSetting?std::string(nativeWaterSetting)=="1":
     isGcTrack&&originalCameraEnabled;
 require50DU(!nativeWaterEnabled||(isGcTrack&&originalCameraEnabled&&(!originalUiActive||courseRaceCold)),
     "GC water/particles require the genuine selected fleet/source-camera world");
 require50DU(!(isMaSaTrack||isRaTrack||directOriginalHuman)||originalCameraEnabled,"MA/SA/RA require the actual recovered camera/visibility owner");
 require50DU(!gcNativeGate||(originalCameraEnabled&&!checkpointInput&&!startAtMenu),
     "GC native world triggers require the genuine fresh fleet/source-camera world");
 const bool gcNativeWorldSelected=isGcTrack&&gcNativeGate&&nativeWaterEnabled&&
     originalCameraEnabled&&!checkpointInput&&!startAtMenu;
 if(isGcTrack){
  require50DU(gcNativeWorldSelected||trackAdmission.gcIncompleteWorld,
      "GC diagnostic omissions or legacy control/camera paths require RR_GC_INCOMPLETE_WORLD=1; ordinary GC owns the selected fleet, source camera, triggers and effects");
  std::cerr<<(gcNativeWorldSelected?"GC NATIVE WORLD: ":"GC INCOMPLETE DIAGNOSTIC WORLD: ")
      <<"real track, navigation, opening/windmill and static-model properties; "
      <<(gcNativeGate?"nine native-geometry triggers with source LostAI/entry/exit and Gate audio owners. ":
          "authored branch triggers and Gate are diagnostically disabled. ")
      <<(nativeWaterEnabled?"Original water/particles/fog owners selected. ":"Water/particle/fog owners are diagnostically disabled. ")
      <<"Native geometry/render/listener policies remain explicit; separate mode1 record and positive-effective cut-camera paths remain guarded.\n";
 }
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
 require50DU(rr::platform::hostTrackAdmitted50IM(loadedTrack,trackAdmission),
     "Track requires its genuine native-world request or explicit diagnostic admission and RR_RACE_MODE=2");
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
 // mutates the actual profile. A Restart has no receipt and reexecutes the
 // source clear against the already-singleton retained subtype3 snapshot.
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
       (selected.mode34==0||selected.mode34==2)&&selected.subtype>=1&&selected.subtype<=3&&
       selected.entrants>=1&&selected.entrants<=raceRacerCapacity&&
       ((selected.packed1125C>>5)&3u)==1&&
       ((selected.packed1125C>>7)&15u)+1u==selected.entrants&&
       (selected.subtype==2||
        (selected.subtype==3?nativeRacerManager&&selected.entrants==1u:
         selected.entrants==raceRacerCapacity))&&
       (selected.mode34!=2||(selected.subtype==1&&selected.laps==3)),
       "native Course requires its actual committed live roster extent");
   return selected.entrants;
  }
  return raceRacerCapacity;
 }();
 const std::uint32_t raceOpponentCount=raceRacerCount-1;
 // Native selected roster, not the retail human-dependent boot root1AE620.
 // Counts use the original1125C layout; every AI retains its real pod/profile.
 const std::uint32_t originalAiHumanCount=allAiEnabled?0u:1u;
 const std::uint32_t originalAiOpponentCount=raceRacerCount-originalAiHumanCount;
 const std::uint16_t originalAiSettings1125C=[&]() -> std::uint16_t {
  // Actual mode0/mode2 human Course launch and Restart consume the retained
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
       rr::game::nativeSingleHumanCourseMode50KG(selected)&&
       selected.rider==rr::game::dlc50DL::profileRider(playerRacer60)&&selected.subtype>=1&&selected.subtype<=3&&selected.laps>=1&&selected.laps<=25&&
       selected.entrants==raceRacerCount&&((selected.packed1125C>>5)&3u)==1&&
       ((selected.packed1125C>>7)&15u)==raceOpponentCount,
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
 // RA constructs its bootstrap from authenticated original resources in memory.
 // Legacy diagnostic tracks retain their explicit corpus; no replacement JSON
 // case, captured output or generated-file fallback is created for RA.
 f::Fixture x(0,".",directOriginalInputs?f::AssetInput::OriginalDisc:f::AssetInput::RetainedDevelopment);
 auto&s=x.state;auto&m=s.bodies.main;
 const auto endpointPolicy=original_disc_query_bootstrap50LE::endpointPolicy();
 native_racer_collision50HW::Owner originalBootstrapCollision;
 J corpus;const J* chosen=nullptr;cold::Shape shape{};
 CollisionFilter48J qfilter{};Type2StaticQuery49X query{};
 std::vector<cold::Mesh> meshes;
 if(directOriginalInputs){
  std::string error;
  const bool prepared=originalBootstrapCollision.prepare(
      {x.assets.originalArchivePath.string(),"Pods\\Pod01\\"},x.assets.originalArchive,endpointPolicy,error);
  require50DU(prepared,error.empty()?"original POD01 collision bootstrap rejected":error.c_str());
  original_disc_query_bootstrap50LE::initialize(originalBootstrapCollision.main(),qfilter,query,false);
 }else{
  std::ifstream file(corpusPath);require50DU(bool(file),"cannot open collision corpus");file>>corpus;
  for(const auto& item:corpus["cases"])if(item["name"]==trackSession->mainCollisionCase)chosen=&item;
  require50DU(chosen,"authored placement case present");const auto& row=*chosen;const auto& qj=row["query"];
  shape=cold::shape(corpus["shapes"][qj["shape"].get<std::size_t>()]);
  if(!sourceStaticCandidates){meshes.reserve(corpus["meshes"].size());
   for(const auto& j:corpus["meshes"])meshes.push_back(cold::mesh(j));}
  qfilter=cold::filter(qj["filter"]);query.resource=&shape.view;query.filter=&qfilter;query.queryScene.value=77;
  query.normalMode54=0u;query.normalMode54Bound=true;
  query.collisionTypeTag=qj["collision_tag"];query.groupTypeTag=qj["group_tag"];query.callbackEnabled=qj["callback"];
  cold::words(query.copied40.data(),qj["copied40"]);cold::words(query.copied50.data(),qj["copied50"]);
  query.skipMask=row["masks"][0];query.dynamicMask=row["masks"][1];query.materialMask=row["masks"][2];
 }
 const auto& mainQueryResource=*query.resource;
 std::vector<std::string> names;
 std::vector<Type2StaticCandidate49X> legacyCandidates;std::size_t stackCount=1;
 if(!sourceStaticCandidates){
 const auto& row=*chosen;
 names.resize(row["candidates"].size());for(std::size_t i=0;i<names.size();++i)names[i]=cold::unhex(row["candidates"][i]["name_hex"]);
 legacyCandidates.reserve(row["order"].size());
 for(auto index:row["order"].get<std::vector<std::size_t>>()){
  const auto&j=row["candidates"][index];auto&m=meshes[j["mesh"].get<std::size_t>()];legacyCandidates.emplace_back();auto&out=legacyCandidates.back();
  out.nodeRadii=m.radii.data();out.nodeRadiusCount=m.radii.size();out.maxDepth=m.maxDepth;out.candidate.hasMaterialScene=j["has_scene"];out.candidate.sceneName=names[index].data();out.candidate.sceneNameBytes=names[index].size();
  auto&v=out.candidate.collision;v.geometry={&m.mesh,cold::matrix(j["world"]),j["id"].get<std::uint32_t>()};v.filter=cold::filter(j["filter"]);v.sphere=cold::sphere(j["sphere"]);v.typeTag=j["type_tag"];
  cold::words(v.copied40.data(),j["copied40"]);cold::words(v.copied50.data(),j["copied50"]);stackCount=std::max(stackCount,1+mainQueryResource.maxDepth+m.maxDepth);
 }
 } // TA retained diagnostic candidate inputs only.
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
 // Restart, Main return and exceptions cannot leave a dangling manager pointer.
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
  require50DU(courseRaceCold&&originalMainApplication.riderProfile&&
      originalMainApplication.riderProfile->courseRaceSnapshot(selected)&&
      original_course_targets50KX::target(selected)&&
      original_course_targets50KX::target(selected)->track==loadedTrack&&
      selected.rider==rr::game::dlc50DL::profileRider(playerRacer60)&&originalMainApplication.profile&&
      selected.mode34==originalMainApplication.profile->mode34&&
      rr::game::nativeSingleHumanCourseMode50KG(selected)&&
      selected.subtype>=1&&selected.subtype<=3&&selected.laps==(humanCourseEnabled?(originalAiSettings1125C&31u):3u)&&selected.entrants==raceRacerCount&&
      ((selected.packed1125C>>5)&3)==1&&((selected.packed1125C>>7)&15)==raceOpponentCount&&
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
 // TB's real Trigger16 action group must retain voice126 before actual
 // collision Storage registrations. The construction layer calls this once
 // after decoding its shared tbTriggerAuthoring, before publishing actions.
 std::shared_ptr<tb_pod_sound50TB::Owner> tbPodSound;
 std::shared_ptr<rr::platform::OriginalSoundMode50KG> tbSoundMode;
 std::filesystem::path tbPreparedAudioRoot;
 bool tbLiveAudioPrepared=false;
 const auto prepareTbPodSound=[&](std::shared_ptr<const tb_trigger_authoring50TB::Owner> authored){
  require50DU(isTbTrack&&nativeOriginalFleet&&authored&&authored->ready()&&
      authored->coldSource()&&!tbLiveAudioPrepared&&!tbPodSound,
      "TB early PodSound requires its actual fresh trigger source and human fleet");
  const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
  tbPreparedAudioRoot=suppliedAudioRoot?std::filesystem::path(suppliedAudioRoot):
      runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO");
  std::string error;
  const bool audioPrepared=liveAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),
      tbPreparedAudioRoot,error,playerRacer60,loadedAudioSettings);
  require50DU(audioPrepared,error.empty()?"TB actual shared audio preparation rejected":error.c_str());
  tbLiveAudioPrepared=true;
  auto* mixer=liveAudio->mixerForMaterialOwner();auto* groups=liveAudio->groupsForProviders();
  require50DU(mixer&&groups&&groups->uses(*mixer),"TB actual shared mixer/groups unavailable");
  // Binding is cold and one-shot. Later material/engine setup reuses this
  // exact actual mode rather than attempting to bind a second one.
  tbSoundMode=originalMainApplication.soundMode;
  if(!tbSoundMode){
   tbSoundMode=std::make_shared<rr::platform::OriginalSoundMode50KG>();
   require50DU(tbSoundMode->initializeFromProfile1101C(1),"TB direct native stereo mode rejected");
  }
  require50DU(mixer->bindOriginalSoundMode50KG(tbSoundMode),"TB early source-mode binding rejected");
  tbPodSound=std::make_shared<tb_pod_sound50TB::Owner>();
  const bool soundReady=tbPodSound->prepare(std::move(authored),*mixer,*groups,tbPreparedAudioRoot,error);
  require50DU(soundReady,error.empty()?"TB actual HUGEDOOR source/voice126 rejected":error.c_str());
 };
 std::shared_ptr<gb_pod_sound50GB::Owner> gbPodSound;
 std::shared_ptr<rr::platform::OriginalSoundMode50KG> gbSoundMode;
 std::filesystem::path gbPreparedAudioRoot;
 bool gbLiveAudioPrepared=false;
 const auto prepareGbPodSound=[&](std::shared_ptr<const gb_trigger_authoring50GB::Owner> authored){
  require50DU(isGbTrack&&nativeOriginalFleet&&authored&&authored->ready()&&
      authored->coldSource()&&!gbLiveAudioPrepared&&!gbPodSound,
      "GB early PodSound requires its actual fresh trigger source and human fleet");
  const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
  gbPreparedAudioRoot=suppliedAudioRoot?std::filesystem::path(suppliedAudioRoot):
      runtime_asset_paths50LB::resolve("work/native/project/disc/AUDIO");
  std::string error;
  const bool audioPrepared=liveAudio->prepare(runtime_asset_paths50LB::resolve("work/native/project/disc"),
      gbPreparedAudioRoot,error,playerRacer60,loadedAudioSettings);
  require50DU(audioPrepared,error.empty()?"GB actual shared audio preparation rejected":error.c_str());
  gbLiveAudioPrepared=true;
  auto* mixer=liveAudio->mixerForMaterialOwner();auto* groups=liveAudio->groupsForProviders();
  require50DU(mixer&&groups&&groups->uses(*mixer),"GB actual shared mixer/groups unavailable");
  // Binding is cold and one-shot. Later material/engine setup reuses this
  // exact actual mode rather than attempting to bind a second one.
  gbSoundMode=originalMainApplication.soundMode;
  if(!gbSoundMode){
   gbSoundMode=std::make_shared<rr::platform::OriginalSoundMode50KG>();
   require50DU(gbSoundMode->initializeFromProfile1101C(1),"GB direct native stereo mode rejected");
  }
  require50DU(mixer->bindOriginalSoundMode50KG(gbSoundMode),"GB early source-mode binding rejected");
  // The PodSound owner exists exactly when a trigger authors a PodSound
  // action (stock GB: the door, trigger 16); absent = none.
  std::cerr<<"gb_course_owner owner=podsound present="<<(authored->authorsPodSound()?1:0)<<'\n';
  if(!authored->authorsPodSound())return;
  gbPodSound=std::make_shared<gb_pod_sound50GB::Owner>();
  const bool soundReady=gbPodSound->prepare(std::move(authored),*mixer,*groups,gbPreparedAudioRoot,error);
  require50DU(soundReady,error.empty()?"GB actual BIGDOOR source/voice126 rejected":error.c_str());
 };
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
 // First usable scope is the explicit muted consumed ALL8 RA spectator route.
 // Source command25 audible-feedback semantics remain unsupported; preserve
 // every counted group state rather than invent an Auxiliary pause exemption.
 const char* spectatorAudioPolicy=std::getenv("RR_AUDIO");
 const bool sourceConsumedPauseActivation=consumedLaunch&&isRaTrack&&allAiEnabled&&
     originalCameraEnabled&&spectatorAudioPolicy&&std::string(spectatorAudioPolicy)=="0";
 // Human Career intro needs the genuine RA race/world prefix, but does not
 // opt into the separate source spectator Pause implementation.
 const bool sourceConsumedIntroActivation=consumedLaunch&&isRaTrack&&humanCourseEnabled&&
     originalCameraEnabled&&dynamicProfile.game34==2&&nativeOrdinaryLaunch11258==1;
 // Actual world/collision ownership remains in the next lexical stage.
 // It retires before this original early fixture/rig/audio prefix, on all exits.
 interactive_world50DU::Context world50DU{
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
  .endpointPolicy=endpointPolicy,
  .gaOriginalDisc=gaOriginalDisc,
  .gbLiveAudioPrepared=gbLiveAudioPrepared,
  .gbOriginalDisc=gbOriginalDisc,
  .gbPodSound=gbPodSound,
  .gbPreparedAudioRoot=gbPreparedAudioRoot,
  .gbSoundMode=gbSoundMode,
  .gcNativeGate=gcNativeGate,
  .gcNativeWorldSelected=gcNativeWorldSelected,
  .gcOriginalDisc=gcOriginalDisc,
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
  .legacyCandidates=legacyCandidates,
  .liveAudio=liveAudio,
  .loadedAudioSettings=loadedAudioSettings,
  .loadedTrack=loadedTrack,
  .m=m,
  .maOriginalDisc=maOriginalDisc,
  .mainQueryResource=mainQueryResource,
  .managerRuntimeSelection=managerRuntimeSelection,
  .mbOriginalDisc=mbOriginalDisc,
  .mcOriginalDisc=mcOriginalDisc,
  .meshes=meshes,
  .mode1Enabled=mode1Enabled,
  .mode1Requested=mode1Requested,
  .monotonicNow50DU=monotonicNow50DU,
  .names=names,
  .nativeGaSelection=nativeGaSelection,
  .nativeGbSelection=nativeGbSelection,
  .nativeMaSelection=nativeMaSelection,
  .nativeMbSelection=nativeMbSelection,
  .nativeMcSelection=nativeMcSelection,
  .nativeOrdinaryLaunch11258=nativeOrdinaryLaunch11258,
  .nativeOriginalFleet=nativeOriginalFleet,
  .nativeRaSelection=nativeRaSelection,
  .nativeRacerManager=nativeRacerManager,
  .nativeRcSelection=nativeRcSelection,
  .nativeRecordActor=nativeRecordActor,
  .nativeSaSelection=nativeSaSelection,
  .nativeScSelection=nativeScSelection,
  .nativeSelectedLights=nativeSelectedLights,
  .nativeTaSelection=nativeTaSelection,
  .nativeTbSelection=nativeTbSelection,
  .nativeTcSelection=nativeTcSelection,
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
  .prepareGbPodSoundCall=interactive_frame50DU::BorrowedCall<void(std::shared_ptr<const gb_trigger_authoring50GB::Owner>)>{prepareGbPodSound},
  .prepareTbPodSoundCall=interactive_frame50DU::BorrowedCall<void(std::shared_ptr<const tb_trigger_authoring50TB::Owner>)>{prepareTbPodSound},
  .qfilter=qfilter,
  .query=query,
  .raceEnabled=raceEnabled,
  .raceOpponentCount=raceOpponentCount,
  .raceRacerCount=raceRacerCount,
  .racerContactsEnabled=racerContactsEnabled,
  .rcOriginalDisc=rcOriginalDisc,
  .renderDiagnostics=renderDiagnostics,
  .reportHost50DU=reportHost50DU,
  .reportRenderer50DU=reportRenderer50DU,
  .requestedWindowSamples=requestedWindowSamples,
  .s=s,
  .saOriginalDisc=saOriginalDisc,
  .scOriginalDisc=scOriginalDisc,
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
  .taOriginalDisc=taOriginalDisc,
  .tbLiveAudioPrepared=tbLiveAudioPrepared,
  .tbOriginalDisc=tbOriginalDisc,
  .tbPodSound=tbPodSound,
  .tbPreparedAudioRoot=tbPreparedAudioRoot,
  .tbSoundMode=tbSoundMode,
  .tcOriginalDisc=tcOriginalDisc,
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

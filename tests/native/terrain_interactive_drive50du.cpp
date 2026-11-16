// Replacement root prefix ONLY; nativeApplicationMain50DU/main body remains unchanged
// except appending the three original services to the single runSession50DU call.
#include "native_last_failure50pd.h"
#include "runtime_allocations50fl.h"
#include "../../src/game/ps2_float_mode50la.h"
#include "terrain_interactive_session50du.h"
#include "json.hpp"
#include "runtime_asset_paths50lb.h"
#include "../../src/assets/dlc_files50dl.h"
#include "track_session_assets50ig_fixture.h"
#include "checkpoint_recovery50fi_fixture.h"
#include "lap_drive50gj_fixture.h"
#include "native_gc_water_session50kg_fixture.h"
#include "original_ui_main_entry50kg.h"
#include "original_course_race_targets50kx.h"
#include "windows_framebuffer_capture50ld.h"
#include "original_ui_capture_navigation50la.h"
#include "player_loading_session50du.h"
#include "../../src/game/collision_normal_mode50ja.h"
#include "../../src/game/control_presets49d.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../src/game/announcer_callbacks50ic.h"
#include "../../platform/linux/native/host_track_selection50im.h"
#include "../../platform/linux/native/material_texture_functions50ee.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using namespace rr::game;
using J=nlohmann::json;
namespace recovery50FI=checkpoint_recovery50fi_fixture;

namespace {
void require50DU(bool ok, const char* message) {
    if(!ok){native_last_failure50PD::record(message);std::cerr<<"require_failed "<<(message?message:"")<<std::endl;throw std::runtime_error(message);}
}
std::uint64_t monotonicNow50DU() noexcept {
    using Clock = std::chrono::steady_clock;
    static const auto origin = Clock::now();
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - origin).count());
}
void reportHost50DU(const rr::platform::GameHostDiagnostic50ER& diagnostic) {
    std::cerr << "host error " << static_cast<unsigned>(diagnostic.error)
              << " at " << (diagnostic.operation ? diagnostic.operation : "unknown")
              << " errno=" << diagnostic.systemError
              << " window=" << static_cast<unsigned>(diagnostic.window.error)
              << " EGL=" << static_cast<unsigned>(diagnostic.headless.error) << '\n';
}
void reportRenderer50DU(const rr::platform::VehicleWindowDiagnostic50DS& diagnostic) {
    std::cerr << "renderer error " << static_cast<unsigned>(diagnostic.error)
              << " at " << (diagnostic.operation ? diagnostic.operation : "unknown")
              << " GL=" << diagnostic.glError << " cleanup=" << diagnostic.cleanupGlError
              << " component=" << diagnostic.component << '\n';
}
}
// This inline frontend body relies on the original helper functions above.
#include "original_race_display_lifetime50la.h"
#include "original_frontend_session50la.h"

#include "windows_native_entry50lb.h"
int nativeApplicationMain50DU(int argc,char**argv){try{
#if defined(_WIN32)
 // Preserve diagnostics even if native ownership cleanup terminates abruptly.
 std::cout.setf(std::ios::unitbuf);std::cerr.setf(std::ios::unitbuf);
 std::cerr<<"windows_tester_start source_canvas=640x448 presentation=1280x896 windowed=1 telemetry=0\n";
#endif
 // A distributable original-disc entry does not require a diagnostic corpus
 // filename. Legacy explicit-corpus invocations remain available for tests.
 const bool discEntry=argc==3&&std::string(argv[1])=="--assets";
 if(discEntry){
  require50DU(argv[2]&&*argv[2],"--assets requires an original disc directory");
#if defined(_WIN32)
  // The no-argument entry already anchored cwd with SetCurrentDirectoryW.
  // Keep its ASCII relative root across legacy narrow argv/env consumers.
  const auto root=windows_native_entry50LB::active()?std::string("."):
      std::filesystem::absolute(argv[2]).string();
#else
  const auto root=std::filesystem::absolute(argv[2]).string();
#endif
  require50DU(std::filesystem::is_directory(root),"--assets directory is unavailable");
  const auto configure=[](const char* name,const char* value){
#if defined(_WIN32)
   return _putenv_s(name,value)==0;
#else
   return setenv(name,value,1)==0;
#endif
  };
  require50DU(configure("RR_ORIGINAL_ASSET_ROOT",root.c_str())&&
      configure("RR_ORIGINAL_UI_ROOT",root.c_str())&&configure("RR_ORIGINAL_MAIN_MENU","1"),
      "original disc environment configuration failed");
  // A normal disc entry uses the existing logical audio owner even when
  // host output is disabled. Game Options Accept needs that real owner to
  // commit188740; the diagnostic muted policy deliberately has no mixer.
  // Keep the existing host-device policy and all explicit output choices.
  if(!std::getenv("RR_ORIGINAL_MAIN_AUDIO"))
   require50DU(configure("RR_ORIGINAL_MAIN_AUDIO","enabled"),"default Main audio policy failed");
  if(std::string(std::getenv("RR_ORIGINAL_MAIN_AUDIO"))=="enabled"&&
     !std::getenv("RR_ORIGINAL_MAIN_AUDIO_ROOT")){
   const auto audioRoot=(std::filesystem::path(root)/"AUDIO").string();
   require50DU(configure("RR_ORIGINAL_MAIN_AUDIO_ROOT",audioRoot.c_str()),
       "default Main audio directory configuration failed");
  }
  // Original Main is the only initial menu. Its existing human Course
  // handoff uses native mode2; do not require diagnostic launcher variables
  // just to reach the normal selection/loading route. Explicit overrides
  // remain visible to the existing validation rather than silently repaired.
  if(!std::getenv("RR_RACE_MODE"))
   require50DU(configure("RR_RACE_MODE","2"),"default native Course mode failed");
  if(!std::getenv("RR_START_MENU"))
   require50DU(configure("RR_START_MENU","0"),"default native Course entry failed");
  // PC enhancement (NOT ORIGINAL): check DLC packages once, before the first
  // original file is opened. No DLC folder = no overlay, stock paths unchanged.
  rr::assets::dlc50DL::startup();
 }
 const bool headlessLive=argc==4&&std::string(argv[2])=="--headless-live";
 if(argc!=2&&!headlessLive&&!discEntry){
  std::cerr<<"usage: terrain_interactive_drive50du CORPUS_JSON [--headless-live CAPTURE_PPM]\n"
             "Use --assets ORIGINAL_DISC_DIRECTORY for normal original Main/Rider/Course entry directly from disc files. This sets the original asset/UI roots and, unless explicitly supplied, RR_RACE_MODE=2, RR_START_MENU=0, RR_ORIGINAL_MAIN_AUDIO=enabled and its AUDIO directory. Existing host-device policy is unchanged; RR_AUDIO_DEVICE=0 disables physical output while retaining logical audio owners. Explicit audio/device policy remains authoritative.\n"
             "Legacy CORPUS_JSON entry remains available separately. RR_GENERATED_RUNTIME_ROOT resolves diagnostic evidence/work inputs and relative CORPUS_JSON; it is not required data for the normal original-disc route. Explicit CLI entry never changes process cwd; Windows no-argument launch anchors it to the executable directory and logs to RacerRevenge.log there. RR_RACE_MODE=1 requires fully prepared source providers; RR_RACE_MODE=2 uses the existing native audio/timing fallback.\n"
             "RR_HUMAN_CONTROL_PRESET=0..7 selects an original row only for direct human startup (default4); original Main retains its own configuration. Normal menu entry retains the device used for an accepted Course Confirm: pad A selects controller driving; keyboard Enter/Space selects keyboard driving (keyboard wins simultaneous confirms). Direct startup defaults to keyboard. RR_HUMAN_INPUT_DEVICE=keyboard or controller explicitly overrides that choice, never an AI role. Normal keyboard rows: W/S throttle/brake, Left/Right steer, Up/Down original left Y. Advanced rows: W/S left engine, Up/Down right engine. Space: boost; R: hold repair; Left Shift: powerslide; Escape: Pause/Resume; F5 or Restart Race: new grid session. On the pad, Start opens Pause or resumes from its main page; D-pad selects, A confirms, Y backs out or cancels. Restart and Main return use their confirmation menu. Checkpoint recovery remains a separately admitted diagnostic path.\n"
             "Original frontend menus accept D-pad or left-stick navigation. Center the stick between steps; only Course uses its original horizontal held-repeat timer. This native stick alias does not change driving, native Pause, or Results controls.\n"
             "When original Results owns input, H is the native keyboard hide/show alias for gamepad B. This applies to the admitted canonical Results route, not the native fallback results panel; original transition and mode restrictions still apply.\n"
             "Normal Main uses authored roster/course order and unlocks. Original-disc Rider images are decoded in process; RR_ORIGINAL_RIDER_IPUM_CACHE is an optional legacy authenticated cache, not a normal runtime data pack. Each race selection must resolve its own original archive, native assets and source owners. Human Course queues a complete cold reload after exit, including unchanged track/rider, so accepted controls apply. No unsupported destination is silently substituted.\n"
             "Main/Options logical audio uses RR_ORIGINAL_MAIN_AUDIO=enabled and RR_ORIGINAL_MAIN_AUDIO_ROOT pointing to AUDIO. The explicit diagnostic muted policy suppresses logical audio and cannot commit Game Options Accept. RR_AUDIO and RR_AUDIO_DEVICE independently control host race output/device policy. With RR_AUDIO_WAV, later Main visits add -menu-session-N and races add -race-session-N before the extension.\n";
  return 2;
 }
 const auto resolvedCorpusPath=runtime_asset_paths50LB::generatedInput(discEntry?
     "evidence/staticType2Producer49X/original-word/native-fixtures.json":argv[1]).string();
 windows_capture50LD::Run captureRun;
 require50DU(!captureRun.enabled()||!headlessLive,"Window capture requires the actual window entry, never --headless-live");
 // Every return destroys the complete old session, including native state and
 // asset owners. Only an explicit host request enters another cold setup.
 recovery50FI::Plan activeCheckpoint{},nextCheckpoint{};bool useCheckpoint=false;
 lap_drive50GJ::Continuity lapContinuity{};
 // One application epoch, selector owner and native control/camera preference
 // owner. Cold race restart reconstructs racers, never application state.
 auto applicationClockOwner=std::make_shared<HostMonotonicClock50ID>();
 auto& applicationClock=*applicationClockOwner;
 auto applicationControlsOwner=std::make_shared<ControlPresets49D>();
 initializeControlPresets49D(*applicationControlsOwner);
 auto& applicationControls=*applicationControlsOwner;
 original_main_entry50KG::Application originalMainApplication(applicationClockOwner,applicationControlsOwner,
     discEntry&&!headlessLive);
 // Keep the actual Application alive while terminal exception cleanup retires
 // its source Aural/Pending/Light/backing. Session-local owners unwind FIRST.
 // Normal Course handoffs remain inside this scope and do not retire services.
 try {
 require50DU(!captureRun.navigate()||originalMainApplication.enabled,"unattended original UI route requires Original Main enabled");
 SharedRandomOwner50ID directApplicationRandom{};
 auto nativeWaterGlobals=std::make_shared<native_gc_water_session50KG::Application>();
 // Original Main retains its real source default/selection. This optional
 // native direct-human selector cannot override a genuine UI-owned profile.
 const char* humanPresetSetting=std::getenv("RR_HUMAN_CONTROL_PRESET");
 require50DU(!humanPresetSetting||(!originalMainApplication.enabled&&humanPresetSetting[0]>='0'&&
     humanPresetSetting[0]<='7'&&humanPresetSetting[1]=='\0'),
     "RR_HUMAN_CONTROL_PRESET must be0..7 and requires original Main disabled");
 if(!originalMainApplication.enabled){
  const auto preset=humanPresetSetting?static_cast<std::uint32_t>(humanPresetSetting[0]-'0'):4u;
  require50DU(selectControlPreset49D(applicationControls,0,preset),"explicit native human preset selection");
 }
 bool enterOriginalUi=originalMainApplication.enabled;
 //17C784 constructor350=0; native startup does not replay retail17D104
 //command-line boot. Retain this real process field through every race/menu.
 const std::int32_t cameraStartup350=0;
 original_course_targets50KX::ColdLoadRequest courseColdLoad;
 // Shared3DF5B0 survives serial racer visits and native cold sessions. The
 // value remains unbound until an actual completed121438/126FA0 writer.
 rr::game::CollisionNormalModeOwner50JA collisionNormalMode{};

 const char* startMenuSetting=std::getenv("RR_START_MENU");
 require50DU(!startMenuSetting||std::string(startMenuSetting)=="0"||std::string(startMenuSetting)=="1",
     "RR_START_MENU must be 0 or 1");
 // Original Main owns its startup screen. The separate host pause/start menu
 // must not default on merely because this is a native window. Explicit
 // conflicting requests still fail at the existing Course admission gate.
 bool startAtMenu=startMenuSetting?std::string(startMenuSetting)=="1":
     !headlessLive&&!originalMainApplication.enabled;
 std::uint32_t selectedRacer60=1; // Persist only explicit next-session front-menu selection.
 // In-memory host settings only. Checkpoint recovery retains effective levels;
 // explicit Start/Restart alone commits the requested configuration.
 auto selectedAudioSettings=rr::game::freshOriginalAudioSettings50IC();
 auto effectiveAudioSettings=selectedAudioSettings;
 // Resolve the process default once. Pending menu selection never mutates the
 // environment or rebinds a live world. Unknown/unbound metadata still fails.
 const char* initialTrackError=nullptr;
 const bool rootedHumanCourseSupported=originalMainApplication.enabled&&
     !runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
 const auto* initialTrack=track_session_assets50IG::select(std::getenv("RR_TRACK"),initialTrackError);
 require50DU(initialTrack,initialTrackError?initialTrackError:"original track admission failed");
 const bool initialGa=track_session_assets50IG::codeEquals(initialTrack->world->code,"GA");
 const bool initialMa=track_session_assets50IG::codeEquals(initialTrack->world->code,"MA");
 const bool initialSa=track_session_assets50IG::codeEquals(initialTrack->world->code,"SA");
 const bool initialRa=track_session_assets50IG::codeEquals(initialTrack->world->code,"RA");
 const bool initialMb=track_session_assets50IG::codeEquals(initialTrack->world->code,"MB");
 const bool initialSc=track_session_assets50IG::codeEquals(initialTrack->world->code,"SC");
 const bool initialTc=track_session_assets50IG::codeEquals(initialTrack->world->code,"TC");
 const bool initialRc=track_session_assets50IG::codeEquals(initialTrack->world->code,"RC");
 // Shared native request capability, separate from process-local unlock fixtures.
 // Catalog authentication and the actual cold loaders still establish resources.
 const char* sharedCourseMode=std::getenv("RR_RACE_MODE");
 const char* sharedCourseAllAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool sharedHumanCourseRequested=rootedHumanCourseSupported&&!startAtMenu&&
     sharedCourseMode&&std::string(sharedCourseMode)=="2"&&
     !(sharedCourseAllAi&&std::string(sharedCourseAllAi)=="1");
 const bool initialTb=track_session_assets50IG::codeEquals(initialTrack->world->code,"TB");
 const bool initialGb=track_session_assets50IG::codeEquals(initialTrack->world->code,"GB");
 const bool initialMc=track_session_assets50IG::codeEquals(initialTrack->world->code,"MC");
 const bool rcNativeWorldRequested=sharedHumanCourseRequested;
 const bool tbNativeWorldRequested=sharedHumanCourseRequested;
 const bool gbNativeWorldRequested=sharedHumanCourseRequested;
 const bool mcNativeWorldRequested=sharedHumanCourseRequested;
 const bool tcNativeWorldRequested=sharedHumanCourseRequested;
 const bool scNativeWorldRequested=sharedHumanCourseRequested;
 const bool mbNativeWorldRequested=sharedHumanCourseRequested;
 require50DU(!(initialRc||initialTb||initialGb||initialMc||initialTc||initialSc||initialMb)||sharedHumanCourseRequested,
     "native human Course loader requires original-disc Main, mode2 and human Player0");
 const char* raRequestSetting=std::getenv("RR_RA_NATIVE_WORLD");
 require50DU(!raRequestSetting||std::string(raRequestSetting)=="0"||std::string(raRequestSetting)=="1",
     "RR_RA_NATIVE_WORLD must be 0 or 1");
 const char* raProfileSetting=std::getenv("RR_ORIGINAL_TEST_PROFILE");
 const bool raNativeWorldRequested=sharedHumanCourseRequested||
     (raRequestSetting&&std::string(raRequestSetting)=="1"&&
      originalMainApplication.enabled&&raProfileSetting&&std::string(raProfileSetting)=="mawhonic-ryloth");
 require50DU(!initialRa||(raNativeWorldRequested&&!startAtMenu),
     "RA requires original-disc Main/Course human entry or the explicit legacy ALL8 request");
 require50DU(track_session_assets50IG::codeEquals(initialTrack->world->code,"TA")||
     track_session_assets50IG::codeEquals(initialTrack->world->code,"GC")||initialGa||initialMa||initialSa||initialRa||initialMb||initialSc||initialTc||initialRc||initialTb||initialGb||initialMc,
     "track is not in the explicit native admission list");
 auto effectiveTrack=initialMc?rr::platform::HostTrack50IM::MC:initialGb?rr::platform::HostTrack50IM::GB:initialTb?rr::platform::HostTrack50IM::TB:initialRc?rr::platform::HostTrack50IM::RC:initialTc?rr::platform::HostTrack50IM::TC:initialSc?rr::platform::HostTrack50IM::SC:initialMb?rr::platform::HostTrack50IM::MB:initialRa?rr::platform::HostTrack50IM::RA:initialMa?rr::platform::HostTrack50IM::MA:initialSa?rr::platform::HostTrack50IM::SA:
     initialGa?rr::platform::HostTrack50IM::GA:
     track_session_assets50IG::codeEquals(initialTrack->world->code,"GC")?
     rr::platform::HostTrack50IM::GC:rr::platform::HostTrack50IM::TA;
 auto selectedTrack=effectiveTrack;
 // Source UI starts on real Rider19. TA/SA/MA/GA Course entry prepares that exact pod
 // in camera slot0; the old eight-racer direct preview remains unchanged.
 if(originalMainApplication.enabled&&(initialGa||initialMa||initialSa||track_session_assets50IG::codeEquals(initialTrack->world->code,"TA")))
  selectedRacer60=19;
 if(initialRa)selectedRacer60=9;
 if(initialMb)selectedRacer60=16;
 if(initialSc)selectedRacer60=7;
 if(initialTc)selectedRacer60=24;
 if(initialRc)selectedRacer60=17;
 if(initialTb)selectedRacer60=18;
 if(initialGb)selectedRacer60=23;
 if(initialMc)selectedRacer60=1;
 const char* incompleteWorldSetting=std::getenv("RR_GC_INCOMPLETE_WORLD");
 const char* selectedModeSetting=std::getenv("RR_RACE_MODE");
 // This admits an authenticated cold destination request only. runSession
 // independently requires the actual world/camera/audio/effect providers;
 // source profile visibility and lock state still own Course availability.
 const auto gcDefaultOnRequested=[](const char* name){
  const char* value=std::getenv(name);return !value||std::string(value)!="0";
 };
 const char* gcAllAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool gcNativeWorldRequested=(sharedHumanCourseRequested||
     (gcAllAiSetting&&std::string(gcAllAiSetting)=="1"))&&!startAtMenu&&
     gcDefaultOnRequested("RR_RACE")&&gcDefaultOnRequested("RR_NATIVE_SELECTED_LIGHTS")&&
     gcDefaultOnRequested("RR_GC_NATIVE_GATE")&&gcDefaultOnRequested("RR_GC_NATIVE_WATER");
 const rr::platform::HostTrackAdmission50IM trackAdmission{
     incompleteWorldSetting&&std::string(incompleteWorldSetting)=="1",
     selectedModeSetting&&std::string(selectedModeSetting)=="2",false,gcNativeWorldRequested,raNativeWorldRequested,mbNativeWorldRequested,scNativeWorldRequested,tcNativeWorldRequested,rcNativeWorldRequested,tbNativeWorldRequested,gbNativeWorldRequested,mcNativeWorldRequested};
 std::uint64_t sessionIndex=0;
 std::unique_ptr<original_course_targets50KX::ConsumedLaunch> consumedCourseLaunch;
 for(;;){
  if(captureRun.deadlineExpired())break;
  PlayerLoadingSession50DU playerLoading(originalMainApplication,
      enterOriginalUi?"Preparing Main":"Preparing race",
      enterOriginalUi?"UI":rr::platform::hostTrackCode50IM(effectiveTrack),sessionIndex);
  try{PlayerLoadingSession50DU::checkStop();}
  catch(const PlayerLoadingCancelled50DU&){break;}
  const auto exit=enterOriginalUi?
      runOriginalFrontend50LA(resolvedCorpusPath.c_str(),headlessLive,headlessLive?argv[3]:nullptr,sessionIndex,
          applicationClock,trackAdmission,originalMainApplication,courseColdLoad,captureRun):
      runSession50DU(resolvedCorpusPath.c_str(),headlessLive,headlessLive?argv[3]:nullptr,
      useCheckpoint?&activeCheckpoint:nullptr,nextCheckpoint,lapContinuity,sessionIndex,
      applicationClock,collisionNormalMode,startAtMenu,selectedRacer60,effectiveAudioSettings,selectedAudioSettings,
      effectiveTrack,selectedTrack,trackAdmission,originalMainApplication,enterOriginalUi,applicationControls,directApplicationRandom,cameraStartup350,courseColdLoad,nativeWaterGlobals,captureRun,consumedCourseLaunch.get(),SessionServices50DU{&monotonicNow50DU,&reportHost50DU,&reportRenderer50DU});
  // Expire even on restart/return: only a new actual Course consumption can
  // create another owner. Stack unwinding also releases it on session failure.
  // Exception: Restart Race of a consumed TA race re-runs the same consumed
  // constructor (tournament intro/race owners), so it renews the launch.
  std::unique_ptr<original_course_targets50KX::ConsumedLaunch> restartLaunch;
  if(exit==SessionExit50DU::Restart&&consumedCourseLaunch&&
     consumedCourseLaunch->targetsTrack(rr::platform::HostTrack50IM::TA)){
   const bool renewed=consumedCourseLaunch->renewForRestart(sessionIndex+1,restartLaunch);
   std::cerr<<"consumed_launch_restart track=TA renewed="<<renewed<<'\n';
  }
  consumedCourseLaunch.reset();
  // Session renderer, GPU, audio and world owners are actually retired.
  // Normal player entry retains the SAME native host/context, not merely the
  // Video wrapper. Explicit diagnostics retain their original closed host.
  playerLoading.end();
  if(originalMainApplication.enabled&&originalMainApplication.lightManager->nativeGameplayGeneration()){
   OriginalUiLightManagerLifetimeDiagnostic50KG lightDiagnostic;
   require50DU(originalMainApplication.lightManager->endNativeGameplay(sessionIndex+1,lightDiagnostic),
       "native gameplay lighting lease did not retire after real world destruction");
  }
  if(exit==SessionExit50DU::ResultsHallReturn)
   require50DU(originalMainApplication.finishNativeResultsHallDeparture50LA(),
       "Results Hall transfer requires real completed session/Light retirement");
  if(exit==SessionExit50DU::PauseMenuReturn)
   require50DU(originalMainApplication.finishNativePauseMenuDeparture50LA(),
       "Pause menu transfer requires real completed session/Display/Light retirement");
  if(exit==SessionExit50DU::Quit)break;
  const char* originalAiSetting=std::getenv("RR_ORIGINAL_AI_FIRST_SLICE");
  const char* allAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
  const bool allOriginalAi=allAiSetting&&std::string(allAiSetting)=="1";
  if(exit==SessionExit50DU::CourseColdLoad){
   require50DU(originalMainApplication.enabled&&originalMainApplication.riderProfile&&
       courseColdLoad.consumeForNextSession(originalMainApplication.riderProfile,
           sessionIndex+1,effectiveTrack,selectedRacer60,consumedCourseLaunch),
       "Course cold-load request lost its real application/profile lifetime");
   selectedTrack=effectiveTrack;
  }else require50DU(!courseColdLoad.pending(),"unconsumed Course cold-load request");
  const bool humanCourse=originalMainApplication.enabled&&!allOriginalAi&&
      (effectiveTrack==rr::platform::HostTrack50IM::TA||
       (rootedHumanCourseSupported&&((effectiveTrack==rr::platform::HostTrack50IM::GC&&gcNativeWorldRequested)||
        effectiveTrack==rr::platform::HostTrack50IM::GA||
        effectiveTrack==rr::platform::HostTrack50IM::MA||effectiveTrack==rr::platform::HostTrack50IM::SA||
        effectiveTrack==rr::platform::HostTrack50IM::MB||
        (effectiveTrack==rr::platform::HostTrack50IM::RA&&raNativeWorldRequested)||
        (effectiveTrack==rr::platform::HostTrack50IM::SC&&scNativeWorldRequested)||
        (effectiveTrack==rr::platform::HostTrack50IM::TC&&tcNativeWorldRequested)||
        (effectiveTrack==rr::platform::HostTrack50IM::RC&&rcNativeWorldRequested)||
        (effectiveTrack==rr::platform::HostTrack50IM::TB&&tbNativeWorldRequested)||
        (effectiveTrack==rr::platform::HostTrack50IM::GB&&gbNativeWorldRequested)||
        (effectiveTrack==rr::platform::HostTrack50IM::MC&&mcNativeWorldRequested))));
  if(allOriginalAi||humanCourse){
   require50DU((exit==SessionExit50DU::Restart||exit==SessionExit50DU::ReturnOriginalMain||exit==SessionExit50DU::CourseColdLoad||exit==SessionExit50DU::ResultsHallReturn||exit==SessionExit50DU::PauseMenuReturn)&&
       rr::platform::validHostTrack50IM(effectiveTrack)&&selectedTrack==effectiveTrack,
       "native roster rebuild requires an explicit supported restart/return/Course request");
  }else if(originalAiSetting&&std::string(originalAiSetting)=="1"){
   require50DU(false,"mixed original AI first slice rejected session rebuild");
  }
  enterOriginalUi=exit==SessionExit50DU::ReturnOriginalMain||exit==SessionExit50DU::ResultsHallReturn||exit==SessionExit50DU::PauseMenuReturn;
  require50DU(!enterOriginalUi||originalMainApplication.enabled,"original Main application owner unavailable");
  if(exit==SessionExit50DU::Restart){effectiveAudioSettings=selectedAudioSettings;effectiveTrack=selectedTrack;}
  if(restartLaunch&&effectiveTrack==rr::platform::HostTrack50IM::TA)consumedCourseLaunch=std::move(restartLaunch);
  require50DU(sessionIndex<std::numeric_limits<std::uint64_t>::max()-1,"audio session index exhausted");
  ++sessionIndex;
  startAtMenu=false; // An explicit restart/start immediately enters its fresh session.
  const char* raceSetting=std::getenv("RR_RACE");
  const bool restartingRace=!(raceSetting&&std::string(raceSetting)=="0");
  useCheckpoint=exit==SessionExit50DU::Checkpoint&&!restartingRace;
  if(restartingRace&&exit==SessionExit50DU::Checkpoint)
   std::cerr<<"Native checkpoint rebuild restarts the entire host race, all lap owners and countdown.\n";
  if(useCheckpoint){activeCheckpoint=nextCheckpoint;std::cerr<<"Starting source-derived host checkpoint session.\n";}
  else {lapContinuity={};std::cerr<<"Starting a fresh authored-grid session.\n";}
 }
 // All real session-local audio/GL/world owners and the matching gameplay
 // Light lease have ended. Transient Course exits deliberately do not retire
 // these process-owned services; only this terminal root continuation does.
 std::string frontendShutdownError;
 const bool frontendServicesClosed=originalMainApplication.finishNativeFrontendAudio(frontendShutdownError);
 require50DU(frontendServicesClosed,frontendShutdownError.c_str());
 rr::platform::GameHostDiagnostic50ER terminalHostDiagnostic;
 require50DU(originalMainApplication.closePlayerWindow50DU(terminalHostDiagnostic),
     "terminal native player window cleanup rejected");
 captureRun.reportClosed();
 return captureRun.timedOut()||(captureRun.navigate()&&(!captureRun.evidenceComplete()||!captureRun.videoHoldComplete()))?3:0;
 }catch(...){
  const auto originalFailure=std::current_exception();
  std::string terminalError;
  if(!originalMainApplication.finishNativeFrontendAudio(terminalError)){
   // Do not let the strict source-owner destructor hide the first failure,
   // and never release live backing merely to make exception unwinding pass.
   try{std::rethrow_exception(originalFailure);}
   catch(const std::exception& e){native_last_failure50PD::record(e.what());std::cerr<<"interactive drive original failure: "<<e.what()<<'\n';}
   catch(...){std::cerr<<"interactive drive original failure: non-standard exception\n";}
   std::cerr<<"terminal frontend source retirement unresolved: "<<terminalError
       <<"; retaining actual owners by terminating\n";
   std::terminate();
  }
  rr::platform::GameHostDiagnostic50ER terminalHostDiagnostic;
  if(!originalMainApplication.closePlayerWindow50DU(terminalHostDiagnostic)){
   reportHost50DU(terminalHostDiagnostic);std::terminate();
  }
  std::rethrow_exception(originalFailure);
 }
}catch(const std::exception&e){std::cerr<<"interactive drive: "<<e.what()<<'\n';return 1;}}

int main(int argc,char** argv){
 // PS2 EE float rule (no subnormals) for the whole game thread.
 rr::game::enforcePs2FloatMode50LA();
#if defined(_WIN32)
 if(argc==1||(argc==3&&std::string(argv[1])=="--assets"))
  return windows_native_entry50LB::run(nativeApplicationMain50DU,argc,argv,argc==1);
#endif
 return nativeApplicationMain50DU(argc,argv);
}

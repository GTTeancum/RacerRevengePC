#pragma once
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../platform/linux/native/game_host50er.h"
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include "json.hpp"

namespace windows_capture50LD {
using Json=nlohmann::json;
// Retain Windows evidence identifiers for existing consumers. Linux uses the
// same native SDL window/readback owner, but never claims Windows measurements.
#if defined(_WIN32)
inline constexpr const char* platformName="windows";
inline constexpr const char* unattendedMode="unattended_windows";
inline constexpr const char* focusedMode="focused_windows";
inline constexpr const char* baselineSchema="native-windows-track-fps-v1";
inline constexpr const char* measurementScope="GPU-completed actual gameplay images submitted through the Windows SDL swap; physical monitor scanout is not measured";
#elif defined(__linux__)
inline constexpr const char* platformName="linux";
inline constexpr const char* unattendedMode="unattended_linux";
inline constexpr const char* focusedMode="focused_linux";
inline constexpr const char* baselineSchema="native-linux-track-fps-v1";
inline constexpr const char* measurementScope="GPU-completed actual gameplay images submitted through the Linux SDL swap; physical monitor scanout is not measured";
#else
#error "Window framebuffer evidence requires the native Windows or Linux SDL window host"
#endif
inline void require(bool okay,const char* message){if(!okay)throw std::runtime_error(message);}
inline bool option(const char* name){
 const auto* value=std::getenv(name);
 require(!value||std::string(value)=="0"||std::string(value)=="1","window capture boolean option must be 0 or 1");
 return value&&value[0]=='1';
}
enum class Stage : std::uint8_t {main,rider,course,race,other,count};
inline const char* stageName(Stage stage)noexcept{
 switch(stage){case Stage::main:return "main";case Stage::rider:return "rider";
 case Stage::course:return "course";case Stage::race:return "track";default:return "other";}
}
// One real application wall budget, retained through every UI/race cold load.
// It never changes SDL's observed focus, input release, or source clocks.
class Run final {
 using Clock=std::chrono::steady_clock;
 const Clock::time_point started_=Clock::now();
 std::filesystem::path directory_;
 bool enabled_=false,automatic_=false,unattended_=false,navigate_=false,timeoutReported_=false;
 bool navigateSa_=false,navigateOcco_=false,navigateDudGa_=false,navigateBenGa_=false,navigateAlderMa_=false,navigateOdyMa_=false,navigateGasganoSa_=false,navigateMawhonicRa_=false,navigateTournamentTa_=false;
 std::uint32_t videoHoldSeconds_=0;
 Clock::time_point baselineFinished_{};
 std::uint32_t wallSeconds_=180;
 std::array<std::filesystem::path,static_cast<std::size_t>(Stage::count)> images_{},receipts_{};
 std::filesystem::path baselineFile_;
 std::uint32_t laterRaceBaselines_=0;
 bool restartOnce_=false,restartIssued_=false,logsOnly_=false;
 Json firstRaceEvidence_;
public:
 Run(){
  const auto* directory=std::getenv("RR_WINDOW_CAPTURE_DIR");
  if(directory){require(*directory,"RR_WINDOW_CAPTURE_DIR must be a nonempty existing directory");directory_=directory;enabled_=true;}
  logsOnly_=option("RR_EVIDENCE_LOGS_ONLY");
  automatic_=option("RR_WINDOW_CAPTURE_AUTO");unattended_=option("RR_UNATTENDED_CAPTURE");
  const auto* route=std::getenv("RR_UNATTENDED_UI_ROUTE");
  require(!route||std::string(route)=="main-rider-course-ta"||std::string(route)=="main-rider-course-sa"||
      std::string(route)=="main-occo20-course-sa"||std::string(route)=="main-dud04-course-ga"||std::string(route)=="main-ben02-course-ga"||std::string(route)=="main-alder10-course-ma"||std::string(route)=="main-ody11-course-ma"||std::string(route)=="main-gasgano07-course-sa"||std::string(route)=="main-mawhonic09-course-ra"||std::string(route)=="main-tournament-course-ta",
      "unsupported unattended original UI route");
  navigate_=route!=nullptr;
  restartOnce_=option("RR_UNATTENDED_RESTART_ONCE");
  require(!restartOnce_||(enabled_&&automatic_&&unattended_&&navigate_),
      "native restart test requires an unattended automatic UI route");
  navigateOcco_=route&&std::string(route)=="main-occo20-course-sa";
  navigateDudGa_=route&&std::string(route)=="main-dud04-course-ga";
  navigateBenGa_=route&&std::string(route)=="main-ben02-course-ga";
  navigateAlderMa_=route&&std::string(route)=="main-alder10-course-ma";
  navigateOdyMa_=route&&std::string(route)=="main-ody11-course-ma";
  navigateGasganoSa_=route&&std::string(route)=="main-gasgano07-course-sa";
  navigateMawhonicRa_=route&&std::string(route)=="main-mawhonic09-course-ra";
  navigateTournamentTa_=route&&std::string(route)=="main-tournament-course-ta";
  const auto* testProfile=std::getenv("RR_ORIGINAL_TEST_PROFILE");
  require(!navigateGasganoSa_||(testProfile&&std::string(testProfile)=="gasgano"),
      "Gasgano/SA capture requires RR_ORIGINAL_TEST_PROFILE=gasgano");
  require(!navigateMawhonicRa_||(testProfile&&std::string(testProfile)=="mawhonic-ryloth"),
      "Mawhonic/RA capture requires RR_ORIGINAL_TEST_PROFILE=mawhonic-ryloth");
  require(!testProfile||(navigateGasganoSa_&&std::string(testProfile)=="gasgano")||
      (navigateMawhonicRa_&&std::string(testProfile)=="mawhonic-ryloth"),
      "RR_ORIGINAL_TEST_PROFILE requires its exact bounded racer/course route");
  navigateSa_=navigateOcco_||(route&&std::string(route)=="main-rider-course-sa");
  require(enabled_||(!automatic_&&!unattended_&&!navigate_),"window capture options require RR_WINDOW_CAPTURE_DIR");
  require(!navigate_||(unattended_&&automatic_),"unattended original UI route requires unattended automatic capture");
  if(const auto* seconds=std::getenv("RR_UNATTENDED_SECONDS")){
   require(unattended_&&*seconds,"RR_UNATTENDED_SECONDS requires explicit unattended capture");
   // Software-rendered RA needs time for the unchanged original intro.
   // Default budgets and all Windows/other-route limits remain300 seconds.
#if defined(__linux__)
   const std::uint32_t maximum=navigateMawhonicRa_?1200u:300u;
#else
   // Full-race test runs (three laps plus load and intro) need up to 600.
   const std::uint32_t maximum=600u;
#endif
   std::uint32_t value=0;
   for(const char* p=seconds;*p;++p){require(*p>='0'&&*p<='9'&&value<=maximum,"invalid unattended wall budget");value=value*10u+std::uint32_t(*p-'0');}
   require(value>0&&value<=maximum,"unattended wall budget exceeds bounded platform/route limit");wallSeconds_=value;
  }
  if(const auto* seconds=std::getenv("RR_WINDOW_VIDEO_HOLD_SECONDS")){
   require(unattended_&&std::string(seconds)=="75","video hold requires unattended capture and exact 75-second recording window");
   videoHoldSeconds_=75;
  }
  if(!enabled_)return;
  require(directory_.is_absolute(),"RR_WINDOW_CAPTURE_DIR must be absolute, independent of the asset working directory");
  require(std::filesystem::is_directory(directory_),"RR_WINDOW_CAPTURE_DIR must already exist");
  require(!navigate_||option("RR_TRACK_FPS_BASELINE"),"complete unattended UI evidence route requires RR_TRACK_FPS_BASELINE=1");
  if(unattended_){
   const auto* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
   // Rooted TA/GA/MA menu routes retain the real human Player0. This permits
   // process-local menu navigation only; it neither supplies host input nor
   // claims a race is ready. Course Confirm still uses its actual destination.
   const bool directHumanRoute=allAi&&std::string(allAi)=="0"&&navigate_&&
       !navigateSa_&&!navigateOcco_&&!navigateGasganoSa_&&!navigateMawhonicRa_;
   require((allAi&&std::string(allAi)=="1")||directHumanRoute,
       "unattended capture requires all-AI mode or an explicit supported direct-human menu route");
  }
  std::cerr<<platformName<<"_capture_run "<<Json{{"evidence_mode",evidenceMode()},{"automatic",automatic_},
      {"video_hold_seconds",videoHoldSeconds_},{"ui_route",navigateTournamentTa_?"main-tournament-course-ta":navigateMawhonicRa_?"main-mawhonic09-course-ra":navigateGasganoSa_?"main-gasgano07-course-sa":navigateOdyMa_?"main-ody11-course-ma":navigateAlderMa_?"main-alder10-course-ma":navigateBenGa_?"main-ben02-course-ga":navigateDudGa_?"main-dud04-course-ga":navigateOcco_?"main-occo20-course-sa":navigateSa_?"main-rider-course-sa":navigate_?"main-rider-course-ta":"none"},{"wall_budget_seconds",unattended_?wallSeconds_:0},
      {"test_profile",testProfileName()},{"actual_focus_preserved",true},{"directory",directory_.string()}}.dump()<<'\n';
 }
 bool logsOnly()const noexcept{return logsOnly_;}
 bool enabled()const noexcept{return enabled_;}
 bool automatic()const noexcept{return automatic_;}
 bool unattended()const noexcept{return unattended_;}
 bool navigate()const noexcept{return navigate_;}
 bool navigateSa()const noexcept{return navigateSa_;}
 bool navigateOcco()const noexcept{return navigateOcco_;}
 bool navigateDudGa()const noexcept{return navigateDudGa_;}
 bool navigateBenGa()const noexcept{return navigateBenGa_;}
 bool navigateAlderMa()const noexcept{return navigateAlderMa_;}
 bool navigateOdyMa()const noexcept{return navigateOdyMa_;}
 bool navigateGasganoSa()const noexcept{return navigateGasganoSa_;}
 bool navigateMawhonicRa()const noexcept{return navigateMawhonicRa_;}
 bool navigateTournamentTa()const noexcept{return navigateTournamentTa_;}
 const char* testProfileName()const noexcept{return navigateMawhonicRa_?"mawhonic-ryloth":navigateGasganoSa_?"gasgano":"none";}
 bool videoHoldComplete()const noexcept{return !videoHoldSeconds_||(!baselineFile_.empty()&&
     Clock::now()-baselineFinished_>=std::chrono::seconds(videoHoldSeconds_));}
 bool timedOut()const noexcept{return timeoutReported_;}
 const char* evidenceMode()const noexcept{return unattended_?unattendedMode:"interactive_window_framebuffer";}
 const std::filesystem::path& directory()const noexcept{return directory_;}
 void noteCapture(Stage stage,bool settled,const std::filesystem::path& image,const std::filesystem::path& receipt){
  if(!settled||stage==Stage::other)return;
  const auto i=static_cast<std::size_t>(stage);images_[i]=image;receipts_[i]=receipt;
 }
 void noteBaseline(const Json& receipt){
  if(!enabled_)return;
  // A later race in the same process (Results -> Garage/Course -> race) keeps
  // the first race's baseline as the evidence receipt and records its own
  // measurement beside it under a new numbered name; nothing is overwritten.
  if(!baselineFile_.empty()&&!restartIssued_){
   const auto later=directory_/("track-fps-baseline-race"+std::to_string(laterRaceBaselines_+2)+".json");
   require(!std::filesystem::exists(later),"track baseline receipt would overwrite an existing file");
   std::ofstream output(later,std::ios::binary);require(bool(output),"cannot open track FPS baseline receipt");
   output<<receipt.dump(2)<<'\n';output.close();require(bool(output),"track FPS baseline receipt write failed");
   ++laterRaceBaselines_;return;
  }
  const auto file=directory_/(restartIssued_?"track-fps-baseline-restarted.json":"track-fps-baseline.json");
  require(baselineFile_.empty()&&!std::filesystem::exists(file),"track baseline receipt would overwrite an existing file");
  std::ofstream output(file,std::ios::binary);require(bool(output),"cannot open track FPS baseline receipt");
  Json evidence=receipt;if(navigateGasganoSa_||navigateMawhonicRa_)evidence["test_profile"]=testProfileName();
  output<<evidence.dump(2)<<'\n';output.close();require(bool(output),"track FPS baseline receipt write failed");
  baselineFinished_=Clock::now();baselineFile_=file;
 }
 bool evidenceComplete()const noexcept{
  return !timeoutReported_&&!receipts_[static_cast<std::size_t>(Stage::main)].empty()&&
      !receipts_[static_cast<std::size_t>(Stage::rider)].empty()&&
      !receipts_[static_cast<std::size_t>(Stage::course)].empty()&&
      !receipts_[static_cast<std::size_t>(Stage::race)].empty()&&!baselineFile_.empty();
 }
 // Process-local native restart test. This consumes a completed first race
 // observation, not a Pause-menu event or a claim that a new race is ready.
 bool takeNativeRestartRequest(){
  if(!restartOnce_||restartIssued_)return false;
  require(evidenceComplete()&&videoHoldComplete(),"restart test requires completed first-race evidence");
  const auto i=static_cast<std::size_t>(Stage::race);
  firstRaceEvidence_={{"image_file",images_[i].string()},{"receipt_file",receipts_[i].string()},
      {"baseline_file",baselineFile_.string()}};
  images_[i].clear();receipts_[i].clear();baselineFile_.clear();baselineFinished_={};
  restartIssued_=true;
  std::cerr<<platformName<<"_native_restart_test_requested "<<firstRaceEvidence_.dump()<<'\n';
  return true;
 }
 // The outer application calls this only after normal owned session cleanup.
 void reportClosed(){
  if(!enabled_)return;
  Json receipt={{"evidence_mode",evidenceMode()},{"completed",evidenceComplete()&&videoHoldComplete()},
      {"video_hold_seconds",videoHoldSeconds_},{"video_hold_complete",videoHoldComplete()},
      {"cleanup_complete",true},{"timed_out",timeoutReported_},{"baseline_file",baselineFile_.string()}};
  receipt["native_restart_test"]={{"requested",restartOnce_},{"issued",restartIssued_},
      {"first_race",firstRaceEvidence_},{"pause_menu_tested",false}};
  if(navigateGasganoSa_||navigateMawhonicRa_)receipt["test_profile"]=testProfileName();
  for(const auto stage:{Stage::main,Stage::rider,Stage::course,Stage::race}){
   const auto i=static_cast<std::size_t>(stage);
   receipt[stageName(stage)]={{"image_file",images_[i].string()},{"receipt_file",receipts_[i].string()}};
  }
  const auto file=directory_/"window-evidence-summary.json";
  require(!std::filesystem::exists(file),"window evidence summary would overwrite an existing file");
  std::ofstream output(file,std::ios::binary);require(bool(output),"cannot open window evidence summary");
  output<<receipt.dump(2)<<'\n';output.close();require(bool(output),"window evidence summary write failed");
  std::cerr<<platformName<<((evidenceComplete()&&videoHoldComplete())?"_evidence_complete ":"_evidence_incomplete ")<<receipt.dump()<<'\n';
 }
 bool deadlineExpired(){
  if(!unattended_||Clock::now()-started_<std::chrono::seconds(wallSeconds_))return false;
  if(!timeoutReported_){timeoutReported_=true;std::cerr<<platformName<<"_capture_timeout "<<Json{
      {"evidence_mode",evidenceMode()},{"wall_budget_seconds",wallSeconds_},{"completed",false},
      {"reason","shared application wall budget expired"}}.dump()<<'\n';}
  return true;
 }
};
// Owns bounded cold readback storage. Requests never draw a substitute image:
// the caller may record only after its genuine publication and window swap.
class Session final {
 Run* run_=nullptr;
 std::filesystem::path directory_;
 std::vector<std::uint8_t> rgba_,row_;
 std::array<bool,static_cast<std::size_t>(Stage::count)> captured_{};
 std::uint64_t serial_=0,session_=0;
 bool requested_=false,limitReported_=false;
#if defined(__linux__)
 struct PendingWindowImage {
  const rr::platform::VehicleWindowGl50DS* renderer=nullptr;
  const rr::platform::GameHost50ER* host=nullptr;
  std::int32_t width=0,height=0;
  std::uint64_t swapSequence=0;
 } pending_;
#endif
public:
 Session()=default;
 Session(const Session&)=delete;
 Session& operator=(const Session&)=delete;
 // The callback must submit this exact BACK image, without another window draw.
 // Frontend uses swapWindow; gameplay uses completeGameplayDisplay, whose only
 // intervening draw is a private history copy. Failure clears unpublished bytes'
 // authority. No renderer or host retains this cold owner's storage.
 template<class Swap> bool swapAndCapture(rr::platform::VehicleWindowGl50DS& renderer,
     rr::platform::GameHost50ER& host,bool capture,std::uint64_t swapSequence,Swap swap){
  if(run_&&run_->logsOnly())capture=false;
#if defined(__linux__)
  pending_={};
  rr::platform::VehicleWindowDiagnostic50DS diagnostic;
  const auto before=host.info();
  if(capture){
   require(run_&&serial_<64&&swapSequence&&host.created()&&host.current()&&host.hasWindow()&&!before.minimized,
       "pre-swap capture requires its live window owner");
   if(!renderer.readbackWindowBackRgba(host,rgba_.data(),rgba_.size(),
       before.drawableWidth,before.drawableHeight,diagnostic)){
    std::cerr<<platformName<<"_framebuffer_readback_failed "<<Json{
        {"error",static_cast<unsigned>(diagnostic.error)},
        {"operation",diagnostic.operation?diagnostic.operation:"unknown"},{"gl_error",diagnostic.glError},
        {"drawable_width",before.drawableWidth},{"drawable_height",before.drawableHeight}}.dump()<<'\n';
    throw std::runtime_error("actual pre-swap window back-buffer readback failed; no private-target fallback");
   }
  }
  if(!swap())return false;
  if(capture){
   const auto& after=host.info();
   require(host.created()&&host.current()&&host.hasWindow()&&!after.minimized&&
       before.drawableWidth==after.drawableWidth&&before.drawableHeight==after.drawableHeight,
       "window capture drawable changed during paired swap");
   pending_={&renderer,&host,diagnostic.readbackWidth,diagnostic.readbackHeight,swapSequence};
  }
  return true;
#else
  (void)renderer;(void)host;(void)capture;(void)swapSequence;
  return swap(); // Windows retains its original post-swap GL_FRONT readback.
#endif
 }
 void discardPendingWindowImage()noexcept{
#if defined(__linux__)
  pending_={};
#endif
 }
 void prepare(Run& run,std::uint64_t sessionIndex,std::int32_t width,std::int32_t height,const char* role){
  if(!run.enabled())return;
  require(width>0&&height>0&&std::uint64_t(width)*std::uint64_t(height)<=std::numeric_limits<std::size_t>::max()/4u,
      "invalid window capture allocation bounds");
  run_=&run;session_=sessionIndex+1;
  directory_=run.directory()/(std::string(role)+"-session-"+std::to_string(session_));
  require(std::filesystem::create_directory(directory_),"window capture session directory already exists; supply a fresh output directory");
  if(!run.logsOnly()){
   rgba_.resize(static_cast<std::size_t>(width)*static_cast<std::size_t>(height)*4u);
   row_.resize(static_cast<std::size_t>(width)*3u);
  }
 }
 bool enabled()const noexcept{return run_!=nullptr;}
 std::uint64_t serial()const noexcept{return serial_;}
 bool captured(Stage stage)const noexcept{return captured_[static_cast<std::size_t>(stage)];}
 void request(const rr::platform::WindowInput50DQ& input){
  if(!run_||!input.focused||input.minimized||input.activityLost)return;
  if(input.pressed[69])requested_=true; // Existing genuine SDL F12 key edge.
 }
 bool due(Stage stage,bool settled){
  if(!run_)return false;
  if(serial_>=64){
   if((requested_||run_->automatic())&&!limitReported_){limitReported_=true;
    std::cerr<<"Window framebuffer capture stopped at the 64-image per-session limit.\n";}
   requested_=false;return false;
  }
  return requested_||(run_->automatic()&&settled&&stage!=Stage::other&&!captured(stage));
 }
 void record(rr::platform::VehicleWindowGl50DS& renderer,const rr::platform::GameHost50ER& host,
     Stage stage,bool settled,Json details){
  require(run_&&serial_<64,"window capture owner is unavailable");
  const auto& info=host.info();
  if(run_->logsOnly()){
   require(host.created()&&host.hasWindow()&&!info.minimized,"log observation needs a live submitted window");
   details["evidence_mode"]="native_logs_only";details["pixel_source"]="none";
   details["image_file"]=nullptr;details["visual_verified"]=false;
   details["physical_desktop_scanout_verified"]=false;
   details["stage"]=stageName(stage);details["sequence"]=serial_+1;details["session"]=session_;
   details["settled"]=settled;details["window_swap_submitted"]=true;
   details["width"]=info.drawableWidth;details["height"]=info.drawableHeight;
   const auto receiptFile=directory_/(std::string(stageName(stage))+"-"+std::to_string(serial_+1)+".json");
   require(!std::filesystem::exists(receiptFile),"log receipt would overwrite evidence");
   std::ofstream receipt(receiptFile,std::ios::binary);require(bool(receipt),"cannot open log receipt");
   receipt<<details.dump(2)<<'\n';receipt.close();require(bool(receipt),"log receipt write failed");
   run_->noteCapture(stage,settled,{},receiptFile);
   ++serial_;if(settled)captured_[static_cast<std::size_t>(stage)]=true;requested_=false;
   std::cerr<<platformName<<"_stage_observation "<<details.dump()<<'\n';return;
  }
#if defined(__linux__)
  const auto paired=pending_;pending_={}; // Consume once, even if file publication fails.
  require(paired.renderer==&renderer&&paired.host==&host&&host.created()&&host.current()&&
      host.hasWindow()&&!info.minimized&&paired.width==info.drawableWidth&&paired.height==info.drawableHeight&&
      details.at("window_swaps").get<std::uint64_t>()==paired.swapSequence,
      "window capture lacks its exact successful swap/owner/drawable pairing");
  const auto width=paired.width,height=paired.height;
#else
  rr::platform::VehicleWindowDiagnostic50DS diagnostic;
  if(!renderer.readbackWindowFrontRgba(rgba_.data(),rgba_.size(),info.drawableWidth,info.drawableHeight,diagnostic)){
   std::cerr<<platformName<<"_framebuffer_readback_failed "<<Json{{"error",static_cast<unsigned>(diagnostic.error)},
       {"operation",diagnostic.operation?diagnostic.operation:"unknown"},{"gl_error",diagnostic.glError},
       {"drawable_width",info.drawableWidth},{"drawable_height",info.drawableHeight}}.dump()<<'\n';
   throw std::runtime_error("actual window front-buffer readback failed; no private-target fallback");
  }
  const auto width=diagnostic.readbackWidth,height=diagnostic.readbackHeight;
#endif
  require(width>0&&height>0&&static_cast<std::size_t>(width)*3u<=row_.size(),"window capture row exceeds cold capacity");
  const auto stem=std::string(stageName(stage))+"-"+std::to_string(serial_+1);
  const auto file=directory_/(stem+".ppm"),pending=directory_/(stem+".ppm.pending");
  require(!std::filesystem::exists(file)&&!std::filesystem::exists(pending),"window capture would overwrite an existing file");
  std::ofstream image(pending,std::ios::binary);
  require(bool(image),"cannot open pending window framebuffer image");
  image<<"P6\n"<<width<<' '<<height<<"\n255\n";
  for(std::int32_t y=height;y>0;--y){
   const auto offset=static_cast<std::size_t>(y-1)*static_cast<std::size_t>(width)*4u;
   for(std::int32_t x=0;x<width;++x)for(unsigned channel=0;channel<3;++channel)
    row_[static_cast<std::size_t>(x)*3u+channel]=rgba_[offset+static_cast<std::size_t>(x)*4u+channel];
   image.write(reinterpret_cast<const char*>(row_.data()),static_cast<std::streamsize>(width)*3);
  }
  image.close();require(bool(image),"window framebuffer image write failed");
  std::filesystem::rename(pending,file);
  details["evidence_mode"]=run_->evidenceMode();
#if defined(__linux__)
  details["pixel_source"]="default_window_framebuffer_GL_BACK_before_matching_actual_SDL_window_swap";
  details["paired_window_swap_sequence"]=paired.swapSequence;
  details["capture_scope"]="pre-swap default-window framebuffer paired with actual swap; not OS desktop or GL_FRONT capture";
#else
  details["pixel_source"]="default_framebuffer_GL_FRONT_after_actual_SDL_window_swap";
#endif
  if(run_->navigateGasganoSa()||run_->navigateMawhonicRa())details["test_profile"]=run_->testProfileName();
  details["physical_desktop_scanout_verified"]=false;
  details["stage"]=stageName(stage);details["sequence"]=serial_+1;details["session"]=session_;
  details["settled"]=settled;
  details["actual_focused"]=info.focused;details["actual_minimized"]=info.minimized;
  details["window_swap_submitted"]=true;details["width"]=width;details["height"]=height;
  details["image_file"]=file.string();
  const auto receiptFile=directory_/(stem+".json");
  std::ofstream receipt(receiptFile,std::ios::binary);
  require(bool(receipt),"cannot open window framebuffer receipt");receipt<<details.dump(2)<<'\n';
  receipt.close();require(bool(receipt),"window framebuffer receipt write failed");
  run_->noteCapture(stage,settled,file,receiptFile);
  ++serial_;if(settled)captured_[static_cast<std::size_t>(stage)]=true;requested_=false;
  std::cerr<<platformName<<"_framebuffer_capture "<<details.dump()<<'\n';
 }
};
}

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
 bool navigateSa_=false,navigateOcco_=false,navigateDudGa_=false,navigateBenGa_=false,navigateAlderMa_=false,navigateOdyMa_=false,navigateGasganoSa_=false;
 std::uint32_t videoHoldSeconds_=0;
 Clock::time_point baselineFinished_{};
 std::uint32_t wallSeconds_=180;
 std::array<std::filesystem::path,static_cast<std::size_t>(Stage::count)> images_{},receipts_{};
 std::filesystem::path baselineFile_;
public:
 Run(){
  const auto* directory=std::getenv("RR_WINDOW_CAPTURE_DIR");
  if(directory){require(*directory,"RR_WINDOW_CAPTURE_DIR must be a nonempty existing directory");directory_=directory;enabled_=true;}
  automatic_=option("RR_WINDOW_CAPTURE_AUTO");unattended_=option("RR_UNATTENDED_CAPTURE");
  const auto* route=std::getenv("RR_UNATTENDED_UI_ROUTE");
  require(!route||std::string(route)=="main-rider-course-ta"||std::string(route)=="main-rider-course-sa"||
      std::string(route)=="main-occo20-course-sa"||std::string(route)=="main-dud04-course-ga"||std::string(route)=="main-ben02-course-ga"||std::string(route)=="main-alder10-course-ma"||std::string(route)=="main-ody11-course-ma"||std::string(route)=="main-gasgano07-course-sa",
      "unsupported unattended original UI route");
  navigate_=route!=nullptr;
  navigateOcco_=route&&std::string(route)=="main-occo20-course-sa";
  navigateDudGa_=route&&std::string(route)=="main-dud04-course-ga";
  navigateBenGa_=route&&std::string(route)=="main-ben02-course-ga";
  navigateAlderMa_=route&&std::string(route)=="main-alder10-course-ma";
  navigateOdyMa_=route&&std::string(route)=="main-ody11-course-ma";
  navigateGasganoSa_=route&&std::string(route)=="main-gasgano07-course-sa";
  const auto* testProfile=std::getenv("RR_ORIGINAL_TEST_PROFILE");
  require(!navigateGasganoSa_||(testProfile&&std::string(testProfile)=="gasgano"),
      "Gasgano/SA capture requires RR_ORIGINAL_TEST_PROFILE=gasgano");
  require(!testProfile||(navigateGasganoSa_&&std::string(testProfile)=="gasgano"),
      "RR_ORIGINAL_TEST_PROFILE requires exact main-gasgano07-course-sa route and gasgano value");
  navigateSa_=navigateOcco_||(route&&std::string(route)=="main-rider-course-sa");
  require(enabled_||(!automatic_&&!unattended_&&!navigate_),"window capture options require RR_WINDOW_CAPTURE_DIR");
  require(!navigate_||(unattended_&&automatic_),"unattended original UI route requires unattended automatic capture");
  if(const auto* seconds=std::getenv("RR_UNATTENDED_SECONDS")){
   require(unattended_&&*seconds,"RR_UNATTENDED_SECONDS requires explicit unattended capture");
   std::uint32_t value=0;
   for(const char* p=seconds;*p;++p){require(*p>='0'&&*p<='9'&&value<=300,"invalid unattended wall budget");value=value*10u+std::uint32_t(*p-'0');}
   require(value>0&&value<=300,"unattended wall budget must be 1..300 seconds");wallSeconds_=value;
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
   require(allAi&&std::string(allAi)=="1","unattended capture requires original AI for all racers, including camera slot 0");
  }
  std::cerr<<platformName<<"_capture_run "<<Json{{"evidence_mode",evidenceMode()},{"automatic",automatic_},
      {"video_hold_seconds",videoHoldSeconds_},{"ui_route",navigateGasganoSa_?"main-gasgano07-course-sa":navigateOdyMa_?"main-ody11-course-ma":navigateAlderMa_?"main-alder10-course-ma":navigateBenGa_?"main-ben02-course-ga":navigateDudGa_?"main-dud04-course-ga":navigateOcco_?"main-occo20-course-sa":navigateSa_?"main-rider-course-sa":navigate_?"main-rider-course-ta":"none"},{"wall_budget_seconds",unattended_?wallSeconds_:0},
      {"test_profile",navigateGasganoSa_?"gasgano":"none"},{"actual_focus_preserved",true},{"directory",directory_.string()}}.dump()<<'\n';
 }
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
  const auto file=directory_/"track-fps-baseline.json";
  require(baselineFile_.empty()&&!std::filesystem::exists(file),"track baseline receipt would overwrite an existing file");
  std::ofstream output(file,std::ios::binary);require(bool(output),"cannot open track FPS baseline receipt");
  Json evidence=receipt;if(navigateGasganoSa_)evidence["test_profile"]="gasgano";
  output<<evidence.dump(2)<<'\n';output.close();require(bool(output),"track FPS baseline receipt write failed");
  baselineFinished_=Clock::now();baselineFile_=file;
 }
 bool evidenceComplete()const noexcept{
  return !timeoutReported_&&!images_[static_cast<std::size_t>(Stage::main)].empty()&&
      !images_[static_cast<std::size_t>(Stage::rider)].empty()&&
      !images_[static_cast<std::size_t>(Stage::course)].empty()&&
      !images_[static_cast<std::size_t>(Stage::race)].empty()&&!baselineFile_.empty();
 }
 // The outer application calls this only after normal owned session cleanup.
 void reportClosed(){
  if(!enabled_)return;
  Json receipt={{"evidence_mode",evidenceMode()},{"completed",evidenceComplete()&&videoHoldComplete()},
      {"video_hold_seconds",videoHoldSeconds_},{"video_hold_complete",videoHoldComplete()},
      {"cleanup_complete",true},{"timed_out",timeoutReported_},{"baseline_file",baselineFile_.string()}};
  if(navigateGasganoSa_)receipt["test_profile"]="gasgano";
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
public:
 void prepare(Run& run,std::uint64_t sessionIndex,std::int32_t width,std::int32_t height,const char* role){
  if(!run.enabled())return;
  require(width>0&&height>0&&std::uint64_t(width)*std::uint64_t(height)<=std::numeric_limits<std::size_t>::max()/4u,
      "invalid window capture allocation bounds");
  run_=&run;session_=sessionIndex+1;
  directory_=run.directory()/(std::string(role)+"-session-"+std::to_string(session_));
  require(std::filesystem::create_directory(directory_),"window capture session directory already exists; supply a fresh output directory");
  rgba_.resize(static_cast<std::size_t>(width)*static_cast<std::size_t>(height)*4u);
  row_.resize(static_cast<std::size_t>(width)*3u);
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
 void record(rr::platform::VehicleWindowGl50DS& renderer,const rr::platform::GameHostInfo50ER& info,
     Stage stage,bool settled,Json details){
  require(run_&&serial_<64,"window capture owner is unavailable");
  rr::platform::VehicleWindowDiagnostic50DS diagnostic;
  if(!renderer.readbackWindowFrontRgba(rgba_.data(),rgba_.size(),info.drawableWidth,info.drawableHeight,diagnostic)){
   std::cerr<<platformName<<"_framebuffer_readback_failed "<<Json{{"error",static_cast<unsigned>(diagnostic.error)},
       {"operation",diagnostic.operation?diagnostic.operation:"unknown"},{"gl_error",diagnostic.glError},
       {"drawable_width",info.drawableWidth},{"drawable_height",info.drawableHeight}}.dump()<<'\n';
   throw std::runtime_error("actual window front-buffer readback failed; no private-target fallback");
  }
  const auto width=diagnostic.readbackWidth,height=diagnostic.readbackHeight;
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
  details["evidence_mode"]=run_->evidenceMode();details["pixel_source"]="default_framebuffer_GL_FRONT_after_actual_SDL_window_swap";
  if(run_->navigateGasganoSa())details["test_profile"]="gasgano";
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

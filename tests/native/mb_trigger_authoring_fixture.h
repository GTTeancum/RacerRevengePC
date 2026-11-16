#pragma once
#include "ga_trigger_authoring_fixture.h"
#include "course_trigger_sources50ct.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// MB trigger source projection, read from the course's own TOB by the common
// trigger reader (course_trigger_sources50ct.h -> sc_trigger_authoring50la.h).
// No action callback, collision membership, reset completion or runtime
// admission is provided by this owner. The stock values below are receipts.
namespace mb_trigger_authoring {
namespace a=rr::assets;namespace g=rr::game;
// Stock receipts (and the dead static-model-params include's pins).
inline constexpr std::size_t count=3,sphereCount=3,boxCount=0;
inline constexpr std::size_t archiveBytes=4421779,decodedBytes=16392192,tobBytes=19909;
inline constexpr std::uint32_t archiveEntries=487;
inline constexpr const char* archiveSha="6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca";
inline constexpr const char* tobSha="f25f10fff204f124e5525ff2c4a7c1e72f9ce17476e1404d1932f6ab6e8c776d";
// Capacity: the common reader's store; MB's native group (NativeTriggerGroup50GA)
// refuses a live set above its own capacity when the count is set.
inline constexpr std::size_t capacity=course_trigger_sources50CT::capacity;
struct Source {
 std::string name;
 std::uint32_t zone=0,actionCount=1,shapeType=5,targetNodeIndex=0;
 g::CollisionSphere48J sphere{};
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=true,reset=false;
 std::int32_t randomTriggerPercent=100;
};
namespace detail {using namespace ga_trigger_authoring::detail;}
class Owner final {
 course_trigger_sources50CT::Owner reader_;
 std::array<Source,capacity> sources_{};
 std::size_t live_=0;
 bool attempted_=false,ready_=false;
 static void need(bool value,const std::string& reason){if(!value)throw std::runtime_error(reason);}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 // aiNodeCount: the live AI spline's node count; zoneCount: the loaded world's.
 bool load(const std::filesystem::path& archivePath,std::uint32_t aiNodeCount,
           std::uint32_t zoneCount,std::string& error){
  if(attempted_){error="MB trigger owner requires a fresh stable lifetime";return false;}attempted_=true;
  try{
   course_trigger_sources50CT::Support support;support.reset=true; // MB's chain implements the pose reset
   if(!reader_.load(archivePath,course_trigger_sources50CT::stockMb,zoneCount,aiNodeCount,error,support))return false;
   // The MB route has no animated-world owner. PORT STAND-IN (flagged): a
   // non-stock TOB that authors animations is refused until the common
   // animation reader (W3) exists.
   need(!reader_.animationCount()||reader_.stockTob(),"MB course archive authors animations: needs the common animation reader (W3)");
   const auto live=reader_.liveCount();
   need(live<=capacity,"MB trigger extent exceeds the reader capacity");
   std::array<Source,capacity> next{};
   for(std::size_t i=0;i<live;++i){const auto& in=reader_.source(i);auto& s=next[i];
    const auto label="MB trigger"+std::to_string(i+1)+": ";
    // PORT STAND-IN (flagged): MB's native action owner implements one LostAI
    // action on a sphere trigger that starts enabled and repeats; any other
    // authored shape/activation/action is refused, not ignored.
    need(in.shapeType==5,label+"box trigger is not supported on the MB native chain");
    need(in.actionCount==1&&in.actions[0].kind==course_trigger_sources50CT::Action::LostAI,
         label+"action other than one LostAI is not supported on the MB native chain");
    need(in.startEnabled&&!in.oneTimeOnly,label+"StartEnabled FALSE / OneTimeOnly TRUE is not supported on the MB native chain");
    need(in.sphere.radius>0.f,label+"sphere radius rejected");
    s.name=in.name;s.zone=in.zone;s.actionCount=1;s.shapeType=5;s.sphere=in.sphere;
    s.targetNodeIndex=in.actions[0].targetNodeIndex;s.reset=in.actions[0].reset;
    s.startEnabled=true;s.oneTimeOnly=false;s.ignorePlayer=in.ignorePlayer;
    s.randomTriggerPercent=in.randomTriggerPercent;
    need(s.targetNodeIndex<aiNodeCount,label+"LostAI target outside the live AI spline");
   }
   sources_=std::move(next);live_=live;ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_;}
 std::size_t liveCount()const noexcept{return ready_?live_:0;}
 // Bounds-checked against the live prefix, never the capacity tail.
 const Source& source(std::size_t i)const {
  if(!ready_||i>=live_)throw std::runtime_error("MB trigger source outside the live prefix");return sources_[i];
 }
 const std::vector<std::uint8_t>& originalTob()const {
  if(!ready_)throw std::runtime_error("MB original TOB owner not ready");return reader_.originalTob();
 }
};
} // namespace mb_trigger_authoring

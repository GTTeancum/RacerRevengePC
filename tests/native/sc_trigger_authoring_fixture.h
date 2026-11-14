#pragma once
#include "sc_trigger_authoring50la.h"
#include "ma_trigger_authoring_fixture.h"
#include <memory>
#include <optional>

// SC owns its authenticated source; only the already-admitted native shape
// arithmetic is shared with MA. This does not claim original trig/VU parity.
namespace sc_trigger_authoring {
namespace a=rr::assets;namespace g=rr::game;
// Capacity (native trigger group); the live count is Owner::liveCount().
inline constexpr std::size_t count=sc_trigger_authoring50LA::count;
inline constexpr std::size_t tobBytes=64047;
inline constexpr const char* tobSha="473351310a098008a477f9e72a6fc7e03d8bb808adcd79f5d810f90aa332b727";
struct Source {
 std::string name;
 std::uint32_t zone=0,actionCount=1,shapeType=5,targetNodeIndex=0;
 g::CollisionSphere48J sphere{};
 g::NativeTriggerBox50MA box{};
 float yaw=0,pitch=0,roll=0;
 bool startEnabled=false,oneTimeOnly=false,ignorePlayer=false;
 std::int32_t randomTriggerPercent=0;
 std::optional<bool> authoredReset;
 bool effectiveReset1B5A60=false;
};
class Owner final {
 std::shared_ptr<const sc_cold_sources50LA::Owner> cold_;
 sc_trigger_authoring50LA::Owner authored_;
 std::array<Source,count> sources_{};
 std::size_t live_=0;
 bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool decode(std::shared_ptr<const sc_cold_sources50LA::Owner> cold,std::string& error){
  if(attempted_){error="SC trigger source requires a fresh lifetime";return false;}attempted_=true;
  try{using ga_trigger_authoring::detail::need;
   const auto* data=cold?cold->sourcesDecoded():nullptr;
   need(data&&data->ai.track.count&&data->ai.gatesPrepared,
       "SC trigger requires its decoded world and AI source");
   sc_trigger_authoring50LA::Owner::Diagnostic diagnostic;
   need(authored_.decode(cold,diagnostic),"SC original trigger authoring rejected");
   const auto* input=authored_.sources();const auto live=authored_.liveCount();
   need(input&&live<=count,"SC trigger extent exceeds the native trigger group");
   std::array<Source,count> next{};
   for(std::size_t i=0;i<live;++i){const auto& raw=(*input)[i];auto& s=next[i];
    s.name=raw.name;s.zone=raw.sourceZoneOrdinal;s.targetNodeIndex=raw.targetAiNodeIndex;
    s.startEnabled=raw.startEnabled;s.oneTimeOnly=raw.oneTimeOnly;
    s.ignorePlayer=raw.ignoreTrigger.has_value();s.randomTriggerPercent=static_cast<std::int32_t>(raw.randomTriggerPercent);
    s.authoredReset=raw.authoredReset;s.effectiveReset1B5A60=raw.resetDefault1B5A60;
    // FLAG stand-in: the native action owner implements Reset off and a 100%
    // trigger chance only; other authored values are refused, not ignored.
    need(!s.effectiveReset1B5A60&&raw.randomTriggerPercent==100.f,
        "SC trigger Reset or RandomTriggerPercent other than 100 is not supported natively");
    for(unsigned k=0;k<3;++k)s.sphere.center.v[k]=raw.position.values[k];
    if(raw.shape==sc_trigger_authoring50LA::Shape::sphere){
     need(raw.radius.has_value(),"SC sphere radius missing");s.sphere.radius=*raw.radius;
    }else{
     need(raw.box&&raw.yaw&&raw.pitch&&raw.roll,"SC box authoring missing");s.shapeType=6;
     for(unsigned k=0;k<3;++k)s.box.half.v[k]=raw.box->values[k];
     s.yaw=*raw.yaw;s.pitch=*raw.pitch;s.roll=*raw.roll;
     s.box.world=ma_trigger_authoring::detail::boxWorld(s.sphere.center,s.yaw,s.pitch,s.roll);
     need(g::validNativeTriggerBox50MA(s.box),"SC native box geometry rejected");
     s.sphere.radius=ma_trigger_authoring::detail::boxRadius(s.box);
    }
    need(std::isfinite(s.sphere.radius)&&s.sphere.radius>0.f,"SC trigger coarse radius rejected");
   }
   sources_=std::move(next);live_=live;cold_=std::move(cold);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_;}
 std::size_t liveCount()const noexcept{return ready_?live_:0;}
 const Source& source(std::size_t i)const{
  if(!ready_||i>=live_)throw std::runtime_error("SC trigger source not ready");return sources_.at(i);
 }
 const sc_cold_sources50LA::Owner* coldSource()const noexcept{return ready_?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{
  if(!ready_)throw std::runtime_error("SC trigger source not ready");return cold_->sourcesDecoded()->memberBytes[3];
 }
};
} // namespace sc_trigger_authoring

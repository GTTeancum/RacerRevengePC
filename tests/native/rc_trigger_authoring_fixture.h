#pragma once
#include "rc_trigger_authoring50la.h"
#include "ma_trigger_authoring_fixture.h"
#include <memory>
#include <optional>

// RC owns its authenticated source; only the already-admitted native shape
// arithmetic is shared with MA. This does not claim original trig/VU parity.
namespace rc_trigger_authoring {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=11,sphereCount=11,boxCount=0;
inline constexpr std::size_t tobBytes=124930;
inline constexpr const char* tobSha="71b1f9609c0a6ce84d71ff69355d9cfcbc8771eb7ddb8876473b4fddd6e23e6d";
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
 std::shared_ptr<const rc_cold_sources50LA::Owner> cold_;
 rc_trigger_authoring50LA::Owner authored_;
 std::array<Source,count> sources_{};
 bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool decode(std::shared_ptr<const rc_cold_sources50LA::Owner> cold,std::string& error){
  if(attempted_){error="RC trigger source requires a fresh lifetime";return false;}attempted_=true;
  try{using ga_trigger_authoring::detail::need;
   need(cold&&cold->ready()&&cold->world().zones.size()==15&&cold->ai().valid()&&cold->ai().count()==209,
       "RC trigger requires its actual decoded world and AI source");
   rc_trigger_authoring50LA::Owner::Diagnostic diagnostic;
   need(authored_.decode(cold,diagnostic),"RC original trigger authoring rejected");
   const auto* input=authored_.sources();need(input&&input->size()==count,"RC original trigger extent");
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){const auto& raw=(*input)[i];auto& s=next[i];
    s.name=raw.name;s.zone=raw.sourceZoneOrdinal;s.targetNodeIndex=raw.targetAiNodeIndex;
    s.startEnabled=raw.startEnabled;s.oneTimeOnly=raw.oneTimeOnly;
    s.ignorePlayer=raw.ignoreTrigger.has_value();s.randomTriggerPercent=100;
    s.authoredReset=raw.authoredReset;s.effectiveReset1B5A60=raw.resetDefault1B5A60;
    need(!s.authoredReset&&!s.effectiveReset1B5A60&&raw.randomTriggerPercent==100.f,
        "RC original reset/RNG source changed");
    for(unsigned k=0;k<3;++k)s.sphere.center.v[k]=raw.position.values[k];
    if(raw.shape==rc_trigger_authoring50LA::Shape::sphere){
     need(raw.radius.has_value(),"RC sphere radius missing");s.sphere.radius=*raw.radius;
    }else{
     need(raw.box&&raw.yaw&&raw.pitch&&raw.roll,"RC box authoring missing");s.shapeType=6;
     for(unsigned k=0;k<3;++k)s.box.half.v[k]=raw.box->values[k];
     s.yaw=*raw.yaw;s.pitch=*raw.pitch;s.roll=*raw.roll;
     s.box.world=ma_trigger_authoring::detail::boxWorld(s.sphere.center,s.yaw,s.pitch,s.roll);
     need(g::validNativeTriggerBox50MA(s.box),"RC native box geometry rejected");
     s.sphere.radius=ma_trigger_authoring::detail::boxRadius(s.box);
    }
    need(std::isfinite(s.sphere.radius)&&s.sphere.radius>0.f,"RC trigger coarse radius rejected");
   }
   sources_=std::move(next);cold_=std::move(cold);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 std::size_t descriptorPayloadBytes()const noexcept {
  const auto inner=authored_.descriptorPayloadBytes();
  if(inner==std::numeric_limits<std::size_t>::max())return inner;
  const auto extra=inner-sizeof(rc_trigger_authoring50LA::Owner);
  if(extra>std::numeric_limits<std::size_t>::max()-sizeof(Owner))return std::numeric_limits<std::size_t>::max();
  std::size_t n=sizeof(Owner)+extra;
  for(const auto& s:sources_){if(s.name.capacity()>=std::numeric_limits<std::size_t>::max()-n)return std::numeric_limits<std::size_t>::max();n+=s.name.capacity()+1;}
  return n;
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const{
  if(!ready_)throw std::runtime_error("RC trigger source not ready");return sources_.at(i);
 }
 const rc_cold_sources50LA::Owner* coldSource()const noexcept{return ready_?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{
  if(!ready_)throw std::runtime_error("RC trigger source not ready");return cold_->originalTob();
 }
};
} // namespace rc_trigger_authoring

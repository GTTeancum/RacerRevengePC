#pragma once
#include "tc_cold_sources50la.h"
#include "ma_trigger_authoring_fixture.h"
#include "course_trigger_sources50ct.h"
#include <memory>
#include <optional>

// TC trigger sources: read from TC's own held TOB by the common trigger reader
// (course_trigger_sources50ct.h -> sc_trigger_authoring50la.h), the reader the
// original uses for every course. Capacity store (nativeTriggerCapacity50TG)
// with the TOB's own live count; 0 authored = no trigger state. The stock
// values (22 triggers, gate Animation + two global Sounds at trigger 11, TOB
// size/SHA) are receipts (course_member_identity / course_count lines).
namespace tc_trigger_authoring50TC {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t capacity=course_trigger_sources50CT::capacity;
inline constexpr std::size_t maxActions=sc_trigger_authoring50LA::maxActions;
inline constexpr std::size_t stockCount=22; // receipt only
// Stock TOB identity: a receipt here; the gate and static-model owners
// (tc_static_model_params50tc.h) still reads these.
inline constexpr std::size_t tobBytes=25512;
inline constexpr const char* tobSha="e080f96e2657e1331ac1f829faa880a5e984ebf1c5fab393daa6539534dd0573";
enum class Action {LostAI,Animation,Sound};
struct ActionSource {
 Action kind=Action::LostAI;
 std::uint32_t targetNodeIndex=0,animationIndex=0;
 std::optional<bool> authoredReset;
 bool effectiveReset1B5A60=false;
 std::string filename;
 bool positional=false,isGlobalSound=false;
};
struct Source {
 std::string name;
 std::uint32_t sourceOrdinal=0,zone=0,actionCount=0,shapeType=5;
 g::CollisionSphere48J sphere{};g::NativeTriggerBox50MA box{};
 float yaw=0,pitch=0,roll=0;
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;
 std::int32_t randomTriggerPercent=100;
 std::array<ActionSource,maxActions> actions{};
};
namespace detail {
using ga_trigger_authoring::detail::need;
using ga_trigger_authoring::detail::block;
using ga_trigger_authoring::detail::optional;
using ga_trigger_authoring::detail::field;
using ga_trigger_authoring::detail::integer;
using ga_trigger_authoring::detail::number;
using ga_trigger_authoring::detail::vector;
inline bool boolean(std::string s){
 for(auto& c:s)if(c>='a'&&c<='z')c=static_cast<char>(c-'a'+'A');
 return ga_trigger_authoring::detail::boolean(s);
}
}
inline constexpr course_trigger_sources50CT::Stock stock{"TC","tracks\\tc.tob",nullptr,0,tobSha,tobBytes,stockCount};
// TC authoring, strongly retaining the single actual cold source (its decoded
// TOB is borrowed). No archive, parameter, MAP/AI, libm or world decode is repeated.
class Owner final {
 std::shared_ptr<const tc_cold_sources50LA::Owner> cold_;
 course_trigger_sources50CT::Owner reader_;
 std::array<Source,capacity> source_{};std::size_t live_=0;bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool decode(std::shared_ptr<const tc_cold_sources50LA::Owner> cold,std::string& error){
  if(attempted_){error="TC trigger source requires a fresh lifetime";return false;}attempted_=true;
  try{using namespace detail;
   need(cold&&cold->ready(),"TC trigger requires its actual ready source world and AI");
   // TC chain: LostAI (no pose Reset), Animation, ordinary Sound; no chance
   // roll, no AI ignore list, no PodSound/Particles (refused).
   course_trigger_sources50CT::Support support;support.sound=true;
   if(!reader_.load(cold->triggerParameters(),cold->originalTob(),stock,cold->world().zones.size(),cold->ai().count(),error,support))return false;
   std::array<Source,capacity> next{};const auto live=reader_.liveCount();
   for(std::size_t i=0;i<live;++i){const auto& in=reader_.source(i);auto& s=next[i];
    s.name=in.name;s.sourceOrdinal=in.sourceOrdinal;s.zone=in.zone;s.actionCount=in.actionCount;s.shapeType=in.shapeType;
    s.sphere=in.sphere;s.box=in.box;s.yaw=in.yaw;s.pitch=in.pitch;s.roll=in.roll;
    s.startEnabled=in.startEnabled;s.oneTimeOnly=in.oneTimeOnly;s.ignorePlayer=in.ignorePlayer;
    s.randomTriggerPercent=in.randomTriggerPercent;
    need(s.actionCount>=1&&s.actionCount<=maxActions,"TC trigger action slots");
    for(std::uint32_t k=0;k<s.actionCount;++k){const auto& ia=in.actions[k];auto& oa=s.actions[k];
     switch(ia.readerKind){
      case sc_trigger_authoring50LA::ActionKind::LostAI:
       oa.kind=Action::LostAI;oa.targetNodeIndex=ia.targetNodeIndex;oa.authoredReset=ia.authoredReset;
       oa.effectiveReset1B5A60=ia.authoredReset.value_or(false);break; // 1B5A60 default 0
      case sc_trigger_authoring50LA::ActionKind::Animation:
       oa.kind=Action::Animation;oa.animationIndex=ia.animationIndex;break; // one-based resolver 2DA190
      case sc_trigger_authoring50LA::ActionKind::Sound:
       oa.kind=Action::Sound;oa.filename=ia.filename;oa.positional=ia.positional;oa.isGlobalSound=ia.isGlobalSound;break;
      default:need(false,"TC trigger action kind outside the TC native chain");
     }
    }
   }
   source_=std::move(next);live_=live;cold_=std::move(cold);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_&&cold_&&cold_->ready();}
 std::size_t liveCount()const noexcept{return ready()?live_:0;}
 const Source& source(std::size_t i)const{
  if(!ready()||i>=live_)throw std::runtime_error("TC trigger source outside the live prefix");return source_[i];
 }
 // First live trigger authoring an action of `kind` (liveCount() when none):
 // the gate animation and the global Sound owner find their trigger by data.
 std::size_t firstTriggerWith(Action kind)const noexcept{
  for(std::size_t i=0;i<liveCount();++i)for(std::uint32_t k=0;k<source_[i].actionCount;++k)
   if(source_[i].actions[k].kind==kind)return i;
  return liveCount();
 }
 bool authors(Action kind)const noexcept{return firstTriggerWith(kind)<liveCount();}
 const tc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{if(!ready())throw std::runtime_error("TC trigger source not ready");return cold_->originalTob();}
};
}

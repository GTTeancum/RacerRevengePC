#pragma once
#include "tb_cold_sources50la.h"
#include "ma_trigger_authoring_fixture.h"
#include "../../src/assets/import_world48f.h"
#include <limits>
#include "course_trigger_sources50ct.h"
#include <optional>

// TB trigger sources: read from TB's own held TOB by the common trigger reader
// (course_trigger_sources50ct.h -> sc_trigger_authoring50la.h), the reader the
// original uses for every course ([Triggers] NumberOfTriggers, TriggerN /
// TriggerNActionK). Capacity store (nativeTriggerCapacity50TG) with the TOB's
// own live count; 0 authored = no trigger state. The stock values (16
// triggers, door Animation + PodSound at trigger 16, TOB SHA) are receipts
// (course_trigger_sources: course_member_identity / course_count lines).
namespace tb_trigger_authoring50TB {
namespace a=rr::assets;namespace g=rr::game;
// Stock TOB identity: a receipt (it moved here from the retired TB door owner).
inline constexpr std::size_t tobBytes=106139;
inline constexpr const char* tobSha="12c908874736ea91dcd6d3f8af97413a0cfe3bcd87cfce0798e8ff86fe5fb0bc";
namespace detail {
using ga_trigger_authoring::detail::need;using ga_trigger_authoring::detail::block;
using ga_trigger_authoring::detail::optional;using ga_trigger_authoring::detail::field;
using ga_trigger_authoring::detail::integer;using ga_trigger_authoring::detail::number;
using ga_trigger_authoring::detail::vector;
inline bool boolean(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=char(c-'a'+'A');return ga_trigger_authoring::detail::boolean(s);}
inline void add(std::size_t& n,std::size_t x)noexcept{n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;}
template<class T> inline void payload(std::size_t& n,const std::vector<T>& v)noexcept {
 if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();else add(n,v.capacity()*sizeof(T));
}
inline void textPayload(std::size_t& n,const std::string& s)noexcept{add(n,s.capacity());add(n,1);}
}
inline constexpr std::size_t capacity=course_trigger_sources50CT::capacity;
inline constexpr std::size_t maxActions=sc_trigger_authoring50LA::maxActions;
inline constexpr std::size_t stockCount=16; // receipt only
enum class Action {LostAI,Animation,PodSound};
struct ActionSource {
 Action kind=Action::LostAI;std::uint32_t targetNodeIndex=0,animationIndex=0;
 std::optional<std::uint32_t> authoredTargetNodeIndex;
 std::optional<bool> authoredReset;bool effectiveReset1B5A60=false;
 std::string filename;g::Vector3_48C soundPosition{};float minDistance=0,maxDistance=0;
 const a::ParameterBlock48F* original=nullptr;
};
struct Source {
 std::string name;std::uint32_t sourceOrdinal=0,zone=0,actionCount=0,shapeType=5;
 g::CollisionSphere48J sphere{};g::NativeTriggerBox50MA box{};float yaw=0,pitch=0,roll=0;
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;std::int32_t randomTriggerPercent=100;
 std::optional<bool> authoredStartEnabled;std::optional<float> authoredRandomTriggerPercent;
 std::optional<std::string> authoredIgnoreTrigger;
 std::array<ActionSource,maxActions> actions{};const a::ParameterBlock48F* original=nullptr;
};
inline constexpr course_trigger_sources50CT::Stock stock{"TB","tracks\\tb.tob",nullptr,0,
 tobSha,tobBytes,stockCount};
// The shared cold source document remains the only decoded TOB; this owner
// borrows it (retained by cold_) and grants no collider ID, C80 lease or action.
class Owner final {
 std::shared_ptr<const tb_cold_sources50LA::Owner> cold_;
 course_trigger_sources50CT::Owner reader_;
 std::array<Source,capacity> source_{};std::size_t live_=0;
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 // Capacity reservation: the staged/retained Source arrays (this owner and the
 // common reader), up to (3+maxActions) retained strings per trigger slot at
 // 64 bytes each, twice (staging), and 1024 bytes of diagnostics. The TOB is
 // retained by the cold source and is never copied here.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  return sizeof(Owner)+sizeof(std::array<Source,capacity>)+2u*capacity*(3u+maxActions)*64u+1024u;
 }
 bool decode(std::shared_ptr<const tb_cold_sources50LA::Owner> cold,std::string& error){
  if(thread_!=std::this_thread::get_id()){error="TB trigger source thread mismatch";return false;}
  if(attempted_){error="TB trigger source requires a fresh owner";return false;}attempted_=true;
  try{using namespace detail;
   need(cold&&cold->ready(),"TB trigger source requires its ready cold source");
   const auto& p=cold->triggerParameters();
   // TB chain: LostAI (no pose Reset), Animation, PodSound; no chance roll,
   // no AI ignore list, no ordinary Sound/Particles (refused by the reader).
   if(!reader_.load(p,cold->originalTob(),stock,cold->world().zones.size(),cold->ai().count(),error))return false;
   std::array<Source,capacity> next{};const auto live=reader_.liveCount();
   for(std::size_t i=0;i<live;++i){const auto& in=reader_.source(i);auto& s=next[i];
    s.name=in.name;s.sourceOrdinal=in.sourceOrdinal;s.zone=in.zone;s.actionCount=in.actionCount;s.shapeType=in.shapeType;
    s.sphere=in.sphere;s.box=in.box;s.yaw=in.yaw;s.pitch=in.pitch;s.roll=in.roll;
    s.startEnabled=in.startEnabled;s.oneTimeOnly=in.oneTimeOnly;s.ignorePlayer=in.ignorePlayer;
    s.randomTriggerPercent=in.randomTriggerPercent;s.original=in.original;
    if(in.startEnabledAuthored)s.authoredStartEnabled=in.startEnabled;
    if(in.randomTriggerPercentAuthored)s.authoredRandomTriggerPercent=static_cast<float>(in.randomTriggerPercent);
    s.authoredIgnoreTrigger=in.ignoreTrigger;
    need(s.actionCount>=1&&s.actionCount<=maxActions,"TB trigger action slots");
    for(std::uint32_t k=0;k<s.actionCount;++k){const auto& ia=in.actions[k];auto& oa=s.actions[k];oa.original=ia.original;
     switch(ia.readerKind){
      case sc_trigger_authoring50LA::ActionKind::LostAI:
       oa.kind=Action::LostAI;oa.targetNodeIndex=ia.targetNodeIndex;oa.authoredReset=ia.authoredReset;
       if(optional(*ia.original,"targetnodeindex"))oa.authoredTargetNodeIndex=ia.targetNodeIndex;
       // Original 1B5A60 default 0 when Reset is omitted.
       oa.effectiveReset1B5A60=ia.authoredReset.value_or(false);break;
      case sc_trigger_authoring50LA::ActionKind::Animation:
       oa.kind=Action::Animation;oa.animationIndex=ia.animationIndex;break;
      case sc_trigger_authoring50LA::ActionKind::PodSound:
       oa.kind=Action::PodSound;oa.filename=ia.filename;oa.soundPosition=ia.position;
       oa.minDistance=ia.minDistance;oa.maxDistance=ia.maxDistance;break;
      default:need(false,"TB trigger action kind outside the TB native chain");
     }
    }
   }
   source_=std::move(next);live_=live;cold_=std::move(cold);
   need(descriptorPayloadBytes()<=maximumAdditionalPayloadBytes(),"TB trigger descriptor reservation exceeded");
   ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 // Live prefix [0,liveCount()); bounds-checked, never the capacity tail.
 std::size_t liveCount()const noexcept{return ready()?live_:0;}
 const Source& source(std::size_t i)const{
  if(!ready()||i>=live_)throw std::runtime_error("TB trigger source outside the live prefix");return source_[i];
 }
 // First live trigger authoring an action of `kind` (liveCount() when none):
 // the door's Animation and PodSound owners find their trigger by data.
 std::size_t firstTriggerWith(Action kind)const noexcept{
  for(std::size_t i=0;i<liveCount();++i)for(std::uint32_t k=0;k<source_[i].actionCount;++k)
   if(source_[i].actions[k].kind==kind)return i;
  return liveCount();
 }
 bool authors(Action kind)const noexcept{return firstTriggerWith(kind)<liveCount();}
 const tb_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{if(!ready())throw std::runtime_error("TB trigger source unavailable");return cold_->originalTob();}
 std::size_t descriptorPayloadBytes()const noexcept {
  using namespace detail;std::size_t n=sizeof(Owner);
  for(std::size_t i=0;i<live_;++i){const auto& s=source_[i];textPayload(n,s.name);if(s.authoredIgnoreTrigger)textPayload(n,*s.authoredIgnoreTrigger);
   for(const auto& a:s.actions)textPayload(n,a.filename);}
  return n;
 }
};
}

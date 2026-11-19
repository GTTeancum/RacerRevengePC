#pragma once
#include "mc_scene_authoring50mc.h"
#include "course_trigger_sources50ct.h"
// MC trigger sources: read from MC's own held TOB by the common trigger reader
// (course_trigger_sources50ct.h -> sc_trigger_authoring50la.h), the reader the
// original uses for every course. Capacity store (nativeTriggerCapacity50TG)
// with the TOB's own live count; 0 authored = no trigger state. The stock
// values (21 triggers, 20 LostAI + bridge Animation, targets, 14 box + 7
// sphere, percent spellings, TOB identity) are receipts. RandomTriggerPercent
// reads the integer prefix (158B30: "99.996002" -> 99), as the reader does.
namespace mc_trigger_authoring50MC {
namespace a=rr::assets;namespace g=rr::game;namespace detail=mc_scene_authoring50MC::detail;
inline constexpr std::size_t capacity=course_trigger_sources50CT::capacity;
inline constexpr std::size_t stockCount=21; // receipt only
enum class Action {LostAI,Animation};
struct Source {
 const a::ParameterBlock48F* original=nullptr;const a::ParameterBlock48F* actionOriginal=nullptr;
 const std::string* name=nullptr;
 std::uint32_t sourceOrdinal=0,zone=0,actionCount=1,shapeType=5,targetNodeIndex=0,animationIndex=0;
 g::Vector3_48C position{},boxHalf{};float radius=0,yaw=0,pitch=0,roll=0,authoredRandomTriggerPercent=100;
 std::int32_t randomTriggerPercent=100;bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;
 std::optional<bool> authoredStartEnabled,authoredReset,authoredActionOneTimeOnly;
 bool effectiveReset1B5A60=false;Action action=Action::LostAI;
 // Box orientation remains authored Euler data; the native shape6 projection
 // is mc_trigger_geometry50mc.h's (same policy as TB/MA).
};
inline constexpr course_trigger_sources50CT::Stock stock{"MC","tracks\\mc.tob",nullptr,0,
 "7070700a884c24a98a0a89d4ea58e61e34f9a89e19af3ca239779c56c0dd14b0",186642,stockCount};
class Owner final {
 std::shared_ptr<const mc_cold_sources50LA::Owner> cold_;
 course_trigger_sources50CT::Owner reader_;
 std::array<Source,capacity> source_{};std::size_t live_=0;
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 // Fixed capacity records (this owner and the common reader's projection);
 // the TOB stays in the cold source; labels/filenames are bounded strings.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept{
  return sizeof(Owner)+sizeof(std::array<Source,capacity>)+2u*capacity*(3u+sc_trigger_authoring50LA::maxActions)*64u+4096u;
 }
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner);}
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::size_t available,std::string& error){
  if(thread_!=std::this_thread::get_id()){error="MC authoring thread mismatch";return false;}
  if(attempted_){error="MC authoring requires a fresh lifetime";return false;}attempted_=true;
  if(available>32u*1024u*1024u||available<maximumAdditionalPayloadBytes()){
   error="MC authoring reservation exceeds remaining allowance";return false;}
  try{using namespace detail;
   need(bool(cold),"MC cold source absent");const auto& p=detail::document(*cold);
   // MC chain: LostAI (no pose Reset), Animation, the trigger-chance roll;
   // no AI ignore list, no PodSound/Sound/Particles (refused).
   course_trigger_sources50CT::Support support;support.randomPercent=true;
   if(!reader_.load(p,cold->originalTob(),stock,cold->world().zones.size(),cold->ai().count(),error,support))return false;
   std::array<Source,capacity> next{};const auto live=reader_.liveCount();
   for(std::size_t i=0;i<live;++i){const auto& in=reader_.source(i);auto& s=next[i];
    s.original=in.original;s.sourceOrdinal=in.sourceOrdinal;s.zone=in.zone;s.position=in.sphere.center;
    s.name=in.original?optional(*in.original,"name"):nullptr;s.oneTimeOnly=in.oneTimeOnly;s.startEnabled=in.startEnabled;
    if(in.startEnabledAuthored)s.authoredStartEnabled=in.startEnabled;
    s.ignorePlayer=in.ignorePlayer;s.randomTriggerPercent=in.randomTriggerPercent;
    if(const auto* v=in.original?optional(*in.original,"randomtriggerpercent"):nullptr)s.authoredRandomTriggerPercent=number(*v);
    s.shapeType=in.shapeType;s.yaw=in.yaw;s.pitch=in.pitch;s.roll=in.roll;
    if(in.shapeType==6)s.boxHalf=in.box.half;else s.radius=in.sphere.radius;
    // PORT STAND-IN (flagged): the MC native action owner implements one
    // action per trigger (every stock MC trigger authors one); more refuse.
    need(in.actionCount==1,"MC trigger with more than one action is not supported natively");
    s.actionCount=in.actionCount;const auto& ia=in.actions[0];s.actionOriginal=ia.original;
    s.authoredActionOneTimeOnly=ia.authoredOneTimeOnly;
    if(ia.readerKind==sc_trigger_authoring50LA::ActionKind::LostAI){
     s.action=Action::LostAI;s.targetNodeIndex=ia.targetNodeIndex;s.authoredReset=ia.authoredReset;
     s.effectiveReset1B5A60=ia.authoredReset.value_or(false); // 1B5A60 default 0
    }else if(ia.readerKind==sc_trigger_authoring50LA::ActionKind::Animation){
     s.action=Action::Animation;s.animationIndex=ia.animationIndex;
    }else need(false,"MC trigger action kind outside the MC native chain");
   }
   source_=std::move(next);live_=live;cold_=std::move(cold);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::string& error){return decode(std::move(cold),maximumAdditionalPayloadBytes(),error);}
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 std::size_t liveCount()const noexcept{return ready()?live_:0;}
 const Source& source(std::size_t i)const{
  if(!ready()||i>=live_)throw std::runtime_error("MC trigger source outside the live prefix");return source_[i];
 }
 bool authors(Action kind)const noexcept{
  for(std::size_t i=0;i<liveCount();++i)if(source_[i].action==kind)return true;return false;
 }
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
};
}

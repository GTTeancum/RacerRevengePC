#pragma once
#include "gb_cold_sources50la.h"
#include "ga_trigger_authoring_fixture.h"
#include "sc_trigger_authoring50la.h"
#include "../../src/game/native_trigger_group50gb.h"
#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <thread>

namespace gb_trigger_authoring50GB {
namespace a=rr::assets;namespace g=rr::game;namespace detail=ga_trigger_authoring::detail;
namespace common=sc_trigger_authoring50LA;
// Capacity store (common course rule): the common trigger reader's capacity
// and action slots, defined once in src\game\native_trigger_capacity50tg.h;
// the GB native group is sized to the same store. The live count is the TOB's
// own NumberOfTriggers (absent [Triggers] or 0 = no triggers).
inline constexpr std::size_t capacity=g::nativeTriggerCapacity50TG,maxActions=g::nativeTriggerActionSlots50TG;
static_assert(capacity==common::count&&maxActions==common::maxActions&&
    capacity==g::nativeTriggerCapacity50GB&&maxActions==g::nativeTriggerActionCapacity50GB,
    "GB native trigger group must hold the common trigger reader's store");
// Stock GB trigger set (18 triggers: 15 LostAI branches, door 16, gate 17,
// fireworks 18). Receipts only, EXCEPT as the trigger indices of GB's
// stock-shaped Animation / PodSound / Particles destination owners, which
// exist only for the stock TOB (PORT STAND-IN, flagged below).
inline constexpr std::size_t stockCount=18,lostCount=15,door=15,secondAnimation=16,particles=17;
inline constexpr std::size_t tobBytes=20617;
inline constexpr const char* tobSha="96469b3953b18d940bd0c8772758c68a75a69c3375b49ef98b966901c2e598d1";
enum class Action {LostAI,Animation,PodSound,Particles};
enum class Ignore {None,Player,AI};
enum class Status {Empty,Input,Capacity,Source,Authoring,Ready};
struct Diagnostic {
 Status status=Status::Empty;std::string message;
 std::size_t ordinal=0,retainedPayloadBytes=0,peakReservedPayloadBytes=0;
};
struct ActionSource {
 Action kind=Action::LostAI;std::uint32_t targetNodeIndex=0,animationIndex=0;
 std::optional<std::uint32_t> authoredTargetNodeIndex;std::optional<bool> authoredReset;
 bool effectiveReset1B5A60=false;
 std::string filename;g::Vector3_48C soundPosition{};float minDistance=0,maxDistance=0;
 std::uint32_t emitterNumber=0,instanceNumber=0; // Authored one-based fields.
 std::size_t emitterIndex=0,instanceIndex=0;bool turnOn=true;
 std::optional<bool> authoredOneTimeOnly; // Retained; source reader does not use it.
 const a::ParameterBlock48F* original=nullptr;
};
struct Source {
 std::string name;std::uint32_t sourceOrdinal=0,zone=0,actionCount=0,shapeType=5;
 g::CollisionSphere48J sphere{};bool startEnabled=true,oneTimeOnly=false;
 Ignore ignore=Ignore::None;std::int32_t randomTriggerPercent=100;
 std::optional<bool> authoredStartEnabled;std::optional<float> authoredRandomTriggerPercent;
 std::optional<std::string> authoredIgnoreTrigger;
 std::array<ActionSource,maxActions> actions{};const a::ParameterBlock48F* original=nullptr;
};
// Immutable source projection of the course TOB's own trigger set, read by the
// common trigger reader. No callback, collider/query identity, live animation,
// emission state or C80 authority is manufactured here.
class Owner final {
 std::shared_ptr<const gb_cold_sources50LA::Owner> cold_;std::array<Source,capacity> source_{};
 std::size_t live_=0;bool stockTob_=false,animationActions_=false,podSoundActions_=false,particleActions_=false;
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
 static void add(std::size_t& n,std::size_t x)noexcept {
  n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;
 }
 bool fail(Diagnostic& d,Status status,const char* message)noexcept {
  d.status=status;try{d.message=message;}catch(...){}d.retainedPayloadBytes=descriptorPayloadBytes();return false;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  // Both staged/retained string sets, actual short labels, SHA copy/padding
  // scratch and diagnostics. Shared original source remains separately owned.
  // Sized by the store capacity; the TOB scratch term keeps the stock-TOB
  // estimate (decode scratch belongs to the cold source's own reservation).
  return sizeof(Owner)+sizeof(std::array<Source,capacity>)+2u*128u*64u+capacity*64u+1024u+4u*(tobBytes+64u)+
   sizeof(common::Owner);
 }
 bool decode(std::shared_ptr<const gb_cold_sources50LA::Owner> cold,
     std::size_t availableAdditionalCpuPayloadBytes,Diagnostic& d){
  d={};
  if(thread_!=std::this_thread::get_id())return fail(d,Status::Input,"GB trigger source thread mismatch");
  if(attempted_)return fail(d,Status::Input,"GB trigger source requires a fresh owner");attempted_=true;
  if(!cold||!cold->ready())return fail(d,Status::Input,"GB trigger source requires actual cold owner");
  d.peakReservedPayloadBytes=maximumAdditionalPayloadBytes();
  if(availableAdditionalCpuPayloadBytes>32u*1024u*1024u||
     d.peakReservedPayloadBytes>availableAdditionalCpuPayloadBytes)
   return fail(d,Status::Capacity,"GB trigger preparation exceeds remaining source allowance");
  try{using namespace detail;
   need(cold&&cold->ready(),"GB trigger source requires actual cold owner");
   // The common trigger reader reads the course's own TOB (one log line
   // "course_triggers site=GB count=N kinds=..."; stock: 18 triggers).
   auto reader=std::make_unique<common::Owner>();common::Owner::Diagnostic read{};
   const auto& p=cold->triggerParameters();
   need(reader->decode(p,cold->world().zones.size(),cold->ai().count(),"GB",read)&&reader->sources(),
    "GB course archive TOB rejected by the common trigger reader (see course_triggers line)");
   const bool stockTob=cold->originalTob().size()==tobBytes&&pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tobSha;
   if(!stockTob)std::cerr<<"course_member_identity site=gb_triggers member=tracks\\gb.tob bytes="<<cold->originalTob().size()<<" stock=0\n";
   const auto live=reader->liveCount();
   need(live<=capacity,"GB live trigger set exceeds the native trigger store");
   std::array<Source,capacity> next{};bool animation=false,podSound=false,particle=false;
   for(std::size_t i=0;i<live;++i){d.ordinal=i+1;const auto& in=(*reader->sources())[i];auto& s=next[i];
    s.original=in.originalTrigger;s.sourceOrdinal=static_cast<std::uint32_t>(i+1);s.zone=in.sourceZoneOrdinal;
    s.name=in.name;s.actionCount=in.actionCount;s.oneTimeOnly=in.oneTimeOnly;s.startEnabled=in.startEnabled;
    if(in.startEnabledAuthored)s.authoredStartEnabled=in.startEnabled;
    if(in.randomTriggerPercentAuthored)s.authoredRandomTriggerPercent=in.randomTriggerPercent;
    // GB native group: sphere triggers at the authored 100 percent only (the
    // source dispatcher draws no RNG). FLAG stand-in: a box trigger or another
    // percent is refused here until the GB group joins the common geometry/RNG.
    need(in.shape==common::Shape::sphere&&in.radius&&std::isfinite(*in.radius)&&*in.radius>0.f,
     "GB native trigger group implements sphere triggers only (box trigger refused)");
    need(in.randomTriggerPercent==100.f,"GB native trigger group implements RandomTriggerPercent 100 only");
    s.randomTriggerPercent=100;s.shapeType=5;
    for(unsigned k=0;k<3;++k)s.sphere.center.v[k]=in.position.values[k];s.sphere.radius=*in.radius;
    if(in.ignoreTrigger){s.authoredIgnoreTrigger=*in.ignoreTrigger;
     if(*in.ignoreTrigger=="PLAYER")s.ignore=Ignore::Player;
     else{need(*in.ignoreTrigger=="AI","GB IgnoreTrigger other than PLAYER/AI");s.ignore=Ignore::AI;}}
    need(s.actionCount>=1&&s.actionCount<=maxActions,"GB trigger action count outside the native action slots");
    for(std::size_t j=0;j<s.actionCount;++j){const auto& x=in.actions[j];auto& action=s.actions[j];
     action.original=x.original;action.authoredOneTimeOnly=x.authoredOneTimeOnly;
     switch(x.kind){
     case common::ActionKind::LostAI:
      // Every LostAI Reset is absent/false in the native action (1B5A60 default 0).
      need(!x.authoredReset||!*x.authoredReset,"GB LostAI Reset is not supported natively");
      action.kind=Action::LostAI;action.authoredTargetNodeIndex=x.targetAiNodeIndex;action.targetNodeIndex=x.targetAiNodeIndex;
      action.authoredReset=x.authoredReset;
      need(action.targetNodeIndex<cold->ai().count(),"GB LostAI target outside the AI spline");break;
     case common::ActionKind::Animation:
      action.kind=Action::Animation;action.animationIndex=x.animationNumber-1u;animation=true;break;
     case common::ActionKind::PodSound:
      action.kind=Action::PodSound;action.filename=x.filename;
      for(unsigned k=0;k<3;++k)action.soundPosition.v[k]=x.position.values[k];
      action.minDistance=x.minDistance;action.maxDistance=x.maxDistance;podSound=true;break;
     case common::ActionKind::Particles:
      action.kind=Action::Particles;action.emitterNumber=x.emitterNumber;action.instanceNumber=x.instanceNumber;
      action.turnOn=x.turnOn;action.emitterIndex=x.emitterNumber-1u;action.instanceIndex=x.instanceNumber-1u;particle=true;break;
     default:
      // FLAG stand-in: GB has no ordinary trigger Sound destination owner yet.
      need(false,"GB trigger Sound action has no native GB destination owner");
     }
    }
   }
   // Animation actions go to the common track-animation player (any row the TOB
   // loads). PORT STAND-IN (flagged): GB's PodSound and Particles destination
   // owners (gb_pod_sound50gb.h, gb_particle_*50gb.h) implement the stock GB set
   // only; a non-stock TOB whose triggers author those kinds is refused here
   // until GB joins the common emitter / trigger-sound owners. LostAI and
   // Animation triggers and an empty trigger set load from any TOB.
   need(stockTob||(!podSound&&!particle),
    "GB TOB triggers author PodSound/Particles actions: GB's destination owners read the stock set only (PORT STAND-IN until the common owners)");
   if(stockTob){
    // Stock receipt: the shipped 18-trigger shape (zones, LostAI targets, the
    // door/gate/fireworks kinds). Logged only when it differs.
    constexpr std::uint32_t targets[lostCount]={189,240,30,253,40,196,46,247,49,85,206,111,228,234,151};
    constexpr std::uint32_t zones[stockCount]={4,4,4,4,4,4,4,4,4,6,6,9,9,12,12,2,2,1};
    bool same=live==stockCount;
    for(std::size_t i=0;same&&i<live;++i){const auto& s=next[i];
     same=s.zone==zones[i]&&s.actionCount==(i==door?2u:i==particles?5u:1u)&&
      (i>=lostCount||s.actions[0].targetNodeIndex==targets[i]);}
    if(!same)std::cerr<<"course_count site=gb_triggers stock_shape=0\n";
   }
   source_=std::move(next);live_=live;stockTob_=stockTob;
   animationActions_=animation;podSoundActions_=podSound;particleActions_=particle;cold_=std::move(cold);
   need(descriptorPayloadBytes()<=maximumAdditionalPayloadBytes(),"GB descriptor reservation exceeded");
   std::cerr<<"gb_course_owner owner=triggers present="<<(live_?1:0)<<" count="<<live_<<" animation_actions="<<(animation?1:0)
       <<" podsound_actions="<<(podSound?1:0)<<" particle_actions="<<(particle?1:0)<<" stock="<<(stockTob?1:0)<<'\n';
   ready_=true;d.status=Status::Ready;d.message.clear();d.retainedPayloadBytes=descriptorPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,Status::Authoring,e.what());}
   catch(...){return fail(d,Status::Authoring,"GB trigger authoring failed");}
 }
 // Compatibility for aggregates that already reserve maximumAdditionalPayloadBytes()
 // before calling. This overload does not account for other owners on their behalf.
 bool decode(std::shared_ptr<const gb_cold_sources50LA::Owner> cold,std::string& error){
  Diagnostic d;const bool okay=decode(std::move(cold),maximumAdditionalPayloadBytes(),d);error=d.message;return okay;
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 // Live prefix [0,liveCount()) of the capacity store.
 std::size_t liveCount()const noexcept{return ready()?live_:0;}
 bool stockTob()const noexcept{return ready()&&stockTob_;}
 // Which destination kinds the authored triggers use (each needs its owner).
 bool authorsAnimationActions()const noexcept{return ready()&&animationActions_;}
 bool authorsPodSound()const noexcept{return ready()&&podSoundActions_;}
 bool authorsParticleActions()const noexcept{return ready()&&particleActions_;}
 const Source& source(std::size_t i)const{if(!ready()||i>=live_)throw std::runtime_error("GB trigger source unavailable");return source_[i];}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{if(!ready())throw std::runtime_error("GB trigger source unavailable");return cold_->originalTob();}
 std::size_t descriptorPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;
  std::size_t n=sizeof(Owner);for(const auto& s:source_){add(n,s.name.capacity());add(n,1);
   if(s.authoredIgnoreTrigger){add(n,s.authoredIgnoreTrigger->capacity());add(n,1);}
   for(const auto& a:s.actions){add(n,a.filename.capacity());add(n,1);}}return n;
 }
};
}

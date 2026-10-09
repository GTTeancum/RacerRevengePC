#pragma once
#include "gc_trigger_authoring50in_fixture.h"
#include "gc_animated_world50ig_fixture.h"
#include "gc_host_route50in_fixture.h"
#include "../../src/game/trigger_entry50in.h"
#include "../../src/game/trigger_actions50in.h"
#include "../../src/game/trigger_sphere50in.h"
#include "../../src/game/type2_resource49x.h"
#include <array>
#include <limits>

// EXPLICIT HOST PLAYABILITY BACKEND. It deliberately uses each actual main /
// secondary COL type2 ROOT BOX against the original TOB trigger sphere for
// entry as well as exit. Original108E90 entry-leaf parity remains unresolved.
// No native collector, query result, spring callback, body impulse or collision
// material is changed. The host calls this after accepted racer frames in its
// current participant order; nine triggers are visited in authored order.
// This global scan deliberately does not claim original zone/category/collector
// filtering. Prepared geometry is supplied by the actual accepted query cache.
namespace gc_host_triggers50IN {
namespace g=rr::game;
inline constexpr std::size_t maxRacers=8,maxColliders=maxRacers*2;
inline constexpr std::uint32_t firstRacerIdentity=0x30000000u,firstTriggerIdentity=0x31000000u;
struct Geometry {
 const g::Type2ResourceView49X* resource=nullptr;
 g::BodyMatrix48G preparedWorld{};
 bool enabled=false;
};
struct SoundHooks {
 void* context=nullptr;
 // Immutable prepared source clip, reserved voice, live mixer and group owner.
 bool (*ready)(void*) noexcept=nullptr;
 void (*start)(void*) noexcept=nullptr;
 bool (*failed)(void*) noexcept=nullptr;
 bool enabled=false;
};
struct Counters {
 std::uint64_t acceptedRacerObservations=0,rootBoxOverlaps=0,entryCallbacks=0,
     exitCallbacks=0,lostAiWrites=0,gateActivations=0,soundRequests=0,soundCulled=0,
     soundDisabled=0,worldServices=0,routeProjections=0,routeChangedInputs=0;
 std::array<std::uint64_t,gc_trigger_authoring50IN::lostCount> lostAiByBranch{};
};
enum class Status {Complete,Unprepared,Input,Geometry,Entry,Action,Service,Audio,Route};
class Owner final {
 const gc_trigger_authoring50IN::Owner* authoring_=nullptr;
 gc_animated_world50IG::Owner* animation_=nullptr;
 SoundHooks sound_{};
 std::array<g::TriggerEntryState50IN,gc_trigger_authoring50IN::count> trigger_{};
 std::array<g::TriggerColliderContacts50IN,maxColliders> collider_{};
 std::array<std::array<bool,maxColliders>,gc_trigger_authoring50IN::count> overlap_{};
 std::array<std::uint32_t,maxRacers> routeTarget_{};
 std::array<bool,maxRacers> routePending_{};
 std::size_t racerCount_=0,currentRacer_=maxRacers;
 std::uint32_t* currentC80_=nullptr;
 const g::Vector3_48C* listeners_=nullptr;std::size_t listenerCount_=0;
 std::uint32_t aiNodeCount_=0;
 bool ready_=false,actionFailed_=false;
 Status terminal_=Status::Complete;
 Status fail(Status status)noexcept{if(terminal_==Status::Complete)terminal_=status;return terminal_;}
 Counters counters_{};
 static constexpr std::size_t triggerIndex(std::uint32_t identity)noexcept {
  return identity>=firstTriggerIdentity?identity-firstTriggerIdentity:gc_trigger_authoring50IN::count;
 }
 static constexpr std::size_t colliderIndex(std::uint32_t identity)noexcept {
  return identity>=firstRacerIdentity?identity-firstRacerIdentity:maxColliders;
 }
 bool preflight(const g::TriggerActionCall50IN& call)const noexcept {
  const auto i=triggerIndex(call.triggerCollider);
  if(i>=gc_trigger_authoring50IN::count||!authoring_||!animation_||
     call.actionIndex>=authoring_->source(i).actionCount)return false;
  if(!call.entering)return true; // All three original callbacks ignore exits.
  const auto c=colliderIndex(call.otherCollider);
  if(c>=racerCount_*2||currentRacer_>=racerCount_||c/2!=currentRacer_)return false;
  const auto& action=authoring_->source(i).actions[call.actionIndex];
  switch(action.kind){
   case gc_trigger_authoring50IN::Action::LostAI:
    return !action.reset&&action.targetNodeIndex<aiNodeCount_&&((c&1u)||currentC80_);
   case gc_trigger_authoring50IN::Action::Animation:
    return animation_->ready()&&action.animationIndex<gc_animated_world50IG::ownerCount;
   case gc_trigger_authoring50IN::Action::PodSound:{
    if(!sound_.enabled)return true;
    const auto decision=g::triggerPodSoundAdmission50IN(true,{action.position,action.minDistance,action.maxDistance},listeners_,listenerCount_);
    return decision.status==g::TriggerActionStatus50IN::Complete&&
        (!decision.play||(sound_.ready&&sound_.start&&sound_.failed&&sound_.ready(sound_.context)&&!sound_.failed(sound_.context)));
   }
  }return false;
 }
 void apply(const g::TriggerActionCall50IN& call)noexcept {
  if(!call.entering){++counters_.exitCallbacks;return;}
  ++counters_.entryCallbacks;
  const auto i=triggerIndex(call.triggerCollider);const auto c=colliderIndex(call.otherCollider);
  const auto& action=authoring_->source(i).actions[call.actionIndex];
  switch(action.kind){
   case gc_trigger_authoring50IN::Action::LostAI:{
    // Only main690 matches1B27C0; a secondary entry still owns its contact
    // bookkeeping but cannot rewrite a racer's target or cause a reset.
    if(!(c&1u)){
     const g::LostAiRacer50IN racer{firstRacerIdentity+static_cast<std::uint32_t>(currentRacer_*2),currentC80_};
     const auto changed=g::triggerLostAi50IN(true,call.otherCollider,{action.targetNodeIndex,false},&racer,1,aiNodeCount_);
     if(changed.status!=g::TriggerActionStatus50IN::Complete)actionFailed_=true;
     counters_.lostAiWrites+=changed.changedRacers;
     if(i<counters_.lostAiByBranch.size())counters_.lostAiByBranch[i]+=changed.changedRacers;
     if(changed.changedRacers){routeTarget_[currentRacer_]=action.targetNodeIndex;routePending_[currentRacer_]=true;}
    }break;
   }
   case gc_trigger_authoring50IN::Action::Animation:
    if(!animation_->activateSequence(action.animationIndex))actionFailed_=true;
    else ++counters_.gateActivations;
    break;
   case gc_trigger_authoring50IN::Action::PodSound:{
    if(!sound_.enabled){++counters_.soundDisabled;break;}
    const auto decision=g::triggerPodSoundAdmission50IN(true,{action.position,action.minDistance,action.maxDistance},listeners_,listenerCount_);
    if(decision.status!=g::TriggerActionStatus50IN::Complete){actionFailed_=true;break;}
    if(!decision.play){++counters_.soundCulled;break;}
    sound_.start(sound_.context);
    if(sound_.failed(sound_.context))actionFailed_=true;else ++counters_.soundRequests;
    break;
   }
  }
 }
 g::TriggerActionHooks50IN hooks()noexcept {
  return {this,[](void* p,const g::TriggerActionCall50IN& c)noexcept{return static_cast<Owner*>(p)->preflight(c);},
      [](void* p,const g::TriggerActionCall50IN& c)noexcept{static_cast<Owner*>(p)->apply(c);}};
 }
 static bool admittedGeometry(const Geometry& geometry)noexcept {
  if(!geometry.enabled)return true;
  const auto* r=geometry.resource;
  return r&&r->nodes&&r->nodeCount&&!(r->root&g::type2Leaf49X)&&r->root<r->nodeCount;
 }
 bool rootOverlap(std::size_t trigger,const Geometry& geometry,bool& hit)const noexcept {
  hit=false;if(!geometry.enabled)return true;
  const auto& source=authoring_->source(trigger);const auto& root=geometry.resource->nodes[geometry.resource->root];
  g::TriggerSphereRootExitInput50IN in{};in.type2PreparedWorld=geometry.preparedWorld;
  for(unsigned axis=0;axis<3;++axis){in.sphere.localCenter.v[axis]=source.sphere.center.v[axis];in.rootCenter.v[axis]=root.center[axis];in.rootHalf.v[axis]=root.half[axis];}
  in.sphere.localCenter.v[3]=in.rootCenter.v[3]=in.rootHalf.v[3]=1.f;
  for(unsigned axis=0;axis<4;++axis)in.sphere.world.v[axis*5]=1.f;
  in.sphere.radius50=source.sphere.radius;
  volatile float squared=source.sphere.radius*source.sphere.radius;in.sphere.radiusSquared54=squared;
  // Source exit predicate is reused as the explicitly HOST entry proxy. It
  // tests actual rotated COL bounds, not pod center or a tuned distance.
  return g::overlapTriggerSphereType2Root50IN(in,hit)==g::TriggerSphereRootExitStatus50IN::Complete;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 bool prepare(const gc_trigger_authoring50IN::Owner& authoring,gc_animated_world50IG::Owner& animation,
              std::size_t racers,std::uint32_t aiNodeCount,const SoundHooks& sound)noexcept {
  if(ready_||!authoring.ready()||!animation.ready()||!racers||racers>maxRacers||!aiNodeCount||
     (sound.enabled&&(!sound.ready||!sound.start||!sound.failed||!sound.ready(sound.context)||sound.failed(sound.context))))return false;
  for(std::size_t i=0;i<gc_trigger_authoring50IN::count;++i){const auto& s=authoring.source(i);
   if(!s.startEnabled||s.randomTriggerPercent!=100||s.actionCount>g::triggerActionCapacity50IN)return false;
  }
  authoring_=&authoring;animation_=&animation;racerCount_=racers;aiNodeCount_=aiNodeCount;sound_=sound;
  for(std::size_t i=0;i<racerCount_*2;++i)collider_[i].identity=firstRacerIdentity+static_cast<std::uint32_t>(i);
  for(std::size_t i=0;i<gc_trigger_authoring50IN::count;++i){const auto& s=authoring.source(i);auto& t=trigger_[i];
   t.flags29=s.startEnabled?3:0;t.flags1B8=static_cast<std::uint8_t>(3u|(s.oneTimeOnly?4u:0u));
   t.randomTriggerPercent90=s.randomTriggerPercent;t.callbackCount98=s.actionCount;
   t.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
  }
  ready_=true;return true;
 }
 // Called within the correct racer-bank visit so C80 belongs to that racer.
 // No borrowed arena pointers survive this call. Listener positions are the
 // explicitly selected HOST listener owner, never original camera parity.
 // Every error is terminal: earlier accepted actions are not rolled back, and
 // a one-shot Gate must never be retried after partial downstream failure.
 Status observeAcceptedRacer(std::size_t racer,const Geometry& main,const Geometry& secondary,
      std::uint32_t& nodeC80,const g::Vector3_48C* listeners,std::size_t listenerCount)noexcept {
  if(terminal_!=Status::Complete)return terminal_;
  if(!ready_)return fail(Status::Unprepared);
  if(racer>=racerCount_||!main.enabled||!admittedGeometry(main)||!admittedGeometry(secondary)||
     listenerCount>2||(listenerCount&&!listeners)||currentRacer_!=maxRacers)return fail(Status::Input);
  std::array<std::array<bool,gc_trigger_authoring50IN::count>,2> next{};
  const Geometry* geometry[]={&main,&secondary};
  for(unsigned body=0;body<2;++body)for(std::size_t i=0;i<gc_trigger_authoring50IN::count;++i)
   if(!rootOverlap(i,*geometry[body],next[body][i]))return fail(Status::Geometry);
  currentRacer_=racer;currentC80_=&nodeC80;listeners_=listeners;listenerCount_=listenerCount;
  auto finish=[&](Status status)noexcept{currentRacer_=maxRacers;currentC80_=nullptr;listeners_=nullptr;listenerCount_=0;return status;};
  const auto actionHooks=hooks();
  for(unsigned body=0;body<2;++body){const auto c=racer*2+body;
   for(std::size_t i=0;i<gc_trigger_authoring50IN::count;++i){
    overlap_[i][c]=next[body][i];if(!next[body][i])continue;++counters_.rootBoxOverlaps;
    const auto& source=authoring_->source(i);
    // Source PLAYER binding excludes the main690 collider only, mutually.
    if(source.ignorePlayer&&racer==0&&body==0)continue;
    auto& state=trigger_[i];
    // Source C8/CC identity-mode filter suppresses the callback, including its
    // touched bit, while reciprocal membership exists. Do not synthesize a
    // repeated enter every sample or consume random values for percent100.
    if(g::trigger_entry50IN_detail::find(state.collider,collider_[c].identity)<state.collider.highWaterD0)continue;
    const auto result=g::triggerContact50IN(state,collider_[c],actionHooks);
    if(result.status!=g::TriggerEntryStatus50IN::Complete)return finish(fail(Status::Entry));
    if(result.randomConsumed)return finish(fail(Status::Action));
    if(actionFailed_)return finish(fail(sound_.enabled&&sound_.failed(sound_.context)?Status::Audio:Status::Action));
   }
  }
  ++counters_.acceptedRacerObservations;return finish(Status::Complete);
 }
 // Once after the shared animation service. Each stored collider keeps its
 // last accepted geometry if its host racer is stopped. No menu/focus-paused
 // sample is admitted, and countdown services have no fabricated racer entry.
 Status serviceWorld()noexcept {
  if(terminal_!=Status::Complete)return terminal_;
  if(!ready_)return fail(Status::Unprepared);
  if(currentRacer_!=maxRacers)return fail(Status::Input);
  const auto actionHooks=hooks();
  for(std::size_t i=0;i<trigger_.size();++i){auto& state=trigger_[i];
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t j=0;j<state.collider.highWaterD0;++j){const auto id=state.collider.membersCC[j];if(!id)continue;
    const auto c=colliderIndex(id);if(c>=racerCount_*2)return fail(Status::Input);
    observations[j]={&collider_[c],overlap_[i][c]};
   }
   const auto result=g::serviceTrigger50IN(state,observations.data(),state.collider.highWaterD0,actionHooks);
   if(result.status!=g::TriggerEntryStatus50IN::Complete)return fail(Status::Service);
  }
  ++counters_.worldServices;return Status::Complete;
 }
 // HOST steering compatibility only, armed by an accepted LostAI write.
 // Canonical source C84 remains untouched even if its old edge does not lead
 // to the new C80. A later source C80 change retires this projection.
 bool projectCoursePrevious(std::size_t racer,const g::TrackNavigation50EU& route,
      std::uint32_t current,std::uint32_t previous,const g::BodyMatrix48G& pose,
      std::uint32_t& output)noexcept {
  output=previous;
  if(terminal_!=Status::Complete)return false;
  if(!ready_||racer>=racerCount_){fail(Status::Input);return false;}
  if(!routePending_[racer])return true;
  if(current!=routeTarget_[racer]){routePending_[racer]=false;return true;}
  if(!gc_host_route50IN::selectPrevious(route,current,previous,pose,output)){
   fail(Status::Route);return false;
  }
  ++counters_.routeProjections;if(output!=previous)++counters_.routeChangedInputs;
  return true;
 }
 Status status()const noexcept{return terminal_;}
 const Counters& counters()const noexcept{return counters_;}
 const g::TriggerEntryState50IN& state(std::size_t i)const{return trigger_.at(i);}
 bool ready()const noexcept{return ready_;}
};
} // namespace gc_host_triggers50IN

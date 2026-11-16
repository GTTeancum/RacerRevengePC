#pragma once
#include "ga_trigger_authoring_fixture.h"
#include "course_trigger_sources50ct.h"
#include "../../src/game/native_trigger_box_geometry50ma.h"
#include "ga_animated_world_fixture.h"
#include "gc_gate_audio50in_fixture.h"
#include "../../src/game/trigger_entry50in.h"
#include "../../src/game/native_trigger_group50ga.h"
#include "../../src/game/trigger_sphere50in.h"
#include "../../src/game/type2_resource49x.h"
#include "../../src/game/trigger_actions50in.h"
#include <array>

// Source action/lifecycle owner only. This file DOES NOT supply the missing
// original type2 -> sphere5 geometric entry backend. The collector may bind
// the separately disclosed unchanged native Gate geometry policy. It never
// calls gc_host_triggers, a root-box entry proxy, or a
// route driver. Only an already-filtered geometric hit may dispatch actions;
// source122BA0 coarse overlap is used solely for lifecycle exit observations.
namespace ga_trigger_actions {
namespace g=rr::game;
inline constexpr std::size_t racers=8,colliders=racers*2;
inline constexpr std::uint32_t firstTriggerIdentity=0x32000000u;
struct RacerIdentities {std::uint32_t main690=0,secondary=0;};
struct ActionLease {
 // Valid only during this synchronous source collider callback. The serial
 // racer bank restores the same live C80 address for each actor; it must never
 // be retained across arena visits or replaced with route-driver state.
 std::uint32_t racerMain690=0;std::uint32_t* nodeC80=nullptr;
 const g::Vector3_48C* listenerPositions=nullptr;std::size_t listenerCount=0;
};
struct Counters {
 std::uint64_t entries=0,exits=0,lostAiWrites=0,animationActivations=0,
   soundStarts=0,soundCulled=0,soundMuted=0;
};
enum class Status {Complete,Unprepared,Input,IgnoredPlayer,Entry,Action,Service};
class Owner final {
 const course_trigger_sources50CT::Owner* source_=nullptr;
 ga_animated_world::Owner* animation_=nullptr;
 gc_gate_audio50IN::Owner* sound_=nullptr;
 std::array<RacerIdentities,racers> racers_{};
 std::size_t racerCount_=0;
 g::NativeTriggerGroup50GA group_{};
 struct Geometry {const g::Type2ResourceView49X* resource=nullptr;g::BodyMatrix48G world{};bool valid=false;};
 std::array<Geometry,colliders> geometry_{};
 const ActionLease* lease_=nullptr;
 std::uint32_t originalAiNodeCount_=0,playerPrefixCount34_=0;
 bool ready_=false,muted_=false,actionFailed_=false;
 Counters counters_{};Status terminal_=Status::Complete;
 std::size_t colliderIndex(std::uint32_t id)const noexcept {
  for(std::size_t i=0;i<group_.colliders.size();++i)if(group_.colliders[i].identity==id)return i;
  return group_.colliders.size();
 }
 bool preflight(const g::TriggerActionCall50IN& call)const noexcept {
  if(!ready_||call.triggerCollider<firstTriggerIdentity)return false;
  const auto t=call.triggerCollider-firstTriggerIdentity;
  if(t>=group_.triggers.size()||call.actionIndex>=source_->source(t).actionCount)return false;
  if(!call.entering)return true; // All authored GA callbacks ignore exits.
  const auto c=colliderIndex(call.otherCollider);
  if(c==group_.colliders.size()||!lease_||lease_->racerMain690!=racers_[c/2].main690)return false;
  const auto& action=source_->source(t).actions[call.actionIndex];
  switch(action.kind){
   case ga_trigger_authoring::Action::LostAI:
    return !action.reset&&action.targetNodeIndex<originalAiNodeCount_&&((c&1u)||lease_->nodeC80);
   case ga_trigger_authoring::Action::Animation:
    return animation_&&animation_->ready()&&action.animationIndex<ga_animated_world::ownerCount;
   case ga_trigger_authoring::Action::PodSound:{
    const auto decision=g::triggerPodSoundAdmission50IN(true,
      {action.position,action.minDistance,action.maxDistance},lease_->listenerPositions,lease_->listenerCount);
    return decision.status==g::TriggerActionStatus50IN::Complete&&
      (!decision.play||muted_||(sound_&&sound_->available()&&sound_->ready()));
   }
  }
  return false;
 }
 void apply(const g::TriggerActionCall50IN& call)noexcept {
  if(!call.entering){++counters_.exits;return;}
  ++counters_.entries;
  const auto t=call.triggerCollider-firstTriggerIdentity;
  const auto& action=source_->source(t).actions[call.actionIndex];
  switch(action.kind){
   case ga_trigger_authoring::Action::LostAI:{
    // Preserve original race traversal and main690 matching. The eight actual
    // identities include every roster slot; IgnoreTrigger PLAYER uses the
    // source manager+34 prefix, never the camera slot or AI class58.
    std::array<g::LostAiRacer50IN,racers> destinations{};
    for(std::size_t i=0;i<racerCount_;++i)destinations[i]={racers_[i].main690,
      racers_[i].main690==lease_->racerMain690?lease_->nodeC80:nullptr};
    const auto result=g::triggerLostAi50IN(true,call.otherCollider,
      {action.targetNodeIndex,action.reset},destinations.data(),racerCount_,originalAiNodeCount_);
    actionFailed_=actionFailed_||result.status!=g::TriggerActionStatus50IN::Complete;group_.actionsFailed=actionFailed_;
    counters_.lostAiWrites+=result.changedRacers;break;
   }
   case ga_trigger_authoring::Action::Animation:
    if(animation_&&animation_->activateSequence(action.animationIndex))++counters_.animationActivations;
    else actionFailed_=group_.actionsFailed=true;
    break;
   case ga_trigger_authoring::Action::PodSound:{
    const auto decision=g::triggerPodSoundAdmission50IN(true,
      {action.position,action.minDistance,action.maxDistance},lease_->listenerPositions,lease_->listenerCount);
    if(decision.status!=g::TriggerActionStatus50IN::Complete){actionFailed_=group_.actionsFailed=true;break;}
    if(!decision.play){++counters_.soundCulled;break;}
    if(muted_){++counters_.soundMuted;break;}
    sound_->start();
    if(sound_->failed())actionFailed_=group_.actionsFailed=true;else ++counters_.soundStarts;
    break;
   }
  }
 }
 g::TriggerActionHooks50IN hooks()noexcept {
  return {this,[](void* p,const g::TriggerActionCall50IN& c)noexcept{return static_cast<Owner*>(p)->preflight(c);},
   [](void* p,const g::TriggerActionCall50IN& c)noexcept{static_cast<Owner*>(p)->apply(c);}};
 }
 Status fail(Status s)noexcept {if(terminal_==Status::Complete)terminal_=s;return terminal_;}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 // Sources come from the course's own TOB (course_trigger_sources50ct.h).
 // animation may be null exactly when the data authors no Animation action;
 // sound is required exactly when it authors a PodSound action.
 bool prepare(const course_trigger_sources50CT::Owner& source,ga_animated_world::Owner* animation,
     const std::array<RacerIdentities,racers>& identities,std::uint32_t aiNodeCount,
     gc_gate_audio50IN::Owner* sound,bool explicitlyMuted,
     std::uint32_t playerPrefixCount34=1u,std::size_t racerCount=racers)noexcept {
  // The trailing default retains the old GA HOST one-player roster policy
  // for old callers only. Source-connected sessions MUST pass real manager+34;
  // neither camera slot0 nor arena class58 determines the PLAYER prefix.
  const auto live=source.liveCount();
  const bool animated=source.authors(ga_trigger_authoring::Action::Animation);
  const bool sounded=source.authors(ga_trigger_authoring::Action::PodSound);
  if(ready_||!source.ready()||!live||(animated&&(!animation||!animation->ready()))||!aiNodeCount||!racerCount||racerCount>racers||playerPrefixCount34>racerCount||
     (sounded&&!explicitlyMuted&&(!sound||!sound->available()||!sound->ready())))return false;
  if(!group_.triggers.setCount(live))return false;
  std::array<std::uint32_t,colliders> ids{};
  for(std::size_t i=0;i<racerCount;++i){ids[i*2]=identities[i].main690;ids[i*2+1]=identities[i].secondary;}
  for(std::size_t i=0;i<racerCount*2;++i){
   if(!ids[i]||(ids[i]>=firstTriggerIdentity&&ids[i]<firstTriggerIdentity+group_.triggers.size()))return false;
   for(std::size_t j=0;j<i;++j)if(ids[i]==ids[j])return false;
  }
  for(std::size_t i=0;i<group_.triggers.size();++i){const auto& authored=source.source(i);
   if(authored.randomTriggerPercent!=100||!authored.actionCount||authored.actionCount>2)return false;
   // Every action target must exist in the loaded data: LostAI inside the live
   // AI spline, Animation inside the animated owners.
   for(std::uint32_t k=0;k<authored.actionCount;++k){const auto& action=authored.actions[k];
    if(action.kind==ga_trigger_authoring::Action::LostAI&&(action.reset||action.targetNodeIndex>=aiNodeCount))return false;
    if(action.kind==ga_trigger_authoring::Action::Animation&&action.animationIndex>=ga_animated_world::ownerCount)return false;
   }
  }
  source_=&source;animation_=animation;racers_=identities;racerCount_=racerCount;originalAiNodeCount_=aiNodeCount;playerPrefixCount34_=playerPrefixCount34;sound_=sound;muted_=explicitlyMuted;
  if(!group_.colliders.setCount(racerCount*2))return false;
  for(std::size_t i=0;i<racerCount*2;++i)group_.colliders[i].identity=ids[i];
  for(std::size_t i=0;i<group_.triggers.size();++i){const auto& authored=source.source(i);auto& state=group_.triggers[i];
   state.flags29=authored.startEnabled?3u:0u;
   state.flags1B8=static_cast<std::uint8_t>(3u|(authored.oneTimeOnly?4u:0u));
   state.randomTriggerPercent90=authored.randomTriggerPercent;
   state.callbackCount98=authored.actionCount;state.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
  }
  ready_=true;group_.actions=hooks();group_.ready=true;
  for(std::size_t i=0;i<group_.triggers.size();++i)if(source.source(i).ignorePlayer){
   group_.ignoreColliderCount[i]=playerPrefixCount34_;
   for(std::size_t j=0;j<playerPrefixCount34_;++j)group_.ignoreColliders[i][j]=identities[j].main690;
  }
  return g::validNativeTriggerGroup50GA(group_);
 }
 // This is NOT a geometric query entry point. Call only after the declared
 // backend and collection/category/identity/reciprocal filters admit a real
 // hit. Native endpoint geometry is explicitly distinct from retail108E90.
 // A disabled Trigger1 is still represented and remains disabled.
 Status dispatchAdmittedContact(std::size_t trigger,std::uint32_t collider,
     const ActionLease& lease,g::TriggerEntryReceipt50IN& receipt)noexcept {
  receipt={};if(terminal_!=Status::Complete)return terminal_;
  if(!ready_)return fail(Status::Unprepared);
  const auto c=colliderIndex(collider);
  if(trigger>=group_.triggers.size()||c==group_.colliders.size()||lease_||lease.racerMain690!=racers_[c/2].main690||
     lease.listenerCount>2||(lease.listenerCount&&!lease.listenerPositions))return fail(Status::Input);
  // Ignoring PLAYER must occur before 2A00A0/touched/fired/list writes. Return
  // a distinct outcome so callers cannot count a suppressed contact as action.
  if(source_->source(trigger).ignorePlayer)for(std::size_t i=0;i<playerPrefixCount34_;++i)
   if(collider==racers_[i].main690)return Status::IgnoredPlayer;
  lease_=&lease;receipt=g::triggerContact50IN(group_.triggers[trigger],group_.colliders[c],hooks());lease_=nullptr;
  if(receipt.status!=g::TriggerEntryStatus50IN::Complete||receipt.randomConsumed)return fail(Status::Entry);
  if(actionFailed_)return fail(Status::Action);
  return Status::Complete;
 }
 // Observations must come from original122BA0 at the world-service boundary,
 // preserving high-water holes and order. This adapter never creates them by
 // scanning racer centers, timers, waypoints, or stale HOST root overlaps.
 Status service(std::size_t trigger,const g::TriggerOverlap50IN* observations,
     std::size_t count,g::TriggerEntryReceipt50IN& receipt)noexcept {
  receipt={};if(terminal_!=Status::Complete)return terminal_;
  if(!ready_)return fail(Status::Unprepared);
  if(trigger>=group_.triggers.size()||lease_||count!=group_.triggers[trigger].collider.highWaterD0||
     count>g::triggerContactCapacity50IN||(count&&!observations))return fail(Status::Input);
  // Every nonnull observation must borrow this exact stable collider owner.
  for(std::size_t i=0;i<count;++i)if(observations&&observations[i].collider){
   bool owned=false;for(auto& c:group_.colliders)owned=owned||observations[i].collider==&c;
   if(!owned)return fail(Status::Input);
  }
  receipt=g::serviceTrigger50IN(group_.triggers[trigger],observations,count,hooks());
  return receipt.status==g::TriggerEntryStatus50IN::Complete?Status::Complete:fail(Status::Service);
 }
 g::TriggerColliderContacts50IN* collider(std::uint32_t identity)noexcept {
  const auto index=colliderIndex(identity);return ready_&&index<group_.colliders.size()?&group_.colliders[index]:nullptr;
 }
 // Enclose exactly one actual main/secondary native query in the correct
 // source racer arena visit. The lease must outlive that synchronous query.
 bool beginQuery(const ActionLease& lease)noexcept {
  if(!ready_||lease_||terminal_!=Status::Complete||lease.listenerCount>2||
     (lease.listenerCount&&!lease.listenerPositions)||!lease.nodeC80)return false;
  bool found=false;for(std::size_t i=0;i<racerCount_;++i)found=found||racers_[i].main690==lease.racerMain690;
  if(!found)return false;lease_=&lease;return true;
 }
 bool endQuery()noexcept {
  if(!lease_)return false;lease_=nullptr;
  if(actionFailed_||group_.actionsFailed){fail(Status::Action);return false;}return true;
 }
 // Store only genuine prepared query state, including initial grounded grid
 // poses and every committed live prefix. Immutable COL owners outlive us.
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,
     const g::BodyMatrix48G& world)noexcept {
  const auto c=colliderIndex(collider);
  if(!ready_||c==group_.colliders.size()||!resource.nodes||!resource.nodeCount||
     (resource.root&g::type2Leaf49X)||resource.root>=resource.nodeCount||
     !g::trigger_sphere50in_detail::affine(world))return false;
  geometry_[c]={&resource,world,true};return true;
 }
 Status serviceWorld()noexcept {
  if(!ready_)return fail(Status::Unprepared);
  if(lease_||terminal_!=Status::Complete)return terminal_==Status::Complete?fail(Status::Input):terminal_;
  for(std::size_t t=0;t<group_.triggers.size();++t){
   const auto& state=group_.triggers[t];const auto& authored=source_->source(t);
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){
    const auto id=state.collider.membersCC[slot];if(!id)continue;
    const auto c=colliderIndex(id);if(c==group_.colliders.size()||!geometry_[c].valid)return fail(Status::Input);
    const auto& geometry=geometry_[c];bool overlap=false;
    if(authored.shapeType==6){
     // Box triggers: the same native shape6 exit kernel as MA's chain.
     if(!g::nativeType2RootTriggerBoxOverlap50MA(*geometry.resource,geometry.world,authored.box,overlap))return fail(Status::Service);
    }else{
    const auto& root=geometry.resource->nodes[geometry.resource->root];
    g::TriggerSphereRootExitInput50IN input{};input.type2PreparedWorld=geometry.world;
    for(unsigned axis=0;axis<3;++axis){input.sphere.localCenter.v[axis]=authored.sphere.center.v[axis];input.rootCenter.v[axis]=root.center[axis];input.rootHalf.v[axis]=root.half[axis];}
    input.sphere.localCenter.v[3]=input.rootCenter.v[3]=input.rootHalf.v[3]=1.f;
    for(unsigned axis=0;axis<4;++axis)input.sphere.world.v[axis*5]=1.f;
    input.sphere.radius50=authored.sphere.radius;volatile float square=authored.sphere.radius*authored.sphere.radius;input.sphere.radiusSquared54=square;
    if(g::overlapTriggerSphereType2Root50IN(input,overlap)!=g::TriggerSphereRootExitStatus50IN::Complete)return fail(Status::Service);
    }
    observations[slot]={&group_.colliders[c],overlap};
   }
   g::TriggerEntryReceipt50IN receipt{};const auto result=service(t,observations.data(),state.collider.highWaterD0,receipt);
   if(result!=Status::Complete)return result;
  }
  return Status::Complete;
 }
 g::NativeTriggerGroup50GA& nativeGroup()noexcept{return group_;}
 const g::TriggerEntryState50IN& state(std::size_t i)const{return group_.triggers.at(i);}
 const Counters& counters()const noexcept{return counters_;}
 Status status()const noexcept{return terminal_;}
 bool ready()const noexcept{return ready_;}
};
}

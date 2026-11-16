#pragma once
#include "ma_trigger_authoring_fixture.h"
#include "course_trigger_sources50ct.h"
#include "../../src/game/native_trigger_group50ga.h"
#include "../../src/game/trigger_actions50in.h"
#include "../../src/game/trigger_sphere50in.h"
#include <array>

namespace ma_trigger_actions {
namespace g=rr::game;
inline constexpr std::size_t racers=8,colliders=racers*2;
inline constexpr std::uint32_t firstTriggerIdentity=0x36000000u;
struct RacerIdentities {std::uint32_t main690=0,secondary=0;};
struct ActionLease {
 // Borrowed ONLY during one genuine main/secondary query in this source
 // racer's live serial arena. Never retained in world state or another bank.
 std::uint32_t racerMain690=0;std::uint32_t* nodeC80=nullptr;
};
struct Counters {std::uint64_t entries=0,exits=0,lostAiWrites=0;};
enum class Status {Complete,Unprepared,Input,IgnoredPlayer,Entry,Action,Service};
class Owner final {
 const course_trigger_sources50CT::Owner* source_=nullptr;
 std::array<RacerIdentities,racers> racers_{};
 std::size_t racerCount_=0;
 g::NativeTriggerGroup50GA group_{};
 struct Geometry {const g::Type2ResourceView49X* resource=nullptr;g::BodyMatrix48G world{};bool valid=false;};
 std::array<Geometry,colliders> geometry_{};
 const ActionLease* lease_=nullptr;
 std::uint32_t originalAiNodeCount_=0,playerPrefixCount34_=0;
 bool ready_=false,attempted_=false,actionFailed_=false;
 Counters counters_{};Status terminal_=Status::Complete;
 std::size_t colliderIndex(std::uint32_t id)const noexcept{return g::nativeTriggerCollider50GA(group_,id);}
 bool preflight(const g::TriggerActionCall50IN& call)const noexcept {
  if(!ready_||call.triggerCollider<firstTriggerIdentity)return false;
  const auto t=call.triggerCollider-firstTriggerIdentity;
  if(t>=group_.triggers.size()||call.actionIndex!=0)return false;
  if(!call.entering)return true;
  const auto c=colliderIndex(call.otherCollider);
  const auto& action=source_->source(t);
  return c<group_.colliders.size()&&lease_&&lease_->racerMain690==racers_[c/2].main690&&
    !action.reset&&action.targetNodeIndex<originalAiNodeCount_&&((c&1u)||lease_->nodeC80);
 }
 void apply(const g::TriggerActionCall50IN& call)noexcept {
  if(!call.entering){++counters_.exits;return;}
  ++counters_.entries;
  const auto& action=source_->source(call.triggerCollider-firstTriggerIdentity);
  std::array<g::LostAiRacer50IN,racers> destinations{};
  for(std::size_t i=0;i<racerCount_;++i)destinations[i]={racers_[i].main690,
    racers_[i].main690==lease_->racerMain690?lease_->nodeC80:nullptr};
  const auto result=g::triggerLostAi50IN(true,call.otherCollider,{action.targetNodeIndex,action.reset},
    destinations.data(),racerCount_,originalAiNodeCount_);
  actionFailed_=actionFailed_||result.status!=g::TriggerActionStatus50IN::Complete;
  group_.actionsFailed=actionFailed_;counters_.lostAiWrites+=result.changedRacers;
 }
 g::TriggerActionHooks50IN hooks()noexcept {
  return {this,[](void* p,const g::TriggerActionCall50IN& c)noexcept{return static_cast<Owner*>(p)->preflight(c);},
   [](void* p,const g::TriggerActionCall50IN& c)noexcept{static_cast<Owner*>(p)->apply(c);}};
 }
 Status fail(Status s)noexcept {if(terminal_==Status::Complete)terminal_=s;return terminal_;}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool prepare(const course_trigger_sources50CT::Owner& source,const std::array<RacerIdentities,racers>& identities,
     std::uint32_t aiNodeCount,std::uint32_t playerPrefixCount34,std::size_t racerCount=racers)noexcept {
  if(attempted_)return false;attempted_=true;
  // Every LostAI target must lie inside the live AI spline (checked per
  // trigger below), not a stock node-count floor.
  const auto live=source.liveCount();
  if(!source.ready()||!live||!aiNodeCount)return false;
  // Source PLAYER resolver1B5340 visits actual roster[0,manager+34).
  // The caller must bind that manager field. AI class58 and camera index do
  // not determine this prefix;0 is admitted only as an explicit source value.
  if(!racerCount||racerCount>racers||playerPrefixCount34>racerCount)return false;
  std::array<std::uint32_t,colliders> ids{};
  for(std::size_t i=0;i<racerCount;++i){ids[i*2]=identities[i].main690;ids[i*2+1]=identities[i].secondary;}
  for(std::size_t i=0;i<racerCount*2;++i){
   if(!ids[i]||(ids[i]>=firstTriggerIdentity&&ids[i]<firstTriggerIdentity+course_trigger_sources50CT::capacity))return false;
   for(std::size_t j=0;j<i;++j)if(ids[i]==ids[j])return false;
  }
  if(!group_.triggers.setCount(live))return false;
  source_=&source;racers_=identities;racerCount_=racerCount;originalAiNodeCount_=aiNodeCount;playerPrefixCount34_=playerPrefixCount34;
  if(!group_.colliders.setCount(racerCount*2))return false;
  for(std::size_t i=0;i<racerCount*2;++i)group_.colliders[i].identity=ids[i];
  for(std::size_t i=0;i<group_.triggers.size();++i){const auto& authored=source.source(i);auto& state=group_.triggers[i];
   // PORT STAND-IN (flagged): this native action owner implements one LostAI
   // action, StartEnabled TRUE and OneTimeOnly FALSE (flags 3/3); other
   // authored values are refused here, never ignored.
   if(!authored.startEnabled||authored.oneTimeOnly||authored.reset||authored.actionCount!=1||
      authored.actions[0].kind!=course_trigger_sources50CT::Action::LostAI||authored.randomTriggerPercent!=100||
      authored.targetNodeIndex>=aiNodeCount)return false;
   state.flags29=3;state.flags1B8=3;state.randomTriggerPercent90=100;state.callbackCount98=1;
   state.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
   // PLAYER-ignore from each trigger's own IgnoreTrigger (stock MA: all 13).
   group_.ignoreColliderCount[i]=authored.ignorePlayer?playerPrefixCount34_:0u;
   for(std::size_t j=0;j<group_.ignoreColliderCount[i];++j)group_.ignoreColliders[i][j]=identities[j].main690;
  }
  ready_=true;group_.actions=hooks();group_.ready=true;
  if(!g::validNativeTriggerGroup50GA(group_)){ready_=false;group_.ready=false;return false;}return true;
 }
 Status dispatchAdmittedContact(std::size_t trigger,std::uint32_t collider,
     const ActionLease& lease,g::TriggerEntryReceipt50IN& receipt)noexcept {
  receipt={};if(terminal_!=Status::Complete)return terminal_;if(!ready_)return fail(Status::Unprepared);
  const auto c=colliderIndex(collider);
  if(trigger>=group_.triggers.size()||c==group_.colliders.size()||lease_||lease.racerMain690!=racers_[c/2].main690)return fail(Status::Input);
  // Exact ignored identity exits BEFORE touched/fired/RNG/membership writes.
  for(std::size_t i=0;i<group_.ignoreColliderCount[trigger];++i)if(collider==group_.ignoreColliders[trigger][i])return Status::IgnoredPlayer;
  lease_=&lease;receipt=g::dispatchNativeTriggerContact50GA(group_,trigger,collider);lease_=nullptr;
  if(receipt.status!=g::TriggerEntryStatus50IN::Complete||receipt.randomConsumed)return fail(Status::Entry);
  return actionFailed_?fail(Status::Action):Status::Complete;
 }
 bool beginQuery(const ActionLease& lease)noexcept {
  if(!ready_||lease_||terminal_!=Status::Complete||!lease.nodeC80)return false;
  bool found=false;for(std::size_t i=0;i<racerCount_;++i)found=found||racers_[i].main690==lease.racerMain690;
  if(!found)return false;lease_=&lease;return true;
 }
 bool endQuery()noexcept {
  if(!lease_)return false;lease_=nullptr;
  if(actionFailed_||group_.actionsFailed){fail(Status::Action);return false;}return true;
 }
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,
     const g::BodyMatrix48G& world)noexcept {
  const auto c=colliderIndex(collider);
  if(!ready_||c==group_.colliders.size()||!resource.nodes||!resource.nodeCount||!resource.rays||!resource.rayCount||
     (resource.root&g::type2Leaf49X)||resource.root>=resource.nodeCount||!g::trigger_sphere50in_detail::affine(world))return false;
  geometry_[c]={&resource,world,true};return true;
 }
 Status serviceWorld()noexcept {
  if(!ready_)return fail(Status::Unprepared);
  if(lease_||terminal_!=Status::Complete)return terminal_==Status::Complete?fail(Status::Input):terminal_;
  if(!g::validNativeTriggerGroup50GA(group_))return fail(Status::Input);
  for(std::size_t t=0;t<group_.triggers.size();++t){const auto& state=group_.triggers[t];const auto& authored=source_->source(t);
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){const auto id=state.collider.membersCC[slot];if(!id)continue;
    const auto c=colliderIndex(id);if(c==group_.colliders.size()||!geometry_[c].valid)return fail(Status::Input);
    const auto& geometry=geometry_[c];bool overlap=false;
    if(authored.shapeType==6){
     if(!g::nativeType2RootTriggerBoxOverlap50MA(*geometry.resource,geometry.world,authored.box,overlap))return fail(Status::Service);
    }else{
     const auto& root=geometry.resource->nodes[geometry.resource->root];g::TriggerSphereRootExitInput50IN input{};input.type2PreparedWorld=geometry.world;
     for(unsigned axis=0;axis<3;++axis){input.sphere.localCenter.v[axis]=authored.sphere.center.v[axis];input.rootCenter.v[axis]=root.center[axis];input.rootHalf.v[axis]=root.half[axis];}
     input.sphere.localCenter.v[3]=input.rootCenter.v[3]=input.rootHalf.v[3]=1.f;
     for(unsigned axis=0;axis<4;++axis)input.sphere.world.v[axis*5]=1.f;
     input.sphere.radius50=authored.sphere.radius;volatile float square=authored.sphere.radius*authored.sphere.radius;input.sphere.radiusSquared54=square;
     if(g::overlapTriggerSphereType2Root50IN(input,overlap)!=g::TriggerSphereRootExitStatus50IN::Complete)return fail(Status::Service);
    }
    observations[slot]={&group_.colliders[c],overlap};
   }
   const auto receipt=g::serviceTrigger50IN(group_.triggers[t],observations.data(),state.collider.highWaterD0,hooks());
   if(receipt.status!=g::TriggerEntryStatus50IN::Complete)return fail(Status::Service);
  }
  return Status::Complete;
 }
 g::TriggerColliderContacts50IN* collider(std::uint32_t identity)noexcept {
  const auto c=colliderIndex(identity);return ready_&&c<group_.colliders.size()?&group_.colliders[c]:nullptr;
 }
 g::NativeTriggerGroup50GA& nativeGroup()noexcept{return group_;}
 const g::TriggerEntryState50IN& state(std::size_t i)const{return group_.triggers.at(i);}
 const Counters& counters()const noexcept{return counters_;}
 Status status()const noexcept{return terminal_;}
 bool ready()const noexcept{return ready_;}
};
} // namespace ma_trigger_actions

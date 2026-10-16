#pragma once
#include "ra_trigger_authoring50la.h"
#include "../../src/game/trigger_entry50in.h"
#include "../../src/game/native_trigger_group50ga.h"
#include "../../src/game/trigger_sphere50in.h"
#include "../../src/game/type2_resource49x.h"
#include "../../src/game/trigger_actions50in.h"
#include <array>

// Two authenticated RA LostAI actions and source trigger lifecycle only.
// Entry geometry is the RA-only current-segment/sphere policy,
// distinct from unresolved original108E90. Source122BA0 is EXIT-only here.
// No animation/audio owners, listener state, HOST triggers, or route driver.
namespace ra_trigger_actions50LA {
namespace g=rr::game;
inline constexpr std::size_t racers=8,colliders=racers*2;
inline constexpr std::uint32_t firstTriggerIdentity=0x34000000u;
struct RacerIdentities {std::uint32_t main690=0,secondary=0;};
struct ActionLease {
 // Borrowed only for one synchronous actual collider query inside the source
 // racer's arena visit. All eight racers retain their source AI/control owner.
 // Never retain this live C80 address across serial racer-bank visits.
 std::uint32_t racerMain690=0;std::uint32_t* nodeC80=nullptr;
};
struct Counters {std::uint64_t entries=0,exits=0,lostAiWrites=0;};
enum class Status {Complete,Unprepared,Input,IgnoredPlayer,Entry,Action,Service};
class Owner final {
 const ra_trigger_authoring50LA::Owner* source_=nullptr;
 std::array<RacerIdentities,racers> racers_{};
 g::NativeTriggerGroup50GA group_{};
 struct Geometry {const g::Type2ResourceView49X* resource=nullptr;g::BodyMatrix48G world{};bool valid=false;};
 std::array<Geometry,colliders> geometry_{};
 const ActionLease* lease_=nullptr;
 std::uint32_t originalAiNodeCount_=0,playerPrefixCount34_=0;
 bool attempted_=false,ready_=false,actionFailed_=false;
 Counters counters_{};Status terminal_=Status::Complete;
 std::size_t colliderIndex(std::uint32_t id)const noexcept {
  return g::nativeTriggerCollider50GA(group_,id);
 }
 bool preflight(const g::TriggerActionCall50IN& call)const noexcept {
  if(!ready_||call.triggerCollider<firstTriggerIdentity)return false;
  const auto t=call.triggerCollider-firstTriggerIdentity;
  if(t>=group_.triggers.size()||call.actionIndex>=1u)return false;
  if(!call.entering)return true; // Original1B27C0 ignores exits.
  const auto c=colliderIndex(call.otherCollider);
  if(c==group_.colliders.size()||!lease_||lease_->racerMain690!=racers_[c/2].main690)return false;
  const auto& action=(*source_->sources())[t].action;
  return !action.reset&&action.targetNodeIndex<originalAiNodeCount_&&((c&1u)||lease_->nodeC80);
 }
 void apply(const g::TriggerActionCall50IN& call)noexcept {
  if(!call.entering){++counters_.exits;return;}
  ++counters_.entries;
  const auto t=call.triggerCollider-firstTriggerIdentity;
  const auto& action=(*source_->sources())[t].action;
  // Original race traversal matches only racer.main690. A secondary contact
  // still owns lifecycle membership but never writes any racer's C80.
  // PLAYER exclusion is identity-based, independent of slot and AI role.
  std::array<g::LostAiRacer50IN,racers> destinations{};
  for(std::size_t i=0;i<racers;++i)destinations[i]={racers_[i].main690,
    racers_[i].main690==lease_->racerMain690?lease_->nodeC80:nullptr};
  const auto result=g::triggerLostAi50IN(true,call.otherCollider,
    {action.targetNodeIndex,action.reset},destinations.data(),destinations.size(),originalAiNodeCount_);
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
 bool prepare(const ra_trigger_authoring50LA::Owner& source,
     const std::array<RacerIdentities,racers>& identities,
     std::uint32_t playerPrefixCount34,std::uint32_t aiNodeCount)noexcept {
  if(attempted_)return false;attempted_=true;
  if(!source.sources()||aiNodeCount!=113u||playerPrefixCount34>racers)return false;
  // Original1B5340 resolves PLAYER by source roster prefix[0,manager+34),
  // via1AF330(index)->main690. The caller supplies that genuine count and
  // source-order identities. It is not class58, AI role, or camera choice.
  // Zero explicitly means an empty source prefix, never a slot0 fallback.
  std::array<std::uint32_t,colliders> ids{};
  for(std::size_t i=0;i<racers;++i){ids[i*2]=identities[i].main690;ids[i*2+1]=identities[i].secondary;}
  for(std::size_t i=0;i<ids.size();++i){
   if(!ids[i]||(ids[i]>=firstTriggerIdentity&&ids[i]<firstTriggerIdentity+ra_trigger_authoring50LA::count))return false;
   for(std::size_t j=0;j<i;++j)if(ids[i]==ids[j])return false;
  }
  for(std::size_t i=0;i<ra_trigger_authoring50LA::count;++i){const auto& s=(*source.sources())[i];
   if(s.randomTriggerPercent90!=100||!s.startEnabled||s.oneTimeOnly||!s.ignorePlayer||
      s.action.reset||s.action.targetNodeIndex>=aiNodeCount)return false;
  }
  // Select only the two authenticated RA triggers before any state writes.
  if(!group_.triggers.setCount(ra_trigger_authoring50LA::count))return false;
  source_=&source;racers_=identities;playerPrefixCount34_=playerPrefixCount34;originalAiNodeCount_=aiNodeCount;
  for(std::size_t i=0;i<ids.size();++i)group_.colliders[i].identity=ids[i];
  for(std::size_t i=0;i<group_.triggers.size();++i){const auto& s=(*source.sources())[i];auto& state=group_.triggers[i];
   state.flags29=s.startEnabled?3u:0u;
   state.flags1B8=static_cast<std::uint8_t>(3u|(s.oneTimeOnly?4u:0u));
   state.randomTriggerPercent90=s.randomTriggerPercent90;
   state.callbackCount98=1u;state.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
   group_.ignoreColliderCount[i]=s.ignorePlayer?playerPrefixCount34_:0u;
   for(std::size_t j=0;j<group_.ignoreColliderCount[i];++j)group_.ignoreColliders[i][j]=identities[j].main690;
  }
  ready_=true;group_.actions=hooks();group_.ready=true;
  if(g::validNativeTriggerGroup50GA(group_))return true;
  ready_=false;group_.ready=false;fail(Status::Input);return false;
 }
 // Call only after real declared-backend geometry and the ordinary bilateral
 // collection/category/identity filters admit this pair. Never feed122BA0.
 Status dispatchAdmittedContact(std::size_t trigger,std::uint32_t collider,
     const ActionLease& lease,g::TriggerEntryReceipt50IN& receipt)noexcept {
  receipt={};if(terminal_!=Status::Complete)return terminal_;
  if(!ready_)return fail(Status::Unprepared);
  const auto c=colliderIndex(collider);
  if(trigger>=group_.triggers.size()||c==group_.colliders.size()||lease_||
     lease.racerMain690!=racers_[c/2].main690)return fail(Status::Input);
  // Suppress the exact source PLAYER prefix before any lifecycle state write.
  for(std::size_t i=0;i<group_.ignoreColliderCount[trigger];++i)
   if(collider==group_.ignoreColliders[trigger][i])return Status::IgnoredPlayer;
  lease_=&lease;receipt=g::dispatchNativeTriggerContact50GA(group_,trigger,collider);lease_=nullptr;
  if(receipt.status!=g::TriggerEntryStatus50IN::Complete||receipt.randomConsumed)return fail(Status::Entry);
  if(actionFailed_)return fail(Status::Action);
  return Status::Complete;
 }
 // One exact original122BA0 exit observation per high-water slot, including
 // null holes. Every nonnull collider must belong to this stable owner.
 Status service(std::size_t trigger,const g::TriggerOverlap50IN* observations,
     std::size_t count,g::TriggerEntryReceipt50IN& receipt)noexcept {
  receipt={};if(terminal_!=Status::Complete)return terminal_;
  if(!ready_)return fail(Status::Unprepared);
  if(trigger>=group_.triggers.size()||lease_||count!=group_.triggers[trigger].collider.highWaterD0||
     count>g::triggerContactCapacity50IN||(count&&!observations))return fail(Status::Input);
  for(std::size_t i=0;i<count;++i)if(observations[i].collider){
   bool owned=false;for(auto& c:group_.colliders)owned=owned||observations[i].collider==&c;
   if(!owned)return fail(Status::Input);
  }
  receipt=g::serviceTrigger50IN(group_.triggers[trigger],observations,count,hooks());
  return receipt.status==g::TriggerEntryStatus50IN::Complete?Status::Complete:fail(Status::Service);
 }
 g::TriggerColliderContacts50IN* collider(std::uint32_t identity)noexcept {
  const auto index=colliderIndex(identity);return ready_&&index<group_.colliders.size()?&group_.colliders[index]:nullptr;
 }
 // Enclose one actual main/secondary native query in that source racer's
 // arena visit. The live C80 lease must outlive the whole synchronous query.
 bool beginQuery(const ActionLease& lease)noexcept {
  if(!ready_||lease_||terminal_!=Status::Complete||!lease.nodeC80)return false;
  bool found=false;for(const auto& r:racers_)found=found||r.main690==lease.racerMain690;
  if(!found)return false;lease_=&lease;return true;
 }
 bool endQuery()noexcept {
  if(!lease_)return false;lease_=nullptr;
  if(actionFailed_||group_.actionsFailed){fail(Status::Action);return false;}return true;
 }
 // Exact genuine prepared poses: initial grounded grid and each committed
 // live prefix. Immutable COL resources and their node storage outlive us.
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
   const auto& state=group_.triggers[t];const auto& authored=(*source_->sources())[t];
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){
    const auto id=state.collider.membersCC[slot];if(!id)continue;
    const auto c=colliderIndex(id);if(c==group_.colliders.size()||!geometry_[c].valid)return fail(Status::Input);
    const auto& geometry=geometry_[c];const auto& root=geometry.resource->nodes[geometry.resource->root];
    g::TriggerSphereRootExitInput50IN input{};input.type2PreparedWorld=geometry.world;
    for(unsigned axis=0;axis<3;++axis){input.sphere.localCenter.v[axis]=authored.sphere.center.v[axis];input.rootCenter.v[axis]=root.center[axis];input.rootHalf.v[axis]=root.half[axis];}
    input.sphere.localCenter.v[3]=input.rootCenter.v[3]=input.rootHalf.v[3]=1.f;
    for(unsigned axis=0;axis<4;++axis)input.sphere.world.v[axis*5]=1.f;
    input.sphere.radius50=authored.sphere.radius;volatile float square=authored.sphere.radius*authored.sphere.radius;input.sphere.radiusSquared54=square;
    bool overlap=false;
    if(g::overlapTriggerSphereType2Root50IN(input,overlap)!=g::TriggerSphereRootExitStatus50IN::Complete)return fail(Status::Service);
    observations[slot]={&group_.colliders[c],overlap};
   }
   g::TriggerEntryReceipt50IN receipt{};const auto result=service(t,observations.data(),state.collider.highWaterD0,receipt);
   if(result!=Status::Complete)return result;
  }
  return Status::Complete;
 }
 g::NativeTriggerGroup50GA& nativeGroup()noexcept{return group_;}
 const ra_trigger_authoring50LA::Owner* authoring()const noexcept{return source_;}
 const g::TriggerEntryState50IN& state(std::size_t i)const{return group_.triggers.at(i);}
 std::uint32_t playerPrefixCount34()const noexcept{return playerPrefixCount34_;}
 const Counters& counters()const noexcept{return counters_;}
 Status status()const noexcept{return terminal_;}
 bool ready()const noexcept{return ready_;}
};
} // namespace ra_trigger_actions50LA

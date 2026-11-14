#pragma once
#include "sc_trigger_authoring_fixture.h"
#include "../../src/game/native_trigger_group50ga.h"
#include "../../src/game/trigger_sphere50in.h"
#include <array>
#include <memory>

namespace sc_trigger_actions {
namespace g=rr::game;
inline constexpr std::size_t racers=8,colliders=racers*2;
inline constexpr std::uint32_t firstTriggerIdentity=0x36000000u;
struct RacerIdentities {std::uint32_t main690=0,secondary=0;};
struct ActionLease {
 // Root supplies actual current Bank arena fields; this borrow expires before
 // any Bank restore. A roster index is not a collider identity or C80 owner.
 std::uint32_t racerMain690=0;std::uint32_t* nodeC80=nullptr;
};
struct Counters {std::uint64_t entries=0,exits=0,lostAiWrites=0;};
enum class Status {Complete,Unprepared,Input,Service};
class Owner final {
 std::shared_ptr<const sc_trigger_authoring::Owner> source_;
 std::array<RacerIdentities,racers> racers_{};
 std::size_t racerCount_=0;
 g::NativeTriggerGroup50GA group_{};
 struct Geometry {g::BodyMatrix48G world{};std::array<float,3> center{},half{};bool valid=false;};
 std::array<Geometry,colliders> geometry_{};
 const ActionLease* lease_=nullptr;
 std::uint32_t originalAiNodeCount_=0;
 bool ready_=false,attempted_=false,servicing_=false;
 Counters counters_{};Status terminal_=Status::Complete;
 static void increment(std::uint64_t& value)noexcept{if(value!=~std::uint64_t{0})++value;}
 std::size_t colliderIndex(std::uint32_t id)const noexcept{return g::nativeTriggerCollider50GA(group_,id);}
 bool preflight(const g::TriggerActionCall50IN& call)const noexcept {
  if(!ready_||terminal_!=Status::Complete||call.triggerCollider<firstTriggerIdentity)return false;
  const auto t=call.triggerCollider-firstTriggerIdentity;
  if(t>=source_->liveCount()||call.actionIndex!=0)return false;
  const auto c=colliderIndex(call.otherCollider);if(c>=group_.colliders.size())return false;
  if(!call.entering)return servicing_&&!lease_;
  const auto& action=source_->source(t);
  return !servicing_&&lease_&&lease_->racerMain690==racers_[c/2].main690&&
    !action.authoredReset&&!action.effectiveReset1B5A60&&
    action.targetNodeIndex<originalAiNodeCount_&&((c&1u)||lease_->nodeC80);
 }
 void apply(const g::TriggerActionCall50IN& call)noexcept {
  if(!call.entering){increment(counters_.exits);return;}
  increment(counters_.entries);
  const auto& action=source_->source(call.triggerCollider-firstTriggerIdentity);
  // Actual1B27C0 scans main690 identities in race order. Preflight proves the
  // only matching live destination. Reset is authenticated absent/default0,
  // so apply consists solely of this infallible scalar store. No class58 test.
  for(std::size_t i=0;i<racerCount_;++i)if(racers_[i].main690==call.otherCollider){
   *lease_->nodeC80=action.targetNodeIndex;increment(counters_.lostAiWrites);
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
 ~Owner(){if(lease_||servicing_)std::terminate();}
 bool prepare(std::shared_ptr<const sc_trigger_authoring::Owner> source,
     const std::array<RacerIdentities,racers>& identities,std::uint32_t actualAiCount,
     std::uint32_t actualPlayerPrefixCount34,std::size_t racerCount=racers)noexcept {
  if(attempted_)return false;attempted_=true;
  const auto* cold=source?source->coldSource():nullptr;
  const auto* data=cold?cold->sourcesDecoded():nullptr;
  if(!source||!source->ready()||!data||!data->ai.gatesPrepared||
     actualAiCount!=data->ai.track.count||!racerCount||racerCount>racers||actualPlayerPrefixCount34>racerCount)return false;
  std::array<std::uint32_t,colliders> ids{};
  for(std::size_t i=0;i<racerCount;++i){ids[i*2]=identities[i].main690;ids[i*2+1]=identities[i].secondary;}
  for(std::size_t i=0;i<racerCount*2;++i){
   if(!ids[i]||(ids[i]>=firstTriggerIdentity&&ids[i]<firstTriggerIdentity+sc_trigger_authoring::count))return false;
   for(std::size_t j=0;j<i;++j)if(ids[i]==ids[j])return false;
  }
  g::NativeTriggerGroup50GA next{};
  const auto live=source->liveCount();
  if(!live||!next.triggers.setCount(live))return false;
  if(!next.colliders.setCount(racerCount*2))return false;
  for(std::size_t i=0;i<racerCount*2;++i)next.colliders[i].identity=ids[i];
  for(std::size_t i=0;i<live;++i){const auto& authored=source->source(i);auto& state=next.triggers[i];
   // PLAYER-ignore comes from each trigger's own IgnoreTrigger (no index rule).
   // FLAG stand-in: flags29/1B8=3 encode StartEnabled TRUE + OneTimeOnly FALSE;
   // other authored activation values are refused until their bits are read.
   if(!authored.startEnabled||authored.oneTimeOnly||
      authored.authoredReset||authored.effectiveReset1B5A60||authored.actionCount!=1||
      authored.randomTriggerPercent!=100||authored.targetNodeIndex>=actualAiCount)return false;
   state.flags29=3;state.flags1B8=3;state.randomTriggerPercent90=100;state.callbackCount98=1;
   state.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
   if(authored.ignorePlayer){next.ignoreColliderCount[i]=actualPlayerPrefixCount34;
    for(std::size_t j=0;j<actualPlayerPrefixCount34;++j)next.ignoreColliders[i][j]=identities[j].main690;
   }
  }
  next.actions=hooks();next.ready=true;
  if(!g::validNativeTriggerGroup50GA(next))return false;
  source_=std::move(source);racers_=identities;racerCount_=racerCount;originalAiNodeCount_=actualAiCount;
  group_=std::move(next);ready_=true;return true;
 }
 bool beginQuery(const ActionLease& lease)noexcept {
  if(!ready_||lease_||servicing_||terminal_!=Status::Complete||!lease.nodeC80)return false;
  bool found=false;for(std::size_t i=0;i<racerCount_;++i)found=found||racers_[i].main690==lease.racerMain690;
  if(!found)return false;lease_=&lease;return true;
 }
 bool endQuery()noexcept {
  if(!lease_)return false;lease_=nullptr;
  return terminal_==Status::Complete&&!group_.actionsFailed;
 }
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,
     const g::BodyMatrix48G& world)noexcept {
  const auto c=colliderIndex(collider);
  if(!ready_||servicing_||terminal_!=Status::Complete||c==group_.colliders.size()||!resource.nodes||!resource.nodeCount||
     !resource.rays||!resource.rayCount||(resource.root&g::type2Leaf49X)||resource.root>=resource.nodeCount||
     !g::trigger_sphere50in_detail::affine(world))return false;
  const auto& root=resource.nodes[resource.root];Geometry next{};next.world=world;
  for(unsigned k=0;k<3;++k){
   if(!std::isfinite(root.center[k])||!std::isfinite(root.half[k])||root.half[k]<0.f)return false;
   next.center[k]=root.center[k];next.half[k]=root.half[k];
  }
  next.valid=true;geometry_[c]=next;return true;
 }
 Status serviceWorld()noexcept {
  if(!ready_)return fail(Status::Unprepared);
  if(lease_||servicing_||terminal_!=Status::Complete)return terminal_==Status::Complete?fail(Status::Input):terminal_;
  if(!g::validNativeTriggerGroup50GA(group_))return fail(Status::Input);
  servicing_=true;struct End{bool& flag;~End(){flag=false;}}end{servicing_};
  for(std::size_t t=0;t<source_->liveCount();++t){const auto& state=group_.triggers[t];const auto& authored=source_->source(t);
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){const auto id=state.collider.membersCC[slot];if(!id)continue;
    const auto c=colliderIndex(id);if(c==group_.colliders.size()||!geometry_[c].valid)return fail(Status::Input);
    const auto& geometry=geometry_[c];bool overlap=false;
    if(authored.shapeType==6){
     // Typed view of only the actual observed root scalars. The exit kernel
     // reads no rays/topology; this never becomes a new query resource owner.
     g::Type2Node49X root{};root.center=geometry.center;root.half=geometry.half;
     g::Type2ResourceView49X view{};view.nodes=&root;view.nodeCount=1;
     if(!g::nativeType2RootTriggerBoxOverlap50MA(view,geometry.world,authored.box,overlap))return fail(Status::Service);
    }else{
     g::TriggerSphereRootExitInput50IN input{};input.type2PreparedWorld=geometry.world;
     for(unsigned k=0;k<3;++k){input.sphere.localCenter.v[k]=authored.sphere.center.v[k];input.rootCenter.v[k]=geometry.center[k];input.rootHalf.v[k]=geometry.half[k];}
     input.sphere.localCenter.v[3]=input.rootCenter.v[3]=input.rootHalf.v[3]=1.f;
     for(unsigned k=0;k<4;++k)input.sphere.world.v[k*5]=1.f;
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
} // namespace sc_trigger_actions

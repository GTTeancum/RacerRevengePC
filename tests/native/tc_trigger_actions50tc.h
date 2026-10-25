#pragma once
#include "tc_trigger_authoring50tc.h"
#include "tc_gate_runtime50tc.h"
#include "tc_global_sound50tc.h"
#include "../../src/game/native_trigger_group50tc.h"
#include "../../src/game/trigger_sphere50in.h"
#include <array>
#include <memory>
#include <thread>
namespace tc_trigger_actions50TC {
namespace g=rr::game;
inline constexpr std::size_t racers=8,colliders=racers*2;
inline constexpr std::uint32_t firstTriggerIdentity=0x38000000u;
struct RacerIdentities {std::uint32_t main690=0,secondary=0;};
struct ActionLease {
 // Exact current Bank arena scalar borrow. QueryCollider is the actual main
 // or secondary collider, never a fabricated index; lifetime ends pre-restore.
 std::uint32_t racerMain690=0,queryCollider=0;std::uint32_t* nodeC80=nullptr;
};
struct Counters {std::uint64_t entries=0,exits=0,lostAiWrites=0,animationActivations=0,soundStarts=0;};
enum class Status {Complete,Unprepared,Input,Service,Action};
class Owner final {
 std::shared_ptr<const tc_trigger_authoring50TC::Owner> source_;
 std::shared_ptr<tc_gate_runtime50TC::Owner> gate_;
 std::shared_ptr<tc_global_sound50TC::Owner> sounds_;
 std::array<RacerIdentities,racers> racers_{};
 std::size_t racerCount_=0;
 g::NativeTriggerGroup50TC group_{};
 struct Geometry {g::BodyMatrix48G world{};std::array<float,3> center{},half{};bool valid=false;};
 std::array<Geometry,colliders> geometry_{};
 const ActionLease* lease_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool ready_=false,attempted_=false,sourcePrepared_=false,bindAttempted_=false,servicing_=false;
 Counters counters_{};Status terminal_=Status::Complete;std::string error_;
 static void increment(std::uint64_t& v)noexcept{if(v!=~std::uint64_t{0})++v;}
 std::size_t colliderIndex(std::uint32_t id)const noexcept{return g::nativeTriggerCollider50TC(group_,id);}
 Status fail(Status s)noexcept {if(terminal_==Status::Complete)terminal_=s;return terminal_;}
 bool preflight(const g::SourceTriggerActionCall50TC& event)const noexcept {
  const auto& call=event.source;
  if(!ready()||terminal_!=Status::Complete||call.triggerCollider<firstTriggerIdentity)return false;
  const auto t=call.triggerCollider-firstTriggerIdentity;const auto c=colliderIndex(call.otherCollider);
  if(t>=tc_trigger_authoring50TC::count||c>=group_.colliders.size())return false;
  const auto& trigger=source_->source(t);if(call.actionIndex>=trigger.actionCount)return false;
  if(!call.entering)return servicing_&&!lease_&&!group_.queryActive&&event.querySerial==0;
  if(servicing_||!lease_||!group_.queryActive||!event.querySerial||event.querySerial!=group_.querySerial||
     group_.queryCollider!=call.otherCollider||lease_->queryCollider!=call.otherCollider||
     lease_->racerMain690!=racers_[c/2].main690)return false;
  const auto& action=trigger.actions[call.actionIndex];
  if(action.kind==tc_trigger_authoring50TC::Action::LostAI)
   return !action.authoredReset&&!action.effectiveReset1B5A60&&action.targetNodeIndex<source_->coldSource()->ai().count()&&((c&1u)||lease_->nodeC80);
  if(action.kind==tc_trigger_authoring50TC::Action::Animation)return t==tc_trigger_authoring50TC::gate&&call.actionIndex==0&&gate_->ready()&&action.animationIndex==0;
  return t==tc_trigger_authoring50TC::gate&&sounds_->readyFor(*source_,call.actionIndex);
 }
 g::SourceTriggerActionResult50TC resume(const g::SourceTriggerActionCall50TC& event)noexcept {
  using A=g::SourceTriggerActionStatus50TC;const auto& call=event.source;
  try{
   if(!preflight(event)){fail(Status::Action);return {A::InvalidBinding,call.sourceSite,0};}
   if(!call.entering){increment(counters_.exits);return {A::Complete,call.sourceSite,0};}
   increment(counters_.entries);const auto& action=source_->source(call.triggerCollider-firstTriggerIdentity).actions[call.actionIndex];
   if(action.kind==tc_trigger_authoring50TC::Action::LostAI){
    for(std::size_t i=0;i<racerCount_;++i)if(racers_[i].main690==call.otherCollider){*lease_->nodeC80=action.targetNodeIndex;increment(counters_.lostAiWrites);}
    return {A::Complete,0x1b27c0u,0xc80u};
   }
   if(action.kind==tc_trigger_authoring50TC::Action::Animation){
    if(!gate_->activateSequence(action.animationIndex)){fail(Status::Action);return {A::Blocked,0x101170u,0x90u};}
    increment(counters_.animationActivations);return {A::Complete,0x101170u,0x90u};
   }
   if(!sounds_->playEntering(*source_,call.actionIndex,action,error_)){fail(Status::Action);return {A::Blocked,0x102920u,0x210u};}
   increment(counters_.soundStarts);return {A::Complete,0x102920u,0x210u};
  }catch(...){fail(Status::Action);return {A::Blocked,call.sourceSite,0};}
 }
 g::SourceTriggerActionHooks50TC hooks()noexcept {
  return {this,[](void* p,const g::SourceTriggerActionCall50TC& c)noexcept{return static_cast<Owner*>(p)->preflight(c);},
    [](void* p,const g::SourceTriggerActionCall50TC& c)noexcept{return static_cast<Owner*>(p)->resume(c);}};
 }
 bool beginQuery(const ActionLease& lease)noexcept {
  if(!ready()||lease_||servicing_||terminal_!=Status::Complete||group_.actionsFailed||!lease.nodeC80||group_.querySerial==~std::uint64_t{0})return false;
  const auto c=colliderIndex(lease.queryCollider);
  if(c>=group_.colliders.size()||lease.racerMain690!=racers_[c/2].main690||!g::releaseCompletedSourceContact50TC(group_.contact))return false;
  lease_=&lease;++group_.querySerial;group_.queryCollider=lease.queryCollider;group_.queryActive=true;return true;
 }
 bool endQuery(bool queryCompleted)noexcept {
  if(!lease_)return false;lease_=nullptr;group_.queryActive=false;
  if(!queryCompleted)fail(Status::Input);
  if(group_.actionsFailed||terminal_!=Status::Complete)return false;
  if(!g::releaseCompletedSourceContact50TC(group_.contact)){fail(Status::Input);return false;}
  return true;
 }
public:
 // A genuine caller keeps ActionLease and the current Bank visit alive for
 // this scope. Destruction always expires raw arena pointers, including errors.
 class QueryBorrow final {
  Owner* owner_=nullptr;
 public:
  QueryBorrow(Owner& owner,const ActionLease& actual)noexcept {if(owner.beginQuery(actual))owner_=&owner;}
  QueryBorrow(const QueryBorrow&)=delete;QueryBorrow&operator=(const QueryBorrow&)=delete;
  ~QueryBorrow(){if(owner_)owner_->endQuery(false);}
  explicit operator bool()const noexcept{return owner_;}
  bool finish(bool actualQueryCompleted)noexcept {if(!owner_)return false;auto* p=owner_;owner_=nullptr;return p->endQuery(actualQueryCompleted);}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(lease_||servicing_||group_.queryActive||thread_!=std::this_thread::get_id())std::terminate();}
 // Cold setup publishes only stable source/group backing. It cannot admit a
 // query until real arena collider identities are bound after Bank visits.
 bool prepare(std::shared_ptr<const tc_trigger_authoring50TC::Owner> source,
     std::shared_ptr<tc_gate_runtime50TC::Owner> gate,std::shared_ptr<tc_global_sound50TC::Owner> sounds)noexcept {
  if(attempted_)return false;attempted_=true;
  if(thread_!=std::this_thread::get_id()||!source||!source->ready()||!source->coldSource()||source->coldSource()->ai().count()!=293||
     !gate||!gate->ready()||gate->coldSource()!=source->coldSource()||!sounds||!sounds->readyFor(*source,1)||!sounds->readyFor(*source,2))return false;
  for(std::size_t i=0;i<tc_trigger_authoring50TC::count;++i){const auto& authored=source->source(i);auto& state=group_.triggers[i];
   if(authored.actionCount!=(i==tc_trigger_authoring50TC::gate?3u:1u)||authored.randomTriggerPercent!=100)return false;
   state.flags29=authored.startEnabled?3:2;state.flags1B8=static_cast<std::uint8_t>(3u|(authored.oneTimeOnly?4u:0u));
   state.randomTriggerPercent90=100;state.callbackCount98=authored.actionCount;
   state.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
  }
  source_=std::move(source);gate_=std::move(gate);sounds_=std::move(sounds);group_.actions=hooks();sourcePrepared_=true;return true;
 }
 bool bindRacerIdentities(const std::array<RacerIdentities,racers>& identities,
     std::uint32_t actualPlayerPrefixCount34,std::size_t racerCount=racers)noexcept {
  if(bindAttempted_||!sourcePrepared_||!preparedSourceFor(*source_)||ready_||lease_||servicing_)return false;bindAttempted_=true;
  if(!racerCount||racerCount>racers||actualPlayerPrefixCount34>racerCount||!sounds_->readyFor(*source_,1)||!sounds_->readyFor(*source_,2))return false;
  std::array<std::uint32_t,colliders> ids{};
  for(std::size_t i=0;i<racerCount;++i){ids[i*2]=identities[i].main690;ids[i*2+1]=identities[i].secondary;}
  for(std::size_t i=0;i<racerCount*2;++i){
   if(!ids[i]||(ids[i]>=firstTriggerIdentity&&ids[i]<firstTriggerIdentity+tc_trigger_authoring50TC::count))return false;
   for(std::size_t j=0;j<i;++j)if(ids[i]==ids[j])return false;
  }
  // Staging contains only scalar/contact records. The published group address
  // stays stable, including candidates already pointing to this same group.
  auto next=group_;
  if(!next.colliders.setCount(racerCount*2))return false;
  for(std::size_t i=0;i<racerCount*2;++i)next.colliders[i].identity=ids[i];
  for(std::size_t i=0;i<tc_trigger_authoring50TC::count;++i)if(source_->source(i).ignorePlayer){
   next.ignoreColliderCount[i]=actualPlayerPrefixCount34;
   for(std::size_t j=0;j<actualPlayerPrefixCount34;++j)next.ignoreColliders[i][j]=identities[j].main690;
  }
  next.ready=true;if(!g::validNativeTriggerGroup50TC(next))return false;
  racers_=identities;racerCount_=racerCount;group_=std::move(next);ready_=true;return true;
 }
 bool preparedSourceFor(const tc_trigger_authoring50TC::Owner& source)const noexcept {
  return sourcePrepared_&&thread_==std::this_thread::get_id()&&source_.get()==&source&&source.ready()&&gate_&&gate_->ready()&&sounds_&&sounds_->prepared();
 }
 const tc_global_sound50TC::Owner* soundOwner()const noexcept{return sounds_.get();}
 const tc_trigger_authoring50TC::Owner* sourceOwner()const noexcept{return source_.get();}
 const tc_cold_sources50LA::Owner* coldSource()const noexcept{return source_?source_->coldSource():nullptr;}
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,
     const g::BodyMatrix48G& world)noexcept {
  const auto c=colliderIndex(collider);
  if(!ready()||servicing_||terminal_!=Status::Complete||c==group_.colliders.size()||!resource.nodes||!resource.nodeCount||
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
  if(!ready())return fail(Status::Unprepared);
  if(lease_||servicing_||group_.queryActive||terminal_!=Status::Complete)return terminal_==Status::Complete?fail(Status::Input):terminal_;
  if(!g::validNativeTriggerGroup50TC(group_))return fail(Status::Input);
  servicing_=true;struct End{bool& flag;~End(){flag=false;}}end{servicing_};
  for(std::size_t t=0;t<tc_trigger_authoring50TC::count;++t){const auto& state=group_.triggers[t];const auto& authored=source_->source(t);
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
   const auto receipt=g::serviceNativeSourceTrigger50TC(group_,t,observations.data(),state.collider.highWaterD0,group_.actions,group_.contact);
   if(receipt.status!=g::SourceTriggerContactStatus50TC::Complete)return fail(Status::Service);
  }
  return Status::Complete;
 }
 g::NativeTriggerGroup50TC& nativeGroup()noexcept{return group_;}
 const g::TriggerEntryState50IN& state(std::size_t i)const{return group_.triggers.at(i);}
 const Counters& counters()const noexcept{return counters_;}
 const std::string& lastError()const noexcept{return error_;}
 Status status()const noexcept{return terminal_;}
 bool ready()const noexcept{return ready_&&thread_==std::this_thread::get_id()&&source_&&source_->ready()&&gate_&&gate_->ready()&&sounds_&&sounds_->prepared();}
};
}

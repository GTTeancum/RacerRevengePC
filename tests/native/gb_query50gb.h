#pragma once
#include "gb_animation_binding50gb.h"
#include "../../src/game/native_trigger_retirement50gb.h"
#include <functional>
namespace gb_query50GB {
namespace g=rr::game;namespace rt=track_runtime_storage50IG;
enum class TerminalStatus:std::uint8_t {Open,Fenced,ExternalBorrower,Backing,Peer,Contacts,Cursor,Complete};
struct TerminalResult {
 TerminalStatus status=TerminalStatus::Open;std::uint64_t epoch=0;
 std::uint16_t completedPeers=0;std::size_t validatedPairs=0,removedPairs=0,owner=0,slot=0;
 g::SourceTriggerContactPhase50GB phase=g::SourceTriggerContactPhase50GB::Idle;
 std::uint64_t querySerial=0;std::size_t candidateOrdinal=0,nextAction=0,actionCount=0;
 g::SourceTriggerContactReceipt50GB lastContact{};
};
// A concrete same-root binding, not a caller-created readiness receipt. It owns
// the aggregate and navigation, while the ActionOwner holds only its identity.
class Binding final {
 native_racer50HN::Bank* bank_=nullptr;
 std::shared_ptr<gb_collision_world50GB::Owner> world_;
 std::shared_ptr<gb_navigation50GB::Owner> navigation_;
 const std::uint32_t* main_=nullptr,*secondary_=nullptr;std::uint32_t* node_=nullptr;
 native_racer50HN::Bank::ReadHandle<std::uint32_t> mainRead_{},secondaryRead_{};
 std::array<g::Vector3_48C,2> listeners_{};std::size_t listenerCount_=0;
 std::uint64_t serial_=0,lastService_=0;
 const std::thread::id thread_=std::this_thread::get_id();
 bool active_=false,fenced_=false,everIssued_=false;
 TerminalResult terminal_{};
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 gb_trigger_actions50GB::Owner* actions()const noexcept{return world_?world_->actions_.get():nullptr;}
 bool idleBank()const noexcept {
  return bank_&&!bank_->retired_&&bank_->sealed_&&bank_->count_>0&&bank_->count_<=8&&
   !bank_->visiting_&&!bank_->readBlocked_&&!bank_->visitDepth_&&!bank_->pendingToken_&&
   !bank_->sourceIssuer_&&!bank_->retirementVisit_&&!bank_->nativeFifoPins_&&
   !bank_->gbAnimationVisit_&&!bank_->gbQueryActive_;
 }
 bool contains(const void* address,std::size_t bytes)const noexcept {
  if(!bank_||!address||!bytes)return false;const auto first=reinterpret_cast<std::uintptr_t>(address);
  if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
  for(const auto& span:bank_->spans_){const auto base=reinterpret_cast<std::uintptr_t>(span.address);
   if(first>=base&&first-base<=span.bytes&&bytes<=span.bytes-(first-base))return true;}
  return false;
 }
 bool disjoint(const void* address,std::size_t bytes)const noexcept {
  if(!bank_||!address||!bytes)return false;const auto first=reinterpret_cast<std::uintptr_t>(address);
  if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
  for(const auto& span:bank_->spans_){const auto base=reinterpret_cast<std::uintptr_t>(span.address);
   if(first<base+span.bytes&&base<first+bytes)return false;}return true;
 }
 bool backing()const noexcept {
  const auto* a=actions();if(!a||!bank_||bank_->gbQueryIssuer_!=this||bank_->gbAnimationBacking_.get()!=world_.get()||
    a->queryBinding_!=this||!contains(main_,sizeof *main_)||!contains(secondary_,sizeof *secondary_)||
    !contains(node_,sizeof *node_)||!disjoint(a,sizeof *a)||!disjoint(this,sizeof *this)||
    !world_->storage_||!world_->storage_->get()||world_->candidates_.size()!=637)return false;
  for(std::size_t i=0;i<18;++i){const auto& c=world_->candidates_[615+i];
   if(c.nativeTriggersGB!=&a->group_||c.nativeTriggerIndex!=i||
     c.candidate.collision.filter.identity!=a->group_.triggers[i].collider.identity||
     a->group_.triggers[i].collider.identity!=gb_trigger_actions50GB::firstTriggerIdentity+i)return false;}
  return true;
 }
 bool cursorCurrent()const noexcept {
  const auto& a=*actions();const auto& c=a.group_.contact;using P=g::SourceTriggerContactPhase50GB;
  if(c.phase==P::Idle)return !c.group&&!c.hooks&&!c.querySerial&&!c.triggerCollider&&!c.otherCollider&&
   !c.nextAction&&!c.actionCount&&!c.dispatchCounted;
  if(!everIssued_||!c.querySerial||c.querySerial>serial_||c.querySerial!=a.group_.querySerial||
    c.group!=&a.group_||c.hooks!=&a.group_.actions||c.triggerIndex>=18||c.colliderIndex>=a.group_.colliders.size()||
    c.triggerCollider!=a.group_.triggers[c.triggerIndex].collider.identity||
    c.otherCollider!=a.group_.colliders[c.colliderIndex].identity||
    c.actionCount!=a.source_->source(c.triggerIndex).actionCount||c.candidateOrdinal>=637)return false;
  if(c.phase==P::Complete)return c.nextAction==c.actionCount&&c.dispatchCounted&&a.group_.lastContact.status==g::SourceTriggerContactStatus50GB::Complete;
  const bool absent=g::trigger_entry50IN_detail::find(a.group_.triggers[c.triggerIndex].collider,c.otherCollider)==a.group_.triggers[c.triggerIndex].collider.highWaterD0&&
   g::trigger_entry50IN_detail::find(a.group_.colliders[c.colliderIndex],c.triggerCollider)==a.group_.colliders[c.colliderIndex].highWaterD0;
  if(!a.group_.actionsFailed||!absent||c.dispatchCounted||c.querySerial!=serial_)return false;
  if(c.phase==P::Actions)return c.nextAction<c.actionCount&&
   (a.group_.lastContact.status==g::SourceTriggerContactStatus50GB::ActionBlocked||a.group_.lastContact.status==g::SourceTriggerContactStatus50GB::OwnerChanged);
  return c.phase==P::AddTriggerMember&&c.nextAction==c.actionCount&&a.group_.lastContact.status==g::SourceTriggerContactStatus50GB::OwnerChanged;
 }
 bool failQuery()noexcept {
  auto* a=actions();if(a)a->fail(gb_trigger_actions50GB::Status::Action);return false;
 }
public:
 Binding()=default;Binding(const Binding&)=delete;Binding&operator=(const Binding&)=delete;
 ~Binding(){if(!thread()||bank_||active_||world_)std::terminate();}
 static constexpr std::size_t requiredPayloadBytes()noexcept{return sizeof(Binding);}
 bool enroll(native_racer50HN::Bank& bank,std::shared_ptr<gb_collision_world50GB::Owner> world,
  std::shared_ptr<gb_navigation50GB::Owner> navigation,
  const std::uint32_t& mainIdentity,const std::uint32_t& secondaryIdentity,std::uint32_t& nodeC80,
  std::uint32_t playerPrefixCount34)noexcept {
  if(!thread()||bank_||world_||fenced_||bank.gbQueryIssuer_||!world||!world->geometryReady()||!world->actualIdle()||
    !navigation||!navigation->runtimeReady()||!navigation->map().sameWorld(world->storage_->get()->world))return false;
  bank_=&bank;world_=std::move(world);navigation_=std::move(navigation);main_=&mainIdentity;secondary_=&secondaryIdentity;node_=&nodeC80;
  const auto undo=[&](){bank_=nullptr;world_.reset();navigation_.reset();main_=secondary_=nullptr;node_=nullptr;return false;};
  auto& a=*actions();if(!idleBank()||bank.gbAnimationBlocked_||a.queryBinding_||a.identitiesBound_||
    !contains(main_,sizeof *main_)||!contains(secondary_,sizeof *secondary_)||!contains(node_,sizeof *node_)||
    !disjoint(&a,sizeof a)||!disjoint(this,sizeof *this)||bank.gbAnimationBacking_.get()!=world_.get())return undo();
  try{mainRead_=bank.bindRead(mainIdentity);secondaryRead_=bank.bindRead(secondaryIdentity);
   std::array<gb_trigger_actions50GB::RacerIdentities,8> identities{};
   for(std::size_t i=0;i<bank.count();++i)identities[i]={bank.read(i,mainRead_),bank.read(i,secondaryRead_)};
   if(!a.bindRacerIdentities(identities,playerPrefixCount34,bank.count()))return undo();
   a.queryBinding_=this;bank.gbQueryIssuer_=this;
   if(!backing()){a.queryBinding_=nullptr;bank.gbQueryIssuer_=nullptr;return undo();}
   return true;
  }catch(...){return undo();}
 }
 bool activateRenderer(std::shared_ptr<gb_particle_render50GB::Owner> renderer)noexcept {
  if(!thread()||fenced_||!backing()||!idleBank()||!world_->geometryReady()||!renderer||
    !renderer->ready()||!renderer->idle()||renderer->runtimeOwner()!=actions()->particles_.get()||actions()->renderer_)return false;
  auto& a=*actions();a.renderer_=std::move(renderer);a.group_.ready=true;
  if(!a.ready()){a.group_.ready=false;a.renderer_.reset();return false;}return true;
 }
 bool bind(native_racer50HN::Bank& bank,std::shared_ptr<gb_collision_world50GB::Owner> world,
  std::shared_ptr<gb_navigation50GB::Owner> navigation,std::shared_ptr<gb_particle_render50GB::Owner> renderer,
  const std::uint32_t& mainIdentity,const std::uint32_t& secondaryIdentity,std::uint32_t& nodeC80,
  std::uint32_t playerPrefixCount34)noexcept {
  return enroll(bank,std::move(world),std::move(navigation),mainIdentity,secondaryIdentity,nodeC80,playerPrefixCount34)&&activateRenderer(std::move(renderer));
 }
 bool enrolled()const noexcept{return thread()&&bank_&&world_;}
 bool ready()const noexcept{return thread()&&!fenced_&&backing()&&navigation_&&navigation_->runtimeReady()&&world_->queryReady();}
 bool commitListeners(const g::Vector3_48C* positions,std::size_t count)noexcept {
  if(!ready()||active_||!idleBank()||count>listeners_.size()||(count&&!positions))return false;
  for(std::size_t i=0;i<count;++i)for(unsigned k=0;k<3;++k)if(!std::isfinite(positions[i].v[k]))return false;
  for(std::size_t i=0;i<count;++i)listeners_[i]=positions[i];listenerCount_=count;return true;
 }
 // Must enclose the real indexed query, not merely its dispatch call. The
 // actual Bank visit is held by its caller and cannot restore during this loan.
 template<class Operation>bool query(std::size_t peer,bool secondary,Operation&& operation)noexcept {
  if(!ready()||active_||peer>=bank_->count_||bank_->gbQueryActive_||bank_->visitDepth_!=1||bank_->currentSlot_!=peer||
    bank_->pendingToken_||bank_->retirementVisit_||bank_->gbAnimationVisit_||bank_->gbAnimationBlocked_||
    serial_==std::numeric_limits<std::uint64_t>::max())return false;
  auto borrow=world_->query();if(!borrow)return false;auto* storage=borrow.storage();auto& a=*actions();
  const auto collider=secondary?*secondary_:*main_;
  if(!storage||*main_!=a.racers_[peer].main690||*secondary_!=a.racers_[peer].secondary||
    !g::releaseCompletedSourceContact50GB(a.group_.contact)||!a.beginRendererQuery())return failQuery();
  gb_trigger_actions50GB::ActionLease lease{*main_,collider,node_,listeners_.data(),listenerCount_};
  ++serial_;everIssued_=true;active_=bank_->gbQueryActive_=true;a.lease_=&lease;
  a.group_.querySerial=serial_;a.group_.queryCollider=collider;a.group_.queryActive=true;
  struct End {Binding& b;gb_trigger_actions50GB::Owner& a;
   ~End(){a.group_.queryActive=false;a.group_.queryCollider=0;a.lease_=nullptr;b.active_=false;b.bank_->gbQueryActive_=false;a.endRendererQuery();}
  }end{*this,a};
  try{const bool accepted=std::invoke(std::forward<Operation>(operation),*storage);
   const auto p=a.group_.contact.phase;
   if(!accepted||a.group_.actionsFailed||(p!=g::SourceTriggerContactPhase50GB::Idle&&p!=g::SourceTriggerContactPhase50GB::Complete))return failQuery();
   return a.ready();
  }catch(...){return failQuery();}
 }
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,const g::BodyMatrix48G& matrix)noexcept {
  if(!thread()||fenced_||!backing()||bank_->visitDepth_!=1||bank_->currentSlot_>=bank_->count_||bank_->pendingToken_||bank_->retirementVisit_)return false;
  const auto peer=bank_->currentSlot_;const auto& a=*actions();
  if(*main_!=a.racers_[peer].main690||*secondary_!=a.racers_[peer].secondary||
    (collider!=*main_&&collider!=*secondary_))return false;
  return actions()->observePrepared(collider,resource,matrix);
 }
 bool serviceWorld(std::uint64_t rootStep)noexcept {
  if(!ready()||active_||!idleBank()||!world_->actualIdle()||!rootStep||rootStep<=lastService_)return false;
  auto& a=*actions();if(!a.beginRendererService())return false;
  struct End{gb_trigger_actions50GB::Owner& a;~End(){a.endRendererService();}}end{a};
  lastService_=rootStep;return a.serviceWorld()==gb_trigger_actions50GB::Status::Complete;
 }
 bool fence()noexcept {
  if(!thread()||active_||!bank_||!world_||!idleBank()||!world_->actualIdle())return false;
  if(fenced_)return world_->phase_==gb_collision_world50GB::Phase::Retiring;
  if(!world_->beginRetirement())return false;
  fenced_=true;actions()->retiring_=true;actions()->group_.ready=false;terminal_.status=TerminalStatus::Fenced;terminal_.epoch=1;return true;
 }
 const TerminalResult& terminalResult()const noexcept{return terminal_;}
 // Root first ends camera/reset/FIFO/draw scopes, closes animation and renderer,
 // and retires every additional registry owner. This method checks the actual
 // objects and performs all remaining membership and reciprocal contact writes.
 bool retireContacts()noexcept {
  if(terminal_.status==TerminalStatus::Complete)return true;
  const auto reject=[&](TerminalStatus status){terminal_.status=status;return false;};
  if(!thread()||!fenced_||!backing()||world_->phase_!=gb_collision_world50GB::Phase::Retiring||!idleBank()||active_||!world_->actualIdle())return reject(TerminalStatus::Backing);
  auto& a=*actions();auto& s=*world_->storage_->get();
  if(!a.queryIdle()||(a.renderer_&&!a.renderer_->closed())||!a.animationRetired()||!a.particleIdle()||bank_->gbAnimationIssuer_||!navigation_||!navigation_->close())return reject(TerminalStatus::ExternalBorrower);
  if(world_->structuralBorrows_||world_->worldBorrows_.leases)return reject(TerminalStatus::ExternalBorrower);
  bank_->gbTerminalBlocked_=true;
  // Bounded internal all-peer retirement sweep, including stopped peers.
  // This is not a public callback bypass for normal Bank admission.
  try{bank_->gbTerminalVisit_=true;
   struct End {native_racer50HN::Bank& b;~End(){b.gbTerminalVisit_=false;}}end{*bank_};
   if(!bank_->visitAll([&](std::size_t peer){terminal_.owner=peer;
    if(s.queue.count||*main_!=a.racers_[peer].main690||*secondary_!=a.racers_[peer].secondary||
      a.group_.colliders[peer*2].identity!=*main_||a.group_.colliders[peer*2+1].identity!=*secondary_)return false;
    for(const auto& member:world_->animationMemberships_)if(member&&(member->state.attached||member->state.object))return false;
    const std::array<g::CollectionRef50BX,3> objects{s.count.mainQuery,s.count.secondaryQuery,s.count.terrainQuery};std::size_t n=0;
    for(auto* member:{&s.mainMembership,&s.secondaryMembership,&s.terrainMembership}){
     if(member->state.attached&&member->state.object!=objects[n])return false;++n;
     const auto r=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,member->state,s.membershipWork);
     if(r.status!=g::LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;}
    for(std::size_t i=0;i<18;++i){auto& member=world_->triggerMemberships_[i];if(!member)return false;
     if(member->state.attached&&member->state.object!=616+i)return false;
     const auto r=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,member->state,s.membershipWork);
     if(r.status!=g::LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;}
    for(std::size_t i=637;i<s.registrations.size();++i){const auto& r=s.registrations[i];if(r.owner||r.slot||r.encodedObject)return false;}
    terminal_.completedPeers|=static_cast<std::uint16_t>(1u<<peer);return true;
   }))return reject(TerminalStatus::Peer);
  }catch(...){return reject(TerminalStatus::Peer);}
  if(terminal_.completedPeers!=static_cast<std::uint16_t>((1u<<bank_->count_)-1u)||!idleBank())return reject(TerminalStatus::Peer);
  const auto validation=g::validateNativeTriggerTerminalGraph50GB(a.group_);
  terminal_.owner=validation.owner;terminal_.slot=validation.slot;
  if(!validation.valid)return reject(TerminalStatus::Contacts);
  if(!cursorCurrent())return reject(TerminalStatus::Cursor);
  terminal_.validatedPairs=validation.pairs;const auto& cursor=a.group_.contact;
  terminal_.phase=cursor.phase;terminal_.querySerial=cursor.querySerial;terminal_.candidateOrdinal=cursor.candidateOrdinal;
  terminal_.nextAction=cursor.nextAction;terminal_.actionCount=cursor.actionCount;terminal_.lastContact=a.group_.lastContact;
  // No calls, allocations, callbacks, RNG, exits or owner release between
  // these stores. All 34 owners and every reciprocal slot are already proved.
  for(auto& trigger:a.group_.triggers)for(auto& id:trigger.collider.membersCC)if(id){
   for(auto& collider:a.group_.colliders)if(collider.identity==id)
    for(auto& reciprocal:collider.membersCC)if(reciprocal==trigger.collider.identity){reciprocal=0;id=0;++terminal_.removedPairs;break;}
  }
  for(const auto& trigger:a.group_.triggers)for(auto id:trigger.collider.membersCC)if(id)std::terminate();
  for(const auto& collider:a.group_.colliders)for(auto id:collider.membersCC)if(id)std::terminate();
  if(terminal_.removedPairs!=terminal_.validatedPairs)std::terminate();terminal_.status=TerminalStatus::Complete;return true;
 }
 bool close()noexcept {
  if(!thread()||active_)return false;if(!bank_)return !world_;
  if(terminal_.status!=TerminalStatus::Complete||!idleBank())return false;
  auto& a=*actions();a.queryBinding_=nullptr;bank_->gbQueryIssuer_=nullptr;bank_=nullptr;
  navigation_.reset();world_.reset();main_=secondary_=nullptr;node_=nullptr;return true;
 }
};
} // namespace gb_query50GB

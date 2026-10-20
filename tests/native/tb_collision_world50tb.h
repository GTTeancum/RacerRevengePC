#pragma once
#include "tb_runtime_ground50la.h"
#include "tb_navigation50tb.h"
#include "tb_trigger_candidates50tb.h"
#include "track_static_candidates50ig_fixture.h"
#include <array>
#include <exception>
#include <memory>
#include <span>
#include <thread>

namespace tb_door_binding50TB { class Binding; }
namespace tb_collision_world50TB {
namespace g=rr::game;
namespace rt=track_runtime_storage50IG;
inline constexpr std::size_t sourceCandidates=320,triggerCandidates=16,doorCandidates=1,totalCandidates=337;
static_assert(triggerCandidates==tb_trigger_authoring50TB::count);
struct Payload {
 std::size_t sharedSource=0,externalReserved=0,ownerDescriptors=0,groundDescriptors=0;
 std::size_t candidateDescriptors=0,memberships=0,navigationReservation=0;
 std::size_t reservedBeforeStorage=0,storageRequested=0,totalRequested=0;
 // Separate original mesh/radii payload, outside Storage's descriptor budget.
 std::size_t independentMesh=0,independentRadii=0;
};
enum class Phase {Empty,Ground,Candidates,Storage,Memberships,DoorPending,Ready,Closed};
struct Diagnostic {
 Phase phase=Phase::Empty;std::size_t trigger=0;std::string message;
 rt::Diagnostic storage{};g::LiveZoneResult50EN membership{};Payload payload{};
};
// Single real collision-world authority. The final navigation lease retains
// this aggregate and its same cold source. This owner never retains navigation
// or a final view, so there is no ownership cycle.
class Owner final : public std::enable_shared_from_this<Owner> {
 friend class ::tb_door_binding50TB::Binding;
 std::shared_ptr<const tb_cold_sources50LA::Owner> source_;
 std::unique_ptr<tb_runtime_ground50LA::Owner> ground_;
 // Declaration order is deliberate: candidates borrow Action/Ground and
 // memberships borrow Storage. Explicit retirement removes real
 // memberships first; reverse destruction then retires Storage and backing.
 std::shared_ptr<const tb_trigger_authoring50TB::Owner> triggers_;
 std::shared_ptr<tb_trigger_actions50TB::Owner> actions_;
 std::shared_ptr<tb_door_runtime50TB::Owner> door_;
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<rt::Binding> bindings_;
 std::array<std::unique_ptr<rt::Membership>,triggerCandidates> memberships_;
 std::unique_ptr<rt::Membership> doorMembership_;
 std::unique_ptr<rt::Owner> storage_;
 std::size_t externalReserved_=0,preStorageReservation_=0,coldLimit_=0;
 const std::thread::id thread_=std::this_thread::get_id();
 bool groundAttempted_=false,storageAttempted_=false;
 Phase phase_=Phase::Empty;Payload payload_{};std::uint64_t generation_=0;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 static void add(std::size_t& n,std::size_t value){
  need(value<=std::numeric_limits<std::size_t>::max()-n,"TB aggregate payload addition overflow");n+=value;
 }
 template<class T> static void addElements(std::size_t& n,std::size_t count){
  need(count<=std::numeric_limits<std::size_t>::max()/sizeof(T),"TB aggregate payload multiplication overflow");add(n,count*sizeof(T));
 }
 bool sameOwners()const noexcept {
  return thread()&&source_&&source_->ready()&&ground_&&ground_->coldSource()==source_.get()&&
   actions_&&triggers_&&triggers_->ready()&&triggers_->coldSource()==source_.get()&&
   actions_->sourceOwner()==triggers_.get()&&door_&&actions_->doorOwner()==door_.get()&&door_->coldSource()==source_.get();
 }
 bool idleForRetirement()const noexcept {
  if(!thread()||(actions_&&!actions_->queryIdle()))return false;
  if(door_&&(door_->binding_||door_->servicing_))return false;
  if(doorMembership_&&(doorMembership_->state.attached||doorMembership_->state.object))return false;
  if(!storage_||!storage_->get())return true;const auto& s=*storage_->get();
  if(s.queue.count||s.mainMembership.state.attached||s.mainMembership.state.object||
     s.secondaryMembership.state.attached||s.secondaryMembership.state.object||
     s.terrainMembership.state.attached||s.terrainMembership.state.object)return false;
  // Root must retire its real racer/camera/recovery/wreck memberships first.
  // Their primary registration is in this same real lifecycle, not a copy.
  for(std::size_t i=totalCandidates;i<s.registrations.size();++i){const auto& r=s.registrations[i];
   if(r.owner||r.slot||r.encodedObject)return false;
  }
  return true;
 }
 bool removeTriggers()noexcept {
  if(!storage_||!storage_->get())return true;auto& s=*storage_->get();
  for(std::size_t i=0;i<triggerCandidates;++i)if(memberships_[i]){
   auto& state=memberships_[i]->state;
   const auto result=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,state,s.membershipWork);
   if(result.status!=g::LiveZoneStatus50EN::Complete||state.attached||state.object)return false;
  }
  return true;
 }
 bool retire()noexcept {
  if(!idleForRetirement()||!removeTriggers())return false;
  generation_=0;phase_=Phase::Closed;groundAttempted_=storageAttempted_=true;
  // No members/mesh/name/group backing may die before real Storage does.
  storage_.reset();for(auto& member:memberships_)member.reset();
  if(door_)door_->candidate_=nullptr;doorMembership_.reset();
  candidates_.clear();bindings_.clear();radii_.clear();
  actions_.reset();triggers_.reset();door_.reset();ground_.reset();source_.reset();return true;
 }
 void account(const rt::Limits& limits){
  Payload p{};p.sharedSource=source_->retainedPayloadBytes();p.externalReserved=externalReserved_;
  p.ownerDescriptors=sizeof(Owner)+sizeof(rt::Owner);
  p.groundDescriptors=ground_->descriptorPayloadBytes();
  addElements<g::Type2StaticCandidate49X>(p.candidateDescriptors,candidates_.capacity());
  addElements<rt::Binding>(p.candidateDescriptors,bindings_.capacity());
  addElements<track_static_candidates50IG::Radii>(p.candidateDescriptors,radii_.capacity());
  for(const auto& candidate:candidates_){const auto& filter=candidate.candidate.collision.filter;
   addElements<std::uint32_t>(p.candidateDescriptors,filter.categoryMasks.capacity());
   for(const auto& list:filter.identityLists)addElements<std::uint32_t>(p.candidateDescriptors,list.identities.capacity());
  }
  addElements<rt::Membership>(p.memberships,triggerCandidates+doorCandidates);
  addElements<g::CollectionZoneLink50BX>(p.memberships,12u*(triggerCandidates+doorCandidates));
  addElements<g::LiveZoneEntry50EN>(p.memberships,12u*(triggerCandidates+doorCandidates));
  p.navigationReservation=tb_navigation50TB::Owner::ownerBytes();
  add(p.navigationReservation,tb_navigation50TB::Owner::persistentPayloadBytes());
  add(p.navigationReservation,tb_navigation50TB::Owner::topologyScratchBytes());
  std::size_t observed=0;
  for(auto n:{p.sharedSource,p.externalReserved,p.ownerDescriptors,p.groundDescriptors,p.candidateDescriptors,p.memberships,p.navigationReservation})add(observed,n);
  need(observed<=preStorageReservation_&&limits.coldPayloadBytes==coldLimit_,"TB actual retained descriptors exceed reserved early envelope");
  p.reservedBeforeStorage=preStorageReservation_; // Keep conservative early peak charged.
  p.independentMesh=ground_->meshPayloadBytes();
  need(p.independentMesh!=std::numeric_limits<std::size_t>::max(),"TB mesh payload accounting overflow");
  for(const auto& radius:radii_)addElements<float>(p.independentRadii,radius.values.capacity());payload_=p;
 }
 bool doorAccess(const ::tb_door_binding50TB::Binding* binding)const noexcept {
  return thread()&&binding&&door_&&door_->binding_==binding&&actions_&&actions_->queryIdle()&&
   storage_&&storage_->get()&&doorMembership_&&(phase_==Phase::DoorPending||phase_==Phase::Ready);
 }
 bool attachDoor(const ::tb_door_binding50TB::Binding* binding,g::LiveZoneResult50EN& result)noexcept {
  if(!doorAccess(binding)||phase_!=Phase::DoorPending||door_->collisionState_.registrationE0)return false;
  auto& s=*storage_->get();auto& member=*doorMembership_;const auto bounds=door_->collisionBounds();
  result=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member.state,member.storage(),
   static_cast<g::CollectionRef50BX>(totalCandidates),4,g::ZoneMethod50CL::Second,0.f,bounds,s.membershipWork);
  door_->collisionState_.registrationE0=member.state.object;
  return result.status==g::LiveZoneStatus50EN::Complete&&member.state.attached&&member.state.object==totalCandidates;
 }
 bool updateDoor(const ::tb_door_binding50TB::Binding* binding,g::LiveZoneResult50EN& result)noexcept {
  if(!doorAccess(binding)||door_->collisionState_.registrationE0!=totalCandidates||
      !doorMembership_->state.attached||doorMembership_->state.object!=totalCandidates)return false;
  auto& s=*storage_->get();const auto bounds=door_->collisionBounds();
  result=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,doorMembership_->state,bounds,s.membershipWork);
  return result.status==g::LiveZoneStatus50EN::Complete;
 }
 bool commitDoor(const ::tb_door_binding50TB::Binding* binding)noexcept {
  if(!doorAccess(binding)||!sameOwners()||!door_->ready_||door_->failed_||
     !doorMembership_->state.attached||doorMembership_->state.object!=totalCandidates||door_->collisionState_.registrationE0!=totalCandidates)return false;
  generation_=1;phase_=Phase::Ready;return true;
 }
 bool removeDoor(const ::tb_door_binding50TB::Binding* binding,g::LiveZoneResult50EN& result)noexcept {
  if(!doorAccess(binding))return false;auto& s=*storage_->get();
  result=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,doorMembership_->state,s.membershipWork);
  if(result.status!=g::LiveZoneStatus50EN::Complete||doorMembership_->state.attached||doorMembership_->state.object)return false;
  door_->collisionState_.registrationE0=0;return true;
 }

public:
 // The only mutable projection is a retained typed borrow. It cannot resize
 // private candidate/radius/binding backing. Keep it through every actual
 // query, Bank visit/enrollment and callback retaining any returned pointer.
 class Borrow final {
  std::shared_ptr<Owner> owner_;
  explicit Borrow(std::shared_ptr<Owner> owner):owner_(std::move(owner)){}
  friend class Owner;
 public:
  Borrow()=default;Borrow(const Borrow&)=delete;Borrow&operator=(const Borrow&)=delete;
  Borrow(Borrow&&)=default;Borrow&operator=(Borrow&&)=delete;
  ~Borrow(){if(owner_&&!owner_->thread())std::terminate();}
  explicit operator bool()const noexcept{return owner_&&owner_->ready();}
  rt::Storage* storage()const noexcept{return *this?owner_->storage_->get():nullptr;}
  g::LiveZoneWorld50EN* world()const noexcept{auto* s=storage();return s?&s->world:nullptr;}
  std::span<g::Type2StaticCandidate49X> candidateStorage()const noexcept {
   return *this?std::span<g::Type2StaticCandidate49X>{owner_->candidates_}:std::span<g::Type2StaticCandidate49X>{};
  }
  std::span<const rt::Binding> bindingStorage()const noexcept {
   return *this?std::span<const rt::Binding>{owner_->bindings_}:std::span<const rt::Binding>{};
  }
  rt::Membership* triggerMembership(std::size_t i)const noexcept{return *this&&i<triggerCandidates?owner_->memberships_[i].get():nullptr;}
  rt::Membership* doorMembership()const noexcept{return *this?owner_->doorMembership_.get():nullptr;}
  tb_trigger_actions50TB::Owner* actions()const noexcept{return *this?owner_->actions_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(!thread()||!retire())std::terminate();}
 // Caller reserves source and all external door/trigger/light/Binding owners
 // before allocation. This returns the complete conservative pre-Storage sum;
 // externalReserved is included once and stays charged through final close.
 static std::size_t requiredPreStoragePayloadBytes(const tb_cold_sources50LA::Owner& source,
      std::size_t externalReserved)noexcept {
  try{std::size_t n=source.retainedPayloadBytes();add(n,externalReserved);
   add(n,sizeof(Owner)+sizeof(rt::Owner));add(n,tb_runtime_ground50LA::Owner::maximumDescriptorPayloadBytes(source));
   addElements<g::Type2StaticCandidate49X>(n,totalCandidates);addElements<rt::Binding>(n,totalCandidates);
   addElements<track_static_candidates50IG::Radii>(n,sourceCandidates);
   addElements<rt::Membership>(n,triggerCandidates+doorCandidates);
   addElements<g::CollectionZoneLink50BX>(n,12u*(triggerCandidates+doorCandidates));
   addElements<g::LiveZoneEntry50EN>(n,12u*(triggerCandidates+doorCandidates));
   add(n,tb_navigation50TB::Owner::ownerBytes());add(n,tb_navigation50TB::Owner::persistentPayloadBytes());
   add(n,tb_navigation50TB::Owner::topologyScratchBytes());return n;
  }catch(...){return std::numeric_limits<std::size_t>::max();}
 }
 bool prepareGround(std::shared_ptr<const tb_cold_sources50LA::Owner> source,
     rr::assets::TrackCollisionMasks50AN masks,const std::vector<std::uint8_t>& actualPhy,
     unsigned actualGridRank,std::size_t externallyReservedPayloadBytes,const rt::Limits& limits,std::string& error) {
  if(!thread()||groundAttempted_||weak_from_this().expired()){error="TB aggregate requires one shared owner-thread lifetime";return false;}
  groundAttempted_=true;phase_=Phase::Ground;
  try{
   need(source&&source->ready(),"TB aggregate actual shared cold source unavailable");
   const auto reserved=requiredPreStoragePayloadBytes(*source,externallyReservedPayloadBytes);
   need(limits.coldPayloadBytes&&limits.coldPayloadBytes<=32u*1024u*1024u&&reserved<limits.coldPayloadBytes,
    "TB entire early source/door/ground/candidate reservation exceeds unchanged budget");
   externalReserved_=externallyReservedPayloadBytes;preStorageReservation_=reserved;coldLimit_=limits.coldPayloadBytes;
   source_=std::move(source);ground_=std::make_unique<tb_runtime_ground50LA::Owner>();
   if(!ground_->prepare(source_,masks,actualPhy,actualGridRank,error))return false;
   need(ground_->descriptorPayloadBytes()<=tb_runtime_ground50LA::Owner::maximumDescriptorPayloadBytes(*source_),
    "TB actual Ground descriptors exceed early reservation");return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool prepareStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> actualApplication,
     std::shared_ptr<const tb_trigger_authoring50TB::Owner> triggers,
     std::shared_ptr<tb_trigger_actions50TB::Owner> actions,
     std::shared_ptr<tb_door_runtime50TB::Owner> door,
     const track_static_candidates50IG::Bindings& names,const rt::Limits& limits,Diagnostic& d)noexcept {
  if(!thread()||storageAttempted_||!ground_||!ground_->ground())return false;
  storageAttempted_=true;phase_=Phase::Candidates;
  try{
   d={};d.phase=phase_;
   need(actualApplication&&actualApplication->application10(),"TB aggregate requires actual live Application");
   need(triggers&&triggers->ready()&&triggers->coldSource()==source_.get(),
       "TB aggregate same authenticated trigger source required");
   need(actions&&door&&actions->sourceOwner()==triggers.get()&&actions->doorOwner()==door.get()&&
       actions->preparedSourceFor(*triggers)&&!actions->nativeGroup().ready&&actions->queryIdle()&&
       door->stagedSourceReady()&&door->collisionPrepared_&&door->binding_&&!door->ready_,
       "TB same staged Action/PodSound/door with actual visual prefix required");
   triggers_=std::move(triggers);actions_=std::move(actions);door_=std::move(door);
   need(sameOwners()&&ground_->ground()->owners.size()==sourceCandidates,"TB aggregate actual320 source prefix unavailable");
   candidates_.reserve(totalCandidates);bindings_.reserve(totalCandidates);radii_.reserve(sourceCandidates);
   track_static_candidates50IG::prepare(*ground_->ground(),names,candidates_,radii_,bindings_);
   need(tb_trigger_candidates50TB::prepare(*triggers_,*actions_,candidates_,bindings_,
       names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8)==sourceCandidates&&
       candidates_.size()==sourceCandidates+triggerCandidates&&bindings_.size()==candidates_.size(),
       "TB source320 plus trigger16 exact candidate join");
   candidates_.resize(totalCandidates);bindings_.resize(totalCandidates);
   for(std::size_t i=0;i<totalCandidates;++i)bindings_[i].candidate=&candidates_[i];
   need(door_->prepareCollisionCandidate(candidates_.back(),names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8),
       "TB sole actual root-bound door candidate preparation");
   bindings_.back()={std::numeric_limits<std::size_t>::max(),&candidates_.back(),true,4,0,g::ZoneMethod50CL::Second,false,door_.get()};
   account(limits);d.payload=payload_;
   auto reduced=limits;reduced.coldPayloadBytes-=payload_.reservedBeforeStorage;
   phase_=Phase::Storage;d.phase=phase_;storage_=std::make_unique<rt::Owner>();
   if(!storage_->prepare(std::move(actualApplication),*ground_->ground(),bindings_.data(),bindings_.size(),reduced,d.storage)){
    d.message=d.storage.message;return false;
   }
   auto& s=*storage_->get();
   need(s.count.terrain==totalCandidates&&s.count.zones==10&&s.count.portals==11&&s.count.portalLinks==22&&s.count.membership==12,
       "TB real Storage source topology/counts changed");
   payload_.storageRequested=d.storage.requestedPayloadBytes;payload_.totalRequested=payload_.reservedBeforeStorage;
   add(payload_.totalRequested,payload_.storageRequested);need(payload_.totalRequested<=limits.coldPayloadBytes,"TB aggregate final Storage budget exceeded");
   d.payload=payload_;phase_=Phase::Memberships;d.phase=phase_;
   for(auto& m:memberships_)m=std::make_unique<rt::Membership>(s.count.membership);
   for(std::size_t i=0;i<triggerCandidates;++i){d.trigger=i;const auto& source=triggers_->source(i);
    auto& member=*memberships_[i];const auto bounds=tb_trigger_candidates50TB::bounds(source);
    d.membership=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member.state,member.storage(),
        static_cast<g::CollectionRef50BX>(sourceCandidates+i+1u),source.zone,g::ZoneMethod50CL::First,0.f,bounds,s.membershipWork);
    if(d.membership.status!=g::LiveZoneStatus50EN::Complete){d.message="TB actual trigger membership attach rejected";return false;}
    d.membership=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,member.state,bounds,s.membershipWork);
    if(d.membership.status!=g::LiveZoneStatus50EN::Complete){d.message="TB actual trigger membership update rejected";return false;}
   }
   doorMembership_=std::make_unique<rt::Membership>(s.count.membership);
   phase_=Phase::DoorPending;d.phase=phase_;d.message.clear();return true;
  }catch(const std::exception& e){try{d.phase=phase_;d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.phase=phase_;d.message="TB aggregate allocation or cold assembly failure";}catch(...){}return false;}
 }
 bool ready()const noexcept{return phase_==Phase::Ready&&generation_&&sameOwners()&&storage_&&storage_->get();}
 bool doorPending()const noexcept{return phase_==Phase::DoorPending&&sameOwners()&&door_&&door_->collisionPrepared_;}
 Phase phase()const noexcept{return phase_;}
 const Payload& payload()const noexcept{return payload_;}
 const authored_runtime_ground50cp_fixture::ColdScene* ground()const noexcept{return thread()&&ground_?ground_->ground():nullptr;}
 const tb_cold_sources50LA::Owner* coldSource()const noexcept{return thread()?source_.get():nullptr;}
 bool bindHumanLift(float actualSelectedLift)noexcept{return thread()&&source_&&ground_&&ground_->bindHumanLift(*source_,actualSelectedLift);}
 Borrow borrow()noexcept {if(!ready())return {};auto self=weak_from_this().lock();return self?Borrow{std::move(self)}:Borrow{};}
 g::TbCollisionWorldLease lease()const noexcept {
  if(!ready())return {};auto self=weak_from_this().lock();if(!self)return {};
  return g::TbCollisionWorldLease{std::move(self),source_,storage_->get()->world,generation_};
 }
 bool close()noexcept {
  if(!thread()||weak_from_this().use_count()>1)return false;
  if(phase_==Phase::Closed)return true;
  return retire();
 }
};
} // namespace tb_collision_world50TB

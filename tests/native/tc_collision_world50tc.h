#pragma once
#include "tc_runtime_ground50la.h"
#include "tc_navigation50tc.h"
#include "tc_trigger_candidates50tc.h"
#include "track_static_candidates50ig_fixture.h"
#include <array>
#include <exception>
#include <memory>
#include <span>
#include <thread>

namespace tc_collision_world50TC {
namespace g=rr::game;
namespace rt=track_runtime_storage50IG;
inline constexpr std::size_t sourceCandidates=69,triggerCandidates=22,totalCandidates=91;
static_assert(triggerCandidates==tc_trigger_authoring50TC::count);
struct Payload {
 std::size_t sharedSource=0,ownerDescriptors=0,groundDescriptors=0;
 std::size_t candidateDescriptors=0,memberships=0,navigationReservation=0;
 std::size_t reservedBeforeStorage=0,storageRequested=0,totalRequested=0;
 // Separate original mesh/radii payload, outside Storage's descriptor budget.
 std::size_t independentMesh=0,independentRadii=0;
};
enum class Phase {Empty,Ground,Candidates,Storage,Memberships,Ready,Closed};
struct Diagnostic {
 Phase phase=Phase::Empty;std::size_t trigger=0;std::string message;
 rt::Diagnostic storage{};g::LiveZoneResult50EN membership{};Payload payload{};
};
// Single real collision-world authority. The final navigation lease retains
// this aggregate and its same cold source. This owner never retains navigation
// or a final view, so there is no ownership cycle.
class Owner final : public std::enable_shared_from_this<Owner> {
 std::shared_ptr<const tc_cold_sources50LA::Owner> source_;
 std::unique_ptr<tc_runtime_ground50LA::Owner> ground_;
 // Declaration order is deliberate: Action also retains Sound, candidates borrow
 // Action/Ground, memberships borrow Storage. Explicit retirement removes real
 // memberships first; reverse destruction then retires Storage and backing.
 std::shared_ptr<tc_global_sound50TC::Owner> sounds_;
 std::shared_ptr<tc_trigger_actions50TC::Owner> actions_;
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<rt::Binding> bindings_;
 std::array<std::unique_ptr<rt::Membership>,triggerCandidates> memberships_;
 std::unique_ptr<rt::Owner> storage_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool groundAttempted_=false,storageAttempted_=false;
 Phase phase_=Phase::Empty;Payload payload_{};std::uint64_t generation_=0;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 static void add(std::size_t& n,std::size_t value){
  need(value<=std::numeric_limits<std::size_t>::max()-n,"TC aggregate payload addition overflow");n+=value;
 }
 template<class T> static void addElements(std::size_t& n,std::size_t count){
  need(count<=std::numeric_limits<std::size_t>::max()/sizeof(T),"TC aggregate payload multiplication overflow");add(n,count*sizeof(T));
 }
 bool sameOwners()const noexcept {
  return thread()&&source_&&source_->ready()&&ground_&&ground_->coldSource()==source_.get()&&
   actions_&&sounds_&&actions_->coldSource()==source_.get()&&actions_->soundOwner()==sounds_.get()&&
   actions_->sourceOwner()&&actions_->sourceOwner()->coldSource()==source_.get();
 }
 bool idleForRetirement()const noexcept {
  if(!thread()||(actions_&&actions_->nativeGroup().queryActive))return false;
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
  candidates_.clear();bindings_.clear();radii_.clear();
  actions_.reset();sounds_.reset();ground_.reset();source_.reset();return true;
 }
 void account(const rt::Limits& limits){
  Payload p{};p.sharedSource=source_->retainedPayloadBytes();
  p.ownerDescriptors=sizeof(Owner)+sizeof(rt::Owner);
  p.groundDescriptors=ground_->descriptorPayloadBytes();
  addElements<g::Type2StaticCandidate49X>(p.candidateDescriptors,candidates_.capacity());
  addElements<rt::Binding>(p.candidateDescriptors,bindings_.capacity());
  addElements<track_static_candidates50IG::Radii>(p.candidateDescriptors,radii_.capacity());
  for(const auto& candidate:candidates_){const auto& filter=candidate.candidate.collision.filter;
   addElements<std::uint32_t>(p.candidateDescriptors,filter.categoryMasks.capacity());
   for(const auto& list:filter.identityLists)addElements<std::uint32_t>(p.candidateDescriptors,list.identities.capacity());
  }
  const auto memberCount=source_->world().portals.size()+1u;
  addElements<rt::Membership>(p.memberships,triggerCandidates);
  addElements<g::CollectionZoneLink50BX>(p.memberships,memberCount*triggerCandidates);
  addElements<g::LiveZoneEntry50EN>(p.memberships,memberCount*triggerCandidates);
  p.navigationReservation=tc_navigation50TC::Owner::ownerBytes();
  add(p.navigationReservation,tc_navigation50TC::Owner::persistentPayloadBytes());
  add(p.navigationReservation,tc_navigation50TC::Owner::topologyScratchBytes());
  for(auto n:{p.sharedSource,p.ownerDescriptors,p.groundDescriptors,p.candidateDescriptors,p.memberships,p.navigationReservation})
   add(p.reservedBeforeStorage,n);
  need(limits.coldPayloadBytes&&limits.coldPayloadBytes<=32u*1024u*1024u&&
       p.reservedBeforeStorage<limits.coldPayloadBytes,"TC aggregate/source reservation exceeds existing Storage budget");
  p.independentMesh=ground_->meshPayloadBytes();
  need(p.independentMesh!=std::numeric_limits<std::size_t>::max(),"TC mesh payload accounting overflow");
  for(const auto& radius:radii_)addElements<float>(p.independentRadii,radius.values.capacity());
  payload_=p;
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
  tc_trigger_actions50TC::Owner* actions()const noexcept{return *this?owner_->actions_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(!thread()||!retire())std::terminate();}
 bool prepareGround(std::shared_ptr<const tc_cold_sources50LA::Owner> source,
     rr::assets::TrackCollisionMasks50AN masks,const std::vector<std::uint8_t>& actualPhy,
     unsigned actualGridRank,std::string& error) {
  if(!thread()||groundAttempted_||weak_from_this().expired()){error="TC aggregate requires one shared owner-thread lifetime";return false;}
  groundAttempted_=true;phase_=Phase::Ground;
  try{
   need(source&&source->ready(),"TC aggregate actual shared cold source unavailable");
   source_=std::move(source);ground_=std::make_unique<tc_runtime_ground50LA::Owner>();
   return ground_->prepare(source_,masks,actualPhy,actualGridRank,error);
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool prepareStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> actualApplication,
     std::shared_ptr<tc_trigger_actions50TC::Owner> actions,
     std::shared_ptr<tc_global_sound50TC::Owner> sounds,
     const track_static_candidates50IG::Bindings& names,const rt::Limits& limits,Diagnostic& d)noexcept {
  if(!thread()||storageAttempted_||!ground_||!ground_->ground())return false;
  storageAttempted_=true;phase_=Phase::Candidates;
  try{
   d={};d.phase=phase_;
   need(actualApplication&&actualApplication->application10(),"TC aggregate requires actual live Application");
   need(actions&&sounds&&actions->sourceOwner()&&actions->coldSource()==source_.get()&&
       actions->soundOwner()==sounds.get()&&actions->preparedSourceFor(*actions->sourceOwner())&&
       !actions->nativeGroup().ready&&!actions->nativeGroup().queryActive,
       "TC aggregate same prepared cold source/Action/Sound required before racer identity binding");
   actions_=std::move(actions);sounds_=std::move(sounds);
   need(sameOwners()&&ground_->ground()->owners.size()==sourceCandidates,"TC aggregate actual69 source prefix unavailable");
   candidates_.reserve(totalCandidates);bindings_.reserve(totalCandidates);radii_.reserve(sourceCandidates);
   track_static_candidates50IG::prepare(*ground_->ground(),names,candidates_,radii_,bindings_);
   need(tc_trigger_candidates50TC::prepare(*actions_->sourceOwner(),*actions_,candidates_,bindings_,
       names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8)==sourceCandidates&&
       candidates_.size()==totalCandidates&&bindings_.size()==totalCandidates,
       "TC source69 plus trigger22 exact candidate join");
   account(limits);d.payload=payload_;
   auto reduced=limits;reduced.coldPayloadBytes-=payload_.reservedBeforeStorage;
   phase_=Phase::Storage;d.phase=phase_;storage_=std::make_unique<rt::Owner>();
   if(!storage_->prepare(std::move(actualApplication),*ground_->ground(),bindings_.data(),bindings_.size(),reduced,d.storage)){
    d.message=d.storage.message;return false;
   }
   auto& s=*storage_->get();
   need(s.count.terrain==totalCandidates&&s.count.zones==25&&s.count.portals==26&&s.count.portalLinks==52&&s.count.membership==27,
       "TC real Storage source topology/counts changed");
   payload_.storageRequested=d.storage.requestedPayloadBytes;payload_.totalRequested=payload_.reservedBeforeStorage;
   add(payload_.totalRequested,payload_.storageRequested);need(payload_.totalRequested<=limits.coldPayloadBytes,"TC aggregate final Storage budget exceeded");
   d.payload=payload_;phase_=Phase::Memberships;d.phase=phase_;
   for(auto& m:memberships_)m=std::make_unique<rt::Membership>(s.count.membership);
   for(std::size_t i=0;i<triggerCandidates;++i){d.trigger=i;const auto& source=actions_->sourceOwner()->source(i);
    auto& member=*memberships_[i];const auto bounds=tc_trigger_candidates50TC::bounds(source);
    d.membership=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member.state,member.storage(),
        static_cast<g::CollectionRef50BX>(sourceCandidates+i+1u),source.zone,g::ZoneMethod50CL::First,0.f,bounds,s.membershipWork);
    if(d.membership.status!=g::LiveZoneStatus50EN::Complete){d.message="TC actual trigger membership attach rejected";return false;}
    d.membership=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,member.state,bounds,s.membershipWork);
    if(d.membership.status!=g::LiveZoneStatus50EN::Complete){d.message="TC actual trigger membership update rejected";return false;}
   }
   generation_=1;phase_=Phase::Ready;d.phase=phase_;d.message.clear();return true;
  }catch(const std::exception& e){try{d.phase=phase_;d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.phase=phase_;d.message="TC aggregate allocation or cold assembly failure";}catch(...){}return false;}
 }
 bool ready()const noexcept{return phase_==Phase::Ready&&generation_&&sameOwners()&&storage_&&storage_->get();}
 Phase phase()const noexcept{return phase_;}
 const Payload& payload()const noexcept{return payload_;}
 const authored_runtime_ground50cp_fixture::ColdScene* ground()const noexcept{return thread()&&ground_?ground_->ground():nullptr;}
 const tc_cold_sources50LA::Owner* coldSource()const noexcept{return thread()?source_.get():nullptr;}
 bool bindHumanLift(float actualSelectedLift)noexcept{return thread()&&source_&&ground_&&ground_->bindHumanLift(*source_,actualSelectedLift);}
 Borrow borrow()noexcept {if(!ready())return {};auto self=weak_from_this().lock();return self?Borrow{std::move(self)}:Borrow{};}
 g::TcCollisionWorldLease lease()const noexcept {
  if(!ready())return {};auto self=weak_from_this().lock();if(!self)return {};
  return g::TcCollisionWorldLease{std::move(self),source_,storage_->get()->world,generation_};
 }
 bool close()noexcept {
  if(!thread()||weak_from_this().use_count()>1)return false;
  if(phase_==Phase::Closed)return true;
  return retire();
 }
};
} // namespace tc_collision_world50TC

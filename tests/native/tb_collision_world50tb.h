#pragma once
#include "tb_runtime_ground50la.h"
#include "tb_navigation50tb.h"
#include "tb_trigger_candidates50tb.h"
#include "track_static_candidates50ig_fixture.h"
#include "track_animation_world50an.h"
#include <array>
#include <exception>
#include <memory>
#include <span>
#include <thread>

namespace tb_collision_world50TB {
namespace g=rr::game;
namespace rt=track_runtime_storage50IG;
// Trigger candidates: the live authored count (capacity store, triggerCapacity
// from the common reader); 0 authored = no trigger candidates/memberships.
// Animation candidates: the track-animation rows with UseCollision (common player,
// track_animation_world50an.h; stock 1, the door frame). The ground prefix keeps
// its existing shape (not this round).
inline constexpr std::size_t sourceCandidates=320,triggerCapacity=tb_trigger_authoring50TB::capacity,
 animationCapacity=rr::assets::TrackAnimationLimits50AN{}.animations;
struct Payload {
 std::size_t sharedSource=0,externalReserved=0,ownerDescriptors=0,groundDescriptors=0;
 std::size_t candidateDescriptors=0,memberships=0,navigationReservation=0;
 std::size_t reservedBeforeStorage=0,storageRequested=0,totalRequested=0;
 // Separate original mesh/radii payload, outside Storage's descriptor budget.
 std::size_t independentMesh=0,independentRadii=0;
};
enum class Phase {Empty,Ground,Candidates,Storage,Memberships,AnimationPending,Ready,Closed};
struct Diagnostic {
 Phase phase=Phase::Empty;std::size_t trigger=0;std::string message;
 rt::Diagnostic storage{};g::LiveZoneResult50EN membership{};Payload payload{};
};
// Single real collision-world authority. The final navigation lease retains
// this aggregate and its same cold source. This owner never retains navigation
// or a final view, so there is no ownership cycle.
class Owner final : public std::enable_shared_from_this<Owner>,public track_animation_world50AN::World {
 std::shared_ptr<const tb_cold_sources50LA::Owner> source_;
 std::unique_ptr<tb_runtime_ground50LA::Owner> ground_;
 // Declaration order is deliberate: candidates borrow Action/Ground and
 // memberships borrow Storage. Explicit retirement removes real
 // memberships first; reverse destruction then retires Storage and backing.
 std::shared_ptr<const tb_trigger_authoring50TB::Owner> triggers_;
 std::shared_ptr<tb_trigger_actions50TB::Owner> actions_;
 std::shared_ptr<track_animation_runtime50AN::Owner> animations_;
 track_animation_world50AN::Lane lane_;
 std::size_t animationCount_=0,plannedAnimations_=0; // live UseCollision rows; the count the reservation was charged for
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<rt::Binding> bindings_;
 std::array<std::unique_ptr<rt::Membership>,triggerCapacity> memberships_;
 std::size_t triggerCount_=0; // live authored triggers
 std::size_t totalCandidates()const noexcept{return sourceCandidates+triggerCount_+animationCount_;}
 std::unique_ptr<rt::Owner> storage_;
 std::size_t externalReserved_=0,preStorageReservation_=0,coldLimit_=0;
 bool publicationBlocked_=false;
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
   actions_->sourceOwner()==triggers_.get()&&(animations_||!animationCount_)&&actions_->doorOwner()==animations_.get();
 }
 bool idleForRetirement()const noexcept {
  if(!thread()||(actions_&&!actions_->queryIdle()))return false;
  if(!lane_.detached())return false;
  if(!storage_||!storage_->get())return true;const auto& s=*storage_->get();
  if(s.queue.count||!s.rowsDetached())return false; //every racer's query rows (shared world: all of them)
  // Root must retire its real racer/camera/recovery/wreck memberships first.
  // Their primary registration is in this same real lifecycle, not a copy.
  for(std::size_t i=totalCandidates();i<s.registrations.size();++i){const auto& r=s.registrations[i];
   if(r.owner||r.slot||r.encodedObject)return false;
  }
  return true;
 }
 bool removeTriggers()noexcept {
  if(!storage_||!storage_->get())return true;auto& s=*storage_->get();
  for(std::size_t i=0;i<triggerCount_;++i)if(memberships_[i]){
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
  lane_.reset();
  candidates_.clear();bindings_.clear();radii_.clear();
  actions_.reset();triggers_.reset();animations_.reset();ground_.reset();source_.reset();return true;
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
  addElements<rt::Membership>(p.memberships,triggerCount_);
  addElements<g::CollectionZoneLink50BX>(p.memberships,12u*triggerCount_);
  addElements<g::LiveZoneEntry50EN>(p.memberships,12u*triggerCount_);
  add(p.memberships,track_animation_world50AN::Lane::payloadBytes(animationCount_,12u));
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
 bool actualIdle()const noexcept{return thread()&&(!actions_||actions_->queryIdle())&&(!storage_||!storage_->get()||!storage_->get()->queue.count);}
 bool currentPublication()const noexcept{return storage_&&storage_->get()&&std::fegetround()==FE_TONEAREST&&lane_.current(*storage_->get());}

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
  rt::Membership* triggerMembership(std::size_t i)const noexcept{return *this&&i<owner_->triggerCount_?owner_->memberships_[i].get():nullptr;}
  std::size_t triggerCount()const noexcept{return *this?owner_->triggerCount_:0;}
  tb_trigger_actions50TB::Owner* actions()const noexcept{return *this?owner_->actions_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner();
 // Caller reserves source and all external door/trigger/light/Binding owners
 // before allocation. This returns the complete conservative pre-Storage sum;
 // externalReserved is included once and stays charged through final close.
 // animations: the track-animation rows with UseCollision the data authors, charged exactly.
 static std::size_t requiredPreStoragePayloadBytes(const tb_cold_sources50LA::Owner& source,
      std::size_t externalReserved,std::size_t animations)noexcept {
  try{need(animations<=animationCapacity,"TB animation collision rows exceed the reader's limit");
   const auto maximumCandidates=sourceCandidates+triggerCapacity+animations;
   std::size_t n=source.retainedPayloadBytes();add(n,externalReserved);
   add(n,sizeof(Owner)+sizeof(rt::Owner));add(n,tb_runtime_ground50LA::Owner::maximumDescriptorPayloadBytes(source));
   addElements<g::Type2StaticCandidate49X>(n,maximumCandidates);addElements<rt::Binding>(n,maximumCandidates);
   addElements<track_static_candidates50IG::Radii>(n,sourceCandidates);
   addElements<rt::Membership>(n,triggerCapacity);
   addElements<g::CollectionZoneLink50BX>(n,12u*triggerCapacity);
   addElements<g::LiveZoneEntry50EN>(n,12u*triggerCapacity);add(n,track_animation_world50AN::Lane::payloadBytes(animations,12u));
   add(n,tb_navigation50TB::Owner::ownerBytes());add(n,tb_navigation50TB::Owner::persistentPayloadBytes());
   add(n,tb_navigation50TB::Owner::topologyScratchBytes());return n;
  }catch(...){return std::numeric_limits<std::size_t>::max();}
 }
 bool prepareGround(std::shared_ptr<const tb_cold_sources50LA::Owner> source,
     rr::assets::TrackCollisionMasks50AN masks,const std::vector<std::uint8_t>& actualPhy,
     unsigned actualGridRank,std::size_t externallyReservedPayloadBytes,const rt::Limits& limits,std::size_t animations,std::string& error) {
  if(!thread()||groundAttempted_||weak_from_this().expired()){error="TB aggregate requires one shared owner-thread lifetime";return false;}
  groundAttempted_=true;phase_=Phase::Ground;
  try{
   need(source&&source->ready(),"TB aggregate actual shared cold source unavailable");
   const auto reserved=requiredPreStoragePayloadBytes(*source,externallyReservedPayloadBytes,animations);plannedAnimations_=animations;
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
     std::shared_ptr<track_animation_runtime50AN::Owner> animations,
     const track_static_candidates50IG::Bindings& names,const rt::Limits& limits,Diagnostic& d)noexcept {
  if(!thread()||storageAttempted_||!ground_||!ground_->ground())return false;
  storageAttempted_=true;phase_=Phase::Candidates;
  try{
   d={};d.phase=phase_;
   need(actualApplication&&actualApplication->application10(),"TB aggregate requires actual live Application");
   need(triggers&&triggers->ready()&&triggers->coldSource()==source_.get(),
       "TB aggregate same authenticated trigger source required");
   // The common track-animation runtime exists exactly when the TOB authors [Animations].
   need(actions&&actions->sourceOwner()==triggers.get()&&actions->doorOwner()==animations.get()&&
       actions->preparedSourceFor(*triggers)&&!actions->nativeGroup().ready&&actions->queryIdle()&&(!animations||animations->ready()),
       "TB same staged Action/PodSound/animation join required");
   animationCount_=track_animation_world50AN::Lane::collisionRowCount(animations.get());
   need(animationCount_==plannedAnimations_,"TB animation collision rows differ from the charged reservation");
   triggers_=std::move(triggers);actions_=std::move(actions);animations_=std::move(animations);
   triggerCount_=triggers_->liveCount();need(triggerCount_<=triggerCapacity,"TB live triggers exceed the trigger capacity");
   need(sameOwners()&&ground_->ground()->owners.size()==sourceCandidates,"TB aggregate actual320 source prefix unavailable");
   candidates_.reserve(totalCandidates());bindings_.reserve(totalCandidates());radii_.reserve(sourceCandidates);
   track_static_candidates50IG::prepare(*ground_->ground(),names,candidates_,radii_,bindings_);
   need(tb_trigger_candidates50TB::prepare(*triggers_,*actions_,candidates_,bindings_,
       names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8)==sourceCandidates&&
       candidates_.size()==sourceCandidates+triggerCount_&&bindings_.size()==candidates_.size(),
       "TB source prefix plus live trigger candidate join");
   need(lane_.prepareCandidates(animations_,candidates_,bindings_,names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8)==sourceCandidates+triggerCount_&&
       candidates_.size()==totalCandidates()&&bindings_.size()==totalCandidates()&&lane_.count()==animationCount_,"TB track-animation candidate lane rejected");
   account(limits);d.payload=payload_;
   auto reduced=limits;reduced.coldPayloadBytes-=payload_.reservedBeforeStorage;
   phase_=Phase::Storage;d.phase=phase_;storage_=std::make_unique<rt::Owner>();
   if(!storage_->prepare(std::move(actualApplication),*ground_->ground(),bindings_.data(),bindings_.size(),reduced,d.storage)){
    d.message=d.storage.message;return false;
   }
   auto& s=*storage_->get();
   need(s.count.terrain==totalCandidates()&&s.count.zones==10&&s.count.portals==11&&s.count.portalLinks==22&&s.count.membership==12,
       "TB real Storage source topology/counts changed");
   payload_.storageRequested=d.storage.requestedPayloadBytes;payload_.totalRequested=payload_.reservedBeforeStorage;
   add(payload_.totalRequested,payload_.storageRequested);need(payload_.totalRequested<=limits.coldPayloadBytes,"TB aggregate final Storage budget exceeded");
   d.payload=payload_;phase_=Phase::Memberships;d.phase=phase_;
   for(std::size_t i=0;i<triggerCount_;++i)memberships_[i]=std::make_unique<rt::Membership>(s.count.membership);
   for(std::size_t i=0;i<triggerCount_;++i){d.trigger=i;const auto& source=triggers_->source(i);
    auto& member=*memberships_[i];const auto bounds=tb_trigger_candidates50TB::bounds(source);
    d.membership=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member.state,member.storage(),
        static_cast<g::CollectionRef50BX>(sourceCandidates+i+1u),source.zone,g::ZoneMethod50CL::First,0.f,bounds,s.membershipWork);
    if(d.membership.status!=g::LiveZoneStatus50EN::Complete){d.message="TB actual trigger membership attach rejected";return false;}
    d.membership=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,member.state,bounds,s.membershipWork);
    if(d.membership.status!=g::LiveZoneStatus50EN::Complete){d.message="TB actual trigger membership update rejected";return false;}
   }
   lane_.makeMemberships(s.count.membership);
   std::cerr<<"tb_course_owner owner=collision_world placed_col="<<sourceCandidates<<" triggers="<<triggerCount_<<" animations="<<animationCount_
       <<" candidates="<<totalCandidates()<<'\n';
   // The track-animation binding publishes the lane next; no [Animations] = complete here.
   if(animations_){phase_=Phase::AnimationPending;d.phase=phase_;d.message.clear();return true;}
   need(currentPublication(),"TB aggregate publication without animations rejected");
   generation_=1;phase_=Phase::Ready;d.phase=phase_;d.message.clear();return true;
  }catch(const std::exception& e){try{d.phase=phase_;d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.phase=phase_;d.message="TB aggregate allocation or cold assembly failure";}catch(...){}return false;}
 }
 // Structural readiness (the retained Borrow, navigation lease and serial retirement read it).
 // The animation binding's publication fence is NOT part of it: the binding leaves the
 // fence up after its close, and TB's retirement still needs the Borrow then.
 bool ready()const noexcept{return phase_==Phase::Ready&&generation_&&sameOwners()&&storage_&&storage_->get();}
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
 // ---- track_animation_world50AN::World (the common binding's view of the lane) ----
 track_animation_world50AN::Lane* animationLane()noexcept override{return thread()&&animations_?&lane_:nullptr;}
 rt::Storage* animationStorage()noexcept override{return thread()&&storage_?storage_->get():nullptr;}
 bool animationIdle()const noexcept override{return actualIdle();}
 bool animationPending()const noexcept override{return phase_==Phase::AnimationPending&&sameOwners()&&storage_&&storage_->get();}
 bool commitAnimationLane()noexcept override{
  if(phase_!=Phase::AnimationPending||!actualIdle()||!sameOwners()||!currentPublication())return false;
  generation_=1;phase_=Phase::Ready;publicationBlocked_=false;return true;}
 void blockAnimationPublication(bool blocked)noexcept override{if(phase_==Phase::Ready||phase_==Phase::AnimationPending)publicationBlocked_=blocked;}
};
} // namespace tb_collision_world50TB

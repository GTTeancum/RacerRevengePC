#pragma once
// Complete source-owned GB world. Live admission is provided only by the
// same-root renderer, Bank, navigation and action binding.
#include "gb_runtime_ground50la.h"
#include "gb_navigation50gb.h"
#include "track_animation_world50an.h"
#include "gb_trigger_candidates50gb.h"
#include "track_static_candidates50ig_fixture.h"
#include <array>
#include <memory>
#include <span>
#include <thread>

namespace gb_query50GB { class Binding; }
namespace gb_collision_world50GB {
namespace g=rr::game;namespace a=rr::assets;namespace rt=track_runtime_storage50IG;
// Capacity store with live counts (common course rule). The ground, trigger
// and animation extents come from the data: placed COL owners (stock 615), the
// TOB's live triggers (stock 18, capacity 22) and the track-animation rows with
// UseCollision (common player, track_animation_world50an.h; stock 4, absent = 0).
// Stock 615 + 18 + 4 = 637 is a receipt.
inline constexpr std::size_t triggerCapacity=gb_trigger_authoring50GB::capacity;
inline constexpr std::size_t animationCapacity=rr::assets::TrackAnimationLimits50AN{}.animations;
inline constexpr std::size_t stockSourceCandidates=615,stockTriggerCandidates=18,stockTotalCandidates=637;
struct Payload {
 std::size_t sharedSource=0,externalReserved=0,ownerDescriptors=0,groundDescriptors=0;
 std::size_t candidateDescriptors=0,memberships=0,navigationReservation=0;
 std::size_t reservedBeforeStorage=0,storageRequested=0,totalRequested=0,independentMesh=0,independentRadii=0;
};
enum class Phase {Empty,Ground,Candidates,Storage,Memberships,AnimationPending,Ready,Retiring,Closed};
struct Diagnostic {Phase phase=Phase::Empty;std::size_t trigger=0;std::string message;rt::Diagnostic storage{};g::LiveZoneResult50EN membership{};Payload payload{};};
// Complete637-owner aggregate only. A615+4 prefix cannot reach Storage/Ready
// or mint navigation authority. Actual trigger source/actions are mandatory.
class Owner final : public std::enable_shared_from_this<Owner>,public track_animation_world50AN::World {
 friend class ::gb_query50GB::Binding;
 std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
 std::unique_ptr<gb_runtime_ground50LA::Owner> ground_;
 std::shared_ptr<const gb_trigger_authoring50GB::Owner> triggers_;
 std::shared_ptr<gb_trigger_actions50GB::Owner> actions_;
 std::shared_ptr<track_animation_runtime50AN::Owner> animations_;
 track_animation_world50AN::Lane lane_;
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<rt::Binding> bindings_;
 std::array<std::unique_ptr<rt::Membership>,triggerCapacity> triggerMemberships_;
 std::unique_ptr<rt::Owner> storage_;
 std::size_t externalReserved_=0,preStorageReservation_=0,coldLimit_=0,queryBorrows_=0,structuralBorrows_=0;
 // Live extents (set at Storage) and the counts the pre-Storage reservation was charged for.
 std::size_t sourceCount_=0,triggerCount_=0,animationCount_=0,membershipCapacity_=0;
 std::size_t plannedTriggers_=0,plannedAnimations_=0;
 mutable g::GbCollisionWorldBorrowState worldBorrows_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool groundAttempted_=false,storageAttempted_=false,publicationBlocked_=false;
 Phase phase_=Phase::Empty;Payload payload_{};std::uint64_t generation_=0;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 static void add(std::size_t& n,std::size_t v){need(v<=std::numeric_limits<std::size_t>::max()-n,"GB aggregate payload addition overflow");n+=v;}
 template<class T>static void addElements(std::size_t& n,std::size_t count){need(count<=std::numeric_limits<std::size_t>::max()/sizeof(T),"GB aggregate payload multiplication overflow");add(n,count*sizeof(T));}
 bool sameOwners()const noexcept {
  return thread()&&source_&&source_->ready()&&ground_&&ground_->coldSource()==source_.get()&&triggers_&&triggers_->ready()&&
   triggers_->coldSource()==source_.get()&&actions_&&actions_->sourceOwner()==triggers_.get()&&
   actions_->preparedSourceFor(*triggers_,false)&&(animations_||!animationCount_)&&
   actions_->animationOwner()==animations_.get();
 }
 bool actualIdle()const noexcept {return thread()&&!queryBorrows_&&(!actions_||actions_->queryIdle())&&
   (!storage_||!storage_->get()||!storage_->get()->queue.count);}
 bool currentPublication()const noexcept {
  if(!storage_||!storage_->get()||std::fegetround()!=FE_TONEAREST)return false;
  return lane_.current(*storage_->get());
 }
 bool idleForRetirement()const noexcept {
  if(!actualIdle()||!lane_.detached())return false;
  if(!storage_||!storage_->get())return true;const auto& s=*storage_->get();
  if(!s.rowsDetached())return false; //every racer's query rows (shared world: all of them)
  for(std::size_t i=totalCount();i<s.registrations.size();++i){const auto& r=s.registrations[i];if(r.owner||r.slot||r.encodedObject)return false;}
  return true;
 }
 bool removeTriggers()noexcept {
  if(!storage_||!storage_->get())return true;auto& s=*storage_->get();
  for(auto& member:triggerMemberships_)if(member){const auto r=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,member->state,s.membershipWork);
   if(r.status!=g::LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;}
  return true;
 }
 bool triggerMembershipsDetached()const noexcept {
  for(const auto& member:triggerMemberships_)if(member&&(member->state.attached||member->state.object))return false;
  return true;
 }
 bool retire()noexcept {
  // Root must remove actual trigger memberships in EVERY Bank peer, then
  // close the genuine ActionOwner. A detached current arena alone proves no
  // all-peer cleanup. Never clear shared hooks on the root's behalf here.
  if(structuralBorrows_||worldBorrows_.leases||!idleForRetirement()||!triggerMembershipsDetached()||(actions_&&!actions_->closed())||!removeTriggers())return false;
  generation_=0;phase_=Phase::Closed;groundAttempted_=storageAttempted_=true;
  storage_.reset();for(auto& m:triggerMemberships_)m.reset();lane_.reset();
  candidates_.clear();bindings_.clear();radii_.clear();actions_.reset();triggers_.reset();animations_.reset();ground_.reset();source_.reset();return true;
 }
 void account(const rt::Limits& limits){
  Payload p{};p.sharedSource=source_->retainedPayloadBytes();p.externalReserved=externalReserved_;
  p.ownerDescriptors=sizeof(Owner)+sizeof(rt::Owner);p.groundDescriptors=ground_->descriptorPayloadBytes();
  addElements<g::Type2StaticCandidate49X>(p.candidateDescriptors,candidates_.capacity());addElements<rt::Binding>(p.candidateDescriptors,bindings_.capacity());
  addElements<track_static_candidates50IG::Radii>(p.candidateDescriptors,radii_.capacity());
  for(const auto& c:candidates_){const auto& f=c.candidate.collision.filter;addElements<std::uint32_t>(p.candidateDescriptors,f.categoryMasks.capacity());
   for(const auto& l:f.identityLists)addElements<std::uint32_t>(p.candidateDescriptors,l.identities.capacity());}
  addElements<rt::Membership>(p.memberships,triggerCount_);
  addElements<g::CollectionZoneLink50BX>(p.memberships,membershipCapacity_*triggerCount_);
  addElements<g::LiveZoneEntry50EN>(p.memberships,membershipCapacity_*triggerCount_);
  add(p.memberships,track_animation_world50AN::Lane::payloadBytes(animationCount_,membershipCapacity_));
  p.navigationReservation=gb_navigation50GB::Owner::ownerBytes()+gb_navigation50GB::Owner::persistentPayloadBytes()+gb_navigation50GB::Owner::topologyScratchBytes();
  std::size_t observed=0;for(auto n:{p.sharedSource,p.externalReserved,p.ownerDescriptors,p.groundDescriptors,p.candidateDescriptors,p.memberships,p.navigationReservation})add(observed,n);
  need(observed<=preStorageReservation_&&limits.coldPayloadBytes==coldLimit_,"GB actual descriptors exceed charged pre-Storage reservation");
  p.reservedBeforeStorage=preStorageReservation_;p.independentMesh=ground_->meshPayloadBytes();need(p.independentMesh!=std::numeric_limits<std::size_t>::max(),"GB independent mesh accounting overflow");
  for(const auto& radius:radii_)addElements<float>(p.independentRadii,radius.values.capacity());payload_=p;
 }
public:
 class QueryBorrow final {
  friend class Owner;std::shared_ptr<Owner> owner_;
  explicit QueryBorrow(std::shared_ptr<Owner> owner):owner_(std::move(owner)){++owner_->queryBorrows_;}
 public:
  QueryBorrow()=default;QueryBorrow(const QueryBorrow&)=delete;QueryBorrow&operator=(const QueryBorrow&)=delete;
  QueryBorrow(QueryBorrow&& other)noexcept:owner_(std::move(other.owner_)){};QueryBorrow&operator=(QueryBorrow&&)=delete;
  ~QueryBorrow(){if(owner_){if(!owner_->thread()||!owner_->queryBorrows_)std::terminate();--owner_->queryBorrows_;}}
  explicit operator bool()const noexcept{return owner_&&owner_->queryReady();}
  rt::Storage* storage()const noexcept{return *this?owner_->storage_->get():nullptr;}
 };
 class Borrow final {
  friend class Owner;std::shared_ptr<Owner> owner_;explicit Borrow(std::shared_ptr<Owner> owner):owner_(std::move(owner)){++owner_->structuralBorrows_;}
 public:
  Borrow()=default;Borrow(const Borrow&)=delete;Borrow&operator=(const Borrow&)=delete;Borrow(Borrow&&)=default;Borrow&operator=(Borrow&&)=delete;
  ~Borrow(){if(owner_){if(!owner_->thread()||!owner_->structuralBorrows_)std::terminate();--owner_->structuralBorrows_;}}
  //Structural validity survives a failed publication for real owner cleanup.
  //It is not permission to execute a collision query.
  explicit operator bool()const noexcept{return owner_&&owner_->thread()&&owner_->storage_&&owner_->storage_->get()&&owner_->phase_!=Phase::Closed;}
  QueryBorrow query()const noexcept{return *this?owner_->query():QueryBorrow{};}
  rt::Storage* registrationStorage()const noexcept{return *this&&owner_->actualIdle()&&
    (owner_->phase_==Phase::Ready||owner_->phase_==Phase::Retiring)?owner_->storage_->get():nullptr;}
  // Registration-only compatibility aliases. Dynamic queries require QueryBorrow.
  rt::Storage* storage()const noexcept{return registrationStorage();}
  g::LiveZoneWorld50EN* world()const noexcept{auto* s=registrationStorage();return s?&s->world:nullptr;}
  std::span<g::Type2StaticCandidate49X> candidateStorage()const noexcept{return *this?std::span<g::Type2StaticCandidate49X>{owner_->candidates_}:std::span<g::Type2StaticCandidate49X>{};}
  std::span<const rt::Binding> bindingStorage()const noexcept{return *this?std::span<const rt::Binding>{owner_->bindings_}:std::span<const rt::Binding>{};}
  rt::Membership* triggerMembership(std::size_t i)const noexcept{return *this&&i<owner_->triggerCount_?owner_->triggerMemberships_[i].get():nullptr;}
  std::size_t triggerCount()const noexcept{return *this?owner_->triggerCount_:0;}
  gb_trigger_actions50GB::Owner* actions()const noexcept{return *this?owner_->actions_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner();
 // Object reference of animation owner i in the one world (after the ground
 // and trigger candidates; stock 634..637).
 std::size_t animationObject(std::size_t i)const noexcept{return sourceCount_+triggerCount_+i+1;}
 std::size_t sourceCount()const noexcept{return sourceCount_;}
 std::size_t triggerCount()const noexcept{return triggerCount_;}
 std::size_t animationCount()const noexcept{return animationCount_;}
 std::size_t totalCount()const noexcept{return sourceCount_+triggerCount_+animationCount_;}
 // triggers / animations: the live counts the data authors (the decoded
 // trigger store and the animation owners), charged exactly.
 static std::size_t requiredPreStoragePayloadBytes(const gb_cold_sources50LA::Owner& source,std::size_t external,
     std::size_t triggers,std::size_t animations)noexcept {
  try{need(triggers<=triggerCapacity&&animations<=animationCapacity,"GB live trigger/animation extent exceeds the store");
   const auto ground=gb_runtime_ground50LA::Owner::placedCollisionCount(source);need(ground,"GB source has no placed COL");
   const auto total=ground+triggers+animations,members=triggers+animations;
   const auto membership=source.world().portals.size()+1; // track_runtime_storage50IG: portals + 1
   std::size_t n=source.retainedPayloadBytes();add(n,external);add(n,sizeof(Owner)+sizeof(rt::Owner));add(n,gb_runtime_ground50LA::Owner::maximumDescriptorPayloadBytes(source));
   addElements<g::Type2StaticCandidate49X>(n,total);addElements<rt::Binding>(n,total);addElements<track_static_candidates50IG::Radii>(n,ground);
   addElements<rt::Membership>(n,triggers);addElements<g::CollectionZoneLink50BX>(n,membership*triggers);
   addElements<g::LiveZoneEntry50EN>(n,membership*triggers);add(n,track_animation_world50AN::Lane::payloadBytes(animations,membership));(void)members;
   add(n,gb_navigation50GB::Owner::ownerBytes()+gb_navigation50GB::Owner::persistentPayloadBytes()+gb_navigation50GB::Owner::topologyScratchBytes());return n;
  }catch(...){return std::numeric_limits<std::size_t>::max();}
 }
 bool prepareGround(std::shared_ptr<const gb_cold_sources50LA::Owner> source,a::TrackCollisionMasks50AN masks,
      const std::vector<std::uint8_t>& actualPhy,unsigned rank,std::size_t external,const rt::Limits& limits,
      std::size_t triggers,std::size_t animations,std::string& error){
  if(!thread()||groundAttempted_||weak_from_this().expired()){error="GB aggregate requires one shared owner-thread lifetime";return false;}
  groundAttempted_=true;phase_=Phase::Ground;
  try{need(source&&source->ready(),"GB same actual cold source required");
   const auto reserved=requiredPreStoragePayloadBytes(*source,external,triggers,animations);
   plannedTriggers_=triggers;plannedAnimations_=animations;
   need(limits.coldPayloadBytes&&limits.coldPayloadBytes<=32u*1024u*1024u&&reserved<limits.coldPayloadBytes,"GB complete shared source/external/aggregate budget exhausted");
   externalReserved_=external;preStorageReservation_=reserved;coldLimit_=limits.coldPayloadBytes;source_=std::move(source);ground_=std::make_unique<gb_runtime_ground50LA::Owner>();
   if(!ground_->prepare(source_,masks,actualPhy,rank,error))return false;
   need(ground_->descriptorPayloadBytes()<=gb_runtime_ground50LA::Owner::maximumDescriptorPayloadBytes(*source_),"GB Ground descriptor reservation exceeded");return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool prepareStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
      std::shared_ptr<const gb_trigger_authoring50GB::Owner> triggers,std::shared_ptr<gb_trigger_actions50GB::Owner> actions,
      std::shared_ptr<track_animation_runtime50AN::Owner> animations,const track_static_candidates50IG::Bindings& names,const rt::Limits& limits,Diagnostic& d)noexcept {
  if(!thread()||storageAttempted_||!ground_||!ground_->ground())return false;storageAttempted_=true;phase_=Phase::Candidates;
  try{d={};d.phase=phase_;need(application&&application->application10(),"GB actual live Application required");
   // The common track-animation runtime exists exactly when the TOB authors
   // [Animations] (absent = no runtime; the trigger actions then hold none either).
   need(triggers&&triggers->ready()&&triggers->coldSource()==source_.get()&&actions&&
     actions->sourceOwner()==triggers.get()&&actions->animationOwner()==animations.get()&&actions->preparedSourceFor(*triggers)&&
     !actions->nativeGroup().ready&&actions->queryIdle()&&(!animations||animations->ready()),
     "GB real same-source trigger/actions/animation join required");
   sourceCount_=ground_->ground()->owners.size();triggerCount_=triggers->liveCount();
   animationCount_=track_animation_world50AN::Lane::collisionRowCount(animations.get());membershipCapacity_=source_->world().portals.size()+1;
   need(triggerCount_==plannedTriggers_&&animationCount_==plannedAnimations_&&sourceCount_&&
     sourceCount_==gb_runtime_ground50LA::Owner::placedCollisionCount(*source_)&&triggerCount_==actions->nativeGroup().triggers.size(),
     "GB live ground/trigger/animation extent differs from the charged reservation");
   course_archive_receipt50CI::countReceipt("gb_collision_world","candidates",totalCount(),stockTotalCandidates);
   triggers_=std::move(triggers);actions_=std::move(actions);animations_=std::move(animations);
   need(sameOwners(),"GB actual placed Ground associations required");
   candidates_.reserve(totalCount());bindings_.reserve(totalCount());radii_.reserve(sourceCount_);
   track_static_candidates50IG::prepare(*ground_->ground(),names,candidates_,radii_,bindings_);
   need(gb_trigger_candidates50GB::prepare(*triggers_,*actions_,candidates_,bindings_,names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8,
     sourceCount_,totalCount())==sourceCount_&&
     candidates_.size()==sourceCount_+triggerCount_&&bindings_.size()==candidates_.size(),"GB actual ground + trigger prefix required");
   need(lane_.prepareCandidates(animations_,candidates_,bindings_,names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8)==sourceCount_+triggerCount_&&
     candidates_.size()==totalCount()&&bindings_.size()==totalCount()&&lane_.count()==animationCount_,"GB track-animation candidate lane rejected");
   account(limits);d.payload=payload_;auto reduced=limits;reduced.coldPayloadBytes-=preStorageReservation_;
   phase_=Phase::Storage;d.phase=phase_;storage_=std::make_unique<rt::Owner>();
   if(!storage_->prepare(std::move(application),*ground_->ground(),bindings_.data(),bindings_.size(),reduced,d.storage)){d.message=d.storage.message;return false;}
   // Storage extent from the data (stock 637 / 13 / 17 / 34 / 18).
   auto& s=*storage_->get();need(s.count.terrain==totalCount()&&s.count.zones==source_->world().zones.size()&&
     s.count.portals==source_->world().portals.size()&&s.count.portalLinks==2*s.count.portals&&
     s.count.membership==membershipCapacity_,"GB Storage extent differs from the course data");
   payload_.storageRequested=d.storage.requestedPayloadBytes;payload_.totalRequested=preStorageReservation_;add(payload_.totalRequested,payload_.storageRequested);
   need(payload_.totalRequested<=limits.coldPayloadBytes,"GB complete aggregate budget exceeded");d.payload=payload_;phase_=Phase::Memberships;d.phase=phase_;
   for(std::size_t i=0;i<triggerCount_;++i)triggerMemberships_[i]=std::make_unique<rt::Membership>(membershipCapacity_);
   for(std::size_t i=0;i<triggerCount_;++i){d.trigger=i;const auto& actual=triggers_->source(i);auto& member=*triggerMemberships_[i];const auto bounds=gb_trigger_candidates50GB::bounds(actual);
    d.membership=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member.state,member.storage(),static_cast<g::CollectionRef50BX>(sourceCount_+i+1),actual.zone,g::ZoneMethod50CL::First,0.f,bounds,s.membershipWork);
    need(d.membership.status==g::LiveZoneStatus50EN::Complete,"GB authentic trigger attachment rejected");
    d.membership=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,member.state,bounds,s.membershipWork);need(d.membership.status==g::LiveZoneStatus50EN::Complete,"GB authentic trigger publication rejected");
   }
   lane_.makeMemberships(membershipCapacity_);
   std::cerr<<"gb_course_owner owner=collision_world placed_col="<<sourceCount_<<" triggers="<<triggerCount_
       <<" animations="<<animationCount_<<" candidates="<<totalCount()<<" stock="<<(totalCount()==stockTotalCandidates&&
       sourceCount_==stockSourceCandidates&&triggerCount_==stockTriggerCandidates?1:0)<<'\n';
   // The track-animation binding publishes the lane next (track_animation_binding50an.h);
   // with no [Animations] the aggregate is complete here.
   if(animations_){phase_=Phase::AnimationPending;d.phase=phase_;d.message.clear();return true;}
   if(!currentPublication())throw std::runtime_error("GB aggregate publication without animations rejected");
   if(!generation_)generation_=1;publicationBlocked_=false;phase_=Phase::Ready;d.phase=phase_;d.message.clear();return true;
  }catch(const std::exception& e){try{d.phase=phase_;d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.phase=phase_;d.message="GB cold aggregate allocation/assembly failure";}catch(...){}return false;}
 }
 bool geometryReady()const noexcept{return phase_==Phase::Ready&&generation_&&!publicationBlocked_&&sameOwners()&&storage_&&storage_->get()&&
   !actions_->nativeGroup().actionsFailed&&currentPublication();}
 // Dynamic readiness is genuine ActionOwner destination admission. It may
 // remain true during a held action query; query() separately requires idle.
 bool queryReady()const noexcept{return geometryReady()&&actions_->queryReady()&&actions_->nativeGroup().ready&&
  !actions_->nativeGroup().actionsFailed;}
 bool beginRetirement()noexcept {
  if(!actualIdle()||phase_==Phase::Closed)return false;
  publicationBlocked_=true;phase_=Phase::Retiring;return true;
 }
 Phase phase()const noexcept{return phase_;}const Payload& payload()const noexcept{return payload_;}
 const authored_runtime_ground50cp_fixture::ColdScene* ground()const noexcept{return thread()&&ground_?ground_->ground():nullptr;}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept{return thread()?source_.get():nullptr;}
 bool bindHumanLift(float lift)noexcept{return thread()&&source_&&ground_&&ground_->bindHumanLift(*source_,lift);}
 Borrow borrow()noexcept {if(!geometryReady())return {};auto self=weak_from_this().lock();return self?Borrow{std::move(self)}:Borrow{};}
 // Cleanup-only issuance after a failed cold/live prefix. It cannot satisfy
 // geometryReady/queryReady or mint final navigation authority. The retained
 // borrow must end before close; root still visits every actual Bank peer.
 Borrow retirementBorrow()noexcept {
  if(phase_!=Phase::Retiring||!actualIdle()||!storage_||!storage_->get())return {};
  auto self=weak_from_this().lock();return self?Borrow{std::move(self)}:Borrow{};
 }
 QueryBorrow query()noexcept {if(!queryReady()||!actions_->queryIdle())return {};auto self=weak_from_this().lock();return self?QueryBorrow{std::move(self)}:QueryBorrow{};}
 g::GbCollisionWorldLease lease()const noexcept {if(!geometryReady())return {};auto self=weak_from_this().lock();return self?g::GbCollisionWorldLease{std::move(self),source_,storage_->get()->world,generation_,worldBorrows_}:g::GbCollisionWorldLease{};}
 bool close()noexcept {if(!thread()||weak_from_this().use_count()>1)return false;if(phase_==Phase::Closed)return true;return retire();}
 // ---- track_animation_world50AN::World (the common binding's view of the lane) ----
 track_animation_world50AN::Lane* animationLane()noexcept override{return thread()&&animations_?&lane_:nullptr;}
 rt::Storage* animationStorage()noexcept override{return thread()&&storage_?storage_->get():nullptr;}
 bool animationIdle()const noexcept override{return actualIdle();}
 bool animationPending()const noexcept override{return phase_==Phase::AnimationPending&&sameOwners()&&storage_&&storage_->get();}
 bool commitAnimationLane()noexcept override{
  if(phase_!=Phase::AnimationPending||!actualIdle()||!sameOwners()||!currentPublication())return false;
  if(!generation_)generation_=1;phase_=Phase::Ready;publicationBlocked_=false;return true;}
 void blockAnimationPublication(bool blocked)noexcept override{if(phase_==Phase::Ready||phase_==Phase::AnimationPending)publicationBlocked_=blocked;}
 bool animationLaneDetached()const noexcept{return lane_.detached();}
};
}

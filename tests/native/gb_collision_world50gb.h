#pragma once
// Complete source-owned GB world. Live admission is provided only by the
// same-root renderer, Bank, navigation and action binding.
#include "gb_runtime_ground50la.h"
#include "gb_navigation50gb.h"
#include "gb_animation_runtime50gb.h"
#include "gb_trigger_candidates50gb.h"
#include "track_static_candidates50ig_fixture.h"
#include <array>
#include <memory>
#include <span>
#include <thread>

namespace gb_animation_binding50GB { class Binding; }
namespace gb_query50GB { class Binding; }
namespace gb_collision_world50GB {
namespace g=rr::game;namespace a=rr::assets;namespace rt=track_runtime_storage50IG;
inline constexpr std::size_t sourceCandidates=615,triggerCandidates=18,animationCandidates=4,totalCandidates=637;
static_assert(triggerCandidates==gb_trigger_authoring50GB::count);
struct Payload {
 std::size_t sharedSource=0,externalReserved=0,ownerDescriptors=0,groundDescriptors=0;
 std::size_t candidateDescriptors=0,memberships=0,navigationReservation=0;
 std::size_t reservedBeforeStorage=0,storageRequested=0,totalRequested=0,independentMesh=0,independentRadii=0;
};
enum class Phase {Empty,Ground,Candidates,Storage,Memberships,AnimationPending,Ready,Retiring,Closed};
struct Diagnostic {Phase phase=Phase::Empty;std::size_t trigger=0;std::string message;rt::Diagnostic storage{};g::LiveZoneResult50EN membership{};Payload payload{};};
// Complete637-owner aggregate only. A615+4 prefix cannot reach Storage/Ready
// or mint navigation authority. Actual trigger source/actions are mandatory.
class Owner final : public std::enable_shared_from_this<Owner> {
 friend class ::gb_animation_binding50GB::Binding;
 friend class ::gb_query50GB::Binding;
 std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
 std::unique_ptr<gb_runtime_ground50LA::Owner> ground_;
 std::shared_ptr<const gb_trigger_authoring50GB::Owner> triggers_;
 std::shared_ptr<gb_trigger_actions50GB::Owner> actions_;
 std::shared_ptr<gb_animation_runtime50GB::Owner> animations_;
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<rt::Binding> bindings_;
 std::array<std::unique_ptr<rt::Membership>,triggerCandidates> triggerMemberships_;
 std::array<std::unique_ptr<rt::Membership>,animationCandidates> animationMemberships_;
 std::array<g::LiveZoneMembership50EN,animationCandidates> committedMemberships_{};
 std::unique_ptr<rt::Owner> storage_;
 std::size_t externalReserved_=0,preStorageReservation_=0,coldLimit_=0,queryBorrows_=0,structuralBorrows_=0;
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
   actions_->preparedSourceFor(*triggers_,false)&&animations_&&
   actions_->animationOwner()==animations_.get()&&animations_->coldSource()==source_.get();
 }
 bool actualIdle()const noexcept {return thread()&&!queryBorrows_&&(!actions_||actions_->queryIdle())&&
   (!storage_||!storage_->get()||!storage_->get()->queue.count);}
 bool animationAccess(const ::gb_animation_binding50GB::Binding* issuer)const noexcept {
  return actualIdle()&&issuer&&animations_&&animations_->binding_==issuer&&storage_&&storage_->get()&&
    (phase_==Phase::AnimationPending||phase_==Phase::Ready||phase_==Phase::Retiring);
 }
 bool currentPublication()const noexcept {
  if(!animations_||!storage_||!storage_->get()||std::fegetround()!=FE_TONEAREST)return false;const auto& s=*storage_->get();
  for(std::size_t i=0;i<animationCandidates;++i){const auto index=sourceCandidates+triggerCandidates+i;const auto object=index+1;
   const auto& state=animations_->state_[i];const auto& c=state.collision;const auto& view=animations_->views_[i];
   const auto* candidate=animations_->candidates_[i];if(!candidate||s.candidates[index]!=candidate||candidate!=&candidates_[index]||
      candidate->shapeType!=1||candidate->compound!=&view||candidate->candidate.collision.shapeType!=1||
      candidate->candidate.collision.compound!=&view||view.parts!=c.parts.data()||view.count!=animations_->source(i).collisionCount||
      std::memcmp(view.ownerWorld.v,c.ownerWorld.v,sizeof c.ownerWorld.v)||std::memcmp(view.center.v,c.center.v,sizeof c.center.v)||
      std::memcmp(view.half.v,c.half.v,sizeof c.half.v)||!animationMemberships_[i])return false;
   const auto& shape=candidate->candidate.collision;const auto& movement=s.movement[index].collision;
   if(s.movement[index].shapeType!=1||s.movement[index].compound!=&view||movement.shapeType!=1||movement.compound!=&view||shape.geometry.id!=0x3e000000u+i||shape.filter.identity!=shape.geometry.id||shape.ownerIdentity9C!=0x3f000000u+i||
      shape.filter.categoryA4!=1u||shape.hasResponseCallbackE8||
      std::memcmp(shape.geometry.worldMatrix.v,c.ownerWorld.v,sizeof c.ownerWorld.v)||
      std::memcmp(shape.sphere.center.v,c.sphereCenter.v,sizeof c.sphereCenter.v)||std::memcmp(&shape.sphere.radius,&c.radius,sizeof(float))||
      std::memcmp(movement.geometry.worldMatrix.v,c.ownerWorld.v,sizeof c.ownerWorld.v)||
      std::memcmp(movement.sphere.center.v,c.sphereCenter.v,sizeof c.sphereCenter.v)||std::memcmp(&movement.sphere.radius,&c.radius,sizeof(float)))return false;
   const auto projected=g::projectCollectionObject50CF(shape.filter,shape.sphere,shape.typeTag,1,0);
   if(!projected.valid||std::memcmp(s.source[index].cachedCenter.v,projected.state.cachedCenter.v,3*sizeof(float))||
      std::memcmp(&s.source[index].cachedRadius,&projected.state.cachedRadius,sizeof(float)))return false;
   const auto& m=animationMemberships_[i]->state;const auto& committed=committedMemberships_[i];
   if(!m.attached||m.object!=object||m.method!=g::ZoneMethod50CL::Second||!m.revision||m.revision!=committed.revision||
      m.lastShapeTag!=1||m.lastType2Resource||m.storage.capacity!=18||m.storage.links!=animationMemberships_[i]->links.data()||
      m.storage.entries!=animationMemberships_[i]->entries.data()||std::memcmp(m.lastShapeWorld.v,c.ownerWorld.v,sizeof c.ownerWorld.v))return false;
   const auto& primary=m.storage.entries[0].registration;const auto& registration=s.registrations[index];
   if(!primary.owner||primary.owner!=registration.owner||primary.slot!=registration.slot||primary.encodedObject!=registration.encodedObject||registration.encodedObject!=(object<<2u))return false;
   g::CollectionBoundsResult48S expected{};const auto bounds=animations_->collisionBounds(i);
   if(g::prepareCollectionBounds48S(bounds.bounds,expected)!=g::CollectionBoundsStatus48S::Complete||
      std::memcmp(m.bounds.center.v,expected.center.v,sizeof expected.center.v)||std::memcmp(m.bounds.half.v,expected.half.v,sizeof expected.half.v)||
      std::memcmp(m.bounds.cachedCenter.v,expected.cachedCenter.v,sizeof expected.cachedCenter.v)||std::memcmp(&m.bounds.cachedRadius,&expected.cachedRadius,sizeof(float)))return false;
  }
  return true;
 }
 bool attachAnimation(const ::gb_animation_binding50GB::Binding* issuer,std::size_t i,g::LiveZoneResult50EN& result)noexcept {
  if(!animationAccess(issuer)||phase_!=Phase::AnimationPending||i>=animationCandidates||!animationMemberships_[i])return false;
  auto& s=*storage_->get();auto& m=*animationMemberships_[i];const auto bounds=animations_->collisionBounds(i);
  result=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,m.state,m.storage(),
    static_cast<g::CollectionRef50BX>(sourceCandidates+triggerCandidates+i+1),animations_->source(i).authored.startingZone,
    g::ZoneMethod50CL::Second,0.f,bounds,s.membershipWork);
  return result.status==g::LiveZoneStatus50EN::Complete&&m.state.attached&&m.state.object==sourceCandidates+triggerCandidates+i+1;
 }
 bool updateAnimation(const ::gb_animation_binding50GB::Binding* issuer,std::size_t i,g::LiveZoneResult50EN& result)noexcept {
  if(!animationAccess(issuer)||i>=animationCandidates||!animationMemberships_[i])return false;
  auto& s=*storage_->get();auto& m=*animationMemberships_[i];const auto object=sourceCandidates+triggerCandidates+i+1;
  if(!m.state.attached||m.state.object!=object||s.candidates[object-1]!=animations_->candidates_[i])return false;
  const auto bounds=animations_->collisionBounds(i);
  const auto& current=animations_->candidates_[i]->candidate.collision;
  const auto projected=g::projectCollectionObject50CF(current.filter,current.sphere,current.typeTag,1,0);
  if(!projected.valid)return false;
  result=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,m.state,bounds,s.membershipWork);
  if(result.status!=g::LiveZoneStatus50EN::Complete)return false;
  //Movement candidate geometry is shared/global, while the mutable query
  //flags/history lanes remain per-racer. Never copy their vectors or flags.
  auto& movement=s.movement[object-1];const auto& actual=animations_->candidates_[i]->candidate.collision;
  movement.collision.geometry.worldMatrix=actual.geometry.worldMatrix;movement.collision.sphere=actual.sphere;
  //Native query-cache projection follows the same current pose in each peer.
  //Keep that racer's query epochs/flags and original cache W untouched.
  auto& objectView=s.objects[object-1];for(unsigned k=0;k<3;++k)objectView.position[k]=projected.object.position[k];
  objectView.scale10=projected.object.scale10;objectView.scale11=projected.object.scale11;
  auto& cached=s.source[object-1];std::memcpy(cached.cachedCenter.v,projected.state.cachedCenter.v,3*sizeof(float));cached.cachedRadius=projected.state.cachedRadius;
  return true;
 }
 bool commitAnimations(const ::gb_animation_binding50GB::Binding* issuer)noexcept {
  if(!animationAccess(issuer)||!sameOwners()||animations_->failed_||!currentPublication())return false;
  for(std::size_t i=0;i<animationCandidates;++i)if(!animations_->state_[i].initialComplete||!animationMemberships_[i]||
    !animationMemberships_[i]->state.attached||animationMemberships_[i]->state.object!=sourceCandidates+triggerCandidates+i+1)return false;
  if(!generation_)generation_=1;phase_=Phase::Ready;publicationBlocked_=false;return true;
 }
 bool removeAnimation(const ::gb_animation_binding50GB::Binding* issuer,std::size_t i,g::LiveZoneResult50EN& result)noexcept {
  if(!animationAccess(issuer)||i>=animationCandidates)return false;if(!animationMemberships_[i])return true;
  auto& s=*storage_->get();auto& m=*animationMemberships_[i];
  result=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,m.state,s.membershipWork);
  return result.status==g::LiveZoneStatus50EN::Complete&&!m.state.attached&&!m.state.object;
 }
 bool idleForRetirement()const noexcept {
  if(!actualIdle()||(animations_&&(animations_->binding_||animations_->servicing_)))return false;
  for(const auto& member:animationMemberships_)if(member&&(member->state.attached||member->state.object))return false;
  if(!storage_||!storage_->get())return true;const auto& s=*storage_->get();
  for(const auto* m:{&s.mainMembership,&s.secondaryMembership,&s.terrainMembership})if(m->state.attached||m->state.object)return false;
  for(std::size_t i=totalCandidates;i<s.registrations.size();++i){const auto& r=s.registrations[i];if(r.owner||r.slot||r.encodedObject)return false;}
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
  storage_.reset();for(auto& m:triggerMemberships_)m.reset();for(auto& m:animationMemberships_)m.reset();
  if(animations_)animations_->candidates_.fill(nullptr);
  candidates_.clear();bindings_.clear();radii_.clear();actions_.reset();triggers_.reset();animations_.reset();ground_.reset();source_.reset();return true;
 }
 void account(const rt::Limits& limits){
  Payload p{};p.sharedSource=source_->retainedPayloadBytes();p.externalReserved=externalReserved_;
  p.ownerDescriptors=sizeof(Owner)+sizeof(rt::Owner);p.groundDescriptors=ground_->descriptorPayloadBytes();
  addElements<g::Type2StaticCandidate49X>(p.candidateDescriptors,candidates_.capacity());addElements<rt::Binding>(p.candidateDescriptors,bindings_.capacity());
  addElements<track_static_candidates50IG::Radii>(p.candidateDescriptors,radii_.capacity());
  for(const auto& c:candidates_){const auto& f=c.candidate.collision.filter;addElements<std::uint32_t>(p.candidateDescriptors,f.categoryMasks.capacity());
   for(const auto& l:f.identityLists)addElements<std::uint32_t>(p.candidateDescriptors,l.identities.capacity());}
  addElements<rt::Membership>(p.memberships,triggerCandidates+animationCandidates);
  addElements<g::CollectionZoneLink50BX>(p.memberships,18*(triggerCandidates+animationCandidates));
  addElements<g::LiveZoneEntry50EN>(p.memberships,18*(triggerCandidates+animationCandidates));
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
  rt::Membership* triggerMembership(std::size_t i)const noexcept{return *this&&i<triggerCandidates?owner_->triggerMemberships_[i].get():nullptr;}
  gb_trigger_actions50GB::Owner* actions()const noexcept{return *this?owner_->actions_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner();
 static std::size_t requiredPreStoragePayloadBytes(const gb_cold_sources50LA::Owner& source,std::size_t external)noexcept {
  try{std::size_t n=source.retainedPayloadBytes();add(n,external);add(n,sizeof(Owner)+sizeof(rt::Owner));add(n,gb_runtime_ground50LA::Owner::maximumDescriptorPayloadBytes(source));
   addElements<g::Type2StaticCandidate49X>(n,totalCandidates);addElements<rt::Binding>(n,totalCandidates);addElements<track_static_candidates50IG::Radii>(n,sourceCandidates);
   addElements<rt::Membership>(n,triggerCandidates+animationCandidates);addElements<g::CollectionZoneLink50BX>(n,18*(triggerCandidates+animationCandidates));
   addElements<g::LiveZoneEntry50EN>(n,18*(triggerCandidates+animationCandidates));
   add(n,gb_navigation50GB::Owner::ownerBytes()+gb_navigation50GB::Owner::persistentPayloadBytes()+gb_navigation50GB::Owner::topologyScratchBytes());return n;
  }catch(...){return std::numeric_limits<std::size_t>::max();}
 }
 bool prepareGround(std::shared_ptr<const gb_cold_sources50LA::Owner> source,a::TrackCollisionMasks50AN masks,
      const std::vector<std::uint8_t>& actualPhy,unsigned rank,std::size_t external,const rt::Limits& limits,std::string& error){
  if(!thread()||groundAttempted_||weak_from_this().expired()){error="GB aggregate requires one shared owner-thread lifetime";return false;}
  groundAttempted_=true;phase_=Phase::Ground;
  try{need(source&&source->ready(),"GB same actual cold source required");const auto reserved=requiredPreStoragePayloadBytes(*source,external);
   need(limits.coldPayloadBytes&&limits.coldPayloadBytes<=32u*1024u*1024u&&reserved<limits.coldPayloadBytes,"GB complete shared source/external/aggregate budget exhausted");
   externalReserved_=external;preStorageReservation_=reserved;coldLimit_=limits.coldPayloadBytes;source_=std::move(source);ground_=std::make_unique<gb_runtime_ground50LA::Owner>();
   if(!ground_->prepare(source_,masks,actualPhy,rank,error))return false;
   need(ground_->descriptorPayloadBytes()<=gb_runtime_ground50LA::Owner::maximumDescriptorPayloadBytes(*source_),"GB Ground descriptor reservation exceeded");return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool prepareStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
      std::shared_ptr<const gb_trigger_authoring50GB::Owner> triggers,std::shared_ptr<gb_trigger_actions50GB::Owner> actions,
      std::shared_ptr<gb_animation_runtime50GB::Owner> animations,const track_static_candidates50IG::Bindings& names,const rt::Limits& limits,Diagnostic& d)noexcept {
  if(!thread()||storageAttempted_||!ground_||!ground_->ground())return false;storageAttempted_=true;phase_=Phase::Candidates;
  try{d={};d.phase=phase_;need(application&&application->application10(),"GB actual live Application required");
   need(triggers&&triggers->ready()&&triggers->coldSource()==source_.get()&&actions&&animations&&
     actions->sourceOwner()==triggers.get()&&actions->animationOwner()==animations.get()&&actions->preparedSourceFor(*triggers)&&
     !actions->nativeGroup().ready&&actions->queryIdle()&&animations->stagedSourceReady()&&animations->binding_&&!animations->publicationReady_,
     "GB real same-source trigger/actions/animation join required");
   for(const auto& s:animations->state_)need(s.visualPrepared&&s.collisionPrepared&&!s.initialAttempted,"GB four actual visual/compound prefixes required");
   triggers_=std::move(triggers);actions_=std::move(actions);animations_=std::move(animations);
   need(sameOwners()&&ground_->ground()->owners.size()==sourceCandidates,"GB actual615 placed Ground associations required");
   candidates_.reserve(totalCandidates);bindings_.reserve(totalCandidates);radii_.reserve(sourceCandidates);
   track_static_candidates50IG::prepare(*ground_->ground(),names,candidates_,radii_,bindings_);
   need(gb_trigger_candidates50GB::prepare(*triggers_,*actions_,candidates_,bindings_,names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8)==sourceCandidates&&
     candidates_.size()==sourceCandidates+triggerCandidates&&bindings_.size()==candidates_.size(),"GB actual615+18 trigger prefix required");
   candidates_.resize(totalCandidates);bindings_.resize(totalCandidates);for(std::size_t i=0;i<totalCandidates;++i)bindings_[i].candidate=&candidates_[i];
   for(std::size_t i=0;i<animationCandidates;++i){const auto index=sourceCandidates+triggerCandidates+i;
    need(animations_->prepareCandidate(i,candidates_[index],names.collisionTypeTag,names.preservedFlags31,names.preservedFlagsF8),"GB genuine compound candidate issuance rejected");
    bindings_[index]={std::numeric_limits<std::size_t>::max(),&candidates_[index],true,animations_->source(i).authored.startingZone,0,g::ZoneMethod50CL::Second};
   }
   account(limits);d.payload=payload_;auto reduced=limits;reduced.coldPayloadBytes-=preStorageReservation_;
   phase_=Phase::Storage;d.phase=phase_;storage_=std::make_unique<rt::Owner>();
   if(!storage_->prepare(std::move(application),*ground_->ground(),bindings_.data(),bindings_.size(),reduced,d.storage)){d.message=d.storage.message;return false;}
   auto& s=*storage_->get();need(s.count.terrain==637&&s.count.zones==13&&s.count.portals==17&&s.count.portalLinks==34&&s.count.membership==18,"GB actual fixed Storage extent changed");
   payload_.storageRequested=d.storage.requestedPayloadBytes;payload_.totalRequested=preStorageReservation_;add(payload_.totalRequested,payload_.storageRequested);
   need(payload_.totalRequested<=limits.coldPayloadBytes,"GB complete aggregate budget exceeded");d.payload=payload_;phase_=Phase::Memberships;d.phase=phase_;
   for(auto& m:triggerMemberships_)m=std::make_unique<rt::Membership>(18);
   for(std::size_t i=0;i<triggerCandidates;++i){d.trigger=i;const auto& actual=triggers_->source(i);auto& member=*triggerMemberships_[i];const auto bounds=gb_trigger_candidates50GB::bounds(actual);
    d.membership=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member.state,member.storage(),static_cast<g::CollectionRef50BX>(sourceCandidates+i+1),actual.zone,g::ZoneMethod50CL::First,0.f,bounds,s.membershipWork);
    need(d.membership.status==g::LiveZoneStatus50EN::Complete,"GB authentic trigger attachment rejected");
    d.membership=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,member.state,bounds,s.membershipWork);need(d.membership.status==g::LiveZoneStatus50EN::Complete,"GB authentic trigger publication rejected");
   }
   for(auto& m:animationMemberships_)m=std::make_unique<rt::Membership>(18);phase_=Phase::AnimationPending;d.phase=phase_;d.message.clear();return true;
  }catch(const std::exception& e){try{d.phase=phase_;d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.phase=phase_;d.message="GB cold aggregate allocation/assembly failure";}catch(...){}return false;}
 }
 bool geometryReady()const noexcept{return phase_==Phase::Ready&&generation_&&!publicationBlocked_&&sameOwners()&&storage_&&storage_->get()&&animations_->ready()&&!actions_->nativeGroup().actionsFailed&&currentPublication();}
 // Dynamic readiness is genuine ActionOwner destination admission. It may
 // remain true during a held action query; query() separately requires idle.
 bool queryReady()const noexcept{return geometryReady()&&actions_->queryReady()&&actions_->nativeGroup().ready&&
  !actions_->nativeGroup().actionsFailed;}
 bool animationPending()const noexcept{return phase_==Phase::AnimationPending&&sameOwners()&&storage_&&storage_->get();}
 bool beginRetirement()noexcept {
  if(!actualIdle()||phase_==Phase::Closed||(animations_&&animations_->servicing_))return false;
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
  if(phase_!=Phase::Retiring||!actualIdle()||(animations_&&animations_->servicing_)||!storage_||!storage_->get())return {};
  auto self=weak_from_this().lock();return self?Borrow{std::move(self)}:Borrow{};
 }
 QueryBorrow query()noexcept {if(!queryReady()||!actions_->queryIdle())return {};auto self=weak_from_this().lock();return self?QueryBorrow{std::move(self)}:QueryBorrow{};}
 g::GbCollisionWorldLease lease()const noexcept {if(!geometryReady())return {};auto self=weak_from_this().lock();return self?g::GbCollisionWorldLease{std::move(self),source_,storage_->get()->world,generation_,worldBorrows_}:g::GbCollisionWorldLease{};}
 bool close()noexcept {if(!thread()||weak_from_this().use_count()>1)return false;if(phase_==Phase::Closed)return true;return retire();}
};
}

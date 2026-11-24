#pragma once
// Common rewrite (R3, B-C11, PLAN 2.2): the ONE collision world of a race, for every
// course. Replaces the six per-course collision worlds ({gb,mb,mc,rc,tb,tc}_collision_world50xx.h),
// their runtime grounds ({gb,mc,rc,sc,tb,tc}_runtime_ground50la.h) and cold sources
// (*_cold_sources50la.h). Generalised from the GB/MC aggregate shape (one
// track_runtime_storage50IG Storage / LiveZoneWorld50EN, one lease), with every extent read
// from the course's own files through the common Track (common_track50cr.h):
//
//   05.04 prepareGround   (271230/27BB40): every SCB instance whose collision member the
//         archive holds becomes one placed COL owner, in source order (the stock counts are
//         receipts only); placement vectors are read from the instance block as decimal ->
//         binary32 (the original's own parse); the 1A9EE0 fresh-game height descriptor is
//         read from the SCB Environment (TerrainFile/WaterFile; water "none" = absent, the
//         2745B0 compare - A9 data branch).
//   08+   placeGrid       (2A2880 input): the grid centre pose from the map spline's first node
//         and the selected racer's PHY lift, once the navigation exists (step 08 runs after
//         the camera session, PLAN 2.1).
//   10+   prepareStorage  : the static candidates of the placed owners, one candidate per live
//         trigger (274E90 rows through R5's one trigger action owner, course_trigger_actions50CT)
//         and the track-animation lane, in
//         one Storage sized from those counts; membership capacity = portals + 1 (GB rule).
//         Runs once the roster's collision owners exist (the shared storage layout sizes
//         every racer's rows from them), i.e. after step 10, as today's world.cpp does.
//   frame queryRacer / observePrepared / serviceWorld: the one query bracket (generalised from
//         mc_query50MC::Binding): one synchronous indexed query under the trigger action lease, the
//         prepared root geometry of each racer collider, and the 2A0B30 exit service once per step.
//
// Nothing here names a course or a pod. Nothing writes guest memory: every object is a
// port-owned host object. Every store is sized once from the data and every write is
// bounds-checked before it happens (Block<T> for the trigger memberships; the reserve()
// below is checked against the counts before any pointer is published).
#include "common_track50cr.h"
#include "authored_runtime_ground50cp_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include "track_static_candidates50ig_fixture.h"
#include "track_animation_world50an.h"
#include "course_trigger_sources50ct.h"
#include "course_trigger_actions50ct.h"
#include "../../src/game/native_trigger_group50mc.h"
#include "../../src/game/native_trigger_retirement50mc.h"
#include <functional>
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace track_collision_world50CR {
namespace g=rr::game;namespace a=rr::assets;namespace rt=track_runtime_storage50IG;
namespace ct=common_track50CR;
namespace ta=course_trigger_actions50CT;
// Trigger collider identities in the one world: R5's numbering (never a guest address).
inline constexpr std::uint32_t firstTriggerIdentity=ta::firstTriggerIdentity,firstTriggerModelIdentity=ta::firstTriggerModelIdentity;
inline constexpr std::size_t animationCapacity=rr::assets::TrackAnimationLimits50AN{}.animations;
// The masks every course passes to the COL placement (4,16,32,64): one rule.
// PORT STAND-IN: port collision category masks (track_collider50ao), the same for every course.
inline constexpr a::TrackCollisionMasks50AN collisionMasks{4,16,32,64};

struct Payload {
 std::size_t groundDescriptors=0,candidateDescriptors=0,memberships=0;
 std::size_t storageRequested=0,totalRequested=0,independentMesh=0,independentRadii=0;
};
enum class Phase {Empty,Ground,Candidates,Storage,Memberships,AnimationPending,Ready,Retiring,Closed};
struct Diagnostic {Phase phase=Phase::Empty;std::size_t trigger=0;std::string message;rt::Diagnostic storage{};g::LiveZoneResult50EN membership{};Payload payload{};};

// The one collision-world lease the navigation store keeps (NavigationStorage::worldLease
// holds `keep`). It aliases the Owner, so close() refuses while any lease is alive.
struct Lease {
 std::shared_ptr<const void> keep;
 const g::LiveZoneWorld50EN* world=nullptr;
 std::uint64_t generation=0;
 explicit operator bool()const noexcept{return keep&&world&&generation;}
 // NavigationStorage::worldLease (R4) keeps exactly this.
 operator std::shared_ptr<const void>()const noexcept{return keep;}
};
// One racer query under the trigger action lease (frame, I4).
struct QueryLease {
 std::size_t slot=0;                       // the racer Bank slot being visited
 std::uint32_t main690=0;                  // the racer's main690 query identity
 std::uint32_t collider=0;                 // this query's collider (main690 or secondary identity)
 std::uint32_t* nodeC80=nullptr;           // the racer's C80 progress node (LostAI writes it)
 const g::Vector3_48C* listener=nullptr;   // accepted camera listeners (null before the first camera)
 std::uint32_t listenerCount=0;
 std::uint32_t* pilotNodeC80=nullptr;
};

// Storage inputs only the race's cold caller (I2) has: query names, the shared roster
// layout and the race's storage limits. The track supplies every count.
struct StorageRequest {
 std::shared_ptr<g::OriginalApplicationEntities50LA> application;
 // R5's one trigger action owner, prepared (source + group + destinations) but not yet bound
 // (null = the TOB authors no triggers). This world holds it and closes it at retirement.
 std::shared_ptr<ta::Owner> actions;
 std::shared_ptr<track_animation_runtime50AN::Owner> animations;    // track.roles.animation (null = no [Animations])
 track_static_candidates50IG::Bindings names{};
 rt::Limits limits{};
};

class Owner final : public std::enable_shared_from_this<Owner>,public track_animation_world50AN::World {
 const ct::Track* track_=nullptr;                       // the race's Track (outlives this owner: Track::release closes it)
 std::unique_ptr<authored_runtime_ground50cp_fixture::ColdScene> scene_;
 std::shared_ptr<ta::Owner> actions_;
 std::shared_ptr<track_animation_runtime50AN::Owner> animations_;
 track_animation_world50AN::Lane lane_;
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<rt::Binding> bindings_;
 ct::Block<std::unique_ptr<rt::Membership>> triggerMemberships_;
 std::unique_ptr<rt::Owner> storage_;
 std::size_t queryBorrows_=0,structuralBorrows_=0;
 std::uint64_t lastService_=0;
 bool queryBound_=false,contactsRetired_=false;
 std::size_t sourceCount_=0,triggerCount_=0,animationCount_=0,membershipCapacity_=0;
 const std::thread::id thread_=std::this_thread::get_id();
 bool groundAttempted_=false,gridPlaced_=false,storageAttempted_=false,publicationBlocked_=false,humanLiftBound_=false;
 Phase phase_=Phase::Empty;Payload payload_{};std::uint64_t generation_=0;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 static void need(bool value,const std::string& message){if(!value)throw std::runtime_error(message);}
 static void add(std::size_t& n,std::size_t v){need(v<=std::numeric_limits<std::size_t>::max()-n,"collision world payload addition overflow");n+=v;}
 template<class T>static void addElements(std::size_t& n,std::size_t count){need(count<=std::numeric_limits<std::size_t>::max()/sizeof(T),"collision world payload multiplication overflow");add(n,count*sizeof(T));}
 static std::string lower(std::string s){for(auto& c:s)if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');return s;}
 static bool hasMember(const ct::Track& track,const std::string& name)noexcept{
  try{return !name.empty()&&track.archive.exists(name);}catch(...){return false;}
 }
 // Decimal -> binary32 placement vector (the original's own parse of the instance block).
 static std::array<float,3> vector(const a::ParameterBlock48F& block,const char* key,const std::array<float,3>* absent=nullptr){
  const std::string* text=nullptr;
  for(const auto& field:block.fields)if(field.name==key){need(!text,"duplicate placement vector");text=&field.value;}
  if(!text&&absent)return *absent; // 27A5A0..27A5AC -> 158CC8: an absent UpVector reads as (0,1,0)
  need(text,"missing placement vector");std::array<float,3> out{};std::size_t begin=0;
  for(unsigned axis=0;axis<3;++axis){auto end=text->find(',',begin);
   need((end==std::string::npos)==(axis==2),"placement vector extent");
   if(end==std::string::npos)end=text->size();auto first=begin,last=end;
   while(first<last&&(*text)[first]==' ')++first;while(last>first&&(*text)[last-1]==' ')--last;
   need(a::parseTrackNumber50AD(text->data()+first,last-first,out[axis]),"placement numeric source");begin=end+1;
  }return out;
 }
 bool sameOwners()const noexcept {
  if(!thread()||!track_||!scene_)return false;
  if(triggerCount_&&(!actions_||!actions_->sourceOwner()||!actions_->nativeGroup()||actions_->triggerCount()!=triggerCount_))return false;
  return animations_||!animationCount_;
 }
 bool triggersIdle()const noexcept{return !actions_||actions_->queryIdle();}
 bool actualIdle()const noexcept {return thread()&&!queryBorrows_&&triggersIdle()&&
   (!storage_||!storage_->get()||!storage_->get()->queue.count);}
 bool currentPublication()const noexcept {
  if(!storage_||!storage_->get()||std::fegetround()!=FE_TONEAREST)return false;
  return lane_.current(*storage_->get());
 }
 std::size_t totalCount()const noexcept{return sourceCount_+triggerCount_+animationCount_;}
 bool idleForRetirement()const noexcept {
  if(!actualIdle()||!lane_.detached())return false;
  if(!storage_||!storage_->get())return true;const auto& s=*storage_->get();
  if(!s.rowsDetached())return false; //every racer's query rows (shared world: all of them)
  for(std::size_t i=totalCount();i<s.registrations.size();++i){const auto& r=s.registrations[i];if(r.owner||r.slot||r.encodedObject)return false;}
  return true;
 }
 bool removeTriggers()noexcept {
  if(!storage_||!storage_->get())return true;auto& s=*storage_->get();
  for(auto& member:triggerMemberships_.live())if(member){const auto r=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,member->state,s.membershipWork);
   if(r.status!=g::LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;}
  return true;
 }
 bool triggerMembershipsDetached()const noexcept {
  for(const auto& member:triggerMemberships_.live())if(member&&(member->state.attached||member->state.object))return false;
  return true;
 }
 bool retire()noexcept {
  // Trigger memberships must be removed in every Bank peer first (root's job); the
  // trigger group must be idle. Never clear shared hooks on the root's behalf here.
  if(structuralBorrows_||!idleForRetirement()||!triggerMembershipsDetached()||!removeTriggers())return false;
  // The trigger action owner closes before the world (every bank peer's membership removed).
  if(actions_){if(queryBound_){if(!actions_->unbindQuery())return false;queryBound_=false;}if(!actions_->close())return false;}
  generation_=0;phase_=Phase::Closed;groundAttempted_=storageAttempted_=true;
  storage_.reset();triggerMemberships_.release();lane_.reset();
  candidates_.clear();bindings_.clear();radii_.clear();actions_.reset();animations_.reset();scene_.reset();track_=nullptr;return true;
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
  // Registration-only alias. Dynamic queries require QueryBorrow.
  rt::Storage* storage()const noexcept{return registrationStorage();}
  g::LiveZoneWorld50EN* world()const noexcept{auto* s=registrationStorage();return s?&s->world:nullptr;}
  std::span<g::Type2StaticCandidate49X> candidateStorage()const noexcept{return *this?std::span<g::Type2StaticCandidate49X>{owner_->candidates_}:std::span<g::Type2StaticCandidate49X>{};}
  std::span<const rt::Binding> bindingStorage()const noexcept{return *this?std::span<const rt::Binding>{owner_->bindings_}:std::span<const rt::Binding>{};}
  rt::Membership* triggerMembership(std::size_t i)const noexcept{
   if(!*this)return nullptr;auto* slot=owner_->triggerMemberships_.at(i);return slot?slot->get():nullptr;}
  std::size_t triggerCount()const noexcept{return *this?owner_->triggerCount_:0;}
  // The one trigger action owner (R5; its group is 2A00A0/2A0E20/2A0B30).
  ta::Owner* actions()const noexcept{return *this?owner_->actions_.get():nullptr;}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner();   // common_track_load50cr.cpp (terminates if it would die while borrowed or attached)

 // 05.04 (271230/27BB40). One attempt. The Track must outlive this owner (Track::release
 // closes the collision role before the archive). Refuses rather than guessing.
 bool prepareGround(const ct::Track& track,std::string& error)noexcept {
  if(!thread()||groundAttempted_||weak_from_this().expired()){error="collision world requires one shared owner-thread attempt";return false;}
  groundAttempted_=true;phase_=Phase::Ground;
  try{
   need(track.archive.opened()&&track.archive.decoded()&&!track.documents.world.zones.empty(),"collision ground requires the opened archive and the decoded SCB");
   const auto& world=track.documents.world;
   auto next=std::make_unique<authored_runtime_ground50cp_fixture::ColdScene>();next->world=world;
   next->stackCount=1;next->lift=0.f;next->pose={};
   std::size_t count=0;for(const auto& instance:world.instances)if(hasMember(track,instance.collision_member))++count;
   need(count&&count<=authored_runtime_ground50cp_fixture::hardCollisionOwners50IG,
    "placed COL count is zero or exceeds the native collision owner store");
   next->owners.reserve(count);a::DecodeError48C decode;
   for(const auto& instance:world.instances){
    if(!hasMember(track,instance.collision_member))continue;
    need(next->owners.size()<count&&instance.parameter_block<world.parameters.blocks.size(),"placed COL source row");
    next->owners.emplace_back();auto& out=next->owners.back();
    out.name=instance.collision_member;out.zone=instance.zone;out.ordinal=instance.ordinal;
    std::vector<std::uint8_t> bytes,cache;a::Collision48G col;std::string why;ct::Diagnostic member;
    need(track.archive.member(out.name,bytes,8u*1024u*1024u,member)&&a::decodeCollision48G(bytes,col,decode),"placed COL decode: "+out.name);
    need(a::encodeCollisionAsset48G(col,cache,why)&&a::loadCollisionMesh48I(cache,out.mesh,why),"native COL mesh preparation: "+out.name);
    const auto& block=world.parameters.blocks[instance.parameter_block];
    static constexpr std::array<float,3> absentUp{0.f,1.f,0.f};
    const auto position=vector(block,"position"),look=vector(block,"lookvector"),up=vector(block,"upvector",&absentUp);
    need(a::prepareTrackCollider50AO(out.mesh,position,look,up,out.name.data(),out.name.size(),collisionMasks,out.instance),
     "COL placement/category preparation: "+out.name);
    next->stackCount=std::max(next->stackCount,out.mesh.nodes.size()+out.mesh.faces.size()+1);
   }
   need(next->owners.size()==count,"COL source order/extent changed");
   // 1A9EE0 fresh-game height descriptor from the SCB Environment: terrain330 is never
   // constructed on this path; water334 is present unless WaterFile is "none" (2745B0).
   std::string heightError;
   need(track_height_cold50JB::prepareFreshGameTrack(world,*track.archive.decoded(),track.identity.scbMember,next->height,heightError),
    "fresh height descriptor rejected: "+heightError);
   need((next->height.sourceDomain.water334==g::UndertrackHeightOwner50EW::Present)==track.hasWater(),
    "height water owner differs from the track's water data");
   sourceCount_=count;track_=&track;scene_=std::move(next);error.clear();return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
   catch(...){try{error="collision ground allocation or source preparation failed";}catch(...){}return false;}
 }
 // Grid centre pose (2A2880 input) from the map spline's first node and its predecessor,
 // and the selected racer's PHY lift. After step 08; before prepareStorage.
 // PORT STAND-IN: the port's cold grid centre (grid_center50ac) kept from the per-course grounds;
 // the original places at 2A2880 from the race start.
 bool placeGrid(const ct::NavigationView& map,unsigned gridRank,const std::vector<std::uint8_t>& selectedPhy,std::string& error)noexcept {
  if(!thread()||!scene_||gridPlaced_||storageAttempted_){error="grid placement requires the ground and one attempt";return false;}
  gridPlaced_=true;
  try{
   const auto* nodes=map.nodes();const auto count=map.count();
   need(nodes&&count&&count<=map.capacity()&&gridRank<=7,"grid rank/map bound");
   std::uint32_t previous=0;
   for(;previous<count;++previous){const auto& node=nodes[previous];
    need(node.nextCount&&node.nextCount<=g::maxUndertrackNext50ET,"grid predecessor edge");
    if(node.next[0]==0)break;}
   need(previous<count&&nodes[0].nextCount,"original grid predecessor");
   g::GridCenterInput50AC grid{};
   std::memcpy(grid.current,nodes[0].position,sizeof(grid.current));
   std::memcpy(grid.previous,nodes[previous].position,sizeof(grid.previous));
   grid.rank=gridRank;grid.nextNode=nodes[0].next[0];
   a::VisualAuthoredValues49R visual{};a::VisualAuthoringDiagnostic49R visualDiagnostic{};
   need(a::readVehicleVisualAuthoring49R(reinterpret_cast<const char*>(selectedPhy.data()),selectedPhy.size(),visual,visualDiagnostic),
    "selected-racer PHY");
   float lift=visual.values[static_cast<unsigned>(a::VisualAuthoredKey49R::MaxLiftHeight)];
   g::BodyMatrix48G pose{};
   need(g::prepareCenterPose50AC(grid,lift,{{0,1,0}},pose),"selected grid center pose");
   scene_->lift=lift;scene_->pose=pose;error.clear();return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
   catch(...){try{error="grid placement failed";}catch(...){}return false;}
 }
 // Storage, trigger memberships and the animation lane, sized from the live counts.
 bool prepareStorage(StorageRequest request,Diagnostic& d)noexcept {
  if(!thread()||storageAttempted_||!scene_||!track_)return false;storageAttempted_=true;phase_=Phase::Candidates;
  try{d={};d.phase=phase_;need(request.application&&request.application->application10(),"actual live Application required");
   if(request.actions)need(request.actions->sourceOwner()&&request.actions->preparedSourceFor(*request.actions->sourceOwner())&&
     request.actions->nativeGroup()&&!request.actions->nativeGroup()->ready&&request.actions->queryIdle()&&
     request.actions->animationOwner()==request.animations.get(),"prepared trigger action owner (same animation runtime) required");
   need(!request.animations||request.animations->ready(),"track animation runtime not ready");
   triggerCount_=request.actions?request.actions->triggerCount():0;
   animationCount_=track_animation_world50AN::Lane::collisionRowCount(request.animations.get());
   need(animationCount_<=animationCapacity,"animation collision rows exceed the reader's limit");
   membershipCapacity_=track_->documents.world.portals.size()+1; // track_runtime_storage50IG: portals + 1
   need(totalCount()<=rt::hardTerrainOwners,"placed COL + triggers + animation rows exceed the terrain owner store");
   actions_=std::move(request.actions);animations_=std::move(request.animations);
   need(sameOwners(),"placed ground / trigger / animation join required");
   candidates_.reserve(totalCount());bindings_.reserve(totalCount());radii_.reserve(sourceCount_);
   track_static_candidates50IG::prepare(*scene_,request.names,candidates_,radii_,bindings_);
   need(candidates_.size()==sourceCount_&&bindings_.size()==sourceCount_,"static candidates of the placed COL owners");
   if(actions_)need(ta::prepareCandidates(*actions_,candidates_,bindings_,rt::hardTerrainOwners,request.names.collisionTypeTag,
     request.names.preservedFlags31,request.names.preservedFlagsF8)==sourceCount_,"trigger candidates rejected");
   need(candidates_.size()==sourceCount_+triggerCount_&&bindings_.size()==candidates_.size(),"placed COL + trigger candidate prefix");
   need(lane_.prepareCandidates(animations_,candidates_,bindings_,request.names.collisionTypeTag,request.names.preservedFlags31,request.names.preservedFlagsF8)==sourceCount_+triggerCount_&&
     candidates_.size()==totalCount()&&bindings_.size()==totalCount()&&lane_.count()==animationCount_,"track-animation candidate lane rejected");
   // Payload of this owner's descriptors (charged against the race's storage limit).
   Payload p{};addElements<g::Type2StaticCandidate49X>(p.candidateDescriptors,candidates_.capacity());
   addElements<rt::Binding>(p.candidateDescriptors,bindings_.capacity());addElements<track_static_candidates50IG::Radii>(p.candidateDescriptors,radii_.capacity());
   addElements<rt::Membership>(p.memberships,triggerCount_);addElements<g::CollectionZoneLink50BX>(p.memberships,membershipCapacity_*triggerCount_);
   addElements<g::LiveZoneEntry50EN>(p.memberships,membershipCapacity_*triggerCount_);
   add(p.memberships,track_animation_world50AN::Lane::payloadBytes(animationCount_,membershipCapacity_));
   for(const auto& owner:scene_->owners){const auto& m=owner.mesh;
    p.independentMesh+=m.vertices.capacity()*sizeof(m.vertices[0])+m.vertex_normals.capacity()*sizeof(m.vertex_normals[0])+
     m.nodes.capacity()*sizeof(m.nodes[0])+m.faces.capacity()*sizeof(m.faces[0]);}
   for(const auto& radius:radii_)addElements<float>(p.independentRadii,radius.values.capacity());
   need(p.candidateDescriptors+p.memberships<request.limits.coldPayloadBytes,"collision world descriptors exceed the race storage limit");
   auto reduced=request.limits;reduced.coldPayloadBytes-=p.candidateDescriptors+p.memberships;
   payload_=p;d.payload=p;
   phase_=Phase::Storage;d.phase=phase_;storage_=std::make_unique<rt::Owner>();
   if(!storage_->prepare(std::move(request.application),*scene_,bindings_.data(),bindings_.size(),reduced,d.storage)){d.message=d.storage.message;return false;}
   auto& s=*storage_->get();need(s.count.terrain==totalCount()&&s.count.zones==track_->documents.world.zones.size()&&
     s.count.portals==track_->documents.world.portals.size()&&s.count.portalLinks==2*s.count.portals&&
     s.count.membership==membershipCapacity_,"Storage extent differs from the course data");
   payload_.storageRequested=d.storage.requestedPayloadBytes;payload_.totalRequested=p.candidateDescriptors+p.memberships;add(payload_.totalRequested,payload_.storageRequested);
   d.payload=payload_;phase_=Phase::Memberships;d.phase=phase_;
   // One membership per live trigger, sized once from the trigger count (bounds-checked block).
   need(triggerMemberships_.size(ct::BlockId::triggerMemberships,triggerCount_,0x274E90u),"trigger membership block refused");
   for(std::size_t i=0;i<triggerCount_;++i){auto* slot=triggerMemberships_.slot(i,0x274E90u);need(slot,"trigger membership slot refused");
    *slot=std::make_unique<rt::Membership>(membershipCapacity_);}
   for(std::size_t i=0;i<triggerCount_;++i){d.trigger=i;const auto& actual=actions_->sourceOwner()->source(i);auto& member=**triggerMemberships_.at(i);const auto bounds=actions_->bounds(i);
    d.membership=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member.state,member.storage(),static_cast<g::CollectionRef50BX>(sourceCount_+i+1),actual.zone,g::ZoneMethod50CL::First,0.f,bounds,s.membershipWork);
    need(d.membership.status==g::LiveZoneStatus50EN::Complete,"trigger attachment rejected");
    d.membership=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,member.state,bounds,s.membershipWork);need(d.membership.status==g::LiveZoneStatus50EN::Complete,"trigger publication rejected");
   }
   lane_.makeMemberships(membershipCapacity_);
   ct::log::trackDataNamed("collisions.candidates",track_->code(),track_->identity.scbMember,totalCount(),false,0);
   // The track-animation binding publishes the lane next; with no [Animations] the world is complete here.
   if(animations_){phase_=Phase::AnimationPending;d.phase=phase_;d.message.clear();return true;}
   if(!currentPublication())throw std::runtime_error("collision world publication without animations rejected");
   if(!generation_)generation_=1;publicationBlocked_=false;phase_=Phase::Ready;d.phase=phase_;d.message.clear();return true;
  }catch(const std::exception& e){try{d.phase=phase_;d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.phase=phase_;d.message="collision world allocation/assembly failure";}catch(...){}return false;}
 }
 bool geometryReady()const noexcept{return phase_==Phase::Ready&&generation_&&!publicationBlocked_&&sameOwners()&&storage_&&storage_->get()&&
   (!actions_||!actions_->nativeGroup()||!actions_->nativeGroup()->actionsFailed)&&currentPublication();}
 // Dynamic readiness: the trigger action owner's destination admission (when the course has triggers).
 bool queryReady()const noexcept{return geometryReady()&&(!actions_||(queryBound_&&actions_->ready()));}
 // Publishes query readiness once the trigger action owner has the race field's identities
 // (actions->bindRacerIdentities, I2 after step 10). No triggers: nothing to bind.
 bool bindQuery()noexcept {
  if(!thread()||queryBound_||!geometryReady())return false;
  if(actions_&&!actions_->bindQuery())return false;
  queryBound_=true;return true;
 }
 bool beginRetirement()noexcept {
  if(!actualIdle()||phase_==Phase::Closed)return false;
  if(actions_&&queryBound_){if(!actions_->unbindQuery())return false;}
  queryBound_=false;publicationBlocked_=true;phase_=Phase::Retiring;return true;
 }
 // ONE synchronous indexed query of one racer collider under the trigger action lease
 // (2A0B30 dispatch runs from inside the query). op(track_runtime_storage50IG::Storage&)->bool.
 // bank: the race's racer Bank (native_racer50HN::Bank). The query must run inside the caller's
 // single visit of lease.slot; for its whole span the Bank's one query fence is up (queryActive_,
 // queryIssuer_=this: no read, bind, visit or nested query), cleared on every exit after the
 // trigger lease ends (generalised from mc_query50MC::Binding::query).
 template<class RacerBank,class Operation> bool queryRacer(RacerBank& bank,const QueryLease& lease,Operation&& op)noexcept {
  if(!queryReady())return false;
  if(bank.retired_||!bank.sealed_||lease.slot>=bank.count_||bank.queryActive_||bank.queryIssuer_||bank.terminalBlocked_||
    bank.visitDepth_!=1||bank.currentSlot_!=lease.slot||bank.pendingToken_||bank.retirementVisit_||
    bank.animationVisit_||bank.animationBlocked_)return false;
  auto borrow=query();if(!borrow)return false;auto* storage=borrow.storage();if(!storage)return false;
  struct Fence {RacerBank& b;~Fence(){b.queryActive_=false;b.queryIssuer_=nullptr;}};
  bank.queryActive_=true;bank.queryIssuer_=this;Fence fence{bank};
  if(!actions_){try{return std::invoke(std::forward<Operation>(op),*storage);}catch(...){return false;}}
  ta::ActionLease actual{lease.main690,lease.collider,lease.nodeC80,lease.listener,lease.listenerCount,lease.pilotNodeC80};
  ta::Owner::QueryBorrow scope(*actions_,actual);if(!scope)return false;
  bool accepted=false;
  try{accepted=std::invoke(std::forward<Operation>(op),*storage);}catch(...){accepted=false;}
  return scope.finish(accepted)&&accepted;
 }
 // A committed prefix's prepared root shape for this racer collider (the exit service reads it).
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,const g::BodyMatrix48G& world)noexcept {
  if(!thread()||!queryReady())return false;
  return !actions_||actions_->observePrepared(collider,resource,world);
 }
 // 2A0B30: the trigger exit service over the committed poses, once per world step.
 bool serviceWorld(std::uint64_t rootStep)noexcept {
  if(!queryReady()||!actualIdle()||!rootStep||rootStep<=lastService_)return false;
  lastService_=rootStep;
  return !actions_||actions_->serviceWorld()==ta::Status::Complete;
 }
 // Teardown, after every racer row is detached and the world is Retiring: validate and clear
 // every reciprocal trigger/collider contact pair (mc_query50MC retireContacts), so the action
 // owner can close. Idempotent.
 bool retireContacts()noexcept {
  if(contactsRetired_||!actions_)return contactsRetired_=true;
  if(!thread()||phase_!=Phase::Retiring||!actualIdle()||!storage_||!storage_->get()||!storage_->get()->rowsDetached())return false;
  auto* group=actions_->nativeGroup();if(!group)return false;
  const auto validation=g::validateNativeTriggerTerminalGraph50MC(*group);if(!validation.valid)return false;
  std::size_t removed=0;
  for(auto& trigger:group->triggers)for(auto& id:trigger.collider.membersCC)if(id){
   for(auto& collider:group->colliders)if(collider.identity==id)
    for(auto& reciprocal:collider.membersCC)if(reciprocal==trigger.collider.identity){reciprocal=0;id=0;++removed;break;}
  }
  for(const auto& trigger:group->triggers)for(auto id:trigger.collider.membersCC)if(id)return false;
  for(const auto& collider:group->colliders)for(auto id:collider.membersCC)if(id)return false;
  if(removed!=validation.pairs)return false;
  return contactsRetired_=true;
 }
 // The same teardown with the racer Bank's one terminal fence (generalised from
 // mc_query50MC::retireContacts, every course): the Bank is closed to normal admission
 // (terminalBlocked_, left set) and one bounded internal all-peer sweep (terminalVisit_ for
 // its span) detaches every racer query row in this world and removes every trigger
 // membership, stopped peers included; then the contact pairs above. Idempotent.
 template<class RacerBank> bool retireContacts(RacerBank& bank)noexcept {
  if(contactsRetired_)return true;
  if(!thread()||phase_!=Phase::Retiring||!actualIdle()||!storage_||!storage_->get()||structuralBorrows_||
    !lane_.detached()||bank.terminalVisit_||bank.queryActive_||bank.visitDepth_)return false;
  auto& s=*storage_->get();
  bank.terminalBlocked_=true;
  try{
   bank.terminalVisit_=true;
   struct End {RacerBank& b;~End(){b.terminalVisit_=false;}}end{bank};
   if(!bank.visitAll([&](std::size_t){
     if(s.queue.count)return false;
     // Every racer query row in the one world (idempotent across peers; the shared rows
     // are not banked, so the first peer's sweep detaches them all).
     if(!s.detachAllRows())return false;
     if(!removeTriggers())return false;
     for(std::size_t i=totalCount();i<s.registrations.size();++i){const auto& r=s.registrations[i];if(r.owner||r.slot||r.encodedObject)return false;}
     return true;
    }))return false;
  }catch(...){return false;}
  return retireContacts();
 }
 Phase phase()const noexcept{return phase_;}const Payload& payload()const noexcept{return payload_;}
 const authored_runtime_ground50cp_fixture::ColdScene* ground()const noexcept{return thread()?scene_.get():nullptr;}
 std::size_t sourceCount()const noexcept{return sourceCount_;}
 std::size_t triggerCount()const noexcept{return triggerCount_;}
 std::size_t animationCount()const noexcept{return animationCount_;}
 std::size_t candidateCount()const noexcept{return totalCount();}
 // The contexts' triggerMembership block (sized once at prepareStorage from the live trigger count).
 ct::Block<std::unique_ptr<rt::Membership>>& triggerMemberships()noexcept{return triggerMemberships_;}
 // Called once from the real racer0 rig/class installation, before grounded grid placement.
 bool bindHumanLift(float lift)noexcept{
  if(!thread()||!scene_||humanLiftBound_||!std::isfinite(lift))return false;
  scene_->lift=lift;humanLiftBound_=true;return true;
 }
 Borrow borrow()noexcept {if(!geometryReady())return {};auto self=weak_from_this().lock();return self?Borrow{std::move(self)}:Borrow{};}
 // Cleanup-only issuance after a failed cold/live prefix. It cannot satisfy geometryReady/queryReady.
 Borrow retirementBorrow()noexcept {
  if(phase_!=Phase::Retiring||!actualIdle()||!storage_||!storage_->get())return {};
  auto self=weak_from_this().lock();return self?Borrow{std::move(self)}:Borrow{};
 }
 QueryBorrow query()noexcept {if(!queryReady()||!triggersIdle())return {};auto self=weak_from_this().lock();return self?QueryBorrow{std::move(self)}:QueryBorrow{};}
 Lease lease()const noexcept {
  if(!geometryReady())return {};auto self=weak_from_this().lock();if(!self)return {};
  const auto* world=&storage_->get()->world;
  return Lease{std::shared_ptr<const void>(self,world),world,generation_};
 }
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
// After prepareStorage: publish Track::roles.storage as an alias of the collision world's
// one Storage (the alias keeps the collision world alive; Track::release drops it first).
inline bool publishStorageRole(ct::Track& track)noexcept {
 auto* collision=track.roles.collision.get();
 if(!collision||track.roles.storage)return false;
 auto borrow=collision->borrow();auto* storage=borrow?borrow.registrationStorage():nullptr;
 if(!storage)return false;
 track.roles.storage=std::shared_ptr<rt::Storage>(track.roles.collision,storage);
 track.counts.storageBytes=collision->payload().totalRequested;
 ct::log::storage("collision_world",collision->candidateCount(),collision->payload().totalRequested,rt::hardTerrainOwners);
 return true;
}
} // namespace track_collision_world50CR

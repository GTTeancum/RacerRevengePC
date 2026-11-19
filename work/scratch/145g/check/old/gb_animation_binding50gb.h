#pragma once
// SOURCE DRAFT HOLD: complete aggregate requires genuine GB trigger actions.
// Runtime and Bank proposals are separately pinned; this is no ready facade.
#include "gb_animation_runtime50gb.h"
#include "gb_animation_sphere50gb.h"
#include "gb_collision_world50gb.h"
#include "native_racer_bank50hn_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <algorithm>
#include <iterator>
#include <thread>
#include <vector>
namespace gb_animation_binding50GB {
namespace g=rr::game;namespace lighting=g::original_gc_gameplay_lighting_candidate;
namespace runtime=gb_animation_runtime50GB;namespace rt=track_runtime_storage50IG;
struct Diagnostic {std::string message;std::uint32_t sourcePC=0;std::size_t owner=0,peer=0;g::LiveZoneResult50EN registry{};runtime::Diagnostic runtime{};};
class Binding final {
 std::shared_ptr<runtime::Owner> runtime_;
 std::shared_ptr<lighting::Owner> lighting_;
 std::shared_ptr<gb_collision_world50GB::Owner> collision_;
 struct Visual {
  std::array<g::CollectionZoneLink50BX,18> links{};std::array<g::LiveZoneEntry50EN,18> entries{};
  g::LiveZoneMembership50EN membership{};gb_animation_sphere50GB::Result sphere{};bool claimed=false;
 };
 std::array<Visual,runtime::ownerCount> visual_{};
 //Private successful-publication observations. No caller can mint one and
 //they never select an arena; every check still uses actual Bank backing.
 std::array<g::LiveZoneMembership50EN,runtime::ownerCount> published_{};
 native_racer50HN::Bank* bank_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false,failed_=false,servicing_=false,bankActivated_=false;
 bool fail(Diagnostic& d,const char* message,std::uint32_t pc)noexcept {
  failed_=true;ready_=false;if(runtime_)runtime_->publicationReady_=false;if(collision_)collision_->publicationBlocked_=true;
  if(bank_)bank_->gbAnimationBlocked_=true;
  d.sourcePC=pc;try{d.message=message;}catch(...){}return false;
 }
 bool runtimeFailure(Diagnostic& d)noexcept {return fail(d,d.runtime.message.c_str(),d.runtime.sourcePC);}
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 bool visualBounds(std::size_t i,g::LiveZoneBounds50EN& out,Diagnostic& d)noexcept {
  if(!runtime_||i>=runtime::ownerCount||!runtime_->source_||!runtime_->source_->ready())return fail(d,"GB actual visual source unavailable",0x26f4b0);
  auto& state=runtime_->state_[i];const auto& source=runtime_->source(i);const auto graph=runtime_->graph(i);
  if(!(state.nodes[0].sceneFlags190&0x10u)||(state.nodes[0].sceneFlags190&4u)||!g::refreshSceneWorld49P(graph,static_cast<runtime::Index>(0)))
   return fail(d,"GB source aggregate sphere root rejected",0x26f4b0);
  std::array<gb_animation_sphere50GB::Row,gb_animation_source50GB::maximumPsgNodes> rows{};
  for(std::uint32_t row=0;row<source.hierarchy.nodeCount;++row){rr::assets::PsgNode49O original{};
   if(!(state.nodes[row+1].sceneFlags190&4u)||!rr::assets::readPsgNode49O(source.hierarchy,row,original)||
       !g::refreshSceneWorld49P(graph,static_cast<runtime::Index>(row+1)))return fail(d,"GB actual PSG visual sphere row rejected",0x28d940);
   rows[row].world=state.nodes[row+1].world100;for(unsigned k=0;k<3;++k)rows[row].center[k]=original.bounds[k];rows[row].radius=original.bounds[6];
  }
  gb_animation_sphere50GB::Result next{};
  if(!gb_animation_sphere50GB::compute(state.nodes[0].world100,rows.data(),source.hierarchy.nodeCount,next))return fail(d,"GB nonzero-root sphere arithmetic rejected",0x28bd50);
  visual_[i].sphere=next;if(next.radius198>0.f)state.nodes[0].sceneFlags190|=0x40u;
  out={};out.bounds.shapeTag=1;out.bounds.shapeWorld=state.nodes[0].world100;
  for(unsigned k=0;k<3;++k){out.bounds.localCenter.v[k]=next.center160[k];out.bounds.localHalf.v[k]=next.radius198;}
  out.bounds.cachedRadius=next.radius198;g::CollectionBoundsResult48S actual{};
  if(g::prepareCollectionBounds48S(out.bounds,actual)!=g::CollectionBoundsStatus48S::Complete)return fail(d,"GB real observer bounds rejected",0x28e230);
  out.bounds.cachedCenter=actual.center;return true;
 }
 bool updateVisual(std::size_t i,g::ResetVisibilityWorld50EY& world,g::LiveZoneWorkspace50EN& work,
      g::CollectionRef50BX object,Diagnostic& d)noexcept {
  auto& m=visual_[i].membership;if(!m.attached||m.object!=object)return fail(d,"GB real visual membership identity changed",0x28ea20);
  g::LiveZoneBounds50EN bounds{};if(!visualBounds(i,bounds,d))return false;
  d.registry=g::updateLiveZoneMembership50EN(world.lifecycle,world.portals,m,bounds,work);
  return d.registry.status==g::LiveZoneStatus50EN::Complete||fail(d,"GB actual visual publication rejected",0x28ea20);
 }
 bool bankIdle(bool sealed)const noexcept {
  return bank_&&bank_->gbAnimationIssuer_==this&&!bank_->retired_&&bank_->sealed_==sealed&&
   !bank_->visiting_&&!bank_->readBlocked_&&!bank_->visitDepth_&&!bank_->pendingToken_&&!bank_->retirementVisit_&&
   !bank_->nativeFifoPins_&&!bank_->gbAnimationVisit_&&(!sealed||bank_->count_>=2);
 }
 //Host-side sorted copy of the sealed Bank's span list, built once at cold
 //activation. A sealed Bank's spans never change (add() refuses after seal),
 //so lookups give the same answers as the full scans, in log time. Used only
 //while it still describes the very same sealed span list; otherwise scan.
 struct SortedSpan {std::uintptr_t first;std::size_t bytes;};
 std::vector<SortedSpan> sortedSpans_;
 const native_racer50HN::Bank* sortedBank_=nullptr;const void* sortedData_=nullptr;
 bool sortedCurrent()const noexcept {
  return bank_&&bank_->sealed_&&sortedBank_==bank_&&sortedData_==static_cast<const void*>(bank_->spans_.data())&&
   sortedSpans_.size()==bank_->spans_.size()&&!sortedSpans_.empty();
 }
 void buildSortedSpans() {
  sortedSpans_.clear();sortedBank_=nullptr;sortedData_=nullptr;
  if(!bank_||!bank_->sealed_)return;
  sortedSpans_.reserve(bank_->spans_.size());
  for(const auto& s:bank_->spans_)sortedSpans_.push_back({reinterpret_cast<std::uintptr_t>(s.address),s.bytes});
  std::sort(sortedSpans_.begin(),sortedSpans_.end(),[](const SortedSpan& a,const SortedSpan& b){return a.first<b.first;});
  //Bank::add() already refuses overlap; confirm before relying on ordering.
  for(std::size_t i=1;i<sortedSpans_.size();++i)
   if(sortedSpans_[i-1].first+sortedSpans_[i-1].bytes>sortedSpans_[i].first){sortedSpans_.clear();return;}
  sortedBank_=bank_;sortedData_=bank_->spans_.data();
 }
 bool exactSpan(const void* p,std::size_t bytes)const noexcept {
  if(!bytes)return true;
  if(sortedCurrent()){const auto first=reinterpret_cast<std::uintptr_t>(p);
   const auto it=std::lower_bound(sortedSpans_.begin(),sortedSpans_.end(),first,
       [](const SortedSpan& s,std::uintptr_t v){return s.first<v;});
   return it!=sortedSpans_.end()&&it->first==first&&it->bytes==bytes;}
  for(const auto& s:bank_->spans_)if(s.address==p&&s.bytes==bytes)return true;return false;
 }
 bool disjointBank(const void* p,std::size_t bytes)const noexcept {
  if(!bytes)return true;const auto first=reinterpret_cast<std::uintptr_t>(p);
  if(!p||bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
  if(sortedCurrent()){
   //Last span starting before p+bytes is the only one that can overlap:
   //spans are disjoint, so their ends are ordered like their starts.
   const auto it=std::lower_bound(sortedSpans_.begin(),sortedSpans_.end(),first+bytes,
       [](const SortedSpan& s,std::uintptr_t v){return s.first<v;});
   if(it==sortedSpans_.begin())return true;
   const auto& s=*std::prev(it);return !(first<s.first+s.bytes);}
  for(const auto& s:bank_->spans_){const auto other=reinterpret_cast<std::uintptr_t>(s.address);
   if(first<other+s.bytes&&other<first+bytes)return false;}return true;
 }
 template<class T>bool registered(const T& v)const noexcept {return exactSpan(&v,sizeof v);}
 template<class T>bool registeredBuffer(const rt::FixedBuffer<T>& v)const noexcept {return exactSpan(v.data(),v.size()*sizeof(T));}
 //Stage B shared world (every course with a roster): the one World, its
 //trigger/animation memberships and every candidate lane must be ABSENT from
 //the racer Bank; the pose is published once into the one world. This is the
 //inverse of the legacy per-racer-copy enrollment below (kept for the legacy
 //layout only, which has no racer rows).
 bool sharedBackingAbsent()const noexcept {
  const auto& s=*collision_->storage_->get();
  auto buf=[&](const auto& v){return disjointBank(v.data(),v.size()*sizeof(*v.data()));};
  auto one=[&](const auto& v){return disjointBank(&v,sizeof v);};
  if(!one(s.lifecycle)||!buf(s.registries)||!buf(s.registrations)||!buf(s.blocks)||!buf(s.trees)||!buf(s.zones)||!one(s.queue)||!buf(s.events)||
     !disjointBank(s.world.portalHeads,s.world.zoneCount*sizeof(g::CollectionRef50BX))||
     !disjointBank(s.world.portalLinks,s.world.portalLinkCount*sizeof(g::LiveZonePortalLink50EN))||
     !disjointBank(s.world.portals,s.world.portalCount*sizeof(g::PortalGeometry50EO))||
     !buf(s.objects)||!buf(s.source)||!buf(s.treeSource))return false;
  for(std::size_t i=0;i<s.count.terrain;++i){const auto& movement=s.movement[i].collision;const auto& target=collision_->candidates_[i].candidate.collision;
   if(!one(movement.filter.flagsF8)||!one(movement.copied40)||!one(movement.copied50)||
      !one(target.filter.flagsF8)||!one(target.copied40)||!one(target.copied50))return false;}
  for(const auto& m:collision_->triggerMemberships_)if(!m||!one(m->state)||!buf(m->links)||!buf(m->entries))return false;
  for(const auto& m:collision_->animationMemberships_)if(!m||!one(m->state)||!buf(m->links)||!buf(m->entries))return false;
  return disjointBank(runtime_.get(),sizeof(runtime::Owner))&&disjointBank(runtime_->shared_,sizeof(*runtime_->shared_))&&
    disjointBank(runtime_->mesh_.vertices.data(),runtime_->mesh_.vertices.size()*sizeof(rr::assets::Vertex48B))&&
    disjointBank(runtime_->mesh_.indices.data(),runtime_->mesh_.indices.size()*sizeof(std::uint32_t))&&disjointBank(this,sizeof(Binding));
 }
 bool sharedWorld()const noexcept {return collision_&&collision_->storage_&&collision_->storage_->get()&&collision_->storage_->get()->sharedLayout();}
 bool bankBacking(bool withAnimations)const noexcept {
  if(!bank_||!collision_||!collision_->storage_||!collision_->storage_->get()||!runtime_)return false;const auto& s=*collision_->storage_->get();
  if(s.sharedLayout())return sharedBackingAbsent();
  //Full actual Storage registration, not a caller-created receipt or count.
  if(!registered(s.lifecycle)||!registeredBuffer(s.registries)||!registeredBuffer(s.registrations)||!registeredBuffer(s.blocks)||
     !registeredBuffer(s.trees)||!registeredBuffer(s.zones)||!registered(s.queue)||!registeredBuffer(s.events)||
     !exactSpan(s.world.portalHeads,s.world.zoneCount*sizeof(g::CollectionRef50BX))||
     !exactSpan(s.world.portalLinks,s.world.portalLinkCount*sizeof(g::LiveZonePortalLink50EN))||
     !exactSpan(s.world.portals,s.world.portalCount*sizeof(g::PortalGeometry50EO))||
     !registeredBuffer(s.objects)||!registeredBuffer(s.source)||!registeredBuffer(s.treeSource)||!registered(s.collectionInput)||
     !registeredBuffer(s.registeredIndices)||!registered(s.registeredCount)||!registeredBuffer(s.probeIndices)||!registered(s.probeCount))return false;
  for(const auto* m:{&s.legacyMainMembership,&s.legacySecondaryMembership,&s.legacyTerrainMembership})
   if(!registered(m->state)||!registeredBuffer(m->links)||!registeredBuffer(m->entries))return false;
  for(std::size_t i=0;i<s.count.terrain;++i){const auto& movement=s.movement[i].collision;const auto& target=collision_->candidates_[i].candidate.collision;
   if(!registered(movement.filter.flagsF8)||!registered(movement.copied40)||!registered(movement.copied50)||
      !registered(target.filter.flagsF8)||!registered(target.copied40)||!registered(target.copied50))return false;
   if(!disjointBank(&target.geometry,sizeof target.geometry)||!disjointBank(&target.sphere,sizeof target.sphere)||
      !disjointBank(&movement.geometry,sizeof movement.geometry)||!disjointBank(&movement.sphere,sizeof movement.sphere))return false;
  }
  for(const auto& m:collision_->triggerMemberships_)if(!m||!registered(m->state)||!registeredBuffer(m->links)||!registeredBuffer(m->entries))return false;
  if(withAnimations)for(const auto& m:collision_->animationMemberships_)if(!m||!registered(m->state)||!registeredBuffer(m->links)||!registeredBuffer(m->entries))return false;
  //Global graph, playback, histories, source pointers and render streams may
  //never be restored per racer. A pose is sampled once, then all peers publish.
  return disjointBank(runtime_.get(),sizeof(runtime::Owner))&&disjointBank(runtime_->shared_,sizeof(*runtime_->shared_))&&
    disjointBank(runtime_->mesh_.vertices.data(),runtime_->mesh_.vertices.size()*sizeof(rr::assets::Vertex48B))&&
    disjointBank(runtime_->mesh_.indices.data(),runtime_->mesh_.indices.size()*sizeof(std::uint32_t))&&disjointBank(this,sizeof(Binding));
 }
 bool memberIdentity(std::size_t i)const noexcept {
  if(!collision_||i>=runtime::ownerCount||!collision_->animationMemberships_[i])return false;
  const auto& s=*collision_->storage_->get();const auto& m=collision_->animationMemberships_[i]->state;
  // Object reference from the aggregate's live extents (stock 634..637).
  const auto object=collision_->animationObject(i);
  if(!m.attached||m.object!=object||m.method!=g::ZoneMethod50CL::Second||m.storage.capacity!=18||!m.storage.entries)return false;
  const auto& expected=published_[i];
  if(m.revision!=expected.revision||m.lastShapeTag!=expected.lastShapeTag||m.lastType2Resource!=expected.lastType2Resource||
    m.storage.links!=expected.storage.links||m.storage.entries!=expected.storage.entries||
    std::memcmp(m.lastShapeWorld.v,expected.lastShapeWorld.v,sizeof m.lastShapeWorld.v)||
    std::memcmp(m.bounds.center.v,expected.bounds.center.v,sizeof m.bounds.center.v)||
    std::memcmp(m.bounds.half.v,expected.bounds.half.v,sizeof m.bounds.half.v)||
    std::memcmp(m.bounds.cachedCenter.v,expected.bounds.cachedCenter.v,sizeof m.bounds.cachedCenter.v)||
    std::memcmp(&m.bounds.cachedRadius,&expected.bounds.cachedRadius,sizeof(float)))return false;
  const auto& p=m.storage.entries[0].registration;const auto& r=s.registrations[object-1];
  return p.owner&&r.owner==p.owner&&r.slot==p.slot&&r.encodedObject==p.encodedObject&&r.encodedObject==(object<<2u)&&
    s.candidates[object-1]==runtime_->candidates_[i]&&runtime_->candidates_[i]->compound==&runtime_->views_[i];
 }
 template<class F>bool allPeers(F&& operation,Diagnostic& d) {
  if(!bankIdle(true)||!bankBacking(true))return fail(d,"GB actual complete idle Bank backing unavailable",0x120230);
  //Shared world: one World, one publication, outside every racer visit.
  if(sharedWorld()){d.peer=0;return operation(std::size_t{0});}
  bank_->gbAnimationVisit_=true;
  struct End{bool& flag;~End(){flag=false;}}end{bank_->gbAnimationVisit_};
  return bank_->visitAll([&](std::size_t peer){d.peer=peer;return operation(peer);});
 }
public:
 Binding()=default;Binding(const Binding&)=delete;Binding&operator=(const Binding&)=delete;Binding(Binding&&)=delete;Binding&operator=(Binding&&)=delete;
 ~Binding(){if(needsClose()||!thread())std::terminate();}
 static constexpr std::size_t requiredPayloadBytes()noexcept{return sizeof(Binding);}
 static constexpr std::size_t bankAdditionalBytesPerRacer()noexcept {
  return runtime::ownerCount*(sizeof(g::LiveZoneMembership50EN)+18*sizeof(g::CollectionZoneLink50BX)+18*sizeof(g::LiveZoneEntry50EN));
 }
 bool needsClose()const noexcept {if(bank_||collision_||(runtime_&&runtime_->binding_==this))return true;for(const auto& v:visual_)if(v.claimed)return true;return false;}
 bool ready()const noexcept {
  if(!thread()||!ready_||failed_||servicing_||!runtime_||!runtime_->ready()||!collision_||!collision_->geometryReady()||!lighting_)return false;
  if(bank_&&(!bankActivated_||!bankIdle(true)||bank_->gbAnimationBlocked_))return false;
  for(std::size_t i=0;i<runtime::ownerCount;++i)if(!visual_[i].claimed||!lighting_->currentGbAnimationOwner(this,i))return false;return true;
 }
 bool failed()const noexcept{return failed_;}
 const runtime::Owner* runtimeOwner()const noexcept{return runtime_.get();}
 const lighting::Owner* lightingOwner()const noexcept{return lighting_.get();}
 bool beginVisual(std::shared_ptr<runtime::Owner> runtime,std::shared_ptr<lighting::Owner> lighting,Diagnostic& d)noexcept {
  std::size_t active=runtime::ownerCount;
  try{d={};if(attempted_||!thread())return fail(d,"GB fresh creating-thread Binding required",0x2d935c);attempted_=true;
   if(!runtime||!lighting||!runtime->stagedSourceReady()||runtime->binding_||!runtime->shared_||!lighting->world()||
      !runtime->coldSource()||lighting->world()->parameters.original_bytes!=runtime->coldSource()->world().parameters.original_bytes)
    return fail(d,"GB same source/lighting/staged graph identity rejected",0x2d935c);
   runtime_=std::move(runtime);lighting_=std::move(lighting);runtime_->binding_=this;
   for(std::size_t i=0;i<runtime::ownerCount;++i){d.owner=i;auto& v=visual_[i];
    if(!lighting_->bindGbAnimationOwner(this,i,&v.membership))return fail(d,"GB real visual issuer claim rejected",0x28e8a0);v.claimed=true;
    g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
    if(!lighting_->beginGbAnimationAccess(this,i,world,work,object))return fail(d,"GB visual prefix access unavailable",0x28e8a0);active=i;
    g::LiveZoneBounds50EN bounds{};if(!visualBounds(i,bounds,d)){lighting_->endGbAnimationAccess(this,i);return false;}
    d.registry=g::attachLiveZoneMembership50EN(world->lifecycle,world->portals,v.membership,
     {v.links.data(),v.entries.data(),v.links.size()},object,runtime_->source(i).authored.startingZone,g::ZoneMethod50CL::Second,0.f,bounds,*work);
    if(d.registry.status!=g::LiveZoneStatus50EN::Complete){lighting_->endGbAnimationAccess(this,i);return fail(d,"GB real visual prefix attachment failed",0x2d935c);}
    runtime_->state_[i].visualPrepared=true;
    const bool prepared=runtime_->prepareCollisionAfterVisual(i,d.runtime);lighting_->endGbAnimationAccess(this,i);active=runtime::ownerCount;
    if(!prepared)return runtimeFailure(d);
   }
   return true;
  }catch(...){if(active<runtime::ownerCount&&lighting_)lighting_->endGbAnimationAccess(this,active);return fail(d,"GB visual prefix exception; reached owners retained",0x2d93dc);}
 }
 bool completePreparation(std::shared_ptr<gb_collision_world50GB::Owner> collision,g::ModelMotionSharedState49T& shared,Diagnostic& d)noexcept {
  std::size_t active=runtime::ownerCount;
  try{d={};if(!thread()||failed_||ready_||collision_||!runtime_||!lighting_||!collision||!collision->animationPending()||
      collision->animations_.get()!=runtime_.get()||collision->coldSource()!=runtime_->coldSource()||runtime_->shared_!=&shared)
    return fail(d,"GB actual complete637-owner pending aggregate required",0x2d93f0);
   collision_=std::move(collision);collision_->publicationBlocked_=true;
   for(std::size_t i=0;i<runtime::ownerCount;++i){d.owner=i;g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
    if(!lighting_->beginGbAnimationAccess(this,i,world,work,object))return fail(d,"GB initial publication visibility lease unavailable",0x2d93f0);active=i;
    if(!collision_->attachAnimation(this,i,d.registry)){lighting_->endGbAnimationAccess(this,i);return fail(d,"GB actual collision attach failed",0x2d93f0);}
    if(!runtime_->initialSample(i,shared,d.runtime)){lighting_->endGbAnimationAccess(this,i);return runtimeFailure(d);}
    if(!updateVisual(i,*world,*work,object,d)){lighting_->endGbAnimationAccess(this,i);return false;}
    //2D94B0 is membership-only. Do not advance compound history a second
    //time after the initial sample; live11FE50 performs the next refresh.
    if(!collision_->updateAnimation(this,i,d.registry)){lighting_->endGbAnimationAccess(this,i);return fail(d,"GB initial collision membership update failed",0x2d94b0);}
    published_[i]=collision_->animationMemberships_[i]->state;collision_->committedMemberships_[i]=published_[i];
    if(!lighting_->commitGbAnimationOwner(this,i)){lighting_->endGbAnimationAccess(this,i);return fail(d,"GB real visual completion rejected",0x2d94b0);}
    lighting_->endGbAnimationAccess(this,i);active=runtime::ownerCount;
   }
   if(!collision_->commitAnimations(this))return fail(d,"GB actual complete animation publication rejected",0x2d94b0);
   runtime_->updateMeshBounds();runtime_->publicationReady_=true;ready_=true;return true;
  }catch(...){if(active<runtime::ownerCount&&lighting_)lighting_->endGbAnimationAccess(this,active);return fail(d,"GB initial sample/publication exception; reached owners retained",0x2d9480);}
 }
 //Call after root enrolls actual Storage, trigger memberships and mutable
 //candidate lanes, before seal. No graph/source/clock/compound lane is banked.
 bool enrollBank(native_racer50HN::Bank& bank,Diagnostic& d)noexcept {
  d={};if(!ready()||!collision_->actualIdle()||bank_||bank.retired_||bank.sealed_||bank.visiting_||bank.readBlocked_||bank.visitDepth_||bank.pendingToken_||bank.gbAnimationIssuer_||bank.gbAnimationBacking_)
   return fail(d,"GB enrollment requires genuine cold unsealed Bank",0);
  bank_=&bank;
  if(!bankBacking(false)){bank_=nullptr;return fail(d,"GB exhaustive real Storage/trigger/candidate enrollment missing",0);}
  const auto priorCount=bank.spans_.size(),priorBytes=bank.bytes_;
  try{if(!sharedWorld())for(auto& m:collision_->animationMemberships_){bank.owner(m->state);bank.elements(m->links.data(),m->links.size());bank.elements(m->entries.data(),m->entries.size());}
   if(!bankBacking(true))throw std::runtime_error("GB animation membership backing rejected");
   bank.gbAnimationBacking_=collision_;bank.gbAnimationIssuer_=this;bank.gbAnimationBlocked_=true;bankActivated_=false;runtime_->publicationReady_=false;collision_->publicationBlocked_=true;ready_=false;return true;
  }catch(...){bank.spans_.resize(priorCount);bank.bytes_=priorBytes;bank_=nullptr;return fail(d,"GB cold enrollment failed without moving prior spans",0);}
 }
 bool activateSealedBank(Diagnostic& d)noexcept {
  d={};try{if(!thread()||failed_||!bankIdle(true)||bankActivated_||!collision_||!collision_->actualIdle()||
      bank_->count_!=visual_[0].membership.object-2u)return fail(d,"GB actual sealed Bank/lighting racer count mismatch",0);
   buildSortedSpans();
   if(!allPeers([&](std::size_t){for(std::size_t i=0;i<runtime::ownerCount;++i)if(!memberIdentity(i))return false;return true;},d))
    return fail(d,"GB genuine cold membership snapshot identity rejected",0);
   if(!collision_->currentPublication())return fail(d,"GB sealed Bank current global publication rejected",0);
   bankActivated_=true;bank_->gbAnimationBlocked_=false;runtime_->publicationReady_=true;collision_->publicationBlocked_=false;ready_=true;return true;
  }catch(...){return fail(d,"GB sealed Bank admission exception",0);}
 }
 //The real Application pause boundary must call this once per enabled101860
 //world service, with actual manager-scaled delta30. No per-racer RNG/sample.
 bool service(float delta,std::uint32_t timestamp,std::uint32_t& raw,g::SharedRandomOwner50ID& random,
      g::ModelMotionSharedState49T& motion,Diagnostic& d)noexcept {
  std::size_t active=runtime::ownerCount;
  try{d={};if(!ready()||!collision_->queryReady()||!bankActivated_||!bankIdle(true)||!bankBacking(true)||!collision_->actualIdle()||runtime_->shared_!=&motion||
      !std::isfinite(delta)||delta<0.f||std::fegetround()!=FE_TONEAREST||!disjointBank(&raw,sizeof raw)||!disjointBank(&random,sizeof random))
    return fail(d,"GB enabled world-service identity/phase/arithmetic admission rejected",0x101860);
   servicing_=true;runtime_->servicing_=true;ready_=false;runtime_->publicationReady_=false;collision_->publicationBlocked_=true;bank_->gbAnimationBlocked_=true;
   struct End{Binding& b;~End(){b.servicing_=false;if(b.runtime_)b.runtime_->servicing_=false;}}end{*this};
   //Hold row0 incomplete during RNG/start service so failure cannot leave a
   //seemingly current lighting owner. This is native access discipline only.
   g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
   if(!lighting_->beginGbAnimationAccess(this,0,world,work,object))return fail(d,"GB world-service visual exclusion unavailable",0x101860);active=0;
   if(!runtime_->serviceRandom(timestamp,raw,random,d.runtime)){lighting_->endGbAnimationAccess(this,0);return runtimeFailure(d);}
   for(std::size_t i=0;i<runtime::ownerCount;++i){d.owner=i;
    if(i){if(!lighting_->beginGbAnimationAccess(this,i,world,work,object))return fail(d,"GB next owner visual exclusion unavailable",0x101728);active=i;}
    bool entered=false;
    if(!runtime_->advance(i,delta,motion,entered,d.runtime)){lighting_->endGbAnimationAccess(this,i);return runtimeFailure(d);}
    if(entered){
     if(!updateVisual(i,*world,*work,object,d)){lighting_->endGbAnimationAccess(this,i);return false;}
     if(!runtime_->refreshCollision(i,d.runtime)){lighting_->endGbAnimationAccess(this,i);return runtimeFailure(d);}
     //All actual peer memberships receive this one prepared shape before
     //the next source owner advances. Stopped owners never enter this branch.
     if(!allPeers([&](std::size_t){return memberIdentity(i)&&collision_->updateAnimation(this,i,d.registry);},d)){
      lighting_->endGbAnimationAccess(this,i);return fail(d,"GB real peer membership publication failed",0x120230);}
     published_[i]=collision_->animationMemberships_[i]->state;collision_->committedMemberships_[i]=published_[i];
    }
    if(!lighting_->commitGbAnimationOwner(this,i)){lighting_->endGbAnimationAccess(this,i);return fail(d,"GB actual current visual commit rejected",0x280d40);}
    lighting_->endGbAnimationAccess(this,i);active=runtime::ownerCount;
   }
   if(!collision_->currentPublication())return fail(d,"GB completed peer sweep current publication rejected",0x120230);
   runtime_->updateMeshBounds();runtime_->publicationReady_=true;collision_->publicationBlocked_=false;bank_->gbAnimationBlocked_=false;ready_=true;return true;
  }catch(...){if(active<runtime::ownerCount&&lighting_)lighting_->endGbAnimationAccess(this,active);return fail(d,"GB ordered world-service exception; reached prefix retained",0x101728);}
 }
 //Close outside real query/camera/draw/reset/FIFO scopes. Remove incomplete
 //visual claims before camera/racer cleanup if staticBusy would block it.
 //All source/runtime/GPU backing stays retained; structural aggregate borrows
 //survive so the root can then retire its real query and trigger memberships.
 bool close(Diagnostic& d)noexcept {
  d={};try{if(!thread()||servicing_||(runtime_&&runtime_->servicing_)||(collision_&&!collision_->actualIdle()))
    return fail(d,"GB retirement has an active real query/service borrower",0x11adc0);
   if(bank_&&!bankIdle(bank_->sealed_))return fail(d,"GB retirement requires actual idle Bank",0x120290);
   ready_=false;if(runtime_)runtime_->publicationReady_=false;
   if(collision_){collision_->publicationBlocked_=true;collision_->phase_=gb_collision_world50GB::Phase::Retiring;}
   if(bank_)bank_->gbAnimationBlocked_=true;
   for(std::size_t n=runtime::ownerCount;n;--n){const auto i=n-1;if(!visual_[i].claimed)continue;
    g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
    if(!lighting_||!lighting_->beginGbAnimationAccess(this,i,world,work,object))return fail(d,"GB real visual retirement lease unavailable",0x28ea80);
    d.registry=g::removeLiveZoneMembership50EN(world->lifecycle,world->portals,visual_[i].membership,*work);lighting_->endGbAnimationAccess(this,i);
    if(d.registry.status!=g::LiveZoneStatus50EN::Complete||!lighting_->releaseGbAnimationOwner(this,i))return fail(d,"GB real visual retirement failed",0x28eab0);
    visual_[i].claimed=false;
   }
   if(collision_){
    if(bank_&&bank_->sealed_){
     if(!allPeers([&](std::size_t){for(std::size_t n=runtime::ownerCount;n;--n){d.owner=n-1;if(!collision_->removeAnimation(this,n-1,d.registry))return false;}return true;},d))
      return fail(d,"GB actual all-peer compound retirement failed",0x120290);
    }else for(std::size_t n=runtime::ownerCount;n;--n)if(!collision_->removeAnimation(this,n-1,d.registry))return fail(d,"GB reached cold compound retirement failed",0x120290);
   }
   if(bank_){bank_->gbAnimationIssuer_=nullptr;bank_->gbAnimationBlocked_=false;bankActivated_=false;bank_=nullptr;}
   if(runtime_){if(runtime_->binding_!=this)return fail(d,"GB runtime retirement issuer changed",0x11adc0);runtime_->binding_=nullptr;}
   collision_.reset();runtime_.reset();lighting_.reset();return true;
  }catch(...){return fail(d,"GB retirement exception; backing retained",0x120290);}
 }
};
}

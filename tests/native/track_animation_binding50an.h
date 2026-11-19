#pragma once
// Common track-animation BINDING: joins the common runtime (track_animation_runtime50an.h) to a
// course's visual-zone lane (lighting owner) and, when the course keeps its own collision world,
// that world's collision lane (track_animation_world50an.h). One copy for every course on that
// route; replaces tb_door_binding50tb.h, tc_gate_visibility50tc.h, gb_animation_binding50gb.h and
// mc_animation_binding50mc.h (same order of operations, rows from the course's own table).
//   2D935C  visual method-2 attach of every row at its StartingZone
//   2D93F0  collision method-2 attach of every UseCollision row (120070)
//   2D9480  time-0 sample (already taken by the runtime's create), then visual and collision update
//   101860  one tick of the runtime, then each moved row's visual and collision memberships
//           (28EA20 / 120230), every Bank peer for a legacy per-racer world
//   2702E0  race reset (ResetAll + re-stop of trigger targets), then every row republished
//   11ADC0  retirement: visual removal, collision removal (120290)
// PORT STAND-IN (flagged): the GA/GC/SA/TA rows stay on the static-candidate route (their
// collision lives in the race's track storage and they register no visual-zone lane); this
// binding is the route for courses whose collision world and visibility world are their own.
#include "track_animation_world50an.h"
#include "track_animation_course50an.h"
#include "track_animation_sphere50an.h"
#include "native_racer_bank50hn_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <algorithm>
#include <thread>
#include <vector>

namespace track_animation_binding50AN {
namespace g=rr::game;namespace lighting=g::original_gc_gameplay_lighting_candidate;
namespace anim=track_animation_runtime50AN;namespace world=track_animation_world50AN;namespace rt=track_runtime_storage50IG;
static_assert(lighting::Owner::maximumTrackAnimationRows==rr::assets::TrackAnimationLimits50AN{}.animations,
  "the lighting lane capacity is the reader's animation limit");
struct Diagnostic {std::string message;std::uint32_t sourcePC=0;std::size_t row=0,peer=0;g::LiveZoneResult50EN registry{};anim::Diagnostic runtime{};};
class Binding final {
 std::shared_ptr<anim::Owner> runtime_;
 std::shared_ptr<lighting::Owner> lighting_;
 std::shared_ptr<world::World> collision_;
 struct Visual {
  std::vector<g::CollectionZoneLink50BX> links;std::vector<g::LiveZoneEntry50EN> entries;
  g::LiveZoneMembership50EN membership{};track_animation_sphere50AN::Result sphere{};bool claimed=false;
 };
 std::vector<Visual> visual_;
 std::vector<g::LiveZoneMembership50EN> published_; // per collision lane row
 std::vector<track_animation_sphere50AN::Row> sphereRows_; // visual-sphere scratch, sized at beginVisual (no per-frame allocation)
 native_racer50HN::Bank* bank_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 std::string label_;
 bool attempted_=false,ready_=false,failed_=false,servicing_=false,bankActivated_=false,collisionPublished_=false;
 bool fail(Diagnostic& d,const char* message,std::uint32_t pc)noexcept{
  failed_=true;ready_=false;if(collision_)collision_->blockAnimationPublication(true);if(bank_)bank_->animationBlocked_=true;
  d.sourcePC=pc;try{d.message=message;}catch(...){}return false;
 }
 bool runtimeFailure(Diagnostic& d)noexcept{return fail(d,d.runtime.message.empty()?"track animation runtime rejected":d.runtime.message.c_str(),d.runtime.sourcePC);}
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 world::Lane* lane()const noexcept{return collision_?collision_->animationLane():nullptr;}
 // 26F4B0 bit 0x10 / 28D940 / 28BD50 over the row's PSG rows at their refreshed worlds.
 bool visualBounds(std::size_t i,g::LiveZoneBounds50EN& out,Diagnostic& d)noexcept{
  try{
   auto& state=runtime_->mutableState(i);const auto& source=runtime_->source().row(i);
   const g::SceneGraph49P graph{state.nodes.data(),static_cast<std::uint32_t>(state.nodes.size())};
   if(!(state.nodes[0].sceneFlags190&0x10u)||(state.nodes[0].sceneFlags190&4u)||!g::refreshSceneWorld49P(graph,static_cast<anim::Index>(0)))
    return fail(d,"track animation visual sphere root rejected",0x26f4b0);
   if(source.hierarchy.nodeCount>sphereRows_.size())return fail(d,"track animation visual sphere scratch extent",0x26f4b0);
   auto* rows=sphereRows_.data();
   for(std::uint32_t r=0;r<source.hierarchy.nodeCount;++r){rr::assets::PsgNode49O original{};
    if(!(state.nodes[r+1].sceneFlags190&4u)||!rr::assets::readPsgNode49O(source.hierarchy,r,original)||
       !g::refreshSceneWorld49P(graph,static_cast<anim::Index>(r+1)))return fail(d,"track animation PSG visual sphere row rejected",0x28d940);
    rows[r].world=state.nodes[r+1].world100;for(unsigned k=0;k<3;++k)rows[r].center[k]=original.bounds[k];rows[r].radius=original.bounds[6];
   }
   track_animation_sphere50AN::Result next{};
   if(!track_animation_sphere50AN::compute(state.nodes[0].world100,rows,source.hierarchy.nodeCount,next))return fail(d,"track animation visual sphere arithmetic rejected",0x28bd50);
   visual_[i].sphere=next;if(next.radius198>0.f)state.nodes[0].sceneFlags190|=0x40u;
   out={};out.bounds.shapeTag=1;out.bounds.shapeWorld=state.nodes[0].world100;
   for(unsigned k=0;k<3;++k){out.bounds.localCenter.v[k]=next.center160[k];out.bounds.localHalf.v[k]=next.radius198;}
   out.bounds.cachedRadius=next.radius198;g::CollectionBoundsResult48S actual{};
   if(g::prepareCollectionBounds48S(out.bounds,actual)!=g::CollectionBoundsStatus48S::Complete)return fail(d,"track animation visual bounds rejected",0x28e230);
   out.bounds.cachedCenter=actual.center;return true;
  }catch(...){return fail(d,"track animation visual bounds exception",0x26f4b0);}
 }
 bool updateVisual(std::size_t i,g::ResetVisibilityWorld50EY& w,g::LiveZoneWorkspace50EN& work,g::CollectionRef50BX object,Diagnostic& d)noexcept{
  auto& m=visual_[i].membership;if(!m.attached||m.object!=object)return fail(d,"track animation visual membership identity changed",0x28ea20);
  g::LiveZoneBounds50EN bounds{};if(!visualBounds(i,bounds,d))return false;
  d.registry=g::updateLiveZoneMembership50EN(w.lifecycle,w.portals,m,bounds,work);
  return d.registry.status==g::LiveZoneStatus50EN::Complete||fail(d,"track animation visual publication rejected",0x28ea20);
 }
 bool sharedWorld()const noexcept{auto* s=collision_?collision_->animationStorage():nullptr;return !s||s->sharedLayout();}
 bool bankIdle(bool sealed)const noexcept{
  return bank_&&bank_->animationIssuer_==this&&!bank_->retired_&&bank_->sealed_==sealed&&!bank_->visiting_&&!bank_->readBlocked_&&
   !bank_->visitDepth_&&!bank_->pendingToken_&&!bank_->retirementVisit_&&!bank_->nativeFifoPins_&&!bank_->animationVisit_&&(!sealed||bank_->count_>=1);
 }
 // Collision publication of one lane row in the one world (shared layout) or in every Bank
 // peer (legacy per-racer layout, the lane memberships are banked).
 template<class F>bool allPeers(F&& operation,Diagnostic& d){
  if(!bank_||sharedWorld()){d.peer=0;return operation(std::size_t{0});}
  if(!bankIdle(true))return fail(d,"track animation Bank peers unavailable",0x120230);
  bank_->animationVisit_=true;struct End{bool& flag;~End(){flag=false;}}end{bank_->animationVisit_};
  return bank_->visitAll([&](std::size_t peer){d.peer=peer;return operation(peer);});
 }
 // One row: visual update, then (UseCollision) the collision lane update in every peer.
 bool publishRow(std::size_t i,Diagnostic& d)noexcept{
  g::ResetVisibilityWorld50EY* w=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
  if(!lighting_->beginTrackAnimationAccess(this,i,w,work,object))return fail(d,"track animation visual access unavailable",0x28ea20);
  struct End{Binding& b;std::size_t i;~End(){b.lighting_->endTrackAnimationAccess(&b,i);}}end{*this,i};
  if(!updateVisual(i,*w,*work,object,d))return false;
  if(auto* l=lane()){const auto k=l->laneOf(i);
   if(k<l->count()){auto* s=collision_->animationStorage();if(!s)return fail(d,"track animation collision storage unavailable",0x120230);
    if(!allPeers([&](std::size_t){return l->update(*s,k,d.registry);},d))return fail(d,"track animation collision publication failed",0x120230);
    published_[k]=l->membership(k)->state;}}
  if(!lighting_->commitTrackAnimationRow(this,i))return fail(d,"track animation visual commit rejected",0x280d40);
  auto& state=runtime_->mutableState(i);state.collisionDirty=false;state.rehomePending=false;return true;
 }
public:
 Binding()=default;Binding(const Binding&)=delete;Binding&operator=(const Binding&)=delete;Binding(Binding&&)=delete;Binding&operator=(Binding&&)=delete;
 ~Binding(){if(needsClose()||!thread())std::terminate();}
 static constexpr std::size_t requiredPayloadBytes()noexcept{return sizeof(Binding);}
 // Extra host backing for `rows` rows on a world with `portals` portals (visual memberships).
 static std::size_t payloadBytes(std::size_t rows,std::size_t portals)noexcept{
  const auto per=sizeof(Visual)+(portals+1)*(sizeof(g::CollectionZoneLink50BX)+sizeof(g::LiveZoneEntry50EN))+sizeof(g::LiveZoneMembership50EN);
  return sizeof(Binding)+rows*per;
 }
 bool needsClose()const noexcept{if(bank_||collision_)return true;for(const auto& v:visual_)if(v.claimed)return true;return false;}
 bool ready()const noexcept{
  if(!thread()||!ready_||failed_||servicing_||!runtime_||!runtime_->ready()||!lighting_)return false;
  if(bank_&&(!bankActivated_||!bankIdle(true)||bank_->animationBlocked_))return false;
  for(std::size_t i=0;i<visual_.size();++i)if(!visual_[i].claimed||!lighting_->currentTrackAnimationRow(this,i))return false;return true;
 }
 bool failed()const noexcept{return failed_;}
 const anim::Owner* runtimeOwner()const noexcept{return runtime_.get();}
 const lighting::Owner* lightingOwner()const noexcept{return lighting_.get();}
 const world::World* collisionWorld()const noexcept{return collision_.get();}
 // 2D935C for every row. The lighting owner was planned with this runtime's row count.
 bool beginVisual(std::shared_ptr<anim::Owner> runtime,std::shared_ptr<lighting::Owner> light,Diagnostic& d)noexcept{
  std::size_t active=SIZE_MAX;
  try{d={};if(attempted_||!thread())return fail(d,"track animation binding requires a fresh creating-thread Binding",0x2d935c);attempted_=true;
   if(!runtime||!runtime->ready()||!light||!light->world()||light->trackAnimationRows()!=runtime->rowCount())
    return fail(d,"track animation visual lane does not match the course's animation rows",0x2d935c);
   runtime_=std::move(runtime);lighting_=std::move(light);label_=runtime_->label();
   const auto capacity=lighting_->world()->portals.size()+1; // live zone membership: portals + 1
   visual_.resize(runtime_->rowCount());
   {std::size_t nodes=0;for(std::size_t i=0;i<visual_.size();++i)nodes=std::max<std::size_t>(nodes,runtime_->source().row(i).hierarchy.nodeCount);sphereRows_.assign(nodes,{});}
   for(std::size_t i=0;i<visual_.size();++i){d.row=i;auto& v=visual_[i];v.links.resize(capacity);v.entries.resize(capacity);
    if(!lighting_->bindTrackAnimationRow(this,i,&v.membership))return fail(d,"track animation visual lane claim rejected",0x28e8a0);v.claimed=true;
    g::ResetVisibilityWorld50EY* w=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
    if(!lighting_->beginTrackAnimationAccess(this,i,w,work,object))return fail(d,"track animation visual access unavailable",0x28e8a0);active=i;
    g::LiveZoneBounds50EN bounds{};if(!visualBounds(i,bounds,d)){lighting_->endTrackAnimationAccess(this,i);return false;}
    d.registry=g::attachLiveZoneMembership50EN(w->lifecycle,w->portals,v.membership,{v.links.data(),v.entries.data(),v.links.size()},
      object,runtime_->source().row(i).authored.startingZone+1,g::ZoneMethod50CL::Second,0.f,bounds,*work);
    lighting_->endTrackAnimationAccess(this,i);active=SIZE_MAX;
    if(d.registry.status!=g::LiveZoneStatus50EN::Complete)return fail(d,"track animation visual attachment failed",0x2d935c);
   }
   return true;
  }catch(...){if(active!=SIZE_MAX&&lighting_)lighting_->endTrackAnimationAccess(this,active);return fail(d,"track animation visual prefix exception; reached rows retained",0x2d93dc);}
 }
 // 2D93F0 + 2D9480: collision (when the course world holds a lane), then each row's visual and
 // collision update at the time-0 pose. world may be null only when no row has UseCollision.
 bool completePreparation(std::shared_ptr<world::World> collision,Diagnostic& d)noexcept{
  try{d={};if(!thread()||failed_||ready_||collision_||!runtime_||!lighting_||visual_.size()!=runtime_->rowCount())
    return fail(d,"track animation preparation requires the visual prefix",0x2d93f0);
   const auto rows=world::Lane::collisionRowCount(runtime_.get());
   if(rows&&!collision)return fail(d,"track animation UseCollision rows need the course collision world's animation lane (PORT STAND-IN: this course's world carries none)",0x2d93f0);
   collision_=std::move(collision);
   if(auto* l=lane()){
    if(!collision_->animationPending()||l->runtime()!=runtime_.get()||l->count()!=rows)return fail(d,"track animation collision lane differs from the runtime",0x2d93f0);
    auto* s=collision_->animationStorage();if(!s)return fail(d,"track animation collision storage unavailable",0x2d93f0);
    collision_->blockAnimationPublication(true);published_.assign(rows,{});
    for(std::size_t k=0;k<rows;++k){d.row=l->row(k);if(!l->attach(*s,k,d.registry))return fail(d,"track animation collision attach failed",0x2d93f0);}
   }
   for(std::size_t i=0;i<visual_.size();++i){d.row=i;if(!publishRow(i,d))return false;}
   if(collision_&&!collision_->commitAnimationLane())return fail(d,"track animation collision publication rejected",0x2d94b0);
   collisionPublished_=true;ready_=true;
   std::cerr<<"track_animation_binding course="<<label_<<" rows="<<visual_.size()<<" visual_lane=1 collision_rows="<<rows
       <<" collision_world="<<(collision_?1:0)<<'\n';
   return true;
  }catch(...){return fail(d,"track animation initial publication exception; reached rows retained",0x2d9480);}
 }
 // Legacy per-racer world: bank the lane memberships before seal. Shared world: nothing to bank.
 bool enrollBank(native_racer50HN::Bank& bank,Diagnostic& d)noexcept{
  d={};if(!ready()||(collision_&&!collision_->animationIdle())||bank_||bank.retired_||bank.sealed_||bank.visiting_||bank.readBlocked_||
     bank.visitDepth_||bank.pendingToken_||bank.animationIssuer_||bank.animationBacking_)return fail(d,"track animation Bank enrollment requires a cold unsealed Bank",0);
  const auto priorCount=bank.spans_.size(),priorBytes=bank.bytes_;
  try{if(auto* l=lane();l&&!sharedWorld())for(std::size_t k=0;k<l->count();++k){auto* m=l->membership(k);
     bank.owner(m->state);bank.elements(m->links.data(),m->links.size());bank.elements(m->entries.data(),m->entries.size());}
   bank_=&bank;bank.animationBacking_=collision_;bank.animationIssuer_=this;bank.animationBlocked_=true;bankActivated_=false;ready_=false;
   if(collision_)collision_->blockAnimationPublication(true);return true;
  }catch(...){bank.spans_.resize(priorCount);bank.bytes_=priorBytes;bank_=nullptr;return fail(d,"track animation Bank enrollment failed without moving prior spans",0);}
 }
 bool activateSealedBank(Diagnostic& d)noexcept{
  d={};try{if(!thread()||failed_||!bankIdle(true)||bankActivated_||(collision_&&!collision_->animationIdle()))return fail(d,"track animation sealed Bank admission rejected",0);
   if(auto* l=lane()){const auto* s=collision_->animationStorage();
    if(!s||!allPeers([&](std::size_t){return l->current(*s);},d))return fail(d,"track animation sealed Bank membership snapshot rejected",0);}
   bankActivated_=true;bank_->animationBlocked_=false;if(collision_)collision_->blockAnimationPublication(false);ready_=true;return true;
  }catch(...){return fail(d,"track animation sealed Bank admission exception",0);}
 }
 // 101860 once per world service: the runtime's tick (pause gate, one shared random draw),
 // then every row that moved republishes its visual and collision memberships.
 bool service(float delta,bool paused,std::uint32_t timestamp,std::uint32_t& raw,g::SharedRandomOwner50ID& random,
     g::ModelMotionSharedState49T& motion,Diagnostic& d)noexcept{
  try{d={};if(!ready()||(collision_&&!collision_->animationIdle()))return fail(d,"track animation world service admission rejected",0x101860);
   servicing_=true;struct End{Binding& b;~End(){b.servicing_=false;}}end{*this};
   if(collision_)collision_->blockAnimationPublication(true);if(bank_)bank_->animationBlocked_=true;
   if(!runtime_->tick(delta,paused,timestamp,raw,random,motion,d.runtime))return runtimeFailure(d);
   for(std::size_t i=0;i<visual_.size();++i){d.row=i;if(runtime_->state(i).collisionDirty&&!publishRow(i,d))return false;}
   if(collision_)collision_->blockAnimationPublication(false);if(bank_)bank_->animationBlocked_=false;return true;
  }catch(...){return fail(d,"track animation world service exception",0x101728);}
 }
 // 2702E0: ResetAll + re-stop the Animation-action targets, then every row republished.
 bool raceReset(Diagnostic& d)noexcept{
  try{d={};if(!ready()||(collision_&&!collision_->animationIdle()))return fail(d,"track animation race reset admission rejected",0x2702e0);
   servicing_=true;struct End{Binding& b;~End(){b.servicing_=false;}}end{*this};
   if(collision_)collision_->blockAnimationPublication(true);if(bank_)bank_->animationBlocked_=true;
   if(!runtime_->raceReset(d.runtime))return runtimeFailure(d);
   for(std::size_t i=0;i<visual_.size();++i){d.row=i;if(!publishRow(i,d))return false;}
   if(collision_)collision_->blockAnimationPublication(false);if(bank_)bank_->animationBlocked_=false;return true;
  }catch(...){return fail(d,"track animation race reset exception",0x2702e0);}
 }
 // 11ADC0 / 120290. Outside real query/camera/draw/reset scopes; backing stays retained until done.
 bool close(Diagnostic& d)noexcept{
  d={};try{if(!thread()||servicing_||(collision_&&!collision_->animationIdle()))return fail(d,"track animation retirement has an active borrower",0x11adc0);
   if(bank_&&!bankIdle(bank_->sealed_))return fail(d,"track animation retirement requires an idle Bank",0x120290);
   ready_=false;if(collision_)collision_->blockAnimationPublication(true);if(bank_)bank_->animationBlocked_=true;
   for(std::size_t n=visual_.size();n;--n){const auto i=n-1;if(!visual_[i].claimed)continue;
    g::ResetVisibilityWorld50EY* w=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
    if(!lighting_||!lighting_->beginTrackAnimationAccess(this,i,w,work,object))return fail(d,"track animation visual retirement access unavailable",0x28ea80);
    if(visual_[i].membership.attached||visual_[i].membership.object)
     d.registry=g::removeLiveZoneMembership50EN(w->lifecycle,w->portals,visual_[i].membership,*work);
    else d.registry.status=g::LiveZoneStatus50EN::Complete;
    lighting_->endTrackAnimationAccess(this,i);
    if(d.registry.status!=g::LiveZoneStatus50EN::Complete||!lighting_->releaseTrackAnimationRow(this,i))return fail(d,"track animation visual retirement failed",0x28eab0);
    visual_[i].claimed=false;
   }
   if(auto* l=lane()){auto* s=collision_->animationStorage();
    if(s){const bool peers=bank_&&bank_->sealed_;
     auto removeAll=[&](std::size_t){for(std::size_t n=l->count();n;--n){d.row=l->row(n-1);if(!l->remove(*s,n-1,d.registry))return false;}return true;};
     if(peers?!allPeers(removeAll,d):!removeAll(0))return fail(d,"track animation collision retirement failed",0x120290);}
   }
   if(bank_){bank_->animationIssuer_=nullptr;bank_->animationBlocked_=false;bankActivated_=false;bank_=nullptr;}
   collision_.reset();lighting_.reset();ready_=false;return true;
  }catch(...){return fail(d,"track animation retirement exception; backing retained",0x120290);}
 }
};
// The lane route's load step, the same on every course that takes it: the course's own
// [Animations] through the common player (track_animation_course50an.h; absent = no runtime),
// then the visual-zone lane planned on the course's lighting owner BEFORE its prepare*Native.
// Throws std::runtime_error with a plain message on refusal.
inline std::shared_ptr<anim::Owner> loadForLane(const std::filesystem::path& archive,const char* code,std::size_t zones,
    lighting::Owner& light,g::SourceLibmState49T& libm,g::ModelMotionSharedState49T& shared){
 auto runtime=track_animation_course50AN::load(archive,code,zones,libm,shared).runtime;
 if(!light.planTrackAnimationRows(runtime?runtime->rowCount():0))
  throw std::runtime_error("track animation rows exceed the visual-zone lane or the lighting owner is already prepared");
 return runtime;
}
// Host payload the course charges for a lane-route runtime: its dynamic mesh, normals and spans
// (the runtime's retained render payload), the lighting lane and this binding's memberships.
inline std::size_t lanePayloadBytes(const anim::Owner* runtime,std::size_t portals)noexcept{
 if(!runtime)return 0;
 const auto rows=runtime->rowCount();
 std::size_t n=runtime->mesh().vertices.capacity()*sizeof(rr::assets::Vertex48B)+runtime->mesh().indices.capacity()*sizeof(std::uint32_t)+
   runtime->normals().capacity()*sizeof(rr::assets::WorldNormal50HK)+runtime->spans().capacity()*sizeof(anim::Span);
 n+=lighting::Owner::trackAnimationStorageReservationBytes(rows);
 n+=Binding::payloadBytes(rows,portals);
 return n;
}
} // namespace track_animation_binding50AN

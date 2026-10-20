#pragma once
#include "tb_door_runtime50tb.h"
#include "tb_door_sphere50tb.h"
#include "tb_collision_world50tb.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <thread>
namespace tb_door_binding50TB {
namespace g=rr::game;namespace lighting=g::original_gc_gameplay_lighting_candidate;
struct Diagnostic {std::string message;std::uint32_t sourcePC=0;g::LiveZoneResult50EN registry{};};
class Binding final {
 std::shared_ptr<tb_door_runtime50TB::Owner> door_;
 std::shared_ptr<lighting::Owner> lighting_;
 std::shared_ptr<tb_collision_world50TB::Owner> collision_;
 std::array<g::CollectionZoneLink50BX,12> links_{};
 std::array<g::LiveZoneEntry50EN,12> entries_{};
 g::LiveZoneMembership50EN membership_{};tb_door_sphere50TB::Result sphere_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,visualClaimed_=false,ready_=false;
 static bool fail(Diagnostic& d,const char* message,std::uint32_t pc)noexcept {
  d.sourcePC=pc;try{d.message=message;}catch(...){}return false;
 }
 bool bounds(g::LiveZoneBounds50EN& out,Diagnostic& d)noexcept {
  if(!door_||!door_->source_||!door_->source_->ready()||door_->nodes_.size()!=4)return fail(d,"TB door source graph unavailable",0x26f4b0);
  const auto graph=g::SceneGraph49P{door_->nodes_.data(),4};
  if(!(door_->nodes_[0].sceneFlags190&0x10u)||(door_->nodes_[0].sceneFlags190&4u)||
      !g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(0)))return fail(d,"TB actual sphere root rejected",0x26f4b0);
  std::array<tb_door_sphere50TB::Row,3> rows{};
  for(std::uint32_t i=0;i<3;++i){rr::assets::PsgNode49O source{};
   if(!(door_->nodes_[i+1].sceneFlags190&4u)||
      !rr::assets::readPsgNode49O(door_->source_->hierarchy(),i,source)||
      !g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(i+1)))return fail(d,"TB original sphere row rejected",0x28d940);
   rows[i].world=door_->nodes_[i+1].world100;
   for(unsigned axis=0;axis<3;++axis)rows[i].center[axis]=source.bounds[axis];rows[i].radius=source.bounds[6];
  }
  tb_door_sphere50TB::Result next{};
  if(!tb_door_sphere50TB::compute(door_->nodes_[0].world100,rows,next))return fail(d,"TB actual sphere projection rejected",0x26f4b0);
  sphere_=next;if(next.radius198>0.f)door_->nodes_[0].sceneFlags190|=0x40u;
  //28E230 consumes center160/equal XYZ half170 through the existing affine
  //box arithmetic. Tag1 selects that numeric leaf, not a collision object.
  out={};out.bounds.shapeTag=1;out.bounds.shapeWorld=door_->nodes_[0].world100;
  for(unsigned axis=0;axis<3;++axis){out.bounds.localCenter.v[axis]=next.center160[axis];out.bounds.localHalf.v[axis]=next.radius198;}
  out.bounds.cachedRadius=next.radius198;g::CollectionBoundsResult48S actual{};
  if(g::prepareCollectionBounds48S(out.bounds,actual)!=g::CollectionBoundsStatus48S::Complete)return fail(d,"TB actual observer box projection rejected",0x28e230);
  out.bounds.cachedCenter=actual.center;return true;
 }
public:
 Binding()=default;Binding(const Binding&)=delete;Binding&operator=(const Binding&)=delete;
 Binding(Binding&&)=delete;Binding&operator=(Binding&&)=delete;
 ~Binding(){if(needsClose()||thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t requiredPayloadBytes()noexcept{return sizeof(Binding);}
 bool ownsRegistration()const noexcept{return visualClaimed_||bool(collision_);}
 bool needsClose()const noexcept{return visualClaimed_||bool(collision_)||(door_&&door_->binding_==this);}
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&visualClaimed_&&door_&&door_->ready()&&
  lighting_&&lighting_->currentTbDoorOwner(this)&&collision_&&collision_->ready();}
 const tb_door_runtime50TB::Owner* runtimeOwner()const noexcept{return door_.get();}
 const lighting::Owner* lightingOwner()const noexcept{return lighting_.get();}
 // Source2D935C visual method2 attach precedes11FE50 and Storage projection.
 // Successful return is a reached prefix only; it cannot issue a world lease.
 bool beginVisual(std::shared_ptr<tb_door_runtime50TB::Owner> door,
      std::shared_ptr<lighting::Owner> lighting,Diagnostic& d)noexcept {
  bool access=false;
  try{
   d={};if(attempted_||thread_!=std::this_thread::get_id())return fail(d,"TB door requires a fresh creating-thread Binding",0x2d935c);attempted_=true;
   if(!door||!lighting||!door->stagedSourceReady()||!door->initialSamplePending_||door->initialSampleAttempted_||
      !door->initialShared_||door->binding_||!lighting->world()||!door->source_->coldSource()||
      lighting->world()->parameters.original_bytes!=door->source_->coldSource()->world().parameters.original_bytes||
      door->source_->authored().startingZone!=4||!door->source_->authored().useCollision)
    return fail(d,"TB exact source/lighting/pending graph identity rejected",0x2d935c);
   door_=std::move(door);lighting_=std::move(lighting);
   if(!lighting_->bindTbDoorOwner(this,&membership_))return fail(d,"TB actual visual issuer unavailable",0x28e8a0);
   visualClaimed_=true;door_->binding_=this;
   g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
   if(!lighting_->beginTbDoorAccess(this,world,work,object))return fail(d,"TB initial visual access unavailable",0x28e8a0);access=true;
   g::LiveZoneBounds50EN actual{};if(!bounds(actual,d)){lighting_->endTbDoorAccess(this);return false;}
   const g::LiveZoneStorage50EN storage{links_.data(),entries_.data(),links_.size()};
   d.registry=g::attachLiveZoneMembership50EN(world->lifecycle,world->portals,membership_,storage,object,4,
       g::ZoneMethod50CL::Second,0.f,actual,*work);
   if(d.registry.status!=g::LiveZoneStatus50EN::Complete){lighting_->endTbDoorAccess(this);return fail(d,"TB actual visual attachment failed",0x2d935c);}
   tb_door_runtime50TB::Diagnostic prepared;
   const bool complete=door_->prepareCollisionAfterVisual(prepared);
   lighting_->endTbDoorAccess(this);access=false;
   if(!complete){d.sourcePC=prepared.sourcePC;d.message=prepared.message;return false;}
   return true;
  }catch(...){if(access&&lighting_)lighting_->endTbDoorAccess(this);return fail(d,"TB visual/shape preparation exception; reached prefix retained",0x2d93dc);}
 }
 // Storage has now admitted the real prepared root candidate. Attach collision,
 // sample the original MOT at0, then update visual before collision. Only the
 // final genuine commit allows the aggregate to issue navigation/world leases.
 bool completePreparation(std::shared_ptr<tb_collision_world50TB::Owner> collision,
      g::ModelMotionSharedState49T& shared,Diagnostic& d)noexcept {
  bool access=false;
  try{
   d={};if(thread_!=std::this_thread::get_id()||!visualClaimed_||!door_||!lighting_||collision_||ready_||
      !collision||!collision->doorPending()||collision->door_.get()!=door_.get()||
      collision->coldSource()!=door_->coldSource()||door_->initialShared_!=&shared||door_->initialSampleAttempted_)
    return fail(d,"TB same DoorPending aggregate/shared motion required",0x2d93f0);
   collision_=std::move(collision);
   g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
   if(!lighting_->beginTbDoorAccess(this,world,work,object))return fail(d,"TB completion visual access unavailable",0x2d93f0);access=true;
   if(!collision_->attachDoor(this,d.registry)){lighting_->endTbDoorAccess(this);return fail(d,"TB actual collision method2 attachment failed",0x2d93f0);}
   tb_door_runtime50TB::Diagnostic sampled;const bool sampledOk=door_->completeInitialSample(shared,sampled);
   if(!sampledOk){lighting_->endTbDoorAccess(this);d.sourcePC=sampled.sourcePC;d.message=sampled.message;return false;}
   g::LiveZoneBounds50EN actual{};
   if(!bounds(actual,d)){lighting_->endTbDoorAccess(this);return false;}
   if(!membership_.attached||membership_.object!=object){lighting_->endTbDoorAccess(this);return fail(d,"TB actual visual identity changed",0x2d9498);}
   d.registry=g::updateLiveZoneMembership50EN(world->lifecycle,world->portals,membership_,actual,*work);
   if(d.registry.status!=g::LiveZoneStatus50EN::Complete){lighting_->endTbDoorAccess(this);return fail(d,"TB initial visual update failed",0x2d9498);}
   if(!collision_->updateDoor(this,d.registry)){lighting_->endTbDoorAccess(this);return fail(d,"TB initial collision update failed",0x2d94b0);}
   if(!lighting_->commitTbDoorOwner(this)||!collision_->commitDoor(this)){lighting_->endTbDoorAccess(this);return fail(d,"TB actual dual membership publication failed",0x2d94b0);}
   door_->publicationReady_=true;door_->posePending_=false;ready_=true;lighting_->endTbDoorAccess(this);access=false;return true;
  }catch(...){if(access&&lighting_)lighting_->endTbDoorAccess(this);return fail(d,"TB initial sample/update exception; reached memberships retained",0x2d9480);}
 }
 // Called immediately after the same Runtime's successful101728 service,
 // outside every real query/Bank/draw/camera/reset lease. No second clock step.
 bool update(Diagnostic& d)noexcept {
  d={};if(thread_!=std::this_thread::get_id()||!visualClaimed_||!door_||!lighting_||!collision_||
      !door_->ready_||door_->failed_||!door_->posePending_||!collision_->doorAccess(this))
   return fail(d,"TB actual pending door pose is unavailable",0x101788);
  g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
  if(!lighting_->beginTbDoorAccess(this,world,work,object))return fail(d,"TB actual door update access unavailable",0x11ad80);
  ready_=false;door_->publicationReady_=false;g::LiveZoneBounds50EN actual{};
  if(!bounds(actual,d)){lighting_->endTbDoorAccess(this);return false;}
  if(!membership_.attached||membership_.object!=object){lighting_->endTbDoorAccess(this);return fail(d,"TB current visual identity changed",0x28ea20);}
  d.registry=g::updateLiveZoneMembership50EN(world->lifecycle,world->portals,membership_,actual,*work);
  if(d.registry.status!=g::LiveZoneStatus50EN::Complete){lighting_->endTbDoorAccess(this);return fail(d,"TB actual visual update failed",0x28ea20);}
  // Original120230 updates the same prepared root bounds. It does not rerun
  //11FE50 or retarget collision to either moving panel.
  if(!collision_->updateDoor(this,d.registry)){lighting_->endTbDoorAccess(this);return fail(d,"TB actual collision update failed",0x120230);}
  const bool committed=lighting_->commitTbDoorOwner(this);lighting_->endTbDoorAccess(this);
  if(!committed)return fail(d,"TB actual updated visual publication failed",0x280d40);
  door_->publicationReady_=true;door_->posePending_=false;ready_=true;return true;
 }
 // Outside active leases, retire an incomplete claimed door BEFORE camera
 // spatial and racer7F0 cleanup if staticBusy would otherwise reject them.
 // All root/window/runtime/lighting backing remains retained through this call.
 bool close(Diagnostic& d)noexcept {
  d={};if(thread_!=std::this_thread::get_id())return fail(d,"TB door retirement thread mismatch",0x11adc0);
  if(collision_&&!collision_->doorAccess(this))return fail(d,"TB door collision query borrower still active",0x11adc0);
  ready_=false;if(door_)door_->publicationReady_=false;
  if(visualClaimed_){
   g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* work=nullptr;g::CollectionRef50BX object=0;
   if(!lighting_||!lighting_->beginTbDoorAccess(this,world,work,object))return fail(d,"TB door retirement visibility lease unavailable",0x28ea80);
   d.registry=g::removeLiveZoneMembership50EN(world->lifecycle,world->portals,membership_,*work);lighting_->endTbDoorAccess(this);
   if(d.registry.status!=g::LiveZoneStatus50EN::Complete)return fail(d,"TB actual visual removal failed",0x28ea80);
   if(!lighting_->releaseTbDoorOwner(this))return fail(d,"TB actual visual issuer release failed",0x28eab0);visualClaimed_=false;
  }
  if(collision_&&!collision_->removeDoor(this,d.registry))return fail(d,"TB actual root collision removal failed",0x120290);
  if(door_){if(door_->binding_&&door_->binding_!=this)return fail(d,"TB door issuer identity changed",0x11adc0);door_->binding_=nullptr;}
  collision_.reset();door_.reset();lighting_.reset();return true;
 }
};
}

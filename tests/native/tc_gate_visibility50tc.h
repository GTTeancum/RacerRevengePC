#pragma once
#include "tc_gate_runtime50tc.h"
#include "tc_gate_sphere50tc.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <thread>

namespace tc_gate_visibility50TC {
namespace g=rr::game;namespace lighting=g::original_gc_gameplay_lighting_candidate;
struct Diagnostic {std::string message;std::uint32_t sourcePC=0;g::LiveZoneResult50EN registry{};};
class Binding final {
 std::shared_ptr<tc_gate_runtime50TC::Owner> gate_;
 std::shared_ptr<lighting::Owner> lighting_;
 std::array<g::CollectionZoneLink50BX,27> links_{};
 std::array<g::LiveZoneEntry50EN,27> entries_{};
 g::LiveZoneMembership50EN membership_{};
 tc_gate_sphere50TC::Result sphere_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,bound_=false,ready_=false;
 static bool fail(Diagnostic& d,const char* message,std::uint32_t pc)noexcept {
  d.sourcePC=pc;try{d.message=message;}catch(...){}return false;
 }
 bool bounds(g::LiveZoneBounds50EN& out,Diagnostic& d)noexcept {
  if(!gate_||!gate_->source_||!gate_->source_->ready()||gate_->nodes_.size()!=8)return fail(d,"TC gate source graph unavailable",0x26f4b0);
  const auto graph=g::SceneGraph49P{gate_->nodes_.data(),8};
  if(!(gate_->nodes_[0].sceneFlags190&0x10u)||(gate_->nodes_[0].sceneFlags190&4u)||
      !g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(0)))return fail(d,"TC actual sphere root rejected",0x26f4b0);
  std::array<tc_gate_sphere50TC::Row,7> rows{};
  for(std::uint32_t i=0;i<7;++i){rr::assets::PsgNode49O source{};
   if(!(gate_->nodes_[i+1].sceneFlags190&4u)||
      !rr::assets::readPsgNode49O(gate_->source_->hierarchy(),i,source)||
      !g::refreshSceneWorld49P(graph,static_cast<g::SceneNodeIndex49P>(i+1)))return fail(d,"TC original sphere row rejected",0x28d940);
   rows[i].world=gate_->nodes_[i+1].world100;
   for(unsigned axis=0;axis<3;++axis)rows[i].center[axis]=source.bounds[axis];rows[i].radius=source.bounds[6];
  }
  tc_gate_sphere50TC::Result next{};
  if(!tc_gate_sphere50TC::compute(gate_->nodes_[0].world100,rows,next))return fail(d,"TC actual sphere projection rejected",0x26f4b0);
  sphere_=next;if(next.radius198>0.f)gate_->nodes_[0].sceneFlags190|=0x40u;
  //28E230 consumes center160/equal XYZ half170 through the existing affine
  //box arithmetic. Tag1 selects that numeric leaf, not a collision object.
  out={};out.bounds.shapeTag=1;out.bounds.shapeWorld=gate_->nodes_[0].world100;
  for(unsigned axis=0;axis<3;++axis){out.bounds.localCenter.v[axis]=next.center160[axis];out.bounds.localHalf.v[axis]=next.radius198;}
  out.bounds.cachedRadius=next.radius198;g::CollectionBoundsResult48S actual{};
  if(g::prepareCollectionBounds48S(out.bounds,actual)!=g::CollectionBoundsStatus48S::Complete)return fail(d,"TC actual observer box projection rejected",0x28e230);
  out.bounds.cachedCenter=actual.center;return true;
 }
public:
 Binding()=default;Binding(const Binding&)=delete;Binding&operator=(const Binding&)=delete;
 Binding(Binding&&)=delete;Binding&operator=(Binding&&)=delete;
 ~Binding(){if(bound_||thread_!=std::this_thread::get_id())std::terminate();}
 bool ownsRegistration()const noexcept{return bound_;} // Includes a claimed partial prefix requiring close.
 bool needsClose()const noexcept{return bound_;}
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&bound_&&gate_&&gate_->ready()&&lighting_&&lighting_->currentTcGateOwner(this);}
 bool prepare(std::shared_ptr<tc_gate_runtime50TC::Owner> gate,std::shared_ptr<lighting::Owner> lighting,
      g::ModelMotionSharedState49T& shared,Diagnostic& d)noexcept {
  bool access=false;
  try{
   d={};if(attempted_||thread_!=std::this_thread::get_id())return fail(d,"TC observer requires fresh creating-thread owner",0x2d935c);attempted_=true;
   if(!gate||!lighting||!gate->initialSamplePending_||gate->initialSampleAttempted_||gate->initialShared_!=&shared||
      gate->failed_||gate->servicing_||gate->thread_!=thread_||!gate->source_||!gate->source_->ready()||
      !gate->source_->coldSource()||!gate->source_->coldSource()->ready()||!lighting->world()||
      lighting->world()->parameters.original_bytes!=gate->source_->coldSource()->world().parameters.original_bytes||
      gate->source_->authored().startingZone!=8||gate->source_->authored().useCollision)
    return fail(d,"TC exact source/lighting/pending graph identity rejected",0x2d935c);
   gate_=std::move(gate);lighting_=std::move(lighting);
   if(!lighting_->bindTcGateOwner(this,&membership_))return fail(d,"TC lighting observer issuer unavailable",0x28e8a0);bound_=true;
   g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* workspace=nullptr;g::CollectionRef50BX object=0;
   if(!lighting_->beginTcGateAccess(this,world,workspace,object))return fail(d,"TC observer access unavailable",0x28e8a0);access=true;
   g::LiveZoneBounds50EN actual{};
   if(!bounds(actual,d)){lighting_->endTcGateAccess(this);return false;}
   const g::LiveZoneStorage50EN storage{links_.data(),entries_.data(),links_.size()};
   d.registry=g::attachLiveZoneMembership50EN(world->lifecycle,world->portals,membership_,storage,object,8,
       g::ZoneMethod50CL::Second,0.f,actual,*workspace);
   if(d.registry.status!=g::LiveZoneStatus50EN::Complete){lighting_->endTcGateAccess(this);return fail(d,"TC actual observer attachment failed",0x2809c0);}
   tc_gate_runtime50TC::Diagnostic sampled;
   if(!gate_->completeInitialSample(shared,sampled)){lighting_->endTcGateAccess(this);d.sourcePC=sampled.sourcePC;d.message=sampled.message;return false;}
   if(!lighting_->commitTcGateOwner(this)){lighting_->endTcGateAccess(this);return fail(d,"TC actual observer publication failed",0x2d9480);}
   lighting_->endTcGateAccess(this);access=false;ready_=true;return true;
  }catch(...){if(access&&lighting_)lighting_->endTcGateAccess(this);return fail(d,"TC observer preparation exception; prefix retained",0x2d935c);}
 }
 bool update(Diagnostic& d)noexcept {
  d={};if(!ready())return fail(d,"TC actual gate observer is not ready",0x11ad80);
  g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* workspace=nullptr;g::CollectionRef50BX object=0;
  if(!lighting_->beginTcGateAccess(this,world,workspace,object))return fail(d,"TC observer update access unavailable",0x28ea20);
  ready_=false;g::LiveZoneBounds50EN actual{};
  if(!bounds(actual,d)){lighting_->endTcGateAccess(this);return false;}
  if(!membership_.attached||membership_.object!=object){lighting_->endTcGateAccess(this);return fail(d,"TC actual observer identity changed",0x28ea20);}
  d.registry=g::updateLiveZoneMembership50EN(world->lifecycle,world->portals,membership_,actual,*workspace);
  if(d.registry.status!=g::LiveZoneStatus50EN::Complete){lighting_->endTcGateAccess(this);return fail(d,"TC actual observer update failed",0x280d40);}
  const bool committed=lighting_->commitTcGateOwner(this);lighting_->endTcGateAccess(this);
  if(!committed)return fail(d,"TC observer update publication failed",0x280d40);ready_=true;return true;
 }
 bool close(Diagnostic& d)noexcept {
  d={};if(thread_!=std::this_thread::get_id())return fail(d,"TC observer retirement thread mismatch",0x11adc0);
  ready_=false;if(!bound_){gate_.reset();lighting_.reset();return true;}
  g::ResetVisibilityWorld50EY* world=nullptr;g::LiveZoneWorkspace50EN* workspace=nullptr;g::CollectionRef50BX object=0;
  if(!lighting_||!lighting_->beginTcGateAccess(this,world,workspace,object))return fail(d,"TC observer retirement access unavailable",0x28ea80);
  d.registry=g::removeLiveZoneMembership50EN(world->lifecycle,world->portals,membership_,*workspace);lighting_->endTcGateAccess(this);
  if(d.registry.status!=g::LiveZoneStatus50EN::Complete)return fail(d,"TC actual observer removal failed",0x281490);
  if(!lighting_->releaseTcGateOwner(this))return fail(d,"TC observer issuer release failed",0x28eab0);
  bound_=false;gate_.reset();lighting_.reset();return true;
 }
};
}

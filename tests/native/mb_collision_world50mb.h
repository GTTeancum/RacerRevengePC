#pragma once
#include "track_static_candidates50ig_fixture.h"
#include "track_navigation_view50mb.h"
#include "mb_trigger_authoring_fixture.h"
#include "native_trigger_group50ga.h"
#include <memory>
#include <array>
#include <cstring>
#include <thread>
#include <span>
#include <cmath>

namespace mb_source_trigger50MB { class Owner; }
namespace mb_collision_world50MB {
namespace g=rr::game;
// One real source/topology lifetime. Navigation retains this aggregate only
// after preparation; this aggregate never retains navigation or its action
// owner. No view can keep Storage alive while dropping its mesh/name backing.
class Owner final : public std::enable_shared_from_this<Owner> {
 friend class ::mb_source_trigger50MB::Owner;
 authored_runtime_ground50cp_fixture::ColdScene ground_;
 std::vector<g::Type2StaticCandidate49X> candidates_;
 std::vector<track_static_candidates50IG::Radii> radii_;
 std::vector<track_runtime_storage50IG::Binding> bindings_;
 mb_trigger_authoring::Owner source_;
 g::NativeTriggerGroup50GA triggers_{};
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,3> triggerMemberships_;
 track_runtime_storage50IG::Owner storage_;
 const mb_source_trigger50MB::Owner* actionOwner_=nullptr;
 const std::thread::id ownerThread_=std::this_thread::get_id();
 bool attempted_=false,groundReady_=false,storageAttempted_=false,ready_=false;
 static constexpr std::uint32_t triggerIdentity=0x36000000u;
public:
 ~Owner();
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool prepareGround(const authored_runtime_ground50cp_fixture::Request& request,
     const g::TrackNavigationView50EU& map,const track_static_candidates50IG::Bindings& names,
     std::string& error) {
  if(std::this_thread::get_id()!=ownerThread_){error="MB collision owner thread mismatch";return false;}
  if(attempted_){error="MB collision owner requires a fresh stable lifetime";return false;}
  attempted_=true;
  try {
   if(!map.gatesPrepared()||map.role()!=g::NavigationRole50MB::Map||map.count()!=265||
      !request.archiveSha256||std::strcmp(request.archiveSha256,mb_trigger_authoring::archiveSha)||
      request.archiveBytes!=mb_trigger_authoring::archiveBytes||!request.worldMember||
      std::strcmp(request.worldMember,"tracks\\mb.scb")||!request.mapSpline||
      std::strcmp(request.mapSpline,"tracks\\mmb.spl")||request.gridRank!=4||
      !request.originalPodPhysics)
    throw std::runtime_error("MB original source/gate/PHY identity mismatch");
   // This is the existing neutral diagnostic grid rank, not a race ranking.
   // Only the legacy reader's extent changes; its center-pose arithmetic is
   // reused unchanged with the actual authenticated first-next predecessor.
   ground_=authored_runtime_ground50cp_fixture::detail::loadWithGrid({4,16,32,64},request,
     [&](const rr::assets::ResArchive48C&,g::GridCenterInput50AC& grid){
      const auto graph=map.track();std::uint32_t previous=0;
      for(;previous<graph.count();++previous){const auto& n=graph.nodes()[previous];
       if(!n.nextCount||n.nextCount>g::maxUndertrackNext50ET)
        throw std::runtime_error("MB original grid predecessor invalid");
       if(n.next[0]==0)break;
      }
      if(previous==graph.count()||!graph.nodes()[0].nextCount)
       throw std::runtime_error("MB original grid predecessor missing");
      std::memcpy(grid.current,graph.nodes()[0].position,sizeof(grid.current));
      std::memcpy(grid.previous,graph.nodes()[previous].position,sizeof(grid.previous));
      grid.rank=request.gridRank;grid.nextNode=graph.nodes()[0].next[0];
     });
   if(ground_.world.zones.size()!=16||ground_.world.portals.size()!=17||ground_.owners.size()!=128)
    throw std::runtime_error("MB actual collision/topology extent changed");
   if(!source_.load(request.archive,292,16,error))return false;
   track_static_candidates50IG::prepare(ground_,names,candidates_,radii_,bindings_);
   if(!triggers_.triggers.setCount(3))throw std::runtime_error("MB trigger count unavailable");
   candidates_.resize(131);bindings_.resize(131);
   for(std::size_t i=0;i<128;++i)bindings_[i].candidate=&candidates_[i];
   for(std::size_t i=0;i<3;++i){const auto& authored=source_.source(i);auto& state=triggers_.triggers[i];
    state.flags29=3;state.flags1B8=3;state.randomTriggerPercent90=100;state.callbackCount98=1;
    state.collider.identity=triggerIdentity+static_cast<std::uint32_t>(i);
    auto& target=candidates_[128+i];target.shapeType=authored.shapeType;
    target.nativeTriggers=&triggers_;target.nativeTriggerIndex=i;
    auto& c=target.candidate.collision;c.shapeType=authored.shapeType;
    c.geometry.id=state.collider.identity;c.filter.identity=c.geometry.id;
    c.ownerIdentity9C=0x37000000u+static_cast<std::uint32_t>(i);c.typeTag=names.collisionTypeTag;
    c.filter.categoryA4=8;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
    c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
    c.filter.flags31=static_cast<std::uint8_t>(names.preservedFlags31|3u);
    c.filter.flagsF8=static_cast<std::uint8_t>(names.preservedFlagsF8|6u);
    c.hasResponseCallbackE8=true;c.sphere=authored.sphere;
    c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
    for(unsigned n=0;n<16;n+=5)c.geometry.worldMatrix.v[n]=1.f;
    bindings_[128+i]={std::numeric_limits<std::size_t>::max(),&target,false,authored.zone,0,g::ZoneMethod50CL::First,true};
   }
   // Fallible action authority is bound later by the genuine source owner.
   // No legacy infallible action callback is installed or made ready here.
   groundReady_=true;return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool prepareStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
     const track_runtime_storage50IG::Limits& limits,track_runtime_storage50IG::Diagnostic& d) noexcept {
  if(std::this_thread::get_id()!=ownerThread_||!groundReady_||storageAttempted_)return false;storageAttempted_=true;
  if(!storage_.prepare(std::move(application),ground_,bindings_.data(),bindings_.size(),limits,d))return false;
  try {for(auto& m:triggerMemberships_)m=std::make_unique<track_runtime_storage50IG::Membership>(storage_.get()->count.membership);}
  catch(...){return false;}
  ready_=true;return true;
 }
 bool attachTriggers() noexcept {
  if(!ready()||actionOwner_)return false;
  auto& storage=*storage_.get();
  for(std::size_t i=0;i<3;++i){auto& membership=*triggerMemberships_[i];const auto& source=source_.source(i);
   if(membership.state.attached||membership.state.object)return false;
   g::LiveZoneBounds50EN bounds{};bounds.bounds.shapeTag=source.shapeType;
   for(unsigned k=0;k<4;++k)bounds.bounds.shapeWorld.v[k*5]=1.f;
   for(unsigned k=0;k<3;++k){bounds.bounds.localCenter.v[k]=source.sphere.center.v[k];bounds.bounds.localHalf.v[k]=source.sphere.radius;bounds.bounds.cachedCenter.v[k]=source.sphere.center.v[k];}
   bounds.bounds.localCenter.v[3]=bounds.bounds.localHalf.v[3]=bounds.bounds.cachedCenter.v[3]=1.f;bounds.bounds.cachedRadius=source.sphere.radius;
   const auto attached=g::attachLiveZoneMembership50EN(storage.lifecycle,storage.world,membership.state,
    membership.storage(),static_cast<g::CollectionRef50BX>(129+i),source.zone,g::ZoneMethod50CL::First,0.f,bounds,storage.membershipWork);
   if(attached.status!=g::LiveZoneStatus50EN::Complete)return false;
   const auto updated=g::updateLiveZoneMembership50EN(storage.lifecycle,storage.world,membership.state,bounds,storage.membershipWork);
   if(updated.status!=g::LiveZoneStatus50EN::Complete)return false;
  }
  return true;
 }
 // Actual mutable membership spans are enrolled in the existing serial Bank,
 // just like MA/SA trigger memberships. No copied membership is published.
 track_runtime_storage50IG::Membership* triggerMembership(std::size_t i)noexcept {
  return ready()&&i<3?triggerMemberships_[i].get():nullptr;
 }
 bool ready()const noexcept{return ready_&&std::this_thread::get_id()==ownerThread_;}
 const auto& originalTob()const noexcept{return source_.originalTob();}
 const authored_runtime_ground50cp_fixture::ColdScene& ground()const noexcept{return ground_;}
 std::span<g::Type2StaticCandidate49X> candidateStorage() noexcept{return candidates_;}
 std::span<const track_runtime_storage50IG::Binding> bindingStorage()const noexcept{return bindings_;}
 const std::vector<track_runtime_storage50IG::Binding>& bindings()const noexcept{return bindings_;}
 bool setHumanLift(float value) noexcept {
  if(!ready()||!std::isfinite(value))return false;ground_.lift=value;return true;
 }
 track_runtime_storage50IG::Storage* storage()noexcept{return ready_?storage_.get():nullptr;}
 const track_runtime_storage50IG::Storage* storage()const noexcept{return ready_?storage_.get():nullptr;}
 const g::LiveZoneWorld50EN& world()const noexcept{return storage_.get()->world;}
 g::MbCollisionWorldLease lease()const noexcept {
  if(!ready())return {};
  auto self=weak_from_this().lock();if(!self)return {};
  return g::MbCollisionWorldLease(std::move(self),storage_.get()->world,1);
 }
 g::Type2StaticCandidate49X* candidates()noexcept{return ready_?candidates_.data():nullptr;}
 std::size_t candidateCount()const noexcept{return ready_?candidates_.size():0;}
};
} // namespace mb_collision_world50MB

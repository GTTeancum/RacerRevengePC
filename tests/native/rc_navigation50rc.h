#pragma once
#include "rc_cold_sources50la.h"
#include "../../src/game/rc_navigation_lease50rc.h"
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <thread>

namespace rc_navigation50RC {
namespace g=rr::game;
class Owner;class RuntimeBorrow;
namespace detail {
class Final final {
 friend class ::rc_navigation50RC::Owner;friend class ::rc_navigation50RC::RuntimeBorrow;
 g::TrackNavigation50EU map{},ai{};
 const g::RcCollisionWorldLease world;
 explicit Final(const g::RcCollisionWorldLease& lease):world(lease){}
};
}
// This closed borrow owns the actual final aggregate lease, not just pointers.
// Its returned legacy references do not carry authority by themselves: the
// root retains this SAME immutable borrow through every unfinished state and
// rechecks valid()/sameWorld before effects, callbacks and continuation entry.
class RuntimeBorrow final {
 std::shared_ptr<const rc_cold_sources50LA::Owner> source_;
 std::shared_ptr<const detail::Final> final_;
 friend class Owner;
 RuntimeBorrow(std::shared_ptr<const rc_cold_sources50LA::Owner> source,
     std::shared_ptr<const detail::Final> final):source_(std::move(source)),final_(std::move(final)){}
 void requireValid()const noexcept{if(!valid())std::terminate();}
public:
 RuntimeBorrow()=default;RuntimeBorrow(const RuntimeBorrow&)=default;RuntimeBorrow(RuntimeBorrow&&)=default;
 RuntimeBorrow& operator=(const RuntimeBorrow&)=delete;RuntimeBorrow& operator=(RuntimeBorrow&&)=delete;
 bool valid()const noexcept{return source_&&source_->ready()&&final_&&final_->world.valid()&&
  final_->world.sourceIdentity()==source_.get()&&final_->map.gatesPrepared&&final_->map.indicesPrepared&&
  final_->ai.gatesPrepared&&final_->ai.indicesPrepared&&final_->map.track.count==186&&final_->ai.track.count==209;}
 bool sameWorld(const g::LiveZoneWorld50EN& world)const noexcept{return valid()&&final_->world.world()==&world;}
 bool sameOwner(const RuntimeBorrow& other)const noexcept{return valid()&&other.valid()&&source_==other.source_&&final_==other.final_;}
 const rc_cold_sources50LA::Owner* sourceIdentity()const noexcept{return valid()?source_.get():nullptr;}
 std::uint64_t generation()const noexcept{return valid()?final_->world.generation():0;}
 const g::TrackNavigation50EU& map()const noexcept{requireValid();return final_->map;}
 const g::TrackNavigation50EU& ai()const noexcept{requireValid();return final_->ai;}
 const g::AiTrackMetadata50IU& mapMetadata()const noexcept{requireValid();return source_->navigation_->mapMetadata;}
 const g::AiTrackMetadata50IU& aiMetadata()const noexcept{requireValid();return source_->navigation_->aiMetadata;}
 const g::LapProgressTrack50GF& mapLap()const noexcept{requireValid();return source_->navigation_->mapLap;}
};
enum class Phase : std::uint8_t {Empty,Source,Prepared,Topology,MapIndices,AiIndices,RuntimeReady,Closed};
struct Diagnostic {Phase phase=Phase::Empty;std::string message;g::TrackNavigationDiagnostic50EU navigation{};};
// Aggregate owns cold source only; final navigation owns aggregate. No cycle.
class Owner final {
 std::shared_ptr<const rc_cold_sources50LA::Owner> source_;
 std::shared_ptr<const detail::Final> final_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool prepareAttempted_=false,bindAttempted_=false;Phase phase_=Phase::Empty;
 bool fail(Diagnostic& d,const char* message){d.phase=phase_;d.message=message;return false;}
 bool topologyMatches(const g::LiveZoneWorld50EN& actual)const {
  if(actual.zoneCount!=15||actual.portalCount!=15||actual.portalLinkCount!=30||
      !actual.portalHeads||!actual.portalLinks||!actual.portals)return false;
  g::CollectionRef50BX heads[15]{};g::LiveZonePortalLink50EN links[30]{};
  g::PortalGeometry50EO portals[15]{};g::LiveZoneWorld50EN expected{};
  const g::LiveZoneWorldStorage50EN backing{heads,15,links,30,portals,15};
  if(g::prepareLiveZoneWorld50EN(source_->navigation_->portals,15,15,backing,expected)!=g::LiveZoneStatus50EN::Complete)return false;
  for(std::size_t i=0;i<15;++i)if(actual.portalHeads[i]!=heads[i])return false;
  for(std::size_t i=0;i<30;++i){const auto& p=actual.portalLinks[i];
   if(p.next!=links[i].next||p.zone!=links[i].zone||p.portal!=links[i].portal)return false;}
  for(std::size_t i=0;i<15;++i)
   if(std::memcmp(actual.portals[i].vertices,portals[i].vertices,sizeof portals[i].vertices)||
      std::memcmp(&actual.portals[i].normal,&portals[i].normal,sizeof portals[i].normal)||
      std::memcmp(actual.portals[i].edgeNormals,portals[i].edgeNormals,sizeof portals[i].edgeNormals))return false;
  return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t persistentPayloadBytes()noexcept{return sizeof(detail::Final)+256;}
 static constexpr std::size_t ownerBytes()noexcept{return sizeof(Owner);}
 static constexpr std::size_t topologyScratchBytes()noexcept{return 15*sizeof(g::CollectionRef50BX)+30*sizeof(g::LiveZonePortalLink50EN)+15*sizeof(g::PortalGeometry50EO)+sizeof(g::LiveZoneWorld50EN);}
 Phase phase()const noexcept{return thread_==std::this_thread::get_id()?phase_:Phase::Empty;}
 bool runtimeReady()const noexcept{return thread_==std::this_thread::get_id()&&source_&&source_->ready()&&final_&&final_->world.valid()&&final_->world.sourceIdentity()==source_.get();}
 bool prepare(std::shared_ptr<const rc_cold_sources50LA::Owner> source,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id())return fail(d,"RC navigation thread mismatch");
  if(prepareAttempted_)return fail(d,"RC navigation source preparation is one-attempt");
  prepareAttempted_=true;phase_=Phase::Source;
  if(!source||!source->ready()||source->map().count()!=186||source->ai().count()!=209)return fail(d,"RC authentic complete cold source required");
  source_=std::move(source);phase_=Phase::Prepared;d.phase=phase_;return true;
 }
 bool bindWorld(const g::RcCollisionWorldLease& lease,std::size_t availableAdditionalPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id())return fail(d,"RC navigation thread mismatch");
  if(!source_||!source_->ready()||bindAttempted_)return fail(d,"RC world binding requires one prepared source owner");
  bindAttempted_=true;phase_=Phase::Topology;
  if(availableAdditionalPayloadBytes<persistentPayloadBytes()+topologyScratchBytes())return fail(d,"RC final navigation exceeds reserved existing payload budget");
  if(!lease.valid()||lease.sourceIdentity()!=source_.get()||!topologyMatches(*lease.world()))return fail(d,"RC same-source actual-world topology mismatch");
  try {
   auto next=std::shared_ptr<detail::Final>(new detail::Final(lease));
   next->map=source_->navigation_->map;next->ai=source_->navigation_->ai;
   phase_=Phase::MapIndices;
   if(!g::bindTrackNavigationZones50EU(*lease.world(),next->map,d.navigation))return fail(d,"RC MAP legacy zone indices rejected");
   phase_=Phase::AiIndices;
   if(!g::bindTrackNavigationZones50EU(*lease.world(),next->ai,d.navigation))return fail(d,"RC AI legacy zone indices rejected");
   final_=std::move(next);phase_=Phase::RuntimeReady;d.phase=phase_;return true;
  }catch(const std::exception& e){return fail(d,e.what());}
 }
 RuntimeBorrow borrow()const noexcept{return runtimeReady()?RuntimeBorrow{source_,final_}:RuntimeBorrow{};}
 bool close()noexcept {
  if(thread_!=std::this_thread::get_id()||(final_&&final_.use_count()!=1))return false;
  final_.reset();source_.reset();prepareAttempted_=bindAttempted_=true;phase_=Phase::Closed;return true;
 }
};
}

#pragma once
// Common track-animation COLLISION LANE inside a course collision world (the worlds that keep
// their own track_runtime_storage50IG Storage: TB, TC, GB, MC). One copy of what the TB door,
// GB four and MC fourteen owners each did: one compound CHARACTER candidate per row with
// UseCollision (2D93F0 -> 120070 method 2), registered at its StartingZone, refreshed when the
// row moved (11AD80 / 120230), all from the course's own [Animations] table.
// Candidates and identities are track_animation_candidates50an.h's (the GA/GC/SA route), so a
// row's candidate is the same on every course whichever storage holds it.
#include "track_animation_candidates50an.h"
#include "track_animation_query_test50an.h"
#include "track_runtime_storage50ig_fixture.h"
#include "collection_projection50cf.h"
#include <cstring>
#include <memory>
#include <vector>

namespace track_animation_world50AN {
namespace g=rr::game;namespace rt=track_runtime_storage50IG;namespace anim=track_animation_runtime50AN;
namespace cand=track_animation_candidates50AN;

class Lane final {
 std::shared_ptr<anim::Owner> runtime_;
 std::vector<std::size_t> rows_;                         // runtime rows with collision, table order
 std::vector<std::unique_ptr<rt::Membership>> memberships_;
 std::vector<g::LiveZoneMembership50EN> committed_;
 std::vector<g::Type2StaticCandidate49X>* candidates_=nullptr; // the world's candidate store (reserved, never relocated)
 std::size_t first_=0,capacity_=0;                       // candidate index of lane row 0; membership capacity
 bool prepared_=false;
 bool queryTest_=false;
 std::array<unsigned,3> querySamples_{};

public:
 // Pre-Storage charge for `rows` collision rows (memberships + their links/entries).
 static std::size_t payloadBytes(std::size_t rows,std::size_t membershipCapacity)noexcept{
  const std::size_t per=sizeof(rt::Membership)+membershipCapacity*(sizeof(g::CollectionZoneLink50BX)+sizeof(g::LiveZoneEntry50EN));
  return rows&&per>std::numeric_limits<std::size_t>::max()/rows?std::numeric_limits<std::size_t>::max():rows*per;
 }
 static std::size_t collisionRowCount(const anim::Owner* runtime)noexcept{return runtime?cand::collisionRowCount(*runtime):0u;}
 // The world's prepareStorage, after its ground + trigger candidates; candidates must already
 // reserve first+rows (no relocation). Returns the first lane candidate index.
 std::size_t prepareCandidates(std::shared_ptr<anim::Owner> runtime,std::vector<g::Type2StaticCandidate49X>& candidates,
     std::vector<rt::Binding>& bindings,std::uint8_t tag,std::uint8_t flags31,std::uint8_t flagsF8){
  if(prepared_||(runtime&&!runtime->ready()))throw std::runtime_error("track animation lane requires a fresh lane and a ready runtime");
  prepared_=true;runtime_=std::move(runtime);first_=candidates.size();candidates_=&candidates;
  if(!runtime_)return first_;
  rows_=cand::collisionRows(*runtime_);
  if(candidates.capacity()<first_+rows_.size())throw std::runtime_error("track animation lane candidates would relocate");
  const auto first=cand::prepare(*runtime_,candidates,bindings,tag,flags31,flagsF8);
  if(first!=first_)throw std::runtime_error("track animation lane candidate order changed");
  return first_;
 }
 // After Storage: one membership per collision row (capacity = the storage's membership count).
 void makeMemberships(std::size_t capacity){
  const char* flag=std::getenv("RR_TEST_MOVING_COLLISION");
  queryTest_=flag&&std::string(flag)=="1"&&runtime_&&runtime_->label()=="NS";
  querySamples_={};
  capacity_=capacity;memberships_.clear();committed_.assign(rows_.size(),{});
  for(std::size_t k=0;k<rows_.size();++k)memberships_.push_back(std::make_unique<rt::Membership>(capacity));
 }
 std::size_t count()const noexcept{return rows_.size();}
 std::size_t first()const noexcept{return first_;}
 std::size_t row(std::size_t k)const{return rows_.at(k);}
 // Lane index of a runtime row, or count() when the row has no collision.
 std::size_t laneOf(std::size_t row)const noexcept{for(std::size_t k=0;k<rows_.size();++k)if(rows_[k]==row)return k;return rows_.size();}
 g::CollectionRef50BX object(std::size_t k)const noexcept{return static_cast<g::CollectionRef50BX>(first_+k+1);}
 const anim::Owner* runtime()const noexcept{return runtime_.get();}
 rt::Membership* membership(std::size_t k)noexcept{return k<memberships_.size()?memberships_[k].get():nullptr;}
 const rt::Membership* membership(std::size_t k)const noexcept{return k<memberships_.size()?memberships_[k].get():nullptr;}
 std::size_t payloadBytes()const noexcept{return payloadBytes(rows_.size(),capacity_);}
 bool attach(rt::Storage& s,std::size_t k,g::LiveZoneResult50EN& result)noexcept{
  if(!runtime_||k>=rows_.size()||!memberships_[k])return false;auto& m=*memberships_[k];
  const auto bounds=cand::bounds(*runtime_,rows_[k]);
  result=g::attachLiveZoneMembership50EN(s.lifecycle,s.world,m.state,m.storage(),object(k),
    cand::authoredZone(*runtime_,rows_[k]),g::ZoneMethod50CL::Second,0.f,bounds,s.membershipWork);
  return result.status==g::LiveZoneStatus50EN::Complete&&m.state.attached&&m.state.object==object(k);
 }
 // 11AD80 / 120230: candidate geometry from the row's current compound, the zone membership,
 // then the movement mirror and the native query-cache projection (query epochs/flags kept).
 bool update(rt::Storage& s,std::size_t k,g::LiveZoneResult50EN& result)noexcept{
  try{
   if(!runtime_||k>=rows_.size()||!memberships_[k])return false;auto& m=*memberships_[k];const auto index=first_+k;
   if(index>=s.count.terrain||!s.candidates[index])return false;
   auto& candidate=*s.candidates[index];
   if(candidate.shapeType!=1||candidate.compound!=&runtime_->state(rows_[k]).collision.view)return false;
   if(!candidates_||&(*candidates_)[index]!=&candidate)return false;
   cand::sync(*runtime_,candidates_->data(),candidates_->size(),first_);
   const auto& state=runtime_->state(rows_[k]);
   if(!state.collisionWanted){ // PORT EXTENSION (b), N64 data only: hidden with its collision off
    if(m.state.attached){result=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,m.state,s.membershipWork);
     return result.status==g::LiveZoneStatus50EN::Complete;}
    return true;}
   if(!m.state.attached)return attach(s,k,result)&&update(s,k,result);
   if(m.state.object!=object(k))return false;
   const auto bounds=cand::bounds(*runtime_,rows_[k]);
   const auto& current=candidate.candidate.collision;
   const auto projected=g::projectCollectionObject50CF(current.filter,current.sphere,current.typeTag,1,0);
   if(!projected.valid)return false;
   // PORT STAND-IN (flagged): 11ADC0/11AD20 re-home a wrapped looping row that left its
   // StartingZone; the ordinary membership update carries it (no stock row leaves its zone).
   result=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,m.state,bounds,s.membershipWork);
   if(result.status!=g::LiveZoneStatus50EN::Complete)return false;
   auto& movement=s.movement[index];movement.collision.geometry.worldMatrix=current.geometry.worldMatrix;movement.collision.sphere=current.sphere;
   auto& objectView=s.objects[index];for(unsigned c=0;c<3;++c)objectView.position[c]=projected.object.position[c];
   objectView.scale10=projected.object.scale10;objectView.scale11=projected.object.scale11;
   auto& cached=s.source[index];std::memcpy(cached.cachedCenter.v,projected.state.cachedCenter.v,3*sizeof(float));cached.cachedRadius=projected.state.cachedRadius;
   committed_[k]=m.state;
   if(queryTest_&&k<querySamples_.size()){
    constexpr float times[]={1.f,11.f,26.f};
    auto& sample=querySamples_[k];
    if(sample<3&&state.time34>=times[sample]){
     if(!track_animation_query_test50AN::contact(*candidate.compound,candidate.candidate.collision.filter.identity,
           rows_[k],state.time34,sample))return false;
     ++sample;
    }
   }
   return true;
  }catch(...){return false;}
 }
 bool remove(rt::Storage& s,std::size_t k,g::LiveZoneResult50EN& result)noexcept{
  if(k>=memberships_.size()||!memberships_[k])return true;auto& m=*memberships_[k];
  if(!m.state.attached&&!m.state.object)return true;
  result=g::removeLiveZoneMembership50EN(s.lifecycle,s.world,m.state,s.membershipWork);
  return result.status==g::LiveZoneStatus50EN::Complete&&!m.state.attached&&!m.state.object;
 }
 bool detached()const noexcept{for(const auto& m:memberships_)if(m&&(m->state.attached||m->state.object))return false;return true;}
 // The world's current-publication check for the lane: every row's candidate is its compound
 // at the committed pose and its membership is the committed one.
 bool current(const rt::Storage& s)const noexcept{
  if(!runtime_)return rows_.empty();
  for(std::size_t k=0;k<rows_.size();++k){const auto index=first_+k;
   if(index>=s.count.terrain||!memberships_[k]||!s.candidates[index])return false;
   const auto& view=runtime_->state(rows_[k]).collision.view;const auto& candidate=*s.candidates[index];
   if(candidate.shapeType!=1||candidate.compound!=&view||s.movement[index].compound!=&view)return false;
   const auto& m=memberships_[k]->state;
   if(runtime_->state(rows_[k]).collisionWanted&&(!m.attached||m.object!=object(k)||m.revision!=committed_[k].revision))return false;
  }
  return true;
 }
 void reset()noexcept{memberships_.clear();committed_.clear();rows_.clear();runtime_.reset();candidates_=nullptr;}
};

// What the common binding needs from a course collision world that holds a Lane.
class World {
public:
 virtual ~World()=default;
 virtual Lane* animationLane()noexcept=0;
 virtual rt::Storage* animationStorage()noexcept=0;
 virtual bool animationIdle()const noexcept=0;      // no query borrowed, queue empty
 virtual bool animationPending()const noexcept=0;   // Storage + memberships made, lane not yet published
 virtual bool commitAnimationLane()noexcept=0;      // publication complete (world becomes Ready)
 virtual void blockAnimationPublication(bool)noexcept=0;
};
} // namespace track_animation_world50AN

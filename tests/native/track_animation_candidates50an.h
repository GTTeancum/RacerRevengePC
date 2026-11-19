#pragma once
// Common track-animation COLLISION candidates on the course's static-candidate storage: one
// compound (shape 1) CHARACTER candidate per row with UseCollision, registered at its StartingZone
// (2D93F0 -> 120070 method 2, radius 0), refreshed whenever the row moved (11AD80 / 120230).
// Generalises the GA and GC copies (ga_animated_candidates_fixture.h,
// gc_animated_candidates50ig_fixture.h): identities, filters and the non-body response admission
// are theirs, the row list comes from the track's own table.
#include "track_animation_runtime50an.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>

namespace track_animation_candidates50AN {
namespace g=rr::game;namespace rt=track_animation_runtime50AN;
// CHARACTER is the actual 3C8178/11AA94 category name. Native registry IDs in the namespace the GA
// and GC owners used; not claimed original bit positions or guest addresses.
inline constexpr std::uint32_t characterMask=1,firstCollider=rt::firstCollisionIdentity,firstModel=rt::firstModelIdentity;
// Rows with a registered candidate, in table order (candidate k = rows()[k]). Allocates:
// cold use only (the per-frame paths below walk the rows in place).
inline std::vector<std::size_t> collisionRows(const rt::Owner& owner){
 std::vector<std::size_t> out;for(std::size_t i=0;i<owner.rowCount();++i)if(owner.state(i).hasCollision)out.push_back(i);return out;}
inline std::size_t collisionRowCount(const rt::Owner& owner)noexcept{
 std::size_t n=0;for(std::size_t i=0;i<owner.rowCount();++i)if(owner.state(i).hasCollision)++n;return n;}
// The authored (1-based) StartingZone, which is the zone collection reference the storage and
// the zone registration use (the reader keeps the original's stored value, authored - 1).
inline std::uint32_t authoredZone(const rt::Owner& owner,std::size_t row){return owner.source().row(row).authored.startingZone+1;}
// Per-frame (11AD80 / 120230 refresh inside the player frame): no allocation, rows walked in place.
inline void sync(const rt::Owner& owner,g::Type2StaticCandidate49X* candidates,std::size_t size,std::size_t first){
 if(!owner.ready()||!candidates||first>size||size-first<collisionRowCount(owner))throw std::runtime_error("track animation candidate extent");
 for(std::size_t i=0,k=0;i<owner.rowCount();++i){if(!owner.state(i).hasCollision)continue;
  auto& c=candidates[first+k++].candidate.collision;const auto& state=owner.state(i).collision;
  c.geometry.worldMatrix=state.ownerWorld;c.sphere.center=state.sphereCenter;c.sphere.radius=state.radius;}
}
inline std::size_t prepare(const rt::Owner& owner,std::vector<g::Type2StaticCandidate49X>& candidates,
    std::vector<track_runtime_storage50IG::Binding>& bindings,std::uint8_t collisionTag,std::uint8_t flags31,std::uint8_t flagsF8){
 const auto rows=collisionRows(owner);
 if(!owner.ready()||candidates.size()!=bindings.size()||candidates.size()+rows.size()>track_runtime_storage50IG::hardTerrainOwners)
  throw std::runtime_error("track animation cold candidate admission (storage capacity)");
 const auto first=candidates.size();candidates.resize(first+rows.size());bindings.resize(first+rows.size());
 for(std::size_t i=0;i<first;++i)bindings[i].candidate=&candidates[i]; // cold resize may relocate the vector
 for(std::size_t k=0;k<rows.size();++k){const auto i=rows[k];auto& target=candidates[first+k];const auto& row=owner.source().row(i);
  const auto& state=owner.state(i);
  target.shapeType=1;target.compound=&state.collision.view;
  target.maxDepth=row.depth.empty()?0:*std::max_element(row.depth.begin(),row.depth.end());
  target.candidate.hasMaterialScene=true;
  target.candidate.sceneName=row.scene.nodes.front().name.data();target.candidate.sceneNameBytes=row.scene.nodes.front().name.size();
  auto& c=target.candidate.collision;c.shapeType=1;c.compound=target.compound;
  c.geometry.id=firstCollider+static_cast<std::uint32_t>(k);c.filter.identity=c.geometry.id;
  c.filter.categoryA4=characterMask;c.filter.acceptMaskB8=0xffffffffu;
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  //11B190 ->11B570(5) ->11A988 forces bit0: dynamic CHARACTER target; caller's top two bits kept.
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|5u);
  c.filter.categoryModeD4=1;c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  c.ownerIdentity9C=firstModel+static_cast<std::uint32_t>(k);c.typeTag=collisionTag;c.hasResponseCallbackE8=false;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  //11B424 MaterialA9=0; no stock TOB overrides it.
  bindings[first+k]={std::numeric_limits<std::size_t>::max(),&target,true,authoredZone(owner,i),0,g::ZoneMethod50CL::Second};
 }
 sync(owner,candidates.data(),candidates.size(),first);return first;
}
// 294C20..294DD8 route CHARACTER through non-body records (see the GA/GC copies).
inline bool admitNonBodyResponse(const rt::Owner& owner,const g::Type2StaticCandidate49X* candidates,std::size_t size,std::size_t first,
    const g::Type2StaticQuery49X& main,const g::Type2StaticQuery49X& secondary)noexcept{
 try{const auto rows=collisionRows(owner);
  if(!owner.ready()||!candidates||first>size||size-first<rows.size())return false;
  const auto route=[](const g::Type2StaticQuery49X& q)noexcept{return q.callbackEnabled&&q.skipMask==8u&&q.dynamicMask==2u&&q.materialMask==4u;};
  if(!route(main)||!route(secondary))return false;
  for(std::size_t k=0;k<rows.size();++k){const auto& t=candidates[first+k];const auto& c=t.candidate.collision;
   if(t.shapeType!=1||t.compound!=&owner.state(rows[k]).collision.view||c.shapeType!=1||c.compound!=t.compound||
      c.filter.categoryA4!=characterMask||c.geometry.id!=firstCollider+k||c.filter.identity!=c.geometry.id||
      c.ownerIdentity9C!=firstModel+k||c.hasResponseCallbackE8||c.typeTag!=main.collisionTypeTag||c.typeTag!=secondary.collisionTypeTag)return false;}
  return true;
 }catch(...){return false;}
}
inline g::LiveZoneBounds50EN bounds(const rt::Owner& owner,std::size_t row){
 const auto& c=owner.state(row).collision;g::LiveZoneBounds50EN out{};
 out.bounds.shapeTag=1;out.bounds.shapeWorld=c.ownerWorld;
 for(unsigned i=0;i<3;++i){out.bounds.localCenter.v[i]=c.center.v[i];out.bounds.localHalf.v[i]=c.half.v[i];out.bounds.cachedCenter.v[i]=c.sphereCenter.v[i];}
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=c.radius;return out;
}
} // namespace track_animation_candidates50AN

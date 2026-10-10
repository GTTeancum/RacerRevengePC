#pragma once
#include "gc_animated_world50ig_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>

namespace gc_animated_candidates50IG {
namespace g=rr::game;
inline constexpr std::size_t count=gc_animated_world50IG::ownerCount;
// CHARACTER is the actual3C8178/11AA94 category name. These are explicit
// native registry IDs, consistent with existing PHYSICSBODY2/STATICOBJECT4;
// they are not claimed original bit positions or guest addresses.
inline constexpr std::uint32_t characterMask=1,firstCollider=0x20000000u,firstModel=0x21000000u;
// A separately owned Gate may follow this two-owner interval. Never touch
// or validate that trailing candidate as an animated compound.
inline void sync(const gc_animated_world50IG::Owner& owner,
    g::Type2StaticCandidate49X* candidates,std::size_t size,std::size_t first){
 if(!owner.ready()||!candidates||first>size||size-first<count)
  throw std::runtime_error("GC animated candidate extent");
 for(unsigned i=0;i<count;++i){auto& candidate=candidates[first+i];auto& c=candidate.candidate.collision;
  const auto& state=owner.state(i).collision;c.geometry.worldMatrix=state.ownerWorld;
  c.sphere.center=state.sphereCenter;c.sphere.radius=state.radius;
 }
}
inline void prepare(const gc_animated_world50IG::Owner& owner,
    std::vector<g::Type2StaticCandidate49X>& candidates,
    std::vector<track_runtime_storage50IG::Binding>& bindings,
    std::uint8_t collisionTag,std::uint8_t flags31,std::uint8_t flagsF8){
 if(!owner.ready()||candidates.size()!=bindings.size()||
    candidates.size()>track_runtime_storage50IG::hardTerrainOwners-count)
  throw std::runtime_error("GC animated cold candidate admission");
 const auto first=candidates.size();candidates.resize(first+count);bindings.resize(first+count);
 // Cold resize may relocate the vector; restore every original SCB borrow.
 for(std::size_t i=0;i<first;++i)bindings[i].candidate=&candidates[i];
 for(unsigned i=0;i<count;++i){auto& target=candidates[first+i];const auto& source=owner.source(i);
  target.shapeType=1;target.compound=&owner.collision(i);
  target.maxDepth=std::max(source.depth[0],source.depth[1]);
  target.candidate.hasMaterialScene=true;
  target.candidate.sceneName=source.scene.nodes.front().name.data();
  target.candidate.sceneNameBytes=source.scene.nodes.front().name.size();
  auto& c=target.candidate.collision;c.shapeType=1;c.compound=target.compound;
  c.geometry.id=firstCollider+i;c.filter.identity=c.geometry.id;
  c.filter.categoryA4=characterMask;c.filter.acceptMaskB8=0xffffffffu;
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  //11B190 ->11B570(5) ->11A988 forcesbit0: dynamic CHARACTER target,
  // not source static flag8. Preserve caller's opaque top two bits.
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|5u);
  c.filter.categoryModeD4=1;c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  c.ownerIdentity9C=firstModel+i;c.typeTag=collisionTag;c.hasResponseCallbackE8=false;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  //11B424 initializes MaterialA9=0; these exact TOB resources do not override it.
  bindings[first+i]={std::numeric_limits<std::size_t>::max(),&target,true,source.startingZone,0,g::ZoneMethod50CL::Second};
 }
 sync(owner,candidates.data(),candidates.size(),first);
}
//294C20..294DD8 routes CHARACTER through non-body records, not the
// PHYSICSBODY virtual callback. Pin the existing native category namespace
// before a session publishes these owners to either spring query. Animation
// does not imply a physical body or a target-side E8 response callback.
inline bool admitNonBodyResponse(const gc_animated_world50IG::Owner& owner,
    const g::Type2StaticCandidate49X* candidates,std::size_t size,std::size_t first,
    const g::Type2StaticQuery49X& main,const g::Type2StaticQuery49X& secondary) noexcept {
 if(!owner.ready()||!candidates||first>size||size-first<count)return false;
 // Named category bindings already shared by the native session: trigger8,
 // PHYSICSBODY2 and STATICOBJECT4. None aliases CHARACTER1.
 const auto queryRoute=[](const g::Type2StaticQuery49X& query) noexcept {
  return query.callbackEnabled&&query.skipMask==8u&&query.dynamicMask==2u&&query.materialMask==4u;
 };
 if(!queryRoute(main)||!queryRoute(secondary))return false;
 for(unsigned i=0;i<count;++i){const auto& target=candidates[first+i];const auto& c=target.candidate.collision;
  if(target.shapeType!=1||target.compound!=&owner.collision(i)||
     c.shapeType!=1||c.compound!=target.compound||c.filter.categoryA4!=characterMask||
     c.geometry.id!=firstCollider+i||c.filter.identity!=c.geometry.id||
     c.ownerIdentity9C!=firstModel+i||c.hasResponseCallbackE8||
     c.typeTag!=main.collisionTypeTag||c.typeTag!=secondary.collisionTypeTag)return false;
 }
 // Existing294BB0 non-body append supplies other44=0/material0, so the
 // bounded50E/50G main and50J secondary responses need no second body owner.
 return true;
}
inline g::LiveZoneBounds50EN bounds(const gc_animated_world50IG::Owner& owner,std::size_t index){
 const auto& c=owner.state(index).collision;g::LiveZoneBounds50EN out{};
 out.bounds.shapeTag=1;out.bounds.shapeWorld=c.ownerWorld;
 for(unsigned i=0;i<3;++i){out.bounds.localCenter.v[i]=c.center.v[i];out.bounds.localHalf.v[i]=c.half.v[i];out.bounds.cachedCenter.v[i]=c.sphereCenter.v[i];}
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;
 out.bounds.cachedRadius=c.radius;return out;
}
}

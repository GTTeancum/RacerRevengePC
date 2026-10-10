#pragma once
#include "gc_animated_world50ig_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>

namespace gc_gate_candidates50KG {
namespace g=rr::game;
// Explicit native identity/category namespace. Source registry category name
// is TRIGGER; these numeric IDs are not original guest addresses or mask bits.
inline constexpr std::uint32_t collider=0x22000000u,model=0x23000000u,triggerMask=8u;
inline std::size_t prepare(gc_animated_world50IG::Owner& owner,
 std::vector<g::Type2StaticCandidate49X>& candidates,
 std::vector<track_runtime_storage50IG::Binding>& bindings,
 std::uint8_t collisionTag,std::uint8_t flags31,std::uint8_t flagsF8){
 const auto& source=owner.gateSource();
 if(!owner.ready()||!source.authenticated||source.startingZone!=11||source.animationOwner!=0||
    candidates.size()!=bindings.size()||candidates.size()>=track_runtime_storage50IG::hardTerrainOwners)
  throw std::runtime_error("GC Gate cold source binding unavailable");
 const auto first=candidates.size();candidates.resize(first+1);bindings.resize(first+1);
 for(std::size_t i=0;i<first;++i)bindings[i].candidate=&candidates[i];
 auto& target=candidates[first];target.shapeType=5;target.gate=&owner.gateBinding();
 auto& c=target.candidate.collision;c.shapeType=5;c.geometry.id=collider;
 c.filter.identity=collider;c.ownerIdentity9C=model;c.typeTag=collisionTag;
 c.filter.categoryA4=triggerMask;c.filter.acceptMaskB8=0xffffffffu;
 c.filter.categoryModeD4=1;c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
 c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
 //11B190 then11B570(7): bits1/2 on; bits0/3/4/5 off. This Gate is
 // not an animated CHARACTER or a PHYSICSBODY. Preserve opaque high bits.
 c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|6u);
 c.hasResponseCallbackE8=true;c.sphere=source.sphere;
 c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
 for(unsigned i=0;i<16;i+=5)c.geometry.worldMatrix.v[i]=1.f;
 bindings[first]={std::numeric_limits<std::size_t>::max(),&target,false,
     source.startingZone,0,g::ZoneMethod50CL::First,true};
 return first;
}
inline g::LiveZoneBounds50EN bounds(const gc_animated_world50IG::Owner& owner){
 const auto& sphere=owner.gateSource().sphere;g::LiveZoneBounds50EN out{};
 out.bounds.shapeTag=5;
 for(unsigned i=0;i<16;i+=5)out.bounds.shapeWorld.v[i]=1.f;
 for(unsigned i=0;i<3;++i){
  out.bounds.localCenter.v[i]=out.bounds.cachedCenter.v[i]=sphere.center.v[i];
  out.bounds.localHalf.v[i]=sphere.radius;
 }
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;
 out.bounds.cachedRadius=sphere.radius;return out;
}
}

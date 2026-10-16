#pragma once
#include "gc_trigger_authoring50in_fixture.h"
#include "gc_trigger_actions50kg_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>
namespace gc_trigger_candidates50KG {
namespace g=rr::game;
inline std::size_t prepare(const gc_trigger_authoring50IN::Owner& source,
 gc_trigger_actions50KG::Owner& actions,std::vector<g::Type2StaticCandidate49X>& candidates,
 std::vector<track_runtime_storage50IG::Binding>& bindings,std::uint8_t tag,
 std::uint8_t flags31,std::uint8_t flagsF8){
 if(!source.ready()||candidates.size()!=bindings.size()||
    candidates.size()>track_runtime_storage50IG::hardTerrainOwners-gc_trigger_authoring50IN::count)
  throw std::runtime_error("GC native trigger cold owner extent");
 const auto first=candidates.size();candidates.resize(first+gc_trigger_authoring50IN::count);bindings.resize(candidates.size());
 for(std::size_t i=0;i<first;++i)bindings[i].candidate=&candidates[i];
 for(std::size_t i=0;i<gc_trigger_authoring50IN::count;++i){const auto& authored=source.source(i);auto& target=candidates[first+i];
  target.shapeType=5;target.nativeTriggers=&actions.nativeGroup();target.nativeTriggerIndex=i;
  auto& c=target.candidate.collision;c.shapeType=5;
  c.geometry.id=gc_trigger_actions50KG::firstTriggerIdentity+static_cast<std::uint32_t>(i);
  c.filter.identity=c.geometry.id;c.ownerIdentity9C=0x23000000u+static_cast<std::uint32_t>(i);c.typeTag=tag;
  c.filter.categoryA4=8u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  // Source trigger child-query scheduling remains in flags29. Its collider
  // target flags follow the same original11B190/11B570(7) construction as GC.
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|6u);
  c.hasResponseCallbackE8=true;c.sphere=authored.sphere;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  for(unsigned n=0;n<16;n+=5)c.geometry.worldMatrix.v[n]=1.f;
  bindings[first+i]={std::numeric_limits<std::size_t>::max(),&target,false,authored.zone,0,g::ZoneMethod50CL::First,true};
 }
 return first;
}
inline g::LiveZoneBounds50EN bounds(const gc_trigger_authoring50IN::Source& source){
 g::LiveZoneBounds50EN out{};out.bounds.shapeTag=5;
 for(unsigned n=0;n<16;n+=5)out.bounds.shapeWorld.v[n]=1.f;
 for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=out.bounds.cachedCenter.v[n]=source.sphere.center.v[n];out.bounds.localHalf.v[n]=source.sphere.radius;}
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=source.sphere.radius;return out;
}
}

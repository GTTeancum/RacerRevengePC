#pragma once
#include "ga_trigger_authoring_fixture.h"
#include "ga_trigger_actions_fixture.h"
#include "course_trigger_sources50ct.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>
namespace ga_trigger_candidates {
namespace g=rr::game;
// Sources come from the course's own TOB (course_trigger_sources50ct.h); the
// live count is the data's, bounded by the native owner store. Box triggers
// use the same shape6 construction as MA's chain.
inline std::size_t prepare(const course_trigger_sources50CT::Owner& source,
 ga_trigger_actions::Owner& actions,std::vector<g::Type2StaticCandidate49X>& candidates,
 std::vector<track_runtime_storage50IG::Binding>& bindings,std::uint8_t tag,
 std::uint8_t flags31,std::uint8_t flagsF8){
 const auto live=source.liveCount();
 if(!source.ready()||candidates.size()!=bindings.size()||live>track_runtime_storage50IG::hardTerrainOwners||
    candidates.size()>track_runtime_storage50IG::hardTerrainOwners-live)
  throw std::runtime_error("GA native trigger cold owner extent");
 const auto first=candidates.size();candidates.resize(first+live);bindings.resize(candidates.size());
 for(std::size_t i=0;i<first;++i)bindings[i].candidate=&candidates[i];
 for(std::size_t i=0;i<live;++i){const auto& authored=source.source(i);auto& target=candidates[first+i];
  target.shapeType=authored.shapeType;target.nativeTriggers=&actions.nativeGroup();target.nativeTriggerIndex=i;
  target.nativeTriggerBox=authored.box;
  auto& c=target.candidate.collision;c.shapeType=authored.shapeType;
  c.geometry.id=ga_trigger_actions::firstTriggerIdentity+static_cast<std::uint32_t>(i);
  c.filter.identity=c.geometry.id;c.ownerIdentity9C=0x33000000u+static_cast<std::uint32_t>(i);c.typeTag=tag;
  c.filter.categoryA4=8u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  // Source trigger child-query scheduling remains in flags29. Its collider
  // target flags follow the same original11B190/11B570(7) construction as GC.
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|6u);
  c.hasResponseCallbackE8=true;c.sphere=authored.sphere;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  for(unsigned n=0;n<16;n+=5)c.geometry.worldMatrix.v[n]=1.f;
  if(authored.shapeType==6)c.geometry.worldMatrix=authored.box.world;
  bindings[first+i]={std::numeric_limits<std::size_t>::max(),&target,false,authored.zone,0,g::ZoneMethod50CL::First,true};
 }
 return first;
}
inline g::LiveZoneBounds50EN bounds(const course_trigger_sources50CT::Source& source){
 g::LiveZoneBounds50EN out{};out.bounds.shapeTag=source.shapeType;
 for(unsigned n=0;n<16;n+=5)out.bounds.shapeWorld.v[n]=1.f;
 if(source.shapeType==6){out.bounds.shapeWorld=source.box.world;
  for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=source.box.center.v[n];out.bounds.localHalf.v[n]=source.box.half.v[n];}
 }else for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=source.sphere.center.v[n];out.bounds.localHalf.v[n]=source.sphere.radius;}
 for(unsigned n=0;n<3;++n)out.bounds.cachedCenter.v[n]=source.sphere.center.v[n];
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=source.sphere.radius;return out;
}
}

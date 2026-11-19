#pragma once
#include "mc_trigger_actions50mc.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>
namespace mc_trigger_candidates50MC {
namespace g=rr::game;
inline std::size_t prepare(const mc_trigger_authoring50MC::Owner& source,
 mc_trigger_actions50MC::Owner& actions,std::vector<g::Type2StaticCandidate49X>& candidates,
 std::vector<track_runtime_storage50IG::Binding>& bindings,std::uint8_t tag,
 std::uint8_t flags31,std::uint8_t flagsF8){
 // Live authored triggers only (capacity store); the COL prefix is whatever
 // the ground placed (stock 89 is the ground's receipt, not checked here).
 // Callers reserve the full join first so the resize below never relocates.
 const auto live=source.liveCount();
 if(!actions.queryIdle()||actions.nativeGroup().ready||!source.ready()||!actions.preparedSourceFor(source)||candidates.size()!=bindings.size()||
    candidates.size()>track_runtime_storage50IG::hardTerrainOwners||
    live>track_runtime_storage50IG::hardTerrainOwners-candidates.size()||
    candidates.capacity()<candidates.size()+live||bindings.capacity()<bindings.size()+live)
  throw std::runtime_error("MC native trigger COL prefix/source owner extent");
 // Cold append only. The aggregate owns all real memberships and must finish
 // this relocation before Storage admission or any Bank enrollment.
 const auto first=candidates.size();candidates.resize(first+live);bindings.resize(candidates.size());
 for(std::size_t i=0;i<first;++i)bindings[i].candidate=&candidates[i];
 for(std::size_t i=0;i<live;++i){const auto& sourceRow=source.source(i);const auto& authored=actions.geometryOwner()->shape(i);auto& target=candidates[first+i];
  target.shapeType=authored.shapeType;target.nativeTriggersMC=&actions.nativeGroup();target.nativeTriggerIndex=i;
  target.nativeTriggerBox=authored.box;
  auto& c=target.candidate.collision;c.shapeType=authored.shapeType;
  c.geometry.id=mc_trigger_actions50MC::firstTriggerIdentity+static_cast<std::uint32_t>(i);
  c.filter.identity=c.geometry.id;c.ownerIdentity9C=mc_trigger_actions50MC::firstTriggerModelIdentity+static_cast<std::uint32_t>(i);c.typeTag=tag;
  c.filter.categoryA4=8u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|6u);
  c.hasResponseCallbackE8=true;c.sphere=authored.sphere;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  for(unsigned n=0;n<16;n+=5)c.geometry.worldMatrix.v[n]=1.f;
  if(authored.shapeType==6)c.geometry.worldMatrix=authored.box.world;
  bindings[first+i]={std::numeric_limits<std::size_t>::max(),&target,false,sourceRow.zone,0,g::ZoneMethod50CL::First,true};
 }
 return first;
}
inline g::LiveZoneBounds50EN bounds(const mc_trigger_geometry50MC::Shape& source){
 g::LiveZoneBounds50EN out{};out.bounds.shapeTag=source.shapeType;
 for(unsigned n=0;n<16;n+=5)out.bounds.shapeWorld.v[n]=1.f;
 if(source.shapeType==6){out.bounds.shapeWorld=source.box.world;
  for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=source.box.center.v[n];out.bounds.localHalf.v[n]=source.box.half.v[n];}
 }else for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=source.sphere.center.v[n];out.bounds.localHalf.v[n]=source.sphere.radius;}
 for(unsigned n=0;n<3;++n)out.bounds.cachedCenter.v[n]=source.sphere.center.v[n];
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=source.sphere.radius;return out;
}
} // namespace mc_trigger_candidates50MC

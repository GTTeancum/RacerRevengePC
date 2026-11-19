#pragma once
#include "gb_trigger_actions50gb.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>
#include <stdexcept>

namespace gb_trigger_candidates50GB {
namespace g=rr::game;namespace source=gb_trigger_authoring50GB;
// groundPrefix: the placed COL candidates already prepared (the data's own
// count; stock 615). aggregateCount: the complete prepaid extent (ground +
// live triggers + animation owners; stock 615 + 18 + 4 = 637).
inline std::size_t prepare(const source::Owner& authored,gb_trigger_actions50GB::Owner& actions,
 std::vector<g::Type2StaticCandidate49X>& candidates,std::vector<track_runtime_storage50IG::Binding>& bindings,
 std::uint8_t tag,std::uint8_t flags31,std::uint8_t flagsF8,std::size_t groundPrefix,std::size_t aggregateCount){
 const auto live=authored.liveCount();
 if(!authored.ready()||!actions.preparedSourceFor(authored)||!actions.queryIdle()||actions.nativeGroup().ready||
    live!=actions.nativeGroup().triggers.size()||!groundPrefix||groundPrefix+live>aggregateCount||
    candidates.size()!=groundPrefix||bindings.size()!=groundPrefix||
    candidates.capacity()<aggregateCount||bindings.capacity()<aggregateCount||
    aggregateCount>track_runtime_storage50IG::hardTerrainOwners)
  throw std::runtime_error("GB actual Ground prefix and prepaid complete candidate spans required");
 // The real aggregate authenticates its Ground prefix. Capacity is prepaid
 // for the complete ground + triggers + animations construction before any
 // pointer is published. These resizes cannot grow either vector; the
 // animation rows (when authored) follow.
 candidates.resize(groundPrefix+live);bindings.resize(candidates.size());
 for(std::size_t i=0;i<groundPrefix;++i)bindings[i].candidate=&candidates[i];
 for(std::size_t i=0;i<live;++i){const auto& s=authored.source(i);auto& target=candidates[groundPrefix+i];
  target={};target.shapeType=5;target.nativeTriggersGB=&actions.nativeGroup();target.nativeTriggerIndex=i;
  auto& c=target.candidate.collision;c.shapeType=5;
  c.geometry.id=gb_trigger_actions50GB::firstTriggerIdentity+static_cast<std::uint32_t>(i);
  c.filter.identity=c.geometry.id;c.ownerIdentity9C=gb_trigger_actions50GB::firstTriggerModelIdentity+static_cast<std::uint32_t>(i);
  c.typeTag=tag;c.filter.categoryA4=8u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|6u);
  c.hasResponseCallbackE8=true;c.sphere=s.sphere;c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  for(unsigned k=0;k<16;k+=5)c.geometry.worldMatrix.v[k]=1.f;
  bindings[groundPrefix+i]={std::numeric_limits<std::size_t>::max(),&target,false,s.zone,0,g::ZoneMethod50CL::First,true};
 }
 return groundPrefix;
}
inline g::LiveZoneBounds50EN bounds(const source::Source& s){
 g::LiveZoneBounds50EN out{};out.bounds.shapeTag=5;
 for(unsigned k=0;k<16;k+=5)out.bounds.shapeWorld.v[k]=1.f;
 for(unsigned k=0;k<3;++k){out.bounds.localCenter.v[k]=s.sphere.center.v[k];
  out.bounds.localHalf.v[k]=s.sphere.radius;out.bounds.cachedCenter.v[k]=s.sphere.center.v[k];}
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;
 out.bounds.cachedRadius=s.sphere.radius;return out;
}
} // namespace gb_trigger_candidates50GB

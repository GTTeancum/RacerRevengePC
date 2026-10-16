#pragma once
#include "sa_trigger_authoring_fixture.h"
#include "sa_trigger_actions_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include <limits>
#include <vector>

// Authenticated TOB additions to the ordinary candidate/zone owner list.
// Exactly four shape5 spheres; no fake SCB rows or root-box entry proxies.
namespace sa_trigger_candidates {
namespace g=rr::game;
inline std::size_t prepare(const sa_trigger_authoring::Owner& source,
 sa_trigger_actions::Owner& actions,std::vector<g::Type2StaticCandidate49X>& candidates,
 std::vector<track_runtime_storage50IG::Binding>& bindings,std::uint8_t tag,
 std::uint8_t flags31,std::uint8_t flagsF8){
 if(!source.ready()||g::nativeTriggerCapacity50GA<sa_trigger_authoring::count||
    candidates.size()!=bindings.size()||
    candidates.size()>track_runtime_storage50IG::hardTerrainOwners-sa_trigger_authoring::count)
  throw std::runtime_error("SA native trigger cold owner extent");
 for(const auto& c:candidates)if(c.nativeTriggers==&actions.nativeGroup())
  throw std::runtime_error("SA native trigger candidates already bound");
 const auto first=candidates.size();
 // Bind the actual immovable action owner before racer bank/grid setup.
 // Parent prepares that owner with this exact source before any query; the
 // shared backend rejects every query while the group remains unprepared.
 // Prepare both replacement vectors before publishing pointer rebinding.
 // Call only cold, before any runtime owner retains these element addresses.
 auto nextCandidates=candidates;auto nextBindings=bindings;
 nextCandidates.resize(first+sa_trigger_authoring::count);nextBindings.resize(nextCandidates.size());
 for(std::size_t i=0;i<first;++i)nextBindings[i].candidate=&nextCandidates[i];
 for(std::size_t i=0;i<sa_trigger_authoring::count;++i){const auto& s=source.source(i);auto& target=nextCandidates[first+i];
  target.shapeType=5;target.nativeTriggers=&actions.nativeGroup();target.nativeTriggerIndex=i;
  auto& c=target.candidate.collision;c.shapeType=5;
  c.geometry.id=sa_trigger_actions::firstTriggerIdentity+static_cast<std::uint32_t>(i);
  c.filter.identity=c.geometry.id;c.ownerIdentity9C=0x33000000u+static_cast<std::uint32_t>(i);c.typeTag=tag;
  c.filter.categoryA4=8u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  // Original11B190/11B570(7) collider construction; child query scheduling
  // stays in trigger.flags29. PLAYER suppression lives in the shared owner.
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|6u);
  c.hasResponseCallbackE8=true;c.sphere=s.sphere;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  for(unsigned n=0;n<16;n+=5)c.geometry.worldMatrix.v[n]=1.f;
  nextBindings[first+i]={std::numeric_limits<std::size_t>::max(),&target,false,s.zone,0,g::ZoneMethod50CL::First,true};
 }
 candidates.swap(nextCandidates);bindings.swap(nextBindings);return first;
}
inline g::LiveZoneBounds50EN bounds(const sa_trigger_authoring::Source& source){
 g::LiveZoneBounds50EN out{};out.bounds.shapeTag=5;
 for(unsigned n=0;n<16;n+=5)out.bounds.shapeWorld.v[n]=1.f;
 for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=out.bounds.cachedCenter.v[n]=source.sphere.center.v[n];out.bounds.localHalf.v[n]=source.sphere.radius;}
 out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=source.sphere.radius;return out;
}
} // namespace sa_trigger_candidates

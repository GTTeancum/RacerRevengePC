#pragma once
#include "track_runtime_storage50ig_fixture.h"
#include <limits>
#include <vector>

namespace track_static_candidates50IG {
using namespace rr::game;
struct Bindings {
    // Native namespace, never a guest address. The caller reserves this whole
    // interval, just as it reserves separate native query identities.
    std::uint32_t firstIdentity=0;
    std::uint8_t collisionTypeTag=0,groupTypeTag=0;
    std::uint8_t preservedFlags31=0,preservedFlagsF8=0;
};
struct Radii {std::vector<float> values;std::size_t maxDepth=0;};
// Original track Standard creator1B5590..1B58D4, through actual COL placement.
// Source11B190(a1=1),141300 and11B570(13) establish the fields below. Runtime
// type/category mappings and the two preserved flag lanes remain caller inputs.
// COL type0 loads with a1=null at1B572C; owner9C stays constructor-null. Thus
// there is NO material scene/name. MaterialID+A9 is handled by50HI separately.
// Authored enumeration is never sorted, collapsed, shortened or filled from TA.
inline void prepare(const authored_runtime_ground50cp_fixture::ColdScene& ground,
    const Bindings& names,std::vector<Type2StaticCandidate49X>& candidates,
    std::vector<Radii>& radii,std::vector<track_runtime_storage50IG::Binding>& bindings) {
    const auto count=ground.owners.size();
    if(!candidates.empty()||!radii.empty()||!bindings.empty()||!count||
       count>authored_runtime_ground50cp_fixture::hardCollisionOwners50IG||
       !names.firstIdentity||count-1>std::numeric_limits<std::uint32_t>::max()-names.firstIdentity||
       names.collisionTypeTag==names.groupTypeTag||
       (names.preservedFlags31&0x0fu)||(names.preservedFlagsF8&0x3fu))
        throw std::runtime_error("invalid original static candidate namespace or cold owner extent");
    candidates.resize(count);radii.resize(count);bindings.resize(count);
    for(std::size_t i=0;i<count;++i){
        const auto& original=ground.owners[i];auto& out=candidates[i];auto& radius=radii[i];
        radius.values.resize(original.mesh.nodes.size());
        std::vector<std::uint32_t> topology(original.mesh.nodes.size()+original.mesh.faces.size());
        if(prepareStaticTargetRadii49X(original.mesh,radius.values.data(),radius.values.size(),
           {topology.data(),topology.size()},radius.maxDepth)!=Type2ResourceStatus49X::Complete)
            throw std::runtime_error("original static candidate radius/topology preparation failed");
        out.nodeRadii=radius.values.data();out.nodeRadiusCount=radius.values.size();out.maxDepth=radius.maxDepth;
        out.candidate.hasMaterialScene=false;out.candidate.sceneName=nullptr;out.candidate.sceneNameBytes=0;
        auto& c=out.candidate.collision;
        const auto identity=names.firstIdentity+static_cast<std::uint32_t>(i);
        c.geometry={&original.mesh,original.instance.world,identity};
        c.filter.identity=identity;c.filter.categoryA4=original.instance.category;
        c.filter.acceptMaskB8=0xffffffffu;
        c.filter.flags31=static_cast<std::uint8_t>(names.preservedFlags31|3u);
        c.filter.flagsF8=static_cast<std::uint8_t>(names.preservedFlagsF8|0x0cu);
        c.filter.categoryModeD4=1;
        c.filter.identityLists[0].mode=1;c.filter.identityLists[1].mode=1;
        c.sphere={{{original.instance.sphere.center.v[0],original.instance.sphere.center.v[1],
            original.instance.sphere.center.v[2]}},original.instance.sphere.radius};
        c.ownerIdentity9C=0;c.typeTag=names.collisionTypeTag;c.hasResponseCallbackE8=false;
        c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
        bindings[i]={i,&out};
    }
}
} // namespace track_static_candidates50IG

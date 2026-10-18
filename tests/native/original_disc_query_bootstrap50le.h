#pragma once
#include "native_racer_collision50hw_fixture.h"
#include "native_query_identities50kx.h"
#include "../../src/game/original_native_collision_categories.h"
#include "../../src/game/static_type2_producer49x.h"

// The ordinary RA path has no collision corpus. Its COL resources are decoded
// by the same native Owner as the actual roster; this binds only native query
// metadata before that roster installs the selected sources and exclusions.
namespace original_disc_query_bootstrap50LE {
using namespace rr::game;
inline native_racer_collision50HW::EndpointPolicy endpointPolicy() noexcept {
    // COL authors XYZ only. Retain the existing explicit HOST opaque-W policy;
    // these words are neither original-file data nor recovered heap contents.
    return {nullptr,[](void*,native_racer_collision50HW::QueryBody,std::size_t index,
        Type2OpaqueEndpointWords49X& out) noexcept {
        if(index>=32768)return false;
        out={0x80000000u+static_cast<std::uint32_t>(index),
             0x7fc10000u+static_cast<std::uint32_t>(index)};
        return true;
    }};
}
inline void initialize(const Type2ResourceView49X& resource,CollisionFilter48J& filter,
    Type2StaticQuery49X& query,bool secondary) {
    using Categories=OriginalNativeCollisionCategories;
    filter={};query={};
    // Native opaque identities, scene IDs and tags belong to this host
    // namespace. No original guest address or source registry ordering is used.
    filter.identity=secondary?native_query_identities50KX::secondary364:
        native_query_identities50KX::main690;
    query.queryScene.value=secondary?78u:77u;
    query.collisionTypeTag=7u;query.groupTypeTag=9u;
    // Source11B190 ->141300(a1=1),212178/212180 and212764/21276C.
    // Zeroed native allocation supplies the source-preserved upper flag bits.
    filter.flags31=3u;filter.flagsF8=6u;
    filter.categoryA4=Categories::physicsBody;filter.acceptMaskB8=0xffffffffu;
    filter.identityLists[0].mode=filter.identityLists[1].mode=1;
    filter.categoryModeD4=1;
    //212790..2127A8 appends the actual POD category to secondary's exclusion
    // list. The real per-racer reciprocal owner exclusions are installed later.
    if(secondary)filter.categoryMasks={Categories::pod};
    query.resource=&resource;query.filter=&filter;query.callbackEnabled=true;
    query.copied40=query.copied50={{0.f,0.f,0.f,1.f}}; //11B450/11B45C.
    query.skipMask=Categories::trigger;query.dynamicMask=Categories::physicsBody;
    query.materialMask=Categories::staticObject;
    //21219C/212788 pass zero to11BBC0, whose11BBEC stores shape+54.
    query.normalMode54=0u;query.normalMode54Bound=true;
}
}

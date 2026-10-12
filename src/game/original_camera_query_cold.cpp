#include "original_camera_query_cold.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game { namespace {
using V=SpringVector4_48N;
using S=OriginalCameraQueryColdStatus;
const V sourceVf0{{0.f,0.f,0.f,1.f}};
bool same(const V& a,const V& b) noexcept {return std::memcmp(&a,&b,sizeof a)==0;}
void base(OriginalCameraWorldQueryState& q,CollectionObject50BX& object,
    CollectionObjectState50BX& collection,OriginalCameraWorldObjectCopies& copies,
    const V& sentinel,std::uint8_t directTag,std::uint32_t shapeTag) noexcept {
    //11B190/141300 ->11B570(2) ->179A80 explicit bit0. Upper bits survive.
    q.flagsF8=static_cast<std::uint8_t>((q.flagsF8&0xc0u)|3u);
    collection.flagsF8=q.flagsF8;collection.epoch=0;
    collection.cachedCenter=sourceVf0; //11B250; radius80 is NOT initialized
    object.typeTag=directTag;object.hasShape=true;object.shapeTag=shapeTag;
    object.filter.flags31=static_cast<std::uint8_t>((object.filter.flags31&0xf0u)|3u);
    object.filter.categoryA4=0;object.filter.acceptMaskB8=0xffffffffu;
    for(auto& ids:object.filter.identities){ids.values=nullptr;ids.capacity=0;ids.count=0;ids.mode=1;}
    object.filter.categories.values=nullptr;object.filter.categories.capacity=0;
    object.filter.categories.count=0;object.filter.categories.mode=1;
    q.currentCandidateIdentity=0;q.copied40=sentinel;q.copied50=sentinel;
    q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.distance84=0.f;
    copies.copied40=sentinel;copies.copied50=sentinel;
}
}
OriginalCameraQueryColdProjection projectOriginalCameraQueryCold(
    const OriginalCameraQueryColdInput& in,const OriginalCameraQueryPairState& priorPair,
    const std::array<CollectionObject50BX,2>& priorObjects,
    const std::array<CollectionObjectState50BX,2>& priorCollection) noexcept {
    OriginalCameraQueryColdProjection out{};
    if(!priorObjects[0].identity||!priorObjects[1].identity||
       priorObjects[0].identity==priorObjects[1].identity||in.directTypeTagGp8010==0xffu||
       priorPair.sphere24.flagsF8!=priorCollection[0].flagsF8||
       priorPair.ray190.flagsF8!=priorCollection[1].flagsF8||
       !same(in.sentinel3E02C0,sourceVf0)||!same(in.zero433DB0,sourceVf0)){
        out.sourcePC=0x11b190u;return out;
    }
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(in.radius178)||in.radius178<=0.f){
        out.status=S::Arithmetic;out.sourcePC=0x11c160u;return out;
    }
    volatile float radiusSquared=in.radius178*in.radius178;
    if(!std::isfinite(radiusSquared)){
        out.status=S::Arithmetic;out.sourcePC=0x11c160u;return out;
    }
    // memcpy retains every supplied but source-unwritten field/bit pattern.
    std::memcpy(&out.pair,&priorPair,sizeof priorPair);
    std::memcpy(&out.objects,&priorObjects,sizeof priorObjects);
    std::memcpy(&out.collection,&priorCollection,sizeof priorCollection);
    base(out.pair.sphere24,out.objects[0],out.collection[0],out.copies[0],
        in.sentinel3E02C0,in.directTypeTagGp8010,5);
    base(out.pair.ray190,out.objects[1],out.collection[1],out.copies[1],
        in.sentinel3E02C0,in.directTypeTagGp8010,3);
    auto& pair=out.pair;pair.sentinel3E02C0=in.sentinel3E02C0;
    pair.sphereCenter00=in.zero433DB0;pair.sphereRadius50=in.radius178;
    pair.sphereRadiusSquared54=radiusSquared;
    //11C114..168: result20 and each result30/60 vector are vf0, count04=0.
    // Native sphere contact keeps only the consumed XYZ; distance08 untouched.
    pair.sphere24.sphere.count=0;
    pair.sphere24.sphere.worldCenter={0.f,0.f,0.f};
    pair.sphere24.sphere.worldNormal={0.f,0.f,0.f};
    pair.sphere24.sphere.worldPoint={0.f,0.f,0.f};
    //11FF60 ->11FE50 ->11FC80 uses unparented identity and source zero center.
    // With initialized (+0,+0,+0,1), every multiply/add yields the same vf0.
    out.collection[0].cachedCenter=sourceVf0;
    out.collection[0].cachedRadius=in.radius178;
    //11C710(1,0,0): constructor initializes these vectors but no scalar ray
    // result arrays.179C18 then copies both COMPLETE position vectors.
    pair.rayNormalMode44=0;
    pair.endpoints00[0]=in.initialPosition;pair.endpoints00[1]=in.initialPosition;
    pair.endpointMetadata[0]=0;pair.endpointMetadata[1]=0;
    auto& t=pair.ray190.type3;
    t.localCenter={{0.f,0.f,0.f}};t.worldCenter=sourceVf0;t.queryCenter=sourceVf0;
    // Native queryRadius aliases query80, which this source prefix never wrote.
    std::memcpy(&t.queryRadius,&out.collection[1].cachedRadius,sizeof t.queryRadius);
    t.result00=sourceVf0;t.result10=sourceVf0;
    t.selectedNormal=sourceVf0;t.selectedPoint=sourceVf0;t.selectedCorrection=sourceVf0;
    auto& ray=pair.ray190.ray;
    ray.localStart={{0.f,0.f,0.f}};ray.localDirection={{0.f,0.f,0.f}};
    ray.worldStart=sourceVf0;ray.worldDirection=sourceVf0;
    ray.normal=sourceVf0;ray.point=sourceVf0;
    //11FFC0 and11FF60 both return without changing shape3 cache/geometry.
    out.status=S::Complete;out.sourcePC=0;return out;
}
}

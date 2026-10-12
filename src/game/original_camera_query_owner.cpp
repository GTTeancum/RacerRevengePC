#include "original_camera_query_owner.h"
#include "query_prepare48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using S=OriginalCameraPairStatus;
using V=SpringVector4_48N;
struct Span {std::uintptr_t first=0,last=0;};
template<class T> bool span(const T* p,std::size_t n,Span& s) noexcept {
    s={};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto first=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
    s={first,first+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writes[48]{};unsigned count=0;
    template<class T> bool write(T* p,std::size_t n) noexcept {
        Span s;if(count==48||!span(p,n,s))return false;
        for(unsigned k=0;k<count;++k)if(overlap(s,writes[k]))return false;
        writes[count++]=s;return true;
    }
    template<class T> bool read(const T* p,std::size_t n) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(unsigned k=0;k<count;++k)if(overlap(s,writes[k]))return false;
        return true;
    }
    bool mesh(const assets::CollisionMesh48I* p) const noexcept {
        return !p||(read(p,1)&&read(p->nodes.data(),p->nodes.size())&&
            read(p->faces.data(),p->faces.size())&&read(p->vertices.data(),p->vertices.size())&&
            read(p->vertex_normals.data(),p->vertex_normals.size()));
    }
};
bool addWorld(Guard& g,const OriginalCameraWorldMutable& w) noexcept {
    return g.write(w.objects,w.objectCapacity)&&g.write(w.methods,w.methodCapacity)&&g.write(w.copies,w.copyCapacity);
}
bool storage(const OriginalCameraQueryPairOwner& o,const V& a,const V& b,const V& c,
    OriginalCameraCollisionCallback* callback,OriginalCameraPairResult& result,
    const OriginalCameraOwnerSpan* extraWrite=nullptr,std::size_t extraWriteCount=0,
    const OriginalCameraOwnerSpan* extraRead=nullptr,std::size_t extraReadCount=0) noexcept {
    if(!o.lifecycle||!o.liveWorld||!o.liveWorkspace||!o.sphereE0||!o.rayE0||!o.state||!o.normalMode3DF5B0)return false;
    Guard g;auto& life=*o.lifecycle;auto& live=*o.liveWorld;auto& w=*o.liveWorkspace;
    if(!g.write(&life,1)||!g.write(&w,1)||!g.write(o.sphereE0,1)||!g.write(o.rayE0,1)||!g.write(o.state,1)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(o.sphereE0->storage.links,o.sphereE0->storage.capacity)||
       !g.write(o.sphereE0->storage.entries,o.sphereE0->storage.capacity)||
       !g.write(o.rayE0->storage.links,o.rayE0->storage.capacity)||
       !g.write(o.rayE0->storage.entries,o.rayE0->storage.capacity)||
       !g.write(w.registries,w.registryCapacity)||!g.write(w.links,w.membershipCapacity)||
       !g.write(w.entries,w.membershipCapacity)||!g.write(w.portalLinks,w.portalLinkCapacity)||
       !g.write(live.portalLinks,live.portalLinkCount)||!addWorld(g,o.mutableWorld)||
       !addWorld(g,o.queryWorkspace.staging)||!g.write(o.queryWorkspace.meshStack,o.queryWorkspace.meshStackCapacity)||
       !g.write(o.normalMode3DF5B0,1)||!g.write(callback,callback?1u:0u)||!g.write(&result,1))return false;
    if(extraWriteCount>16||extraReadCount>32||(extraWriteCount&&!extraWrite)||(extraReadCount&&!extraRead))return false;
    for(std::size_t k=0;k<extraWriteCount;++k)if(!g.write(static_cast<const unsigned char*>(extraWrite[k].data),extraWrite[k].bytes))return false;
    for(std::size_t k=0;k<extraReadCount;++k)if(!g.read(static_cast<const unsigned char*>(extraRead[k].data),extraRead[k].bytes))return false;
    const auto& in=o.world;
    if(!g.read(&o,1)||!g.read(&a,1)||!g.read(&b,1)||!g.read(&c,1)||!g.read(&live,1)||
       !g.read(live.portalHeads,live.zoneCount)||!g.read(live.portals,live.portalCount)||
       !g.read(in.objects,in.objectCount)||!g.read(in.geometry,in.objectCount)||
       !g.read(in.globalFallbackZone443108,in.globalFallbackZone443108?1u:0u)||
       !g.read(in.directTypeTagGp8010,in.directTypeTagGp8010?1u:0u)||
       !g.read(in.staticObjectMask3E0720,in.staticObjectMask3E0720?1u:0u))return false;
    // Published method/zone/block/word spans intentionally belong to life;
    // identity is checked below, not incorrectly admitted as independent reads.
    for(std::size_t k=0;k<in.objectCount;++k){
        const auto& f=in.objects[k].filter;
        for(const auto& ids:f.identities)if(!g.read(ids.values,ids.capacity))return false;
        if(!g.read(f.categories.values,f.categories.capacity))return false;
        const auto& geometry=in.geometry[k];if(!g.mesh(geometry.mesh))return false;
        if(geometry.compound){const auto& p=*geometry.compound;
            if(!g.read(&p,1)||p.count>maxCompoundMembers50IJ||!g.read(p.parts,p.count))return false;
            for(std::size_t j=0;j<p.count;++j)if(!g.mesh(p.parts[j].mesh)||
                !g.read(p.parts[j].nodeRadii,p.parts[j].nodeRadiusCount))return false;
        }
    }
    return true;
}
bool binding(const OriginalCameraQueryPairOwner& o,bool attached) noexcept {
    const auto& life=*o.lifecycle;const auto& in=o.world;
    if(!life.perBlock||!life.zoneCount||life.zoneCount!=in.zoneCount||life.zoneCount!=o.liveWorld->zoneCount||
       life.objectCount!=in.objectCount||in.methodCount!=2u*life.zoneCount||
       in.methods!=life.storage.views.methods||in.zones!=life.storage.views.zones||
       !in.globalFallbackZone443108||!in.directTypeTagGp8010||!in.staticObjectMask3E0720||
       *in.globalFallbackZone443108>in.zoneCount||
       !o.sphereObject||!o.rayObject||o.sphereObject==o.rayObject||
       o.sphereObject>in.objectCount||o.rayObject>in.objectCount||
       o.mutableWorld.objectCapacity<in.objectCount||o.mutableWorld.copyCapacity<in.objectCount||
       o.mutableWorld.methodCapacity<in.methodCount||
       in.objects[o.sphereObject-1].shapeTag!=5||in.objects[o.rayObject-1].shapeTag!=3)return false;
    const auto check=[&](const LiveZoneMembership50EN& m,CollectionRef50BX ref,
        const OriginalCameraWorldQueryState& q) noexcept {
        if(m.attached!=attached||(attached&&(m.object!=ref||m.method!=ZoneMethod50CL::Second)))return false;
        const auto& copies=o.mutableWorld.copies[ref-1];
        return o.mutableWorld.objects[ref-1].flagsF8==q.flagsF8&&(q.flagsF8&3u)==3u&&
            !(q.flagsF8&8u)&&!std::memcmp(&copies.copied40,&q.copied40,sizeof q.copied40)&&
            !std::memcmp(&copies.copied50,&q.copied50,sizeof q.copied50);
    };
    return check(*o.sphereE0,o.sphereObject,o.state->sphere24)&&check(*o.rayE0,o.rayObject,o.state->ray190);
}
struct Math {
    bool ok=true;
    float keep(float f) noexcept {ok=ok&&std::isfinite(f);return f;}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float f=a*b;return keep(f);}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float f=a+b;return keep(f);}
};
CollectionBoundsInput48S bounds(const OriginalCameraQueryPairOwner& o,bool sphere) noexcept {
    CollectionBoundsInput48S b{};const auto& state=*o.state;
    b.shapeTag=sphere?5u:3u;b.shapeWorld.v[0]=b.shapeWorld.v[5]=b.shapeWorld.v[10]=b.shapeWorld.v[15]=1.f;
    const auto ref=sphere?o.sphereObject:o.rayObject;
    const auto& object=o.mutableWorld.objects[ref-1];b.cachedCenter=object.cachedCenter;b.cachedRadius=object.cachedRadius;
    if(sphere){b.localCenter=state.sphereCenter00;b.localHalf={{state.sphereRadius50,state.sphereRadius50,state.sphereRadius50,1.f}};}
    else {b.pairCount=1;b.endpoints=state.endpoints00;b.endpointCount=2;b.endpointScenes=state.endpointMetadata;b.endpointSceneCount=2;}
    return b;
}
bool setSphere(OriginalCameraQueryPairOwner& o,const V& center,bool refresh) noexcept {
    auto& s=*o.state;s.sphereCenter00=center;
    if(!refresh)return true;
    //11FE50 unparented shape5 ->11F680: identity is still multiplied. Keep
    // the separately rounded scalar form and source XYZ add order at signed0.
    Math m;auto& cached=o.mutableWorld.objects[o.sphereObject-1];V prepared{{0.f,0.f,0.f,1.f}};
    for(unsigned k=0;k<3;++k){
        const float x=m.mul(center.v[0],k==0?1.f:0.f),y=m.mul(center.v[1],k==1?1.f:0.f),z=m.mul(center.v[2],k==2?1.f:0.f);
        prepared.v[k]=m.add(m.add(m.add(x,y),z),0.f);
    }
    m.keep(s.sphereRadius50);if(!m.ok||s.sphereRadius50<=0.f)return false;
    cached.cachedCenter=prepared;cached.cachedRadius=s.sphereRadius50;return true;
}
bool setRay(OriginalCameraQueryPairOwner& o,const V& a,const V& b) noexcept {
    const V source[2]={a,b};QueryPairStagingInput48O in{};in.pairCount=1;in.source=source;in.sourceCapacity=2;
    std::uint32_t tag=3;
    return stageQueryPairs48O(in,o.state->endpoints00,2,o.state->endpointMetadata,2,tag)==QueryPairStagingStatus48O::Staged&&tag==3;
}
S fail(OriginalCameraPairResult& out,S s,std::uint32_t pc) noexcept {out.status=s;out.sourcePC=pc;return s;}
bool prepareRegistration(OriginalCameraQueryPairOwner& o,bool sphere,bool attach,CollectionRef50BX zone,
    OriginalCameraPairResult& out) noexcept {
    auto& m=sphere?*o.sphereE0:*o.rayE0;const auto ref=sphere?o.sphereObject:o.rayObject;
    const auto before=m.revision;LiveZoneBounds50EN b{};b.bounds=bounds(o,sphere);
    if(attach){const auto storage=m.storage;
        out.registration=attachLiveZoneMembership50EN(*o.lifecycle,*o.liveWorld,m,storage,ref,zone,ZoneMethod50CL::Second,0.f,b,*o.liveWorkspace);
    }else out.registration=updateLiveZoneMembership50EN(*o.lifecycle,*o.liveWorld,m,b,*o.liveWorkspace);
    if(out.registration.status!=LiveZoneStatus50EN::Complete){fail(out,S::LiveRegistration,attach?0x120070u:0x120230u);return false;}
    ++out.updatesCommitted;
    if(!m.attached||m.object!=ref||m.method!=ZoneMethod50CL::Second||m.lastShapeTag!=b.bounds.shapeTag||
       (!attach&&(before==std::numeric_limits<std::uint64_t>::max()||m.revision!=before+1u))||
       std::memcmp(&m.lastShapeWorld,&b.bounds.shapeWorld,sizeof m.lastShapeWorld)){
        fail(out,S::Binding,0x120230u);return false;
    }
    auto& object=o.mutableWorld.objects[ref-1];object.cachedCenter=m.bounds.cachedCenter;object.cachedRadius=m.bounds.cachedRadius;
    return true;
}
bool query(OriginalCameraQueryPairOwner& o,bool sphere,OriginalCameraCollisionCallback& camera,OriginalCameraPairResult& out) noexcept {
    auto in=o.world;auto& m=sphere?*o.sphereE0:*o.rayE0;auto& s=*o.state;
    in.queryObject=sphere?o.sphereObject:o.rayObject;in.registrationChainPresent=m.attached;
    in.zoneLinks=m.storage.links;in.zoneLinkCount=m.storage.capacity;in.zoneHead=1;
    in.shapeTag=sphere?5u:3u;in.bounds=bounds(o,sphere);
    in.sphereRadius50=s.sphereRadius50;in.sphereRadiusSquared54=s.sphereRadiusSquared54;
    in.sentinel3E02C0=s.sentinel3E02C0;in.type3NormalMode44=s.rayNormalMode44;
    auto& q=sphere?s.sphere24:s.ray190;
    const auto status=queryOriginalCameraWorld(in,o.mutableWorld,o.queryWorkspace,q,camera,out.world);
    if(status!=OriginalCameraWorldStatus::Complete){fail(out,S::WorldQuery,out.world.sourcePC);return false;}
    // Both native query-state flags alias their actual object.F8 fields. The
    // iterator can mark the OTHER camera query visited even though its target
    // bit is clear. In particular, a ray hit skips the following sphere query;
    // preserve that accepted candidate write before the next pair admission.
    s.sphere24.flagsF8=o.mutableWorld.objects[o.sphereObject-1].flagsF8;
    s.ray190.flagsF8=o.mutableWorld.objects[o.rayObject-1].flagsF8;
    const CollisionNormalModeWrite50JA normalWrite{out.world.normalMode3DF5B0,out.world.normalModeWritten,true};
    if(!o.normalMode3DF5B0->acceptCompleted(normalWrite)){fail(out,S::Binding,0x121438u);return false;}
    ++out.queriesCommitted;return true;
}
} // namespace
bool admitOriginalCameraQueryPairStorage(const OriginalCameraQueryPairOwner& o,
    const OriginalCameraOwnerSpan* writable,std::size_t writableCount,
    const OriginalCameraOwnerSpan* readable,std::size_t readableCount) noexcept {
    const V temporary{};OriginalCameraPairResult diagnostic{};
    return storage(o,temporary,temporary,temporary,nullptr,diagnostic,writable,writableCount,readable,readableCount);
}
S updateOriginalCameraQueryPair(OriginalCameraQueryPairOwner& o,const V& target,const V& endpoint,const V& sphereCenter,
    bool executeQueries,OriginalCameraCollisionCallback& camera,OriginalCameraPairResult& out) noexcept {
    if(!storage(o,target,endpoint,sphereCenter,&camera,out))return S::Storage;
    out={};if(!binding(o,true))return fail(out,S::Binding,0x179a80u);
    if(std::fegetround()!=FE_TONEAREST)return fail(out,S::Arithmetic,0x17a870u);
    if(executeQueries)camera.hit114=0;
    if(!setSphere(o,sphereCenter,true))return fail(out,S::Arithmetic,0x11f680u);
    if(!prepareRegistration(o,true,false,0,out))return out.status;
    if(!setRay(o,target,endpoint))return fail(out,S::Binding,0x11ca80u);
    if(!prepareRegistration(o,false,false,0,out))return out.status;
    if(executeQueries){
        if(!query(o,false,camera,out))return out.status;
        if(!camera.hit114&&!query(o,true,camera,out))return out.status;
    }
    return S::Complete;
}
S attachOriginalCameraQueryPair(OriginalCameraQueryPairOwner& o,CollectionRef50BX zone,const V& position,
    OriginalCameraPairResult& out) noexcept {
    if(!storage(o,position,position,position,nullptr,out))return S::Storage;
    out={};if(!binding(o,false)||!zone||zone>o.lifecycle->zoneCount)return fail(out,S::Binding,0x179a80u);
    if(std::fegetround()!=FE_TONEAREST)return fail(out,S::Arithmetic,0x179a80u);
    if(!setSphere(o,position,false))return fail(out,S::Arithmetic,0x11c1f0u);
    if(!prepareRegistration(o,true,true,zone,out))return out.status;
    if(!setRay(o,position,position))return fail(out,S::Binding,0x11ca80u);
    if(!prepareRegistration(o,false,true,zone,out))return out.status;
    return S::Complete;
}
}

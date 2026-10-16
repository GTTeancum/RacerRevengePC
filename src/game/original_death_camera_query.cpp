#include "original_death_camera_query.h"
#include "query_prepare48o.h"
#include <cfenv>
#include <cstring>
#include <limits>
#include <new>

namespace rr::game { namespace {
using S=OriginalDeathCameraQueryStatus;
using V=SpringVector4_48N;
const V sourceVf0{{0.f,0.f,0.f,1.f}};
bool same(const V& a,const V& b) noexcept {return std::memcmp(&a,&b,sizeof a)==0;}
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
bool storage(const OriginalDeathCameraQueryOwner& o,const V& a,const V& b,
    OriginalDeathCameraQueryResult& result,const OriginalCameraOwnerSpan* extraWrite=nullptr,
    std::size_t extraWriteCount=0,const OriginalCameraOwnerSpan* extraRead=nullptr,std::size_t extraReadCount=0,bool absentState=false,
    const OriginalDeathCameraQueryColdOwner* coldWrite=nullptr) noexcept {
    if(!o.lifecycle||!o.liveWorld||!o.liveWorkspace||!o.membershipE0||(!o.state&&!absentState)||!o.normalMode3DF5B0)return false;
    Guard g;
    if(coldWrite&&(!g.write(coldWrite,1)||!g.write(coldWrite->globalValid3E0730,1)))return false;
    auto& life=*o.lifecycle;auto& live=*o.liveWorld;auto& w=*o.liveWorkspace;auto& membership=*o.membershipE0;
    if(!g.write(&life,1)||!g.write(&w,1)||!g.write(&membership,1)||!g.write(o.state,o.state?1u:0u)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(membership.storage.links,membership.storage.capacity)||
       !g.write(membership.storage.entries,membership.storage.capacity)||
       !g.write(w.registries,w.registryCapacity)||!g.write(w.links,w.membershipCapacity)||
       !g.write(w.entries,w.membershipCapacity)||!g.write(w.portalLinks,w.portalLinkCapacity)||
       !g.write(live.portalLinks,live.portalLinkCount)||!addWorld(g,o.mutableWorld)||
       !addWorld(g,o.queryWorkspace.staging)||!g.write(o.queryWorkspace.meshStack,o.queryWorkspace.meshStackCapacity)||
       !g.write(o.normalMode3DF5B0,1)||!g.write(&result,1))return false;
    if(extraWriteCount>16||extraReadCount>32||(extraWriteCount&&!extraWrite)||(extraReadCount&&!extraRead))return false;
    for(std::size_t k=0;k<extraWriteCount;++k)if(!g.write(static_cast<const unsigned char*>(extraWrite[k].data),extraWrite[k].bytes))return false;
    for(std::size_t k=0;k<extraReadCount;++k)if(!g.read(static_cast<const unsigned char*>(extraRead[k].data),extraRead[k].bytes))return false;
    const auto& in=o.world;
    if(!g.read(&o,1)||!g.read(&a,1)||!g.read(&b,1)||!g.read(&live,1)||
       !g.read(live.portalHeads,live.zoneCount)||!g.read(live.portals,live.portalCount)||
       !g.read(in.objects,in.objectCount)||!g.read(in.geometry,in.objectCount)||
       !g.read(in.globalFallbackZone443108,in.globalFallbackZone443108?1u:0u)||
       !g.read(in.directTypeTagGp8010,in.directTypeTagGp8010?1u:0u)||
       !g.read(in.staticObjectMask3E0720,in.staticObjectMask3E0720?1u:0u))return false;
    // Methods/zones/blocks/words are the lifecycle's real published views.
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
bool binding(const OriginalDeathCameraQueryOwner& o,bool attached) noexcept {
    if(!o.lifecycle||!o.liveWorld||!o.liveWorkspace||!o.membershipE0||!o.state||!o.normalMode3DF5B0)return false;
    const auto& life=*o.lifecycle;const auto& in=o.world;const auto& s=*o.state;const auto& m=*o.membershipE0;
    if(!o.owner9C||s.owner9C!=o.owner9C||s.callbackE4||s.rayNormalMode44||
       !life.perBlock||life.perBlock>1024u||!life.zoneCount||life.zoneCount!=in.zoneCount||life.zoneCount!=o.liveWorld->zoneCount||
       life.objectCount!=in.objectCount||life.zoneCount>std::numeric_limits<std::size_t>::max()/2u||
       in.methodCount!=2u*life.zoneCount||in.methods!=life.storage.views.methods||in.zones!=life.storage.views.zones||
       life.storage.registryCapacity<in.methodCount||life.storage.registrationCapacity<in.objectCount||
       life.storage.views.methodCapacity<in.methodCount||life.storage.views.zoneCapacity<in.zoneCount||
       !life.storage.registries||!life.storage.registrations||
       !in.objects||!in.geometry||!in.globalFallbackZone443108||!in.directTypeTagGp8010||!in.staticObjectMask3E0720||
       *in.globalFallbackZone443108>in.zoneCount||!o.object120||o.object120>in.objectCount||
       !o.mutableWorld.objects||!o.mutableWorld.copies||!o.mutableWorld.methods||
       o.mutableWorld.objectCapacity<in.objectCount||o.mutableWorld.copyCapacity<in.objectCount||
       o.mutableWorld.methodCapacity<in.methodCount||m.attached!=attached||
       (attached&&(m.object!=o.object120||m.method!=ZoneMethod50CL::Second||!m.storage.capacity||
           !m.storage.links||!m.storage.entries||!m.storage.links[0].zone||m.storage.links[0].zone>in.zoneCount)))return false;
    if(attached){
        if(m.lastShapeTag!=3||!m.revision||o.object120>(std::numeric_limits<CollectionRef50BX>::max()>>2u))return false;
        BodyMatrix48G identity{};identity.v[0]=identity.v[5]=identity.v[10]=identity.v[15]=1.f;
        if(std::memcmp(&m.lastShapeWorld,&identity,sizeof identity))return false;
        const auto& primary=m.storage.entries[0].registration;
        const auto& registered=life.storage.registrations[o.object120-1];
        if(primary.owner!=registered.owner||primary.slot!=registered.slot||primary.encodedObject!=registered.encodedObject)return false;
        std::size_t count=0;
        for(CollectionRef50BX ref=1;ref;ref=m.storage.links[ref-1].next){
            if(ref>m.storage.capacity||++count>m.storage.capacity)return false;
            const auto& link=m.storage.links[ref-1];const auto& entry=m.storage.entries[ref-1];
            if(!entry.used||!link.zone||link.zone>in.zoneCount||
               entry.registration.owner!=&life.storage.registries[2u*(link.zone-1u)+1u]||
               entry.registration.encodedObject!=(o.object120<<2u)||!entry.registration.slot||
               entry.registration.slot>1024u||entry.registration.owner->words[entry.registration.slot-1u]!=(o.object120<<2u))return false;
        }
        std::size_t used=0;for(std::size_t k=0;k<m.storage.capacity;++k)if(m.storage.entries[k].used)++used;
        if(used!=count)return false;
    }
    const auto& object=in.objects[o.object120-1];const auto& f=object.filter;
    if(!object.identity||object.identity!=s.objectIdentity120||!object.hasShape||object.shapeTag!=3||object.typeTag!=*in.directTypeTagGp8010||
       f.categoryA4||f.acceptMaskB8!=0xffffffffu||(f.flags31&15u)!=3u||
       f.categories.mode!=0||f.categories.count!=1||f.categories.capacity<1||!f.categories.values||
       f.categories.values[0]!=s.staticCategoryMaskD8)return false;
    for(const auto& ids:f.identities)if(ids.count||ids.mode!=1)return false;
    const auto& q=s.query120;const auto& copies=o.mutableWorld.copies[o.object120-1];
    return o.mutableWorld.objects[o.object120-1].flagsF8==q.flagsF8&&(q.flagsF8&15u)==3u&&
        same(copies.copied40,q.copied40)&&same(copies.copied50,q.copied50)&&same(s.sentinel3E02C0,sourceVf0);
}
S fail(OriginalDeathCameraQueryResult& out,S s,std::uint32_t pc) noexcept {out.status=s;out.sourcePC=pc;return s;}
CollectionBoundsInput48S bounds(const OriginalDeathCameraQueryOwner& o) noexcept {
    CollectionBoundsInput48S b{};const auto& s=*o.state;const auto& object=o.mutableWorld.objects[o.object120-1];
    b.shapeTag=3;b.shapeWorld.v[0]=b.shapeWorld.v[5]=b.shapeWorld.v[10]=b.shapeWorld.v[15]=1.f;
    b.cachedCenter=object.cachedCenter;b.cachedRadius=object.cachedRadius;
    b.pairCount=1;b.endpoints=s.endpoints00;b.endpointCount=2;
    b.endpointScenes=s.endpointMetadata;b.endpointSceneCount=2;return b;
}
bool setRay(OriginalDeathCameraQueryOwner& o,const V& a,const V& b) noexcept {
    const V source[2]={a,b};QueryPairStagingInput48O in{};in.pairCount=1;in.source=source;in.sourceCapacity=2;
    std::uint32_t tag=3;
    return stageQueryPairs48O(in,o.state->endpoints00,2,o.state->endpointMetadata,2,tag)==QueryPairStagingStatus48O::Staged&&tag==3;
}
bool registration(OriginalDeathCameraQueryOwner& o,bool attach,CollectionRef50BX zone,OriginalDeathCameraQueryResult& out) noexcept {
    auto& m=*o.membershipE0;const auto before=m.revision;LiveZoneBounds50EN b{};b.bounds=bounds(o);
    if(attach){const auto storage=m.storage;
        out.registration=attachLiveZoneMembership50EN(*o.lifecycle,*o.liveWorld,m,storage,o.object120,zone,
            ZoneMethod50CL::Second,0.f,b,*o.liveWorkspace);
    }else out.registration=updateLiveZoneMembership50EN(*o.lifecycle,*o.liveWorld,m,b,*o.liveWorkspace);
    if(out.registration.status!=LiveZoneStatus50EN::Complete){fail(out,S::LiveRegistration,attach?0x120070u:0x120230u);return false;}
    ++out.updatesCommitted;
    if(!m.attached||m.object!=o.object120||m.method!=ZoneMethod50CL::Second||m.lastShapeTag!=3||
       (!attach&&(before==std::numeric_limits<std::uint64_t>::max()||m.revision!=before+1u))||
       std::memcmp(&m.lastShapeWorld,&b.bounds.shapeWorld,sizeof m.lastShapeWorld)){
        fail(out,S::Binding,0x120230u);return false;
    }
    auto& object=o.mutableWorld.objects[o.object120-1];object.cachedCenter=m.bounds.cachedCenter;object.cachedRadius=m.bounds.cachedRadius;
    return true;
}
bool remove(OriginalDeathCameraQueryOwner& o,OriginalDeathCameraQueryResult& out) noexcept {
    const bool attached=o.membershipE0->attached;
    out.registration=removeLiveZoneMembership50EN(*o.lifecycle,*o.liveWorld,*o.membershipE0,*o.liveWorkspace);
    if(out.registration.status!=LiveZoneStatus50EN::Complete){fail(out,S::LiveRegistration,0x120290u);return false;}
    if(attached)++out.removalsCommitted;
    if(o.membershipE0->attached){fail(out,S::Binding,0x120290u);return false;}
    return true;
}
} // namespace
OriginalDeathCameraQueryColdProjection projectOriginalDeathCameraQueryCold(
    const OriginalDeathCameraQueryColdInput& in,const OriginalDeathCameraQueryState& prior,
    const CollectionObject50BX& object,const CollectionObjectState50BX& collection) noexcept {
    OriginalDeathCameraQueryColdProjection out{};
    if(!in.owner9C||!object.identity||in.directTypeTagGp8010==0xffu||
       prior.query120.flagsF8!=collection.flagsF8||!same(in.sentinel3E02C0,sourceVf0)||
       !in.categoryD8||reinterpret_cast<std::uintptr_t>(in.categoryD8)%alignof(std::uint32_t)||
       *in.categoryD8!=in.staticObjectMask3E0720){out.sourcePC=0x22d8a8u;return out;}
    std::memcpy(&out.state,&prior,sizeof prior);std::memcpy(&out.object,&object,sizeof object);
    std::memcpy(&out.collection,&collection,sizeof collection);
    auto& s=out.state;auto& q=s.query120;auto& f=out.object.filter;
    q.flagsF8=static_cast<std::uint8_t>((q.flagsF8&0xc0u)|3u);
    out.collection.flagsF8=q.flagsF8;out.collection.epoch=0;out.collection.cachedCenter=sourceVf0;
    out.object.typeTag=in.directTypeTagGp8010;out.object.hasShape=true;out.object.shapeTag=3;
    f.flags31=static_cast<std::uint8_t>((f.flags31&0xf0u)|3u);
    f.categoryA4=0;f.acceptMaskB8=0xffffffffu;
    for(auto& ids:f.identities){ids.values=nullptr;ids.capacity=0;ids.count=0;ids.mode=1;}
    f.categories={in.categoryD8,1,1,0}; //124160(0)/124170(mask), NOT B8
    s.owner9C=in.owner9C;s.callbackE4=0;s.staticCategoryMaskD8=in.staticObjectMask3E0720;s.objectIdentity120=object.identity;
    s.sentinel3E02C0=in.sentinel3E02C0;s.rayNormalMode44=0;
    q.currentCandidateIdentity=0;q.copied40=in.sentinel3E02C0;q.copied50=in.sentinel3E02C0;
    q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.distance84=0.f;
    out.copies.copied40=in.sentinel3E02C0;out.copies.copied50=in.sentinel3E02C0;
    s.endpoints00[0]=in.initialPosition;s.endpoints00[1]=in.initialPosition;
    s.endpointMetadata[0]=0;s.endpointMetadata[1]=0;
    auto& t=q.type3;t.localCenter={{0.f,0.f,0.f}};t.worldCenter=sourceVf0;t.queryCenter=sourceVf0;
    std::memcpy(&t.queryRadius,&out.collection.cachedRadius,sizeof t.queryRadius);
    t.result00=sourceVf0;t.result10=sourceVf0;
    t.selectedNormal=sourceVf0;t.selectedPoint=sourceVf0;t.selectedCorrection=sourceVf0;
    auto& ray=q.ray;ray.localStart={{0.f,0.f,0.f}};ray.localDirection={{0.f,0.f,0.f}};
    ray.worldStart=sourceVf0;ray.worldDirection=sourceVf0;ray.normal=sourceVf0;ray.point=sourceVf0;
    //11C710 does not initialize shape40, result60/74/78/7C or scalar ray
    // arrays.11B190 does not initialize cachedRadius80. Their priors survive.
    out.status=S::Complete;return out;
}
bool admitOriginalDeathCameraQueryColdStorage(const OriginalDeathCameraQueryOwner& o,
    const OriginalDeathCameraQueryColdOwner& cold,const OriginalCameraOwnerSpan* writes,std::size_t writeCount,
    const OriginalCameraOwnerSpan* reads,std::size_t readCount) noexcept {
    if(o.state)return cold.complete&&o.state==cold.allocation120.get()&&
        admitOriginalDeathCameraQueryStorage(o,writes,writeCount,reads,readCount);
    if(cold.attempted||cold.complete||cold.allocation120||cold.categoryD8||!cold.identity120||
       !cold.globalValid3E0730||!o.object120||o.object120>o.world.objectCount||
       !o.world.objects||!o.world.geometry||
       cold.object120!=&o.world.objects[o.object120-1u]||
       cold.geometry120!=&o.world.geometry[o.object120-1u]||
       cold.object120->identity!=cold.identity120||cold.object120->hasShape||
       !o.owner9C||!o.world.directTypeTagGp8010||*o.world.directTypeTagGp8010==0xffu||
       !o.world.staticObjectMask3E0720||!o.mutableWorld.objects||!o.mutableWorld.copies||
       o.mutableWorld.objectCapacity<o.world.objectCount||o.mutableWorld.copyCapacity<o.world.objectCount||
       !o.lifecycle||o.lifecycle->objectCount!=o.world.objectCount||
       !o.membershipE0||o.membershipE0->attached||!o.membershipE0->storage.capacity||
       !o.membershipE0->storage.links||!o.membershipE0->storage.entries)return false;
    const V temporary{};OriginalDeathCameraQueryResult result{};
    return storage(o,temporary,temporary,result,writes,writeCount,reads,readCount,true,&cold);
}
S constructOriginalDeathCameraQuery22D8A8(OriginalDeathCameraQueryOwner& o,
    OriginalDeathCameraQueryColdOwner& cold) noexcept {
    if(!admitOriginalDeathCameraQueryColdStorage(o,cold,nullptr,0,nullptr,0))return S::Binding;
    if(o.state)return S::Complete; //22D888 retained query branch
    cold.attempted=true;cold.sourcePC=0x22d894u;
    //22D894 allocates the actual native query; failure keeps the preceding
    //scene removal and22D87C binding. It never publishes a fake readiness bit.
    cold.allocation120.reset(new(std::nothrow) OriginalDeathCameraQueryState{});
    if(!cold.allocation120)return S::Storage;
    auto& s=*cold.allocation120;auto& q=s.query120;
    auto& object=*cold.object120;auto& collection=o.mutableWorld.objects[o.object120-1u];
    auto& copies=o.mutableWorld.copies[o.object120-1u];
    //11B190 cold base constructor; source-unwritten bytes retain their
    //initialized native allocation priors. No type3 result is consumed yet.
    object.identity=cold.identity120;object.typeTag=*o.world.directTypeTagGp8010;
    object.hasShape=false;object.shapeTag=0;
    object.filter.flags31=3;object.filter.categoryA4=0;object.filter.acceptMaskB8=0xffffffffu;
    for(auto& ids:object.filter.identities)ids={nullptr,0,0,1};
    object.filter.categories={nullptr,0,0,1};
    q.flagsF8=6;collection.flagsF8=6;collection.epoch=0;collection.cachedCenter=sourceVf0;
    q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.distance84=0.f;
    q.copied40=q.copied50=sourceVf0;copies={sourceVf0,sourceVf0};
    s.sentinel3E02C0=sourceVf0;s.objectIdentity120=object.identity;
    cold.sourcePC=0x22d8b0u;
    o.state=&s; //22D8B0: publication happens before11B570 or D8 allocation
    q.flagsF8=static_cast<std::uint8_t>((q.flagsF8&~0x0eu)|2u); //11B570(2)
    collection.flagsF8=q.flagsF8;
    *cold.globalValid3E0730=0; //11B5F4;141C20 is the actual identity return
    s.owner9C=o.owner9C;s.callbackE4=0; //22D8C4/22D8CC
    object.filter.categories.mode=0; //124160(0)
    cold.sourcePC=0x124214u;
    cold.categoryD8.reset(new(std::nothrow) std::uint32_t(*o.world.staticObjectMask3E0720));
    if(!cold.categoryD8)return S::Storage; //124170/3080C0; earlier writes survive
    object.filter.categories={cold.categoryD8.get(),1,1,0};
    s.staticCategoryMaskD8=*cold.categoryD8;
    q.flagsF8=static_cast<std::uint8_t>(q.flagsF8|1u);collection.flagsF8=q.flagsF8;
    //11C710(1,0,0) uses independent native typed state for shape and result.
    //The original pair endpoints are initialized to3E02C0 here; the WORLD
    //body position is not read until22D928 and written later by11CA80.
    cold.sourcePC=0x11c710u;
    const OriginalDeathCameraQueryColdInput input{o.owner9C,sourceVf0,sourceVf0,
        *o.world.directTypeTagGp8010,*o.world.staticObjectMask3E0720,cold.categoryD8.get()};
    const auto projection=projectOriginalDeathCameraQueryCold(input,s,object,collection);
    if(projection.status!=S::Complete)return projection.status;
    s=projection.state;object=projection.object;collection=projection.collection;copies=projection.copies;
    *cold.geometry120={};cold.geometry120->world.v[0]=cold.geometry120->world.v[5]=
        cold.geometry120->world.v[10]=cold.geometry120->world.v[15]=1.f;
    cold.complete=true;cold.sourcePC=0x22d91cu;return S::Complete;
}
bool admitOriginalDeathCameraQueryStorage(const OriginalDeathCameraQueryOwner& o,
    const OriginalCameraOwnerSpan* writable,std::size_t writableCount,
    const OriginalCameraOwnerSpan* readable,std::size_t readableCount) noexcept {
    const V temporary{};OriginalDeathCameraQueryResult diagnostic{};
    return storage(o,temporary,temporary,diagnostic,writable,writableCount,readable,readableCount);
}
bool originalDeathCameraQueryAttached(const OriginalDeathCameraQueryOwner& o,std::uint64_t death68Key) noexcept {
    const V temporary{};OriginalDeathCameraQueryResult diagnostic{};
    return death68Key&&o.owner9C==death68Key&&storage(o,temporary,temporary,diagnostic)&&binding(o,true);
}
S attachOriginalDeathCameraQuery120(OriginalDeathCameraQueryOwner& o,CollectionRef50BX zone,const V& position,
    OriginalDeathCameraQueryResult& out) noexcept {
    if(!storage(o,position,position,out))return S::Storage;
    out={};if(!binding(o,o.membershipE0->attached)||!zone||zone>o.lifecycle->zoneCount)return fail(out,S::Binding,0x22d950u);
    if(std::fegetround()!=FE_TONEAREST)return fail(out,S::Arithmetic,0x22d950u);
    if(!setRay(o,position,position))return fail(out,S::Binding,0x11ca80u);
    if(!remove(o,out)||!registration(o,true,zone,out))return out.status;
    return S::Complete;
}
S refreshOriginalDeathCameraQuery120(OriginalDeathCameraQueryOwner& o,OriginalDeathCameraQueryResult& out) noexcept {
    const V temporary{};
    if(!storage(o,temporary,temporary,out))return S::Storage;
    out={};if(!binding(o,true))return fail(out,S::Binding,0x22dc50u);
    if(std::fegetround()!=FE_TONEAREST)return fail(out,S::Arithmetic,0x22dc50u);
    if(!registration(o,false,0,out))return out.status;
    return S::Complete;
}
S updateOriginalDeathCameraQuery120(OriginalDeathCameraQueryOwner& o,const V& start,const V& end,
    OriginalDeathCameraQueryResult& out) noexcept {
    if(!storage(o,start,end,out))return S::Storage;
    out={};if(!binding(o,true))return fail(out,S::Binding,0x22f724u);
    if(std::fegetround()!=FE_TONEAREST)return fail(out,S::Arithmetic,0x22f724u);
    if(!setRay(o,start,end))return fail(out,S::Binding,0x11ca80u);
    // Shape3's11FE50 has no work;11FF60 still calls the real120230.
    if(!registration(o,false,0,out))return out.status;
    auto in=o.world;const auto& m=*o.membershipE0;auto& s=*o.state;
    in.queryObject=o.object120;in.registrationChainPresent=m.attached;
    in.zoneLinks=m.storage.links;in.zoneLinkCount=m.storage.capacity;in.zoneHead=1;
    in.shapeTag=3;in.bounds=bounds(o);in.sentinel3E02C0=s.sentinel3E02C0;in.type3NormalMode44=s.rayNormalMode44;
    const OriginalCameraWorldCallbackBinding callback{s.owner9C,s.callbackE4,nullptr};
    const auto status=queryOriginalCameraWorld(in,o.mutableWorld,o.queryWorkspace,s.query120,callback,out.world);
    if(status!=OriginalCameraWorldStatus::Complete)return fail(out,S::WorldQuery,out.world.sourcePC);
    ++out.queriesCommitted;
    s.query120.flagsF8=o.mutableWorld.objects[o.object120-1].flagsF8;
    const CollisionNormalModeWrite50JA normalWrite{out.world.normalMode3DF5B0,out.world.normalModeWritten,true};
    if(!o.normalMode3DF5B0->acceptCompleted(normalWrite))return fail(out,S::Binding,0x126fa0u);
    if(out.world.resetGlobalsWritten)o.normalMode3DF5B0->acceptCompletedWorldReset();
    return S::Complete;
}
S removeOriginalDeathCameraQuery120(OriginalDeathCameraQueryOwner& o,OriginalDeathCameraQueryResult& out) noexcept {
    const V temporary{};
    if(!storage(o,temporary,temporary,out))return S::Storage;
    out={};if(!binding(o,o.membershipE0->attached))return fail(out,S::Binding,0x22d0c4u);
    if(!remove(o,out))return out.status;
    return S::Complete;
}
}

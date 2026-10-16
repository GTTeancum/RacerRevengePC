#include "original_camera_world_query.h"
#include "type3_prepare48p.h"
#include "type3_query_internal48u.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <optional>

namespace rr::game { namespace {
using S=OriginalCameraWorldStatus;
using Ref=CollectionRef50BX;
using V4=SpringVector4_48N;
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
    Span writable[10]{};unsigned used=0;
    template<class T>bool write(T* p,std::size_t n) noexcept {
        Span s;if(used==10||!span(p,n,s))return false;
        for(unsigned k=0;k<used;++k)if(overlap(s,writable[k]))return false;
        writable[used++]=s;return true;
    }
    template<class T>bool read(const T* p,std::size_t n) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(unsigned k=0;k<used;++k)if(overlap(s,writable[k]))return false;
        return true;
    }
};
bool writes(Guard& g,const OriginalCameraWorldMutable& s) noexcept {
    return g.write(s.objects,s.objectCapacity)&&g.write(s.methods,s.methodCapacity)&&g.write(s.copies,s.copyCapacity);
}
bool meshRead(const Guard& g,const assets::CollisionMesh48I* m) noexcept {
    return !m||(g.read(m,1)&&g.read(m->nodes.data(),m->nodes.size())&&
        g.read(m->faces.data(),m->faces.size())&&g.read(m->vertices.data(),m->vertices.size())&&
        g.read(m->vertex_normals.data(),m->vertex_normals.size()));
}
bool storage(const OriginalCameraWorldInput& in,const OriginalCameraWorldMutable& live,
    const OriginalCameraWorldWorkspace& work,OriginalCameraWorldQueryState& q,
    const OriginalCameraWorldCallbackBinding& callback,OriginalCameraWorldResult& out) noexcept {
    Guard g;
    if(!writes(g,live)||!writes(g,work.staging)||!g.write(work.meshStack,work.meshStackCapacity)||
       !g.write(&q,1)||!g.write(callback.follow1792D0,callback.follow1792D0?1u:0u)||!g.write(&out,1)||
       !g.read(&in,1)||!g.read(&live,1)||!g.read(&work,1)||!g.read(&callback,1)||
       !g.read(in.objects,in.objectCount)||!g.read(in.geometry,in.objectCount)||
       !g.read(in.methods,in.methodCount)||!g.read(in.zones,in.zoneCount)||
       !g.read(in.zoneLinks,in.zoneLinkCount)||
       !g.read(in.globalFallbackZone443108,in.globalFallbackZone443108?1u:0u)||
       !g.read(in.directTypeTagGp8010,in.directTypeTagGp8010?1u:0u)||
       !g.read(in.staticObjectMask3E0720,in.staticObjectMask3E0720?1u:0u)||
       !g.read(in.bounds.endpoints,in.bounds.endpointCount)||
       !g.read(in.bounds.endpointScenes,in.bounds.endpointSceneCount)||
       !g.read(in.bounds.scenes,in.bounds.sceneCount))return false;
    for(std::size_t k=0;k<in.objectCount;++k){
        const auto& f=in.objects[k].filter;
        for(const auto& ids:f.identities)if(!g.read(ids.values,ids.capacity))return false;
        if(!g.read(f.categories.values,f.categories.capacity))return false;
        const auto& geometry=in.geometry[k];if(!meshRead(g,geometry.mesh))return false;
        if(geometry.compound){
            const auto& c=*geometry.compound;
            if(!g.read(&c,1)||c.count>maxCompoundMembers50IJ||!g.read(c.parts,c.count))return false;
            for(std::size_t j=0;j<c.count;++j)if(!meshRead(g,c.parts[j].mesh))return false;
        }
    }
    for(std::size_t k=0;k<in.methodCount;++k){
        if(in.methods[k].method!=CollectionTree50BX::Method::SearchList)continue;
        const auto& list=in.methods[k].searchList;
        if(!g.read(list.blocks,list.blockCapacity))return false;
        for(std::size_t j=0;j<list.blockCapacity;++j)if(!g.read(list.blocks[j].words,list.blocks[j].capacity))return false;
    }
    return true;
}
struct Math {
    bool valid=true;
    float keep(float f) noexcept {valid=valid&&std::isfinite(f);return f;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {keep(a);keep(b);if(b==0.f){valid=false;return 0.f;}volatile float r=a/b;return keep(r);}
    float root(float a) noexcept {keep(a);volatile float r=static_cast<float>(std::sqrt(static_cast<double>(a)));return keep(r);}
    float squared(const V4& a,const V4& b) noexcept {
        const float x=sub(a.v[0],b.v[0]),y=sub(a.v[1],b.v[1]),z=sub(a.v[2],b.v[2]);
        return add(add(mul(x,x),mul(y,y)),mul(z,z));
    }
};
std::array<float,16> matrix(const BodyMatrix48G& m) noexcept {
    std::array<float,16> out{};std::memcpy(out.data(),m.v,sizeof m.v);return out;
}
Type3RayBuffers48P rays(OriginalCameraSingleRay& r) noexcept {
    return {&r.localStart,&r.localDirection,&r.worldStart,&r.worldDirection,&r.normal,&r.point,
        &r.hit,&r.fraction,1,&r.length,1};
}
struct Query {
    const OriginalCameraWorldInput& in;
    const OriginalCameraWorldWorkspace& work;
    OriginalCameraWorldQueryState state;
    bool invokeFollow1792D0=false;
    std::optional<OriginalCameraCollisionCallback> camera;
    OriginalCameraWorldResult& result;
    CollectionBoundsResult48S bounds{};
    Ref link=0,zone=0,method=0;std::size_t linkSteps=0;
    unsigned pass=1;float distance=0.f;
    bool fail(S s,std::uint32_t pc,Ref object=0) noexcept {
        if(result.status==S::Complete){result.status=s;result.sourcePC=pc;result.object=object;result.method=method;}
        return false;
    }
    bool ref(Ref r,std::size_t n) const noexcept {return r&&static_cast<std::size_t>(r)<=n;}
    bool select(Ref nextZone,std::uint32_t pc) noexcept {
        if(!ref(nextZone,in.zoneCount))return fail(S::Topology,pc);
        zone=nextZone;method=pass==1?in.zones[zone-1].firstMethod:in.zones[zone-1].secondMethod;
        return ref(method,in.methodCount)||fail(S::Binding,pc);
    }
    bool open() noexcept {
        if(!ref(method,in.methodCount))return fail(S::Topology,0x29470cu);
        if(in.methods[method-1].method!=CollectionTree50BX::Method::SearchList)return fail(S::UnsupportedMethod,0x294724u);
        auto& m=work.staging.methods[method-1];SearchListCursor50BR cursor{m.listBlock44,m.listSlot48,m.flags16};
        resetSearchList50BR(cursor,true);m.listBlock44=cursor.block44;m.listSlot48=cursor.slot48;m.flags16=cursor.flags16;return true;
    }
    bool methodNext(Ref& object) noexcept {
        object=0;if(!ref(method,in.methodCount))return fail(S::Topology,0x2947b8u);
        if(in.methods[method-1].method!=CollectionTree50BX::Method::SearchList)return fail(S::UnsupportedMethod,0x2947c4u);
        auto& m=work.staging.methods[method-1];SearchListCursor50BR cursor{m.listBlock44,m.listSlot48,m.flags16};std::uint32_t word=0;
        const auto status=nextSearchList50BR(in.methods[method-1].searchList,cursor,word);
        if(status!=SearchListStatus50BR::Complete)return fail(status==SearchListStatus50BR::InsufficientCapacity?S::Capacity:S::Topology,0x27e6c0u);
        m.listBlock44=cursor.block44;m.listSlot48=cursor.slot48;m.flags16=cursor.flags16;
        if(!word)return true;if(word&3u)return fail(S::Topology,0x27e6c0u);
        object=word>>2u;return ref(object,in.objectCount)||fail(S::Topology,0x121b68u,object);
    }
    bool next(Ref& object) noexcept {
        if(!methodNext(object)||object)return result.status==S::Complete;
        method=0;
        if(link){
            if(++linkSteps>in.zoneLinkCount)return fail(S::Topology,0x29483cu);
            link=in.zoneLinks[link-1].next;
            if(link){
                if(!ref(link,in.zoneLinkCount))return fail(S::Topology,0x294850u);
                if(!select(in.zoneLinks[link-1].zone,0x294850u))return false;
            }
        }
        //121920 first iterator passes t3=1; second iterator passes t3=0.
        if(!method&&pass==1&&*in.globalFallbackZone443108&&zone!=*in.globalFallbackZone443108)
            if(!select(*in.globalFallbackZone443108,0x294924u))return false;
        if(!method){object=0;return true;}
        if(!open())return false;
        return methodNext(object); // one newly opened method read, not a loop
    }
    bool list(const CollectionWords50BX& words,std::uint32_t value,bool category) noexcept {
        bool found=false;
        if(words.count>0){
            if(static_cast<std::size_t>(words.count)>words.capacity){fail(S::Capacity,0x120e70u);return false;}
            for(std::int32_t k=0;k<words.count;++k)if(category?(words.values[k]&value)!=0:words.values[k]==value){
                found=true;if(!category||words.mode==1)break;
            }
        }
        if(category)return words.mode==1?!found:found;
        if(words.count<=0)return true;
        return !(found&&words.mode==1)&&!(!found&&words.mode==0);
    }
    bool eligible(Ref object) noexcept {
        if(object==in.queryObject)return false;
        const auto& q=in.objects[in.queryObject-1].filter;const auto& c=in.objects[object-1].filter;
        if(!(work.staging.objects[object-1].flagsF8&4u)||!(c.flags31&1u)||!(c.categoryA4&q.acceptMaskB8))return false;
        for(unsigned k=0;k<2;++k)
            if(!list(q.identities[k],in.objects[object-1].identity,false)||!list(c.identities[k],in.objects[in.queryObject-1].identity,false))return false;
        return list(q.categories,c.categoryA4,true)&&list(c.categories,q.categoryA4,true);
    }
    bool candidate(Ref object) noexcept {
        ++result.visited;const auto& definition=in.objects[object-1];
        if(definition.typeTag!=*in.directTypeTagGp8010)return true; // constructor F8 bit0 skips groups
        state.currentCandidateIdentity=definition.identity;
        auto& current=work.staging.objects[object-1];current.flagsF8=static_cast<std::uint8_t>(current.flagsF8|0x10u);
        const auto& q=work.staging.objects[in.queryObject-1];Math m;
        const float gap=m.sub(m.root(m.squared(q.cachedCenter,current.cachedCenter)),m.add(q.cachedRadius,current.cachedRadius));
        if(!m.valid)return fail(S::Arithmetic,0x126eb0u,object);
        if(!(gap<=0.f)){if(pass==1&&gap<distance)distance=gap;return true;}
        if(pass==1)distance=0.f; //121BBC..121BC0, BEFORE eligibility
        if(!eligible(object))return result.status==S::Complete;
        const auto& geometry=in.geometry[object-1];bool hit=false;
        if(in.shapeTag==5){
            const OriginalCameraSphereQuery sphere{{{q.cachedCenter.v[0],q.cachedCenter.v[1],q.cachedCenter.v[2]}},in.sphereRadius50,in.sphereRadiusSquared54};
            const OriginalCameraSphereWorkspace stack{work.meshStack,work.meshStackCapacity};
            OriginalCameraSphereContact nextContact{};OriginalCameraSphereStatus status=OriginalCameraSphereStatus::Miss;
            if(definition.shapeTag==0){
                if(!definition.hasShape||!geometry.mesh)return fail(S::Binding,0x1242e8u,object);
                status=queryOriginalCameraSphereMesh(sphere,*geometry.mesh,matrix(geometry.world),state.sphere,stack,nextContact);
            }else if(definition.shapeTag==1){
                if(!definition.hasShape||!geometry.compound)return fail(S::Binding,0x1243e4u,object);
                status=queryOriginalCameraSphereCompound(sphere,*geometry.compound,state.sphere,stack,nextContact);
            }
            if(status!=OriginalCameraSphereStatus::Hit&&status!=OriginalCameraSphereStatus::Miss)return fail(S::Narrowphase,0x124270u,object);
            hit=status==OriginalCameraSphereStatus::Hit;if(hit)state.sphere=nextContact;
        }else{
            Type3QueryInput48P input{};input.normalMode=in.type3NormalMode44;
            Type3QueryContext48P context{};context.pairCount=1;context.entry=Type3QueryEntry48P::Dispatcher;
            Type3Candidate48P candidate{};candidate.shapeType=definition.shapeTag;
            candidate.collision.filter.flags31=definition.filter.flags31;
            candidate.collision.filter.identity=definition.identity;candidate.collision.geometry.id=definition.identity;
            candidate.compound=geometry.compound;
            const CollisionSphere48J coarse{{{current.cachedCenter.v[0],current.cachedCenter.v[1],current.cachedCenter.v[2]}},current.cachedRadius};
            result.normalModeWritten=true;result.normalMode3DF5B0=in.type3NormalMode44; //126FA0
            detail::Type3VectorScratch48U scratch{};
            const auto status=detail::dispatchType3BorrowedWithScratch48U(input,context,candidate,coarse,geometry.mesh,
                geometry.world,definition.hasShape,rays(state.ray),work.meshStack,work.meshStackCapacity,state.type3,hit,scratch);
            if(status!=Type3QueryStatus48P::Complete)return fail(S::Narrowphase,0x126f40u,object);
            if(scratch.written&&result.type3VectorScratchKnown)result.type3VectorScratchVF9W=scratch.vf9W;
        }
        if(!hit)return true;
        ++result.geometryHits;state.flagsF8=static_cast<std::uint8_t>(state.flagsF8|0x20u);
        auto& copies=work.staging.copies[object-1];copies.copied40=state.copied40;copies.copied50=state.copied50;
        //121D10..121D24 conditionally calls the query's actual E4. Death's
        //22D8CC null store skips this entire block, not the surrounding copies.
        if(invokeFollow1792D0){
          result.type3VectorScratchKnown=false; //No scratch claim for this separate callback route.
          ++result.callbacks; //1792D0 is invoked even when its category gate returns
          if(definition.filter.categoryA4&*in.staticObjectMask3E0720){
            if(in.shapeTag==5){
                if(state.sphere.count){camera->pointE0=state.sphere.worldPoint;camera->normalF0=state.sphere.worldNormal;}
            }else{
                //1792D0 casts result+4 as signed integer, NOT type3 hitCount60.
                //11DD00 copied sentinel into result00. Its ordinary Y=0 skips
                // the incompatible shape5-layout vector copy; never relabel it.
                std::int32_t rawY;std::memcpy(&rawY,&state.type3.result00.v[1],4);
                if(rawY>0)return fail(S::UnsupportedCallback,0x1792f0u,object);
            }
            camera->hit114=1;
          }
        }
        if(geometry.hasResponseCallbackE8)return fail(S::UnsupportedCallback,0x121d2cu,object);
        return true;
    }
    bool runPass(unsigned which) noexcept {
        pass=which;link=in.zoneHead;linkSteps=0;
        if(!ref(link,in.zoneLinkCount))return fail(S::Topology,0x294650u);
        if(!select(in.zoneLinks[link-1].zone,0x294650u)||!open())return false;
        for(;;){Ref object=0;if(!next(object))return false;if(!object)return true;if(!candidate(object))return false;}
    }
    bool run() noexcept {
        auto& q=work.staging.objects[in.queryObject-1];
        if(!(state.flagsF8&2u)){
            state.flagsF8=static_cast<std::uint8_t>(state.flagsF8&~0x20u);q.flagsF8=state.flagsF8;
            result.returnedDistance=std::numeric_limits<float>::max();return true;
        }
        if(!(state.flagsF8&1u))return fail(S::UnsupportedQuery,0x121d7cu);
        Math m;const float square=m.squared(q.cachedCenter,state.previousCenter70);float moved=0.f;
        if(square!=0.f){volatile float reciprocal=static_cast<float>(1.0/std::sqrt(static_cast<double>(square)));m.keep(reciprocal);moved=m.div(1.f,reciprocal);}
        state.distance84=m.sub(state.distance84,moved);state.previousCenter70=q.cachedCenter;
        if(!m.valid)return fail(S::Arithmetic,0x1219a0u);
        state.distance84=0.f; // a1=0 always enters query, even cached distance>0
        result.resetGlobalsWritten=true;
        state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&~0x20u)|0x10u);
        if(in.shapeTag==5)state.sphere.count=0;
        else{
            if(in.bounds.pairCount!=1||in.bounds.endpointCount<2)return fail(S::UnsupportedQuery,0x11dd00u);
            const std::uint32_t zeroScenes[2]={0,0};
            const auto* scenes=in.bounds.endpointScenes?in.bounds.endpointScenes:zeroScenes;
            const auto sceneCount=in.bounds.endpointScenes?in.bounds.endpointSceneCount:2u;
            const Type3PrepareInput48P prep{1,in.bounds.endpoints,in.bounds.endpointCount,scenes,sceneCount,in.sentinel3E02C0};
            auto buffers=rays(state.ray);
            if(prepareType3Rays48P(prep,buffers,state.type3)!=Type3PrepareStatus48P::Complete)return fail(S::Narrowphase,0x11dd00u);
            q.cachedCenter=state.type3.queryCenter;q.cachedRadius=state.type3.queryRadius;
        }
        auto b=in.bounds;b.cachedCenter=q.cachedCenter;b.cachedRadius=q.cachedRadius;
        if(prepareCollectionBounds48S(b,bounds)!=CollectionBoundsStatus48S::Complete)return fail(S::Arithmetic,0x1202e0u);
        q.cachedCenter=bounds.cachedCenter;q.cachedRadius=bounds.cachedRadius;
        //Actual120CC0 setsVF9.W=1;120D04/38/6C increment it to4. The
        //SearchList/filter/coarse-sphere paths and type3 result11E330/11E470
        //preserve this lane; only entered mesh transforms below replace it.
        if(in.shapeTag==3&&!invokeFollow1792D0){
            result.type3VectorScratchKnown=true;result.type3VectorScratchVF9W=4.f;
        }
        // SearchList27E660 leaves the initial min/max output vectors at zero.
        const float left=m.sub(m.sub(bounds.center.v[0],bounds.half.v[0]),0.f);
        const float back=m.sub(m.sub(bounds.center.v[2],bounds.half.v[2]),0.f);
        const float right=m.sub(0.f,m.add(bounds.center.v[0],bounds.half.v[0]));
        const float front=m.sub(0.f,m.add(bounds.center.v[2],bounds.half.v[2]));
        distance=detail::scalarMin48O(detail::scalarMin48O(detail::scalarMin48O(left,back),right),front);
        if(!m.valid)return fail(S::Arithmetic,0x121ae8u);
        if(!runPass(1))return false;
        bool distinct=false;Ref cursor=in.zoneHead;std::size_t steps=0;
        while(cursor){
            if(++steps>in.zoneLinkCount||!ref(cursor,in.zoneLinkCount))return fail(S::Topology,0x122090u);
            const auto z=in.zoneLinks[cursor-1].zone;if(!ref(z,in.zoneCount))return fail(S::Topology,0x122094u);
            distinct=distinct||in.zones[z-1].firstMethod!=in.zones[z-1].secondMethod;cursor=in.zoneLinks[cursor-1].next;
        }
        if(distinct&&!runPass(2))return false;
        if(state.flagsF8&0x20u){state.previousCenter70={{0.f,-10000.f,0.f,1.f}};distance=0.f;}
        state.distance84=distance;result.returnedDistance=distance;q.flagsF8=state.flagsF8;
        return true;
    }
};
} // namespace
OriginalCameraWorldStatus queryOriginalCameraWorld(const OriginalCameraWorldInput& in,
    const OriginalCameraWorldMutable& live,const OriginalCameraWorldWorkspace& work,
    OriginalCameraWorldQueryState& state,const OriginalCameraWorldCallbackBinding& callback,
    OriginalCameraWorldResult& result) noexcept {
    if(!storage(in,live,work,state,callback,result))return S::Storage;
    result={};auto fail=[&](S s,std::uint32_t pc){result.status=s;result.sourcePC=pc;return s;};
    if(!callback.owner9C||
       (callback.callbackE4==0&&callback.follow1792D0)||
       (callback.callbackE4!=0&&(callback.callbackE4!=0x1792d0u||!callback.follow1792D0)))
        return fail(S::Binding,0x121d10u);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::Arithmetic,0x121920u);
    if((in.shapeTag!=3&&in.shapeTag!=5)||in.bounds.shapeTag!=in.shapeTag)return fail(S::UnsupportedQuery,0x121bd4u);
    if(!in.methodCount||!in.zoneCount||!in.registrationChainPresent||!in.zoneHead||in.zoneHead>in.zoneLinkCount||
       !in.queryObject||in.queryObject>in.objectCount||!in.globalFallbackZone443108||
       !in.directTypeTagGp8010||!in.staticObjectMask3E0720||
       *in.globalFallbackZone443108>in.zoneCount)return fail(S::Binding,0x121ac4u);
    if(in.objects[in.queryObject-1].shapeTag!=in.shapeTag||
       live.objects==nullptr||live.objectCapacity<in.objectCount||work.staging.objectCapacity<in.objectCount||
       live.methodCapacity<in.methodCount||work.staging.methodCapacity<in.methodCount||
       live.copyCapacity<in.objectCount||work.staging.copyCapacity<in.objectCount)return fail(S::Capacity,0x121920u);
    if(live.objects[in.queryObject-1].flagsF8!=state.flagsF8||
       std::memcmp(&live.copies[in.queryObject-1].copied40,&state.copied40,sizeof state.copied40)||
       std::memcmp(&live.copies[in.queryObject-1].copied50,&state.copied50,sizeof state.copied50))return fail(S::Binding,0x121940u);
    if(in.objectCount>(std::numeric_limits<Ref>::max()>>2u))return fail(S::Capacity,0x294740u);
    for(std::size_t k=0;k<in.objectCount;++k){
        if(!in.objects[k].identity)return fail(S::Binding,0x120e70u);
        for(std::size_t j=0;j<k;++j)if(in.objects[j].identity==in.objects[k].identity)return fail(S::Topology,0x120e70u);
    }
    std::memcpy(work.staging.objects,live.objects,in.objectCount*sizeof(*live.objects));
    std::memcpy(work.staging.methods,live.methods,in.methodCount*sizeof(*live.methods));
    std::memcpy(work.staging.copies,live.copies,in.objectCount*sizeof(*live.copies));
    const bool invokesFollow=callback.callbackE4==0x1792d0u;
    Query query{in,work,state,invokesFollow,
        invokesFollow?std::optional<OriginalCameraCollisionCallback>{*callback.follow1792D0}:std::nullopt,result};
    if(!query.run())return result.status==S::Complete?fail(S::Topology,0x121920u):result.status;
    std::memcpy(live.objects,work.staging.objects,in.objectCount*sizeof(*live.objects));
    std::memcpy(live.methods,work.staging.methods,in.methodCount*sizeof(*live.methods));
    std::memcpy(live.copies,work.staging.copies,in.objectCount*sizeof(*live.copies));
    state=query.state;
    if(invokesFollow)*callback.follow1792D0=*query.camera;
    return S::Complete;
}
OriginalCameraWorldStatus queryOriginalCameraWorld(const OriginalCameraWorldInput& in,
    const OriginalCameraWorldMutable& live,const OriginalCameraWorldWorkspace& work,
    OriginalCameraWorldQueryState& state,OriginalCameraCollisionCallback& camera,
    OriginalCameraWorldResult& result) noexcept {
    const OriginalCameraWorldCallbackBinding callback{
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&camera)),0x1792d0u,&camera};
    return queryOriginalCameraWorld(in,live,work,state,callback,result);
}
}

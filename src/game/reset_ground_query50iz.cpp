#include "reset_ground_query50iz.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using Status=ResetGroundStatus50IZ;
using Ref=CollectionRef50BX;
struct Span {std::uintptr_t first=0,last=0;};
template<class T> bool span(const T* p,std::size_t count,Span& out) noexcept {
    out={};if(!count)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||
       count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto first=reinterpret_cast<std::uintptr_t>(p),bytes=count*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
    out={first,first+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writable[7]{};unsigned count=0;
    template<class T> bool write(T* p,std::size_t n) noexcept {
        Span s;if(count==7||!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlap(s,writable[i]))return false;
        writable[count++]=s;return true;
    }
    template<class T> bool read(const T* p,std::size_t n) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlap(s,writable[i]))return false;
        return true;
    }
};
bool storage(const ResetGroundInput50IZ& in,const ResetGroundWorld50IZ& world,
    const ResetGroundMutable50IZ& live,const ResetGroundWorkspace50IZ& work,
    ResetGroundResult50IZ& out,ResetGroundDiagnostic50IZ& diagnostic) noexcept {
    Guard g;
    if(!g.write(live.objects,live.objectCapacity)||!g.write(live.methods,live.methodCapacity)||
       !g.write(work.objects,work.objectCapacity)||!g.write(work.methods,work.methodCapacity)||
       !g.write(work.meshStack,work.meshStackCapacity)||!g.write(&out,1)||!g.write(&diagnostic,1))return false;
    if(!g.read(&in,1)||!g.read(&world,1)||!g.read(&live,1)||!g.read(&work,1)||
       !g.read(world.objects,world.objectCount)||!g.read(world.geometry,world.objectCount)||
       !g.read(world.methods,world.methodCount)||!g.read(world.zones,world.zoneCount)||
       !g.read(in.globalFallbackZone443108,in.globalFallbackZone443108?1u:0u)||
       !g.read(in.directTypeTagGp8010,in.directTypeTagGp8010?1u:0u)||
       !g.read(in.normalMode3DF5B0,in.normalMode3DF5B0?1u:0u))return false;
    for(std::size_t i=0;i<world.methodCount;++i){
        const auto& method=world.methods[i];
        if(method.method!=CollectionTree50BX::Method::SearchList)continue;
        const auto& list=method.searchList;
        if(!g.read(list.blocks,list.blockCapacity))return false;
        for(std::size_t k=0;k<list.blockCapacity;++k)
            if(!g.read(list.blocks[k].words,list.blocks[k].capacity))return false;
    }
    for(std::size_t i=0;i<world.objectCount;++i){
        const auto* candidate=world.geometry[i].shape0;
        if(!candidate)continue;
        if(!g.read(candidate,1))return false;
        if(!candidate->mesh)continue;
        const auto& mesh=*candidate->mesh;
        if(!g.read(&mesh,1)||!g.read(mesh.nodes.data(),mesh.nodes.size())||
           !g.read(mesh.faces.data(),mesh.faces.size())||!g.read(mesh.vertices.data(),mesh.vertices.size())||
           !g.read(mesh.vertex_normals.data(),mesh.vertex_normals.size()))return false;
    }
    // Unused122350 identity/category-list spans and shape-group projections
    // are not consumed by this exact staticOnly=1/exclusionCount=0 call.
    return true;
}
struct Math {
    bool valid=true;
    float keep(float v) noexcept {valid=valid&&std::isfinite(v);return v;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float x=a*b;return keep(x);}
};
float word(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
struct Query {
    const ResetGroundInput50IZ& in;
    const ResetGroundWorld50IZ& world;
    const ResetGroundWorkspace50IZ& work;
    ResetGroundResult50IZ result;
    ResetGroundDiagnostic50IZ& diagnostic;
    Ref zone=0,method=0;
    bool failed=false;
    bool fail(Status status,std::uint32_t pc,Ref object=0,
        ResetTreeStatus50AK narrow=ResetTreeStatus50AK::Complete) noexcept {
        if(!failed)diagnostic={status,pc,object,method,zone,narrow};
        failed=true;return false;
    }
    bool validRef(Ref value,std::size_t count) const noexcept {
        return value&&static_cast<std::size_t>(value)<=count;
    }
    bool select(Ref requested,std::uint32_t pc) noexcept {
        if(!validRef(requested,world.zoneCount))return fail(Status::Topology,pc);
        zone=requested;const auto& z=world.zones[zone-1u];
        method=z.firstMethod?z.firstMethod:z.secondMethod;
        return validRef(method,world.methodCount)||fail(Status::Binding,pc);
    }
    bool resetMethod(Ref target,bool open,std::uint32_t pc) noexcept {
        if(!validRef(target,world.methodCount)){
            fail(Status::Topology,pc);diagnostic.method=target;return false;
        }
        if(world.methods[target-1u].method!=CollectionTree50BX::Method::SearchList){
            fail(Status::UnsupportedMethod,pc);diagnostic.method=target;return false;
        }
        auto& state=work.methods[target-1u];
        SearchListCursor50BR cursor{state.listBlock44,state.listSlot48,state.flags16};
        resetSearchList50BR(cursor,open);
        state.listBlock44=cursor.block44;state.listSlot48=cursor.slot48;state.flags16=cursor.flags16;
        return true;
    }
    bool methodNext(Ref& object) noexcept {
        object=0;
        if(!validRef(method,world.methodCount))return fail(Status::Topology,0x2947b8u);
        const auto& def=world.methods[method-1u];
        if(def.method!=CollectionTree50BX::Method::SearchList)
            return fail(Status::UnsupportedMethod,0x2947c4u);
        auto& state=work.methods[method-1u];
        SearchListCursor50BR cursor{state.listBlock44,state.listSlot48,state.flags16};
        std::uint32_t encoded=0;
        const auto status=nextSearchList50BR(def.searchList,cursor,encoded);
        if(status!=SearchListStatus50BR::Complete)
            return fail(status==SearchListStatus50BR::InsufficientCapacity?Status::Capacity:Status::Topology,
                0x27e6c0u);
        state.listBlock44=cursor.block44;state.listSlot48=cursor.slot48;state.flags16=cursor.flags16;
        if(!encoded)return true;
        if(encoded&3u)return fail(Status::Topology,0x27e6c0u);
        object=encoded>>2u;
        return validRef(object,world.objectCount)||fail(Status::Topology,0x124df8u,object);
    }
    bool next(Ref& object) noexcept {
        if(!methodNext(object)||object)return !failed;
        const auto& z=world.zones[zone-1u];
        //2947DC..294824. Same first/second pointer is never opened twice.
        method=method==z.firstMethod&&z.secondMethod!=z.firstMethod?z.secondMethod:0u;
        // The actual reset280890 temporary node has next=0.294830..294848
        // therefore exhausts its one-entry chain on the first zone exit.
        const auto fallback=*in.globalFallbackZone443108;
        if(!method&&fallback&&zone!=fallback){
            if(!select(fallback,0x2949b0u))return false;
        }
        if(!method){object=0;return true;}
        if(!resetMethod(method,true,0x294b14u))return false;
        //294B28 is exactly ONE read from the newly opened method. A zero
        // ends124CB0 even if some other zone/method might contain candidates.
        return methodNext(object);
    }
    bool candidate(Ref ref) noexcept {
        if(result.visited==std::numeric_limits<std::size_t>::max())
            return fail(Status::Capacity,0x124df8u,ref);
        const auto& object=world.objects[ref-1u];++result.visited;
        if(object.typeTag!=*in.directTypeTagGp8010)return true; //124EA8 staticOnly=1.
        //124E08..124E40 exclusionCount=0, then124E44 category mask.
        if(!(object.filter.categoryA4&in.floorMask))return true;
        ++result.eligible;
        auto& state=work.objects[ref-1u];
        state.flagsF8=static_cast<std::uint8_t>(state.flagsF8|0x10u); //124E84, before eligibility31.
        if(!(object.filter.flags31&1u))return true; //1245A8..1245C8 genuine early return.
        if(object.shapeTag!=0u)return fail(Status::UnsupportedShape,0x1247f8u,ref);
        const auto* shape=world.geometry[ref-1u].shape0;
        if(!object.hasShape||!shape||!shape->mesh||shape->id!=object.identity)
            return fail(Status::Binding,0x12482cu,ref);
        const Vector3_48C center{{state.cachedCenter.v[0],state.cachedCenter.v[1],state.cachedCenter.v[2]}};
        ResetCandidateResult50AM found{};
        found.point=result.point;found.normal=result.normal;
        ++result.queried;
        const auto status=queryResetCandidate50AM(in.start,result.end,shape->worldMatrix,center,
            state.cachedRadius,object.filter.flags31,*shape->mesh,*in.normalMode3DF5B0,
            work.meshStack,work.meshStackCapacity,found);
        if(status!=ResetTreeStatus50AK::Complete)return fail(Status::Narrowphase,0x124e88u,ref,status);
        if(found.hit){
            result.point=found.point;result.normal=found.normal;
            result.end=found.point; //124E98..124EA4, all four lanes.
            result.hit=true;result.lastHitObject=ref;++result.hits;
        }
        return true;
    }
    bool close() noexcept {
        const auto& z=world.zones[zone-1u];
        //294B40 closes both distinct methods of the FINAL current zone,
        // regardless of which method yielded the last object or was opened.
        if(z.firstMethod&&!resetMethod(z.firstMethod,false,0x294b68u))return false;
        if(z.secondMethod&&z.secondMethod!=z.firstMethod&&
           !resetMethod(z.secondMethod,false,0x294b94u))return false;
        return true;
    }
    bool run() noexcept {
        result.end=in.end;result.hit=false;
        result.visited=result.eligible=result.queried=result.hits=0;
        result.lastHitObject=result.finalZone=0;
        result.searchCenter={{0.f,0.f,0.f,1.f}};
        result.searchHalf={{0.f,word(0x7effffffu),0.f,1.f}};
        Math f;
        result.searchCenter.v[0]=f.mul(.5f,f.add(in.start.v[0],in.end.v[0]));
        result.searchCenter.v[2]=f.mul(.5f,f.add(in.start.v[2],in.end.v[2]));
        float halfX=f.mul(.5f,f.sub(in.start.v[0],in.end.v[0]));
        float halfZ=f.mul(.5f,f.sub(in.start.v[2],in.end.v[2]));
        if(halfX<0.f)halfX=-halfX;
        if(halfZ<0.f)halfZ=-halfZ;
        result.searchHalf.v[0]=halfX;result.searchHalf.v[2]=halfZ;
        if(!f.valid)return fail(Status::Arithmetic,0x124d30u);
        if(!select(in.zoneC34,0x294650u)||!resetMethod(method,true,0x294724u))return false;
        for(;;){Ref object=0;if(!next(object))return false;if(!object)break;
            if(!candidate(object))return false;
        }
        result.finalZone=zone;return close();
    }
};
} // namespace

ResetGroundStatus50IZ queryResetGround50IZ(const ResetGroundInput50IZ& in,
    const ResetGroundWorld50IZ& world,const ResetGroundMutable50IZ& live,
    const ResetGroundWorkspace50IZ& work,ResetGroundResult50IZ& out,
    ResetGroundDiagnostic50IZ& diagnostic) noexcept {
    // Storage rejection does not write a possibly aliased diagnostic.
    if(!storage(in,world,live,work,out,diagnostic))return Status::Storage;
    auto fail=[&](Status s,std::uint32_t pc) noexcept {
        diagnostic={s,pc,0,0,0,ResetTreeStatus50AK::Complete};return s;
    };
    diagnostic={};
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::RoundingMode,0x124cb0u);
    if(!world.objectCount||!world.methodCount||!world.zoneCount||!in.zoneC34||
       in.zoneC34>world.zoneCount||!in.globalFallbackZone443108||!in.directTypeTagGp8010||
       !in.normalMode3DF5B0)return fail(Status::Binding,0x124cb0u);
    if(*in.globalFallbackZone443108>world.zoneCount)return fail(Status::Binding,0x294924u);
    if(world.objectCount>(std::numeric_limits<Ref>::max()>>2u)||
       world.methodCount>std::numeric_limits<Ref>::max()||world.zoneCount>std::numeric_limits<Ref>::max())
        return fail(Status::Capacity,0x294600u);
    if(live.objectCapacity<world.objectCount||work.objectCapacity<world.objectCount||
       live.methodCapacity<world.methodCount||work.methodCapacity<world.methodCount)
        return fail(Status::Capacity,0x294600u);
    for(std::size_t i=0;i<world.objectCount;++i){
        if(!world.objects[i].identity)return fail(Status::Binding,0x124df8u);
        for(std::size_t j=0;j<i;++j)if(world.objects[i].identity==world.objects[j].identity)
            return fail(Status::Topology,0x124df8u);
    }
    for(std::size_t i=0;i<world.zoneCount;++i)
        if(world.zones[i].firstMethod>world.methodCount||world.zones[i].secondMethod>world.methodCount)
            return fail(Status::Topology,0x294650u);
    std::memcpy(work.objects,live.objects,world.objectCount*sizeof(*work.objects));
    std::memcpy(work.methods,live.methods,world.methodCount*sizeof(*work.methods));
    Query query{in,world,work,out,diagnostic};
    if(!query.run())return diagnostic.status;
    std::memcpy(live.objects,work.objects,world.objectCount*sizeof(*live.objects));
    std::memcpy(live.methods,work.methods,world.methodCount*sizeof(*live.methods));
    out=query.result;diagnostic={};return Status::Complete;
}
} // namespace rr::game

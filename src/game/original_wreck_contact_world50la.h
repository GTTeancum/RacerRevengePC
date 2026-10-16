#pragma once
#include "original_wreck_owner50la.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
enum class OriginalWreckContactWorldStatus50LA : std::uint8_t {
    complete,owner,binding,storage,capacity,topology,method,group,shape,arithmetic,narrowphase
};
struct OriginalWreckContactWorldResult50LA {
    OriginalWreckContactWorldStatus50LA status=OriginalWreckContactWorldStatus50LA::owner;
    std::uint32_t sourcePC=0;
    CollectionRef50BX object=0,method=0;
    std::size_t visited=0,dispatches=0;
    float returnedDistance=0.f;
    bool queryCommitted=false;
};
namespace original_wreck_world_detail50LA {
using S=OriginalWreckContactWorldStatus50LA;
using Ref=CollectionRef50BX;
struct Span {std::uintptr_t first=0,last=0;};
template<class T>inline bool span(const T* p,std::size_t n,Span& s)noexcept {
    s={};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto first=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
    s={first,first+bytes};return true;
}
inline bool overlap(Span a,Span b)noexcept{return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writes[32]{};unsigned count=0;
    template<class T>bool write(T* p,std::size_t n)noexcept {
        Span s;if(count==32||!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        writes[count++]=s;return true;
    }
    template<class T>bool read(const T* p,std::size_t n)const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        return true;
    }
};
inline bool worldWrites(Guard& g,const OriginalCameraWorldMutable& v)noexcept {
    return g.write(v.objects,v.objectCapacity)&&g.write(v.methods,v.methodCapacity)&&g.write(v.copies,v.copyCapacity);
}
inline bool meshRead(const Guard& g,const assets::CollisionMesh48I* m)noexcept {
    return !m||(g.read(m,1)&&g.read(m->nodes.data(),m->nodes.size())&&g.read(m->faces.data(),m->faces.size())&&
        g.read(m->vertices.data(),m->vertices.size())&&g.read(m->vertex_normals.data(),m->vertex_normals.size()));
}
inline bool compoundRead(const Guard& g,const CompoundCollisionView50IJ* p)noexcept {
    if(!p)return true;
    if(!g.read(p,1)||p->count>maxCompoundMembers50IJ||!g.read(p->parts,p->count))return false;
    for(std::size_t i=0;i<p->count;++i)
        if(!meshRead(g,p->parts[i].mesh)||!g.read(p->parts[i].nodeRadii,p->parts[i].nodeRadiusCount))return false;
    return true;
}
inline bool storage(OriginalOnePieceWreckOwner50LA& owner,OriginalWreckContactWorldResult50LA& out)noexcept {
    auto& q=owner.queries.query(0);auto& b=*q.type2;const auto& w=owner.queries.worldBinding();
    const auto& in=w.worldQuery;Guard g;
    if(!g.write(&q,1)||!g.write(&b,1)||!g.write(&owner.physical.spring,1)||
       !g.write(owner.physical.records.data(),owner.physical.records.size())||
       !worldWrites(g,w.mutableQuery)||!worldWrites(g,w.queryWorkspace.staging)||
       !g.write(w.normalMode3DF5B0,1)||!g.write(&out,1)||
       !g.write(b.stack.data(),b.stack.size()))return false;
    for(auto* bank:{&b.live,&b.banks[0],&b.banks[1],&b.scratch})
        if(!g.write(bank->pairs.data(),bank->pairs.size())||!g.write(bank->metadata.data(),bank->metadata.size()))return false;
    if(!g.read(&w,1)||!g.read(in.objects,in.objectCount)||!g.read(in.geometry,in.objectCount)||
       !g.read(in.methods,in.methodCount)||!g.read(in.zones,in.zoneCount)||
       !g.read(q.membership.storage.links,q.membership.storage.capacity)||
       !g.read(q.membership.storage.entries,q.membership.storage.capacity)||
       !g.read(in.globalFallbackZone443108,1)||!g.read(w.collisionTypeGp8010,1)||
       !g.read(w.groupTypeGp8014,1)||!g.read(w.skipCategory3E0728,1)||
       !g.read(w.physicsBodyCategory3E0718,1)||!g.read(in.staticObjectMask3E0720,1)||
       !g.read(w.sentinel3E02C0,1)||!g.read(w.type2Targets,w.type2TargetCount))return false;
    const auto& resource=owner.assets.queryE0();
    if(!g.read(&resource,1)||!g.read(resource.nodes,resource.nodeCount)||!g.read(resource.rays,resource.rayCount))return false;
    for(std::size_t i=0;i<in.objectCount;++i){
        const auto& f=in.objects[i].filter;
        for(const auto& ids:f.identities)if(!g.read(ids.values,ids.capacity))return false;
        if(!g.read(f.categories.values,f.categories.capacity)||!meshRead(g,in.geometry[i].mesh)||
           !compoundRead(g,in.geometry[i].compound))return false;
    }
    for(std::size_t i=0;i<w.type2TargetCount;++i){
        const auto* t=w.type2Targets[i];if(!g.read(t,1))return false;
        const auto& c=t->candidate;const auto& f=c.collision.filter;
        if(!g.read(c.sceneName,c.sceneNameBytes)||!g.read(t->nodeRadii,t->nodeRadiusCount)||
           !meshRead(g,c.collision.geometry.mesh)||!compoundRead(g,t->compound)||
           !g.read(f.categoryMasks.data(),f.categoryMasks.size()))return false;
        for(const auto& list:f.identityLists)if(!g.read(list.identities.data(),list.identities.size()))return false;
    }
    for(std::size_t i=0;i<in.methodCount;++i){
        if(in.methods[i].method!=CollectionTree50BX::Method::SearchList)continue;
        const auto& list=in.methods[i].searchList;
        if(!g.read(list.blocks,list.blockCapacity))return false;
        for(std::size_t j=0;j<list.blockCapacity;++j)if(!g.read(list.blocks[j].words,list.blocks[j].capacity))return false;
    }
    return true;
}
struct Math {
    bool valid=true;
    float keep(float x)noexcept{valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b)noexcept{keep(a);keep(b);volatile float x=a+b;return keep(x);}
    float sub(float a,float b)noexcept{keep(a);keep(b);volatile float x=a-b;return keep(x);}
    float mul(float a,float b)noexcept{keep(a);keep(b);volatile float x=a*b;return keep(x);}
    float div(float a,float b)noexcept{keep(a);keep(b);if(b==0.f){valid=false;return 0.f;}volatile float x=a/b;return keep(x);}
    float root(float a)noexcept{keep(a);volatile float x=static_cast<float>(std::sqrt(static_cast<double>(a)));return keep(x);}
    float squared(const SpringVector4_48N& a,const SpringVector4_48N& b)noexcept {
        const float x=sub(a.v[0],b.v[0]),y=sub(a.v[1],b.v[1]),z=sub(a.v[2],b.v[2]);
        return add(add(mul(x,x),mul(y,y)),mul(z,z));
    }
};
inline bool words(const CollectionWords50BX& a,const std::vector<std::uint32_t>& b)noexcept {
    if(a.count<=0)return b.empty();
    if(static_cast<std::size_t>(a.count)>a.capacity||static_cast<std::size_t>(a.count)!=b.size()||!a.values)return false;
    for(std::size_t i=0;i<b.size();++i)if(a.values[i]!=b[i])return false;
    return true;
}
inline bool sameFilter(const CollectionObject50BX& a,const CollisionFilter48J& b)noexcept {
    if(a.identity!=b.identity||a.filter.categoryA4!=b.categoryA4||a.filter.acceptMaskB8!=b.acceptMaskB8||
       a.filter.flags31!=b.flags31||a.filter.categories.mode!=b.categoryModeD4||!words(a.filter.categories,b.categoryMasks))return false;
    for(unsigned i=0;i<2;++i)if(a.filter.identities[i].mode!=b.identityLists[i].mode||
        !words(a.filter.identities[i],b.identityLists[i].identities))return false;
    return true;
}
struct Transaction {
    OriginalOnePieceWreckOwner50LA& owner;
    OriginalWreckContactWorldResult50LA& report;
    const OriginalWreckQueryWorld50LA& world;
    OriginalWreckQueries50LA::Query& query;
    OriginalWreckType2Storage50LA& buffer;
    Type2StaticResult49X result{};
    SpringVector4_48N previous70{};
    std::uint32_t currentCategory=0,currentIdentity=0;
    float distance=0.f;
    Ref link=0,zone=0,method=0;
    std::size_t linkSteps=0;
    unsigned pass=1,bank=0;
    bool fail(S s,std::uint32_t pc,Ref object=0)noexcept {
        if(report.status==S::complete){report.status=s;report.sourcePC=pc;report.object=object;report.method=method;}
        return false;
    }
    static bool ref(Ref r,std::size_t n)noexcept{return r&&r<=n;}
    bool select(Ref z,std::uint32_t pc)noexcept {
        const auto& in=world.worldQuery;
        if(!ref(z,in.zoneCount))return fail(S::topology,pc);
        zone=z;method=pass==1?in.zones[z-1].firstMethod:in.zones[z-1].secondMethod;
        return ref(method,in.methodCount)||fail(S::binding,pc);
    }
    bool open()noexcept {
        if(!ref(method,world.worldQuery.methodCount))return fail(S::topology,0x29470cu);
        if(world.worldQuery.methods[method-1].method!=CollectionTree50BX::Method::SearchList)return fail(S::method,0x294724u);
        auto& m=world.queryWorkspace.staging.methods[method-1];
        SearchListCursor50BR c{m.listBlock44,m.listSlot48,m.flags16};resetSearchList50BR(c,true);
        m.listBlock44=c.block44;m.listSlot48=c.slot48;m.flags16=c.flags16;return true;
    }
    bool methodNext(Ref& object)noexcept {
        object=0;const auto& in=world.worldQuery;
        if(!ref(method,in.methodCount))return fail(S::topology,0x2947b8u);
        if(in.methods[method-1].method!=CollectionTree50BX::Method::SearchList)return fail(S::method,0x2947c4u);
        auto& m=world.queryWorkspace.staging.methods[method-1];SearchListCursor50BR c{m.listBlock44,m.listSlot48,m.flags16};
        std::uint32_t word=0;const auto status=nextSearchList50BR(in.methods[method-1].searchList,c,word);
        if(status!=SearchListStatus50BR::Complete)return fail(status==SearchListStatus50BR::InsufficientCapacity?S::capacity:S::topology,0x27e6c0u);
        m.listBlock44=c.block44;m.listSlot48=c.slot48;m.flags16=c.flags16;
        if(!word)return true;if(word&3u)return fail(S::topology,0x27e6c0u);
        object=word>>2u;return ref(object,world.objectCount)||fail(S::topology,0x121b68u,object);
    }
    bool next(Ref& object)noexcept {
        if(!methodNext(object)||object)return report.status==S::complete;
        method=0;
        if(link){
            if(++linkSteps>query.membership.storage.capacity)return fail(S::topology,0x29483cu);
            link=query.membership.storage.links[link-1].next;
            if(link){
                if(!ref(link,query.membership.storage.capacity))return fail(S::topology,0x294850u);
                if(!select(query.membership.storage.links[link-1].zone,0x294850u))return false;
            }
        }
        const auto fallback=*world.worldQuery.globalFallbackZone443108;
        if(!method&&pass==1&&fallback&&zone!=fallback)if(!select(fallback,0x294924u))return false;
        if(!method){object=0;return true;}
        if(!open())return false;
        return methodNext(object); // source reads one newly opened method only
    }
    bool list(const CollectionWords50BX& l,std::uint32_t value,bool category)noexcept {
        bool found=false;
        if(l.count>0){
            if(!l.values||static_cast<std::size_t>(l.count)>l.capacity){fail(S::capacity,0x120e70u);return false;}
            for(std::int32_t i=0;i<l.count;++i)if(category?(l.values[i]&value)!=0:l.values[i]==value){found=true;if(!category||l.mode==1)break;}
        }
        if(category)return l.mode==1?!found:found;
        if(l.count<=0)return true;
        return !(found&&l.mode==1)&&!(!found&&l.mode==0);
    }
    bool eligible(Ref object)noexcept {
        if(object==owner.queries.ref(0))return false;
        const auto& q=owner.queries.object(0).filter;const auto& c=world.objects[object-1].filter;
        if(!(world.queryWorkspace.staging.objects[object-1].flagsF8&4u)||!(c.flags31&1u)||!(c.categoryA4&q.acceptMaskB8))return false;
        for(unsigned i=0;i<2;++i)if(!list(q.identities[i],world.objects[object-1].identity,false)||
            !list(c.identities[i],owner.queries.object(0).identity,false))return false;
        return list(q.categories,c.categoryA4,true)&&list(c.categories,q.categoryA4,true);
    }
    bool dispatch(Ref object)noexcept {
        const auto& definition=world.objects[object-1];const auto& geometry=world.geometry[object-1];
        if(!definition.hasShape)return fail(S::binding,0x121410u,object);
        if(definition.shapeTag!=0&&definition.shapeTag!=1)return fail(S::shape,0x121410u,object);
        if(object>world.type2TargetCount)return fail(S::binding,0x121410u,object);
        const auto& target=*world.type2Targets[object-1];const auto& c=target.candidate.collision;
        if(target.shapeType!=definition.shapeTag||c.geometry.id!=definition.identity||
           !sameFilter(definition,c.filter)||c.geometry.mesh!=geometry.mesh||target.compound!=geometry.compound||
           c.hasResponseCallbackE8!=geometry.hasResponseCallbackE8)return fail(S::binding,0x121410u,object);
        CollisionFilter48J filter{};filter.identity=owner.queries.object(0).identity;
        filter.flagsF8=result.queryFlagsF8;
        // The real120E70 above already used BOTH canonical filters. This
        // scalar-only projection is consumed by continuation initialization,
        // never substituted for the source eligibility gates.
        Type2StaticQuery49X input{};input.resource=&owner.assets.queryE0();std::memcpy(&input.worldMatrix,&query.type2World80,sizeof input.worldMatrix);
        input.filter=&filter;input.queryScene=owner.modelSceneIdentity();
        input.skipMask=*world.skipCategory3E0728;input.dynamicMask=*world.physicsBodyCategory3E0718;
        input.materialMask=*world.worldQuery.staticObjectMask3E0720;input.normalMode54=query.normalMode;
        input.normalMode54Bound=true;input.resetRecordsBefore=false;
        const auto& qs=world.queryWorkspace.staging.objects[owner.queries.ref(0)-1];
        input.sphere.radius=qs.cachedRadius;for(unsigned i=0;i<3;++i)input.sphere.center.v[i]=qs.cachedCenter.v[i];
        const auto& copies=owner.queries.copies(0);
        std::memcpy(input.copied40.data(),&copies.copied40,16);std::memcpy(input.copied50.data(),&copies.copied50,16);
        auto& before=buffer.banks[bank];auto& after=buffer.banks[bank^1u];auto& scratch=buffer.scratch;
        Type2StaticInitial49X initial{};std::memcpy(&initial.selected,&result.selected,sizeof initial.selected);initial.records=result.records;
        initial.pairs=before.pairs.data();initial.metadata=before.metadata.data();initial.pairSlots=before.pairs.size();
        initial.recordSlots=before.records.data();initial.recordCount=before.records.size();
        Type2StaticWorkspace49X work{scratch.pairs.data(),scratch.pairs.size(),scratch.metadata.data(),scratch.metadata.size(),
            scratch.records.data(),scratch.records.size(),&scratch.effect,1,buffer.stack.data(),buffer.stack.size(),nullptr,0};
        Type2StaticDestination49X destination{after.pairs.data(),after.pairs.size(),after.metadata.data(),after.metadata.size(),
            after.records.data(),after.records.size(),&after.effect,1};
        TerrainCandidateEffects48J currentEffect{};currentEffect.identity=definition.identity;
        currentEffect.flagsF8=world.queryWorkspace.staging.objects[object-1].flagsF8;
        const auto& priorCopies=world.queryWorkspace.staging.copies[object-1];
        std::memcpy(currentEffect.copied40.data(),&priorCopies.copied40,16);std::memcpy(currentEffect.copied50.data(),&priorCopies.copied50,16);
        const Type2WorldContinuation50CC continuation{&result,currentCategory,definition.shapeTag==0?&geometry.world:nullptr,&currentEffect};
        Type2StaticResult49X nextResult{};std::uint32_t nextCategory=0;
        const auto status=continueType2WorldCandidate50CC(input,target,initial,work,destination,continuation,nextResult,nextCategory);
        if(status!=Type2StaticStatus49X::Complete)return fail(S::narrowphase,0x121410u,object);
        std::memcpy(&result,&nextResult,sizeof result);currentCategory=nextCategory;bank^=1u;++report.dispatches;
        auto& out=world.queryWorkspace.staging;
        out.objects[object-1].flagsF8=after.effect.flagsF8;
        std::memcpy(&out.copies[object-1].copied40,after.effect.copied40.data(),16);
        std::memcpy(&out.copies[object-1].copied50,after.effect.copied50.data(),16);
        out.objects[owner.queries.ref(0)-1].flagsF8=result.queryFlagsF8;return true;
    }
    bool candidate(Ref object)noexcept {
        ++report.visited;const auto& definition=world.objects[object-1];
        if(definition.typeTag!=*world.collisionTypeGp8010){
            if(pass==1&&!(result.queryFlagsF8&1u)&&definition.typeTag==*world.groupTypeGp8014)
                return fail(S::group,0x121d7cu,object);
            return true;
        }
        currentIdentity=definition.identity;result.currentCandidateIdentity=currentIdentity;
        auto& current=world.queryWorkspace.staging.objects[object-1];current.flagsF8|=0x10u;
        const auto& q=world.queryWorkspace.staging.objects[owner.queries.ref(0)-1];Math m;
        const float gap=m.sub(m.root(m.squared(q.cachedCenter,current.cachedCenter)),m.add(q.cachedRadius,current.cachedRadius));
        if(!m.valid)return fail(S::arithmetic,0x126eb0u,object);
        if(gap>0.f){if(pass==1&&gap<distance)distance=gap;return true;}
        if(pass==1)distance=0.f;
        if(!eligible(object))return report.status==S::complete;
        return dispatch(object);
    }
    bool runPass(unsigned which)noexcept {
        pass=which;link=1;linkSteps=0;
        if(!ref(link,query.membership.storage.capacity))return fail(S::topology,0x294650u);
        if(!select(query.membership.storage.links[link-1].zone,0x294650u)||!open())return false;
        for(;;){Ref object=0;if(!next(object))return false;if(!object)return true;if(!candidate(object))return false;}
    }
    bool run()noexcept {
        auto& q=world.queryWorkspace.staging.objects[owner.queries.ref(0)-1];Math m;
        const float square=m.squared(q.cachedCenter,query.previousCenter70);float moved=0.f;
        if(square!=0.f){volatile float reciprocal=static_cast<float>(1.0/std::sqrt(static_cast<double>(square)));m.keep(reciprocal);moved=m.div(1.f,reciprocal);}
        m.sub(query.distance84,moved);std::memcpy(&previous70,&q.cachedCenter,sizeof previous70);
        if(!m.valid)return fail(S::arithmetic,0x1219a0u);
        // Actuala1=0 always discards the updated distance and enters11DD00.
        result={};std::memcpy(&result.selected,&buffer.result90.selected,sizeof result.selected);
        std::memcpy(&result.selected.first,world.sentinel3E02C0,sizeof result.selected.first);
        std::memcpy(&result.selected.second,world.sentinel3E02C0,sizeof result.selected.second);result.selected.fraction=1.f;
        result.records=owner.physical.spring;result.currentCandidateIdentity=query.currentIdentity94;
        currentIdentity=query.currentIdentity94;result.queryFlagsF8=static_cast<std::uint8_t>((q.flagsF8|0x10u)&~0x20u);
        result.normalModeWrite.bound=true;q.flagsF8=result.queryFlagsF8;
        auto& first=buffer.banks[0];
        std::memcpy(first.pairs.data(),buffer.live.pairs.data(),first.pairs.size()*sizeof(SpringCumulativePair48N));
        std::memcpy(first.metadata.data(),buffer.live.metadata.data(),first.metadata.size()*sizeof(Type2PairMetadata49X));
        std::memcpy(first.records.data(),owner.physical.records.data(),sizeof first.records);
        if(result.records.currentCandidate130){
            bool found=false;
            for(std::size_t i=0;i<world.objectCount;++i)if(world.objects[i].identity==result.records.currentCandidate130){
                currentCategory=world.objects[i].filter.categoryA4;found=true;break;
            }
            if(!found)return fail(S::binding,0x294bf4u);
        }
        Type2BoundsInput50BY input{};input.resource=&owner.assets.queryE0();std::memcpy(&input.preparedWorld,&query.type2World80,sizeof input.preparedWorld);
        std::memcpy(input.cachedCenterWords,&q.cachedCenter,16);std::memcpy(&input.cachedRadiusWord,&q.cachedRadius,4);
        CollectionBoundsResult48S bounds{};
        if(prepareType2Bounds50BY(input,bounds)!=CollectionBoundsStatus48S::Complete)return fail(S::arithmetic,0x1202e0u);
        std::memcpy(&q.cachedCenter,&bounds.cachedCenter,sizeof q.cachedCenter);std::memcpy(&q.cachedRadius,&bounds.cachedRadius,sizeof q.cachedRadius);
        const float left=m.sub(m.sub(bounds.center.v[0],bounds.half.v[0]),0.f),back=m.sub(m.sub(bounds.center.v[2],bounds.half.v[2]),0.f);
        const float right=m.sub(0.f,m.add(bounds.center.v[0],bounds.half.v[0])),front=m.sub(0.f,m.add(bounds.center.v[2],bounds.half.v[2]));
        distance=detail::scalarMin48O(detail::scalarMin48O(detail::scalarMin48O(left,back),right),front);
        if(!m.valid)return fail(S::arithmetic,0x121ae8u);
        if(!runPass(1))return false;
        bool distinct=false;Ref cursor=1;std::size_t steps=0;
        while(cursor){
            if(++steps>query.membership.storage.capacity||!ref(cursor,query.membership.storage.capacity))return fail(S::topology,0x122090u);
            const auto z=query.membership.storage.links[cursor-1].zone;
            if(!ref(z,world.worldQuery.zoneCount))return fail(S::topology,0x122094u);
            distinct=distinct||world.worldQuery.zones[z-1].firstMethod!=world.worldQuery.zones[z-1].secondMethod;
            cursor=query.membership.storage.links[cursor-1].next;
        }
        if(distinct&&!runPass(2))return false;
        // Actual11E330/11E470 are no-ops for shape2, including on a hit.
        if(result.queryFlagsF8&0x20u){previous70={{0.f,-10000.f,0.f,1.f}};distance=0.f;}
        result.currentCandidateIdentity=currentIdentity;return true;
    }
};
} // namespace original_wreck_world_detail50LA

// Actual295A1C->121920(0), bounded to SearchList direct static0/compound1,
// with immediate ordered static294BB0 callbacks. It consumes its authentic
// E0 registration after the detector's11FF60; no explicit candidate list,
// camera relabeling, terrain fallback or caller-supplied miss is accepted.
// Groups, other target shapes, dynamic callbacks and reached E8 remain guards.
// Numerical/world/query/contact writes commit together only on Complete.
// The preceding detector reset/model/registration prefixes remain committed;
// a native failure is terminal for that attempted source call, not a retry.
inline OriginalWreckContactWorldResult50LA queryOriginalWreckContactWorld50LA(
    OriginalOnePieceWreckOwner50LA& owner,std::uint64_t actualUpdatedRevision)noexcept {
    using namespace original_wreck_world_detail50LA;OriginalWreckContactWorldResult50LA out;
    const auto fail=[&](S s,std::uint32_t pc){out.status=s;out.sourcePC=pc;return out;};
    if(!owner.prepared()||!owner.queries.coherent(0)||owner.detector.query38!=&owner.queries.query(0)||
       owner.detector.objects74)return fail(S::owner,0x295a1cu);
    auto& queries=owner.queries;auto& q=queries.query(0);const auto& w=queries.worldBinding();
    // Source121940's disabled branch consumes neitherresult90 nor its world.
    if(!(queries.state(0).flagsF8&2u)){
        queries.state(0).flagsF8&=static_cast<std::uint8_t>(~0x20u);
        out.status=S::complete;out.returnedDistance=std::numeric_limits<float>::max();out.queryCommitted=true;return out;
    }
    const auto& in=w.worldQuery;const auto& m=q.membership;
    if(!q.type2||!w.normalMode3DF5B0||!w.sentinel3E02C0||!w.groupTypeGp8014||
       !w.skipCategory3E0728||!w.physicsBodyCategory3E0718||!in.staticObjectMask3E0720||
       !in.globalFallbackZone443108||!in.methods||!in.zones||!in.methodCount||!in.zoneCount||
       !m.attached||m.object!=queries.ref(0)||m.method!=ZoneMethod50CL::Second||
       !actualUpdatedRevision||m.revision!=actualUpdatedRevision||m.lastShapeTag!=2||
       m.lastType2Resource!=&owner.assets.queryE0()||
       std::memcmp(m.lastShapeWorld.v,q.type2World80.v,sizeof q.type2World80.v)||
       std::memcmp(&m.bounds.cachedCenter,&queries.state(0).cachedCenter,sizeof m.bounds.cachedCenter)||
       std::memcmp(&m.bounds.cachedRadius,&queries.state(0).cachedRadius,sizeof m.bounds.cachedRadius)||
       !m.storage.entries||!m.storage.capacity||!m.storage.entries[0].used||!owner.modelSceneIdentity().value||
       in.objectCount!=w.objectCount||in.objects!=w.objects||in.geometry!=w.geometry||
       w.mutableQuery.objects!=w.states||w.mutableQuery.copies!=w.copies||
       w.mutableQuery.objectCapacity<w.objectCount||w.mutableQuery.copyCapacity<w.objectCount||
       w.mutableQuery.methodCapacity<in.methodCount||w.queryWorkspace.staging.objectCapacity<w.objectCount||
       w.queryWorkspace.staging.copyCapacity<w.objectCount||w.queryWorkspace.staging.methodCapacity<in.methodCount||
       w.type2TargetCount>w.objectCount||*in.globalFallbackZone443108>in.zoneCount)
        return fail(S::binding,0x121920u);
    const auto& registration=w.lifecycle->storage.registrations[queries.ref(0)-1];
    const auto& saved=m.storage.entries[0].registration;
    if(!registration.owner||registration.owner!=saved.owner||registration.slot!=saved.slot||
       registration.encodedObject!=saved.encodedObject||saved.encodedObject!=(queries.ref(0)<<2u))return fail(S::binding,0x121ac4u);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x121920u);
    if(!storage(owner,out))return fail(S::storage,0x121920u);
    const auto rays=owner.assets.queryE0().rayCount;
    if(!rays||w.objectCount>(std::numeric_limits<Ref>::max()>>2u))return fail(S::capacity,0x121920u);
    auto& b=*q.type2;
    for(const auto* bank:{&b.live,&b.banks[0],&b.banks[1],&b.scratch})
        if(bank->pairs.size()!=rays||bank->metadata.size()!=rays)return fail(S::capacity,0x11dd00u);
    for(std::size_t i=0;i<w.objectCount;++i){
        if(!w.objects[i].identity)return fail(S::binding,0x120e70u);
        for(std::size_t j=0;j<i;++j)if(w.objects[i].identity==w.objects[j].identity)return fail(S::topology,0x120e70u);
    }
    auto& stage=w.queryWorkspace.staging;
    std::memcpy(stage.objects,w.states,w.objectCount*sizeof(*w.states));
    std::memcpy(stage.methods,w.mutableQuery.methods,in.methodCount*sizeof(*stage.methods));
    std::memcpy(stage.copies,w.copies,w.objectCount*sizeof(*w.copies));
    out.status=S::complete;Transaction transaction{owner,out,w,q,b};
    if(!transaction.run()){
        if(out.status==S::complete)return fail(S::topology,0x121920u);
        return out;
    }
    auto& final=b.banks[transaction.bank];std::memcpy(&b.result90,&transaction.result,sizeof b.result90);
    std::memcpy(b.live.pairs.data(),final.pairs.data(),rays*sizeof(SpringCumulativePair48N));
    std::memcpy(b.live.metadata.data(),final.metadata.data(),rays*sizeof(Type2PairMetadata49X));
    owner.physical.spring=transaction.result.records;
    std::memcpy(owner.physical.records.data(),final.records.data(),sizeof final.records);
    std::memcpy(w.states,stage.objects,w.objectCount*sizeof(*w.states));
    std::memcpy(w.mutableQuery.methods,stage.methods,in.methodCount*sizeof(*stage.methods));
    std::memcpy(w.copies,stage.copies,w.objectCount*sizeof(*w.copies));
    std::memcpy(&q.previousCenter70,&transaction.previous70,sizeof q.previousCenter70);
    q.distance84=transaction.distance;q.currentIdentity94=transaction.currentIdentity;
    w.normalMode3DF5B0->acceptCompletedWorldReset();
    // bound=true is established before traversal, even when no dispatch ran.
    w.normalMode3DF5B0->acceptCompleted(transaction.result.normalModeWrite);
    out.returnedDistance=transaction.distance;out.queryCommitted=true;out.sourcePC=0x122324u;return out;
}
} // namespace rr::game

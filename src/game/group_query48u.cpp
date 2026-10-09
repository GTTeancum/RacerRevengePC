#include "group_query48u.h"
#include "type3_query_internal48u.h"
#include "type3_prepare48p.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Status=Type3QueryStatus48P;
using Ref=GroupQueryRef48U;
using V=Vector3_48C;
struct Span {std::uintptr_t begin=0,end=0;};
bool overlap(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
template<class T>bool span(const T* p,std::size_t n,Span& out) noexcept {
    out={};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    out={begin,begin+bytes};return true;
}
struct Guard {
    Span writes[28]{};std::size_t count=0;
    template<class T>bool write(T* p,std::size_t n) noexcept {
        Span s{};if(!span(p,n,s)||count==28)return false;
        for(std::size_t i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        writes[count++]=s;return true;
    }
    template<class T>bool read(const T* p,std::size_t n) const noexcept {
        Span s{};if(!span(p,n,s))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        return true;
    }
    bool filter(const CollisionFilter48J& f) const noexcept {
        return read(f.categoryMasks.data(),f.categoryMasks.size())&&
            read(f.identityLists[0].identities.data(),f.identityLists[0].identities.size())&&
            read(f.identityLists[1].identities.data(),f.identityLists[1].identities.size());
    }
    bool mesh(const assets::CollisionMesh48I* m) const noexcept {
        return !m||(read(m,1)&&read(m->nodes.data(),m->nodes.size())&&read(m->faces.data(),m->faces.size())&&
            read(m->vertices.data(),m->vertices.size())&&read(m->vertex_normals.data(),m->vertex_normals.size()));
    }
    bool writeRays(const Type3RayBuffers48P& b) noexcept {
        return write(b.localStarts,b.capacity)&&write(b.localDirections,b.capacity)&&
            write(b.worldStarts,b.capacity)&&write(b.worldDirections,b.capacity)&&
            write(b.normals,b.capacity)&&write(b.points,b.capacity)&&write(b.hits,b.capacity)&&
            write(b.fractions,b.capacity)&&write(b.lengths,b.lengthCapacity);
    }
    bool readRays(const Type3RaySource48P& b) const noexcept {
        return read(b.localStarts,b.capacity)&&read(b.localDirections,b.capacity)&&
            read(b.worldStarts,b.capacity)&&read(b.worldDirections,b.capacity)&&
            read(b.normals,b.capacity)&&read(b.points,b.capacity)&&read(b.hits,b.capacity)&&
            read(b.fractions,b.capacity)&&read(b.lengths,b.lengthCapacity);
    }
};
template<class T>void copy(T* d,const T* s,std::size_t n) noexcept {if(n)std::memcpy(d,s,n*sizeof(T));}
void copyRays(const Type3RaySource48P& s,const Type3RayBuffers48P& d) noexcept {
    copy(d.localStarts,s.localStarts,s.capacity);copy(d.localDirections,s.localDirections,s.capacity);
    copy(d.worldStarts,s.worldStarts,s.capacity);copy(d.worldDirections,s.worldDirections,s.capacity);
    copy(d.normals,s.normals,s.capacity);copy(d.points,s.points,s.capacity);
    copy(d.hits,s.hits,s.capacity);copy(d.fractions,s.fractions,s.capacity);copy(d.lengths,s.lengths,s.lengthCapacity);
}
Type3RaySource48P readRays(const Type3RayBuffers48P& b,std::size_t n,std::size_t lengths) noexcept {
    return {b.localStarts,b.localDirections,b.worldStarts,b.worldDirections,b.normals,b.points,b.hits,b.fractions,n,b.lengths,lengths};
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    void operands(float a,float b) noexcept {valid=valid&&std::isfinite(a)&&std::isfinite(b);}
    float add(float a,float b) noexcept {operands(a,b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {operands(a,b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {operands(a,b);volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {operands(a,b);volatile float r=a/b;return keep(r);}
    float root(float a) noexcept {operands(a,a);volatile float r=std::sqrt(a);return keep(r);}
};
CollisionSphere48J sphere(const GroupQueryObjectState48U& s) noexcept {
    return {{{s.center.v[0],s.center.v[1],s.center.v[2]}},s.radius};
}
bool ref(Ref r,std::size_t count) noexcept {return r&&static_cast<std::size_t>(r)<=count;}
struct Run {
    const Type3QueryInput48P& input;
    const Type3QueryContext48P& context;
    const Type3QuerySource48P& source;
    const GroupQueryInput48U& groups;
    const GroupQueryWorkspace48U& workspace;
    Type3QueryResult48P result{};
    std::uint32_t springCategory=0;
    GroupQueryBuffers48U b;
    void syncQuery() noexcept {
        if(!groups.queryObject)return;
        auto& q=b.objects[groups.queryObject-1];
        q.center=result.state.queryCenter;q.radius=result.state.queryRadius;
        q.flagsF8=result.queryFlags;q.copied40=input.copied40;q.copied50=input.copied50;
    }
    Status callback(const Type3Candidate48P& candidate) noexcept {
        if(groups.queryCallback==GroupQueryCallback48U::None)return Status::Complete;
        if(groups.queryCallback!=GroupQueryCallback48U::Static294BB0)return Status::UnsupportedScope;
        ++result.callbacks;
        const auto& c=candidate.collision;
        const SpringStaticType3CallbackInput48P in{context.bodyScene,context.bodyScenePresent,c.filter.identity,
            c.filter.categoryA4,springCategory,input.skipMask,input.dynamicMask,input.materialMask,3,
            candidate.hasMaterialScene,candidate.sceneName,candidate.sceneNameBytes};
        const SpringStaticType3CallbackSource48P from{
            b.rays.worldStarts,source.rays.capacity,b.rays.worldDirections,source.rays.capacity,
            b.rays.lengths,source.rays.lengthCapacity,b.rays.normals,source.rays.capacity,
            b.rays.points,source.rays.capacity,b.rays.hits,source.rays.capacity,
            source.sceneMetadata,source.sceneMetadataCount};
        const auto previous=result.spring.currentCandidate130;SpringStaticCallbackResult48N appended{};
        const auto called=applySpringStaticType3Callback48P(in,from,b.records,source.recordCount,result.spring,appended);
        if(called==SpringCallbackStatus48N::InvalidInput)return Status::InvalidInput;
        if(called==SpringCallbackStatus48N::InsufficientCapacity)return Status::InsufficientCapacity;
        if(called==SpringCallbackStatus48N::UnsupportedScope)return Status::UnsupportedScope;
        if(called==SpringCallbackStatus48N::Complete){
            result.appended+=appended.appended;result.saturated+=appended.saturated;
            if(!previous||(springCategory&input.dynamicMask))springCategory=c.filter.categoryA4;
        }
        return Status::Complete;
    }
    Status dispatch(Ref r) noexcept {
        const auto& object=groups.objects[r-1];const auto& candidate=*object.candidate;
        const auto& c=candidate.collision;auto& state=b.objects[r-1];
        const assets::CollisionMesh48I* mesh=c.geometry.mesh;const BodyMatrix48G* matrix=&c.geometry.worldMatrix;
        if(candidate.shapeType==0&&object.shape0){
            if(!ref(object.shape0,groups.shapeCount))return Status::InvalidInput;
            mesh=groups.shapes[object.shape0-1].mesh;matrix=&b.shapes[object.shape0-1].composed00;
        }
        bool hit=false;
        const auto status=detail::dispatchType3Borrowed48U(input,context,candidate,sphere(state),mesh,*matrix,object.hasShape,
            b.rays,workspace.treeStack,workspace.treeStackCapacity,result.state,hit);
        if(status!=Status::Complete||!hit)return status;
        result.hasAnyHit=true;++result.geometryHits;
        result.queryFlags=static_cast<std::uint8_t>(result.queryFlags|0x20u);syncQuery();
        state.copied40=input.copied40;state.copied50=input.copied50;
        const auto called=callback(candidate);if(called!=Status::Complete)return called;
        if(c.hasResponseCallbackE8)return Status::UnsupportedScope;
        return Status::Complete;
    }
    void current(Ref r) noexcept {
        result.currentCandidate=groups.objects[r-1].candidate->collision.filter.identity;
        b.objects[r-1].flagsF8=static_cast<std::uint8_t>(b.objects[r-1].flagsF8|0x10u);
        if(r==groups.queryObject){result.queryFlags=b.objects[r-1].flagsF8;syncQuery();}
    }
    Status member(Ref owner,Ref r,const GroupQueryGroup48U& group) noexcept {
        if(!ref(r,groups.objectCount))return Status::InvalidInput;
        const auto& outer=groups.objects[owner-1];const auto& object=groups.objects[r-1];
        const auto& candidate=*object.candidate;
        if(!object.hasShape)return Status::UnsupportedScope;
        Math m;
        const V translation{{m.mul(static_cast<float>(outer.position[0]),groups.worldPositionScale),
            m.mul(static_cast<float>(outer.position[1]),groups.worldPositionScale),
            m.mul(static_cast<float>(outer.position[2]),groups.worldPositionScale)}};
        // Both ratios are reached even on the tag0 and nonzero no-op paths.
        const float scale18C=m.div(m.add(group.base18C,m.div(m.mul(static_cast<float>(outer.scale11),
            m.sub(group.other194,group.base18C)),128.f)),group.base18C);
        const float scale180=m.div(m.add(group.base180,m.div(m.mul(static_cast<float>(outer.scale10),
            m.sub(group.base180,group.other184)),128.f)),group.base180);
        if(!m.valid)return Status::InvalidInput;
        if(candidate.shapeType==0){
            if(!object.shape0)return Status::UnsupportedScope;
            if(!ref(object.shape0,groups.shapeCount))return Status::InvalidInput;
            const auto index=object.shape0-1;const auto& def=groups.shapes[index];
            if(!def.mesh||def.mesh->nodes.empty())return Status::UnsupportedScope;
            const auto& node=def.mesh->nodes[0];
            const GroupShapeGeometry48T geometry{{{node.center[0],node.center[1],node.center[2]}},
                {{node.half[0],node.half[1],node.half[2]}}};
            const GroupShapeSource48T shape{def.local40,&geometry};
            GroupShapeState48T next{};GroupShapeSphere48T bound{};
            const auto updated=setGroupShape0Bounded48T(shape,translation,b.shapes[index],next,bound);
            if(updated!=GroupShapeStatus48T::Complete)return Status::InvalidInput;
            b.shapes[index]=next;b.objects[r-1].center=bound.center;b.objects[r-1].radius=bound.radius;
        }else if(candidate.shapeType==4||candidate.shapeType==5){
            if(!object.shape45)return Status::UnsupportedScope;
            if(!ref(object.shape45,groups.shape45Count))return Status::InvalidInput;
            b.shapes45[object.shape45-1]={translation,scale18C,scale180};
        }
        current(r);return dispatch(r);
    }
    Status group(Ref r) noexcept {
        const auto& object=groups.objects[r-1];Math m;
        const float x=m.mul(static_cast<float>(object.position[0]),groups.worldPositionScale);
        const float z=m.mul(static_cast<float>(object.position[2]),groups.worldPositionScale);
        if(!m.valid)return Status::InvalidInput;
        if(!ref(object.group,groups.groupCount))return Status::InvalidInput;
        const auto& g=groups.groups[object.group-1];
        const float radius=m.add(g.base18C,m.div(m.mul(static_cast<float>(object.scale11),m.sub(g.other194,g.base18C)),128.f));
        const float dx=m.sub(result.state.queryCenter.v[0],x),dz=m.sub(result.state.queryCenter.v[2],z);
        const float gap=m.sub(m.root(m.add(m.mul(dx,dx),m.mul(dz,dz))),m.add(result.state.queryRadius,radius));
        if(!m.valid)return Status::InvalidInput;
        if(!(gap<=0.f)||g.memberCount<=0)return Status::Complete;
        if(static_cast<std::size_t>(g.memberCount)>g.memberCapacity)return Status::InsufficientCapacity;
        for(std::int32_t i=0;i<g.memberCount;++i){
            const auto status=member(r,g.members[i],g);if(status!=Status::Complete)return status;
        }
        return Status::Complete;
    }
    Status run() noexcept {
        result.state=input.initial;result.spring=context.spring;
        result.currentCandidate=input.initialQueryCurrent;result.queryFlags=input.filter.flagsF8;
        result.candidateCount=groups.outerCount;springCategory=input.initialSpringCurrentCategory;
        if(context.resetSpring)resetSpringRecords48N(result.spring);
        const Type3PrepareInput48P preparation{context.pairCount,source.endpoints,source.endpointCount,
            source.sceneMetadata,source.sceneMetadataCount,input.sentinel};
        const auto prepared=prepareType3Rays48P(preparation,b.rays,result.state);
        if(prepared==Type3PrepareStatus48P::UnsupportedScope)return Status::UnsupportedScope;
        if(prepared==Type3PrepareStatus48P::CapacityExceeded)return Status::InsufficientCapacity;
        if(prepared!=Type3PrepareStatus48P::Complete)return Status::InvalidInput;
        result.queryFlags=static_cast<std::uint8_t>((input.filter.flagsF8|0x10u)&~0x20u);syncQuery();
        for(std::size_t i=0;i<groups.outerCount;++i){
            const auto r=groups.outerOrder[i];if(!ref(r,groups.objectCount))return Status::InvalidInput;
            const auto& c=groups.objects[r-1].candidate->collision;
            if(c.typeTag==input.collisionTypeTag){
                current(r);bool hit=false;
                const auto bounded=detail::outerSphereType3Borrowed48U(result.state,sphere(b.objects[r-1]),hit);
                if(bounded!=Status::Complete)return bounded;
                if(!hit||!detail::eligibleType3Borrowed48U(input.filter,c.filter,b.objects[r-1].flagsF8))continue;
                const auto queried=dispatch(r);if(queried!=Status::Complete)return queried;
            }else if(!(result.queryFlags&1u)&&c.typeTag==input.groupTypeTag){
                const auto queried=group(r);if(queried!=Status::Complete)return queried;
            }
        }
        return Status::Complete;
    }
};
}

Type3QueryStatus48P queryStaticType3GroupsBounded48U(
    const Type3QueryInput48P& input,const Type3QueryContext48P& context,const Type3QuerySource48P& source,
    const GroupQueryInput48U& groups,const GroupQueryState48U& initial,
    const GroupQueryWorkspace48U& workspace,const GroupQueryBuffers48U& destination,
    Type3QueryResult48P& output) noexcept {
    if(context.entry!=Type3QueryEntry48P::LimitedList||!context.pairCount||
       context.pairCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()/2)||
       groups.outerCount>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
       groups.objectCount>std::numeric_limits<Ref>::max()||groups.groupCount>std::numeric_limits<Ref>::max()||
       groups.shapeCount>std::numeric_limits<Ref>::max()||groups.shape45Count>std::numeric_limits<Ref>::max()||
       (groups.queryCallback!=GroupQueryCallback48U::None&&groups.queryCallback!=GroupQueryCallback48U::Static294BB0&&
        groups.queryCallback!=GroupQueryCallback48U::Unknown))return Status::InvalidInput;
    const auto n=static_cast<std::size_t>(context.pairCount);const auto& b=workspace.buffers;
    if(source.rays.capacity<n||source.rays.lengthCapacity<n||source.endpointCount<n*2||source.sceneMetadataCount<n*2||
       b.rays.capacity<source.rays.capacity||destination.rays.capacity<source.rays.capacity||
       b.rays.lengthCapacity<source.rays.lengthCapacity||destination.rays.lengthCapacity<source.rays.lengthCapacity||
       b.recordCapacity<source.recordCount||destination.recordCapacity<source.recordCount||
       initial.objectCapacity<groups.objectCount||b.objectCapacity<groups.objectCount||destination.objectCapacity<groups.objectCount||
       initial.shapeCapacity<groups.shapeCount||b.shapeCapacity<groups.shapeCount||destination.shapeCapacity<groups.shapeCount||
       initial.shape45Capacity<groups.shape45Count||b.shape45Capacity<groups.shape45Count||destination.shape45Capacity<groups.shape45Count)
        return Status::InsufficientCapacity;
    Guard guard;
    if(!guard.writeRays(b.rays)||!guard.writeRays(destination.rays)||
       !guard.write(b.records,b.recordCapacity)||!guard.write(destination.records,destination.recordCapacity)||
       !guard.write(b.objects,b.objectCapacity)||!guard.write(destination.objects,destination.objectCapacity)||
       !guard.write(b.shapes,b.shapeCapacity)||!guard.write(destination.shapes,destination.shapeCapacity)||
       !guard.write(b.shapes45,b.shape45Capacity)||!guard.write(destination.shapes45,destination.shape45Capacity)||
       !guard.write(workspace.treeStack,workspace.treeStackCapacity)||!guard.write(&output,1))return Status::InvalidInput;
    if(!guard.read(&input,1)||!guard.read(&context,1)||!guard.read(&source,1)||!guard.read(&groups,1)||
       !guard.read(&initial,1)||!guard.read(&workspace,1)||!guard.read(&destination,1)||
       !guard.read(groups.objects,groups.objectCount)||!guard.read(groups.groups,groups.groupCount)||
       !guard.read(groups.shapes,groups.shapeCount)||!guard.read(groups.shape45Tags,groups.shape45Count)||
       !guard.read(groups.outerOrder,groups.outerCount)||!guard.read(workspace.identityOrder,workspace.identityOrderCount)||
       !guard.read(initial.objects,initial.objectCapacity)||!guard.read(initial.shapes,initial.shapeCapacity)||
       !guard.read(initial.shapes45,initial.shape45Capacity)||!guard.readRays(source.rays)||
       !guard.read(source.endpoints,source.endpointCount)||!guard.read(source.sceneMetadata,source.sceneMetadataCount)||
       !guard.read(source.records,source.recordCount)||!guard.filter(input.filter))return Status::InvalidInput;
    // Validate all nested spans, including unvisited objects/groups/shapes.
    for(std::size_t i=0;i<groups.objectCount;++i){
        const auto* p=groups.objects[i].candidate;
        if(!guard.read(p,1)||!guard.filter(p->collision.filter)||!guard.read(p->sceneName,p->sceneNameBytes)||
           !guard.mesh(p->collision.geometry.mesh))return Status::InvalidInput;
        if(p->compound){
            if(!guard.read(p->compound,1)||p->compound->count>maxCompoundMembers50IJ||
               !guard.read(p->compound->parts,p->compound->count))return Status::InvalidInput;
            for(std::size_t j=0;j<p->compound->count;++j){const auto& part=p->compound->parts[j];
                if(!guard.mesh(part.mesh)||!guard.read(part.nodeRadii,part.nodeRadiusCount))return Status::InvalidInput;
            }
        }
    }
    for(std::size_t i=0;i<groups.groupCount;++i)
        if(!guard.read(groups.groups[i].members,groups.groups[i].memberCapacity))return Status::InvalidInput;
    for(std::size_t i=0;i<groups.shapeCount;++i)if(!guard.mesh(groups.shapes[i].mesh))return Status::InvalidInput;
    if(groups.queryObject&&!ref(groups.queryObject,groups.objectCount))return Status::InvalidInput;
    const bool ordered=workspace.identityOrder||workspace.identityOrderCount;
    if(ordered){
        if(workspace.identityOrderCount!=groups.objectCount)return Status::InvalidInput;
        std::uint32_t previous=0;
        for(std::size_t i=0;i<workspace.identityOrderCount;++i){
            const auto r=workspace.identityOrder[i];if(!ref(r,groups.objectCount))return Status::InvalidInput;
            const auto identity=groups.objects[r-1].candidate->collision.filter.identity;
            if(identity<=previous)return Status::InvalidInput;
            previous=identity;
        }
    }
    for(std::size_t i=0;i<groups.objectCount;++i){
        const auto& object=groups.objects[i];const auto& candidate=*object.candidate;
        const auto identity=candidate.collision.filter.identity;
        if(!identity)return Status::InvalidInput;
        if(identity==input.filter.identity&&groups.queryObject!=i+1)return Status::InvalidInput;
        if(groups.queryObject==i+1&&(identity!=input.filter.identity||candidate.shapeType!=3||
           candidate.collision.filter.flags31!=input.filter.flags31||!object.hasShape))return Status::InvalidInput;
        if(candidate.shapeType==0&&object.shape0&&!ref(object.shape0,groups.shapeCount))return Status::InvalidInput;
        if((candidate.shapeType==4||candidate.shapeType==5)&&object.shape45){
            if(!ref(object.shape45,groups.shape45Count))return Status::InvalidInput;
            if(groups.shape45Tags[object.shape45-1]!=candidate.shapeType)return Status::UnsupportedScope;
        }
        if(!ordered)for(std::size_t j=0;j<i;++j)
            if(identity==groups.objects[j].candidate->collision.filter.identity)return Status::InvalidInput;
    }
    for(std::size_t i=0;i<n*2;++i)if(source.sceneMetadata[i])return Status::UnsupportedScope;
    copyRays(source.rays,b.rays);copy(b.records,source.records,source.recordCount);
    copy(b.objects,initial.objects,groups.objectCount);copy(b.shapes,initial.shapes,groups.shapeCount);
    copy(b.shapes45,initial.shapes45,groups.shape45Count);
    Run run{input,context,source,groups,workspace,{},0,b};
    const auto status=run.run();if(status!=Status::Complete)return status;
    copyRays(readRays(b.rays,source.rays.capacity,source.rays.lengthCapacity),destination.rays);
    copy(destination.records,b.records,source.recordCount);copy(destination.objects,b.objects,groups.objectCount);
    copy(destination.shapes,b.shapes,groups.shapeCount);copy(destination.shapes45,b.shapes45,groups.shape45Count);
    output=run.result;return Status::Complete;
}
}

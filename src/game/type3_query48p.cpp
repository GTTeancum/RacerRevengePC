#include "type3_query48p.h"
#include "type3_prepare48p.h"
#include "collision_type3_node48p.h"
#include "type3_query_internal48u.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Status=Type3QueryStatus48P;
using V=Vector3_48C;
using V4=SpringVector4_48N;
struct Span {std::uintptr_t begin,end;};
bool overlap(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
template<class T>bool span(const T* p,std::size_t n,Span& r) noexcept {
    r={0,0};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;
    r={a,a+bytes};return true;
}
template<class T>void copy(T* d,const T* s,std::size_t n) noexcept {if(n)std::memcpy(d,s,n*sizeof(T));}
bool finite(const V& v) noexcept {for(float x:v.v)if(!std::isfinite(x))return false;return true;}
V xyz(const V4& v) noexcept {return {{v.v[0],v.v[1],v.v[2]}};}
V vector(const std::array<float,3>& v) noexcept {return {{v[0],v[1],v[2]}};}
V4 quad(const V& v,float w=1.f) noexcept {return {{v.v[0],v.v[1],v.v[2],w}};}
struct Math {
    bool valid=true;
    float keep(float v) noexcept {valid=valid&&std::isfinite(v);return v;}
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
    V transform(const BodyMatrix48G& m,const V& v,bool point,bool addZero=false) noexcept {
        V r{};for(unsigned k=0;k<3;++k){
            r.v[k]=add(add(mul(m.v[k],v.v[0]),mul(m.v[4+k],v.v[1])),mul(m.v[8+k],v.v[2]));
            if(point)r.v[k]=add(r.v[k],m.v[12+k]);
            else if(addZero)r.v[k]=add(r.v[k],0.f);
        }return r;
    }
    BodyMatrix48G inverse(const BodyMatrix48G& m) noexcept {
        BodyMatrix48G b{};
        for(unsigned c=0;c<3;++c)for(unsigned r=0;r<3;++r)b.v[c*4+r]=add(m.v[r*4+c],0.f);
        const V t{{m.v[12],m.v[13],m.v[14]}};
        const auto rotated=transform(b,t,false);
        for(unsigned i=0;i<3;++i)b.v[12+i]=mul(rotated.v[i],-1.f);
        b.v[15]=1.f;return b;
    }
};
bool identities(const CollisionIdentityList48J& list,std::uint32_t id) noexcept {
    if(list.identities.empty())return true;
    const bool found=std::find(list.identities.begin(),list.identities.end(),id)!=list.identities.end();
    return !((found&&list.mode==1)||(!found&&list.mode==0));
}
bool categories(const CollisionFilter48J& filter,std::uint32_t category) noexcept {
    bool found=false;for(auto mask:filter.categoryMasks)if(mask&category){found=true;break;}
    return filter.categoryModeD4==1?!found:found;
}
bool eligible(const CollisionFilter48J& q,const CollisionFilter48J& c,std::uint8_t flagsF8) noexcept {
    if(q.identity==c.identity||!(flagsF8&4u)||!(c.flags31&1u)||!(c.categoryA4&q.acceptMaskB8))return false;
    for(unsigned i=0;i<2;++i)if(!identities(q.identityLists[i],c.identity)||!identities(c.identityLists[i],q.identity))return false;
    return categories(q,c.categoryA4)&&categories(c,q.categoryA4);
}
bool finiteSphere(const CollisionSphere48J& s) noexcept {return finite(s.center)&&std::isfinite(s.radius)&&s.radius>=0.f;}
Status outerSphere(const Type3QueryState48P& q,const CollisionSphere48J& c,bool& hit) noexcept {
    if(!finite(xyz(q.queryCenter))||!std::isfinite(q.queryRadius)||q.queryRadius<0||!finiteSphere(c))return Status::InvalidInput;
    Math m;float d[3];for(unsigned k=0;k<3;++k)d[k]=m.sub(q.queryCenter.v[k],c.center.v[k]);
    const float square=m.add(m.add(m.mul(d[0],d[0]),m.mul(d[1],d[1])),m.mul(d[2],d[2]));
    volatile float distance=std::sqrt(square);
    const float gap=m.sub(distance,m.add(q.queryRadius,c.radius));
    if(!m.valid)return Status::InvalidInput;
    hit=gap<=0.f;return Status::Complete;
}
void copyRays(const Type3RaySource48P& s,const Type3RayBuffers48P& d) noexcept {
    copy(d.localStarts,s.localStarts,s.capacity);copy(d.localDirections,s.localDirections,s.capacity);
    copy(d.worldStarts,s.worldStarts,s.capacity);copy(d.worldDirections,s.worldDirections,s.capacity);
    copy(d.normals,s.normals,s.capacity);copy(d.points,s.points,s.capacity);
    copy(d.hits,s.hits,s.capacity);copy(d.fractions,s.fractions,s.capacity);copy(d.lengths,s.lengths,s.lengthCapacity);
}
Type3RaySource48P readRays(const Type3RayBuffers48P& b,std::size_t count,std::size_t lengths) noexcept {
    return {b.localStarts,b.localDirections,b.worldStarts,b.worldDirections,b.normals,b.points,b.hits,b.fractions,count,b.lengths,lengths};
}
Type3Segment48P segment(const Type3RayBuffers48P& b,std::size_t i) noexcept {
    return {quad(b.localStarts[i]),quad(b.localDirections[i]),b.lengths[i]};
}
Status dispatchMesh50IJ(const Type3QueryInput48P& input,const Type3QueryContext48P& context,
    const assets::CollisionMesh48I* mesh,const BodyMatrix48G& matrix,bool updateCenter,bool identityValid,
    const Type3RayBuffers48P& rays,std::uint32_t* stack,std::size_t stackCapacity,
    Type3QueryState48P& state,bool& hit,detail::Type3VectorScratch48U* scratch=nullptr) noexcept {
    const auto count=static_cast<std::size_t>(context.pairCount);Math math;
    if(!mesh||mesh->nodes.empty()||mesh->faces.empty()||mesh->vertices.empty()||
       (!mesh->vertex_normals.empty()&&mesh->vertex_normals.size()!=mesh->vertices.size()))return Status::InvalidInput;
    if(mesh->reverse_mode)return Status::UnsupportedScope;
    if(!identityValid)return Status::InvalidInput;
    for(unsigned col=0;col<4;++col)for(unsigned axis=0;axis<3;++axis)
        if(!std::isfinite(matrix.v[col*4+axis]))return Status::InvalidInput;
    const auto inverse=math.inverse(matrix);
    for(std::size_t i=0;i<count;++i){
        if(!finite(xyz(rays.worldStarts[i]))||!finite(xyz(rays.worldDirections[i]))||!std::isfinite(rays.lengths[i]))return Status::InvalidInput;
        rays.localDirections[i]=math.transform(inverse,xyz(rays.worldDirections[i]),false,true);
        rays.localStarts[i]=math.transform(inverse,xyz(rays.worldStarts[i]),true);
    }
    if(updateCenter)state.localCenter=math.transform(inverse,xyz(state.worldCenter),true);
    else if(!finite(state.localCenter))return Status::InvalidInput;
    if(!math.valid)return Status::InvalidInput;
    //1271D0/12721C (compound1274C4 local-start counterpart): every reached
    //point transform resetsVF9.W=1. The admitted miss tree/triangle helpers
    //use onlyVF4..6, preserving it until the result transforms below.
    if(scratch&&(count||updateCenter)){scratch->vf9W=1.f;scratch->written=true;}
    if(!stackCapacity)return Status::InsufficientCapacity;
    if(mesh->nodes.size()>std::numeric_limits<std::size_t>::max()-mesh->faces.size())return Status::InvalidInput;
    const auto visitLimit=mesh->nodes.size()+mesh->faces.size();std::size_t used=1,visits=0;stack[0]=0;
    constexpr auto leaf=assets::collision_mesh_leaf48i;
    while(used){
        if(++visits>visitLimit)return Status::InvalidInput; // Imported topology is a tree, not an unbounded/cyclic graph.
        const auto tag=stack[--used];if(tag&~(leaf|0x7fffu))return Status::InvalidInput;
        const auto index=tag&0x7fffu;
        if(tag&leaf){
            if(index>=mesh->faces.size())return Status::InvalidInput;
            const auto& face=mesh->faces[index];Type3Triangle48P triangle{};
            triangle.hasVertexNormals=!mesh->vertex_normals.empty();triangle.normalMode=input.normalMode;
            for(unsigned k=0;k<3;++k){
                const auto v=face.vertices[k];if(v>=mesh->vertices.size())return Status::InvalidInput;
                triangle.vertices[k]=vector(mesh->vertices[v]);
                if(triangle.hasVertexNormals)triangle.vertexNormals[k]=vector(mesh->vertex_normals[v]);
            }
            for(std::size_t i=0;i<count;++i){
                Type3PairLeafState48P pair{rays.fractions[i],rays.normals[i],rays.points[i],rays.hits[i]};
                Type3SelectedLeafState48P selected{state.selectedFraction,state.selectedNormal,state.selectedPoint,state.selectedCorrection,state.hitCount};
                const auto status=applyType3Triangle48P(segment(rays,i),triangle,pair,selected);
                if(status==Type3LeafStatus48P::InvalidInput)return Status::InvalidInput;
                if(status==Type3LeafStatus48P::UnsupportedNormalMode)return Status::UnsupportedScope;
                if(status==Type3LeafStatus48P::UnsupportedInterpolationBoundary)return Status::UnsupportedInterpolationBoundary;
                if(status==Type3LeafStatus48P::Hit){
                    rays.fractions[i]=pair.fraction;rays.normals[i]=pair.normal;rays.points[i]=pair.point;rays.hits[i]=pair.hit;
                    state.selectedFraction=selected.fraction;state.selectedNormal=selected.normal;
                    state.selectedPoint=selected.point;state.selectedCorrection=selected.correction;state.hitCount=selected.hitCount;
                }
            }
        }else{
            if(index>=mesh->nodes.size())return Status::InvalidInput;
            const auto& node=mesh->nodes[index];const auto center=vector(node.center),half=vector(node.half);
            const auto sphere=type3SphereBox48P(state.localCenter,state.radius,center,half);
            if(sphere==Type3NodeStatus48P::InvalidInput)return Status::InvalidInput;
            if(sphere==Type3NodeStatus48P::Separated)continue;
            bool overlaps=false;
            for(std::size_t i=0;i<count;++i){
                const auto ray=type3RayBox48P(segment(rays,i),center,half);
                if(ray==Type3NodeStatus48P::InvalidInput)return Status::InvalidInput;
                if(ray==Type3NodeStatus48P::Overlap){overlaps=true;break;}
            }
            if(!overlaps)continue;
            if(stackCapacity-used<2)return Status::InsufficientCapacity;
            stack[used++]=node.children[1];stack[used++]=node.children[0];
        }
    }
    for(std::size_t i=0;i<count;++i)if(rays.hits[i]){
        hit=true;rays.points[i]=quad(math.transform(matrix,xyz(rays.points[i]),true),1.f);
        rays.normals[i]=quad(math.transform(matrix,xyz(rays.normals[i]),false,true),2.f);
    }
    if(hit){
        state.selectedPoint=quad(math.transform(matrix,xyz(state.selectedPoint),true),1.f);
        state.selectedNormal=quad(math.transform(matrix,xyz(state.selectedNormal),false,true),2.f);
        state.selectedCorrection=quad(math.transform(matrix,xyz(state.selectedCorrection),false,true),3.f);
        //12731C resets1;127360 and127394 increment to3. Compound1275D4,
        //127618,12764C repeat the same sequence, even after an earlier member
        //hit when this member contributes none (the source hit is cumulative).
        if(scratch){scratch->vf9W=3.f;scratch->written=true;}
    }
    return math.valid?Status::Complete:Status::InvalidInput;
}
Status dispatchBorrowed(const Type3QueryInput48P& input,const Type3QueryContext48P& context,const Type3Candidate48P& candidate,
    const CollisionSphere48J& sphere,const assets::CollisionMesh48I* mesh,const BodyMatrix48G& matrix,bool shapePresent,
    const Type3RayBuffers48P& rays,std::uint32_t* stack,std::size_t stackCapacity,
    Type3QueryState48P& state,bool& hit,detail::Type3VectorScratch48U* scratch=nullptr) noexcept {
    hit=false;const auto count=static_cast<std::size_t>(context.pairCount);const auto& c=candidate.collision;
    for(std::size_t i=0;i<count;++i)rays.hits[i]=0;
    if(!(c.filter.flags31&1u))return Status::Complete;
    if(!finite(xyz(state.worldCenter))||!std::isfinite(state.radius)||state.radius<0||!finiteSphere(sphere))return Status::InvalidInput;
    Math math;float d[3];for(unsigned k=0;k<3;++k)d[k]=math.sub(state.worldCenter.v[k],sphere.center.v[k]);
    const float radius=math.add(sphere.radius,state.radius),squareRadius=math.mul(radius,radius);
    const float square=math.add(math.add(math.mul(d[0],d[0]),math.mul(d[1],d[1])),math.mul(d[2],d[2]));
    if(!math.valid)return Status::InvalidInput;
    if(!(square<squareRadius))return Status::Complete;
    state.selectedFraction=1.f;state.hitCount=0;
    for(std::size_t i=0;i<count;++i)rays.fractions[i]=-1.f;
    if(candidate.shapeType!=0&&candidate.shapeType!=1)return Status::Complete;
    if(!shapePresent)return Status::UnsupportedScope;
    if(candidate.shapeType==0)return dispatchMesh50IJ(input,context,mesh,matrix,true,c.geometry.id==c.filter.identity,rays,stack,stackCapacity,state,hit,scratch);
    if(!candidate.compound)return Status::UnsupportedScope;
    const auto& compound=*candidate.compound;
    if(compound.count>maxCompoundMembers50IJ||(compound.count&&!compound.parts))return Status::InvalidInput;
    //1273A8..127668: no active gate and no recomputation of shape.localCenter.
    // All flags/fractions and cumulative hit survive between ordered members.
    for(std::size_t i=0;i<compound.count;++i){
        const auto& part=compound.parts[i];
        const auto status=dispatchMesh50IJ(input,context,part.mesh,part.worldMatrix,false,c.geometry.id==c.filter.identity,rays,stack,stackCapacity,state,hit,scratch);
        if(status!=Status::Complete)return status;
    }
    return Status::Complete;
}
Status dispatch(const Type3QueryInput48P& input,const Type3QueryContext48P& context,const Type3Candidate48P& candidate,
    const Type3RayBuffers48P& rays,std::uint32_t* stack,std::size_t stackCapacity,
    Type3QueryState48P& state,bool& hit) noexcept {
    const auto& c=candidate.collision;
    return dispatchBorrowed(input,context,candidate,c.sphere,c.geometry.mesh,c.geometry.worldMatrix,true,
        rays,stack,stackCapacity,state,hit);
}
}
namespace detail {
Type3QueryStatus48P dispatchType3Borrowed48U(const Type3QueryInput48P& input,
    const Type3QueryContext48P& context,const Type3Candidate48P& candidate,
    const CollisionSphere48J& sphere,const assets::CollisionMesh48I* mesh,const BodyMatrix48G& matrix,bool shapePresent,
    const Type3RayBuffers48P& rays,std::uint32_t* stack,std::size_t stackCapacity,
    Type3QueryState48P& state,bool& hit) noexcept {
    return dispatchBorrowed(input,context,candidate,sphere,mesh,matrix,shapePresent,rays,stack,stackCapacity,state,hit);
}
Type3QueryStatus48P dispatchType3BorrowedWithScratch48U(const Type3QueryInput48P& input,
    const Type3QueryContext48P& context,const Type3Candidate48P& candidate,
    const CollisionSphere48J& sphere,const assets::CollisionMesh48I* mesh,const BodyMatrix48G& matrix,bool shapePresent,
    const Type3RayBuffers48P& rays,std::uint32_t* stack,std::size_t stackCapacity,
    Type3QueryState48P& state,bool& hit,Type3VectorScratch48U& scratch) noexcept {
    scratch.written=false;
    return dispatchBorrowed(input,context,candidate,sphere,mesh,matrix,shapePresent,
        rays,stack,stackCapacity,state,hit,&scratch);
}
bool eligibleType3Borrowed48U(const CollisionFilter48J& query,const CollisionFilter48J& candidate,
    std::uint8_t flagsF8) noexcept {return eligible(query,candidate,flagsF8);}
Type3QueryStatus48P outerSphereType3Borrowed48U(const Type3QueryState48P& query,
    const CollisionSphere48J& candidate,bool& hit) noexcept {return outerSphere(query,candidate,hit);}
}
namespace {
template<class R>bool raySpans(const R& b,Span* r) noexcept {
    return span(b.localStarts,b.capacity,r[0])&&span(b.localDirections,b.capacity,r[1])&&
        span(b.worldStarts,b.capacity,r[2])&&span(b.worldDirections,b.capacity,r[3])&&
        span(b.normals,b.capacity,r[4])&&span(b.points,b.capacity,r[5])&&
        span(b.hits,b.capacity,r[6])&&span(b.fractions,b.capacity,r[7])&&span(b.lengths,b.lengthCapacity,r[8]);
}
}
Type3QueryStatus48P queryStaticType3WithContextBounded48P(const Type3QueryInput48P& input,
    const Type3QueryContext48P& context,const Type3QuerySource48P& source,const Type3Candidate48P* candidates,std::size_t count,
    const Type3QueryWorkspace48P& workspace,const Type3QueryBuffers48P& destination,
    Type3QueryResult48P& output) noexcept {
    const bool direct=context.entry==Type3QueryEntry48P::Dispatcher;
    if((context.entry!=Type3QueryEntry48P::LimitedList&&!direct)||!context.pairCount||
       context.pairCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()/2)||
       (direct&&count!=1))return Status::InvalidInput;
    const auto n=static_cast<std::size_t>(context.pairCount);const auto& scratch=workspace.buffers;
    if(source.rays.capacity<n||source.rays.lengthCapacity<n||
       scratch.rays.capacity<source.rays.capacity||destination.rays.capacity<source.rays.capacity||
       scratch.rays.lengthCapacity<source.rays.lengthCapacity||destination.rays.lengthCapacity<source.rays.lengthCapacity||
       scratch.recordCapacity<source.recordCount||destination.recordCapacity<source.recordCount||
       scratch.effectCapacity<count||destination.effectCapacity<count)return Status::InsufficientCapacity;
    Span writes[24]{},reads[18]{};
    if(!raySpans(scratch.rays,writes)||!raySpans(destination.rays,writes+9)||
       !span(scratch.records,scratch.recordCapacity,writes[18])||!span(destination.records,destination.recordCapacity,writes[19])||
       !span(scratch.effects,scratch.effectCapacity,writes[20])||!span(destination.effects,destination.effectCapacity,writes[21])||
       !span(workspace.treeStack,workspace.treeStackCapacity,writes[22])||!span(&output,1,writes[23])||
       !span(&input,1,reads[0])||!span(&source,1,reads[1])||!span(&workspace,1,reads[2])||!span(&destination,1,reads[3])||
       !span(candidates,count,reads[4])||!raySpans(source.rays,reads+5)||
       !span(source.endpoints,source.endpointCount,reads[14])||!span(source.sceneMetadata,source.sceneMetadataCount,reads[15])||
       !span(source.records,source.recordCount,reads[16])||!span(&context,1,reads[17]))return Status::InvalidInput;
    for(unsigned i=0;i<24;++i){
        for(unsigned j=0;j<i;++j)if(overlap(writes[i],writes[j]))return Status::InvalidInput;
        for(auto read:reads)if(overlap(writes[i],read))return Status::InvalidInput;
    }
    const auto readSpan=[&](const auto* p,std::size_t amount) noexcept {
        Span r{};if(!span(p,amount,r))return false;
        for(auto w:writes)if(overlap(w,r))return false;
        return true;
    };
    const auto readFilter=[&](const CollisionFilter48J& f) noexcept {
        return readSpan(f.categoryMasks.data(),f.categoryMasks.size())&&
            readSpan(f.identityLists[0].identities.data(),f.identityLists[0].identities.size())&&
            readSpan(f.identityLists[1].identities.data(),f.identityLists[1].identities.size());
    };
    if(!readFilter(input.filter))return Status::InvalidInput;
    for(std::size_t i=0;i<count;++i){
        const auto& c=candidates[i];if(!readFilter(c.collision.filter)||!readSpan(c.sceneName,c.sceneNameBytes))return Status::InvalidInput;
        if(c.compound){
            if(!readSpan(c.compound,1)||c.compound->count>maxCompoundMembers50IJ||
               !readSpan(c.compound->parts,c.compound->count))return Status::InvalidInput;
            for(std::size_t j=0;j<c.compound->count;++j){const auto& part=c.compound->parts[j];const auto* m=part.mesh;
                if(!readSpan(m,1)||!readSpan(m->nodes.data(),m->nodes.size())||!readSpan(m->faces.data(),m->faces.size())||
                   !readSpan(m->vertices.data(),m->vertices.size())||!readSpan(m->vertex_normals.data(),m->vertex_normals.size())||
                   !readSpan(part.nodeRadii,part.nodeRadiusCount))return Status::InvalidInput;
            }
        }
        if(const auto* mesh=c.collision.geometry.mesh){
            if(!readSpan(mesh,1)||!readSpan(mesh->nodes.data(),mesh->nodes.size())||
               !readSpan(mesh->faces.data(),mesh->faces.size())||!readSpan(mesh->vertices.data(),mesh->vertices.size())||
               !readSpan(mesh->vertex_normals.data(),mesh->vertex_normals.size()))return Status::InvalidInput;
        }
    }
    copyRays(source.rays,scratch.rays);copy(scratch.records,source.records,source.recordCount);
    Type3QueryResult48P result{};result.normalModeWrite.bound=input.normalModeBound50JA;result.state=input.initial;result.spring=context.spring;
    result.currentCandidate=input.initialQueryCurrent;result.queryFlags=input.filter.flagsF8;result.candidateCount=count;
    if(!direct){
        if(context.resetSpring)resetSpringRecords48N(result.spring);
        auto rays=scratch.rays;
        const Type3PrepareInput48P preparation{context.pairCount,source.endpoints,source.endpointCount,
            source.sceneMetadata,source.sceneMetadataCount,input.sentinel};
        const auto prepared=prepareType3Rays48P(preparation,rays,result.state);
        if(prepared==Type3PrepareStatus48P::UnsupportedScope)return Status::UnsupportedScope;
        if(prepared==Type3PrepareStatus48P::CapacityExceeded)return Status::InsufficientCapacity;
        if(prepared!=Type3PrepareStatus48P::Complete)return Status::InvalidInput;
        result.queryFlags=static_cast<std::uint8_t>((input.filter.flagsF8|0x10u)&~0x20u);
    }
    std::uint32_t springCategory=input.initialSpringCurrentCategory;
    for(std::size_t i=0;i<count;++i){
        const auto& c=candidates[i].collision;
        scratch.effects[i]={c.filter.identity,c.filter.flagsF8,c.copied40,c.copied50};
        auto& effect=scratch.effects[i];
        if(!direct){
            if(c.typeTag!=input.collisionTypeTag){
                if(c.typeTag==input.groupTypeTag&&!(result.queryFlags&1u))return Status::UnsupportedScope;
                continue;
            }
            result.currentCandidate=c.filter.identity;effect.flagsF8=static_cast<std::uint8_t>(effect.flagsF8|0x10u);
            bool overlap=false;const auto sphere=outerSphere(result.state,c.sphere,overlap);
            if(sphere!=Status::Complete)return sphere;
            if(!overlap||!eligible(input.filter,c.filter,c.filter.flagsF8))continue;
        }
        //126FA0 is reached at dispatch entry before flags31/inner sphere.
        recordCollisionNormalMode50JA(result.normalModeWrite,input.normalMode);
        bool hit=false;const auto queried=dispatch(input,context,candidates[i],scratch.rays,workspace.treeStack,
            workspace.treeStackCapacity,result.state,hit);
        if(queried!=Status::Complete)return queried;
        if(!hit)continue;
        result.hasAnyHit=true;++result.geometryHits;
        if(direct)continue;
        result.queryFlags=static_cast<std::uint8_t>(result.queryFlags|0x20u);
        effect.copied40=input.copied40;effect.copied50=input.copied50;
        if(input.callbackEnabled){
            ++result.callbacks;
            const auto& candidate=candidates[i];
            const SpringStaticType3CallbackInput48P callback{context.bodyScene,context.bodyScenePresent,c.filter.identity,
                c.filter.categoryA4,springCategory,input.skipMask,input.dynamicMask,input.materialMask,3,
                candidate.hasMaterialScene,candidate.sceneName,candidate.sceneNameBytes};
            const SpringStaticType3CallbackSource48P callbackSource{
                scratch.rays.worldStarts,source.rays.capacity,scratch.rays.worldDirections,source.rays.capacity,
                scratch.rays.lengths,source.rays.lengthCapacity,scratch.rays.normals,source.rays.capacity,
                scratch.rays.points,source.rays.capacity,scratch.rays.hits,source.rays.capacity,
                source.sceneMetadata,source.sceneMetadataCount};
            const auto previous=result.spring.currentCandidate130;SpringStaticCallbackResult48N appended{};
            const auto called=applySpringStaticType3Callback48P(callback,callbackSource,scratch.records,
                source.recordCount,result.spring,appended);
            if(called==SpringCallbackStatus48N::InvalidInput)return Status::InvalidInput;
            if(called==SpringCallbackStatus48N::InsufficientCapacity)return Status::InsufficientCapacity;
            if(called==SpringCallbackStatus48N::UnsupportedScope)return Status::UnsupportedScope;
            if(called==SpringCallbackStatus48N::Complete){
                result.appended+=appended.appended;result.saturated+=appended.saturated;
                if(!previous||(springCategory&input.dynamicMask))springCategory=c.filter.categoryA4;
            }
        }
        if(c.hasResponseCallbackE8)return Status::UnsupportedScope;
    }
    copyRays(readRays(scratch.rays,source.rays.capacity,source.rays.lengthCapacity),destination.rays);
    copy(destination.records,scratch.records,source.recordCount);copy(destination.effects,scratch.effects,count);
    output=result;return Status::Complete;
}
Type3QueryStatus48P queryStaticType3Bounded48P(const Type3QueryInput48P& input,
    const Type3QuerySource48P& source,const Type3Candidate48P* candidates,std::size_t count,
    const Type3QueryWorkspace48P& workspace,const Type3QueryBuffers48P& destination,
    Type3QueryResult48P& output) noexcept {
    const Type3QueryContext48P context{input.spring,input.bodyScene,input.pairCount,
        input.resetSpring,input.bodyScenePresent,input.entry};
    return queryStaticType3WithContextBounded48P(input,context,source,candidates,count,workspace,destination,output);
}
}

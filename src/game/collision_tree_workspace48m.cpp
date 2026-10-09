#include "collision_tree48i.h"
#include <cmath>
#include <cstring>
#include <limits>
#include <cstdint>

namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool finite(const Vector3_48C& v) noexcept {for(float f:v.v)if(!std::isfinite(f))return false;return true;}
bool affine(const BodyMatrix48G& m) noexcept {
    for(float x:m.v)if(!std::isfinite(x))return false;
    return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
Vector3_48C vector(const std::array<float,3>& a) noexcept {return {{a[0],a[1],a[2]}};}
Vector3_48C difference(const Vector3_48C& a,const Vector3_48C& b) noexcept {
    return {{sub(a.v[0],b.v[0]),sub(a.v[1],b.v[1]),sub(a.v[2],b.v[2])}};
}
Vector3_48C direction(const BodyMatrix48G& m,const Vector3_48C& a) noexcept {
    Vector3_48C r{};
    for(unsigned i=0;i<3;++i)r.v[i]=add(add(mul(m.v[i],a.v[0]),mul(m.v[4+i],a.v[1])),mul(m.v[8+i],a.v[2]));
    return r;
}
Vector3_48C point(const BodyMatrix48G& m,const Vector3_48C& a) noexcept {
    auto r=direction(m,a);for(unsigned i=0;i<3;++i)r.v[i]=add(r.v[i],m.v[12+i]);return r;
}
BodyMatrix48G relative(const BodyMatrix48G& target,const BodyMatrix48G& query) noexcept {
    // 108BA4..108C08 transposes basis, with the original add-zero operations;
    // it does not perform a general inverse or repair a nonorthogonal basis.
    BodyMatrix48G inverse{};inverse.v[15]=1.f;
    for(unsigned c=0;c<3;++c)for(unsigned i=0;i<3;++i)inverse.v[c*4+i]=add(target.v[i*4+c],0.f);
    auto t=direction(inverse,{{target.v[12],target.v[13],target.v[14]}});
    for(unsigned i=0;i<3;++i)inverse.v[12+i]=mul(t.v[i],-1.f);
    BodyMatrix48G r{};
    for(unsigned c=0;c<4;++c)for(unsigned i=0;i<4;++i)
        r.v[c*4+i]=add(add(add(mul(inverse.v[i],query.v[c*4]),mul(inverse.v[4+i],query.v[c*4+1])),mul(inverse.v[8+i],query.v[c*4+2])),mul(inverse.v[12+i],query.v[c*4+3]));
    return r;
}
bool overlaps(const assets::CollisionMeshNode48I& n,const Vector3_48C& first,const Vector3_48C& second) noexcept {
    const auto a=difference(first,vector(n.center)),b=difference(second,vector(n.center));
    Vector3_48C mid{},half{},absHalf{};
    for(unsigned i=0;i<3;++i) {
        mid.v[i]=mul(.5f,add(a.v[i],b.v[i]));
        half.v[i]=sub(mid.v[i],a.v[i]);absHalf.v[i]=std::fabs(half.v[i]);
        if(!(sub(sub(std::fabs(mid.v[i]),n.half[i]),absHalf.v[i])<=0.f))return false;
    }
    // Keep the three original separate multiply/add or multiply/subtract pairs.
    const unsigned a0[3]={1,0,0},a1[3]={2,2,1};
    for(unsigned k=0;k<3;++k) {
        const unsigned i=a0[k],j=a1[k];
        const float lhs=std::fabs(sub(mul(mid.v[i],half.v[j]),mul(mid.v[j],half.v[i])));
        const float rhs=add(mul(n.half[i],absHalf.v[j]),mul(n.half[j],absHalf.v[i]));
        if(!(lhs<=rhs))return false;
    }
    return true;
}
float scalarLength(const Vector3_48C& d) noexcept {
    const float squared=add(add(mul(d.v[0],d.v[0]),mul(d.v[1],d.v[1])),mul(d.v[2],d.v[2]));
    if(squared==0.f)return 0.f;
    // EE scalar positive RSQRT interface: double root/divide, rounded once;
    // then original reciprocal to get length (1086B0/1086BC).
    volatile float reciprocal=static_cast<float>(1.0/std::sqrt(static_cast<double>(squared)));
    return div(1.f,reciprocal);
}
CollisionContact48I contact(const TriangleHit48H& hit,const DirectedSegment48H& segment,
    const assets::CollisionMeshFace48I& face,const CollisionCandidate48I& candidate,std::uint32_t index) noexcept {
    CollisionContact48I r{};r.fraction=hit.fraction;r.candidateId=candidate.id;r.face=index;
    r.point=point(candidate.worldMatrix,hit.point);r.normal=direction(candidate.worldMatrix,hit.normal);
    float scale;const std::uint32_t scaleBits=0x38000100u;std::memcpy(&scale,&scaleBits,4);
    Vector3_48C normal{},remaining{};
    for(unsigned i=0;i<3;++i) {
        normal.v[i]=mul(scale,static_cast<float>(face.normal[i]));
        remaining.v[i]=mul(sub(1.f,hit.fraction),mul(segment.length,segment.direction.v[i]));
    }
    const float projected=add(add(mul(remaining.v[0],normal.v[0]),mul(remaining.v[1],normal.v[1])),mul(1.f,mul(remaining.v[2],normal.v[2])));
    for(unsigned i=0;i<3;++i)remaining.v[i]=mul(projected,normal.v[i]);
    r.correction=direction(candidate.worldMatrix,remaining);
    // 1214FC / 121530 add vf0 after direction transforms (including +0 XYZ).
    for(unsigned i=0;i<3;++i){r.normal.v[i]=add(r.normal.v[i],0.f);r.correction.v[i]=add(r.correction.v[i],0.f);}
    return r;
}
struct Extent {std::uintptr_t first=0,last=0;};
bool extent(const void* p,std::size_t count,std::size_t element,Extent& out) noexcept {
    if(count&&!p)return false;
    if(count>std::numeric_limits<std::size_t>::max()/element)return false;
    const auto bytes=count*element;
    const auto start=reinterpret_cast<std::uintptr_t>(p);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-start)return false;
    out={start,start+bytes};return true;
}
bool overlaps(const Extent& a,const Extent& b) noexcept {
    return a.first<a.last&&b.first<b.last&&a.first<b.last&&b.first<a.last;
}
bool validExtents(const LocalSegment48I& query,const CollisionCandidate48I* candidates,
    std::size_t count,const CollisionWorkspace48M& workspace,
    CollisionResult48I& output,std::size_t& traceUsed) noexcept {
    if((workspace.stackCapacity&&reinterpret_cast<std::uintptr_t>(workspace.stack)%alignof(std::uint32_t))||
       (workspace.traceCapacity&&reinterpret_cast<std::uintptr_t>(workspace.trace)%alignof(CollisionVisit48I))||
       (count&&reinterpret_cast<std::uintptr_t>(candidates)%alignof(CollisionCandidate48I)))return false;
    Extent writable[4],candidateExtent;
    if(!extent(workspace.stack,workspace.stackCapacity,sizeof(std::uint32_t),writable[0])||
       !extent(workspace.trace,workspace.traceCapacity,sizeof(CollisionVisit48I),writable[1])||
       !extent(&output,1,sizeof(output),writable[2])||
       !extent(&traceUsed,1,sizeof(traceUsed),writable[3])||
       !extent(candidates,count,sizeof(*candidates),candidateExtent))return false;
    for(unsigned i=0;i<4;++i)for(unsigned j=0;j<i;++j)
        if(overlaps(writable[i],writable[j]))return false;
    const auto disjoint=[&](const void* p,std::size_t n,std::size_t size) noexcept {
        Extent input;if(!extent(p,n,size,input))return false;
        for(const auto& w:writable)if(overlaps(w,input))return false;
        return true;
    };
    if(!disjoint(&query,1,sizeof(query))||!disjoint(&workspace,1,sizeof(workspace)))return false;
    for(const auto& w:writable)if(overlaps(w,candidateExtent))return false;
    for(std::size_t c=0;c<count;++c){
        const auto* m=candidates[c].mesh;
        if(!m||reinterpret_cast<std::uintptr_t>(m)%alignof(assets::CollisionMesh48I))return false;
        if(!disjoint(m,1,sizeof(*m))||
           !disjoint(m->vertices.data(),m->vertices.size(),sizeof(m->vertices[0]))||
           !disjoint(m->vertex_normals.data(),m->vertex_normals.size(),sizeof(m->vertex_normals[0]))||
           !disjoint(m->nodes.data(),m->nodes.size(),sizeof(m->nodes[0]))||
           !disjoint(m->faces.data(),m->faces.size(),sizeof(m->faces[0])))return false;
    }
    return true;
}
}

CollisionQueryStatus48I queryOrderedCollisionBounded48M(const LocalSegment48I& query,
    const CollisionCandidate48I* candidates,std::size_t count,
    const CollisionWorkspace48M& workspace,CollisionResult48I& output,
    std::size_t& traceUsed) noexcept {
    if(!validExtents(query,candidates,count,workspace,output,traceUsed))return CollisionQueryStatus48I::InvalidInput;
    traceUsed=0;
    if(!finite(query.first)||!finite(query.second)||!affine(query.worldMatrix))return CollisionQueryStatus48I::InvalidInput;
    CollisionResult48I result{};result.selected.fraction=1.f;result.nearestPair.fraction=1.f;
    bool found=false,pairFound=false;
    const auto record=[&](const CollisionVisit48I& visit) noexcept {
        if(!workspace.trace)return true;
        if(traceUsed==workspace.traceCapacity)return false;
        workspace.trace[traceUsed++]=visit;return true;
    };
    for(std::size_t c=0;c<count;++c){
        const auto& candidate=candidates[c];
        if(!affine(candidate.worldMatrix))return CollisionQueryStatus48I::InvalidInput;
        const auto& mesh=*candidate.mesh;
        if(mesh.reverse_mode!=0||mesh.nodes.empty()||mesh.faces.empty()||
           mesh.nodes.size()>std::numeric_limits<std::size_t>::max()-mesh.faces.size())return CollisionQueryStatus48I::InvalidInput;
        const std::size_t visitLimit=mesh.nodes.size()+mesh.faces.size();
        const auto matrix=relative(candidate.worldMatrix,query.worldMatrix);
        const auto first=point(matrix,query.first),second=point(matrix,query.second);
        if(!finite(first)||!finite(second))return CollisionQueryStatus48I::InvalidInput;
        const auto delta=difference(second,first);const float length=scalarLength(delta);
        if(!std::isfinite(length))return CollisionQueryStatus48I::InvalidInput;
        // Preserve the original zero-direction rejection without its transient
        // zero times infinity, as in the original native48I boundary.
        if(length==0.f)continue;
        const float inverseLength=div(1.f,length);
        DirectedSegment48H segment{first,{{mul(delta.v[0],inverseLength),mul(delta.v[1],inverseLength),mul(delta.v[2],inverseLength)}},length};
        if(!workspace.stackCapacity)return CollisionQueryStatus48I::InsufficientCapacity;
        std::size_t stackUsed=1,visited=0;workspace.stack[0]=0;
        while(stackUsed){
            if(visited==visitLimit)return CollisionQueryStatus48I::InvalidInput;
            ++visited;
            const auto tag=workspace.stack[--stackUsed];
            const auto index=tag&~assets::collision_mesh_leaf48i;
            if(!(tag&assets::collision_mesh_leaf48i)){
                if(index>=mesh.nodes.size())return CollisionQueryStatus48I::InvalidInput;
                const auto& node=mesh.nodes[index];
                if(!finite(vector(node.center))||!finite(vector(node.half)))return CollisionQueryStatus48I::InvalidInput;
                const bool overlap=overlaps(node,first,second);++result.nodesVisited;
                if(!record({candidate.id,index,false,overlap}))return CollisionQueryStatus48I::InsufficientCapacity;
                if(!overlap){++result.nodesPruned;continue;}
                if(workspace.stackCapacity-stackUsed<2)return CollisionQueryStatus48I::InsufficientCapacity;
                workspace.stack[stackUsed++]=node.children[1];
                workspace.stack[stackUsed++]=node.children[0];
            }else{
                if(index>=mesh.faces.size())return CollisionQueryStatus48I::InvalidInput;
                const auto& face=mesh.faces[index];Triangle48H triangle{};
                triangle.hasVertexNormals=!mesh.vertex_normals.empty();
                for(unsigned i=0;i<3;++i){
                    if(face.vertices[i]>=mesh.vertices.size()||(triangle.hasVertexNormals&&face.vertices[i]>=mesh.vertex_normals.size()))return CollisionQueryStatus48I::InvalidInput;
                    triangle.vertices[i]=vector(mesh.vertices[face.vertices[i]]);
                    if(triangle.hasVertexNormals)triangle.vertexNormals[i]=vector(mesh.vertex_normals[face.vertices[i]]);
                }
                TriangleHit48H hit{};const auto status=intersectTriangle48H(segment,triangle,hit);++result.leavesTested;
                if(!record({candidate.id,index,true,status==TriangleQueryStatus48H::Hit}))return CollisionQueryStatus48I::InsufficientCapacity;
                if(status==TriangleQueryStatus48H::InvalidInput)return CollisionQueryStatus48I::InvalidInput;
                if(status==TriangleQueryStatus48H::UnsupportedInterpolationBoundary)return CollisionQueryStatus48I::UnsupportedInterpolationBoundary;
                if(status!=TriangleQueryStatus48H::Hit)continue;
                ++result.leafHits;
                const auto current=contact(hit,segment,face,candidate,index);
                if(!finite(current.point)||!finite(current.normal)||!finite(current.correction))return CollisionQueryStatus48I::InvalidInput;
                if(!pairFound||hit.fraction<result.nearestPair.fraction){result.nearestPair=current;pairFound=true;}
                if(hit.fraction<=result.selected.fraction){result.selected=current;found=true;}
            }
        }
    }
    if(!found)return CollisionQueryStatus48I::Miss;
    output=result;return CollisionQueryStatus48I::Hit;
}
}

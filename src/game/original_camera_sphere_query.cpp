#include "original_camera_sphere_query.h"
#include "scalar_minmax48o.h"
#include "compound_collision50ij.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using V=std::array<float,3>;
using S=OriginalCameraSphereStatus;
struct Arithmetic {
    bool valid=true;
    float checked(float r) noexcept {valid=valid&&std::isfinite(r);return r;}
    float add(float a,float b) noexcept {volatile float r=a+b;return checked(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return checked(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return checked(r);}
    float div(float a,float b) noexcept {
        if(b==0.f){valid=false;return 0.f;}
        volatile float r=a/b;return checked(r);
    }
    V difference(const V& a,const V& b) noexcept {
        return {{sub(a[0],b[0]),sub(a[1],b[1]),sub(a[2],b[2])}};
    }
    float dot(const V& a,const V& b) noexcept {
        // 106E70 uses XYZ products, then X+Y, then 1*Z.
        return add(add(mul(a[0],b[0]),mul(a[1],b[1])),mul(1.f,mul(a[2],b[2])));
    }
    V cross(const V& a,const V& b) noexcept {
        V r{};for(unsigned k=0;k<3;++k)
            r[k]=sub(mul(a[(k+1)%3],b[(k+2)%3]),mul(b[(k+1)%3],a[(k+2)%3]));
        return r;
    }
    V toLocal(const std::array<float,16>& m,const V& p) noexcept {
        V t{},r{};
        for(unsigned k=0;k<3;++k)t[k]=sub(p[k],add(m[12+k],0.f));
        for(unsigned k=0;k<3;++k)r[k]=add(add(mul(add(m[4*k],0.f),t[0]),
            mul(add(m[4*k+1],0.f),t[1])),mul(add(m[4*k+2],0.f),t[2]));
        return r;
    }
    V toWorld(const std::array<float,16>& m,const V& p,bool point) noexcept {
        V r{};for(unsigned k=0;k<3;++k)
            r[k]=add(add(add(mul(m[k],p[0]),mul(m[4+k],p[1])),mul(m[8+k],p[2])),point?m[12+k]:0.f);
        return r;
    }
    V normalized(const V& n) noexcept {
        const float squared=dot(n,n);
        // Same finite VRSQRT convention as collision_query48H: zero denominator
        // saturates Q to FLT_MAX; do not replace a zero normal with an axis.
        volatile float root=std::sqrt(std::fabs(squared));checked(root);
        const float q=squared==0.f?std::numeric_limits<float>::max():div(1.f,root);
        return {{mul(n[0],q),mul(n[1],q),mul(n[2],q)}};
    }
};
bool finite(const V& v) noexcept {for(float f:v)if(!std::isfinite(f))return false;return true;}
std::array<float,16> arrayMatrix(const float (&source)[16]) noexcept {
    std::array<float,16> result{};std::memcpy(result.data(),source,sizeof source);return result;
}
bool affine(const std::array<float,16>& m) noexcept {
    for(float f:m)if(!std::isfinite(f))return false;
    return m[3]==0.f&&m[7]==0.f&&m[11]==0.f&&m[15]==1.f;
}
struct Extent {std::uintptr_t first=0,last=0;};
bool extent(const void* p,std::size_t n,std::size_t size,Extent& e) noexcept {
    if(n&&!p)return false;
    if(size&&n>std::numeric_limits<std::size_t>::max()/size)return false;
    const auto first=reinterpret_cast<std::uintptr_t>(p);const auto bytes=n*size;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
    e={first,first+bytes};return true;
}
bool overlaps(const Extent& a,const Extent& b) noexcept {
    return a.first<a.last&&b.first<b.last&&a.first<b.last&&b.first<a.last;
}
bool storage(const OriginalCameraSphereQuery& q,const assets::CollisionMesh48I& mesh,
    const std::array<float,16>& world,const OriginalCameraSphereContact& prior,
    const OriginalCameraSphereWorkspace& w,OriginalCameraSphereContact& out) noexcept {
    if(w.capacity&&reinterpret_cast<std::uintptr_t>(w.tags)%alignof(std::uint32_t))return false;
    Extent scratch,result;
    if(!extent(w.tags,w.capacity,sizeof(*w.tags),scratch)||!extent(&out,1,sizeof out,result)||overlaps(scratch,result))return false;
    const auto disjoint=[&](const void* p,std::size_t n,std::size_t size,bool allowResult=false) noexcept {
        Extent in;return extent(p,n,size,in)&&!overlaps(scratch,in)&&(allowResult||!overlaps(result,in));
    };
    return disjoint(&q,1,sizeof q)&&disjoint(&mesh,1,sizeof mesh)&&
        disjoint(&world,1,sizeof world)&&disjoint(&prior,1,sizeof prior,true)&&
        disjoint(&w,1,sizeof w)&&
        disjoint(mesh.vertices.data(),mesh.vertices.size(),sizeof mesh.vertices[0])&&
        disjoint(mesh.vertex_normals.data(),mesh.vertex_normals.size(),sizeof mesh.vertex_normals[0])&&
        disjoint(mesh.nodes.data(),mesh.nodes.size(),sizeof mesh.nodes[0])&&
        disjoint(mesh.faces.data(),mesh.faces.size(),sizeof mesh.faces[0]);
}
bool overlapsNode(Arithmetic& a,const V& local,const assets::CollisionMeshNode48I& node,
    const OriginalCameraSphereQuery& query) noexcept {
    V distance{},residual{};
    for(unsigned k=0;k<3;++k){
        distance[k]=std::fabs(a.sub(local[k],node.center[k]));
        if(!(a.sub(a.sub(distance[k],node.half[k]),query.radius50)<=0.f))return false;
    }
    for(unsigned k=0;k<3;++k)
        residual[k]=a.sub(distance[k],distance[k]<node.half[k]?distance[k]:node.half[k]);
    const float squared=a.add(a.add(a.mul(residual[0],residual[0]),a.mul(residual[1],residual[1])),a.mul(residual[2],residual[2]));
    return !(query.radiusSquared54<=squared); // exact tangent is rejected109DF8
}
bool inside(Arithmetic& a,const V (&v)[3],const V& p,const V& n) noexcept {
    // 109F10's four oriented cross tests. No segment-query projectedInside48H.
    for(unsigned origin=1;origin<3;++origin){
        const V first=a.difference(v[(origin+1)%3],v[origin]);
        const V second=a.difference(v[(origin+2)%3],v[origin]);
        const V offset=a.difference(p,v[origin]);
        if(a.dot(n,a.cross(first,offset))<0.f)return false;
        if(a.dot(n,a.cross(offset,second))<0.f)return false;
    }
    return true;
}
bool edgeHit(Arithmetic& a,const V& center,const V& start,const V& end,float radius) noexcept {
    const V direction=a.difference(end,start),offset=a.difference(center,start);
    float t=a.div(a.dot(direction,offset),a.dot(direction,direction));
    t=detail::scalarMin48O(t,1.f);t=detail::scalarMax48O(t,0.f);
    V distance{};for(unsigned k=0;k<3;++k)
        distance[k]=a.sub(center[k],a.add(start[k],a.mul(direction[k],t)));
    const float squared=a.add(a.add(a.mul(distance[0],distance[0]),a.mul(distance[1],distance[1])),a.mul(distance[2],distance[2]));
    // Scalar SQRT109EF0, distinct from the reciprocal-length helper1086B0.
    volatile float length=static_cast<float>(std::sqrt(static_cast<double>(squared)));
    a.checked(length);return std::fabs(length)<radius;
}
}

OriginalCameraSphereStatus queryOriginalCameraSphereMesh(
    const OriginalCameraSphereQuery& query,const assets::CollisionMesh48I& mesh,
    const std::array<float,16>& world,const OriginalCameraSphereContact& prior,
    const OriginalCameraSphereWorkspace& workspace,
    OriginalCameraSphereContact& output) noexcept {
    if(!storage(query,mesh,world,prior,workspace,output)||std::fegetround()!=FE_TONEAREST||
       !finite(query.worldCenter)||!affine(world)||!std::isfinite(query.radius50)||
       !std::isfinite(query.radiusSquared54)||query.radius50<0.f||query.radiusSquared54<0.f||
       prior.count>1||(prior.count&&!std::isfinite(prior.planeDistance))||
       mesh.nodes.empty()||mesh.faces.empty()||
       mesh.nodes.size()>std::numeric_limits<std::size_t>::max()-mesh.faces.size())return S::InvalidInput;
    if(mesh.reverse_mode!=0)return S::UnsupportedVertexStorage;
    if(!workspace.capacity)return S::InsufficientWorkspace;
    Arithmetic a;const V local=a.toLocal(world,query.worldCenter);
    if(!a.valid)return S::UnsupportedArithmetic;
    const auto visitLimit=mesh.nodes.size()+mesh.faces.size();
    std::size_t used=1,visited=0;workspace.tags[0]=0;
    while(used){
        if(visited++==visitLimit)return S::InvalidInput; // decoded rooted-tree contract
        const auto tag=workspace.tags[--used];
        if(tag&~(assets::collision_mesh_leaf48i|0x7fffu))return S::InvalidInput;
        const auto index=tag&0x7fffu;
        if(!(tag&assets::collision_mesh_leaf48i)){
            if(index>=mesh.nodes.size())return S::InvalidInput;
            const auto& node=mesh.nodes[index];
            if(!finite(node.center)||!finite(node.half))return S::InvalidInput;
            for(float f:node.half)if(f<0.f)return S::InvalidInput;
            const bool overlap=overlapsNode(a,local,node,query);
            if(!a.valid)return S::UnsupportedArithmetic;
            if(!overlap)continue;
            if(workspace.capacity-used<2)return S::InsufficientWorkspace;
            workspace.tags[used++]=node.children[1];workspace.tags[used++]=node.children[0];
            continue;
        }
        if(index>=mesh.faces.size())return S::InvalidInput;
        const auto& face=mesh.faces[index];V v[3]{};
        for(unsigned k=0;k<3;++k){
            if(face.vertices[k]>=mesh.vertices.size())return S::InvalidInput;
            v[k]=mesh.vertices[face.vertices[k]];if(!finite(v[k]))return S::InvalidInput;
        }
        float scale;const std::uint32_t scaleWord=0x38000100u;std::memcpy(&scale,&scaleWord,sizeof scale);
        V n{};for(unsigned k=0;k<3;++k)n[k]=a.mul(scale,static_cast<float>(face.normal[k]));
        const float distance=a.dot(n,a.difference(local,v[0]));
        if(!a.valid)return S::UnsupportedArithmetic;
        if(distance<0.f||!(distance<=query.radius50))continue;
        V projected{};for(unsigned k=0;k<3;++k)projected[k]=a.sub(local[k],a.mul(distance,n[k]));
        bool hit=inside(a,v,projected,n);
        if(!a.valid)return S::UnsupportedArithmetic;
        // Source checks edges0->1,1->2,2->0, stopping at first strict-radius hit.
        for(unsigned k=0;k<3&&!hit;++k){
            hit=edgeHit(a,local,v[k],v[(k+1)%3],query.radius50);
            if(!a.valid)return S::UnsupportedArithmetic;
        }
        if(!hit)continue;
        if(prior.count&&!(distance<prior.planeDistance))continue;
        OriginalCameraSphereContact result{};
        result.count=1;result.planeDistance=distance;result.worldCenter=query.worldCenter;
        result.worldNormal=a.toWorld(world,a.normalized(n),false);
        // Edge acceptance STILL stores the plane-projected point108...095AC.
        result.worldPoint=a.toWorld(world,projected,true);result.face=index;
        if(!a.valid)return S::UnsupportedArithmetic;
        output=result;return S::Hit; //108E90 short-circuits its other child
    }
    return S::Miss;
}
}

namespace rr::game {
OriginalCameraSphereStatus queryOriginalCameraSphereCompound(
    const OriginalCameraSphereQuery& query,const CompoundCollisionView50IJ& compound,
    const OriginalCameraSphereContact& prior,const OriginalCameraSphereWorkspace& workspace,
    OriginalCameraSphereContact& output) noexcept {
    const auto ownerWorld=arrayMatrix(compound.ownerWorld.v);
    if(std::fegetround()!=FE_TONEAREST||compound.count>maxCompoundMembers50IJ||
       (compound.count&&!compound.parts)||!finite(query.worldCenter)||
       !affine(ownerWorld)||!std::isfinite(query.radius50)||
       !std::isfinite(query.radiusSquared54)||query.radius50<0.f||query.radiusSquared54<0.f||
       prior.count>1||(prior.count&&!std::isfinite(prior.planeDistance)))return S::InvalidInput;
    Extent scratch,result,description,parts;
    if(!extent(workspace.tags,workspace.capacity,sizeof(*workspace.tags),scratch)||
       !extent(&output,1,sizeof output,result)||!extent(&compound,1,sizeof compound,description)||
       !extent(compound.parts,compound.count,sizeof(*compound.parts),parts)||
       overlaps(scratch,result)||overlaps(scratch,description)||overlaps(scratch,parts)||
       overlaps(result,description)||overlaps(result,parts))return S::InvalidInput;
    assets::CollisionMeshNode48I coarse{};
    for(unsigned k=0;k<3;++k){coarse.center[k]=compound.center.v[k];coarse.half[k]=compound.half.v[k];}
    if(!finite(coarse.center)||!finite(coarse.half))return S::InvalidInput;
    for(float h:coarse.half)if(h<0.f)return S::InvalidInput;
    Arithmetic a;const auto local=a.toLocal(ownerWorld,query.worldCenter);
    const bool overlap=overlapsNode(a,local,coarse,query);
    if(!a.valid)return S::UnsupportedArithmetic;
    if(!overlap)return S::Miss;
    for(std::size_t k=0;k<compound.count;++k){
        const auto& part=compound.parts[k];if(!part.mesh)return S::InvalidInput;
        const auto status=queryOriginalCameraSphereMesh(query,*part.mesh,arrayMatrix(part.worldMatrix.v),prior,workspace,output);
        if(status!=S::Miss)return status;
    }
    return S::Miss;
}
}

#pragma once
#include "scene_graph49p.h"
#include "../assets/collision_mesh48i.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace rr::game {
constexpr std::size_t maxCompoundMembers50IJ=1024;
// One physical shape1 owner has an ordered span of original mesh records. Freeze
// all backing addresses at cold load. No part is an independent filter/callback owner.
struct CompoundCollisionPart50IJ {
    const assets::CollisionMesh48I* mesh=nullptr;
    BodyMatrix48G worldMatrix{}; // original part+00: COL local40 * bound scene world80
    std::uint32_t active=1;      // original container+E4; type3 deliberately ignores it
    const float* nodeRadii=nullptr;
    std::size_t nodeRadiusCount=0,maxDepth=0; // cold49X preparation, needed by type2
};
struct CompoundCollisionView50IJ {
    const CompoundCollisionPart50IJ* parts=nullptr;
    std::size_t count=0;
    BodyMatrix48G ownerWorld{}; // original container+80
    Vector3_48C center{},half{}; // original container+00/+10, in owner-local space
    // Original3C7DE0..3C7E70 initializes3DF540/550/560 to these exact axes.
    // Kept explicit for the numerical seam; unused original fourth lanes are1.
    std::array<Vector3_48C,3> coarseAxes{{Vector3_48C{{1.f,0.f,0.f}},
        Vector3_48C{{0.f,1.f,0.f}},Vector3_48C{{0.f,0.f,1.f}}}};
};
namespace compound50ij_detail {
using V=Vector3_48C;
struct Math {
    bool valid=true;
    float keep(float f) noexcept {valid=valid&&std::isfinite(f);return f;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float r=a*b;return keep(r);}
    V transform(const BodyMatrix48G& m,const V& a,bool point) noexcept {
        V r{};for(unsigned i=0;i<3;++i)
            r.v[i]=add(add(add(mul(m.v[i],a.v[0]),mul(m.v[4+i],a.v[1])),mul(m.v[8+i],a.v[2])),point?m.v[12+i]:0.f);
        return r;
    }
    BodyMatrix48G inverse(const BodyMatrix48G& m) noexcept {
        BodyMatrix48G r{};r.v[15]=1.f;
        for(unsigned row=0;row<3;++row)for(unsigned col=0;col<3;++col)r.v[4*row+col]=add(m.v[4*col+row],0.f);
        // Original inverse has no extra add-zero after its 3-term translation sum.
        for(unsigned i=0;i<3;++i)r.v[12+i]=mul(add(add(mul(r.v[i],m.v[12]),mul(r.v[4+i],m.v[13])),mul(r.v[8+i],m.v[14])),-1.f);
        return r;
    }
    BodyMatrix48G relative(const BodyMatrix48G& first,const BodyMatrix48G& second) noexcept {
        const auto inv=inverse(first);BodyMatrix48G r{};
        for(unsigned row=0;row<4;++row)for(unsigned col=0;col<4;++col)
            r.v[4*row+col]=add(add(add(mul(inv.v[col],second.v[4*row]),mul(inv.v[4+col],second.v[4*row+1])),mul(inv.v[8+col],second.v[4*row+2])),mul(inv.v[12+col],second.v[4*row+3]));
        return r;
    }
    float dot(const V& a,const V& b) noexcept {
        const float x=mul(a.v[0],b.v[0]),y=mul(a.v[1],b.v[1]),z=mul(a.v[2],b.v[2]);
        return add(add(x,y),mul(1.f,z));
    }
    V cross(const V& a,const V& b) noexcept {
        V r{};for(unsigned i=0;i<3;++i)r.v[i]=sub(mul(a.v[(i+1)%3],b.v[(i+2)%3]),mul(a.v[(i+2)%3],b.v[(i+1)%3]));return r;
    }
};
inline bool affine(const BodyMatrix48G& m) noexcept {
    for(float f:m.v)if(!std::isfinite(f))return false;
    return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
inline bool box(const V& c,const V& h) noexcept {
    for(unsigned i=0;i<3;++i)if(!std::isfinite(c.v[i])||!std::isfinite(h.v[i])||h.v[i]<0.f)return false;return true;
}
inline V vector(const std::array<float,3>& a) noexcept {return {{a[0],a[1],a[2]}};}
inline void flatten(const SceneGraph49P& graph,SceneNodeIndex49P root,
    SceneNodeIndex49P* ordered,std::size_t& used) noexcept {
    //28C400 emits direct siblings before descending through those siblings.
    auto child=graph.nodes[static_cast<std::uint32_t>(root)].firstChild184;
    for(auto n=child;n!=SceneNodeIndex49P::none;n=graph.nodes[static_cast<std::uint32_t>(n)].nextSibling188)ordered[used++]=n;
    for(auto n=child;n!=SceneNodeIndex49P::none;n=graph.nodes[static_cast<std::uint32_t>(n)].nextSibling188)flatten(graph,n,ordered,used);
}
}
//126450: climb to ultimate root, put it at zero, then28C400 flatten. Does NOT
// assume PSG row identity, subtract/add an exporter base frame, or match names.
// Cold binding; output must be disjoint from asset/graph backing. It may contain
// up to1024 repeated references to the at-most80 source scene nodes.
inline bool bindCompoundSceneNodes50IJ(const assets::CollisionCompoundMesh50IJ& asset,
    const SceneGraph49P& graph,SceneNodeIndex49P supplied,
    SceneNodeIndex49P* output,std::size_t capacity) noexcept {
    if(!validateSceneGraph49P(graph)||static_cast<std::int32_t>(supplied)<0||
       static_cast<std::uint32_t>(supplied)>=graph.count||asset.members.size()>maxCompoundMembers50IJ||
       capacity<asset.members.size()||(!asset.members.empty()&&!output))return false;
    auto root=supplied;
    while(graph.nodes[static_cast<std::uint32_t>(root)].parent180!=SceneNodeIndex49P::none)root=graph.nodes[static_cast<std::uint32_t>(root)].parent180;
    SceneNodeIndex49P order[maxSceneNodes49P];std::size_t used=1;order[0]=root;
    compound50ij_detail::flatten(graph,root,order,used);
    for(const auto& part:asset.members)if(part.scene_node_index>=used)return false;
    for(std::size_t i=0;i<asset.members.size();++i)output[i]=order[asset.members[i].scene_node_index];
    return true;
}
//11E630 original compound-local bounds. Inactive member0 leaves the initialized
// zero extrema in place; later active parts merge with zero, not a repaired
// first-active special case. All part matrices already represent11F680 updates.
// Failure leaves both outputs unchanged. No allocation or scene mutation.
inline bool compoundBounds50IJ(const CompoundCollisionPart50IJ* parts,std::size_t count,
    const BodyMatrix48G& ownerWorld,Vector3_48C& center,Vector3_48C& half) noexcept {
    using namespace compound50ij_detail;
    if(std::fegetround()!=FE_TONEAREST||count>maxCompoundMembers50IJ||(count&&!parts)||!affine(ownerWorld))return false;
    Math m;const auto inverse=m.inverse(ownerWorld);V low{},high{};
    for(std::size_t i=0;i<count;++i){const auto& part=parts[i];if(!part.active)continue;
        if(!part.mesh||part.mesh->nodes.empty()||!affine(part.worldMatrix))return false;
        const auto& n=part.mesh->nodes[0];if(!box(vector(n.center),vector(n.half)))return false;
        V axis[3]{};for(unsigned j=0;j<3;++j){axis[j].v[j]=n.half[j];axis[j]=m.transform(inverse,m.transform(part.worldMatrix,axis[j],false),false);}
        const auto c=m.transform(inverse,m.transform(part.worldMatrix,vector(n.center),true),true);
        for(unsigned j=0;j<3;++j){
            const float h=m.add(std::fabs(axis[2].v[j]),m.add(std::fabs(axis[0].v[j]),std::fabs(axis[1].v[j])));
            const float a=m.sub(c.v[j],h),b=m.add(c.v[j],h);
            if(i==0){low.v[j]=a;high.v[j]=b;}
            else{low.v[j]=low.v[j]<a?low.v[j]:a;high.v[j]=high.v[j]<=b?b:high.v[j];}
        }
    }
    V c{},h{};for(unsigned j=0;j<3;++j){c.v[j]=m.mul(.5f,m.add(low.v[j],high.v[j]));h.v[j]=m.mul(.5f,m.sub(high.v[j],low.v[j]));}
    if(!m.valid)return false;center=c;half=h;return true;
}
//11F9B4..11FA2C: radius uses unscaled local half, then center becomes world XYZ.
// Failure leaves outputs unchanged. No rescaling for owner orientation/scale.
inline bool compoundSphere50IJ(const CompoundCollisionView50IJ& compound,
    Vector3_48C& worldCenter,float& radius) noexcept {
    using namespace compound50ij_detail;
    if(std::fegetround()!=FE_TONEAREST||!box(compound.center,compound.half)||!affine(compound.ownerWorld))return false;
    Math m;const auto& h=compound.half;
    const float square=m.add(m.add(m.mul(h.v[0],h.v[0]),m.mul(h.v[1],h.v[1])),m.mul(h.v[2],h.v[2]));
    volatile float length=std::sqrt(square);m.keep(length);
    const auto center=m.transform(compound.ownerWorld,compound.center,true);
    if(!m.valid)return false;worldCenter=center;radius=length;return true;
}
//122BA0(shape2,shape1)->108D00->105CA0: ordered15-axis inclusive box overlap.
// Query root-box selection is caller-owned. The explicit initialized basis is
// used verbatim, including cross-axis zeros. No epsilon, normalization or SAT
// shortcut substitutes for the source arithmetic. False means invalid numeric;
// a geometric separation is returned through overlap. Output unchanged on error.
inline bool compoundCoarseOverlap50IJ(const Vector3_48C& queryCenter,const Vector3_48C& queryHalf,
    const BodyMatrix48G& queryWorld,const CompoundCollisionView50IJ& compound,bool& overlap) noexcept {
    using namespace compound50ij_detail;
    if(std::fegetround()!=FE_TONEAREST||!box(compound.center,compound.half)||
       !affine(queryWorld)||!affine(compound.ownerWorld))return false;
    for(unsigned j=0;j<3;++j)if(!std::isfinite(queryCenter.v[j])||!std::isfinite(queryHalf.v[j]))return false;
    Math m;const auto relative=m.relative(queryWorld,compound.ownerWorld);
    const auto targetCenter=m.transform(relative,compound.center,true);
    V delta{},q[3]{},t[3]{},axis[15]{};
    for(unsigned j=0;j<3;++j){
        delta.v[j]=m.sub(targetCenter.v[j],queryCenter.v[j]);q[j].v[j]=queryHalf.v[j];t[j].v[j]=compound.half.v[j];
        t[j]=m.transform(relative,t[j],false);axis[j]=compound.coarseAxes[j];
        axis[3+j]={{relative.v[4*j],relative.v[4*j+1],relative.v[4*j+2]}};
    }
    for(unsigned index=0;index<15;++index){
        if(index>=6)axis[index]=m.cross(axis[(index-6)/3],axis[3+(index-6)%3]);
        const auto& a=axis[index];
        const float distance=std::fabs(m.dot(delta,a));
        const float qr=m.add(std::fabs(m.dot(q[2],a)),m.add(std::fabs(m.dot(q[0],a)),std::fabs(m.dot(q[1],a))));
        const float tr=m.add(std::fabs(m.dot(t[2],a)),m.add(std::fabs(m.dot(t[0],a)),std::fabs(m.dot(t[1],a))));
        const float radius=m.add(qr,tr);if(!m.valid)return false;
        if(!(distance<=radius)){overlap=false;return true;}
    }
    if(!m.valid)return false;overlap=true;return true;
}
}

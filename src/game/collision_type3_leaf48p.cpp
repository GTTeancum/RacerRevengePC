#include "collision_type3_leaf48p.h"
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
float add(float a,float b) noexcept { volatile float r=a+b;return r; }
float sub(float a,float b) noexcept { volatile float r=a-b;return r; }
float mul(float a,float b) noexcept { volatile float r=a*b;return r; }
float div(float a,float b) noexcept { volatile float r=a/b;return r; }
bool finite(const Vector3_48C& v) noexcept {
    return std::isfinite(v.v[0])&&std::isfinite(v.v[1])&&std::isfinite(v.v[2]);
}
Vector3_48C difference(const Vector3_48C& a,const Vector3_48C& b) noexcept {
    return {{sub(a.v[0],b.v[0]),sub(a.v[1],b.v[1]),sub(a.v[2],b.v[2])}};
}
float dot(const Vector3_48C& a,const Vector3_48C& b) noexcept {
    return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(1.f,mul(a.v[2],b.v[2])));
}
Vector3_48C normalize(const Vector3_48C& a) noexcept {
    const float squared=dot(a,a);
    volatile float root=std::sqrt(std::fabs(squared));
    const float q=squared==0.f ? std::numeric_limits<float>::max():div(1.f,root);
    return {{mul(a.v[0],q),mul(a.v[1],q),mul(a.v[2],q)}};
}
Vector3_48C blend(const Vector3_48C& a,const Vector3_48C& b,float t) noexcept {
    const float s=sub(1.f,t);
    return {{add(mul(t,b.v[0]),mul(s,a.v[0])),add(mul(t,b.v[1]),mul(s,a.v[1])),add(mul(t,b.v[2]),mul(s,a.v[2]))}};
}
// 10E3A0..10EAD4: six ordered scanline branch pairs and edge direction.
bool projectedInside(const Type3Triangle48P& tri,const Vector3_48C& p,
    const Vector3_48C& n) noexcept {
    unsigned x,y;
    if(std::fabs(n.v[2])>std::fabs(n.v[0]) && std::fabs(n.v[2])>std::fabs(n.v[1])) {x=0;y=1;}
    else if(std::fabs(n.v[1])>std::fabs(n.v[0])) {x=0;y=2;}
    else {x=1;y=2;}
    const float a=tri.vertices[0].v[y],b=tri.vertices[1].v[y],c=tri.vertices[2].v[y],q=p.v[y];
    unsigned i0,j0,i1,j1;
    if(q<=a && !(q<b)) {i0=0;j0=1;if(q<=c){i1=1;j1=2;}else{i1=0;j1=2;}}
    else if(q<=b && !(q<a)) {i0=0;j0=1;if(q<=c){i1=0;j1=2;}else{i1=1;j1=2;}}
    else if(q<=c && !(q<b)) {if(q<=a){i0=0;j0=1;i1=1;j1=2;}else{i0=1;j0=2;i1=0;j1=2;}}
    else if(q<=b && !(q<c)) {if(q<=a){i0=0;j0=2;i1=1;j1=2;}else{i0=0;j0=1;i1=1;j1=2;}}
    else if(q<=a && !(q<c)) {if(q<=b){i0=0;j0=2;i1=1;j1=2;}else{i0=0;j0=1;i1=0;j1=2;}}
    else if(q<=c && !(q<a)) {if(q<=b){i0=0;j0=1;i1=0;j1=2;}else{i0=1;j0=2;i1=0;j1=2;}}
    else return false;
    auto intersection=[&](unsigned i,unsigned j) {
        const auto& first=tri.vertices[i];const auto& second=tri.vertices[j];
        return add(first.v[x],div(mul(sub(second.v[x],first.v[x]),sub(q,first.v[y])),sub(second.v[y],first.v[y])));
    };
    const float first=intersection(i0,j0),second=intersection(i1,j1);
    if(p.v[x]<=first && !(p.v[x]<second))return true;
    return p.v[x]<=second && !(p.v[x]<first);
}
bool interpolatedNormal(const Type3Triangle48P& tri,const Vector3_48C& p,
    const Vector3_48C& face,Vector3_48C& normal) noexcept {
    // Original1106B0 chooses X or Z as the first cut (110704), then
    // another coordinate from face normal magnitudes (110A34 / 110EEC).
    const unsigned cut=std::fabs(face.v[0])>std::fabs(face.v[2]) ? 2u:0u;
    const unsigned last=cut==2 ? (std::fabs(face.v[1])>std::fabs(face.v[0])?0u:1u)
                               : (std::fabs(face.v[1])>std::fabs(face.v[2])?2u:1u);
    unsigned edges[3]={};unsigned count=0;
    for(unsigned i=0;i<3;++i) {
        const float a=tri.vertices[i].v[cut],b=tri.vertices[(i+1)%3].v[cut],q=p.v[cut];
        if((a<=q && !(b<=q)) || (!(a<q) && b<q))edges[count++]=i;
    }
    // The original unconditionally reads two edge slots. A missing slot is
    // caller-stack-dependent; do not invent a native value for that case.
    if(count!=2)return false;
    Vector3_48C positions[2],normals[2];
    for(unsigned k=0;k<2;++k) {
        const unsigned i=edges[k],j=(i+1)%3;
        const float t=div(sub(p.v[cut],tri.vertices[i].v[cut]),sub(tri.vertices[j].v[cut],tri.vertices[i].v[cut]));
        positions[k]=blend(tri.vertices[i],tri.vertices[j],t);
        normals[k]=blend(tri.vertexNormals[i],tri.vertexNormals[j],t);
    }
    const float denominator=sub(positions[1].v[last],positions[0].v[last]);
    if(denominator==0.f)return false;
    const float t=div(sub(p.v[last],positions[0].v[last]),denominator);
    normal=normalize(blend(normals[0],normals[1],t));
    return finite(normal);
}
}
namespace {
Vector3_48C xyz(const SpringVector4_48N& v) noexcept {return {{v.v[0],v.v[1],v.v[2]}};}
SpringVector4_48N vec4(const Vector3_48C& v,float w) noexcept {return {{v.v[0],v.v[1],v.v[2],w}};}
}
Type3LeafStatus48P applyType3Triangle48P(const Type3Segment48P& s,
    const Type3Triangle48P& tri,Type3PairLeafState48P& pair,
    Type3SelectedLeafState48P& selected) noexcept {
    if(!finite(xyz(s.start))||!finite(xyz(s.direction))||!std::isfinite(s.length)||
       !std::isfinite(pair.fraction)||!std::isfinite(selected.fraction))return Type3LeafStatus48P::InvalidInput;
    for(const auto& v:tri.vertices)if(!finite(v))return Type3LeafStatus48P::InvalidInput;
    // 10E1A4..10E238 computes the face independently from packed tree planes.
    const auto a=difference(tri.vertices[1],tri.vertices[0]);
    const auto b=difference(tri.vertices[2],tri.vertices[0]);
    Vector3_48C cross{};
    for(unsigned i=0;i<3;++i)cross.v[i]=sub(mul(a.v[(i+1)%3],b.v[(i+2)%3]),mul(b.v[(i+1)%3],a.v[(i+2)%3]));
    const auto face=normalize(cross);
    if(!finite(face))return Type3LeafStatus48P::InvalidInput;
    const float facing=dot(face,xyz(s.direction));
    if(!(facing<0.f)||s.length<=0.f)return Type3LeafStatus48P::Miss;
    const float fraction=div(dot(face,difference(tri.vertices[0],xyz(s.start))),mul(facing,s.length));
    if(!std::isfinite(fraction))return Type3LeafStatus48P::InvalidInput;
    if(fraction<0.f || !(fraction<=1.f))return Type3LeafStatus48P::Miss;
    Vector3_48C point{};
    for(unsigned i=0;i<3;++i)point.v[i]=add(s.start.v[i],mul(s.length,mul(fraction,s.direction.v[i])));
    if(!finite(point))return Type3LeafStatus48P::InvalidInput;
    if(!projectedInside(tri,point,face))return Type3LeafStatus48P::Miss;
    auto p=pair;auto result=selected;
    // 10EAE0: interpolate ONLY on a strict pair improvement or negative prior.
    if(fraction<p.fraction || p.fraction<0.f) {
        p.fraction=fraction;p.point=vec4(point,1.f);p.normal=vec4(face,0.f);
        if(tri.hasVertexNormals&&tri.normalMode) {
            if(tri.normalMode!=1)return Type3LeafStatus48P::UnsupportedNormalMode;
            for(const auto& n:tri.vertexNormals)if(!finite(n))return Type3LeafStatus48P::InvalidInput;
            Vector3_48C normal{};
            if(!interpolatedNormal(tri,point,face,normal))return Type3LeafStatus48P::UnsupportedInterpolationBoundary;
            p.normal=vec4(normal,1.f);
        }
    }
    p.hit=1;++result.hitCount;
    // 10EC40 compares the retained pair fraction, not this triangle's fraction.
    if(p.fraction<result.fraction) {
        if(!finite(xyz(p.normal)))return Type3LeafStatus48P::InvalidInput;
        result.fraction=p.fraction;result.normal=p.normal;result.point=p.point;
        const float remaining=sub(1.f,p.fraction);
        Vector3_48C delta{};
        for(unsigned i=0;i<3;++i)delta.v[i]=mul(remaining,s.direction.v[i]);
        const float projection=dot(delta,xyz(p.normal));
        for(unsigned i=0;i<3;++i)result.correction.v[i]=mul(projection,p.normal.v[i]);
        result.correction.v[3]=1.f;
        if(!finite(xyz(result.correction)))return Type3LeafStatus48P::InvalidInput;
    }
    pair=p;selected=result;
    return Type3LeafStatus48P::Hit;
}
}

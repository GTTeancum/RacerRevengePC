#pragma once
#include "type2_resource49x.h"
#include "terrain_query48j.h"
#include <algorithm>
#include <cmath>

namespace rr::game {
// Explicit native compatibility policy, NOT the original108E90 leaf decoder.
// Uses the actual loaded type2 endpoint pairs, transformed by the current
// canonical query matrix. Optional continuous coverage is the two-triangle
// surface swept between previous accepted/current endpoints. It approximates
// rotating rigid motion by linear endpoint motion; collection/filter admission
// remains the caller's responsibility. No camera, route or stall inputs exist.
namespace native_gate_geometry50kg_detail {
struct V {double x,y,z;};
inline V sub(V a,V b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline V add(V a,V b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline V scale(V a,double s) noexcept {return {a.x*s,a.y*s,a.z*s};}
inline double dot(V a,V b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline V cross(V a,V b) noexcept {return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline double segmentSquared(V p,V a,V b) noexcept {
 const auto ab=sub(b,a);const double length=dot(ab,ab);
 const double t=length>0?std::clamp(dot(sub(p,a),ab)/length,0.0,1.0):0.0;
 const auto d=sub(p,add(a,scale(ab,t)));return dot(d,d);
}
inline double triangleSquared(V p,V a,V b,V c) noexcept {
 // Projection is admitted only inside the triangle. Degenerate triangles
 // reduce to their actual edges rather than inventing width or an epsilon.
 const auto ab=sub(b,a),ac=sub(c,a),n=cross(ab,ac);const double nn=dot(n,n);
 double result=std::min({segmentSquared(p,a,b),segmentSquared(p,b,c),segmentSquared(p,c,a)});
 if(nn>0){
  const double signedHeight=dot(sub(p,a),n);const auto q=sub(p,scale(n,signedHeight/nn));
  if(dot(cross(ab,sub(q,a)),n)>=0&&dot(cross(sub(c,b),sub(q,b)),n)>=0&&
     dot(cross(sub(a,c),sub(q,c)),n)>=0)result=std::min(result,signedHeight*signedHeight/nn);
 }
 return result;
}
inline bool affine(const BodyMatrix48G& m) noexcept {
 for(float x:m.v)if(!std::isfinite(x))return false;
 return m.v[3]==0&&m.v[7]==0&&m.v[11]==0&&m.v[15]==1;
}
inline bool point(const BodyMatrix48G& m,const SpringVector4_48N& p,V& out) noexcept {
 float v[3];
 for(unsigned i=0;i<3;++i){
  if(!std::isfinite(p.v[i]))return false;
  volatile float x=m.v[i]*p.v[0],y=m.v[4+i]*p.v[1],z=m.v[8+i]*p.v[2];
  volatile float xy=x+y,xyz=xy+z,w=xyz+m.v[12+i];v[i]=w;
  if(!std::isfinite(v[i]))return false;
 }
 out={v[0],v[1],v[2]};return true;
}
}
inline bool nativeType2GateOverlap50KG(const Type2ResourceView49X& shape,
 const BodyMatrix48G& current,const BodyMatrix48G* previous,
 const CollisionSphere48J& sphere,bool& overlap) noexcept {
 using namespace native_gate_geometry50kg_detail;
 if(!shape.rays||!shape.rayCount||shape.rayCount>32768||!affine(current)||
    (previous&&!affine(*previous))||!std::isfinite(sphere.radius)||sphere.radius<=0)return false;
 for(float x:sphere.center.v)if(!std::isfinite(x))return false;
 const V center{sphere.center.v[0],sphere.center.v[1],sphere.center.v[2]};
 const double radiusSquared=double(sphere.radius)*sphere.radius;
 bool found=false;
 for(std::size_t i=0;i<shape.rayCount;++i){
  const auto& ray=shape.rays[i];V a{},b{};
  if(!point(current,ray.first,a)||!point(current,ray.second,b))return false;
  double distance=segmentSquared(center,a,b);
  if(previous){V pa{},pb{};if(!point(*previous,ray.first,pa)||!point(*previous,ray.second,pb))return false;
   distance=std::min({distance,segmentSquared(center,pa,pb),
       triangleSquared(center,pa,pb,b),triangleSquared(center,pa,b,a)});
  }
  if(!std::isfinite(distance))return false;
  found=found||distance<=radiusSquared;
 }
 overlap=found;return true;
}
}

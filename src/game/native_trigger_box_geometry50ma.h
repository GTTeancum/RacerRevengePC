#pragma once
#include "native_gate_geometry50kg.h"
#include <array>
#include <cmath>

namespace rr::game {
// Source shape6: matrix00, local center40 and authored local half50. The Box
// field is copied unchanged by11C260, not divided by2. Original2A04B0 builds
// its basis from the parsed look/up. This is a disclosed native geometry
// policy, NOT retail108E90,107xxx, original trig or VU arithmetic parity.
struct NativeTriggerBox50MA {
 BodyMatrix48G world{};
 Vector3_48C center{},half{};
};
namespace native_trigger_box50ma_detail {
using namespace native_gate_geometry50kg_detail;
inline bool finite(V p)noexcept{return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline V vector(const Vector3_48C& p)noexcept{return {p.v[0],p.v[1],p.v[2]};}
struct Inverse {V row[3]{},translation{},center{},half{};};
inline bool inverse(const NativeTriggerBox50MA& box,Inverse& out)noexcept {
 if(!affine(box.world))return false;
 out.center=vector(box.center);out.half=vector(box.half);
 if(!finite(out.center)||!finite(out.half)||out.half.x<=0||out.half.y<=0||out.half.z<=0)return false;
 const auto& m=box.world;
 const V a{m.v[0],m.v[1],m.v[2]},b{m.v[4],m.v[5],m.v[6]},c{m.v[8],m.v[9],m.v[10]};
 // Authored rigid OBB basis only, with a small representation tolerance.
 // This bounds inverse arithmetic without admitting an ill-conditioned shear
 // or silently repairing arbitrary caller matrices.
 if(std::fabs(dot(a,a)-1.0)>0.0001||std::fabs(dot(b,b)-1.0)>0.0001||std::fabs(dot(c,c)-1.0)>0.0001||
    std::fabs(dot(a,b))>0.0001||std::fabs(dot(a,c))>0.0001||std::fabs(dot(b,c))>0.0001)return false;
 const double determinant=dot(a,cross(b,c));
 if(!std::isfinite(determinant)||determinant==0)return false;
 out.row[0]=scale(cross(b,c),1.0/determinant);
 out.row[1]=scale(cross(c,a),1.0/determinant);
 out.row[2]=scale(cross(a,b),1.0/determinant);
 out.translation={m.v[12],m.v[13],m.v[14]};
 return finite(out.row[0])&&finite(out.row[1])&&finite(out.row[2]);
}
inline bool local(const Inverse& inverse,V p,V& out)noexcept {
 const auto d=sub(p,inverse.translation);
 out=sub({dot(inverse.row[0],d),dot(inverse.row[1],d),dot(inverse.row[2],d)},inverse.center);
 return finite(out);
}
inline bool segment(V a,V b,V half)noexcept {
 double lo=0,hi=1;const double av[3]={a.x,a.y,a.z},bv[3]={b.x,b.y,b.z},h[3]={half.x,half.y,half.z};
 for(unsigned k=0;k<3;++k){const double d=bv[k]-av[k];
  if(d==0){if(av[k]<-h[k]||av[k]>h[k])return false;continue;}
  double x=(-h[k]-av[k])/d,y=(h[k]-av[k])/d;
  if(x>y)std::swap(x,y);lo=std::max(lo,x);hi=std::min(hi,y);
  if(lo>hi)return false;
 }
 return true;
}
inline bool separated(V axis,V a,V b,V c,V half)noexcept {
 const double x=dot(axis,a),y=dot(axis,b),z=dot(axis,c);
 const double radius=std::fabs(axis.x)*half.x+std::fabs(axis.y)*half.y+std::fabs(axis.z)*half.z;
 return std::min({x,y,z})>radius||std::max({x,y,z})<-radius;
}
inline bool triangle(V a,V b,V c,V half)noexcept {
 // Full triangle/AABB SAT in authored box coordinates. Zero axes from a
 // degenerate triangle reject nothing; the remaining axes cover its edges.
 const V unit[3]={{1,0,0},{0,1,0},{0,0,1}},edges[3]={sub(b,a),sub(c,b),sub(a,c)};
 for(const auto axis:unit)if(separated(axis,a,b,c,half))return false;
 if(separated(cross(edges[0],edges[1]),a,b,c,half))return false;
 for(const auto edge:edges)for(const auto axis:unit)if(separated(cross(edge,axis),a,b,c,half))return false;
 return true;
}
}
inline bool validNativeTriggerBox50MA(const NativeTriggerBox50MA& box)noexcept {
 native_trigger_box50ma_detail::Inverse inverse{};
 return native_trigger_box50ma_detail::inverse(box,inverse);
}
// Same admitted loaded type2 endpoint pairs as native Gate spheres. Current
// segments plus optional previous segments and two triangles per swept pair;
// rotating motion is approximated by linear endpoint motion. No extra width,
// epsilon, center-distance hit, root-box hit, route or time input is introduced.
inline bool nativeType2TriggerBoxOverlap50MA(const Type2ResourceView49X& shape,
 const BodyMatrix48G& current,const BodyMatrix48G* previous,
 const NativeTriggerBox50MA& box,bool& overlap)noexcept {
 using namespace native_trigger_box50ma_detail;
 Inverse inv{};
 if(!shape.rays||!shape.rayCount||shape.rayCount>32768||!affine(current)||
    (previous&&!affine(*previous))||!inverse(box,inv))return false;
 bool found=false;
 for(std::size_t i=0;i<shape.rayCount;++i){const auto& ray=shape.rays[i];V a{},b{},wa{},wb{};
  if(!point(current,ray.first,wa)||!point(current,ray.second,wb)||!local(inv,wa,a)||!local(inv,wb,b))return false;
  bool hit=segment(a,b,inv.half);
  if(previous){V pa{},pb{};
   if(!point(*previous,ray.first,wa)||!point(*previous,ray.second,wb)||!local(inv,wa,pa)||!local(inv,wb,pb))return false;
   hit=hit||segment(pa,pb,inv.half)||triangle(pa,pb,b,inv.half)||triangle(pa,b,a,inv.half);
  }
  found=found||hit;
 }
 overlap=found;return true;
}
} // namespace rr::game

namespace rr::game {
// Disclosed native box6 lifecycle coarse observation only: genuine prepared
// type2 root against the authored OBB, full parallelepiped SAT. This may keep
// an existing pair alive; it NEVER admits a new trigger contact or C80 write.
// Boundary is inclusive. No claim of original122BA0 exceptional-float parity.
inline bool nativeType2RootTriggerBoxOverlap50MA(const Type2ResourceView49X& shape,
 const BodyMatrix48G& world,const NativeTriggerBox50MA& box,bool& overlap)noexcept {
 using namespace native_trigger_box50ma_detail;Inverse inv{};
 if(!shape.nodes||!shape.nodeCount||(shape.root&type2Leaf49X)||shape.root>=shape.nodeCount||!affine(world)||!inverse(box,inv))return false;
 const auto& root=shape.nodes[shape.root];
 for(unsigned k=0;k<3;++k)if(!std::isfinite(root.center[k])||!std::isfinite(root.half[k])||root.half[k]<0.f)return false;
 V corners[8]{};
 for(unsigned bits=0;bits<8;++bits){SpringVector4_48N p{};p.v[3]=1.f;
  for(unsigned k=0;k<3;++k)p.v[k]=root.center[k]+root.half[k]*((bits&(1u<<k))?1.f:-1.f);
  V w{};if(!point(world,p,w)||!local(inv,w,corners[bits]))return false;
 }
 const V unit[3]={{1,0,0},{0,1,0},{0,0,1}};
 const V edge[3]={sub(corners[1],corners[0]),sub(corners[2],corners[0]),sub(corners[4],corners[0])};
 const auto separate=[&](V axis)noexcept{
  const double radius=std::fabs(axis.x)*inv.half.x+std::fabs(axis.y)*inv.half.y+std::fabs(axis.z)*inv.half.z;
  double lo=dot(corners[0],axis),hi=lo;
  for(unsigned i=1;i<8;++i){const double p=dot(corners[i],axis);lo=std::min(lo,p);hi=std::max(hi,p);}
  return lo>radius||hi<-radius;
 };
 bool separated=false;
 for(const auto axis:unit)separated=separated||separate(axis);
 for(unsigned i=0;i<3;++i)separated=separated||separate(cross(edge[i],edge[(i+1)%3]));
 for(const auto e:edge)for(const auto axis:unit)separated=separated||separate(cross(e,axis));
 overlap=!separated;return true;
}
} // namespace rr::game

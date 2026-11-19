#pragma once
#include "../../src/game/body_scene48g.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>

namespace tb_door_sphere50TB {
namespace g=rr::game;
struct Row {g::BodyMatrix48G world{};std::array<float,3> center{};float radius=0;};
struct Result {std::array<float,3> center160{};float radius198=0;};
namespace detail {
struct Math {
 bool valid=true;
 float keep(float v)noexcept{valid=valid&&std::isfinite(v);return v;}
 float add(float a,float b)noexcept{volatile float v=a+b;return keep(v);}
 float sub(float a,float b)noexcept{volatile float v=a-b;return keep(v);}
 float mul(float a,float b)noexcept{volatile float v=a*b;return keep(v);}
 std::array<float,3> point(const std::array<float,3>& p,const g::BodyMatrix48G& m)noexcept{
  std::array<float,3> out{};
  for(unsigned j=0;j<3;++j){const float x=mul(m.v[j],p[0]),y=mul(m.v[4+j],p[1]);
   const float xy=add(x,y),z=mul(m.v[8+j],p[2]);out[j]=add(add(xy,z),m.v[12+j]);}
  return out;
 }
};
inline bool affine(const g::BodyMatrix48G& m)noexcept {
 for(float v:m.v)if(!std::isfinite(v))return false;
 return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
}
// Typed native projection of26F4B0 bit10/28D940 for the actual TB root and
// three source rows. Caller supplies authenticated PSG centers/radius194 and
// refreshed actual node worlds, in source order. No geometry-derived bounds.
// It does not create a model, observer or membership. Output is transactional;
// PS2 VU accumulator/status parity is not claimed by native binary32 arithmetic.
inline bool compute(const g::BodyMatrix48G& root,const std::array<Row,3>& rows,Result& output)noexcept{
 if(std::fegetround()!=FE_TONEAREST||!detail::affine(root))return false;
 // Actual TB source has no authored orientation and exact zero offset. Do
 // not silently generalize this source admission to other root transforms.
 for(unsigned i=0;i<16;++i)if(root.v[i]!=(i%5==0?1.f:0.f))return false;
 for(const auto& row:rows){if(!detail::affine(row.world)||!std::isfinite(row.radius)||row.radius<0.f)return false;
  for(float v:row.center)if(!std::isfinite(v))return false;}
 detail::Math m;std::array<float,3> center{};float radius=0.f;
 for(const auto& row:rows){const auto point=m.point(row.center,row.world);
  if(radius<=0.f){center=point;radius=row.radius;}
  else {const float dx=std::fabs(m.sub(point[0],center[0]));const float dy=std::fabs(m.sub(point[1],center[1]));
   float distance=dy<=dx?dx:dy;const float dz=std::fabs(m.sub(point[2],center[2]));if(!(dz<=distance))distance=dz;
   const float candidate=m.add(distance,row.radius);if(!(candidate<=radius))radius=candidate;}
 }
 //28BD50 builds the inverse matrix first: transpose canonicalized root rows,
 // separately compute negative translation, THEN apply the point operation.
 // This is not reassociated into (point-translation)*rotation.
 g::BodyMatrix48G clean=root,inverse{};for(float& v:clean.v)v=m.add(v,0.f);
 for(unsigned i=0;i<3;++i)for(unsigned j=0;j<3;++j)inverse.v[i*4+j]=clean.v[j*4+i];
 inverse.v[15]=1.f;
 for(unsigned j=0;j<3;++j){const float x=m.mul(inverse.v[j],clean.v[12]),y=m.mul(inverse.v[4+j],clean.v[13]);
  const float xy=m.add(x,y),z=m.mul(inverse.v[8+j],clean.v[14]);inverse.v[12+j]=m.mul(m.add(xy,z),-1.f);}
 Result next{m.point(center,inverse),radius};if(!m.valid)return false;output=next;return true;
}
}

#pragma once
#include "../../src/game/body_scene48g.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>

namespace mc_animation_sphere50MC {
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
 //107140 and28BEB0..BC: ordered XYZ vector multiply/accumulate then
 //translation add. Every native operation rounds separately to binary32.
 std::array<float,3> point(const std::array<float,3>& p,const g::BodyMatrix48G& matrix)noexcept{
  std::array<float,3> out{};
  for(unsigned c=0;c<3;++c)out[c]=mul(matrix.v[c],p[0]);
  for(unsigned c=0;c<3;++c)out[c]=add(out[c],mul(matrix.v[4+c],p[1]));
  for(unsigned c=0;c<3;++c)out[c]=add(out[c],mul(matrix.v[8+c],p[2]));
  for(unsigned c=0;c<3;++c)out[c]=add(out[c],matrix.v[12+c]);
  return out;
 }
};
inline bool affine(const g::BodyMatrix48G& m)noexcept{
 for(float v:m.v)if(!std::isfinite(v))return false;
 return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
inline bool actualRoot(const g::BodyMatrix48G& root,std::size_t rows)noexcept{
 if(!affine(root))return false;
 // All14 exact MC TOB Offset values are0,0,0 and omit orientation.
 // Bridge PSG has2 descendant rows; each of the13 rays has5. Their distinct
 // animated placements live in MOT positions, NOT the separate model root.
 for(unsigned i=0;i<16;++i)if(root.v[i]!=(i%5==0?1.f:0.f))return false;
 return rows==2||rows==5;
}
inline g::BodyMatrix48G inverse28BD50(const g::BodyMatrix48G& root,Math& m)noexcept{
 g::BodyMatrix48G inverse=root;
 //28BE38..44 first forms add-zero copies of ALL basis/fourth lanes,
 //including those overwritten by the six ordered transpose lane moves.
 for(unsigned row=0;row<3;++row)for(unsigned c=0;c<4;++c)
  inverse.v[4*row+c]=m.add(root.v[4*row+c],0.f);
 inverse.v[4]=m.add(0.f,root.v[1]); //28BE48
 inverse.v[8]=m.add(0.f,root.v[2]); //28BE4C
 inverse.v[9]=m.add(0.f,root.v[6]); //28BE50
 inverse.v[1]=m.add(0.f,root.v[4]); //28BE54
 inverse.v[2]=m.add(0.f,root.v[8]); //28BE58
 inverse.v[6]=m.add(0.f,root.v[9]); //28BE5C
 for(unsigned c=0;c<4;++c)inverse.v[12+c]=m.add(root.v[12+c],0.f); //28BE68
 std::array<float,3> translation{};
 for(unsigned c=0;c<3;++c)translation[c]=m.mul(inverse.v[c],root.v[12]); //28BE6C
 for(unsigned c=0;c<3;++c)translation[c]=m.add(translation[c],m.mul(inverse.v[4+c],root.v[13])); //28BE70
 for(unsigned c=0;c<3;++c)translation[c]=m.add(translation[c],m.mul(inverse.v[8+c],root.v[14])); //28BE74
 const float negativeOne=m.sub(0.f,1.f); //28BE78..7C under finite surviving VU scratch
 for(unsigned c=0;c<3;++c)inverse.v[12+c]=m.mul(translation[c],negativeOne); //28BE80
 return inverse;
}
}
//Proposed typed native projection of26F4B0 bit0x10 /28D940 for exactly the
//14 MC source models. Their model root flags0x98 omit own geometry bit4;
//all actual PSG rows are eligible0xA4. Caller supplies EVERY retained PSG
//bounds[0..2]/bounds[6] and its refreshed actual world, in source order.
//The root-inclusive scene indices, collision members and rendered surface
//bounds are not substituted for this PSG enumeration.
//
//This leaf owns no graph/cache, model, observer or membership. It requires
//strict native FP (nearest rounding, no fast-math/contraction), finite source
//inputs and finite surviving source VU scratch for self-subtraction zeroing.
//It preserves the admitted binary32 operation policy, not EE/VU accumulator,
//denormal, status, exception or arbitrary poisoned-register equivalence.
//Output commits only on success. Two/five rows do not confer live readiness.
inline bool compute(const g::BodyMatrix48G& root,const Row* rows,
                    std::size_t rowCount,Result& output)noexcept{
 if(std::fegetround()!=FE_TONEAREST||!rows||!detail::actualRoot(root,rowCount))return false;
 for(std::size_t i=0;i<rowCount;++i){const auto& row=rows[i];
  if(!detail::affine(row.world)||!std::isfinite(row.radius)||row.radius<0.f)return false;
  for(float v:row.center)if(!std::isfinite(v))return false;
 }
 detail::Math m;std::array<float,3> center{};float radius=0.f;
 //26F548..580 passes each source descendant to28D940 without recursion.
 for(std::size_t i=0;i<rowCount;++i){const auto& row=rows[i];
  const auto point=m.point(row.center,row.world);
  if(radius<=0.f){center=point;radius=row.radius;} //28DA64..90
  else{
   const float dx=std::fabs(m.sub(point[0],center[0])); //28D9C8..D8
   const float dy=std::fabs(m.sub(point[1],center[1])); //28D9DC..F4
   float distance=dy<=dx?dx:dy; //28D9F8..DA0C
   const float dz=std::fabs(m.sub(point[2],center[2])); //28DA10..24
   if(!(dz<=distance))distance=dz; //28DA28..3C
   const float candidate=m.add(distance,row.radius); //28DA40..48
   if(!(candidate<=radius))radius=candidate; //28DA4C..60
  }
 }
 //26F588 calls28BD50 AFTER the world-space fold. Build the full ordered
 //inverse first, including negative translation, THEN apply the point.
 //Do not replace this with an AABB or reassociate to (center-translation).
 const auto inverse=detail::inverse28BD50(root,m);
 const Result next{m.point(center,inverse),radius};
 if(!m.valid)return false;
 output=next;return true; //26F598 center160;26F5A0..C4 half170/174/178/radius198
}
template<std::size_t N>
inline bool compute(const g::BodyMatrix48G& root,const std::array<Row,N>& rows,
                    Result& output)noexcept{
 return compute(root,rows.data(),rows.size(),output);
}
}

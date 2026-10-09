#pragma once
#include "../../src/assets/model_geometry49o.h"
#include "../../src/game/source_acos49t.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

// Cold original-MOT v1 loader projection. The original152570 loader converts
// legacy float quaternion keys through145A20->100D30; it DOES NOT retain them
// as float-quaternion interpolation tracks. Keep the original bytes separately.
namespace original_legacy_motion50IG {
namespace detail {
inline void need(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
inline std::uint32_t u32(const std::uint8_t* p){return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
inline float value(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
inline std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
inline float read(const std::uint8_t* p){return value(u32(p));}
inline float add(float a,float b){volatile float x=a+b;need(std::isfinite(x),"legacy MOT addition nonfinite");return x;}
inline float mul(float a,float b){volatile float x=a*b;need(std::isfinite(x),"legacy MOT product nonfinite");return x;}
inline float div(float a,float b){need(b!=0.f,"legacy MOT zero divisor");volatile float x=a/b;need(std::isfinite(x),"legacy MOT quotient nonfinite");return x;}
inline std::uint8_t byte(float f){need(std::isfinite(f)&&f>-2147483648.f&&f<2147483648.f,"legacy MOT integer overflow");return static_cast<std::uint8_t>(static_cast<std::int32_t>(f));}
inline void put(std::vector<std::uint8_t>& v,std::uint32_t x){for(unsigned i=0;i<4;++i)v.push_back(static_cast<std::uint8_t>(x>>(i*8)));}
inline std::array<std::uint8_t,4> convert(const std::uint8_t* p,rr::game::SourceLibmState49T& libm){
 //15298C..152A24 reads time, XYZ, W, then passes WXYZ to145A20.
 const std::array<float,3> xyz{read(p+4),read(p+8),read(p+12)};
 const float w=read(p+16);
 need(std::isfinite(w)&&w>=-1.f&&w<=1.f,"legacy MOT quaternion scalar outside source acos domain");
 for(const auto f:xyz)need(std::isfinite(f),"legacy MOT quaternion nonfinite");
 std::uint32_t angleBits=0;
 need(rr::game::sourceAcos49T(bits(w),libm,angleBits),"legacy MOT source acos state rejected");
 const float angle=mul(2.f,value(angleBits));std::array<float,3> axis{};
 if(angle==0.f)axis={1.f,0.f,0.f};
 else {
  std::uint32_t sineBits=0;
  need(rr::game::sourceSinePositiveReduced50IG(bits(mul(.5f,angle)),sineBits),
      "legacy MOT half-angle exceeds recovered positive sine domain");
  const float inverse=div(1.f,value(sineBits));
  for(unsigned i=0;i<3;++i)axis[i]=mul(xyz[i],inverse);
 }
 //100D64..100DCC: ADDA of x*x,y*y then MADD z*z and scalar RSQRT.
 // Source's zero-axis shared fallback remains unjoined, not invented here.
 const float square=add(add(mul(axis[0],axis[0]),mul(axis[1],axis[1])),mul(axis[2],axis[2]));
 need(square!=0.f,"legacy MOT zero-axis shared fallback is not recovered");
 volatile float inverse=(bits(square)&0x7f800000u)==0?value(0x7f7fffffu):
     static_cast<float>(1.0/std::sqrt(static_cast<double>(square)));
 need(std::isfinite(inverse),"legacy MOT scalar reciprocal root nonfinite");
 for(auto& f:axis)f=mul(f,inverse);
 //100DD0..100E7C: exact separate binary32 operations, +.5 then EE signed
 // truncation, retaining the low byte. Negative axes are intentionally biased.
 std::array<std::uint8_t,4> out{};
 out[0]=byte(add(.5f,div(mul(255.f,angle),value(0x40c90fdbu))));
 for(unsigned i=0;i<3;++i)out[i+1]=byte(add(.5f,mul(127.f,axis[i])));
 return out;
}
}
// Input remains original and is never rewritten. Output is the exact bounded
// runtime key representation, with synthetic -2 serialization only as a
// neutral bridge into the existing49O view reader; it is NOT original bytes.
// Original identity must be checked by the resource owner before this call.
inline bool convert(const std::vector<std::uint8_t>& original,
    rr::game::SourceLibmState49T& libm,std::vector<std::uint8_t>& runtime,std::string& error){
 try {using namespace detail;
  need(std::fegetround()==FE_TONEAREST,"legacy MOT requires nearest binary32 arithmetic");
  need(original.size()>=24&&original.size()<=rr::assets::maxMotBytes49O,"legacy MOT byte bounds");
  const auto frames=u32(original.data());
  need(frames&&frames<=rr::assets::maxMotKeysPerTrack49O,"legacy MOT version1 frame bounds");
  need(u32(original.data()+8)==0&&u32(original.data()+12)==1,"legacy MOT admits only rotation0/Bezier1");
  const auto positions=u32(original.data()+16),rotations=u32(original.data()+20);
  need(positions<=rr::assets::maxMotTracks49O&&rotations<=rr::assets::maxMotTracks49O,"legacy MOT track bounds");
  auto nextLibm=libm;std::vector<std::uint8_t> next;next.reserve(original.size()+4);
  put(next,0xfffffffeu);next.insert(next.end(),original.begin(),original.begin()+24);
  std::size_t at=24;
  for(unsigned channel=0;channel<2;++channel){const auto count=channel?rotations:positions;
   for(std::uint32_t i=0;i<count;++i){
    need(at<=original.size()&&8<=original.size()-at,"legacy MOT track header truncated");
    const auto keys=u32(original.data()+at),row=u32(original.data()+at+4);
    need(keys&&keys<=rr::assets::maxMotKeysPerTrack49O&&row<rr::assets::maxModelNodes49O,"legacy MOT key/row bounds");
    put(next,keys);put(next,row);at+=8;
    const std::size_t stride=channel?20:40;
    need(at<=original.size()&&std::size_t(keys)*stride<=original.size()-at,"legacy MOT key data truncated");
    for(std::uint32_t k=0;k<keys;++k){const auto* key=original.data()+at;
     if(channel){put(next,u32(key));const auto packed=detail::convert(key,nextLibm);next.insert(next.end(),packed.begin(),packed.end());}
     else next.insert(next.end(),key,key+40);
     at+=stride;
    }
   }
  }
  need(at==original.size(),"legacy MOT unconsumed original suffix");
  rr::assets::MotionView49O checked{};rr::assets::GeometryDiagnostic49O diagnostic{};
  need(rr::assets::readMot49O(next.data(),next.size(),checked,diagnostic),"legacy MOT converted runtime representation rejected");
  runtime=std::move(next);libm=nextLibm;error.clear();return true;
 }catch(const std::exception& e){error=e.what();return false;}
}
} // namespace original_legacy_motion50IG

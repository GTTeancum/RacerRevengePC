#pragma once
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>

namespace rr::game::original_gb_light_flash50GB {
// Numerical leaf only. The typed GB lighting owner authenticates all authoring
// and owns publication; these scalar structs confer no source/runtime authority.
namespace detail {
inline float add(float a,float b) noexcept {volatile float r=a+b;return r;}
inline float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
inline float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
inline float div(float a,float b) noexcept {volatile float r=a/b;return r;}
inline bool scalar(float x) noexcept {return std::isfinite(x)&&std::fpclassify(x)!=FP_SUBNORMAL;}
inline bool arithmetic() noexcept {return std::fegetround()==FE_TONEAREST;}
inline float randomUnit(std::uint32_t& raw) noexcept {
 raw=raw*0x41c64e6du+0x3039u;
 volatile float converted=static_cast<float>(raw&0x7fffffffu);
 return mul(converted,0x1p-31f);
}
inline bool packed(const std::array<float,4>& color,std::uint32_t& result) noexcept {
 std::array<std::uint32_t,4> c{};
 for(unsigned i=0;i<4;++i){
  if(!scalar(color[i])||color[i]<0.f||color[i]>255.f)return false;
  const float rounded=std::nearbyint(color[i]);c[i]=static_cast<std::uint32_t>(rounded);
 }
 result=(c[3]<<24)|(c[0]<<16)|(c[1]<<8)|c[2];return true;
}
}
struct AmbientState {
 float elapsed=0.f,idleThreshold=25.f,endThreshold=.24f;
};
//2754E0 publishes using OLD thresholds, then rolls the cycle and increments.
// Actual GB defaults are source-authored, including the initial .24 end value.
inline bool advanceAmbient(AmbientState& state,float delta,std::uint32_t& sharedRaw,
 bool& publishFlash) noexcept {
 using namespace detail;
 // Bound the native fixed-step domain; no private clock or catch-up loop.
 if(!arithmetic()||!scalar(delta)||delta<0.f||delta>1.f||
    !scalar(state.elapsed)||state.elapsed<0.f||!scalar(state.idleThreshold)||
    state.idleThreshold<15.f||state.idleThreshold>25.f||
    !scalar(state.endThreshold)||state.endThreshold<0.f)return false;
 auto next=state;auto raw=sharedRaw;const bool flash=state.elapsed>state.idleThreshold;
 if(state.elapsed>state.endThreshold){
  next.elapsed=0.f;
  const float idle=mul(randomUnit(raw),25.f);
  next.idleThreshold=idle>15.f?idle:15.f; //2755A4 MAX.S.
  const float duration=mul(randomUnit(raw),.24f);
  const float bounded=duration<.2f?duration:.2f; //2755E0 MIN.S.
  next.endThreshold=add(bounded,next.idleThreshold);
 }
 next.elapsed=add(next.elapsed,delta);
 if(!scalar(next.elapsed)||!scalar(next.endThreshold))return false;
 state=next;sharedRaw=raw;publishFlash=flash;return true;
}
struct PointState {
 std::uint32_t pattern=0,originalPacked=0;
 float period=2.f,elapsed=0.f,filter=.1f;
};
inline bool initializePoint(std::uint32_t pattern,const std::array<float,4>& color,
 PointState& output) noexcept {
 if(!detail::arithmetic())return false;PointState next;next.pattern=pattern;
 if(!detail::packed(color,next.originalPacked))return false;output=next;return true;
}
//147650: same original operation order and low5-bit SRLV behavior. The caller
// publishes the resulting color to the actual emitted point. Original146C40
// also updates an attached flare; native flare ownership is a separate join.
inline bool advancePoint(PointState& state,float delta,const std::array<float,4>& current,
 std::array<float,4>& output) noexcept {
 using namespace detail;
 if(!arithmetic()||!scalar(delta)||delta<0.f||delta>1.f||state.period!=2.f||state.filter!=.1f||
    !scalar(state.elapsed)||state.elapsed<0.f||state.elapsed>state.period)return false;
 // Original all-one pattern takes the no-service branch, before time changes.
 if(state.pattern==0xffffffffu){output=current;return true;}
 std::uint32_t currentPacked=0;if(!packed(current,currentPacked))return false;
 auto next=state;next.elapsed=add(next.elapsed,delta);
 while(next.elapsed>next.period)next.elapsed=sub(next.elapsed,next.period);
 const float frame=div(mul(33.f,next.elapsed),next.period);
 if(!scalar(frame)||frame<0.f||frame>33.f)return false;
 const auto shift=static_cast<std::uint32_t>(std::nearbyint(frame))&31u;
 const auto target=((next.pattern>>shift)&1u)?next.originalPacked:0u;
 std::array<float,4> color{};constexpr unsigned shifts[]{16,8,0,24};
 for(unsigned i=0;i<4;++i){
  const float old=static_cast<float>((currentPacked>>shifts[i])&255u);
  const float wanted=static_cast<float>((target>>shifts[i])&255u);
  color[i]=add(mul(next.filter,old),mul(sub(1.f,next.filter),wanted));
  if(!scalar(color[i]))return false;
 }
 state=next;output=color;return true;
}
}

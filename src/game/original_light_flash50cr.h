#pragma once
// Timed track lights, numerical leaf (common rewrite R6; replaces the GB-only
// original_gb_light_flash50gb.h). Every value comes from the track's SCB:
//  [Environment] AmbientColorFlashMinIdleTime / IdleTime / MinDuration /
//  Duration / ColorRGB (270E9C enables, 2754E0 services), and per light
//  HasFlasher / FlashPattern / FlashPeriod / FlashFilter (2779E4 reads,
//  147650 services). The lighting owner reads the authoring and owns
//  publication; these scalar structs confer no source/runtime authority.
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <string>
#include <type_traits>

namespace rr::game::original_light_flash50CR {
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
// [Environment] AmbientColorFlash* as authored (stock GB: 15 / 25 / .2 / .24).
struct AmbientTimings {
 float minIdle=0.f,idle=0.f,minDuration=0.f,duration=0.f;
};
inline bool validTimings(const AmbientTimings& t) noexcept {
 using namespace detail;
 return scalar(t.minIdle)&&scalar(t.idle)&&scalar(t.minDuration)&&scalar(t.duration)&&
  t.minIdle>=0.f&&t.idle>0.f&&t.minDuration>=0.f&&t.duration>=0.f;
}
struct AmbientState {
 AmbientTimings timings{};
 // The initial cycle uses the authored IdleTime and Duration directly.
 float elapsed=0.f,idleThreshold=0.f,endThreshold=0.f;
};
inline bool initializeAmbient(const AmbientTimings& t,AmbientState& out) noexcept {
 if(!detail::arithmetic()||!validTimings(t))return false;
 AmbientState next;next.timings=t;next.idleThreshold=t.idle;next.endThreshold=t.duration;
 out=next;return true;
}
//2754E0 publishes using OLD thresholds, then rolls the cycle and increments.
inline bool advanceAmbient(AmbientState& state,float delta,std::uint32_t& sharedRaw,
 bool& publishFlash) noexcept {
 using namespace detail;
 const auto& t=state.timings;
 // Bound the native fixed-step domain; no private clock or catch-up loop.
 const float highestIdle=t.idle>t.minIdle?t.idle:t.minIdle;
 if(!arithmetic()||!validTimings(t)||!scalar(delta)||delta<0.f||delta>1.f||
    !scalar(state.elapsed)||state.elapsed<0.f||!scalar(state.idleThreshold)||
    state.idleThreshold<0.f||state.idleThreshold>highestIdle||
    !scalar(state.endThreshold)||state.endThreshold<0.f)return false;
 auto next=state;auto raw=sharedRaw;const bool flash=state.elapsed>state.idleThreshold;
 if(state.elapsed>state.endThreshold){
  next.elapsed=0.f;
  const float idle=mul(randomUnit(raw),t.idle);
  next.idleThreshold=idle>t.minIdle?idle:t.minIdle;          //2755A4 MAX.S.
  const float duration=mul(randomUnit(raw),t.duration);
  const float bounded=duration<t.minDuration?duration:t.minDuration; //2755E0 MIN.S.
  next.endThreshold=add(bounded,next.idleThreshold);
 }
 next.elapsed=add(next.elapsed,delta);
 if(!scalar(next.elapsed)||!scalar(next.endThreshold))return false;
 state=next;sharedRaw=raw;publishFlash=flash;return true;
}
// FlashPattern: comma-separated 0/1 entries, entry k -> bit (31-k) (stock GB
// "0,1,1,0,..." = 0x6ED66EBB). PORT STAND-IN (flagged): exactly 32 entries are
// admitted; another length is refused, never padded or truncated.
inline bool parsePattern(const std::string& text,std::uint32_t& out) noexcept {
 std::uint32_t bits=0;std::size_t at=0;unsigned count=0;
 while(at<=text.size()){
  if(at==text.size()||count>=32u)return false;
  const char c=text[at];
  if(c!='0'&&c!='1')return false;
  if(c=='1')bits|=1u<<(31u-count);
  ++count;++at;
  if(at==text.size())break;
  if(text[at]!=',')return false;
  ++at;
 }
 if(count!=32u)return false;
 out=bits;return true;
}
struct PointState {
 std::uint32_t pattern=0,originalPacked=0;
 float period=0.f,elapsed=0.f,filter=0.f;
};
inline bool initializePoint(std::uint32_t pattern,float period,float filter,
 const std::array<float,4>& color,PointState& output) noexcept {
 using namespace detail;
 if(!arithmetic()||!scalar(period)||!(period>0.f)||!scalar(filter)||filter<0.f||filter>1.f)return false;
 PointState next;next.pattern=pattern;next.period=period;next.filter=filter;
 if(!packed(color,next.originalPacked))return false;output=next;return true;
}
//147650: same original operation order and low5-bit SRLV behavior. The caller
// publishes the resulting color to the actual emitted point. Original146C40
// also updates an attached flare; native flare ownership is a separate join.
inline bool advancePoint(PointState& state,float delta,const std::array<float,4>& current,
 std::array<float,4>& output) noexcept {
 using namespace detail;
 if(!arithmetic()||!scalar(delta)||delta<0.f||delta>1.f||!scalar(state.period)||!(state.period>0.f)||
    !scalar(state.filter)||state.filter<0.f||state.filter>1.f||
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
// One track light flare (common rewrite, W0 round 2 area E/F contract): one row
// per SCB LightN that authors HasLightFlare (2778D8), read by the ONE light
// reader (2783D0) at 05.13 for every course, drawn by the renderer's one shared
// flare pass (area F). Trivially copyable value row; it confers no draw,
// visibility or entity authority. Source of every field:
//   position  the light's Position (descriptor +14, the 278068 borrow), w = 1
//   color     the flare strip's GS byte colour 23D750 publishes (strip +70): the
//             LightFlareColorRGB lanes CVT.W.S-converted (278000..278024), packed
//             A<<24|R<<16|G<<8|B (278038) and halved per byte, alpha 64
//             ({127,127,127,64} for 255,255,255)
//   size      LightFlareSize (+A0, 277974, default 1.0)
//   zone      the light's InitialZone, SCB ZoneN numbering (1-based; 0 = not
//             zone-gated, never produced by the reader: InitialZone defaults to 1)
//   light     the SCB LightN ordinal that authored it (diagnostics)
//   texture   LightFlareTextureMapName (+A4, 277994, default "none"), NUL-terminated;
//             a dotted name is refused by the reader (2779B4)
struct TrackFlare50CR {
 float position[4];
 float color[4];
 float size;
 std::uint32_t zone;
 std::uint32_t light;
 char texture[64];
};
static_assert(std::is_trivially_copyable_v<TrackFlare50CR>);
}

#pragma once
#include <cstdint>
#include <limits>
namespace original_main_entry50KG::pcm_drain_detail50KG {
// Arithmetic only; this plan grants no source/output readiness. Preserve the
// existing uint32 microsecond API's representable interval, subdividing only
// its <=1-second transport calls. No synthetic source updates are issued.
inline constexpr std::uint64_t maximumWavFrames=(std::uint64_t{0xffffffff}-36)/4;
inline constexpr std::uint32_t maximumChunkMicroseconds=1000000;
struct Counters {
 std::uint32_t rate=0;
 std::uint64_t simulationMicroseconds=0,mixedFrames=0,sourceMixedFrames=0,capturedFrames=0;
 bool captureActive=false;
};
enum class Error {none,accounting,overflow,captureLimit};
struct Plan {
 std::uint64_t nanoseconds=0,microseconds=0,frames=0;
 std::uint32_t remainderNanoseconds=0,priorSampleRemainder=0,chunks=0;
};
inline bool addable(std::uint64_t a,std::uint64_t b)noexcept {
 return b<=std::numeric_limits<std::uint64_t>::max()-a;
}
inline std::uint64_t framesThrough(const Plan& p,std::uint32_t rate,std::uint64_t microseconds)noexcept {
 // Only use with a validated plan and prefix <=p.microseconds<=UINT32_MAX.
 return (microseconds*rate+p.priorSampleRemainder)/1000000;
}
inline Error prepare(std::uint64_t ns,std::uint64_t accepted,std::uint32_t remainder,
 const Counters& c,Plan& p)noexcept {
 p={};
 if(c.rate<8000||c.rate>192000||remainder>=1000||remainder!=accepted%1000||
    c.simulationMicroseconds!=accepted/1000||c.sourceMixedFrames!=c.mixedFrames||
    (c.captureActive&&c.capturedFrames!=c.mixedFrames))return Error::accounting;
 // accepted/1000 bounds the quotient, so these products fit uint64 for all
 // accepted nanosecond totals and every supported mixer sample rate.
 const auto quotient=c.simulationMicroseconds/1000000;
 const auto fractional=(c.simulationMicroseconds%1000000)*c.rate;
 const auto previousFrames=quotient*c.rate+fractional/1000000;
 if(c.mixedFrames!=previousFrames)return Error::accounting;
 if(!addable(ns,remainder)||!addable(accepted,ns))return Error::overflow;
 const auto carried=ns+remainder,us=carried/1000;
 if(us>std::numeric_limits<std::uint32_t>::max()||!addable(c.simulationMicroseconds,us))return Error::overflow;
 p.nanoseconds=ns;p.microseconds=us;p.remainderNanoseconds=static_cast<std::uint32_t>(carried%1000);
 p.priorSampleRemainder=static_cast<std::uint32_t>(fractional%1000000);
 p.frames=framesThrough(p,c.rate,us);
 p.chunks=static_cast<std::uint32_t>(us?(us-1)/maximumChunkMicroseconds+1:1);
 if(!addable(c.mixedFrames,p.frames)||!addable(c.sourceMixedFrames,p.frames)||
    !addable(c.capturedFrames,c.captureActive?p.frames:0))return Error::overflow;
 if(c.captureActive&&(c.capturedFrames>maximumWavFrames||p.frames>maximumWavFrames-c.capturedFrames))return Error::captureLimit;
 return Error::none;
}
// Testable iteration primitive; callbacks are the actual output calls in the
// live owner. A false attempt stops immediately. Its prefix may already have
// been published, so the owning AudioOutput must latch a terminal fault.
struct Progress {
 std::uint64_t completedMicroseconds=0,attemptedMicroseconds=0;
 std::uint32_t completedChunks=0;
 bool issued=false,complete=false;
};
template<class Attempt>bool drain(const Plan& p,Progress& progress,Attempt&& attempt)noexcept {
 progress={};
 do {
  const auto remaining=p.microseconds-progress.completedMicroseconds;
  const auto chunk=static_cast<std::uint32_t>(remaining>maximumChunkMicroseconds?maximumChunkMicroseconds:remaining);
  progress.attemptedMicroseconds=chunk;progress.issued=true;
  if(!attempt(chunk,progress.completedMicroseconds+chunk))return false;
  progress.completedMicroseconds+=chunk;++progress.completedChunks;
 }while(progress.completedMicroseconds<p.microseconds);
 progress.complete=true;return true;
}
}

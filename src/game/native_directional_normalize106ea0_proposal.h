#pragma once
// SOURCE ADAPTATION: authentic second RA Directional and private particle
// constructor inline callers reuse this numerical leaf.
// This numeric adapter itself grants no source-owner authority.
// Explicit native adaptation of106EA0, NOT an EE/VU numerical emulator.
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Directional native adaptation requires strict floating-point semantics"
#endif

namespace rr::game::directional_native_proposal {
using Words = std::array<std::uint32_t, 4>;
struct Trace {
    Words squares{}; // xyz only; unused w remains0, not a claimed VU register word.
    std::uint32_t accX = 0, sumX = 0, root = 0, q = 0;
};
enum class Status { complete, nativeEnvironment, unsupportedDomain };
namespace detail {
inline float fromWord(std::uint32_t v) noexcept {
    float r; std::memcpy(&r, &v, sizeof r); return r;
}
inline std::uint32_t word(float v) noexcept {
    std::uint32_t r; std::memcpy(&r, &v, sizeof r); return r;
}
inline bool normalOrZeroWord(std::uint32_t v) noexcept {
    const auto u = v & 0x7fffffffU;
    return u == 0 || (u >= 0x00800000U && u < 0x7f800000U);
}
inline bool normalOrZero(float v) noexcept { return normalOrZeroWord(word(v)); }
// Volatile materialization separates native binary32 steps and prevents FMA
// contraction across the helper boundaries under strict compiler semantics.
// It does NOT emulate VU truncation, ACC internals, or eliminate x87/libm
// differences on arbitrary targets. See the proposal's admission policy.
inline float mul(float a, float b) noexcept {volatile float r = a*b; return r;}
inline float add(float a, float b) noexcept {volatile float r = a+b; return r;}
inline bool product(float a, float b, float& r) noexcept {
    r = mul(a,b);
    return normalOrZero(r) && (r != 0.f || a == 0.f || b == 0.f);
}
}

// Caller supplies the already-owned descriptor look18 words, NOT a previously
// normalized UI observation. Owner equality branch147194..1471F8 stays outside
// this numeric adapter. All-zero XYZ is unsupported here: do not invent a
// direction or divide by zero. Caller must retain the source equality branch.
// Output/trace are transactional on rejection. Host FP status flags/errno may
// change: they are not source VU flags and are not a returned verification.
inline Status normalize106EA0(const Words& input, Words& output,
                             Trace* trace = nullptr) noexcept {
    static_assert(sizeof(float) == sizeof(std::uint32_t), "binary32 storage");
    static_assert(std::numeric_limits<float>::is_iec559 &&
                  std::numeric_limits<float>::radix == 2 &&
                  std::numeric_limits<float>::digits == 24, "binary32 required");
    // Read only: do not silently change a process-wide/native FP environment.
    if (std::fegetround() != FE_TONEAREST) return Status::nativeEnvironment;
    std::array<float,4> v{};
    for (unsigned i=0;i<4;++i) {
        if (!detail::normalOrZeroWord(input[i])) return Status::unsupportedDomain;
        v[i]=detail::fromWord(input[i]);
    }
    Trace next{};
    std::array<float,3> sq{};
    //106EA4 VMUL.xyz vf5,vf4,vf4.
    for (unsigned i=0;i<3;++i) {
        if (!detail::product(v[i],v[i],sq[i])) return Status::unsupportedDomain;
        next.squares[i]=detail::word(sq[i]);
    }
    //106EA8 VADDw.x vf6,vf0,vf0: architectural vf0=(0,0,0,1).
    const float one=detail::add(0.f,1.f);
    //106EAC VADDAy.x ACC,vf5,vf5: x²+y².
    const float acc=detail::add(sq[0],sq[1]);
    if (!detail::normalOrZero(acc)) return Status::unsupportedDomain;
    next.accX=detail::word(acc);
    //106EB0 VMADDz.x vf5,vf6,vf5: ACC+1*z², NOT reordered sum.
    float zTerm;
    if (!detail::product(one,sq[2],zTerm)) return Status::unsupportedDomain;
    const float sum=detail::add(acc,zTerm);
    if (!detail::normalOrZero(sum) || !(sum>0.f)) return Status::unsupportedDomain;
    next.sumX=detail::word(sum);
    //106EB4 VRSQRT Q,vf0.w,vf5.x. Native policy: rounded sqrtf-equivalent
    // followed by rounded division, not an approximate rsqrt instruction.
    volatile float root=std::sqrt(sum);
    if (!detail::normalOrZero(root) || !(root>0.f)) return Status::unsupportedDomain;
    volatile float q=1.f/root;
    if (!detail::normalOrZero(q) || !(q>0.f)) return Status::unsupportedDomain;
    next.root=detail::word(root); next.q=detail::word(q);
    //106EB8 VADDx.w vf6,vf4,vf0; preserve operation, not arbitrary W=1.
    const float w=detail::add(v[3],0.f);
    if (!detail::normalOrZero(w)) return Status::unsupportedDomain;
    //106EBC WAITQ ->106EC0 VMULQ.xyz vf6,vf4,Q ->106EC8 full SQ store.
    Words result{};
    for (unsigned i=0;i<3;++i) {
        float lane;
        if (!detail::product(v[i],q,lane)) return Status::unsupportedDomain;
        result[i]=detail::word(lane);
    }
    result[3]=detail::word(w);
    output=result;
    if (trace) *trace=next;
    return Status::complete;
}
}

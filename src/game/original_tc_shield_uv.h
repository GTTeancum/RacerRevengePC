#pragma once
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::game {
// Exact three float4 shader-constant rows, not a position-space TexGen matrix.
// TC_SHIELD's fourth authored row is untouched by 246CC0/29A910.
struct OriginalTcShieldUvRows {
    float rows[3][4]{{1.f,0.f,0.f,0.f},
                     {0.f,1.f,0.f,0.f},
                     {0.f,0.f,1.f,0.f}};
};
struct OriginalTcShieldUv {
    OriginalTcShieldUvRows matrix{};
    std::uint32_t frame60=0;
    std::uint32_t controllerTimestamp8=0xffffffffu;
};
// Native binary32 render-boundary projection; no R5900/VU/status parity claim.
// This deliberately admits only the authenticated TC_SHIELD subset:
// center/angle/shear=0, scale=1, U velocity=0, V velocity=.75,
// uv_frames=1, uv_Looping=0, uv_ReverseLoopOnEnd=0, animatesWhilePaused=0.
// Defaults come from 29A490; explicit overrides come from the pinned PSM.
inline bool validOriginalTcShieldUv(const OriginalTcShieldUv& state) noexcept {
    const auto& m=state.matrix.rows;
    return m[0][0]==1.f && m[0][1]==0.f && m[0][2]==0.f && m[0][3]==0.f &&
        m[1][0]==0.f && m[1][1]==1.f && m[1][2]==0.f && m[1][3]==0.f &&
        m[2][0]==0.f && std::isfinite(m[2][1]) && m[2][1]>=-5.f && m[2][1]<=0.f &&
        m[2][2]==1.f && m[2][3]==0.f;
}
// 29AB50 copies the numeric UV leaf and uniform rows, but resets the OUTER
// controller timestamp. This is not a reinitialization of a running prototype.
inline OriginalTcShieldUv cloneOriginalTcShieldUv(const OriginalTcShieldUv& prototype) noexcept {
    auto clone=prototype;
    clone.controllerTimestamp8=0xffffffffu;
    return clone;
}
// Source 29AC90 -> 246CC0 -> 29A910 -> 29A840. The timestamp is used ONLY
// for same-tick suppression. UV velocity is per successful service, not per
// second. Do not synthesize ticks, use delta time, catch up a hidden owner, or
// reset on pause/camera changes. Caller supplies its existing application+230
// timestamp, application+11C pause and any genuinely installed callback result.
// Omitted callback means the source null-callback path (true at 29ACE4).
inline bool serviceOriginalTcShieldUv(OriginalTcShieldUv& state,
    std::uint32_t sourceApplicationTimestamp, bool applicationPaused,
    bool callbackAllowsUpdate=true) noexcept {
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
        "Original TC UV state requires IEEE binary32");
    if(std::fegetround()!=FE_TONEAREST || !validOriginalTcShieldUv(state)) return false;
    if(sourceApplicationTimestamp==state.controllerTimestamp8 || applicationPaused) return true;
    auto next=state;
    if(callbackAllowsUpdate) {
        if(next.frame60==0) {
            // 29A918..29A940: first service writes identity; it does not scroll.
            next.matrix=OriginalTcShieldUvRows{};
        } else {
            // 29A6B4..29A6D8 constructs step[2][1] = 0 - v_velocity.
            // 29A978..29A9D8 multiplies current rows by that step. For the
            // proven TC identity basis this is exactly one binary32 subtract.
            volatile float translated=next.matrix.rows[2][1]-0.75f;
            float v=translated;
            // 29AA20..29AA60: strict abs(value)>5, then CVT.W.S/truncation
            // and subtraction, NOT fmod on every tick or a [0,1) wrap.
            if(std::fabs(v)>5.f) {
                volatile float integral=static_cast<float>(static_cast<std::int32_t>(v));
                volatile float reduced=v-integral;
                v=reduced;
            }
            next.matrix.rows[2][1]=v;
        }
        // 29A840..29A86C increments a source word. Omitted uv_Looping is 0,
        // so uv_frames=1 does not reset this counter. Unsigned wrap is explicit.
        ++next.frame60;
    }
    // 29AD48..29AD58 stamps even after a callback rejects the leaf update;
    // the earlier same-timestamp and pause exits do not stamp.
    next.controllerTimestamp8=sourceApplicationTimestamp;
    if(!validOriginalTcShieldUv(next)) return false;
    state=next;
    return true;
}
}

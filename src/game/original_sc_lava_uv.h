#pragma once
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::game {
// Exact SC lava identity-basis subset of 29A490 / 29A910. Each original
// shader pass owns its own rows, counter and outer same-timestamp word.
struct OriginalScLavaUvRows {
    float rows[3][4]{{1.f,0.f,0.f,0.f},{0.f,1.f,0.f,0.f},{0.f,0.f,1.f,0.f}};
};
struct OriginalScLavaUv {
    OriginalScLavaUvRows matrix{};
    std::uint32_t frame60=0, controllerTimestamp8=0xffffffffu;
};
inline bool validOriginalScLavaUv(const OriginalScLavaUv& state) noexcept {
    const auto& m=state.matrix.rows;
    return m[0][0]==1.f && m[0][1]==0.f && m[0][2]==0.f && m[0][3]==0.f &&
        m[1][0]==0.f && m[1][1]==1.f && m[1][2]==0.f && m[1][3]==0.f &&
        std::isfinite(m[2][0]) && std::fabs(m[2][0])<=5.f &&
        std::isfinite(m[2][1]) && std::fabs(m[2][1])<=5.f && m[2][2]==1.f && m[2][3]==0.f;
}
// Timestamp is the actual application+230 sample, pause is application+11C.
// Velocities are per selected service, never elapsed-time rates. This native
// binary32 projection does not claim R5900 accumulator/status or VU parity.
inline bool serviceOriginalScLavaUv(OriginalScLavaUv& state,std::uint32_t pass,
    std::uint32_t sourceApplicationTimestamp,bool applicationPaused,
    bool callbackAllowsUpdate=true) noexcept {
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559);
    if(pass<1 || pass>2 || std::fegetround()!=FE_TONEAREST || !validOriginalScLavaUv(state))return false;
    if(sourceApplicationTimestamp==state.controllerTimestamp8 || applicationPaused)return true;
    auto next=state;
    if(callbackAllowsUpdate) {
        if(!next.frame60) next.matrix=OriginalScLavaUvRows{}; //29A918..29A940
        else {
            // Exact binary32 encodings of the two authenticated PSM decimals.
            const float velocity[2][2]{{0x1.a36e2ep-12f,-0x1.99e8cap-14f},
                                       {-0x1.9c34f6p-12f,0x1.d19158p-14f}};
            for(unsigned axis=0;axis<2;++axis) {
                //29A68C..29A6D8 constructs -velocity; identity-basis matrix
                // update29A978..29A9D8 adds that translation independently.
                volatile float translated=next.matrix.rows[2][axis]-velocity[pass-1][axis];
                float value=translated;
                //29A9DC..29AA60, separate strict abs>5 reduction for U and V.
                if(std::fabs(value)>5.f) {
                    volatile float integral=static_cast<float>(static_cast<std::int32_t>(value));
                    volatile float reduced=value-integral;value=reduced;
                }
                next.matrix.rows[2][axis]=value;
            }
        }
        ++next.frame60; //uv_frames=1; omitted looping/reverse are both zero.
    }
    //29AD48..58 stamps after a false callback too; pause/repeat exits do not.
    next.controllerTimestamp8=sourceApplicationTimestamp;
    if(!validOriginalScLavaUv(next))return false;
    state=next;return true;
}
}

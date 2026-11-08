#pragma once
// Per-race native record of destroyed racers (original states 3/4).
//
// The racer's own vehicleState40 (set to 4 by 2261B0, 3 by 226750) remains
// the authority read by the rank, camera and renderer. This record holds the
// original racer fields the native frame state does not carry (4C, 268, 270)
// and the native bookkeeping needed by host owners (hide, logging). Reset at
// every cold race activation; Restart Race rebuilds the racers (the original
// restores them through 1AEA70 -> virtual78(a1=1) -> 228110).
#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace rr::game {

struct NativeDestroyedRacer50PD {
    bool destroyed=false;
    std::uint32_t handler=0;      // 0x2261B0 (explode, state4) or 0x226750 (wreck, state3)
    std::uint32_t decisionPC=0;   // 225C50 dispatch site
    std::uint32_t state40=0;
    float timer4C=0.f;            // 2261CC / 22675C write 0
    float timer268=0.f;           // 2261D8 writes 0.6f; 228BD4 decrements in state4
    float explosion270[4]{};      // 226288: body288 local XYZ, 274 += 5
    std::uint64_t frame=0;        // completed native frame of the transition
    std::uint64_t destroyedFrames=0;
    std::uint32_t attackerD30=0xffffffffu; // 225F04: kill credit target (manager index) or -1
};

inline std::array<NativeDestroyedRacer50PD,8> nativeDestroyedRacers50PD{};
inline void resetNativeDestroyedRacers50PD() noexcept { nativeDestroyedRacers50PD={}; }
inline bool nativeRacerDestroyed50PD(std::size_t slot) noexcept {
    return slot<nativeDestroyedRacers50PD.size()&&nativeDestroyedRacers50PD[slot].destroyed;
}

// Unattended test only: RR_TEST_DESTROY="slot@seconds[,slot@seconds...]"
// sets the racer's canonical B9D bit0 once the race clock passes the time,
// which makes 225C50 select the original 2261B0 explode handler.
struct NativeTestDestroy50PD { int slot=-1; double seconds=0; bool fired=false; };
inline std::array<NativeTestDestroy50PD,8> nativeTestDestroys50PD=[]{
    std::array<NativeTestDestroy50PD,8> out{};
    const char* v=std::getenv("RR_TEST_DESTROY");
    if(!v)return out;
    std::string text(v);std::size_t at=0,used=0;
    while(at<text.size()&&used<out.size()){
        const auto comma=text.find(',',at);
        const auto item=text.substr(at,comma==std::string::npos?std::string::npos:comma-at);
        const auto split=item.find('@');
        if(split!=std::string::npos){
            const int slot=std::atoi(item.substr(0,split).c_str());
            const double seconds=std::atof(item.substr(split+1).c_str());
            if(slot>=0&&slot<8&&seconds>=0)out[used++]={slot,seconds,false};
        }
        if(comma==std::string::npos)break;
        at=comma+1;
    }
    return out;
}();
// Set by the race loop for the slot whose test time has passed this tick.
inline std::array<bool,8> nativeTestDestroyPending50PD{};

} // namespace rr::game

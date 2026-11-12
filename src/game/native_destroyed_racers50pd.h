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
    // 225C50 suffix work applied at the next world boundary (other racers'
    // arenas and the shared music owner are not borrowed inside a frame).
    bool killCreditPending=false;      // 225F34: attacker CD0 += 1
    std::uint32_t musicSong=0;         // 226184: 1A46E0(race10C, 17..19, 0); 0 = none
    bool suffixApplied=false;
    // Human one-side loss (227370 state1 = left lost, 227A40 state2 = right
    // lost): the pod keeps racing on the surviving side until 228620's 4C/BA8
    // test (2289BC..228A90) calls 2261B0 or 226750 with no 225C50 suffix.
    bool limping=false;
    std::uint32_t limpState40=0;
    float limp4C=0.f;                  // 2273C8/227A98: 2.0f; 228A00 decrements
    std::uint64_t limpFrame=0,limpFrames=0;
};

inline std::array<NativeDestroyedRacer50PD,8> nativeDestroyedRacers50PD{};
inline void resetNativeDestroyedRacers50PD() noexcept { nativeDestroyedRacers50PD={}; }
inline bool nativeRacerDestroyed50PD(std::size_t slot) noexcept {
    return slot<nativeDestroyedRacers50PD.size()&&nativeDestroyedRacers50PD[slot].destroyed;
}

// Unattended test only: RR_TEST_DESTROY="slot@seconds[:attacker][:w][,...]"
// sets the racer's canonical B9D bit0 once the race clock passes the time,
// which makes 225C50 select the original 2261B0 explode handler. ":w"
// instead dispatches 225C50's 225D9C site (slot 8C, 226750 wreck in place).
struct NativeTestDestroy50PD { int slot=-1; double seconds=0; bool fired=false; int attacker=-1; bool wreck=false; int limp=0; };
inline std::array<NativeTestDestroy50PD,8> nativeTestDestroys50PD=[]{
    std::array<NativeTestDestroy50PD,8> out{};
    const char* v=std::getenv("RR_TEST_DESTROY");
    if(!v)return out;
    std::string text(v);std::size_t at=0,used=0;
    while(at<text.size()&&used<out.size()){
        const auto comma=text.find(',',at);
        auto item=text.substr(at,comma==std::string::npos?std::string::npos:comma-at);
        const auto wreckTag=item.find(":w");const bool wreck=wreckTag!=std::string::npos;
        if(wreck)item.erase(wreckTag,2);
        // ":left"/":right": the human's 225E7C/225EC0 dispatch (slot 90/94).
        int limp=0;
        for(const auto& tag:{std::pair<const char*,int>{":left",1},{":right",2}}){
            const auto at=item.find(tag.first);
            if(at!=std::string::npos){limp=tag.second;item.erase(at,std::string(tag.first).size());}
        }
        const auto split=item.find('@');
        if(split!=std::string::npos){
            const int slot=std::atoi(item.substr(0,split).c_str());
            const auto colon=item.find(':',split);
            const double seconds=std::atof(item.substr(split+1,colon==std::string::npos?std::string::npos:colon-split-1).c_str());
            const int attacker=colon==std::string::npos?-1:std::atoi(item.substr(colon+1).c_str());
            if(slot>=0&&slot<8&&seconds>=0&&attacker<8&&attacker!=slot)out[used++]={slot,seconds,false,attacker,wreck,limp};
        }
        if(comma==std::string::npos)break;
        at=comma+1;
    }
    return out;
}();
// Set by the race loop for the slot whose test time has passed this tick.
inline std::array<bool,8> nativeTestDestroyPending50PD{};
inline std::array<bool,8> nativeTestDestroyWreck50PD{};
inline std::array<int,8> nativeTestDestroyLimp50PD{};
// Optional attacker per pending slot (-1 none): the hook then also writes the
// victim's D30 marker and D2C=2.0 as 22A37C..22A3B4 do for a racer hit.
inline std::array<int,8> nativeTestDestroyAttacker50PD{-1,-1,-1,-1,-1,-1,-1,-1};

} // namespace rr::game

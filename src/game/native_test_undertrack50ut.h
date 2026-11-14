#pragma once
// Unattended test only: RR_TEST_UNDERTRACK="slot@seconds[,slot@seconds...]"
// (0 = player, 1..7 = original AI). Once the race clock passes the time, that
// racer's next physics step starts with its pod moved below Y=0: the parentless
// body scenes are lowered through the game's own 28AF90 translation setter and
// the prior sweep point is cleared (terrain_interactive_frame_physics50du.cpp),
// as a pod that fell through the track. The original 228BBC test then sees it
// and the real 2255B0 recovery runs. No recovery step is simulated. Inert
// unless the variable is set.
#include <array>
#include <cstdlib>
#include <string>

namespace rr::game {
struct NativeTestUndertrack50UT { int slot=-1; double seconds=0; bool fired=false; };
inline std::array<NativeTestUndertrack50UT,16> nativeTestUndertracks50UT=[]{
    std::array<NativeTestUndertrack50UT,16> out{};
    const char* v=std::getenv("RR_TEST_UNDERTRACK");
    if(!v)return out;
    const std::string text(v);std::size_t at=0,used=0;
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
// Set by the race loop when a slot's time passes; consumed by that racer's frame.
inline std::array<bool,8> nativeTestUndertrackPending50UT{};
// Drop target: main190 world Y = this - 99 (well below the 228BAC zero test).
inline constexpr float nativeTestUndertrackY50UT=-1.f;
} // namespace rr::game

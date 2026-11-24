#pragma once
#include <cstddef>
#include <cstdlib>
#include <cstring>

// Set only by the optional game command line, before the frontend starts.
namespace player_ai_pilot50DU {
inline bool enabled=false;
inline bool drives(unsigned slot) noexcept {
    static const bool testSecond=[](){
        const auto* value=std::getenv("RR_TEST_VS_AI_P2");
        return value&&std::strcmp(value,"1")==0;
    }();
    return enabled&&(slot==0u||(slot==1u&&testSecond));
}
}

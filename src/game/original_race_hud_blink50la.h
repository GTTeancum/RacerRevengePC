#pragma once
#include <array>
#include <cmath>
#include <cstdint>

namespace rr::game {
// Genuine race constructor1A6618/1A661C lanes, owned by the existing native
// race Session. This numeric prefix neither samples time nor schedules itself.
struct OriginalRaceHudBlink50LA {
    std::array<std::uint32_t,4> d8{};
    std::array<float,4> e8{};
    bool advance1A7C78(float actualRaceDelta,std::uint8_t actualRaceFlags1B0,
        std::uint32_t actualApplicationPause11C)noexcept {
        if(!std::isfinite(actualRaceDelta)||actualRaceDelta<0)return false;
        //1A7B94 nonzero branch reaches children but skips this numeric loop.
        if(actualApplicationPause11C)return true;
        auto elapsed=e8;auto blink=d8;
        const std::size_t views=actualRaceFlags1B0&3u;
        for(std::size_t i=0;i<views;++i){
            if(!std::isfinite(elapsed[i])||elapsed[i]<0)return false;
            volatile float sum=elapsed[i]+actualRaceDelta;
            if(!std::isfinite(sum))return false;
            elapsed[i]=sum;
            // Original ordered-less comparison; no overshoot carry/repeat.
            if(!(elapsed[i]<.25f)){elapsed[i]=0;blink[i]=blink[i]?0u:1u;}
        }
        e8=elapsed;d8=blink;return true;
    }
};
}

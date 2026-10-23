#pragma once
#include <array>
#include <cstdint>
namespace rr::platform {
//Actual single13EA10 helper payload. Its native owner retains source capacity
//and source-written words; there is no guest DMA executor or copied renderer.
struct alignas(16) OriginalUiGarageFogPacket50KG final {
    std::array<std::uint32_t,128> words; //1340BC capacity128 DWORDs.
    std::uint32_t color30=0;bool applied=false;
    explicit OriginalUiGarageFogPacket50KG(std::uint32_t argb)noexcept:color30(argb){
        //Three VIF commands, actual134650 UNPACK, GIFtag and one A+D.
        const std::uint32_t rgb=((argb>>16)&255u)|(argb&0xff00u)|((argb&255u)<<16);
        const std::array<std::uint32_t,16> written{0x11000000,0x03000000,0x02000000,0x6c028320,
            0x00008001,0x10000000,0x0000000e,0,rgb,0,0x3d,0,0x14000000,0,0,0};
        for(std::size_t i=0;i<written.size();++i)words[i]=written[i];
        //13 words before1342B0 pads3. DMA tag references4 QWORDs. The native
        //buffer pointer is retained by its owner, never truncated into word4.
    }
    bool current(std::uint32_t argb)const noexcept {
        return color30==argb&&words[8]==(((argb>>16)&255u)|(argb&0xff00u)|((argb&255u)<<16))&&
            words[10]==0x3d&&words[12]==0x14000000;
    }
};
}

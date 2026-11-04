#pragma once
#include "original_video_packet_builder2593f0_native50la.h"

namespace rr::game {
class OriginalApplicationVideoSource50LA;
//Explicit native CPU encoding of25CAD0's packet, NOT an entered source call.
//Owns independent immutable output; never aliases the paused Video+88 buffer.
//No GPU/IRQ/mode/frame/clear receipt, source pointer or submission method.
class OriginalVideoClearPacket25CAD0Native50LA final {
    friend class OriginalApplicationVideoSource50LA;
    detail::OriginalVideoPacketBuilder2593F0Native50LA packet_;
    OriginalVideoClearPacket25CAD0Native50LA()noexcept=default;
    bool encode(std::uint32_t red,std::uint32_t green,std::uint32_t blue,std::uint32_t alpha)noexcept {
        packet_.reset2593F0(); //25CB00, metadata only.
        if(!packet_.beginDma259410()||!packet_.beginGifAd259480(true))return false;
        if(!packet_.ad259510(0x1a,0x0000000000000001ull)|| //25CB24.
           !packet_.ad259510(0x4e,0x0000000130000000ull)|| //25CB40.
           !packet_.ad259510(0x4c,0x0000000000100000ull)|| //25CB50.
           !packet_.ad259510(0x18,0x0000600000006000ull)|| //25CB68.
           !packet_.ad259510(0x40,0x03ff000003ff0000ull)|| //25CB84.
           !packet_.ad259510(0x47,0x0000000000030013ull)|| //25CB98.
           !packet_.ad259510(0x00,0x0000000000000006ull))return false; //25CBA8.
        const std::uint64_t rgbaq=std::uint64_t(red&255u)|(std::uint64_t(green&255u)<<8)|
            (std::uint64_t(blue&255u)<<16)|(std::uint64_t(alpha&255u)<<24)|0x3f80000000000000ull;
        if(!packet_.ad259510(0x01,rgbaq)|| //25CBE8, low8 RGBA and exact Q bits1.0f.
           !packet_.ad259510(0x04,0x0000000060006000ull)|| //25CBFC.
           !packet_.ad259510(0x04,0x00000000a000a000ull)|| //25CC14.
           !packet_.end259630())return false; //25CC1C, still before2596A0 submission.
        return packet_.cursor04_==208&&packet_.qwords1C_==13&&!packet_.dmaTag0C_&&!packet_.gifTag10_&&!packet_.dmaQwords14_&&!packet_.gifLoops18_;
    }
public:
    ~OriginalVideoClearPacket25CAD0Native50LA()noexcept=default;
    OriginalVideoClearPacket25CAD0Native50LA(const OriginalVideoClearPacket25CAD0Native50LA&)=delete;
    OriginalVideoClearPacket25CAD0Native50LA& operator=(const OriginalVideoClearPacket25CAD0Native50LA&)=delete;
    const std::uint8_t* data()const noexcept{return packet_.bytes_.data();}
    std::size_t sizeBytes()const noexcept{return packet_.cursor04_;}
    //Immutable numeric artifact can outlive its issuer. It is never accepted
    //as source ownership, a DMA address or a rendering/completion capability.
};
}

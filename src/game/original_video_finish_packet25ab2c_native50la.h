#pragma once
#include "original_video_packet_builder2593f0_native50la.h"

namespace rr::game {
class OriginalApplicationVideoSource50LA;
//25AB2C..25AB5C's immutable native CPU encoding only, not an entered frame.
//The source CSR read/OR2/write precedes it;2596A0(1,1) submission and the
//CSR bit1 FINISH wait follow it. None is discharged by owning these bytes.
class OriginalVideoFinishPacket25AB2CNative50LA final {
    friend class OriginalApplicationVideoSource50LA;
    detail::OriginalVideoPacketBuilder2593F0Native50LA packet_;
    OriginalVideoFinishPacket25AB2CNative50LA()noexcept=default;
    bool encode()noexcept {
        packet_.reset2593F0(); //25AB2C, metadata only.
        if(!packet_.beginDma259410()|| //25AB34.
           !packet_.beginGifAd259480(true)|| //25AB40, EOP=1.
           !packet_.ad259510(0x61,1)|| //25AB50, GS FINISH data=64-bit1.
           !packet_.end259630())return false; //25AB58, before submission.
        return packet_.cursor04_==64&&packet_.qwords1C_==4&&!packet_.dmaTag0C_&&
            !packet_.gifTag10_&&!packet_.dmaQwords14_&&!packet_.gifLoops18_;
    }
public:
    ~OriginalVideoFinishPacket25AB2CNative50LA()noexcept=default;
    OriginalVideoFinishPacket25AB2CNative50LA(const OriginalVideoFinishPacket25AB2CNative50LA&)=delete;
    OriginalVideoFinishPacket25AB2CNative50LA& operator=(const OriginalVideoFinishPacket25AB2CNative50LA&)=delete;
    const std::uint8_t* data()const noexcept{return packet_.bytes_.data();}
    std::size_t sizeBytes()const noexcept{return packet_.cursor04_;}
    //This numeric artifact may outlive its issuer. It is not a DMA address,
    //packet queue entry, source Video capability, IRQ event or completion token.
};
}

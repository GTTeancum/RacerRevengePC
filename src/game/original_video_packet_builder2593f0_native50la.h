#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace rr::game {
class OriginalVideoClearPacket25CAD0Native50LA;
class OriginalVideoFinishPacket25AB2CNative50LA;
namespace detail {
//Private CPU encoding shared by the two bounded source packet producers.
//No generic builder API, hardware state, guest address or submission authority.
class OriginalVideoPacketBuilder2593F0Native50LA final {
    friend class ::rr::game::OriginalVideoClearPacket25CAD0Native50LA;
    friend class ::rr::game::OriginalVideoFinishPacket25AB2CNative50LA;
    static constexpr std::size_t capacityBytes_=384;
    //Host initialization only. reset2593F0 does NOT clear packet backing.
    //Only the actually encoded prefix is exposed, never unused array bytes.
    alignas(64) std::array<std::uint8_t,capacityBytes_> bytes_{};
    std::size_t cursor04_=0;
    std::optional<std::size_t> dmaTag0C_,gifTag10_; //Typed offsets, not pointer bits.
    std::uint32_t dmaQwords14_=0,gifLoops18_=0,qwords1C_=0;
    void word(std::size_t p,std::uint32_t value)noexcept {
        for(unsigned i=0;i<4;++i)bytes_[p+i]=std::uint8_t(value>>(8*i));
    }
    std::uint32_t word(std::size_t p)const noexcept {
        std::uint32_t value=0;
        for(unsigned i=0;i<4;++i)value|=std::uint32_t(bytes_[p+i])<<(8*i);
        return value;
    }
    void reset2593F0()noexcept {
        cursor04_=0;dmaTag0C_.reset();gifTag10_.reset();
        dmaQwords14_=0;gifLoops18_=0;qwords1C_=0;
    }
    bool appendRecord(std::uint32_t a,std::uint32_t b,std::uint32_t c,std::uint32_t d)noexcept {
        //Native bounds admission BEFORE any write. Original helpers assert
        //capacity only afterward. Both bounded producers fit24-qword backing;
        //we neither execute source assertions nor mutate their shared guards.
        if(cursor04_>capacityBytes_-16||cursor04_!=std::size_t(qwords1C_)*16)return false;
        word(cursor04_,a);cursor04_+=4;
        word(cursor04_,b);cursor04_+=4;
        word(cursor04_,c);cursor04_+=4;
        word(cursor04_,d);cursor04_+=4;
        ++qwords1C_;return true;
    }
    bool closeTag(std::optional<std::size_t>& tag,std::uint32_t& count)noexcept {
        if(!tag)return true; //Original null branch does not clear count.
        if(*tag%16||*tag>=cursor04_||cursor04_-*tag<16)return false;
        word(*tag,word(*tag)|count); //Full32-bit OR, not masked replacement.
        count=0;tag.reset();return true;
    }
    bool dmaTag259810(std::uint32_t id)noexcept {
        //Only the actual clear/FINISH CNT/END forms: other arguments are zero.
        //Do not generalize this specialization to arbitrary DMA source pointers.
        return appendRecord(id<<28,0,0,0);
    }
    bool beginDma259410()noexcept {
        if(!closeTag(dmaTag0C_,dmaQwords14_)||!closeTag(gifTag10_,gifLoops18_))return false;
        dmaTag0C_=cursor04_;return dmaTag259810(1);
    }
    bool beginGifAd259480(bool endOfPacket)noexcept {
        if(!closeTag(gifTag10_,gifLoops18_))return false;
        gifTag10_=cursor04_;
        //259920 arguments: registers=0xE,nreg=1,pre/prim/flg/nloop=0,
        //and259480 normalizes its input to the EOP bit. No hardware effect.
        if(!appendRecord(endOfPacket?0x8000u:0u,0x10000000u,0x0eu,0))return false;
        ++dmaQwords14_;return true; //2594E8..2594F0, after tag write.
    }
    bool ad259510(std::uint32_t reg,std::uint64_t value)noexcept {
        //259A40 writes dataLo,dataHi,register,zero and advances the cursor.
        //Its returned record pointer is unnecessary here, never a GPU result.
        if(!appendRecord(std::uint32_t(value),std::uint32_t(value>>32),reg,0))return false;
        ++dmaQwords14_;++gifLoops18_;return true; //259524..259538.
    }
    bool end259630()noexcept {
        //The source patches DMA before GIF, then appends the END tag.
        if(!closeTag(dmaTag0C_,dmaQwords14_)||!closeTag(gifTag10_,gifLoops18_))return false;
        return dmaTag259810(7);
    }
    OriginalVideoPacketBuilder2593F0Native50LA()noexcept=default;
    OriginalVideoPacketBuilder2593F0Native50LA(const OriginalVideoPacketBuilder2593F0Native50LA&)=delete;
    OriginalVideoPacketBuilder2593F0Native50LA& operator=(const OriginalVideoPacketBuilder2593F0Native50LA&)=delete;
};
}
}

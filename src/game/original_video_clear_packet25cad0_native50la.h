#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace rr::game {
class OriginalApplicationVideoSource50LA;
//Explicit native CPU encoding of25CAD0's packet, NOT an entered source call.
//Owns independent immutable output; never aliases the paused Video+88 buffer.
//No GPU/IRQ/mode/frame/clear receipt, source pointer or submission method.
class OriginalVideoClearPacket25CAD0Native50LA final {
    friend class OriginalApplicationVideoSource50LA;
    static constexpr std::size_t capacityBytes_=384;
    //Host initialization only. reset2593F0 does NOT clear packet backing.
    //Only the actually encoded prefix is exposed, never unused array bytes.
    alignas(64) std::array<std::uint8_t,capacityBytes_> bytes_{};
    std::size_t cursor04_=0;
    std::optional<std::size_t> dmaTag0C_,gifTag10_; //Typed offsets, not pointer bits.
    std::uint32_t dmaQwords14_=0,gifLoops18_=0,qwords1C_=0;
    OriginalVideoClearPacket25CAD0Native50LA()noexcept=default;
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
        //capacity only afterward. This13-qword producer fits24-qword backing;
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
        //Only25CAD0's actual CNT/END forms: remaining arguments are zero.
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
    bool encode(std::uint32_t red,std::uint32_t green,std::uint32_t blue,std::uint32_t alpha)noexcept {
        reset2593F0(); //25CB00, metadata only.
        if(!beginDma259410()||!beginGifAd259480(true))return false;
        if(!ad259510(0x1a,0x0000000000000001ull)|| //25CB24.
           !ad259510(0x4e,0x0000000130000000ull)|| //25CB40.
           !ad259510(0x4c,0x0000000000100000ull)|| //25CB50.
           !ad259510(0x18,0x0000600000006000ull)|| //25CB68.
           !ad259510(0x40,0x03ff000003ff0000ull)|| //25CB84.
           !ad259510(0x47,0x0000000000030013ull)|| //25CB98.
           !ad259510(0x00,0x0000000000000006ull))return false; //25CBA8.
        const std::uint64_t rgbaq=std::uint64_t(red&255u)|(std::uint64_t(green&255u)<<8)|
            (std::uint64_t(blue&255u)<<16)|(std::uint64_t(alpha&255u)<<24)|0x3f80000000000000ull;
        if(!ad259510(0x01,rgbaq)|| //25CBE8, low8 RGBA and exact Q bits1.0f.
           !ad259510(0x04,0x0000000060006000ull)|| //25CBFC.
           !ad259510(0x04,0x00000000a000a000ull)|| //25CC14.
           !end259630())return false; //25CC1C, still before2596A0 submission.
        return cursor04_==208&&qwords1C_==13&&!dmaTag0C_&&!gifTag10_&&!dmaQwords14_&&!gifLoops18_;
    }
public:
    ~OriginalVideoClearPacket25CAD0Native50LA()noexcept=default;
    OriginalVideoClearPacket25CAD0Native50LA(const OriginalVideoClearPacket25CAD0Native50LA&)=delete;
    OriginalVideoClearPacket25CAD0Native50LA& operator=(const OriginalVideoClearPacket25CAD0Native50LA&)=delete;
    const std::uint8_t* data()const noexcept{return bytes_.data();}
    std::size_t sizeBytes()const noexcept{return cursor04_;}
    //Immutable numeric artifact can outlive its issuer. It is never accepted
    //as source ownership, a DMA address or a rendering/completion capability.
};
}

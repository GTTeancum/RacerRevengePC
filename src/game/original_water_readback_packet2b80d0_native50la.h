#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

namespace rr::game {
// Exact numeric preparation of the water GS->memory transfer at2B80D0.
// This value has NO source-call, allocation, DMA, CSR, callback or GPU authority.
// All operands are supplied raw32-bit source words, not authenticated owners.
// No renderer API accepts this artifact as a transfer/completion capability.
class OriginalWaterReadbackPacket2B80D0Native50LA final {
public:
    struct SourceWords {
        std::uint32_t sourceBaseA1=0,sourcePitchA2=0,widthA3=0,heightT0=0;
        std::uint32_t sourceXT1=0,sourceYT2=0,sourceFormatT3=0,bytesPerPixelStack0=0;
    };
    struct CameraWords {
        std::uint32_t left1E0=0,top1E4=0;
    };
    // Count-only native span requirement. It does not identify memory, validate
    // a pixel format or promise that any transfer was submitted/completed.
    struct NativeByteExtent {
        std::size_t destinationOffset=0,transferBytes=0,requiredBytes=0;
    };
private:
    SourceWords words_;
    alignas(16) std::array<std::uint8_t,128> packet_{};
    std::uint32_t byteProduct_=0,receiveQwords_=0,destinationOffset_=0;
    std::optional<NativeByteExtent> nativeExtent_;
    static std::uint32_t asr(std::uint32_t value,unsigned shift)noexcept {
        // Only constant nonzero shifts2,4,6 are used. No host signed shift.
        return (value>>shift)|((value&0x80000000u)?(~std::uint32_t{0}<<(32-shift)):0u);
    }
    static std::uint32_t signedTruncate(std::uint32_t value,unsigned shift)noexcept {
        // Original ADDIU adjustment followed by SRA, truncating toward zero.
        if(value&0x80000000u)value+=(std::uint32_t{1}<<shift)-1u;
        return asr(value,shift);
    }
    static std::uint64_t sx32(std::uint32_t value)noexcept {
        return std::uint64_t(value)|((value&0x80000000u)?0xffffffff00000000ull:0ull);
    }
    static std::uint32_t sx16(std::uint32_t value)noexcept {
        value&=0xffffu;return value|((value&0x8000u)?0xffff0000u:0u);
    }
    void word(std::size_t offset,std::uint32_t value)noexcept {
        for(unsigned i=0;i<4;++i)packet_[offset+i]=std::uint8_t(value>>(8*i));
    }
    void qword(std::size_t index,std::uint32_t a,std::uint32_t b,std::uint32_t c,std::uint32_t d)noexcept {
        const auto p=index*16;word(p,a);word(p+4,b);word(p+8,c);word(p+12,d);
    }
    void ad(std::size_t index,std::uint32_t reg,std::uint64_t value)noexcept {
        qword(index,std::uint32_t(value),std::uint32_t(value>>32),reg,0);
    }
    void checkNativeExtent(std::optional<std::uint64_t> unwrappedOffset)noexcept {
        nativeExtent_.reset();
        // Source arithmetic below remains defined for EVERY raw word. Native
        // full-byte storage is deliberately narrower: positive, no signed
        // product overflow, exact qword extent, and the source alignment
        // predicate satisfied. This does not execute the mutable assertion.
        const auto w=words_.widthA3,h=words_.heightT0,b=words_.bytesPerPixelStack0;
        if(!unwrappedOffset||!w||!h||!b||((w|h|b)&0x80000000u))return;
        const std::uint64_t pixels=std::uint64_t(w)*h;
        if(pixels>std::uint64_t{0x7fffffffu}/b)return;
        const auto bytes=pixels*b;
        if(bytes%16u||(receiveQwords_&7u)||bytes!=std::uint64_t(receiveQwords_)*16u)return;
        const auto offset=*unwrappedOffset;
        if(offset!=destinationOffset_||offset>std::numeric_limits<std::size_t>::max()||
           bytes>std::numeric_limits<std::size_t>::max()-offset)return;
        nativeExtent_=NativeByteExtent{std::size_t(offset),std::size_t(bytes),std::size_t(offset+bytes)};
    }
    explicit OriginalWaterReadbackPacket2B80D0Native50LA(SourceWords words,std::uint32_t offset,
        std::optional<std::uint64_t> unwrappedOffset)noexcept:words_(words),destinationOffset_(offset) {
        //2B8108/8118: MULT's low32-bit product, then signed /16 at8128..34.
        //Unsigned multiplication intentionally retains wrapping source bits.
        byteProduct_=words.widthA3*words.heightT0*words.bytesPerPixelStack0;
        receiveQwords_=signedTruncate(byteProduct_,4);
        //2B81A8..8288: reset, CNT, EOP PACKED A+D with FIVE registers, END.
        //Source443D98 is a separate64-qword buffer, not Video+88's24-qwords.
        //This independent immutable output contains only its eight used qwords.
        qword(0,0x10000006u,0,0,0); //CNT QWC6 is not receive DMA QWC.
        qword(1,0x00008005u,0x10000000u,0x0000000eu,0);
        // Preserve the actual sign/zero extensions and full64-bit OR. There
        // are no convenient GS field masks in these original instructions.
        const auto bitblt=sx32(words.sourceBaseA1)|
            (std::uint64_t(asr(words.sourcePitchA2,6))<<16)|
            (std::uint64_t(words.sourceFormatT3)<<24);
        ad(2,0x50u,bitblt); //2B8200, BITBLTBUF.
        ad(3,0x52u,sx32(words.widthA3)|(sx32(words.heightT0)<<32)); //2B8228, TRXREG.
        ad(4,0x51u,sx32(words.sourceXT1)|(sx32(words.sourceYT2)<<16)); //2B8250, TRXPOS.
        ad(5,0x61u,0); //2B8264, FINISH data0, unlike frame25AB50's data1.
        ad(6,0x53u,1); //2B8278, TRXDIR local-to-host.
        qword(7,0x70000000u,0,0,0);
        checkNativeExtent(unwrappedOffset);
    }
public:
    // Immutable numeric values are freely copyable; they cannot retain or
    // manufacture actual source process history, packet ownership or a pointer.
    OriginalWaterReadbackPacket2B80D0Native50LA(const OriginalWaterReadbackPacket2B80D0Native50LA&)=default;
    OriginalWaterReadbackPacket2B80D0Native50LA& operator=(const OriginalWaterReadbackPacket2B80D0Native50LA&)=delete;
    static OriginalWaterReadbackPacket2B80D0Native50LA encodeNumeric(SourceWords words)noexcept {
        return OriginalWaterReadbackPacket2B80D0Native50LA(words,0,std::uint64_t{0});
    }
    // Arithmetic of2B45B0..4608 only. The caller must separately own the
    // authentic branch, Video262E90 result, selected Camera and443C88 value.
    // We do not read or change383418, Application11C,443DC8 or any pointer.
    static OriginalWaterReadbackPacket2B80D0Native50LA colorArguments(
        std::uint32_t video262E90,CameraWords camera,std::uint32_t offset443C88)noexcept {
        SourceWords words;
        words.sourceBaseA1=sx16(video262E90<<5);words.sourcePitchA2=640;
        words.widthA3=128;words.heightT0=128;
        words.sourceXT1=camera.left1E0+offset443C88-1u;words.sourceYT2=camera.top1E4;
        words.sourceFormatT3=0;words.bytesPerPixelStack0=4;
        return OriginalWaterReadbackPacket2B80D0Native50LA(words,0,std::uint64_t{0});
    }
    // Argument arithmetic shared by initial2B4614 and continuation2B489C.
    // Initial index is0; continuation index is the owner's actual443DC8-1.
    // This API does not derive/admit that state or test index against bandsD8.
    static OriginalWaterReadbackPacket2B80D0Native50LA depthArguments(
        std::uint32_t sourceBase443CD0,CameraWords camera,std::uint32_t offset443CF0,
        std::uint32_t columnsDC,std::uint32_t index)noexcept {
        SourceWords words;
        words.sourceBaseA1=sourceBase443CD0;words.sourcePitchA2=640;words.widthA3=92;
        //2B4648..465C and2B48AC..48EC: signed trunc((DC+3)/4)*4.
        //ADDIU and SLL wrap; negative/overflow inputs are not normalized away.
        words.heightT0=signedTruncate(columnsDC+3u,2)<<2;
        const std::uint32_t x23=((index<<1)+index)*8u-index;
        words.sourceXT1=camera.left1E0+offset443CF0+(x23<<2); //2B48F0..4918: +92*index.
        words.sourceYT2=camera.top1E4;words.sourceFormatT3=0x30u;words.bytesPerPixelStack0=4;
        //2B4718..4740 forms the ORIGINAL stride from unrounded DC. It is
        //not the rounded transfer-height stride. Keep every wrapping shift.
        std::uint32_t stride=((columnsDC<<1)+columnsDC)*8u-columnsDC;
        stride<<=4;stride>>=2;stride+=3u;stride>>=2;
        const std::uint32_t byteOffset=(index*stride)<<4; //2B48A4/48BC.
        // A native count-only extent exists only when none of those offset
        // operations wrapped. This still does not validate an allocated span.
        std::optional<std::uint64_t> unwrapped;
        const auto fullStride=std::uint64_t(columnsDC)*23u*16u;
        if(!(columnsDC&0x80000000u)&&fullStride<=0xffffffffu){
            const auto qwords=((fullStride>>2)+3u)>>2;
            if(qwords<=0xffffffffu&&(!qwords||index<=std::uint64_t{0xffffffffu}/qwords/16u))
                unwrapped=std::uint64_t(index)*qwords*16u;
        }
        return OriginalWaterReadbackPacket2B80D0Native50LA(words,byteOffset,unwrapped);
    }
    const SourceWords& sourceWords()const noexcept{return words_;}
    const std::uint8_t* data()const noexcept{return packet_.data();}
    static constexpr std::size_t sizeBytes()noexcept{return 128;}
    std::uint32_t sourceByteProductWord()const noexcept{return byteProduct_;}
    std::uint32_t sourceReceiveQwordsWord()const noexcept{return receiveQwords_;}
    bool sourceAlignmentPredicate()const noexcept{return (receiveQwords_&7u)==0;}
    std::uint32_t sourceDestinationOffsetWord()const noexcept{return destinationOffset_;}
    const std::optional<NativeByteExtent>& checkedNativeByteExtent()const noexcept{return nativeExtent_;}
    //After8288 the source must still perform CSR write2,2596A0(1,0), CSR
    //FINISH wait, VIF1_STAT/BUSDIR writes and receive QWC/MADR/CHCR setup.
    //No part of those effects, the assertion handler or callback return ran.
};
}

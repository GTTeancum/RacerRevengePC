#include "original_save_career_projection50kg.h"
#include <cstring>
#include <limits>
#include <utility>

namespace rr::game {
namespace {
using Projection=OriginalSaveCareerProjection50KG;
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24,"original binary32 fields");
static_assert(std::numeric_limits<std::int16_t>::min()==-32768,"signed original prize half");
std::uint32_t little32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
        (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}
std::int16_t signedLittle16(const std::uint8_t* p) noexcept {
    const auto bits=std::uint32_t(p[0])|(std::uint32_t(p[1])<<8);
    const auto value=bits<0x8000u?static_cast<std::int32_t>(bits):
        static_cast<std::int32_t>(bits)-65536;
    return static_cast<std::int16_t>(value);
}
bool supportedAbilityBits(std::uint32_t bits) noexcept {
    const auto magnitude=bits&0x7fffffffu;
    const auto exponent=bits&0x7f800000u;
    // Inspect representations before float conversion, including signaling NaN.
    if(exponent==0x7f800000u)return false;
    if(exponent==0&&magnitude!=0)return false;
    return (bits&0x80000000u)==0||magnitude==0;
}
Projection::Status result(Projection::Diagnostic& d,Projection::Status s,
    std::size_t offset=0) noexcept {d.status=s;d.payloadByteOffset=offset;return s;}
}
OriginalSaveCareerProjection50KG::OriginalSaveCareerProjection50KG(
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope,std::uint32_t index) noexcept
    :envelope_(std::move(envelope)),index_(index) {}
OriginalSaveCareerProjection50KG::Status OriginalSaveCareerProjection50KG::retainCareer(
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope,std::uint32_t index,
    std::unique_ptr<OriginalSaveCareerProjection50KG>& out,Diagnostic& d) noexcept {
    d={};
    if(!envelope)return result(d,Status::missingEnvelope);
    if(index>=careerCount)return result(d,Status::careerIndex);
    try {
        std::unique_ptr<OriginalSaveCareerProjection50KG> next(
            new OriginalSaveCareerProjection50KG(std::move(envelope),index));
        const auto base=OriginalSavePayloadEnvelope50KG::careerPayloadOffset+
            static_cast<std::size_t>(index)*careerStride;
        const auto* source=next->envelope_->originalBytes().data();
        for(std::size_t pod=0;pod<podCount;++pod){
            const auto row=base+pod*podStride;auto& target=next->fields_.pods[pod];
            for(std::size_t lane=0;lane<6;++lane){
                const auto offset=row+lane*4;const auto bits=little32(source+offset);
                if(!supportedAbilityBits(bits))return result(d,Status::unsupportedAbility,offset);
                std::memcpy(&target.abilities1920[lane],&bits,sizeof(bits));
            }
            for(std::size_t course=0;course<13;++course){
                const auto offset=row+0x18+course;const auto medal=source[offset];
                if(medal>8)return result(d,Status::unsupportedMedal,offset);
                target.medals1938[course]=medal;
                target.remainingPrizes1946[course]=signedLittle16(source+row+0x26+course*2);
            }
        }
        // Mode2 result/name entry uses strcpy on219D. The reset writes only an
        // empty string, so byte20 need NOT be zero when an earlier NUL exists.
        const auto nameOffset=base+0x87d;
        if(!std::memchr(source+nameOffset,0,21))
            return result(d,Status::unterminatedCareerName,nameOffset);
        std::memcpy(next->name219D_.data(),source+nameOffset,21);
        const auto progress=source[base+0x87c];
        if((progress&31u)>13u)return result(d,Status::unsupportedProgress,base+0x87c);
        next->fields_.earned2190=little32(source+base+0x870);
        next->fields_.unspent2194=little32(source+base+0x874);
        next->fields_.kills2198=little32(source+base+0x878);
        next->fields_.progress219C=progress; // Retain the unrelated upper three bits.
        out=std::move(next);return result(d,Status::ok);
    } catch(...) {return result(d,Status::allocationFailure);}
}
}

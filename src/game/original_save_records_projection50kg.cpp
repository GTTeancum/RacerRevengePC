#include "original_save_records_projection50kg.h"
#include <cstring>
#include <limits>
#include <utility>

namespace rr::game {
namespace {
using Projection=OriginalSaveRecordsProjection50KG;
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24,"original binary32 record fields");
std::uint32_t little32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
        (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}
bool supportedTimeBits(std::uint32_t bits) noexcept {
    const auto magnitude=bits&0x7fffffffu,exponent=bits&0x7f800000u;
    return exponent!=0x7f800000u&&(exponent!=0||magnitude==0);
}
constexpr std::size_t payloadOffset(std::size_t profileOffset) noexcept {
    return OriginalSavePayloadEnvelope50KG::recordsPayloadOffset+
        profileOffset-OriginalSavePayloadEnvelope50KG::recordsProfileOffset;
}
Projection::Status result(Projection::Diagnostic& d,Projection::Status s,
    std::size_t offset=0) noexcept {d.status=s;d.payloadByteOffset=offset;return s;}
}
OriginalSaveRecordsProjection50KG::OriginalSaveRecordsProjection50KG(
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope) noexcept
    :envelope_(std::move(envelope)) {}
OriginalSaveRecordsProjection50KG::Status OriginalSaveRecordsProjection50KG::retainRecords(
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope,
    std::unique_ptr<OriginalSaveRecordsProjection50KG>& out,Diagnostic& d) noexcept {
    d={};if(!envelope)return result(d,Status::missingEnvelope);
    try {
        std::unique_ptr<OriginalSaveRecordsProjection50KG> next(
            new OriginalSaveRecordsProjection50KG(std::move(envelope)));
        const auto* source=next->envelope_->originalBytes().data();
        for(std::size_t course=0;course<courseCount;++course){
            auto& target=next->courses_[course];
            const auto base=OriginalSavePayloadEnvelope50KG::recordsPayloadOffset+course*courseStride;
            for(std::size_t category=0;category<3;++category){
                auto& names=category==0?target.lapNames102FC:
                    category==1?target.raceNames10348:target.knockoutNames10394;
                const auto row=base+category*0x4c;
                for(std::size_t rank=0;rank<3;++rank){
                    const auto offset=row+rank*4;const auto bits=little32(source+offset);
                    if(category==2)target.knockouts10388[rank]=bits;
                    else {
                        if(!supportedTimeBits(bits))return result(d,Status::unsupportedTime,offset);
                        auto& times=category==0?target.lap102F0:target.race1033C;
                        std::memcpy(&times[rank],&bits,sizeof(bits));
                    }
                    const auto name=row+0xc+rank*21;
                    if(source[name+20]!=0)return result(d,Status::unterminatedName,name+20);
                    std::memcpy(names[rank].data(),source+name,21);
                }
            }
        }
        auto& fields=next->shared_;
        for(std::size_t slot=0;slot<fields.playerNames10F68.size();++slot){
            const auto offset=payloadOffset(0x10f68)+slot*21;
            // strcpy consumers need an in-field terminator. Source empty-name
            // reset does not initialize bytes after its first NUL.
            if(!std::memchr(source+offset,0,21))
                return result(d,Status::unterminatedPlayerName,offset);
            std::memcpy(fields.playerNames10F68[slot].data(),source+offset,21);
        }
        for(std::size_t rider=0;rider<fields.unlock10FAC.size();++rider)
            fields.unlock10FAC[rider]=little32(source+payloadOffset(0x10fac)+rider*4);
        fields.availability11018=source[payloadOffset(0x11018)];
        if((fields.availability11018&31u)>13u)
            return result(d,Status::unsupportedAvailability,payloadOffset(0x11018));
        fields.soundMode1101C=little32(source+payloadOffset(0x1101c));
        if(fields.soundMode1101C>2)return result(d,Status::unsupportedSoundMode,payloadOffset(0x1101c));
        for(std::size_t lane=0;lane<fields.volume11020.size();++lane){
            const auto offset=payloadOffset(0x11020)+lane*4;
            fields.volume11020[lane]=little32(source+offset);
            if(fields.volume11020[lane]>100)return result(d,Status::unsupportedVolume,offset);
        }
        fields.flags11050=source[payloadOffset(0x11050)];
        out=std::move(next);return result(d,Status::ok);
    } catch(...) {return result(d,Status::allocationFailure);}
}
}

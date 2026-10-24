#include "original_save_payload_envelope50kg.h"
#include <utility>

namespace rr::game {
namespace {
using Envelope=OriginalSavePayloadEnvelope50KG;
std::uint32_t little32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
        (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}
Envelope::Status result(Envelope::Diagnostic& d,Envelope::Status status,
    std::size_t offset=0) noexcept {
    d.status=status;d.byteOffset=offset;return status;
}
}
OriginalSavePayloadEnvelope50KG::OriginalSavePayloadEnvelope50KG(const std::uint8_t* data)
    :bytes_(data,data+payloadSize) {}
OriginalSavePayloadEnvelope50KG::Status OriginalSavePayloadEnvelope50KG::retainOriginalBytes(
    const std::uint8_t* data,std::size_t size,
    std::unique_ptr<OriginalSavePayloadEnvelope50KG>& out,Diagnostic& d) noexcept {
    d={};
    if(!data)return result(d,Status::nullInput);
    if(size!=payloadSize)return result(d,Status::wrongSize);
    if(little32(data)!=originalVersion)return result(d,Status::wrongVersion);
    if(data[packedFlagPayloadOffset]>1)
        return result(d,Status::noncanonicalPackedFlag,packedFlagPayloadOffset);
    try {
        std::unique_ptr<OriginalSavePayloadEnvelope50KG> next(
            new OriginalSavePayloadEnvelope50KG(data));
        out=std::move(next);
        return result(d,Status::ok);
    } catch(...) {return result(d,Status::allocationFailure);}
}
std::uint32_t OriginalSavePayloadEnvelope50KG::identity191C() const noexcept {
    return little32(bytes_.data()+4);
}
bool OriginalSavePayloadEnvelope50KG::rawIdentityEquals(std::uint32_t expected) const noexcept {
    return identity191C()==expected;
}
std::uint8_t OriginalSavePayloadEnvelope50KG::packedFlag11054Bit0() const noexcept {
    return bytes_[packedFlagPayloadOffset];
}
OriginalSavePayloadEnvelope50KG::Region OriginalSavePayloadEnvelope50KG::careerRegion() const noexcept {
    return {bytes_.data()+careerPayloadOffset,careerSize,careerProfileOffset};
}
OriginalSavePayloadEnvelope50KG::Region OriginalSavePayloadEnvelope50KG::recordsRegion() const noexcept {
    return {bytes_.data()+recordsPayloadOffset,recordsSize,recordsProfileOffset};
}
OriginalSavePayloadEnvelope50KG::Status OriginalSavePayloadEnvelope50KG::copyOriginalBytes(
    std::vector<std::uint8_t>& out,Diagnostic& d) const noexcept {
    d={};
    try {
        auto next=bytes_;out.swap(next);return result(d,Status::ok);
    } catch(...) {return result(d,Status::allocationFailure);}
}
}

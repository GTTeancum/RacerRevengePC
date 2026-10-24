#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace rr::game {
// Pure original wire-envelope ownership. This is NOT a complete semantic
// decoder, a fresh-profile serializer, a live-profile importer or a storage
// capability. There is deliberately no constructor from native profile subsets.
// Only supplied structurally canonical payload bytes can construct this owner.
// Acceptance proves neither original provenance nor semantic field validity.
class OriginalSavePayloadEnvelope50KG final {
public:
    static constexpr std::size_t payloadSize=0xF508;
    static constexpr std::uint32_t originalVersion=7;
    static constexpr std::size_t careerPayloadOffset=8;
    static constexpr std::size_t careerProfileOffset=0x1920;
    static constexpr std::size_t careerSize=0xE79C;
    static constexpr std::size_t recordsPayloadOffset=0xE7A4;
    static constexpr std::size_t recordsProfileOffset=0x102F0;
    static constexpr std::size_t recordsSize=0xD64;
    static constexpr std::size_t packedFlagPayloadOffset=0x4A;
    static constexpr std::size_t packedFlagProfileOffset=0x11054;

    enum class Status:std::uint8_t {
        ok, nullInput, wrongSize, wrongVersion, noncanonicalPackedFlag,
        allocationFailure
    };
    struct Diagnostic {
        Status status=Status::ok;
        std::size_t byteOffset=0;
    };
    struct Region {
        const std::uint8_t* bytes=nullptr;
        std::size_t size=0;
        std::size_t originalProfileOffset=0;
    };

    // Strict envelope validation only. Exact length excludes truncation/trailing
    // data; LE version must equal7. Original186BA4..A8 writes only bit0 at byte4A,
    // so this supported original-writer envelope rejects upper bits there.
    // Identity is retained, not invented or range-normalized. Its relationship
    // to the selected storage/profile owner remains that owner's obligation.
    // Failure leaves out unchanged. Caller must retain its source for this call.
    static Status retainOriginalBytes(const std::uint8_t* data,std::size_t size,
        std::unique_ptr<OriginalSavePayloadEnvelope50KG>& out,Diagnostic&) noexcept;

    OriginalSavePayloadEnvelope50KG(const OriginalSavePayloadEnvelope50KG&)=delete;
    OriginalSavePayloadEnvelope50KG& operator=(const OriginalSavePayloadEnvelope50KG&)=delete;
    ~OriginalSavePayloadEnvelope50KG()=default;
    std::uint32_t identity191C() const noexcept;
    // Corresponds only to caller1868BC..D4 comparison of probe186E50 result.
    // Does not confer slot/volume provenance, card presence or load readiness.
    // Full original Load imports the new identity; equality is NOT a load gate.
    // Raw comparison only: probe failure sentinel -1 is not interpreted here.
    bool rawIdentityEquals(std::uint32_t expected191C) const noexcept;
    std::uint8_t packedFlag11054Bit0() const noexcept;
    // Views borrow this immutable envelope; no live profile pointers are used.
    // These opaque regions are evidence for a future typed import plan, NOT
    // buffers to memcpy into native C++ owners. byte4A is the special override.
    Region careerRegion() const noexcept;
    Region recordsRegion() const noexcept;
    const std::vector<std::uint8_t>& originalBytes() const noexcept {return bytes_;}
    // Byte-identical export, no default synthesis or profile serialization.
    // Failure leaves out unchanged. This does not write a filesystem target.
    Status copyOriginalBytes(std::vector<std::uint8_t>& out,Diagnostic&) const noexcept;
private:
    explicit OriginalSavePayloadEnvelope50KG(const std::uint8_t* data);
    std::vector<std::uint8_t> bytes_;
};
static_assert(OriginalSavePayloadEnvelope50KG::careerPayloadOffset+
    OriginalSavePayloadEnvelope50KG::careerSize==
    OriginalSavePayloadEnvelope50KG::recordsPayloadOffset,"original first region extent");
static_assert(OriginalSavePayloadEnvelope50KG::recordsPayloadOffset+
    OriginalSavePayloadEnvelope50KG::recordsSize==
    OriginalSavePayloadEnvelope50KG::payloadSize,"original payload extent");
}

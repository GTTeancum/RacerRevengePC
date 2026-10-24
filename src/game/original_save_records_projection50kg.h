#pragma once
#include "original_save_payload_envelope50kg.h"
#include "original_ui_hall_records50kg.h"
#include <memory>

namespace rr::game {
// Only persisted fields: no native known flag, remembered UI group, fabricated
// localization provenance, preset action reconstruction or active owner state.
struct OriginalSaveSharedFields50KG {
    // Source1864B0 reset /1CA53C..570 edits /1AC4AC..DC result reads.
    std::array<std::array<std::uint8_t,21>,2> playerNames10F68{};
    std::array<std::uint32_t,27> unlock10FAC{};
    std::uint8_t availability11018=0;
    std::uint32_t soundMode1101C=0;
    std::array<std::uint32_t,4> volume11020{};
    std::uint8_t flags11050=0;
};
// Read-only projection of13 Hall rows plus the explicitly named saved fields.
// All other bytes remain opaque in the retained immutable envelope. This is
// neither whole-save validation nor authority to alter live/shared profiles.
class OriginalSaveRecordsProjection50KG final {
public:
    static constexpr std::size_t courseCount=13,courseStride=0xE4;
    enum class Status:std::uint8_t {
        ok,missingEnvelope,unsupportedTime,unterminatedName,
        unsupportedAvailability,unsupportedSoundMode,unsupportedVolume,
        allocationFailure,unterminatedPlayerName
    };
    struct Diagnostic {Status status=Status::ok;std::size_t payloadByteOffset=0;};
    // Supported native restrictions: ordinary finite binary32 times; original
    // writer's name[20]==0; availability.low5<=13; mode<=2; volumes<=100.
    // Zero/negative finite times remain exact (formatter189000 displays dashes).
    // KO and unlock words remain raw uint32; no guessed upper bound, sorted-rank
    // requirement, printable-name normalization or progression proof is added.
    // Failure preserves out; success never fills any current application owner.
    static Status retainRecords(std::shared_ptr<const OriginalSavePayloadEnvelope50KG>,
        std::unique_ptr<OriginalSaveRecordsProjection50KG>& out,Diagnostic&) noexcept;
    OriginalSaveRecordsProjection50KG(const OriginalSaveRecordsProjection50KG&)=delete;
    OriginalSaveRecordsProjection50KG& operator=(const OriginalSaveRecordsProjection50KG&)=delete;
    ~OriginalSaveRecordsProjection50KG()=default;
    const std::array<OriginalUiHallCourseRecords50KG,courseCount>& courses() const noexcept {return courses_;}
    const OriginalSaveSharedFields50KG& sharedFields() const noexcept {return shared_;}
    const OriginalSavePayloadEnvelope50KG& envelope() const noexcept {return *envelope_;}
private:
    explicit OriginalSaveRecordsProjection50KG(std::shared_ptr<const OriginalSavePayloadEnvelope50KG>) noexcept;
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope_;
    std::array<OriginalUiHallCourseRecords50KG,courseCount> courses_{};
    OriginalSaveSharedFields50KG shared_{};
};
static_assert(OriginalSaveRecordsProjection50KG::courseCount*
    OriginalSaveRecordsProjection50KG::courseStride==0xB94,"original13 Hall extent");
static_assert(0x11050-OriginalSavePayloadEnvelope50KG::recordsProfileOffset<
    OriginalSavePayloadEnvelope50KG::recordsSize,"last projected shared byte within payload");
}

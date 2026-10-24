#pragma once
#include "original_save_payload_envelope50kg.h"
#include "original_tournament_result50kg.h"
#include <memory>

namespace rr::game {
// Read-only typed projection of ONE original career block. Retains every source
// byte through an immutable envelope, including unmodeled bytes. NOT a live
// import proposal, whole-save validator, earned-progress authority or IO owner.
class OriginalSaveCareerProjection50KG final {
public:
    static constexpr std::size_t careerCount=27,careerStride=0x894;
    static constexpr std::size_t podCount=27,podStride=0x50;
    enum class Status:std::uint8_t {
        ok,missingEnvelope,careerIndex,unsupportedAbility,unsupportedMedal,
        unsupportedProgress,allocationFailure,unterminatedCareerName
    };
    struct Diagnostic {
        Status status=Status::ok;
        std::size_t payloadByteOffset=0;
    };
    // Supported-domain safety restrictions, not original Load validation:
    // abilities must be nonnegative ordinary binary32 (signed zero permitted),
    // medals0..8, progress.low5<=13, and a NUL within the21-byte career name.
    // Unknown bits/padding are NEVER filled,
    // clamped, normalized or discarded from the retained envelope. Signed prize
    // halves and unsigned counters are read exactly without invented bounds.
    // Success validates this career only and leaves all other careers opaque.
    // Failure leaves any previous out unchanged. No live profile is accepted.
    static Status retainCareer(std::shared_ptr<const OriginalSavePayloadEnvelope50KG>,
        std::uint32_t index,std::unique_ptr<OriginalSaveCareerProjection50KG>& out,
        Diagnostic&) noexcept;
    OriginalSaveCareerProjection50KG(const OriginalSaveCareerProjection50KG&)=delete;
    OriginalSaveCareerProjection50KG& operator=(const OriginalSaveCareerProjection50KG&)=delete;
    ~OriginalSaveCareerProjection50KG()=default;
    std::uint32_t careerIndex() const noexcept {return index_;}
    const OriginalTournamentCareer50KG& fields() const noexcept {return fields_;}
    // Source219D,21 bytes including an in-range terminator; suffix retained.
    const std::array<std::uint8_t,21>& name219D()const noexcept{return name219D_;}
    const OriginalSavePayloadEnvelope50KG& envelope() const noexcept {return *envelope_;}
private:
    OriginalSaveCareerProjection50KG(std::shared_ptr<const OriginalSavePayloadEnvelope50KG>,
        std::uint32_t) noexcept;
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope_;
    std::uint32_t index_;
    OriginalTournamentCareer50KG fields_{};
    std::array<std::uint8_t,21> name219D_{};
};
static_assert(OriginalSaveCareerProjection50KG::careerCount*
    OriginalSaveCareerProjection50KG::careerStride==
    OriginalSavePayloadEnvelope50KG::careerSize,"original27 career extent");
static_assert(OriginalSaveCareerProjection50KG::podCount*
    OriginalSaveCareerProjection50KG::podStride==0x870,"original27 pod extent");
}

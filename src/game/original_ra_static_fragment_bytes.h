#pragma once
#include "original_gameplay_blur_leaf.h"
#include <cstdint>
#include <optional>

namespace rr::game {

// SOURCE-ONLY NUMERIC CANDIDATE. No renderer admission or runtime integration.
// These are already-resolved byte operands, NOT raw texels/vertices or floats.
// Their sampling/interpolation policy and provenance must be established by a
// separate owner. In particular, do not reapply TEXA to sampled image bytes.
struct RaStaticByteRgba {
    std::uint8_t red = 0, green = 0, blue = 0, alpha = 0;
};
struct RaStaticAlreadySampledRgba { RaStaticByteRgba value; };
struct RaStaticAlreadyInterpolatedPacketRgba { RaStaticByteRgba value; };

// Supplied only AFTER any required fog RGB operation on the returned modulated
// color. For FGE=0 the external owner supplies that color unchanged. This leaf
// neither produces fog nor decides whether bypassing it is permitted. No alpha
// field: the surviving MODULATE alpha is retained independently below.
struct RaStaticResolvedPreblendRgb {
    std::uint8_t red = 0, green = 0, blue = 0;
};
// The current destination SAMPLE in source primitive order, not a span snapshot.
// Obtaining it, multisample ownership, and coverage are outside this arithmetic.
struct RaStaticCurrentDestinationRgba { RaStaticByteRgba value; };

// Numeric subset only, not a state decoder or an authenticated material profile.
// Retained inactive selectors are checked too. Other map/pass/program/sampler,
// fog, depth, coverage, and target fields remain the external owner's obligation.
struct RaStaticByteSelectors {
    std::int32_t tcc, tfx, ate, atst, aref, afail, abe;
    std::int32_t alphaA, alphaB, alphaC, alphaD, alphaFix;
    std::int32_t colclamp, fba, pabe;
};

inline bool raStaticByteSelectorsSupported(const RaStaticByteSelectors& s) noexcept {
    if (s.tcc != 1 || s.tfx != 0 || s.atst != 6 || s.afail != 0 ||
        s.alphaA != 0 || s.alphaB != 1 || s.alphaC != 0 || s.alphaD != 1 ||
        s.alphaFix != 128 || s.colclamp != 1 || s.fba != 0 || s.pabe != 0)
        return false;
    // Exact numeric projections of observed RA combinations, not their Cartesian
    // product: no ATE-off blend, and AREF120 only with ATE and ABE both enabled.
    return (s.ate == 0 && s.aref == 0 && s.abe == 0) ||
           (s.ate == 1 && s.aref == 0 && (s.abe == 0 || s.abe == 1)) ||
           (s.ate == 1 && s.aref == 120 && s.abe == 1);
}

namespace ra_static_byte_detail {
inline std::uint8_t clampByte(std::int32_t value) noexcept {
    return static_cast<std::uint8_t>(value < 0 ? 0 : value > 255 ? 255 : value);
}
inline std::uint8_t modulate(std::uint8_t texture, std::uint8_t packet) noexcept {
    // Nonnegative integer division is floor. Clamp(floor(x)) equals
    // floor(clamp(x,0,255)) on this domain; product is at most65025.
    return clampByte((static_cast<std::int32_t>(texture) * packet) / 128);
}
inline std::uint8_t blend(std::uint8_t source, std::uint8_t dest,
                          std::uint8_t alpha) noexcept {
    const std::int32_t difference = static_cast<std::int32_t>(source) - dest;
    // Product in[-65025,65025]: negation/+127 in the existing helper cannot
    // overflow. Unlike the blur wrapper, allow alpha>128 and saturate output.
    return clampByte(dest + gameplayBlurSignedFloor128(difference * alpha));
}
} // namespace ra_static_byte_detail

enum class RaStaticByteStageStatus { unsupported_selectors, alpha_rejected, alpha_passed };
struct RaStaticByteStageResult;

// A local arithmetic token only; it proves neither fog completion nor any
// renderer readiness. Construction is limited to successful MODULATE/test.
class RaStaticAlphaPassedBytes {
public:
    const RaStaticByteRgba& modulated() const noexcept { return modulated_; }

    RaStaticByteRgba compose(RaStaticResolvedPreblendRgb source,
                            RaStaticCurrentDestinationRgba destination) const noexcept {
        using ra_static_byte_detail::blend;
        if (!blendEnabled_)
            return {source.red, source.green, source.blue, modulated_.alpha};
        return {blend(source.red, destination.value.red, modulated_.alpha),
                blend(source.green, destination.value.green, modulated_.alpha),
                blend(source.blue, destination.value.blue, modulated_.alpha),
                modulated_.alpha}; // Destination alpha is deliberately unused.
    }

private:
    RaStaticByteRgba modulated_;
    bool blendEnabled_;
    RaStaticAlphaPassedBytes(RaStaticByteRgba value, bool blendEnabled) noexcept
        : modulated_(value), blendEnabled_(blendEnabled) {}
    friend RaStaticByteStageResult modulateAndTestRaStaticBytes(
        const RaStaticByteSelectors&, RaStaticAlreadySampledRgba,
        RaStaticAlreadyInterpolatedPacketRgba) noexcept;
};

struct RaStaticByteStageResult {
    RaStaticByteStageStatus status;
    std::optional<RaStaticAlphaPassedBytes> surviving;
};

inline RaStaticByteStageResult modulateAndTestRaStaticBytes(
    const RaStaticByteSelectors& selectors, RaStaticAlreadySampledRgba sampled,
    RaStaticAlreadyInterpolatedPacketRgba interpolated) noexcept {
    if (!raStaticByteSelectorsSupported(selectors))
        return {RaStaticByteStageStatus::unsupported_selectors, std::nullopt};
    using ra_static_byte_detail::modulate;
    const auto& t = sampled.value;
    const auto& p = interpolated.value;
    const RaStaticByteRgba source{modulate(t.red, p.red), modulate(t.green, p.green),
                                modulate(t.blue, p.blue), modulate(t.alpha, p.alpha)};
    if (selectors.ate == 1 && source.alpha <= selectors.aref)
        // AFAIL KEEP: no surviving color result; caller must issue no color or
        // depth write for this fragment. This function performs neither write.
        return {RaStaticByteStageStatus::alpha_rejected, std::nullopt};
    return {RaStaticByteStageStatus::alpha_passed,
            RaStaticAlphaPassedBytes(source, selectors.abe == 1)};
}

} // namespace rr::game

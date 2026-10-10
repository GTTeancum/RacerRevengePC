#pragma once
#include <cmath>
#include <cstdint>

// NEW, INACTIVE SOURCE CANDIDATE. Static reconstruction, not recovered source.
// No guest addresses are dereferenced; no instructions are interpreted.
// See SOURCE-EVIDENCE.md. Not compiled, syntax-checked, tested or activated.
namespace rr::game::ui_depth_candidate {

enum class Status : std::uint8_t {
    complete, missingAuthenticatedOwner, inconsistentVideoOwner,
    unsupportedNative16BitMode, unsupportedCanvas, nonfinite, packetRange,
    fractionalSubtractionNeedsSourceRoundingOwner
};

// The integrating frontend must supply facts from its immutable authenticated
// native video-mode owner. Merely filling this record does not authenticate it.
// identity must name that owner and outlive all dependent packet geometry.
// Original video pointer: global 0x443A60, constructor 0x259E00, mode setter 0x25B9F0.
struct AuthenticatedVideoFacts {
    const void* identity = nullptr;
    std::int32_t width = 0, height = 0;
    std::int32_t colorBits = 0;       // source owner+08
    std::int32_t packetZMaximum = 0;  // source owner+60
    std::uint32_t zBufferFormat = 0;  // source owner+5C
};

// Existing native renderer stores D24. Do not silently rescale a 16-color-bit owner
// or admit it merely because its integer Z fits inside the renderer's range.
inline Status admitNativeD24Owner(const AuthenticatedVideoFacts& owner) noexcept {
    if (!owner.identity) return Status::missingAuthenticatedOwner;
    if (owner.zBufferFormat != 0x31u) return Status::inconsistentVideoOwner;
    if (owner.colorBits == 16) {
        if (owner.packetZMaximum != 0xffff) return Status::inconsistentVideoOwner;
        return Status::unsupportedNative16BitMode;
    }
    if (owner.colorBits != 32 || owner.packetZMaximum != 0xffffff)
        return Status::inconsistentVideoOwner;
    if (owner.width != 640 || owner.height != 448) return Status::unsupportedCanvas;
    return Status::complete;
}

struct PacketDepth {
    float sourcePositionZ = 0;       // exact source binary32 before FTOI0
    std::uint32_t integerPacketZ = 0;
};

namespace detail {
inline float sourceSubtract(float a, float b) noexcept {
    volatile float value = a - b; // one binary32 SUB.S, no algebraic rewrite
    return value;
}
inline Status finish(float z, PacketDepth& output) noexcept {
    if (!std::isfinite(z)) return Status::nonfinite;
    // Intentionally excludes saturation, wrap, negative and out-of-D24 cases.
    // For this bounded nonnegative domain, the native conversion truncates
    // toward zero exactly like source FTOI0; this is not metric CVT.W.S.
    if (z < 0.f || z > 16777215.f) return Status::packetRange;
    const PacketDepth next{z, static_cast<std::uint32_t>(z)};
    output = next;
    return Status::complete;
}
}

// ScreenText 0x2E10DC..0x2E1110 and assigned ScreenTexture depth 0x2ACA44..0x2ACA88:
// z = depth<0 ? -depth : binary32(float(owner+60)-depth).
// The same assignment is inlined by Main 0x1E55BC..0x1E55F4 (depth 15).
inline Status textOrAssignedTexture(const AuthenticatedVideoFacts& owner,
    float assignedDepth, PacketDepth& output) noexcept {
    const auto status = admitNativeD24Owner(owner);
    if (status != Status::complete) return status;
    if (!std::isfinite(assignedDepth)) return Status::nonfinite;
    // Integer subtractions inside the admitted D24 range are exact. A broader
    // positive fractional-depth path needs its own source FPU-rounding owner.
    if (assignedDepth >= 0.f && std::floor(assignedDepth) != assignedDepth)
        return Status::fractionalSubtractionNeedsSourceRoundingOwner;
    const float z = assignedDepth < 0.f ? -assignedDepth :
        detail::sourceSubtract(static_cast<float>(owner.packetZMaximum), assignedDepth);
    return detail::finish(z, output);
}

// ScreenTexture constructor packet 0x2DF398..0x2DF408 always subtracts; its depth
// member is initially 1 at 0x2DEFA4. Do not import the later setter's sign branch.
inline Status initialTexture(const AuthenticatedVideoFacts& owner,
    float initialDepth, PacketDepth& output) noexcept {
    const auto status = admitNativeD24Owner(owner);
    if (status != Status::complete) return status;
    if (!std::isfinite(initialDepth)) return Status::nonfinite;
    if (std::floor(initialDepth) != initialDepth)
        return Status::fractionalSubtractionNeedsSourceRoundingOwner;
    return detail::finish(detail::sourceSubtract(
        static_cast<float>(owner.packetZMaximum), initialDepth), output);
}

// Icon authored-negative special case 0x2AC848..0x2AC880 is a separate operation:
// assignedDepth = float(owner+60 - 300); then the normal sign-aware assignment.
inline Status authoredIcon(const AuthenticatedVideoFacts& owner,
    float authoredDepth, PacketDepth& output) noexcept {
    const auto status = admitNativeD24Owner(owner);
    if (status != Status::complete) return status;
    if (!std::isfinite(authoredDepth)) return Status::nonfinite;
    const float assignedDepth = authoredDepth < 0.f ?
        static_cast<float>(owner.packetZMaximum - 300) : authoredDepth;
    return textOrAssignedTexture(owner, assignedDepth, output);
}

// For a source-position Z already supplied by a separately authenticated native
// arithmetic owner, FTOI0 is closed across the admitted bounded float domain.
inline Status fromOwnedSourcePosition(const AuthenticatedVideoFacts& owner,
    float sourcePositionZ, PacketDepth& output) noexcept {
    const auto status = admitNativeD24Owner(owner);
    if (status != Status::complete) return status;
    return detail::finish(sourcePositionZ, output);
}

// No ordering, enabled-pass, texture-animation, framebuffer-alpha, depth-clear,
// or race-backdrop ownership is asserted by these arithmetic helpers.
}

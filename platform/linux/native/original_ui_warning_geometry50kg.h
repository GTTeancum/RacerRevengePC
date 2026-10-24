#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::platform {
// Numeric inputs only. These are NOT retained owners, object handles, receiver
// capabilities, callback seals, resource evidence, or render-ready projections.
struct OriginalUiWarningStripValues50KG {
    float positionX10=0,positionY14=0,height38=0,rotation3C=0;
    std::uint8_t flags5CValue=0;
    // Canonical Quad mapping: height38 = uv30[2], rotation3C = uv30[3].
    // size20 and every unlisted scalar are untouched by 204AC0.
};
struct OriginalUiWarningValues50KG {
    float delta290=0,timer294=0;
    std::uint32_t active298=0,primaryPresent29C=0,secondaryPresent2A0=0;
    std::uint32_t primaryEnabled474=0,secondaryEnabled478=0;
    std::array<OriginalUiWarningStripValues50KG,3> strips{}; // 40, A0, 100.
};
enum class OriginalUiWarningOperationKind50KG : std::uint8_t {
    positionX10,positionY14,height38,flags5C,timer294,
    callDim2E21D0,callText2E0170,callStrip2DF550,callInherited141D70
};
struct OriginalUiWarningOperation50KG {
    OriginalUiWarningOperationKind50KG kind=OriginalUiWarningOperationKind50KG::timer294;
    std::uint32_t sourcePC=0,receiverOffset=0,valueBits=0;
    // Only flags5C operations use this. The owner may OR it into its separate
    // flags5CKnownMask; do not replace that mask or infer other bits are known.
    std::uint8_t assertedDirtyBits=0;
};
enum class OriginalUiWarningGeometryStatus50KG : std::uint8_t {
    planned,unsupportedArithmetic,operationCapacity
};
struct OriginalUiWarningGeometryPlan50KG {
    // A fixed instruction-order transcript, NOT a publication. The caller must
    // first prove the real retained470 receiver, sealed third-pass callback,
    // scalar epoch, mutable packets and resources. Calls still need real
    // consumers; no operation here invokes, commits, publishes or retires them.
    std::array<OriginalUiWarningOperation50KG,64> operations{};
    std::size_t operationCount=0;
    OriginalUiWarningGeometryStatus50KG status=OriginalUiWarningGeometryStatus50KG::planned;
    std::uint32_t unsupportedPC=0;
};

// Models only 204AC0..2054A8, from the supplied post-update retained values.
// It deliberately does not perform 2048F0's separate update decrement, query a
// controller, construct an asset, reset source state, or impersonate a phase.
// No-warning emits ONLY inherited141D70 and does not inspect animation inputs.
// An unsupported numeric result returns no consumable operation prefix.
// Supported arithmetic: finite normal binary32 or signed zero, nearest-even,
// individually rounded instructions. Nonfinite/subnormal or host non-nearest
// arithmetic is explicitly unsupported, not claimed to emulate EE exceptions.
OriginalUiWarningGeometryPlan50KG planOriginalUiWarning204AC050KG(
    const OriginalUiWarningValues50KG&)noexcept;
}

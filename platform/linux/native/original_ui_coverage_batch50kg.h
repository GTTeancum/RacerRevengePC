#pragma once
#include "original_ui_native_draw50kg.h"
namespace rr::platform {
// Contiguous ordered opaque AA0 triangles can select their nearest surviving
// D24 fragment before consulting each retained destination sample. Both paths
// use the same GE/GREATER comparison and depth writes; ties retain original
// primitive order. No blended/AA1/nonwriting/ALWAYS/NEVER span qualifies.
inline bool originalUiCoverageOpaqueBatch50KG(const OriginalUiSpan50KG& span,
    const OriginalUiNativeSpan50KG& state)noexcept {
    return state.coverage==OriginalUiCoverage50KG::aa0&&
        span.blend==OriginalUiBlend50KG::opaque&&state.depthWrite&&
        (state.depth==OriginalUiDepth50KG::greaterEqual||state.depth==OriginalUiDepth50KG::greater);
}
}

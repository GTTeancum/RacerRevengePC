#pragma once
#include "original_ui_canvas50kg.h"
#include "../../../src/game/original_ui_main_layout50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include <cstring>

namespace rr::platform {
// Cold material resolver supplies this binding from the actual original
// material and uploaded image. No defaults are selected by the draw adapter.
struct OriginalUiResolvedDraw50KG {
    const char* materialName=nullptr;
    std::uint32_t textureIndex=0;
    OriginalUiBlend50KG blend=OriginalUiBlend50KG::opaque;
    float alphaScale=1,colorScale=1;
};
// Source ScreenTexture's strip TL,TR,BL,BR becomes the renderer's perimeter
// TL,TR,BR,BL. UV, raw source packet color, and caller painter order survive.
// Depth ordering must already have been supplied by the original page owner;
// the native UI target uses ordered alpha composition, not world depth tests.
bool appendOriginalUiOverlay50KG(OriginalUiCanvas50KG&,
    const game::OriginalUiOverlayQuad50KG&,const OriginalUiResolvedDraw50KG&) noexcept;
// Call once for each actual enabled line, in source order. Color/alpha are the
// current committed line-owner state, never recomputed from selected/disabled.
// This static transform adapter rejects neither transparent nor zero-area
// genuine glyphs. Active move/scale/rotation must use their separately updated
// transform through the underlying canvas instead of this settled path.
bool appendOriginalUiTextLine50KG(OriginalUiCanvas50KG&,
    const game::OriginalUiPositionedLine50KG&,std::uint32_t currentPackedColor,
    std::int16_t currentAlpha,const OriginalUiResolvedDraw50KG&) noexcept;
}

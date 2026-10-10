#pragma once
#include <array>
#include <cstdint>

namespace rr::game {
struct OriginalUiOverlayVertex50KG { float x=0,y=0,u=0,v=0; };
struct OriginalUiOverlayQuad50KG {
    // Original material lookup name, not a guessed texture/PST filename.
    const char* materialName=nullptr;
    // Original ScreenTexture packet order: TL, TR, BL, BR (triangle strip).
    std::array<OriginalUiOverlayVertex50KG,4> vertices{};
    float authoredDepth=1;
    // Exact packet channels from 0x2DF388: (RGB >> 1), (alpha >> 2).
    // These are NOT conventional 0..255 sRGB/alpha values. The renderer must
    // reproduce the original material/GS blend contract before normalizing.
    std::array<std::uint8_t,4> sourcePacketColor{{127,127,127,63}};
    std::uint32_t sourcePC=0;
};
struct OriginalUiMainBackground50KG {
    static constexpr float nativeWidth=640, nativeHeight=448;
    std::array<OriginalUiOverlayQuad50KG,4> quadrants{};
};
// 0x1E5580..0x1E5B08, with reciprocal atlas dimensions from 0x2DF180.
// Four 320x224 quads; source sample endpoints are 319 and 223, NOT texture
// dimensions minus one. Input dimensions belong to the actual material's
// decoded original atlas. This function never invents or supplies an atlas.
// Original depth packet value is videoDepthMax-authoredDepth (here 15).
// Screen coordinates are those before any native-to-window letterboxing.
bool layoutOriginalUiMainBackground50KG(std::uint32_t atlasWidth,
    std::uint32_t atlasHeight,OriginalUiMainBackground50KG& output) noexcept;
struct OriginalUiMainArrows50KG {
    std::uint16_t textWidgetId=0; // source 5..9 for group1 selection1..5
    OriginalUiOverlayQuad50KG beforeText,afterText;
};
struct OriginalUiMainArrowInput50KG {
    std::uint16_t selectedSlot=0;
    // Original first ScreenText position (+0x20), AFTER justification. Width
    // is the integer returned by source ScreenText::GetWidth 0x2E16F0.
    float textX=0,textY=0;
    std::int32_t sourceMeasuredTextWidth=0;
    // The actual current material-frame dimensions, not placeholder sizes.
    std::uint32_t rightWidth=0,rightHeight=0,leftWidth=0,leftHeight=0;
};
// 0x1E6410..0x1E6684: a right-facing material before the text and a
// left-facing material after it, at their full actual dimensions/UVs.
// Original arrow assets are ATM-backed materials; this does not claim a
// standalone PST exists or implement the still-separate animation decoder.
bool layoutOriginalUiMainArrows50KG(const OriginalUiMainArrowInput50KG&,
    OriginalUiMainArrows50KG& output) noexcept;
}

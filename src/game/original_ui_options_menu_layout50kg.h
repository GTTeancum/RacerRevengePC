#pragma once
#include "original_ui_main_layout50kg.h" // Shared ScreenTexture quad type only.
namespace rr::game {
struct OriginalUiOptionsBackground50KG {std::array<OriginalUiOverlayQuad50KG,4> quadrants{};};
struct OriginalUiOptionsSelection50KG {
    std::uint16_t textWidgetId=0;
    OriginalUiOverlayQuad50KG beforeText,afterText;
};
struct OriginalUiOptionsSelectionInput50KG {
    std::uint16_t selectedSlot=0;
    // Actual first ScreenText position after original justification, and exact
    // integer GetWidth. Never use label center coordinates or host font metrics.
    float textX=0,textY=0;std::int32_t sourceMeasuredTextWidth=0;
    // ScreenTexture cold creation dimensions. ATM frame changes do not resize it.
    std::uint32_t rightWidth=0,rightHeight=0,leftWidth=0,leftHeight=0;
};
// Source slot->widget mapping from table3B0110 and loads1F9B08..1F9B60.
std::uint16_t originalUiOptionsSelectedText50KG(std::uint16_t slot) noexcept;
// Options creator1F8E64..1F93C8 and render1F9AAC..1F9ACC. Real mmbkg atlas only.
bool layoutOriginalUiOptionsBackground50KG(std::uint32_t atlasWidth,std::uint32_t atlasHeight,
    OriginalUiOptionsBackground50KG&) noexcept;
// Native projection of1F9B68..1F9CE4; real animated arrow materials, not a frame
// rectangle. Source quad channels/depth/UV conventions match ScreenTexture.
bool layoutOriginalUiOptionsSelection50KG(const OriginalUiOptionsSelectionInput50KG&,
    OriginalUiOptionsSelection50KG&) noexcept;
// Page4 source1F5F40, same numeric ScreenTexture conventions; actual value
// widget comes from the page4 controller's group/slot mapping.
bool layoutOriginalUiGameOptionsSelection50KG(std::uint16_t valueWidget,
    const OriginalUiOptionsSelectionInput50KG&,OriginalUiOptionsSelection50KG&) noexcept;
bool layoutOriginalUiGameOptionsBackground50KG(std::uint32_t,std::uint32_t,
    OriginalUiOptionsBackground50KG&) noexcept;
}

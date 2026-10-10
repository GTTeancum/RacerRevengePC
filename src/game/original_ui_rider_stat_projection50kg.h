#pragma once
#include <array>
#include <cstdint>

namespace rr::game {
// NEW bounded projection of 1EBBF0; not a renderer, profile owner or widget owner.
// The caller must bind these observations to the SAME original rider at 4C4.
struct OriginalUiRiderStatInput50KG {
    bool profileReady=false;
    std::uint32_t selectedRider=0,observedRider=0,mode34=0,unlocked10FAC=0;
    float panelY4DC=0;
    std::uint32_t accentColor17FA00=0;
    std::array<float,6> current1920{},maximum0408{};
    std::array<std::uint8_t,13> medal1938{};
    std::uint8_t careerTrack219C=0;
};
enum class OriginalUiRiderStatKind50KG : std::uint8_t {
    medalOutline,medalGold,medalSilver,medalBronze,careerMarker,
    panelLeft,panelRight,riderPortrait,notch,label
};
struct OriginalUiRiderStatDraw50KG {
    OriginalUiRiderStatKind50KG kind{};
    std::uint32_t objectOffset=0,sourcePC=0;
    float x=0,y=0;
    // 0 means preserve texture-derived construction dimension. Labels use none.
    float widthOverride=0,heightOverride=0;
    // Packed original RGB, BEFORE source 133F20 >>1 / GS normalization.
    // Only true means 1EB640 explicitly refreshed this object's color.
    bool hasPackedColor=false;
    std::uint32_t packedColor=0;
    // label: localization index 200..205, denmrk14, capacity20, right alignment.
    std::uint16_t localizationIndex=0;
};
struct OriginalUiRiderStatProjection50KG {
    std::array<OriginalUiRiderStatDraw50KG,156> draws{};
    std::uint16_t count=0;
    std::array<std::int32_t,6> current{},maximum{};
    // Existing widget id4 uses this update, but its draw remains base-page owned.
    std::int32_t lockedWidgetX=460,lockedWidgetY=320;
};
// Invalid/missing observations preserve output. No I/O, allocation, guest state,
// texture creation or placeholder text. Renderer must use authenticated owners.
bool projectOriginalUiRiderStats50KG(const OriginalUiRiderStatInput50KG&,
    OriginalUiRiderStatProjection50KG&) noexcept;
}

#pragma once
#include "../assets/original_ui_pages50kg.h"
namespace rr::game {
struct OriginalUiRiderLighting50KG {
    const assets::OriginalUiMemberIdentity50KG* uiIdentity=nullptr;
    // Actual current light-manager50 and its sole active ambient slot60.
    // Manager1CCD88 allocates one ambient slot before loading these lights.
    std::array<float,4> ambient50{},ambientSlot60{};
    // Actual second registered Directional light+3C, not camera tint.
    std::array<float,4> directional3C{};
    // Light+78/7C/80. Target-position path normalizes at1CE310, then147200.
    std::array<float,3> directionalLook78{};
};
struct OriginalUiRiderLightCapture50KG {
    bool captured=false;
    std::array<float,4> ambient520{},directional530{};
};
struct OriginalUiRiderLightingDiagnostic50KG {
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
// Actual Rider manager's two-light population. Authenticated ui.dat only;
// source-native half-scale colors, no replacement shading/material policy.
// Native scalar normalization, no guest execution or VU precision emulator.
bool prepareOriginalUiRiderLighting50KG(const assets::OriginalUiArchive50KG&,
    OriginalUiRiderLighting50KG&,OriginalUiRiderLightingDiagnostic50KG&) noexcept;
// Opening1E9E70 captures the ACTUAL current colors. Directional passes through
//146A30 ARGB packing/truncation then1E9ED4..1E9FB0 unpack; ambient is exact copy.
bool captureOriginalUiRiderLighting50KG(const OriginalUiRiderLighting50KG&,
    OriginalUiRiderLightCapture50KG&) noexcept;
// Appearance1EAFB0:149900 updates ambient50 and sole active slot60;146C40 sets
// second directional3C/4C. Unlocked restores capture; locked is source black
// with ambientW64/directionalW255. Closing-threshold1EA3E0 restores capture.
bool applyOriginalUiRiderLighting50KG(bool unlocked,const OriginalUiRiderLightCapture50KG&,
    OriginalUiRiderLighting50KG&) noexcept;
}

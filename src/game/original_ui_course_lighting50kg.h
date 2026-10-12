// Reconstructed source-only candidate, 2026-10-12; not byte-identical to lost candidate.
#pragma once
#include "../assets/original_ui_pages50kg.h"
namespace rr::game {
struct OriginalUiCourseLighting50KG {
    const assets::OriginalUiMemberIdentity50KG* uiIdentity=nullptr;
    // Actual current light-manager50 and its sole active ambient slot60.
    // Manager1CCD88 allocates one ambient slot before loading these lights.
    std::array<float,4> ambient50{},ambientSlot60{};
    // Actual second registered Directional light+3C, not camera tint.
    std::array<float,4> directional3C{};
    // Light+78/7C/80. Target-position path normalizes at1CE310, then147200.
    std::array<float,3> directionalLook78{};
};
struct OriginalUiCourseLightingDiagnostic50KG {
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
// Actual Course manager's two-light population. Authenticated ui.dat only;
// source-native half-scale colors, no replacement shading/material policy.
// Native scalar normalization, no guest execution or VU precision emulator.
bool prepareOriginalUiCourseLighting50KG(const assets::OriginalUiArchive50KG&,
    OriginalUiCourseLighting50KG&,OriginalUiCourseLightingDiagnostic50KG&) noexcept;
}

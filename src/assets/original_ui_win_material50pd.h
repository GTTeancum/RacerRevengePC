#pragma once
// UIWin material entries. Defined in original_ui_material50kg.cpp beside the
// shared numeric kernels they reuse; declared here so the widely included
// material header keeps only its one new provenance enumerator.
#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"

namespace rr::assets {
// UIWin load 2077F0 calls the shared page constructor 1D63B0, whose
// HasPopup 1D6698 builds the four page prompts. Cross/triangle use OVERLAYS,
// square/circle the UIWin-local PSTs; no PSM exists for any of them.
bool readOriginalUiWinPromptFallbackMaterial50PD(const OriginalUiArchive50KG& win,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
// Exact UIWin trophy pass2: SpecularMapCompressed, specmap1 from UIWIN.RES,
// ALPHA(0,2,1,1) destination-alpha additive, zWriteDisabled=0 (depth WRITES),
// alphaTestOnOff=0, inherited DefaultMT fog. Only the eleven authenticated
// trophy PSMs are admitted. Accepted solely by the sealed source-RGBA route.
bool prepareOriginalUiWinTrophySpecularMaterial50PD(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRider19SpecularMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;
}

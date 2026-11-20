#pragma once
// PORT EXTENSION, NOT ORIGINAL (DLC menu-art step C): menu materials of an
// add-on package's own UI archive that the original builds without a PSM.
// Narrow on purpose: included by original_ui_material50kg.cpp (definition), the
// add-on model table and the Results material references only.
#include "original_ui_material50kg.h"
#include "import_texture48e.h"
#include <string>

namespace rr::assets {
class OriginalStandardMaterialObject50LA;
// Menu-art step D: the add-on Results icon (ui.results_icon, "<stem>_fsm.pst",
// no PSM) has no row in the standard material table. Its native policy is the
// FINAL ScreenObject pass of a stock Results portrait (stockPortrait: the pass
// OriginalStandardMaterialObject50LA::prepareResultsScreenFallback2475C0 lowers,
// first constructed by 2475C0 and projected onto the current 249610 context)
// lowered over the add-on's own decoded image, with that portrait's actual
// FINAL object as the Results-screen authentication. Refused unless the pass
// is a Results/Garage/MemoryCard canonical missing-PSM fallback.
bool prepareOriginalResultsAddonPortraitMaterial50DL(const OriginalUiMaterialPass50KG& stockPortrait,
    const OriginalStandardMaterialObject50LA& stockFinal,const Texture48E& addonImage,
    OriginalUiNativeMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
// The add-on rider portrait (ui.portraits, "<stem>_flrg.pst", no PSM): the same
// ScreenTexture missing-PSM fallback the original constructs for a stock rider
// portrait (readOriginalUiRiderPortraitFallbackMaterial50KG), with the add-on
// archive as its provenance archive and its own PST as the fallback texture.
// `registered` is the member name without extension ("ui\\textures\\a2r_ark_flrg").
// Refused unless the archive is an add-on archive (identity addonUi), the PST
// exists and no PSM of that name exists.
bool readOriginalUiAddonPortraitFallbackMaterial50DL(const OriginalUiArchive50KG& addonUi,
    const std::string& registered,const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
}

#pragma once
// PORT EXTENSION, NOT ORIGINAL (DLC menu-art step C): menu materials of an
// add-on package's own UI archive that the original builds without a PSM.
// Narrow on purpose: included by original_ui_material50kg.cpp (definition) and
// the add-on model table only.
#include "original_ui_material50kg.h"
#include <string>

namespace rr::assets {
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

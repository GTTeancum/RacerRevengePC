#pragma once
#include "original_ui_material_adapter50kg.h"

namespace rr::platform {
// Successful prepareOriginalUiLitMaterial50KM output for the exact authored
// pass/image is required. Reuses only the bounded raster-policy conversion;
// never passes a Diffuse material through ScreenObject source admission.
// Source packet geometry/W/fog, original draw order, model phase, texture
// publication and opaquePresentationAlphaOnly proof remain caller-owned.
inline bool convertOriginalUiLitMaterialSpan50KM(
    const assets::OriginalUiLitMaterial50KM& material,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    OriginalUiMaterialSpan50KG next;
    if(!convertOriginalUiMaterialSpan50KG(material.raster,binding,next))return false;
    switch(material.program){
    case assets::OriginalUiLitProgram50KM::diffuse:
        next.span.materialMode=OriginalUiMaterialMode50KG::sourceDiffuse;break;
    case assets::OriginalUiLitProgram50KM::diffuseCompressed:
        next.span.materialMode=OriginalUiMaterialMode50KG::sourceDiffuseCompressed;break;
    default:return false;
    }
    next.native.sourceFogEnabled=material.fogEnabled;
    next.native.sourceAlphaTestGreaterZero=material.alphaTestGreaterZero;
    output=next;return true;
}
}

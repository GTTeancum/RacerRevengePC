#pragma once
#include "original_ui_rider_model_scene50kg.h"
#include "original_ui_rider_lighting50kg.h"
#include "original_ui_rider_render_context50kg.h"
#include "original_ui_rider_clip_camera50kg.h"
#include "original_ui_rider_model_fog50kg.h"
#include "original_ui_lit_geometry50km.h"
namespace rr::game {
enum class OriginalUiRiderPacketError50KG : std::uint8_t {
    none,input,camera,lights,selection,palette,bounds
};
struct OriginalUiRiderPacketDiagnostic50KG {
    OriginalUiRiderPacketError50KG error=OriginalUiRiderPacketError50KG::none;
    OriginalUiRiderModelDiagnostic50KG model;
};
// Current selected-render snapshot. The Scene48D pointer is borrowed from the
// actual cold model owner and must not outlive it. ModelPalette preserves the
// real root separately from each original source-index palette entry, including
// the distinct root/model basis consumed by SpecularMapCompressed UVs.
struct OriginalUiRiderModelPacket50KG {
    const assets::Scene48D* scene=nullptr;
    OriginalUiLitContext50KM context;
    ModelPalette49S palette;
    OriginalUiRiderModelFog50KG fog;
    std::uint8_t sourceModelFlags191=0;
};
// Concrete camera+fog+global-light join for the actual User_Rotate graphics
// model. Its constructor240740 receives a3=0 at1D35E8, so model1B3 bit0 is
// clear even when Widgets.Dat says IsLit=T. This is NOT character lighting.
// sourceModelFlags191 must come from actual selected-render preparation
//26EFC0/240C00, not a fabricated selected=true or the widget highlight flag.
// Caller supplies original active render-context/debug-gate ownership. No LOD,
// clipping, membership or queue admission is inferred here. Scene world caches
// may refresh on failure; packet output is published only on full success.
bool prepareOriginalUiRiderPodPacket50KG(
    const assets::OriginalUiRiderPodAssets50KG&,OriginalUiRiderPodScene50KG&,
    const OriginalUiRiderRenderContext50KG&,const OriginalUiRiderLighting50KG&,
    std::uint8_t sourceModelFlags191,OriginalUiRiderModelPacket50KG&,
    OriginalUiRiderPacketDiagnostic50KG&) noexcept;
// Camera numeric join shared by pod, binder and the future character member
// owner. Caller supplies actual already-selected light matrices. This function
// does not treat global rows as an acceptable per-model character substitute.
bool prepareOriginalUiRiderLitContext50KG(const assets::OriginalUiArchive50KG&,
    const OriginalUiRiderRenderContext50KG&,
    const original_racer_lighting50KM::LightMatrices&,OriginalUiLitContext50KM&) noexcept;
}

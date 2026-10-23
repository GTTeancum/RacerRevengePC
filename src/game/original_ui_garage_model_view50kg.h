#pragma once
#include "original_racer_model_view50la.h"
#include "original_ui_rider_visibility50kg.h"
#include "../assets/import_scene48d.h"
namespace rr::game::garage_model_view50KG {
// Numeric source leaves only. The caller owns real Camera114D28/Search27EAF0,
// current canonical world/bounds and actual26F000 publication. Neither result
// is a visible-model receipt, a packet-preparation receipt or a frame seal.
struct Selection {
    std::uint8_t flags191=0,flags1B2=0,coreMask=0,guardMask=0,remainingMask=0;
    bool selected=false,disabled=false,guardChecked=false;
};
bool select240C00(const BodyMatrix48G& world,
    const OriginalRacerAggregateBounds50LA& bounds,
    const std::array<float,16>& actualObserverMatrix60,
    const std::array<float,16>& actualCameraCombined100,
    const std::array<float,16>& actualCameraGuard2D0,
    const OriginalUiRiderVisibilityPolicy50KG& actualPolicy,
    std::uint8_t actualFlags03,std::uint8_t actualFlags191,
    std::uint8_t actualFlags1B2,Selection&) noexcept;
struct LodInput {
    std::array<float,4> actualCenter{}; //140 if one node;160 otherwise.
    std::array<float,3> actualLocalF0{},actualCamera180{};
    float actualRadius=0,actualFocal1D8=0,actualGeometryScale14=0;
    float actualLodScale383360=0,actualRadiusScale383368=0;
};
struct Lod {
    std::uint8_t selected1B0=0;
    float distance1BC=0,scaleF20=0,projectedRadiusF21=0;
};
bool selectLod26F070(const assets::Scene48D& actualScene,const LodInput&,Lod&) noexcept;
}

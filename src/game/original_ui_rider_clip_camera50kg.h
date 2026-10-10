#pragma once
#include "original_ui_rider_camera50kg.h"
namespace rr::game {
struct OriginalUiRiderClipCamera50KG {
    std::array<float,16> clipProjection290{},combinedClip2D0{};
    float guardScale334=0,guardScale338=0;
};
//237240 original full-GS-range guard factors,236EC0/237090 separate
// clipping projection. Use the ACTUAL render-boundary cameraAfterFog; source
// projection80 for raster and clipProjection290 for clipping are distinct.
// Does not perform clipping, visibility/LOD selection or render registration.
bool prepareOriginalUiRiderClipCamera50KG(const OriginalUiRiderCamera50KG&,
    OriginalUiRiderClipCamera50KG&) noexcept;
}

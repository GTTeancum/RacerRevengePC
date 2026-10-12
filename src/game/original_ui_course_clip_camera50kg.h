// Reconstructed source-only candidate, 2026-10-12; not byte-identical to lost candidate.
#pragma once
#include "original_ui_course_camera50kg.h"
namespace rr::game {
struct OriginalUiCourseClipCamera50KG {
    std::array<float,16> clipProjection290{},combinedClip2D0{};
    float guardScale334=0,guardScale338=0;
};
//237240 original full-GS-range guard factors,236EC0/237090 separate
// clipping projection. Use the ACTUAL render-boundary cameraAfterFog; source
// projection80 for raster and clipProjection290 for clipping are distinct.
// Does not perform clipping, visibility/LOD selection or render registration.
bool prepareOriginalUiCourseClipCamera50KG(const OriginalUiCourseCamera50KG&,
    OriginalUiCourseClipCamera50KG&) noexcept;
}

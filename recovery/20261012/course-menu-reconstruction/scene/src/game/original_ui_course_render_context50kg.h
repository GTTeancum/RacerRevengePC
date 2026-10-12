// Reconstructed source-only candidate, 2026-10-12; not byte-identical to lost candidate.
#pragma once
#include "original_ui_course_camera50kg.h"
namespace rr::game {
struct OriginalUiCourseRenderContext50KG {
    const assets::OriginalUiMemberIdentity50KG* fogIdentity=nullptr;
    float visibility=1,haziness=.5f;
    std::array<float,3> sourceFogRgb{};
    // Actual13E830 start/end, and2451D4..2451F4/24BE38 row33.zw values.
    float fogStart=0,fogEnd=0,packetFogStart33Z=0,packetFogScale33W=0;
    std::uint32_t packedFogColorGs=0;
    OriginalUiCourseCamera50KG cameraAfterFog;
    std::array<float,2> sourceCanvasOffset{{1728,1824}};
};
struct OriginalUiCourseRenderContextDiagnostic50KG {
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
// Actual UI-manager1CE7B0 fog owner plus13EA30 active camera range callback.
// Source maximum1200/minimum900/base200, not Main background4000/1333/64.
// Authentic Course spells Visibilty incorrectly; original "Visibility" reader
// takes default1. Never reinterpret the typo as a differently authored key.
// Prepares render-boundary camera near1/far=fogEnd, preserving viewC0 and
// rebuilding projection Z terms, combined100 and sourceScreen250.
// Does not force any model's fog-enable packet flag or register a camera.
bool prepareOriginalUiCourseRenderContext50KG(const assets::OriginalUiArchive50KG&,
    const OriginalUiCourseCamera50KG&,OriginalUiCourseRenderContext50KG&,
    OriginalUiCourseRenderContextDiagnostic50KG&) noexcept;
}

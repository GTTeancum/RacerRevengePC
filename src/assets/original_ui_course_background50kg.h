#pragma once
#include "original_ui_archive50kg.h"
#include "import_texture48e.h"
namespace rr::assets {
// Explicit native image policy for canonical Course planet0 background.
// Original2A6490 creates a raw texture, not a fabricated PSM ScreenTexture.
bool isOriginalUiCourseBackground50KG(const OriginalUiMemberView50KG&) noexcept;
bool decodeOriginalUiCourseBackground50KG(const OriginalUiMemberView50KG&,
    Texture48E&,DecodeError48C&) noexcept;
}

#pragma once
#include "original_ui_archive50kg.h"
#include "import_texture48e.h"
namespace rr::assets {
// Explicit native image policy for the five authenticated Hall backgrounds.
// Original2A6490 creates a raw texture, not a fabricated PSM ScreenTexture.
const char* originalUiHallBackgroundName50KG(std::uint32_t planet)noexcept;
bool isOriginalUiHallBackgroundIdentity50KG(const OriginalUiMemberIdentity50KG*)noexcept;
bool isOriginalUiHallBackground50KG(const OriginalUiMemberView50KG&) noexcept;
bool decodeOriginalUiHallBackground50KG(const OriginalUiMemberView50KG&,
    Texture48E&,DecodeError48C&) noexcept;
}

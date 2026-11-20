#pragma once
#include "original_ui_archive50kg.h"
#include "import_texture48e.h"
namespace rr::assets {
// Explicit native image policy for the four initially unlocked Course backgrounds.
// Original2A6490 creates a raw texture, not a fabricated PSM ScreenTexture.
const char* originalUiCourseBackgroundName50KG(std::uint32_t planet)noexcept;
bool isOriginalUiCourseBackgroundIdentity50KG(const OriginalUiMemberIdentity50KG*)noexcept;
bool isOriginalUiCourseBackground50KG(const OriginalUiMemberView50KG&) noexcept;
bool decodeOriginalUiCourseBackground50KG(const OriginalUiMemberView50KG&,
    Texture48E&,DecodeError48C&) noexcept;
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR (DLC add-on courses, 146r-U): a package
// course_background file (dlc:/ path admitted by the DLC registry, the 640x448
// one-level layout of the stock *_bkg.pst) carries a port-owned identity whose
// sha256 field points at this tag. isOriginalUiCourseBackground50KG and the
// decoder accept it with the same layout checks as the five stock members;
// isOriginalUiCourseBackgroundIdentity50KG stays catalog-only.
extern const char originalUiAddonCourseBackgroundTag50DL[];
bool isOriginalUiAddonCourseBackgroundIdentity50DL(const OriginalUiMemberIdentity50KG*)noexcept;
}

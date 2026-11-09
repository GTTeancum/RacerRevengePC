#pragma once
#include "original_ui_archive50kg.h"
#include "import_texture48e.h"
namespace rr::assets {
// UIUnlock gallery page backgrounds (+8C, 2A6530/2A6E90): artpod01, artpil01
// and artpla01 in UIUNLOCK.RES, complete 640x448 indexed PSTs like the Hall
// and Course backgrounds. Only these three authenticated members qualify.
bool isOriginalUiUnlockBackgroundIdentity50PD(const OriginalUiMemberIdentity50KG*)noexcept;
bool isOriginalUiUnlockBackground50PD(const OriginalUiMemberView50KG&)noexcept;
bool decodeOriginalUiUnlockBackground50PD(const OriginalUiMemberView50KG&,
    Texture48E&,DecodeError48C&)noexcept;
}

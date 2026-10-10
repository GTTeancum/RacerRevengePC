#pragma once
#include "original_ui_pages50kg.h"

namespace rr::assets {
struct OriginalUiIconRect50KG { std::int32_t left=0,top=0,right=0,bottom=0; };
struct OriginalUiIcon50KG {
    const OriginalUiMemberIdentity50KG* identity=nullptr;
    std::uint16_t widgetId=0;
    std::string textureName,materialName;
    OriginalUiIconRect50KG partialRect{},mapRect{};
    bool hasPartialRect=false,hasMapRect=false,hasAngle=false;
    std::int32_t x=0,y=0,alpha=255;
    float angle=0,pivotX=0,pivotY=0,depth=1;
    std::uint32_t color=0xffc8c8c8;
    bool enableOnSelect=false,disableOnSelect=false,enableOnDeselect=false,disableOnDeselect=false;
    std::array<bool,2> wantsJoystickInput{{false,false}};
};
// Actual Icon constructor/create2AC4B0/2AC590. Authored rectangles retain the
// original token order left,right,top,bottom (158E60), not CSS/XYWH order.
// Missing rectangles remain absent until the real cold-bound atlas dimensions
// are bound; no placeholder image size enters this descriptor.
bool readOriginalUiIcon50KG(const OriginalUiDat50KG&,std::uint16_t widgetId,
    OriginalUiIcon50KG&,OriginalUiPageDiagnostic50KG&) noexcept;
}

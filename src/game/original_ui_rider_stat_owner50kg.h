#pragma once
#include "../assets/original_ui_rider_stat_assets50kg.h"
#include "original_ui_rider_stat_projection50kg.h"
#include "original_ui_main_layout50kg.h"
#include "original_ui_text50kg.h"
#include "original_ui_texture_animation50kg.h"
namespace rr::game {
struct OriginalUiRiderStatPacket50KG {
    std::uint32_t objectOffset=0,imageIndex=0,sourcePC=0;
    const assets::OriginalUiRiderStatResource50KG* resource=nullptr;
    OriginalUiOverlayQuad50KG sprite;
    std::int8_t labelIndex=-1;
};
struct OriginalUiRiderStatFrame50KG {
    std::vector<OriginalUiRiderStatPacket50KG> packets;
    std::array<OriginalUiTextLayout50KG,6> labels;
    std::array<std::array<std::uint8_t,4>,6> labelColors{};
    std::int32_t lockedWidgetX=0,lockedWidgetY=0;
};
struct OriginalUiRiderStatClock50KG {bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;};
struct OriginalUiRiderStatOwnerDiagnostic50KG {
    enum class Error:std::uint8_t {none,alreadyBound,input,resource,appearance,clock,text,arithmetic,allocation} error=Error::none;
    std::uint32_t objectOffset=0;
    OriginalUiTextDiagnostic50KG text;
    assets::OriginalUiPageDiagnostic50KG parse;
};
// Live custom ScreenTexture/ScreenText population, independent of DAT widgets.
// Actual appearance1EB640, persistent constructor dimensions, source marker
// unique animation and ordered1EBBF0 draw packets. No GPU/span publication.
// Returned frame borrows this owner's pinned actual assets. Drain it before
// replacing/destroying the owner; packet indices require combined-bank remap.
class OriginalUiRiderStatOwner50KG final {
public:
    OriginalUiRiderStatOwner50KG() noexcept;~OriginalUiRiderStatOwner50KG();
    OriginalUiRiderStatOwner50KG(const OriginalUiRiderStatOwner50KG&)=delete;
    OriginalUiRiderStatOwner50KG& operator=(const OriginalUiRiderStatOwner50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG>,OriginalUiRiderStatOwnerDiagnostic50KG&) noexcept;
    bool applyAppearance(const OriginalUiRiderStatInput50KG&,OriginalUiRiderStatOwnerDiagnostic50KG&) noexcept;
    bool project(const OriginalUiRiderStatInput50KG&,const OriginalUiRiderStatClock50KG&,
        OriginalUiRiderStatFrame50KG&,OriginalUiRiderStatOwnerDiagnostic50KG&) noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}

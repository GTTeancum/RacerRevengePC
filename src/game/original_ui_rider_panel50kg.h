#pragma once
#include "original_ui_aperture_owner50kg.h"
#include "original_ui_rider_dynamic_owner50kg.h"
namespace rr::game {
// Actual page4DC panel and4E0/4E4 transition gate, not route readiness.
// The session owns input/profile/closing hooks and must invoke bypass only at
// the actual source Back/second-confirm callback, never to skip missing assets.
class OriginalUiRiderPanel50KG final {
public:
    bool bind(OriginalUiApertureOwner50KG&,const OriginalUiRiderSelectionData50KG&,std::uint8_t player) noexcept;
    void entry74() noexcept;
    void acceptedBack() noexcept;
    void confirmWhileClosing() noexcept;
    OriginalUiApertureStatus50KG begin7C(OriginalUiTransitionPhase50KG) noexcept;
    // PORT EXTENSION (DLC): addonCharacter, when given, is the shown add-on
    // character whose own _sel readiness gates the exit (in place of the anchor's).
    bool update38(float,OriginalUiTransitionLifecycle50KG,OriginalUiTransitionPhase50KG,
        const OriginalUiNavigation50KG&,OriginalUiRiderDynamicOwner50KG&,
        const OriginalUiRiderCharacterScene50KG* addonCharacter=nullptr) noexcept;
    OriginalUiTransitionCompletion50KG pollCompletion(OriginalUiTransitionPhase50KG)const noexcept;
    float offset4DC()const noexcept{return offset_;}
    bool inputBlocked4E0()const noexcept{return blocked_;}
    bool bypass4E4()const noexcept{return bypass_;}
private:
    OriginalUiApertureOwner50KG* aperture_=nullptr;
    const assets::OriginalUiPage50KG* page_=nullptr;
    std::uint8_t player_=0;
    // Constructor1E7C1C/1E7C40, retained across later page entry callbacks.
    float offset_=140;
    bool blocked_=false,bypass_=false;
};
}

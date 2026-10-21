#pragma once
#include "original_ui_options_menu_controller50kg.h"

namespace rr::game {
// Native reconstruction of authenticated SLUS_202.68 Credits12/6, not guest execution.
struct OriginalUiCreditsConfig50KG {
    std::int32_t lecBeginControl7DC=-1; // Despite its name this is a GROUP SLOT.
    float fadeIn7E0=1,fadeOut7E4=1,on7E8=3,off7EC=.25f;
};
struct OriginalUiCreditsState50KG {
    OriginalUiNavigation50KG navigation{};
    bool loaded=false;
    std::uint8_t phase7D4=0; // 0 fade in, 1 hold, 2 fade out, 3 off.
    float remaining7D8=0;
    std::uint32_t inputFlags42C=0;
};
struct OriginalUiCreditsAlpha50KG {
    std::uint16_t widgetId=0;
    std::uint8_t alpha=0;
    std::uint32_t sourcePC=0;
};
enum class OriginalUiCreditsIntent50KG : std::uint8_t {none,optionsPage1,unlockScreen10,garageScreen13};
struct OriginalUiCreditsPlan50KG {
    OriginalUiCreditsState50KG state{};
    // Credits-only bound. Authenticated entry emits 3+188+188+188+14=581
    // widget callbacks. No existing controller's default capacity is changed.
    std::array<OriginalUiOptionsMenuCallback50KG,640> callbacks{};
    std::size_t callbackCount=0;
    // Preserve the source ordering: entry's three direct disables precede
    // alpha reset, then group callbacks; card advance writes alpha first.
    std::array<OriginalUiCreditsAlpha50KG,192> alphaWrites{};
    std::size_t alphaCount=0;
    std::size_t alphaAfterCallbackCount=0;
    bool writesPageInputFlags42C=false;
    std::uint32_t pageInputFlags42C=0;
    bool pageAccepted=false,soundRequested=false,flushRequested=false;
    std::uint8_t soundId=0;
    std::uint32_t soundPC=0,flushPC=0;
    bool activateBorrowedCamera=false,clearSceneField3D8=false,applyPageSceneField430=false;
    bool startMusic12=false,fadeMusic=false;
    float musicFadeSeconds=0;
    std::uint32_t musicPC=0;
    // Proposals only. Caller must preflight route and side-effect owners before
    // publishing state, alpha, callbacks, music or audio. No guessed admission.
    OriginalUiCreditsIntent50KG intent=OriginalUiCreditsIntent50KG::none;
    std::uint32_t requestPC=0;
    bool completionPredicateRequired=false;
    bool saveUpdateDelta298=false;
    float updateDelta298=0;
};
struct OriginalUiCreditsRender50KG {
    // Four mmbkg quadrants, +710/+770 permanent logolec layers, then conditional
    // logo/text alpha work, then shared aperture1D7550. Geometry stays external.
    bool drawBackground=true,drawPermanentLogos=true,drawCredits=false,renderAperture=true;
    std::uint16_t selectedSlot=0,controlId=0;
    std::uint8_t cardAlpha=0,rainAlpha=0,lecAlpha=0;
    bool drawRainLogo=false,drawLecLogo=false;
};
class OriginalUiCreditsController50KG final {
public:
    OriginalUiCreditsController50KG() noexcept;
    ~OriginalUiCreditsController50KG();
    OriginalUiCreditsController50KG(const OriginalUiCreditsController50KG&)=delete;
    OriginalUiCreditsController50KG& operator=(const OriginalUiCreditsController50KG&)=delete;
    OriginalUiOptionsMenuStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,assets::OriginalUiPageDiagnostic50KG&) noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    const OriginalUiCreditsConfig50KG* config() const noexcept;
    bool initialState(OriginalUiCreditsState50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG phase(const OriginalUiCreditsState50KG&,OriginalUiOptionsMenuPhase50KG,
        bool originalProfileKnown,std::uint32_t mode34,OriginalUiCreditsPlan50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG input(const OriginalUiCreditsState50KG&,std::uint8_t controller,
        assets::OriginalUiMovementInput50KG,bool containerReached,bool pageInputEnabled268,
        OriginalUiCreditsPlan50KG&) const noexcept;
    // managerPhase170 1/2 pauses card timing. Predicate is the actual187E30
    // result, needed only for unskippable mode2 completion, never an inference.
    OriginalUiOptionsMenuStatus50KG update(const OriginalUiCreditsState50KG&,float delta,
        std::uint32_t managerPhase170,bool completionPredicateKnown,bool completionPredicate,
        OriginalUiCreditsPlan50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG render(const OriginalUiCreditsState50KG&,
        std::uint32_t managerPhase170,OriginalUiCreditsRender50KG&) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
// Describes the exact uncommitted destination. Destination admission remains
// false and unavailable until the actual host transaction verifies its owners.
bool describeOriginalUiCreditsReturn50KG(const OriginalUiCreditsPlan50KG&,OriginalUiRouteDecision50KG&) noexcept;
// Bounded Options return only. The mode2 Unlock/Garage branches remain outside
// this admission shape until their real profile/destination owners exist.
bool originalUiCreditsReturnShape50KG(const OriginalUiRouteDecision50KG&) noexcept;
}

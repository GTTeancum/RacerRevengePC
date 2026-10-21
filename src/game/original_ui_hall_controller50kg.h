#pragma once
#include "original_ui_hall_records50kg.h"
#include "original_ui_options_menu_controller50kg.h"
namespace rr::game {
enum class OriginalUiHallPhase50KG : std::uint8_t {requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4,closedB8};
struct OriginalUiHallState50KG {
    OriginalUiNavigation50KG navigation{};
    bool loaded=false;
    std::uint32_t inputFlags42C=0;
    float deferred1460=0;
};
// Pure original-source proposal. No callback/effect readiness is implied by a
// successful plan. Session must preflight retained profile, video, audio and
// draw owners before committing the ordered effects.
struct OriginalUiHallPlan50KG {
    OriginalUiHallState50KG state{};
    std::array<OriginalUiOptionsMenuCallback50KG,256> callbacks{};
    std::size_t callbackCount=0;
    bool writesInputFlags=false,activateCamera=false,clearScene3D8=false,applyBackground430=false;
    bool startSong5=false,fadeMusic=false;
    bool writePlanet=false;std::uint32_t planet=0,planetPC=0;
    bool pauseVideo=false;std::size_t pauseAfterCallbackCount=0;
    bool resumeVideo=false,drainPreviousFrame=false,setVideoEffect250=false;
    bool soundRequested=false,flushRequested=false,pageAccepted=false;
    std::uint32_t soundId=0,soundPC=0,flushPC=0;
    bool backRequested=false;OriginalUiRouteDecision50KG back{};
    bool saveUpdateDelta=false;float updateDelta=0;
};
class OriginalUiHallOfFameController50KG final {
public:
    OriginalUiHallOfFameController50KG() noexcept;
    ~OriginalUiHallOfFameController50KG();
    OriginalUiHallOfFameController50KG(const OriginalUiHallOfFameController50KG&)=delete;
    OriginalUiHallOfFameController50KG& operator=(const OriginalUiHallOfFameController50KG&)=delete;
    OriginalUiOptionsMenuStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,assets::OriginalUiPageDiagnostic50KG&) noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    bool initialState(OriginalUiHallState50KG&) const noexcept;
    bool selectedCourse(const OriginalUiHallState50KG&,std::uint32_t& course,std::uint32_t& planet) const noexcept;
    OriginalUiOptionsMenuStatus50KG phase(const OriginalUiHallState50KG&,OriginalUiHallPhase50KG,const OriginalUiHallProfile50KG&,OriginalUiHallPlan50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG input(const OriginalUiHallState50KG&,std::uint8_t controller,assets::OriginalUiMovementInput50KG,bool containerReached,bool inputEnabled,const OriginalUiHallProfile50KG&,OriginalUiHallPlan50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG update(const OriginalUiHallState50KG&,float elapsedSeconds,OriginalUiHallPlan50KG&) const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
bool originalUiHallBackShape50KG(const OriginalUiRouteDecision50KG&) noexcept;
}

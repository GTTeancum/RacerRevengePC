#pragma once
#include "original_ui_hall_records50kg.h"
#include "original_ui_options_menu_controller50kg.h"
namespace rr::game {
enum class OriginalUiHallPhase50KG : std::uint8_t {requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4,closedB8};
// Hall Cross destinations. win/unlock/garage/course are page vtable+A0 screen
// requests (11/1 -> N/1). ending is 1E40A0: page1468=1, 4B4=3, 268=0, close
// doors (vtable+B4); closed B8 hook 1E4980 then calls 17DE10(App,0x28,3,0).
// assertion is PodUIPages.cpp:2989 ("didn't finish the last track in a
// circuit"); with 133660 disabled it reaches only sound1 at 1E41E4.
enum class OriginalUiHallConfirmRoute50KG : std::uint8_t {none,win,ending,unlock,garage,course,assertion};
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
    // 1E3FC0..1E41C8 Cross (event4, 42C bit0). The route request precedes
    // sound1 (1E41E4) and its flush (1E41FC), as Back's does.
    bool confirmRequested=false;OriginalUiHallConfirmRoute50KG confirmRoute=OriginalUiHallConfirmRoute50KG::none;
    OriginalUiRouteDecision50KG confirm{};
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
// Exact Cross screen-request shape: 11/1 -> 7/1 (1E4048 or 1E40EC), 10/1
// (1E418C), 13/1 (1E41AC) or 6/1 (1E41C4, non-tournament); no profile write.
bool originalUiHallConfirmShape50KG(const OriginalUiRouteDecision50KG&) noexcept;
}

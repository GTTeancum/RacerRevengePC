#pragma once
#include "original_ui_options_menu_controller50kg.h"

namespace rr::game {
// Pure bounded source-state reconstruction, not storage or renderer readiness.
// Its owning bank must authenticate Options memcard.dat before admitting it.
struct OriginalUiMemoryCardStrip50KG { float x=0,y=0,width=640,height=0; };
struct OriginalUiMemoryCardState50KG {
    bool entered=false;
    std::uint32_t inputFlags42C=0;
    bool callback4C8=false,inputBlocked4CC=false,load4D4=false,save4D8=false;
    float pollTimer4DC=0,updateDelta298=0;
    bool dialogActive=false;
    std::uint32_t dialogState30=0;
    float timer398=0,timer39C=0,delta3A0=0;
    bool completed3A4=false,closeOnDone3A8=false,closing3AC=false,animate3B0=false,resetMode3B4=false;
    std::int32_t value3B8=0;
    std::array<std::int32_t,9> textIds{{-1,-1,-1,-1,-1,-1,-1,-1,-1}};
    std::array<std::uint8_t,4> color3E0{};
    std::array<OriginalUiMemoryCardStrip50KG,3> strips{};
};
enum class OriginalUiMemoryCardStatus50KG : std::uint8_t {
    ok,noChange,unavailableStorage,invalidState,invalidInput,invalidDelta
};
struct OriginalUiMemoryCardPlan50KG {
    OriginalUiMemoryCardState50KG state{};
    std::array<OriginalUiOptionsMenuCallback50KG,6> callbacks{};
    std::size_t callbackCount=0;
    std::uint32_t sourcePC=0;
    bool pageAccepted=false,writesPageInputFlags42C=false;
    std::uint32_t pageInputFlags42C=0;
    bool soundRequested=false,flushRequested=false,soundForce=false;
    std::uint8_t soundId=0,soundFlags=0; // 292810: a2 bit1, a3(force) bit2.
    std::uint32_t soundPC=0,flushPC=0;
    bool activateBorrowedCamera=false,applyPageSceneField430=false;
    // 1F6A70..84: APP+2D8 -> UI+47C -> controller popup result+3D8.
    bool resetGlobalPopupResult3D8=false;
    bool enableOwnedDialog=false,resetOwnedDialogText=false;
    bool saveUpdateDelta298=false,updateDialogChildren=false;
    bool returnToOptions=false;
    std::uint32_t requestPC=0;
};
struct OriginalUiMemoryCardRender50KG {
    // Page callbacks occur before aperture/prompts. Dialog operations below
    // occur afterwards at the actual final child render, not widget rendering.
    OriginalUiMemoryCardPlan50KG page{};
    OriginalUiMemoryCardState50KG state{}; // publish only after ALL real effects
    bool renderAperture=true,drawDialog=false,drawStrips=false;
    bool drawDialogText=false,drawDialogIcons=false; // false in state4/-1 scope
    std::array<OriginalUiMemoryCardStrip50KG,3> strips{};
    // Mandatory original render effect, NOT an initial-frame-clear substitute.
    // 200C5C color/ALPHA setup; 200C98→25B1A0(1)→25E610 direct GS sprite;
    // 200CB0 renderer virtual4C restore, all before strip submission200CFC.
    bool requiresDirectVideoSprite=false;
    std::uint64_t directAlpha=0x8000000009ULL;
    std::uint32_t directSetupPC=0x200C88,directSpritePC=0x200C98,directRestorePC=0x200CB0;
    std::uint32_t sourcePC=0x1F6DE0,dialogSourcePC=0x1FFFD0;
};
class OriginalUiMemoryCard50KG final {
public:
    static OriginalUiMemoryCardState50KG initialState() noexcept;
    static bool valid(const OriginalUiMemoryCardState50KG&) noexcept;
    // Requested/previous callbacks cover the authenticated static control0
    // widgets2,16 only; entity/aperture owner effects remain with the caller.
    static OriginalUiMemoryCardStatus50KG phase(const OriginalUiMemoryCardState50KG&,
        OriginalUiOptionsMenuPhase50KG,OriginalUiMemoryCardPlan50KG&) noexcept;
    static OriginalUiMemoryCardStatus50KG callback(const OriginalUiMemoryCardState50KG&,
        std::uint32_t event,OriginalUiMemoryCardPlan50KG&) noexcept;
    // Actual page/container delivery gates. There is no guessed phase gate.
    // Source5/7 reject with unchanged state and zero effects, before mutation.
    static OriginalUiMemoryCardStatus50KG input(const OriginalUiMemoryCardState50KG&,
        std::uint8_t controller,assets::OriginalUiMovementInput50KG,
        bool containerReached,bool pageInputEnabled268,OriginalUiMemoryCardPlan50KG&) noexcept;
    // Page update checks completion BEFORE updating child delta. No animation
    // timer advances here. Render suppression must not advance dialog timers.
    static OriginalUiMemoryCardStatus50KG update(const OriginalUiMemoryCardState50KG&,
        float delta,OriginalUiMemoryCardPlan50KG&) noexcept;
    static OriginalUiMemoryCardStatus50KG render(const OriginalUiMemoryCardState50KG&,
        OriginalUiMemoryCardRender50KG&) noexcept;
};
bool sameOriginalUiMemoryCardState50KG(const OriginalUiMemoryCardState50KG&,const OriginalUiMemoryCardState50KG&) noexcept;
bool describeOriginalUiMemoryCardReturn50KG(const OriginalUiMemoryCardPlan50KG&,OriginalUiRouteDecision50KG&) noexcept;
bool originalUiMemoryCardReturnShape50KG(const OriginalUiRouteDecision50KG&) noexcept;
}

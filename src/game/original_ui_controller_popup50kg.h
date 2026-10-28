#pragma once
#include <array>
#include <cstdint>

namespace rr::game {
// Authenticated SLUS_202.68 PodUIControllerPopupDialog at application UI+47C.
// This is a retained shared overlay, not an Options screen or a route request.
struct OriginalUiControllerPopupStrip50KG {
    float x=0,y=0,width=640,height=0;
};
struct OriginalUiControllerPopupState50KG {
    // Bounded ownership of authenticated2060A0 text arguments, not a guest field.
    bool garagePurchaseText=false;
    float delta3C0=0,timer3C4=0;
    bool closing3C8=false,active3CC=false,hasConfirm3D0=false,hasCancel3D4=false;
    std::uint32_t result3D8=0; // 0 reset, 1 awaiting input, 2 Confirm, 3 Cancel.
    std::array<OriginalUiControllerPopupStrip50KG,3> strips{};
};
struct OriginalUiControllerPopupPlan50KG {
    OriginalUiControllerPopupState50KG state{};
    bool accepted=false,soundRequested=false,flushRequested=false;
    std::uint8_t soundId=0;
    bool soundForce=false;
    std::uint32_t sourcePC=0,soundPC=0,flushPC=0;
};
struct OriginalUiControllerPopupRender50KG {
    OriginalUiControllerPopupState50KG state{}; // Publish only after draw succeeds.
    bool drawDim=false,drawMessage=false,drawConfirm=false,drawCancel=false,drawStrips=false;
    std::array<OriginalUiControllerPopupStrip50KG,3> strips{};
    std::uint32_t sourcePC=0x2064C0;
};
enum class OriginalUiControllerPopupStatus50KG : std::uint8_t {ok,invalidState,invalidDelta};
class OriginalUiControllerPopup50KG final {
public:
    static constexpr std::uint16_t controller2TextId=88,confirmTextId=90;
    static OriginalUiControllerPopupState50KG initialState() noexcept;
    // 207330, called before detach/reappend to the shared manager child tail.
    static OriginalUiControllerPopupStatus50KG attach(const OriginalUiControllerPopupState50KG&,OriginalUiControllerPopupPlan50KG&) noexcept;
    // Options entry clears ONLY the result field at 1F9614.
    static OriginalUiControllerPopupStatus50KG optionsEntryResetResult(const OriginalUiControllerPopupState50KG&,OriginalUiControllerPopupPlan50KG&) noexcept;
    static OriginalUiControllerPopupStatus50KG startController2(const OriginalUiControllerPopupState50KG&,OriginalUiControllerPopupPlan50KG&) noexcept;
    static OriginalUiControllerPopupStatus50KG startGaragePurchase(const OriginalUiControllerPopupState50KG&,OriginalUiControllerPopupPlan50KG&) noexcept;
    // sourceInput is the resolved logical input (Confirm4/Back6). eventKind2
    // means an event accepted by 127870(...,2,event,...), not a polled hold.
    // controller is the original event object's +8 controller field.
    static OriginalUiControllerPopupStatus50KG input(const OriginalUiControllerPopupState50KG&,std::uint8_t controller,std::uint8_t sourceInput,bool eventKind2,OriginalUiControllerPopupPlan50KG&) noexcept;
    // Original 206420 only saves delta. There is no popup pause/phase gate.
    static OriginalUiControllerPopupStatus50KG update(const OriginalUiControllerPopupState50KG&,float delta,OriginalUiControllerPopupPlan50KG&) noexcept;
    // Original 2064C0 computes geometry from the old timer, decrements by saved
    // delta, then snaps on crossing <=0. Inactive calls never advance the timer.
    static OriginalUiControllerPopupStatus50KG render(const OriginalUiControllerPopupState50KG&,OriginalUiControllerPopupRender50KG&) noexcept;
    static bool valid(const OriginalUiControllerPopupState50KG&) noexcept;
};
}

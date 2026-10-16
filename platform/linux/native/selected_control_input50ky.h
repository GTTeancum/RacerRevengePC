#pragma once
#include "keyboard_packet50dr.h"
#include "window_host50dq.h"
#include "../../../src/game/control_presets49d.h"

namespace rr::platform {
// Native host transport policy, not a recovered PC keyboard layout. Original
//49D remains the sole action-row/configuration owner,48X the input reader, and
//48W the normal/twin-engine binder. No driving-input generator is introduced.
struct SelectedControlInput50KY {
    KeyboardPacketMapping50DR keyboard{};
    game::SelectedControlPreset49D selected{};
    std::uint32_t preset=0;
    bool twinEngine=false,cameraCycleBound=false,manualResetBound=false;
    std::int32_t cameraCycleCode=-1,manualResetCode=-1;
};
inline bool bindSelectedControlInput50KY(const game::ControlPresets49D& preferences,
    std::uint32_t actionSlot,std::uint32_t configurationSlot,SelectedControlInput50KY& out) noexcept {
    if(actionSlot>=game::controlPresetSlots49D||configurationSlot>=game::controlPresetSlots49D)return false;
    const auto& slot=preferences.slots[actionSlot];
    const auto configuration=preferences.slots[configurationSlot].configuration11040;
    // Original invalid-index retention remains available in49D. Human host
    // binding admits only canonical exposed rows, never guesses retained data.
    if(slot.preset11048>=8||configuration>1)return false;
    game::ControlPresets49D canonical{};
    if(!game::selectControlPreset49D(canonical,0,slot.preset11048))return false;
    const auto& a=slot.actions;const auto& b=canonical.slots[0].actions;
    if(a.action690!=b.action690||a.action694!=b.action694||a.flag698!=b.flag698||a.flag69C!=b.flag69C||
       a.action6A0!=b.action6A0||a.action6A4!=b.action6A4||a.action6A8!=b.action6A8||a.action6AC!=b.action6AC||
       a.action6B0!=b.action6B0||a.flag6B4!=b.flag6B4||configuration!=(slot.preset11048>=4?1u:0u))return false;
    SelectedControlInput50KY result{};
    if(!game::resolveControlPreset49D(preferences,actionSlot,configurationSlot,result.selected))return false;
    result.preset=slot.preset11048;result.twinEngine=result.selected.configurationEntryNonzero;
    for(auto& axis:result.keyboard.axes)axis.source=KeyboardAxisSource50DR::Centered;
    for(auto& button:result.keyboard.buttons)button.source=KeyboardButtonSource50DR::Released;
    auto key=[&](std::int32_t code,std::uint16_t scancode) noexcept {
        if(code<0||code>=static_cast<std::int32_t>(game::dualShockActiveButtons48Y))return false;
        auto& destination=result.keyboard.buttons[static_cast<std::size_t>(code)];
        if(destination.source!=KeyboardButtonSource50DR::Released)return false;
        destination={KeyboardButtonSource50DR::Key,scancode};return true;
    };
    if(!key(a.flag69C,21)||!key(a.flag698,44)||!key(a.flag6B4,225))return false; //R,Space,Left Shift.
    if(result.twinEngine){
        // packet7 = left Y; packet5 = right Y. Negative drives that engine,
        // positive brakes it. Preserve independent input and original shaping.
        result.keyboard.axes[3]={KeyboardAxisSource50DR::KeyPair,26,22,KeyboardOpposingKeys50DR::Center};
        result.keyboard.axes[1]={KeyboardAxisSource50DR::KeyPair,82,81,KeyboardOpposingKeys50DR::Center};
    }else{
        // W/S select the actual row's throttle/brake descriptors. Left/Right
        // use packet6 (left X); Up/Down use packet7 (left Y), retaining the
        // original standard-mode axis consumer rather than twin-throttle math.
        if(!key(a.action694,26)||!key(a.action690,22))return false;
        result.keyboard.axes[2]={KeyboardAxisSource50DR::KeyPair,80,79,KeyboardOpposingKeys50DR::Center};
        result.keyboard.axes[3]={KeyboardAxisSource50DR::KeyPair,82,81,KeyboardOpposingKeys50DR::Center};
    }
    out=result;return true;
}
// Invoke only after the same human held/event owner has acquired its real
// prepared camera child. Merely selecting a preset does not grant that owner.
inline bool bindSelectedCameraCycle50KY(const game::ControlPresets49D& preferences,
    std::uint32_t slot,SelectedControlInput50KY& binding) noexcept {
    if(binding.cameraCycleBound)return false;
    SelectedControlInput50KY current;
    if(!bindSelectedControlInput50KY(preferences,slot,slot,current)||
       current.preset!=binding.preset||current.twinEngine!=binding.twinEngine)return false;
    const auto code=preferences.slots[slot].actions.action6A4;
    if(code<0||code>=static_cast<std::int32_t>(game::dualShockActiveButtons48Y)||
       current.keyboard.buttons[static_cast<std::size_t>(code)].source!=KeyboardButtonSource50DR::Released)return false;
    current.keyboard.buttons[static_cast<std::size_t>(code)]={KeyboardButtonSource50DR::Key,6}; // Native C.
    current.cameraCycleCode=code;current.cameraCycleBound=true;binding=current;return true;
}
// Bound only after genuine1A16F0 ->2255B0, physical owners and camera exist.
// Native T is transport policy; descriptor8 remains the original SELECT input.
inline bool bindSelectedManualReset50LC(const game::ControlPresets49D& preferences,
    std::uint32_t slot,SelectedControlInput50KY& binding) noexcept {
    if(binding.manualResetBound||slot>=game::controlPresetSlots49D)return false;
    SelectedControlInput50KY current;
    if(!bindSelectedControlInput50KY(preferences,slot,slot,current)||
       current.preset!=binding.preset||current.twinEngine!=binding.twinEngine)return false;
    const auto code=preferences.slots[slot].actions.action6AC;
    if(code<0||code>=static_cast<std::int32_t>(game::dualShockActiveButtons48Y)||
       binding.keyboard.buttons[static_cast<std::size_t>(code)].source!=KeyboardButtonSource50DR::Released)return false;
    binding.keyboard.buttons[static_cast<std::size_t>(code)]={KeyboardButtonSource50DR::Key,23};
    binding.manualResetCode=code;binding.manualResetBound=true;return true;
}
inline bool selectedKeyboardHeld50KY(const WindowInput50DQ& input,bool cameraCycleBound=false,
    bool manualResetBound=false) noexcept {
    constexpr std::uint16_t keys[]={26,22,82,81,80,79,44,21,225};
    for(auto key:keys)if(input.down[key])return true;
    return (cameraCycleBound&&input.down[6])||(manualResetBound&&input.down[23]);
}
inline void releaseSelectedKeyboard50KY(WindowInput50DQ& input,bool cameraCycleBound=false,
    bool manualResetBound=false) noexcept {
    constexpr std::uint16_t keys[]={26,22,82,81,80,79,44,21,225};
    for(auto key:keys)input.down[key]=input.pressed[key]=0;
    if(cameraCycleBound)input.down[6]=input.pressed[6]=0;
    if(manualResetBound)input.down[23]=input.pressed[23]=0;
}
// SDL's existing controller mapping database supplies logical controls. This
// explicit positional native policy targets the original48Y descriptors:
// A/B/X/Y -> Cross/Circle/Square/Triangle, shoulders/triggers -> L1/R1/L2/R2.
// Public libpad packet layout corroborates48Y's pressure offsets; no original
// joystick driver, guest code, hardware connection, or analog face pressure is
// emulated. SDL digital buttons contribute0/255; actual triggers retain value.
inline std::uint8_t controllerStickByte50KY(std::int16_t value) noexcept {
    return static_cast<std::uint8_t>((static_cast<std::int32_t>(value)+32768)/256);
}
inline std::uint8_t controllerTriggerByte50KY(std::int16_t value) noexcept {
    return value<=0?0:static_cast<std::uint8_t>((static_cast<std::uint32_t>(value)*255u+16383u)/32767u);
}
inline bool selectedControllerHeld50KY(const WindowGameController50DQ& input) noexcept {
    if(!input.connected)return false;
    for(auto button:input.buttons)if(button)return true;
    // Re-arm inside the existing48X central deadband, not exact analog zero.
    for(std::size_t axis=0;axis<4;++axis){const auto value=controllerStickByte50KY(input.axes[axis]);
        if(value<=102||value>=153)return true;}
    return controllerTriggerByte50KY(input.axes[4])>=26||controllerTriggerByte50KY(input.axes[5])>=26;
}
struct SelectedControllerLatch50KY {
    std::int32_t instanceId=-1;
    bool armed=false;
};
enum class SelectedControllerStatus50KY : std::uint8_t {
    live,releasedDisconnected,releasedInactive,awaitingNeutral,unsupportedOriginalAction,invalidSnapshot
};
struct SelectedControllerReceipt50KY {
    SelectedControllerStatus50KY status=SelectedControllerStatus50KY::releasedDisconnected;
    std::int32_t unsupportedCode=-1;
};
inline bool composeSelectedControllerPacket50KY(const SelectedControlInput50KY& binding,
    const WindowGameController50DQ& input,bool enabled,SelectedControllerLatch50KY& latch,
    game::DevicePacket48Y& out,SelectedControllerReceipt50KY& receipt) noexcept {
    receipt={};
    game::DevicePacket48Y packet{};packet.bytes[2]=packet.bytes[3]=0xff;
    for(unsigned i=4;i<8;++i)packet.bytes[i]=128;
    if(!input.connected){latch={};out=packet;return true;}
    if(input.instanceId<0){receipt.status=SelectedControllerStatus50KY::invalidSnapshot;return false;}
    if(latch.instanceId!=input.instanceId){latch.instanceId=input.instanceId;latch.armed=false;}
    if(!enabled){latch.armed=false;receipt.status=SelectedControllerStatus50KY::releasedInactive;out=packet;return true;}
    for(auto button:input.buttons)if(button>1){receipt.status=SelectedControllerStatus50KY::invalidSnapshot;return false;}
    if(input.axes[4]<0||input.axes[5]<0){receipt.status=SelectedControllerStatus50KY::invalidSnapshot;return false;}
    if(!latch.armed){
        latch.armed=!selectedControllerHeld50KY(input);
        if(!latch.armed){receipt.status=SelectedControllerStatus50KY::awaitingNeutral;out=packet;return true;}
    }
    // Array index is an original descriptor; values are SDL2 public button IDs.
    //4/5 are triggers and therefore sampled from axes below. GUIDE has no
    //original action identity and stays solely with the host/operating system.
    constexpr std::int8_t buttonMap[16]={3,1,0,2,-1,-1,9,10,4,6,8,7,11,14,12,13};
    std::uint8_t values[16]{};
    for(unsigned code=0;code<16;++code)if(buttonMap[code]>=0)
        values[code]=input.buttons[static_cast<unsigned>(buttonMap[code])]?255:0;
    values[4]=controllerTriggerByte50KY(input.axes[4]);values[5]=controllerTriggerByte50KY(input.axes[5]);
    // Start is owned by the explicitly labeled native host pause/menu edge,
    // never by an invented original pause callback. Keep it out of gameplay.
    values[9]=0;
    // Driving/steering and actually bound camera/reset calls are owned here.
    // Other look child routes remain guarded. Reject a reached missing action explicitly;
    // do not suppress it, substitute a HOST action, or fabricate callback success.
    const auto& actions=binding.selected.actions;
    for(unsigned code=0;code<16;++code){
        if(values[code]<26)continue;
        const auto signedCode=static_cast<std::int32_t>(code);
        const bool supported=code==13||code==15||signedCode==actions.flag698||
            signedCode==actions.flag69C||signedCode==actions.flag6B4||
            signedCode==actions.action694||signedCode==actions.action690||
            (binding.cameraCycleBound&&signedCode==binding.cameraCycleCode)||
            (binding.manualResetBound&&signedCode==binding.manualResetCode);
        if(!supported){receipt.status=SelectedControllerStatus50KY::unsupportedOriginalAction;
            receipt.unsupportedCode=signedCode;return false;}
    }
    packet.bytes[4]=controllerStickByte50KY(input.axes[2]);packet.bytes[5]=controllerStickByte50KY(input.axes[3]);
    packet.bytes[6]=controllerStickByte50KY(input.axes[0]);packet.bytes[7]=controllerStickByte50KY(input.axes[1]);
    const auto descriptors=game::initialDualShockDescriptors48Y();
    std::uint16_t inactive=0xffff;
    for(unsigned code=0;code<16;++code){const auto& descriptor=descriptors.records[code];
        if(descriptor.flags&2u)packet.bytes[descriptor.packetOffset]=values[code];
        else if(values[code])inactive=static_cast<std::uint16_t>(inactive&~descriptor.digitalMask);
    }
    packet.bytes[2]=static_cast<std::uint8_t>(inactive>>8);packet.bytes[3]=static_cast<std::uint8_t>(inactive);
    receipt.status=SelectedControllerStatus50KY::live;out=packet;return true;
}
}

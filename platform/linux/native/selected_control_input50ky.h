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
    //Canonical selected row retained independently of optional owner binding:
    //an intentionally unmapped descriptor is not a mapped-but-missing action.
    game::ControlActionRow49D sourceActions{};
    std::uint32_t preset=0;
    bool twinEngine=false,cameraCycleBound=false,manualResetBound=false;
    std::int32_t cameraCycleCode=-1,manualResetCode=-1;
    std::int32_t temporaryLookCodes[2]={-1,-1};
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
    result.sourceActions=a;
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
    //Revalidate source selection above, but preserve the caller's actual PC
    //transport and any already-owned manual reset instead of regenerating it.
    current.keyboard=binding.keyboard;
    current.manualResetBound=binding.manualResetBound;
    current.manualResetCode=binding.manualResetCode;
    const auto code=preferences.slots[slot].actions.action6A4;
    if(code<0||code>=static_cast<std::int32_t>(game::dualShockActiveButtons48Y)||
       current.keyboard.buttons[static_cast<std::size_t>(code)].source!=KeyboardButtonSource50DR::Released)return false;
    for(const auto& button:binding.keyboard.buttons)
        if(button.source==KeyboardButtonSource50DR::Key&&button.scancode==6)return false;
    for(const auto& axis:binding.keyboard.axes)
        if(axis.source==KeyboardAxisSource50DR::KeyPair&&
           (axis.negativeScancode==6||axis.positiveScancode==6))return false;
    current.keyboard.buttons[static_cast<std::size_t>(code)]={KeyboardButtonSource50DR::Key,6}; // Native C.
    current.cameraCycleCode=code;
    current.temporaryLookCodes[0]=preferences.slots[slot].actions.action6A8;
    current.temporaryLookCodes[1]=preferences.slots[slot].actions.action6B0;
    //Native PC adaptation only: held Q/E reach the SAME original temporary
    //view actions already owned by this prepared camera. The camera's actual
    //176DF0/1774A0 handlers own priority, offsets and release restoration.
    constexpr std::uint16_t lookKeys[]={20,8}; //SDL scancodes Q/E.
    for(unsigned i=0;i<2;++i){
        const auto look=current.temporaryLookCodes[i];
        if(look<0||look>=static_cast<std::int32_t>(game::dualShockActiveButtons48Y)||
           current.keyboard.buttons[static_cast<std::size_t>(look)].source!=KeyboardButtonSource50DR::Released||
           binding.keyboard.buttons[static_cast<std::size_t>(look)].source!=KeyboardButtonSource50DR::Released)return false;
        //Do not steal a caller's explicit PC mapping. Canonical bindings use
        //neither key; any conflicting custom mapping keeps binding unchanged.
        for(const auto& button:binding.keyboard.buttons)
            if(button.source==KeyboardButtonSource50DR::Key&&button.scancode==lookKeys[i])return false;
        for(const auto& axis:binding.keyboard.axes)
            if(axis.source==KeyboardAxisSource50DR::KeyPair&&
               (axis.negativeScancode==lookKeys[i]||axis.positiveScancode==lookKeys[i]))return false;
        current.keyboard.buttons[static_cast<std::size_t>(look)]={KeyboardButtonSource50DR::Key,lookKeys[i]};
    }
    current.cameraCycleBound=true;binding=current;return true;
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
    //Native T may not steal another already-configured PC action/axis.
    //Stage no mutation before all conflict checks; camera bindings survive.
    for(const auto& button:binding.keyboard.buttons)
        if(button.source==KeyboardButtonSource50DR::Key&&button.scancode==23)return false;
    for(const auto& axis:binding.keyboard.axes)
        if(axis.source==KeyboardAxisSource50DR::KeyPair&&
           (axis.negativeScancode==23||axis.positiveScancode==23))return false;
    binding.keyboard.buttons[static_cast<std::size_t>(code)]={KeyboardButtonSource50DR::Key,23};
    binding.manualResetCode=code;binding.manualResetBound=true;return true;
}
//Observe the SAME mapping consumed by50DR, including genuine optional-owner
//bindings and explicit PC customizations. No source action or key is inferred
//from a camera/reset Boolean. Invalid mapped scancodes fail neutral admission
//closed; packet composition retains its existing full validation/diagnostics.
inline bool selectedKeyboardHeld50KY(const WindowInput50DQ& input,
    const SelectedControlInput50KY& binding) noexcept {
    const auto held=[&](std::uint16_t key)noexcept {
        return key==0||key>=input.down.size()||input.down[key]!=0;
    };
    for(const auto& button:binding.keyboard.buttons){
        if(button.source==KeyboardButtonSource50DR::Released)continue;
        if(button.source!=KeyboardButtonSource50DR::Key||held(button.scancode))return true;
    }
    for(const auto& axis:binding.keyboard.axes){
        if(axis.source==KeyboardAxisSource50DR::Centered)continue;
        if(axis.source!=KeyboardAxisSource50DR::KeyPair||
           held(axis.negativeScancode)||held(axis.positiveScancode))return true;
    }
    return false;
}
inline void releaseSelectedKeyboard50KY(WindowInput50DQ& input,
    const SelectedControlInput50KY& binding) noexcept {
    const auto release=[&](std::uint16_t key)noexcept {
        if(key&&key<input.down.size()&&key<input.pressed.size())input.down[key]=input.pressed[key]=0;
    };
    for(const auto& button:binding.keyboard.buttons)
        if(button.source==KeyboardButtonSource50DR::Key)release(button.scancode);
    for(const auto& axis:binding.keyboard.axes)if(axis.source==KeyboardAxisSource50DR::KeyPair){
        release(axis.negativeScancode);release(axis.positiveScancode);
    }
    //Repeated mappings clear idempotently. Unrelated keys and the actual
    //released-edge observations remain untouched, as in the prior policy.
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
    // Both temporary-look actions retain their original selected descriptors.
    //48Z routes every descriptor edge;49A/176DF0 then query held actions,
    //not the edge's code. Preserve unmapped buttons in the original packet:
    //they must not drop throttle, pause, or invent a driving action. In
    //particular an extra edge still queries an already-held mapped action,
    //matching source dispatch rather than silently filtering that edge.
    //Only a MAPPED action missing its owner releases the whole packet and
    //disarms until neutral, with an explicit unsupportedOriginalAction receipt.
    const auto& actions=binding.selected.actions;
    const auto& row=binding.sourceActions;
    const std::int32_t mappedCodes[]={row.action690,row.action694,row.flag698,
        row.flag69C,row.action6A0,row.action6A4,row.action6A8,row.action6AC,
        row.action6B0,row.flag6B4,13,15};
    for(unsigned code=0;code<16;++code){
        if(values[code]<26)continue;
        const auto signedCode=static_cast<std::int32_t>(code);
        bool mapped=false;
        for(const auto mappedCode:mappedCodes)if(signedCode==mappedCode){mapped=true;break;}
        if(!mapped)continue; //Keep its pressure/digital bit and genuine event.
        const bool supported=code==13||code==15||signedCode==actions.flag698||
            signedCode==actions.flag69C||signedCode==actions.flag6B4||
            signedCode==actions.action694||signedCode==actions.action690||
            (binding.cameraCycleBound&&(signedCode==binding.cameraCycleCode||
                signedCode==binding.temporaryLookCodes[0]||signedCode==binding.temporaryLookCodes[1]))||
            (binding.manualResetBound&&signedCode==binding.manualResetCode);
        if(!supported){receipt.status=SelectedControllerStatus50KY::unsupportedOriginalAction;
            receipt.unsupportedCode=signedCode;latch.armed=false;out=packet;return true;}
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

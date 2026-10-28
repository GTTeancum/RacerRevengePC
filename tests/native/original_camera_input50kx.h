#pragma once
#include "original_camera_session50kx.h"
#include "../../src/game/controller_actions50ec.h"
#include "../../platform/linux/native/keyboard_packet50dr.h"
#include "../../platform/linux/native/window_host50dq.h"

namespace original_camera_input50KX {
// Explicit native keyboard transport: C feeds only the selected camera-cycle
// action. All other buttons and axes are released/centered on this independent
// device0. This real held bank never becomes any racer's driving reader.
//237DF0 supplies ordered edges;12815C dispatches EVERY press while176DF0
//queries the current held bank. No host key-repeat/cooldown/cycle-code filter.
class Owner final {
    rr::game::ControllerIngress50CX ingress_{rr::game::DeviceButtonBank48Y{}};
    std::array<rr::game::InputEvent48Z,32> eventStorage_{};
    rr::game::InputEventQueue48Z queue_{eventStorage_.data(),32,0};
    rr::game::ControllerActions50EC actions_{ingress_,queue_,0};
    rr::game::DeviceButtonDescriptors48Y descriptors_=rr::game::initialDualShockDescriptors48Y();
    rr::game::EventContext48Z context_{};
    rr::game::EventRecordSources48Z records_{};
    rr::game::ManagerHeldSources49A held_{};
    rr::game::EventChild48Z cameraChild_{rr::game::noEventChild48Z,1};
    rr::game::JoystickApplicationBridge48Z application_{};
    rr::game::ManagerEventCallbacks48Z callbacks_{};
    original_camera_session50KX::Session* session_=nullptr;
    const rr::game::ControlPresets49D* preferences_=nullptr;
    bool failed_=false,armed_=true;
    static rr::game::EventCallbackResult48Z child(void* opaque,std::uint32_t index,bool press,
        rr::game::InputEvent48Z& event,rr::game::DeviceButtonRecord48Y& record) noexcept {
        auto* self=static_cast<Owner*>(opaque);
        if(!self||index||!self->session_)return {false,0};
        return press?self->session_->press(self->held_,event,record):self->session_->release(self->held_,event,record);
    }
public:
    Owner(){
        records_.devices[0]={ingress_.buttons().records,32};held_.devices[0]=&ingress_.buttons();
        application_.root={0x600u,0,&cameraChild_,1};application_.childCallback=&child;
        application_.childContext=this;application_.visitBudget=1;
        callbacks_={&application_,rr::game::joystickReleaseBridge48Z,rr::game::joystickPressBridge48Z};
    }
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    bool bind(original_camera_session50KX::Session& session,const rr::game::ControlPresets49D& preferences) noexcept {
        if(session_||!session.ready())return false;session_=&session;preferences_=&preferences;return true;
    }
    bool sample(const rr::platform::WindowInput50DQ& window,bool enabled,std::uint8_t rootFlags) noexcept {
        using namespace rr::game;namespace p=rr::platform;
        if(!session_||!preferences_||failed_)return false;
        if(!enabled)armed_=false;
        else if(!window.down[6])armed_=true;
        enabled=enabled&&armed_; //Native pause/menu policy requires releasing C before re-arming.
        const auto code=preferences_->slots[0].actions.action6A4;
        if(code<0||code>=static_cast<std::int32_t>(dualShockActiveButtons48Y)){failed_=true;return false;}
        p::KeyboardPacketMapping50DR mapping{};
        for(auto& axis:mapping.axes)axis.source=p::KeyboardAxisSource50DR::Centered;
        for(auto& button:mapping.buttons)button.source=p::KeyboardButtonSource50DR::Released;
        mapping.buttons[static_cast<std::size_t>(code)]={p::KeyboardButtonSource50DR::Key,6}; //Native C.
        DevicePacket48Y base{},packet{};base.bytes[2]=base.bytes[3]=0xff;
        p::KeyboardPacketDiagnostic50DR diagnostic;
        if(!p::composeKeyboardPacket50DR({true,enabled},{window.down.data(),window.down.size()},mapping,
            p::KeyboardUnavailablePolicy50DR::ReleaseAll,base,packet,diagnostic)){failed_=true;return false;}
        application_.rootFlags228=rootFlags;held_.rootFlags228=rootFlags;
        if(actions_.samplePacket(true,true,packet,descriptors_,dualShockActiveButtons48Y)!=ControllerActionStatus50EC::Ok||
           ingress_.deviceSample().status!=DevicePacketStatus48Y::Accepted||
           actions_.appendSample()!=ControllerActionStatus50EC::Ok){failed_=true;return false;}
        const auto dispatched=actions_.dispatch(context_,records_,callbacks_);
        if(dispatched.status!=ControllerActionStatus50EC::Ok||!dispatched.dispatchAttempted||
           dispatched.dispatch.status!=EventDispatchStatus48Z::Complete||
           actions_.acknowledgeComplete()!=ControllerActionStatus50EC::Ok){failed_=true;return false;}
        return true;
    }
};
}

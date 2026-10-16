#pragma once
#include "window_host50dq.h"
#include "../../../src/assets/original_ui_movement_control50kg.h"
#include <array>

namespace rr::platform {
// Native physical-device translation into existing2A7410/2AB010 input IDs.
// No DevicePacket48Y, racer/control owner, controller-presence capability,
// analog threshold, timer, repeated input or synthesized button edge is created.
class OriginalUiControllerNavigation50LA final {
    std::int32_t instance_=-1;
    bool armed_=false;
public:
    using Events=std::array<bool,8>;
    bool observe(const WindowInput50DQ& input,bool active,Events& events)noexcept {
        events={};const auto& controller=input.controllers[0];
        if(!input.gameControllersEnabled||!controller.connected){instance_=-1;armed_=false;return true;}
        if(controller.instanceId<0)return false;
        const bool changed=instance_!=controller.instanceId;
        if(changed){instance_=controller.instanceId;armed_=false;}
        // Same activity gates as keyboard, plus a batch crossing focus loss.
        // Reconnect and resume must observe released navigation buttons first.
        if(!active||!input.focused||input.minimized||input.activityLost||
           input.eventBudgetExhausted||input.quitRequested){armed_=false;return true;}
        // SDL public mapped button IDs: DPAD left/right/up/down, A/X/Y/B.
        // Source right-pad UP is Back6; right-pad RIGHT is event7, not Back.
        constexpr unsigned buttons[]={13,14,11,12,0,2,3,1};
        static_assert(static_cast<unsigned>(assets::OriginalUiMovementInput50KG::rpadRight)==7,
            "original menu event order must remain authenticated");
        bool neutral=true;
        for(const auto button:buttons){
            if(controller.buttons[button]>1||controller.pressed[button]>1||controller.released[button]>1)return false;
            neutral=neutral&&!controller.buttons[button]&&!controller.pressed[button];
        }
        if(changed||!armed_){armed_=neutral;return true;}
        for(unsigned event=0;event<events.size();++event)
            events[event]=controller.pressed[buttons[event]]!=0;
        return true;
    }
};
} // namespace rr::platform

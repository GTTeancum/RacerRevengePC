#pragma once
#include "window_host50dq.h"
#include "../../../src/assets/original_ui_movement_control50kg.h"
#include <array>

namespace rr::platform {
// Native physical-device translation into existing2A7410/2AB010 input IDs.
// Left-stick thresholds/edges below are explicit native UI policy, not source
// pad/axis-list dispatch. No DevicePacket48Y, racer/control owner, presence
// capability or repeat timer is created; actual pages retain all admission.
class OriginalUiControllerNavigation50LA final {
    std::int32_t instance_=-1;
    bool armed_=false;
    std::array<bool,2> horizontalHeld_{}; // Native Course levels from this same poll.
    bool keyboardHorizontalArmed_=false;
    static constexpr unsigned noStickDirection_=4;
    static constexpr unsigned menuKeys_[]={80,79,82,81,26,22,40,88,44,41,21,43};
    bool stickArmed_=false;
    unsigned stickDirection_=noStickDirection_;
    void disarmStick()noexcept {stickArmed_=false;stickDirection_=noStickDirection_;}
    void disarm()noexcept {
        instance_=-1;armed_=keyboardHorizontalArmed_=false;
        horizontalHeld_={};disarmStick();
    }
    void observeNativeStick(const WindowInput50DQ& input,std::uint8_t slot,
        bool directPadIntent,std::array<bool,8>& events)noexcept {
        bool directIntent=directPadIntent;
        // Keyboard belongs only to port zero. A direct menu action wins over
        // a new stick direction, including Confirm/Back in this same poll.
        if(slot==0)for(auto key:menuKeys_)
            directIntent=directIntent||input.down[key]||input.pressed[key];
        if(directIntent){disarmStick();return;}
        const auto& axes=input.controllers[slot].axes;
        const std::int32_t x=axes[0],y=axes[1]; // Actual SDL LEFTX/LEFTY only.
        const std::int32_t ax=x<0?-x:x,ay=y<0?-y:y; // -32768 is safe after promotion.
        // Native hysteresis: center within1/4 range, engage at1/2 range.
        // Both axes must center before a new excursion may produce an edge.
        if(ax<=8192&&ay<=8192){stickArmed_=true;stickDirection_=noStickDirection_;return;}
        const auto direction=[&](std::int32_t threshold)noexcept {
            if(ax>ay&&ax>=threshold)return x<0?0u:1u;
            if(ay>ax&&ay>=threshold)return y<0?2u:3u;
            return noStickDirection_;
        };
        if(stickArmed_){
            const auto selected=direction(16384);
            // An exact diagonal waits for strict dominance; it neither picks
            // an arbitrary axis nor consumes a still-unselected excursion.
            if(selected==noStickDirection_)return;
            stickDirection_=selected;stickArmed_=false;events[selected]=true;
        }else if(stickDirection_==noStickDirection_)return;
        else if(direction(8193)!=stickDirection_){
            // Rotation, sign change, release threshold or a diagonal ends the
            // admitted hold. Never retarget/repeat before both axes center.
            stickDirection_=noStickDirection_;return;
        }
        if(stickDirection_<2)horizontalHeld_[stickDirection_]=true;
    }
public:
    using Events=std::array<bool,8>;
    std::array<bool,2> courseHorizontalHeld()const noexcept{return horizontalHeld_;}
    bool observe(const WindowInput50DQ& input,bool active,Events& events,
        std::uint8_t slot=0)noexcept {
        events={};horizontalHeld_={};
        const auto invalid=[&]()noexcept {events={};disarm();return false;};
        if(slot>=input.controllers.size())return invalid();
        const bool activePoll=active&&input.focused&&!input.minimized&&!input.activityLost&&
            !input.eventBudgetExhausted&&!input.quitRequested;
        // Inactive ownership needs no keyboard/controller validation. This
        // also lets a race-owned poll revoke frontend latches without reading
        // or rejecting another owner's input, even before Results handoff.
        if(!activePoll){disarm();return true;}
        // Separate native keyboard hold admission. SDL repeat keydown can
        // restore down[] without a fresh pressed[] after focus loss. Require
        // both arrows released after any inactive poll; never turn OS repeat
        // into a new source edge or revive an old Course hold on resume.
        if(slot==0){
            for(const auto key:menuKeys_)
                if(input.down[key]>1||input.pressed[key]>1||input.released[key]>1)return invalid();
            if(!keyboardHorizontalArmed_)keyboardHorizontalArmed_=
                !input.down[80]&&!input.down[79]&&!input.pressed[80]&&!input.pressed[79];
            if(keyboardHorizontalArmed_)horizontalHeld_={input.down[80]!=0,input.down[79]!=0};
        }
        const auto& controller=input.controllers[slot];
        if(!input.gameControllersEnabled||!controller.connected){
            instance_=-1;armed_=false;disarmStick();return true;
        }
        if(controller.instanceId<0)return invalid();
        const bool changed=instance_!=controller.instanceId;
        if(changed){instance_=controller.instanceId;armed_=false;disarmStick();}
        // SDL public mapped button IDs: DPAD left/right/up/down, A/X/Y/B.
        // Source right-pad UP is Back6; right-pad RIGHT is event7, not Back.
        constexpr unsigned buttons[]={13,14,11,12,0,2,3,1};
        static_assert(static_cast<unsigned>(assets::OriginalUiMovementInput50KG::rpadRight)==7,
            "original menu event order must remain authenticated");
        bool neutral=true;
        for(const auto button:buttons){
            if(controller.buttons[button]>1||controller.pressed[button]>1||controller.released[button]>1)return invalid();
            neutral=neutral&&!controller.buttons[button]&&!controller.pressed[button];
        }
        if(changed||!armed_)armed_=neutral;
        else{
            for(unsigned event=0;event<events.size();++event)
                events[event]=controller.pressed[buttons[event]]!=0;
            horizontalHeld_[0]=horizontalHeld_[0]||controller.buttons[buttons[0]]!=0;
            horizontalHeld_[1]=horizontalHeld_[1]||controller.buttons[buttons[1]]!=0;
        }
        // Independent arming: an off-center stick never blocks Dpad or face
        // buttons. Direct intent suppresses the stick even before pad arming.
        observeNativeStick(input,slot,!neutral,events);
        // These are observed levels, not repeated events. Course's actual
        // page update owns its own timers; no other menu gains host repeat.
        return true;
    }
};
} // namespace rr::platform

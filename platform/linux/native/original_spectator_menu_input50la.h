#pragma once
#include "window_host50dq.h"
#include "../../../src/game/event_dispatch48z.h"

namespace rr::platform {
// Physical host UI edges only. Code8 is the native ALL8 spectator-menu
// command; it does not claim original127870 or human-racer input admission.
class OriginalSpectatorMenuInput50LA final {
    bool keyboardArmed_=false,padArmed_=false;
    std::int32_t padInstance_=-1;
public:
    struct Batch {
        std::array<game::InputEvent48Z,6> events{};
        std::array<game::DeviceButtonRecord48Y,6> records{};
        std::size_t count=0;
    };
    bool observe(const WindowInput50DQ& input,bool active,Batch& out)noexcept {
        out={};
        constexpr unsigned keys[]={41,82,81,40,88,80,79}; //Escape, Up, Down, Return, keypad Enter, Left, Right.
        constexpr unsigned buttons[]={6,11,12,0,13,14}; //SDL mapped Start, Up, Down, A, Left, Right.
        for(auto key:keys)if(input.down[key]>1||input.pressed[key]>1||input.released[key]>1)return false;
        const auto& pad=input.controllers[0];
        const bool connected=input.gameControllersEnabled&&pad.connected;
        if(connected){
            if(pad.instanceId<0)return false;
            for(auto button:buttons)if(pad.buttons[button]>1||pad.pressed[button]>1||pad.released[button]>1)return false;
        }
        if(!connected){padInstance_=-1;padArmed_=false;}
        else if(padInstance_!=pad.instanceId){padInstance_=pad.instanceId;padArmed_=false;}
        if(!active||!input.focused||input.minimized||input.activityLost||input.eventBudgetExhausted||input.quitRequested){
            keyboardArmed_=padArmed_=false;return true;
        }
        std::array<bool,6> edges{};
        bool keyboardNeutral=true;
        for(auto key:keys)keyboardNeutral=keyboardNeutral&&!input.down[key]&&!input.pressed[key];
        if(keyboardArmed_){
            edges={{input.pressed[41]!=0,input.pressed[82]!=0,input.pressed[81]!=0,
                input.pressed[40]!=0||input.pressed[88]!=0,input.pressed[80]!=0,input.pressed[79]!=0}};
        }else keyboardArmed_=keyboardNeutral;
        if(connected){
            bool neutral=true;
            for(auto button:buttons)neutral=neutral&&!pad.buttons[button]&&!pad.pressed[button];
            if(padArmed_){
                for(std::size_t i=0;i<edges.size();++i)edges[i]=edges[i]||pad.pressed[buttons[i]];
            }else padArmed_=neutral;
        }
        constexpr std::uint32_t codes[]={8,12,14,2,15,13};
        // Keyboard and first physical controller share native UI device0.
        // Simultaneous matching edges dispatch once; no repeat or held drive packet.
        // Existing opening/up/down/Confirm order remains; source1BD7C0 tests
        // Left15 before Right13. These are native physical aliases, not a pad packet.
        for(std::size_t i=0;i<edges.size();++i)if(edges[i]){
            out.events[out.count]={codes[i],2,0,1,0};
            out.records[out.count]={1,1.f,0,0,0,0};++out.count;
        }
        return true;
    }
};
}

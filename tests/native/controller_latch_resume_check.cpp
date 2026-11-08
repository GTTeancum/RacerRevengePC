// Selected controller latch at race start, menu resume, focus loss and
// reconnect. Original reader 1A1A20 reads held pad state every frame; its
// only race-start condition is the manager gate 1B0B80 (flags58 bit1 set,
// bit0 clear). So held throttle must be live on the first racing frame.
#include "../../platform/linux/native/selected_control_input50ky.h"
#include "../../src/game/control_presets49d.h"
#include <cstdio>
#include <stdexcept>
#include <string>

namespace {
unsigned checks=0;
void need(bool value,const char* what){++checks;if(!value)throw std::runtime_error(std::string("check failed: ")+what);}
using rr::platform::SelectedControllerStatus50KY;
constexpr unsigned sdlA=0;          // SDL A -> original descriptor2 (Cross).
constexpr unsigned crossPressure=14; // Descriptor2 packet pressure byte.
struct Rig {
    rr::game::ControlPresets49D preferences{};
    rr::platform::SelectedControlInput50KY binding;
    rr::platform::WindowGameController50DQ pad{};
    rr::platform::SelectedControllerLatch50KY latch;
    rr::platform::SelectedControllerReceipt50KY receipt;
    rr::game::DevicePacket48Y packet{};
    Rig(){
        rr::game::initializeControlPresets49D(preferences);
        need(rr::platform::bindSelectedControlInput50KY(preferences,0,0,binding),"preset0 bind");
        pad.connected=true;pad.instanceId=7;
    }
    SelectedControllerStatus50KY frame(bool enabled,bool interrupted,bool countdown){
        need(rr::platform::composeSelectedControllerPacket50KY(binding,pad,enabled,latch,packet,receipt,
            interrupted,countdown),"compose");
        return receipt.status;
    }
};
}

int main(){
    try{
        { // 1. Throttle held through the countdown into the first racing frame.
            Rig r;r.pad.buttons[sdlA]=1;
            for(int i=0;i<60;++i)need(r.frame(false,false,true)==SelectedControllerStatus50KY::releasedInactive,"countdown releases packet");
            need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"green: held throttle is live (no wait for neutral)");
            need(r.packet.bytes[crossPressure]==255,"green: Cross pressure reaches the original packet");
        }
        { // 2. Throttle held while a host menu closes: no freeze, press ignored until released.
            Rig r;need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"racing");
            r.pad.buttons[sdlA]=1;
            need(r.frame(false,false,false)==SelectedControllerStatus50KY::releasedInactive,"menu open");
            need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"menu closed: world runs (no awaitingNeutral)");
            need(r.packet.bytes[crossPressure]==0,"button held from the menu is ignored");
            r.pad.buttons[sdlA]=0;need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"released");
            r.pad.buttons[sdlA]=1;need(r.frame(true,false,false)==SelectedControllerStatus50KY::live&&
                r.packet.bytes[crossPressure]==255,"fresh press after the menu works");
        }
        { // 3. Focus loss still requires neutral (unchanged policy).
            Rig r;need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"racing");
            r.pad.buttons[sdlA]=1;
            need(r.frame(false,true,false)==SelectedControllerStatus50KY::releasedInactive,"focus lost");
            need(r.frame(true,false,false)==SelectedControllerStatus50KY::awaitingNeutral,"focus regained with held button waits for neutral");
            r.pad.buttons[sdlA]=0;need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"neutral re-arms");
        }
        { // 4. A different controller instance still requires neutral.
            Rig r;need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"first device armed at once");
            r.pad.connected=false;r.frame(true,false,false);
            r.pad.connected=true;r.pad.instanceId=8;r.pad.buttons[sdlA]=1;
            need(r.frame(true,false,false)==SelectedControllerStatus50KY::awaitingNeutral,"reconnected device waits for neutral");
        }
        { // 5. Triangle (SDL Y) is the camera-view action (preset row 6A4) once
          // the race camera has bound it: the press reaches the packet, live.
            constexpr unsigned sdlY=3;
            const auto descriptors=rr::game::initialDualShockDescriptors48Y();
            const auto& triangle=descriptors.records[0];
            const auto down=[&](const rr::game::DevicePacket48Y& p){
                if(triangle.flags&2u)return p.bytes[triangle.packetOffset]==255;
                const std::uint16_t inactive=static_cast<std::uint16_t>((p.bytes[2]<<8)|p.bytes[3]);
                return (inactive&triangle.digitalMask)==0;
            };
            Rig r;need(r.preferences.slots[0].actions.action6A4==0,"preset0 camera action is Triangle");
            need(rr::platform::bindSelectedCameraCycle50KY(r.preferences,0,r.binding),"camera cycle binds");
            need(r.binding.cameraCycleCode==0,"camera cycle code is descriptor0 (Triangle)");
            need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"racing");
            need(!down(r.packet),"Triangle up");
            r.pad.buttons[sdlY]=1;
            need(r.frame(true,false,false)==SelectedControllerStatus50KY::live,"Triangle press stays live");
            need(down(r.packet),"Triangle press reaches the original packet");
            r.pad.buttons[sdlY]=0;
            need(r.frame(true,false,false)==SelectedControllerStatus50KY::live&&!down(r.packet),"Triangle release");
        }
        std::printf("controller latch checks=%u passed\n",checks);
        return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}
}

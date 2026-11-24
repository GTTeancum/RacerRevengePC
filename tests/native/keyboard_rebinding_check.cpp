#include "../../platform/linux/native/selected_control_input50ky.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace p=rr::platform;
namespace g=rr::game;
namespace kb=p::keyboard_bindings;

static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}

static g::DevicePacket48Y packet(const p::SelectedControlInput50KY& binding,
    const p::WindowInput50DQ& input,bool focused=true){
    g::DevicePacket48Y initial{},out{};initial.bytes[2]=initial.bytes[3]=255;
    std::array<std::uint8_t,512> translated;
    p::KeyboardPacketDiagnostic50DR diagnostic;
    need(p::composeKeyboardPacket50DR({true,focused},
        {kb::drivingKeys(binding.twinEngine,input.down,translated),input.down.size()},binding.keyboard,
        p::KeyboardUnavailablePolicy50DR::ReleaseAll,initial,out,diagnostic),"packet composition");
    return out;
}
static bool equal(const g::DevicePacket48Y& a,const g::DevicePacket48Y& b){
    return std::memcmp(a.bytes,b.bytes,sizeof(a.bytes))==0;
}
int main(){
    unsigned actions=0;
    for(unsigned preset=0;preset<8;++preset){
        g::ControlPresets49D preferences{};g::initializeControlPresets49D(preferences);
        need(g::selectControlPreset49D(preferences,0,preset),"preset");
        p::SelectedControlInput50KY binding;
        need(p::bindSelectedControlInput50KY(preferences,0,0,binding),"driver");
        need(p::bindSelectedCameraCycle50KY(preferences,0,binding),"camera");
        need(p::bindSelectedManualReset50LC(preferences,0,binding),"reset");
        auto family=binding.twinEngine?kb::Twin:kb::Standard;
        auto& layout=kb::layouts[family];
        for(const auto& action:kb::actions){
            if(action.family!=kb::Both&&action.family!=family)continue;
            if((action.original==4||action.original==7)&&preset!=0)continue;
            p::WindowInput50DQ old{},assigned{},clear{};
            old.down[action.original]=1;assigned.down[12]=1;
            layout=kb::Layout{};
            const auto expected=packet(binding,old),neutral=packet(binding,clear);
            need(!equal(expected,neutral),"stock action must produce input");
            layout.keys[action.original]=12;layout.custom=true;
            need(equal(expected,packet(binding,assigned)),"new key must produce original action");
            need(equal(neutral,packet(binding,old)),"old key must be released");
            need(equal(neutral,packet(binding,assigned,false)),"focus loss must release");
            need(p::selectedKeyboardHeld50KY(assigned,binding),"resume latch must see new key");
            p::releaseSelectedKeyboard50KY(assigned,binding);
            need(!assigned.down[12],"resume must release new physical key");
            layout.keys[action.original]=0;assigned.down[12]=1;
            need(equal(neutral,packet(binding,assigned))&&equal(neutral,packet(binding,old)),"cleared action");
            ++actions;
        }
    }
    std::printf("Verified %u action/preset combinations across all eight presets.\n",actions);
}

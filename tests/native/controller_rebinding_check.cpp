#include "../../platform/linux/native/selected_control_input50ky.h"
#include <cstdio>
#include <stdexcept>
namespace p=rr::platform;
namespace g=rr::game;
namespace pads=p::controller_bindings;
static void need(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(){
    unsigned cases=0;
    for(unsigned player=0;player<2;++player)for(unsigned preset=0;preset<8;++preset){
        g::ControlPresets49D prefs{};g::initializeControlPresets49D(prefs);
        need(g::selectControlPreset49D(prefs,player,preset),"select");
        p::SelectedControlInput50KY binding;
        need(p::bindSelectedControlInput50KY(prefs,player,player,binding),"bind");
        need(p::bindSelectedCameraCycle50KY(prefs,player,binding)&&p::bindSelectedManualReset50LC(prefs,player,binding),"optional owners");
        need(binding.player==player&&binding.preset==preset,"owner retained");
        auto& layout=pads::layouts[player][preset];
        const int target=binding.sourceActions.action6A4;
        std::swap(layout.buttons[target],layout.buttons[4]);
        std::swap(layout.axes[1],layout.axes[3]);
        p::SelectedControllerLatch50KY latch{};p::SelectedControllerReceipt50KY receipt{};
        p::WindowGameController50DQ pad{};pad.connected=true;pad.instanceId=2;
        g::DevicePacket48Y packet{};const auto desc=g::initialDualShockDescriptors48Y().records[target];
        auto sample=[&](bool live,bool interrupted=true){
            need(p::composeSelectedControllerPacket50KY(binding,pad,live,latch,packet,receipt,interrupted),"compose");
        };
        pad.axes[4]=16384;pad.axes[3]=-16384;sample(true);
        need(packet.bytes[desc.packetOffset]==128&&packet.bytes[7]==64&&packet.bytes[5]==128,"analog remap");
        sample(false,false);sample(true);
        need(packet.bytes[desc.packetOffset]==0,"held across pause");
        pad.axes[4]=0;sample(true);pad.axes[4]=32767;sample(true);need(packet.bytes[desc.packetOffset]==255,"re-arm");
        sample(false);sample(true);need(receipt.status==p::SelectedControllerStatus50KY::awaitingNeutral,"focus");
        pad.axes={};sample(true);layout.buttons[target]=-1;layout.axes[1]=-1;pad.axes[4]=32767;pad.axes[3]=-32768;sample(true);
        need(packet.bytes[desc.packetOffset]==0&&packet.bytes[7]==128,"unbound");
        pad.connected=false;sample(true);need(receipt.status==p::SelectedControllerStatus50KY::releasedDisconnected,"disconnect");
        layout={};++cases;
    }
    auto axes=pads::identity<4>();const auto original=axes;
    for(auto invalid:{L"0,1,1,3",L"0,1,2,4",L"0,1,",L"0,1,2,3,"})
        need(!pads::parse(invalid,axes)&&axes==original,"invalid configuration must preserve default");
    need(pads::parse(L"3,1,-1,0",axes)&&axes[2]==-1,"valid configuration");
    auto buttons=pads::identity<16>();need(!pads::parse(L"9,1,2,3,4,5,6,7,8,0,10,11,12,13,14,15",buttons),"Start reserved");
    std::printf("Passed %u player/preset combinations and profile validation.\n",cases);
}

// Authored regression cases. NOT built or run as part of this source patch.
// Run only inside the next user-authorized combined validation window.
#include "../../src/game/original_camera_mode_input.h"
#include "../../platform/linux/native/selected_control_input50ky.h"
#include <stdexcept>

namespace {
void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
using namespace rr::game;
void transport(){
    ControlPresets49D preferences{};initializeControlPresets49D(preferences);
    rr::platform::SelectedControlInput50KY binding;
    require(rr::platform::bindSelectedControlInput50KY(preferences,0,0,binding),"preset bind");
    rr::platform::WindowGameController50DQ pad{};pad.connected=true;pad.instanceId=7;
    rr::platform::SelectedControllerLatch50KY latch;
    rr::platform::SelectedControllerReceipt50KY receipt;DevicePacket48Y packet{};
    const auto compose=[&](){return rr::platform::composeSelectedControllerPacket50KY(binding,pad,true,latch,packet,receipt);};
    require(compose()&&latch.armed,"neutral arm");
    pad.buttons[9]=1; //Actual SDL L1 -> original descriptor6, no owner yet.
    require(compose()&&receipt.status==rr::platform::SelectedControllerStatus50KY::unsupportedOriginalAction,
        "mapped L1 missing its owner must be reported without throwing");
    require(receipt.unsupportedCode==6&&!latch.armed,"missing-owner descriptor/re-arm");
    require(packet.bytes[16]==0&&packet.bytes[14]==0&&packet.bytes[2]==255&&packet.bytes[3]==255,
        "unsupported packet must release all held actions");
    require(compose()&&receipt.status==rr::platform::SelectedControllerStatus50KY::awaitingNeutral,
        "held unsupported button must not produce repeated rejection/logs");
    pad.buttons[9]=0;require(compose()&&latch.armed,"neutral re-arm");
    require(rr::platform::bindSelectedCameraCycle50KY(preferences,0,binding),"camera input bind");
    pad.buttons[9]=1;
    require(compose()&&receipt.status==rr::platform::SelectedControllerStatus50KY::live&&packet.bytes[16]==255,
        "bound L1 must retain descriptor6 pressure");
    pad.buttons[11]=1; //Unused original D-pad-up descriptor12.
    pad.buttons[0]=1; //Mapped Cross throttle.
    require(compose()&&receipt.status==rr::platform::SelectedControllerStatus50KY::live&&latch.armed&&
        packet.bytes[16]==255&&packet.bytes[14]==255&&packet.bytes[10]==255,
        "unmapped D-pad up must preserve throttle, look and its own source descriptor");
    pad.buttons[4]=1; //Mapped SELECT/manual reset with no reset owner bound.
    require(compose()&&receipt.status==rr::platform::SelectedControllerStatus50KY::unsupportedOriginalAction&&
        receipt.unsupportedCode==8&&packet.bytes[16]==0&&packet.bytes[14]==0&&!latch.armed,
        "mapped reset missing owner must still release whole packet");
}
void allPresetUnmappedButtons(){
    //Every canonical preset is covered, without changing physical mappings.
    constexpr std::int8_t buttons[16]={3,1,0,2,-1,-1,9,10,4,6,8,7,11,14,12,13};
    for(std::uint32_t preset=0;preset<8;++preset){
        ControlPresets49D preferences{};initializeControlPresets49D(preferences);
        require(selectControlPreset49D(preferences,0,preset),"select preset");
        rr::platform::SelectedControlInput50KY binding;
        require(rr::platform::bindSelectedControlInput50KY(preferences,0,0,binding),"bind preset");
        require(rr::platform::bindSelectedCameraCycle50KY(preferences,0,binding),"bind camera");
        require(rr::platform::bindSelectedManualReset50LC(preferences,0,binding),"bind reset");
        const auto& row=preferences.slots[0].actions;
        const std::int32_t mapped[]={row.action690,row.action694,row.flag698,row.flag69C,row.action6A0,
            row.action6A4,row.action6A8,row.action6AC,row.action6B0,row.flag6B4,13,15};
        for(unsigned code=0;code<16;++code){
            bool assigned=false;for(auto action:mapped)if(action==static_cast<std::int32_t>(code))assigned=true;
            if(assigned)continue;
            require(buttons[code]>=0,"unmapped source descriptor has physical button");
            rr::platform::WindowGameController50DQ pad{};pad.connected=true;pad.instanceId=7;
            rr::platform::SelectedControllerLatch50KY latch;
            rr::platform::SelectedControllerReceipt50KY receipt;DevicePacket48Y packet{};
            require(rr::platform::composeSelectedControllerPacket50KY(binding,pad,true,latch,packet,receipt),"arm");
            pad.buttons[static_cast<unsigned>(buttons[code])]=1;
            pad.axes[1]=pad.axes[3]=-32768; //Retain both actual stick inputs.
            require(rr::platform::composeSelectedControllerPacket50KY(binding,pad,true,latch,packet,receipt)&&
                receipt.status==rr::platform::SelectedControllerStatus50KY::live&&latch.armed&&
                packet.bytes[5]==0&&packet.bytes[7]==0,"unmapped button cannot interrupt stick controls");
            const auto descriptor=initialDualShockDescriptors48Y().records[code];
            const auto inactive=static_cast<std::uint16_t>((packet.bytes[2]<<8)|packet.bytes[3]);
            require((descriptor.flags&2u)?packet.bytes[descriptor.packetOffset]==255:
                !(inactive&descriptor.digitalMask),"unmapped physical descriptor remains in source packet");
        }
    }
}
void followLook(){
    ControlPresets49D preferences{};initializeControlPresets49D(preferences);
    OriginalFollowPreset presets[4]{};
    for(int i=0;i<4;++i){presets[i].id00=i;presets[i].fov10=80;presets[i].distance0C=10.f+i;}
    OriginalFollowCamera follow;
    require(prepareOriginalFollowCamera(follow,{1,2,3,4,0},{5,0,0,presets,4}).status==OriginalFollowStatus::ok,"follow prepare");
    require(activateOriginalFollowCamera(follow).status==OriginalFollowStatus::ok,"follow activation");
    OriginalRaceCameraOwner camera;camera.bound=true;camera.mode44=0;
    camera.binding.sessionGeneration=1;camera.binding.racerKey=2;camera.binding.mainBodyKey=3;
    OriginalAttachedCamera attached;OriginalFollowSceneBinding scene;
    OriginalFollowChildRegistry followChildren;OriginalAttachedChildRegistry attachedChildren;
    ResetVisibilityWorld50EY world;OriginalCameraSceneOwner spatial;OriginalFollowVisibilityZones zones;
    LiveZoneWorkspace50EN workspace;OriginalFollowSceneTransitionInput transition;
    OriginalCameraModeInputOwners owners{camera,follow,attached,scene,followChildren,attachedChildren,world,spatial,zones,workspace};
    DeviceButtonBank48Y bank{};ManagerHeldSources49A held{};held.devices[0]=&bank;
    const std::uint32_t view=0;
    OriginalCameraModeInputEnvironment environment{held,preferences,&view,1,0,0,0,2,1};
    follow.distanceOffset58=2;follow.yawOffset5C=3;follow.pitchOffset60=4;follow.fovOffset64=5;
    bank.records[6].held=1;
    auto result=advanceOriginalCameraCyclePress176DF0(owners,environment,transition);
    require(result.status==OriginalCameraModeInputStatus::needsChildRoute142350&&camera.flags62==0x20&&
        follow.current100==1,"L1 must select original preset1 without cycle audio/preference write");
    require(preferences.slots[0].field11030==5,"temporary look must not persist camera mode");
    result=advanceOriginalCameraLookRelease1774A0(owners,environment,transition);
    require(result.status==OriginalCameraModeInputStatus::needsChildRoute142350&&camera.flags62==0x20,
        "unrelated release cannot clear held L1");
    bank.records[6].held=0;
    result=advanceOriginalCameraLookRelease1774A0(owners,environment,transition);
    require(result.status==OriginalCameraModeInputStatus::needsChildRoute142350&&camera.flags62==0&&follow.current100==0,
        "L1 release restores saved record");
    require(follow.distanceOffset58==2&&follow.yawOffset5C==3&&follow.pitchOffset60==4&&follow.fovOffset64==5,
        "L1 release restores saved offsets");
    bank.records[1].held=bank.records[6].held=1;
    result=advanceOriginalCameraCyclePress176DF0(owners,environment,transition);
    require(result.status==OriginalCameraModeInputStatus::needsChildRoute142350&&camera.flags62==0x40&&
        follow.current100==3,"simultaneous look must preserve6A8 priority");
    bank.records[6].held=0;
    result=advanceOriginalCameraLookRelease1774A0(owners,environment,transition);
    require(camera.flags62==0x40&&follow.current100==3,"non-owning look release must not restore");
    bank.records[1].held=0;
    result=advanceOriginalCameraLookRelease1774A0(owners,environment,transition);
    require(camera.flags62==0&&follow.current100==0,"priority look release restores");
    bank.records[6].held=1;environment.raceStateFlags58=1;
    result=advanceOriginalCameraCyclePress176DF0(owners,environment,transition);
    require(result.status==OriginalCameraModeInputStatus::needsChildRoute142350&&camera.flags62==0,
        "race gate must suppress temporary look");
}
}
int main(){transport();allPresetUnmappedButtons();followLook();}

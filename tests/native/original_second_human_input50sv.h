#pragma once
// Split screen (VS. MODE): human 1's driving input through the SAME original
// path as human 0 - its own device ingress, action lease, player event state
// and held-state reader. Host-side
// storage only; nothing here touches guest memory.
//
// Original (SLUS_202.68):
// - 1A767C binds pad i to human i: racer+5C = i, racer+FF0 = game+4 -> +10[i]
//   (device i), button map profile + i*0x28 + 0x6B4 (player i's control
//   preset) [R]. So human 1 = device 1 = pad 2 only; the keyboard stays
//   device 0 (the original has no keyboard split) [R].
// - 1A1A20 reads that racer's held pad state every frame, with no state gate
//   (the countdown hold is the reader's own input, as for human 0) [R].
// - Manual reset 2255B0 is reached through the player's event effects; for
//   human 1 it needs the per-racer reset owner (pod-load milestone e).
//   Until then a reached reset effect is reported and refused, exactly as
//   human 0's unsupported effects are.
#include "../../src/game/controller_ingress50cx.h"
#include "../../src/game/controller_actions50ec.h"
#include "../../src/game/player_events49a.h"
#include "../../src/game/event_dispatch48z.h"
#include "../../src/game/device_held48y.h"
#include "../../platform/linux/native/selected_control_input50ky.h"
#include <cstdint>
#include <cstdio>
#include <utility>

struct SecondHumanInput50SV {
    // [I] action/preset index 1 = profile + 1*0x28 + 0x6B4 (player 2's row).
    static constexpr std::uint32_t deviceIndex=1,source5C=1,actionIndexFFC=1;
    rr::platform::SelectedControlInput50KY binding{};
    rr::game::SelectedControlPreset49D selected{};
    rr::game::DeviceButtonBank48Y initialButtons{};
    rr::game::ControllerIngress50CX controller{initialButtons};
    rr::game::InputEvent48Z eventStorage[32]{};
    rr::game::InputEventQueue48Z events{eventStorage,32,0};
    rr::game::ControllerActions50EC actions{controller,events,deviceIndex};
    rr::game::PlayerEventState49A player{};
    rr::game::PlayerEventEnvironment49A environment{};
    rr::game::PlayerEventBindings49A bindings{};
    rr::game::EventChild48Z applicationChildren[2]{{rr::game::noEventChild48Z,1},{rr::game::noEventChild48Z,1}};
    rr::game::JoystickApplicationBridge48Z application{};
    rr::game::PlayerEventChildBinding49A child{};
    rr::game::ManagerEventCallbacks48Z callbacks{};
    rr::game::EventContext48Z context{false,0,{nullptr,0}};
    rr::game::EventRecordSources48Z records{};
    rr::game::PlayerEffectCall49A unavailableEffect{};
    rr::game::DevicePacket48Y packet{};
    rr::platform::SelectedControllerLatch50KY latch{};
    rr::platform::SelectedControllerReceipt50KY receipt{};
    // Owned for the whole race: the environment keeps a pointer into the rows.
    decltype(rr::game::playerEventPresetRows49D(std::declval<const rr::game::ControlPresets49D&>())) rows{};
    rr::game::DeviceButtonDescriptors48Y descriptors=rr::game::initialDualShockDescriptors48Y();
    // Human 1's racer fields at the last frame boundary (written by its racer
    // frame), passed to the next sample as human 0's own fields are.
    std::uint32_t state40=0;std::uint8_t gate58=0;
    bool bound=false,packetLive=false;
    std::uint64_t samples=0,presses=0,releases=0;

    SecondHumanInput50SV()=default;
    SecondHumanInput50SV(const SecondHumanInput50SV&)=delete;
    SecondHumanInput50SV& operator=(const SecondHumanInput50SV&)=delete;

    // Same order and topology as human 0's stack in terrain_interactive_setup50du.cpp.
    bool bind(const rr::game::ControlPresets49D& presets) noexcept {
        if(bound)return false;
        if(!rr::platform::bindSelectedControlInput50KY(presets,actionIndexFFC,source5C,binding))return false;
        if(!rr::game::resolveControlPreset49D(presets,actionIndexFFC,source5C,selected))return false;
        player.source5C=source5C;player.indexFFC=actionIndexFFC;
        rows=rr::game::playerEventPresetRows49D(presets);
        environment.actionRows=rows.rows;environment.actionRowCount=rr::game::controlPresetSlots49D;
        environment.flags29=1;
        bindings.effects={this,[](void* opaque,const rr::game::PlayerEffectCall49A& call) noexcept {
            auto& owner=*static_cast<SecondHumanInput50SV*>(opaque);owner.unavailableEffect=call;
            std::fprintf(stderr,"second_human_effect_refused kind=%u\n",static_cast<unsigned>(call.kind));
            return false;}};
        bindings.children={0x600u,rr::game::noEventChild48Z,nullptr,0};
        bindings.childVisitBudget=0;
        application.rootFlags228=0;application.root={0x600u,0,applicationChildren,1};application.visitBudget=1;
        child.childIndex=0;child.player=&player;child.environment=&environment;child.bindings=&bindings;
        if(!rr::game::bindControllerPlayerActions50EC(controller,deviceIndex,records,application,child,callbacks))return false;
        bound=true;return true;
    }
    // 1A1A20's held reader context for this human (same fields as human 0's).
    rr::game::DeviceQueryContext48Y queryContext() const noexcept {
        return {context.present,application.rootFlags228,environment.held.context518,context.mask1898};
    }
    // One poll: compose pad 2's packet with the human-0 policy, then the same
    // sample/append/dispatch/acknowledge cycle. `vehicleState40` and `gate58`
    // are human 1's racer fields (its own frame), as human 0 passes its own.
    bool service(const rr::platform::WindowGameController50DQ& pad,bool live,bool releaseAll,bool countdownHold,
        std::uint32_t vehicleState40,std::uint8_t gate58) noexcept {
        if(!bound)return false;
        if(!rr::platform::composeSelectedControllerPacket50KY(binding,pad,live,latch,packet,receipt,releaseAll,countdownHold))
            return false;
        packetLive=receipt.status==rr::platform::SelectedControllerStatus50KY::live;
        player.field40=vehicleState40;environment.gate58=gate58;
        environment.held.rootFlags228=application.rootFlags228;environment.held.context1898=context.mask1898;
        if(actions.samplePacket(true,true,packet,descriptors,rr::game::dualShockActiveButtons48Y)!=
           rr::game::ControllerActionStatus50EC::Ok)return false;
        if(controller.deviceSample().status!=rr::game::DevicePacketStatus48Y::Accepted)return false;
        if(actions.appendSample()!=rr::game::ControllerActionStatus50EC::Ok)return false;
        const auto result=actions.dispatch(context,records,callbacks);
        if(result.status!=rr::game::ControllerActionStatus50EC::Ok||!result.dispatchAttempted||
           result.dispatch.status!=rr::game::EventDispatchStatus48Z::Complete)return false;
        const auto& sample=controller.deviceSample();
        if(result.dispatch.eventsCompleted!=sample.transitionCount||
           result.dispatch.callbacksInvoked!=sample.transitionCount)return false;
        for(std::uint32_t i=0;i<sample.transitionCount;++i)++(sample.transitions[i].held?presses:releases);
        ++samples;
        return actions.acknowledgeComplete()==rr::game::ControllerActionStatus50EC::Ok;
    }
};

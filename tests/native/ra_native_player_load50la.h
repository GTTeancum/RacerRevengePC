#pragma once
#include "../../src/game/controller_actions50ec.h"
#include "../../src/game/pod_speech50hg.h"
#include <array>
namespace live_audio50HF {class Owner;}
namespace native_racer50HN {class Bank;}
namespace interactive_cold50DU {struct OwnersContext;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {struct AuthoredBinding49N;}
namespace ra_cold_roster50LA {
class Operation;
// Private native caller descriptor, issued by the actual cold human route.
// These are permanent enclosing owners, never the temporary OwnersContext.
// Copying Arena preserves this identity; it cannot issue source Player input
// axes, construction, derived dispatch or a guest1A0570 call receipt.
class NativePlayerLoadCaller50LA final {
    friend struct ::interactive_cold50DU::OwnersContext;
    friend class Operation;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    ::native_racer50HN::Bank* bank_=nullptr;
    rr::game::ControllerIngress50CX* ingress_=nullptr;
    rr::game::ControllerActions50EC* actions_=nullptr;
    rr::game::InputEventQueue48Z* queue_=nullptr;
    rr::game::JoystickApplicationBridge48Z* application_=nullptr;
    rr::game::PlayerEventChildBinding49A* child_=nullptr;
    rr::game::PlayerEventState49A* player_=nullptr;
    rr::game::PlayerEventEnvironment49A* environment_=nullptr;
    rr::game::AuthoredBinding49N* authored_=nullptr;
    float* steering10A4_=nullptr; // Actual unbanked finish consumer, not new storage.
    const std::array<std::uint32_t,8>* racer60_=nullptr;
    ::live_audio50HF::Owner* audio_=nullptr;
    rr::game::PodSpeechCounts50HG* speechCounts_=nullptr; // Actual unbanked root owner.
    rr::game::PodSpeechState50HG* speechState_=nullptr; // Same Bank-switched native consumer.
    NativePlayerLoadCaller50LA(::native_racer50HN::Bank& bank,
        rr::game::ControllerIngress50CX& ingress,rr::game::ControllerActions50EC& actions,
        rr::game::InputEventQueue48Z& queue,rr::game::JoystickApplicationBridge48Z& application,
        rr::game::PlayerEventChildBinding49A& child,rr::game::PlayerEventState49A& player,
        rr::game::PlayerEventEnvironment49A& environment,rr::game::AuthoredBinding49N& authored,
        float& steering,const std::array<std::uint32_t,8>& racer60,::live_audio50HF::Owner* audio,
        rr::game::PodSpeechCounts50HG* speechCounts,rr::game::PodSpeechState50HG& speechState)noexcept
        :bank_(&bank),ingress_(&ingress),actions_(&actions),queue_(&queue),application_(&application),
         child_(&child),player_(&player),environment_(&environment),authored_(&authored),steering10A4_(&steering),
         racer60_(&racer60),audio_(audio),speechCounts_(speechCounts),speechState_(&speechState){}
public:
    NativePlayerLoadCaller50LA()noexcept=default; // Empty for ALL8/source-fresh routes.
};
}

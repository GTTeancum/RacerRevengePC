#pragma once
#include "../../../src/game/original_wreck_components50la.h"
#include <cstdint>
namespace rr::platform {
// Constructor result only. Neither nonnull Entity nor this diagnostic grants
// a22A210 return,2C1170 initialization,22A25C attachment or Player dispatch.
// Native allocation refusal is NOT the retail heap-null/return0 branch.
enum class NativePlayerSpeechConstructorStatus50LA : std::uint8_t {
    ownerRejected,nativeAllocationPending,constructorComplete
};
struct NativePlayerSpeechConstructorResult50LA {
    NativePlayerSpeechConstructorStatus50LA status=
        NativePlayerSpeechConstructorStatus50LA::ownerRejected;
    std::uint32_t sourcePC=0x22a210u;
    rr::game::OriginalWreckComponent50LA* entity=nullptr;
};
} // namespace rr::platform

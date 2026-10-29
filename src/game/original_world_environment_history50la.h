#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <thread>
namespace rr::platform {class OriginalUiGarageSceneSetup50KG;}
namespace rr::game::original_world_environment50KG {
class Owner;
// One process history, not one guard per Environment instance. Native-only
// projections withdraw certainty and never restore it on cleanup.
class EnvironmentHistory50LA final {
    friend class Owner;
    friend class ::rr::platform::OriginalUiGarageSceneSetup50KG;
    struct Guard {std::atomic<bool> known{true};std::uint8_t initialized=0;std::uint32_t enabled=0;};
    struct State {const std::thread::id thread=std::this_thread::get_id();std::array<Guard,3> guard;};
    static State& state()noexcept {static State s;return s;}
    static void invalidateCubeNative()noexcept {state().guard[0].known.store(false,std::memory_order_release);}
    static void invalidateBoundsNative()noexcept {state().guard[1].known.store(false,std::memory_order_release);state().guard[2].known.store(false,std::memory_order_release);}
    // Source first-use initializer, then actual always-true narrow assertion.
    // False predicate is NOT converted into a guessed debug-policy result.
    static bool enter(unsigned index,bool predicate,std::uint32_t& pc)noexcept {
        static constexpr std::uint32_t first[]={0x276d8c,0x277094,0x27710c};
        static constexpr std::uint32_t call[]={0x276ddc,0x2770dc,0x277154};
        if(index>=3)return false;auto& s=state();pc=first[index];
        auto& g=s.guard[index];if(s.thread!=std::this_thread::get_id()||!g.known.load(std::memory_order_acquire))return false;
        if(!g.initialized){g.enabled=1;g.initialized=1;} //word store then byte store.
        if(g.enabled&&!predicate){pc=call[index];return false;}
        //133660 returns0 for nonzero a0 regardless of live36C100 debugpolicy.
        return true;
    }
};
} // namespace rr::game::original_world_environment50KG

#pragma once
#include <atomic>
#include <cstdint>
#include <thread>
namespace ra_cold_aggregate50LA {class Owner;}
namespace source_world_cscamera50LA {
// One process history for the actual 3D64E0 byte and 442FD8 word. Startup
// 100008..100030 explicitly clears both; GP is produced at100034..10005C.
// Native projections cannot certify that this source interval was executed.
class History final {
    friend class ::ra_cold_aggregate50LA::Owner;
    struct State {
        const std::thread::id thread=std::this_thread::get_id();
        std::atomic<bool> known{true};
        std::uint8_t initialized3D64E0=0;
        std::uint32_t enabled442FD8=0;
    };
    static State& state()noexcept {static State s;return s;}
    static void invalidateNative()noexcept {
        state().known.store(false,std::memory_order_release);
    }
    static bool enter(std::int32_t zones,std::int32_t selected,std::uint32_t& pc)noexcept {
        auto& s=state();pc=0x272460;
        if(s.thread!=std::this_thread::get_id()||!s.known.load(std::memory_order_acquire))return false;
        if(!s.initialized3D64E0){
            pc=0x272474;s.enabled442FD8=1;
            pc=0x272478;s.initialized3D64E0=1;
        }
        pc=0x272484;
        if(s.enabled442FD8){
            pc=0x2724b4;
            // The XORI delay slot passes !(zones < selected). Nonzero a0
            // returns0 from133660 irrespective of the live debug global.
            // A false predicate is NOT converted to a guessed debug result.
            if(zones<selected)return false;
        }
        return true;
    }
};
}

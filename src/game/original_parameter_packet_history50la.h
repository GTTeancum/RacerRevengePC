#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <thread>
#include <limits>
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
//One process history for original134xxx; not a per-World packet authority.
class OriginalParameterPacketHistory50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    struct State {
        std::array<std::uint8_t,8> guard{};
        std::array<std::uint32_t,8> enabled{};
        std::uint32_t placeholder28=0,target30=0,words38=0,format40=0;
        std::thread::id thread;bool busy=false;
        std::uint64_t serial=0,scratchSerial=0;bool scratchAccessible=false;
    };
    //Original startup100018..30 clears3D5A00..44F600; GP3DD970.
    //Signed gp8294..82B0 =>3D5C04..3D5C20; words3E1C48..80 and
    //scratch3E1C28/30/38/40 are in that genuine cleared interval.
    static State& state()noexcept {static State s;return s;}
    static std::atomic<bool>& unknown()noexcept {static std::atomic<bool> v{false};return v;}
    static std::atomic_flag& lock()noexcept {static std::atomic_flag f=ATOMIC_FLAG_INIT;return f;}
    static bool enter()noexcept {
        if(lock().test_and_set(std::memory_order_acquire))return false;
        auto& s=state();const auto thread=std::this_thread::get_id();
        if(s.busy||s.serial==std::numeric_limits<std::uint64_t>::max()||(s.thread!=std::thread::id{}&&s.thread!=thread)){lock().clear(std::memory_order_release);return false;}
        s.thread=thread;s.busy=true;++s.serial;return true;
    }
    static void leave()noexcept {state().busy=false;lock().clear(std::memory_order_release);}
    static bool guard(unsigned index,std::uint32_t readPC,std::uint32_t wordPC,
        std::uint32_t bytePC,std::uint32_t& pc)noexcept {
        pc=readPC;auto& s=state();
        if(!s.busy||s.thread!=std::this_thread::get_id()||unknown().load(std::memory_order_acquire)||index>=8)return false;
        if(!s.guard[index]){pc=wordPC;s.enabled[index]=1;pc=bytePC;s.guard[index]=1;}
        return true;
    }
static bool canWithdrawNative(std::uint64_t serial)noexcept {
        if(!serial||lock().test_and_set(std::memory_order_acquire))return false;
        const auto& s=state();const bool okay=!s.busy&&s.thread==std::this_thread::get_id()&&serial<=s.serial;
        lock().clear(std::memory_order_release);return okay;
    }
    static bool withdrawNative(std::uint64_t serial)noexcept {
        if(!serial||lock().test_and_set(std::memory_order_acquire))return false;
        auto& s=state();const bool okay=!s.busy&&s.thread==std::this_thread::get_id()&&serial<=s.serial;
        if(okay&&s.scratchSerial==serial)s.scratchAccessible=false;
        lock().clear(std::memory_order_release);return okay;
    }
public:
    //Native-only uncertainty, not an original assertion write. Sticky across
    //all owner cleanup; joins only admitted unrepresented134xxx producer paths.
    static void invalidateNative134xxx()noexcept {unknown().store(true,std::memory_order_release);}
};
}

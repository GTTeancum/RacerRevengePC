#pragma once
#include <atomic>
#include <cstdint>
#include <thread>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
//One actual2C5F80 first-use history; native unknown is not a guest write.
class OriginalSunFlareSaturationHistory50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    struct State {
        std::uint8_t guard8DF0=0;
        std::uint32_t enabled444158=0;
        std::thread::id thread;
    };
    //GP3DD970 + signed8DF0 =3D6760; both lanes are in the actual
    //100018..30 cleared3D5A00..44F600 interval.
    static State& state()noexcept {static State s;return s;}
    static std::atomic<bool>& unknown()noexcept {static std::atomic<bool> u{false};return u;}
    static std::atomic_flag& lock()noexcept {static std::atomic_flag f=ATOMIC_FLAG_INIT;return f;}
    static bool observe2C5FDC(std::uint32_t& pc)noexcept {
        pc=0x2c5fdc;
        if(lock().test_and_set(std::memory_order_acquire))return false;
        auto& s=state();const auto current=std::this_thread::get_id();
        if(unknown().load(std::memory_order_acquire)||
           (s.thread!=std::thread::id{}&&s.thread!=current)){
            lock().clear(std::memory_order_release);return false;
        }
        s.thread=current;
        if(!s.guard8DF0){
            pc=0x2c5ff0;s.enabled444158=1;
            pc=0x2c5ff4;s.guard8DF0=1;
        }
        //Admitted rows0..9 satisfy actual signed row < same source zones30.
        //133660(a0=1) returns0 independently of global debug mode36C100.
        lock().clear(std::memory_order_release);return true;
    }
public:
    static void invalidateNative2C5F80()noexcept {unknown().store(true,std::memory_order_release);}
};
}

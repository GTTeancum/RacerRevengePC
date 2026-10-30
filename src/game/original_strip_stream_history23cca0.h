#pragma once
#include <array>
#include <atomic>
#include <cstdint>
namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
//Process startup-zero stream guards3D6200/04, words435458/60;
//UV guards3D61F4/F8/FC, words435440/48/50; triple guards3D61D0/D4/D8
//words4353F8/5400/5408; position guards3D61C4/C8/CC, words4353E0/E8/F0. Not per-World.
class OriginalStripStreamHistory23CCA0 final {
    friend class ::ra_pod_scene50LA::Owner;
    struct State{std::array<std::atomic<std::uint8_t>,11> guard{};std::array<std::atomic<std::uint32_t>,11> enabled{};std::atomic<bool> unknown{false};};
    static State& state()noexcept {static State s;return s;}
public:
    struct Receipt{bool entered=false,ready=false,conditionRead=false,passed=false;std::uint8_t guard=0,stores=0;std::uint32_t enabled=0;};
private:
    static bool begin(unsigned index,Receipt& r)noexcept {
        if(index>=11||r.entered)return false;r.entered=true;auto& s=state();if(s.unknown.load(std::memory_order_acquire))return false;
        r.guard=s.guard[index].load(std::memory_order_acquire);
        if(!r.guard){s.enabled[index].store((index==2||index==5||index==8)?0xffffffffu:1u,std::memory_order_relaxed);r.stores|=1;
            s.guard[index].store(1,std::memory_order_release);r.stores|=2;}
        r.enabled=s.enabled[index].load(std::memory_order_relaxed);r.ready=true;return true;
    }
    static bool finish(Receipt& r,bool condition)noexcept {
        if(!r.ready||r.conditionRead||r.passed)return false;if(r.enabled){r.conditionRead=true;if(!condition)return false;}r.passed=true;return true;
    }
public:
    static bool current(const Receipt& r)noexcept {
        if(!r.entered)return !r.ready&&!r.conditionRead&&!r.passed&&!r.guard&&!r.stores&&!r.enabled;
        if(r.guard>1||r.stores>3||(r.stores&&r.guard)||(r.ready&&!r.guard&&r.stores!=3)||
            (r.conditionRead&&(!r.ready||!r.enabled)))return false;
        return !r.passed||(r.ready&&(!r.enabled||r.conditionRead));
    }
    static void invalidateNative()noexcept {state().unknown.store(true,std::memory_order_release);}
};
}

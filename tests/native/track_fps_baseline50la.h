#pragma once
#include <cstdint>

// Native window observation only. No simulation, render, swap, wait, allocation,
// or clock mutation. The caller supplies steady-clock nanoseconds immediately
// after the real GPU-completed gameplay image's successful window swap call.
namespace track_fps_baseline50LA {
inline constexpr std::uint64_t warmupNanoseconds=5000000000ull;
inline constexpr std::uint64_t measurementNanoseconds=5000000000ull;
struct Settings {
    std::int32_t width=0,height=0,swapInterval=0;
    bool focused=false,unattendedWindow=false;
    bool same(const Settings& other) const noexcept {
        return width==other.width&&height==other.height&&swapInterval==other.swapInterval&&
            focused==other.focused&&unattendedWindow==other.unattendedWindow;
    }
};
struct Receipt {
    Settings settings{};
    std::uint64_t beginNs=0,endNs=0,beginSwaps=0,endSwaps=0,beginTick=0,endTick=0;
    std::uint64_t warmupNs=0,warmupSwaps=0,discardedIntervals=0;
};
struct Boundary {
    std::uint64_t ns=0,swaps=0,tick=0;
};
enum class Observation { none,warmupAnchor,measurementStart,measurementSample,completed };
// Observational output only. Discard and re-anchor can occur in the SAME call.
struct Transition {
    bool discarded=false;
    Observation observation=Observation::none;
    Boundary current{},begin{};
};
class Baseline final {
public:
    explicit Baseline(bool enabled=false) noexcept: enabled_(enabled) {}
    bool enabled() const noexcept { return enabled_; }
    bool complete() const noexcept { return complete_; }
    // Discard rather than subtract paused/capture time from a mixed interval.
    // A fresh actual swap must anchor another full warmup after interruption.
    void discard(Transition* transition=nullptr) noexcept {
        if(transition)*transition={};
        if(!enabled_||complete_)return;
        if(transition)transition->discarded=true;
        if(anchored_)++discardedIntervals_;
        anchored_=measuring_=false;
    }
    bool observe(std::uint64_t now,std::uint64_t swaps,std::uint64_t tick,
        const Settings& settings,Receipt& out,Transition* transition=nullptr) noexcept {
        if(transition)*transition={};
        if(!enabled_||complete_)return false;
        if(anchored_&&(!settings_.same(settings)||now<lastNs_||swaps<lastSwaps_||tick<lastTick_))
            discard(transition);
        if(transition)transition->current={now,swaps,tick};
        if(!anchored_){
            settings_=settings;anchorNs_=now;anchorSwaps_=swaps;
            lastNs_=now;lastSwaps_=swaps;lastTick_=tick;anchored_=true;
            if(transition)transition->observation=Observation::warmupAnchor;
            return false;
        }
        // A repeated callback cannot create another completed frame.
        if(swaps==lastSwaps_)return false;
        lastNs_=now;lastSwaps_=swaps;lastTick_=tick;
        if(!measuring_){
            if(now-anchorNs_<warmupNanoseconds)return false;
            warmupNs_=now-anchorNs_;warmupSwaps_=swaps-anchorSwaps_;
            beginNs_=now;beginSwaps_=swaps;beginTick_=tick;measuring_=true;
            if(transition){
                transition->observation=Observation::measurementStart;
                transition->begin={beginNs_,beginSwaps_,beginTick_};
            }
            return false;
        }
        if(transition){
            transition->observation=Observation::measurementSample;
            transition->begin={beginNs_,beginSwaps_,beginTick_};
        }
        if(now-beginNs_<measurementNanoseconds||tick<=beginTick_)return false;
        out={settings_,beginNs_,now,beginSwaps_,swaps,beginTick_,tick,
            warmupNs_,warmupSwaps_,discardedIntervals_};
        complete_=true; // At most one completed baseline per native session.
        if(transition)transition->observation=Observation::completed;
        return true;
    }
private:
    bool enabled_=false,complete_=false,anchored_=false,measuring_=false;
    Settings settings_{};
    std::uint64_t anchorNs_=0,anchorSwaps_=0,lastNs_=0,lastSwaps_=0,lastTick_=0;
    std::uint64_t beginNs_=0,beginSwaps_=0,beginTick_=0,warmupNs_=0,warmupSwaps_=0;
    std::uint64_t discardedIntervals_=0;
};
} // namespace track_fps_baseline50LA

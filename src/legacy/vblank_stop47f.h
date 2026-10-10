#pragma once
// Observer for matched native experiments. It requests the ordinary cooperative
// stop after the selected VBlank event has applied its original side effects.
#include <charconv>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <stdexcept>

class RrVblankStop47F {
    inline static thread_local RrVblankStop47F* active_=nullptr;
    RrVblankStop47F* previous_=nullptr;
    const void* owner_;
    uint64_t target_=0;
    bool reached_=false;
public:
    explicit RrVblankStop47F(const void* owner):owner_(owner) {
        if(const char* value=std::getenv("RR_STOP_AT_VBLANK47F")) {
            const char* end=value;
            while(*end) ++end;
            const auto parsed=std::from_chars(value,end,target_);
            if(parsed.ec!=std::errc{} || parsed.ptr!=end || !target_ || target_>10000000u)
                throw std::invalid_argument("RR_STOP_AT_VBLANK47F must be an integer in 1..10000000");
        }
        previous_=active_; active_=this;
    }
    ~RrVblankStop47F() {
        if(target_ && !reached_) std::cerr<<"[rr:fixed-stop47f] target="<<target_<<" not_reached=1\n";
        active_=previous_;
    }
    static bool reached(const void* owner,uint64_t tick,uint64_t cycles) {
        auto* observer=active_;
        if(!observer || observer->owner_!=owner || !observer->target_ || observer->reached_ || tick<observer->target_)
            return false;
        observer->reached_=true;
        std::cerr<<"[rr:fixed-stop47f] target="<<observer->target_<<" actual="<<tick<<" ee_cycles="<<cycles<<"\n";
        return true;
    }
};

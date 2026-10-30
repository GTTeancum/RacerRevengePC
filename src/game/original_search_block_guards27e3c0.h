#pragma once
#include <atomic>
#include <cstdint>
namespace rr::game::search_block_guards27E3C0 {
// Same process-global startup-zero guards:GP3DD970+8BC0/8BC4=3D6530/34;
// enabled443078/443080. Legacy allocation adaptations revoke provenance.
inline std::atomic<std::uint32_t> shared{0};
inline constexpr std::uint32_t unknown=0x80000000u;
inline void invalidateNative()noexcept{shared.fetch_or(unknown,std::memory_order_release);}
struct History {bool entered=false,returned=false,asserted=false,allocationNonNull=false;
    std::uint8_t reads=0,stores=0,guard=0,enabled=0;std::uint32_t pc=0;};
inline bool read(unsigned bit,std::uint8_t& out)noexcept {
    const auto s=shared.load(std::memory_order_acquire);if(s&unknown)return false;
    out=std::uint8_t((s>>bit)&1u);return true;
}
inline bool one(unsigned bit)noexcept {
    auto s=shared.load(std::memory_order_acquire);for(;;){if(s&unknown)return false;
        if(shared.compare_exchange_weak(s,s|(1u<<bit),std::memory_order_acq_rel,std::memory_order_acquire))return true;}
}
inline bool run(unsigned g,bool allocationNonNull,History& h,std::uint32_t& pc)noexcept {
    if(g>1||h.entered)return false;h.entered=true;h.allocationNonNull=allocationNonNull;
    const auto at=[&](std::uint32_t v){pc=h.pc=v;};
    at(g?0x27e4a8:0x27e41c);if(!read(g*2+1,h.guard))return false;h.reads|=1;
    if(!h.guard){at(g?0x27e4bc:0x27e430);if(!one(g*2))return false;h.stores|=1;
        at(g?0x27e4c0:0x27e434);if(!one(g*2+1))return false;h.stores|=2;}
    at(g?0x27e4c8:0x27e43c);if(!read(g*2,h.enabled))return false;h.reads|=2;
    if(h.enabled){at(g?0x27e4f4:0x27e454);h.asserted=true;
        if(!allocationNonNull)return false; //Bound before false133660 diagnostics/BRK; no invented return.
    }
    h.returned=true;at(g?0x27e524:0x27e484);return true;
}
inline bool valid(const History& h)noexcept {
    if(!h.entered)return !h.returned&&!h.asserted&&!h.allocationNonNull&&!h.reads&&!h.stores&&!h.guard&&!h.enabled&&!h.pc;
    if(!h.pc||h.reads>3||h.stores>3||h.guard>1||h.enabled>1||
       ((h.stores&2)&&!(h.stores&1))||((h.stores&1)&&(!(h.reads&1)||h.guard))||
       ((h.reads&2)&&(!(h.reads&1)||(!h.guard&&h.stores!=3)))||
       (h.asserted&&(!(h.reads&2)||!h.enabled)))return false;
    return !h.returned||(h.reads==3&&(!h.enabled||(h.asserted&&h.allocationNonNull)));
}
}

#pragma once
#include <atomic>
#include <cstdint>
namespace rr::game::selected_array_guards27ED40 {
//Startup100008..100030 clears3D5A00..44F600. GP3DD970 makes8BD0/8BD4
//the bytes3D6540/44, with enabled words443098/A0. Never reset on cleanup.
inline std::atomic<std::uint32_t> shared{0};
inline constexpr std::uint32_t unknown=0x80000000u;
inline void invalidateNative()noexcept{shared.fetch_or(unknown,std::memory_order_release);}
struct History {
    bool entered=false,returned=false;
    std::uint8_t readMask=0,storeMask=0,assertMask=0;
    std::uint8_t guard[2]{},enabled[2]{};
    std::uint32_t pc=0;
};
inline bool read(unsigned bit,std::uint8_t& out)noexcept {
    const auto state=shared.load(std::memory_order_acquire);
    if(state&unknown)return false;out=static_cast<std::uint8_t>((state>>bit)&1u);return true;
}
inline bool storeOne(unsigned bit)noexcept {
    auto state=shared.load(std::memory_order_acquire);
    for(;;){if(state&unknown)return false;
        if(shared.compare_exchange_weak(state,state|(1u<<bit),std::memory_order_acq_rel,std::memory_order_acquire))return true;}
}
//Only actual27ED10 capacity0 and requested(30,30) are admitted. This helper
//does NOT allocate, publish a selected pointer, or grant constructor return.
inline bool runFresh30(History& h,std::uint32_t& pc)noexcept {
    if(h.entered){pc=h.pc;return false;}h.entered=true;
    const auto at=[&](std::uint32_t v){pc=h.pc=v;};
    constexpr std::uint32_t reads[]={0x27ed54,0x27edc4},writes[]={0x27ed70,0x27edd8},
        guardWrites[]={0x27ed74,0x27eddc},enabledReads[]={0x27ed7c,0x27ede4},assertions[]={0x27ed94,0x27ee04};
    for(unsigned g=0;g<2;++g){
        at(reads[g]);if(!read(g*2+1,h.guard[g]))return false;h.readMask|=1u<<g;
        if(!h.guard[g]){
            at(writes[g]);if(!storeOne(g*2))return false;h.storeMask|=1u<<g;
            at(guardWrites[g]);if(!storeOne(g*2+1))return false;h.storeMask|=1u<<(g+2);
        }
        at(enabledReads[g]);if(!read(g*2,h.enabled[g]))return false;h.readMask|=1u<<(g+2);
        if(h.enabled[g]){
            if(g)at(0x27edf0); //Actual capacity0 read AFTER second enabled read.
            at(assertions[g]);h.assertMask|=1u<<g;
            //133660 receives1:30>0, then capacity==0. Actual return0.
        }
    }
    h.returned=true;at(0x27ee34);return true;
}
inline bool preflight(const History& h)noexcept {
    if(!h.entered)return !h.returned&&!h.readMask&&!h.storeMask&&!h.assertMask&&!h.pc&&
        !h.guard[0]&&!h.guard[1]&&!h.enabled[0]&&!h.enabled[1];
    if((h.readMask&~15u)||(h.storeMask&~15u)||(h.assertMask&~3u)||!h.pc)return false;
    for(unsigned g=0;g<2;++g){
        const bool gr=h.readMask&(1u<<g),er=h.readMask&(1u<<(g+2));
        if(h.guard[g]>1||h.enabled[g]>1||(!gr&&h.guard[g])||(!er&&h.enabled[g]))return false;
        if(g&&gr&&!(h.assertMask&1u))return false;
        if((h.storeMask&(1u<<g))&&(!gr||h.guard[g]))return false;
        if((h.storeMask&(1u<<(g+2)))&&!(h.storeMask&(1u<<g)))return false;
        if(er&&(!gr||(!h.guard[g]&&!(h.storeMask&(1u<<(g+2))))||h.enabled[g]!=1))return false;
        if((h.assertMask&(1u<<g))&&(!er||!h.enabled[g]))return false;
    }
    return !h.returned||(h.readMask==15&&h.assertMask==3&&h.pc==0x27ee34);
}
}

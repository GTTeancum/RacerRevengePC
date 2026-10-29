#pragma once
#include <atomic>
#include <cstdint>
namespace rr::platform::source_portal_guards50LA {
// Actual SLUS_202.68 startup100008..100030 clears3D5A00..44F600.
// gp=3DD970 at10005C:83AC/B0/B4 ->3D5D1C/20/24; enables426F90/98/A0.
// Bits2n=enabled word,2n+1=guard byte. No API resets this process history.
inline std::atomic<std::uint32_t> history{0};
inline constexpr std::uint32_t unknown=0x80000000u;
inline void invalidateNative()noexcept {history.fetch_or(unknown,std::memory_order_release);}
inline bool read(unsigned bit,std::uint8_t& value)noexcept {
    const auto state=history.load(std::memory_order_acquire);
    if(state&unknown)return false;value=static_cast<std::uint8_t>((state>>bit)&1u);return true;
}
inline bool storeOne(unsigned bit)noexcept {
    auto state=history.load(std::memory_order_acquire);
    for(;;){if(state&unknown)return false;
        if(history.compare_exchange_weak(state,state|(1u<<bit),std::memory_order_acq_rel,std::memory_order_acquire))return true;}
}
}

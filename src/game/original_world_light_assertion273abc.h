#pragma once
#include <atomic>
#include <cstdint>
namespace rr::game::world_light_assertion273ABC {
//Startup100008..100030 clears3D5A00..44F600. GP3DD970+signed8B74
//is byte3D64E4. Its enabled word is442FE0. One process history, no World reset.
inline std::atomic<std::uint32_t> shared{0};
inline constexpr std::uint32_t unknown=0x80000000u;
inline void invalidateNative()noexcept{shared.fetch_or(unknown,std::memory_order_release);}
inline bool read(unsigned bit,std::uint8_t& out)noexcept {
    const auto s=shared.load(std::memory_order_acquire);if(s&unknown)return false;
    out=static_cast<std::uint8_t>((s>>bit)&1u);return true;
}
inline bool storeOne(unsigned bit)noexcept {
    auto s=shared.load(std::memory_order_acquire);
    for(;;){if(s&unknown)return false;
        if(shared.compare_exchange_weak(s,s|(1u<<bit),std::memory_order_acq_rel,std::memory_order_acquire))return true;}
}
}

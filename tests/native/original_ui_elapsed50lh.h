#pragma once
#include <cstdint>
#include <limits>

// Native-host elapsed arithmetic only. This helper cannot issue a cold receipt,
// reset a source clock, acknowledge an update, or advance audio. Callers supply
// their retained wall baseline and only actual owner-measured cold credits.
namespace original_ui_elapsed50LH {
enum class Error : std::uint8_t {none,backwards,coldExceedsWall,stall};
struct Interval {
    Error error=Error::none;
    std::uint64_t wallNanoseconds=0,excludedColdNanoseconds=0,activeNanoseconds=0;
};
inline constexpr std::uint64_t activeStallLimitNanoseconds=1000000000ull;
inline constexpr Interval project(std::uint64_t last,std::uint64_t now,std::uint64_t cold) noexcept {
    Interval out;
    // Preserve the existing first/resumed-update convention: zero is an
    // unknown baseline; no inactive duration becomes source catch-up.
    if(!last)return out;
    if(now<last){out.error=Error::backwards;return out;}
    out.wallNanoseconds=now-last;
    if(cold>out.wallNanoseconds){out.error=Error::coldExceedsWall;return out;}
    out.excludedColdNanoseconds=cold;
    out.activeNanoseconds=out.wallNanoseconds-cold;
    if(out.activeNanoseconds>activeStallLimitNanoseconds)out.error=Error::stall;
    return out;
}
inline Interval consume(std::uint64_t& last,std::uint64_t now,std::uint64_t& cold) noexcept {
    const auto out=project(last,now,cold);
    // Match the existing throwing host path: invalid ordering does not mutate;
    // a valid wall interval is consumed before its genuine active-stall gate.
    if(out.error!=Error::backwards&&out.error!=Error::coldExceedsWall){last=now;cold=0;}
    return out;
}
inline bool creditCold(std::uint64_t measured,std::uint64_t& update,std::uint64_t& material) noexcept {
    const auto maximum=std::numeric_limits<std::uint64_t>::max();
    if(measured>maximum-update||measured>maximum-material)return false;
    // Transactional overflow check: neither independent consumer changes if
    // either cannot represent this actual measured interval.
    update+=measured;material+=measured;return true;
}
}

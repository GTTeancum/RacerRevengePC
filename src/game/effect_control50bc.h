#pragma once
#include <cstdint>
#include <cstring>
namespace rr::game {
// Bounded control-state projection only, not particle simulation/rendering.
// Caller must bind concrete owners whose source virtual80/84 are15ACE0/15AD00.
struct EffectControlState50BC {std::uint32_t position30[4],previous40[4],velocity60[4],word19C,intensity204;std::uint8_t flags1D4;};
inline void startEffect50BC(EffectControlState50BC&s) noexcept {s.flags1D4=static_cast<std::uint8_t>((s.flags1D4&0xfdu)|2u);s.word19C=0;}
inline void stopEffect50BC(EffectControlState50BC&s) noexcept {s.flags1D4=static_cast<std::uint8_t>(s.flags1D4&0xfdu);s.flags1D4=static_cast<std::uint8_t>(s.flags1D4&0xfbu);std::memcpy(s.previous40,s.position30,16);s.word19C=0;}
// Source loads incoming position before replacing previous40. Input may alias
// either position record, preserving original15ACB0 sequencing and opaque W.
inline void positionEffect50BC(EffectControlState50BC&s,const std::uint32_t(&v)[4]) noexcept {std::uint32_t copy[4];std::memcpy(copy,v,16);std::memcpy(s.previous40,s.position30,16);std::memcpy(s.position30,copy,16);}
inline void velocityEffect50BC(EffectControlState50BC&s,const std::uint32_t(&v)[4]) noexcept {std::uint32_t copy[4];std::memcpy(copy,v,16);std::memcpy(s.velocity60,copy,16);}
//19A810 is admitted only for the concrete derived owner with field204.
inline void intensityEffect50BC(EffectControlState50BC&s,std::uint32_t word) noexcept {s.intensity204=word;}
}

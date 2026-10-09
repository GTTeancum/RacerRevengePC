#pragma once
#include "effect_authoring50bg.h"
#include "../game/effect_dispatch50be.h"
namespace rr::assets {
struct EffectColdOpaque50BJ {std::uint8_t postBaseFlags1D4;std::uint32_t unusedNonParticleWord;};
// Creates only a native control projection, not an entire source emitter.
// Identity/profile must be supplied from the genuine creation binding. Upper
// constructor flag bits remain explicit opaque input. The nonparticle word is
// unconsumed adapter storage, NOT a recovered source204/render parameter.
// Requires authored Velocity XYZ; preserves constructor W=1 (158C00 writesXYZ).
// Supports fresh modes0/1; false preserves output. Cold, bounded, allocation-free.
bool buildEffectControlOwner50BJ(std::uint32_t id,rr::game::EffectProfile50BE,
 const char*text,std::size_t bytes,const EffectColdOpaque50BJ&,
 rr::game::EffectOwner50BE&) noexcept;
}

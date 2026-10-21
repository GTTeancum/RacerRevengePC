#pragma once
#include "shared_random_owner50id.h"
#include "lap_host_clock50id.h"
#include <cfenv>
#include <cstdint>

namespace rr::game {
// Original2A013C..230, same application lazy seed/range slots and308030 state
// as rider voice, camera, particles and lap events. Never per-trigger RNG.
struct OriginalTriggerChance50MC {
 std::uint32_t* raw=nullptr;SharedRandomOwner50ID* shared=nullptr;HostMonotonicClock50ID* clock=nullptr;
 bool bound()const noexcept{return raw&&shared&&clock;}
};
struct OriginalTriggerChanceReceipt50MC {
 std::uint32_t draw=0,rawAfter=0;std::int32_t percent=0;bool seeded=false,drawn=false;
};
// Positive toward-zero source arithmetic is evaluated in integer/rational
// form, independent of host float casts. This reuses the established recovered
// EE contract in OriginalUiRiderVoiceChoice50KG; it is not newly observed
// runtime FCSR or a generic-MIPS rounding-mode emulation.
inline std::int32_t originalTriggerPercent50MC(std::uint32_t draw)noexcept {
 if(draw>0x7fffffffu)return 0;
 std::uint32_t quantized=draw,shift=0;
 for(auto magnitude=draw;magnitude>=0x01000000u;magnitude>>=1)++shift;
 if(shift)quantized&=~((std::uint32_t{1}<<shift)-1u);
 return static_cast<std::int32_t>((std::uint64_t{quantized}*100u)>>31)+1;
}
inline bool drawOriginalTriggerChance50MC(const OriginalTriggerChance50MC& binding,
 OriginalTriggerChanceReceipt50MC& out)noexcept {
 out={};if(!binding.bound()||std::fegetround()!=FE_TONEAREST)return false;
 auto& seed=binding.shared->seed;auto& range=binding.shared->range;
 if(!seed.lazy80A8){seed.seed3D6B30=0;seed.lazy80A8=true;}
 if(!seed.seed3D6B30){std::uint32_t time=0;if(!sampleHostTimestamp50ID(*binding.clock,time))return false;
  seed.seed3D6B30=time;*binding.raw=time;out.seeded=true;}
 *binding.raw=*binding.raw*0x41c64e6du+0x3039u;out.draw=*binding.raw&0x7fffffffu;out.rawAfter=*binding.raw;out.drawn=true;
 out.percent=originalTriggerPercent50MC(out.draw);
 if(!range.lazy80AC){range.rangeCheck3D6B38=1;range.lazy80AC=true;}
 return !range.rangeCheck3D6B38||(out.percent>=1&&out.percent<=100);
}
}

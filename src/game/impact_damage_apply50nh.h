#pragma once
// One impact-damage hit through the original rule 221754 (the gate at 221754..76C, the
// final-damage rule against +64/+68, the environmental totals D38/D3C and the 0.5-s
// cooldown 7E0), exactly as the impact consumers apply a contact hit. Exposed for the N64
// BreakObject action (n64_track_hazards_runtime50nh.h); PORT EXTENSION, NOT ORIGINAL as a
// caller: Revenge has no trigger that damages a pod. The rule itself is the original's and
// is the same code the consumers run (body in impact_consumers49z.cpp). Kept in its own
// header so impact_consumers49z.h (264 includers) is not edited.
//
// mask: 1 = component index 1, 0x10 = index 0 (221754's two flags). Writes only `state`.
// Returns false when the arithmetic is not ordinary (state then unchanged).
#include "impact_consumers49z.h"

namespace rr::game {
bool applyImpactDamage50NH(const ImpactConsumerInput49Z& input, ImpactConsumerState49Z& state,
                           unsigned mask, float amount) noexcept;
}

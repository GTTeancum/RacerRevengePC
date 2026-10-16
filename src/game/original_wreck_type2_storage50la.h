#pragma once
#include "indexed_type2_contact50cc.h"
#include <array>
#include <vector>

namespace rr::game {
// Embedded solely in the actual E0 query owner. One live result90/backing;
// banks and scratch are cold-allocated transaction workspace, never providers
// or independent collision objects. Native initial stale bytes are not a hit.
struct OriginalWreckType2Storage50LA {
    struct Bank {
        std::vector<SpringCumulativePair48N> pairs;
        std::vector<Type2PairMetadata49X> metadata;
        std::array<SpringRecord48N,50> records{};
        TerrainCandidateEffects48J effect{};
    };
    Type2StaticResult49X result90{};
    Bank live,banks[2],scratch;
    std::vector<Type2WorkItem49X> stack;
};
} // namespace rr::game

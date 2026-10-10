#pragma once
#include "mmi.h"
namespace rr {
struct CompactLeaf47m {
    uint32_t entry;
    const uint32_t* words;
    uint32_t word_count;
};
struct LeafExit47m { uint32_t return_pc, branch_pc, executed_words; bool non_nop_delay; };
// Initial compact integration: straight-line register-only leaves ending in
// JR $ra plus a delay instruction. Unsupported input cannot partly commit CPU
// state. No memory, scheduling, allocation or host clock calls occur here.
bool executeCompactLeaf47m(MmiState& state,const CompactLeaf47m& leaf,LeafExit47m& exit) noexcept;
}

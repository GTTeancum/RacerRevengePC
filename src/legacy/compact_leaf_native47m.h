#pragma once
#include "ps2_runtime.h"
#include "../core/compact_leaf47m.h"
#include <cstring>
#if defined(PS2X_STRICT_RETURN_DIAGNOSTICS) && PS2X_STRICT_RETURN_DIAGNOSTICS
#error Compact leaf adapter requires the verified non-diagnostic return mode
#endif
namespace rr_leaf47m {
struct Entry { rr::CompactLeaf47m leaf; PS2Runtime::RecompiledFunction original; bool sets_delay; };
bool execute(R5900Context& ctx,const Entry& entry) {
    rr::MmiState state{};
    std::memcpy(state.GPR.r+1,ctx.r+1,31*sizeof(ctx.r[0]));
    state.HI.UD[0]=ctx.hi; state.HI.UD[1]=ctx.hi1;
    state.LO.UD[0]=ctx.lo; state.LO.UD[1]=ctx.lo1;
    // SA instructions are excluded by the ELF-verified eligibility manifest.
    rr::LeafExit47m result{};
    if(!rr::executeCompactLeaf47m(state,entry.leaf,result))return false;
    std::memcpy(ctx.r+1,state.GPR.r+1,31*sizeof(ctx.r[0]));
    ctx.hi=state.HI.UD[0];ctx.hi1=state.HI.UD[1];
    ctx.lo=state.LO.UD[0];ctx.lo1=state.LO.UD[1];
    ctx.pc=result.return_pc;
    if(entry.sets_delay){ctx.branch_pc=result.branch_pc;ctx.in_delay_slot=false;}
    return true;
}
}

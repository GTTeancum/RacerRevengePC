#include "compact_leaf47m.h"
#include "scalar47m.h"
namespace rr {
bool executeCompactLeaf47m(MmiState& state,const CompactLeaf47m& leaf,LeafExit47m& exit) noexcept {
    if(!leaf.words || leaf.word_count<2 || leaf.word_count>128 || (leaf.entry&3u)
       || leaf.entry>UINT32_MAX-leaf.word_count*4u || leaf.words[leaf.word_count-2]!=0x03e00008u)
        return false;
    MmiState working=state;
    uint32_t returnPc=0;
    for(uint32_t i=0;i<leaf.word_count;++i){
        if(i==leaf.word_count-2){returnPc=working.GPR.r[31].UL[0];continue;}
        const uint32_t instruction=leaf.words[i];
        // QFSRV's borrowed implementation expects byte SA in [0,15]. Do not
        // invoke it on an invalid/unadapted native SA value.
        if((instruction>>26)==0x1cu && (instruction&0x7ffu)==0x6e8u && working.sa>15)return false;
        if(!executeScalar47m(working,instruction) && !executeMmi(working,instruction))return false;
    }
    state=working;
    exit={returnPc,leaf.entry+(leaf.word_count-2)*4u,leaf.word_count,leaf.words[leaf.word_count-1]!=0};
    return true;
}
}

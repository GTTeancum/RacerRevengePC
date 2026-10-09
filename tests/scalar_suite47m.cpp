#include "scalar_suite47m.h"
#include "scalar47m.h"
#include "compact_leaf47m.h"
bool referenceScalar47m(rr::MmiState&,uint32_t) noexcept;
namespace {
uint32_t randomWord(uint32_t& s){s^=s<<13;s^=s>>17;s^=s<<5;return s;}
void check(CoreTestResult& r,bool ok,unsigned line){++r.checks;if(!ok){++r.failures;if(!r.first_failure_line)r.first_failure_line=line;}}
void digest(CoreTestResult& r,uint32_t x){r.digest=(r.digest^x)*16777619u;}
bool equal(const rr::MmiState& a,const rr::MmiState& b){
    for(unsigned reg=0;reg<32;++reg)for(unsigned lane=0;lane<4;++lane)
        if(a.GPR.r[reg].UL[lane]!=b.GPR.r[reg].UL[lane])return false;
    for(unsigned lane=0;lane<4;++lane)if(a.HI.UL[lane]!=b.HI.UL[lane] || a.LO.UL[lane]!=b.LO.UL[lane])return false;
    return a.code==b.code && a.sa==b.sa;
}
}
#define CHECK(x) check(result,(x),__LINE__)
CoreTestResult run_scalar_suite47m() noexcept {
    CoreTestResult result{0,0,0,2166136261u};uint32_t seed=0x47d412ef;
    const uint32_t edge[]={0,1,0xffffffffu,0x7fffffffu,0x80000000u,0x55555555u,0xaaaaaaaau,0x7fff8000u};
    for(unsigned major=0;major<64;++major)for(unsigned fn=0;fn<64;++fn)for(unsigned round=0;round<32;++round){
        rr::MmiState input{};
        for(unsigned reg=1;reg<34;++reg){auto& value=reg<32?input.GPR.r[reg]:(reg==32?input.HI:input.LO);
            for(unsigned lane=0;lane<4;++lane)value.UL[lane]=round<8?edge[(round+reg+lane)%8]:randomWord(seed);}
        input.sa=round%16;input.code=0xabcdef01;
        const unsigned rs=round%4?1:0,rt=round%5?2:0,rd=round%6;
        const uint32_t code=(major<<26)|(rs<<21)|(rt<<16)|(rd<<11)|(round<<6)|fn;
        auto candidate=input,reference=input;
        const bool a=rr::executeScalar47m(candidate,code),b=referenceScalar47m(reference,code);
        CHECK(a==b);CHECK(equal(candidate,reference));CHECK(a || equal(candidate,input));
        for(const auto& reg:candidate.GPR.r)for(uint32_t value:reg.UL)digest(result,value);
        for(uint32_t value:candidate.HI.UL)digest(result,value);
        for(uint32_t value:candidate.LO.UL)digest(result,value);
        digest(result,candidate.code);digest(result,candidate.sa);
    }
    // Independent ADDIU arithmetic across all immediate encodings; preserve upper lanes.
    for(uint32_t immediate=0;immediate<65536;++immediate){
        rr::MmiState state{};state.GPR.r[1].UL[0]=0x7fff8000u;
        state.GPR.r[2].UD[1]=0x0123456789abcdefull;
        CHECK(rr::executeScalar47m(state,(9u<<26)|(1u<<21)|(2u<<16)|immediate));
        const int32_t value=static_cast<int32_t>(0x7fff8000u+static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(immediate))));
        CHECK(state.GPR.r[2].SD[0]==value);CHECK(state.GPR.r[2].UD[1]==0x0123456789abcdefull);
    }
    // REGIMM SA helpers use the opcode's RT selector, not a destination register.
    for(unsigned sub=24;sub<=25;++sub)for(unsigned v=0;v<32;++v)for(unsigned immediate=0;immediate<32;++immediate){
        rr::MmiState state{};state.GPR.r[1].UL[0]=v;
        CHECK(rr::executeScalar47m(state,(1u<<26)|(1u<<21)|(sub<<16)|immediate));
        const uint32_t expected=sub==24?((v&15)^(immediate&15)):(((v&7)^(immediate&7))<<1);
        CHECK(state.sa==expected);
    }
    {
        // Return target must be latched before the delay slot overwrites RA.
        const uint32_t words[]={0x24821234u,0x70222e28u,0x03e00008u,0x241fffffu};
        rr::MmiState state{};state.GPR.r[4].UL[0]=7;state.GPR.r[31].UL[0]=0x10203040;
        rr::LeafExit47m exit{};
        CHECK(rr::executeCompactLeaf47m(state,{0x1000,words,4},exit));
        CHECK(exit.return_pc==0x10203040 && exit.branch_pc==0x1008 && exit.executed_words==4 && exit.non_nop_delay);
        CHECK(state.GPR.r[31].SD[0]==-1 && state.GPR.r[2].SD[0]==0x123b);
        const auto before=state;const auto beforeExit=exit;
        const uint32_t bad[]={0x24821234u,0x8c220000u,0x03e00008u,0};
        CHECK(!rr::executeCompactLeaf47m(state,{0x1000,bad,4},exit));CHECK(equal(state,before));
        CHECK(exit.return_pc==beforeExit.return_pc && exit.executed_words==beforeExit.executed_words);
        CHECK(!rr::executeCompactLeaf47m(state,{0x1001,words,4},exit));
        CHECK(!rr::executeCompactLeaf47m(state,{0xfffffff8u,words,4},exit));
        CHECK(!rr::executeCompactLeaf47m(state,{0x1000,nullptr,4},exit));
        const uint32_t qfsrv[]={0x70221ee8u,0x03e00008u,0};
        state.sa=128;const auto invalidSa=state;
        CHECK(!rr::executeCompactLeaf47m(state,{0x1000,qfsrv,3},exit));CHECK(equal(state,invalidSa));
    }
    return result;
}

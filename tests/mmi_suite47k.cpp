#include "mmi_suite47k.h"
#include "mmi.h"

bool referenceMmi47k(rr::MmiState&, uint32_t) noexcept;
namespace {
void check(CoreTestResult& r, bool ok, unsigned line) {
    ++r.checks;
    if (!ok) { ++r.failures; if (!r.first_failure_line) r.first_failure_line=line; }
}
void digest(CoreTestResult& r, uint32_t x) { r.digest=(r.digest^x)*16777619u; }
uint32_t next(uint32_t& s) { s^=s<<13; s^=s>>17; s^=s<<5; return s; }
bool equalRegister(const rr::MmiRegister& a, const rr::MmiRegister& b) {
    for(unsigned i=0;i<4;++i) if(a.UL[i]!=b.UL[i]) return false;
    return true;
}
bool equalState(const rr::MmiState& a,const rr::MmiState& b) {
    for(unsigned i=0;i<32;++i) if(!equalRegister(a.GPR.r[i],b.GPR.r[i])) return false;
    return equalRegister(a.HI,b.HI) && equalRegister(a.LO,b.LO) && a.code==b.code && a.sa==b.sa;
}
uint32_t code(unsigned fn,unsigned sub,unsigned rs,unsigned rt,unsigned rd) {
    return (0x1cu<<26)|(rs<<21)|(rt<<16)|(rd<<11)|(sub<<6)|fn;
}
}
#define CHECK(x) check(result,(x),__LINE__)
CoreTestResult run_mmi_suite47k() noexcept {
    CoreTestResult result{0,0,0,2166136261u};
    uint32_t seed=0x47c099ab;
    const uint32_t edges[]={0u,1u,0xffffffffu,0x7fffffffu,0x80000000u,
        0x7fff8000u,0x80007fffu,0x7f80ff00u,0x0080ff7fu,0xaaaaaaaau,0x55555555u,0x00010001u};
    const unsigned aliases[][3]={{1,2,3},{1,2,1},{1,2,2},{1,1,3},{1,1,1},
        {0,2,3},{1,0,3},{1,2,0},{0,0,0},{31,30,29}};
    // Every major/subfunction field, all shifts, zero and overlapping registers,
    // signed boundaries, packed saturation patterns, HI/LO carries, random words.
    for(unsigned fn=0;fn<64;++fn) for(unsigned sub=0;sub<32;++sub)
        for(unsigned round=0;round<16;++round) for(const auto& alias:aliases) {
            rr::MmiState original{};
            for(unsigned reg=1;reg<34;++reg) {
                auto& v=reg<32?original.GPR.r[reg]:(reg==32?original.HI:original.LO);
                for(unsigned lane=0;lane<4;++lane)
                    v.UL[lane]=round<12?edges[(round+reg+lane)%12]:next(seed);
            }
            original.sa=round; original.code=0xfeed1234u;
            auto candidate=original, reference=original;
            const uint32_t instruction=code(fn,sub,alias[0],alias[1],alias[2]);
            const bool accepted=rr::executeMmi(candidate,instruction);
            const bool referenceAccepted=referenceMmi47k(reference,instruction);
            CHECK(accepted==referenceAccepted);
            CHECK(equalState(candidate,reference));
            CHECK(accepted || equalState(candidate,original));
            CHECK(equalRegister(candidate.GPR.r[0],original.GPR.r[0]));
            for(const auto& v:candidate.GPR.r) for(auto word:v.UL) digest(result,word);
            for(auto word:candidate.HI.UL) digest(result,word);
            for(auto word:candidate.LO.UL) digest(result,word);
            digest(result,candidate.code); digest(result,candidate.sa);
        }
    // Independently specified byte concatenation validates all QFSRV offsets,
    // particularly the 64-bit boundary and overlapping destination cases.
    for(unsigned shift=0;shift<16;++shift) for(const auto& alias:aliases) {
        rr::MmiState state{};
        for(unsigned r=1;r<32;++r) for(unsigned b=0;b<16;++b)
            state.GPR.r[r].UC[b]=static_cast<uint8_t>(r*17+b);
        uint8_t source[32];
        for(unsigned b=0;b<16;++b) {
            source[b]=state.GPR.r[alias[1]].UC[b];
            source[b+16]=state.GPR.r[alias[0]].UC[b];
        }
        state.sa=shift;
        CHECK(rr::executeMmi(state,code(0x28,0x1b,alias[0],alias[1],alias[2])));
        for(unsigned b=0;b<16;++b)
            CHECK(state.GPR.r[alias[2]].UC[b]==(alias[2]?source[b+shift]:0));
    }
    // Saturating unsigned byte addition is checked without the PCSX2 oracle.
    for(unsigned x=0;x<256;++x) for(unsigned y=0;y<256;++y) {
        rr::MmiState state{};
        for(unsigned b=0;b<16;++b) { state.GPR.r[1].UC[b]=static_cast<uint8_t>(x); state.GPR.r[2].UC[b]=static_cast<uint8_t>(y); }
        CHECK(rr::executeMmi(state,code(0x28,0x18,1,2,3)));
        for(unsigned b=0;b<16;++b) CHECK(state.GPR.r[3].UC[b]==(x+y>255?255:x+y));
    }
    // Wrong major opcodes must leave even the saved instruction untouched.
    for(unsigned major=0;major<64;++major) if(major!=0x1c) {
        rr::MmiState state{}; state.code=0xabcdef01; state.LO.UL[2]=0x87654321;
        const auto before=state;
        CHECK(!rr::executeMmi(state,(major<<26)|code(0,0,1,2,3)&0x03ffffffu));
        CHECK(equalState(state,before));
    }
    return result;
}

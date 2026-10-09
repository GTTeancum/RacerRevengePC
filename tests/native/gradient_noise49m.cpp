#include "gradient_noise49m.h"
#include "../../tools/gameplay/gradient_noise_oracle49m.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>

namespace {
bool watchAllocations=false;
std::size_t hotAllocations=0;
}
void* operator new(std::size_t n){if(watchAllocations)++hotAllocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void*p)noexcept{std::free(p);}
void operator delete[](void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}
void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
namespace {
using rr::game::GradientNoiseState49M;
using rr::game::evaluateGradientNoise49M;
using original49m::bits;
std::size_t checks=0,cases=0;
std::bitset<original49m::Machine::pcCount> covered,taken,notTaken,excluded;
void require(bool value,const char*what){++checks;if(!value){watchAllocations=false;std::fprintf(stderr,"FAIL %s after%zu checks\n",what,checks);std::exit(1);}}
original49m::State oracleState(const GradientNoiseState49M&s){original49m::State r;for(unsigned i=0;i<256;++i){r.permutation[i]=s.permutation[i];r.gradient1D[i]=s.gradient1D[i];}r.scratchCell=s.scratchCell;r.initialized=s.initialized;return r;}
void compare(const GradientNoiseState49M&a,const original49m::State&b){require(a.initialized==b.initialized,"readiness");require(a.scratchCell==b.scratchCell,"signed conversion scratch");for(unsigned i=0;i<256;++i){require(a.permutation[i]==b.permutation[i],"permutation table");require(bits(a.gradient1D[i])==bits(b.gradient1D[i]),"gradient table");}}
void one(GradientNoiseState49M&state,std::uint32_t&raw,float x){
    original49m::Machine machine;
    const bool cold=!state.initialized;
    const auto expected=machine.run(oracleState(state),raw,x);
    float actual=original49m::fp(0x7fc12345u);
    watchAllocations=true;const bool ok=evaluateGradientNoise49M(state,raw,x,actual);watchAllocations=false;
    require(ok,"native admitted supported source input");require(bits(actual)==bits(expected.result),"noise bit equality");require(raw==expected.raw,"raw RNG transition");compare(state,expected.state);
    require(machine.draws==(cold?1791u:0u),"exact cold/ready draw count");
    require(machine.maxPermutationReadIndex<256&&machine.maxGradientReadIndex<256,"mirror tails not read");
    covered|=machine.executedPcs;taken|=machine.branchTakenPcs;notTaken|=machine.branchNotTakenPcs;excluded|=machine.excludedPcs;++cases;
}
void reject(GradientNoiseState49M state,std::uint32_t raw,float x){
    std::array<unsigned char,sizeof(state)> before{};std::memcpy(before.data(),&state,sizeof(state));
    const auto beforeRaw=raw;float result=original49m::fp(0x7fc12345u);
    watchAllocations=true;const bool ok=evaluateGradientNoise49M(state,raw,x,result);watchAllocations=false;
    require(!ok,"invalid rejected");require(raw==beforeRaw,"rejection raw unchanged");require(bits(result)==0x7fc12345u,"rejection output unchanged");require(std::memcmp(before.data(),&state,sizeof(state))==0,"rejection state bytewise unchanged");
}
std::uint32_t fixtureWord(std::uint32_t&x){x^=x<<13;x^=x>>17;x^=x<<5;return x;}
}
int main(){
    const int modes[]={FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO};
    require(std::fesetround(FE_TONEAREST)==0,"golden nearest");
    struct Golden{std::uint32_t seed,finalRaw,result;};
    const Golden goldens[]={{0,0x2eec7f83u,0xbe6ae700u},{1,0xe84456e8u,0xbddb6000u},
        {0xdeadbeefu,0x8f0c8cceu,0x3cbb0800u},{0xffffffffu,0x7594a81eu,0x3d87b400u}};
    for(const auto&g:goldens){GradientNoiseState49M state;auto raw=g.seed;float out=0;require(evaluateGradientNoise49M(state,raw,0.375f,out),"golden succeeds");require(raw==g.finalRaw&&bits(out)==g.result,"independent original-word golden");}
    const float coordinates[]={0.0f,-0.0f,0.1f,-0.1f,0.25f,255.75f,256.25f,
        -4096.0f,-4095.5f,-4096.5f,-4097.25f,-4351.75f,-4352.5f,
        -3840.5f,-3841.0f,65535.5f,-65535.5f,0x1p24f,-0x1p24f,
        -2147487744.0f,2147479424.0f,std::numeric_limits<float>::denorm_min(),
        -std::numeric_limits<float>::denorm_min()};
    std::uint32_t arbitrary=0x6879ab12u;
    for(const int mode:modes){require(std::fesetround(mode)==0,"rounding mode available");
        // Every low9-bit seed class; high bits remain part of the checked raw RNG.
        for(std::uint32_t seed=0;seed<512;++seed){
            GradientNoiseState49M state;
            for(unsigned i=0;i<256;++i){state.permutation[i]=0xffffffffu;state.gradient1D[i]=original49m::fp(0x7fc12345u);}
            state.scratchCell=-17;
            std::uint32_t raw=(seed&1u)?(0xfffffe00u|seed):seed;
            one(state,raw,coordinates[seed%(sizeof(coordinates)/sizeof(*coordinates))]);
            // Cold-state projection is a permutation and exact1/256 gradients.
            std::bitset<256> seen;
            for(unsigned i=0;i<256;++i){require(!seen.test(state.permutation[i]),"no duplicate shuffled entry");seen.set(state.permutation[i]);volatile float scale=state.gradient1D[i]*256.0f;require(scale==std::trunc(scale),"exact gradient quantum");}
            one(state,raw,coordinates[(seed+1)%(sizeof(coordinates)/sizeof(*coordinates))]);
        }
        GradientNoiseState49M ready;std::uint32_t raw=0x8badf00du;one(ready,raw,0.0f);
        for(const float x:coordinates)one(ready,raw,x);
        // Every cell and seam, both signs of the shifted fraction.
        for(unsigned i=0;i<256;++i){one(ready,raw,static_cast<float>(i)-4096.0f+0.5f);one(ready,raw,-4096.0f-static_cast<float>(i)-0.5f);}
        for(unsigned i=0;i<2048;++i){const auto word=fixtureWord(arbitrary);const auto signedSmall=static_cast<std::int32_t>(word&0x1fffffu)-0x100000;volatile float x=static_cast<float>(signedSmall)*0.015625f;one(ready,raw,x);}
        for(unsigned i=0;i<4096;++i){const float x=original49m::fp(fixtureWord(arbitrary));volatile float shifted=4096.0f+x;if(std::isfinite(x)&&std::isfinite(shifted)&&shifted>=-0x1p31f&&shifted<0x1p31f)one(ready,raw,x);}
        for(bool initialized:{false,true}){auto invalid=ready;invalid.initialized=initialized;
            reject(invalid,raw,std::numeric_limits<float>::infinity());reject(invalid,raw,-std::numeric_limits<float>::infinity());reject(invalid,raw,std::numeric_limits<float>::quiet_NaN());reject(invalid,raw,2147479552.0f);reject(invalid,raw,-2147488000.0f);}
        auto invalid=ready;invalid.permutation[0]=256;reject(invalid,raw,0.25f);
        invalid=ready;invalid.gradient1D[invalid.permutation[0]]=1.0f;reject(invalid,raw,0.25f);
        invalid=ready;invalid.gradient1D[invalid.permutation[1]]=std::numeric_limits<float>::quiet_NaN();reject(invalid,raw,0.25f);
    }
    require(covered.count()==187&&excluded.count()==78,"exact word accounting");
    for(const std::uint32_t pc:{0x14f028u,0x14f02cu,0x14f030u,0x14f070u,0x14f074u,0x14f078u,0x14f108u,0x14f10cu,0x14f110u,0x14f1e8u,0x14f1ecu,0x14f1f0u})require(!covered.test(original49m::Machine::pcIndex(pc))&&!excluded.test(original49m::Machine::pcIndex(pc)),"only unreachable signed-correction words remain");
    require(hotAllocations==0,"zero hot allocations");require((covered&excluded).none(),"exclusions never claimed executed");
    require(taken.test(original49m::Machine::pcIndex(0x14eed0))&&notTaken.test(original49m::Machine::pcIndex(0x14eed0)),"both lazy paths");
    for(const std::uint32_t pc:{0x14f020u,0x14f068u,0x14f100u,0x14f1e0u}){require(taken.test(original49m::Machine::pcIndex(pc)),"rand31 nonnegative");require(!notTaken.test(original49m::Machine::pcIndex(pc)),"negative remainder branch unreachable");}
    std::fesetround(FE_TONEAREST);
    std::printf("{\"suite\":\"gradient-noise49m\",\"status\":\"passed\",\"cases\":%zu,\"checks\":%zu,\"rounding_modes\":4,\"hot_allocations\":%zu,\"state_bytes\":%zu,\"original_word_count\":%zu,\"executed_words\":%zu,\"excluded_words\":%zu,\"taken_branch_pcs\":%zu,\"not_taken_branch_pcs\":%zu,\"unexecuted_pcs\":[",cases,checks,hotAllocations,sizeof(GradientNoiseState49M),original49m::Machine::pcCount,covered.count(),excluded.count(),taken.count(),notTaken.count());
    bool comma=false;for(const auto range:std::array<std::array<std::uint32_t,2>,3>{{{{0x14eec0,0x14efc4}},{{0x14efd0,0x14f2f0}},{{0x308030,0x308060}}}})for(auto pc=range[0];pc<range[1];pc+=4)if(!covered.test(original49m::Machine::pcIndex(pc))){std::printf("%s\"0x%08x\"",comma?",":"",pc);comma=true;}
    std::puts("]}");
}

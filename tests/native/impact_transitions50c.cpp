#include "../../src/game/impact_transitions50c.h"
#include <array>
#include <cfenv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <new>
using namespace rr::game;
static bool hot=false;static unsigned allocs=0,checks=0,fails=0;
void* operator new(std::size_t n){if(hot)++allocs;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}void operator delete(void*p)noexcept{std::free(p);}void operator delete[](void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
template<class T>static auto bytes(const T&t){std::array<unsigned char,sizeof t>b{};std::memcpy(b.data(),&t,sizeof t);return b;}
static void check(bool yes,const char*what,unsigned row=0){++checks;if(!yes){if(fails<20)std::printf("FAIL %u %s\n",row,what);++fails;}}
static auto call(const ImpactTransitionInput50C&i){hot=true;const auto r=inspectImpactTransition50C(i);hot=false;return r;}
static void put(float&f,std::uint32_t bits){std::memcpy(&f,&bits,4);}
int main(int argc,char**argv){
    static_assert(sizeof(ImpactTransitionInput50C)==28);static_assert(sizeof(ImpactTransitionResult50C)==16);
    if(argc!=2)return 2;check(std::fesetround(FE_TONEAREST)==0,"nearest");std::ifstream file(argv[1],std::ios::binary);std::uint32_t rows=0,width=0;
    file.read(reinterpret_cast<char*>(&rows),4);file.read(reinterpret_cast<char*>(&width),4);if(!file||rows!=8192||width!=11)return 3;
    for(unsigned row=0;row<rows;++row){
        std::uint32_t words[11]{};file.read(reinterpret_cast<char*>(words),sizeof words);if(!file)return 4;
        ImpactTransitionInput50C i;std::memcpy(&i,words,sizeof i);const auto snapshot=bytes(i);const auto got=call(i);
        check(std::memcmp(&got,words+7,sizeof got)==0,"original first call or complete return",row);
        check(bytes(i)==snapshot,"immutable input bytes",row);
    }
    ImpactTransitionInput50C i{};
    for(int round:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
        check(std::fesetround(round)==0,"set round");const auto b=bytes(i);const auto got=call(i);
        check(got.status==ImpactTransitionStatus50C::arithmetic&&got.sourcePC==0x225c50&&bytes(i)==b,"round rejection without mutation");
    }
    check(std::fesetround(FE_TONEAREST)==0,"restore nearest");
    const std::uint32_t exceptional[]={0x7f800001,0xff800001,0x7fc12345,0xffc12345,0x7f800000,0xff800000,1,0x80000001,0x007fffff,0x807fffff};
    for(auto value:exceptional){
        i={};put(i.impactBA8,value);put(i.threshold50,value);const auto before=bytes(i);std::uint32_t raw=0;std::memcpy(&raw,&i.impactBA8,4);
        check(raw==value,"raw exceptional precall");
        check(call(i).status==ImpactTransitionStatus50C::noTransition&&bytes(i)==before,"low damage source gate leaves BA8/50 unconsumed");
        i.flagsB9D=1;check(call(i).sourcePC==0x225d10,"actual flag selects88 without unused scalar validation");
        i={};put(i.damage7D0[1],value);const auto badInput=bytes(i);const auto bad=call(i);
        check(bad.status==ImpactTransitionStatus50C::arithmetic&&bad.sourcePC==0x225c80&&bytes(i)==badInput,"consumed first operand rejected raw");
        i={};i.damage7D0[1]=1;put(i.impactBA8,value);i.threshold50=1;check(call(i).sourcePC==0x225cd4,"consumed BA8 rejected");
        i.impactBA8=0;put(i.threshold50,value);check(call(i).sourcePC==0x225ce4,"consumed threshold rejected");
        // First high damage + actual force reaches88 without consuming other damage.
        i={};i.damage7D0[1]=1;i.impactBA8=2;i.threshold50=1;put(i.damage7D0[3],value);put(i.damage7D0[0],value);put(i.damage7D0[2],value);
        const auto forced=call(i);check(forced.status==ImpactTransitionStatus50C::unsupportedTransition&&forced.sourcePC==0x225d10,"source short circuit preserves unused damage");
    }
    for(auto sign:{0u,0x80000000u}){
        i={};for(auto&d:i.damage7D0)put(d,sign);put(i.impactBA8,sign);put(i.threshold50,sign);
        check(call(i).status==ImpactTransitionStatus50C::noTransition,"signed zeros admitted");
        i.damage7D0[1]=1;const auto tie=call(i);check(tie.sourcePC==0x225e7c,"abs signedzero equal threshold keeps90 route");
    }
    check(allocs==0,"no hot allocations");std::printf("50C decision checks=%u failures=%u hot_allocations=%u fixtures=%u\n",checks,fails,allocs,rows);return fails?1:0;
}

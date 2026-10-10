#include "../../src/game/impact_aftermath50a.h"
#include <array>
#include <cfenv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <new>
using namespace rr::game;
static bool hot=false;static unsigned allocations=0,checks=0,failures=0;
void* operator new(std::size_t n) {if(hot)++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
template<class T>static auto bytes(const T& t){std::array<unsigned char,sizeof t>b{};std::memcpy(b.data(),&t,sizeof t);return b;}
static void check(bool v,const char* text,unsigned row=0){++checks;if(!v){if(failures<15)std::printf("FAIL %u %s\n",row,text);++failures;}}
static std::uint32_t word(const float& f){std::uint32_t w;std::memcpy(&w,&f,4);return w;}
static void put(float& f,std::uint32_t w){std::memcpy(&f,&w,4);}
static auto call(const ImpactRepairInput50A&i,const ImpactRepairState50A&s,ImpactRepairState50A&o){hot=true;auto r=repairImpactDamage50A(i,s,o);hot=false;return r;}
int main(int argc,char**argv){
    static_assert(sizeof(ImpactRepairInput50A)==16);static_assert(sizeof(ImpactRepairState50A)==28);
    if(argc!=2)return 2;check(std::fesetround(FE_TONEAREST)==0,"nearest");
    std::ifstream f(argv[1],std::ios::binary);std::uint32_t rows=0,width=0;f.read(reinterpret_cast<char*>(&rows),4);f.read(reinterpret_cast<char*>(&width),4);
    if(!f||rows!=4096||width!=19)return 3;
    for(unsigned row=0;row<rows;++row){
        std::array<std::uint32_t,19> r{};f.read(reinterpret_cast<char*>(r.data()),sizeof r);if(!f)return 4;
        ImpactRepairInput50A i;ImpactRepairState50A s,o;std::memcpy(&i,r.data(),sizeof i);std::memcpy(&s,r.data()+4,sizeof s);
        const auto ib=bytes(i);const auto sb=bytes(s);auto status=call(i,s,o);
        check(static_cast<unsigned>(status)==r[11],"status",row);
        if(std::memcmp(&o,r.data()+12,sizeof o)&&failures<5){
            std::array<std::uint32_t,7>ow{};std::memcpy(ow.data(),&o,sizeof o);
            for(unsigned j=0;j<7;++j)if(ow[j]!=r[12+j])std::printf("word%u native=%08x original=%08x\n",j,ow[j],r[12+j]);
        }
        check(std::memcmp(&o,r.data()+12,sizeof o)==0,"source state words",row);
        auto alias=s;check(call(i,alias,alias)==status&&bytes(alias)==bytes(o),"state alias",row);
        check(bytes(i)==ib&&bytes(s)==sb,"input immutable bytes",row);
    }
    ImpactRepairInput50A i{2,15,1.0f/60.0f,.014f};ImpactRepairState50A s{},o{};
    for(int rounding:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
        check(std::fesetround(rounding)==0,"set rounding");std::memset(&o,0x7a,sizeof o);const auto before=bytes(o);
        check(call(i,s,o)==ImpactAftermathStatus50A::arithmetic&&bytes(o)==before,"round atomic reject");
        auto gate=i;gate.flagsB9C=0;check(call(gate,s,o)==ImpactAftermathStatus50A::skipped&&bytes(o)==bytes(s),"skip precedes rounding");
    }
    check(std::fesetround(FE_TONEAREST)==0,"restore nearest");
    const std::uint32_t patterns[]={0x7f800001,0xff800001,0x7fc12345,0xffc12345,0x7f800000,0xff800000,1,0x80000001,0x007fffff,0x807fffff,0,0x80000000};
    for(auto w:patterns){
        for(unsigned j=0;j<sizeof s;j+=4)std::memcpy(reinterpret_cast<unsigned char*>(&s)+j,&w,4);
        auto gate=i;gate.flagsB9C=0;put(gate.factor,w);put(gate.repairCF4,w);
        check(word(gate.factor)==w,"raw bits precall");check(call(gate,s,o)==ImpactAftermathStatus50A::skipped&&bytes(o)==bytes(s),"raw early copy");
        s.timer7E4=.5f;auto timed=i;put(timed.repairCF4,w);
        check(call(timed,s,o)==ImpactAftermathStatus50A::applied&&word(o.damage7D0[0])==w&&word(o.environmentalD38)==w,"unconsumed raw timed copy");
        auto absent=i;absent.componentPresence=0;s.timer7E4=0;put(absent.factor,w);put(absent.repairCF4,w);
        check(call(absent,s,o)==ImpactAftermathStatus50A::applied&&word(o.damage7D0[0])==w,"real absent models copy");
        if((w&0x7fffffffu)!=0){s={};put(s.damage7D0[0],w);std::memset(&o,0xab,sizeof o);const auto before=bytes(o);
            check(call(i,s,o)==ImpactAftermathStatus50A::arithmetic&&bytes(o)==before,"consumed exceptional atomic reject");}
    }
    s={};s.damage7D0[0]=.4f;s.damage7D0[1]=.4f;s.damage7D0[2]=.6f;s.damage7D0[3]=.5f;i.repairCF4=.125f;
    check(call(i,s,o)==ImpactAftermathStatus50A::applied&&word(o.damage7D0[2])==word(o.damage7D0[1]),"original secondary cross-pair assignment");
    s={};put(s.timer7E4,0x00800001);put(i.factor,0x00800000);std::memset(&o,0xa1,sizeof o);const auto before=bytes(o);
    check(call(i,s,o)==ImpactAftermathStatus50A::arithmetic&&bytes(o)==before,"subnormal result reject");
    s={};put(s.damage7D0[0],0x7f7fffff);put(s.damage7D0[1],0xff7fffff);
    check(call(i,s,o)==ImpactAftermathStatus50A::arithmetic,"overflow result reject");
    check(allocations==0,"no hot allocation");std::printf("50A repair checks=%u failures=%u allocations=%u fixtures=%u\n",checks,failures,allocations,rows);return failures?1:0;
}

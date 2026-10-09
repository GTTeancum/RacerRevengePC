#include "../../src/game/impact_effects50b.h"
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
static auto call(const SceneGraph49P&g,const ImpactEffectsInput50B&i,std::uint32_t&r,ImpactEffectsWorkspace50B&w,ImpactEffectEvent50B*e,std::size_t cap,std::size_t&count){hot=true;auto s=updateImpactEffects50B(g,i,r,w,e,cap,count);hot=false;return s;}
int main(int argc,char**argv){
    static_assert(sizeof(SceneNode49P)==144);static_assert(sizeof(ImpactEffectEvent50B)==32);static_assert(offsetof(ImpactEffectsInput50B,damage00Matches)==184);
    if(argc!=2)return 2;check(std::fesetround(FE_TONEAREST)==0,"nearest");std::ifstream f(argv[1],std::ios::binary);std::uint32_t rows=0,width=0;
    f.read(reinterpret_cast<char*>(&rows),4);f.read(reinterpret_cast<char*>(&width),4);if(!f||rows!=1024||width!=1254)return 3;
    for(unsigned row=0;row<rows;++row){
        std::array<std::uint32_t,1254> r{};f.read(reinterpret_cast<char*>(r.data()),sizeof r);if(!f)return 4;
        ImpactEffectsInput50B i{};std::memcpy(&i,r.data(),184);std::uint8_t names[12]{};for(unsigned j=0;j<12;++j)names[j]=static_cast<std::uint8_t>(r[46+j]);i.damage00Matches=names;i.nameCount=12;
        SceneNode49P nodes[12];std::memcpy(nodes,r.data()+59,sizeof nodes);auto rng=r[58];ImpactEffectsWorkspace50B w{};ImpactEffectEvent50B events[42];std::memset(events,0x8a,sizeof events);std::size_t count=999;
        const auto before=bytes(nodes);const auto eb=bytes(events);const auto ib=bytes(i);const auto nb=bytes(names);
        auto status=call({nodes,12},i,rng,w,events,41,count);
        check(static_cast<unsigned>(status)==r[491],"status",row);
        if(r[491])check(bytes(nodes)==before&&bytes(events)==eb&&rng==r[58]&&count==999,"atomic binding reject",row);
        else{
            check(rng==r[492],"shared RNG",row);check(std::memcmp(nodes,r.data()+493,sizeof nodes)==0,"source graph caches",row);
            check(count==r[925],"event count",row);
            if(count<=41&&std::memcmp(events,r.data()+926,count*sizeof(events[0]))&&fails<4){
                const auto*got=reinterpret_cast<const std::uint32_t*>(events);
                for(unsigned j=0;j<count*8;++j)if(got[j]!=r[926+j])std::printf("eventword%u got=%08x want=%08x\n",j,got[j],r[926+j]);
            }
            check(count<=41&&std::memcmp(events,r.data()+926,count*sizeof(events[0]))==0,"ordered original event words",row);
            check(std::memcmp(&events[41],eb.data()+41*sizeof(events[0]),sizeof(events[0]))==0,"output bound",row);
            if(count){
                std::memcpy(nodes,before.data(),sizeof nodes);rng=r[58];std::size_t c=456;const auto outputs=bytes(events);
                check(call({nodes,12},i,rng,w,events,count-1,c)==ImpactEffectsStatus50B::capacity&&bytes(nodes)==before&&bytes(events)==outputs&&rng==r[58]&&c==456,"late capacity atomicity",row);
            }
        }
        check(bytes(i)==ib&&bytes(names)==nb,"immutable descriptors",row);
    }
    ImpactEffectsInput50B i{};for(auto&m:i.models)m.node=SceneNodeIndex49P::none;ImpactEffectsWorkspace50B w{};ImpactEffectEvent50B ev[1]{};std::uint32_t rng=123;std::size_t n=987;
    check(call({nullptr,0},i,rng,w,nullptr,0,n)==ImpactEffectsStatus50B::applied&&n==0&&rng==123,"empty actual bindings");
    for(int round:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){check(std::fesetround(round)==0,"set round");const auto e=bytes(ev);n=123;
        check(call({nullptr,0},i,rng,w,ev,1,n)==ImpactEffectsStatus50B::arithmetic&&bytes(ev)==e&&n==123&&rng==123,"round atomic");}
    check(std::fesetround(FE_TONEAREST)==0,"restore round");
    // Raw payloads in source-copy-only cached positions and velocity, and poison
    // numerics behind authentic hidden/absent gates. No float-byvalue assertions.
    const std::uint32_t patterns[]={0x7f800001,0xff800001,0x7fc12345,0xffc12345,0x7f800000,0xff800000,1,0x80000001,0x007fffff,0x807fffff,0,0x80000000};
    SceneNode49P node{};node.parent180=node.firstChild184=node.nextSibling188=SceneNodeIndex49P::none;node.sceneFlags190=1;
    std::uint8_t match=1;i.damage00Matches=&match;i.nameCount=1;i.models[0].node=static_cast<SceneNodeIndex49P>(0);i.models[0].effect804={1,0};i.timers820[0]=.1f;
    ImpactEffectEvent50B output[42];
    for(auto bits:patterns){
        std::memcpy(&node.world100.v[12],&bits,4);std::memcpy(&i.bodyVelocity70[0],&bits,4);
        const auto before=bytes(node);const auto inputs=bytes(i);std::uint32_t raw=0;std::memcpy(&raw,&node.world100.v[12],4);
        check(raw==bits,"raw payload precall");n=0;
        check(call({&node,1},i,rng,w,output,41,n)==ImpactEffectsStatus50B::applied&&n==3,"copy-only output calls");
        std::uint32_t position=0,velocity=0;std::memcpy(&position,&output[2].value[0],4);std::memcpy(&velocity,&output[1].value[0],4);
        check(position==bits&&velocity==bits&&bytes(node)==before&&bytes(i)==inputs,"source-copy exceptional words");
        i.models[0].flags03=4;std::memcpy(&i.damage7D0[1],&bits,4);std::memcpy(&i.timers820[0],&bits,4);i.nameCount=0;i.damage00Matches=nullptr;
        check(call({&node,1},i,rng,w,nullptr,0,n)==ImpactEffectsStatus50B::applied&&n==0,"hidden gate unconsumed numerics/metadata");
        i.models[0].flags03=0;i.damage7D0[1]=0;i.timers820[0]=.1f;i.nameCount=1;i.damage00Matches=&match;
    }
    i.models[0].effect800={1,0};std::memset(output,0x74,sizeof output);const auto oldEvents=bytes(output);const auto oldNode=bytes(node);const auto oldRng=rng;n=123;
    check(call({&node,1},i,rng,w,output,41,n)==ImpactEffectsStatus50B::binding&&bytes(output)==oldEvents&&bytes(node)==oldNode&&rng==oldRng&&n==123,"effect alias rejected atomically");
    i.models[0].effect800={2,0};i.damage7D0[1]=.8f;i.timers820[0]=.1f;
    std::uint32_t nan=0x7f800001;std::memcpy(&i.timers820[4],&nan,4);i.effectD40=3;
    check(call({&node,1},i,rng,w,output,41,n)==ImpactEffectsStatus50B::arithmetic&&bytes(output)==oldEvents&&bytes(node)==oldNode&&rng==oldRng&&n==123,"late arithmetic preserves staged events/graph");
    check(allocs==0,"no hot allocation");std::printf("50B effects checks=%u failures=%u hot_allocations=%u fixtures=%u\n",checks,fails,allocs,rows);return fails?1:0;
}

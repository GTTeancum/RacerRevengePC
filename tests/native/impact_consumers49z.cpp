#include "../../src/game/impact_consumers49z.h"
#include <array>
#include <cfenv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <new>

using namespace rr::game;
static bool hot = false;
static unsigned allocations = 0, checks = 0, failures = 0;
void* operator new(std::size_t n) { if (hot) ++allocations; if (auto p = std::malloc(n ? n : 1)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
static void check(bool value, const char* text, unsigned row = 0) {
    ++checks; if (!value) { if (failures < 20) std::printf("FAIL row=%u %s\n", row, text); ++failures; }
}
template<class T> static auto bytes(const T& t) { std::array<unsigned char, sizeof(T)> b{}; std::memcpy(b.data(), &t, sizeof t); return b; }
static void put(float& f, std::uint32_t w) { std::memcpy(&f, &w, 4); }
static std::uint32_t word(const float& f) { std::uint32_t w; std::memcpy(&w, &f, 4); return w; }
static ImpactConsumerStatus49Z call(const ImpactConsumerInput49Z& i, const ImpactConsumerState49Z& s,
    ImpactConsumerState49Z& o, ImpactConsumerEvent49Z* e, std::size_t cap, std::size_t& count) {
    hot = true; auto r = consumeImpacts49Z(i,s,o,e,cap,count); hot = false; return r;
}
int main(int argc, char** argv) {
    static_assert(sizeof(ImpactConsumerInput49Z) == 14*4);
    static_assert(sizeof(ImpactConsumerState49Z) == 29*4);
    static_assert(sizeof(ImpactConsumerEvent49Z) == 6*4);
    if (argc != 2) return 2;
    check(std::fesetround(FE_TONEAREST) == 0, "rounding");
    std::ifstream file(argv[1], std::ios::binary); std::uint32_t countRows=0,width=0;
    file.read(reinterpret_cast<char*>(&countRows),4); file.read(reinterpret_cast<char*>(&width),4);
    if (!file || width != 86 || countRows != 4096) return 3;
    for (unsigned row=0; row<countRows; ++row) {
        std::array<std::uint32_t,86> r{}; file.read(reinterpret_cast<char*>(r.data()),sizeof r);
        if (!file) return 4;
        ImpactConsumerInput49Z i; ImpactConsumerState49Z s,o;
        std::memcpy(&i,r.data(),sizeof i); std::memcpy(&s,r.data()+14,sizeof s);
        const auto ib=bytes(i); const auto sb=bytes(s);
        std::memset(&o,0xa5,sizeof o); const auto ob=bytes(o);
        ImpactConsumerEvent49Z e[3]; std::memset(e,0x5a,sizeof e); const auto eb=bytes(e);
        std::size_t n=123;
        const auto status=call(i,s,o,e,2,n);
        check(static_cast<unsigned>(status)==r[43],"status",row);
        if (r[43]==2) { check(bytes(o)==ob && bytes(e)==eb && n==123,"atomic partner reject",row); }
        else {
            if (std::memcmp(&o,r.data()+44,sizeof o) && failures<5) {
                std::array<std::uint32_t,29> ow{};std::memcpy(ow.data(),&o,sizeof o);
                for (unsigned j=0;j<29;++j) if(ow[j]!=r[44+j]) std::printf("word %u native=%08x original=%08x\n",j,ow[j],r[44+j]);
            }
            check(std::memcmp(&o,r.data()+44,sizeof o)==0,"state words",row);
            check(n==r[73],"event count",row);
            check(n<=2 && std::memcmp(e,r.data()+74,n*sizeof(e[0]))==0,"ordered event words",row);
            check(std::memcmp(&e[2],eb.data()+2*sizeof(e[0]),sizeof(e[0]))==0,"event bound",row);
            auto alias=s; ImpactConsumerEvent49Z ae[2]; std::size_t an=123;
            check(call(i,alias,alias,ae,2,an)==status && bytes(alias)==bytes(o) && an==n,"exact state alias",row);
            if (n) {
                auto rejected=o; const auto rb=bytes(rejected); std::size_t rn=456;
                check(call(i,s,rejected,e,1,rn)==ImpactConsumerStatus49Z::capacity && bytes(rejected)==rb && rn==456,"capacity rejection",row);
            }
        }
        check(bytes(i)==ib && bytes(s)==sb,"immutable descriptors byte snapshot",row);
    }
    ImpactConsumerInput49Z i{}; i.managerFlags58=2; i.componentPresence=15; i.profile34=2; i.factor=1.0f/60.0f;
    i.finalDamageThreshold64=1; ImpactConsumerState49Z s{}; s.partnerId=0xffffffffu;
    ImpactConsumerEvent49Z e[2]; std::size_t n=7; ImpactConsumerState49Z o{};
    for (int rounding : {FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
        check(std::fesetround(rounding)==0,"set nonnearest");
        std::memset(&o,0x91,sizeof o);std::memset(e,0x92,sizeof e);n=789;
        const auto ob=bytes(o);const auto eb=bytes(e);
        check(call(i,s,o,e,2,n)==ImpactConsumerStatus49Z::arithmetic && bytes(o)==ob && bytes(e)==eb && n==789,"nonnearest atomic reject");
        auto gate=i;gate.managerFlags58=0;
        check(call(gate,s,o,nullptr,0,n)==ImpactConsumerStatus49Z::skipped && bytes(o)==bytes(s) && n==0,"nonnearest legitimate gate");
    }
    check(std::fesetround(FE_TONEAREST)==0,"restore nearest");
    // Manager gate and CD8 early return must preserve arbitrary unconsumed data.
    const std::uint32_t exceptional[]={0x7f800001,0xff800001,0x7fc12345,0xffc54321,0x7f800000,0xff800000,1,0x80000001,0x007fffff,0x807fffff,0,0x80000000};
    for (auto w:exceptional) {
        for (unsigned gate=0; gate<2; ++gate) {
            auto gi=i; gi.managerFlags58=gate ? 2u:0u; gi.flagsCD8=gate;
            for (std::size_t p=0;p<sizeof s;p+=4) std::memcpy(reinterpret_cast<unsigned char*>(&s)+p,&w,4);
            put(gi.factor,w); const auto before=bytes(s);
            check(word(gi.factor)==w,"raw exceptional precondition");
            check(call(gi,s,o,nullptr,0,n)==ImpactConsumerStatus49Z::skipped && bytes(o)==before && n==0,"raw skip copy");
        }
    }
    s={};s.partnerId=0xffffffffu; s.cooldown7E0=0.5f; // Do not consume/clear first six maxima.
    for (auto w:exceptional) {
        put(s.directionalD08[2],w);put(s.environmentalD38,w);const auto before=bytes(s);
        check(word(s.directionalD08[2])==w && word(s.environmentalD38)==w,"copy-only raw precall");
        check(call(i,s,o,e,2,n)==ImpactConsumerStatus49Z::applied && word(o.directionalD08[2])==w && word(o.environmentalD38)==w && bytes(s)==before,"copy-only exceptional payload");
    }
    for (auto w:exceptional) if ((w & 0x7fffffffu)!=0) {
        s={};s.partnerId=0xffffffffu;put(s.timers820[0],w);std::memset(&o,0x79,sizeof o);std::memset(e,0x89,sizeof e);n=987;
        const auto ob=bytes(o); const auto eb=bytes(e);
        check(call(i,s,o,e,2,n)==ImpactConsumerStatus49Z::arithmetic && bytes(o)==ob && bytes(e)==eb && n==987,"consumed exceptional atomic reject");
    }
    s={};s.partnerId=0xffffffffu;s.timers820[0]=1.0f;
    auto boundary=i;put(boundary.factor,0x3f7fffff);
    check(call(boundary,s,o,e,2,n)==ImpactConsumerStatus49Z::applied && word(o.timers820[0])==0x33800000,"ordinary cancellation boundary");
    put(s.timers820[0],0x00800001);put(boundary.factor,0x00800000);
    check(call(boundary,s,o,e,2,n)==ImpactConsumerStatus49Z::arithmetic,"subnormal result reject");
    s={};s.partnerId=0xffffffffu;put(s.directionalD08[6],0x7f7fffff);put(s.directionalD08[7],0x7f7fffff);
    check(call(i,s,o,e,2,n)==ImpactConsumerStatus49Z::arithmetic,"overflow result reject");
    check(allocations==0,"no hot allocations");
    std::printf("49Z checks=%u failures=%u hot_allocations=%u fixtures=%u\n",checks,failures,allocations,countRows);
    return failures ? 1:0;
}

#include "../../src/game/control_shaping48v.h"
#include "../../tools/gameplay/input_shaping_oracle48v.h"
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>

static std::uint64_t allocations=0;
void* operator new(std::size_t n) {++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n) {return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::game;
using original48v::bits;
using original48v::fp;

int main() {
    std::uint64_t checks=0,cases=0,instructions=0,cosineCalls=0;
    std::array<std::uint64_t,249> visits{};
    auto require=[&](bool ok,const char* label){++checks;if(!ok)throw std::runtime_error(label);};
    auto same=[&](const ControlInputStep48V& a,const ControlInputStep48V& b){
        for(unsigned i=0;i<4;++i) {require(bits(a.shaping.targets.values[i])==bits(b.shaping.targets.values[i]),"quantized ELF differential");require(bits(a.filtered.values[i])==bits(b.filtered.values[i]),"composed ELF differential");}
        require(bits(a.shaping.smoothedChannel0D28)==bits(b.shaping.smoothedChannel0D28),"D28 ELF differential");
    };
    auto compare=[&](const ControlChannels48B& raw,float history,std::uint32_t mode,
                     const ControlChannels48B& prior,ControlRateLimits48B rates,float elapsed,unsigned flags=0){
        original48v::Machine machine;
        const auto expected=machine.run(raw,history,mode,prior,rates,elapsed,flags);
        const auto shapeMode=mode?ControlShapeMode48V::BypassNonlinear:ControlShapeMode48V::Nonlinear;
        ControlInputStep48V got{};
        require(stepControlInputs48V(raw,history,shapeMode,prior,rates,elapsed,got),"valid input rejected");
        same(got,expected);
        ControlShapingResult48V shape{};
        require(shapeControlInputs48V(raw,history,shapeMode,shape),"shape-only rejected");
        require(std::memcmp(&shape,&got.shaping,sizeof shape)==0,"shape-only composition mismatch");
        require(machine.cosineCalls==(mode?0u:4u),"original mode branch");
        require(machine.reflections==0,"bounded cosine path escaped");
        require(machine.globals[0]==((flags&1)?0x3f123456u:0x3f000000u),"original lazy first initialization");
        require(machine.globals[1]==((flags&2)?0xbe987654u:0x3e800000u),"original lazy second initialization");
        ++cases;instructions+=machine.steps;cosineCalls+=machine.cosineCalls;
        for(unsigned i=0;i<visits.size();++i)visits[i]+=machine.visits[i];
        return got;
    };
    std::uint32_t random;
    random=0x48f217deu;
    auto next=[&](){random^=random<<13;random^=random>>17;random^=random<<5;return random;};
    try {
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}) {
            require(std::fesetround(rounding)==0,"rounding mode setup");
            for(std::uint32_t mode:{0u,1u,0xffffffffu}) {
                for(unsigned n=0;n<3000;++n) {
                    ControlChannels48B raw{},prior{};
                    for(unsigned i=0;i<4;++i) {
                        raw.values[i]=float(int(next()%65537)-32768)/32768.0f;
                        prior.values[i]=float(int(next()%65537)-32768)/16384.0f;
                    }
                    compare(raw,float(int(next()%65537)-32768)/4096.0f,mode,prior,
                            {float(next()%1024)/64.0f,float(next()%1024)/64.0f},float(next()%1024)/4096.0f,n&3);
                }
                // Conversion thresholds and their immediately adjacent binary32 values.
                for(int k=-100;k<100;++k) {
                    volatile float threshold=(float(k)+0.5f)/100.0f;
                    for(float value:{std::nextafter(threshold,-1.0f),float(threshold),std::nextafter(threshold,1.0f)})
                        compare({{value,-value,value,-value}},0.25f,mode,{}, {4,3},0.01f);
                }
                for(float value:{-1.0f,std::nextafter(-1.0f,0.0f),-0.5f,-0.125f,-0.0f,0.0f,
                                 std::numeric_limits<float>::denorm_min(),-std::numeric_limits<float>::denorm_min(),
                                 std::numeric_limits<float>::min(),-std::numeric_limits<float>::min(),
                                 0.125f,0.5f,std::nextafter(1.0f,0.0f),1.0f})
                    compare({{value,-value,-value,value}},-0.0f,mode,{{-0.0f,0.0f,1.0f,-1.0f}},{0.0f,-0.0f},-0.0f);
                ControlInputStep48V state{};
                for(unsigned n=0;n<512;++n) {
                    const float x=(n%64<32)?1.0f:-1.0f;
                    state=compare({{x,-x,x/2,-x/2}},state.shaping.smoothedChannel0D28,mode,state.filtered,{4,3},1.0f/120.0f);
                }
            }
        }
        require(std::fesetround(FE_TONEAREST)==0,"restore rounding");
        const auto golden=compare({{-1,1,-0.5f,0.5f}},0,0,{}, {100,100},1);
        require(bits(golden.shaping.targets.values[0])==0xbf7d70a3u,"negative endpoint golden (-0.99)");
        require(bits(golden.shaping.targets.values[1])==0x3f800000u,"positive endpoint golden (+1)");
        require(bits(golden.shaping.targets.values[2])==0xbe8f5c29u,"negative half golden (-0.28)");
        require(bits(golden.shaping.targets.values[3])==0x3e947ae1u,"positive half golden (+0.29)");
        const auto bypass=compare({{-1,1,-0.5f,0.5f}},0,1,{}, {100,100},1);
        require(bits(bypass.shaping.targets.values[2])==0xbefae147u,"bypass negative half golden (-0.49)");
        require(bits(golden.shaping.smoothedChannel0D28)==0xbdccccd3u,"pre-quantization D28 golden");
        // Transactional failure and every input/output alias permitted by the typed API.
        const ControlInputStep48V sentry{{{{12,13,14,15}},16},{{17,18,19,20}}};
        for(float bad:{std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),
                       std::numeric_limits<float>::quiet_NaN(),1.01f,-1.01f}) {
            for(unsigned channel=0;channel<4;++channel) {
                ControlChannels48B raw{};raw.values[channel]=bad;auto out=sentry;
                require(!stepControlInputs48V(raw,0,ControlShapeMode48V::Nonlinear,{}, {4,3},.01f,out),"bad raw accepted");
                require(std::memcmp(&out,&sentry,sizeof out)==0,"bad raw mutated output");
            }
        }
        for(float bad:{std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
            auto out=sentry;
            require(!stepControlInputs48V({},bad,ControlShapeMode48V::Nonlinear,{}, {4,3},.01f,out),"bad history accepted");
            require(std::memcmp(&out,&sentry,sizeof out)==0,"bad history mutated output");
        }
        for(float bad:{-1.0f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
            auto out=sentry;
            require(!stepControlInputs48V({},0,ControlShapeMode48V::Nonlinear,{}, {4,3},bad,out),"bad dt accepted");
            require(std::memcmp(&out,&sentry,sizeof out)==0,"late limiter failure mutated output");
            require(!stepControlInputs48V({},0,ControlShapeMode48V::Nonlinear,{}, {4,bad},.01f,out),"bad y rate accepted");
            require(std::memcmp(&out,&sentry,sizeof out)==0,"late rate failure mutated output");
        }
        auto out=sentry;
        require(!stepControlInputs48V({},0,static_cast<ControlShapeMode48V>(42),{}, {4,3},1,out),"invalid mode accepted");
        require(std::memcmp(&out,&sentry,sizeof out)==0,"invalid mode mutation");
        require(!stepControlInputs48V({},0,ControlShapeMode48V::Nonlinear,{}, {std::numeric_limits<float>::max(),3},2,out),"overflow limit accepted");
        require(std::memcmp(&out,&sentry,sizeof out)==0,"overflow mutation");
        ControlInputStep48V alias{{{{-0.75f,0.25f,-0.5f,1.0f}},.35f},{{.1f,-.2f,.3f,-.4f}}};
        const auto expected=compare(alias.shaping.targets,alias.shaping.smoothedChannel0D28,0,alias.filtered,{4,3},.01f);
        require(stepControlInputs48V(alias.shaping.targets,alias.shaping.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,alias.filtered,{4,3},.01f,alias),"alias rejected");
        same(alias,expected);
        auto shape=ControlShapingResult48V{{{-.75f,.25f,-.5f,1}},.35f};
        require(shapeControlInputs48V(shape.targets,shape.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,shape),"shape alias rejected");
        require(std::memcmp(&shape,&expected.shaping,sizeof shape)==0,"shape alias mismatch");
        for(unsigned rawMember=0;rawMember<2;++rawMember) for(unsigned priorMember=0;priorMember<2;++priorMember) {
            ControlInputStep48V both{{{{-.75f,.25f,-.5f,1}},.35f},{{.1f,-.2f,.3f,-.4f}}};
            const auto& aliasedRaw=rawMember?both.filtered:both.shaping.targets;
            const auto& aliasedPrior=priorMember?both.filtered:both.shaping.targets;
            const auto reference=compare(aliasedRaw,both.shaping.smoothedChannel0D28,0,aliasedPrior,{4,3},.01f);
            require(stepControlInputs48V(aliasedRaw,both.shaping.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,aliasedPrior,{4,3},.01f,both),"cross-member alias rejected");
            same(both,reference);
            const auto saved=both;
            require(!stepControlInputs48V(aliasedRaw,both.shaping.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,aliasedPrior,{4,-1},.01f,both),"alias late failure accepted");
            require(std::memcmp(&both,&saved,sizeof both)==0,"alias failure mutated inputs/output");
        }
        for(unsigned channel=0;channel<4;++channel) {
            ControlChannels48B prior{};prior.values[channel]=std::numeric_limits<float>::quiet_NaN();
            auto badOut=sentry;
            require(!stepControlInputs48V({},0,ControlShapeMode48V::Nonlinear,prior,{4,3},.01f,badOut),"bad prior accepted");
            require(std::memcmp(&badOut,&sentry,sizeof badOut)==0,"bad prior mutated output");
        }
        auto badShape=ControlShapingResult48V{{{.1f,.2f,.3f,1.01f}},.25f};
        const auto savedShape=badShape;
        require(!shapeControlInputs48V(badShape.targets,badShape.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,badShape),"shape alias bad input accepted");
        require(std::memcmp(&badShape,&savedShape,sizeof badShape)==0,"shape alias failure mutation");
        const auto before=allocations;
        for(unsigned n=0;n<20000;++n) require(stepControlInputs48V({{.75f,-.5f,.25f,-1}},alias.shaping.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,alias.filtered,{4,3},.01f,alias),"hot step failure");
        require(allocations==before,"hot allocation");
        // Every instruction in the bounded caller is reached across fixtures.
        for(auto count:visits) require(count!=0,"uncovered original caller instruction");
        std::cout<<"control_shaping48v: "<<checks<<" checks; "<<cases<<" original-word cases; "<<instructions<<" original instructions; "<<cosineCalls<<" cosine calls; all 249 caller words visited; four rounding modes; zero hot allocations\n";
        return 0;
    } catch(const std::exception& error) {
        std::fesetround(FE_TONEAREST);
        std::cerr<<"control_shaping48v after "<<checks<<" checks / "<<cases<<" cases: "<<error.what()<<'\n';return 1;
    }
}

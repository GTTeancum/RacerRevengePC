#include "../../src/game/player_input48x.h"
#include "../../tools/gameplay/player_input_oracle48x.h"
#include "../../tools/gameplay/control_binding_oracle48w.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>

static std::uint64_t allocations=0;
void* operator new(std::size_t n){++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::game;
using original48w::bits;
using original48w::fp;
struct Provider {
    std::uint32_t salt=0;unsigned count=0;int failAt=-1;bool fullTrace=false;
    std::array<std::int32_t,8> codes{};std::array<std::uint32_t,8> masks{};
    bool answer(std::int32_t code,std::uint32_t mask) noexcept {
        const auto n=count++;
        if(n<codes.size()){codes[n]=code;masks[n]=mask;}
        if(fullTrace)return n>=2;
        const auto x=static_cast<std::uint32_t>(code)^salt^(n*0x45d9f3bu);
        return ((x^(x>>11)^(x>>19))&1)!=0;
    }
    static PlayerActionResult48X query(void* opaque,std::int32_t code,std::uint32_t mask) noexcept {
        auto& p=*static_cast<Provider*>(opaque);const unsigned n=p.count;
        const bool match=p.answer(code,mask);
        if(static_cast<int>(n)==p.failAt)return PlayerActionResult48X::Unavailable;
        return match?PlayerActionResult48X::Match:PlayerActionResult48X::NoMatch;
    }
};
int main(){
    std::uint64_t checks=0,axisCases=0,readerCases=0,connectedCases=0,originalInstructions=0,reflections=0;
    std::array<std::uint64_t,37> axisCoverage{};std::array<std::uint64_t,235> readerCoverage{};std::array<std::uint64_t,12> gateCoverage{};
    auto require=[&](bool ok,const char* label){++checks;if(!ok)throw std::runtime_error(label);};
    auto record=[&](const original48x::Stats& s){originalInstructions+=s.steps;for(unsigned i=0;i<37;++i)axisCoverage[i]+=s.axisVisits[i];for(unsigned i=0;i<235;++i)readerCoverage[i]+=s.readerVisits[i];for(unsigned i=0;i<12;++i)gateCoverage[i]+=s.gateVisits[i];};
    auto axis=[&](float numerator,float denominator,const InputAxisRange48X& p){
        original48x::Stats stats{};
        const float expected=original48x::axis(numerator,denominator,{p.minimum,p.maximum,p.center,p.upperDeadband,p.lowerDeadband},&stats);
        float got=99;require(conditionInputAxis48X(numerator,denominator,p,got),"valid axis rejected");require(bits(got)==bits(expected),"axis original-word differential");
        ++axisCases;record(stats);return expected;
    };
    auto reader=[&](const PlayerReadContext48X& context,const PlayerActionCodes48X& codes,std::uint32_t salt){
        original48x::ReaderInput input{};
        for(unsigned i=0;i<4;++i)input.components[i]=context.componentValues.values[i];
        input.codes={codes.action694,codes.action690,codes.flag698,codes.flag69C,codes.flag6B4};
        input.gateByte=context.gateByte58;input.previousFlags=context.previousFlagsB9C;
        input.source7D0=context.source7D0;input.source7D4=context.source7D4;input.source7D8=context.source7D8;input.source7DC=context.source7DC;input.deviceIndex=static_cast<int>(readerCases%4);
        Provider reference{};reference.salt=salt;original48x::Stats stats{};
        const auto expected=original48x::reader(input,[&](std::int32_t code,std::uint32_t mask){return reference.answer(code,mask);},&stats);
        Provider native{};native.salt=salt;PlayerReadResult48X got{};
        require(readPlayerInput48X(context,codes,{&native,Provider::query},got),"valid reader rejected");
        for(unsigned i=0;i<4;++i)require(bits(got.raw.values[i])==bits(expected.raw[i]),"reader channel differential");
        require(bits(got.actionB7C)==bits(expected.actionB7C)&&bits(got.actionB80)==bits(expected.actionB80),"reader action differential");
        require(got.flagsB9C==expected.flags,"reader flag differential");
        require(native.count==expected.queryCount&&native.count==reference.count,"query count/order mismatch");
        for(unsigned i=0;i<native.count;++i)require(native.codes[i]==expected.trace[i].code&&native.masks[i]==expected.trace[i].mask&&native.masks[i]==0x3fu,"query protocol mismatch");
        ++readerCases;record(stats);return got;
    };
    const InputAxisRange48X authored{-1,1,0,fp(0x3e4ccccdu),fp(0xbe4ccccdu)};
    const ControlBindingParameters48W bindingParameters{0,1,200,.25f,.5f,.25f};
    const ControlModelBindings48W oldBindings{{.2f,.3f},{.4f,.5f},.6f,1,.7f,.8f,.9f};
    std::uint32_t random=0x48f127d1u;
    auto next=[&](){random^=random<<13;random^=random>>17;random^=random<<5;return random;};
    try {
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}) {
            require(std::fesetround(rounding)==0,"rounding setup");
            for(unsigned byte=0;byte<256;++byte) {
                const float value=axis(float(byte),255,authored);
                require(value>=-fp(0x3f800001u)&&value<=fp(0x3f800001u),"authored endpoint envelope");
                const DualShockAxisBytes48X sample{static_cast<std::uint8_t>(byte),static_cast<std::uint8_t>(255-byte),static_cast<std::uint8_t>(byte),static_cast<std::uint8_t>(255-byte)};
                ControlChannels48B conditioned{};require(conditionDualShockPlayerAxes48X(sample,conditioned),"packet conditioning failure");
                const float expected[4]={value,axis(float(255-byte),255,authored),axis(float(255-byte),255,authored),axis(float(255-byte),255,authored)};
                for(unsigned i=0;i<4;++i)require(bits(conditioned.values[i])==bits(expected[i]),"packet offset/inversion mismatch");
            }
            for(float numerator:{.25f,.5f,.75f})axis(numerator,1,{-1,1,0,.5f,-.5f});
            // Distinct component values isolate packet4/5/6/7 mapping.
            ControlChannels48B isolated{};require(conditionDualShockPlayerAxes48X({0,64,192,255},isolated),"isolated packet failure");
            const float isolatedExpected[4]={axis(192,255,authored),axis(255,255,authored),axis(255,255,authored),axis(64,255,authored)};
            for(unsigned i=0;i<4;++i)require(bits(isolated.values[i])==bits(isolatedExpected[i]),"isolated packet mapping");
            for(unsigned n=0;n<4000;++n) {
                const float center=float(int(next()%8193)-4096)/1024;
                const float low=center-.25f-float(next()%2048)/1024;
                const float high=center+.25f+float(next()%2048)/1024;
                const InputAxisRange48X p{low-1-float(next()%4096)/1024,high+1+float(next()%4096)/1024,center,high,low};
                axis(float(int(next()%2049)-512),1024,p); // Outside-range values prove no invented clamp.
                PlayerReadContext48X context{{{float(int(next()%513)-256)/256,float(int(next()%513)-256)/256,0,1}},
                    float(int(next()%3)-1),float(int(next()%3)-1),float(int(next()%3)-1),float(int(next()%3)-1),
                    static_cast<std::uint8_t>(next()),static_cast<std::uint8_t>(next())};
                PlayerActionCodes48X codes{n%3?int(next()%20):-1,n%5?int(next()%20):-1,int(next()%24)-3,int(next()%24)-3,int(next()%24)-3};
                const auto salt=next();reader(context,codes,salt);
                const DualShockAxisBytes48X sample{static_cast<std::uint8_t>(next()),static_cast<std::uint8_t>(next()),static_cast<std::uint8_t>(next()),static_cast<std::uint8_t>(next())};
                const float nums[4]={float(sample.packet6),float(sample.packet7),float(255-sample.packet4),float(sample.packet5)};
                for(unsigned i=0;i<4;++i)context.componentValues.values[i]=axis(nums[i],255,authored);
                const auto expectedReader=reader(context,codes,salt);
                ControlBindingInput48W binding{{},float(next()%300),next()%7,(n&1)!=0,(n&2)!=0,99,99,0,99,99};
                auto originalBinding=binding;originalBinding.actionB7C=expectedReader.actionB7C;originalBinding.actionB80=expectedReader.actionB80;originalBinding.flagsB9C=expectedReader.flagsB9C;
                originalBinding.source7D0=context.source7D0;originalBinding.source7D4=context.source7D4;
                original48w::Machine machine;machine.prepareBinding(originalBinding,bindingParameters,oldBindings,n%17);
                const auto expectedControl=machine.run(expectedReader.raw,.375f,binding.mode58Nonzero?1u:0u,{{.1f,-.2f,.3f,-.4f}},{4,3},.01f,3);
                const auto expectedBindings=machine.continueBinding();
                originalInstructions+=machine.steps;reflections+=machine.reflections;
                Provider native{};native.salt=salt;PlayerControlStep48X got{};
                require(stepDualShockPlayerControls48X(sample,context,codes,{&native,Provider::query},.375f,{{.1f,-.2f,.3f,-.4f}},{4,3},.01f,binding,bindingParameters,oldBindings,got),"connected sample rejected");
                for(unsigned i=0;i<4;++i)require(bits(got.components.values[i])==bits(context.componentValues.values[i]),"connected components differential");
                require(std::memcmp(&got.bound.control,&expectedControl,sizeof expectedControl)==0,"connected shape/filter differential");
                require(std::memcmp(&got.bound.bindings,&expectedBindings,sizeof expectedBindings)==0,"connected binding differential");
                require(got.player.flagsB9C==expectedReader.flagsB9C,"connected player flag mismatch");
                ++connectedCases;
            }
            // Exhaust all state-gate bytes and preservation of unrelated prior flags.
            for(unsigned gate=0;gate<256;++gate)for(unsigned positive=0;positive<5;++positive) {
                PlayerReadContext48X context{{{-.25f,.5f,-.75f,1}},positive==1?1.f:0.f,positive==2?1.f:0.f,positive==3?1.f:0.f,positive==4?1.f:0.f,static_cast<std::uint8_t>(gate),0xf7};
                reader(context,{15,13,-1,-1,-1},0x1234u+positive);
            }
            // Direct endpoint reflection proof, including both raw signs.
            for(unsigned signs=0;signs<16;++signs)for(auto mode:{ControlShapeMode48V::Nonlinear,ControlShapeMode48V::BypassNonlinear}) for(float history:{0.0f,.375f}) {
                ControlChannels48B raw{};for(unsigned i=0;i<4;++i)raw.values[i]=fp(0x3f800001u|((signs&(1u<<i))?0x80000000u:0u));
                original48w::Machine machine;const auto expected=machine.run(raw,history,mode==ControlShapeMode48V::Nonlinear?0u:1u,{}, {4,3},.01f,3);
                ControlShapingResult48V got{};require(shapePlayerControlChannels48X(raw,history,mode,got),"authored endpoint shaping rejected");
                require(std::memcmp(&got,&expected.shaping,sizeof got)==0,"original reflected cosine differential");
                originalInstructions+=machine.steps;reflections+=machine.reflections;
            }
        }
        require(std::fesetround(FE_TONEAREST)==0,"restore rounding");
        require(reflections>0,"no original cosine reflection exercised");
        PlayerReadContext48X context{{{-.25f,.5f,-.75f,1}},1,1,1,1,2,0xff};
        const PlayerActionCodes48X codes{2,3,4,5,6};
        PlayerReadResult48X sentry{{{12,13,14,15}},16,17,0xaa};
        for(int failure=0;failure<7;++failure) {
            Provider p{};p.fullTrace=true;p.failAt=failure;auto out=sentry;
            require(!readPlayerInput48X(context,codes,{&p,Provider::query},out),"unavailable query accepted");
            require(std::memcmp(&out,&sentry,sizeof out)==0,"partial query failure mutation");
            require(p.count==static_cast<unsigned>(failure+1),"query did not stop at failure");
        }
        Provider p{};auto out=sentry;
        require(!readPlayerInput48X(context,codes,{&p,nullptr},out),"null provider accepted");
        require(std::memcmp(&out,&sentry,sizeof out)==0,"null provider mutation");
        for(float bad:{std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
            auto badContext=context;badContext.componentValues.values[3]=bad;p={};out=sentry;
            require(!readPlayerInput48X(badContext,codes,{&p,Provider::query},out),"nonfinite component accepted");
            require(p.count==0&&std::memcmp(&out,&sentry,sizeof out)==0,"invalid reader queried/mutated");
            float axisOut=17;require(!conditionInputAxis48X(bad,255,authored,axisOut)&&axisOut==17,"axis nonfinite mutation");
        }
        float axisOut=17;require(!conditionInputAxis48X(1,0,authored,axisOut)&&axisOut==17,"zero divisor accepted");
        auto badRange=authored;badRange.upperDeadband=1;
        require(!conditionInputAxis48X(1,255,badRange,axisOut)&&axisOut==17,"singular deadband accepted");
        ControlShapingResult48V shaping{{{12,13,14,15}},16};const auto savedShaping=shaping;
        require(!shapePlayerControlChannels48X({{fp(0x3f800002u),0,0,0}},0,ControlShapeMode48V::Nonlinear,shaping),"unproven endpoint range accepted");
        require(std::memcmp(&shaping,&savedShaping,sizeof shaping)==0,"endpoint rejection mutation");
        auto axisAlias=authored;
        const float aliasExpected=axis(200,255,authored);
        require(conditionInputAxis48X(200,255,axisAlias,axisAlias.minimum),"axis parameter/output alias rejected");
        require(bits(axisAlias.minimum)==bits(aliasExpected),"axis alias mismatch");
        axisOut=17;
        require(!conditionInputAxis48X(std::numeric_limits<float>::max(),std::numeric_limits<float>::min(),authored,axisOut)&&axisOut==17,"axis overflow mutated output");
        const auto invalidQuery=[](void*,std::int32_t,std::uint32_t) noexcept {return static_cast<PlayerActionResult48X>(99);};
        out=sentry;
        require(!readPlayerInput48X(context,codes,{nullptr,invalidQuery},out),"invalid callback result accepted");
        require(std::memcmp(&out,&sentry,sizeof out)==0,"invalid callback result mutation");
        ControlShapingResult48V reflectedAlias{{{fp(0x3f800001u),-fp(0x3f800001u),0,1}},.375f};
        original48w::Machine reflectedOracle;
        const auto reflectedExpected=reflectedOracle.run(reflectedAlias.targets,reflectedAlias.smoothedChannel0D28,0,{}, {4,3},.01f,3);
        require(shapePlayerControlChannels48X(reflectedAlias.targets,reflectedAlias.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,reflectedAlias),"reflected shape alias rejected");
        require(std::memcmp(&reflectedAlias,&reflectedExpected.shaping,sizeof reflectedAlias)==0,"reflected shape alias mismatch");
        const auto savedReflected=reflectedAlias;
        require(!shapePlayerControlChannels48X(reflectedAlias.targets,std::numeric_limits<float>::quiet_NaN(),ControlShapeMode48V::Nonlinear,reflectedAlias),"invalid reflected history accepted");
        require(std::memcmp(&reflectedAlias,&savedReflected,sizeof reflectedAlias)==0,"invalid reflected history mutation");
        ControlBindingInput48W binding{{},100,0,false,true,0,0,0,0,0};
        PlayerControlStep48X state{};p={};
        require(stepDualShockPlayerControls48X({0,64,192,255},context,codes,{&p,Provider::query},0,{}, {4,3},.01f,binding,bindingParameters,oldBindings,state),"initial alias state");
        PlayerControlStep48X unaliased{};Provider q{};p={};
        require(stepDualShockPlayerControls48X({0,64,192,255},context,codes,{&q,Provider::query},state.bound.control.shaping.smoothedChannel0D28,state.bound.control.filtered,{4,3},.01f,binding,bindingParameters,state.bound.bindings,unaliased),"alias expected failure");
        require(stepDualShockPlayerControls48X({0,64,192,255},context,codes,{&p,Provider::query},state.bound.control.shaping.smoothedChannel0D28,state.bound.control.filtered,{4,3},.01f,binding,bindingParameters,state.bound.bindings,state),"connected alias failure");
        require(std::memcmp(&state.bound,&unaliased.bound,sizeof state.bound)==0,"connected alias mismatch");
        const auto beforeState=state;auto parameters=bindingParameters;parameters.speed1B84=std::numeric_limits<float>::infinity();p={};
        require(!stepDualShockPlayerControls48X({0,64,192,255},context,codes,{&p,Provider::query},state.bound.control.shaping.smoothedChannel0D28,state.bound.control.filtered,{4,3},.01f,binding,parameters,state.bound.bindings,state),"late binding failure accepted");
        require(std::memcmp(&state,&beforeState,sizeof state)==0,"late binding mutation");
        const auto before=allocations;
        for(unsigned n=0;n<20000;++n){p={};p.salt=n;require(stepDualShockPlayerControls48X({0,64,192,255},context,codes,{&p,Provider::query},state.bound.control.shaping.smoothedChannel0D28,state.bound.control.filtered,{4,3},.01f,binding,bindingParameters,state.bound.bindings,state),"hot player step failure");}
        require(allocations==before,"hot player allocation");
        unsigned axisVisited=0,readerVisited=0,gateVisited=0;
        for(auto x:axisCoverage)axisVisited+=x!=0;
        for(auto x:readerCoverage)readerVisited+=x!=0;
        for(auto x:gateCoverage)gateVisited+=x!=0;
        require(axisVisited==37&&readerVisited==235&&gateVisited==12,"original block coverage gap");
        std::cout<<"player_input48x: "<<checks<<" checks; "<<axisCases<<" axis cases; "<<readerCases<<" reader cases; "<<connectedCases<<" connected; "<<originalInstructions<<" instructions; "<<reflections<<" cosine reflections; original words "<<axisVisited<<'/'<<readerVisited<<'/'<<gateVisited<<"; zero hot allocations; result "<<sizeof(PlayerControlStep48X)<<" bytes\n";
        return 0;
    }catch(const std::exception& e){std::fesetround(FE_TONEAREST);std::cerr<<"player_input48x after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}
}

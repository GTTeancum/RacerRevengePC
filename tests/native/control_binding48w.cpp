#include "../../src/game/control_binding48w.h"
#include "../../tools/gameplay/control_binding_oracle48w.h"
#include <array>
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
using original48w::bits;
using original48w::fp;

int main() {
    std::uint64_t checks=0,cases=0,connectedCases=0,instructions=0;
    std::array<std::uint64_t,437> visits{};
    auto require=[&](bool ok,const char* label){++checks;if(!ok)throw std::runtime_error(label);};
    auto same=[&](const ControlModelBindings48W& a,const ControlModelBindings48W& b){
        for(unsigned i=0;i<2;++i) {require(bits(a.engineDrive[i])==bits(b.engineDrive[i]),"engine mapping differential");require(bits(a.aeroDrive[i])==bits(b.aeroDrive[i]),"aero mapping differential");}
        require(bits(a.aeroBrakeEC)==bits(b.aeroBrakeEC),"brake differential");
        require(a.aeroTriggerD8==b.aeroTriggerD8,"exact trigger bit extraction");
        require(bits(a.aeroAux50)==bits(b.aeroAux50),"aux50 preservation/write");
        require(bits(a.aeroAuxF0)==bits(b.aeroAuxF0),"auxF0 copy");
        require(bits(a.aeroAuxF4)==bits(b.aeroAuxF4),"auxF4 copy");
    };
    auto compare=[&](const ControlBindingInput48W& in,const ControlBindingParameters48W& p,
                     const ControlModelBindings48W& previous,bool connected=false){
        original48w::Machine original;
        original.prepareBinding(in,p,previous,static_cast<unsigned>(cases%17));
        ControlBoundStep48W expected{},got{};
        if(connected) {
            const ControlChannels48B raw=in.filtered;
            expected.control=original.run(raw,0.375f,in.mode58Nonzero?7u:0u,{{.1f,-.2f,.3f,-.4f}},{4,3},.01f,3);
            expected.bindings=original.continueBinding();
            require(stepAndBindControlInputs48W(raw,.375f,{{.1f,-.2f,.3f,-.4f}},{4,3},.01f,in,p,previous,got),"connected valid input rejected");
            require(std::memcmp(&got.control,&expected.control,sizeof got.control)==0,"connected input prefix differential");
            ++connectedCases;
        } else {
            expected.bindings=original.continueBinding();
            require(bindVehicleControls48W(in,p,previous,got.bindings),"valid binding rejected");
        }
        if(std::memcmp(&got.bindings,&expected.bindings,sizeof got.bindings)!=0) {
            std::cerr<<"case "<<cases<<" connected="<<connected<<" rounding="<<std::fegetround()<<" state="<<in.vehicleState40<<" mode="<<in.mode58Nonzero<<" config="<<in.configurationEntryNonzero<<" flags="<<unsigned(in.flagsB9C)<<'\n';
            for(float x:in.filtered.values)std::cerr<<" ch="<<std::hex<<bits(x)<<std::dec;
            for(float x:{in.bodyCachedSpeed34,in.actionB7C,in.actionB80,p.speed1B84,p.scale1B88,p.speed2B8C,p.scale2B90,p.scale4346B8,p.bias4346C0})std::cerr<<" value="<<std::hex<<bits(x)<<std::dec;
            for(unsigned i=0;i<2;++i)std::cerr<<" engine"<<i<<" got="<<std::hex<<bits(got.bindings.engineDrive[i])<<" expected="<<bits(expected.bindings.engineDrive[i])<<std::dec;
            for(unsigned i=0;i<2;++i)std::cerr<<" aero"<<i<<" got="<<std::hex<<bits(got.bindings.aeroDrive[i])<<" expected="<<bits(expected.bindings.aeroDrive[i])<<std::dec;
            std::cerr<<'\n';
        }
        same(got.bindings,expected.bindings);
        ++cases;instructions+=original.steps;
        for(unsigned i=0;i<visits.size();++i)visits[i]+=original.bindingVisits[i];
        return got.bindings;
    };
    std::uint32_t random=0x48f2181cu;
    auto next=[&](){random^=random<<13;random^=random>>17;random^=random<<5;return random;};
    auto number=[&](float divisor){return float(int(next()%65537)-32768)/divisor;};
    const ControlModelBindings48W prior{{.2f,.3f},{.4f,.5f},.6f,7,.7f,.8f,.9f};
    const ControlBindingParameters48W nominal{0,1,200,.25f,.5f,.25f};
    try {
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}) {
            require(std::fesetround(rounding)==0,"rounding setup");
            // Equal speed endpoints leave scale at +zero. Negative channel
            // values then produce -zero steering. The inactive action route
            // uses literal integer zero stores, not a floating maximum that
            // may preserve the steering sign on equality (218824/218854).
            for(float channel:{-1.0f,-0.0f,0.0f,1.0f}) {
                ControlBindingInput48W zeroSteering{{{channel,0,0,0}},0,0,true,false,0,0,0,0,0};
                const auto bound=compare(zeroSteering,{0,1,0,1,.5f,.25f},prior);
                require(bits(bound.aeroDrive[0])==0u&&bits(bound.aeroDrive[1])==0u,
                    "inactive actions store positive zero for either steering zero sign");
            }
            for(unsigned n=0;n<12000;++n) {
                ControlBindingInput48W in{};
                for(float& value:in.filtered.values)value=number(32768);
                in.bodyCachedSpeed34=number(32);in.vehicleState40=next()%7;
                in.mode58Nonzero=next()&1;in.configurationEntryNonzero=next()&1;
                in.actionB7C=(next()&1)?number(4096):0;in.actionB80=(next()&1)?number(4096):0;
                in.flagsB9C=static_cast<std::uint8_t>(next());in.source7D0=number(4096);in.source7D4=number(4096);
                const float speed=number(128);
                ControlBindingParameters48W p{speed,number(16384),(n%17)?number(128):speed,number(16384),number(16384),number(16384)};
                compare(in,p,prior,n%4==0);
            }
            // Mode, action/state branches, saturation threshold neighbors and signed zero.
            for(unsigned state:{0u,1u,2u,3u,4u,0xffffffffu}) for(unsigned mode=0;mode<4;++mode)
                for(unsigned actions=0;actions<4;++actions) for(float value:{-1.0f,-0.0f,0.0f,.9f,std::nextafter(.9f,0.0f),std::nextafter(.9f,1.0f),1.0f}) {
                    ControlBindingInput48W in{{{value,value,-value,value}},100,state,(mode&1)!=0,(mode&2)!=0,
                        (actions&1)?-2.0f:-0.0f,(actions&2)?3.0f:0.0f,4,.125f,-.25f};
                    compare(in,nominal,prior);
                }
            for(float sign:{-1.0f,1.0f}) {
                ControlBindingInput48W saturated{{{sign,0,0,0}},50,0,true,false,1,0,0,0,0};
                const auto bound=compare(saturated,{0,2,100,2,.5f,.25f},prior);
                const unsigned reduced=sign<0?0:1;
                require(bits(bound.engineDrive[reduced])==(rounding==FE_DOWNWARD?0x80000000u:0u),"saturated drive cancellation rounding golden");
                require(bits(bound.engineDrive[1-reduced])==0x3f800000u,"saturated other engine golden");
            }
            // Persistent transitions deliberately retain EC/aux50 where not written.
            auto state=prior;
            for(unsigned n=0;n<512;++n) {
                ControlBindingInput48W in{{{n&1?.7f:-.7f,n&2?.95f:-.6f,0,n&4?.95f:-.6f}},100,n%5,(n%7)==0,(n%7)!=0,
                    float(n%2),float(n%3),static_cast<std::uint8_t>(n),.25f,-.5f};
                state=compare(in,nominal,state);
            }
        }
        require(std::fesetround(FE_TONEAREST)==0,"restore rounding");
        ControlBindingInput48W in{{{.5f,-.75f,0,.95f}},100,0,false,true,1,1,4,.125f,-.25f};
        const auto dual=compare(in,nominal,prior);
        require(bits(dual.engineDrive[0])==bits(.75f)&&bits(dual.engineDrive[1])==0,"dual negative channel drive golden");
        require(bits(dual.aeroDrive[0])==0&&bits(dual.aeroDrive[1])==bits(.95f),"dual positive channel aero golden");
        require(dual.aeroTriggerD8==1,"trigger golden is one");
        in.vehicleState40=3;
        const auto disabled=compare(in,nominal,prior);
        require(bits(disabled.aeroBrakeEC)==bits(prior.aeroBrakeEC)&&bits(disabled.aeroAux50)==bits(prior.aeroAux50),"disabled branch must preserve EC/aux50");
        in.vehicleState40=0;in.mode58Nonzero=true;
        auto expected=compare(in,nominal,prior);
        auto alias=prior;
        require(bindVehicleControls48W(in,nominal,alias,alias),"binding alias rejected");same(alias,expected);
        // Typed adapter: six proven fields change, every other producer input survives.
        EngineInput48D engine{{.11f,.22f},123,true,false,true};
        AeroInput48E aero{{{1,2,3}},{{4,5,6}},.75f,true,{{7,8,9}},true,false,true,.33f,.44f,.55f};
        const auto savedEngine=engine;const auto savedAero=aero;
        require(applyControlModelBindings48W(expected,engine,aero,engine,aero),"adapter alias rejected");
        for(unsigned i=0;i<2;++i)require(bits(engine.drive[i])==bits(expected.engineDrive[i]),"adapter engine field");
        require(bits(engine.bodySpeed)==bits(savedEngine.bodySpeed)&&engine.boost==savedEngine.boost&&engine.repair==savedEngine.repair&&engine.fullTorque==savedEngine.fullTorque,"adapter changed unrelated engine fields");
        require(bits(aero.driveE4)==bits(expected.aeroDrive[0])&&bits(aero.driveE8)==bits(expected.aeroDrive[1])&&bits(aero.brakeEc)==bits(expected.aeroBrakeEC)&&aero.triggerD8==(expected.aeroTriggerD8!=0),"adapter aero fields");
        for(unsigned i=0;i<3;++i) {
            require(bits(aero.bodyLocalVelocity.v[i])==bits(savedAero.bodyLocalVelocity.v[i]),"adapter local velocity preservation");
            require(bits(aero.angularVelocity.v[i])==bits(savedAero.angularVelocity.v[i]),"adapter angular preservation");
            require(bits(aero.worldFlow40.v[i])==bits(savedAero.worldFlow40.v[i]),"adapter flow preservation");
        }
        require(bits(aero.bodyAngleE0)==bits(savedAero.bodyAngleE0)&&aero.subtractWorldFlow==savedAero.subtractWorldFlow&&aero.triggerD0==savedAero.triggerD0&&aero.fullSteeringE0==savedAero.fullSteeringE0,"adapter unrelated aero preservation");
        const auto sentinel=prior;
        for(float bad:{std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
            for(unsigned field=0;field<16;++field) {
                auto badInput=in;auto parameters=nominal;auto previous=prior;auto out=sentinel;
                if(field<4)badInput.filtered.values[field]=bad;
                else if(field==4)badInput.bodyCachedSpeed34=bad;
                else if(field==5)badInput.actionB7C=bad;
                else if(field==6)badInput.actionB80=bad;
                else if(field==7)badInput.source7D0=bad;
                else if(field==8)badInput.source7D4=bad;
                else if(field==9)parameters.speed1B84=bad;
                else if(field==10)parameters.speed2B8C=bad;
                else if(field==11)parameters.scale1B88=bad;
                else if(field==12)parameters.scale2B90=bad;
                else if(field==13)parameters.scale4346B8=bad;
                else if(field==14)parameters.bias4346C0=bad;
                else previous.aeroAux50=bad;
                require(!bindVehicleControls48W(badInput,parameters,previous,out),"invalid binding accepted");
                require(std::memcmp(&out,&sentinel,sizeof out)==0,"invalid binding mutated output");
            }
        }
        auto overflow=nominal;overflow.scale2B90=std::numeric_limits<float>::max();overflow.scale1B88=-std::numeric_limits<float>::max();
        alias=prior;
        require(!bindVehicleControls48W(in,overflow,alias,alias),"overflow interpolation accepted");
        require(std::memcmp(&alias,&prior,sizeof alias)==0,"overflow alias mutation");
        ControlBoundStep48W combined{{{{{.5f,-.75f,0,.95f}},.375f},{{.1f,-.2f,.3f,-.4f}}},prior};
        ControlBoundStep48W unaliased{};
        require(stepAndBindControlInputs48W(combined.control.shaping.targets,combined.control.shaping.smoothedChannel0D28,combined.control.filtered,{4,3},.01f,in,nominal,combined.bindings,unaliased),"connected baseline rejected");
        require(stepAndBindControlInputs48W(combined.control.shaping.targets,combined.control.shaping.smoothedChannel0D28,combined.control.filtered,{4,3},.01f,in,nominal,combined.bindings,combined),"connected alias rejected");
        require(std::memcmp(&combined,&unaliased,sizeof combined)==0,"connected alias mismatch");
        const auto savedCombined=combined;
        require(!stepAndBindControlInputs48W(combined.control.shaping.targets,combined.control.shaping.smoothedChannel0D28,combined.control.filtered,{4,3},.01f,in,overflow,combined.bindings,combined),"connected late error accepted");
        require(std::memcmp(&combined,&savedCombined,sizeof combined)==0,"connected late failure mutation");
        auto badBindings=expected;badBindings.aeroBrakeEC=std::numeric_limits<float>::quiet_NaN();
        std::array<unsigned char,sizeof engine> engineBytes{};std::array<unsigned char,sizeof aero> aeroBytes{};
        std::memcpy(engineBytes.data(),&engine,sizeof engine);std::memcpy(aeroBytes.data(),&aero,sizeof aero);
        require(!applyControlModelBindings48W(badBindings,engine,aero,engine,aero),"invalid adapter accepted");
        require(std::memcmp(engineBytes.data(),&engine,sizeof engine)==0&&std::memcmp(aeroBytes.data(),&aero,sizeof aero)==0,"adapter rejection mutation");
        const auto before=allocations;
        for(unsigned n=0;n<20000;++n) {
            in.vehicleState40=n%5;in.mode58Nonzero=n&1;in.configurationEntryNonzero=n&2;
            require(stepAndBindControlInputs48W({{.5f,-.75f,0,.95f}},combined.control.shaping.smoothedChannel0D28,combined.control.filtered,{4,3},.01f,in,nominal,combined.bindings,combined),"hot connected step failed");
            require(applyControlModelBindings48W(combined.bindings,engine,aero,engine,aero),"hot adapter failed");
        }
        require(allocations==before,"hot allocation");
        unsigned covered=0;
        for(unsigned i=0;i<visits.size();++i) {if(visits[i])++covered;else std::cerr<<"unvisited binding PC "<<std::hex<<(0x2181c4+4*i)<<std::dec<<'\n';}
        require(covered==visits.size(),"uncovered binding word");
        std::cout<<"control_binding48w: "<<checks<<" checks; "<<cases<<" original-word cases ("<<connectedCases<<" connected); "<<instructions<<" instructions; all "<<covered<<" binding words visited; four rounding modes; zero hot allocations; structures "<<sizeof(ControlModelBindings48W)<<'/'<<sizeof(ControlBoundStep48W)<<" bytes\n";
        return 0;
    } catch(const std::exception& error) {
        std::fesetround(FE_TONEAREST);
        std::cerr<<"control_binding48w after "<<checks<<" checks / "<<cases<<" cases: "<<error.what()<<'\n';return 1;
    }
}

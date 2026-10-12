#include "boost_controls50fk.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <initializer_list>

namespace rr::game { namespace {
float fp(std::uint32_t bits) noexcept {float value;std::memcpy(&value,&bits,4);return value;}
bool fail(BoostDiagnostic50FK& d,BoostError50FK e,std::uint32_t pc) noexcept {
    d={e,pc};return false;
}
struct Math {
    bool valid=true;
    float keep(float value) noexcept {valid=valid&&std::isfinite(value);return value;}
    float add(float a,float b) noexcept {volatile float value=a+b;return keep(value);}
    float sub(float a,float b) noexcept {volatile float value=a-b;return keep(value);}
    float mul(float a,float b) noexcept {volatile float value=a*b;return keep(value);}
    float div(float a,float b) noexcept {volatile float value=a/b;return keep(value);}
};
}
bool projectBoostThermalNumbers50FK(const BoostInput50FK& i,const BoostParameters50FK& p,
    const BoostState50FK& previous,BoostState50FK& output,BoostDiagnostic50FK& d) noexcept {
    d={BoostError50FK::None,0};
    if(std::fegetround()!=FE_TONEAREST||i.frameFactor<0.f||previous.vehicle64>2u||
       previous.vehicle68>2u)return fail(d,BoostError50FK::Input,0x2176e0);
    for(float value:{i.frameFactor,i.engine70,i.engine74,i.cachedSpeed34,
        p.baseCooling80,p.speedCooling84,p.heatRate88,p.engineDifference5C,
        previous.duration6C,previous.speedRatio70,previous.heat7C})
        if(!std::isfinite(value))return fail(d,BoostError50FK::Input,0x2176e0);
    auto next=previous;Math f;
    next.vehicle68=next.vehicle64; //2176FC, even on the released path.
    float heating=0.f;
    if((i.flagsB9C&1u)&&!(i.flagsB9C&2u)) {
        heating=f.mul(p.heatRate88,f.add(i.engine74,i.engine70));
        //217744 skips the state store when heating<=0; it DOES NOT clear a
        // previous1/2. Only the input-release/repair path below clears64.
        if(!(heating<=0.f))next.vehicle64=1u;
    }else next.vehicle64=0u;
    //21779C/217870 player A8 is1A2B70 (JR RA/NOP), on both arguments0/1.
    if(i.vehicle58==0u&&i.profile34!=2u&&i.profile10FC0!=0u)next.heat7C=0.f;
    else {
        const float cooling=f.mul(i.cachedSpeed34,f.mul(p.speedCooling84,i.cachedSpeed34));
        const float rate=f.sub(f.sub(heating,cooling),p.baseCooling80);
        next.heat7C=f.add(next.heat7C,f.mul(i.frameFactor,rate));
    }
    if(!(next.heat7C<1.f)) {next.heat7C=1.f;next.vehicle64=2u;}
    else if(next.heat7C<0.f)next.heat7C=0.f;
    const float units=fp(0x3f2e8bc1u);
    const float numerator=f.sub(f.mul(units,i.cachedSpeed34),20.f);
    const float denominator=f.sub(f.mul(units,p.engineDifference5C),20.f);
    if(denominator==0.f)return fail(d,BoostError50FK::Arithmetic,0x2178e0);
    next.speedRatio70=f.div(numerator,denominator);
    if(next.vehicle64==1u||next.vehicle64==2u)next.duration6C=f.add(next.duration6C,i.frameFactor);
    else if(next.vehicle64==0u)next.duration6C=0.f;
    if(!f.valid)return fail(d,BoostError50FK::Arithmetic,0x2176e0);
    output=next;return true;
}
bool advancePlayerBoost50FK(const BoostInput50FK& i,const BoostParameters50FK& p,
    const BoostState50FK& previous,BoostState50FK& output,BoostDiagnostic50FK& d) noexcept {
    if(i.d40Present)return fail(d,BoostError50FK::AudioOwner,0x217760);
    return projectBoostThermalNumbers50FK(i,p,previous,output,d);
}
}

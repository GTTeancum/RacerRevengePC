#include "player_input48x.h"
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace rr::game {
namespace {
bool finite(float value) noexcept {std::uint32_t b;std::memcpy(&b,&value,4);return (b&0x7f800000u)!=0x7f800000u;}
float fp(std::uint32_t bits) noexcept {float value;std::memcpy(&value,&bits,4);return value;}
struct Arithmetic {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&finite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a,y=b;volatile float r=x+y;return keep(r);}
    float sub(float a,float b) noexcept {volatile float x=a,y=b;volatile float r=x-y;return keep(r);}
    float mul(float a,float b) noexcept {volatile float x=a,y=b;volatile float r=x*y;return keep(r);}
    float div(float a,float b) noexcept {volatile float x=a,y=b;volatile float r=x/y;return keep(r);}
};
float reflectedOriginalCosine(float argument,Arithmetic& f) noexcept {
    float sign=1.0f;
    if(!(argument<=fp(0x3fc90fdbu))) {sign=-1.0f;argument=f.sub(fp(0x40490fdbu),argument);}
    else if(argument<fp(0xbfc90fdbu)) {argument=f.sub(fp(0xc0490fdbu),argument);sign=-1.0f;}
    const float square=f.mul(argument,argument);
    float value=f.mul(fp(0xb493f27eu),square);
    value=f.add(fp(0x37d00d01u),value);
    value=f.mul(square,value);
    value=f.add(fp(0xbab60b61u),value);
    value=f.mul(square,value);
    value=f.add(fp(0x3d2aaaabu),value);
    value=f.mul(square,value);
    value=f.add(-0.5f,value);
    value=f.mul(square,value);
    value=f.add(1.0f,value);
    return f.mul(sign,value);
}
}

bool conditionInputAxis48X(float numerator,float denominator,const InputAxisRange48X& p,float& out) noexcept {
    for(float value:{numerator,denominator,p.minimum,p.maximum,p.center,p.upperDeadband,p.lowerDeadband})
        if(!finite(value))return false;
    if(!(denominator>0.0f&&p.minimum<p.lowerDeadband&&p.lowerDeadband<=p.center&&
         p.center<=p.upperDeadband&&p.upperDeadband<p.maximum))return false;
    Arithmetic f;
    const float quotient=f.div(numerator,denominator);
    const float span=f.sub(p.maximum,p.minimum);
    float value=f.add(p.minimum,f.mul(quotient,span));
    if(!(value<p.upperDeadband)) {
        const float offset=f.sub(value,p.upperDeadband);
        const float denominatorHigh=f.sub(p.maximum,p.upperDeadband);
        const float numeratorHigh=f.sub(p.maximum,p.center);
        value=f.add(f.mul(offset,f.div(numeratorHigh,denominatorHigh)),p.center);
    } else if(value<=p.lowerDeadband) {
        const float offset=f.sub(value,p.lowerDeadband);
        const float denominatorLow=f.sub(p.lowerDeadband,p.minimum);
        const float numeratorLow=f.sub(p.center,p.minimum);
        value=f.add(f.mul(offset,f.div(numeratorLow,denominatorLow)),p.center);
    } else value=p.center;
    if(!f.valid)return false;
    out=value;return true;
}

bool conditionDualShockPlayerAxes48X(DualShockAxisBytes48X sample,ControlChannels48B& out) noexcept {
    const InputAxisRange48X p{-1.0f,1.0f,0.0f,fp(0x3e4ccccdu),fp(0xbe4ccccdu)};
    Arithmetic f;
    const float values[4]={float(sample.packet6),float(sample.packet7),
                           f.sub(255.0f,float(sample.packet4)),float(sample.packet5)};
    ControlChannels48B result{};
    for(unsigned i=0;i<4;++i)if(!conditionInputAxis48X(values[i],255.0f,p,result.values[i]))return false;
    out=result;return true;
}

bool readPlayerInput48X(const PlayerReadContext48X& in,const PlayerActionCodes48X& codes,
                       PlayerActionProvider48X provider,PlayerReadResult48X& out) noexcept {
    if(!provider.query)return false;
    for(float value:in.componentValues.values)if(!finite(value))return false;
    for(float value:{in.source7D0,in.source7D4,in.source7D8,in.source7DC})if(!finite(value))return false;
    PlayerReadResult48X result{};result.raw=in.componentValues;result.flagsB9C=in.previousFlagsB9C;
    auto query=[&](std::int32_t code,bool& match) noexcept {
        const auto answer=provider.query(provider.context,code,0x3fu);
        if(answer!=PlayerActionResult48X::NoMatch&&answer!=PlayerActionResult48X::Match)return false;
        match=answer==PlayerActionResult48X::Match;return true;
    };
    bool match=false;
    if(!query(15,match))return false;
    if(match)result.raw.values[0]=-1.0f;
    else {
        if(!query(13,match))return false;
        if(match)result.raw.values[0]=1.0f;
    }
    match=false;
    if(codes.action694!=-1&&!query(codes.action694,match))return false;
    result.actionB7C=match?1.0f:0.0f;
    match=false;
    if(codes.action690!=-1&&!query(codes.action690,match))return false;
    result.actionB80=match?1.0f:0.0f;
    if((in.gateByte58&3u)==2u) {
        if(!query(codes.flag698,match))return false;
        result.flagsB9C=static_cast<std::uint8_t>((result.flagsB9C&~1u)|(match?1u:0u));
        if(!query(codes.flag69C,match))return false;
        // Original short-circuit order is 7D4,7D0,7DC,7D8; finite domain.
        const bool flag1=match&&(!(in.source7D4<=0.0f)||!(in.source7D0<=0.0f)||
                                !(in.source7DC<=0.0f)||!(in.source7D8<=0.0f));
        result.flagsB9C=static_cast<std::uint8_t>((result.flagsB9C&~2u)|(flag1?2u:0u));
        if(!query(codes.flag6B4,match))return false;
        result.flagsB9C=static_cast<std::uint8_t>((result.flagsB9C&~4u)|(match?4u:0u));
    } else result.flagsB9C=static_cast<std::uint8_t>(result.flagsB9C&~7u);
    out=result;return true;
}

bool shapePlayerControlChannels48X(const ControlChannels48B& raw,float history,
                                  ControlShapeMode48V mode,ControlShapingResult48V& out) noexcept {
    if(!finite(history)||(mode!=ControlShapeMode48V::Nonlinear&&mode!=ControlShapeMode48V::BypassNonlinear))return false;
    bool ordinary=true;
    const float limit=fp(0x3f800001u);
    for(float value:raw.values) {
        if(!finite(value)||value < -limit||value>limit)return false;
        ordinary=ordinary&&value>=-1.0f&&value<=1.0f;
    }
    if(ordinary)return shapeControlInputs48V(raw,history,mode,out);
    Arithmetic f;
    ControlChannels48B shaped=raw;
    if(mode==ControlShapeMode48V::Nonlinear)for(unsigned i=0;i<4;++i) {
        const float cosine=reflectedOriginalCosine(f.mul(fp(0x3fc90fdbu),raw.values[i]),f);
        shaped.values[i]=raw.values[i]<0.0f?f.sub(cosine,1.0f):f.sub(1.0f,cosine);
    }
    ControlShapingResult48V result{};
    const float contribution=f.mul(fp(0x3dcccccdu),shaped.values[0]);
    result.smoothedChannel0D28=f.add(contribution,f.mul(fp(0x3f666666u),history));
    for(unsigned i=0;i<4;++i) {
        const float scaled=f.mul(100.0f,shaped.values[i]);
        const auto integer=static_cast<std::int32_t>(f.add(0.5f,scaled));
        volatile float asFloat=static_cast<float>(integer);
        result.targets.values[i]=f.mul(fp(0x3c23d70au),asFloat);
    }
    if(!f.valid)return false;
    out=result;return true;
}

bool stepDualShockPlayerControls48X(DualShockAxisBytes48X sample,const PlayerReadContext48X& playerContext,
                                   const PlayerActionCodes48X& codes,PlayerActionProvider48X provider,
                                   float previousD28,const ControlChannels48B& previousFiltered,
                                   ControlRateLimits48B rates,float elapsedFactor,
                                   const ControlBindingInput48W& bindingContext,
                                   const ControlBindingParameters48W& parameters,
                                   const ControlModelBindings48W& previousBindings,PlayerControlStep48X& out) noexcept {
    PlayerControlStep48X result{};
    if(!conditionDualShockPlayerAxes48X(sample,result.components))return false;
    auto context=playerContext;context.componentValues=result.components;
    if(!readPlayerInput48X(context,codes,provider,result.player))return false;
    const auto mode=bindingContext.mode58Nonzero?ControlShapeMode48V::BypassNonlinear:ControlShapeMode48V::Nonlinear;
    if(!shapePlayerControlChannels48X(result.player.raw,previousD28,mode,result.bound.control.shaping))return false;
    if(!stepControlRateFilter48B(result.bound.control.shaping.targets,previousFiltered,rates,elapsedFactor,result.bound.control.filtered))return false;
    auto binding=bindingContext;binding.filtered=result.bound.control.filtered;
    binding.actionB7C=result.player.actionB7C;binding.actionB80=result.player.actionB80;binding.flagsB9C=result.player.flagsB9C;
    binding.source7D0=playerContext.source7D0;binding.source7D4=playerContext.source7D4;
    if(!bindVehicleControls48W(binding,parameters,previousBindings,result.bound.bindings))return false;
    out=result;return true;
}
}

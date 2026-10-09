#include "gradient_noise49m.h"
#include <cstring>

namespace rr::game {
namespace {
bool finite(float value) noexcept {
    std::uint32_t word;std::memcpy(&word,&value,sizeof(word));
    return (word&0x7f800000u)!=0x7f800000u;
}
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float divide(float a,float b) noexcept {volatile float r=a/b;return r;}
std::uint32_t next(std::uint32_t& raw) noexcept {
    raw=raw*0x41c64e6du+0x3039u;
    return raw&0x7fffffffu;
}
void initializeProjected(GradientNoiseState49M& state,std::uint32_t& raw) noexcept {
    for(std::uint32_t i=0;i<256;++i){
        state.permutation[i]=i;
        const auto integer=static_cast<std::int32_t>(next(raw)&511u)-256;
        volatile float converted=static_cast<float>(integer);
        state.gradient1D[i]=divide(converted,256.0f);
        // The next2 and next3 original draws initialize unconsumed2-D/3-D
        // gradients. Their normalization has no dependency back to this slice.
        // Retain every random transition; do not replace the source draw order.
        for(unsigned unusedGradient=0;unusedGradient<5;++unusedGradient)next(raw);
    }
    for(std::uint32_t i=255;i!=0;--i){
        const auto j=next(raw)&255u;
        const auto saved=state.permutation[i];
        state.permutation[i]=state.permutation[j];
        state.permutation[j]=saved;
    }
    // Original copies258 entries to each table's256-offset mirror. 14EEC0
    // masks both indices to255, so those writes cannot affect retained reads.
    state.initialized=true;
}
bool evaluateReady(const GradientNoiseState49M& state,float shifted,
                   std::int32_t cell,float& result) noexcept {
    const auto first=static_cast<std::uint32_t>(cell)&255u;
    const auto second=(first+1u)&255u;
    const auto p0=state.permutation[first],p1=state.permutation[second];
    if(p0>=256||p1>=256)return false;
    const float g0=state.gradient1D[p0],g1=state.gradient1D[p1];
    if(!finite(g0)||!finite(g1)||g0<-1.0f||g0>0x1.fep-1f||
       g1<-1.0f||g1>0x1.fep-1f)return false;
    volatile float cellAsFloat=static_cast<float>(cell);
    const float fraction=sub(shifted,cellAsFloat);
    const float previous=sub(fraction,1.0f);
    const float twice=mul(2.0f,fraction);
    const float curveFactor=sub(3.0f,twice);
    const float left=mul(fraction,g0);
    const float square=mul(fraction,fraction);
    const float curve=mul(curveFactor,square);
    const float right=mul(previous,g1);
    const float delta=sub(right,left);
    const float weighted=mul(curve,delta);
    result=add(left,weighted);
    return finite(result);
}
}
bool evaluateGradientNoise49M(GradientNoiseState49M& state,
    std::uint32_t& sharedRawRandomState,float coordinate,float& output) noexcept {
    if(!finite(coordinate))return false;
    const float shifted=add(4096.0f,coordinate);
    if(!finite(shifted)||shifted< -0x1p31f||shifted>=0x1p31f)return false;
    // Original CVT.W.S is signed truncation, including negative shifted input.
    const auto cell=static_cast<std::int32_t>(shifted);
    float result;
    if(state.initialized){
        if(!evaluateReady(state,shifted,cell,result))return false;
        state.scratchCell=cell;
    }else{
        GradientNoiseState49M prepared;
        auto raw=sharedRawRandomState;
        initializeProjected(prepared,raw);
        if(!evaluateReady(prepared,shifted,cell,result))return false;
        prepared.scratchCell=cell;
        state=prepared;
        sharedRawRandomState=raw;
    }
    output=result;
    return true;
}
}

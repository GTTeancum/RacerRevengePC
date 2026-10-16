#pragma once
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

// Pure native arithmetic candidate, NOT owner/renderer admission. No camera,
// model flag, material FGE, packet submission or lifetime is minted here.
// Source:240A94..240AD0,340188..340218,339D38..339E78,
// 33A118..33A228,338398..3384E0,3387C8..3388F8.
// Ordinary binary32 operations/native sqrt intentionally do not emulate VU
// EFU, accumulator/denormal modes or claim PS2 numerical parity.
namespace ra_static_fog_numeric50LA {
enum class Status { complete,input,arithmetic };
using Matrix=std::array<float,16>;
struct CompressedPreamble {
    std::array<float,3> localCameraInRawUnits{};
    float endTimesUnscaledSlope=0,scaledSlope=0;
};
namespace detail {
inline float add(float a,float b)noexcept{volatile float r=a+b;return r;}
inline float sub(float a,float b)noexcept{volatile float r=a-b;return r;}
inline float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
inline float div(float a,float b)noexcept{volatile float r=a/b;return r;}
inline bool finite(float x)noexcept{return std::isfinite(x);}
inline bool ready()noexcept{
    return sizeof(float)==sizeof(std::uint32_t)&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::digits==24&&std::fegetround()==FE_TONEAREST;
}
}
// packetModel is the ACTUAL source packet matrix, not the semantic import's
// visual matrix. World-space240B04 uses identity basis plus PSG origin1D0..1D8;
// local single-node241678 uses current model world100. The caller must recover
// these real operands through a separate typed owner before using the result.
// positionScale is model1E0; row42.w is freshly rounded1/positionScale at240AC8,
// NOT PSG quantization[3]. sourceSlope is real245140 contextB8/packet33.w.
inline Status prepareCompressed(const Matrix& packetModel,
    const std::array<float,3>& sourceCamera180,float positionScale,
    float sourceEnd33Z,float sourceSlope33W,CompressedPreamble& output)noexcept {
    using namespace detail;
    if(!ready())return Status::arithmetic;
    for(float x:packetModel)if(!finite(x))return Status::input;
    for(float x:sourceCamera180)if(!finite(x))return Status::input;
    if(packetModel[3]!=0||packetModel[7]!=0||packetModel[11]!=0||packetModel[15]!=1||
       !finite(positionScale)||positionScale<=0||!finite(sourceEnd33Z)||
       !finite(sourceSlope33W))return Status::input;
    const float inverseScale=div(1.f,positionScale);
    if(!finite(inverseScale))return Status::arithmetic;
    CompressedPreamble next;
    std::array<float,3> delta{};
    for(unsigned c=0;c<3;++c)delta[c]=sub(sourceCamera180[c],packetModel[12+c]);
    for(unsigned a=0;a<3;++a){
        const float x=mul(delta[0],packetModel[a*4]);
        const float y=mul(delta[1],packetModel[a*4+1]);
        const float z=mul(delta[2],packetModel[a*4+2]);
        //3401C0..3401F0 preserves the distinct lane addition orders.
        const float local=a==0?add(add(x,y),z):a==1?add(add(y,x),z):add(add(z,x),y);
        next.localCameraInRawUnits[a]=mul(local,inverseScale); //340218
        if(!finite(next.localCameraInRawUnits[a]))return Status::arithmetic;
    }
    next.endTimesUnscaledSlope=mul(sourceEnd33Z,sourceSlope33W); //3401F8 BEFORE scale
    next.scaledSlope=mul(sourceSlope33W,positionScale); //340210
    if(!finite(next.endTimesUnscaledSlope)||!finite(next.scaledSlope))return Status::arithmetic;
    output=next;return Status::complete;
}
// Call only for the true source compute-fog bit02 path. This pure function
// accepts numeric snapshots, which cannot prove that branch or current owner.
// Position is original signed24 integer data BEFORE quantization/origin.
inline Status radialByte(const CompressedPreamble& in,
    const std::array<std::int32_t,3>& rawPosition,std::uint8_t& output)noexcept {
    using namespace detail;
    if(!ready())return Status::arithmetic;
    if(!finite(in.endTimesUnscaledSlope)||!finite(in.scaledSlope))return Status::input;
    std::array<float,3> delta{};
    for(unsigned c=0;c<3;++c){
        if(rawPosition[c]<-8388608||rawPosition[c]>8388607||
           !finite(in.localCameraInRawUnits[c]))return Status::input;
        delta[c]=sub(static_cast<float>(rawPosition[c]),in.localCameraInRawUnits[c]);
    }
    const float xy=add(mul(delta[0],delta[0]),mul(delta[1],delta[1]));
    const float squared=add(xy,mul(delta[2],delta[2]));
    if(!finite(squared)||squared<0)return Status::arithmetic;
    const float distance=std::sqrt(squared); //native approximation of source ELENG
    float biased=add(sub(524288.f,in.endTimesUnscaledSlope),mul(distance,in.scaledSlope));
    if(!finite(distance)||!finite(biased))return Status::arithmetic;
    if(biased<524288.f)biased=524288.f;
    if(biased>524543.f)biased=524543.f;
    std::uint32_t bits=0;std::memcpy(&bits,&biased,sizeof(bits));
    output=static_cast<std::uint8_t>((bits>>4)&255u);return Status::complete;
}
// Only the separately proven bit02-clear branch emits this constant. FGE0
// merely disables the fragment fog operation; it does not prove bit02 clear.
inline constexpr std::uint8_t noComputeFogByte=255;
}

#include "original_ui_main_particle_geometry50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool finite(float value) noexcept {return std::isfinite(value);}
std::array<float,4> transform(const std::array<float,3>& point,
    const std::array<float,16>& matrix) noexcept {
    std::array<float,4> result{};
    // Source row-vector center transform; individual native binary32 steps.
    for(unsigned c=0;c<4;++c){
        float v=mul(matrix[c],point[0]);
        v=add(v,mul(matrix[4+c],point[1]));
        v=add(v,mul(matrix[8+c],point[2]));
        result[c]=add(v,matrix[12+c]);
    }
    return result;
}
float fixed4(float value) noexcept {
    // Source XYZF2 packet X/Y are fixed4. Caller already bounded magnitude.
    return div(std::trunc(mul(value,16.f)),16.f);
}
}
OriginalUiMainParticleGeometryStatus50KG projectOriginalUiMainParticleQuad50KG(
    const OriginalUiMainParticlePacket50KG& packet,std::uint8_t stateIndex,
    const OriginalUiMainParticleCamera50KG& camera,
    const OriginalUiMainParticleRenderConstants50KG& constants,
    OriginalUiMainParticleQuad50KG& output) noexcept {
    using Status=OriginalUiMainParticleGeometryStatus50KG;
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
        "Native ParticleCode requires binary32");
    if(stateIndex>1 || packet.sourceFrame<52 || packet.sourceFrame>75 ||
        packet.sourceDepthBucket>=originalUiMainDepthBuckets50KG ||
        !finite(packet.sizeTimesCamera1D8) || packet.sizeTimesCamera1D8<0 ||
        !finite(packet.quantizedSourceDepth) || !finite(packet.sourceBrightness) ||
        packet.sourceBrightness<0 || packet.sourceBrightness>128)
        return Status::input;
    for(float v:packet.position)if(!finite(v))return Status::input;
    for(const auto& uv:packet.normalizedUv)for(float v:uv)if(!finite(v))return Status::input;
    for(float v:camera.combinedSourceScreen250)if(!finite(v))return Status::input;
    for(float v:constants.combinedClip100)if(!finite(v))return Status::input;
    if(!finite(constants.fogStart33Z) || !finite(constants.fogScale33W) ||
        constants.sourceCanvasOffset[0]!=1728.f || constants.sourceCanvasOffset[1]!=1824.f)
        return Status::input;
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    OriginalUiMainParticleQuad50KG next;
    next.stateIndex=stateIndex;next.sourceParticleIndex=packet.sourceParticleIndex;
    next.sourceFrame=packet.sourceFrame;next.sourceDepthBucket=packet.sourceDepthBucket;
    const auto clip=transform(packet.position,constants.combinedClip100);
    for(float v:clip)if(!finite(v))return Status::arithmetic;
    //321518 CLIPw then321538 FCAND sets the two strip drawing-kick bits.
    // Native center rejection preserves that whole-particle decision. W<=0 is
    // the native finite projection rule; no behind-camera substitute is drawn.
    if(clip[3]<=0 || std::abs(clip[0])>clip[3] || std::abs(clip[1])>clip[3] ||
        std::abs(clip[2])>clip[3]) {output=next;return Status::complete;}
    const auto screen=transform(packet.position,camera.combinedSourceScreen250);
    for(float v:screen)if(!finite(v))return Status::arithmetic;
    if(screen[3]<=0){output=next;return Status::complete;}
    const float reciprocal=div(1.f,screen[3]);
    const float x=mul(screen[0],reciprocal),y=mul(screen[1],reciprocal);
    const float z=mul(screen[2],reciprocal);
    const float extent=mul(packet.sizeTimesCamera1D8,reciprocal);
    const float fog=mul(sub(packet.quantizedSourceDepth,constants.fogStart33Z),constants.fogScale33W);
    if(!finite(reciprocal)||!finite(x)||!finite(y)||!finite(z)||!finite(extent)||!finite(fog))
        return Status::arithmetic;
    if(z<0 || z>16777215.f || extent>1048576.f)return Status::arithmetic;

    //3213C8..3213E0 and3215E8: source writes float bits of brightness+2^23
    // into packed RGBA alpha. IEEE binary32 at this magnitude rounds to units;
    // the byte is therefore nearest-even, not a guessed truncating conversion.
    const float biasedAlpha=add(8388608.f,packet.sourceBrightness);
    std::uint32_t alphaBits=0;std::memcpy(&alphaBits,&biasedAlpha,sizeof(alphaBits));
    const auto alpha=static_cast<std::uint8_t>(alphaBits&0xffu);
    if(alpha>128)return Status::arithmetic;
    // FTOI4.w followed by XYZF2's fog field extraction retains the integer
    // coefficient. Normalize by256 for the GS fog equation, not by255.
    const float fogByte=std::trunc(std::clamp(fog,0.f,254.f));
    const float fogFactor=div(fogByte,256.f);
    // The source uses XYZF2 FTOI4.z and the packed Z field discards its low4
    // bits. Positive admitted depth therefore becomes one integer packet Z.
    const float packetZ=std::trunc(z);
    // Source strip order TL,BL,TR,BR. Preserve its asymmetric screen-boundary
    // clamps before fixed4 conversion (321580..3215C8).
    const float left=sub(x,extent),right=add(x,extent);
    const float top=sub(y,extent),bottom=add(y,extent);
    const std::array<std::array<float,2>,4> strip{{
        {{std::max(0.f,left),std::max(0.f,top)}},
        {{std::clamp(left,0.f,4094.f),std::clamp(bottom,0.f,4094.f)}},
        {{std::clamp(right,0.f,4094.f),std::clamp(top,0.f,4094.f)}},
        {{std::min(right,4094.f),std::min(bottom,4094.f)}}
    }};
    constexpr unsigned nativeFromStrip[]={1,0,2,3};
    constexpr unsigned uvFromStrip[]={0,3,1,2}; //packet UV is TL,TR,BR,BL.
    for(unsigned n=0;n<4;++n){
        const auto source=nativeFromStrip[n];auto& v=next.vertices[n];
        for(float value:strip[source])
            if(!finite(value)||std::abs(value)>134217720.f)return Status::arithmetic;
        v.position[0]=sub(fixed4(strip[source][0]),constants.sourceCanvasOffset[0]);
        v.position[1]=sub(fixed4(strip[source][1]),constants.sourceCanvasOffset[1]);
        v.position[2]=packetZ;
        v.uv[0]=packet.normalizedUv[uvFromStrip[source]][0];
        v.uv[1]=packet.normalizedUv[uvFromStrip[source]][1];
        for(unsigned c=0;c<3;++c)v.color[c]=constants.sourceLighting[source][c];
        v.color[3]=alpha;next.fogFactors[n]=fogFactor;
    }
    next.visible=true;output=next;return Status::complete;
}
}

#include "engine_flare50gv.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
float fp(std::uint32_t word) noexcept { float value;std::memcpy(&value,&word,4);return value; }
std::uint32_t bits(float value) noexcept { std::uint32_t word;std::memcpy(&word,&value,4);return word; }
bool finite(float value) noexcept {const auto word=bits(value),e=word&0x7f800000u;return e!=0x7f800000u&&(e||(word&0x7fffffu)==0);}
struct Math {
    bool good=true;
    float check(float value) noexcept {if(!finite(value))good=false;return value;}
    float add(float a,float b) noexcept {volatile float v=a+b;return check(v);}
    float sub(float a,float b) noexcept {volatile float v=a-b;return check(v);}
    float mul(float a,float b) noexcept {volatile float v=a*b;return check(v);}
    float div(float a,float b) noexcept {volatile float v=a/b;return check(v);}
};
bool affine(const assets::FlameUniformRows50W& u) noexcept {
    for(const auto& row:u.rows)for(float v:row)if(!finite(v))return false;
    // Columns carried by the source TexGen rows form a 3x3 affine ST matrix.
    return u.rows[0][2]==0.f&&u.rows[1][2]==0.f&&u.rows[2][2]==1.f&&
           u.rows[0][3]==0.f&&u.rows[1][3]==0.f&&u.rows[2][3]==0.f;
}
void exportUV(const assets::FlameUniformRows50W& u,float(&uv)[6]) noexcept {
    uv[0]=u.rows[0][0];uv[1]=u.rows[1][0];uv[2]=u.rows[2][0];
    uv[3]=u.rows[0][1];uv[4]=u.rows[1][1];uv[5]=u.rows[2][1];
}
}
bool advanceEngineFlare50GV(const VehicleFrameState49W& frame,
    const BaseTailState50X& tail,const assets::PodFlameOwners50W& owners,
    std::uint32_t stamp,EngineFlareOutput50GV& output,
    EngineFlareDiagnostic50GV& diagnostic) noexcept {
    diagnostic={};
    const auto fail=[&](EngineFlareError50GV e,std::uint32_t side,std::uint32_t pc){diagnostic={e,side,pc};return false;};
    if(frame.phase!=VehicleFramePhase49W::Ready&&frame.phase!=VehicleFramePhase49W::PostGraphics)
        return fail(EngineFlareError50GV::phase,0,0x29ac90);
    if(std::fegetround()!=FE_TONEAREST)return fail(EngineFlareError50GV::roundingMode,0,0);
    const auto& main=frame.bodies.main;
    if(main.applicationActive11C)return fail(EngineFlareError50GV::unsupportedState,0,0x29acb8);
    if(stamp==0xffffffffu)return fail(EngineFlareError50GV::input,0,0x29acac);
    if(!owners.records[2].words[1]||owners.records[2].words[1]!=owners.records[3].words[1]||
       owners.records[2].words[0]!=0x20f110u||owners.records[3].words[0]!=0x20ee90u)
        return fail(EngineFlareError50GV::binding,0,0x29acec);
    EngineFlareOutput50GV next{};next.sharedRandomState=frame.noiseOwner().sharedRandomState;
    const bool steering=main.components.aero.lowSpeedDc&&(frame.fields.applicationByte58&2u);
    for(unsigned side=0;side<2;++side){
        next.uniforms[side]=owners.uniforms[side+2];next.flameAC4[side]=tail.flameAC4[side];
        next.frameStamp[side]=owners.records[side+2].words[2];
        if(!affine(next.uniforms[side]))return fail(EngineFlareError50GV::input,side,0x246b70);
        if(next.frameStamp[side]==stamp){exportUV(next.uniforms[side],next.uv[side]);continue;}
        Math math;
        float magnitude=0.f;
        if(steering){
            float drive=main.controls.player.raw.values[0];
            if(!finite(drive))return fail(EngineFlareError50GV::input,side,side?0x20ef54:0x20f1d4);
            if(side)drive=-drive;
            magnitude=math.mul(6000.f,drive>0.f?drive:0.f);
        }else{
            const auto& thrust=main.components.engine.localThrust[side].v;
            for(unsigned axis=0;axis<3;++axis)if(!finite(thrust[axis]))
                return fail(EngineFlareError50GV::input,side,side?0x20ef88:0x20f204);
            const float yy=math.mul(thrust[1],thrust[1]);
            const float xx=math.mul(thrust[0],thrust[0]);
            const float xy=math.add(xx,yy);
            const float xyz=math.add(xy,math.mul(thrust[2],thrust[2]));
            magnitude=math.check(std::sqrt(xyz));
        }
        next.thrustMagnitude[side]=magnitude;
        const bool active=(side?tail.flagsACC:tail.flagsAD0)!=0;
        if(active){
            const float old=fp(tail.flameAC4[side]);
            if(!finite(old))return fail(EngineFlareError50GV::input,side,side?0x20efc8:0x20f244);
            const float rate=magnitude<old?fp(0x3d4ccccdu):fp(0x3e19999au);
            magnitude=math.add(old,math.mul(rate,math.sub(magnitude,old)));
            next.flameAC4[side]=bits(magnitude);
        }
        float normalized=math.div(active?math.add(4000.f,magnitude):magnitude,10000.f);
        if(active&&normalized<fp(0x3f19999au))normalized=fp(0x3f19999au);
        //308030 returns masked31-bit result, but stores the complete32-bit LCG state.
        next.sharedRandomState=next.sharedRandomState*0x41c64e6du+0x3039u;
        volatile float converted=static_cast<float>(next.sharedRandomState&0x7fffffffu);
        const float random=math.mul(fp(0x30000000u),converted);
        const float scale=math.add(math.div(normalized,fp(0x3f8ccccdu)),math.mul(fp(0x3dcccccdu),random));
        next.uniforms[side].rows[1][1]=scale;
        float scroll=math.add(next.uniforms[side].rows[2][0],fp(0x3de353f8u));
        if(scroll>1.f)scroll=math.sub(scroll,1.f);
        next.uniforms[side].rows[2][0]=scroll;
        if(!math.good)return fail(EngineFlareError50GV::arithmetic,side,side?0x20f0a8:0x20f324);
        next.frameStamp[side]=stamp;++next.callbacks;++next.randomDraws;
        exportUV(next.uniforms[side],next.uv[side]);
    }
    output=next;return true;
}
void commitEngineFlare50GV(const EngineFlareOutput50GV& result,VehicleFrameState49W& frame,
    BaseTailState50X& tail,assets::PodFlameOwners50W& owners) noexcept {
    for(unsigned side=0;side<2;++side){tail.flameAC4[side]=result.flameAC4[side];
        owners.uniforms[side+2]=result.uniforms[side];owners.records[side+2].words[2]=result.frameStamp[side];}
    frame.noiseOwner().sharedRandomState=result.sharedRandomState;
}
}

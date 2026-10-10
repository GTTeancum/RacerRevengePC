#pragma once
#include "body_scene48g.h"
#include "player_input48x.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <limits>

namespace rr::game {
// Source-only recovery of player 1A36A0..1A392C and steering 1A3460..1A369C.
// NOT compiled or executed. Integration must retain the source caller gate:
// !1B0B80(race manager) && (player.CD8 & 1), after navigation, before 228620.
// This leaf never changes vehicle58 or the selected controller configuration.
// In particular, a nonzero twin-engine configuration can ignore B5C/actions.
struct PlayerFinishInput50IA {
    float resultsCountdown94;
    // Exact capture of aero350+100 made by player construction 1A0858..5C.
    float authoredSteering10A4;
    float bodyCachedSpeed34;
    float bodyWorldAngularA4;
    // Engine34C+5C: theoretical top speed minus actual top speed after authoring.
    float engineDifference5C;
    float bodySceneWorldPosition[3];
    float mapCurrentNodePosition[3]; // Actual MAP node indexed by live C88.
    float mainSceneLocalForward[3]; // 28A530 first output: local E0/E4/E8.
    float mainSceneLocalUpY;        // 28A530 second output's Y: local D4.
    std::uint8_t previousFlagsB9C;
};
struct PlayerFinishResult50IA {
    PlayerReadResult48X player;
    float aeroSteering100;
};
namespace player_finish_detail50IA {
inline float word(std::uint32_t bits) noexcept {
    float result; std::memcpy(&result,&bits,sizeof result); return result;
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {volatile float x=a/b;return keep(x);}
};
// Exact 172DF0 wrapper around 13C1F0. Its X==0 positive-Z behavior is zero,
// not libm atan2 nor the positive-axis branch of originalAngle48G itself.
inline bool directionAngle(const float v[3],float& out) noexcept {
    if(v[0]==0.0f) {
        out=v[2]<0.0f?word(0xbfc90fdbu):0.0f;
        return true;
    }
    return originalAngle48G(v[2],v[0],out);
}
}
// Finite source-projection domain with strict separately rounded binary32
// operations and FE_TONEAREST. Caller owns already-resolved world/local scene
// getters and genuine map target; no navigation, timer, pose or physics write.
// Unsupported arithmetic leaves out unchanged. No allocation or guest runtime.
inline bool driveFinishedPlayer50IA(const PlayerFinishInput50IA& in,
                                    PlayerFinishResult50IA& out) noexcept {
    using namespace player_finish_detail50IA;
    if(std::fegetround()!=FE_TONEAREST)return false;
    for(float v:{in.resultsCountdown94,in.authoredSteering10A4,
        in.bodyCachedSpeed34,in.bodyWorldAngularA4,in.engineDifference5C,
        in.mainSceneLocalUpY})if(!std::isfinite(v))return false;
    for(unsigned axis=0;axis<3;++axis)
        if(!std::isfinite(in.bodySceneWorldPosition[axis])||
           !std::isfinite(in.mapCurrentNodePosition[axis])||
           !std::isfinite(in.mainSceneLocalForward[axis]))return false;
    Math f; PlayerFinishResult50IA result{};
    float factor=1.0f;
    if(in.resultsCountdown94<2.0f)
        factor=detail::scalarMax48O(word(0x3ecccccdu),
                                   f.div(in.resultsCountdown94,2.0f));
    result.aeroSteering100=f.mul(f.mul(in.authoredSteering10A4,2.5f),factor);

    float direction[3];
    for(unsigned axis=0;axis<3;++axis)
        direction[axis]=f.sub(in.mapCurrentNodePosition[axis],in.bodySceneWorldPosition[axis]);
    const float xx=f.mul(direction[0],direction[0]);
    const float yy=f.mul(direction[1],direction[1]);
    const float zz=f.mul(direction[2],direction[2]);
    const float squared=f.add(f.add(xx,yy),zz);
    if(!f.valid)return false;
    if(squared==0.0f) {
        // Original static434060 is (0,0,0,1), not a fabricated forward axis.
        direction[0]=direction[1]=direction[2]=0.0f;
    } else {
        // Same admitted scalar RSQRT convention as the existing native core.
        if(squared<std::numeric_limits<float>::min())return false;
        volatile float root=std::sqrt(squared);
        const float reciprocal=f.div(1.0f,root);
        for(float& value:direction)value=f.mul(value,reciprocal);
    }
    float desired,current;
    if(!f.valid||!directionAngle(direction,desired)||
       !directionAngle(in.mainSceneLocalForward,current))return false;
    if(in.mainSceneLocalUpY<0.0f) {desired=-desired;current=-current;}
    if(current>0.0f&&desired<0.0f)desired=f.add(desired,word(0x40c90fdbu));
    float error=f.sub(current,desired);
    if(error<word(0xc0490fdbu))error=f.add(error,word(0x40c90fdbu));

    // Source 1A3548..1A3630. Near-straight keeps gain3; the equal-endpoint
    // path instead uses gain0. This is not the different AI172E60 formula.
    float gain=3.0f;
    const float speed=f.mul(word(0x3f2e8bc1u),in.bodyCachedSpeed34);
    // 1A3570 writes f1=f2*f1; f2 retains the unit-conversion constant.
    // 1A3574 therefore computes f3=f2*f0, NOT convertedSpeed*engineDifference.
    const float basis=f.mul(word(0x3f2e8bc1u),in.engineDifference5C);
    if(!(std::fabs(error)<=word(0x3e4ccccdu))) {
        const float high=f.add(basis,100.0f);
        const float low=f.sub(basis,200.0f);
        const float offset=f.sub(speed,low); // Taken branch's actual delay slot.
        if(high==low)gain=0.0f;
        else {
            const float span=f.sub(high,low);
            const float amount=f.div(f.mul(offset,2.0f),span);
            gain=f.sub(gain,amount);
            if(!(gain<=5.0f))gain=5.0f;
            else if(gain<3.0f)gain=3.0f;
        }
    }
    float steering=f.sub(f.mul(gain,error),in.bodyWorldAngularA4);
    const float magnitude=std::fabs(steering);
    if(!(magnitude<=1.0f))steering=f.div(steering,magnitude);
    result.player.raw.values[0]=steering;
    result.player.raw.values[1]=0.0f;
    result.player.raw.values[2]=0.0f;
    result.player.raw.values[3]=0.0f;
    // 1A3828..1A38CC reloads speed before its independent conversion.
    const float actionSpeed=f.mul(word(0x3f2e8bc1u),in.bodyCachedSpeed34);
    if(actionSpeed>160.0f&&in.resultsCountdown94>1.0f) {
        result.player.actionB7C=0.0f; result.player.actionB80=1.0f;
    } else if(in.resultsCountdown94>1.25f) {
        result.player.actionB7C=1.0f; result.player.actionB80=1.0f;
    } else {
        result.player.actionB7C=0.0f; result.player.actionB80=0.0f;
    }
    result.player.flagsB9C=static_cast<std::uint8_t>(in.previousFlagsB9C&0xf8u);
    if(!f.valid)return false;
    out=result;return true;
}
}

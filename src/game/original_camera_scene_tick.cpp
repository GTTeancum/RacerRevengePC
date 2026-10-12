#include "original_camera_scene_tick.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
float constant(std::uint32_t word) noexcept {float f;std::memcpy(&f,&word,sizeof f);return f;}
struct Math {
    bool ok=true;
    float read(float x) noexcept {ok=ok&&std::isfinite(x)&&(x==0||std::fabs(x)>=std::numeric_limits<float>::min());return x;}
    float add(float a,float b) noexcept {read(a);read(b);volatile float v=a+b;return read(v);}
    float sub(float a,float b) noexcept {read(a);read(b);volatile float v=a-b;return read(v);}
    float mul(float a,float b) noexcept {read(a);read(b);volatile float v=a*b;return read(v);}
    float angle(float input) noexcept {
        //13C770, the same polynomial recovered in50GO. NOT sourceAcos49T.
        const float absolute=std::fabs(read(input));
        if(input>1)return 0;
        if(input< -1)return constant(0x40490fdbu);
        float t=add(constant(0xc372b84fu),mul(constant(0x42a8a11bu),absolute));
        t=add(constant(0x43836812u),mul(t,absolute));
        t=add(constant(0xc3031cc3u),mul(absolute,t));
        t=add(constant(0x41ed4ad5u),mul(absolute,t));
        t=add(constant(0xbfb9d5d7u),mul(absolute,t));
        t=add(constant(0x3d06871eu),mul(absolute,t));
        return input<0?add(constant(0x3fc90fdbu),t):sub(constant(0x3fc90fdbu),t);
    }
};
}
bool bindFreshOriginalCameraSceneTick(OriginalCameraSceneTick& state,
    std::uint64_t session,std::uint64_t scene) noexcept {
    if(state.bound||!session||!scene)return false;
    OriginalCameraSceneTick next;next.sessionGeneration=session;next.sceneKey=scene;next.bound=true;
    state=next;return true;
}
OriginalCameraSceneTickStatus advanceOriginalCameraSceneTick114AE0(
    OriginalCameraSceneTick& state,std::uint64_t session,std::uint64_t scene,
    const assets::TVCameraVector49E& position,const assets::TVCameraVector49E& look,
    std::uint8_t flags) noexcept {
    using S=OriginalCameraSceneTickStatus;
    if(!state.bound||state.sessionGeneration!=session||state.sceneKey!=scene)return S::owner;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    Math m;auto next=state;
    const float dx=m.sub(position.x,state.previousPosition1B0.x),
        dy=m.sub(position.y,state.previousPosition1B0.y),dz=m.sub(position.z,state.previousPosition1B0.z);
    //114B40/44 multiply X/Y,114B48 ADD.A,114B4C MADD of Z.
    const float xx=m.mul(dx,dx),yy=m.mul(dy,dy),acc=m.add(xx,yy);
    const float square=m.add(acc,m.mul(dz,dz));
    if(!m.ok||square<0)return S::arithmetic;
    volatile float distance=std::sqrt(square);next.distance240=m.read(distance);
    std::memcpy(&next.previousPosition1B0,&position,sizeof position);
    const float x=m.mul(look.x,state.previousLook1C0.x),
        y=m.mul(look.y,state.previousLook1C0.y),z=m.mul(look.z,state.previousLook1C0.z);
    const float one=m.add(0,1),dot=m.add(m.add(x,y),m.mul(one,z));
    float angle=m.angle(dot);if(angle<0)angle=-angle;next.angle244=angle;
    std::memcpy(&next.previousLook1C0,&look,sizeof look);
    if(!m.ok)return S::arithmetic;
    if(flags&4u){state=next;return S::skippedBySceneFlag;}
    if(next.countdown210) --next.countdown210;
    if(next.countdown20C) --next.countdown20C;
    state=next;return S::needsMatrixVisibilityAndChildren;
}
}

#include "tv_camera_update49f.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Vector=assets::TVCameraVector49E;
bool finite(float value) noexcept {
    std::uint32_t bits;std::memcpy(&bits,&value,4);
    return (bits&0x7f800000u)!=0x7f800000u;
}
struct Arithmetic {
    bool valid=true;
    bool inputs(float a,float b) noexcept {
        valid=valid&&finite(a)&&finite(b);return valid;
    }
    float keep(float value) noexcept {valid=valid&&finite(value);return value;}
    float add(float a,float b) noexcept {
        if(!inputs(a,b))return 0;
        volatile float x=a,y=b;volatile float r=x+y;return keep(r);
    }
    float sub(float a,float b) noexcept {
        if(!inputs(a,b))return 0;
        volatile float x=a,y=b;volatile float r=x-y;return keep(r);
    }
    float mul(float a,float b) noexcept {
        if(!inputs(a,b))return 0;
        volatile float x=a,y=b;volatile float r=x*y;return keep(r);
    }
    float div(float a,float b) noexcept {
        if(!inputs(a,b)||b==0){valid=false;return 0;}
        volatile float x=a,y=b;volatile float r=x/y;return keep(r);
    }
    float root(float value) noexcept {
        if(!inputs(value,0)||value<0){valid=false;return 0;}
        volatile float x=value;volatile float r=std::sqrt(x);return keep(r);
    }
};
void copyVector(Vector& destination,const Vector& source) noexcept {
    std::memcpy(&destination,&source,sizeof(Vector));
}
float dotSquared(Arithmetic& f,float x,float y,float z) noexcept {
    const float xx=f.mul(x,x),yy=f.mul(y,y);
    const float accumulator=f.add(xx,yy),zz=f.mul(z,z);
    return f.add(accumulator,zz);
}
bool rawFov(const assets::TVCameraRecord49E& record,bool stationary,
            const Vector& position,const Vector& target,float& result) noexcept {
    const float lo=record.min_fov,hi=record.max_fov;
    if(!finite(lo)||!finite(hi))return false;
    if(lo==hi){result=lo;return true;}
    if(stationary){result=hi;return true;}
    Arithmetic f;
    const float dx=f.sub(position.x,target.x),dz=f.sub(position.z,target.z);
    const float xx=f.mul(dx,dx),zz=f.mul(dz,dz);
    const float distance=f.root(f.add(xx,zz));
    const float denominator=f.sub(record.max_fov_distance,record.min_fov_distance);
    const float range=f.sub(hi,lo),remaining=f.sub(record.max_fov_distance,distance);
    const float factor=f.div(remaining,denominator);
    const float increment=f.mul(range,factor);
    float candidate=f.add(lo,increment);
    if(!f.valid)return false;
    if(candidate<lo)candidate=lo;
    if(!(candidate<=hi))candidate=hi;
    result=candidate;return true;
}
bool triggerSceneShake(const assets::TVCameraSpan49E<const TVCameraSceneSample49F>& samples,
                       TVCameraSelectorState49E& state) noexcept {
    for(std::size_t i=0;i<samples.size;++i){
        const auto& sample=samples[i];
        if(sample.inactive40!=0)continue;
        Arithmetic f;
        const float speed=f.root(dotSquared(f,sample.velocity.x,sample.velocity.y,sample.velocity.z));
        const float dx=f.sub(state.positionA0.x,sample.localPosition.x);
        const float dy=f.sub(state.positionA0.y,sample.localPosition.y);
        const float dz=f.sub(state.positionA0.z,sample.localPosition.z);
        const float squaredDistance=dotSquared(f,dx,dy,dz);
        if(!f.valid)return false;
        if(!(squaredDistance<=3025.0f)||speed<170.0f||
           static_cast<std::int32_t>(i)==state.sourceD0)continue;
        const float distanceRatio=f.div(squaredDistance,3025.0f);
        const float speedExcess=f.sub(speed,170.0f);
        const float speedRatio=f.div(speedExcess,170.0f);
        const float proximity=f.sub(1.0f,distanceRatio);
        const float factor=f.mul(speedRatio,proximity);
        const float intensity=detail::scalarMin48O(1.0f,factor);
        state.fieldC0=f.mul(0.45f,intensity);
        state.fieldC4=0;
        state.wordCC=1;
        state.fieldC8=f.mul(14.0f,intensity);
        state.sourceD0=static_cast<std::int32_t>(i);
        return f.valid;
    }
    return true;
}
bool transformShake(TVCameraSelectorState49E& shared,TVCameraUpdateState49F& state,
                    float elapsed,TVCameraRandom49E& random,Vector& position) noexcept {
    if(shared.wordCC==0)return true;
    if(!finite(state.shakeVectorB0.x))return false;
    Arithmetic f;
    if(state.shakeVectorB0.x!=0){
        copyVector(position,shared.positionA0);
        state.shakeVectorB0.x=0;
    }else{
        float randomXYZ[3];
        for(float& value:randomXYZ){
            const auto word=nextTVCameraRandom49E(random);
            volatile std::int32_t signedWord=static_cast<std::int32_t>(word);
            volatile float converted=static_cast<float>(signedWord);
            value=f.mul(0x1p-31f,converted);
        }
        const float squared=dotSquared(f,randomXYZ[0],randomXYZ[1],randomXYZ[2]);
        const float q=f.div(1.0f,f.root(squared));
        const float nx=f.mul(randomXYZ[0],q),ny=f.mul(randomXYZ[1],q),nz=f.mul(randomXYZ[2],q);
        const float ratio=f.div(shared.fieldC4,shared.fieldC0);
        const float fade=f.sub(1.0f,ratio);
        const float ix=f.mul(fade,nx),iy=f.mul(fade,ny),iz=f.mul(fade,nz);
        const float sx=f.mul(shared.fieldC8,ix),sy=f.mul(shared.fieldC8,iy),sz=f.mul(shared.fieldC8,iz);
        state.shakeVectorB0={sx,sy,sz,1};
        position.x=f.add(position.x,sx);
        position.y=f.add(position.y,sy);
        position.z=f.add(position.z,sz);
    }
    shared.fieldC4=f.add(shared.fieldC4,elapsed);
    if(!f.valid||!finite(shared.fieldC0))return false;
    if(!(shared.fieldC4<shared.fieldC0)){
        shared.fieldC0=0;shared.fieldC4=0;shared.wordCC=0;
        shared.fieldC8=0;shared.sourceD0=-1;
    }
    return true;
}
}
bool updateTVCamera49F(const assets::TVCameraView49E& data,
    TVCameraSelectorState49E& shared,TVCameraUpdateState49F& state,
    const TVCameraUpdateInput49F& input,TVCameraRandom49E& random,
    TVCameraUpdateOutput49F& output) noexcept {
    const auto cameras=data.cameras();
    if(std::fegetround()!=FE_TONEAREST||!data.loaded()||cameras.empty()||shared.current40<0||
       static_cast<std::uint32_t>(shared.current40)>=cameras.size||
       !finite(input.elapsed)||input.sceneSamples.size>
       static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
       (input.sceneSamples.size&&!input.sceneSamples.data))return false;
    TVCameraSelectorState49E nextShared;
    TVCameraUpdateState49F nextState;
    TVCameraUpdateOutput49F nextOutput;
    std::memcpy(&nextShared,&shared,sizeof(shared));
    std::memcpy(&nextState,&state,sizeof(state));
    std::memcpy(&nextOutput,&output,sizeof(output));
    auto nextRandom=random;
    const auto& selected=cameras[static_cast<std::uint32_t>(shared.current40)];
    // Body-attached2D2170:28B1A0 writes XYZ only; body+70 copies all128 bits.
    nextState.playerPosition60.x=input.resolvedPlayerPosition.x;
    nextState.playerPosition60.y=input.resolvedPlayerPosition.y;
    nextState.playerPosition60.z=input.resolvedPlayerPosition.z;
    copyVector(nextState.playerVelocity70,input.bodyVelocity);
    Vector target;copyVector(target,nextState.playerPosition60);
    Arithmetic f;
    target.y=f.add(target.y,input.targetYOffset);
    if(!f.valid)return false;
    copyVector(nextState.cameraPosition10,selected.position);
    if(!rawFov(selected,shared.stationary80!=0,nextState.cameraPosition10,target,
               nextState.rawFov04))return false;
    copyVector(nextOutput.position,nextState.cameraPosition10);
    nextOutput.fov=nextState.rawFov04;
    if(shared.stationary80!=0){
        copyVector(nextOutput.look,selected.look);copyVector(nextOutput.up,selected.up);
        nextOutput.mode=0;
    }else{
        copyVector(nextOutput.target,target);nextOutput.mode=1;
    }
    if(!triggerSceneShake(input.sceneSamples,nextShared)||
       !transformShake(nextShared,nextState,input.elapsed,nextRandom,nextOutput.position))return false;
    // Source MIN.S then DIV.S; no lower clamp, no use of the filter extrema.
    if(!finite(nextState.filterTau04))return false;
    const float numerator=detail::scalarMin48O(input.elapsed,nextState.filterTau04);
    nextState.filterBlend08=f.div(numerator,nextState.filterTau04);
    const float error=f.sub(nextOutput.fov,nextShared.filterCurrent00);
    const float increment=f.mul(nextState.filterBlend08,error);
    nextShared.filterCurrent00=f.add(nextShared.filterCurrent00,increment);
    nextOutput.fov=nextShared.filterCurrent00;
    if(!f.valid)return false;
    std::memcpy(&shared,&nextShared,sizeof(shared));
    std::memcpy(&state,&nextState,sizeof(state));
    random=nextRandom;
    std::memcpy(&output,&nextOutput,sizeof(output));
    return true;
}
}

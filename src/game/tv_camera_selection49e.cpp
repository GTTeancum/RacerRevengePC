#include "tv_camera_selection49e.h"
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
bool finite(float value) noexcept {
    std::uint32_t bits;std::memcpy(&bits,&value,4);
    return (bits&0x7f800000u)!=0x7f800000u;
}
struct Arithmetic {
    bool valid=true;
    float keep(float v) noexcept {valid=valid&&finite(v);return v;}
    float sub(float a,float b) noexcept {volatile float x=a,y=b;volatile float v=x-y;return keep(v);}
    float add(float a,float b) noexcept {volatile float x=a,y=b;volatile float v=x+y;return keep(v);}
    float mul(float a,float b) noexcept {volatile float x=a,y=b;volatile float v=x*y;return keep(v);}
};
bool distance(const assets::TVCameraVector49E& a,const assets::TVCameraVector49E& b,
              float& out) noexcept {
    if(!finite(a.x)||!finite(a.y)||!finite(a.z))return false;
    Arithmetic f;
    // Original execution order is Y, Z, X, with player minus camera.
    const float dy=f.sub(b.y,a.y),dz=f.sub(b.z,a.z),dx=f.sub(b.x,a.x);
    const float xx=f.mul(dx,dx),yy=f.mul(dy,dy);
    const float accumulator=f.add(xx,yy);
    const float zz=f.mul(dz,dz);
    out=f.add(accumulator,zz);
    return f.valid&&out<std::numeric_limits<float>::max();
}
}
void seedTVCameraRandom49E(TVCameraRandom49E& state,std::uint32_t seed) noexcept {
    state.raw=seed;
}
std::uint32_t nextTVCameraRandom49E(TVCameraRandom49E& state) noexcept {
    state.raw=state.raw*0x41c64e6du+0x3039u;
    return state.raw&0x7fffffffu;
}
void resetTVCameraSelector49E(TVCameraSelectorState49E& state) noexcept {
    state.current40=0;
    state.hold90=0.6f;
}
bool selectTVCamera49E(const assets::TVCameraView49E& data,TVCameraSelectorState49E& state,
    const assets::TVCameraVector49E& player,float elapsed,
    TVCameraRandom49E& random,TVCameraSelection49E& output) noexcept {
    const auto cameras=data.cameras();
    if(!data.loaded()||cameras.empty()||!finite(player.x)||!finite(player.y)||
       !finite(player.z)||!finite(elapsed))return false;
    TVCameraSelectorState49E result;
    std::memcpy(&result,&state,sizeof(result));
    auto rng=random;
    bool changed=false;
    std::uint32_t desired=0;
    float best=std::numeric_limits<float>::max();
    if(state.current40<0){
        for(std::uint32_t i=0;i<cameras.size;++i){
            float d;
            if(!distance(cameras[i].position,player,d))return false;
            if(d<best){best=d;desired=i;}
        }
        result.previous44=state.current40;
        result.current40=static_cast<std::int32_t>(desired);
        result.hold90=0.6f;
        changed=true;
    }else{
        const auto current=static_cast<std::uint32_t>(state.current40);
        if(current>=cameras.size)return false;
        desired=current;
        const auto links=data.nextCameras(current);
        for(const auto index:links){
            if(index>=cameras.size)return false;
            float d;
            if(!distance(cameras[index].position,player,d))return false;
            if(d<best){best=d;desired=index;}
        }
        float currentDistance;
        if(!distance(cameras[current].position,player,currentDistance))return false;
        if(currentDistance<best)desired=current;
        if(desired!=current){
            if(!finite(result.hold90))return false;
            if(result.hold90>0){
                Arithmetic f;result.hold90=f.sub(result.hold90,elapsed);
                if(!f.valid)return false;
            }else{
                result.hold90=0.6f;
                result.previous44=state.current40;
                result.current40=static_cast<std::int32_t>(desired);
                changed=true;
            }
        }
    }
    const auto& selected=cameras[static_cast<std::uint32_t>(result.current40)];
    std::memcpy(&result.positionA0,&selected.position,sizeof(result.positionA0));
    TVCameraSelection49E selection{};
    std::memcpy(&selection.position,&selected.position,sizeof(selection.position));
    selection.changed=changed;
    if(changed){
        if(!finite(selected.max_fov)||!finite(selected.stationary))return false;
        result.fieldC0=0;
        result.fieldC4=0;
        result.wordCC=0;
        result.fieldC8=0;
        result.sourceD0=-1;
        result.filterCurrent00=selected.max_fov;
        const auto word=nextTVCameraRandom49E(rng);
        volatile std::int32_t signedWord=static_cast<std::int32_t>(word);
        volatile float converted=static_cast<float>(signedWord);
        Arithmetic f;
        const float scaled=f.mul(0x1p-31f,converted);
        const float percent=f.mul(100.0f,scaled);
        if(!f.valid)return false;
        result.stationary80=percent<selected.stationary?1:0;
    }
    std::memcpy(&state,&result,sizeof(state));
    random=rng;
    std::memcpy(&output,&selection,sizeof(output));
    return true;
}
}

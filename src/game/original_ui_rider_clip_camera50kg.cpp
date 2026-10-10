#include "original_ui_rider_clip_camera50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool finite(const std::array<float,16>& m) noexcept {for(float f:m)if(!std::isfinite(f))return false;return true;}
}
bool prepareOriginalUiRiderClipCamera50KG(const OriginalUiRiderCamera50KG& camera,
    OriginalUiRiderClipCamera50KG& output) noexcept {
    if(!camera.pageIdentity||std::fegetround()!=FE_TONEAREST||
        !finite(camera.core.view)||!finite(camera.core.projection)||
        !finite(camera.sourceScreen.sourceScreen140))return false;
    const auto& screen=camera.sourceScreen.sourceScreen140;
    const float x=std::fabs(screen[0]),y=std::fabs(screen[5]);
    const float edgeX=add(screen[12],x),edgeY=add(screen[13],y);
    const float roomX=std::min(edgeX,sub(4095,edgeX));
    const float roomY=std::min(edgeY,sub(4095,edgeY));
    if(!(x>0&&y>0&&roomX>0&&roomY>0))return false;
    OriginalUiRiderClipCamera50KG next{};
    next.guardScale334=div(x,roomX);next.guardScale338=div(y,roomY);
    next.clipProjection290=camera.core.projection;
    for(unsigned row=0;row<4;++row){
        next.clipProjection290[4*row]=mul(next.clipProjection290[4*row],next.guardScale334);
        next.clipProjection290[4*row+1]=mul(next.clipProjection290[4*row+1],next.guardScale338);
    }
    const auto& a=camera.core.view;const auto& b=next.clipProjection290;
    for(unsigned row=0;row<4;++row)for(unsigned col=0;col<4;++col){
        float value=mul(b[col],a[4*row]);
        value=add(value,mul(b[4+col],a[4*row+1]));
        value=add(value,mul(b[8+col],a[4*row+2]));
        next.combinedClip2D0[4*row+col]=add(value,mul(b[12+col],a[4*row+3]));
    }
    if(!finite(next.clipProjection290)||!finite(next.combinedClip2D0))return false;
    output=next;return true;
}
}

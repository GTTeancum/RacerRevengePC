#pragma once
// Source-only PositionStats numeric subset. UNTESTED; no owner, resource,
// snapshot, scheduling, geometry or renderer readiness is established here.
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cmath>
namespace rr::game::position_stats_detail50LA {
struct LayoutOffsets {float x=0,y=0;};
//191290..1912E8: only the source mode0/1/2 and one-based view1/2 domain.
//These are owner32B0/32B4, NOT a complete layout or a viewport transform.
inline bool layoutOffsets191290(std::int32_t mode,std::uint32_t oneBasedView,
    LayoutOffsets& out)noexcept {
    if(mode<0||mode>2||oneBasedView<1||oneBasedView>2)return false;
    LayoutOffsets next;
    if(mode==1)next.y=oneBasedView==1?0.f:-27.f;
    else if(mode==2)next.x=oneBasedView==1?-300.f:-320.f;
    out=next;return true;
}
struct Visibility {
    bool position=false,countCD0=false,map=false;
};
//Actual preference10F9C[view], common manager[view].state40, and11258.
//Closed original preference0..7 domain. No class/selected-racer inference.
//17FB90: {1,2,3,7};17FC30: {2,4,5,7};17FCD0: {3,5,6,7}.
//Only the first two reject subtype3. Output remains unchanged on rejection.
inline bool preferenceVisibility17FB90(std::uint32_t preference10F9C,
    std::uint32_t actualViewState40,std::uint32_t subtype11258,Visibility& out)noexcept {
    if(preference10F9C>7)return false;
    const auto p=preference10F9C;Visibility next;
    if(actualViewState40==0){
        next.position=subtype11258!=3&&(p==1||p==2||p==3||p==7);
        next.countCD0=subtype11258!=3&&(p==2||p==4||p==5||p==7);
        next.map=p==3||p>=5;
    }
    out=next;return true;
}
//191BE0 wrapper, before preference routines or child Entity141D70. This is
//an own-leaf visibility observation, never permission to submit geometry.
inline bool ownLeafVisible191BE0(std::uint32_t actualApplication268,
    std::uint8_t race1B0,std::uint8_t raceEntity29,std::uint8_t ui1E4,
    bool& out)noexcept {
    if(actualApplication268>1)return false;
    out=!(actualApplication268==1&&(race1B0&3u)<2u)&&
        !(race1B0&0x10u)&&!(raceEntity29&4u)&&!(ui1E4&1u);
    return true;
}
//191DF0..191E00: LB(cameraA8), DSLL32/DSRA32 sign-extend LOW NIBBLE.
//This is not the camera's followed-racer key or Application268 view index.
inline std::int32_t signedCameraIndex191DF0(std::uint8_t actualCameraA8)noexcept {
    const std::int32_t low=actualCameraA8&15u;
    return low<8?low:low-16;
}
//Caller must resolve the actual same-session managerB4->3C[index] record
//using signedCameraIndex191DF0 and real bounds BEFORE observing placeCCC.
//No ranking computation, sorting, +1 adjustment or copied host position exists
//here. Actual manager50 is active count, not total count30 or a guessed eight.
struct PositionNumbers {std::int32_t placeCCC=0;std::uint32_t activeCount50=0;};
inline bool observePositionNumbers191F00(std::int32_t actualRecord3C,
    std::uint32_t actualActiveCount50,std::size_t actualManagerCount30,
    PositionNumbers& out)noexcept {
    if(actualManagerCount30>8||actualActiveCount50>actualManagerCount30)return false;
    out={actualRecord3C,actualActiveCount50};return true;
}

//19252C/192538 call original13BBA0 then13BAD0 on actual body+E4.
//Separate native binary32 polynomial operations; no host trigonometry, VU
//runtime or angle inferred from a camera/route direction. Finite heading in
//[-pi,pi] is the bounded native domain. Output is unchanged on rejection.
inline bool radarHeading1924C0(float heading,float& cosine,float& sine)noexcept {
    const auto fp=[](std::uint32_t v)noexcept{return std::bit_cast<float>(v);};
    if(!std::isfinite(heading)||heading<fp(0xc0490fdbu)||heading>fp(0x40490fdbu))return false;
    const auto add=[](float a,float b)noexcept{volatile float r=a+b;return r;};
    const auto sub=[](float a,float b)noexcept{volatile float r=a-b;return r;};
    const auto mul=[](float a,float b)noexcept{volatile float r=a*b;return r;};
    float x=heading,sign=1.f;
    if(x>fp(0x3fc90fdbu)){x=sub(fp(0x40490fdbu),x);sign=-1.f;}
    else if(x<fp(0xbfc90fdbu)){x=sub(fp(0xc0490fdbu),x);sign=-1.f;}
    const float square=mul(x,x);
    float c=add(fp(0x37d00d01u),mul(fp(0xb493f27eu),square));
    c=add(fp(0xbab60b61u),mul(square,c));c=add(fp(0x3d2aaaabu),mul(square,c));
    c=add(-.5f,mul(square,c));c=mul(sign,add(1.f,mul(square,c)));
    float s=add(fp(0xb9500d01u),mul(fp(0x3638ef1du),square));
    s=add(fp(0x3c088889u),mul(square,s));s=add(fp(0xbe2aaaabu),mul(square,s));
    s=add(x,mul(mul(x,square),s));
    if(!std::isfinite(c)||!std::isfinite(s))return false;
    cosine=c;sine=s;return true;
}
//1922B0: native scalar projection of an actual world point into the existing
//PositionStats radar. Inputs are observations of owner32C0/32C8,32D4/32D8,
//32DC and32E4/32E8, not a replacement camera, route or racer driver.
struct RadarProjectionInputs {
    float centerX=0,centerZ=0,cosine=0,sine=0,scale=0,screenX=0,screenY=0;
};
struct RadarProjectedPoint {
    float x=0,y=0;
    std::uint32_t alpha=0;
    bool outside=false;
};
inline bool projectRadarPoint1922B0(const RadarProjectionInputs& p,
    float worldX,float worldZ,RadarProjectedPoint& output)noexcept {
    const float inputs[]={p.centerX,p.centerZ,p.cosine,p.sine,p.scale,
        p.screenX,p.screenY,worldX,worldZ};
    for(float value:inputs)if(!std::isfinite(value))return false;
    // Keep source single-precision multiply/add boundaries separate. These
    // native arithmetic operations do not execute source instructions.
    const auto add=[](float a,float b)noexcept{volatile float r=a+b;return r;};
    const auto sub=[](float a,float b)noexcept{volatile float r=a-b;return r;};
    const auto mul=[](float a,float b)noexcept{volatile float r=a*b;return r;};
    const float dx=sub(worldX,p.centerX),dz=sub(worldZ,p.centerZ);
    const float rotatedX=add(mul(dx,p.cosine),mul(dz,p.sine));
    const float rotatedY=-add(-mul(dx,p.sine),mul(dz,p.cosine));
    const float x=mul(p.scale,rotatedX),y=mul(p.scale,rotatedY);
    if(!std::isfinite(x)||!std::isfinite(y))return false;
    const auto outside=[&](float halfWidth,float top,float bottom)noexcept{
        return x>halfWidth||x< -halfWidth||y>top||y<bottom;
    };
    RadarProjectedPoint next;
    next.outside=outside(45.f,21.f,-57.f);
    if(!next.outside){
        next.alpha=255;
        if(outside(39.f,15.f,-51.f)){
            next.alpha=192;
            if(outside(41.f,17.f,-53.f)){
                next.alpha=128;
                if(outside(43.f,19.f,-55.f))next.alpha=64;
            }
        }
    }
    //The original writes projected coordinates even for an outside point;
    //the caller may still clip its segment against the same radar bounds.
    next.x=add(x,p.screenX);next.y=add(y,p.screenY);
    if(!std::isfinite(next.x)||!std::isfinite(next.y))return false;
    output=next;return true;
}
} //namespace rr::game::position_stats_detail50LA

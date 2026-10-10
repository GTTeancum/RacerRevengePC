#include "collision_type3_node48p.h"
#include <cmath>

namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool finite(const Vector3_48C& v) noexcept {
    return std::isfinite(v.v[0])&&std::isfinite(v.v[1])&&std::isfinite(v.v[2]);
}
bool validBox(const Vector3_48C& center,const Vector3_48C& half) noexcept {
    return finite(center)&&finite(half)&&half.v[0]>=0.f&&half.v[1]>=0.f&&half.v[2]>=0.f;
}
bool finiteXYZ(const SpringVector4_48N& v) noexcept {
    return std::isfinite(v.v[0])&&std::isfinite(v.v[1])&&std::isfinite(v.v[2]);
}
}
Type3NodeStatus48P type3SphereBox48P(const Vector3_48C& localCenter,float radius,
    const Vector3_48C& boxCenter,const Vector3_48C& boxHalf) noexcept {
    if(!finite(localCenter)||!std::isfinite(radius)||radius<0.f||!validBox(boxCenter,boxHalf))
        return Type3NodeStatus48P::InvalidInput;
    const float delta[3]={sub(boxCenter.v[0],localCenter.v[0]),
                          sub(boxCenter.v[1],localCenter.v[1]),
                          sub(boxCenter.v[2],localCenter.v[2])};
    for(unsigned axis=0;axis<3;++axis)
        if(!(std::fabs(delta[axis])<=add(boxHalf.v[axis],radius)))return Type3NodeStatus48P::Separated;
    return Type3NodeStatus48P::Overlap;
}
Type3NodeStatus48P type3RayBox48P(const Type3Segment48P& segment,
    const Vector3_48C& boxCenter,const Vector3_48C& boxHalf) noexcept {
    if(!finiteXYZ(segment.start)||!finiteXYZ(segment.direction)||!std::isfinite(segment.length)||
       !validBox(boxCenter,boxHalf))return Type3NodeStatus48P::InvalidInput;
    const float delta[3]={sub(boxCenter.v[0],segment.start.v[0]),
                          sub(boxCenter.v[1],segment.start.v[1]),
                          sub(boxCenter.v[2],segment.start.v[2])};
    if(std::fabs(delta[0])<=boxHalf.v[0]&&std::fabs(delta[1])<=boxHalf.v[1]&&
       std::fabs(delta[2])<=boxHalf.v[2])return Type3NodeStatus48P::Overlap;
    // Each row is normal axis, first transverse axis, second transverse axis.
    // This also preserves Y-before-Z for X, Y-before-X for Z, Z-before-X for Y.
    constexpr unsigned axes[3][3]={{0,1,2},{2,1,0},{1,2,0}};
    for(const auto& axesForFace:axes) {
        const unsigned axis=axesForFace[0];
        for(unsigned side=0;side<2;++side) {
            const float face=side==0 ? add(boxCenter.v[axis],boxHalf.v[axis])
                                     : sub(boxCenter.v[axis],boxHalf.v[axis]);
            const float time=div(sub(face,segment.start.v[axis]),segment.direction.v[axis]);
            // 10EF60/74 and corresponding faces. The negated <= matters for NaN.
            if(time<0.f||!(time<=segment.length))continue;
            const unsigned first=axesForFace[1],second=axesForFace[2];
            const float firstDelta=sub(add(segment.start.v[first],mul(time,segment.direction.v[first])),boxCenter.v[first]);
            if(!(std::fabs(firstDelta)<=boxHalf.v[first]))continue;
            const float secondDelta=sub(add(segment.start.v[second],mul(time,segment.direction.v[second])),boxCenter.v[second]);
            if(!(std::fabs(secondDelta)<=boxHalf.v[second]))continue;
            return Type3NodeStatus48P::Overlap;
        }
    }
    return Type3NodeStatus48P::Separated;
}
}

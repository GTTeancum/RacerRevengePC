#pragma once
// Source-only numeric+3220 constructor/setter subset. UNTESTED, uncalled.
// No clipping119FC0, primitive/material owner, or renderer readiness.
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
namespace rr::game {
class OriginalPositionMapBounds50LA final {
    //119E50 -> eight1048E0 vector constructors (SQC2 vf0, W=1), followed by
    //explicit corner stores. The repeated store+88 does NOT initialize+8C.
    std::array<std::array<float,4>,8> corners_{};
    float minX_=0,minY_=0,maxX_=0;
    float maxYBacking_=0; //Native backing only; source value unavailable until set.
    bool set_=false;
public:
    OriginalPositionMapBounds50LA()noexcept {
        const float lo=std::bit_cast<float>(std::uint32_t{0xff7fffff});
        const float hi=std::bit_cast<float>(std::uint32_t{0x7f7fffff});
        minX_=minY_=lo;maxX_=hi;
        for(std::size_t i=0;i<8;++i){
            const auto n=i&3u;
            corners_[i][0]=(n==1||n==2)?hi:lo;
            corners_[i][1]=(n==2||n==3)?hi:lo;
            corners_[i][2]=lo; corners_[i][3]=1.f; //Retained original vf0 W.
        }
    }
    //119F60 stores f12/f13/f14/f15 at80/84/88/8C; updates first four X/Y,
    //then copies all four lanes into corners4..7. No Z/W initialization here.
    //Bounded native domain only; rejection is not a claim of retail validation.
    bool set119F60(float minX,float minY,float maxX,float maxY)noexcept {
        for(float v:{minX,minY,maxX,maxY})if(!std::isfinite(v))return false;
        if(minX>maxX||minY>maxY)return false;
        minX_=minX;minY_=minY;maxX_=maxX;maxYBacking_=maxY;
        corners_[0][0]=corners_[3][0]=minX;
        corners_[1][0]=corners_[2][0]=maxX;
        corners_[0][1]=corners_[1][1]=minY;
        corners_[2][1]=corners_[3][1]=maxY;
        for(std::size_t i=0;i<4;++i)corners_[i+4]=corners_[i];
        set_=true;return true;
    }
    //No consumer may treat constructor-only+8C as zero/known bounds.
    bool observe(std::array<float,4>& bounds,
        std::array<std::array<float,4>,8>& corners)const noexcept {
        if(!set_)return false;
        bounds={{minX_,minY_,maxX_,maxYBacking_}};corners=corners_;return true;
    }
};
}

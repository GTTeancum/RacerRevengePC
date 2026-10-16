#pragma once
// Uncalled original +3220 numeric owner. No layout, GPU/resource readiness,
// full PositionStats Entity or original hierarchy is supplied by this type.
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
namespace rr::game {
class OriginalPositionStatsClip50LA final {
public:
    using Vector=std::array<float,4>;
    using Segment=std::array<Vector,2>;
    enum class Result:std::uint8_t {accepted,rejected,unsetBounds,outsideNativeDomain,iterationLimit};
private:
    std::array<Vector,8> corners_{};
    std::array<float,4> bounds_{}; //+80,+84,+88,+8C; last unavailable until setter.
    bool set_=false;
    // Preserve separate source single-precision operations; no fused expression.
    static float sub(float a,float b)noexcept {volatile float r=a-b;return r;}
    static float mul(float a,float b)noexcept {volatile float r=a*b;return r;}
    static float div(float a,float b)noexcept {volatile float r=a/b;return r;}
    static float add(float a,float b)noexcept {volatile float r=a+b;return r;}
    unsigned code(const Vector& p)const noexcept {
        unsigned bits=0;
        if(p[1]>bounds_[3])bits=8;else if(p[1]<bounds_[1])bits=4;
        if(p[0]>bounds_[2])bits|=2;else if(p[0]<bounds_[0])bits|=1;
        return bits;
    }
    static bool finiteXYZ(const Vector& p)noexcept {
        return std::isfinite(p[0])&&std::isfinite(p[1])&&std::isfinite(p[2]);
    }
    static void copyXYZ(const Segment& from,Segment& to)noexcept {
        for(std::size_t i=0;i<2;++i)for(std::size_t j=0;j<3;++j)to[i][j]=from[i][j];
    }
public:
    OriginalPositionStatsClip50LA()noexcept {
        const float lo=std::bit_cast<float>(std::uint32_t{0xff7fffff});
        const float hi=std::bit_cast<float>(std::uint32_t{0x7f7fffff});
        //119E50 calls eight1048E0 constructors: SQC2 vf0, NOT integer SQ zero.
        //Explicit stores set Z to lo; W retains vf0's 1. +88 is stored twice;
        //+8C has no source constructor store. set_ keeps it unavailable.
        for(std::size_t i=0;i<8;++i){const auto n=i&3u;
            corners_[i]={{(n==1||n==2)?hi:lo,(n==2||n==3)?hi:lo,lo,1.f}};
        }
        bounds_[0]=bounds_[1]=lo;bounds_[2]=hi;
    }
    bool set119F60(float x0,float y0,float x1,float y1)noexcept {
        //Explicit bounded native admission, not a retail validation claim.
        if(!std::isfinite(x0)||!std::isfinite(y0)||!std::isfinite(x1)||!std::isfinite(y1)||x0>x1||y0>y1)return false;
        bounds_={{x0,y0,x1,y1}};
        corners_[0][0]=corners_[3][0]=x0;corners_[1][0]=corners_[2][0]=x1;
        corners_[0][1]=corners_[1][1]=y0;corners_[2][1]=corners_[3][1]=y1;
        for(std::size_t i=0;i<4;++i)corners_[i+4]=corners_[i];
        set_=true;return true;
    }
    bool observe(std::array<float,4>& bounds,std::array<Vector,8>& corners)const noexcept {
        if(!set_)return false;bounds=bounds_;corners=corners_;return true;
    }
    Result clip119FC0(const Segment& input,Segment& output)const noexcept {
        if(!set_)return Result::unsetBounds;
        //W is neither tested nor written by119FC0. Finite XYZ is the admitted
        //native domain; rejection/unsupported arithmetic preserve all output.
        if(!finiteXYZ(input[0])||!finiteXYZ(input[1]))return Result::outsideNativeDomain;
        const Segment original=input;
        auto candidate=original;
        unsigned a=code(original[0]),b=code(original[1]);
        if(!(a|b)){copyXYZ(original,output);return Result::accepted;}
        if(a&b)return Result::rejected;
        //The source reuses ORIGINAL endpoints/deltas for each intersection,
        //not the previously clipped pair. First endpoint has priority until
        //inside;119FC0 then processes the second. Horizontal edge has priority.
        for(unsigned attempt=0;attempt<8;++attempt){
            const unsigned selected=a?a:b;
            Vector& p=candidate[a?0:1];
            const float dx=sub(original[1][0],original[0][0]);
            const float dy=sub(original[1][1],original[0][1]);
            const float dz=sub(original[1][2],original[0][2]);
            if(!std::isfinite(dx)||!std::isfinite(dy)||!std::isfinite(dz))return Result::outsideNativeDomain;
            if(selected&12u){
                if(dy==0.f)return Result::outsideNativeDomain;
                p[1]=(selected&4u)?bounds_[1]:bounds_[3];
                const float delta=sub(p[1],original[0][1]);
                p[0]=add(original[0][0],div(mul(dx,delta),dy));
                p[2]=add(original[0][2],div(mul(dz,delta),dy));
            }else{
                if(dx==0.f)return Result::outsideNativeDomain;
                p[0]=(selected&1u)?bounds_[0]:bounds_[2];
                const float delta=sub(p[0],original[0][0]);
                p[1]=add(original[0][1],div(mul(dy,delta),dx));
                p[2]=add(original[0][2],div(mul(dz,delta),dx));
            }
            if(!finiteXYZ(p))return Result::outsideNativeDomain;
            if(a)a=code(p);else b=code(p);
            if(!(a|b)){copyXYZ(candidate,output);return Result::accepted;}
            if(a&b)return Result::rejected;
        }
        //Bounded native guard, not an invented original invisible result.
        return Result::iterationLimit;
    }
};
}

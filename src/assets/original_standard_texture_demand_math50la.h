#pragma once
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <limits>

namespace rr::assets::detail {
// Narrow native CPU arithmetic for24A430/104A00, not an instruction/FPU
// interpreter. Reject special/subnormal operands, overflow/underflow and
// inexact binary32 intermediates in the original exact-only operations. The
// separately named demand operations below narrow an exact binary64 result
// toward zero, never silently using host nearest-even. Neither path claims
// EE silicon low-bit equivalence: Sony section8.8 expressly permits differences.
// No other selection/renderer arithmetic is certified by this type.
struct StandardTextureDemandMath50LA {
    static bool ordinary(float x) noexcept {
        return std::isfinite(x)&&(x==0.f||std::fpclassify(x)==FP_NORMAL);
    }
    static bool supported() noexcept {
        static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
            std::numeric_limits<float>::digits==24,"binary32 demand input");
        static_assert(std::numeric_limits<double>::is_iec559&&
            std::numeric_limits<double>::digits>=53,"exact binary32 product backing");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
        return false;
#else
        return std::fegetround()==FE_TONEAREST;
#endif
    }
    static bool fromExact(double exact,float& out) noexcept {
        const double maximum=static_cast<double>(std::numeric_limits<float>::max());
        if(!std::isfinite(exact)||exact>maximum||exact< -maximum)return false;
        const float value=static_cast<float>(exact);
        if(!ordinary(value)||static_cast<double>(value)!=exact)return false;
        out=value;return true;
    }
    // Only24A430/104A00 use this bounded native directed-rounding path.
    // Exact ordinary results are unchanged. Exceptional/underflow/overflow
    // behavior remains unsupported; no source saturation or flush is invented.
    static bool fromTowardZero(double exact,float& out,const char** reason=nullptr) noexcept {
        const auto reject=[reason](const char* why) noexcept {
            if(reason)*reason=why;
            return false;
        };
        if(reason)*reason=nullptr;
        const double magnitude=std::abs(exact);
        if(!std::isfinite(exact))return reject("texture-demand nonfinite result");
        if(magnitude>static_cast<double>(std::numeric_limits<float>::max()))
            return reject("texture-demand overflow");
        if(exact!=0.0&&magnitude<static_cast<double>(std::numeric_limits<float>::min()))
            return reject("texture-demand underflow");
        // The caller proves exact binary64 backing and host nearest-even.
        // Force the narrowing before comparison; one adjacent binary32 step
        // suffices precisely when that narrowing rounded away from zero.
        volatile float narrowed=static_cast<float>(exact);
        float value=narrowed;
        if(std::abs(static_cast<double>(value))>magnitude)
            value=std::nextafter(value,0.f);
        if(!ordinary(value)||std::abs(static_cast<double>(value))>magnitude)
            return reject("texture-demand directed conversion");
        out=value;return true;
    }
    static bool multiplyTowardZero(float a,float b,float& out,const char** reason=nullptr) noexcept {
        if(reason)*reason=nullptr;
        if(!supported()){
            if(reason)*reason="texture-demand host mode";
            return false;
        }
        if(!ordinary(a)||!ordinary(b)){
            if(reason)*reason="texture-demand operand";
            return false;
        }
        // Two24-bit significands need at most48 bits, exact in binary64.
        return fromTowardZero(static_cast<double>(a)*static_cast<double>(b),out,reason);
    }
    static bool addTowardZero(float a,float b,float& out) noexcept {
        if(!supported()||!ordinary(a)||!ordinary(b))return false;
        // Preserve the existing bound: aligning at most29 binary places keeps
        // the exact sum within binary64, including cancellation. A zero term
        // is exact without an exponent comparison.
        if(a!=0.f&&b!=0.f&&std::abs(std::ilogb(a)-std::ilogb(b))>29)return false;
        return fromTowardZero(static_cast<double>(a)+static_cast<double>(b),out);
    }
    static bool subtractTowardZero(float a,float b,float& out) noexcept {
        return addTowardZero(a,-b,out);
    }
    static bool multiply(float a,float b,float& out) noexcept {
        if(!ordinary(a)||!ordinary(b))return false;
        // Two24-bit significands need at most48 bits, exact in binary64.
        return fromExact(static_cast<double>(a)*static_cast<double>(b),out);
    }
    static bool add(float a,float b,float& out) noexcept {
        if(!ordinary(a)||!ordinary(b))return false;
        // Reject large exponent separations before binary64 could swallow an
        //operand and make a false exactness claim. Zero is an exact operand.
        if(a!=0.f&&b!=0.f&&std::abs(std::ilogb(a)-std::ilogb(b))>29)return false;
        return fromExact(static_cast<double>(a)+static_cast<double>(b),out);
    }
    static bool subtract(float a,float b,float& out) noexcept {return add(a,-b,out);}
    static bool divideByPowerOfTwo(float a,float b,float& out) noexcept {
        if(!ordinary(a)||!ordinary(b)||b<=0.f)return false;
        int exponent=0;
        if(std::frexp(b,&exponent)!=.5f)return false;
        return fromExact(static_cast<double>(a)/static_cast<double>(b),out);
    }
};
}

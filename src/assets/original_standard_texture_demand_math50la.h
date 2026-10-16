#pragma once
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <limits>

namespace rr::assets::detail {
// Narrow native CPU arithmetic for24A430/104A00, not an instruction/FPU
// interpreter. Reject special/subnormal operands, overflow/underflow and
// inexact binary32 intermediates instead of silently using host nearest-even
// for a source that only supports toward-zero. Exactness admission avoids
// that known rounding mismatch; it is NOT proof of all EE hardware low-bit
// behavior. No other selection/renderer arithmetic is certified by this type.
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

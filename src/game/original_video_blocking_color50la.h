#pragma once
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game {
// Numeric inputs only, not an original Video, setter receipt or fade sample.
// A source owner must borrow its real3E4/3F4/3F8/3FC fields. A native caller
// may explicitly supply numbers without claiming any source invocation.
struct OriginalVideoFadeColorCoefficients50LA {
    std::uint32_t fromArgb=0;
    float redSlope=0,greenSlope=0,blueSlope=0;
};

// Pure numeric projection of25FAC0's positive-alpha-difference subset.
// This does NOT perform that source call, its assertion/global writes, or
// its ordered Video stores. In particular it never initializes source3E8.
// Native failure leaves output unchanged; it is not an original return value.
inline bool deriveOriginalVideoFadeColorCoefficients25FAC0(std::uint32_t fromArgb,
    std::uint32_t toArgb,OriginalVideoFadeColorCoefficients50LA& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    const auto deltaAlpha=std::int32_t((toArgb>>24)&255u)-std::int32_t((fromArgb>>24)&255u);
    if(deltaAlpha<=0)return false; // Explicit native admission, not the source assertion service.
    const auto difference=[&](unsigned shift) noexcept {
        return std::int32_t((toArgb>>shift)&255u)-std::int32_t((fromArgb>>shift)&255u);
    };
    //25FB64/84/BC/C0 conversions and25FB9C/C4/C8 divisions. Each
    // scalar step rounds separately under the existing native FP contract.
    volatile float divisor=static_cast<float>(deltaAlpha);
    volatile float redDelta=static_cast<float>(difference(16));
    volatile float red=redDelta/divisor;
    volatile float greenDelta=static_cast<float>(difference(8));
    volatile float blueDelta=static_cast<float>(difference(0));
    volatile float green=greenDelta/divisor;
    volatile float blue=blueDelta/divisor;
    if(!std::isfinite(red)||!std::isfinite(green)||!std::isfinite(blue))return false;
    output={fromArgb,red,green,blue};return true;
}

// Shared numeric body of2610D8..261250 and261998..261AAC. The caller
// supplies the start/current alpha; this function neither samples a clock
// nor advances, clamps or chooses those values. Both paths use FROM RGB
// and current-start, including the negative delta on the original IN path.
// Original start alpha is A(from) for OUT and A(to) for IN, but those choices
// belong to the actual blocking caller, not this arithmetic helper.
// Finite signed-int32 conversion is the admitted native EE CVT.W.S contract.
// Output remains unchanged on rejection. A value carries no source authority.
inline bool computeOriginalVideoBlockingColor50LA(const OriginalVideoFadeColorCoefficients50LA& input,
    std::int32_t startAlpha,std::int32_t currentAlpha,std::uint64_t& gsRgbaq) noexcept {
    if(std::fegetround()!=FE_TONEAREST || !std::isfinite(input.redSlope) ||
       !std::isfinite(input.greenSlope) || !std::isfinite(input.blueSlope))return false;
    // Original SUBU wraps at32 bits before signed CVT.S.W, without C++ UB.
    const std::uint32_t rawDelta=std::uint32_t(currentAlpha)-std::uint32_t(startAlpha);
    std::int32_t signedDelta;std::memcpy(&signedDelta,&rawDelta,sizeof(signedDelta));
    volatile float delta=static_cast<float>(signedDelta);
    // Preserve original operation order: all three MUL/ADD lanes precede
    // CVT.W.S red, green, blue. Volatile stores prevent contraction into FMA.
    volatile float redProduct=input.redSlope*delta;
    volatile float redValue=static_cast<float>((input.fromArgb>>16)&255u)+redProduct;
    volatile float greenProduct=input.greenSlope*delta;
    volatile float greenValue=static_cast<float>((input.fromArgb>>8)&255u)+greenProduct;
    volatile float blueProduct=input.blueSlope*delta;
    volatile float blueValue=static_cast<float>(input.fromArgb&255u)+blueProduct;
    if(!std::isfinite(redProduct)||!std::isfinite(greenProduct)||!std::isfinite(blueProduct))return false;
    const auto convert=[](float value,std::int32_t& out) noexcept {
        if(!std::isfinite(value)||double(value)<-2147483648.0||double(value)>=2147483648.0)return false;
        out=static_cast<std::int32_t>(value);return true;
    };
    std::int32_t red=0,green=0,blue=0;
    if(!convert(redValue,red)||!convert(greenValue,green)||!convert(blueValue,blue))return false;
    // Sign-extend each converted lane BEFORE unmasked64-bit OR packing.
    // Negative or oversized lanes may affect later lanes and the nominal Q.
    // Do not clamp, byte-mask, use ARGB packing or normalize alpha here.
    const auto lane=[](std::int32_t value) noexcept {
        return std::uint64_t(std::int64_t(value));
    };
    gsRgbaq=lane(red)|(lane(green)<<8)|(lane(blue)<<16)|(lane(currentAlpha)<<24)|
        (std::uint64_t(0x3f800000u)<<32);
    return true;
}
} // namespace rr::game

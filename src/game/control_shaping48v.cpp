#include "control_shaping48v.h"
#include <cstdint>
#include <cstring>

namespace rr::game {
namespace {
float fromBits(std::uint32_t bits) noexcept {
    float value; std::memcpy(&value, &bits, sizeof(value)); return value;
}
bool finite(float value) noexcept {
    std::uint32_t bits; std::memcpy(&bits, &value, sizeof(bits));
    return (bits & 0x7f800000u) != 0x7f800000u;
}
float add(float a, float b) noexcept { volatile float r = a + b; return r; }
float sub(float a, float b) noexcept { volatile float r = a - b; return r; }
float mul(float a, float b) noexcept { volatile float r = a * b; return r; }

// Original helper 0x13BBA0..0x13BCA0, restricted to its unreflected interval.
// raw in [-1,1] implies argument in [-float(pi/2),float(pi/2)] in all four host
// rounding modes. Preserve original Horner operand order and final multiply.
float originalCosine(float argument) noexcept {
    const float square = mul(argument, argument);
    float value = mul(fromBits(0xb493f27eu), square);
    value = add(fromBits(0x37d00d01u), value);
    float product = mul(square, value);
    value = add(fromBits(0xbab60b61u), product);
    value = mul(square, value);
    value = add(fromBits(0x3d2aaaabu), value);
    product = mul(square, value);
    value = add(-0.5f, product);
    value = mul(square, value);
    value = add(1.0f, value);
    return mul(1.0f, value);
}
}

bool shapeControlInputs48V(const ControlChannels48B& raw,
                          float previousSmoothedChannel0D28,
                          ControlShapeMode48V mode,
                          ControlShapingResult48V& out) noexcept {
    if (!finite(previousSmoothedChannel0D28) ||
        (mode != ControlShapeMode48V::Nonlinear && mode != ControlShapeMode48V::BypassNonlinear))
        return false;
    ControlChannels48B shaped{};
    for (unsigned i = 0; i != 4; ++i) {
        const float value = raw.values[i];
        if (!finite(value) || value < -1.0f || value > 1.0f) return false;
        if (mode == ControlShapeMode48V::Nonlinear) {
            const float cosine = originalCosine(mul(fromBits(0x3fc90fdbu), value));
            shaped.values[i] = value < 0.0f ? sub(cosine, 1.0f) : sub(1.0f, cosine);
        } else shaped.values[i] = value;
    }
    ControlShapingResult48V result{};
    // MULA.S then MADD.S: two separately rounded products, followed by addition.
    const float contribution = mul(fromBits(0x3dcccccdu), shaped.values[0]);
    const float history = mul(fromBits(0x3f666666u), previousSmoothedChannel0D28);
    result.smoothedChannel0D28 = add(contribution, history);
    if (!finite(result.smoothedChannel0D28)) return false;
    for (unsigned i = 0; i != 4; ++i) {
        const float scaled = mul(100.0f, shaped.values[i]);
        const float biased = add(0.5f, scaled);
        // CVT.W.S truncates toward zero. The bounded domain keeps this cast
        // safely within int32. Do not replace this asymmetric rule with round.
        const auto integer = static_cast<std::int32_t>(biased);
        volatile float integerAsFloat = static_cast<float>(integer);
        result.targets.values[i] = mul(fromBits(0x3c23d70au), integerAsFloat);
    }
    out = result;
    return true;
}

bool stepControlInputs48V(const ControlChannels48B& raw,
                         float previousSmoothedChannel0D28,
                         ControlShapeMode48V mode,
                         const ControlChannels48B& previousFiltered,
                         ControlRateLimits48B rates, float elapsedFactor,
                         ControlInputStep48V& out) noexcept {
    ControlInputStep48V result{};
    if (!shapeControlInputs48V(raw, previousSmoothedChannel0D28, mode, result.shaping) ||
        !stepControlRateFilter48B(result.shaping.targets, previousFiltered, rates,
                                 elapsedFactor, result.filtered)) return false;
    out = result;
    return true;
}
}

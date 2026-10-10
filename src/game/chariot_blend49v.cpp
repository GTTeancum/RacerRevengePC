#include "chariot_blend49v.h"
#include <cfenv>
#include <cmath>
#include <cstring>

#ifndef RR_CHARIOT_BLEND49V_ROW
#define RR_CHARIOT_BLEND49V_ROW(source, row) ((void)0)
#endif

namespace rr::game::detail {
namespace {
using V = VisualVector49R;
using M = BodyMatrix48G;
std::uint32_t bits(float x) noexcept {
    std::uint32_t w; std::memcpy(&w, &x, sizeof w); return w;
}
float fp(std::uint32_t w) noexcept {
    float x; std::memcpy(&x, &w, sizeof x); return x;
}
// The same separate binary32 and VRSQRT convention as49U's retained Math.
// The inline21BE00/20 sums have plain VADDZ, not a scalar EE RSQRT or FMA.
struct Math {
    bool valid = true;
    float read(float a) noexcept { valid = valid && std::isfinite(a); return a; }
    float add(float a, float b) noexcept { read(a); read(b); volatile float x = a + b; return read(x); }
    float sub(float a, float b) noexcept { read(a); read(b); volatile float x = a - b; return read(x); }
    float mul(float a, float b) noexcept { read(a); read(b); volatile float x = a * b; return read(x); }
    float div(float a, float b) noexcept { read(a); read(b); volatile float x = a / b; return read(x); }
    V normalize(const V& v) noexcept {
        const float xx = mul(v.v[0], v.v[0]);
        const float yy = mul(v.v[1], v.v[1]);
        const float zz = mul(v.v[2], v.v[2]);
        const float norm = add(add(xx, yy), zz);
        if (norm != 0.0f && (bits(norm) & 0x7f800000u) == 0) valid = false;
        float q;
        if (norm == 0.0f) q = fp((bits(norm) & 0x80000000u) | 0x7f7fffffu);
        else { volatile float root = std::sqrt(norm); q = div(1.0f, read(root)); }
        return V{{mul(v.v[0], q), mul(v.v[1], q), mul(v.v[2], q), 0.0f}};
    }
};
V row(const M& m, unsigned index, Math& f) noexcept {
    return V{{f.read(m.v[4 * index]), f.read(m.v[4 * index + 1]),
        f.read(m.v[4 * index + 2]), 1.0f}};
}
V cross(const V& a, const V& b, Math& f) noexcept {
    V result{};
    // VOPMULA.xyz followed by VOPMSUB.xyz; the second product has the
    // source's reversed operands. All products round before subtraction.
    const float products[3] = {f.mul(a.v[1], b.v[2]),
        f.mul(a.v[2], b.v[0]), f.mul(a.v[0], b.v[1])};
    for (unsigned i = 0; i < 3; ++i)
        result.v[i] = f.sub(products[i], f.mul(b.v[(i + 1) % 3], a.v[(i + 2) % 3]));
    return result;
}
float interpolate(float alpha, float first, float last, Math& f) noexcept {
    if (alpha <= 0.0f) return first;
    // Every upper-endpoint branch executes this SUB.S in its delay slot,
    // even when its subsequent MOV.S copies the physical endpoint.
    const float difference = f.sub(last, first);
    if (alpha < 1.0f) return f.add(first, f.mul(alpha, difference));
    return last;
}
}

bool blendChariotMatrix49V(const M& virtualLocal, const M& physicalCachedWorld,
    float oldElapsed84C, float positiveDuration848, M& output) noexcept {
    if (std::fegetround() != FE_TONEAREST || !std::isfinite(oldElapsed84C) ||
        !std::isfinite(positiveDuration848) || positiveDuration848 <= 0.0f) return false;
    Math f;
    RR_CHARIOT_BLEND49V_ROW(0u, 3u);
    const V virtualPosition = row(virtualLocal, 3, f);
    RR_CHARIOT_BLEND49V_ROW(0u, 2u);
    const V virtualForward = row(virtualLocal, 2, f);
    RR_CHARIOT_BLEND49V_ROW(0u, 1u);
    const V virtualUp = row(virtualLocal, 1, f);
    RR_CHARIOT_BLEND49V_ROW(1u, 3u);
    const V physicalPosition = row(physicalCachedWorld, 3, f);
    RR_CHARIOT_BLEND49V_ROW(1u, 2u);
    const V physicalForward = row(physicalCachedWorld, 2, f);
    RR_CHARIOT_BLEND49V_ROW(1u, 1u);
    const V physicalUp = row(physicalCachedWorld, 1, f);
    const float alpha = f.div(oldElapsed84C, positiveDuration848);
    V position{}, forward{}, up{};
    for (unsigned i = 0; i < 3; ++i)
        position.v[i] = interpolate(alpha, virtualPosition.v[i], physicalPosition.v[i], f);
    for (unsigned i = 0; i < 3; ++i)
        forward.v[i] = interpolate(alpha, virtualForward.v[i], physicalForward.v[i], f);
    for (unsigned i = 0; i < 3; ++i)
        up.v[i] = interpolate(alpha, virtualUp.v[i], physicalUp.v[i], f);
    const V x = cross(up, forward, f);
    const V rebuiltUp = cross(forward, x, f);
    const V normalizedUp = f.normalize(rebuiltUp);
    const V normalizedForward = f.normalize(forward);
    const V right = cross(normalizedUp, normalizedForward, f);
    M result{};
    for (unsigned i = 0; i < 3; ++i) {
        result.v[i] = right.v[i];
        result.v[4 + i] = normalizedUp.v[i];
        result.v[8 + i] = normalizedForward.v[i];
        result.v[12 + i] = position.v[i];
    }
    result.v[15] = 1.0f;
    if (!f.valid) return false;
    output = result;
    return true;
}
}

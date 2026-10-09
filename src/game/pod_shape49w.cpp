#include "pod_shape49w.h"
#include <cfenv>
#include <cmath>
#include <cstring>

#ifndef RR_POD_SHAPE49W_STAGE
#define RR_POD_SHAPE49W_STAGE(pc) ((void)0)
#endif

namespace rr::game {
namespace {
struct Math {
    bool valid = true;
    float read(float a) noexcept { valid = valid && std::isfinite(a); return a; }
    float mul(float a, float b) noexcept { read(a); read(b); volatile float x = a * b; return read(x); }
    float add(float a, float b) noexcept { read(a); read(b); volatile float x = a + b; return read(x); }
    float root(float a) noexcept {
        read(a);
        // EE scalar SQRT.S flushes a subnormal operand to signed zero.
        // Normal finite inputs use a double root rounded once to binary32.
        std::uint32_t word; std::memcpy(&word, &a, sizeof(word));
        if ((word & 0x7f800000u) == 0) {
            word &= 0x80000000u;
            float zero; std::memcpy(&zero, &word, sizeof(zero)); return zero;
        }
        volatile float x = static_cast<float>(std::sqrt(static_cast<double>(a)));
        return read(x);
    }
};
}
bool transformPodShape49W(const PodShapeSource49W& source,
    const BodyMatrix48G& mainCachedWorld100, PodShapeState49W& state) noexcept {
    // Admission happens before even the historical copies. Constructor E0=0
    // proves initialization only; it never supplies this current-state fact.
    if (state.type88 != 0 || state.registrationE0 != 0 ||
        std::fegetround() != FE_TONEAREST) return false;
    PodShapeState49W next = state;
    Math f;
    RR_POD_SHAPE49W_STAGE(0x11f6d8u);
    std::memcpy(&next.previousC0, &state.world80, sizeof(next.previousC0));
    RR_POD_SHAPE49W_STAGE(0x11f708u);
    std::memcpy(&next.world80, &mainCachedWorld100, sizeof(next.world80));
    const auto& world = next.world80;
    //11F748..11F78C: VMULAx, VMADDAy, VMADDAz, VMADDw, all4 lanes.
    // The memory convention is local rows times incoming basis rows.
    RR_POD_SHAPE49W_STAGE(0x11f748u);
    for (unsigned row = 0; row < 4; ++row) for (unsigned lane = 0; lane < 4; ++lane) {
        float value = f.mul(world.v[lane], source.local40.v[4 * row]);
        value = f.add(value, f.mul(world.v[4 + lane], source.local40.v[4 * row + 1]));
        value = f.add(value, f.mul(world.v[8 + lane], source.local40.v[4 * row + 2]));
        next.composed00.v[4 * row + lane] = f.add(value, f.mul(world.v[12 + lane], source.local40.v[4 * row + 3]));
    }
    //11F7FC..11F81C: scalar MUL.S x²,y², ADDA.S, MADD.S z², SQRT.S.
    RR_POD_SHAPE49W_STAGE(0x11f7b0u);
    const float xx = f.mul(source.rootHalf.v[0], source.rootHalf.v[0]);
    const float yy = f.mul(source.rootHalf.v[1], source.rootHalf.v[1]);
    const float xy = f.add(xx, yy);
    next.radius80 = f.root(f.add(xy, f.mul(source.rootHalf.v[2], source.rootHalf.v[2])));
    //11F834 forces W=1; XYZ uses incoming world, NOT composed00.
    RR_POD_SHAPE49W_STAGE(0x11f834u);
    next.center60.v[3] = 1.0f;
    for (unsigned lane = 0; lane < 3; ++lane) {
        float value = f.mul(world.v[lane], source.rootCenter.v[0]);
        value = f.add(value, f.mul(world.v[4 + lane], source.rootCenter.v[1]));
        value = f.add(value, f.mul(world.v[8 + lane], source.rootCenter.v[2]));
        next.center60.v[lane] = f.add(value, world.v[12 + lane]);
    }
    RR_POD_SHAPE49W_STAGE(0x12023cu);
    // The actual120240 branch returns without deriving registration bounds.
    if (!f.valid) return false;
    state = next;
    return true;
}
}

#include "body_integration48c.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace rr::game {
namespace {
std::uint32_t bits(float x) noexcept { std::uint32_t b; std::memcpy(&b, &x, 4); return b; }
float fromBits(std::uint32_t b) noexcept { float x; std::memcpy(&x, &b, 4); return x; }
// Store each original arithmetic result at binary32 precision. In particular,
// the original MADD/MSUB paths round the multiplication before addition.
struct Arithmetic {
    bool valid = true;
    float keep(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a, float b) noexcept { volatile float r = a + b; return keep(r); }
    float sub(float a, float b) noexcept { volatile float r = a - b; return keep(r); }
    float mul(float a, float b) noexcept { volatile float r = a * b; return keep(r); }
    float div(float a, float b) noexcept { volatile float r = a / b; return keep(r); }
    float root(float a) noexcept {
        if ((bits(a) & 0x7f800000u) == 0) return fromBits(bits(a) & 0x80000000u);
        volatile float r = static_cast<float>(std::sqrt(static_cast<double>(a)));
        return keep(r);
    }
    float reciprocalRoot(float a) noexcept {
        // Existing reference's positive RSQRT contract: one final float rounding.
        if ((bits(a) & 0x7f800000u) == 0) return fromBits(0x7f7fffffu);
        volatile float r = static_cast<float>(1.0 / std::sqrt(static_cast<double>(a)));
        if ((bits(r) & 0x7f800000u) == 0) return 0.0f;
        return keep(r);
    }
};
bool finite(const Vector3_48C& x) noexcept {
    for (float f : x.v) if (!std::isfinite(f)) return false;
    return true;
}
bool finite(const Quaternion48C& x) noexcept {
    for (float f : x.v) if (!std::isfinite(f)) return false;
    return true;
}
void limit(Vector3_48C& v, float squaredLimit, Arithmetic& f) noexcept {
    const float xx = f.mul(v.v[0], v.v[0]);
    const float yy = f.mul(v.v[1], v.v[1]);
    const float zz = f.mul(v.v[2], v.v[2]);
    // VU adds x+y, then (1*z)+ACC. The explicit multiply preserves its rounding.
    const float norm2 = f.add(f.add(xx, yy), f.mul(1.0f, zz));
    if (!(norm2 <= squaredLimit)) {
        const float scale = f.root(f.div(squaredLimit, norm2));
        for (float& x : v.v) x = f.mul(x, scale);
    }
}
template<unsigned N>
void integrate(float (&state)[N], const float (&derivative)[N],
               float (&history)[N], float dt, Arithmetic& f) noexcept {
    const float halfDt = f.mul(0.5f, dt);
    for (unsigned i = 0; i < N; ++i) {
        const float slope = f.sub(f.mul(3.0f, derivative[i]), history[i]);
        state[i] = f.add(state[i], f.mul(halfDt, slope));
        history[i] = derivative[i];
    }
}
Quaternion48C derivative(const Quaternion48C& q, const Vector3_48C& w,
                         Arithmetic& f) noexcept {
    Quaternion48C d{};
    d.v[0] = f.mul(-0.5f, f.add(f.add(f.mul(q.v[1], w.v[0]),
                                    f.mul(q.v[2], w.v[1])), f.mul(q.v[3], w.v[2])));
    d.v[1] = f.mul(0.5f, f.sub(f.add(f.mul(q.v[0], w.v[0]),
                                   f.mul(q.v[2], w.v[2])), f.mul(q.v[3], w.v[1])));
    d.v[2] = f.mul(0.5f, f.sub(f.add(f.mul(q.v[0], w.v[1]),
                                   f.mul(q.v[3], w.v[0])), f.mul(q.v[1], w.v[2])));
    d.v[3] = f.mul(0.5f, f.sub(f.add(f.mul(q.v[0], w.v[2]),
                                   f.mul(q.v[1], w.v[1])), f.mul(q.v[2], w.v[0])));
    return d;
}
void normalize(Quaternion48C& q, Arithmetic& f) noexcept {
    // Original 0x14F410 order: q[2]^2+(q[1]^2+q[0]^2)+q[3]^2.
    const float norm2 = f.add(f.add(f.mul(q.v[2], q.v[2]),
                                   f.add(f.mul(q.v[1], q.v[1]), f.mul(q.v[0], q.v[0]))),
                             f.mul(q.v[3], q.v[3]));
    if (norm2 == 0.0f) {
        // ELF and preserved running RAM both contain four zeros at 0x382B60.
        // Do not silently replace the game's fallback with identity.
        q = {};
    } else {
        const float scale = f.reciprocalRoot(norm2);
        for (float& x : q.v) x = f.mul(x, scale);
    }
}
}

bool integrateBody48C(const BodyState48C& input, const BodyParameters48C& p,
                      const BodyLoads48C& loads, float dt, BodyState48C& output) noexcept {
    if (!std::isfinite(dt) || dt < 0 || !std::isfinite(p.mass) ||
        !std::isfinite(p.inverseMass) || !finite(loads.force) || !finite(loads.torque) ||
        !finite(input.position) || !finite(input.velocity) || !finite(input.angularVelocity) ||
        !finite(input.orientation) || !finite(input.previousPositionDerivative) ||
        !finite(input.previousLinearAcceleration) || !finite(input.previousAngularAcceleration) ||
        !finite(input.previousOrientationDerivative)) return false;
    for (const auto& column : p.inverseInertiaColumns) if (!finite(column)) return false;
    for (float cap : {p.linearAccelerationLimitSquared, p.angularAccelerationLimitSquared,
                      p.linearVelocityLimitSquared, p.angularVelocityLimitSquared})
        if (!std::isfinite(cap) || cap < 0) return false;

    Arithmetic f;
    BodyState48C s = input;
    if (p.mass > 0.0f) {
        for (unsigned i = 0; i < 3; ++i) {
            s.linearAcceleration.v[i] = f.mul(p.inverseMass, loads.force.v[i]);
            if (!p.translationEnabled[i]) {
                s.velocity.v[i] = 0.0f;
                s.linearAcceleration.v[i] = 0.0f;
                s.previousLinearAcceleration.v[i] = 0.0f;
            }
        }
        limit(s.linearAcceleration, p.linearAccelerationLimitSquared, f);
    } else {
        // Original skips translation locks and retains acceleration history here.
        s.linearAcceleration = {};
    }
    integrate(s.velocity.v, s.linearAcceleration.v, s.previousLinearAcceleration.v, dt, f);
    limit(s.velocity, p.linearVelocityLimitSquared, f);
    integrate(s.position.v, s.velocity.v, s.previousPositionDerivative.v, dt, f);

    for (unsigned i = 0; i < 3; ++i) {
        const float a = f.mul(p.inverseInertiaColumns[0].v[i], loads.torque.v[0]);
        const float b = f.mul(p.inverseInertiaColumns[1].v[i], loads.torque.v[1]);
        const float c = f.mul(p.inverseInertiaColumns[2].v[i], loads.torque.v[2]);
        s.angularAcceleration.v[i] = f.add(f.add(f.add(a, b), c), 0.0f);
        if (!p.rotationEnabled[i]) {
            s.angularVelocity.v[i] = 0.0f;
            s.angularAcceleration.v[i] = 0.0f;
            s.previousAngularAcceleration.v[i] = 0.0f;
        }
    }
    limit(s.angularAcceleration, p.angularAccelerationLimitSquared, f);
    integrate(s.angularVelocity.v, s.angularAcceleration.v, s.previousAngularAcceleration.v, dt, f);
    limit(s.angularVelocity, p.angularVelocityLimitSquared, f);
    s.orientationDerivative = derivative(s.orientation, s.angularVelocity, f);
    integrate(s.orientation.v, s.orientationDerivative.v, s.previousOrientationDerivative.v, dt, f);
    normalize(s.orientation, f);
    if (!f.valid) return false;
    output = s;
    return true;
}
}

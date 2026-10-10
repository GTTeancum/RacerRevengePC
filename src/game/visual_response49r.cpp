#include "vehicle_visual49r_internal.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

// Optional test-build observation only. Shipping builds have no observer,
// callback, allocation, branch or persistent trace state.
#ifndef RR_VISUAL49R_STAGE
#define RR_VISUAL49R_STAGE(pc) ((void)0)
#endif

namespace rr::game::detail {
namespace {
using V = VisualVector49R;
float fp(std::uint32_t word) noexcept { float x; std::memcpy(&x, &word, 4); return x; }
std::uint32_t bits(float x) noexcept { std::uint32_t word; std::memcpy(&word, &x, 4); return word; }
struct Math {
    bool valid = true;
    float read(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a, float b) noexcept { read(a); read(b); volatile float x = a + b; return read(x); }
    float sub(float a, float b) noexcept { read(a); read(b); volatile float x = a - b; return read(x); }
    float mul(float a, float b) noexcept { read(a); read(b); volatile float x = a * b; return read(x); }
    float div(float a, float b) noexcept { read(a); read(b); volatile float x = a / b; return read(x); }
    float neg(float a) noexcept { read(a); return fp(bits(a) ^ 0x80000000u); }
    float abs(float a) noexcept { read(a); return fp(bits(a) & 0x7fffffffu); }
    // EE MIN/MAX order the finite binary words, including -0 below +0.
    float maximum(float a, float b) noexcept {
        read(a); read(b); const auto x = static_cast<std::int32_t>(bits(a));
        const auto y = static_cast<std::int32_t>(bits(b));
        return fp(static_cast<std::uint32_t>((x < 0 && y < 0) ? (x < y ? x : y) : (x > y ? x : y)));
    }
    float minimum(float a, float b) noexcept {
        read(a); read(b); const auto x = static_cast<std::int32_t>(bits(a));
        const auto y = static_cast<std::int32_t>(bits(b));
        return fp(static_cast<std::uint32_t>((x < 0 && y < 0) ? (x > y ? x : y) : (x < y ? x : y)));
    }
    float scalarRoot(float a) noexcept {
        read(a);
        // Retained EE reference convention: subnormal input becomes signed0.
        if ((bits(a) & 0x7f800000u) == 0) return fp(bits(a) & 0x80000000u);
        volatile float x = static_cast<float>(std::sqrt(static_cast<double>(a))); return read(x);
    }
    float reciprocalRoot(float a) noexcept {
        read(a);
        // 14F410 uses the scalar EE RSQRT model, not the VU Q model below.
        if ((bits(a) & 0x7f800000u) == 0) return fp(0x7f7fffffu);
        volatile float x = static_cast<float>(1.0 / std::sqrt(static_cast<double>(a)));
        if ((bits(x) & 0x7f800000u) == 0) return 0.0f;
        return read(x);
    }
    float squared(const V& v, bool vu) noexcept {
        const float x = mul(v.v[0], v.v[0]);
        const float y = mul(v.v[1], v.v[1]);
        const float z = mul(v.v[2], v.v[2]);
        return add(add(x, y), vu ? mul(1.0f, z) : z);
    }
    V difference(const V& a, const V& b) noexcept {
        return V{{sub(a.v[0], b.v[0]), sub(a.v[1], b.v[1]), sub(a.v[2], b.v[2]), 1.0f}};
    }
    V vuNormalize(const V& v) noexcept {
        const float norm = squared(v, true); float q;
        //49R excludes positive subnormal squared VU norms; the retained host
        // reference is not a proof of hardware underflow behavior there.
        if (norm != 0.0f && (bits(norm) & 0x7f800000u) == 0) valid = false;
        // VRSQRT(1,0) saturates Q. The original still multiplies all XYZ.
        if (norm == 0.0f) q = fp((bits(norm) & 0x80000000u) | 0x7f7fffffu);
        else { volatile float root = std::sqrt(norm); q = div(1.0f, read(root)); }
        return V{{mul(v.v[0], q), mul(v.v[1], q), mul(v.v[2], q), add(v.v[3], 0.0f)}};
    }
    float dot(const V& a, const V& b) noexcept {
        const float x = mul(a.v[0], b.v[0]);
        const float y = mul(a.v[1], b.v[1]);
        const float z = mul(a.v[2], b.v[2]);
        return add(add(x, y), mul(1.0f, z));
    }
};
bool fail(VehicleVisualDiagnostic49R& d, VehicleVisualError49R error, std::uint32_t pc) noexcept {
    d = {error, pc}; return false;
}
void lazy(VisualLazyWord49R& value, std::uint32_t word) noexcept {
    if (value.guard == 0) { value.word = word; value.guard = 1; }
}
float clampEndpoints(float candidate, float first, float last, Math& f) noexcept {
    f.read(candidate); f.read(first); f.read(last);
    if (last <= first) {
        if (candidate < last) return last;
        if (!(candidate <= first)) return first;
    } else {
        if (!(candidate <= last)) return last;
        if (candidate < first) return first;
    }
    return candidate;
}
float speedInterpolation(float square, float denominator, float first, float last, Math& f) noexcept {
    f.read(denominator);
    if (denominator == 0.0f) return 0.0f;
    const float den = f.sub(denominator, 0.0f);
    const float dx = f.sub(square, 0.0f);
    const float dy = f.sub(last, first);
    return clampEndpoints(f.add(first, f.div(f.mul(dx, dy), den)), first, last, f);
}
float turnInterpolation(float raw, float first, float last, Math& f) noexcept {
    const float dx = f.sub(f.abs(raw), fp(0x3d4ccccdu));
    const float dy = f.sub(last, first);
    return clampEndpoints(f.add(first, f.div(f.mul(dx, dy), fp(0x3f733333u))), first, last, f);
}
float damageInterpolation(float damage, float last, Math& f) noexcept {
    const float dx = f.sub(damage, 0.0f);
    const float dy = f.sub(last, 1.0f);
    return clampEndpoints(f.add(1.0f, f.div(f.mul(dx, dy), 1.0f)), 1.0f, last, f);
}
struct Amplitudes { float common, translation[4], orientation[4]; };
Amplitudes prepare(const VehicleVisualInput49R& in, const VehicleVisualParameters49R& p,
                   VehicleVisualState49R& s, Math& f) noexcept {
    // 21C470 is a branch delay instruction, even with a zero A54.
    const float square = f.mul(in.cachedBodySpeed34, in.cachedBodySpeed34);
    Amplitudes a{};
    a.common = speedInterpolation(square, p.effectiveSpeedSquaredA54, p.minAmplitudeA44, p.maxAmplitudeA48, f);
    const float speedTime = speedInterpolation(square, p.effectiveSpeedSquaredA54, p.minTimeA34, p.maxTimeA38, f);
    float rightTime = speedTime, leftTime = speedTime;
    if (in.vehicle40 != 0) {
        for (unsigned i = 0; i < 4; ++i) { a.translation[i] = p.damageTranslationA58; a.orientation[i] = p.damageOrientationA5C; }
    } else {
        const bool slide = in.aeroDC != 0 && f.abs(in.rawB5C) > fp(0x3d4ccccdu);
        float rightScale = 1.0f, leftScale = 1.0f;
        if (slide) {
            a.common = 1.0f;
            const float turnAmplitude = turnInterpolation(in.rawB5C, p.minAmplitudeA44, p.maxAmplitudeA48, f);
            const float turnTime = turnInterpolation(in.rawB5C, p.minTimeA34, p.maxTimeA38, f);
            if (in.rawB5C > 0.0f) { rightScale = p.minAmplitudeA44; leftScale = turnAmplitude; leftTime = turnTime; }
            else { rightScale = turnAmplitude; leftScale = p.minAmplitudeA44; rightTime = turnTime; }
        }
        const float damage[4] = {in.damage7D0, in.damage7D8, in.damage7D4, in.damage7DC};
        for (unsigned i = 0; i < 4; ++i) {
            const float factor = damageInterpolation(damage[i], p.damageTranslationA58, f);
            a.translation[i] = slide ? f.mul(i < 2 ? rightScale : leftScale, factor) : factor;
        }
        for (unsigned i = 0; i < 4; ++i) {
            const float factor = damageInterpolation(damage[i], p.damageOrientationA5C, f);
            a.orientation[i] = slide ? f.mul(i < 2 ? rightScale : leftScale, factor) : factor;
        }
    }
    s.phase8B0 = f.add(s.phase8B0, f.mul(leftTime, in.frameFactor));
    s.phase900 = f.add(s.phase900, f.mul(rightTime, in.frameFactor));
    s.phase950 = f.add(s.phase950, f.mul(leftTime, in.frameFactor));
    s.phase9A0 = f.add(s.phase9A0, f.mul(rightTime, in.frameFactor));
    return a;
}
// Full reached28BBB0 inverse scratch, including translation arithmetic which
// the final direction rotation does not use. Input direction W is copy-only.
V inverseDirection(const BodyMatrix48G& m, const V& v, Math& f) noexcept {
    BodyMatrix48G inverse = m;
    for (unsigned c = 0; c < 3; ++c) for (unsigned r = 0; r < 4; ++r)
        inverse.v[c * 4 + r] = (r == 3 || r == c) ? f.add(m.v[c * 4 + r], 0.0f) : f.add(0.0f, m.v[r * 4 + c]);
    for (unsigned r = 0; r < 3; ++r) {
        float x = f.mul(inverse.v[r], m.v[12]);
        x = f.add(x, f.mul(inverse.v[4 + r], m.v[13]));
        x = f.add(x, f.mul(inverse.v[8 + r], m.v[14]));
        inverse.v[12 + r] = f.mul(x, -1.0f);
    }
    inverse.v[15] = f.add(m.v[15], 0.0f);
    V out{{0, 0, 0, 1}};
    for (unsigned r = 0; r < 3; ++r) {
        float x = f.mul(inverse.v[r], v.v[0]);
        x = f.add(x, f.mul(inverse.v[4 + r], v.v[1]));
        x = f.add(x, f.mul(inverse.v[8 + r], v.v[2]));
        out.v[r] = f.add(x, 0.0f);
    }
    return out;
}
void normalize(Quaternion48C& q, Math& f) noexcept {
    const float norm = f.add(f.add(f.mul(q.v[2], q.v[2]),
        f.add(f.mul(q.v[1], q.v[1]), f.mul(q.v[0], q.v[0]))), f.mul(q.v[3], q.v[3]));
    if (norm == 0.0f) q = {}; // Source382B60: zero quaternion, not identity.
    else { const float scale = f.reciprocalRoot(norm); for (float& x : q.v) x = f.mul(x, scale); }
}
void integrate(VisualResponse49R& s, const V& force, const V& torque, float dt, Math& f) noexcept {
    for (unsigned i = 0; i < 3; ++i) s.linearVelocity.v[i] = f.add(s.linearVelocity.v[i], f.mul(dt, force.v[i]));
    for (unsigned i = 0; i < 3; ++i) s.position.v[i] = f.add(s.position.v[i], f.mul(dt, s.linearVelocity.v[i]));
    for (unsigned i = 0; i < 3; ++i) s.angularVelocity.v[i] = f.add(s.angularVelocity.v[i], f.mul(dt, torque.v[i]));
    const auto& q = s.orientation.v; const auto& w = s.angularVelocity.v;
    Quaternion48C derivative{};
    derivative.v[0] = f.mul(-0.5f, f.add(f.add(f.mul(q[1], w[0]), f.mul(q[2], w[1])), f.mul(q[3], w[2])));
    derivative.v[1] = f.mul(0.5f, f.sub(f.add(f.mul(q[0], w[0]), f.mul(q[2], w[2])), f.mul(q[3], w[1])));
    derivative.v[2] = f.mul(0.5f, f.sub(f.add(f.mul(q[0], w[1]), f.mul(q[3], w[0])), f.mul(q[1], w[2])));
    derivative.v[3] = f.mul(0.5f, f.sub(f.add(f.mul(q[0], w[2]), f.mul(q[1], w[1])), f.mul(q[2], w[0])));
    for (float& x : derivative.v) x = f.mul(x, dt);
    for (unsigned i = 0; i < 4; ++i) s.orientation.v[i] = f.add(s.orientation.v[i], derivative.v[i]);
    normalize(s.orientation, f);
}
void addScaled(V& into, float scale, const V& source, Math& f) noexcept {
    float products[3]; for (unsigned i = 0; i < 3; ++i) products[i] = f.mul(scale, source.v[i]);
    for (unsigned i = 0; i < 3; ++i) into.v[i] = f.add(into.v[i], products[i]);
}
bool noiseAdd(float& lane, float coordinate, float amplitude, CoupledNoiseContext49M& noise, Math& f) noexcept {
    float value;
    if (!f.valid || !evaluateGradientNoise49M(noise.noise, noise.sharedRandomState, coordinate, value)) return false;
    lane = f.add(lane, f.mul(amplitude, value)); return f.valid;
}
bool emitRoot(VisualResponse49R& s, bool right, float phase, float common, float translation,
              float orientation, const VehicleVisualInput49R& in, const VehicleVisualParameters49R& p,
              const BodyMatrix48G& containerWorld, const SceneGraph49P& graph, SceneNodeIndex49P node,
              CoupledNoiseContext49M& noise, Math& f, VehicleVisualDiagnostic49R& d) noexcept {
    V position = s.position;
    if (in.vehicle844 == 0) {
        const float amplitude = f.mul(common, translation);
        const float x = right ? phase : f.add(5.0f, phase);
        if (!noiseAdd(position.v[0], x, amplitude, noise, f) ||
            !noiseAdd(position.v[1], f.add(right ? 2.0f : 7.0f, phase), amplitude, noise, f))
            return fail(d, VehicleVisualError49R::noise, right ? 0x21e7b8 : 0x21f0e4);
    }
    f.read(p.verticalLimit7EC); f.read(position.v[1]);
    if (!(position.v[1] <= p.verticalLimit7EC)) {
        position.v[1] = p.verticalLimit7EC; s.position.v[1] = p.verticalLimit7EC; s.linearVelocity.v[1] = 0.0f;
    } else if (position.v[1] < f.neg(p.verticalLimit7EC)) {
        position.v[1] = f.neg(p.verticalLimit7EC); s.position.v[1] = f.neg(p.verticalLimit7EC); s.linearVelocity.v[1] = 0.0f;
    }
    Quaternion48C visual = s.orientation;
    if (in.vehicle844 == 0) {
        const float amplitude = f.mul(p.orientationScaleA64, f.mul(common, orientation));
        const float offsets[4] = {right ? 2.0f : 11.0f, right ? 17.0f : 13.0f, right ? 5.0f : 7.0f, 1.0f};
        for (unsigned i = 0; i < 4; ++i)
            if (!noiseAdd(visual.v[(i + 1) % 4], f.add(offsets[i], phase), amplitude, noise, f))
                return fail(d, VehicleVisualError49R::noise, right ? 0x21e888 : 0x21f1b4);
    }
    normalize(visual, f); // Also required with844!=0.
    BodyMatrix48G local{};
    if (!f.valid || !quaternionToMatrix48G(visual, local)) return fail(d, VehicleVisualError49R::arithmetic, 0x14f620);
    //14F620 executes this discarded rotated-position scratch before returning
    // the unrotated position. Retain its finite-arithmetic admission.
    for (unsigned r = 0; r < 3; ++r) {
        float x = f.mul(local.v[r], position.v[0]);
        x = f.add(x, f.mul(local.v[4 + r], position.v[1]));
        x = f.add(x, f.mul(local.v[8 + r], position.v[2]));
        (void)f.add(x, 0.0f);
        local.v[12 + r] = position.v[r];
    }
    local.v[15] = 1.0f;
    BodyMatrix48G composed{};
    for (unsigned row = 0; row < 4; ++row) for (unsigned lane = 0; lane < 4; ++lane) {
        float x = f.mul(containerWorld.v[lane], local.v[4 * row]);
        x = f.add(x, f.mul(containerWorld.v[4 + lane], local.v[4 * row + 1]));
        x = f.add(x, f.mul(containerWorld.v[8 + lane], local.v[4 * row + 2]));
        composed.v[4 * row + lane] = f.add(x, f.mul(containerWorld.v[12 + lane], local.v[4 * row + 3]));
    }
    if (!f.valid) return fail(d, VehicleVisualError49R::arithmetic, right ? 0x21e97c : 0x21f2a8);
    if (!setLocalMatrix49P(graph, node, composed)) return fail(d, VehicleVisualError49R::graph, 0x28b490);
    return true;
}
}

bool updateVisualResponses49R(const VehicleVisualInput49R& in, const VehicleVisualParameters49R& p,
    const VisualSceneBindings49R& binding, const VisualFrame49R& frame, const SceneGraph49P& graph,
    VehicleVisualState49R& s, VisualShared49R& shared, CoupledNoiseContext49M& noise,
    VehicleVisualDiagnostic49R& d) noexcept {
    if (std::fegetround() != FE_TONEAREST) return fail(d, VehicleVisualError49R::rounding_mode, 0x21c440);
    Math f;
    const Amplitudes amplitudes = prepare(in, p, s, f);
    RR_VISUAL49R_STAGE(0x21d74cu);
    float inputScale = 0.0f;
    if (f.read(p.effectiveInputSpeedA68) != 0.0f) {
        const float dx = f.sub(in.cachedBodySpeed34, 0.0f);
        const float den = f.sub(p.effectiveInputSpeedA68, 0.0f);
        inputScale = f.add(0.0f, f.div(f.mul(dx, 1.0f), den));
        if (!(inputScale <= 1.0f)) inputScale = 1.0f;
        else if (inputScale < 0.0f) inputScale = 0.0f;
    }
    const float engineDifference = f.sub(in.engine68, in.engine6C);
    const float aeroDifference = f.sub(in.aeroE8, in.aeroE4);
    const float selected = f.abs(engineDifference) <= f.abs(aeroDifference) ? aeroDifference : engineDifference;
    const float differential = f.mul(selected, inputScale);
    const bool bit1 = (in.applicationByte58 & 2u) != 0;
    float leftThrust, rightThrust, leftDrag = 0.0f, rightDrag = 0.0f;
    if (!bit1) { leftThrust = in.engine70; rightThrust = in.engine74; }
    else if (in.aeroDC != 0) {
        leftThrust = f.maximum(in.rawB5C, 0.0f);
        rightThrust = f.maximum(f.neg(in.rawB5C), 0.0f);
        leftDrag = f.maximum(f.neg(in.rawB5C), 0.0f);
        rightDrag = f.maximum(in.rawB5C, 0.0f);
    } else {
        leftDrag = f.mul(inputScale, in.aeroE4);
        leftThrust = f.mul(inputScale, in.engine70);
        rightThrust = f.mul(inputScale, in.engine74);
        rightDrag = f.mul(inputScale, in.aeroE8);
    }
    if (!f.valid) return fail(d, VehicleVisualError49R::arithmetic, 0x21d91c);
    Vector3_48C angleRight{}, angleLeft{};
    if (!quaternionToAngles48G(s.right.orientation, angleRight) || !quaternionToAngles48G(s.left.orientation, angleLeft))
        return fail(d, VehicleVisualError49R::arithmetic, 0x14fc00);
    if (!refreshSceneWorld49P(graph, binding.main190)) return fail(d, VehicleVisualError49R::graph, 0x28bbb0);
    V impact = inverseDirection(graph.nodes[static_cast<std::uint32_t>(binding.main190)].world100, in.impactBA0, f);
    lazy(shared.globals[6], 0x41200000u); lazy(shared.globals[7], 0x40000000u);
    const float bound = fp(shared.globals[6].word);
    const auto clampImpact = [&](float x) noexcept { return f.minimum(f.maximum(x, f.neg(bound)), bound); };
    if (f.read(in.objectD34_10) > fp(0x3e4ccccdu) && (in.vehicleD00 != 0 || in.vehicleCFC != 0)) impact.v[0] = 0.0f;
    else impact.v[0] = clampImpact(impact.v[0]);
    for (unsigned i = 0; i < 3; ++i) impact.v[i] = clampImpact(impact.v[i]);
    RR_VISUAL49R_STAGE(0x21db30u);
    const V delta = f.difference(s.right.position, s.left.position);
    const V rest = f.difference(p.rightBase2C0, p.leftBase2B0);
    const float restLength = f.scalarRoot(f.squared(rest, false));
    const float currentLength = f.scalarRoot(f.squared(delta, false));
    V binder = f.vuNormalize(delta);
    const float extension = f.sub(currentLength, restLength);
    const float scalar = f.mul(p.binderSpringA10, extension);
    for (unsigned i = 0; i < 3; ++i) binder.v[i] = f.mul(binder.v[i], scalar);
    V binderTorque{{0, 0, 0, 1}};
    for (unsigned i = 0; i < 3; ++i) binderTorque.v[i] = f.mul(p.binderTorqueA30, f.sub(angleRight.v[i], angleLeft.v[i]));
    binderTorque.v[1] = 0.0f;
    RR_VISUAL49R_STAGE(0x21e040u);
    const float thrustGain = bit1 ? 1.0f : fp(shared.globals[5].word);
    if (!f.valid) return fail(d, VehicleVisualError49R::arithmetic, 0x21e074);
    // Both old angle snapshots and the old-position binder survive the complete
    // right update/root write before the left engine begins.
    for (unsigned side = 0; side < 2; ++side) {
        const bool right = side == 0;
        VisualResponse49R& response = right ? s.right : s.left;
        V force = shared.zero434630, torque = shared.zero434630;
        if (bit1) for (unsigned i = 0; i < 3; ++i) {
            force.v[i] = right ? f.neg(binder.v[i]) : binder.v[i];
            torque.v[i] = right ? binderTorque.v[i] : f.neg(binderTorque.v[i]);
        }
        if (in.vehicle844 != 0) force.v[1] = f.sub(force.v[1], fp(shared.globals[2].word));
        const float horizontalImpact = right ? f.minimum(impact.v[0], 0.0f) : f.maximum(impact.v[0], 0.0f);
        force.v[0] = f.add(force.v[0], f.mul(p.impact9F0, horizontalImpact));
        force.v[1] = f.add(force.v[1], f.mul(p.impact9F0, impact.v[1]));
        force.v[2] = f.add(force.v[2], f.mul(p.impact9F0, impact.v[2]));
        force.v[1] = f.add(force.v[1], f.mul(p.groundImpact9F4, f.dot(in.probeVector1D0, frame.mainUp)));
        force.v[2] = f.add(force.v[2], f.mul(right ? rightThrust : leftThrust, f.mul(thrustGain, p.thrust9FC)));
        force.v[2] = f.add(force.v[2], f.mul(p.dragA04, right ? rightDrag : leftDrag));
        const float lift = f.mul(p.liftA00, differential);
        force.v[1] = right ? f.sub(force.v[1], lift) : f.add(force.v[1], lift);
        const float separation = f.mul(p.separation9F8, right ? f.abs(f.minimum(differential, 0.0f)) : f.maximum(differential, 0.0f));
        force.v[0] = right ? f.add(force.v[0], separation) : f.sub(force.v[0], separation);
        const V displacement = f.difference(right ? p.rightBase2C0 : p.leftBase2B0, response.position);
        const bool fullSpring = (in.applicationByte58 & 6u) != 0;
        const float spring = fullSpring ? p.springA08 : f.mul(fp(shared.globals[4].word), p.springA08);
        addScaled(force, spring, displacement, f);
        const float damping = fullSpring ? p.dampingA0C : f.mul(fp(shared.globals[3].word), p.dampingA0C);
        addScaled(force, damping, response.linearVelocity, f);
        torque.v[2] = f.add(torque.v[2], f.mul(p.rollA24, differential));
        if (in.aeroD8 != 0) {
            const float pitch = f.mul(p.slidePitchA1C, right ? f.minimum(differential, 0.0f) : f.maximum(differential, 0.0f));
            torque.v[0] = right ? f.sub(torque.v[0], pitch) : f.add(torque.v[0], pitch);
        } else {
            const float pitch = f.mul(p.pitchA20, differential);
            torque.v[0] = right ? f.add(torque.v[0], pitch) : f.sub(torque.v[0], pitch);
        }
        torque.v[1] = f.add(torque.v[1], f.mul(in.aeroD8 != 0 ? p.slideYawA18 : p.yawA14, differential));
        const auto& angle = right ? angleRight : angleLeft;
        const V angleVector{{angle.v[0], angle.v[1], angle.v[2], 1.0f}};
        addScaled(torque, p.angleReturnA28, angleVector, f);
        addScaled(torque, p.angularDampingA2C, response.angularVelocity, f);
        RR_VISUAL49R_STAGE(right ? 0x21e62cu : 0x21ef4cu);
        integrate(response, force, torque, in.frameFactor, f);
        if (!f.valid) return fail(d, VehicleVisualError49R::arithmetic, right ? 0x21e79c : 0x21f0bc);
        RR_VISUAL49R_STAGE(right ? 0x21e7a0u : 0x21f0c0u);
        const unsigned amplitudeIndex = right ? 0 : 2;
        if (!emitRoot(response, right, right ? s.phase900 : s.phase8B0, amplitudes.common,
            amplitudes.translation[amplitudeIndex], amplitudes.orientation[amplitudeIndex], in, p,
            graph.nodes[static_cast<std::uint32_t>(binding.container194)].world100, graph,
            right ? binding.right1B8 : binding.left1BC, noise, f, d)) return false;
        RR_VISUAL49R_STAGE(right ? 0x21e9f4u : 0x21f320u);
    }
    // The source reaches this guard before testing the admitted empty B54 list.
    lazy(shared.globals[8], 0x41200000u);
    RR_VISUAL49R_STAGE(0x220608u);
    d = {VehicleVisualError49R::none, 0x22089cu};
    return true;
}
}

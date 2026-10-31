#include "original_standard_placement_globals50la.h"
#include "chariot_ik49t.h"
#include "character_mode0_ik50kg.h"
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstring>

namespace rr::game {
namespace {
using U = std::uint32_t;
using I = SceneNodeIndex49P;
using V = ChariotIkVector49T;
using M = BodyMatrix48G;
constexpr I none = I::none;
float fp(U w) noexcept { float f; std::memcpy(&f, &w, 4); return f; }
U bits(float f) noexcept { U w; std::memcpy(&w, &f, 4); return w; }
bool fail(ChariotIkDiagnostic49T& d, ChariotIkError49T e, U pc, U hand = 0) noexcept {
    d = {e, pc, hand}; return false;
}
struct Math {
    bool valid = true;
    float keep(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a, float b) noexcept { keep(a); keep(b); volatile float x = a + b; return keep(x); }
    float sub(float a, float b) noexcept { keep(a); keep(b); volatile float x = a - b; return keep(x); }
    float mul(float a, float b) noexcept { keep(a); keep(b); volatile float x = a * b; return keep(x); }
    float div(float a, float b) noexcept { keep(a); keep(b); volatile float x = a / b; return keep(x); }
    float neg(float a) noexcept { keep(a); return fp(bits(a) ^ 0x80000000u); }
    float abs(float a) noexcept { keep(a); return fp(bits(a) & 0x7fffffffu); }
    float scalarRoot(float a) noexcept {
        keep(a); const U w = bits(a);
        if ((w & 0x7f800000u) == 0) return fp(w & 0x80000000u);
        // Retained EE SQRT.S takes the absolute operand, including negative
        // roundoff in1-dot^2. It is distinct from VU Q below.
        volatile float x = static_cast<float>(std::sqrt(static_cast<double>(abs(a))));
        return keep(x);
    }
    float scalarReciprocalRoot(float a) noexcept {
        keep(a); const U w = bits(a);
        if ((w & 0x7f800000u) == 0) return fp((w & 0x80000000u) | 0x7f7fffffu);
        volatile float x = static_cast<float>(1.0 / std::sqrt(static_cast<double>(abs(a))));
        U b = bits(x);
        if ((b & 0x7fffffffu) == 0x7f800000u) b = (b & 0x80000000u) | 0x7f7fffffu;
        else if ((b & 0x7f800000u) == 0) b &= 0x80000000u;
        return keep(fp(b));
    }
    float square(const V& a, bool vu = false) noexcept {
        const float x = mul(a.v[0], a.v[0]);
        const float y = mul(a.v[1], a.v[1]);
        const float z = mul(a.v[2], a.v[2]);
        return add(add(x, y), vu ? mul(1.0f, z) : z);
    }
    float length(const V& a) noexcept {
        const float s = square(a);
        return s == 0.0f ? 0.0f : div(1.0f, scalarReciprocalRoot(s));
    }
    V difference(const V& a, const V& b) noexcept {
        return V{{sub(a.v[0], b.v[0]), sub(a.v[1], b.v[1]), sub(a.v[2], b.v[2]), 1.0f}};
    }
    float dot(const V& a, const V& b) noexcept {
        const float x = mul(a.v[0], b.v[0]);
        const float y = mul(a.v[1], b.v[1]);
        const float z = mul(a.v[2], b.v[2]);
        return add(add(x, y), mul(1.0f, z));
    }
    V cross(const V& a, const V& b, float w) noexcept {
        V out{{0, 0, 0, w}};
        for (U i = 0; i < 3; ++i) {
            const U j = (i + 1) % 3, k = (i + 2) % 3;
            out.v[i] = sub(mul(a.v[j], b.v[k]), mul(b.v[j], a.v[k]));
        }
        return out;
    }
    V scalarNormalize(V a) noexcept {
        const float s = square(a);
        if (s == 0.0f) return V{{0.0f, 0.0f, 0.0f, 1.0f}};
        const float q = scalarReciprocalRoot(s);
        for (U i = 0; i < 3; ++i) a.v[i] = mul(a.v[i], q);
        return a;
    }
    V vuNormalize(const V& a, bool multipliedZ = true, bool zeroFirstW = false) noexcept {
        const float s = square(a, multipliedZ); float q;
        if (s == 0.0f) q = fp((bits(s) & 0x80000000u) | 0x7f7fffffu);
        else {
            if (s < 0.0f || !std::isfinite(s) || (bits(s) & 0x7f800000u) == 0) valid = false;
            volatile float root = std::sqrt(s);
            q = div(1.0f, keep(root));
        }
        return V{{mul(a.v[0], q), mul(a.v[1], q), mul(a.v[2], q),
            zeroFirstW ? add(0.0f, a.v[3]) : add(a.v[3], 0.0f)}};
    }
};
SceneNode49P& node(const SceneGraph49P& g, I i) noexcept { return g.nodes[static_cast<U>(i)]; }
bool index(I i, U count) noexcept { return static_cast<std::int32_t>(i) >= 0 && static_cast<U>(i) < count; }
bool finiteGraph(const SceneGraph49P& g) noexcept {
    if (!validateSceneGraph49P(g)) return false;
    for (U i = 0; i < g.count; ++i) for (U j = 0; j < 16; ++j)
        if (!std::isfinite(g.nodes[i].localC0.v[j]) || !std::isfinite(g.nodes[i].world100.v[j])) return false;
    return true;
}
bool chainBinding(const SceneGraph49P& g, const ChariotIkChainBinding49T& b) noexcept {
    return index(b.shoulder, g.count) && index(b.elbow, g.count) && index(b.wrist, g.count) &&
        b.shoulder != b.elbow && b.shoulder != b.wrist && b.elbow != b.wrist &&
        node(g, b.elbow).parent180 == b.shoulder && node(g, b.wrist).parent180 == b.elbow &&
        (b.target == none || index(b.target, g.count));
}
bool bindings(const SceneGraph49P& g, const ChariotIkBindings49T& b) noexcept {
    auto optional = [&](const ChariotIkChainBinding49T& c) noexcept {
        return c.wrist == none ? c.shoulder == none && c.elbow == none && c.target == none : chainBinding(g, c);
    };
    if (!optional(b.left) || !optional(b.right)) return false;
    const I left[3] = {b.left.shoulder, b.left.elbow, b.left.wrist};
    const I right[3] = {b.right.shoulder, b.right.elbow, b.right.wrist};
    for (I a : left) for (I z : right) if (a != none && a == z) return false;
    const I targets[2] = {b.left.target, b.right.target};
    for (I t : targets) if (t != none) {
        for (I a : left) if (t == a) return false;
        for (I a : right) if (t == a) return false;
    }
    return b.left.target == none || b.right.target == none || b.left.target != b.right.target;
}
bool libmContext(const SourceLibmState49T& s) noexcept {
    return s.mode == 1 && s.defaultMatherr && s.coldNanRecord;
}
V xyz(const M& m, U offset) noexcept { return V{{m.v[offset], m.v[offset + 1], m.v[offset + 2], 1.0f}}; }
V worldPosition(const SceneGraph49P& g, I i, Math& f) noexcept {
    f.valid = refreshSceneWorld49P(g, i) && f.valid;
    return xyz(node(g, i).world100, 12);
}
M transposeMoves(const M& m, Math& f) noexcept {
    M out{};
    for (U i = 0; i < 16; ++i) out.v[i] = f.add(m.v[i], 0.0f);
    out.v[4] = f.add(0.0f, m.v[1]); out.v[8] = f.add(0.0f, m.v[2]);
    out.v[9] = f.add(0.0f, m.v[6]); out.v[1] = f.add(0.0f, m.v[4]);
    out.v[2] = f.add(0.0f, m.v[8]); out.v[6] = f.add(0.0f, m.v[9]);
    return out;
}
M inverseWorld(const M& m, Math& f) noexcept {
    //28BBB0/28BD50 preserve fourth lanes while transposing basis with exact
    // positive-zero additions. Both execute the inverse translation scratch.
    M out = m;
    for (U c = 0; c < 3; ++c) for (U r = 0; r < 4; ++r)
        out.v[c * 4 + r] = (r == 3 || r == c) ? f.add(m.v[c * 4 + r], 0.0f) : f.add(0.0f, m.v[r * 4 + c]);
    for (U r = 0; r < 3; ++r) {
        float a = f.mul(out.v[r], m.v[12]);
        a = f.add(a, f.mul(out.v[4 + r], m.v[13]));
        a = f.add(a, f.mul(out.v[8 + r], m.v[14]));
        out.v[12 + r] = f.mul(a, -1.0f);
    }
    out.v[15] = f.add(m.v[15], 0.0f);
    return out;
}
V inverseVector(const SceneGraph49P& g, I i, const V& v, bool point, Math& f) noexcept {
    f.valid = refreshSceneWorld49P(g, i) && f.valid;
    const M inverse = inverseWorld(node(g, i).world100, f);
    V out{{0, 0, 0, 1}};
    for (U r = 0; r < 3; ++r) {
        float a = f.mul(inverse.v[r], v.v[0]);
        a = f.add(a, f.mul(inverse.v[4 + r], v.v[1]));
        a = f.add(a, f.mul(inverse.v[8 + r], v.v[2]));
        out.v[r] = f.add(a, point ? inverse.v[12 + r] : 0.0f);
    }
    return out;
}
M product(const M& local, const M& parent, Math& f) noexcept {
    M out{};
    for (U row = 0; row < 4; ++row) for (U lane = 0; lane < 4; ++lane) {
        float a = f.mul(parent.v[lane], local.v[4 * row]);
        a = f.add(a, f.mul(parent.v[4 + lane], local.v[4 * row + 1]));
        a = f.add(a, f.mul(parent.v[8 + lane], local.v[4 * row + 2]));
        out.v[4 * row + lane] = f.add(a, f.mul(parent.v[12 + lane], local.v[4 * row + 3]));
    }
    return out;
}
M relative(const M& requested, const M& parent, Math& f) noexcept {
    const V delta = f.difference(xyz(requested, 12), xyz(parent, 12));
    const M t = transposeMoves(requested, f);
    M out = transposeMoves(product(parent, t, f), f);
    const M pt = transposeMoves(parent, f);
    for (U r = 0; r < 3; ++r) {
        float a = f.mul(pt.v[r], delta.v[0]);
        a = f.add(a, f.mul(pt.v[4 + r], delta.v[1]));
        out.v[12 + r] = f.add(a, f.mul(pt.v[8 + r], delta.v[2]));
    }
    out.v[15] = f.add(0.0f, 1.0f);
    return out;
}
float acos(float a, SourceLibmState49T& libm, Math& f) noexcept {
    U out = 0;
    if (!f.valid || !sourceAcos49T(bits(a), libm, out)) f.valid = false;
    return f.keep(fp(out));
}
float sine(float a, Math& f) noexcept {
    if (!(a <= fp(0x3fc90fdbu))) a = f.sub(fp(0x40490fdbu), a);
    else if (a < fp(0xbfc90fdbu)) a = f.sub(fp(0xc0490fdbu), a);
    const float sq = f.mul(a, a);
    float r = f.mul(fp(0x3638ef1du), sq);
    r = f.add(fp(0xb9500d01u), r);
    r = f.mul(sq, r);
    r = f.add(fp(0x3c088889u), r);
    r = f.mul(sq, r);
    r = f.add(fp(0xbe2aaaabu), r);
    return f.add(a, f.mul(f.mul(a, sq), r));
}
float cosine(float a, Math& f) noexcept {
    float sign = 1.0f;
    if (!(a <= fp(0x3fc90fdbu))) { a = f.sub(fp(0x40490fdbu), a); sign = -1.0f; }
    else if (a < fp(0xbfc90fdbu)) { a = f.sub(fp(0xc0490fdbu), a); sign = -1.0f; }
    const float sq = f.mul(a, a);
    float r = f.mul(fp(0xb493f27eu), sq);
    r = f.add(fp(0x37d00d01u), r);
    r = f.mul(sq, r);
    r = f.add(fp(0xbab60b61u), r);
    r = f.mul(sq, r);
    r = f.add(fp(0x3d2aaaabu), r);
    r = f.mul(sq, r);
    r = f.add(-0.5f, r);
    r = f.mul(sq, r);
    r = f.add(1.0f, r);
    return f.mul(sign, r);
}
M axisAngle(const V& supplied, float cosineValue, float sineValue, Math& f) noexcept {
    //112C50/28B6A0 normalize XYZ with VADDx+y+z, not the VMADDz
    // variant used by the solver's normalized direction vectors.
    const V a = f.vuNormalize(supplied, false);
    const float oneMinus = f.sub(1.0f, cosineValue);
    V sinAxis{{f.mul(a.v[0], sineValue), f.mul(a.v[1], sineValue), f.mul(a.v[2], sineValue), 0}};
    M out{};
    for (U row = 0; row < 3; ++row) for (U r = 0; r < 3; ++r)
        out.v[row * 4 + r] = f.mul(f.mul(a.v[r], a.v[row]), oneMinus);
    out.v[0] = f.add(out.v[0], cosineValue);
    out.v[1] = f.add(out.v[1], sinAxis.v[2]); out.v[2] = f.sub(out.v[2], sinAxis.v[1]);
    out.v[4] = f.sub(out.v[4], sinAxis.v[2]); out.v[5] = f.add(out.v[5], cosineValue);
    out.v[6] = f.add(out.v[6], sinAxis.v[0]); out.v[8] = f.add(out.v[8], sinAxis.v[1]);
    out.v[9] = f.sub(out.v[9], sinAxis.v[0]); out.v[10] = f.add(out.v[10], cosineValue);
    out.v[15] = 1.0f;
    return out;
}
void lazy(VisualLazyWord49R& s) noexcept { if (s.guard == 0) { s.word = 0xffffffffu; s.guard = 1; } }
bool setLocal(const SceneGraph49P& g, I i, const M& m, Math& f) noexcept {
    return f.valid && setLocalMatrix49P(g, i, m);
}
bool constructAxis(const SceneGraph49P& g, const ChariotIkChainBinding49T& binding,
               const V& axis, ChariotIkChain49T& out, SourceLibmState49T& libm, Math& f) noexcept {
    out.shoulder0 = binding.shoulder; out.elbow4 = binding.elbow; out.wrist8 = binding.wrist;
    const V a = worldPosition(g, binding.shoulder, f);
    const V b = worldPosition(g, binding.elbow, f);
    const V c = worldPosition(g, binding.wrist, f);
    out.savedElbowLocal30 = xyz(node(g, binding.elbow).localC0, 12);
    out.axis50 = axis;
    const V cb = f.difference(b, c), ab = f.difference(a, b);
    const float firstLength = f.length(cb), secondLength = f.length(ab);
    out.lengthSquared14 = f.add(f.mul(firstLength, firstLength), f.mul(secondLength, secondLength));
    //Source recomputes both lengths for18 rather than reading14's operands.
    const float firstAgain = f.length(cb), secondAgain = f.length(ab);
    out.inverseLengthProduct18 = f.div(1.0f, f.mul(f.mul(2.0f, firstAgain), secondAgain));
    const V localA = inverseVector(g, binding.elbow, a, true, f);
    const V localC = inverseVector(g, binding.elbow, c, true, f);
    const float heightA = f.abs(f.dot(localA, out.axis50));
    const float dotA = f.dot(localA, out.axis50);
    V planeA{{0, 0, 0, 1}};
    for (U i = 0; i < 3; ++i) {
        const float p = f.mul(heightA, out.axis50.v[i]);
        planeA.v[i] = dotA <= 0.0f ? f.add(localA.v[i], p) : f.sub(localA.v[i], p);
    }
    const float projectedA = f.length(planeA);
    const float heightC = f.abs(f.dot(localC, out.axis50));
    const float dotC = f.dot(localC, out.axis50);
    V planeC{{0, 0, 0, 1}};
    for (U i = 0; i < 3; ++i) {
        const float p = f.mul(heightC, out.axis50.v[i]);
        planeC.v[i] = dotC <= 0.0f ? f.add(localC.v[i], p) : f.sub(localC.v[i], p);
    }
    const float projectedC = f.length(planeC);
    const float signA = f.dot(localA, out.axis50), signC = f.dot(localC, out.axis50);
    const float axial = (signA < 0.0f) == (signC < 0.0f) ? f.sub(heightA, heightC) : f.add(heightA, heightC);
    out.projectedSquared1C = f.add(f.mul(projectedC, projectedC), f.add(f.mul(projectedA, projectedA), f.mul(axial, axial)));
    out.inverseProjectedProduct20 = f.div(1.0f, f.mul(2.0f, f.mul(projectedC, projectedA)));
    planeA = f.scalarNormalize(planeA);
    const V direction = inverseVector(g, binding.elbow, f.vuNormalize(f.difference(b, c)), false, f);
    planeC = f.scalarNormalize(planeC);
    //28F44C's first24 store is overwritten; retain its arithmetic admission.
    (void)f.dot(planeA, direction);
    for (U i = 0; i < 3; ++i) planeC.v[i] = f.neg(planeC.v[i]);
    out.restCosine24 = f.dot(planeA, planeC);
    out.restSine28 = f.scalarRoot(f.sub(1.0f, f.mul(out.restCosine24, out.restCosine24)));
    out.restAngle2C = acos(out.restCosine24, libm, f);
    const V cross = f.cross(planeA, direction, 1.0f);
    if (!(f.dot(out.axis50, cross) <= 0.0f)) out.restAngle2C = f.neg(out.restAngle2C);
    out.debug40 = 0; out.mode10 = 1;
    return f.valid;
}
bool construct(const SceneGraph49P& g, const ChariotIkChainBinding49T& binding,
               bool right, ChariotIkChain49T& out, SourceLibmState49T& libm, Math& f) noexcept {
    const V axis = right ? V{{fp(0x80000000u), -1.0f, fp(0x80000000u), 1.0f}} : V{{0, 1, 0, 1}};
    return constructAxis(g, binding, axis, out, libm, f);
}
bool wristOrientation(const SceneGraph49P& g, const ChariotIkChainBinding49T& b,
                      VisualShared49R& shared, Math& f) noexcept {
    f.valid = refreshSceneWorld49P(g, b.target) && f.valid;
    const V forward = xyz(node(g, b.target).world100, 8);
    const V up = xyz(node(g, b.target).world100, 4);
    OriginalStandardPlacementGlobals50LA::basisNativeRouteUnknown();
    lazy(shared.orientationSentinel4433D8);
    const V old = xyz(node(g, b.wrist).localC0, 12);
    V normalUp = f.vuNormalize(up, false, true), normalForward = f.vuNormalize(forward, false, true);
    const V right = f.cross(normalUp, normalForward, 0.0f);
    normalUp.v[3] = normalForward.v[3] = 0.0f;
    M requested{};
    for (U i = 0; i < 4; ++i) {
        requested.v[i] = right.v[i]; requested.v[4 + i] = normalUp.v[i];
        requested.v[8 + i] = normalForward.v[i]; requested.v[12 + i] = old.v[i];
    }
    const I parent = node(g, b.wrist).parent180;
    M result = requested;
    if (parent != none) {
        f.valid = refreshSceneWorld49P(g, parent) && f.valid;
        result = relative(requested, node(g, parent).world100, f);
    }
    for (U i = 0; i < 3; ++i) result.v[12 + i] = old.v[i];
    return setLocal(g, b.wrist, result, f);
}
bool solve(const SceneGraph49P& g, const ChariotIkChainBinding49T& b,
           const ChariotIkChain49T& record, SourceLibmState49T& libm,
           ChariotIkShared49T& shared, VisualShared49R& visual, Math& f) noexcept {
    const V target = worldPosition(g, b.target, f);
    lazy(shared.sentinel443450); lazy(shared.sentinel443458);
    const V root = worldPosition(g, record.shoulder0, f);
    V towardTarget = f.difference(target, root);
    const float distance = f.scalarRoot(f.square(towardTarget));
    float candidate = f.mul(record.inverseProjectedProduct20,
        f.sub(f.mul(distance, distance), record.projectedSquared1C));
    if (candidate < -1.0f) candidate = -1.0f;
    const float angle = f.add(acos(candidate, libm, f), record.restAngle2C);
    const float co = cosine(angle, f), si = sine(angle, f);
    M elbow = axisAngle(record.axis50, co, si, f);
    for (U i = 0; i < 3; ++i) elbow.v[12 + i] = record.savedElbowLocal30.v[i];
    if (!setLocal(g, record.elbow4, elbow, f)) return false;
    const V wrist = worldPosition(g, record.wrist8, f);
    const V direction = f.vuNormalize(f.difference(wrist, root));
    towardTarget = f.vuNormalize(towardTarget);
    const V axisWorld = f.vuNormalize(f.cross(direction, towardTarget, 0.0f));
    const V axisLocal = inverseVector(g, record.shoulder0, axisWorld, false, f);
    const float alignment = f.dot(direction, towardTarget);
    const float sineAlignment = f.scalarRoot(f.sub(1.0f, f.mul(alignment, alignment)));
    const M old = node(g, record.shoulder0).localC0;
    M rotated = product(axisAngle(axisLocal, alignment, sineAlignment, f), old, f);
    for (U i = 0; i < 4; ++i) rotated.v[12 + i] = old.v[12 + i];
    if (!setLocal(g, record.shoulder0, rotated, f)) return false;
    return wristOrientation(g, b, visual, f);
}
//28B6A0 multiplies a local axis-angle increment into the existing local,
// then restores all four translation lanes before original cache invalidation.
bool rotateLocalMode0(const SceneGraph49P& g, I i, const V& axis,
                      float co, float si, Math& f) noexcept {
    const M old = node(g, i).localC0;
    M rotated = product(axisAngle(axis, co, si, f), old, f);
    for (U j = 0; j < 4; ++j) rotated.v[12 + j] = old.v[12 + j];
    return setLocal(g, i, rotated, f);
}
bool solveMode0(const SceneGraph49P& g, const ChariotIkChain49T& r,
                const V& target, ModelMode0IkShared50KG& shared, Math& f) noexcept {
    lazy(shared.sentinel443460); lazy(shared.sentinel443468);
    //290230..290258: preserve all three reads, including their lazy caches.
    const V shoulder = worldPosition(g, r.shoulder0, f);
    const V elbow = worldPosition(g, r.elbow4, f);
    const V wrist = worldPosition(g, r.wrist8, f);
    V towardTarget = f.difference(target, shoulder);
    const float distance = f.scalarRoot(f.square(towardTarget));
    //2902CC..29033C: both clamps are present in mode0.
    float desiredCosine = f.mul(r.inverseLengthProduct18,
        f.sub(r.lengthSquared14, f.mul(distance, distance)));
    if (!(desiredCosine <= 1.0f)) desiredCosine = 1.0f;
    if (desiredCosine < -1.0f) desiredCosine = -1.0f;
    const float desiredSine = f.scalarRoot(f.sub(1.0f, f.mul(desiredCosine, desiredCosine)));
    //290340..2904DC: current angle and current bend axis, NOT the rest axis50.
    const V towardShoulder = f.vuNormalize(f.difference(shoulder, elbow));
    const V towardWrist = f.vuNormalize(f.difference(wrist, elbow));
    const float currentCosine = f.dot(towardShoulder, towardWrist);
    const float currentSine = f.scalarRoot(f.sub(1.0f, f.mul(currentCosine, currentCosine)));
    V axis = f.scalarNormalize(f.cross(towardShoulder, towardWrist, 0.0f));
    //2904E0 MULA.S then2904E4 MADD.S form cos(current-desired).
    const float deltaCosine = f.add(f.mul(currentCosine, desiredCosine),
        f.mul(currentSine, desiredSine));
    const float deltaSine = f.scalarRoot(f.sub(1.0f, f.mul(deltaCosine, deltaCosine)));
    if (currentCosine < desiredCosine) {
        for (U j = 0; j < 3; ++j) axis.v[j] = f.mul(-1.0f, axis.v[j]);
        axis.v[3] = 1.0f;
    }
    axis = inverseVector(g, r.elbow4, axis, false, f);
    if (!rotateLocalMode0(g, r.elbow4, axis, deltaCosine, deltaSine, f)) return false;
    //290580..2906D8: align the newly bent chain with the target direction.
    const V newWrist = worldPosition(g, r.wrist8, f);
    const V direction = f.vuNormalize(f.difference(newWrist, shoulder));
    towardTarget = f.vuNormalize(towardTarget);
    const V worldAxis = f.vuNormalize(f.cross(direction, towardTarget, 0.0f));
    const V localAxis = inverseVector(g, r.shoulder0, worldAxis, false, f);
    const float alignment = f.dot(direction, towardTarget);
    const float sineAlignment = f.scalarRoot(f.sub(1.0f, f.mul(alignment, alignment)));
    return rotateLocalMode0(g, r.shoulder0, localAxis, alignment, sineAlignment, f);
}
bool recordContext(const ChariotIkChain49T& r, const ChariotIkChainBinding49T& b, bool right) noexcept {
    if (r.shoulder0 != b.shoulder || r.elbow4 != b.elbow || r.wrist8 != b.wrist || r.mode10 != 1 || r.debug40 != 0) return false;
    const float coefficients[] = {r.lengthSquared14, r.inverseLengthProduct18, r.projectedSquared1C,
        r.inverseProjectedProduct20, r.restCosine24, r.restSine28, r.restAngle2C};
    for (float a : coefficients) if (!std::isfinite(a)) return false;
    for (float a : r.savedElbowLocal30.v) if (!std::isfinite(a)) return false;
    return bits(r.savedElbowLocal30.v[3]) == 0x3f800000u &&
        bits(r.axis50.v[0]) == (right ? 0x80000000u : 0u) &&
        bits(r.axis50.v[1]) == (right ? 0xbf800000u : 0x3f800000u) &&
        bits(r.axis50.v[2]) == (right ? 0x80000000u : 0u) && bits(r.axis50.v[3]) == 0x3f800000u;
}
bool overlap(const void* a, std::size_t n, const void* b, std::size_t m) noexcept {
    if (!n || !m) return false;
    const auto x = reinterpret_cast<std::uintptr_t>(a), y = reinterpret_cast<std::uintptr_t>(b);
    return x <= y ? y - x < n : x - y < m;
}
struct Span { const void* p; std::size_t bytes; };
bool spans(const Span* span, U count) noexcept {
    for (U i = 0; i < count; ++i) for (U j = i + 1; j < count; ++j)
        if (overlap(span[i].p, span[i].bytes, span[j].p, span[j].bytes)) return false;
    return true;
}
void copyGraph(const SceneGraph49P& graph, SceneNode49P* target) noexcept {
    for (U i = 0; i < graph.count; ++i) target[i] = graph.nodes[i];
}
}

bool initializeChariotIk49T(const ChariotIkBindings49T& binding, const SceneGraph49P& graph,
    ChariotIkPair49T& records, SourceLibmState49T& libm, ChariotIkWorkspace49T& work,
    ChariotIkDiagnostic49T& diagnostic) noexcept {
    diagnostic = {};
    if (std::fegetround() != FE_TONEAREST) return fail(diagnostic, ChariotIkError49T::rounding_mode, 0x28eb90);
    if (!finiteGraph(graph)) return fail(diagnostic, ChariotIkError49T::graph, 0x28eb90);
    const Span live[] = {{graph.nodes, graph.count * sizeof(SceneNode49P)}, {&records, sizeof records},
        {&libm, sizeof libm}, {&work, sizeof work}, {&binding, sizeof binding}, {&graph, sizeof graph}, {&diagnostic, sizeof diagnostic}};
    if (!spans(live, 7)) return fail(diagnostic, ChariotIkError49T::records, 0x28eb90);
    if (!chainBinding(graph, binding.left) || !chainBinding(graph, binding.right) || !bindings(graph, binding)) return fail(diagnostic, ChariotIkError49T::bindings, 0x28eb90);
    if (!libmContext(libm)) return fail(diagnostic, ChariotIkError49T::context, 0x302730);
    copyGraph(graph, work.nodes); work.libm = libm;
    const SceneGraph49P staged{work.nodes, graph.count}; Math f;
    if (!construct(staged, binding.left, false, work.records.left, work.libm, f))
        return fail(diagnostic, ChariotIkError49T::arithmetic, 0x28eb90, 0);
    if (!construct(staged, binding.right, true, work.records.right, work.libm, f))
        return fail(diagnostic, ChariotIkError49T::arithmetic, 0x28eb90, 1);
    copyGraph(staged, graph.nodes); records = work.records; libm = work.libm;
    return true;
}

bool initializeChariotIkRecord50LA(const ChariotIkChainBinding49T& binding, const V& axis,
    const SceneGraph49P& graph, ChariotIkChain49T& record, SourceLibmState49T& libm,
    ChariotIkWorkspace49T& work, ChariotIkDiagnostic49T& diagnostic) noexcept {
    diagnostic = {};
    if (std::fegetround() != FE_TONEAREST) return fail(diagnostic, ChariotIkError49T::rounding_mode, 0x28eb90);
    if (!finiteGraph(graph)) return fail(diagnostic, ChariotIkError49T::graph, 0x28eb90);
    const Span live[] = {{graph.nodes, graph.count * sizeof(SceneNode49P)}, {&record, sizeof record},
        {&libm, sizeof libm}, {&work, sizeof work}, {&binding, sizeof binding},
        {&axis, sizeof axis}, {&graph, sizeof graph}, {&diagnostic, sizeof diagnostic}};
    if (!spans(live, 8)) return fail(diagnostic, ChariotIkError49T::records, 0x28eb90);
    if (!chainBinding(graph, binding)) return fail(diagnostic, ChariotIkError49T::bindings, 0x28eb90);
    for (float x : axis.v) if (!std::isfinite(x)) return fail(diagnostic, ChariotIkError49T::context, 0x28eb90);
    if (!libmContext(libm)) return fail(diagnostic, ChariotIkError49T::context, 0x302730);
    copyGraph(graph, work.nodes); work.libm = libm;
    const SceneGraph49P staged{work.nodes, graph.count}; Math f;
    if (!constructAxis(staged, binding, axis, work.records.left, work.libm, f))
        return fail(diagnostic, ChariotIkError49T::arithmetic, 0x28eb90);
    copyGraph(staged, graph.nodes); record = work.records.left; libm = work.libm;
    return true;
}

bool solveChariotHands49T(const ChariotIkBindings49T& binding, const ChariotIkPair49T& records,
    const SceneGraph49P& graph, SourceLibmState49T& libm, ChariotIkShared49T& shared,
    VisualShared49R& visual, ChariotIkWorkspace49T& work, ChariotIkDiagnostic49T& diagnostic) noexcept {
    diagnostic = {};
    if (std::fegetround() != FE_TONEAREST) return fail(diagnostic, ChariotIkError49T::rounding_mode, 0x290a00);
    if (!finiteGraph(graph)) return fail(diagnostic, ChariotIkError49T::graph, 0x290a00);
    const Span live[] = {{graph.nodes, graph.count * sizeof(SceneNode49P)}, {&records, sizeof records},
        {&libm, sizeof libm}, {&shared, sizeof shared}, {&visual, sizeof visual}, {&work, sizeof work},
        {&binding, sizeof binding}, {&graph, sizeof graph}, {&diagnostic, sizeof diagnostic}};
    if (!spans(live, 9)) return fail(diagnostic, ChariotIkError49T::records, 0x290a00);
    if (!bindings(graph, binding)) return fail(diagnostic, ChariotIkError49T::bindings, 0x290a00);
    const bool left = binding.left.target != none, right = binding.right.target != none;
    if (!left && !right) return true;
    if (!libmContext(libm)) return fail(diagnostic, ChariotIkError49T::context, 0x302730);
    if ((left && !recordContext(records.left, binding.left, false)) ||
        (right && !recordContext(records.right, binding.right, true))) return fail(diagnostic, ChariotIkError49T::context, 0x28f580);
    copyGraph(graph, work.nodes); work.libm = libm; work.shared = shared; work.visual = visual;
    const SceneGraph49P staged{work.nodes, graph.count}; Math f;
    if (left && !solve(staged, binding.left, records.left, work.libm, work.shared, work.visual, f))
        return fail(diagnostic, ChariotIkError49T::arithmetic, 0x290a00, 0);
    if (right && !solve(staged, binding.right, records.right, work.libm, work.shared, work.visual, f))
        return fail(diagnostic, ChariotIkError49T::arithmetic, 0x290a00, 1);
    copyGraph(staged, graph.nodes); libm = work.libm; shared = work.shared; visual = work.visual;
    return true;
}
bool initializeModelMode0Ik50KG(I endpoint, const SceneGraph49P& graph,
    ChariotIkChain49T& record, SourceLibmState49T& libm, ChariotIkWorkspace49T& work,
    ChariotIkDiagnostic49T& diagnostic) noexcept {
    diagnostic = {};
    if (std::fegetround() != FE_TONEAREST) return fail(diagnostic, ChariotIkError49T::rounding_mode, 0x1568c8);
    if (!finiteGraph(graph)) return fail(diagnostic, ChariotIkError49T::graph, 0x1568c8);
    const Span live[] = {{graph.nodes, graph.count * sizeof(SceneNode49P)}, {&record, sizeof record},
        {&libm, sizeof libm}, {&work, sizeof work}, {&graph, sizeof graph}, {&diagnostic, sizeof diagnostic}};
    if (!spans(live, 6)) return fail(diagnostic, ChariotIkError49T::records, 0x1568c8);
    if (!index(endpoint, graph.count)) return fail(diagnostic, ChariotIkError49T::bindings, 0x156658);
    const I elbow = node(graph, endpoint).parent180;
    if (!index(elbow, graph.count)) return fail(diagnostic, ChariotIkError49T::bindings, 0x156658);
    const I shoulder = node(graph, elbow).parent180;
    const ChariotIkChainBinding49T binding{shoulder, elbow, endpoint, none};
    if (!chainBinding(graph, binding)) return fail(diagnostic, ChariotIkError49T::bindings, 0x15666c);
    if (!libmContext(libm)) return fail(diagnostic, ChariotIkError49T::context, 0x302730);
    copyGraph(graph, work.nodes); work.libm = libm;
    const SceneGraph49P staged{work.nodes, graph.count}; Math f;
    //433400 is initialized to(+1,+0,+0,1) by3C8DD0..3C8E4C.
    if (!constructAxis(staged, binding, V{{1.0f, 0.0f, 0.0f, 1.0f}}, work.records.left, work.libm, f))
        return fail(diagnostic, ChariotIkError49T::arithmetic, 0x28eb90);
    work.records.left.mode10 = 0; //156920, after the complete constructor.
    copyGraph(staged, graph.nodes); record = work.records.left; libm = work.libm;
    return true;
}

bool solveModelMode0Ik50KG(const ChariotIkChain49T& record, const Vector3_48C& target,
    ModelMode0IkShared50KG& shared, const SceneGraph49P& graph,
    ChariotIkWorkspace49T& work, ChariotIkDiagnostic49T& diagnostic) noexcept {
    diagnostic = {};
    if (std::fegetround() != FE_TONEAREST) return fail(diagnostic, ChariotIkError49T::rounding_mode, 0x28ffd0);
    if (!finiteGraph(graph)) return fail(diagnostic, ChariotIkError49T::graph, 0x28ffd0);
    const Span live[] = {{graph.nodes, graph.count * sizeof(SceneNode49P)}, {&record, sizeof record},
        {&target, sizeof target}, {&shared, sizeof shared}, {&work, sizeof work},
        {&graph, sizeof graph}, {&diagnostic, sizeof diagnostic}};
    if (!spans(live, 7)) return fail(diagnostic, ChariotIkError49T::records, 0x28ffd0);
    const ChariotIkChainBinding49T binding{record.shoulder0, record.elbow4, record.wrist8, none};
    if (!chainBinding(graph, binding)) return fail(diagnostic, ChariotIkError49T::bindings, 0x28ffd0);
    if (record.mode10 != 0 || record.debug40 != 0 ||
        !std::isfinite(record.lengthSquared14) || !std::isfinite(record.inverseLengthProduct18))
        return fail(diagnostic, ChariotIkError49T::context, 0x28f5b8);
    for (float x : target.v) if (!std::isfinite(x)) return fail(diagnostic, ChariotIkError49T::arithmetic, 0x290264);
    copyGraph(graph, work.nodes);
    ModelMode0IkShared50KG stagedShared = shared;
    const SceneGraph49P staged{work.nodes, graph.count}; Math f;
    if (!solveMode0(staged, record, V{{target.v[0], target.v[1], target.v[2], 1.0f}}, stagedShared, f))
        return fail(diagnostic, ChariotIkError49T::arithmetic, 0x28ffd0);
    copyGraph(staged, graph.nodes); shared = stagedShared;
    return true;
}

bool modelMode0IkCallback50KG(const void* context, U chain, const Vector3_48C& target,
    ModelMode0IkShared50KG& shared, const SceneGraph49P& graph) noexcept {
    if (context == nullptr) return false;
    const auto& records = *static_cast<const ModelMode0IkContext50KG*>(context);
    if (records.chains == nullptr || records.count > maxSceneNodes49P || chain >= records.count) return false;
    ChariotIkWorkspace49T work;
    ChariotIkDiagnostic49T diagnostic{};
    return solveModelMode0Ik50KG(records.chains[chain], target, shared, graph, work, diagnostic);
}

}

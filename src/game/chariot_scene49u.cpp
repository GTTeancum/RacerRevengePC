#include "chariot_scene49u.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game::detail {
namespace {
using I = SceneNodeIndex49P;
using V = VisualVector49R;
using M = BodyMatrix48G;
float fp(std::uint32_t word) noexcept { float x; std::memcpy(&x, &word, 4); return x; }
struct Math {
    bool valid = true;
    float read(float a) noexcept { valid = valid && std::isfinite(a); return a; }
    float add(float a, float b) noexcept { read(a); read(b); volatile float x = a + b; return read(x); }
    float sub(float a, float b) noexcept { read(a); read(b); volatile float x = a - b; return read(x); }
    float mul(float a, float b) noexcept { read(a); read(b); volatile float x = a * b; return read(x); }
};
bool index(const SceneGraph49P& g, I i) noexcept {
    const auto n = static_cast<std::int32_t>(i);
    return n >= 0 && static_cast<std::uint32_t>(n) < g.count;
}
bool admit(const SceneGraph49P& g, I i) noexcept {
    return std::fegetround() == FE_TONEAREST && validateSceneGraph49P(g) && index(g, i);
}
SceneNode49P& node(const SceneGraph49P& g, I i) noexcept {
    return g.nodes[static_cast<std::uint32_t>(i)];
}
bool refresh(const SceneGraph49P& g, I i, SceneVectorContext49U& c) noexcept {
    // Only a dirty nonroot executes a VU product. The final product overwrites
    // VF9 with this requested node's first world row, after any ancestor work.
    const bool product = !(node(g, i).sceneFlags190 & 1u) && node(g, i).parent180 != I::none;
    if (!refreshSceneWorld49P(g, i)) return false;
    if (product) c.vf9W = node(g, i).world100.v[3];
    return true;
}
M inverse(const M& m, Math& f) noexcept {
    M out = m;
    // All original ADD moves execute, even on lanes overwritten by transpose.
    for (unsigned i = 0; i < 12; ++i) out.v[i] = f.add(m.v[i], 0.0f);
    out.v[4] = f.add(0.0f, m.v[1]); out.v[8] = f.add(0.0f, m.v[2]);
    out.v[9] = f.add(0.0f, m.v[6]); out.v[1] = f.add(0.0f, m.v[4]);
    out.v[2] = f.add(0.0f, m.v[8]); out.v[6] = f.add(0.0f, m.v[9]);
    for (unsigned i = 12; i < 16; ++i) out.v[i] = f.add(m.v[i], 0.0f);
    for (unsigned r = 0; r < 3; ++r) {
        float a = f.mul(out.v[r], m.v[12]);
        a = f.add(a, f.mul(out.v[4 + r], m.v[13]));
        a = f.add(a, f.mul(out.v[8 + r], m.v[14]));
        out.v[12 + r] = f.mul(a, -1.0f);
    }
    return out;
}
V vector(const M& m, const V& in, bool point, SceneVectorContext49U& c, Math& f) noexcept {
    V out{};
    for (unsigned r = 0; r < 3; ++r) {
        float a = f.mul(m.v[r], in.v[0]);
        a = f.add(a, f.mul(m.v[4 + r], in.v[1]));
        a = f.add(a, f.mul(m.v[8 + r], in.v[2]));
        out.v[r] = f.add(a, point ? m.v[12 + r] : 0.0f);
    }
    if (point) c.vf9W = 1.0f;
    else { volatile float w = c.vf9W + 1.0f; c.vf9W = w; }
    out.v[3] = c.vf9W;
    return out;
}
M transpose(const M& m, Math& f) noexcept {
    M out{};
    for (unsigned i = 0; i < 16; ++i) out.v[i] = f.add(m.v[i], 0.0f);
    out.v[4] = f.add(0.0f, m.v[1]); out.v[8] = f.add(0.0f, m.v[2]);
    out.v[9] = f.add(0.0f, m.v[6]); out.v[1] = f.add(0.0f, m.v[4]);
    out.v[2] = f.add(0.0f, m.v[8]); out.v[6] = f.add(0.0f, m.v[9]);
    return out;
}
M product(const M& local, const M& parent, Math& f) noexcept {
    M out{};
    for (unsigned r = 0; r < 4; ++r) for (unsigned lane = 0; lane < 4; ++lane) {
        float a = f.mul(parent.v[lane], local.v[4 * r]);
        a = f.add(a, f.mul(parent.v[4 + lane], local.v[4 * r + 1]));
        a = f.add(a, f.mul(parent.v[8 + lane], local.v[4 * r + 2]));
        out.v[4 * r + lane] = f.add(a, f.mul(parent.v[12 + lane], local.v[4 * r + 3]));
    }
    return out;
}
M parentRelative(const M& requested, const M& parent, Math& f) noexcept {
    const V delta{{f.sub(requested.v[12], parent.v[12]),
        f.sub(requested.v[13], parent.v[13]), f.sub(requested.v[14], parent.v[14]), 0}};
    const M t = transpose(requested, f);
    M out = transpose(product(parent, t, f), f);
    const M pt = transpose(parent, f);
    for (unsigned r = 0; r < 3; ++r) {
        float a = f.mul(pt.v[r], delta.v[0]);
        a = f.add(a, f.mul(pt.v[4 + r], delta.v[1]));
        out.v[12 + r] = f.add(a, f.mul(pt.v[8 + r], delta.v[2]));
    }
    out.v[15] = 1.0f;
    return out;
}
void sincos(float angle, float& sine, float& cosine, Math& f) noexcept {
    float a = f.read(angle), sign = 1.0f;
    if (!(a <= fp(0x3fc90fdbu))) { a = f.sub(fp(0x40490fdbu), a); sign = -1.0f; }
    else if (a < fp(0xbfc90fdbu)) { a = f.sub(fp(0xc0490fdbu), a); sign = -1.0f; }
    const float sq = f.mul(a, a);
    float r = f.mul(fp(0xb493f27eu), sq);
    r = f.add(fp(0x37d00d01u), r); r = f.mul(sq, r);
    r = f.add(fp(0xbab60b61u), r); r = f.mul(sq, r);
    r = f.add(fp(0x3d2aaaabu), r); r = f.mul(sq, r);
    r = f.add(-0.5f, r); r = f.mul(sq, r); r = f.add(1.0f, r);
    cosine = f.mul(sign, r);
    r = f.mul(fp(0x3638ef1du), sq);
    r = f.add(fp(0xb9500d01u), r); r = f.mul(sq, r);
    r = f.add(fp(0x3c088889u), r); r = f.mul(sq, r);
    r = f.add(fp(0xbe2aaaabu), r);
    sine = f.add(a, f.mul(f.mul(a, sq), r));
}
}

bool axes49U(const SceneGraph49P& g, I i, I relative, SceneVectorContext49U& c,
    V& forward, V& up) noexcept {
    if (!admit(g, i) || !index(g, relative) || i == relative) return false;
    if (!refresh(g, i, c)) return false;
    //28A700 copies both XYZ vectors before either inverse getter is invoked.
    for (unsigned r = 0; r < 3; ++r) forward.v[r] = node(g, i).world100.v[8 + r];
    for (unsigned r = 0; r < 3; ++r) up.v[r] = node(g, i).world100.v[4 + r];
    const V savedForward = forward, savedUp = up;
    return inverseDirection49U(g, relative, savedForward, c, forward) &&
        inverseDirection49U(g, relative, savedUp, c, up);
}
bool position49U(const SceneGraph49P& g, I i, I relative, SceneVectorContext49U& c, V& out) noexcept {
    if (!admit(g, i) || !index(g, relative) || i == relative) return false;
    if (!refresh(g, i, c)) return false;
    for (unsigned r = 0; r < 3; ++r) out.v[r] = node(g, i).world100.v[12 + r];
    const V saved = out;
    if (!refresh(g, relative, c)) return false;
    Math f; const M m = inverse(node(g, relative).world100, f);
    out = vector(m, saved, true, c, f);
    return f.valid;
}
bool direction49U(const SceneGraph49P& g, I i, const V& in,
    SceneVectorContext49U& c, V& out) noexcept {
    if (!admit(g, i) || !refresh(g, i, c)) return false;
    Math f; out = vector(node(g, i).world100, in, false, c, f);
    return f.valid;
}
bool inverseDirection49U(const SceneGraph49P& g, I i, const V& in,
    SceneVectorContext49U& c, V& out) noexcept {
    if (!admit(g, i) || !refresh(g, i, c)) return false;
    Math f; const M m = inverse(node(g, i).world100, f);
    out = vector(m, in, false, c, f);
    return f.valid;
}
bool relativeMatrix49U(const SceneGraph49P& g, I i, I relative,
    SceneVectorContext49U& c, M& out) noexcept {
    if (!admit(g, i) || !index(g, relative) || i == relative) return false;
    if (!refresh(g, i, c)) return false;
    std::memcpy(&out, &node(g, i).world100, sizeof out);
    const I parent = node(g, relative).parent180;
    if (parent == I::none) return true;
    if (!refresh(g, parent, c)) return false;
    Math f; out = parentRelative(out, node(g, parent).world100, f);
    return f.valid;
}
bool copyLocal49U(const SceneGraph49P& g, I i, const M& input) noexcept {
    //28B3E0 first snapshots all16 words, whereas28B490 reads in pairs. Inputs
    // here are disjoint or an exact local matrix, so the snapshot preserves
    // both source forms even for a self assignment.
    M saved; std::memcpy(&saved, &input, sizeof saved);
    return setLocalMatrix49P(g, i, saved);
}
bool rotateVector49U(const V& axis, const V& in, float angle, V& out) noexcept {
    if (std::fegetround() != FE_TONEAREST) return false;
    Math f; float si, co; sincos(angle, si, co, f);
    const float x = f.mul(axis.v[0], in.v[0]);
    const float y = f.mul(axis.v[1], in.v[1]);
    const float z = f.mul(axis.v[2], in.v[2]);
    const float dot = f.add(f.add(x, y), f.mul(1.0f, z));
    float projection[3], delta[3];
    for (unsigned r = 0; r < 3; ++r) projection[r] = f.mul(dot, axis.v[r]);
    for (unsigned r = 0; r < 3; ++r) delta[r] = f.sub(in.v[r], projection[r]);
    const float cross[3] = {
        f.sub(f.mul(delta[2], axis.v[1]), f.mul(delta[1], axis.v[2])),
        f.sub(f.mul(delta[0], axis.v[2]), f.mul(delta[2], axis.v[0])),
        f.sub(f.mul(delta[1], axis.v[0]), f.mul(delta[0], axis.v[1]))};
    float result[3];
    for (unsigned r = 0; r < 3; ++r)
        result[r] = f.add(projection[r], f.add(f.mul(co, delta[r]), f.mul(si, cross[r])));
    if (!f.valid) return false;
    for (unsigned r = 0; r < 3; ++r) out.v[r] = result[r];
    return true;
}
bool matrixQuaternion49U(const M& in, Quaternion48C& out) noexcept {
    return std::fegetround() == FE_TONEAREST && matrixToQuaternion48G(in, out);
}
}

#include "graph_body49w.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using I = SceneNodeIndex49P;
using V = VisualVector49R;
using M = BodyMatrix48G;
using D = GraphBodyDiagnostic49W;
float fp(std::uint32_t word) noexcept { float x; std::memcpy(&x, &word, 4); return x; }
std::uint32_t bits(float x) noexcept { std::uint32_t word; std::memcpy(&word, &x, 4); return word; }
struct Math {
    bool valid = true;
    float read(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a, float b) noexcept { read(a); read(b); volatile float x = a + b; return read(x); }
    float sub(float a, float b) noexcept { read(a); read(b); volatile float x = a - b; return read(x); }
    float mul(float a, float b) noexcept { read(a); read(b); volatile float x = a * b; return read(x); }
    float div(float a, float b) noexcept { read(a); read(b); volatile float x = a / b; return read(x); }
    float square(const V& v) noexcept {
        return add(add(mul(v.v[0], v.v[0]), mul(v.v[1], v.v[1])), mul(v.v[2], v.v[2]));
    }
    float scalarRoot(float x) noexcept {
        read(x);
        // Original EE scalar sqrt flushes a subnormal input to signed zero.
        if ((bits(x) & 0x7f800000u) == 0) return fp(bits(x) & 0x80000000u);
        volatile float result = static_cast<float>(std::sqrt(static_cast<double>(x)));
        return read(result);
    }
    float cosine(float x) noexcept {
        read(x); float sign = 1.0f;
        if (x > fp(0x3fc90fdbu)) { x = sub(fp(0x40490fdbu), x); sign = -1.0f; }
        else if (x < fp(0xbfc90fdbu)) { x = sub(fp(0xc0490fdbu), x); sign = -1.0f; }
        const float d = mul(x, x);
        float t = add(fp(0x37d00d01u), mul(fp(0xb493f27eu), d));
        t = add(fp(0xbab60b61u), mul(d, t)); t = add(fp(0x3d2aaaabu), mul(d, t));
        t = add(-0.5f, mul(d, t)); return mul(sign, add(1.0f, mul(d, t)));
    }
    float sine(float x) noexcept {
        read(x);
        if (x > fp(0x3fc90fdbu)) x = sub(fp(0x40490fdbu), x);
        else if (x < fp(0xbfc90fdbu)) x = sub(fp(0xc0490fdbu), x);
        const float d = mul(x, x);
        float t = add(fp(0xb9500d01u), mul(fp(0x3638ef1du), d));
        t = add(fp(0x3c088889u), mul(d, t)); t = add(fp(0xbe2aaaabu), mul(d, t));
        return add(x, mul(mul(x, d), t));
    }
};
bool index(const SceneGraph49P& graph, I i) noexcept {
    const auto n = static_cast<std::int32_t>(i);
    return n >= 0 && static_cast<std::uint32_t>(n) < graph.count;
}
SceneNode49P& node(const SceneGraph49P& graph, I i) noexcept {
    return graph.nodes[static_cast<std::uint32_t>(i)];
}
bool admitLeaf(const SceneGraph49P& graph, I i) noexcept {
    return std::fegetround() == FE_TONEAREST && validateSceneGraph49P(graph) && index(graph, i);
}
struct Range { const void* data; std::size_t bytes; };
bool overlaps(Range a, Range b) noexcept {
    if (!a.bytes || !b.bytes) return false;
    const auto x = reinterpret_cast<std::uintptr_t>(a.data);
    const auto y = reinterpret_cast<std::uintptr_t>(b.data);
    return x <= y ? y - x < a.bytes : x - y < b.bytes;
}
V vector(const Vector3_48C& v, float w = 0.0f) noexcept {
    return {{v.v[0], v.v[1], v.v[2], w}};
}
void xyz(Vector3_48C& out, const V& in) noexcept { std::memcpy(out.v, in.v, sizeof out.v); }
void clear(GraphBodyWorkspace49W& w) noexcept {
    // 168304,168310,16831C,168328,168334,168340, then1654B4.
    w.body.loads.force = {}; w.force180W = 1.0f;
    w.body.loads.torque = {}; w.torque190W = 1.0f;
    w.body.accumulator1c0 = {}; w.extra.accumulator1C0W = 1.0f;
    w.body.accumulator1d0 = {}; w.extra.accumulator1D0W = 1.0f;
    w.body.positionCorrection1a0 = {}; w.extra.positionCorrection1A0W = 1.0f;
    w.body.rotationCorrection1b0 = {}; w.extra.rotationCorrection1B0W = 1.0f;
    w.body.callbackState238 = 0;
}
}

namespace detail {
bool pointGraph49W(const SceneGraph49P& graph, I i, const V& input,
    SceneVectorContext49U& context, V& output) noexcept {
    if (!admitLeaf(graph, i) || !refreshSceneWorld49P(graph, i)) return false;
    Math f; V result{}; const auto& m = node(graph, i).world100;
    // 28BB78 writesVF9.W=1 independently of all prior VU scratch state.
    context.vf9W = 1.0f; result.v[3] = 1.0f;
    for (unsigned r = 0; r < 3; ++r) {
        float a = f.mul(m.v[r], input.v[0]);
        a = f.add(a, f.mul(m.v[4 + r], input.v[1]));
        a = f.add(a, f.mul(m.v[8 + r], input.v[2]));
        result.v[r] = f.add(a, m.v[12 + r]);
    }
    if (!f.valid) return false;
    output = result; return true;
}

bool rotateAxisGraph49W(const SceneGraph49P& graph, I i, const V& input, float angle) noexcept {
    if (!admitLeaf(graph, i)) return false;
    Math f;
    // The original28B840 calls13BBA0 before13BAD0, then112B20.
    const float c = f.cosine(angle), s = f.sine(angle), t = f.sub(1.0f, c);
    V axis = input; const float d = f.square(axis);
    float q;
    //112B90 VRSQRT uses VU sqrt then divide (not the scalar EE flush rule).
    if (d == 0.0f) q = fp((bits(d) & 0x80000000u) | 0x7f7fffffu);
    else { volatile float root = std::sqrt(std::fabs(d)); q = f.div(1.0f, root); }
    for (unsigned r = 0; r < 3; ++r) axis.v[r] = f.mul(axis.v[r], q);
    M rotation{}; float scaled[3];
    for (unsigned r = 0; r < 3; ++r) scaled[r] = f.mul(axis.v[r], s);
    for (unsigned col = 0; col < 3; ++col) for (unsigned r = 0; r < 3; ++r)
        rotation.v[4 * col + r] = f.mul(f.mul(axis.v[r], axis.v[col]), t);
    rotation.v[0] = f.add(rotation.v[0], c);
    rotation.v[1] = f.add(rotation.v[1], scaled[2]);
    rotation.v[2] = f.sub(rotation.v[2], scaled[1]);
    rotation.v[4] = f.sub(rotation.v[4], scaled[2]);
    rotation.v[5] = f.add(rotation.v[5], c);
    rotation.v[6] = f.add(rotation.v[6], scaled[0]);
    rotation.v[8] = f.add(rotation.v[8], scaled[1]);
    rotation.v[9] = f.sub(rotation.v[9], scaled[0]);
    rotation.v[10] = f.add(rotation.v[10], c);
    rotation.v[3] = rotation.v[7] = rotation.v[11] = f.sub(0.0f, 0.0f);
    rotation.v[15] = 1.0f;
    const M old = node(graph, i).localC0; M result{};
    for (unsigned col = 0; col < 4; ++col) for (unsigned r = 0; r < 4; ++r) {
        float a = f.mul(old.v[r], rotation.v[4 * col]);
        a = f.add(a, f.mul(old.v[4 + r], rotation.v[4 * col + 1]));
        a = f.add(a, f.mul(old.v[8 + r], rotation.v[4 * col + 2]));
        result.v[4 * col + r] = f.add(a, f.mul(old.v[12 + r], rotation.v[4 * col + 3]));
    }
    //112C40 restores the complete saved local translation, after the product.
    std::memcpy(result.v + 12, old.v + 12, 4 * sizeof(float));
    if (!f.valid) return false;
    // Identical28C4F0 + ready-gated289A20 continuation after local mutation.
    return setLocalMatrix49P(graph, i, result);
}
}

bool advanceGraphBody49W(RootBodyState48H& body,
    SecondaryActivationSupplement49V& supplement, GraphBodyExtraLanes49W& extra,
    GraphBodyLaneRefs49W lanes, std::uint32_t& sceneFlags190,
    const BodyParameters48C& parameters, const Vector3_48C& centerOffset,
    I binding, bool enabled, const GraphBodyDomain49W& domain, float dt,
    const SceneGraph49P& graph, GraphBodyWorkspace49W& w, D& diagnostic) noexcept {
    const Range ranges[] = {
        {&body, sizeof body}, {&supplement, sizeof supplement}, {&extra, sizeof extra},
        {&lanes.velocity70W, sizeof(float)}, {&lanes.angularVelocityB0W, sizeof(float)},
        {&lanes.force180W, sizeof(float)}, {&lanes.torque190W, sizeof(float)},
        {&sceneFlags190, sizeof sceneFlags190}, {&parameters, sizeof parameters},
        {&centerOffset, sizeof centerOffset}, {&domain, sizeof domain},
        {&graph, sizeof graph}, {graph.nodes, std::size_t(graph.count) * sizeof(SceneNode49P)},
        {&w, sizeof w}
    };
    // Do not write an overlapping diagnostic: doing so would violate atomicity.
    for (const auto& r : ranges) if (overlaps(r, {&diagnostic, sizeof diagnostic})) return false;
    for (unsigned i = 0; i < sizeof ranges / sizeof ranges[0]; ++i)
        for (unsigned j = i + 1; j < sizeof ranges / sizeof ranges[0]; ++j)
            if (overlaps(ranges[i], ranges[j])) { diagnostic = D::StorageAlias; return false; }
    diagnostic = D::None;
    const auto fail = [&diagnostic](D reason) noexcept { diagnostic = reason; return false; };
    if (std::fegetround() != FE_TONEAREST) return fail(D::RoundingMode);
    if ((domain.componentFlags24 & 2u) && domain.firstComponentChildPresent18) return fail(D::ComponentRoute);
    if (domain.callbackMethod9C != GraphBodyCallback49W::Clear238) return fail(D::CallbackMethod);
    if (!validateSceneGraph49P(graph)) return fail(D::GraphTopology);
    if (binding != I::none) {
        if (!index(graph, binding) || node(graph, binding).parent180 != I::none) return fail(D::SceneBinding);
        if (std::memcmp(&body.scene, &node(graph, binding).world100, sizeof body.scene) != 0 ||
            static_cast<std::uint8_t>(sceneFlags190) != node(graph, binding).sceneFlags190)
            return fail(D::SceneProjection);
    }
    const bool active = binding != I::none && enabled;
    if (active && domain.rootObserverPresent19C) return fail(D::Observer);
    if (active && domain.integrationMethod290 != GraphBodyIntegrator49W::OriginalAdamsBashforth2) return fail(D::IntegrationMethod);
    // All independently-owned state is staged before any numerical operation.
    std::memcpy(&w.body, &body, sizeof body);
    std::memcpy(&w.supplement, &supplement, sizeof supplement);
    std::memcpy(&w.extra, &extra, sizeof extra);
    std::memcpy(&w.force180W, &lanes.force180W, sizeof(float));
    std::memcpy(&w.torque190W, &lanes.torque190W, sizeof(float));
    if (graph.count) std::memcpy(w.nodes, graph.nodes, graph.count * sizeof(SceneNode49P));
    const SceneGraph49P staged{w.nodes, graph.count};
    if (active) {
        auto& b = w.body;
        auto& s = w.supplement;
        detail::SceneVectorContext49U context{0.0f};
        b.previousVelocity90 = b.body.velocity;
        std::memcpy(&s.previousVelocity90W, &lanes.velocity70W, sizeof(float));
        V point{};
        if (!detail::pointGraph49W(staged, binding, vector(centerOffset), context, point)) return fail(D::Arithmetic);
        xyz(b.body.position, point); w.extra.position60W = point.v[3];
        std::memcpy(&b.scene, &node(staged, binding).world100, sizeof b.scene);
#ifdef RR_GRAPH_BODY49W_STAGE
        RR_GRAPH_BODY49W_STAGE(0x167ca0u, b, s, w.extra, staged);
#endif
        if (!matrixToQuaternion48G(node(staged, binding).world100, b.body.orientation)) return fail(D::Arithmetic);
        if (!integrateBody48C(b.body, parameters, b.loads, dt, b.body)) return fail(D::Arithmetic);
        s.positionDerivative2B8 = b.body.velocity;
        s.linearAcceleration2D0 = b.body.linearAcceleration;
        s.angularAcceleration308 = b.body.angularAcceleration;
        s.linearAccelerationC0W = 1.0f;
        s.angularAccelerationD0W = 2.0f;
        M orientation{};
        if (!quaternionToMatrix48G(b.body.orientation, orientation) ||
            !setParentlessWorldMatrix49P(staged, binding, orientation)) return fail(D::Arithmetic);
#ifdef RR_GRAPH_BODY49W_STAGE
        RR_GRAPH_BODY49W_STAGE(0x168128u, b, s, w.extra, staged);
#endif
        Math f;
        const auto& correction = b.rotationCorrection1b0.v;
        for (float x : correction) f.read(x);
        if (!f.valid) return fail(D::Arithmetic);
        if (correction[0] != 0.0f || correction[1] != 0.0f || correction[2] != 0.0f) {
            V axis{};
            if (!detail::direction49U(staged, binding, vector(b.rotationCorrection1b0), context, axis)) return fail(D::Arithmetic);
            std::memcpy(&b.scene, &node(staged, binding).world100, sizeof b.scene);
            const float length = f.scalarRoot(f.square(axis));
            if (!f.valid) return fail(D::Arithmetic);
            //1681F0 skips28B840 even for a nonzero input whose length underflows.
            if (length != 0.0f) {
                const float inverseLength = f.div(1.0f, length);
                for (unsigned r = 0; r < 3; ++r) axis.v[r] = f.mul(axis.v[r], inverseLength);
                if (!f.valid || !detail::rotateAxisGraph49W(staged, binding, axis, length)) return fail(D::Arithmetic);
            }
        }
#ifdef RR_GRAPH_BODY49W_STAGE
        RR_GRAPH_BODY49W_STAGE(0x168234u, b, s, w.extra, staged);
#endif
        // This second point getter always establishesVF9.W=1 before the suffix.
        if (!detail::pointGraph49W(staged, binding, vector(centerOffset), context, point)) return fail(D::Arithmetic);
        std::memcpy(&b.scene, &node(staged, binding).world100, sizeof b.scene);
#ifdef RR_GRAPH_BODY49W_STAGE
        RR_GRAPH_BODY49W_STAGE(0x168244u, b, s, w.extra, staged);
#endif
        float delta[3];
        for (unsigned r = 0; r < 3; ++r) delta[r] = f.sub(b.positionCorrection1a0.v[r], point.v[r]);
        for (unsigned r = 0; r < 3; ++r) b.body.position.v[r] = f.add(b.body.position.v[r], delta[r]);
        const SceneDisplacement49P displacement{{b.body.position.v[0], b.body.position.v[1], b.body.position.v[2], w.extra.position60W}};
        if (!f.valid || !addParentlessTranslation49P(staged, binding, displacement)) return fail(D::Arithmetic);
#ifdef RR_GRAPH_BODY49W_STAGE
        RR_GRAPH_BODY49W_STAGE(0x1682c4u, b, s, w.extra, staged);
#endif
        // Actual admitted observer19C==0 takes28EA30->28EA64: no effects.
        V localVelocity{}, worldAngularVelocity{};
        if (!detail::inverseDirection49U(staged, binding, vector(b.body.velocity, lanes.velocity70W), context, localVelocity)) return fail(D::Arithmetic);
        xyz(b.localVelocity80, localVelocity); s.localVelocity80W = localVelocity.v[3];
        if (!detail::direction49U(staged, binding, vector(b.body.angularVelocity, lanes.angularVelocityB0W), context, worldAngularVelocity)) return fail(D::Arithmetic);
        xyz(b.worldAngularVelocityA0, worldAngularVelocity); w.extra.worldAngularVelocityA0W = worldAngularVelocity.v[3];
        std::memcpy(&b.scene, &node(staged, binding).world100, sizeof b.scene);
#ifdef RR_GRAPH_BODY49W_STAGE
        RR_GRAPH_BODY49W_STAGE(0x1682fcu, b, s, w.extra, staged);
#endif
    }
    clear(w);
#ifdef RR_GRAPH_BODY49W_STAGE
    RR_GRAPH_BODY49W_STAGE(0x168354u, w.body, w.supplement, w.extra, staged);
#endif
    std::memcpy(&body, &w.body, sizeof body);
    std::memcpy(&supplement, &w.supplement, sizeof supplement);
    std::memcpy(&extra, &w.extra, sizeof extra);
    std::memcpy(&lanes.force180W, &w.force180W, sizeof(float));
    std::memcpy(&lanes.torque190W, &w.torque190W, sizeof(float));
    if (graph.count) std::memcpy(graph.nodes, w.nodes, graph.count * sizeof(SceneNode49P));
    if (active) sceneFlags190 = (sceneFlags190 & 0xffffff00u) | node(staged, binding).sceneFlags190;
    return true;
}
}

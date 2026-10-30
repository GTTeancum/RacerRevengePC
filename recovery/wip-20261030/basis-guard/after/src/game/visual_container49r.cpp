#include "original_standard_placement_globals50la.h"
#include "vehicle_visual49r_internal.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game::detail {
namespace {
using V = VisualVector49R;
using M = BodyMatrix48G;
using I = SceneNodeIndex49P;

float fromBits(std::uint32_t word) noexcept {
    float value;
    std::memcpy(&value, &word, sizeof value);
    return value;
}
std::uint32_t bits(float value) noexcept {
    std::uint32_t word;
    std::memcpy(&word, &value, sizeof word);
    return word;
}
bool indexInRange(I index, std::uint32_t count) noexcept {
    const auto integer = static_cast<std::int32_t>(index);
    return integer >= 0 && static_cast<std::uint32_t>(integer) < count;
}
SceneNode49P& node(const SceneGraph49P& graph, I index) noexcept {
    return graph.nodes[static_cast<std::uint32_t>(index)];
}
bool fail(VehicleVisualDiagnostic49R& diagnostic,
          VehicleVisualError49R error, std::uint32_t stage) noexcept {
    diagnostic = {error, stage};
    return false;
}
struct Arithmetic {
    bool valid = true;
    float keep(float value) noexcept {
        valid = valid && std::isfinite(value);
        return value;
    }
    float add(float a, float b) noexcept {
        volatile float value = a + b;
        return keep(value);
    }
    float sub(float a, float b) noexcept {
        volatile float value = a - b;
        return keep(value);
    }
    float mul(float a, float b) noexcept {
        volatile float value = a * b;
        return keep(value);
    }
    float div(float a, float b) noexcept {
        volatile float value = a / b;
        return keep(value);
    }
    float reciprocalRoot(float denominator) noexcept {
        // Retained VU reference semantics, unlike scalar EE RSQRT.S:
        // separately rounded binary32 sqrt followed by binary32 division.
        // The original zero-denominator Q is maximum finite, not a fallback
        // axis or an early-out. XYZ multiplication still executes afterward.
        if (denominator == 0.0f)
            return fromBits((bits(denominator) & 0x80000000u) | 0x7f7fffffu);
        if (!std::isfinite(denominator) || denominator < 0.0f) {
            valid = false;
            return 0.0f;
        }
        //49R admits zero and positive normal squared norms. Subnormal
        // denominators would enter the reference's operand-flush behavior,
        // outside this retained finite binary32 instruction model.
        if ((bits(denominator) & 0x7f800000u) == 0) {
            valid = false;
            return 0.0f;
        }
        volatile float root = std::sqrt(denominator);
        return div(1.0f, keep(root));
    }
    V normalize(const V& value, bool prefix) noexcept {
        const float x = mul(value.v[0], value.v[0]);
        const float y = mul(value.v[1], value.v[1]);
        const float z = mul(value.v[2], value.v[2]);
        const float xy = add(x, y);
        //21C3AC uses VMADDz with the generated1. The two28A090
        // normalizations use VADDz and must remain distinct source steps.
        const float denominator = add(xy, prefix ? mul(add(0.0f, 1.0f), z) : z);
        const float q = reciprocalRoot(denominator);
        V result{};
        result.v[3] = prefix ? add(value.v[3], 0.0f) : add(0.0f, value.v[3]);
        for (unsigned lane = 0; lane < 3; ++lane)
            result.v[lane] = mul(value.v[lane], q);
        return result;
    }
    V cross(const V& a, const V& b) noexcept {
        V result{{0.0f, 0.0f, 0.0f, 0.0f}};
        for (unsigned lane = 0; lane < 3; ++lane) {
            const unsigned j = (lane + 1) % 3, k = (lane + 2) % 3;
            result.v[lane] = sub(mul(a.v[j], b.v[k]), mul(b.v[j], a.v[k]));
        }
        return result;
    }
    M transposeMoves(const M& input) noexcept {
        M result{};
        //289DBC..289DE0 /289E2C..289E50: all four rows first add
        // positive zero, then six off-diagonal lanes add in reverse order.
        for (unsigned lane = 0; lane < 16; ++lane)
            result.v[lane] = add(input.v[lane], 0.0f);
        result.v[4] = add(0.0f, input.v[1]);
        result.v[8] = add(0.0f, input.v[2]);
        result.v[9] = add(0.0f, input.v[6]);
        result.v[1] = add(0.0f, input.v[4]);
        result.v[2] = add(0.0f, input.v[8]);
        result.v[6] = add(0.0f, input.v[9]);
        return result;
    }
    M relativeMatrix(const M& requested, const M& parent) noexcept {
        //289D50 is its original two-transpose relative conversion. It is
        // not a general inverse, including for a non-rigid supplied parent.
        V delta{{sub(requested.v[12], parent.v[12]),
                 sub(requested.v[13], parent.v[13]),
                 sub(requested.v[14], parent.v[14]), 0.0f}};
        const M transposed = transposeMoves(requested);
        M intermediate{};
        for (unsigned row = 0; row < 4; ++row) {
            for (unsigned lane = 0; lane < 4; ++lane) {
                float value = mul(transposed.v[lane], parent.v[row * 4]);
                value = add(value, mul(transposed.v[4 + lane], parent.v[row * 4 + 1]));
                value = add(value, mul(transposed.v[8 + lane], parent.v[row * 4 + 2]));
                intermediate.v[row * 4 + lane] =
                    add(value, mul(transposed.v[12 + lane], parent.v[row * 4 + 3]));
            }
        }
        M result = transposeMoves(intermediate);
        const M parentTranspose = transposeMoves(parent);
        // These values are later overwritten by28A090's XYZ restoration,
        // but their original arithmetic and finite admission still execute.
        for (unsigned lane = 0; lane < 3; ++lane) {
            float value = mul(parentTranspose.v[lane], delta.v[0]);
            value = add(value, mul(parentTranspose.v[4 + lane], delta.v[1]));
            result.v[12 + lane] = add(value, mul(parentTranspose.v[8 + lane], delta.v[2]));
        }
        result.v[15] = add(0.0f, 1.0f);
        return result;
    }
};
V readXYZ(const M& matrix, unsigned offset) noexcept {
    //28B1A0/28A700 overwrite XYZ only; each caller stack vector starts VF0.
    V result{{0.0f, 0.0f, 0.0f, 1.0f}};
    std::memcpy(result.v, matrix.v + offset, 3 * sizeof(float));
    return result;
}
}

bool prepareVisualContainer49R(const SceneGraph49P& graph,
    const VisualSceneBindings49R& bindings, std::uint32_t probe108,
    VehicleVisualState49R& state, VisualShared49R& shared,
    VisualFrame49R& frame, VehicleVisualDiagnostic49R& diagnostic) noexcept {
    if (std::fegetround() != FE_TONEAREST)
        return fail(diagnostic, VehicleVisualError49R::rounding_mode, 0x21c120);
    if (!validateSceneGraph49P(graph) ||
        !indexInRange(bindings.main190, graph.count) ||
        !indexInRange(bindings.container194, graph.count) ||
        bindings.main190 == bindings.container194 ||
        node(graph, bindings.main190).parent180 != I::none ||
        node(graph, bindings.container194).parent180 != bindings.main190)
        return fail(diagnostic, VehicleVisualError49R::graph, 0x21c120);

    constexpr std::uint32_t coldWords[6] = {
        0x3f59999au, 0x3f19999au, 0x42c80000u,
        0x3e4ccccdu, 0x3e4ccccdu, 0x40800000u
    };
    for (unsigned i = 0; i < 6; ++i) {
        if (shared.globals[i].guard == 0) {
            shared.globals[i].word = coldWords[i];
            shared.globals[i].guard = 1;
        }
    }
    if (!refreshSceneWorld49P(graph, bindings.main190))
        return fail(diagnostic, VehicleVisualError49R::arithmetic, 0x21c2cc);
    const auto& mainWorld = node(graph, bindings.main190).world100;
    frame.mainPosition = readXYZ(mainWorld, 12);
    frame.correctedForward = readXYZ(mainWorld, 8);
    frame.mainUp = readXYZ(mainWorld, 4);

    Arithmetic f;
    const float alpha = fromBits(shared.globals[probe108 == 0 ? 0 : 1].word);
    const float beta = f.sub(1.0f, alpha);
    V currentWeighted{{0.0f, 0.0f, 0.0f, 1.0f}};
    V oldWeighted{{0.0f, 0.0f, 0.0f, 1.0f}};
    for (unsigned lane = 0; lane < 3; ++lane)
        currentWeighted.v[lane] = f.mul(beta, frame.mainUp.v[lane]);
    for (unsigned lane = 0; lane < 3; ++lane)
        oldWeighted.v[lane] = f.mul(alpha, state.upC20.v[lane]);
    for (unsigned lane = 0; lane < 3; ++lane)
        state.upC20.v[lane] = f.add(oldWeighted.v[lane], currentWeighted.v[lane]);
    state.upC20.v[3] = 1.0f;
    state.upC20 = f.normalize(state.upC20, true);
    const V intermediateRight = f.cross(frame.correctedForward, state.upC20);
    // This is the actual sp+B0 value at the21C440 boundary, not the old
    // getter value that21C3F8 overwrote. MainUp remains the original getter.
    frame.correctedForward = f.cross(state.upC20, intermediateRight);
    if (!f.valid)
        return fail(diagnostic, VehicleVisualError49R::arithmetic, 0x21c3f8);

    OriginalStandardPlacementGlobals50LA::basisNativeRouteUnknown();
    if (shared.orientationSentinel4433D8.guard == 0) {
        shared.orientationSentinel4433D8.word = 0xffffffffu;
        shared.orientationSentinel4433D8.guard = 1;
    }
    auto& child = node(graph, bindings.container194);
    const V oldTranslation = readXYZ(child.localC0, 12);
    //28A090(null, correctedForward, C20, 0,1,0): t0!=1 skips the
    // optional orthogonalization. Both normalizations still execute.
    V up = f.normalize(state.upC20, false);
    V forward = f.normalize(frame.correctedForward, false);
    const V right = f.cross(up, forward);
    up.v[3] = forward.v[3] = 0.0f;
    M requested{};
    for (unsigned lane = 0; lane < 4; ++lane) {
        requested.v[lane] = right.v[lane];
        requested.v[4 + lane] = up.v[lane];
        requested.v[8 + lane] = forward.v[lane];
        requested.v[12 + lane] = oldTranslation.v[lane];
    }
    if (!refreshSceneWorld49P(graph, bindings.main190))
        return fail(diagnostic, VehicleVisualError49R::arithmetic, 0x28a474);
    M local = f.relativeMatrix(requested, node(graph, bindings.main190).world100);
    for (unsigned lane = 0; lane < 3; ++lane)
        local.v[12 + lane] = oldTranslation.v[lane];
    if (!f.valid)
        return fail(diagnostic, VehicleVisualError49R::arithmetic, 0x289d50);
    // Compose the proven49P final-local setter/flag transition. The source's
    // temporary matrix stores remain arithmetic temporaries here: this API
    // claims identical represented state and ordered flag stores, not every
    // intermediate local-matrix memory write in289D50/28A090.
    if (!setLocalMatrix49P(graph, bindings.container194, local) ||
        !refreshSceneWorld49P(graph, bindings.container194))
        return fail(diagnostic, VehicleVisualError49R::arithmetic, 0x21c41c);
    const auto& childWorld = child.world100;
    frame.containerPosition = readXYZ(childWorld, 12);
    frame.containerForward = readXYZ(childWorld, 8);
    frame.containerUp = readXYZ(childWorld, 4);
    state.publishedUpAF0 = frame.containerUp;
    diagnostic = {VehicleVisualError49R::none, 0x21c440};
    return true;
}
}

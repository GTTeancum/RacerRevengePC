#include "engine_animation49q.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using Index = SceneNodeIndex49P;
bool reject(EngineAnimationDiagnostic49Q& d, EngineAnimationError49Q e,
            std::uint32_t engine = 0) noexcept {
    d.error = e; d.engine = engine; return false;
}
float value(std::uint32_t word) noexcept {
    float f; std::memcpy(&f, &word, sizeof f); return f;
}
struct Arithmetic {
    bool valid = true;
    float keep(float f) noexcept { valid = valid && std::isfinite(f); return f; }
    float add(float a, float b) noexcept { volatile float f = a + b; return keep(f); }
    float sub(float a, float b) noexcept { volatile float f = a - b; return keep(f); }
    float mul(float a, float b) noexcept { volatile float f = a * b; return keep(f); }
    float div(float a, float b) noexcept { volatile float f = a / b; return keep(f); }
};
bool root(const SceneGraph49P& g, Index i) noexcept {
    const auto n = static_cast<std::int32_t>(i);
    return n >= 0 && static_cast<std::uint32_t>(n) < g.count &&
        g.nodes[n].parent180 == Index::none;
}
bool modelBinding(const SceneGraph49P& g, const EngineMotionModel49Q& model) noexcept {
    if (!model.bindings || model.bindings->count == 0 || model.bindings->count > 64)
        return false;
    for (std::uint32_t row = 0; row < model.bindings->count; ++row) {
        auto i = static_cast<Index>(model.bindings->rows[row].sceneNode);
        if (static_cast<std::uint32_t>(i) >= g.count || i == model.modelRoot) return false;
        while (i != Index::none && i != model.modelRoot)
            i = g.nodes[static_cast<std::uint32_t>(i)].parent180;
        if (i != model.modelRoot) return false;
    }
    return true;
}
bool active(const EngineMotionModel49Q& m) noexcept {
    return m.motion && m.modelRoot != Index::none && (m.sceneFlags191 & 1u) != 0;
}
bool overlap(const void* a, std::size_t aBytes, const void* b, std::size_t bBytes) noexcept {
    if (aBytes == 0 || bBytes == 0) return false;
    const auto x = reinterpret_cast<std::uintptr_t>(a);
    const auto y = reinterpret_cast<std::uintptr_t>(b);
    return x <= y ? y - x < aBytes : x - y < bBytes;
}
bool records(const EngineAnimationInput49Q& input, const EngineAnimationModels49Q& models,
             const SceneGraph49P& graph, const EngineAnimationState49Q& state,
             const EngineAnimationWorkspace49Q& work) noexcept {
    const auto graphBytes = graph.count * sizeof(SceneNode49P);
    if (overlap(graph.nodes, graphBytes, &state, sizeof state) ||
        overlap(graph.nodes, graphBytes, &work, sizeof work) ||
        overlap(&state, sizeof state, &work, sizeof work) ||
        overlap(&input, sizeof input, &work, sizeof work) ||
        overlap(&models, sizeof models, &work, sizeof work) ||
        overlap(&graph, sizeof graph, &work, sizeof work)) return false;
    const EngineMotionModel49Q* model[2] = {&models.left, &models.right};
    for (const auto* m : model) {
        if (!active(*m)) continue;
        if (overlap(m->motion, sizeof *m->motion, &work, sizeof work) ||
            (m->bindings && overlap(m->bindings, sizeof *m->bindings, &work, sizeof work)) ||
            overlap(m->motion->data, m->motion->bytes, &work, sizeof work)) return false;
    }
    return true;
}
bool filter(AnimationFilter49Q& state, float target, float dt, float& mirror) noexcept {
    if (!std::isfinite(state.value0) || !std::isfinite(state.time4) ||
        !(state.time4 > 0) || !std::isfinite(target)) return false;
    Arithmetic f;
    const float limited = detail::scalarMin48O(dt, state.time4);
    state.alpha8 = f.div(limited, state.time4);
    const float difference = f.sub(target, state.value0);
    const float increment = f.mul(state.alpha8, difference);
    state.value0 = f.add(state.value0, increment);
    mirror = state.value0;
    return f.valid;
}
float clamp(float f) noexcept {
    return detail::scalarMin48O(detail::scalarMax48O(f, 0.0f), 1.0f);
}
}

bool initializeEngineAnimationFilters49Q(float left, float right,
                                         EngineAnimationState49Q& state) noexcept {
    if (!std::isfinite(left) || !std::isfinite(right)) return false;
    state.leftB00 = {left, value(0x3e4ccccdu), 1.0f};
    state.rightB08 = {right, value(0x3e4ccccdu), 1.0f};
    state.leftB04 = left;
    state.rightB0C = right;
    return true;
}

bool advanceEngineAnimation49Q(const EngineAnimationInput49Q& input,
    const EngineAnimationModels49Q& models, const SceneGraph49P& graph,
    EngineAnimationState49Q& state, EngineAnimationWorkspace49Q& work,
    EngineAnimationDiagnostic49Q& diagnostic) noexcept {
    diagnostic = {};
    if (input.phase != EngineAnimationPhase49Q::baseAnimation21A000 ||
        !input.engineMidRecordsAbsent)
        return reject(diagnostic, EngineAnimationError49Q::context);
    if (std::fegetround() != FE_TONEAREST)
        return reject(diagnostic, EngineAnimationError49Q::rounding_mode);
    if (!validateSceneGraph49P(graph))
        return reject(diagnostic, EngineAnimationError49Q::graph);
    if (!records(input, models, graph, state, work))
        return reject(diagnostic, EngineAnimationError49Q::records);
    auto staged = state;
    //21A010..21A02C: a signed-byte nonzero test; cold entry writes literal1.
    if (staged.enableGuard8784 == 0) {
        staged.enable4748 = 1;
        staged.enableGuard8784 = 1;
    }
    if (staged.enable4748 == 0) { state = staged; return true; }
    if (!std::isfinite(input.animationDelta23C) || input.animationDelta23C < 0)
        return reject(diagnostic, EngineAnimationError49Q::input);
    // Separate source filters and mirror writes at21A040..21A0D4.
    if (!filter(staged.leftB00, input.aeroE4, input.animationDelta23C, staged.leftB04) ||
        !filter(staged.rightB08, input.aeroE8, input.animationDelta23C, staged.rightB0C))
        return reject(diagnostic, EngineAnimationError49Q::arithmetic);
    const bool chariotClamp = input.chariotModelPresent &&
        (input.chariotFlags191 & 1u) != 0 && input.brakeLeftMotionPresent &&
        input.brakeRightMotionPresent;
    if (chariotClamp) {
        //21A10C..21A180. The two subsequent Chariot motion calls have no
        // engine-subtree effect under the selected distinct-model contract.
        staged.leftB04 = clamp(staged.leftB04);
        staged.rightB0C = clamp(staged.rightB0C);
    }
    const EngineMotionModel49Q* model[2] = {&models.left, &models.right};
    for (unsigned i = 0; i < 2; ++i) {
        if (model[i]->modelRoot != Index::none && !root(graph, model[i]->modelRoot))
            return reject(diagnostic, EngineAnimationError49Q::binding, i);
    }
    if (models.left.modelRoot != Index::none && models.left.modelRoot == models.right.modelRoot)
        return reject(diagnostic, EngineAnimationError49Q::binding);
    if (input.chariotModelPresent &&
        (!root(graph, models.chariotModelRoot) ||
         models.chariotModelRoot == models.left.modelRoot ||
         models.chariotModelRoot == models.right.modelRoot))
        return reject(diagnostic, EngineAnimationError49Q::binding);
    for (unsigned i = 0; i < 2; ++i)
        if (active(*model[i]) && !modelBinding(graph, *model[i]))
            return reject(diagnostic, EngineAnimationError49Q::binding, i);
    if (graph.count != 0)
        std::memcpy(work.nodes, graph.nodes, graph.count * sizeof(SceneNode49P));
    const SceneGraph49P stagedGraph{work.nodes, graph.count};
    float* mirror[2] = {&staged.leftB04, &staged.rightB0C};
    ModelMotionState49Q* motionState[2] = {&staged.leftModel, &staged.rightModel};
    for (unsigned i = 0; i < 2; ++i) {
        if (!active(*model[i])) continue;
        //Left2A0/1BC/1A8 at21A220..21A29C precedes right29C/1B8/1A4.
        *mirror[i] = clamp(*mirror[i]);
        if (!sampleModelMotion49Q(*model[i]->motion, *model[i]->bindings, *mirror[i],
                                 stagedGraph, *motionState[i], work.motion, diagnostic.motion))
            return reject(diagnostic, EngineAnimationError49Q::motion, i);
    }
    if (graph.count != 0)
        std::memcpy(graph.nodes, work.nodes, graph.count * sizeof(SceneNode49P));
    state = staged;
    return true;
}

bool engineAnimationInputFromBindings49Q(const ControlModelBindings48W& bindings,
    const EngineAnimationInput49Q& context, EngineAnimationInput49Q& out) noexcept {
    if (!std::isfinite(bindings.aeroDrive[0]) || !std::isfinite(bindings.aeroDrive[1])) return false;
    auto result = context;
    result.aeroE4 = bindings.aeroDrive[0];
    result.aeroE8 = bindings.aeroDrive[1];
    out = result;
    return true;
}
}

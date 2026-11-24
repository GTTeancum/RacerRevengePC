#pragma once
#include "../assets/model_geometry49o.h"
#include "../assets/model_bindings49o.h"
#include "scene_graph49p.h"
#include <cstdint>

namespace rr::game {
struct ModelMotionState49T { std::uint32_t model18; };
// Source writable lazy records, shared across all models and motions.
// Any nonzero guard byte is warm; warm value words are opaque and store-only.
struct ModelMotionSharedState49T {
    std::uint32_t interpolationValue4332F0;
    std::uint32_t pairValue433478;
    std::uint8_t interpolationGuard3D5D8C;
    std::uint8_t pairGuard3D5DC4;
    //154480 shared entry sentinel, independent of interpolation/pair guards.
    std::uint32_t updateValue433468 = 0;
    std::uint8_t updateGuard3D5DBC = 0;
};
struct ModelMotionWorkspace49T { SceneNode49P nodes[maxSceneNodes49P]; };
enum class ModelMotionError49T : std::uint32_t {
    none, rounding_mode, input, topology, binding, sample_time, arithmetic
};
struct ModelMotionDiagnostic49T {
    ModelMotionError49T error;
    std::uint32_t track;
    std::uint32_t channel; // 0=rotation, 1=position.
};
// Original1559F0/155E70 nonmirrored, unmasked routes. All rotations precede
// positions, every track immediately executes source-equivalent scene setters,
// and repeated target rows remain ordered. Basis setters clear their fourth
// lanes and preserve translation; position setters change XYZ only. Cached
// worlds, topology and unrelated flags survive; dirty/ready mixtures are not
// normalized. Every successful call clears model18. Lazy records initialize
// only when their original helper entry is reached, including interpolation
// at fraction0; final rotation keys skip the interpolation helper.
//
// Views and bindings must be successfully admitted, alive and unchanged49O
// records. Further admission: coherent forest1..maxSceneNodes49P, finite local/world lanes,
// bindings1..64, >=2 keys per consumed track, covered sample times, finite
// consumed values/intermediates, fraction/weight in[0,1], FE_TONEAREST.
// Pair counts/row sequences must match. BOTH motions use first.duration*fraction;
// weight0 selects the second resource through original arithmetic, without an
// endpoint shortcut. Pair does not consume second.duration.
// Retains49O original software trig tables and cold acos padding; separate
// nearest/even binary32 operations. ShortAxis trig uses explicitly documented
// native double sin/cos table construction; ByteAxis tables remain original.
// No IK, mirror/mask, allocation,
// I/O, world refresh, playback clock or ownership service.
// Graph, mutable state, shared state, diagnostic, scratch and immutable inputs
// must be disjoint; serial access. Fixed-record scratch overlap is rejected.
// False leaves graph/model/shared unchanged. Scratch is unspecified afterward;
// diagnostic is always assigned. Shared state must persist in original call order.
bool sampleModelMotion49T(const assets::MotionView49O& motion,
    const assets::MotionNodeBindings49O& bindings, float frameFraction,
    const SceneGraph49P& graph, ModelMotionState49T& state,
    ModelMotionSharedState49T& shared, ModelMotionWorkspace49T& workspace,
    ModelMotionDiagnostic49T& diagnostic) noexcept;
// Source156500 allocates one32-bit mask lane per source binding row.
// Any nonzero lane suppresses that row in1559F0 before sampling/setters.
// This is a numeric view, not proof of a widget/model lifetime or node search.
struct ModelMotionMask50KG {
    std::uint32_t count=0;
    std::uint32_t rows[64]{};
};
// Same nonmirrored1559F0 contract and bounds as above, with the real source
// row mask. The caller retains its independently resolved mask, graph and
// bindings. Masked tracks do not initialize interpolation state or setters.
// No ancestor propagation, static-pose substitute or playback-clock update.
bool sampleModelMotionMasked50KG(const assets::MotionView49O&,
    const assets::MotionNodeBindings49O&,const ModelMotionMask50KG&,float frameFraction,
    const SceneGraph49P&,ModelMotionState49T&,ModelMotionSharedState49T&,
    ModelMotionWorkspace49T&,ModelMotionDiagnostic49T&) noexcept;
// Original154480's nonmirrored, unmasked direct-time sampling route with
// no cross-motion blend or scale-restoration owners. Consumes sampleTime as
// current34, with no duration/fraction roundtrip. Rotation145850 clamps at the
// final key; Bezier144EB0 extrapolates beyond the last key using the last two
// positions. This is needed on the nonlooping completion frame before154A00
// stops playback. Existing fractional APIs retain their covered-time admission.
// No playback clock, model-pointer or owner state is changed by this helper.
// Same49T graph/binding/shared/transaction contracts apply; time is finite>=0.
bool sampleModelMotionTime50IG(const assets::MotionView49O&,
    const assets::MotionNodeBindings49O&,float sampleTime,const SceneGraph49P&,
    ModelMotionSharedState49T&,ModelMotionWorkspace49T&,ModelMotionDiagnostic49T&,bool independentChannels=false) noexcept;
bool sampleModelMotionBlend49T(const assets::MotionView49O& first,
    const assets::MotionView49O& second,
    const assets::MotionNodeBindings49O& bindings, float frameFraction,
    float blendWeight, const SceneGraph49P& graph, ModelMotionState49T& state,
    ModelMotionSharedState49T& shared, ModelMotionWorkspace49T& workspace,
    ModelMotionDiagnostic49T& diagnostic) noexcept;
}

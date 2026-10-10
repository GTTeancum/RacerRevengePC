#pragma once
#include "chariot_ik49t.h"

namespace rr::game {
// The two28FFD0 lazy words are distinct from the mode1 words443450/443458.
// Only debug40=0 is admitted; profiling lookup/counters are outside this owner.
struct ModelMode0IkShared50KG {
    VisualLazyWord49R sentinel443460{}, sentinel443468{};
};
// Immutable source-initialized records, in156620 endpoint registration order.
// Records and descriptor remain alive/disjoint from the graph during callback.
struct ModelMode0IkContext50KG {
    const ChariotIkChain49T* chains = nullptr;
    std::uint32_t count = 0;
};
// Source1568C8..156920: derive endpoint's parent/grandparent, run the FULL
// constructor28EB90 with source433400=(1,0,0,1), then overwrite mode10=0.
// Call at the exact registration boundary: for Rider, entry has already been
// sampled at .001 and captured. Do not substitute raw PSG bind pose or a later
// transition pose. All constructor coefficients and libm/cache effects remain.
// The caller owns registration order and whole-model setup rollback.
// False leaves graph, record and libm unchanged; workspace may change.
bool initializeModelMode0Ik50KG(SceneNodeIndex49P endpoint,
    const SceneGraph49P&, ChariotIkChain49T&, SourceLibmState49T&,
    ChariotIkWorkspace49T&, ChariotIkDiagnostic49T&) noexcept;
// Source28FFD0, reached through28F580 when mode10=0. Target is a world point.
// Rotates existing elbow/shoulder locals; preserves their translations; does
// not replace elbow with rest pose and does not assign endpoint orientation.
// Only14/18 and node identities/mode/debug are consumed from the record.
// False leaves graph and both lazy words unchanged; workspace may change.
bool solveModelMode0Ik50KG(const ChariotIkChain49T&, const Vector3_48C& worldTarget,
    ModelMode0IkShared50KG&, const SceneGraph49P&,
    ChariotIkWorkspace49T&, ChariotIkDiagnostic49T&) noexcept;
// Signature for ModelPlaybackIkSolve50KG. Uses bounded automatic scratch and
// commits only to the graph/lazy records supplied by playback's transaction.
// The context cannot hide mutable global state or an external graph pointer.
bool modelMode0IkCallback50KG(const void* context, std::uint32_t chain,
    const Vector3_48C& worldTarget, ModelMode0IkShared50KG& stagedShared,
    const SceneGraph49P& stagedGraph) noexcept;
// All calls require finite local/world matrices throughout the graph, valid
// topology, disjoint records/scratch/descriptors, strict binary32 operations,
// FE_TONEAREST and serial access.
// Existing49T scalar EE roots and VU zero-or-normal reciprocal-root boundary
// apply. Full constructor requires finite nonzero length/projected products.
// Solve admits the original zero/collinear vector paths: no invented fallback
// axis or identity result is substituted. Nonfinite consumed arithmetic and
// subnormal nonzero VU norms fail; debug/profiling mode is unsupported.
}

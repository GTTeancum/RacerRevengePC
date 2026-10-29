#pragma once
#include "vehicle_visual49r.h"

namespace rr::game::detail {
// Internal leaves for the admitted21A660 transaction, not public atomic graph
// operations. False can leave transaction scratch caches/outputs changed.
// Caller provides serial, nonaliased descriptors/output/storage and strict FP
// (FE_TONEAREST, no contraction/fast math). No allocation or scene ownership.
// All consumed arithmetic must be finite. Copy-only words retain their bits.
// The source context also requires finite surviving VU scratch operands for
// its self-subtraction zeroing, notably VF12.W before28BCE0/28BE80. Finite
// scene/input values alone do not establish that precondition. Arbitrary
// poisoned VU register files are outside this bounded leaf equivalence.
// Graph topology is a coherent49P forest; node/relative must be valid, nonnull
// and distinct. No equal-node or null-relative getter route is admitted here.
//
// 28B9B0/28BBB0 ADD.xyzw increments the priorVF9.W. It is not universally1.
// Lazy dirty nonroot refresh instead writes world100[3] to that scratch lane;
// inverse-position28BD50 writes1. Preserve this explicit source scratch state.
// No runtime finiteness gate is imposed on vf9W:21A660's direction resultW is
// dead to represented state. Its wrapper chooses finite scratch under the
// finite surviving-VU precondition; focused four-lane equality uses finiteW.
struct SceneVectorContext49U { float vf9W; };
bool axes49U(const SceneGraph49P&, SceneNodeIndex49P node,
    SceneNodeIndex49P relative, SceneVectorContext49U&,
    VisualVector49R& forward, VisualVector49R& up) noexcept;
bool position49U(const SceneGraph49P&, SceneNodeIndex49P node,
    SceneNodeIndex49P relative, SceneVectorContext49U&, VisualVector49R&) noexcept;
bool direction49U(const SceneGraph49P&, SceneNodeIndex49P node,
    const VisualVector49R&, SceneVectorContext49U&, VisualVector49R&) noexcept;
bool inverseDirection49U(const SceneGraph49P&, SceneNodeIndex49P node,
    const VisualVector49R&, SceneVectorContext49U&, VisualVector49R&) noexcept;
//Pure numerical direction leaf only: caller authenticates the actual clean
//scene owner and source getter cut. No graph refresh or getter receipt.
bool cleanMatrixDirectionPrefix49U(const BodyMatrix48G&,const VisualVector49R&,
    SceneVectorContext49U&,VisualVector49R&)noexcept;
//Pure28BAB0 point arithmetic for an authenticated clean owner; W becomes1.
bool cleanMatrixPointPrefix49U(const BodyMatrix48G&,const VisualVector49R&,
    SceneVectorContext49U&,VisualVector49R&)noexcept;
//Pure28BD50 arithmetic after actual owner refresh; output pointW becomes1.
bool cleanMatrixInversePointPrefix49U(const BodyMatrix48G&,const VisualVector49R&,
    SceneVectorContext49U&,VisualVector49R&)noexcept;
//Pure numerical inverse-direction counterpart; same caller-owned clean cut.
bool cleanMatrixInverseDirectionPrefix49U(const BodyMatrix48G&,const VisualVector49R&,
    SceneVectorContext49U&,VisualVector49R&)noexcept;
// 28B550 uses relative.parent's world, never relative.world. A parentless
// relative causes a full16-word copy of node.world, with no arithmetic.
bool relativeMatrix49U(const SceneGraph49P&, SceneNodeIndex49P node,
    SceneNodeIndex49P relative, SceneVectorContext49U&, BodyMatrix48G&) noexcept;
// Complete28B3E0: snapshot/copy all16 LOCAL words, then source-ordered49P
// ancestor marking/invalidation. Does not obtain or assign a world matrix.
bool copyLocal49U(const SceneGraph49P&, SceneNodeIndex49P,
    const BodyMatrix48G&) noexcept;
// 142B80+13BFE0: original polynomial and scalar/VU operation order, with no
// normalization or periodic range reduction. Only outputXYZ is written;
// outputW is preserved, including a copy-only nonfinite payload.
bool rotateVector49U(const VisualVector49R& axis, const VisualVector49R& input,
    float angle, VisualVector49R& output) noexcept;
// 14F950: original largest-component tie policy; no normalization.
bool matrixQuaternion49U(const BodyMatrix48G&, Quaternion48C&) noexcept;
}

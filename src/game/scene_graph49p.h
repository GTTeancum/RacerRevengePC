#pragma once
#include "body_scene48g.h"
#include <cstdint>

namespace rr::game {
// Largest retained roster forest: Pod08 has 61 PSG nodes plus seven owners.
// This is host storage capacity; individual PSG/MOT palettes remain capped at64.
constexpr std::uint32_t maxSceneNodes49P = 80;
// Native record indices, never original guest pointers. No ownership is implied.
enum class SceneNodeIndex49P : std::int32_t { none = -1 };
struct SceneNode49P {
    BodyMatrix48G localC0, world100;
    SceneNodeIndex49P parent180 = SceneNodeIndex49P::none;
    SceneNodeIndex49P firstChild184 = SceneNodeIndex49P::none;
    SceneNodeIndex49P nextSibling188 = SceneNodeIndex49P::none;
    std::uint8_t sceneFlags190;
};
struct SceneGraph49P { SceneNode49P* nodes; std::uint32_t count; };
struct SceneDisplacement49P { float v[4]; };

// Read-only admission of a caller-initialized, coherent acyclic forest, <=maxSceneNodes49P
// nodes. Roots have no siblings; each nonroot appears exactly once in its
// parent's first-child/next-sibling list. Node order need not be parent-first.
// Empty forests are valid but cannot be the target of an operation. No cache
// coherence is inferred: a dirty node may have ready descendants, and a ready
// node's cached world may differ from the supplied local/parent matrices.
bool validateSceneGraph49P(const SceneGraph49P& graph) noexcept;

// Independent bounded source operations, not a body wrapper or frame scheduler.
// All operations require admitted topology, a valid target, FE_TONEAREST and
// serial access. Matrices keep all 16 source words and flags retain unrelated
// bits. Copy-only paths do not reinterpret or canonicalize floating-point bits.
// Only arithmetic operands/intermediates must be finite, with separate binary32
// multiply/add operations (strict FP, no fast-math/contraction). False leaves
// all represented graph state unchanged. No allocation, callback, bounds
// recomputation, observer28EA20, animation, spawn, or scene ownership is supplied.
//
// 289B20 with relative=null AND target.parent=null. Replaces the entire local,
// including translation and fourth lanes; world remains cached until requested.
bool setParentlessWorldMatrix49P(const SceneGraph49P& graph,
    SceneNodeIndex49P node, const BodyMatrix48G& matrix) noexcept;
// 28AF90 with relative=null AND target.parent=null. Adds displacement XYZ to
// local translation in source order. Its W is loaded but not used arithmetically;
// localFC and all 12 basis/fourth lanes are preserved.
bool addParentlessTranslation49P(const SceneGraph49P& graph,
    SceneNodeIndex49P node, const SceneDisplacement49P& displacement) noexcept;
// 28B490 copies local, then marks bit80 on this node/ancestors. All three
// setters clear this node's ready01 and invalidate descendants ONLY if it was
// ready. An already-dirty setter does not repair ready descendants.
bool setLocalMatrix49P(const SceneGraph49P& graph,
    SceneNodeIndex49P node, const BodyMatrix48G& matrix) noexcept;
// 28C4F0: sets bit80 from the requested node upward, preserving every other bit.
bool markSceneAncestors49P(const SceneGraph49P& graph,
    SceneNodeIndex49P node) noexcept;
// 289A20: processes next siblings before the requested node; only ready nodes
// are cleared and descended into. Already-dirty nodes skip their descendants.
bool invalidateSceneReady49P(const SceneGraph49P& graph,
    SceneNodeIndex49P node) noexcept;
// 2897B0/289940: ready returns untouched. Dirty root copies local to world;
// dirty child lazily refreshes its parent and computes world=local*parentWorld
// in the exact VU multiply/add order. Only the required ancestor chain changes;
// descendants/siblings are not eagerly refreshed. Result lives in world100.
bool refreshSceneWorld49P(const SceneGraph49P& graph,
    SceneNodeIndex49P node) noexcept;
}

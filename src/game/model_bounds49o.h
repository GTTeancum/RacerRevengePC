#pragma once
#include "body_scene48g.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
constexpr std::size_t maxModelBoundsNodes49O=64;
struct ModelBoundsNode49O {
    BodyMatrix48G local;
    Vector3_48C center140, halfExtent150;
    std::int32_t parent; // -1 means graphics model root, otherwise earlier node.
    std::uint8_t sceneFlags190; // Original bit04 alone selects box contribution.
};
struct ModelBoundsContext49O {
    BodyMatrix48G rootWorld100; // Explicit source-ready root world, not its local.
    Vector3_48C rootCenter140, rootHalfExtent150;
    std::uint8_t rootSceneFlags190; // Bit10 sphere route is outside this API.
};
struct ModelBoundsWorkspace49O { BodyMatrix48G world[maxModelBoundsNodes49O]; };
struct ModelBoundsResult49O {
    float minimum[4], maximum[4], center160[4], halfExtent170[4], radius198;
    std::uint8_t rootSceneFlags190;
    bool seeded;
};
// Numeric box route26F4B0->28DAF0, including original local*parent world order,
// world point/directions, root-basis transpose conversion, filtered flat union
// and source SQRT radius. Stored sphere194 is deliberately absent. This is not
// an animation/PSG loader or a cache-root radius substitution.
//
// All node worlds are regenerated from locals; node flag01 does not admit stale
// independent cached world100 values. Only node flag04 is consumed. This is a
// deliberate current-transform boundary, not a generic live scene-cache API.
// Fresh-source callers must supply the actual post-motion local matrices and
// proven scene flags in original flat source order. The unparented fresh Pod01
// root orientation is identity by213800/28A090; other ready root inputs remain
// explicit. The root transpose is original behavior, NOT a general inverse.
//
// 0..64 parent-first affine finite matrices; eligible centers/half extents finite,
// eligible half extents nonnegative. Every workspace world is eagerly computed,
// including filtered nodes; any overflow/nonfinite intermediate rejects.
// Requires FE_TONEAREST and separate binary32 operations (no fast-math/FMA).
// Allocation-free. Output unchanged on failure; workspace is caller scratch and
// may change. Nodes, context, workspace and output must be disjoint storage.
bool computeModelBounds49O(const ModelBoundsNode49O* nodes, std::size_t count,
    const ModelBoundsContext49O& context, ModelBoundsWorkspace49O& workspace,
    ModelBoundsResult49O& output) noexcept;
}

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

// Ordinary static A2=0 load: source row0 IS the model root, not a child.
// Caller supplies that actual alias in original1B8 order, with1..64 rows;
// row0.parent=-1 and every later parent is an earlier source row. Row0's
// flags/box must be the SAME root values in context, bit-for-bit. Root own04
// must be set; sphere10 remains unsupported. Locals are finite affine source
// inputs, but row0.local is not multiplied: its actual ready rootWorld100 is
// copied into workspace.world[0]. This prevents applying placement twice.
//
// Preserve26F4B0's own-root box seed, THEN28DAF0 projection of every eligible
// source row, including row0 through that SAME root world/basis. Do not omit
// its repeated projection or replace the root-basis transpose with an inverse.
// Remaining numeric/disjoint-storage/output-on-failure rules above apply.
// This numeric API does not establish model identity, graph readiness, source
// resource acquisition, bounds publication, or observer membership ownership.
bool computeDirectRootModelBounds50KG(const ModelBoundsNode49O* sourceRows,
    std::size_t count, const ModelBoundsContext49O& context,
    ModelBoundsWorkspace49O& workspace, ModelBoundsResult49O& output) noexcept;
//Pure numeric stages of28D670/28DAF0. These consume actual current world
//caches; no regeneration, radius, flags or scene publication is performed.
//FE_TONEAREST and finite operands/intermediates are an explicit native bound.
struct SourceLocalBoundsState49O {
    float minimum[4]{0,0,0,1},maximum[4]{0,0,0,1};
    bool seeded=false;
};
bool seedLocalBounds28D670(const Vector3_48C&,const Vector3_48C&,bool eligible,
    SourceLocalBoundsState49O&)noexcept;
struct SourceLocalBoundsProjection49O {Vector3_48C centerWorld{},axesWorld[3]{};};
bool captureLocalBounds28DAF0(const BodyMatrix48G& currentWorld,const Vector3_48C& center,
    const Vector3_48C& half,unsigned getter,SourceLocalBoundsProjection49O&)noexcept;
bool mergeLocalBounds28DAF0(const SourceLocalBoundsProjection49O&,const BodyMatrix48G& rootWorld,
    SourceLocalBoundsState49O&)noexcept;
bool finishLocalBounds28D670(const SourceLocalBoundsState49O&,bool half,float output[4])noexcept;

}

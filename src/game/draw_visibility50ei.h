#pragma once
#include "../assets/native_asset48b.h"
#include <cstdint>

namespace rr::game {
enum class DrawClipDepth50EI : std::uint8_t {
    unspecified, negativeOneToOne, zeroToOne
};
enum class DrawUncertainPolicy50EI : std::uint8_t { retain, reject };

struct StaticDrawBounds50EI {
    // Opaque caller draw identifier. Array order is submission order; duplicate
    // identifiers remain duplicates. Bounds enclose actual world-space visuals.
    std::uint32_t drawId = 0;
    assets::Bounds48B worldBounds{};
};
struct DrawVisibilityInput50EI {
    const StaticDrawBounds50EI* draws = nullptr;
    std::uint32_t drawCount = 0;
    // Sixteen finite floats, ROW-VECTOR matrix in COLUMN-MAJOR storage:
    // clip[column] = sum(world[row] * matrix[column * 4 + row]), world.w = 1.
    // Use the exact complete matrix supplied to GL_FALSE uniform upload, with
    // no second model transform, transpose or depth conversion here.
    const float* worldViewProjection = nullptr;
    DrawClipDepth50EI clipDepth = DrawClipDepth50EI::unspecified;
    DrawUncertainPolicy50EI uncertainPolicy = DrawUncertainPolicy50EI::retain;
};
struct DrawVisibilityBuffer50EI {
    std::uint32_t* drawIds = nullptr;
    std::uint32_t capacity = 0;
};
struct DrawVisibilityResult50EI {
    std::uint32_t retained = 0, culled = 0, uncertainRetained = 0;
};
enum class DrawVisibilityError50EI : std::uint8_t {
    none, storage, clipDepth, uncertainPolicy, unsupportedArithmetic,
    nonfiniteMatrix, invalidBounds, uncertainTransform, capacity
};
struct DrawVisibilityDiagnostic50EI {
    DrawVisibilityError50EI error = DrawVisibilityError50EI::none;
    // UINT32_MAX for a call-wide error; otherwise an input draw ordinal, not ID.
    std::uint32_t drawOrdinal = UINT32_MAX;
};

// Optional HOST VISUAL optimization, not recovered original visibility policy.
// Cull only when all eight AABB corners are provably outside a common standard
// homogeneous clip plane. Touching, intersecting and numerically ambiguous
// bounds stay. No perspective divide, occlusion, sorting, LOD or collision use.
// GL depth clamp/user clipping and shader displacement must not invalidate the
// caller's six-plane clip contract or the supplied world-space bounds.
//
// Bounds must have finite endpoints and minimum <= maximum on all three axes.
// Matrix/depth/policy are validated even for an empty list. Null draws is legal
// only at drawCount zero; null drawIds only at capacity zero. Capacity is an
// element count, not bytes; it need only fit the retained IDs. Zero-volume boxes
// are valid. Nonfinite input data is an error, never an implicit visible list.
//
// Arithmetic uses strict IEEE binary32/binary64 with nearest-even host rounding,
// no fast math, and conservative binary32 transform error margins. Subnormal
// inputs or a transform that cannot be bounded safely in binary32 are uncertain:
// retain includes that whole draw, reject returns uncertainTransform. Boundary
// overlap alone is ordinary retention, not an arithmetic error.
//
// Single source-order pass, O(drawCount), constant stack, no allocation/callbacks.
// Input arrays, matrix, output array, result and diagnostic must be live, stable
// and mutually disjoint for the call. Nothing is retained or modified in inputs.
// Success writes exactly result.retained IDs; the remaining buffer is untouched.
// Failure preserves result, but may overwrite an output prefix: use a staging
// buffer and publish it ONLY on true. Diagnostic always describes this call.
// There is no successful partial list or automatic fallback. No physics, world
// registration, simulation scheduling or authoritative draw state is changed.
bool collectVisibleStaticDraws50EI(const DrawVisibilityInput50EI&,
    const DrawVisibilityBuffer50EI&, DrawVisibilityResult50EI&,
    DrawVisibilityDiagnostic50EI&) noexcept;
}

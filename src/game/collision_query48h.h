#pragma once
#include "body_integration48c.h"

namespace rr::game {
// Original 10B840 call boundary: start + (direction * fraction) * length.
// The terrain caller supplies a normalized direction and positive length.
struct DirectedSegment48H {
    Vector3_48C start;
    Vector3_48C direction;
    float length;
};
struct Triangle48H {
    Vector3_48C vertices[3];
    Vector3_48C vertexNormals[3];
    bool hasVertexNormals;
};
struct TriangleHit48H {
    Vector3_48C point;
    Vector3_48C normal;
    float fraction;
};
enum class TriangleQueryStatus48H {
    Miss, Hit, InvalidInput, UnsupportedInterpolationBoundary
};
// Typed recovery of original mode1, unquantized vertex branch of 10B840.
// Preserves one-sided plane test, inclusive segment endpoints, original
// projected scanline tests and mode1 vertex-normal interpolation order.
// No tree, material/category filter, candidate ordering or penetration output.
// Transactional: only Hit changes output. See docs/terrain-query48h.md for the
// original interpolation's uninitialized-edge boundary and tested FP domain.
TriangleQueryStatus48H intersectTriangle48H(const DirectedSegment48H& segment,
    const Triangle48H& triangle, TriangleHit48H& output) noexcept;
}

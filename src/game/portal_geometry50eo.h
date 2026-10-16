#pragma once
#include "body_integration48c.h"
#include "body_scene48g.h"
#include <cstdint>

namespace rr::game {
// Prepared from the four authored vertices in their original order. The source
// portal stores these at +30..+60, the normal at +70, and edge normals at +80.
// Keep this value unchanged between preparation and the queries that use it.
struct PortalGeometry50EO {
    Vector3_48C vertices[4]{};
    Vector3_48C normal{};
    Vector3_48C edgeNormals[4]{};
};
enum class PortalGeometryStatus50EO : std::uint8_t {
    Complete, InvalidInput, DegenerateGeometry
};

// Original230690: normalize((v1-v0) cross (v2-v0)), followed by four
// normalize((v[next]-v[i]) cross normal). Does not reorder vertices, infer
// zone direction, fit a plane, or replace the authored shape with an AABB.
// Zero-length normal/edge normalization is an explicit native domain boundary.
PortalGeometryStatus50EO preparePortalGeometry50EO(
    const Vector3_48C (&vertices)[4], PortalGeometry50EO& output) noexcept;

// Original230BD0, with its unused optional intersection-point output omitted.
// Preserves the signed raw-word XOR admission, [0,1] segment fraction, and
// raw sign-bit edge predicates (including their distinction between +/-0).
// The source's coplanar/parallel zero-denominator case is a completed miss.
PortalGeometryStatus50EO crossPortal230BD0_50EO(
    const PortalGeometry50EO& portal, const Vector3_48C& oldCenter,
    const Vector3_48C& newCenter, bool& crossed) noexcept;

// Original231150: inclusive plane slab and four inclusive edge half-spaces.
// This is the source's expanded-half-space test, not closest-point distance
// to a quad. Absolute plane distance is returned even on a completed miss.
PortalGeometryStatus50EO overlapPortal231150_50EO(
    const PortalGeometry50EO& portal, const Vector3_48C& center, float radius,
    bool& overlaps, float& absolutePlaneDistance) noexcept;

struct PortalProjectionQuad50EO { float xy[4][2]{}; };
//230450..230554 literal quad construction/copy/reorder. This is the shared
// source434940 input, not a host-window rectangle. Failure leaves output alone.
bool constructPortalViewport230450_50EO(PortalProjectionQuad50EO& output) noexcept;
enum class PortalProjectionStatus50EO : std::uint8_t {
    Hit, Miss, InvalidInput, ArithmeticFailure, CapacityExceeded
};
// Bounded native230880 leaf only; no zone traversal, visibility/LOD or owner
// publication. Borrow the actual prepared portal and current source camera
// +180 position, +1FC near and +100 combined, plus the actual434940 quad.
// Never substitute a host raster rectangle or guard/screen transform.
// Forced input W=1; classification uses |W|, intersections use signed W.
// Retains six-plane order, point order and reached binary32 arithmetic.
// Only Hit changes output. Miss means completed source rejection; numeric or
// capacity failures are NOT culling decisions. Local banks hold16 vertices;
// final clipped count must fit the original10-element230880 local buffer.
// Inputs are immutable during the call; output may alias the bypass quad but
// otherwise must be disjoint from the inputs.
// Requires an actually prepared nondegenerate portal, finite consumed values,
// FE_TONEAREST and strict binary32 operations. No exceptional VU parity claim.
// Near bypass consumes no combined matrix; other paths consume no bypass quad.
PortalProjectionStatus50EO projectPortal230880_50EO(
    const PortalGeometry50EO& portal, const Vector3_48C& cameraPosition,
    float cameraNear, const BodyMatrix48G& cameraCombined,
    const PortalProjectionQuad50EO& borrowedQuad434940,
    PortalProjectionQuad50EO& output) noexcept;

// Original263260 bounded viewport intersection. Preserves containment order,
// duplicate candidates, endpoint hits, four-point reorder and envelope fallback.
// All input coordinates must be finite; strict nearest binary32 required.
// Hit alone publishes output; Miss is completed-empty, safety failures are not
// visibility decisions. Exact output aliasing with either input is supported.
// No render-zone owner/stamp publication or gameplay culling is enabled here.
PortalProjectionStatus50EO intersectPortalViewport263260_50EO(
    const PortalProjectionQuad50EO& a, const PortalProjectionQuad50EO& b,
    PortalProjectionQuad50EO& output) noexcept;

// The three geometry functions above are bounded and allocation-free. Non-Complete leaves every
// output unchanged. Inputs and consumed arithmetic must be finite; radius
// must be nonnegative and a prepared portal must be nondegenerate. Preparation
// may alias output.vertices. Other outputs must not overlap one another.
// Requires round-to-nearest and strict binary32 operations (no fast math/FMA).
// VU normalization uses the retained native48G/49F rounded-sqrt then divide
// contract; this does not claim all exceptional PS2/VU floating-point modes.
}

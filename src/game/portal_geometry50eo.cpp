#include "portal_geometry50eo.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
// Authenticated directly against file-backed SLUS_202.68, SHA-256
// c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30:
// 230690..230874 preparation, 230BD0..231150 crossing,
// 231150..2312FC overlap, and 106E70..106E98 dot (exclusive ends).
using V = Vector3_48C;
using Status = PortalGeometryStatus50EO;
std::uint32_t bits(float value) noexcept {
    std::uint32_t result;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}
bool finite(const V& value) noexcept {
    for (float component : value.v)
        if (!std::isfinite(component)) return false;
    return true;
}
bool zero(const V& value) noexcept {
    return value.v[0] == 0.0f && value.v[1] == 0.0f && value.v[2] == 0.0f;
}
struct Arithmetic {
    bool valid = true;
    float keep(float value) noexcept {
        valid = valid && std::isfinite(value);
        return value;
    }
    float add(float a, float b) noexcept {
        volatile float result = a + b;
        return keep(result);
    }
    float sub(float a, float b) noexcept {
        volatile float result = a - b;
        return keep(result);
    }
    float mul(float a, float b) noexcept {
        volatile float result = a * b;
        return keep(result);
    }
    float div(float a, float b) noexcept {
        volatile float result = a / b;
        return keep(result);
    }
    V sub(const V& a, const V& b) noexcept {
        V result{};
        for (unsigned i = 0; i != 3; ++i) result.v[i] = sub(a.v[i], b.v[i]);
        return result;
    }
    float dot(const V& a, const V& b) noexcept {
        // VMUL.xyz; VADDw.x(0,0)=1; VADDAy.x; VMADDz.x.
        const float x = mul(a.v[0], b.v[0]);
        const float y = mul(a.v[1], b.v[1]);
        const float z = mul(a.v[2], b.v[2]);
        const float one = add(0.0f, 1.0f);
        const float xy = add(x, y);
        return add(xy, mul(one, z));
    }
    V cross(const V& a, const V& b) noexcept {
        // VOPMULA.xyz ACC,a,b; VOPMSUB.xyz result,b,a.
        const float x = mul(a.v[1], b.v[2]);
        const float y = mul(a.v[2], b.v[0]);
        const float z = mul(a.v[0], b.v[1]);
        V result{};
        result.v[0] = sub(x, mul(b.v[1], a.v[2]));
        result.v[1] = sub(y, mul(b.v[2], a.v[0]));
        result.v[2] = sub(z, mul(b.v[0], a.v[1]));
        return result;
    }
    Status normalize(const V& value, V& result) noexcept {
        const float squared = dot(value, value);
        if (!valid) return Status::InvalidInput;
        if (squared == 0.0f) return Status::DegenerateGeometry;
        // Original230748/230848 VRSQRT, in the retained48G/49F finite
        // native arithmetic model: round sqrt before the reciprocal.
        volatile float root = std::sqrt(squared);
        const float scale = div(1.0f, keep(root));
        for (unsigned i = 0; i != 3; ++i) result.v[i] = mul(value.v[i], scale);
        return valid ? Status::Complete : Status::InvalidInput;
    }
};
Status admit(const PortalGeometry50EO& portal) noexcept {
    if (std::fegetround() != FE_TONEAREST || !finite(portal.normal))
        return Status::InvalidInput;
    for (unsigned i = 0; i != 4; ++i)
        if (!finite(portal.vertices[i]) || !finite(portal.edgeNormals[i]))
            return Status::InvalidInput;
    if (zero(portal.normal)) return Status::DegenerateGeometry;
    for (const V& edge : portal.edgeNormals)
        if (zero(edge)) return Status::DegenerateGeometry;
    return Status::Complete;
}
}

PortalGeometryStatus50EO preparePortalGeometry50EO(
    const Vector3_48C (&vertices)[4], PortalGeometry50EO& output) noexcept {
    if (std::fegetround() != FE_TONEAREST) return Status::InvalidInput;
    PortalGeometry50EO prepared{};
    for (unsigned i = 0; i != 4; ++i) {
        if (!finite(vertices[i])) return Status::InvalidInput;
        prepared.vertices[i] = vertices[i];
    }
    Arithmetic f;
    const V first = f.sub(vertices[1], vertices[0]);
    const V diagonal = f.sub(vertices[2], vertices[0]);
    Status status = f.normalize(f.cross(first, diagonal), prepared.normal);
    if (status != Status::Complete) return status;
    for (unsigned i = 0; i != 4; ++i) {
        const V edge = f.sub(vertices[(i + 1) & 3], vertices[i]);
        status = f.normalize(f.cross(edge, prepared.normal), prepared.edgeNormals[i]);
        if (status != Status::Complete) return status;
    }
    output = prepared;
    return Status::Complete;
}

PortalGeometryStatus50EO crossPortal230BD0_50EO(
    const PortalGeometry50EO& portal, const Vector3_48C& oldCenter,
    const Vector3_48C& newCenter, bool& crossed) noexcept {
    const Status status = admit(portal);
    if (status != Status::Complete) return status;
    if (!finite(oldCenter) || !finite(newCenter)) return Status::InvalidInput;
    Arithmetic f;
    const V oldRelative = f.sub(oldCenter, portal.vertices[0]);
    const V newRelative = f.sub(newCenter, portal.vertices[0]);
    const float oldSide = f.dot(portal.normal, oldRelative);
    const float newSide = f.dot(portal.normal, newRelative);
    if (!f.valid) return Status::InvalidInput;
    const std::uint32_t sideXor = bits(oldSide) ^ bits(newSide);
    // 230CE4 BLEZ of the signed raw-word XOR. Equal same-sign distances
    // also pass this gate, unlike a generic opposite-sign predicate.
    if (sideXor != 0 && (sideXor & 0x80000000u) == 0) {
        crossed = false;
        return Status::Complete;
    }
    const V delta = f.sub(newCenter, oldCenter);
    const float planeOffset = -f.dot(portal.vertices[0], portal.normal);
    const float oldProjection = f.dot(portal.normal, oldCenter);
    const float denominator = f.dot(portal.normal, delta);
    const float numerator = -f.add(planeOffset, oldProjection);
    if (!f.valid) return Status::InvalidInput;
    // Scalar DIV.S by zero cannot produce a fraction in [0,1]. This also
    // makes a coplanar/stationary segment a miss without a host FP exception.
    if (denominator == 0.0f) {
        crossed = false;
        return Status::Complete;
    }
    const float fraction = f.div(numerator, denominator);
    if (!f.valid) return Status::InvalidInput;
    // 230DB4 is strict <0; 230DCC is <=1. Both endpoint ties are admitted.
    if (fraction < 0.0f || !(fraction <= 1.0f)) {
        crossed = false;
        return Status::Complete;
    }
    V point{};
    for (unsigned i = 0; i != 3; ++i)
        point.v[i] = f.add(oldCenter.v[i], f.mul(fraction, delta.v[i]));
    if (!f.valid) return Status::InvalidInput;
    for (unsigned i = 0; i != 4; ++i) {
        const V edge = f.sub(portal.vertices[(i + 1) & 3], portal.vertices[i]);
        const V relative = f.sub(point, portal.vertices[i]);
        const float inside = f.dot(portal.normal, f.cross(edge, relative));
        if (!f.valid) return Status::InvalidInput;
        // 230F00/230FB0/231060/231110 BGEZ on the float's raw word.
        if (bits(inside) & 0x80000000u) {
            crossed = false;
            return Status::Complete;
        }
    }
    crossed = true;
    return Status::Complete;
}

PortalGeometryStatus50EO overlapPortal231150_50EO(
    const PortalGeometry50EO& portal, const Vector3_48C& center, float radius,
    bool& overlaps, float& absolutePlaneDistance) noexcept {
    const Status status = admit(portal);
    if (status != Status::Complete) return status;
    if (!finite(center) || !std::isfinite(radius) || radius < 0.0f)
        return Status::InvalidInput;
    Arithmetic f;
    const V firstRelative = f.sub(center, portal.vertices[0]);
    const float signedDistance = f.dot(firstRelative, portal.normal);
    if (!f.valid) return Status::InvalidInput;
    const float distance = std::fabs(signedDistance);
    bool hit = signedDistance <= radius && !(signedDistance < -radius);
    // Source23120C uses the retained first relative vector, then computes
    // center-v[i] for i=1..3. It does not project the center onto the plane.
    for (unsigned i = 0; hit && i != 4; ++i) {
        const V relative = i == 0 ? firstRelative : f.sub(center, portal.vertices[i]);
        const float edgeDistance = f.dot(relative, portal.edgeNormals[i]);
        if (!f.valid) return Status::InvalidInput;
        hit = edgeDistance <= radius;
    }
    overlaps = hit;
    absolutePlaneDistance = distance;
    return Status::Complete;
}
}

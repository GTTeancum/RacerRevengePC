#include "portal_geometry50eo.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Portal geometry requires strict finite binary32 arithmetic"
#endif

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

PortalGeometryStatus50EO portalMaximumDiagonal230580_50EO(
    const PortalGeometry50EO& portal, float& output) noexcept {
    const Status status=admit(portal);
    if(status!=Status::Complete)return status;
    Arithmetic f;
    const V first=f.sub(portal.vertices[2],portal.vertices[0]);
    const V second=f.sub(portal.vertices[3],portal.vertices[1]);
    const float a=f.dot(first,first),b=f.dot(second,second);
    if(!f.valid||a<0.0f||b<0.0f)return Status::InvalidInput;
    //230634/230664 SQRT.S, then230674 MAX.S (46000A28).
    volatile float rootA=std::sqrt(a),rootB=std::sqrt(b);
    const float x=f.keep(rootA),y=f.keep(rootB);
    if(!f.valid)return Status::InvalidInput;
    output=x>y?x:y;return Status::Complete;
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

namespace rr::game {
namespace {
using ProjectionStatus = PortalProjectionStatus50EO;
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
    "Portal projection requires IEEE binary32");
struct ClipPoint { float v[4]{0.f, 0.f, 0.f, 1.f}; };
unsigned portalOutcode(const ClipPoint& p) noexcept {
    const float extent = std::fabs(p.v[3]);
    unsigned code = 0;
    for (unsigned axis = 0; axis != 3; ++axis) {
        if (p.v[axis] > extent) code |= 1u << (axis * 2);
        if (p.v[axis] < -extent) code |= 2u << (axis * 2);
    }
    return code;
}
ProjectionStatus clipPortal(ClipPoint (&points)[10], unsigned& count,
    Arithmetic& f) noexcept {
    ClipPoint banks[2][16]{};
    for (unsigned i = 0; i != count; ++i) banks[0][i] = points[i];
    for (int plane = 5; plane >= 0; --plane) {
        const auto* source = banks[(plane & 1) ^ 1];
        auto* destination = banks[plane & 1];
        unsigned emitted = 0;
        const unsigned axis = static_cast<unsigned>(plane) >> 1;
        const unsigned mask = 1u << plane;
        for (unsigned i = 0; i != count; ++i) {
            const auto& previous = source[i ? i - 1 : count - 1];
            const auto& current = source[i];
            const bool previousOutside = (portalOutcode(previous) & mask) != 0;
            const bool currentOutside = (portalOutcode(current) & mask) != 0;
            if (!previousOutside && !currentOutside) {
                if (emitted == 16) return ProjectionStatus::CapacityExceeded;
                destination[emitted++] = previous;
                continue;
            }
            if (previousOutside && currentOutside) continue;
            // Classification used |W|; these source intersections do NOT.
            const float dPrevious = (plane & 1) ? f.add(previous.v[3], previous.v[axis]) :
                f.sub(previous.v[axis], previous.v[3]);
            const float dCurrent = (plane & 1) ? f.add(current.v[3], current.v[axis]) :
                f.sub(current.v[axis], current.v[3]);
            const float denominator = f.sub(dPrevious, dCurrent);
            if (!f.valid || denominator == 0.f) return ProjectionStatus::ArithmeticFailure;
            const float reciprocal = f.div(1.f, denominator);
            if (!f.valid) return ProjectionStatus::ArithmeticFailure;
            if (previousOutside) {
                const float negativeCurrent = -dCurrent;
                const float t = f.mul(reciprocal, negativeCurrent);
                if (emitted == 16) return ProjectionStatus::CapacityExceeded;
                auto& intersection = destination[emitted++];
                for (unsigned lane = 0; lane != 4; ++lane) {
                    const float delta = f.sub(previous.v[lane], current.v[lane]);
                    const float product = f.mul(t, delta);
                    intersection.v[lane] = f.add(current.v[lane], product);
                }
            } else {
                const float t = f.mul(reciprocal, dPrevious);
                if (emitted > 14) return ProjectionStatus::CapacityExceeded;
                destination[emitted++] = previous;
                auto& intersection = destination[emitted++];
                for (unsigned lane = 0; lane != 4; ++lane) {
                    const float delta = f.sub(current.v[lane], previous.v[lane]);
                    const float product = f.mul(t, delta);
                    intersection.v[lane] = f.add(previous.v[lane], product);
                }
            }
            if (!f.valid) return ProjectionStatus::ArithmeticFailure;
        }
        count = emitted;
        if (!count) return ProjectionStatus::Miss;
    }
    if (count > 10) return ProjectionStatus::CapacityExceeded;
    for (unsigned i = 0; i != count; ++i) points[i] = banks[0][i];
    return ProjectionStatus::Hit;
}
bool reorderPortal(PortalProjectionQuad50EO& quad, Arithmetic& f) noexcept {
    const auto original = quad;
    const auto& p = original.xy;
    //263780..26379C: subtraction/product order is not a generic area helper.
    const float y01 = f.sub(p[0][1], p[1][1]);
    const float y20 = f.sub(p[2][1], p[0][1]);
    const float y12 = f.sub(p[1][1], p[2][1]);
    const float a1 = f.mul(p[1][0], y20);
    const float a0 = f.mul(p[0][0], y12);
    const float aSum = f.add(a0, a1);
    const float aLast = f.mul(p[2][0], y01);
    const float a = f.add(aSum, aLast);
    if (!f.valid) return false;
    unsigned order[4]{0, 1, 2, 3};
    if (a < 0.f) {
        const float y02 = f.sub(p[0][1], p[2][1]);
        const float y30 = f.sub(p[3][1], p[0][1]);
        const float b3 = f.mul(p[3][0], y02);
        const float b2 = f.mul(p[2][0], y30);
        const float bSum = f.add(b2, b3);
        const float y23 = f.sub(p[2][1], p[3][1]);
        const float bLast = f.mul(p[0][0], y23);
        const float b = f.add(bSum, bLast);
        if (!f.valid) return false;
        if (!(b < 0.f)) {
            const float y32 = f.sub(p[3][1], p[2][1]);
            const float c1 = f.mul(p[1][0], y32);
            const float y21 = f.sub(p[2][1], p[1][1]);
            const float c3 = f.mul(p[3][0], y21);
            const float cSum = f.add(c1, c3);
            const float y13 = f.sub(p[1][1], p[3][1]);
            const float cLast = f.mul(p[2][0], y13);
            const float c = f.add(cSum, cLast);
            if (!f.valid) return false;
            if (c < 0.f) { order[2] = 3; order[3] = 2; }
            else { order[1] = 3; order[2] = 1; order[3] = 2; }
        }
    } else {
        const float y32 = f.sub(p[3][1], p[2][1]);
        const float d0 = f.mul(p[0][0], y32);
        const float d3 = f.mul(p[3][0], y20);
        const float dSum = f.add(d0, d3);
        const float y03 = f.sub(p[0][1], p[3][1]);
        const float dLast = f.mul(p[2][0], y03);
        const float d = f.add(dSum, dLast);
        if (!f.valid) return false;
        const bool negativeD = d < 0.f;
        //2638E4 is an ordinary branch delay slot, reached on BOTH D arms.
        const float y30 = f.sub(p[3][1], p[0][1]);
        if (!f.valid) return false;
        if (negativeD) { order[1] = 3; order[3] = 1; }
        else {
            const float e3 = f.mul(p[3][0], y01);
            const float e1 = f.mul(p[1][0], y30);
            const float eSum = f.add(e1, e3);
            const float y13 = f.sub(p[1][1], p[3][1]);
            const float eLast = f.mul(p[0][0], y13);
            const float e = f.add(eSum, eLast);
            if (!f.valid) return false;
            if (e < 0.f) { order[1] = 2; order[2] = 1; }
            else { order[1] = 2; order[2] = 3; order[3] = 1; }
        }
    }
    for (unsigned i = 0; i != 4; ++i)
        for (unsigned lane = 0; lane != 2; ++lane) quad.xy[i][lane] = p[order[i]][lane];
    return true;
}
}

bool constructPortalViewport230450_50EO(PortalProjectionQuad50EO& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    PortalProjectionQuad50EO next{{{-1.f,-1.f},{-1.f,1.f},{1.f,1.f},{1.f,-1.f}}};
    Arithmetic f;
    if(!reorderPortal(next,f))return false;
    output=next;return true;
}

PortalProjectionStatus50EO projectPortal230880_50EO(
    const PortalGeometry50EO& portal, const Vector3_48C& cameraPosition,
    float cameraNear, const BodyMatrix48G& cameraCombined,
    const PortalProjectionQuad50EO& borrowedQuad434940,
    PortalProjectionQuad50EO& output) noexcept {
    if (std::fegetround() != FE_TONEAREST || !finite(portal.normal) ||
        zero(portal.normal) || !finite(portal.vertices[0]) ||
        !finite(cameraPosition) || !std::isfinite(cameraNear)) return ProjectionStatus::InvalidInput;
    Arithmetic f;
    const float portalDot = f.dot(portal.normal, portal.vertices[0]);
    const float cameraDot = f.dot(portal.normal, cameraPosition);
    const float difference = f.sub(portalDot, cameraDot);
    if (!f.valid) return ProjectionStatus::ArithmeticFailure;
    if (std::fabs(difference) < cameraNear) {
        for (const auto& point : borrowedQuad434940.xy)
            for (float lane : point) if (!std::isfinite(lane)) return ProjectionStatus::InvalidInput;
        output = borrowedQuad434940;
        return ProjectionStatus::Hit;
    }
    //14EAE0 snapshots all16 words; authored corner W is never consumed.
    const BodyMatrix48G matrix = cameraCombined;
    for (float lane : matrix.v) if (!std::isfinite(lane)) return ProjectionStatus::InvalidInput;
    ClipPoint points[10]{};
    unsigned combinedAnd = 0x3fu, combinedOr = 0;
    for (unsigned i = 0; i != 4; ++i) {
        const auto& source = portal.vertices[i];
        if (!finite(source)) return ProjectionStatus::InvalidInput;
        const float forcedW = f.add(1.f, 0.f);
        for (unsigned lane = 0; lane != 4; ++lane)
            points[i].v[lane] = f.mul(matrix.v[lane], source.v[0]);
        for (unsigned axis = 1; axis != 4; ++axis)
            for (unsigned lane = 0; lane != 4; ++lane) {
                const float product = f.mul(matrix.v[axis * 4 + lane], axis == 3 ? forcedW : source.v[axis]);
                points[i].v[lane] = f.add(points[i].v[lane], product);
            }
        if (!f.valid) return ProjectionStatus::ArithmeticFailure;
    }
    for (unsigned i = 0; i != 4; ++i) {
        const unsigned code = portalOutcode(points[i]);
        combinedAnd &= code;
        combinedOr |= code;
    }
    unsigned count = 4;
    if (combinedOr) {
        if (combinedAnd) return ProjectionStatus::Miss;
        const auto status = clipPortal(points, count, f);
        if (status != ProjectionStatus::Hit) return status;
    }
    for (unsigned i = 0; i != count; ++i) {
        if (points[i].v[3] == 0.f) return ProjectionStatus::ArithmeticFailure;
        const float reciprocal = f.div(1.f, points[i].v[3]);
        points[i].v[0] = f.mul(points[i].v[0], reciprocal);
        points[i].v[1] = f.mul(points[i].v[1], reciprocal);
        if (!f.valid) return ProjectionStatus::ArithmeticFailure;
    }
    PortalProjectionQuad50EO next{};
    if (count == 4 || count == 3) {
        for (unsigned i = 0; i != 4; ++i)
            for (unsigned lane = 0; lane != 2; ++lane)
                next.xy[i][lane] = points[i < count ? i : 2].v[lane];
        if (!reorderPortal(next, f)) return ProjectionStatus::ArithmeticFailure;
    } else {
        float minX = points[0].v[0], maxX = minX;
        float minY = points[0].v[1], maxY = minY;
        for (unsigned i = 1; i != count; ++i) {
            if (points[i].v[0] < minX) minX = points[i].v[0];
            if (!(points[i].v[0] <= maxX)) maxX = points[i].v[0];
            if (points[i].v[1] < minY) minY = points[i].v[1];
            if (!(points[i].v[1] <= maxY)) maxY = points[i].v[1];
        }
        next = {{{minX, maxY}, {maxX, maxY}, {maxX, minY}, {minX, minY}}};
    }
    output = next;
    return ProjectionStatus::Hit;
}
}

namespace rr::game {
namespace {
// Original2636A0: directed nonpositive half-planes, including boundaries.
bool viewportContains(const PortalProjectionQuad50EO& q, const float* p,
    Arithmetic& f) noexcept {
    for (unsigned i=0;i<4;++i) {
        const unsigned j=(i+1)&3;
        const float px=f.sub(p[0],q.xy[i][0]);
        const float ny=f.sub(q.xy[i][1],q.xy[j][1]);
        const float py=f.sub(p[1],q.xy[i][1]);
        const float dx=f.sub(q.xy[j][0],q.xy[i][0]);
        const float first=f.mul(ny,px);
        const float second=f.mul(dx,py);
        const float side=f.add(first,second);
        if (!f.valid || !(side<=0.f)) return false;
    }
    return true;
}
// Original262FE0: ordered reciprocal reuse and inclusive segment endpoints.
bool viewportSegment(const float* a0,const float* a1,const float* b0,
    const float* b1,float (&out)[2],Arithmetic& f) noexcept {
    const float sx=f.sub(b1[0],a1[0]);
    const float bx=f.sub(b1[0],b0[0]);
    const float ax=f.sub(a0[0],a1[0]);
    const float by=f.sub(b1[1],b0[1]);
    const float sy=f.sub(b1[1],a1[1]);
    const float ay=f.sub(a0[1],a1[1]);
    const float d0=f.mul(ax,by);
    const float d1=f.mul(bx,ay);
    const float d=f.sub(d0,d1);
    if (!f.valid || d==0.f) return false;
    const float nt0=f.mul(sx,by);
    const float nt1=f.mul(sy,bx);
    const float nt=f.sub(nt0,nt1);
    const float reciprocal=f.div(1.f,d);
    const float t=f.mul(nt,reciprocal);
    if (!f.valid || t<0.f || !(t<=1.f)) return false;
    const float nu0=f.mul(ax,sy);
    const float nu1=f.mul(ay,sx);
    const float nu=f.sub(nu0,nu1);
    const float u=f.mul(nu,reciprocal);
    if (!f.valid || u<0.f) return false;
    const bool upper=u<=1.f;
    //2630AC ordinary branch delay slot: reached even when upper is false.
    const float xt=f.mul(ax,t);
    if (!f.valid || !upper) return false;
    const float x=f.add(a1[0],xt);
    const float yt=f.mul(ay,t);
    const float y=f.add(a1[1],yt);
    if (!f.valid) return false;
    out[0]=x;out[1]=y;return true;
}
bool viewportWeakTurns(const PortalProjectionQuad50EO& q,Arithmetic& f) noexcept {
    float edges[4][2]{};
    for (unsigned i=0;i<4;++i) {
        edges[i][0]=f.sub(q.xy[(i+1)&3][0],q.xy[i][0]);
        edges[i][1]=f.sub(q.xy[(i+1)&3][1],q.xy[i][1]);
    }
    if (!f.valid) return false;
    for (unsigned i=0;i<4;++i) {
        const unsigned j=(i+1)&3;
        const float negativeY=-edges[i][1];
        const float first=f.mul(edges[i][0],edges[j][1]);
        const float second=f.mul(negativeY,edges[j][0]);
        const float turn=f.add(first,second);
        if (!f.valid || !(turn<=0.f)) return false;
    }
    return true;
}
}
PortalProjectionStatus50EO intersectPortalViewport263260_50EO(
    const PortalProjectionQuad50EO& a,const PortalProjectionQuad50EO& b,
    PortalProjectionQuad50EO& output) noexcept {
    using Result=PortalProjectionStatus50EO;
    if (std::fegetround()!=FE_TONEAREST) return Result::InvalidInput;
    for (unsigned i=0;i<4;++i)for(unsigned lane=0;lane<2;++lane)
        if(!std::isfinite(a.xy[i][lane])||!std::isfinite(b.xy[i][lane]))return Result::InvalidInput;
    Arithmetic f;
    float candidates[24][2]{};unsigned count=0;
    auto append=[&](const float* p) {
        if(count==24)return false;
        candidates[count][0]=p[0];candidates[count][1]=p[1];++count;return true;
    };
    unsigned contained=0;
    for(unsigned i=0;i<4;++i) {
        const bool inside=viewportContains(a,b.xy[i],f);
        if(!f.valid)return Result::ArithmeticFailure;
        if(inside){if(!append(b.xy[i]))return Result::CapacityExceeded;++contained;}
    }
    if(contained==4){output=b;return Result::Hit;}
    contained=0;
    for(unsigned i=0;i<4;++i) {
        const bool inside=viewportContains(b,a.xy[i],f);
        if(!f.valid)return Result::ArithmeticFailure;
        if(inside){if(!append(a.xy[i]))return Result::CapacityExceeded;++contained;}
    }
    if(contained==4){output=a;return Result::Hit;}
    for(unsigned i=0;i<4;++i)for(unsigned j=0;j<4;++j) {
        float point[2]{};
        const bool hit=viewportSegment(a.xy[i],a.xy[(i+1)&3],b.xy[j],b.xy[(j+1)&3],point,f);
        if(!f.valid)return Result::ArithmeticFailure;
        if(hit&&!append(point))return Result::CapacityExceeded;
    }
    if(!count)return Result::Miss;
    PortalProjectionQuad50EO result;
    if(count==4) {
        for(unsigned i=0;i<4;++i)for(unsigned lane=0;lane<2;++lane)result.xy[i][lane]=candidates[i][lane];
        if(!reorderPortal(result,f))return Result::ArithmeticFailure;
        const bool accepted=viewportWeakTurns(result,f);
        if(!f.valid)return Result::ArithmeticFailure;
        if(accepted){output=result;return Result::Hit;}
    }
    float minX=candidates[0][0],maxX=minX,minY=candidates[0][1],maxY=minY;
    for(unsigned i=1;i<count;++i) {
        const float x=candidates[i][0],y=candidates[i][1];
        if(x<minX)minX=x;
        if(!(x<=maxX))maxX=x;
        if(y<minY)minY=y;
        if(!(y<=maxY))maxY=y;
    }
    result.xy[0][0]=minX;result.xy[0][1]=maxY;
    result.xy[1][0]=maxX;result.xy[1][1]=maxY;
    result.xy[2][0]=maxX;result.xy[2][1]=minY;
    result.xy[3][0]=minX;result.xy[3][1]=minY;
    output=result;return Result::Hit;
}
}

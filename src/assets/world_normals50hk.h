#pragma once
#include <array>
#include <cmath>

namespace rr::assets {
// Separate stream: RRMESH01 and Vertex48B remain byte-for-byte compatible.
struct WorldNormal50HK { float xyz[3]{}; };
static_assert(sizeof(WorldNormal50HK) == 12, "World normal float3 stream");

// Source uses row-vector affine matrices. The inverse transpose preserves
// perpendicularity under nonuniform scale/shear as well as rigid placements.
// No neighboring vertices are averaged: authored splits and hard edges remain.
// A zero authored normal stays zero, requesting geometric fallback in shader.
inline bool transformWorldNormal50HK(const std::array<double, 3>& source,
    const std::array<double, 16>& m, WorldNormal50HK& output) noexcept {
    for (double v : source) if (!std::isfinite(v)) return false;
    for (double v : m) if (!std::isfinite(v)) return false;
    if (m[3] != 0 || m[7] != 0 || m[11] != 0 || m[15] != 1) return false;
    const double a=m[0], b=m[1], c=m[2], d=m[4], e=m[5], f=m[6],
        g=m[8], h=m[9], i=m[10];
    const double cofactor[9] = {e*i-f*h, f*g-d*i, d*h-e*g,
        c*h-b*i, a*i-c*g, b*g-a*h, b*f-c*e, c*d-a*f, a*e-b*d};
    const double determinant = a*cofactor[0]+b*cofactor[1]+c*cofactor[2];
    if (!std::isfinite(determinant) || determinant == 0) return false;
    double transformed[3]{};
    for (unsigned axis=0; axis<3; ++axis) {
        transformed[axis] = (source[0]*cofactor[axis] + source[1]*cofactor[3+axis] +
            source[2]*cofactor[6+axis]) / determinant;
        if (!std::isfinite(transformed[axis])) return false;
    }
    const double length = std::hypot(transformed[0], transformed[1], transformed[2]);
    if (!std::isfinite(length)) return false;
    WorldNormal50HK next{};
    if (length > 0) for (unsigned axis=0; axis<3; ++axis)
        next.xyz[axis] = static_cast<float>(transformed[axis] / length);
    output=next;
    return true;
}
} // namespace rr::assets

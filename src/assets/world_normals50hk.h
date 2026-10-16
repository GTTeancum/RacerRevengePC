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
struct WorldNormalTransform50HK {
    std::array<double, 9> cofactor{};
    double determinant=0;
};
inline bool prepareWorldNormalTransform50HK(const std::array<double, 16>& m,
    WorldNormalTransform50HK& output) noexcept {
    for (double v : m) if (!std::isfinite(v)) return false;
    if (m[3] != 0 || m[7] != 0 || m[11] != 0 || m[15] != 1) return false;
    const double a=m[0], b=m[1], c=m[2], d=m[4], e=m[5], f=m[6],
        g=m[8], h=m[9], i=m[10];
    const double cofactor[9] = {e*i-f*h, f*g-d*i, d*h-e*g,
        c*h-b*i, a*i-c*g, b*g-a*h, b*f-c*e, c*d-a*f, a*e-b*d};
    const double determinant = a*cofactor[0]+b*cofactor[1]+c*cofactor[2];
    if (!std::isfinite(determinant) || determinant == 0) return false;
    output={{cofactor[0],cofactor[1],cofactor[2],cofactor[3],cofactor[4],
        cofactor[5],cofactor[6],cofactor[7],cofactor[8]},determinant};
    return true;
}
// Caller retains a successful preparation for this exact current matrix.
// Preserve division and per-vertex arithmetic order; do not predivide cofactors.
inline bool transformPreparedWorldNormal50HK(const std::array<double, 3>& source,
    const WorldNormalTransform50HK& prepared, WorldNormal50HK& output) noexcept {
    for (double v : source) if (!std::isfinite(v)) return false;
    const auto& cofactor=prepared.cofactor;
    const double determinant=prepared.determinant;
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
inline bool transformWorldNormal50HK(const std::array<double, 3>& source,
    const std::array<double, 16>& m, WorldNormal50HK& output) noexcept {
    for (double v : source) if (!std::isfinite(v)) return false;
    WorldNormalTransform50HK prepared;
    if (!prepareWorldNormalTransform50HK(m,prepared)) return false;
    return transformPreparedWorldNormal50HK(source,prepared,output);
}
} // namespace rr::assets

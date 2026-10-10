#pragma once
// Native presentation approximation, not recovered game shadow rendering.
// Fit one soft directional footprint to the actual current opaque vertices of
// each submitted pod part. The track shader receives it on real world-space
// triangles; this is not a ground-plane decal, collision or physics change.
#include "../../../src/game/vehicle_draw50cm.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::platform {
// Surface-to-light direction shared with native world illumination. Only the
// X/Y and Z/Y ratios are needed to project down onto an arbitrary receiver Y.
inline constexpr double nativeShadowSlopeX50HM = 0.35 / 0.85;
inline constexpr double nativeShadowSlopeZ50HM = 0.30 / 0.85;
struct NativePodShadow50HM {
    // World-space footprint center at its highest opaque vertex, then bounded
    // receiver reach. A zero opacity disables this caster.
    float centerAndReach[4]{};
    // Symmetric inverse ellipse: qxx, qxz, qzz, maximum occlusion.
    float shapeAndOpacity[4]{};
};

// The caller explicitly chooses opaque, depth-writing spans. Exclude flames,
// transparent effects and non-depth-writing materials; never infer by names.
// Uses the current transformed vertex payload, never serialized model bounds
// or a later physical pose. Selected span vertices are source ordered and
// bounded by maxRigidVertices49S; no heap storage, GL calls or state mutation.
// A successful empty/degenerate selection disables the caster. False preserves
// output, so the caller must reject this render or explicitly clear on failure.
inline bool buildNativePodShadow50HM(const game::VehicleDrawSubmission50CM& mesh,
    std::uint64_t opaqueSpanMask, NativePodShadow50HM& output) noexcept {
    if (!mesh.vertices || !mesh.vertexCount || mesh.vertexCount > game::maxRigidVertices49S ||
        !mesh.spans || !mesh.spanCount || mesh.spanCount > game::maxVehicleDrawSpans50CM ||
        (mesh.spanCount < 64 && (opaqueSpanMask >> mesh.spanCount))) return false;
    std::uint32_t expectedVertex = 0;
    for (std::uint32_t i = 0; i < mesh.spanCount; ++i) {
        const auto& span = mesh.spans[i];
        if (span.vertex_start != expectedVertex || !span.vertex_count ||
            span.vertex_count > mesh.vertexCount - expectedVertex) return false;
        expectedVertex += span.vertex_count;
    }
    if (expectedVertex != mesh.vertexCount) return false;
    NativePodShadow50HM result{};
    if (!opaqueSpanMask) { output = result; return true; }
    double low[3] = {INFINITY, INFINITY, INFINITY};
    double high[3] = {-INFINITY, -INFINITY, -INFINITY};
    std::uint32_t samples = 0;
    for (std::uint32_t i = 0; i < mesh.spanCount; ++i) {
        if (!(opaqueSpanMask & (std::uint64_t{1} << i))) continue;
        const auto& span = mesh.spans[i];
        for (std::uint32_t n = span.vertex_start; n < span.vertex_start + span.vertex_count; ++n) {
            const auto& p = mesh.vertices[n].position;
            for (unsigned axis = 0; axis < 3; ++axis) {
                if (!std::isfinite(p[axis])) return false;
                low[axis] = std::min(low[axis], double(p[axis]));
                high[axis] = std::max(high[axis], double(p[axis]));
            }
            ++samples;
        }
    }
    if (!samples) { output = result; return true; }
    // Relative coordinates and double accumulation avoid subtracting squared
    // large world positions. Rotation follows the actual presented body.
    const double originX = (low[0] + high[0]) * .5;
    const double originY = high[1];
    const double originZ = (low[2] + high[2]) * .5;
    double sumX = 0., sumZ = 0., sumXX = 0., sumXZ = 0., sumZZ = 0.;
    for (std::uint32_t i = 0; i < mesh.spanCount; ++i) {
        if (!(opaqueSpanMask & (std::uint64_t{1} << i))) continue;
        const auto& span = mesh.spans[i];
        for (std::uint32_t n = span.vertex_start; n < span.vertex_start + span.vertex_count; ++n) {
            const auto& p = mesh.vertices[n].position;
            const double x = double(p[0]) - originX - nativeShadowSlopeX50HM * (double(p[1]) - originY);
            const double z = double(p[2]) - originZ - nativeShadowSlopeZ50HM * (double(p[1]) - originY);
            sumX += x; sumZ += z; sumXX += x*x; sumXZ += x*z; sumZZ += z*z;
        }
    }
    const double covarianceXX = sumXX - sumX*sumX/samples;
    const double covarianceXZ = sumXZ - sumX*sumZ/samples;
    const double covarianceZZ = sumZZ - sumZ*sumZ/samples;
    const double angle = .5 * std::atan2(2.*covarianceXZ, covarianceXX-covarianceZZ);
    const double uX = std::cos(angle), uZ = std::sin(angle);
    double minU = INFINITY, maxU = -INFINITY, minV = INFINITY, maxV = -INFINITY;
    for (std::uint32_t i = 0; i < mesh.spanCount; ++i) {
        if (!(opaqueSpanMask & (std::uint64_t{1} << i))) continue;
        const auto& span = mesh.spans[i];
        for (std::uint32_t n = span.vertex_start; n < span.vertex_start + span.vertex_count; ++n) {
            const auto& p = mesh.vertices[n].position;
            const double x = double(p[0]) - originX - nativeShadowSlopeX50HM * (double(p[1]) - originY);
            const double z = double(p[2]) - originZ - nativeShadowSlopeZ50HM * (double(p[1]) - originY);
            const double u = x*uX + z*uZ, v = -x*uZ + z*uX;
            minU = std::min(minU, u); maxU = std::max(maxU, u);
            minV = std::min(minV, v); maxV = std::max(maxV, v);
        }
    }
    // Explicit native presentation values: 10% soft padding and at most 48%
    // darkening. A bounded height fade avoids shading distant lower decks.
    const double radiusU = (maxU-minU)*.55, radiusV = (maxV-minV)*.55;
    if (radiusU < 1e-4 || radiusV < 1e-4) { output = result; return true; }
    const double midU = (minU+maxU)*.5, midV = (minV+maxV)*.5;
    const double inverseU = 1./(radiusU*radiusU), inverseV = 1./(radiusV*radiusV);
    const double values[8] = {
        originX + midU*uX - midV*uZ, originY, originZ + midU*uZ + midV*uX,
        std::clamp(8.*std::sqrt(radiusU*radiusV), 32., 256.),
        uX*uX*inverseU + uZ*uZ*inverseV, uX*uZ*(inverseU-inverseV),
        uZ*uZ*inverseU + uX*uX*inverseV, .48
    };
    for (unsigned i = 0; i < 8; ++i) {
        if (!std::isfinite(values[i]) || std::abs(values[i]) > std::numeric_limits<float>::max()) return false;
        (i < 4 ? result.centerAndReach[i] : result.shapeAndOpacity[i-4]) = static_cast<float>(values[i]);
    }
    // Float uniforms must retain a positive-definite ellipse. Fail closed on
    // extreme/unrepresentable footprints instead of shadowing the whole world.
    const double qxx = result.shapeAndOpacity[0], qxz = result.shapeAndOpacity[1];
    const double qzz = result.shapeAndOpacity[2];
    if (!(qxx > 0. && qzz > 0. && qxx*qzz > qxz*qxz)) return false;
    output = result;
    return true;
}

// Conservative world-XZ receiver bounds for one submitted racer's three
// footprints. Only opponents need this broad phase; the player's original
// three-caster shader path stays unchanged. Includes every permitted height,
// projected light offset and soft-edge spread, rather than model-space bounds.
// False preserves output. An empty group uses inverted bounds and shades none.
inline bool buildNativePodShadowReceiverBounds50HM(
    const NativePodShadow50HM (&shadows)[3], float (&output)[4]) noexcept {
    double lowX = INFINITY, lowZ = INFINITY, highX = -INFINITY, highZ = -INFINITY;
    bool enabled = false;
    for (const auto& shadow : shadows) {
        if (shadow.shapeAndOpacity[3] <= 0.f) continue;
        for (float value : shadow.centerAndReach)
            if (!std::isfinite(value)) return false;
        for (float value : shadow.shapeAndOpacity)
            if (!std::isfinite(value)) return false;
        const auto& center = shadow.centerAndReach;
        const double qxx = shadow.shapeAndOpacity[0], qxz = shadow.shapeAndOpacity[1];
        const double qzz = shadow.shapeAndOpacity[2], determinant = qxx*qzz - qxz*qxz;
        if (!(qxx > 0. && qzz > 0. && determinant > 0. && center[3] > 0.f)) return false;
        // sqrt(diag(inverse(Q))) gives each axis's ellipse support. 1.75
        // conservatively covers maximum spread 1.6 * sqrt(edge radius 1.15),
        // with margin for the shader's float arithmetic near the soft edge.
        const double radiusX = 1.75 * std::sqrt(qzz / determinant);
        const double radiusZ = 1.75 * std::sqrt(qxx / determinant);
        lowX = std::min(lowX, double(center[0]) - nativeShadowSlopeX50HM*center[3] - radiusX);
        lowZ = std::min(lowZ, double(center[2]) - nativeShadowSlopeZ50HM*center[3] - radiusZ);
        highX = std::max(highX, double(center[0]) + radiusX);
        highZ = std::max(highZ, double(center[2]) + radiusZ);
        enabled = true;
    }
    float result[4] = {1.f, 1.f, -1.f, -1.f};
    if (enabled) {
        const double values[4] = {lowX, lowZ, highX, highZ};
        for (unsigned i = 0; i < 4; ++i) {
            if (!std::isfinite(values[i]) || std::abs(values[i]) > std::numeric_limits<float>::max())
                return false;
            // Round outward, including when a tiny footprint lies at a large
            // world coordinate. Never accidentally clip its own receiver edge.
            result[i] = std::nextafter(static_cast<float>(values[i]),
                i < 2 ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity());
            if (!std::isfinite(result[i])) return false;
        }
    }
    std::copy(result, result + 4, output);
    return true;
}
}

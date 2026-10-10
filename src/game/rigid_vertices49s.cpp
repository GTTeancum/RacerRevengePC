#include "rigid_vertices49s.h"
#include <cfenv>
#include <cmath>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Rigid diagnostic projection requires strict finite IEEE arithmetic"
#endif

namespace rr::game {
namespace {
static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559,
    "IEEE binary64 required");
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
    "IEEE binary32 required");
bool reject(RigidVertexDiagnostic49S& d, RigidVertexError49S error,
    std::uint32_t vertex = 0, std::uint32_t matrix = 0, std::uint32_t lane = 0) noexcept {
    d = {error, vertex, matrix, lane};
    return false;
}
bool project(const RigidVertex49S& vertex, const BodyMatrix48G& matrix,
    RigidPosition49S& position, std::uint32_t index,
    RigidVertexDiagnostic49S& d) noexcept {
    for (std::uint32_t axis = 0; axis < 3; ++axis) {
        double sum = 0;
        for (std::uint32_t k = 0; k < 3; ++k) {
            const double product = vertex.positionLocal[k] * static_cast<double>(matrix.v[k * 4 + axis]);
            if (!std::isfinite(product))
                return reject(d, RigidVertexError49S::arithmetic_nonfinite, index, vertex.paletteIndex, axis);
            sum = sum + product;
            if (!std::isfinite(sum))
                return reject(d, RigidVertexError49S::arithmetic_nonfinite, index, vertex.paletteIndex, axis);
        }
        const double value = sum + static_cast<double>(matrix.v[12 + axis]);
        if (!std::isfinite(value))
            return reject(d, RigidVertexError49S::arithmetic_nonfinite, index, vertex.paletteIndex, axis);
        const double maximum = static_cast<double>(std::numeric_limits<float>::max());
        if (value > maximum || value < -maximum)
            return reject(d, RigidVertexError49S::output_range, index, vertex.paletteIndex, axis);
        position.v[axis] = static_cast<float>(value);
    }
    return true;
}
}

bool projectRigidVertices49S(RigidVertexView49S input, RigidPaletteView49S palette,
    RigidPositionView49S output, RigidVertexDiagnostic49S& d) noexcept {
    d = {RigidVertexError49S::none, 0, 0, 0};
    if (!input.vertices || !palette.matrices || !output.positions)
        return reject(d, RigidVertexError49S::invalid_view);
    if (!input.count || input.count > maxRigidVertices49S || !palette.count ||
        palette.count > maxRigidPalette49S || output.capacity > maxRigidVertices49S)
        return reject(d, RigidVertexError49S::count_limit);
    if (output.capacity < input.count)
        return reject(d, RigidVertexError49S::output_capacity);
    if (std::fegetround() != FE_TONEAREST)
        return reject(d, RigidVertexError49S::rounding_mode);
    for (std::uint32_t i = 0; i < palette.count; ++i) {
        const auto& m = palette.matrices[i];
        for (std::uint32_t lane = 0; lane < 16; ++lane)
            if (!std::isfinite(m.v[lane]))
                return reject(d, RigidVertexError49S::matrix_nonfinite, 0, i, lane);
        for (std::uint32_t lane = 3; lane < 16; lane += 4)
            if (m.v[lane] != (lane == 15 ? 1.0f : 0.0f))
                return reject(d, RigidVertexError49S::matrix_nonaffine, 0, i, lane);
    }
    for (std::uint32_t i = 0; i < input.count; ++i) {
        const auto& v = input.vertices[i];
        if (v.paletteIndex >= palette.count)
            return reject(d, RigidVertexError49S::palette_index, i, v.paletteIndex);
        for (std::uint32_t lane = 0; lane < 3; ++lane)
            if (!std::isfinite(v.positionLocal[lane]))
                return reject(d, RigidVertexError49S::position_nonfinite, i, v.paletteIndex, lane);
        RigidPosition49S checked{};
        if (!project(v, palette.matrices[v.paletteIndex], checked, i, d)) return false;
    }
    for (std::uint32_t i = 0; i < input.count; ++i) {
        RigidPosition49S position{};
        // The first pass admitted the same stable input, palette and FP mode.
        (void)project(input.vertices[i], palette.matrices[input.vertices[i].paletteIndex], position, i, d);
        output.positions[i] = position;
    }
    return true;
}
}

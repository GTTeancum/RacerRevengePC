#pragma once
#include "body_scene48g.h"
#include <cstdint>

namespace rr::game {
constexpr std::uint32_t maxRigidVertices49S = 8192;
constexpr std::uint32_t maxRigidPalette49S = 64;

// Bounded storage includes POD02's authentic 4124-vertex chariot.
// Compact, immutable projection of the admitted RRSCENE1 semantic position
// and rigid bone association. No bind transform has been applied to positions.
struct RigidVertex49S {
    double positionLocal[3];
    std::uint16_t paletteIndex;
};
struct RigidPosition49S { float v[3]; };
struct RigidVertexView49S { const RigidVertex49S* vertices; std::uint32_t count; };
struct RigidPaletteView49S { const BodyMatrix48G* matrices; std::uint32_t count; };
struct RigidPositionView49S { RigidPosition49S* positions; std::uint32_t capacity; };
enum class RigidVertexError49S : std::uint8_t {
    none, invalid_view, count_limit, output_capacity, rounding_mode,
    matrix_nonfinite, matrix_nonaffine, palette_index, position_nonfinite,
    arithmetic_nonfinite, output_range
};
struct RigidVertexDiagnostic49S {
    RigidVertexError49S error;
    // Vertex/matrix element reported for the failed check; zero otherwise.
    std::uint32_t vertex, matrix, lane;
};

// Diagnostic cache/display arithmetic, NOT original VU vertex-program parity.
// Each already-resolved world matrix is consumed directly: no inverse bind,
// hierarchy traversal, root/placement reapplication, UV/color/index mutation,
// allocation, I/O, callback or ownership operation.
//
// Admission: 1..8192 vertices, 1..64 finite affine matrices; finite local XYZ;
// valid palette IDs; output capacity between input.count and8192. All views,
// backing arrays and diagnostic are distinct live caller-owned records with
// stable contents/lifetimes for this serial call. Overlap and lifetime are
// preconditions, not inferred or repaired by this function.
//
// Requires FE_TONEAREST, strict IEEE binary64/binary32 and contraction disabled.
// For each axis start double sum=0, multiply/add k=0,1,2 separately in order,
// add translation, then convert once to float. Matrices are promoted exactly
// from float. Intermediate results must be finite and the final double must
// lie within +/-FLT_MAX. Signed zeros follow those operations.
//
// Two passes validate before committing. On false all output positions remain
// untouched; diagnostic reports the rejection. On true only input.count output
// positions are written. No scratch allocation is used.
bool projectRigidVertices49S(RigidVertexView49S input, RigidPaletteView49S palette,
    RigidPositionView49S output, RigidVertexDiagnostic49S& diagnostic) noexcept;
}

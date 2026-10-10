#pragma once

namespace rr::assets {
// A cold source-derived sidecar. The original flattened position/UV/color and
// index streams remain unchanged. XY is authored pass one; ZW is pass two.
// ProjectedPrelit uses ordinary perspective-correct affine XY coordinates.
struct TrackProjectedUv { float uv[4]{}; };
}

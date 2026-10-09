#pragma once
#include "vehicle_visual49r.h"

namespace rr::game::detail {
// Pure numerical leaf of21BA08..21BE7C. The caller supplies the freshly
// produced virtual matrix (source stack260 / hidden198.localC0) and the
// secondary body's existing scene.world100 cache. This leaf never refreshes
// a scene or substitutes its local matrix. It extracts rows3,2,1 from virtual,
// then3,2,1 from physical, with source14EBB0's synthesized W=1.
//
// Source clamps oldElapsed84C / positiveDuration848 per XYZ lane, then always
// rebuilds the basis, including at both endpoints. Strict nearest/even separate
// binary32 operations; finite consumed arithmetic and surviving VU scratch;
// positive subnormal squared VU norms are excluded, truezero is retained.
// Source row0 and input fourth lanes are not consumed. No graph/state writes,
// callbacks, allocation or I/O. False preserves output. Caller publishes the
// output before adding frameFactor to84C and clearing83C only for updated>D.
bool blendChariotMatrix49V(const BodyMatrix48G& newlyProducedVirtualLocal,
    const BodyMatrix48G& secondaryCachedWorld, float oldElapsed84C,
    float positiveDuration848, BodyMatrix48G& output) noexcept;
}

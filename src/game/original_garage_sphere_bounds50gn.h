#pragma once
#include "scene_graph49p.h"
#include <array>
#include <cstdint>
#include <optional>

namespace rr::game {
// One retained Application lifetime. Never reset this source static on page
// teardown. 26F4D4..F4 initializes assertion442F60 on the first sphere call.
struct OriginalGarageSphereShared50GN {
    bool lazy8B5C=false;
    std::int32_t assertion442F60=0;
};
struct OriginalGarageSphereRow50GN {
    SceneNodeIndex49P node=SceneNodeIndex49P::none;
    std::array<float,4> center140{};
    // An absent original194 must not become a synthesized zero. It is needed
    // only when this canonical scene row has actual190 bit04 set.
    std::optional<float> radius194;
};
struct OriginalGarageSphereResult50GN {
    std::array<float,4> center160{};
    float radius198=0;
    // Source writes170 XYZ to radius198, but DOES NOT write170 W.
    std::uint8_t rootSceneFlags190=0;
};
struct OriginalGarageSphereDiagnostic50GN {
    std::uint32_t sourcePC=0;
    std::int32_t sourceRow=-1;
};
// Exact bounded26F4B0 bit10 route and flat28D940(a3=0) iteration. Source rows
// must be supplied in the actual model1B8 order, not hierarchy traversal order.
// The caller owns/authenticates the model count, row table and stored194 words.
// This performs actual lazy2897B0 reads on the borrowed canonical graph; ready
// cached worlds are not regenerated from locals. Filtered04-clear rows are not
// refreshed or numerically inspected. No recursion is added to28D940.
// The first positive-radius center stays fixed; expansion is max-absolute XYZ
// distance plus stored194, NOT Euclidean distance or a minimal enclosing sphere.
// Root-local160 uses the actual28BD50 transpose/inverse-translation operation
// order, not a general inverse or a subtract-translation-first shortcut.
// Serial FE_TONEAREST, finite non-subnormal arithmetic only. Output is committed
// only on success; successful graph/lazy-global prefixes survive later failure.
// It does not publish bounds, change observer membership, clear80, or draw.
bool computeOriginalGarageSphereBounds26F4B050GN(const SceneGraph49P&,
    const OriginalGarageSphereRow50GN& root,
    const OriginalGarageSphereRow50GN* sourceRows,std::uint32_t sourceRowCount,
    OriginalGarageSphereShared50GN&,OriginalGarageSphereResult50GN&,
    OriginalGarageSphereDiagnostic50GN&)noexcept;
}

#pragma once
#include "original_camera_query_owner.h"
#include <array>

namespace rr::game {
struct OriginalCameraQueryColdInput {
    // Actual camera179320/178 (constructor default3.5, loader may replace it).
    float radius178=0.f;
    SpringVector4_48N initialPosition{};
    // SAME initialized application globals read by179A80/11B190. Their source
    // initializers3C8220/3C966C supply (+0,+0,+0,1); no private mutable cache.
    SpringVector4_48N sentinel3E02C0{},zero433DB0{};
    std::uint8_t directTypeTagGp8010=0;
};
enum class OriginalCameraQueryColdStatus { Complete,Binding,Arithmetic };
struct OriginalCameraQueryColdProjection {
    OriginalCameraQueryColdStatus status=OriginalCameraQueryColdStatus::Binding;
    std::uint32_t sourcePC=0;
    OriginalCameraQueryPairState pair{};
    // Index0=sphere camera24; index1=ray camera190. Identities come from the
    // caller's actual object records, never fabricated numeric scene IDs.
    std::array<CollectionObject50BX,2> objects{};
    std::array<CollectionObjectState50BX,2> collection{};
    std::array<OriginalCameraWorldObjectCopies,2> copies{};
};
// Source-only numeric projection of the TWO fresh179A80 allocation branches:
//11B190(1),11B570(2),123EA0(1),11C0B0/11C710(1,0,0), and their initial11FF60.
// The ray branch includes its initial11CA80(position,position). Projection
// precedes each branch's detach/setter/120070; attachOriginalCameraQueryPair
// owns those actual live registrations, in source sphere-then-ray order.
//
// All priors are fully initialized native storage carrying unspecified source
// bytes. This function overwrites ONLY source-written fields; in particular it
// preserves ray cachedRadius80, shape radius40, result hitCount60/fraction74,
// ray hit/fraction/length, opaque78/7C, and sphere result planeDistance08.
// The two flag aliases must agree on entry. Native inactive record fields and
// unrelated shape bookkeeping are retained. No raw uninitialized C++ objects.
//
// Constructor epoch=0, flags31 low nibble=3 and F8 low6=3 are published along
// with the actual filter defaults, copied40/50, previous70 and distance84.
// Fresh shape3's11FF60 is a no-op: real120070/1202E0 FIRST computes its cache.
// Sphere initial11FF60 publishes its actual zero-center/radius cache; the later
// setter changes shape00 without refreshing that cache before registration.
//
// Successful receipt must be joined to the SAME actual pair/object/cache/copy
// owners before attachment. Caller binds E4=1792D0 and owner9C, keeps E8/EC/F0
// null, and provides detached E0 backing. No allocation, name registration,
// scene/node construction, global3E0730 write, or gameplay bootstrap is claimed.
// Input scope is initialized source globals, finite positive radius and its
// finite binary32 square, FE_TONEAREST/no contraction. Failure mutates nothing;
// only status/sourcePC in the by-value result are meaningful on failure.
OriginalCameraQueryColdProjection projectOriginalCameraQueryCold(
    const OriginalCameraQueryColdInput&,const OriginalCameraQueryPairState& priorPair,
    const std::array<CollectionObject50BX,2>& priorObjects,
    const std::array<CollectionObjectState50BX,2>& priorCollection) noexcept;
}

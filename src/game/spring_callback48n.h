#pragma once
#include "spring_records48n.h"

namespace rr::game {
// Original type2 result arrays +50/+54/+58, coalesced without changing lanes.
// These are CUMULATIVE per-pair values at this callback, including untouched
// pairs. The callback does not test a hit flag, transform, normalize or dedupe.
struct SpringCumulativePair48N {
    SpringVector4_48N first,second,normal,surfacePoint;
};
struct SpringStaticCallbackInput48N {
    const SpringCumulativePair48N* pairs=nullptr;
    std::size_t pairCount=0;
    std::uint32_t candidateIdentity=0,candidateCategoryA4=0;
    // Category of state.currentCandidate130, ignored when that identity is zero.
    std::uint32_t currentCandidateCategoryA4=0;
    std::uint32_t skipMask=0,dynamicMask=0,materialMask=0;
    std::uint32_t queryTypeTag=2;
    SpringSceneId48N queryScene{0}; // query shape+44, not material scene
    bool hasMaterialScene=false;   // candidate+9C is present
    const char* sceneName=nullptr;  // material scene+38, includes a bounded NUL
    std::size_t sceneNameBytes=0;
};
struct SpringStaticCallbackResult48N {
    std::size_t appended=0,saturated=0;
    std::uint32_t material=0;
};
enum class SpringCallbackStatus48N { Complete,Skipped,UnsupportedScope,InvalidInput,InsufficientCapacity };
// Original294BB0 static type2 callback. Skip mask is tested first and Skipped
// leaves ALL writable arguments unchanged. Dynamic categories and query tags
// other than2 are unsupported and transactional. No virtual-body calls occur.
// The first '.' or NUL delimits the suffix. Exactly "frX" directly before it
// yields signed-byte(X)-'0', with no digit validation. Missing delimiter/NUL is
// InvalidInput; delimiter before index3 is UnsupportedScope (original may read
// preceding scene bytes). No material scene or unmatched category yields0.
// currentCandidate130 changes only if previously zero or its explicit previous
// category matches dynamicMask. No reset occurs here; ordered-query ownership
// performs resetSpringRecords48N before starting when the original calls it.
// Native storage must cover the full nonnegative logical record capacity;
// original saturation is successful Complete with a saturated count. Input
// descriptors, pair/name spans, state, records and result must not overlap any
// writable span. All other outcomes leave state/records/result unchanged.
// Fixed stack, no allocation, no locks/global state, no guest-memory reads.
SpringCallbackStatus48N applySpringStaticCallback48N(const SpringStaticCallbackInput48N&,
    SpringRecord48N*,std::size_t storageCapacity,SpringRecordState48N&,
    SpringStaticCallbackResult48N&) noexcept;
}

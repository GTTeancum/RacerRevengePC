#pragma once
#include "body_scene48g.h"
#include "spring_callback48n.h"

namespace rr::game {
struct SpringStaticType3CallbackInput48P {
    // Cached (query+A0)->scene+100; original does not refresh or inspect flags.
    BodyMatrix48G bodyScene{};
    bool bodyScenePresent=true;
    std::uint32_t candidateIdentity=0,candidateCategoryA4=0,currentCandidateCategoryA4=0;
    std::uint32_t skipMask=0,dynamicMask=0,materialMask=0,queryTypeTag=3;
    bool hasMaterialScene=false;
    const char* sceneName=nullptr;
    std::size_t sceneNameBytes=0;
};
struct SpringStaticType3CallbackSource48P {
    const SpringVector4_48N* worldStarts=nullptr;std::size_t worldStartCount=0;
    const SpringVector4_48N* worldDirections=nullptr;std::size_t worldDirectionCount=0;
    // shape+14: EVERY hit consumes lengths[0..2], not lengths[i]. Stale backing
    // must be explicitly supplied when the shape has fewer than three pairs.
    const float* lengths=nullptr;std::size_t lengthCount=0;
    const SpringVector4_48N* normals=nullptr;std::size_t normalCount=0;
    const SpringVector4_48N* surfacePoints=nullptr;std::size_t surfacePointCount=0;
    const std::uint32_t* hitFlags=nullptr;std::size_t hitFlagCount=0;
    // Raw shape+04 words. Callback reads sceneMetadata[i], not [2*i].
    const std::uint32_t* sceneMetadata=nullptr;std::size_t sceneMetadataCount=0;
};
// Original294BB0 static type3 callback. Signed state.pointCount7C drives the
// loop; only nonzero hitFlags[i] append, in ascending order. Negative counts
// have no iterations. A hit reconstructs start.xyz + direction.xyz*lengths[0..2]
// and inverse-transforms start/end by cached bodyScene transpose3x3 after
// translation subtraction and original add-zero matrix copies. Local Ws are1;
// normal/surface vec4 bits are unchanged. No fraction substitution or refresh.
// Skip is tested first and leaves all writable arguments unchanged. Dynamic
// categories/other query tags are unsupported. Material/currentCandidate prefix
// uses the48N implementation, and append/saturation/stale bytes use48N records.
// No-hit calls ignore bodyScene and all source spans except needed hitFlags.
// Other spans must cover the highest hit index, lengths must cover3. Consumed
// arithmetic inputs/intermediates must be finite; unused W and copied payloads
// need not be. No orthogonality/affine-W constraint is invented.
// Full declared consumed spans participate in pointer/alignment/overlap checks;
// state, records and result are disjoint from one another and all consumed
// readable spans/descriptors. Invalid/capacity/unsupported outcomes are fully
// transactional. The full nonnegative logical record capacity must be backed.
// Two bounded arithmetic passes, constant stack, no allocation, threads, locks
// or guest memory. Scratch does not escape. Strict binary32, no fast math/FMA.
SpringCallbackStatus48N applySpringStaticType3Callback48P(
    const SpringStaticType3CallbackInput48P&,const SpringStaticType3CallbackSource48P&,
    SpringRecord48N*,std::size_t storageCapacity,SpringRecordState48N&,
    SpringStaticCallbackResult48N&) noexcept;
}

#pragma once
#include "pod_shape49w.h"
#include "collection_bounds48s.h"
namespace rr::game {
struct Type0BoundsInput50EW {
    const PodShapeSource49W* resource=nullptr;
    BodyMatrix48G composed00{};
    std::uint32_t cachedCenterWords[4]{},cachedRadiusWord=0;
};
// Original120378..1203F8 then120C50 common tail. Reads the real type0 root's
// center/half and already-composed shape00; does not substitute world80 or
// recompute11F680. Preserves cached sphere bits. Distinct backed input/resource/
// output, strict finite binary32/FE_TONEAREST. No allocation or scene mutation.
CollectionBoundsStatus48S prepareType0Bounds50EW(const Type0BoundsInput50EW&,
    CollectionBoundsResult48S&) noexcept;
}

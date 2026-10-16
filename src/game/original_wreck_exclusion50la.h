#pragma once
#include "candidate_collection50bx.h"
#include <limits>

namespace rr::game {
enum class OriginalWreckExclusionStatus50LA : std::uint8_t {complete,binding,capacity};
// Source123EB0 over the SAME canonical C0/C4 list. Fixed native backing is
// reserved cold; capacity exhaustion rejects before source append publication.
// Duplicate scan precedes first-hole scan. Count is a high-water extent.
inline OriginalWreckExclusionStatus50LA addOriginalWreckExclusion123EB0(
    CollectionWords50BX& canonical,std::uint32_t* backing,std::size_t capacity,
    std::uint32_t actualTargetIdentity)noexcept {
    using S=OriginalWreckExclusionStatus50LA;
    if(!backing||!actualTargetIdentity||canonical.values!=backing||canonical.capacity!=capacity||
       canonical.count<0||static_cast<std::size_t>(canonical.count)>capacity)return S::binding;
    for(std::int32_t i=0;i<canonical.count;++i)if(backing[i]==actualTargetIdentity)return S::complete;
    for(std::int32_t i=0;i<canonical.count;++i)if(!backing[i]){backing[i]=actualTargetIdentity;return S::complete;}
    if(static_cast<std::size_t>(canonical.count)==capacity||canonical.count==std::numeric_limits<std::int32_t>::max())
        return S::capacity;
    backing[canonical.count]=actualTargetIdentity;++canonical.count;return S::complete;
}
// Source123FA0 clears ONLY the first matching slot. No compaction, C4 decrement,
// or special self-exclusion protection: group detach genuinely visits j==i.
inline OriginalWreckExclusionStatus50LA removeOriginalWreckExclusion123FA0(
    CollectionWords50BX& canonical,std::uint32_t* backing,std::size_t capacity,
    std::uint32_t actualTargetIdentity)noexcept {
    using S=OriginalWreckExclusionStatus50LA;
    if(!backing||!actualTargetIdentity||canonical.values!=backing||canonical.capacity!=capacity||
       canonical.count<0||static_cast<std::size_t>(canonical.count)>capacity)return S::binding;
    for(std::int32_t i=0;i<canonical.count;++i)if(backing[i]==actualTargetIdentity){backing[i]=0;break;}
    return S::complete;
}
} // namespace rr::game

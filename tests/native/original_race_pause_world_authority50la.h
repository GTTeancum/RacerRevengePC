#pragma once
#include <cstdint>
#include <memory>

namespace ra_cold_aggregate50LA { class Owner; }
namespace rr::platform { class RaceAudioGroups50IC; }
namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalRaceConstructor50LA;
// Private synchronous view of the SAME retained world. No authority token,
// copied readiness flag, lifetime extension or world implementation import.
class OriginalRacePauseWorldAuthority50LA final {
    friend class OriginalRaceConstructor50LA;
    static bool current(const ra_cold_aggregate50LA::Owner&,
        const std::shared_ptr<OriginalApplicationEntities50LA>&,
        platform::RaceAudioGroups50IC* const&,const std::uint64_t&)noexcept;
};
}

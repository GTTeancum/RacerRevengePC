#pragma once
#include <cstdint>

namespace rr::platform {
// Explicit currently bound tracks only. This is not the broader original SCB
// metadata catalog and never means that an incomplete dynamic world is complete.
enum class HostTrack50IM : std::uint8_t { TA, GC };
struct HostTrackAdmission50IM {
    bool gcIncompleteWorld = false; // Explicit RR_GC_INCOMPLETE_WORLD=1.
    bool gcMode2 = false;           // Explicit RR_RACE_MODE=2; never auto-selected.
};
inline constexpr bool validHostTrack50IM(HostTrack50IM track) noexcept {
    return track == HostTrack50IM::TA || track == HostTrack50IM::GC;
}
inline constexpr const char* hostTrackCode50IM(HostTrack50IM track) noexcept {
    return track == HostTrack50IM::TA ? "TA" : track == HostTrack50IM::GC ? "GC" : nullptr;
}
inline constexpr bool hostTrackAdmitted50IM(HostTrack50IM track,
    const HostTrackAdmission50IM& admission) noexcept {
    return track == HostTrack50IM::TA ||
        (track == HostTrack50IM::GC && admission.gcIncompleteWorld && admission.gcMode2);
}
}

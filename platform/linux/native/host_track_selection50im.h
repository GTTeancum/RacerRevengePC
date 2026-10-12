#pragma once
#include <cstdint>

namespace rr::platform {
// Explicit currently bound tracks only. This is not the broader original SCB
// metadata catalog and never means that an incomplete dynamic world is complete.
enum class HostTrack50IM : std::uint8_t { TA, GC, GA };
struct HostTrackAdmission50IM {
    bool gcIncompleteWorld = false; // Explicit RR_GC_INCOMPLETE_WORLD=1.
    bool gcMode2 = false;           // Explicit RR_RACE_MODE=2; never auto-selected.
    bool gaOriginalAiCandidate = false; // Explicit one-session fresh all-AI preview only.
};
inline constexpr bool validHostTrack50IM(HostTrack50IM track) noexcept {
    return track == HostTrack50IM::TA || track == HostTrack50IM::GC || track == HostTrack50IM::GA;
}
inline constexpr const char* hostTrackCode50IM(HostTrack50IM track) noexcept {
    return track == HostTrack50IM::TA ? "TA" : track == HostTrack50IM::GC ? "GC" :
        track == HostTrack50IM::GA ? "GA" : nullptr;
}
inline constexpr bool hostTrackAdmitted50IM(HostTrack50IM track,
    const HostTrackAdmission50IM& admission) noexcept {
    return track == HostTrack50IM::TA ||
        (track == HostTrack50IM::GC && admission.gcIncompleteWorld && admission.gcMode2) ||
        (track == HostTrack50IM::GA && admission.gaOriginalAiCandidate && admission.gcMode2);
}
}

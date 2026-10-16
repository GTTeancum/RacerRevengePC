#pragma once
#include <cstdint>

namespace rr::platform {
// Explicit currently bound tracks only. This is not the broader original SCB
// metadata catalog and never means that an incomplete dynamic world is complete.
enum class HostTrack50IM : std::uint8_t { TA, GC, GA, MA, SA, RA };
struct HostTrackAdmission50IM {
    bool gcIncompleteWorld = false; // Explicit acknowledgement for diagnostic/legacy GC paths.
    bool gcMode2 = false;           // Explicit RR_RACE_MODE=2; never auto-selected.
    bool gaOriginalAiCandidate = false; // Explicit original-AI GA admission, including supported cold Course loads.
    // Cold request capability only. The runtime must still construct/check its
    // actual ALL8, camera, trigger, audio, water/effect and GPU owners. This
    // does not imply profile unlocks or completed destination preparation.
    bool gcNativeWorldRequested = false;
    bool raNativeWorldRequested = false; // Explicit bounded native RA cold request; runtime checks remain mandatory.
};
inline constexpr bool validHostTrack50IM(HostTrack50IM track) noexcept {
    return track == HostTrack50IM::TA || track == HostTrack50IM::GC || track == HostTrack50IM::GA ||
        track == HostTrack50IM::MA || track == HostTrack50IM::SA || track == HostTrack50IM::RA;
}
inline constexpr const char* hostTrackCode50IM(HostTrack50IM track) noexcept {
    return track == HostTrack50IM::TA ? "TA" : track == HostTrack50IM::GC ? "GC" :
        track == HostTrack50IM::GA ? "GA" : track == HostTrack50IM::MA ? "MA" :
        track == HostTrack50IM::SA ? "SA" : track == HostTrack50IM::RA ? "RA" : nullptr;
}
inline constexpr bool hostTrackAdmitted50IM(HostTrack50IM track,
    const HostTrackAdmission50IM& admission) noexcept {
    return track == HostTrack50IM::TA ||
        (track == HostTrack50IM::RA && admission.raNativeWorldRequested && admission.gcMode2) ||
        ((track == HostTrack50IM::MA || track == HostTrack50IM::SA) && admission.gcMode2) ||
        (track == HostTrack50IM::GC && (admission.gcNativeWorldRequested || admission.gcIncompleteWorld) && admission.gcMode2) ||
        (track == HostTrack50IM::GA && admission.gaOriginalAiCandidate && admission.gcMode2);
}
}

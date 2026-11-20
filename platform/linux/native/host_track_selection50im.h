#pragma once
#include <array>
#include <cstdint>

namespace rr::platform {
// Explicit currently bound tracks only. This is not the broader original SCB
// metadata catalog and never means that an incomplete dynamic world is complete.
// Course (146r): a course on the ONE common race route whose code is not a stock
// code (an add-on course of the DLC registry). Its code is data, latched once per
// consumed launch in courseRouteCode50IM(); no stock loader is chosen for it.
// PORT EXTENSION, NOT ORIGINAL.
enum class HostTrack50IM : std::uint8_t { TA, GC, GA, MA, SA, RA, MB, SC, TC, RC, TB, GB, MC, Course };
// The one storage of the latched common-route course code (two upper-case
// characters + NUL). Written only by latchCourseRouteCode50IM (bounds-checked:
// exactly two [A-Z0-9] characters, otherwise refused and cleared).
inline std::array<char,3>& courseRouteCodeStorage50IM() noexcept {static std::array<char,3> code{};return code;}
inline bool latchCourseRouteCode50IM(const char* code) noexcept {
    auto& s=courseRouteCodeStorage50IM();s={};
    if(!code||!code[0]||!code[1]||code[2])return false;
    for(unsigned i=0;i<2;++i){
        const char c=code[i]>='a'&&code[i]<='z'?static_cast<char>(code[i]-'a'+'A'):code[i];
        if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9'))){s={};return false;}
        s[i]=c;
    }
    return true;
}
inline const char* courseRouteCode50IM() noexcept {const auto& s=courseRouteCodeStorage50IM();return s[0]?s.data():nullptr;}
struct HostTrackAdmission50IM {
    bool gcIncompleteWorld = false; // Explicit acknowledgement for diagnostic/legacy GC paths.
    bool gcMode2 = false;           // Explicit RR_RACE_MODE=2; never auto-selected.
    bool gaOriginalAiCandidate = false; // Retired: GA is admitted like MA/SA (kept only for positional initialisers).
    // Cold request capability only. The runtime must still construct/check its
    // actual ALL8, camera, trigger, audio, water/effect and GPU owners. This
    // does not imply profile unlocks or completed destination preparation.
    bool gcNativeWorldRequested = false;
    bool raNativeWorldRequested = false; // Normal rooted human Course or explicit legacy ALL8 request; runtime ownership checks remain mandatory.
    bool mbNativeWorldRequested = false; // Shared native cold request capability; original profile unlocks and actual owners are independent.
    bool scNativeWorldRequested = false; // Shared native cold request capability; original profile unlocks and actual owners are independent.
    bool tcNativeWorldRequested = false; // Shared native cold request capability; original profile unlocks and actual owners are independent.
    bool rcNativeWorldRequested = false; // Shared native cold request capability; original profile unlocks and actual owners are independent.
    bool tbNativeWorldRequested = false; // Shared native cold request capability; original profile unlocks and actual owners are independent.
    bool gbNativeWorldRequested = false; // Shared native cold request capability; original profile unlocks and actual owners are independent.
    bool mcNativeWorldRequested = false; // Shared native cold request capability; original profile unlocks and actual owners are independent.
};
inline bool validHostTrack50IM(HostTrack50IM track) noexcept {
    return track == HostTrack50IM::TA || track == HostTrack50IM::GC || track == HostTrack50IM::GA ||
        track == HostTrack50IM::MA || track == HostTrack50IM::SA || track == HostTrack50IM::RA || track == HostTrack50IM::MB || track == HostTrack50IM::SC || track == HostTrack50IM::TC || track == HostTrack50IM::RC || track == HostTrack50IM::TB || track == HostTrack50IM::GB || track == HostTrack50IM::MC ||
        (track == HostTrack50IM::Course && courseRouteCode50IM() != nullptr);
}
inline const char* hostTrackCode50IM(HostTrack50IM track) noexcept {
    if (track == HostTrack50IM::Course) return courseRouteCode50IM();
    return track == HostTrack50IM::TA ? "TA" : track == HostTrack50IM::GC ? "GC" :
        track == HostTrack50IM::GA ? "GA" : track == HostTrack50IM::MA ? "MA" :
        track == HostTrack50IM::SA ? "SA" : track == HostTrack50IM::RA ? "RA" : track == HostTrack50IM::MB ? "MB" : track == HostTrack50IM::SC ? "SC" : track == HostTrack50IM::TC ? "TC" : track == HostTrack50IM::RC ? "RC" : track == HostTrack50IM::TB ? "TB" : track == HostTrack50IM::GB ? "GB" : track == HostTrack50IM::MC ? "MC" : nullptr;
}
inline bool hostTrackAdmitted50IM(HostTrack50IM track,
    const HostTrackAdmission50IM& admission) noexcept {
    return track == HostTrack50IM::TA ||
        // The common route takes the MA/SA/GA admission rule (mode2, Player0 + 7 AI checked at the session).
        (track == HostTrack50IM::Course && courseRouteCode50IM() != nullptr && admission.gcMode2) ||
        (track == HostTrack50IM::MC && admission.mcNativeWorldRequested && admission.gcMode2) ||
        (track == HostTrack50IM::GB && admission.gbNativeWorldRequested && admission.gcMode2) ||
        (track == HostTrack50IM::TB && admission.tbNativeWorldRequested && admission.gcMode2) ||
        (track == HostTrack50IM::RC && admission.rcNativeWorldRequested && admission.gcMode2) ||
        (track == HostTrack50IM::TC && admission.tcNativeWorldRequested && admission.gcMode2) ||
        (track == HostTrack50IM::SC && admission.scNativeWorldRequested && admission.gcMode2) ||
        (track == HostTrack50IM::MB && admission.mbNativeWorldRequested && admission.gcMode2) ||
        (track == HostTrack50IM::RA && admission.raNativeWorldRequested && admission.gcMode2) ||
        ((track == HostTrack50IM::MA || track == HostTrack50IM::SA || track == HostTrack50IM::GA) && admission.gcMode2) ||
        (track == HostTrack50IM::GC && (admission.gcNativeWorldRequested || admission.gcIncompleteWorld) && admission.gcMode2);
}
}

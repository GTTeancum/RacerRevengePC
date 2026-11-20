#pragma once
#include <array>
#include <cstdint>

namespace rr::platform {
// Host menu/harness selection key only (W0-HANDOFF 3.1). Race-load and race-frame
// code read the course code and source from common_track50CR::Track, never this
// enum. This is not the broader original SCB metadata catalog.
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
// Common rewrite (I1, B-C02/B-C03): ONE admission for every course. The former
// per-course request flags (gc/ra/mb/sc/tc/rc/tb/gb/mc NativeWorldRequested), the
// GA candidate flag and the GC incomplete-world acknowledgement are deleted.
struct HostTrackAdmission50IM {
    // RETIRED (B-C03): its environment switch is deleted; this is always false. The
    // field stays only because host_session_menu50if.h (unassigned) and the I3
    // publication receipt still read it; their owners drop the reads.
    bool gcIncompleteWorld = false;
    // RR_RACE_MODE=2 (explicit, never auto-selected), for every course. The name is
    // kept for the existing readers (host_session_menu50if.h, I3 receipt).
    bool gcMode2 = false;
    // Cold request capability only, the same for every course: original-disc Main,
    // rooted Course entry, mode2, no host start menu. The runtime still constructs
    // and checks every actual owner; this implies no unlock and no readiness.
    bool nativeWorldRequested = false;
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
// One rule for every course (B-C02): a supported host selection in mode2. The
// original has no admission list (17C16C / 17D678 -> 1A65C0 for any course).
inline bool hostTrackAdmitted50IM(HostTrack50IM track,
    const HostTrackAdmission50IM& admission) noexcept {
    return validHostTrack50IM(track) && admission.gcMode2;
}
// Host selection key of a course code (file-name data, e.g. RR_TRACK): the stock
// enumerator whose code it is, or Course with the code latched for a registry
// course. Refused (false, nothing changed) for an empty or malformed code.
inline bool hostTrackForCode50IM(const char* code, bool registryCourse, HostTrack50IM& out) noexcept {
    if (!code || !code[0] || !code[1] || code[2]) return false;
    const auto upper = [](char c) noexcept { return c >= 'a' && c <= 'z' ? static_cast<char>(c - 'a' + 'A') : c; };
    if (!registryCourse) {
        for (unsigned i = 0; i < static_cast<unsigned>(HostTrack50IM::Course); ++i) {
            const auto track = static_cast<HostTrack50IM>(i);
            const char* own = hostTrackCode50IM(track);
            if (own && own[0] == upper(code[0]) && own[1] == upper(code[1])) { out = track; return true; }
        }
        return false;
    }
    if (!latchCourseRouteCode50IM(code)) return false;
    out = HostTrack50IM::Course;
    return true;
}
}

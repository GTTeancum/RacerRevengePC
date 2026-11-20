#pragma once
#include "track_source_fixture.h"
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace track_visual_profile_fixture {
struct Profile {
    const char* container;
    const char* selectionSha;
    const char* exportSha;
    const char* archiveSha;
    std::uint32_t vertices, indices, scenes, materials, instances, surfaces, drawSpans;
    std::uint32_t maxWorldInstance, archiveEntries;
    std::size_t archiveBytes, archiveDecodedBytes;
};
inline const Profile& get(track_source_fixture::Track track) {
    static constexpr Profile ta{
        "TRACKS/TA.RES",
        "12fad4579276c9fce30abd3009420a8529a033e1d7271ae6b0688927f8adf674",
        "ef2bdffbb36f5669c0d84eb9b31e5a3f8f4853b5ab966898bb768efd42eb47c4",
        "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8",
        110196,209514,228,29,284,353,353,317,383,3334562,16u*1024u*1024u};
    static constexpr Profile gc{
#include "gc_visual_profile.inc"
    };
#if __has_include("ga_visual_profile.inc")
    // Produced only by authenticated original GA cold preparation. Missing
    // generated pins are an explicit unavailable asset profile, never TA/GC.
    static constexpr Profile ga{
#include "ga_visual_profile.inc"
    };
#endif
#if __has_include("ma_visual_profile.inc")
    static constexpr Profile ma{
#include "ma_visual_profile.inc"
    };
#endif
#if __has_include("sa_visual_profile.inc")
    static constexpr Profile sa{
#include "sa_visual_profile.inc"
    };
#endif
    switch (track) {
    case track_source_fixture::Track::ta: return ta;
    case track_source_fixture::Track::gc: return gc;
    case track_source_fixture::Track::ga:
#if __has_include("ga_visual_profile.inc")
        return ga;
#else
        throw std::runtime_error("GA native visual assets have not been cold-prepared and pinned");
#endif
    case track_source_fixture::Track::ma:
#if __has_include("ma_visual_profile.inc")
        return ma;
#else
        throw std::runtime_error("MA native visual assets have not been cold-prepared and pinned");
#endif
    case track_source_fixture::Track::sa:
#if __has_include("sa_visual_profile.inc")
        return sa;
#else
        throw std::runtime_error("SA native visual assets have not been cold-prepared and pinned");
#endif
    // A registry course on the common race route names no stock profile: these
    // are stock pins, so it gets what a stock code without a pinned row gets.
    // Its world is loaded by the common-route owner from its own data instead.
    case track_source_fixture::Track::course:
        throw std::runtime_error("Course has no stock visual profile (its world loads from its own data)");
    default: throw std::runtime_error("Unsupported original visual track identity");
    }
}
}

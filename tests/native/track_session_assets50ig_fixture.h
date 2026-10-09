#pragma once
#include <cstddef>
#include <cstring>

// Cold session selection. Metadata availability is deliberately distinct from
// runnable admission: RRWORLD retains original SCB, not PSG/COL/SPL/sky payloads.
// No new track is admitted by changing an environment variable or finding a
// similarly named TA resource. The existing TA visual/normal/sky authenticators
// remain authoritative until a separately reviewed native binding replaces them.
namespace track_session_assets50IG {
struct WorldMetadata {
    const char* code;
    const char* container;
    const char* worldMember;
    std::size_t zones, portals, placements;
};
inline constexpr WorldMetadata worlds[] = {
    {"TA","TRACKS/TA.RES","tracks\\ta.scb",9,10,318},
    {"GA","TRACKS/GA.RES","tracks\\ga.scb",12,18,753},
    {"GB","TRACKS/GB.RES","tracks\\gb.scb",13,17,975},
    {"GC","TRACKS/GC.RES","tracks\\gc.scb",12,13,999},
    {"MA","TRACKS/MA.RES","tracks\\ma.scb",9,10,1051},
    {"MB","TRACKS/MB.RES","tracks\\mb.scb",16,17,1018},
    {"RA","TRACKS/RA.RES","tracks\\ra.scb",10,10,1261},
    {"RC","TRACKS/RC.RES","tracks\\rc.scb",15,15,1277},
    {"SA","TRACKS/SA.RES","tracks\\sa.scb",25,25,1869},
    {"SC","TRACKS/SC.RES","tracks\\sc.scb",30,34,4107},
    {"TB","TRACKS/TB.RES","tracks\\tb.scb",10,11,1112},
    {"TC","TRACKS/TC.RES","tracks\\tc.scb",25,26,600}
};
struct SessionAssets {
    const WorldMetadata* world;
    const char* archive;
    const char* mapSpline;
    const char* aiSpline;
    const char* impactTuning;
    const char* mainCollisionCase;
    const char* secondaryCollisionCase;
    const char* visualRoot;
    const char* visualMesh;
    const char* visualReport;
    const char* skyTextureRoot;
    unsigned gridRank, initialCollisionZone;
    std::size_t terrainOwners;
};
inline constexpr SessionAssets retainedTA{
    &worlds[0],
    "work/native/project/disc/TRACKS/TA.RES",
    "tracks\\mta.spl", "tracks\\ata.spl",
    "work/impactAuthoring49Y/ta.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "evidence/visual49G",
    "evidence/visual49G/export-placed-normal/ta-world.rrmesh",
    "evidence/visual49G/export-placed-normal/export-report.json",
    "evidence/sky50GW",4,1,37
};
inline bool codeEquals(const char* requested,const char* code) noexcept {
    if(!requested||!requested[0]||!requested[1]||requested[2])return false;
    for(unsigned i=0;i<2;++i){
        const auto c=requested[i];
        const auto upper=c>='a'&&c<='z'?static_cast<char>(c-'a'+'A'):c;
        if(upper!=code[i])return false;
    }
    return true;
}
inline const WorldMetadata* findWorld(const char* code) noexcept {
    if(!code)code="TA";
    for(const auto& world:worlds)if(codeEquals(code,world.code))return &world;
    return nullptr;
}
// Read-only menu interface: an unavailable row must not launch or display TA
// under another track name. Do not treat findWorld() as playable admission.
inline const SessionAssets* sessionAssets(const WorldMetadata& world) noexcept {
    return codeEquals(world.code,"TA")?&retainedTA:nullptr;
}
inline const SessionAssets* select(const char* code,const char*& error) noexcept {
    const auto* world=findWorld(code);
    if(!world){error="RR_TRACK must identify a retained original track (TA, GA, GB, GC, MA, MB, RA, RC, SA, SC, TB, TC)";return nullptr;}
    const auto* assets=sessionAssets(*world);
    if(!assets){error="Requested original track is unavailable: original archive, collision, M/A splines, textured visuals, sky, and native track binding must be prepared; retained SCB metadata is insufficient";return nullptr;}
    error=nullptr;return assets;
}
} // namespace track_session_assets50IG

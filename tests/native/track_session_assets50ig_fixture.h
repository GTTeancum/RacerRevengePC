#pragma once
#include <cstddef>
#include <cstring>

// Cold session selection. Metadata availability is deliberately distinct from
// runnable admission: RRWORLD retains original SCB, not PSG/COL/SPL/sky payloads.
// Selection binds only original resources; each cold provider authenticates
// them independently. GC uses its own source profile and bounded native joins.
// Source binding is not validation or admission of unjoined dynamic owners.
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
    const char* archiveSha256;
    std::size_t archiveBytes;
    bool originalStaticCandidates, tuningInArchive, nativeMode1, materialAudio;

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
    "evidence/sky50GW",4,1,37,
    "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8",3334562,
    false,false,true,true
};
// These are actual transferred original source paths/counts. Selecting GC
// starts strict cold admission in main; it is NOT a gameplay-validation claim.
// Source decoder/material/normal/sky owners must all succeed. No silent TA or
// untextured fallback exists. Native record configuration remains unjoined;
// explicit mode2 is required. Actual12-row GC material audio is separately bound.
inline constexpr SessionAssets retainedGC{
    &worlds[3],"work/native/project/disc/TRACKS/GC.RES",
    "tracks\\mgc.spl","tracks\\agc.spl","tracks\\gc.tun",
    // Only original POD01 query resources/explicit query inputs are read from
    // these corpus rows. No corpus TA terrain geometry or candidates are used.
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "evidence/visualGC","evidence/visualGC/export-placed-normal/gc-world.rrmesh",
    "evidence/visualGC/export-placed-normal/export-report.json","evidence/skyGC",4,1,551,
    "69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",4205067,
    true,true,false,true
};
// Authenticated Watchtower source binding. Runtime admission separately requires
// explicit fresh all-eight-original-AI preview admission. A static metadata row
// or prepared texture cache alone cannot authorize gameplay or a menu launch.
inline constexpr SessionAssets retainedGA{
    &worlds[1],"work/native/project/disc/TRACKS/GA.RES",
    "tracks\\mga.spl","tracks\\aga.spl","tracks\\ga.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "evidence/visualGA","evidence/visualGA/export-placed-normal/ga-world.rrmesh",
    "evidence/visualGA/export-placed-normal/export-report.json","evidence/skyGA",4,1,450,
    "063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9",2834942,
    true,true,false,true
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
// Read-only source-binding lookup, not successful cold/runtime admission.
// An unbound row must never launch or display TA under another track name.
inline const SessionAssets* sessionAssets(const WorldMetadata& world) noexcept {
    return codeEquals(world.code,"TA")?&retainedTA:
        codeEquals(world.code,"GC")?&retainedGC:
        codeEquals(world.code,"GA")?&retainedGA:nullptr;
}
inline const SessionAssets* select(const char* code,const char*& error) noexcept {
    const auto* world=findWorld(code);
    if(!world){error="RR_TRACK must identify a retained original track (TA, GA, GB, GC, MA, MB, RA, RC, SA, SC, TB, TC)";return nullptr;}
    const auto* assets=sessionAssets(*world);
    if(!assets){error="Requested original track is unavailable: original archive, collision, M/A splines, textured visuals, sky, and native track binding must be prepared; retained SCB metadata is insufficient";return nullptr;}
    error=nullptr;return assets;
}
} // namespace track_session_assets50IG

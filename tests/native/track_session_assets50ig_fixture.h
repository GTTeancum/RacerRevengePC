#pragma once
#include "runtime_asset_paths50lb.h"
#include <cstddef>
#include <cstring>
#include <array>
#include <string>

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
// Own each resolved string for exactly the cold/live session that borrows it.
// Archive member names remain unchanged; only actual filesystem inputs move.
class ResolvedSessionAssets final {
    std::array<std::string,6> paths_;
public:
    SessionAssets assets;
    explicit ResolvedSessionAssets(const SessionAssets& source):assets(source) {
        const char* inputs[]={source.archive,source.visualRoot,source.visualMesh,
            source.visualReport,source.skyTextureRoot,source.impactTuning};
        for(std::size_t i=0;i<paths_.size();++i)
            paths_[i]=(i==5&&source.tuningInArchive)?inputs[i]:
                runtime_asset_paths50LB::resolve(inputs[i]).string();
        assets.archive=paths_[0].c_str();assets.visualRoot=paths_[1].c_str();
        assets.visualMesh=paths_[2].c_str();assets.visualReport=paths_[3].c_str();
        assets.skyTextureRoot=paths_[4].c_str();assets.impactTuning=paths_[5].c_str();
    }
    ResolvedSessionAssets(const ResolvedSessionAssets&)=delete;
    ResolvedSessionAssets& operator=(const ResolvedSessionAssets&)=delete;
    ResolvedSessionAssets(ResolvedSessionAssets&&)=delete;
    ResolvedSessionAssets& operator=(ResolvedSessionAssets&&)=delete;
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
// The remaining original unlocked destinations. Every cold provider still
// authenticates its own source; a metadata row never substitutes missing data.
inline constexpr SessionAssets retainedMA{
    &worlds[4],"work/native/project/disc/TRACKS/MA.RES",
    "tracks\\mma.spl","tracks\\ama.spl","tracks\\ma.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "evidence/visualMA","evidence/visualMA/export-placed-normal/ma-world.rrmesh",
    "evidence/visualMA/export-placed-normal/export-report.json","evidence/skyMA",4,1,106,
    "2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228",3691592,
    true,true,false,true
};
inline constexpr SessionAssets retainedSA{
    &worlds[8],"work/native/project/disc/TRACKS/SA.RES",
    "tracks\\msa.spl","tracks\\asa.spl","tracks\\sa.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "evidence/visualSA","evidence/visualSA/export-placed-normal/sa-world.rrmesh",
    "evidence/visualSA/export-placed-normal/export-report.json","evidence/skySA",4,1,155,
    "34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a",3786696,
    true,true,false,true
};
// Native RA source binding only. Empty legacy mesh/report/sky paths are
// intentional: the retained aggregate owns original PSG/material/sky sources.
// This row requests the explicit same-owner native RA cold route. Its
// constructor, query, renderer and camera checks must still all succeed. In particular,
// a metadata row must never send Ryloth through the legacy TA visual defaults.
inline constexpr SessionAssets retainedRA{
    &worlds[6],"work/native/project/disc/TRACKS/RA.RES",
    "tracks\\mra.spl","tracks\\ara.spl","tracks\\ra.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "","","","",4,1,178,
    "1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2",4604561,
    true,true,false,true
};
// Authenticated MB source binding, not a completed native-course admission.
// Fresh2169C0 placement receives the actual first application table entries:
// 1AE578/57C -> 1AE588/58C -> 1AE590; C34 store at216A2C.
// Ordered native table reference1 is therefore not copied from camera defaults.
// Runtime must bind those actual MB tables/world and construct every owner.
// Empty legacy exports are intentional; original MB source owners supply data.
inline constexpr SessionAssets retainedMB{
    &worlds[5],"work/native/project/disc/TRACKS/MB.RES",
    "tracks\\mmb.spl","tracks\\amb.spl","tracks\\mb.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "","","","",4,1,128,
    "6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca",4421779,
    true,true,false,true
};
// Exact SC original resources. This selects a cold attempt, not readiness.
// The existing fresh-session grid rank remains4. Initial collision reference1
// is the first actual Application table entry, as in generic1AE578/216A2C.
// SC preparation must retain that ordered world; no camera zone is substituted.
//252 is the authenticated per-placement COL match count, not a visual count.
inline constexpr SessionAssets retainedSC{
    &worlds[9],"work/native/project/disc/TRACKS/SC.RES",
    "tracks\\msc.spl","tracks\\asc.spl","tracks\\sc.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "","","","",4,1,252,
    "8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d",2974353,
    true,true,false,true
};
// Authenticated TC metadata selects a cold attempt, never completed readiness.
// Rank4 preserves the existing native human assignment. Initial ref1 denotes
// actual Application table entry0; the root must retain that ordered table.
// 69 is the own per-placement COL membership count, not600 model placements.
inline constexpr SessionAssets retainedTC{
    &worlds[11],"work/native/project/disc/TRACKS/TC.RES",
    "tracks\\mtc.spl","tracks\\atc.spl","tracks\\tc.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "","","","",4,1,69,
    "7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490",3607868,
    true,true,false,true
};
// Authenticated RC source binding selects a cold attempt, not readiness.
// Existing native human rank4 and actual first Application table ref1 remain;
// the genuine RC aggregate must construct those same ordered tables.
//112 is own per-placement COL membership, not total visual placements.
inline constexpr SessionAssets retainedRC{
    &worlds[7],"work/native/project/disc/TRACKS/RC.RES",
    "tracks\\mrc.spl","tracks\\arc.spl","tracks\\rc.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "","","","",4,1,112,
    "ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b",4655141,
    true,true,false,true
};
// Exact original TB archive metadata selects a cold attempt, not readiness.
//320 counts all placed COL memberships, including four visual+COL placements.
//The independently owned animated door is separate from this static set.
inline constexpr SessionAssets retainedTB{
    &worlds[10],"work/native/project/disc/TRACKS/TB.RES",
    "tracks\\mtb.spl","tracks\\atb.spl","tracks\\tb.tun",
    "podboxm.col-explicit37-authored-placements-forward",
    "chariotm.col-explicit37-authored-placements-forward",
    "","","","",4,1,320,
    "1188206d7b1f64910c119192829fc357b9d4a9f021d64e8a2b7561f2c113a09b",3749557,
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
        codeEquals(world.code,"GA")?&retainedGA:
        codeEquals(world.code,"MA")?&retainedMA:
        codeEquals(world.code,"SA")?&retainedSA:
        codeEquals(world.code,"RA")?&retainedRA:
        codeEquals(world.code,"MB")?&retainedMB:
        codeEquals(world.code,"SC")?&retainedSC:
        codeEquals(world.code,"TC")?&retainedTC:
        codeEquals(world.code,"RC")?&retainedRC:
        codeEquals(world.code,"TB")?&retainedTB:nullptr;
}
inline const SessionAssets* select(const char* code,const char*& error) noexcept {
    const auto* world=findWorld(code);
    if(!world){error="RR_TRACK must identify a retained original track (TA, GA, GB, GC, MA, MB, RA, RC, SA, SC, TB, TC)";return nullptr;}
    const auto* assets=sessionAssets(*world);
    if(!assets){error="Requested original track is unavailable: original archive, collision, M/A splines, textured visuals, sky, and native track binding must be prepared; retained SCB metadata is insufficient";return nullptr;}
    error=nullptr;return assets;
}
} // namespace track_session_assets50IG

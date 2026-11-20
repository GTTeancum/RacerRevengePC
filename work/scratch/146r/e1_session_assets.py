from edlib import edit
edit('tests/native/track_session_assets50ig_fixture.h', [
('''#include "runtime_asset_paths50lb.h"
''', '''#include "runtime_asset_paths50lb.h"
#include "../../src/assets/dlc_registry50dl.h"
#include "../../src/game/dlc_course_entries50dl.h"
'''),
('''    bool originalStaticCandidates, tuningInArchive, nativeMode1, materialAudio;

};''', '''    bool originalStaticCandidates, tuningInArchive, nativeMode1, materialAudio;
    // 146r: a row built from the DLC registry for a non-stock course code (the
    // ONE common race route). Its archive is the package file (dlc:/ path), its
    // SHA/bytes the listed ones (the overlay checks the actual file against the
    // listing), member names are formed from the code, counts come from the data
    // (terrainOwners 0 = take the live count). PORT EXTENSION, NOT ORIGINAL.
    bool registry=false;
    // Archive that holds impactTuning when it is not the session archive
    // (inherit.tuning: the named stock course's TRACKS/<CODE>.RES); nullptr = session archive.
    const char* tuningArchive=nullptr;
};'''),
('''    std::array<std::string,6> paths_;
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
    }''', '''    std::array<std::string,7> paths_;
public:
    SessionAssets assets;
    explicit ResolvedSessionAssets(const SessionAssets& source):assets(source) {
        const char* inputs[]={source.archive,source.visualRoot,source.visualMesh,
            source.visualReport,source.skyTextureRoot,source.impactTuning,source.tuningArchive?source.tuningArchive:""};
        for(std::size_t i=0;i<paths_.size();++i)
            paths_[i]=(i==5&&source.tuningInArchive)?inputs[i]:
                runtime_asset_paths50LB::resolve(inputs[i]).string();
        assets.archive=paths_[0].c_str();assets.visualRoot=paths_[1].c_str();
        assets.visualMesh=paths_[2].c_str();assets.visualReport=paths_[3].c_str();
        assets.skyTextureRoot=paths_[4].c_str();assets.impactTuning=paths_[5].c_str();
        assets.tuningArchive=source.tuningArchive?paths_[6].c_str():nullptr;
    }'''),
('''inline const SessionAssets* select(const char* code,const char*& error) noexcept {
    const auto* world=findWorld(code);
    if(!world){error="RR_TRACK''', '''// ---- Common-route row for a registry course (146r) ----------------------------
// The ONE storage of the registry session row: rebuilt by registrySession() for
// the code asked (the latched launch code), never written anywhere else. The
// strings own every const char* the SessionAssets/WorldMetadata rows point at.
struct RegistrySessionStorage {
    std::string code,container,worldMember,archive,mapSpline,aiSpline,tuning,tuningArchive,sha;
    WorldMetadata world{};
    SessionAssets assets{};
};
inline RegistrySessionStorage& registrySessionStorage() noexcept {static RegistrySessionStorage s;return s;}
// Row of an admitted add-on course code ("NV"/"nv"); nullptr for any other code.
inline const SessionAssets* registrySession(const char* code,const char*& error) noexcept {
    try{
        rr::game::dlc50DL::AddonCourseLoad load;
        const auto* course=code&&code[0]&&code[1]&&!code[2]?rr::assets::dlc50DL::addonCourseByCode(std::string(code,2)):nullptr;
        if(!course||!rr::game::dlc50DL::addonCourseLoad(course->number,load)){
            error="course code is neither a stock course nor an admitted add-on course";return nullptr;
        }
        auto& s=registrySessionStorage();s=RegistrySessionStorage{};
        s.code=load.codeUpper;s.container="TRACKS/"+load.codeUpper+".RES";s.worldMember=load.scb;
        s.archive=load.archive;s.mapSpline=load.mapSpline;s.aiSpline=load.aiSpline;
        s.tuning=load.tuningMember;s.sha=course->archiveSha256;
        // inherit.tuning: the member is read from the stock course's own archive.
        if(load.tuningInherited)s.tuningArchive="work/native/project/disc/"+load.tuningArchive;
        s.world=WorldMetadata{s.code.c_str(),s.container.c_str(),s.worldMember.c_str(),0,0,0}; // counts from the data
        s.assets=SessionAssets{&s.world,s.archive.c_str(),s.mapSpline.c_str(),s.aiSpline.c_str(),s.tuning.c_str(),
            "podboxm.col-explicit37-authored-placements-forward",
            "chariotm.col-explicit37-authored-placements-forward",
            "","","","",4,1,0,s.sha.c_str(),static_cast<std::size_t>(course->archiveBytes),
            true,true,false,true};
        s.assets.registry=true;s.assets.tuningArchive=s.tuningArchive.empty()?nullptr:s.tuningArchive.c_str();
        error=nullptr;return &s.assets;
    }catch(...){error="registry course row could not be built";return nullptr;}
}
inline const SessionAssets* select(const char* code,const char*& error) noexcept {
    const auto* world=findWorld(code);
    if(!world&&code&&rr::assets::dlc50DL::addonCourseCount())return registrySession(code,error);
    if(!world){error="RR_TRACK'''),
])

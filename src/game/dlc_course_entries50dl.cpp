#include "dlc_course_entries50dl.h"
#include "dlc_course_records50dl.h"
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR: see dlc_course_entries50dl.h.
#include "../assets/dlc_registry50dl.h"
#include "original_course_descriptor50kg.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <utility>

namespace rr::game::dlc50DL {
namespace reg=rr::assets::dlc50DL;
namespace {
std::atomic<unsigned> selected{0}; // the one port-owned add-on course selection (0 = stock)
std::once_flag testOnce,announceOnce;
std::string upperCode(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=char(c-'a'+'A');return s;}
std::string lowerCode(std::string s){for(auto& c:s)if(c>='A'&&c<='Z')c=char(c-'A'+'a');return s;}
// Descriptor index of a stock code ("tb" -> 1), the order CourseIndex uses.
int stockCourseIndex(const std::string& code){
    const auto u=upperCode(code);
    if(u.size()!=2)return -1;
    const auto& d=rr::game::originalFreshCourseDescriptors50KG;
    for(std::size_t i=0;i<d.size();++i)if(d[i].first==u[0]&&d[i].second==u[1])return static_cast<int>(i);
    return -1;
}
std::string quoted(std::string s){for(auto& c:s)if(c=='"')c='\'';return "\""+s+"\"";}
}

std::size_t addonCourseCount() noexcept {return reg::addonCourseCount();}
const reg::AddonCourse* addonCourseAt(std::size_t index) noexcept {return reg::addonCourseAt(index);}
const reg::AddonCourse* addonCourse(unsigned number) noexcept {return reg::addonCourse(number);}

bool planAppendedCourseControls(const std::vector<AuthoredCourseControl>& authored,std::size_t authoredControlCount,
                                std::size_t controlLimit,std::vector<AppendedCourseControl>& out,std::string& why){
    out.clear();why.clear();
    const std::size_t count=reg::addonCourseCount();
    if(!count)return true;                                 // stock: nothing appended
    if(authoredControlCount>controlLimit||count>controlLimit-authoredControlCount){
        why="add-on courses do not fit the page control capacity ("+std::to_string(controlLimit)+")";return false;
    }
    std::vector<AppendedCourseControl> next;next.reserve(count);
    for(std::size_t i=0;i<count;++i){
        const auto* c=reg::addonCourseAt(i);
        if(!c){why="registry row missing";return false;}
        const int index=stockCourseIndex(c->courseScreenTemplate);
        int from=-1;
        for(const auto& a:authored)                         // the template's control that carries widgets
            if(index>=0&&a.courseIndex==unsigned(index)&&a.widgets){from=a.controlId;break;}
        if(from<0){why="course "+c->code+": stock template "+c->courseScreenTemplate+" has no authored course control";return false;}
        AppendedCourseControl a;
        a.controlId=static_cast<int>(authoredControlCount+i);a.layoutFrom=from;a.number=c->number;
        a.key=c->key;a.name=c->name;a.planetText=c->planetName;a.templateCode=c->courseScreenTemplate;a.preview=c->coursePreview;
        a.backgroundFile=c->backgroundFile;a.backgroundStock=c->backgroundStockCode;
        next.push_back(std::move(a));
    }
    out=std::move(next);return true;
}

bool selectAddonCourse(unsigned number) noexcept {
    if(!reg::addonCourse(number))return false; // bounds: admitted add-on numbers only
    selected.store(number);return true;
}
void clearAddonCourse() noexcept {selected.store(0);}
unsigned selectedAddonCourse() noexcept {
    const unsigned number=selected.load();
    return reg::addonCourse(number)?number:0u;
}
std::string courseKey(unsigned number){
    reg::CourseRow row;
    if(!reg::courseRow(number,row))return std::string();
    return row.addon?row.row->key:"stock:"+row.code;
}
bool addonCourseAllowsMode(unsigned number,AddonCourseMode mode) noexcept {
    const auto* c=reg::addonCourse(number);
    return c&&(mode==AddonCourseMode::single?c->single:mode==AddonCourseMode::timeTrial?c->timeTrial:c->vs);
}
unsigned addonCourseForLaunch(unsigned mode34,unsigned subtype) noexcept {
    const unsigned number=selectedAddonCourse();
    if(mode34==1)return subtype==1&&addonCourseAllowsMode(number,AddonCourseMode::vs)?number:0u;
    if(!number||mode34!=0)return 0;
    const auto mode=subtype==3?AddonCourseMode::timeTrial:AddonCourseMode::single;
    return (subtype>=1&&subtype<=3&&addonCourseAllowsMode(number,mode))?number:0u;
}
void applyTestAddonCourse() noexcept {
    try{std::call_once(testOnce,[]{
        const char* key=std::getenv("RR_TEST_ADDON_COURSE");
        if(!key||!*key)return;
        for(std::size_t i=0;i<reg::addonCourseCount();++i){
            const auto* c=reg::addonCourseAt(i);
            if(c->key==key&&selectAddonCourse(c->number)){
                const auto route=addonCourseRoute();
                std::fprintf(stderr,"dlc_course_test key=%s number=%u code=%s status=selected route=%s%s%s%s\n",key,c->number,c->code.c_str(),
                    route.name,route.available?"":" launch=refused reason=\"",route.available?"":route.reason,route.available?"":"\"");
                return;
            }
        }
        std::fprintf(stderr,"dlc_course_test key=%s status=refused reason=\"not an admitted add-on course\"\n",key);
    });}catch(...){}
}

bool addonCourseLoad(unsigned number,AddonCourseLoad& out){
    out=AddonCourseLoad{};
    const auto* c=reg::addonCourse(number);
    if(!c)return false;
    AddonCourseLoad l;
    l.number=c->number;l.key=c->key;l.code=lowerCode(c->code);l.codeUpper=upperCode(c->code);l.name=c->name;l.planetName=c->planetName;
    l.archive=c->archive;
    // Members the original names by course code, with the add-on code (DLC-JSON.md).
    const std::string t="tracks\\";
    l.scb=t+l.code+".scb";l.tob=t+l.code+".tob";l.aub=t+l.code+".aub";
    l.mapSpline=t+"m"+l.code+".spl";l.aiSpline=t+"a"+l.code+".spl";
    l.cub=c->cube?t+l.code+".cub":std::string();
    l.tvCamera=c->tvCamera?t+l.code+"TVCam.pos":std::string();
    for(unsigned n=1;n<=c->cutCameras;++n){
        const char nn[3]={char('0'+n/10),char('0'+n%10),0};
        l.cutCameras.push_back(t+l.code+nn+".vub");
    }
    // inherit.tuning / inherit.particles: members in the archive always win.
    l.tuningInherited=!c->ownTuning;
    l.tuningArchive=l.tuningInherited?"TRACKS/"+upperCode(c->inheritTuning)+".RES":c->archive;
    l.tuningMember=t+(l.tuningInherited?lowerCode(c->inheritTuning):l.code)+".tun";
    l.particlesInherited=!c->ownParticles;
    l.particleArchive=l.particlesInherited?"TRACKS/"+upperCode(c->inheritParticles)+".RES":c->archive;
    l.introEvents=c->introEvents;l.scriptLines=c->scriptLines;
    l.loadingFromPackage=!c->loadingStock;
    l.loadingScreen=l.loadingFromPackage?c->loadingFile:"UI/TEXTURES/LD_"+upperCode(c->loadingStockCode)+".PST";
    l.loadingSha256=c->loadingSha256;
    l.phyClass=c->phyClass;l.aiClass=c->aiClass;l.rivalRacer=c->rivalRacer;
    for(const auto& [on,name]:{std::pair<bool,const char*>{c->single,"single"},{c->timeTrial,"time_trial"},{c->vs,"vs"}})
        if(on)l.modes+=(l.modes.empty()?"":",")+std::string(name);
    l.background=c->backgroundFile.empty()?"stock:"+c->backgroundStockCode:c->backgroundFile;
    l.route=addonCourseRoute().name;
    out=std::move(l);return true;
}
bool addonCourseLoadByCode(const char* code,AddonCourseLoad& out){
    out=AddonCourseLoad{};
    try{
        if(!code||!code[0]||!code[1]||code[2])return false;
        const auto* c=reg::addonCourseByCode(lowerCode(std::string(code,2)));
        return c&&addonCourseLoad(c->number,out);
    }catch(...){return false;}
}
std::string addonCourseLoadLine(const AddonCourseLoad& l){
    std::string cuts=std::to_string(l.cutCameras.size());
    return "dlc_course key="+l.key+" number="+std::to_string(l.number)+" code="+l.code+
        " archive="+l.archive+" scb="+l.scb+" tob="+l.tob+" aub="+l.aub+" map="+l.mapSpline+" ai_spline="+l.aiSpline+
        " cub="+(l.cub.empty()?std::string("absent"):l.cub)+" cut_cameras="+cuts+" tv_camera="+(l.tvCamera.empty()?"absent":"1")+
        " tuning="+(l.tuningInherited?"inherited:"+l.tuningArchive+"#"+l.tuningMember:std::string("own"))+
        " particles="+(l.particlesInherited?"inherited:"+l.particleArchive:std::string("own"))+
        " intro_events="+std::to_string(l.introEvents)+" script_lines="+std::to_string(l.scriptLines)+
        (l.introEvents==0?" intro=accepted_no_lines":" intro=lines")+
        " loading="+(l.loadingFromPackage?"file:"+l.loadingScreen+" loading_receipt="+(l.loadingSha256.size()>=16?l.loadingSha256.substr(0,16):l.loadingSha256)
                                         :"stock:"+l.loadingScreen)+
        " phy_class="+l.phyClass+" ai_class="+l.aiClass+" rival="+(l.rivalRacer?std::to_string(l.rivalRacer):std::string("none"))+
        " modes="+l.modes+" background="+l.background+" route="+l.route+" name="+quoted(l.name)+" planet_text="+quoted(l.planetName);
}

bool addonHandlingClass(const char* code,char& out) noexcept {
    try{
        if(!code||!code[0]||!code[1]||code[2])return false;
        const auto* c=reg::addonCourseByCode(std::string(code,2));
        if(!c)return false;
        out=c->phyClass;return true;
    }catch(...){return false;}
}

AddonCourseRoute addonCourseRoute() noexcept {
    // 146r: the ONE common route. Stock SA runs the same owners with its stock
    // profile as receipts; every other stock code keeps its own loader for now
    // (PORT STAND-INS, flagged where they are selected).
    return {true,"common",""};
}

void announceAddonCourses() noexcept {
    try{std::call_once(announceOnce,[]{
        for(std::size_t i=0;i<reg::addonCourseCount();++i){
            AddonCourseLoad l;
            if(addonCourseLoad(reg::addonCourseAt(i)->number,l))std::fprintf(stderr,"%s\n",addonCourseLoadLine(l).c_str());
        }
    });}catch(...){}
    applyTestAddonCourse();
}

// ---- RECORDS HOOK (DLC save side file, helper 146s) -----------------------------
// Wired by 148r: site 2 is the add-on branch of the rider profile's 1ABFC0
// (qualification on the add-on rows) and 1AC440 commit (submit); the Hall
// session shows the rows after the 13 stock courses (site 1's Hall twin).
// The header's "HOOK, not wired" note predates 148r (left to avoid a rebuild).
// src\assets\dlc_progress50dl.h DlcProgress50DL, filled from the registry as the
// HOOK comment in dlc_registry50dl.h lists. Three call sites, add-on courses only
// (stock courses keep the stock Hall; an add-on course never touches it):
//  1. Course screen, appended control selected:  progress.courseHall(addonCourseRecordsTarget(n), view)
//     -> the records shown under the course (defaults when none were set).
//  2. Results of a race on an add-on course:     ProgressRaceResult r; r.courseKey=addonCourseRecordsTarget(n);
//     (racers, names and 1125C/1125D facts exactly as the stock Hall path reads them) -> progress.submit(r)
//     instead of the stock 1AC440 insertion into profile 0x102F0 + c*0xE4 (c < 13 only).
//  3. Right after each stock save write:          progress.saveIfChanged()
std::string addonCourseRecordsTarget(unsigned number){
    const auto* c=reg::addonCourse(number);
    return c?c->key:std::string();
}

// ---- Last add-on course record ranks (148r, dlc_course_records50dl.h) ----------
namespace {std::mutex recordMutex;AddonCourseRecordRanks lastRecord{};}
bool noteAddonCourseRecord(const AddonCourseRecordRanks& r) noexcept {
    if(!reg::addonCourse(r.number)||r.lap>3u||r.race>3u||r.knockouts>3u)return false;
    std::lock_guard<std::mutex> lock(recordMutex);lastRecord=r;return true;
}
AddonCourseRecordRanks lastAddonCourseRecord() noexcept {
    std::lock_guard<std::mutex> lock(recordMutex);
    return reg::addonCourse(lastRecord.number)?lastRecord:AddonCourseRecordRanks{};
}
void clearAddonCourseRecord() noexcept {std::lock_guard<std::mutex> lock(recordMutex);lastRecord={};}

// ---- Course-screen highlight (146r-U) ---------------------------------------------
namespace {std::atomic<unsigned> highlightedCourse{0};}
bool setHighlightedAddonCourse(unsigned number) noexcept {
    if(number&&!reg::addonCourse(number))return false; // bounds: 0 or an admitted add-on number
    highlightedCourse.store(number);return true;
}
unsigned highlightedAddonCourse() noexcept {
    const unsigned number=highlightedCourse.load();
    return reg::addonCourse(number)?number:0u;
}

} // namespace rr::game::dlc50DL

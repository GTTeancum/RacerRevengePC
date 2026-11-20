// Host-only check of the add-on course layer (helper 146c). Runs the game's own
// DLC startup pass (dlc50DL::startup) over RR_DLC_ROOT, then the course rows,
// the Course-screen control plan over the stock track.dat course group, and the
// race-load data decisions. Not a game run: no window, audio or race. Writes nothing.
#include "../../../../src/assets/dlc_files50dl.h"
#include "../../../../src/assets/dlc_registry50dl.h"
#include "../../../../src/game/dlc_course_entries50dl.h"
#include <cstdio>
#include <string>
#include <vector>

using namespace rr::assets::dlc50DL;
namespace g=rr::game::dlc50DL;
int main(){
    startup();
    g::announceAddonCourses();
    const unsigned testCourse=g::selectedAddonCourse(); // RR_TEST_ADDON_COURSE, applied by announceAddonCourses()
    std::printf("check test_course=%u\n",testCourse);
    bool ok=true;
    // Stock UI/UICourse/track.dat group 1 (control id, CourseIndex, planet, widgets), in group order.
    const std::vector<g::AuthoredCourseControl> authored{{1,0,0,2},{4,11,4,2},{5,4,5,2},{2,8,1,2},{3,7,2,2},{6,1,0,2},{9,5,5,2},
        {7,9,1,2},{13,12,4,2},{14,6,5,2},{11,10,1,2},{12,3,2,2},{10,2,0,2},{8,12,4,0}};
    std::vector<g::AppendedCourseControl> controls;std::string why;
    const bool planned=g::planAppendedCourseControls(authored,24,256,controls,why);
    std::printf("check plan=%d controls=%zu why=\"%s\"\n",planned?1:0,controls.size(),why.c_str());
    ok=ok&&planned&&controls.size()==addonCourseCount();
    for(std::size_t i=0;i<controls.size();++i){
        const auto& a=controls[i];const auto* c=addonCourseAt(i);
        std::printf("check control id=%d layout_from=%d number=%u key=%s template=%s preview=%s background=%s name=\"%s\" planet_text=\"%s\"\n",
            a.controlId,a.layoutFrom,a.number,a.key.c_str(),a.templateCode.c_str(),
            a.preview.empty()?"borrowed":a.preview.c_str(),(a.backgroundFile.empty()?"stock:"+a.backgroundStock:a.backgroundFile).c_str(),a.name.c_str(),a.planetText.c_str());
        ok=ok&&a.controlId==int(24+i)&&a.number==c->number&&a.layoutFrom>0;
    }
    std::vector<g::AppendedCourseControl> none;
    const bool tooMany=addonCourseCount()==0||!g::planAppendedCourseControls(authored,24,24,none,why);
    std::printf("check capacity_refused=%d why=\"%s\"\n",tooMany?1:0,why.c_str());ok=ok&&tooMany;
    for(std::size_t i=0;i<addonCourseCount();++i){
        const auto* c=addonCourseAt(i);g::AddonCourseLoad l;
        const bool got=g::addonCourseLoad(c->number,l);
        char cls=0;const bool phy=g::addonHandlingClass(l.code.c_str(),cls)&&cls==c->phyClass;
        std::string resolved="1";
        try{host(l.archive);if(l.loadingFromPackage)host(l.loadingScreen);if(!c->coursePreview.empty())host(c->coursePreview);}
        catch(const std::exception& e){resolved=std::string("0 (")+e.what()+")";}
        const bool sel=g::selectAddonCourse(c->number)&&g::selectedAddonCourse()==c->number&&g::courseKey(c->number)==c->key&&
            g::addonCourseRecordsTarget(c->number)==c->key&&g::addonCourseAllowsMode(c->number,g::AddonCourseMode::single)==c->single&&
            g::addonCourseAllowsMode(c->number,g::AddonCourseMode::timeTrial)==c->timeTrial&&
            g::addonCourseAllowsMode(c->number,g::AddonCourseMode::vs)==c->vs;
        g::clearAddonCourse();
        std::printf("check load key=%s got=%d phy=%d host=%s selection=%d intro_events=%u script_lines=%u preview=%u/%u/%ux%u loading_sha=%s\n",
            c->key.c_str(),got?1:0,phy?1:0,resolved.c_str(),sel?1:0,l.introEvents,l.scriptLines,
            c->previewKind,c->previewLevels,c->previewWidth,c->previewHeight,c->loadingSha256.substr(0,16).c_str());
        ok=ok&&got&&phy&&resolved=="1"&&sel&&l.tuningInherited&&l.particlesInherited&&l.introEvents==0;
    }
    char cls=0;
    const bool stockRule=!g::addonHandlingClass("ta",cls)&&!g::addonHandlingClass("sc",cls)&&!g::addonHandlingClass("e",cls)&&
        !g::selectAddonCourse(12)&&!g::selectAddonCourse(31)&&!g::selectAddonCourse(unsigned(32+addonCourseCount()))&&
        g::selectedAddonCourse()==0&&g::courseKey(0)=="stock:ta"&&g::courseKey(13).empty()&&g::addonCourseRecordsTarget(0).empty();
    const auto route=g::addonCourseRoute();
    std::printf("check stock_rule=%d route=%s available=%d reason=\"%s\"\n",stockRule?1:0,route.name,route.available?1:0,route.reason);
    ok=ok&&stockRule;
    std::printf("check result=%s\n",ok?"ok":"FAIL");
    return ok?0:1;
}

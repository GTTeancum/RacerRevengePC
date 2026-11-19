// Host-only check of the DLC registry (helper 146p). Runs the game's own startup
// pass (dlc50DL::startup) over RR_DLC_ROOT and prints what the registry holds.
// Not a game run: no window, no audio, no race. Writes nothing.
#include "../../../../src/assets/dlc_files50dl.h"
#include "../../../../src/assets/dlc_registry50dl.h"
#include "../../../../src/game/dlc_rider_entries50dl.h"
#include <cstdio>
#include <set>
#include <string>

using namespace rr::assets::dlc50DL;
namespace g=rr::game::dlc50DL;
int main(){
    startup();
    std::printf("check frozen=%d pods=%zu courses=%zu materials=%zu ui_materials=%zu\n",registryFrozen()?1:0,
        addonPodCount(),addonCourseCount(),addonPodMaterialNames().size(),addonPodUiMaterialNames().size());
    bool ok=true;std::set<unsigned> numbers;
    for(std::size_t i=0;i<addonPodCount();++i){
        const auto* p=addonPodAt(i);
        PodRow row;const bool found=podRow(p->number,row);
        const bool good=found&&row.addon&&row.row==p&&addonPod(p->number)==p&&addonPodByPodId(p->podId)==p&&
            addonNumberRange(p->number)&&numbers.insert(p->number).second;
        // every material name is owned by exactly this pod
        bool owned=true;for(const auto& m:p->materials)owned=owned&&addonPodOwningMaterial(m)==p;
        for(const auto& m:p->uiMaterials)owned=owned&&addonPodOwningMaterial(m)==p;
        std::string resolved;
        try{resolved=host(p->archive).u8string().size()?"1":"0";host(p->camera);host(p->ui.archive);}catch(const std::exception& e){resolved=std::string("0 (")+e.what()+")";}
        std::printf("check pod number=%u key=%s podId=%s archive=%s camera=%s ui=%s name=\"%s\" pod_members=%zu ui_members=%zu materials=%zu ui_materials=%zu row=%d owned=%d host=%s\n",
            p->number,p->key.c_str(),row.podId.c_str(),row.archive.c_str(),row.camera.c_str(),p->ui.archive.c_str(),p->name.c_str(),
            p->podMembers,p->uiMembers,p->materials.size(),p->uiMaterials.size(),good?1:0,owned?1:0,resolved.c_str());
        bool stats=true;for(int l=0;l<6;++l)stats=stats&&p->baseAbilities[l]>=0&&p->maxAbilities[l]>=0;
        const bool menu=g::addonRider(p->number)==p&&g::riderKey(p->number)==p->key&&g::selectAddonRider(p->number)&&
            g::selectedAddonRider()==p->number&&!g::addonRiderMayBeAi(p->number,false)&&g::addonRiderMayBeAi(p->number,true)==p->aiOnAddonCourses;
        g::clearAddonRider();
        std::printf("check rider number=%u stats_base=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f stats_max=%.2f,%.2f,%.2f,%.2f,%.2f,%.2f menu=%d\n",p->number,
            p->baseAbilities[0],p->baseAbilities[1],p->baseAbilities[2],p->baseAbilities[3],p->baseAbilities[4],p->baseAbilities[5],
            p->maxAbilities[0],p->maxAbilities[1],p->maxAbilities[2],p->maxAbilities[3],p->maxAbilities[4],p->maxAbilities[5],menu?1:0);
        ok=ok&&good&&owned&&stats&&menu&&resolved=="1";
    }
    for(std::size_t i=0;i<addonCourseCount();++i){
        const auto* c=addonCourseAt(i);CourseRow row;const bool found=courseRow(c->number,row);
        const bool good=found&&row.addon&&row.row==c&&addonCourseByCode(c->code)==c;
        std::printf("check course number=%u key=%s code=%s phy=%c ai=%c rival=%u loading=%s preview=%s tun=%s particles=%s name=\"%s\" planet=\"%s\" records_key=%s row=%d\n",
            c->number,c->key.c_str(),c->code.c_str(),c->phyClass,c->aiClass,c->rivalRacer,
            c->loadingStock?("stock:"+c->loadingStockCode).c_str():c->loadingFile.c_str(),c->coursePreview.c_str(),
            c->ownTuning?"own":c->inheritTuning.c_str(),c->ownParticles?"own":c->inheritParticles.c_str(),
            c->name.c_str(),c->planetName.c_str(),c->recordsKey.c_str(),good?1:0);
        ok=ok&&good;
    }
    // Stock rows and refusals of numbers outside the registry.
    PodRow stock;CourseRow course;
    const bool stockOk=podRow(4,stock)&&!stock.addon&&stock.podId=="POD04"&&stock.archive=="PODS/POD04/POD04.RES"&&
        !podRow(5,stock)&&!podRow(31,stock)&&!podRow(100,stock)&&addonPod(31)==nullptr&&
        addonPod(unsigned(32+addonPodCount()))==nullptr&&courseRow(12,course)&&course.code=="sc"&&course.rivalRacer==16&&
        !courseRow(13,course)&&addonCourse(unsigned(32+addonCourseCount()))==nullptr;
    const bool selRefused=!g::selectAddonRider(4)&&!g::selectAddonRider(31)&&!g::selectAddonRider(unsigned(32+addonPodCount()))&&
        g::selectedAddonRider()==0&&g::riderKey(4)=="stock:04"&&g::riderKey(5).empty()&&g::addonRiderMayBeAi(4,false);
    g::applyTestAddonRider();
    std::printf("check stock_rows=%d selection_bounds=%d test_rider=%u admitted_packages=%zu\n",stockOk?1:0,selRefused?1:0,
        g::selectedAddonRider(),admittedPackageCount());
    // A dlc:/ path that is not a listed file is refused (never stock).
    bool refusedForeign=false;
    try{host("dlc:/no-such-package/PODS/X/X.RES");}catch(const std::exception&){refusedForeign=true;}
    std::printf("check dlc_unlisted_refused=%d\n",refusedForeign||addonPodCount()+addonCourseCount()==0?1:0);
    ok=ok&&stockOk&&selRefused;
    std::printf("check result=%s\n",ok?"ok":"FAIL");
    return ok?0:1;
}

#include "original_ui_light_manager_lifetime50kg.h"
#include "original_ui_course_lighting50kg.h"
#include "original_ui_tournament_camera50kg.h"
#include "original_ui_garage_camera50kg.h"
#include "original_ui_controls_model50kg.h"
#include <cmath>
#include <cstring>
#include <limits>
#include <exception>

namespace rr::game {
namespace {
using Error=OriginalUiLightManagerLifetimeError50KG;
using Diagnostic=OriginalUiLightManagerLifetimeDiagnostic50KG;
using Cache=OriginalUiLightCacheState50KG;
using Archive=assets::OriginalUiArchiveId50KG;
bool fail(Diagnostic& d,Error error,std::uint32_t pc) noexcept {d={error,pc};return false;}
bool same(const assets::OriginalUiArchiveIdentity50KG* a,Archive id) noexcept {
    return a&&a==assets::originalUiArchiveIdentity50KG(id);
}
// Catalog-only lookup avoids temporary std::string allocation in hot noexcept
// operations. The actual immutable archive was authenticated before entry.
const assets::OriginalUiMemberIdentity50KG* member(Archive archive,const char* name) noexcept {
    const auto* identity=assets::originalUiArchiveIdentity50KG(archive);
    if(!identity)return nullptr;
    for(std::size_t i=0;i<identity->memberCount;++i){
        const auto* row=assets::originalUiMemberIdentity50KG(archive,i);
        if(row&&std::strcmp(row->name,name)==0)return row;
    }
    return nullptr;
}
bool finite(const std::array<float,4>& values) noexcept {
    for(float x:values)if(!std::isfinite(x))return false;return true;
}
}
bool OriginalUiLightManagerLifetime50KG::nativeProjectionStep(NativeProjectionOperation operation,
    const assets::OriginalUiArchiveIdentity50KG* identity,std::uint64_t generation)const noexcept {
    if(!concreteBorrower_)return !concreteProjection_&&!concreteProjectionRetired_;
    return concreteProjection_&&concreteProjection_(concreteBorrower_,*this,operation,identity,generation);
}
bool OriginalUiLightManagerLifetime50KG::constructAtApplicationStartup(Diagnostic& d) noexcept {
    d={};if(cacheState_!=Cache::notConstructed)return fail(d,Error::phase,0x140084);
    //147FF8, observed from the sole140084 application construction call.
    cacheState_=Cache::absentSinceApplicationConstruction;return true;
}
bool OriginalUiLightManagerLifetime50KG::beginUiManager(const assets::OriginalUiArchive50KG& archive,
    Diagnostic& d) noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x148B60);
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(activeUi_||nativeGameplayGeneration_||managerGeneration_==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Error::phase,0x1CCD88);
    const auto* identity=archive.identity();
    if(!archive.loaded()||!(same(identity,Archive::mainMenu)||same(identity,Archive::options)||same(identity,Archive::rider)||same(identity,Archive::course)||same(identity,Archive::hallOfFame)||same(identity,Archive::tournament)||same(identity,Archive::garage)||
       same(identity,Archive::unlock)||same(identity,Archive::win)))
        return fail(d,Error::archive,0x1CCD88);
    if(!enteredFirstMain_&&!same(identity,Archive::mainMenu))return fail(d,Error::phase,0x140084);
    const auto* ui=member(Archive::rider,"UI\\UIRider\\ui.dat");
    const auto* widgets=member(Archive::rider,"UI\\UIRider\\Widgets.Dat");
    if(same(identity,Archive::rider)&&(!ui||!widgets))return fail(d,Error::archive,0x1CDB10);
    const auto* courseUi=member(Archive::course,"UI\\UICourse\\ui.dat");
    if(same(identity,Archive::course)&&!courseUi)return fail(d,Error::archive,0x1CDB10);
    const auto* controlsUi=member(Archive::options,"UI\\UIOption\\ui.dat");
    const auto* controlsWidgets=member(Archive::options,"UI\\UIOption\\Widgets.Dat");
    const auto* controlsScene=member(Archive::options,"ui\\UIOption\\opt.scn");
    if(same(identity,Archive::options)&&(!controlsUi||!controlsWidgets||!controlsScene))return fail(d,Error::archive,0x277FCC);
    // Hall uses its own original screen manager, not Options ownership or a
    // synthetic light registry. Authenticate its actual page closure while
    // preserving the shared APPLICATION cache and manager generation contract.
    if(same(identity,Archive::hallOfFame)&&
       (!member(Archive::hallOfFame,"UI\\UIHOF\\ui.dat")||
        !member(Archive::hallOfFame,"UI\\UIHOF\\Widgets.Dat")||
        !member(Archive::hallOfFame,"UI\\UIHOF\\hall.dat")))return fail(d,Error::archive,0x1CCD88);
    // Same authenticated UI manager light population; no new cache history.
    const auto* tournamentUi=member(Archive::tournament,"UI\\UITourn\\ui.dat");
    if(same(identity,Archive::tournament)&&(!tournamentUi||
       !member(Archive::tournament,"UI\\UITourn\\Widgets.Dat")||
       !member(Archive::tournament,"UI\\UITourn\\standing.dat")))return fail(d,Error::archive,0x1CCD88);
    // UIUnlock (10) and UIWin (7): their own screen managers, reached after a
    // race acknowledgement or a Hall/Win successor. Same authenticated page
    // closure check as the Hall; no new cache history or light registry.
    if(same(identity,Archive::unlock)&&
       (!member(Archive::unlock,"UI\\UIUnlock\\ui.dat")||
        !member(Archive::unlock,"UI\\UIUnlock\\Widgets.Dat")||
        !member(Archive::unlock,"UI\\UIUnlock\\unlock.dat")))return fail(d,Error::archive,0x1CCD88);
    if(same(identity,Archive::win)&&
       (!member(Archive::win,"UI\\UIWin\\ui.dat")||
        !member(Archive::win,"UI\\UIWin\\Widgets.Dat")||
        !member(Archive::win,"UI\\UIWin\\win.dat")))return fail(d,Error::archive,0x1CCD88);
    const auto* garageUi=member(Archive::garage,"UI\\UIGarage\\ui.dat");
    const auto* garageScene=member(Archive::garage,"ui\\UIGarage\\garage.scn");
    if(same(identity,Archive::garage)&&(!garageUi||!garageScene||
       !member(Archive::garage,"UI\\UIGarage\\garage.dat")))return fail(d,Error::archive,0x1CCD88);
    // All source/archive preflight precedes the concrete native receipt. After
    // this checked commit the scalar stores below cannot fail or call out.
    if(!nativeProjectionStep(NativeProjectionOperation::beginUi,identity,managerGeneration_+1))
        return fail(d,Error::phase,0x148B60);
    // This existing scalar registry is the native renderer projection. With a
    // concrete owner it does NOT issue counted148B60 zones or148F30 Light rows.
    // Neither path writesA4; do not reassign cacheState, even to null.
    activeUi_=identity;riderUiIdentity_=ui;riderWidgetIdentity_=widgets;
    riderRegistry_={};riderRegistryReady_=false;
    tournamentUiIdentity_=same(identity,Archive::tournament)?tournamentUi:nullptr;
    tournamentRegistry_={};tournamentAmbient50_={};tournamentDirectional3C_={};tournamentRegistryReady_=false;
    garageUiIdentity_=same(identity,Archive::garage)?garageUi:nullptr;
    garageSceneIdentity_=same(identity,Archive::garage)?garageScene:nullptr;
    garageRegistry_={};garageAmbient50_={};garageDirectional3C_={};garageRegistryReady_=false;
    courseUiIdentity_=same(identity,Archive::course)?courseUi:nullptr;
    courseRegistry_={};courseRegistryReady_=false;
    controlsUiIdentity_=same(identity,Archive::options)?controlsUi:nullptr;
    controlsSceneIdentity_=same(identity,Archive::options)?controlsScene:nullptr;
    controlsWidgetIdentity_=same(identity,Archive::options)?controlsWidgets:nullptr;
    controlsRegistry_={};controlsRegistryReady_=false;
    enteredFirstMain_=true;++managerGeneration_;return true;
}
bool OriginalUiLightManagerLifetime50KG::releaseUiManager() noexcept {
    if(!activeUi_||!nativeProjectionStep(NativeProjectionOperation::endUi,activeUi_,managerGeneration_))return false;
    activeUi_=nullptr;riderUiIdentity_=nullptr;riderWidgetIdentity_=nullptr;
    riderRegistry_={};riderRegistryReady_=false;
    controlsUiIdentity_=nullptr;controlsSceneIdentity_=nullptr;controlsWidgetIdentity_=nullptr;
    controlsRegistry_={};controlsRegistryReady_=false;
    tournamentUiIdentity_=nullptr;tournamentRegistry_={};tournamentAmbient50_={};tournamentDirectional3C_={};tournamentRegistryReady_=false;
    garageUiIdentity_=nullptr;garageSceneIdentity_=nullptr;garageRegistry_={};garageAmbient50_={};garageDirectional3C_={};garageRegistryReady_=false;
    courseUiIdentity_=nullptr;courseRegistry_={};courseRegistryReady_=false;
    //148110's non-reset ofA4 is intentional, including an untracked state.
    return true;
}
bool OriginalUiLightManagerLifetime50KG::publishRiderRegistry(const OriginalUiRiderLighting50KG& lighting,
    Diagnostic& d) noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x148F30);
    riderRegistryReady_=false;
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::rider)||!riderUiIdentity_||lighting.uiIdentity!=riderUiIdentity_)
        return fail(d,Error::phase,0x1CDB10);
    if(!finite(lighting.ambient50)||!finite(lighting.ambientSlot60)||!finite(lighting.directional3C))
        return fail(d,Error::registry,0x148F30);
    for(float x:lighting.directionalLook78)if(!std::isfinite(x))return fail(d,Error::registry,0x147200);
    original_racer_lighting50KM::SmallDirectionalRegistry next{};
    next.count=2;next.registered[0].type=4;
    //146C80 keeps ambient light RGB slots zero; its color lives at+5C.
    auto& directional=next.registered[1];directional.type=2;
    directional.direction78=lighting.directionalLook78;directional.diffuse3C={lighting.directional3C[0],lighting.directional3C[1],lighting.directional3C[2]};
    directional.specular4C=directional.diffuse3C; //146C40 mirrors3C into4C.
    next.selectedZoneAmbient=lighting.ambientSlot60;
    riderRegistry_=next;riderRegistryReady_=true;return true;
}
bool OriginalUiLightManagerLifetime50KG::publishTournamentRegistry(const OriginalUiTournamentLighting50KG& lighting,
    Diagnostic& d) noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x148F30);
    tournamentRegistryReady_=false;
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::tournament)||!tournamentUiIdentity_||lighting.uiIdentity!=tournamentUiIdentity_)
        return fail(d,Error::phase,0x1CDB10);
    if(!finite(lighting.ambient50)||!finite(lighting.ambientSlot60)||!finite(lighting.directional3C))
        return fail(d,Error::registry,0x148F30);
    for(float x:lighting.directionalLook78)if(!std::isfinite(x))return fail(d,Error::registry,0x147200);
    original_racer_lighting50KM::SmallDirectionalRegistry next{};
    next.count=2;next.registered[0].type=4;
    //146C80 keeps ambient light RGB slots zero; its color lives at+5C.
    auto& directional=next.registered[1];directional.type=2;
    directional.direction78=lighting.directionalLook78;directional.diffuse3C={lighting.directional3C[0],lighting.directional3C[1],lighting.directional3C[2]};
    directional.specular4C=directional.diffuse3C; //146C40 mirrors3C into4C.
    next.selectedZoneAmbient=lighting.ambientSlot60;
    tournamentRegistry_=next;tournamentAmbient50_=lighting.ambient50;
    tournamentDirectional3C_=lighting.directional3C;tournamentRegistryReady_=true;return true;
}
bool OriginalUiLightManagerLifetime50KG::matchesTournamentRegistry(
    const OriginalUiTournamentLighting50KG& lighting) const noexcept {
    if(!nativeProjectionCurrent()||cacheState_!=Cache::absentSinceApplicationConstruction ||
       !same(activeUi_,Archive::tournament) || !tournamentUiIdentity_ ||
       lighting.uiIdentity!=tournamentUiIdentity_ || !tournamentRegistryReady_ ||
       tournamentRegistry_.count!=2 || tournamentAmbient50_!=lighting.ambient50 ||
       tournamentDirectional3C_!=lighting.directional3C || !finite(lighting.ambient50) ||
       !finite(lighting.ambientSlot60) || !finite(lighting.directional3C))return false;
    const auto& ambient=tournamentRegistry_.registered[0];
    const auto& directional=tournamentRegistry_.registered[1];
    if(ambient.type!=4 || directional.type!=2 ||
       tournamentRegistry_.selectedZoneAmbient!=lighting.ambientSlot60 ||
       directional.direction78!=lighting.directionalLook78)return false;
    for(std::size_t i=0;i<3;++i) {
        if(!std::isfinite(lighting.directionalLook78[i]) ||
           ambient.direction78[i]!=0.0f || ambient.diffuse3C[i]!=0.0f ||
           ambient.specular4C[i]!=0.0f ||
           directional.diffuse3C[i]!=lighting.directional3C[i] ||
           directional.specular4C[i]!=lighting.directional3C[i])return false;
    }
    return true;
}
bool OriginalUiLightManagerLifetime50KG::publishGarageRegistry(const OriginalUiGarageLighting50KG& lighting,
    Diagnostic& d) noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x148F30);
    garageRegistryReady_=false;
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::garage)||!garageUiIdentity_||lighting.uiIdentity!=garageUiIdentity_||!garageSceneIdentity_||lighting.sceneIdentity!=garageSceneIdentity_)
        return fail(d,Error::phase,0x1CDB10);
    if(!finite(lighting.ambient50)||!finite(lighting.ambientSlot60)||!finite(lighting.directional3C))
        return fail(d,Error::registry,0x148F30);
    for(float x:lighting.directionalLook78)if(!std::isfinite(x))return fail(d,Error::registry,0x147200);
    original_racer_lighting50KM::SmallDirectionalRegistry next{};
    next.count=2;next.registered[0].type=4;
    //146C80 keeps ambient light RGB slots zero; its color lives at+5C.
    auto& directional=next.registered[1];directional.type=2;
    directional.direction78=lighting.directionalLook78;directional.diffuse3C={lighting.directional3C[0],lighting.directional3C[1],lighting.directional3C[2]};
    directional.specular4C=directional.diffuse3C; //146C40 mirrors3C into4C.
    next.selectedZoneAmbient=lighting.ambientSlot60;
    garageRegistry_=next;garageAmbient50_=lighting.ambient50;
    garageDirectional3C_=lighting.directional3C;garageRegistryReady_=true;return true;
}
bool OriginalUiLightManagerLifetime50KG::matchesGarageRegistry(
    const OriginalUiGarageLighting50KG& lighting) const noexcept {
    if(!nativeProjectionCurrent()||cacheState_!=Cache::absentSinceApplicationConstruction ||
       !same(activeUi_,Archive::garage) || !garageUiIdentity_ ||
       lighting.uiIdentity!=garageUiIdentity_ || !garageSceneIdentity_ ||
       lighting.sceneIdentity!=garageSceneIdentity_ || !garageRegistryReady_ ||
       garageRegistry_.count!=2 || garageAmbient50_!=lighting.ambient50 ||
       garageDirectional3C_!=lighting.directional3C || !finite(lighting.ambient50) ||
       !finite(lighting.ambientSlot60) || !finite(lighting.directional3C))return false;
    const auto& ambient=garageRegistry_.registered[0];
    const auto& directional=garageRegistry_.registered[1];
    if(ambient.type!=4 || directional.type!=2 ||
       garageRegistry_.selectedZoneAmbient!=lighting.ambientSlot60 ||
       directional.direction78!=lighting.directionalLook78)return false;
    for(std::size_t i=0;i<3;++i) {
        if(!std::isfinite(lighting.directionalLook78[i]) ||
           ambient.direction78[i]!=0.0f || ambient.diffuse3C[i]!=0.0f ||
           ambient.specular4C[i]!=0.0f ||
           directional.diffuse3C[i]!=lighting.directional3C[i] ||
           directional.specular4C[i]!=lighting.directional3C[i])return false;
    }
    return true;
}
bool OriginalUiLightManagerLifetime50KG::publishCourseRegistry(const OriginalUiCourseLighting50KG& lighting,
    Diagnostic& d) noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x148F30);
    courseRegistryReady_=false;
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::course)||!courseUiIdentity_||lighting.uiIdentity!=courseUiIdentity_)
        return fail(d,Error::phase,0x1CDB10);
    if(!finite(lighting.ambient50)||!finite(lighting.ambientSlot60)||!finite(lighting.directional3C))
        return fail(d,Error::registry,0x148F30);
    for(float x:lighting.directionalLook78)if(!std::isfinite(x))return fail(d,Error::registry,0x147200);
    original_racer_lighting50KM::SmallDirectionalRegistry next{};
    next.count=2;next.registered[0].type=4;
    //146C80 keeps ambient light RGB slots zero; its color lives at+5C.
    auto& directional=next.registered[1];directional.type=2;
    directional.direction78=lighting.directionalLook78;directional.diffuse3C={lighting.directional3C[0],lighting.directional3C[1],lighting.directional3C[2]};
    directional.specular4C=directional.diffuse3C; //146C40 mirrors3C into4C.
    next.selectedZoneAmbient=lighting.ambientSlot60;
    courseRegistry_=next;courseRegistryReady_=true;return true;
}
bool OriginalUiLightManagerLifetime50KG::publishControlsRegistry(const OriginalUiControlsLighting50KG& lighting,
    Diagnostic& d) noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x148F30);
    controlsRegistryReady_=false;
    if(cacheState_!=Cache::absentSinceApplicationConstruction)return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::options)||!controlsUiIdentity_||lighting.uiIdentity!=controlsUiIdentity_||
       !controlsSceneIdentity_||lighting.sceneIdentity!=controlsSceneIdentity_)
        return fail(d,Error::phase,0x277FCC);
    if(!finite(lighting.ambient50)||!finite(lighting.ambientSlot60)||!finite(lighting.directional3C))
        return fail(d,Error::registry,0x148F30);
    for(float x:lighting.directionalLook78)if(!std::isfinite(x))return fail(d,Error::registry,0x147200);
    original_racer_lighting50KM::SmallDirectionalRegistry next{};next.count=2;next.registered[0].type=4;
    auto& directional=next.registered[1];directional.type=2;directional.direction78=lighting.directionalLook78;
    directional.diffuse3C={lighting.directional3C[0],lighting.directional3C[1],lighting.directional3C[2]};
    directional.specular4C=directional.diffuse3C;next.selectedZoneAmbient=lighting.ambientSlot60;
    controlsRegistry_=next;controlsRegistryReady_=true;return true;
}
bool OriginalUiLightManagerLifetime50KG::selectControlsModel(const OriginalUiControlsModelSelection50KG& selection,
    original_racer_lighting50KM::LightMatrices& output,Diagnostic& d) const noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x14A0E8);
    if(cacheState_!=Cache::absentSinceApplicationConstruction)return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::options)||!controlsRegistryReady_||controlsRegistry_.count!=2)
        return fail(d,Error::registry,0x14A128);
    if(!selection.owner||selection.owner->widgetIdentity()!=controlsWidgetIdentity_||
       !selection.owner->isCurrentSelection(selection)||!selection.selected()||selection.sourceZoneIndex48!=0)
        return fail(d,Error::membership,0x14A118);
    const auto status=original_racer_lighting50KM::projectSmallDirectionalRegistry(controlsRegistry_,output);
    if(status!=original_racer_lighting50KM::Status::complete)
        return fail(d,status==original_racer_lighting50KM::Status::arithmetic?Error::arithmetic:Error::registry,0x149BB0);
    return true;
}
bool OriginalUiLightManagerLifetime50KG::selectRiderCharacter(
    const assets::OriginalUiRiderCharacterAssets50KG& assets,
    const OriginalUiRiderModelSelection50KG& selection,
    original_racer_lighting50KM::LightMatrices& output,Diagnostic& d) const noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x14A0E8);
    // This is the actual14A0E8 branch, established by owned startup/lifetime.
    // There is no call to14A860, no cache hit/miss assertion and no insertion.
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::rider)||!riderRegistryReady_||riderRegistry_.count!=2)
        return fail(d,Error::registry,0x14A128);
    if(!assets.loaded()||assets.widgetSource()!=riderWidgetIdentity_||
        assets.widgetId()!=selection.membership.widget||!selection.membership.owner||
        !selection.membership.owner->isCurrentSelection(selection)||!selection.selected()||
        selection.disabled||selection.sourceZoneIndex48!=0)return fail(d,Error::membership,0x14A118);
    const auto status=original_racer_lighting50KM::projectSmallDirectionalRegistry(riderRegistry_,output);
    if(status!=original_racer_lighting50KM::Status::complete)
        return fail(d,status==original_racer_lighting50KM::Status::arithmetic?Error::arithmetic:Error::registry,0x149BB0);
    return true;
}
bool OriginalUiLightManagerLifetime50KG::beginNativeGameplay(std::uint64_t generation,Diagnostic& d) noexcept {
    d={};if(!nativeProjectionCurrent())return fail(d,Error::phase,0x148B60);
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0);
    if(!generation||nativeGameplayGeneration_||activeUi_||!enteredFirstMain_||
       riderRegistryReady_||courseRegistryReady_||controlsRegistryReady_||tournamentRegistryReady_||garageRegistryReady_)return fail(d,Error::phase,0);
    if(!nativeProjectionStep(NativeProjectionOperation::beginGameplay,nullptr,generation))return fail(d,Error::phase,0);
    nativeGameplayGeneration_=generation;return true;
}
bool OriginalUiLightManagerLifetime50KG::endNativeGameplay(std::uint64_t generation,Diagnostic& d) noexcept {
    d={};if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0);
    if(!generation||generation!=nativeGameplayGeneration_||activeUi_)
        return fail(d,Error::phase,0);
    if(!nativeProjectionStep(NativeProjectionOperation::endGameplay,nullptr,generation))return fail(d,Error::phase,0);
    nativeGameplayGeneration_=0;return true;
}
void OriginalUiLightManagerLifetime50KG::invalidateForUntrackedScene() noexcept {
    if(concreteBorrower_||concreteProjection_)std::terminate();
    if(cacheState_==Cache::absentSinceApplicationConstruction)cacheState_=Cache::untracked;
    riderRegistryReady_=false;courseRegistryReady_=false;controlsRegistryReady_=false;tournamentRegistryReady_=false;garageRegistryReady_=false;
}
void OriginalUiLightManagerLifetime50KG::destroyApplication() noexcept {
    if(concreteBorrower_||concreteProjection_)std::terminate();
    nativeGameplayGeneration_=0;
    activeUi_=nullptr;riderUiIdentity_=nullptr;riderWidgetIdentity_=nullptr;
    riderRegistry_={};riderRegistryReady_=false;
    controlsUiIdentity_=nullptr;controlsSceneIdentity_=nullptr;controlsWidgetIdentity_=nullptr;
    controlsRegistry_={};controlsRegistryReady_=false;
    tournamentUiIdentity_=nullptr;tournamentRegistry_={};tournamentAmbient50_={};tournamentDirectional3C_={};tournamentRegistryReady_=false;
    garageUiIdentity_=nullptr;garageSceneIdentity_=nullptr;garageRegistry_={};garageAmbient50_={};garageDirectional3C_={};garageRegistryReady_=false;
    courseUiIdentity_=nullptr;courseRegistry_={};courseRegistryReady_=false;cacheState_=Cache::destroyed;
}
}

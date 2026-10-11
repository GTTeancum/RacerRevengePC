#include "original_ui_light_manager_lifetime50kg.h"
#include <cmath>
#include <cstring>
#include <limits>

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
bool OriginalUiLightManagerLifetime50KG::constructAtApplicationStartup(Diagnostic& d) noexcept {
    d={};if(cacheState_!=Cache::notConstructed)return fail(d,Error::phase,0x140084);
    //147FF8, observed from the sole140084 application construction call.
    cacheState_=Cache::absentSinceApplicationConstruction;return true;
}
bool OriginalUiLightManagerLifetime50KG::beginUiManager(const assets::OriginalUiArchive50KG& archive,
    Diagnostic& d) noexcept {
    d={};if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(activeUi_||managerGeneration_==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Error::phase,0x1CCD88);
    const auto* identity=archive.identity();
    if(!archive.loaded()||!(same(identity,Archive::mainMenu)||same(identity,Archive::options)||same(identity,Archive::rider)))
        return fail(d,Error::archive,0x1CCD88);
    if(!enteredFirstMain_&&!same(identity,Archive::mainMenu))return fail(d,Error::phase,0x140084);
    const auto* ui=member(Archive::rider,"UI\\UIRider\\ui.dat");
    const auto* widgets=member(Archive::rider,"UI\\UIRider\\Widgets.Dat");
    if(same(identity,Archive::rider)&&(!ui||!widgets))return fail(d,Error::archive,0x1CDB10);
    //148B60 invokes148110, then initializes the requested zone rows. Neither
    // function writesA4; do not reassign cacheState here, even to null.
    activeUi_=identity;riderUiIdentity_=ui;riderWidgetIdentity_=widgets;
    riderRegistry_={};riderRegistryReady_=false;enteredFirstMain_=true;++managerGeneration_;return true;
}
bool OriginalUiLightManagerLifetime50KG::releaseUiManager() noexcept {
    if(!activeUi_)return false;
    activeUi_=nullptr;riderUiIdentity_=nullptr;riderWidgetIdentity_=nullptr;
    riderRegistry_={};riderRegistryReady_=false;
    //148110's non-reset ofA4 is intentional, including an untracked state.
    return true;
}
bool OriginalUiLightManagerLifetime50KG::publishRiderRegistry(const OriginalUiRiderLighting50KG& lighting,
    Diagnostic& d) noexcept {
    d={};riderRegistryReady_=false;
    if(cacheState_!=Cache::absentSinceApplicationConstruction)
        return fail(d,Error::untrackedLifetime,0x14A0E8);
    if(!same(activeUi_,Archive::rider)||!riderUiIdentity_||lighting.uiIdentity!=riderUiIdentity_)
        return fail(d,Error::phase,0x1CDB10);
    if(!finite(lighting.ambient50)||!finite(lighting.ambientSlot60)||!finite(lighting.directional3C))
        return fail(d,Error::registry,0x148F30);
    for(float x:lighting.directionalLook78)if(!std::isfinite(x))return fail(d,Error::registry,0x147200);
    original_racer_lighting50KM::SmallDirectionalRegistry next{};
    next.count=2;next.registered[0].type=4;next.registered[0].diffuse3C={lighting.ambient50[0],lighting.ambient50[1],lighting.ambient50[2]};
    next.registered[0].specular4C=next.registered[0].diffuse3C;
    auto& directional=next.registered[1];directional.type=2;
    directional.direction78=lighting.directionalLook78;directional.diffuse3C={lighting.directional3C[0],lighting.directional3C[1],lighting.directional3C[2]};
    directional.specular4C=directional.diffuse3C; //146C40 mirrors3C into4C.
    next.selectedZoneAmbient=lighting.ambientSlot60;
    riderRegistry_=next;riderRegistryReady_=true;return true;
}
bool OriginalUiLightManagerLifetime50KG::selectRiderCharacter(
    const assets::OriginalUiRiderCharacterAssets50KG& assets,
    const OriginalUiRiderModelSelection50KG& selection,
    original_racer_lighting50KM::LightMatrices& output,Diagnostic& d) const noexcept {
    d={};
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
void OriginalUiLightManagerLifetime50KG::invalidateForUntrackedScene() noexcept {
    if(cacheState_==Cache::absentSinceApplicationConstruction)cacheState_=Cache::untracked;
    riderRegistryReady_=false;
}
void OriginalUiLightManagerLifetime50KG::destroyApplication() noexcept {
    activeUi_=nullptr;riderUiIdentity_=nullptr;riderWidgetIdentity_=nullptr;
    riderRegistry_={};riderRegistryReady_=false;cacheState_=Cache::destroyed;
}
}

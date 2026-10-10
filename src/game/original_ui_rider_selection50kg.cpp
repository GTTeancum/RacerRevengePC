#include "original_ui_rider_selection50kg.h"
#include <new>
#include <string>
#include <utility>

namespace rr::game {
namespace {
using S=OriginalUiRiderSelectionStatus50KG;
using N=OriginalUiNavigationStatus50KG;
bool failure(OriginalUiRiderSelectionDiagnostic50KG& d,S s,std::uint32_t pc) noexcept {
    d.status=s;d.sourcePC=pc;return false;
}
bool sameNavigation(const OriginalUiRiderSelectionData50KG& data,
    const OriginalUiNavigation50KG& nav) noexcept {
    return data.page() && nav.page==data.page() && nav.page->groups.size()>1 &&
        !nav.page->groups[1].controls.empty();
}
bool riderAt(const OriginalUiRiderSelectionData50KG& data,
    const OriginalUiNavigation50KG& nav,std::uint16_t slot,std::uint32_t& rider) noexcept {
    const auto& controls=nav.page->groups[1].controls;
    if (!slot || slot>controls.size()) return false;
    const auto control=controls[slot-1];
    if (control<0) return false;
    const auto value=data.controlRider(static_cast<std::uint16_t>(control));
    if (value<0 || value>=32) return false;
    rider=static_cast<std::uint32_t>(value);return true;
}
S navigationFailure(N n) noexcept {
    if (n==N::commandCapacity) return S::capacity;
    if (n==N::noSelectableControl) return S::noEligibleRider;
    return S::invalidSelection;
}
}
bool OriginalUiRiderSelectionData50KG::load(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    OriginalUiRiderSelectionDiagnostic50KG& d) noexcept {
    d={};
    if (archive_) return failure(d,S::alreadyLoaded,0x1E8060);
    if (!archive || !archive->loaded() || !archive->identity() ||
        archive->identity()->id!=assets::OriginalUiArchiveId50KG::rider)
        return failure(d,S::invalidArchive,0x1CFE80);
    try {
        assets::OriginalUiDat50KG index,page,widgets;
        assets::OriginalUiPageIndex50KG pages;
        if (!assets::readOriginalUiDat50KG(*archive,"UI/UIRider/ui.dat",index,d.parse) ||
            !assets::readOriginalUiPageIndex50KG(index,pages,d.parse))
            return failure(d,S::parseFailure,0x1CEC8C);
        // These exact authenticated bytes author both variants to rider.dat.
        // Never infer player from the page file's PageNo (it is 1 in both).
        if (pages.pageFiles.size()!=2 || pages.pageFiles[0]!="rider.dat" ||
            pages.pageFiles[1]!="rider.dat")
            return failure(d,S::invalidPage,0x1CFF00);
        if (!assets::readOriginalUiDat50KG(*archive,"UI/UIRider/"+pages.pageFiles[0],page,d.parse) ||
            !assets::readOriginalUiDat50KG(*archive,"UI/UIRider/Widgets.Dat",widgets,d.parse,
                assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))
            return failure(d,S::parseFailure,0x1E8060);
        assets::OriginalUiPage50KG model;
        if (!assets::readOriginalUiPage50KG(page,widgets,model,d.parse))
            return failure(d,S::parseFailure,0x1D63B0);
        std::array<std::int32_t,assets::originalUiControlLimit50KG> riders;
        riders.fill(-1);
        for (std::size_t id=0;id<model.controls.size();++id) {
            const auto* section=page.section("Control "+std::to_string(id));
            if (section && !assets::originalUiInteger50KG(*section,"PodIndex",-1,
                    riders[id],d.parse,0x1E9C20))
                return failure(d,S::parseFailure,0x1E9C20);
        }
        if (model.pageNo!=1 || model.groups.size()<=1 || model.groups[1].controls.empty())
            return failure(d,S::invalidPage,0x1E9DA4);
        // Authored Rider group has no holes; entry directly dereferences each
        // control. A malformed roster is unavailable, never patched in place.
        for (auto id:model.groups[1].controls)
            if (id<0 || static_cast<std::size_t>(id)>=model.controls.size() ||
                static_cast<std::size_t>(id)>=riders.size() ||
                riders[static_cast<std::size_t>(id)]<0 ||
                riders[static_cast<std::size_t>(id)]>=32)
                return failure(d,S::invalidPage,0x1E9DC8);
        page_=std::move(model);riders_=riders;archive_=std::move(archive);
        return true;
    } catch (const std::bad_alloc&) { return failure(d,S::allocation,0x1E8060); }
      catch (...) { return failure(d,S::parseFailure,0x1E8060); }
}
S projectOriginalUiRiderAppearance50KG(std::uint32_t rider,
    const OriginalUiRiderSelectionProfile50KG& p,OriginalUiRiderAppearance50KG& out) noexcept {
    if (!p.ownerReady) return S::missingProfile;
    if (rider>=p.riders.size()) return S::invalidSelection;
    const auto& row=p.riders[rider];
    if (!row.ownerReady) return S::missingProfile;
    OriginalUiRiderAppearance50KG result;
    auto add=[&](std::uint16_t w,bool enable,std::uint32_t pc) noexcept {
        result.widgets[result.count++]={w,enable,pc};
    };
    result.useSavedColors=row.unlock10FAC!=0;
    if (row.unlock10FAC) {
        add(4,false,0x1EB024);add(1,true,0x1EB038);
        if (p.mode34==2) {
            if (!row.careerByteOwnerReady) return S::missingProfile;
            add(117,true,0x1EB068);add(118,true,0x1EB07C);add(119,true,0x1EB090);
        }
        const bool canReset=p.mode34==2 && row.career1938!=0;
        result.promptMask42C=canReset?7:3;
        add(7,canReset,canReset?0x1EB0F4:0x1EB118);
    } else {
        add(4,true,0x1EB164);add(1,false,0x1EB178);add(7,false,0x1EB18C);
        add(117,false,0x1EB1A0);add(118,false,0x1EB1B4);add(119,false,0x1EB1C8);
        result.promptMask42C=2;
    }
    out=result;return S::ok;
}
S planOriginalUiRiderEntry50KG(const OriginalUiRiderSelectionData50KG& data,
    const OriginalUiNavigation50KG& nav,std::uint8_t player,
    const OriginalUiRiderSelectionProfile50KG& p,OriginalUiRiderSelectionPlan50KG& out) noexcept {
    if (!sameNavigation(data,nav)) return S::invalidPage;
    if (player>1) return S::invalidPlayer;
    if (!p.ownerReady) return S::missingProfile;
    std::uint32_t rider=0;
    if (p.mode34==0) rider=p.remembered6F0;
    else if (p.mode34==1) rider=player?p.remembered6F8:p.remembered6F4;
    else if (p.mode34==2) rider=p.remembered6FC;
    else return S::invalidProfile;
    if (rider>=32) return S::invalidProfile;
    std::uint16_t slot=0;
    const auto& controls=nav.page->groups[1].controls;
    for (std::size_t i=0;i<controls.size();++i)
        if (data.controlRider(static_cast<std::uint16_t>(controls[i]))==static_cast<std::int32_t>(rider))
            slot=static_cast<std::uint16_t>(i+1); // No source early break.
    if (!slot) return S::invalidSelection;
    OriginalUiRiderSelectionPlan50KG result;
    result.nextNavigation=nav;result.player4C0=player;
    result.rider4C4=rider;result.slot4C8=slot;result.sourcePC=0x1E9C50;
    const auto status=enterOriginalUiGroup50KG(result.nextNavigation,1,slot,player,result.navigationCommands);
    if (status!=N::ok) return navigationFailure(status);
    const auto appearance=projectOriginalUiRiderAppearance50KG(rider,p,result.appearance);
    if (appearance!=S::ok) return appearance;
    // OpeningAC runs appearance, THEN clears promptMask42C again. OpenedB0
    // runs appearance again. This plan preserves the raw appearance result;
    // neither hook is implemented or represented as completed here.
    out=result;return S::ok;
}
S planOriginalUiRiderMove50KG(const OriginalUiRiderSelectionData50KG& data,
    const OriginalUiNavigation50KG& nav,std::uint8_t player,std::uint8_t controller,
    std::uint8_t event,bool popup,bool blocked,const OriginalUiRiderSelectionProfile50KG& p,
    OriginalUiRiderSelectionPlan50KG& out) noexcept {
    if (!sameNavigation(data,nav)) return S::invalidPage;
    if (player>1) return S::invalidPlayer;
    if (controller!=player || popup || blocked) { out={};return S::noChange; }
    if (event>1) return S::unsupportedInput;
    if (!p.ownerReady) return S::missingProfile;
    if (nav.controllerGroup[player]!=1) return S::invalidSelection;
    std::uint32_t oldRider=0;
    if (!riderAt(data,nav,nav.groups[1].selectedSlot,oldRider)) return S::invalidSelection;
    OriginalUiRiderSelectionPlan50KG result;
    result.nextNavigation=nav;result.player4C0=player;
    result.sourcePC=event==1?0x1EB1F0:0x1EB370;
    const auto count=nav.page->groups[1].controls.size();
    bool found=false;
    // At most one complete cycle: replaces the source's potential infinite
    // loop for malformed/all-hidden profile state with atomic rejection.
    for (std::size_t attempt=0;attempt<count;++attempt) {
        OriginalUiCommands50KG step;
        const auto status=moveOriginalUiControl50KG(result.nextNavigation,1,event==1,step);
        if (status!=N::ok) return navigationFailure(status);
        if (step.count>result.navigationCommands.values.size()-result.navigationCommands.count)
            return S::capacity;
        for (std::size_t i=0;i<step.count;++i)
            result.navigationCommands.values[result.navigationCommands.count++]=step.values[i];
        ++result.visitedControls;
        result.slot4C8=result.nextNavigation.groups[1].selectedSlot;
        if (!riderAt(data,result.nextNavigation,result.slot4C8,result.rider4C4)) return S::invalidSelection;
        const auto& row=p.riders[result.rider4C4];
        if (!row.ownerReady) return S::missingProfile;
        if (row.unlock10FAC || !row.hidden730) { found=true;break; }
    }
    if (!found) return S::noEligibleRider;
    const auto appearance=projectOriginalUiRiderAppearance50KG(result.rider4C4,p,result.appearance);
    if (appearance!=S::ok) return appearance;
    result.captureAndRestorePodTransform=true;result.resetSelectedControl=true;result.deferred4EC=3;
    out=result;return S::ok;
}
} // namespace rr::game

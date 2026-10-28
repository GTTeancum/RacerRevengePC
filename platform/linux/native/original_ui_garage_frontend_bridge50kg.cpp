#include "original_ui_garage_session50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"

namespace original_main_entry50KG {
namespace {
using SD=rr::assets::OriginalStandardMaterialDiagnostic50LA;
bool failBridge(SD& d,std::string& error,std::uint32_t pc,const char* text)noexcept {
    d.status=rr::assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=pc;
    try{error=text;}catch(...){}return false;
}
}
bool Application::inputGaragePage1FDE70(Frontend& frontend,std::uint32_t input,std::uint32_t controller,
    rr::platform::OriginalUiGarageNativeInput50KG& out,SD& d,std::string& error)const noexcept {
    using Result=rr::platform::OriginalUiGarageNativeInput50KG;
    using Page=rr::platform::OriginalUiGaragePageOwner50KG;
    out={};d={};error.clear();const auto owner=garageCustodyOwner_;
    if(!owner||owner->entryFrontend_!=&frontend||frontend.materialApplicationOwner!=this||
       !owner->publishedOwnerCurrent(*this)||!owner->page_||!owner->managerPublished1B8274_||
       owner->managerLoadPhase_!=rr::platform::OriginalUiGarageSession50KG::ManagerLoadPhase::published)
        return failBridge(d,error,0x1FDE70,"Garage input lacks its actual published same-Frontend manager");
    auto& page=*owner->page_;
    if(!page.sourceEntryReturned1FAE00()||!page.router_||page.router_->sourceVtable!=0x3D3D00||
       page.base_.router134!=page.router_.get()||page.router_->entity.parent1C!=&page.entity_)
        return failBridge(d,error,0x1FDE70,"Garage input lost its real Page/Router owner");
    out.selectedStat=page.field4E4_;
    // Actual disabled Router/Page input does not invoke the Page handler.
    // In particular opening and closing cannot replay an Accept/Back edge.
    if(!(page.router_->entity.flags29&1u)||!page.base_.input268){
        out.status=Result::Status::ignored;out.sourcePC=0x1FDE70;return true;
    }
    // Dispatcher47C precedes the Page. A newly opened dialog cannot consume
    // the same Confirm edge; a pre-existing one publishes2/3 before Page reads.
    auto dialogs=state?state->sharedDialogs:nullptr;
    if(!dialogs||!dialogs->reorderCompleteFor1B872C(page)||dialogs->garagePopupInputFailed_||
       page.drawPending_||page.runtimeWidgetPending_||page.sharedHookPending_||
       dialogs->popup47CDraw_||dialogs->nativePopup47CFifoPins_)
        return failBridge(d,error,0x2062D0,"Garage popup lost its retained input owner or has a failed source prefix");
    rr::game::OriginalUiControllerPopupPlan50KG popup;
    if(controller>255u||input>255u||rr::game::OriginalUiControllerPopup50KG::input(state->controllerPopupState,
        static_cast<std::uint8_t>(controller),static_cast<std::uint8_t>(input),true,popup)!=rr::game::OriginalUiControllerPopupStatus50KG::ok)
        return failBridge(d,error,0x2062D0,"Garage popup input state is invalid");
    bool popupCommitted=false;std::uint64_t popupBoundary=0;
    if(popup.accepted){
        rr::platform::OriginalApplicationAural50KG::Diagnostic ad;
        if(!owner->audio_||!sourceAural00Owner_||!owner->audio_->canPlayGaragePageEffect1FDE70(page,popup.soundId)||
           !sourceAural00Owner_->canFinishGaragePageInput267BE0(page,ad))
            return failBridge(d,error,popup.soundPC,"Garage popup sound/transport source is not ready");
        state->controllerPopupState=popup.state;popupCommitted=true;
        if(!owner->audio_->playGaragePageEffect1FDE70(page,popup.soundId)||
           !sourceAural00Owner_->finishGaragePageInput267BE0(page,popupBoundary,ad)){
            dialogs->garagePopupInputFailed_=true;out.status=Result::Status::failed;
            out.sourceEffectsCommitted=true;d.sourceEffectsCommitted=true;
            return failBridge(d,error,ad.sourcePC?ad.sourcePC:popup.soundPC,"Garage popup source prefix failed; result cannot be replayed");
        }
    }
    Page::InputOutcome actual;Page::Diagnostic pd;
    const bool returned=page.dispatchInput1FDE70(input,controller,actual,pd);
    out.serial=actual.serial;out.transportBoundary=actual.transportBoundary?actual.transportBoundary:popupBoundary;
    out.sourcePC=actual.sourcePC?actual.sourcePC:pd.sourcePC;out.selectedStat=page.field4E4_;
    out.sourceEffectsCommitted=popupCommitted||actual.sourceEffectsCommitted||pd.sourceEffectsCommitted;
    out.requestWritten=actual.requestWritten;
    switch(actual.status){
    case Page::InputOutcome::Status::ignored:out.status=Result::Status::ignored;break;
    case Page::InputOutcome::Status::completed:out.status=Result::Status::completed;break;
    case Page::InputOutcome::Status::failed:out.status=Result::Status::failed;break;
    default:out.status=Result::Status::unavailable;break;
    }
    if(!returned){
        if(out.sourceEffectsCommitted)out.status=Result::Status::failed;
        d.sourceEffectsCommitted=out.sourceEffectsCommitted;
        return failBridge(d,error,out.sourcePC,"Garage actual Page input did not complete its retained source operation");
    }
    d.sourcePC=out.sourcePC;d.sourceEffectsCommitted=out.sourceEffectsCommitted;return true;
}
bool Application::publishGarageTarget140A68(Frontend& frontend,int width,int height,
    rr::platform::VehicleWindowDiagnostic50DS& d,std::string& error)const noexcept {
    using S=rr::platform::OriginalUiGarageSession50KG;
    d={};error.clear();const auto s=garageCustodyOwner_;
    if(!s||s->entryFrontend_!=&frontend||frontend.materialApplicationOwner!=this||
       !s->publishedOwnerCurrent(*this)||!s->drawContribution_||
       s->runtimeRootPhase_!=S::RootRuntimePhase::completed||
       s->runtimeTailPhase_!=S::RootTailPhase::timingCommitted||s->runtimeRootReturnPC_!=0x140D60){
        try{error="Garage target publication requires its complete real Root and committed timing";}catch(...){}return false;
    }
    if(!s->drawContribution_->publishReturnedTarget140A68(width,height,d)){
        try{error="Garage completed target publication refused its retained renderer lifetime";}catch(...){}return false;
    }
    s->runtimeTargetPublished140A68_=true;return true;
}
bool Application::prepareNextGarageFrame140CE0(Frontend& frontend,SD& d,std::string& error)const noexcept {
    using S=rr::platform::OriginalUiGarageSession50KG;
    d={};error.clear();const auto owner=garageCustodyOwner_;
    if(!owner||owner->entryFrontend_!=&frontend||frontend.materialApplicationOwner!=this||
       !owner->publishedOwnerCurrent(*this)||!owner->page_)
        return failBridge(d,error,0x140CE0,"Garage next frame lacks its same live manager");
    auto& s=*owner;auto& page=*s.page_;
    original_ui_source_timing50LH::PublicationView timing;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!frontend.sourceDispatchTiming.retainPublication(timing)||!timing.sameOwner(s.entryTiming_)||
       !timing.readPrepared(sample,serial))
        return failBridge(d,error,0x140AC0,"Garage next frame requires the actual prepared Frontend timestamp");
    if(s.runtimeRootPhase_==S::RootRuntimePhase::notStarted){
        if(s.runtimePhase_!=S::RuntimePhase::notStarted||s.runtimeFrame_||s.drawContribution_)
            return failBridge(d,error,0x140CE0,"Garage first frame contains an undischargeable prior prefix");
        return true;
    }
    if(s.runtimeRootPhase_!=S::RootRuntimePhase::completed||
       s.runtimeTailPhase_!=S::RootTailPhase::timingCommitted||
       s.runtimeRootReturnPC_!=0x140D60||!s.runtimeFrame_||!s.drawContribution_||
       serial<=s.runtimeRootPublication_||serial<=page.lastCompletedUpdatePublication_||
       page.updateFailed_||!page.updateReturned_||page.drawPending_||
       page.runtimeWidgetPending_||page.sharedHookPending_||page.timerCallbackPending_||page.speechStopPending_||
       s.managerLifecycleCallbackPending_||sourceApplicationMode22C_!=0)
        return failBridge(d,error,0x140CE0,"Garage prior frame or source callback is not complete; no replay/reset");
    rr::platform::OriginalUiGarageDrawContribution50KG::Diagnostic draw;
    // This first consumes the actual published prior target and renews the
    // same bank lease. It validates OLD Session/frame receipts before reset.
    if(!s.drawContribution_->rearmNextPublication140230(timing,serial,draw))
        return failBridge(d,error,draw.sourcePC,"Garage renderer refused the exact next-publication lifetime");
    // Reset only native invocation cursors. All source Page/Scene/audio,
    // Camera, material, particle, timing and input histories survive.
    page.updatePublication_=0;page.updateSourcePC_=0;page.updateChild_=nullptr;
    page.updatedPageChildren_=0;page.updatedLights_=0;page.updateReturned_=false;
    page.drawPublication_=0;page.drawSourcePC_=0;page.sharedApertureOrdinal_=0;
    s.runtimeFrame_.reset();s.runtimePhase_=S::RuntimePhase::notStarted;
    s.runtimeRootPhase_=S::RootRuntimePhase::notStarted;s.runtimeTailPhase_=S::RootTailPhase::notStarted;
    s.runtimeTailSample_={};s.runtimeTailPublication_=0;s.runtimeTailAudioBoundary_=0;s.runtimeTailAudioFrame_=0;
    s.runtimeTailMainRevision_=0;s.runtimeTailMixerRevision_=0;s.runtimeTailGroupSequence_=0;
    s.runtimeRootReturnPC_=0;s.runtimeTargetPublished140A68_=false;s.runtimeOuterChild_=nullptr;s.runtimeRootChild_=nullptr;
    s.runtimeRootPublication_=0;s.runtimeRootSourcePC_=0;s.runtimeLightReturnPC_=0;
    s.runtimeRoot10ReturnPC_=0;s.runtimeOuter26CReturnPC_=0;s.runtimeManagerChild_=nullptr;
    s.runtimeSourcePC_=0;s.runtimeManagerReturnPC_=0;s.runtimeDispatcherReturnPC_=0;
    s.runtimeVisitedChildren_=0;s.runtimeReturnedTextOuters_=0;
    return true;
}
bool Application::loadPreparedGarageManager1D1950(Frontend& frontend,
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> fonts,
    std::shared_ptr<rr::platform::OriginalUiGarageSession50KG>& out,SD& d,std::string& error)const noexcept {
    d={};error.clear();
    if(frontend.materialApplicationOwner!=this||!frontend.garageDestinationAssets_||
       !frontend.garageDestinationAssets_->loaded()||
       frontend.garageDestinationAssets_->archive()!=frontend.garageDestinationArchive_)
        return failBridge(d,error,0x1D1950,"Garage load requires its actual retained authenticated destination inputs");
    const bool loaded=loadGarageManager1D1950(frontend,frontend.garageDestinationAssets_,std::move(fonts),1,d,error);
    // Retain the exact actual partial prefix even when its later source call
    // failed. This is custody, never a successful load/frame receipt.
    if(garageCustodyOwner_&&garageCustodyOwner_->issuer_==this&&
       (!garageCustodyOwner_->entryFrontend_||garageCustodyOwner_->entryFrontend_==&frontend)&&
       garageCustodyOwner_->sceneAssets_==frontend.garageDestinationAssets_)out=garageCustodyOwner_;
    if(!loaded)return false;
    if(!garageCustodyOwner_||!garageManagerLoadedCurrent(frontend,*garageCustodyOwner_))
        return failBridge(d,error,0x1B8274,"Garage loader did not publish its exact completed manager/Page owner");
    out=garageCustodyOwner_;return true;
}
bool Application::garageManagerLoadedCurrent(const Frontend& frontend,
    const rr::platform::OriginalUiGarageSession50KG& s)const noexcept {
    using S=rr::platform::OriginalUiGarageSession50KG;
    const auto display=frontend.garageTransferDisplay_.lock();
    // Observe the actual retained binding, not coordinator readiness: that
    // higher layer itself asks whether this manager is current.
    return garageCustodyOwner_.get()==&s&&s.entryFrontend_==&frontend&&
        frontend.materialApplicationOwner==this&&s.publishedOwnerCurrent(*this)&&
        s.managerDrivePhase_==S::ManagerDrivePhase::returned&&
        s.managerLoadPhase_==S::ManagerLoadPhase::published&&s.managerPublished1B8274_&&
        s.page_&&s.canRequestPage1D6130(*s.page_)&&
        frontend.garageTransferPhase_==Frontend::GarageTransferPhase::ready&&
        frontend.garageTransferIdentityCurrent(s.grant_)&&display&&frontend.garageTransferActive_&&
        display->frontendTransferBindingCurrent50KG(*frontend.garageTransferActive_,frontend.uploadLifetime_)&&
        frontend.coordinator.borrowsRenderer(display->renderer_);
}
bool Application::observeGarageRuntime50KG(const Frontend& frontend,
    const rr::platform::OriginalUiGarageSession50KG& s,rr::platform::OriginalUiGarageNativeState50KG& out)const noexcept {
    using S=rr::platform::OriginalUiGarageSession50KG;
    out={};if(s.issuer_!=this||s.ownerThread_!=std::this_thread::get_id())return false;
    const bool retired=s.closed_&&s.normalFrontend_==&frontend&&
        s.normalRetirementPhase_==S::NormalRetirementPhase::currentCleared;
    if(!retired&&!garageManagerLoadedCurrent(frontend,s))return false;
    out.retired=retired;out.loaded=!retired;out.publication=s.runtimeRootPublication_;
    out.managerLifecycle=s.managerLifecycle16C_;out.managerTransition=s.managerTransition170_;
    out.rootReturned=s.runtimeRootPhase_==S::RootRuntimePhase::completed&&s.runtimeRootReturnPC_==0x140D60;
    out.timingCommitted=s.runtimeTailPhase_==S::RootTailPhase::timingCommitted;
    out.targetPublished=s.runtimeTargetPublished140A68_;
    if(out.timingCommitted)out.updateSeconds=s.runtimeTailSample_.updateSeconds;
    if(s.runtimeFrame_){std::uint32_t pause=0;if(!s.runtimeFrame_->pause_.readPause11C(pause))return false;out.paused=pause!=0;}
    if(s.page_){
        const auto& page=*s.page_;const auto& shared=page.shared_;
        out.selectedStat=page.field4E4_;
        if(!shared.top284||!shared.bottom28C||!shared.state4B4||!shared.speed4B8||
           *shared.state4B4<0||*shared.state4B4>3)return false;
        out.aperture.bound=true;out.aperture.top284=*shared.top284;out.aperture.bottom28C=*shared.bottom28C;
        out.aperture.state=static_cast<rr::game::OriginalUiApertureState50KG>(*shared.state4B4);
        out.aperture.speed4B8=*shared.speed4B8;out.aperture.savedDelta298=shared.savedDelta298;
        out.aperture.hasUpdateDelta=s.runtimeFrame_!=nullptr;out.aperture.inputEnabled268=page.base_.input268!=0;
        out.aperture.geometryPrepared=page.aperturePacketCurrent2E21D0(0)&&page.aperturePacketCurrent2E21D0(1);
    }
    return true;
}

}

namespace rr::platform {
std::shared_ptr<OriginalUiRiderProfileOwner50KG>
OriginalUiGarageSession50KG::profileOwnerForReturn17DE10()const noexcept {
    if(!issuer_||ownerThread_!=std::this_thread::get_id()||!managerQueued17DE10_||
       !managerPendingScreen164_||*managerPendingScreen164_!=9||
       !managerPendingPage168_||*managerPendingPage168_!=1||managerField15C_!=1||managerField174_||
       !returnProfile17DE10_||runtimeRootPhase_!=RootRuntimePhase::completed||
       runtimeTailPhase_!=RootTailPhase::timingCommitted||!runtimeTargetPublished140A68_)return {};
    if(closed_){
        if(normalRetirementPhase_!=NormalRetirementPhase::currentCleared||issuer_->garageCustodyOwner_)return {};
    }else if(issuer_->garageCustodyOwner_.get()!=this||failed_||!published_)return {};
    const auto current=returnProfile17DE10_->stamp();
    if(current.generation!=returnProfileStamp17DE10_.generation||current.mode34!=returnProfileStamp17DE10_.mode34||current.mode34!=2)return {};
    return returnProfile17DE10_;
}
}

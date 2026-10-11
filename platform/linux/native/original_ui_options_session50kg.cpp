#include "original_ui_options_session50kg.h"
#include <cmath>
#include <limits>
#include <new>
#include <utility>
namespace rr::platform {
namespace {
using Status=OriginalUiOptionsSessionStatus50KG;
using Diagnostic=OriginalUiOptionsSessionDiagnostic50KG;
using WidgetStatus=game::OriginalUiOptionsWidgetStatus50KG;
using Phase=game::OriginalUiOptionsMenuPhase50KG;
using Notice=game::OriginalUiTransitionNotificationKind50KG;
using Hook=game::OriginalUiApertureHookKind50KG;
using ApertureStatus=game::OriginalUiApertureStatus50KG;
bool widgetOkay(WidgetStatus s) noexcept {return s==WidgetStatus::ok||s==WidgetStatus::noChange||s==WidgetStatus::hidden;}
bool apertureOkay(ApertureStatus s) noexcept {return s==ApertureStatus::ok||s==ApertureStatus::noChange;}
Status result(Diagnostic& d,Status s,std::uint32_t pc=0) noexcept {d.status=s;d.sourcePC=pc;return s;}
bool sameDecision(const game::OriginalUiRouteDecision50KG& a,const game::OriginalUiRouteDecision50KG& b) noexcept {
    const auto& x=a.request;const auto& y=b.request;
    if(a.action!=b.action||a.blockers!=b.blockers||a.admitted!=b.admitted||
        a.profileProposalComplete!=b.profileProposalComplete||a.selectedSlot!=b.selectedSlot||
        a.selectedRider!=b.selectedRider||a.inputPC!=b.inputPC||x.kind!=y.kind||
        x.sourceScreen!=y.sourceScreen||x.sourcePage!=y.sourcePage||x.targetScreen!=y.targetScreen||
        x.targetPage!=y.targetPage||x.sourcePC!=y.sourcePC||a.profileChanges.count!=b.profileChanges.count||
        a.profileChanges.count>a.profileChanges.values.size())return false;
    for(std::size_t i=0;i<a.profileChanges.count;++i){const auto& p=a.profileChanges.values[i];const auto& q=b.profileChanges.values[i];
        if(p.field!=q.field||p.operation!=q.operation||p.value!=q.value||p.replaceMask!=q.replaceMask||p.sourcePC!=q.sourcePC)return false;
    }
    return true;
}
}
void resetOriginalUiOptionsProfileFields50KG(OriginalUiOptionsProfile50KG& profile) noexcept {
    profile.savedSlot6E4=1; //186660 li v0,1;186668 sw v0,+6E4
}
struct OriginalUiOptionsSession50KG::Storage {
    OriginalUiOptionsSessionCold50KG cold;
    OriginalUiOptionsDrawContribution50KG collector;
    game::OriginalUiApertureOwner50KG aperture,gameAperture;
    bool gamePrepared=false;
    OriginalUiOptionsSessionSnapshot50KG state;
    Diagnostic callbackFault;
    game::OriginalUiRouteDecision50KG pendingBack{};
    game::OriginalUiGameSettingsProfile50KG pendingGameProfile{};
    game::OriginalUiGameSettings50KG pendingGameLocal{};
    std::uint64_t pendingSoundRevision=0;
    bool pendingAcceptCommitted=false;
    std::uint64_t proposalSerial=0;
    bool dependenciesPrepared=false,inOperation=false,rendering=false,paused=false,updatePrepared=false;
    OriginalUiMainSessionUpdate50KG preparedUpdate{};
    game::OriginalUiOptionsMenuWidgets50KG& bank() noexcept {return *cold.resources->widgets();}
    bool operational() const noexcept {return dependenciesPrepared&&!state.faulted&&!state.quitRequested;}
    void fault(Status s,std::uint32_t pc) noexcept {if(!state.faulted){state.faulted=true;callbackFault.status=s;callbackFault.sourcePC=pc;}}
    game::OriginalUiTransitionSnapshot50KG transition() const noexcept {return cold.externalTransition->snapshot();}
    std::uint8_t activePage() const noexcept {return transition().current.page;}
    game::OriginalUiApertureOwner50KG& activeAperture() noexcept {return activePage()==4?gameAperture:aperture;}
    const game::OriginalUiApertureOwner50KG& activeAperture() const noexcept {return activePage()==4?gameAperture:aperture;}
    bool ownsPage(std::uint8_t page) const noexcept {return page==1||(page==4&&gamePrepared&&cold.gameSettingsProfile&&game::validOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile));}
    bool currentPage() const noexcept {const auto t=transition();return t.bound&&t.current.screen==12&&ownsPage(t.current.page);}
    bool idleInput() const noexcept {const auto t=transition();return currentPage()&&t.lifecycle==game::OriginalUiTransitionLifecycle50KG::idle&&
        t.phase==game::OriginalUiTransitionPhase50KG::none&&!t.initialRestartPending&&t.requested.screen==12&&t.requested.page==t.current.page&&!t.applicationRequestQueued&&state.pageShown&&state.inputContainerShown&&
        activeAperture().snapshot().inputEnabled268&&activeAperture().snapshot().state==game::OriginalUiApertureState50KG::open;}
    bool canApplyGameOptions(std::uint32_t mode) const noexcept {
        return cold.gameSettingsProfile&&game::validOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile)&&
            cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&cold.audio&&cold.soundMode&&
            cold.audio->soundModeOwner50KG()==cold.soundMode.get()&&cold.soundMode->ready()&&
            game::validOriginalSoundModeFields50KG(cold.soundMode->fields())&&
            static_cast<std::uint32_t>(cold.soundMode->fields().profile1101C)==cold.gameSettingsProfile->soundMode1101C&&
            cold.audio->canApplySoundMode18874050KG(mode)&&cold.audio->canPlayGameOptionsEffect(1)&&
            state.gameOptionsAcceptCount!=std::numeric_limits<std::uint64_t>::max();
    }
    bool pendingAcceptCurrent() const noexcept {
        const auto* page=cold.resources->widgets()->gameOptionsState();
        return !pendingAcceptCommitted&&page&&page->loaded&&
            game::sameOriginalUiGameSettings50KG(page->local,pendingGameLocal)&&
            canApplyGameOptions(pendingGameLocal.soundMode)&&
            game::sameOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile,pendingGameProfile)&&
            cold.soundMode->revision()==pendingSoundRevision;
    }
    bool acceptedGameSettingsCurrent() const noexcept {
        if(!pendingAcceptCommitted||!cold.gameSettingsProfile||!cold.soundMode||!cold.audio||
            cold.audio->soundModeOwner50KG()!=cold.soundMode.get()||
            cold.soundMode->revision()!=pendingSoundRevision+1||
            !game::validOriginalSoundModeFields50KG(cold.soundMode->fields())||
            static_cast<std::uint32_t>(cold.soundMode->fields().profile1101C)!=pendingGameLocal.soundMode)return false;
        auto expected=pendingGameProfile;expected.soundMode1101C=pendingGameLocal.soundMode;
        expected.volume11020=pendingGameLocal.volume;expected.savedGroup6E8=pendingBack.selectedSlot;
        expected.flags11050=static_cast<std::uint8_t>((expected.flags11050&~7u)|
            (pendingGameLocal.autoSave?1u:0u)|(pendingGameLocal.verticalSplit?2u:0u)|(pendingGameLocal.vibration?4u:0u));
        const auto* page=cold.resources->widgets()->gameOptionsState();
        return page&&page->loaded&&game::sameOriginalUiGameSettings50KG(page->local,pendingGameLocal)&&
            game::sameOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile,expected)&&
            cold.audio->canPlayGameOptionsEffect(1);
    }
    void widgetPhase(Phase p,std::uint32_t pc,std::uint8_t page=0) noexcept {
        if(!page)page=activePage();
        if(page==4){if(!ownsPage(4)){fault(Status::unsupportedState,pc);return;}
            if(!widgetOkay(bank().gameOptionsPhase(p,*cold.gameSettingsProfile,callbackFault.widget)))fault(Status::widget,pc);return;}
        if(!cold.profile->known||cold.profile->savedSlot6E4<1||cold.profile->savedSlot6E4>6){fault(Status::unsupportedState,pc);return;}
        if(!widgetOkay(bank().phase(p,cold.profile->known,static_cast<std::uint16_t>(cold.profile->savedSlot6E4),callbackFault.widget)))fault(Status::widget,pc);
    }
    bool audioEffect(std::uint32_t id) noexcept {
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return true;}
        if(!cold.audio||!cold.audio->ready()||!(activePage()==4?cold.audio->playGameOptionsEffect(id):cold.audio->playOptionsEffect(id))){++state.audio.failed;fault(Status::audio,0x292360);return false;}
        ++state.audio.submitted;return true;
    }
    static bool pageEffects(void* p,const game::OriginalUiOptionsMenuPageResult50KG& result) noexcept {
        auto& s=*static_cast<Storage*>(p);
        // Unavailable routes defer every effect until real destination admission.
        // Genuine directions sound BEFORE the later movement widget callback.
        return result.routeStatus==game::OriginalUiRouteStatus50KG::unavailable||!result.soundRequested||s.audioEffect(result.soundId);
    }
    static bool available(void* p) noexcept {auto& s=*static_cast<Storage*>(p);return s.operational()&&s.cold.resources->exitText()->prepared();}
    static void hook(void* p,const game::OriginalUiApertureHook50KG& event) noexcept {
        auto& s=*static_cast<Storage*>(p);if(!s.operational())return;
        switch(event.kind) {
        case Hook::openingAC:s.widgetPhase(Phase::openingAC,event.callPC);break;
        case Hook::openedB0:s.widgetPhase(Phase::openedB0,event.callPC);break;
        case Hook::closingB4:s.widgetPhase(Phase::closingB4,event.callPC);break;
        case Hook::closingThresholdB8:
            // Options target1F9FE0 is an actual return-only leaf.
            if(event.targetPC!=(s.activePage()==4?0x1F5F30u:0x1F9FE0u))s.fault(Status::unsupportedState,event.callPC);break;
        case Hook::closedBC:
            // Shared1D7A00 completion is implemented in the aperture itself.
            if(event.targetPC!=0x1D7A00)s.fault(Status::unsupportedState,event.callPC);break;
        case Hook::screenExitObject440:
            if(!s.rendering||!s.collector.projectExitText(event,s.callbackFault.draw))s.fault(Status::draw,event.callPC);break;
        }
    }
    static void overlay(void* p,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& r) noexcept {
        auto& s=*static_cast<Storage*>(p);if(!s.operational())return;
        if(!s.rendering||!s.collector.projectAperture(ordinal,r,s.callbackFault.draw))s.fault(Status::draw,0x1D798C);
    }
    bool updateWidgets(const std::vector<std::uint16_t>& ids) noexcept {
        for(auto id:ids)if(bank().widgetState(id).kind==game::OriginalUiOptionsWidgetKind50KG::text)
            if(!widgetOkay(bank().updateText(id,true,callbackFault.widget))){fault(Status::widget,0x141ED0);return false;}
        return true;
    }
};
OriginalUiOptionsSession50KG::OriginalUiOptionsSession50KG() noexcept=default;
OriginalUiOptionsSession50KG::~OriginalUiOptionsSession50KG()=default;
Status OriginalUiOptionsSession50KG::bind(const OriginalUiOptionsSessionCold50KG& cold,Diagnostic& d) noexcept {
    d={};if(storage_)return result(d,Status::alreadyBound);
    if(!cold.resources||!cold.resources->loaded()||!cold.resources->widgets()||!cold.resources->exitText()||
        !cold.resources->exitText()->prepared()||!cold.optionsArchive||!cold.optionsArchive->loaded()||
        cold.optionsArchive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options))return result(d,Status::resource);
    if(!cold.externalTransition||cold.externalTransition->snapshot().bound||!cold.profile||!cold.profile->known||
        cold.profile->savedSlot6E4<1||cold.profile->savedSlot6E4>6||!cold.commonProfile||!cold.commonProfile->known||
        !cold.application||!cold.application->known||(cold.application->globalFlags228&4u)||cold.application->transportE00!=-1)
        return result(d,Status::unsupportedState);
    if(cold.audioMode!=OriginalUiMainAudioMode50KG::enabled&&cold.audioMode!=OriginalUiMainAudioMode50KG::muted)return result(d,Status::invalidInput);
    if(cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&(!cold.audio||!cold.audio->ready()))return result(d,Status::audio);
    if(cold.audioMode==OriginalUiMainAudioMode50KG::muted&&cold.audio)return result(d,Status::invalidInput);
    try {
        auto s=std::make_unique<Storage>();s->cold=cold;
        if(!s->collector.bindFrame(cold.resources,cold.gpu,d.draw))return result(d,Status::draw);
        const auto* page=s->bank().page();
        if(!page||page->useVideoAsTexture||page->background!="*INVALID*")return result(d,Status::unsupportedState,0x1D63B0);
        for(unsigned c=0;c<3;++c){std::int32_t channel=0;
            if(!game::convertOriginalUiApertureCvtWS50KG(page->backgroundColor[c],channel)||channel<0||channel>255)
                return result(d,Status::unsupportedState,0x1D6D24);
            s->state.packedBackground430|=static_cast<std::uint32_t>(channel)<<(16u-8u*c);
        }
        s->dependenciesPrepared=true;
        game::OriginalUiApertureCallbacks50KG callbacks;callbacks.context=s.get();callbacks.available=&Storage::available;
        callbacks.hook=&Storage::hook;callbacks.submitOverlay=&Storage::overlay;
        if(s->aperture.bindOptionsMenu(cold.optionsArchive,callbacks,d.aperture)!=ApertureStatus::ok)return result(d,Status::aperture);
        if(cold.gameSettingsProfile){
            if(!game::validOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile)||!cold.resources->gameOptionsExitText()||
                !cold.resources->gameOptionsExitText()->prepared())return result(d,Status::unsupportedState);
            if(s->gameAperture.bindGameOptions(cold.optionsArchive,callbacks,d.aperture)!=ApertureStatus::ok)return result(d,Status::aperture);
            s->gamePrepared=true;
        }
        // No manager bind, tick, widget phase, shared-profile write or music
        // start during this cold prepare. Coordinator owns the initial notices.
        s->state.bound=true;storage_=std::move(s);return result(d,Status::ok);
    }catch(const std::bad_alloc&){return result(d,Status::allocation);}catch(...){return result(d,Status::resource);}
}
Status OriginalUiOptionsSession50KG::notifyTransition(const game::OriginalUiTransitionNotification50KG& event,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(!s->operational()||s->inOperation||event.route.screen!=12||!s->ownsPage(event.route.page))return result(d,Status::unsupportedState,event.sourcePC);
    const auto t=s->transition();
    if(!t.bound||event.lifecycle!=t.lifecycle||event.phase!=t.phase||
        !((t.current.screen==12&&t.current.page==event.route.page)||(t.requested.screen==12&&t.requested.page==event.route.page)))return result(d,Status::invalidInput,event.sourcePC);
    s->state.framePublished=false;s->state.backProposalPending=false;s->inOperation=true;
    switch(event.kind) {
    case Notice::requestedPage90:s->widgetPhase(Phase::requestedPage90,event.sourcePC,event.route.page);s->state.pageShown=true;break;
    case Notice::previousPage94:s->widgetPhase(Phase::previousPage94,event.sourcePC,event.route.page);s->state.pageShown=false;break;
    case Notice::pageEntry74:
        s->widgetPhase(Phase::entry74,event.sourcePC,event.route.page);
        // Page+C4 aliases the one real manager camera(+2B0); entry enables and
        // registers that camera. Renderer uses its authenticated prepared pose.
        s->state.cameraActive=true;s->cold.application->uiObject3D8=0;break;
    case Notice::managerEntry84:break; // inherited2A4330 return-only leaf
    case Notice::beginTransition7C:
        s->state.backProposalPending=false;
        if(!apertureOkay((event.route.page==4?s->gameAperture:s->aperture).beginTransition7C(event.phase)))s->fault(Status::aperture,event.sourcePC);break;
    case Notice::pageAuxiliary1C:s->state.inputContainerShown=false;break;
    case Notice::pageAuxiliary24:s->state.inputContainerShown=true;break;
    }
    s->inOperation=false;if(s->state.faulted){d=s->callbackFault;return d.status;}return result(d,Status::ok);
}
game::OriginalUiTransitionCompletion50KG OriginalUiOptionsSession50KG::pollCompletion(game::OriginalUiTransitionRoute50KG route,game::OriginalUiTransitionPhase50KG phase) const noexcept {
    const auto* s=storage_.get();
    if(!s||!s->operational()||s->inOperation||route.screen!=12||!s->ownsPage(route.page))return game::OriginalUiTransitionCompletion50KG::unavailable;
    const auto t=s->transition();
    if(!t.bound||t.current.screen!=route.screen||t.current.page!=route.page||t.phase!=phase)return game::OriginalUiTransitionCompletion50KG::unavailable;
    return (route.page==4?s->gameAperture:s->aperture).pollCompletion(route,phase);
}
const game::OriginalUiTransitionOwner50KG* OriginalUiOptionsSession50KG::externalLifecycleOwner() const noexcept {
    return storage_?storage_->cold.externalTransition.get():nullptr;
}
bool OriginalUiOptionsSession50KG::ownsPreparedPage(std::uint8_t page) const noexcept {
    const auto* s=storage_.get();return s&&s->operational()&&!s->inOperation&&s->ownsPage(page);
}
Status OriginalUiOptionsSession50KG::input(const OriginalUiMainSessionInput50KG& in,OriginalUiOptionsSessionInputReceipt50KG& receipt,Diagnostic& d) noexcept {
    d={};receipt={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->updatePrepared||in.controller>1||in.originalInput>15)return result(d,Status::invalidInput);
    s->state.framePublished=false;s->state.backProposalPending=false;s->pendingAcceptCommitted=false;receipt.sourceLifecycle=s->transition();
    receipt.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
    if(!receipt.containerReached||!s->activeAperture().snapshot().inputEnabled268)return result(d,Status::noChange);
    if(s->cold.application->transportE00!=-1)return result(d,Status::unsupportedState,0x267BE0);
    s->inOperation=true;receipt.pageCallbackRan=true;
    if(s->activePage()==4){
        const auto ws=s->bank().gameOptionsInput(in.controller,static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),true,true,receipt.gameOptions,d.widget);
        receipt.pageAccepted=receipt.gameOptions.pageAccepted;Status status=Status::noChange;
        if(!widgetOkay(ws)&&ws!=WidgetStatus::unavailable){s->fault(Status::widget,0x1F50F0);status=Status::widget;}
        else if(receipt.gameOptions.intent==game::OriginalUiGameOptionsIntent50KG::accept&&s->idleInput()&&
            s->canApplyGameOptions(receipt.gameOptions.state.local.soundMode)){
            game::OriginalUiRouteDecision50KG route;
            if(!game::describeOriginalUiGameOptionsAccept50KG(receipt.gameOptions.state.local,
                receipt.gameOptions.rememberedGroup6E8,route))status=Status::unavailableDestination;
            else{
                s->pendingGameProfile=*s->cold.gameSettingsProfile;s->pendingGameLocal=receipt.gameOptions.state.local;
                s->pendingSoundRevision=s->cold.soundMode->revision();
                s->pendingBack=route;s->state.backProposalPending=true;++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;
                receipt.routeProposal={this,s->proposalSerial,route};status=Status::pageProposed;
            }
        }else if(receipt.gameOptions.intent==game::OriginalUiGameOptionsIntent50KG::cancel&&s->idleInput()){
            game::OriginalUiRouteDecision50KG route;route.inputPC=0x1F50F0;route.profileProposalComplete=true;
            route.action=game::OriginalUiRouteAction50KG::unavailable;
            route.blockers=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner);
            route.selectedSlot=receipt.gameOptions.rememberedGroup6E8;
            route.request.kind=game::OriginalUiRouteKind50KG::changePage;route.request.sourceScreen=12;route.request.sourcePage=4;
            route.request.targetScreen=12;route.request.targetPage=1;route.request.sourcePC=0x1F51D8;
            route.profileChanges.count=1;route.profileChanges.values[0]={game::OriginalUiProfileField50KG::optionsGroup6E8,
                game::OriginalUiProfileWrite50KG::word,route.selectedSlot,0xFFFFFFFFu,0x1F51C8};
            s->pendingBack=route;s->state.backProposalPending=true;++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;
            receipt.routeProposal={this,s->proposalSerial,route};status=Status::pageProposed;
        }else if(receipt.gameOptions.intent!=game::OriginalUiGameOptionsIntent50KG::none)status=Status::unavailableDestination;
        else if(receipt.gameOptions.soundRequested)status=s->audioEffect(receipt.gameOptions.soundId)?Status::ok:Status::audio;
        s->inOperation=false;if(s->state.faulted){d=s->callbackFault;status=d.status;}receipt.status=status;return result(d,status);
    }
    game::OriginalUiOptionsContext50KG context;
    context.admission.sourceControllerReady=true;context.admission.selectionOwnerReady=true;
    context.admission.profileOwnerReady=s->cold.profile->known&&s->cold.commonProfile->known;
    context.popupActive=in.popupActive;context.sourceInputBlocked=in.sourceInputBlocked;
    context.secondControllerPresent=in.secondControllerPresent;context.mode34=s->cold.commonProfile->mode34;
    // All destinations are closed here. Back is an UNACCEPTED proposal for the
    // coordinator; this page cannot attest another screen's native readiness.
    const auto ws=s->bank().inputWithPageEffects(in.controller,static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),
        true,true,context,s,&Storage::pageEffects,receipt.page,receipt.movement,d.widget);
    receipt.pageAccepted=receipt.page.pageAccepted;Status status=Status::noChange;
    if(!widgetOkay(ws)&&ws!=WidgetStatus::unavailable){s->fault(Status::widget,0x1F9640);status=Status::widget;}
    else if(receipt.page.routeStatus==game::OriginalUiRouteStatus50KG::unavailable) {
        const auto& route=receipt.page.route;
        status=route.action==game::OriginalUiRouteAction50KG::controllerRequired?Status::secondControllerRequired:Status::unavailableDestination;
        if(in.originalInput==6&&s->idleInput()&&route.profileProposalComplete&&route.request.kind==game::OriginalUiRouteKind50KG::changeScreen&&
            route.request.targetScreen==4&&route.request.targetPage==1&&route.blockers==static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)) {
            s->pendingBack=route;s->state.backProposalPending=true;++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;receipt.routeProposal={this,s->proposalSerial,route};status=Status::backProposed;
        }
        if(in.originalInput==4&&s->idleInput()&&s->ownsPage(4)&&route.selectedSlot==3&&route.profileProposalComplete&&
            route.request.kind==game::OriginalUiRouteKind50KG::changePage&&route.request.targetScreen==12&&route.request.targetPage==4&&
            route.blockers==static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)){
            s->pendingBack=route;s->state.backProposalPending=true;++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;
            receipt.routeProposal={this,s->proposalSerial,route};status=Status::pageProposed;
        }
        // Unavailable subpages and missing real popup are not successful work:
        // retain prospective audio/profile requests, but do not partially apply.
    } else if(receipt.page.soundRequested)status=Status::ok;
    // Actual page-first then selected Movement_Control5 traversal is inside
    // bank.input, including source popup/controller-suppressed page callbacks.
    s->inOperation=false;if(s->state.faulted){d=s->callbackFault;status=d.status;}receipt.status=status;return result(d,status);
}
bool OriginalUiOptionsSession50KG::routeProposalCurrent(const OriginalUiOptionsRouteProposal50KG& proposal) const noexcept {
    const auto& decision=proposal.decision;
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||s->updatePrepared||!s->state.backProposalPending||!s->idleInput()||proposal.source!=this||!proposal.serial||proposal.serial!=s->proposalSerial)return false;
    const auto& expected=s->pendingBack;
    const auto* nav=expected.request.sourcePage==4?&s->cold.resources->widgets()->gameOptionsState()->navigation:s->cold.resources->widgets()->navigation();
    const bool selection=expected.request.sourcePage==4?
        (s->ownsPage(4)&&nav&&nav->controllerGroup[0]==expected.selectedSlot):
        (nav&&nav->controllerGroup[0]==1&&nav->groups[1].selectedSlot==expected.selectedSlot);
    if(!s->cold.profile->known||!s->cold.commonProfile->known||!s->cold.application->known||
        (s->cold.application->globalFlags228&4u)||s->cold.application->transportE00!=-1||
        !selection||s->activePage()!=expected.request.sourcePage||
        !decision.profileProposalComplete||!sameDecision(decision,expected))return false;
    return !game::originalUiGameOptionsAcceptShape50KG(expected)||s->pendingAcceptCurrent();
}
bool OriginalUiOptionsSession50KG::commitGameOptionsAccept(const OriginalUiOptionsRouteProposal50KG& proposal,
    const game::OriginalUiRouteDecision50KG& admitted) noexcept {
    auto* s=storage_.get();
    if(!s||!routeProposalCurrent(proposal)||!game::originalUiGameOptionsAcceptShape50KG(proposal.decision)||
        !game::originalUiGameOptionsAcceptShape50KG(admitted)||!admitted.admitted||admitted.blockers||
        admitted.action!=game::OriginalUiRouteAction50KG::request||!s->ownsPage(1))return false;
    auto original=admitted;original.admitted=false;original.action=game::OriginalUiRouteAction50KG::unavailable;
    original.blockers=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner);
    if(!sameDecision(original,proposal.decision)||!s->pendingAcceptCurrent())return false;
    // Full preflight above. All owners are main-thread-only; no callback,
    // allocation, queue operation or other fallible work may be inserted here.
    auto& profile=*s->cold.gameSettingsProfile;const auto& local=s->pendingGameLocal;
    profile.flags11050=static_cast<std::uint8_t>((profile.flags11050&~1u)|(local.autoSave?1u:0u)); //1F672C
    profile.soundMode1101C=local.soundMode; //1F6744
    profile.volume11020[0]=local.volume[0]; //1F6760
    profile.volume11020[1]=local.volume[1]; //1F677C
    profile.volume11020[2]=local.volume[2]; //1F6798
    profile.volume11020[3]=local.volume[3]; //1F67B4
    profile.flags11050=static_cast<std::uint8_t>((profile.flags11050&~2u)|(local.verticalSplit?2u:0u)); //1F67EC
    profile.flags11050=static_cast<std::uint8_t>((profile.flags11050&~4u)|(local.vibration?4u:0u)); //1F6824
    if(!s->cold.audio->applySoundMode18874050KG(profile.soundMode1101C)){
        // The concrete setter can reject only before its own mutation. This
        // defensive restore preserves all owners if the preflight contract is
        // ever broken; no callback can observe these temporary profile stores.
        profile=s->pendingGameProfile;return false;
    }
    profile.savedGroup6E8=proposal.decision.selectedSlot; //1F5158, AFTER188740
    s->pendingAcceptCommitted=true;++s->state.gameOptionsAcceptCount;
    s->state.lastGameOptionsSoundRevision=s->cold.soundMode->revision();
    return true; // Actual manager request follows, then SELECT1/flush.
}
Status OriginalUiOptionsSession50KG::notifyRouteAccepted(const OriginalUiOptionsRouteProposal50KG& proposal,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(!s->operational()||s->inOperation||s->updatePrepared||!s->state.backProposalPending||proposal.source!=this||
        !proposal.serial||proposal.serial!=s->proposalSerial)return result(d,Status::invalidInput);
    const auto& decision=proposal.decision;const auto& pending=s->pendingBack;
    const auto& r=decision.request;
    if(!sameDecision(decision,pending))return result(d,Status::invalidInput);
    const bool accept=game::originalUiGameOptionsAcceptShape50KG(decision);
    if(!accept&&decision.profileChanges.count!=1)return result(d,Status::invalidInput);
    const auto& p=decision.profileChanges.values[accept?8:0];
    const bool gamePage=r.sourcePage==4;const bool cancel=gamePage&&!accept;
    const bool sameScreen=r.kind==game::OriginalUiRouteKind50KG::changePage;
    const bool shape=accept||(cancel?(sameScreen&&r.sourceScreen==12&&r.targetScreen==12&&r.targetPage==1&&r.sourcePC==0x1F51D8&&
        decision.inputPC==0x1F50F0&&p.field==game::OriginalUiProfileField50KG::optionsGroup6E8&&p.sourcePC==0x1F51C8):
        (r.sourceScreen==12&&r.sourcePage==1&&p.field==game::OriginalUiProfileField50KG::optionsSlot6E4&&
         ((sameScreen&&r.targetScreen==12&&r.targetPage==4&&decision.selectedSlot==3&&r.sourcePC==0x1F9898&&p.sourcePC==0x1F9940)||
          (!sameScreen&&r.targetScreen==4&&r.targetPage==1&&r.sourcePC==0x1F99A4&&p.sourcePC==0x1F9994))));
    const auto t=s->transition();const auto* nav=gamePage?&s->bank().gameOptionsState()->navigation:s->bank().navigation();
    const bool selection=gamePage?(s->ownsPage(4)&&nav&&nav->controllerGroup[0]==decision.selectedSlot):
        (nav&&nav->controllerGroup[0]==1&&nav->groups[1].selectedSlot==decision.selectedSlot);
    const bool profile=accept?s->acceptedGameSettingsCurrent():
        cancel?(s->cold.gameSettingsProfile&&s->cold.gameSettingsProfile->savedGroup6E8==p.value):s->cold.profile->savedSlot6E4==p.value;
    if(!shape||!selection||!profile||!t.bound||t.current.screen!=12||t.current.page!=r.sourcePage||t.requested.screen!=r.targetScreen||t.requested.page!=r.targetPage||
        t.lifecycle!=(sameScreen?game::OriginalUiTransitionLifecycle50KG::requestPageExit:game::OriginalUiTransitionLifecycle50KG::requestScreenExit)||
        t.phase!=game::OriginalUiTransitionPhase50KG::none||t.applicationRequestQueued||t.initialRestartPending||
        !s->cold.profile->known||!s->cold.commonProfile->known||!s->cold.application->known||
        (s->cold.application->globalFlags228&4u)||s->cold.application->transportE00!=-1)return result(d,Status::unsupportedState);
    s->state.framePublished=false;s->state.backProposalPending=false;s->inOperation=true;
    const bool okay=s->audioEffect(cancel||!sameScreen?5:1);s->inOperation=false;
    // Accept1F51A0 reaches267BE0 only after SELECT1F5188. The preflighted
    // shared transportE00=-1 takes its exact no-work return. No device drain,
    // fabricated active-transport success or extra PCM advance is substituted.
    if(!okay){d=s->callbackFault;return d.status;}return result(d,Status::ok);
}
Status OriginalUiOptionsSession50KG::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG& in,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation)return result(d,Status::invalidInput);
    if(s->updatePrepared){s->fault(Status::unsupportedState,0x234440);d=s->callbackFault;return d.status;}
    if(s->inOperation||!s->currentPage()||!std::isfinite(in.elapsedSeconds)||in.elapsedSeconds<0||in.elapsedSeconds>1||
        (in.paused&&in.elapsedSeconds!=0))return result(d,Status::invalidInput);
    if((s->cold.application->globalFlags228&4u)||s->cold.application->transportE00!=-1)return result(d,Status::unsupportedState);
    s->state.framePublished=false;s->state.backProposalPending=false;s->inOperation=true;s->paused=in.paused;
    if(s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled){s->cold.audio->pause(in.paused);
        if(!s->cold.audio->update(in.elapsedSeconds))s->fault(Status::audio,0x234440);}
    s->inOperation=false;if(s->state.faulted){d=s->callbackFault;return d.status;}
    s->preparedUpdate=in;s->updatePrepared=true;return result(d,Status::ok);
}
Status OriginalUiOptionsSession50KG::update(const OriginalUiMainSessionUpdate50KG& in,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||!s->currentPage())return result(d,Status::invalidInput);
    if(!s->updatePrepared||in.elapsedSeconds!=s->preparedUpdate.elapsedSeconds||in.paused!=s->preparedUpdate.paused||
        in.debugChordHeld!=s->preparedUpdate.debugChordHeld){s->fault(Status::unsupportedState,0x2A3C80);d=s->callbackFault;return d.status;}
    s->updatePrepared=false;s->state.backProposalPending=false;s->inOperation=true;
    const auto* order=s->bank().order();if(!s->state.faulted)s->updateWidgets(order->ordinaryWidgets);
    if(!s->state.faulted&&s->state.pageShown&&(s->activePage()==4?s->gameAperture.saveGameOptionsUpdateDelta38(in.elapsedSeconds):s->aperture.saveOptionsMenuUpdateDelta38(in.elapsedSeconds))!=ApertureStatus::ok)
        s->fault(Status::aperture,s->activePage()==4?0x1F5BE4:0x1F9A74);
    if(!s->state.faulted)s->updateWidgets(order->onTopWidgets);
    // Options1D0DA0 attaches scene siblings after Pod widgets: particle manager,
    // borrowed stationary manager camera, then actual Type8 fire emitter.
    auto& fire=*s->cold.resources->fire();
    if(!s->state.faulted){s->state.lastParticleAdvance=fire.advance(in.elapsedSeconds,in.paused);
        if(!s->state.lastParticleAdvance.okay())s->fault(Status::particle,0x160E40);}
    if(!s->state.faulted){s->state.lastFireEmit=fire.emit(in.elapsedSeconds,in.paused,s->cold.application->rawRandom308030,s->cold.application->fireSerial433970);
        if(!s->state.lastFireEmit.okay())s->fault(Status::particle,0x15AEA0);}
    s->inOperation=false;if(s->state.faulted){d=s->callbackFault;return d.status;}
    ++s->state.updateCount;return result(d,Status::ok);
}
Status OriginalUiOptionsSession50KG::render(const OriginalUiOptionsClock50KG& clock,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->inOperation)return result(d,Status::invalidInput);
    s->state.framePublished=false;
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->updatePrepared){s->fault(Status::unsupportedState,0x234440);d=s->callbackFault;return d.status;}
    if(!s->currentPage()||!s->state.updateCount||!s->state.cameraActive||(s->paused&&clock.advanceAnimations))return result(d,Status::invalidInput);
    s->state.backProposalPending=false;s->inOperation=true;s->rendering=true;
    bool okay=s->collector.beginPage(s->activePage(),clock,d.draw)&&s->collector.projectOrdinaryWidgets(true,d.draw)&&s->collector.projectPage(s->state.pageShown,d.draw);
    if(okay)okay=apertureOkay(s->activeAperture().render48(s->transition().lifecycle));
    okay=okay&&!s->state.faulted;
    if(okay)okay=s->collector.projectPrompts(d.draw)&&s->collector.projectOnTopWidgets(true,d.draw)&&s->collector.projectParticles(d.draw)&&s->collector.drain(d.draw);
    s->rendering=false;s->inOperation=false;
    if(!okay){if(s->state.faulted){d=s->callbackFault;return d.status;}return result(d,Status::draw);}
    ++s->state.renderCount;s->state.framePublished=true;return result(d,Status::ok);
}
OriginalUiOptionsSessionSnapshot50KG OriginalUiOptionsSession50KG::snapshot() const noexcept {
    if(!storage_)return {};auto out=storage_->state;out.transition=storage_->transition();out.aperture=storage_->activeAperture().snapshot();
    out.promptMask42C=storage_->cold.resources->widgets()->pageInputFlags42C();return out;
}
const OriginalUiNativeDraw50KG* OriginalUiOptionsSession50KG::draw() const noexcept {
    return storage_&&storage_->state.framePublished&&!storage_->state.faulted&&!storage_->state.quitRequested&&
        !storage_->inOperation&&!storage_->updatePrepared&&storage_->currentPage()?storage_->collector.draw():nullptr;
}
void OriginalUiOptionsSession50KG::requestQuit() noexcept {
    if(!storage_||storage_->inOperation)return;storage_->state.quitRequested=true;storage_->state.framePublished=false;
}
}

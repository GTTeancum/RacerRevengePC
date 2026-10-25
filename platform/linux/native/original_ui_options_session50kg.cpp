#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_options_session50kg.h"
#include "original_ui_controller_popup_resources50kg.h"
#include <cmath>
#include <exception>
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
bool sameCommonHall(const OriginalUiMainProfile50KG& a,const OriginalUiMainProfile50KG& b) noexcept {
 return a.known==b.known&&a.mode34==b.mode34&&a.previousMode38==b.previousMode38&&a.savedSlot6E0==b.savedSlot6E0&&a.availability11018==b.availability11018&&a.flags11054==b.flags11054&&a.playerBits1125C==b.playerBits1125C;
}
bool hallRequest(const game::OriginalUiRouteDecision50KG& d) noexcept {return d.inputPC==0x1F9640&&d.selectedSlot==4&&d.request.kind==game::OriginalUiRouteKind50KG::changeScreen&&d.request.sourceScreen==12&&d.request.sourcePage==1&&d.request.targetScreen==11&&d.request.targetPage==1&&d.request.sourcePC==0x1F98C8;}
// Source1F9878 is an owned dialog operation, never a page3 admission.
bool controllerPopupRequest(const game::OriginalUiRouteDecision50KG& d) noexcept {
    using namespace game;
    if(d.action!=OriginalUiRouteAction50KG::controllerRequired||d.admitted||!d.profileProposalComplete||
       d.inputPC!=0x1F9640||d.selectedSlot!=2||d.request.kind!=OriginalUiRouteKind50KG::changePage||
       d.request.sourceScreen!=12||d.request.sourcePage!=1||d.request.targetScreen!=12||d.request.targetPage!=3||
       d.request.sourcePC!=0x1F9744||d.profileChanges.count!=1||
       d.blockers!=(static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::controllerPresence)|
                    static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner)))return false;
    const auto&w=d.profileChanges.values[0];
    return w.field==OriginalUiProfileField50KG::optionsSlot6E4&&w.operation==OriginalUiProfileWrite50KG::word&&
        w.value==2&&w.replaceMask==0xFFFFFFFFu&&w.sourcePC==0x1F9940;
}
bool controls2EntryRequest(const game::OriginalUiRouteDecision50KG& d) noexcept {
    using namespace game;
    if(d.action!=OriginalUiRouteAction50KG::unavailable||d.admitted||!d.profileProposalComplete||
       d.inputPC!=0x1F9640||d.selectedSlot!=2||d.request.kind!=OriginalUiRouteKind50KG::changePage||
       d.request.sourceScreen!=12||d.request.sourcePage!=1||d.request.targetScreen!=12||d.request.targetPage!=3||
       d.request.sourcePC!=0x1F9744||d.profileChanges.count!=1||
       d.blockers!=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner))return false;
    const auto& w=d.profileChanges.values[0];
    return w.field==OriginalUiProfileField50KG::optionsSlot6E4&&w.operation==OriginalUiProfileWrite50KG::word&&
        w.value==2&&w.replaceMask==0xFFFFFFFFu&&w.sourcePC==0x1F9940;
}
bool sameNavigation(const game::OriginalUiNavigation50KG& a,const game::OriginalUiNavigation50KG& b) noexcept {
    if(a.page!=b.page||a.controllerGroup!=b.controllerGroup)return false;
    for(std::size_t i=0;i<a.groups.size();++i)
        if(a.groups[i].enabled!=b.groups[i].enabled||a.groups[i].selectedSlot!=b.groups[i].selectedSlot)return false;
    for(std::size_t i=0;i<a.controls.size();++i){const auto& x=a.controls[i];const auto& y=b.controls[i];
        if(x.enabled!=y.enabled||x.blocked!=y.blocked||x.selected!=y.selected||x.shown!=y.shown)return false;}
    return true;
}
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
    game::OriginalUiApertureOwner50KG aperture,gameAperture,creditsAperture,memoryCardAperture;
    bool memoryCardPrepared=false;
    game::OriginalUiMemoryCardPlan50KG pendingMemoryCard;
    bool gamePrepared=false,creditsPrepared=false,controlsRegistryPublished=false;
    game::OriginalUiCreditsPlan50KG pendingCredits;
    std::array<game::OriginalUiApertureOwner50KG,2> controlsAperture;
    std::array<game::OriginalUiControlsController50KG,2> controlsController;
    std::array<game::OriginalUiControlsState50KG,2> controlsState;
    std::array<bool,2> controlsPrepared{};
    game::ControlPresets49D pendingControlsBefore{},pendingControlsAfter{};
    game::OriginalUiControlsState50KG pendingControlsLocal{};
    bool pendingControlsCommitted=false;
    // A pending input snapshot is distinct from the later actual manager-entry
    // receipt. Neither is a device reservation or a completed render receipt.
    std::uint64_t pendingControls2Poll=0,controls2EntrySerial=0,controls2EntryPoll=0;
    OriginalUiOptionsProfile50KG pendingControls2Options{};
    OriginalUiMainProfile50KG pendingControls2Common{};
    game::ControlPresets49D pendingControls2Presets{};
    game::OriginalUiControlsState50KG pendingControls2Local{};
    game::OriginalUiNavigation50KG pendingControls2Navigation{};
    struct ControlsExecution {const game::OriginalUiControlsPlan50KG* plan=nullptr;bool stateCommitted=false;};
    ControlsExecution* controlsExecution=nullptr;
    OriginalUiOptionsSessionSnapshot50KG state;
    Diagnostic callbackFault;
    game::OriginalUiRouteDecision50KG pendingBack{};
    OriginalUiMainProfile50KG pendingHallCommon{};
    game::OriginalUiGameSettingsProfile50KG pendingGameProfile{};
    game::OriginalUiGameSettings50KG pendingGameLocal{};
    std::uint64_t pendingSoundRevision=0;
    bool pendingAcceptCommitted=false;
    std::uint64_t proposalSerial=0;
    WindowControllerPresence50DQ popupPresence=WindowControllerPresence50DQ::Unknown;
    bool popupOpenedByInput=false,popupAttached=false;
    bool dependenciesPrepared=false,inOperation=false,rendering=false,paused=false,updatePrepared=false,sourceClosing=false,sourceTornDown=false;
    OriginalUiMainSessionUpdate50KG preparedUpdate{};
    game::OriginalUiOptionsMenuWidgets50KG& bank() noexcept {return *cold.resources->widgets();}
    bool operational() const noexcept {return dependenciesPrepared&&!state.faulted&&!state.quitRequested&&!sourceClosing;}
    void fault(Status s,std::uint32_t pc) noexcept {if(!state.faulted){state.faulted=true;callbackFault.status=s;callbackFault.sourcePC=pc;}}
    game::OriginalUiTransitionSnapshot50KG transition() const noexcept {return cold.externalTransition->snapshot();}
    std::uint8_t activePage() const noexcept {return transition().current.page;}
    static bool isControlsPage(std::uint8_t page) noexcept {return page==2||page==3;}
    game::OriginalUiApertureOwner50KG& apertureFor(std::uint8_t page) noexcept {return isControlsPage(page)?controlsAperture[page-2]:page==5?memoryCardAperture:page==6?creditsAperture:page==4?gameAperture:aperture;}
    const game::OriginalUiApertureOwner50KG& apertureFor(std::uint8_t page) const noexcept {return isControlsPage(page)?controlsAperture[page-2]:page==5?memoryCardAperture:page==6?creditsAperture:page==4?gameAperture:aperture;}
    game::OriginalUiApertureOwner50KG& activeAperture() noexcept {return apertureFor(activePage());}
    const game::OriginalUiApertureOwner50KG& activeAperture() const noexcept {return apertureFor(activePage());}
    bool ownsPage(std::uint8_t page) const noexcept {return page==1||
        (page==5&&memoryCardPrepared&&cold.resources->memoryCardReady())||
        (page==6&&creditsPrepared&&cold.commonProfile->known&&cold.commonProfile->mode34!=2)||
        (isControlsPage(page)&&controlsPrepared[page-2]&&cold.controlsProfile&&cold.resources->controlsModelReady())||
        (page==4&&gamePrepared&&cold.gameSettingsProfile&&game::validOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile));}
    bool memoryCardPlan(const game::OriginalUiMemoryCardPlan50KG& plan) noexcept {
        if(!widgetOkay(bank().preflightMemoryCardPlan(plan,callbackFault.widget)))return false;
        if(plan.soundRequested&&(plan.soundFlags!=1||plan.soundForce||!plan.flushRequested||cold.application->transportE00!=-1))return false;
        if(plan.soundRequested&&cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
           (!cold.audio||!cold.audio->canPlayOptionsEffect(plan.soundId)))return false;
        if(plan.activateBorrowedCamera&&!cold.resources->camera())return false;
        if(!widgetOkay(bank().commitMemoryCardPlan(plan,callbackFault.widget)))return false;
        if(plan.activateBorrowedCamera)state.cameraActive=true;
        // Actual APP->UI+47C, not the unrelated application UI scene field.
        if(plan.resetGlobalPopupResult3D8)cold.application->controllerPopupState.result3D8=0;
        if(plan.applyPageSceneField430){const auto* page=bank().memoryCardPage();if(!page)return false;
            std::uint32_t packed=0;for(unsigned c=0;c<3;++c){std::int32_t v=0;
                if(!game::convertOriginalUiApertureCvtWS50KG(page->backgroundColor[c],v)||v<0||v>255)return false;
                packed|=std::uint32_t(v)<<(16-8*c);}state.packedBackground430=packed;}
        if(plan.saveUpdateDelta298&&!apertureOkay(memoryCardAperture.saveMemoryCardUpdateDelta38(plan.state.updateDelta298)))return false;
        return !plan.soundRequested||audioEffect(plan.soundId);
    }
    bool stageMemoryCardReturn(const game::OriginalUiMemoryCardPlan50KG& plan) noexcept {
        game::OriginalUiRouteDecision50KG decision;
        if(!idleInput()||!ownsPage(1)||!game::describeOriginalUiMemoryCardReturn50KG(plan,decision)||
           !game::originalUiMemoryCardReturnShape50KG(decision))return false;
        pendingMemoryCard=plan;pendingBack=decision;state.backProposalPending=true;
        ++proposalSerial;if(!proposalSerial)++proposalSerial;return true;
    }
    bool memoryCardReturnCurrent() const noexcept {
        const auto* current=cold.resources->widgets()->memoryCardState();
        return current&&game::sameOriginalUiMemoryCardState50KG(*current,pendingMemoryCard.state);
    }
    bool creditsPlan(const game::OriginalUiCreditsPlan50KG& plan) noexcept {
        if(!widgetOkay(bank().preflightCreditsPlan(plan,callbackFault.widget)))return false;
        if(cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
           ((plan.startMusic12&&(!cold.audio||!cold.audio->canStartCreditsLoop12()))||
            (plan.fadeMusic&&(!cold.audio||!cold.audio->canFadeCreditsMusic()))))return false;
        if(!widgetOkay(bank().commitCreditsPlan(plan,callbackFault.widget)))return false;
        if(plan.activateBorrowedCamera){if(!cold.resources->camera())return false;state.cameraActive=true;}
        if(plan.clearSceneField3D8)cold.application->uiObject3D8=0;
        if(plan.applyPageSceneField430){const auto* page=bank().creditsPage();if(!page)return false;
            std::uint32_t packed=0;for(unsigned c=0;c<3;++c){std::int32_t v=0;
                if(!game::convertOriginalUiApertureCvtWS50KG(page->backgroundColor[c],v)||v<0||v>255)return false;
                packed|=std::uint32_t(v)<<(16-8*c);}state.packedBackground430=packed;}
        if(plan.startMusic12||plan.fadeMusic){
            if(cold.audioMode==OriginalUiMainAudioMode50KG::muted)++state.audio.suppressedByMutedMode;
            else {if(!(plan.startMusic12?cold.audio->startCreditsLoop12():cold.audio->fadeCreditsMusic()))return false;
                ++state.audio.submitted;}}
        if(plan.saveUpdateDelta298&&!apertureOkay(creditsAperture.saveCreditsUpdateDelta38(plan.updateDelta298)))return false;
        return true;
    }
    bool stageCreditsReturn(const game::OriginalUiCreditsPlan50KG& plan) noexcept {
        game::OriginalUiRouteDecision50KG decision;
        if(!idleInput()||!game::describeOriginalUiCreditsReturn50KG(plan,decision)||
           !game::originalUiCreditsReturnShape50KG(decision)||!ownsPage(1))return false;
        pendingCredits=plan;pendingBack=decision;state.backProposalPending=true;
        ++proposalSerial;if(!proposalSerial)++proposalSerial;return true;
    }
    bool creditsReturnCurrent() const noexcept {
        const auto* current=cold.resources->widgets()->creditsState();const auto& expected=pendingCredits.state;
        if(!current||current->navigation.page!=expected.navigation.page||current->loaded!=expected.loaded||
           current->phase7D4!=expected.phase7D4||current->remaining7D8!=expected.remaining7D8||
           current->inputFlags42C!=expected.inputFlags42C||current->navigation.controllerGroup!=expected.navigation.controllerGroup)return false;
        for(std::size_t i=0;i<current->navigation.groups.size();++i){const auto& a=current->navigation.groups[i];const auto& b=expected.navigation.groups[i];
            if(a.enabled!=b.enabled||a.selectedSlot!=b.selectedSlot)return false;}
        for(std::size_t i=0;i<current->navigation.controls.size();++i){const auto& a=current->navigation.controls[i];const auto& b=expected.navigation.controls[i];
            if(a.enabled!=b.enabled||a.blocked!=b.blocked||a.selected!=b.selected||a.shown!=b.shown)return false;}
        return cold.audioMode==OriginalUiMainAudioMode50KG::muted||!pendingCredits.soundRequested||
            (cold.audio&&cold.audio->canPlayGameOptionsEffect(pendingCredits.soundId));
    }
    bool sameControlsLocal(const game::OriginalUiControlsState50KG& a,const game::OriginalUiControlsState50KG& b) const noexcept {
        return a.navigation.page==b.navigation.page&&a.controllerIndex4DC==b.controllerIndex4DC&&
            a.frontView4CC==b.frontView4CC&&a.modelBlocked4D0==b.modelBlocked4D0&&a.loaded==b.loaded&&
            a.configuration4D4==b.configuration4D4&&a.preset4D8==b.preset4D8;
    }
    bool controlsSoundReady(std::uint32_t id) const noexcept {
        return cold.audioMode==OriginalUiMainAudioMode50KG::muted||
            (cold.audio&&cold.audio->canPlayControlsEffect(id));
    }
    bool pendingControlsCurrent() const noexcept {
        const auto page=pendingBack.request.sourcePage;
        return !pendingControlsCommitted&&isControlsPage(page)&&ownsPage(page)&&
            sameControlsLocal(controlsState[page-2],pendingControlsLocal)&&
            game::sameOriginalUiControlsPresets50KG(*cold.controlsProfile,pendingControlsBefore)&&
            controlsSoundReady(pendingBack.request.sourcePC==0x1F39AC?1:5)&&
            (pendingBack.request.sourcePC!=0x1F39AC||state.controlsAcceptCount!=std::numeric_limits<std::uint64_t>::max());
    }
    static bool controlsEffect(void* opaque,const game::OriginalUiControlsEffect50KG& e) noexcept {
        auto& s=*static_cast<Storage*>(opaque);if(!s.controlsExecution||!s.controlsExecution->plan)return false;
        const auto& plan=*s.controlsExecution->plan;const auto index=plan.state.controllerIndex4DC;
        using Kind=game::OriginalUiControlsEffectKind50KG;using Scene=game::OriginalUiControlsSceneAction50KG;
        switch(e.kind){
        case Kind::model:return s.cold.resources->applyControlsModelEffect(e,s.callbackFault.draw.resource);
        case Kind::scene:
            switch(e.sceneAction){
            case Scene::activateBorrowedCamera:
                if(!s.cold.resources->camera())return false;s.state.cameraActive=true;return true;
            case Scene::clearSceneField3D8:s.cold.application->uiObject3D8=0;return true;
            case Scene::applyPageSceneField430:{
                const auto* page=s.controlsController[index].page();if(!page)return false;
                std::uint32_t packed=0;for(unsigned c=0;c<3;++c){std::int32_t v=0;
                    if(!game::convertOriginalUiApertureCvtWS50KG(page->backgroundColor[c],v)||v<0||v>255)return false;
                    packed|=std::uint32_t(v)<<(16-8*c);}s.state.packedBackground430=packed;return true;}
            case Scene::routeToSecondController:s.cold.application->courseUi474=0;s.cold.application->courseUi478=1;return true;
            case Scene::routeToFirstController:s.cold.application->courseUi474=1;s.cold.application->courseUi478=0;return true;
            }return false;
        case Kind::previewPresets:
            if(!s.cold.controlsProfile||!plan.previewPerformed||
                !game::sameOriginalUiControlsPresets50KG(*s.cold.controlsProfile,plan.previewBefore))return false;
            //Replay the real temporary slot0 select/restore on the one live
            //owner, after preflight. Source1F3F3C observes that helper's row;
            //the ordered text effects already hold the same deterministic row.
            (void)game::previewControlPreset49D(*s.cold.controlsProfile,plan.state.preset4D8);
            return game::sameOriginalUiControlsPresets50KG(*s.cold.controlsProfile,plan.previewAfter);
        case Kind::drawBackground:return s.rendering&&s.collector.projectControlsBackground(e.ordinal,s.callbackFault.draw);
        case Kind::renderAperture:
            if(!s.rendering||index>=2)return false;
            //1F3D94 may synchronously call B0/B8. Publish the preceding source
            //state now, so those nested phase writes survive the render plan.
            s.controlsState[index]=plan.state;s.controlsExecution->stateCommitted=true;
            return apertureOkay(s.controlsAperture[index].render48(s.transition().lifecycle));
        case Kind::widget:
            //The bank commits the shared visual flag first, then the actual
            //Widget36 Soultree reset/select/deselect/clear callback reaches us.
            return e.widgetId==36&&s.cold.resources->applyControlsModelEffect(e,s.callbackFault.draw.resource);
        case Kind::localizedText:return false; //Handled by the concrete widget bank.
        }return false;
    }
    bool applyControlsPlan(const game::OriginalUiControlsPlan50KG& plan) noexcept {
        const auto index=plan.state.controllerIndex4DC;
        if(index>=2||!cold.controlsProfile||
            !widgetOkay(bank().preflightControlsPlan(plan,callbackFault.widget))||
            (plan.previewPerformed&&!game::sameOriginalUiControlsPresets50KG(*cold.controlsProfile,plan.previewBefore)))return false;
        ControlsExecution execution{&plan,false};auto* previous=controlsExecution;controlsExecution=&execution;
        const bool okay=widgetOkay(bank().commitControlsPlan(plan,this,&Storage::controlsEffect,callbackFault.widget));
        controlsExecution=previous;
        if(okay&&!execution.stateCommitted)controlsState[index]=plan.state;
        return okay;
    }
    void controlsPhase(game::OriginalUiControlsPhase50KG phase,std::uint8_t page,std::uint32_t pc) noexcept {
        if(!ownsPage(page)){fault(Status::unsupportedState,pc);return;}
        game::OriginalUiControlsPlan50KG plan;
        if(controlsController[page-2].phase(controlsState[page-2],phase,*cold.controlsProfile,plan)!=game::OriginalUiOptionsMenuStatus50KG::ok||
            !applyControlsPlan(plan))fault(Status::widget,pc);
    }
    bool currentPage() const noexcept {const auto t=transition();return t.bound&&t.current.screen==12&&ownsPage(t.current.page);}
    bool idleInput() const noexcept {const auto t=transition();return currentPage()&&t.lifecycle==game::OriginalUiTransitionLifecycle50KG::idle&&
        t.phase==game::OriginalUiTransitionPhase50KG::none&&!t.initialRestartPending&&t.requested.screen==12&&t.requested.page==t.current.page&&!t.applicationRequestQueued&&state.pageShown&&state.inputContainerShown&&
        activeAperture().snapshot().inputEnabled268&&activeAperture().snapshot().state==game::OriginalUiApertureState50KG::open;}
    bool canApplyGameOptions(std::uint32_t mode) const noexcept {
        return cold.gameSettingsProfile&&game::validOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile)&&
            cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&cold.audio&&cold.soundMode&&
            canAcceptOriginalUiAudioOptions50KG(*cold.gameSettingsProfile,*cold.audio,*cold.soundMode,mode)&&
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
        if(page==5){game::OriginalUiMemoryCardPlan50KG plan;
            if(!ownsPage(5)||game::OriginalUiMemoryCard50KG::phase(*bank().memoryCardState(),p,plan)!=game::OriginalUiMemoryCardStatus50KG::ok||
               !memoryCardPlan(plan))fault(Status::widget,pc);return;}
        if(page==6){game::OriginalUiCreditsPlan50KG plan;
            if(!ownsPage(6)||bank().creditsController()->phase(*bank().creditsState(),p,cold.commonProfile->known,
                cold.commonProfile->mode34,plan)!=game::OriginalUiOptionsMenuStatus50KG::ok||!creditsPlan(plan))fault(Status::widget,pc);
            return;}
        if(isControlsPage(page)){
            game::OriginalUiControlsPhase50KG phase;
            switch(p){
            case Phase::requestedPage90:phase=game::OriginalUiControlsPhase50KG::requestedPage90;break;
            case Phase::previousPage94:phase=game::OriginalUiControlsPhase50KG::previousPage94;break;
            case Phase::entry74:phase=game::OriginalUiControlsPhase50KG::entry74;break;
            case Phase::openingAC:phase=game::OriginalUiControlsPhase50KG::openingAC;break;
            case Phase::openedB0:phase=game::OriginalUiControlsPhase50KG::openedB0;break;
            case Phase::closingB4:phase=game::OriginalUiControlsPhase50KG::closingB4;break;
            default:fault(Status::unsupportedState,pc);return;}
            controlsPhase(phase,page,pc);return;
        }
        if(page==4){if(!ownsPage(4)){fault(Status::unsupportedState,pc);return;}
            if(!widgetOkay(bank().gameOptionsPhase(p,*cold.gameSettingsProfile,callbackFault.widget)))fault(Status::widget,pc);return;}
        if(!cold.profile->known||cold.profile->savedSlot6E4<1||cold.profile->savedSlot6E4>6){fault(Status::unsupportedState,pc);return;}
        if(!widgetOkay(bank().phase(p,cold.profile->known,static_cast<std::uint16_t>(cold.profile->savedSlot6E4),callbackFault.widget)))fault(Status::widget,pc);
    }
    bool audioEffect(std::uint32_t id) noexcept {
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return true;}
        if(!cold.audio||!cold.audio->ready()||!(isControlsPage(activePage())?cold.audio->playControlsEffect(id):activePage()==4?cold.audio->playGameOptionsEffect(id):cold.audio->playOptionsEffect(id))){++state.audio.failed;fault(Status::audio,0x292360);return false;}
        ++state.audio.submitted;return true;
    }
    static bool pageEffects(void* p,const game::OriginalUiOptionsMenuPageResult50KG& result) noexcept {
        auto& s=*static_cast<Storage*>(p);
        // Source1F9878 owns a dialog, not the unavailable page3 destination.
        // Keep popup -> remembered slot -> sound/flush inside the page callback,
        // before the later selected Movement_Control traversal.
        if(controllerPopupRequest(result.route)&&s.popupPresence==WindowControllerPresence50DQ::Absent&&
           s.idleInput()&&s.activePage()==1&&!s.cold.application->controllerPopupState.active3CC){
            const auto owner=s.cold.resources->controllerPopup();
            if(!owner||!owner->loaded()||owner->poisoned()||owner!=s.cold.application->controllerPopupResources)return true;
            if(s.cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
               (!s.cold.audio||!s.cold.audio->canPlayOptionsEffect(1)))return true;
            game::OriginalUiControllerPopupPlan50KG plan;
            if(game::OriginalUiControllerPopup50KG::startController2(s.cold.application->controllerPopupState,plan)!=
               game::OriginalUiControllerPopupStatus50KG::ok)return false;
            s.cold.application->controllerPopupState=plan.state;
            s.cold.profile->savedSlot6E4=2;
            if(!s.audioEffect(1))return false;
            s.popupOpenedByInput=true;return true;
        }
        // Other unavailable routes still defer effects until real admission.
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
            if(isControlsPage(s.activePage())){
                if(event.targetPC!=0x1F45E0u)s.fault(Status::unsupportedState,event.callPC);
                else s.controlsPhase(game::OriginalUiControlsPhase50KG::closingThresholdB8,s.activePage(),event.callPC);
            }else if(event.targetPC!=(s.activePage()==5?0x1D79F0u:s.activePage()==6?0x1F8980u:s.activePage()==4?0x1F5F30u:0x1F9FE0u))s.fault(Status::unsupportedState,event.callPC);break;
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
    bool updateWidgets(const std::vector<std::uint16_t>& ids,float elapsed) noexcept {
        for(auto id:ids){
            if(id==36&&bank().widgetState(id).shown){
                if(!cold.resources->advanceControlsModel(elapsed,callbackFault.draw.resource)){fault(Status::widget,0x154480);return false;}
            }else if(bank().widgetState(id).kind==game::OriginalUiOptionsWidgetKind50KG::text)
                if(!widgetOkay(bank().updateText(id,true,callbackFault.widget))){fault(Status::widget,0x141ED0);return false;}
        }return true;
    }
};
OriginalUiOptionsSession50KG::OriginalUiOptionsSession50KG() noexcept=default;
OriginalUiOptionsSession50KG::~OriginalUiOptionsSession50KG(){
    // Shared retains this Session facade as part of its exact invocation.
    // Leaking only Storage cannot preserve the facade. Refuse destruction
    // until actual explicit retirement instead of leaving a dangling authority.
    if(storage_&&storage_->collector.menuShared_&&storage_->collector.sourceTeardownPending())std::terminate();
    // Existing non-Shared fallback retains backing after failed explicit close.
    // Destruction is not evidence of submission retirement or facade lifetime.
    if(sourceTeardownPending())storage_.release();
}
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
        if(cold.controlsProfile&&cold.resources->controlsModelReady())for(std::uint8_t index=0;index<2;++index){
            auto* exit=cold.resources->controlsExitText(index);
            if(!exit||!exit->prepared()||s->controlsController[index].bind(cold.optionsArchive,index,d.widget.parse)!=game::OriginalUiOptionsMenuStatus50KG::ok||
                !s->controlsController[index].initialState(s->controlsState[index])||
                s->controlsAperture[index].bindControls(cold.optionsArchive,std::uint8_t(index+2),callbacks,d.aperture)!=ApertureStatus::ok)
                return result(d,Status::resource,0x1F2F60);
            s->controlsPrepared[index]=true;
        }
        if(cold.resources->memoryCardReady()){
            if(!cold.resources->memoryCardExitText()||!cold.resources->memoryCardExitText()->prepared()||
               s->memoryCardAperture.bindMemoryCard(cold.optionsArchive,callbacks,d.aperture)!=ApertureStatus::ok)return result(d,Status::aperture);
            s->memoryCardPrepared=true;
        }
        if(cold.resources->widgets()->creditsPage()&&cold.resources->creditsExitText()&&
           cold.resources->creditsExitText()->prepared()&&
           (cold.audioMode==OriginalUiMainAudioMode50KG::muted||(cold.audio&&cold.audio->creditsMenuReady()))){
            if(s->creditsAperture.bindCredits(cold.optionsArchive,callbacks,d.aperture)!=ApertureStatus::ok)return result(d,Status::aperture);
            s->creditsPrepared=true;
        }
        // No manager bind, tick, widget phase, shared-profile write or music
        // start during this cold prepare. Coordinator owns the initial notices.
        s->state.bound=true;storage_=std::move(s);
        if(const auto shared=storage_->cold.application->sharedDialogs){
            if(!storage_->collector.bindSharedMenu470(*this,shared)){
                storage_->fault(Status::draw,0x204A00);d=storage_->callbackFault;return d.status;
            }
        }
        return result(d,Status::ok);
    }catch(const std::bad_alloc&){return result(d,Status::allocation);}catch(...){return result(d,Status::resource);}
}
Status OriginalUiOptionsSession50KG::notifyTransition(const game::OriginalUiTransitionNotification50KG& event,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
    if(!s->operational()||s->inOperation||event.route.screen!=12||!s->ownsPage(event.route.page))return result(d,Status::unsupportedState,event.sourcePC);
    // Prepared page ownership alone cannot manufacture a Controls2 lifecycle.
    // Require the actual committed entry receipt before ANY page3 callback,
    // including requested90/entry74 camera and widget effects.
    if(event.route.page==3&&(!s->controls2EntrySerial||!s->controls2EntryPoll))
        return result(d,Status::unsupportedState,event.sourcePC);
    const auto t=s->transition();
    if(!t.bound||event.lifecycle!=t.lifecycle||event.phase!=t.phase||
        !((t.current.screen==12&&t.current.page==event.route.page)||(t.requested.screen==12&&t.requested.page==event.route.page)))return result(d,Status::invalidInput,event.sourcePC);
    s->state.framePublished=false;s->state.backProposalPending=false;s->inOperation=true;
    switch(event.kind) {
    case Notice::requestedPage90:s->widgetPhase(Phase::requestedPage90,event.sourcePC,event.route.page);s->state.pageShown=true;break;
    case Notice::previousPage94:
        s->widgetPhase(Phase::previousPage94,event.sourcePC,event.route.page);s->state.pageShown=false;
        // The old page's final frame must already have been accepted/retired:
        // notifyTransition rejects any pending shared invocation above. B8 is
        // deliberately NOT this boundary, and cannot revoke frame custody.
        if(event.route.page==3){s->controls2EntrySerial=0;s->controls2EntryPoll=0;}
        break;
    case Notice::pageEntry74:
        if(!s->popupAttached){
            // Original attachment happens during loading before manager entry.
            // Native cold preparation is prospective: publish it here only for
            // the inactive retained owner, before any page entry callback.
            if(s->cold.application->controllerPopupState.active3CC){s->fault(Status::unsupportedState,0x207330);break;}
            if(const auto shared=s->cold.application->sharedDialogs){OriginalUiSharedDialogs50KG::Diagnostic sd;
                if(!shared->reappendOptionsEntry1B86D0(sd)){s->fault(Status::unsupportedState,sd.sourcePC);break;}
            }else{
                game::OriginalUiControllerPopupPlan50KG plan;
                if(game::OriginalUiControllerPopup50KG::attach(s->cold.application->controllerPopupState,plan)!=
                   game::OriginalUiControllerPopupStatus50KG::ok){s->fault(Status::unsupportedState,0x207330);break;}
                s->cold.application->controllerPopupState=plan.state;
            }
            s->popupAttached=true;
        }
        if(event.route.page==1){
            game::OriginalUiControllerPopupPlan50KG plan;
            if(game::OriginalUiControllerPopup50KG::optionsEntryResetResult(s->cold.application->controllerPopupState,plan)!=
               game::OriginalUiControllerPopupStatus50KG::ok){s->fault(Status::unsupportedState,0x1F9614);break;}
            s->cold.application->controllerPopupState=plan.state;
        }
        if(!s->controlsRegistryPublished&&s->cold.resources->controlsModelReady()){
            if(!s->cold.resources->publishControlsRegistry(s->callbackFault.draw.resource)){s->fault(Status::resource,0x1CDB10);break;}
            s->controlsRegistryPublished=true;
        }
        s->widgetPhase(Phase::entry74,event.sourcePC,event.route.page);
        // Page+C4 aliases the one real manager camera(+2B0); entry enables and
        // registers that camera. Renderer uses its authenticated prepared pose.
        s->state.cameraActive=true;if(event.route.page!=5)s->cold.application->uiObject3D8=0;break;
    case Notice::managerEntry84:break; // inherited2A4330 return-only leaf
    case Notice::beginTransition7C:
        s->state.backProposalPending=false;
        if(event.route.page==5){
            // Authentic page override1F74D0: exit2 sets4C8, entry is no-op.
            // It does NOT call inherited aperture transition1D7480/1D74E0.
            game::OriginalUiMemoryCardPlan50KG plan;
            if(game::OriginalUiMemoryCard50KG::callback(*s->bank().memoryCardState(),static_cast<std::uint32_t>(event.phase),plan)!=game::OriginalUiMemoryCardStatus50KG::ok||
               !s->memoryCardPlan(plan))s->fault(Status::widget,0x1F74D0);
        }else if(!apertureOkay(s->apertureFor(event.route.page).beginTransition7C(event.phase)))s->fault(Status::aperture,event.sourcePC);break;
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
    // Page5 virtual+80=1F74F0 returns1, independent of retained aperture.
    if(route.page==5)return game::OriginalUiTransitionCompletion50KG::complete;
    return s->apertureFor(route.page).pollCompletion(route,phase);
}
const game::OriginalUiTransitionOwner50KG* OriginalUiOptionsSession50KG::externalLifecycleOwner() const noexcept {
    return storage_?storage_->cold.externalTransition.get():nullptr;
}
bool OriginalUiOptionsSession50KG::ownsPreparedPage(std::uint8_t page) const noexcept {
    const auto* s=storage_.get();return s&&s->operational()&&!s->inOperation&&s->ownsPage(page);
}
bool OriginalUiOptionsSession50KG::beginNativeLoadInstall(
    const OriginalUiOptionsSessionCold50KG& expected)noexcept {
    auto* s=storage_.get();
    if(!s||!s->operational()||s->sourceTornDown||s->inOperation||s->rendering||s->updatePrepared||
       s->paused||s->state.backProposalPending||!s->idleInput()||s->activePage()!=5||
       s->controlsExecution||s->controls2EntrySerial||s->controls2EntryPoll||
       !s->cold.application||!s->cold.application->known||
       s->cold.application->controllerPopupState.active3CC)return false;
    const auto sameOwner=[](const auto& a,const auto& b)noexcept {
        return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
    };
    const auto& c=s->cold;
    if(!sameOwner(c.resources,expected.resources)||!sameOwner(c.gpu,expected.gpu)||
       !sameOwner(c.optionsArchive,expected.optionsArchive)||!sameOwner(c.profile,expected.profile)||
       !sameOwner(c.commonProfile,expected.commonProfile)||!sameOwner(c.application,expected.application)||
       !sameOwner(c.gameSettingsProfile,expected.gameSettingsProfile)||!sameOwner(c.controlsProfile,expected.controlsProfile)||
       !sameOwner(c.audio,expected.audio)||!sameOwner(c.soundMode,expected.soundMode)||
       !sameOwner(c.externalTransition,expected.externalTransition)||
       c.audioMode!=OriginalUiMainAudioMode50KG::enabled||c.audioMode!=expected.audioMode)return false;
    const auto shared=c.application->sharedDialogs;
    if(!shared||!sharedMenuLifetimeCurrent(*shared,s->collector)||!shared->menuTopologyCurrent()||
       shared->menu470Owner()||shared->menuUpdateFailedOwner_||shared->menuUpdateAborted_||
       shared->menu470Phase_!=OriginalUiSharedDialogs50KG::Menu470Phase::idle||
       shared->demand470Pending_||shared->update470Pending_||
       shared->menuDisplayPinned_||shared->menuUpdateFailedDisplayPinned_)return false;
    // pendingControls2* may remain historical; only actual pending/lifetime
    // authority above excludes. Never erase stale snapshots to fake quiescence.
    s->inOperation=true;return true;
}
void OriginalUiOptionsSession50KG::endNativeLoadInstall(bool committed)noexcept {
    // Only the paired private Frontend call can reach here after begin succeeds.
    if(committed)storage_->state.framePublished=false;
    storage_->inOperation=false;
}
Status OriginalUiOptionsSession50KG::input(const OriginalUiMainSessionInput50KG& in,OriginalUiOptionsSessionInputReceipt50KG& receipt,Diagnostic& d) noexcept {
    d={};receipt={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->updatePrepared||in.controller>1||in.originalInput>15)return result(d,Status::invalidInput);
    s->state.framePublished=false;s->state.backProposalPending=false;s->pendingAcceptCommitted=false;s->pendingControlsCommitted=false;receipt.sourceLifecycle=s->transition();
    // Shared popup precedes the Options manager in original global traversal.
    // Snapshot-active dispatch means the opening Confirm is not reused.
    // The popup does not consume input: Options suppresses page actions, while
    // its selected movement widget still receives mapped directions silently.
    if(s->cold.application->controllerPopupState.active3CC){
        game::OriginalUiControllerPopupPlan50KG plan;
        if(game::OriginalUiControllerPopup50KG::input(s->cold.application->controllerPopupState,
            in.controller,in.originalInput,true,plan)!=game::OriginalUiControllerPopupStatus50KG::ok)
            return result(d,Status::unsupportedState,0x2062D0);
        if(plan.accepted){
            if(s->cold.application->transportE00!=-1)return result(d,Status::unsupportedState,0x267BE0);
            if(plan.soundRequested&&s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
               (!s->cold.audio||!s->cold.audio->canPlayOptionsEffect(plan.soundId)))return result(d,Status::audio,plan.soundPC);
            s->cold.application->controllerPopupState=plan.state;
            if(plan.soundRequested&&!s->audioEffect(plan.soundId)){d=s->callbackFault;return d.status;}
            receipt.popupChanged=true;receipt.popupSourcePC=plan.sourcePC;
        }
    }
    receipt.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
    if(!receipt.containerReached||!s->activeAperture().snapshot().inputEnabled268)return result(d,Status::noChange);
    if(s->cold.application->transportE00!=-1)return result(d,Status::unsupportedState,0x267BE0);
    s->inOperation=true;receipt.pageCallbackRan=true;
    if(s->activePage()==5){game::OriginalUiMemoryCardPlan50KG plan;
        const auto status=game::OriginalUiMemoryCard50KG::input(*s->bank().memoryCardState(),in.controller,
            static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),true,true,plan);
        receipt.pageAccepted=plan.pageAccepted;Status returned=Status::noChange;
        if(status==game::OriginalUiMemoryCardStatus50KG::unavailableStorage)returned=Status::unavailableDestination;
        else if(status!=game::OriginalUiMemoryCardStatus50KG::ok&&status!=game::OriginalUiMemoryCardStatus50KG::noChange)returned=Status::widget;
        else if(!s->memoryCardPlan(plan))returned=Status::widget;
        else if(plan.soundRequested)returned=Status::ok;
        s->inOperation=false;receipt.status=returned;return result(d,returned,plan.sourcePC);
    }
    if(s->activePage()==6){game::OriginalUiCreditsPlan50KG plan;
        const auto status=s->bank().creditsController()->input(*s->bank().creditsState(),in.controller,
            static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),true,true,plan);
        receipt.pageAccepted=plan.pageAccepted;Status returned=Status::noChange;
        if(status!=game::OriginalUiOptionsMenuStatus50KG::ok&&status!=game::OriginalUiOptionsMenuStatus50KG::noChange&&
           status!=game::OriginalUiOptionsMenuStatus50KG::unavailable)returned=Status::widget;
        else if(plan.intent!=game::OriginalUiCreditsIntent50KG::none){
            if(s->stageCreditsReturn(plan)){receipt.routeProposal={this,s->proposalSerial,s->pendingBack};returned=Status::pageProposed;}
            else returned=Status::unavailableDestination;
        }
        s->inOperation=false;receipt.status=returned;return result(d,returned);
    }
    if(Storage::isControlsPage(s->activePage())){
        const auto index=s->activePage()-2;
        if(index==1){std::uint64_t poll=0;const auto shared=s->cold.application->sharedDialogs;
            // A stale caller Present cannot edit, Accept or Cancel after an
            // actual absent/unknown poll. The real host adapter additionally
            // owns instance changes and neutral release-to-arm, unchanged.
            if(in.controller!=1||in.secondControllerObservation!=WindowControllerPresence50DQ::Present||
               !shared||!s->controls2EntrySerial||!s->controls2EntryPoll||
               !shared->controls2ObservedPresent(poll)||poll<s->controls2EntryPoll||s->cold.application->courseUi474!=0||s->cold.application->courseUi478!=1){
                s->inOperation=false;receipt.status=Status::noChange;return result(d,Status::noChange);}
        }
        const auto status=s->controlsController[index].input(s->controlsState[index],in.controller,
            static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),true,true,receipt.controls);
        receipt.pageAccepted=receipt.controls.pageAccepted;Status returned=Status::noChange;
        if(status!=game::OriginalUiOptionsMenuStatus50KG::ok&&status!=game::OriginalUiOptionsMenuStatus50KG::noChange&&
            status!=game::OriginalUiOptionsMenuStatus50KG::unavailable){s->fault(Status::widget,0x1F3950);returned=Status::widget;}
        else if(!s->applyControlsPlan(receipt.controls)){s->fault(Status::widget,0x1F3950);returned=Status::widget;}
        else if(receipt.controls.intent!=game::OriginalUiControlsIntent50KG::none){
            game::OriginalUiRouteDecision50KG route;
            if(s->idleInput()&&s->ownsPage(1)&&s->controlsSoundReady(receipt.controls.soundId)&&
                game::describeOriginalUiControlsReturn50KG(s->controlsState[index],receipt.controls.intent,route)){
                s->pendingControlsBefore=*s->cold.controlsProfile;s->pendingControlsLocal=s->controlsState[index];
                s->pendingBack=route;s->state.backProposalPending=true;++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;
                receipt.routeProposal={this,s->proposalSerial,route};returned=Status::pageProposed;
            }else returned=Status::unavailableDestination;
        }else if(receipt.controls.soundRequested)returned=s->audioEffect(receipt.controls.soundId)?Status::ok:Status::audio;
        s->inOperation=false;if(s->state.faulted){d=s->callbackFault;returned=d.status;}receipt.status=returned;return result(d,returned);
    }
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
        }else if(receipt.gameOptions.intent==game::OriginalUiGameOptionsIntent50KG::cancel&&s->idleInput()&&
            (s->cold.audioMode==OriginalUiMainAudioMode50KG::muted||
             (s->cold.audio&&s->cold.audio->canPlayGameOptionsEffect(5)))){
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
    s->popupPresence=in.secondControllerObservation;s->popupOpenedByInput=false;
    game::OriginalUiOptionsContext50KG context;
    context.admission.sourceControllerReady=true;context.admission.selectionOwnerReady=true;
    context.admission.profileOwnerReady=s->cold.profile->known&&s->cold.commonProfile->known;
    context.popupActive=in.popupActive||s->cold.application->controllerPopupState.active3CC;context.sourceInputBlocked=in.sourceInputBlocked;
    context.secondControllerPresent=in.secondControllerObservation==WindowControllerPresence50DQ::Present;context.mode34=s->cold.commonProfile->mode34;
    // All destinations are closed here. Back is an UNACCEPTED proposal for the
    // coordinator; this page cannot attest another screen's native readiness.
    const auto ws=s->bank().inputWithPageEffects(in.controller,static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),
        true,true,context,s,&Storage::pageEffects,receipt.page,receipt.movement,d.widget);
    receipt.pageAccepted=receipt.page.pageAccepted;
    if(s->popupOpenedByInput){receipt.popupChanged=true;receipt.popupSourcePC=0x1F9878;}
    Status status=receipt.popupChanged?Status::ok:Status::noChange;
    if(!widgetOkay(ws)&&ws!=WidgetStatus::unavailable){s->fault(Status::widget,0x1F9640);status=Status::widget;}
    else if(receipt.page.routeStatus==game::OriginalUiRouteStatus50KG::unavailable) {
        const auto& route=receipt.page.route;
        status=s->popupOpenedByInput?Status::ok:
            route.action==game::OriginalUiRouteAction50KG::controllerRequired&&s->popupPresence==WindowControllerPresence50DQ::Absent?
            Status::secondControllerRequired:Status::unavailableDestination;
        if(in.originalInput==6&&s->idleInput()&&route.profileProposalComplete&&route.request.kind==game::OriginalUiRouteKind50KG::changeScreen&&
            route.request.targetScreen==4&&route.request.targetPage==1&&route.blockers==static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)) {
            s->pendingBack=route;s->state.backProposalPending=true;++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;receipt.routeProposal={this,s->proposalSerial,route};status=Status::backProposed;
        }
        if(in.originalInput==4&&s->idleInput()&&hallRequest(route)&&route.profileProposalComplete&&
           route.blockers==static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)){
            s->pendingHallCommon=*s->cold.commonProfile;s->pendingBack=route;s->state.backProposalPending=true;
            ++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;
            receipt.routeProposal={this,s->proposalSerial,route};status=Status::screenProposed;
        }
        // Only page3 adds a retained real-poll and exact owner/resource proof.
        // The actual Absent popup above and Unknown failure remain unchanged.
        std::uint64_t controls2Poll=0;const auto shared=s->cold.application->sharedDialogs;
        const bool controls2Ready=route.request.targetPage!=3||
            (in.controller==0&&in.secondControllerObservation==WindowControllerPresence50DQ::Present&&
             controls2EntryRequest(route)&&shared&&controls2EntryReady(*shared,controls2Poll));
        if(in.originalInput==4&&s->idleInput()&&(route.request.targetPage>=2&&route.request.targetPage<=6)&&
            controls2Ready&&s->ownsPage(route.request.targetPage)&&route.selectedSlot==(route.request.targetPage>=5?route.request.targetPage:route.request.targetPage-1)&&route.profileProposalComplete&&
            route.request.kind==game::OriginalUiRouteKind50KG::changePage&&route.request.targetScreen==12&&
            route.blockers==static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)){
            if(route.request.targetPage==3){
                s->pendingControls2Poll=controls2Poll;s->pendingControls2Options=*s->cold.profile;
                s->pendingControls2Common=*s->cold.commonProfile;s->pendingControls2Presets=*s->cold.controlsProfile;
                s->pendingControls2Local=s->controlsState[1];s->pendingControls2Navigation=*s->bank().navigation();
            }
            s->pendingBack=route;s->state.backProposalPending=true;++s->proposalSerial;if(!s->proposalSerial)++s->proposalSerial;
            receipt.routeProposal={this,s->proposalSerial,route};status=Status::pageProposed;
        }
        // Other unavailable subpages retain prospective audio/profile requests.
        // The independently owned absent-device dialog above is not page3 admission.
    } else if(receipt.page.soundRequested)status=Status::ok;
    // Actual page-first then selected Movement_Control5 traversal is inside
    // bank.input, including source popup/controller-suppressed page callbacks.
    s->inOperation=false;if(s->state.faulted){d=s->callbackFault;status=d.status;}receipt.status=status;return result(d,status);
}
bool OriginalUiOptionsSession50KG::routeProposalCurrent(const OriginalUiOptionsRouteProposal50KG& proposal) const noexcept {
    const auto& decision=proposal.decision;
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||s->updatePrepared||!s->state.backProposalPending||!s->idleInput()||proposal.source!=this||!proposal.serial||proposal.serial!=s->proposalSerial)return false;
    const auto& expected=s->pendingBack;
    if(expected.request.sourcePage==1&&expected.request.targetScreen==12&&expected.request.targetPage==3){
        std::uint64_t poll=0;const auto shared=s->cold.application->sharedDialogs;
        return controls2EntryRequest(expected)&&sameDecision(decision,expected)&&shared&&
            controls2EntryReady(*shared,poll)&&poll==s->pendingControls2Poll&&controls2EntrySnapshotCurrent(false);
    }
    if(hallRequest(expected)){
        if(!sameCommonHall(*s->cold.commonProfile,s->pendingHallCommon)||!sameDecision(decision,expected))return false;
        game::OriginalUiOptionsContext50KG context;context.mode34=s->cold.commonProfile->mode34;
        context.admission.sourceControllerReady=context.admission.selectionOwnerReady=context.admission.profileOwnerReady=true;
        game::OriginalUiRouteDecision50KG fresh;const auto* nav=s->cold.resources->widgets()->navigation();
        return nav&&s->activePage()==1&&game::dispatchOriginalUiOptions50KG(*nav,0,game::OriginalUiRouteInput50KG::activate,context,fresh)==game::OriginalUiRouteStatus50KG::unavailable&&sameDecision(fresh,expected);
    }
    if(game::originalUiMemoryCardReturnShape50KG(expected))
        return s->activePage()==5&&sameDecision(decision,expected)&&s->memoryCardReturnCurrent()&&
            s->cold.commonProfile->known&&s->cold.application->known&&!(s->cold.application->globalFlags228&4u)&&s->cold.application->transportE00==-1;
    if(game::originalUiCreditsReturnShape50KG(expected))
        return s->activePage()==6&&sameDecision(decision,expected)&&s->creditsReturnCurrent()&&
            s->cold.commonProfile->known&&s->cold.commonProfile->mode34!=2&&s->cold.application->known&&
            !(s->cold.application->globalFlags228&4u)&&s->cold.application->transportE00==-1;
    if(game::originalUiControlsReturnShape50KG(expected))
        return s->activePage()==expected.request.sourcePage&&decision.profileProposalComplete&&sameDecision(decision,expected)&&
            s->cold.profile->known&&s->cold.commonProfile->known&&s->cold.application->known&&
            !(s->cold.application->globalFlags228&4u)&&s->cold.application->transportE00==-1&&s->pendingControlsCurrent();
    const auto* nav=expected.request.sourcePage==4?&s->cold.resources->widgets()->gameOptionsState()->navigation:s->cold.resources->widgets()->navigation();
    const bool selection=expected.request.sourcePage==4?
        (s->ownsPage(4)&&nav&&nav->controllerGroup[0]==expected.selectedSlot):
        (nav&&nav->controllerGroup[0]==1&&nav->groups[1].selectedSlot==expected.selectedSlot);
    if(!s->cold.profile->known||!s->cold.commonProfile->known||!s->cold.application->known||
        (s->cold.application->globalFlags228&4u)||s->cold.application->transportE00!=-1||
        !selection||s->activePage()!=expected.request.sourcePage||
        !decision.profileProposalComplete||!sameDecision(decision,expected))return false;
    if(game::originalUiGameOptionsAcceptShape50KG(expected))return s->pendingAcceptCurrent();
    // Cancel commits the remembered group before BACK5. Recheck the exact
    // real effect at coordinator admission, not just when input proposed it,
    // so a stale proposal cannot publish the route after capacity was lost.
    if(expected.request.sourceScreen==12&&expected.request.sourcePage==4&&
       expected.request.targetScreen==12&&expected.request.targetPage==1&&
       expected.request.sourcePC==0x1F51D8)
        return s->cold.audioMode==OriginalUiMainAudioMode50KG::muted||
            (s->cold.audio&&s->cold.audio->canPlayGameOptionsEffect(5));
    return true;
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
bool OriginalUiOptionsSession50KG::commitControlsReturn(const OriginalUiOptionsRouteProposal50KG& proposal,
    const game::OriginalUiRouteDecision50KG& admitted) noexcept {
    auto* s=storage_.get();if(!s||!routeProposalCurrent(proposal)||!game::originalUiControlsReturnShape50KG(proposal.decision)||
        !game::originalUiControlsReturnShape50KG(admitted)||!admitted.admitted||admitted.blockers||
        admitted.action!=game::OriginalUiRouteAction50KG::request||!s->ownsPage(1))return false;
    auto original=admitted;original.admitted=false;original.action=game::OriginalUiRouteAction50KG::unavailable;
    original.blockers=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner);
    if(!sameDecision(original,proposal.decision)||!s->pendingControlsCurrent())return false;
    s->pendingControlsAfter=s->pendingControlsBefore;
    const bool accept=admitted.request.sourcePC==0x1F39AC;
    if(accept&&!game::applyOriginalUiControlsAccept50KG(s->pendingControlsLocal,s->pendingControlsBefore,s->pendingControlsAfter))return false;
    //Only the actual source Accept selects the shared preset. Cancel has no
    //profile store; neither path invents a disk save or remembered-group write.
    if(accept){
        //The preflight above already admitted the index and computed this
        //deterministic row. Invoke188880's native owner on canonical storage.
        (void)game::selectControlPreset49D(*s->cold.controlsProfile,
            s->pendingControlsLocal.controllerIndex4DC,s->pendingControlsLocal.preset4D8);
        ++s->state.controlsAcceptCount;
    }
    s->pendingControlsCommitted=true;return true;
}
Status OriginalUiOptionsSession50KG::notifyRouteAccepted(const OriginalUiOptionsRouteProposal50KG& proposal,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(!s->operational()||s->inOperation||s->updatePrepared||!s->state.backProposalPending||proposal.source!=this||
        !proposal.serial||proposal.serial!=s->proposalSerial)return result(d,Status::invalidInput);
    const auto& decision=proposal.decision;const auto& pending=s->pendingBack;
    const auto& r=decision.request;
    if(!sameDecision(decision,pending))return result(d,Status::invalidInput);
    if(hallRequest(decision)){
        auto expected=s->pendingHallCommon;if(expected.mode34!=5){expected.previousMode38=expected.mode34;expected.mode34=5;}
        const auto t=s->transition();const auto* nav=s->bank().navigation();
        if(!sameCommonHall(*s->cold.commonProfile,expected)||!s->cold.profile->known||s->cold.profile->savedSlot6E4!=4||
           !nav||nav->controllerGroup[0]!=1||nav->groups[1].selectedSlot!=4||!t.bound||t.current.screen!=12||t.current.page!=1||
           t.requested.screen!=11||t.requested.page!=1||t.lifecycle!=game::OriginalUiTransitionLifecycle50KG::requestScreenExit||
           t.phase!=game::OriginalUiTransitionPhase50KG::none||t.applicationRequestQueued||t.initialRestartPending||
           !s->cold.application->known||(s->cold.application->globalFlags228&4u)||s->cold.application->transportE00!=-1)
            return result(d,Status::unsupportedState,0x1F98C8);
        s->state.framePublished=false;s->state.backProposalPending=false;
        if(!s->audioEffect(1)){d=s->callbackFault;return d.status;}return result(d,Status::ok);
    }
    if(game::originalUiMemoryCardReturnShape50KG(decision)){
        const auto t=s->transition();
        if(!s->memoryCardReturnCurrent()||!t.bound||t.current.screen!=12||t.current.page!=5||t.requested.screen!=12||t.requested.page!=1||
           t.lifecycle!=game::OriginalUiTransitionLifecycle50KG::requestPageExit||t.phase!=game::OriginalUiTransitionPhase50KG::none||
           t.applicationRequestQueued||t.initialRestartPending)return result(d,Status::unsupportedState,r.sourcePC);
        s->state.framePublished=false;s->state.backProposalPending=false;return result(d,Status::ok);
    }
    if(game::originalUiCreditsReturnShape50KG(decision)){
        const auto t=s->transition();
        if(!s->creditsReturnCurrent()||!t.bound||t.current.screen!=12||t.current.page!=6||
           t.requested.screen!=12||t.requested.page!=1||t.lifecycle!=game::OriginalUiTransitionLifecycle50KG::requestPageExit||
           t.phase!=game::OriginalUiTransitionPhase50KG::none||t.applicationRequestQueued||t.initialRestartPending)
            return result(d,Status::unsupportedState,r.sourcePC);
        s->state.framePublished=false;s->state.backProposalPending=false;
        if(s->pendingCredits.soundRequested&&!s->audioEffect(s->pendingCredits.soundId)){d=s->callbackFault;return d.status;}
        return result(d,Status::ok);
    }
    if(game::originalUiControlsReturnShape50KG(decision)){
        const auto t=s->transition();const auto index=r.sourcePage-2;
        if(index>=2||!s->pendingControlsCommitted||!s->ownsPage(r.sourcePage)||
            !s->sameControlsLocal(s->controlsState[index],s->pendingControlsLocal)||
            !game::sameOriginalUiControlsPresets50KG(*s->cold.controlsProfile,s->pendingControlsAfter)||
            !t.bound||t.current.screen!=12||t.current.page!=r.sourcePage||t.requested.screen!=12||t.requested.page!=1||
            t.lifecycle!=game::OriginalUiTransitionLifecycle50KG::requestPageExit||
            t.phase!=game::OriginalUiTransitionPhase50KG::none||t.applicationRequestQueued||t.initialRestartPending||
            !s->cold.application->known||(s->cold.application->globalFlags228&4u)||s->cold.application->transportE00!=-1)
            return result(d,Status::unsupportedState,0x1F3950);
        s->state.framePublished=false;s->state.backProposalPending=false;s->inOperation=true;
        const bool okay=s->audioEffect(r.sourcePC==0x1F39AC?1:5);s->inOperation=false;
        if(!okay){d=s->callbackFault;return d.status;}return result(d,Status::ok);
    }
    const bool accept=game::originalUiGameOptionsAcceptShape50KG(decision);
    if(!accept&&decision.profileChanges.count!=1)return result(d,Status::invalidInput);
    const auto& p=decision.profileChanges.values[accept?8:0];
    const bool gamePage=r.sourcePage==4;const bool cancel=gamePage&&!accept;
    const bool sameScreen=r.kind==game::OriginalUiRouteKind50KG::changePage;
    const bool shape=accept||(cancel?(sameScreen&&r.sourceScreen==12&&r.targetScreen==12&&r.targetPage==1&&r.sourcePC==0x1F51D8&&
        decision.inputPC==0x1F50F0&&p.field==game::OriginalUiProfileField50KG::optionsGroup6E8&&p.sourcePC==0x1F51C8):
        (r.sourceScreen==12&&r.sourcePage==1&&p.field==game::OriginalUiProfileField50KG::optionsSlot6E4&&
         ((sameScreen&&r.targetScreen==12&&(r.targetPage>=2&&r.targetPage<=6)&&decision.selectedSlot==(r.targetPage>=5?r.targetPage:r.targetPage-1)&&
           r.sourcePC==(r.targetPage==2?0x1F96FCu:r.targetPage==3?0x1F9744u:r.targetPage==5?0x1F98F4u:r.targetPage==6?0x1F9910u:0x1F9898u)&&p.sourcePC==0x1F9940)||
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
    if(controls2EntryRequest(decision)){
        std::uint64_t poll=0;const auto shared=s->cold.application->sharedDialogs;
        if(!shared||s->controls2EntrySerial||!shared->controls2ObservedPresent(poll)||
           poll!=s->pendingControls2Poll||!controls2EntrySnapshotCurrent(true)||
           !shared->controls2MenuResourcesCurrent(*this,s->collector))return result(d,Status::unsupportedState,0x1F9744);
        // Issued only after the manager committed this exact source request and
        // profile write. It survives opening and source B8 until previousPage94.
        s->controls2EntrySerial=proposal.serial;s->controls2EntryPoll=poll;
    }
    s->state.framePublished=false;s->state.backProposalPending=false;s->inOperation=true;
    const bool okay=s->audioEffect(cancel||!sameScreen?5:1);s->inOperation=false;
    // Accept1F51A0 reaches267BE0 only after SELECT1F5188. The preflighted
    // shared transportE00=-1 takes its exact no-work return. No device drain,
    // fabricated active-transport success or extra PCM advance is substituted.
    if(!okay){d=s->callbackFault;return d.status;}return result(d,Status::ok);
}
Status OriginalUiOptionsSession50KG::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG& in,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
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
    if(s->cold.application->sharedDialogs&&s->cold.application->sharedDialogs->menu470PendingFor(s->collector))
        return result(d,Status::unsupportedState,0x204A00);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||!s->currentPage())return result(d,Status::invalidInput);
    if(!s->updatePrepared||in.elapsedSeconds!=s->preparedUpdate.elapsedSeconds||in.paused!=s->preparedUpdate.paused||
        in.debugChordHeld!=s->preparedUpdate.debugChordHeld){s->fault(Status::unsupportedState,0x2A3C80);d=s->callbackFault;return d.status;}
    s->updatePrepared=false;s->state.backProposalPending=false;s->inOperation=true;
    if(const auto shared=s->cold.application->sharedDialogs){
        OriginalUiSharedDialogs50KG::Diagnostic sharedDiagnostic;
        if(!shared->advanceMenu470(*this,s->collector,in.elapsedSeconds,sharedDiagnostic)){
            s->fault(Status::unsupportedState,sharedDiagnostic.sourcePC);s->inOperation=false;d=s->callbackFault;return d.status;
        }
    }else{
        game::OriginalUiControllerPopupPlan50KG popupUpdate;
        if(game::OriginalUiControllerPopup50KG::update(s->cold.application->controllerPopupState,in.elapsedSeconds,popupUpdate)!=
           game::OriginalUiControllerPopupStatus50KG::ok)s->fault(Status::unsupportedState,0x206420);
        else s->cold.application->controllerPopupState=popupUpdate.state;
    }
    const auto* order=s->bank().order();if(!s->state.faulted)s->updateWidgets(order->ordinaryWidgets,in.elapsedSeconds);
    if(!s->state.faulted&&s->state.pageShown&&s->activePage()==5){game::OriginalUiMemoryCardPlan50KG plan;
        if(game::OriginalUiMemoryCard50KG::update(*s->bank().memoryCardState(),in.elapsedSeconds,plan)!=game::OriginalUiMemoryCardStatus50KG::ok||
           !s->memoryCardPlan(plan))s->fault(Status::widget,0x1F6D40);
        else if(plan.returnToOptions&&!s->stageMemoryCardReturn(plan))s->fault(Status::unsupportedState,plan.requestPC);
    }
    if(!s->state.faulted&&s->state.pageShown&&s->activePage()==6){game::OriginalUiCreditsPlan50KG plan;
        const auto status=s->bank().creditsController()->update(*s->bank().creditsState(),in.elapsedSeconds,
            static_cast<std::uint32_t>(s->transition().phase),false,false,plan);
        if((status!=game::OriginalUiOptionsMenuStatus50KG::ok&&status!=game::OriginalUiOptionsMenuStatus50KG::unavailable)||
           plan.completionPredicateRequired||!s->creditsPlan(plan))s->fault(Status::widget,0x1F85C0);
        else if(plan.intent!=game::OriginalUiCreditsIntent50KG::none&&!s->stageCreditsReturn(plan))s->fault(Status::unsupportedState,plan.requestPC);
    }
    if(!s->state.faulted&&s->state.pageShown&&s->activePage()!=6&&s->activePage()!=5&&(Storage::isControlsPage(s->activePage())?s->activeAperture().saveControlsUpdateDelta38(in.elapsedSeconds):s->activePage()==4?s->gameAperture.saveGameOptionsUpdateDelta38(in.elapsedSeconds):s->aperture.saveOptionsMenuUpdateDelta38(in.elapsedSeconds))!=ApertureStatus::ok)
        s->fault(Status::aperture,Storage::isControlsPage(s->activePage())?0x1F3CD4:s->activePage()==4?0x1F5BE4:0x1F9A74);
    if(!s->state.faulted)s->updateWidgets(order->onTopWidgets,in.elapsedSeconds);
    // Options1D0DA0 attaches scene siblings after Pod widgets: particle manager,
    // borrowed stationary manager camera, then actual Type8 fire emitter.
    auto& fire=*s->cold.resources->fire();
    if(!s->state.faulted){s->state.lastParticleAdvance=fire.advance(in.elapsedSeconds,in.paused);
        if(!s->state.lastParticleAdvance.okay())s->fault(Status::particle,0x160E40);}
    if(!s->state.faulted){s->state.lastFireEmit=fire.emitSource15E2A8(in.elapsedSeconds,in.paused,s->cold.application->rawRandom308030,
        s->cold.application->fireSerial433970,s->cold.application->fireLazy84C8_);
        if(!s->state.lastFireEmit.okay())s->fault(Status::particle,0x15AEA0);}
    s->inOperation=false;if(s->state.faulted){d=s->callbackFault;return d.status;}
    ++s->state.updateCount;return result(d,Status::ok);
}
bool OriginalUiOptionsSession50KG::controls2EntryReady(const OriginalUiSharedDialogs50KG& shared,
    std::uint64_t& poll)const noexcept {
    const auto* s=storage_.get();poll=0;
    const auto* nav=s&&s->cold.resources?s->cold.resources->widgets()->navigation():nullptr;
    return nav&&nav->controllerGroup[0]==1&&nav->groups[1].selectedSlot==2&&s->operational()&&!s->updatePrepared&&s->state.updateCount&&s->idleInput()&&s->activePage()==1&&
        !s->controls2EntrySerial&&s->ownsPage(3)&&s->controlsRegistryPublished&&
        s->controlsState[1].controllerIndex4DC==1&&s->controlsState[1].navigation.page==s->controlsController[1].page()&&
        s->cold.profile->known&&s->cold.commonProfile->known&&s->cold.application->known&&
        !(s->cold.application->globalFlags228&4u)&&s->cold.application->transportE00==-1&&
        !s->cold.application->controllerPopupState.active3CC&&s->cold.application->courseUi474==1&&s->cold.application->courseUi478==0&&
        shared.controls2ObservedPresent(poll)&&!shared.menu470Owner()&&
        shared.menu470Phase_==OriginalUiSharedDialogs50KG::Menu470Phase::idle&&
        !shared.demand470Pending_&&!shared.update470Pending_&&
        shared.controls2MenuResourcesCurrent(*this,s->collector);
}
bool OriginalUiOptionsSession50KG::controls2EntrySnapshotCurrent(bool committed)const noexcept {
    const auto* s=storage_.get();if(!s||!s->pendingControls2Poll||!s->cold.profile||!s->cold.commonProfile||
        !s->cold.controlsProfile||!s->cold.resources||!s->cold.resources->widgets()->navigation())return false;
    return s->cold.profile->known==s->pendingControls2Options.known&&
        s->cold.profile->savedSlot6E4==(committed?2u:s->pendingControls2Options.savedSlot6E4)&&
        sameCommonHall(*s->cold.commonProfile,s->pendingControls2Common)&&
        game::sameOriginalUiControlsPresets50KG(*s->cold.controlsProfile,s->pendingControls2Presets)&&
        s->sameControlsLocal(s->controlsState[1],s->pendingControls2Local)&&
        sameNavigation(s->controlsState[1].navigation,s->pendingControls2Local.navigation)&&
        sameNavigation(*s->cold.resources->widgets()->navigation(),s->pendingControls2Navigation);
}
bool OriginalUiOptionsSession50KG::controls2WarningCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiOptionsDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return sharedMenuLifetimeCurrent(shared,draw)&&s->operational()&&!s->updatePrepared&&s->state.updateCount&&
        s->currentPage()&&s->activePage()==3&&s->controls2EntrySerial&&s->controls2EntryPoll&&
        s->controlsRegistryPublished&&s->controlsState[1].loaded&&s->controlsState[1].controllerIndex4DC==1&&
        s->controlsState[1].navigation.page==s->controlsController[1].page();
}
bool OriginalUiOptionsSession50KG::admitSharedMenu470(const OriginalUiSharedDialogs50KG& shared)const noexcept {
    const auto* s=storage_.get();OriginalUiSharedDialogs50KG::Diagnostic d;
    return s&&shared.admitControls2Menu470(*this,s->collector,d);
}
bool OriginalUiOptionsSession50KG::readSharedMenuRasterInputs(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiOptionsDrawContribution50KG& draw,OriginalUiMenuRasterInputs50KG& out)const noexcept {
    const auto* s=storage_.get();
    if(!sharedMenuLifetimeCurrent(shared,draw)||s->state.faulted||s->sourceClosing||
       !s->cold.resources->loaded()||!s->cold.gpu||!s->cold.gpu->initialized()||
       !draw.sharedMenuRasterResourcesCurrent(shared,s->cold.resources.get(),s->cold.gpu.get(),
           s->cold.gpu->publicationToken()))return false;
    const auto* video=s->cold.resources->video();
    if(!video||!video->lifetime||video->width!=640||video->height!=448||
       video->colorBits!=32||video->packetZMaximum!=0xFFFFFF||video->zBufferFormat!=0x31)return false;
    OriginalUiMenuRasterInputs50KG next;
    const auto* camera=s->cold.resources->camera();
    if(!s->state.cameraActive||!s->cold.resources->menuCameraConstructionCurrent(s->cold.optionsArchive,camera))return false;
    // This exact retained constructor is 1D0580 ->2361C0(640,448), not a
    // borrowed Garage camera or display-size inference.
    next.camera=camera;next.configurationIdentity=camera->uiIdentity;
    next.viewport={{0,0,640,448}};
    next.session=this;next.resources=s->cold.resources.get();next.videoLifetime=video->lifetime;
    next.cpu=s->cold.resources->textures();next.gpu=s->cold.gpu.get();
    next.publication=s->cold.gpu->publicationToken();
    if(!next.cpu||!next.publication)return false;
    next.xyOffsetFixed4={{(2048u-next.viewport[2]/2u-next.viewport[0])*16u,
        (2048u-next.viewport[3]/2u-next.viewport[1])*16u}};
    out=std::move(next);return true;
}
bool OriginalUiOptionsSession50KG::sharedMenuLifetimeCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiOptionsDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return s&&s->state.bound&&!s->sourceTornDown&& &s->collector==&draw&&s->cold.application&&
        s->cold.application->sharedDialogs.get()==&shared&&s->cold.resources&&
        s->cold.resources->controllerPopup()==shared.controller47C_.resources;
}
bool OriginalUiOptionsSession50KG::sharedMenuUnwindEntryCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiOptionsDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return sharedMenuLifetimeCurrent(shared,draw)&&!s->inOperation&&!s->rendering;
}
bool OriginalUiOptionsSession50KG::sharedMenuRenderCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiOptionsDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return sharedMenuLifetimeCurrent(shared,draw)&&s->inOperation&&s->rendering&&s->operational()&&
        !s->updatePrepared&&s->state.updateCount&&s->currentPage();
}
bool OriginalUiOptionsSession50KG::sharedMenuUpdateCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiOptionsDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return sharedMenuLifetimeCurrent(shared,draw)&&s->inOperation&&!s->rendering&&s->operational()&&
        !s->updatePrepared&&s->currentPage();
}
Status OriginalUiOptionsSession50KG::render(const OriginalUiOptionsClock50KG& clock,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->inOperation)return result(d,Status::invalidInput);
    s->state.framePublished=false;
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->updatePrepared){s->fault(Status::unsupportedState,0x234440);d=s->callbackFault;return d.status;}
    if(!s->currentPage()||!s->state.updateCount||!s->state.cameraActive||(s->paused&&clock.advanceAnimations))return result(d,Status::invalidInput);
    s->state.backProposalPending=false;s->inOperation=true;s->rendering=true;
    game::OriginalUiControllerPopupRender50KG popupRender;
    game::OriginalUiMemoryCardRender50KG memoryRender;
    const bool memoryShown=s->activePage()==5&&s->state.pageShown;
    if(memoryShown&&game::OriginalUiMemoryCard50KG::render(*s->bank().memoryCardState(),memoryRender)!=game::OriginalUiMemoryCardStatus50KG::ok){
        s->rendering=false;s->inOperation=false;return result(d,Status::widget,0x1F6DE0);}
    bool okay=s->collector.beginPage(s->activePage(),clock,d.draw);
    if(okay){
        if(const auto shared=s->cold.application->sharedDialogs){
            OriginalUiSharedDialogs50KG::Diagnostic sd;
            okay=shared->renderMenuPopupSibling(popupRender,sd);
            if(!okay)d.draw.sourcePC=sd.sourcePC;
        }else okay=game::OriginalUiControllerPopup50KG::render(s->cold.application->controllerPopupState,popupRender)==
            game::OriginalUiControllerPopupStatus50KG::ok;
    }
    okay=okay&&
        s->collector.projectControllerRequiredPopup(popupRender,d.draw)&&s->collector.projectOrdinaryWidgets(true,d.draw)&&
        (memoryShown?s->collector.projectMemoryCardPage(memoryRender,d.draw):s->activePage()==6&&s->state.pageShown?s->collector.projectCreditsPage(static_cast<std::uint32_t>(s->transition().phase),d.draw):s->collector.projectPage(s->state.pageShown,d.draw));
    if(okay&&Storage::isControlsPage(s->activePage())&&s->state.pageShown){
        bool animationNonzero=false;const bool known=s->cold.resources->controlsAnimationObservation(animationNonzero);
        game::OriginalUiControlsPlan50KG plan;const auto index=s->activePage()-2;
        okay=s->controlsController[index].render(s->controlsState[index],*s->cold.controlsProfile,
            known,animationNonzero,plan)==game::OriginalUiOptionsMenuStatus50KG::ok&&s->applyControlsPlan(plan);
    }else if(okay)okay=apertureOkay(s->activeAperture().render48(s->transition().lifecycle));
    okay=okay&&!s->state.faulted;
    if(okay)okay=s->collector.projectPrompts(d.draw)&&(!memoryShown||s->collector.projectMemoryCardDialog(memoryRender,d.draw))&&
        s->collector.projectOnTopWidgets(true,d.draw)&&s->collector.projectParticles(d.draw)&&s->collector.drain(d.draw);
    if(okay&&memoryShown)okay=widgetOkay(s->bank().commitMemoryCardRender(memoryRender,d.widget));
    s->rendering=false;s->inOperation=false;
    if(!okay){if(s->state.faulted){d=s->callbackFault;return d.status;}return result(d,Status::draw);}
    // The retained shared owner has already committed the genuine47C body
    // before its fallible projection. Never replay or roll back that prefix.
    if(!s->cold.application->sharedDialogs)s->cold.application->controllerPopupState=popupRender.state;
    ++s->state.renderCount;s->state.framePublished=true;return result(d,Status::ok);
}
OriginalUiOptionsSessionSnapshot50KG OriginalUiOptionsSession50KG::snapshot() const noexcept {
    if(!storage_)return {};auto out=storage_->state;out.controllerPopup=storage_->cold.application->controllerPopupState;out.transition=storage_->transition();out.aperture=storage_->activeAperture().snapshot();
    out.promptMask42C=storage_->cold.resources->widgets()->pageInputFlags42C();return out;
}
bool OriginalUiOptionsSession50KG::appliedAudioOptions(game::OriginalUiAudioOptions50KG& out) const noexcept {
    const auto* s=storage_.get();
    return s&&s->operational()&&!s->inOperation&&!s->updatePrepared&&
        s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&s->cold.gameSettingsProfile&&
        s->cold.audio&&s->cold.soundMode&&readOriginalUiAppliedAudioOptions50KG(
            *s->cold.gameSettingsProfile,*s->cold.audio,*s->cold.soundMode,out);
}
const game::OriginalUiControlsState50KG* OriginalUiOptionsSession50KG::controlsState(std::uint8_t index) const noexcept {
    return storage_&&index<2&&storage_->controlsPrepared[index]?&storage_->controlsState[index]:nullptr;
}
const OriginalUiNativeDraw50KG* OriginalUiOptionsSession50KG::draw() const noexcept {
    return storage_&&storage_->state.framePublished&&!storage_->state.faulted&&!storage_->state.quitRequested&&
        !storage_->inOperation&&!storage_->updatePrepared&&storage_->currentPage()?storage_->collector.draw():nullptr;
}
const OriginalUiSourceFrame50KM* OriginalUiOptionsSession50KG::sourceFrame() const noexcept {
    return storage_&&storage_->state.framePublished&&storage_->operational()&&
        !storage_->inOperation&&!storage_->updatePrepared&&storage_->currentPage()?storage_->collector.sourceFrame():nullptr;
}
bool OriginalUiOptionsSession50KG::sourceTeardownPending() const noexcept {
    return storage_&&!storage_->sourceTornDown&&storage_->collector.sourceTeardownPending();
}
Status OriginalUiOptionsSession50KG::teardownSource(VehicleWindowGl50DS* renderer,Diagnostic& d) noexcept {
    d={};if(!storage_||storage_->sourceTornDown)return result(d,Status::ok);
    auto& s=*storage_;if(s.inOperation||s.rendering)return result(d,Status::invalidInput);
    s.sourceClosing=true;s.state.framePublished=false;
    if(s.collector.sourceTeardownPending()&&(!renderer||!s.collector.retireSourceRenderer(*renderer,d.draw)))
        return result(d,Status::draw);
    s.sourceTornDown=true;return result(d,Status::ok);
}
void OriginalUiOptionsSession50KG::requestQuit() noexcept {
    if(!storage_||storage_->inOperation)return;storage_->state.quitRequested=true;storage_->state.framePublished=false;
}
}

namespace rr::platform {
bool OriginalUiOptionsSession50KG::pendingAutomaticCreditsReturn(OriginalUiOptionsRouteProposal50KG& out) const noexcept {
    const auto* s=storage_.get();if(!s||!s->state.backProposalPending||s->pendingBack.request.sourcePC!=0x1F8744)return false;
    OriginalUiOptionsRouteProposal50KG next{this,s->proposalSerial,s->pendingBack};if(!routeProposalCurrent(next))return false;out=next;return true;
}
}

namespace rr::platform {
const game::OriginalUiMemoryCardState50KG* OriginalUiOptionsSession50KG::memoryCardState() const noexcept {
    return storage_&&storage_->memoryCardPrepared?storage_->cold.resources->widgets()->memoryCardState():nullptr;
}
bool OriginalUiOptionsSession50KG::pendingAutomaticMemoryCardReturn(OriginalUiOptionsRouteProposal50KG& out) const noexcept {
    const auto* s=storage_.get();if(!s||!s->state.backProposalPending||s->pendingBack.request.sourcePC!=0x1F6DB0)return false;
    OriginalUiOptionsRouteProposal50KG next{this,s->proposalSerial,s->pendingBack};if(!routeProposalCurrent(next))return false;out=next;return true;
}
}

#include "original_ui_rider_session50kg.h"
#include "original_ui_main_draw50kg.h"
#include "original_ui_course_draw_resources50kg.h"
#include "original_ui_course_hologram_draw50kg.h"
#include "vehicle_window_gl50ds.h"
#include <cmath>
#include <limits>
#include <new>
#include <utility>
namespace rr::platform {
namespace {
using Status=OriginalUiRiderSessionStatus50KG;
using Diagnostic=OriginalUiRiderSessionDiagnostic50KG;
using Notice=game::OriginalUiTransitionNotificationKind50KG;
using Life=game::OriginalUiTransitionLifecycle50KG;
using Phase=game::OriginalUiTransitionPhase50KG;
using Completion=game::OriginalUiTransitionCompletion50KG;
using Hook=game::OriginalUiApertureHookKind50KG;
using ApertureStatus=game::OriginalUiApertureStatus50KG;
using Operation=game::OriginalUiControlOperation50KG;
using Kind=game::OriginalUiRiderWidgetKind50KG;
// Paired with bounded frontend admission; actual animation and RNG ownership
// predicates are still required. Runtime acceptance is pending.
constexpr bool boundedRiderConfirmEnabled=true;
bool preparedRider(std::uint32_t rider) noexcept{return rider==19||rider==20;}
Status result(Diagnostic& d,Status s,std::uint32_t pc=0) noexcept {d.status=s;d.sourcePC=pc;return s;}
bool apertureOkay(ApertureStatus s) noexcept {return s==ApertureStatus::ok||s==ApertureStatus::noChange;}
bool genuine(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,assets::OriginalUiArchiveId50KG id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool sameStamp(OriginalUiRiderProfileStamp50KG a,OriginalUiRiderProfileStamp50KG b) noexcept {
    return a.generation==b.generation&&a.mode34==b.mode34;
}
bool sameProfile(const game::OriginalUiRiderSelectionProfile50KG& a,const game::OriginalUiRiderSelectionProfile50KG& b) noexcept {
    if(a.ownerReady!=b.ownerReady||a.mode34!=b.mode34||a.remembered6F0!=b.remembered6F0||
       a.remembered6F4!=b.remembered6F4||a.remembered6F8!=b.remembered6F8||a.remembered6FC!=b.remembered6FC)return false;
    for(std::size_t i=0;i<a.riders.size();++i){const auto& x=a.riders[i];const auto& y=b.riders[i];
        if(x.ownerReady!=y.ownerReady||x.unlock10FAC!=y.unlock10FAC||x.hidden730!=y.hidden730||
           x.careerByteOwnerReady!=y.careerByteOwnerReady||x.career1938!=y.career1938)return false;
    }return true;
}
bool sameCommon(const OriginalUiMainProfile50KG& a,const OriginalUiMainProfile50KG& b) noexcept {
    return a.known==b.known&&a.mode34==b.mode34&&a.previousMode38==b.previousMode38&&a.savedSlot6E0==b.savedSlot6E0&&
        a.availability11018==b.availability11018&&a.flags11054==b.flags11054&&a.playerBits1125C==b.playerBits1125C;
}
bool sameDecision(const game::OriginalUiRouteDecision50KG& a,const game::OriginalUiRouteDecision50KG& b) noexcept {
    const auto& x=a.request;const auto& y=b.request;
    if(a.action!=b.action||a.blockers!=b.blockers||a.admitted!=b.admitted||a.profileProposalComplete!=b.profileProposalComplete||
       a.selectedSlot!=b.selectedSlot||a.selectedRider!=b.selectedRider||a.inputPC!=b.inputPC||x.kind!=y.kind||
       x.sourceScreen!=y.sourceScreen||x.sourcePage!=y.sourcePage||x.targetScreen!=y.targetScreen||x.targetPage!=y.targetPage||
       x.sourcePC!=y.sourcePC||a.profileChanges.count!=b.profileChanges.count||a.profileChanges.count>a.profileChanges.values.size())return false;
    for(std::size_t i=0;i<a.profileChanges.count;++i){const auto& p=a.profileChanges.values[i];const auto& q=b.profileChanges.values[i];
        if(p.field!=q.field||p.operation!=q.operation||p.value!=q.value||p.replaceMask!=q.replaceMask||p.sourcePC!=q.sourcePC)return false;
    }return true;
}
bool sameLife(const game::OriginalUiTransitionSnapshot50KG& a,const game::OriginalUiTransitionSnapshot50KG& b) noexcept {
    return a.bound==b.bound&&a.current.screen==b.current.screen&&a.current.page==b.current.page&&
        a.requested.screen==b.requested.screen&&a.requested.page==b.requested.page&&a.lifecycle==b.lifecycle&&a.phase==b.phase&&
        a.applicationRequestQueued==b.applicationRequestQueued&&a.initialRestartPending==b.initialRestartPending;
}
bool confirmShape(const game::OriginalUiRouteDecision50KG& d) noexcept {
    using F=game::OriginalUiProfileField50KG;using W=game::OriginalUiProfileWrite50KG;
    if(d.inputPC!=0x1EA460||!preparedRider(d.selectedRider)||!d.profileProposalComplete||d.profileChanges.count!=3||
       d.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||d.request.sourceScreen!=5||d.request.sourcePage!=1||
       d.request.targetScreen!=6||d.request.targetPage!=1||d.request.sourcePC!=0x1EA604)return false;
    const auto& a=d.profileChanges.values[0];const auto& b=d.profileChanges.values[1];const auto& c=d.profileChanges.values[2];
    return a.field==F::riderMode0_6F0&&a.operation==W::word&&a.value==static_cast<std::uint32_t>(d.selectedRider)&&a.replaceMask==0xFFFFFFFFu&&a.sourcePC==0x1EA538&&
        b.field==F::rider1Bits1125D&&b.operation==W::maskedByte&&b.value==(static_cast<std::uint32_t>(d.selectedRider)<<3)&&b.replaceMask==0xF8&&b.sourcePC==0x1EA58C&&
        c.field==F::comparisonRider100C0&&c.operation==W::word&&c.value==static_cast<std::uint32_t>(d.selectedRider)&&c.replaceMask==0xFFFFFFFFu&&c.sourcePC==0x1EA5A8;
}
void controlState(game::OriginalUiControlState50KG& s,Operation op,bool value) noexcept {
    if(op==Operation::show||op==Operation::reset)s.shown=true;
    else if(op==Operation::hide)s.shown=false;
    else if(op==Operation::select)s.selected=true;
    else if(op==Operation::deselect||op==Operation::clearSelection)s.selected=false;
    else if(op==Operation::setEnabled)s.enabled=value;
}
bool appendControl(game::OriginalUiNavigation50KG& nav,game::OriginalUiCommands50KG& commands,
    std::int32_t control,Operation op,bool value,std::uint32_t pc) noexcept {
    if(control==-1)return true;
    if(!nav.page||control<0||std::size_t(control)>=nav.page->controls.size()||commands.count==commands.values.size())return false;
    commands.values[commands.count++]={static_cast<std::uint16_t>(control),op,value,pc};
    controlState(nav.controls[static_cast<std::size_t>(control)],op,value);return true;
}
bool groupMembers(game::OriginalUiNavigation50KG& nav,game::OriginalUiCommands50KG& commands,
    std::size_t id,Operation op,bool value,std::uint32_t memberPC,std::uint32_t associationPC) noexcept {
    if(!nav.page||id>=nav.page->groups.size())return false;
    for(auto c:nav.page->groups[id].controls)if(!appendControl(nav,commands,c,op,value,memberPC))return false;
    for(auto c:nav.page->groups[id].associations)if(!appendControl(nav,commands,c,op,value,associationPC))return false;
    return true;
}
}
struct OriginalUiRiderSession50KG::Storage {
    // Destruction order is intentional: every lazy application/cache/resource
    // owner and backend outlives DynamicOwner; collector drains before either.
    OriginalUiRiderSessionCold50KG cold;
    game::OriginalUiRiderBackend50KG backend;
    game::OriginalUiRiderDynamicOwner50KG dynamic;
    game::OriginalUiApertureOwner50KG aperture;
    game::OriginalUiRiderPanel50KG panel;
    OriginalUiRiderDrawContribution50KG collector;
    game::OriginalUiRiderCamera50KG camera;
    game::OriginalUiRiderRenderContext50KG renderContext;
    game::OriginalUiRiderLighting50KG lighting;
    game::OriginalUiRiderLightCapture50KG savedLighting;
    std::array<std::vector<std::string>,3> localized; //167,168,172
    OriginalUiRiderSessionSnapshot50KG state;
    Diagnostic callbackFault;
    game::OriginalUiRiderSelectionProfile50KG pendingProfile;
    OriginalUiMainProfile50KG pendingCommon;
    game::OriginalUiRouteDecision50KG pendingBack;
    std::uint8_t pendingPacked1125D=0;std::uint32_t pendingComparison100C0=0;
    OriginalUiRiderProfileStamp50KG pendingStamp;
    game::OriginalUiTransitionSnapshot50KG pendingLifecycle;
    OriginalUiMainSessionUpdate50KG preparedUpdate;
    OriginalUiMainAudio50KG::RiderConfirmPlayback confirmPlayback;
    std::uint64_t proposalSerial=0,lightGeneration=0;
    bool dependenciesPrepared=false,inOperation=false,rendering=false,paused=false,updatePrepared=false;
    bool requested=false,entered=false,managerEntered=false,opening=false,opened=false,closing=false,restored=false,closed=false;
    bool backAccepted=false,confirmAccepted=false,entryAuxiliarySeen=false,entryCompleted=false;
    bool backendBound=false,collectorBound=false,sourceClosing=false,sourceTornDown=false;
    // No fallible source destruction is first attempted by a destructor.
    // Failed private cold bind has never acquired a graph/helper domain.
    static bool consumeMaterialEvents(void* context,bool rebuild) noexcept {
        auto& s=*static_cast<Storage*>(context);
        if(!s.collector.consumeMaterials(rebuild,s.callbackFault.draw)){
            s.state.faulted=true;s.callbackFault.status=Status::draw;return false;
        }
        return true;
    }
    game::OriginalUiRiderWidgets50KG& widgets() noexcept {return *cold.resources->widgets();}
    const game::OriginalUiRiderWidgets50KG& widgets() const noexcept {return *cold.resources->widgets();}
    const game::OriginalUiNavigation50KG* nav() const noexcept {return widgets().navigation(0);}
    const game::OriginalUiRiderSelectionData50KG* selection() const noexcept {return widgets().selectionData();}
    bool operational() const noexcept {return dependenciesPrepared&&!sourceClosing&&!state.faulted&&!state.quitRequested&&!dynamic.poisoned()&&!widgets().poisoned();}
    bool audioReady() const noexcept {
        return cold.audioMode==OriginalUiMainAudioMode50KG::muted?!cold.audio:
            cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&cold.audio&&cold.audio->riderMenuReady();
    }
    bool backAudioReady() const noexcept {
        return audioReady()&&(cold.audioMode==OriginalUiMainAudioMode50KG::muted||cold.audio->canPlayRiderBack5());
    }
    bool backAudio() noexcept {
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return true;}
        if(!audioReady()||!cold.audio->playRiderBack5()){
            ++state.audio.failed;fault(Status::audio,0x1EA848);return false;
        }
        ++state.audio.submitted;return true;
    }
    bool selectedConfirmCharacter(std::uint16_t& character) const noexcept {
        const auto* n=nav();if(!n||!n->page||n->page->groups.size()<=1||n->controllerGroup[0]!=1||
           !state.slot4C8||n->groups[1].selectedSlot!=state.slot4C8||
           state.slot4C8>n->page->groups[1].controls.size()||!preparedRider(state.rider4C4))return false;
        const auto control=n->page->groups[1].controls[state.slot4C8-1];
        if(control<0||static_cast<std::size_t>(control)>=n->page->controls.size())return false;
        const auto& ids=n->page->controls[control].widgets;
        // 1ECF40 tests size4; 1ECF70 returns the exact fourth member.
        if(ids.size()!=4||ids[3]!=(state.rider4C4==19?53:54)||widgets().state(ids[3]).kind!=Kind::character||
           !backend.attached(ids[3]))return false;
        const auto actual=dynamic.state(ids[3]);
        if(actual.kind!=Kind::character||!actual.selected||!actual.shown)return false;
        character=ids[3];return true;
    }
    bool confirmAnimationPreflight(game::OriginalUiRiderDynamicDiagnostic50KG& d) const noexcept {
        std::uint16_t character=0;
        return selectedConfirmCharacter(character)&&dynamic.preflightConfirmCharacter(character,d);
    }
    bool confirmVoiceChronologyReady() const noexcept {
        game::OriginalUiRiderVoiceChoiceDiagnostic50KG diagnostic;
        return boundedRiderConfirmEnabled&&cold.application->clock101A50&&
            cold.application->riderVoiceChoice.preflight(state.rider4C4,diagnostic);
    }
    bool confirmAudioReady() const noexcept {
        return audioReady()&&(cold.audioMode==OriginalUiMainAudioMode50KG::muted||cold.audio->canPlayRiderConfirm());
    }
    bool applyConfirmVoiceChronology() noexcept {
        if(!confirmVoiceChronologyReady()||!confirmAudioReady()){
            fault(Status::unsupportedState,0x1EA694);return false;}
        if(!cold.application->riderVoiceChoice.choose(state.rider4C4,*cold.application->clock101A50,
            cold.application->sharedNoise308030,cold.application->sharedRandomSelectors,
            state.voice,callbackFault.voice)){
            fault(Status::audio,callbackFault.voice.sourcePC);return false;}
        // The actual choice precedes1EA780 in both modes. Enabled output plays
        // that exact original UIACCEPT event, without an extra RNG draw.
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted)++state.audio.suppressedByMutedMode;
        else if(!cold.audio->playRiderConfirmVoice(state.voice.soundId,confirmPlayback)){
            ++state.audio.failed;fault(Status::audio,0x1EA780);return false;
        }else ++state.audio.submitted;
        return true;
    }
    bool confirmReady(game::OriginalUiRiderDynamicDiagnostic50KG& d) const noexcept {
        return confirmAudioReady()&&cold.prospectiveMode==0&&
            cold.commonProfile->mode34==0&&(cold.commonProfile->playerBits1125C&0x60)==0x20&&
            confirmAnimationPreflight(d)&&confirmVoiceChronologyReady();
    }
    bool processCurrent() const noexcept {
        return cold.application->known&&cold.commonProfile->known&&cold.profile->commonProfile()==cold.commonProfile.get()&&
            !(cold.application->globalFlags228&4u)&&cold.application->transportE00==-1&&
            &cold.application->rawRandom308030==&cold.application->sharedNoise308030.sharedRandomState&&audioReady();
    }
    bool liveProfile(game::OriginalUiRiderSelectionProfile50KG& p) const noexcept {
        return processCurrent()&&cold.commonProfile->mode34==cold.prospectiveMode&&cold.profile->selectionSnapshot(p);
    }
    game::OriginalUiTransitionSnapshot50KG transition() const noexcept {return cold.externalTransition->snapshot();}
    bool currentPage() const noexcept {const auto t=transition();return t.bound&&t.current.screen==5&&t.current.page==1;}
    bool lightCurrent() const noexcept {
        return cold.lightManager->cacheState()==game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction&&
            cold.lightManager->activeUiIdentity()==cold.riderArchive->identity()&&lightGeneration&&
            cold.lightManager->managerGeneration()==lightGeneration;
    }
    bool idleInput() const noexcept {
        const auto t=transition();const auto a=aperture.snapshot();
        return currentPage()&&entryCompleted&&opened&&!closing&&!backAccepted&&!confirmAccepted&&state.pageShown&&state.inputContainerShown&&
            t.requested.screen==5&&t.requested.page==1&&t.lifecycle==Life::idle&&t.phase==Phase::none&&!t.initialRestartPending&&
            !t.applicationRequestQueued&&a.inputEnabled268&&a.state==game::OriginalUiApertureState50KG::open&&
            !panel.inputBlocked4E0()&&panel.offset4DC()==0;
    }
    bool invalidateProposal() noexcept {
        state.backProposalPending=false;state.confirmProposalPending=false;
        if(proposalSerial==std::numeric_limits<std::uint64_t>::max()){fault(Status::unsupportedState,0x1EA460);return false;}
        ++proposalSerial;return true;
    }
    void cpuDrain() noexcept {collector.drain();state.framePublished=false;}
    void fault(Status status,std::uint32_t pc) noexcept {
        if(!state.faulted){state.faulted=true;callbackFault.status=status;callbackFault.sourcePC=pc;}
        state.backProposalPending=false;state.confirmProposalPending=false;state.framePublished=false;
    }
    bool navigate(const game::OriginalUiNavigation50KG& next,const game::OriginalUiCommands50KG& commands,std::uint32_t pc) noexcept {
        if(!widgets().commitNavigation(0,next,commands,dynamic.hooks(),callbackFault.widget)){
            callbackFault.dynamic=dynamic.diagnostic();callbackFault.backend=backend.diagnostic();fault(Status::widget,pc);return false;
        }return true;
    }
    bool widget(std::uint16_t id,Operation op,bool value,std::uint32_t pc) noexcept {
        const game::OriginalUiRiderWidgetCallback50KG call{id,op,value,pc};
        if(!widgets().apply(&call,1,dynamic.hooks(),callbackFault.widget)){fault(Status::widget,pc);return false;}return true;
    }
    bool groupEnabled(std::size_t id,bool value,std::uint32_t pc) noexcept {
        auto next=*nav();game::OriginalUiCommands50KG commands;
        if(id>=next.page->groups.size()){fault(Status::widget,pc);return false;}
        next.groups[id].enabled=value;
        if(!groupMembers(next,commands,id,Operation::setEnabled,value,0x2A5044,0x2A5088)){fault(Status::widget,pc);return false;}
        return navigate(next,commands,pc);
    }
    bool requested90() noexcept {
        auto next=*nav();game::OriginalUiCommands50KG commands;next.controllerGroup={{-1,-1}};
        for(std::size_t group=0;group<next.page->groups.size();++group)
            if(!groupMembers(next,commands,group,Operation::reset,false,0x2A57D8,0x2A5818)){fault(Status::widget,0x2A6660);return false;}
        if(!navigate(next,commands,0x1ECE00))return false;
        state.pageShown=true;requested=true;return true;
    }
    bool entry74() noexcept {
        game::OriginalUiRiderSelectionProfile50KG profile;game::OriginalUiRiderSelectionPlan50KG plan;
        if(!liveProfile(profile)||game::planOriginalUiRiderEntry50KG(*selection(),*nav(),0,profile,plan)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           !preparedRider(plan.rider4C4)){fault(Status::profile,0x1E9C50);return false;}
        if(cold.lightManager->activeUiIdentity()!=cold.riderArchive->identity()||
           cold.lightManager->cacheState()!=game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction){fault(Status::lighting,0x1CDB10);return false;}
        lightGeneration=cold.lightManager->managerGeneration();
        if(!lightGeneration||!cold.lightManager->publishRiderRegistry(lighting,callbackFault.lightManager)){fault(Status::lighting,0x1CDB10);return false;}
        state.promptMask42C=0;panel.entry74();state.fadeMusic4E8=true;
        if(!widget(4,Operation::setEnabled,false,0x1E9C80)||!groupEnabled(2,false,0x1E9C90))return false;
        // Remembered search publishes page fields at74, but selection/load is
        // still deferred to the source openingAC group-entry call.
        state.rider4C4=plan.rider4C4;state.slot4C8=plan.slot4C8;
        state.cameraActive=true;cold.application->uiObject3D8=0;entered=true;return true;
    }
    bool appearance() noexcept {
        game::OriginalUiRiderSelectionProfile50KG p;game::OriginalUiRiderAppearance50KG a;
        if(!liveProfile(p)||game::projectOriginalUiRiderAppearance50KG(state.rider4C4,p,a)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           !game::applyOriginalUiRiderLighting50KG(a.useSavedColors,savedLighting,lighting)){fault(Status::profile,0x1EAFB0);return false;}
        for(std::size_t i=0;i<a.count;++i)if(!widget(a.widgets[i].widget,Operation::setEnabled,a.widgets[i].enable,a.widgets[i].sourcePC))return false;
        state.promptMask42C=a.promptMask42C;return true;
    }
    bool openingAC() noexcept {
        game::OriginalUiRiderSelectionProfile50KG p;game::OriginalUiRiderSelectionPlan50KG plan;
        if(!entered||opening||!lightCurrent()||!liveProfile(p)||
           game::planOriginalUiRiderEntry50KG(*selection(),*nav(),0,p,plan)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           !preparedRider(plan.rider4C4)||plan.rider4C4!=state.rider4C4||plan.slot4C8!=state.slot4C8||
           !game::captureOriginalUiRiderLighting50KG(lighting,savedLighting)){fault(Status::unsupportedState,0x1E9E70);return false;}
        if(!navigate(plan.nextNavigation,plan.navigationCommands,0x1E9FD4))return false;
        const auto control=nav()->page->groups[1].controls[state.slot4C8-1];const auto& ids=nav()->page->controls[control].widgets;
        if(ids.size()!=4||!backend.attached(ids[0])||!backend.attached(ids[3])||
           !dynamic.podScene(ids[0])||!dynamic.podScene(ids[0])->loaded()||!dynamic.characterScene(ids[3])||
           !dynamic.characterScene(ids[3])->attached()){fault(Status::model,0x1E9FD4);return false;}
        state.selectionAttached=true;
        if(!appearance())return false;state.promptMask42C=0;
        // Source1EA03C checks the actual predecessor.234900(7,1) restarts
        // the shared menu stream after appearance, before Group2 disable.
        // Initial entry starts no character voice and consumes no audio RNG.
        if(cold.previousScreen==5)++state.audio.suppressedBySourceGate;
        else if(cold.audioMode==OriginalUiMainAudioMode50KG::muted)++state.audio.suppressedByMutedMode;
        else if(!audioReady()||!cold.audio->startRiderLoop7()){
            ++state.audio.failed;fault(Status::audio,0x1EA060);return false;
        }else ++state.audio.submitted;
        if(!groupEnabled(2,false,0x1EA070))return false;opening=true;return true;
    }
    bool openedB0() noexcept {
        if(!opening||opened||closing||!groupEnabled(2,true,0x1EA0AC)){fault(Status::unsupportedState,0x1EA090);return false;}
        const auto mode=cold.commonProfile->mode34;
        if(!widgets().setDynamicLines(87,localized[mode==2?1:0],callbackFault.widget)){
            fault(Status::widget,mode==2?0x1EA158:0x1EA1F0);return false;}
        if(mode!=2&&!widgets().setDynamicLines(6,localized[2],callbackFault.widget)){fault(Status::widget,0x1EA294);return false;}
        if(!appearance())return false;opened=true;return true;
    }
    bool closingB4() noexcept {
        if(!opening||closing||(!backAccepted&&!confirmAccepted)){fault(Status::unsupportedState,0x1EA360);return false;}
        state.promptMask42C=0;
        if(!groupEnabled(2,false,0x1EA37C))return false;
        const auto* n=nav();if(!state.slot4C8||state.slot4C8>n->page->groups[1].controls.size()){fault(Status::widget,0x1EA38C);return false;}
        const auto control=n->page->groups[1].controls[state.slot4C8-1];
        if(control<0||std::size_t(control)>=n->page->controls.size()||n->page->controls[control].widgets.size()<2){fault(Status::widget,0x1EA394);return false;}
        // Exact SECOND widget, the selected rider's ordinary name text.
        if(!widget(n->page->controls[control].widgets[1],Operation::setEnabled,false,0x1EA39C))return false;
        if(state.fadeMusic4E8){
            if(cold.audioMode==OriginalUiMainAudioMode50KG::muted)++state.audio.suppressedByMutedMode;
            else if(!audioReady()||!cold.audio->fadeRiderMusic()){
                ++state.audio.failed;fault(Status::audio,0x1EA3C0);return false;
            }else ++state.audio.submitted;
        }
        closing=true;return true;
    }
    bool restoreB8() noexcept {
        if(!closing||restored||!game::applyOriginalUiRiderLighting50KG(true,savedLighting,lighting)){
            fault(Status::lighting,0x1EA3E0);return false;}
        restored=true;return true;
    }
    bool pageColors() noexcept {
        OriginalUiRiderProfileColors50KG colors;
        if(!cold.profile->colors(state.rider4C4,colors)){fault(Status::profile,0x1EB674);return false;}
        // First source1EB640 affects live DAT objects; custom stat objects are
        //then updated by collector.projectPage at its unchanged source point.
        for(auto id:{2,1,7,6})if(!widgets().setColor(static_cast<std::uint16_t>(id),colors.secondary17FA10,callbackFault.widget)){
            fault(Status::widget,0x1EB690);return false;}
        for(auto id:{87,88,89,90,4,117,118,119})if(!widgets().setColor(static_cast<std::uint16_t>(id),colors.accent17FA00,callbackFault.widget)){
            fault(Status::widget,0x1EB6F8);return false;}
        return true;
    }
    static bool available(void* context) noexcept {
        auto& s=*static_cast<Storage*>(context);const auto* exit=s.cold.resources->exitText(0);
        return s.operational()&&exit&&exit->prepared()&&s.audioReady();
    }
    static void hook(void* context,const game::OriginalUiApertureHook50KG& e) noexcept {
        auto& s=*static_cast<Storage*>(context);if(!s.operational())return;
        switch(e.kind){
        case Hook::openingAC:if(e.targetPC!=0x1E9E70)s.fault(Status::aperture,e.callPC);else s.openingAC();break;
        case Hook::openedB0:if(e.targetPC!=0x1EA090)s.fault(Status::aperture,e.callPC);else s.openedB0();break;
        case Hook::closingB4:if(e.targetPC!=0x1EA360)s.fault(Status::aperture,e.callPC);else s.closingB4();break;
        case Hook::closingThresholdB8:if(e.targetPC!=0x1EA3E0)s.fault(Status::aperture,e.callPC);else s.restoreB8();break;
        case Hook::closedBC:
            if(e.targetPC!=0x1D7A00||!s.closing||!s.restored||s.closed)s.fault(Status::aperture,e.callPC);else s.closed=true;break;
        case Hook::screenExitObject440:
            if(!s.rendering||!s.closing||!s.collector.projectExitText(e,s.callbackFault.draw))s.fault(Status::draw,e.callPC);break;
        }
    }
    static void overlay(void* context,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& r) noexcept {
        auto& s=*static_cast<Storage*>(context);if(!s.operational())return;
        if(!s.rendering||!s.collector.projectAperture(ordinal,r,s.callbackFault.draw))s.fault(Status::draw,0x1D798C);
    }
    bool updateWidgets(const std::vector<std::uint16_t>& order,float dt) noexcept {
        for(auto id:order){const auto kind=widgets().state(id).kind;
            if(kind==Kind::text){if(!widgets().updateText(id,true,callbackFault.widget)&&
                callbackFault.widget.error!=game::OriginalUiRiderWidgetError50KG::hidden){fault(Status::widget,0x141ED0);return false;}}
            else if(kind==Kind::pod||kind==Kind::character){
                if(!dynamic.advanceWidget(id,dt,true)){callbackFault.dynamic=dynamic.diagnostic();fault(Status::model,0x141ED0);return false;}
            }
        }return true;
    }
};
OriginalUiRiderSession50KG::OriginalUiRiderSession50KG() noexcept=default;
OriginalUiRiderSession50KG::~OriginalUiRiderSession50KG(){
    if(sourceTeardownPending()){
        // Terminal contract misuse, not successful cleanup or a retry. Retain
        // every callback/graph/token dependency instead of freeing an object
        // still borrowed by the renderer. Normal frontend close retains the
        // accessible session and reports the exact failure until retry succeeds.
        storage_.release();
    }
}

Status OriginalUiRiderSession50KG::bind(const OriginalUiRiderSessionCold50KG& cold,Diagnostic& d) noexcept {
    return bindCold(cold,game::OriginalUiRiderColdPopulation50KG::rider19And20,d);
}
Status OriginalUiRiderSession50KG::bindCold(const OriginalUiRiderSessionCold50KG& cold,
    game::OriginalUiRiderColdPopulation50KG population,Diagnostic& d) noexcept {
    d={};if(storage_)return result(d,Status::alreadyBound);
    if(population!=game::OriginalUiRiderColdPopulation50KG::selected19&&
       population!=game::OriginalUiRiderColdPopulation50KG::rider19And20)return result(d,Status::invalidInput);
    const bool fromMain=cold.previousScreen==4&&cold.predecessorResources&&
        !cold.predecessorCourseResources&&!cold.predecessorCourseHolograms&&
        cold.predecessorResources->loaded()&&cold.predecessorResources->materials()&&cold.predecessorResources->particleMaterial();
    const bool fromCourse=cold.previousScreen==6&&!cold.predecessorResources&&
        cold.predecessorCourseResources&&cold.predecessorCourseHolograms&&
        cold.predecessorCourseResources->loaded()&&cold.predecessorCourseResources->widgets()&&
        cold.predecessorCourseResources->materials()&&cold.predecessorCourseHolograms->loaded()&&
        cold.prospectiveMode==0;
    if((!fromMain&&!fromCourse)||!cold.resources||!cold.resources->loaded()||!cold.resources->widgets()||!cold.resources->bankInput()||
       !cold.resources->exitText(0)||!cold.resources->exitText(0)->prepared()||!cold.statAssets||!cold.statAssets->loaded()||
       !cold.binderAssets||!cold.binderAssets->loaded()||!genuine(cold.riderArchive,assets::OriginalUiArchiveId50KG::rider)||
       !genuine(cold.appStart,assets::OriginalUiArchiveId50KG::appStart)||!genuine(cold.podTextures,assets::OriginalUiArchiveId50KG::podTextures)||
       !cold.ipumCache||!cold.ipumCache->loaded()||!cold.english||!cold.english->loaded()||!cold.materialRuntime.overrideKnown)
        return result(d,Status::resource);
    if(!cold.profile||!cold.profile->initialized()||!cold.commonProfile||!cold.commonProfile->known||
       cold.profile->commonProfile()!=cold.commonProfile.get()||!cold.application||!cold.application->known||
       (cold.application->globalFlags228&4u)||cold.application->transportE00!=-1||cold.prospectiveMode>2||(fromCourse&&cold.commonProfile->mode34!=0)||
       &cold.application->rawRandom308030!=&cold.application->sharedNoise308030.sharedRandomState||
       !cold.lightManager||cold.lightManager->cacheState()!=game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction||
       !cold.externalTransition||cold.externalTransition->snapshot().bound)return result(d,Status::unsupportedState);
    if((cold.audioMode==OriginalUiMainAudioMode50KG::muted&&cold.audio)||
       (cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&(!cold.audio||!cold.audio->riderMenuReady()))||
       (cold.audioMode!=OriginalUiMainAudioMode50KG::muted&&cold.audioMode!=OriginalUiMainAudioMode50KG::enabled))
        return result(d,Status::unavailableDestination,0x234900);
    try {
        auto s=std::make_unique<Storage>();s->cold=cold;s->cold.english=nullptr;s->cold.predecessorResources=nullptr;
        s->cold.predecessorCourseResources=nullptr;s->cold.predecessorCourseHolograms=nullptr;
        const auto* page=s->selection()?s->selection()->page():nullptr;
        if(!page||page->groups.size()!=3||page->groups[1].controls.size()!=23||!s->nav()||
           page->useVideoAsTexture||page->background!="*INVALID*"||!cold.resources->background(19)||
           !cold.statAssets->portrait(19)||cold.resources->bankInput()->owners.size()!=1||
           cold.resources->bankInput()->owners[0]!=cold.resources->images())return result(d,Status::resource,0x1D63B0);
        const auto coldControl=[&](std::size_t slot,std::uint16_t control,std::int32_t rider,
            const std::array<std::uint16_t,4>& expected){
            if(slot>=page->groups[1].controls.size()||page->groups[1].controls[slot]!=control||
               control>=page->controls.size()||s->selection()->controlRider(control)!=rider)return false;
            const auto& widgets=page->controls[control].widgets;if(widgets.size()!=expected.size())return false;
            for(std::size_t i=0;i<expected.size();++i)if(widgets[i]!=expected[i])return false;
            return true;
        };
        // Verify actual source memberships without changing navigation or
        // inventing select callbacks to warm a second rider. Both original
        // descriptors remain immutable until actual source selection.
        if(!coldControl(0,16,19,{{27,79,109,53}})||
           (population==game::OriginalUiRiderColdPopulation50KG::rider19And20&&
            (!coldControl(1,17,20,{{28,80,110,54}})||!cold.resources->background(20)||!cold.statAssets->portrait(20))))
            return result(d,Status::resource,0x1D63B0);
        const unsigned indices[]={167,168,172};
        for(unsigned i=0;i<3;++i){const auto* entry=cold.english->at(indices[i]);
            if(!entry||entry->text.size()>20||entry->text.find('\0')!=std::string::npos)return result(d,Status::resource,0x1EA158);
            s->localized[i].push_back(entry->text);
        }
        for(unsigned c=0;c<3;++c){std::int32_t channel=0;
            if(!game::convertOriginalUiApertureCvtWS50KG(page->backgroundColor[c],channel)||channel<0||channel>255)
                return result(d,Status::unsupportedState,0x1D6D24);
            s->state.packedBackground430|=static_cast<std::uint32_t>(channel)<<(16u-8u*c);
        }
        if(!game::prepareOriginalUiRiderCamera50KG(*cold.riderArchive,s->camera,d.camera))return result(d,Status::camera,d.camera.sourcePC);
        if(!game::prepareOriginalUiRiderRenderContext50KG(*cold.riderArchive,s->camera,s->renderContext,d.renderContext))
            return result(d,Status::camera,d.renderContext.sourcePC);
        if(!game::prepareOriginalUiRiderLighting50KG(*cold.riderArchive,s->lighting,d.lighting))return result(d,Status::lighting,d.lighting.sourcePC);
        game::OriginalUiRiderBackendInput50KG backend;backend.rider=cold.riderArchive;backend.appStart=cold.appStart;
        backend.podTextures=cold.podTextures;backend.binder=cold.binderAssets;backend.materialRuntime=cold.materialRuntime;
        backend.staticImages=*cold.resources->bankInput();backend.staticImages.ipumCache=cold.ipumCache.get();
        backend.staticImages.owners.push_back(cold.statAssets->images());
        // Canonical closure comes from actual retained material records, not
        // hand-written basename/image lists. Inventories never acquire owners.
        const auto addStaticRow=[&](const std::string& name){
            game::OriginalUiRiderMaterialRow50KG row;
            if(!game::OriginalUiRiderMaterialRow50KG::prepare(*cold.appStart,name,row,d.backend.lifetime))return false;
            for(const auto& prior:backend.staticRows)if(prior.same(row))return true;
            backend.staticRows.push_back(row);return true;
        };
        if(!cold.resources->materials())return result(d,Status::resource);
        for(const auto& material:*cold.resources->materials())if(!addStaticRow(material.registeredName))return result(d,Status::resource);
        for(unsigned i=0;i<=static_cast<unsigned>(assets::OriginalUiRiderStatAsset50KG::labelFont);++i){
            const auto* material=cold.statAssets->resource(static_cast<assets::OriginalUiRiderStatAsset50KG>(i));
            if(!material||!addStaticRow(material->registeredName))return result(d,Status::resource);
        }
        for(std::uint8_t rider=1;rider<=26;++rider)if(const auto* material=cold.statAssets->portrait(rider))
            if(!addStaticRow(material->registeredName))return result(d,Status::resource);
        if(fromMain){
            for(const auto& material:*cold.predecessorResources->materials())
                if(!addStaticRow(material.registeredName))return result(d,Status::resource);
            const auto* particle=cold.predecessorResources->particleMaterial();
            if(!particle->identity||!particle->identity->name)return result(d,Status::resource);
            std::string particleName=particle->identity->name;
            if(particleName.size()<4||particleName.substr(particleName.size()-4)!=".psm")return result(d,Status::resource);
            particleName.resize(particleName.size()-4);
            if(!addStaticRow(particleName))return result(d,Status::resource);
        }else{
            // Actual Course inventory, no predecessor image/epoch/session replay.
            for(const auto& material:*cold.predecessorCourseResources->materials())
                if(!addStaticRow(material.registeredName))return result(d,Status::resource);
            const auto* widgets=cold.predecessorCourseResources->widgets();std::size_t count=0;
            for(std::uint16_t id=1;id<85;++id){
                if(widgets->state(id).kind!=game::OriginalUiCourseWidgetKind50KG::character)continue;
                const auto* asset=cold.predecessorCourseHolograms->modelAssets(id);
                const auto* material=cold.predecessorCourseHolograms->material(id);
                if(!asset||!asset->loaded()||asset->widgetId()!=id||!asset->scene()||!material||
                   !material->identity||!material->identity->name||material->passes.size()!=2)return result(d,Status::resource);
                std::string name=material->identity->name;
                if(name.size()<4||name.substr(name.size()-4)!=".psm")return result(d,Status::resource);
                name.resize(name.size()-4);if(!addStaticRow(name))return result(d,Status::resource);++count;
            }
            // Two passes share one PSM row. Raw background has no material row.
            if(count!=13)return result(d,Status::resource);
        }
        if(!s->backend.bind(backend,d.backend))return result(d,Status::backend);
        s->backendBound=true;
        if(!s->dynamic.bind(cold.riderArchive,s->widgets(),cold.application->modelMotion,cold.application->modelMode0Ik,
            cold.application->modelLibm,cold.application->sharedNoise308030,s->backend.callbacks(),d.dynamic))return result(d,Status::model,d.dynamic.sourcePC);
        game::OriginalUiRiderSelectionProfile50KG copiedProfile;game::OriginalUiRiderSelectionPlan50KG coldPlan;
        // Main's live mode4 is legitimate. Only this immutable cold projection
        // sees the exact prospective source decision; the shared word is untouched.
        if(!cold.profile->selectionSnapshotForMode(cold.prospectiveMode,copiedProfile)||
           game::planOriginalUiRiderEntry50KG(*s->selection(),*s->nav(),0,copiedProfile,coldPlan)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           !preparedRider(coldPlan.rider4C4))return result(d,Status::unavailableDestination,0x1E9C50);
        // This local session has not escaped. Prepare genuine immutable
        // descriptor candidates, then one complete bank and irreversible seal.
        // Subsequent callback preflight only reads that sealed population.
        if(!s->dynamic.prepareCold(population,d.dynamic))return result(d,Status::resource,d.dynamic.sourcePC);
        if(!s->backend.prepareColdAndSeal(s->dynamic,d.backend))return result(d,Status::backend);
        if(!s->widgets().preflightNavigation(0,coldPlan.nextNavigation,coldPlan.navigationCommands,s->dynamic.hooks(),d.widget)){
            d.dynamic=s->dynamic.diagnostic();d.backend=s->backend.diagnostic();return result(d,Status::resource,0x1E9FD4);}
        // All callback implementations and their original cold resources now
        // exist. bind invokes no hook; the local object cannot escape until the
        // full aperture/panel/composer ownership cycle is established below.
        s->dependenciesPrepared=true;
        game::OriginalUiApertureCallbacks50KG callbacks{s.get(),&Storage::available,&Storage::hook,&Storage::overlay};
        if(s->aperture.bindRider(cold.riderArchive,1,callbacks,d.aperture)!=ApertureStatus::ok)return result(d,Status::aperture,d.aperture.sourcePC);
        if(!s->panel.bind(s->aperture,*s->selection(),0))return result(d,Status::aperture,0x1ECFE0);
        const std::array<const game::OriginalUiApertureOwner50KG*,2> apertures{{&s->aperture,nullptr}};
        if(!s->collector.bind(cold.resources,cold.statAssets,cold.binderAssets,s->backend,s->dynamic,*cold.lightManager,
            0,1,cold.externalTransition,apertures,d.draw))return result(d,Status::draw,d.draw.sourcePC);
        s->collectorBound=true;
        // Every material/pass/image policy is admitted before the coordinator
        // can approve entry. This warms real retained owners only: no model is
        // attached, no navigation/widget state changes and no RNG is consumed.
        if(!s->collector.prepareBank(d.draw))return result(d,Status::draw,d.draw.sourcePC);
        if(!s->backend.bindMaterialConsumer(&s->collector,s.get(),&Storage::consumeMaterialEvents))return result(d,Status::backend);
        s->state.bound=true;storage_=std::move(s);return result(d,Status::ok);
    }catch(const std::bad_alloc&){return result(d,Status::allocation);}catch(...){return result(d,Status::resource);}
}
bool OriginalUiRiderSession50KG::canEnterMode(std::uint32_t mode) const noexcept {
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||mode!=s->cold.prospectiveMode||!s->processCurrent())return false;
    game::OriginalUiRiderSelectionProfile50KG profile;game::OriginalUiRiderSelectionPlan50KG plan;
    return s->cold.profile->selectionSnapshotForMode(mode,profile)&&
        game::planOriginalUiRiderEntry50KG(*s->selection(),*s->nav(),0,profile,plan)==game::OriginalUiRiderSelectionStatus50KG::ok&&
        preparedRider(plan.rider4C4);
}
std::uint32_t OriginalUiRiderSession50KG::intendedMode() const noexcept {
    return storage_?storage_->cold.prospectiveMode:std::numeric_limits<std::uint32_t>::max();
}
bool OriginalUiRiderSession50KG::ownsPreparedPage(std::uint8_t page) const noexcept {
    return page==1&&storage_&&canEnterMode(storage_->cold.prospectiveMode);
}
const game::OriginalUiTransitionOwner50KG* OriginalUiRiderSession50KG::externalLifecycleOwner() const noexcept {
    return storage_?storage_->cold.externalTransition.get():nullptr;
}
Status OriginalUiRiderSession50KG::notifyTransition(const game::OriginalUiTransitionNotification50KG& e,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(!s->operational()||s->inOperation||s->rendering||e.route.screen!=5||e.route.page!=1)return result(d,Status::unsupportedState,e.sourcePC);
    const auto t=s->transition();
    if(!t.bound||t.current.screen!=5||t.current.page!=1||e.lifecycle!=t.lifecycle||e.phase!=t.phase||!s->processCurrent())
        return result(d,Status::invalidInput,e.sourcePC);
    // Borrowed sealed vertices must be released before any model/widget
    // callback. Actual GPU/source-owner retirement remains the frontend's job.
    s->cpuDrain();if(!s->invalidateProposal()){d=s->callbackFault;return d.status;}s->inOperation=true;
    bool valid=true;
    switch(e.kind){
    case Notice::requestedPage90:
        valid=!s->requested&&t.initialRestartPending&&t.lifecycle==Life::idle&&t.phase==Phase::none&&e.sourcePC==0x2A3C44;
        if(valid)s->requested90();break;
    case Notice::pageEntry74:
        valid=s->requested&&!s->entered&&t.initialRestartPending&&t.lifecycle==Life::idle&&t.phase==Phase::none&&e.sourcePC==0x2A3C58;
        if(valid)s->entry74();break;
    case Notice::managerEntry84:
        valid=s->entered&&!s->managerEntered&&t.lifecycle==Life::initialEntry&&t.phase==Phase::none&&e.sourcePC==0x2A3D68;
        if(valid)s->managerEntered=true;break; // Actual inherited2A4330 return.
    case Notice::beginTransition7C:
        if(t.phase==Phase::entry){valid=s->managerEntered&&!s->opening&&t.lifecycle==Life::waitPageTransition&&e.sourcePC==0x2A3D8C;}
        else valid=t.phase==Phase::exit&&(s->backAccepted||s->confirmAccepted)&&!s->closing&&t.lifecycle==Life::waitScreenExit&&e.sourcePC==0x2A3DFC;
        if(valid&&!apertureOkay(s->panel.begin7C(t.phase)))s->fault(Status::aperture,e.sourcePC);break;
    case Notice::pageAuxiliary1C:
        if(e.sourcePC==0x2A3DA4)valid=s->opening&&!s->entryAuxiliarySeen&&t.lifecycle==Life::waitPageTransition&&t.phase==Phase::entry;
        else valid=e.sourcePC==0x2A3E8C&&s->closed&&(s->backAccepted||s->confirmAccepted)&&t.lifecycle==Life::waitScreenExit&&t.phase==Phase::exit;
        if(valid){s->state.inputContainerShown=false;if(e.sourcePC==0x2A3DA4)s->entryAuxiliarySeen=true;}break;
    case Notice::pageAuxiliary24:
        valid=e.sourcePC==0x2A3EE0&&s->entryAuxiliarySeen&&!s->entryCompleted&&s->opened&&
            t.lifecycle==Life::waitPageTransition&&t.phase==Phase::entry&&s->panel.pollCompletion(Phase::entry)==Completion::complete;
        if(valid){s->state.inputContainerShown=true;s->entryCompleted=true;}break;
    case Notice::previousPage94:
        // Actual Rider1ECE10 ->2A6E90 disables its page entity. A page change
        // cannot be admitted in this bounded page1-only owner.
        valid=false;break;
    }
    if(!valid)s->fault(Status::transition,e.sourcePC);
    s->inOperation=false;if(s->state.faulted){s->cpuDrain();d=s->callbackFault;return d.status;}return result(d,Status::ok);
}
Completion OriginalUiRiderSession50KG::pollCompletion(game::OriginalUiTransitionRoute50KG route,Phase phase) const noexcept {
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||route.screen!=5||route.page!=1||!s->currentPage()||
        !s->processCurrent()||!s->lightCurrent())return Completion::unavailable;
    const auto t=s->transition();if(t.phase!=phase)return Completion::unavailable;
    if(phase==Phase::entry&&!s->opening)return Completion::unavailable;
    if(phase==Phase::exit&&!s->backAccepted&&!s->confirmAccepted)return Completion::unavailable;
    return s->panel.pollCompletion(phase);
}
Status OriginalUiRiderSession50KG::input(const OriginalUiMainSessionInput50KG& in,OriginalUiRiderSessionInputReceipt50KG& receipt,Diagnostic& d) noexcept {
    d={};receipt={};auto* s=storage_.get();
    const auto finish=[&](Status status,std::uint32_t pc=0){receipt.status=status;return result(d,status,pc);};
    if(!s)return finish(Status::notBound);if(s->state.faulted)return finish(Status::faulted);if(s->state.quitRequested)return finish(Status::quit);
    if(s->inOperation||s->updatePrepared||in.controller>1||in.originalInput>15)return finish(Status::invalidInput);
    s->cpuDrain();if(!s->invalidateProposal()){d=s->callbackFault;receipt.status=d.status;return d.status;}
    receipt.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
    if(!receipt.containerReached||!s->aperture.snapshot().inputEnabled268)return finish(Status::noChange);
    receipt.pageCallbackRan=true;
    if(in.controller!=0||in.popupActive||in.sourceInputBlocked)return finish(Status::noChange);
    if(!s->processCurrent()||!s->lightCurrent()||s->cold.application->uiObject3D8)return finish(Status::unsupportedState,0x1EA460);
    if(s->state.deferred4EC)return finish(Status::noChange,0x1EB510);
    if(s->panel.inputBlocked4E0())return finish(in.originalInput==4?Status::unavailableDestination:Status::noChange,0x1EA460);
    game::OriginalUiRiderSelectionProfile50KG profile;
    if(!s->liveProfile(profile))return finish(Status::profile,0x1EA460);
    if(in.originalInput<=1){
        const auto planned=game::planOriginalUiRiderMove50KG(*s->selection(),*s->nav(),0,in.controller,in.originalInput,
            in.popupActive,s->panel.inputBlocked4E0(),profile,receipt.selection);
        if(planned==game::OriginalUiRiderSelectionStatus50KG::noChange)return finish(Status::noChange);
        if(planned!=game::OriginalUiRiderSelectionStatus50KG::ok)return finish(Status::unsupportedState,0x1EB1F0);
        // Preserve authored order. Do not wrap a two-rider substitute roster:
        // a next/previous reaching any other rider remains unavailable.
        if(!s->idleInput()||receipt.selection.visitedControls!=1||!preparedRider(receipt.selection.rider4C4)||
           !preparedRider(s->state.rider4C4)||!s->audioReady()||
           (s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&!s->cold.audio->canPlayRiderAdvance2()))
            return finish(Status::unavailableDestination,receipt.selection.sourcePC);
        receipt.moveSerial=s->proposalSerial;receipt.moveEvent=in.originalInput;
        return finish(Status::moveProposed,receipt.selection.sourcePC);
    }
    if(in.originalInput!=4&&in.originalInput!=6)return finish(in.originalInput==5?Status::unavailableDestination:Status::noChange,0x1EA460);
    game::OriginalUiRiderContext50KG context;context.playerIndex=0;context.selectedRider=s->state.rider4C4;context.mode34=profile.mode34;
    context.packed1125C=s->cold.commonProfile->playerBits1125C;
    context.admission.sourceControllerReady=true;context.admission.selectionOwnerReady=true;context.admission.profileOwnerReady=true;
    context.unlockOwnerReady=profile.riders[s->state.rider4C4].ownerReady;
    context.selectedUnlockWord10FAC=profile.riders[s->state.rider4C4].unlock10FAC;
    const auto routed=game::dispatchOriginalUiRider50KG(in.controller,static_cast<game::OriginalUiRouteInput50KG>(in.originalInput),context,receipt.decision);
    if(routed==game::OriginalUiRouteStatus50KG::noChange)return finish(Status::noChange);
    if(routed!=game::OriginalUiRouteStatus50KG::unavailable)return finish(Status::unsupportedState,0x1EA460);
    if(in.originalInput==4){
        const auto& r=receipt.decision;
        if(!s->idleInput()||!confirmShape(r)||r.admitted||r.action!=game::OriginalUiRouteAction50KG::unavailable||
           r.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)||
           !s->confirmReady(d.dynamic))return finish(Status::unavailableDestination,0x1EA694);
        if(!s->cold.profile->riderConfirmSnapshot(s->pendingPacked1125D,s->pendingComparison100C0))
            return finish(Status::profile,0x1EA58C);
        s->pendingProfile=profile;s->pendingCommon=*s->cold.commonProfile;s->pendingStamp=s->cold.profile->stamp();
        s->pendingBack=r;s->pendingLifecycle=s->transition();s->state.confirmProposalPending=true;
        receipt.routeProposal={this,s->proposalSerial,r,s->pendingStamp};return finish(Status::confirmProposed,0x1EA604);
    }
    const auto& r=receipt.decision;
    if(!s->idleInput()||!s->backAudioReady()||!r.profileProposalComplete||r.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)||
       r.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||r.request.targetScreen!=4||r.request.targetPage!=1||
       r.profileChanges.count!=1||r.selectedRider!=static_cast<std::int32_t>(s->state.rider4C4))return finish(Status::unavailableDestination,0x1EA810);
    s->pendingProfile=profile;s->pendingCommon=*s->cold.commonProfile;s->pendingStamp=s->cold.profile->stamp();
    s->pendingBack=r;s->pendingLifecycle=s->transition();s->state.backProposalPending=true;
    receipt.routeProposal={this,s->proposalSerial,r,s->pendingStamp};return finish(Status::backProposed,0x1EA810);
}
Status OriginalUiRiderSession50KG::commitMove(const OriginalUiRiderSessionInputReceipt50KG& receipt,
    VehicleWindowGl50DS& renderer,VehicleWindowDiagnostic50DS& rd,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->operational()||s->inOperation||s->updatePrepared||!s->idleInput()||s->state.deferred4EC||
       receipt.status!=Status::moveProposed||receipt.moveSerial!=s->proposalSerial||receipt.moveEvent>1)
        return result(d,Status::invalidInput);
    game::OriginalUiRiderSelectionProfile50KG profile;game::OriginalUiRiderSelectionPlan50KG plan;
    if(!s->liveProfile(profile)||game::planOriginalUiRiderMove50KG(*s->selection(),*s->nav(),0,0,
       receipt.moveEvent,false,false,profile,plan)!=game::OriginalUiRiderSelectionStatus50KG::ok||
       plan.visitedControls!=1||!preparedRider(plan.rider4C4)||plan.rider4C4!=receipt.selection.rider4C4||
       plan.slot4C8!=receipt.selection.slot4C8||!plan.captureAndRestorePodTransform||!plan.resetSelectedControl||plan.deferred4EC!=3)
        return result(d,Status::unavailableDestination,0x1EB1F0);
    const auto outgoing=static_cast<std::uint16_t>(s->state.rider4C4==19?27:28);
    const auto incoming=static_cast<std::uint16_t>(plan.rider4C4==19?27:28);
    auto* old=s->dynamic.podScene(outgoing);const auto asset=s->dynamic.podAssets(incoming);
    game::OriginalUiRiderPodOrientation50KG orientation;game::BodyMatrix48G pose;
    if(!old||!s->backend.attached(outgoing)||!asset||!asset->config()||!s->cold.resources->background(plan.rider4C4)||
       !s->cold.statAssets->portrait(plan.rider4C4)||!old->captureDirectionalOrientation(orientation,d.dynamic.model)||
       !game::originalUiRiderPodOrientationPose50KG(asset->config()->position,orientation,pose))
        return result(d,Status::model,0x28A530);
    if(s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&!s->cold.audio->canPlayRiderAdvance2())
        return result(d,Status::unavailableDestination,0x292360);
    const auto generation=s->collector.sourceSubmissionWatermark(),frontier=s->backend.materialLifetime().sequence();
    if(!generation||s->collector.sourceRenderer()!=&renderer||!renderer.retireOriginalUiSourceOwner(s->collector,rd)||
       !rd.submittedWorkCompleted)return result(d,Status::backend);
    s->cpuDrain();
    if(!s->backend.acceptSourceRetirement(&s->collector,generation,frontier))return result(d,Status::backend);
    if(!s->widgets().preflightNavigation(0,plan.nextNavigation,plan.navigationCommands,s->dynamic.hooks(),d.widget))
        return result(d,Status::unavailableDestination,plan.sourcePC);
    auto selected=plan.nextNavigation;game::OriginalUiCommands50KG repeat;
    const auto control=selected.page->groups[1].controls[plan.slot4C8-1];
    // 2A5A00 dispatches widget+78: repeated SELECT, never generic reset/show.
    if(!appendControl(selected,repeat,control,Operation::select,false,receipt.moveEvent?0x1EB304:0x1EB484)||
       !s->widgets().preflightNavigation(0,selected,repeat,s->dynamic.hooks(),d.widget))
        return result(d,Status::unavailableDestination,0x2A5A00);
    s->inOperation=true;
    bool okay=s->navigate(plan.nextNavigation,plan.navigationCommands,plan.sourcePC);
    if(okay){s->state.rider4C4=plan.rider4C4;s->state.slot4C8=plan.slot4C8;okay=s->appearance();}
    // Native frontend has retained the old source and suppresses publication
    // across these three updates, representing video+84=1 at this boundary.
    if(okay)okay=s->navigate(selected,repeat,0x2A5A00);
    auto* next=s->dynamic.podScene(incoming);
    if(okay&&(!next||!s->backend.attached(incoming)||!next->restoreDirectionalOrientation(orientation,d.dynamic.model))){
        s->fault(Status::model,0x289EC0);okay=false;}
    if(okay){
        s->state.deferred4EC=3;
        if(s->cold.audioMode==OriginalUiMainAudioMode50KG::muted)++s->state.audio.suppressedByMutedMode;
        else {OriginalUiMainAudio50KG::RiderAdvanceCommandReceipt played;
            if(!s->cold.audio->playRiderAdvance2(played)){s->fault(Status::audio,0x292360);okay=false;}
            else ++s->state.audio.submitted;}
    }
    s->inOperation=false;
    if(!okay){if(!s->state.faulted)s->fault(Status::widget,plan.sourcePC);d=s->callbackFault;return d.status;}
    return result(d,Status::ok,plan.sourcePC);
}
bool OriginalUiRiderSession50KG::routeProposalCurrent(const OriginalUiRiderRouteProposal50KG& p) const noexcept {
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||s->updatePrepared||(!s->state.backProposalPending&&!s->state.confirmProposalPending)||
       p.source!=this||!p.serial||p.serial!=s->proposalSerial||!s->idleInput()||!s->lightCurrent()||!s->processCurrent()||
       s->cold.application->uiObject3D8||!sameDecision(p.decision,s->pendingBack)||!sameStamp(p.profileStamp,s->pendingStamp)||
       !sameStamp(s->cold.profile->stamp(),s->pendingStamp)||!sameCommon(*s->cold.commonProfile,s->pendingCommon)||
       !sameLife(s->transition(),s->pendingLifecycle)||!preparedRider(s->state.rider4C4)||s->nav()->controllerGroup[0]!=1||
       s->nav()->groups[1].selectedSlot!=s->state.slot4C8)return false;
    if(s->state.confirmProposalPending){
        game::OriginalUiRiderDynamicDiagnostic50KG preflight;std::uint8_t packed=0;std::uint32_t comparison=0;
        if(s->state.backProposalPending||!confirmShape(p.decision)||!s->confirmReady(preflight)||
           !s->cold.profile->riderConfirmSnapshot(packed,comparison)||packed!=s->pendingPacked1125D||
           comparison!=s->pendingComparison100C0)return false;
    }else if(!s->backAudioReady())return false;
    game::OriginalUiRiderSelectionProfile50KG current;return s->liveProfile(current)&&sameProfile(current,s->pendingProfile);
}
Status OriginalUiRiderSession50KG::notifyRouteAccepted(const OriginalUiRiderRouteProposal50KG& p,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.confirmProposalPending){
        if(!s->operational()||s->inOperation||s->updatePrepared||s->backAccepted||s->confirmAccepted||s->state.backProposalPending||
           p.source!=this||!p.serial||p.serial!=s->proposalSerial||!sameDecision(p.decision,s->pendingBack)||
           !confirmShape(p.decision)||!sameStamp(p.profileStamp,s->pendingStamp)||!s->processCurrent()||!s->lightCurrent()||
           !s->confirmReady(d.dynamic)||s->cold.application->uiObject3D8||!sameCommon(*s->cold.commonProfile,s->pendingCommon))
            return result(d,Status::invalidInput,0x1EA604);
        auto expectedLife=s->pendingLifecycle;expectedLife.requested={6,1};expectedLife.lifecycle=Life::requestScreenExit;
        const auto stamp=s->cold.profile->stamp();std::uint8_t packed=0;std::uint32_t comparison=0;
        if(!sameLife(s->transition(),expectedLife)||s->pendingStamp.generation==std::numeric_limits<std::uint64_t>::max()||
           stamp.generation!=s->pendingStamp.generation+1||stamp.mode34!=0||
           !s->cold.profile->riderConfirmSnapshot(packed,comparison)||
           packed!=static_cast<std::uint8_t>((s->pendingPacked1125D&7u)|(s->state.rider4C4<<3))||comparison!=s->state.rider4C4)
            return result(d,Status::profile,0x1EA58C);
        auto expected=s->pendingProfile;expected.remembered6F0=s->state.rider4C4;game::OriginalUiRiderSelectionProfile50KG actual;
        if(!s->liveProfile(actual)||!sameProfile(actual,expected))return result(d,Status::profile,0x1EA538);
        std::uint16_t character=0;if(!s->selectedConfirmCharacter(character))return result(d,Status::model,0x1ECF70);
        s->cpuDrain();s->state.fadeMusic4E8=true; //1EA620, after actual route/profile commit.
        if(!s->applyConfirmVoiceChronology()){d=s->callbackFault;return d.status;}
        if(!s->dynamic.confirmCharacter(character)){d.dynamic=s->dynamic.diagnostic();s->fault(Status::model,0x1D2690);return result(d,Status::model,0x1D2690);}
        //1EA7B0 follows the actual voice command and character _sel action.
        if(s->cold.audioMode==OriginalUiMainAudioMode50KG::muted)++s->state.audio.suppressedByMutedMode;
        else if(!s->cold.audio->playRiderConfirmSelection1()){
            ++s->state.audio.failed;s->fault(Status::audio,0x1EA7B0);d=s->callbackFault;return d.status;
        }else ++s->state.audio.submitted;
        s->state.confirmProposalPending=false;s->confirmAccepted=true;
        // Do NOT call acceptedBack/confirmWhileClosing or set bypass4E4. The
        // panel waits for actual _sel >80% readiness before closing aperture.
        return result(d,Status::ok);
    }
    if(!s->operational()||s->inOperation||s->updatePrepared||s->backAccepted||!s->state.backProposalPending||p.source!=this||
       !p.serial||p.serial!=s->proposalSerial||!sameDecision(p.decision,s->pendingBack)||!sameStamp(p.profileStamp,s->pendingStamp)||
       !s->processCurrent()||!s->lightCurrent()||!s->backAudioReady()||s->cold.application->uiObject3D8||!sameCommon(*s->cold.commonProfile,s->pendingCommon)||
       !preparedRider(s->state.rider4C4)||s->nav()->controllerGroup[0]!=1||s->nav()->groups[1].selectedSlot!=s->state.slot4C8)
        return result(d,Status::invalidInput,0x1EA810);
    const auto t=s->transition();auto expectedLife=s->pendingLifecycle;
    expectedLife.requested={4,1};expectedLife.lifecycle=Life::requestScreenExit;
    const auto stamp=s->cold.profile->stamp();
    if(!sameLife(t,expectedLife)||s->pendingStamp.generation==std::numeric_limits<std::uint64_t>::max()||
       stamp.generation!=s->pendingStamp.generation+1||stamp.mode34!=s->pendingStamp.mode34)
        return result(d,Status::unsupportedState,0x1EA810);
    auto expected=s->pendingProfile;const auto& change=p.decision.profileChanges.values[0];
    if(change.operation!=game::OriginalUiProfileWrite50KG::word||change.value!=s->state.rider4C4||change.replaceMask!=0xFFFFFFFFu)
        return result(d,Status::invalidInput,0x1EA810);
    switch(change.field){
    case game::OriginalUiProfileField50KG::riderMode0_6F0:expected.remembered6F0=s->state.rider4C4;break;
    case game::OriginalUiProfileField50KG::rider1Mode1_6F4:expected.remembered6F4=s->state.rider4C4;break;
    case game::OriginalUiProfileField50KG::riderOtherMode6FC:expected.remembered6FC=s->state.rider4C4;break;
    default:return result(d,Status::invalidInput,0x1EA810);
    }
    game::OriginalUiRiderSelectionProfile50KG current;
    if(!s->liveProfile(current)||!sameProfile(current,expected))return result(d,Status::profile,0x1EA810);
    s->cpuDrain();s->state.backProposalPending=false;s->state.confirmProposalPending=false;s->backAccepted=true;
    // Real page1 Back1EA82C/1EA830 sets fade4E8 and bypass4E4. These cannot
    // certify Main readiness and are not published for an unaccepted proposal.
    s->state.fadeMusic4E8=true;s->panel.acceptedBack();
    if(!s->backAudio()){d=s->callbackFault;return d.status;}
    // Retail233410 queues a backend play command before267BE0 submits it.
    // Our concrete native mixer applies that play directly, without a guest
    // command buffer. The retained transportE00==-1 represents no projected
    // pending work; do not add a device flush or another PCM advance here.
    return result(d,Status::ok);
}
Status OriginalUiRiderSession50KG::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG& in,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->rendering||!s->currentPage())return result(d,Status::invalidInput);
    if(s->updatePrepared){s->fault(Status::unsupportedState,0x2A3C80);d=s->callbackFault;return d.status;}
    if(!std::isfinite(in.elapsedSeconds)||in.elapsedSeconds<0||in.elapsedSeconds>1||(in.paused&&in.elapsedSeconds!=0))
        return result(d,Status::invalidInput);
    if(!s->processCurrent()||!s->lightCurrent()||s->cold.application->uiObject3D8)
        return result(d,Status::unsupportedState,0x267BE0);
    s->cpuDrain();if(!s->invalidateProposal()){d=s->callbackFault;return d.status;}
    // Shared native transport service precedes the sole manager tick, exactly
    // once, as for Main/Options. Character traversal and its genuine shared
    // motion RNG stay in update below. No voice/RNG draw is added for silence.
    s->inOperation=true;s->paused=in.paused;
    if(s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled){
        s->cold.audio->pause(in.paused);
        if(!s->cold.audio->update(in.elapsedSeconds))s->fault(Status::audio,0x234440);
    }
    s->inOperation=false;if(s->state.faulted){d=s->callbackFault;return d.status;}
    s->preparedUpdate=in;s->updatePrepared=true;return result(d,Status::ok);
}
Status OriginalUiRiderSession50KG::update(const OriginalUiMainSessionUpdate50KG& in,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->rendering||!s->currentPage())return result(d,Status::invalidInput);
    if(!s->updatePrepared||in.elapsedSeconds!=s->preparedUpdate.elapsedSeconds||in.paused!=s->preparedUpdate.paused||
       in.debugChordHeld!=s->preparedUpdate.debugChordHeld){s->fault(Status::unsupportedState,0x2A3C80);d=s->callbackFault;return d.status;}
    s->updatePrepared=false;s->cpuDrain();if(!s->invalidateProposal()){d=s->callbackFault;return d.status;}
    if(!s->processCurrent()||!s->lightCurrent()||s->cold.application->uiObject3D8){
        s->fault(Status::unsupportedState,0x1EB510);d=s->callbackFault;return d.status;}
    s->inOperation=true;const auto* ordinary=s->widgets().order(false);const auto* top=s->widgets().order(true);
    if(!ordinary||!top)s->fault(Status::widget,0x141ED0);
    if(!s->state.faulted)s->updateWidgets(*ordinary,in.elapsedSeconds);
    if(!s->state.faulted&&s->state.pageShown){
        // 1EB510..1EB588:3->2->1, then renderer drain/effect request. The
        // frontend owns that final native presentation boundary, before the
        // next ordinary render, using the preceding completed source image.
        if(s->state.deferred4EC)--s->state.deferred4EC;
        const auto t=s->transition();
        if(!s->panel.update38(in.elapsedSeconds,t.lifecycle,t.phase,*s->nav(),s->dynamic))s->fault(Status::aperture,0x1ECFE0);
        if(!s->state.faulted&&s->aperture.saveRiderUpdateDelta38(in.elapsedSeconds)!=ApertureStatus::ok)
            s->fault(Status::aperture,0x1EB620);
    }
    if(!s->state.faulted)s->updateWidgets(*top,in.elapsedSeconds);
    s->inOperation=false;
    if(s->state.faulted){s->cpuDrain();d=s->callbackFault;return d.status;}
    if(s->state.updates==std::numeric_limits<std::uint64_t>::max()){s->fault(Status::unsupportedState,0x1EB4F0);d=s->callbackFault;return d.status;}
    ++s->state.updates;return result(d,Status::ok);
}
Status OriginalUiRiderSession50KG::render(const OriginalUiRiderDrawClock50KG& clock,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->rendering||s->updatePrepared||!s->currentPage()||!clock.known||(s->paused&&clock.advanceAnimations))
        return result(d,Status::invalidInput);
    if(!s->processCurrent()||!s->lightCurrent()||!s->state.cameraActive||!s->state.pageShown||
       s->cold.application->uiObject3D8||!s->aperture.snapshot().hasUpdateDelta)return result(d,Status::unsupportedState,0x1ECE20);
    game::OriginalUiRiderStatInput50KG stats;
    if(!s->cold.profile->statSnapshot(s->state.rider4C4,s->panel.offset4DC(),stats))return result(d,Status::profile,0x1EBBF0);
    s->cpuDrain();if(!s->invalidateProposal()){d=s->callbackFault;return d.status;}
    s->inOperation=true;s->rendering=true;
    // Original order is ordinary widgets/models, custom page appearance/stat
    // draw/backgrounds, actual aperture(+440 BEFORE rectangles), prompts, top
    // widgets, and group drain. The collector alone can seal the full frame.
    bool okay=s->collector.begin(clock,s->renderContext,s->lighting,d.draw)&&
        s->collector.projectOrdinaryWidgets(true,d.draw);
    if(okay)okay=s->pageColors();
    if(okay)okay=s->collector.projectPage(stats,true,d.draw);
    if(okay){const auto status=s->aperture.render48(s->transition().lifecycle);
        if(!apertureOkay(status)){d.aperture.status=status;s->fault(Status::aperture,0x1ECE80);okay=false;}
        if(s->state.faulted)okay=false;
    }
    if(okay)okay=s->collector.projectPrompts(s->state.promptMask42C,d.draw)&&
        s->collector.projectOnTopWidgets(true,d.draw)&&s->collector.finish(d.draw);
    s->rendering=false;s->inOperation=false;
    if(!okay){if(!s->state.faulted){s->callbackFault.draw=d.draw;s->fault(Status::draw,d.draw.sourcePC);}
        s->cpuDrain();d=s->callbackFault;return d.status;}
    if(s->state.renders==std::numeric_limits<std::uint64_t>::max()){
        s->fault(Status::unsupportedState,0x1ECE20);s->cpuDrain();d=s->callbackFault;return d.status;}
    ++s->state.renders;s->state.framePublished=true;return result(d,Status::ok);
}
Status OriginalUiRiderSession50KG::prepareBank(Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->rendering)return result(d,Status::invalidInput);
    s->cpuDrain();
    if(!s->collector.prepareBank(d.draw))return result(d,Status::draw,d.draw.sourcePC);
    return result(d,Status::ok);
}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiRiderSession50KG::bank() const noexcept {
    return storage_&&storage_->operational()?storage_->collector.bank():nullptr;
}
const std::vector<MaterialTextureSampler50EB>* OriginalUiRiderSession50KG::samplers() const noexcept {
    return storage_&&storage_->operational()?storage_->collector.samplers():nullptr;
}
Status OriginalUiRiderSession50KG::bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB> gpu,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->rendering)return result(d,Status::invalidInput);
    s->cpuDrain();if(!s->collector.bindPublishedBank(std::move(gpu),d.draw))return result(d,Status::draw,d.draw.sourcePC);
    return result(d,Status::ok);
}
const OriginalUiSourceFrame50KM* OriginalUiRiderSession50KG::sourceFrame() const noexcept {
    const auto* s=storage_.get();return s&&s->operational()&&s->state.framePublished?s->collector.frame():nullptr;
}
const OriginalUiRiderDrawContribution50KG* OriginalUiRiderSession50KG::rendererOwner() const noexcept {
    return storage_?&storage_->collector:nullptr;
}
OriginalUiRiderSessionSnapshot50KG OriginalUiRiderSession50KG::snapshot() const noexcept {
    const auto* s=storage_.get();if(!s)return {};
    auto out=s->state;out.blocked4E0=s->panel.inputBlocked4E0();out.bypass4E4=s->panel.bypass4E4();out.panelY4DC=s->panel.offset4DC();
    out.transition=s->transition();out.aperture=s->aperture.snapshot();out.bankGeneration=s->backend.generation();
    out.registryGeneration=s->backend.registryGeneration();out.framePublished=sourceFrame()!=nullptr;return out;
}
void OriginalUiRiderSession50KG::drain() noexcept {if(storage_)storage_->cpuDrain();}
void OriginalUiRiderSession50KG::requestQuit() noexcept {
    if(!storage_)return;storage_->cpuDrain();storage_->state.backProposalPending=false;storage_->state.confirmProposalPending=false;storage_->state.quitRequested=true;
}
bool OriginalUiRiderSession50KG::sourceTeardownPending()const noexcept {
    return storage_&&!storage_->sourceTornDown&&storage_->backendBound&&
        (storage_->collector.hasSourceSubmission()||storage_->backend.materialLifetime().hasOwners()||
         storage_->backend.materialLifetime().pendingEvents());
}
Status OriginalUiRiderSession50KG::teardownSource(VehicleWindowGl50DS* renderer,
    VehicleWindowDiagnostic50DS& rendererDiagnostic,Diagnostic& d) noexcept {
    d={};if(!storage_||storage_->sourceTornDown)return result(d,Status::ok);
    auto& s=*storage_;if(s.inOperation||s.rendering)return result(d,Status::invalidInput);
    s.sourceClosing=true;
    // Retire only the utterance issued by this Rider page. A private unentered
    // candidate holds no playback receipt and cannot stop the active page.
    if(s.confirmPlayback.issued()&&(!s.cold.audio||!s.cold.audio->releaseRiderConfirmVoice(s.confirmPlayback)))
        return result(d,Status::audio,0x2924A0);
    const auto generation=s.collector.sourceSubmissionWatermark();
    const auto frontier=s.backend.materialLifetime().sequence();
    // This proof is recorded by beginSourceSubmission on the exact collector.
    // A never-submitted private candidate may clean up without GL work.
    if(s.collector.hasSourceSubmission()){
        if(!renderer||s.collector.sourceRenderer()!=renderer||!renderer->retireOriginalUiSourceOwner(s.collector,rendererDiagnostic)||
           !rendererDiagnostic.submittedWorkCompleted)return result(d,Status::backend);
    }
    s.collector.drain();s.state.framePublished=false;
    if(!s.backend.acceptSourceRetirement(&s.collector,generation,frontier))return result(d,Status::backend);
    if(!s.dynamic.drain()){d.dynamic=s.dynamic.diagnostic();d.backend=s.backend.diagnostic();return result(d,Status::model);}
    s.state.selectionAttached=false; // dynamic cleanup completed even if a later consumer must retry
    if(!s.collector.consumeMaterials(false,d.draw))return result(d,Status::draw,d.draw.sourcePC);
    if(!s.backend.finalizeTeardown(s.collector.materialSequence(),d.backend))return result(d,Status::backend);
    s.sourceTornDown=true;s.dependenciesPrepared=false;s.state.selectionAttached=false;return result(d,Status::ok);
}

} // namespace rr::platform

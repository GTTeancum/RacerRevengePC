#include "original_ui_rider_session50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_draw50kg.h"
#include "original_ui_course_draw_resources50kg.h"
#include "original_ui_course_hologram_draw50kg.h"
#include "vehicle_window_gl50ds.h"
#include "original_ui_rider_addon_overlay50dl.h"
#include "../../../src/assets/dlc_registry50dl.h"
#include "../../../src/game/dlc_rider_entries50dl.h"
#include "../../../src/game/dlc_career50dl.h"
#include "../../../src/game/original_ui_rider_addon_models50dl.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <bit>
#include <cfenv>
#include <algorithm>
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
constexpr std::uint32_t hiddenTraversalMask=(1u<<26)|(1u<<21)|(1u<<25)|(1u<<14)|(1u<<1);
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
// Native transaction evidence for the source Main remembered-slot write only.
bool mainRememberedSlotProjection(const OriginalUiMainProfile50KG& before,
    std::uint16_t previousSlot,std::uint16_t selectedSlot,std::uint32_t prospectiveMode,
    OriginalUiMainProfile50KG& output) noexcept {
    if(!before.known||previousSlot<1||previousSlot>5||before.savedSlot6E0!=previousSlot||
       selectedSlot<1||selectedSlot>3||prospectiveMode!=std::uint32_t(selectedSlot-1)||
       before.mode34!=prospectiveMode)return false;
    output=before;output.savedSlot6E0=selectedSlot;return true;
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
bool confirmShape(const game::OriginalUiRouteDecision50KG& d,std::uint32_t selectedRider,std::uint32_t mode) noexcept {
    using F=game::OriginalUiProfileField50KG;using W=game::OriginalUiProfileWrite50KG;
    if((mode!=0&&mode!=2)||d.inputPC!=0x1EA460||d.selectedRider<0||d.selectedRider>=27||
       static_cast<std::uint32_t>(d.selectedRider)!=selectedRider||!d.profileProposalComplete||d.profileChanges.count!=3||
       d.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||d.request.sourceScreen!=5||d.request.sourcePage!=1||
       d.request.targetScreen!=(mode==2?9:6)||d.request.targetPage!=1||d.request.sourcePC!=(mode==2?0x1EA5E4u:0x1EA604u))return false;
    const auto& a=d.profileChanges.values[0];const auto& b=d.profileChanges.values[1];const auto& c=d.profileChanges.values[2];
    return a.field==(mode==2?F::riderOtherMode6FC:F::riderMode0_6F0)&&a.operation==W::word&&a.value==static_cast<std::uint32_t>(d.selectedRider)&&a.replaceMask==0xFFFFFFFFu&&a.sourcePC==(mode==2?0x1EA54Cu:0x1EA538u)&&
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
    using NativeProjection=game::OriginalUiRiderWidgets50KG::NativeProjection;
    NativeProjection nativeBefore{},nativePost{};
    std::array<NativeProjection,4> nativeEntryPhases{};
    OriginalUiRiderResidencyDemand50KG nativeDemand;
    OriginalUiRiderProfileStamp50KG nativeStamp{};
    OriginalUiMainProfile50KG nativeCommon{};
    game::OriginalUiRiderSelectionProfile50KG nativeEntryProfile{};
    std::shared_ptr<OriginalUiRiderResidentPool50KG> nativePool;
    std::uint64_t nativeWidgetGeneration=0,nativePreparedWidgetGeneration=0;
    std::uint64_t nativePreparedMaterialGeneration=0;
    bool nativePrepared=false,nativeEntryActive=false,nativeEntryCommitted=false;
    std::uint8_t nativeEntryPhase=0;

    // PORT EXTENSION, NOT ORIGINAL (DLC menu-art step A): the add-on pod model
    // table (empty without add-on pods; loaded only for a free-play carousel).
    game::OriginalUiRiderAddonModels50DL addonModels;
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
    bool sharedEntryAttempted=false,sharedEntryComplete=false,resetInputActive=false;
    bool resetPending=false;
    std::uint32_t resetRider=0;
    OriginalUiRiderProfileStamp50KG resetStamp{};
    bool drawablePoliciesPrepared=false;
    std::uint32_t destinationPreviewRiders=0;
    // ---- PORT EXTENSION, NOT ORIGINAL (DLC D4/D5, DESIGN.md 4.4): add-on riders
    // after the last stock rider of the free-play carousel, then the first stock
    // rider again. An add-on slot is port state only: the original navigation,
    // widgets, models, rider4C4/slot4C8 and profile stay on the stock control the
    // add-on block follows (the anchor), and render draws the add-on over it
    // (original_ui_rider_addon_overlay50dl.h). No add-on number ever reaches an
    // original field, table or the stock save. With no admitted add-on pod (or in
    // Tournament) addonCarousel is false and none of this runs.
    bool addonCarousel=false;           // fixed at bind: free play (mode0 single, mode1 VS) with admitted add-on pods
    // Human player this Rider page selects for: page 5/1 = P1 (0). The port builds
    // only page 5/1 (previousPage94 refuses a page change); a page 5/2 owner for P2
    // would bind with 1, and every add-on call below already takes this index.
    std::uint8_t addonPlayer=0;
    std::size_t addonSlot=0;            // 0 = stock rider; k = addonRiderAt(k-1), 1..addonRiderCount()
    bool addonAnchorLast=true;          // entered by Next past the last (true) or Previous past the first (false) stock rider
    std::uint32_t addonConfirmRider=0;  // stock rider an add-on Confirm records (the last stock Confirm's rider)
    bool addonSkipLogged=false,addonRestoreTried=false,addonModelLogged=false,addonAdvanceLogged=false;
    bool addonCharacterLogged=false,addonSelectLogged=false,addonPageLogged=false,addonBinderLogged=false;
    std::string addonCharacterNote;
    // The shown add-on character (step B), or nullptr.
    game::OriginalUiRiderCharacterScene50KG* addonCharacter() noexcept {
        return addonSlot&&addonModelRow(addonSlot)?addonModels.liveCharacter(addonSlot-1):nullptr;
    }
    const game::OriginalUiRiderCharacterScene50KG* addonCharacter() const noexcept {
        return const_cast<Storage*>(this)->addonCharacter();
    }
    // The anchor stock rider's authored character widget (placement stand-in).
    std::uint16_t anchorCharacterLayout() const noexcept {
        std::array<std::uint16_t,4> ids{};
        return controlWidgets(state.slot4C8,state.rider4C4,ids)?ids[3]:0;
    }
    std::uint16_t anchorCharacterWidget() const noexcept {
        std::array<std::uint16_t,4> ids{};
        return addonSlot&&controlWidgets(state.slot4C8,state.rider4C4,ids)?ids[3]:0;
    }
    std::array<std::string,4> addonText{}; // name, short name, package name, Huttese name of the highlighted add-on
    bool addonColorsLogged=false;
    // PORT EXTENSION (DLC): the colours the highlighted rider shows. Stock: that
    // rider's profile colours (unchanged path). Add-on: its package ui.colors, else
    // the layout_slot rider's, else (PORT STAND-IN, logged) the anchor's.
    bool riderColors(OriginalUiRiderProfileColors50KG& out) noexcept {
        const auto* pod=addonPod();
        if(!addonSlot||!pod)return cold.profile->colors(state.rider4C4,out);
        const char* source="package";
        if(pod->ui.colorsAuthored)out={pod->ui.accentColor,pod->ui.secondaryColor};
        else if(pod->ui.layoutSlot&&cold.profile->colors(pod->ui.layoutSlot,out))source="layout_slot";
        else if(cold.profile->colors(state.rider4C4,out))source="anchor_stand_in";
        else return false;
        if(!addonColorsLogged){addonColorsLogged=true;
            std::fprintf(stderr,"dlc_rider_colors key=%s number=%u source=%s rider=%u accent=%06X secondary=%06X\n",pod->key.c_str(),pod->number,source,
                pod->ui.colorsAuthored?0u:(pod->ui.layoutSlot?pod->ui.layoutSlot:state.rider4C4),unsigned(out.accent17FA00),unsigned(out.secondary17FA10));}
        return true;
    }
    riderAddon50DL::Overlay addonOverlay{};
    const assets::dlc50DL::AddonPod* addonPod() const noexcept {
        return addonSlot?game::dlc50DL::addonRiderAt(addonSlot-1):nullptr; // registry bounds-checks the index
    }
    // The rider every original Confirm field receives: the highlighted stock rider,
    // or for an add-on the stock rider of the last stock Confirm (packed1125D).
    std::uint32_t confirmRider() const noexcept {return addonSlot?addonConfirmRider:state.rider4C4;}
    // PORT EXTENSION (DLC career, dlc_career50dl.h): the 1EA870/1800E0 career
    // reset gate. A stock slot is the original's; an add-on slot (P1, Tournament)
    // applies the same rule to the add-on pod's own career.
    bool careerResetAllowed() const noexcept {
        if(!addonSlot)return cold.profile->canResetRiderCareer1800E0(state.rider4C4,cold.profile->stamp());
        const auto* pod=addonPod();
        return pod&&addonPlayer==0&&cold.commonProfile->mode34==2u&&game::dlc50DL::canResetAddonCareer(pod->number);
    }
    void addonLog(const char* action) const noexcept {
        const auto* pod=addonPod();
        std::fprintf(stderr,"dlc_rider_carousel player=%u action=%s slot=%zu key=%s number=%u anchor_rider=%u anchor_slot=%u\n",
            unsigned(addonPlayer)+1u,action,addonSlot,pod?pod->key.c_str():"-",pod?pod->number:0u,state.rider4C4,unsigned(state.slot4C8));
    }
    // The add-on model table row of carousel slot k (1-based), only when it is
    // the same registry pod and ready; nullptr otherwise (bounds-checked).
    game::OriginalUiRiderAddonModels50DL::Row* addonModelRow(std::size_t slot) noexcept {
        const auto* pod=slot?game::dlc50DL::addonRiderAt(slot-1):nullptr;
        auto* row=pod?addonModels.row(slot-1):nullptr;
        return row&&row->ready&&row->scene&&row->number==pod->number?row:nullptr;
    }
    game::OriginalUiRiderPodScene50KG* anchorPodScene() noexcept {
        std::array<std::uint16_t,4> ids{};
        return controlWidgets(state.slot4C8,state.rider4C4,ids)?dynamic.podScene(ids[0]):nullptr;
    }
    bool setAddonSlot(std::size_t slot) noexcept {
        if(!addonCarousel||slot>game::dlc50DL::addonRiderCount())return false;
        const auto previous=addonSlot;
        if(slot&&!game::dlc50DL::addonRiderAt(slot-1))return false;
        // Model handoff, as an original move: the incoming pod takes the outgoing
        // pod's directional orientation; the add-on User_Rotate runs while shown.
        {
            auto* fromRow=addonModelRow(previous);auto* toRow=addonModelRow(slot);
            auto* from=previous?(fromRow?fromRow->scene.get():nullptr):anchorPodScene();
            auto* to=slot?(toRow?toRow->scene.get():nullptr):anchorPodScene();
            if(from&&to&&from!=to&&from->loaded()&&to->loaded()){
                game::OriginalUiRiderPodOrientation50KG orientation;game::OriginalUiRiderModelDiagnostic50KG md;
                if(from->captureDirectionalOrientation(orientation,md))(void)to->restoreDirectionalOrientation(orientation,md);
            }
            if(fromRow)fromRow->scene->deselected();
            if(toRow)toRow->scene->selected();
            // Step B: the outgoing add-on character unloads, the incoming one loads
            // and starts its entry motion, as a deselected/selected stock character.
            if(previous&&previous!=slot)addonModels.stopCharacter(previous-1);
            if(slot&&slot!=previous&&toRow){
                game::OriginalUiRiderModelDiagnostic50KG cd;
                if(!toRow->characterReady)addonCharacterNote=toRow->characterRefused.empty()?"character not prepared":toRow->characterRefused;
                else if(!addonModels.startCharacter(slot-1,anchorCharacterLayout(),cold.application->modelMotion,cold.application->modelMode0Ik,
                    cold.application->modelLibm,cd))addonCharacterNote="character graph does not attach (error "+std::to_string(unsigned(cd.error))+")";
                else addonCharacterNote.clear();
            }
        }
        addonSlot=slot;
        const auto* pod=addonPod();
        if(slot&&!pod){addonSlot=previous;return false;}
        addonModelLogged=false;addonCharacterLogged=false;addonSelectLogged=false;addonPageLogged=false;addonBinderLogged=false;
        try{addonText={pod?pod->name:std::string(),pod?pod->shortName:std::string(),pod?pod->packageName:std::string(),
            pod?pod->ui.hutteseName:std::string()};}
        catch(...){addonText={};}
        addonSkipLogged=false;addonColorsLogged=false;
        game::dlc50DL::setHighlightedAddonRider(addonPlayer,pod?pod->number:0u);
        return true;
    }
    // Same original rider-advance sound (and receipts) a stock move plays.
    bool addonAdvanceSound() noexcept {
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return true;}
        OriginalUiMainAudio50KG::RiderAdvanceCommandReceipt played;
        if(!cold.audio->playRiderAdvance2(played)){++state.audio.failed;fault(Status::audio,0x292360);return false;}
        ++state.audio.submitted;return true;
    }
    // Add-on pods are unlocked at once (D5): the original 1EAFB0 projection of an
    // UNLOCKED row for the anchor, whatever the anchor's own lock. Local profile
    // copy only; leaving the add-on block replays the anchor's real appearance().
    bool addonAppearance() noexcept {
        game::OriginalUiRiderSelectionProfile50KG p;game::OriginalUiRiderAppearance50KG a;
        if(!liveProfile(p)||state.rider4C4>=p.riders.size()){fault(Status::profile,0x1EAFB0);return false;}
        p.riders[state.rider4C4].unlock10FAC=1;
        // DLC career: the 1EAFB0 reset prompt reads the add-on career's own medal 0.
        if(cold.prospectiveMode==2){
            game::dlc50DL::AddonCareerView50DL career;const auto* pod=addonPod();
            if(!pod||!game::dlc50DL::addonCareerView(pod->number,career)){fault(Status::profile,0x1EAFB0);return false;}
            p.riders[state.rider4C4].career1938=career.own.medals1938[0];
        }
        if(game::projectOriginalUiRiderAppearance50KG(state.rider4C4,p,a)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           !game::applyOriginalUiRiderLighting50KG(true,savedLighting,lighting)){fault(Status::profile,0x1EAFB0);return false;}
        for(std::size_t i=0;i<a.count;++i)if(!widget(a.widgets[i].widget,Operation::setEnabled,a.widgets[i].enable,a.widgets[i].sourcePC))return false;
        state.promptMask42C=a.promptMask42C;return true;
    }
    enum class AddonMove : std::uint8_t {stock,handled,unavailable,faulted};
    // Called with the unchanged original move plan. Only the original wrap (Next
    // from the last eligible stock slot, Previous from the first) and moves inside
    // the add-on block are taken over; every other move stays the original one.
    AddonMove addonMove(bool next,const game::OriginalUiRiderSelectionPlan50KG& plan) noexcept {
        const auto count=game::dlc50DL::addonRiderCount();
        const auto* n=nav();
        if(!addonCarousel||!count||!n||!n->page||n->page->groups.size()<=1)return AddonMove::stock;
        const auto from=n->groups[1].selectedSlot;
        const bool wraps=next?plan.slot4C8<=from:plan.slot4C8>=from;
        if(!addonSlot&&!wraps)return AddonMove::stock;
        if(!idleInput()||(cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&!cold.audio->canPlayRiderAdvance2()))
            return AddonMove::unavailable;
        if(!addonSlot){
            addonAnchorLast=next;
            if(!setAddonSlot(next?1:count))return AddonMove::unavailable;
            if(!addonAppearance()||!addonAdvanceSound())return AddonMove::faulted;
            addonLog("enter");return AddonMove::handled;
        }
        if(next?addonSlot<count:addonSlot>1){
            if(!setAddonSlot(next?addonSlot+1:addonSlot-1))return AddonMove::unavailable;
            if(!addonAdvanceSound())return AddonMove::faulted;
            addonLog("step");return AddonMove::handled;
        }
        // Leaving the add-on block. Its stock neighbour is the anchor itself, or
        // the stock rider across the original wrap (then the original move runs).
        const bool acrossWrap=next==addonAnchorLast;
        addonLog(acrossWrap?"leave_wrap":"leave");
        if(!setAddonSlot(0)||!appearance())return AddonMove::faulted;
        if(acrossWrap)return AddonMove::stock;
        if(!addonAdvanceSound())return AddonMove::faulted;
        return AddonMove::handled;
    }
    // Once per page, at its first idle update: re-highlight the add-on Back
    // remembered, when the page opened on that same anchor and the anchor is
    // still at the wrap the add-on block hangs off. Otherwise nothing changes.
    bool restoreRememberedAddon() noexcept {
        if(!addonCarousel||addonSlot||addonRestoreTried||!entryCompleted||!opened||!idleInput())return true;
        addonRestoreTried=true;
        game::dlc50DL::RememberedAddonRider50DL r;
        if(!game::dlc50DL::rememberedAddonRider(addonPlayer,r)||r.anchorRider!=state.rider4C4)return true;
        std::size_t slot=0;
        for(std::size_t i=0;i<game::dlc50DL::addonRiderCount();++i)
            if(const auto* pod=game::dlc50DL::addonRiderAt(i);pod&&pod->number==r.racer){slot=i+1;break;}
        game::OriginalUiRiderSelectionProfile50KG p;game::OriginalUiRiderSelectionPlan50KG plan;
        const auto* n=nav();
        if(!slot||!n||!n->page||n->page->groups.size()<=1||!liveProfile(p)||
           game::planOriginalUiRiderMove50KG(*selection(),*n,0,0,r.anchorLast?1:0,false,false,p,plan)!=
               game::OriginalUiRiderSelectionStatus50KG::ok)return true;
        const auto from=n->groups[1].selectedSlot;
        if(!(r.anchorLast?plan.slot4C8<=from:plan.slot4C8>=from))return true; // anchor no longer at the wrap
        addonAnchorLast=r.anchorLast;
        if(!setAddonSlot(slot))return true;
        if(!addonAppearance())return false;
        addonLog("restore");return true;
    }
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
    bool preparedTraversal(std::uint32_t rider) const noexcept {
        // Actual authored pair and sealed backend resources, not an identity
        // admission list. Traversal still requires every intermediate callback.
        return dynamic.podWidget(rider)&&dynamic.characterWidget(rider)&&backend.preparedRider(rider);
    }
    bool preparedRider(std::uint32_t rider,OriginalUiRiderContributionDiagnostic50KG* failure=nullptr) const noexcept {
        if(failure)*failure={};
        if(rider>=32||!preparedTraversal(rider))return false;
        if((cold.requestedTraversalRiders&(1u<<rider))&&!(destinationPreviewRiders&(1u<<rider)))return false;
        if(!drawablePoliciesPrepared)return true; // Cold resource checks precede policy preparation.
        OriginalUiRiderContributionDiagnostic50KG diagnostic;
        const bool okay=collector.drawableModel(dynamic.podWidget(rider),diagnostic)&&
            collector.drawableModel(dynamic.characterWidget(rider),diagnostic);
        if(!okay&&failure)*failure=diagnostic;
        return okay;
    }
    bool controlWidgets(std::uint16_t slot,std::uint32_t rider,std::array<std::uint16_t,4>& out) const noexcept {
        const auto* n=nav();
        if(!n||!n->page||!selection()||n->page->groups.size()<=1||!slot||
           slot>n->page->groups[1].controls.size())return false;
        const auto control=n->page->groups[1].controls[slot-1];
        if(control<0||static_cast<std::size_t>(control)>=n->page->controls.size()||
           selection()->controlRider(static_cast<std::uint16_t>(control))!=static_cast<std::int32_t>(rider))return false;
        const auto& ids=n->page->controls[control].widgets;
        if(ids.size()!=4||widgets().state(ids[0]).kind!=Kind::pod||widgets().state(ids[3]).kind!=Kind::character)return false;
        for(std::size_t i=0;i<out.size();++i)out[i]=ids[i];
        return true;
    }
    bool preparedMovement(const game::OriginalUiRiderSelectionPlan50KG& plan) const noexcept {
        const auto* n=nav();
        if(!n||!n->page||!selection()||plan.nextNavigation.page!=n->page||
           !plan.navigationCommands.count||plan.navigationCommands.count>plan.navigationCommands.values.size())return false;
        // Read every authored intermediate callback before the frontend retains
        // or retires the outgoing source. A prepared final rider cannot stand
        // in for a hidden intermediate character/pod that SELECT really loads.
        for(std::size_t i=0;i<plan.navigationCommands.count;++i){
            const auto& command=plan.navigationCommands.values[i];
            if(command.controlId>=n->page->controls.size())return false;
            for(const auto id:n->page->controls[command.controlId].widgets){
                const auto kind=widgets().state(id).kind;
                const bool needs=(command.operation==Operation::select&&(kind==Kind::pod||kind==Kind::character))||
                    (command.operation==Operation::clearSelection&&kind==Kind::character);
                if(!needs)continue;
                const auto rider=selection()->controlRider(command.controlId);
                if(rider<0||!preparedTraversal(static_cast<std::uint32_t>(rider))||
                   id!=(kind==Kind::pod?dynamic.podWidget(rider):dynamic.characterWidget(rider)))return false;
            }
        }
        return true;
    }
    bool selectedConfirmCharacter(std::uint16_t& character) const noexcept {
        const auto* n=nav();if(!n||!n->page||n->page->groups.size()<=1||n->controllerGroup[0]!=1||
           !state.slot4C8||n->groups[1].selectedSlot!=state.slot4C8||
           state.slot4C8>n->page->groups[1].controls.size()||!preparedRider(state.rider4C4))return false;
        std::array<std::uint16_t,4> ids{};
        // 1ECF40 tests size4; 1ECF70 returns the actual fourth control member.
        if(!controlWidgets(state.slot4C8,state.rider4C4,ids)||!backend.attached(ids[3]))return false;
        const auto actual=dynamic.state(ids[3]);
        if(actual.kind!=Kind::character||!actual.selected||!actual.shown)return false;
        character=ids[3];return true;
    }
    bool confirmAnimationPreflight(game::OriginalUiRiderDynamicDiagnostic50KG& d) const noexcept {
        std::uint16_t character=0;
        if(!selectedConfirmCharacter(character))return false;
        // PORT EXTENSION (DLC step B): an add-on rider confirms with its own _sel motion.
        if(addonSlot){const auto* scene=addonCharacter();
            return scene&&scene->preflightConfirm(d.model);}
        return dynamic.preflightConfirmCharacter(character,d);
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
    bool updateConfirmStop1EB5C8(float dt) noexcept {
        auto& timer=state.confirmStopTimer7D24;
        if(!std::isfinite(timer)){fault(Status::unsupportedState,0x1EB5C8);return false;}
        if(timer<=0)return true;
        // Native admission, not an original branch. The preceding ordinary
        // widgets/panel effects remain committed if strict arithmetic fails.
        if(std::fegetround()!=FE_TONEAREST){fault(Status::unsupportedState,0x1EB5E4);return false;}
        volatile float next=timer-dt;
        timer=next; //1EB5E8: retain overshoot, with no clamp or catch-up.
        if(!panel.bypass4E4()||timer>0)return true;
        // Original2924A0 stops page53B8's UIACCEPT group. This bounded native
        // page can issue only its one retained confirm utterance. Release the
        // exact transport/revision receipt; never stop generic selection audio,
        // another page's voice, music or a mixer-wide group. Empty/muted has
        // no owned utterance to stop. This is no new PCM advance or IOP claim.
        if(!audioReady()||(cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
            !cold.audio->releaseRiderConfirmVoice(confirmPlayback))){
            ++state.audio.failed;fault(Status::audio,0x1EB60C);return false;
        }
        // A successful release consumes the receipt. Teardown will not repeat
        // it, and the nonpositive source timer prevents another update stop.
        return true;
    }
    bool confirmReady(game::OriginalUiRiderDynamicDiagnostic50KG& d) const noexcept {
        OriginalUiRiderProfileOwner50KG::RiderSelectionFacts rider;
        const auto mode=cold.prospectiveMode;
        // Match the Tournament destination, including completed standings;
        // earned/unsaved native progress does not require a fresh career.
        // The frontend still retains every destination owner before writes/audio.
        if(mode==2&&addonSlot){
            // PORT EXTENSION (DLC career): the add-on pod's own career, same gate.
            game::dlc50DL::AddonCareerView50DL career;const auto* pod=addonPod();
            if(!pod||!game::dlc50DL::addonCareerView(pod->number,career)||(career.progress219C&31u)>13u)return false;
        }else if(mode==2){
            OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot career;
            if(!cold.profile->tournamentCareerSnapshot(state.rider4C4,career)||
               (career.cardFlag100BC&&!career.nativeBetaStorage)||
               (career.progress219C&31u)>13u)return false;
        }
        // PORT EXTENSION: an add-on rider has no confirm voice (DLC voice absent),
        // so the original per-rider voice preflight does not gate its Confirm.
        return confirmAudioReady()&&(mode==0||mode==2)&&(!addonSlot||(addonPod()&&(mode!=2||addonPlayer==0)))&&
            cold.commonProfile->mode34==mode&&(cold.commonProfile->playerBits1125C&0x60)==0x20&&
            cold.profile->riderSelectionFacts(confirmRider(),rider)&&rider.unlock10FAC&&
            confirmAnimationPreflight(d)&&(addonSlot?boundedRiderConfirmEnabled&&cold.application->clock101A50:
                confirmVoiceChronologyReady());
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
    bool nativePhase(std::uint8_t phase) noexcept {
        if(!nativeEntryActive)return true;
        if(phase!=nativeEntryPhase||phase>=nativeEntryPhases.size()||
           !widgets().matchesNativeProjection(nativeEntryPhases[phase])){
            fault(Status::resource,0x1E9E70);return false;
        }
        ++nativeEntryPhase;if(nativeEntryPhase==nativeEntryPhases.size())nativeEntryActive=false;
        return true;
    }
    bool projectNativeAppearance(NativeProjection& projection,std::uint32_t rider,
        const game::OriginalUiRiderSelectionProfile50KG& profile,Diagnostic& d) const noexcept {
        game::OriginalUiRiderAppearance50KG appearance;
        if(game::projectOriginalUiRiderAppearance50KG(rider,profile,appearance)!=game::OriginalUiRiderSelectionStatus50KG::ok)return false;
        for(std::size_t i=0;i<appearance.count;++i){const auto& w=appearance.widgets[i];
            const game::OriginalUiRiderWidgetCallback50KG call{w.widget,Operation::setEnabled,w.enable,w.sourcePC};
            if(!cold.resources->widgets()->projectNativeCallbacks(projection,&call,1,d.widget))return false;
        }
        return true;
    }
    bool projectNativeGroup(NativeProjection& projection,game::OriginalUiNavigation50KG& navigation,
        std::size_t group,bool enabled,Diagnostic& d) const noexcept {
        game::OriginalUiCommands50KG commands;
        if(!navigation.page||group>=navigation.page->groups.size())return false;
        navigation.groups[group].enabled=enabled;
        return groupMembers(navigation,commands,group,Operation::setEnabled,enabled,0x2A5044,0x2A5088)&&
            cold.resources->widgets()->projectNativeNavigation(projection,0,navigation,commands,d.widget);
    }
    bool projectNativeEntry(const game::OriginalUiRiderSelectionProfile50KG& profile,
        NativeProjection& before,std::array<NativeProjection,4>& phases,NativeProjection& projection,Diagnostic& d) const noexcept {
        if(!widgets().beginNativeProjection(dynamic,projection,d.widget)){result(d,Status::widget);return false;}
        before=projection;auto navigation=*nav();
        navigation.controllerGroup={{-1,-1}};game::OriginalUiCommands50KG resets;
        for(std::size_t group=0;group<navigation.page->groups.size();++group)
            if(!groupMembers(navigation,resets,group,Operation::reset,false,0x2A57D8,0x2A5818)){result(d,Status::widget);return false;}
        if(!widgets().projectNativeNavigation(projection,0,navigation,resets,d.widget)){result(d,Status::widget);return false;}
        phases[0]=projection;
        game::OriginalUiRiderSelectionPlan50KG remembered;
        if(game::planOriginalUiRiderEntry50KG(*selection(),navigation,0,profile,remembered)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           !preparedRider(remembered.rider4C4)){result(d,Status::unavailableDestination);return false;}
        const game::OriginalUiRiderWidgetCallback50KG lockText{4,Operation::setEnabled,false,0x1E9C80};
        if(!widgets().projectNativeCallbacks(projection,&lockText,1,d.widget)||
           !projectNativeGroup(projection,navigation,2,false,d)){result(d,Status::widget);return false;}
        phases[1]=projection;
        game::OriginalUiRiderSelectionPlan50KG opening;
        if(game::planOriginalUiRiderEntry50KG(*selection(),navigation,0,profile,opening)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           opening.rider4C4!=remembered.rider4C4||opening.slot4C8!=remembered.slot4C8||
           !preparedMovement(opening)||
           !widgets().projectNativeNavigation(projection,0,opening.nextNavigation,opening.navigationCommands,d.widget))
            {result(d,Status::unavailableDestination);return false;}
        navigation=opening.nextNavigation;
        if(!projectNativeAppearance(projection,opening.rider4C4,profile,d)||
           !projectNativeGroup(projection,navigation,2,false,d)){result(d,Status::widget);return false;}
        phases[2]=projection;
        if(!projectNativeGroup(projection,navigation,2,true,d)||
           !projectNativeAppearance(projection,opening.rider4C4,profile,d)){result(d,Status::widget);return false;}
        phases[3]=projection;
        for(const auto& phase:phases)if(!nativeDrawableClosure(phase,d))return false;
        return true;
    }
    bool nativeDrawableClosure(const NativeProjection& projection,Diagnostic& d) const noexcept {
        std::array<bool,120> models{};
        for(std::size_t i=1;i<models.size();++i){
            models[i]=projection.registered[i]&&projection.states[i].shown;
            if(!models[i])continue;
            std::uint32_t owners=0;
            for(const auto rider:game::originalUiRiderRoster50KG)
                if(dynamic.podWidget(rider)==i||dynamic.characterWidget(rider)==i){
                    ++owners;if(!preparedRider(rider)){result(d,Status::unavailableDestination);return false;}
                }
            if(owners!=1){result(d,Status::unavailableDestination);return false;}
        }
        return collector.nativeDrawableModels(models,d.draw);
    }
    bool nativeImageClosure(const NativeProjection& projection,std::vector<std::uint32_t>& images,bool& requiresCoverage,Diagnostic& d) const noexcept {
        if(!nativeDrawableClosure(projection,d))return false;
        std::array<bool,120> models{};
        for(std::size_t i=1;i<models.size();++i)
            models[i]=projection.registered[i]&&projection.states[i].shown;
        return collector.nativeResidencyImages(models,images,requiresCoverage,d.draw);
    }
    bool navigate(const game::OriginalUiNavigation50KG& next,const game::OriginalUiCommands50KG& commands,std::uint32_t pc) noexcept {
        if(nativeWidgetGeneration==std::numeric_limits<std::uint64_t>::max()){fault(Status::unsupportedState,pc);return false;}
        if(!widgets().commitNavigation(0,next,commands,dynamic.hooks(),callbackFault.widget)){
            callbackFault.dynamic=dynamic.diagnostic();callbackFault.backend=backend.diagnostic();fault(Status::widget,pc);return false;
        }
        ++nativeWidgetGeneration;return true;
    }
    bool widget(std::uint16_t id,Operation op,bool value,std::uint32_t pc) noexcept {
        if(nativeWidgetGeneration==std::numeric_limits<std::uint64_t>::max()){fault(Status::unsupportedState,pc);return false;}
        const game::OriginalUiRiderWidgetCallback50KG call{id,op,value,pc};
        if(!widgets().apply(&call,1,dynamic.hooks(),callbackFault.widget)){fault(Status::widget,pc);return false;}
        ++nativeWidgetGeneration;return true;
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
        state.pageShown=true;requested=true;return nativePhase(0);
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
        state.cameraActive=true;cold.application->uiObject3D8=0;entered=true;return nativePhase(1);
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
        if(!groupEnabled(2,false,0x1EA070))return false;opening=true;return nativePhase(2);
    }
    bool openedB0() noexcept {
        if(!opening||opened||closing||!groupEnabled(2,true,0x1EA0AC)){fault(Status::unsupportedState,0x1EA090);return false;}
        const auto mode=cold.commonProfile->mode34;
        if(!widgets().setDynamicLines(87,localized[mode==2?1:0],callbackFault.widget)){
            fault(Status::widget,mode==2?0x1EA158:0x1EA1F0);return false;}
        if(mode!=2&&!widgets().setDynamicLines(6,localized[2],callbackFault.widget)){fault(Status::widget,0x1EA294);return false;}
        if(!appearance())return false;opened=true;return nativePhase(3);
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
        if(!riderColors(colors)){fault(Status::profile,0x1EB674);return false;}
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
                if(addonSlot&&id==anchorCharacterWidget()){
                    // PORT EXTENSION (DLC step B): the hidden anchor character does not
                    // advance (or draw RNG) while an add-on is shown; the add-on character
                    // advances here instead, with the same shared motion/IK/RNG owners.
                    if(auto* scene=addonCharacter()){game::OriginalUiRiderModelDiagnostic50KG md;
                        if(!scene->advance(dt,cold.application->sharedNoise308030,cold.application->modelMotion,
                            cold.application->modelMode0Ik,md)){fault(Status::model,0x141ED0);return false;}}
                    continue;
                }
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
    std::uint32_t requested=0;
    if((cold.requestedTraversalRiders&~hiddenTraversalMask)||
       !game::originalUiRiderPreviewRequest50KG(cold.previewSet,cold.requestedPreviewRiders,requested))
        return result(d,Status::invalidInput);
    const bool gasgano=game::originalUiRiderRequested50KG(requested,7);
    const auto population=(cold.prepareCleggLockedPreview||game::originalUiRiderRequested50KG(requested,3))?
        (gasgano?game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview:
                  game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndCleggPreview):
        (gasgano?game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasgano:
                  game::OriginalUiRiderColdPopulation50KG::defaultUnlocked);
    auto prepared=cold;prepared.requestedTraversalRiders|=hiddenTraversalMask;
    // Immutable pair readiness follows actual retained visibility, including
    // locked previews. This read neither awards nor selects any rider; source
    // Confirm independently requires its actual unlock. Fresh rider1 stays hidden.
    game::OriginalUiRiderSelectionProfile50KG visibleProfile;
    const auto* selection=cold.resources&&cold.resources->widgets()?cold.resources->widgets()->selectionData():nullptr;
    const auto* page=selection?selection->page():nullptr;
    if(cold.profile&&cold.profile->selectionSnapshotForMode(cold.prospectiveMode,visibleProfile)&&
       visibleProfile.ownerReady&&page&&page->groups.size()>1){
        for(const auto control:page->groups[1].controls){
            if(control<0)continue;const auto rider=selection->controlRider(static_cast<std::uint16_t>(control));
            if(rider<0||static_cast<std::size_t>(rider)>=visibleProfile.riders.size())continue;
            const auto& row=visibleProfile.riders[static_cast<std::size_t>(rider)];
            if(row.ownerReady&&(row.unlock10FAC||!row.hidden730)&&
               !game::originalUiRiderColdMember50KG(population,requested,static_cast<std::uint32_t>(rider)))
                prepared.requestedPreviewRiders|=std::uint32_t{1}<<rider;
        }
    }
    return bindCold(prepared,population,d);
}
Status OriginalUiRiderSession50KG::bindCold(const OriginalUiRiderSessionCold50KG& cold,
    game::OriginalUiRiderColdPopulation50KG population,Diagnostic& d) noexcept {
    d={};if(storage_)return result(d,Status::alreadyBound);
    if(population!=game::OriginalUiRiderColdPopulation50KG::selected19&&
       population!=game::OriginalUiRiderColdPopulation50KG::rider19And20&&
       population!=game::OriginalUiRiderColdPopulation50KG::defaultUnlocked&&
       population!=game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasgano&&
       population!=game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndCleggPreview&&
       population!=game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview)return result(d,Status::invalidInput);
    const bool clegg=population==game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndCleggPreview||
        population==game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview;
    const bool gasgano=population==game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasgano||
        population==game::OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview;
    std::uint32_t requested=0;
    if((cold.requestedTraversalRiders&~hiddenTraversalMask)||
       !game::originalUiRiderPreviewRequest50KG(cold.previewSet,cold.requestedPreviewRiders,requested))
        return result(d,Status::invalidInput);
    if(clegg&&!cold.prepareCleggLockedPreview&&!game::originalUiRiderRequested50KG(requested,3))
        return result(d,Status::invalidInput);
    const bool gasganoPreview=game::originalUiRiderRequested50KG(requested,7);
    if(cold.previewSet==game::OriginalUiRiderPreviewSet50KG::gasgano7&&!gasgano)
        return result(d,Status::invalidInput);
    // Resource opt-in never changes the actual profile or source Confirm rules.
    if(gasgano&&!gasganoPreview)return result(d,Status::invalidInput);
    const bool fromMain=cold.previousScreen==4&&cold.predecessorResources&&
        !cold.predecessorCourseResources&&!cold.predecessorCourseHolograms&&!cold.predecessorTournament&&
        cold.predecessorResources->loaded()&&cold.predecessorResources->materials()&&cold.predecessorResources->particleMaterial();
    const bool fromCourse=cold.previousScreen==6&&!cold.predecessorResources&&!cold.predecessorTournament&&
        cold.predecessorCourseResources&&cold.predecessorCourseHolograms&&
        cold.predecessorCourseResources->loaded()&&cold.predecessorCourseResources->widgets()&&
        cold.predecessorCourseResources->materials()&&cold.predecessorCourseHolograms->loaded()&&
        cold.prospectiveMode==0;
    const bool fromTournament=cold.previousScreen==9&&!cold.predecessorResources&&
        !cold.predecessorCourseResources&&!cold.predecessorCourseHolograms&&cold.predecessorTournament&&cold.prospectiveMode==2;
    if((!fromMain&&!fromCourse&&!fromTournament)||!cold.resources||!cold.resources->loaded()||!cold.resources->widgets()||!cold.resources->bankInput()||
       !cold.resources->exitText(0)||!cold.resources->exitText(0)->prepared()||!cold.statAssets||!cold.statAssets->loaded()||
       !cold.binderAssets||!cold.binderAssets->loaded()||!genuine(cold.riderArchive,assets::OriginalUiArchiveId50KG::rider)||
       !genuine(cold.appStart,assets::OriginalUiArchiveId50KG::appStart)||!genuine(cold.podTextures,assets::OriginalUiArchiveId50KG::podTextures)||
       !cold.ipumCache||!cold.ipumCache->loaded()||!cold.english||!cold.english->loaded()||!cold.materialRuntime.overrideKnown)
        return result(d,Status::resource);
    if(!cold.profile||!cold.profile->initialized()||!cold.commonProfile||!cold.commonProfile->known||
       cold.profile->commonProfile()!=cold.commonProfile.get()||!cold.application||!cold.application->known||
       (cold.application->globalFlags228&4u)||cold.application->transportE00!=-1||cold.prospectiveMode>2||(fromCourse&&cold.commonProfile->mode34!=0)||(fromTournament&&cold.commonProfile->mode34!=2)||
       &cold.application->rawRandom308030!=&cold.application->sharedNoise308030.sharedRandomState||
       !cold.lightManager||cold.lightManager->cacheState()!=game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction||
       !cold.externalTransition||cold.externalTransition->snapshot().bound)return result(d,Status::unsupportedState);
    if((cold.audioMode==OriginalUiMainAudioMode50KG::muted&&cold.audio)||
       (cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&(!cold.audio||!cold.audio->riderMenuReady()))||
       (cold.audioMode!=OriginalUiMainAudioMode50KG::muted&&cold.audioMode!=OriginalUiMainAudioMode50KG::enabled))
        return result(d,Status::unavailableDestination,0x234900);
    try {
        auto s=std::make_unique<Storage>();s->cold=cold;s->destinationPreviewRiders=requested;s->cold.english=nullptr;s->cold.predecessorResources=nullptr;
        s->cold.predecessorCourseResources=nullptr;s->cold.predecessorCourseHolograms=nullptr;
        const auto* page=s->selection()?s->selection()->page():nullptr;
        if(!page||page->groups.size()!=3||page->groups[1].controls.size()!=23||!s->nav()||
           page->useVideoAsTexture||page->background!="*INVALID*"||!cold.resources->background(19)||
           !cold.statAssets->portrait(19)||cold.resources->bankInput()->owners.size()!=1||
           cold.resources->bankInput()->owners[0]!=cold.resources->images())return result(d,Status::resource,0x1D63B0);
        // Resolve original group membership and widget IDs from authenticated
        // rider.dat. No rider arithmetic or compact substitute roster.
        for(const auto rider:game::originalUiRiderRoster50KG){
            if(!game::originalUiRiderColdMember50KG(population,requested,rider))continue;
            std::size_t matches=0;
            for(std::size_t slot=1;slot<=page->groups[1].controls.size();++slot){
                const auto control=page->groups[1].controls[slot-1];
                if(control<0||static_cast<std::size_t>(control)>=page->controls.size())return result(d,Status::resource,0x1D63B0);
                if(s->selection()->controlRider(static_cast<std::uint16_t>(control))!=static_cast<std::int32_t>(rider))continue;
                std::array<std::uint16_t,4> ids{};
                if(!s->controlWidgets(static_cast<std::uint16_t>(slot),rider,ids))return result(d,Status::resource,0x1D63B0);
                ++matches;
            }
            if(matches!=1||!cold.resources->background(rider)||!cold.statAssets->portrait(rider))
                return result(d,Status::resource,0x1D63B0);
        }
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
        }else if(fromCourse){
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
        if(fromTournament)backend.tournamentPredecessor=cold.predecessorTournament;
        // PORT EXTENSION, NOT ORIGINAL (DLC menu-art step A): add-on pod models for the
        // free-play carousel, loaded while this session is still private. The layout
        // source is the authored pod widget of the default entry rider 19 (PORT STAND-IN).
        if((cold.prospectiveMode==0||cold.prospectiveMode==1)&&game::dlc50DL::addonRiderCount()){
            std::uint16_t layout=0,characterLayout=0;
            game::OriginalUiRiderAddonModels50DL::Input addon;
            for(const auto control:page->groups[1].controls){
                if(control<0||static_cast<std::size_t>(control)>=page->controls.size())continue;
                const auto rider=s->selection()->controlRider(static_cast<std::uint16_t>(control));
                const auto& all=page->controls[static_cast<std::size_t>(control)].widgets;
                // Authored pod/character widgets of every stock rider (ui.layout_slot).
                if(rider>0&&static_cast<std::size_t>(rider)<addon.podWidgets.size()&&all.size()==4&&
                   s->widgets().state(all[0]).kind==Kind::pod&&s->widgets().state(all[3]).kind==Kind::character){
                    addon.podWidgets[static_cast<std::size_t>(rider)]=all[0];addon.characterWidgets[static_cast<std::size_t>(rider)]=all[3];}
                if(rider!=19)continue;
                const auto& ids=all;
                if(!ids.empty()&&s->widgets().state(ids[0]).kind==Kind::pod)layout=ids[0];
                if(ids.size()==4&&s->widgets().state(ids[3]).kind==Kind::character)characterLayout=ids[3];
            }
            addon.stockRider=cold.riderArchive;addon.appStart=cold.appStart;addon.podTextures=cold.podTextures;
            addon.materialRuntime=cold.materialRuntime;addon.ipumCache=cold.ipumCache.get();addon.layoutWidget=layout;
            addon.characterLayoutWidget=characterLayout;
            if(layout&&s->addonModels.load(addon))backend.addonModels=&s->addonModels;
            else std::fprintf(stderr,"dlc_rider_model_prepare status=refused reason=\"no authored layout widget or table\"\n");
        }
        if(!s->backend.bind(backend,d.backend))return result(d,Status::backend);
        s->backendBound=true;
        if(!s->dynamic.bind(cold.riderArchive,s->widgets(),cold.application->modelMotion,cold.application->modelMode0Ik,
            cold.application->modelLibm,cold.application->sharedNoise308030,s->backend.callbacks(),d.dynamic))return result(d,Status::model,d.dynamic.sourcePC);
        game::OriginalUiRiderSelectionProfile50KG copiedProfile;game::OriginalUiRiderSelectionPlan50KG coldPlan;
        // Main's live mode4 is legitimate. Only this immutable cold projection
        // sees the exact prospective source decision; the shared word is untouched.
        if(!cold.profile->selectionSnapshotForMode(cold.prospectiveMode,copiedProfile)||
           game::planOriginalUiRiderEntry50KG(*s->selection(),*s->nav(),0,copiedProfile,coldPlan)!=game::OriginalUiRiderSelectionStatus50KG::ok)
            return result(d,Status::unavailableDestination,0x1E9C50);
        // This local session has not escaped. Prepare genuine immutable
        // descriptor candidates, then one complete bank and irreversible seal.
        // Subsequent callback preflight only reads that sealed population.
        if(!s->dynamic.prepareCold(population,d.dynamic,cold.previewSet,cold.requestedPreviewRiders|cold.requestedTraversalRiders))return result(d,Status::resource,d.dynamic.sourcePC);
        if(!s->backend.prepareColdAndSeal(s->dynamic,d.backend))return result(d,Status::backend);
        if(population!=game::OriginalUiRiderColdPopulation50KG::selected19&&
           (!s->preparedRider(19)||!s->preparedRider(20)))return result(d,Status::unavailableDestination,0x1E9C50);
        if(gasgano&&!s->preparedRider(7))return result(d,Status::unavailableDestination,0x1E9C50);
        if(clegg&&!s->preparedRider(3))return result(d,Status::unavailableDestination,0x1E9C50);
        for(const auto rider:game::originalUiRiderRoster50KG)
            if(game::originalUiRiderRequested50KG(requested|cold.requestedTraversalRiders,rider)&&!s->preparedTraversal(rider))
                return result(d,Status::unavailableDestination,0x1E9C50);
        if(!s->preparedRider(coldPlan.rider4C4))return result(d,Status::unavailableDestination,0x1E9C50);
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
        s->drawablePoliciesPrepared=true;
        Storage::NativeProjection initialBefore,initialAfter;std::array<Storage::NativeProjection,4> initialPhases;
        if(!s->preparedRider(coldPlan.rider4C4,&d.draw))
            return d.draw.error!=OriginalUiRiderContributionError50KG::none
                ?result(d,Status::draw,d.draw.sourcePC)
                :result(d,Status::unavailableDestination,0x1E9C50);
        if(!s->projectNativeEntry(copiedProfile,initialBefore,initialPhases,initialAfter,d))
            return result(d,Status::unavailableDestination,0x1E9C50);
        if(!s->backend.bindMaterialConsumer(&s->collector,s.get(),&Storage::consumeMaterialEvents))return result(d,Status::backend);
        // PORT EXTENSION (DLC D3/D4): add-on riders join the free-play carousel only.
        if(const auto addons=game::dlc50DL::addonRiderCount()){
            // Free play: mode0 (one human) and mode1 (two-player VS); Tournament
            // (mode2, P1 page) for the add-on pod's own career (dlc_career50dl.h).
            s->addonCarousel=cold.prospectiveMode==0||cold.prospectiveMode==1||
                (cold.prospectiveMode==2&&s->addonPlayer==0);
            game::dlc50DL::setHighlightedAddonRider(s->addonPlayer,0);
            std::fprintf(stderr,"dlc_rider_carousel player=%u action=%s mode=%u addons=%zu stock_slots=%zu\n",
                unsigned(s->addonPlayer)+1u,s->addonCarousel?"ready":"off",cold.prospectiveMode,addons,page->groups[1].controls.size());
        }
        s->state.bound=true;storage_=std::move(s);
        if(const auto shared=storage_->cold.application->sharedDialogs){
            if(!storage_->cold.resources->controllerPopup()||
               !storage_->collector.bindSharedMenu470(*this,shared))return result(d,Status::draw,0x204A00);
        }else if(storage_->cold.resources->controllerPopup())return result(d,Status::resource,0x2055B0);
        return result(d,Status::ok);
    }catch(const std::bad_alloc&){return result(d,Status::allocation);}catch(...){return result(d,Status::resource);}
}
bool OriginalUiRiderSession50KG::canEnterMode(std::uint32_t mode) const noexcept {
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||mode!=s->cold.prospectiveMode||!s->processCurrent())return false;
    game::OriginalUiRiderSelectionProfile50KG profile;game::OriginalUiRiderSelectionPlan50KG plan;
    return s->cold.profile->selectionSnapshotForMode(mode,profile)&&
        game::planOriginalUiRiderEntry50KG(*s->selection(),*s->nav(),0,profile,plan)==game::OriginalUiRiderSelectionStatus50KG::ok&&
        s->preparedRider(plan.rider4C4);
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
    if(!s->operational()||!s->backend.predecessorRetired()||s->inOperation||s->rendering||e.route.screen!=5||e.route.page!=1)return result(d,Status::unsupportedState,e.sourcePC);
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
        if(valid){
            if(const auto shared=s->cold.application->sharedDialogs){
                // Genuine constructor1E809C operation, deferred by native cold
                // preparation until this exact incoming owner is activated.
                // Never mutate the outgoing Main during private prewarming.
                OriginalUiSharedDialogs50KG::Diagnostic sd;
                if(s->sharedEntryAttempted){s->fault(Status::unsupportedState,0x1E809C);break;}
                s->sharedEntryAttempted=true;
                if(!shared->reappendRiderEntry1E809C(*this,sd)){
                    s->fault(Status::unsupportedState,sd.sourcePC);break;}
                s->sharedEntryComplete=true;
            }
            s->requested90();
        }break;
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
    const auto shared=s->cold.application->sharedDialogs;
    if(shared&&shared->menu470PendingFor(s->collector))return finish(Status::unsupportedState,0x204A00);
    s->cpuDrain();if(!s->invalidateProposal()){d=s->callbackFault;receipt.status=d.status;return d.status;}
    auto& popup=s->cold.application->controllerPopupState;
    // Global47C receives this same event before the Rider page. The opening
    // input5 cannot retroactively become popup Confirm. Closing state remains
    // active until its real renderer timer returns; no early dismissal.
    if(popup.active3CC){
        if(!shared||!s->sharedEntryComplete||!sharedMenuLifetimeCurrent(*shared,s->collector)||
           !s->collector.sharedPopupInputCurrent(*shared)||!popup.riderResetText)
            return finish(Status::unavailableDestination,0x2062D0);
        game::OriginalUiControllerPopupPlan50KG plan;
        if(game::OriginalUiControllerPopup50KG::input(popup,in.controller,in.originalInput,true,plan)!=
           game::OriginalUiControllerPopupStatus50KG::ok)return finish(Status::unsupportedState,0x2062D0);
        if(plan.accepted){
            if(!s->resetPending||!s->processCurrent()||
               (s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
                !s->cold.audio->canPlayMenuEffect(plan.soundId)))return finish(Status::audio,plan.soundPC);
            popup=plan.state;
            if(s->cold.audioMode==OriginalUiMainAudioMode50KG::muted)++s->state.audio.suppressedByMutedMode;
            else if(!s->cold.audio->playMenuEffect(plan.soundId)){
                ++s->state.audio.failed;s->fault(Status::audio,plan.soundPC);d=s->callbackFault;return d.status;
            }else ++s->state.audio.submitted;
            // Actual native command applies directly; transportE00==-1 was
            // authenticated by processCurrent. No extra PCM advance/flush.
        }
    }
    receipt.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
    if(!receipt.containerReached||!s->aperture.snapshot().inputEnabled268)return finish(Status::noChange);
    receipt.pageCallbackRan=true;
    //1EA478..490 reads47C result BEFORE player/controller/action dispatch.
    // This same Confirm must return after result handling, never launch a race.
    if(popup.result3D8){
        if(!shared||!s->resetPending||!popup.riderResetText||s->resetRider!=s->state.rider4C4||
           !sameStamp(s->cold.profile->stamp(),s->resetStamp)||!s->processCurrent()||!s->lightCurrent())
            return finish(Status::unsupportedState,0x1EAEA4);
        if(popup.result3D8==1u)return finish(Status::noChange,0x1EAF10);
        if(popup.result3D8==3u){
            popup.result3D8=0;s->resetPending=false;return finish(Status::ok,0x1EAF04);
        }
        if(popup.result3D8!=2u)return finish(Status::unsupportedState,0x1EAEA4);
        if(s->addonSlot){
            // PORT EXTENSION (DLC career): 1800E0 on the add-on pod's own career
            // (DLC side save); the anchor's stock career is not touched. The
            // add-on appearance is then projected as the original 1EAFB0 does.
            const auto* pod=s->addonPod();
            if(!s->idleInput()||!pod||!s->careerResetAllowed())return finish(Status::unavailableDestination,0x1EAEC8);
            if(!game::dlc50DL::resetAddonCareer(pod->number))return finish(Status::profile,0x1EAEC8);
            popup.result3D8=0;s->resetPending=false;
            if(!s->addonAppearance()){d=s->callbackFault;return d.status;}
            return finish(Status::ok,0x1EAEE8);
        }
        game::OriginalUiRiderSelectionProfile50KG before,after;
        game::OriginalUiRiderAppearance50KG appearance;
        Storage::NativeProjection projected;
        if(!s->idleInput()||!s->liveProfile(before)||s->resetRider>=before.riders.size()||
           !s->cold.profile->canResetRiderCareer1800E0(s->resetRider,s->resetStamp))
            return finish(Status::unavailableDestination,0x1EAEC8);
        after=before;after.riders[s->resetRider].career1938=0;
        if(game::projectOriginalUiRiderAppearance50KG(s->resetRider,after,appearance)!=
               game::OriginalUiRiderSelectionStatus50KG::ok||
           s->nativeWidgetGeneration>std::numeric_limits<std::uint64_t>::max()-appearance.count||
           !s->widgets().beginNativeProjection(s->dynamic,projected,d.widget)||
           !s->projectNativeAppearance(projected,s->resetRider,after,d)||!s->nativeDrawableClosure(projected,d))
            return finish(Status::unavailableDestination,0x1EAFB0);
        if(!s->cold.profile->resetRiderCareer1800E0(s->resetRider,s->resetStamp))
            return finish(Status::profile,0x1EAEC8);
        //1EAEE8 delay store precedes actual appearance1EAFB0. Do not undo the
        //profile if the later widget/lighting consumer fails.
        popup.result3D8=0;s->resetPending=false;
        if(!s->appearance()){d=s->callbackFault;return d.status;}
        return finish(Status::ok,0x1EAEE8);
    }
    if(in.controller!=0||in.popupActive||popup.active3CC||in.sourceInputBlocked)return finish(Status::noChange);
    if(!s->processCurrent()||!s->lightCurrent()||s->cold.application->uiObject3D8)return finish(Status::unsupportedState,0x1EA460);
    if(s->state.deferred4EC)return finish(Status::noChange,0x1EB510);
    if(s->panel.inputBlocked4E0()){
        if(in.originalInput!=4)return finish(Status::noChange,0x1EAF40);
        // Actual page0/controller0 second Confirm1EAF34..5C. Existing
        // container/input268 gates above still apply during panel travel and
        // readiness wait. Do not replay the accepted route, profile, voice
        // selection/RNG or character _sel action from the first Confirm.
        s->panel.confirmWhileClosing(); //1EAF54: bypass4E4=1 first.
        s->state.confirmStopTimer7D24=std::bit_cast<float>(std::uint32_t{0x3EB33333}); //1EAF5C
        return finish(Status::ok,0x1EAF5C);
    }
    game::OriginalUiRiderSelectionProfile50KG profile;
    if(!s->liveProfile(profile))return finish(Status::profile,0x1EA460);
    if(in.originalInput<=1){
        const auto planned=game::planOriginalUiRiderMove50KG(*s->selection(),*s->nav(),0,in.controller,in.originalInput,
            in.popupActive,s->panel.inputBlocked4E0(),profile,receipt.selection);
        if(planned==game::OriginalUiRiderSelectionStatus50KG::noChange)return finish(Status::noChange);
        if(planned!=game::OriginalUiRiderSelectionStatus50KG::ok)return finish(Status::unsupportedState,0x1EB1F0);
        // PORT EXTENSION (DLC D4): the add-on block between the last and first stock rider.
        switch(s->addonMove(in.originalInput==1,receipt.selection)){
        case Storage::AddonMove::stock:break;
        case Storage::AddonMove::handled:receipt.selection={};return finish(Status::ok,in.originalInput==1?0x1EB1F0u:0x1EB370u);
        case Storage::AddonMove::unavailable:receipt.selection={};return finish(Status::unavailableDestination,0x1EB1F0);
        case Storage::AddonMove::faulted:receipt.selection={};d=s->callbackFault;receipt.status=d.status;return d.status;
        }
        // Preserve the original planner's order and hidden-control traversal.
        // Never retry the plan to skip a visible but unprepared neighbor.
        if(!s->idleInput()||!receipt.selection.visitedControls||!s->preparedMovement(receipt.selection)||
           !s->preparedRider(receipt.selection.rider4C4)||
           !s->preparedRider(s->state.rider4C4)||!s->audioReady()||
           (s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&!s->cold.audio->canPlayRiderAdvance2()))
            return finish(Status::unavailableDestination,receipt.selection.sourcePC);
        receipt.moveSerial=s->proposalSerial;receipt.moveEvent=in.originalInput;
        return finish(Status::moveProposed,receipt.selection.sourcePC);
    }
    if(in.originalInput==5u){
        if(!(s->state.promptMask42C&4u))return finish(Status::noChange,0x1EA870);
        if(!shared||s->resetPending||!s->idleInput()||!s->sharedEntryComplete||
           s->cold.prospectiveMode!=2u||!s->collector.sharedPopupInputCurrent(*shared)||
           !s->careerResetAllowed()||
           (s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&!s->cold.audio->canPlayMenuEffect(1)))
            return finish(Status::unavailableDestination,0x1EAA20);
        OriginalUiSharedDialogs50KG::Diagnostic sd;
        s->resetInputActive=true;
        const bool admitted=shared->canStartRiderReset2060A0(*this,sd);
        if(!admitted){s->resetInputActive=false;return finish(Status::unavailableDestination,sd.sourcePC);}
        s->resetRider=s->state.rider4C4;s->resetStamp=s->cold.profile->stamp();
        const bool started=shared->startRiderReset2060A0(*this,sd);
        s->resetInputActive=false;
        if(!started){s->fault(Status::unsupportedState,sd.sourcePC);d=s->callbackFault;return d.status;}
        s->resetPending=true;
        if(s->cold.audioMode==OriginalUiMainAudioMode50KG::muted)++s->state.audio.suppressedByMutedMode;
        else if(!s->cold.audio->playMenuEffect(1)){
            ++s->state.audio.failed;s->fault(Status::audio,0x1EAA40);d=s->callbackFault;return d.status;
        }else ++s->state.audio.submitted;
        return finish(Status::ok,0x1EAA20);
    }
    if(in.originalInput!=4&&in.originalInput!=6)return finish(Status::noChange,0x1EA460);
    // PORT EXTENSION (DLC): an add-on Confirm runs the original page-1 Confirm for the
    // stock rider of P1's last stock Confirm (packed1125D, unchanged by it), so every
    // original field and the stock save keep that stock selection; the add-on number
    // is held only by dlc50DL::selectAddonRider at acceptance. Back stays original.
    s->addonConfirmRider=0;
    if(in.originalInput==4&&s->addonSlot){
        std::uint8_t packed=0;std::uint32_t comparison=0;
        if(!s->addonPod()||s->addonPlayer!=0||!s->cold.profile->riderConfirmSnapshot(packed,comparison)) // mode 2: DLC career (dlc_career50dl.h)
            return finish(Status::profile,0x1EA58C);
        const std::uint32_t stock=packed>>3u;
        if(stock>=27||!profile.riders[stock].ownerReady||!profile.riders[stock].unlock10FAC){
            std::fprintf(stderr,"dlc_rider_confirm key=%s status=refused reason=\"no unlocked stock Confirm to keep\" stock_rider=%u\n",
                s->addonPod()->key.c_str(),stock);
            return finish(Status::unavailableDestination,0x1EA694);
        }
        s->addonConfirmRider=stock;
        // DLC career (dlc_career50dl.h): latch the add-on career for its anchor
        // BEFORE the Tournament destination is prepared, so every career
        // snapshot that destination takes already reads the add-on career. The
        // Tournament screen then draws the add-on's own pod model in the
        // anchor's place, or nothing (never the anchor's model).
        // HOLD (coordinator, 2026-11-21, build cr-25): the original race-end route
        // 1AB5BC sends every tournament race without a Hall/Win/Unlock stop to the
        // Garage (13/1, 0x1AB818), and the race-return Garage entry faults the
        // frontend when 1D1950 refuses an add-on career (Garage model step G2-G6
        // not done). So the add-on Tournament Confirm stays refused until the
        // Garage loads the add-on's own model; the latch below is then restored.
        // TEST ONLY (eyecheck R5): RR_TEST_ADDON_TOURNAMENT=1 lets the add-on
        // Tournament Confirm through the latch, honoured only on an unattended
        // capture run (RR_UNATTENDED_CAPTURE=1, the same gate the runner's other
        // RR_TEST_ switches use). Players always get the refusal above.
        static const bool testAddonTournament=[]{
            const auto on=[](const char* name){const char* v=std::getenv(name);return v&&v[0]=='1'&&!v[1];};
            return on("RR_UNATTENDED_CAPTURE")&&on("RR_TEST_ADDON_TOURNAMENT");}();
        if(s->cold.prospectiveMode==2&&!testAddonTournament){
            std::fprintf(stderr,"dlc_rider_confirm key=%s status=refused reason=\"add-on tournament career waits for the add-on Garage model (race end goes to the Garage)\"\n",
                s->addonPod()->key.c_str());
            return finish(Status::unavailableDestination,0x1EA58C);
        }
        if(s->cold.prospectiveMode==2){
            std::fprintf(stderr,"dlc_rider_confirm key=%s status=latching mode=tournament test_override=1\n",s->addonPod()->key.c_str());
            if(!game::dlc50DL::latchTournamentCareer(stock,s->addonPod()->number))return finish(Status::profile,0x1EA58C);
        }
    }else if(in.originalInput==4&&s->cold.prospectiveMode==2)game::dlc50DL::clearTournamentCareer(); // stock tournament Confirm
    const auto selected=in.originalInput==4?s->confirmRider():s->state.rider4C4;
    if(selected>=profile.riders.size())return finish(Status::unsupportedState,0x1EA460);
    game::OriginalUiRiderContext50KG context;context.playerIndex=0;context.selectedRider=selected;context.mode34=profile.mode34;
    context.packed1125C=s->cold.commonProfile->playerBits1125C;
    context.admission.sourceControllerReady=true;context.admission.selectionOwnerReady=true;context.admission.profileOwnerReady=true;
    context.unlockOwnerReady=profile.riders[selected].ownerReady;
    context.selectedUnlockWord10FAC=profile.riders[selected].unlock10FAC;
    const auto routed=game::dispatchOriginalUiRider50KG(in.controller,static_cast<game::OriginalUiRouteInput50KG>(in.originalInput),context,receipt.decision);
    if(routed==game::OriginalUiRouteStatus50KG::noChange)return finish(Status::noChange);
    if(routed!=game::OriginalUiRouteStatus50KG::unavailable)return finish(Status::unsupportedState,0x1EA460);
    if(in.originalInput==4){
        const auto& r=receipt.decision;
        if(!s->idleInput()||!confirmShape(r,s->confirmRider(),s->cold.prospectiveMode)||r.admitted||r.action!=game::OriginalUiRouteAction50KG::unavailable||
           r.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)||
           !s->confirmReady(d.dynamic))return finish(Status::unavailableDestination,0x1EA694);
        if(!s->cold.profile->riderConfirmSnapshot(s->pendingPacked1125D,s->pendingComparison100C0))
            return finish(Status::profile,0x1EA58C);
        s->pendingProfile=profile;s->pendingCommon=*s->cold.commonProfile;s->pendingStamp=s->cold.profile->stamp();
        s->pendingBack=r;s->pendingLifecycle=s->transition();s->state.confirmProposalPending=true;
        receipt.routeProposal={this,s->proposalSerial,r,s->pendingStamp};return finish(Status::confirmProposed,r.request.sourcePC);
    }
    const auto& r=receipt.decision;
    if(!s->idleInput()||!s->backAudioReady()||!r.profileProposalComplete||r.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)||
       r.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||r.request.targetScreen!=4||r.request.targetPage!=1||
       r.profileChanges.count!=1||r.selectedRider!=static_cast<std::int32_t>(s->state.rider4C4))return finish(Status::unavailableDestination,0x1EA810);
    s->pendingProfile=profile;s->pendingCommon=*s->cold.commonProfile;s->pendingStamp=s->cold.profile->stamp();
    s->pendingBack=r;s->pendingLifecycle=s->transition();s->state.backProposalPending=true;
    receipt.routeProposal={this,s->proposalSerial,r,s->pendingStamp};return finish(Status::backProposed,0x1EA810);
}
Status OriginalUiRiderSession50KG::prepareEntryResidency(OriginalUiRiderResidencyDemand50KG& output,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->operational()||s->requested||s->inOperation||s->rendering||s->nativeEntryActive)
        return result(d,Status::invalidInput);
    try {
        game::OriginalUiRiderSelectionProfile50KG profile;
        if(!s->processCurrent()||!s->cold.profile->selectionSnapshotForMode(s->cold.prospectiveMode,profile)||
           !s->invalidateProposal())return result(d,Status::profile);
        Storage::NativeProjection projection,before;std::array<Storage::NativeProjection,4> phases;
        if(!s->projectNativeEntry(profile,before,phases,projection,d))return result(d,Status::unavailableDestination);
        OriginalUiRiderResidencyDemand50KG demand;
        demand.session_=this;demand.serial_=s->proposalSerial;demand.bank_=bank();demand.entry_=true;
        for(const auto& phase:phases){std::vector<std::uint32_t> images;bool requiresCoverage=false;
            if(!s->nativeImageClosure(phase,images,requiresCoverage,d))return result(d,Status::draw,d.draw.sourcePC);
            demand.indices_.insert(demand.indices_.end(),images.begin(),images.end());
            demand.requiresCoverage_=demand.requiresCoverage_||requiresCoverage;
        }
        std::sort(demand.indices_.begin(),demand.indices_.end());
        demand.indices_.erase(std::unique(demand.indices_.begin(),demand.indices_.end()),demand.indices_.end());
        if(!s->nativePool)s->nativePool=std::shared_ptr<OriginalUiRiderResidentPool50KG>(new OriginalUiRiderResidentPool50KG(*this,bank()));
        output=demand;s->nativeDemand=std::move(demand);s->nativeBefore=before;s->nativePost=projection;
        s->nativeEntryPhases=phases;s->nativeEntryPhase=0;s->nativeStamp=s->cold.profile->stamp();
        s->nativeCommon=*s->cold.commonProfile;s->nativeEntryProfile=profile;s->nativeEntryCommitted=false;
        s->nativePreparedWidgetGeneration=s->nativeWidgetGeneration;
        s->nativePreparedMaterialGeneration=s->backend.registryGeneration();s->nativePrepared=true;
        return result(d,Status::ok);
    }catch(...){return result(d,Status::allocation);}
}
Status OriginalUiRiderSession50KG::prepareMoveResidency(const OriginalUiRiderSessionInputReceipt50KG& receipt,
    OriginalUiRiderResidencyDemand50KG& output,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->operational()||!s->nativePool||s->inOperation||s->rendering||!s->idleInput()||
       receipt.status!=Status::moveProposed||receipt.moveSerial!=s->proposalSerial||receipt.moveEvent>1)
        return result(d,Status::invalidInput);
    try {
        game::OriginalUiRiderSelectionProfile50KG profile;game::OriginalUiRiderSelectionPlan50KG plan;
        if(!s->liveProfile(profile)||game::planOriginalUiRiderMove50KG(*s->selection(),*s->nav(),0,0,
           receipt.moveEvent,false,false,profile,plan)!=game::OriginalUiRiderSelectionStatus50KG::ok||
           !plan.visitedControls||!s->preparedMovement(plan)||!s->preparedRider(plan.rider4C4)||
           plan.rider4C4!=receipt.selection.rider4C4||plan.slot4C8!=receipt.selection.slot4C8)
            return result(d,Status::unavailableDestination);
        Storage::NativeProjection projection;
        if(!s->widgets().beginNativeProjection(s->dynamic,projection,d.widget))return result(d,Status::widget);
        const auto before=projection;
        if(!s->widgets().projectNativeNavigation(projection,0,plan.nextNavigation,plan.navigationCommands,d.widget)||
           !s->projectNativeAppearance(projection,plan.rider4C4,profile,d))return result(d,Status::widget);
        auto selected=plan.nextNavigation;game::OriginalUiCommands50KG repeat;
        const auto control=selected.page->groups[1].controls[plan.slot4C8-1];
        if(!appendControl(selected,repeat,control,Operation::select,false,receipt.moveEvent?0x1EB304:0x1EB484)||
           !s->widgets().projectNativeNavigation(projection,0,selected,repeat,d.widget))return result(d,Status::widget);
        OriginalUiRiderResidencyDemand50KG demand;
        demand.session_=this;demand.serial_=s->proposalSerial;demand.bank_=bank();
        // No draw occurs between the navigation, appearance and repeated SELECT.
        // Include the final whole-material closure, not a predicted clipped frame.
        if(!s->nativeImageClosure(projection,demand.indices_,demand.requiresCoverage_,d))return result(d,Status::draw,d.draw.sourcePC);
        output=demand;s->nativeDemand=std::move(demand);s->nativeBefore=before;s->nativePost=projection;
        s->nativeStamp=s->cold.profile->stamp();s->nativeCommon=*s->cold.commonProfile;s->nativeEntryProfile=profile;s->nativePreparedWidgetGeneration=s->nativeWidgetGeneration;
        s->nativePreparedMaterialGeneration=s->backend.registryGeneration();s->nativePrepared=true;
        return result(d,Status::ok);
    }catch(...){return result(d,Status::allocation);}
}
bool OriginalUiRiderSession50KG::residencyProposalCurrent(const OriginalUiRiderResidencyDemand50KG& demand) const noexcept {
    const auto* s=storage_.get();return s&&residencyProposalCurrentWithCommon(demand,s->nativeCommon);
}
bool OriginalUiRiderSession50KG::residencyProposalCurrentWithCommon(const OriginalUiRiderResidencyDemand50KG& demand,
    const OriginalUiMainProfile50KG& expectedCommon) const noexcept {
    const auto* s=storage_.get();
    game::OriginalUiRiderSelectionProfile50KG actual;
    if(!s||!s->processCurrent()||!s->cold.profile->selectionSnapshotForMode(s->cold.prospectiveMode,actual)||
       !sameProfile(actual,s->nativeEntryProfile))return false;
    return s->operational()&&s->nativePrepared&&!s->nativeDemand.consumed_&&
        demand.session_==this&&demand.serial_==s->proposalSerial&&demand.serial_==s->nativeDemand.serial_&&
        demand.bank_==bank()&&demand.bank_==s->nativeDemand.bank_&&demand.entry_==s->nativeDemand.entry_&&
        demand.indices_==s->nativeDemand.indices_&&demand.requiresCoverage_==s->nativeDemand.requiresCoverage_&&!demand.consumed_&&
        s->nativePreparedWidgetGeneration==s->nativeWidgetGeneration&&
        s->nativePreparedMaterialGeneration==s->backend.registryGeneration()&&
        sameStamp(s->cold.profile->stamp(),s->nativeStamp)&&sameCommon(*s->cold.commonProfile,expectedCommon)&&
        s->widgets().matchesNativeProjection(s->nativeBefore);
}
bool OriginalUiRiderSession50KG::acceptMainRememberedSlotCommit(
    std::uint16_t previousSlot,std::uint16_t selectedSlot) noexcept {
    auto* s=storage_.get();OriginalUiMainProfile50KG expected;
    if(!s||!s->nativeEntryCommitted||!s->nativeDemand.entry_||
       !mainRememberedSlotProjection(s->nativeCommon,previousSlot,selectedSlot,s->cold.prospectiveMode,expected)||
       !residencyProposalCurrentWithCommon(s->nativeDemand,expected))return false;
    // All unchanged residency/profile/owner guards passed. Publish only +6E0.
    s->nativeCommon.savedSlot6E0=expected.savedSlot6E0;return true;
}
bool OriginalUiRiderSession50KG::acceptEntryProfileCommit(OriginalUiRiderProfileStamp50KG before,
    const OriginalUiMainProfile50KG& expected,EntryOrigin origin) noexcept {
    auto* s=storage_.get();if(!s||!s->operational()||!s->nativePrepared||!s->nativeDemand.entry_||s->nativeDemand.consumed_||
       !sameStamp(before,s->nativeStamp)||!sameCommon(*s->cold.commonProfile,expected)||
       expected.mode34!=s->cold.prospectiveMode||s->nativePreparedWidgetGeneration!=s->nativeWidgetGeneration||
       s->nativePreparedMaterialGeneration!=s->backend.registryGeneration()||!s->widgets().matchesNativeProjection(s->nativeBefore))return false;
    const bool courseBack=origin==EntryOrigin::course;
    if((origin==EntryOrigin::main&&s->cold.previousScreen!=4)||
       (courseBack&&(s->cold.previousScreen!=6||s->cold.prospectiveMode!=0))||
       (origin==EntryOrigin::tournament&&(s->cold.previousScreen!=9||s->cold.prospectiveMode!=2||!s->cold.predecessorTournament))||
       (origin!=EntryOrigin::main&&origin!=EntryOrigin::course&&origin!=EntryOrigin::tournament))return false;
    const auto after=s->cold.profile->stamp();
    if(after.mode34!=s->cold.prospectiveMode||
       (origin==EntryOrigin::tournament&&before.mode34!=2)||
       (courseBack?(before.mode34!=0||after.mode34!=0||before.generation==std::numeric_limits<std::uint64_t>::max()||
            after.generation!=before.generation+1):after.generation!=before.generation))return false;
    game::OriginalUiRiderSelectionProfile50KG actual;
    if(!s->liveProfile(actual)||!sameProfile(actual,s->nativeEntryProfile))return false;
    s->nativeStamp=after;s->nativeCommon=expected;s->nativeEntryCommitted=true;return true;
}
bool OriginalUiRiderSession50KG::acceptTournamentPredecessorRetirement(
    game::OriginalUiTournamentPredecessorRetirement50KG& receipt,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->operational()||s->cold.previousScreen!=9||!s->cold.predecessorTournament||
       !s->nativeEntryCommitted||!s->nativePrepared||s->requested||s->entered||s->inOperation||s->rendering||
       !sameStamp(s->nativeStamp,s->cold.profile->stamp())||!sameCommon(s->nativeCommon,*s->cold.commonProfile)){
        result(d,Status::invalidInput);return false;
    }
    if(!s->backend.acceptTournamentPredecessorRetirement(receipt,d.backend)){result(d,Status::backend);return false;}
    return true;
}
bool OriginalUiRiderSession50KG::consumeEntryResidency() noexcept {
    auto* s=storage_.get();if(!s||!s->nativeEntryCommitted||!s->nativeDemand.entry_||!residencyProposalCurrent(s->nativeDemand))return false;
    s->nativeDemand.consumed_=true;s->nativeEntryActive=true;s->nativeEntryPhase=0;return true;
}
const OriginalUiRiderResidencyDemand50KG* OriginalUiRiderSession50KG::residencyDemand() const noexcept {
    return storage_&&storage_->nativePrepared?&storage_->nativeDemand:nullptr;
}
std::shared_ptr<OriginalUiRiderResidentPool50KG> OriginalUiRiderSession50KG::residentPool() const noexcept {
    return storage_?storage_->nativePool:nullptr;
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
       !plan.visitedControls||!s->preparedMovement(plan)||!s->preparedRider(plan.rider4C4)||plan.rider4C4!=receipt.selection.rider4C4||
       plan.slot4C8!=receipt.selection.slot4C8||!plan.captureAndRestorePodTransform||!plan.resetSelectedControl||plan.deferred4EC!=3)
        return result(d,Status::unavailableDestination,0x1EB1F0);
    std::array<std::uint16_t,4> outgoingIds{},incomingIds{};
    if(!s->preparedRider(s->state.rider4C4)||
       !s->controlWidgets(s->state.slot4C8,s->state.rider4C4,outgoingIds)||
       !s->controlWidgets(plan.slot4C8,plan.rider4C4,incomingIds))
        return result(d,Status::unavailableDestination,0x1EB1F0);
    const auto outgoing=outgoingIds[0],incoming=incomingIds[0];
    auto* old=s->dynamic.podScene(outgoing);const auto asset=s->dynamic.podAssets(incoming);
    game::OriginalUiRiderPodOrientation50KG orientation;game::BodyMatrix48G pose;
    if(!old||!s->backend.attached(outgoing)||!asset||!asset->config()||!s->cold.resources->background(plan.rider4C4)||
       !s->cold.statAssets->portrait(plan.rider4C4)||!old->captureDirectionalOrientation(orientation,d.dynamic.model)||
       !game::originalUiRiderPodOrientationPose50KG(asset->config()->position,orientation,pose))
        return result(d,Status::model,0x28A530);
    if(s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&!s->cold.audio->canPlayRiderAdvance2())
        return result(d,Status::unavailableDestination,0x292360);
    Storage::NativeProjection finalProjection;
    if(!s->widgets().beginNativeProjection(s->dynamic,finalProjection,d.widget)||
       !s->widgets().projectNativeNavigation(finalProjection,0,plan.nextNavigation,plan.navigationCommands,d.widget)||
       !s->projectNativeAppearance(finalProjection,plan.rider4C4,profile,d))return result(d,Status::unavailableDestination);
    auto finalNavigation=plan.nextNavigation;game::OriginalUiCommands50KG finalRepeat;
    const auto finalControl=finalNavigation.page->groups[1].controls[plan.slot4C8-1];
    if(!appendControl(finalNavigation,finalRepeat,finalControl,Operation::select,false,receipt.moveEvent?0x1EB304:0x1EB484)||
       !s->widgets().projectNativeNavigation(finalProjection,0,finalNavigation,finalRepeat,d.widget)||
       !s->nativeDrawableClosure(finalProjection,d))return result(d,Status::unavailableDestination);
    const auto generation=s->collector.sourceSubmissionWatermark(),frontier=s->backend.materialLifetime().sequence();
    if(!generation||s->collector.sourceRenderer()!=&renderer)return result(d,Status::unavailableDestination);
    if(s->nativePool){
        if(!residencyProposalCurrent(s->nativeDemand)||s->nativeDemand.entry_)return result(d,Status::unavailableDestination);
        s->nativeDemand.consumed_=true;
    }
    // Retire the exact source frame AND its shared canonical/history lease
    // before model callbacks or the sparse GPU publication is replaced.
    if(!s->collector.retireSourceRenderer(renderer,rd,d.draw)||
       !rd.submittedWorkCompleted)return result(d,Status::backend,d.draw.sourcePC);
    s->cpuDrain();
    if(!s->backend.acceptSourceRetirement(&s->collector,generation,frontier))return result(d,Status::backend);
    if(!s->widgets().preflightNavigation(0,plan.nextNavigation,plan.navigationCommands,s->dynamic.hooks(),d.widget)){
        // Retirement is irreversible. An unexpected allocation/backend failure
        // here is a cleanup-owned fault, never a usable unchanged destination.
        s->callbackFault.widget=d.widget;s->fault(Status::widget,plan.sourcePC);d=s->callbackFault;return d.status;
    }
    auto selected=plan.nextNavigation;game::OriginalUiCommands50KG repeat;
    const auto control=selected.page->groups[1].controls[plan.slot4C8-1];
    // 2A5A00 dispatches widget+78: repeated SELECT, never generic reset/show.
    if(!appendControl(selected,repeat,control,Operation::select,false,receipt.moveEvent?0x1EB304:0x1EB484)||
       !s->widgets().preflightNavigation(0,selected,repeat,s->dynamic.hooks(),d.widget)){
        s->callbackFault.widget=d.widget;s->fault(Status::widget,0x2A5A00);d=s->callbackFault;return d.status;
    }
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
    if(okay&&(!s->widgets().matchesNativeProjection(finalProjection)||
       (s->nativePool&&!s->widgets().matchesNativeProjection(s->nativePost)))){
        s->fault(Status::resource,plan.sourcePC);okay=false;
    }
    if(!okay){if(!s->state.faulted)s->fault(Status::widget,plan.sourcePC);d=s->callbackFault;return d.status;}
    return result(d,Status::ok,plan.sourcePC);
}
bool OriginalUiRiderSession50KG::ownsProfile(const OriginalUiRiderProfileOwner50KG& profile) const noexcept {
    return storage_&&storage_->cold.profile.get()==&profile;
}
bool OriginalUiRiderSession50KG::routeProposalCurrent(const OriginalUiRiderRouteProposal50KG& p) const noexcept {
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||s->updatePrepared||(!s->state.backProposalPending&&!s->state.confirmProposalPending)||
       p.source!=this||!p.serial||p.serial!=s->proposalSerial||!s->idleInput()||!s->lightCurrent()||!s->processCurrent()||
       s->cold.application->uiObject3D8||!sameDecision(p.decision,s->pendingBack)||!sameStamp(p.profileStamp,s->pendingStamp)||
       !sameStamp(s->cold.profile->stamp(),s->pendingStamp)||!sameCommon(*s->cold.commonProfile,s->pendingCommon)||
       !sameLife(s->transition(),s->pendingLifecycle)||!s->preparedRider(s->state.rider4C4)||s->nav()->controllerGroup[0]!=1||
       s->nav()->groups[1].selectedSlot!=s->state.slot4C8)return false;
    if(s->state.confirmProposalPending){
        game::OriginalUiRiderDynamicDiagnostic50KG preflight;std::uint8_t packed=0;std::uint32_t comparison=0;
        if(s->state.backProposalPending||!confirmShape(p.decision,s->confirmRider(),s->cold.prospectiveMode)||!s->confirmReady(preflight)||
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
           !confirmShape(p.decision,s->confirmRider(),s->cold.prospectiveMode)||!sameStamp(p.profileStamp,s->pendingStamp)||!s->processCurrent()||!s->lightCurrent()||
           !s->confirmReady(d.dynamic)||s->cold.application->uiObject3D8||!sameCommon(*s->cold.commonProfile,s->pendingCommon))
            return result(d,Status::invalidInput,s->cold.prospectiveMode==2?0x1EA5E4:0x1EA604);
        auto expectedLife=s->pendingLifecycle;expectedLife.requested={static_cast<std::uint8_t>(s->cold.prospectiveMode==2?9:6),1};expectedLife.lifecycle=Life::requestScreenExit;
        const auto stamp=s->cold.profile->stamp();std::uint8_t packed=0;std::uint32_t comparison=0;
        if(!sameLife(s->transition(),expectedLife)||s->pendingStamp.generation==std::numeric_limits<std::uint64_t>::max()||
           stamp.generation!=s->pendingStamp.generation+1||stamp.mode34!=s->cold.prospectiveMode||
           !s->cold.profile->riderConfirmSnapshot(packed,comparison)||
           packed!=static_cast<std::uint8_t>((s->pendingPacked1125D&7u)|(s->confirmRider()<<3))||comparison!=s->confirmRider())
            return result(d,Status::profile,0x1EA58C);
        auto expected=s->pendingProfile;
        if(s->cold.prospectiveMode==2)expected.remembered6FC=s->confirmRider();else expected.remembered6F0=s->confirmRider();
        game::OriginalUiRiderSelectionProfile50KG actual;
        if(!s->liveProfile(actual)||!sameProfile(actual,expected))return result(d,Status::profile,s->cold.prospectiveMode==2?0x1EA54C:0x1EA538);
        std::uint16_t character=0;if(!s->selectedConfirmCharacter(character))return result(d,Status::model,0x1ECF70);
        s->cpuDrain();s->state.fadeMusic4E8=true; //1EA600(mode2)/1EA620(mode0), after actual route/profile commit.
        // PORT EXTENSION: an add-on rider has no confirm voice (no choice, no RNG draw).
        if(s->addonSlot){if(!s->confirmAudioReady()){s->fault(Status::unsupportedState,0x1EA694);d=s->callbackFault;return d.status;}
            ++s->state.audio.suppressedBySourceGate;}
        else if(!s->applyConfirmVoiceChronology()){d=s->callbackFault;return d.status;}
        if(s->addonSlot){
            // PORT EXTENSION (DLC step B): the add-on character plays its own _sel;
            // its readiness gates the panel exit as a stock character's does.
            auto* scene=s->addonCharacter();game::OriginalUiRiderModelDiagnostic50KG md;
            if(!scene||!scene->confirm(md)){s->fault(Status::model,0x1D2690);return result(d,Status::model,0x1D2690);}
            if(!s->addonSelectLogged){s->addonSelectLogged=true;const auto* pod=s->addonPod();
                std::fprintf(stderr,"dlc_rider_character key=%s motion=sel status=started\n",pod?pod->key.c_str():"-");}
        }else if(!s->dynamic.confirmCharacter(character)){d.dynamic=s->dynamic.diagnostic();s->fault(Status::model,0x1D2690);return result(d,Status::model,0x1D2690);}
        //1EA7B0 follows the actual voice command and character _sel action.
        if(s->cold.audioMode==OriginalUiMainAudioMode50KG::muted)++s->state.audio.suppressedByMutedMode;
        else if(!s->cold.audio->playRiderConfirmSelection1()){
            ++s->state.audio.failed;s->fault(Status::audio,0x1EA7B0);d=s->callbackFault;return d.status;
        }else ++s->state.audio.submitted;
        s->state.confirmProposalPending=false;s->confirmAccepted=true;
        // PORT EXTENSION (DLC): the port-owned add-on selection follows the accepted
        // Confirm; a stock Confirm returns to the stock selection the profile keeps.
        if(const auto* pod=s->addonPod()){
            const bool selected=game::dlc50DL::selectAddonRider(s->addonPlayer,pod->number);
            std::fprintf(stderr,"dlc_rider_confirm player=%u key=%s number=%u status=%s stock_rider=%u\n",
                unsigned(s->addonPlayer)+1u,pod->key.c_str(),pod->number,selected?"selected":"refused",s->confirmRider());
        }else game::dlc50DL::clearAddonRider(s->addonPlayer);
        if(s->addonCarousel)game::dlc50DL::forgetAddonRider(s->addonPlayer); // only Back re-highlights
        // Do NOT call acceptedBack/confirmWhileClosing or set bypass4E4. The
        // panel waits for actual _sel >80% readiness before closing aperture.
        return result(d,Status::ok);
    }
    if(!s->operational()||s->inOperation||s->updatePrepared||s->backAccepted||!s->state.backProposalPending||p.source!=this||
       !p.serial||p.serial!=s->proposalSerial||!sameDecision(p.decision,s->pendingBack)||!sameStamp(p.profileStamp,s->pendingStamp)||
       !s->processCurrent()||!s->lightCurrent()||!s->backAudioReady()||s->cold.application->uiObject3D8||!sameCommon(*s->cold.commonProfile,s->pendingCommon)||
       !s->preparedRider(s->state.rider4C4)||s->nav()->controllerGroup[0]!=1||s->nav()->groups[1].selectedSlot!=s->state.slot4C8)
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
    // PORT EXTENSION (DLC): Back records the highlighted add-on port-owned (the
    // original Back word above keeps the anchor stock rider); a stock Back forgets it.
    if(s->addonCarousel){
        if(const auto* pod=s->addonPod()){
            const bool kept=game::dlc50DL::rememberAddonRider(s->addonPlayer,pod->number,s->state.rider4C4,s->addonAnchorLast);
            std::fprintf(stderr,"dlc_rider_back player=%u key=%s number=%u status=%s anchor_rider=%u\n",
                unsigned(s->addonPlayer)+1u,pod->key.c_str(),pod->number,kept?"remembered":"refused",s->state.rider4C4);
        }else game::dlc50DL::forgetAddonRider(s->addonPlayer);
    }
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
        if(!s->panel.update38(in.elapsedSeconds,t.lifecycle,t.phase,*s->nav(),s->dynamic,s->addonCharacter()))s->fault(Status::aperture,0x1ECFE0);
        if(!s->state.faulted)s->updateConfirmStop1EB5C8(in.elapsedSeconds);
        if(!s->state.faulted&&s->aperture.saveRiderUpdateDelta38(in.elapsedSeconds)!=ApertureStatus::ok)
            s->fault(Status::aperture,0x1EB620);
    }
    if(!s->state.faulted)s->updateWidgets(*top,in.elapsedSeconds);
    if(!s->state.faulted)if(const auto shared=s->cold.application->sharedDialogs){
        OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!shared->advanceMenu470(*this,s->collector,in.elapsedSeconds,sd))s->fault(Status::unsupportedState,sd.sourcePC);
    }
    s->inOperation=false;
    if(!s->state.faulted&&s->addonSlot)if(auto* row=s->addonModelRow(s->addonSlot)){
        game::OriginalUiRiderModelDiagnostic50KG md;
        if(!row->scene->advance(in.elapsedSeconds,md)&&!s->addonAdvanceLogged){s->addonAdvanceLogged=true;
            std::fprintf(stderr,"dlc_rider_model key=%s status=paused reason=\"User_Rotate advance refused\"\n",row->key.c_str());}
    }
    if(!s->state.faulted&&!s->restoreRememberedAddon()&&!s->state.faulted)s->fault(Status::widget,0x1EAFB0);
    if(s->state.faulted){s->cpuDrain();d=s->callbackFault;return d.status;}
    if(s->state.updates==std::numeric_limits<std::uint64_t>::max()){s->fault(Status::unsupportedState,0x1EB4F0);d=s->callbackFault;return d.status;}
    ++s->state.updates;return result(d,Status::ok);
}
bool OriginalUiRiderSession50KG::sharedMenuLifetimeCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiRiderDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();
    return s&&s->state.bound&&!s->sourceTornDown&&&s->collector==&draw&&s->cold.application&&
        s->cold.application->sharedDialogs.get()==&shared&&s->cold.resources&&
        s->cold.resources->controllerPopup()==shared.controller47C_.resources;
}
bool OriginalUiRiderSession50KG::sharedMenuUnwindEntryCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiRiderDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();return sharedMenuLifetimeCurrent(shared,draw)&&!s->inOperation&&!s->rendering;
}
bool OriginalUiRiderSession50KG::sharedMenuRenderCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiRiderDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();return sharedMenuLifetimeCurrent(shared,draw)&&s->sharedEntryComplete&&
        s->inOperation&&s->rendering&&s->operational()&&!s->updatePrepared&&s->state.updates&&s->currentPage();
}
bool OriginalUiRiderSession50KG::sharedMenuUpdateCurrent(const OriginalUiSharedDialogs50KG& shared,
    const OriginalUiRiderDrawContribution50KG& draw)const noexcept {
    const auto* s=storage_.get();return sharedMenuLifetimeCurrent(shared,draw)&&s->sharedEntryComplete&&
        s->inOperation&&!s->rendering&&s->operational()&&!s->updatePrepared&&s->currentPage();
}
bool OriginalUiRiderSession50KG::sharedMenuEntryCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
    const auto* s=storage_.get();if(!s||!sharedMenuLifetimeCurrent(shared,s->collector))return false;
    const auto t=s->transition();
    return s->inOperation&&!s->rendering&&s->operational()&&s->backend.predecessorRetired()&&
        s->sharedEntryAttempted&&!s->sharedEntryComplete&&!s->requested&&!s->entered&&
        t.bound&&t.current.screen==5&&t.current.page==1&&t.initialRestartPending&&
        t.lifecycle==Life::idle&&t.phase==Phase::none;
}
bool OriginalUiRiderSession50KG::sharedResetInputCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
    const auto* s=storage_.get();return s&&sharedMenuLifetimeCurrent(shared,s->collector)&&
        s->resetInputActive&&s->sharedEntryComplete&&s->operational()&&!s->updatePrepared&&!s->rendering&&
        s->idleInput()&&s->processCurrent()&&s->lightCurrent()&&s->cold.prospectiveMode==2u&&
        (s->state.promptMask42C&4u)&&!s->cold.application->controllerPopupState.active3CC&&
        !s->cold.application->controllerPopupState.result3D8&&s->collector.sharedPopupInputCurrent(shared)&&
        s->careerResetAllowed();
}
Status OriginalUiRiderSession50KG::render(const OriginalUiRiderDrawClock50KG& clock,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation||s->rendering||s->updatePrepared||!s->currentPage()||!clock.known||(s->paused&&clock.advanceAnimations))
        return result(d,Status::invalidInput);
    if(!s->processCurrent()||!s->lightCurrent()||!s->state.cameraActive||!s->state.pageShown||
       s->cold.application->uiObject3D8||!s->aperture.snapshot().hasUpdateDelta)return result(d,Status::unsupportedState,0x1ECE20);
    Storage::NativeProjection liveProjection;
    if(!s->widgets().beginNativeProjection(s->dynamic,liveProjection,d.widget)||!s->nativeDrawableClosure(liveProjection,d))
        return result(d,Status::unavailableDestination);
    game::OriginalUiRiderStatInput50KG stats;
    if(!s->cold.profile->statSnapshot(s->state.rider4C4,s->panel.offset4DC(),stats))return result(d,Status::profile,0x1EBBF0);
    if(s->addonSlot){
        // PORT EXTENSION (DLC): the add-on's own PHY stats in the original projection
        // (current = [BaseAbilities], maximum = [MaxAbilities], lanes in the original
        // order), unlocked, no medals, identity = the add-on's own registry number
        // (the stats code accepts 32..99 and projects no stock portrait for it).
        // Colours: Storage::riderColors (package ui.colors, else the layout_slot
        // rider's, else the anchor's as a logged PORT STAND-IN), as pageColors uses.
        const auto* pod=s->addonPod();std::array<std::uint16_t,4> ids{};
        if(!pod||!s->controlWidgets(s->state.slot4C8,s->state.rider4C4,ids))return result(d,Status::unavailableDestination,0x1EBBF0);
        stats.selectedRider=stats.observedRider=pod->number;
        stats.unlocked10FAC=1;stats.medal1938={};stats.careerTrack219C=0;
        for(std::size_t lane=0;lane<stats.current1920.size();++lane){
            stats.current1920[lane]=pod->baseAbilities[lane];stats.maximum0408[lane]=pod->maxAbilities[lane];
        }
        // DLC career (Tournament): the career's own row, medals and progress, as
        // statSnapshot gives a stock rider its career's diagonal row.
        if(s->cold.prospectiveMode==2){
            game::dlc50DL::AddonCareerView50DL career;
            if(!game::dlc50DL::addonCareerView(pod->number,career))return result(d,Status::profile,0x1EBBF0);
            stats.current1920=career.own.abilities1920;stats.medal1938=career.own.medals1938;
            stats.careerTrack219C=career.progress219C&31u;
        }
        auto& overlay=s->addonOverlay;overlay={};
        overlay.collector=&s->collector;overlay.suppressed=ids;overlay.nameWidget=ids[1];
        overlay.lines[0]={&s->addonText[0],&s->addonText[1]}; // name only (the package name is logged, not drawn)
        overlay.hutteseWidget=ids[2];overlay.hutteseLines[0]={&s->addonText[3],nullptr};
        {OriginalUiRiderProfileColors50KG colors;
            if(!s->riderColors(colors))return result(d,Status::profile,0x1EBBF0);
            stats.accentColor17FA00=colors.accent17FA00;}
        overlay.statRider=pod->number;overlay.anchorRider=s->state.rider4C4;
        // Menu-art step A: the add-on's own pod model in place of the anchor pod.
        if(s->addonModelRow(s->addonSlot))overlay.modelRow=s->addonSlot-1;
        else overlay.modelRefused="no ready add-on model row";
    }
    const riderAddon50DL::Scope addonScope(s->addonSlot?&s->addonOverlay:nullptr);
    s->cpuDrain();if(!s->invalidateProposal()){d=s->callbackFault;return d.status;}
    s->inOperation=true;s->rendering=true;
    // Retained global47C projects before this page. Its native sibling
    // callback is followed by ordinary widgets/models, custom appearance/stat
    // draw/backgrounds, aperture(+440 BEFORE rectangles), prompts, top
    // widgets and group drain. Only the full collector can seal the frame.
    bool okay=s->collector.begin(clock,s->renderContext,s->lighting,d.draw);
    if(okay&&s->cold.application->sharedDialogs){
        game::OriginalUiControllerPopupRender50KG popupRender;
        OriginalUiSharedDialogs50KG::Diagnostic sd;
        okay=s->cold.application->sharedDialogs->renderMenuPopupSibling(popupRender,sd);
        if(!okay)d.draw.sourcePC=sd.sourcePC;
        else okay=s->collector.projectControllerRequiredPopup(popupRender,d.draw);
    }
    if(okay)okay=s->collector.projectOrdinaryWidgets(true,d.draw);
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
    if(s->addonSlot&&!s->addonPageLogged){
        // One line per add-on page asset (first rendered frame of the highlight).
        s->addonPageLogged=true;const auto* pod=s->addonPod();const auto* row=s->addonModelRow(s->addonSlot);
        std::fprintf(stderr,"dlc_rider_page key=%s portrait=%s background=%s%s%s%s\n",pod?pod->key.c_str():"-",
            s->addonOverlay.portraitDrawn?"drawn":(row&&row->portraitReady?"refused":"none"),
            s->addonOverlay.backgroundDrawn?"drawn":(row&&row->backgroundReady?"refused":"none"),
            s->addonOverlay.pageRefused?" reason=\"":"",s->addonOverlay.pageRefused?s->addonOverlay.pageRefused:"",
            s->addonOverlay.pageRefused?"\"":"");
    }
    if(s->addonSlot&&!s->addonCharacterLogged){
        s->addonCharacterLogged=true;const auto* pod=s->addonPod();
        const bool drawn=s->addonOverlay.characterPackets&&!s->addonOverlay.characterRefused;
        const char* why=!s->addonCharacterNote.empty()?s->addonCharacterNote.c_str():
            s->addonOverlay.characterRefused?s->addonOverlay.characterRefused:"no packet";
        const auto* crow=s->addonModelRow(s->addonSlot);
        std::fprintf(stderr,"dlc_rider_character key=%s motion=ent status=%s packets=%u layout=%s layout_widget=%u%s%s%s\n",
            pod?pod->key.c_str():"-",drawn?"drawn":"refused",s->addonOverlay.characterPackets,
            crow&&crow->characterLayoutFixed?"layout_slot":"anchor",crow?unsigned(crow->characterLayout):0u,
            drawn?"":" reason=\"",drawn?"":why,drawn?"":"\"");
    }
    if(s->addonSlot&&!s->addonBinderLogged){
        // One line per highlighted add-on pod: its engine cables (binders).
        s->addonBinderLogged=true;const auto* pod=s->addonPod();
        const bool drawn=s->addonOverlay.binderPackets&&!s->addonOverlay.binderRefused;
        std::fprintf(stderr,"dlc_binder screen=rider key=%s status=%s packets=%u%s%s%s\n",pod?pod->key.c_str():"-",
            drawn?"drawn":"none",s->addonOverlay.binderPackets,drawn?"":" reason=\"",
            drawn?"":(s->addonOverlay.binderRefused?s->addonOverlay.binderRefused:"no packet"),drawn?"":"\"");
    }
    if(s->addonSlot&&!s->addonModelLogged){
        // One line per highlighted add-on model (first rendered frame).
        s->addonModelLogged=true;const auto* pod=s->addonPod();const auto* row=s->addonModelRow(s->addonSlot);
        const bool drawn=s->addonOverlay.modelPackets&&!s->addonOverlay.modelRefused;
        std::fprintf(stderr,"dlc_rider_model key=%s number=%u model=%s status=%s packets=%u%s%s%s\n",
            pod?pod->key.c_str():"-",pod?pod->number:0u,pod?pod->ui.riderModel.c_str():"-",drawn?"drawn":"refused",
            s->addonOverlay.modelPackets,drawn?"":" reason=\"",drawn?"":(s->addonOverlay.modelRefused?s->addonOverlay.modelRefused:
                (row?"no packet":"no ready add-on model row")),drawn?"":"\"");
    }
    if(s->addonSlot&&!s->addonSkipLogged&&(s->addonOverlay.drawnLines||s->addonOverlay.skippedLines)){
        s->addonSkipLogged=true;const auto* pod=s->addonPod();
        std::fprintf(stderr,"dlc_rider_draw key=%s number=%u name_lines_drawn=%u name_lines_skipped=%u huttese=\"%s\" package=\"%s\" stats=pod_phy model=pod portrait=own background=own character=own\n",
            pod?pod->key.c_str():"-",pod?pod->number:0u,s->addonOverlay.drawnLines,s->addonOverlay.skippedLines,
            pod?pod->ui.hutteseName.c_str():"-",pod?pod->packageName.c_str():"-");
    }
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
    if(!gpu)return result(d,Status::invalidInput);
    if(s->nativePool){
        const auto& demand=s->nativeDemand;
        if(!s->nativePrepared||!gpu->riderSparse()||gpu->riderSession()!=this||gpu->riderBank()!=bank().get()||
           gpu->riderDemandSerial()!=demand.serial_)return result(d,Status::invalidInput);
        for(std::uint32_t i=0;i<gpu->textureCount();++i)
            if(bool(gpu->asset(i))!=demand.includes(i))return result(d,Status::invalidInput);
        if(demand.entry_){
            if(!residencyProposalCurrent(demand))return result(d,Status::unavailableDestination);
        }else if(!demand.consumed_||!s->widgets().matchesNativeProjection(s->nativePost))return result(d,Status::invalidInput);
    }else if(gpu->riderSparse())return result(d,Status::invalidInput);
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
    out.resetPopupPending=s->resetPending;
    out.transition=s->transition();out.aperture=s->aperture.snapshot();out.bankGeneration=s->backend.generation();
    std::uint16_t character=0;
    out.selectedCharacterReady=out.selectionAttached&&s->selectedConfirmCharacter(character)&&
        (s->addonSlot?(s->addonCharacter()&&s->addonCharacter()->sourceReadiness()):s->dynamic.sourceCharacterReadiness(character));
    out.registryGeneration=s->backend.registryGeneration();out.framePublished=sourceFrame()!=nullptr;return out;
}
void OriginalUiRiderSession50KG::drain() noexcept {if(storage_)storage_->cpuDrain();}
void OriginalUiRiderSession50KG::requestQuit() noexcept {
    if(!storage_)return;storage_->cpuDrain();storage_->state.backProposalPending=false;storage_->state.confirmProposalPending=false;storage_->state.quitRequested=true;
}
bool OriginalUiRiderSession50KG::sourceTeardownPending()const noexcept {
    return storage_&&!storage_->sourceTornDown&&storage_->backendBound&&
        (storage_->collector.sourceTeardownPending()||storage_->collector.hasSourceSubmission()||storage_->backend.materialLifetime().hasOwners()||
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
    if(s.collector.sourceTeardownPending()||s.collector.hasSourceSubmission()){
        if(!renderer||!s.collector.retireSourceRenderer(*renderer,rendererDiagnostic,d.draw))
            return result(d,Status::backend,d.draw.sourcePC);
    }
    s.collector.drain();s.state.framePublished=false;
    if(!s.collector.releasePublishedBank(d.draw))return result(d,Status::draw,d.draw.sourcePC);
    if(!s.backend.acceptSourceRetirement(&s.collector,generation,frontier))return result(d,Status::backend);
    if(!s.dynamic.drain()){d.dynamic=s.dynamic.diagnostic();d.backend=s.backend.diagnostic();return result(d,Status::model);}
    s.state.selectionAttached=false; // dynamic cleanup completed even if a later consumer must retry
    if(!s.collector.consumeMaterials(false,d.draw))return result(d,Status::draw,d.draw.sourcePC);
    if(!s.backend.finalizeTeardown(s.collector.materialSequence(),d.backend))return result(d,Status::backend);
    s.sourceTornDown=true;s.dependenciesPrepared=false;s.state.selectionAttached=false;
    if(s.addonCarousel)game::dlc50DL::setHighlightedAddonRider(s.addonPlayer,0); // no Rider page, no highlight
    s.addonModels.drain(); // add-on graphs hold no lifetime rows; the renderer is retired above
    return result(d,Status::ok);
}

} // namespace rr::platform

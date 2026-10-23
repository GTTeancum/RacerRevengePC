#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_rider_profile50kg.h"
#include "original_ui_main_audio50kg.h"
#include "original_application_aural50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <limits>
namespace rr::platform {
bool OriginalUiGarageSession50KG::canRequestPage1D6130(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    return issuer_&&issuer_->garageCustodyOwner_.get()==this&&published_&&!failed_&&
        !retiring_&&!closed_&&page_.get()==&page&&page.issuer_==issuer_&&
        grant_&&grant_==page.grant_&&!grant_.owner_before(page.grant_)&&
        !page.grant_.owner_before(grant_)&&grant_->current()&&scope_&&scope_->live().okay()&&
        scope_->issuer_==issuer_&&scope_->dispatcherLive_&&scope_->managerLive_&&
        scope_->managerPublished_&&managerPublished1B8274_&&managerUpdateFieldsConstructed_&&
        scope_->dispatcherCurrentManager30_==&scope_->manager_&&
        scope_->dispatcherCurrentManagerGeneration30_==entryManagerGeneration_&&
        managerDispatcher2AC_==&scope_->dispatcher_&&
        managerPage144Known_&&managerPage0Stored_&&manager150Known_&&
        managerPageTable144_&&managerPageCount148_==1&&managerSelected150_==1&&
        managerPageTable144_[0]==&page.entity_&&managerCurrentPage14C_==&page.entity_&&
        page.sourceEntryReturned1FAE00();
}
bool OriginalUiGarageSession50KG::requestPage1D6130(
    const OriginalUiGaragePageOwner50KG& page,bool& written)noexcept {
    written=false;
    if(!canRequestPage1D6130(page)||
       page.inputPhase_!=OriginalUiGaragePageOwner50KG::InputPhase::request||
       !page.inputSerial_||page.inputController_||
       !((page.inputSource_==4u&&page.inputPlan_.requestCallPC==0x1FE0F4u)||
         (page.inputSource_==6u&&page.inputPlan_.requestCallPC==0x1FE158u)))return false;
    // The source virtualA0 call occurs even in this case; it simply performs
    // no request writes. Its caller still invokes Watto, effect and267BE0.
    if(managerTransition170_==2u)return true;
    // Do not read an unknown word as a C++ integer. Copy its unknown source
    // provenance to the actual prior-request field, then publish the known
    // new word. This is NOT a copy of manager150/currentPage14C.
    scope_->dispatcherPreviousPage48_=managerPendingPage168_; //1CCF48
    managerPendingPage168_=1;                                //1CCF50
    scope_->dispatcherPreviousScreen44_=managerPendingScreen164_; //1CCF78
    managerPendingScreen164_=9;                              //1CCF80
    managerLifecycle16C_=6;                                  //1D6178
    written=true;return true;
}
bool OriginalUiGaragePageOwner50KG::inputGroupStepCurrent(std::int32_t direction)const noexcept {
    if((direction!=-1&&direction!=1)||!sourceEntryReturned1FAE00()||runtimeWidgetPending_||sharedHookPending_||
       groups_.size()<=2||!groups_[2]||!groupSlotsKnown_[2]||!groupTable_||
       groupTable_[2]!=groups_[2].get())return false;
    const auto& g=*groups_[2];
    if(!g.loaded||g.retirementStarted||g.payloadRetired||g.count34!=6||
       g.controlsKnown!=6||!g.controls38||!g.selection3C||*g.selection3C<1||
       *g.selection3C>6||!g.selected40||g.selected40!=g.controls38[*g.selection3C-1])return false;
    auto controlCurrent=[&](const PageControl* row,bool selecting)noexcept {
        if(!row)return false;
        bool owned=false;
        for(std::size_t n=0;n<controls_.size();++n)
            if(controls_[n].get()==row&&controlSlotsKnown_[n]&&controlTable_&&controlTable_[n]==row)owned=true;
        if(!owned||!row->loaded||row->retirementStarted||row->payloadRetired||row->count34<0||
           row->widgetsKnown!=static_cast<std::size_t>(row->count34)||
           (row->widgetsKnown&&!row->widgets38)||!widgets_||!widgets_->managerTable154_)return false;
        for(std::size_t n=0;n<row->widgetsKnown;++n){
            auto* widget=row->widgets38[n];if(!widget){if(selecting)return false;continue;}
            bool found=false;
            for(std::size_t k=0;k<widgets_->rows_.size();++k)
                if(widgets_->slotsKnown_[k]&&widgets_->managerTable154_[k]==widget)found=true;
            if(!found)return false;
        }
        return true;
    };
    if(!controlCurrent(g.selected40,false))return false;
    auto selection=*g.selection3C;
    for(std::size_t n=0;n<g.controlsKnown;++n){
        selection+=direction;if(selection<1)selection=g.count34;else if(selection>g.count34)selection=1;
        const auto* row=g.controls38[selection-1];if(!row)continue;
        // The real helpers skip missing/disabled rows. They do not infer
        // their result from Page4E4, and must have a terminating eligible row.
        if(!controlCurrent(row,false))return false;
        if(row->sectionPresent30&&row->field3C==0)return controlCurrent(row,true);
    }
    return false;
}
bool OriginalUiGaragePageOwner50KG::inputGroupStep2A50F0(
    std::int32_t direction,Diagnostic& d)noexcept {
    const auto pc=direction<0?0x2A51B0u:0x2A50F0u;
    if(inputPhase_!=InputPhase::group||!inputSerial_||!inputGroupStepCurrent(direction))
        return fail(d,Status::runtimePending,pc);
    auto& g=*groups_[2];
    //2A5110/2A51D0: unselect ONLY the actual old row, BEFORE3C/40 writes.
    if(!runtimeControlCall(g.selected40,0x7C,0,d))return false;
    for(std::size_t n=0;n<g.controlsKnown;++n){
        *g.selection3C+=direction;
        if(*g.selection3C<1)g.selection3C=g.count34;else if(*g.selection3C>g.count34)g.selection3C=1;
        g.selected40=g.controls38[*g.selection3C-1];d.sourceEffectsCommitted=true;
        if(!g.selected40||!g.selected40->sectionPresent30||g.selected40->field3C)continue;
        // Actual Page caller passes a1=0, so select the new row rather than
        // returning an unselected row pointer through an invented output.
        return runtimeControlCall(g.selected40,0x78,0,d);
    }
    return fail(d,Status::runtimePending,pc);
}
bool OriginalUiGaragePageOwner50KG::dispatchInput1FDE70(
    std::uint32_t sourceInput,std::uint32_t controller,InputOutcome& out,Diagnostic& d)noexcept {
    out={};d={};
    auto unavailable=[&](std::uint32_t pc)noexcept {
        out.sourcePC=pc;return fail(d,Status::runtimePending,pc);
    };
    if(inputPhase_==InputPhase::failed){
        out.status=InputOutcome::Status::failed;out.serial=inputSerial_;
        out.sourceEffectsCommitted=inputEffectsCommitted_;out.sourcePC=inputFailurePC_;
        d.sourceEffectsCommitted=inputEffectsCommitted_;return unavailable(inputFailurePC_);
    }
    if((inputPhase_!=InputPhase::idle&&inputPhase_!=InputPhase::returned)||
       inputSerial_==std::numeric_limits<std::uint64_t>::max()||!issuer_||
       !sourceEntryReturned1FAE00()||!entryProfileCurrent()||!base_.input268||
       runtimeWidgetPending_||sharedHookPending_||!information4DC_||!issuer_->state||!issuer_->state->known||
       !issuer_->state->sharedDialogs||!issuer_->state->sharedDialogs->loadedCurrent())
        return unavailable(0x1FDE70);
    const auto session=issuer_->garageCustodyOwner_;
    const auto& dialogs=*issuer_->state->sharedDialogs;
    if(!session||session->page_.get()!=this||session->issuer_!=issuer_||
       dialogs.application_.lock()!=issuer_->state||dialogs.scope_!=session->scope_||
       !session->canRequestPage1D6130(*this))return unavailable(0x1FDEC4);
    game::OriginalUiGarageInputContext50KG context;
    context.controller=controller;context.event=sourceInput;
    context.managerState170=session->managerTransition170_;
    context.dialogChoice3D8=issuer_->state->controllerPopupState.result3D8;
    context.informationOverlay4DC=*information4DC_;
    game::OriginalUiGarageState50KG state;
    // Modal testing and source controller/no-event rejection precede numeric
    // state reads. A fresh temporary popup state never confers this branch.
    auto plan=game::planOriginalUiGarageFreshInput50KG(state,context);
    if(plan.status==game::OriginalUiGarageInputStatus50KG::unavailableModal)
        return unavailable(plan.branchPC);
    if(plan.status==game::OriginalUiGarageInputStatus50KG::ignored){
        ++inputSerial_;inputSource_=sourceInput;inputController_=controller;inputPlan_=plan;
        inputPhase_=InputPhase::returned;inputEffectsCommitted_=false;inputRequestWritten_=false;
        inputTransportBoundary_=0;inputWattoReceipt_={};inputWattoDiagnostic_={};
        out.status=InputOutcome::Status::ignored;out.serial=inputSerial_;out.sourcePC=0x1FE7AC;return true;
    }
    if(plan.status==game::OriginalUiGarageInputStatus50KG::proposed&&
       plan.intent==game::OriginalUiGarageInputIntent50KG::shortenClosingWait){
        if(context.dialogChoice3D8||!context.informationOverlay4DC||controller||
           (sourceInput!=4&&sourceInput!=6)||!entryField4E0_)
            return unavailable(0x1FE788);
        // The real1FE74C suffix never replays Back/Accept's route, Watto or
        // audio calls. Publish this invocation, then perform its two stores.
        ++inputSerial_;inputSource_=sourceInput;inputController_=controller;inputPlan_=plan;
        inputPhase_=InputPhase::stateWrite;inputEffectsCommitted_=false;inputRequestWritten_=false;
        inputTransportBoundary_=0;inputWattoReceipt_={};inputWattoDiagnostic_={};
        timer34B4_[4]=0x3EB33333u; //1FE788: exact source34C4 binary32 bits.
        entryField4E0_=1;          //1FE790: bypass remaining closing wait.
        inputEffectsCommitted_=true;inputPhase_=InputPhase::returned;
        out.status=InputOutcome::Status::completed;out.serial=inputSerial_;
        out.sourceEffectsCommitted=true;out.sourcePC=0x1FE7AC;
        d.sourceEffectsCommitted=true;d.sourcePC=0x1FE7AC;return true;
    }
    OriginalUiRiderProfileOwner50KG::GarageProfileSnapshot profile;
    if(!entryProfile_||!entryProfile_->owner->selectedGarageProfileSnapshot(profile)||
       !available34B0_)return unavailable(0x1FDF18);
    state.selected4E4=field4E4_;state.available34B0=*available34B0_;
    for(std::size_t row=0;row<6;++row){
        if(!current4E8_[row]||!delta500_[row]||
           !game::convertOriginalUiGarageStat50KG(profile.base0180[row],state.base[row])||
           !game::convertOriginalUiGarageStat50KG(profile.maximum0408[row],state.maximum[row]))
            return unavailable(0x1FDF18);
        state.current4E8[row]=*current4E8_[row];state.delta500[row]=*delta500_[row];
    }
    state.known=true;plan=game::planOriginalUiGarageFreshInput50KG(state,context);
    if(plan.status!=game::OriginalUiGarageInputStatus50KG::proposed)return unavailable(plan.branchPC);
    if(!session->audio_||!issuer_->sourceAural00Owner_)return unavailable(plan.soundPC);
    auto& main=*session->audio_;auto& aural=*issuer_->sourceAural00Owner_;
    OriginalApplicationAural50KG::Diagnostic audioDiagnostic;
    if(!main.canPlayGaragePageEffect1FDE70(*this,plan.sound)||
       !aural.canFinishGaragePageInput267BE0(*this,audioDiagnostic))return unavailable(plan.soundPC);
    const auto direction=plan.intent==game::OriginalUiGarageInputIntent50KG::previousStat?-1:1;
    if(plan.selectionGroupCallPC&&!inputGroupStepCurrent(direction))return unavailable(plan.selectionGroupCallPC);
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG wattoDiagnostic;
    if(plan.hasWattoAction&&(!watto33B0_||
       !watto33B0_->canRespondPage1D5320(*this,plan.wattoAction,wattoDiagnostic)))
        return unavailable(wattoDiagnostic.sourcePC?wattoDiagnostic.sourcePC:plan.wattoPC);
    // Publish one non-replayable invocation only after read-only owner checks.
    ++inputSerial_;inputSource_=sourceInput;inputController_=controller;inputPlan_=plan;
    inputEffectsCommitted_=false;inputRequestWritten_=false;inputTransportBoundary_=0;
    inputWattoReceipt_={};inputWattoDiagnostic_={};inputPhase_=InputPhase::planned;
    auto failed=[&](std::uint32_t pc)noexcept {
        inputEffectsCommitted_=inputEffectsCommitted_||d.sourceEffectsCommitted||inputWattoReceipt_.effectsStarted;
        inputPhase_=InputPhase::failed;inputFailurePC_=pc;
        out.status=InputOutcome::Status::failed;out.serial=inputSerial_;
        out.requestWritten=inputRequestWritten_;if(inputRequestWritten_)out.request=inputPlan_.request;
        out.sourceEffectsCommitted=inputEffectsCommitted_;out.sourcePC=pc;
        d.sourceEffectsCommitted=inputEffectsCommitted_;return fail(d,Status::runtimePending,pc);
    };
    if(plan.stateWritePC){
        inputPhase_=InputPhase::stateWrite;
        if(plan.intent==game::OriginalUiGarageInputIntent50KG::previousStat){
            --field4E4_; //1FE1B4, unsigned arithmetic defines source word wrap.
            if(field4E4_==std::numeric_limits<std::uint32_t>::max())field4E4_=5; //1FE1C8
        }else if(plan.intent==game::OriginalUiGarageInputIntent50KG::nextStat){
            ++field4E4_; //1FE224
            if(field4E4_==6u)field4E4_=0; //1FE238
        }else delta500_[field4E4_]=plan.next.delta500[field4E4_];
        inputEffectsCommitted_=true;d.sourceEffectsCommitted=true;
    }
    if(plan.selectionGroupCallPC){
        inputPhase_=InputPhase::group;
        if(!inputGroupStep2A50F0(direction,d))return failed(d.sourcePC);
    }
    if(plan.requestCallPC){
        inputPhase_=InputPhase::request;
        if(!session->requestPage1D6130(*this,inputRequestWritten_))return failed(plan.requestCallPC);
        inputEffectsCommitted_=inputEffectsCommitted_||inputRequestWritten_;
    }
    if(plan.hasWattoAction){
        inputPhase_=InputPhase::watto;
        if(!watto33B0_->respondPage1D5320(*this,plan.wattoAction,inputWattoReceipt_,inputWattoDiagnostic_))
            return failed(inputWattoDiagnostic_.sourcePC?inputWattoDiagnostic_.sourcePC:plan.wattoPC);
        inputEffectsCommitted_=inputEffectsCommitted_||inputWattoReceipt_.effectsStarted;
    }
    inputPhase_=InputPhase::effect;const auto revision=main.controlRevision_;
    const bool played=main.playGaragePageEffect1FDE70(*this,plan.sound);
    inputEffectsCommitted_=inputEffectsCommitted_||revision!=main.controlRevision_;
    if(!played)return failed(plan.soundPC);
    inputPhase_=InputPhase::transport;
    if(!aural.finishGaragePageInput267BE0(*this,inputTransportBoundary_,audioDiagnostic))
        return failed(audioDiagnostic.sourcePC?audioDiagnostic.sourcePC:plan.transportPC);
    inputEffectsCommitted_=true;inputPhase_=InputPhase::returned;
    out.status=InputOutcome::Status::completed;out.serial=inputSerial_;
    out.transportBoundary=inputTransportBoundary_;out.requestWritten=inputRequestWritten_;
    if(inputRequestWritten_)out.request=inputPlan_.request;
    out.sourceEffectsCommitted=true;out.sourcePC=0x1FE7AC;
    d.sourceEffectsCommitted=true;d.sourcePC=0x1FE7AC;return true;
}
} // namespace rr::platform

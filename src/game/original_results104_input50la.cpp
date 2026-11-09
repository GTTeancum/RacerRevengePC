#include "original_results104_storage50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_position_stats_adapter50la.h"
#include "../../tests/native/original_camera_cycle_audio50kx.h"
#include "../../tests/native/original_race_hud_scope50la.h"
#include "../../tests/native/race_music50ie_fixture.h"
#include "../../platform/linux/native/game_host50er.h"
#include "../../platform/linux/native/original_save_profile_owner50kg.h"
#include "../assets/original_application_localization50kg.h"
#include <bit>
#include <cmath>
#include <thread>

namespace rr::game {
bool OriginalRaceConstructor50LA::storeResultsNoTimeAndReconcile1CA4DC(bool& committed)noexcept {
    committed=false;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!profile_||!counted_||rolledBack_)
        return false;
    const auto before=profile_->stamp();
    if(endEntryProfile_&&(endEntryProfile_->generation!=before.generation||
       endEntryProfile_->mode34!=before.mode34))return false;
    platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection a,b;
    if(!profile_->courseRaceSnapshot(a)||
       (endEntryProfile_&&a.subtype!=endEntrySubtype_))return false;
    // This private synchronous store changes ONLY actual11054 bit0 and the
    // native generation, with no callback or name/record/dirty/save mutation.
    if(!profile_->storeResultsNoTime1CA4DC(before))return false;
    committed=true;
    const auto after=profile_->stamp();
    if(!profile_->courseRaceSnapshot(b)||after.generation!=before.generation+1u||
       after.mode34!=before.mode34||a.course!=b.course||a.planet!=b.planet||
       a.rider!=b.rider||a.subtype!=b.subtype||a.laps!=b.laps||
       a.entrants!=b.entrants||a.packed1125C!=b.packed1125C||a.mode34!=b.mode34)
        return false; // Retain the write and old stamp; never replay it.
    if(endEntryProfile_)endEntryProfile_=after;
    return true;
}
bool OriginalRaceConstructor50LA::storeResultsNameAndReconcile1CA528(std::uint32_t mode,
    std::string_view name,bool& committed)noexcept {
    committed=false;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!profile_||!counted_||rolledBack_)
        return false;
    const auto before=profile_->stamp();
    if(endEntryProfile_&&(endEntryProfile_->generation!=before.generation||
       endEntryProfile_->mode34!=before.mode34))return false;
    platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection a,b;
    if(!profile_->courseRaceSnapshot(a)||
       (endEntryProfile_&&a.subtype!=endEntrySubtype_))return false;
    // Private synchronous method changes ONLY selected name bytes and its
    // native generation. There is no callback, save, result award, reset or
    // other Profile write between these observations.
    if(!profile_->storeResultsName1CA528(mode,name,before))return false;
    committed=true;
    const auto after=profile_->stamp();
    if(!profile_->courseRaceSnapshot(b)||after.generation!=before.generation+1u||
       after.mode34!=before.mode34||a.course!=b.course||a.planet!=b.planet||
       a.rider!=b.rider||a.subtype!=b.subtype||a.laps!=b.laps||
       a.entrants!=b.entrants||a.packed1125C!=b.packed1125C||a.mode34!=b.mode34)
        return false; // Retain mutation and old stamp; caller stops, never replays.
    // No suspended end invocation is fabricated when none existed. Only the
    // authenticated immediate descendant replaces an already matching stamp.
    if(endEntryProfile_)endEntryProfile_=after;
    return true;
}
bool OriginalResults10450LA::pressHostInput1C8C30(const ::original_main_entry50KG::Application& application,
    OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    ::original_camera_cycle_audio50KX::Owner& audio,platform::GameHost50ER& host,
    OriginalResults104Diagnostic50LA& d)noexcept {
    d={};
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c8c30;return false;
    }
    if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1c8c30;return false;}
    auto& p=*impl_;auto& input=host.resultsInput50LA_;
    const auto resumedPC=application.resultsBlockingInputReturnPC50LA(*this);
    const bool resumed=resumedPC!=0u;
    const auto resumeDeactivationPC=resumedPC==0x1c8f5c?0x1c8f64u:
        resumedPC==0x1c98bc?0x1c98c4u:resumedPC==0x1ca678?0x1ca688u:0x1ca788u;
    if(p.nativeFrameBorrows||!constructed()||!p.loadComplete||!p.base||!p.derived||!retirementCurrent()||
       !endDispatchOwnerCurrent1AB520(race,observer)||!observer.camera_||!observer.identityCurrent()||
       !host.ownerThread()||!host.created()||!host.hasWindow()||!input.fixedUiCurrent()||
       (resumed?!host.retainedResultsPollCurrent50LA(*this):
         (!input.edgesOpen_||!input.revision_||input.revision_<=p.lastInputAttemptPoll)))
        return p.stop(d,Impl::S::owner,0x1c8c30,"Results actual fresh Window poll/Camera/Bank invocation unavailable");
    // Claim this exact native poll before the first source read. No frame can
    // replay it after partial effects; normal clients retire unclaimed edges.
    if(!resumed){
        // Only this admitted active issuer may extend the producer's actual
        // SAME poll. Application/Loop predecessor/modal gates already passed;
        // all concrete Camera/Bank/Results fences above precede this append.
        if((p.component.flags29&13u)!=1u||!host.appendResultsHideSamePoll50LA(*this))
            return p.stop(d,Impl::S::owner,0x1c8c30,"Results same-poll B issuer changed or already consumed");
        input.edgesOpen_=false;input.claimedRevision_=input.revision_;p.lastInputAttemptPoll=input.revision_;
        host.claimedResultsOwner50LA_=this;host.claimedResultsEvent50LA_=nullptr;
    }
    if(input.hideIssuer_!=this)
        return p.stop(d,Impl::S::owner,0x1c8c30,"Results extended poll issuer changed");
    if(input.hideAppend_==platform::OriginalResultsHostInput50LA::HideAppend::capacity||
       input.hideAppend_==platform::OriginalResultsHostInput50LA::HideAppend::invalid)
        return p.stop(d,Impl::S::source,0x128080,
            "Results whole same-poll batch rejected before callbacks; ten-event bound or B observation invalid");
    //A source-partial prior invocation cannot restart. Its actually active UI
    //still owns these controls: do not leak a later Confirm into host Results.
    if(!resumed&&p.inputCpu&&p.inputCpu->blocked)
        return p.stop(d,Impl::S::incomplete,p.inputCpu->nextSourcePC,
            "Results source-partial input remains stopped; native UI edge retired without replay");
    if(resumed&&(!p.inputCpu||!p.inputCpu->blocked||p.inputCpu->nextSourcePC!=resumedPC||
       p.inputCpu->pollRevision!=input.revision_))
        return p.stop(d,Impl::S::owner,resumeDeactivationPC,
            "Results retained source input cursor changed");
    if(!resumed)p.inputCpu.emplace();
    auto& out=*p.inputCpu;out.pollRevision=input.revision_;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* why){
        out.nextSourcePC=pc;out.blocked=out.sourceEffectsCommitted;return p.stop(d,status,pc,why);
    };
    const auto changed=[&](){p.displayCpu.reset();p.nativeDisplay.reset();out.sourceEffectsCommitted=true;};
    const auto feedback=[&](std::uint32_t key,std::uint32_t pc){
        // Actual SAME Camera's race10C+160 emitter. Its explicit Results
        // preparation owns authored keys1..6; other preparation policies
        // still reject unprepared keys. Muted/unresolved is not PCM success.
        out.nextSourcePC=pc;
        const auto r=audio.dispatchResultsFeedback(key);
        using A=::original_camera_cycle_audio50KX::Owner::Result;
        if(r!=A::started&&r!=A::unavailable&&r!=A::muted)
            return stop(Impl::S::owner,pc,"Results actual race UI feedback owner unavailable after reached stores");
        return true;
    };
    try {
        const auto first=resumed?host.retainedResultsPollEvent50LA_:0u;
        for(std::size_t eventIndex=first;eventIndex<input.count_;++eventIndex){
            auto& event=input.events_[eventIndex].event;
            host.claimedResultsEvent50LA_=&event;
            if(resumed)host.retainedResultsPollEvent50LA_=eventIndex;
            // Native transport supplies only fresh category2 presses. Record
            // bytes remain owned by that same poll; no ControllerActions queue
            // is acknowledged or device-held query synthesized here.
            if(event.category!=2u||event.transition!=1u||event.deviceId>1u)
                return stop(Impl::S::source,0x1c8c30,"Results native event domain changed");
            if(resumed&&eventIndex==first){
                // Return from the REAL completed260850 wait. Do not replay
                // this event's already-consumed predecessors/name/audio.
                //141520 clears bit0 then bit1, then141460 visits actual empty
                // Entity children. No detached-constructor helper/close.
                if(p.disabled||((resumedPC==0x1ca778||resumedPC==0x1ca678)&&!p.nameEntry)||p.component.firstChild18||p.component.descendants24)
                    return stop(Impl::S::owner,resumeDeactivationPC,
                        "Results attached deactivation owner changed");
                p.component.flags29=std::uint8_t(p.component.flags29&~1u);
                p.component.flags29=std::uint8_t(p.component.flags29&~2u);
                p.disabled=true;changed();
                if(resumedPC==0x1ca678u){
                    application.setExplicitAnimationPause(false); //1CA698 actual Application11C.
                    using Observation=::original_position_stats_adapter50LA::Owner;
                    Observation::EndObservation current;Observation::Diagnostic observed;
                    Observation::ResultsRecordIdentity row;
                    const auto* materials=p.domain->materials382B38().get();
                    OriginalRaceRecordRacerObservation50KG values;
                    if((race.flags1B0KnownMask_&3u)!=3u||(race.flags1B0KnownValue_&3u)!=1u||
                       !observer.captureEndObservation1AB520(p.application.get(),materials,
                            p.component.sessionGeneration,current,observed)||
                       !observer.captureResultsRecordIdentity(p.application.get(),materials,
                            p.component.sessionGeneration,0,row,observed)||row.publication!=current.publication||
                       !observer.readResultsBestLapCB4(row,values.bestLapCB4,observed)||
                       !observer.readResultsRaceTimeCB8(row,values.timeCB8,observed)||
                       !observer.readResultsObservedTimingCD0(row,values.countCD0,observed))
                        return stop(Impl::S::owner,0x1ac440,"Results Hall same single-human Bank operands unavailable");
                    std::optional<OriginalRaceRecordInsertionStage50KG> stage;
                    OriginalRaceRecordInsertionStage50KG::Diagnostic insertion;
                    std::unique_ptr<platform::OriginalSaveProfileOwner50KG> prepared;
                    if(!race.profile_->prepareSingleHumanRecordInsertion1AC440(values,1u,stage,insertion)||!stage||
                       !race.profile_->prepareHallRecordProvenance1AC440(*stage,prepared)||!prepared||
                       !observer.resultsRowsCurrent(p.application.get(),materials,p.component.sessionGeneration,current.publication)||
                       !application.commitResultsHallTransfer50LA(race,*stage,*prepared))
                        return stop(Impl::S::owner,0x1ca6a4,"Results Hall actual mutation/dirty/queued request incomplete");
                    // Special subtype3 has NO20C acknowledgement store.
                }else p.derived->acknowledgement20C=1u; //1C8F7C /1C98D8 /1CA794.
                if(resumedPC==0x1ca778||resumedPC==0x1ca678)p.nameEntry->fieldAD8=0u; //1CA798.
            }else{
            //Actual Root142350 reaches the earlier Race custom slot64 and
            //its concrete children BEFORE this event enters Results1C8C30.
            //No batch-wide preflight can reorder these reads after a prior
            //event's name edits. Unsupported predecessors stop this poll.
            OriginalResults104Diagnostic50LA predecessor;
            if(!application.passResultsProcessPredecessors50LA(race,&event,predecessor))
                return stop(predecessor.status,predecessor.sourcePC,predecessor.operation);
            if(!race.passResultsInputPredecessors1A87A0(observer,event,input,predecessor))
                return stop(predecessor.status,predecessor.sourcePC,predecessor.operation);
            // Next actual142350 iteration sees deactivation. Earlier Root/
            // Race callbacks above still ran; this Results callback does not.
            if(!(p.component.flags29&1u)||(p.component.flags29&8u)){
                if(!application.passResultsProcessSuccessors50LA(race))
                    return stop(Impl::S::owner,0x142350,"Results queued request event successor changed");
                ++out.eventsCompleted;continue;
            }
            if(!p.mode1F0)return stop(Impl::S::incomplete,0x1c8c50,"Results1F0 has no reached base-update producer");
            bool filtered=*p.mode1F0!=0u;
            if(!filtered){
                if((race.flags1B0KnownMask_&3u)!=3u)
                    return stop(Impl::S::incomplete,0x1c8c70,"Results canonical Race1B0 low2 producer not reached");
                if((race.flags1B0KnownValue_&3u)==2u){
                    if(p.derived->mode200==3u&&event.deviceId!=0u)filtered=true;
                    if(p.derived->mode200==4u&&event.deviceId!=1u)filtered=true;
                }else if(event.deviceId!=0u)filtered=true;
            }
            //1C8CDC filtered return never reads390/ADC;1C8D0C similarly
            //skips ADC unless390 passed. Preserve reached-read chronology.
            //Every early return is the ACTUAL empty-child142350 case.
            if(filtered){++out.eventsCompleted;continue;}
            const float transition=std::bit_cast<float>(p.derived->field390);
            if(!(transition<=0.f)){++out.eventsCompleted;continue;}
            const float cheat=std::bit_cast<float>(p.derived->fieldADC);
            if(!(cheat<=0.f)){++out.eventsCompleted;continue;}
            bool visiblePage=false;
            //1C8D28 hidden!=0 branches to1C98DC WITHOUT reading164.
            if(!p.base->hidden160){
                const float opening=p.base->transition164;
                if(!std::isfinite(opening))
                    return stop(Impl::S::source,0x1c8d34,"Results shared transition native numeric domain");
                visiblePage=opening<0.f;
            }
            if(visiblePage){
                const auto mode=p.derived->mode200;
                if(mode==1u){
                    std::uint8_t flags=0;OriginalResults104Diagnostic50LA managerRead;
                    if(!race.readResultsManagerFlags58(*observer.camera_,flags,managerRead))
                        return stop(managerRead.status,0x1c8d68,managerRead.operation);
                    if(flags&1u){
                        bool action=false;
                        if(!input.matchesNativeCategory2Code1277D0(3,event,action))
                            return stop(Impl::S::owner,0x1c8d84,"Results source action3 mapping unavailable");
                        if(action)return stop(Impl::S::incomplete,0x1c8d94,
                            "Results action3 Profile/restart1AC8E0 route unavailable");
                        if(!input.matchesNativeCategory2Code1277D0(2,event,action))
                            return stop(Impl::S::owner,0x1c8e20,"Results source action2 mapping unavailable");
                        if(action){
                            // Source2E0 short circuit BEFORE Profile34/7E4.
                            if(!application.sourceUseProfile2E0_)
                                return stop(Impl::S::incomplete,0x1c8e38,"Actual Application2E0 producer absent");
                            if(*application.sourceUseProfile2E0_){
                                if(!race.profile_)return stop(Impl::S::owner,0x1c8e44,"Results actual Profile absent");
                                if(race.profile_->stamp().mode34==2u){
                                    OriginalResults104Diagnostic50LA modeDiagnostic;
                                    const auto prefix=setModeCpuPrefix1CA8E0(race,observer,2u,1u,modeDiagnostic);
                                    if(prefix!=ModeCpuPrefix::none)changed();
                                    if(prefix!=ModeCpuPrefix::humanFeedbackTail&&prefix!=ModeCpuPrefix::zeroHumanTail)
                                        return stop(modeDiagnostic.status,modeDiagnostic.sourcePC,
                                            "Results actual mode2/device-feedback prefix incomplete; never replay");
                                    if(!feedback(2,0x1c8e70))return false;
                                }else{
                                bool record=false;
                                if(!race.profile_->resultsRecordBit7E4(record))
                                    return stop(Impl::S::owner,0x1c8e80,"Actual Profile7E4 bit0 unavailable");
                                if(record){
                                    using Query=OriginalRaceRecordQualification50KG;
                                    using Observation=::original_position_stats_adapter50LA::Owner;
                                    std::array<OriginalRaceRecordQualificationRacerObservation50KG,2> rows{};
                                    std::size_t available=0;std::optional<Query> query;Query::Diagnostic qd;
                                    const auto stamp=race.profile_->stamp();
                                    const auto* materials=p.domain->materials382B38().get();
                                    Observation::EndObservation current;Observation::Diagnostic observed;
                                    if(!observer.captureEndObservation1AB520(p.application.get(),materials,
                                        p.component.sessionGeneration,current,observed))
                                        return stop(Impl::S::owner,0x1c8e94,"Results selector1 actual observation unavailable");
                                    const std::optional<std::uint8_t> low2=(race.flags1B0KnownMask_&3u)==3u?
                                        std::optional<std::uint8_t>{std::uint8_t(race.flags1B0KnownValue_&3u)}:std::nullopt;
                                    // Ask the original selector BEFORE fetching operands.
                                    // Actual CD4 retirement precedes timing/CD0 reads;
                                    // missing operands never become zero or earned credit.
                                    while(!race.profile_->prepareRaceRecordQualification1ABFC0(1,low2,
                                        rows.data(),available,query,qd)){
                                        if(qd.status!=Query::Status::missingRacer||qd.managerIndex!=available||available>=rows.size())
                                            return stop(Impl::S::incomplete,qd.sourcePC,"Results selector1 source operand unavailable");
                                        Observation::ResultsRecordIdentity row;auto& value=rows[available];
                                        if(!observer.captureResultsRecordIdentity(p.application.get(),materials,
                                            p.component.sessionGeneration,available,row,observed)||row.publication!=current.publication||
                                           !observer.readEndRetirementCD4(row,value.retirementCD4,observed))
                                            return stop(Impl::S::owner,qd.sourcePC,"Results selector1 actual row owner changed");
                                        if(!value.retirementCD4&&
                                           (!observer.readResultsBestLapCB4(row,value.records.bestLapCB4,observed)||
                                            !observer.readResultsRaceTimeCB8(row,value.records.timeCB8,observed)||
                                            !observer.readResultsObservedTimingCD0(row,value.records.countCD0,observed)))
                                            return stop(Impl::S::owner,qd.sourcePC,"Results selector1 actual timing unavailable");
                                        ++available;
                                    }
                                    const auto now=race.profile_->stamp();
                                    if(!query||now.generation!=stamp.generation||now.mode34!=stamp.mode34||
                                       !observer.resultsRowsCurrent(p.application.get(),materials,p.component.sessionGeneration,current.publication))
                                        return stop(Impl::S::owner,0x1c8e9c,"Results selector1 same publication changed");
                                    if(!query->originalV0())return stop(Impl::S::incomplete,0x1c8ec0,
                                        "Results selector1 false requires unsupported actual second-human mode4 route");
                                    OriginalResults104Diagnostic50LA modeDiagnostic;
                                    const auto modePrefix=setModeCpuPrefix1CA8E0(race,observer,3u,1u,modeDiagnostic);
                                    if(modePrefix!=ModeCpuPrefix::none)changed();
                                    if(modePrefix!=ModeCpuPrefix::humanFeedbackTail&&modePrefix!=ModeCpuPrefix::zeroHumanTail)
                                        return stop(modeDiagnostic.status,modeDiagnostic.sourcePC,
                                            "Results actual mode3 scalar/device-feedback prefix incomplete; never replay");
                                    if(!feedback(2,0x1c8ed8))return false;
                                }else{
                                const auto aural=application.sourceAural00Owner_;
                                const auto scope=race.endHud_.lock();
                                platform::OriginalApplicationAural50KG::Diagnostic ad;
                                if(!aural||!race.raceAudio_||!scope||!scope->music_||!scope->announcer_||
                                   !scope->gameplay_||!scope->engines_||!scope->endStateCurrent(race,*observer.camera_)||
                                   (race.music10C_?!race.music10C_->nativeBackendCurrent(*scope):(race.world74_!=nullptr||!scope->music_))||
                                   !aural->stopResultsNative26D070(application,*race.raceAudio_,*scope->music_,audio,
                                       *scope->announcer_,*scope->gameplay_,*scope->engines_,scope->countdown_,
                                       race.entity_.sessionGeneration,ad))
                                    return stop(Impl::S::owner,0x1c8ef0,"Results summary actual native stop unavailable");
                                changed();
                                // Summary differs from name DONE: fade0 FIRST,
                                // then feedback2. Do not borrow .5/order/AD8.
                                if(!scope->endStateCurrent(race,*observer.camera_)||
                                   (race.music10C_?!race.music10C_->nativeBackendCurrent(*scope):(race.world74_!=nullptr||!scope->music_))||
                                   !scope->music_->requestFade1A4AB0(0.f).completed())
                                    return stop(Impl::S::incomplete,0x1c8f08,"Results summary actual Music fade0 incomplete");
                                if(!feedback(2,0x1c8f20))return false;
                                if(!scope->endStateCurrent(race,*observer.camera_)||
                                   (race.music10C_?!race.music10C_->nativeBackendCurrent(*scope):(race.world74_!=nullptr||!scope->music_))||
                                   !aural->finishResultsNative267BE0(application,*race.raceAudio_,ad))
                                    return stop(Impl::S::owner,0x1c8f38,"Results summary actual control finish unavailable");
                                if(!application.suspendResultsBlockingInput50LA(*this,race,*observer.camera_,host,
                                    ::original_main_entry50KG::Application::ResultsBlockingInputOrigin50LA::summary))
                                    return stop(Impl::S::owner,0x1c8f5c,"Results summary retained fade handoff unavailable");
                                return stop(Impl::S::incomplete,0x1c8f5c,"Results summary original event retained across260850");
                                }
                                }
                            }
                        }
                    }
                }
                if((mode==3u&&event.deviceId==0u)||(mode==4u&&event.deviceId==1u)){
                    if(!p.editA9C||!p.nameEntry||!p.nameEntry->fieldABC)
                        return stop(Impl::S::incomplete,0x1c8fb8,"Results actual name-entry buffer/selection initializer absent");
                    auto& fields=*p.nameEntry;
                    if(p.editA9C->find('\0')!=std::string::npos)
                        return stop(Impl::S::source,0x1c8fb8,"Results edit buffer native CString domain");
                    const auto length=p.editA9C->size(); //3093A8 reads EDIT buffer here.
                    fields.fieldAC0=*fields.fieldABC;changed(); //1C8FD4 before action query.
                    const auto selected=std::int32_t(*fields.fieldABC);
                    if(selected<1||selected>42)
                        return stop(Impl::S::source,0x1c8fe8,"Results keyboard selection outside recovered1..42 domain");
                    const auto remainder=[](std::int32_t value){const auto r=value%13;return r?r:13;};
                    const auto remembered=[&](std::uint32_t pc,std::int32_t fallback,std::int32_t add,std::int32_t& value){
                        if(!fields.fieldAC4)return stop(Impl::S::incomplete,pc,"Results remembered keyboard columnAC4 unknown");
                        const auto prior=*fields.fieldAC4;
                        if(prior< -1||prior>13||prior==0)
                            return stop(Impl::S::source,pc,"Results remembered keyboard column native domain");
                        value=prior==-1?fallback:prior+add;return true;
                    };
                    std::int32_t next=selected;bool moved=false;
                    std::uint32_t audioPC=0;
                    bool resetRememberedAfterSelection=false;
                    if(event.code==14u){ //Down1C8FE8..1C9288.
                        audioPC=selected==40?0x1c9024u:selected==41?0x1c9068u:selected==42?0x1c90acu:
                            selected>=27&&selected<31?0x1c9110u:selected>=31&&selected<36?0x1c9170u:
                            selected>=36&&selected<40?0x1c91dcu:0x1c9280u;
                        if(selected>=40){
                            const auto pc=selected==40?0x1c8ff8u:selected==41?0x1c903cu:0x1c9080u;
                            if(!remembered(pc,3+(selected-40)*4,0,next))return false;moved=true;
                        }
                        else if(selected>=27&&selected<31){if(length){fields.fieldAC4=remainder(selected);next=40;moved=true;}}
                        else if(selected>=31&&selected<36){fields.fieldAC4=remainder(selected);next=41;moved=true;}
                        else if(selected>=36&&selected<40){if(length){fields.fieldAC4=remainder(selected);next=42;moved=true;}}
                        else {next=selected+13;moved=true;}
                    }else if(event.code==12u){ //Up1C92A8..1C9488.
                        audioPC=selected==40?0x1c92e8u:selected==41?0x1c9330u:selected==42?0x1c9378u:
                            selected<5?0x1c93b8u:selected<10?0x1c93f8u:selected<14?0x1c9444u:0x1c9480u;
                        if(selected>=40){
                            const auto pc=selected==40?0x1c92b8u:selected==41?0x1c9300u:0x1c9348u;
                            if(!remembered(pc,29+(selected-40)*4,26,next))return false;moved=true;
                        }
                        else if(selected<5){if(length){fields.fieldAC4=selected;next=40;moved=true;}}
                        else if(selected<10){fields.fieldAC4=selected;next=41;moved=true;}
                        else if(selected<14){if(length){fields.fieldAC4=selected;next=42;moved=true;}}
                        else {next=selected-13;if(next<=0)next+=42;moved=true;}
                    }else if(event.code==15u){ //Left1C94A8..1C95A8.
                        audioPC=selected==40?0x1c94e0u:selected==41?0x1c951cu:selected==42?0x1c9558u:0x1c95a0u;
                        if(selected>=40){if(length){next=selected==40?42:selected-1;resetRememberedAfterSelection=true;moved=true;}}
                        else {next=selected-1;if(next%13==0)next+=13;moved=true;}
                    }else if(event.code==13u){ //Right1C95C8..1C96D0.
                        audioPC=selected==40?0x1c9600u:selected==41?0x1c9640u:selected==42?0x1c967cu:0x1c96c8u;
                        if(selected>=40){if(length){next=selected==42?40:selected+1;resetRememberedAfterSelection=true;moved=true;}}
                        else {next=selected+1;if(next%13==1)next-=13;moved=true;}
                    }
                    if(moved){
                        fields.fieldABC=std::uint32_t(next);changed(); //Store BEFORE source audio/assertion.
                        if(resetRememberedAfterSelection)fields.fieldAC4=-1; //Horizontal special-key order.
                        if(next<1||next>42)return stop(Impl::S::source,0x1c91fc,"Results source navigation assertion reached");
                        if(!feedback(event.code==12u||event.code==15u?3u:4u,audioPC))return false;
                    }else if(event.code==2u){ //1C96F4 ->1CA340(a1=0), NOT an ack.
                        if(!p.publishedA90||!p.nameA90||!p.nameA90->string8||p.nameA90->length5C>10u||
                           p.nameA90->string8[p.nameA90->length5C])
                            return stop(Impl::S::owner,0x1ca364,"Results actual displayed nameA90 string unavailable");
                        // Source uses DISPLAYED A90, not the newer editA9C.
                        out.nextSourcePC=0x1ca368;
                        const std::string displayed(p.nameA90->string8.get(),p.nameA90->length5C);
                        if(displayed.find('\0')!=std::string::npos)
                            return stop(Impl::S::source,0x1ca368,"Results displayed CString/length consistency domain");
                        if(selected==42){
                            //3090E8 compares the displayed A90 CString, not editA9C.
                            //Literal3AD130 is "NO TIME". Its first store precedes
                            //audio1 and the actual Profile11054 bit0 store.
                            if(displayed=="NO TIME"){
                                p.derived->fieldADC=0x40800000u;changed();
                                if(!feedback(1,0x1ca4a0))return false;
                                bool committed=false;
                                if(!race.storeResultsNoTimeAndReconcile1CA4DC(committed)){
                                    if(committed)changed();
                                    return stop(Impl::S::owner,0x1ca4dc,
                                        "Results actual11054 bit0 mutation/stamp reconciliation stopped");
                                }
                                changed(); // No name copy, AD8 reset, fade or ACK on this branch.
                                fields.fieldAB4=1u;fields.fieldAB8=0u; //1CA8B4..8BC before helper return.
                            }else {
                            if(!race.profile_)return stop(Impl::S::owner,0x1ca4fc,"Results actual Profile absent");
                            bool nameCommitted=false;
                            if(!race.storeResultsNameAndReconcile1CA528(mode,displayed,nameCommitted)){
                                if(nameCommitted)changed();
                                return stop(Impl::S::owner,mode==3u?0x1ca528u:0x1ca56cu,
                                    "Results name-only Profile mutation/stamp reconciliation stopped");
                            }
                            changed(); // Actual persisted backing changed; no save/dirty/ack.
                            if(mode==3u){
                                using Query=OriginalRaceRecordQualification50KG;
                                using Observation=::original_position_stats_adapter50LA::Owner;
                                std::array<OriginalRaceRecordQualificationRacerObservation50KG,2> rows{};
                                std::size_t available=0;std::optional<Query> query;Query::Diagnostic qd;
                                const auto stamp=race.profile_->stamp();
                                const auto* materials=p.domain->materials382B38().get();
                                Observation::EndObservation current;Observation::Diagnostic observed;
                                if(!observer.captureEndObservation1AB520(p.application.get(),materials,
                                    p.component.sessionGeneration,current,observed))
                                    return stop(Impl::S::owner,0x1ca590,"Results qualification owner unavailable after name store");
                                const std::optional<std::uint8_t> low2=(race.flags1B0KnownMask_&3u)==3u?
                                    std::optional<std::uint8_t>{std::uint8_t(race.flags1B0KnownValue_&3u)}:std::nullopt;
                                while(!race.profile_->prepareRaceRecordQualification1ABFC0(2,low2,
                                    rows.data(),available,query,qd)){
                                    if(qd.status!=Query::Status::missingRacer||qd.managerIndex!=available||available>=rows.size())
                                        return stop(Impl::S::incomplete,qd.sourcePC,"Results reached qualification operand unrepresented; name retained");
                                    Observation::ResultsRecordIdentity row;auto& value=rows[available];
                                    if(!observer.captureResultsRecordIdentity(p.application.get(),materials,
                                        p.component.sessionGeneration,available,row,observed)||row.publication!=current.publication||
                                       !observer.readEndRetirementCD4(row,value.retirementCD4,observed))
                                        return stop(Impl::S::owner,qd.sourcePC,"Results actual qualification row retired");
                                    if(!value.retirementCD4&&
                                       (!observer.readResultsBestLapCB4(row,value.records.bestLapCB4,observed)||
                                        !observer.readResultsRaceTimeCB8(row,value.records.timeCB8,observed)||
                                        !observer.readResultsObservedTimingCD0(row,value.records.countCD0,observed)))
                                        return stop(Impl::S::owner,qd.sourcePC,"Results actual qualification operands unavailable");
                                    ++available;
                                }
                                const auto now=race.profile_->stamp();
                                if(!query||now.generation!=stamp.generation||now.mode34!=stamp.mode34||
                                   !observer.resultsRowsCurrent(p.application.get(),materials,p.component.sessionGeneration,current.publication))
                                    return stop(Impl::S::owner,0x1ca598,"Results qualification publication changed");
                                if(query->originalV0())return stop(Impl::S::incomplete,0x1ca5a8,
                                    "Results actual second-player qualification requires unjoined mode4 handoff");
                            }
                            //1CA5DC short circuit: subtype11258 is reached ONLY
                            //for mode34==0. Both routes first require real Aural26D070.
                            const auto profile=race.profile_->stamp();std::uint32_t subtype=0;
                            if(profile.mode34==0u&&!race.profile_->resultsSubtype11258(subtype))
                                return stop(Impl::S::owner,0x1ca5f0,"Results actual Profile11258 read unavailable");
                            const bool special=profile.mode34==0u&&subtype==3u;
                            const auto aural=application.sourceAural00Owner_;
                            const auto musicScope=race.endHud_.lock();
                            platform::OriginalApplicationAural50KG::Diagnostic ad;
                            if(!aural||!race.raceAudio_||!musicScope||!musicScope->music_||!musicScope->announcer_||
                               !musicScope->gameplay_||!musicScope->engines_||
                               !musicScope->endStateCurrent(race,*observer.camera_)||
                               (race.music10C_?!race.music10C_->nativeBackendCurrent(*musicScope):(race.world74_!=nullptr||!musicScope->music_))||
                               !aural->stopResultsNative26D070(application,*race.raceAudio_,*musicScope->music_,audio,
                                   *musicScope->announcer_,*musicScope->gameplay_,*musicScope->engines_,
                                   musicScope->countdown_,race.entity_.sessionGeneration,ad))
                                return stop(Impl::S::owner,special?0x1ca608u:0x1ca708u,
                                    "Results actual same-Application native Aural command0B stop unavailable; name retained");
                            changed(); // Native transport effects are consumed, never replayable.
                            if(!feedback(2,special?0x1ca620u:0x1ca720u))return false;
                            if(!musicScope->endStateCurrent(race,*observer.camera_)||
                               (race.music10C_?!race.music10C_->nativeBackendCurrent(*musicScope):(race.world74_!=nullptr||!musicScope->music_))||
                               !musicScope->music_->requestFade1A4AB0(0.5f).completed())
                                return stop(Impl::S::incomplete,special?0x1ca63cu:0x1ca73cu,
                                    "Results actual same10C music fade incomplete; prior native stop/feedback retained");
                            if(!musicScope->endStateCurrent(race,*observer.camera_)||
                               (race.music10C_?!race.music10C_->nativeBackendCurrent(*musicScope):(race.world74_!=nullptr||!musicScope->music_))||
                               !aural->finishResultsNative267BE0(application,*race.raceAudio_,ad))
                                return stop(Impl::S::owner,special?0x1ca654u:0x1ca754u,
                                    "Results real same-Application native control completion unavailable after stop/fade");
                            if(special){
                                if(!application.prepareResultsHallTransfer50LA(race)||
                                   !application.suspendResultsBlockingInput50LA(*this,race,*observer.camera_,host,
                                      ::original_main_entry50KG::Application::ResultsBlockingInputOrigin50LA::hall))
                                    return stop(Impl::S::owner,0x1ca678,"Results Hall actual request/fade custody unavailable");
                                return stop(Impl::S::incomplete,0x1ca678,"Results Hall original event retained across260850");
                            }
                            if(!application.suspendResultsBlockingInput50LA(*this,race,*observer.camera_,host,
                                ::original_main_entry50KG::Application::ResultsBlockingInputOrigin50LA::nameDone))
                                return stop(Impl::S::owner,0x1ca778,"Results same-poll blocking fade lifetime handoff failed");
                            return stop(Impl::S::incomplete,0x1ca778,
                                "Results genuine source event retained across blocking Video260850");
                            }
                        }
                        if(selected==40){
                            if(displayed.size()>=10){if(!feedback(6,0x1ca3c4))return false;}
                            else {
                                if(!feedback(2,0x1ca3e4))return false; //BEFORE edit.
                                out.nextSourcePC=0x1ca3f8;
                                *p.editA9C=displayed+" ";changed();fields.fieldAD8=0u;
                                fields.fieldAB4=1u;fields.fieldAB8=0u;
                            }
                        }else if(selected==41){
                            out.nextSourcePC=0x1ca414;
                            *p.editA9C=displayed;changed(); //1CA414 BEFORE length test/audio.
                            if(!displayed.empty()){
                                p.editA9C->resize(displayed.size()-1); //1CA428 beforekey5.
                                if(!feedback(5,0x1ca43c))return false;
                            }else if(!feedback(6,0x1ca45c))return false;
                            fields.fieldAD8=0u;fields.fieldAB4=1u;fields.fieldAB8=0u;
                        }else if(selected!=42){
                            if(displayed.size()>=10){if(!feedback(6,0x1ca7c0))return false;}
                            else {
                                if(!fields.fieldAD8)return stop(Impl::S::incomplete,0x1ca7d0,"Results first-characterAD8 unknown");
                                const auto i=std::size_t(selected-1);
                                if(!p.publishedA94||!p.keysA94||i>=p.keysConstructed||!p.keysA94[i]||
                                   !p.keysA94[i]->string8||p.keysA94[i]->length5C!=1u||!p.keysA94[i]->string8[0]||p.keysA94[i]->string8[1])
                                    return stop(Impl::S::source,0x1ca7f4,"Results actual one-byte key Text payload unavailable");
                                const char key=p.keysA94[i]->string8[0];
                                out.nextSourcePC=*fields.fieldAD8?0x1ca7f8u:0x1ca828u;
                                *p.editA9C=*fields.fieldAD8?std::string(1,key):displayed+key;changed();
                                if(!feedback(2,0x1ca840))return false; //AFTER edit, BEFORE AD8/blink.
                                fields.fieldAD8=0u;fields.fieldAB4=1u;fields.fieldAB8=0u;
                            }
                        }
                    }
                }else if(mode==2u){
                    //1C9704 does NOT repeat mode1's B4/2E0/Profile34 gates.
                    bool action=false;
                    if(!input.matchesNativeCategory2Code1277D0(3,event,action))
                        return stop(Impl::S::owner,0x1c971c,"Results tournament action3 mapping unavailable");
                    if(action)return stop(Impl::S::incomplete,0x1c972c,
                        "Results tournament restart1AC8E0 route unavailable");
                    if(!input.matchesNativeCategory2Code1277D0(2,event,action))
                        return stop(Impl::S::owner,0x1c97c0,"Results tournament action2 mapping unavailable");
                    if(action){
                        bool record=false;
                        if(!race.profile_||!race.profile_->resultsRecordBit7E4(record))
                            return stop(Impl::S::owner,0x1c97dc,"Results tournament actual Profile7E4 unavailable");
                        if(record){
                            using Query=OriginalRaceRecordQualification50KG;
                            using Observation=::original_position_stats_adapter50LA::Owner;
                            std::array<OriginalRaceRecordQualificationRacerObservation50KG,2> rows{};
                            std::size_t available=0;std::optional<Query> query;Query::Diagnostic qd;
                            const auto stamp=race.profile_->stamp();
                            const auto* materials=p.domain->materials382B38().get();
                            Observation::EndObservation current;Observation::Diagnostic observed;
                            if(!observer.captureEndObservation1AB520(p.application.get(),materials,
                                p.component.sessionGeneration,current,observed))
                                return stop(Impl::S::owner,0x1c97f0,"Results tournament selector1 observation unavailable");
                            const std::optional<std::uint8_t> low2=(race.flags1B0KnownMask_&3u)==3u?
                                std::optional<std::uint8_t>{std::uint8_t(race.flags1B0KnownValue_&3u)}:std::nullopt;
                            while(!race.profile_->prepareRaceRecordQualification1ABFC0(1,low2,
                                rows.data(),available,query,qd)){
                                if(qd.status!=Query::Status::missingRacer||qd.managerIndex!=available||available>=rows.size())
                                    return stop(Impl::S::incomplete,qd.sourcePC,"Results tournament selector1 operand unavailable");
                                Observation::ResultsRecordIdentity row;auto& value=rows[available];
                                if(!observer.captureResultsRecordIdentity(p.application.get(),materials,
                                    p.component.sessionGeneration,available,row,observed)||row.publication!=current.publication||
                                   !observer.readEndRetirementCD4(row,value.retirementCD4,observed))
                                    return stop(Impl::S::owner,qd.sourcePC,"Results tournament selector1 row owner changed");
                                if(!value.retirementCD4&&
                                   (!observer.readResultsBestLapCB4(row,value.records.bestLapCB4,observed)||
                                    !observer.readResultsRaceTimeCB8(row,value.records.timeCB8,observed)||
                                    !observer.readResultsObservedTimingCD0(row,value.records.countCD0,observed)))
                                    return stop(Impl::S::owner,qd.sourcePC,"Results tournament selector1 timing unavailable");
                                ++available;
                            }
                            const auto now=race.profile_->stamp();
                            if(!query||now.generation!=stamp.generation||now.mode34!=stamp.mode34||
                               !observer.resultsRowsCurrent(p.application.get(),materials,p.component.sessionGeneration,current.publication))
                                return stop(Impl::S::owner,0x1c97f8,"Results tournament selector1 publication changed");
                            if(!query->originalV0())return stop(Impl::S::incomplete,0x1c981c,
                                "Results tournament selector1 false requires unsupported actual second-human mode4");
                            OriginalResults104Diagnostic50LA modeDiagnostic;
                            const auto prefix=setModeCpuPrefix1CA8E0(race,observer,3u,1u,modeDiagnostic);
                            if(prefix!=ModeCpuPrefix::none)changed();
                            if(prefix!=ModeCpuPrefix::humanFeedbackTail&&prefix!=ModeCpuPrefix::zeroHumanTail)
                                return stop(modeDiagnostic.status,modeDiagnostic.sourcePC,
                                    "Results tournament actual mode3/device-feedback partial prefix retained");
                            if(!feedback(2,0x1c9834))return false;
                        }else{
                            const auto aural=application.sourceAural00Owner_;
                            const auto scope=race.endHud_.lock();
                            platform::OriginalApplicationAural50KG::Diagnostic ad;
                            if(!aural||!race.raceAudio_||!scope||!scope->music_||!scope->announcer_||
                               !scope->gameplay_||!scope->engines_||!scope->endStateCurrent(race,*observer.camera_)||
                               (race.music10C_?!race.music10C_->nativeBackendCurrent(*scope):(race.world74_!=nullptr||!scope->music_))||
                               !aural->stopResultsNative26D070(application,*race.raceAudio_,*scope->music_,audio,
                                   *scope->announcer_,*scope->gameplay_,*scope->engines_,scope->countdown_,
                                   race.entity_.sessionGeneration,ad))
                                return stop(Impl::S::owner,0x1c984c,"Results tournament actual native stop unavailable");
                            changed();
                            // Unlike summary: .5; unlike name DONE: Music BEFORE feedback.
                            if(!scope->endStateCurrent(race,*observer.camera_)||
                               (race.music10C_?!race.music10C_->nativeBackendCurrent(*scope):(race.world74_!=nullptr||!scope->music_))||
                               !scope->music_->requestFade1A4AB0(0.5f).completed())
                                return stop(Impl::S::incomplete,0x1c9868,"Results tournament actual Music fade incomplete");
                            if(!feedback(2,0x1c9880))return false;
                            if(!scope->endStateCurrent(race,*observer.camera_)||
                               (race.music10C_?!race.music10C_->nativeBackendCurrent(*scope):(race.world74_!=nullptr||!scope->music_))||
                               !aural->finishResultsNative267BE0(application,*race.raceAudio_,ad))
                                return stop(Impl::S::owner,0x1c9898,"Results tournament actual control finish unavailable");
                            if(!application.suspendResultsBlockingInput50LA(*this,race,*observer.camera_,host,
                                ::original_main_entry50KG::Application::ResultsBlockingInputOrigin50LA::tournament))
                                return stop(Impl::S::owner,0x1c98bc,"Results tournament same-poll fade handoff unavailable");
                            return stop(Impl::S::incomplete,0x1c98bc,"Results tournament original event retained across260850");
                        }
                    }
                }
            }
            }
            //1C98DC reads actual11258 BEFORE the literal category2/code1
            //comparison. Subtype3 skips the hide branch entirely.
            if(!race.profile_)return stop(Impl::S::owner,0x1c98e8,"Results actual Profile absent");
            platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection profile;
            if(!race.profile_->courseRaceSnapshot(profile))
                return stop(Impl::S::owner,0x1c98f4,"Results actual Profile11258 unavailable");
            if(profile.subtype!=3u&&event.code==1u){
                // Both visible/hidden paths use <=0 (not the display's <0).
                // Positive/NaN transition skips the toggle but STILL reaches
                // feedback2 at1C9AC4. Do not reset390/20C/layout208 here.
                if(p.base->transition164<=0.f){
                    const bool hiding=p.base->hidden160==0u;
                    p.base->transition164=.25f; //1C9944 /1C9A18 BEFORE160.
                    p.base->hidden160=hiding?1u:0u;changed();
                    const auto ordinal=hiding?427:426;
                    const auto readPC=hiding?0x1c995cu:0x1c9a2cu;
                    using Dictionary=assets::OriginalApplicationLocalization50KG;
                    const auto dictionary=application.sourceLocalization32COwner_;
                    std::shared_ptr<const Dictionary::SelectedText> selected;
                    assets::OriginalApplicationLocalizationDiagnostic50KG localized;
                    std::string_view bytes;
                    if(!dictionary||p.application->localization32CNativeOwner_!=dictionary.get()||
                       !application.borrowLocalizationText18AC60(*dictionary,ordinal,selected,localized)||
                       !Dictionary::SelectedText::BorrowReader::readCurrent(selected,application,*dictionary,ordinal,bytes))
                        return stop(Impl::S::owner,readPC,"Results actual selected Application32C hide caption unavailable");
                    if(!p.titles490)return stop(Impl::S::owner,readPC,"Results actual title500 owner absent");
                    auto& title=(*p.titles490)[1]; //Text500: string508, flags55A,capacity55B,length55C.
                    if(!title.string8||title.length5C>title.capacity5B||title.string8[title.length5C])
                        return stop(Impl::S::owner,readPC,"Results owned hide title string backing changed");
                    // Full strlen/equality before capacity clamp; terminator
                    // and dirty bits0/1 follow the exact reached store order.
                    p.assign(title,bytes.data(),bytes.size());
                }
                if(!feedback(2,0x1c9ad4))return false;
                out.sourceEffectsCommitted=true; // Reached sound call even when no toggle occurred.
            }
            //Exact concrete142350: actual firstChild18 and descendants24 are
            //empty (retirementCurrent above). No surrogate child or renderer.
            if(!application.passResultsProcessSuccessors50LA(race))
                return stop(Impl::S::owner,0x142350,"Results queued request event successor changed");
            ++out.eventsCompleted;
        }
        out.blocked=false;out.nextSourcePC=0x1c9b04;d.status=Impl::S::complete;d.sourcePC=0x1c9b04;return true;
    }catch(...){return stop(Impl::S::allocation,out.nextSourcePC,"Results bounded native name storage failed after retained prefix");}
}
}

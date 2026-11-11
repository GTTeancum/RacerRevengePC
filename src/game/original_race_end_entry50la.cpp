#include "original_results104_storage50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_race_hud_scope50la.h"
#include "../../tests/native/ra_source_camera_ownership50la.h"
#include "advancement_selectors49k.h"
#include <thread>
#include <bit>
#include <cfenv>
#include <cmath>

namespace rr::game {
bool OriginalResults10450LA::activate1A7E18(const OriginalRaceConstructor50LA& race,
    OriginalResults104Diagnostic50LA& d)noexcept {
    d={};
    // Reuse the actual Race's immutable construction-thread fence. Reject
    // before touching Impl; no cross-thread borrowed activation is supported.
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1a7e18;return false;
    }
    if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1a7e18;return false;}
    auto& p=*impl_;
    if(!constructed()||!p.loadComplete||race.results104_.get()!=this||
       !race.counted_||race.rolledBack_||race.application_!=p.application||
       race.entity_.sessionGeneration!=p.component.sessionGeneration||!retirementCurrent()||
       !p.application->entityAuthority_.canRetire(race.entity_)||
       !original_entity_chain_detail50LA::forest(p.application->entityAuthority_.view()))
        return p.stop(d,Impl::S::owner,0x1a7e18,"Results activation requires its same current Race/Application Entity");
    // Concrete table3CF3A0: +20=141560, +2C=1414C0. Source writes activity
    // BEFORE the notification tail. Preflight native empty-child support;
    // owned ScreenObjects are resources, never fabricated Entity children.
    // retirementCurrent already proves firstChild18/descendants24 empty.
    p.component.flags29=std::uint8_t((p.component.flags29&~1u)|1u); //141578
    p.component.flags29=std::uint8_t((p.component.flags29&~2u)|2u); //14158C
    p.disabled=false; //Native mirror of those source stores, not a new gate.
    //141598 -> actual1414C0 sees null18 and returns141518. Nothing else is
    //activated, mode200/previous204/dirty208/acknowledgment20C stay untouched.
    // EXPLICIT NATIVE VISUAL ADAPTATION, after that completed source leaf:
    // initialize only the first owned activation's wholly unknown key pulse.
    // The pair is the recovered FALLING-CLAMP endpoint1C8B90/94 (direction1,
    // pulse+0), not evidence that the original constructor initialized it.
    // Source1C8AE0 keeps using the real Root23C; no clock or input state is set.
    using PulseStart=Impl::PulseStartProvenance;
    if(p.pulseStartProvenance==PulseStart::notObserved){
        if(!p.pulse394&&!p.direction398){
            p.direction398=1u;
            p.pulse394=0u; //Positive binary32 zero, NOT a substitute delta.
            p.pulseStartProvenance=PulseStart::nativeFirstActivationEndpoint;
        }else{
            // A prior complete pair is preserved without asserting its origin.
            // A partial pair remains partial: never repair one unknown lane,
            // replace the known lane, or retry on a later activation/resume.
            p.pulseStartProvenance=p.pulse394&&p.direction398?
                PulseStart::preservedKnownPair:PulseStart::preservedPartialUnknown;
        }
    }
    // SEPARATE NATIVE VISUAL POLICY: AB8/AB4 have no recovered constructor
    //initializer. Their only recovered consumers accumulate cursor blink or
    //choose whether to DRAW the cursor; source input reads neither lane.
    //Use the actual1C9C60/64 visual endpoint ONCE, not its name/record body.
    //This does not call1C9B10 early or establish any other name-entry field.
    using BlinkStart=Impl::CursorBlinkStartProvenance;
    if(p.cursorBlinkStartProvenance==BlinkStart::notObserved){
        const bool timerKnown=p.nameEntry&&p.nameEntry->fieldAB8.has_value();
        const bool cursorKnown=p.nameEntry&&p.nameEntry->fieldAB4.has_value();
        if(!timerKnown&&!cursorKnown){
            if(!p.nameEntry)p.nameEntry.emplace(); //Other optionals remain unknown.
            p.nameEntry->fieldAB8=0u; //Positive binary32 zero, not a supplied clock/delta.
            p.nameEntry->fieldAB4=1u; //Cursor-visible endpoint, not input/ACK authority.
            p.cursorBlinkStartProvenance=BlinkStart::nativeFirstActivationNameEntryEndpoint;
        }else{
            //Preserve a known phase, or a partial unknown pair, verbatim.
            //No later activation/resume repairs it or restarts this policy.
            p.cursorBlinkStartProvenance=timerKnown&&cursorKnown?
                BlinkStart::preservedKnownPair:BlinkStart::preservedPartialUnknown;
        }
    }
    d.status=Impl::S::complete;d.sourcePC=0x141518;return true;
}

void OriginalRaceConstructor50LA::continueEndEntryPrefix1A7DD0(
    OriginalRaceEndEntryDiagnostic50LA& d)noexcept {
    using S=OriginalRaceEndEntryDiagnostic50LA::Status;
    d={};
    if(resultsFeedbackThread_!=std::this_thread::get_id()){
        d.operation="Race end prefix requires its actual construction thread";return;
    }
    const auto reached=[&]()noexcept {
        d.reachedStateKnown=true;
        d.f8VisibilityCommitted=endEntrySubtype_==1u&&
            (endEntryStep_>=EndEntryStep::results);
        d.resultsActivationCommitted=endEntrySubtype_==1u&&
            (endEntryStep_>=EndEntryStep::hud);
        d.hudStopCommitted=endEntryStep_>=EndEntryStep::dispatch;
        d.recordBit7E4Committed=endEntryStep_>=EndEntryStep::recordBit;
        d.rivalWord7E8Committed=endEntryStep_>=EndEntryStep::rivalWord;
        d.acknowledgement20CRead=endEntryStep_>=EndEntryStep::ackRead&&bool(endAcknowledgement20C_);
        d.waitingBit5Committed=endEntryStep_>=EndEntryStep::waitingBit;
        d.resultsModeScalarReached=endEntryStep_>=EndEntryStep::waitingModeScalar;
        d.resultsModeFeedbackReturned=endEntryStep_>=EndEntryStep::waitingPlaces;
        d.dispatchReturned1AB520=endDispatchReturned_;
        if(d.acknowledgement20CRead)d.observedAcknowledgement20C=*endAcknowledgement20C_;
    };
    const auto stop=[&](S status,std::uint32_t pc,const char* why)noexcept {
        reached();d.status=status;d.sourcePC=pc;d.operation=why;
    };
    if(!counted_||rolledBack_||!constructorSuffix_||!loadPrefixReached_||
       !application_||!application_->application10()||application_->attachmentLock423F58()||
       !application_->entityAuthority_.canRetire(entity_)||!profile_||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view())){
        stop(S::owner,0x1a7dd0,"Race end prefix current counted Application/Profile ownership absent");return;
    }
    platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection current;
    if(!profile_->courseRaceSnapshot(current)){
        stop(S::profile,0x1a7de8,"Race end prefix actual readable Profile subtype unavailable");return;
    }
    const auto stamp=profile_->stamp();
    if(endEntryStep_==EndEntryStep::subtype){
        endEntryProfile_=stamp;endEntrySubtype_=current.subtype; //Actual11258 read, not launch override.
        endEntryStep_=endEntrySubtype_==1u?EndEntryStep::f8:EndEntryStep::hud;
    }else if(!endEntryProfile_||stamp.generation!=endEntryProfile_->generation||
             stamp.mode34!=endEntryProfile_->mode34||current.subtype!=endEntrySubtype_){
        stop(S::profile,0x1a7dd0,"Suspended end invocation Profile changed; prior effects retained, never replayed");return;
    }
    if(endEntrySubtype_==1u&&(endEntryStep_>=EndEntryStep::results)&&
       (!f8Attached_||!endEntryF8_||f8Owner_.get()!=endEntryF8_||f8Source_!=endEntryF8_||
        !f8Source_->belongsTo(application_,entity_.sessionGeneration))){
        stop(S::owner,0x1a7e0c,"Suspended prefix F8 owner was retired/replaced; prior effects retained");return;
    }
    if(endEntrySubtype_==1u&&(endEntryStep_>=EndEntryStep::results)){
        const auto* f8=f8Source_->entity();
        if(!f8||f8->parent1C!=&entity_||f8->mask20!=0xffffffffu){
            stop(S::owner,0x1a7e0c,"Suspended prefix F8 attachment changed; prior effects retained");return;
        }
    }
    if(endEntrySubtype_==1u&&(endEntryStep_>=EndEntryStep::hud)&&
       (!endEntryResults_||results104_.get()!=endEntryResults_||
        !results104_->belongsTo(application_,entity_.sessionGeneration)||!results104_->retirementCurrent())){
        stop(S::owner,0x1a7e24,"Suspended prefix Results owner was retired/replaced; prior effects retained");return;
    }
    if(endEntryStep_==EndEntryStep::f8){
        if(!f8Attached_||!f8Owner_||f8Source_!=f8Owner_.get()||
           !f8Source_->belongsTo(application_,entity_.sessionGeneration)){
            stop(S::owner,0x1a7df4,"Actual published/attached Race+F8 absent");return;
        }
        const auto* f8=f8Source_->entity();
        if(!f8||f8->parent1C!=&entity_||f8->mask20!=0xffffffffu){
            stop(S::owner,0x1a7df4,"F8 is not this Race's actual source child");return;
        }
        //1A7DF8..1A7E08: a1=0,a2=1,a3=1,t0=1,t1=1. The existing concrete
        //leaf writes bits2/3/4 even without a bit0 edge; immediate preserves1E0.
        //No Camera uiFlags1E4_/transition shadow is read or overwritten here.
        if(!f8Source_->visibility199770(0,1,1,1,1)){
            stop(S::source,0x1a7e04,"Actual F8 visibility leaf rejected before its stores");return;
        }
        endEntryF8_=f8Source_;endEntryStep_=EndEntryStep::results;
    }
    if(endEntryStep_==EndEntryStep::results){
        //Do not preflight this later source call before the F8 effects. A
        //failure here retains the earlier visibility and resumes HERE.
        if(!results104_){stop(S::owner,0x1a7e0c,"Actual Race104 absent after F8 visibility");return;}
        OriginalResults104Diagnostic50LA result;
        if(!results104_->activate1A7E18(*this,result)){
            stop(S::source,result.sourcePC,result.operation?result.operation:
                "Actual Results slot20 activation rejected; F8 effects retained");return;
        }
        endEntryResults_=results104_.get();endEntryStep_=EndEntryStep::hud;
    }
    if(endEntryStep_==EndEntryStep::hud){
        //Source every-subtype1A7E24 calls1AD230. The private registration
        //authenticates the actual Context HUD, Bank and borrowed live Camera.
        const auto hud=endHud_.lock();
        if(!hud||!hud->stop1AD230(*this)){
            stop(S::owner,0x1a7e24,"Actual same-Race HUD lifetime/viewport rows unavailable; earlier effects retained");return;
        }
        endEntryStep_=EndEntryStep::dispatch;
    }
    if(endDispatchReturned_){ //Every race (RA included) re-enters 1A7DD0..1AB520 each update.
        // Common race: every race update re-enters 1A7DD0..1AB520, whose
        // 1AB5B0 reads Results+20C. Zero (bit5 already set) returns at
        // 1AB924; nonzero takes the acknowledgement branch 1AB5BC. The race
        // update tail (1A7F58.., the 1A8158 replay loop) is the native loop's.
        const auto* derived=results104_&&results104_->impl_&&results104_->impl_->derived?&*results104_->impl_->derived:nullptr;
        if(!derived){stop(S::owner,0x1ab5b0,"Actual Results20C read unavailable");return;}
        // The same 1AB5B0 read the RA branch retains; the 1AB5BC rows (1AC440)
        // check this observed acknowledgement.
        // Retained only once nonzero: the Results input borrow requires the
        // unread state while the replay waits for the confirm.
        if(derived->acknowledgement20C)endAcknowledgement20C_=derived->acknowledgement20C;
        stop(S::callerReturned,derived->acknowledgement20C?0x1ab5bc:0x1abe48,
            derived->acknowledgement20C?"Common race: Results acknowledged (1AB5BC)":"Common race: Results replay running");
        d.acknowledgement20CRead=true;d.observedAcknowledgement20C=derived->acknowledgement20C;return;
    }
    //Actual Race vtable+78 is1AB520. Its supported pre-ack prefix is private and
    //uncalled from the live loop; this is not genuine end admission.
    if(!results104_||current.subtype!=1u){
        stop(S::incomplete,0x1abbc0,"Actual1AB520 null104/non-subtype1 branch; separate tail unimplemented");return;
    }
    reached();
    const auto scope=endHud_.lock();
    if(!scope){stop(S::owner,0x1a7e34,"Actual end-prefix lifetime was revoked; prior effects retained");return;}
    scope->continuePreAck1AB520(*this,d);
    if(endDispatchReturned_)return; //d already says callerReturned (replay started).
}

void OriginalRaceConstructor50LA::continueEndCaller1A7E40(
    OriginalRaceEndEntryDiagnostic50LA& d)noexcept {
    ra_source_camera50LA::OwnershipBridge* endRuntime=nullptr;
    using S=OriginalRaceEndEntryDiagnostic50LA::Status;
    const auto stop=[&](S status,std::uint32_t pc,const char* why)noexcept {
        d.status=status;d.sourcePC=pc;d.operation=why;
    };
    // Private call only after the parent's current ownership/profile fence.
    // No new1AB520 invocation; no source return receipt is produced.
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!endDispatchReturned_){
        stop(S::owner,0x1a7e40,"Caller requires same-thread returned1AB520");return;
    }
    using Cut=EndCallerCut;
    if(!vcr78Owner_||vcr78Source_!=vcr78Owner_.get()||!vcr78Source_->belongsToRace(*this)){
        stop(S::owner,0x1a7e40,"Actual published Race78 absent; native null cannot select1A82E0");return;
    }
    if(endCallerCut_==Cut::owner78){
        endCallerVcr_=vcr78Source_;endCallerCut_=Cut::manager58;
    }else if(endCallerVcr_!=vcr78Source_){
        stop(S::owner,0x1a7e40,"Suspended caller78 replaced; saved locals retained");return;
    }
    if(endCallerCut_==Cut::manager58){
        const auto scope=endHud_.lock();
        if(!scope||!scope->camera_||!scope->endStateCurrent(*this,*scope->camera_)||
           !scope->manager_||!scope->bank_||!timingB4_||
           !timingB4_->readEndCallerMode(*this,*scope->manager_,*scope->bank_,endCallerMode_)){
            stop(S::owner,0x1a7f64,"Actual B4 flags58 bits3..4 unavailable");return;
        }
        endCallerCut_=Cut::subtype;
    }
    if(endCallerCut_==Cut::subtype){
        platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection current;
        if(!profile_||!profile_->courseRaceSnapshot(current)){
            stop(S::profile,endCallerMode_?0x1a7f90:0x1a8108,"Actual caller Profile subtype unavailable");return;
        }
        // This private invocation supports only the original subtype1 end
        // dispatch. Do not borrow its source path for the separate subtype3
        // lap-backup/reset/rate calls at1A7F9C or1A8114.
        if(current.subtype!=1u){
            stop(S::incomplete,endCallerMode_?0x1a7f94:0x1a810c,"Separate caller subtype branch unsupported");return;
        }
        endCallerCut_=(endCallerMode_==1u||endCallerMode_==3u)?Cut::associationHead:Cut::vcrBit5;
    }
    if(endCallerCut_==Cut::associationHead){
        const auto scope=endHud_.lock();bool finished=false;
        if(!scope||!scope->readEndAssociationHead1A8074(*this,finished)){
            stop(S::owner,0x1a8074,"Actual associated3C[0] C90/CD8 read unavailable");return;
        }
        // Source3C is the stable association array, NOT retirement40. Bit0
        //set skips the racer loop AND Race1A8/VCR-start effects at1A80BC..EC.
        if(finished)endCallerCut_=Cut::vcrBit5;
        else {
            endCallerIndexS1_=0; //1A8090 branch-delay slot.
            endCallerCut_=Cut::recordLoopCount;
        }
    }
    while(endCallerCut_==Cut::recordLoopCount||endCallerCut_==Cut::recordRacerGetter){
        if(endCallerCut_==Cut::recordLoopCount){
            const auto scope=endHud_.lock();std::int32_t count=0;
            if(!endCallerIndexS1_||!scope||!scope->readEndAssociatedCount1A80AC(*this,count)){
                stop(S::owner,0x1a80ac,"Actual B0 manager count30 unavailable; source S1 retained");return;
            }
            endCallerCut_=*endCallerIndexS1_<count?Cut::recordRacerGetter:Cut::recordAfterLoop;
            if(endCallerCut_==Cut::recordAfterLoop)break;
        }
        const auto scope=endHud_.lock();
        if(!endCallerIndexS1_||*endCallerIndexS1_<0||!scope||!scope->camera_||
           !scope->endStateCurrent(*this,*scope->camera_)||!(endRuntime=borrowEndRuntimeNative())){
            stop(S::owner,0x1a8094,"Actual same-world indexed racer owner unavailable");return;
        }
        std::uint32_t pc=0x1a8094;
        if(!endRuntime->recordEndRacer229F20(*scope->camera_,
            static_cast<std::size_t>(*endCallerIndexS1_),pc)){
            stop(S::source,pc,"Actual attached229F20 incomplete; reached per-racer C38 retained, index unchanged");return;
        }
        ++*endCallerIndexS1_; //1A80A4, only after complete229F20 return.
        endCallerCut_=Cut::recordLoopCount; //Reload actual B0 count each iteration.
    }
    if(endCallerCut_==Cut::recordAfterLoop){
        //1A80BC/C0/C4: unsigned backing preserves ADDIU's wraparound. This
        //is reached only AFTER every229F20 returns; never on its current cut.
        field1A8_+=1u;
        endCallerCut_=Cut::recordVcrStart;
    }
    if(endCallerCut_==Cut::recordVcrStart){
        OriginalSimpleVCRDiagnostic50LA vcr;
        if(!vcr78Source_->startRecord1A80C8(*this,vcr)){
            stop(S::owner,vcr.sourcePC,vcr.operation);return;
        }
        endCallerCut_=Cut::vcrBit5; //1A80E8 branch, bit1 store in delay slot.
    }
    if(endCallerCut_==Cut::vcrBit5){
        bool bit5=false;OriginalSimpleVCRDiagnostic50LA vcr;
        if(!vcr78Source_->readEndCallerBit5(*this,bit5,vcr)){
            stop(S::owner,vcr.sourcePC,vcr.operation);return;
        }
        if(bit5)endCallerIndexS1_=0; //1A8174 delay slot, before1A8190 count read.
        endCallerCut_=bit5?Cut::replayRacers:Cut::rootTail;
    }
    while(endCallerCut_>=Cut::replayRacers&&endCallerCut_<=Cut::replayRacerWorld){
        const auto scope=endHud_.lock();
        if(!endCallerIndexS1_||*endCallerIndexS1_<0||!scope||!scope->camera_||
           !scope->endStateCurrent(*this,*scope->camera_)||!(endRuntime=borrowEndRuntimeNative())){
            stop(S::owner,0x1a8190,"Actual replay-loop indexed owner unavailable");return;
        }
        if(endCallerCut_==Cut::replayRacers){
            std::int32_t count=0;
            if(!scope->readEndAssociatedCount1A80AC(*this,count)){
                stop(S::owner,0x1a8194,"Actual replay B0 count30 unavailable");return;
            }
            if(*endCallerIndexS1_>=count){endCallerCut_=Cut::replayWorldTail;break;}
            endCallerCut_=Cut::replayRacerMode;
        }
        const auto slot=static_cast<std::size_t>(*endCallerIndexS1_);
        const auto& session=*scope->camera_;auto& runtime=*endRuntime;
        const auto returnedRacer=[&]()noexcept {
            ++*endCallerIndexS1_; //1A8188, only after actual22A174 epilogue.
            endReplayZoneC38_.reset();endReplayCenterC40_.reset();
            endCallerCut_=Cut::replayRacers;
        };
        if(endCallerCut_==Cut::replayRacerMode){
            std::uint32_t mode=0;
            if(!runtime.readReplayMode3C229F94(session,slot,mode)){
                stop(S::owner,0x229f94,"Actual canonical indexed racer mode3C unavailable");return;
            }
            // Zero is an ACTUAL stored source lane, not an absent-pointer
            //fallback.229F98 skips ALL CD4/C38/C40/reset work on this branch.
            if(!mode){returnedRacer();continue;}
            endCallerCut_=Cut::replayRacerRetirement;
        }
        if(endCallerCut_==Cut::replayRacerRetirement){
            std::uint32_t retirement=0;
            if(!runtime.readReplayRetirementCD4(session,slot,retirement)){
                stop(S::owner,0x229fa0,"Actual indexed racer retirementCD4 unavailable");return;
            }
            //229FB8 reads THIS canonical Race1A8 only after nonzeroCD4.
            //229FBC is signed SLT, not unsigned expiry/positive-time logic.
            if(retirement&&std::bit_cast<std::int32_t>(field1A8_)>=std::bit_cast<std::int32_t>(retirement)){
                returnedRacer();continue;
            }
            endCallerCut_=Cut::replayRacerZone;
        }
        if(endCallerCut_==Cut::replayRacerZone){
            std::uint32_t zone=0;
            if(!runtime.readReplayRecordedZoneC38(session,slot,zone)){
                stop(S::owner,0x229fd0,"Actual reached racerC38 unavailable; no default recorded zone");return;
            }
            endReplayZoneC38_=zone;endCallerCut_=Cut::replayRacerCenter;
        }
        if(endCallerCut_==Cut::replayRacerCenter){
            std::array<std::uint32_t,4> center;
            if(!runtime.readReplayRecordedCenterC40(session,slot,center)){
                stop(S::owner,0x229fdc,"Actual reached racerC40 unavailable; C38 operand retained");return;
            }
            endReplayCenterC40_=center;endCallerCut_=Cut::replayRacerWorld;
        }
        // Same canonical C38/C40 written by229F20, now saved source operands.
        // No host scene projection substitutes original World4C4/4C0 arrays,
        //BC0/BD0/C30/C34 stores or216A30/228110/later virtual effects.
        stop(S::incomplete,0x229ff8,"Actual nonzero-mode World74/4C4/4C0 and replay-reset body unrepresented");return;
    }
    if(endCallerCut_==Cut::replayWorldTail){
        stop(S::incomplete,0x1a81a4,"All indexed229F80 calls returned; actual World74 virtual74 and later replay tail unrepresented");return;
    }
    continueEndCommonRoot1A82E0(d);
}

void OriginalRaceConstructor50LA::continueEndCommonRoot1A82E0(
    OriginalRaceEndEntryDiagnostic50LA& d)noexcept {
    using S=OriginalRaceEndEntryDiagnostic50LA::Status;using Cut=EndCallerCut;
    const auto stop=[&](S status,std::uint32_t pc,const char* why)noexcept {
        d.status=status;d.sourcePC=pc;d.operation=why;
    };
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!application_||!counted_||rolledBack_||
       application_->attachmentLock423F58()||!application_->entityAuthority_.canRetire(entity_)||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view())){
        stop(S::owner,0x1a82e0,"Actual common-root Race/Application/Entity lifetime unavailable");return;
    }
    if(endCallerCut_!=Cut::rootTail&&endCallerCut_!=Cut::rootEntity&&
       endCallerCut_!=Cut::rootRenderer&&endCallerCut_!=Cut::rootAfterEntity&&
       !(endCallerCut_>=Cut::rootPhysics&&endCallerCut_<=Cut::callerReturn)){
        stop(S::source,0x1a82e0,"Common tail not reached by the saved caller cut");return;
    }
    auto& app=*application_;
    if(endCallerCut_==Cut::rootRenderer){
        stop(S::incomplete,0x141f3c,"Saved eligible child awaits actual renderer+40 slot20(0); no predicate replay");return;
    }
    if(endCallerCut_==Cut::rootTail){
        if(!app.raceUpdateGuard8610_){
            app.raceUpdateProfile434178_=-1; //1A82F4 before guard byte.
            app.raceUpdateGuard8610_=1; //1A82FC, process lifetime, not per-race readiness.
        }
        endCallerCut_=Cut::rootEntity;
    }
    if(endCallerCut_==Cut::rootEntity){
        // No fabricated delta: require exact already-saved argument from the
        //same pending canonical end-clock invocation. Never read latest23C.
        if(!endEntrySourceF20_||!endClockRootPublication_||!endClockProgress_||
           endClockProgress_->status!=OriginalHumanEndClockStatus50KG::needOriginalEndDispatch1A7DD0){
            stop(S::incomplete,0x1a8304,"Exact suspended invocation f20 absent; reached lazy global store retained");return;
        }
        if(!endRootChildrenRead_){
            //141EE0/EE8 parent descendants gate; only then load child18.
            endRootChildS0_=(entity_.descendants24&2u)?entity_.firstChild18:nullptr;
            endRootEnrollmentSerial_=app.entityAuthority_.enrollmentSerial_;
            endRootChildrenRead_=true;
        }
        while(endRootChildS0_){
            const auto* child=endRootChildS0_;
            //Authority membership is checked before dereferencing a suspended
            //borrow. A replacement/reparent is not a reason to restart traversal.
            bool enrolled=false;
            for(std::size_t i=0;i<app.entityAuthority_.count_;++i)
                if(app.entityAuthority_.entries_[i].node==child){enrolled=true;break;}
            if(endRootEnrollmentSerial_!=app.entityAuthority_.enrollmentSerial_||
               !enrolled||!app.entityAuthority_.canRetire(*child)||child->parent1C!=&entity_){
                stop(S::owner,0x141efc,"Suspended Entity child ownership changed");return;
            }
            const auto flags=child->flags29Value();
            //141EFC..141F34: bit0 active, no bit2/3, own mask20 bit1.
            if((flags&13u)==1u&&(child->mask20&2u)){
                //Original MUST call actual renderer442F70+40 virtual20(0)
                //before this child's slot38(f20). Neither is a generic no-op.
                endCallerCut_=Cut::rootRenderer; //Predicates already read; retain this exact call cut.
                stop(S::incomplete,0x141f3c,"Actual renderer+40 slot20(0) and eligible child slot38 remain unrepresented");return;
            }
            endRootChildS0_=child->next14; //141F68 only after a real skip/return.
        }
        endRootReturnS4_=1u; //Actual141F84 return, saved at1A830C; NOT1A7B40.
        endCallerCut_=Cut::rootAfterEntity;
    }
    if(endCallerCut_==Cut::rootAfterEntity){
        if(!app.raceUpdateGuard8614_){
            app.raceUpdateProfile434180_=-1;app.raceUpdateGuard8614_=1;
        }
        if(!app.raceUpdateGuard8618_){
            app.raceUpdateScale434188_=0x42700000u; //1A8344:60.f source bits.
            app.raceUpdateGuard8618_=1;
        }
        if(!app.raceUpdateGuard861C_){
            const float scale=std::bit_cast<float>(app.raceUpdateScale434188_);
            if(std::fegetround()!=FE_TONEAREST||!std::isfinite(scale)||scale==0.f){
                stop(S::source,0x1a8370,"Source reciprocal outside bounded binary32 admission");return;
            }
            volatile float one=1.f;volatile float quotient=one/scale;
            if(!std::isfinite(quotient)){
                stop(S::source,0x1a8370,"Source reciprocal outside finite binary32 domain");return;
            }
            app.raceUpdateGuard861C_=1; //1A8378 BEFORE the float store1A837C.
            app.raceUpdateReciprocal434190_=std::bit_cast<std::uint32_t>(float(quotient));
        }
        endCallerCut_=Cut::rootPhysics;
    }
    continueEndPhysics1A8390(d);


}

void OriginalRaceConstructor50LA::continueEndPhysics1A8390(
    OriginalRaceEndEntryDiagnostic50LA& d)noexcept {
    ra_source_camera50LA::OwnershipBridge* endRuntime=nullptr;
    using S=OriginalRaceEndEntryDiagnostic50LA::Status;using Cut=EndCallerCut;
    const auto stop=[&](S status,std::uint32_t pc,const char* why)noexcept {
        d.status=status;d.sourcePC=pc;d.operation=why;
    };
    if(endCallerCut_==Cut::rootPhysics){
        Schedule49K schedule{};
        if(!endEntrySourceF20_||!endRootReturnS4_||!application_||
           !selectSchedule49K(std::bit_cast<float>(*endEntrySourceF20_),
               std::bit_cast<float>(application_->raceUpdateScale434188_),schedule)){
            stop(S::source,0x1a8390,"Actual saved-f20/scale outside existing bounded49K schedule domain");return;
        }
        //Existing49K implements EE CVT.W.S truncation, NOT host lrint or
        //guessed current FCSR conversion. No frame scheduling callback runs.
        endPhysicsFirstS3_=true;endPhysicsPassS5_=0;
        endPhysicsCountS2_=schedule.count;
        endPhysicsFactorF20_=std::bit_cast<std::uint32_t>(schedule.factorPerSubstep);
        endPhysicsIndexS1_=0;endCallerCut_=Cut::physicsCount;
    }
    for(;;){
        if(endCallerCut_>=Cut::physicsMusicBody&&endCallerCut_<=Cut::musicClear&&
           (!endMusicSource10C_||music10C_.get()!=endMusicSource10C_)){
            stop(S::owner,0x1a847c,"Saved same-invocation music10C target replaced");return;
        }
        if(endCallerCut_==Cut::physicsCall){
            const auto scope=endHud_.lock();
            if(endPhysicsIndexS1_<0||!scope||!scope->camera_||
               !scope->endStateCurrent(*this,*scope->camera_)||!(endRuntime=borrowEndRuntimeNative())||
               !endRuntime->replayRacerIdentityCurrent(*scope->camera_,
                   static_cast<std::size_t>(endPhysicsIndexS1_))){
                stop(S::owner,0x1a83fc,"Saved indexed219810 target no longer has its actual same-world owner");return;
            }
            //21982C..219908 profiling-only native adaptation. Do not invent
            //guest index434728 or skip any physical/gameplay operation.
            if(!application_->observeNativeSourceProfile50LA(
                   OriginalApplicationEntities50LA::NativeSourceProfileSite50LA::runSimulation)){
                stop(S::owner,0x21982c,"Actual process native profiling registry unavailable");return;
            }
            endCallerCut_=Cut::physicsBody;
        }
        if(endCallerCut_==Cut::physicsGroundedSlopeStopped){
            stop(S::incomplete,endGroundedSlopeStopPC_,
                "Grounded slope prefix retained; unsupported owner/getter/scalar cut is terminal for this invocation");return;
        }
        if(endCallerCut_==Cut::physicsGroundedSlope){
            const auto scope=endHud_.lock();
            if(endPhysicsIndexS1_<0||!scope||!scope->camera_||
               !scope->endStateCurrent(*this,*scope->camera_)||!application_||
               application_->nativeOwnerThread_!=std::this_thread::get_id()||!(endRuntime=borrowEndRuntimeNative())){
                stop(S::owner,0x219964,"Actual same-App grounded slope owner unavailable");return;
            }
            auto& runtime=*endRuntime;const auto slot=static_cast<std::size_t>(endPhysicsIndexS1_);
            if(!runtime.replayRacerIdentityCurrent(*scope->camera_,slot)){
                stop(S::owner,0x219964,"Actual indexed slope racer unavailable");return;
            }
            //One-shot after owner admission. All lazy/scratch/read prefixes
            //survive a later rejection; no repaired world or numeric replay.
            endCallerCut_=Cut::physicsGroundedSlopeStopped;
            endGroundedSlopeStopPC_=0x219964;
            auto& lazy=application_->nativeGroundedGravityStatics_;
            if(!lazy.initialized[0]){
                lazy.words[0]=0x43c13334u; //219978 before guard219980.
                lazy.initialized[0]=true;
            }
            if(!lazy.initialized[1]){
                lazy.words[1]=0x3f34fdf4u; //21999C before guard2199A4.
                lazy.initialized[1]=true;
            }
            endGroundedForward60_={0u,0u,0u,0x3f800000u}; //2199A8 SQ VF0.
            endGroundedUp70_={0u,0u,0u,0x3f800000u}; //2199B0 SQ VF0.
            if(!runtime.readEndGroundedAxes2199B8(*scope->camera_,slot,
                endGroundedForward60_,endGroundedUp70_,endGroundedSlopeStopPC_))continue;
            endGroundedF4_=lazy.words[0]; //2199C4 actual native process lane.
            const float up=std::bit_cast<float>(endGroundedUp70_[1]); //2199C8.
            endPhysicsGravityF12_=0u; //2199CC MTC1 zero.
            endGroundedF1_=lazy.words[1]; //2199D4.
            const float threshold=std::bit_cast<float>(*endGroundedF1_);
            endGroundedSlopeStopPC_=0x2199d8;
            if(std::fegetround()!=FE_TONEAREST||!std::isfinite(threshold))continue;
            const bool zeroThreshold=threshold==0.f; //2199D8; up not yet consumed.
            std::uint32_t vehicle=0;endGroundedSlopeStopPC_=0x2199e4;
            //Delay-slot load executes even when the zero-threshold branch wins.
            if(!runtime.readEndGrounded3782199E4(*scope->camera_,slot,vehicle))continue;
            endGroundedF3_=vehicle;
            if(zeroThreshold){endCallerCut_=Cut::physicsGravityStore;continue;}
            const float limit=std::bit_cast<float>(*endGroundedF4_);
            const float base=std::bit_cast<float>(*endGroundedF3_);
            endGroundedSlopeStopPC_=0x2199e8;
            if(!std::isfinite(up))continue;
            volatile float delta=up-threshold;if(!std::isfinite(delta))continue;
            endGroundedSlopeStopPC_=0x2199ec;
            volatile float denominator=0.f-threshold;if(!std::isfinite(denominator))continue;
            endGroundedSlopeStopPC_=0x2199f0;
            if(!std::isfinite(limit)||!std::isfinite(base))continue;
            volatile float span=limit-base;if(!std::isfinite(span))continue;
            endGroundedSlopeStopPC_=0x2199f4;
            volatile float numerator=delta*span;if(!std::isfinite(numerator))continue;
            endGroundedSlopeStopPC_=0x2199f8;
            volatile float quotient=numerator/denominator;if(!std::isfinite(quotient))continue;
            const bool descending=limit<=base; //2199FC, before delay-slot ADD.
            endGroundedSlopeStopPC_=0x219a0c;
            volatile float sum=base+quotient;if(!std::isfinite(sum))continue;
            float value=sum;
            if(descending){
                if(value<limit)value=limit; //219A40..54.
                else if(!(value<=base))value=base; //219A58..68.
            }else{
                if(!(value<=limit))value=limit; //219A10..24.
                else if(value<base)value=base; //219A28..3C.
            }
            endPhysicsGravityF12_=std::bit_cast<std::uint32_t>(value);
            endCallerCut_=Cut::physicsGravityStore;continue;
        }
        if(endCallerCut_==Cut::physicsCandidatesStopped){
            const char* reason=endNativeContactReturned_?
                "Actual native219420 returned;later terrain/source-force cut retained with prior effects":
                endNativeContactStaticsEntered_?
                "Actual native219420 prefix committed;matched bounds/body or source scene cut retained":
                (endNativeSecondarySkipped_||endNativeSecondaryDetectorReturned_)?
                "Actual native secondary branch returned/skipped;219420 still unrepresented":
                endNativeSecondaryBodyBound_?
                "Actual native secondary body/838 bound;secondary detector/query cut retained without replay":
                endNativeLimitedReturned_?
                "Actual scoped native122660/detector returned;219BE4 secondary branch unrepresented":
                endNativeLimitedInitialized_?
                "Actual limited result/count reset committed;candidate cut retained without query replay":
                endNativeQueryPrefixReturned_?
                "Actual native11FF60 preparation/live membership returned;2959F4 query-list branch unrepresented":
                endNativeQueryCachePrepared_?"Native query cache committed; membership/pose cut retained without replay":
                endNativeDetectorEntered_?(endNativeDetectorReturned_?
                    "Native disabled detector returned;actual219BE4 secondary branch unrepresented":
                    "Native detector prefix retained;dirty scene/query preparation remains unsupported"):
                endPhysicsCandidatesMerged_?"Actual native candidate queries/merge returned;detector owner unavailable":
                    "Scoped candidate invocation stopped;no projection refresh or earlier query replay";
            stop(S::incomplete,endPhysicsCandidateStopPC_,reason);return;
        }
        if(endCallerCut_==Cut::physicsWorldPreparation){
            if(!endPhysicsFirstS3_){
                stop(S::incomplete,0x219c24,"Later substep380 world/query branch unrepresented");return;
            }
            const auto scope=endHud_.lock();
            if(endPhysicsIndexS1_<0||!scope||!scope->camera_||
               !scope->endStateCurrent(*this,*scope->camera_)||!(endRuntime=borrowEndRuntimeNative())){
                stop(S::owner,0x219ad4,"Actual same-world scoped candidate owner unavailable");return;
            }
            //One synchronous Bank visit owns actual projection and both query
            //calls. Expiration is terminal for this attempt, even if a later
            //native adapter rejects after query1's effects have committed.
            endCallerCut_=Cut::physicsCandidatesStopped;
            ::ra_source_camera50LA::OwnershipBridge::EndCandidateResult result{};
            //endStateCurrent was checked immediately above, before Bank::visit.
            //Do not repeat its Bank scalar reads inside the live visit. The
            //lexical reader retains that SAME registration and current owners.
            struct EngineEnableContext {
                OriginalRaceConstructor50LA* race;
                decltype(scope) held;
                const ::original_camera_session50KX::Session* session;
                const ::native_ai50IV::Session* manager;
                const ::native_racer50HN::Bank* bank;
            } engineContext{this,scope,scope->camera_,scope->manager_,scope->bank_};
            const ::ra_source_camera50LA::OwnershipBridge::EndEngineEnableReader engineEnable{
                &engineContext,[](const void* raw,bool& enabled)noexcept {
                    const auto& c=*static_cast<const EngineEnableContext*>(raw);
                    auto& race=*c.race;const auto& held=c.held;
                    if(!held||race.endHud_.lock().get()!=held.get()||
                       held->camera_!=c.session||!c.session||
                       held->manager_!=c.manager||held->bank_!=c.bank||!c.manager||!c.bank||
                       !held->active_||held->race_.lock().get()!=&race||
                       held->generation_!=race.entity_.sessionGeneration||
                       !race.counted_||race.rolledBack_||!race.timingB4_)return false;
                    return race.timingB4_->readQueuedEventEnable58(race,*c.manager,*c.bank,enabled);
                },scope->camera_,endPhysicsFactorF20_};
            const bool entered=endRuntime->runEndFirstCandidateQueries(
                *scope->camera_,static_cast<std::size_t>(endPhysicsIndexS1_),result,engineEnable);
            endPhysicsCandidateStopPC_=result.stoppedPC;
            endPhysicsProjectionRevision_=result.projectionRevision;
            endPhysicsFirstCandidateCount_=result.firstCount;endPhysicsSecondCandidateCount_=result.secondCount;
            endPhysicsMergedCount_=result.mergedCount;
            endPhysicsFirstQueryReturned_=result.firstReturned;endPhysicsSecondQueryReturned_=result.secondReturned;
            endPhysicsCandidatesMerged_=result.merged;
            endNativeDetectorEntered_=result.nativeDetectorEntered;
            endNativeDetectorReset_=result.nativeDetectorReset;
            endNativeDetectorReturned_=result.nativeDetectorReturned;
            endNativeQueryCachePrepared_=result.nativeQueryCachePrepared;
            endNativeQueryMembershipCommitted_=result.nativeQueryMembershipCommitted;
            endNativeQueryPrefixReturned_=result.nativeQueryPrefixReturned;
            endNativeQueryMembershipRevision_=result.nativeQueryMembershipRevision;
            endNativeLimitedInitialized_=result.nativeLimitedInitialized;
            endNativeLimitedReturned_=result.nativeLimitedReturned;
            endNativeLimitedOrdinal_=result.nativeLimitedOrdinal;
            endNativeSecondaryBodyBound_=result.nativeSecondaryBodyBound;
            endNativeSecondarySkipped_=result.nativeSecondarySkipped;
            endNativeSecondaryDetectorEntered_=result.nativeSecondaryDetectorEntered;
            endNativeSecondaryDetectorReset_=result.nativeSecondaryDetectorReset;
            endNativeSecondaryDetectorReturned_=result.nativeSecondaryDetectorReturned;
            endNativeSecondaryCachePrepared_=result.nativeSecondaryCachePrepared;
            endNativeSecondaryMembershipCommitted_=result.nativeSecondaryMembershipCommitted;
            endNativeSecondaryQueryPrefixReturned_=result.nativeSecondaryQueryPrefixReturned;
            endNativeSecondaryMembershipRevision_=result.nativeSecondaryMembershipRevision;
            endNativeSecondaryLimitedInitialized_=result.nativeSecondaryLimitedInitialized;
            endNativeSecondaryLimitedReturned_=result.nativeSecondaryLimitedReturned;
            endNativeSecondaryLimitedOrdinal_=result.nativeSecondaryLimitedOrdinal;
            endNativeContactStaticsEntered_=result.nativeContactStaticsEntered;
            endNativeContactScenePrepared_=result.nativeContactScenePrepared;
            endNativeContactReturned_=result.nativeContactReturned;
            endNativeContactPosition_=result.nativeContactPosition;
            endNativeContactForward_=result.nativeContactForward;
            endNativeContactUp_=result.nativeContactUp;
            endNativeContactOrdinal_=result.nativeContactOrdinal;
            endNativeContactMatchedIdentity_=result.nativeContactMatchedIdentity;
            if(!entered){stop(S::owner,0x219ad4,"Concrete scoped materializer borrow unavailable; attempt retained");return;}
            continue;
        }
        if(endCallerCut_>=Cut::physicsEngineFlags&&endCallerCut_<=Cut::physicsEngine38Store){
            const auto scope=endHud_.lock();
            if(endPhysicsIndexS1_<0||!scope||!scope->camera_||
               !scope->endStateCurrent(*this,*scope->camera_)||!(endRuntime=borrowEndRuntimeNative())){
                stop(S::owner,0x219a90,"Actual same-world native engine-field owner unavailable");return;
            }
            auto& runtime=*endRuntime;const auto slot=static_cast<std::size_t>(endPhysicsIndexS1_);
            using Lane=::ra_source_camera50LA::OwnershipBridge::EndNativeEngineLane;
            if(endCallerCut_==Cut::physicsEngineFlags){
                std::uint32_t state64=0;
                if(!runtime.readEndNativeBoost64(*scope->camera_,slot,state64)){
                    stop(S::owner,0x219a90,"Actual registered BoostState64 unavailable");return;
                }
                //Exact219A94..AAC comparisons; not inferred held-boost/thermal
                //input, stale selector flags or a rerun of2176E0.
                endPhysicsEngine34_=(state64==1u||state64==2u)?1u:0u;
                endCallerCut_=Cut::physicsEngine34Store;
            }
            if(endCallerCut_==Cut::physicsEngine34Store){
                if(!endPhysicsEngine34_||!runtime.storeEndNativeEngineLane(
                    *scope->camera_,slot,Lane::engine34,*endPhysicsEngine34_)){
                    stop(S::owner,0x219ab8,"Actual native engine34 store owner unavailable");return;
                }
                endCallerCut_=Cut::physicsB9CRead; //Store34 BEFORE LBU B9C.
            }
            if(endCallerCut_==Cut::physicsB9CRead){
                std::uint8_t flags=0;
                if(!runtime.readEndNativeFlagsB9C(*scope->camera_,slot,flags)){
                    stop(S::owner,0x219abc,"Actual same-racer B9C unavailable after engine34 store");return;
                }
                endPhysicsEngine38_=std::uint32_t((flags>>1u)&1u); //DSLL32/DSRL32 extracts bit1.
                endCallerCut_=Cut::physicsEngine38Store;
            }
            if(!endPhysicsEngine38_||!runtime.storeEndNativeEngineLane(
                *scope->camera_,slot,Lane::engine38,*endPhysicsEngine38_)){
                stop(S::owner,0x219ad0,"Actual native engine38 store owner unavailable; engine34 retained");return;
            }
            endCallerCut_=Cut::physicsWorldPreparation;continue;
        }
        if(endCallerCut_==Cut::physicsBody||endCallerCut_==Cut::physicsGrounded||
           endCallerCut_==Cut::physicsGroundedSpeed||endCallerCut_==Cut::physicsAirborneRead||
           endCallerCut_==Cut::physicsGravityStore){
            const auto scope=endHud_.lock();
            if(endPhysicsIndexS1_<0||!scope||!scope->camera_||
               !scope->endStateCurrent(*this,*scope->camera_)||!(endRuntime=borrowEndRuntimeNative())){
                stop(S::owner,0x21990c,"Actual same-world gravity owner unavailable");return;
            }
            auto& runtime=*endRuntime;const auto slot=static_cast<std::size_t>(endPhysicsIndexS1_);
            if(endCallerCut_==Cut::physicsBody){
                std::uint32_t probe=0;
                if(!runtime.readEndProbe10421990C(*scope->camera_,slot,probe)){
                    stop(S::owner,0x21990c,"Actual current registered348+104 unavailable");return;
                }
                endCallerCut_=probe?Cut::physicsGrounded:Cut::physicsAirborneRead;
                if(probe){
                    endGroundedBrakeF0_.reset();endGroundedSpeedF1_.reset();
                    continue;
                }
            }
            if(endCallerCut_==Cut::physicsGrounded){
                //219920 authors the actual F12 register +zero BEFORE the EC
                //read. This is not a guessed gravity value or probe decision.
                endPhysicsGravityF12_=0u;
                if(!endGroundedBrakeF0_){
                    std::uint32_t bits=0;
                    if(!runtime.readEndGroundedBrake219924(*scope->camera_,slot,bits)){
                        stop(S::owner,0x219924,"Actual same-racer native Aero EC lane unavailable");return;
                    }
                    endGroundedBrakeF0_=bits; //Actual read survives later admission failure.
                }
                const float brake=std::bit_cast<float>(*endGroundedBrakeF0_);
                if(!std::isfinite(brake)){
                    stop(S::source,0x219928,"Grounded EC comparison outside finite scalar domain");return;
                }
                //C.LE.S brake,+0: speed is not consumed on the taken branch.
                endCallerCut_=brake<=0.f?Cut::physicsGroundedSlope:Cut::physicsGroundedSpeed;
                continue;
            }
            if(endCallerCut_==Cut::physicsGroundedSpeed){
                if(!endGroundedSpeedF1_){
                    std::uint32_t bits=0;
                    if(!runtime.readEndGroundedSpeed219948(*scope->camera_,slot,bits)){
                        stop(S::owner,0x219948,"Actual same-racer cached340+34 lane unavailable; EC decision retained");return;
                    }
                    endGroundedSpeedF1_=bits;
                }
                const float speed=std::bit_cast<float>(*endGroundedSpeedF1_);
                if(!std::isfinite(speed)){
                    stop(S::source,0x21994c,"Grounded speed comparison outside finite scalar domain");return;
                }
                if(!(speed<std::bit_cast<float>(std::uint32_t{0x4212aa7f}))){
                    endCallerCut_=Cut::physicsGroundedSlope;continue;
                }
                //21995C jumps to219A80 with the previously authored F12=+0.
                //The exact existing gravity344 target store still executes.
                endCallerCut_=Cut::physicsGravityStore;
            }
            if(endCallerCut_==Cut::physicsAirborneRead){
                std::uint32_t bits=0;
                if(!runtime.readEndAirborne37C219A74(*scope->camera_,slot,bits)){
                    stop(S::owner,0x219a74,"Actual same-Bank Fixture37C provider unavailable");return;
                }
                endPhysicsGravityF12_=bits;endCallerCut_=Cut::physicsGravityStore;
            }
            if(!endPhysicsGravityF12_||!runtime.storeEndAirborneGravity219A88(
                *scope->camera_,slot,*endPhysicsGravityF12_)){
                stop(S::owner,0x219a88,"Actual same-racer gravity344/body340 scalar-store owner unavailable");return;
            }
            endCallerCut_=Cut::physicsEngineFlags;continue;
        }
        if(endCallerCut_==Cut::physicsGhost){
            stop(S::incomplete,0x1a8448,"Saved subtype3 awaits actual B0+40 ghost physics branch; no subtype reread");return;
        }
        if(endCallerCut_==Cut::physicsMusic){
            const auto scope=endHud_.lock();
            if(!scope||!music10C_||!music10C_->nativeBackendCurrent(*scope)){
                stop(S::owner,0x1a8478,"Actual same-Race10C music owner/provider lifetime unavailable");return;
            }
            if(endMusicSource10C_&&music10C_.get()!=endMusicSource10C_){
                stop(S::owner,0x1a847c,"Saved same-invocation music10C target replaced");return;
            }
            endMusicSource10C_=music10C_.get(); //Actual caller target before callee profiling.
            //1A4BB8..1A4C8C: distinct native profile identity, NOT a claim
            //that this manager's original event producers already executed.
            if(!application_->observeNativeSourceProfile50LA(
                   OriginalApplicationEntities50LA::NativeSourceProfileSite50LA::processQueuedEvents)){
                stop(S::owner,0x1a4bb8,"Actual process native profiling registry unavailable");return;
            }
            endCallerCut_=Cut::physicsMusicBody;
        }
        if(endCallerCut_==Cut::physicsMusicBody){
            const auto scope=endHud_.lock();bool enabled=false;
            if(!scope||!scope->camera_||!scope->endStateCurrent(*this,*scope->camera_)||
               !scope->manager_||!scope->bank_||!timingB4_||
               !timingB4_->readQueuedEventEnable58(*this,*scope->manager_,*scope->bank_,enabled)){
                stop(S::owner,0x1a4ca0,"Actual B4 bit1 unavailable at ProcessQueuedEvents");return;
            }
            endCallerCut_=enabled?Cut::musicRaceFlags:Cut::musicClear;
        }
        if(endCallerCut_==Cut::musicRaceFlags){
            if(!(flags1B0KnownMask_&0x10u)){
                stop(S::source,0x1a4cb4,"Actual Race1B0 bit4 unknown");return;
            }
            //1A4CB8/CBC: DSLL32 sa27 then DSRL32 sa31 selects bit4.
            endCallerCut_=(flags1B0KnownValue_&0x10u)?Cut::musicClear:Cut::musicCount;
        }
        if(endCallerCut_==Cut::musicCount){
            const auto scope=endHud_.lock();std::uint32_t count=0;
            if(!scope||!music10C_||!music10C_->readQueuedCount150(*this,*scope,count)){
                stop(S::owner,0x1a4cf0,"Actual current counted10C queue150 unavailable");return;
            }
            endMusicQueueCount150_=count;
            endCallerCut_=count?Cut::musicDispatch:Cut::musicClear;
        }
        if(endCallerCut_==Cut::musicDispatch){
            //Initial source S0=0 at1A4CC8.1A4CF4 is signed SLT S0,count,
            //not an unsigned/nonzero queue test. Consume the SAVED actual150
            //read; a suspended positive count must never be reread as empty.
            if(!endMusicQueueCount150_){
                stop(S::source,0x1a4cf4,"Saved actual queue150 operand absent; no event or clear claimed");return;
            }
            if(std::bit_cast<std::int32_t>(*endMusicQueueCount150_)<=0){
                endCallerCut_=Cut::musicClear;continue;
            }
            const auto scope=endHud_.lock();OriginalRaceMusicManagerDiagnostic50LA dispatched;
            if(!scope||!music10C_||!music10C_->dispatchNativeQueuedEvent3_1A4D30(
                *this,*scope,*endMusicQueueCount150_,dispatched)){
                stop(S::incomplete,dispatched.sourcePC?dispatched.sourcePC:0x1a4cd4,
                    dispatched.operation?dispatched.operation:"Actual same-manager positive FIFO lifetime unavailable");return;
            }
            //Only genuine supported3/6 and bounded1/5 returns advance this saved loop.
            //Unsupported payloads remain owned/pending, never silently dropped.
            endCallerCut_=Cut::musicClear;continue;
        }
        if(endCallerCut_==Cut::musicClear){
            const auto scope=endHud_.lock();
            if(!scope||!music10C_||!music10C_->clearQueuedCount1A5C40(*this,*scope)){
                stop(S::owner,0x1a4d04,"Actual10C queue-clear target lifetime unavailable");return;
            }
            //1A5C40 returned, then1A4D0C..D24 restore registers and return.
            //Reached by actual flag-skip, saved count<=0, or all owned subscriber returns.
            endCallerCut_=Cut::callerReturn;
        }
        if(endCallerCut_==Cut::callerReturn){
            if(!endRootReturnS4_){
                stop(S::source,0x1a8480,"Actual earlier Entity return s4 unavailable");return;
            }
            if(!endCallerReturnV0_)endCallerReturnV0_=*endRootReturnS4_; //1A8480.
            //Remaining1A8484..84AC is saved-register/stack epilogue only.
            //No generic native frame, World reset or pending child call is credited.
            d.callerReturned1A7B40=true;d.callerReturnV0=*endCallerReturnV0_;
            stop(S::callerReturned,0x1a84a8,
                "Bounded same-invocation Race caller returned after actual tail; native profiling adaptation, no live end admission");return;
        }
        if(endCallerCut_==Cut::physicsCount){
            const auto scope=endHud_.lock();std::int32_t count=0;
            if(!scope||!scope->readEndAssociatedCount1A80AC(*this,count)){
                stop(S::owner,0x1a8410,"Actual B0 count30 unavailable at physics loop reload");return;
            }
            endCallerCut_=endPhysicsIndexS1_<count?Cut::physicsGetter:Cut::physicsProfile;
        }
        if(endCallerCut_==Cut::physicsGetter){
            const auto scope=endHud_.lock();std::uint32_t state=0;
            if(endPhysicsIndexS1_<0||!scope||!scope->camera_||
               !scope->endStateCurrent(*this,*scope->camera_)||!(endRuntime=borrowEndRuntimeNative())||
               !endRuntime->readEndPhysicsState40(*scope->camera_,
                   static_cast<std::size_t>(endPhysicsIndexS1_),state)){
                stop(S::owner,0x1a83d0,"Actual same-world indexed racer/state40 unavailable");return;
            }
            if(state!=3u&&state!=4u){
                endCallerCut_=Cut::physicsCall;continue;
            }
            ++endPhysicsIndexS1_; //1A83E4 or1A8408, only genuine states3/4 skip.
            endCallerCut_=Cut::physicsCount;continue;
        }
        if(endCallerCut_==Cut::physicsProfile){
            platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection current;
            if(!profile_||!profile_->courseRaceSnapshot(current)){
                stop(S::profile,0x1a843c,"Actual Profile subtype unavailable after indexed physics loop");return;
            }
            if(current.subtype==3u){
                endCallerCut_=Cut::physicsGhost;continue;
            }
            ++endPhysicsPassS5_; //1A8444 branch delay slot for non3.
            endPhysicsFirstS3_=false; //1A8474 branch delay slot, including final pass.
            if(endPhysicsPassS5_>=endPhysicsCountS2_)endCallerCut_=Cut::physicsMusic;
            else {endPhysicsIndexS1_=0;endCallerCut_=Cut::physicsCount;}
            continue;
        }
        stop(S::source,0x1a8390,"Physics prefix not reached by saved source cut");return;
    }
}

}

namespace original_main_entry50KG {
void Application::continueConsumedRaceEndEntry1A7DD0(
    rr::game::OriginalRaceEndEntryDiagnostic50LA& d)const noexcept {
    d={};
    //Existing immutable Application construction-thread identity, checked
    //before touching its mutable shared Race slot; no new thread/ready token.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()){
        d.operation="Application end prefix requires its construction thread";return;
    }
    const auto race=sourceRace28COwner_;
    if(!enabled||!race||raceConstructorColdOwner50LA_.lock()!=race||
       !entitiesOwner_||race->application_!=entitiesOwner_||
       !profile||!profile->known||!riderProfile||race->profile_!=riderProfile||
       riderProfile->commonProfile()!=profile.get()){
        d.operation="End prefix requires actual Application28C and its same retained Profile";return;
    }
    //PRIVATE AND UNCALLED. This is a concrete source body, not end admission.
    //The existing genuine timer/fade cut is NOT wired until its same Session/
    //canonical F8 state and end-root joins are proven. No supplied flags,
    //times, finished-race booleans, callback or acknowledgment are accepted.
    race->continueEndEntryPrefix1A7DD0(d);
}
}

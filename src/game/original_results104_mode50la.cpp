#include "original_results104_storage50la.h"
#include "../../tests/native/original_results_live_owner_read50la.h"
#include "../../tests/native/original_results_feedback_scope50la.h"

namespace rr::game {
OriginalResults10450LA::ModeCpuPrefix OriginalResults10450LA::setModeCpuPrefix1CA8E0(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    std::uint32_t modeA1,std::uint32_t animateA2,OriginalResults104Diagnostic50LA& d)noexcept {
    using Read=::original_results_roster50LA::Read;
    const auto liveRace=Read::raceAccess(race);
    using P=ModeCpuPrefix;
    d={};
    // Native affinity fence, before touching Impl or the mutable weak
    // registration. Its lifetime is lexical, not a cross-thread device lease.
    if(liveRace.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1ca8e0;return P::none;
    }
    if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1ca8e0;return P::none;}
    auto& p=*impl_;
    if(!constructed()||!p.loadComplete||!p.derived||liveRace.results104_.get()!=this||
       !liveRace.counted_||liveRace.rolledBack_||liveRace.application_!=p.application||
       liveRace.entity_.sessionGeneration!=p.component.sessionGeneration||!retirementCurrent()||
       !liveResultsRaceAuthorityCurrent(race,p.application)){
        p.stop(d,Impl::S::owner,0x1ca8e0,"Results mode prefix requires its actual retained Race owner");return P::none;
    }
    p.modeCpu.reset();p.modeCpu.emplace();auto& out=*p.modeCpu;
    out.requestedMode=modeA1;out.animateArgument=animateA2;
    // The private suspended end body supplies the literal original1AB948
    // arguments(1,0). It is still not called by the live loop. A future caller
    // must use its own reached A1/A2, never use this as UI admission.
    if(modeA1!=p.derived->mode200){
        out.changed=true;p.displayCpu.reset();
        p.derived->previous204=p.derived->mode200; //1CA8EC.
        p.derived->mode200=modeA1;                //1CA8F4.
        p.dirtyLayout208=1u; //1CA8FC delay slot, INCLUDING animateA2==0.
        if(animateA2)p.derived->field390=modeA1==4u?0x3f000000u:0x3e800000u;
    }
    out.prefix=P::scalarPrefix;
    //1CA924..934 is reached even when A1 equals current200. It tailcalls
    //1AF770: for each ACTUAL human device, virtual+40(2) is238870, clearing
    //duration6CC and6D0. It does not change input mode or issue hardware IO.
    out.nextSourcePC=0x1af770;
    const auto tail=continueModeFeedbackCpu1AF770(race,observer,d);
    // An owner rejection in the later tail cannot erase an already reached
    // scalar prefix; the caller must retain that cursor instead of replaying.
    return tail==P::none?P::scalarPrefix:tail;
}

OriginalResults10450LA::ModeCpuPrefix OriginalResults10450LA::continueModeFeedbackCpu1AF770(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    using Read=::original_results_roster50LA::Read;
    const auto liveRace=Read::raceAccess(race);
    using P=ModeCpuPrefix;
    d={};
    if(liveRace.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1af770;return P::none;
    }
    if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1af770;return P::none;}
    auto& p=*impl_;
    if(!constructed()||!p.loadComplete||!p.derived||liveRace.results104_.get()!=this||
       !liveRace.counted_||liveRace.rolledBack_||liveRace.application_!=p.application||
       liveRace.entity_.sessionGeneration!=p.component.sessionGeneration||!retirementCurrent()||
       !liveResultsRaceAuthorityCurrent(race,p.application)||!p.modeCpu||
       p.modeCpu->prefix==P::none){
        p.stop(d,Impl::S::owner,0x1af770,"Results same-owner suspended mode invocation absent");return P::none;
    }
    auto& out=*p.modeCpu;
    if(out.prefix==P::zeroHumanTail||out.prefix==P::humanFeedbackTail){
        // A completed tail belongs to this invocation. Do not clear devices
        // twice if its private caller has not yet advanced its own cursor.
        p.stop(d,Impl::S::incomplete,0x1af7e0,"Results private mode feedback already returned");
        return out.prefix;
    }
    const auto operation=p.loadedRosterOperation.lock();
    if(!Read::loadedOperationOwnerFieldsCurrent(operation,Read::raceWorldIdentity(race),p.application,p.domain,p.component.sessionGeneration)||
       !Read::loadedOperationBankCurrent(operation,Read::observerBankIdentity(observer))){
        p.stop(d,Impl::S::owner,0x1af770,"Results actual manager human-count/feedback ownership absent");return out.prefix;
    }
    // A missing device borrow stops AFTER the first manager34 loop read.
    // Keep that consumed value across retry instead of restarting the setter.
    if(!out.humanCountKnown){
        out.actualHumanCount=Read::operationHumanCount(operation);out.humanCountKnown=true;
    }
    if(out.actualHumanCount){
        out.nextSourcePC=0x1af7a4;
        const auto feedback=liveRace.resultsFeedback_.lock();
        if(!feedback||!feedback->clearHumans1AF770(race,Read::observerBankIdentity(observer),out.actualHumanCount)){
            p.stop(d,Impl::S::owner,0x1af7a4,"Results same-thread actual feedback device borrow absent or retired");
            return out.prefix;
        }
        out.prefix=P::humanFeedbackTail;out.nextSourcePC=0x1af7e0;
        p.stop(d,Impl::S::incomplete,0x1af7e0,"Results private mode feedback returned; live end admission remains absent");
        return out.prefix;
    }
    // Only the actual zero-human owner takes this original zero-iteration
    // tail. It cannot authorize a human Results path or a feedback receipt.
    out.prefix=P::zeroHumanTail;out.nextSourcePC=0x1af7e0;
    p.stop(d,Impl::S::incomplete,0x1af7e0,"Results private zero-human mode tail returned; live end admission remains absent");
    return out.prefix;
}
}

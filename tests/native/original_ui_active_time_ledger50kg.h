#pragma once
#include "../../src/game/lap_host_clock50id.h"
#include "../../platform/linux/native/audio_output50hd.h"
#include <chrono>
#include <cstdint>
#include <limits>
#include <memory>

namespace rr::platform {
struct OriginalUiFrontendInputReceipt50KG;
struct OriginalUiFrontendUpdateReceipt50KG;
}

namespace original_main_entry50KG {
class AudioOutput;
struct Frontend;
// No issuer is supplied by this slice. A future concrete application owner
// must join scheduler/context, immutable image pair and steady transport.
class OriginalUiPresentationAudioLease50KG;

enum class ActiveTimeError50KG : std::uint8_t {
    none,notBound,alreadyBound,clockDomain,backwards,overflow,coldOrder,
    coldDuration,staleTicket,invalidPageReceipt,stall,outputUnavailable,
    outputAccounting,incompleteOutput,poisoned
};
struct ActiveTimeDiagnostic50KG {
    ActiveTimeError50KG error=ActiveTimeError50KG::none;
    const char* operation=nullptr;
    rr::platform::AudioOutputDiagnostic50HD output{};
};
enum class AudioIntervalCause50KG : std::uint8_t {acceptedPage,directionPresentation};
enum class AudioCheckpoint50KG : std::uint8_t {
    none,pumpEntry,preWait,postBoundary,budgetWaitService,finalBudgetComplete
};
enum class AudioAccountingKind50KG : std::uint8_t {none,pcm,explicitMute};
enum class AudioAdvanceResult50KG : std::uint8_t {rejected,noChange,accounted,incomplete};
enum class UiAudioPolicy50KG : std::uint8_t {enabled,explicitMute};
enum class InactiveReason50KG : std::uint8_t {pause,focusLoss,minimized,invalidDrawable,eventBudget,quit};

// Diagnostics only. These public values cannot be submitted as authority.
struct ActiveEndpointObservation50KG {
    const rr::game::HostMonotonicClock50ID* clockIdentity=nullptr;
    std::uint64_t activityEpoch=0,wallNanoseconds=0,activeNanoseconds=0;
};
struct AudioCounterObservation50KG {
    std::uint64_t pcmNanoseconds=0,sourceMixedFrames=0;
    std::uint32_t remainderNanoseconds=0;
    // Successful cumulative microseconds and mixed frames represent the live
    // AudioSampleClock values; no second mutable sample clock is introduced.
    rr::platform::AudioOutputInfo50HD output{};
    // Cumulative Main mixer history remains sourceMixedFrames. Only frames
    // since this actual output-open origin belong to output.mixedFrames.
    std::uint64_t sourceFrameOrigin=0;
};
struct AudioIntervalObservation50KG {
    AudioAccountingKind50KG kind=AudioAccountingKind50KG::none;
    AudioIntervalCause50KG cause=AudioIntervalCause50KG::acceptedPage;
    AudioCheckpoint50KG checkpoint=AudioCheckpoint50KG::none;
    std::uint64_t intervalSequence=0,acceptedPageSequence=0,transitionSerial=0;
    // Boundary identity is nonzero only for an actually completed boundary.
    // preWait/budget service are not successful presentations.
    std::uint64_t presentationSequence=0,deltaNanoseconds=0,accountedNanoseconds=0;
    ActiveEndpointObservation50KG begin{},end{};
    AudioCounterObservation50KG before{},after{};
    rr::platform::AudioOutputDiagnostic50HD diagnostic{};
};
class AudioIntervalReceipt50KG final {
public:
    const AudioIntervalObservation50KG& observation()const noexcept{return observation_;}
    bool accountsPcm()const noexcept{return observation_.kind==AudioAccountingKind50KG::pcm;}
    bool accountsExplicitMute()const noexcept{return observation_.kind==AudioAccountingKind50KG::explicitMute;}
private:
    friend class AudioOutput;
    friend class OriginalUiActiveTimeLedger50KG;
    std::weak_ptr<const std::uint8_t> ledgerLifetime_,outputLifetime_;
    AudioIntervalObservation50KG observation_{};
};
struct IncompleteAudioAttempt50KG {
    bool present=false,outputCallMade=false,outputReturnedSuccess=false,countersObserved=false;
    AudioIntervalObservation50KG attempted{}; // kind remains none, never PCM/mute success
    ActiveTimeDiagnostic50KG diagnostic{};
};
struct ActiveTimeExclusionReceipt50KG {
    bool recorded=false;
    InactiveReason50KG reason=InactiveReason50KG::pause;
    ActiveEndpointObservation50KG before{},after{};
    std::uint64_t sequence=0,wallNanoseconds=0,coldNanoseconds=0,lastColdSequence=0;
};
class PageIntervalTicket50KG final {
public:
    std::uint64_t pageNanoseconds()const noexcept{return pageNanoseconds_;}
    const ActiveEndpointObservation50KG& endpoint()const noexcept{return endpoint_;}
private:
    friend class OriginalUiActiveTimeLedger50KG;
    std::weak_ptr<const std::uint8_t> lifetime_;
    ActiveEndpointObservation50KG endpoint_{};
    std::uint64_t revision_=0,coldSequence_=0,coldNanoseconds_=0,pageNanoseconds_=0;
};

// DORMANT application-private core. Frontend owns the sole instance; no page,
// session, renderer, callback or arbitrary elapsed-time caller can issue a
// ticket. The binding and PCM completion definitions live with AudioOutput.
// Accepted-page/cold/presentation owner bridges below are deliberately DECLARED
// ONLY. Adding their actual source receipts/admission is a separate slice.
class OriginalUiActiveTimeLedger50KG final {
public:
    OriginalUiActiveTimeLedger50KG()=default;
    OriginalUiActiveTimeLedger50KG(const OriginalUiActiveTimeLedger50KG&)=delete;
    OriginalUiActiveTimeLedger50KG& operator=(const OriginalUiActiveTimeLedger50KG&)=delete;
    OriginalUiActiveTimeLedger50KG(OriginalUiActiveTimeLedger50KG&&)=delete;
    OriginalUiActiveTimeLedger50KG& operator=(OriginalUiActiveTimeLedger50KG&&)=delete;
    bool poisoned()const noexcept{return poisoned_;}
    std::uint64_t activeNanoseconds()const noexcept{return endpoint_.activeNanoseconds;}
    std::uint64_t lastPageUpdateEndpoint()const noexcept{return lastPageUpdateEndpoint_;}
    bool pageEndpointKnown()const noexcept{return pageEndpointKnown_;}
    bool acceptedPageAwaitingAudio()const noexcept{return acceptedPagePending_;}
    std::uint64_t lastAcceptedPageSequence()const noexcept{return lastAcceptedPageSequence_;}
    std::uint64_t lastAudioEndpoint()const noexcept{return lastAudioEndpoint_;}
    std::uint64_t audioIntervalSequence()const noexcept{return audioIntervalSequence_;}
    const AudioIntervalReceipt50KG& lastSuccessfulAudioReceipt()const noexcept{return lastSuccessful_;}
    const IncompleteAudioAttempt50KG& incompleteAudioAttempt()const noexcept{return incomplete_;}
private:
    friend struct Frontend;
    friend class AudioOutput;
    using Clock=std::chrono::steady_clock;
    // This native-only primitive admits the actual Linux nanosecond clock,
    // rather than hiding an unchecked duration_cast or estimating an offset.
    static_assert(Clock::period::num==1&&Clock::period::den==1000000000,
        "UI active-time ledger requires native nanosecond steady_clock");
    static_assert(std::numeric_limits<Clock::rep>::is_integer&&
        std::numeric_limits<Clock::rep>::digits<=std::numeric_limits<std::uint64_t>::digits,
        "UI active-time ledger requires representable native clock ticks");
    struct ProjectedInterval {
        ActiveEndpointObservation50KG endpoint{};
        std::uint64_t revision=0,coldSequence=0,coldNanoseconds=0;
    };
    struct AdmittedInterval {
        std::weak_ptr<const std::uint8_t> ledgerLifetime,outputLifetime;
        AudioIntervalObservation50KG observation{};
    };
    bool bind(const rr::game::HostMonotonicClock50ID&,AudioOutput&,UiAudioPolicy50KG,
        ActiveTimeDiagnostic50KG&);
    bool recordCold(const rr::platform::OriginalUiFrontendInputReceipt50KG&,
        ActiveTimeDiagnostic50KG&) noexcept;
    AudioAdvanceResult50KG acceptPageInterval(const PageIntervalTicket50KG&,
        const rr::platform::OriginalUiFrontendUpdateReceipt50KG&,
        AudioIntervalReceipt50KG&,ActiveTimeDiagnostic50KG&) noexcept;
    AudioAdvanceResult50KG advancePresentationAudio(const OriginalUiPresentationAudioLease50KG&,
        AudioCheckpoint50KG,AudioIntervalReceipt50KG&,ActiveTimeDiagnostic50KG&) noexcept;

    static bool fail(ActiveTimeDiagnostic50KG& d,ActiveTimeError50KG e,const char* op) noexcept {
        d.error=e;d.operation=op;return false;
    }
    static bool addable(std::uint64_t a,std::uint64_t b) noexcept {
        return b<=std::numeric_limits<std::uint64_t>::max()-a;
    }
    bool usable(ActiveTimeDiagnostic50KG& d)const noexcept {
        if(poisoned_)return fail(d,ActiveTimeError50KG::poisoned,"UI active-time fault latched");
        if(!clock_||!output_||!lifetime_||outputLifetime_.expired())
            return fail(d,ActiveTimeError50KG::notBound,"UI active-time/output lifetime");
        if(clock_->epoch!=clockEpoch_)return fail(d,ActiveTimeError50KG::clockDomain,"UI application epoch changed");
        return true;
    }
    bool rawAt(Clock::time_point point,std::uint64_t& ns,ActiveTimeDiagnostic50KG& d)const noexcept {
        if(!clock_||clock_->epoch!=clockEpoch_)
            return fail(d,ActiveTimeError50KG::clockDomain,"UI raw application epoch");
        const auto origin=clockEpoch_.time_since_epoch().count(),value=point.time_since_epoch().count();
        // Nonnegative same-domain ticks make subtraction representable in the
        // native signed rep. Reject an unsupported origin before subtracting.
        if(origin<0||value<origin)return fail(d,ActiveTimeError50KG::backwards,"UI raw sample before epoch");
        const auto distance=value-origin;
        if(static_cast<std::uintmax_t>(distance)>std::numeric_limits<std::uint64_t>::max())
            return fail(d,ActiveTimeError50KG::overflow,"UI raw nanoseconds");
        ns=static_cast<std::uint64_t>(distance);return true;
    }
    bool project(Clock::time_point now,bool active,ProjectedInterval& out,
        ActiveTimeDiagnostic50KG& d)const noexcept {
        if(!usable(d))return false;
        if(acceptedPagePending_)return fail(d,ActiveTimeError50KG::staleTicket,"UI accepted page awaits PCM");
        std::uint64_t raw=0;if(!rawAt(now,raw,d))return false;
        if(raw<endpoint_.wallNanoseconds||raw<clock_->lastNanoseconds)
            return fail(d,ActiveTimeError50KG::backwards,"UI observation order");
        const auto wall=raw-endpoint_.wallNanoseconds;
        if(pendingColdNanoseconds_>wall||(coldPending_&&raw<lastColdEnd_))
            return fail(d,ActiveTimeError50KG::coldDuration,"UI cold exclusion exceeds observation");
        const auto delta=active&&previousActive_?wall-pendingColdNanoseconds_:0;
        if(!addable(endpoint_.activeNanoseconds,delta)||!addable(revision_,1)||
            (active!=previousActive_&&!addable(endpoint_.activityEpoch,1)))
            return fail(d,ActiveTimeError50KG::overflow,"UI observation counters");
        out.endpoint={clock_,endpoint_.activityEpoch+(active!=previousActive_?1u:0u),raw,
            endpoint_.activeNanoseconds+delta};
        out.revision=revision_;out.coldSequence=coldSequence_;out.coldNanoseconds=pendingColdNanoseconds_;
        return true;
    }
    bool preparePageInterval(PageIntervalTicket50KG& ticket,ActiveTimeDiagnostic50KG& d)const noexcept {
        d={};ticket={};ProjectedInterval p;
        if(!project(Clock::now(),true,p,d))return false;
        if(lastAcceptedPageSequence_==std::numeric_limits<std::uint64_t>::max())
            return fail(d,ActiveTimeError50KG::overflow,"UI accepted page sequence");
        if(pageEndpointKnown_&&p.endpoint.activeNanoseconds<lastPageUpdateEndpoint_)
            return fail(d,ActiveTimeError50KG::backwards,"UI accepted page endpoint");
        const auto elapsed=pageEndpointKnown_&&previousActive_?
            p.endpoint.activeNanoseconds-lastPageUpdateEndpoint_:0;
        // Intervening PCM checkpoints NEVER move this page baseline.
        if(elapsed>1000000000ull)return fail(d,ActiveTimeError50KG::stall,"UI accepted page interval");
        if(p.endpoint.activeNanoseconds<lastAudioEndpoint_||
            p.endpoint.activeNanoseconds-lastAudioEndpoint_>1000000000ull)
            return fail(d,ActiveTimeError50KG::stall,"UI unconsumed PCM interval");
        if(p.endpoint.activeNanoseconds!=lastAudioEndpoint_&&
            audioIntervalSequence_==std::numeric_limits<std::uint64_t>::max())
            return fail(d,ActiveTimeError50KG::overflow,"UI audio interval sequence");
        ticket.lifetime_=lifetime_;ticket.endpoint_=p.endpoint;ticket.revision_=p.revision;
        ticket.coldSequence_=p.coldSequence;ticket.coldNanoseconds_=p.coldNanoseconds;
        ticket.pageNanoseconds_=elapsed;return true;
    }
    // Called ONLY by the future concrete recordCold(actual input receipt)
    // bridge. Preserve the existing measured duration even for failed loads.
    bool recordMeasuredCold(Clock::time_point begin,Clock::time_point end,std::uint64_t measured,
        ActiveTimeDiagnostic50KG& d) noexcept {
        d={};if(!usable(d))return false;
        if(acceptedPagePending_)return fail(d,ActiveTimeError50KG::staleTicket,"UI cold receipt while page awaits PCM");
        std::uint64_t b=0,e=0;if(!rawAt(begin,b,d)||!rawAt(end,e,d))return false;
        if(e<b||b<endpoint_.wallNanoseconds||(coldKnown_&&b<lastColdEnd_)||
            (coldKnown_&&b==lastColdBegin_&&e==lastColdEnd_))
            return fail(d,ActiveTimeError50KG::coldOrder,"UI serialized cold receipt");
        if(measured!=e-b)return fail(d,ActiveTimeError50KG::coldDuration,"UI measured cold duration");
        if(!addable(pendingColdNanoseconds_,measured)||!addable(coldSequence_,1)||!addable(revision_,1))
            return fail(d,ActiveTimeError50KG::overflow,"UI cold counters");
        pendingColdNanoseconds_+=measured;lastColdBegin_=b;lastColdEnd_=e;
        coldKnown_=true;coldPending_=true;++coldSequence_;++revision_;return true;
    }
    bool ticketCurrent(const PageIntervalTicket50KG& t,ActiveTimeDiagnostic50KG& d)const noexcept {
        if(!usable(d))return false;
        if(acceptedPagePending_||t.lifetime_.lock()!=lifetime_||t.revision_!=revision_||t.coldSequence_!=coldSequence_||
            t.coldNanoseconds_!=pendingColdNanoseconds_||t.endpoint_.clockIdentity!=clock_)
            return fail(d,ActiveTimeError50KG::staleTicket,"UI accepted page ticket");
        return true;
    }
    // The actual accepted receipt bridge must validate accepted=true and its
    // exact next sequence BEFORE this call; a caller-written success is not a
    // public API. Keep this real page fact even if the subsequent output fails.
    bool acknowledgeAcceptedPage(const PageIntervalTicket50KG& t,std::uint64_t actualSequence,
        ActiveTimeDiagnostic50KG& d) noexcept {
        if(!ticketCurrent(t,d))return false;
        if(lastAcceptedPageSequence_==std::numeric_limits<std::uint64_t>::max()||
            actualSequence!=lastAcceptedPageSequence_+1)
            return fail(d,ActiveTimeError50KG::invalidPageReceipt,"UI actual accepted page sequence");
        lastAcceptedPageSequence_=actualSequence;lastPageUpdateEndpoint_=t.endpoint_.activeNanoseconds;
        pageEndpointKnown_=true;acceptedPagePending_=true;return true;
    }
    AudioAdvanceResult50KG finishCheckpoint(const ProjectedInterval&,AudioIntervalCause50KG,
        AudioCheckpoint50KG,std::uint64_t transitionSerial,std::uint64_t presentationSequence,
        AudioIntervalReceipt50KG&,ActiveTimeDiagnostic50KG&) noexcept;
    void commitObservation(const ProjectedInterval& p,bool active) noexcept {
        endpoint_=p.endpoint;previousActive_=active;pendingColdNanoseconds_=0;coldPending_=false;++revision_;
    }
    bool observeInactive(InactiveReason50KG reason,ActiveTimeExclusionReceipt50KG& receipt,
        ActiveTimeDiagnostic50KG& d) noexcept {
        d={};receipt={};ProjectedInterval p;if(!project(Clock::now(),false,p,d))return false;
        if(!addable(exclusionSequence_,1))return fail(d,ActiveTimeError50KG::overflow,"UI exclusion sequence");
        receipt={true,reason,endpoint_,p.endpoint,exclusionSequence_+1,
            p.endpoint.wallNanoseconds-endpoint_.wallNanoseconds,p.coldNanoseconds,p.coldSequence};
        ++exclusionSequence_;commitObservation(p,false);pageEndpointKnown_=false;return true;
    }
    // A future real interruption bridge must call the actual pause/flush owner
    // and retire/poison its presentation lease. This core does neither.
    const rr::game::HostMonotonicClock50ID* clock_=nullptr;
    Clock::time_point clockEpoch_{};
    AudioOutput* output_=nullptr;
    std::shared_ptr<const std::uint8_t> lifetime_;
    std::weak_ptr<const std::uint8_t> outputLifetime_;
    ActiveEndpointObservation50KG endpoint_{},lastAudioObservation_{};
    bool previousActive_=false,pageEndpointKnown_=false,acceptedPagePending_=false,poisoned_=false;
    bool coldKnown_=false,coldPending_=false;
    std::uint64_t revision_=0,lastPageUpdateEndpoint_=0,lastAcceptedPageSequence_=0;
    std::uint64_t lastAudioEndpoint_=0,audioIntervalSequence_=0,exclusionSequence_=0;
    std::uint64_t pendingColdNanoseconds_=0,coldSequence_=0,lastColdBegin_=0,lastColdEnd_=0;
    AudioIntervalReceipt50KG lastSuccessful_{};
    IncompleteAudioAttempt50KG incomplete_{};
};
}

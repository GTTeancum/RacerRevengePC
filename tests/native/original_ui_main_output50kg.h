#pragma once
#include "original_ui_active_time_ledger50kg.h"
#include "../../platform/linux/native/original_ui_main_audio50kg.h"
#include "../../platform/linux/native/audio_output50hd.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include <array>
#include <limits>
#include <memory>
#include <string>
#include <utility>

namespace original_main_entry50KG {
// Dedicated frontend device/WAV ownership. No reference to a race mixer, live
// owner or race voice is accepted. Close this owner BEFORE any future race
// output opens. Main currently has no admitted gameplay destination.
class AudioOutput final {
public:
    AudioOutput() = default;
    AudioOutput(const AudioOutput&)=delete;
    AudioOutput& operator=(const AudioOutput&)=delete;
    AudioOutput(AudioOutput&&)=delete;
    AudioOutput& operator=(AudioOutput&&)=delete;
    ~AudioOutput() noexcept {
        rr::platform::AudioOutputDiagnostic50HD diagnostic;close(diagnostic);
    }
    bool prepare(const std::filesystem::path& root,
        const rr::platform::AudioOutputRequest50HD& request,std::string& error,
        rr::platform::AudioOutputDiagnostic50HD& diagnostic,
        std::shared_ptr<rr::platform::OriginalSoundMode50KG> soundMode={}) {
        if(attempted_ || closed_ || typedBound_){error="Main output requires a fresh stable owner";return false;}
        attempted_=true;
        if(!mixer_.initialize(voices_.data(),voices_.size(),request.sampleRate)){
            error="Main dedicated mixer initialization failed";return false;
        }
        if(soundMode&&!mixer_.bindOriginalSoundMode50KG(std::move(soundMode))){
            error="Main source sound-mode consumer binding failed";return false;
        }
        transport_=std::make_shared<rr::platform::OriginalUiMainAudio50KG>();
        if(!transport_->prepare(root,mixer_,error))return false;
        if(!output_.open(request,diagnostic)){
            error=std::string("Main output open failed: ")+diagnostic.detail.data();return false;
        }
        enabled_=true;error.clear();return true;
    }
    std::shared_ptr<rr::platform::OriginalUiMainAudio50KG> transport() const noexcept {
        return transport_;
    }
    bool enabled() const noexcept { return enabled_; }
    // Checks this dedicated output's exact prepared transport and mixer. A
    // muted/closed output has no capability; mode storage alone is insufficient.
    const rr::platform::OriginalSoundMode50KG* soundModeOwner50KG() const noexcept {
        return enabled_&&!closed_&&transport_?transport_->soundModeOwner50KG():nullptr;
    }
    bool canApplySoundMode18874050KG(std::uint32_t mode) const noexcept {
        return enabled_&&!closed_&&!advancementFault_&&transport_&&transport_->canApplySoundMode18874050KG(mode);
    }
    bool applySoundMode18874050KG(std::uint32_t mode) noexcept {
        return canApplySoundMode18874050KG(mode)&&transport_->applySoundMode18874050KG(mode);
    }
    void pause(bool value) noexcept {
        if(!enabled_ || closed_)return;
        if(advancementFault_&&!value)return; // Faulted output permits pause/close, never replay/resume.
        transport_->pause(value);
        output_.pauseDevice(value); // Flush stale device PCM; retain source cursor/WAV.
    }
    // The SAME elapsed nanosecond interval supplied to the accepted source
    // update. Carry fractional microseconds rather than round every frame.
    // Paused wall time is excluded by the caller; no render-rate resampling.
    bool advanceAcceptedNanoseconds(std::uint64_t nanoseconds,
        rr::platform::AudioOutputDiagnostic50HD& diagnostic) noexcept {
        diagnostic={};
        // A future typed binding is exclusive and one-way. Existing callers
        // retain the exact legacy accounting until that private bind happens.
        if(typedBound_||advancementFault_){
            diagnostic.error=rr::platform::AudioOutputError50HD::Clock;
            diagnostic.operation="Main output typed owner or advancement fault";return false;
        }
        legacyAdvanced_=true;
        if(!enabled_)return true; // Explicit mute owns no output or PCM transport.
        if(closed_ || !transport_ || transport_->paused() || nanoseconds>1000000000ull ||
            nanoseconds>std::numeric_limits<std::uint64_t>::max()-acceptedNanoseconds_){
            diagnostic.error=rr::platform::AudioOutputError50HD::Clock;
            diagnostic.operation="Main output accepted interval";return false;
        }
        const auto carried=nanoseconds+remainderNanoseconds_;
        if(!output_.advanceMicroseconds(*transport_,static_cast<std::uint32_t>(carried/1000),diagnostic))
            return false;
        remainderNanoseconds_=static_cast<std::uint32_t>(carried%1000);
        acceptedNanoseconds_+=nanoseconds;
        return true;
    }
    rr::platform::AudioOutputInfo50HD info() const noexcept { return output_.info(); }
    std::uint64_t sourceMixedFrames() const noexcept { return mixer_.mixedFrames(); }
    std::uint64_t acceptedNanoseconds() const noexcept { return acceptedNanoseconds_; }
    std::uint32_t remainderNanoseconds() const noexcept { return remainderNanoseconds_; }
    bool close(rr::platform::AudioOutputDiagnostic50HD& diagnostic) noexcept {
        const bool result=output_.close(diagnostic); // Joins callback/finalizes WAV first.
        if(transport_)transport_->reset(); // Stops only our own frontend voices.
        closed_=true;return result;
    }
private:
    friend class OriginalUiActiveTimeLedger50KG;
    AudioCounterObservation50KG counterObservation()const noexcept {
        return {acceptedNanoseconds_,mixer_.mixedFrames(),remainderNanoseconds_,output_.info()};
    }
    // Only the ledger can construct this typed context. It does not itself
    // certify Song7, event2, a swap or a steady presentation lease.
    AudioAdvanceResult50KG advanceAdmittedNanoseconds(
        const OriginalUiActiveTimeLedger50KG::AdmittedInterval& interval,
        AudioIntervalReceipt50KG& receipt,IncompleteAudioAttempt50KG& incomplete,
        ActiveTimeDiagnostic50KG& d) noexcept {
        using Error=ActiveTimeError50KG;
        using Result=AudioAdvanceResult50KG;
        d={};receipt={};incomplete={};
        auto reject=[&](Error error,const char* operation) noexcept {
            d.error=error;d.operation=operation;return Result::rejected;
        };
        const auto& requested=interval.observation;
        if(advancementFault_)return reject(Error::poisoned,"UI typed output fault latched");
        const auto ledger=typedLedgerLifetime_.lock();
        if(!typedBound_||!ledger||interval.ledgerLifetime.lock()!=ledger||
            interval.outputLifetime.lock()!=typedOutputLifetime_||closed_)
            return reject(Error::outputUnavailable,"UI typed output lifetime");
        if(requested.begin.clockIdentity!=typedClock_||requested.end.clockIdentity!=typedClock_||
            requested.end.wallNanoseconds<requested.begin.wallNanoseconds||
            requested.end.activityEpoch<requested.begin.activityEpoch||
            requested.begin.activeNanoseconds!=typedAccountedNanoseconds_||
            requested.end.activeNanoseconds<requested.begin.activeNanoseconds)
            return reject(Error::clockDomain,"UI typed interval endpoints");
        const auto nanoseconds=requested.end.activeNanoseconds-requested.begin.activeNanoseconds;
        if(nanoseconds!=requested.deltaNanoseconds||nanoseconds>1000000000ull||
            !OriginalUiActiveTimeLedger50KG::addable(typedAccountedNanoseconds_,nanoseconds)||
            requested.accountedNanoseconds!=typedAccountedNanoseconds_+nanoseconds)
            return reject(Error::stall,"UI typed bounded interval");
        if((nanoseconds&&typedIntervalSequence_==std::numeric_limits<std::uint64_t>::max())||
            requested.intervalSequence!=typedIntervalSequence_+(nanoseconds?1u:0u))
            return reject(Error::overflow,"UI typed interval sequence");
        auto observed=requested;observed.kind=AudioAccountingKind50KG::none;
        observed.before=counterObservation();
        if(typedMuted_){
            if(enabled_||attempted_||transport_||observed.before.pcmNanoseconds||
                observed.before.sourceMixedFrames||observed.before.remainderNanoseconds||
                observed.before.output.simulationMicroseconds||observed.before.output.mixedFrames||
                observed.before.output.capturedFrames||observed.before.output.captureActive)
                return reject(Error::outputAccounting,"UI explicit mute owns no PCM output");
        }else{
            if(!enabled_||!transport_||!transport_->ready()||transport_->paused()||
                observed.before.output.paused||transport_->sampleRate()!=observed.before.output.sampleRate)
                return reject(Error::outputUnavailable,"UI typed live transport");
            if(acceptedNanoseconds_!=typedAccountedNanoseconds_||remainderNanoseconds_>=1000||
                remainderNanoseconds_!=acceptedNanoseconds_%1000||
                observed.before.output.simulationMicroseconds!=acceptedNanoseconds_/1000||
                observed.before.sourceMixedFrames!=observed.before.output.mixedFrames)
                return reject(Error::outputAccounting,"UI typed prior successful counters");
        }
        if(!nanoseconds)return Result::noChange; // No call, sequence or fabricated PCM receipt.
        if(!typedMuted_){
            if(!OriginalUiActiveTimeLedger50KG::addable(nanoseconds,remainderNanoseconds_)||
                !OriginalUiActiveTimeLedger50KG::addable(acceptedNanoseconds_,nanoseconds))
                return reject(Error::overflow,"UI typed nanosecond carry");
            const auto carried=nanoseconds+remainderNanoseconds_;
            const auto microseconds=carried/1000;
            const auto rate=observed.before.output.sampleRate;
            if(!rate||microseconds>1000000||microseconds>std::numeric_limits<std::uint32_t>::max()||
                !OriginalUiActiveTimeLedger50KG::addable(observed.before.output.simulationMicroseconds,microseconds)||
                !OriginalUiActiveTimeLedger50KG::addable(observed.before.sourceMixedFrames,rate)||
                !OriginalUiActiveTimeLedger50KG::addable(observed.before.output.mixedFrames,rate)||
                !OriginalUiActiveTimeLedger50KG::addable(observed.before.output.capturedFrames,rate))
                return reject(Error::overflow,"UI typed output counter preflight");
            rr::platform::AudioOutputDiagnostic50HD outputDiagnostic;
            // Irreversible: even false may consume the sample clock, voice
            // prefix, complete WAV blocks, partial bytes and device queue.
            const bool ok=output_.advanceMicroseconds(*transport_,static_cast<std::uint32_t>(microseconds),outputDiagnostic);
            observed.after=counterObservation();observed.diagnostic=outputDiagnostic;
            const bool countersOk=ok&&
                observed.after.output.sampleRate==rate&&
                observed.after.output.simulationMicroseconds==observed.before.output.simulationMicroseconds+microseconds&&
                observed.after.sourceMixedFrames>=observed.before.sourceMixedFrames&&
                observed.after.output.mixedFrames>=observed.before.output.mixedFrames&&
                observed.after.sourceMixedFrames-observed.before.sourceMixedFrames==
                    observed.after.output.mixedFrames-observed.before.output.mixedFrames&&
                observed.after.output.capturedFrames>=observed.before.output.capturedFrames&&
                (observed.before.output.captureActive?
                    observed.after.output.capturedFrames-observed.before.output.capturedFrames==
                        observed.after.output.mixedFrames-observed.before.output.mixedFrames:
                    observed.after.output.capturedFrames==observed.before.output.capturedFrames);
            if(!countersOk){
                advancementFault_=true;
                d.error=ok?Error::outputAccounting:Error::incompleteOutput;
                d.operation="UI typed irreversible output attempt";d.output=outputDiagnostic;
                incomplete={true,true,ok,true,observed,d};return Result::incomplete;
            }
            // Preserve the existing TWO-stage policy. AudioOutput's actual
            // sample clock still carries microseconds*rate modulo 1,000,000.
            // Never replace it with floor(total nanoseconds*rate/1e9).
            remainderNanoseconds_=static_cast<std::uint32_t>(carried%1000);
            acceptedNanoseconds_+=nanoseconds;
            observed.after.pcmNanoseconds=acceptedNanoseconds_;
            observed.after.remainderNanoseconds=remainderNanoseconds_;
            observed.kind=AudioAccountingKind50KG::pcm;
        }else{
            observed.after=counterObservation();observed.kind=AudioAccountingKind50KG::explicitMute;
        }
        typedAccountedNanoseconds_+=nanoseconds;++typedIntervalSequence_;
        receipt.ledgerLifetime_=ledger;receipt.outputLifetime_=typedOutputLifetime_;
        receipt.observation_=observed;return Result::accounted;
    }
    // Reverse destruction: output, transport/PCM, mixer, borrowed voice array.
    std::array<rr::platform::AudioVoice50HD,
        rr::platform::OriginalUiMainAudio50KG::requiredVoices> voices_{};
    rr::platform::AudioMixer50HD mixer_;
    std::shared_ptr<rr::platform::OriginalUiMainAudio50KG> transport_;
    rr::platform::AudioOutput50HD output_;
    bool attempted_=false,enabled_=false,closed_=false;
    std::uint64_t acceptedNanoseconds_=0;
    std::uint32_t remainderNanoseconds_=0;
    // Dormant until the application explicitly binds its private ledger.
    // No ordinary path creates these lifetimes or selects typed mode.
    bool legacyAdvanced_=false,typedBound_=false,typedMuted_=false,advancementFault_=false;
    std::weak_ptr<const std::uint8_t> typedLedgerLifetime_;
    std::shared_ptr<const std::uint8_t> typedOutputLifetime_;
    const rr::game::HostMonotonicClock50ID* typedClock_=nullptr;
    std::uint64_t typedAccountedNanoseconds_=0,typedIntervalSequence_=0;
};

inline bool OriginalUiActiveTimeLedger50KG::bind(const rr::game::HostMonotonicClock50ID& clock,
    AudioOutput& output,UiAudioPolicy50KG policy,ActiveTimeDiagnostic50KG& d) {
    d={};
    if(clock_||lifetime_||output.typedBound_)return fail(d,ActiveTimeError50KG::alreadyBound,"UI ledger stable binding");
    if(policy!=UiAudioPolicy50KG::enabled&&policy!=UiAudioPolicy50KG::explicitMute)
        return fail(d,ActiveTimeError50KG::outputUnavailable,"UI ledger explicit audio policy");
    const bool muted=policy==UiAudioPolicy50KG::explicitMute;
    const auto before=output.counterObservation();
    if(output.closed_||output.legacyAdvanced_||output.advancementFault_||before.pcmNanoseconds||
        before.remainderNanoseconds||before.sourceMixedFrames||before.output.simulationMicroseconds||
        before.output.mixedFrames||before.output.capturedFrames||
        (muted?(output.attempted_||output.enabled_||bool(output.transport_)):
            (!output.enabled_||!output.transport_||!output.transport_->ready()||output.transport_->paused()||before.output.paused)))
        return fail(d,ActiveTimeError50KG::outputUnavailable,"UI ledger requires fresh explicit audio policy");
    const auto now=Clock::now();
    const auto origin=clock.epoch.time_since_epoch().count(),value=now.time_since_epoch().count();
    if(origin<0||value<origin)return fail(d,ActiveTimeError50KG::clockDomain,"UI ledger application origin");
    const auto raw=static_cast<std::uint64_t>(value-origin);
    if(raw<clock.lastNanoseconds)return fail(d,ActiveTimeError50KG::backwards,"UI ledger initial observation");
    // Cold, one-time storage allocation only. Failure cannot partially bind.
    auto lifetime=std::make_shared<const std::uint8_t>(0);
    auto outputLifetime=std::make_shared<const std::uint8_t>(0);
    clock_=&clock;clockEpoch_=clock.epoch;output_=&output;lifetime_=std::move(lifetime);
    outputLifetime_=outputLifetime;endpoint_={&clock,1,raw,0};lastAudioObservation_=endpoint_;
    output.typedBound_=true;output.typedMuted_=muted;output.typedLedgerLifetime_=lifetime_;
    output.typedOutputLifetime_=std::move(outputLifetime);output.typedClock_=&clock;return true;
}

inline AudioAdvanceResult50KG OriginalUiActiveTimeLedger50KG::finishCheckpoint(
    const ProjectedInterval& projected,AudioIntervalCause50KG cause,AudioCheckpoint50KG checkpoint,
    std::uint64_t transitionSerial,std::uint64_t presentationSequence,
    AudioIntervalReceipt50KG& receipt,ActiveTimeDiagnostic50KG& d) noexcept {
    using Result=AudioAdvanceResult50KG;
    d={};receipt={};
    auto rejected=[&]() noexcept {
        // A rejected retry must not erase the first irreversible attempt.
        if(poisoned_&&incomplete_.present)return Result::rejected;
        if(!acceptedPagePending_)return Result::rejected;
        poisoned_=true;incomplete_={};incomplete_.present=true;incomplete_.diagnostic=d;
        auto& attempted=incomplete_.attempted;
        attempted.cause=cause;attempted.checkpoint=checkpoint;
        attempted.acceptedPageSequence=lastAcceptedPageSequence_;
        attempted.transitionSerial=transitionSerial;attempted.presentationSequence=presentationSequence;
        attempted.begin=lastAudioObservation_;attempted.end=projected.endpoint;
        if(output_&&!outputLifetime_.expired()){
            output_->advancementFault_=true;incomplete_.countersObserved=true;
            attempted.before=output_->counterObservation();attempted.after=attempted.before;
        }
        return Result::incomplete;
    };
    if(!usable(d))return rejected();
    if(projected.revision!=revision_||projected.coldSequence!=coldSequence_||
        projected.coldNanoseconds!=pendingColdNanoseconds_||
        projected.endpoint.clockIdentity!=clock_||projected.endpoint.activeNanoseconds<lastAudioEndpoint_||
        projected.endpoint.wallNanoseconds<endpoint_.wallNanoseconds||
        !addable(revision_,1)||
        (cause==AudioIntervalCause50KG::acceptedPage?
            (!acceptedPagePending_||projected.endpoint.activeNanoseconds!=lastPageUpdateEndpoint_||
                checkpoint!=AudioCheckpoint50KG::none||transitionSerial||presentationSequence):
            (acceptedPagePending_||!previousActive_||!pageEndpointKnown_||!transitionSerial||
                checkpoint==AudioCheckpoint50KG::none||
                ((checkpoint==AudioCheckpoint50KG::postBoundary)!=(presentationSequence!=0))))) {
        fail(d,ActiveTimeError50KG::staleTicket,"UI checkpoint projection/owner cause");return rejected();
    }
    const auto delta=projected.endpoint.activeNanoseconds-lastAudioEndpoint_;
    if(delta>1000000000ull||(delta&&!addable(audioIntervalSequence_,1))){
        fail(d,ActiveTimeError50KG::stall,"UI checkpoint bounded interval");return rejected();
    }
    AdmittedInterval interval;
    interval.ledgerLifetime=lifetime_;interval.outputLifetime=outputLifetime_;
    auto& requested=interval.observation;
    requested.cause=cause;requested.checkpoint=checkpoint;
    requested.intervalSequence=audioIntervalSequence_+(delta?1u:0u);
    requested.acceptedPageSequence=lastAcceptedPageSequence_;
    requested.transitionSerial=transitionSerial;requested.presentationSequence=presentationSequence;
    requested.begin=lastAudioObservation_;requested.end=projected.endpoint;
    requested.deltaNanoseconds=delta;requested.accountedNanoseconds=projected.endpoint.activeNanoseconds;
    const auto result=output_->advanceAdmittedNanoseconds(interval,receipt,incomplete_,d);
    if(result==Result::rejected||result==Result::incomplete){
        // Includes a real accepted page followed by failed preflight. That page
        // must not be replayed even when no low-level output call was made.
        poisoned_=true;output_->advancementFault_=true;
        if(!incomplete_.present){
            requested.before=output_->counterObservation();requested.after=requested.before;
            incomplete_={true,false,false,true,requested,d};
        }
        return Result::incomplete;
    }
    // No active wall observation can commit before matching successful PCM or
    // the zero-delta/no-PCM result. Cold duration is consumed here exactly once.
    commitObservation(projected,true);
    acceptedPagePending_=false;
    if(result==Result::accounted){
        lastAudioEndpoint_=projected.endpoint.activeNanoseconds;lastAudioObservation_=projected.endpoint;
        ++audioIntervalSequence_;lastSuccessful_=receipt;
    }
    return result;
}
}

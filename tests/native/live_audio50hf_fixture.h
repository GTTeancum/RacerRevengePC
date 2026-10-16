#pragma once
// Stable native client owner. This subscribes to source-backed gameplay fields;
// its typed human reset fields are canonical, while the full retail emitter
// scheduler remains unrepresented. No missing core provider is bypassed.
#include "gameplay_audio_provider50he_fixture.h"
#include "pod_audio_owner50he_fixture.h"
#include "race_audio_groups50ic_fixture.h"
#include "selected_player_audio50ij_fixture.h"
#include "../../src/game/original_human_engine_reset50lb.h"
#include "../../platform/linux/native/audio_output50hd.h"
#include "../../platform/linux/native/announcer_episode50ic.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace live_audio50HF {
// Existing0..219 reservations remain fixed; original D44 arrays own the
// separate220..571 suffix and share this SAME sample clock/mixer.
inline constexpr std::size_t voiceCount=rr::platform::audioMaximumVoices50HD,providerFirstVoice=0,lapFirstVoice=60,lapVoiceCount=4;
static_assert(voiceCount<=rr::platform::audioMaximumVoices50HD);
inline constexpr std::uint32_t sampleRate=48000,stepMicroseconds=16000,stepFrames=768;
static_assert(gameplay_audio50HE::legacyCueCount==lapFirstVoice);
static_assert(gameplay_audio50HE::extraFirstVoice+gameplay_audio50HE::cueCount-
    gameplay_audio50HE::legacyCueCount<=voiceCount);
static_assert(lapFirstVoice+lapVoiceCount<=voiceCount);

enum class EventStatus : std::uint8_t { Complete,Unprepared,Paused,Closed,Input,Capacity,ApplyRejected };
struct EventReceipt {
    EventStatus status=EventStatus::Unprepared;
    std::size_t commandCount=0;
    std::array<rr::game::AudioCommand50HE,5> commands{};
    std::array<gameplay_audio50HE::ApplyResult,5> results{};
    // Complete can include explicit missing PCM. It is not a playback claim.
    bool okay()const noexcept{return status==EventStatus::Complete;}
};
struct Counters {
    std::uint64_t engineUpdates=0,warningUpdates=0,boostUpdates=0,repairUpdates=0;
    std::uint64_t commands=0,starts=0,stops=0,controls=0,inactive=0,unavailable=0,rejected=0;
    std::uint64_t inputRejected=0,lapRequests=0,committedSteps=0,mixFailures=0,resets=0,groupFailures=0;
};
inline bool hasJoinedTrigger(rr::game::AudioCue50HE cue) noexcept {
    return (cue.bank==rr::game::AudioBank50HE::PodEvent&&cue.key>=1&&cue.key<=6)||
        (cue.bank==rr::game::AudioBank50HE::PodEngine&&cue.key>=1&&cue.key<=3);
}
inline constexpr std::size_t unsupportedTriggerCues() noexcept{return gameplay_audio50HE::legacyCueCount-9;}

// Own this at a stable address. PCM, voices, mixer and both providers are never
// moved or exposed mutably; their borrowed references stay valid until close.
// All public methods belong to the simulation/main thread. Backend callbacks
// only consume the output's separately owned PCM ring.
class Owner final {
public:
    Owner()=default;
    ~Owner()noexcept {rr::platform::AudioOutputDiagnostic50HD diagnostic;close(diagnostic);}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;

    // Native storage stage only. The caller constructs the real six groups,
    // world, then seventh group on these SAME owners before prepare().
    bool beginCountedRaceAudioStorage()noexcept {
        if(prepareAttempted_||prepared_||closed_||coldStorage_||mixer_.count()||
           groups_.coldStarted_||groups_.mixer_||groups_.groups_.initialized)return false;
        if(!mixer_.initialize(voices_.data(),voices_.size(),sampleRate))return false;
        coldStorage_=true;coldStorageEpoch_=mixer_.groupEpoch50IC();
        coldStorageThread_=std::this_thread::get_id();return true;
    }
    rr::platform::AudioMixer50HD* mixerForCountedConstruction()noexcept {
        return coldStorageBorrowLive()?&mixer_:nullptr;
    }
    rr::platform::RaceAudioGroups50IC* groupsForCountedConstruction()noexcept {
        return coldStorageBorrowLive()?&groups_:nullptr;
    }

    // Cold only, once per owner, before any gameplay work. All authenticated
    // selected requests remain present, including unavailable records.
    // POD01 has60 authored semantic slots; the other seven have48. All use the
    // same legacy sixty-slot layout; POD19 adds seven slots152..158. Immutable
    // PCM aliases are resolved only once, never across different original names.
    // discRoot supplies retained originals; audioRoot is the explicit AUDIO
    // directory. No search of another computer or guessed external path occurs.
    bool prepare(const std::filesystem::path& discRoot,const std::filesystem::path& audioRoot,
        std::string& error,std::uint32_t playerRacer60=1,
        const rr::game::RaceAudioSettings50IC& audioSettings=rr::game::freshOriginalAudioSettings50IC()) {
        if(prepareAttempted_||closed_) {error="50HF preparation requires a fresh stable owner";return false;}
        if(coldStorage_&&(!coldStorageBorrowLive()||!groups_.coldStarted_||
           groups_.coldNext_!=rr::game::raceAudioGroupCount50IC)) {
            error="50HF counted audio storage requires its seven live constructed groups";return false;
        }
        prepareAttempted_=true;
        if(!rr::game::announcer_callbacks50IC_detail::settings(audioSettings)) {
            error="50HF original audio configuration must be integer 0..100";return false;
        }
        try {
            if(!selected_player_audio50IJ::read(discRoot,playerRacer60,selected_,error)||
               !rr::assets::loadAudioBank50HC(audioRoot,selected_.requests,bank_,error))return false;
            if(bank_.clips.size()!=selected_.requests.size()) {
                error="50HF authenticated selected audio catalog count differs";return false;
            }
            if(!coldStorage_&&!mixer_.initialize(voices_.data(),voices_.size(),sampleRate)) {
                error="50HF bounded mixer initialization failed";return false;
            }
            // The session owner supplies one immutable effective configuration.
            // Original reset defaults remain the default for other callers.
            // Next-session menu edits never mutate this owner's restore target.
            if(!groups_.initialize(mixer_,audioSettings)||
               !race_audio_groups50IC::bindGameplayCatalog(groups_,providerFirstVoice,playerRacer60==19).complete||
               !race_audio_groups50IC::bindProviderRange(groups_,
                   race_audio_groups50IC::ProviderRole::LapPodEvent,lapFirstVoice,lapVoiceCount)) {
                error="50IC source group ownership initialization failed";return false;
            }
            const auto* identity=original_player_identity50IK::find(playerRacer60);
            if(!identity){error="50IK original player identity unavailable";return false;}
            if(!provider_.prepare(bank_,mixer_,providerFirstVoice,error,&selected_.catalog)||
               !pod_audio50HE::prepareFromPod01(lap_,mixer_,bank_,lapFirstVoice,lapVoiceCount,
                   error,original_player_identity50IK::archivePath(discRoot,*identity),
                   playerRacer60))return false;
            prepared_=true;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
        catch(...){error="50HF audio preparation failed";return false;}
    }

    // Cold, AFTER WindowHost50DQ::create. Zero decoded samples refuse the
    // request before output.open, so an empty bank cannot create a proof WAV.
    // A real backend may be unavailable while the requested PCM capture works.
    // Caller must inspect info().deviceStatus before reporting device playback.
    bool openOutput(const rr::platform::AudioOutputRequest50HD& request,
        rr::platform::AudioOutputDiagnostic50HD& diagnostic)noexcept {
        if(closed_)return fail(diagnostic,rr::platform::AudioOutputError50HD::NotOpen,"50HF.open","Audio owner is closed");
        if(!prepared_)return fail(diagnostic,rr::platform::AudioOutputError50HD::InvalidRequest,"50HF.open","Audio bank has not been authenticated and prepared");
        if(outputOpen_)return fail(diagnostic,rr::platform::AudioOutputError50HD::AlreadyOpen,"50HF.open","Audio output is already open");
        if(!bank_.ready)return fail(diagnostic,rr::platform::AudioOutputError50HD::InvalidRequest,"50HF.open","No decoded original audio samples are available; output and WAV capture were not opened");
        if(request.sampleRate!=sampleRate)return fail(diagnostic,rr::platform::AudioOutputError50HD::InvalidRequest,"50HF.open","The live 16ms bridge requires 48000Hz output");
        if(!output_.open(request,diagnostic))return false;
        outputOpen_=true;
        if(paused_)output_.pauseDevice(true);
        return true;
    }

    // These event methods never allocate or read/decode files. The inputs must
    // come from the actual source phase. No speech, collision or debris trigger
    // is invented merely because its authored sample was registered.
    EventReceipt engine(const rr::game::PodEngineParameters50HE& parameters,
        const rr::game::PodEngineInput50HE& input)noexcept {
        EventReceipt r;if(!admit(r))return r;
        rr::game::PodEngineControls50HE controls;
        if(!rr::game::podEngineControls50HE(parameters,input,controls))return rejectInput(r);
        using namespace rr::game;
        const AudioCue50HE engineCue{AudioBank50HE::PodEngine,1},binderCue{AudioBank50HE::PodEngine,2};
        r.commands[0]={provider_.active(engineCue)?AudioOperation50HE::Controls:AudioOperation50HE::Start,
            engineCue,controls.engineGain,controls.enginePitch,true,0x19f5b0};
        r.commands[1]={provider_.active(binderCue)?AudioOperation50HE::Controls:AudioOperation50HE::Start,
            binderCue,controls.binderGain,controls.binderPitch,true,0x19f5b0};
        r.commandCount=2;++counters_.engineUpdates;apply(r);return r;
    }
    EventReceipt warnings(const rr::game::PodWarningInput50HE& input)noexcept {
        EventReceipt r;if(!admit(r))return r;
        auto next=humanController_.warning;
        const auto status=rr::game::updatePodWarnings50HE(input,humanController_.warning,next,
            r.commands.data(),r.commands.size(),r.commandCount);
        if(!projected(status,r))return r;
        humanController_.warning=next;++counters_.warningUpdates;apply(r);return r;
    }
    EventReceipt boostAtBoundary(bool heldBoost,float heating,std::uint32_t previous64)noexcept {
        EventReceipt r;if(!admit(r))return r;
        auto next=humanController_.boost;
        const auto status=rr::game::updatePodBoostAudio50HE(heldBoost,heating,previous64,humanController_.boost,next,
            r.commands.data(),r.commands.size(),r.commandCount);
        if(!projected(status,r))return r;
        humanController_.boost=next;++counters_.boostUpdates;apply(r);return r;
    }
    EventReceipt repairCompletion(std::uint8_t flagsB9C,std::uint32_t vehicle3C,
        const float (&damage7D0)[4])noexcept {
        EventReceipt r;if(!admit(r))return r;
        const auto status=rr::game::repairCompletionAudio50HE(flagsB9C,vehicle3C,damage7D0,
            r.commands.data(),r.commands.size(),r.commandCount);
        if(!projected(status,r))return r;
        ++counters_.repairUpdates;apply(r);return r;
    }
    // Admission for another separately recovered source event owner. This
    // method does not establish or manufacture the caller's gameplay trigger;
    // static sourceSite remains provenance, never a dispatch/authentication key.
    EventReceipt applySourceCommand(const rr::game::AudioCommand50HE& command)noexcept {
        EventReceipt r;if(!admit(r))return r;
        r.commands[0]=command;r.commandCount=1;apply(r);return r;
    }

    // This is the genuine bounded lap292360->233410 path only. A completed
    // unavailable/no-channel call is still completed; it has no audible start.
    // The caller's void 50GG effect acknowledgement remains originalV0=0,
    // separate from this receipt's actual nested sound result (0 or 1).
    pod_audio50HE::DispatchReceipt dispatchLap(std::uint32_t key=1,
        std::uint32_t flags=0,std::uint32_t argumentA3=0)noexcept {
        if(!prepared_||closed_||paused_)return {};
        ++counters_.lapRequests;
        return pod_audio50HE::dispatchLap(lap_,key,flags,argumentA3);
    }

    // Exactly once AFTER each committed simulation step, never per video frame,
    // failed/blocked step, or paused interval. 48kHz *16ms gives exactly768 frames.
    // Missing-bank mode has no output clock; it must not fabricate mixed frames.
    bool committedStep(rr::platform::AudioOutputDiagnostic50HD& diagnostic)noexcept {
        if(!outputOpen_||closed_)return fail(diagnostic,rr::platform::AudioOutputError50HD::NotOpen,"50HF.step","Audio output is not open");
        if(paused_)return fail(diagnostic,rr::platform::AudioOutputError50HD::InvalidRequest,"50HF.step","Paused audio must not advance the simulation sample clock");
        if(!announcer_.pollCompletion()) {
            ++counters_.groupFailures;
            return fail(diagnostic,rr::platform::AudioOutputError50HD::Mixer,"50IC.episode","Shared announcer ownership rejected");
        }
        const bool mixed=output_.advanceMicroseconds(mixer_,stepMicroseconds,diagnostic);
        // A failed output write may still follow a completed real mix. Always
        // observe actual voice activity, without inventing a completed duration.
        const bool restored=announcer_.pollCompletion();
        if(!restored)++counters_.groupFailures;
        if(!mixed) {
            ++counters_.mixFailures;return false;
        }
        if(!restored) {
            return fail(diagnostic,rr::platform::AudioOutputError50HD::Mixer,"50IC.completion","Announcer group restoration rejected");
        }
        ++counters_.committedSteps;return true;
    }
    void pause(bool paused)noexcept {
        if(closed_)return;
        paused_=paused;announcer_.pause(paused);if(outputOpen_)output_.pauseDevice(paused);
    }
    // Host reset policy, not a reconstructed original emitter reset method.
    // Preserve immutable registrations and cumulative output/capture clock.
    void resetVoices()noexcept {
        if(!prepared_||closed_)return;
        if(!announcer_.stop())++counters_.groupFailures;
        mixer_.stopAll();humanController_.warning={};humanController_.boost={};++counters_.resets;
    }
    // Explicit close BEFORE WindowHost50DQ destruction/SDL_Quit. Idempotent.
    // Destructor is only a fallback; host construction order remains important.
    bool close(rr::platform::AudioOutputDiagnostic50HD& diagnostic)noexcept {
        if(closeComplete_){diagnostic={};return true;}
        const bool restored=announcer_.stop();
        mixer_.stopAll();
        const bool result=output_.close(diagnostic);
        outputOpen_=false;closed_=true;
        if(!restored){++counters_.groupFailures;if(result)return fail(diagnostic,
            rr::platform::AudioOutputError50HD::Mixer,"50IC.close","Announcer group restoration rejected");}
        closeComplete_=result&&restored;return closeComplete_;
    }

    bool prepared()const noexcept{return prepared_;}
    bool outputOpen()const noexcept{return outputOpen_;}
    bool paused()const noexcept{return paused_;}
    bool closed()const noexcept{return closed_;}
    rr::game::OriginalHumanEngineResetOwner50LB& humanController() noexcept{return humanController_;}
    bool boostPitch()const noexcept{return humanController_.boost.pitchBoost();}
    const rr::game::PodWarningState50HE& warningState()const noexcept{return humanController_.warning;}
    const rr::assets::AudioBank50HC& bank()const noexcept{return bank_;}
    const gameplay_audio50HE::Provider& provider()const noexcept{return provider_;}
    // Root integration may reserve the disjoint64..79 material-sound range.
    // This remains main-thread-only; the provider must not touch0..63.
    rr::platform::AudioMixer50HD* mixerForMaterialOwner()noexcept {
        return prepared_&&!closed_?&mixer_:nullptr;
    }
    rr::platform::RaceAudioGroups50IC* groupsForProviders()noexcept {
        return prepared_&&!closed_?&groups_:nullptr;
    }
    rr::platform::AnnouncerEpisode50IC* sharedAnnouncer()noexcept {
        return prepared_&&!closed_?&announcer_:nullptr;
    }
    const rr::game::RaceAudioSettings50IC& audioSettings()const noexcept{return groups_.configuredLevels();}
    const rr::platform::AnnouncerEpisodeCounters50IC& announcerCounters()const noexcept{return announcer_.counters();}
    const pod_audio50HE::Owner& lapOwner()const noexcept{return lap_;}
    const Counters& counters()const noexcept{return counters_;}
    const rr::game::PodEngineParameters50HE& engineParameters()const noexcept{return selected_.engine;}
    const rr::game::PodSpeechCounts50HG& speechCounts()const noexcept{return selected_.speech;}
    rr::platform::AudioOutputInfo50HD info()const noexcept{return output_.info();}
    std::uint64_t mixedFrames()const noexcept{return mixer_.mixedFrames();}
    std::uint64_t clippedSamples()const noexcept{return mixer_.clippedSamples();}
    std::size_t activeVoices()const noexcept {
        std::size_t result=0;for(std::size_t n=0;n<voices_.size();++n)if(mixer_.active(n))++result;
        return result;
    }
    std::size_t loadedUnsupportedTriggerCues()const noexcept {
        std::size_t result=0;
        for(const auto& definition:gameplay_audio50HE::catalog) {
            const rr::game::AudioCue50HE cue{definition.bank,definition.key};
            if(!hasJoinedTrigger(cue)&&provider_.available(cue))++result;
        }
        return result;
    }
private:
    static bool fail(rr::platform::AudioOutputDiagnostic50HD& diagnostic,
        rr::platform::AudioOutputError50HD error,const char* operation,const char* detail)noexcept {
        diagnostic={};diagnostic.error=error;diagnostic.operation=operation;
        for(std::size_t n=0;n+1<diagnostic.detail.size()&&detail[n];++n)diagnostic.detail[n]=detail[n];
        return false;
    }
    bool admit(EventReceipt& r)const noexcept {
        if(closed_){r.status=EventStatus::Closed;return false;}
        if(!prepared_){r.status=EventStatus::Unprepared;return false;}
        if(paused_){r.status=EventStatus::Paused;return false;}
        r.status=EventStatus::Complete;return true;
    }
    EventReceipt rejectInput(EventReceipt& r)noexcept {
        r.status=EventStatus::Input;++counters_.inputRejected;return r;
    }
    bool projected(rr::game::AudioEventStatus50HE status,EventReceipt& r)noexcept {
        if(status==rr::game::AudioEventStatus50HE::Complete)return true;
        r.status=status==rr::game::AudioEventStatus50HE::Capacity?EventStatus::Capacity:EventStatus::Input;
        ++counters_.inputRejected;return false;
    }
    void apply(EventReceipt& r)noexcept {
        using Result=gameplay_audio50HE::ApplyResult;
        for(std::size_t n=0;n<r.commandCount;++n) {
            const auto result=provider_.apply(r.commands[n]);r.results[n]=result;++counters_.commands;
            switch(result) {
                case Result::Started:++counters_.starts;break;
                case Result::Stopped:++counters_.stops;break;
                case Result::Controlled:++counters_.controls;break;
                case Result::Inactive:++counters_.inactive;break;
                case Result::Unavailable:++counters_.unavailable;break;
                default:++counters_.rejected;r.status=EventStatus::ApplyRejected;break;
            }
        }
    }
    bool coldStorageBorrowLive()const noexcept {
        return coldStorage_&&coldStorageThread_==std::this_thread::get_id()&&
            !prepareAttempted_&&!prepared_&&!closed_&&!outputOpen_&&
            mixer_.groupEpoch50IC()==coldStorageEpoch_&&mixer_.count()==voices_.size()&&
            !mixer_.requiresVoiceGroupGains50IC()&&!groups_.coldRetiring_&&
            !groups_.nativeBindingsRetired_&&!groups_.mixer_;
    }
    bool coldStorage_=false,closeComplete_=false;
    std::uint64_t coldStorageEpoch_=0;
    std::thread::id coldStorageThread_{};
    // Destruction reverses this order; close also precedes every borrowed PCM
    // lifetime. No mutable accessors can resize a live PCM vector or voice array.
    rr::assets::AudioBank50HC bank_{};
    std::array<rr::platform::AudioVoice50HD,voiceCount> voices_{};
    rr::platform::AudioMixer50HD mixer_{};
    rr::platform::RaceAudioGroups50IC groups_{};
    rr::platform::AnnouncerEpisode50IC announcer_{};
    gameplay_audio50HE::Provider provider_{};
    selected_player_audio50IJ::Authoring selected_{};
    pod_audio50HE::Owner lap_{};
    rr::platform::AudioOutput50HD output_{};
    rr::game::OriginalHumanEngineResetOwner50LB humanController_{};
    Counters counters_{};
    bool prepareAttempted_=false,prepared_=false,outputOpen_=false,paused_=false,closed_=false;
};
}

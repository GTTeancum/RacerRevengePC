#pragma once
#include <chrono>
#include <cstdio>
// Stable native client owner. This subscribes to source-backed gameplay fields;
// its typed human reset fields are canonical, while the full retail emitter
// scheduler remains unrepresented. No missing core provider is bypassed.
#include "gameplay_audio_provider50he_fixture.h"
#include "pod_audio_owner50he_fixture.h"
#include "race_audio_groups50ic_fixture.h"
#include "selected_player_audio50ij_fixture.h"
#include "native_player_speech_authoring50la.h"
#include "native_player_speech_config_prefix50la.h"
#include "../../src/game/original_human_engine_reset50lb.h"
#include "../../platform/linux/native/audio_output50hd.h"
#include "../../platform/linux/native/announcer_episode50ic.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace interactive_continuation50DU {struct Context;}
namespace original_race_hud_scope50LA {class Registration;class Scope;}
namespace original_main_entry50KG {struct Application;}
namespace native_racer50HN {class Bank;}
namespace ra_source_camera50LA {class OwnershipBridge;}
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
    friend class rr::platform::OriginalApplicationAural50KG;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend struct ::interactive_continuation50DU::Context;
    friend class ::original_race_hud_scope50LA::Registration;
    friend class ::original_race_hud_scope50LA::Scope;
    // Canonical Setup/Context human speech borrows. The new forward phase
    // is distinct from the old successful-native-initializer receipt below.
    // Neither phase alone proves source22A210 return or22A25C attachment.
    const ::original_main_entry50KG::Application* speechApplication_=nullptr;
    const ::native_racer50HN::Bank* speechBankOwner_=nullptr;
    std::uint64_t speechGeneration_=0;
    std::uint32_t preparedRacer60_=0xffffffffu;
    rr::game::PodSpeechCounts50HG* humanSpeechCounts_=nullptr;
    rr::game::PodSpeechState50HG* humanSpeechState_=nullptr;
    std::uint32_t* humanSpeechRaw_=nullptr;
    // Forward custody is not the later initializeSpeech receipt. No copied
    // PodSpeechState or counts; construction writes the same root-owned fields.
    std::shared_ptr<rr::game::OriginalApplicationEntities50LA> speechEntities_;
    bool speechForwardBound_=false,speechConstructorAttempted_=false;
    enum class NativeSpeechInputStatus : std::uint8_t {
        notAttempted,ownerRejected,authoringRejected,nativeBindingRejected,captured
    };
    NativeSpeechInputStatus speechInputStatus_=NativeSpeechInputStatus::notAttempted;
    native_player_speech50LA::Authoring::Diagnostic speechInputDiagnostic_;
    std::unique_ptr<const native_player_speech50LA::Authoring> speechMountedAuthoring_;
    std::optional<native_player_speech50LA::LoadPrefix> speechConfigPrefix_;
    bool speechConfigAttempted_=false;
    std::array<const native_player_speech50LA::Authoring::Event*,gameplay_audio50HE::cueCount> speechAuthoredEvents_{};
    std::array<const rr::assets::AudioClip50HC*,gameplay_audio50HE::cueCount> speechNativeRecords_{};
    bool nativeSpeechBindingsCurrent()const noexcept {
        if(!speechMountedAuthoring_||speechInputStatus_!=NativeSpeechInputStatus::captured||
           speechMountedAuthoring_->racer60()!=preparedRacer60_)return false;
        for(std::size_t i=0;i<gameplay_audio50HE::cueCount;++i){
            const auto bank=gameplay_audio50HE::catalog[i].bank;
            if(bank!=rr::game::AudioBank50HE::PodCurses&&bank!=rr::game::AudioBank50HE::PodTaunts&&
               bank!=rr::game::AudioBank50HE::PodSpecial)continue;
            const auto& request=selected_.catalog[i];const auto* event=speechAuthoredEvents_[i];
            const auto* record=speechNativeRecords_[i];
            if(request.name.empty()){if(event||record||provider_.authored_[i]||provider_.ready_[i])return false;continue;}
            if(!event||!record||!event->sectionPresent||!event->filename.present||
               event->filename.text!=request.name||!event->isStream.present||event->isStream.text!="1"||
               event->isLoop.present||event->isStereo.present||!request.stream||request.loop||request.stereo||
               !event->metadata||bank_.find(request.name)!=record||!provider_.authored_[i]||
               record->request.stream!=request.stream||record->request.loop!=request.loop||
               record->request.stereo!=request.stereo||
               provider_.ready_[i]!=(record->state==rr::assets::AudioAssetState50HC::ready))return false;
            const auto& clip=provider_.clips_[i];const auto& pcm=record->pcm;
            if(provider_.ready_[i]){
                if(clip.samples!=pcm.samples.data()||clip.frames!=pcm.frames||clip.sampleRate!=pcm.sampleRate||
                   clip.channels!=pcm.channels||clip.loopStart!=(pcm.hasEmbeddedLoop?pcm.loopStart:0u)||
                   clip.loopEnd!=(pcm.hasEmbeddedLoop?pcm.loopEnd:0u))return false;
            }else if(clip.samples||clip.frames||clip.sampleRate||clip.channels||clip.loopStart||clip.loopEnd)return false;
        }
        // Actual playback revision remains in the ONE Provider; no frozen
        // revision, second voice, source channel claim or alternate PCM exists.
        return true;
    }
    bool retainNativeHumanSpeechInputs2C1170(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation,
        std::shared_ptr<const rr::assets::OriginalStandardArchiveProvider50LA> provider,
        std::shared_ptr<const native_racer50HW::Rig> rig,
        const rr::game::OriginalWreckComponent50LA& racer,
        const rr::game::OriginalWreckComponent50LA* d48)noexcept {
        if(speechInputStatus_!=NativeSpeechInputStatus::notAttempted)return false;
        speechInputStatus_=NativeSpeechInputStatus::ownerRejected;
        if(!nativeHumanSpeechProviderCurrent()||!speechForwardBound_||!speechConstructorAttempted_||
           speechApplication_!=&app||speechBankOwner_!=&bank||speechGeneration_!=generation||
           !speechEntities_||!rig||!provider||!groups_.coldNativePlayerSpeech_||
           groups_.coldNativePlayerSpeech_->nativeExpectedRacer!=&racer||
           &groups_.coldNativePlayerSpeech_->entity!=d48||
           !groups_.nativePlayerSpeechBackingCurrent())return false;
        const auto* selected=original_player_identity50IK::find(rig->podId);
        if(!selected||selected->racer60!=preparedRacer60_||!groups_.coldNativePlayerSpeech_->nativeAural)return false;
        auto metadata=groups_.coldNativePlayerSpeech_->nativeAural->retainedStreamMetadata();
        speechInputStatus_=NativeSpeechInputStatus::authoringRejected;
        auto authored=native_player_speech50LA::Authoring::capture(std::move(provider),std::move(rig),
            std::move(metadata),speechInputDiagnostic_);
        if(!authored)return false;
        speechInputStatus_=NativeSpeechInputStatus::nativeBindingRejected;
        const auto& counts=authored->counts();const auto& selectedCounts=selected_.speech;
        if(counts!=std::array<std::uint8_t,9>{selectedCounts.lowDamage,selectedCounts.wallHit,selectedCounts.passed,
            selectedCounts.attacked,selectedCounts.passing,selectedCounts.attacking,selectedCounts.death,
            selectedCounts.win,selectedCounts.lose})return false;
        decltype(speechAuthoredEvents_) events{};decltype(speechNativeRecords_) records{};
        constexpr rr::game::AudioBank50HE banks[]={rr::game::AudioBank50HE::PodCurses,
            rr::game::AudioBank50HE::PodTaunts,rr::game::AudioBank50HE::PodSpecial};
        for(unsigned category=0;category<3;++category){const auto* rows=authored->category(category);
            if(!rows)return false;
            for(unsigned n=0;n<rows->count;++n){const auto& event=rows->events[n];
                const auto index=gameplay_audio50HE::cueIndex({banks[category],event.id});
                if(index>=gameplay_audio50HE::cueCount)return false;
                const auto& request=selected_.catalog[index];
                if(!event.sectionPresent){if(!request.name.empty()||provider_.authored_[index])return false;continue;}
                if(event.filename.text!=request.name||request.name.empty()||!request.stream||request.loop||request.stereo||
                   !provider_.authored_[index])return false;
                const auto* record=bank_.find(request.name);if(!record)return false;
                events[index]=&event;records[index]=record;
            }
        }
        // Publish only immutable native inputs. No count/cursor/timer/source
        // RNG store, Config/file handle, Event, Sound, or initialized receipt.
        speechMountedAuthoring_=std::move(authored);speechAuthoredEvents_=events;speechNativeRecords_=records;
        speechInputStatus_=NativeSpeechInputStatus::captured;
        if(!nativeSpeechBindingsCurrent()){
            speechAuthoredEvents_.fill(nullptr);speechNativeRecords_.fill(nullptr);speechMountedAuthoring_.reset();
            speechInputStatus_=NativeSpeechInputStatus::nativeBindingRejected;return false;
        }
        return true;
    }
    bool beginNativeHumanSpeechConfig2C1190(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation,
        const rr::game::OriginalAudioDirectorySource50LA& directory,
        const rr::game::OriginalWreckComponent50LA& racer,
        const rr::game::OriginalWreckComponent50LA* d48,std::uint32_t actualRacer60,
        std::uint32_t& pc)noexcept {
        if(speechConfigAttempted_||speechConfigPrefix_)return false;
        if(!nativeHumanSpeechProviderCurrent()||!speechForwardBound_||!speechConstructorAttempted_||
           speechApplication_!=&app||speechBankOwner_!=&bank||speechGeneration_!=generation||
           speechInputStatus_!=NativeSpeechInputStatus::captured||!speechMountedAuthoring_||
           speechMountedAuthoring_->racer60()!=actualRacer60||preparedRacer60_!=actualRacer60||
           !groups_.coldNativePlayerSpeech_||
           groups_.coldNativePlayerSpeech_->nativeExpectedRacer!=&racer||
           &groups_.coldNativePlayerSpeech_->entity!=d48||
           !groups_.nativePlayerSpeechBackingCurrent())return false;
        speechConfigAttempted_=true;speechConfigPrefix_.emplace();
        const bool filenameReturned=speechConfigPrefix_->begin2C1190(app,directory,&racer,d48,actualRacer60);
        pc=speechConfigPrefix_->pc_;return filenameReturned;
        // True means ONLY the constructor and bounded filename prefix returned.
        // The source open1577D0, counts, RNG and2C1170 return remain pending.
    }
    // 2C1300..2C1914 in the player's own 2C1170: the six rotating cursor draws
    // of THIS D48, at their source position (after the 2C11F0.. counts, before
    // any AI pod), on the bound counts/state and the same raw 308030 word.
    // One writer: the later all-eight initializer skips an initialized human.
    bool initializeNativeHumanSpeechCursors2C1300(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation,std::uint32_t& actualRaw)noexcept {
        if(!nativeHumanSpeechProviderCurrent()||!speechForwardBound_||!speechConstructorAttempted_||
           speechApplication_!=&app||speechBankOwner_!=&bank||speechGeneration_!=generation||
           humanSpeechRaw_!=&actualRaw||!humanSpeechCounts_||!humanSpeechState_||humanSpeechState_->initialized||
           speechInputStatus_!=NativeSpeechInputStatus::captured||!speechConfigPrefix_)return false;
        return rr::game::initializePodSpeech50HG(*humanSpeechCounts_,actualRaw,*humanSpeechState_)==
            rr::game::PodSpeechStatus50HG::Complete;
    }
    void withdrawNativeSpeechInputs()noexcept {
        // Abandon only native stack custody before its racer/D48 aliases die.
        // No source Config destructor, close, or load-return effect is claimed.
        if(speechConfigPrefix_){speechConfigPrefix_->abandonNative();speechConfigPrefix_.reset();}
        speechAuthoredEvents_.fill(nullptr);speechNativeRecords_.fill(nullptr);speechMountedAuthoring_.reset();
        // Attempt/result history is not reset and cannot authorize replay.
    }
    bool nativeHumanSpeechProviderCurrent()const noexcept {
        if(!prepared_||closed_||!coldStorage_||coldStorageThread_!=std::this_thread::get_id()||
           coldStorageEpoch_!=mixer_.groupEpoch50IC()||!groups_.uses(mixer_)||
           provider_.mixer_!=&mixer_||provider_.epoch_!=mixer_.groupEpoch50IC())return false;
        for(std::size_t i=0;i<gameplay_audio50HE::cueCount;++i){
            const auto bank=gameplay_audio50HE::catalog[i].bank;
            if(bank!=rr::game::AudioBank50HE::PodCurses&&bank!=rr::game::AudioBank50HE::PodTaunts&&
               bank!=rr::game::AudioBank50HE::PodSpecial)continue;
            const bool authored=!selected_.catalog[i].name.empty();
            if(provider_.authored_[i]!=authored)return false;
            if(authored&&(provider_.voiceSlots_[i]!=gameplay_audio50HE::cueVoice(providerFirstVoice,i)||
               groups_.binding(provider_.voiceSlots_[i])!=rr::game::RaceAudioGroup50IC::Character11C))return false;
        }
        return !speechMountedAuthoring_||nativeSpeechBindingsCurrent();
    }
    bool bindNativeHumanSpeechForward22A210(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,
        const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& entities,
        std::uint64_t generation,std::uint32_t racer60,rr::game::PodSpeechCounts50HG& counts,
        rr::game::PodSpeechState50HG& state,std::uint32_t& actualRaw)noexcept {
        if(!nativeHumanSpeechProviderCurrent()||speechApplication_||speechBankOwner_||speechGeneration_||
           humanSpeechCounts_||humanSpeechState_||humanSpeechRaw_||speechEntities_||speechForwardBound_||
           speechConstructorAttempted_||!generation||preparedRacer60_!=racer60||state.initialized||
           !entities||groups_.coldApplication_!=entities||
           groups_.coldApplication_.owner_before(entities)||entities.owner_before(groups_.coldApplication_)||
           groups_.coldSession_!=generation)return false;
        const auto& c=selected_.speech;
        if(counts.lowDamage!=c.lowDamage||counts.wallHit!=c.wallHit||counts.passed!=c.passed||
           counts.attacked!=c.attacked||counts.passing!=c.passing||counts.attacking!=c.attacking||
           counts.death!=c.death||counts.win!=c.win||counts.lose!=c.lose)return false;
        speechApplication_=&app;speechBankOwner_=&bank;speechGeneration_=generation;speechEntities_=entities;
        humanSpeechCounts_=&counts;humanSpeechState_=&state;humanSpeechRaw_=&actualRaw;
        speechForwardBound_=true;return true;
    }
    rr::platform::NativePlayerSpeechConstructorResult50LA constructNativeHumanSpeech2C1000(
        const ::original_main_entry50KG::Application& app,const ::native_racer50HN::Bank& bank,
        const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& entities,
        std::uint64_t generation,std::uint32_t racer60,
        const rr::game::OriginalWreckComponent50LA& race,const rr::game::OriginalWreckComponent50LA& character11C,
        rr::game::OriginalWreckComponent50LA& racer,std::uint32_t& actualRaw)noexcept {
        if(!nativeHumanSpeechProviderCurrent()||!speechForwardBound_||speechConstructorAttempted_||
           speechApplication_!=&app||speechBankOwner_!=&bank||speechGeneration_!=generation||
           preparedRacer60_!=racer60||speechEntities_!=entities||speechEntities_.owner_before(entities)||
           entities.owner_before(speechEntities_)||humanSpeechRaw_!=&actualRaw||
           !humanSpeechCounts_||!humanSpeechState_||humanSpeechState_->initialized)return {};
        speechConstructorAttempted_=true; // Never replay a refused native allocation.
        return groups_.constructNativePlayerSpeech2C1000(this,entities,generation,race,character11C,racer,
            *humanSpeechCounts_,*humanSpeechState_);
    }
    bool canWithdrawNativeHumanSpeechForward22A210(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation)const noexcept {
        // Native alias cancellation only, including failure before Session
        // exists. No source object, count, serial, numeric field or RNG write.
        return speechForwardBound_&&!groups_.coldNativePlayerSpeech_&&speechApplication_==&app&&
            speechBankOwner_==&bank&&speechGeneration_==generation&&generation&&speechEntities_&&
            speechEntities_==groups_.coldApplication_&&
            !speechEntities_.owner_before(groups_.coldApplication_)&&
            !groups_.coldApplication_.owner_before(speechEntities_)&&
            coldStorageThread_==std::this_thread::get_id()&&
            humanSpeechCounts_&&humanSpeechState_&&humanSpeechRaw_;
    }
    bool withdrawNativeHumanSpeechForward22A210(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation)noexcept {
        if(!canWithdrawNativeHumanSpeechForward22A210(app,bank,generation))return false;
        speechApplication_=nullptr;speechBankOwner_=nullptr;speechGeneration_=0;
        humanSpeechCounts_=nullptr;humanSpeechState_=nullptr;humanSpeechRaw_=nullptr;
        withdrawNativeSpeechInputs();
        speechForwardBound_=false;speechEntities_.reset();return true;
    }
    bool canRetireNativeHumanSpeech2C10B0(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation,
        const rr::game::OriginalWreckComponent50LA& racer,
        const rr::game::OriginalWreckComponent50LA* publishedD48)const noexcept {
        if(!nativeHumanSpeechProviderCurrent()||!speechForwardBound_||speechApplication_!=&app||
           speechBankOwner_!=&bank||speechGeneration_!=generation||!speechEntities_||
           !humanSpeechCounts_||!humanSpeechState_||!humanSpeechRaw_)return false;
        if(!groups_.coldNativePlayerSpeech_)return publishedD48==nullptr;
        return groups_.canRetireNativePlayerSpeech2C10B0(this,racer,publishedD48);
    }
    bool retireNativeHumanSpeech2C10B0(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation,
        const rr::game::OriginalWreckComponent50LA& racer,
        const rr::game::OriginalWreckComponent50LA* historicalD48)noexcept {
        if(!canRetireNativeHumanSpeech2C10B0(app,bank,generation,racer,historicalD48))return false;
        // Caller first withdraws its native aliases/sourceD48 after this same
        // preflight. Revoke our borrowed state before its source backing dies.
        speechApplication_=nullptr;speechBankOwner_=nullptr;speechGeneration_=0;
        humanSpeechCounts_=nullptr;humanSpeechState_=nullptr;humanSpeechRaw_=nullptr;
        withdrawNativeSpeechInputs();
        speechForwardBound_=false;speechEntities_.reset();
        if(groups_.coldNativePlayerSpeech_&&
           !groups_.retireNativePlayerSpeech2C10B0(this,racer,historicalD48))std::terminate();
        return true;
    }

    bool retainNativeHumanSpeechInitialization(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation,std::uint32_t racer60,
        rr::game::PodSpeechCounts50HG& counts,rr::game::PodSpeechState50HG& state,
        std::uint32_t& actualRaw)noexcept {
        //Called ONLY immediately after actual successful initializeSpeech in
        //Context's real activation. No RNG, timer, cursor or guest D48 store.
        if(!prepared_||closed_||speechForwardBound_||speechConstructorAttempted_||speechApplication_||
           !generation||preparedRacer60_!=racer60||!state.initialized)return false;
        const auto& c=selected_.speech;
        if(counts.lowDamage!=c.lowDamage||counts.wallHit!=c.wallHit||counts.passed!=c.passed||
           counts.attacked!=c.attacked||counts.passing!=c.passing||counts.attacking!=c.attacking||
           counts.death!=c.death||counts.win!=c.win||counts.lose!=c.lose)return false;
        speechApplication_=&app;speechBankOwner_=&bank;speechGeneration_=generation;
        humanSpeechCounts_=&counts;humanSpeechState_=&state;humanSpeechRaw_=&actualRaw;return true;
    }
    bool nativeHumanSpeechOwner(const ::original_main_entry50KG::Application& app,
        const ::native_racer50HN::Bank& bank,std::uint64_t generation,std::uint32_t actualRacer60,
        std::uint32_t& actualRaw,rr::game::PodSpeechCounts50HG*& counts,
        rr::game::PodSpeechState50HG*& state)noexcept {
        counts=nullptr;state=nullptr;
        if(!prepared_||closed_||speechForwardBound_||speechConstructorAttempted_||
           speechApplication_!=&app||speechBankOwner_!=&bank||
           speechGeneration_!=generation||preparedRacer60_!=actualRacer60||
           humanSpeechRaw_!=&actualRaw||!humanSpeechCounts_||!humanSpeechState_||!humanSpeechState_->initialized)return false;
        counts=humanSpeechCounts_;state=humanSpeechState_;return true;
    }
    void revokeNativeHumanSpeech()noexcept {
        if(speechForwardBound_||groups_.coldNativePlayerSpeech_)std::terminate(); // Use authenticated retirement.
        speechApplication_=nullptr;speechBankOwner_=nullptr;speechGeneration_=0;
        humanSpeechCounts_=nullptr;humanSpeechState_=nullptr;humanSpeechRaw_=nullptr;
    }

    bool canStopResultsGameplay26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&,std::uint64_t)const noexcept;
    // ALL8 has no bound human controller. Its prepared selected Provider is
    // NOT a stop owner: row0 AI loops already own the overlapping57..59 slots.
    // This proves only the actual shared mixer backing and absence of human
    // authority; outer residual-active checks still cover every unclaimed voice.
    bool all8NonHumanMixerOwner26D070(const rr::platform::AudioMixer50HD& mixer,
        const rr::platform::RaceAudioGroups50IC& groups,std::uint32_t row0Racer60)const noexcept {
        return coldStorage_&&coldStorageThread_==std::this_thread::get_id()&&prepared_&&!closed_&&
            &mixer_==&mixer && &groups_==&groups&&coldStorageEpoch_==mixer.groupEpoch50IC()&&
            groups.uses(mixer)&&preparedRacer60_==row0Racer60&&!humanController_.bound()&&
            !speechApplication_&&!speechBankOwner_&&!speechGeneration_&&
            !humanSpeechCounts_&&!humanSpeechState_&&!humanSpeechRaw_;
    }
    bool resultsGameplayStopVoice26D070(std::size_t n,std::size_t& slot)const noexcept {
        return provider_.resultsStopVoice26D070(n,slot);
    }
    void stopResultsGameplayPreflighted26D070()noexcept {
        provider_.stopResultsNativePreflighted26D070();
    }
    bool canStopResultsLap26D070(const rr::platform::AudioMixer50HD& mixer,
        const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
        if(!canStopResultsGameplay26D070(mixer,groups,session)||lap_.mixer!=&mixer||
           lap_.mixerEpoch!=mixer.groupEpoch50IC()||!lap_.voiceCount||
           lap_.firstVoice>=mixer.count()||lap_.voiceCount>mixer.count()-lap_.firstVoice||
           !mixer.canRecordControlChanges(lap_.voiceCount))return false;
        for(std::size_t n=0;n<pod_audio50HE::eventCount;++n){
            const auto& s=lap_.sounds[n];const auto& row=lap_.rows[n];
            if(s.authored.key!=n+1||row.owner!=&s||s.fields.resourceIndexA0!=static_cast<std::int32_t>(n)||
               s.authored.stereo||s.authored.stream||s.authored.loop!=(n>=2)||
               row.sampleResult!=s.fields.sample28)return false;
        }
        // Only actual dispatchLap event1 has this transport. Do not infer
        // permission for other authenticated but unimplemented event rows.
        const auto& sound=lap_.sounds[0];
        if(lap_.selectedOwner1F0&&lap_.selectedOwner1F0!=&sound)return false;
        for(std::size_t n=0;n<lap_.voiceCount;++n){const auto slot=lap_.firstVoice+n;
            rr::platform::AudioVoiceObservation50HD v;
            if(groups.binding(slot)!=rr::game::RaceAudioGroup50IC::Effects110||
               !mixer.inspectVoice(slot,v)||!v.groupKnown||
               v.playbackRevision!=lap_.playbackRevisions[slot])return false;
            if(!v.playbackRevision){if(v.voice.active)return false;continue;}
            const auto& a=v.voice.clip;const auto& b=sound.clip;
            if(sound.availability!=rr::assets::AudioAssetState50HC::ready||
               sound.fields.sample28!=0||!rr::platform::AudioMixer50HD::validClip(b)||
               a.samples!=b.samples||a.frames!=b.frames||a.sampleRate!=b.sampleRate||
               a.channels!=b.channels||a.loopStart!=b.loopStart||a.loopEnd!=b.loopEnd||
               v.voice.loop)return false;
        }
        return true;
    }
    std::size_t resultsLapStopCount26D070()const noexcept{return lap_.voiceCount;}
    std::size_t resultsLapStopVoice26D070(std::size_t n)const noexcept {
        return n<lap_.voiceCount?lap_.firstVoice+n:rr::platform::audioMaximumVoices50HD;
    }
    void stopResultsLapPreflighted26D070()noexcept {
        for(std::size_t n=0;n<lap_.voiceCount;++n)mixer_.stop(lap_.firstVoice+n);
        // Preserve selectedOwner, latest source channel2C/24/7C, retained PCM,
        // every captured native overlap identity and existing callback order.
    }
public:
    Owner()=default;
    ~Owner()noexcept;
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

    // Explicit muted backend policy still needs the real seven-group control
    // suffix. Consume the same staged mixer once; no PCM/provider/output claim.
    bool finishCountedMutedGroups(const rr::game::RaceAudioSettings50IC& settings)noexcept {
        if(!coldStorageBorrowLive()||!groups_.coldStarted_||
           groups_.coldNext_!=rr::game::raceAudioGroupCount50IC||
           !rr::game::announcer_callbacks50IC_detail::settings(settings))return false;
        prepareAttempted_=true; // Construction borrows expire before effects.
        return groups_.initialize(mixer_,settings);
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
            // The session owner supplies the initial effective configuration.
            // Original reset defaults remain the default for other callers.
            // Legacy next-session edits do not mutate this restore target;
            // native live Pause changes it only with the actual shared profile.
            // The extra speech slots (catalog keys past the 60 base keys, voices
            // 152+) join their source voice-volume group whenever this racer's
            // selected catalog authors one of them (POD19's own sounds.ini sums
            // curses 24 and special 11, filling keys 20..24 and 10..11; Watto 25
            // fills two), never by racer number.
            bool selectedExtraSpeech=false;
            for(std::size_t i=gameplay_audio50HE::legacyCueCount;i<selected_.catalog.size();++i)
                if(!selected_.catalog[i].name.empty())selectedExtraSpeech=true;
            if(!groups_.initialize(mixer_,audioSettings)||
               !race_audio_groups50IC::bindGameplayCatalog(groups_,providerFirstVoice,selectedExtraSpeech).complete||
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
            preparedRacer60_=playerRacer60;prepared_=true;error.clear();return true;
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
        using StepClock=std::chrono::steady_clock;static unsigned slowLines=0;
        const auto slow=[&](const char* part,StepClock::time_point begin){
            const double ms=std::chrono::duration<double,std::milli>(StepClock::now()-begin).count();
            if(ms>20.0&&slowLines<200){++slowLines;std::fprintf(stderr,"audio_step_slow part=%s ms=%.1f%c",part,ms,10);}
        };
        auto begin=StepClock::now();
        if(!announcer_.pollCompletion()) {
            ++counters_.groupFailures;
            return fail(diagnostic,rr::platform::AudioOutputError50HD::Mixer,"50IC.episode","Shared announcer ownership rejected");
        }
        slow("announcer_poll_before",begin);begin=StepClock::now();
        const bool mixed=output_.advanceMicroseconds(mixer_,stepMicroseconds,diagnostic);
        slow("output_advance",begin);begin=StepClock::now();
        // A failed output write may still follow a completed real mix. Always
        // observe actual voice activity, without inventing a completed duration.
        const bool restored=announcer_.pollCompletion();
        slow("announcer_poll_after",begin);
        if(!restored)++counters_.groupFailures;
        if(!mixed) {
            ++counters_.mixFailures;return false;
        }
        if(!restored) {
            return fail(diagnostic,rr::platform::AudioOutputError50HD::Mixer,"50IC.completion","Announcer group restoration rejected");
        }
        ++counters_.committedSteps;return true;
    }
    // Native spectator-menu scheduling only. Caller supplies actual host-time
    // chunks (at most one16ms audio quantum), never a physics-step receipt.
    // Original group pause state remains authoritative for every voice cursor.
    bool serviceNativeMenuAudio(std::uint32_t elapsedMicroseconds,
        rr::platform::AudioOutputDiagnostic50HD& diagnostic)noexcept {
        const auto* worldPaused=groups_.sourceState44(rr::game::RaceAudioGroup50IC::World4D8);
        if(!outputOpen_||!prepared_||closed_||paused_||!coldStorage_||!worldPaused||!*worldPaused||
           !elapsedMicroseconds||elapsedMicroseconds>stepMicroseconds)
            return fail(diagnostic,rr::platform::AudioOutputError50HD::InvalidRequest,
                "50HF.menu","Actual counted menu audio requires a live unpaused output and bounded host interval");
        if(!announcer_.pollCompletion()){
            ++counters_.groupFailures;
            return fail(diagnostic,rr::platform::AudioOutputError50HD::Mixer,
                "50IC.menu","Shared announcer ownership rejected");
        }
        const bool mixed=output_.advanceMicroseconds(mixer_,elapsedMicroseconds,diagnostic);
        // Output can reject after actual samples were consumed. Always poll
        // their real completion, with no world/AI/announcer scheduling update.
        const bool restored=announcer_.pollCompletion();
        if(!restored)++counters_.groupFailures;
        if(!mixed){++counters_.mixFailures;return false;}
        if(!restored)return fail(diagnostic,rr::platform::AudioOutputError50HD::Mixer,
            "50IC.menuCompletion","Announcer group restoration rejected");
        return true; // Never increments committedSteps: no simulation step ran.
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
        if(speechForwardBound_||groups_.coldNativePlayerSpeech_)return fail(diagnostic,
            rr::platform::AudioOutputError50HD::Mixer,"50HF.close","Source D48 custody must retire before audio close");
        revokeNativeHumanSpeech(); //Native borrows end BEFORE provider/output teardown.
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

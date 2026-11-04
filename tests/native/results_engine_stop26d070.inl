#pragma once
// Private transport-only leaves. Included after concrete owners are complete;
// do not introduce include cycles into logical_sound/emitter construction.
#include "live_audio50hf_fixture.h"
#include "original_ai_audio_owner.h"

inline bool rr::platform::LogicalAudioOwner::canStopResultsNative26D070(
    const AudioMixer50HD& mixer)const noexcept {
    if(mixer_!=&mixer||!count_||mixer.groupEpoch50IC()!=epoch_||
       !mixer.canRecordControlChanges(count_))return false;
    for(std::size_t n=0;n<count_;++n){const auto& s=sounds_[n];
        if(!registered_[n]||!owns(s)||s.slot>=mixer.count())return false;
        for(std::size_t p=0;p<n;++p)if(sounds_[p].slot==s.slot)return false;
        AudioVoiceObservation50HD v;
        if(!mixer.inspectVoice(s.slot,v)||!v.groupKnown||v.playbackRevision!=s.revision)return false;
        if(!v.playbackRevision){if(v.voice.active)return false;continue;}
        const auto& a=v.voice.clip;const auto& b=s.clip;
        if(!s.available||!AudioMixer50HD::validClip(b)||a.samples!=b.samples||
           a.frames!=b.frames||a.sampleRate!=b.sampleRate||a.channels!=b.channels||
           a.loopStart!=b.loopStart||a.loopEnd!=b.loopEnd)return false;
        if(v.voice.active&&!active(s))return false;
    }
    return true;
}
inline void rr::platform::LogicalAudioOwner::stopResultsNativePreflighted26D070()noexcept {
    // NOT233670/close: preserve channel2C, elapsed24, word7C and tokens.
    // Aural separately proved disjointness from its exact native Intro lease.
    for(std::size_t n=0;n<count_;++n)mixer_->stop(sounds_[n].slot);
}

inline bool gameplay_audio50HE::Provider::resultsStopVoice26D070(
    std::size_t n,std::size_t& slot)const noexcept {
    if(n>=cueCount||!authored_[n]||!rr::platform::RaceAudioGroups50IC::represented(
       rr::game::gameplayAudioGroup50IC({catalog[n].bank,catalog[n].key})))return false;
    slot=voiceSlots_[n];return true;
}
inline bool gameplay_audio50HE::Provider::canStopResultsNative26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups)const noexcept {
    if(mixer_!=&mixer||epoch_!=mixer.groupEpoch50IC()||!groups.uses(mixer)||
       !mixer.canRecordControlChanges(cueCount))return false;
    for(std::size_t n=0;n<cueCount;++n){std::size_t slot;
        // Unrepresented debris/wreck/TrackIntro rows stay unclaimed. Outer
        // residual-active check rejects them rather than exempting a range.
        if(!resultsStopVoice26D070(n,slot))continue;
        const auto group=rr::game::gameplayAudioGroup50IC({catalog[n].bank,catalog[n].key});
        if(slot>=mixer.count()||groups.binding(slot)!=group)return false;
        for(std::size_t p=0;p<n;++p)if(authored_[p]&&voiceSlots_[p]==slot)return false;
        rr::platform::AudioVoiceObservation50HD v;
        if(!mixer.inspectVoice(slot,v)||!v.groupKnown||v.playbackRevision!=revisions_[n])return false;
        if(!v.playbackRevision){if(v.voice.active)return false;continue;}
        const auto& a=v.voice.clip;const auto& b=clips_[n];
        if(!ready_[n]||!rr::platform::AudioMixer50HD::validClip(b)||
           a.samples!=b.samples||a.frames!=b.frames||a.sampleRate!=b.sampleRate||
           a.channels!=b.channels||a.loopStart!=b.loopStart||a.loopEnd!=b.loopEnd)return false;
    }
    return true;
}
inline void gameplay_audio50HE::Provider::stopResultsNativePreflighted26D070()noexcept {
    for(std::size_t n=0;n<cueCount;++n){std::size_t slot;
        if(resultsStopVoice26D070(n,slot))mixer_->stop(slot);
    }
}

inline bool live_audio50HF::Owner::canStopResultsGameplay26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
    if(!coldStorage_||coldStorageThread_!=std::this_thread::get_id()||
       !prepared_||closed_||&mixer_!=&mixer||&groups_!=&groups||
       coldStorageEpoch_!=mixer.groupEpoch50IC()||!groups.uses(mixer)||
       !humanController_.bound()||!session||
       humanController_.identity().session!=session||
       humanController_.identity().managerOrdinal!=0)return false;
    return provider_.canStopResultsNative26D070(mixer,groups);
}

inline bool native_original_ai_audio::Owner::canStopResultsEngines26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
    // Exact human Course domain. ALL8 uses its separate gate below; never
    // borrow a human controller or admit an alternative host subscriber.
    if(!prepared_||!activated_||!session||firstAiSlot_!=1||racerCount_!=8||
       !mode_||mode_!=mixer.originalSoundMode50KG()||!mode_->ready()||
       !humanResetOwner_||!humanResetOwner_->bound()||
       humanResetOwner_->identity().session!=session||!groups.uses(mixer))return false;
    return canStopNativeEngineRows26D070(mixer,groups,session);
}
inline bool native_original_ai_audio::Owner::canStopAll8Engines26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
    if(!prepared_||!activated_||!session||firstAiSlot_!=0||racerCount_!=8||
       !mode_||mode_!=mixer.originalSoundMode50KG()||!mode_->ready()||
       humanResetOwner_||humanResetD44_||humanMaterials_.resultsStopCount26D070()||
       materialD44_.humanCount()!=0||materialD44_.racerCount()!=8||!groups.uses(mixer))return false;
    for(std::size_t i=0;i<racerCount_;++i)if(!materialD44_.ownsAiSlot(i))return false;
    return canStopNativeEngineRows26D070(mixer,groups,session);
}
inline bool native_original_ai_audio::Owner::canStopNativeEngineRows26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
    // Reached only after one complete domain gate. Actual per-row owners,
    // sound addresses, revisions/PCM and group bindings supply every claim.
    for(std::size_t i=firstAiSlot_;i<racerCount_;++i){const auto& r=racers_[i];
        if(r.owner!=this||r.slot!=i||!r.engine.constructed||!r.engine.prepared||
           r.engine.blocked||r.engine.inUpdate||r.engine.identity.session!=session||
           r.engine.identity.managerOrdinal!=i||r.loops.resultsStopCount26D070()!=3||
           !r.loops.canStopResultsNative26D070(mixer))return false;
        for(std::size_t j=0;j<3;++j){const auto& s=r.sounds[j];
            if(s.session!=session||s.nativeOwner!=&r.loops||
               s.nativeSound!=&r.loops.sounds_[j]||
               s.multiplier54!=&r.loops.sounds_[j].multiplier54||
               s.position60!=r.loops.sounds_[j].position60.data()||
               groups.binding(r.loops.resultsStopVoice26D070(j))!=
                   rr::game::RaceAudioGroup50IC::Engines114)return false;
        }
    }
    return mixer.canRecordControlChanges(resultsEngineStopCount26D070());
}

inline bool native_original_ai_audio::Owner::canStopResultsAuxiliary26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
    if(!canStopResultsEngines26D070(mixer,groups,session))return false;
    return canStopNativeAuxiliaryRows26D070(mixer,groups,session);
}
inline bool native_original_ai_audio::Owner::canStopAll8Auxiliary26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
    if(!canStopAll8Engines26D070(mixer,groups,session))return false;
    return canStopNativeAuxiliaryRows26D070(mixer,groups,session);
}
inline bool native_original_ai_audio::Owner::canStopNativeAuxiliaryRows26D070(
    const rr::platform::AudioMixer50HD& mixer,
    const rr::platform::RaceAudioGroups50IC& groups,std::uint64_t session)const noexcept {
    using Group=rr::game::RaceAudioGroup50IC;
    // Exact domain gate already proved all engine rows and real pool shape.
    // ALL8's firstAiSlot0 reaches no human branch below. No reset/close occurs.
    if(!materialPrepared_||!materialNativeBound_||!speechPrepared_||!speechInitialized_||
       materialD44_.humanCount()!=firstAiSlot_||materialD44_.racerCount()!=racerCount_)return false;
    const auto* group44=groups.sourceState44(Group::Effects110);
    if(!group44)return false;
    for(std::size_t i=0;i<racerCount_;++i){
        const bool human=i<firstAiSlot_;
        const auto* row=human?humanResetD44_:racers_[i].materialD44;
        const auto& channel=human?humanMaterials_:racers_[i].materials;
        const auto* vehicle=human?&humanResetOwner_->identity():&racers_[i].engine.identity;
        if(!row||row->poolOrdinal()!=i||row->vehicle248()!=vehicle||
           vehicle->session!=session||vehicle->managerOrdinal!=i||
           (!human&&!materialD44_.ownsAiSlot(i)))return false;
        const auto& state=row->material();
        if(!state.setupApplied||state.blocked||state.inDispatch||state.vehicle248!=vehicle||
           !state.collisionCount||state.collisionCount>state.collision.size()||
           !state.driveCount||state.driveCount>state.drive.size()||
           channel.resultsStopCount26D070()!=state.collisionCount+state.driveCount||
           !channel.canStopResultsNative26D070(mixer))return false;
        for(std::size_t j=0;j<channel.resultsStopCount26D070();++j){
            const bool drive=j>=state.collisionCount;
            const auto key=drive?j-state.collisionCount:j;
            const auto& s=drive?state.drive[key].sound:state.collision[key];
            if(s.nativeOwner!=&channel||s.nativeSound!=&channel.sounds_[j]||
               s.authoredKey4!=key+1||s.groupState44!=group44||
               groups.binding(channel.resultsStopVoice26D070(j))!=Group::Effects110)return false;
        }
        if(human)continue;
        const auto& r=racers_[i];
        if(r.wrecks.resultsStopCount26D070()!=4||!r.wrecks.canStopResultsNative26D070(mixer)||
           !r.speechProvider.canStopResultsNative26D070(mixer,groups))return false;
        for(std::size_t j=0;j<4;++j){const auto& s=r.sounds[j+3];
            if(s.session!=session||!s.soundKey||s.nativeOwner!=&r.wrecks||
               s.nativeSound!=&r.wrecks.sounds_[j]||
               s.multiplier54!=&r.wrecks.sounds_[j].multiplier54||
               s.position60!=r.wrecks.sounds_[j].position60.data()||
               groups.binding(r.wrecks.resultsStopVoice26D070(j))!=Group::Engines114)return false;
        }
        // Actual prepareSpeech constructs only this racer's D48 categories.
        // Never let generic Provider support widen this particular owner.
        for(std::size_t n=0;n<gameplay_audio50HE::cueCount;++n){std::size_t slot;
            if(!r.speechProvider.resultsStopVoice26D070(n,slot))continue;
            const auto bank=gameplay_audio50HE::catalog[n].bank;
            if((bank!=rr::game::AudioBank50HE::PodCurses&&bank!=rr::game::AudioBank50HE::PodTaunts&&
                bank!=rr::game::AudioBank50HE::PodSpecial)||groups.binding(slot)!=Group::Character11C)return false;
        }
    }
    return mixer.canRecordControlChanges(resultsAuxiliaryStopCount26D070());
}

inline std::size_t native_original_ai_audio::Owner::resultsAuxiliaryStopCount26D070()const noexcept {
    std::size_t count=humanMaterials_.resultsStopCount26D070();
    for(std::size_t i=firstAiSlot_;i<racerCount_;++i){const auto& r=racers_[i];
        count+=r.materials.resultsStopCount26D070()+r.wrecks.resultsStopCount26D070();
        for(std::size_t n=0;n<gameplay_audio50HE::cueCount;++n){std::size_t slot;
            if(r.speechProvider.resultsStopVoice26D070(n,slot))++count;
        }
    }
    return count;
}
inline std::size_t native_original_ai_audio::Owner::resultsAuxiliaryStopVoice26D070(
    std::size_t ordinal)const noexcept {
    const auto humanCount=humanMaterials_.resultsStopCount26D070();
    if(ordinal<humanCount)return humanMaterials_.resultsStopVoice26D070(ordinal);
    ordinal-=humanCount;
    for(std::size_t i=firstAiSlot_;i<racerCount_;++i){const auto& r=racers_[i];
        for(const auto* channel:{&r.materials,&r.wrecks}){
            const auto count=channel->resultsStopCount26D070();
            if(ordinal<count)return channel->resultsStopVoice26D070(ordinal);
            ordinal-=count;
        }
        for(std::size_t n=0;n<gameplay_audio50HE::cueCount;++n){std::size_t slot;
            if(r.speechProvider.resultsStopVoice26D070(n,slot)){
                if(!ordinal)return slot;
                --ordinal;
            }
        }
    }
    return rr::platform::audioMaximumVoices50HD;
}
inline void native_original_ai_audio::Owner::stopResultsAuxiliaryPreflighted26D070()noexcept {
    humanMaterials_.stopResultsNativePreflighted26D070();
    for(std::size_t i=firstAiSlot_;i<racerCount_;++i){auto& r=racers_[i];
        r.materials.stopResultsNativePreflighted26D070();
        r.wrecks.stopResultsNativePreflighted26D070();
        r.speechProvider.stopResultsNativePreflighted26D070();
    }
    // No233670/2933D0: source channel/elapsed/selected-drive/cached slope,
    // D48 speech cursors/cooldowns, emitter/duck state and PCM owners survive.
}

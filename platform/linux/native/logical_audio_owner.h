#pragma once
#include "audio_mixer50hd.h"
#include <array>
#include <cstdint>
#include <limits>

namespace rr::platform {
// Native replacement for the source channel backend. Tokens are deliberately
// NOT PS2 channel numbers. Each token names one actual mixer playback revision;
// a replaced/reused slot never grants control of somebody else's sound.
class LogicalAudioOwner final {
public:
    enum class Result : std::uint8_t { complete, unavailable, owner, backend, exhausted };
    struct Sound {
        AudioClip50HD clip{};
        bool available=false;
        // Actual232ED0 logical fields, consumed by233670 and the AI emitter.
        std::int32_t channel2C=-1;
        std::uint32_t elapsedBits24=0,word7C=0;
        float multiplier54=1.f;
        std::array<float,4> position60{{0.f,0.f,0.f,1.f}};
    private:
        LogicalAudioOwner* owner=nullptr;
        std::size_t slot=0;
        std::uint64_t revision=0;
        std::int32_t token=-1;
        friend class LogicalAudioOwner;
    };
    LogicalAudioOwner()=default;
    ~LogicalAudioOwner(){close();}
    LogicalAudioOwner(const LogicalAudioOwner&)=delete;
    LogicalAudioOwner& operator=(const LogicalAudioOwner&)=delete;
    LogicalAudioOwner(LogicalAudioOwner&&)=delete;
    LogicalAudioOwner& operator=(LogicalAudioOwner&&)=delete;

    // Caller owns this exclusive range. Binding does not create PCM, sample
    // IDs, group gains, playback, or original factory/registration receipts.
    bool bind(AudioMixer50HD& mixer,std::size_t first,std::size_t count) noexcept {
        if(attempted_||!count||count>sounds_.size()||first>mixer.count()||
           count>mixer.count()-first)return false;
        attempted_=true;
        for(std::size_t i=0;i<count;++i)if(mixer.active(first+i))return false;
        mixer_=&mixer;count_=count;epoch_=mixer.groupEpoch50IC();
        for(std::size_t i=0;i<count;++i){sounds_[i].owner=this;sounds_[i].slot=first+i;}
        return true;
    }
    bool registerClip(std::size_t ordinal,const AudioClip50HD& clip,bool available) noexcept {
        if(!mixer_||ordinal>=count_||registered_[ordinal]||
           (available&&!AudioMixer50HD::validClip(clip)))return false;
        auto& s=sounds_[ordinal];s.clip=clip;s.available=available;
        registered_[ordinal]=true;return true;
    }
    Sound* sound(std::size_t ordinal) noexcept {
        return mixer_&&ordinal<count_&&registered_[ordinal]?&sounds_[ordinal]:nullptr;
    }
    bool active(const Sound& s) const noexcept {
        return owns(s)&&s.channel2C!=-1&&s.channel2C==s.token&&s.revision&&
            mixer_->playbackRevision(s.slot)==s.revision&&mixer_->active(s.slot);
    }
    Result start(Sound& s,float gain,float pitch,bool loop) noexcept {
        if(!owns(s))return Result::owner;
        if(!s.available)return Result::unavailable;
        // An active foreign revision is never stolen. Inactive slots are
        // reusable, and each successful start receives a fresh native token.
        if(mixer_->active(s.slot)&&!active(s))return Result::owner;
        if(nextToken_==std::numeric_limits<std::int32_t>::max())return Result::exhausted;
        if(!mixer_->canRecordControlChanges(1))return Result::exhausted;
        if(!mixer_->play(s.slot,s.clip,gain,pitch,loop,0.f))return Result::backend;
        s.token=++nextToken_;s.channel2C=s.token;s.revision=mixer_->playbackRevision(s.slot);
        return Result::complete;
    }
    Result controls(Sound& s,float gain,float pitch) noexcept {
        if(!active(s))return Result::owner;
        return mixer_->controls(s.slot,gain,pitch,0.f)?Result::complete:Result::backend;
    }
    //233670(a1=0), through233710's genuine active predicate. The native
    // backend stops actual PCM directly instead of reproducing the hardware
    // command queue. Source branch-specific logical writes remain exact.
    Result stop233670(Sound& s) noexcept {
        if(!owns(s))return Result::owner;
        if(s.channel2C==-1)return Result::complete;
        if(!active(s)){s.channel2C=-1;return Result::complete;}
        mixer_->stop(s.slot);
        s.elapsedBits24=0;s.channel2C=-1;s.word7C=0;
        return Result::complete;
    }
    void close() noexcept {
        if(!mixer_)return;
        for(std::size_t i=0;i<count_;++i){auto& s=sounds_[i];
            // Destruction must release owned PCM even if diagnostic revision
            // capacity was exhausted. It must never stop a foreign playback.
            if(active(s))mixer_->stop(s.slot);
            s.channel2C=-1;s.owner=nullptr;
        }
        mixer_=nullptr;count_=0;
    }
private:
    bool owns(const Sound& s) const noexcept {
        if(!mixer_||mixer_->groupEpoch50IC()!=epoch_||s.owner!=this)return false;
        for(std::size_t i=0;i<count_;++i)
            if(&sounds_[i]==&s)return registered_[i];
        return false;
    }
    AudioMixer50HD* mixer_=nullptr;
    std::array<Sound,audioMaximumVoices50HD> sounds_{};
    std::array<bool,audioMaximumVoices50HD> registered_{};
    std::size_t count_=0;
    std::int32_t nextToken_=0;
    std::uint64_t epoch_=0;
    bool attempted_=false;
};
}

#pragma once
// ONE native Dax episode owner. This retains the existing HOST request policy;
// it is not original category admission, RNG, priority, queue or delay logic.
#include "race_audio_groups50ic.h"

namespace rr::platform {
enum class HostAnnouncerResult50IC:std::uint8_t {Started,Busy,Rejected};
struct AnnouncerEpisodeCounters50IC {
    std::uint64_t starts=0,begins=0,ends=0,completions=0,preemptions=0,rejected=0;
};
class AnnouncerEpisode50IC final {
public:
    AnnouncerEpisode50IC()=default;
    AnnouncerEpisode50IC(const AnnouncerEpisode50IC&)=delete;
    AnnouncerEpisode50IC&operator=(const AnnouncerEpisode50IC&)=delete;
    AnnouncerEpisode50IC(AnnouncerEpisode50IC&&)=delete;
    AnnouncerEpisode50IC&operator=(AnnouncerEpisode50IC&&)=delete;
    bool prepare(AudioMixer50HD& mixer,RaceAudioGroups50IC& groups,std::size_t slot)noexcept {
        // This owner installs the binding itself. An already-bound slot cannot
        // be attached to a second adapter, even when its group would match.
        if(mixer_||!groups.uses(mixer)||slot>=mixer.count()||mixer.active(slot)||
           groups.binding(slot)!=game::RaceAudioGroup50IC::Unrepresented||
           !groups.bind(slot,game::RaceAudioGroup50IC::Announcer120))return false;
        mixer_=&mixer;groups_=&groups;slot_=slot;return true;
    }
    bool uses(const AudioMixer50HD& mixer)const noexcept {
        return mixer_==&mixer&&groups_&&groups_->uses(mixer);
    }
    bool active()const noexcept{return mixer_&&mixer_->active(slot_);}
    bool episode()const noexcept{return episode_;}
    std::size_t slot()const noexcept{return slot_;}
    void pause(bool paused)noexcept{paused_=paused;}
    // Called after the actual common mix and before another request. Pause
    // never advances this voice. No duration estimate or separate host timer.
    bool pollCompletion()noexcept {
        if(!mixer_)return true;
        if(!groups_->uses(*mixer_))return false;
        if(episode_&&!active()){
            if(!restore())return false;
            ++counters_.completions;
        }
        // Detect raw playback outside an owned episode. Replacing a voice during
        // an episode is prohibited by the single-writer contract, not detected.
        return episode_||!active();
    }
    HostAnnouncerResult50IC requestHost(const AudioClip50HD& clip,bool preempt)noexcept {
        if(!mixer_||paused_||!groups_->uses(*mixer_)||!AudioMixer50HD::validClip(clip)||
           !pollCompletion())return rejected();
        if(active()&&!preempt)return HostAnnouncerResult50IC::Busy;
        if(active()){
            mixer_->stop(slot_);
            if(!restore())return rejected();
            ++counters_.preemptions;
        }
        const auto begun=groups_->begin();
        if(!begun.complete)return rejected();
        ++counters_.begins;episode_=true;
        if(!mixer_->play(slot_,clip,1.f,1.f,false)){
            // Retain episode_=true if restoration itself fails so a subsequent
            // completion/stop can retry it; never silently strand ducked gains.
            restore();return rejected();
        }
        ++counters_.starts;return HostAnnouncerResult50IC::Started;
    }
    // Host reset/close, or an explicit speech cancellation. Stop then restore;
    // this is independent of pause and idempotent after successful restoration.
    bool stop()noexcept {
        if(!mixer_)return true;
        if(!groups_->uses(*mixer_))return false;
        mixer_->stop(slot_);return !episode_||restore();
    }
    const AnnouncerEpisodeCounters50IC& counters()const noexcept{return counters_;}
private:
    bool restore()noexcept {
        if(!groups_->end().complete)return false;
        episode_=false;++counters_.ends;return true;
    }
    HostAnnouncerResult50IC rejected()noexcept{++counters_.rejected;return HostAnnouncerResult50IC::Rejected;}
    AudioMixer50HD* mixer_=nullptr;
    RaceAudioGroups50IC* groups_=nullptr;
    std::size_t slot_=audioMaximumVoices50HD;
    bool episode_=false,paused_=false;
    AnnouncerEpisodeCounters50IC counters_{};
};
} // namespace rr::platform

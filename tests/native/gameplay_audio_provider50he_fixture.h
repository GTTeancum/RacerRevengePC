#pragma once
#include "../../src/game/gameplay_audio50he.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include <array>
#include <cmath>
#include <string>

namespace live_audio50HF { class Owner; }
namespace native_original_ai_audio { class Owner; }
namespace rr::platform { class RaceAudioGroups50IC; }

namespace gameplay_audio50HE {
using namespace rr::game;
struct CueDefinition {AudioBank50HE bank;std::uint32_t key;const char* filename;bool authoredLoop,stream;};
inline constexpr CueDefinition catalog[]={
#include "audio_cue_catalog50he.inc"
    // Additional POD19 semantic keys. No default filename: only its own
    // authenticated category files may populate these selected-player slots.
    {AudioBank50HE::PodCurses,20u,nullptr,false,true},
    {AudioBank50HE::PodCurses,21u,nullptr,false,true},
    {AudioBank50HE::PodCurses,22u,nullptr,false,true},
    {AudioBank50HE::PodCurses,23u,nullptr,false,true},
    {AudioBank50HE::PodCurses,24u,nullptr,false,true},
    {AudioBank50HE::PodSpecial,10u,nullptr,false,true},
    {AudioBank50HE::PodSpecial,11u,nullptr,false,true},
};
inline constexpr std::size_t cueCount=sizeof(catalog)/sizeof(*catalog);
inline constexpr std::size_t legacyCueCount=60,extraFirstVoice=152;
static_assert(cueCount==67,"Retained Pod01/TA keys plus seven genuine POD19 keys");
// Preserve every existing reservation: lap60..63, material64..79, race80..95,
// opponents96..116, music117, GC118..125, Gate126, GA128..151.
inline constexpr std::size_t cueVoice(std::size_t first,std::size_t index) noexcept {
    return index<legacyCueCount?first+index:extraFirstVoice+index-legacyCueCount;
}
inline std::size_t cueIndex(AudioCue50HE cue) noexcept {
    for(std::size_t n=0;n<cueCount;++n)
        if(catalog[n].bank==cue.bank&&catalog[n].key==cue.key)return n;
    return cueCount;
}
inline const char* cueFilename(AudioCue50HE cue) noexcept {
    const auto n=cueIndex(cue);return n<cueCount?catalog[n].filename:nullptr;
}
enum class ApplyResult : std::uint8_t { Started,Stopped,Controlled,Inactive,Unavailable,
    Unprepared,InvalidCue,InvalidCommand,PlaybackRejected };
class Provider final {
public:
    Provider()=default;Provider(const Provider&)=delete;Provider& operator=(const Provider&)=delete;
    Provider(Provider&&)=delete;Provider& operator=(Provider&&)=delete;
    // Cold only. Registers the selected original authored names. Legacy default
    // is all60 POD01/TA cues; an explicit selection can leave unauthored slots
    // empty. No new event is invented merely because its sample is registered.
    // Policy: one dedicated mixer slot per cue (bounded restart of same cue).
    // An explicit map admits a separately owned, compact per-racer D48 range;
    // its caller must authenticate the selected catalog and reserve that range.
    // Native lap owner needs a separate voice range. Bank PCM must stay alive
    // and immutable. It is permissible that every sample is explicitly missing.
    bool prepare(const rr::assets::AudioBank50HC& bank,rr::platform::AudioMixer50HD& mixer,
        std::size_t firstVoice,std::string& error,
        const std::array<rr::assets::AudioRequest50HC,cueCount>* selected=nullptr,
        const std::array<std::size_t,cueCount>* explicitVoices=nullptr) {
        if(mixer_||firstVoice>mixer.count()||
           (!explicitVoices&&legacyCueCount>mixer.count()-firstVoice)) {
            error="50HE provider requires a fresh owner and60 dedicated mixer voices";return false;
        }
        std::array<rr::platform::AudioClip50HD,cueCount> prepared{};
        std::array<bool,cueCount> ready{},authored{};std::size_t loaded=0;
        std::array<std::size_t,cueCount> voiceSlots{};
        for(std::size_t n=0;n<cueCount;++n) {
            // A selected player's empty slot is genuinely unauthored, not a
            // request to fall back to the legacy POD01 catalog filename.
            if(selected?(*selected)[n].name.empty():catalog[n].filename==nullptr)continue;
            const auto voice=explicitVoices?(*explicitVoices)[n]:cueVoice(firstVoice,n);
            if(voice>=mixer.count()||(!explicitVoices&&n>=legacyCueCount&&voice>=firstVoice&&
                voice-firstVoice<legacyCueCount)) {
                error="50HE selected extra speech voice overlaps or exceeds mixer capacity";return false;
            }
            for(std::size_t previous=0;previous<n;++previous)
                if(authored[previous]&&voiceSlots[previous]==voice){
                    error="50HE authored cues require distinct owned voices";return false;
                }
            voiceSlots[n]=voice;authored[n]=true;
            const std::string name=selected?(*selected)[n].name:catalog[n].filename;
            const bool loop=selected?(*selected)[n].loop:catalog[n].authoredLoop;
            const bool stream=selected?(*selected)[n].stream:catalog[n].stream;
            const auto* item=bank.find(name);
            if(!item){error="50HE catalog name absent from resolved bank: "+name;return false;}
            if(item->request.stereo||(selected&&(*selected)[n].stereo)||
                item->request.loop!=loop||item->request.stream!=stream) {
                error="50HE catalog authoring differs from bank: "+name;return false;
            }
            if(item->state!=rr::assets::AudioAssetState50HC::ready)continue;
            const auto& pcm=item->pcm;
            prepared[n]={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,
                pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
            if(pcm.channels!=1||pcm.frames!=pcm.samples.size()||!rr::platform::AudioMixer50HD::validClip(prepared[n])) {
                error="50HE ready clip has invalid PCM: "+name;return false;
            }
            ready[n]=true;++loaded;
        }
        clips_=prepared;ready_=ready;authored_=authored;loaded_=loaded;voiceSlots_=voiceSlots;
        mixer_=&mixer;epoch_=mixer.groupEpoch50IC();error.clear();return true;
    }
    // Hot, ordered, allocation-free. Return values report actual mixer action;
    // registration, an emitted command, or missing bytes are never playback.
    ApplyResult apply(const AudioCommand50HE& command) noexcept {
        if(!mixer_)return ApplyResult::Unprepared;
        const auto index=cueIndex(command.cue);if(index==cueCount||!authored_[index])return ApplyResult::InvalidCue;
        if(!std::isfinite(command.gain)||command.gain<0.f||!std::isfinite(command.pitch)||command.pitch<=0.f)
            return ApplyResult::InvalidCommand;
        const auto slot=voiceSlots_[index];
        if(command.operation==AudioOperation50HE::Stop) {
            const bool active=mixer_->active(slot);mixer_->stop(slot);++stops_;
            return active?ApplyResult::Stopped:ApplyResult::Inactive;
        }
        if(!ready_[index]){++unavailable_;return ApplyResult::Unavailable;}
        if(command.operation==AudioOperation50HE::Controls) {
            if(!mixer_->active(slot))return ApplyResult::Inactive;
            if(!mixer_->controls(slot,command.gain,command.pitch))return ApplyResult::PlaybackRejected;
            ++controls_;return ApplyResult::Controlled;
        }
        if(command.operation!=AudioOperation50HE::Start)return ApplyResult::InvalidCommand;
        if(!mixer_->play(slot,clips_[index],command.gain,command.pitch,command.loop))return ApplyResult::PlaybackRejected;
        revisions_[index]=mixer_->playbackRevision(slot);
        ++starts_;return ApplyResult::Started;
    }
    void stopAll() noexcept {
        if(mixer_)for(std::size_t n=0;n<cueCount;++n)if(authored_[n])mixer_->stop(voiceSlots_[n]);
    }
    bool available(AudioCue50HE cue)const noexcept {
        const auto index=cueIndex(cue);return mixer_&&index<cueCount&&ready_[index];
    }
    bool active(AudioCue50HE cue)const noexcept {
        const auto index=cueIndex(cue);return mixer_&&index<cueCount&&authored_[index]&&
            mixer_->active(voiceSlots_[index]);
    }
    std::size_t loaded()const noexcept{return loaded_;}
    std::uint64_t starts()const noexcept{return starts_;}
    std::uint64_t stops()const noexcept{return stops_;}
    std::uint64_t controls()const noexcept{return controls_;}
    std::uint64_t unavailable()const noexcept{return unavailable_;}
private:
    friend class ::live_audio50HF::Owner;
    friend class ::native_original_ai_audio::Owner;
    bool canStopResultsNative26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&)const noexcept;
    bool resultsStopVoice26D070(std::size_t,std::size_t&)const noexcept;
    void stopResultsNativePreflighted26D070()noexcept;
    std::uint64_t epoch_=0;
    std::array<std::uint64_t,cueCount> revisions_{};
    rr::platform::AudioMixer50HD* mixer_=nullptr;
    std::array<rr::platform::AudioClip50HD,cueCount> clips_{};
    std::array<bool,cueCount> ready_{},authored_{};
    std::array<std::size_t,cueCount> voiceSlots_{};
    std::size_t loaded_=0;
    std::uint64_t starts_=0,stops_=0,controls_=0,unavailable_=0;
};
}

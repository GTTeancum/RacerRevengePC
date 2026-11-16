#pragma once
#include "../../src/assets/audio_assets50hc.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

// PCM provider only. Source action admission belongs to trigger_actions50in.h.
// This owner never makes a hit, picks a listener, runs a timer, synthesizes a
// substitute sound, or controls the shared output device/pause lifecycle.
namespace gc_gate_audio50IN {
namespace a=rr::assets;namespace p=rr::platform;namespace g=rr::game;
inline constexpr std::size_t voice=126;
inline constexpr const char* filename="sfx/race/bigdoor.vag";
inline constexpr const char* sourceSha256="d06f02f4e58684bdf39ca66999daea436bd34d0b35df0bad1ea1b413bdc6c746";
enum class Failure:std::uint8_t {None,Unprepared,Ownership,VoiceOccupied,BackendRejected};
struct Counters {std::uint64_t requests=0,starts=0,unavailable=0,rejected=0;};
// Stable address; shared mixer/groups MUST outlive this owner.126 is exclusive,
// outside current0..109 gameplay/race/opponents,117music,118..125GCmaterials.
// Preparation refuses a pre-bound/active126. Authoring's once-only Gate never
// requests overlapping starts. A second start while playing fails explicitly.
class Owner final {
    a::AudioBank50HC bank_;
    p::AudioClip50HD clip_{};
    p::AudioMixer50HD*mixer_=nullptr;
    p::RaceAudioGroups50IC*groups_=nullptr;
    bool attempted_=false,prepared_=false,available_=false;
    Failure failure_=Failure::None;
    Counters counters_{};
    bool ownership()const noexcept {
        return prepared_&&mixer_&&groups_&&groups_->uses(*mixer_)&&
            voice<mixer_->count()&&mixer_->hasVoiceGroupGain50IC(voice)&&
            groups_->binding(voice)==g::RaceAudioGroup50IC::World4D8;
    }
public:
    Owner()=default;
    ~Owner()noexcept{if(ownership())mixer_->stop(voice);}
    Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
    // requested: the PodSound action's own Filename (default: stock BIGDOOR).
    // The stock file keeps its exact identity/framing checks; another file is
    // admitted by the shared audio bank and mixer clip rules only.
    bool prepare(p::AudioMixer50HD&mixer,p::RaceAudioGroups50IC&groups,
        const std::filesystem::path&audioRoot,std::string&error,std::string requested=filename){
        if(attempted_){error="50IN Gate audio requires a fresh stable owner";return false;}
        attempted_=true;
        if(!groups.uses(mixer)||voice>=mixer.count()||mixer.active(voice)||
           groups.binding(voice)!=g::RaceAudioGroup50IC::Unrepresented||
           !groups.canBind(voice,g::RaceAudioGroup50IC::World4D8)){
            error="50IN Gate audio exclusive voice126/source World4D8 unavailable";return false;
        }
        try {
            for(auto&c:requested){if(c=='\\')c='/';if(c>='A'&&c<='Z')c=static_cast<char>(c+32);}
            const bool stockFile=requested==filename;
            if(!stockFile)std::cerr<<"course_member_identity site=pod_sound member="<<requested<<" stock=0\n";
            const std::vector<a::AudioRequest50HC> requests={{requested.c_str(),false,false,false}};
            if(!a::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
            const auto*sound=bank_.find(requested.c_str());
            if(!sound||bank_.clips.size()!=1){error="50IN Gate audio catalog differs";return false;}
            const bool present=sound->state==a::AudioAssetState50HC::ready;
            if(present){
                const auto&pcm=sound->pcm;
                if(stockFile&&(sound->sourceSha256!=sourceSha256||sound->sourceBytes!=26128||
                   pcm.sourceSha256!=sourceSha256||pcm.sampleRate!=22050||
                   pcm.channels!=1||pcm.frames!=45612||pcm.samples.size()!=45612||
                   pcm.hasEmbeddedLoop||pcm.loopStart||pcm.loopEnd)){
                    error="50IN Gate original BIGDOOR identity/mono/end framing differs";return false;
                }
                clip_={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
                if(!p::AudioMixer50HD::validClip(clip_)){
                    error="50IN Gate original PCM view rejected";return false;
                }
            }else if(sound->state!=a::AudioAssetState50HC::missing){
                error="50IN Gate original BIGDOOR is invalid or unsupported: "+sound->detail;return false;
            }
            if(!groups.bind(voice,g::RaceAudioGroup50IC::World4D8)){
                error="50IN Gate source World4D8 binding rejected";return false;
            }
            mixer_=&mixer;groups_=&groups;available_=present;prepared_=true;error.clear();return true;
        }catch(const std::exception&e){error=e.what();return false;}
        catch(...){error="50IN Gate audio preparation failed";return false;}
    }
    // Pure preflight. Missing PCM is a represented unavailable action/no-op,
    // not a fake playback or permission to select a fallback sound.
    bool ready()const noexcept {
        return !failed()&&ownership()&&!mixer_->active(voice)&&
            (!available_||p::AudioMixer50HD::validClip(clip_));
    }
    void start()noexcept {
        ++counters_.requests;
        if(failed()){++counters_.rejected;return;}
        if(!prepared_){failure_=Failure::Unprepared;++counters_.rejected;return;}
        if(!ownership()){failure_=Failure::Ownership;++counters_.rejected;return;}
        if(mixer_->active(voice)){failure_=Failure::VoiceOccupied;++counters_.rejected;return;}
        if(!available_){++counters_.unavailable;return;}
        // Constructor volume100,center pan,base pitch. Group gain belongs ONLY
        // to World4D8.1B5B00's mask2 is immediate submission,not a loop request.
        if(!mixer_->play(voice,clip_,1.f,1.f,false,0.f)){
            failure_=Failure::BackendRejected;++counters_.rejected;return;
        }
        ++counters_.starts;
    }
    bool failed()const noexcept{return failure_!=Failure::None;}
    Failure failure()const noexcept{return failure_;}
    bool available()const noexcept{return prepared_&&available_;}
    const Counters&counters()const noexcept{return counters_;}
    const char*error()const noexcept {
        switch(failure_){
            case Failure::None:return "";
            case Failure::Unprepared:return "50IN Gate audio start before preparation";
            case Failure::Ownership:return "50IN Gate audio source group/mixer lifetime changed";
            case Failure::VoiceOccupied:return "50IN Gate once-only voice126 is already active";
            case Failure::BackendRejected:return "50IN Gate native mixer rejected preflighted start";
        }
        return "50IN Gate audio unknown failure";
    }
};
}

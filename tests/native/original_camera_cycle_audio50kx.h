#pragma once
#include "race_audio50hq_fixture.h"

namespace original_camera_cycle_audio50KX {
// Actual1A42C0->race10C+160 response emitter uses raceUI.ini and group124;
//176DF0 calls1A4B90(key4)->292360(key4,flags1,a3=0). The native playback
//policy is one exclusive restarting PCM voice, not PS2 channel emulation.
//All other race/mixer owners and their ordering remain the existing services.
class Owner final {
    rr::assets::AudioBank50HC bank_;
    rr::platform::AudioMixer50HD* mixer_=nullptr;
    rr::platform::RaceAudioGroups50IC* groups_=nullptr;
    rr::platform::AudioClip50HD clip_{};
    bool attempted_=false,available_=false,muted_=false,closed_=false;
public:
    static constexpr std::size_t voice=159;
    enum class Result {started,unavailable,muted,unprepared,rejected};
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    ~Owner() noexcept {close();}
    // Explicit native RR_AUDIO=0 output policy. The real camera event still
    // completes, but this is neither successful playback nor missing PCM.
    bool prepareMuted() noexcept {
        if(attempted_||closed_)return false;
        attempted_=true;muted_=true;return true;
    }
    void close() noexcept {
        if(mixer_)mixer_->stop(voice);
        mixer_=nullptr;groups_=nullptr;closed_=true;
    }
    bool prepare(const std::filesystem::path& discRoot,const std::filesystem::path& audioRoot,
        rr::platform::AudioMixer50HD& mixer,rr::platform::RaceAudioGroups50IC& groups,
        std::string& error){
        namespace d=pod_materials50ed_fixture::detail;
        if(attempted_||closed_){error="camera cycle audio requires fresh owner";return false;}attempted_=true;
        try{
            d::need(groups.uses(mixer)&&voice<mixer.count()&&!mixer.active(voice)&&
                groups.binding(voice)==rr::game::RaceAudioGroup50IC::Unrepresented,
                "camera cycle audio voice159 is unavailable or already owned");
            d::Inputs input;input.capacity=509;
            const auto bytes=input.read(discRoot/"AUDIO/RACEUI.INI",509);
            d::need(bytes.size()==509&&d::sha256(bytes)=="0358ce0f68065d521b022f939b9ea903804d0479a5ce9536ba8b87dde2212aaa",
                "original raceUI.ini identity differs");
            const auto authored=race_audio50HQ::detail::readEvents(bytes,7);
            const auto& event4=authored[3];
            d::need(!event4.loop&&!event4.stereo&&!event4.stream&&event4.name=="SFX\\UI\\right.vag",
                "original camera cycle event4 differs");
            const std::vector<rr::assets::AudioRequest50HC> requests{event4};
            if(!rr::assets::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
            d::need(bank_.clips.size()==1,"camera cycle audio registry is incomplete");
            const auto& sound=bank_.clips[0];
            if(sound.state==rr::assets::AudioAssetState50HC::ready){
                const auto& pcm=sound.pcm;clip_={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
                d::need(pcm.channels==1&&rr::platform::AudioMixer50HD::validClip(clip_),
                    "camera cycle ready PCM is invalid");available_=true;
            }
            d::need(groups.bind(voice,rr::game::RaceAudioGroup50IC::Auxiliary124),
                "camera cycle actual group124 bind failed");
            mixer_=&mixer;groups_=&groups;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
    }
    Result dispatchEvent4() noexcept {
        if(closed_)return Result::rejected;
        if(muted_)return Result::muted;
        if(!mixer_)return Result::unprepared;
        if(!groups_->uses(*mixer_)||groups_->binding(voice)!=rr::game::RaceAudioGroup50IC::Auxiliary124)
            return Result::rejected;
        if(!available_)return Result::unavailable; // Actual unresolved sample, no silent invented PCM.
        return mixer_->play(voice,clip_,1.f,1.f,false)?Result::started:Result::rejected;
    }
};
}

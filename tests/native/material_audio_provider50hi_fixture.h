#pragma once
#include "../../src/assets/material_audio50hi.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "pod_materials50ed_fixture.h"
#include <array>
#include <filesystem>
#include <string>
namespace material_audio50HI {
inline constexpr std::size_t soundCount=16;
enum class ApplyResult : std::uint8_t { Started,Stopped,Controlled,Inactive,Unavailable,Rejected };
struct Sound {
    rr::platform::AudioClip50HD clip{};
    rr::assets::AudioAssetState50HC availability=rr::assets::AudioAssetState50HC::missing;
};
struct Receipt {
    rr::game::AudioEventStatus50HE status=rr::game::AudioEventStatus50HE::Input;
    std::size_t commandCount=0;
    std::array<rr::game::MaterialAudioCommand50HI,rr::game::maxMaterialAudioCommands50HI> commands{};
    std::array<ApplyResult,rr::game::maxMaterialAudioCommands50HI> results{};
    bool okay()const noexcept{return status==rr::game::AudioEventStatus50HE::Complete;}
};
struct Counters {std::uint64_t updates=0,starts=0,stops=0,controls=0,inactive=0,unavailable=0,rejected=0;};
// Host subscriber only. It does NOT register a synthetic canonical D40, assert
// original virtual dispatch occurred, or acknowledge an absent game callback.
class Provider final {
public:
    Provider()=default;Provider(const Provider&)=delete;Provider&operator=(const Provider&)=delete;
    Provider(Provider&&)=delete;Provider&operator=(Provider&&)=delete;
    bool prepare(const std::filesystem::path&discRoot,const std::filesystem::path&audioRoot,
        rr::platform::AudioMixer50HD&mixer,std::size_t firstVoice,std::string&error){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{d::need(firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,"50HI dedicated16-slot range unavailable");
            d::Inputs files;files.capacity=3334562;auto bytes=files.read(discRoot/"TRACKS/TA.RES",3334562);
            d::need(bytes.size()==3334562&&d::sha256(bytes)=="b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8","50HI original TA archive identity differs");
            a::ResArchive48C archive;a::DecodeError48C decode;std::vector<std::uint8_t>aub;
            d::need(a::decodeResource48C(bytes,archive,decode)&&a::extractResMember48C(archive,"tracks\\ta.aub",aub,decode),"50HI actual TA.aub unavailable");
            d::need(aub.size()==25431&&d::sha256(aub)=="7474707d4182f29cc8b24203680aa311c9749e745078f3d80545f93f8d97f017","50HI original TA.aub identity differs");
            std::vector<a::AudioRequest50HC>requests;
            d::need(a::readMaterialAudioCatalog50HI(aub,catalog_,requests,error),"50HI material authoring rejected");
            d::need(requests.size()==13&&a::loadAudioBank50HC(audioRoot,requests,bank_,error),"50HI real material PCM bank failed");
            for(std::size_t i=0;i<soundCount;++i){const auto*clip=bank_.find(i<8?catalog_.collision[i]:catalog_.drive[i-8]);d::need(clip,"50HI catalog row missing bank record");auto&s=sounds_[i];s.availability=clip->state;
                if(clip->state==a::AudioAssetState50HC::ready){const auto&p=clip->pcm;s.clip={p.samples.data(),p.frames,p.sampleRate,p.channels,p.hasEmbeddedLoop?p.loopStart:0,p.hasEmbeddedLoop?p.loopEnd:0};d::need(rr::platform::AudioMixer50HD::validClip(s.clip),"50HI ready PCM invalid");}}
            firstVoice_=firstVoice;mixer_=&mixer;error.clear();return true;
        }catch(const std::exception&e){error=e.what();return false;}
    }
    ApplyResult apply(const rr::game::MaterialAudioCommand50HI&c)noexcept{
        using namespace rr::game;
        if(!mixer_||c.key<1||c.key>8||(c.kind!=MaterialCueKind50HI::Collision&&c.kind!=MaterialCueKind50HI::DriveOn)){++counters_.rejected;return ApplyResult::Rejected;}
        const auto i=(c.kind==MaterialCueKind50HI::Collision?0u:8u)+c.key-1;const auto slot=firstVoice_+i;
        if(c.operation==AudioOperation50HE::Stop){mixer_->stop(slot);++counters_.stops;return ApplyResult::Stopped;}
        if(sounds_[i].availability!=rr::assets::AudioAssetState50HC::ready){++counters_.unavailable;return ApplyResult::Unavailable;}
        if(c.operation==AudioOperation50HE::Controls){if(!mixer_->active(slot)){++counters_.inactive;return ApplyResult::Inactive;}if(mixer_->controls(slot,c.gain,c.pitch)){++counters_.controls;return ApplyResult::Controlled;}}
        else if(c.operation==AudioOperation50HE::Start){
            // Actual292B80/292DA0 do not restart a sound owner already playing.
            if(mixer_->active(slot)){++counters_.inactive;return ApplyResult::Inactive;}
            if(mixer_->play(slot,sounds_[i].clip,c.gain,c.pitch,c.loop)){++counters_.starts;return ApplyResult::Started;}
        }
        ++counters_.rejected;return ApplyResult::Rejected;
    }
    Receipt update(const rr::game::MaterialAudioInput50HI&input,std::uint32_t&sharedRandomState)noexcept{
        Receipt r;if(!mixer_)return r;
        auto next=state_;r.status=rr::game::updateMaterialAudio50HI(catalog_.parameters,input,state_,next,sharedRandomState,r.commands.data(),r.commands.size(),r.commandCount);
        if(!r.okay())return r;state_=next;++counters_.updates;
        for(std::size_t i=0;i<r.commandCount;++i){r.results[i]=apply(r.commands[i]);if(r.results[i]==ApplyResult::Rejected)r.status=rr::game::AudioEventStatus50HE::Input;}return r;
    }
    void reset()noexcept{if(mixer_)for(std::size_t i=0;i<soundCount;++i)mixer_->stop(firstVoice_+i);state_={};}
    bool prepared()const noexcept{return mixer_!=nullptr;}
    const rr::assets::AudioBank50HC&bank()const noexcept{return bank_;}
    const rr::assets::MaterialAudioCatalog50HI&catalog()const noexcept{return catalog_;}
    const rr::game::MaterialAudioState50HI&state()const noexcept{return state_;}
    const Counters&counters()const noexcept{return counters_;}
private:
    rr::assets::AudioBank50HC bank_{};
    rr::assets::MaterialAudioCatalog50HI catalog_{};
    std::array<Sound,soundCount>sounds_{};
    rr::platform::AudioMixer50HD*mixer_=nullptr;
    std::size_t firstVoice_=0;
    rr::game::MaterialAudioState50HI state_{};
    Counters counters_{};bool attempted_=false;
};
}

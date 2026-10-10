#pragma once
#include "../../src/assets/material_audio50hi.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "pod_materials50ed_fixture.h"
#include <array>
#include <filesystem>
#include <string>
namespace material_audio50HI {
inline constexpr std::size_t soundCount=16; // Preserved TA base voice reservation64..79.
inline constexpr std::size_t maximumSoundCount=24,extraFirstVoice=118,maximumExtraVoices=8;
enum class Track:std::uint8_t {ta,gc};
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
        rr::platform::AudioMixer50HD&mixer,std::size_t firstVoice,std::string&error,Track track=Track::ta){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{d::need(firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,"50HI dedicated16-slot range unavailable");
            const bool gc=track==Track::gc;
            const std::size_t archiveBytes=gc?4205067u:3334562u;
            const char* archiveSha=gc?"69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9":
                "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8";
            d::Inputs files;files.capacity=archiveBytes;
            auto bytes=files.read(discRoot/(gc?"TRACKS/GC.RES":"TRACKS/TA.RES"),archiveBytes);
            d::need(bytes.size()==archiveBytes&&d::sha256(bytes)==archiveSha,"50HI selected original archive identity differs");
            a::ResArchive48C archive;a::DecodeError48C decode;std::vector<std::uint8_t>aub;
            d::need(a::decodeResource48C(bytes,archive,decode)&&
                a::extractResMember48C(archive,gc?"tracks\\gc.aub":"tracks\\ta.aub",aub,decode),"50HI selected original AUB unavailable");
            d::need(aub.size()==(gc?32438u:25431u)&&d::sha256(aub)==(gc?
                "eb4046a25b5b08412c8fa8644107ab826499af754f1093283cc4566e2d879c1d":
                "7474707d4182f29cc8b24203680aa311c9749e745078f3d80545f93f8d97f017"),"50HI selected original AUB identity differs");
            std::vector<a::AudioRequest50HC>requests;
            d::need(a::readMaterialAudioCatalog50HI(aub,catalog_,requests,error),"50HI material authoring rejected");
            rowCount_=catalog_.parameters.rowCount;activeSoundCount_=2u*rowCount_;
            d::need(rowCount_==(gc?12u:8u)&&catalog_.materialCount==(gc?4u:1u),"50HI selected material profile differs");
            d::need(requests.size()==(gc?21u:13u)&&a::loadAudioBank50HC(audioRoot,requests,bank_,error),"50HI real material PCM bank failed");
            // Actual12+12 rows keep TA's64..79 unchanged; eight additional
            // native sound owners use118..125, clear of Dax/racer/music slots.
            if(gc)d::need(firstVoice+soundCount<=extraFirstVoice&&
                extraFirstVoice+maximumExtraVoices<=mixer.count(),"50HI exclusive extra material slots unavailable");
            for(std::size_t i=0;i<activeSoundCount_;++i){
                const bool drive=i>=rowCount_;const auto key=i%rowCount_;
                slots_[i]=key<8?firstVoice+(drive?8u:0u)+key:
                    extraFirstVoice+(drive?4u:0u)+key-8u;
                if(gc)d::need(!mixer.hasVoiceGroupGain50IC(slots_[i]),"50HI material voice already belongs to another provider");
                const auto*clip=bank_.find(drive?catalog_.drive[key]:catalog_.collision[key]);
                d::need(clip,"50HI catalog row missing bank record");auto&sound=sounds_[i];sound.availability=clip->state;
                if(clip->state==a::AudioAssetState50HC::ready){const auto&p=clip->pcm;
                    sound.clip={p.samples.data(),p.frames,p.sampleRate,p.channels,p.hasEmbeddedLoop?p.loopStart:0,p.hasEmbeddedLoop?p.loopEnd:0};
                    d::need(rr::platform::AudioMixer50HD::validClip(sound.clip),"50HI ready PCM invalid");}
            }
            firstVoice_=firstVoice;mixer_=&mixer;error.clear();return true;
        }catch(const std::exception&e){error=e.what();return false;}
    }
    ApplyResult apply(const rr::game::MaterialAudioCommand50HI&c)noexcept{
        using namespace rr::game;
        if(!mixer_||c.key<1||c.key>rowCount_||(c.kind!=MaterialCueKind50HI::Collision&&c.kind!=MaterialCueKind50HI::DriveOn)){++counters_.rejected;return ApplyResult::Rejected;}
        const auto i=(c.kind==MaterialCueKind50HI::Collision?0u:rowCount_)+c.key-1;const auto slot=slots_[i];
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
    void reset()noexcept{if(mixer_)for(std::size_t i=0;i<activeSoundCount_;++i)mixer_->stop(slots_[i]);state_={};}
    std::size_t extraVoiceCount()const noexcept{return activeSoundCount_>soundCount?activeSoundCount_-soundCount:0;}
    bool prepared()const noexcept{return mixer_!=nullptr;}
    const rr::assets::AudioBank50HC&bank()const noexcept{return bank_;}
    const rr::assets::MaterialAudioCatalog50HI&catalog()const noexcept{return catalog_;}
    const rr::game::MaterialAudioState50HI&state()const noexcept{return state_;}
    const Counters&counters()const noexcept{return counters_;}
private:
    rr::assets::AudioBank50HC bank_{};
    rr::assets::MaterialAudioCatalog50HI catalog_{};
    std::array<Sound,maximumSoundCount>sounds_{};
    std::array<std::size_t,maximumSoundCount>slots_{};
    rr::platform::AudioMixer50HD*mixer_=nullptr;
    std::size_t firstVoice_=0,rowCount_=8,activeSoundCount_=soundCount;
    rr::game::MaterialAudioState50HI state_{};
    Counters counters_{};bool attempted_=false;
};
}

#pragma once
// ONE material-audio provider for every course (PLAN B-C17). Original 271600
// (the AUB [Sounds] material catalog, 271674..271804 assignments) and
// 279B60/279E50 (collision and drive-on sound rows) read whatever AUB the
// course archive holds; nothing here names a course. The provider is created
// once per race by the 05.12 step (common_track50CR::readAub271600) from the
// Track's own AUB member and published as Track::roles.audio.
#include "../../src/assets/material_audio50hi.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "pod_materials50ed_fixture.h"
#include "audio_retained_payload50mc.h"
#include <array>
#include <cctype>
#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>
namespace material_audio50HI {
// Native host voice layout (one rule, every course). PORT MAPPING (flagged):
// the original's sound owners are not host mixer voices; the port gives the
// first 8 collision and 8 drive-on rows the base range 64..79 and every further
// row the exclusive block 192..219 (below the per-racer material rows), clear
// of GC/Gate 118..126, the positional pool 128..151, gameplay 152..158,
// camera 159 and the AI engines 160..191. The per-course extra blocks
// (118/128/192 with 4..28 voices) it replaces are gone.
inline constexpr std::size_t baseFirstVoice=64;             // base host range start (receipt ALL aub.first_voice)
inline constexpr std::size_t soundCount=16;                 // base rows 64..79 (8 collision + 8 drive-on)
inline constexpr std::size_t maximumSoundCount=44;          // largest native row layout (16 + 28)
inline constexpr std::size_t extraFirstVoice=192,maximumExtraVoices=maximumSoundCount-soundCount;
static_assert(extraFirstVoice+maximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
static_assert(extraFirstVoice+maximumExtraVoices<=rr::platform::audioMaximumVoices50HD);
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
    // 271600 for the race's course: `aub` is the Track's own tracks\<code>.aub
    // member (stock disc or registry dlc:/ archive, the same bytes the 05.12
    // step read). `code` is data (log lines only). Row and material counts
    // come from the AUB, bounded by the native sound stores.
    bool prepareTrack(const std::vector<std::uint8_t>& aub,std::string_view code,
        const std::filesystem::path& audioRoot,rr::platform::AudioMixer50HD& mixer,
        std::size_t firstVoice,std::string& error){
        namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{d::need(firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,"50HI dedicated16-slot range unavailable");
            d::need(!aub.empty(),"50HI course AUB unavailable");
            code_=std::string(code);
            return prepareCatalog(aub,audioRoot,mixer,firstVoice,error);
        }catch(const std::exception&e){error=e.what();return false;}
    }
    const std::string& code()const noexcept{return code_;}
private:
    bool prepareCatalog(const std::vector<std::uint8_t>& aub,const std::filesystem::path& audioRoot,
        rr::platform::AudioMixer50HD& mixer,std::size_t firstVoice,std::string& error){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        try{
            std::vector<a::AudioRequest50HC>requests;
            d::need(a::readMaterialAudioCatalog50HI(aub,catalog_,requests,error),"50HI material authoring rejected");
            collisionRowCount_=catalog_.parameters.rowCount;driveRowCount_=catalog_.parameters.driveRowCount;
            activeSoundCount_=collisionRowCount_+driveRowCount_;uniqueClips_=requests.size();
            // PORT STAND-IN (flagged): the native voice layout assumes at least
            // the 8+8 base rows (fewer would underflow its extra-slot arithmetic)
            // and at most the 44-row store; another AUB is refused, logged.
            d::need(collisionRowCount_>=8&&driveRowCount_>=8&&activeSoundCount_<=maximumSoundCount,"50HI material rows outside the native sound layout (8..44)");
            d::need(a::loadAudioBank50HC(audioRoot,requests,bank_,error),"50HI real material PCM bank failed");
            // All source owners retain distinct slots (a section reread by
            // 157D00 still owns its own slot). Only PCM is shared.
            const auto extraCollisionRows=collisionRowCount_-8u;
            const auto extraRows=activeSoundCount_-soundCount;
            if(extraRows)d::need(firstVoice+soundCount<=extraFirstVoice&&
                extraFirstVoice+extraRows<=mixer.count(),"50HI exclusive extra material slots unavailable");
            for(std::size_t i=0;i<activeSoundCount_;++i){
                const bool drive=i>=collisionRowCount_;const auto key=drive?i-collisionRowCount_:i;
                slots_[i]=key<8?firstVoice+(drive?8u:0u)+key:
                    extraFirstVoice+(drive?extraCollisionRows:0u)+key-8u;
                d::need(!mixer.hasVoiceGroupGain50IC(slots_[i])&&!mixer.active(slots_[i]),"50HI material voice already belongs to another provider");
                const auto*clip=bank_.find(drive?catalog_.drive[key]:catalog_.collision[key]);
                d::need(clip,"50HI catalog row missing bank record");auto&sound=sounds_[i];sound.availability=clip->state;
                if(clip->state==a::AudioAssetState50HC::ready){const auto&p=clip->pcm;
                    sound.clip={p.samples.data(),p.frames,p.sampleRate,p.channels,p.hasEmbeddedLoop?p.loopStart:0,p.hasEmbeddedLoop?p.loopEnd:0};
                    d::need(rr::platform::AudioMixer50HD::validClip(sound.clip),"50HI ready PCM invalid");}
            }
            firstVoice_=firstVoice;mixer_=&mixer;error.clear();return true;
        }catch(const std::exception&e){error=e.what();return false;}
    }
public:
    ApplyResult apply(const rr::game::MaterialAudioCommand50HI&c)noexcept{
        using namespace rr::game;
        if(!mixer_||(c.kind!=MaterialCueKind50HI::Collision&&c.kind!=MaterialCueKind50HI::DriveOn)){++counters_.rejected;return ApplyResult::Rejected;}
        const bool drive=c.kind==MaterialCueKind50HI::DriveOn;
        if(c.key<1||c.key>(drive?driveRowCount_:collisionRowCount_)){++counters_.rejected;return ApplyResult::Rejected;}
        const auto i=(drive?collisionRowCount_:0u)+c.key-1;const auto slot=slots_[i];
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
    std::size_t collisionRowCount()const noexcept{return collisionRowCount_;}
    std::size_t driveRowCount()const noexcept{return driveRowCount_;}
    std::size_t extraVoiceFirst()const noexcept{return extraFirstVoice;}
    std::size_t extraVoiceCount()const noexcept{return activeSoundCount_>soundCount?activeSoundCount_-soundCount:0;}
    bool prepared()const noexcept{return mixer_!=nullptr;}
    std::size_t materialCount()const noexcept{return catalog_.materialCount;}
    std::size_t uniqueClipCount()const noexcept{return uniqueClips_;}
    // Includes this Provider's inline storage once, excludes retained shared source.
    std::size_t retainedPayloadBytes()const noexcept{
        namespace payload=audio_retained_payload50MC;
        std::size_t bytes=sizeof(*this)+payload::bankDynamicBytes(bank_)+
            catalog_.orderedObjectMaterials.capacity()*sizeof(rr::assets::MaterialAudioCatalog50HI::ObjectMaterial);
        for(const auto& name:catalog_.collision)bytes+=payload::externalStringBytes(name);
        for(const auto& name:catalog_.drive)bytes+=payload::externalStringBytes(name);
        for(const auto& row:catalog_.orderedObjectMaterials)bytes+=payload::externalStringBytes(row.object);
        return bytes;
    }
    std::size_t retainedPcmCapacityBytes()const noexcept{return audio_retained_payload50MC::bankPcmCapacityBytes(bank_);}
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
    std::string code_;
    std::size_t firstVoice_=0,collisionRowCount_=8,driveRowCount_=8,activeSoundCount_=soundCount,uniqueClips_=0;
    rr::game::MaterialAudioState50HI state_{};
    Counters counters_{};bool attempted_=false;
};
}

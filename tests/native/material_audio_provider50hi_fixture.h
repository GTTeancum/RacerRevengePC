#pragma once
#include "../../src/assets/material_audio50hi.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "pod_materials50ed_fixture.h"
#include <array>
#include <filesystem>
#include <string>
namespace material_audio50HI {
inline constexpr std::size_t soundCount=16; // Preserved TA base voice reservation64..79.
inline constexpr std::size_t maximumSoundCount=44,extraFirstVoice=118,maximumExtraVoices=8;
// Keep GC 118..125 and Gate 126 intact. GA exclusively owns 128..151.
inline constexpr std::size_t gaExtraFirstVoice=128,gaMaximumExtraVoices=24;
// MA/SA are selected-track-exclusive. Neither aliases GC/Gate, GA,
// gameplay152..158, camera159, nor original AI engines160..191.
inline constexpr std::size_t maSaExtraFirstVoice=192,maMaximumExtraVoices=12,saMaximumExtraVoices=28;
static_assert(soundCount+saMaximumExtraVoices==maximumSoundCount);
static_assert(gaExtraFirstVoice+gaMaximumExtraVoices<=maSaExtraFirstVoice);
static_assert(maSaExtraFirstVoice+saMaximumExtraVoices<=rr::platform::audioMaximumVoices50HD);
// RA measured12+12 distinct host rows:16 base plus8 extra. This range
// is selected-track-exclusive, with its own bound rather than an MA/SA count.
inline constexpr std::size_t raExtraFirstVoice=192,raMaximumExtraVoices=8;
static_assert(soundCount+raMaximumExtraVoices==24);
static_assert(raExtraFirstVoice+raMaximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
enum class Track:std::uint8_t {ta,gc,ga,ma,sa,ra};
struct TrackProfile {
    Track track;const char* archiveName;std::size_t archiveBytes;const char* archiveSha;
    const char* aubName;std::size_t aubBytes;const char* aubSha;
    std::uint32_t collisionRows,driveRows,materials;std::size_t uniqueClips,extraVoiceFirst;
};
inline constexpr TrackProfile trackProfiles[]{
    {Track::ta,"TRACKS/TA.RES",3334562u,"b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8",
     "tracks\\ta.aub",25431u,"7474707d4182f29cc8b24203680aa311c9749e745078f3d80545f93f8d97f017",8,8,1,13,extraFirstVoice},
    {Track::gc,"TRACKS/GC.RES",4205067u,"69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",
     "tracks\\gc.aub",32438u,"eb4046a25b5b08412c8fa8644107ab826499af754f1093283cc4566e2d879c1d",12,12,4,21,extraFirstVoice},
    {Track::ga,"TRACKS/GA.RES",2834942u,"063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9",
     "tracks\\ga.aub",41363u,"8302ea938033d539feece2a13ebc2f3c2171938be13770afdebc980940a043ec",20,20,4,37,gaExtraFirstVoice},
    {Track::ma,"TRACKS/MA.RES",3691592u,"2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228",
     "tracks\\ma.aub",39850u,"1cbf1814fd87cc684fcca94b6980d8d40c323f12cc2ddff6f2a94d376f938dc4",16,12,3,23,maSaExtraFirstVoice},
    {Track::sa,"TRACKS/SA.RES",3786696u,"34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a",
     "tracks\\sa.aub",44414u,"ec272d864f435354904f87f2a240d6f52e1a23060c0b8c9b8e803cd79d1fe887",32,12,2,23,maSaExtraFirstVoice},
    {Track::ra,"TRACKS/RA.RES",4604561u,"1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2",
     "tracks\\ra.aub",22511u,"c0977667b46b918872c3d351f745ce59260037b9f604b76aea23b11bb5c88c6a",12,12,1,21,raExtraFirstVoice}
};
inline constexpr const TrackProfile* profile(Track track)noexcept{
    for(const auto& p:trackProfiles)if(p.track==track)return &p;return nullptr;
}
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
            const auto* selected=profile(track);d::need(selected,"50HI unsupported source track");
            d::Inputs files;files.capacity=selected->archiveBytes;
            auto bytes=files.read(discRoot/selected->archiveName,selected->archiveBytes);
            d::need(bytes.size()==selected->archiveBytes&&d::sha256(bytes)==selected->archiveSha,"50HI selected original archive identity differs");
            a::ResArchive48C archive;a::DecodeError48C decode;std::vector<std::uint8_t>aub;
            d::need(a::decodeResource48C(bytes,archive,decode)&&
                a::extractResMember48C(archive,selected->aubName,aub,decode),"50HI selected original AUB unavailable");
            d::need(aub.size()==selected->aubBytes&&d::sha256(aub)==selected->aubSha,"50HI selected original AUB identity differs");
            std::vector<a::AudioRequest50HC>requests;
            d::need(a::readMaterialAudioCatalog50HI(aub,catalog_,requests,error),"50HI material authoring rejected");
            collisionRowCount_=catalog_.parameters.rowCount;driveRowCount_=catalog_.parameters.driveRowCount;
            activeSoundCount_=collisionRowCount_+driveRowCount_;
            d::need(collisionRowCount_==selected->collisionRows&&driveRowCount_==selected->driveRows&&
                catalog_.materialCount==selected->materials&&activeSoundCount_<=maximumSoundCount,"50HI selected material profile differs");
            if(track==Track::ma||track==Track::sa||track==Track::ra)d::need(catalog_.defaultMaterial==1,"50HI selected default material differs");
            // RA declares NumMaterials1. Material2 is outside271674..271804.
            if(track==Track::ra){
                d::need(catalog_.orderedObjectMaterials.empty(),"RA material assignment traversal differs");
                for(std::size_t i=0;i<12;++i)d::need(catalog_.collisionSectionKeys[i]==i+1&&
                    catalog_.driveSectionKeys[i]==i+1,"RA exact source sections differ");
            }
            d::need(requests.size()==selected->uniqueClips&&a::loadAudioBank50HC(audioRoot,requests,bank_,error),"50HI real material PCM bank failed");
            // All source owners retain distinct slots, including SA17..32,
            // whose source section selection rereads16. Only PCM is shared.
            extraFirstVoice_=selected->extraVoiceFirst;
            const auto extraCollisionRows=collisionRowCount_-8u;
            const auto extraRows=activeSoundCount_-soundCount;
            if(extraRows)d::need(firstVoice+soundCount<=extraFirstVoice_&&
                extraFirstVoice_+extraRows<=mixer.count(),"50HI exclusive extra material slots unavailable");
            for(std::size_t i=0;i<activeSoundCount_;++i){
                const bool drive=i>=collisionRowCount_;const auto key=drive?i-collisionRowCount_:i;
                slots_[i]=key<8?firstVoice+(drive?8u:0u)+key:
                    extraFirstVoice_+(drive?extraCollisionRows:0u)+key-8u;
                if(track!=Track::ta)d::need(!mixer.hasVoiceGroupGain50IC(slots_[i])&&!mixer.active(slots_[i]),"50HI material voice already belongs to another provider");
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
    std::size_t extraVoiceFirst()const noexcept{return extraFirstVoice_;}
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
    std::size_t firstVoice_=0,extraFirstVoice_=extraFirstVoice,collisionRowCount_=8,driveRowCount_=8,activeSoundCount_=soundCount;
    rr::game::MaterialAudioState50HI state_{};
    Counters counters_{};bool attempted_=false;
};
}

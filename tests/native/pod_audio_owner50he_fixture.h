#pragma once
// Cold authenticated six-event Pod01 owner with a real native PCM provider.
// This supplies the specific lap292360->233410 call, not a complete D40 owner.
#include "lap_sound_owner50gh_fixture.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include <limits>

namespace pod_audio50HE {
using SoundFields=lap_sound50GH::SoundFields;
using EventAuthoring=lap_sound50GH::EventAuthoring;
inline constexpr std::size_t eventCount=lap_sound50GH::eventCount;
struct Sound {
    EventAuthoring authored{};
    SoundFields fields{};
    rr::platform::AudioClip50HD clip{};
    rr::assets::AudioAssetState50HC availability=rr::assets::AudioAssetState50HC::missing;
};
struct ResourceRow {Sound* owner=nullptr;std::int32_t sampleResult=-1;};
struct Owner {
    std::array<Sound,eventCount> sounds{};
    std::array<ResourceRow,eventCount> rows{};
    Sound* selectedOwner1F0=nullptr;
    rr::platform::AudioMixer50HD* mixer=nullptr;
    std::size_t firstVoice=0,voiceCount=0;
    std::uint64_t providerRequests=0,playbackCalls=0,started=0,unavailable=0,noChannel=0;
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    bool prepared()const noexcept{return mixer!=nullptr;}
};

// Bank PCM must remain immutable and alive longer than owner/mixer voices.
// Every authored row must exist in the bank; missing samples must be explicit
// missing/unsupported/invalid records, never absent registry backing. Provider
// sample indices are native bounded indices0..5, never fake device handles.
// The mixer voice range belongs exclusively to this owner. Cold allocations
// and archive reads finish before committing the owner. Missing PCM is allowed
// and remains a real negative resolve result; it never becomes a silent clip.
inline bool prepareFromPod01(Owner& out,rr::platform::AudioMixer50HD& mixer,
    const rr::assets::AudioBank50HC& bank,std::size_t firstVoice,std::size_t voiceCount,
    std::string& error,const std::filesystem::path& archivePath=
        "work/native/project/disc/PODS/POD01/POD01.RES") {
    namespace a=rr::assets;namespace shared=pod_materials50ed_fixture::detail;
    try {
        shared::need(!out.prepared()&&voiceCount&&firstVoice<mixer.count()&&
            voiceCount<=mixer.count()-firstVoice,"50HE fresh owner and real mixer voice range required");
        shared::Inputs input;input.capacity=lap_sound50GH::podArchiveBytes;
        auto bytes=input.read(archivePath,lap_sound50GH::podArchiveBytes);
        shared::need(bytes.size()==lap_sound50GH::podArchiveBytes&&
            shared::sha256(bytes)==lap_sound50GH::podArchiveSha256,"50HE Pod01 archive identity mismatch");
        a::ResArchive48C archive;a::DecodeError48C decode;a::DecodeLimits48C limits;
        limits.resource_file_bytes=lap_sound50GH::podArchiveBytes;
        limits.resource_decoded_bytes=16u*1024u*1024u;limits.resource_entries=512;limits.resource_name_bytes=256;
        shared::need(a::decodeResource48C(bytes,archive,decode,nullptr,limits),"50HE Pod01 archive decode failed");
        std::vector<std::uint8_t> member;
        shared::need(a::extractResMember48C(archive,lap_sound50GH::eventMember,member,decode,
            lap_sound50GH::eventMemberBytes),"50HE p_event member missing");
        shared::need(member.size()==lap_sound50GH::eventMemberBytes&&
            shared::sha256(member)==lap_sound50GH::eventMemberSha256,"50HE p_event identity mismatch");
        const auto authored=lap_sound50GH::detail::parse(member);
        std::array<Sound,eventCount> prepared{};
        for(std::size_t n=0;n<eventCount;++n) {
            const auto* source=bank.find(std::string(authored[n].filename.data()));
            shared::need(source!=nullptr,"50HE authored sample lacks a resolved bank record");
            shared::need(source->request.loop==authored[n].loop&&
                source->request.stereo==authored[n].stereo&&source->request.stream==authored[n].stream,
                "50HE authored sample flags differ from bank request");
            auto& sound=prepared[n];sound.authored=authored[n];sound.availability=source->state;
            sound.fields.flags10=1u|(sound.authored.loop?0x80u:0u);
            sound.fields.loop1B0=sound.authored.loop?1u:0u;
            sound.fields.resourceIndexA0=static_cast<std::int32_t>(n);
            if(source->state==a::AudioAssetState50HC::ready) {
                const auto& pcm=source->pcm;
                sound.clip={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,
                    pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
                shared::need(pcm.channels==1&&pcm.frames==pcm.samples.size()&&
                    rr::platform::AudioMixer50HD::validClip(sound.clip),"50HE ready sample has invalid PCM storage");
                sound.fields.sample28=static_cast<std::int32_t>(n);
                // Native provider exposes normalized pitch4096. We deliberately
                // do not fabricate PS2 bank/hardware metadata or duration20.
                sound.fields.word50=4096;sound.fields.word40=4096;sound.fields.word4C=4096;
            }
        }
        // All throwing/cold operations have finished; stable native references
        // and actual sample resolutions are published together.
        out.sounds=prepared;
        for(std::size_t n=0;n<eventCount;++n)
            out.rows[n]={&out.sounds[n],out.sounds[n].fields.sample28};
        out.firstVoice=firstVoice;out.voiceCount=voiceCount;out.providerRequests=eventCount;
        out.mixer=&mixer;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50HE native event owner preparation failed";}catch(...){}return false;}
}
enum class DispatchStatus : std::uint8_t { CompletedStarted,CompletedUnavailable,
    CompletedNoChannel,Unprepared,UnsupportedRequest,InvalidOwner };
struct DispatchReceipt {
    DispatchStatus status=DispatchStatus::Unprepared;
    std::int32_t originalV0=0,sample=-1,channel=-1;
    std::uint32_t selectedKey=0;
    bool selectionStored=false,sampleRefreshed=false,playbackCalled=false;
    SoundFields before{},after{};
    bool completed()const noexcept {
        return status==DispatchStatus::CompletedStarted||status==DispatchStatus::CompletedUnavailable||
            status==DispatchStatus::CompletedNoChannel;
    }
};
// Only flags0/nonstream lap Event1 is admitted. The selected-owner store happens
// before sample refresh/play. A successful result is backed by mixer.play and
// mixer.active; missing PCM returns0 before channel/24/7C stores. Voice capacity
// failure is the actual native provider negative channel result. No callbacks,
// fake successes, allocation, file reads or decoding occur on this hot path.
inline DispatchReceipt dispatchLap(Owner& owner,std::uint32_t key=1,
    std::uint32_t flags=0,std::uint32_t argumentA3=0) noexcept {
    DispatchReceipt r{};
    if(!owner.mixer)return r;
    if(key!=1||flags||argumentA3){r.status=DispatchStatus::UnsupportedRequest;return r;}
    if(!owner.voiceCount||owner.firstVoice>=owner.mixer->count()||
        owner.voiceCount>owner.mixer->count()-owner.firstVoice){r.status=DispatchStatus::InvalidOwner;return r;}
    for(std::size_t n=0;n<eventCount;++n) {
        const auto& s=owner.sounds[n];const auto& row=owner.rows[n];
        if(s.authored.key!=n+1||row.owner!=&s||s.fields.resourceIndexA0!=static_cast<std::int32_t>(n)||
           s.authored.stereo||s.authored.stream||s.authored.loop!=(n>=2)||
           row.sampleResult!=s.fields.sample28||
           (row.sampleResult>=0&&(row.sampleResult!=static_cast<std::int32_t>(n)||
               s.availability!=rr::assets::AudioAssetState50HC::ready||
               !rr::platform::AudioMixer50HD::validClip(s.clip)))) {
            r.status=DispatchStatus::InvalidOwner;return r;
        }
    }
    auto& sound=owner.sounds[0];r.before=sound.fields;r.selectedKey=1;
    // Original292810 precedes233410.
    owner.selectedOwner1F0=&sound;r.selectionStored=true;
    if(sound.fields.sample28<0) {
        sound.fields.sample28=owner.rows[0].sampleResult;r.sampleRefreshed=true;
    }
    r.sample=sound.fields.sample28;
    if(r.sample<0) {
        ++owner.unavailable;r.status=DispatchStatus::CompletedUnavailable;r.after=sound.fields;return r;
    }
    const auto slot=owner.mixer->availableVoice(owner.firstVoice,owner.voiceCount);
    // Native provider call is real even when capacity is exhausted: play on its
    // invalid sentinel slot returnsfalse without modifying any mixer voice.
    ++owner.playbackCalls;r.playbackCalled=true;
    const bool started=owner.mixer->play(slot,sound.clip,
        static_cast<float>(sound.fields.word38)/100.f,
        static_cast<float>(sound.fields.word40)/static_cast<float>(sound.fields.word50),false);
    sound.fields.channel2C=started?static_cast<std::int32_t>(slot):-1;
    sound.fields.word4C=sound.fields.word40;sound.fields.word44=sound.fields.word38;
    sound.fields.word48=sound.fields.word3C;
    r.channel=sound.fields.channel2C;
    if(!started){++owner.noChannel;r.status=DispatchStatus::CompletedNoChannel;r.after=sound.fields;return r;}
    sound.fields.word24=0;sound.fields.word7C=1;++owner.started;
    r.originalV0=1;r.status=DispatchStatus::CompletedStarted;r.after=sound.fields;return r;
}
}

#pragma once
#include "audio_mixer50hd.h"
#include "../../../src/assets/audio_assets50hc.h"
#include "../../../src/assets/original_ta_intro_authoring_candidate.h"
#include "../../../tests/native/original_ui_source_timing50lh.h"
#include <memory>
#include <vector>
namespace rr::platform {
class OriginalApplicationAural50KG;
class RaceAudioGroups50IC;
// Native PCM transport plus genuine same-Aural source stream/channel custody.
// It is not a source Sound, group, emitter, Entity or a second audio clock.
class NativeRaceIntroTransport50LA final {
    friend class RaceAudioGroups50IC;
    friend class OriginalApplicationAural50KG;
public:
    struct Playback {std::size_t voice=0;std::uint64_t revision=0,serial=0;};
    enum class Dispatch : std::uint8_t { unavailable,sourceFalse,sourceTrue };
    ~NativeRaceIntroTransport50LA()noexcept;
    NativeRaceIntroTransport50LA(const NativeRaceIntroTransport50LA&)=delete;
    NativeRaceIntroTransport50LA& operator=(const NativeRaceIntroTransport50LA&)=delete;
private:
    // Caller must already bind the exclusive range to its actual Announcer120
    // group, retain the immutable preload, and keep mixer storage alive.
    static std::unique_ptr<NativeRaceIntroTransport50LA> create(
        std::shared_ptr<const OriginalApplicationAural50KG>,AudioMixer50HD&,
        std::size_t firstVoice,std::size_t count,
        std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>,
        std::shared_ptr<const assets::AudioBank50HC>)noexcept;
    bool current()const noexcept;
    bool currentWithMixerPause(bool)const noexcept;
    std::int32_t lastSourceResult()const noexcept{return lastSourceResult_;}
    static bool renderCurrent(const void*,std::size_t)noexcept;
    bool canPlay(unsigned eventId)const noexcept;
    Dispatch play(unsigned eventId,Playback&)noexcept;
    // False means unavailable identity, not a false source activity result.
    bool active(const Playback&,bool& sourceActive)const noexcept;
    bool stop(const Playback&,bool& wasSourceActive)noexcept;
    // Actual Application23C must already be published by the Root issuer.
    // Returns the SAME admitted factor for following Sound233E00 traversal.
    bool service(const original_ui_source_timing50LH::PublicationView&,float& factor)noexcept;
    bool canSetPaused(bool)const noexcept;
    bool setPaused(bool)noexcept;
    bool drain()noexcept;
    bool release()noexcept;
    struct Assets {
        std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate> authoring;
        std::shared_ptr<const assets::AudioBank50HC> bank;
        std::array<const assets::AudioClip50HC*,assets::originalRaceIntroEventsMax50LA> pcm{};
        std::array<AudioClip50HD,assets::originalRaceIntroEventsMax50LA> clips{};
        const assets::OriginalStreamMetadataRow50KZ* source(unsigned,const AudioClip50HD&)const noexcept;
    };
    struct Instance {
        Playback playback{};
        NativeSourceVoiceRoutes50KG::Route route{};
        unsigned eventId=0;
    };
    NativeRaceIntroTransport50LA()noexcept=default;
    std::shared_ptr<const OriginalApplicationAural50KG> aural_;
    std::shared_ptr<const Assets> assets_;
    AudioMixer50HD* mixer_=nullptr;
    std::size_t first_=0,count_=0;
    std::uint64_t epoch_=0,nextSerial_=0,lastPublication_=0;
    std::uint32_t rate_=0;
    std::int32_t lastSourceResult_=0;
    original_ui_source_timing50LH::PublicationView timing_;
    std::vector<Instance> instances_;
    bool leased_=false,released_=false,paused_=false;
};
}

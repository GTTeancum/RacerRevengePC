#pragma once
#include "original_ui_main_audio50kg.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <array>
#include <memory>
#include <thread>

namespace original_main_entry50KG { class AudioOutput; }
namespace rr::game { class OriginalApplicationEntities50LA; }
namespace rr::platform {
// Private native storage allocated cold, not an output device, a source
// hardware channel budget, a menu capability or a claim of Garage entry.
// Its root lifetime may retain ordinary frontend audio across source entries.
// The actual Application publishes its attempted owner BEFORE preparation.
// MainAudio aliases this control block, so Actors/Watto/SoundGroup retaining
// MainAudio also retain its exact mixer, voice array and sound-mode borrow.
class OriginalUiGarageAudioBacking50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_main_entry50KG::AudioOutput;
    friend class OriginalApplicationAural50KG;
    friend class OriginalUiMainAudio50KG;
    using Application=::original_main_entry50KG::Application;
    enum class Stage : std::uint8_t {
        published,mixerInitialized,soundModeBound,metadataPrepared,
        transportPrepared,transportFinished,closed
    };
    explicit OriginalUiGarageAudioBacking50KG(const Application&) noexcept;
    bool sameRoot(const Application&)const noexcept;
    bool inputsCurrent(const Application&)const noexcept;
    const Application* issuer_=nullptr;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    game::OriginalApplicationAnimationClockView50KG issuerLifetime_;
    std::shared_ptr<game::OriginalApplicationEntities50LA> authority_;
    std::filesystem::path assetRoot_,audioRoot_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart_;
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> metadata_;
    // Destruction order is intentional: MainAudio resets while mixer, voices,
    // source-mode and authenticated authoring still exist. Never replace any
    // of these stable-address members or construct an independent Main owner.
    std::shared_ptr<OriginalSoundMode50KG> soundMode_;
    std::array<AudioVoice50HD,OriginalUiMainAudio50KG::garageSceneRequiredVoices> voices_{};
    AudioMixer50HD mixer_;
    OriginalUiMainAudio50KG transport_;
    std::uint64_t mixerEpoch_=0;
    std::uint32_t nativeOutputRate_=0;
    Stage stage_=Stage::published;
    bool published_=false,failed_=false,closed_=false;
    // Actual successive output/device-close history, never a device-ready
    // flag. Each new native output pins the same transport and captures its
    // real cumulative mixer origin; source startup remains one-shot.
    ::original_main_entry50KG::AudioOutput* frontendOutputOwner_=nullptr;
    const game::OriginalWreckComponent50LA* frontendOutputAuralEntity_=nullptr;
    bool frontendOutputAttempted_=false,frontendOutputClosed_=false,frontendOutputCloseFailed_=false;
    std::uint64_t frontendOutputSerial_=0;
    bool frontendOutputAccountingFailed_=false;
    std::uint64_t frontendClosedSourceFrames_=0,frontendClosedMainRevision_=0,frontendClosedMixerRevision_=0;
    // Bounded latest actual268300 descendant of THIS closed output, not a
    // replacement device-close receipt. Original close revisions/frame/voice
    // history above survive. Cleared only by a genuine subsequent close.
    std::uint64_t frontendClosedReset268300MainRevision_=0,frontendClosedReset268300MixerRevision_=0;
    // Same-Aural synchronous stop descendant only; original close history
    // is retained, never a device-close replay or a generic revision waiver.
    std::uint64_t frontendClosedStop26D070MainRevision_=0,frontendClosedStop26D070MixerRevision_=0;
    std::array<std::uint64_t,OriginalUiMainAudio50KG::garageSceneRequiredVoices> frontendClosedPlaybackRevisions_{};
    std::array<double,OriginalUiMainAudio50KG::garageSceneRequiredVoices> frontendClosedSourceFramesByVoice_{};
public:
    ~OriginalUiGarageAudioBacking50KG()noexcept;
    OriginalUiGarageAudioBacking50KG(const OriginalUiGarageAudioBacking50KG&)=delete;
    OriginalUiGarageAudioBacking50KG& operator=(const OriginalUiGarageAudioBacking50KG&)=delete;
    OriginalUiGarageAudioBacking50KG(OriginalUiGarageAudioBacking50KG&&)=delete;
    OriginalUiGarageAudioBacking50KG& operator=(OriginalUiGarageAudioBacking50KG&&)=delete;
};
}

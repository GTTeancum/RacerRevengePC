#pragma once
#include "audio_mixer50hd.h"
#include "audio_stereo_producer50hd.h"
#include "../../../src/assets/audio_assets50hc.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

namespace rr::platform {
// Host-only frontend transport. The application supplies a DEDICATED freshly
// initialized mixer/voice array, never the live race mixer or its slot117.
// This transport must be reset and its device output closed before race output
// begins. Mixer/storage outlive this stable-address owner. All calls main-thread.
class OriginalUiMainAudio50KG final : public AudioStereoProducer50HD {
public:
    static constexpr std::size_t musicVoice = 0, firstEffectVoice = 1;
    static constexpr std::size_t effectVoiceCount = 4, requiredVoices = 5;
    OriginalUiMainAudio50KG() = default;
    ~OriginalUiMainAudio50KG() noexcept override { reset(); }
    OriginalUiMainAudio50KG(const OriginalUiMainAudio50KG&) = delete;
    OriginalUiMainAudio50KG& operator=(const OriginalUiMainAudio50KG&) = delete;
    OriginalUiMainAudio50KG(OriginalUiMainAudio50KG&&) = delete;
    OriginalUiMainAudio50KG& operator=(OriginalUiMainAudio50KG&&) = delete;
    // AUDIO itself is the explicit root. Cold reads authenticate exact metadata
    // and original VAGs before decoding. Missing/changed bytes fail; no substitute.
    bool prepare(const std::filesystem::path& audioRoot, AudioMixer50HD& dedicatedFrontendMixer,
        std::string& error) noexcept;
    bool ready() const noexcept override;
    std::uint32_t sampleRate() const noexcept override {
        return current() ? mixer_->sampleRate() : 0;
    }
    // Source1E68CC ->234900(stream,9,1). Session owns +44/+48 admission checks.
    // Replaces only this frontend music voice, loops original PCM [0,frames).
    bool startMainLoop9() noexcept;
    // Source loop-mode234540, targetvolume0, duration1000ms for Main B4.
    // Bounded source calls admitted: zero immediate stop or exactly1 second.
    // Native policy: linear gain at accepted source-update boundaries, followed
    // by the source0.35s silent retirement margin. This is not IOP timing emulation.
    bool fadeMain(float seconds = 1.f) noexcept;
    // Authenticated UIEVENTS IDs1(select),3(left),4(right),7(badcall), all one-shot.
    // Source292810 flags1 ->233410 stops an active sound before restart.
    // One voice per original event; different IDs may overlap.
    bool playMenuEffect(std::uint32_t originalId) noexcept;
    bool update(float sourceSeconds) noexcept;
    void pause(bool value) noexcept { paused_ = value; }
    bool paused() const noexcept { return paused_; }
    // Application must use this gate for the dedicated frontend mixer. Paused
    // calls write silence without consuming PCM. No device callback uses this.
    bool mixStereo16(std::int16_t* output, std::size_t frames) noexcept override;
    void reset() noexcept;
    bool mainLoopActive() const noexcept;
    float mainGain() const noexcept { return musicGain_; }
private:
    bool current() const noexcept;
    bool attempted_ = false, prepared_ = false, paused_ = false;
    bool ownsMusic_ = false, fading_ = false;
    AudioMixer50HD* mixer_ = nullptr;
    std::uint64_t mixerEpoch_ = 0;
    std::size_t mixerCount_ = 0;
    float musicGain_ = 0.4f, fadeStartGain_ = 0.4f;
    float fadeElapsed_ = 0.f, fadeDuration_ = 0.f, fadeRetireAt_ = 0.f;
    // Original UI setup1B7944 music40/100 and1B78D0 effects100/100.
    std::array<assets::AudioPcm50HC, 5> pcm_{};
    std::array<AudioClip50HD, 5> clips_{};
    std::array<bool, effectVoiceCount> ownsEffect_{};
};
}

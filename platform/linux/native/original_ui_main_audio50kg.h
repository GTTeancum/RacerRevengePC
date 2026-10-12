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
    static constexpr std::size_t effectVoiceCount = 6, requiredVoices = 7;
    static constexpr std::size_t riderConfirmClipCount = 19;
    // One accepted default-unlocked Rider Confirm per page lifetime. Its UIACCEPT stream
    // borrows a separate native voice; it cannot replace an active utterance.
    static constexpr std::size_t riderConfirmVoice = requiredVoices, courseRequiredVoices = requiredVoices + 1;
    enum class SteadyRiderError : std::uint8_t {
        none, preparationUnavailable, mixerUnavailable, mixerEpochChanged,
        mixerShapeChanged, revisionUnavailable, paused, soundModeUnavailable,
        soundModeOwnerChanged, unsupportedRouting, unsupportedGainLayer,
        wrongMusic, nonsteadyEnvelope, invalidMusicClip, musicVoiceChanged,
        foreignEffect, invalidEffectClip, effectVoiceChanged
    };
    // Read-only owner facts, not a caller-writable admission flag or a lease.
    // A future application lease must retain this exact transport/output
    // lifetime, match its actual application mode owner, and revalidate all
    // three control revisions at every checkpoint. PCM cursors may progress.
    struct SteadyTransportObservation {
        const OriginalUiMainAudio50KG* transport = nullptr;
        const AudioMixer50HD* mixer = nullptr;
        const OriginalSoundMode50KG* soundMode = nullptr;
        std::uint64_t controlRevision = 0, mixerControlRevision = 0;
        std::uint64_t mixerEpoch = 0, soundModeRevision = 0, sourceMixedFrames = 0;
        std::size_t voiceCount = 0, rejectedVoice = courseRequiredVoices;
        std::uint32_t outputRate = 0, musicSong = 0, profileSoundMode = 0;
        float masterGain = 0.f, musicGain = 0.f;
        bool paused = false, ownsMusic = false, fading = false;
        AudioVoiceObservation50HD music{};
        std::array<AudioVoiceObservation50HD, effectVoiceCount> effects{};
        SteadyRiderError error = SteadyRiderError::preparationUnavailable;
        const char* detail = "Rider steady transport not observed";
    };
    struct RiderAdvanceCommandObservation {
        const OriginalUiMainAudio50KG* transport = nullptr;
        const AudioMixer50HD* mixer = nullptr;
        const OriginalSoundMode50KG* soundMode = nullptr;
        std::uint64_t controlRevisionBefore = 0, controlRevisionAfter = 0;
        std::uint64_t mixerControlRevisionBefore = 0, mixerControlRevisionAfter = 0;
        std::uint64_t mixerEpoch = 0, soundModeRevision = 0, sourceMixedFrames = 0;
        std::uint32_t eventId = 0;
        std::size_t voiceSlot = courseRequiredVoices;
        AudioVoiceObservation50HD voice{};
    };
    class RiderAdvanceCommandReceipt final {
    public:
        bool issued() const noexcept { return issued_; }
        const RiderAdvanceCommandObservation& observation() const noexcept { return observation_; }
    private:
        friend class OriginalUiMainAudio50KG;
        bool issued_ = false;
        RiderAdvanceCommandObservation observation_{};
    };
    class RiderConfirmPlayback final {
    public:
        bool issued() const noexcept { return transport_ != nullptr; }
    private:
        friend class OriginalUiMainAudio50KG;
        const OriginalUiMainAudio50KG* transport_ = nullptr;
        std::uint64_t revision_ = 0;
    };
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
    // Optional additional cold original Song7. Call after prepare and BEFORE
    // any playback/mixing. Main/Options require no Rider music bytes. Failure
    // leaves their already prepared transport intact and Rider unavailable.
    bool prepareRiderMenu(const std::filesystem::path& audioRoot, std::string& error) noexcept;
    // Optional Course destination, before any playback: original Song11 and
    // all 19 default-unlocked Rider Confirm choices. Native preload of authored IsStream=1
    // speech uses the existing decoder/mixer, with no IOP streamer emulation.
    bool prepareCourseMenu(const std::filesystem::path& audioRoot, std::string& error) noexcept;
    bool ready() const noexcept override;
    bool riderMenuReady() const noexcept;
    bool courseMenuReady() const noexcept;
    bool canStartCourseLoop11() const noexcept;
    bool startCourseLoop11() noexcept;
    bool canFadeCourseMusic() const noexcept;
    bool fadeCourseMusic() noexcept;
    // Actual Course input1DEF18/1DF0E0/1DF1C8/1DF480: IDs1,5,3,4.
    bool canPlayCourseEffect(std::uint32_t originalId) const noexcept;
    bool playCourseEffect(std::uint32_t originalId) noexcept;
    // Preflight every possible choice BEFORE the real shared RNG draw.
    bool canPlayRiderConfirm() const noexcept;
    bool playRiderConfirmVoice(std::uint32_t soundId, RiderConfirmPlayback&) noexcept;
    bool playRiderConfirmSelection1() noexcept;
    bool releaseRiderConfirmVoice(RiderConfirmPlayback&) noexcept;
    // Concrete source-mode application through THIS transport's actual mixer.
    // No attached owner, stale mixer, or paused transport can certify Accept.
    const OriginalSoundMode50KG* soundModeOwner50KG() const noexcept;
    bool canApplySoundMode18874050KG(std::uint32_t mode) const noexcept;
    bool applySoundMode18874050KG(std::uint32_t mode) noexcept;
    std::uint32_t sampleRate() const noexcept override {
        return current() ? mixer_->sampleRate() : 0;
    }
    // Source1E68CC ->234900(stream,9,1). Session owns +44/+48 admission checks.
    // Replaces only this frontend music voice, loops original PCM [0,frames).
    bool startMainLoop9() noexcept;
    // Rider1EA060 ->234900(stream,7,1). Replaces the SAME music voice and
    // cancels its outgoing fade; no new stream, clock, RNG or music overlap.
    bool startRiderLoop7() noexcept;
    // Source1EA3C0 invokes the same234440(1.0) fade on that shared stream.
    bool fadeRiderMusic() noexcept;
    // Bounded Rider5/1 normal Back only:1EA848 ->292360(event5,1,0).
    // Read-only preflight is required before the actual profile/route commit.
    bool canPlayRiderBack5() const noexcept;
    bool playRiderBack5() noexcept;
    // Dormant Rider direction event2 ->292360(2,1,0), the authenticated
    // ADVANCE.VAG clip6/voice6 operation. No page/session invokes this yet.
    // Receipt certifies one successful command, never current audibility,
    // continued effect activity, movement, a page update or presentation.
    bool canPlayRiderAdvance2() const noexcept;
    bool playRiderAdvance2(RiderAdvanceCommandReceipt&) noexcept;
    bool inspectSteadyRiderTransport(SteadyTransportObservation&) const noexcept;
    // Source loop-mode234540, targetvolume0, duration1000ms for Main B4.
    // Bounded source calls admitted: zero immediate stop or exactly1 second.
    // Native policy: linear gain at accepted source-update boundaries, followed
    // by the source0.35s silent retirement margin. This is not IOP timing emulation.
    // A concretely idle owned transport is a successful no-change command,
    // matching source234480's early return ignored by Main B4. A foreign or
    // stale active voice remains unavailable; true is not the source result.
    bool fadeMain(float seconds = 1.f) noexcept;
    // Authenticated UIEVENTS IDs1(select),3(left),4(right),7(badcall), all one-shot.
    // Source292810 flags1 ->233410 stops an active sound before restart.
    // One voice per original event; different IDs may overlap.
    bool playMenuEffect(std::uint32_t originalId) noexcept;
    // Actual Options menu1F9640 admits events1,3,4,5 only. Main allowlist stays unchanged.
    bool playOptionsEffect(std::uint32_t originalId) noexcept;
    // Page4 source1F50F0 additionally owns event2(defaults) and7(clamp).
    // Read-only concrete preflight for a serialized profile/route transaction.
    bool canPlayGameOptionsEffect(std::uint32_t originalId) const noexcept;
    bool playGameOptionsEffect(std::uint32_t originalId) noexcept;
    bool update(float sourceSeconds) noexcept;
    void pause(bool value) noexcept {
        if(paused_!=value){paused_=value;noteControlChange();}
    }
    bool paused() const noexcept { return paused_; }
    // Application must use this gate for the dedicated frontend mixer. Paused
    // calls write silence without consuming PCM. No device callback uses this.
    bool mixStereo16(std::int16_t* output, std::size_t frames) noexcept override;
    void reset() noexcept;
    bool mainLoopActive() const noexcept;
    bool riderLoopActive() const noexcept;
    std::uint32_t activeMusicSong() const noexcept;
    float mainGain() const noexcept { return musicGain_; }
private:
    bool canRecordControlChange(std::uint64_t count = 1) const noexcept;
    void noteControlChange() noexcept;
    bool current() const noexcept;
    bool startMusicLoop(const AudioClip50HD&, std::uint32_t song) noexcept;
    bool canPlayEffectClip(std::size_t) const noexcept;
    bool playEffectClip(std::size_t) noexcept;
    bool attempted_ = false, prepared_ = false, paused_ = false;
    bool riderAttempted_ = false, riderPrepared_ = false;
    bool courseAttempted_ = false, coursePrepared_ = false;
    bool ownsMusic_ = false, fading_ = false;
    AudioMixer50HD* mixer_ = nullptr;
    std::uint64_t mixerEpoch_ = 0;
    std::size_t mixerCount_ = 0;
    std::uint32_t mixerRate_ = 0;
    const OriginalSoundMode50KG* preparedSoundMode_ = nullptr;
    std::uint64_t controlRevision_ = 0, musicPlaybackRevision_ = 0;
    std::array<std::uint64_t, effectVoiceCount> effectPlaybackRevisions_{};
    bool controlRevisionExhausted_ = false;
    float musicGain_ = 0.4f, fadeStartGain_ = 0.4f;
    float fadeElapsed_ = 0.f, fadeDuration_ = 0.f, fadeRetireAt_ = 0.f;
    std::uint32_t musicSong_ = 0;
    // Original UI setup1B7944 music40/100 and1B78D0 effects100/100.
    std::array<assets::AudioPcm50HC, 7> pcm_{};
    std::array<AudioClip50HD, 7> clips_{};
    assets::AudioPcm50HC riderPcm_{};
    AudioClip50HD riderClip_{};
    assets::AudioPcm50HC coursePcm_{};
    AudioClip50HD courseClip_{};
    std::array<assets::AudioPcm50HC, riderConfirmClipCount> riderConfirmPcm_{};
    std::array<AudioClip50HD, riderConfirmClipCount> riderConfirmClips_{};
    std::uint64_t riderConfirmPlaybackRevision_ = 0;
    bool ownsRiderConfirm_ = false;
    std::array<bool, effectVoiceCount> ownsEffect_{};
};
}

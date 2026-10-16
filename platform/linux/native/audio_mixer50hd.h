#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>

namespace rr::platform {
class OriginalSoundMode50KG;
class RaceAudioGroups50IC;
// Existing0..219 reservations plus the eight distinct original D44 arrays.
// Each admitted track has at most44 collision+drive rows (SA32+12). These
// are native exclusive playback slots, never guest hardware channel IDs.
inline constexpr std::size_t originalMaterialFirstVoice50HD=220;
inline constexpr std::size_t originalMaterialRowsPerRacer50HD=44;
inline constexpr std::size_t originalMaterialRacers50HD=8;
inline constexpr std::size_t originalSpeechFirstVoice50HD=originalMaterialFirstVoice50HD+
    originalMaterialRowsPerRacer50HD*originalMaterialRacers50HD;
// Largest authenticated D48 bank is POD19:24 curses+12 taunts+11 specials.
// All eight racers retain distinct logical voices, including the followed AI.
inline constexpr std::size_t originalSpeechVoicesPerRacer50HD=47;
inline constexpr std::size_t originalSpeechRacers50HD=8;
inline constexpr std::size_t audioMaximumVoices50HD=originalSpeechFirstVoice50HD+
    originalSpeechVoicesPerRacer50HD*originalSpeechRacers50HD;

// Borrowed decoded, signed PCM16. Samples and the voice array must outlive use.
// Mono or interleaved stereo; frame counts are not scalar sample counts.
// Embedded loop end is exclusive. Both zero means loop the entire clip.
struct AudioClip50HD {
    const std::int16_t* samples = nullptr;
    std::uint64_t frames = 0;
    std::uint32_t sampleRate = 0;
    std::uint8_t channels = 0;
    std::uint64_t loopStart = 0, loopEnd = 0;
};
struct AudioVoice50HD {
    AudioClip50HD clip{};
    double sourceFrame = 0.0;
    float gain = 1.0f, pitch = 1.0f, pan = 0.0f;
    bool active = false, loop = false;
};
// Main-thread copied observation only. Playback identity changes on every
// successful play/restart, including an identical clip in the same slot.
// Natural PCM cursor advance and one-shot completion change neither revision.
struct AudioVoiceObservation50HD {
    AudioVoice50HD voice{};
    std::uint64_t playbackRevision = 0;
    float groupGain = 1.0f, originalLeft = 1.0f, originalRight = 1.0f;
    bool groupKnown = false;
    bool groupPaused = false;
};

// Single producer/main-thread mixer. It never allocates, loads assets, opens a
// device, synthesizes a substitute effect, or touches the SDL callback thread.
class AudioMixer50HD final {
public:
    bool initialize(AudioVoice50HD* voices, std::size_t count,
                    std::uint32_t outputRate = 48000) noexcept;
    // Cold bind to the real shared source-mode owner, before any playback.
    // Legacy behavior remains exact when no mode owner is installed. With a
    // bound owner, ordinary play/controls represent actual centered routing;
    // nonzero host pan is rejected rather than inventing a source azimuth.
    bool bindOriginalSoundMode50KG(std::shared_ptr<OriginalSoundMode50KG>) noexcept;
    OriginalSoundMode50KG* originalSoundMode50KG() noexcept {return soundMode50KG_.get();}
    const OriginalSoundMode50KG* originalSoundMode50KG() const noexcept {return soundMode50KG_.get();}
    // Explicit engine-angle controls26CBA0. Source233950 orientation is a
    // separate projection. Unknown native phase selectors reject atomically.
    bool playOriginalAngle50KG(std::size_t slot,const AudioClip50HD&,float gain,
        float pitch,bool loop,std::int32_t engineAngle) noexcept;
    bool controlsOriginalAngle50KG(std::size_t slot,float gain,float pitch,
        std::int32_t engineAngle) noexcept;
    bool play(std::size_t slot, const AudioClip50HD&, float gain = 1.0f,
              float pitch = 1.0f, bool loop = false, float pan = 0.0f) noexcept;
    bool controls(std::size_t slot, float gain, float pitch,
                  float pan = 0.0f) noexcept;
    // Source-owned rates can be below the legacy .125 presentation limit.
    // Positive finite rates are admitted only if double source coordinates
    // keep advancing throughout this actual clip and remain in the precise
    // integer-index domain. Existing clients keep their legacy controls.
    // Centered original-mode routing/group ownership are still enforced.
    bool playSourceRate(std::size_t slot,const AudioClip50HD&,float gain,
                        float pitch,bool loop,std::int32_t engineAngle=0) noexcept;
    bool controlsSourceRate(std::size_t slot,float gain,float pitch,
                            std::int32_t engineAngle=0) noexcept;
    // Actual integer source volume enters26CBA0 BEFORE directional integer
    // division. Requires the real mode owner. The resulting L/R percentages
    // include volume; voice.gain is unity, so volume is never applied twice.
    // Legacy gain APIs above retain their existing normalization policy.
    // Native source-volume rendering admits normalized rate0 as silent paused
    // playback: token/active status/cursor survive until positive controls or
    // stop. This is a native policy, not emulated IOP pitch-zero behavior.
    bool playSourceVolume(std::size_t slot,const AudioClip50HD&,std::int32_t volume,
                          float pitch,bool loop,std::int32_t engineAngle) noexcept;
    bool controlsSourceVolume(std::size_t slot,std::int32_t volume,float pitch,
                              std::int32_t engineAngle) noexcept;
    bool setLoop(std::size_t slot, bool loop) noexcept;
    bool setMasterGain(float gain) noexcept;
    // 50IC persistent group layer. These gains are independent of voice.gain:
    // play/restart, controls and stop do not erase or multiply them into it.
    // The source-owner adapter supplies a normalized integer group volume.
    bool setVoiceGroupGain50IC(std::size_t slot, float gain) noexcept;
    bool hasVoiceGroupGain50IC(std::size_t slot) const noexcept;
    // One-way until initialize. Refuses any currently active unbound voice;
    // subsequent play/controls on unbound slots fail rather than bypass groups.
    bool requireVoiceGroupGains50IC() noexcept;
    bool requiresVoiceGroupGains50IC() const noexcept { return requireGroups50IC_; }
    std::uint64_t groupEpoch50IC() const noexcept { return groupEpoch50IC_; }
    void stop(std::size_t slot) noexcept;
    void stopAll() noexcept;
    bool active(std::size_t slot) const noexcept;
    bool inspectVoice(std::size_t slot, AudioVoiceObservation50HD&) const noexcept;
    float masterGain() const noexcept { return masterGain_; }
    std::uint64_t controlRevision() const noexcept { return controlRevision_; }
    std::uint64_t playbackRevision(std::size_t slot) const noexcept {
        return slot<count_ ? playbackRevisions_[slot] : 0;
    }
    // Overflow permanently invalidates observation identities, never wraps.
    // Existing playback/stop behavior is preserved even after exhaustion;
    // new capability commands preflight the required revision capacity.
    bool canRecordControlChanges(std::uint64_t count = 0) const noexcept;
    // Returns count() if there is no free slot within [first, first+count).
    std::size_t availableVoice(std::size_t first = 0,
                              std::size_t count = audioMaximumVoices50HD) const noexcept;
    // At most one output-rate second per call. No application heap allocation.
    // Returns false for invalid arguments without touching output or voices.
    bool mixStereo16(std::int16_t* output, std::size_t frames) noexcept;
    std::size_t count() const noexcept { return count_; }
    std::uint32_t sampleRate() const noexcept { return rate_; }
    std::uint64_t mixedFrames() const noexcept { return mixedFrames_; }
    std::uint64_t clippedSamples() const noexcept { return clippedSamples_; }
    static bool validClip(const AudioClip50HD&) noexcept;
private:
    friend class RaceAudioGroups50IC;
    // Actual group owner only; independent of gain, pitch and playback token.
    bool setVoiceGroupPaused50IC(std::size_t slot,bool paused) noexcept;
    bool retireStoppedVoiceGroup50IC(std::size_t slot) noexcept;
    void noteControlChange() noexcept;
    bool originalRouting50KG(std::int32_t engineAngle,float& left,float& right,
                            std::int32_t volume=100) const noexcept;
    std::shared_ptr<OriginalSoundMode50KG> soundMode50KG_;
    float originalLeft50KG_[audioMaximumVoices50HD]{};
    float originalRight50KG_[audioMaximumVoices50HD]{};
    AudioVoice50HD* voices_ = nullptr;
    std::size_t count_ = 0;
    std::uint32_t rate_ = 0;
    float masterGain_ = 1.0f;
    std::uint64_t mixedFrames_ = 0, clippedSamples_ = 0;
    float groupGains50IC_[audioMaximumVoices50HD]{};
    bool groupKnown50IC_[audioMaximumVoices50HD]{};
    bool groupPaused50IC_[audioMaximumVoices50HD]{};
    bool requireGroups50IC_ = false;
    std::uint64_t groupEpoch50IC_ = 0;
    std::uint64_t controlRevision_ = 0;
    std::uint64_t playbackRevisions_[audioMaximumVoices50HD]{};
    bool controlRevisionExhausted_ = false;
};

// Integer cumulative sample clock: floor(total microseconds * rate / 1e6).
// 48 kHz produces exactly 768 frames for every native 16 ms simulation step.
// At other rates, fractional frames carry forward instead of rounding per tick.
class AudioSampleClock50HD final {
public:
    bool initialize(std::uint32_t sampleRate = 48000) noexcept;
    bool advance(std::uint32_t microseconds, std::uint32_t& frames) noexcept;
    std::uint64_t microseconds() const noexcept { return microseconds_; }
    std::uint64_t frames() const noexcept { return frames_; }
    std::uint32_t sampleRate() const noexcept { return rate_; }
private:
    std::uint32_t rate_ = 0, remainder_ = 0;
    std::uint64_t microseconds_ = 0, frames_ = 0;
};
}

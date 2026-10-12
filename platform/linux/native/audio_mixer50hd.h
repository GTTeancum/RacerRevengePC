#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>

namespace rr::platform {
class OriginalSoundMode50KG;
inline constexpr std::size_t audioMaximumVoices50HD = 160;

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
    void noteControlChange() noexcept;
    bool originalRouting50KG(std::int32_t engineAngle,float& left,float& right) const noexcept;
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

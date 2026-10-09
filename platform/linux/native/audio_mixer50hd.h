#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::platform {
inline constexpr std::size_t audioMaximumVoices50HD = 128;

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

// Single producer/main-thread mixer. It never allocates, loads assets, opens a
// device, synthesizes a substitute effect, or touches the SDL callback thread.
class AudioMixer50HD final {
public:
    bool initialize(AudioVoice50HD* voices, std::size_t count,
                    std::uint32_t outputRate = 48000) noexcept;
    bool play(std::size_t slot, const AudioClip50HD&, float gain = 1.0f,
              float pitch = 1.0f, bool loop = false, float pan = 0.0f) noexcept;
    bool controls(std::size_t slot, float gain, float pitch,
                  float pan = 0.0f) noexcept;
    bool setLoop(std::size_t slot, bool loop) noexcept;
    bool setMasterGain(float gain) noexcept;
    void stop(std::size_t slot) noexcept;
    void stopAll() noexcept;
    bool active(std::size_t slot) const noexcept;
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
    AudioVoice50HD* voices_ = nullptr;
    std::size_t count_ = 0;
    std::uint32_t rate_ = 0;
    float masterGain_ = 1.0f;
    std::uint64_t mixedFrames_ = 0, clippedSamples_ = 0;
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

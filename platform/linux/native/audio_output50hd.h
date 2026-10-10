#pragma once
#include "audio_mixer50hd.h"
#include <array>
#include <cstdint>

namespace rr::platform {
enum class AudioDeviceStatus50HD : std::uint8_t {
    NotRequested, Ready, Unavailable, RejectedNonPlayback
};
const char* audioDeviceStatusName50HD(AudioDeviceStatus50HD) noexcept;
enum class AudioOutputError50HD : std::uint8_t {
    None, InvalidRequest, AlreadyOpen, NotOpen, Allocation,
    CaptureOpen, CaptureWrite, CaptureFinalize, CaptureLimit, Mixer, Clock
};
struct AudioOutputDiagnostic50HD {
    AudioOutputError50HD error = AudioOutputError50HD::None;
    const char* operation = nullptr;
    std::array<char, 256> detail{};
    // A device queue overflow does not discard the independent WAV recording.
    std::uint64_t droppedDeviceFrames = 0;
};
struct AudioOutputRequest50HD {
    bool tryDevice = true;
    // Null disables capture. A nonempty path creates a NEW file, never replaces
    // an existing recording. Parent directories must already exist.
    const char* wavPath = nullptr;
    std::uint32_t sampleRate = 48000;
    std::uint16_t callbackFrames = 1024;
    // Wait for this much mixed audio before starting the real device callback.
    std::uint16_t prebufferFrames = 1536;
};
struct AudioOutputInfo50HD {
    AudioDeviceStatus50HD deviceStatus = AudioDeviceStatus50HD::NotRequested;
    std::array<char, 64> driver{};
    std::array<char, 256> deviceDetail{};
    std::uint32_t sampleRate = 0;
    bool captureActive = false, paused = false;
    std::uint64_t simulationMicroseconds = 0, mixedFrames = 0, capturedFrames = 0;
    std::uint64_t deviceQueuedFrames = 0, deviceConsumedFrames = 0;
    std::uint64_t deviceUnderrunFrames = 0, deviceOverflowFrames = 0;
};

// Cold-allocated fixed storage, then a main-thread producer and bounded SDL2
// callback. No application allocations, file I/O, mixing, or locking occur in
// the callback. SDL/driver internals and file-system I/O are not hard-real-time.
// Create AFTER WindowHost50DQ::create and close BEFORE that host's SDL_Quit.
// All public methods and mixer mutations belong to one caller thread.
class AudioOutput50HD final {
public:
    AudioOutput50HD() noexcept = default;
    ~AudioOutput50HD() noexcept;
    AudioOutput50HD(const AudioOutput50HD&) = delete;
    AudioOutput50HD& operator=(const AudioOutput50HD&) = delete;
    // Device failure is graceful: open succeeds with deviceStatus=Unavailable,
    // preserving capture/mixing. Check info() before claiming audible output.
    bool open(const AudioOutputRequest50HD&, AudioOutputDiagnostic50HD&) noexcept;
    // Call exactly once for each successfully committed simulation interval,
    // including quiet intervals. Do not call once per rendered video frame.
    // One call <= 1 second; uses an integer sample clock, never wall-clock sleep.
    // Every WAV payload byte is copied from the SAME PCM buffer queued to SDL.
    // Device starvation/overflow are counted separately and never faked away.
    bool advanceMicroseconds(AudioMixer50HD&, std::uint32_t microseconds,
                             AudioOutputDiagnostic50HD&) noexcept;
    // Main-thread pause flushes stale device data, not voices or captured PCM.
    // Stop simulation advancement while paused to keep the capture clock true.
    void pauseDevice(bool paused) noexcept;
    AudioOutputInfo50HD info() const noexcept;
    // Flush/finalize WAV and close device. Does not wait for queued audio tails.
    // Stats remain readable after close. Calling close again is harmless.
    bool close(AudioOutputDiagnostic50HD&) noexcept;
private:
    struct State;
    State* state_ = nullptr;
    AudioOutputInfo50HD finalInfo_{};
};
}

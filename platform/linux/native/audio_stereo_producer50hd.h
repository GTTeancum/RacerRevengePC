#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::platform {
// Typed, synchronous main-thread PCM producer. AudioOutput never retains this
// reference and the device callback never invokes it. Implementations retain
// their own policy gates (including frontend pause) around the actual mixer.
// Rate is signed PCM16 interleaved stereo frames/second. A failed mix is fatal
// to the current output; no replacement or guessed silence is inserted.
class AudioStereoProducer50HD {
public:
    virtual ~AudioStereoProducer50HD() = default;
    virtual bool ready() const noexcept = 0;
    virtual std::uint32_t sampleRate() const noexcept = 0;
    virtual bool mixStereo16(std::int16_t*, std::size_t frames) noexcept = 0;
};
}

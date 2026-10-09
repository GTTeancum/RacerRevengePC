#include "audio_mixer50hd.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace rr::platform {
namespace {
bool validRate(std::uint32_t rate) noexcept { return rate >= 8000 && rate <= 192000; }
bool validControls(float gain, float pitch, float pan) noexcept {
    return std::isfinite(gain) && gain >= 0.0f && gain <= 8.0f &&
           std::isfinite(pitch) && pitch >= 0.125f && pitch <= 8.0f &&
           std::isfinite(pan) && pan >= -1.0f && pan <= 1.0f;
}
std::int16_t quantize(double value, std::uint64_t& clipped) noexcept {
    if (value > 32767.0) { ++clipped; return 32767; }
    if (value < -32768.0) { ++clipped; return -32768; }
    return static_cast<std::int16_t>(std::lround(value));
}
}
bool AudioMixer50HD::validClip(const AudioClip50HD& clip) noexcept {
    // Double source coordinates retain sub-frame precision across practical
    // decoded clips. This also bounds all pointer-index multiplications.
    return clip.samples && clip.frames && clip.frames <= (std::uint64_t{1} << 40) &&
           validRate(clip.sampleRate) && (clip.channels == 1 || clip.channels == 2) &&
           clip.frames <= std::numeric_limits<std::size_t>::max() / clip.channels &&
           ((!clip.loopStart && !clip.loopEnd) ||
            (clip.loopStart < clip.loopEnd && clip.loopEnd <= clip.frames));
}
bool AudioMixer50HD::initialize(AudioVoice50HD* voices, std::size_t count,
                              std::uint32_t outputRate) noexcept {
    if (!voices || !count || count > audioMaximumVoices50HD || !validRate(outputRate))
        return false;
    voices_ = voices; count_ = count; rate_ = outputRate;
    masterGain_ = 1.0f; mixedFrames_ = clippedSamples_ = 0;
    for (std::size_t i = 0; i < count_; ++i) voices_[i] = {};
    return true;
}
bool AudioMixer50HD::play(std::size_t slot, const AudioClip50HD& clip, float gain,
                        float pitch, bool loop, float pan) noexcept {
    if (slot >= count_ || !validClip(clip) || !validControls(gain, pitch, pan)) return false;
    voices_[slot] = {clip, 0.0, gain, pitch, pan, true, loop};
    return true;
}
bool AudioMixer50HD::controls(std::size_t slot, float gain, float pitch, float pan) noexcept {
    if (slot >= count_ || !validControls(gain, pitch, pan)) return false;
    auto& voice = voices_[slot];
    voice.gain = gain; voice.pitch = pitch; voice.pan = pan;
    return true;
}
bool AudioMixer50HD::setLoop(std::size_t slot, bool loop) noexcept {
    if (slot >= count_) return false;
    voices_[slot].loop = loop;
    return true;
}
bool AudioMixer50HD::setMasterGain(float gain) noexcept {
    if (!std::isfinite(gain) || gain < 0.0f || gain > 4.0f) return false;
    masterGain_ = gain; return true;
}
void AudioMixer50HD::stop(std::size_t slot) noexcept {
    if (slot < count_) voices_[slot].active = false;
}
void AudioMixer50HD::stopAll() noexcept {
    for (std::size_t i = 0; i < count_; ++i) voices_[i].active = false;
}
bool AudioMixer50HD::active(std::size_t slot) const noexcept {
    return slot < count_ && voices_[slot].active;
}
std::size_t AudioMixer50HD::availableVoice(std::size_t first, std::size_t count) const noexcept {
    if (first >= count_) return count_;
    const auto end = first + std::min(count, count_ - first);
    for (auto i = first; i < end; ++i) if (!voices_[i].active) return i;
    return count_;
}
bool AudioMixer50HD::mixStereo16(std::int16_t* output, std::size_t frames) noexcept {
    if (!voices_ || !rate_ || frames > rate_ || (!output && frames) ||
        frames > std::numeric_limits<std::uint64_t>::max() - mixedFrames_) return false;
    for (std::size_t f = 0; f < frames; ++f) {
        double left = 0.0, right = 0.0;
        for (std::size_t i = 0; i < count_; ++i) {
            auto& voice = voices_[i];
            if (!voice.active) continue;
            const auto& clip = voice.clip;
            const std::uint64_t loopStart = clip.loopEnd ? clip.loopStart : 0;
            const std::uint64_t loopEnd = clip.loopEnd ? clip.loopEnd : clip.frames;
            // Handles enabling looping after the initial pass went beyond its
            // embedded end. Stop never rewinds or restarts another voice.
            if (voice.loop && voice.sourceFrame >= static_cast<double>(loopEnd))
                voice.sourceFrame = static_cast<double>(loopStart) +
                    std::fmod(voice.sourceFrame - static_cast<double>(loopStart),
                              static_cast<double>(loopEnd - loopStart));
            if (voice.sourceFrame >= static_cast<double>(clip.frames)) {
                voice.active = false; continue;
            }
            const auto first = static_cast<std::uint64_t>(voice.sourceFrame);
            auto second = first + 1;
            if (voice.loop && second >= loopEnd) second = loopStart;
            else if (second >= clip.frames) second = first;
            const double fraction = voice.sourceFrame - static_cast<double>(first);
            const auto a = static_cast<std::size_t>(first) * clip.channels;
            const auto b = static_cast<std::size_t>(second) * clip.channels;
            const double sampleLeft = clip.samples[a] +
                (static_cast<double>(clip.samples[b]) - clip.samples[a]) * fraction;
            const auto rightChannel = static_cast<std::size_t>(clip.channels - 1);
            const double sampleRight = clip.samples[a + rightChannel] +
                (static_cast<double>(clip.samples[b + rightChannel]) - clip.samples[a + rightChannel]) * fraction;
            left += sampleLeft * voice.gain * (1.0f - std::max(voice.pan, 0.0f));
            right += sampleRight * voice.gain * (1.0f + std::min(voice.pan, 0.0f));
            voice.sourceFrame += static_cast<double>(clip.sampleRate) / rate_ * voice.pitch;
            if (voice.loop && voice.sourceFrame >= static_cast<double>(loopEnd))
                voice.sourceFrame = static_cast<double>(loopStart) +
                    std::fmod(voice.sourceFrame - static_cast<double>(loopStart),
                              static_cast<double>(loopEnd - loopStart));
            else if (!voice.loop && voice.sourceFrame >= static_cast<double>(clip.frames))
                voice.active = false;
        }
        output[2 * f] = quantize(left * masterGain_, clippedSamples_);
        output[2 * f + 1] = quantize(right * masterGain_, clippedSamples_);
    }
    mixedFrames_ += frames;
    return true;
}
bool AudioSampleClock50HD::initialize(std::uint32_t sampleRate) noexcept {
    if (!validRate(sampleRate)) return false;
    rate_ = sampleRate; remainder_ = 0; microseconds_ = frames_ = 0; return true;
}
bool AudioSampleClock50HD::advance(std::uint32_t microseconds, std::uint32_t& frames) noexcept {
    frames = 0;
    if (!rate_ || microseconds > 1000000 ||
        microseconds > std::numeric_limits<std::uint64_t>::max() - microseconds_) return false;
    const auto scaled = static_cast<std::uint64_t>(microseconds) * rate_ + remainder_;
    const auto count = static_cast<std::uint32_t>(scaled / 1000000);
    if (count > std::numeric_limits<std::uint64_t>::max() - frames_) return false;
    remainder_ = static_cast<std::uint32_t>(scaled % 1000000);
    microseconds_ += microseconds; frames_ += count; frames = count; return true;
}
}

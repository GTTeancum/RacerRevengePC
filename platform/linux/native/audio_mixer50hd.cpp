#include "audio_mixer50hd.h"
#include "original_sound_mode50kg.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

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
    if (!voices || !count || count > audioMaximumVoices50HD || !validRate(outputRate) ||
        groupEpoch50IC_ == std::numeric_limits<std::uint64_t>::max())
        return false;
    voices_ = voices; count_ = count; rate_ = outputRate;
    soundMode50KG_.reset();
    masterGain_ = 1.0f; mixedFrames_ = clippedSamples_ = 0;
    ++groupEpoch50IC_; requireGroups50IC_ = false;
    for (std::size_t i = 0; i < count_; ++i) {
        voices_[i] = {}; groupGains50IC_[i] = 1.0f; groupKnown50IC_[i] = false;
        originalLeft50KG_[i] = originalRight50KG_[i] = 1.0f;
    }
    return true;
}
bool AudioMixer50HD::bindOriginalSoundMode50KG(std::shared_ptr<OriginalSoundMode50KG> owner) noexcept {
    if(!owner||!owner->ready()||soundMode50KG_||!voices_||!count_||mixedFrames_)return false;
    for(std::size_t i=0;i<count_;++i)if(voices_[i].active)return false;
    soundMode50KG_=std::move(owner);return true;
}
bool AudioMixer50HD::originalRouting50KG(std::int32_t angle,float& left,float& right) const noexcept {
    if(!soundMode50KG_||!soundMode50KG_->ready())return false;
    game::OriginalSoundRouting50KG routing;
    if(!game::projectOriginalSoundRouting26CBA050KG(soundMode50KG_->fields(),angle,100,routing))return false;
    // Original source proves packet selectors0/1/L/R, but the retained inputs
    // do not include their IOP consumer. Only selector0 is admitted to PCM.
    // In particular no rear-phase interpretation or fake surround is invented.
    if(routing.phaseSelector!=0)return false;
    left=static_cast<float>(routing.leftPercent)/100.f;
    right=static_cast<float>(routing.rightPercent)/100.f;
    return true;
}
bool AudioMixer50HD::playOriginalAngle50KG(std::size_t slot,const AudioClip50HD& clip,
    float gain,float pitch,bool loop,std::int32_t engineAngle) noexcept {
    float left=0.f,right=0.f;
    if(slot>=count_||!validClip(clip)||!validControls(gain,pitch,0.f)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right))return false;
    voices_[slot]={clip,0.0,gain,pitch,0.f,true,loop};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::controlsOriginalAngle50KG(std::size_t slot,float gain,float pitch,
    std::int32_t engineAngle) noexcept {
    float left=0.f,right=0.f;
    if(slot>=count_||!validControls(gain,pitch,0.f)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right))return false;
    auto& voice=voices_[slot];voice.gain=gain;voice.pitch=pitch;voice.pan=0.f;
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::play(std::size_t slot, const AudioClip50HD& clip, float gain,
                        float pitch, bool loop, float pan) noexcept {
    if(soundMode50KG_)return pan==0.f&&playOriginalAngle50KG(slot,clip,gain,pitch,loop,0);
    if (slot >= count_ || !validClip(clip) || !validControls(gain, pitch, pan) ||
        (requireGroups50IC_ && !groupKnown50IC_[slot])) return false;
    voices_[slot] = {clip, 0.0, gain, pitch, pan, true, loop};
    return true;
}
bool AudioMixer50HD::controls(std::size_t slot, float gain, float pitch, float pan) noexcept {
    if(soundMode50KG_)return pan==0.f&&controlsOriginalAngle50KG(slot,gain,pitch,0);
    if (slot >= count_ || !validControls(gain, pitch, pan) ||
        (requireGroups50IC_ && !groupKnown50IC_[slot])) return false;
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
bool AudioMixer50HD::setVoiceGroupGain50IC(std::size_t slot, float gain) noexcept {
    if (slot >= count_ || !std::isfinite(gain) || gain < 0.0f || gain > 1.0f) return false;
    groupGains50IC_[slot] = gain; groupKnown50IC_[slot] = true; return true;
}
bool AudioMixer50HD::hasVoiceGroupGain50IC(std::size_t slot) const noexcept {
    return slot < count_ && groupKnown50IC_[slot];
}
bool AudioMixer50HD::requireVoiceGroupGains50IC() noexcept {
    if (!voices_ || !count_) return false;
    for (std::size_t i = 0; i < count_; ++i)
        if (voices_[i].active && !groupKnown50IC_[i]) return false;
    requireGroups50IC_ = true; return true;
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
            // Exactly one retained group multiplier; Controls changes only
            // voice.gain. Unbound legacy voices retain unity until strict mode.
            // Mode is consumed at source play/controls, not by retroactively
            // remapping active voices when188740 changes the manager fields.
            const float panLeft=soundMode50KG_?originalLeft50KG_[i]:(1.0f-std::max(voice.pan,0.0f));
            const float panRight=soundMode50KG_?originalRight50KG_[i]:(1.0f+std::min(voice.pan,0.0f));
            left += sampleLeft * voice.gain * groupGains50IC_[i] * panLeft;
            right += sampleRight * voice.gain * groupGains50IC_[i] * panRight;
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

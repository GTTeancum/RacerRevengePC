#pragma once
#include "../../platform/linux/native/original_ui_main_audio50kg.h"
#include "../../platform/linux/native/audio_output50hd.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include <array>
#include <limits>
#include <memory>
#include <string>
#include <utility>

namespace original_main_entry50KG {
// Dedicated frontend device/WAV ownership. No reference to a race mixer, live
// owner or race voice is accepted. Close this owner BEFORE any future race
// output opens. Main currently has no admitted gameplay destination.
class AudioOutput final {
public:
    AudioOutput() = default;
    AudioOutput(const AudioOutput&)=delete;
    AudioOutput& operator=(const AudioOutput&)=delete;
    AudioOutput(AudioOutput&&)=delete;
    AudioOutput& operator=(AudioOutput&&)=delete;
    ~AudioOutput() noexcept {
        rr::platform::AudioOutputDiagnostic50HD diagnostic;close(diagnostic);
    }
    bool prepare(const std::filesystem::path& root,
        const rr::platform::AudioOutputRequest50HD& request,std::string& error,
        rr::platform::AudioOutputDiagnostic50HD& diagnostic,
        std::shared_ptr<rr::platform::OriginalSoundMode50KG> soundMode={}) {
        if(attempted_ || closed_){error="Main output requires a fresh stable owner";return false;}
        attempted_=true;
        if(!mixer_.initialize(voices_.data(),voices_.size(),request.sampleRate)){
            error="Main dedicated mixer initialization failed";return false;
        }
        if(soundMode&&!mixer_.bindOriginalSoundMode50KG(std::move(soundMode))){
            error="Main source sound-mode consumer binding failed";return false;
        }
        transport_=std::make_shared<rr::platform::OriginalUiMainAudio50KG>();
        if(!transport_->prepare(root,mixer_,error))return false;
        if(!output_.open(request,diagnostic)){
            error=std::string("Main output open failed: ")+diagnostic.detail.data();return false;
        }
        enabled_=true;error.clear();return true;
    }
    std::shared_ptr<rr::platform::OriginalUiMainAudio50KG> transport() const noexcept {
        return transport_;
    }
    bool enabled() const noexcept { return enabled_; }
    // Checks this dedicated output's exact prepared transport and mixer. A
    // muted/closed output has no capability; mode storage alone is insufficient.
    const rr::platform::OriginalSoundMode50KG* soundModeOwner50KG() const noexcept {
        return enabled_&&!closed_&&transport_?transport_->soundModeOwner50KG():nullptr;
    }
    bool canApplySoundMode18874050KG(std::uint32_t mode) const noexcept {
        return enabled_&&!closed_&&transport_&&transport_->canApplySoundMode18874050KG(mode);
    }
    bool applySoundMode18874050KG(std::uint32_t mode) noexcept {
        return canApplySoundMode18874050KG(mode)&&transport_->applySoundMode18874050KG(mode);
    }
    void pause(bool value) noexcept {
        if(!enabled_ || closed_)return;
        transport_->pause(value);
        output_.pauseDevice(value); // Flush stale device PCM; retain source cursor/WAV.
    }
    // The SAME elapsed nanosecond interval supplied to the accepted source
    // update. Carry fractional microseconds rather than round every frame.
    // Paused wall time is excluded by the caller; no render-rate resampling.
    bool advanceAcceptedNanoseconds(std::uint64_t nanoseconds,
        rr::platform::AudioOutputDiagnostic50HD& diagnostic) noexcept {
        diagnostic={};
        if(!enabled_)return true; // Explicit mute owns no output or PCM transport.
        if(closed_ || !transport_ || transport_->paused() || nanoseconds>1000000000ull ||
            nanoseconds>std::numeric_limits<std::uint64_t>::max()-acceptedNanoseconds_){
            diagnostic.error=rr::platform::AudioOutputError50HD::Clock;
            diagnostic.operation="Main output accepted interval";return false;
        }
        const auto carried=nanoseconds+remainderNanoseconds_;
        if(!output_.advanceMicroseconds(*transport_,static_cast<std::uint32_t>(carried/1000),diagnostic))
            return false;
        remainderNanoseconds_=static_cast<std::uint32_t>(carried%1000);
        acceptedNanoseconds_+=nanoseconds;
        return true;
    }
    rr::platform::AudioOutputInfo50HD info() const noexcept { return output_.info(); }
    std::uint64_t sourceMixedFrames() const noexcept { return mixer_.mixedFrames(); }
    std::uint64_t acceptedNanoseconds() const noexcept { return acceptedNanoseconds_; }
    std::uint32_t remainderNanoseconds() const noexcept { return remainderNanoseconds_; }
    bool close(rr::platform::AudioOutputDiagnostic50HD& diagnostic) noexcept {
        const bool result=output_.close(diagnostic); // Joins callback/finalizes WAV first.
        if(transport_)transport_->reset(); // Stops only our own frontend voices.
        closed_=true;return result;
    }
private:
    // Reverse destruction: output, transport/PCM, mixer, borrowed voice array.
    std::array<rr::platform::AudioVoice50HD,
        rr::platform::OriginalUiMainAudio50KG::requiredVoices> voices_{};
    rr::platform::AudioMixer50HD mixer_;
    std::shared_ptr<rr::platform::OriginalUiMainAudio50KG> transport_;
    rr::platform::AudioOutput50HD output_;
    bool attempted_=false,enabled_=false,closed_=false;
    std::uint64_t acceptedNanoseconds_=0;
    std::uint32_t remainderNanoseconds_=0;
};
}

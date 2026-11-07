#pragma once
#include "../../platform/linux/native/logical_audio_owner.h"
#include "original_sound_mode50kg.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::game {
enum class AiLogicalSoundStatus : std::uint8_t { complete,input,nativeBackend };
struct AiLogicalSoundReceipt {
    AiLogicalSoundStatus status=AiLogicalSoundStatus::input;
    bool sourceReturned=false;
    std::uint32_t sourcePC=0;
};
// Native sample registration replaces source-positive hardware sample numbers.
// PCM absence gives the genuine233410 false result; it never creates a voice.
// Native token/slot/revision ownership replaces26B5E0's hardware transport.
// Source flags2 changes immediate/queued hardware submission only; native
// rendering already consumes the accepted command at its one mix boundary.
inline AiLogicalSoundReceipt startAiLogicalSound233410(
    platform::LogicalAudioOwner& owner,platform::LogicalAudioOwner::Sound& s,
    std::uint32_t flags) noexcept {
    AiLogicalSoundReceipt r;
    if(flags&~7u)return r;
    r.status=AiLogicalSoundStatus::complete;
    if(!s.available){r.sourcePC=0x233450u;return r;}
    const bool wasActive=owner.active(s);
    if(wasActive&&!(flags&1u)){
        // Source can allocate another channel here. This fixed-range native
        // sound owner cannot represent that extra simultaneous voice yet.
        r.status=AiLogicalSoundStatus::nativeBackend;r.sourcePC=0x233544u;return r;
    }
    if(wasActive&&owner.stop233670(s)!=platform::LogicalAudioOwner::Result::complete){
        r.status=AiLogicalSoundStatus::nativeBackend;r.sourcePC=0x233484u;return r;
    }
    const bool effective=(flags&4u)!=0;
    const auto volume=effective?s.volume44:s.volume38;
    const auto pitch=effective?s.pitch4C:s.pitch40;
    const auto angle=effective?s.angle48:s.angle3C;
    if(volume<0||volume>100||pitch<=0||pitch>=16384){
        r.status=AiLogicalSoundStatus::nativeBackend;r.sourcePC=0x233514u;return r;
    }
    const auto applied=owner.startSourceVolume(s,volume,
        static_cast<float>(pitch)/4096.f,s.loop1B0!=0,angle);
    if(applied!=platform::LogicalAudioOwner::Result::complete){
        r.status=AiLogicalSoundStatus::nativeBackend;r.sourcePC=0x233544u;return r;
    }
    if(!effective){s.pitch4C=s.pitch40;s.volume44=s.volume38;s.angle48=s.angle3C;}
    s.elapsedBits24=0;s.word7C=1;r.sourceReturned=true;r.sourcePC=0x233584u;return r;
}

//233AE0's actual scalar/angle/relative-vector operations on the SAME registered
// sound fields. Inhibition is the caller's actual source sound/group pause
// state. The recovered code computes a Doppler threshold but unconditionally
// writes58=1, so no speculative velocity pitch adjustment is introduced.
inline AiLogicalSoundReceipt spatialAiLogicalSound233AE0(
    platform::LogicalAudioOwner& owner,platform::LogicalAudioOwner::Sound& s,
    const OriginalSoundModeFields50KG& mode,float attenuation,
    std::int32_t sourceAngle,const float (&listener)[4],bool inhibited) noexcept {
    AiLogicalSoundReceipt r;
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(attenuation)||
       !validOriginalSoundModeFields50KG(mode)||s.volume38<0||s.volume38>100)return r;
    float relative[4]={0.f,0.f,0.f,1.f};
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(listener[i])||!std::isfinite(s.position60[i]))return r;
        volatile float delta=listener[i]-s.position60[i];
        if(!std::isfinite(delta))return r;relative[i]=delta;
    }
    r.status=AiLogicalSoundStatus::complete;
    if(inhibited){r.sourcePC=0x233b70u;return r;}
    const float clamped=detail::scalarMin48O(detail::scalarMax48O(0.f,attenuation),100.f);
    volatile float scaled=static_cast<float>(s.volume38)*clamped;
    // Existing recovered EE CVT.W.S contract truncates toward zero.
    s.volume44=static_cast<std::int32_t>(scaled)/100;
    // Preserve the write before source's angle validity branch.
    const auto oriented=mode.reverseAngle9918?std::int64_t(sourceAngle):-std::int64_t(sourceAngle);
    const auto adjusted=oriented<0?oriented+359:oriented;
    if(adjusted<std::numeric_limits<std::int32_t>::min()||
       adjusted>std::numeric_limits<std::int32_t>::max()){
        r.status=AiLogicalSoundStatus::input;return r;
    }
    s.angle48=static_cast<std::int32_t>(adjusted);
    if(sourceAngle< -360||sourceAngle>360){r.sourcePC=0x233ca4u;return r;}
    s.multiplier58=1.f;
    for(unsigned i=0;i<4;++i)s.relative90[i]=relative[i];
    volatile float base=static_cast<float>(s.pitch4C)*s.multiplier58;
    volatile float pitch=s.multiplier54*base;
    if(!std::isfinite(pitch)||pitch< -2147483648.f||pitch>=2147483648.f){
        r.status=AiLogicalSoundStatus::input;r.sourcePC=0x233d18u;return r;
    }
    const auto word=static_cast<std::int32_t>(pitch);
    if(word< -1||word>=16384){r.sourcePC=0x233d40u;return r;}
    if(s.channel2C<0){r.sourceReturned=true;r.sourcePC=0x233d50u;return r;}
    // A one-shot that played to its end keeps its logical token until the
    // next stop/start; its voice is no longer playing. Treat it as the idle
    // channel above (as stop233670 does) instead of controlling a dead voice.
    if(!owner.active(s)){s.channel2C=-1;r.sourceReturned=true;r.sourcePC=0x233d50u;return r;}
    // Positive-rate native backend. Zero/-1 source transport semantics need
    // their own backend handling; never substitute a clamped audible pitch.
    if(word<=0||owner.controlsSourceVolume(s,s.volume44,
        static_cast<float>(word)/4096.f,s.angle48)!=platform::LogicalAudioOwner::Result::complete){
        r.status=AiLogicalSoundStatus::nativeBackend;r.sourcePC=0x233d78u;return r;
    }
    r.sourceReturned=true;r.sourcePC=0x233d78u;return r;
}
}

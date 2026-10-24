#pragma once
// PRIVATE, UNACTIVATED SOURCE-ONLY PROPOSAL. No call site/build/runtime proof.
#include "../../../src/game/scalar_minmax48o.h"
#include <cstring>
#include <limits>
namespace rr::platform {
inline bool RaceAudioGroups50IC::coldTaIntroSettersCurrent(unsigned eventId)const noexcept {
    if(eventId<1||eventId>3||!coldTaIntroGroupIdentity()||coldRetiring_||nativeBindingsRetired_||
       !groups_.initialized||!mixer_||coldMixer_!=mixer_||coldEpoch_!=epoch_||
       mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=count_||
       !mixer_->requiresVoiceGroupGains50IC())return false;
    const auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(!p.taIntroConstructed||p.count48!=3||!p.taIntroManager||!p.taIntroManager->current())return false;
    // Preserve the genuinely never-started, unbound native phase. This is not
    // the ordinary current() gate, which intentionally remains closed here.
    for(std::size_t n=0;n<count_;++n)
        if(mixer_->active(n)||mixer_->hasVoiceGroupGain50IC(n)||
           bindings_[n]!=game::RaceAudioGroup50IC::Unrepresented)return false;
    for(std::size_t n=0;n<p.count48;++n)if(!coldTaIntroSoundIdentity(n))return false;
    return true;
}
inline bool RaceAudioGroups50IC::coldTaIntroVolumeWord233750(
    float input,std::int32_t& word)noexcept {
    static_assert(sizeof(float)==sizeof(std::uint32_t)&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::radix==2&&std::numeric_limits<float>::digits==24&&
        std::numeric_limits<float>::max_exponent==128&&std::numeric_limits<float>::min_exponent==-125,
        "Cold TA setters require the existing finite IEEE binary32 convention");
    std::uint32_t bits=0;std::memcpy(&bits,&input,sizeof(bits));
    // Native bounded admission, NOT an invented retail rejection. Exceptional
    // EE NaN/infinity behavior is outside this owner; refusal changes no Sound.
    if((bits&0x7f800000u)==0x7f800000u)return false;
    // Same finite clamp result as233750/233830, using existing scalar ordering
    // (including signed zeros), not std::min/max's host tie behavior. The
    // subsequent CVT.W.S follows the existing truncation-toward-zero convention.
    // No product, accumulation, float re-rounding or source time is involved.
    const float clamped=game::detail::scalarMin48O(game::detail::scalarMax48O(0.f,input),100.f);
    word=static_cast<std::int32_t>(clamped); // Bounded0..100, never overflow.
    return true;
}
inline RaceAudioGroups50IC::ColdTaSoundSetResult
RaceAudioGroups50IC::setColdTaIntroBaseVolume233830(unsigned eventId,float volume)noexcept {
    if(!coldTaIntroSettersCurrent(eventId))return ColdTaSoundSetResult::ownerRejected;
    std::int32_t word=0;
    if(!coldTaIntroVolumeWord233750(volume,word))return ColdTaSoundSetResult::numberRejected;
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    p.logicalMembers4C[eventId-1]->volume38=word; //2338B0, ONLY base38.
    return ColdTaSoundSetResult::sourceTrue;
}
inline RaceAudioGroups50IC::ColdTaSoundSetResult
RaceAudioGroups50IC::setColdTaIntroEffectiveVolume233750(unsigned eventId,float volume)noexcept {
    if(!coldTaIntroSettersCurrent(eventId))return ColdTaSoundSetResult::ownerRejected;
    std::int32_t word=0;
    if(!coldTaIntroVolumeWord233750(volume,word))return ColdTaSoundSetResult::numberRejected;
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    auto& sound=*p.logicalMembers4C[eventId-1];
    sound.volume44=word; //2337D0 occurs BEFORE the activity query.
    // Sound vtable3D1BB0+24 is233710, not lazy233E90. The authenticated
    // channel==-1 branch returns0 immediately, so2337E4 skips26C8F0. It
    // does not write channel/sample/metadata, multiply group gain or change38.
    return ColdTaSoundSetResult::sourceTrue; //233818.
}
inline RaceAudioGroups50IC::ColdTaSoundSetResult
RaceAudioGroups50IC::setColdTaIntroBasePitch233A50(unsigned eventId,std::int32_t pitch)noexcept {
    if(!coldTaIntroSettersCurrent(eventId))return ColdTaSoundSetResult::ownerRejected;
    // Source SLTIU rejects negative signed arguments as well as16384 and above.
    if(static_cast<std::uint32_t>(pitch)>=16384u)return ColdTaSoundSetResult::sourceFalse;
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    p.logicalMembers4C[eventId-1]->pitch40=pitch; //233A68. No50/4C/pause write.
    return ColdTaSoundSetResult::sourceTrue;
}
inline RaceAudioGroups50IC::ColdTaSoundSetResult
RaceAudioGroups50IC::setColdTaIntroEffectivePitch2339A0(unsigned eventId,std::int32_t pitch)noexcept {
    if(!coldTaIntroSettersCurrent(eventId))return ColdTaSoundSetResult::ownerRejected;
    if(static_cast<std::uint32_t>(pitch)>=16384u)return ColdTaSoundSetResult::sourceFalse; //2339B4.
    const auto group=static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120);
    auto& sound=*coldGroups_[group].logicalMembers4C[eventId-1];
    // Cold identity keeps80==0; retain the source guard/order without
    // claiming admission of inhibited80 states or a setter for that word.
    if(sound.word80)return ColdTaSoundSetResult::sourceFalse; //2339CC, before group read.
    if(groups_.state44[group])return ColdTaSoundSetResult::sourceFalse; //Actual canonical+44.
    //2339F8 virtual+24 ->233710 sees channel==-1 and returns0.233A00
    //takes its no-backend branch; its delay slot sets return1, then233A30
    //writes4C. Do not infer failure from inactivity or update40/50 with4C.
    sound.pitch4C=pitch;
    return ColdTaSoundSetResult::sourceTrue;
}
} // namespace rr::platform

#include "audio_mixer50hd.h"
#include "native_source_reverb50kg.h"
#include "original_sound_mode50kg.h"
#include "original_ui_main_audio50kg.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <exception>

namespace rr::platform {
namespace {
bool validRate(std::uint32_t rate) noexcept { return rate >= 8000 && rate <= 192000; }
bool validControls(float gain, float pitch, float pan) noexcept {
    return std::isfinite(gain) && gain >= 0.0f && gain <= 8.0f &&
           std::isfinite(pitch) && pitch >= 0.125f && pitch <= 8.0f &&
           std::isfinite(pan) && pan >= -1.0f && pan <= 1.0f;
}
bool validSourceRate(const AudioClip50HD& clip,std::uint32_t rate,
                     float gain,float pitch,bool allowNativeSilentZero=false) noexcept {
    if(!AudioMixer50HD::validClip(clip)||!validRate(rate)||
       !std::isfinite(gain)||gain<0.f||gain>8.f||
       !std::isfinite(pitch)||pitch<0.f)return false;
    // Explicit native source-volume rendering policy. A zero normalized rate
    // keeps the logical voice active and freezes its cursor, producing silence.
    // It is not a claim about IOP/SPU output at source pitch zero.
    if(pitch==0.f)return allowNativeSilentZero;
    const double end=static_cast<double>(clip.frames);
    const double step=static_cast<double>(clip.sampleRate)/rate*pitch;
    // Largest source-coordinate ULP in this clip. One full ULP guarantees
    // progress even with round-to-nearest ties at the largest coordinate.
    const double minimum=std::nextafter(end,std::numeric_limits<double>::infinity())-end;
    // The sampler normalizes/stops before indexing after an advance. Bound
    // that intermediate sum below2^52 to retain exact integer coordinates;
    // validClip already bounds the actual indexed extent by2^40.
    return std::isfinite(step)&&step>=minimum&&step<=0x1p52-end;
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
AudioMixer50HD::~AudioMixer50HD()noexcept {
    if(nativeSourceWetProcessor_||auralMasterOwner_||nativeWattoRouting_.command||nativeSceneRouting_.command||nativeRaceIntroTransport_)std::terminate();
}
bool AudioMixer50HD::canRecordControlChanges(std::uint64_t count) const noexcept {
    return !controlRevisionExhausted_ &&
        count<=std::numeric_limits<std::uint64_t>::max()-controlRevision_;
}
void AudioMixer50HD::noteControlChange() noexcept {
    if (canRecordControlChanges(1)) ++controlRevision_;
    else controlRevisionExhausted_=true; // Never expose a reused terminal identity.
}
bool AudioMixer50HD::initialize(AudioVoice50HD* voices, std::size_t count,
                              std::uint32_t outputRate) noexcept {
    if (nativeSourceWetProcessor_||auralMasterOwner_||nativeWattoRouting_.command||nativeSceneRouting_.command||nativeRaceIntroTransport_||!voices || !count || count > audioMaximumVoices50HD || !validRate(outputRate) ||
        groupEpoch50IC_ == std::numeric_limits<std::uint64_t>::max())
        return false;
    voices_ = voices; count_ = count; rate_ = outputRate;
    soundMode50KG_.reset();
    masterGain_ = 1.0f; mixedFrames_ = clippedSamples_ = 0;
    ++groupEpoch50IC_; requireGroups50IC_ = false;
    noteControlChange();
    for (std::size_t i = 0; i < count_; ++i) {
        voices_[i] = {}; groupGains50IC_[i] = 1.0f; groupKnown50IC_[i] = false;
        groupPaused50IC_[i] = false;
        originalLeft50KG_[i] = originalRight50KG_[i] = 1.0f;
        playbackRevisions_[i] = 0;
    }
    return true;
}
bool AudioMixer50HD::coldNativeGarageRange(const NativeGarageRoutingClaim& c)const noexcept {
    if(!voices_||c.storage!=voices_||c.epoch!=groupEpoch50IC_||!c.voiceCount||
       c.firstVoice>=count_||c.voiceCount>count_-c.firstVoice||!canRecordControlChanges())return false;
    for(std::size_t i=c.firstVoice;i<c.firstVoice+c.voiceCount;++i){const auto& v=voices_[i];
        // Stopped/completed voices cannot prove zero contribution. Pending
        // Scene Sounds may still have no native group binding at1FB594; that
        // genuine absence is not replaced with a gain or a fake source handle.
        if(playbackRevisions_[i]||v.active||v.clip.samples||v.clip.frames||v.clip.sampleRate||v.clip.channels||
           v.clip.loopStart||v.clip.loopEnd||v.sourceFrame!=0.0||v.gain!=1.f||v.pitch!=1.f||v.pan!=0.f||v.loop||
           (c.kind==NativeGarageRoutingKind::watto&&!groupKnown50IC_[i])||
           groupGains50IC_[i]!=1.f||groupPaused50IC_[i]||
           originalLeft50KG_[i]!=1.f||originalRight50KG_[i]!=1.f)return false;
    }
    return true;
}
bool AudioMixer50HD::canClaimNativeGarageRouting(const NativeGarageRoutingClaim& c)const noexcept {
    if(!c.command||!c.main||!c.entity||!c.sequence||!c.generation||!c.objectKey||c.routingApplied||
       c.sourceGroupSlot<0||static_cast<std::size_t>(c.sourceGroupSlot)>=persistentWetRouting50KG_.size()||
       !canRecordControlChanges(1)||!coldNativeGarageRange(c))return false;
    const auto scene=c.kind==NativeGarageRoutingKind::scene;
    if(scene){
        if(!c.scene||c.group||count_!=55||c.firstVoice!=49||c.voiceCount!=4)return false;
    }else if(!c.group||c.scene||(count_!=49&&count_!=55)||c.firstVoice!=8||c.voiceCount!=41)return false;
    const auto& same=scene?nativeSceneRouting_:nativeWattoRouting_;
    const auto& other=scene?nativeWattoRouting_:nativeSceneRouting_;
    if(same.command)return false;
    if(other.command&&(other.command==c.command||other.main!=c.main||other.entity==c.entity||
       other.sourceGroupSlot==c.sourceGroupSlot||other.epoch!=c.epoch||other.storage!=c.storage||
       !nativeGarageRoutingCurrent(other,other.command,other.routingApplied)||
       !(c.firstVoice+c.voiceCount<=other.firstVoice||other.firstVoice+other.voiceCount<=c.firstVoice)))return false;
    return true;
}
bool AudioMixer50HD::retainedNativeGarageRange(const NativeGarageRoutingClaim& c)const noexcept {
    if(!voices_||c.storage!=voices_||c.epoch!=groupEpoch50IC_||!c.voiceCount||
       c.firstVoice>=count_||c.voiceCount>count_-c.firstVoice||!auralMasterOwner_||
       !nativeSourceWetProcessor_||!nativeSourceWetProcessorCurrent(
           *auralMasterOwner_,*nativeSourceWetProcessor_))return false;
    for(std::size_t i=c.firstVoice;i<c.firstVoice+c.voiceCount;++i)
        if(voices_[i].active||nativeSourceVoiceRoutes_[i].count||groupPaused50IC_[i]||
           groupGains50IC_[i]!=1.f||(c.kind==NativeGarageRoutingKind::watto&&!groupKnown50IC_[i]))return false;
    return true;
}
bool AudioMixer50HD::claimRetainedNativeGarageRouting(const NativeGarageRoutingClaim& c)noexcept {
    if(!c.command||!c.main||!c.entity||!c.sequence||!c.generation||!c.objectKey||c.routingApplied||
       c.sourceGroupSlot<0||static_cast<std::size_t>(c.sourceGroupSlot)>=persistentWetRouting50KG_.size()||
       !canRecordControlChanges(1)||!retainedNativeGarageRange(c))return false;
    const bool scene=c.kind==NativeGarageRoutingKind::scene;
    if(scene){if(!c.scene||c.group||count_!=55||c.firstVoice!=49||c.voiceCount!=4)return false;}
    else if(!c.group||c.scene||(count_!=49&&count_!=55)||c.firstVoice!=8||c.voiceCount!=41)return false;
    auto& same=scene?nativeSceneRouting_:nativeWattoRouting_;
    const auto& other=scene?nativeWattoRouting_:nativeSceneRouting_;
    if(same.command)return false;
    if(other.command&&(other.command==c.command||other.main!=c.main||other.entity==c.entity||
       other.sourceGroupSlot==c.sourceGroupSlot||other.epoch!=c.epoch||other.storage!=c.storage||
       !(other.routingApplied?nativeGarageRoutingLiveCurrent(other,other.command):retainedNativeGarageRange(other))||
       !(c.firstVoice+c.voiceCount<=other.firstVoice||other.firstVoice+other.voiceCount<=c.firstVoice)))return false;
    same=c;noteControlChange();return true;
}
bool AudioMixer50HD::applyRetainedNativeGarageRouting(NativeGarageRoutingClaim& c,const void* command)noexcept {
    if(!command||c.command!=command||c.routingApplied||!retainedNativeGarageRange(c)||
       c.sourceGroupSlot<0||static_cast<std::size_t>(c.sourceGroupSlot)>=persistentWetRouting50KG_.size()||
       !canRecordControlChanges(1))return false;
    // All source reservations have genuinely stopped. Preserve clip custody,
    // and apply the actual ordered source-slot byte without a fake live scan.
    persistentWetRouting50KG_[c.sourceGroupSlot]=1;c.routingApplied=true;noteControlChange();return true;
}
bool AudioMixer50HD::claimRetainedNativeWattoRouting(const NativeWattoRoutingClaim& c)noexcept {
    return c.kind==NativeGarageRoutingKind::watto&&claimRetainedNativeGarageRouting(c);
}
bool AudioMixer50HD::applyRetainedNativeWattoRouting(const void* command)noexcept {
    return applyRetainedNativeGarageRouting(nativeWattoRouting_,command);
}
bool AudioMixer50HD::claimRetainedNativeSceneRouting(const NativeSceneRoutingClaim& c)noexcept {
    return c.kind==NativeGarageRoutingKind::scene&&claimRetainedNativeGarageRouting(c);
}
bool AudioMixer50HD::applyRetainedNativeSceneRouting(const void* command)noexcept {
    return applyRetainedNativeGarageRouting(nativeSceneRouting_,command);
}
bool AudioMixer50HD::nativeRetainedWattoRoutingCurrent(const void* command,bool applied)const noexcept {
    const auto& c=nativeWattoRouting_;
    if(applied)return nativeGarageRoutingLiveCurrent(c,command);
    return command&&c.command==command&&!c.routingApplied&&c.main&&c.entity&&
        c.sequence&&c.generation&&c.objectKey&&c.sourceGroupSlot>=0&&
        static_cast<std::size_t>(c.sourceGroupSlot)<persistentWetRouting50KG_.size()&&
        retainedNativeGarageRange(c);
}
bool AudioMixer50HD::nativeRetainedSceneRoutingCurrent(const void* command,bool applied)const noexcept {
    const auto& c=nativeSceneRouting_;
    if(applied)return nativeGarageRoutingLiveCurrent(c,command);
    return command&&c.command==command&&!c.routingApplied&&c.main&&c.entity&&
        c.sequence&&c.generation&&c.objectKey&&c.sourceGroupSlot>=0&&
        static_cast<std::size_t>(c.sourceGroupSlot)<persistentWetRouting50KG_.size()&&
        retainedNativeGarageRange(c);
}
bool AudioMixer50HD::canClaimNativeWattoRouting(const NativeWattoRoutingClaim& c)const noexcept {
    return c.kind==NativeGarageRoutingKind::watto&&canClaimNativeGarageRouting(c);
}
bool AudioMixer50HD::claimNativeWattoRouting(const NativeWattoRoutingClaim& c)noexcept {
    if(!canClaimNativeWattoRouting(c))return false;
    nativeWattoRouting_=c;noteControlChange();return true;
}
bool AudioMixer50HD::claimNativeSceneRouting(const NativeSceneRoutingClaim& c)noexcept {
    if(c.kind!=NativeGarageRoutingKind::scene||!canClaimNativeGarageRouting(c))return false;
    nativeSceneRouting_=c;noteControlChange();return true;
}
bool AudioMixer50HD::nativeGarageRoutingCurrent(const NativeGarageRoutingClaim& c,
    const void* command,bool applied)const noexcept {
    return command&&c.command==command&&c.routingApplied==applied&&coldNativeGarageRange(c)&&
        c.sourceGroupSlot>=0&&static_cast<std::size_t>(c.sourceGroupSlot)<persistentWetRouting50KG_.size()&&
        (!applied||persistentWetRouting50KG_[c.sourceGroupSlot]==1);
}
bool AudioMixer50HD::nativeWattoRoutingCurrent(const void* command,bool applied)const noexcept {
    return nativeGarageRoutingCurrent(nativeWattoRouting_,command,applied);
}
bool AudioMixer50HD::nativeSceneRoutingCurrent(const void* command,bool applied)const noexcept {
    return nativeGarageRoutingCurrent(nativeSceneRouting_,command,applied);
}
bool AudioMixer50HD::nativeSceneImmediateRoutingCurrent(const void* command)const noexcept {
    const auto& c=nativeSceneRouting_;
    if(c.routingApplied)return nativeGarageRoutingLiveCurrent(c,command);
    // The real accepted Scene25 claim already identifies its native slots and
    // actual source registry slot. Immediate RPC8 may precede its submission.
    // Do not replace retained routing history with a fresh zero/true value.
    return command&&c.command==command&&c.kind==NativeGarageRoutingKind::scene&&
        c.main&&c.scene&&!c.group&&c.entity&&c.sequence&&c.generation&&c.objectKey&&
        voices_&&c.storage==voices_&&c.epoch==groupEpoch50IC_&&count_==55&&
        c.firstVoice==49&&c.voiceCount==4&&c.sourceGroupSlot>=0&&
        static_cast<std::size_t>(c.sourceGroupSlot)<persistentWetRouting50KG_.size()&&
        persistentWetRouting50KG_[c.sourceGroupSlot]<=1&&auralMasterOwner_&&nativeSourceWetProcessor_&&
        nativeSourceWetProcessorCurrent(*auralMasterOwner_,*nativeSourceWetProcessor_);
}
bool AudioMixer50HD::nativeSceneSampleVoiceCurrent(std::size_t slot)const noexcept {
    const auto& c=nativeSceneRouting_;
    if(!nativeSceneImmediateRoutingCurrent(c.command)||slot<c.firstVoice||
       slot-c.firstVoice>=c.voiceCount||!playbackRevisions_[slot]||
       !groupKnown50IC_[slot]||groupGains50IC_[slot]!=1.f||groupPaused50IC_[slot])return false;
    const auto& v=voices_[slot];const auto& route=nativeSourceVoiceRoutes_[slot];
    if(!validClip(v.clip)||v.clip.channels!=1||v.loop||
       !nativeSourceRoutes_.current(route)||route.count!=1)return false;
    const auto& cell=nativeSourceRoutes_.cells_[route.cells[0]];
    return cell.kind==NativeVoiceKind::sample&&cell.sourceBound&&!cell.loop&&!cell.paused&&
        std::isfinite(cell.duration)&&cell.duration>0.f&&std::isfinite(cell.elapsed)&&cell.elapsed>=0.f;
}
bool AudioMixer50HD::nativeWattoSourceVoiceCurrent(std::size_t slot)const noexcept {
    const auto& c=nativeWattoRouting_;
    // Render-only use of a real privately published Watto reservation. The
    // caller Main has already checked its strong Group/Sound/PCM ownership;
    // this bounded Mixer proof cannot publish, steal or revise that voice.
    if(c.kind!=NativeGarageRoutingKind::watto||count_!=55||
       !nativeGarageRoutingLiveCurrent(c,c.command)||slot<c.firstVoice||
       slot-c.firstVoice>=c.voiceCount||!playbackRevisions_[slot]||
       !groupKnown50IC_[slot]||groupGains50IC_[slot]!=1.f||groupPaused50IC_[slot])return false;
    const auto& v=voices_[slot];const auto& route=nativeSourceVoiceRoutes_[slot];
    if(!validClip(v.clip)||v.clip.channels!=1||v.clip.sampleRate!=22050||
       v.clip.loopStart||v.clip.loopEnd||v.loop||v.gain!=1.f||v.pan!=0.f||
       v.pitch!=OriginalUiGarageWattoAudio50KG::nativeRendererPitch||
       !std::isfinite(v.sourceFrame)||v.sourceFrame<0.0||
       (v.active&&v.sourceFrame>=static_cast<double>(v.clip.frames))||
       !nativeSourceRoutes_.current(route)||route.count!=1)return false;
    const auto& cell=nativeSourceRoutes_.cells_[route.cells[0]];
    if(!cell.sourceBound||cell.loop||cell.paused||!std::isfinite(cell.duration)||cell.duration<=0.f||
       !std::isfinite(cell.elapsed)||cell.elapsed<0.f)return false;
    std::int32_t volume=0;
    if(cell.kind==NativeVoiceKind::sample){
        // Actual wing SourceSound flags1, volume17, mono/nonloop sample.
        if(cell.streamSlot||cell.streamGeneration)return false;
        volume=17;
    }else if(cell.kind==NativeVoiceKind::stream){
        // Actual response flags2, volume100, mono/nonloop stream. The native
        // Aural-issued slot/generation survives from atomic publication; it
        // is NOT a guest stream handle or an ID inferred from PCM length.
        if(cell.streamSlot>=8||!cell.streamGeneration)return false;
        std::array<std::uint64_t,8> references{};
        if(!nativeSourceRoutes_.streamReferences267CB0(references)||
           references[cell.streamSlot]!=cell.streamGeneration)return false;
        volume=100;
    }else return false;
    float left=0.f,right=0.f;
    return originalRouting50KG(0,left,right,volume)&&
        originalLeft50KG_[slot]==left&&originalRight50KG_[slot]==right;
}
bool AudioMixer50HD::applyNativeSceneImmediateRouting(const void* command)noexcept {
    auto& c=nativeSceneRouting_;
    if(c.routingApplied||!nativeSceneImmediateRoutingCurrent(command)||!canRecordControlChanges(1))return false;
    for(std::size_t slot=c.firstVoice;slot<c.firstVoice+c.voiceCount;++slot)
        if((voices_[slot].active||nativeSourceVoiceRoutes_[slot].count)&&
           !nativeSceneSampleVoiceCurrent(slot))return false;
    // IRX B820 updates the real group byte and every currently matching voice.
    // These Scene samples all have actual InitSound policy1 and no reached
    // command14/20 override. Their render sends consume this same slot byte,
    // so this write updates both existing and future matching native voices.
    persistentWetRouting50KG_[c.sourceGroupSlot]=1;c.routingApplied=true;noteControlChange();return true;
}
// Live custody validates the exact retained claim, not constructor-zero voice
// history. Stopped clips and their revisions remain owned by the Sound backing.
bool AudioMixer50HD::nativeSourceVoiceReserved(std::size_t slot,std::uint64_t revision)const noexcept {
    if(!voices_||slot>=count_||!revision||playbackRevisions_[slot]!=revision||
       !nativeSourceWetProcessor_||!auralMasterOwner_||
       !nativeSourceWetProcessorCurrent(*auralMasterOwner_,*nativeSourceWetProcessor_))return false;
    return nativeSourceRoutes_.current(nativeSourceVoiceRoutes_[slot]);
}
bool AudioMixer50HD::nativeGarageRoutingLiveCurrent(const NativeGarageRoutingClaim& c,
    const void* command)const noexcept {
    if(!command||c.command!=command||!c.routingApplied||!c.main||!c.entity||
       !c.sequence||!c.generation||!c.objectKey||!voices_||c.storage!=voices_||
       c.epoch!=groupEpoch50IC_||!c.voiceCount||c.firstVoice>=count_||
       c.voiceCount>count_-c.firstVoice||c.sourceGroupSlot<0||
       static_cast<std::size_t>(c.sourceGroupSlot)>=persistentWetRouting50KG_.size()||
       persistentWetRouting50KG_[c.sourceGroupSlot]!=1||!auralMasterOwner_||
       !nativeSourceWetProcessor_||!nativeSourceWetProcessorCurrent(
           *auralMasterOwner_,*nativeSourceWetProcessor_))return false;
    if(c.kind==NativeGarageRoutingKind::scene)
        return c.scene&&!c.group&&count_==55&&c.firstVoice==49&&c.voiceCount==4;
    return c.group&&!c.scene&&(count_==49||count_==55)&&c.firstVoice==8&&c.voiceCount==41;
}
bool AudioMixer50HD::canRetireNativeGarageRouting(const NativeGarageRoutingClaim& c,
    const void* command)const noexcept {
    if(!nativeGarageRoutingLiveCurrent(c,command)||!canRecordControlChanges(1))return false;
    for(std::size_t i=c.firstVoice;i<c.firstVoice+c.voiceCount;++i)
        if(voices_[i].active||nativeSourceVoiceRoutes_[i].count)return false;
    return true;
}
bool AudioMixer50HD::nativeWattoRoutingLiveCurrent(const void* command)const noexcept {
    return nativeGarageRoutingLiveCurrent(nativeWattoRouting_,command);
}
bool AudioMixer50HD::canRetireNativeWattoRouting(const void* command)const noexcept {
    return canRetireNativeGarageRouting(nativeWattoRouting_,command);
}
bool AudioMixer50HD::retireNativeWattoRouting(const void* command)noexcept {
    if(!canRetireNativeWattoRouting(command))return false;
    // No clip, revision, source-slot byte or DSP history is cleared here.
    nativeWattoRouting_={};noteControlChange();return true;
}
bool AudioMixer50HD::nativeSceneRoutingLiveCurrent(const void* command)const noexcept {
    return nativeGarageRoutingLiveCurrent(nativeSceneRouting_,command);
}
bool AudioMixer50HD::canRetireNativeSceneRouting(const void* command)const noexcept {
    return canRetireNativeGarageRouting(nativeSceneRouting_,command);
}
bool AudioMixer50HD::retireNativeSceneRouting(const void* command)noexcept {
    if(!canRetireNativeSceneRouting(command))return false;
    // No clip, revision, source-slot byte or DSP history is cleared here.
    nativeSceneRouting_={};noteControlChange();return true;
}
bool AudioMixer50HD::applyColdNativeGarageRouting(NativeGarageRoutingClaim& c,const void* command)noexcept {
    if(!nativeGarageRoutingCurrent(c,command,false)||!canRecordControlChanges(1))return false;
    // IRX B848..B86C writes the source-slot byte. The active-voice scan has
    // exactly zero matches in the claimed native range, proven, not skipped.
    persistentWetRouting50KG_[c.sourceGroupSlot]=1;
    c.routingApplied=true;noteControlChange();return true;
}
bool AudioMixer50HD::applyColdNativeWattoRouting(const void* command)noexcept {
    return applyColdNativeGarageRouting(nativeWattoRouting_,command);
}
bool AudioMixer50HD::applyColdNativeSceneRouting(const void* command)noexcept {
    return applyColdNativeGarageRouting(nativeSceneRouting_,command);
}
bool AudioMixer50HD::canRetireColdNativeWattoRouting(const void* command)const noexcept {
    return nativeWattoRoutingCurrent(command,true)&&canRecordControlChanges(1);
}
bool AudioMixer50HD::retireColdNativeWattoRouting(const void* command)noexcept {
    if(!canRetireColdNativeWattoRouting(command))return false;
    // Detach only Watto after actual235150. Scene custody is independent.
    // Neither the source slot byte nor global effect history is reset.
    nativeWattoRouting_={};noteControlChange();return true;
}
bool AudioMixer50HD::canRetireColdNativeSceneRouting(const void* command)const noexcept {
    return nativeSceneRoutingCurrent(command,true)&&canRecordControlChanges(1);
}
bool AudioMixer50HD::retireColdNativeSceneRouting(const void* command)noexcept {
    if(!canRetireColdNativeSceneRouting(command))return false;
    // Only Main's real final source-owner retirement may remove this claim.
    // SoundGroup destruction never clears the persistent source-slot byte.
    nativeSceneRouting_={};noteControlChange();return true;
}
bool AudioMixer50HD::canRecycleRetiredColdSceneBindings()const noexcept {
    if(!voices_||count_!=55||nativeSceneRouting_.command||!canRecordControlChanges(1))return false;
    for(std::size_t i=49;i<53;++i){const auto& v=voices_[i];
        if(!groupKnown50IC_[i]||groupGains50IC_[i]!=1.f||groupPaused50IC_[i]||
           playbackRevisions_[i]||v.active||v.clip.samples||v.clip.frames||v.clip.sampleRate||
           v.clip.channels||v.clip.loopStart||v.clip.loopEnd||v.sourceFrame!=0.0||
           v.gain!=1.f||v.pitch!=1.f||v.pan!=0.f||v.loop||
           originalLeft50KG_[i]!=1.f||originalRight50KG_[i]!=1.f)return false;
    }
    return true;
}
bool AudioMixer50HD::recycleRetiredColdSceneBindings()noexcept {
    if(!canRecycleRetiredColdSceneBindings())return false;
    // Only host binding presence returns to constructor state. Unity is the
    // unbound initial gain too; no source Group field or wet byte is written.
    for(std::size_t i=49;i<53;++i)groupKnown50IC_[i]=false;
    noteControlChange();return true;
}
bool AudioMixer50HD::canRecycleRetiredSceneBindings()const noexcept {
    if(!voices_||count_!=55||nativeSceneRouting_.command||!canRecordControlChanges(1)||
       !auralMasterOwner_||!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(*auralMasterOwner_,*nativeSourceWetProcessor_))return false;
    for(std::size_t i=49;i<53;++i)
        if(voices_[i].active||nativeSourceVoiceRoutes_[i].count||!groupKnown50IC_[i]||
           groupGains50IC_[i]!=1.f||groupPaused50IC_[i])return false;
    return true;
}
bool AudioMixer50HD::recycleRetiredSceneBindings()noexcept {
    if(!canRecycleRetiredSceneBindings())return false;
    // Main's genuine retired Scene owner retains all stopped clip backing.
    // Recycle binding presence only, never voice identity or source history.
    for(std::size_t i=49;i<53;++i)groupKnown50IC_[i]=false;
    noteControlChange();return true;
}
bool AudioMixer50HD::publishColdNativeSceneVoiceGroup50IC(
    const void* command,std::size_t slot,float sourceGroupGain)noexcept {
    const auto& c=nativeSceneRouting_;
    if(!nativeSceneRoutingCurrent(command,c.routingApplied)||slot<c.firstVoice||
       slot-c.firstVoice>=c.voiceCount||groupKnown50IC_[slot]||sourceGroupGain!=1.f||
       !canRecordControlChanges(1))return false;
    // Actual source Group38=100 supplies this initial unity gain, exactly as
    // the unclaimed source-row publication. No voice, pause, route or claim
    // identity changes; all playback/control entries remain wet-gated.
    groupGains50IC_[slot]=sourceGroupGain;groupKnown50IC_[slot]=true;noteControlChange();return true;
}
bool AudioMixer50HD::publishRetainedNativeSceneVoiceGroup50IC(
    const void* command,std::size_t slot,float sourceGroupGain)noexcept {
    const auto& c=nativeSceneRouting_;
    if(!command||c.command!=command||!retainedNativeGarageRange(c)||
       slot<c.firstVoice||slot-c.firstVoice>=c.voiceCount||groupKnown50IC_[slot]||
       sourceGroupGain!=1.f||!canRecordControlChanges(1))return false;
    groupGains50IC_[slot]=sourceGroupGain;groupKnown50IC_[slot]=true;noteControlChange();return true;
}
bool AudioMixer50HD::bindNativeSourceWetProcessor(const OriginalApplicationAural50KG& owner,
    NativeSourceReverb50KG& processor,decltype(&NativeSourceReverb50KG::preflight) preflight,
    decltype(&NativeSourceReverb50KG::process) process) noexcept {
    if(!preflight||!process||auralMasterOwner_!=&owner||nativeSourceWetProcessor_||!voices_||!count_||rate_!=48000||
       mixedFrames_||nativeWattoRouting_.command||nativeSceneRouting_.command||
       !soundMode50KG_||!soundMode50KG_->ready()||
       !processor.configured()||processor.nextFrame()!=mixedFrames_)return false;
    for(std::size_t i=0;i<count_;++i)if(voices_[i].active||playbackRevisions_[i]||nativeSourceVoiceRoutes_[i].count)return false;
    for(const auto& c:nativeSourceRoutes_.cells_)if(c.reserved)return false;
    // Actual source267910 cold initialization, not a reset on Garage entry.
    nativeSourceRoutes_=NativeSourceVoiceRoutes50KG{};
    nativeSourceWetProcessor_=&processor;nativeSourceWetEpoch_=groupEpoch50IC_;
    nativeWetPreflight_=preflight;nativeWetProcess_=process;
    return true;
}
bool AudioMixer50HD::nativeSourceWetProcessorCurrent(const OriginalApplicationAural50KG& owner,
    const NativeSourceReverb50KG& processor)const noexcept {
    return auralMasterOwner_==&owner&&nativeSourceWetProcessor_==&processor&&
        nativeSourceWetEpoch_==groupEpoch50IC_&&nativeWetPreflight_&&nativeWetProcess_&&rate_==48000&&processor.configured()&&
        processor.nextFrame()==mixedFrames_;
}
bool AudioMixer50HD::canReleaseNativeSourceWetProcessor(const OriginalApplicationAural50KG& owner,
    const NativeSourceReverb50KG& processor)const noexcept {
    if(!nativeSourceWetProcessorCurrent(owner,processor)||nativeWattoRouting_.command||nativeSceneRouting_.command)return false;
    for(std::size_t i=0;i<count_;++i)if(voices_[i].active||nativeSourceVoiceRoutes_[i].count)return false;
    for(const auto& c:nativeSourceRoutes_.cells_)if(c.reserved)return false;
    return true;
}
bool AudioMixer50HD::releaseNativeSourceWetProcessor(const OriginalApplicationAural50KG& owner,
    NativeSourceReverb50KG& processor) noexcept {
    if(!canReleaseNativeSourceWetProcessor(owner,processor))return false;
    // No master write, work clear or invented drain. The owner destroys its
    // retained history only after this exact borrow has been retired.
    nativeSourceWetProcessor_=nullptr;nativeWetPreflight_=nullptr;nativeWetProcess_=nullptr;nativeSourceWetEpoch_=0;return true;
}
bool AudioMixer50HD::canRetireStoppedNativeGarageClips(const OriginalUiMainAudio50KG& main,
    const AudioVoiceObservation50HD* expected,std::size_t size)const noexcept {
    using Main=OriginalUiMainAudio50KG;
    if(std::this_thread::get_id()!=main.nativeControlThread_||main.mixer_!=this||!main.prepared_||
       main.finalNativePcmRetired_||!main.ordinaryOutputRetired_||main.mixerEpoch_!=groupEpoch50IC_||
       main.mixerRate_!=rate_||main.mixerCount_!=count_||count_!=Main::garageSceneRequiredVoices||
       !voices_||!expected||size!=Main::finalGarageClipCount||
       main.nativeWetOwner_!=auralMasterOwner_||main.nativeWetProcessor_!=nativeSourceWetProcessor_||
       !nativeSourceWetProcessor_||main.nativeWetEpoch_!=nativeSourceWetEpoch_||
       nativeWattoRouting_.command||nativeSceneRouting_.command)return false;
    // Reject all native reservations, including stale per-slot routes and a
    // reserved pool cell which no logical slot presently identifies. EOF alone
    // cannot satisfy this exact actual source-stop/release boundary.
    for(std::size_t slot=0;slot<count_;++slot)
        if(voices_[slot].active||nativeSourceVoiceRoutes_[slot].count)return false;
    for(const auto& cell:nativeSourceRoutes_.cells_)if(cell.reserved)return false;
    const auto sameClip=[](const AudioClip50HD& a,const AudioClip50HD& b)noexcept {
        return a.samples==b.samples&&a.frames==b.frames&&a.sampleRate==b.sampleRate&&
            a.channels==b.channels&&a.loopStart==b.loopStart&&a.loopEnd==b.loopEnd;
    };
    for(std::size_t i=0;i<size;++i){AudioVoiceObservation50HD now;const auto& old=expected[i];
        if(!inspectVoice(Main::courseRequiredVoices+i,now)||old.voice.active||
           !sameClip(now.voice.clip,old.voice.clip)||now.playbackRevision!=old.playbackRevision||
           now.voice.sourceFrame!=old.voice.sourceFrame||now.voice.gain!=old.voice.gain||now.voice.pitch!=old.voice.pitch||
           now.voice.pan!=old.voice.pan||now.voice.loop!=old.voice.loop||now.groupKnown!=old.groupKnown||
           now.groupPaused!=old.groupPaused||now.groupGain!=old.groupGain||
           now.originalLeft!=old.originalLeft||now.originalRight!=old.originalRight)return false;
    }
    return true;
}
void AudioMixer50HD::retireStoppedNativeGarageClipsPreflighted()noexcept {
    // Native lifetime withdrawal only, not a source stop/restart/control call.
    // Preserve playback/control revisions, cursor, route ages/generations,
    // bus/group state, clocks, persistent routing and the entire wet processor.
    for(std::size_t slot=OriginalUiMainAudio50KG::courseRequiredVoices;
        slot<OriginalUiMainAudio50KG::garageSceneVoiceEnd;++slot)voices_[slot].clip={};
}
bool AudioMixer50HD::canServiceNativeSourceVoices267CB0(
    const OriginalApplicationAural50KG& owner,float factor)const noexcept {
    if(!std::isfinite(factor)||factor<0.f||!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(owner,*nativeSourceWetProcessor_))return false;
    auto next=nativeSourceRoutes_;
    if(!next.serviceChannels267CB0(factor))return false;
    bool expires=false;
    for(std::size_t i=0;i<count_;++i){const auto& route=nativeSourceVoiceRoutes_[i];
        if(!route.count){if(voices_[i].active)return false;continue;}
        if(!nativeSourceRoutes_.current(route))return false;
        if(!next.current(route)){
            // Native PCM completion and source reservation expiry are distinct.
            // The bounded service cannot drop a still-rendering core route.
            // Leave the whole update unchanged until its real tail is drained.
            if(voices_[i].active)return false;
            expires=true;
        }
    }
    return !expires||canRecordControlChanges(1);
}
bool AudioMixer50HD::serviceNativeSourceVoices267CB0(
    const OriginalApplicationAural50KG& owner,float factor)noexcept {
    if(!canServiceNativeSourceVoices267CB0(owner,factor))return false;
    if(!nativeSourceRoutes_.serviceChannels267CB0(factor))std::terminate();
    bool expired=false;
    for(std::size_t i=0;i<count_;++i){auto& route=nativeSourceVoiceRoutes_[i];
        if(route.count&&!nativeSourceRoutes_.current(route)){
            // Proven stopped by the identical preflight. Keep raw clip and
            // its strong Sound lease; only the expired EE reservation ends.
            route={};expired=true;
        }
    }
    if(expired)noteControlChange();
    // Aural separately validates/owns the actual37C stream-slot suffix.
    // Live special branches and still-playing expiry remain closed. Stream
    // associations are retained through their exact inactive grace interval.
    return true;
}
bool AudioMixer50HD::canReserveNativeSourceMonoVoices(std::size_t count)const noexcept {
    if(count>48||!auralMasterOwner_||!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(*auralMasterOwner_,*nativeSourceWetProcessor_))return false;
    auto samples=nativeSourceRoutes_,streams=nativeSourceRoutes_;
    // Both original mono scan orders must fit the promised bound. These are
    // unpublished reservations, not fake elapsed/publication or a DSP write.
    for(std::size_t i=0;i<count;++i){NativeVoiceRoute a,b;
        if(!samples.reserve(NativeVoiceKind::sample,1,a)||
           !streams.reserve(NativeVoiceKind::stream,1,b))return false;
    }
    return true;
}
bool AudioMixer50HD::nativeSourceStreamReferences267CB0(
    const OriginalApplicationAural50KG& owner,float factor,bool afterService,
    std::array<std::uint64_t,8>& out)const noexcept {
    if(!std::isfinite(factor)||factor<0.f||!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(owner,*nativeSourceWetProcessor_))return false;
    auto next=nativeSourceRoutes_;
    if(afterService&&(!canServiceNativeSourceVoices267CB0(owner,factor)||
       !next.serviceChannels267CB0(factor)))return false;
    return next.streamReferences267CB0(out);
}
AudioMixer50HD::NativeVoiceKind AudioMixer50HD::nativeDefaultVoiceKind(std::size_t slot)const noexcept {
    // Actual retained frontend producers: stereo Song stream, six sample
    // effect instances, Rider stream; four authenticated Scene sample slots.
    if(slot==0||slot==7)return NativeVoiceKind::stream;
    if(slot>=1&&slot<=6)return NativeVoiceKind::sample;
    if(nativeSceneRouting_.command&&slot>=nativeSceneRouting_.firstVoice&&
       slot-nativeSceneRouting_.firstVoice<nativeSceneRouting_.voiceCount)return NativeVoiceKind::sample;
    return NativeVoiceKind::unknown;
}
bool AudioMixer50HD::prepareNativeVoiceRoute(std::size_t slot,const AudioClip50HD& clip,
    NativeVoiceKind kind,NativeSourceVoiceRoutes50KG& pool,NativeVoiceRoute& route)const noexcept {
    if(!nativeSourceWetProcessor_)return true;
    if(slot>=count_||!auralMasterOwner_||!nativeSourceWetProcessorCurrent(*auralMasterOwner_,*nativeSourceWetProcessor_)||
       kind==NativeVoiceKind::unknown||!validClip(clip))return false;
    pool=nativeSourceRoutes_;route=nativeSourceVoiceRoutes_[slot];
    if(voices_[slot].active&&!route.count)return false;
    // Existing native logical-instance replacement is one atomic stop/play.
    // Plan it on a copy so unavailable source routes leave the old voice alive.
    return pool.release(route)&&pool.reserve(kind,clip.channels,route);
}
void AudioMixer50HD::retireNativeVoiceRoute(std::size_t slot)noexcept {
    if(!nativeSourceWetProcessor_)return;
    if(slot>=count_||!nativeSourceRoutes_.release(nativeSourceVoiceRoutes_[slot]))std::terminate();
}
bool AudioMixer50HD::nativeVoiceWetSend(std::size_t slot)const noexcept {
    const NativeGarageRoutingClaim* c=nullptr;
    for(const auto* claim:{&nativeWattoRouting_,&nativeSceneRouting_})
        if(claim->command&&slot>=claim->firstVoice&&slot-claim->firstVoice<claim->voiceCount)c=claim;
    // Reserved source groups0/1 start dry. Only genuine ordered command25
    // publication changes the reached Garage groups. Per-sample AE64 overrides
    // are outside this bounded producer set; InitSound5EA4 publishes policy1.
    if(!c)return false;
    if((!c->routingApplied&&c->kind!=NativeGarageRoutingKind::scene)||
       c->sourceGroupSlot<0||static_cast<std::size_t>(c->sourceGroupSlot)>=persistentWetRouting50KG_.size())return false;
    const auto value=persistentWetRouting50KG_[c->sourceGroupSlot];
    const auto& r=nativeSourceVoiceRoutes_[slot];
    if(!r.count)return false;
    const auto kind=nativeSourceRoutes_.cells_[r.cells[0]].kind;
    return kind==NativeVoiceKind::stream?value!=0:value==1;
}
bool AudioMixer50HD::nativeWetVoiceAdmitted(std::size_t slot)const noexcept {
    const auto owns=[&](const NativeGarageRoutingClaim& c){
        return c.command&&slot>=c.firstVoice&&slot-c.firstVoice<c.voiceCount;
    };
    // Both pending and applied command25(true) require a real wet processor.
    // A persistent byte is routing state, never permission to play dry PCM,
    // alter gains, pause the group, or steal its voices.
    // DSP construction is not the live Sound/custody retirement contract.
    // Preserve the existing claimed-range gate until that concrete owner
    // accepts played histories; never broaden cold retirement implicitly.
    return !owns(nativeWattoRouting_)&&!owns(nativeSceneRouting_)&&!nativeRaceIntroClaimed(slot);
}
bool AudioMixer50HD::inspectNativeGarageRouting(const NativeGarageRoutingClaim& c,
    NativeGarageRoutingObservation& out)const noexcept {
    out={};if(!c.command)return false;
    out.kind=c.kind;out.main=c.main;out.group=c.group;out.scene=c.scene;out.entity=c.entity;out.sequence=c.sequence;
    out.generation=c.generation;out.objectKey=c.objectKey;out.epoch=c.epoch;
    out.firstVoice=c.firstVoice;out.voiceCount=c.voiceCount;out.sourceGroupSlot=c.sourceGroupSlot;
    if(c.sourceGroupSlot>=0&&static_cast<std::size_t>(c.sourceGroupSlot)<persistentWetRouting50KG_.size())
        out.persistentRoutingByte=persistentWetRouting50KG_[c.sourceGroupSlot];
    out.queued=!c.routingApplied;out.routingApplied=c.routingApplied;out.neverPlayed=coldNativeGarageRange(c);
    return true;
}
bool AudioMixer50HD::inspectNativeWattoRouting(NativeWattoRoutingObservation& out)const noexcept {
    return inspectNativeGarageRouting(nativeWattoRouting_,out);
}
bool AudioMixer50HD::inspectNativeSceneRouting(NativeSceneRoutingObservation& out)const noexcept {
    return inspectNativeGarageRouting(nativeSceneRouting_,out);
}
bool AudioMixer50HD::inspectPersistentWetRoutingByte(std::size_t slot,std::uint8_t& out)const noexcept {
    out=0;if(slot>=persistentWetRouting50KG_.size())return false;out=persistentWetRouting50KG_[slot];return true;
}
bool AudioMixer50HD::bindOriginalSoundMode50KG(std::shared_ptr<OriginalSoundMode50KG> owner) noexcept {
    if(!owner||!owner->ready()||soundMode50KG_||!voices_||!count_||mixedFrames_)return false;
    for(std::size_t i=0;i<count_;++i)if(voices_[i].active)return false;
    soundMode50KG_=std::move(owner);noteControlChange();return true;
}
bool AudioMixer50HD::originalRouting50KG(std::int32_t angle,float& left,float& right,
    std::int32_t volume) const noexcept {
    if(!soundMode50KG_||!soundMode50KG_->ready())return false;
    game::OriginalSoundRouting50KG routing;
    if(!game::projectOriginalSoundRouting26CBA050KG(soundMode50KG_->fields(),angle,volume,routing))return false;
    float routedLeft=static_cast<float>(routing.leftPercent)/100.f;
    float routedRight=static_cast<float>(routing.rightPercent)/100.f;
    // Actual PS2SOUND SettingsVoice8A04 retains packet+14 at voice+2C.
    // ServiceVoices2074..20E8 and StreamEntry534..5C0 write ordinary VOLL/R
    // for0/1;76 complements the left register,82 complements the right.
    // Supplied LIBSD export5 writes that value unchanged to the SPU2 port.
    // Keep this backend's established percentage/group/master magnitude
    // policy and apply the proved channel polarity only. Hardware0x7fff-q
    // encodes -q-1, including -1 at q=0; native zero remains silence. Neither
    //14-bit quantization nor the original service/ramp cadence is claimed.
    switch(routing.phaseSelector){
        case 0: case 1: break;
        case 76: routedLeft=-routedLeft;break;
        case 82: routedRight=-routedRight;break;
        default:return false;
    }
    left=routedLeft;right=routedRight;
    return true;
}
bool AudioMixer50HD::playOriginalAngle50KG(std::size_t slot,const AudioClip50HD& clip,
    float gain,float pitch,bool loop,std::int32_t engineAngle) noexcept {
    float left=0.f,right=0.f;
    if(slot>=count_||!nativeWetVoiceAdmitted(slot)||!validClip(clip)||!validControls(gain,pitch,0.f)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right))return false;
    NativeSourceVoiceRoutes50KG pool;NativeVoiceRoute route;
    if(!prepareNativeVoiceRoute(slot,clip,nativeDefaultVoiceKind(slot),pool,route))return false;
    if(nativeSourceWetProcessor_){nativeSourceRoutes_=pool;nativeSourceVoiceRoutes_[slot]=route;}
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot]={clip,0.0,gain,pitch,0.f,true,loop};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::controlsOriginalAngle50KG(std::size_t slot,float gain,float pitch,
    std::int32_t engineAngle) noexcept {
    float left=0.f,right=0.f;
    if(slot>=count_||!nativeWetVoiceAdmitted(slot)||!validControls(gain,pitch,0.f)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right))return false;
    auto& voice=voices_[slot];
    if(voice.gain!=gain||voice.pitch!=pitch||voice.pan!=0.f||
        originalLeft50KG_[slot]!=left||originalRight50KG_[slot]!=right)noteControlChange();
    voice.gain=gain;voice.pitch=pitch;voice.pan=0.f;
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::play(std::size_t slot, const AudioClip50HD& clip, float gain,
                        float pitch, bool loop, float pan) noexcept {
    if(soundMode50KG_)return pan==0.f&&playOriginalAngle50KG(slot,clip,gain,pitch,loop,0);
    if(nativeSourceWetProcessor_)return false;
    if (slot >= count_ || !nativeWetVoiceAdmitted(slot) || !validClip(clip) || !validControls(gain, pitch, pan) ||
        (requireGroups50IC_ && !groupKnown50IC_[slot])) return false;
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot] = {clip, 0.0, gain, pitch, pan, true, loop};
    return true;
}
bool AudioMixer50HD::controls(std::size_t slot, float gain, float pitch, float pan) noexcept {
    if(soundMode50KG_)return pan==0.f&&controlsOriginalAngle50KG(slot,gain,pitch,0);
    if (slot >= count_ || !nativeWetVoiceAdmitted(slot) || !validControls(gain, pitch, pan) ||
        (requireGroups50IC_ && !groupKnown50IC_[slot])) return false;
    auto& voice = voices_[slot];
    if(voice.gain!=gain||voice.pitch!=pitch||voice.pan!=pan)noteControlChange();
    voice.gain = gain; voice.pitch = pitch; voice.pan = pan;
    return true;
}
bool AudioMixer50HD::playSourceRate(std::size_t slot,const AudioClip50HD& clip,
    float gain,float pitch,bool loop,std::int32_t engineAngle) noexcept {
    float left=1.f,right=1.f;
    if(slot>=count_||!nativeWetVoiceAdmitted(slot)||!validSourceRate(clip,rate_,gain,pitch)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       (!soundMode50KG_&&engineAngle!=0)||
       (soundMode50KG_&&!originalRouting50KG(engineAngle,left,right)))return false;
    NativeSourceVoiceRoutes50KG pool;NativeVoiceRoute route;
    if(!prepareNativeVoiceRoute(slot,clip,nativeDefaultVoiceKind(slot),pool,route))return false;
    if(nativeSourceWetProcessor_){nativeSourceRoutes_=pool;nativeSourceVoiceRoutes_[slot]=route;}
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot]={clip,0.0,gain,pitch,0.f,true,loop};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::controlsSourceRate(std::size_t slot,float gain,float pitch,
    std::int32_t engineAngle) noexcept {
    float left=1.f,right=1.f;
    if(slot>=count_||!nativeWetVoiceAdmitted(slot)||!validSourceRate(voices_[slot].clip,rate_,gain,pitch)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       (!soundMode50KG_&&engineAngle!=0)||
       (soundMode50KG_&&!originalRouting50KG(engineAngle,left,right)))return false;
    auto& voice=voices_[slot];
    if(voice.gain!=gain||voice.pitch!=pitch||voice.pan!=0.f||
       originalLeft50KG_[slot]!=left||originalRight50KG_[slot]!=right)noteControlChange();
    voice.gain=gain;voice.pitch=pitch;voice.pan=0.f;
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::playSourceVolume(std::size_t slot,const AudioClip50HD& clip,
    std::int32_t volume,float pitch,bool loop,std::int32_t engineAngle) noexcept {
    return playSourceVolumeWithNativeKind(slot,clip,volume,pitch,loop,engineAngle,nativeDefaultVoiceKind(slot));
}
bool AudioMixer50HD::playGarageSourceVolume(std::size_t slot,const AudioClip50HD& clip,
    std::int32_t volume,float pitch,bool stream)noexcept {
    const auto& c=nativeWattoRouting_;
    if(!c.command||slot<c.firstVoice||slot-c.firstVoice>=c.voiceCount)return false;
    return playSourceVolumeWithNativeKind(slot,clip,volume,pitch,false,0,
        stream?NativeVoiceKind::stream:NativeVoiceKind::sample);
}
bool AudioMixer50HD::playSourceVolumeWithNativeKind(std::size_t slot,const AudioClip50HD& clip,
    std::int32_t volume,float pitch,bool loop,std::int32_t engineAngle,NativeVoiceKind kind) noexcept {
    float left=0.f,right=0.f;
    if(slot>=count_||!nativeWetVoiceAdmitted(slot)||!validSourceRate(clip,rate_,1.f,pitch,true)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right,volume))return false;
    NativeSourceVoiceRoutes50KG pool;NativeVoiceRoute route;
    if(!prepareNativeVoiceRoute(slot,clip,kind,pool,route))return false;
    if(nativeSourceWetProcessor_){nativeSourceRoutes_=pool;nativeSourceVoiceRoutes_[slot]=route;}
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot]={clip,0.0,1.f,pitch,0.f,true,loop};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::playNativeSourceOrdinarySample(const OriginalApplicationAural50KG& owner,
    std::size_t slot,const AudioClip50HD& clip,float gain,float pitch,
    const NativeSourceSampleStart50KG& source)noexcept {
    float left=0.f,right=0.f;
    if(slot<1||slot>6||slot>=count_||!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(owner,*nativeSourceWetProcessor_)||
       !nativeWetVoiceAdmitted(slot)||!canRecordControlChanges(1)||
       !validClip(clip)||!validControls(gain,pitch,0.f)||source.stream1A8||
       source.stereo1AC!=(clip.channels==2)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||!originalRouting50KG(0,left,right))return false;
    NativeSourceVoiceRoutes50KG pool;NativeVoiceRoute route;
    if(!prepareNativeVoiceRoute(slot,clip,NativeVoiceKind::sample,pool,route)||
       !pool.bindSamplePublication266DA0(route,source.duration20,source.loop1B0))return false;
    nativeSourceRoutes_=pool;nativeSourceVoiceRoutes_[slot]=route;
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot]={clip,0.0,gain,pitch,0.f,true,source.loop1B0};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::playNativeSourceOrdinaryStream(const OriginalApplicationAural50KG& owner,
    std::size_t slot,const AudioClip50HD& clip,float gain,float pitch,
    const NativeSourceStreamStart50KG& source)noexcept {
    float left=0.f,right=0.f;
    if((slot!=0&&slot!=7)||slot>=count_||source.count!=clip.channels||!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(owner,*nativeSourceWetProcessor_)||
       !nativeWetVoiceAdmitted(slot)||!canRecordControlChanges(1)||
       !validClip(clip)||!validControls(gain,pitch,0.f)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||!originalRouting50KG(0,left,right))return false;
    NativeSourceVoiceRoutes50KG pool;NativeVoiceRoute route;
    if(!prepareNativeVoiceRoute(slot,clip,NativeVoiceKind::stream,pool,route)||
       !pool.bindStreamPublication266DA0(route,source.duration,source.loop,
           source.slots,source.generations,source.count))return false;
    nativeSourceRoutes_=pool;nativeSourceVoiceRoutes_[slot]=route;
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot]={clip,0.0,gain,pitch,0.f,true,source.loop};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::playNativeSourceStreamVolume(const void* command,
    const OriginalApplicationAural50KG& owner,std::size_t slot,const AudioClip50HD& clip,
    std::int32_t volume,float pitch,std::int32_t engineAngle,
    const NativeSourceStreamStart50KG& source)noexcept {
    float left=0.f,right=0.f;
    const auto& claim=nativeWattoRouting_;
    if(!command||claim.command!=command||!nativeGarageRoutingLiveCurrent(claim,command)||
       slot>=count_||slot<claim.firstVoice||slot-claim.firstVoice>=claim.voiceCount||
       source.count!=clip.channels||!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(owner,*nativeSourceWetProcessor_)||!canRecordControlChanges(1)||
       !validSourceRate(clip,rate_,1.f,pitch,true)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right,volume))return false;
    NativeSourceVoiceRoutes50KG pool;NativeVoiceRoute route;
    if(!prepareNativeVoiceRoute(slot,clip,NativeVoiceKind::stream,pool,route)||
       !pool.bindStreamPublication266DA0(route,source.duration,source.loop,
           source.slots,source.generations,source.count))return false;
    nativeSourceRoutes_=pool;nativeSourceVoiceRoutes_[slot]=route;
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot]={clip,0.0,1.f,pitch,0.f,true,source.loop};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::playNativeSourceSampleVolume(const void* command,std::size_t slot,
    const AudioClip50HD& clip,std::int32_t volume,float pitch,std::int32_t engineAngle,
    const NativeSourceSampleStart50KG& source)noexcept {
    // The authentic caller has already retained/validated nativeSample28 and
    // its strong PCM lease. This cannot admit a stream or invent sample IDs.
    const NativeGarageRoutingClaim* claim=nullptr;
    if(command&&nativeWattoRouting_.command==command)claim=&nativeWattoRouting_;
    else if(command&&nativeSceneRouting_.command==command)claim=&nativeSceneRouting_;
    return claim&&nativeGarageRoutingLiveCurrent(*claim,command)&&
        publishNativeSourceSampleVolume(*claim,slot,clip,volume,pitch,engineAngle,source);
}
bool AudioMixer50HD::playNativeSceneImmediateSampleVolume(const void* command,std::size_t slot,
    const AudioClip50HD& clip,std::int32_t volume,float pitch,std::int32_t engineAngle,
    const NativeSourceSampleStart50KG& source)noexcept {
    // Only Main's real retained Droid Sound callback issues this private path.
    // Preserve the original immediate flag2, not an invented deferred voice.
    return nativeSceneImmediateRoutingCurrent(command)&&!source.stream1A8&&
        !source.stereo1AC&&!source.loop1B0&&source.duration20>0.f&&
        publishNativeSourceSampleVolume(nativeSceneRouting_,slot,clip,volume,pitch,engineAngle,source);
}
bool AudioMixer50HD::publishNativeSourceSampleVolume(const NativeGarageRoutingClaim& claim,std::size_t slot,
    const AudioClip50HD& clip,std::int32_t volume,float pitch,std::int32_t engineAngle,
    const NativeSourceSampleStart50KG& source)noexcept {
    // Both private wrappers authenticate their exact claim immediately above;
    // no callback or mutation separates that proof from this common publisher.
    float left=0.f,right=0.f;
    if(slot>=count_||slot<claim.firstVoice||slot-claim.firstVoice>=claim.voiceCount||
       !canRecordControlChanges(1)||source.stream1A8||
       source.stereo1AC!=(clip.channels==2)||!std::isfinite(source.duration20)||source.duration20<0.f||
       !validSourceRate(clip,rate_,1.f,pitch,true)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right,volume))return false;
    NativeSourceVoiceRoutes50KG pool;NativeVoiceRoute route;
    if(!prepareNativeVoiceRoute(slot,clip,NativeVoiceKind::sample,pool,route)||
       !pool.bindSamplePublication266DA0(route,source.duration20,source.loop1B0))return false;
    // Publish source reservation, metadata and native PCM together only after
    // all checks. No source factor is fabricated from this play or DSP time.
    nativeSourceRoutes_=pool;nativeSourceVoiceRoutes_[slot]=route;
    noteControlChange();playbackRevisions_[slot]=controlRevision_;
    voices_[slot]={clip,0.0,1.f,pitch,0.f,true,source.loop1B0};
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::controlsSourceVolume(std::size_t slot,std::int32_t volume,
    float pitch,std::int32_t engineAngle) noexcept {
    float left=0.f,right=0.f;
    if(slot>=count_||!nativeWetVoiceAdmitted(slot)||!validSourceRate(voices_[slot].clip,rate_,1.f,pitch,true)||
       (requireGroups50IC_&&!groupKnown50IC_[slot])||
       !originalRouting50KG(engineAngle,left,right,volume))return false;
    auto& voice=voices_[slot];
    if(voice.gain!=1.f||voice.pitch!=pitch||voice.pan!=0.f||
       originalLeft50KG_[slot]!=left||originalRight50KG_[slot]!=right)noteControlChange();
    voice.gain=1.f;voice.pitch=pitch;voice.pan=0.f;
    originalLeft50KG_[slot]=left;originalRight50KG_[slot]=right;return true;
}
bool AudioMixer50HD::setLoop(std::size_t slot, bool loop) noexcept {
    if (slot >= count_ || !nativeWetVoiceAdmitted(slot)) return false;
    if(voices_[slot].loop!=loop)noteControlChange();
    voices_[slot].loop = loop;
    return true;
}
bool AudioMixer50HD::setMasterGain(float gain) noexcept {
    if (!std::isfinite(gain) || gain < 0.0f || gain > 4.0f) return false;
    if(masterGain_!=gain)noteControlChange();
    masterGain_ = gain; return true;
}
bool AudioMixer50HD::setVoiceGroupGain50IC(std::size_t slot, float gain) noexcept {
    if (slot >= count_ || (!nativeWetVoiceAdmitted(slot)&&!nativeRaceIntroClaimed(slot)) || !std::isfinite(gain) || gain < 0.0f || gain > 1.0f) return false;
    if(!groupKnown50IC_[slot]||groupGains50IC_[slot]!=gain)noteControlChange();
    groupGains50IC_[slot] = gain; groupKnown50IC_[slot] = true; return true;
}
bool AudioMixer50HD::setVoiceGroupPaused50IC(std::size_t slot,bool paused) noexcept {
    if(slot>=count_||(!nativeWetVoiceAdmitted(slot)&&!nativeRaceIntroClaimed(slot))||!groupKnown50IC_[slot])return false;
    if(groupPaused50IC_[slot]!=paused)noteControlChange();
    groupPaused50IC_[slot]=paused;return true;
}
bool AudioMixer50HD::hasVoiceGroupGain50IC(std::size_t slot) const noexcept {
    return slot < count_ && groupKnown50IC_[slot];
}
bool AudioMixer50HD::retireStoppedVoiceGroup50IC(std::size_t slot) noexcept {
    if(slot>=count_||!nativeWetVoiceAdmitted(slot)||voices_[slot].active||!groupKnown50IC_[slot])return false;
    noteControlChange();
    groupKnown50IC_[slot]=false;groupGains50IC_[slot]=0.f;groupPaused50IC_[slot]=false;
    return true;
}
bool AudioMixer50HD::requireVoiceGroupGains50IC() noexcept {
    if (!voices_ || !count_) return false;
    for (std::size_t i = 0; i < count_; ++i)
        if (voices_[i].active && !groupKnown50IC_[i]) return false;
    if(!requireGroups50IC_)noteControlChange();
    requireGroups50IC_ = true; return true;
}
void AudioMixer50HD::stop(std::size_t slot) noexcept {
    if(nativeRaceIntroClaimed(slot))return; // Exact lease performs its own typed source stop.
    if(slot<count_&&(voices_[slot].active||nativeSourceVoiceRoutes_[slot].count)){
        retireNativeVoiceRoute(slot);noteControlChange();voices_[slot].active=false;
    }
}
void AudioMixer50HD::stopAll() noexcept {
    bool changed=false;
    for (std::size_t i = 0; i < count_; ++i) {
        if(nativeRaceIntroClaimed(i))continue;
        const bool retainedRoute=nativeSourceVoiceRoutes_[i].count!=0;
        if(voices_[i].active||retainedRoute)retireNativeVoiceRoute(i);
        changed=changed||voices_[i].active||retainedRoute;voices_[i].active=false;
    }
    if(changed)noteControlChange();
}
bool AudioMixer50HD::active(std::size_t slot) const noexcept {
    return slot < count_ && voices_[slot].active;
}
bool AudioMixer50HD::inspectVoice(std::size_t slot, AudioVoiceObservation50HD& out) const noexcept {
    out={};
    if(!voices_||slot>=count_||!canRecordControlChanges())return false;
    out.voice=voices_[slot];out.playbackRevision=playbackRevisions_[slot];
    out.groupGain=groupGains50IC_[slot];out.groupKnown=groupKnown50IC_[slot];
    out.groupPaused=groupPaused50IC_[slot];
    out.originalLeft=originalLeft50KG_[slot];out.originalRight=originalRight50KG_[slot];
    return true;
}
std::size_t AudioMixer50HD::availableVoice(std::size_t first, std::size_t count) const noexcept {
    if (first >= count_) return count_;
    const auto end = first + std::min(count, count_ - first);
    for (auto i = first; i < end; ++i) if (!voices_[i].active&&nativeWetVoiceAdmitted(i)) return i;
    return count_;
}
bool AudioMixer50HD::mixStereo16(std::int16_t* output, std::size_t frames) noexcept {
    if (!voices_ || !rate_ || frames > rate_ || (!output && frames) ||
        frames > std::numeric_limits<std::uint64_t>::max() - mixedFrames_) return false;
    NativeSourceReverb50KG::Frame effectInput{},effectOutput{};
    if(nativeSourceWetProcessor_){
        if(!auralMasterOwner_||!nativeSourceWetProcessorCurrent(*auralMasterOwner_,*nativeSourceWetProcessor_))return false;
        // Preflight every actual source allocation before output, history or
        // cursor mutation. A logical voice index is not a source core/channel.
        for(std::size_t i=0;i<count_;++i)if(voices_[i].active){
            const auto& route=nativeSourceVoiceRoutes_[i];
            if(!nativeSourceRoutes_.current(route)||route.count!=voices_[i].clip.channels)return false;
        }
        if(frames&&!(nativeSourceWetProcessor_->*nativeWetPreflight_)(&effectInput,1,&effectOutput,1,1,mixedFrames_))return false;
    }
    // Generic claimed-range mutation remains closed. The distinct Scene and
    // Watto render proofs accept only actual private source reservations with
    // retained PCM and exact command claims. Watto requires applied25; only
    // Scene's immediate path may use its genuine prior persistent routing byte.
    for(std::size_t i=0;i<count_;++i)if(voices_[i].active&&!nativeWetVoiceAdmitted(i)&&
        !nativeSceneSampleVoiceCurrent(i)&&!nativeWattoSourceVoiceCurrent(i)&&
        !(nativeRaceIntroClaimed(i)&&nativeRaceIntroRenderCurrent_&&
          nativeRaceIntroRenderCurrent_(nativeRaceIntroTransport_,i)))return false;
    for (std::size_t f = 0; f < frames; ++f) {
        double left = 0.0, right = 0.0;
        std::array<double,2> wetLeft{},wetRight{};
        for (std::size_t i = 0; i < count_; ++i) {
            auto& voice = voices_[i];
            if (!voice.active) continue;
            // Original group pause is distinct from volume ducking: preserve
            // cursor, loop boundary, active token and pitch while output is silent.
            if (groupPaused50IC_[i]) continue;
            // Zero-rate native source-volume policy: do not sample, advance,
            // retire, or replace the token; later positive controls resume.
            if (voice.pitch==0.f) continue;
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
                // Decoder EOF is not267CB0's source duration+margin expiry.
                // Retain the source reservation until its real update or an
                // explicit source/native lifetime stop discharges it.
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
            // Source master gain is an additional real PCM consumer, not a
            // queued-control receipt or a claim of wet DSP implementation.
            const float sourceMaster=auralMasterOwner_?auralMasterGain_[auralMasterBus_[i]]:1.f;
            const auto voiceLeft=sampleLeft * voice.gain * groupGains50IC_[i] * panLeft * sourceMaster;
            const auto voiceRight=sampleRight * voice.gain * groupGains50IC_[i] * panRight * sourceMaster;
            left+=voiceLeft;right+=voiceRight;
            if(nativeSourceWetProcessor_&&nativeVoiceWetSend(i)){
                const auto& route=nativeSourceVoiceRoutes_[i];
                wetLeft[route.cells[0]/24]+=voiceLeft;
                wetRight[route.cells[route.count==2?1:0]/24]+=voiceRight;
            }
            voice.sourceFrame += static_cast<double>(clip.sampleRate) / rate_ * voice.pitch;
            if (voice.loop && voice.sourceFrame >= static_cast<double>(loopEnd))
                voice.sourceFrame = static_cast<double>(loopStart) +
                    std::fmod(voice.sourceFrame - static_cast<double>(loopStart),
                              static_cast<double>(loopEnd - loopStart));
            else if (!voice.loop && voice.sourceFrame >= static_cast<double>(clip.frames)){
                voice.active = false;
            }
        }
        if(nativeSourceWetProcessor_){
            // Existing native PCM interpolation/mixing remains the input model;
            // each source core is summed/saturated separately before its own
            // retained processor. This is not hardware-bit-exact SPU mixing.
            std::uint64_t intermediateClips=0;
            for(unsigned c=0;c<2;++c){effectInput.core[c].left=quantize(wetLeft[c],intermediateClips);
                effectInput.core[c].right=quantize(wetRight[c],intermediateClips);}
            if(!(nativeSourceWetProcessor_->*nativeWetProcess_)(&effectInput,1,&effectOutput,1,1,mixedFrames_+f))std::terminate();
            for(const auto& core:effectOutput.core){left+=core.left;right+=core.right;}
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

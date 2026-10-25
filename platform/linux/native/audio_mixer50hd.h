#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <array>
#include "native_source_voice_routes50kg.h"
#include "native_source_reverb50kg.h"

namespace rr::game {struct OriginalWreckComponent50LA;}
namespace rr::platform {
class NativeRaceIntroTransport50LA;
class OriginalSoundMode50KG;
class OriginalApplicationAural50KG;
class NativeSourceReverb50KG;
class OriginalUiGarageWattoAudio50KG;
class OriginalUiGarageDroidAudio50GN;
class RaceAudioGroups50IC;
class OriginalUiMainAudio50KG;
class OriginalUiGarageWattoSoundGroup50KG;
class OriginalUiGaragePodScene50KG;
// Existing0..219 reservations plus the eight distinct original D44 arrays.
// Each admitted track has at most44 collision+drive rows (SA32+12). These
// are native exclusive playback slots, never guest hardware channel IDs.
inline constexpr std::size_t originalMaterialFirstVoice50HD=220;
inline constexpr std::size_t originalMaterialRowsPerRacer50HD=44;
inline constexpr std::size_t originalMaterialRacers50HD=8;
inline constexpr std::size_t originalSpeechFirstVoice50HD=originalMaterialFirstVoice50HD+
    originalMaterialRowsPerRacer50HD*originalMaterialRacers50HD;
// Largest authenticated D48 bank is POD19:24 curses+12 taunts+11 specials.
// All eight racers retain distinct logical voices, including the followed AI.
inline constexpr std::size_t originalSpeechVoicesPerRacer50HD=47;
inline constexpr std::size_t originalSpeechRacers50HD=8;
inline constexpr std::size_t originalTaIntroFirstVoice50HD=originalSpeechFirstVoice50HD+
    originalSpeechVoicesPerRacer50HD*originalSpeechRacers50HD;
inline constexpr std::size_t originalTaIntroVoices50HD=3;
inline constexpr std::size_t audioMaximumVoices50HD=originalTaIntroFirstVoice50HD+originalTaIntroVoices50HD;

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
    bool groupPaused = false;
};

// Single producer/main-thread mixer. It never allocates, loads assets, opens a
// device, synthesizes a substitute effect, or touches the SDL callback thread.
class AudioMixer50HD final {
public:
    AudioMixer50HD()=default;
    ~AudioMixer50HD()noexcept;
    //Retained Main/voice/control identities require this stable native owner.
    //Copy/assignment must not bypass initialize's live-routing refusal.
    AudioMixer50HD(const AudioMixer50HD&)=delete;
    AudioMixer50HD& operator=(const AudioMixer50HD&)=delete;
    AudioMixer50HD(AudioMixer50HD&&)=delete;
    AudioMixer50HD& operator=(AudioMixer50HD&&)=delete;
    enum class NativeGarageRoutingKind : std::uint8_t { watto, scene };
    struct NativeGarageRoutingObservation {
        NativeGarageRoutingKind kind=NativeGarageRoutingKind::watto;
        const OriginalUiGaragePodScene50KG* scene=nullptr;
        const OriginalUiMainAudio50KG* main=nullptr;
        const OriginalUiGarageWattoSoundGroup50KG* group=nullptr;
        const game::OriginalWreckComponent50LA* entity=nullptr;
        std::uint64_t sequence=0,generation=0,objectKey=0,epoch=0;
        std::size_t firstVoice=0,voiceCount=0;
        std::int32_t sourceGroupSlot=-1;
        std::uint8_t persistentRoutingByte=0;
        bool queued=false,routingApplied=false,neverPlayed=false,wetOutputSupported=false;
    };
    using NativeWattoRoutingObservation=NativeGarageRoutingObservation;
    using NativeSceneRoutingObservation=NativeGarageRoutingObservation;
    bool inspectNativeWattoRouting(NativeWattoRoutingObservation&)const noexcept;
    bool inspectNativeSceneRouting(NativeSceneRoutingObservation&)const noexcept;
    bool inspectPersistentWetRoutingByte(std::size_t,std::uint8_t&)const noexcept;
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
    // Source-owned rates can be below the legacy .125 presentation limit.
    // Positive finite rates are admitted only if double source coordinates
    // keep advancing throughout this actual clip and remain in the precise
    // integer-index domain. Existing clients keep their legacy controls.
    // Centered original-mode routing/group ownership are still enforced.
    bool playSourceRate(std::size_t slot,const AudioClip50HD&,float gain,
                        float pitch,bool loop,std::int32_t engineAngle=0) noexcept;
    bool controlsSourceRate(std::size_t slot,float gain,float pitch,
                            std::int32_t engineAngle=0) noexcept;
    // Actual integer source volume enters26CBA0 BEFORE directional integer
    // division. Requires the real mode owner. The resulting L/R percentages
    // include volume; voice.gain is unity, so volume is never applied twice.
    // Legacy gain APIs above retain their existing normalization policy.
    // Native source-volume rendering admits normalized rate0 as silent paused
    // playback: token/active status/cursor survive until positive controls or
    // stop. This is a native policy, not emulated IOP pitch-zero behavior.
    bool playSourceVolume(std::size_t slot,const AudioClip50HD&,std::int32_t volume,
                          float pitch,bool loop,std::int32_t engineAngle) noexcept;
    bool controlsSourceVolume(std::size_t slot,std::int32_t volume,float pitch,
                              std::int32_t engineAngle) noexcept;
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
    friend class RaceAudioGroups50IC;
    friend class OriginalApplicationAural50KG;
    friend class NativeRaceIntroTransport50LA;
    const NativeRaceIntroTransport50LA* nativeRaceIntroTransport_=nullptr;
    bool (*nativeRaceIntroRenderCurrent_)(const void*,std::size_t)noexcept=nullptr;
    std::size_t nativeRaceIntroFirst_=0,nativeRaceIntroCount_=0;
    bool nativeRaceIntroClaimed(std::size_t slot)const noexcept {
        return nativeRaceIntroTransport_&&slot>=nativeRaceIntroFirst_&&
            slot-nativeRaceIntroFirst_<nativeRaceIntroCount_;
    }
    friend class OriginalUiGarageWattoAudio50KG;
    friend class OriginalUiGarageDroidAudio50GN;
    bool nativeSourceVoiceReserved(std::size_t,std::uint64_t)const noexcept;
    struct NativeSourceSampleStart50KG {
        float duration20=0.f;
        bool loop1B0=false,stream1A8=false,stereo1AC=false;
    };
    // Aural issues these native slot generations after actual stream-slot
    // allocation. They are not guest sample integers or physical addresses.
    struct NativeSourceStreamStart50KG {
        std::array<std::uint8_t,2> slots{};
        std::array<std::uint64_t,2> generations{};
        std::uint8_t count=0;
        float duration=0.f;
        bool loop=false;
    };
    bool playNativeSourceOrdinarySample(const OriginalApplicationAural50KG&,std::size_t,
        const AudioClip50HD&,float gain,float pitch,const NativeSourceSampleStart50KG&)noexcept;
    bool playNativeSourceOrdinaryStream(const OriginalApplicationAural50KG&,std::size_t,
        const AudioClip50HD&,float gain,float pitch,const NativeSourceStreamStart50KG&)noexcept;
    bool playNativeSourceStreamVolume(const void*,const OriginalApplicationAural50KG&,std::size_t,
        const AudioClip50HD&,std::int32_t volume,float pitch,std::int32_t angle,
        const NativeSourceStreamStart50KG&)noexcept;
    bool canReserveNativeSourceMonoVoices(std::size_t count)const noexcept;
    bool nativeSourceStreamReferences267CB0(const OriginalApplicationAural50KG&,float,
        bool afterService,std::array<std::uint64_t,8>&)const noexcept;
    // Exact source-owner caller only; public play methods remain closed for
    // claimed ranges. Typed cache/PCM proof stays with the actual Sound owner.
    bool playNativeSourceSampleVolume(const void* command,std::size_t slot,
        const AudioClip50HD&,std::int32_t volume,float pitch,std::int32_t engineAngle,
        const NativeSourceSampleStart50KG&)noexcept;
    bool playNativeSceneImmediateSampleVolume(const void* command,std::size_t slot,
        const AudioClip50HD&,std::int32_t volume,float pitch,std::int32_t engineAngle,
        const NativeSourceSampleStart50KG&)noexcept;

    // Native dry master buses for source SoundInterface232BD0/232C20. They
    // remain distinct from dynamic SoundGroup gains and the later UI music40.
    // Only the exact retained Aural interface may bind/release this cold lease.
    const OriginalApplicationAural50KG* auralMasterOwner_=nullptr;
    std::array<float,2> auralMasterGain_{{1.f,1.f}};
    std::array<std::uint8_t,audioMaximumVoices50HD> auralMasterBus_{};
    friend class OriginalUiMainAudio50KG;
    bool canRetireStoppedNativeGarageClips(const OriginalUiMainAudio50KG&,
        const AudioVoiceObservation50HD*,std::size_t)const noexcept;
    void retireStoppedNativeGarageClipsPreflighted()noexcept;
    //A backend binding of the already-counted source group, NOT a second
    //group registry. Main alone supplies actual identity and native range.
    struct NativeGarageRoutingClaim {
        NativeGarageRoutingKind kind=NativeGarageRoutingKind::watto;
        const void* command=nullptr;
        const OriginalUiMainAudio50KG* main=nullptr;
        const OriginalUiGarageWattoSoundGroup50KG* group=nullptr;
        const OriginalUiGaragePodScene50KG* scene=nullptr;
        const game::OriginalWreckComponent50LA* entity=nullptr;
        const AudioVoice50HD* storage=nullptr;
        std::uint64_t sequence=0,generation=0,objectKey=0,epoch=0;
        std::size_t firstVoice=0,voiceCount=0;
        std::int32_t sourceGroupSlot=-1;
        bool routingApplied=false;
    };
    // Two source-specific entry types share one native range/consumer policy.
    // They bind already-counted Groups, never allocate source registry slots.
    struct NativeWattoRoutingClaim final : NativeGarageRoutingClaim {};
    struct NativeSceneRoutingClaim final : NativeGarageRoutingClaim {
        NativeSceneRoutingClaim()noexcept {kind=NativeGarageRoutingKind::scene;}
    };
    bool canClaimNativeGarageRouting(const NativeGarageRoutingClaim&)const noexcept;
    bool retainedNativeGarageRange(const NativeGarageRoutingClaim&)const noexcept;
    bool claimRetainedNativeGarageRouting(const NativeGarageRoutingClaim&)noexcept;
    bool applyRetainedNativeGarageRouting(NativeGarageRoutingClaim&,const void*)noexcept;
    bool coldNativeGarageRange(const NativeGarageRoutingClaim&)const noexcept;
    bool nativeGarageRoutingCurrent(const NativeGarageRoutingClaim&,const void*,bool)const noexcept;
    bool applyColdNativeGarageRouting(NativeGarageRoutingClaim&,const void*)noexcept;
    bool nativeGarageRoutingLiveCurrent(const NativeGarageRoutingClaim&,const void*)const noexcept;
    bool canRetireNativeGarageRouting(const NativeGarageRoutingClaim&,const void*)const noexcept;
    bool inspectNativeGarageRouting(const NativeGarageRoutingClaim&,NativeGarageRoutingObservation&)const noexcept;
    bool canClaimNativeWattoRouting(const NativeWattoRoutingClaim&)const noexcept;
    bool claimNativeWattoRouting(const NativeWattoRoutingClaim&)noexcept;
    bool claimRetainedNativeWattoRouting(const NativeWattoRoutingClaim&)noexcept;
    bool applyRetainedNativeWattoRouting(const void*)noexcept;
    bool nativeRetainedWattoRoutingCurrent(const void*,bool)const noexcept;
    bool nativeWattoRoutingCurrent(const void*,bool applied)const noexcept;
    bool applyColdNativeWattoRouting(const void*)noexcept;
    bool canRetireColdNativeWattoRouting(const void*)const noexcept;
    bool retireColdNativeWattoRouting(const void*)noexcept;
    bool nativeWattoRoutingLiveCurrent(const void*)const noexcept;
    bool canRetireNativeWattoRouting(const void*)const noexcept;
    bool retireNativeWattoRouting(const void*)noexcept;
    bool claimNativeSceneRouting(const NativeSceneRoutingClaim&)noexcept;
    bool claimRetainedNativeSceneRouting(const NativeSceneRoutingClaim&)noexcept;
    bool applyRetainedNativeSceneRouting(const void*)noexcept;
    bool nativeRetainedSceneRoutingCurrent(const void*,bool)const noexcept;
    bool nativeSceneRoutingCurrent(const void*,bool applied)const noexcept;
    bool applyColdNativeSceneRouting(const void*)noexcept;
    // Real233410(flags2)/26B5E0 immediate sample ownership. Pending25 does
    // not erase the source slot's already-applied persistent routing byte.
    bool nativeSceneImmediateRoutingCurrent(const void*)const noexcept;
    bool nativeSceneSampleVoiceCurrent(std::size_t)const noexcept;
    bool nativeWattoSourceVoiceCurrent(std::size_t)const noexcept;
    bool applyNativeSceneImmediateRouting(const void*)noexcept;
    bool publishNativeSourceSampleVolume(const NativeGarageRoutingClaim&,std::size_t,
        const AudioClip50HD&,std::int32_t,float,std::int32_t,
        const NativeSourceSampleStart50KG&)noexcept;
    // Main authenticates the actual pending source Sound and native row before
    // this once-only initial binding. This is not wet routing via gain/pause.
    bool publishColdNativeSceneVoiceGroup50IC(const void*,std::size_t,float)noexcept;
    bool publishRetainedNativeSceneVoiceGroup50IC(const void*,std::size_t,float)noexcept;
    bool canRetireColdNativeSceneRouting(const void*)const noexcept;
    bool retireColdNativeSceneRouting(const void*)noexcept;
    bool nativeSceneRoutingLiveCurrent(const void*)const noexcept;
    bool canRetireNativeSceneRouting(const void*)const noexcept;
    bool retireNativeSceneRouting(const void*)noexcept;
    // Exact retired four-Sound binding only. Preserves PCM/voice revisions,
    // every other voice, routing history, clock, bus gains and mixer epoch.
    bool canRecycleRetiredColdSceneBindings()const noexcept;
    bool recycleRetiredColdSceneBindings()noexcept;
    bool canRecycleRetiredSceneBindings()const noexcept;
    bool recycleRetiredSceneBindings()noexcept;
    bool nativeWetVoiceAdmitted(std::size_t)const noexcept;
    // Same native Aural lifetime, not one processor per menu/group. These
    // private cold/retirement joins do not grant per-voice routing readiness.
    bool bindNativeSourceWetProcessor(const OriginalApplicationAural50KG&,
        NativeSourceReverb50KG&,decltype(&NativeSourceReverb50KG::preflight),
        decltype(&NativeSourceReverb50KG::process)) noexcept;
    bool nativeSourceWetProcessorCurrent(const OriginalApplicationAural50KG&,
        const NativeSourceReverb50KG&) const noexcept;
    bool releaseNativeSourceWetProcessor(const OriginalApplicationAural50KG&,
        NativeSourceReverb50KG&) noexcept;
    bool canServiceNativeSourceVoices267CB0(const OriginalApplicationAural50KG&,float)const noexcept;
    bool serviceNativeSourceVoices267CB0(const OriginalApplicationAural50KG&,float)noexcept;
    using NativeVoiceKind=NativeSourceVoiceRoutes50KG::Kind;
    using NativeVoiceRoute=NativeSourceVoiceRoutes50KG::Route;
    NativeVoiceKind nativeDefaultVoiceKind(std::size_t)const noexcept;
    bool prepareNativeVoiceRoute(std::size_t,const AudioClip50HD&,NativeVoiceKind,
        NativeSourceVoiceRoutes50KG&,NativeVoiceRoute&)const noexcept;
    void retireNativeVoiceRoute(std::size_t)noexcept;
    bool nativeVoiceWetSend(std::size_t)const noexcept;
    bool playGarageSourceVolume(std::size_t,const AudioClip50HD&,std::int32_t,float,bool)noexcept;
    bool playSourceVolumeWithNativeKind(std::size_t,const AudioClip50HD&,std::int32_t,
        float,bool,std::int32_t,NativeVoiceKind)noexcept;
    NativeSourceReverb50KG* nativeSourceWetProcessor_=nullptr;
    decltype(&NativeSourceReverb50KG::preflight) nativeWetPreflight_=nullptr;
    decltype(&NativeSourceReverb50KG::process) nativeWetProcess_=nullptr;
    std::uint64_t nativeSourceWetEpoch_=0;
    NativeSourceVoiceRoutes50KG nativeSourceRoutes_;
    std::array<NativeSourceVoiceRoutes50KG::Route,audioMaximumVoices50HD> nativeSourceVoiceRoutes_{};

    NativeGarageRoutingClaim nativeWattoRouting_{},nativeSceneRouting_{};
    //IRX module entry clears these25 bytes. Neither Init6E80 nor235150
    //clears them; preserve across initialize and source owner retirement.
    std::array<std::uint8_t,25> persistentWetRouting50KG_{};
    // Actual group owner only; independent of gain, pitch and playback token.
    bool setVoiceGroupPaused50IC(std::size_t slot,bool paused) noexcept;
    bool retireStoppedVoiceGroup50IC(std::size_t slot) noexcept;
    void noteControlChange() noexcept;
    bool originalRouting50KG(std::int32_t engineAngle,float& left,float& right,
                            std::int32_t volume=100) const noexcept;
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
    bool groupPaused50IC_[audioMaximumVoices50HD]{};
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

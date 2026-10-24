#pragma once
#include "audio_mixer50hd.h"
#include "native_source_reverb50kg.h"
#include "audio_stereo_producer50hd.h"
#include "original_ui_garage_watto_audio50kg.h"
#include "original_ui_garage_droid_audio50gn.h"
#include "../../../src/assets/audio_assets50hc.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>

namespace original_main_entry50KG { struct Application; }
namespace rr::game { struct OriginalWreckComponent50LA; }
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
class OriginalUiGaragePageOwner50KG;
class NativeSourceReverb50KG;
class OriginalApplicationAural50KG;
class OriginalUiGarageAudioBacking50KG;
class OriginalUiGarageSession50KG;
// Host-only frontend transport. The application supplies a DEDICATED freshly
// initialized mixer/voice array, never the live race mixer or its slot117.
// The ordinary standalone transport resets before race output begins. A Root-
// retained transport instead requires real device-close and ordinary-voice
// retirement before race output, retaining PCM/cursors/clock until terminal
// source retirement. Mixer/storage outlive this stable owner. Main-thread only.
class OriginalUiMainAudio50KG final : public AudioStereoProducer50HD {
    // Only the one private all-owner Load coordinator may apply the already
    // prevalidated188740 fields/revisions inside its nonfailing store interval.
    friend class OriginalUiRiderProfileOwner50KG;
    friend class OriginalAudioSampleCache50KG;
    friend class OriginalAudioPendingWaves50KG;
    friend bool AudioMixer50HD::canRetireStoppedNativeGarageClips(
        const OriginalUiMainAudio50KG&,const AudioVoiceObservation50HD*,std::size_t)const noexcept;
    friend class OriginalUiGarageWattoSoundGroup50KG;
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageDroidAudio50GN;
    friend class OriginalUiGarageWattoAudio50KG;
    friend class OriginalUiGaragePageOwner50KG;
    // Native ordered command custody, not a guest RPC buffer or applied DSP.
    //The source26D6C0 queue holds64 commands. A full native queue stays closed
    //outside the two typed Garage cold consumers; no eviction or synthetic flush.
    struct QueuedGroupControl;
    class ReverbEnqueueReceipt final {
        friend class OriginalUiMainAudio50KG;
        std::weak_ptr<const QueuedGroupControl> command_;
        std::uint64_t sequence_=0;
    };
    bool enqueueWattoReverb26D6C0(const std::shared_ptr<OriginalUiGarageWattoSoundGroup50KG>&,
        ReverbEnqueueReceipt&,std::string&)noexcept;
    bool queuedWattoReverbCurrent(const OriginalUiGarageWattoSoundGroup50KG&,const ReverbEnqueueReceipt&)const noexcept;
    bool mayRetireQueuedWattoGroup(const OriginalUiGarageWattoSoundGroup50KG&)const noexcept;
    //Synchronous native cold service. This is not an invented source flush PC
    //and does not grant wet-output, playback, or Page readiness.
    bool serviceColdGarageRouting(std::string&)noexcept;
    bool releaseRetiredColdWattoRouting(OriginalUiGarageWattoSoundGroup50KG&)noexcept;
    bool queuedGroupIdentityCurrent(const QueuedGroupControl&)const noexcept;
    bool queuedGroupRoutingCurrent(const QueuedGroupControl&)const noexcept;
    bool queuedGroupHistoryCurrent(const QueuedGroupControl&)const noexcept;
    bool queuedGroupSourceLifetimeCurrent(const QueuedGroupControl&)const noexcept;
    bool queuedSceneImmediateLifetimeCurrent(const QueuedGroupControl&)const noexcept;
    bool queuedRetiredSceneLifetimeCurrent(const QueuedGroupControl&)const noexcept;
    bool nativeWattoWetLifetimeCurrent(const OriginalUiGarageWattoSoundGroup50KG&)const noexcept;
    bool publishNativeWattoSample(OriginalUiGarageWattoAudio50KG&,std::size_t sound,std::size_t slot,const AudioClip50HD&)noexcept;
    bool publishNativeSceneSample(OriginalUiGarageDroidAudio50GN&,std::size_t ordinal,const AudioClip50HD&)noexcept;
    bool nativeSceneWetLifetimeCurrent(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool liveGarageVoiceRangesCurrent()const noexcept;
    bool ownedStoppedGarageClipHistoryCurrent()const noexcept;
    bool wattoConstructionHistoryCurrent(const OriginalUiGarageWattoSoundGroup50KG&)const noexcept;
    bool sceneConstructionHistoryCurrent(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool releaseRetiredSceneRouting(const std::shared_ptr<OriginalUiGaragePageOwner50KG>&,OriginalUiGaragePodScene50KG&)noexcept;
    bool releaseRetiredWattoRouting(OriginalUiGarageWattoSoundGroup50KG&)noexcept;
    bool precedingGroupControlsCurrent()const noexcept;
    // Aural owns the processor and this Main; these exact non-owning borrows
    // cannot outlive its interface. No ordinary Main target calls a DSP TU.
    const OriginalApplicationAural50KG* nativeWetOwner_=nullptr;
    NativeSourceReverb50KG* nativeWetProcessor_=nullptr;
    decltype(&NativeSourceReverb50KG::setRawMasters) nativeWetSetRawMasters_=nullptr;
    // Exact Aural-issued members, not untyped callbacks. Ordinary Main-only
    // targets retain their old link boundary and their unbound dry behavior.
    using NativeSongPublisher=bool (OriginalApplicationAural50KG::*)(OriginalUiMainAudio50KG&,
        std::uint32_t,const AudioClip50HD&,float,float)const noexcept;
    using NativeEffectPublisher=bool (OriginalApplicationAural50KG::*)(OriginalUiMainAudio50KG&,
        std::size_t,const AudioClip50HD&,float,float)const noexcept;
    NativeSongPublisher nativeWetPlaySong_=nullptr;
    NativeEffectPublisher nativeWetPlayEffect_=nullptr,nativeWetPlayRider_=nullptr;
    using NativeWattoPublisher=bool (OriginalApplicationAural50KG::*)(OriginalUiMainAudio50KG&,
        const void*,std::size_t,std::size_t,const AudioClip50HD&,std::int32_t,float,std::int32_t)const noexcept;
    NativeWattoPublisher nativeWetPlayWatto_=nullptr;
    using NativePlaybackPreflight=bool (OriginalApplicationAural50KG::*)(const OriginalUiMainAudio50KG&,std::size_t)const noexcept;
    NativePlaybackPreflight nativeWetCanPublish_=nullptr;
    bool nativeSourcePlaybackCurrent(std::size_t maximumMonoPlays=0)const noexcept;
    // Native dry-music envelope custody only. Ordered command25 may still
    // be queued; this never authorizes a source voice, wet routing or PCM.
    bool nativeGaragePageControlCustodyCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    // Exact live input-enabled Garage Page only; this is not a second public
    // effect API. Page owns the source action/selection/response cursor.
    bool nativeGaragePageAudioCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool nativeGaragePageInputAudioCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    // Opening preflight includes raw25, optional outgoing stop and Song6
    // start budgets. Execution separately proves the actual raw25 predecessor.
    bool nativeGaragePageSong6StartCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool canStartGaragePageSong6_234900(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool startGaragePageSong6_234900(const OriginalUiGaragePageOwner50KG&)noexcept;
    bool canFadeGaragePageSong6_234440(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool fadeGaragePageSong6_234440(const OriginalUiGaragePageOwner50KG&)noexcept;
    // Garage Root services only this existing native envelope, at the same
    // pre-manager boundary as ordinary prepareUpdateAudio. No PCM/voice aging.
    bool advanceGarageNativeMusicControl(const OriginalUiGaragePageOwner50KG&,float)noexcept;
    bool canPlayGaragePageEffect1FDE70(const OriginalUiGaragePageOwner50KG&,std::uint32_t)const noexcept;
    bool playGaragePageEffect1FDE70(const OriginalUiGaragePageOwner50KG&,std::uint32_t)noexcept;
    bool nativeWattoPlaybackCurrent(const OriginalUiGarageWattoAudio50KG&)const noexcept;
    bool nativeScenePlaybackCurrent(const OriginalUiGarageDroidAudio50GN&)const noexcept;
    bool nativeSceneImmediatePlaybackCurrent(const OriginalUiGarageDroidAudio50GN&)const noexcept;

    std::array<const assets::OriginalStreamMetadataRow50KZ*,57> nativeRiderSources_{};
    const assets::OriginalStreamMetadataRow50KZ* nativeRiderSourceCurrent(std::size_t,const AudioClip50HD&)const noexcept;
    const assets::OriginalStreamMetadataRow50KZ* nativeWattoStreamSourceCurrent(std::size_t,const AudioClip50HD&)const noexcept;
    bool nativeWattoStreamStartCurrent(std::size_t,std::size_t,const AudioClip50HD&,const void*)const noexcept;
    bool publishNativeWattoStream(OriginalUiGarageWattoAudio50KG&,std::size_t,std::size_t,const AudioClip50HD&)noexcept;
    struct NativeEffectSource {
        std::uint32_t event=0,payloadBytes=0,headerRate=0,durationMs=0,pitch=0;
        float duration20=0.f;
    };
    std::array<NativeEffectSource,6> nativeEffectSources_{};
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> nativeSourceMetadata_;
    const assets::OriginalStreamMetadataRow50KZ* nativeSongSourceCurrent(
        std::uint32_t,const AudioClip50HD&)const noexcept;
    bool nativeEffectSourceCurrent(std::size_t,const AudioClip50HD&,
        AudioMixer50HD::NativeSourceSampleStart50KG&)const noexcept;
    std::uint64_t nativeWetEpoch_=0;
    bool nativeWetInterfaceRetired_=false;
    std::uint64_t nativeWetMasterCount_=0,nativeWetMasterSequence_=0,nativeWetMasterFrame_=0;
    std::uint32_t nativeWetMasterPercent_=0;
    std::uint16_t nativeWetMasterRaw_=0;
    bool nativeWetInterfaceCurrent()const noexcept;
    bool queuedGroupProcessorCurrent(const QueuedGroupControl&)const noexcept;
    // Only the genuine Page opening/closing source callers may use this
    // bounded absolute command2 endpoint. Optional delay/feedback are both -1.
    // No pending command25 may be overtaken; acceptance and DSP application
    // are synchronous at the supplied current native frame boundary.
    bool canApplyGarageMasterReverb26A820(const OriginalUiGaragePageOwner50KG&,
        std::uint32_t absolutePercent,std::uint64_t acceptedFrame)const noexcept;
    bool applyGarageMasterReverb26A820(const OriginalUiGaragePageOwner50KG&,
        std::uint32_t absolutePercent,std::uint64_t acceptedFrame)noexcept;
    bool appendColdGroupControl(std::shared_ptr<QueuedGroupControl>,std::string&)noexcept;
    bool enqueueSceneReverb26D6C0(const std::shared_ptr<OriginalUiGaragePodScene50KG>&,
        std::string&)noexcept;
    bool queuedSceneReverbCurrent(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool sceneGroupIdentityCurrent(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool sceneRoutingHistoryCurrent(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool mayRetireQueuedSceneGroup(const OriginalUiGaragePodScene50KG&)const noexcept;
    // Root-issued native lifetime, independent of a source Page's readiness.
    // No public caller can mint this boundary or turn it into wet admission.
    enum class GarageSourceLifetime : std::uint8_t { unbound, initialPrefix, constructing, constructed, retired, emptyEntryRetired, sharedPageEntryRetired, partialSceneEntryRetired, lateBorrowEntryRetired, loadedPageEntryRetired, wattoBindingEntryRetired, publishedObservationEntryRetired, actorSampleEntryRetired, unsealedRenderEntryRetired };
    enum class UnresolvedEntryFailure : std::uint8_t { partialAttachment,lateBorrow,loadedPageHide,wattoBinding };
    GarageSourceLifetime garageSourceLifetime_=GarageSourceLifetime::unbound;
    std::weak_ptr<OriginalUiGarageAudioBacking50KG> garagePersistentBacking_;
    std::weak_ptr<OriginalUiGarageSession50KG> garageEntryCustody_;
    std::uint64_t garageEntryGeneration_=0,garageEntryEpoch_=0;
    bool bindRetainedGarageTransport(const std::shared_ptr<OriginalUiGarageAudioBacking50KG>&,std::string&)noexcept;
    bool beginRetainedGarageEntry(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    // Exact failed base-cache prefix only. Native retirement is distinct from
    // completed Page/Scene rearm and does not authorize failed-entry replay.
    bool preflightEmptyGarageEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool retireEmptyGarageEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool emptyGarageEntryFailureCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,bool managerRetired)const noexcept;
    // Exact failed shared prompt load: Watto exists, Scene does not. Only the
    // actual Session retirement cursor can discharge this nonempty prefix.
    bool preflightSharedPageEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool retireSharedPageEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    // GUI76 source prefix: unresolved Scene plus Watto were genuinely destroyed.
    // Terminal custody is distinct from both normal rearm and never-Scene abort.
    bool preflightPartialSceneEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool retirePartialSceneEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool partialSceneEntryFailureCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,bool managerRetired)const noexcept;
    bool terminalPartialSceneRowsCurrent()const noexcept;
    bool preflightLateBorrowEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool retireLateBorrowEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool lateBorrowEntryFailureCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,bool managerRetired)const noexcept;
    bool terminalLateBorrowRowsCurrent()const noexcept;
    bool preflightLoadedPageEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool retireLoadedPageEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool loadedPageEntryFailureCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,bool managerRetired)const noexcept;
    bool preflightWattoBindingEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool retireWattoBindingEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool wattoBindingEntryFailureCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,bool managerRetired)const noexcept;
    // Exact resolved, published, never-framed failed observation. The first
    // endpoint is readonly; the drain is private native terminal admission,
    // not guest267BE0, and the final endpoint never issues a reentry receipt.
    bool preflightPublishedObservationEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool drainPublishedObservationEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool retirePublishedObservationEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    // Distinct first partial-Actor-frame failure. Real ages, stream slots,
    // active reservations and retained PCM survive until genuine Group stops.
    bool preflightActorSampleEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool drainActorSampleEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool retireActorSampleEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    // Completed update followed by an actual failed unsealed Draw return.
    // Readonly custody also precedes native Draw release; only the distinct
    // released-Draw retirement cursor may drain controls or retire source owners.
    bool preflightUnsealedRenderEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)const noexcept;
    bool drainUnsealedRenderEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    bool retireUnsealedRenderEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,std::string&)noexcept;
    enum class ResolvedEntryFailure : std::uint8_t { publishedObservation, actorSample, unsealedRender };
    bool resolvedEntryFailureSourceCustodyCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,
        ResolvedEntryFailure)const noexcept;
    bool drainResolvedEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,
        ResolvedEntryFailure,std::string&)noexcept;
    bool retireResolvedEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,
        ResolvedEntryFailure,std::string&)noexcept;
    bool terminalLoadedPageRowsCurrent()const noexcept;
    bool terminalWattoBindingRowsCurrent()const noexcept;
    bool unresolvedEntryFailureCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,
        bool managerRetired,UnresolvedEntryFailure)const noexcept;
    bool retireUnresolvedEntryFailure(const std::shared_ptr<OriginalUiGarageSession50KG>&,
        UnresolvedEntryFailure,std::string&)noexcept;
    bool terminalUnresolvedSceneRowsCurrent()const noexcept;
    bool unresolvedLateBorrowActorAudioCurrent(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGaragePodScene50KG&)const noexcept;
    bool unresolvedLoadedPageActorAudioCurrent(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGaragePodScene50KG&)const noexcept;
    bool unresolvedWattoBindingActorAudioCurrent(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGaragePodScene50KG&)const noexcept;
    bool unresolvedSceneEntryFailureCurrent(const OriginalUiGarageSession50KG&,
        const OriginalUiGaragePodScene50KG&,bool sceneRetiring)const noexcept;
    bool unresolvedLateBorrowEntryFailureCurrent(const OriginalUiGarageSession50KG&,
        const OriginalUiGaragePodScene50KG&,bool sceneRetiring)const noexcept;
    bool unresolvedLoadedPageEntryFailureCurrent(const OriginalUiGarageSession50KG&,
        const OriginalUiGaragePodScene50KG&,bool sceneRetiring)const noexcept;
    bool unresolvedWattoBindingEntryFailureCurrent(const OriginalUiGarageSession50KG&,
        const OriginalUiGaragePodScene50KG&,bool sceneRetiring)const noexcept;
    bool sharedPageEntryFailureCurrent(const std::shared_ptr<OriginalUiGarageSession50KG>&,bool managerRetired)const noexcept;
    bool retainedGarageTransportCurrent()const noexcept;
    bool retainedGarageStorageCurrent()const noexcept;
    static constexpr std::size_t finalGarageClipCount=
        OriginalUiGarageWattoAudio50KG::instanceCount+OriginalUiGarageDroidAudio50GN::soundCount;
    struct FinalNativePcmRetirement {
        std::array<AudioVoiceObservation50HD,finalGarageClipCount> voices{};
        std::uint64_t mainRevision=0,mixerRevision=0,mixedFrames=0,wattoRevision=0,droidRevision=0;
    };
    FinalNativePcmRetirement finalNativePcmReceipt_{};
    bool finalNativePcmRetired_=false;
    bool canRetireFinalNativePcm(const OriginalAudioSampleCache50KG&,FinalNativePcmRetirement&)const noexcept;
    void retireFinalNativePcmPreflighted(const FinalNativePcmRetirement&)noexcept;
    bool finalNativePcmRetirementCurrent()const noexcept;
    bool retainedGarageEntryCurrent()const noexcept;
    bool reconstructibleGarageRowsCurrent()const noexcept;
    bool silentGarageVoiceRangesCurrent()const noexcept;
    bool ordinaryVoiceOwnershipCurrent()const noexcept;
    bool retainedGarageOrdinaryCurrent()const noexcept;
    // Only the real root-owned output's checked device-close continuation.
    bool canRetireOrdinaryFrontendOutput()const noexcept;
    bool retireOrdinaryFrontendOutputAfterDeviceClose()noexcept;
    bool retiredOrdinaryFrontendOutputCurrent()const noexcept;
    bool rebindOrdinaryFrontendOutput(std::uint32_t applicationPause)noexcept;
    // Native ordinary output use is detached, NOT source Aural/Main teardown.
    // Only the exact Root close receipt permits a fresh output reattachment.
    bool ordinaryOutputRetired_=false;
    enum class GarageReentryTransport : std::uint8_t { pristineCold, retainedOrdinary };
    bool rearmGarageAfterManagerRetirementRetainingTransport(
        const std::shared_ptr<OriginalUiGaragePageOwner50KG>&,std::uint64_t,
        const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>&,std::string&)noexcept;
    bool rearmGarageAfterManagerRetirement(
        const std::shared_ptr<OriginalUiGaragePageOwner50KG>&,std::uint64_t,
        const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>&,GarageReentryTransport,std::string&)noexcept;
    // Only the completed normal manager closure can recycle a Scene lifetime.
    // Does not prepare a new Page, flush pending work, or admit wet playback.
    bool rearmGarageAfterManagerRetirementCold(
        const std::shared_ptr<OriginalUiGaragePageOwner50KG>&,std::uint64_t,
        const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>&,std::string&)noexcept;
    std::weak_ptr<OriginalUiGaragePageOwner50KG> lastRearmedGaragePage_;
    std::uint64_t lastRearmedGarageGeneration_=0;
    // Final actual Page owner tail only, not merely Group235150 completion.
    bool releaseRetiredColdSceneRouting(const std::shared_ptr<OriginalUiGaragePageOwner50KG>&,
        OriginalUiGaragePodScene50KG&)noexcept;
    // Called only from Droid's genuine resolved-row publication, after its
    // authenticated binding/handle check and native LogicalAudioOwner insert.
    bool publishColdSceneSampleGroupGain(OriginalUiGarageDroidAudio50GN&,std::size_t,
        const AudioClip50HD&)noexcept;
    static constexpr std::size_t nativeGroupControlCapacity=64;
    std::array<std::shared_ptr<QueuedGroupControl>,nativeGroupControlCapacity> nativeGroupControls_{};
    std::size_t nativeGroupControlCount_=0;
    std::uint64_t nativeGroupControlSequence_=0;
    std::thread::id nativeControlThread_=std::this_thread::get_id();
    OriginalUiGarageWattoAudio50KG* garageWattoAudioCold() noexcept;
    // Cold sample binding needs the existing transport, not completed Garage
    // reverb/playback admission. Public readiness remains unchanged.
    const OriginalUiGarageWattoAudio50KG* nativeWattoSampleTransport() const noexcept;
    const OriginalUiGarageDroidAudio50GN* nativeDroidSampleTransport() const noexcept;
    bool nativeDroidSampleIdentity(const OriginalUiGarageDroidAudio50GN*,const AudioMixer50HD*,std::uint64_t) const noexcept;
    bool nativeWattoSampleIdentity(const OriginalUiGarageWattoAudio50KG*,const AudioMixer50HD*,std::uint64_t) const noexcept;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageActors50GN;
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalApplicationAural50KG;
public:
    enum class NativeGroupControlKind : std::uint8_t { watto, scene };
    struct NativeGroupControlObservation {
        NativeGroupControlKind kind=NativeGroupControlKind::watto;
        const OriginalUiGaragePodScene50KG* scene=nullptr;
        const OriginalUiGarageWattoSoundGroup50KG* group=nullptr;
        const game::OriginalWreckComponent50LA* countedEntity=nullptr;
        const AudioMixer50HD* mixer=nullptr;
        const NativeSourceReverb50KG* wetProcessor=nullptr;
        std::uint64_t sequence=0,groupGeneration=0,objectKey=0,mixerEpoch=0,acceptedControlRevision=0;
        std::uint64_t acceptedFrame=0,routingFrame=0;
        std::int32_t groupSlot=-1;
        std::uint32_t sourceCommand=0;
        std::array<std::uint32_t,3> sourceArguments{};
        //applied means wet DSP/output, never merely applied routing metadata.
        bool queued=false,routingApplied=false,applied=false,identityCurrent=false,neverPlayed=false;
        bool processorBound=false;
    };
    //Read-only pending versus retained custody. Never a playback/Page token.
    std::size_t pendingNativeGroupControls()const noexcept;
    std::size_t retainedNativeGroupControls()const noexcept{return nativeGroupControlCount_;}
    bool inspectNativeGroupControl(std::size_t,NativeGroupControlObservation&)const noexcept;
    struct NativeMasterControlObservation {
        const NativeSourceReverb50KG* processor=nullptr;
        std::uint64_t appliedCount=0,sequence=0,acceptedFrame=0;
        std::uint32_t command=2,absolutePercent=0;
        std::uint16_t rawLeft=0,rawRight=0;
        bool processorBound=false,numericallyApplied=false;
    };
    NativeMasterControlObservation inspectNativeMasterControl()const noexcept;
    static constexpr std::size_t musicVoice = 0, firstEffectVoice = 1;
    static constexpr std::size_t effectVoiceCount = 6, requiredVoices = 7;
    static constexpr std::size_t riderConfirmClipCount = 57; // Two additional authored Kraid clips; mixer voice count unchanged.
    // One accepted Rider Confirm per page lifetime. Its UIACCEPT stream
    // borrows a separate native voice; it cannot replace an active utterance.
    static constexpr std::size_t riderConfirmVoice = requiredVoices, courseRequiredVoices = requiredVoices + 1;
    enum class SteadyRiderError : std::uint8_t {
        none, preparationUnavailable, mixerUnavailable, mixerEpochChanged,
        mixerShapeChanged, revisionUnavailable, paused, soundModeUnavailable,
        soundModeOwnerChanged, unsupportedRouting, unsupportedGainLayer,
        wrongMusic, nonsteadyEnvelope, invalidMusicClip, musicVoiceChanged,
        foreignEffect, invalidEffectClip, effectVoiceChanged
    };
    // Read-only owner facts, not a caller-writable admission flag or a lease.
    // A future application lease must retain this exact transport/output
    // lifetime, match its actual application mode owner, and revalidate all
    // three control revisions at every checkpoint. PCM cursors may progress.
    struct SteadyTransportObservation {
        const OriginalUiMainAudio50KG* transport = nullptr;
        const AudioMixer50HD* mixer = nullptr;
        const OriginalSoundMode50KG* soundMode = nullptr;
        std::uint64_t controlRevision = 0, mixerControlRevision = 0;
        std::uint64_t mixerEpoch = 0, soundModeRevision = 0, sourceMixedFrames = 0;
        std::size_t voiceCount = 0, rejectedVoice = courseRequiredVoices;
        std::uint32_t outputRate = 0, musicSong = 0, profileSoundMode = 0;
        float masterGain = 0.f, musicGain = 0.f;
        bool paused = false, ownsMusic = false, fading = false;
        AudioVoiceObservation50HD music{};
        std::array<AudioVoiceObservation50HD, effectVoiceCount> effects{};
        SteadyRiderError error = SteadyRiderError::preparationUnavailable;
        const char* detail = "Rider steady transport not observed";
    };
    struct RiderAdvanceCommandObservation {
        const OriginalUiMainAudio50KG* transport = nullptr;
        const AudioMixer50HD* mixer = nullptr;
        const OriginalSoundMode50KG* soundMode = nullptr;
        std::uint64_t controlRevisionBefore = 0, controlRevisionAfter = 0;
        std::uint64_t mixerControlRevisionBefore = 0, mixerControlRevisionAfter = 0;
        std::uint64_t mixerEpoch = 0, soundModeRevision = 0, sourceMixedFrames = 0;
        std::uint32_t eventId = 0;
        std::size_t voiceSlot = courseRequiredVoices;
        AudioVoiceObservation50HD voice{};
    };
    class RiderAdvanceCommandReceipt final {
    public:
        bool issued() const noexcept { return issued_; }
        const RiderAdvanceCommandObservation& observation() const noexcept { return observation_; }
    private:
        friend class OriginalUiMainAudio50KG;
        bool issued_ = false;
        RiderAdvanceCommandObservation observation_{};
    };
    class RiderConfirmPlayback final {
    public:
        bool issued() const noexcept { return transport_ != nullptr; }
    private:
        friend class OriginalUiMainAudio50KG;
        const OriginalUiMainAudio50KG* transport_ = nullptr;
        std::uint64_t revision_ = 0;
    };
    OriginalUiMainAudio50KG() = default;
    ~OriginalUiMainAudio50KG() noexcept override { reset(); }
    OriginalUiMainAudio50KG(const OriginalUiMainAudio50KG&) = delete;
    OriginalUiMainAudio50KG& operator=(const OriginalUiMainAudio50KG&) = delete;
    OriginalUiMainAudio50KG(OriginalUiMainAudio50KG&&) = delete;
    OriginalUiMainAudio50KG& operator=(OriginalUiMainAudio50KG&&) = delete;
    // AUDIO itself is the explicit root. Cold reads authenticate exact metadata
    // and original VAGs before decoding. Missing/changed bytes fail; no substitute.
    bool prepare(const std::filesystem::path& audioRoot, AudioMixer50HD& dedicatedFrontendMixer,
        std::string& error) noexcept;
    // Explicit bounded Garage opt-in, never an arbitrary larger mixer.
    // Legacy prepare remains7/8 voices. This exact49 shape adds41 exclusively
    // owned finite Garage instance slots while preserving the SAME music voice.
    static constexpr std::size_t garageRequiredVoices=OriginalUiGarageWattoAudio50KG::requiredFrontendVoices;
    bool prepareGarageFrontend(const std::filesystem::path& audioRoot,
        std::shared_ptr<const assets::OriginalStreamMetadata50KZ>,AudioMixer50HD&,std::string&) noexcept;
    // Separate exact53 aggregate: the original49 contract is unchanged.
    // Slots49..52 belong only to fresh Garage's four once-only scene Sounds.
    // This bounded scene lifetime does not authorize general101DA0 replay.
    static constexpr std::size_t garageSceneRequiredVoices=OriginalUiGarageDroidAudio50GN::requiredFrontendVoices;
    bool prepareGarageSceneFrontend(const std::filesystem::path& audioRoot,
        std::shared_ptr<const assets::OriginalStreamMetadata50KZ>,AudioMixer50HD&,std::string&) noexcept;
    bool garageSceneMenuReady() const noexcept;
    OriginalUiGarageDroidAudio50GN* garageDroidAudio() noexcept;
    const OriginalUiGarageDroidAudio50GN* garageDroidAudio() const noexcept;
    bool garageMenuReady() const noexcept;
    bool canStartGarageLoop6() const noexcept;
    bool startGarageLoop6() noexcept;
    bool canFadeGarageMusic() const noexcept;
    bool fadeGarageMusic() noexcept;
    OriginalUiGarageWattoAudio50KG* garageWattoAudio() noexcept;
    const OriginalUiGarageWattoAudio50KG* garageWattoAudio() const noexcept;
    // Optional additional cold original Song7. Call after prepare and BEFORE
    // any playback/mixing. Main/Options require no Rider music bytes. Failure
    // leaves their already prepared transport intact and Rider unavailable.
    bool prepareRiderMenu(const std::filesystem::path& audioRoot, std::string& error) noexcept;
    // Optional Course destination, before any playback: original Song11 and
    // all 19 default-unlocked choices, Gasgano's and Mawhonic's two each, and
    // Young Anakin's four original choices. Asset capacity is not voice count.
    // Audio preparation grants no profile unlock or destination capability.
    // Native preload of authored IsStream=1
    // speech uses the existing decoder/mixer, with no IOP streamer emulation.
    bool prepareCourseMenu(const std::filesystem::path& audioRoot, std::string& error) noexcept;
    // Optional original Song12, independently cold-prepared after prepare and
    // before any playback/mixing. Credits adds PCM, not a mixer voice or clock.
    // Missing/changed music leaves the already prepared menu audio intact.
    bool prepareCreditsMenu(const std::filesystem::path& audioRoot, std::string& error) noexcept;
    // Optional independently authenticated Song5 for Hall of Fame, cold only.
    // Identical encoded lengths do not make this the Rider Song7 asset.
    bool prepareHallOfFameMenu(const std::filesystem::path& audioRoot, std::string& error) noexcept;
    bool ready() const noexcept override;
    bool riderMenuReady() const noexcept;
    bool courseMenuReady() const noexcept;
    bool creditsMenuReady() const noexcept;
    bool hallOfFameMenuReady() const noexcept;
    // Hall of Fame AC1E47FC/1E4844 ->234900(stream,5,1); B4
    // 1E4950..1E495C ->234440(stream,1.0). Caller owns mode5 routing.
    bool canStartHallOfFameLoop5() const noexcept;
    bool startHallOfFameLoop5() noexcept;
    bool canFadeHallOfFameMusic() const noexcept;
    bool fadeHallOfFameMusic() noexcept;
    // Credits opening1F8850 ->234900(stream,12,1); closing1F8910
    // ->234440(stream,1.0). Replaces/fades the same frontend music voice.
    bool canStartCreditsLoop12() const noexcept;
    bool startCreditsLoop12() noexcept;
    bool canFadeCreditsMusic() const noexcept;
    bool fadeCreditsMusic() noexcept;
    bool canStartCourseLoop11() const noexcept;
    bool startCourseLoop11() noexcept;
    bool canFadeCourseMusic() const noexcept;
    bool fadeCourseMusic() noexcept;
    // Actual Course input1DEF18/1DF0E0/1DF1C8/1DF480: IDs1,5,3,4.
    bool canPlayCourseEffect(std::uint32_t originalId) const noexcept;
    bool playCourseEffect(std::uint32_t originalId) noexcept;
    // Preflight every possible choice BEFORE the real shared RNG draw.
    bool canPlayRiderConfirm() const noexcept;
    bool playRiderConfirmVoice(std::uint32_t soundId, RiderConfirmPlayback&) noexcept;
    bool playRiderConfirmSelection1() noexcept;
    bool releaseRiderConfirmVoice(RiderConfirmPlayback&) noexcept;
    // Concrete source-mode application through THIS transport's actual mixer.
    // No attached owner, stale mixer, or paused transport can certify Accept.
    const OriginalSoundMode50KG* soundModeOwner50KG() const noexcept;
    bool canApplySoundMode18874050KG(std::uint32_t mode) const noexcept;
    // Capacity for the serialized188740 then SELECT1 Accept transaction.
    bool canAcceptGameOptionsAudio50KG(std::uint32_t mode) const noexcept;
    bool applySoundMode18874050KG(std::uint32_t mode) noexcept;
    std::uint32_t sampleRate() const noexcept override {
        return current() ? mixer_->sampleRate() : 0;
    }
    // Source1E68CC ->234900(stream,9,1). Session owns +44/+48 admission checks.
    // Replaces only this frontend music voice, loops original PCM [0,frames).
    bool startMainLoop9() noexcept;
    // Rider1EA060 ->234900(stream,7,1). Replaces the SAME music voice and
    // cancels its outgoing fade; no new stream, clock, RNG or music overlap.
    bool startRiderLoop7() noexcept;
    // Source1EA3C0 invokes the same234440(1.0) fade on that shared stream.
    bool fadeRiderMusic() noexcept;
    // Bounded Rider5/1 normal Back only:1EA848 ->292360(event5,1,0).
    // Read-only preflight is required before the actual profile/route commit.
    bool canPlayRiderBack5() const noexcept;
    bool playRiderBack5() noexcept;
    // Dormant Rider direction event2 ->292360(2,1,0), the authenticated
    // ADVANCE.VAG clip6/voice6 operation. No page/session invokes this yet.
    // Receipt certifies one successful command, never current audibility,
    // continued effect activity, movement, a page update or presentation.
    bool canPlayRiderAdvance2() const noexcept;
    bool playRiderAdvance2(RiderAdvanceCommandReceipt&) noexcept;
    bool inspectSteadyRiderTransport(SteadyTransportObservation&) const noexcept;
    // Source loop-mode234540, targetvolume0, duration1000ms for Main B4.
    // Bounded source calls admitted: zero immediate stop or exactly1 second.
    // Native policy: linear gain at accepted source-update boundaries, followed
    // by the source0.35s silent retirement margin. This is not IOP timing emulation.
    // A concretely idle owned transport is a successful no-change command,
    // matching source234480's early return ignored by Main B4. A foreign or
    // stale active voice remains unavailable; true is not the source result.
    bool fadeMain(float seconds = 1.f) noexcept;
    // Authenticated UIEVENTS IDs1(select),3(left),4(right),7(badcall), all one-shot.
    // Source292810 flags1 ->233410 stops an active sound before restart.
    // One voice per original event; different IDs may overlap.
    bool canPlayMenuEffect(std::uint32_t originalId) const noexcept;
    bool playMenuEffect(std::uint32_t originalId) noexcept;
    // Actual Options menu1F9640 admits events1,3,4,5 only. Main allowlist stays unchanged.
    bool canPlayOptionsEffect(std::uint32_t originalId) const noexcept;
    bool playOptionsEffect(std::uint32_t originalId) noexcept;
    // Page4 source1F50F0 additionally owns event2(defaults) and7(clamp).
    // Read-only concrete preflight for a serialized profile/route transaction.
    bool canPlayGameOptionsEffect(std::uint32_t originalId) const noexcept;
    bool playGameOptionsEffect(std::uint32_t originalId) noexcept;
    // Controls1F3950 owns SELECT1, ADVANCE2, left3, right4 and BACK5.
    // The same real effect voices are retained across same-manager pages.
    bool canPlayControlsEffect(std::uint32_t originalId) const noexcept;
    bool playControlsEffect(std::uint32_t originalId) noexcept;
    bool update(float sourceSeconds) noexcept;
    void pause(bool value) noexcept {
        if(paused_!=value){paused_=value;garageAudio_.paused_=value;garageDroidAudio_.paused_=value;noteControlChange();}
    }
    bool paused() const noexcept { return paused_; }
    // Application must use this gate for the dedicated frontend mixer. Paused
    // calls write silence without consuming PCM. No device callback uses this.
    bool mixStereo16(std::int16_t* output, std::size_t frames) noexcept override;
    void reset() noexcept;
    bool mainLoopActive() const noexcept;
    bool riderLoopActive() const noexcept;
    std::uint32_t activeMusicSong() const noexcept;
    float mainGain() const noexcept { return musicGain_; }
private:
    // Downstream music leaves only, NOT Garage AC/B4 completion. Source AC
    // invokes real26A820(25% wet master) BEFORE1FAF58; no production caller
    // may reach this leaf until that concrete predecessor exists. There is no
    // caller-supplied reverb/ready flag. Ordinary8 retains its SAME mixer,
    // music voice, clock and mode; no scene/Watto Sound is constructed here.
    bool prepareGarageSong6LeafPcm(const std::filesystem::path&,std::string&) noexcept;
    bool canStartGarageSong6Leaf1FAF58() const noexcept;
    bool startGarageSong6Leaf1FAF58() noexcept;
    bool canFadeGarageSong6Leaf1FB09C() const noexcept;
    bool fadeGarageSong6Leaf1FB09C() noexcept;
    bool garageSong6LeafCurrent() const noexcept;
    bool garageSong6LeafAttempted_=false,garageSong6LeafPrepared_=false;
    // Cold transport is not menu readiness. Actual Actors interleave source
    // Sound construction with each model/initial-pose/bounds continuation.
    bool prepareGarageSceneColdTransport(const std::filesystem::path&,
        std::shared_ptr<const assets::OriginalStreamMetadata50KZ>,AudioMixer50HD&,std::string&) noexcept;
    OriginalUiGarageDroidAudio50GN* garageDroidAudioCold() noexcept;
    // Observation only: this slice has no counted Scene+4D8 retirement witness.
    // Native retired_ merely means stop/logical close, NOT source registry or
    // per-Sound reference retirement. Only never-populated rows may release.
    bool garageSceneNativeRowsNeverPopulatedCold()const noexcept {
        const auto& droids=garageDroidAudio_;
        if(droids.sourceSceneGroup_)return false;
        if(!droids.current()||!droids.quiet())return false;
        if(droids.nextActor_||droids.constructedSounds_)return false;
        // append publishes source/sound BEFORE incrementing the count. A
        // failed partial row is not empty merely because the count is zero.
        for(const auto& row:droids.rows_)
            if(row.source||row.sound||row.played108||row.playbackRevision)return false;
        return true;
    }

    bool finishGarageSceneColdTransport(std::string&) noexcept;
    bool prepareImpl(const std::filesystem::path&,AudioMixer50HD&,std::string&,bool,
        std::shared_ptr<const assets::OriginalStreamMetadata50KZ>,bool scene=false,bool incremental=false) noexcept;
    bool garageColdPending_=false,garageColdFinishAttempted_=false;
    std::filesystem::path garageColdRoot_;
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> garageColdMetadata_;

    bool garageGroupCurrent() const noexcept;
    bool courseMixerShape() const noexcept;
    OriginalUiGarageDroidAudio50GN garageDroidAudio_;
    OriginalUiGarageWattoAudio50KG garageAudio_;
    assets::AudioPcm50HC garagePcm_{};
    AudioClip50HD garageClip_{};
    bool garagePrepared_=false,garageScenePrepared_=false;
    bool canRecordControlChange(std::uint64_t count = 1) const noexcept;
    void noteControlChange() noexcept;
    bool current() const noexcept;
    bool startMusicLoop(const AudioClip50HD&, std::uint32_t song,float pitch=1.f) noexcept;
    bool canPlayEffectClip(std::size_t) const noexcept;
    bool playEffectClip(std::size_t) noexcept;
    bool attempted_ = false, prepared_ = false, paused_ = false;
    bool riderAttempted_ = false, riderPrepared_ = false;
    bool courseAttempted_ = false, coursePrepared_ = false;
    bool creditsAttempted_ = false, creditsPrepared_ = false;
    bool hallOfFameAttempted_ = false, hallOfFamePrepared_ = false;
    bool ownsMusic_ = false, fading_ = false;
    AudioMixer50HD* mixer_ = nullptr;
    std::uint64_t mixerEpoch_ = 0;
    std::size_t mixerCount_ = 0;
    std::uint32_t mixerRate_ = 0;
    const OriginalSoundMode50KG* preparedSoundMode_ = nullptr;
    std::uint64_t controlRevision_ = 0, musicPlaybackRevision_ = 0;
    std::array<std::uint64_t, effectVoiceCount> effectPlaybackRevisions_{};
    bool controlRevisionExhausted_ = false;
    float musicGain_ = 0.4f, fadeStartGain_ = 0.4f, musicPitch_ = 1.f;
    float fadeElapsed_ = 0.f, fadeDuration_ = 0.f, fadeRetireAt_ = 0.f;
    std::uint32_t musicSong_ = 0;
    // Original UI setup1B7944 music40/100 and1B78D0 effects100/100.
    std::array<assets::AudioPcm50HC, 7> pcm_{};
    std::array<AudioClip50HD, 7> clips_{};
    assets::AudioPcm50HC riderPcm_{};
    AudioClip50HD riderClip_{};
    assets::AudioPcm50HC coursePcm_{};
    AudioClip50HD courseClip_{};
    assets::AudioPcm50HC creditsPcm_{};
    AudioClip50HD creditsClip_{};
    assets::AudioPcm50HC hallOfFamePcm_{};
    AudioClip50HD hallOfFameClip_{};
    std::array<assets::AudioPcm50HC, riderConfirmClipCount> riderConfirmPcm_{};
    std::array<AudioClip50HD, riderConfirmClipCount> riderConfirmClips_{};
    std::uint64_t riderConfirmPlaybackRevision_ = 0;
    bool ownsRiderConfirm_ = false;
    std::array<bool, effectVoiceCount> ownsEffect_{};
};
}

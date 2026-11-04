#pragma once
#include "audio_mixer50hd.h"
#include "original_stream_library_source50la.h"
#include "native_race_intro_transport50la.h"
#include "../../../src/game/original_application_entities50la.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <array>
#include <memory>
#include <optional>
#include <thread>
namespace original_main_entry50KG {struct Application;}
namespace rr::game {class OriginalResults10450LA;class OriginalRaceMusicManager50LA;class OriginalRaceConstructor50LA;}
namespace race_music50IE {class Owner;}
namespace original_camera_cycle_audio50KX {class Owner;}
namespace source_announcer50IE {class Owner;}
namespace live_audio50HF {class Owner;}
namespace native_original_ai_audio {class Owner;}
namespace race_audio50HQ {class Owner;}
namespace rr::assets {struct OriginalStreamMetadataRow50KZ;
class OriginalStreamMetadata50KZ;class OriginalStandardArchiveProvider50LA;}
namespace rr::platform {
class OriginalUiGarageAudioBacking50KG;
class OriginalUiLightManagerOwner50KG;
class OriginalUiMainAudio50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalAudioPendingWaves50KG;
class AudioMixer50HD;
class NativeSourceReverb50KG;
// Application00, NOT MainAudio, a SoundGroup, or the detached AI audio list.
// Native interface initialization retains the real cold mixer/transport. It
// owns the bounded numerical wet processor without admitting routed playback,
// recreating guest RPC, or importing hardware voice IDs.
class OriginalApplicationAural50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalStreamLibrarySource50LA;
    friend class OriginalUiLightManagerOwner50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class RaceAudioGroups50IC;
    friend class NativeRaceIntroTransport50LA;
    friend class rr::game::OriginalResults10450LA;
    friend class rr::game::OriginalRaceMusicManager50LA;
    friend class rr::game::OriginalRaceConstructor50LA;
    using Application=::original_main_entry50KG::Application;
    using E=game::OriginalApplicationEntities50LA;
    using C=game::OriginalWreckComponent50LA;
public:
    enum class Status : std::uint8_t {complete,owner,affinity,phase,topology,allocation,backend,registeredEmitter,unsupportedNativeReset};
    struct Diagnostic {Status status=Status::complete;std::uint32_t sourcePC=0;};
    struct ListObservation {
        std::uint32_t count=0,growth=0,capacity=0;
        const void* backing=nullptr;
        bool constructed=false,allocatedBit0=false;
    };
    struct Observation {
        const C* entity=nullptr;const void* interfaceA0=nullptr;
        const OriginalUiMainAudio50KG* nativeMain=nullptr;const AudioMixer50HD* mixer=nullptr;
        const NativeSourceReverb50KG* wetProcessor=nullptr;
        std::array<ListObservation,3> lists{};
        std::uint32_t sourceCapacity3C=0,nativeVoiceCount=0;
        std::array<std::uint32_t,2> masterPercent{{0,0}};
        std::uint64_t clearCalls=0;
        std::uint64_t wetProcessorNextFrame=0;
        bool constructed=false,interfaceKnown=false,nativeInitialized=false;
        bool attached=false,retired=false,masterControlsApplied=false,wetOutputSupported=false;
        bool wetProcessorConfigured=false,wetProcessorBound=false;
    };
    ~OriginalApplicationAural50KG()noexcept;
    OriginalApplicationAural50KG(const OriginalApplicationAural50KG&)=delete;
    OriginalApplicationAural50KG& operator=(const OriginalApplicationAural50KG&)=delete;
    bool current()const noexcept;
    Observation inspect()const noexcept;
    // Actual authenticated APPSTART owner already retained by this same interface.
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> retainedStreamMetadata()const noexcept;
private:
    // Read-only native construction custody for the canonical race group.
    // Ownership comes from the actual Application, never a caller alias.
    std::shared_ptr<const OriginalApplicationAural50KG> retainColdStreamManager232DB0(
        const std::shared_ptr<E>&,
        const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>&,
        const std::shared_ptr<const assets::OriginalStandardArchiveProvider50LA>&)const noexcept;
    bool playNativeMainSong234170(OriginalUiMainAudio50KG&,std::uint32_t,
        const AudioClip50HD&,float,float)const noexcept;
    bool playNativeMainEffect233410(OriginalUiMainAudio50KG&,std::size_t,
        const AudioClip50HD&,float,float)const noexcept;
    bool playNativeMainRider233410(OriginalUiMainAudio50KG&,std::size_t,
        const AudioClip50HD&,float,float)const noexcept;
    bool playNativeWattoStream233410(OriginalUiMainAudio50KG&,const void*,std::size_t,std::size_t,
        const AudioClip50HD&,std::int32_t,float,std::int32_t)const noexcept;
    //Reached restart1AC950 only. Actual owned5C signed-empty return; any
    //positive entry stops before its unrepresented virtual74 callback.
    static bool restartOwnedList103210(const Application&,const std::shared_ptr<E>&,
        std::optional<std::int32_t>& indexS0,std::optional<std::uint32_t>& count5C,
        const C*& reachedEmitter,Diagnostic&)noexcept;
    static bool firstStartupInputsCurrent(const Application&)noexcept;
    bool boundedSourceBodyCurrent()const noexcept;
    bool canPublishNativeGarageVoices(const OriginalUiMainAudio50KG&,std::size_t)const noexcept;
    bool nativeOutputSourceQuiescent()const noexcept;
    std::shared_ptr<OriginalAudioPendingWaves50KG> nativeOutputPendingNamespace()const noexcept;
    static bool nativeOutputSourceRetired(const Application&,const C*)noexcept;
    struct List {
        // Actual1043E0/104410 shape. Unwritten pointer slots are not read.
        std::unique_ptr<C*[]> storage;
        std::uint32_t count=0,growth=0,capacity=0;
        bool constructed=false,allocatedBit0=false;
        void construct1043E0()noexcept {constructed=true;}
        bool initialize104410()noexcept;
        C* at104750(std::uint32_t index)const noexcept;
        bool remove1046D0(const C*)noexcept;
        bool valid()const noexcept;
    };
    struct NativeInterface {
        OriginalStreamLibrarySource50LA sourceStreamLibrary;
        explicit NativeInterface(const OriginalApplicationAural50KG& owner)noexcept
            :sourceStreamLibrary(owner,this){}
        // Non-Entity2329A0 owner. Actual267910's three8-row allocations;
        // third contents are unknown until a producer writes them.
        std::unique_ptr<std::uint32_t[]> handles9C0,reuseAge9C8;
        std::unique_ptr<std::uint64_t[]> rows9CC;
        enum class StreamKind : std::uint8_t {song,riderConfirm,watto,raceIntro};
        struct StreamSlot {
            StreamKind kind=StreamKind::song;
            std::uint32_t sourceOrdinal=0; // Song selection or retained Sound ordinal.
            std::uint64_t generation=0; // Native lifetime identity, NOT a guest handle.
            const assets::OriginalStreamMetadataRow50KZ* metadata=nullptr;
            AudioClip50HD clip{};
            std::shared_ptr<const NativeRaceIntroTransport50LA::Assets> raceAssets;
            float age=180.f;
            bool occupied=false;
        };
        std::array<StreamSlot,8> streamSlots{};
        std::uint64_t streamGeneration=0;
        NativeRaceIntroTransport50LA* raceIntroLease=nullptr; // Actual mutable unique-owned transport borrower.
        bool streamRetirementAuthorized=false;
        // Native synchronous submission-boundary observations, NOT guest E00,
        // RPC packet indexes, source channel ages or a hardware-complete flag.
        std::uint64_t controlBoundaryCount=0,controlBoundaryFrame=0;
        std::uint64_t controlBoundaryMainRevision=0,controlBoundaryMixerRevision=0;
        std::uint64_t controlBoundaryGroupSequence=0;
        ~NativeInterface()noexcept;
        std::int32_t streamSelector9C4=-1; //26661C/2666B0, ordinary269650 never changes it.
        std::shared_ptr<OriginalUiMainAudio50KG> main;
        // Strong ownership follows the real2329A0 interface lifetime. Mixer
        // borrows this exact processor; source group/pause/clear103DD0 do not
        // destroy or reconstruct its histories. Destruction precedes main.
        std::unique_ptr<NativeSourceReverb50KG> wetProcessor;
        AudioMixer50HD* mixer=nullptr;
        std::uint64_t mixerEpoch=0;
        std::uint32_t nativeRate=0,nativeCount=0,channelCount37C=0;
        std::array<std::uint32_t,2> masterPercent{{0,0}};
        bool initialized28Bit0=false,mastersApplied=false,wetProcessorBound=false;
    };
    const Application* issuer_=nullptr;
    std::shared_ptr<E> entities_;
    game::OriginalApplicationAnimationClockView50KG lifetime_;
    std::thread::id thread_=std::this_thread::get_id();
    C entity_;std::uint64_t generation_=0;
    std::array<List,3> lists_;
    // Typed source ownership; no public listener factory or arbitrary provider.
#include "original_application_listener50kg.inl"
    using ListenerManager=game::OriginalRaceMusicManager50LA;
    using ListenerXYZ=std::array<std::uint32_t,3>;
    enum class NativeListenerPhase:std::uint8_t {empty,constructing,ready,failed,retiring};
    NativeListenerPhase nativeListenerPhase_=NativeListenerPhase::empty;
    const ListenerManager* nativeListenerIssuer_=nullptr; // Identity only, never dereferenced.
    std::array<NativeListener290A90*,4> listeners4C_{};
    std::array<std::unique_ptr<NativeListener290A90>,4> nativeListenerOwners_{};
    bool listenerCustodyCurrent(bool allowIncomplete)const noexcept;
    bool currentIncludingIncompleteListener()const noexcept;
    bool listenerTerminalCleanupCurrent()const noexcept;
    bool beginSingleRaceListener1A4620(const ListenerManager&,Diagnostic&)noexcept;
    bool removeSingleRaceListener103040(const ListenerManager&,Diagnostic&)noexcept;
    bool listenerCurrent(const ListenerManager&)const noexcept;
    //Private actual-manager/Registration lane borrow; neither a public spatial
    //provider nor proof that an original D48 constructor/resource loader ran.
    bool observeSingleRaceSpeech104140(const ListenerManager&,
        const std::array<std::uint32_t,3>& actualLocalXYZ,
        const std::array<std::uint32_t,4>& actualMain70,
        std::array<std::uint32_t,128>& retainedScratch,
        std::array<bool,128>& scratchKnown,bool& actualAudible,Diagnostic&)const noexcept;
    bool readSingleRaceListener290E10(const ListenerManager&,ListenerXYZ&,Diagnostic&)const noexcept;
    bool writeSingleRaceListener1030B0(const ListenerManager&,const std::array<const float*,9>&,
        const ListenerXYZ&,Diagnostic&)noexcept;
    std::uint32_t word30_=0,word34_=0,word38_=0,word3C_=0,word40_=0,word44_=0,word48_=0;
    std::int32_t word98_=-1;
    bool flags9CBit0_=true,constructed_=false,retired_=false,attached_=false;
    // nullopt means102BE0 never wroteA0, NOT a fabricated source null pointer.
    bool nativeStreamSlotsCurrent(bool permitRetired=false)const noexcept;
    bool nativeStreamFinalRetirementCurrent()const noexcept;
    bool prepareNativeStreamStart(OriginalUiMainAudio50KG&,NativeInterface::StreamKind,
        std::uint32_t,const AudioClip50HD&,bool,
        std::array<NativeInterface::StreamSlot,8>&,AudioMixer50HD::NativeSourceStreamStart50KG&)const noexcept;
    void commitNativeStreamSlots(const std::array<NativeInterface::StreamSlot,8>&)const noexcept;
    bool previewNativeStreamSuffix(float,std::array<NativeInterface::StreamSlot,8>&)const noexcept;
    bool raceIntroTransportCurrent(const NativeRaceIntroTransport50LA&,bool acquiring=false)const noexcept;
    bool raceIntroPublicationServiced(const original_ui_source_timing50LH::PublicationView&,std::uint64_t)const noexcept;
    bool raceIntroFactorCurrent(const NativeRaceIntroTransport50LA&,
        const original_ui_source_timing50LH::PublicationView&,float&,std::uint64_t&)const noexcept;
    std::optional<std::unique_ptr<NativeInterface>> interfaceA0_;
    bool initializationAttempted_=false,nativeInitialized_=false;
    std::uint64_t clearCalls_=0;
    OriginalApplicationAural50KG(const Application&,std::shared_ptr<E>)noexcept;
    static std::uint64_t key(const void* p)noexcept{return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));}
    static bool fail(Diagnostic& d,Status s,std::uint32_t pc)noexcept{d={s,pc};return false;}
    static bool construct13FE24(const Application&,std::shared_ptr<OriginalApplicationAural50KG>&,Diagnostic&)noexcept;
    bool sourceIdentity()const noexcept;
    bool backendCurrent(bool permitFailedTransport=false)const noexcept;
    bool sourceStreamLibraryIdentityCurrent(const OriginalStreamLibrarySource50LA&,
        const assets::OriginalStandardTextureResolver50LA*)const noexcept;
    bool exactAuralOnlyStartupPrefix()const noexcept;
    bool initializeNative102DE0(Diagnostic&)noexcept;
    // Explicit bounded native Light/Aural prefix, not complete retail startup.
    bool attachNativePrefix140108(Diagnostic&)noexcept;
    // Actual UI1B7300 call at1B7340, issued by Application only between
    // real dispatcher construction/publication and its17C21C attachment.
    bool canActivateUiInitializer1B7340(Diagnostic&)const noexcept;
    bool activateUiInitializer1B7340(Diagnostic&)noexcept;
    bool canInitializeUiReverb232C70(Diagnostic&)const noexcept;
    bool initializeUiReverb232C70(Diagnostic&)noexcept;
    // Distinct later1B7300 issuance by Application, after its actual retired
    // dispatcher receipt and fresh output bind. No first-start guard or
    // persistent source/PCM/DSP history is reset to admit this command.
    bool canInitializeUiReentryReverb232C70(Diagnostic&)const noexcept;
    bool initializeUiReentryReverb232C70(Diagnostic&)noexcept;
    // Exact eligible Root141ED0 callback. The factor is the current real
    // Application23C publication, not PCM time, wall time or Main fade dt.
    bool canUpdate103720(float sourceFactor23C,Diagnostic&)const noexcept;
    bool update103720(float sourceFactor23C,Diagnostic&)noexcept;
    // Actual enclosing140D24 caller, after renderer26F910. Native voice and
    // master publications are synchronous; finish any genuinely pending typed
    // group controls at this SAME sample boundary, without mixing or aging.
    bool finishNativeControls267BE0(Diagnostic&)noexcept;
    // Explicit synchronous native command0B adaptation, not guest E00/20/21.
    bool stopResultsNative26D070(const Application&,const RaceAudioGroups50IC&,
        ::race_music50IE::Owner&,::original_camera_cycle_audio50KX::Owner&,
        ::source_announcer50IE::Owner&,::live_audio50HF::Owner&,
        ::native_original_ai_audio::Owner&,::race_audio50HQ::Owner*,std::uint64_t,Diagnostic&)noexcept;
    bool finishResultsNative267BE0(const Application&,const RaceAudioGroups50IC&,Diagnostic&)noexcept;
    // Same transport endpoint at actual Page input call sites, not a render
    // return. Retail267BE0 does not read Application22C; Root's distinct
    // wrapper above still requires its actual completed-render mode0.
    bool canFinishGaragePageInput267BE0(const OriginalUiGaragePageOwner50KG&,Diagnostic&)const noexcept;
    bool finishGaragePageInput267BE0(const OriginalUiGaragePageOwner50KG&,
        std::uint64_t& completedBoundary,Diagnostic&)noexcept;
    bool submitNativeControls267BE0(Diagnostic&)noexcept;
    // Exact Application-owned native replacement of reached268300. No public
    // reset/receipt and no source caller is admitted by this leaf. Active PCM,
    // residual cache leases and intro transport remain outside its supported
    // domain; enrolled Group/list registrations are preserved, not cleared.
    // Rejection precedes every mutation. RPC35 is the
    // recovered IOP ClearMemory, NOT StopAllVoices or a wet/output reset.
    bool resetNativeMemory268300(const Application&,Diagnostic&)noexcept;
    bool clear103DD0(Diagnostic&)noexcept;
    bool releaseRegistered103D00(C&,Diagnostic&)noexcept;
    // Native partial-prefix cleanup is separate from the source102CD0 suffix.
    // Successful full-source suffix requires empty lists/listeners and stopped
    // real backend; neither path clears source Application00 identity.
    bool retireNativeOrEmpty102CD0(Diagnostic&)noexcept;
};
}

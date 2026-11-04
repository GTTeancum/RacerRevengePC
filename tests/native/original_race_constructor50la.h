#pragma once
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_source_camera50LA { class OwnershipBridge; }
#include "original_ui_main_entry50kg.h"
#include "original_course_race_targets50kx.h"
#include "../../src/game/original_race_hud_blink50la.h"
#include "../../src/game/original_results104_50la.h"
#include "../../src/game/original_race_f8_50la.h"
#include "../../src/game/original_race_end_entry50la.h"
#include "../../src/game/original_simple_vcr50la.h"
#include "../../src/game/original_race_music_manager50la.h"
#include "../../src/game/original_race_timing_manager50la.h"
#include "../../src/game/original_human_finish50kg.h"
#include <limits>
#include <optional>
#include <string_view>
#include <thread>
#include "../../platform/linux/native/race_audio_groups50ic.h"

namespace original_results_feedback50LA { class Registration; class Scope; }
namespace original_race_hud_scope50LA { class Registration; class Scope; }
namespace interactive_continuation50DU { struct Context; }
namespace native_world_water50LA {class Domain;}
namespace rr::game {
struct OriginalRaceHudText50KG;
// Actual counted1A65C0 prefix, admitted only by consumed Course ownership.
// Application28C publication is a separate same-owner facet; full load remains gated.
class OriginalRaceConstructor50LA final {
    // Only the existing lexical human-RA continuation binds its94 reference;
    // no public timer/finished setter or ownership receipt is exposed.
    friend struct ::interactive_continuation50DU::Context;
    friend struct ::original_main_entry50KG::Application;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class ::original_pause_observation50LA::Owner;
    friend class ::original_results_roster50LA::Read;
    friend class OriginalResults10450LA;
    friend class OriginalSimpleVCR50LA;
    friend class OriginalRaceMusicManager50LA;
    friend class OriginalRaceTimingManager50LA;
    friend class ::original_results_feedback50LA::Registration;
    friend class ::original_results_feedback50LA::Scope;
    friend class ::original_race_hud_scope50LA::Registration;
    friend class ::original_race_hud_scope50LA::Scope;
    // Immutable entry fence: reject a foreign Results setter thread BEFORE
    // touching source fields or the mutable weak registration below.
    const std::thread::id resultsFeedbackThread_=std::this_thread::get_id();
    // Native same-session feedback borrow only. Weak registration cannot
    // extend the actual Continuation/setup device lifetime or own this Race.
    std::weak_ptr<::original_results_feedback50LA::Registration> resultsFeedback_;
    std::weak_ptr<::original_race_hud_scope50LA::Registration> endHud_;
    // Native position inside ONE suspended source invocation, not a source
    // finish/phase flag. No production caller or reset/replay API exists.
    enum class EndEntryStep:std::uint8_t {subtype,f8,results,hud,dispatch,recordBit,rivalWord,
        ackRead,waitingBit,waitingModeScalar,waitingPlaces};
    EndEntryStep endEntryStep_=EndEntryStep::subtype;
    std::optional<platform::OriginalUiRiderProfileStamp50KG> endEntryProfile_;
    std::uint32_t endEntrySubtype_=0;
    //Identity markers only: compare against the actual owned slots before
    //dereferencing; never extend F8/Results teardown lifetime.
    const OriginalRaceF850LA* endEntryF8_=nullptr;
    const OriginalResults10450LA* endEntryResults_=nullptr;
    void continueEndEntryPrefix1A7DD0(OriginalRaceEndEntryDiagnostic50LA&)noexcept;
    //Same single suspended invocation, not once-per-race or earned authority.
    std::optional<std::uint64_t> endDispatchPublication_;
    std::optional<std::uint32_t> endAcknowledgement20C_;
    // The callee may reach its actual epilogue while1A7B40 remains stopped
    // before its later78 read. No repeat/reset policy is authorized by this.
    bool endDispatchReturned_=false;
    // Native saved program counter/locals of the SAME caller invocation.
    // Unpublished78 is unknown, never the source null branch.
    enum class EndCallerCut:std::uint8_t {owner78,manager58,subtype,associationHead,recordLoopCount,recordRacerGetter,recordAfterLoop,recordVcrStart,
        vcrBit5,replayRacers,replayRacerMode,replayRacerRetirement,replayRacerZone,
        replayRacerCenter,replayRacerWorld,replayWorldTail,rootTail,rootEntity,rootRenderer,rootAfterEntity,rootPhysics,physicsCount,physicsGetter,physicsCall,physicsBody,physicsGrounded,physicsGroundedSpeed,physicsGroundedSlope,physicsGroundedSlopeStopped,physicsAirborneRead,physicsGravityStore,physicsEngineFlags,physicsEngine34Store,physicsB9CRead,physicsEngine38Store,physicsWorldPreparation,physicsCandidatesStopped,physicsProfile,physicsGhost,physicsMusic,physicsMusicBody,musicRaceFlags,musicCount,musicDispatch,musicClear,callerReturn};
    EndCallerCut endCallerCut_=EndCallerCut::owner78;
    const OriginalSimpleVCR50LA* endCallerVcr_=nullptr;
    std::uint8_t endCallerMode_=0;
    std::optional<std::int32_t> endCallerIndexS1_; //Reached1A8090 or1A8174 initializes0.
    // Actual229F80 saved register operands, not new canonical recording lanes.
    std::optional<std::uint32_t> endReplayZoneC38_;
    std::optional<std::array<std::uint32_t,4>> endReplayCenterC40_;
    //Synchronous private borrow; same actual World74 native guards, no readiness receipt.
    ra_source_camera50LA::OwnershipBridge* borrowEndRuntimeNative()const noexcept;
    bool endWorldCurrentNative()const noexcept;
    bool endResetNativeTriggerList2DCC70(std::uint32_t&)noexcept;
    bool endResetNativeParticles161F60(std::uint32_t&)noexcept;
    bool endResetLiveWorldPhysics2702E0(std::uint32_t&)noexcept;

    void continueEndCaller1A7E40(OriginalRaceEndEntryDiagnostic50LA&)noexcept;
    // Saved argument of the exact Root publication that reached1A7DD0.
    // Not latest-frame delta, an elapsed timer, or end admission authority.
    std::optional<std::uint32_t> endEntrySourceF20_;
    bool endRootChildrenRead_=false;
    std::uint64_t endRootEnrollmentSerial_=0;
    const OriginalWreckComponent50LA* endRootChildS0_=nullptr;
    std::optional<std::uint32_t> endRootReturnS4_;
    const OriginalRaceMusicManager50LA* endMusicSource10C_=nullptr; //Saved1A847C target identity.
    std::optional<std::uint32_t> endMusicQueueCount150_;
    std::optional<std::uint32_t> endCallerReturnV0_; //Only actual1A8480 after all reached calls returned.
    std::uint32_t endPhysicsCandidateStopPC_=0x219ad4;
    std::uint64_t endPhysicsProjectionRevision_=0; //Expired synchronous materializer identity; never readiness.
    std::int32_t endPhysicsFirstCandidateCount_=0,endPhysicsSecondCandidateCount_=0,endPhysicsMergedCount_=0;
    bool endPhysicsFirstQueryReturned_=false,endPhysicsSecondQueryReturned_=false,endPhysicsCandidatesMerged_=false;
    bool endNativeDetectorEntered_=false,endNativeDetectorReset_=false,endNativeDetectorReturned_=false;
    bool endNativeQueryCachePrepared_=false,endNativeQueryMembershipCommitted_=false,endNativeQueryPrefixReturned_=false;
    std::uint64_t endNativeQueryMembershipRevision_=0;
    bool endNativeLimitedInitialized_=false,endNativeLimitedReturned_=false;
    std::size_t endNativeLimitedOrdinal_=0;
    bool endNativeSecondaryBodyBound_=false,endNativeSecondarySkipped_=false;
    bool endNativeSecondaryDetectorEntered_=false,endNativeSecondaryDetectorReset_=false,endNativeSecondaryDetectorReturned_=false;
    bool endNativeSecondaryCachePrepared_=false,endNativeSecondaryMembershipCommitted_=false,endNativeSecondaryQueryPrefixReturned_=false;
    std::uint64_t endNativeSecondaryMembershipRevision_=0;
    bool endNativeSecondaryLimitedInitialized_=false,endNativeSecondaryLimitedReturned_=false;
    std::size_t endNativeSecondaryLimitedOrdinal_=0;
    bool endNativeContactStaticsEntered_=false,endNativeContactScenePrepared_=false,endNativeContactReturned_=false;
    std::array<std::uint32_t,3> endNativeContactPosition_{},endNativeContactForward_{},endNativeContactUp_{};
    std::size_t endNativeContactOrdinal_=0;
    std::uint32_t endNativeContactMatchedIdentity_=0;
    std::optional<std::uint32_t> endPhysicsEngine34_,endPhysicsEngine38_; //Reached source operands, not a new engine owner.
    std::optional<std::uint32_t> endPhysicsGravityF12_; //Reached219920 +zero or219A74 read, not a second gravity owner.
    std::optional<std::uint32_t> endGroundedBrakeF0_,endGroundedSpeedF1_; //Exact reached reads; never repaired on resume.
    // Caller SQ VF0 scratch at2199A8/B0; getter writes XYZ only, W stays1.
    std::array<std::uint32_t,4> endGroundedForward60_{},endGroundedUp70_{};
    std::optional<std::uint32_t> endGroundedF4_,endGroundedF1_,endGroundedF3_;
    std::uint32_t endGroundedSlopeStopPC_=0x219964;

    std::optional<std::uint32_t> endPhysicsFactorF20_;
    std::uint32_t endPhysicsCountS2_=0,endPhysicsPassS5_=0;
    bool endPhysicsFirstS3_=true;
    std::int32_t endPhysicsIndexS1_=0;
    void continueEndPhysics1A8390(OriginalRaceEndEntryDiagnostic50LA&)noexcept;
    void continueEndCommonRoot1A82E0(OriginalRaceEndEntryDiagnostic50LA&)noexcept;


    // Source locals initialized only after1AB948's complete feedback tail
    // returns:1AB950 s1=12,1AB958 s0=0. No low2/count or CCC is invented.
    struct EndWaitingPlaceScan {
        enum class Cut:std::uint8_t {minimumLow2,minimumGetter,selectionLow2,
            selectionGetter,selectedGetter,controllerLow2,controllerSlot,owner78,
            vcrStopped,vcrFlag,worldReset,worldResetStopped,pause,pauseStopped,auralStop,
            auralStopped,musicFade,musicFadeStopped,particleBoundary,particleStopped,worldRestartBoundary,worldRestartStopped};
        Cut cut=Cut::minimumLow2;
        //Actual live registers, not a sorted Results row or inferred winner.
        //S1 is overwritten with-1 at1AB99C; S2 first exists at1AB9A0.
        std::int32_t valueS1=12,indexS0=0;
        std::optional<std::int32_t> minimumS2;
        OriginalSimpleVCR50LA* savedVcr=nullptr;
        std::uint32_t stoppedPC=0;
        std::optional<std::uint32_t> pause11C;
        std::optional<std::int32_t> auralIndexS0;
        std::optional<std::uint32_t> auralCount5C;
        const OriginalWreckComponent50LA* auralReachedEmitter=nullptr;
        //Native terminal custody only: never a completed restart receipt.
    };
    std::optional<EndWaitingPlaceScan> endWaitingPlaceScan_;
    // Constructor1A65EC establishes a null78 slot. Native absence is NOT
    // loaded-null78. Only the private original load leaf below publishes it.
    std::unique_ptr<OriginalSimpleVCR50LA> vcr78Owner_;
    OriginalSimpleVCR50LA* vcr78Source_=nullptr;
    // Native position in a dormant1A6DF4..1A6EA0 leaf, not readiness or a
    // predecessor receipt. NO caller until original1A6DEC and earlier loaders
    // return; native world/camera equivalents do not establish that edge.
    enum class VcrLoadCut:std::uint8_t {subtype,allocate,setup,publish,register90,
        afterRegistration,allocationRejected,setupRejected,registrationRejected,retired};
    VcrLoadCut vcrLoadCut_=VcrLoadCut::subtype;
    std::optional<platform::OriginalUiRiderProfileStamp50KG> vcrLoadProfile_;
    std::uint32_t vcrLoadSubtype_=0;
    bool constructVcr78At1A6DF4(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    // Only partial/future detached native cancellation, NOT source teardown
    //1A6840, attachment1AB1A8, or permission to retire a live replay owner.
    bool retireNativeVcr78Detached(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    // Actual1A6CAC publication owns the counted manager BEFORE its load.
    // Empty native custody is not a source constructor-null10C assertion.
    std::unique_ptr<OriginalRaceMusicManager50LA> music10C_;
    std::size_t sourcePlayerMusicBorrows_=0; // Blocks10C retirement while typed33C aliases live.
    //Actual counted1AF7F0 object, published before1AF960. Native time30/flags58
    //and lists remain the ONE existing Setup manager, never copied here.
    std::unique_ptr<OriginalRaceTimingManager50LA> timingB4_;
    bool initializeNativeTimingB4(::interactive_continuation50DU::Context&,
        ::native_racer50HN::Bank&,std::uint32_t&)noexcept;
    bool retireNativeTimingB4(std::uint32_t&)noexcept;
    bool readResultsManagerFlags58(const ::original_camera_session50KX::Session&,
        std::uint8_t&,OriginalResults104Diagnostic50LA&)const noexcept;
    void continuePreAck1AB520(const ::original_position_stats_adapter50LA::Owner&,
        OriginalRaceEndEntryDiagnostic50LA&)noexcept;
    bool resultsInputPredecessorTopology50LA(OriginalResults104Diagnostic50LA&)const noexcept;
    // Only the synchronous Results name-only mutation may advance the
    // retained end stamp. Foreign Profile changes still reject, never refresh.
    bool storeResultsNameAndReconcile1CA528(std::uint32_t,std::string_view,bool& committed)noexcept;
    bool storeResultsNoTimeAndReconcile1CA4DC(bool& committed)noexcept;
    //Private readonly1A87A0 branches through the genuine preceding Race and
    //its concrete142350 children, then actual inactive Pause sibling. Only
    //the owned fixed native UI mapping is admitted; sourceCBC remains absent.
    bool passResultsInputPredecessors1A87A0(const ::original_position_stats_adapter50LA::Owner&,
        const InputEvent48Z&,const platform::OriginalResultsHostInput50LA&,
        OriginalResults104Diagnostic50LA&)const noexcept;
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::shared_ptr<platform::OriginalUiRiderProfileOwner50KG> profile_;
    OriginalWreckComponent50LA entity_;
    original_course_targets50KX::Selection launchSelection_{}; //Actual consumed provenance, not a guest field.
    OriginalRaceHudBlink50LA hudBlink_;
    // Native typed numeric lanes actually written by this constructor. Child
    // pointer slots acquire only explicitly reached native children below.
    // B8/BC/C0 are not represented as null because1A65C0 never writes them.
    float field94_=4.f;
    // Same constructor-zero98 lane, now consumed by the existing real ending
    // cursor. Native progress retains one actual Root invocation, not a timer.
    OriginalHumanEndClock50KG endClock98_;
    std::uint64_t endClockRootPublication_=0;
    std::optional<OriginalHumanEndClockResult50KG> endClockProgress_;
    std::uint32_t field30_=0,field44_=0,field58_=0,field6C_=0,field70_=0;
    std::uint32_t field8C_=0,field90_=0,field9C_=0,fieldA0_=0,fieldA4_=0,fieldA8_=0;
    // Actual first source Camera34, not a guest integer or a second Session.
    std::shared_ptr<OriginalFrontendCameraCold50LA> camera34_;
    std::array<std::uint32_t,3> fields38_{}; //Other constructor-null view slots.
    std::array<std::uint32_t,4> fields48_{};
    enum class CameraInitStep:std::uint8_t {unreached,pause,construct,viewportPrefix,
        nativePending,nativeReady,nativeRetiring,retired};
    CameraInitStep cameraInitStep_=CameraInitStep::unreached;
    // Distinct private source initializer custody. Never a Session/Follow or
    // GPU/world-publication receipt. Display is the SAME loadDisplay_ owner.
    struct LoadingCamera34 {
        bool entered=false,terminal=false,pinned=false,selected=false;
        bool selectionWithdrawn=false,cameraDisposed=false,returned=false;
        std::uint8_t drains=0,callbacks=0;
        bool suffixEntered=false,suffixReturned=false,arrayPublished=false,arrayWithdrawn=false;
        std::uint8_t suffixDrains=0;
        std::uint32_t stopPC=0x114a00;
        std::shared_ptr<OriginalFrontendCameraCold50LA> prior;
    } loadingCamera34_;
    bool loadingCameraSourceNullSuffixCurrent()const noexcept;
    friend class ::ra_cold_aggregate50LA::Owner;
    bool readSourceTrackName187330(std::array<char,3>& out)const noexcept {
        return profile_&&profile_->selectedTrackName187330(out);
    }
    bool loadSourceTrackSegment1A6D48(const original_main_entry50KG::Application&,
        const std::string&,std::string&,std::uint32_t&)noexcept;

    // Comparison only; the actual world owns this Session. Close revokes before
    // fallible retirement. This does not own a second viewport or renderer.
    const ::original_camera_session50KX::Session* cameraNativeAdapter34_=nullptr;
    bool ownsNativeCameraAdapter34(const ::original_camera_session50KX::Session&)const noexcept;
    bool nativeCameraAdapter34LifetimeCurrent(const ::original_camera_session50KX::Session&)const noexcept;
    std::uint32_t field128_=3;
    //Actual HUD pointer slots, constructor-null then published only by the
    //reached1A8CC4 producer. Optional reset means native borrow unavailable,
    //never a fabricated original write of null during lexical revocation.
    struct HudTextSlots {
        std::array<std::array<const OriginalRaceHudText50KG*,7>,2> records12C{};
        std::array<const OriginalRaceHudText50KG*,2> time164{},status17C{};
        std::array<std::array<const OriginalRaceHudText50KG*,2>,2> explosions16C{};
    };
    std::optional<HudTextSlots> hudTextSlots_{std::in_place};
    //All three gleam pointer families are constructor-null initially. The
    //native HUD combines representations; publication withdraws this fact,
    //never invents guest addresses or aliases distinct original objects.
    bool hudGleamConstructorNullKnown_=true;
    std::int32_t field1A4_=-1;
    std::uint32_t field1A8_=0,field1AC_=0;
    // Constructor writes bits2..7 of1B0 and bits0..1 of1B1 only.
    // Unknown lanes are deliberately inaccessible, not zero-filled flags.
    std::uint8_t flags1B0KnownMask_=0xfcu;
    // Canonical represented lanes:1AB93C may set known bit5. Low2 remain
    // unknown until reached1A6C10/1A6C50; no Session projection is copied.
    std::uint8_t flags1B0KnownValue_=0x08u;
    const std::uint8_t flags1B1KnownMask_=0x03u,flags1B1KnownValue_=0;
    bool counted_=false,constructorSuffix_=false,rolledBack_=false;
    platform::RaceAudioGroups50IC* raceAudio_=nullptr; //Actual caller retains through rollback.
    std::array<OriginalWreckComponent50LA*,6> raceGroups_{};
    std::size_t raceGroupCount_=0,raceGroupsAttached_=0;
    bool raceGroupsAttempted_=false,raceMaskRecomputed_=false;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> resourceDomain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> resourceContext_;
    bool resourcesAttempted_=false,resourcesPrepared_=false;
    std::shared_ptr<original_main_entry50KG::RaceDisplayLifetime50LA> loadDisplay_;
    std::shared_ptr<original_main_entry50KG::NativeCollectorFifo50LA> loadFifo_;
    bool loadPrefixAttempted_=false,loadPrefixReached_=false;
    std::shared_ptr<ra_cold_aggregate50LA::Owner> world74_;
    bool worldAttempted_=false;
    enum class NativeWater70State:std::uint8_t {unreached,published,withdrawn};
    NativeWater70State nativeWater70State_=NativeWater70State::unreached;
    // Actual native absent-water copy, NOT guest pointer bits. World owns the
    // constructor domain; this weak identity cannot postpone its retirement.
    std::weak_ptr<const native_world_water50LA::Domain> nativeWater70Domain_;
    std::optional<std::nullptr_t> nativeWater70Object_;
    bool continueNativeTrackAfterWater(const ::original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool stageNativeFirstWreckLoad(const ::original_main_entry50KG::Application&,
        ::original_camera_session50KX::Session&,::ra_source_camera50LA::OwnershipBridge&,std::uint32_t&)noexcept;
    bool readSameControllerWorldCount174E48(const ::original_camera_session50KX::Session&,
        std::uint32_t&,std::uint32_t&)const noexcept;
    // Native resumable position in THIS World constructor suffix, not source
    // finish/phase authority. Unreached until actual World74 publication.
    // Reset/default-low2 stores cannot replay after a later native rejection.
    enum class WorldInitStep:std::uint8_t {unreached,soundReset,defaultLow2,profileLow2,camera};
    WorldInitStep worldInitStep_=WorldInitStep::unreached;
    //Native loading progress, NOT Application330 or a source loader return.
    //The real retained world owns its SCB reader/backing below; no second scene.
    enum class NativeTrackLoadStep:std::uint8_t {unreached,archive,savedDirectory,scb,sceneInfo,scbRetired,tob,nativeAdaptedA4,tobRetired,aub,aubRetired,nativeAdaptedReturn};
    NativeTrackLoadStep nativeTrackLoadStep_=NativeTrackLoadStep::unreached;
    std::array<char,3> nativeTrackName_{};
    std::array<char,260> savedModelDirectory1A9EE0_{};
    bool nativeTrackArchiveCurrent(const ::original_main_entry50KG::Application&)const noexcept;
    bool bindNativeTrackScb34(const ::original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeTrackAfterTob(const ::original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeTrackAfterScb(const ::original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool worldResetPredecessor1A6BF0Current()const noexcept;
    // Distinct TA source backing and counted World prefix. No RA cast/alias,
    // no fabricated full-world load, physics-index or runtime topology owner.
    std::shared_ptr<ta_original_world50LE::Owner> taWorld74_;
    OriginalWreckComponent50LA taWorldEntity_;
    bool taWorldCounted_=false,taWorldPrefixComplete_=false;
    OriginalWreckComponent50LA* nativeTaController48_=nullptr;
    std::uint32_t taWorldMode30_=0,taUseEnvironment35C_=0,taWorldMode524_=0;
    std::unique_ptr<OriginalWreckComponent50LA*[]> taWorldList500_;
    std::uint32_t taWorldListCapacity504_=0,taWorldListGrowth508_=0,taWorldListCount50C_=0;
    bool constructTaWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<ta_original_world50LE::Owner>,std::uint32_t&)noexcept;
    bool retireNativeTaWorldPrefix(std::uint32_t&)noexcept;
    // Independently typed GC static backing and actual counted1B2870 prefix.
    // Runtime query/water owners remain their real caller-owned objects; this
    // prefix does not claim slot30 load or fabricate collision topology.
    std::shared_ptr<gc_original_world50LM::Owner> gcWorld74_;
    OriginalWreckComponent50LA gcWorldEntity_;
    bool gcWorldCounted_=false,gcWorldPrefixComplete_=false;
    std::uint32_t gcWorldMode30_=0,gcUseEnvironment35C_=0,gcWorldMode524_=0;
    std::unique_ptr<OriginalWreckComponent50LA*[]> gcWorldList500_;
    std::uint32_t gcWorldListCapacity504_=0,gcWorldListGrowth508_=0,gcWorldListCount50C_=0;
    bool constructGcWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<gc_original_world50LM::Owner>,std::uint32_t&)noexcept;
    bool retireNativeGcWorldPrefix(std::uint32_t&)noexcept;
    // Independently typed MA static backing and actual counted1B2870 prefix.
    // Runtime query/water owners remain their real caller-owned objects; this
    // prefix does not claim slot30 load or fabricate collision topology.
    std::shared_ptr<ma_original_world50LM::Owner> maWorld74_;
    OriginalWreckComponent50LA maWorldEntity_;
    bool maWorldCounted_=false,maWorldPrefixComplete_=false;
    std::uint32_t maWorldMode30_=0,maUseEnvironment35C_=0,maWorldMode524_=0;
    std::unique_ptr<OriginalWreckComponent50LA*[]> maWorldList500_;
    std::uint32_t maWorldListCapacity504_=0,maWorldListGrowth508_=0,maWorldListCount50C_=0;
    bool constructMaWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<ma_original_world50LM::Owner>,std::uint32_t&)noexcept;
    bool retireNativeMaWorldPrefix(std::uint32_t&)noexcept;
    // Independently typed SA static backing and actual counted1B2870 prefix.
    // Runtime query/water owners remain their real caller-owned objects; this
    // prefix does not claim slot30 load or fabricate collision topology.
    std::shared_ptr<sa_original_world50LM::Owner> saWorld74_;
    OriginalWreckComponent50LA saWorldEntity_;
    bool saWorldCounted_=false,saWorldPrefixComplete_=false;
    std::uint32_t saWorldMode30_=0,saUseEnvironment35C_=0,saWorldMode524_=0;
    std::unique_ptr<OriginalWreckComponent50LA*[]> saWorldList500_;
    std::uint32_t saWorldListCapacity504_=0,saWorldListGrowth508_=0,saWorldListCount50C_=0;
    bool constructSaWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<sa_original_world50LM::Owner>,std::uint32_t&)noexcept;
    bool retireNativeSaWorldPrefix(std::uint32_t&)noexcept;
    // Independently typed GA static backing and actual counted1B2870 prefix.
    // Runtime query/water owners remain their real caller-owned objects; this
    // prefix does not claim slot30 load or fabricate collision topology.
    std::shared_ptr<ga_original_world50LH::Owner> gaWorld74_;
    OriginalWreckComponent50LA gaWorldEntity_;
    bool gaWorldCounted_=false,gaWorldPrefixComplete_=false;
    std::uint32_t gaWorldMode30_=0,gaUseEnvironment35C_=0,gaWorldMode524_=0;
    std::unique_ptr<OriginalWreckComponent50LA*[]> gaWorldList500_;
    std::uint32_t gaWorldListCapacity504_=0,gaWorldListGrowth508_=0,gaWorldListCount50C_=0;
    bool constructGaWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<ga_original_world50LH::Owner>,std::uint32_t&)noexcept;
    bool retireNativeGaWorldPrefix(std::uint32_t&)noexcept;
    std::shared_ptr<OriginalPauseStorage50LA> pause100_;
    // Native custody exists only after actual1A8B00 publication, never a
    // fabricated null source slot or a completed full Race-load receipt.
    std::unique_ptr<OriginalResults10450LA> results104_;
    // One pending/loaded native custody envelope. Actual F8 source slot is
    // constructor-null1A679C until the loader-return publication1A9144 below;
    // acquiring native backing is NOT that source pointer store.
    std::unique_ptr<OriginalRaceF850LA> f8Owner_;
    OriginalRaceF850LA* f8Source_=nullptr;
    bool f8Attached_=false;
    bool attachF8Cold1AAF54()noexcept;
    bool retireF8Cold(const char*&)noexcept;
    bool nativeRootAttached_=false,nativeRootMaskReached_=false;
    // Distinct native accepted-world publication, NEVER original slot30 or
    //17D6A0 completion. Records only reached custody for exact partial unwind.
    enum class NativeHumanRootStep:std::uint8_t {unreached,appending,published,detached};
    NativeHumanRootStep nativeHumanRootStep_=NativeHumanRootStep::unreached;
    bool attachNativeHumanResultsRoot50LA()noexcept;
    bool detachNativeHumanResultsRoot50LA(const char*&)noexcept;
    bool canRetainPause100()const noexcept;
    bool attachNativePauseParent()noexcept;
    bool retireNativePauseParent(const char*&)noexcept;
    bool retireNativeWorldAndGroups(std::uint32_t&,const char*&)noexcept;
    bool pauseWorld1ACF50(bool)noexcept;
    bool pauseEntity142040(bool)noexcept;
    // Actual retained Pause caller; native mixer transport remains distinct
    // from source235500's SPU command queue. No public profile/gain setter.
    bool adjustPauseAudio1BD7C0(const OriginalPauseStorage50LA&,
        OriginalUiGameSettingsProfile50KG&,const OriginalApplicationAnimationClockView50KG&,
        std::uint32_t,bool,bool&)noexcept;
    bool constructWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<ra_cold_aggregate50LA::Owner>,std::uint32_t&)noexcept;
    bool populateResources1AA5A0(std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,
        assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        d={};
        if(!detachedPrefix()||!constructorSuffix_||!loadPrefixReached_||resourcesAttempted_||raceGroupsAttempted_||
           !domain||!context){d.status=assets::OriginalStandardMaterialStatus50LA::phase;
            d.sourcePC=0x1aa5a0;return false;}
        resourcesAttempted_=true;resourceDomain_=std::move(domain);resourceContext_=std::move(context);
        // Exact1AA5AC/1AA5B8 pair. No14D440 collector rebuild belongs here.
        // The existing ScreenTexture leaf implements this identical ordered
        // whole-table pair over the SAME382B38/382B70/383400/resolver owners.
        if(!resourceDomain_->populateTimeStatsResources2DF144(resourceContext_,d))return false;
        resourcesPrepared_=true;d.sourcePC=0x1aa5c8;return true;
    }
    inline static constexpr RaceAudioGroup50IC groupOrder_[]={RaceAudioGroup50IC::Effects110,
        RaceAudioGroup50IC::Engines114,RaceAudioGroup50IC::Music118,RaceAudioGroup50IC::Character11C,
        RaceAudioGroup50IC::Announcer120,RaceAudioGroup50IC::Auxiliary124};
    OriginalRaceConstructor50LA(std::shared_ptr<OriginalApplicationEntities50LA> app,
        std::shared_ptr<platform::OriginalUiRiderProfileOwner50KG> profile)noexcept
        :application_(std::move(app)),profile_(std::move(profile)){}
    bool initializeCount(std::uint64_t session)noexcept {
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&entity_));
        if(application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
           !application_->entityAuthority_.canEnroll(entity_,key))return false;
        const auto result=initializeOriginalWreckComponent50LA(entity_,
            OriginalWreckComponentKind50LA::entity141300,session,key,true,
            application_->objectCount423F60());
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        counted_=true;
        if(!application_->entityAuthority_.enroll(entity_))std::terminate();
        return true;
    }
    bool canPublish28C()const noexcept {
        return detachedPrefix()&&constructorSuffix_&&!loadPrefixAttempted_&&
            !resourcesAttempted_&&!raceGroupsAttempted_&&
            !application_->attachmentLock423F58()&&application_->entityAuthority_.canRetire(entity_);
    }
    bool detachedPrefix()const noexcept {
        return counted_&&!rolledBack_&&application_&&entity_.reference00==1&&
            entity_.kind==OriginalWreckComponentKind50LA::entity141300&&
            entity_.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&entity_))&&
            entity_.sessionGeneration&&!entity_.previous10&&!entity_.next14&&!entity_.parent1C&&
            !entity_.firstChild18&&entity_.mask20==(nativeRootMaskReached_?0xffffffffu:0u)&&
            entity_.descendants24==((constructorSuffix_&&!raceMaskRecomputed_)?0xffffffffu:0u)&&
            entity_.category28==0xffu&&(entity_.flags29&15u)==3u;
    }
public:
    OriginalRaceConstructor50LA(const OriginalRaceConstructor50LA&)=delete;
    OriginalRaceConstructor50LA& operator=(const OriginalRaceConstructor50LA&)=delete;
    ~OriginalRaceConstructor50LA()noexcept {if(counted_&&!rolledBack_)std::terminate();}
    // Actual six2350D0/load/141960 calls. Earlier1A6AA0 load-prefix effects
    // and later world/racer loading are not certified by this bounded leaf.
    bool constructRaceGroups1A6B5C(platform::RaceAudioGroups50IC& audio,
        platform::AudioMixer50HD& mixer,std::uint32_t& sourcePC)noexcept {
        sourcePC=0x1a6b5c;
        if(!detachedPrefix()||!constructorSuffix_||!loadPrefixReached_||!resourcesPrepared_||raceGroupsAttempted_||
           audio.coldStarted_||audio.mixer_||audio.groups_.initialized)return false;
        raceGroupsAttempted_=true;raceAudio_=&audio;
        for(std::size_t i=0;i<6;++i){
            sourcePC=0x1a6b80;
            if(!audio.constructNextCold2350D0(mixer,application_,entity_.sessionGeneration,groupOrder_[i]))return false;
            auto& child=audio.coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity;
            // The existing helper fuses constructor+load. Its load cannot
            // observe the race slot; retain that actual pointer before attach.
            raceGroups_[raceGroupCount_++]=&child; //Original slot store1A6B88.
            sourcePC=0x1a6ba8;
            const auto result=application_->entityAuthority_.attachChildren(entity_,child,0xffffffffu,
                application_->attachmentLock423F58());
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
            ++raceGroupsAttached_;
        }
        sourcePC=0x1a6bc0;return true;
    }
    bool rollbackNativeRaceWorldFailure(std::uint32_t& sourcePC)noexcept;
    // Borrowed identities only. Actual attachment/retirement stays with this
    // consumed race's Application authority, never caller-supplied counts.
    const OriginalWreckComponent50LA* nativeTaRaceEntity()const noexcept {
        return counted_&&!rolledBack_&&taWorldCounted_&&taWorldPrefixComplete_?&entity_:nullptr;
    }
    // Intro custody is selected by the actual consumed Course identity and
    // distinct typed world owner. Definitions follow the complete world types.
    const OriginalWreckComponent50LA* nativeIntroRaceEntity()const noexcept;
    const OriginalWreckComponent50LA* nativeIntroWorldEntity()const noexcept;
    const OriginalWreckComponent50LA* nativeIntroControllerSlot48()const noexcept {
        return nativeIntroRaceEntity()?nativeTaController48_:nullptr;
    }
    bool retainNativeIntroControllerSlot48(OriginalWreckComponent50LA& controller)noexcept {
        if(!nativeIntroRaceEntity()||nativeTaController48_||!application_||
           application_->attachmentLock423F58()||controller.kind!=OriginalWreckComponentKind50LA::cameraController174A30||
           controller.reference00!=1||controller.sessionGeneration!=entity_.sessionGeneration||
           controller.parent1C||controller.previous10||controller.next14||controller.mask20||
           !application_->entityAuthority_.canRetire(controller))return false;
        nativeTaController48_=&controller;return true;
    }
    bool clearNativeIntroControllerSlot48(const OriginalWreckComponent50LA& controller)noexcept {
        return clearNativeTaControllerSlot48(controller);
    }
    // Actual1A95DC slot48 publication. This is a pointer store, NOT141960:
    // the inspected original caller/174DF0 path does not attach a parent edge.
    bool retainNativeTaControllerSlot48(OriginalWreckComponent50LA& controller)noexcept {
        if(!nativeTaRaceEntity()||nativeTaController48_||!application_||
           application_->attachmentLock423F58()||controller.kind!=OriginalWreckComponentKind50LA::cameraController174A30||
           controller.reference00!=1||controller.sessionGeneration!=entity_.sessionGeneration||
           controller.parent1C||controller.previous10||controller.next14||controller.mask20||
           !application_->entityAuthority_.canRetire(controller))return false;
        nativeTaController48_=&controller;return true;
    }
    bool clearNativeTaControllerSlot48(const OriginalWreckComponent50LA& controller)noexcept {
        if(!nativeTaController48_)return true;
        if(nativeTaController48_!=&controller||!application_||application_->attachmentLock423F58()||
           controller.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(controller))return false;
        nativeTaController48_=nullptr;return true;
    }
    const OriginalWreckComponent50LA* nativeTaControllerSlot48()const noexcept {return nativeTaController48_;}
    const OriginalWreckComponent50LA* nativeTaWorldEntity()const noexcept {
        return nativeTaRaceEntity()?&taWorldEntity_:nullptr;
    }
    const ta_original_world50LE::Owner* nativeTaWorldBacking()const noexcept {
        return nativeTaRaceEntity()?taWorld74_.get():nullptr;
    }
    const gc_original_world50LM::Owner* nativeGcWorldBacking()const noexcept {
        return nativeIntroWorldEntity()==&gcWorldEntity_?gcWorld74_.get():nullptr;
    }
    const ma_original_world50LM::Owner* nativeMaWorldBacking()const noexcept {
        return nativeIntroWorldEntity()==&maWorldEntity_?maWorld74_.get():nullptr;
    }
    const sa_original_world50LM::Owner* nativeSaWorldBacking()const noexcept {
        return nativeIntroWorldEntity()==&saWorldEntity_?saWorld74_.get():nullptr;
    }
    const ga_original_world50LH::Owner* nativeGaWorldBacking()const noexcept {
        return nativeIntroWorldEntity()==&gaWorldEntity_?gaWorld74_.get():nullptr;
    }
    // Reverse native construction-failure cleanup. If world construction has
    // acquired its seventh group, its own explicit rollback must run first.
    bool rollbackNativeRaceGroupsFailure(std::uint32_t& sourcePC)noexcept {
        sourcePC=0x1a6b5c;
        if(world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||maWorld74_||maWorldCounted_||saWorld74_||saWorldCounted_||gaWorld74_||gaWorldCounted_||nativeTaController48_)return false;
        if(!raceAudio_)return raceGroupCount_==0;
        auto& audio=*raceAudio_;
        if(!counted_||rolledBack_||application_->attachmentLock423F58()||
           (raceGroupCount_&&(audio.coldApplication_!=application_||
             audio.coldThread_!=std::this_thread::get_id()||!audio.coldMixer_||
             audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC()))||
           audio.coldGroups_[static_cast<std::size_t>(RaceAudioGroup50IC::World4D8)].constructed||
           entity_.firstChild18!=(raceGroupsAttached_?raceGroups_[0]:nullptr))return false;
        // Validate the exact admitted child chain, including a possible final
        // constructed-but-not-attached group, before the first detach.
        for(std::size_t i=0;i<raceGroupCount_;++i){
            const auto* child=raceGroups_[i];
            if(!child||child!=&audio.coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity||
               child->reference00!=1||child->firstChild18)return false;
            if(i<raceGroupsAttached_){
                if(child->parent1C!=&entity_||child->previous10!=(i?raceGroups_[i-1]:nullptr)||
                   child->next14!=(i+1<raceGroupsAttached_?raceGroups_[i+1]:nullptr))return false;
            }else if(child->parent1C||child->previous10||child->next14)return false;
        }
        while(raceGroupCount_){
            const auto i=raceGroupCount_-1;auto& child=*raceGroups_[i];
            if(i<raceGroupsAttached_){
                sourcePC=0x141a70;
                if(application_->entityAuthority_.detach(child,application_->attachmentLock423F58()).status!=
                    OriginalWreckComponentStatus50LA::complete)return false;
                --raceGroupsAttached_;raceMaskRecomputed_=true;
            }
            sourcePC=0x235150;
            if(!audio.releaseEmptyCold142590(groupOrder_[i]))return false;
            raceGroups_[i]=nullptr;--raceGroupCount_;
        }
        raceAudio_=nullptr;return true;
    }
    // Explicit native allocation-failure/cancel policy, NOT1A6840/142590.
    // Group rollback must finish first; only this constructor's detached count
    // and retained native backing retire here. Profile side effects stay reached.
    bool rollbackNativeConstructionFailure(std::uint32_t& sourcePC)noexcept {
        sourcePC=0x1a65cc;
        if(rolledBack_)return true;
        if(!counted_){rolledBack_=true;return true;}
        if(sourcePlayerMusicBorrows_||camera34_||cameraNativeAdapter34_||!endHud_.expired()||pause100_||results104_||f8Owner_||f8Attached_||vcr78Owner_||vcr78Source_||music10C_||timingB4_||nativeRootAttached_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||maWorld74_||maWorldCounted_||saWorld74_||saWorldCounted_||gaWorld74_||gaWorldCounted_||nativeTaController48_||loadDisplay_||loadFifo_||raceAudio_||raceGroupCount_||!detachedPrefix()||application_->attachmentLock423F58()||
           !application_->objectCount423F60()||!application_->entityAuthority_.retire(entity_))return false;
        --entity_.reference00;--application_->objectCount423F60();
        rolledBack_=true;profile_.reset();resourceContext_.reset();resourceDomain_.reset();return true;
    }
};
}
namespace original_main_entry50KG {
inline bool Application::populateConsumedRaceResources1AA5A0(rr::game::OriginalRaceConstructor50LA& owner,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    d={};
    if(raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
       !materialContextOwner_||!materialContextOwner_->live()||
       materialContextOwner_->videoIdentity()!=videoOwner_.get()||
       materialContextOwner_->materialApplication()!=materialApplication||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()){
        d.status=rr::assets::OriginalStandardMaterialStatus50LA::video;d.sourcePC=0x1aa5a0;return false;
    }
    return owner.populateResources1AA5A0(standardMaterialDomainOwner_,materialContextOwner_,d);
}
inline std::shared_ptr<rr::game::OriginalRaceConstructor50LA> Application::constructConsumedRaceCold1A65C0(
    original_course_targets50KX::ConsumedLaunch& launch,std::uint64_t session,std::uint32_t& sourcePC)const noexcept {
    using T=rr::game::OriginalRaceConstructor50LA;
    sourcePC=0x1a65c0;
    if(!enabled||!profile||!riderProfile||!entitiesOwner_||!entitiesOwner_->application10()||
       entitiesOwner_->attachmentLock423F58()||sourceRace28COwner_||!raceConstructorColdOwner50LA_.expired()||
       launch.raceConstructorClaimed_||!launch.target_||
       !launch.matches(*riderProfile,session,launch.target_->track,launch.selection_.rider))return {};
    std::shared_ptr<T> owner;
    try {owner=std::shared_ptr<T>(new T(entitiesOwner_,riderProfile));}catch(...){return {};}
    owner->launchSelection_=launch.selection_;
    if(!owner->initializeCount(session))return {};
    launch.raceConstructorClaimed_=true;
    sourcePC=0x1a66cc;
    if(!riderProfile->beginPodRace1A66CC(*profile)){
        std::uint32_t rollbackPC=0;
        if(!owner->rollbackNativeConstructionFailure(rollbackPC))std::terminate();
        return {};
    }
    owner->entity_.descendants24=0xffffffffu; //1A6808, not generic detached0.
    owner->constructorSuffix_=true;
    raceConstructorColdOwner50LA_=owner;sourcePC=0x1a6830;return owner;
}
}

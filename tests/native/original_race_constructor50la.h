#pragma once
// Common rewrite (R1, PLAN B-C04/B-C05): one counted race 1A65C0 with ONE World
// at race+74 (1B2870) for every course and every pod. No per-course member,
// world slot or branch; the course is data (launchCourse_, the World's Track).
namespace ra_source_camera50LA { class OwnershipBridge; }
#include "common_race_world50cr.h"
#include "original_ui_main_entry50kg.h"
#include "original_tunnel_track50la.h"
#include "original_course_race_targets50kx.h"
#include "../../src/game/original_race_hud_blink50la.h"
#include "../../src/game/original_results104_50la.h"
#include "../../src/game/original_race_f8_50la.h"
#include "../../src/game/original_race_end_entry50la.h"
#include "../../src/game/original_simple_vcr50la.h"
#include "../../src/game/original_race_music_manager50la.h"
#include "../../src/game/original_race_timing_manager50la.h"
#include "../../src/game/original_race_racer_manager50la.h"
#include "../../src/game/original_human_finish50kg.h"
#include <limits>
#include <optional>
#include <string_view>
#include <thread>
#include "../../platform/linux/native/race_audio_groups50ic.h"

namespace original_results_feedback50LA { class Registration; class Scope; }
namespace common_race_start50CR { class Owner; }
namespace common_race_end50CR { class Owner; }
namespace original_consumed_stage50DU { class Owner; }
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
    friend class ::ra_source_camera50LA::OwnershipBridge; //Synchronous same-Race native handling getter.
    friend struct ::original_main_entry50KG::Application;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class ::original_pause_observation50LA::Owner;
    friend class ::original_results_roster50LA::Read;
    friend class OriginalResults10450LA;
    friend class OriginalSimpleVCR50LA;
    friend class ::native_race_replay50PD::Owner; //Common race recording/replay (race78, 90, 1A8, 1B0).
    friend class OriginalRaceMusicManager50LA;
    friend class OriginalRaceTimingManager50LA;
    friend class OriginalRaceRacerManager50LA;
    friend class ::common_race_start50CR::Owner;
    friend class ::common_race_end50CR::Owner;
    friend class ::common_track50CR::Load;
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
            auralStopped,musicFade,musicFadeStopped,particleBoundary,particleStopped,worldRestartBoundary,worldRestartStopped,racerRestart,racerRestartStopped};
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
    // After an in-place 1AC8E0 (Pause RESTART 1BD934, Time Trial 1ABC0C,
    // Practice 1ABD28; common_race_start50CR, R9): every 1A7DD0/1AB520/1A7E40
    // invocation cursor and saved local returns to its constructor value, so
    // the next finish runs a fresh 1A7DD0. Effects already made stay.
    void resetEndInvocation1A7DD0()noexcept {
        endEntryStep_=EndEntryStep::subtype;endEntryProfile_.reset();endEntrySubtype_=0;
        endEntryF8_=nullptr;endEntryResults_=nullptr;
        endDispatchPublication_.reset();endAcknowledgement20C_.reset();endDispatchReturned_=false;
        endCallerCut_=EndCallerCut::owner78;endCallerVcr_=nullptr;endCallerMode_=0;
        endCallerIndexS1_.reset();endReplayZoneC38_.reset();endReplayCenterC40_.reset();
        endEntrySourceF20_.reset();endRootChildrenRead_=false;endRootEnrollmentSerial_=0;
        endRootChildS0_=nullptr;endRootReturnS4_.reset();endMusicSource10C_=nullptr;
        endMusicQueueCount150_.reset();endCallerReturnV0_.reset();
        endPhysicsCandidateStopPC_=0x219ad4;endPhysicsProjectionRevision_=0;
        endPhysicsFirstCandidateCount_=endPhysicsSecondCandidateCount_=endPhysicsMergedCount_=0;
        endPhysicsFirstQueryReturned_=endPhysicsSecondQueryReturned_=endPhysicsCandidatesMerged_=false;
        endNativeDetectorEntered_=endNativeDetectorReset_=endNativeDetectorReturned_=false;
        endNativeQueryCachePrepared_=endNativeQueryMembershipCommitted_=endNativeQueryPrefixReturned_=false;
        endNativeQueryMembershipRevision_=0;
        endNativeLimitedInitialized_=endNativeLimitedReturned_=false;endNativeLimitedOrdinal_=0;
        endNativeSecondaryBodyBound_=endNativeSecondarySkipped_=false;
        endNativeSecondaryDetectorEntered_=endNativeSecondaryDetectorReset_=endNativeSecondaryDetectorReturned_=false;
        endNativeSecondaryCachePrepared_=endNativeSecondaryMembershipCommitted_=endNativeSecondaryQueryPrefixReturned_=false;
        endNativeSecondaryMembershipRevision_=0;
        endNativeSecondaryLimitedInitialized_=endNativeSecondaryLimitedReturned_=false;endNativeSecondaryLimitedOrdinal_=0;
        endNativeContactStaticsEntered_=endNativeContactScenePrepared_=endNativeContactReturned_=false;
        endNativeContactPosition_={};endNativeContactForward_={};endNativeContactUp_={};
        endNativeContactOrdinal_=0;endNativeContactMatchedIdentity_=0;
        endPhysicsEngine34_.reset();endPhysicsEngine38_.reset();endPhysicsGravityF12_.reset();
        endGroundedBrakeF0_.reset();endGroundedSpeedF1_.reset();
        endGroundedForward60_={};endGroundedUp70_={};
        endGroundedF4_.reset();endGroundedF1_.reset();endGroundedF3_.reset();
        endGroundedSlopeStopPC_=0x219964;
        endPhysicsFactorF20_.reset();endPhysicsCountS2_=0;endPhysicsPassS5_=0;
        endPhysicsFirstS3_=true;endPhysicsIndexS1_=0;
        endWaitingPlaceScan_.reset();
    }
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
    // Window close only, after the HUD scope revoked its registration: drops
    // the suspended 1A7DD0/1A7E40/1AB520 invocation's saved locals so the
    // detached VCR/F8/Results retirements can run. Effects already made on
    // F8/Results/HUD/World stay; nothing is replayed or completed.
    bool cancelNativeEndInvocation50PD(const char*& gate)noexcept {
        if(resultsFeedbackThread_!=std::this_thread::get_id()){gate="end invocation cancel requires Race thread";return false;}
        if(!endHud_.expired()){gate="end invocation cancel requires revoked HUD scope";return false;}
        endEntryStep_=EndEntryStep::subtype;endEntryProfile_.reset();endEntrySubtype_=0;
        endEntryF8_=nullptr;endEntryResults_=nullptr;
        endDispatchPublication_.reset();endAcknowledgement20C_.reset();endDispatchReturned_=false;
        endCallerCut_=EndCallerCut::owner78;endCallerVcr_=nullptr;endCallerMode_=0;
        endCallerIndexS1_.reset();endReplayZoneC38_.reset();endReplayCenterC40_.reset();
        endEntrySourceF20_.reset();endRootChildrenRead_=false;endRootEnrollmentSerial_=0;
        endRootChildS0_=nullptr;endRootReturnS4_.reset();endMusicSource10C_=nullptr;
        endWaitingPlaceScan_.reset();
        return true;
    }
    // Actual1A6CAC publication owns the counted manager BEFORE its load.
    // Empty native custody is not a source constructor-null10C assertion.
    std::unique_ptr<OriginalRaceMusicManager50LA> music10C_;
    std::size_t sourcePlayerMusicBorrows_=0; // Blocks10C retirement while typed33C aliases live.
    //Actual counted1AF7F0 object, published before1AF960. Native time30/flags58
    //and lists remain the ONE existing Setup manager, never copied here.
    // Counted2A1160 map owner retains the SAME RA World source projection.
    // Native cold association, not completion of1A6F54/2A1260 startup calls.
    std::shared_ptr<OriginalTunnelTrack50LA> mapB8_;
    bool retireNativeMapB8(std::uint32_t&)noexcept;
    // Counted1AD440 manager over the SAME native Bank. This explicit native
    // roster adapter does not publish guest arrays44/3C or claim1AD520 return.
    // The independent subtype3 record owner is never duplicated on this route.
    std::unique_ptr<OriginalRaceRacerManager50LA> racerB0_;
    bool initializeNativeRacerB0(::interactive_continuation50DU::Context&,
        ::native_racer50HN::Bank&,std::uint32_t&)noexcept;
    bool retireNativeRacerB0(std::uint32_t&)noexcept;
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
    // BC/C0 remain unrepresented. B8 is absent until the explicit native
    // map association above;1A65C0 never writes any of these slots.
    float field94_=4.f;
    // Same constructor-zero98 lane, now consumed by the existing real ending
    // cursor. Native progress retains one actual Root invocation, not a timer.
    OriginalHumanEndClock50KG endClock98_;
    std::uint64_t endClockRootPublication_=0;
    std::optional<OriginalHumanEndClockResult50KG> endClockProgress_;
    // Native saved caller history only. The genuine1A9EE0 result is consumed
    // once, never inferred from the source leaf's pending bool or Session.
    // No new source field, World borrow, or manager-readiness receipt.
    struct SourceWorldReturn1A6D88 {
        bool entered=false,nonzeroBranchTaken=false,delaySlotExecuted=false;
        std::optional<std::uint32_t> returnV0;
        std::uint32_t pc=0x1a6d88u;
    } sourceWorldReturn1A6D88_;
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
    // One rule for every race's end-of-race user: the Application-issued
    // Session bound as Camera34 was issued for THIS race and ITS one World
    // (NativeTaIssuance race/world pair, R2). Replaces raceCameraCourse34(course).
    bool raceCamera34Issued(const ::original_camera_session50KX::Session&)const noexcept;
    std::shared_ptr<::original_camera_session50KX::Session> borrowRaceCamera34()const noexcept;
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
    // Common rewrite (R1, B-C05): the ONE race World at race+74 (1B2870, 0x550)
    // for every course. 1A6BC0 publishes it (constructWorld1A6BC0 below);
    // 1A6D80 -> 1A9EE0 then fills its Track (common_track50CR::Load, R3).
    std::shared_ptr<common_race_world50CR::World> world74_;
    bool worldAttempted_=false;
    // Launch course code as data (stock or registry row), copied at 1A65C0 from
    // the consumed launch; the World's Track identity must name the same code.
    common_track50CR::Code launchCourse_{};
    // Actual1A95DC slot48 camera controller (174A30) of the one World.
    OriginalWreckComponent50LA* nativeController48_=nullptr;
    // Native resumable position in THIS World constructor suffix, not source
    // finish/phase authority. Unreached until actual World74 publication.
    // Reset/default-low2 stores cannot replay after a later native rejection.
    enum class WorldInitStep:std::uint8_t {unreached,soundReset,defaultLow2,profileLow2,camera};
    WorldInitStep worldInitStep_=WorldInitStep::unreached;
    bool worldResetPredecessor1A6BF0Current()const noexcept;
    // 1A6BC0 -> 1B2870 -> 141300 -> 2350D0(World4D8) -> 1B2918, one rule for every course.
    bool constructWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<common_track50CR::Track>,std::uint32_t&)noexcept;
    // 1B2870 destructor -> 235150 -> 1B5F64 (the World's own counted prefix).
    bool retireNativeWorldPrefix(std::uint32_t&)noexcept;
    friend bool ::common_race_world50CR::construct1A6BC0(OriginalRaceConstructor50LA&,
        const original_main_entry50KG::Application&,std::shared_ptr<common_track50CR::Track>,
        std::shared_ptr<common_race_world50CR::World>&,std::uint32_t&)noexcept;
    friend bool ::common_race_world50CR::retire1B2870(OriginalRaceConstructor50LA&,
        common_race_world50CR::World&,std::uint32_t&)noexcept;
    friend const OriginalWreckComponent50LA* ::common_race_world50CR::introWorldEntity(
        const OriginalRaceConstructor50LA&)noexcept;
    friend class ::original_consumed_stage50DU::Owner;
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
    // Private leaf for a DISTINCT authenticated native caller occurrence.
    // OwnershipBridge retains its one-shot history only after genuine native
    // Player-style return; never reset/reuse resourcesAttempted_ above.
    bool populateNativeReturnedPlayerResources1AA5A0(
        assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
        d={};
        if(resultsFeedbackThread_!=std::this_thread::get_id()||!counted_||rolledBack_||
           !resourcesPrepared_||!resourceDomain_||!resourceDomain_->prepared()||
           !resourceContext_||!resourceContext_->live()){
            d.status=assets::OriginalStandardMaterialStatus50LA::phase;
            d.sourcePC=0x1aa5a0u;return false;
        }
        // Actual ascending14D370 scan followed by29E340, on the retained
        // canonical owners/context. This can publish newly referenced rows;
        // it is not a cached success, collector rebuild or forced residency.
        return resourceDomain_->populateTimeStatsResources2DF144(resourceContext_,d);
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
    // Common rewrite: the race's one World (null before 1A6BC0 / after retirement).
    // Borrowed by the camera factory (R2), the track load (R3) and the readers.
    const common_race_world50CR::World* world74()const noexcept {return world74_.get();}
    // Intro custody: the race's one World, counted and prefix-complete (one rule,
    // no per-course exclusion list). Definitions follow the complete World type.
    const OriginalWreckComponent50LA* nativeIntroRaceEntity()const noexcept;
    const OriginalWreckComponent50LA* nativeIntroWorldEntity()const noexcept;
    // The race holds its one counted World.
    bool anyWorld()const noexcept{return world74_!=nullptr;}
    const OriginalWreckComponent50LA* nativeIntroControllerSlot48()const noexcept {
        return nativeIntroRaceEntity()?nativeController48_:nullptr;
    }
    // Actual1A95DC slot48 publication. This is a pointer store, NOT141960:
    // the inspected original caller/174DF0 path does not attach a parent edge.
    bool retainNativeIntroControllerSlot48(OriginalWreckComponent50LA& controller)noexcept {
        if(!nativeIntroRaceEntity()||nativeController48_||!application_||
           application_->attachmentLock423F58()||controller.kind!=OriginalWreckComponentKind50LA::cameraController174A30||
           controller.reference00!=1||controller.sessionGeneration!=entity_.sessionGeneration||
           controller.parent1C||controller.previous10||controller.next14||controller.mask20||
           !application_->entityAuthority_.canRetire(controller))return false;
        nativeController48_=&controller;return true;
    }
    bool clearNativeIntroControllerSlot48(const OriginalWreckComponent50LA& controller)noexcept {
        if(!nativeController48_)return true;
        if(nativeController48_!=&controller||!application_||application_->attachmentLock423F58()||
           controller.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(controller))return false;
        nativeController48_=nullptr;return true;
    }
    // Reverse native construction-failure cleanup. If world construction has
    // acquired its seventh group, its own explicit rollback must run first.
    bool rollbackNativeRaceGroupsFailure(std::uint32_t& sourcePC)noexcept {
        sourcePC=0x1a6b5c;
        if(world74_||nativeController48_)return false;
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
        if(sourcePlayerMusicBorrows_||camera34_||cameraNativeAdapter34_||!endHud_.expired()||pause100_||results104_||f8Owner_||f8Attached_||vcr78Owner_||vcr78Source_||music10C_||timingB4_||racerB0_||mapB8_||nativeRootAttached_||world74_||nativeController48_||loadDisplay_||loadFifo_||raceAudio_||raceGroupCount_||!detachedPrefix()||application_->attachmentLock423F58()||
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
       launch.raceConstructorClaimed_||
       // The launch's course is data (stock or registry row), never a HostTrack value.
       !launch.matchesCourse(*riderProfile,session,launch.courseCode(),launch.selection_.rider))return {};
    common_track50CR::Code launchCourse{};
    if(!common_track50CR::Code::parse(launch.courseCode(),launchCourse))return {};
    std::shared_ptr<T> owner;
    try {owner=std::shared_ptr<T>(new T(entitiesOwner_,riderProfile));}catch(...){return {};}
    // The consumed selection as the race runs it: after 1AD520 (sameLaunch1AD520,
    // matched above) a Time Trial's provenance is its solo field, so every later
    // same-profile check and the roster's field size read the same values.
    original_course_targets50KX::Selection running{};
    if(!riderProfile->courseRaceSnapshot(running)||
       !original_course_targets50KX::sameLaunch1AD520(launch.selection_,running))return {};
    owner->launchSelection_=running;
    owner->launchCourse_=launchCourse;
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

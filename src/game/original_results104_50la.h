#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
namespace original_main_entry50KG { struct Application; class ResultsRendererLease50LA; class NativeResultsFrame50LA; }
namespace original_camera_session50KX { class Session; }
namespace original_race_hud_scope50LA { class Registration; }
namespace original_camera_cycle_audio50KX { class Owner; }
namespace rr::platform { class GameHost50ER; class VehicleWindowGl50DS; }
namespace original_results_roster50LA { class Read; }
namespace original_position_stats_adapter50LA { class Owner; }
namespace common_roster50CR { class Owner; }
namespace common_race_start50CR { class Owner; }
namespace native_racer50HN { class Bank; }
namespace rr::assets {
class OriginalStandardMaterialDomain50LA;
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialCollectorReceipt50LA;
class OriginalStandardMaterialContext50LA;
class OriginalApplicationFonts50LA;
class OriginalPauseSourceData50LA;
}
namespace rr::game {
class OriginalApplicationEntities50LA;
struct OriginalWreckComponent50LA;
class OriginalRaceConstructor50LA;
struct OriginalResults104Diagnostic50LA {
    enum class Status : std::uint8_t { complete, phase, owner, source, allocation, material, font, incomplete };
    Status status=Status::complete;
    std::uint32_t sourcePC=0;
    std::size_t object=0, retainedPrimitives=0;
    const char* operation=nullptr;
};
// One counted Results Entity, owned only by its actual consumed Race104.
// There is no public construction/load/draw/input API. Completed loading is
// not a Results acknowledgement, Hall transition or full Race constructor.
class OriginalResults10450LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_main_entry50KG::ResultsRendererLease50LA;
    friend class ::original_main_entry50KG::NativeResultsFrame50LA;
    friend class OriginalRaceConstructor50LA;
    friend class rr::platform::VehicleWindowGl50DS;
    friend class ::original_race_hud_scope50LA::Registration;
    friend class ::common_race_start50CR::Owner; // 1AC8E0 1ACCD8 race104 141520 (disable1A8B1C) only
    static bool nameEntryRaceCurrent1C9B10(const OriginalRaceConstructor50LA&,const OriginalResults10450LA*,
        const std::shared_ptr<OriginalApplicationEntities50LA>&,const std::uint64_t&)noexcept;
    static bool liveResultsRaceAuthorityCurrent(const OriginalRaceConstructor50LA&,
        const std::shared_ptr<OriginalApplicationEntities50LA>&)noexcept;
    //Implementation-only include isolation; no cached count or new custody.
    static bool demandObserverCameraMatches1C15B0(const ::original_position_stats_adapter50LA::Owner&,
        const ::original_camera_session50KX::Session&)noexcept;
    static const ::native_racer50HN::Bank* demandObserverBankIdentity1C16F8(
        const ::original_position_stats_adapter50LA::Owner&)noexcept;
    bool demandRosterCountCurrent1C16F8(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,const ::common_roster50CR::Owner*,
        const ::native_racer50HN::Bank*,std::size_t&)const;
    //Private synchronous custody reads; no cached readiness or new ownership.
    static bool careerRosterCurrentNative(const ::common_roster50CR::Owner*,
        const OriginalRaceConstructor50LA&,const std::shared_ptr<OriginalApplicationEntities50LA>&,
        const std::shared_ptr<assets::OriginalStandardMaterialDomain50LA>&,std::uint64_t,
        const ::original_position_stats_adapter50LA::Owner&)noexcept;
    static bool careerRacerCurrentNative(const ::common_roster50CR::Owner*,
        std::uint32_t)noexcept;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    OriginalResults10450LA(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t);
    bool constructed()const noexcept;
    bool load1BEEB0(original_results_roster50LA::Read&,
        std::shared_ptr<assets::OriginalStandardMaterialDomain50LA>,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA>,
        std::shared_ptr<assets::OriginalApplicationFonts50LA>,
        std::shared_ptr<const assets::OriginalPauseSourceData50LA>,
        std::string& actualDirectory36B970,const std::string& actualFontDirectory36B420,
        std::uint32_t actualVideoMaximum60,OriginalResults104Diagnostic50LA&)noexcept;
    bool disable1A8B1C()noexcept;
    // Private concrete slot20=141560 for the owning Race's1A7E18 call.
    // Only the actually empty Entity-child case is represented; no mode,
    // acknowledgment, feedback, draw or scheduler admission is implied.
    bool activate1A7E18(const OriginalRaceConstructor50LA&,OriginalResults104Diagnostic50LA&)noexcept;
    //Readonly same loaded-Operation/Bank custody for the private Race prefix.
    //Does not synthesize original managerB0, finish, input or20C authority.
    bool endDispatchOwnerCurrent1AB520(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&)const noexcept;
    bool loadedDisabled()const noexcept;
    bool belongsTo(const std::shared_ptr<OriginalApplicationEntities50LA>&,std::uint64_t)const noexcept;
    bool retirementCurrent()const noexcept;
    // Private native Frame lifetime leaves. Only this Results owner's actual
    // table and captured receipt may be pinned; no new seal is minted.
    bool pinNativeFrameCollector50LA(assets::OriginalStandardMaterialTable50LA&,
        const assets::OriginalStandardMaterialCollectorReceipt50LA&)noexcept;
    // Called after the same Frame releases its source borrow, before the
    // retained Race/table owners are destroyed. Not a renderer receipt.
    void unpinNativeFrameCollector50LA(assets::OriginalStandardMaterialTable50LA&)noexcept;
    // Parent teardown ONLY: authentic post104970 detached retry, never a live Entity.
    bool detachedRetirementCurrent(const OriginalRaceConstructor50LA&)const noexcept;
    OriginalWreckComponent50LA* entity()noexcept;
    bool attachedMask(std::uint32_t)noexcept;
    // Unjoined, private CPU preparation ONLY. No member of this enum means
    // slot48/74/78 completion, renderer admission, activation or input ack.
    // The exact retained Race104 and its Profile are checked before preparing
    // the existing owned ScreenObjects; no caller-supplied display facts.
    enum class DisplayCpuPrefix : std::uint8_t { none, textures, titles, roster, recordSummary, keyboard, careerWinnings, careerSummary };
    DisplayCpuPrefix prepareDisplayCpuPrefix(const OriginalRaceConstructor50LA&,
        OriginalResults104Diagnostic50LA&)noexcept;
    DisplayCpuPrefix prepareRosterPageCpuPrefix(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    DisplayCpuPrefix prepareRecordSummaryCpuPrefix(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    DisplayCpuPrefix prepareKeyboardPageCpuPrefix(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Actual1C4CA0 career-page CPU body through1C6F98. Its first-human
    // CCC/CD0/CD4 observations never certify earned credit or commit a prize.
    DisplayCpuPrefix prepareCareerWinningsCpuPrefix(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Remaining1C6F98..1C8A74 CPU messages/statistics; still no slot48,
    // submission or renderer lifecycle completion.
    DisplayCpuPrefix prepareCareerSummaryCpuPrefix(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Private native CPU lowering only: snapshots every reached derived
    // primitive at its source occurrence, including clean Text packets.
    // Uncalled until actual slot48/Frame/B4/time/renderer lifetime joins;
    // its retained output is not a draw/queue/completion capability.
    DisplayCpuPrefix captureNativeDerivedCpu1C21B0(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&,bool twoPlayerBase=false)noexcept;
    // Uncalled single-human shared-base CPU traversal. Reads actual completed
    // native Root timing, not a caller delta or source last-camera authority.
    DisplayCpuPrefix captureNativeBaseCpu1B9220(
        const ::original_main_entry50KG::Application&,const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Slot40 numeric demand body under the accepted final one-human native
    // Camera adaptation; no source App0C/264 or mode3 receipt is invented.
    bool demandNativeFinalCamera1C15B0(const ::original_main_entry50KG::Application&,
        const OriginalRaceConstructor50LA&,const ::original_camera_session50KX::Session&,
        const ::original_position_stats_adapter50LA::Owner&,OriginalResults104Diagnostic50LA&)noexcept;
    // Actual source dirty-layout/name-query prefix, stopping BEFORE the
    // independent RaceB4/Profile render gate. No live scheduler or submission.
    enum class Slot48CpuPrefix : std::uint8_t { none, dirtyLayout, afterRefreshGate, renderGate, resetBranch, sharedBaseCpu };
    // Private same-root CPU slot48 path, not source mode5/last-camera or GPU admission.
    Slot48CpuPrefix captureNativeSlot48Cpu1C17A0(
        const ::original_main_entry50KG::Application&,const OriginalRaceConstructor50LA&,
        const ::original_camera_session50KX::Session&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    Slot48CpuPrefix prepareSlot48LayoutCpuPrefix1C17A0(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Only Application's private same-Root bridge supplies these arguments.
    // Original394/398 initialization remains unknown. Actual first activation
    // may apply the separately tagged native visual endpoint policy; this
    // exact update never seeds, repairs or restarts phase itself.
    enum class UpdateCpuPrefix : std::uint8_t { none, blink, pulse, timers, baseReturned };
    bool updateCpuPrefix1C8AE0(float actual23C,std::uint64_t publication,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Actual1B9FE0 state chronology, with EXPLICIT native host availability
    // in place of original device6C4. The original pad protocol is not ported.
    bool updateBaseInput1B9FE0(const OriginalRaceConstructor50LA&,
        const platform::GameHost50ER&,OriginalResults104Diagnostic50LA&)noexcept;
    // Only the real lexical Camera/Bank/audio borrow can consume the actual
    // GameHost-owned fresh poll. No public key, string, mode or ack setter.
    bool pressHostInput1C8C30(const ::original_main_entry50KG::Application&,
        OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        ::original_camera_cycle_audio50KX::Owner&,platform::GameHost50ER&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Original1CA8E0 scalar chronology; no public mode/ready setter. Actual
    // human feedback238870 tail remains required even if mode is unchanged.
    enum class ModeCpuPrefix : std::uint8_t { none, scalarPrefix, zeroHumanTail, humanFeedbackTail };
    ModeCpuPrefix setModeCpuPrefix1CA8E0(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        std::uint32_t modeA1,std::uint32_t animateA2,OriginalResults104Diagnostic50LA&)noexcept;
    // Resume only that same owner's already-started private mode invocation.
    // Never repeats200/204/208/390 stores or a completed feedback clear.
    ModeCpuPrefix continueModeFeedbackCpu1AF770(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Bounded first-human1C9B10 producer, not a live input/record commit.
    // Actual timing CD0 is observed only; no earned-credit authority follows.
    enum class NameEntryCpuPrefix : std::uint8_t { none, keyboard, name, title,
        lapQuery, raceQuery, recordQueries };
    NameEntryCpuPrefix prepareNameEntryCpu1C9B10(const OriginalRaceConstructor50LA&,
        const ::original_position_stats_adapter50LA::Owner&,
        OriginalResults104Diagnostic50LA&)noexcept;
    // Host partial cleanup visits only acquired objects. It never reports the
    // full source1BEC40 destructor or original renderer-retirement receipt.
    bool closeDetached(OriginalResults104Diagnostic50LA&)noexcept;
public:
    OriginalResults10450LA(const OriginalResults10450LA&)=delete;
    OriginalResults10450LA& operator=(const OriginalResults10450LA&)=delete;
    ~OriginalResults10450LA()noexcept;
};
}

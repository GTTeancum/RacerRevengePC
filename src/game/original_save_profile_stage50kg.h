#pragma once
#include "original_save_career_projection50kg.h"
#include "original_save_records_projection50kg.h"
#include "original_ui_game_options_controller50kg.h"
#include "control_presets49d.h"

namespace rr::game {
// Pure preparation, not a live import transaction or completion receipt.
// Supplied before-snapshots are ordinary data, NOT authenticated owner access.
// It stages every27-career projection and saved records from one immutable
// envelope and preserves all out-of-payload state represented by these snapshots.
// No live pointers, mutation callback, apply(), success-dialog token or IO exists.
class OriginalSaveProfileStage50KG final {
public:
    enum class Status:std::uint8_t {
        ok,missingEnvelope,invalidCurrentSettings,careerFields,recordsFields,
        unsupportedControlConfiguration,unsupportedControlPreset,allocationFailure,
        unsupportedHudPreference,unsupportedCameraMode
    };
    struct Diagnostic {
        Status status=Status::ok;
        std::size_t payloadByteOffset=0;
        std::uint32_t careerIndex=0;
        OriginalSaveCareerProjection50KG::Diagnostic career{};
        OriginalSaveRecordsProjection50KG::Diagnostic records{};
    };
    // All operations affect private temporary data only. Failure preserves out.
    // Settings known/group and both control action rows are copied unchanged.
    // Saved control config<=1/preset<=7 is an explicit supported-menu restriction,
    // not original Load rejection behavior. Five additional words per slot stay
    // exact: HUD words require0..7; camera mode requires0..7 excluding3.
    // Toggle is a zero/nonzero predicate; follow variant is a searched ID, not
    // an array index. Actual camera-bank/route readiness remains a separate gate.
    static Status prepare(std::shared_ptr<const OriginalSavePayloadEnvelope50KG>,
        const OriginalUiGameSettingsProfile50KG& currentSettings,
        const ControlPresets49D& currentControls,std::uint8_t currentFlags11054,
        std::unique_ptr<OriginalSaveProfileStage50KG>& out,Diagnostic&) noexcept;
    OriginalSaveProfileStage50KG(const OriginalSaveProfileStage50KG&)=delete;
    OriginalSaveProfileStage50KG& operator=(const OriginalSaveProfileStage50KG&)=delete;
    ~OriginalSaveProfileStage50KG()=default;
    const OriginalSavePayloadEnvelope50KG& envelope() const noexcept {return *envelope_;}
    // Retain the exact same immutable source for private metadata/provenance staging.
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> retainedEnvelope() const noexcept {return envelope_;}
    const std::array<OriginalTournamentCareer50KG,27>& careers() const noexcept {return careers_;}
    const std::array<std::array<std::uint8_t,21>,27>& careerNames219D()const noexcept{return careerNames_;}
    const OriginalSaveRecordsProjection50KG& records() const noexcept {return *records_;}
    const OriginalUiGameSettingsProfile50KG& settingsBefore() const noexcept {return settingsBefore_;}
    const OriginalUiGameSettingsProfile50KG& settingsAfter() const noexcept {return settingsAfter_;}
    const ControlPresets49D& controlsBefore() const noexcept {return controlsBefore_;}
    const ControlPresets49D& controlsAfter() const noexcept {return controlsAfter_;}
    std::uint8_t flags11054Before() const noexcept {return flagsBefore_;}
    std::uint8_t flags11054After() const noexcept {return flagsAfter_;}
private:
    OriginalSaveProfileStage50KG(std::shared_ptr<const OriginalSavePayloadEnvelope50KG>,
        const OriginalUiGameSettingsProfile50KG&,const ControlPresets49D&,std::uint8_t) noexcept;
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope_;
    std::array<OriginalTournamentCareer50KG,27> careers_{};
    std::array<std::array<std::uint8_t,21>,27> careerNames_{};
    std::unique_ptr<OriginalSaveRecordsProjection50KG> records_;
    OriginalUiGameSettingsProfile50KG settingsBefore_,settingsAfter_;
    ControlPresets49D controlsBefore_,controlsAfter_;
    std::uint8_t flagsBefore_,flagsAfter_;
};
}

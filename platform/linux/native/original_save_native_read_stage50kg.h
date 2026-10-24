#pragma once
#include "original_save_native_reader50kg.h"
#include "../../../src/game/original_save_profile_stage50kg.h"

namespace rr::platform {
class OriginalUiRiderProfileOwner50KG;
class OriginalUiFrontend50KG;
// Private cold preparation only. Not a full-load capability or live mutation.
// Keeps the actual host read receipt and every projection inseparable. No API
// admits arbitrary bytes or an independently prepared envelope/stage.
class OriginalSaveNativeReadStage50KG final {
public:
    enum class Status:std::uint8_t {
        ok,missingProvider,readFailure,envelopeFailure,stageFailure,allocationFailure
    };
    struct Diagnostic {
        Status status=Status::ok;
        OriginalSaveNativeReader50KG::Diagnostic read{};
        game::OriginalSavePayloadEnvelope50KG::Diagnostic envelope{};
        game::OriginalSaveProfileStage50KG::Diagnostic stage{};
    };
    ~OriginalSaveNativeReadStage50KG()=default;
    OriginalSaveNativeReadStage50KG(const OriginalSaveNativeReadStage50KG&)=delete;
    OriginalSaveNativeReadStage50KG& operator=(const OriginalSaveNativeReadStage50KG&)=delete;
    const OriginalSaveNativeReader50KG::Receipt& receipt()const noexcept{return *receipt_;}
    const game::OriginalSaveProfileStage50KG& projection()const noexcept{return *stage_;}
    // Neither accessor implies all semantic owners have been reconstructed.
private:
    friend class OriginalUiRiderProfileOwner50KG;
    friend class OriginalUiFrontend50KG;
    OriginalSaveNativeReadStage50KG()=default;
    // No current caller. Only the future genuine private lifecycle may invoke
    // this; settings/controls are before-snapshots, NOT owner attestations.
    // Performs actual IO if called. All failures preserve out/live profiles.
    static Status readAndPrepare(std::shared_ptr<const OriginalSaveNativeReader50KG>,
        const game::OriginalUiGameSettingsProfile50KG&,
        const game::ControlPresets49D&,std::uint8_t currentFlags11054,
        std::unique_ptr<OriginalSaveNativeReadStage50KG>& out,Diagnostic&)noexcept;
    std::unique_ptr<const OriginalSaveNativeReader50KG::Receipt> receipt_;
    std::unique_ptr<game::OriginalSaveProfileStage50KG> stage_;
};
}

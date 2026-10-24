#include "original_save_native_read_stage50kg.h"
#include <utility>

namespace rr::platform {
OriginalSaveNativeReadStage50KG::Status OriginalSaveNativeReadStage50KG::readAndPrepare(
    std::shared_ptr<const OriginalSaveNativeReader50KG> provider,
    const game::OriginalUiGameSettingsProfile50KG& settings,
    const game::ControlPresets49D& controls,std::uint8_t flags,
    std::unique_ptr<OriginalSaveNativeReadStage50KG>& out,Diagnostic& d)noexcept {
    d={};
    if(!provider)return d.status=Status::missingProvider;
    try {
        // Capture before IO under the future caller's serialized owner domain.
        // A later commit still requires exact owner/generation revalidation.
        const auto settingsBefore=settings;const auto controlsBefore=controls;
        std::unique_ptr<OriginalSaveNativeReadStage50KG> next(new OriginalSaveNativeReadStage50KG);
        // Full read includes its own version/identity. Never join a prior header
        // probe to later bytes, re-open a different root, or accept a raw vector.
        if(provider->read(OriginalSaveNativeReader50KG::Kind::payload186C70,next->receipt_,d.read)!=
            OriginalSaveNativeReader50KG::Status::ok)return d.status=Status::readFailure;
        const auto& bytes=next->receipt_->bytes();
        std::unique_ptr<game::OriginalSavePayloadEnvelope50KG> owned;
        if(game::OriginalSavePayloadEnvelope50KG::retainOriginalBytes(bytes.data(),bytes.size(),owned,d.envelope)!=
            game::OriginalSavePayloadEnvelope50KG::Status::ok)return d.status=Status::envelopeFailure;
        std::shared_ptr<const game::OriginalSavePayloadEnvelope50KG> envelope(std::move(owned));
        if(game::OriginalSaveProfileStage50KG::prepare(std::move(envelope),settingsBefore,controlsBefore,flags,next->stage_,d.stage)!=
            game::OriginalSaveProfileStage50KG::Status::ok)return d.status=Status::stageFailure;
        // Both temporary owners retain exactly this read. Unknown payload fields
        // remain opaque; no application owner, action row, sound or flag changes.
        out=std::move(next);return Status::ok;
    }catch(...){return d.status=Status::allocationFailure;}
}
}

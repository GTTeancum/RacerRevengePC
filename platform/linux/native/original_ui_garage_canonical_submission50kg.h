#pragma once
#include "original_ui_native_draw50kg.h"
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_main_particle_material50kg.h"
#include "../../../src/assets/original_standard_material_table50la.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include <memory>
#include <vector>
namespace rr::platform {
class OriginalUiGarageDrawContribution50KG;
class OriginalUiGarageModelResource50KG;
// One genuine canonical-table transaction, owned by a future COMPLETE Garage
// composer. This is not a SourceOwner and cannot construct/seal a source frame.
// The three model families are lowered distinctly; visibility,
// cameras, particles and remaining Page draws are still required elsewhere.
// Native histories reside on actual StandardMaterial passes across frames and
// Back/reentry; neither this per-frame object nor repeated PSG slots own them.
class OriginalUiGarageCanonicalSubmission50KG final {
public:
    ~OriginalUiGarageCanonicalSubmission50KG();
    OriginalUiGarageCanonicalSubmission50KG(const OriginalUiGarageCanonicalSubmission50KG&)=delete;
    OriginalUiGarageCanonicalSubmission50KG& operator=(const OriginalUiGarageCanonicalSubmission50KG&)=delete;
private:
    friend class OriginalUiGarageDrawContribution50KG;
    friend class VehicleWindowGl50DS;
    friend class OriginalUiSourceFrame50KM;
    using Table=assets::OriginalStandardMaterialTable50LA;
    using Map=assets::OriginalStandardMaterialMapBindingView50LA;
    using Model=OriginalUiGarageModelResource50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    OriginalUiGarageCanonicalSubmission50KG() noexcept=default;
    bool initialize(std::shared_ptr<Table>,const assets::OriginalStandardMaterialCollectorReceipt50LA&,
        std::shared_ptr<const assets::OriginalUiImageBank50KG>,const OriginalUiSourceOwner50KM&,
        std::uint64_t frameGeneration,D&)noexcept;
    // Actual origin1C4 and final representative1C4, not a recipe-selected row.
    // The origin's parameters/animation remain reachable through map.origin;
    // they are never replaced by the representative's parameter owner.
    bool stageModelPass(std::shared_ptr<const Model> origin,std::uint32_t materialSlot,std::uint32_t pass,
        std::shared_ptr<const Model> representative,std::uint32_t representativeSlot,
        std::shared_ptr<const assets::OriginalUiGarageModelMaterials50KG> representativePolicy,
        std::uint32_t imageOwner,const OriginalUiMaterialSpanBinding50KG&,
        OriginalUiMaterialSpan50KG&,Map&,D&)noexcept;
    bool stageParticleContexts249610(std::int32_t,const void*,
        const assets::OriginalUiArchive50KG&,const assets::OriginalUiMainParticleMaterial50KG&,
        const std::array<std::uint32_t,2>&,const std::vector<OriginalUiMaterialSpan50KG>&,D&)noexcept;
    bool stageWarning470ScreenPass(OriginalUiGarageDrawContribution50KG&,std::int32_t,std::uint32_t,const void*,
        const assets::OriginalUiArchive50KG&,const assets::OriginalUiMaterial50KG&,
        const assets::OriginalUiNativeMaterial50KG&,const OriginalUiMaterialSpanBinding50KG&,
        OriginalUiMaterialSpan50KG&,Map&,D&)noexcept;
    bool stageWarning470UntexturedScreenPass(OriginalUiGarageDrawContribution50KG&,std::int32_t,std::uint32_t,const void*,
        const assets::OriginalUiArchive50KG&,const assets::OriginalUiMaterial50KG&,
        const assets::OriginalUiNativeUntexturedMaterial50KG&,const OriginalUiUntexturedMaterialSpanBinding50KG&,
        OriginalUiMaterialSpan50KG&,Map&,D&)noexcept;
    bool stageScreenPass(std::int32_t,std::uint32_t,const void*,
        const assets::OriginalUiArchive50KG&,const assets::OriginalUiMaterial50KG&,
        const assets::OriginalUiNativeMaterial50KG&,const OriginalUiMaterialSpanBinding50KG&,
        OriginalUiMaterialSpan50KG&,Map&,D&)noexcept;
    bool stageUntexturedScreenPass(std::int32_t,std::uint32_t,const void*,
        const assets::OriginalUiArchive50KG&,const assets::OriginalUiMaterial50KG&,
        const assets::OriginalUiNativeUntexturedMaterial50KG&,const OriginalUiUntexturedMaterialSpanBinding50KG&,
        OriginalUiMaterialSpan50KG&,Map&,D&)noexcept;
    bool preflightBindingSet(const OriginalUiSourceOwner50KM*,std::uint64_t,
        const OriginalUiNativeDraw50KG*,const VehicleWindowGl50DS&)const noexcept;
    bool beginGarageBatch(OriginalUiGarageDrawContribution50KG&,const VehicleWindowGl50DS&)noexcept;
    bool preflight(const OriginalUiSourceFrame50KM&,const OriginalUiSourceOwner50KM*,
        std::uint64_t,const OriginalUiNativeDraw50KG*,const VehicleWindowGl50DS&)const noexcept;
    bool begin(const OriginalUiSourceFrame50KM&,const OriginalUiSourceOwner50KM*,
        std::uint64_t,const OriginalUiNativeDraw50KG*,const VehicleWindowGl50DS&)noexcept;
    bool finish(std::uint64_t generation,bool accepted,bool sourceWorkStarted)noexcept;
    bool authenticateImage(const Map&,std::uint32_t,assets::OriginalStandardMaterialNativeBinding50KG&,D&)const noexcept;
    void unpin()noexcept;
    struct Contribution {
        std::shared_ptr<const Model> origin,representative;
        std::shared_ptr<const assets::OriginalUiGarageModelMaterials50KG> policy;
        std::uint32_t originSlot=0,representativeSlot=0;
        OriginalUiMaterialSpan50KG span{};
        const void* screenReceiver=nullptr;
    };
    std::shared_ptr<Table> table_;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank_;
    assets::OriginalStandardMaterialCollectorReceipt50LA receipt_{};
    std::vector<Contribution> contributions_;
    const void* particleReceiver_=nullptr;
    std::vector<OriginalUiMaterialSpan50KG> particleSpans_;
    std::vector<Map> maps_;
    std::vector<assets::OriginalStandardMaterialNativeBinding50KG> images_;
    const OriginalUiSourceOwner50KM* owner_=nullptr;
    const OriginalUiSourceFrame50KM* frame_=nullptr;
    const VehicleWindowGl50DS* renderer_=nullptr;
    std::uint64_t generation_=0;
    bool pinned_=false,attempted_=false,pending_=false,terminal_=false;
};
}

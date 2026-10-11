#pragma once
#include "original_ui_rider_character50kg.h"
#include "original_ui_user_rotate50kg.h"
#include "original_ui_rider_binder50kg.h"
#include "original_ui_rider_material_lifetime50kg.h"
#include "model_palette49s.h"
#include "../assets/original_ui_rider_pod_assets50kg.h"
#include <memory>

namespace rr::game {
enum class OriginalUiRiderModelError50KG : std::uint8_t {
    none,alreadyPrepared,asset,graph,binding,pose,phase,playback,ik,palette,binder,allocation,lifetime
};
struct OriginalUiRiderModelDiagnostic50KG {
    OriginalUiRiderModelError50KG error=OriginalUiRiderModelError50KG::none;
    std::uint32_t sourcePC=0;
    ModelPlaybackDiagnostic50KG playback{};
    ChariotIkDiagnostic49T ik{};
    ModelPaletteDiagnostic49S palette{};
    OriginalUiRiderMaterialLifetimeDiagnostic50KG lifetime{};
    // A failed cleanup retains the exact domain, including its invalidated
    // phase. The original load/playback error remains the primary diagnostic.
    bool cleanupPending=false;
};
// One real model graph per dynamic widget. PSG nodes retain their original
// hierarchy/order beneath the actual additional model root. No vehicle-part
// scaffolding, synthetic bones or zero-sized fake engine models.
class OriginalUiRiderCharacterScene50KG final {
public:
    OriginalUiRiderCharacterScene50KG() noexcept;
    ~OriginalUiRiderCharacterScene50KG();
    OriginalUiRiderCharacterScene50KG(const OriginalUiRiderCharacterScene50KG&)=delete;
    OriginalUiRiderCharacterScene50KG& operator=(const OriginalUiRiderCharacterScene50KG&)=delete;
    bool beginLoad(std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG>,
        ModelMotionSharedState49T&,ModelMode0IkShared50KG&,OriginalUiRiderMaterialLifetime50KG&,
        const OriginalUiRiderMaterialFootprint50KG&,OriginalUiRiderModelDiagnostic50KG&) noexcept;
    // Call at actual observer-attachment + parent callback completion boundary.
    // Captures155730, constructs optional l_IK/r_IK chains in source order from
    // sampled graph, binds endpoints, installs real mode0 solver, sets IK204.
    bool finishAttachment(SourceLibmState49T&,OriginalUiRiderModelDiagnostic50KG&) noexcept;
    // Source widget survives model destruction. Retain idle counter/last idle and
    // ready across real unload/reload, rather than constructing a fresh widget.
    // Only after successful backend drain/detach with completion entitlement.
    // The exact domain survives false; this does not acquire that entitlement.
    bool modelDestroyedAfterDetach(OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool requiresModelCleanup() const noexcept;
    bool reload(ModelMotionSharedState49T&,ModelMode0IkShared50KG&,
        OriginalUiRiderMaterialLifetime50KG&,const OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool attached() const noexcept;
    void selected() noexcept;
    bool confirm(OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool advance(float elapsedSeconds,CoupledNoiseContext49M&,ModelMotionSharedState49T&,
        ModelMode0IkShared50KG&,OriginalUiRiderModelDiagnostic50KG&) noexcept;
    // Source modelFlags191 selection is supplied by the actual renderer owner.
    bool projectPalette(std::uint8_t modelFlags191,ModelPalette49S&,
        OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool sourceReadiness() const noexcept;
    const assets::Scene48D* scene() const noexcept;
    SceneGraph49P graph() noexcept;
    const OriginalUiRiderCharacter50KG* lifecycle() const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
class OriginalUiRiderPodScene50KG final {
public:
    OriginalUiRiderPodScene50KG() noexcept;
    ~OriginalUiRiderPodScene50KG();
    OriginalUiRiderPodScene50KG(const OriginalUiRiderPodScene50KG&)=delete;
    OriginalUiRiderPodScene50KG& operator=(const OriginalUiRiderPodScene50KG&)=delete;
    bool load(std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG>,
        OriginalUiRiderMaterialLifetime50KG&,const OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderModelDiagnostic50KG&) noexcept;
    // Only after successful backend drain/detach with completion entitlement.
    // The exact domain survives false; this does not acquire that entitlement.
    bool modelDestroyedAfterDetach(OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool requiresModelCleanup() const noexcept;
    bool reload(OriginalUiRiderMaterialLifetime50KG&,const OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    // Rider direction choreography captures BEFORE outgoing deselection, then
    // restores AFTER final incoming SELECT has loaded and attached this pod.
    // Only a copied value crosses those graph lifetimes. Caller must establish
    // actual attachment/selection and drain outstanding graph/packet borrows;
    // these operations neither detach owners nor alter cache membership.
    bool captureDirectionalOrientation(OriginalUiRiderPodOrientation50KG&,
        OriginalUiRiderModelDiagnostic50KG&) const noexcept;
    bool restoreDirectionalOrientation(const OriginalUiRiderPodOrientation50KG&,
        OriginalUiRiderModelDiagnostic50KG&) noexcept;
    void selected() noexcept;void deselected() noexcept;
    bool advance(float elapsedSeconds,OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool projectPalette(std::uint8_t modelFlags191,ModelPalette49S&,
        OriginalUiRiderModelDiagnostic50KG&) noexcept;
    bool binderPresent(std::size_t pair) const noexcept;
    bool projectBinder(std::size_t pair,const std::array<float,3>& originalCameraColumn,
        OriginalUiRiderBinderQuad50KG&,OriginalUiRiderModelDiagnostic50KG&) noexcept;
    const assets::Scene48D* scene() const noexcept;
    SceneGraph49P graph() noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}

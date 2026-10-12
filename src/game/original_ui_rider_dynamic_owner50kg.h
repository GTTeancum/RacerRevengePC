// NEW unverified source reconstruction: read-only Confirm preflight.
#pragma once
#include "original_ui_rider_widgets50kg.h"
#include "original_ui_rider_model_scene50kg.h"
namespace rr::platform {class OriginalUiRiderSession50KG;}
namespace rr::game {
// Immutable descriptor population only. Neither value authorizes source
// selection, graph creation, material registration or a destination change.
enum class OriginalUiRiderColdPopulation50KG : std::uint8_t {selected19,rider19And20,defaultUnlocked};
class OriginalUiRiderDynamicOwner50KG;
struct OriginalUiRiderDynamicBackend50KG {
    void* context=nullptr;
    // Cold resources and bank are sealed before this call. Validate the whole
    // ordered batch without allocating or changing resource publication.
    bool (*preflight)(void*,const OriginalUiRiderDynamicOwner50KG&,
        const OriginalUiRiderWidgetCallback50KG*,std::size_t) noexcept=nullptr;
    // Actual entity-mask2 attachment +28E8A0 visibility/cache membership,
    // optional real binders with entity mask100A, then parent+A4 resource synchronization.
    // Character's entry.001 sample already exists; capture/IK follows success.
    bool (*attach)(void*,std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept=nullptr;
    // Must drain borrowed packets, disable/remove actual model and binder
    // registrations, remove28E8A0 membership, detach entities. Called even
    // after failed partial attach. False preserves all remaining owners and
    // forbids graph destruction; caller may perform explicit cleanup later.
    bool (*drainAndDetach)(void*,std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept=nullptr;
    // Actual141630/1415A0 recursive enable/disable, including attached children.
    // Valid even when the dynamic widget has no loaded model.
    bool (*setShown)(void*,std::uint16_t,bool) noexcept=nullptr;
    // Stable backend ledger plus privately sealed graph provenance. Accessors
    // cannot acquire owners or widen the admitted runtime population.
    OriginalUiRiderMaterialLifetime50KG* (*materialLifetime)(void*) noexcept=nullptr;
    const OriginalUiRiderMaterialFootprint50KG* (*graphFootprint)(void*,std::uint16_t) noexcept=nullptr;
};
enum class OriginalUiRiderDynamicError50KG : std::uint8_t {
    none,alreadyBound,input,parse,cold,model,backend,poisoned,allocation
};
enum class OriginalUiRiderDynamicCleanupPhase50KG : std::uint8_t {none,callbackActive,detach,graphRelease};
struct OriginalUiRiderDynamicDiagnostic50KG {
    OriginalUiRiderDynamicError50KG error=OriginalUiRiderDynamicError50KG::none;
    std::uint16_t widget=0;std::uint32_t sourcePC=0;
    assets::OriginalUiPageDiagnostic50KG parse;
    assets::OriginalUiRiderCharacterAssetDiagnostic50KG character;
    assets::OriginalUiRiderPodDiagnostic50KG pod;
    OriginalUiRiderModelDiagnostic50KG model;
    // Cleanup failure is secondary to the original callback/load failure.
    bool cleanupPending=false;
    std::uint16_t cleanupWidget=0;
    OriginalUiRiderDynamicCleanupPhase50KG cleanupPhase=OriginalUiRiderDynamicCleanupPhase50KG::none;
    OriginalUiRiderModelDiagnostic50KG cleanupModel;
};
// Concrete dynamic callbacks for the shared119-widget population. The backend
// remains an explicit REQUIRED live renderer/entity/cache owner, never a success
// stub. Widget phase/navigation and original profile effects remain caller-owned.
// All shared motion/IK/libm/RNG and backend context must outlive this owner.
// Destroy/drain this owner before its backend or source widget manager.
class OriginalUiRiderDynamicOwner50KG final {
public:
    OriginalUiRiderDynamicOwner50KG() noexcept;~OriginalUiRiderDynamicOwner50KG();
    OriginalUiRiderDynamicOwner50KG(const OriginalUiRiderDynamicOwner50KG&)=delete;
    OriginalUiRiderDynamicOwner50KG& operator=(const OriginalUiRiderDynamicOwner50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,const OriginalUiRiderWidgets50KG&,
        ModelMotionSharedState49T&,ModelMode0IkShared50KG&,SourceLibmState49T&,
        CoupledNoiseContext49M&,const OriginalUiRiderDynamicBackend50KG&,
        OriginalUiRiderDynamicDiagnostic50KG&) noexcept;
    OriginalUiRiderDynamicHooks50KG hooks() noexcept;
    const OriginalUiRiderDynamicDiagnostic50KG& diagnostic() const noexcept;
    bool poisoned() const noexcept;
    std::uint16_t podWidget(std::uint32_t rider) const noexcept;
    std::uint16_t characterWidget(std::uint32_t rider) const noexcept;
    OriginalUiRiderWidgetState50KG state(std::uint16_t) const noexcept;
    bool drain() noexcept;
    // Invoked in actual widget traversal order, after ancestor enable gates.
    // Caller continues child traversal afterwards. Does not invent a wall clock.
    bool advanceWidget(std::uint16_t,float elapsedSeconds,bool ancestorsShown) noexcept;
    bool preflightConfirmCharacter(std::uint16_t,OriginalUiRiderDynamicDiagnostic50KG&) const noexcept;
    bool confirmCharacter(std::uint16_t) noexcept;
    bool sourceCharacterReadiness(std::uint16_t) const noexcept;
    OriginalUiRiderCharacterScene50KG* characterScene(std::uint16_t) noexcept;
    OriginalUiRiderPodScene50KG* podScene(std::uint16_t) noexcept;
    std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> characterAssets(std::uint16_t) const noexcept;
    std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> podAssets(std::uint16_t) const noexcept;
private:
    friend class rr::platform::OriginalUiRiderSession50KG;
    // The session invokes this once while its candidate is still private.
    // Local candidates commit together; no dynamic/source callback is run.
    bool prepareCold(OriginalUiRiderColdPopulation50KG,OriginalUiRiderDynamicDiagnostic50KG&) noexcept;
    bool preflight(const OriginalUiRiderWidgetCallback50KG*,std::size_t) noexcept;
    bool apply(const OriginalUiRiderWidgetCallback50KG&,OriginalUiRiderWidgetState50KG&) noexcept;
    bool loadModel(std::uint16_t) noexcept;
    bool unloadModel(std::uint16_t) noexcept;
    bool shown(std::uint16_t,bool) noexcept;
    bool generic(std::uint16_t,bool) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiRiderDynamicDiagnostic50KG diagnostic_;
};
}

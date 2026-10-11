#pragma once
#include "original_ui_rider_widgets50kg.h"
#include "original_ui_rider_model_scene50kg.h"
namespace rr::game {
class OriginalUiRiderDynamicOwner50KG;
struct OriginalUiRiderDynamicBackend50KG {
    void* context=nullptr;
    // Cold resources exist before this call. Must admit actual material/image
    // bank, scene-list and callback capacity for the COMPLETE ordered batch.
    bool (*preflight)(void*,const OriginalUiRiderDynamicOwner50KG&,
        const OriginalUiRiderWidgetCallback50KG*,std::size_t) noexcept=nullptr;
    // Actual entity-mask2 attachment +28E8A0 visibility/cache membership,
    // optional real binders with entity mask100A, then parent+A4 resource synchronization.
    // Character's entry.001 sample already exists; capture/IK follows success.
    bool (*attach)(void*,std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept=nullptr;
    // Must drain borrowed packets, disable/remove actual model and binder
    // registrations, remove28E8A0 membership, detach entities. Called even
    // after failed partial attach. Must finish cleanup before returning.
    void (*drainAndDetach)(void*,std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept=nullptr;
    // Actual141630/1415A0 recursive enable/disable, including attached children.
    // Valid even when the dynamic widget has no loaded model.
    bool (*setShown)(void*,std::uint16_t,bool) noexcept=nullptr;
};
enum class OriginalUiRiderDynamicError50KG : std::uint8_t {
    none,alreadyBound,input,parse,cold,model,backend,poisoned,allocation
};
struct OriginalUiRiderDynamicDiagnostic50KG {
    OriginalUiRiderDynamicError50KG error=OriginalUiRiderDynamicError50KG::none;
    std::uint16_t widget=0;std::uint32_t sourcePC=0;
    assets::OriginalUiPageDiagnostic50KG parse;
    assets::OriginalUiRiderCharacterAssetDiagnostic50KG character;
    assets::OriginalUiRiderPodDiagnostic50KG pod;
    OriginalUiRiderModelDiagnostic50KG model;
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
    OriginalUiRiderWidgetState50KG state(std::uint16_t) const noexcept;
    void drain() noexcept;
    // Invoked in actual widget traversal order, after ancestor enable gates.
    // Caller continues child traversal afterwards. Does not invent a wall clock.
    bool advanceWidget(std::uint16_t,float elapsedSeconds,bool ancestorsShown) noexcept;
    bool confirmCharacter(std::uint16_t) noexcept;
    bool sourceCharacterReadiness(std::uint16_t) const noexcept;
    OriginalUiRiderCharacterScene50KG* characterScene(std::uint16_t) noexcept;
    OriginalUiRiderPodScene50KG* podScene(std::uint16_t) noexcept;
    std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> characterAssets(std::uint16_t) const noexcept;
    std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> podAssets(std::uint16_t) const noexcept;
private:
    bool preflight(const OriginalUiRiderWidgetCallback50KG*,std::size_t) noexcept;
    bool apply(const OriginalUiRiderWidgetCallback50KG&,OriginalUiRiderWidgetState50KG&) noexcept;
    bool loadModel(std::uint16_t) noexcept;
    void unloadModel(std::uint16_t) noexcept;
    bool shown(std::uint16_t,bool) noexcept;
    bool generic(std::uint16_t,bool) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiRiderDynamicDiagnostic50KG diagnostic_;
};
}

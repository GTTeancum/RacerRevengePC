// NEW unverified source reconstruction: read-only Confirm preflight.
#pragma once
#include "original_ui_rider_widgets50kg.h"
#include "original_ui_rider_model_scene50kg.h"
namespace rr::platform {class OriginalUiRiderSession50KG;}
namespace rr::game {
// Immutable descriptor population only. No value authorizes source
// selection, graph creation, material registration or a destination change.
enum class OriginalUiRiderColdPopulation50KG : std::uint8_t {
    selected19,rider19And20,defaultUnlocked,defaultUnlockedAndGasgano,
    defaultUnlockedAndCleggPreview,defaultUnlockedAndGasganoAndCleggPreview
};
// Additive immutable resources only; never unlocks or selects a rider.
enum class OriginalUiRiderPreviewSet50KG : std::uint8_t {none,indexed23And24,gasgano7,mawhonic9};
// Original rider.dat identities only. Bit N requests rider N's complete pair.
// A request conveys no unlock, selection, Confirm, or decoder admission.
inline constexpr std::uint32_t originalUiRiderPreviewMask50KG=
    (1u<<1)|(1u<<3)|(1u<<7)|(1u<<8)|(1u<<9)|(1u<<14)|(1u<<16)|(1u<<17)|
    (1u<<18)|(1u<<21)|(1u<<22)|(1u<<23)|(1u<<24)|(1u<<25)|(1u<<26);
inline constexpr std::uint32_t originalUiRiderRoster50KG[]{
    1,2,3,4,7,8,9,10,11,12,14,15,16,17,18,19,20,21,22,23,24,25,26};
inline constexpr bool originalUiRiderRequested50KG(std::uint32_t mask,std::uint32_t rider) noexcept {
    return rider<32&&(mask&(std::uint32_t{1}<<rider))!=0;
}
inline bool originalUiRiderPreviewRequest50KG(OriginalUiRiderPreviewSet50KG legacy,
    std::uint32_t requested,std::uint32_t& combined) noexcept {
    combined=0;if(requested&~originalUiRiderPreviewMask50KG)return false;
    switch(legacy){
    case OriginalUiRiderPreviewSet50KG::none:break;
    case OriginalUiRiderPreviewSet50KG::indexed23And24:requested|=(1u<<23)|(1u<<24);break;
    case OriginalUiRiderPreviewSet50KG::gasgano7:requested|=1u<<7;break;
    case OriginalUiRiderPreviewSet50KG::mawhonic9:requested|=1u<<9;break;
    default:return false;
    }
    combined=requested;return true;
}
// PORT STAND-IN (flagged, B-P05): the cold population names riders (19/20,
// 3, 7 and the default-unlocked set) instead of reading the profile's unlock
// bits; the session (original_ui_rider_session50kg.cpp) must pass the mask.
inline bool originalUiRiderColdMember50KG(OriginalUiRiderColdPopulation50KG population,
    std::uint32_t requested,std::uint32_t rider) noexcept {
    if(originalUiRiderRequested50KG(requested,rider))return true;
    if(population==OriginalUiRiderColdPopulation50KG::selected19)return rider==19;
    if(population==OriginalUiRiderColdPopulation50KG::rider19And20)return rider==19||rider==20;
    if(rider==3)return population==OriginalUiRiderColdPopulation50KG::defaultUnlockedAndCleggPreview||
        population==OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview;
    if(rider==7)return population==OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasgano||
        population==OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview;
    return rider==2||rider==4||rider==10||rider==11||rider==12||rider==15||rider==19||rider==20;
}
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
    bool previewRequested(std::uint32_t rider) const noexcept;
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
    friend class OriginalUiRiderWidgets50KG;
    // Native material-closure projection only. The exact bound widget owner and
    // healthy actual entries supply provenance; copied state/registered values
    // do not authorize callbacks, attachment, material acquisition or rendering.
    bool nativeProjectionHealthy() const noexcept;
    bool nativeProjectionForWidgets(const OriginalUiRiderWidgets50KG&) const noexcept;
    bool nativeProjectionSnapshot(std::uint16_t,OriginalUiRiderWidgetState50KG&,
        bool& registered) const noexcept;
    bool projectNativeOne(const OriginalUiRiderWidgetCallback50KG&,
        OriginalUiRiderWidgetState50KG&,bool& registered) const noexcept;
    bool matchesNativeProjection(std::uint16_t,const OriginalUiRiderWidgetState50KG&,
        bool registered) const noexcept;
    // Pure descriptor semantics shared with the real callback's expected-result
    // check. Deliberately has no global-health gate on real apply/cleanup paths.
    bool projectNativeDescriptorOne(const OriginalUiRiderWidgetCallback50KG&,
        OriginalUiRiderWidgetState50KG&,bool& registered) const noexcept;
    // The session invokes this once while its candidate is still private.
    // Local candidates commit together; no dynamic/source callback is run.
    bool prepareCold(OriginalUiRiderColdPopulation50KG,OriginalUiRiderDynamicDiagnostic50KG&,
        OriginalUiRiderPreviewSet50KG = OriginalUiRiderPreviewSet50KG::none,
        std::uint32_t requestedPreviewRiders=0) noexcept;
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

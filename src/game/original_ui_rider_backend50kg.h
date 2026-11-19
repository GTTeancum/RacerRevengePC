#pragma once
#include "original_ui_rider_dynamic_owner50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_ui_rider_binder_animation50kg.h"
#include "original_ui_rider_material_lifetime50kg.h"
#include "original_ui_tournament_predecessor50kg.h"
#include "../assets/original_ui_image_bank50kg.h"
#include "../assets/original_ui_material_binding50kg.h"
#include "../assets/original_ui_rider_model_materials50kg.h"
#include "original_ui_rider_addon_models50dl.h"
#include <string>
#include <vector>
namespace rr::platform {class OriginalUiRiderDrawContribution50KG;class OriginalUiTournamentDrawContribution50KG;}
namespace rr::game {
struct OriginalUiRiderBackendInput50KG {
    std::shared_ptr<const assets::OriginalUiArchive50KG> rider,appStart,podTextures;
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binder;
    assets::OriginalUiMaterialRuntime50KG materialRuntime;
    assets::OriginalUiImageBankInput50KG staticImages;
    // Canonical rows of the actual retained Rider widgets/stat assets and
    // predecessor Main records. An inventory is not a static live-owner token.
    std::vector<OriginalUiRiderMaterialRow50KG> staticRows;
    // Complete real Tournament predecessor evidence stays distinct from the
    // current Rider static population. Overlap authenticates immutable source
    // definitions only; a private same-owner retirement is required to attach.
    std::shared_ptr<const OriginalUiTournamentPredecessorInventory50KG> tournamentPredecessor;
    // PORT EXTENSION, NOT ORIGINAL (DLC): the session's add-on pod model table.
    // Its ready rows' images are appended to the bank AFTER every stock owner, so
    // stock owner indices and the stock bank are unchanged. nullptr = none.
    OriginalUiRiderAddonModels50DL* addonModels=nullptr;
};
enum class OriginalUiRiderBackendError50KG : std::uint8_t {
    none,input,alreadyBound,material,bank,membership,binder,busy,lifetime,completion,allocation
};
struct OriginalUiRiderBackendDiagnostic50KG {
    OriginalUiRiderBackendError50KG error=OriginalUiRiderBackendError50KG::none;
    std::uint16_t widget=0;
    assets::OriginalUiRiderModelMaterialDiagnostic50KG material;
    assets::OriginalUiImageBankDiagnostic50KG bank;
    OriginalUiRiderVisibilityDiagnostic50KG membership;
    OriginalUiRiderMaterialLifetimeDiagnostic50KG lifetime;
};
// A fresh mutable native PSM semantic instance, constructed in table/pass order
// by the private parent+A4 issuer. Immutable descriptor aliases never mint it.
// Consumers may stage copies; only accepted source submission commits states.
class OriginalUiRiderLiveMaterial50KG final {
public:
    OriginalUiRiderLiveMaterial50KG()=default;
    ~OriginalUiRiderLiveMaterial50KG()=default;
    OriginalUiRiderLiveMaterial50KG(const OriginalUiRiderLiveMaterial50KG&)=delete;
    OriginalUiRiderLiveMaterial50KG& operator=(const OriginalUiRiderLiveMaterial50KG&)=delete;
private:
    friend class OriginalUiRiderBackend50KG;
    friend class OriginalUiTournamentPodOwner50KG;
    friend class rr::platform::OriginalUiTournamentDrawContribution50KG;
    friend class rr::platform::OriginalUiRiderDrawContribution50KG;
    const OriginalUiRiderMaterialLifetime50KG* ledger=nullptr;
    OriginalUiRiderMaterialRow50KG row{};
    std::uint64_t epoch=0;
    std::vector<const assets::OriginalUiMaterialPass50KG*> sources;
    std::vector<assets::OriginalUiMaterialBindingState50KG> states;
};
// Authenticated prepared graph/helper ownership with renderer-first retirement. Source
// mutation is synchronous and nonreentrant. Session admission still owns the
// complete direction/appearance/deferred/audio transaction; cold data alone
// never authorizes callbacks or replacement of a live source generation.
class OriginalUiRiderBackend50KG final {
public:
    OriginalUiRiderBackend50KG() noexcept;~OriginalUiRiderBackend50KG();
    OriginalUiRiderBackend50KG(const OriginalUiRiderBackend50KG&)=delete;
    OriginalUiRiderBackend50KG& operator=(const OriginalUiRiderBackend50KG&)=delete;
    bool bind(const OriginalUiRiderBackendInput50KG&,OriginalUiRiderBackendDiagnostic50KG&) noexcept;
    OriginalUiRiderDynamicBackend50KG callbacks() noexcept;
    const OriginalUiRiderBackendDiagnostic50KG& diagnostic() const noexcept;
    OriginalUiRiderVisibilityOwner50KG* visibility() noexcept;
    std::shared_ptr<const assets::OriginalUiArchive50KG> riderArchive()const noexcept;
    bool preparedRider(std::uint32_t rider) const noexcept;
    bool beginFrame() noexcept;
    void endFrame() noexcept;
    std::uint64_t generation() const noexcept;
    std::uint64_t registryGeneration() const noexcept;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank() const noexcept;
    const assets::OriginalUiImageBankMapping50KG* staticImage(std::uint32_t,std::uint32_t) const noexcept;
    const assets::OriginalUiImageBankMapping50KG* modelImage(std::uint16_t,std::uint32_t) const noexcept;
    const assets::OriginalUiImageBankMapping50KG* binderImage(std::uint32_t) const noexcept;
    // PORT EXTENSION (DLC): image `image` of add-on model table row `row`.
    const assets::OriginalUiImageBankMapping50KG* addonImage(std::size_t row,std::uint32_t image) const noexcept;
    const assets::OriginalUiImageBankMapping50KG* addonCharacterImage(std::size_t row,std::uint32_t image) const noexcept;
    const OriginalUiRiderAddonModels50DL* addonModels() const noexcept;
    const assets::OriginalUiRiderModelMaterials50KG* materials(std::uint16_t) const noexcept;
    const OriginalUiRiderVisibilityHandle50KG* membership(std::uint16_t) const noexcept;
    const OriginalUiRiderMaterialLifetime50KG& materialLifetime()const noexcept;
    bool canonicalMaterialRow(const std::string&,OriginalUiRiderMaterialRow50KG&)const noexcept;
    std::shared_ptr<OriginalUiRiderLiveMaterial50KG> liveMaterial(const std::string&)const noexcept;
    std::shared_ptr<OriginalUiRiderLiveMaterial50KG> materialInstance(const OriginalUiRiderMaterialRow50KG&,std::uint64_t)const noexcept;
    bool attached(std::uint16_t) const noexcept;
    bool shown(std::uint16_t) const noexcept;
    OriginalUiRiderBinderAnimation50KG* binder(std::uint16_t,std::uint8_t pair) noexcept;
private:
    friend class rr::platform::OriginalUiRiderSession50KG;
    friend class rr::platform::OriginalUiRiderDrawContribution50KG;
    bool prepareColdAndSeal(const OriginalUiRiderDynamicOwner50KG&,OriginalUiRiderBackendDiagnostic50KG&) noexcept;
    bool acceptTournamentPredecessorRetirement(OriginalUiTournamentPredecessorRetirement50KG&,
        OriginalUiRiderBackendDiagnostic50KG&) noexcept;
    bool predecessorRetired() const noexcept;
    bool bindMaterialConsumer(const rr::platform::OriginalUiRiderDrawContribution50KG*,void*,bool(*)(void*,bool) noexcept) noexcept;
    bool sourceSubmissionStarted(const rr::platform::OriginalUiRiderDrawContribution50KG*,std::uint64_t) noexcept;
    // Called only by the session after the exact renderer's real retirement.
    // Owner, attempted source generation and pre-retirement event frontier must
    // still match. No public caller-provided completion/readiness flag exists.
    bool acceptSourceRetirement(const rr::platform::OriginalUiRiderDrawContribution50KG*,std::uint64_t,std::uint64_t) noexcept;
    bool cleanupEntitled()const noexcept;
    bool finalizeMaterialEvents(std::uint64_t) noexcept;
    bool finalizeTeardown(std::uint64_t,OriginalUiRiderBackendDiagnostic50KG&) noexcept;
    bool synchronizeMaterials() noexcept;
    const OriginalUiRiderMaterialFootprint50KG* graphFootprint(std::uint16_t)const noexcept;
    bool preflight(const OriginalUiRiderDynamicOwner50KG&,const OriginalUiRiderWidgetCallback50KG*,std::size_t) noexcept;
    bool attach(std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept;
    bool detach(std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept;
    bool setShown(std::uint16_t,bool) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiRiderBackendDiagnostic50KG diagnostic_;
};
}

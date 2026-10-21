#pragma once
#include "original_ui_rider_backend50kg.h"
#include "original_application_animation_clock50kg.h"
#include "original_ui_rider_model_packet50kg.h"
#include "original_ui_lit_geometry50km.h"
namespace rr::platform {
class OriginalUiTournamentSession50KG;
class OriginalUiTournamentDrawContribution50KG;
}
namespace rr::game {
class OriginalUiTournamentCameraOwner50KG;
struct OriginalUiTournamentCameraFrame50KG;
struct OriginalUiTournamentPodInput50KG {
    // Aliasing shared_ptr to the actual retained career resource is permitted.
    std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> pod;
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,podTextures;
    assets::OriginalUiMaterialRuntime50KG runtime{};
    const assets::OriginalUiIpumTextureCache50KG* ipumCache=nullptr;
    // Actual CURRENT Tournament static row inventory, not predecessor rows.
    // The session retires the complete predecessor before attachSelected.
    std::vector<OriginalUiRiderMaterialRow50KG> currentStaticRows;
};
enum class OriginalUiTournamentPodError50KG:std::uint8_t {
    none,input,alreadyBound,material,binder,lifetime,membership,model,phase,completion,camera,allocation
};
struct OriginalUiTournamentPodDiagnostic50KG {
    OriginalUiTournamentPodError50KG error=OriginalUiTournamentPodError50KG::none;
    std::uint32_t sourcePC=0;
    assets::OriginalUiRiderModelMaterialDiagnostic50KG material{};
    assets::OriginalUiRiderBinderAssetDiagnostic50KG binder{};
    OriginalUiRiderMaterialLifetimeDiagnostic50KG lifetime{};
    OriginalUiRiderVisibilityDiagnostic50KG visibility{};
    OriginalUiRiderModelDiagnostic50KG model{};
};
struct OriginalUiTournamentPodPacket50KG {
    OriginalUiRiderModelSelection50KG selection{};
    OriginalUiRiderModelPacket50KG model{};
    bool emitsGeometry=false;
};
struct OriginalUiTournamentBinderPacket50KG {
    OriginalUiRiderBinderFrame50KG image{};
    OriginalUiLitBinderPacket50KM geometry{};
};
// Native User_Rotate ownership for the actual selected Tournament widget.
// Cold assets never mint a graph, live material epoch, membership or render
// receipt. Session attachment is after the exact predecessor's real teardown;
// destruction is after this collector's actual renderer retirement.
class OriginalUiTournamentPodOwner50KG final {
public:
    OriginalUiTournamentPodOwner50KG() noexcept;
    ~OriginalUiTournamentPodOwner50KG();
    OriginalUiTournamentPodOwner50KG(const OriginalUiTournamentPodOwner50KG&)=delete;
    OriginalUiTournamentPodOwner50KG& operator=(const OriginalUiTournamentPodOwner50KG&)=delete;
    bool prepareCold(const OriginalUiTournamentPodInput50KG&,OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> podAssets()const noexcept;
    std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG> materials()const noexcept;
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binderAssets()const noexcept;
    const assets::OriginalUiImageBankInput50KG* bankInput()const noexcept;
    const OriginalUiRiderMaterialLifetime50KG& materialLifetime()const noexcept;
    std::uint64_t registryGeneration()const noexcept;
    bool canonicalMaterialRow(const std::string&,OriginalUiRiderMaterialRow50KG&)const noexcept;
    std::shared_ptr<OriginalUiRiderLiveMaterial50KG> materialInstance(const OriginalUiRiderMaterialRow50KG&,std::uint64_t)const noexcept;
    std::shared_ptr<OriginalUiRiderLiveMaterial50KG> liveMaterial(const std::string&)const noexcept;
    bool attached()const noexcept;
    bool shown()const noexcept;
    // Observation only: never authorizes a different owner's retirement.
    bool teardownComplete()const noexcept;
    std::uint16_t widgetId()const noexcept;
    bool binderPresent(std::size_t pair)const noexcept;
private:
    friend class rr::platform::OriginalUiTournamentSession50KG;
    friend class rr::platform::OriginalUiTournamentDrawContribution50KG;
    bool bindMaterialConsumer(const rr::platform::OriginalUiTournamentDrawContribution50KG*,void*,bool(*)(void*,bool) noexcept) noexcept;
    // Source1D33C0 SELECT: generic state before/after 1D3590 is owned by
    // the session's real widget callback, not synthesized by this model owner.
    bool attachSelected(OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    bool setShown(bool) noexcept;
    // Actual inherited f12 from the Session's prepared source update.
    // Application caps f20 before root slot38; raw Application250 is NOT this delta.
    // Only the retained Session may issue the next one-use update sequence.
    bool advance(float inheritedDelta,std::uint64_t sessionUpdateSequence,OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    bool beginFrame() noexcept;
    void endFrame() noexcept;
    bool prepareSelected(const OriginalUiTournamentCameraOwner50KG&,const OriginalUiTournamentCameraFrame50KG&,
        OriginalUiTournamentPodPacket50KG&,OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    // Each actual helper runs geometry then unique animation, even when its
    // parent graphics model is culled; no helper runs for a hidden widget.
    bool projectBinder(std::size_t,const OriginalUiTournamentCameraOwner50KG&,const OriginalUiTournamentCameraFrame50KG&,
        std::uint32_t sourceMilliseconds,bool advanceAnimations,OriginalUiTournamentBinderPacket50KG&,
        OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    bool sourceSubmissionStarted(const rr::platform::OriginalUiTournamentDrawContribution50KG*,std::uint64_t) noexcept;
    bool acceptSourceRetirement(const rr::platform::OriginalUiTournamentDrawContribution50KG*,std::uint64_t,std::uint64_t) noexcept;
    bool detachAfterRetirement(OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    bool finalizeTeardown(std::uint64_t consumed,OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    bool cleanupEntitled()const noexcept;
    bool synchronizeMaterials(OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    bool finalizeMaterialEvents(std::uint64_t,OriginalUiTournamentPodDiagnostic50KG&) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}

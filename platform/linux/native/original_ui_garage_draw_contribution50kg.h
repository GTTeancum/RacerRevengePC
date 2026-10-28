#pragma once
#include "original_ui_garage_particle_packet_geometry50kg.h"
#include "../../../src/assets/original_ui_main_particle_material50kg.h"
#include "vehicle_window_gl50ds.h"
#include "original_ui_native_draw50kg.h"
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_garage_model_materials50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/assets/original_standard_material_table50la.h"
#include "original_ui_garage_model_lowering50kg.h"
#include "original_ui_garage_owned_model_geometry50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include "../../../src/game/original_frontend_camera_cold50la.h"
#include "../../../tests/native/original_ui_source_timing50lh.h"
#include <array>
#include <memory>
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}
namespace rr::game {class OriginalZoneScissor50LA;}
namespace rr::platform {
class OriginalUiGarageSession50KG;
class OriginalUiSharedDialogs50KG;
class OriginalUiGarageTextOverlay50KG;class OriginalUiGarageScreenText50KG;class OriginalUiGarageWidget1Owner50KG;
struct OriginalUiGarageScreenTextDiagnostic50KG;
class OriginalUiGarageModelResource50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageVisibility50KG;
class OriginalUiGarageSceneParticles50KG;
class OriginalUiGarageDrawContribution50KG;
class OriginalUiGarageCanonicalSubmission50KG;
class OriginalUiFrontend50KG;
class OriginalUiGarageQuad50KG;
// A retained, live source Camera borrow, not an authored camera projection,
// an immutable menu recipe or a complete SourceFrame credential.
class OriginalUiGarageCameraFrame50KG final {
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiGarageVisibility50KG;
    const OriginalUiGarageDrawContribution50KG* cursor_=nullptr;
    explicit OriginalUiGarageCameraFrame50KG(const OriginalUiGarageDrawContribution50KG* p)noexcept:cursor_(p){}
    bool searchInvocation114D28Current(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageVisibility50KG&)const noexcept;
    bool readSearchPublication114D28(std::uint64_t&)const noexcept;
    bool readFinalSearchInvocation2805F0(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageVisibility50KG&,const game::OriginalWreckComponent50LA&,std::uint64_t&)const noexcept;

    struct ModelRenderInputs {
        std::uint32_t viewportX1E0=0,viewportY1E4=0,viewportWidth1E8=0,viewportHeight1EC=0;
        float fogStartB0=0,secondMapDistance382FA0=0;
    };
    bool readRenderInputs114E00(ModelRenderInputs&,std::uint64_t&,std::uint32_t&)const noexcept;
    bool renderInvocation114E00Current(const OriginalUiGaragePageOwner50KG&,const OriginalUiGarageVisibility50KG&)const noexcept;
    bool collectGarageModel241280(OriginalUiGarageModelResource50KG&,assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool publishModelScissor280118(OriginalUiGarageVisibility50KG&,
        std::shared_ptr<game::OriginalZoneScissor50LA>,assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool incrementModelCount26F050()const noexcept;
    bool drainSearchBlock27EC68(VehicleWindowDiagnostic50DS&,std::uint32_t&)const noexcept;
public:
    using Matrix=std::array<float,16>;
    struct Fields {
        Matrix viewC0{},projection80{},screen140{},combined100{},combinedScreen250{},guardProjection290{},guardCombined2D0{};
        std::array<float,4> position180{},look190{},up1A0{};
        float guardX334=0,guardY338=0,near1FC=0,far200=0,fov1D0=0,aspect1F8=0,focal1D8=0;
        std::uint32_t width204=0,height208=0;
    };
    bool current()const noexcept;
    bool read(Fields&,std::uint32_t& pendingPC)const noexcept;
};
class OriginalUiGarageDrawContribution50KG final:public OriginalUiSourceOwner50KM {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiSharedDialogMaterialHistory50KG;
    friend class OriginalUiGarageCameraFrame50KG;
    friend class OriginalUiSharedDialogs50KG;
    friend class OriginalUiGarageSession50KG;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class OriginalUiGarageWidget1Owner50KG;
    friend class OriginalUiGarageTextOverlay50KG;
    friend class OriginalUiGarageScreenText50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageQuad50KG;
    friend class VehicleWindowGl50DS;
    friend class OriginalUiFrontend50KG;
    friend class OriginalUiGarageCanonicalSubmission50KG;
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    using C=game::OriginalWreckComponent50LA;
    using Camera=game::OriginalFrontendCameraCold50LA;
    using Application=::original_main_entry50KG::Application;
    enum class Phase:std::uint8_t {issued,disabling,selecting,firstPass,search114D28,firstPassReturned,viewport,secondPass,secondPassReturned,thirdPass,modelRender114E00,collectorDrain24B7B0,rendererEnd2440E0,finalPass,renderReturned140230,unavailable};
    struct Diagnostic {std::uint32_t sourcePC=0;VehicleWindowDiagnostic50DS renderer;};
    const Application* issuer_=nullptr;
    std::weak_ptr<OriginalUiGarageSession50KG> session_;
    std::shared_ptr<OriginalUiGaragePageOwner50KG> page_;
    std::shared_ptr<OriginalUiGarageRendererLease50KG> renderer_;
    ::original_ui_source_timing50LH::PublicationView timing_;
    std::uint64_t publication_=0;
    C* manager_=nullptr;const C* dispatcher_=nullptr;
    std::shared_ptr<Camera> camera_;
    OriginalUiGarageCameraFrame50KG cameraFrame_{this};
    Phase phase_=Phase::issued;std::uint32_t sourcePC_=0x140230;
    std::size_t disabledSlots_=0,demandedPageQuads_=0;
    C* callback_=nullptr;bool attempted_=false;
    // Strong native lease on the actual shared owner until inherited return
    // or authenticated native failed-frame unwind, never page-Quad authority.
    std::shared_ptr<OriginalUiSharedDialogs50KG> shared470FirstPassOwner_;
    std::uint64_t shared470FirstPassEpoch_=0;
    bool shared470InheritedReturned_=false;
    std::uint64_t shared470FirstPassCompletedEpoch_=0,shared470FirstPassCompletedUpdate_=0;
    std::shared_ptr<OriginalUiSharedDialogs50KG> shared470ThirdPassOwner_;
    std::uint64_t shared470ThirdPassEpoch_=0;
    bool shared470ThirdPassInheritedReturned_=false;
    bool shared470ThirdPassReceiverCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
    bool shared470ThirdPassFailedUnwindCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
    std::shared_ptr<OriginalUiSharedDialogs50KG> shared47COwner_;
    std::uint64_t shared47CEpoch_=0,shared47CDemandPublication_=0;
    bool shared47CReceiverCurrent(const OriginalUiSharedDialogs50KG&,std::uint32_t)const noexcept;
    bool shared47CFailedUnwindCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
    bool shared470FirstPassReceiverCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
    bool shared470FailedUnwindCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
    std::uint8_t textRestoreStep_=0;
    const OriginalUiGarageTextOverlay50KG* activeText_=nullptr;
    const OriginalUiGarageQuad50KG* activePageQuad_=nullptr;
    const OriginalUiGarageScreenText50KG* activePageCostText_=nullptr;
    struct QuadRenderInputs {
        std::uint32_t viewportWidth1E8=0,viewportHeight1EC=0;
        bool sourceQuadEnabled383678=false;std::uint64_t publication=0;
    };
    bool readQuadInputs2DF550(const OriginalUiGarageQuad50KG&,QuadRenderInputs&,std::uint32_t&)const noexcept;
    bool readWarning470QuadInputs2DF550(const OriginalUiSharedDialogs50KG&,const OriginalUiGarageQuad50KG&,QuadRenderInputs&,std::uint32_t&)const noexcept;
    bool readPopup47CQuadInputs2DF550(const OriginalUiSharedDialogs50KG&,const OriginalUiGarageQuad50KG&,QuadRenderInputs&,std::uint32_t&)const noexcept;
    bool collectWarning470Quad246760(OriginalUiSharedDialogs50KG&,OriginalUiGarageQuad50KG&,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool collectPopup47CQuad246760(OriginalUiSharedDialogs50KG&,OriginalUiGarageQuad50KG&,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool quadInvocation2DF550Current(const OriginalUiGarageQuad50KG&)const noexcept;
    bool collectQuadPrimitive246760(OriginalUiGarageQuad50KG&,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool renderReturned140230Current(const Application&)const noexcept;
    bool frontendTextureServicesCurrent(const OriginalUiFrontend50KG&)const noexcept;
    bool frontendTextureRendererCurrent(const VehicleWindowGl50DS&)const noexcept;
    OriginalUiFrontend50KG* textureCoordinator_=nullptr;
    MaterialTextureGlRequest50EB drainTextureRequest_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> drainAppStart_;
    std::shared_ptr<const assets::OriginalUiIpumTextureCache50KG> drainIpum_;
    std::shared_ptr<assets::OriginalUiImageBank50KG> drainImageBank_;
    std::shared_ptr<MaterialTextureGl50EB> drainGpu_;
    std::vector<std::shared_ptr<const assets::OriginalUiGarageModelMaterials50KG>> drainPolicies_;
    std::vector<std::shared_ptr<OriginalUiGarageModelResource50KG>> drainModels_;
    std::vector<MaterialTextureSampler50EB> drainSamplers_;
    std::vector<std::shared_ptr<const assets::OriginalUiScreenAssets50KG>> drainScreenImages_;
    std::vector<std::shared_ptr<const assets::OriginalUiArchive50KG>> drainScreenArchives_;
    std::vector<MaterialTextureSampler50EB> drainScreenSamplers_;
    struct DrainScreenPolicy {
        bool untextured=false;assets::OriginalUiNativeUntexturedMaterial50KG untexturedPolicy;
        std::int32_t registration=-1;std::uint32_t imageOwner=0;
        std::shared_ptr<const assets::OriginalUiArchive50KG> materialArchive;
        assets::OriginalUiMaterial50KG material;
        assets::OriginalUiNativeMaterial50KG policy;
    };
    std::vector<DrainScreenPolicy> drainScreenPolicies_;
    struct DrainParticlePolicy {
        std::int32_t registration=-1,materialRow=-1;
        std::array<std::uint32_t,2> imageOwners{};
        std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
        assets::OriginalUiMainParticleMaterial50KG material;
    };
    std::unique_ptr<DrainParticlePolicy> drainParticlePolicy_;
    std::unique_ptr<OriginalUiGarageParticlePacketGeometry50KG> drainParticleGeometry_;
    std::vector<OriginalUiMaterialSpan50KG> drainParticleSpans_;
    std::vector<OriginalUiSpan50KG> drainParticleRanges_;
    std::vector<OriginalUiNativeSpan50KG> drainParticleStates_;
    bool prepareDrainParticlePolicy24B130(assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool lowerAndBindQueuedParticle24B26C(std::size_t,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool drainParticleRecordCurrent()const noexcept;
    bool drainParticleClippedEmptyCurrent()const noexcept;
    bool warning470DrainReceiverCurrent(const void*,std::int32_t,std::int32_t)const noexcept;
    bool warning470DrainBindingCurrent(const void*,std::int32_t,std::int32_t)const noexcept;
    bool prepareDrainWarning470DimPolicy24B130(const OriginalUiSharedDialogs50KG&,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool prepareDrainPopup47CDimPolicy24B130(const OriginalUiSharedDialogs50KG&,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool prepareDrainAperturePolicy24B130(std::size_t,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool prepareDrainScreenPolicies24B130(assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool lowerAndBindQueuedScreen24B26C(std::size_t,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    std::vector<assets::Vertex48B> drainScreenVertices_;
    std::vector<std::array<float,3>> drainScreenStq_;
    std::vector<float> drainScreenW_,drainScreenFog_;
    bool drainResourcesAttempted_=false,drainResourcesBound_=false;
    bool prepareDrainModelPolicies24B130(assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool bindQueuedModel249610(const OriginalUiGarageOwnedModelGeometry50KG&,OriginalUiMaterialSpan50KG&,
        assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    std::shared_ptr<::original_main_entry50KG::NativeCollectorFifo50LA> fifoForBackendDrain()const noexcept;
    bool prepareDrainResources24B130(assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool releaseDrainResourcesBeforeRetirement(VehicleWindowDiagnostic50DS&)noexcept;


    struct TextRenderInputs {
        std::uint32_t viewportWidth1E8=0,viewportHeight1EC=0;
        float videoDepthMaximum60=0;std::uint64_t publication=0;
    };
    struct ApertureRenderInputs {std::uint32_t viewportWidth1E8=0,viewportHeight1EC=0;std::int32_t videoDepthMaximum60=0;
        bool sourceApertureEnabled383680=false;std::uint64_t publication=0;};
    bool readApertureInputs2E21D0(const OriginalUiGaragePageOwner50KG&,std::size_t,ApertureRenderInputs&,std::uint32_t&)const noexcept;
    bool collectAperturePrimitive246760(OriginalUiGaragePageOwner50KG&,std::size_t,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool readWarning470DimInputs2E21D0(const OriginalUiSharedDialogs50KG&,ApertureRenderInputs&,std::uint32_t&)const noexcept;
    bool readPopup47CDimInputs2E21D0(const OriginalUiSharedDialogs50KG&,ApertureRenderInputs&,std::uint32_t&)const noexcept;
    bool readWarning470TextInputs2E0170(const OriginalUiSharedDialogs50KG&,const OriginalUiGarageScreenText50KG&,TextRenderInputs&,std::uint32_t&)const noexcept;
    bool readPopup47CTextInputs2E0170(const OriginalUiSharedDialogs50KG&,const OriginalUiGarageScreenText50KG&,TextRenderInputs&,std::uint32_t&)const noexcept;
    bool collectWarning470Text246760(OriginalUiSharedDialogs50KG&,OriginalUiGarageScreenText50KG&,std::size_t,OriginalUiGarageScreenTextDiagnostic50KG&)noexcept;
    bool collectPopup47CText246760(OriginalUiSharedDialogs50KG&,OriginalUiGarageScreenText50KG&,std::size_t,OriginalUiGarageScreenTextDiagnostic50KG&)noexcept;
    bool readPageTextInputs2E0170(const OriginalUiGarageScreenText50KG&,TextRenderInputs&,std::uint32_t&)const noexcept;
    bool readTextInputs29D3F0(const OriginalUiGarageTextOverlay50KG&,TextRenderInputs&,std::uint32_t&)const noexcept;
    bool textInvocation29D3F0Current(const OriginalUiGarageTextOverlay50KG&)const noexcept;
    bool restoreTextState29D8E8(const OriginalUiGarageTextOverlay50KG&,std::uint32_t tag,
        std::uint32_t value,std::uint32_t flags,std::uint32_t& pendingPC)noexcept;
    bool screenTextInvocation2E0170Current(const OriginalUiGarageScreenText50KG&)const noexcept;
    bool collectScreenTextPrimitive246760(OriginalUiGarageScreenText50KG&,std::size_t,
        OriginalUiGarageScreenTextDiagnostic50KG&)noexcept;
    bool collectModel241280(OriginalUiGarageModelResource50KG&,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool publishModelScissor280118(OriginalUiGarageVisibility50KG&,
        std::shared_ptr<game::OriginalZoneScissor50LA>,assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    //Actual reached prelude, separate from complete-frame acceptance.
    bool viewportDepthCleared_=false,cameraPhase4Cleared_=false;
    std::array<std::uint32_t,4> viewportWords_{};
    std::array<std::uint32_t,2> sourceXyOffset_{};
    // The frame remains unsealed until every actual family/callback and
    // enclosing render tail has returned. There is deliberately no accessor.
    //One real24B130 batch can be issued without declaring the Root frame complete.
    //Pointers in this view are retained by the drain-owned geometry/material vectors.
    std::unique_ptr<OriginalUiGarageOwnedModelGeometry50KG> drainActiveGeometry_;
    std::size_t drainActiveRecordOrdinal_=0;
    std::uint32_t drainCommittedQuads_=0;
    OriginalUiMaterialSpan50KG drainActiveSpan_;
    OriginalUiGarageModelLowering50KG::Context drainContext_;
    bool drainContextCaptured_=false;
    bool drainModelClippedEmptyCurrent()const noexcept {
        return drainActiveGeometry_&&drainActiveGeometry_->quadCount==0&&
            drainActiveGeometry_->publication==publication_&&drainActiveGeometry_->epoch==drainEpoch_&&
            drainActiveGeometry_->recordOrdinal==drainActiveRecordOrdinal_&&
            drainActiveGeometry_->sourceTriangleCount&&drainActiveGeometry_->sourceTriangleCount==drainActiveGeometry_->clippedTriangleCount;
    }
    std::vector<std::size_t> drainOrder_;
    std::size_t drainCursor_=0;std::uint32_t drainEpoch_=0;
    OriginalUiNativeDraw50KG drainBatchNative_;
    std::shared_ptr<OriginalUiGarageCanonicalSubmission50KG> drainBatchCanonical_;
    bool drainBatchPending_=false,drainBatchSubmitted_=false,drainBatchFailed_=false;
    OriginalUiNativeDraw50KG native_;
    OriginalUiSourceFrame50KM frame_;
    OriginalUiGarageDrawContribution50KG(const Application&,const std::shared_ptr<OriginalUiGarageSession50KG>&,
        std::shared_ptr<OriginalUiGarageRendererLease50KG>,::original_ui_source_timing50LH::PublicationView,
        std::uint64_t,C*)noexcept;
    bool identityCurrent()const noexcept;
    std::shared_ptr<OriginalUiGarageDrawContribution50KG> retainSelfForFifo()const noexcept;
    bool fail(Diagnostic&,std::uint32_t)noexcept;
    bool inheritedAuralRenderReceiverCurrent(const C&)const noexcept;
    bool invokeSharedDialogRender(C&,std::uint32_t,bool&,Diagnostic&)noexcept;
    bool rejectRenderCallback(Diagnostic&,std::uint32_t,const char*,const C&)noexcept;
    bool select26F940(Diagnostic&)noexcept;
    bool enable1156E0(Diagnostic&)noexcept;
    bool setCameraRange114F20(float,float,Diagnostic&)noexcept;
    bool invoke141CA8(C&,Diagnostic&)noexcept;
    bool walk141C30(C&,Diagnostic&)noexcept;
    bool viewport114A00(Diagnostic&)noexcept;
    bool beginRenderer244020(Diagnostic&)noexcept;
    bool finishRenderer2440E0(Diagnostic&)noexcept;
    bool walk141E20(C&,Diagnostic&)noexcept;
    bool invoke141E98(C&,Diagnostic&)noexcept;
    bool terminalQueue105990Current()const noexcept;
    bool finalSearchInvocation2805F0(const OriginalUiGaragePageOwner50KG&,const OriginalUiGarageVisibility50KG&,
        const C&,std::uint64_t&)const noexcept;
    bool publishReturnedTarget140A68(std::int32_t,std::int32_t,VehicleWindowDiagnostic50DS&)noexcept;
    bool rearmNextPublication140230(::original_ui_source_timing50LH::PublicationView,std::uint64_t,Diagnostic&)noexcept;
    bool targetCompleted140A38_=false,targetPublished140A68_=false,targetPublishAttempted_=false;

    bool finalPassReturned140844_=false,rendererEndReturned2442CC_=false;
    std::uint32_t finalEnabledSlots_=0;

    bool walk141CE0(C&,Diagnostic&)noexcept;
    bool invoke141D40(C&,Diagnostic&)noexcept;
    bool walk141D70(C&,Diagnostic&)noexcept;
    bool invoke141DE8(C&,Diagnostic&)noexcept;
    // Exact24B890 ->24BA50 call while the SAME native FIFO is draining.
    // Caller retains this context for that drain; no complete-frame authority.
    bool lowerQueuedModel24B26C(std::size_t realRecordOrdinal,
        const OriginalUiGarageModelLowering50KG::Context&,OriginalUiGarageOwnedModelGeometry50KG&,
        assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool captureModelContext24BA50(OriginalUiGarageModelLowering50KG::Context&,
        assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool camera114D50(Diagnostic&)noexcept;
    bool fog13EA10(C&,Diagnostic&)noexcept;
    bool particleProject162870(C&,Diagnostic&)noexcept;
    bool buildParticlePacket162010(OriginalUiGarageSceneParticles50KG&,Diagnostic&)noexcept;
    bool buildParticleScratch162230(OriginalUiGarageSceneParticles50KG&,Diagnostic&)noexcept;
    bool collectParticlePrimitive24B9B0(OriginalUiGarageSceneParticles50KG&,
        assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool lightParticleCorners149310(OriginalUiGarageSceneParticles50KG&,Diagnostic&)noexcept;
    bool particleProjectionInvocation1628ECCurrent(const OriginalUiGarageSceneParticles50KG&)const noexcept;
    bool particleRender162D30(C&,Diagnostic&)noexcept;
    bool drawPageChildren141D70(OriginalUiGaragePageOwner50KG&,std::uint32_t&)noexcept;
    bool drainCollector24B7B0(Diagnostic&)noexcept;
    bool readModelRenderInputs(OriginalUiGarageCameraFrame50KG::ModelRenderInputs&,std::uint32_t&)const noexcept;
    bool continue140230(Diagnostic&)noexcept;
    // Native cleanup stages, not Source Page/manager retirement. A failed
    // GPU close may resume after the actual target lease was already released.
    enum class NativeRetirementPhase:std::uint8_t {notStarted,admitted,preludeReleased,released};
    NativeRetirementPhase nativeRetirementPhase_=NativeRetirementPhase::notStarted;
    std::uint64_t nativeRetirementPublication_=0;
    bool nativeRetirementOwnerCurrent()const noexcept;
    // Native-only observation; the caller separately proves source unwind.
    bool unsealedNativeRendererQuiescent()const noexcept;
    bool releaseUnsealedBeforeRetirement()noexcept;
    bool beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&)const noexcept override;
    void finishSourceSubmission(std::uint64_t,bool,bool)const noexcept override;
public:
    ~OriginalUiGarageDrawContribution50KG()noexcept override;
};
}

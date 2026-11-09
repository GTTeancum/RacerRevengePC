#pragma once
// UIUnlock 2D full-frame composer, same source drain grouping and submission
// history contract as the Hall composer. Source order of one frame:
// raw +44 page background, ordinary widgets, page 20B710 (four %02d_bkg
// sprites), +440 exit text, aperture 0/1, prompts (+42C), OnTop widgets, drain.
// The selected control's User_Rotate pod and Soultree_Character pilot are 3D
// widgets with their own model owner; this composer does not draw them.
#include "../../../src/game/original_application_animation_clock50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
#include "original_ui_unlock_draw_resources50pd.h"
namespace rr::platform {
class VehicleWindowGl50DS;
enum class OriginalUiUnlockContributionError50PD : std::uint8_t {
    none,publication,input,resource,alreadyBound,notBound,sourceOrder,clock,geometry,projection,capacity,material,allocation,unfinished
};
struct OriginalUiUnlockContributionDiagnostic50PD {
    OriginalUiUnlockContributionError50PD error=OriginalUiUnlockContributionError50PD::none;
    OriginalUiUnlockDrawDiagnostic50PD resource{};
    game::OriginalUiUnlockWidgetDiagnostic50PD widgets{};
    std::uint16_t widgetId=0;std::uint32_t sourcePC=0;
};
enum class OriginalUiUnlockSegment50PD : std::uint8_t {rawBackground,ordinaryWidgets,page,exitText,aperture,prompts,onTopWidgets};
struct OriginalUiUnlockPacket50PD {
    std::uint32_t firstQuad=0,quadCount=0,materialIndex=0,imageIndex=0,frameIndex=0;
    std::uint32_t sourceOrdinal=0,sourcePC=0;
    std::uint16_t widgetId=0;OriginalUiUnlockSegment50PD segment=OriginalUiUnlockSegment50PD::ordinaryWidgets;
};
struct OriginalUiUnlockClock50PD {bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;
    game::OriginalApplicationAnimationClockView50KG applicationAnimation{};};
// Explicit host sampler adaptation of raw 2A6E90/2442E0/25CC60 for the three
// UIUnlock gallery PSTs only (artpod01/artpil01/artpla01, 640x448).
bool prepareOriginalUiUnlockBackgroundSpan50PD(const assets::OriginalUiScreenAssets50KG&,std::uint32_t image,
    OriginalUiSpan50KG&,OriginalUiNativeSpan50KG&) noexcept;
class OriginalUiUnlockDrawContribution50PD final : public OriginalUiSourceOwner50KM {
public:
    OriginalUiUnlockDrawContribution50PD() noexcept;
    ~OriginalUiUnlockDrawContribution50PD();
    OriginalUiUnlockDrawContribution50PD(const OriginalUiUnlockDrawContribution50PD&)=delete;
    OriginalUiUnlockDrawContribution50PD& operator=(const OriginalUiUnlockDrawContribution50PD&)=delete;
    bool bindFrame(std::shared_ptr<OriginalUiUnlockDrawResources50PD>,
        std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool begin(const OriginalUiUnlockClock50PD&,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool projectOrdinaryWidgets(bool ancestorsShown,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    // 20C9A0..20C9F0: four sprites of pod2D40 when it is not -1.
    bool projectPage(std::int32_t pod2D40,bool pageShown,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool projectExitText(const game::OriginalUiApertureHook50KG&,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool projectAperture(std::uint8_t,const game::OriginalUiApertureRectangle50KG&,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    // Actual page+42C: 0 or 1 (20B400 writes 1 = cross prompt).
    bool projectPrompts(std::uint32_t mask,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool projectOnTopWidgets(bool ancestorsShown,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool drain(OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    const OriginalUiSourceFrame50KM* sourceFrame() const noexcept;
    bool sourceTeardownPending() const noexcept;
    bool retireSourceRenderer(VehicleWindowGl50DS&,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    const std::vector<OriginalUiUnlockPacket50PD>* packets() const noexcept;
    const OriginalUiUnlockDrawResources50PD* resources() const noexcept;
private:
    bool beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&) const noexcept override;
    void finishSourceSubmission(std::uint64_t,bool sourceAccepted,bool sourceWorkStarted) const noexcept override;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiSourceFrame50KM frame_;
    bool bind(std::shared_ptr<OriginalUiUnlockDrawResources50PD>,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool append(const game::OriginalUiOverlayQuad50KG&,std::uint32_t material,float packetZ,
        OriginalUiUnlockSegment50PD,std::uint16_t,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool appendText(const game::OriginalUiTextLayout50KG&,const std::array<std::uint8_t,4>&,float packetZ,
        std::uint32_t material,std::uint32_t pc,OriginalUiUnlockSegment50PD,std::uint16_t,
        OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    bool widgets(bool onTop,bool ancestorsShown,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
    // Rider-path 3D projection of one dynamic widget (lit surfaces + binders).
    bool model(std::uint16_t,OriginalUiUnlockContributionDiagnostic50PD&) noexcept;
};
}

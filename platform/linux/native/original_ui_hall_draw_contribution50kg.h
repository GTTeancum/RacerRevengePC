#pragma once
#include "../../../src/game/original_application_animation_clock50kg.h"
#include "original_ui_hall_draw_resources50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
namespace rr::platform {
enum class OriginalUiHallContributionError50KG : std::uint8_t {
    none,publication,input,resource,alreadyBound,notBound,sourceOrder,clock,geometry,projection,capacity,material,allocation,unfinished
};
struct OriginalUiHallContributionDiagnostic50KG {
    OriginalUiHallContributionError50KG error=OriginalUiHallContributionError50KG::none;
    OriginalUiHallDrawDiagnostic50KG resource{};
    game::OriginalUiHallWidgetDiagnostic50KG widgets{};
    std::uint16_t widgetId=0;std::uint32_t sourcePC=0;
};
enum class OriginalUiHallSegment50KG : std::uint8_t {rawBackground,ordinaryWidgets,page,exitText,aperture,prompts,onTopWidgets};
struct OriginalUiHallPacket50KG {
    std::uint32_t firstQuad=0,quadCount=0,materialIndex=0,imageIndex=0,frameIndex=0;
    std::uint32_t sourceOrdinal=0,sourcePC=0;
    std::uint16_t widgetId=0;OriginalUiHallSegment50KG segment=OriginalUiHallSegment50KG::ordinaryWidgets;
};
struct OriginalUiHallClock50KG {bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;
    game::OriginalApplicationAnimationClockView50KG applicationAnimation{};};
// Actual mode5 Hall full-frame composer. Owns the exact source drain grouping
// and successful-submission material history; no model or particle surrogate.
class OriginalUiHallDrawContribution50KG final : public OriginalUiSourceOwner50KM {
public:
    OriginalUiHallDrawContribution50KG() noexcept;
    ~OriginalUiHallDrawContribution50KG();
    OriginalUiHallDrawContribution50KG(const OriginalUiHallDrawContribution50KG&)=delete;
    OriginalUiHallDrawContribution50KG& operator=(const OriginalUiHallDrawContribution50KG&)=delete;
    bool bind(std::shared_ptr<OriginalUiHallDrawResources50KG>,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool bindFrame(std::shared_ptr<OriginalUiHallDrawResources50KG>,
        std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    // Planet validates current page input only. Raw +44 uses resource-retained
    // +154 from the preceding shown +48 page render, or no image initially.
    bool begin(const OriginalUiHallClock50KG&,std::uint32_t planet,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool projectOrdinaryWidgets(bool ancestorsShown,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool projectPage(const OriginalUiHallPageInput50KG&,bool pageAncestorsShown,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool projectExitText(const game::OriginalUiApertureHook50KG&,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool projectAperture(std::uint8_t,const game::OriginalUiApertureRectangle50KG&,
        OriginalUiHallContributionDiagnostic50KG&) noexcept;
    // Session supplies actual page+42C: hidden0 or mode5 Back-only2.
    bool projectPrompts(std::uint32_t mask,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool projectOnTopWidgets(bool ancestorsShown,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool drain(OriginalUiHallContributionDiagnostic50KG&) noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    const OriginalUiSourceFrame50KM* sourceFrame() const noexcept;
    bool sourceTeardownPending() const noexcept;
    bool retireSourceRenderer(VehicleWindowGl50DS&,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool finishContribution(OriginalUiHallContributionDiagnostic50KG&) noexcept;
    const std::vector<assets::Vertex48B>* vertices() const noexcept;
    const std::vector<OriginalUiHallPacket50KG>* packets() const noexcept;
    const OriginalUiHallDrawResources50KG* resources() const noexcept;
private:
    bool beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&) const noexcept override;
    void finishSourceSubmission(std::uint64_t,bool sourceAccepted,bool sourceWorkStarted) const noexcept override;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiSourceFrame50KM frame_;
    bool append(const game::OriginalUiOverlayQuad50KG&,std::uint32_t material,float packetZ,
        OriginalUiHallSegment50KG,std::uint16_t,OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool appendText(const game::OriginalUiTextLayout50KG&,const std::array<std::uint8_t,4>&,
        std::uint32_t material,std::uint32_t pc,OriginalUiHallSegment50KG,std::uint16_t,
        OriginalUiHallContributionDiagnostic50KG&) noexcept;
    bool widgets(bool onTop,bool ancestorsShown,OriginalUiHallContributionDiagnostic50KG&) noexcept;
};
}

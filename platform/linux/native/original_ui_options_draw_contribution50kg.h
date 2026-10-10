#pragma once
#include "original_ui_options_draw_resources50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
namespace rr::platform {
enum class OriginalUiOptionsContributionError50KG : std::uint8_t {
    none,alreadyBound,notBound,sourceOrder,clock,geometry,projection,capacity,material,allocation,unfinished
};
struct OriginalUiOptionsContributionDiagnostic50KG {
    OriginalUiOptionsContributionError50KG error=OriginalUiOptionsContributionError50KG::none;
    OriginalUiOptionsDrawDiagnostic50KG resource{};
    game::OriginalUiOptionsWidgetDiagnostic50KG widgets{};
    std::uint16_t widgetId=0;std::uint32_t sourcePC=0;
};
enum class OriginalUiOptionsSegment50KG : std::uint8_t {ordinaryWidgets,page,aperture,prompts,onTopWidgets};
struct OriginalUiOptionsPacket50KG {
    std::uint32_t firstQuad=0,quadCount=0,materialIndex=0,imageIndex=0,frameIndex=0;
    std::uint32_t sourceOrdinal=0,sourcePC=0;
    std::uint16_t widgetId=0;OriginalUiOptionsSegment50KG segment=OriginalUiOptionsSegment50KG::ordinaryWidgets;
};
struct OriginalUiOptionsClock50KG {bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;};
// CPU packet CONTRIBUTION only. Does not clear a target or assert completion of
// Options manager/camera/scene/popup drawing. No opaque-alpha certification or
// native whole-frame draw is exposed. The full-frame owner must merge other
// real source siblings before registry drain and certify backend publication.
class OriginalUiOptionsDrawContribution50KG final {
public:
    OriginalUiOptionsDrawContribution50KG() noexcept;
    ~OriginalUiOptionsDrawContribution50KG();
    OriginalUiOptionsDrawContribution50KG(const OriginalUiOptionsDrawContribution50KG&)=delete;
    OriginalUiOptionsDrawContribution50KG& operator=(const OriginalUiOptionsDrawContribution50KG&)=delete;
    bool bind(std::shared_ptr<OriginalUiOptionsDrawResources50KG>,OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    bool begin(const OriginalUiOptionsClock50KG&,OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    bool projectOrdinaryWidgets(bool ancestorsShown,OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    // Exact page1F9A80 draw point. Page visibility gates whole contribution.
    bool projectPage(bool pageAncestorsShown,OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    // Wire only to real shared-aperture submitOverlay, original+424 then+428.
    bool projectAperture(std::uint8_t,const game::OriginalUiApertureRectangle50KG&,
        OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    // Reads the same bank's genuine source mask42C (0 or3), no caller mask.
    bool projectPrompts(OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    bool projectOnTopWidgets(bool ancestorsShown,OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    bool finishContribution(OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    // Original submission sequence, NOT material-sorted or ready for standalone
    // presentation. Each packet references the pinned resources' native policy.
    const std::vector<assets::Vertex48B>* vertices() const noexcept;
    const std::vector<OriginalUiOptionsPacket50KG>* packets() const noexcept;
    const OriginalUiOptionsDrawResources50KG* resources() const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
    bool append(const game::OriginalUiOverlayQuad50KG&,std::uint32_t material,float packetZ,
        OriginalUiOptionsSegment50KG,std::uint16_t,OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
    bool widgets(bool onTop,bool ancestorsShown,OriginalUiOptionsContributionDiagnostic50KG&) noexcept;
};
}

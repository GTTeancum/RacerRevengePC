#pragma once
// UIWin full-frame composer: ordinary widgets (text, overlay icons, the
// selected User_Rotate trophy and star), the page render 208AA0 background
// tiles, shared 1D7550 exit text/bars, 1D7A10 prompts, then the canonical
// 29C490 registry drain into one sealed source-RGBA frame.
#include "original_ui_win_draw_resources50pd.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
namespace rr::platform {
enum class OriginalUiWinContributionError50PD:std::uint8_t {
    none,publication,input,resource,alreadyBound,notBound,sourceOrder,clock,geometry,projection,capacity,material,model,allocation,unfinished
};
struct OriginalUiWinContributionDiagnostic50PD {
    OriginalUiWinContributionError50PD error=OriginalUiWinContributionError50PD::none;
    OriginalUiWinDrawDiagnostic50PD resource{};game::OriginalUiWinPageDiagnostic50PD page{};game::OriginalUiWinSceneDiagnostic50PD scene{};
    std::uint16_t widgetId=0;std::uint32_t sourcePC=0;
};
enum class OriginalUiWinSegment50PD:std::uint8_t {ordinaryWidgets,model,page,exitText,aperture,prompts};
struct OriginalUiWinClock50PD {bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;
    game::OriginalApplicationAnimationClockView50KG applicationAnimation{};};
class OriginalUiWinDrawContribution50PD final : public OriginalUiSourceOwner50KM {
public:
    OriginalUiWinDrawContribution50PD() noexcept;~OriginalUiWinDrawContribution50PD();
    OriginalUiWinDrawContribution50PD(const OriginalUiWinDrawContribution50PD&)=delete;
    OriginalUiWinDrawContribution50PD& operator=(const OriginalUiWinDrawContribution50PD&)=delete;
    bool bindFrame(std::shared_ptr<OriginalUiWinDrawResources50PD>,std::shared_ptr<const MaterialTextureGl50EB>,
        OriginalUiWinContributionDiagnostic50PD&) noexcept;
    bool begin(const OriginalUiWinClock50PD&,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    bool projectOrdinaryWidgets(bool ancestorsShown,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    // 208AA0 first part: the four %02d_bkg tiles of the actual career P+100C0.
    bool projectPage(std::uint32_t career,bool pageShown,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    bool projectExitText(const game::OriginalUiApertureHook50KG&,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    bool projectAperture(std::uint8_t,const game::OriginalUiApertureRectangle50KG&,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    // Actual page+42C: 0 hidden, 1 cross (208910).
    bool projectPrompts(std::uint32_t mask,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    bool drain(OriginalUiWinContributionDiagnostic50PD&) noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    const OriginalUiSourceFrame50KM* sourceFrame() const noexcept;
    bool sourceTeardownPending() const noexcept;
    bool retireSourceRenderer(VehicleWindowGl50DS&,OriginalUiWinContributionDiagnostic50PD&) noexcept;
private:
    bool beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&) const noexcept override;
    void finishSourceSubmission(std::uint64_t,bool sourceAccepted,bool sourceWorkStarted) const noexcept override;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiSourceFrame50KM frame_;
    bool append(const game::OriginalUiOverlayQuad50KG&,std::uint32_t material,float packetZ,
        OriginalUiWinSegment50PD,std::uint16_t,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    bool appendText(const game::OriginalUiTextLayout50KG&,const std::array<std::uint8_t,4>&,std::uint32_t material,
        std::uint32_t pc,OriginalUiWinSegment50PD,std::uint16_t,OriginalUiWinContributionDiagnostic50PD&) noexcept;
    bool model(std::uint16_t,OriginalUiWinContributionDiagnostic50PD&) noexcept;
};
}

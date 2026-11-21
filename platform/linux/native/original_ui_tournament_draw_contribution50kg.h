#pragma once
#include "original_ui_tournament_widgets50kg.h"
#include "original_ui_tournament_career_resources50kg.h"
#include "original_ui_lit_material_adapter50km.h"
#include "../../../src/game/original_ui_tournament_camera50kg.h"
#include "../../../src/game/original_ui_tournament_pod_owner50kg.h"
#include "../../../src/game/original_ui_tournament_predecessor50kg.h"
namespace rr::platform {
struct OriginalUiTournamentDrawClock50KG {
 bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;
 game::OriginalApplicationAnimationClockView50KG applicationAnimation{};
};
enum class OriginalUiTournamentContributionError50KG:std::uint8_t {
 none,input,alreadyBound,resource,bank,publication,phase,clock,widget,model,geometry,material,schedule,capacity,allocation,unfinished
};
struct OriginalUiTournamentContributionDiagnostic50KG {
 OriginalUiTournamentContributionError50KG error=OriginalUiTournamentContributionError50KG::none;
 std::uint16_t widget=0;std::uint32_t material=0,pass=0,sourcePC=0;
 assets::OriginalUiMaterialDiagnostic50KG materialDetail{};
 assets::OriginalUiImageBankDiagnostic50KG bank{};
 OriginalUiTournamentWidgetsDiagnostic50KG widgetDetail{};
 game::OriginalUiTournamentPodDiagnostic50KG pod{};
 game::OriginalUiTournamentCameraDiagnostic50KG camera{};
};
// Full source-RGBA Tournament composer. All constructor registrations (even
// hidden popup references), actual selected model passes and both live binder
// helpers participate in the canonical row/pass schedule. No partial2D seal.
class OriginalUiTournamentDrawContribution50KG final:public OriginalUiSourceOwner50KM {
public:
 OriginalUiTournamentDrawContribution50KG() noexcept;~OriginalUiTournamentDrawContribution50KG();
 OriginalUiTournamentDrawContribution50KG(const OriginalUiTournamentDrawContribution50KG&)=delete;
 OriginalUiTournamentDrawContribution50KG& operator=(const OriginalUiTournamentDrawContribution50KG&)=delete;
 static bool staticRows(const OriginalUiTournamentWidgetResources50KG&,
     const OriginalUiTournamentCareerResources50KG&,std::vector<game::OriginalUiRiderMaterialRow50KG>&,
     OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
private:
 friend class OriginalUiTournamentSession50KG;
 friend class OriginalUiFrontend50KG;
 bool bind(std::shared_ptr<OriginalUiTournamentWidgetResources50KG>,
     std::shared_ptr<OriginalUiTournamentCareerResources50KG>,std::shared_ptr<OriginalUiTournamentWidgets50KG>,
     std::shared_ptr<game::OriginalUiTournamentPodOwner50KG>,std::shared_ptr<game::OriginalUiTournamentCameraOwner50KG>,
     std::shared_ptr<const game::OriginalUiTransitionOwner50KG>,const game::OriginalUiApertureOwner50KG*,
     OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
public:
 bool prepareBank(OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 std::shared_ptr<const assets::OriginalUiImageBank50KG> bank() const noexcept;
 const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
 bool bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool begin(const OriginalUiTournamentDrawClock50KG&,const game::OriginalUiTournamentCameraFrame50KG&,
     OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool projectOrdinaryWidgets(bool ancestorsShown,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool projectCareer(std::uint8_t actualPopupFlags29,bool pageShown,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool projectExitText(const game::OriginalUiApertureHook50KG&,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool projectAperture(std::uint8_t,const game::OriginalUiApertureRectangle50KG&,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool projectPrompts(std::uint32_t actualMask42C,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool projectOnTopWidgets(bool ancestorsShown,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool finish(OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 const OriginalUiSourceFrame50KM* frame() const noexcept;
 void drain() noexcept;
 bool retireSourceRenderer(VehicleWindowGl50DS&,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool sourceTeardownPending() const noexcept;
 std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG> predecessorInventory(OriginalUiTournamentContributionDiagnostic50KG&) const noexcept;
private:
 bool retirePredecessorInventory(std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG>,
     game::OriginalUiTournamentPredecessorRetirement50KG&,OriginalUiTournamentContributionDiagnostic50KG&) const noexcept;
 bool retireUnsubmittedSource(OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool retireStaticRegistrations(OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool retirementComplete() const noexcept;
private:
 bool consumeMaterials(bool,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 std::uint64_t materialSequence() const noexcept;
 std::uint64_t sourceSubmissionWatermark() const noexcept;
 bool beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&) const noexcept override;
 void finishSourceSubmission(std::uint64_t,bool,bool) const noexcept override;
 bool widgets(bool,bool,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool model(std::uint16_t,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 // PORT EXTENSION (DLC career): the add-on model in place of the anchor pod (never the anchor).
 bool addonModel(const game::OriginalUiTournamentPodPacket50KG&,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool addonBinders(OriginalUiTournamentContributionDiagnostic50KG&) noexcept; // DLC career: the add-on pod's own cables
 bool append(const game::OriginalUiOverlayQuad50KG&,const assets::OriginalUiMaterialPass50KG*,std::uint32_t,float,
     OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 bool appendText(const game::OriginalUiTextLayout50KG&,const std::array<std::uint8_t,4>&,
     const assets::OriginalUiMaterialPass50KG*,std::uint32_t,OriginalUiTournamentContributionDiagnostic50KG&) noexcept;
 struct Storage;std::unique_ptr<Storage> storage_;OriginalUiSourceFrame50KM frame_;
};
}

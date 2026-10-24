#pragma once
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include "../../../src/game/original_ui_controller_popup50kg.h"
#include <array>
#include <memory>
#include <vector>
namespace rr::game { class OriginalUiControllerPopupGeometry50KG; }
namespace rr::platform {
struct OriginalUiOptionsColdInput50KG;
struct OriginalUiMainColdInput50KG;
struct OriginalUiMainMaterialBinding50KG;
class OriginalUiMainDraw50KG;
class OriginalUiSharedDialogs50KG;
class OriginalUiSharedDialogMaterialHistory50KG;
class OriginalUiMainDrawResources50KG;
class OriginalUiSourceOwner50KM;
struct OriginalUiOptionsMaterialBinding50KG;
class OriginalUiOptionsDrawContribution50KG;
class OriginalUiOptionsDrawResources50KG;
enum class OriginalUiControllerPopupResourceError50KG : std::uint8_t {
 none,input,identity,video,material,geometry,history,busy,poisoned,allocation,canonicalBridgePending
};
struct OriginalUiControllerPopupResourceDiagnostic50KG {
 OriginalUiControllerPopupResourceError50KG error=OriginalUiControllerPopupResourceError50KG::none;
 std::uint32_t sourcePC=0,registryToken=0;
};
// Original global UI+47C lifetime, independent of the active manager bank.
// Retains source assets and exact registered-material histories, never indexes
// into an expired Options bank. Rejoins require the same genuine Video owner.
class OriginalUiControllerPopupResources50KG final {
public:
 static constexpr std::array<std::uint32_t,5> registryTokens{{3228,3229,3231,3232,3233}};
 OriginalUiControllerPopupResources50KG() noexcept;
 ~OriginalUiControllerPopupResources50KG();
 OriginalUiControllerPopupResources50KG(const OriginalUiControllerPopupResources50KG&)=delete;
 OriginalUiControllerPopupResources50KG& operator=(const OriginalUiControllerPopupResources50KG&)=delete;
 bool loaded() const noexcept;
 bool poisoned() const noexcept;
 const game::OriginalUiControllerPopupGeometry50KG* geometry() const noexcept;
 bool project(const game::OriginalUiControllerPopupRender50KG&,OriginalUiControllerPopupResourceDiagnostic50KG&) noexcept;
private:
 friend class OriginalUiSharedDialogMaterialHistory50KG;
 bool transferInitialHistoryTo(const OriginalUiSharedDialogs50KG&,
   const std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG>&,
   OriginalUiControllerPopupResourceDiagnostic50KG&)noexcept;
 friend class OriginalUiSharedDialogs50KG;
 bool sharedDialogOwnerCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
 bool bindSharedDialogOwner2055B0(OriginalUiSharedDialogs50KG&)noexcept;
 bool sharedDialogBoundCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
 bool preflightSharedRetirement20DF80(const OriginalUiSharedDialogs50KG&)const noexcept;
 bool retireSharedPayloadPart20DF80(OriginalUiSharedDialogs50KG&,std::size_t,OriginalUiControllerPopupResourceDiagnostic50KG&)noexcept;
 bool finishSharedPayload20E008(OriginalUiSharedDialogs50KG&)noexcept;
 friend class OriginalUiOptionsDrawResources50KG;
 friend class OriginalUiMainDraw50KG;
 friend class OriginalUiMainDrawResources50KG;
 bool prepareOrRejoin(const OriginalUiMainColdInput50KG&,
   std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
   const std::vector<OriginalUiMainMaterialBinding50KG>&,
   OriginalUiControllerPopupResourceDiagnostic50KG&) noexcept;
 friend class OriginalUiOptionsDrawContribution50KG;
 bool prepareOrRejoin(const OriginalUiOptionsColdInput50KG&,
   std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
   const std::vector<OriginalUiOptionsMaterialBinding50KG>&,
   OriginalUiControllerPopupResourceDiagnostic50KG&) noexcept;
 bool copyHistory(std::uint32_t,const assets::OriginalUiMaterialEquivalenceKey50KG&,
   assets::OriginalUiMaterialBindingState50KG&,OriginalUiControllerPopupResourceDiagnostic50KG&) const noexcept;
 bool acquire(const OriginalUiSourceOwner50KM*,std::uint64_t&) noexcept;
 void release(const OriginalUiSourceOwner50KM*,bool retired) noexcept;
 bool current(const OriginalUiSourceOwner50KM*,std::uint64_t) const noexcept;
 bool commit(const OriginalUiSourceOwner50KM*,std::uint64_t&,
   std::array<assets::OriginalUiMaterialBindingState50KG,5>&) noexcept;
 void poison(const OriginalUiSourceOwner50KM*) noexcept;
 struct Storage;std::unique_ptr<Storage> storage_;
};
}

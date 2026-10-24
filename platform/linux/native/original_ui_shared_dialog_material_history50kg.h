#pragma once
#include "original_ui_controller_popup_resources50kg.h"
#include <array>
#include <memory>
#include "../../../src/assets/original_standard_material_table50la.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"

namespace rr::platform {
class OriginalUiSourceFrame50KM;
class OriginalUiGarageCanonicalSubmission50KG;
class OriginalUiGarageDrawContribution50KG;
class VehicleWindowGl50DS;
class OriginalUiSharedDialogMaterialHistory50KG final {
    friend class OriginalUiSharedDialogs50KG;
    friend class OriginalUiControllerPopupResources50KG;
    friend class OriginalUiMainDraw50KG;
    friend class OriginalUiGarageCanonicalSubmission50KG;
    friend class OriginalUiOptionsDrawContribution50KG;
    using Key=assets::OriginalUiMaterialEquivalenceKey50KG;
    using History=assets::OriginalUiMaterialBindingState50KG;
    using D=OriginalUiControllerPopupResourceDiagnostic50KG;
public:
    ~OriginalUiSharedDialogMaterialHistory50KG()noexcept;
    OriginalUiSharedDialogMaterialHistory50KG(const OriginalUiSharedDialogMaterialHistory50KG&)=delete;
    OriginalUiSharedDialogMaterialHistory50KG& operator=(const OriginalUiSharedDialogMaterialHistory50KG&)=delete;
private:
    // This is not the five-role47C geometry enum. Do not use these indexes to
    // interpret OriginalUiControllerPopupMaterial50KG packet.material.
    static constexpr std::array<std::uint32_t,6> registryTokens{{3228,3229,3230,3231,3232,3233}};
    static constexpr std::array<std::size_t,5> from47C{{0,1,3,4,5}};
    enum class Phase : std::uint8_t {prepared,attached,awaiting470,retired};
    OriginalUiSharedDialogMaterialHistory50KG()noexcept=default;
    struct Storage;std::unique_ptr<Storage> storage_;

    // Only this exact initial retained shared prefix may transfer its actual
    // unbound47C histories. No caller-supplied owner/readiness or cold fallback.
    static bool bootstrapInitial(OriginalUiSharedDialogs50KG&,
        const OriginalUiOptionsColdInput50KG&,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        const std::vector<OriginalUiOptionsMaterialBinding50KG>&,D&)noexcept;
    static bool bootstrapInitial(OriginalUiSharedDialogs50KG&,
        const OriginalUiMainColdInput50KG&,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        const std::vector<OriginalUiMainMaterialBinding50KG>&,D&)noexcept;
    bool initialOwnerCurrent(const OriginalUiSharedDialogs50KG&)const noexcept;
    bool source47CTransferCurrent(const OriginalUiSharedDialogs50KG&,
        const OriginalUiControllerPopupResources50KG&)const noexcept;
    bool preparedSourcesCurrent(const OriginalUiOptionsColdInput50KG&)const noexcept;
    bool takeColdFive(const OriginalUiSharedDialogs50KG&,
        const OriginalUiControllerPopupResources50KG&,
        const std::array<Key,5>&,std::array<History,5>&,D&)noexcept;
    bool preflight47CRetirement(const OriginalUiSharedDialogs50KG&)const noexcept;
    bool finish47CPayload(const OriginalUiSharedDialogs50KG&,
        const OriginalUiControllerPopupResources50KG&)noexcept;
    bool finishAfter470(OriginalUiSharedDialogs50KG&,D&)noexcept;

    // Only the real Draw's FINALREP walk may create these observations. They
    // are not receipts: stageMenu also authenticates the actual sealed frame.
    struct MenuUse {std::uint32_t originToken=0,representativeToken=0,spanIndex=0,
        imageIndex=assets::unresolvedMaterialTexture50EA;};
    bool rejoinBank(const OriginalUiOptionsColdInput50KG&,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        const std::vector<OriginalUiOptionsMaterialBinding50KG>&,D&)const noexcept;
    bool acquire(const OriginalUiSourceOwner50KM*,std::uint64_t&,D&)noexcept;
    bool current(const OriginalUiSourceOwner50KM*,std::uint64_t)const noexcept;
    bool copyHistory(const OriginalUiSourceOwner50KM*,std::uint64_t,std::uint32_t,
        const Key&,History&,D&)const noexcept;
    bool stageMenu(const OriginalUiSourceOwner50KM*,std::uint64_t,const OriginalUiSourceFrame50KM&,
        const assets::OriginalUiImageBank50KG&,const std::vector<MenuUse>&,
        const std::array<History,6>&,D&)noexcept;
    bool beginMenu(const OriginalUiSourceOwner50KM*,std::uint64_t,const OriginalUiSourceFrame50KM&,
        const VehicleWindowGl50DS&)noexcept;
    bool cancelMenuBegin(const OriginalUiSourceOwner50KM*,std::uint64_t,const OriginalUiSourceFrame50KM&,
        const VehicleWindowGl50DS&)noexcept;
    bool finishMenu(const OriginalUiSourceOwner50KM*,std::uint64_t&,const OriginalUiSourceFrame50KM&,
        bool accepted,bool started)noexcept;
    bool menuStagePending(const OriginalUiSourceOwner50KM*)const noexcept;
    bool releaseMenu(const OriginalUiMainDraw50KG*,const VehicleWindowGl50DS*)noexcept;
    bool releaseMenu(const OriginalUiOptionsDrawContribution50KG*,const VehicleWindowGl50DS*)noexcept;
    bool releaseMenuAfterRetirement(const OriginalUiSourceOwner50KM*,const VehicleWindowGl50DS*)noexcept;
    static bool stageGarage(OriginalUiGarageCanonicalSubmission50KG&,OriginalUiGarageDrawContribution50KG&,D&)noexcept;
    bool garageCurrent(const OriginalUiGarageCanonicalSubmission50KG&)const noexcept;
    void commitGarage(OriginalUiGarageCanonicalSubmission50KG&)noexcept;
    void cancelGarage(OriginalUiGarageCanonicalSubmission50KG&,bool poison)noexcept;
    bool bindInitialCanonical(D&)noexcept;
    bool canonicalCurrent()const noexcept;
    bool sameCanonicalHistory(std::size_t,const History&,
        const assets::OriginalStandardMaterialStateContext50LA&,
        const assets::OriginalStandardMaterialNativeBinding50KG&)const noexcept;
    bool authenticateImage(std::size_t,const assets::OriginalUiImageBank50KG&,std::uint32_t,
        const History&,assets::OriginalStandardMaterialNativeBinding50KG&,D&)const noexcept;
    // No generic publication API can bypass the accepted transaction.
    bool commit(const OriginalUiSourceOwner50KM*,std::uint64_t&,std::array<History,6>&,D&)noexcept;
    void poison(const OriginalUiSourceOwner50KM*)noexcept;
};
}

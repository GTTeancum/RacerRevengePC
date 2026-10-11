#pragma once
#include "original_ui_rider_dynamic_owner50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_ui_rider_binder_animation50kg.h"
#include "../assets/original_ui_image_bank50kg.h"
#include "../assets/original_ui_rider_model_materials50kg.h"
namespace rr::game {
struct OriginalUiRiderBackendInput50KG {
    std::shared_ptr<const assets::OriginalUiArchive50KG> rider,appStart,podTextures;
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binder;
    assets::OriginalUiMaterialRuntime50KG materialRuntime;
    // Actual static widget/font/stat/background image owners and every archive
    // they use. Backend appends binder and live/new model owners, never merges IDs.
    assets::OriginalUiImageBankInput50KG staticImages;
};
enum class OriginalUiRiderBackendError50KG : std::uint8_t {
    none,input,alreadyBound,material,bank,membership,binder,busy,allocation
};
struct OriginalUiRiderBackendDiagnostic50KG {
    OriginalUiRiderBackendError50KG error=OriginalUiRiderBackendError50KG::none;
    std::uint16_t widget=0;
    assets::OriginalUiRiderModelMaterialDiagnostic50KG material;
    assets::OriginalUiImageBankDiagnostic50KG bank;
    OriginalUiRiderVisibilityDiagnostic50KG membership;
};
// Actual CPU resource/entity/membership backend. Does not enable a destination
// or publish a GPU bank. All callbacks synchronously perform their owner work.
// A renderer consumes one bank generation and copies projected vertices during
// a non-reentrant frame lease; selection/attachment is forbidden during a lease.
// The complete composer must release the lease before any selection/teardown.
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
    bool beginFrame() noexcept;
    void endFrame() noexcept;
    std::uint64_t generation() const noexcept;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank() const noexcept;
    const assets::OriginalUiImageBankMapping50KG* staticImage(std::uint32_t owner,std::uint32_t image) const noexcept;
    const assets::OriginalUiImageBankMapping50KG* modelImage(std::uint16_t widget,std::uint32_t image) const noexcept;
    const assets::OriginalUiImageBankMapping50KG* binderImage(std::uint32_t image) const noexcept;
    const assets::OriginalUiRiderModelMaterials50KG* materials(std::uint16_t) const noexcept;
    const OriginalUiRiderVisibilityHandle50KG* membership(std::uint16_t) const noexcept;
    bool attached(std::uint16_t) const noexcept;
    bool shown(std::uint16_t) const noexcept;
    // Children are visited after their actual widget, then model, then binder
    // pair0/pair1. 141960 appends siblings; 2/100A/8 are entity MASKS, not sort keys.
    OriginalUiRiderBinderAnimation50KG* binder(std::uint16_t,std::uint8_t pair) noexcept;
private:
    bool preflight(const OriginalUiRiderDynamicOwner50KG&,const OriginalUiRiderWidgetCallback50KG*,std::size_t) noexcept;
    bool attach(std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept;
    void detach(std::uint16_t,OriginalUiRiderDynamicOwner50KG&) noexcept;
    bool setShown(std::uint16_t,bool) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiRiderBackendDiagnostic50KG diagnostic_;
};
}

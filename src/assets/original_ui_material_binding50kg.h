#pragma once
#include "original_ui_material_equivalence50kg.h"
#include "original_ui_image_bank50kg.h"

namespace rr::assets {
enum class OriginalUiMaterialBindingError50KG : std::uint8_t {
    none,alreadyInitialized,key,uninitialized,untextured,disabled,bank,image,provenance,policy,allocation
};
struct OriginalUiMaterialBindingDiagnostic50KG {
    OriginalUiMaterialBindingError50KG error=OriginalUiMaterialBindingError50KG::none;
    std::uint32_t imageIndex=unresolvedMaterialTexture50EA,sourcePC=0;
};
// Retained HOST semantic history for one real registered material/pass lifetime.
// The composer owns that lifetime and the actual29C490 representative/redirect
// graph. Repeated PSG references are not new material/pass lifetimes.
//
// The immutable native base-image policy replaces historical texture-address /
// palette-address / residency fields with actual original image provenance.
// This is intentionally not literal guest-allocation/mip-residency parity.
// No addresses, guest packet bytes, virtual device or caller readiness flag.
// Keep the common native source video/depth owner unchanged across comparisons.
class OriginalUiMaterialBindingState50KG final {
public:
    // Initializes COLD once from the approved construction comparator. Copyable
    // for transactional frame staging. Reinitializing retained records rejects.
    bool initialize(const OriginalUiMaterialEquivalenceKey50KG&,
        OriginalUiMaterialBindingDiagnostic50KG&) noexcept;
    // Call only for the enabled final leaf REPRESENTATIVE that actually bound
    // this image in an accepted source-frame publication. Alias submitters do
    // not acquire the representative's binding. No geometry/empty group means
    // no bind. For a failed/partial renderer publication, discard or retire the
    // frame owner; do not guess which source groups completed from span counts.
    //
    // Reads the actual combined bank's authenticated immutable ScreenAssets.
    // Index is bank-local input only, never an equality identity. This method
    // does not upload/draw or certify a GPU receipt. The composer supplies the
    // existing real publication boundary, then commits its staged state copies.
    // Zero-map passes have no binding; do not call this method for them.
    // State is unchanged on failure. Rebinding the same native image is safe.
    bool recordDrainedBinding(const OriginalUiImageBank50KG& combinedBank,
        std::uint32_t combinedImageIndex,OriginalUiMaterialBindingDiagnostic50KG&) noexcept;
    OriginalUiMaterialEquivalence50KG compare(const OriginalUiMaterialBindingState50KG&) const noexcept;
    bool initialized() const noexcept {return initialized_;}
    bool bound() const noexcept {return bound_;}
private:
    OriginalUiMaterialEquivalenceKey50KG construction_,effective_;
    bool initialized_=false,bound_=false;
    const OriginalUiMemberIdentity50KG* member_=nullptr;
    std::size_t pstOffset_=0,pstBytes_=0,storedMip_=0,mipHeader_=0,mipPayload_=0;
    std::uint32_t width_=0,height_=0,kind_=0;
    OriginalUiIpumColorPolicy50KG ipumPolicy_=OriginalUiIpumColorPolicy50KG::unavailable;
};
}

#pragma once
#include "original_ui_atm50kg.h"
#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"
#include <memory>

namespace rr::assets {
enum class OriginalUiRiderBinderAssetError50KG : std::uint8_t {
    none,alreadyLoaded,archive,material,materialProfile,animationProfile,atm,images,allocation
};
struct OriginalUiRiderBinderAssetDiagnostic50KG {
    OriginalUiRiderBinderAssetError50KG error=OriginalUiRiderBinderAssetError50KG::none;
    OriginalUiMaterialDiagnostic50KG material;
    DecodeError48C atm;
    OriginalUiScreenAssetDiagnostic50KG images;
};
// Immutable authenticated CPU owner. Shared among live binder instances, but
// NEVER stores their unique animation phase or certifies renderer admission.
class OriginalUiRiderBinderAssets50KG final {
public:
    OriginalUiRiderBinderAssets50KG() noexcept;
    ~OriginalUiRiderBinderAssets50KG();
    OriginalUiRiderBinderAssets50KG(const OriginalUiRiderBinderAssets50KG&)=delete;
    OriginalUiRiderBinderAssets50KG& operator=(const OriginalUiRiderBinderAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG> rider,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        const OriginalUiMaterialRuntime50KG&,OriginalUiRiderBinderAssetDiagnostic50KG&) noexcept;
    bool loadTournament(std::shared_ptr<const OriginalUiArchive50KG> tournament,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        const OriginalUiMaterialRuntime50KG&,OriginalUiRiderBinderAssetDiagnostic50KG&) noexcept;
    // Separate authenticated UIUnlock binder.psm/binder.atm owner.
    bool loadUnlock(std::shared_ptr<const OriginalUiArchive50KG> unlock,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        const OriginalUiMaterialRuntime50KG&,OriginalUiRiderBinderAssetDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    const OriginalUiMaterialPass50KG* pass() const noexcept;
    const OriginalUiAtm50KG* animation() const noexcept;
    const OriginalUiScreenAssets50KG* images() const noexcept;
    OriginalUiMemberView50KG atmMember() const noexcept;
private:
    bool loadImpl(std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<const OriginalUiArchive50KG>,const OriginalUiMaterialRuntime50KG&,
        OriginalUiRiderBinderAssetDiagnostic50KG&,bool tournament,bool unlock=false) noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}

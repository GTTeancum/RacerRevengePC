#pragma once
#include "original_ui_garage_scene_assets50kg.h"
#include "original_ui_garage_watto_assets50kg.h"
#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"

namespace rr::assets {
enum class OriginalUiGarageMaterialKind50KG : std::uint8_t { absent,staticModel,actor,selected19E,watto };
enum class OriginalUiGarageModelMaterialError50KG : std::uint8_t {
    none,alreadyLoaded,input,material,texture,animatedTexture,images,allocation
};
struct OriginalUiGarageModelMaterialDiagnostic50KG {
    OriginalUiGarageModelMaterialError50KG error=OriginalUiGarageModelMaterialError50KG::none;
    std::uint32_t material=0,pass=0;
    OriginalUiMaterialDiagnostic50KG materialDetail{};
    OriginalUiScreenAssetDiagnostic50KG images{};
};
struct OriginalUiGarageModelMaterialPass50KG {
    OriginalUiMaterialPass50KG source;
    std::uint32_t imageIndex=unresolvedMaterialTexture50EA;
};
// Authenticated immutable material/image owner for ONE actual Garage model
// domain. Preserve PSG material slots, repeated names, all pass order/disabled
// passes, exact DefaultMT and source metadata. Distinct models may share image
// identity but never become a single model or canonical live material owner.
// This creates no material references, renderer admission or scene capability.
class OriginalUiGarageModelMaterials50KG final {
public:
    OriginalUiGarageModelMaterials50KG() noexcept;
    ~OriginalUiGarageModelMaterials50KG();
    OriginalUiGarageModelMaterials50KG(const OriginalUiGarageModelMaterials50KG&)=delete;
    OriginalUiGarageModelMaterials50KG& operator=(const OriginalUiGarageModelMaterials50KG&)=delete;
    bool loadStatic(std::shared_ptr<const OriginalUiGarageSceneAssets50KG>,std::size_t,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,const OriginalUiMaterialRuntime50KG&,
        const OriginalUiIpumTextureCache50KG*,OriginalUiGarageModelMaterialDiagnostic50KG&) noexcept;
    bool loadActor(std::shared_ptr<const OriginalUiGarageSceneAssets50KG>,std::size_t,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,const OriginalUiMaterialRuntime50KG&,
        const OriginalUiIpumTextureCache50KG*,OriginalUiGarageModelMaterialDiagnostic50KG&) noexcept;
    bool loadSelected(std::shared_ptr<const OriginalUiGarageSceneAssets50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,const OriginalUiMaterialRuntime50KG&,
        const OriginalUiIpumTextureCache50KG*,OriginalUiGarageModelMaterialDiagnostic50KG&) noexcept;
    bool loadWatto(std::shared_ptr<const OriginalUiGarageWattoAssets50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> garage,std::shared_ptr<const OriginalUiArchive50KG> appStart,
        const OriginalUiMaterialRuntime50KG&,const OriginalUiIpumTextureCache50KG*,
        OriginalUiGarageModelMaterialDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    OriginalUiGarageMaterialKind50KG kind() const noexcept;
    std::size_t sourceOrdinal() const noexcept;
    const OriginalUiMemberIdentity50KG* modelIdentity() const noexcept;
    std::uint32_t materialCount() const noexcept;
    std::uint32_t passCount(std::uint32_t material) const noexcept;
    const OriginalUiGarageModelMaterialPass50KG* pass(std::uint32_t material,std::uint32_t pass) const noexcept;
    std::shared_ptr<const OriginalUiScreenAssets50KG> images() const noexcept;
    std::shared_ptr<const OriginalUiArchive50KG> archive() const noexcept;
private:
    bool load(std::shared_ptr<const void>,const Scene48D*,const OriginalUiMemberIdentity50KG*,
        OriginalUiGarageMaterialKind50KG,std::size_t,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<const OriginalUiArchive50KG>,const OriginalUiMaterialRuntime50KG&,
        const OriginalUiIpumTextureCache50KG*,OriginalUiGarageModelMaterialDiagnostic50KG&) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}

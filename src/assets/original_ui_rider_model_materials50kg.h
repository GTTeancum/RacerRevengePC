#pragma once
#include "original_ui_rider_character_assets50kg.h"
#include "original_ui_rider_pod_assets50kg.h"
#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"
namespace rr::assets {
enum class OriginalUiRiderModelMaterialError50KG : std::uint8_t {
    none,alreadyLoaded,input,material,texture,animatedTexture,images,allocation
};
struct OriginalUiRiderModelMaterialDiagnostic50KG {
    OriginalUiRiderModelMaterialError50KG error=OriginalUiRiderModelMaterialError50KG::none;
    std::uint32_t material=0,pass=0;
    OriginalUiMaterialDiagnostic50KG materialDetail;
    OriginalUiScreenAssetDiagnostic50KG images;
};
struct OriginalUiRiderModelMaterialPass50KG {
    OriginalUiMaterialPass50KG source;
    std::uint32_t imageIndex=unresolvedMaterialTexture50EA;
};
// Concrete cold material/image population for a real loaded Rider model.
// Preserves original PSG slot order INCLUDING repeated material names, original
// pass order/count, authored program spelling and resolved DefaultMT fields.
// This bounded owner supports genuine zero/one-map STATIC PST passes; an ATM
// or missing/unsupported texture fails, never falls back to another material.
// Pod body textures belong to original UIPODTEX, populated by 1B7630..1B7838
// into the original shared resource registry before Rider model creation.
// Exact archive membership is retained; ambiguous duplicate names reject.
// No renderer/program admission or GPU/destination readiness is implied.
class OriginalUiRiderModelMaterials50KG final {
public:
    OriginalUiRiderModelMaterials50KG() noexcept;~OriginalUiRiderModelMaterials50KG();
    OriginalUiRiderModelMaterials50KG(const OriginalUiRiderModelMaterials50KG&)=delete;
    OriginalUiRiderModelMaterials50KG& operator=(const OriginalUiRiderModelMaterials50KG&)=delete;
    bool loadCharacter(std::shared_ptr<const OriginalUiRiderCharacterAssets50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> rider,std::shared_ptr<const OriginalUiArchive50KG> appStart,
        const OriginalUiMaterialRuntime50KG&,const OriginalUiIpumTextureCache50KG*,
        OriginalUiRiderModelMaterialDiagnostic50KG&) noexcept;
    bool loadPod(std::shared_ptr<const OriginalUiRiderPodAssets50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        std::shared_ptr<const OriginalUiArchive50KG> podTextures,
        const OriginalUiMaterialRuntime50KG&,const OriginalUiIpumTextureCache50KG*,
        OriginalUiRiderModelMaterialDiagnostic50KG&) noexcept;
    // Separate authenticated Tournament entry; Rider loadPod remains Rider-only.
    bool loadTournamentPod(std::shared_ptr<const OriginalUiRiderPodAssets50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        std::shared_ptr<const OriginalUiArchive50KG> podTextures,
        const OriginalUiMaterialRuntime50KG&,const OriginalUiIpumTextureCache50KG*,
        OriginalUiRiderModelMaterialDiagnostic50KG&) noexcept;
    // Separate authenticated UIUnlock entries (pod widgets 9..23 with UIPODTEX,
    // pilot widgets 24..38 with UIUnlock-local textures).
    bool loadUnlockPod(std::shared_ptr<const OriginalUiRiderPodAssets50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        std::shared_ptr<const OriginalUiArchive50KG> podTextures,
        const OriginalUiMaterialRuntime50KG&,const OriginalUiIpumTextureCache50KG*,
        OriginalUiRiderModelMaterialDiagnostic50KG&) noexcept;
    bool loadUnlockCharacter(std::shared_ptr<const OriginalUiRiderCharacterAssets50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> unlock,std::shared_ptr<const OriginalUiArchive50KG> appStart,
        const OriginalUiMaterialRuntime50KG&,const OriginalUiIpumTextureCache50KG*,
        OriginalUiRiderModelMaterialDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    std::uint32_t materialCount() const noexcept;
    std::uint32_t passCount(std::uint32_t material) const noexcept;
    const OriginalUiRiderModelMaterialPass50KG* pass(std::uint32_t material,std::uint32_t pass) const noexcept;
    std::shared_ptr<const OriginalUiScreenAssets50KG> images() const noexcept;
private:
    bool load(std::shared_ptr<const void> modelPin,const Scene48D*,
        std::shared_ptr<const OriginalUiArchive50KG>,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<const OriginalUiArchive50KG> podTextures,
        const OriginalUiMaterialRuntime50KG&,const OriginalUiIpumTextureCache50KG*,
        OriginalUiRiderModelMaterialDiagnostic50KG&,bool tournament=false,bool unlock=false) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}

#pragma once
#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"
#include "original_ui_localization50kg.h"
#include "original_ui_atm50kg.h"
#include <array>
namespace rr::assets {
enum class OriginalUiRiderStatAssetError50KG : std::uint8_t {
    none,alreadyLoaded,archive,localization,member,material,animation,images,allocation
};
struct OriginalUiRiderStatAssetDiagnostic50KG {
    OriginalUiRiderStatAssetError50KG error=OriginalUiRiderStatAssetError50KG::none;
    std::uint32_t object=0;
    OriginalUiMaterialDiagnostic50KG material;
    OriginalUiScreenAssetDiagnostic50KG images;
    DecodeError48C atm;
};
enum class OriginalUiRiderStatAsset50KG : std::uint8_t {
    panelLeft,panelRight,notch,medalOutline,medalGold,medalSilver,medalBronze,careerMarker,labelFont
};
struct OriginalUiRiderStatResource50KG {
    std::string registeredName;
    OriginalUiMaterial50KG material;
    // Original stored ATM order, or exactly one real standalone PST image.
    // All indices are owner-local until the combined image bank remaps them.
    std::vector<std::uint32_t> images;
    bool animated=false;
};
// Actual1E81BC/1E89D8..1E9448 cold resource population. Real UIFONTS CEL/PST,
// LOC200..205, Rider PSMs/ATMs/PSTs and source-created missing-PSM ScreenMT.
// No forged text descriptor, substitute portrait, shader admission or GL action.
class OriginalUiRiderStatAssets50KG final {
public:
    OriginalUiRiderStatAssets50KG() noexcept;~OriginalUiRiderStatAssets50KG();
    OriginalUiRiderStatAssets50KG(const OriginalUiRiderStatAssets50KG&)=delete;
    OriginalUiRiderStatAssets50KG& operator=(const OriginalUiRiderStatAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG> rider,
        std::shared_ptr<const OriginalUiArchive50KG> fonts,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        const OriginalUiEnglish50KG&,const OriginalUiMaterialRuntime50KG&,
        OriginalUiRiderStatAssetDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    const OriginalUiRiderStatResource50KG* resource(OriginalUiRiderStatAsset50KG) const noexcept;
    const OriginalUiRiderStatResource50KG* portrait(std::uint8_t rider) const noexcept;
    const std::string* label(std::uint8_t stat) const noexcept;
    const OriginalUiMemberIdentity50KG* localizedTextSource() const noexcept;
    std::shared_ptr<const OriginalUiScreenAssets50KG> images() const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}

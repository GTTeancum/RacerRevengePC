#pragma once
#include "../../../src/game/original_ui_memory_card_geometry50kg.h"
#include <memory>
namespace rr::platform {
class OriginalUiOptionsDrawResources50KG;
struct OriginalUiOptionsMaterialBinding50KG;
struct OriginalUiMemoryCardResourceDiagnostic50KG {
    enum class Error : std::uint8_t {none,alreadyPrepared,identity,material,geometry,allocation} error=Error::none;
    game::OriginalUiMemoryCardGeometryDiagnostic50KG geometry{};
    std::uint32_t sourcePC=0,registryToken=0;
};
// Source page+438 is page-owned, not the shared Controller2 popup. Its source
// registry objects and image bank are the actual retained Options manager's.
// No independent material history, alias, invented DAT, or storage status.
class OriginalUiMemoryCardResources50KG final {
public:
    bool loaded()const noexcept{return bool(images_);}
    const game::OriginalUiMemoryCardGeometry50KG* geometry()const noexcept{return loaded()?&geometry_:nullptr;}
    bool project(const game::OriginalUiMemoryCardRender50KG&,OriginalUiMemoryCardResourceDiagnostic50KG&) noexcept;
    std::uint32_t overlayMaterial()const noexcept{return overlayMaterial_;}
private:
    friend class OriginalUiOptionsDrawResources50KG;
    bool prepare(const game::OriginalUiOptionsMenuWidgets50KG&,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        const std::vector<OriginalUiOptionsMaterialBinding50KG>&,OriginalUiMemoryCardResourceDiagnostic50KG&) noexcept;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images_;
    game::OriginalUiMemoryCardGeometry50KG geometry_;
    std::uint32_t overlayMaterial_=assets::unresolvedMaterialTexture50EA;
};
}

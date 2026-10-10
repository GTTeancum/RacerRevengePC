#pragma once
#include "original_ui_archive50kg.h"
#include "original_ui_font50kg.h"
#include "material_texture_assets50ea.h"
#include "import_texture48e.h"
#include <memory>
#include <vector>

namespace rr::assets {
struct OriginalUiImageSource50KG {
    std::uint64_t identity = 0;
    // Exact authenticated member. Standalone PST uses offset0/size=member.size;
    // ATM frames use the independently recovered stored-PST frame range.
    OriginalUiMemberView50KG member{};
    std::size_t pstOffset = 0, pstBytes = 0;
    // Optional exact same-name CEL for a font atlas. No metrics are generated.
    OriginalUiMemberView50KG cel{};
};
enum class OriginalUiScreenAssetError50KG : std::uint8_t {
    none, alreadyLoaded, sources, texture, font, bank, allocation, unexpectedFailure
};
struct OriginalUiScreenAssetDiagnostic50KG {
    OriginalUiScreenAssetError50KG error = OriginalUiScreenAssetError50KG::none;
    std::size_t image = 0;
    DecodeError48C texture;
    OriginalUiCelDiagnostic50KG font;
    MaterialTextureAssetDiagnostic50EA bank;
};
// Immutable cold owner for a screen's real texture/font resources. Original
// RES members are authenticated upstream; exact stored-PST ranges come from
// source-defined standalone/ATM loaders. No path search, guessed atlas,
// arbitrary image file, material default, sampler policy or GL call occurs.
// Failure publishes neither textures, decoded source evidence nor fonts and
// leaves an empty owner retryable. Reload preserves the published owner.
class OriginalUiScreenAssets50KG final {
public:
    OriginalUiScreenAssets50KG() noexcept;
    ~OriginalUiScreenAssets50KG();
    OriginalUiScreenAssets50KG(const OriginalUiScreenAssets50KG&)=delete;
    OriginalUiScreenAssets50KG& operator=(const OriginalUiScreenAssets50KG&)=delete;
    bool load(const std::vector<OriginalUiImageSource50KG>&,
        OriginalUiScreenAssetDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    const MaterialTextureAssets50EA* textures() const noexcept;
    // Exact decodeTexture48E result, including original format, palette, all
    // mips and raw alpha. Index matches sources[index] and textures()->texture(index),
    // whose Texture48B is parsed from this same result's base_rrtex bytes.
    // Null means unloaded or invalid index. Immutable borrowed access; keep
    // this owner alive. No material admission or GPU readiness is implied.
    const Texture48E* decodedTexture(std::uint32_t index) const noexcept;
    const OriginalUiFont50KG* font(std::uint32_t index) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}

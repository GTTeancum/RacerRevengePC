#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/game/original_ui_rider_stat_owner50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include <memory>
#include <vector>
namespace rr::platform {
struct OriginalUiRiderStatDrawPacket50KG {
    std::uint32_t firstQuad=0,quadCount=0,imageIndex=0,objectOffset=0,sourcePC=0;
    const assets::OriginalUiRiderStatResource50KG* resource=nullptr;
    assets::OriginalUiNativeMaterial50KG policy{};
};
struct OriginalUiRiderStatDrawDiagnostic50KG {
    enum class Error:std::uint8_t {none,input,resource,mapping,material,geometry,capacity,allocation} error=Error::none;
    std::uint32_t objectOffset=0;
    assets::OriginalUiMaterialDiagnostic50KG material;
};
// Real custom ScreenTexture/ScreenText -> native vertex packets. This is only
// the page's stat contribution. No sealed complete-frame publication is exposed.
// Pins actual cold assets and exact combined image bank for every borrowed pass.
class OriginalUiRiderStatDraw50KG final {
public:
    bool project(std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG>,
        const game::OriginalUiRiderStatFrame50KG&,
        std::shared_ptr<const assets::OriginalUiImageBank50KG>,std::uint32_t combinedOwner,
        OriginalUiRiderStatDrawDiagnostic50KG&) noexcept;
    void drain() noexcept;
    const std::vector<assets::Vertex48B>& vertices()const noexcept{return vertices_;}
    const std::vector<OriginalUiRiderStatDrawPacket50KG>& packets()const noexcept{return packets_;}
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank()const noexcept{return bank_;}
private:
    std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> assets_;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank_;
    std::vector<assets::Vertex48B> vertices_;
    std::vector<OriginalUiRiderStatDrawPacket50KG> packets_;
};
}

#pragma once
#include "original_ui_memory_card50kg.h"
#include "original_ui_options_menu_widgets50kg.h"
#include "original_ui_text_geometry50kg.h"
#include "original_ui_main_layout50kg.h"
#include <array>
#include <memory>
namespace rr::game {
enum class OriginalUiMemoryCardGeometryError50KG : std::uint8_t {
    none,alreadyPrepared,notPrepared,identity,resource,text,render,unsupported,allocation
};
struct OriginalUiMemoryCardGeometryDiagnostic50KG {
    OriginalUiMemoryCardGeometryError50KG error=OriginalUiMemoryCardGeometryError50KG::none;
    OriginalUiTextGeometryDiagnostic50KG text{};
    std::uint32_t sourcePC=0;std::uint16_t widgetId=0;
};
struct OriginalUiMemoryCardCloneLine50KG {
    std::string text;
    std::uint8_t capacity5B=0,characterCount5C=0,allocatedPackets5D=0;
    std::int16_t alpha58=255;
    std::uint32_t color54=0;
};
struct OriginalUiMemoryCardTemplate50KG {
    std::uint16_t widgetId=0;
    //2037D0 builds independent ScreenText arrays, not live widget draw aliases.
    std::vector<OriginalUiMemoryCardCloneLine50KG> lines;
    std::unique_ptr<OriginalUiTextGeometry50KG> geometry;
};
// Only state4 with all nine dynamic IDs=-1 is admitted. Template clone arrays
// are real cold source ownership but never displayed as fabricated card status.
// Source direct-video sprite is a separate mandatory native command owner.
class OriginalUiMemoryCardGeometry50KG final {
public:
    using Diagnostic=OriginalUiMemoryCardGeometryDiagnostic50KG;
    bool prepare(const OriginalUiOptionsMenuWidgets50KG&,const assets::OriginalUiScreenAssets50KG&,
        std::uint32_t fontImage,std::uint32_t overlayImage,Diagnostic&) noexcept;
    bool project(const OriginalUiMemoryCardRender50KG&,Diagnostic&) noexcept;
    bool prepared()const noexcept{return prepared_;}
    const std::array<OriginalUiMemoryCardTemplate50KG,9>& templates()const noexcept{return templates_;}
    const std::array<OriginalUiOverlayQuad50KG,3>& strips()const noexcept{return strips_;}
    std::size_t stripCount()const noexcept{return stripCount_;}
private:
    bool prepared_=false;
    float inverseWidth_=0,inverseHeight_=0;
    std::array<OriginalUiMemoryCardTemplate50KG,9> templates_;
    std::array<OriginalUiOverlayQuad50KG,3> strips_;
    std::size_t stripCount_=0;
};
}

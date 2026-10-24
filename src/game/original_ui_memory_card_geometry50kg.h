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
    //2037D0/201970 build independent ScreenText arrays, not live widget draw aliases.
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
    // Deliberately private and uncalled: a resource snapshot of the bounded
    // 1F6F0C Load tuple after a genuine 1FF8B0 reset, not storage authority.
    // Dynamic201970 arrays are independent of static2037D0 templates, even
    // when both use widget23. No arbitrary-ID replacement/negative-ID clearing
    // is exposed; the original retains arrays for changed negative IDs.
    struct LoadCloneSet {
        std::array<std::int32_t,9> textIds{{24,32,23,26,27,30,31,-1,-1}};
        // +2E0,+2E8,+2F0,+2F8,+300,+308,+30C, in textIds order.
        std::array<OriginalUiMemoryCardTemplate50KG,7> arrays;
        std::array<OriginalUiOverlayQuad50KG,2> icons; // +160 X, +1C0 Triangle
    };
    enum class LoadDrawKind : std::uint8_t { textArray,cross,triangle };
    struct LoadDrawItem {
        LoadDrawKind kind=LoadDrawKind::textArray;
        std::uint8_t arrayIndex=0;
        std::uint32_t sourcePC=0;
    };
    struct LoadDrawRecipe {
        std::array<LoadDrawItem,5> items{};
        std::size_t count=0;
    };
    // Allocate all seven owners before publishing output. Caller must retain
    // the authentic image bank and later retire this set only after its last
    // accepted/retired frame, including any failed contribution custody.
    bool prepareLoadClones(const OriginalUiOptionsMenuWidgets50KG&,
        const assets::OriginalUiScreenAssets50KG&,std::uint32_t fontImage,
        std::uint32_t crossImage,std::uint32_t triangleImage,
        std::unique_ptr<LoadCloneSet>& output,Diagnostic&) const noexcept;
    // Text/icon recipe ONLY. Does not replace valid(), render(), project(),
    // pre-render material effects, direct-video, strips, or frame acceptance.
    static bool describeLoadDraw(const LoadCloneSet&,const OriginalUiMemoryCardState50KG&,
        std::uint32_t actualDialogFlags34,LoadDrawRecipe&,Diagnostic&) noexcept;
    bool prepared_=false;
    float inverseWidth_=0,inverseHeight_=0;
    std::array<OriginalUiMemoryCardTemplate50KG,9> templates_;
    std::array<OriginalUiOverlayQuad50KG,3> strips_;
    std::size_t stripCount_=0;
};
}

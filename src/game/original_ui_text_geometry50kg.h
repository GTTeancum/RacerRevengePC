#pragma once
#include "original_ui_text50kg.h"
#include "original_ui_text_position50kg.h"
#include <vector>

namespace rr::game {
struct OriginalUiPositionedLine50KG {
    OriginalUiTextLayout50KG glyphs;
    OriginalUiTextPosition50KG position;
    std::int32_t measuredWidth=0, measuredHeight=0;
};
struct OriginalUiTextGeometryDiagnostic50KG {
    std::size_t line=0;
    OriginalUiTextDiagnostic50KG glyph;
    enum class Error : std::uint8_t { none, alreadyPrepared, input, glyph, metric, position, allocation } error=Error::none;
};
// Cold geometry for the source's scale1/angle0 text construction. Descriptor,
// CEL and atlas belong to immutable authentic owners; only the exact atlas
// pointer is retained in glyph output. Widget callbacks own current color,
// opacity and enable state separately, and cannot rewrite this geometry.
class OriginalUiTextGeometry50KG final {
public:
    OriginalUiTextGeometry50KG()=default;
    OriginalUiTextGeometry50KG(const OriginalUiTextGeometry50KG&)=delete;
    OriginalUiTextGeometry50KG& operator=(const OriginalUiTextGeometry50KG&)=delete;
    bool prepare(const assets::OriginalUiOverlayText50KG&,const assets::OriginalUiFont50KG&,
        const assets::Texture48B&,OriginalUiTextGeometryDiagnostic50KG&) noexcept;
    // Source SetText geometry. Keeps the authentic dynamic descriptor identity
    // and checks its original line count/capacity; does not invent static text.
    bool prepareDynamic(const assets::OriginalUiOverlayText50KG&,const std::vector<std::string>&,
        const assets::OriginalUiFont50KG&,const assets::Texture48B&,OriginalUiTextGeometryDiagnostic50KG&) noexcept;
    bool ready()const noexcept{return ready_;}
    std::uint16_t widgetId()const noexcept{return widgetId_;}
    const std::vector<OriginalUiPositionedLine50KG>& lines()const noexcept{return lines_;}
private:
    bool prepareLines(const assets::OriginalUiOverlayText50KG&,const std::vector<std::string>&,
        const assets::OriginalUiFont50KG&,const assets::Texture48B&,OriginalUiTextGeometryDiagnostic50KG&) noexcept;
    bool ready_=false;
    std::uint16_t widgetId_=0;
    std::vector<OriginalUiPositionedLine50KG> lines_;
};
}

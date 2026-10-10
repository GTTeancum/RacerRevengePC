#include "original_ui_text_geometry50kg.h"
#include <new>
#include <utility>

namespace rr::game {
bool OriginalUiTextGeometry50KG::prepare(const assets::OriginalUiOverlayText50KG& widget,
    const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas,
    OriginalUiTextGeometryDiagnostic50KG& d) noexcept {
    d={};using E=OriginalUiTextGeometryDiagnostic50KG::Error;
    if(ready_){d.error=E::alreadyPrepared;return false;}
    // Dynamic text must use a separately owned source SetText operation; never
    // freeze an empty dynamic label and claim that its live owner is joined.
    // Widget identity/ordinal admission belongs to the authenticated descriptor
    // and page membership owner. Zero is not an uninitialized-state sentinel.
    if(!widget.identity||widget.lines.size()>64||
        widget.textSource==assets::OriginalUiTextSource50KG::dynamic){d.error=E::input;return false;}
    return prepareLines(widget,widget.lines,font,atlas,d);
}
bool OriginalUiTextGeometry50KG::prepareDynamic(const assets::OriginalUiOverlayText50KG& widget,
    const std::vector<std::string>& lines,const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas,
    OriginalUiTextGeometryDiagnostic50KG& d) noexcept {
    d={};using E=OriginalUiTextGeometryDiagnostic50KG::Error;
    if(ready_){d.error=E::alreadyPrepared;return false;}
    if(!widget.identity||widget.textSource!=assets::OriginalUiTextSource50KG::dynamic||
        lines.size()!=widget.lines.size()||lines.size()>64||widget.dynamicLineLength<0||widget.dynamicLineLength>255){d.error=E::input;return false;}
    for(const auto& text:lines)if(text.size()>static_cast<std::size_t>(widget.dynamicLineLength)||text.find('\0')!=std::string::npos){d.error=E::input;return false;}
    return prepareLines(widget,lines,font,atlas,d);
}
bool OriginalUiTextGeometry50KG::prepareLines(const assets::OriginalUiOverlayText50KG& widget,
    const std::vector<std::string>& texts,const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas,
    OriginalUiTextGeometryDiagnostic50KG& d) noexcept {
    using E=OriginalUiTextGeometryDiagnostic50KG::Error;
    try {
        // The original SetText writes live strings, while authored positioning
        // and resource identities remain unchanged. Position's empty-line gate
        // must observe that live string, not the initially empty descriptor.
        auto positioned=widget;positioned.lines=texts;
        std::vector<OriginalUiPositionedLine50KG> next(texts.size());
        for(std::size_t i=0;i<next.size();++i){
            d.line=i;const auto& text=texts[i];auto& line=next[i];
            OriginalUiTextInput50KG input{&font,&atlas,
                reinterpret_cast<const std::uint8_t*>(text.data()),text.size(),0,0};
            if(!layoutOriginalUiText50KG(input,line.glyphs,d.glyph)){d.error=E::glyph;return false;}
            if(!roundOriginalUiTextMetric50KG(line.glyphs.widthBeforeScaleAndRounding,1,line.measuredWidth)||
                !roundOriginalUiTextMetric50KG(line.glyphs.firstGlyphHeightBeforeScaleAndRounding,1,line.measuredHeight)){
                d.error=E::metric;return false;}
            const OriginalUiTextPositionInput50KG p{&positioned,i,line.glyphs.fontLineHeight,
                line.measuredWidth,line.measuredHeight};
            if(!positionOriginalUiText50KG(p,line.position)){d.error=E::position;return false;}
            // Run the source glyph arithmetic at the real positioned origin;
            // translating already-rounded corners would change half-pixel math.
            input.localX=line.position.positionedX;input.localY=line.position.positionedY;
            if(!layoutOriginalUiText50KG(input,line.glyphs,d.glyph)){d.error=E::glyph;return false;}
        }
        lines_=std::move(next);widgetId_=widget.widgetId;ready_=true;d={};return true;
    }catch(const std::bad_alloc&){d.error=E::allocation;return false;}
    catch(...){d.error=E::allocation;return false;}
}
}

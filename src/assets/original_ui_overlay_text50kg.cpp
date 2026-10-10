#include "original_ui_overlay_text50kg.h"
#include <cmath>
#include <utility>

namespace rr::assets {
namespace {
bool fail(OriginalUiPageDiagnostic50KG& d,OriginalUiPageError50KG e,
    const OriginalUiDatSection50KG& s,const std::string& key,std::uint32_t pc) noexcept {
    const auto* v=s.find(key);d={e,v?v->valueOffset:s.sourceOffset,pc};return false;
}
std::uint32_t truncateColor(float f) noexcept {
    // Source CVT.W.S truncates; color() admits only finite [0,255] inputs.
    return static_cast<std::uint32_t>(static_cast<std::int32_t>(f));
}
bool color(const OriginalUiDatSection50KG& s,const char* key,float fallback,
    std::uint32_t& output,OriginalUiPageDiagnostic50KG& d,std::uint32_t pc) noexcept {
    std::array<float,3> v{};
    if (!originalUiTriple50KG(s,key,{{fallback,fallback,fallback}},v,d,pc)) return false;
    for (float f:v) if (!std::isfinite(f) || f<0 || f>255)
        return fail(d,OriginalUiPageError50KG::unsupportedValue,s,key,pc);
    output=0xff000000u | (truncateColor(v[0])<<16) | (truncateColor(v[1])<<8) | truncateColor(v[2]);
    return true;
}
bool flag(const OriginalUiDatSection50KG& s,const char* key,bool& value,
    OriginalUiPageDiagnostic50KG& d,std::uint32_t pc) noexcept {
    return originalUiBoolean50KG(s,key,false,value,d,pc);
}
bool split(const std::string& text,OriginalUiOverlayText50KG& result,
    const OriginalUiDatSection50KG& s,const char* key,OriginalUiPageDiagnostic50KG& d) {
    std::size_t at=0;
    do {
        const auto found=text.find('@',at);const auto end=found==std::string::npos?text.size():found;
        if (end-at>255 || result.lines.size()>=64)
            return fail(d,OriginalUiPageError50KG::capacity,s,key,0x2A9AD0);
        result.lines.push_back(text.substr(at,end-at));
        if (found==std::string::npos) break;
        at=end+1;
    } while (at<=text.size());
    result.usesExplicitHeight=result.height!=0;return true;
}
}
bool readOriginalUiOverlayText50KG(const OriginalUiDat50KG& widgets,
    std::uint16_t widgetId,const OriginalUiEnglish50KG* english,
    OriginalUiOverlayText50KG& output,OriginalUiPageDiagnostic50KG& d) noexcept {
    if (!widgets.identity()) {d={OriginalUiPageError50KG::missingMember,0,0x2A9000};return false;}
    try {
        const auto* s=widgets.section("Widget "+std::to_string(widgetId));
        if (!s) {d={OriginalUiPageError50KG::missingSection,0,0x2A9000};return false;}
        OriginalUiOverlayText50KG r;r.identity=widgets.identity();r.widgetId=widgetId;
        const auto* type=s->find("Type");
        if (!type || type->text!="Overlay_Text") return fail(d,OriginalUiPageError50KG::unsupportedValue,*s,"Type",0x2A4748);
        const auto* font=s->find("FontName");
        if (!font || font->text.empty()) return fail(d,OriginalUiPageError50KG::missingKey,*s,"FontName",0x2A90B0);
        r.fontName=font->text;const auto* material=s->find("MaterialName");
        r.materialName=material?material->text:r.fontName;
        if (r.fontName.size()>127 || r.materialName.empty() || r.materialName.size()>127)
            return fail(d,OriginalUiPageError50KG::capacity,*s,"FontName",0x2A90B0);
        if (!originalUiInteger50KG(*s,"XPosition",0,r.x,d,0x2A90E4) ||
            !originalUiInteger50KG(*s,"YPosition",0,r.y,d,0x2A90FC) ||
            !originalUiInteger50KG(*s,"Height",0,r.height,d,0x2A9380) ||
            !originalUiInteger50KG(*s,"Alpha",255,r.alpha,d,0x2A91D0) ||
            !originalUiNumber50KG(*s,"LeadingFactor",1,r.leadingFactor,d,0x2A9138) ||
            !originalUiNumber50KG(*s,"Depth",1,r.depth,d,0x2A91EC) ||
            !color(*s,"Color",40,r.color,d,0x2A921C) ||
            !color(*s,"HighlightColor",255,r.highlightColor,d,0x2A9290) ||
            !color(*s,"DisabledColor",128,r.disabledColor,d,0x2A9314) ||
            !flag(*s,"EnableOnSelect",r.enableOnSelect,d,0x2A9038) ||
            !flag(*s,"DisableOnSelect",r.disableOnSelect,d,0x2A9050) ||
            !flag(*s,"EnableOnDeSelect",r.enableOnDeselect,d,0x2A9068) ||
            !flag(*s,"DisableOnDeSelect",r.disableOnDeselect,d,0x2A9080) ||
            !flag(*s,"WantsJoystickInput1",r.wantsJoystickInput[0],d,0x2A44C0) ||
            !flag(*s,"WantsJoystickInput2",r.wantsJoystickInput[1],d,0x2A44C0) ||
            !flag(*s,"OnTop",r.onTop,d,0x1CD890)) return false;
        if (r.x<-32768 || r.x>32767 || r.y<-32768 || r.y>32767 || r.height<-32768 || r.height>32767 ||
            !std::isfinite(r.leadingFactor) || !std::isfinite(r.depth))
            return fail(d,OriginalUiPageError50KG::unsupportedValue,*s,"XPosition",0x2A90E4);
        r.hasHighlightColor=s->find("HighlightColor")!=nullptr;r.hasDisabledColor=s->find("DisabledColor")!=nullptr;
        if (const auto* v=s->find("Justification")) {
            if (v->text=="Right") r.justification=OriginalUiTextJustification50KG::right;
            else if (v->text=="CenterBoth") r.justification=OriginalUiTextJustification50KG::centerBoth;
            else if (v->text=="CenterVertical") r.justification=OriginalUiTextJustification50KG::centerVertical;
            else if (v->text=="CenterHorizontal") r.justification=OriginalUiTextJustification50KG::centerHorizontal;
            // Source strcmp chain falls back to Left for any other string.
        }
        if (const auto* id=s->find("TextID")) {
            r.textSource=OriginalUiTextSource50KG::localized;r.textId=id->text;
            const auto* e=english?english->find(r.textId):nullptr;
            if (!e) return fail(d,OriginalUiPageError50KG::reference,*s,"TextID",0x2A9458);
            if (!split(e->text,r,*s,"TextID",d)) return false;
        } else if (s->find("TextDynamicLineLength")) {
            r.textSource=OriginalUiTextSource50KG::dynamic;std::int32_t count=0;
            if (!originalUiInteger50KG(*s,"TextDynamicLineLength",0,r.dynamicLineLength,d,0x2A9174) ||
                !originalUiInteger50KG(*s,"TextDynamicLineCount",0,count,d,0x2A9190)) return false;
            if (count<0 || count>64 || r.dynamicLineLength<0 || r.dynamicLineLength>255)
                return fail(d,OriginalUiPageError50KG::capacity,*s,"TextDynamicLineCount",0x2A9568);
            r.lines.resize(static_cast<std::size_t>(count));
        } else if (s->find("NumberOfLines")) {
            r.textSource=OriginalUiTextSource50KG::authoredLines;std::int32_t count=0;
            if (!originalUiInteger50KG(*s,"NumberOfLines",1,count,d,0x2A91B4)) return false;
            if (count<0 || count>64) return fail(d,OriginalUiPageError50KG::capacity,*s,"NumberOfLines",0x2A9740);
            for (std::int32_t i=1;i<=count;++i) {
                const auto key="Line"+std::to_string(i);const auto* v=s->find(key);
                if (!v) return fail(d,OriginalUiPageError50KG::missingKey,*s,key,0x2A9778);
                if (v->text.size()>255) return fail(d,OriginalUiPageError50KG::capacity,*s,key,0x2A9778);
                r.lines.push_back(v->text);
            }
        } else {
            const auto* text=s->find("Text");
            if (!text) return fail(d,OriginalUiPageError50KG::missingKey,*s,"Text",0x2A9950);
            if (r.height) { if (!split(text->text,r,*s,"Text",d)) return false; }
            else {
                if (text->text.size()>255) return fail(d,OriginalUiPageError50KG::capacity,*s,"Text",0x2A9950);
                r.lines.push_back(text->text);
            }
        }
        output=std::move(r);d={};return true;
    } catch (...) { d={OriginalUiPageError50KG::allocation,0,0x2A9000};return false; }
}
}

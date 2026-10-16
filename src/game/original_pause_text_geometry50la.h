#pragma once
#include "original_pause_storage50la.h"
#include "original_ui_text50kg.h"
#include "original_ui_text_position50kg.h"
#include <cmath>

namespace rr::game {
// Serial CPU leaf only. No ownership, text setter, visibility, callback or GPU
// receipt. The caller retains Pause for each complete call. Object0 is base180;
// objects1..13 are title2D0, prompt340, then the11 original list texts.
class OriginalPauseTextGeometry50LA final {
    using Storage=OriginalPauseStorage50LA;
    using Text=Storage::Text;
    static Text* text(Storage& owner,std::size_t object)noexcept {
        if(owner.stage_!=Storage::Stage::derivedResources||!owner.context_||
           !owner.context_->live()||object>owner.derivedTexts_.size())return nullptr;
        return object?&owner.derivedTexts_[object-1]:&owner.baseText_;
    }
    static bool layout(const Text& t,float x,float y,OriginalUiTextLayout50KG& out,
        OriginalUiTextDiagnostic50KG& d)noexcept {
        if(!t.font4C||!t.string8||t.length5C>t.capacity5B||t.string8[t.length5C]||
           !std::isfinite(t.field40)||t.field40<=0)return false;
        const auto* atlas=t.font4C->texture281F90();
        if(!atlas||!atlas->sourceWidth14()||!atlas->sourceHeight18()||
           *atlas->sourceWidth14()<=0||*atlas->sourceHeight18()<=0)return false;
        OriginalUiTextInput50KG input{&t.font4C->metrics(),nullptr,
            reinterpret_cast<const std::uint8_t*>(t.string8.get()),t.length5C,x,y};
        return layoutOriginalUiTextDimensions50LA(input,
            static_cast<std::uint32_t>(*atlas->sourceWidth14()),
            static_cast<std::uint32_t>(*atlas->sourceHeight18()),out,d);
    }
public:
    struct Diagnostic {std::uint32_t sourcePC=0;std::size_t object=0;OriginalUiTextDiagnostic50KG glyph;};
    // Original2E16F0 result after source scale and CVT.W.S. Selected width
    // resolves the private original880 selection, never a host menu index.
    static bool selectedWidth2E16F0(Storage& owner,std::int32_t& width,Diagnostic& d)noexcept {
        if(owner.selection880_>=11){d={0x2E16F0,0,{}};return false;}
        return width2E16F0(owner,3+owner.selection880_,width,d);
    }
    static bool width2E16F0(Storage& owner,std::size_t object,std::int32_t& width,Diagnostic& d)noexcept {
        d={0x2E16F0,object,{}};auto* t=text(owner,object);if(!t)return false;
        OriginalUiTextLayout50KG run;
        if(!layout(*t,0,0,run,d.glyph)||
           !roundOriginalUiTextMetric50KG(run.widthBeforeScaleAndRounding,t->field40,width))return false;
        d.sourcePC=0;return true;
    }
    static std::size_t activeVertices(const Storage::Text& t,std::size_t packet)noexcept {
        if(packet>=t.active5E)return 0;
        const auto first=packet*36;
        return t.length5C>first?2*std::min<std::size_t>(36,t.length5C-first):0;
    }
    // Bounded zero-rotation2E0170 ->2E0F50 ->2E0A30 regeneration. Native
    // validation precedes writes. Unsupported rotation rejects, never flattens.
    static bool regenerate2E0170(Storage& owner,std::size_t object,
        std::uint32_t actualVideoMaximum60,Diagnostic& d)noexcept {
        d={0x2E0170,object,{}};auto* t=text(owner,object);
        if(!t||actualVideoMaximum60!=0xffffffu||t->field44!=0||t->field48!=1||
           !std::isfinite(t->x10)||!std::isfinite(t->y14)||t->justification50>4||
           !t->glyphsC||!t->packets64||!t->primitives4||t->packetCount5D!=2||t->count0!=2||
           (t->capacity5B!=40&&t->capacity5B!=60))return false;
        if(!(t->flags5A&3u)){d.sourcePC=0;return true;}
        OriginalUiTextLayout50KG run;
        if(!layout(*t,0,0,run,d.glyph))return false;
        float x=t->x20,y=t->y24;
        if(t->flags5A&1u){
            x=t->x10;y=t->y14;
            if(t->length5C&&t->justification50){
                std::int32_t width=0,height=0,coordinate=0;
                if(!roundOriginalUiTextMetric50KG(run.widthBeforeScaleAndRounding,t->field40,width)||
                   !roundOriginalUiTextMetric50KG(run.firstGlyphHeightBeforeScaleAndRounding,t->field40,height))return false;
                const auto j=t->justification50;
                if(j==1||j==3||j==4){
                    volatile float shift=j==4?float(width):float(width)*.5f;
                    if(!roundOriginalUiTextMetric50KG(x-shift,1,coordinate))return false;
                    x=float(coordinate);
                }
                if(j==2||j==3){
                    volatile float shift=float(height)*.5f;
                    if(!roundOriginalUiTextMetric50KG(y-shift,1,coordinate))return false;
                    y=float(coordinate);
                }
            }
        }
        const bool inverse=(t->flags5A&5u)!=0;
        if(!layout(*t,inverse?x/t->field40:x,inverse?y/t->field40:y,run,d.glyph))return false;
        for(std::size_t p=0;p<2;++p){
            const auto expected=2*(p?t->capacity5B-36:36);
            if(!t->primitives4[p]||t->primitives4[p]->packet10!=&t->packets64[p]||
               !t->packets64[p].vertices||t->packets64[p].count!=std::size_t(expected))return false;
        }
        for(std::size_t n=0;n<t->length5C;++n)for(std::size_t v=0;v<2;++v){
            const auto& point=run.quads[n].vertices[v?2:0];
            if(t->glyphsC[n].vertices[v].packet!=&t->packets64[n/36].vertices[(n%36)*2+v]||
               !std::isfinite(point.x*t->field40)||!std::isfinite(point.y*t->field40))return false;
        }
        t->active5E=std::uint8_t((t->length5C+35u)/36u);t->x20=x;t->y24=y;
        const float z=float(actualVideoMaximum60)-t->field48;
        for(std::size_t n=0;n<t->length5C;++n)for(std::size_t v=0;v<2;++v){
            auto& vertex=t->glyphsC[n].vertices[v];const auto& point=run.quads[n].vertices[v?2:0];
            vertex.authored00[0]=point.x;vertex.authored00[1]=point.y;vertex.authored00[2]=z;
            vertex.packet->stq[0]=point.u;vertex.packet->stq[1]=point.v;
            vertex.packet->xyz=vertex.authored00;
            vertex.packet->xyz[0]=point.x*t->field40;vertex.packet->xyz[1]=point.y*t->field40;
        }
        t->flags5A=std::uint8_t(t->flags5A&~7u);d.sourcePC=0;return true;
    }
};
}

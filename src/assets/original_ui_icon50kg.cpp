#include "original_ui_icon50kg.h"
#include "../game/original_ui_text_position50kg.h"
#include <limits>
#include <utility>

namespace rr::assets {
namespace {
bool fail(OriginalUiPageDiagnostic50KG& d,OriginalUiPageError50KG e,
    const OriginalUiDatSection50KG& s,const char* key,std::uint32_t pc) noexcept {
    const auto* v=s.find(key);d={e,v?v->valueOffset:s.sourceOffset,pc};return false;
}
bool integerToken(const std::string& s,std::size_t begin,std::size_t end,std::int32_t& out) noexcept {
    if(begin==end)return false;bool neg=s[begin]=='-';if(neg||s[begin]=='+')++begin;
    if(begin==end)return false;std::uint64_t n=0;
    for(auto i=begin;i<end;++i){if(s[i]<'0'||s[i]>'9')return false;
        n=n*10+unsigned(s[i]-'0');if(n>std::uint64_t(std::numeric_limits<std::int32_t>::max())+unsigned(neg))return false;}
    out=static_cast<std::int32_t>(neg?-static_cast<std::int64_t>(n):static_cast<std::int64_t>(n));return true;
}
bool rect(const OriginalUiDatSection50KG& s,const char* key,OriginalUiIconRect50KG& out,
    bool& present,OriginalUiPageDiagnostic50KG& d,std::uint32_t pc) {
    const auto* value=s.find(key);present=value!=nullptr;if(!value)return true;
    if(value->text.size()>127)return fail(d,OriginalUiPageError50KG::capacity,s,key,pc);
    std::int32_t v[4]{};std::size_t at=0;
    for(unsigned i=0;i<4;++i){const auto end=i==3?value->text.size():value->text.find(',',at);
        if(end==std::string::npos||!integerToken(value->text,at,end,v[i]))
            return fail(d,OriginalUiPageError50KG::integerSyntax,s,key,pc);
        at=end+1;}
    out={v[0],v[2],v[1],v[3]};return true;
}
bool flag(const OriginalUiDatSection50KG& s,const char* key,bool& out,
    OriginalUiPageDiagnostic50KG& d,std::uint32_t pc) {
    return originalUiBoolean50KG(s,key,false,out,d,pc);
}
}
bool readOriginalUiIcon50KG(const OriginalUiDat50KG& widgets,std::uint16_t id,
    OriginalUiIcon50KG& output,OriginalUiPageDiagnostic50KG& d) noexcept {
    if(!widgets.identity()){d={OriginalUiPageError50KG::missingMember,0,0x2ac590};return false;}
    try {
        const auto* s=widgets.section("Widget "+std::to_string(id));
        if(!s){d={OriginalUiPageError50KG::missingSection,0,0x2ac590};return false;}
        const auto* type=s->find("Type");
        if(!type||type->text!="Icon")return fail(d,OriginalUiPageError50KG::unsupportedValue,*s,"Type",0x2a4798);
        OriginalUiIcon50KG r;r.identity=widgets.identity();r.widgetId=id;
        const auto* texture=s->find("Texture");
        if(!texture||texture->text.empty()||texture->text=="*INVALID*")
            return fail(d,OriginalUiPageError50KG::missingKey,*s,"Texture",0x2ac650);
        if(texture->text.size()>259)return fail(d,OriginalUiPageError50KG::capacity,*s,"Texture",0x2ac650);
        r.textureName=texture->text;r.materialName=r.textureName;
        // Source1169D0 searches backward, and2AC680 zeros the final dot suffix.
        const auto dot=r.materialName.rfind('.');if(dot!=std::string::npos)r.materialName.resize(dot);
        if(r.materialName.empty())return fail(d,OriginalUiPageError50KG::reference,*s,"Texture",0x2ac6b0);
        if(!flag(*s,"EnableOnSelect",r.enableOnSelect,d,0x2ac5c8)||
            !flag(*s,"DisableOnSelect",r.disableOnSelect,d,0x2ac5e0)||
            !flag(*s,"EnableOnDeSelect",r.enableOnDeselect,d,0x2ac5f8)||
            !flag(*s,"DisableOnDeSelect",r.disableOnDeselect,d,0x2ac610)||
            !flag(*s,"WantsJoystickInput1",r.wantsJoystickInput[0],d,0x2a44c0)||
            !flag(*s,"WantsJoystickInput2",r.wantsJoystickInput[1],d,0x2a44c0)||
            !originalUiNumber50KG(*s,"Angle",0,r.angle,d,0x2ac700))return false;
        r.hasAngle=s->find("Angle")!=nullptr;
        if(r.hasAngle&&(!originalUiNumber50KG(*s,"PivotX",0,r.pivotX,d,0x2ac720)||
            !originalUiNumber50KG(*s,"PivotY",0,r.pivotY,d,0x2ac738)))return false;
        if(!rect(*s,"PartialRect",r.partialRect,r.hasPartialRect,d,0x2ac750)||
            !rect(*s,"MapRect",r.mapRect,r.hasMapRect,d,0x2ac774))return false;
        if(!r.hasMapRect&&(!originalUiInteger50KG(*s,"XLocation",0,r.x,d,0x2ac794)||
            !originalUiInteger50KG(*s,"YLocation",0,r.y,d,0x2ac7ac)))return false;
        if(!originalUiInteger50KG(*s,"Alpha",255,r.alpha,d,0x2ac7f4)||
            !originalUiNumber50KG(*s,"Depth",1,r.depth,d,0x2ac810))return false;
        if(r.alpha<-32768||r.alpha>32767)return fail(d,OriginalUiPageError50KG::unsupportedValue,*s,"Alpha",0x2aca04);
        if(const auto* color=s->find("Color"))if(color->text.size()>127)
            return fail(d,OriginalUiPageError50KG::capacity,*s,"Color",0x158c20);
        std::array<float,3> rgb{};if(!originalUiTriple50KG(*s,"Color",{{200,200,200}},rgb,d,0x2ac840))return false;
        std::uint32_t color=0xff000000;
        for(unsigned i=0;i<3;++i){std::int32_t channel=0;
            if(!game::roundOriginalUiTextMetric50KG(rgb[i],1,channel)||channel<0||channel>255)
                return fail(d,OriginalUiPageError50KG::unsupportedValue,*s,"Color",0x2acabc);
            color|=std::uint32_t(channel)<<(16-8*i);}
        r.color=color;output=std::move(r);d={};return true;
    }catch(...){d={OriginalUiPageError50KG::allocation,0,0x2ac590};return false;}
}
}

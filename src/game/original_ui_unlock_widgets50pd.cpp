#include "original_ui_unlock_widgets50pd.h"
#include "../assets/original_ui_overlay_text50kg.h"
#include <utility>

namespace rr::game {
namespace {
using E=OriginalUiUnlockWidgetError50PD;using D=OriginalUiUnlockWidgetDiagnostic50PD;using K=OriginalUiUnlockWidgetKind50PD;
using O=OriginalUiControlOperation50KG;using T=OriginalUiTextWidgetStatus50KG;
bool fail(D& d,E e) noexcept {d.error=e;return false;}
bool idOk(std::uint16_t id) noexcept {return id>0&&id<originalUiUnlockWidgetStorageCount50PD;}
bool operation(O o) noexcept {
    switch(o){case O::show:case O::hide:case O::select:case O::deselect:case O::clearSelection:case O::setEnabled:case O::reset:return true;}
    return false;
}
// Authored UIUnlock layout: 1..4 text, 5..7 Icon, 8 Empty, 9..23 User_Rotate,
// 24..38 Soultree_Character, 39..53 Huttese Overlay_Text.
K expected(std::uint16_t id) noexcept {
    if(id<=4)return K::text;if(id<=7)return K::icon;if(id==8)return K::empty;
    if(id<=23)return K::userRotate;if(id<=38)return K::character;return K::text;
}
struct Visibility {bool enableOnSelect=false,disableOnSelect=false,enableOnDeselect=false,disableOnDeselect=false;};
}
struct OriginalUiUnlockWidgets50PD::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;assets::OriginalUiDat50KG widgets;
    std::array<OriginalUiUnlockWidgetState50PD,originalUiUnlockWidgetStorageCount50PD> states{};
    std::array<assets::OriginalUiOverlayText50KG,originalUiUnlockWidgetStorageCount50PD> texts;
    std::array<assets::OriginalUiIcon50KG,originalUiUnlockWidgetStorageCount50PD> icons;
    std::array<OriginalUiTextWidget50KG,originalUiUnlockWidgetStorageCount50PD> textWidgets;
    std::array<Visibility,originalUiUnlockWidgetStorageCount50PD> models{};
    std::array<std::vector<std::string>,originalUiUnlockWidgetStorageCount50PD> dynamic;
    struct Image{std::shared_ptr<const assets::OriginalUiScreenAssets50KG> owner;std::uint32_t index=0;OriginalUiIconLayout50KG layout{};};
    std::array<Image,originalUiUnlockWidgetStorageCount50PD> images;
    std::vector<std::uint16_t> ordinary,top;
};
OriginalUiUnlockWidgets50PD::OriginalUiUnlockWidgets50PD() noexcept=default;
OriginalUiUnlockWidgets50PD::~OriginalUiUnlockWidgets50PD()=default;
bool OriginalUiUnlockWidgets50PD::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const assets::OriginalUiEnglish50KG* english,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::unlock))
        return fail(d,E::archive);
    if(!english||!english->loaded())return fail(d,E::input);
    try {
        auto s=std::make_unique<Storage>();s->archive=std::move(archive);
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIUnlock/Widgets.Dat",s->widgets,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,E::parse);
        const auto* general=s->widgets.section("General");std::int32_t count=0;
        if(!general||!assets::originalUiInteger50KG(*general,"NumberOfWidgets",0,count,d.parse,0)||count!=53)
            return fail(d,E::population);
        for(std::uint16_t id=1;id<=53;++id){
            d.widget=id;const auto* section=s->widgets.section("Widget "+std::to_string(id));
            if(!section||!section->find("Type"))return fail(d,E::population);
            const auto& type=section->find("Type")->text;auto& state=s->states[id];
            state.kind=type=="Empty"?K::empty:type=="Overlay_Text"?K::text:type=="Icon"?K::icon:
                type=="User_Rotate"?K::userRotate:type=="Soultree_Character"?K::character:K::absent;
            if(state.kind!=expected(id))return fail(d,E::population);
            if(state.kind==K::empty)continue;
            if(!assets::originalUiBoolean50KG(*section,"OnTop",false,state.onTop,d.parse,0x1CD890))return fail(d,E::parse);
            if(state.kind==K::text){
                if(!assets::readOriginalUiOverlayText50KG(s->widgets,id,english,s->texts[id],d.parse))return fail(d,E::parse);
                const auto& text=s->texts[id];
                if(text.wantsJoystickInput[0]||text.wantsJoystickInput[1])return fail(d,E::input);
                const bool dynamic=text.textSource==assets::OriginalUiTextSource50KG::dynamic;
                // Only the 20B710 message widget4 is dynamic (2 lines x 60).
                if(dynamic!=(id==4))return fail(d,E::input);
                if(dynamic){if(text.lines.size()!=2||text.dynamicLineLength!=60)return fail(d,E::input);
                    s->dynamic[id].assign(text.lines.size(),std::string{});}
                if(s->textWidgets[id].bind(text)!=T::ok)return fail(d,E::text);
            }else if(state.kind==K::icon){
                if(!assets::readOriginalUiIcon50KG(s->widgets,id,s->icons[id],d.parse))return fail(d,E::parse);
                if(s->icons[id].wantsJoystickInput[0]||s->icons[id].wantsJoystickInput[1])return fail(d,E::input);
            }else{
                // Generic UIWidget select/deselect visibility flags (2A79xx).
                auto& v=s->models[id];
                if(!assets::originalUiBoolean50KG(*section,"EnableOnSelect",false,v.enableOnSelect,d.parse,0)||
                   !assets::originalUiBoolean50KG(*section,"DisableOnSelect",false,v.disableOnSelect,d.parse,0)||
                   !assets::originalUiBoolean50KG(*section,"EnableOnDeSelect",false,v.enableOnDeselect,d.parse,0)||
                   !assets::originalUiBoolean50KG(*section,"DisableOnDeSelect",false,v.disableOnDeselect,d.parse,0))
                    return fail(d,E::parse);
            }
            if(!state.onTop)s->ordinary.push_back(id);
        }
        for(std::uint16_t id=53;id;--id)if(s->states[id].kind!=K::empty&&s->states[id].onTop)s->top.push_back(id);
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
const assets::OriginalUiDat50KG* OriginalUiUnlockWidgets50PD::widgetsDat() const noexcept {return storage_?&storage_->widgets:nullptr;}
const std::vector<std::uint16_t>* OriginalUiUnlockWidgets50PD::order(bool top) const noexcept {return storage_?(top?&storage_->top:&storage_->ordinary):nullptr;}
OriginalUiUnlockWidgetState50PD OriginalUiUnlockWidgets50PD::state(std::uint16_t id) const noexcept {
    if(!storage_||!idOk(id))return {};auto s=storage_->states[id];
    if(s.kind==K::text){s.selected=storage_->textWidgets[id].selected();s.shown=storage_->textWidgets[id].shown();}
    return s;
}
const assets::OriginalUiOverlayText50KG* OriginalUiUnlockWidgets50PD::textDescriptor(std::uint16_t id) const noexcept {
    return storage_&&idOk(id)&&storage_->states[id].kind==K::text?&storage_->texts[id]:nullptr;
}
const assets::OriginalUiIcon50KG* OriginalUiUnlockWidgets50PD::iconDescriptor(std::uint16_t id) const noexcept {
    return storage_&&idOk(id)&&storage_->states[id].kind==K::icon?&storage_->icons[id]:nullptr;
}
const OriginalUiTextWidget50KG* OriginalUiUnlockWidgets50PD::textWidget(std::uint16_t id) const noexcept {return textDescriptor(id)?&storage_->textWidgets[id]:nullptr;}
bool OriginalUiUnlockWidgets50PD::dynamicText(std::uint16_t id) const noexcept {
    const auto* t=textDescriptor(id);return t&&t->textSource==assets::OriginalUiTextSource50KG::dynamic;
}
const std::vector<std::string>* OriginalUiUnlockWidgets50PD::dynamicLines(std::uint16_t id) const noexcept {return dynamicText(id)?&storage_->dynamic[id]:nullptr;}
bool OriginalUiUnlockWidgets50PD::preflight(const OriginalUiOptionsMenuCallback50KG* callbacks,std::size_t count,D& d) const noexcept {
    d={};if(!storage_)return fail(d,E::notBound);if((count&&!callbacks)||count>256)return fail(d,E::callback);
    for(std::size_t i=0;i<count;++i){const auto& c=callbacks[i];d.widget=c.widgetId;d.callback=i;d.sourcePC=c.sourcePC;
        if(!idOk(c.widgetId)||storage_->states[c.widgetId].kind==K::empty||storage_->states[c.widgetId].kind==K::absent||!operation(c.operation))
            return fail(d,E::callback);}
    return true;
}
bool OriginalUiUnlockWidgets50PD::apply(const OriginalUiOptionsMenuCallback50KG* callbacks,std::size_t count,D& d) noexcept {
    if(!preflight(callbacks,count,d))return false;
    for(std::size_t i=0;i<count;++i){
        const auto& c=callbacks[i];auto& state=storage_->states[c.widgetId];d.widget=c.widgetId;d.callback=i;d.sourcePC=c.sourcePC;
        if(state.kind==K::text){
            auto& text=storage_->textWidgets[c.widgetId];
            if((c.operation==O::setEnabled?text.setControlEnabled(c.value):text.apply(c.operation,c.value))!=T::ok)return fail(d,E::text);
            continue;
        }
        bool onSelect=false,offSelect=false,onDeselect=false,offDeselect=false;
        if(state.kind==K::icon){const auto& icon=storage_->icons[c.widgetId];
            onSelect=icon.enableOnSelect;offSelect=icon.disableOnSelect;onDeselect=icon.enableOnDeselect;offDeselect=icon.disableOnDeselect;}
        else{const auto& v=storage_->models[c.widgetId];
            onSelect=v.enableOnSelect;offSelect=v.disableOnSelect;onDeselect=v.enableOnDeselect;offDeselect=v.disableOnDeselect;}
        const auto visibility=[&](bool show,bool hide){if(show!=hide)state.shown=show;};
        switch(c.operation){
        case O::show:case O::reset:state.shown=true;break;
        case O::hide:state.shown=false;break;
        case O::select:state.selected=true;visibility(onSelect,offSelect);break;
        case O::deselect:state.selected=false;visibility(onDeselect,offDeselect);break;
        case O::clearSelection:state.selected=false;break;
        case O::setEnabled:state.selected=c.value;state.shown=c.value;break;
        }
    }
    d={};return true;
}
bool OriginalUiUnlockWidgets50PD::setWidgetColor(std::uint16_t id,std::uint32_t color,D& d) noexcept {
    d={};d.widget=id;if(!storage_||!idOk(id))return fail(d,E::notBound);
    if(storage_->states[id].kind==K::text){if(storage_->textWidgets[id].setWidgetColor(color)!=T::ok)return fail(d,E::text);}
    else if(storage_->states[id].kind==K::icon){auto& image=storage_->images[id];if(!image.owner)return fail(d,E::missingResource);
        image.layout.quad.sourcePacketColor=originalUiTextPacketColor50KG(color,static_cast<std::int16_t>(storage_->icons[id].alpha));}
    else return fail(d,E::input);
    return true;
}
bool OriginalUiUnlockWidgets50PD::preflightDynamicText(std::uint16_t id,const std::string& value,
    std::vector<std::string>& pieces,D& d) const noexcept {
    d={};d.widget=id;d.sourcePC=0x2A9DE0;if(!dynamicText(id))return fail(d,E::text);
    const auto& text=storage_->texts[id];
    try {
        std::vector<std::string> out;std::size_t start=0;
        for(;;){const auto at=value.find('@',start);out.push_back(value.substr(start,at==std::string::npos?std::string::npos:at-start));
            if(at==std::string::npos)break;start=at+1;}
        if(out.size()>text.lines.size())return fail(d,E::capacity);                 // assert 2A9EE0 (line 3D5)
        for(const auto& piece:out)if(piece.size()>std::size_t(text.dynamicLineLength))return fail(d,E::capacity);
        pieces=std::move(out);return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiUnlockWidgets50PD::setDynamicText(std::uint16_t id,const std::string& value,D& d) noexcept {
    std::vector<std::string> pieces;if(!preflightDynamicText(id,value,pieces,d))return false;
    auto& widget=storage_->textWidgets[id];auto& lines=storage_->dynamic[id];
    try {
        for(std::size_t i=0;i<lines.size();++i){
            const auto* line=widget.line(i);if(!line)return fail(d,E::text);
            if(line->locked)continue;                                                  // shared +CC gate
            if(i<pieces.size()){lines[i]=pieces[i];                                    // 29D940
                if(widget.setupLine(i,OriginalUiTextLineSetup50KG{})!=T::ok)return fail(d,E::text);} // 29DA40(255,-1)
            else lines[i].clear();                                                     // 2A9F98 empty string
        }
    }catch(...){return fail(d,E::allocation);}
    return true;
}
bool OriginalUiUnlockWidgets50PD::updateText(std::uint16_t id,bool shown,D& d) noexcept {
    d={};d.widget=id;if(!textDescriptor(id))return fail(d,E::text);
    const auto s=storage_->textWidgets[id].updateStatic(shown);
    return s==T::ok?true:fail(d,s==T::hidden?E::hidden:E::text);
}
bool OriginalUiUnlockWidgets50PD::projectText(std::uint16_t id,std::size_t line,float fontHeight,std::int32_t width,
    std::int32_t height,bool shown,OriginalUiUnlockTextProjection50PD& out,D& d) noexcept {
    d={};d.widget=id;const auto* text=textDescriptor(id);if(!text)return fail(d,E::text);
    OriginalUiUnlockTextProjection50PD p;p.descriptor=text;
    if(!positionOriginalUiText50KG({text,line,fontHeight,width,height},p.position))return fail(d,E::text);
    const auto status=storage_->textWidgets[id].projectLine(line,p.line,shown);
    if(status!=T::ok)return fail(d,status==T::hidden?E::hidden:E::text);
    if(dynamicText(id)){if(line>=storage_->dynamic[id].size())return fail(d,E::text);p.text=&storage_->dynamic[id][line];}
    else{if(line>=text->lines.size())return fail(d,E::text);p.text=&text->lines[line];}
    out=p;return true;
}
bool OriginalUiUnlockWidgets50PD::prepareIcon(std::uint16_t id,std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,
    std::uint32_t index,std::int32_t depth,D& d) noexcept {
    d={};d.widget=id;const auto* icon=iconDescriptor(id);if(!icon)return fail(d,E::icon);
    if(storage_->images[id].owner||!images||!images->loaded()||!images->textures())return fail(d,E::missingResource);
    const auto* image=images->textures()->texture(index);if(!image||!images->decodedTexture(index))return fail(d,E::missingResource);
    OriginalUiIconLayout50KG layout;
    if(layoutOriginalUiIcon50KG(*icon,image->texture,depth,layout)!=OriginalUiIconLayoutStatus50KG::complete)return fail(d,E::icon);
    auto& b=storage_->images[id];b.owner=std::move(images);b.index=index;b.layout=layout;return true;
}
bool OriginalUiUnlockWidgets50PD::projectIcon(std::uint16_t id,bool shown,OriginalUiUnlockIconProjection50PD& out,D& d) const noexcept {
    d={};d.widget=id;const auto* icon=iconDescriptor(id);if(!icon)return fail(d,E::icon);
    if(!shown||!storage_->states[id].shown)return fail(d,E::hidden);
    const auto& image=storage_->images[id];if(!image.owner)return fail(d,E::missingResource);
    out={icon,image.owner.get(),image.index,image.layout};return true;
}
}

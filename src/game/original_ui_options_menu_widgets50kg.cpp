#include "original_ui_options_menu_widgets50kg.h"
#include <string>
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalUiOptionsWidgetStatus50KG;
using Kind=OriginalUiOptionsWidgetKind50KG;
using Control=OriginalUiOptionsMenuStatus50KG;
using Op=OriginalUiControlOperation50KG;
using Text=OriginalUiTextWidgetStatus50KG;
using Diagnostic=OriginalUiOptionsWidgetDiagnostic50KG;
Status finish(Diagnostic& d,Status s) noexcept {d.status=s;return s;}
bool admitted(std::uint16_t id) noexcept {
    for(auto known:originalUiOptionsMenuWidgetIds50KG)if(id==known)return true;
    return false;
}
bool valid(Op op) noexcept {
    switch(op) {case Op::show:case Op::hide:case Op::select:case Op::deselect:
    case Op::clearSelection:case Op::setEnabled:case Op::reset:return true;}
    return false;
}
Status textResult(Diagnostic& d,Text s) noexcept {
    d.text=s;return finish(d,s==Text::ok?Status::ok:s==Text::hidden?Status::hidden:Status::textFailure);
}
}
struct OriginalUiOptionsMenuWidgets50KG::Storage {
    OriginalUiOptionsMenuController50KG controller;
    OriginalUiNavigation50KG navigation;
    std::array<OriginalUiOptionsWidgetState50KG,107> state{};
    std::array<assets::OriginalUiOverlayText50KG,107> texts;
    std::array<assets::OriginalUiIcon50KG,107> icons;
    std::array<OriginalUiTextWidget50KG,107> textWidgets;
    struct Image {std::shared_ptr<const assets::OriginalUiScreenAssets50KG> owner;
        std::uint32_t index=0;OriginalUiIconLayout50KG layout{};};
    std::array<Image,107> images;
    OriginalUiOptionsWidgetOrder50KG order;
    std::uint32_t flags42C=0;
};
OriginalUiOptionsMenuWidgets50KG::OriginalUiOptionsMenuWidgets50KG() noexcept=default;
OriginalUiOptionsMenuWidgets50KG::~OriginalUiOptionsMenuWidgets50KG()=default;
Status OriginalUiOptionsMenuWidgets50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const assets::OriginalUiEnglish50KG* english,Diagnostic& d) noexcept {
    d={};if(storage_)return finish(d,Status::alreadyBound);
    try {
        auto s=std::make_unique<Storage>();
        d.controller=s->controller.bind(std::move(archive),d.parse);
        if(d.controller!=Control::ok || !s->controller.initialNavigation(s->navigation))
            return finish(d,Status::controllerFailure);
        const auto& dat=*s->controller.widgetsDat();
        for(auto id:originalUiOptionsMenuWidgetIds50KG) {
            d.widgetId=id;auto& state=s->state[id];
            state.kind=id==5?Kind::movement:(id==98||id==99||id==105)?Kind::icon:Kind::text;
            const auto* section=dat.section("Widget "+std::to_string(id));
            if(!section)return finish(d,Status::parseFailure);
            if(!assets::originalUiBoolean50KG(*section,"OnTop",false,state.onTop,d.parse,0x1CD890))
                return finish(d,Status::parseFailure);
            if(state.kind==Kind::text) {
                if(!assets::readOriginalUiOverlayText50KG(dat,id,english,s->texts[id],d.parse))
                    return finish(d,Status::parseFailure);
                const auto& text=s->texts[id];
                if(text.textSource==assets::OriginalUiTextSource50KG::dynamic ||
                    text.wantsJoystickInput[0] || text.wantsJoystickInput[1])return finish(d,Status::invalidInput);
                d.text=s->textWidgets[id].bind(text);
                if(d.text!=Text::ok)return finish(d,Status::textFailure);
            } else if(state.kind==Kind::icon) {
                if(!assets::readOriginalUiIcon50KG(dat,id,s->icons[id],d.parse))return finish(d,Status::parseFailure);
                if(s->icons[id].wantsJoystickInput[0] || s->icons[id].wantsJoystickInput[1])return finish(d,Status::invalidInput);
            }
            // Widget5 descriptor and exact input mappings were admitted by controller.bind.
            if(!state.onTop)s->order.ordinaryWidgets.push_back(id);
        }
        for(auto it=originalUiOptionsMenuWidgetIds50KG.rbegin();it!=originalUiOptionsMenuWidgetIds50KG.rend();++it)
            if(s->state[*it].onTop)s->order.onTopWidgets.push_back(*it);
        std::size_t movementRegistrations=0;
        for(const auto& control:s->controller.page()->controls)for(auto id:control.widgets) {
            if(!admitted(id))return finish(d,Status::invalidWidget);
            if(id==5)++movementRegistrations;
        }
        if(movementRegistrations!=1)return finish(d,Status::invalidInput);
        storage_=std::move(s);d={};return Status::ok;
    } catch(...) {return finish(d,Status::allocation);}
}
bool OriginalUiOptionsMenuWidgets50KG::bound() const noexcept {return bool(storage_);}
const assets::OriginalUiPage50KG* OriginalUiOptionsMenuWidgets50KG::page() const noexcept {
    return storage_?storage_->controller.page():nullptr;
}
const OriginalUiNavigation50KG* OriginalUiOptionsMenuWidgets50KG::navigation() const noexcept {
    return storage_?&storage_->navigation:nullptr;
}
const OriginalUiOptionsWidgetOrder50KG* OriginalUiOptionsMenuWidgets50KG::order() const noexcept {
    return storage_?&storage_->order:nullptr;
}
OriginalUiOptionsWidgetState50KG OriginalUiOptionsMenuWidgets50KG::widgetState(std::uint16_t id) const noexcept {
    if(!storage_||!admitted(id))return {};
    auto state=storage_->state[id];
    if(state.kind==Kind::text) {state.selected=storage_->textWidgets[id].selected();state.shown=storage_->textWidgets[id].shown();}
    return state;
}
const assets::OriginalUiOverlayText50KG* OriginalUiOptionsMenuWidgets50KG::textDescriptor(std::uint16_t id) const noexcept {
    return storage_&&admitted(id)&&storage_->state[id].kind==Kind::text?&storage_->texts[id]:nullptr;
}
const assets::OriginalUiIcon50KG* OriginalUiOptionsMenuWidgets50KG::iconDescriptor(std::uint16_t id) const noexcept {
    return storage_&&admitted(id)&&storage_->state[id].kind==Kind::icon?&storage_->icons[id]:nullptr;
}
const OriginalUiTextWidget50KG* OriginalUiOptionsMenuWidgets50KG::textWidget(std::uint16_t id) const noexcept {
    return textDescriptor(id)?&storage_->textWidgets[id]:nullptr;
}
std::uint32_t OriginalUiOptionsMenuWidgets50KG::pageInputFlags42C() const noexcept {return storage_?storage_->flags42C:0;}
Status OriginalUiOptionsMenuWidgets50KG::commitPlan(const OriginalUiOptionsMenuPlan50KG& p,Diagnostic& d) noexcept {
    if(p.navigation.page!=page()||p.callbackCount>p.callbacks.size())return finish(d,Status::invalidCommand);
    for(std::size_t i=0;i<p.callbackCount;++i) {
        const auto& c=p.callbacks[i];d.widgetId=c.widgetId;d.callbackIndex=i;d.sourcePC=c.sourcePC;
        if(!admitted(c.widgetId)||!valid(c.operation))return finish(d,Status::invalidCommand);
    }
    for(std::size_t i=0;i<p.callbackCount;++i) {
        const auto& c=p.callbacks[i];auto& state=storage_->state[c.widgetId];
        if(state.kind==Kind::text) {
            auto& text=storage_->textWidgets[c.widgetId];
            if(c.operation==Op::setEnabled)(void)text.setControlEnabled(c.value);
            else (void)text.apply(c.operation,c.value);
        } else {
            bool showOnSelect=true,hideOnSelect=false,showOnDeselect=false,hideOnDeselect=true;
            if(state.kind==Kind::icon) {
                const auto& icon=storage_->icons[c.widgetId];
                showOnSelect=icon.enableOnSelect;hideOnSelect=icon.disableOnSelect;
                showOnDeselect=icon.enableOnDeselect;hideOnDeselect=icon.disableOnDeselect;
            }
            const auto visibility=[&](bool show,bool hide){if(show!=hide)state.shown=show;};
            switch(c.operation) {
            case Op::show:case Op::reset:state.shown=true;break;
            case Op::hide:state.shown=false;break;
            case Op::select:state.selected=true;visibility(showOnSelect,hideOnSelect);break;
            case Op::deselect:state.selected=false;visibility(showOnDeselect,hideOnDeselect);break;
            case Op::clearSelection:state.selected=false;break;
            case Op::setEnabled:state.selected=c.value;state.shown=c.value;break;
            }
        }
    }
    storage_->navigation=p.navigation;
    if(p.writesPageInputFlags42C)storage_->flags42C=p.pageInputFlags42C;
    return finish(d,Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::phase(OriginalUiOptionsMenuPhase50KG phase,bool known,
    std::uint16_t slot,Diagnostic& d) noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    OriginalUiOptionsMenuPlan50KG p;d.controller=storage_->controller.phase(storage_->navigation,phase,known,slot,p);
    if(d.controller!=Control::ok)return finish(d,Status::controllerFailure);
    return commitPlan(p,d);
}
Status OriginalUiOptionsMenuWidgets50KG::input(std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    bool reached,bool enabled,const OriginalUiOptionsContext50KG& context,OriginalUiOptionsMenuPageResult50KG& output,
    OriginalUiMovementNotification50KG& notification,Diagnostic& d) noexcept {
    return inputWithPageEffects(controller,input,reached,enabled,context,nullptr,nullptr,output,notification,d);
}
Status OriginalUiOptionsMenuWidgets50KG::inputWithPageEffects(std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    bool reached,bool enabled,const OriginalUiOptionsContext50KG& context,void* effectsContext,
    bool (*pageEffects)(void*,const OriginalUiOptionsMenuPageResult50KG&) noexcept,
    OriginalUiOptionsMenuPageResult50KG& output,OriginalUiMovementNotification50KG& notification,Diagnostic& d) noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    OriginalUiOptionsMenuInput50KG delivery{reached,enabled,false};OriginalUiOptionsMenuPageResult50KG pageResult;
    const auto status=storage_->controller.pageInput(storage_->navigation,controller,input,delivery,context,pageResult);
    d.controller=status;
    if(status!=Control::ok&&status!=Control::noChange&&status!=Control::unavailable)return finish(d,Status::controllerFailure);
    if(pageEffects&&!pageEffects(effectsContext,pageResult))return finish(d,Status::controllerFailure);
    delivery.movementSelectedAtSnapshot=storage_->state[5].selected;
    OriginalUiOptionsMenuPlan50KG p;OriginalUiMovementNotification50KG notice;
    const auto movement=storage_->controller.movementInput(storage_->navigation,controller,input,delivery,pageResult.pageAccepted,p,notice);
    if(movement!=Control::ok&&movement!=Control::noChange) {d.controller=movement;return finish(d,Status::controllerFailure);}
    const auto committed=commitPlan(p,d);if(committed!=Status::ok)return committed;
    output=pageResult;notification=notice;
    return finish(d,status==Control::unavailable?Status::unavailable:
        (status==Control::noChange&&movement==Control::noChange)?Status::noChange:Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::updateText(std::uint16_t id,bool shown,Diagnostic& d) noexcept {
    d={};d.widgetId=id;if(!storage_)return finish(d,Status::notBound);
    if(!textDescriptor(id))return finish(d,Status::invalidWidget);
    return textResult(d,storage_->textWidgets[id].updateStatic(shown));
}
Status OriginalUiOptionsMenuWidgets50KG::projectText(std::uint16_t id,std::size_t line,float fontHeight,
    std::int32_t width,std::int32_t height,bool shown,OriginalUiOptionsTextProjection50KG& output,Diagnostic& d) noexcept {
    d={};d.widgetId=id;if(!storage_)return finish(d,Status::notBound);
    const auto* text=textDescriptor(id);if(!text)return finish(d,Status::invalidWidget);
    OriginalUiOptionsTextProjection50KG projected;
    if(!positionOriginalUiText50KG({text,line,fontHeight,width,height},projected.position))return finish(d,Status::invalidInput);
    const auto result=storage_->textWidgets[id].projectLine(line,projected.line,shown);
    if(result!=Text::ok)return textResult(d,result);
    projected.descriptor=text;projected.widgetId=id;projected.lineIndex=line;output=projected;return textResult(d,Text::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::prepareIcon(std::uint16_t id,
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,std::uint32_t index,
    std::int32_t depth,Diagnostic& d) noexcept {
    d={};d.widgetId=id;if(!storage_)return finish(d,Status::notBound);
    const auto* icon=iconDescriptor(id);if(!icon)return finish(d,Status::invalidWidget);
    if(storage_->images[id].owner)return finish(d,Status::alreadyPrepared);
    if(!images||!images->loaded()||!images->textures())return finish(d,Status::missingResource);
    const auto* atlas=images->textures()->texture(index);
    if(!atlas||!images->decodedTexture(index))return finish(d,Status::missingResource);
    OriginalUiIconLayout50KG layout;d.icon=layoutOriginalUiIcon50KG(*icon,atlas->texture,depth,layout);
    if(d.icon!=OriginalUiIconLayoutStatus50KG::complete)return finish(d,Status::layoutFailure);
    auto& bound=storage_->images[id];bound.owner=std::move(images);bound.index=index;bound.layout=layout;return Status::ok;
}
Status OriginalUiOptionsMenuWidgets50KG::projectIcon(std::uint16_t id,bool shown,
    OriginalUiOptionsIconProjection50KG& output,Diagnostic& d) const noexcept {
    d={};d.widgetId=id;if(!storage_)return finish(d,Status::notBound);
    const auto* icon=iconDescriptor(id);if(!icon)return finish(d,Status::invalidWidget);
    if(!shown||!storage_->state[id].shown)return finish(d,Status::hidden);
    const auto& image=storage_->images[id];if(!image.owner)return finish(d,Status::missingResource);
    output={icon,image.owner.get(),image.index,image.layout};return Status::ok;
}
}

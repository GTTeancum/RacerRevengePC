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
    for(auto known:originalUiOptionsManagerWidgetIds50KG)if(id==known)return true;
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
    OriginalUiGameOptionsController50KG gameController;
    OriginalUiGameOptionsState50KG gameState;
    OriginalUiControlsController50KG controlsController;
    OriginalUiCreditsController50KG creditsController;
    OriginalUiCreditsState50KG creditsState;
    assets::OriginalUiDat50KG memoryCardDat;
    assets::OriginalUiPage50KG memoryCardPage;
    OriginalUiNavigation50KG memoryCardNavigation;
    OriginalUiMemoryCard50KG memoryCardController;
    OriginalUiMemoryCardState50KG memoryCardState=OriginalUiMemoryCard50KG::initialState();
    struct Localized {std::uint16_t stringId=0;std::size_t writtenLineCount=0;std::vector<std::string> lines;};
    std::array<std::vector<Localized>,originalUiOptionsWidgetStorageCount50KG> localized;
    std::array<std::string,originalUiOptionsWidgetStorageCount50KG> dynamicSecond;
    std::array<std::array<std::uint16_t,2>,originalUiOptionsWidgetStorageCount50KG> renderedString;
    std::array<std::string,originalUiOptionsWidgetStorageCount50KG> dynamic;
    std::array<std::int32_t,originalUiOptionsWidgetStorageCount50KG> renderedVolume;
    std::array<OriginalUiOptionsWidgetState50KG,originalUiOptionsWidgetStorageCount50KG> state{};
    std::array<assets::OriginalUiOverlayText50KG,originalUiOptionsWidgetStorageCount50KG> texts;
    std::array<assets::OriginalUiIcon50KG,originalUiOptionsWidgetStorageCount50KG> icons;
    std::array<OriginalUiTextWidget50KG,originalUiOptionsWidgetStorageCount50KG> textWidgets;
    struct Image {std::shared_ptr<const assets::OriginalUiScreenAssets50KG> owner;
        std::uint32_t index=0;OriginalUiIconLayout50KG layout{};};
    std::array<Image,originalUiOptionsWidgetStorageCount50KG> images;
    OriginalUiOptionsWidgetOrder50KG order;
    std::uint32_t flags42C=0;
};
bool OriginalUiOptionsMenuWidgets50KG::nativeManagerSourceCurrent(
    const std::shared_ptr<const assets::OriginalUiArchive50KG>& expected,
    const assets::OriginalUiDat50KG*& out)const noexcept {
    out=nullptr;return storage_&&storage_->controller.nativeSourceCurrent(expected,out);
}
OriginalUiOptionsMenuWidgets50KG::OriginalUiOptionsMenuWidgets50KG() noexcept=default;
OriginalUiOptionsMenuWidgets50KG::~OriginalUiOptionsMenuWidgets50KG()=default;
Status OriginalUiOptionsMenuWidgets50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const assets::OriginalUiEnglish50KG* english,Diagnostic& d) noexcept {
    d={};if(storage_)return finish(d,Status::alreadyBound);
    try {
        auto s=std::make_unique<Storage>();s->renderedVolume.fill(-1);
        d.controller=s->controller.bind(archive,d.parse);
        if(d.controller!=Control::ok || !s->controller.initialNavigation(s->navigation))
            return finish(d,Status::controllerFailure);
        d.controller=s->gameController.bind(archive,d.parse);
        if(d.controller!=Control::ok||!s->gameController.initialState(s->gameState))return finish(d,Status::controllerFailure);
        d.controller=s->controlsController.bind(archive,0,d.parse);
        if(d.controller!=Control::ok)return finish(d,Status::controllerFailure);
        for(auto& row:s->renderedString)row.fill(0xffffu);
        const auto& dat=*s->controller.widgetsDat();
        d.controller=s->creditsController.bind(archive,d.parse);
        if(d.controller!=Control::ok||!s->creditsController.initialState(s->creditsState))
            return finish(d,Status::controllerFailure);
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UIOption/memcard.dat",s->memoryCardDat,d.parse)||
            !assets::readOriginalUiPage50KG(s->memoryCardDat,dat,s->memoryCardPage,d.parse))
            return finish(d,Status::parseFailure);
        const auto& mp=s->memoryCardPage;
        if(mp.identity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,46)||
            mp.pageNo!=5||mp.widgetCount!=357||mp.controls.size()!=1||mp.groups.size()!=1||
            mp.controls[0].widgets!=std::vector<std::uint16_t>({2,16})||
            mp.groups[0].controls!=std::vector<std::int16_t>({0})||!mp.groups[0].keepEnabled||
            !mp.groups[0].associations.empty()||!mp.hasPopup||
            mp.popupWidgets!=std::array<std::int32_t,9>{{23,43,41,42,30,31,40,44,45}}||
            mp.openSize!=69||mp.openTime!=.5f||mp.start!="Open")return finish(d,Status::parseFailure);
        if(!bindOriginalUiNavigation50KG(mp,s->memoryCardNavigation))return finish(d,Status::controllerFailure);
        for(auto id:originalUiOptionsManagerWidgetIds50KG) {
            d.widgetId=id;auto& state=s->state[id];
            const auto* section=dat.section("Widget "+std::to_string(id));
            if(!section)return finish(d,Status::parseFailure);
            const auto* type=section->find("Type");if(!type)return finish(d,Status::parseFailure);
            state.kind=type->text=="Overlay_Text"?Kind::text:type->text=="Icon"?Kind::icon:
                id==5&&type->text=="Movement_Control"?Kind::movement:
                id==36&&type->text=="Soultree_Character"?Kind::model:Kind::absent;
            if(state.kind==Kind::absent)return finish(d,Status::invalidWidget);
            if(!assets::originalUiBoolean50KG(*section,"OnTop",false,state.onTop,d.parse,0x1CD890))
                return finish(d,Status::parseFailure);
            if(state.kind==Kind::text) {
                if(!assets::readOriginalUiOverlayText50KG(dat,id,english,s->texts[id],d.parse))
                    return finish(d,Status::parseFailure);
                const auto& text=s->texts[id];
                if(text.wantsJoystickInput[0]||text.wantsJoystickInput[1])return finish(d,Status::invalidInput);
                if(text.textSource==assets::OriginalUiTextSource50KG::dynamic){
                    const bool volume=id==38||id==112||id==114||id==165;
                    const bool controls=id==47||id==130||id==77||(id>=80&&id<=84)||(id>=93&&id<=96)||id==145||id==150||id==155;
                    if((!volume&&!controls)||text.lines.empty()||text.lines.size()>2||text.dynamicLineLength<0)return finish(d,Status::invalidInput);
                    s->dynamic[id].reserve(static_cast<std::size_t>(text.dynamicLineLength));
                    s->dynamicSecond[id].reserve(static_cast<std::size_t>(text.dynamicLineLength));
                    if(volume&&(text.lines.size()!=1||text.dynamicLineLength!=4))return finish(d,Status::invalidInput);
                    if(controls){
                        if(!english||!english->loaded())return finish(d,Status::missingResource);
                        const unsigned first=id==47?70:id==130?62:74,last=id==47?71:id==130?69:85;
                        for(unsigned stringId=first;stringId<=last;++stringId){
                            //188C10 action labels never consume75/76 here.
                            if(first==74&&(stringId==75||stringId==76))continue;
                            const auto* entry=english->at(stringId);if(!entry)return finish(d,Status::missingResource);
                            Storage::Localized value;value.stringId=static_cast<std::uint16_t>(stringId);
                            std::size_t at=0;
                            do{const auto found=entry->text.find('@',at),end=found==std::string::npos?entry->text.size():found;
                                if(value.lines.size()>=text.lines.size()||end-at>static_cast<std::size_t>(text.dynamicLineLength))return finish(d,Status::invalidInput);
                                value.lines.push_back(entry->text.substr(at,end-at));
                                if(found==std::string::npos)break;at=end+1;
                            }while(at<=entry->text.size());
                            value.writtenLineCount=value.lines.size();value.lines.resize(text.lines.size());s->localized[id].push_back(std::move(value));
                        }
                    }
                }
                d.text=s->textWidgets[id].bind(text);
                if(d.text!=Text::ok)return finish(d,Status::textFailure);
            } else if(state.kind==Kind::icon) {
                if(!assets::readOriginalUiIcon50KG(dat,id,s->icons[id],d.parse))return finish(d,Status::parseFailure);
                if(s->icons[id].wantsJoystickInput[0] || s->icons[id].wantsJoystickInput[1])return finish(d,Status::invalidInput);
            }
            // Widget5 descriptor and exact input mappings were admitted by controller.bind.
            if(!state.onTop)s->order.ordinaryWidgets.push_back(id);
        }
        for(auto it=originalUiOptionsManagerWidgetIds50KG.rbegin();it!=originalUiOptionsManagerWidgetIds50KG.rend();++it)
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
const assets::OriginalUiPage50KG* OriginalUiOptionsMenuWidgets50KG::memoryCardPage() const noexcept {
    return storage_?&storage_->memoryCardPage:nullptr;
}
const OriginalUiNavigation50KG* OriginalUiOptionsMenuWidgets50KG::memoryCardNavigation() const noexcept {
    return storage_?&storage_->memoryCardNavigation:nullptr;
}
const OriginalUiMemoryCard50KG* OriginalUiOptionsMenuWidgets50KG::memoryCardController() const noexcept {
    return storage_?&storage_->memoryCardController:nullptr;
}
const OriginalUiMemoryCardState50KG* OriginalUiOptionsMenuWidgets50KG::memoryCardState() const noexcept {
    return storage_?&storage_->memoryCardState:nullptr;
}
Status OriginalUiOptionsMenuWidgets50KG::preflightMemoryCardPlan(const OriginalUiMemoryCardPlan50KG& p,Diagnostic& d) const noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    if(!OriginalUiMemoryCard50KG::valid(p.state)||p.callbackCount>p.callbacks.size()||
        (p.writesPageInputFlags42C&&(p.pageInputFlags42C!=p.state.inputFlags42C||
        (p.pageInputFlags42C!=0&&p.pageInputFlags42C!=2))))return finish(d,Status::invalidCommand);
    if((p.enableOwnedDialog&&(!p.state.dialogActive||p.state.dialogState30!=4))||
        (p.resetOwnedDialogText&&p.state.textIds!=std::array<std::int32_t,9>{{-1,-1,-1,-1,-1,-1,-1,-1,-1}}))
        return finish(d,Status::invalidCommand);
    for(std::size_t i=0;i<p.callbackCount;++i){const auto& c=p.callbacks[i];
        d.callbackIndex=i;d.widgetId=c.widgetId;d.sourcePC=c.sourcePC;
        bool owns=false;for(auto id:originalUiMemoryCardWidgetIds50KG)if(c.widgetId==id)owns=true;
        if(!owns||!valid(c.operation)||storage_->state[c.widgetId].kind==Kind::absent)
            return finish(d,Status::invalidCommand);
    }
    return finish(d,Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::commitMemoryCardPlan(const OriginalUiMemoryCardPlan50KG& p,Diagnostic& d) noexcept {
    if(preflightMemoryCardPlan(p,d)!=Status::ok)return d.status;
    if(commitCallbacks(p.callbacks.data(),p.callbackCount,d)!=Status::ok)return d.status;
    if(p.writesPageInputFlags42C)storage_->flags42C=p.pageInputFlags42C;
    storage_->memoryCardState=p.state;return finish(d,Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::commitMemoryCardRender(const OriginalUiMemoryCardRender50KG& r,Diagnostic& d) noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    if(!OriginalUiMemoryCard50KG::valid(r.state)||r.sourcePC!=0x1F6DE0||r.dialogSourcePC!=0x1FFFD0)
        return finish(d,Status::invalidCommand);
    storage_->memoryCardState=r.state;return finish(d,Status::ok);
}
const assets::OriginalUiPage50KG* OriginalUiOptionsMenuWidgets50KG::creditsPage() const noexcept {
    return storage_?storage_->creditsController.page():nullptr;
}
const OriginalUiCreditsController50KG* OriginalUiOptionsMenuWidgets50KG::creditsController() const noexcept {
    return storage_?&storage_->creditsController:nullptr;
}
const OriginalUiCreditsState50KG* OriginalUiOptionsMenuWidgets50KG::creditsState() const noexcept {
    return storage_?&storage_->creditsState:nullptr;
}
Status OriginalUiOptionsMenuWidgets50KG::preflightCreditsPlan(const OriginalUiCreditsPlan50KG& plan,Diagnostic& d) const noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    if(plan.state.navigation.page!=creditsPage()||plan.callbackCount>plan.callbacks.size()||
        plan.alphaCount>plan.alphaWrites.size()||plan.alphaAfterCallbackCount>plan.callbackCount)return finish(d,Status::invalidCommand);
    for(std::size_t i=0;i<plan.alphaCount;++i){const auto& a=plan.alphaWrites[i];
        d.callbackIndex=i;d.widgetId=a.widgetId;d.sourcePC=a.sourcePC;
        if(a.widgetId<170||a.widgetId>357||!textDescriptor(a.widgetId))return finish(d,Status::invalidWidget);
    }
    for(std::size_t i=0;i<plan.callbackCount;++i){const auto& c=plan.callbacks[i];
        d.callbackIndex=i;d.widgetId=c.widgetId;d.sourcePC=c.sourcePC;
        bool owns=false;for(auto id:originalUiCreditsWidgetIds50KG)if(id==c.widgetId)owns=true;
        if(!owns||!valid(c.operation))return finish(d,Status::invalidCommand);
    }
    return finish(d,Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::commitCreditsPlan(const OriginalUiCreditsPlan50KG& plan,Diagnostic& d) noexcept {
    if(preflightCreditsPlan(plan,d)!=Status::ok)return d.status;
    for(std::size_t i=0;i<plan.alphaAfterCallbackCount;++i)
        if(commitCallbacks(&plan.callbacks[i],1,d)!=Status::ok)return d.status;
    for(std::size_t i=0;i<plan.alphaCount;++i){const auto& a=plan.alphaWrites[i];
        d.callbackIndex=i;d.widgetId=a.widgetId;d.sourcePC=a.sourcePC;
        d.text=storage_->textWidgets[a.widgetId].setWidgetAlpha(a.alpha);
        if(d.text!=Text::ok)return finish(d,Status::textFailure);
    }
    // Exact Credits581-entry stream does not inflate the shared256-entry
    // default callback admission. Every entry was preflighted above.
    for(std::size_t i=plan.alphaAfterCallbackCount;i<plan.callbackCount;++i)
        if(commitCallbacks(&plan.callbacks[i],1,d)!=Status::ok)return d.status;
    storage_->creditsState=plan.state;
    if(plan.writesPageInputFlags42C)storage_->flags42C=plan.pageInputFlags42C;
    return finish(d,Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::setCreditsCardAlpha(std::uint16_t control,std::uint8_t alpha,Diagnostic& d) noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    const auto* page=creditsPage();
    if(!control||control>=page->controls.size())return finish(d,Status::invalidInput);
    for(auto id:page->controls[control].widgets)
        if(id<170||id>357||!textDescriptor(id))return finish(d,Status::invalidWidget);
    for(auto id:page->controls[control].widgets){d.widgetId=id;
        d.text=storage_->textWidgets[id].setWidgetAlpha(alpha);
        if(d.text!=Text::ok)return finish(d,Status::textFailure);
    }
    return finish(d,Status::ok);
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
    const auto status=commitCallbacks(p.callbacks.data(),p.callbackCount,d);if(status!=Status::ok)return status;
    storage_->navigation=p.navigation;
    if(p.writesPageInputFlags42C)storage_->flags42C=p.pageInputFlags42C;
    return finish(d,Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::commitCallbacks(const OriginalUiOptionsMenuCallback50KG* callbacks,std::size_t count,Diagnostic& d) noexcept {
    if((!callbacks&&count)||count>256)return finish(d,Status::invalidCommand);
    for(std::size_t i=0;i<count;++i) {
        const auto& c=callbacks[i];d.widgetId=c.widgetId;d.callbackIndex=i;d.sourcePC=c.sourcePC;
        if(!admitted(c.widgetId)||!valid(c.operation))return finish(d,Status::invalidCommand);
    }
    for(std::size_t i=0;i<count;++i) {
        const auto& c=callbacks[i];auto& state=storage_->state[c.widgetId];
        if(state.kind==Kind::text) {
            auto& text=storage_->textWidgets[c.widgetId];
            if(c.operation==Op::setEnabled)(void)text.setControlEnabled(c.value);
            else (void)text.apply(c.operation,c.value);
        } else {
            bool showOnSelect=true,hideOnSelect=false,showOnDeselect=false,hideOnDeselect=true;
            if(state.kind==Kind::model){
                // Common Soultree Widget36 authors none of the four flags.
                //2A8770 defaults every selection-visibility flag to false.
                showOnSelect=hideOnSelect=showOnDeselect=hideOnDeselect=false;
            }else if(state.kind==Kind::icon) {
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
            case Op::setEnabled:if(state.kind!=Kind::model)state.selected=c.value;state.shown=c.value;break;
            }
        }
    }
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
const assets::OriginalUiPage50KG* OriginalUiOptionsMenuWidgets50KG::gameOptionsPage() const noexcept {return storage_?storage_->gameController.page():nullptr;}
const OriginalUiGameOptionsState50KG* OriginalUiOptionsMenuWidgets50KG::gameOptionsState() const noexcept {return storage_?&storage_->gameState:nullptr;}
Status OriginalUiOptionsMenuWidgets50KG::commitGameOptionsPlan(const OriginalUiGameOptionsPlan50KG& p,Diagnostic& d) noexcept {
    if(p.state.navigation.page!=gameOptionsPage()||p.callbackCount>p.callbacks.size())return finish(d,Status::invalidCommand);
    const auto status=commitCallbacks(p.callbacks.data(),p.callbackCount,d);if(status!=Status::ok)return status;
    storage_->gameState=p.state;if(p.writesPageInputFlags42C)storage_->flags42C=p.pageInputFlags42C;return Status::ok;
}
Status OriginalUiOptionsMenuWidgets50KG::gameOptionsPhase(OriginalUiOptionsMenuPhase50KG phase,
    const OriginalUiGameSettingsProfile50KG& profile,Diagnostic& d) noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);OriginalUiGameOptionsPlan50KG p;
    d.controller=storage_->gameController.phase(storage_->gameState,phase,profile,p);
    if(d.controller!=Control::ok)return finish(d,Status::controllerFailure);return commitGameOptionsPlan(p,d);
}
Status OriginalUiOptionsMenuWidgets50KG::gameOptionsInput(std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    bool reached,bool enabled,OriginalUiGameOptionsPlan50KG& out,Diagnostic& d) noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);OriginalUiGameOptionsPlan50KG p;
    d.controller=storage_->gameController.input(storage_->gameState,controller,input,reached,enabled,p);
    if(d.controller!=Control::ok&&d.controller!=Control::noChange&&d.controller!=Control::unavailable)return finish(d,Status::controllerFailure);
    const auto status=commitGameOptionsPlan(p,d);if(status!=Status::ok)return status;out=p;
    return finish(d,d.controller==Control::unavailable?Status::unavailable:d.controller==Control::noChange?Status::noChange:Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::refreshGameOptionsVolumeText(Diagnostic& d) noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    if(!storage_->gameState.loaded)return finish(d,Status::invalidInput);
    for(auto value:storage_->gameState.local.volume)if(value>100)return finish(d,Status::invalidInput);
    constexpr std::array<std::uint16_t,4> ids{{38,112,114,165}};
    for(auto id:ids)if(!storage_->textWidgets[id].line(0))return finish(d,Status::invalidWidget);
    // ELF3AFFF0 "%d%%", SetText1F5FB8/1F5FE4/1F6010/1F603C.
    for(std::size_t i=0;i<ids.size();++i){const auto v=storage_->gameState.local.volume[i];
        // 2A9FF0 ->2A9DE0 calls29D940 then29DA40(255,indefinite), even
        // for an unchanged string. Both have the same original +CC lock gate.
        auto& text=storage_->textWidgets[ids[i]];if(text.line(0)->locked)continue;
        char bytes[4];std::size_t count=0;
        if(v>=100)bytes[count++]='1';if(v>=10)bytes[count++]=char('0'+(v/10)%10);
        bytes[count++]=char('0'+v%10);bytes[count++]='%';storage_->dynamic[ids[i]].assign(bytes,count);storage_->renderedVolume[ids[i]]=static_cast<std::int32_t>(v);
        d.text=text.setupLine(0,OriginalUiTextLineSetup50KG{});
        if(d.text!=Text::ok)return finish(d,Status::textFailure);
    }
    return Status::ok;
}
const std::string* OriginalUiOptionsMenuWidgets50KG::dynamicText(std::uint16_t id,std::size_t line) const noexcept {
    const auto* d=textDescriptor(id);return d&&d->textSource==assets::OriginalUiTextSource50KG::dynamic&&line<d->lines.size()&&line<2?
        (line?&storage_->dynamicSecond[id]:&storage_->dynamic[id]):nullptr;
}
const assets::OriginalUiPage50KG* OriginalUiOptionsMenuWidgets50KG::controlsPage() const noexcept {
    return storage_?storage_->controlsController.page():nullptr;
}
const std::vector<std::string>* OriginalUiOptionsMenuWidgets50KG::controlsLocalizedLines(std::uint16_t id,std::uint16_t stringId) const noexcept {
    if(!storage_||id>=168)return nullptr;
    for(const auto& value:storage_->localized[id])if(value.stringId==stringId)return &value.lines;
    return nullptr;
}
std::uint16_t OriginalUiOptionsMenuWidgets50KG::controlsRenderedString(std::uint16_t id,std::size_t line) const noexcept {
    return storage_&&id<168&&line<2?storage_->renderedString[id][line]:0xffffu;
}
Status OriginalUiOptionsMenuWidgets50KG::preflightControlsPlan(const OriginalUiControlsPlan50KG& plan,Diagnostic& d) const noexcept {
    d={};if(!storage_)return finish(d,Status::notBound);
    if(!plan.state.navigation.page||plan.state.navigation.page->identity!=controlsPage()->identity||
        plan.effectCount>plan.effects.size()||plan.state.controllerIndex4DC>1)return finish(d,Status::invalidCommand);
    for(std::size_t i=0;i<plan.effectCount;++i){const auto& e=plan.effects[i];d.callbackIndex=i;d.widgetId=e.widgetId;d.sourcePC=e.sourcePC;
        using E=OriginalUiControlsEffectKind50KG;
        switch(e.kind){
        case E::widget:if(!admitted(e.widgetId)||!valid(e.operation))return finish(d,Status::invalidCommand);break;
        case E::localizedText:if(!controlsLocalizedLines(e.widgetId,e.stringId))return finish(d,Status::invalidCommand);break;
        case E::model:if(e.widgetId!=36)return finish(d,Status::invalidWidget);break;
        case E::drawBackground:if(e.ordinal>3)return finish(d,Status::invalidCommand);break;
        case E::scene:case E::previewPresets:case E::renderAperture:break;
        default:return finish(d,Status::invalidCommand);
        }
    }
    return finish(d,Status::ok);
}
Status OriginalUiOptionsMenuWidgets50KG::commitControlsPlan(const OriginalUiControlsPlan50KG& plan,
    void* context,bool (*effect)(void*,const OriginalUiControlsEffect50KG&) noexcept,Diagnostic& d) noexcept {
    if(preflightControlsPlan(plan,d)!=Status::ok)return d.status;
    for(std::size_t i=0;i<plan.effectCount;++i){const auto& e=plan.effects[i];d.callbackIndex=i;d.widgetId=e.widgetId;d.sourcePC=e.sourcePC;
        if(e.kind==OriginalUiControlsEffectKind50KG::widget){
            const OriginalUiOptionsMenuCallback50KG callback{e.widgetId,e.operation,e.value,e.sourcePC};
            if(commitCallbacks(&callback,1,d)!=Status::ok)return d.status;
            if(storage_->state[e.widgetId].kind==Kind::model&&(!effect||!effect(context,e)))return finish(d,Status::controllerFailure);
        }else if(e.kind==OriginalUiControlsEffectKind50KG::localizedText){
            std::size_t written=0;for(const auto& value:storage_->localized[e.widgetId])if(value.stringId==e.stringId)written=value.writtenLineCount;
            const auto& lines=*controlsLocalizedLines(e.widgetId,e.stringId);auto& widget=storage_->textWidgets[e.widgetId];
            for(std::size_t line=0;line<lines.size();++line){const auto* state=widget.line(line);if(!state)return finish(d,Status::invalidWidget);
                if(state->locked)continue;
                (line?storage_->dynamicSecond[e.widgetId]:storage_->dynamic[e.widgetId]).assign(lines[line]);
                storage_->renderedString[e.widgetId][line]=e.stringId;
                //2A9DE0 sets up each written split line. Trailing empty lines
                // are only cleared at2A9FAC, without re-enabling the overlay.
                if(line<written){
                    d.text=widget.setupLine(line,OriginalUiTextLineSetup50KG{});if(d.text!=Text::ok)return finish(d,Status::textFailure);
                }
            }
        }else if(!effect||!effect(context,e))return finish(d,Status::controllerFailure);
    }
    if(plan.writesPageInputFlags42C)storage_->flags42C=plan.pageInputFlags42C;
    return finish(d,Status::ok);
}
std::int32_t OriginalUiOptionsMenuWidgets50KG::gameOptionsRenderedVolume(std::uint16_t id) const noexcept {
    return dynamicText(id)?storage_->renderedVolume[id]:-1;
}
std::uint16_t OriginalUiOptionsMenuWidgets50KG::gameOptionsSelectedValueWidget() const noexcept {
    return storage_?storage_->gameController.selectedValueWidget(storage_->gameState):0;
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
    const auto* positioned=text;
    // Cold variants retain the source justification and live empty-line gate.
    // No descriptor mutation or per-frame string allocation is needed.
    if(text->textSource==assets::OriginalUiTextSource50KG::dynamic){
        const auto* live=dynamicText(id,line);if(!live)return finish(d,Status::invalidInput);
        if(!positionOriginalUiText50KG({text,line,fontHeight,0,0},projected.position))return finish(d,Status::invalidInput);
        if(!live->empty()){
            const auto j=text->justification;
            if(j==assets::OriginalUiTextJustification50KG::right)projected.position.positionedX-=float(width);
            else if(j==assets::OriginalUiTextJustification50KG::centerBoth||j==assets::OriginalUiTextJustification50KG::centerHorizontal)
                projected.position.positionedX=float(static_cast<std::int32_t>(projected.position.positionedX-float(width)*.5f));
            if(j==assets::OriginalUiTextJustification50KG::centerBoth||j==assets::OriginalUiTextJustification50KG::centerVertical)
                projected.position.positionedY=float(static_cast<std::int32_t>(projected.position.positionedY-float(height)*.5f));
        }
    }else if(!positionOriginalUiText50KG({positioned,line,fontHeight,width,height},projected.position))return finish(d,Status::invalidInput);
    const auto result=storage_->textWidgets[id].projectLine(line,projected.line,shown);
    if(result!=Text::ok)return textResult(d,result);
    if(text->textSource==assets::OriginalUiTextSource50KG::dynamic)projected.line.text=dynamicText(id,line);
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

#include "original_ui_main_widget_manager50kg.h"
#include <string>
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalUiMainWidgetStatus50KG;
using Diagnostic=OriginalUiMainWidgetDiagnostic50KG;
using Kind=OriginalUiMainWidgetKind50KG;
using Op=OriginalUiControlOperation50KG;
using NavStatus=OriginalUiNavigationStatus50KG;
using TextStatus=OriginalUiTextWidgetStatus50KG;
using MovementStatus=OriginalUiMovementStatus50KG;
constexpr const char* mainPath="UI\\UIMain\\";
constexpr const char* pageNames[]={"","main.dat","legal.dat","opening.dat","demo.dat","gallery.dat"};
Status result(Diagnostic& d,Status s) noexcept {d.status=s;return s;}
bool operationValid(Op op) noexcept {
    switch(op) {
    case Op::show:case Op::hide:case Op::select:case Op::deselect:
    case Op::clearSelection:case Op::setEnabled:case Op::reset:return true;
    }
    return false;
}
bool pageIdValid(std::uint16_t id) noexcept {return id && id<=originalUiMainPageCount50KG;}
bool widgetIdValid(std::uint16_t id) noexcept {return id && id<=originalUiMainWidgetCount50KG;}
Kind expectedKind(std::uint16_t id) noexcept {
    if(id==3)return Kind::movement;
    if(id==4 || id==10)return Kind::empty;
    if(id==27 || id==28 || (id>=34 && id<=37) || (id>=47 && id<=49))return Kind::icon;
    return Kind::text;
}
const char* typeName(Kind kind) noexcept {
    switch(kind) {
    case Kind::text:return "Overlay_Text";
    case Kind::icon:return "Icon";
    case Kind::empty:return "Empty";
    case Kind::movement:return "Movement_Control";
    default:return "";
    }
}
Status textResult(Diagnostic& d,TextStatus s) noexcept {
    d.text=s;
    switch(s) {
    case TextStatus::ok:return result(d,Status::ok);
    case TextStatus::hidden:return result(d,Status::hidden);
    case TextStatus::lockedOut:return result(d,Status::lockedOut);
    case TextStatus::invalidLine:return result(d,Status::invalidLine);
    case TextStatus::invalidInput:return result(d,Status::invalidInput);
    default:return result(d,Status::textFailure);
    }
}
bool navigationBatch(NavStatus s) noexcept {
    return s==NavStatus::ok || s==NavStatus::noChange || s==NavStatus::unavailable;
}
void controlState(OriginalUiControlState50KG& s,Op op,bool value) noexcept {
    switch(op) {
    case Op::select:s.selected=true;break;
    case Op::deselect:case Op::clearSelection:s.selected=false;break;
    case Op::show:s.shown=true;break;
    case Op::hide:s.shown=false;break;
    case Op::setEnabled:s.enabled=value;break;
    case Op::reset:break;
    }
}
}

struct OriginalUiMainWidgetManager50KG::Storage {
    // Destruction runs in reverse: runtime owners die before their descriptors
    // and archive. Arrays never resize or relocate after cold publication.
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    assets::OriginalUiDat50KG ui,widgets;
    assets::OriginalUiPageIndex50KG index;
    std::array<assets::OriginalUiDat50KG,originalUiMainPageCount50KG+1> pageDats;
    std::array<assets::OriginalUiPage50KG,originalUiMainPageCount50KG+1> pages;
    std::array<OriginalUiNavigation50KG,originalUiMainPageCount50KG+1> navigation;
    std::array<OriginalUiMainInputRegistrations50KG,originalUiMainPageCount50KG+1> registrations;
    std::array<OriginalUiMainWidgetState50KG,originalUiMainWidgetCount50KG+1> states;
    std::array<assets::OriginalUiOverlayText50KG,originalUiMainWidgetCount50KG+1> texts;
    std::array<assets::OriginalUiIcon50KG,originalUiMainWidgetCount50KG+1> icons;
    assets::OriginalUiMovementControl50KG movement;
    OriginalUiMainWidgetOrder50KG order;
    struct IconImage {
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG> owner;
        std::uint32_t index=0;
        OriginalUiIconLayout50KG layout;
    };
    std::array<IconImage,originalUiMainWidgetCount50KG+1> images;
    std::array<OriginalUiTextWidget50KG,originalUiMainWidgetCount50KG+1> textWidgets;
    OriginalUiMovementWidget50KG movementWidget;
};
OriginalUiMainWidgetManager50KG::OriginalUiMainWidgetManager50KG() noexcept=default;
OriginalUiMainWidgetManager50KG::~OriginalUiMainWidgetManager50KG()=default;

Status OriginalUiMainWidgetManager50KG::bindMain(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const assets::OriginalUiEnglish50KG* english,Diagnostic& d) noexcept {
    d={};
    if(storage_)return result(d,Status::alreadyBound);
    if(!archive || !archive->loaded() || !archive->identity() ||
        archive->identity()->id!=assets::OriginalUiArchiveId50KG::mainMenu)
        return result(d,Status::invalidArchive);
    try {
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);
        if(!assets::readOriginalUiDat50KG(*next->archive,std::string(mainPath)+"ui.dat",next->ui,d.parse) ||
            !assets::readOriginalUiPageIndex50KG(next->ui,next->index,d.parse) ||
            !assets::readOriginalUiDat50KG(*next->archive,std::string(mainPath)+"Widgets.Dat",next->widgets,d.parse))
            return result(d,Status::parseFailure);
        if(next->index.pageFiles.size()!=originalUiMainPageCount50KG)
            return result(d,Status::invalidPage);
        // Authenticate manager/page pairing through actual index membership,
        // not authored PageNo. Demo's original PageNo=3 while its slot is4.
        for(std::uint16_t p=1;p<=originalUiMainPageCount50KG;++p) {
            d.pageId=p;
            const auto& authored=next->index.pageFiles[p-1];
            const auto member=next->archive->find(std::string(mainPath)+authored);
            const auto expected=next->archive->find(std::string(mainPath)+pageNames[p]);
            if(!member.identity || member.identity!=expected.identity)
                return result(d,Status::invalidPage);
            if(!assets::readOriginalUiDat50KG(*next->archive,std::string(mainPath)+authored,next->pageDats[p],d.parse) ||
                !assets::readOriginalUiPage50KG(next->pageDats[p],next->widgets,next->pages[p],d.parse))
                return result(d,Status::parseFailure);
            if(next->pages[p].widgetCount!=originalUiMainWidgetCount50KG ||
                !bindOriginalUiNavigation50KG(next->pages[p],next->navigation[p]))
                return result(d,Status::invalidPage);
        }
        for(std::uint16_t id=1;id<=originalUiMainWidgetCount50KG;++id) {
            d={};d.widgetId=id;
            const auto* section=next->widgets.section("Widget "+std::to_string(id));
            if(!section) {d.parse={assets::OriginalUiPageError50KG::missingSection,0,0x1CD858};
                return result(d,Status::parseFailure);}
            const auto* type=section->find("Type");const auto kind=expectedKind(id);
            if(!type || type->text!=typeName(kind))return result(d,Status::unsupportedWidget);
            auto& state=next->states[id];state.kind=kind;
            // Empty is source null1CD70C, NOT a UIWidget. Its ID is retained,
            // but no input flags, callbacks, resource or entity are invented.
            if(kind==Kind::empty)continue;
            bool onTop=false;
            if(!assets::originalUiBoolean50KG(*section,"OnTop",false,onTop,d.parse,0x1CD890) ||
                !assets::originalUiBoolean50KG(*section,"WantsJoystickInput1",false,state.wantsJoystickInput[0],d.parse,0x1CD8E0) ||
                !assets::originalUiBoolean50KG(*section,"WantsJoystickInput2",false,state.wantsJoystickInput[1],d.parse,0x1CD8E0))
                return result(d,Status::parseFailure);
            state.onTop=onTop;
            if(kind==Kind::text) {
                auto& descriptor=next->texts[id];
                if(!assets::readOriginalUiOverlayText50KG(next->widgets,id,english,descriptor,d.parse))
                    return result(d,Status::parseFailure);
                if(descriptor.textSource==assets::OriginalUiTextSource50KG::dynamic ||
                    state.wantsJoystickInput[0] || state.wantsJoystickInput[1])
                    return result(d,Status::unsupportedText);
                const auto status=next->textWidgets[id].bind(descriptor);
                if(status!=TextStatus::ok)return textResult(d,status);
            } else if(kind==Kind::icon) {
                if(!assets::readOriginalUiIcon50KG(next->widgets,id,next->icons[id],d.parse))
                    return result(d,Status::parseFailure);
                if(state.wantsJoystickInput[0] || state.wantsJoystickInput[1])
                    return result(d,Status::unsupportedWidget);
                // AlphaEnabled on47..49 is retained in widgetsDat. Icon2AC590
                // has no lookup for it; do not reinterpret it as GS blend state.
            } else {
                if(!assets::readOriginalUiMovementControl50KG(next->widgets,id,next->movement,d.parse))
                    return result(d,Status::parseFailure);
                d.movement.status=next->movementWidget.bindMain(next->movement);
                if(d.movement.status!=MovementStatus::ok)return result(d,Status::movementFailure);
            }
        }
        next->order.ordinaryWidgets.reserve(originalUiMainWidgetCount50KG);
        next->order.onTopWidgets.reserve(originalUiMainWidgetCount50KG);
        for(std::uint16_t id=1;id<=originalUiMainWidgetCount50KG;++id)
            if(next->states[id].kind!=Kind::empty && !next->states[id].onTop)
                next->order.ordinaryWidgets.push_back(id);
        for(std::uint16_t id=originalUiMainWidgetCount50KG;id;--id)
            if(next->states[id].kind!=Kind::empty && next->states[id].onTop)
                next->order.onTopWidgets.push_back(id);
        for(std::uint16_t p=1;p<=originalUiMainPageCount50KG;++p) {
            for(const auto& control:next->pages[p].controls) for(auto id:control.widgets) {
                d.pageId=p;d.widgetId=id;
                // Actual authenticated pages have no Empty memberships. A
                // future unexpected null reference must not call show/select
                // helpers that dereference null in the original.
                if(next->states[id].kind==Kind::empty)return result(d,Status::invalidWidget);
                for(std::size_t c=0;c<2;++c)if(next->states[id].wantsJoystickInput[c])
                    next->registrations[p].joystick[c].push_back(id);
            }
        }
        // Bounded dispatcher below covers the proven one Main registration.
        const auto& input=next->registrations[1].joystick;
        if(input[0].size()!=1 || input[0][0]!=3 || !input[1].empty())
            return result(d,Status::invalidInput);
        storage_=std::move(next);d={};return Status::ok;
    } catch(...) {d={};return result(d,Status::allocation);}
}

bool OriginalUiMainWidgetManager50KG::bound() const noexcept {return bool(storage_);}
const assets::OriginalUiDat50KG* OriginalUiMainWidgetManager50KG::uiDat() const noexcept {
    return storage_?&storage_->ui:nullptr;
}
const assets::OriginalUiDat50KG* OriginalUiMainWidgetManager50KG::widgetsDat() const noexcept {
    return storage_?&storage_->widgets:nullptr;
}
const assets::OriginalUiDat50KG* OriginalUiMainWidgetManager50KG::pageDat(std::uint16_t p) const noexcept {
    return storage_ && pageIdValid(p)?&storage_->pageDats[p]:nullptr;
}
const assets::OriginalUiPage50KG* OriginalUiMainWidgetManager50KG::page(std::uint16_t p) const noexcept {
    return storage_ && pageIdValid(p)?&storage_->pages[p]:nullptr;
}
const OriginalUiNavigation50KG* OriginalUiMainWidgetManager50KG::navigation(std::uint16_t p) const noexcept {
    return storage_ && pageIdValid(p)?&storage_->navigation[p]:nullptr;
}
const OriginalUiMainWidgetOrder50KG* OriginalUiMainWidgetManager50KG::managerWidgetOrder() const noexcept {
    return storage_?&storage_->order:nullptr;
}
const OriginalUiMainInputRegistrations50KG* OriginalUiMainWidgetManager50KG::inputRegistrations(std::uint16_t p) const noexcept {
    return storage_ && pageIdValid(p)?&storage_->registrations[p]:nullptr;
}
OriginalUiMainWidgetState50KG OriginalUiMainWidgetManager50KG::widgetState(std::uint16_t id) const noexcept {
    if(!storage_ || !widgetIdValid(id))return {};
    auto state=storage_->states[id];
    if(state.kind==Kind::text) {state.selected=storage_->textWidgets[id].selected();state.shown=storage_->textWidgets[id].shown();}
    else if(state.kind==Kind::movement) {state.selected=storage_->movementWidget.selected();state.shown=storage_->movementWidget.shown();}
    return state;
}
const assets::OriginalUiOverlayText50KG* OriginalUiMainWidgetManager50KG::textDescriptor(std::uint16_t id) const noexcept {
    return storage_ && widgetIdValid(id) && storage_->states[id].kind==Kind::text?&storage_->texts[id]:nullptr;
}
const assets::OriginalUiIcon50KG* OriginalUiMainWidgetManager50KG::iconDescriptor(std::uint16_t id) const noexcept {
    return storage_ && widgetIdValid(id) && storage_->states[id].kind==Kind::icon?&storage_->icons[id]:nullptr;
}
const OriginalUiTextWidget50KG* OriginalUiMainWidgetManager50KG::textWidget(std::uint16_t id) const noexcept {
    return textDescriptor(id)?&storage_->textWidgets[id]:nullptr;
}
const OriginalUiMovementWidget50KG* OriginalUiMainWidgetManager50KG::movementWidget(std::uint16_t id) const noexcept {
    return storage_ && id==3?&storage_->movementWidget:nullptr;
}

Status OriginalUiMainWidgetManager50KG::preflight(std::uint16_t p,const OriginalUiCommands50KG& commands,
    Diagnostic& d) const noexcept {
    d={};d.pageId=p;
    if(!storage_)return result(d,Status::notBound);
    if(!pageIdValid(p))return result(d,Status::invalidPage);
    if(commands.count>commands.values.size())return result(d,Status::invalidCommand);
    std::size_t count=0;
    for(std::size_t i=0;i<commands.count;++i) {
        const auto& command=commands.values[i];d.commandIndex=i;d.controlId=command.controlId;d.sourcePC=command.sourcePC;
        if(command.controlId>=storage_->pages[p].controls.size() || !operationValid(command.operation))
            return result(d,Status::invalidCommand);
        const auto& widgets=storage_->pages[p].controls[command.controlId].widgets;
        for(std::size_t j=0;j<widgets.size();++j) {
            const auto id=widgets[j];d.widgetId=id;d.membershipIndex=j;
            if(!widgetIdValid(id) || storage_->states[id].kind==Kind::empty || storage_->states[id].kind==Kind::absent)
                return result(d,Status::invalidWidget);
            ++count;
        }
    }
    d={};d.pageId=p;d.commandCount=commands.count;d.callbackCount=count;return Status::ok;
}
void OriginalUiMainWidgetManager50KG::commitWidget(std::uint16_t id,Op operation,bool value) noexcept {
    auto& state=storage_->states[id];
    if(state.kind==Kind::text) {
        if(operation==Op::setEnabled)(void)storage_->textWidgets[id].setControlEnabled(value);
        else (void)storage_->textWidgets[id].apply(operation,value);
    } else if(state.kind==Kind::movement) {
        (void)storage_->movementWidget.apply(operation,value);
    } else if(state.kind==Kind::icon) {
        const auto& descriptor=storage_->icons[id];
        const auto visibility=[&](bool show,bool hide){if(show!=hide)state.shown=show;};
        // Icon2ACD50/2ACDD0 share UIWidget selection/visibility semantics.
        // Neither modifies color or alpha. Inherited +80/+84 are separate
        // operations; UIControl2A5BA4 calls nonvirtual2A7A00 instead.
        switch(operation) {
        case Op::show:case Op::reset:state.shown=true;break;
        case Op::hide:state.shown=false;break;
        case Op::select:state.selected=true;visibility(descriptor.enableOnSelect,descriptor.disableOnSelect);break;
        case Op::deselect:state.selected=false;visibility(descriptor.enableOnDeselect,descriptor.disableOnDeselect);break;
        case Op::clearSelection:state.selected=false;break;
        case Op::setEnabled:state.selected=value;state.shown=value;break;
        }
    }
}
void OriginalUiMainWidgetManager50KG::commitCommands(std::uint16_t p,const OriginalUiCommands50KG& commands) noexcept {
    for(std::size_t i=0;i<commands.count;++i) {
        const auto& c=commands.values[i];
        for(auto id:storage_->pages[p].controls[c.controlId].widgets)commitWidget(id,c.operation,c.value);
        controlState(storage_->navigation[p].controls[c.controlId],c.operation,c.value);
    }
}
Status OriginalUiMainWidgetManager50KG::applyCommands(std::uint16_t p,const OriginalUiCommands50KG& commands,Diagnostic& d) noexcept {
    const auto s=preflight(p,commands,d);if(s!=Status::ok)return s;
    commitCommands(p,commands);return Status::ok;
}
Status OriginalUiMainWidgetManager50KG::applyWidget(std::uint16_t id,Op op,bool value,Diagnostic& d) noexcept {
    d={};d.widgetId=id;
    if(!storage_)return result(d,Status::notBound);
    if(!widgetIdValid(id) || storage_->states[id].kind==Kind::empty || storage_->states[id].kind==Kind::absent)
        return result(d,Status::invalidWidget);
    if(!operationValid(op))return result(d,Status::invalidCommand);
    commitWidget(id,op,value);d.callbackCount=1;return Status::ok;
}
Status OriginalUiMainWidgetManager50KG::finishNavigation(std::uint16_t p,OriginalUiNavigation50KG& next,
    const OriginalUiCommands50KG& commands,NavStatus navStatus,Diagnostic& d) noexcept {
    if(!navigationBatch(navStatus)) {d={};d.pageId=p;d.navigation=navStatus;return result(d,Status::navigationFailure);}
    const auto s=preflight(p,commands,d);d.navigation=navStatus;if(s!=Status::ok)return s;
    commitCommands(p,commands);storage_->navigation[p]=next;
    return result(d,navStatus==NavStatus::noChange?Status::noChange:navStatus==NavStatus::unavailable?Status::unavailable:Status::ok);
}
Status OriginalUiMainWidgetManager50KG::enterGroup(std::uint16_t p,std::uint16_t group,std::uint16_t slot,
    std::uint8_t controller,Diagnostic& d) noexcept {
    d={};d.pageId=p;if(!storage_)return result(d,Status::notBound);if(!pageIdValid(p))return result(d,Status::invalidPage);
    auto next=storage_->navigation[p];OriginalUiCommands50KG commands;
    const auto s=enterOriginalUiGroup50KG(next,group,slot,controller,commands);
    return finishNavigation(p,next,commands,s,d);
}
Status OriginalUiMainWidgetManager50KG::moveControl(std::uint16_t p,std::uint16_t group,bool forward,Diagnostic& d) noexcept {
    d={};d.pageId=p;if(!storage_)return result(d,Status::notBound);if(!pageIdValid(p))return result(d,Status::invalidPage);
    auto next=storage_->navigation[p];OriginalUiCommands50KG commands;
    const auto s=moveOriginalUiControl50KG(next,group,forward,commands);
    return finishNavigation(p,next,commands,s,d);
}
Status OriginalUiMainWidgetManager50KG::enableControl(std::uint16_t p,std::uint16_t control,bool enabled,Diagnostic& d) noexcept {
    d={};d.pageId=p;if(!storage_)return result(d,Status::notBound);if(!pageIdValid(p))return result(d,Status::invalidPage);
    auto next=storage_->navigation[p];OriginalUiCommands50KG commands;
    const auto s=enableOriginalUiControl50KG(next,control,enabled,commands);
    return finishNavigation(p,next,commands,s,d);
}
Status OriginalUiMainWidgetManager50KG::dispatchMain(std::uint8_t controller,OriginalUiMainInput50KG input,
    const OriginalUiMainContext50KG& context,OriginalUiMainDecision50KG& output,Diagnostic& d) noexcept {
    d={};d.pageId=1;if(!storage_)return result(d,Status::notBound);
    auto next=storage_->navigation[1];OriginalUiCommands50KG commands;OriginalUiMainDecision50KG decision;
    const auto ns=dispatchOriginalUiMain50KG(next,controller,input,context,commands,decision);
    const auto s=finishNavigation(1,next,commands,ns,d);
    if(s==Status::ok || s==Status::noChange || s==Status::unavailable)output=decision;
    return s;
}
Status OriginalUiMainWidgetManager50KG::dispatchMainMovement(std::uint8_t controller,
    assets::OriginalUiMovementInput50KG input,const OriginalUiMainMovementDelivery50KG& delivery,
    OriginalUiMovementNotification50KG& output,Diagnostic& d) noexcept {
    d={};d.pageId=1;d.widgetId=3;if(!storage_)return result(d,Status::notBound);
    if(controller>=2)return result(d,Status::invalidInput);
    // Cold admission proved the sole input registration is widget3 on0.
    // Snapshot AFTER the caller's page callback, BEFORE widget callbacks.
    OriginalUiMovementDelivery50KG sourceDelivery{delivery.containerReached,delivery.pageInputEnabled268,
        delivery.pageAccepted,storage_->movementWidget.selected()};
    auto next=storage_->navigation[1];OriginalUiCommands50KG commands;OriginalUiMovementNotification50KG notice;
    OriginalUiMovementDiagnostic50KG movementDiagnostic;
    const auto ms=storage_->movementWidget.dispatchMain(next,controller,input,sourceDelivery,commands,notice,movementDiagnostic);
    if(ms!=MovementStatus::ok && ms!=MovementStatus::noChange) {
        d.movement=movementDiagnostic;return result(d,Status::movementFailure);
    }
    const auto s=finishNavigation(1,next,commands,ms==MovementStatus::ok?NavStatus::ok:NavStatus::noChange,d);
    d.movement=movementDiagnostic;d.widgetId=3;
    if(s==Status::ok || s==Status::noChange)output=notice;
    return s;
}

Status OriginalUiMainWidgetManager50KG::applyMainWidgetPhase(OriginalUiMainWidgetPhase50KG phase,
    const OriginalUiMainWidgetPhaseInput50KG& input,Diagnostic& d) noexcept {
    d={};d.pageId=1;if(!storage_)return result(d,Status::notBound);
    const auto& page=storage_->pages[1];
    if(page.groups.size()<3 || page.controls.size()<8 || page.groups[1].controls.size()!=5)
        return result(d,Status::invalidPage);
    const bool needsProfile=phase==OriginalUiMainWidgetPhase50KG::openingAC || phase==OriginalUiMainWidgetPhase50KG::openedB0;
    if(needsProfile && (!input.originalProfileKnown || !input.savedSlot6E0 || input.savedSlot6E0>5))
        return result(d,Status::invalidInput);
    auto next=storage_->navigation[1];
    struct Callback {std::uint16_t widget=0;Op operation=Op::hide;bool value=false;std::uint32_t pc=0;};
    // Native bound for this authenticated 49-slot bank, not a source constant.
    // Largest actual Main phase fits comfortably, including all duplicates.
    std::array<Callback,128> callbacks{};std::size_t count=0,commands=0;
    const auto widget=[&](std::uint16_t id,Op op,bool value,std::uint32_t pc) {
        if(!widgetIdValid(id) || storage_->states[id].kind==Kind::empty ||
            storage_->states[id].kind==Kind::absent || count==callbacks.size())return false;
        callbacks[count++]={id,op,value,pc};return true;
    };
    const auto control=[&](std::int32_t id,Op op,bool value,std::uint32_t pc) {
        if(id==-1)return true;
        if(id<0 || static_cast<std::size_t>(id)>=page.controls.size())return false;
        for(auto member:page.controls[static_cast<std::size_t>(id)].widgets)
            if(!widget(member,op,value,pc))return false;
        controlState(next.controls[static_cast<std::size_t>(id)],op,value);++commands;return true;
    };
    const auto groupMembers=[&](std::size_t id,Op op,bool value,std::uint32_t memberPC,std::uint32_t associationPC) {
        if(id>=page.groups.size())return false;
        for(auto member:page.groups[id].controls)if(!control(member,op,value,memberPC))return false;
        for(auto member:page.groups[id].associations)if(!control(member,op,value,associationPC))return false;
        return true;
    };
    const auto groupEnabled=[&](std::size_t id,bool enabled) {
        if(id>=page.groups.size())return false;
        next.groups[id].enabled=enabled;
        return groupMembers(id,Op::setEnabled,enabled,0x2A5044,0x2A5088);
    };
    const auto groupDeselect=[&](std::size_t id) {
        if(id>=page.groups.size())return false;
        const auto& g=page.groups[id];
        for(auto member:g.associations)if(!control(member,Op::deselect,false,0x2A4E78))return false;
        for(auto member:g.controls)if(!control(member,g.keepEnabled?Op::clearSelection:Op::deselect,
            false,g.keepEnabled?0x2A4F10:0x2A4EC4))return false;
        return true;
    };
    const auto selectSlot=[&](std::size_t id,std::uint16_t slot) {
        if(id>=page.groups.size() || !slot || slot>page.groups[id].controls.size())return false;
        next.groups[id].selectedSlot=slot;
        for(auto member:page.groups[id].controls)if(!control(member,Op::deselect,false,0x2A4FA8))return false;
        return control(page.groups[id].controls[slot-1],Op::select,false,0x2A4FD4);
    };
    const auto descriptionsOff=[&](std::uint32_t firstPC) {
        for(std::uint16_t id=29;id<=33;++id)
            if(!widget(id,Op::deselect,false,firstPC+std::uint32_t(id-29)*0x1c))return false;
        return true;
    };
    bool valid=true;
    switch(phase) {
    case OriginalUiMainWidgetPhase50KG::requestedPage90:
        // 1D7390 ->2A6660 resets controller attachments, then every group.
        // Group2A57A0 resets members before associations; reset is show-only.
        next.controllerGroup={{-1,-1}};
        for(std::size_t group=0;valid && group<page.groups.size();++group)
            valid=groupMembers(group,Op::reset,false,0x2A57D8,0x2A5818);
        break;
    case OriginalUiMainWidgetPhase50KG::previousPage94:
        // 2A6810 group hide loop: members then associations, no deduplication.
        for(std::size_t group=0;valid && group<page.groups.size();++group)
            valid=groupMembers(group,Op::hide,false,0x2A4D58,0x2A4D8C);
        break;
    case OriginalUiMainWidgetPhase50KG::entry74:
        valid=widget(1,Op::setEnabled,false,0x1E5D40) && groupEnabled(1,false) && groupEnabled(2,false);
        break;
    case OriginalUiMainWidgetPhase50KG::openingAC: {
        const bool gallery=(input.profile11018&0x60)!=0;
        valid=groupEnabled(1,true) && groupDeselect(1) &&
            control(page.groups[1].controls[4],Op::setEnabled,gallery,gallery?0x1E679C:0x1E6728) &&
            widget(34,Op::setEnabled,gallery,gallery?0x1E67B0:0x1E673C) &&
            widget(28,Op::setEnabled,gallery,gallery?0x1E67C4:0x1E6750) &&
            widget(36,Op::setEnabled,!gallery,gallery?0x1E67D8:0x1E6764) &&
            widget(37,Op::setEnabled,!gallery,gallery?0x1E67EC:0x1E6778) &&
            selectSlot(1,input.savedSlot6E0) && descriptionsOff(0x1E6824);
        // The following source menu-effect9 call is outside this widget owner.
        break;
    }
    case OriginalUiMainWidgetPhase50KG::openedB0: {
        valid=widget(1,Op::setEnabled,true,0x1E6910) && groupEnabled(2,true);
        if(valid) {
            OriginalUiCommands50KG generated;
            const auto ns=enterOriginalUiGroup50KG(next,1,input.savedSlot6E0,0,generated);
            d.navigation=ns;
            if(ns!=NavStatus::ok)return result(d,Status::navigationFailure);
            for(std::size_t i=0;valid && i<generated.count;++i) {
                const auto& c=generated.values[i];valid=control(c.controlId,c.operation,c.value,c.sourcePC);
            }
        }
        break;
    }
    case OriginalUiMainWidgetPhase50KG::closingB4:
        valid=widget(1,Op::setEnabled,false,0x1E6980) && descriptionsOff(0x1E699C) && groupEnabled(2,false);
        // The following source effect fade234440 is outside this owner.
        break;
    default:return result(d,Status::invalidInput);
    }
    if(!valid)return result(d,Status::invalidCommand);
    // All validation and private navigation changes precede the first actual
    // callback. No allocation, external callback or reentry occurs in commit.
    for(std::size_t i=0;i<count;++i)commitWidget(callbacks[i].widget,callbacks[i].operation,callbacks[i].value);
    storage_->navigation[1]=next;d.commandCount=commands;d.callbackCount=count;return Status::ok;
}

Status OriginalUiMainWidgetManager50KG::prepareIcon(std::uint16_t id,
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,std::uint32_t index,
    std::int32_t depthMaximum,Diagnostic& d) noexcept {
    d={};d.widgetId=id;if(!storage_)return result(d,Status::notBound);
    const auto* descriptor=iconDescriptor(id);if(!descriptor)return result(d,Status::invalidWidget);
    if(storage_->images[id].owner)return result(d,Status::alreadyPrepared);
    if(!images || !images->loaded() || !images->textures())return result(d,Status::missingResource);
    const auto* texture=images->textures()->texture(index);
    if(!texture || !images->decodedTexture(index))return result(d,Status::missingResource);
    OriginalUiIconLayout50KG layout;
    d.icon=layoutOriginalUiIcon50KG(*descriptor,texture->texture,depthMaximum,layout);
    if(d.icon!=OriginalUiIconLayoutStatus50KG::complete)return result(d,Status::layoutFailure);
    auto& image=storage_->images[id];image.layout=layout;image.index=index;image.owner=std::move(images);
    return Status::ok;
}
Status OriginalUiMainWidgetManager50KG::projectIcon(std::uint16_t id,bool ancestorsShown,
    OriginalUiMainIconProjection50KG& output,Diagnostic& d) const noexcept {
    d={};d.widgetId=id;if(!storage_)return result(d,Status::notBound);
    const auto* descriptor=iconDescriptor(id);if(!descriptor)return result(d,Status::invalidWidget);
    if(!ancestorsShown || !storage_->states[id].shown)return result(d,Status::hidden);
    const auto& image=storage_->images[id];if(!image.owner)return result(d,Status::missingResource);
    output={descriptor,image.owner.get(),image.index,image.layout};return Status::ok;
}
OriginalUiTextWidget50KG* OriginalUiMainWidgetManager50KG::mutableText(std::uint16_t id,Diagnostic& d) noexcept {
    d={};d.widgetId=id;
    if(!storage_) {d.status=Status::notBound;return nullptr;}
    if(!textDescriptor(id)) {d.status=Status::invalidWidget;return nullptr;}
    return &storage_->textWidgets[id];
}
Status OriginalUiMainWidgetManager50KG::updateText(std::uint16_t id,bool ancestorsShown,Diagnostic& d) noexcept {
    auto* widget=mutableText(id,d);return widget?textResult(d,widget->updateStatic(ancestorsShown)):d.status;
}
Status OriginalUiMainWidgetManager50KG::projectText(std::uint16_t id,std::size_t line,
    const OriginalUiMainTextMetrics50KG& metrics,bool ancestorsShown,
    OriginalUiMainTextProjection50KG& output,Diagnostic& d) noexcept {
    auto* widget=mutableText(id,d);d.lineIndex=line;if(!widget)return d.status;
    const auto* descriptor=widget->descriptor();
    if(line>=descriptor->lines.size())return result(d,Status::invalidLine);
    OriginalUiMainTextProjection50KG projected;
    OriginalUiTextPositionInput50KG input;input.widget=descriptor;input.line=line;
    input.fontLineHeight=metrics.fontLineHeight;input.measuredWidth=metrics.measuredWidth;input.measuredHeight=metrics.measuredHeight;
    if(!positionOriginalUiText50KG(input,projected.position))return result(d,Status::invalidMetrics);
    const auto s=widget->projectLine(line,projected.line,ancestorsShown);
    if(s!=TextStatus::ok)return textResult(d,s);
    projected.descriptor=descriptor;projected.widgetId=id;projected.lineIndex=line;output=projected;
    return textResult(d,TextStatus::ok);
}
Status OriginalUiMainWidgetManager50KG::setupLine(std::uint16_t id,std::size_t line,
    const OriginalUiTextLineSetup50KG& setup,Diagnostic& d) noexcept {
    auto* widget=mutableText(id,d);d.lineIndex=line;return widget?textResult(d,widget->setupLine(line,setup)):d.status;
}
Status OriginalUiMainWidgetManager50KG::setLineColor(std::uint16_t id,std::size_t line,
    std::uint32_t color,Diagnostic& d) noexcept {
    auto* widget=mutableText(id,d);d.lineIndex=line;return widget?textResult(d,widget->setLineColor(line,color)):d.status;
}
Status OriginalUiMainWidgetManager50KG::setLineAlpha(std::uint16_t id,std::size_t line,
    std::int32_t alpha,float duration,Diagnostic& d) noexcept {
    auto* widget=mutableText(id,d);d.lineIndex=line;return widget?textResult(d,widget->setLineAlpha(line,alpha,duration)):d.status;
}
Status OriginalUiMainWidgetManager50KG::stopLine(std::uint16_t id,std::size_t line,Diagnostic& d) noexcept {
    auto* widget=mutableText(id,d);d.lineIndex=line;return widget?textResult(d,widget->stopLine(line)):d.status;
}
}

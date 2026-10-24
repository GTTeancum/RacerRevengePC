#include "original_ui_options_menu_controller50kg.h"
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalUiOptionsMenuStatus50KG;
using Op=OriginalUiControlOperation50KG;
using Plan=OriginalUiOptionsMenuPlan50KG;
using NavStatus=OriginalUiNavigationStatus50KG;
bool widget(Plan& p,std::uint16_t id,Op op,bool value,std::uint32_t pc) noexcept {
    if(!id || id>357 || p.callbackCount==p.callbacks.size())return false;
    p.callbacks[p.callbackCount++]={id,op,value,pc};return true;
}
bool control(Plan& p,std::int32_t id,Op op,bool value,std::uint32_t pc) noexcept {
    if(id==-1)return true;
    if(id<0 || static_cast<std::size_t>(id)>=p.navigation.page->controls.size())return false;
    for(auto w:p.navigation.page->controls[static_cast<std::size_t>(id)].widgets)
        if(!widget(p,w,op,value,pc))return false;
    auto& s=p.navigation.controls[static_cast<std::size_t>(id)];
    switch(op) {
    case Op::select:s.selected=true;break;
    case Op::deselect:case Op::clearSelection:s.selected=false;break;
    case Op::show:s.shown=true;break;
    case Op::hide:s.shown=false;break;
    case Op::setEnabled:s.enabled=value;break;
    case Op::reset:break;
    }
    return true;
}
bool group(Plan& p,std::size_t id,Op op,bool value,std::uint32_t memberPC,std::uint32_t associationPC) noexcept {
    const auto& g=p.navigation.page->groups[id];
    for(auto c:g.controls)if(!control(p,c,op,value,memberPC))return false;
    for(auto c:g.associations)if(!control(p,c,op,value,associationPC))return false;
    return true;
}
bool enabled(Plan& p,std::size_t id,bool value) noexcept {
    p.navigation.groups[id].enabled=value;
    return group(p,id,Op::setEnabled,value,0x2A5044,0x2A5088);
}
bool expand(Plan& p,const OriginalUiCommands50KG& commands) noexcept {
    for(std::size_t i=0;i<commands.count;++i) {
        const auto& c=commands.values[i];
        if(!control(p,c.controlId,c.operation,c.value,c.sourcePC))return false;
    }
    return true;
}
bool descriptions(Plan& p,std::uint32_t firstPC) noexcept {
    constexpr std::array<std::uint16_t,6> ids{{100,106,101,102,103,104}};
    for(std::size_t i=0;i<ids.size();++i)
        if(!widget(p,ids[i],Op::deselect,false,firstPC+static_cast<std::uint32_t>(i)*0x1c))return false;
    return true;
}
}
struct OriginalUiOptionsMenuController50KG::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    assets::OriginalUiDat50KG pageDat,widgets;
    assets::OriginalUiPage50KG page;
    assets::OriginalUiMovementControl50KG movement;
};
bool OriginalUiOptionsMenuController50KG::nativeSourceCurrent(
    const std::shared_ptr<const assets::OriginalUiArchive50KG>& expected,
    const assets::OriginalUiDat50KG*& out)const noexcept {
    out=nullptr;
    if(!storage_||!expected||storage_->archive.get()!=expected.get()||
       storage_->archive.owner_before(expected)||expected.owner_before(storage_->archive)||
       !expected->loaded()||expected->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options)||
       storage_->widgets.identity()!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,1)||
       storage_->page.widgetCount!=357)return false;
    out=&storage_->widgets;return true;
}
OriginalUiOptionsMenuController50KG::OriginalUiOptionsMenuController50KG() noexcept=default;
OriginalUiOptionsMenuController50KG::~OriginalUiOptionsMenuController50KG()=default;
Status OriginalUiOptionsMenuController50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};if(storage_)return Status::alreadyBound;
    if(!archive || !archive->loaded() || archive->identity()!=
        assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options))return Status::invalidArchive;
    try {
        auto s=std::make_unique<Storage>();s->archive=std::move(archive);
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\menu.dat",s->pageDat,d) ||
            !assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\Widgets.Dat",s->widgets,d,
                assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder) ||
            !assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d) ||
            !assets::readOriginalUiMovementControl50KG(s->widgets,5,s->movement,d))return Status::parseFailure;
        if(s->page.pageNo!=1 || s->page.widgetCount!=357 || s->page.controls.size()!=9 ||
            s->page.groups.size()!=3 || s->page.groups[1].controls.size()!=6 ||
            s->movement.gridSize || !s->movement.wantsJoystickInput[0] || s->movement.wantsJoystickInput[1])
            return Status::invalidPage;
        for(std::size_t i=0;i<16;++i)
            if(s->movement.mappings[i]!=(i==2?-2:(i==3?-1:0)))return Status::invalidPage;
        storage_=std::move(s);return Status::ok;
    } catch(...) {return Status::allocation;}
}
const assets::OriginalUiPage50KG* OriginalUiOptionsMenuController50KG::page() const noexcept {
    return storage_?&storage_->page:nullptr;
}
const assets::OriginalUiDat50KG* OriginalUiOptionsMenuController50KG::widgetsDat() const noexcept {
    return storage_?&storage_->widgets:nullptr;
}
bool OriginalUiOptionsMenuController50KG::initialNavigation(OriginalUiNavigation50KG& out) const noexcept {
    return storage_ && bindOriginalUiNavigation50KG(storage_->page,out);
}
Status OriginalUiOptionsMenuController50KG::phase(const OriginalUiNavigation50KG& nav,
    OriginalUiOptionsMenuPhase50KG phase,bool known,std::uint16_t slot,Plan& output) const noexcept {
    if(!storage_)return Status::notBound;
    if(nav.page!=page())return Status::invalidPage;
    if((phase==OriginalUiOptionsMenuPhase50KG::openingAC || phase==OriginalUiOptionsMenuPhase50KG::openedB0) &&
        (!known || !slot || slot>6))return Status::invalidInput;
    Plan p;p.navigation=nav;bool valid=true;
    switch(phase) {
    case OriginalUiOptionsMenuPhase50KG::requestedPage90:
        p.navigation.controllerGroup={{-1,-1}};
        for(std::size_t i=0;valid && i<3;++i)valid=group(p,i,Op::reset,false,0x2A57D8,0x2A5818);
        break;
    case OriginalUiOptionsMenuPhase50KG::previousPage94:
        for(std::size_t i=0;valid && i<3;++i)valid=group(p,i,Op::hide,false,0x2A4D58,0x2A4D8C);
        break;
    case OriginalUiOptionsMenuPhase50KG::entry74:
        p.writesPageInputFlags42C=true;p.pageInputFlags42C=0;
        valid=widget(p,1,Op::setEnabled,false,0x1F9570) && widget(p,2,Op::setEnabled,false,0x1F9584) &&
            widget(p,3,Op::setEnabled,false,0x1F9598) && enabled(p,1,false) && enabled(p,2,false);
        break;
    case OriginalUiOptionsMenuPhase50KG::openingAC:
        valid=enabled(p,1,true);
        // 2A6BC0 selects the slot without deselecting groups or attaching a controller.
        p.navigation.groups[1].selectedSlot=slot;
        for(auto c:page()->groups[1].controls)if(valid)valid=control(p,c,Op::deselect,false,0x2A4FA8);
        if(valid)valid=control(p,page()->groups[1].controls[slot-1],Op::select,false,0x2A4FD4);
        if(valid)valid=descriptions(p,0x1F9D70);
        break;
    case OriginalUiOptionsMenuPhase50KG::openedB0: {
        p.writesPageInputFlags42C=true;p.pageInputFlags42C=3;
        valid=widget(p,1,Op::setEnabled,true,0x1F9E44) && widget(p,2,Op::setEnabled,true,0x1F9E58) &&
            widget(p,3,Op::setEnabled,true,0x1F9E6C) && widget(p,3,Op::deselect,false,0x1F9E88) && enabled(p,2,true);
        OriginalUiCommands50KG commands;
        if(valid) {
            const auto status=enterOriginalUiGroup50KG(p.navigation,1,slot,0,commands);
            if(status!=NavStatus::ok)return Status::navigationFailure;
            valid=expand(p,commands);
        }
        break;
    }
    case OriginalUiOptionsMenuPhase50KG::closingB4:
        p.writesPageInputFlags42C=true;p.pageInputFlags42C=0;
        valid=widget(p,1,Op::setEnabled,false,0x1F9EF0) && widget(p,2,Op::setEnabled,false,0x1F9F04) &&
            descriptions(p,0x1F9F20) && enabled(p,2,false);
        break;
    default:return Status::invalidInput;
    }
    if(!valid)return Status::capacity;
    output=p;return Status::ok;
}
Status OriginalUiOptionsMenuController50KG::pageInput(const OriginalUiNavigation50KG& nav,
    std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    const OriginalUiOptionsMenuInput50KG& delivery,const OriginalUiOptionsContext50KG& context,
    OriginalUiOptionsMenuPageResult50KG& output) const noexcept {
    if(!storage_)return Status::notBound;
    if(nav.page!=page())return Status::invalidPage;
    const auto event=static_cast<std::uint8_t>(input);
    if(controller>=2 || event>=16)return Status::invalidInput;
    OriginalUiOptionsMenuPageResult50KG r;
    if(!delivery.containerReached || !delivery.pageInputEnabled268) {output=r;return Status::noChange;}
    r.pageAccepted=true; // Unconditional source return at1F9A64.
    if(controller!=0 || context.popupActive || context.sourceInputBlocked) {output=r;return Status::noChange;}
    if(event==2 || event==3) {
        r.soundId=event==2?3:4;r.soundPC=event==2?0x1F9A00:0x1F9A34;
        r.flushPC=event==2?0x1F9A18:0x1F9A4C;r.soundRequested=true;r.flushRequested=true;
        output=r;return Status::ok;
    }
    if(event!=4 && event!=6) {output=r;return Status::noChange;}
    r.routeStatus=dispatchOriginalUiOptions50KG(nav,controller,
        event==4?OriginalUiRouteInput50KG::activate:OriginalUiRouteInput50KG::back,context,r.route);
    if(r.routeStatus!=OriginalUiRouteStatus50KG::ok && r.routeStatus!=OriginalUiRouteStatus50KG::unavailable &&
        r.routeStatus!=OriginalUiRouteStatus50KG::noChange)return Status::navigationFailure;
    if(r.routeStatus==OriginalUiRouteStatus50KG::noChange) {output=r;return Status::noChange;}
    // Missing controller: popup call1F9878 returns to unconditional branch
    // 1F9880->1F991C, then remembered-slot store1F9940, sound1F9950, flush1F9968.
    // Native unavailable is not a source early return. These remain requests;
    // the caller must not commit a rejected route/profile change.
    r.soundRequested=true;r.flushRequested=true;r.soundId=event==4?1:5;
    r.soundPC=event==4?0x1F9950:0x1F99D0;r.flushPC=event==4?0x1F9968:0x1F99E8;
    output=r;return r.routeStatus==OriginalUiRouteStatus50KG::unavailable?Status::unavailable:Status::ok;
}
Status OriginalUiOptionsMenuController50KG::movementInput(const OriginalUiNavigation50KG& nav,
    std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    const OriginalUiOptionsMenuInput50KG& delivery,bool pageAccepted,Plan& output,
    OriginalUiMovementNotification50KG& notification) const noexcept {
    if(!storage_)return Status::notBound;
    if(nav.page!=page())return Status::invalidPage;
    const auto event=static_cast<std::uint8_t>(input);
    if(controller>=2 || event>=16)return Status::invalidInput;
    Plan p;p.navigation=nav;OriginalUiMovementNotification50KG notice;
    const auto mapping=storage_->movement.mappings[event];
    if(!delivery.containerReached || !delivery.pageInputEnabled268 || !pageAccepted ||
        !delivery.movementSelectedAtSnapshot || !storage_->movement.wantsJoystickInput[controller] || !mapping ||
        nav.controllerGroup[controller]==-1) {output=p;notification=notice;return Status::noChange;}
    const auto attached=nav.controllerGroup[controller];
    if(attached<0 || static_cast<std::size_t>(attached)>=page()->groups.size())return Status::invalidPage;
    OriginalUiCommands50KG commands;
    const auto status=moveOriginalUiControl50KG(p.navigation,static_cast<std::uint16_t>(attached),mapping==-1,commands);
    if(status!=NavStatus::ok && status!=NavStatus::noChange)return Status::navigationFailure;
    if(!expand(p,commands))return Status::capacity;
    notice.issued=true;notice.mapping=mapping;notice.callPC=0x2AB490;notice.targetPC=0x2A4330;
    output=p;notification=notice;return status==NavStatus::ok?Status::ok:Status::noChange;
}
}

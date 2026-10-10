#include "original_ui_navigation50kg.h"

namespace rr::game {
namespace {
using Op=OriginalUiControlOperation50KG;
using Status=OriginalUiNavigationStatus50KG;
bool valid(const assets::OriginalUiPage50KG& p) noexcept {
    if (!p.identity || p.controls.empty() || p.groups.empty() ||
        p.controls.size()>assets::originalUiControlLimit50KG ||
        p.groups.size()>assets::originalUiGroupLimit50KG ||
        p.widgetCount>assets::originalUiWidgetLimit50KG) return false;
    std::size_t total=0;
    for (const auto& c:p.controls) {
        total+=c.widgets.size();
        if (total>assets::originalUiMembershipLimit50KG) return false;
        for (auto id:c.widgets) if (!id || id>p.widgetCount) return false;
    }
    for (const auto& g:p.groups) {
        total+=g.controls.size()+g.associations.size();
        if (total>assets::originalUiMembershipLimit50KG) return false;
        for (auto id:g.controls) if (id<-1 || id>=static_cast<std::int32_t>(p.controls.size())) return false;
        for (auto id:g.associations) if (id>=p.controls.size()) return false;
    }
    return true;
}
bool command(OriginalUiNavigation50KG& n,OriginalUiCommands50KG& c,
    std::int32_t id,Op op,std::uint32_t pc,bool value=false) noexcept {
    if (id<0) return true; // Original null ControlID member.
    if (c.count==c.values.size()) return false;
    c.values[c.count++]={static_cast<std::uint16_t>(id),op,value,pc};
    auto& s=n.controls[std::size_t(id)];
    if (op==Op::select) s.selected=true;
    else if (op==Op::deselect || op==Op::clearSelection) s.selected=false;
    else if (op==Op::show) s.shown=true;
    else if (op==Op::hide) s.shown=false;
    else if (op==Op::setEnabled) s.enabled=value;
    return true;
}
bool deselectGroup(OriginalUiNavigation50KG& n,OriginalUiCommands50KG& c,std::size_t id) noexcept {
    const auto& g=n.page->groups[id];
    for (auto control:g.associations) if (!command(n,c,control,Op::deselect,0x2A4E78)) return false;
    const auto op=g.keepEnabled?Op::clearSelection:Op::deselect;
    for (auto control:g.controls) if (!command(n,c,control,op,g.keepEnabled?0x2A4F10:0x2A4EC4)) return false;
    return true;
}
bool selectSlot(OriginalUiNavigation50KG& n,OriginalUiCommands50KG& c,
    std::size_t id,std::uint16_t slot) noexcept {
    const auto& g=n.page->groups[id];
    n.groups[id].selectedSlot=slot;
    for (auto control:g.controls) if (!command(n,c,control,Op::deselect,0x2A4FA8)) return false;
    return command(n,c,g.controls[slot-1],Op::select,0x2A4FD4);
}
}
bool bindOriginalUiNavigation50KG(const assets::OriginalUiPage50KG& p,
    OriginalUiNavigation50KG& out) noexcept {
    if (!valid(p)) return false;
    OriginalUiNavigation50KG result;result.page=&p;
    for (std::size_t i=0;i<p.controls.size();++i) result.controls[i].enabled=p.controls[i].present;
    for (std::size_t i=0;i<p.groups.size();++i)
        result.groups[i].selectedSlot=p.groups[i].controls.empty()?0:1;
    out=result;return true;
}
Status enterOriginalUiGroup50KG(OriginalUiNavigation50KG& nav,std::uint16_t id,
    std::uint16_t slot,std::uint8_t controller,OriginalUiCommands50KG& output) noexcept {
    if (!nav.page || !valid(*nav.page)) return Status::invalidPage;
    if (controller>=2) return Status::invalidController;
    if (id>=nav.page->groups.size() || !nav.groups[id].enabled) return Status::invalidGroup;
    if (!slot || slot>nav.page->groups[id].controls.size()) return Status::invalidSlot;
    OriginalUiNavigation50KG next=nav;OriginalUiCommands50KG commands;
    for (std::size_t i=1;i<nav.page->groups.size();++i) {
        bool owned=false;for (auto group:nav.controllerGroup) if (group==static_cast<std::int16_t>(i)) owned=true;
        if (!owned && !deselectGroup(next,commands,i)) return Status::commandCapacity;
    }
    for (auto c:nav.page->groups[id].associations)
        if (!command(next,commands,c,Op::select,0x2A4DFC)) return Status::commandCapacity;
    if (!selectSlot(next,commands,id,slot)) return Status::commandCapacity;
    next.controllerGroup[controller]=static_cast<std::int16_t>(id);
    nav=next;output=commands;return Status::ok;
}
Status moveOriginalUiControl50KG(OriginalUiNavigation50KG& nav,std::uint16_t id,
    bool forward,OriginalUiCommands50KG& output) noexcept {
    if (!nav.page || !valid(*nav.page)) return Status::invalidPage;
    if (id>=nav.page->groups.size()) return Status::invalidGroup;
    const auto& group=nav.page->groups[id];
    const auto count=group.controls.size();const auto old=nav.groups[id].selectedSlot;
    if (!count) { output={};return Status::noChange; }
    if (!old || old>count) return Status::invalidSlot;
    auto slot=std::size_t(old);bool found=false;
    for (std::size_t attempts=0;attempts<count;++attempts) {
        slot=forward ? (slot==count?1:slot+1) : (slot==1?count:slot-1);
        const auto c=group.controls[slot-1];
        if (c>=0 && nav.controls[std::size_t(c)].enabled && !nav.controls[std::size_t(c)].blocked) {
            found=true;break;
        }
    }
    if (!found) return Status::noSelectableControl;
    OriginalUiNavigation50KG next=nav;OriginalUiCommands50KG commands;
    if (!command(next,commands,group.controls[old-1],Op::deselect,forward?0x2A5110:0x2A51D0) ||
        !command(next,commands,group.controls[slot-1],Op::select,forward?0x2A518C:0x2A5244))
        return Status::commandCapacity;
    next.groups[id].selectedSlot=static_cast<std::uint16_t>(slot);
    nav=next;output=commands;return Status::ok;
}
Status selectOriginalUiGroupSlot50KG(OriginalUiNavigation50KG& nav,std::uint16_t id,
    std::uint16_t slot,OriginalUiCommands50KG& output) noexcept {
    if(!nav.page || !valid(*nav.page))return Status::invalidPage;
    if(id>=nav.page->groups.size())return Status::invalidGroup;
    if(!slot || slot>nav.page->groups[id].controls.size())return Status::invalidSlot;
    OriginalUiNavigation50KG next=nav;OriginalUiCommands50KG commands;
    if(!selectSlot(next,commands,id,slot))return Status::commandCapacity;
    nav=next;output=commands;return Status::ok;
}
Status moveOriginalUiGroup50KG(OriginalUiNavigation50KG& nav,std::uint8_t controller,
    bool forward,OriginalUiCommands50KG& output) noexcept {
    if(!nav.page || !valid(*nav.page))return Status::invalidPage;
    if(controller>=nav.controllerGroup.size())return Status::invalidController;
    const auto old=nav.controllerGroup[controller];
    if(old<0){output={};return Status::noChange;}
    const auto count=nav.page->groups.size()-1;
    if(!count || !old || static_cast<std::size_t>(old)>count)return Status::invalidGroup;
    std::size_t id=static_cast<std::size_t>(old);bool found=false;
    for(std::size_t attempt=0;attempt<count;++attempt){
        id=forward?(id==count?1:id+1):(id==1?count:id-1);
        if(nav.groups[id].enabled){found=true;break;}
    }
    if(!found)return Status::noSelectableControl;
    const auto slot=nav.groups[id].selectedSlot;
    if(!slot || slot>nav.page->groups[id].controls.size())return Status::invalidSlot;
    OriginalUiNavigation50KG next=nav;OriginalUiCommands50KG commands;
    if(!deselectGroup(next,commands,static_cast<std::size_t>(old)))return Status::commandCapacity;
    for(auto c:nav.page->groups[id].associations)
        if(!command(next,commands,c,Op::select,0x2A4DFC))return Status::commandCapacity;
    if(!selectSlot(next,commands,id,slot))return Status::commandCapacity;
    next.controllerGroup[controller]=static_cast<std::int16_t>(id);
    nav=next;output=commands;return Status::ok;
}
Status enableOriginalUiControl50KG(OriginalUiNavigation50KG& nav,std::uint16_t id,
    bool enabled,OriginalUiCommands50KG& output) noexcept {
    if (!nav.page || !valid(*nav.page) || id>=nav.page->controls.size()) return Status::invalidPage;
    OriginalUiCommands50KG commands;
    if (!command(nav,commands,id,Op::setEnabled,0x2A5B60,enabled)) return Status::commandCapacity;
    output=commands;return Status::ok;
}
Status dispatchOriginalUiMain50KG(OriginalUiNavigation50KG& nav,std::uint8_t controller,
    OriginalUiMainInput50KG input,const OriginalUiMainContext50KG& context,
    OriginalUiCommands50KG& commands,OriginalUiMainDecision50KG& decision) noexcept {
    if (!nav.page || !valid(*nav.page) || nav.page->groups.size()<=1) return Status::invalidPage;
    OriginalUiMainDecision50KG result;
    result.selectedSlot=nav.groups[1].selectedSlot;result.sourcePC=0x1E5E00;
    if (controller!=0 || context.popupActive || context.sourceInputBlocked) {
        commands={};decision=result;return Status::noChange;
    }
    if (input==OriginalUiMainInput50KG::previous || input==OriginalUiMainInput50KG::next) {
        const auto status=moveOriginalUiControl50KG(nav,1,input==OriginalUiMainInput50KG::next,commands);
        if (status!=Status::ok && status!=Status::noChange) return status;
        result.action=OriginalUiMainAction50KG::changedSelection;
        result.selectedSlot=nav.groups[1].selectedSlot;decision=result;return status;
    }
    if (input!=OriginalUiMainInput50KG::activate) return Status::unsupportedInput;
    if (result.selectedSlot<1 || result.selectedSlot>5 ||
        result.selectedSlot>nav.page->groups[1].controls.size()) return Status::invalidSlot;
    const auto control=nav.page->groups[1].controls[result.selectedSlot-1];
    if (control<0 || !nav.controls[std::size_t(control)].enabled || nav.controls[std::size_t(control)].blocked)
        return Status::noSelectableControl;
    if (result.selectedSlot<=3) {
        result.originalGameMode=static_cast<std::int8_t>(result.selectedSlot-1);
        result.originalPlayerBits=result.selectedSlot==2?0x40:0x20;
        result.routeArgument=1;result.targetScreen=5;
        result.sourcePC=result.selectedSlot==1?0x1E5EC8:(result.selectedSlot==2?0x1E5F2C:0x1E60EC);
        if (result.selectedSlot==2 && !context.secondControllerPresent)
            result.action=OriginalUiMainAction50KG::secondControllerRequired;
        else if (!(context.supportedGameModes&(1u<<result.originalGameMode)))
            result.action=OriginalUiMainAction50KG::unavailableMode;
        else result.action=OriginalUiMainAction50KG::changeScreen;
    } else if (result.selectedSlot==4) {
        result.routeArgument=1;result.targetScreen=12;result.sourcePC=0x1E6150;
        result.action=context.supportsScreen12?OriginalUiMainAction50KG::changeScreen:
            OriginalUiMainAction50KG::unavailableScreen;
    } else {
        result.sourcePC=0x1E6170;result.routeArgument=5; // Same current screen, new page5.
        result.action=context.supportsPage5?OriginalUiMainAction50KG::changePage:
            OriginalUiMainAction50KG::unavailableScreen;
    }
    commands={};decision=result;
    return result.action==OriginalUiMainAction50KG::changeScreen ||
        result.action==OriginalUiMainAction50KG::changePage ? Status::ok : Status::unavailable;
}
}

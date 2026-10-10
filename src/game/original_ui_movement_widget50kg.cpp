#include "original_ui_movement_widget50kg.h"
#include <cstring>

namespace rr::game {
namespace {
using Status=OriginalUiMovementStatus50KG;
Status result(OriginalUiMovementDiagnostic50KG& d,Status status,std::uint32_t pc=0) noexcept {
    d.status=status;d.sourcePC=pc;return status;
}
bool mainPageIdentity(const assets::OriginalUiMemberIdentity50KG* identity) noexcept {
    if(!identity)return false;
    const auto* archive=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::mainMenu);
    if(!archive)return false;
    for(std::size_t i=0;i<archive->memberCount;++i) {
        const auto* member=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::mainMenu,i);
        if(member==identity)return std::strcmp(member->name,"UI\\UIMain\\main.dat")==0 ||
            std::strcmp(member->name,"UI/UIMain/main.dat")==0;
    }
    return false;
}
}
Status OriginalUiMovementWidget50KG::bindMain(const assets::OriginalUiMovementControl50KG& descriptor) noexcept {
    if(descriptor_)return Status::alreadyBound;
    if(!assets::isOriginalUiMainMovement50KG(descriptor))return Status::invalidDescriptor;
    if(descriptor.gridSize!=0)return Status::unsupportedMapping;
    for(auto mapping:descriptor.mappings)if(mapping!=0&&mapping!=-1&&mapping!=-2)
        return Status::unsupportedMapping;
    descriptor_=&descriptor;selected_=false;shown_=false;return Status::ok;
}
Status OriginalUiMovementWidget50KG::apply(OriginalUiControlOperation50KG operation,bool value) noexcept {
    if(!descriptor_)return Status::notBound;
    switch(operation) {
    case OriginalUiControlOperation50KG::show:shown_=true;break;
    case OriginalUiControlOperation50KG::hide:shown_=false;break;
    case OriginalUiControlOperation50KG::select:selected_=true;shown_=true;break;
    case OriginalUiControlOperation50KG::deselect:selected_=false;shown_=false;break;
    case OriginalUiControlOperation50KG::clearSelection:selected_=false;break;
    case OriginalUiControlOperation50KG::setEnabled:selected_=value;shown_=value;break;
    case OriginalUiControlOperation50KG::reset:shown_=true;break;
    default:return Status::invalidOperation;
    }
    return Status::ok;
}
Status OriginalUiMovementWidget50KG::dispatchMain(OriginalUiNavigation50KG& nav,
    std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    const OriginalUiMovementDelivery50KG& delivery,OriginalUiCommands50KG& commands,
    OriginalUiMovementNotification50KG& notification,OriginalUiMovementDiagnostic50KG& d) const noexcept {
    d={};
    if(!descriptor_)return result(d,Status::notBound);
    if(controller>=descriptor_->wantsJoystickInput.size())return result(d,Status::invalidController,0x2AB3A4);
    const auto button=static_cast<std::uint8_t>(input);
    if(button>=descriptor_->mappings.size())return result(d,Status::invalidInput,0x2AB1E4);
    if(!nav.page||!mainPageIdentity(nav.page->identity)||nav.page->pageNo!=1)
        return result(d,Status::invalidPage,0x2AB3AC);
    if(!delivery.containerReached||!delivery.pageInputEnabled268||!delivery.pageAccepted||
        !delivery.selectedAtSnapshot||!descriptor_->wantsJoystickInput[controller]) {
        commands={};notification={};return result(d,Status::noChange,0x2A73A0);
    }
    const auto attached=nav.controllerGroup[controller];
    if(attached==-1) {commands={};notification={};return result(d,Status::noChange,0x2AB3C0);}
    if(attached<0||static_cast<std::size_t>(attached)>=nav.page->groups.size())
        return result(d,Status::invalidPage,0x2AB3B8);
    const auto mapping=descriptor_->mappings[button];
    if(mapping==0) {commands={};notification={};return result(d,Status::noChange,0x2AB470);}
    if(mapping!=-1&&mapping!=-2)return result(d,Status::unsupportedMapping,0x2AB3D4);
    auto next=nav;OriginalUiCommands50KG generated;
    const auto status=moveOriginalUiControl50KG(next,static_cast<std::uint16_t>(attached),mapping==-1,generated);
    d.navigation=status;
    if(status!=OriginalUiNavigationStatus50KG::ok&&status!=OriginalUiNavigationStatus50KG::noChange)
        return result(d,Status::navigationFailure,mapping==-1?0x2AB400:0x2AB410);
    OriginalUiMovementNotification50KG notice;notice.issued=true;notice.mapping=mapping;
    notice.callPC=0x2AB490;notice.targetPC=0x2A4330;
    nav=next;commands=generated;notification=notice;
    return result(d,status==OriginalUiNavigationStatus50KG::noChange?Status::noChange:Status::ok,
        mapping==-1?0x2AB400:0x2AB410);
}
}

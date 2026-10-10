#include "original_ui_movement_control50kg.h"
#include <cstring>

namespace rr::assets {
namespace {
constexpr const char* mappingKeys[]={"Lpad_Left","Lpad_Right","Lpad_Up","Lpad_Down",
    "Rpad_Down","Rpad_Left","Rpad_Up","Rpad_Right","Start","Select",
    "L1","R1","L2","R2","L3","R3"};
constexpr std::uint32_t mappingReadPC[]={0x2AB058,0x2AB070,0x2AB088,0x2AB0A0,
    0x2AB0B8,0x2AB0D0,0x2AB0E8,0x2AB100,0x2AB118,0x2AB130,
    0x2AB148,0x2AB160,0x2AB178,0x2AB190,0x2AB1A8,0x2AB1C0};
char lower(char c) noexcept {return c>='A'&&c<='Z'?char(c+'a'-'A'):c;}
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(lower(a[i])!=lower(b[i]))return false;
    return i==a.size()&&!b[i];
}
bool known(const std::string& key) noexcept {
    for(const char* candidate:mappingKeys)if(same(key,candidate))return true;
    return same(key,"Type")||same(key,"GridSize")||same(key,"OnTop")||
        same(key,"WantsJoystickInput1")||same(key,"WantsJoystickInput2");
}
}
bool readOriginalUiMovementControl50KG(const OriginalUiDat50KG& widgets,
    std::uint16_t widgetId,OriginalUiMovementControl50KG& output,
    OriginalUiPageDiagnostic50KG& d) noexcept {
    if(!widgets.identity()){d={OriginalUiPageError50KG::missingMember,0,0x2AB010};return false;}
    try {
        const auto* s=widgets.section("Widget "+std::to_string(widgetId));
        if(!s){d={OriginalUiPageError50KG::missingSection,0,0x2AB010};return false;}
        const auto* type=s->find("Type");
        if(!type||type->text!="Movement_Control") {
            d={OriginalUiPageError50KG::unsupportedValue,type?type->valueOffset:s->sourceOffset,0x1CD4F8};
            return false;
        }
        for(const auto& value:s->values)if(value.sourceValuePresent&&!known(value.key)) {
            d={OriginalUiPageError50KG::unsupportedValue,value.keyOffset,0x2AB010};return false;
        }
        OriginalUiMovementControl50KG next;next.identity=widgets.identity();next.widgetId=widgetId;
        if(!originalUiInteger50KG(*s,"GridSize",0,next.gridSize,d,0x2AB040))return false;
        for(std::size_t i=0;i<next.mappings.size();++i)
            if(!originalUiInteger50KG(*s,mappingKeys[i],0,next.mappings[i],d,mappingReadPC[i]))return false;
        if(!originalUiBoolean50KG(*s,"WantsJoystickInput1",false,next.wantsJoystickInput[0],d,0x1CD8E0)||
            !originalUiBoolean50KG(*s,"WantsJoystickInput2",false,next.wantsJoystickInput[1],d,0x1CD8E0)||
            !originalUiBoolean50KG(*s,"OnTop",false,next.onTop,d,0x1CD890))return false;
        output=next;d={};return true;
    } catch(...) {d={OriginalUiPageError50KG::allocation,0,0x2AB010};return false;}
}
bool isOriginalUiMainMovement50KG(const OriginalUiMovementControl50KG& descriptor) noexcept {
    if(!descriptor.identity||descriptor.widgetId!=3)return false;
    const auto* archive=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::mainMenu);
    if(!archive)return false;
    for(std::size_t i=0;i<archive->memberCount;++i) {
        const auto* member=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::mainMenu,i);
        if(member==descriptor.identity)
            return std::strcmp(member->name,"UI\\UIMain\\Widgets.Dat")==0 ||
                std::strcmp(member->name,"UI/UIMain/Widgets.Dat")==0;
    }
    return false;
}
}

#include "original_ui_course_widget_controller50kg.h"
#include <utility>
namespace rr::game { namespace {
using S=OriginalUiCourseWidgetControllerStatus50KG;using P=OriginalUiCourseWidgetPlan50KG;
using O=OriginalUiControlOperation50KG;using K=OriginalUiCourseWidgetStepKind50KG;
bool step(P& p,std::uint16_t widget,O op,bool value,std::uint32_t pc)noexcept {
 if(!widget||widget>84||p.count==p.steps.size())return false;p.steps[p.count++]={K::widget,widget,op,value,pc};return true;}
bool control(P& p,std::int32_t id,O op,bool value,std::uint32_t pc)noexcept {
 if(id==-1)return true;if(id<0||std::size_t(id)>=p.navigation.page->controls.size())return false;
 for(auto widget:p.navigation.page->controls[std::size_t(id)].widgets)if(!step(p,widget,op,value,pc))return false;
 auto& s=p.navigation.controls[std::size_t(id)];switch(op){case O::select:s.selected=true;break;case O::deselect:case O::clearSelection:s.selected=false;break;case O::show:s.shown=true;break;case O::hide:s.shown=false;break;case O::setEnabled:s.enabled=value;break;case O::reset:break;}return true;}
bool group(P& p,std::size_t id,O op,bool value,std::uint32_t pc,std::uint32_t assoc)noexcept {
 if(id>=p.navigation.page->groups.size())return false;const auto& g=p.navigation.page->groups[id];
 for(auto c:g.controls)if(!control(p,c,op,value,pc))return false;for(auto c:g.associations)if(!control(p,c,op,value,assoc))return false;return true;}
bool enable(P& p,std::size_t id,bool v)noexcept {p.navigation.groups[id].enabled=v;return group(p,id,O::setEnabled,v,0x2A5044,0x2A5088);}
bool expand(P& p,const OriginalUiCommands50KG& c)noexcept {for(std::size_t i=0;i<c.count;++i){const auto& s=c.values[i];if(!control(p,s.controlId,s.operation,s.value,s.sourcePC))return false;}return true;}
bool audio(P& p,K k,std::uint32_t pc)noexcept {if(p.count==p.steps.size())return false;p.steps[p.count++]={k,0,O::hide,false,pc};return true;}
bool prompt(P& p,std::uint32_t mask,std::uint32_t pc)noexcept {if(p.count==p.steps.size())return false;p.steps[p.count++]={K::promptMask42C,0,O::hide,false,pc,mask};p.writesPromptMask42C=true;p.promptMask42C=mask;return true;}
bool bounded(const OriginalUiCourseWidgetProfile50KG& p)noexcept {
 constexpr std::array<std::uint8_t,13> order{{0,11,4,8,7,1,5,9,12,6,10,3,2}};
 return p.mode34==0&&p.course700>=1&&p.course700<=13&&p.subtype70C>=1&&p.subtype70C<=3&&(!(p.flags100BC&1)||p.nativeBetaStorage)&&(p.flags11018&31u)>=1&&(p.flags11018&31u)<=13&&((p.flags1125C>>5)&3)<2&&p.courseRecords100C4==order;}
bool boundedResumableTournament(const OriginalUiCourseWidgetProfile50KG& p)noexcept {
 constexpr std::array<std::uint8_t,13> order{{0,11,4,8,7,1,5,9,12,6,10,3,2}};
 if(p.mode34!=2||p.career100C0>=27||p.career100C0!=p.selectedRider||!p.selectedUnlock10FAC||
    (p.progress219C&31u)>=13u||p.courseSlot14E4!=(p.progress219C&31u)+1u||
    p.tier14E8!=(p.courseSlot14E4<5?1u:p.courseSlot14E4<9?2u:3u)||
    ((p.flags100BC&1)&&!p.nativeBetaStorage)||((p.flags1125C>>5)&3)!=1||p.courseRecords100C4!=order)return false;
 for(auto medal:p.medals1938)if(medal>8)return false;
 return true;
}
}
struct OriginalUiCourseWidgetController50KG::Storage {std::shared_ptr<const assets::OriginalUiArchive50KG> archive;assets::OriginalUiDat50KG pageDat,widgets;assets::OriginalUiPage50KG page;std::array<std::int32_t,24> courseIndex{};};
OriginalUiCourseWidgetController50KG::OriginalUiCourseWidgetController50KG()noexcept=default;
OriginalUiCourseWidgetController50KG::~OriginalUiCourseWidgetController50KG()=default;
S OriginalUiCourseWidgetController50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,assets::OriginalUiPageDiagnostic50KG& d)noexcept {
 d={};if(storage_)return S::alreadyBound;if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course))return S::archive;
 try{auto s=std::make_unique<Storage>();s->archive=std::move(archive);
 if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UICourse/track.dat",s->pageDat,d)||!assets::readOriginalUiDat50KG(*s->archive,"UI/UICourse/Widgets.Dat",s->widgets,d,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||!assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d))return S::parse;
 if(s->page.pageNo!=1||s->page.widgetCount!=84||s->page.controls.size()!=24||s->page.groups.size()!=6||s->page.groups[1].controls.size()!=13||s->page.groups[2].controls.size()!=3||!s->page.groups[3].controls.empty()||s->page.groups[4].controls!=std::vector<std::int16_t>{22}||s->page.groups[5].controls!=std::vector<std::int16_t>{23})return S::page;
 s->courseIndex.fill(-1);for(auto c:s->page.groups[1].controls){if(c<0||std::size_t(c)>=s->courseIndex.size())return S::page;const auto* item=s->pageDat.section("Control "+std::to_string(c));if(!item||!item->find("CourseIndex")||!assets::originalUiInteger50KG(*item,"CourseIndex",-1,s->courseIndex[std::size_t(c)],d,0x1DA380))return S::parse;}
 if(s->courseIndex[std::size_t(s->page.groups[1].controls[0])]!=0)return S::page;storage_=std::move(s);return S::ok;
 }catch(...){return S::allocation;}}
const assets::OriginalUiPage50KG* OriginalUiCourseWidgetController50KG::page()const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiCourseWidgetController50KG::widgetsDat()const noexcept{return storage_?&storage_->widgets:nullptr;}
bool OriginalUiCourseWidgetController50KG::initialNavigation(OriginalUiNavigation50KG& n)const noexcept{return storage_&&bindOriginalUiNavigation50KG(storage_->page,n);}
S OriginalUiCourseWidgetController50KG::moveCourse(const OriginalUiNavigation50KG& n,bool forward,
    const OriginalUiCourseWidgetProfile50KG& profile,P& out)const noexcept{
    if(!storage_)return S::notBound;if(n.page!=page())return S::page;if(!bounded(profile))return S::unsupportedProfile;
    if(n.controllerGroup[0]!=1||!n.groups[1].enabled)return S::invalidNavigation;
    P p;p.navigation=n;OriginalUiCommands50KG commands;
    if(moveOriginalUiControl50KG(p.navigation,1,forward,commands)!=OriginalUiNavigationStatus50KG::ok||!expand(p,commands))return S::invalidNavigation;
    const auto slot=p.navigation.groups[1].selectedSlot;
    if(!slot||slot>page()->groups[1].controls.size()||!control(p,page()->groups[1].controls[slot-1],O::reset,false,0x2A5A38))return S::invalidNavigation;
    out=p;return S::ok;
}
S OriginalUiCourseWidgetController50KG::moveEventType(const OriginalUiNavigation50KG& n,bool forward,
    const OriginalUiCourseWidgetProfile50KG& profile,P& out)const noexcept {
    if(!storage_)return S::notBound;if(n.page!=page())return S::page;if(!bounded(profile))return S::unsupportedProfile;
    if(n.controllerGroup[0]!=2||!n.groups[2].enabled)return S::invalidNavigation;
    P p;p.navigation=n;OriginalUiCommands50KG commands;
    //1DF210/1DF4C8 move the real group2;1DF224/1DF4DC read its slot.
    if(moveOriginalUiControl50KG(p.navigation,2,forward,commands)!=OriginalUiNavigationStatus50KG::ok||!expand(p,commands))return S::invalidNavigation;
    const auto subtype=p.navigation.groups[2].selectedSlot;
    if(subtype<1||subtype>3)return S::invalidNavigation;
    //2A4C70/2A4D20 show/hide controls then associations, not enable/disable.
    if(!group(p,3,subtype==3?O::hide:O::show,false,
        subtype==3?0x2A4D58:0x2A4CA8,subtype==3?0x2A4D8C:0x2A4CDC))return S::capacity;
    out=p;return S::ok;
}
S OriginalUiCourseWidgetController50KG::moveRow(const OriginalUiNavigation50KG& n,bool down,
    const OriginalUiCourseWidgetProfile50KG& profile,P& out)const noexcept {
    if(!storage_)return S::notBound;if(n.page!=page())return S::page;if(!bounded(profile))return S::unsupportedProfile;
    const auto old=n.controllerGroup[0];
    if(old<1||old>3||n.controllerGroup[1]!=-1||!n.groups[1].enabled||!n.groups[2].enabled||!n.groups[3].enabled||
       !n.groups[1].selectedSlot||n.groups[1].selectedSlot>page()->groups[1].controls.size()||
       (n.groups[2].selectedSlot<1||n.groups[2].selectedSlot>3)||n.groups[3].selectedSlot!=0)return S::invalidNavigation;
    // Source1DF674/1DF810 choose the destination explicitly, not by scanning
    // every enabled group (the enabled Back/Confirm groups are not rows).
    const auto last=n.groups[2].selectedSlot==3?2:3;
    if(old>last)return S::invalidNavigation;
    const auto id=static_cast<std::size_t>(down?(old==last?1:old+1):(old==1?last:old-1));
    P p;p.navigation=n;const auto& previous=page()->groups[static_cast<std::size_t>(old)];
    //2A6A80 -> 2A4E40: associations first, then controls in authored order.
    for(auto c:previous.associations)if(!control(p,c,O::deselect,false,0x2A4E78))return S::capacity;
    for(auto c:previous.controls)if(!control(p,c,previous.keepEnabled?O::clearSelection:O::deselect,false,
        previous.keepEnabled?0x2A4F10:0x2A4EC4))return S::capacity;
    const auto& next=page()->groups[id];
    for(auto c:next.associations)if(!control(p,c,O::select,false,0x2A4DFC))return S::capacity;
    //2A4F64/68 returns before writing +3C if the group has zero controls.
    // Never invent a control or slot for the genuine lap group3.
    if(!next.controls.empty()){
        const auto slot=p.navigation.groups[id].selectedSlot;
        if(!slot||slot>next.controls.size())return S::invalidNavigation;
        for(auto c:next.controls)if(!control(p,c,O::deselect,false,0x2A4FA8))return S::capacity;
        if(!control(p,next.controls[slot-1],O::select,false,0x2A4FD4))return S::capacity;
    }
    p.navigation.controllerGroup[0]=static_cast<std::int16_t>(id); //2A6B78/88
    out=p;return S::ok;
}
S OriginalUiCourseWidgetController50KG::moveLaps(const OriginalUiNavigation50KG& n,std::uint32_t laps,bool forward,
    const OriginalUiCourseWidgetProfile50KG& profile,OriginalUiCourseLapMove50KG& out)const noexcept {
    if(!storage_)return S::notBound;if(n.page!=page())return S::page;if(!bounded(profile))return S::unsupportedProfile;
    if(n.controllerGroup[0]!=3||n.controllerGroup[1]!=-1||!n.groups[1].enabled||!n.groups[2].enabled||!n.groups[3].enabled||
       !n.groups[1].selectedSlot||n.groups[1].selectedSlot>page()->groups[1].controls.size()||
       n.groups[3].selectedSlot!=0||(n.groups[2].selectedSlot!=1&&n.groups[2].selectedSlot!=2)||laps<1||laps>(n.groups[2].selectedSlot==2?8u:25u))return S::invalidNavigation;
    OriginalUiCourseLapMove50KG next;
    const auto maximum=n.groups[2].selectedSlot==2?8u:25u;
    next.laps14EC=forward?(laps==maximum?1:laps+1):(laps==1?maximum:laps-1);
    next.sourcePC=n.groups[2].selectedSlot==2?(forward?0x1DF604:0x1DF350):(forward?0x1DF5C4:0x1DF310);
    next.audioPC=forward?0x1DF64C:0x1DF398;next.effectId=forward?4u:3u;
    // Source bits3F266666. These are the initial physical-edge timers.
    // The same live Course Session later consumes actual held levels in
    // update1DFEA0; it never replays this edge to synthesize a repeat.
    next.repeatRight14D4=forward?0.65f:0.0f;next.repeatLeft14D8=forward?0.0f:0.65f;
    out=next;return S::ok;
}
S OriginalUiCourseWidgetController50KG::phase(const OriginalUiNavigation50KG& n,OriginalUiCourseWidgetPhase50KG phase,const OriginalUiCourseWidgetProfile50KG& profile,P& out)const noexcept {
 if(!storage_)return S::notBound;if(n.page!=page())return S::page;
 const bool tournament=boundedResumableTournament(profile);
 if(!bounded(profile)&&!tournament)return S::unsupportedProfile;
 P p;p.navigation=n;bool ok=true;switch(phase){
 case OriginalUiCourseWidgetPhase50KG::requestedPage90:p.navigation.controllerGroup={{-1,-1}};for(std::size_t i=0;i<6&&ok;++i)ok=group(p,i,O::reset,false,0x2A57D8,0x2A5818);break;
 case OriginalUiCourseWidgetPhase50KG::previousPage94:for(std::size_t i=0;i<6&&ok;++i)ok=group(p,i,O::hide,false,0x2A4D58,0x2A4D8C);break;
 case OriginalUiCourseWidgetPhase50KG::entry74:ok=prompt(p,0,0x1DA4AC);for(std::size_t i=1;i<6&&ok;++i)ok=enable(p,i,false);break;
 case OriginalUiCourseWidgetPhase50KG::openingAC:{ok=enable(p,1,true);for(auto c:page()->groups[1].controls)if(ok)ok=control(p,c,O::setEnabled,false,0x1E0380);
 const auto available=tournament?std::uint32_t((profile.progress219C&31u)+1u):std::uint32_t(profile.flags11018&31u);
 for(std::size_t i=0;ok&&i<available;++i)for(auto c:page()->groups[1].controls)if(storage_->courseIndex[std::size_t(c)]==profile.courseRecords100C4[i]){ok=control(p,c,O::setEnabled,true,0x1E03F0);break;}
 OriginalUiCommands50KG commands;
 if(tournament){
  // 1DA810 sets a1=1 in the branch delay slot. 1DA8A8 enables group5;
  // mode2 never enables groups2/3. Current slot-1==progress skips Widget63 disable.
  if(ok)ok=enable(p,5,true);
 }else{
  if(ok)ok=enable(p,2,true);if(ok){if(selectOriginalUiGroupSlot50KG(p.navigation,2,static_cast<std::uint16_t>(profile.subtype70C),commands)!=OriginalUiNavigationStatus50KG::ok)return S::invalidNavigation;ok=expand(p,commands);}
  if(ok)ok=enable(p,3,true);
  if(ok&&profile.subtype70C==3)ok=group(p,3,O::hide,false,0x2A4D58,0x2A4D8C);
 }
 const auto slot=tournament?profile.courseSlot14E4:profile.course700;
 if(ok){if(enterOriginalUiGroup50KG(p.navigation,1,static_cast<std::uint16_t>(slot),0,commands)!=OriginalUiNavigationStatus50KG::ok)return S::invalidNavigation;ok=expand(p,commands);}
 if(ok)ok=audio(p,K::audioStartLoop11,0x1DA924);if(ok)ok=enable(p,4,false);break;}
 case OriginalUiCourseWidgetPhase50KG::openedB0:
  ok=enable(p,4,true);if(ok)ok=prompt(p,3,0x1DA974);
  // Resumed next-course entry preserves source input ownership; a completed
  // podium advances to the next slot, while nonpodium retries retain Confirm.
  if(tournament){if(ok)ok=step(p,7,O::deselect,false,0x1DAA38);if(ok)ok=step(p,8,O::deselect,false,0x1DAA54);}break;
 case OriginalUiCourseWidgetPhase50KG::closingB4:ok=prompt(p,0,0x1DAA7C);if(ok)ok=enable(p,4,false);if(ok)ok=audio(p,K::audioFade1,0x1DAAA4);break;
 case OriginalUiCourseWidgetPhase50KG::closedB8:break;
 default:return S::invalidNavigation;}
 if(!ok)return S::capacity;out=p;return S::ok;}
}

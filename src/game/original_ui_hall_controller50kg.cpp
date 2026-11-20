#include "original_ui_hall_controller50kg.h"
#include <cmath>
#include <utility>
namespace rr::game { namespace {
using S=OriginalUiOptionsMenuStatus50KG;using P=OriginalUiHallPlan50KG;using State=OriginalUiHallState50KG;using O=OriginalUiControlOperation50KG;
bool callback(P& p,std::uint16_t id,O op,bool v,std::uint32_t pc) noexcept {if(!id||id>30||p.callbackCount==p.callbacks.size())return false;p.callbacks[p.callbackCount++]={id,op,v,pc};return true;}
bool control(P& p,std::int32_t id,O op,bool v,std::uint32_t pc) noexcept {
 if(id==-1)return true;if(id<0||std::size_t(id)>=p.state.navigation.page->controls.size())return false;
 for(auto w:p.state.navigation.page->controls[id].widgets)if(!callback(p,w,op,v,pc))return false;
 auto& c=p.state.navigation.controls[id];switch(op){case O::select:c.selected=true;break;case O::deselect:case O::clearSelection:c.selected=false;break;case O::show:c.shown=true;break;case O::hide:c.shown=false;break;case O::setEnabled:c.enabled=v;break;case O::reset:break;}return true;
}
bool group(P& p,unsigned id,O op,bool v,std::uint32_t pc,std::uint32_t assoc) noexcept {const auto& g=p.state.navigation.page->groups[id];for(auto c:g.controls)if(!control(p,c,op,v,pc))return false;for(auto c:g.associations)if(!control(p,c,op,v,assoc))return false;if(op==O::setEnabled)p.state.navigation.groups[id].enabled=v;return true;}
bool enabled(P& p,unsigned id,bool v) noexcept{return group(p,id,O::setEnabled,v,0x2A5044,0x2A5088);}
bool expand(P& p,const OriginalUiCommands50KG& c) noexcept {for(std::size_t i=0;i<c.count;++i)if(!control(p,c.values[i].controlId,c.values[i].operation,c.values[i].value,c.values[i].sourcePC))return false;return true;}
bool profile(const OriginalUiHallProfile50KG& p) noexcept {constexpr std::array<std::uint8_t,13> order{{0,11,4,8,7,1,5,9,12,6,10,3,2}};
 if(!p.ownerReady||(p.availability11018&31)<1||(p.availability11018&31)>13||p.courseOrder100C4!=order)return false;
 if(p.mode34==5)return p.remembered6EC>=1&&p.remembered6EC<=13;
 //1E4764: mode2 opens on slot 187B10+1 (6EC is not read). 1E4718: mode0
 // opens on P+700 (the Free Play race-return Hall, 1AB884). Mode1 (P+710)
 // and the 1E4788 "Invalid gamemode" assertion stay unsupported here.
 if(p.mode34==0)return p.course700>=1&&p.course700<=13;
 return p.mode34==2&&p.order187B10<13;}
// Cross table 1E3FC0..1E41C8. Mode2 rows; any other mode -> Course 6/1.
bool confirmDecision(const OriginalUiHallProfile50KG& v,OriginalUiHallConfirmRoute50KG& route,std::uint8_t& screen,std::uint32_t& pc) noexcept {
 using R=OriginalUiHallConfirmRoute50KG;screen=0;pc=0;
 if(v.mode34!=2){route=R::course;screen=6;pc=0x1E41C4;return true;}
 if(v.boundary102CC){const auto next=v.order187B10+1u;
  if(next==4u||next==8u){route=R::win;screen=7;pc=0x1E4048;return true;}
  if(next==13u){if(v.finalMedal1938==1u){route=R::ending;screen=3;pc=0x1E40A0;return false;}route=R::win;screen=7;pc=0x1E40EC;return true;}
  route=R::assertion;pc=0x1E4148;return false;}
 if(v.unlockPending187E30){route=R::unlock;screen=10;pc=0x1E418C;return true;}
 route=R::garage;screen=13;pc=0x1E41AC;return true;}
bool valid(const State& s,const assets::OriginalUiPage50KG* page) noexcept{return page&&s.navigation.page==page&&std::isfinite(s.deferred1460)&&s.deferred1460>=0&&s.deferred1460<=3&&std::floor(s.deferred1460)==s.deferred1460;}
}
struct OriginalUiHallOfFameController50KG::Storage {std::shared_ptr<const assets::OriginalUiArchive50KG> archive;assets::OriginalUiDat50KG dat,widgets;assets::OriginalUiPage50KG page;std::array<std::int32_t,17> courses{},planets{};};
OriginalUiHallOfFameController50KG::OriginalUiHallOfFameController50KG() noexcept=default;
OriginalUiHallOfFameController50KG::~OriginalUiHallOfFameController50KG()=default;
S OriginalUiHallOfFameController50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> a,assets::OriginalUiPageDiagnostic50KG& d) noexcept {
 d={};if(storage_)return S::alreadyBound;if(!a||!a->loaded()||a->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::hallOfFame))return S::invalidArchive;
 try{auto s=std::make_unique<Storage>();s->archive=std::move(a);
 if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIHOF/hall.dat",s->dat,d)||!assets::readOriginalUiDat50KG(*s->archive,"UI/UIHOF/Widgets.Dat",s->widgets,d)||!assets::readOriginalUiPage50KG(s->dat,s->widgets,s->page,d))return S::parseFailure;
 constexpr std::array<std::int16_t,13> order{{1,4,5,2,3,6,9,7,13,14,11,12,10}};
 if(s->page.pageNo!=1||s->page.widgetCount!=30||s->page.groups.size()!=3||s->page.controls.size()!=17||s->page.groups[1].controls.size()!=13||s->page.groups[2].controls!=std::vector<std::int16_t>{16}||s->page.groups[1].associations!=std::vector<std::uint16_t>{15})return S::invalidPage;
 constexpr const char* planets[]={"tatooine","gamorr","ryloth","coruscant","sullust","mon calamari"};s->courses.fill(-1);s->planets.fill(-1);
 for(std::size_t i=0;i<order.size();++i){const auto id=order[i];if(s->page.groups[1].controls[i]!=id)return S::invalidPage;const auto* c=s->dat.section("Control "+std::to_string(id));if(!c||!assets::originalUiInteger50KG(*c,"CourseIndex",-1,s->courses[id],d,0x1E0760)||s->courses[id]<0||s->courses[id]>12)return S::parseFailure;const auto* v=c->find("Planet");if(!v)return S::parseFailure;for(int j=0;j<6;++j)if(v->text==planets[j])s->planets[id]=j;if(s->planets[id]<0)return S::invalidPage;}
 storage_=std::move(s);return S::ok;}catch(...){return S::allocation;}
}
const assets::OriginalUiPage50KG* OriginalUiHallOfFameController50KG::page()const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiHallOfFameController50KG::widgetsDat()const noexcept{return storage_?&storage_->widgets:nullptr;}
bool OriginalUiHallOfFameController50KG::initialState(State& out)const noexcept {if(!storage_)return false;State s;if(!bindOriginalUiNavigation50KG(*page(),s.navigation))return false;out=s;return true;}
bool OriginalUiHallOfFameController50KG::selectedCourse(const State& s,std::uint32_t& course,std::uint32_t& planet)const noexcept {if(!valid(s,page()))return false;auto slot=s.navigation.groups[1].selectedSlot;if(!slot||slot>13)return false;const auto id=page()->groups[1].controls[slot-1];course=storage_->courses[id];planet=storage_->planets[id];return true;}
S OriginalUiHallOfFameController50KG::phase(const State& s,OriginalUiHallPhase50KG phase,const OriginalUiHallProfile50KG& view,P& out)const noexcept {
 if(!storage_)return S::notBound;if(!valid(s,page())||!profile(view))return S::invalidInput;P p;p.state=s;bool ok=true;
 switch(phase){
 case OriginalUiHallPhase50KG::requestedPage90:case OriginalUiHallPhase50KG::previousPage94:
 if(phase==OriginalUiHallPhase50KG::requestedPage90)p.state.navigation.controllerGroup={{-1,-1}};
 for(unsigned g=0;g<3&&ok;++g)ok=group(p,g,phase==OriginalUiHallPhase50KG::requestedPage90?O::reset:O::hide,false,phase==OriginalUiHallPhase50KG::requestedPage90?0x2A57D8:0x2A4D58,phase==OriginalUiHallPhase50KG::requestedPage90?0x2A5818:0x2A4D8C);break;
 case OriginalUiHallPhase50KG::entry74:p.state.loaded=true;p.state.inputFlags42C=0;p.writesInputFlags=true;ok=callback(p,2,O::setEnabled,false,0x1E1714)&&callback(p,1,O::setEnabled,false,0x1E1728)&&enabled(p,1,false)&&enabled(p,2,false);p.activateCamera=true;p.clearScene3D8=true;p.applyBackground430=true;break;
 case OriginalUiHallPhase50KG::openingAC:{if(!s.loaded)return S::invalidInput;ok=enabled(p,1,true);for(auto c:page()->groups[1].controls)if(ok)ok=control(p,c,O::setEnabled,false,0x1E44E4);
 for(unsigned i=0;i<(view.availability11018&31)&&ok;++i)for(auto c:page()->groups[1].controls)if(storage_->courses[c]==view.courseOrder100C4[i]){ok=control(p,c,O::setEnabled,true,0x1E4550);break;}
 // 1E46F4 mode5 -> P+6EC; 1E4764 mode2 -> 187B10(P)+1.
 const auto slot=static_cast<std::uint16_t>(view.mode34==5?view.remembered6EC:
     view.mode34==0?view.course700:view.order187B10+1u);
 OriginalUiCommands50KG commands;if(ok)ok=enterOriginalUiGroup50KG(p.state.navigation,1,slot,0,commands)==OriginalUiNavigationStatus50KG::ok&&expand(p,commands);
 std::uint32_t course=0;if(ok)ok=selectedCourse(p.state,course,p.planet);p.writePlanet=true;p.planetPC=0x1E4834;p.startSong5=true;if(ok)ok=enabled(p,2,false);break;}
 case OriginalUiHallPhase50KG::openedB0:if(!s.loaded)return S::invalidInput;
 // 1E48A4: mode5 -> 42C=2 + prompt[154]+4 (widget2); otherwise 42C=1 + [154]+0.
 if(view.mode34==5){ok=enabled(p,2,true)&&callback(p,2,O::setEnabled,true,0x1E48C0);p.state.inputFlags42C=2;}
 else{ok=enabled(p,2,true)&&callback(p,1,O::setEnabled,true,0x1E48E0);p.state.inputFlags42C=1;}
 p.writesInputFlags=true;break;
 case OriginalUiHallPhase50KG::closingB4:p.state.inputFlags42C=0;p.writesInputFlags=true;ok=callback(p,2,O::setEnabled,false,0x1E4920)&&callback(p,1,O::setEnabled,false,0x1E4934)&&enabled(p,2,false);p.fadeMusic=true;break;
 case OriginalUiHallPhase50KG::closedB8:break;
 }
 if(!ok)return S::capacity;out=p;return S::ok;
}
S OriginalUiHallOfFameController50KG::input(const State& s,std::uint8_t controller,assets::OriginalUiMovementInput50KG event,bool reached,bool inputEnabled,const OriginalUiHallProfile50KG& view,P& out)const noexcept {
 if(!storage_)return S::notBound;if(!valid(s,page())||!profile(view)||controller>1||static_cast<unsigned>(event)>15)return S::invalidInput;P p;p.state=s;
 if(!reached||!inputEnabled){out=p;return S::noChange;}p.pageAccepted=true;if(controller){out=p;return S::noChange;}const auto e=static_cast<unsigned>(event);
 if(e==4&&(s.inputFlags42C&1)){
  //1E3FC0: bit0 only (never set by the mode5 opening). Movement deferral is
  // not consulted, matching Back. The table reads live profile facts.
  OriginalUiHallConfirmRoute50KG route;std::uint8_t screen=0;std::uint32_t pc=0;
  const bool request=confirmDecision(view,route,screen,pc);
  p.confirmRequested=true;p.confirmRoute=route;
  auto& c=p.confirm;c.action=OriginalUiRouteAction50KG::unavailable;c.inputPC=0x1E3F70;c.profileProposalComplete=true;c.profileChanges.count=0;
  if(request){c.request={OriginalUiRouteKind50KG::changeScreen,11,1,screen,1,pc};
   c.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);}
  else if(route==OriginalUiHallConfirmRoute50KG::ending){
   // Not a page screen request: native end scene (17DE10 0x28,3,0) absent.
   c.request={OriginalUiRouteKind50KG::none,11,1,3,0,0x1E49A4};
   c.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);
   out=p;return S::unavailable;}
  else{ // assertion: 133660 disabled -> only 1E41CC sound1 + flush.
   c.action=OriginalUiRouteAction50KG::none;p.soundRequested=p.flushRequested=true;p.soundId=1;p.soundPC=0x1E41E4;p.flushPC=0x1E41FC;out=p;return S::ok;}
  p.soundRequested=p.flushRequested=true;p.soundId=1;p.soundPC=0x1E41E4;p.flushPC=0x1E41FC;out=p;return S::unavailable;}
 if(e==6&&(s.inputFlags42C&2)&&view.mode34==5){const auto slot=s.navigation.groups[1].selectedSlot;if(!slot||slot>13)return S::invalidInput;p.backRequested=true;p.back.action=OriginalUiRouteAction50KG::unavailable;p.back.request={OriginalUiRouteKind50KG::changeScreen,11,1,12,1,0x1E4260};p.back.inputPC=0x1E3F70;p.back.selectedSlot=slot;p.back.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);p.back.profileProposalComplete=true;p.back.profileChanges.count=1;p.back.profileChanges.values[0]={OriginalUiProfileField50KG::hallSlot6EC,OriginalUiProfileWrite50KG::word,slot,0xFFFFFFFFu,0x1E4254};p.soundRequested=p.flushRequested=true;p.soundId=5;p.soundPC=0x1E4280;p.flushPC=0x1E4298;out=p;return S::unavailable;}
 if(e!=0&&e!=1){out=p;return S::noChange;}
 OriginalUiCommands50KG commands;if(moveOriginalUiControl50KG(p.state.navigation,1,e==1,commands)!=OriginalUiNavigationStatus50KG::ok||commands.count!=2)return S::navigationFailure;
 const auto& old=commands.values[0];const auto& next=commands.values[1];
 if(!control(p,old.controlId,old.operation,old.value,old.sourcePC))return S::capacity;
 p.pauseVideo=true;p.pauseAfterCallbackCount=p.callbackCount;
 // With a nonnull output control,2A517C/2A5234 defers the new select until
 // AFTER video25B100(true); exactly one callback, never duplicate selection.
 if(!control(p,next.controlId,O::select,false,e==1?0x1E4388:0x1E4428))return S::capacity;
 std::uint32_t course=0;if(!selectedCourse(p.state,course,p.planet))return S::invalidInput;p.writePlanet=true;p.planetPC=e==1?0x1E43D0:0x1E4470;p.state.deferred1460=3;
 p.soundRequested=p.flushRequested=true;p.soundId=e==1?4:3;p.soundPC=e==1?0x1E4310:0x1E42C8;p.flushPC=e==1?0x1E4328:0x1E42E0;out=p;return S::ok;
}
S OriginalUiHallOfFameController50KG::update(const State& s,float dt,P& out)const noexcept {if(!storage_)return S::notBound;if(!valid(s,page())||!std::isfinite(dt)||dt<0)return S::invalidInput;P p;p.state=s;p.saveUpdateDelta=true;p.updateDelta=dt;if(s.deferred1460!=0){if(s.deferred1460<=1){p.resumeVideo=true;p.drainPreviousFrame=true;p.setVideoEffect250=true;}p.state.deferred1460=s.deferred1460-1;}out=p;return S::ok;}
bool originalUiHallConfirmShape50KG(const OriginalUiRouteDecision50KG& d) noexcept {
 if(d.request.kind!=OriginalUiRouteKind50KG::changeScreen||d.request.sourceScreen!=11||d.request.sourcePage!=1||d.request.targetPage!=1||
    d.inputPC!=0x1E3F70||!d.profileProposalComplete||d.profileChanges.count!=0)return false;
 switch(d.request.targetScreen){case 7:return d.request.sourcePC==0x1E4048||d.request.sourcePC==0x1E40EC;case 10:return d.request.sourcePC==0x1E418C;
 case 13:return d.request.sourcePC==0x1E41AC;case 6:return d.request.sourcePC==0x1E41C4;default:return false;}}
bool originalUiHallBackShape50KG(const OriginalUiRouteDecision50KG& d) noexcept {if(d.request.kind!=OriginalUiRouteKind50KG::changeScreen||d.request.sourceScreen!=11||d.request.sourcePage!=1||d.request.targetScreen!=12||d.request.targetPage!=1||d.request.sourcePC!=0x1E4260||d.inputPC!=0x1E3F70||d.selectedSlot<1||d.selectedSlot>13||!d.profileProposalComplete||d.profileChanges.count!=1)return false;const auto& w=d.profileChanges.values[0];return w.field==OriginalUiProfileField50KG::hallSlot6EC&&w.operation==OriginalUiProfileWrite50KG::word&&w.value==d.selectedSlot&&w.replaceMask==0xFFFFFFFFu&&w.sourcePC==0x1E4254;}
}

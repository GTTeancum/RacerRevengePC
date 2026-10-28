#include "original_ui_garage_session50kg.h"
#include "original_ui_tournament_session50kg.h"
#include "original_ui_main_draw50kg.h"
#include "vehicle_window_gl50ds.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
namespace rr::platform { namespace {
using S=OriginalUiTournamentSessionStatus50KG;using D=OriginalUiTournamentSessionDiagnostic50KG;
using O=game::OriginalUiControlOperation50KG;using K=OriginalUiTournamentWidgetKind50KG;
using N=game::OriginalUiTransitionNotificationKind50KG;using Life=game::OriginalUiTransitionLifecycle50KG;
using Phase=game::OriginalUiTransitionPhase50KG;using Completion=game::OriginalUiTransitionCompletion50KG;
using H=game::OriginalUiApertureHookKind50KG;using AS=game::OriginalUiApertureStatus50KG;
using P=game::OriginalUiTournamentCleanPhase50KG;using E=game::OriginalUiTournamentCleanEffect50KG;
using C=game::OriginalUiOptionsMenuCallback50KG;
S result(D& d,S s,std::uint32_t pc=0)noexcept{d.status=s;d.sourcePC=pc;return s;}
bool okay(AS s)noexcept{return s==AS::ok||s==AS::noChange;}
bool stamp(OriginalUiRiderProfileStamp50KG a,OriginalUiRiderProfileStamp50KG b)noexcept{return a.generation==b.generation&&a.mode34==b.mode34;}
bool common(const OriginalUiMainProfile50KG&a,const OriginalUiMainProfile50KG&b)noexcept{return a.known==b.known&&a.mode34==b.mode34&&a.previousMode38==b.previousMode38&&a.savedSlot6E0==b.savedSlot6E0&&a.availability11018==b.availability11018&&a.flags11054==b.flags11054&&a.playerBits1125C==b.playerBits1125C;}
bool decision(const game::OriginalUiRouteDecision50KG&a,const game::OriginalUiRouteDecision50KG&b)noexcept{
 if(a.action!=b.action||a.blockers!=b.blockers||a.admitted!=b.admitted||a.profileProposalComplete!=b.profileProposalComplete||a.selectedSlot!=b.selectedSlot||a.selectedRider!=b.selectedRider||a.inputPC!=b.inputPC||a.profileChanges.count!=b.profileChanges.count||a.profileChanges.count>a.profileChanges.values.size())return false;
 const auto&x=a.request;const auto&y=b.request;if(x.kind!=y.kind||x.sourceScreen!=y.sourceScreen||x.sourcePage!=y.sourcePage||x.targetScreen!=y.targetScreen||x.targetPage!=y.targetPage||x.sourcePC!=y.sourcePC)return false;
 for(std::size_t i=0;i<a.profileChanges.count;++i){const auto&x=a.profileChanges.values[i];const auto&y=b.profileChanges.values[i];if(x.field!=y.field||x.operation!=y.operation||x.value!=y.value||x.replaceMask!=y.replaceMask||x.sourcePC!=y.sourcePC)return false;}return true;
}
bool genuine(const std::shared_ptr<const assets::OriginalUiArchive50KG>&a,assets::OriginalUiArchiveId50KG id)noexcept{return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);}
bool viewableCareer(const OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot&p)noexcept{
 return (!p.cardFlag100BC||p.nativeBetaStorage)&&(p.progress219C&31u)<=13&&std::all_of(p.medals1938.begin(),p.medals1938.end(),[](auto x){return x<=8;});
}
void controlState(game::OriginalUiControlState50KG&s,O op,bool value)noexcept{
 switch(op){case O::show:s.shown=true;break;case O::hide:s.shown=false;break;case O::select:s.selected=true;break;case O::deselect:case O::clearSelection:s.selected=false;break;case O::setEnabled:s.enabled=value;break;case O::reset:break;}
}
}
struct OriginalUiTournamentSession50KG::Storage {
 // Collector is destroyed first. All descriptors, models, clocks and immutable
 // banks stay retained through real renderer and material retirement.
 OriginalUiTournamentSessionCold50KG cold;
 OriginalUiMainProfile50KG coldCommon{};OriginalUiRiderProfileStamp50KG coldStamp{},liveStamp{},pendingStamp{};
 OriginalUiRiderProfileOwner50KG::TournamentTextMetadataSnapshot textMetadata{};
 OriginalUiRiderProfileColors50KG colors{};
 std::shared_ptr<OriginalUiTournamentWidgets50KG> widgets;
 std::shared_ptr<game::OriginalUiTournamentPodOwner50KG> pod;
 std::shared_ptr<game::OriginalUiTournamentCameraOwner50KG> camera,selectedCamera;
 game::OriginalUiApertureOwner50KG aperture;
 game::OriginalUiNavigation50KG nav{};
 // The real popup owns independent cloned ScreenTexts and all seven texture
 // objects in the retained resource population. None is an Entity child.
 struct Popup438 {
  std::shared_ptr<OriginalUiTournamentWidgetResources50KG> resources;
  std::uint8_t entity29=0; //141300 followed by actual constructor disable141520.
  std::uint8_t initialized34=3; //2037D0 clone initialization flags, not visibility.
  bool constructed()const noexcept{return resources&&resources->popupClones438()&&resources->popupTextures438();}
  bool disable141520()noexcept{if(!constructed())return false;entity29=std::uint8_t(entity29&~3u);return true;} //141460's actual child list is empty.
 }popup;
 OriginalUiTournamentSessionSnapshot50KG state{};D failure{};
 OriginalUiMainSessionUpdate50KG prepared{};
 game::OriginalUiRouteDecision50KG pending{};
 std::uint64_t serial=0,lightGeneration=0;
 std::uint8_t pendingInput=0,pendingSound=0;
 std::uint32_t packedBackground430=0;bool cameraShown=false;
 bool entryCommitted=false,populated=false,requested=false,entered=false,managerEntered=false,entryAuxiliary=false,entryCompleted=false;
 bool opening=false,opened=false,closing=false,threshold=false,closed=false,backCommitted=false,backAccepted=false;
 bool inOperation=false,rendering=false,updatePrepared=false,paused=false,bankPrepared=false,bankPublished=false;
 bool sourceClosing=false,sourceTornDown=false;
 OriginalUiTournamentDrawContribution50KG collector;
 bool muted()const noexcept{return cold.audioMode==OriginalUiMainAudioMode50KG::muted;}
 // These Course-named methods own the SAME authentic shared Song11/effects,
 // not a Course session or a substitute Tournament music stream.
 bool audioReady()const noexcept{return muted()?!cold.audio:cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&cold.audio&&cold.audio->courseMenuReady();}
 bool effectReady(std::uint8_t sound)const noexcept{return (sound==1||sound==5)&&audioReady()&&(muted()||cold.audio->canPlayCourseEffect(sound));}
 bool effect(std::uint8_t sound)noexcept{if(muted()){++state.audio.suppressedByMutedMode;return true;}if(!cold.audio->playCourseEffect(sound)){++state.audio.failed;return false;}++state.audio.submitted;return true;}
 bool process()const noexcept{return cold.application&&cold.application->known&&cold.commonProfile&&cold.commonProfile->known&&cold.profile&&cold.profile->commonProfile()==cold.commonProfile.get()&&!(cold.application->globalFlags228&4u)&&cold.application->transportE00==-1&&audioReady();}
 bool operational()const noexcept{return state.bound&&!state.faulted&&!state.quitRequested&&!sourceClosing&&widgets&&!widgets->poisoned();}
 auto transition()const noexcept{return cold.externalTransition->snapshot();}
 bool currentPage()const noexcept{const auto t=transition();return t.bound&&t.current.screen==9&&t.current.page==1;}
 bool entryPreparationCurrent()const noexcept{
  if(const auto* p=std::get_if<OriginalUiRiderRouteProposal50KG>(&cold.sourceProposal))return cold.resources->riderConfirmPreparationCurrent(*p);
  if(const auto* p=std::get_if<OriginalUiCourseRouteProposal50KG>(&cold.sourceProposal))return cold.resources->courseBackPreparationCurrent(*p);
  if(const auto* p=std::get_if<std::shared_ptr<const OriginalUiGarageSession50KG>>(&cold.sourceProposal))return cold.resources->garageBackPreparationCurrent(*p);
  return false;
 }
 bool profileCurrent()const noexcept{
  OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot p;
  return process()&&entryCommitted&&common(coldCommon,*cold.commonProfile)&&stamp(cold.profile->stamp(),liveStamp)&&cold.resources->current()&&cold.profile->tournamentCareerSnapshot(state.career,p)&&viewableCareer(p)&&stamp(p.stamp,liveStamp);
 }
 bool lightCurrent()const noexcept{return cold.lightManager&&camera&&camera->authoredLighting()&&lightGeneration&&cold.lightManager->managerGeneration()==lightGeneration&&cold.lightManager->activeUiIdentity()==cold.tournamentArchive->identity()&&cold.lightManager->matchesTournamentRegistry(*camera->authoredLighting());}
 bool popupCurrent()const noexcept{
  if(!popup.constructed()||(popup.entity29&3u))return false;
  for(auto id:nav.page->popupWidgets)if(id<1||id>=64||widgets->state(std::uint16_t(id)).shown)return false;
  return true;
 }
 bool idleInput()const noexcept{const auto t=transition();const auto a=aperture.snapshot();return operational()&&currentPage()&&profileCurrent()&&lightCurrent()&&popupCurrent()&&entryCompleted&&opened&&!closing&&!backAccepted&&state.pageShown&&state.inputContainerShown&&state.framePublished&&t.requested.screen==9&&t.requested.page==1&&t.lifecycle==Life::idle&&t.phase==Phase::none&&!t.initialRestartPending&&a.inputEnabled268&&a.state==game::OriginalUiApertureState50KG::open;}
 void fault(S status,std::uint32_t pc)noexcept{if(!state.faulted){failure.status=status;failure.sourcePC=pc;}state.faulted=true;state.framePublished=false;}
 void cpuDrain()noexcept{collector.drain();state.framePublished=false;}
 static bool podPreflight(void*context,const C*steps,std::size_t count)noexcept{
  auto&s=*static_cast<Storage*>(context);if(!s.profileCurrent()||!s.bankPublished||!s.pod||!steps)return false;
  for(std::size_t i=0;i<count;++i)if(s.widgets->state(steps[i].widgetId).kind==K::userRotate){
   if(steps[i].widgetId!=s.pod->widgetId()||steps[i].operation!=O::select||s.pod->attached()||s.populated)return false;
   const auto*desc=s.cold.widgets->pod(steps[i].widgetId);if(!desc||!desc->enableOnSelect||desc->disableOnSelect)return false;
  }return true;
 }
 static bool podCallback(void*context,const C&step,OriginalUiTournamentWidgetState50KG&state)noexcept{
  auto&s=*static_cast<Storage*>(context);if(step.widgetId!=s.pod->widgetId()||step.operation!=O::select||s.pod->attached())return false;
  const auto*desc=s.cold.widgets->pod(step.widgetId);if(!desc)return false;
  //1D33D0 generic2A78C0 precedes load1D34D0. Its shown state is
  // authenticated from this selected widget, not a false initial-state guess.
  state.selected=true;if(desc->enableOnSelect!=desc->disableOnSelect)state.shown=desc->enableOnSelect;
  if(!state.shown||!s.pod->attachSelected(s.failure.model))return false;
  //1D34E0 repeats genericSELECT,1D34E8 unpauses,1D34F4 explicitly shows.
  state.selected=true;state.shown=s.pod->shown();return state.shown;
 }
 OriginalUiTournamentWidgetHooks50KG hooks()noexcept{return {this,&podPreflight,&podCallback};}
 bool group(std::size_t id,O op,bool value,std::uint32_t memberPC,std::uint32_t associationPC)noexcept{
  if(!nav.page||id>=nav.page->groups.size())return false;
  std::array<C,256> callbacks{};std::size_t count=0;auto next=nav;
  const auto control=[&](std::int16_t c,std::uint32_t pc){if(c==-1)return true;if(c<0||std::size_t(c)>=nav.page->controls.size())return false;
   for(auto widget:nav.page->controls[std::size_t(c)].widgets){if(count==callbacks.size())return false;callbacks[count++]={widget,op,value,pc};}controlState(next.controls[std::size_t(c)],op,value);return true;};
  for(auto c:nav.page->groups[id].controls)if(!control(c,memberPC))return false;
  for(auto c:nav.page->groups[id].associations)if(!control(c,associationPC))return false;
  if(!widgets->apply(callbacks.data(),count,hooks(),failure.widget))return false;
  if(op==O::setEnabled)next.groups[id].enabled=value;nav=next;return true;
 }
 bool auxiliary(bool enable)noexcept{
  if(!populated||!widgets->dynamicComplete())return false;
  if(enable)nav.controllerGroup={{-1,-1}};
  for(std::size_t id=0;id<nav.page->groups.size();++id)if(!group(id,enable?O::reset:O::hide,false,enable?0x2A57D8:0x2A4D58,enable?0x2A5818:0x2A4D8C))return false;
  state.pageShown=enable;return true;
 }
 bool populate()noexcept{
  if(populated||!profileCurrent()||!popupCurrent()||!lightCurrent()||pod->widgetId()<39||pod->widgetId()>61)return false;
  const C selected{pod->widgetId(),O::select,false,std::uint32_t(0x1EECCC+(pod->widgetId()-39)*0x24)};
  if(!widgets->apply(&selected,1,hooks(),failure.widget))return false;
  // Exact1EEC40 order. All allocations occurred cold; these publications are
  // the real bounded SetText boundaries, not preparation effects.
  for(auto id:{37u,38u,11u,9u,10u})if(!widgets->publishDynamic(std::uint16_t(id),failure.widget))return false;
  for(auto id:{27u,28u,29u,30u,7u,38u,9u,10u,31u,32u,33u,34u,35u,36u})if(!widgets->setColor(std::uint16_t(id),colors.accent17FA00,failure.widget))return false;
  for(auto id:{1u,2u,3u})if(!widgets->setColor(std::uint16_t(id),colors.secondary17FA10,failure.widget))return false;
  if(!widgets->setColor(11,0xCCCCCC,failure.widget))return false;populated=true;return true;
 }
 bool phase(P phase)noexcept{
  OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot profile;game::OriginalUiTournamentCleanPlan50KG plan;
  if(!profileCurrent()||!cold.profile->tournamentCareerSnapshot(state.career,profile)||!game::planOriginalUiTournamentCleanPhase50KG(phase,profile.cardFlag100BC&&!profile.nativeBetaStorage,bool(camera),plan))return false;
  for(std::size_t i=0;i<plan.count;++i){const auto&step=plan.steps[i];
   if(step.kind==E::startSong11&&(!audioReady()||(!muted()&&!cold.audio->canStartCourseLoop11())))return false;
   if(step.kind==E::fadeMusic1&&(!audioReady()||(!muted()&&!cold.audio->canFadeCourseMusic())))return false;
  }
  for(std::size_t i=0;i<plan.count;++i){const auto&step=plan.steps[i];switch(step.kind){
   case E::promptMask:state.promptMask42C=step.value;break;
   case E::groupEnabled:if(!group(step.argument,O::setEnabled,step.value!=0,0x2A5044,0x2A5088))return false;break;
   case E::widgetEnabled:{const C c{std::uint16_t(step.argument),O::setEnabled,step.value!=0,step.sourcePC};if(!widgets->apply(&c,1,hooks(),failure.widget))return false;break;}
   case E::hidePopup438:if(!popup.disable141520()||!popupCurrent())return false;break;
   case E::resetExit4060:if(!okay(aperture.resetTournamentExitRequestedAtEntry74()))return false;break;
   case E::resetTimer4064:state.timer4064=0;break;
   case E::showCamera4BC:cameraShown=true;break;
   case E::activateCamera4BC:if(!cameraShown||!camera||!lightCurrent())return false;selectedCamera=camera;state.cameraActive=true;break;
   case E::resetSharedPopupResult3D8:cold.application->uiObject3D8=0;break;
   case E::applyBackground430:{std::uint32_t packed=0;for(unsigned j=0;j<3;++j){std::int32_t c=0;if(!game::convertOriginalUiApertureCvtWS50KG(nav.page->backgroundColor[j],c)||c<0||c>255)return false;packed|=std::uint32_t(c)<<(16-8*j);}packedBackground430=packed;break;}
   case E::startSong11:case E::fadeMusic1:if(muted())++state.audio.suppressedByMutedMode;else{if(!(step.kind==E::startSong11?cold.audio->startCourseLoop11():cold.audio->fadeCourseMusic())){++state.audio.failed;return false;}++state.audio.submitted;}break;
  }}return true;
 }
 static bool available(void*context)noexcept{auto&s=*static_cast<Storage*>(context);return !s.state.faulted&&!s.sourceClosing&&s.process()&&s.popupCurrent()&&s.cold.widgets->exitText()&&s.cold.widgets->exitText()->prepared()&&s.camera&&s.pod;}
 static void hook(void*context,const game::OriginalUiApertureHook50KG&e)noexcept{
  auto&s=*static_cast<Storage*>(context);if(!s.operational())return;
  switch(e.kind){
   case H::openingAC:if(e.targetPC!=0x1EEB50||!s.entered||s.opening||!s.phase(P::openingAC))s.fault(S::aperture,e.callPC);else s.opening=true;break;
   case H::openedB0:if(e.targetPC!=0x1EEBA0||!s.opening||s.opened||!s.phase(P::openedB0))s.fault(S::aperture,e.callPC);else s.opened=true;break;
   case H::closingB4:if(e.targetPC!=0x1EEBE0||!s.backAccepted||s.closing||!s.phase(P::closingB4))s.fault(S::aperture,e.callPC);else s.closing=true;break;
   case H::closingThresholdB8:if(e.targetPC!=0x1EEC30||!s.closing||s.threshold||!s.phase(P::closedB8))s.fault(S::aperture,e.callPC);else s.threshold=true;break;
   case H::closedBC:if(e.targetPC!=0x1D7A00||!s.closing||!s.threshold||s.closed)s.fault(S::aperture,e.callPC);else s.closed=true;break;
   case H::screenExitObject440:if(!s.rendering||!s.closing||!s.collector.projectExitText(e,s.failure.draw))s.fault(S::draw,e.callPC);break;
  }
 }
 static void overlay(void*context,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG&r)noexcept{auto&s=*static_cast<Storage*>(context);if(!s.operational()||!s.rendering||!s.collector.projectAperture(ordinal,r,s.failure.draw))s.fault(S::draw,0x1D798C);}
 bool updateWidgets(bool top)noexcept{
  const auto*order=widgets->order(top);if(!order)return false;
  for(auto id:*order){const auto state=widgets->state(id);if(state.kind==K::text){if(!widgets->updateText(id,true,failure.widget))return false;}
   else if(state.kind==K::userRotate&&state.shown){if(id!=pod->widgetId()||!pod->shown()||!pod->advance(prepared.elapsedSeconds,this->state.updateCount+1,failure.model))return false;}
  }return true;
 }
};
OriginalUiTournamentSession50KG::OriginalUiTournamentSession50KG()noexcept=default;
OriginalUiTournamentSession50KG::~OriginalUiTournamentSession50KG(){
 if(storage_&&!storage_->sourceTornDown){D d;if(teardownSource(nullptr,d)!=S::ok)(void)storage_.release();}
}
S OriginalUiTournamentSession50KG::bind(const OriginalUiTournamentSessionCold50KG&cold,D&d)noexcept{
 d={};if(storage_)return result(d,S::alreadyBound);
 if(!cold.resources||!cold.resources->loaded()||!cold.resources->current()||!cold.widgets||!cold.widgets->loaded()||!cold.widgets->page()||!cold.widgets->pageData()||!cold.widgets->exitText()||!cold.widgets->exitText()->prepared()||!cold.english||!cold.english->loaded()||!cold.profile||!cold.profile->initialized()||!cold.commonProfile||!cold.commonProfile->known||cold.commonProfile->mode34!=2||!cold.application||!cold.application->known||!cold.lightManager||!cold.externalTransition||cold.externalTransition->snapshot().bound||!genuine(cold.tournamentArchive,assets::OriginalUiArchiveId50KG::tournament)||!genuine(cold.appStart,assets::OriginalUiArchiveId50KG::appStart)||!cold.podTextures)return result(d,S::resource);
 bool sourceCurrent=false;
 if(const auto* p=std::get_if<OriginalUiRiderRouteProposal50KG>(&cold.sourceProposal))
  sourceCurrent=p->source&&p->source->ownsProfile(*cold.profile)&&p->source->routeProposalCurrent(*p)&&cold.resources->riderConfirmPreparationCurrent(*p);
 else if(const auto* p=std::get_if<OriginalUiCourseRouteProposal50KG>(&cold.sourceProposal))
  sourceCurrent=p->source&&p->source->ownsProfile(*cold.profile)&&p->source->routeProposalCurrent(*p)&&cold.resources->courseBackPreparationCurrent(*p);
 else if(const auto* p=std::get_if<std::shared_ptr<const OriginalUiGarageSession50KG>>(&cold.sourceProposal))
  sourceCurrent=*p&&(*p)->profileOwnerForReturn17DE10()==cold.profile&&cold.resources->garageBackPreparationCurrent(*p);
 if(!sourceCurrent||!cold.resources->sameProfileOwner(*cold.profile))return result(d,S::profile);
 try{auto s=std::make_unique<Storage>();s->cold=cold;s->cold.english=nullptr;s->coldCommon=*cold.commonProfile;s->coldStamp=cold.profile->stamp();
  if(!s->process()||!cold.resources->career()||!viewableCareer(*cold.resources->career())||!stamp(cold.resources->career()->stamp,s->coldStamp))return result(d,S::profile);
  std::uint32_t pause=0;if(!cold.applicationAnimation.readPause11C(pause))return result(d,S::unsupportedState,0x140C50);
  s->state.career=cold.resources->career()->career100C0;
  if(!cold.resources->pod()||!cold.resources->pod()->loaded()||!cold.profile->tournamentTextMetadataSnapshot(s->state.career,s->textMetadata)||!stamp(s->textMetadata.stamp,s->coldStamp)||s->textMetadata.progressIndex>13||!cold.profile->colors(s->state.career,s->colors)||!game::bindOriginalUiNavigation50KG(*cold.widgets->page(),s->nav))return result(d,S::profile);
  if(s->nav.page->groups.size()!=3||s->nav.page->controls.size()!=3||!s->nav.page->hasPopup)return result(d,S::resource);
  s->widgets=std::make_shared<OriginalUiTournamentWidgets50KG>();if(!s->widgets->bind(cold.widgets,d.widget))return result(d,S::widget);
  const auto localized=[&](std::int32_t id)->const std::string*{if(id<0)return nullptr;const auto*e=cold.english->at(std::size_t(id));return e?&e->text:nullptr;};
  const auto*name=localized(s->textMetadata.racerName3C);const auto*money=localized(321);const auto*earned=localized(323);const auto*kills=localized(324);
  if(!name||!money||!earned||!kills)return result(d,S::resource,0x1EF0F4);
  std::string courseLine;
  if(s->textMetadata.progressIndex==13){
   // Original1EF254 ->1EF4B0..1EF5E0: localization325 and326,
   // joined by the original3AF218 "%s %s" format, not a 14th course.
   const auto*completed=localized(325);const auto*congratulations=localized(326);
   if(!completed||!congratulations)return result(d,S::resource,0x1EF5DC);
   courseLine=*completed+" "+*congratulations;
  }else{
   const auto*next=localized(322);const auto*course=localized(s->textMetadata.courseTitle1127C);const auto*planet=localized(s->textMetadata.planetTitle11281);
   if(!next||!course||!planet)return result(d,S::resource,0x1EF49C);
   courseLine=*next+" "+*course+", "+*planet;
  }
  const std::array<std::uint16_t,5> ids{{37,38,11,9,10}};const std::array<std::string,5> values{{*name,*money,courseLine,*earned,*kills}};
  for(std::size_t i=0;i<ids.size();++i)if(!s->widgets->prepareDynamic(ids[i],{values[i]},d.widget))return result(d,S::widget);
  s->popup.resources=cold.widgets;if(!s->popup.disable141520()||!s->popupCurrent())return result(d,S::widget,0x1D66AC);
  s->camera=std::make_shared<game::OriginalUiTournamentCameraOwner50KG>();if(!s->camera->bind(cold.tournamentArchive,d.camera))return result(d,S::camera);
  s->pod=std::make_shared<game::OriginalUiTournamentPodOwner50KG>();game::OriginalUiTournamentPodInput50KG podInput;
  podInput.pod=std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG>(cold.resources,cold.resources->pod());podInput.appStart=cold.appStart;podInput.podTextures=cold.podTextures;podInput.runtime=cold.materialRuntime;podInput.ipumCache=cold.ipumCache.get();
  if(!OriginalUiTournamentDrawContribution50KG::staticRows(*cold.widgets,*cold.resources,podInput.currentStaticRows,d.draw)||!s->pod->prepareCold(podInput,d.model))return result(d,S::model);
  game::OriginalUiApertureCallbacks50KG callbacks{s.get(),&Storage::available,&Storage::hook,&Storage::overlay};
  if(s->aperture.bindTournament(cold.tournamentArchive,callbacks,d.aperture)!=AS::ok)return result(d,S::aperture);
  if(!s->collector.bind(cold.widgets,cold.resources,s->widgets,s->pod,s->camera,cold.externalTransition,&s->aperture,d.draw))return result(d,S::draw);
  if(!s->entryPreparationCurrent())return result(d,S::profile);
  s->state.bound=true;storage_=std::move(s);return result(d,S::ok);
 }catch(...){return result(d,S::resource);}
}
S OriginalUiTournamentSession50KG::prepareBank(D&d)noexcept{d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational()||s->inOperation||s->entryCommitted||s->bankPrepared||!s->entryPreparationCurrent())return result(d,S::unsupportedState);if(!s->collector.prepareBank(d.draw))return result(d,S::draw);s->bankPrepared=true;return result(d,S::ok);}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiTournamentSession50KG::bank()const noexcept{return storage_&&storage_->operational()?storage_->collector.bank():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiTournamentSession50KG::samplers()const noexcept{return storage_&&storage_->operational()?storage_->collector.samplers():nullptr;}
S OriginalUiTournamentSession50KG::bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB>gpu,D&d)noexcept{d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational()||s->inOperation||s->entryCommitted||!s->bankPrepared||s->bankPublished||!s->collector.bindPublishedBank(std::move(gpu),d.draw))return result(d,S::draw);s->bankPublished=true;return result(d,S::ok);}
bool OriginalUiTournamentSession50KG::acceptEntryProfileCommit(const OriginalUiRiderRouteProposal50KG&p)noexcept{
 auto*s=storage_.get();if(!s||!s->operational()||s->inOperation||s->entryCommitted||!s->bankPublished||!s->process()||!common(s->coldCommon,*s->cold.commonProfile))return false;
 if(!s->cold.resources->acceptRiderConfirmCommit(p,s->failure.resource)){s->fault(S::profile,0x1EA5E4);return false;}
 s->liveStamp=s->cold.profile->stamp();s->entryCommitted=true;s->cold.sourceProposal=std::monostate{};return s->profileCurrent();
}
bool OriginalUiTournamentSession50KG::acceptEntryProfileCommit(const OriginalUiCourseRouteProposal50KG&p)noexcept{
 auto*s=storage_.get();if(!s||!s->operational()||s->inOperation||s->entryCommitted||!s->bankPublished||!s->process()||!common(s->coldCommon,*s->cold.commonProfile)||
  !std::holds_alternative<OriginalUiCourseRouteProposal50KG>(s->cold.sourceProposal))return false;
 if(!s->cold.resources->acceptCourseBackCommit(p,s->failure.resource)){s->fault(S::profile,0x1DF0B4);return false;}
 s->liveStamp=s->cold.profile->stamp();s->entryCommitted=true;s->cold.sourceProposal=std::monostate{};return s->profileCurrent();
}
bool OriginalUiTournamentSession50KG::acceptEntryProfileCommit(const std::shared_ptr<const OriginalUiGarageSession50KG>& p)noexcept{
 auto*s=storage_.get();if(!s||!p||!s->operational()||s->inOperation||s->entryCommitted||!s->bankPublished||!s->process()||
  !common(s->coldCommon,*s->cold.commonProfile)||!std::holds_alternative<std::shared_ptr<const OriginalUiGarageSession50KG>>(s->cold.sourceProposal)||
  std::get<std::shared_ptr<const OriginalUiGarageSession50KG>>(s->cold.sourceProposal)!=p)return false;
 if(!s->cold.resources->acceptGarageBackCommit(p,s->failure.resource)){s->fault(S::profile,0x17DE10);return false;}
 s->liveStamp=s->cold.profile->stamp();s->entryCommitted=true;s->cold.sourceProposal=std::monostate{};return s->profileCurrent();
}
S OriginalUiTournamentSession50KG::notifyTransition(const game::OriginalUiTransitionNotification50KG&e,D&d)noexcept{
 d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational()||s->inOperation||s->rendering||!s->currentPage()||!s->profileCurrent()||e.route.screen!=9||e.route.page!=1)return result(d,S::unsupportedState,e.sourcePC);
 // Frontend preserves audio -> manager2A3C80 -> children141ED0. A prepared
 // update is therefore the required tick-notification window, not reentry.
 // Initial2A3C10 notifications instead run before any update is prepared.
 // Keep both windows distinct; active callback/render execution stays barred.
 bool updateNotice=false;
 switch(e.kind){
  case N::requestedPage90:case N::pageEntry74:break;
  case N::managerEntry84:case N::beginTransition7C:case N::pageAuxiliary1C:case N::pageAuxiliary24:case N::previousPage94:updateNotice=true;break;
  default:return result(d,S::transition,e.sourcePC);
 }
 if(s->updatePrepared!=updateNotice)return result(d,S::unsupportedState,e.sourcePC);
 const auto t=s->transition();if(e.phase!=t.phase||e.lifecycle!=t.lifecycle)return result(d,S::transition,e.sourcePC);
 s->cpuDrain();s->state.backProposalPending=false;s->inOperation=true;bool valid=true;
 switch(e.kind){
  case N::requestedPage90:
   valid=!s->requested&&!s->populated&&t.initialRestartPending&&t.lifecycle==Life::idle&&t.phase==Phase::none&&e.sourcePC==0x2A3C44;
   if(valid){const auto*lighting=s->camera->authoredLighting();if(!lighting||s->cold.lightManager->activeUiIdentity()!=s->cold.tournamentArchive->identity()||!s->cold.lightManager->publishTournamentRegistry(*lighting,s->failure.light)){s->fault(S::camera,0x1CDB10);break;}s->lightGeneration=s->cold.lightManager->managerGeneration();
    if(!s->populate()||!s->auxiliary(true)){s->fault(S::widget,0x1EEC40);break;}s->requested=true;}break;
  case N::pageEntry74:valid=s->requested&&!s->entered&&t.initialRestartPending&&t.lifecycle==Life::idle&&t.phase==Phase::none&&e.sourcePC==0x2A3C58;if(valid){if(!s->phase(P::entry74))s->fault(S::widget,0x1EE9E0);else s->entered=true;}break;
  case N::managerEntry84:valid=s->entered&&!s->managerEntered&&t.lifecycle==Life::initialEntry&&t.phase==Phase::none&&e.sourcePC==0x2A3D68;if(valid)s->managerEntered=true;break;
  case N::beginTransition7C:
   if(t.phase==Phase::entry)valid=s->managerEntered&&!s->opening&&t.lifecycle==Life::waitPageTransition&&e.sourcePC==0x2A3D8C;
   else valid=t.phase==Phase::exit&&s->backAccepted&&!s->closing&&t.lifecycle==Life::waitScreenExit&&e.sourcePC==0x2A3DFC;
   if(valid&&!okay(s->aperture.beginTransition7C(t.phase)))s->fault(S::aperture,e.sourcePC);break;
  case N::pageAuxiliary1C:
   if(e.sourcePC==0x2A3DA4)valid=s->managerEntered&&!s->entryAuxiliary&&t.lifecycle==Life::waitPageTransition&&t.phase==Phase::entry;
   else valid=e.sourcePC==0x2A3E8C&&s->closed&&s->backAccepted&&t.lifecycle==Life::waitScreenExit&&t.phase==Phase::exit;
   if(valid){s->state.inputContainerShown=false;if(e.sourcePC==0x2A3DA4)s->entryAuxiliary=true;}break;
  case N::pageAuxiliary24:
   // Tournament poll80 entry completes immediately, BEFORE its first render
   // opens the aperture. Do not import Course/Hall's opened prerequisite.
   valid=s->entryAuxiliary&&!s->entryCompleted&&e.sourcePC==0x2A3EE0&&t.lifecycle==Life::waitPageTransition&&t.phase==Phase::entry&&s->aperture.pollCompletion({9,1},Phase::entry)==Completion::complete;
   if(valid){s->entryCompleted=true;s->state.inputContainerShown=true;}break;
  case N::previousPage94:valid=s->requested&&s->entered;if(valid&&!s->auxiliary(false))s->fault(S::widget,0x2A6810);break;
 }
 if(!valid)s->fault(S::transition,e.sourcePC);s->inOperation=false;if(s->state.faulted){d=s->failure;return d.status;}return result(d,S::ok);
}
Completion OriginalUiTournamentSession50KG::pollCompletion(game::OriginalUiTransitionRoute50KG r,Phase p)const noexcept{
 const auto*s=storage_.get();if(!s||!s->operational()||s->inOperation||!s->currentPage()||!s->profileCurrent()||!s->lightCurrent()||r.screen!=9||r.page!=1||s->transition().phase!=p)return Completion::unavailable;
 if(p==Phase::exit&&(!s->backAccepted||!s->closing))return Completion::unavailable;return s->aperture.pollCompletion(r,p);
}
const game::OriginalUiTransitionOwner50KG* OriginalUiTournamentSession50KG::externalLifecycleOwner()const noexcept{return storage_?storage_->cold.externalTransition.get():nullptr;}
bool OriginalUiTournamentSession50KG::ownsPreparedPage(std::uint8_t page)const noexcept{const auto*s=storage_.get();return s&&s->operational()&&!s->inOperation&&s->bankPublished&&page==1&&s->popupCurrent()&&s->pod->podAssets()&&s->camera->archive();}
S OriginalUiTournamentSession50KG::input(const OriginalUiMainSessionInput50KG&in,OriginalUiTournamentSessionInputReceipt50KG&r,D&d)noexcept{
 d={};r={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,s->state.quitRequested?S::quit:S::faulted);if(s->inOperation||s->updatePrepared||in.controller>1||in.originalInput>15)return result(d,S::invalidInput);
 // Once accepted, later physical input cannot erase the consumed route
 // whose real retirement will authenticate incoming Course entry.
 s->state.backProposalPending=false;if(!s->backAccepted)s->backCommitted=false;r.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
 if(!r.containerReached||in.popupActive||in.sourceInputBlocked||!s->idleInput())return result(d,r.status=S::noChange);
 r.pageCallbackRan=true;r.pageAccepted=true;
 const auto plan=game::planOriginalUiTournamentInput50KG(in.controller,in.originalInput,s->popup.entity29,std::uint32_t(s->transition().phase));
 if(plan.status!=game::OriginalUiTournamentInputStatus50KG::proposed)return result(d,r.status=S::noChange);
 // All three genuine inputs produce only a typed proposal here. The
 // coordinator must admit the actual destination before its source sound.
 const bool continuing=in.originalInput==4&&plan.request.targetScreen==6&&plan.sound==1;
 const bool back=in.originalInput==6&&plan.request.targetScreen==5&&plan.sound==5;
 const bool garage=in.originalInput==5&&plan.request.targetScreen==13&&plan.sound==1;
 if(!continuing&&!back&&!garage)return result(d,r.status=S::unavailableDestination,plan.requestCallPC);
 if(!s->effectReady(plan.sound)||plan.request.kind!=game::OriginalUiRouteKind50KG::changeScreen)return result(d,r.status=S::audio,plan.soundPC);
 s->pending={};s->pending.action=game::OriginalUiRouteAction50KG::unavailable;s->pending.request=plan.request;s->pending.blockers=std::uint32_t(game::OriginalUiRouteBlocker50KG::destinationOwner);s->pending.selectedRider=std::int32_t(s->state.career);s->pending.inputPC=0x1F2310;s->pending.profileProposalComplete=true;
 s->pendingStamp=s->liveStamp;s->pendingInput=in.originalInput;s->pendingSound=plan.sound;s->state.backProposalPending=true;if(++s->serial==0)++s->serial;r.routeProposal={this,s->serial,s->pending,s->pendingStamp};return result(d,r.status=garage?S::garageProposed:continuing?S::continueProposed:S::backProposed);
}
bool OriginalUiTournamentSession50KG::ownsProfile(const OriginalUiRiderProfileOwner50KG&p)const noexcept{return storage_&&storage_->cold.profile.get()==&p;}
bool OriginalUiTournamentSession50KG::routeProposalCurrent(const OriginalUiTournamentRouteProposal50KG&p)const noexcept{
 const auto*s=storage_.get();return s&&s->idleInput()&&!s->inOperation&&!s->updatePrepared&&!s->backCommitted&&s->state.backProposalPending&&p.source==this&&p.serial&&p.serial==s->serial&&stamp(p.profileStamp,s->pendingStamp)&&stamp(p.profileStamp,s->liveStamp)&&decision(p.decision,s->pending)&&s->effectReady(s->pendingSound);
}
bool OriginalUiTournamentSession50KG::commitRoute(const OriginalUiTournamentRouteProposal50KG&p,const game::OriginalUiRouteDecision50KG&admitted)noexcept{
 if(!routeProposalCurrent(p)||!admitted.admitted||admitted.blockers)return false;auto expected=p.decision;expected.admitted=true;expected.blockers=0;expected.action=game::OriginalUiRouteAction50KG::request;
 const auto&s=*storage_;
 const bool continuing=s.pendingInput==4&&s.pendingSound==1&&admitted.request.targetScreen==6&&admitted.request.sourcePC==0x1F2374;
 const bool back=s.pendingInput==6&&s.pendingSound==5&&admitted.request.targetScreen==5&&admitted.request.sourcePC==0x1F23C8;
 const bool garage=s.pendingInput==5&&s.pendingSound==1&&admitted.request.targetScreen==13&&admitted.request.sourcePC==0x1F241C;
 if((!continuing&&!back&&!garage)||!decision(expected,admitted)||admitted.profileChanges.count||admitted.inputPC!=0x1F2310||
  admitted.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||admitted.request.sourceScreen!=9||admitted.request.sourcePage!=1||admitted.request.targetPage!=1)return false;
 storage_->backCommitted=true;return true; //1F2310 writes no profile fields.
}
S OriginalUiTournamentSession50KG::notifyRouteAccepted(const OriginalUiTournamentRouteProposal50KG&p,D&d)noexcept{
 d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational()||s->inOperation||s->updatePrepared||!s->backCommitted||!s->state.backProposalPending||p.source!=this||p.serial!=s->serial||!decision(p.decision,s->pending)||!stamp(p.profileStamp,s->pendingStamp)||!s->profileCurrent())return result(d,S::invalidInput);
 const auto t=s->transition();if(!s->currentPage()||t.requested.screen!=p.decision.request.targetScreen||t.requested.page!=1||t.lifecycle!=Life::requestScreenExit||t.phase!=Phase::none||t.applicationRequestQueued||t.initialRestartPending)return result(d,S::transition);
 s->state.backProposalPending=false;s->state.framePublished=false;s->backAccepted=true;
 const auto soundPC=s->pendingInput==5?0x1F243Cu:s->pendingInput==4?0x1F2394u:0x1F23E8u;
 if(!s->effect(s->pendingSound)){s->fault(S::audio,soundPC);return result(d,S::audio,soundPC);}return result(d,S::ok); //267BE0 actual transportE00==-1 proven by process().
}
bool OriginalUiTournamentSession50KG::routeProposalRetired(const OriginalUiTournamentRouteProposal50KG&p)const noexcept{
 const auto*s=storage_.get();if(!s||!s->state.bound||s->state.faulted||s->state.quitRequested||!s->sourceTornDown||
  !s->sourceClosing||s->inOperation||s->rendering||!s->backCommitted||!s->backAccepted||!s->collector.retirementComplete()||
  s->collector.sourceTeardownPending()||s->pendingInput!=4||s->pendingSound!=1||p.source!=this||!p.serial||p.serial!=s->serial||
  !decision(p.decision,s->pending)||!stamp(p.profileStamp,s->pendingStamp)||!stamp(p.profileStamp,s->liveStamp)||
  !s->process()||!common(s->coldCommon,*s->cold.commonProfile)||!stamp(s->cold.profile->stamp(),s->liveStamp)||!s->cold.resources->current())return false;
 const auto t=s->transition();const auto&r=p.decision.request;
 return r.kind==game::OriginalUiRouteKind50KG::changeScreen&&r.sourceScreen==9&&r.sourcePage==1&&r.targetScreen==6&&r.targetPage==1&&
  r.sourcePC==0x1F2374&&p.decision.inputPC==0x1F2310&&!p.decision.profileChanges.count&&
  t.current.screen==9&&t.current.page==1&&t.requested.screen==6&&t.requested.page==1&&t.lifecycle==Life::queueScreen&&t.applicationRequestQueued;
}
S OriginalUiTournamentSession50KG::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&in,D&d)noexcept{
 d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
 if(s->inOperation||s->updatePrepared||s->state.updateCount==std::numeric_limits<std::uint64_t>::max()||!s->currentPage()||!s->profileCurrent()||!s->lightCurrent()||!std::isfinite(in.elapsedSeconds)||in.elapsedSeconds<0||in.elapsedSeconds>0.06f||(in.paused&&in.elapsedSeconds!=0))return result(d,S::invalidInput);
 s->cpuDrain();s->state.backProposalPending=false;s->paused=in.paused;
 if(!s->muted()){s->cold.audio->pause(in.paused);if(!s->cold.audio->update(in.elapsedSeconds)){s->fault(S::audio,0x234440);return result(d,S::audio);}}
 s->prepared=in;s->updatePrepared=true;return result(d,S::ok);
}
S OriginalUiTournamentSession50KG::update(const OriginalUiMainSessionUpdate50KG&in,D&d)noexcept{
 d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
 if(s->inOperation||!s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!s->lightCurrent()||in.elapsedSeconds!=s->prepared.elapsedSeconds||in.paused!=s->prepared.paused||in.debugChordHeld!=s->prepared.debugChordHeld)return result(d,S::invalidInput);
 s->updatePrepared=false;s->inOperation=true;bool success=s->updateWidgets(false);
 if(success&&s->state.pageShown){float next=0;success=game::updateOriginalUiTournamentTimer50KG(s->state.timer4064,in.elapsedSeconds,next);if(success){s->state.timer4064=next;success=okay(s->aperture.saveTournamentUpdateDelta38(in.elapsedSeconds));}}
 // Manager2A3FDC/141ED0 visits ordinary children, page38, then on-top
 // children. Page1F2470 stores298 before its own inherited child list.
 // User_Rotate1D38D0 inherits this same source-capped f12/f20;250 is
 // a separate uncapped clock lane and must not replace the dispatch value.
 if(success)success=s->updateWidgets(true);
 s->inOperation=false;if(!success){s->fault(S::widget,0x1F2470);d=s->failure;return d.status;}++s->state.updateCount;return result(d,S::ok);
}
S OriginalUiTournamentSession50KG::render(const OriginalUiMainFrameClock50KG&clock,D&d)noexcept{
 d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
 if(s->inOperation||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!s->lightCurrent()||!s->state.updateCount||!s->state.pageShown||!s->cameraShown||s->selectedCamera!=s->camera||!s->popupCurrent()||(s->paused&&clock.advanceAnimations))return result(d,S::invalidInput);
 s->cpuDrain();s->state.backProposalPending=false;s->inOperation=s->rendering=true;
 game::OriginalApplicationAnimationClockSample50KG actual,forwarded;
 bool success=s->cold.applicationAnimation.sameOwner(clock.applicationAnimation)&&s->cold.applicationAnimation.read(actual)&&clock.applicationAnimation.read(forwarded)&&actual.current230==forwarded.current230&&actual.pause11C==forwarded.pause11C&&actual.pending120==forwarded.pending120&&s->camera->prepareFrame(s->cold.applicationAnimation,s->cold.lightManager,d.camera);
 const auto*camera=success?s->camera->frame():nullptr;
 if(success)success=camera&&s->collector.begin({clock.known,clock.advanceAnimations,actual.current230,s->cold.applicationAnimation},*camera,d.draw);
 if(!success&&camera)s->camera->retireFrame(*camera);
 if(success)success=s->collector.projectOrdinaryWidgets(true,d.draw)&&s->collector.projectCareer(s->popup.entity29,s->state.pageShown,d.draw);
 if(success)success=okay(s->aperture.beginTournamentEntryAtRender48(s->transition().phase,s->popup.entity29))&&!s->state.faulted&&okay(s->aperture.render48(s->transition().lifecycle))&&!s->state.faulted;
 if(success)success=s->collector.projectPrompts(s->state.promptMask42C,d.draw)&&s->collector.projectOnTopWidgets(true,d.draw)&&s->collector.finish(d.draw);
 s->inOperation=s->rendering=false;if(!success){s->collector.drain();if(s->state.faulted){d=s->failure;return d.status;}s->fault(S::draw,0x1F2700);return result(d,S::draw,0x1F2700);}s->state.framePublished=true;++s->state.renderCount;return result(d,S::ok);
}
OriginalUiTournamentSessionSnapshot50KG OriginalUiTournamentSession50KG::snapshot()const noexcept{if(!storage_)return {};auto r=storage_->state;r.transition=storage_->transition();r.aperture=storage_->aperture.snapshot();return r;}
const game::OriginalUiNavigation50KG* OriginalUiTournamentSession50KG::navigation()const noexcept{return storage_?&storage_->nav:nullptr;}
const OriginalUiNativeDraw50KG* OriginalUiTournamentSession50KG::draw()const noexcept{return nullptr;} //No partial/legacy2D path; use complete sealed sourceFrame.
const OriginalUiSourceFrame50KM* OriginalUiTournamentSession50KG::sourceFrame()const noexcept{const auto*s=storage_.get();return s&&s->operational()&&s->state.framePublished&&!s->inOperation&&!s->updatePrepared&&s->currentPage()?s->collector.frame():nullptr;}
std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG> OriginalUiTournamentSession50KG::predecessorInventory(D&d)const noexcept{
 d={};const auto*s=storage_.get();if(!s||!s->operational()||s->inOperation||s->updatePrepared||!s->profileCurrent()){result(d,S::unsupportedState);return {};}
 auto inventory=s->collector.predecessorInventory(d.draw);if(!inventory)result(d,S::draw);return inventory;
}
bool OriginalUiTournamentSession50KG::retirePredecessorInventory(std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG>inventory,game::OriginalUiTournamentPredecessorRetirement50KG&retired,D&d)const noexcept{
 d={};const auto*s=storage_.get();if(!s||!s->sourceTornDown||s->inOperation||s->collector.sourceTeardownPending()){result(d,S::unsupportedState);return false;}
 if(!s->collector.retirePredecessorInventory(std::move(inventory),retired,d.draw)){result(d,S::draw);return false;}return true;
}
bool OriginalUiTournamentSession50KG::sourceTeardownPending()const noexcept{return storage_&&!storage_->sourceTornDown;}
S OriginalUiTournamentSession50KG::teardownSource(VehicleWindowGl50DS*renderer,D&d)noexcept{
 d={};auto*s=storage_.get();if(!s||s->sourceTornDown)return result(d,S::ok);if(s->inOperation||s->rendering)return result(d,S::busy);
 s->sourceClosing=true;s->state.framePublished=false;s->state.backProposalPending=false;s->updatePrepared=false;s->collector.drain();
 if(s->collector.sourceTeardownPending()){if(!renderer||!s->collector.retireSourceRenderer(*renderer,d.draw))return result(d,S::draw);}
 else if(!s->collector.retireUnsubmittedSource(d.draw))return result(d,S::draw);
 if(!s->pod->detachAfterRetirement(d.model))return result(d,S::model);
 if(!s->collector.consumeMaterials(false,d.draw)||!s->pod->finalizeTeardown(s->collector.materialSequence(),d.model))return result(d,S::model);
 if(!s->collector.retireStaticRegistrations(d.draw)||!s->collector.retirementComplete())return result(d,S::draw);
 s->selectedCamera.reset();s->cameraShown=false;s->state.cameraActive=false;s->sourceTornDown=true;return result(d,S::ok);
}
void OriginalUiTournamentSession50KG::requestQuit()noexcept{if(storage_&&!storage_->inOperation){storage_->state.quitRequested=true;storage_->state.framePublished=false;}}
}

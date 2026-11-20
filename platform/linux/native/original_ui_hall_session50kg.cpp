#include "original_ui_hall_session50kg.h"
#include "original_ui_hall_addon50dl.h"
#include "../../../src/assets/dlc_progress50dl.h"
#include "../../../src/assets/dlc_registry50dl.h"
#include "../../../src/game/dlc_course_entries50dl.h"
#include "../../../src/game/dlc_course_records50dl.h"
#include "../../../src/game/original_course_descriptor50kg.h"
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>
namespace rr::platform { namespace {
using S=OriginalUiHallSessionStatus50KG;using D=OriginalUiHallSessionDiagnostic50KG;
using HP=game::OriginalUiHallPhase50KG;using MS=game::OriginalUiOptionsMenuStatus50KG;
using N=game::OriginalUiTransitionNotificationKind50KG;using H=game::OriginalUiApertureHookKind50KG;
using AS=game::OriginalUiApertureStatus50KG;using Life=game::OriginalUiTransitionLifecycle50KG;
S result(D& d,S s,std::uint32_t pc=0)noexcept{d.status=s;d.sourcePC=pc;return s;}
bool okay(AS s)noexcept{return s==AS::ok||s==AS::noChange;}
bool stamp(OriginalUiRiderProfileStamp50KG a,OriginalUiRiderProfileStamp50KG b)noexcept{return a.generation==b.generation&&a.mode34==b.mode34;}
bool common(const OriginalUiMainProfile50KG&a,const OriginalUiMainProfile50KG&b)noexcept{return a.known==b.known&&a.mode34==b.mode34&&a.previousMode38==b.previousMode38&&a.savedSlot6E0==b.savedSlot6E0&&a.availability11018==b.availability11018&&a.flags11054==b.flags11054&&a.playerBits1125C==b.playerBits1125C;}
bool decision(const game::OriginalUiRouteDecision50KG&a,const game::OriginalUiRouteDecision50KG&b)noexcept{
 if(a.action!=b.action||a.blockers!=b.blockers||a.admitted!=b.admitted||a.profileProposalComplete!=b.profileProposalComplete||a.selectedSlot!=b.selectedSlot||a.selectedRider!=b.selectedRider||a.inputPC!=b.inputPC||a.request.kind!=b.request.kind||a.request.sourceScreen!=b.request.sourceScreen||a.request.sourcePage!=b.request.sourcePage||a.request.targetScreen!=b.request.targetScreen||a.request.targetPage!=b.request.targetPage||a.request.sourcePC!=b.request.sourcePC||a.profileChanges.count!=b.profileChanges.count||a.profileChanges.count>a.profileChanges.values.size())return false;
 for(std::size_t i=0;i<a.profileChanges.count;++i){const auto&x=a.profileChanges.values[i];const auto&y=b.profileChanges.values[i];if(x.field!=y.field||x.operation!=y.operation||x.value!=y.value||x.replaceMask!=y.replaceMask||x.sourcePC!=y.sourcePC)return false;}return true;
}
}
struct OriginalUiHallOfFameSession50KG::Storage {
 OriginalUiHallSessionCold50KG cold;OriginalUiHallSessionSnapshot50KG state{};D faultDetail{};
 game::OriginalUiHallOfFameController50KG controller;game::OriginalUiHallState50KG nav;
 game::OriginalUiApertureOwner50KG aperture;OriginalUiHallDrawContribution50KG collector;
 OriginalUiMainProfile50KG coldCommon{},liveCommon{};OriginalUiRiderProfileStamp50KG coldStamp{},liveStamp{},pendingStamp{};
 game::OriginalUiHallPlan50KG pending;OriginalUiMainSessionUpdate50KG prepared{};
 bool entryCommitted=false,backCommitted=false,inOperation=false,rendering=false,updatePrepared=false,paused=false,sourceClosing=false,sourceTornDown=false;
 bool confirmPending=false,confirmCommitted=false;
 std::uint32_t entryMode=5; // 5 Options entry; 2 tournament race return (unchanged P+34).
 std::uint64_t serial=0;
 // ---- PORT EXTENSION, NOT ORIGINAL BEHAVIOUR (DLC add-on course records, 148r).
 // The Options Hall (mode5) and the Free Play race-return Hall (mode0) only; the
 // tournament Hall (mode2) never lists an add-on course (D3). After the last
 // enabled stock course the course arrows continue through the admitted add-on
 // courses in number order, then wrap to the first stock course; Previous runs
 // the block backwards (the Course screen's add-on block, 146r-U). An add-on
 // entry is port state only: the original navigation, widgets, planet write and
 // remembered 6EC stay on the stock control the block follows (the anchor).
 // Only the original wrap (Next from the last enabled slot, Previous from the
 // first, as the unchanged move plan reports it) and moves inside the block are
 // taken over; leaving across the wrap applies the original move from the
 // anchor. Drawn: the add-on's own Hall rows (DLC side file, its hall_defaults
 // until a record is kept), its name in the anchor's course-name text, its
 // planet name in the header, the template course's Hall planet background and
 // colours. The race-return Hall opens on the add-on course just raced.
 struct HallAddon {unsigned number=0;std::string key,name,planetName;std::uint32_t planet=0;};
 std::vector<HallAddon> addons;bool addonCarousel=false,addonAnchorLast=true;std::size_t addonSlot=0;
 OriginalUiHallAddonFrame50DL addonFrame{};bool addonDrawLogged=false;
 const HallAddon* addon()const noexcept{return addonSlot&&addonSlot<=addons.size()?&addons[addonSlot-1]:nullptr;}
 void addonLog(const char* action)const noexcept{const auto* a=addon();
  std::fprintf(stderr,"dlc_hall_addon action=%s mode=%u slot=%zu key=%s number=%u anchor_slot=%u anchor_course=%u\n",action,entryMode,addonSlot,
   a?a->key.c_str():"-",a?a->number:0u,unsigned(nav.navigation.groups[1].selectedSlot),state.courseIndex);}
 // Bind: one entry per admitted add-on course whose rows the side file holds.
 void planAddons()noexcept{
  try{
   if(entryMode==2||!game::dlc50DL::addonCourseCount())return;
   const auto* page=controller.page();if(!page||page->groups.size()<2)return;
   for(std::size_t i=0;i<game::dlc50DL::addonCourseCount();++i){
    const auto* c=game::dlc50DL::addonCourseAt(i);if(!c)continue;
    assets::dlc50DL::ProgressHallView view;
    if(!assets::dlc50DL::progressOwner().courseHall(c->key,view)){
     std::fprintf(stderr,"dlc_hall_addon action=skipped key=%s number=%u reason=\"no DLC records this session\"\n",c->key.c_str(),c->number);continue;}
    // The template stock course's Hall planet (course_screen.stock_template).
    int stock=-1;const auto& descriptors=game::originalFreshCourseDescriptors50KG;
    const auto up=[](char x){return x>='a'&&x<='z'?char(x-'a'+'A'):x;};
    if(c->courseScreenTemplate.size()==2)for(std::size_t k=0;k<descriptors.size();++k)
     if(descriptors[k].first==up(c->courseScreenTemplate[0])&&descriptors[k].second==up(c->courseScreenTemplate[1]))stock=int(k);
    HallAddon e;e.number=c->number;e.key=c->key;e.name=c->name;e.planetName=c->planetName;e.planet=state.planetIndex;
    bool found=false;
    for(std::size_t slot=1;slot<=page->groups[1].controls.size()&&stock>=0;++slot){
     auto probe=nav;probe.navigation.groups[1].selectedSlot=static_cast<std::uint16_t>(slot);
     std::uint32_t course=0,planet=0;
     if(controller.selectedCourse(probe,course,planet)&&course==std::uint32_t(stock)&&
        cold.resources->backgroundImage(planet)!=assets::unresolvedMaterialTexture50EA){e.planet=planet;found=true;break;}
    }
    std::fprintf(stderr,"dlc_hall_addon action=planned mode=%u key=%s number=%u name=\"%s\" planet=%u template=%s%s\n",entryMode,
     e.key.c_str(),e.number,e.name.c_str(),e.planet,c->courseScreenTemplate.c_str(),found?"":" template_planet=missing");
    if(found)addons.push_back(std::move(e));
   }
   addonCarousel=!addons.empty();
  }catch(...){addons.clear();addonCarousel=false;}
 }
 // Race-return Hall (mode0): open on the add-on course this race ran.
 void openRacedAddon()noexcept{
  if(entryMode!=0||!addonCarousel||addonSlot)return;
  OriginalUiRiderProfileOwner50KG::CourseRaceSelection race;
  if(!cold.profile->courseRaceSnapshot(race))return;
  const unsigned number=game::dlc50DL::addonCourseForLaunch(0,race.subtype);
  if(!number)return;
  for(std::size_t i=0;i<addons.size();++i)if(addons[i].number==number){
   addonAnchorLast=true;addonSlot=i+1;addonDrawLogged=false;addonLog("open_raced");return;}
  std::fprintf(stderr,"dlc_hall_addon action=open_raced_skipped number=%u reason=\"not planned\"\n",number);
 }
 // The anchor control's course-name text widget (0 = none).
 std::uint16_t anchorName()const noexcept{
  const auto* page=controller.page();const auto slot=nav.navigation.groups[1].selectedSlot;
  if(!page||page->groups.size()<2||!slot||slot>page->groups[1].controls.size())return 0;
  const auto id=page->groups[1].controls[slot-1];
  if(id<0||std::size_t(id)>=page->controls.size())return 0;
  for(const auto w:page->controls[std::size_t(id)].widgets)if(cold.resources->widgets()->textDescriptor(w))return w;
  return 0;
 }
 bool operational()const noexcept{return !state.faulted&&!state.quitRequested&&!sourceClosing&&!sourceTornDown;}
 auto transition()const noexcept{return cold.externalTransition->snapshot();}
 bool currentPage()const noexcept{const auto t=transition();return t.bound&&t.current.screen==11&&t.current.page==1;}
 bool process()const noexcept{return cold.application->known&&!(cold.application->globalFlags228&4u)&&cold.application->transportE00==-1&&cold.commonProfile->known&&cold.profile->commonProfile()==cold.commonProfile.get();}
 bool profileCurrent()const noexcept{game::OriginalUiHallProfile50KG p;return entryCommitted&&process()&&common(*cold.commonProfile,liveCommon)&&stamp(cold.profile->stamp(),liveStamp)&&cold.profile->hallSnapshot(p);}
 bool idle()const noexcept{const auto t=transition();return currentPage()&&t.lifecycle==Life::idle&&t.phase==game::OriginalUiTransitionPhase50KG::none&&!t.applicationRequestQueued&&!t.initialRestartPending;}
 bool muted()const noexcept{return cold.audioMode==OriginalUiMainAudioMode50KG::muted;}
 bool audioReady()const noexcept{return muted()?!cold.audio:cold.audio&&cold.audio->hallOfFameMenuReady();}
 bool soundReady(std::uint32_t id)const noexcept{return audioReady()&&(muted()||cold.audio->canPlayGameOptionsEffect(id));}
 bool sound(std::uint32_t id)noexcept{if(muted()){++state.audio.suppressedByMutedMode;return true;}if(!cold.audio->playGameOptionsEffect(id)){++state.audio.failed;return false;}++state.audio.submitted;return true;}
 void fault(S v,std::uint32_t pc)noexcept{if(!state.faulted){faultDetail.status=v;faultDetail.sourcePC=pc;}state.faulted=true;state.framePublished=false;}
 bool apply(const game::OriginalUiHallPlan50KG&p)noexcept{
  auto& bank=*cold.resources->widgets();
  if(!profileCurrent()||!bank.preflight(p.callbacks.data(),p.callbackCount,faultDetail.widget)||
     (p.soundRequested&&!soundReady(p.soundId))||
     (p.startSong5&&(!audioReady()||(!muted()&&!cold.audio->canStartHallOfFameLoop5())))||
     (p.fadeMusic&&(!audioReady()||(!muted()&&!cold.audio->canFadeHallOfFameMusic())))||
     (p.pauseVideo&&(state.videoPaused84||state.movementPresentationPending||p.pauseAfterCallbackCount>p.callbackCount))||
     ((p.resumeVideo||p.drainPreviousFrame||p.setVideoEffect250)&&(!state.videoPaused84||nav.deferred1460!=1||!p.resumeVideo||!p.drainPreviousFrame||!p.setVideoEffect250)))return false;
  if(p.activateCamera&&!cold.resources->camera())return false;
  if(p.pauseVideo){if(!bank.apply(p.callbacks.data(),p.pauseAfterCallbackCount,faultDetail.widget))return false;
   state.videoPaused84=true;
   if(!bank.apply(p.callbacks.data()+p.pauseAfterCallbackCount,p.callbackCount-p.pauseAfterCallbackCount,faultDetail.widget))return false;
  }else if(!bank.apply(p.callbacks.data(),p.callbackCount,faultDetail.widget))return false;
  if(p.activateCamera)state.cameraActive=true;
  if(p.clearScene3D8)cold.application->uiObject3D8=0;
  if(p.applyBackground430){std::uint32_t packed=0;for(unsigned i=0;i<3;++i){std::int32_t x=0;if(!game::convertOriginalUiApertureCvtWS50KG(controller.page()->backgroundColor[i],x)||x<0||x>255)return false;packed|=std::uint32_t(x)<<(16-8*i);}state.packedBackground430=packed;}
  if(p.writePlanet){if(!cold.profile->commitHallPlanet(p.planet,liveStamp))return false;liveStamp=cold.profile->stamp();}
  if(p.startSong5||p.fadeMusic){if(muted())++state.audio.suppressedByMutedMode;else{if(!(p.startSong5?cold.audio->startHallOfFameLoop5():cold.audio->fadeHallOfFameMusic()))return false;++state.audio.submitted;}}
  // Renderer queue completion and source framebuffer byteblend are executed by
  // the frontend before its next render. This flag is a pending obligation,
  // not a fabricated drain receipt. Only that owner can discharge it.
  if(p.resumeVideo){state.videoPaused84=false;state.movementPresentationPending=true;}
  if(p.saveUpdateDelta&&!okay(aperture.saveHallOfFameUpdateDelta38(p.updateDelta)))return false;
  nav=p.state;if(!controller.selectedCourse(nav,state.courseIndex,state.planetIndex))return false;
  if(p.soundRequested&&!sound(p.soundId))return false;return true;
 }
 void phase(HP p,std::uint32_t pc)noexcept{game::OriginalUiHallProfile50KG view;game::OriginalUiHallPlan50KG plan;
  if(!profileCurrent()||!cold.profile->hallSnapshot(view)||controller.phase(nav,p,view,plan)!=MS::ok||!apply(plan))fault(S::widget,pc);}
 static bool available(void* p)noexcept{auto&s=*static_cast<Storage*>(p);return s.operational()&&s.cold.resources->exitText()&&s.cold.resources->exitText()->prepared();}
 static void hook(void*p,const game::OriginalUiApertureHook50KG&e)noexcept{auto&s=*static_cast<Storage*>(p);if(!s.operational())return;
  switch(e.kind){case H::openingAC:s.phase(HP::openingAC,e.targetPC);if(s.operational())s.openRacedAddon();break;case H::openedB0:s.phase(HP::openedB0,e.targetPC);break;case H::closingB4:s.phase(HP::closingB4,e.targetPC);break;case H::closingThresholdB8:s.phase(HP::closedB8,e.targetPC);break;case H::closedBC:if(e.targetPC!=0x1D7A00)s.fault(S::aperture,e.targetPC);break;case H::screenExitObject440:if(!s.rendering||!s.collector.projectExitText(e,s.faultDetail.draw))s.fault(S::draw,e.callPC);break;}}
 static void overlay(void*p,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG&r)noexcept{auto&s=*static_cast<Storage*>(p);if(s.operational()&&(!s.rendering||!s.collector.projectAperture(ordinal,r,s.faultDetail.draw)))s.fault(S::draw,0x1D798C);}
 bool updateWidgets(bool top)noexcept{const auto* ids=cold.resources->widgets()->order(top);if(!ids)return false;for(auto id:*ids){auto& b=*cold.resources->widgets();if(b.state(id).kind==game::OriginalUiHallWidgetKind50KG::text&&!b.updateText(id,true,faultDetail.widget)&&faultDetail.widget.error!=game::OriginalUiHallWidgetError50KG::hidden)return false;}return true;}
};
OriginalUiHallOfFameSession50KG::OriginalUiHallOfFameSession50KG()noexcept=default;
OriginalUiHallOfFameSession50KG::~OriginalUiHallOfFameSession50KG()=default;
S OriginalUiHallOfFameSession50KG::bind(const OriginalUiHallSessionCold50KG&cold,D&d)noexcept{
 d={};if(storage_)return result(d,S::alreadyBound);
 if(!cold.resources||!cold.resources->loaded()||!cold.resources->widgets()||!cold.resources->camera()||!cold.resources->exitText()||!cold.resources->exitText()->prepared()||!cold.hallArchive||cold.hallArchive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::hallOfFame))return result(d,S::resource);
 if(!cold.externalTransition||cold.externalTransition->snapshot().bound||!cold.profile||!cold.profile->hallRecordsReady()||!cold.commonProfile||!cold.commonProfile->known||cold.profile->commonProfile()!=cold.commonProfile.get()||!cold.application||!cold.application->known||(cold.application->globalFlags228&4)||cold.application->transportE00!=-1)return result(d,S::profile);
 if((cold.audioMode!=OriginalUiMainAudioMode50KG::muted&&cold.audioMode!=OriginalUiMainAudioMode50KG::enabled)||(cold.audioMode==OriginalUiMainAudioMode50KG::muted?bool(cold.audio):!cold.audio||!cold.audio->hallOfFameMenuReady()))return result(d,S::audio);
 const bool raceReturn=cold.origin==OriginalUiHallEntryOrigin50KG::tournamentRaceReturn;
 if((!raceReturn&&cold.origin!=OriginalUiHallEntryOrigin50KG::options)||(raceReturn&&cold.commonProfile->mode34!=2&&cold.commonProfile->mode34!=0))return result(d,S::profile);
 // Race return keeps P+34: Tournament (mode2, 1AB698) or Free Play (mode0, 1AB884).
 const std::uint32_t entryMode=raceReturn?cold.commonProfile->mode34:5u;
 try{auto s=std::make_unique<Storage>();s->cold=cold;s->coldCommon=*cold.commonProfile;s->coldStamp=cold.profile->stamp();s->entryMode=entryMode;s->state.entryMode34=entryMode;game::OriginalUiHallProfile50KG view;game::OriginalUiHallPlan50KG plan;
  assets::OriginalUiPageDiagnostic50KG parse;if(s->controller.bind(cold.hallArchive,parse)!=MS::ok||!s->controller.initialState(s->nav)||!cold.profile->hallSnapshotForMode(entryMode,view)||s->controller.phase(s->nav,HP::entry74,view,plan)!=MS::ok)return result(d,S::profile);
  if(!s->collector.bindFrame(cold.resources,cold.gpu,d.draw))return result(d,S::draw);
  game::OriginalUiApertureCallbacks50KG cb;cb.context=s.get();cb.available=&Storage::available;cb.hook=&Storage::hook;cb.submitOverlay=&Storage::overlay;
  if(s->aperture.bindHallOfFame(cold.hallArchive,cb,d.aperture)!=AS::ok)return result(d,S::aperture);
  s->planAddons(); // PORT EXTENSION (148r): add-on course entries; none = stock Hall unchanged
  s->state.bound=true;storage_=std::move(s);return result(d,S::ok);
 }catch(...){return result(d,S::resource);}
}
bool OriginalUiHallOfFameSession50KG::acceptEntryProfileCommit()noexcept{auto*s=storage_.get();if(!s||!s->operational()||s->entryCommitted||s->inOperation||!s->process())return false;
 if(s->entryMode!=5){
  // 17DE10(App,6,11,1) -> 17DB80/ld_ui writes no mode: the common profile and
  // rider-profile generation must be exactly those seen at bind.
  const auto st=s->cold.profile->stamp();
  if(!common(s->coldCommon,*s->cold.commonProfile)||s->coldCommon.mode34!=s->entryMode||st.generation!=s->coldStamp.generation||st.mode34!=s->entryMode)return false;
  s->liveCommon=s->coldCommon;s->liveStamp=st;s->entryCommitted=true;return true;}
 auto expected=s->coldCommon;if(expected.mode34!=5){expected.previousMode38=expected.mode34;expected.mode34=5;}auto st=s->cold.profile->stamp();if(!common(expected,*s->cold.commonProfile)||st.generation!=s->coldStamp.generation||st.mode34!=5)return false;s->liveCommon=expected;s->liveStamp=st;s->entryCommitted=true;return true;}
S OriginalUiHallOfFameSession50KG::notifyTransition(const game::OriginalUiTransitionNotification50KG&e,D&d)noexcept{
 d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational()||s->inOperation||!s->profileCurrent()||e.route.screen!=11||e.route.page!=1)return result(d,S::unsupportedState,e.sourcePC);
 const auto t=s->transition();if(!t.bound||e.lifecycle!=t.lifecycle||e.phase!=t.phase||!((t.current.screen==11&&t.current.page==1)||(t.requested.screen==11&&t.requested.page==1)))return result(d,S::transition,e.sourcePC);
 s->state.framePublished=false;s->state.backProposalPending=false;s->confirmPending=false;s->inOperation=true;
 switch(e.kind){case N::requestedPage90:s->phase(HP::requestedPage90,e.sourcePC);s->state.pageShown=true;break;case N::previousPage94:s->phase(HP::previousPage94,e.sourcePC);s->state.pageShown=false;break;case N::pageEntry74:s->phase(HP::entry74,e.sourcePC);break;case N::managerEntry84:break;case N::beginTransition7C:if(!okay(s->aperture.beginTransition7C(e.phase)))s->fault(S::aperture,e.sourcePC);break;case N::pageAuxiliary1C:s->state.inputContainerShown=false;break;case N::pageAuxiliary24:s->state.inputContainerShown=true;break;}
 s->inOperation=false;if(s->state.faulted){d=s->faultDetail;return d.status;}return result(d,S::ok);
}
game::OriginalUiTransitionCompletion50KG OriginalUiHallOfFameSession50KG::pollCompletion(game::OriginalUiTransitionRoute50KG r,game::OriginalUiTransitionPhase50KG p)const noexcept{const auto*s=storage_.get();if(!s||!s->operational()||s->inOperation||!s->profileCurrent()||r.screen!=11||r.page!=1)return game::OriginalUiTransitionCompletion50KG::unavailable;const auto t=s->transition();if(!s->currentPage()||t.phase!=p)return game::OriginalUiTransitionCompletion50KG::unavailable;return s->aperture.pollCompletion(r,p);}
const game::OriginalUiTransitionOwner50KG* OriginalUiHallOfFameSession50KG::externalLifecycleOwner()const noexcept{return storage_?storage_->cold.externalTransition.get():nullptr;}
bool OriginalUiHallOfFameSession50KG::ownsPreparedPage(std::uint8_t p)const noexcept{return storage_&&storage_->operational()&&!storage_->inOperation&&p==1;}
bool OriginalUiHallOfFameSession50KG::canBeginMovement()const noexcept{const auto*s=storage_.get();return s&&s->operational()&&!s->inOperation&&!s->updatePrepared&&s->profileCurrent()&&s->idle()&&s->state.pageShown&&s->state.inputContainerShown&&s->state.framePublished&&s->aperture.snapshot().inputEnabled268&&!s->state.videoPaused84&&!s->state.movementPresentationPending&&!s->nav.deferred1460&&!s->state.backProposalPending&&!s->confirmPending;}
bool OriginalUiHallOfFameSession50KG::movementInProgress()const noexcept{return storage_&&storage_->operational()&&(storage_->nav.deferred1460>0||storage_->state.movementPresentationPending);}
bool OriginalUiHallOfFameSession50KG::completeMovementPresentation()noexcept{auto*s=storage_.get();if(!s||!s->operational()||s->inOperation||s->updatePrepared||!s->state.movementPresentationPending||s->nav.deferred1460||s->state.videoPaused84)return false;s->state.movementPresentationPending=false;return true;}
S OriginalUiHallOfFameSession50KG::input(const OriginalUiMainSessionInput50KG&in,OriginalUiHallSessionInputReceipt50KG&r,D&d)noexcept{
 d={};r={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,s->state.quitRequested?S::quit:S::faulted);if(s->inOperation||s->updatePrepared||in.controller>1||in.originalInput>15)return result(d,S::invalidInput);
 const bool movementReady=canBeginMovement();s->state.backProposalPending=false;s->backCommitted=false;s->confirmPending=false;s->confirmCommitted=false;
 r.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
 if(!r.containerReached||!s->aperture.snapshot().inputEnabled268||in.popupActive||in.sourceInputBlocked||movementInProgress())return result(d,r.status=S::noChange);
 if(!s->profileCurrent())return result(d,r.status=S::profile);
 if(in.controller==0&&in.originalInput<=1&&!movementReady)return result(d,r.status=S::noChange);
 game::OriginalUiHallProfile50KG view;game::OriginalUiHallPlan50KG plan;if(!s->cold.profile->hallSnapshot(view))return result(d,r.status=S::profile);
 r.pageCallbackRan=true;auto status=s->controller.input(s->nav,in.controller,static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),true,true,view,plan);r.pageAccepted=plan.pageAccepted;
 if(status!=MS::ok&&status!=MS::noChange&&status!=MS::unavailable)return result(d,r.status=S::widget);
 if(plan.confirmRequested){
  r.confirmRoute=plan.confirmRoute;
  if(plan.confirmRoute==game::OriginalUiHallConfirmRoute50KG::ending){
   // STAND-IN: 1E40A0 (1468=1, doors close, B8 1E4980 -> 17DE10(App,0x28,3,0))
   // has no native end-scene receiver yet. Nothing is committed: no sound1,
   // no close; the page stays open and input remains live.
   r.routeProposal={this,0,plan.confirm,s->liveStamp};return result(d,r.status=S::unavailableDestination,0x1E40A0);}
  if(plan.confirmRoute!=game::OriginalUiHallConfirmRoute50KG::assertion){
   if(!s->idle()||!s->soundReady(1)||!game::originalUiHallConfirmShape50KG(plan.confirm))return result(d,r.status=S::unavailableDestination);
   s->pending=plan;s->pendingStamp=s->liveStamp;s->confirmPending=true;++s->serial;if(!s->serial)++s->serial;
   r.routeProposal={this,s->serial,plan.confirm,s->pendingStamp};return result(d,r.status=S::confirmProposed);}
  // assertion: falls through to apply(): sound1 + flush only.
 }
 if(plan.backRequested){if(!s->idle()||!s->soundReady(5)||!game::originalUiHallBackShape50KG(plan.back))return result(d,r.status=S::unavailableDestination);s->pending=plan;s->pendingStamp=s->liveStamp;s->state.backProposalPending=true;++s->serial;if(!s->serial)++s->serial;r.routeProposal={this,s->serial,plan.back,s->pendingStamp};return result(d,r.status=S::backProposed);}
 if(status==MS::noChange)return result(d,r.status=S::noChange);
 // PORT EXTENSION (DLC add-on course, 148r): the add-on block at the wrap.
 if(s->addonCarousel&&in.controller==0&&(in.originalInput==0||in.originalInput==1)&&status==MS::ok&&plan.pauseVideo&&plan.soundRequested){
  const bool forward=in.originalInput==1;
  const auto from=s->nav.navigation.groups[1].selectedSlot,to=plan.state.navigation.groups[1].selectedSlot;
  const bool wraps=forward?to<=from:to>=from;
  if(s->addonSlot||wraps){
   if(!s->soundReady(plan.soundId))return result(d,r.status=S::audio,plan.soundPC);
   const auto count=s->addons.size();bool stockMove=false;const char* action=nullptr;
   if(!s->addonSlot){s->addonAnchorLast=forward;s->addonSlot=forward?1:count;action="enter";}
   else if(forward?s->addonSlot<count:s->addonSlot>1){s->addonSlot=forward?s->addonSlot+1:s->addonSlot-1;action="step";}
   else{const bool across=forward==s->addonAnchorLast;s->addonLog(across?"leave_wrap":"leave");s->addonSlot=0;stockMove=across;}
   s->addonDrawLogged=false;
   if(!stockMove){
    if(action)s->addonLog(action);
    s->state.framePublished=false;s->inOperation=true;const bool played=s->sound(plan.soundId);s->inOperation=false;
    if(!played){s->fault(S::audio,plan.soundPC);d=s->faultDetail;return r.status=d.status;}
    return result(d,r.status=S::ok);
   }
   // Across the wrap: the original move from the anchor follows unchanged.
  }
 }
 s->state.framePublished=false;s->inOperation=true;const bool applied=s->apply(plan);s->inOperation=false;if(!applied){s->fault(S::widget,0x1E3F70);d=s->faultDetail;return r.status=d.status;}
 r.movementAccepted=plan.pauseVideo;return result(d,r.status=S::ok);
}
bool OriginalUiHallOfFameSession50KG::routeProposalCurrent(const OriginalUiHallRouteProposal50KG&p)const noexcept{const auto*s=storage_.get();if(!s||!s->operational()||s->inOperation||s->updatePrepared||s->backCommitted||!s->state.backProposalPending||p.source!=this||!p.serial||p.serial!=s->serial||!stamp(p.profileStamp,s->pendingStamp)||!stamp(p.profileStamp,s->liveStamp)||!s->profileCurrent()||!s->idle()||!decision(p.decision,s->pending.back)||!s->soundReady(5))return false;game::OriginalUiHallProfile50KG view;game::OriginalUiHallPlan50KG expected;return s->cold.profile->hallSnapshot(view)&&s->controller.input(s->nav,0,assets::OriginalUiMovementInput50KG(6),true,true,view,expected)==MS::unavailable&&expected.backRequested&&decision(expected.back,p.decision);}
bool OriginalUiHallOfFameSession50KG::commitBack(const OriginalUiHallRouteProposal50KG&p,const game::OriginalUiRouteDecision50KG&admitted)noexcept{if(!routeProposalCurrent(p)||!admitted.admitted||admitted.blockers)return false;auto expected=p.decision;expected.admitted=true;expected.blockers=0;expected.action=game::OriginalUiRouteAction50KG::request;if(!decision(expected,admitted))return false;auto&s=*storage_;if(!s.cold.profile->commitHallBack(admitted.selectedSlot,admitted,s.liveStamp))return false;s.liveStamp=s.cold.profile->stamp();s.backCommitted=true;return true;}
S OriginalUiHallOfFameSession50KG::notifyRouteAccepted(const OriginalUiHallRouteProposal50KG&p,D&d)noexcept{d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational()||s->inOperation||s->updatePrepared||!s->backCommitted||!s->state.backProposalPending||p.source!=this||p.serial!=s->serial||!decision(p.decision,s->pending.back)||!stamp(p.profileStamp,s->pendingStamp)||!s->profileCurrent())return result(d,S::invalidInput);const auto t=s->transition();if(!s->currentPage()||t.requested.screen!=12||t.requested.page!=1||t.lifecycle!=Life::requestScreenExit||t.phase!=game::OriginalUiTransitionPhase50KG::none||t.applicationRequestQueued||t.initialRestartPending)return result(d,S::transition);s->state.framePublished=false;s->state.backProposalPending=false;if(!s->sound(5)){s->fault(S::audio,0x1E4280);return result(d,S::audio);}return result(d,S::ok);}
bool OriginalUiHallOfFameSession50KG::confirmProposalCurrent(const OriginalUiHallRouteProposal50KG&p)const noexcept{const auto*s=storage_.get();
 if(!s||!s->operational()||s->inOperation||s->updatePrepared||s->confirmCommitted||!s->confirmPending||p.source!=this||!p.serial||p.serial!=s->serial||
    !stamp(p.profileStamp,s->pendingStamp)||!stamp(p.profileStamp,s->liveStamp)||!s->profileCurrent()||!s->idle()||
    !s->pending.confirmRequested||!decision(p.decision,s->pending.confirm)||!game::originalUiHallConfirmShape50KG(p.decision)||!s->soundReady(1))return false;
 // Same live table again (1E3FC0 reads P+34/102CC/187B10/187E30 at the press).
 game::OriginalUiHallProfile50KG view;game::OriginalUiHallPlan50KG expected;
 return s->cold.profile->hallSnapshot(view)&&s->controller.input(s->nav,0,assets::OriginalUiMovementInput50KG(4),true,true,view,expected)==MS::unavailable&&
  expected.confirmRequested&&decision(expected.confirm,p.decision);}
bool OriginalUiHallOfFameSession50KG::commitConfirm(const OriginalUiHallRouteProposal50KG&p,const game::OriginalUiRouteDecision50KG&admitted)noexcept{
 if(!confirmProposalCurrent(p)||!admitted.admitted||admitted.blockers)return false;auto expected=p.decision;expected.admitted=true;expected.blockers=0;expected.action=game::OriginalUiRouteAction50KG::request;
 if(!decision(expected,admitted))return false;storage_->confirmCommitted=true;return true;}
S OriginalUiHallOfFameSession50KG::notifyConfirmAccepted(const OriginalUiHallRouteProposal50KG&p,D&d)noexcept{d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);
 if(!s->operational()||s->inOperation||s->updatePrepared||!s->confirmCommitted||!s->confirmPending||p.source!=this||p.serial!=s->serial||!decision(p.decision,s->pending.confirm)||!stamp(p.profileStamp,s->pendingStamp)||!s->profileCurrent())return result(d,S::invalidInput);
 const auto t=s->transition();if(!s->currentPage()||t.requested.screen!=p.decision.request.targetScreen||t.requested.page!=p.decision.request.targetPage||t.lifecycle!=Life::requestScreenExit||t.phase!=game::OriginalUiTransitionPhase50KG::none||t.applicationRequestQueued||t.initialRestartPending)return result(d,S::transition);
 s->state.framePublished=false;s->confirmPending=false;if(!s->sound(1)){s->fault(S::audio,0x1E41E4);return result(d,S::audio);}return result(d,S::ok);}
S OriginalUiHallOfFameSession50KG::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&in,D&d)noexcept{d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);if(s->inOperation||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!std::isfinite(in.elapsedSeconds)||in.elapsedSeconds<0||in.elapsedSeconds>1||(in.paused&&in.elapsedSeconds!=0))return result(d,S::invalidInput);s->state.framePublished=false;s->state.backProposalPending=false;s->confirmPending=false;s->paused=in.paused;if(!s->muted()){s->cold.audio->pause(in.paused);if(!s->cold.audio->update(in.elapsedSeconds)){s->fault(S::audio,0x234440);return result(d,S::audio);}}s->prepared=in;s->updatePrepared=true;return result(d,S::ok);}
S OriginalUiHallOfFameSession50KG::update(const OriginalUiMainSessionUpdate50KG&in,D&d)noexcept{d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);if(s->inOperation||!s->currentPage()||!s->profileCurrent()||!s->updatePrepared||in.elapsedSeconds!=s->prepared.elapsedSeconds||in.paused!=s->prepared.paused||in.debugChordHeld!=s->prepared.debugChordHeld)return result(d,S::invalidInput);s->updatePrepared=false;s->inOperation=true;game::OriginalUiHallPlan50KG plan;bool ok=true;
 // Source1E45C0 performs page deferred/video/delta work before generic2A6BB0.
 if(s->state.pageShown)ok=s->controller.update(s->nav,in.elapsedSeconds,plan)==MS::ok&&s->apply(plan);
 if(ok)ok=s->updateWidgets(false)&&s->updateWidgets(true);s->inOperation=false;if(!ok){s->fault(S::widget,0x1E45C0);d=s->faultDetail;return d.status;}++s->state.updateCount;return result(d,S::ok);}
S OriginalUiHallOfFameSession50KG::render(const OriginalUiMainFrameClock50KG&clock,D&d)noexcept{d={};auto*s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);if(s->inOperation||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!s->state.updateCount||!s->state.pageShown||!s->state.cameraActive||s->state.movementPresentationPending||(s->paused&&clock.advanceAnimations))return result(d,S::invalidInput);s->state.framePublished=false;s->state.backProposalPending=false;s->confirmPending=false;s->inOperation=s->rendering=true;
 game::OriginalUiHallCourseRecords50KG row;bool ok=s->cold.profile->hallBrowseRecordSnapshot(s->state.courseIndex,row);
 // PORT EXTENSION (DLC add-on course, 148r): an add-on entry draws its own rows,
 // planet, name and planet header over the anchor (see Storage::HallAddon).
 const auto* addon=s->addon();std::uint32_t planet=s->state.planetIndex;
 if(ok&&addon){
  assets::dlc50DL::ProgressHallView view;
  if(assets::dlc50DL::progressOwner().courseHall(addon->key,view))row=view.rows;
  else{row=assets::dlc50DL::emptyHallRows();if(!s->addonDrawLogged)std::fprintf(stderr,"dlc_hall_addon action=rows_unavailable key=%s\n",addon->key.c_str());}
  planet=addon->planet;
  s->addonFrame={};s->addonFrame.anchorName=s->anchorName();s->addonFrame.name=&addon->name;s->addonFrame.planetName=&addon->planetName;
 }
 OriginalUiHallPageInput50KG page{s->state.courseIndex,planet,s->aperture.snapshot().state==game::OriginalUiApertureState50KG::open,&row};
 OriginalUiCourseProfileColors50KG colors;if(ok)ok=s->cold.profile->hallBrowsePlanetColors(planet,colors);if(ok){page.colorsKnown=true;page.primaryColor79C=colors.primary79C;page.secondaryColor7A0=colors.secondary7A0;}
 // 1E2010/1E2860/1E30A8: skipped in mode5. Otherwise, when the shown control's
 // CourseIndex equals 1125F&15, row r of each category is drawn 0xCCCCCC if
 // its first- or second-human rank equals r+1. An add-on race's ranks are the
 // add-on course's own (dlc_course_records50dl); its anchor row is not this
 // race's row, so the stock recolour is not applied for an add-on race.
 if(ok&&s->entryMode!=5){game::OriginalUiHallProfile50KG view;ok=s->cold.profile->hallSnapshot(view);
  OriginalUiRiderProfileOwner50KG::CourseRaceSelection race;
  const unsigned raced=view.mode34==0&&s->cold.profile->courseRaceSnapshot(race)?game::dlc50DL::addonCourseForLaunch(0,race.subtype):0u;
  if(ok&&addon){const auto last=game::dlc50DL::lastAddonCourseRecord();
   if(last.number&&last.number==addon->number&&raced==addon->number){const std::uint32_t ranks[3]={last.lap,last.race,last.knockouts};
    for(std::size_t c=0;c<3;++c)for(std::uint32_t r=0;r<3;++r)page.newRecordRow[c*3+r]=ranks[c]==r+1;}}
  else if(ok&&!raced&&view.mode34!=5&&s->state.courseIndex==view.raceCourse1125F)
   for(std::size_t c=0;c<3;++c)for(std::uint32_t r=0;r<3;++r)page.newRecordRow[c*3+r]=view.ranks11260[c]==r+1||view.ranks1126C[c]==r+1;}
 if(addon)setOriginalUiHallAddonFrame50DL(&s->addonFrame);
 if(ok)ok=s->collector.begin({clock.known,clock.advanceAnimations,clock.milliseconds,clock.applicationAnimation},planet,d.draw)&&s->collector.projectOrdinaryWidgets(true,d.draw)&&s->collector.projectPage(page,s->state.pageShown,d.draw);
 if(ok&&s->state.pageShown){ok=s->cold.profile->commitHallRender(s->liveStamp);if(ok)s->liveStamp=s->cold.profile->stamp();}
 if(ok)ok=okay(s->aperture.render48(s->transition().lifecycle))&&!s->state.faulted;
 if(ok)ok=s->collector.projectPrompts(s->nav.inputFlags42C,d.draw)&&s->collector.projectOnTopWidgets(true,d.draw)&&s->collector.drain(d.draw);
 setOriginalUiHallAddonFrame50DL(nullptr);
 if(ok&&addon&&!s->addonDrawLogged){s->addonDrawLogged=true;
  bool marked=false;for(const bool m:page.newRecordRow)marked=marked||m;
  std::fprintf(stderr,"dlc_hall_draw key=%s number=%u name_drawn=%u name_skipped=%u planet_header=%u planet=%u lap0=%.2f race0=%.2f "
   "knockout0=%u new_record_rows=%u anchor_course=%u stock_row=untouched\n",addon->key.c_str(),addon->number,s->addonFrame.nameDrawn,
   s->addonFrame.nameSkipped,s->addonFrame.planetDrawn,planet,double(row.lap102F0[0]),double(row.race1033C[0]),unsigned(row.knockouts10388[0]),
   unsigned(marked),s->state.courseIndex);}
 s->inOperation=s->rendering=false;if(!ok){if(s->state.faulted){d=s->faultDetail;return d.status;}s->fault(S::draw,0x1E3EF0);return result(d,S::draw,0x1E3EF0);}s->state.framePublished=true;++s->state.renderCount;return result(d,S::ok);}
OriginalUiHallSessionSnapshot50KG OriginalUiHallOfFameSession50KG::snapshot()const noexcept{if(!storage_)return {};auto out=storage_->state;out.transition=storage_->transition();out.aperture=storage_->aperture.snapshot();out.deferred1460=storage_->nav.deferred1460;out.promptMask42C=storage_->nav.inputFlags42C;out.confirmProposalPending=storage_->confirmPending;out.entryMode34=storage_->entryMode;return out;}
const game::OriginalUiHallState50KG* OriginalUiHallOfFameSession50KG::navigationState()const noexcept{return storage_?&storage_->nav:nullptr;}
const OriginalUiNativeDraw50KG* OriginalUiHallOfFameSession50KG::draw()const noexcept{return storage_&&storage_->operational()&&storage_->state.framePublished&&!storage_->inOperation&&!storage_->updatePrepared&&storage_->currentPage()?storage_->collector.draw():nullptr;}
const OriginalUiSourceFrame50KM* OriginalUiHallOfFameSession50KG::sourceFrame()const noexcept{return storage_&&storage_->operational()&&storage_->state.framePublished&&!storage_->inOperation&&!storage_->updatePrepared&&storage_->currentPage()?storage_->collector.sourceFrame():nullptr;}
bool OriginalUiHallOfFameSession50KG::sourceTeardownPending()const noexcept{return storage_&&!storage_->sourceTornDown&&storage_->collector.sourceTeardownPending();}
S OriginalUiHallOfFameSession50KG::teardownSource(VehicleWindowGl50DS*r,D&d)noexcept{d={};auto*s=storage_.get();if(!s||s->sourceTornDown)return result(d,S::ok);if(s->inOperation||s->rendering)return result(d,S::busy);s->sourceClosing=true;s->state.framePublished=false;if(s->collector.sourceTeardownPending()&&(!r||!s->collector.retireSourceRenderer(*r,d.draw)))return result(d,S::draw);s->sourceTornDown=true;return result(d,S::ok);}
void OriginalUiHallOfFameSession50KG::requestQuit()noexcept{if(storage_&&!storage_->inOperation){storage_->state.quitRequested=true;storage_->state.framePublished=false;}}
}

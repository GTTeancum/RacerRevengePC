#include "original_ui_win_session50pd.h"
#include "../../../src/game/original_unlock_checks50pd.h"
#include "../../../src/game/dlc_career50dl.h"
#include "../../../src/assets/dlc_registry50dl.h"
#include <cmath>
#include <cstdio>
#include <limits>

namespace rr::platform {
namespace {
using S=OriginalUiWinSessionStatus50PD;using D=OriginalUiWinSessionDiagnostic50PD;
using WP=game::OriginalUiWinPhase50PD;using N=game::OriginalUiTransitionNotificationKind50KG;
using H=game::OriginalUiApertureHookKind50KG;using AS=game::OriginalUiApertureStatus50KG;
using Life=game::OriginalUiTransitionLifecycle50KG;using Music=game::OriginalUiWinMusic50PD;
S result(D& d,S s,std::uint32_t pc=0)noexcept{d.status=s;d.sourcePC=pc;return s;}
bool okay(AS s)noexcept{return s==AS::ok||s==AS::noChange;}
bool stamp(OriginalUiRiderProfileStamp50KG a,OriginalUiRiderProfileStamp50KG b)noexcept{return a.generation==b.generation&&a.mode34==b.mode34;}
bool sameDecision(const game::OriginalUiRouteDecision50KG& a,const game::OriginalUiRouteDecision50KG& b)noexcept{
    return a.action==b.action&&a.blockers==b.blockers&&a.admitted==b.admitted&&a.profileProposalComplete==b.profileProposalComplete&&
        a.selectedSlot==b.selectedSlot&&a.selectedRider==b.selectedRider&&a.inputPC==b.inputPC&&a.request.kind==b.request.kind&&
        a.request.sourceScreen==b.request.sourceScreen&&a.request.sourcePage==b.request.sourcePage&&a.request.targetScreen==b.request.targetScreen&&
        a.request.targetPage==b.request.targetPage&&a.request.sourcePC==b.request.sourcePC&&a.profileChanges.count==b.profileChanges.count&&!a.profileChanges.count;
}
}
struct OriginalUiWinSession50PD::Storage {
    OriginalUiWinSessionCold50PD cold;OriginalUiWinSessionSnapshot50PD state{};D faultDetail{};
    game::OriginalUiApertureOwner50KG aperture;OriginalUiWinDrawContribution50PD collector;
    OriginalUiRiderProfileStamp50KG coldStamp{};OriginalUiWinRouteProposal50PD pending{};
    OriginalUiMainSessionUpdate50KG prepared{};std::uint32_t career=0;
    bool inOperation=false,rendering=false,updatePrepared=false,paused=false,sourceClosing=false,sourceTornDown=false;
    std::uint64_t serial=0;
    game::OriginalUiWinPage50PD& page()noexcept{return *cold.resources->page();}
    bool operational()const noexcept{return !state.faulted&&!state.quitRequested&&!sourceClosing&&!sourceTornDown;}
    auto transition()const noexcept{return cold.externalTransition->snapshot();}
    bool currentPage()const noexcept{const auto t=transition();return t.bound&&t.current.screen==7&&t.current.page==1;}
    bool process()const noexcept{return cold.application->known&&!(cold.application->globalFlags228&4u)&&cold.application->transportE00==-1&&
        cold.commonProfile->known&&cold.profile->initialized()&&cold.profile->commonProfile()==cold.commonProfile.get();}
    // UIWin is reached only from the tournament acknowledgement rows (P+34==2).
    // The page itself writes no profile field; any change is a foreign mutation.
    bool profileCurrent()const noexcept{return process()&&cold.commonProfile->mode34==2&&stamp(cold.profile->stamp(),coldStamp);}
    bool idle()const noexcept{const auto t=transition();return currentPage()&&t.lifecycle==Life::idle&&t.phase==game::OriginalUiTransitionPhase50KG::none&&!t.applicationRequestQueued&&!t.initialRestartPending;}
    bool muted()const noexcept{return cold.audioMode==OriginalUiMainAudioMode50KG::muted;}
    bool audioReady()const noexcept{return muted()?!cold.audio:cold.audio&&cold.audio->winMenuReady();}
    void fault(S v,std::uint32_t pc)noexcept{if(!state.faulted){faultDetail.status=v;faultDetail.sourcePC=pc;}state.faulted=true;state.framePublished=false;}
    bool facts(game::OriginalUnlockFacts50PD& f)const noexcept{return profileCurrent()&&cold.profile->unlockFacts187E30(f);}
    bool musicReady(Music m)const noexcept{
        if(m==Music::none)return true;if(!audioReady())return false;if(muted())return true;
        return m==Music::fade1s?cold.audio->canFadeMusic234440():cold.audio->canStartSong234880(m==Music::song10Once?10u:8u);}
    bool music(Music m)noexcept{
        if(m==Music::none)return true;if(muted()){++state.audio.suppressedByMutedMode;return true;}
        const bool ok=m==Music::fade1s?cold.audio->fadeMusic234440(1.0f):cold.audio->startSong234880(m==Music::song10Once?10u:8u);
        if(!ok){++state.audio.failed;return false;}++state.audio.submitted;return true;}
    // Keep the User_Rotate model lifetime equal to the page widget's loaded state.
    bool syncModels(D& d)noexcept{
        for(std::uint16_t id=9;id<=14;++id){auto* m=cold.resources->model(id);if(!m)return false;const bool loaded=page().state(id).loaded;
            if(loaded&&!m->attached()){if(!m->attach(d.scene))return false;}else if(!loaded&&m->attached())m->release();}
        return true;}
    bool apply(const game::OriginalUiWinPlan50PD& p,D& d)noexcept{
        if(!profileCurrent()||!page().preflight(p,d.page)||!musicReady(p.music))return false;
        std::uint32_t packed=state.packedBackground430;
        if(p.applyBackground430){packed=0;for(unsigned i=0;i<3;++i){std::int32_t x=0;
            if(!game::convertOriginalUiApertureCvtWS50KG(page().page()->backgroundColor[i],x)||x<0||x>255)return false;packed|=std::uint32_t(x)<<(16-8*i);}}
        if(!page().apply(p,d.page)||!syncModels(d))return false;
        if(p.activateCamera)state.cameraActive=true;
        if(p.clearScene3D8)cold.application->uiObject3D8=0;
        if(p.applyBackground430)state.packedBackground430=packed;
        return music(p.music);}
    void phase(WP kind,std::uint32_t pc,const game::OriginalUiWinCircuit50PD* entry=nullptr)noexcept{
        game::OriginalUiWinPlan50PD plan;D d;
        if(!page().phase(kind,entry,plan,d.page)||!apply(plan,d)){fault(S::widget,pc);faultDetail.page=d.page;faultDetail.scene=d.scene;}}
    static bool available(void* p)noexcept{auto& s=*static_cast<Storage*>(p);return s.operational()&&s.cold.resources->exitText()&&s.cold.resources->exitText()->prepared();}
    static void hook(void* p,const game::OriginalUiApertureHook50KG& e)noexcept{auto& s=*static_cast<Storage*>(p);if(!s.operational())return;
        switch(e.kind){
        case H::openingAC:if(e.targetPC!=0x208810){s.fault(S::aperture,e.targetPC);break;}s.phase(WP::openingAC,e.targetPC);break;
        case H::openedB0:if(e.targetPC!=0x208900){s.fault(S::aperture,e.targetPC);break;}s.phase(WP::openedB0,e.targetPC);break;
        case H::closingB4:if(e.targetPC!=0x208950){s.fault(S::aperture,e.targetPC);break;}s.phase(WP::closingB4,e.targetPC);break;
        case H::closingThresholdB8:if(e.targetPC!=0x2089C0)s.fault(S::aperture,e.targetPC);break;   // empty 2089C0
        case H::closedBC:if(e.targetPC!=0x1D7A00)s.fault(S::aperture,e.targetPC);break;              // empty 1D7A00
        case H::screenExitObject440:if(!s.rendering||!s.collector.projectExitText(e,s.faultDetail.draw))s.fault(S::draw,e.callPC);break;}}
    static void overlay(void* p,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& r)noexcept{auto& s=*static_cast<Storage*>(p);
        if(s.operational()&&(!s.rendering||!s.collector.projectAperture(ordinal,r,s.faultDetail.draw)))s.fault(S::draw,0x1D798C);}
};
OriginalUiWinSession50PD::OriginalUiWinSession50PD()noexcept=default;
OriginalUiWinSession50PD::~OriginalUiWinSession50PD()=default;
S OriginalUiWinSession50PD::bind(const OriginalUiWinSessionCold50PD& cold,D& d)noexcept{
    d={};if(storage_)return result(d,S::alreadyBound);
    if(!cold.resources||!cold.resources->loaded()||!cold.resources->page()||!cold.resources->page()->bound()||
       !cold.resources->exitText()||!cold.resources->exitText()->prepared()||!cold.gpu||
       !cold.winArchive||cold.winArchive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::win))return result(d,S::resource);
    if(!cold.externalTransition||cold.externalTransition->snapshot().bound||!cold.profile||!cold.profile->initialized()||
       !cold.commonProfile||!cold.commonProfile->known||cold.profile->commonProfile()!=cold.commonProfile.get()||cold.commonProfile->mode34!=2||
       !cold.application||!cold.application->known||(cold.application->globalFlags228&4)||cold.application->transportE00!=-1)return result(d,S::profile);
    if((cold.audioMode!=OriginalUiMainAudioMode50KG::muted&&cold.audioMode!=OriginalUiMainAudioMode50KG::enabled)||
       (cold.audioMode==OriginalUiMainAudioMode50KG::muted?bool(cold.audio):!cold.audio||!cold.audio->winMenuReady()))return result(d,S::audio);
    try{
        auto s=std::make_unique<Storage>();s->cold=cold;s->coldStamp=cold.profile->stamp();
        // Entry 2084C0 must resolve; background/colour reads at render need the career.
        game::OriginalUnlockFacts50PD facts;game::OriginalUiWinCircuit50PD circuit;std::uint32_t pc=0;
        if(!s->facts(facts)||!game::resolveOriginalUiWinCircuit50PD(facts,circuit,pc))return result(d,S::profile,pc?pc:0x2084C0);
        OriginalUiRiderProfileColors50KG colors;
        if(!cold.resources->background(facts.career100C0)||!cold.profile->colors(facts.career100C0,colors))return result(d,S::profile,0x208AF4);
        s->career=facts.career100C0;s->state.career=facts.career100C0;
        if(!s->collector.bindFrame(cold.resources,cold.gpu,d.draw))return result(d,S::draw);
        game::OriginalUiApertureCallbacks50KG cb;cb.context=s.get();cb.available=&Storage::available;cb.hook=&Storage::hook;cb.submitOverlay=&Storage::overlay;
        if(s->aperture.bindCircuitWin(cold.winArchive,cb,d.aperture)!=AS::ok)return result(d,S::aperture);
        s->state.bound=true;storage_=std::move(s);return result(d,S::ok);
    }catch(...){return result(d,S::resource);}
}
S OriginalUiWinSession50PD::notifyTransition(const game::OriginalUiTransitionNotification50KG& e,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    // The page reads the profile at its own entry (2084C0). A screen shown
    // before it in the same frontend (the tournament Hall of Fame: record
    // insertion and name entry) legitimately changes the profile, so the
    // not-yet-shown page takes its profile baseline at its page request.
    if(e.kind==N::requestedPage90&&!s->state.pageShown&&s->operational()&&!s->inOperation&&
       s->process()&&s->cold.commonProfile->mode34==2&&e.route.screen==7&&e.route.page==1)
        s->coldStamp=s->cold.profile->stamp();
    if(!s->operational()||s->inOperation||!s->profileCurrent()||e.route.screen!=7||e.route.page!=1){
        std::fprintf(stderr,"win_session notify_rejected kind=%u operational=%d in_operation=%d profile_current=%d route=%u/%u page_shown=%d\n",
            unsigned(e.kind),int(s->operational()),int(s->inOperation),int(s->profileCurrent()),unsigned(e.route.screen),unsigned(e.route.page),
            int(s->state.pageShown));
        return result(d,S::unsupportedState,e.sourcePC);}
    const auto t=s->transition();
    if(!t.bound||e.lifecycle!=t.lifecycle||e.phase!=t.phase||!((t.current.screen==7&&t.current.page==1)||(t.requested.screen==7&&t.requested.page==1)))return result(d,S::transition,e.sourcePC);
    s->state.framePublished=false;s->state.routeProposalPending=false;s->inOperation=true;
    switch(e.kind){
    case N::requestedPage90:s->phase(WP::requestedPage90,e.sourcePC);s->state.pageShown=true;break;
    case N::previousPage94:s->phase(WP::previousPage94,e.sourcePC);s->state.pageShown=false;break;
    case N::pageEntry74:{game::OriginalUnlockFacts50PD facts;game::OriginalUiWinCircuit50PD circuit;std::uint32_t pc=0;
        if(!s->facts(facts)||!game::resolveOriginalUiWinCircuit50PD(facts,circuit,pc))s->fault(S::profile,pc?pc:0x2084C0);
        else s->phase(WP::entry74,0x2084C0,&circuit);break;}
    case N::managerEntry84:break;
    case N::beginTransition7C:if(!okay(s->aperture.beginTransition7C(e.phase)))s->fault(S::aperture,e.sourcePC);break;
    case N::pageAuxiliary1C:s->state.inputContainerShown=false;break;
    case N::pageAuxiliary24:s->state.inputContainerShown=true;break;}
    s->inOperation=false;if(s->state.faulted){d=s->faultDetail;return d.status;}return result(d,S::ok);
}
game::OriginalUiTransitionCompletion50KG OriginalUiWinSession50PD::pollCompletion(game::OriginalUiTransitionRoute50KG r,game::OriginalUiTransitionPhase50KG p)const noexcept{
    const auto* s=storage_.get();
    if(!s||!s->operational()||s->inOperation||!s->profileCurrent()||r.screen!=7||r.page!=1)return game::OriginalUiTransitionCompletion50KG::unavailable;
    const auto t=s->transition();if(!s->currentPage()||t.phase!=p)return game::OriginalUiTransitionCompletion50KG::unavailable;
    return s->aperture.pollCompletion(r,p);
}
const game::OriginalUiTransitionOwner50KG* OriginalUiWinSession50PD::externalLifecycleOwner()const noexcept{return storage_?storage_->cold.externalTransition.get():nullptr;}
bool OriginalUiWinSession50PD::ownsPreparedPage(std::uint8_t p)const noexcept{return storage_&&storage_->operational()&&!storage_->inOperation&&p==1;}
S OriginalUiWinSession50PD::input(const OriginalUiMainSessionInput50KG& in,OriginalUiWinSessionInputReceipt50PD& r,D& d)noexcept{
    d={};r={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational())return result(d,s->state.quitRequested?S::quit:S::faulted);
    if(s->inOperation||s->updatePrepared||in.controller>1||in.originalInput>15)return result(d,S::invalidInput);
    s->state.routeProposalPending=false;
    r.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
    // Generic page dispatch reaches 2089D0 only with page+268 set (bars open).
    if(!r.containerReached||!s->aperture.snapshot().inputEnabled268||in.popupActive||in.sourceInputBlocked)return result(d,r.status=S::noChange);
    if(!s->profileCurrent())return result(d,r.status=S::profile);
    game::OriginalUnlockFacts50PD facts;if(!s->facts(facts))return result(d,r.status=S::profile,0x187E30);
    const bool unlock=game::evaluateOriginalUnlocks187E30(facts).any();
    game::OriginalUiWinInputResult50PD out;r.pageCallbackRan=true;
    if(!s->page().input(in.controller,in.originalInput,unlock,out,d.page))return result(d,r.status=S::widget,d.page.sourcePC);
    r.pageAccepted=out.pageAccepted;if(!out.routeRequested)return result(d,r.status=S::noChange);
    if(!s->idle()||!s->audioReady()||(!s->muted()&&!s->cold.audio->canPlaySelect292360()))return result(d,r.status=S::unavailableDestination,0x208A68);
    ++s->serial;if(!s->serial)++s->serial;
    s->pending={this,s->serial,out.route,s->cold.profile->stamp()};s->state.routeProposalPending=true;r.routeProposal=s->pending;
    return result(d,r.status=S::routeProposed,out.route.request.sourcePC);
}
bool OriginalUiWinSession50PD::routeProposalCurrent(const OriginalUiWinRouteProposal50PD& p)const noexcept{
    const auto* s=storage_.get();
    if(!s||!s->operational()||s->inOperation||s->updatePrepared||!s->state.routeProposalPending||p.source!=this||!p.serial||p.serial!=s->serial||
       !stamp(p.profileStamp,s->pending.profileStamp)||!s->profileCurrent()||!s->idle()||!sameDecision(p.decision,s->pending.decision))return false;
    // The 187E30 choice must still select the same destination.
    game::OriginalUnlockFacts50PD facts;if(!s->facts(facts))return false;
    const auto target=game::evaluateOriginalUnlocks187E30(facts).any()?10:13;
    return p.decision.request.targetScreen==target&&p.decision.request.targetPage==1;
}
S OriginalUiWinSession50PD::notifyRouteAccepted(const OriginalUiWinRouteProposal50PD& p,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||s->inOperation||s->updatePrepared||!s->state.routeProposalPending||p.source!=this||p.serial!=s->serial||
       !sameDecision(p.decision,s->pending.decision)||!stamp(p.profileStamp,s->pending.profileStamp)||!s->profileCurrent())return result(d,S::invalidInput);
    const auto t=s->transition();
    if(!s->currentPage()||t.requested.screen!=p.decision.request.targetScreen||t.requested.page!=1||t.lifecycle!=Life::requestScreenExit||
       t.phase!=game::OriginalUiTransitionPhase50KG::none||t.applicationRequestQueued||t.initialRestartPending)return result(d,S::transition);
    s->state.framePublished=false;s->state.routeProposalPending=false;
    if(s->muted()){++s->state.audio.suppressedByMutedMode;return result(d,S::ok);}
    if(!s->cold.audio->playSelect292360()){++s->state.audio.failed;s->fault(S::audio,0x208A68);return result(d,S::audio,0x208A68);}
    ++s->state.audio.submitted;return result(d,S::ok);
}
S OriginalUiWinSession50PD::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG& in,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
    if(s->inOperation||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!std::isfinite(in.elapsedSeconds)||in.elapsedSeconds<0||in.elapsedSeconds>1||
       (in.paused&&in.elapsedSeconds!=0))return result(d,S::invalidInput);
    s->state.framePublished=false;s->state.routeProposalPending=false;s->paused=in.paused;
    if(!s->muted()){s->cold.audio->pause(in.paused);if(!s->cold.audio->update(in.elapsedSeconds)){s->fault(S::audio,0x234440);return result(d,S::audio);}}
    s->prepared=in;s->updatePrepared=true;return result(d,S::ok);
}
S OriginalUiWinSession50PD::update(const OriginalUiMainSessionUpdate50KG& in,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
    if(s->inOperation||!s->currentPage()||!s->profileCurrent()||!s->updatePrepared||in.elapsedSeconds!=s->prepared.elapsedSeconds||
       in.paused!=s->prepared.paused)return result(d,S::invalidInput);
    s->updatePrepared=false;s->inOperation=true;bool ok=true;
    // 209A70: page+298 = dt, then 2A6BB0 child update (texts, User_Rotate 1D38B0).
    if(s->state.pageShown)ok=okay(s->aperture.saveCircuitWinUpdateDelta38(in.elapsedSeconds));
    if(ok)for(auto id:*s->page().order()){const auto st=s->page().state(id);
        if(st.kind==game::OriginalUiWinWidgetKind50PD::text){if(!s->page().updateText(id,true,d.page)&&d.page.error!=game::OriginalUiWinPageError50PD::hidden){ok=false;break;}}
        else if(st.kind==game::OriginalUiWinWidgetKind50PD::rotate&&st.loaded){auto* m=s->cold.resources->model(id);
            if(!m||!m->attached()||!m->advance(in.elapsedSeconds,d.scene)){ok=false;break;}}}
    s->inOperation=false;if(!ok){s->fault(S::widget,0x209A70);s->faultDetail.page=d.page;s->faultDetail.scene=d.scene;d=s->faultDetail;return d.status;}
    ++s->state.updateCount;return result(d,S::ok);
}
S OriginalUiWinSession50PD::render(const OriginalUiMainFrameClock50KG& clock,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
    if(s->inOperation||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!s->state.updateCount||!s->state.pageShown||
       !s->state.cameraActive||(s->paused&&clock.advanceAnimations))return result(d,S::invalidInput);
    s->state.framePublished=false;s->state.routeProposalPending=false;s->inOperation=s->rendering=true;
    auto& page=s->page();bool ok=s->collector.begin({clock.known,clock.advanceAnimations,clock.milliseconds,clock.applicationAnimation},d.draw)&&
        s->collector.projectOrdinaryWidgets(true,d.draw)&&s->collector.projectPage(s->career,s->state.pageShown,d.draw);
    // 208AF4..208BA4: accent 17FA00 on widgets 3,5 (text) and 6,7,8 (icons),
    // secondary 17FA10 on widget 1. They affect the NEXT ordinary widget draw.
    OriginalUiRiderProfileColors50KG colors;
    if(ok)ok=s->cold.profile->colors(s->career,colors);
    // PORT EXTENSION (DLC career): the same 208AF4 rule on the add-on pod's own
    // colours (package ui.colors) when its career won; the anchor's otherwise.
    if(ok)if(const auto* addon=rr::game::dlc50DL::activeAddonCareer(s->career))
        if(const auto* pod=rr::assets::dlc50DL::addonPod(addon->number);pod&&pod->ui.colorsAuthored){
            colors.accent17FA00=pod->ui.accentColor;colors.secondary17FA10=pod->ui.secondaryColor;}
    if(ok)for(std::uint16_t id:{std::uint16_t(3),std::uint16_t(5),std::uint16_t(6),std::uint16_t(7),std::uint16_t(8)})
        if(!(ok=page.setWidgetColor(id,colors.accent17FA00,d.page)))break;
    if(ok)ok=page.setWidgetColor(game::originalUiWinContinue50PD,colors.secondary17FA10,d.page);
    // 208BB0: 2A6FA0 reads group1's selected slot, 308C90 formats, 2A9FF0 sets widget5.
    if(ok){game::OriginalUiWinCircuit50PD c=page.circuit();c.place=static_cast<std::uint8_t>(page.selectedPlace());
        const auto* message=s->cold.resources->message(c);
        ok=message&&page.setDynamicText(game::originalUiWinMessage50PD,*message,d.page);}
    if(ok)ok=okay(s->aperture.render48(s->transition().lifecycle))&&!s->state.faulted;
    if(ok)ok=s->collector.projectPrompts(page.promptMask42C(),d.draw)&&s->collector.drain(d.draw);
    s->inOperation=s->rendering=false;
    if(!ok){if(s->state.faulted){d=s->faultDetail;return d.status;}s->fault(S::draw,0x208AA0);s->faultDetail.draw=d.draw;s->faultDetail.page=d.page;return result(d,S::draw,0x208AA0);}
    s->state.framePublished=true;++s->state.renderCount;return result(d,S::ok);
}
OriginalUiWinSessionSnapshot50PD OriginalUiWinSession50PD::snapshot()const noexcept{
    if(!storage_)return {};auto out=storage_->state;out.transition=storage_->transition();out.aperture=storage_->aperture.snapshot();
    const auto* page=storage_->cold.resources->page();out.promptMask42C=page->promptMask42C();out.circuit=page->circuit();out.selectedPlace=page->selectedPlace();return out;
}
const OriginalUiNativeDraw50KG* OriginalUiWinSession50PD::draw()const noexcept{
    return storage_&&storage_->operational()&&storage_->state.framePublished&&!storage_->inOperation&&!storage_->updatePrepared&&storage_->currentPage()?storage_->collector.draw():nullptr;}
const OriginalUiSourceFrame50KM* OriginalUiWinSession50PD::sourceFrame()const noexcept{
    return storage_&&storage_->operational()&&storage_->state.framePublished&&!storage_->inOperation&&!storage_->updatePrepared&&storage_->currentPage()?storage_->collector.sourceFrame():nullptr;}
bool OriginalUiWinSession50PD::sourceTeardownPending()const noexcept{return storage_&&!storage_->sourceTornDown&&storage_->collector.sourceTeardownPending();}
S OriginalUiWinSession50PD::teardownSource(VehicleWindowGl50DS* r,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s||s->sourceTornDown)return result(d,S::ok);if(s->inOperation||s->rendering)return result(d,S::busy);
    s->sourceClosing=true;s->state.framePublished=false;
    if(s->collector.sourceTeardownPending()&&(!r||!s->collector.retireSourceRenderer(*r,d.draw)))return result(d,S::draw);
    s->sourceTornDown=true;return result(d,S::ok);
}
void OriginalUiWinSession50PD::requestQuit()noexcept{if(storage_&&!storage_->inOperation){storage_->state.quitRequested=true;storage_->state.framePublished=false;}}
}

#include "original_ui_unlock_session50pd.h"
#include <cmath>
#include <cstdio>
#include <limits>

namespace rr::platform {
namespace {
using S=OriginalUiUnlockSessionStatus50PD;using D=OriginalUiUnlockSessionDiagnostic50PD;
using UP=game::OriginalUiUnlockPhase50PD;using MS=game::OriginalUiOptionsMenuStatus50KG;
using N=game::OriginalUiTransitionNotificationKind50KG;using H=game::OriginalUiApertureHookKind50KG;
using AS=game::OriginalUiApertureStatus50KG;using Life=game::OriginalUiTransitionLifecycle50KG;
using Next=game::OriginalUiUnlockNext50PD;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
S result(D& d,S s,std::uint32_t pc=0) noexcept {d.status=s;d.sourcePC=pc;return s;}
bool okay(AS s) noexcept {return s==AS::ok||s==AS::noChange;}
bool stamp(OriginalUiRiderProfileStamp50KG a,OriginalUiRiderProfileStamp50KG b) noexcept {return a.generation==b.generation&&a.mode34==b.mode34;}
bool decision(const game::OriginalUiRouteDecision50KG& a,const game::OriginalUiRouteDecision50KG& b) noexcept {
    if(a.action!=b.action||a.blockers!=b.blockers||a.admitted!=b.admitted||a.profileProposalComplete!=b.profileProposalComplete||
       a.selectedSlot!=b.selectedSlot||a.selectedRider!=b.selectedRider||a.inputPC!=b.inputPC||a.request.kind!=b.request.kind||
       a.request.sourceScreen!=b.request.sourceScreen||a.request.sourcePage!=b.request.sourcePage||
       a.request.targetScreen!=b.request.targetScreen||a.request.targetPage!=b.request.targetPage||
       a.request.sourcePC!=b.request.sourcePC||a.profileChanges.count!=b.profileChanges.count||
       a.profileChanges.count>a.profileChanges.values.size())return false;
    for(std::size_t i=0;i<a.profileChanges.count;++i){const auto& x=a.profileChanges.values[i];const auto& y=b.profileChanges.values[i];
        if(x.field!=y.field||x.operation!=y.operation||x.value!=y.value||x.replaceMask!=y.replaceMask||x.sourcePC!=y.sourcePC)return false;}
    return true;
}
}
struct OriginalUiUnlockSession50PD::Storage {
    OriginalUiUnlockSessionCold50PD cold;OriginalUiUnlockSessionSnapshot50PD state{};D faultDetail{};
    assets::OriginalUiEnglish50KG english;
    game::OriginalUiUnlockState50PD nav;game::OriginalUiApertureOwner50KG aperture;OriginalUiUnlockDrawContribution50PD collector;
    OriginalUiRiderProfileStamp50KG liveStamp{},pendingStamp{};
    game::OriginalUiUnlockPlan50PD pending;OriginalUiMainSessionUpdate50KG prepared{};
    bool inOperation=false,rendering=false,updatePrepared=false,paused=false,sourceClosing=false,sourceTornDown=false;
    bool routeCommitted=false;std::uint64_t serial=0;
    const game::OriginalUiUnlockPage50PD& page()const noexcept{return *cold.resources->page();}
    game::OriginalUiUnlockWidgets50PD& bank()noexcept{return *cold.resources->widgets();}
    bool operational()const noexcept{return !state.faulted&&!state.quitRequested&&!sourceClosing&&!sourceTornDown;}
    auto transition()const noexcept{return cold.externalTransition->snapshot();}
    bool currentPage()const noexcept{const auto t=transition();return t.bound&&t.current.screen==10&&t.current.page==1;}
    bool process()const noexcept{return cold.application->known&&!(cold.application->globalFlags228&4u)&&cold.application->transportE00==-1&&
        cold.commonProfile->known&&cold.profile->commonProfile()==cold.commonProfile.get();}
    bool profileCurrent()const noexcept{return process()&&stamp(cold.profile->stamp(),liveStamp);}
    bool idle()const noexcept{const auto t=transition();return currentPage()&&t.lifecycle==Life::idle&&
        t.phase==game::OriginalUiTransitionPhase50KG::none&&!t.applicationRequestQueued&&!t.initialRestartPending;}
    bool muted()const noexcept{return cold.audioMode==OriginalUiMainAudioMode50KG::muted;}
    // 20B3E0/20B420 operate Song7 on the same frontend stream as Rider 1EA060/1EA3C0.
    bool audioReady()const noexcept{return muted()?!cold.audio:cold.audio&&cold.audio->riderMenuReady();}
    bool soundReady(std::uint32_t id)const noexcept{return audioReady()&&(muted()||cold.audio->canPlayMenuEffect(id));}
    bool sound(std::uint32_t id)noexcept{if(muted()){++state.audio.suppressedByMutedMode;return true;}
        if(!cold.audio->playMenuEffect(id)){++state.audio.failed;return false;}++state.audio.submitted;return true;}
    void fault(S v,std::uint32_t pc)noexcept{if(!state.faulted){faultDetail.status=v;faultDetail.sourcePC=pc;}state.faulted=true;state.framePublished=false;}
    // Eight 187E30 leaves as the page sees them at its activation.
    bool checks(game::OriginalUnlockChecks50PD& out,std::int32_t& rival)const noexcept{
        game::OriginalUnlockFacts50PD f;if(!cold.profile->unlockFacts187E30(f))return false;
        out=game::evaluateOriginalUnlocks187E30(f);rival=out.pending[0]?f.rival11280:-1;return true;}
    bool apply(const game::OriginalUiUnlockPlan50PD& p)noexcept{
        auto& widgets=bank();
        if(!profileCurrent()||!widgets.preflight(p.callbacks.data(),p.callbackCount,faultDetail.widget)||
           ((p.startSong7||p.fadeMusic)&&!audioReady())||
           (p.setsPageBackground8C&&cold.resources->galleryImage(p.pageBackground8C)==unresolved))return false;
        // One callback at a time: the dynamic pod/pilot owner sees each
        // callback right after the widget's generic state changed (Rider
        // OriginalUiRiderDynamicOwner50KG::apply order).
        auto& models=*cold.resources->models();
        for(std::size_t i=0;i<p.callbackCount;++i){const auto& c=p.callbacks[i];
            if(!widgets.apply(&c,1,faultDetail.widget))return false;
            if(models.dynamicWidget(c.widgetId)){const auto state=widgets.state(c.widgetId);
                if(!models.apply(c,state.selected,state.shown,faultDetail.resource.models))return false;}}
        if(p.clearScene3D8)cold.application->uiObject3D8=0;                 // 20B3AC App2D8+47C+3D8
        if(p.applyBackground430){std::uint32_t packed=0;                   // 20B3B8 global442F70+28
            for(unsigned i=0;i<3;++i){std::int32_t x=0;
                if(!game::convertOriginalUiApertureCvtWS50KG(page().page()->backgroundColor[i],x)||x<0||x>255)return false;
                packed|=std::uint32_t(x)<<(16-8*i);}
            state.packedBackground430=packed;}
        if(p.setsPageBackground8C){if(!cold.resources->setPageBackground8C(p.pageBackground8C))return false;state.pageBackground=p.pageBackground8C;}
        if(p.startSong7||p.fadeMusic){if(muted())++state.audio.suppressedByMutedMode;else{
            if(!(p.startSong7?cold.audio->startRiderLoop7():cold.audio->fadeRiderMusic()))return false;++state.audio.submitted;}}
        if(p.saveUpdateDelta&&!okay(aperture.saveUnlockUpdateDelta38(p.updateDelta)))return false;
        nav=p.state;return true;
    }
    void phase(UP p,std::uint32_t pc)noexcept{
        game::OriginalUnlockChecks50PD c;std::int32_t rival=-1;game::OriginalUiUnlockPlan50PD plan;
        if(p==UP::entry74&&!checks(c,rival)){fault(S::profile,pc);return;}
        if(!profileCurrent()||page().phase(nav,p,c,rival,plan)!=MS::ok||!apply(plan))fault(S::widget,pc);
    }
    static bool available(void* p)noexcept{auto& s=*static_cast<Storage*>(p);
        return s.operational()&&s.cold.resources->exitText()&&s.cold.resources->exitText()->prepared();}
    static void hook(void* p,const game::OriginalUiApertureHook50KG& e)noexcept{auto& s=*static_cast<Storage*>(p);if(!s.operational())return;
        switch(e.kind){
        case H::openingAC:s.phase(UP::openingAC,e.targetPC);break;
        case H::openedB0:s.phase(UP::openedB0,e.targetPC);break;
        case H::closingB4:s.phase(UP::closingB4,e.targetPC);break;
        case H::closingThresholdB8:s.phase(UP::closedB8,e.targetPC);break;
        case H::closedBC:if(e.targetPC!=0x1D7A00)s.fault(S::aperture,e.targetPC);break;
        case H::screenExitObject440:if(!s.rendering||!s.collector.projectExitText(e,s.faultDetail.draw))s.fault(S::draw,e.callPC);break;}}
    static void overlay(void* p,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& r)noexcept{auto& s=*static_cast<Storage*>(p);
        if(s.operational()&&(!s.rendering||!s.collector.projectAperture(ordinal,r,s.faultDetail.draw)))s.fault(S::draw,0x1D798C);}
    bool updateWidgets(bool top)noexcept{const auto* ids=bank().order(top);if(!ids)return false;
        for(auto id:*ids){if(bank().state(id).kind==game::OriginalUiUnlockWidgetKind50PD::text&&!bank().updateText(id,true,faultDetail.widget)&&
            faultDetail.widget.error!=game::OriginalUiUnlockWidgetError50PD::hidden)return false;}
        return true;}
    const std::string* loc(std::int32_t id)const noexcept{if(id<0)return nullptr;const auto* e=english.at(std::size_t(id));return e?&e->text:nullptr;}
    // 20B72C..20C99C: once per activation, before the sprites and 1D7550.
    bool pageText()noexcept{
        game::OriginalUiUnlockTextPlan50PD t;game::OriginalUiUnlockState50PD after;
        if(!nav.refreshText2D64)return true;
        if(!page().composeText(nav,t,after))return false;
        if(t.sourcePC==0x20C99C){nav=after;return true;} // no pending flag: only the clear
        std::uint32_t textColor=game::originalUiUnlockGalleryTextColor50PD,iconColor=game::originalUiUnlockGalleryIconColor50PD,
            continueColor=game::originalUiUnlockGalleryTextColor50PD;
        std::int32_t nameId=t.nameLoc;
        if(!t.gallery){OriginalUiRiderProfileColors50KG colors;
            if(t.colorPod<0||!cold.profile->colors(std::uint32_t(t.colorPod),colors))return false;
            textColor=iconColor=colors.accent17FA00;continueColor=colors.secondary17FA10;   // P+A8+8r / P+AC+8r
            if(t.rivalName&&!cold.profile->resultsRacerName3C(std::uint32_t(t.colorPod),nameId))return false; // P+3C[r]
        }
        const auto* header=loc(std::int32_t(t.headerLoc));const auto* name=loc(nameId);if(!header||!name)return false;
        // 308C90(sp+60,"%s@%s",...): 128-byte stack buffer at sp+60..sp+E0.
        std::string message;try{message=*header+"@"+*name;}catch(...){return false;}
        if(message.size()>=128)return false;
        if(!cold.resources->setDynamicText(game::originalUiUnlockMessageWidget50PD,message,faultDetail.resource))return false;
        auto& w=bank();auto& wd=faultDetail.widget;
        if(!w.setWidgetColor(game::originalUiUnlockTitleWidget50PD,textColor,wd)||!w.setWidgetColor(game::originalUiUnlockMessageWidget50PD,textColor,wd))return false;
        for(auto id:game::originalUiUnlockBandWidgets50PD)if(!w.setWidgetColor(id,iconColor,wd))return false;
        if(!w.setWidgetColor(game::originalUiUnlockContinueWidget50PD,continueColor,wd))return false;
        nav=after;return true;
    }
};
OriginalUiUnlockSession50PD::OriginalUiUnlockSession50PD() noexcept=default;
OriginalUiUnlockSession50PD::~OriginalUiUnlockSession50PD()=default;
S OriginalUiUnlockSession50PD::bind(const OriginalUiUnlockSessionCold50PD& cold,D& d) noexcept {
    d={};if(storage_)return result(d,S::alreadyBound);
    if(!cold.resources||!cold.resources->loaded()||!cold.resources->page()||!cold.resources->widgets()||!cold.resources->exitText()||
       !cold.resources->exitText()->prepared()||!cold.gpu||!cold.unlockArchive||
       cold.unlockArchive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::unlock)||
       !cold.english||!cold.english->loaded())return result(d,S::resource);
    if(!cold.externalTransition||cold.externalTransition->snapshot().bound||!cold.profile||!cold.profile->hallRecordsReady()||
       !cold.commonProfile||!cold.commonProfile->known||cold.profile->commonProfile()!=cold.commonProfile.get()||
       !cold.application||!cold.application->known||(cold.application->globalFlags228&4)||cold.application->transportE00!=-1)
        return result(d,S::profile);
    // 209CCC / 20AE68 debug branches are not native behaviour here.
    if(!cold.application344||*cold.application344!=0)return result(d,S::unsupportedState,0x20AE68);
    if((cold.audioMode!=OriginalUiMainAudioMode50KG::muted&&cold.audioMode!=OriginalUiMainAudioMode50KG::enabled)||
       (cold.audioMode==OriginalUiMainAudioMode50KG::muted?bool(cold.audio):!cold.audio||!cold.audio->riderMenuReady()))return result(d,S::audio);
    try {
        auto s=std::make_unique<Storage>();s->cold=cold;s->cold.english=nullptr;s->english=*cold.english;s->liveStamp=cold.profile->stamp();
        game::OriginalUnlockFacts50PD facts;if(!cold.profile->unlockFacts187E30(facts))return result(d,S::profile);
        if(!s->page().initialState(s->nav))return result(d,S::widget);
        if(!s->collector.bindFrame(cold.resources,cold.gpu,d.draw))return result(d,S::draw);
        // Process-shared playback/IK/libm owners and the shared RNG word.
        auto* app=cold.application.get();
        if(!cold.resources->models()||!cold.resources->models()->bindLive({&app->modelMotion,&app->modelMode0Ik,&app->modelLibm,
            &app->sharedNoise308030},d.resource.models))return result(d,S::resource);
        game::OriginalUiApertureCallbacks50KG cb;cb.context=s.get();cb.available=&Storage::available;cb.hook=&Storage::hook;cb.submitOverlay=&Storage::overlay;
        if(s->aperture.bindUnlock(cold.unlockArchive,cb,d.aperture)!=AS::ok)return result(d,S::aperture);
        const auto* config=s->aperture.config();
        // unlock.dat PageInfo: OpenSize=69, Time=0.5, Start=Closed (310 px/s).
        if(!config||config->openSize274!=69||config->time4B0!=0.5f||!config->startClosed)return result(d,S::aperture);
        s->state.bound=true;storage_=std::move(s);return result(d,S::ok);
    }catch(...){return result(d,S::resource);}
}
S OriginalUiUnlockSession50PD::notifyTransition(const game::OriginalUiTransitionNotification50KG& e,D& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    // The page reads the profile at its own entry (20AE40). A screen shown
    // before it in the same frontend (the tournament Hall of Fame: record
    // insertion and name entry) legitimately changes the profile, so the
    // not-yet-shown page takes its profile baseline at its page request, as
    // the Win page does.
    if(e.kind==N::requestedPage90&&!s->state.pageShown&&s->operational()&&!s->inOperation&&
       s->process()&&e.route.screen==10&&e.route.page==1)
        s->liveStamp=s->cold.profile->stamp();
    if(!s->operational()||s->inOperation||!s->profileCurrent()||e.route.screen!=10||e.route.page!=1)return result(d,S::unsupportedState,e.sourcePC);
    const auto t=s->transition();
    if(!t.bound||e.lifecycle!=t.lifecycle||e.phase!=t.phase||!((t.current.screen==10&&t.current.page==1)||(t.requested.screen==10&&t.requested.page==1)))
        return result(d,S::transition,e.sourcePC);
    s->state.framePublished=false;s->state.routeProposalPending=false;s->inOperation=true;
    switch(e.kind){
    case N::requestedPage90:s->phase(UP::requestedPage90,e.sourcePC);s->state.pageShown=true;break;
    case N::previousPage94:s->phase(UP::previousPage94,e.sourcePC);s->state.pageShown=false;break;
    case N::pageEntry74:s->routeCommitted=false;s->phase(UP::entry74,e.sourcePC);break;  // 20AE40
    case N::managerEntry84:break;
    case N::beginTransition7C:if(!okay(s->aperture.beginTransition7C(e.phase)))s->fault(S::aperture,e.sourcePC);break;
    case N::pageAuxiliary1C:s->state.inputContainerShown=false;break;
    case N::pageAuxiliary24:s->state.inputContainerShown=true;break;
    }
    s->inOperation=false;if(s->state.faulted){d=s->faultDetail;return d.status;}return result(d,S::ok);
}
game::OriginalUiTransitionCompletion50KG OriginalUiUnlockSession50PD::pollCompletion(game::OriginalUiTransitionRoute50KG r,
    game::OriginalUiTransitionPhase50KG p) const noexcept {
    const auto* s=storage_.get();
    if(!s||!s->operational()||s->inOperation||!s->profileCurrent()||r.screen!=10||r.page!=1)return game::OriginalUiTransitionCompletion50KG::unavailable;
    const auto t=s->transition();if(!s->currentPage()||t.phase!=p)return game::OriginalUiTransitionCompletion50KG::unavailable;
    return s->aperture.pollCompletion(r,p);
}
const game::OriginalUiTransitionOwner50KG* OriginalUiUnlockSession50PD::externalLifecycleOwner() const noexcept {return storage_?storage_->cold.externalTransition.get():nullptr;}
bool OriginalUiUnlockSession50PD::ownsPreparedPage(std::uint8_t p) const noexcept {return storage_&&storage_->operational()&&!storage_->inOperation&&p==1;}
S OriginalUiUnlockSession50PD::input(const OriginalUiMainSessionInput50KG& in,OriginalUiUnlockSessionInputReceipt50PD& r,D& d) noexcept {
    d={};r={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational())return result(d,s->state.quitRequested?S::quit:S::faulted);
    if(s->inOperation||s->updatePrepared||in.controller>1||in.originalInput>15)return result(d,S::invalidInput);
    s->state.routeProposalPending=false;s->routeCommitted=false;
    r.containerReached=s->currentPage()&&s->state.pageShown&&s->state.inputContainerShown;
    // Container input only reaches the page while the doors are fully open (+268).
    if(!r.containerReached||!s->aperture.snapshot().inputEnabled268||in.popupActive||in.sourceInputBlocked)return result(d,r.status=S::noChange);
    if(!s->profileCurrent())return result(d,r.status=S::profile);
    game::OriginalUiUnlockPlan50PD plan;
    r.pageCallbackRan=true;
    const auto status=s->page().input(s->nav,in.controller,in.originalInput,s->cold.commonProfile->mode34,plan);r.pageAccepted=plan.pageAccepted;
    if(status==MS::noChange)return result(d,r.status=S::noChange);
    if(status!=MS::unavailable||plan.next==Next::none)return result(d,r.status=S::widget);
    if(!s->idle()||!s->soundReady(plan.soundId)||!game::originalUiUnlockRouteShape50PD(plan.route,plan.next))
        return result(d,r.status=S::unavailableDestination);
    s->pending=plan;s->pendingStamp=s->liveStamp;s->state.routeProposalPending=true;++s->serial;if(!s->serial)++s->serial;
    r.routeProposal={this,s->serial,plan.next,plan.route,plan.grantRequested,plan.grantCheck,plan.grantTier,s->pendingStamp};
    return result(d,r.status=S::routeProposed);
}
bool OriginalUiUnlockSession50PD::routeProposalCurrent(const OriginalUiUnlockRouteProposal50PD& p) const noexcept {
    const auto* s=storage_.get();
    if(!s||!s->operational()||s->inOperation||s->updatePrepared||s->routeCommitted||!s->state.routeProposalPending||p.source!=this||
       !p.serial||p.serial!=s->serial||!stamp(p.profileStamp,s->pendingStamp)||!stamp(p.profileStamp,s->liveStamp)||!s->profileCurrent()||
       !s->idle()||p.next!=s->pending.next||!decision(p.decision,s->pending.route)||p.grant!=s->pending.grantRequested||
       p.grantCheck!=s->pending.grantCheck||p.grantTier!=s->pending.grantTier||!s->soundReady(s->pending.soundId))return false;
    // The flag being granted must still be pending on the actual profile.
    if(p.grant){game::OriginalUnlockChecks50PD c;std::int32_t rival=-1;
        if(!s->checks(c,rival)||p.grantCheck>=c.pending.size()||!c.pending[p.grantCheck]||(p.grantCheck==7&&c.galleryTier!=p.grantTier))return false;}
    game::OriginalUiUnlockPlan50PD expected;
    return s->page().input(s->nav,0,4,s->cold.commonProfile->mode34,expected)==MS::unavailable&&expected.next==p.next&&
        decision(expected.route,p.decision);
}
bool OriginalUiUnlockSession50PD::commitRoute(const OriginalUiUnlockRouteProposal50PD& p,const game::OriginalUiRouteDecision50KG& admitted) noexcept {
    if(!routeProposalCurrent(p)||!admitted.admitted||admitted.blockers)return false;
    auto expected=p.decision;expected.admitted=true;expected.blockers=0;expected.action=game::OriginalUiRouteAction50KG::request;
    if(!decision(expected,admitted))return false;
    auto& s=*storage_;
    if(p.grant){OriginalUiRiderProfileStamp50KG after{};
        if(!s.cold.profile->grantUnlock20B480(p.grantCheck,p.grantTier,s.liveStamp,after))return false;
        s.liveStamp=after;++s.state.grants;}
    s.nav=s.pending.state;s.routeCommitted=true;return true;
}
S OriginalUiUnlockSession50PD::notifyRouteAccepted(const OriginalUiUnlockRouteProposal50PD& p,D& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||s->inOperation||s->updatePrepared||!s->routeCommitted||!s->state.routeProposalPending||p.source!=this||
       p.serial!=s->serial||!decision(p.decision,s->pending.route)||!s->profileCurrent())return result(d,S::invalidInput);
    const auto t=s->transition();const auto& req=p.decision.request;
    const bool reshow=p.next==Next::reshow;
    if(!s->currentPage()||t.requested.screen!=req.targetScreen||t.requested.page!=req.targetPage||
       t.lifecycle!=(reshow?Life::requestPageExit:Life::requestScreenExit)||t.phase!=game::OriginalUiTransitionPhase50KG::none||
       t.applicationRequestQueued||t.initialRestartPending)return result(d,S::transition);
    s->state.framePublished=false;s->state.routeProposalPending=false;
    // 20B6D0 292360(App2D8+40,1,0) then 20B6E8 267BE0 (flush), on both paths.
    if(!s->sound(s->pending.soundId)){s->fault(S::audio,s->pending.soundPC);return result(d,S::audio);}
    return result(d,S::ok);
}
S OriginalUiUnlockSession50PD::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG& in,D& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
    if(s->inOperation||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!std::isfinite(in.elapsedSeconds)||
       in.elapsedSeconds<0||in.elapsedSeconds>1||(in.paused&&in.elapsedSeconds!=0))return result(d,S::invalidInput);
    s->state.framePublished=false;s->state.routeProposalPending=false;s->paused=in.paused;
    if(!s->muted()){s->cold.audio->pause(in.paused);if(!s->cold.audio->update(in.elapsedSeconds)){s->fault(S::audio,0x234440);return result(d,S::audio);}}
    s->prepared=in;s->updatePrepared=true;return result(d,S::ok);
}
S OriginalUiUnlockSession50PD::update(const OriginalUiMainSessionUpdate50KG& in,D& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
    if(s->inOperation||!s->currentPage()||!s->profileCurrent()||!s->updatePrepared||in.elapsedSeconds!=s->prepared.elapsedSeconds||
       in.paused!=s->prepared.paused||in.debugChordHeld!=s->prepared.debugChordHeld)return result(d,S::invalidInput);
    s->updatePrepared=false;s->inOperation=true;bool ok=true;
    // 20CA30 stores dt at +298, then the generic child update 2A6BB0.
    if(s->state.pageShown){game::OriginalUiUnlockPlan50PD plan;ok=s->page().update(s->nav,in.elapsedSeconds,plan)==MS::ok&&s->apply(plan);}
    if(ok)ok=s->updateWidgets(false)&&s->updateWidgets(true);
    // Same child update: User_Rotate 1D38B0 (0.05 rev/s about Y) and pilot playback 1D2730.
    if(ok)ok=s->cold.resources->models()->advance(in.elapsedSeconds,s->state.pageShown,s->faultDetail.resource.models);
    s->inOperation=false;if(!ok){s->fault(S::widget,0x20CA30);d=s->faultDetail;return d.status;}
    ++s->state.updateCount;return result(d,S::ok);
}
S OriginalUiUnlockSession50PD::render(const OriginalUiMainFrameClock50KG& clock,D& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);if(!s->operational())return result(d,S::faulted);
    if(s->inOperation||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!s->state.updateCount||!s->state.pageShown||
       (s->paused&&clock.advanceAnimations))return result(d,S::invalidInput);
    s->state.framePublished=false;s->state.routeProposalPending=false;s->inOperation=s->rendering=true;
    bool ok=s->collector.begin({clock.known,clock.advanceAnimations,clock.milliseconds,clock.applicationAnimation},d.draw)&&
        s->collector.projectOrdinaryWidgets(true,d.draw);
    // Page +48 20B710: one-shot text/colour work, sprites, then 1D7550.
    if(ok&&s->state.pageShown&&!s->pageText()){s->inOperation=s->rendering=false;s->fault(S::widget,0x20B710);d=s->faultDetail;return d.status;}
    if(ok)ok=s->collector.projectPage(s->nav.pod2D40,s->state.pageShown,d.draw);
    if(ok)ok=okay(s->aperture.render48(s->transition().lifecycle))&&!s->state.faulted;
    if(ok)ok=s->collector.projectPrompts(s->nav.inputFlags42C,d.draw)&&s->collector.projectOnTopWidgets(true,d.draw)&&s->collector.drain(d.draw);
    s->inOperation=s->rendering=false;
    if(!ok){if(s->state.faulted){d=s->faultDetail;return d.status;}s->fault(S::draw,0x20B710);return result(d,S::draw,0x20B710);}
    s->state.framePublished=true;++s->state.renderCount;return result(d,S::ok);
}
OriginalUiUnlockSessionSnapshot50PD OriginalUiUnlockSession50PD::snapshot() const noexcept {
    if(!storage_)return {};auto out=storage_->state;out.transition=storage_->transition();out.aperture=storage_->aperture.snapshot();
    out.pod2D40=storage_->nav.pod2D40;out.promptMask42C=storage_->nav.inputFlags42C;out.activations=storage_->nav.activations;
    out.pending=storage_->nav.pending;out.routeCommitted=storage_->routeCommitted;return out;
}
const game::OriginalUiUnlockState50PD* OriginalUiUnlockSession50PD::pageState() const noexcept {return storage_?&storage_->nav:nullptr;}
const OriginalUiNativeDraw50KG* OriginalUiUnlockSession50PD::draw() const noexcept {
    return storage_&&storage_->operational()&&storage_->state.framePublished&&!storage_->inOperation&&!storage_->updatePrepared&&
        storage_->currentPage()?storage_->collector.draw():nullptr;
}
const OriginalUiSourceFrame50KM* OriginalUiUnlockSession50PD::sourceFrame() const noexcept {
    return storage_&&storage_->operational()&&storage_->state.framePublished&&!storage_->inOperation&&!storage_->updatePrepared&&
        storage_->currentPage()?storage_->collector.sourceFrame():nullptr;
}
bool OriginalUiUnlockSession50PD::sourceTeardownPending() const noexcept {return storage_&&!storage_->sourceTornDown&&storage_->collector.sourceTeardownPending();}
S OriginalUiUnlockSession50PD::teardownSource(VehicleWindowGl50DS* r,D& d) noexcept {
    d={};auto* s=storage_.get();if(!s||s->sourceTornDown)return result(d,S::ok);if(s->inOperation||s->rendering)return result(d,S::busy);
    s->sourceClosing=true;s->state.framePublished=false;
    if(s->collector.sourceTeardownPending()&&(!r||!s->collector.retireSourceRenderer(*r,d.draw)))return result(d,S::draw);
    // After renderer retirement: every live pod/pilot graph, helper and membership.
    if(!s->cold.resources->models()->drain(d.resource.models))return result(d,S::resource);
    s->sourceTornDown=true;return result(d,S::ok);
}
void OriginalUiUnlockSession50PD::requestQuit() noexcept {if(storage_&&!storage_->inOperation){storage_->state.quitRequested=true;storage_->state.framePublished=false;}}
}

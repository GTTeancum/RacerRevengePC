// RECONSTRUCTED FROM VISIBLE TASK CONTEXT AFTER WORKSPACE ROLLBACK.
// Not byte-verified against lost candidate. Popup cloning/disable owner joined;
// source-only review still required before destination admission.
#include "original_ui_course_session50kg.h"
#include <cmath>
#include <limits>
#include <utility>
#include "../../../src/assets/original_ui_course_background50kg.h"
namespace rr::platform {
namespace {
using S=OriginalUiCourseSessionStatus50KG;using D=OriginalUiCourseSessionDiagnostic50KG;
using Phase=game::OriginalUiTransitionPhase50KG;using Life=game::OriginalUiTransitionLifecycle50KG;
using Notice=game::OriginalUiTransitionNotificationKind50KG;using Completion=game::OriginalUiTransitionCompletion50KG;
using WP=game::OriginalUiCourseWidgetPhase50KG;using WK=game::OriginalUiCourseWidgetKind50KG;
using Hook=game::OriginalUiApertureHookKind50KG;using AS=game::OriginalUiApertureStatus50KG;
S result(D& d,S s,std::uint32_t pc=0)noexcept{d.status=s;d.sourcePC=pc;return s;}
bool apertureOkay(AS s)noexcept{return s==AS::ok||s==AS::noChange;}
bool sameStamp(OriginalUiRiderProfileStamp50KG a,OriginalUiRiderProfileStamp50KG b)noexcept{return a.generation==b.generation&&a.mode34==b.mode34;}
bool sameDecision(const game::OriginalUiRouteDecision50KG& a,const game::OriginalUiRouteDecision50KG& b)noexcept{
    if(a.action!=b.action||a.blockers!=b.blockers||a.selectedSlot!=b.selectedSlot||a.selectedRider!=b.selectedRider||
       a.inputPC!=b.inputPC||a.profileProposalComplete!=b.profileProposalComplete||a.admitted!=b.admitted||
       a.request.kind!=b.request.kind||a.request.sourceScreen!=b.request.sourceScreen||a.request.sourcePage!=b.request.sourcePage||
       a.request.targetScreen!=b.request.targetScreen||a.request.targetPage!=b.request.targetPage||a.request.sourcePC!=b.request.sourcePC||
       a.profileChanges.count!=b.profileChanges.count)return false;
    for(std::size_t i=0;i<a.profileChanges.count;++i){const auto& x=a.profileChanges.values[i];const auto& y=b.profileChanges.values[i];
        if(x.field!=y.field||x.operation!=y.operation||x.value!=y.value||x.replaceMask!=y.replaceMask||x.sourcePC!=y.sourcePC)return false;}
    return true;
}
bool sameLife(const game::OriginalUiTransitionSnapshot50KG& a,const game::OriginalUiTransitionSnapshot50KG& b)noexcept{
    return a.bound==b.bound&&a.current.screen==b.current.screen&&a.current.page==b.current.page&&
        a.requested.screen==b.requested.screen&&a.requested.page==b.requested.page&&a.lifecycle==b.lifecycle&&
        a.phase==b.phase&&a.applicationRequestQueued==b.applicationRequestQueued&&a.initialRestartPending==b.initialRestartPending;
}
bool bounded(const game::OriginalUiCourseRememberedProfile50KG& p,const OriginalUiMainProfile50KG& common)noexcept{
    constexpr std::array<std::uint8_t,13> order{{0,11,4,8,7,1,5,9,12,6,10,3,2}};
    return p.ownerReady&&p.mode34==0&&common.mode34==0&&p.course700>=1&&p.course700<=13&&p.subtype70C==1&&
        p.laps704>=1&&p.laps704<=25&&p.entrants708==8&&!p.cardFlag100BC&&(p.availability11018&31u)>=1&&(p.availability11018&31u)<=13&&
        ((common.playerBits1125C>>5)&3)==1&&p.courseOrder100C4==order&&
        p.planet0Primary79C==0xc2a684&&p.planet0Secondary7A0==0x80674a;
}
}
struct OriginalUiCourseSession50KG::Storage {
    OriginalUiCourseSessionCold50KG cold;OriginalUiCourseSessionSnapshot50KG state{};D faultDetail{};
    bool sebulbaTbTest=false;
    bool kraidGbTest=false;
    bool youngAnakinMcTest=false;
    bool scorchRcTest=false;
    bool tzidikTcTest=false;
    bool gasganoScTest=false; // Captured from the retained owner at bind.
    OriginalUiCourseDrawContribution50KG collector;
    game::OriginalUiCourseSelectionData50KG selection;
    game::OriginalUiCoursePageDraw50KG page;
    game::OriginalUiCourseCamera50KG camera;
    game::OriginalUiCourseRenderContext50KG renderContext;
    game::OriginalUiCourseLighting50KG lighting;
    game::OriginalUiApertureOwner50KG aperture;
    OriginalUiRiderProfileStamp50KG coldStamp{},liveStamp{};
    bool published=false,requested=false,entered=false,managerEntered=false;
    bool opening=false,opened=false,entryAuxiliary=false,entryCompleted=false;
    bool closing=false,restored=false,closed=false;
    std::uint64_t proposalSerial=0;
    game::OriginalUiRouteDecision50KG pendingDecision{};
    game::OriginalUiCourseContext50KG pendingContext{};
    game::OriginalUiTransitionSnapshot50KG pendingLifecycle{};
    OriginalUiRiderProfileStamp50KG pendingStamp{};
    bool rendering=false,inOperation=false,updatePrepared=false,paused=false;
    float preparedDelta=0;
    std::array<std::string,6> planetNamesBA0;
    bool selectionRefreshCompleted=false;
    game::OriginalUiCourseWidgets50KG& widgets()noexcept{return *cold.resources->widgets();}
    bool exitAccepted()const noexcept{return state.backAccepted||state.launchAccepted;}
    bool operational()const noexcept{return !state.faulted&&!state.quitRequested;}
    bool audioReady()const noexcept{return cold.audioMode==OriginalUiMainAudioMode50KG::muted?!cold.audio:
        cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&cold.audio&&cold.audio->courseMenuReady();}
    bool effectReady(std::uint32_t id)const noexcept{return audioReady()&&
        (cold.audioMode==OriginalUiMainAudioMode50KG::muted||cold.audio->canPlayCourseEffect(id));}
    bool effect(std::uint32_t id,std::uint32_t pc)noexcept{
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted&&!cold.audio){++state.audio.suppressedByMutedMode;return true;}
        if(!audioReady()||!cold.audio->playCourseEffect(id)){++state.audio.failed;fault(S::audio,pc);return false;}
        ++state.audio.submitted;return true;
    }
    bool processCurrent()const noexcept{return cold.application->known&&cold.commonProfile->known&&
        cold.profile->commonProfile()==cold.commonProfile.get()&&audioReady()&&
        !(cold.application->globalFlags228&4)&&cold.application->transportE00==-1&&
        &cold.application->rawRandom308030==&cold.application->sharedNoise308030.sharedRandomState;}
    auto transition()const noexcept{return cold.externalTransition->snapshot();}
    bool currentPage()const noexcept{const auto t=transition();return t.bound&&t.current.screen==6&&t.current.page==1;}
    bool lightCurrent()const noexcept{return cold.lightManager->activeUiIdentity()==cold.courseArchive->identity()&&
        cold.lightManager->cacheState()==game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction&&
        state.registryGeneration&&cold.lightManager->managerGeneration()==state.registryGeneration&&cold.lightManager->courseRegistryReady();}
    bool popupCurrent()const noexcept{
        const auto* w=cold.resources->widgets();
        return w&&w->popupClones438()&&w->popupTemplatesHidden438()&&!w->popupEnabled438();
    }
    bool testProfileCurrent(const game::OriginalUiCourseRememberedProfile50KG& p)const noexcept{
        return p.youngAnakinMcTest==youngAnakinMcTest&&p.youngAnakinMcTest==cold.profile->youngAnakinMcTestProfile()&&p.kraidGbTest==kraidGbTest&&p.kraidGbTest==cold.profile->kraidGbTestProfile()&&p.sebulbaTbTest==sebulbaTbTest&&p.sebulbaTbTest==cold.profile->sebulbaTbTestProfile()&&p.scorchRcTest==scorchRcTest&&p.scorchRcTest==cold.profile->scorchRcTestProfile()&&p.tzidikTcTest==tzidikTcTest&&p.tzidikTcTest==cold.profile->tzidikTcTestProfile()&&p.gasganoScTest==gasganoScTest&&p.gasganoScTest==cold.profile->gasganoScTestProfile();}
    bool profileCurrent()const noexcept{game::OriginalUiCourseRememberedProfile50KG p;
        return cold.profile->courseSnapshot(p)&&bounded(p,*cold.commonProfile)&&testProfileCurrent(p)&&
            (!entered||sameStamp(cold.profile->stamp(),liveStamp))&&(!entered||(p.packed1125E>>5)==state.selectedPlanet40);}
    void fault(S status,std::uint32_t pc)noexcept{if(!state.faulted){faultDetail.status=status;faultDetail.sourcePC=pc;}state.faulted=true;state.framePublished=false;}
    void cpuDrain()noexcept{collector.drain();state.framePublished=false;}
    bool idleInput()const noexcept{
        const auto t=transition();const auto a=aperture.snapshot();
        return currentPage()&&entryCompleted&&opened&&!closing&&!exitAccepted()&&state.pageShown&&
            state.inputContainerShown&&t.requested.screen==6&&t.requested.page==1&&t.lifecycle==Life::idle&&
            t.phase==Phase::none&&!t.initialRestartPending&&a.inputEnabled268&&a.state==game::OriginalUiApertureState50KG::open;
    }
    bool context(game::OriginalUiCourseContext50KG& c)const noexcept{
        if(!profileCurrent()||!cold.resources->widgets()->navigation()||!selection.page())return false;
        const auto* n=cold.resources->widgets()->navigation();
        // Source Confirm/Back dispatch does not require course-row focus.
        if(n->controllerGroup[0]<1||n->controllerGroup[0]>3||state.laps14EC<1||state.laps14EC>25||
            n->groups[1].selectedSlot!=state.courseSlot14E4||
            n->groups[2].selectedSlot!=state.subtypeSlot14F4)return false;
        game::OriginalUiCourseControl50KG control;
        if(!selection.control(static_cast<std::uint16_t>(state.courseSlot14E4),control)||
            control.planet40!=state.selectedPlanet40||control.course44!=state.selectedCourse44)return false;
        c={};c.admission.sourceControllerReady=true;c.admission.selectionOwnerReady=true;c.admission.profileOwnerReady=true;
        OriginalUiRiderProfileOwner50KG::CourseRaceSelection retained;
        if(!cold.profile->courseRaceSnapshot(retained))return false;
        c.mode34=0;c.packed1125C=retained.packed1125C;c.firstControllerPresent=true;
        c.confirmEnabled42C=(state.promptMask42C&1)!=0;c.courseSlot14E4=static_cast<std::uint16_t>(state.courseSlot14E4);
        c.subtypeSlot14F4=static_cast<std::uint16_t>(state.subtypeSlot14F4);
        c.courseGroupSlotCount=static_cast<std::uint16_t>(selection.page()->groups[1].controls.size());
        c.subtypeGroupSlotCount=static_cast<std::uint16_t>(selection.page()->groups[2].controls.size());
        c.laps14EC=state.laps14EC;c.entrants14F0=state.entrants14F0;c.courseControlDataReady=true;c.selectedControlWord44=control.course44;
        return true;
    }
    bool widgetProfile(game::OriginalUiCourseWidgetProfile50KG& out)const noexcept{
        game::OriginalUiCourseRememberedProfile50KG p;if(!cold.profile->courseSnapshot(p)||!bounded(p,*cold.commonProfile)||!testProfileCurrent(p))return false;
        out={};out.mawhonicRylothTest=p.mawhonicRylothTest;out.wanMbTest=p.wanMbTest;out.gasganoScTest=p.gasganoScTest;out.tzidikTcTest=p.tzidikTcTest;out.scorchRcTest=p.scorchRcTest;out.sebulbaTbTest=p.sebulbaTbTest;out.kraidGbTest=p.kraidGbTest;out.youngAnakinMcTest=p.youngAnakinMcTest;out.mode34=p.mode34;out.course700=p.course700;out.subtype70C=p.subtype70C;
        out.flags100BC=p.cardFlag100BC?1:0;out.flags11018=p.availability11018;
        out.flags1125C=cold.commonProfile->playerBits1125C;out.courseRecords100C4=p.courseOrder100C4;return true;
    }
    static bool preflight(void* context,const game::OriginalUiCourseWidgetStep50KG* steps,std::size_t count)noexcept{
        auto& s=*static_cast<Storage*>(context);if(!s.operational()||!s.processCurrent()||(!steps&&count))return false;
        for(std::size_t i=0;i<count;++i){const auto& step=steps[i];
            if(s.cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&
               ((step.kind==game::OriginalUiCourseWidgetStepKind50KG::audioStartLoop11&&!s.cold.audio->canStartCourseLoop11())||
                (step.kind==game::OriginalUiCourseWidgetStepKind50KG::audioFade1&&!s.cold.audio->canFadeCourseMusic())))return false;
            if(step.kind==game::OriginalUiCourseWidgetStepKind50KG::widget&&s.widgets().state(step.widget).kind==WK::character){
                auto* model=s.cold.holograms->model(step.widget);
                if(!model||!model->preflightApply(step.operation,step.value,s.cold.application->modelMotion,
                    s.cold.application->modelMode0Ik,s.faultDetail.model))return false;
            }
        }return true;
    }
    static bool character(void* context,const game::OriginalUiCourseWidgetStep50KG& step,game::OriginalUiCourseWidgetState50KG& state)noexcept{
        auto& s=*static_cast<Storage*>(context);auto* model=s.cold.holograms->model(step.widget);
        if(!model||!model->apply(step.operation,step.value,s.cold.application->modelMotion,s.cold.application->modelMode0Ik,s.faultDetail.model))return false;
        state.shown=model->shown();state.selected=model->selected();return true;
    }
    static bool audio(void* context,const game::OriginalUiCourseWidgetStep50KG& step)noexcept{
        auto& s=*static_cast<Storage*>(context);
        const bool start=step.kind==game::OriginalUiCourseWidgetStepKind50KG::audioStartLoop11;
        if(!start&&step.kind!=game::OriginalUiCourseWidgetStepKind50KG::audioFade1)return false;
        if(s.cold.audioMode==OriginalUiMainAudioMode50KG::muted&&!s.cold.audio){++s.state.audio.suppressedByMutedMode;return true;}
        if(!s.audioReady()||!(start?s.cold.audio->startCourseLoop11():s.cold.audio->fadeCourseMusic())){
            ++s.state.audio.failed;s.fault(S::audio,step.sourcePC);return false;}
        ++s.state.audio.submitted;return true;
    }
    game::OriginalUiCourseWidgetHooks50KG hooks()noexcept{return {this,&preflight,&character,&audio};}
    bool widgetPhase(WP phase)noexcept{
        game::OriginalUiCourseWidgetProfile50KG p;if(!widgetProfile(p)){fault(S::profile,0x1DA4A0);return false;}
        if(!widgets().phase(phase,p,hooks(),faultDetail.widget)){fault(S::widget,0x1DA7A0);return false;}
        state.promptMask42C=widgets().promptMask42C();return true;
    }
    static bool available(void* context)noexcept{
        auto& s=*static_cast<Storage*>(context);auto* exit=s.cold.resources->exitText();
        return s.operational()&&s.processCurrent()&&s.popupCurrent()&&s.page.prepared()&&exit&&exit->prepared();
    }
    static void hook(void* context,const game::OriginalUiApertureHook50KG& e)noexcept{
        auto& s=*static_cast<Storage*>(context);if(!s.operational())return;
        switch(e.kind){
        case Hook::openingAC:
            if(e.targetPC!=0x1DA7A0||!s.entered||s.opening||s.widgets().popupEnabled438())s.fault(S::aperture,e.callPC);
            else if(s.widgetPhase(WP::openingAC))s.opening=true;break;
        case Hook::openedB0:
            if(e.targetPC!=0x1DA950||!s.opening||s.opened)s.fault(S::aperture,e.callPC);
            else if(s.widgetPhase(WP::openedB0))s.opened=true;break;
        case Hook::closingB4:
            if(e.targetPC!=0x1DAA70||!s.exitAccepted()||s.closing)s.fault(S::aperture,e.callPC);
            else if(s.widgetPhase(WP::closingB4))s.closing=true;break;
        case Hook::closingThresholdB8:
            if(e.targetPC!=0x1DAAC0||!s.closing||s.restored)s.fault(S::aperture,e.callPC);
            else if(s.widgetPhase(WP::closedB8))s.restored=true;break;
        case Hook::closedBC:
            if(e.targetPC!=0x1D7A00||!s.closing||!s.restored||s.closed)s.fault(S::aperture,e.callPC);else s.closed=true;break;
        case Hook::screenExitObject440:
            if(!s.rendering||!s.closing||!s.collector.projectExitText(e,s.faultDetail.draw))s.fault(S::draw,e.callPC);break;
        }
    }
    static void overlay(void* context,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& r)noexcept{
        auto& s=*static_cast<Storage*>(context);
        if(!s.rendering||!s.collector.projectAperture(ordinal,r,s.faultDetail.draw))s.fault(S::draw,0x1D798C);
    }
    bool updateWidgets(const std::vector<std::uint16_t>& order,float dt)noexcept{
        for(const auto id:order){const auto kind=widgets().state(id).kind;
            if(kind==WK::text){if(!widgets().updateText(id,true,faultDetail.widget)&&faultDetail.widget.error!=game::OriginalUiCourseWidgetError50KG::hidden){fault(S::widget,0x141ED0);return false;}}
            else if(kind==WK::character){auto* m=cold.holograms->model(id);
                if(!m||!m->advance(dt,cold.application->modelMotion,cold.application->modelMode0Ik,faultDetail.model)){fault(S::model,0x2A8BB0);return false;}}
        }return true;
    }
};
OriginalUiCourseSession50KG::OriginalUiCourseSession50KG()noexcept=default;
OriginalUiCourseSession50KG::~OriginalUiCourseSession50KG()=default;
S OriginalUiCourseSession50KG::bind(const OriginalUiCourseSessionCold50KG& cold,D& d)noexcept{
    d={};if(storage_)return result(d,S::alreadyBound);
    if(!cold.resources||!cold.resources->loaded()||!cold.resources->widgets()||!cold.resources->widgets()->popupClones438()||
       !cold.resources->widgets()->popupTemplatesHidden438()||cold.resources->widgets()->popupEnabled438()||!cold.resources->video()||
       !cold.holograms||!cold.holograms->loaded()||!cold.courseArchive||!cold.english||!cold.english->loaded()||
       !cold.profile||!cold.profile->initialized()||!cold.commonProfile||!cold.application||!cold.lightManager||
       !cold.externalTransition||cold.externalTransition->snapshot().bound||cold.previousScreen!=5)return result(d,S::unsupportedState);
    if((cold.audioMode==OriginalUiMainAudioMode50KG::muted&&cold.audio)||
       (cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&(!cold.audio||!cold.audio->courseMenuReady()))||
       (cold.audioMode!=OriginalUiMainAudioMode50KG::muted&&cold.audioMode!=OriginalUiMainAudioMode50KG::enabled))
        return result(d,S::audio,0x1DA924);
    try{
        auto s=std::make_unique<Storage>();s->cold=cold;s->cold.english=nullptr;s->coldStamp=cold.profile->stamp();
        s->gasganoScTest=cold.profile->gasganoScTestProfile();
        s->tzidikTcTest=cold.profile->tzidikTcTestProfile();
        s->scorchRcTest=cold.profile->scorchRcTestProfile();
        s->sebulbaTbTest=cold.profile->sebulbaTbTestProfile();
        s->kraidGbTest=cold.profile->kraidGbTestProfile();
        s->youngAnakinMcTest=cold.profile->youngAnakinMcTestProfile();
        if(!s->processCurrent()||!s->profileCurrent()||s->coldStamp.generation>std::numeric_limits<std::uint64_t>::max()-2)return result(d,S::profile);
        if(!s->selection.load(cold.courseArchive,d.selection))return result(d,S::resource);
        game::OriginalUiCourseRememberedProfile50KG p;game::OriginalUiCourseEntryPrefix50KG entry;
        if(!cold.profile->courseSnapshot(p)||game::planOriginalUiCourseEntryPrefix50KG(s->selection,p,entry)!=game::OriginalUiCourseSelectionStatus50KG::ok||
            entry.courseSlot14E4>13u||!assets::originalUiCourseBackgroundName50KG(entry.selectedControl.planet40)||entry.showPopup438)return result(d,S::profile,0x1DA618);
        if(!game::prepareOriginalUiCourseCamera50KG(*cold.courseArchive,s->camera,d.camera)||
           !game::prepareOriginalUiCourseRenderContext50KG(*cold.courseArchive,s->camera,s->renderContext,d.renderContext))return result(d,S::camera);
        if(!game::prepareOriginalUiCourseLighting50KG(*cold.courseArchive,s->lighting,d.lighting))return result(d,S::lighting);
        const auto* mats=cold.resources->materials();if(!mats)return result(d,S::resource);
        const auto font=cold.resources->materialIndex("font/denmrk16"),overlay=cold.resources->materialIndex("ui/textures/overlay1");
        if(font>=mats->size()||overlay>=mats->size()||(*mats)[font].images.size()!=1||(*mats)[overlay].images.size()!=1)return result(d,S::resource);
        game::OriginalUiCoursePageDrawDiagnostic50KG pd;
        if(!s->page.prepare(cold.courseArchive,*cold.english,cold.resources->images(),(*mats)[font].images[0],(*mats)[overlay].images[0],
            cold.resources->video()->packetZMaximum,pd))return result(d,S::resource,pd.sourcePC);
        //1DDF00 stores BA0 from the actual English owner, capacity40. Visible
        // course names remain their authored control text widgets27/31/32/28.
        constexpr std::uint32_t ids[]={11,15,12,16,14,13};
        for(auto planet:{0u,1u,4u,5u,2u}){const auto* text=cold.english->at(ids[planet]);if(!text)return result(d,S::resource,0x1DDF64);
            s->planetNamesBA0[planet]=text->text.substr(0,40);
            if(cold.resources->backgroundImage(planet)==assets::unresolvedMaterialTexture50EA)return result(d,S::resource,0x1DDF00);}
        if(!s->collector.bind(cold.resources,cold.holograms,d.draw)||!s->collector.prepareBank(d.draw))return result(d,S::draw);
        game::OriginalUiApertureCallbacks50KG callbacks{s.get(),&Storage::available,&Storage::hook,&Storage::overlay};
        if(s->aperture.bindCourse(cold.courseArchive,callbacks,d.aperture)!=AS::ok)return result(d,S::aperture);
        s->state.bound=true;storage_=std::move(s);return result(d,S::ok);
    }catch(...){return result(d,S::allocation);}
}
bool OriginalUiCourseSession50KG::ownsPreparedPage(std::uint8_t page)const noexcept{
    return page==1&&storage_&&storage_->operational()&&storage_->published&&storage_->page.prepared()&&storage_->processCurrent()&&storage_->profileCurrent()&&storage_->popupCurrent();
}
bool OriginalUiCourseSession50KG::canEnterMode(std::uint32_t mode)const noexcept{return mode==0&&ownsPreparedPage(1);}
const game::OriginalUiTransitionOwner50KG* OriginalUiCourseSession50KG::externalLifecycleOwner()const noexcept{return storage_?storage_->cold.externalTransition.get():nullptr;}
S OriginalUiCourseSession50KG::notifyTransition(const game::OriginalUiTransitionNotification50KG& e,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||s->inOperation||s->rendering||!s->currentPage()||e.route.screen!=6||e.route.page!=1||!s->processCurrent())return result(d,S::unsupportedState,e.sourcePC);
    const auto t=s->transition();if(e.phase!=t.phase||e.lifecycle!=t.lifecycle)return result(d,S::invalidInput,e.sourcePC);
    s->cpuDrain();s->state.backProposalPending=false;s->inOperation=true;bool valid=true;
    switch(e.kind){
    case Notice::requestedPage90:
        valid=!s->requested&&t.initialRestartPending&&t.lifecycle==Life::idle&&t.phase==Phase::none&&e.sourcePC==0x2A3C44;
        if(valid&&s->widgetPhase(WP::requestedPage90))s->requested=true;break;
    case Notice::pageEntry74:{
        valid=s->requested&&!s->entered&&t.initialRestartPending&&t.lifecycle==Life::idle&&t.phase==Phase::none&&e.sourcePC==0x2A3C58;
        if(!valid)break;
        game::OriginalUiCourseRememberedProfile50KG p;game::OriginalUiCourseEntryPrefix50KG entry;
        const auto stamp=s->cold.profile->stamp();OriginalUiRiderProfileOwner50KG::RiderSelectionFacts rider;
        valid=s->profileCurrent()&&stamp.mode34==0&&stamp.generation==s->coldStamp.generation+1&&
            s->cold.profile->courseRiderSelectionSnapshot(rider)&&
            s->cold.profile->courseSnapshot(p)&&game::planOriginalUiCourseEntryPrefix50KG(s->selection,p,entry)==game::OriginalUiCourseSelectionStatus50KG::ok&&
            !entry.showPopup438&&entry.courseSlot14E4<=13&&assets::originalUiCourseBackgroundName50KG(entry.selectedControl.planet40);
        if(!valid){s->fault(S::profile,0x1DA4A0);break;}
        if(s->cold.lightManager->activeUiIdentity()!=s->cold.courseArchive->identity()||
            !s->cold.lightManager->publishCourseRegistry(s->lighting,s->faultDetail.lightManager)){s->fault(S::lighting,0x1CDB10);break;}
        s->state.registryGeneration=s->cold.lightManager->managerGeneration();
        if(!s->cold.profile->commitCourseEntry(s->selection,entry,stamp)){s->fault(S::profile,0x1DA618);break;}
        s->liveStamp=s->cold.profile->stamp();s->state.courseSlot14E4=entry.courseSlot14E4;s->state.subtypeSlot14F4=entry.subtypeSlot14F4;
        s->state.laps14EC=entry.laps14EC;s->state.entrants14F0=entry.entrants14F0;
        s->state.selectedPlanet40=entry.selectedControl.planet40;s->state.selectedCourse44=entry.selectedControl.course44;
        if(!s->widgetPhase(WP::entry74))break;
        // Source1DA714 disables the actual popup clone entity. Source template
        // widgets stay manager-owned and hidden; no invented reparenting.
        if(!s->widgets().disableFreshPopup438(s->faultDetail.widget)){s->fault(S::widget,0x1DA714);break;}
        s->state.popupEnabled438=s->widgets().popupEnabled438();s->state.cameraActive=true;s->state.pageShown=true;
        s->cold.application->uiObject3D8=0;s->entered=true;break;}
    case Notice::managerEntry84:
        valid=s->entered&&!s->managerEntered&&t.lifecycle==Life::initialEntry&&t.phase==Phase::none&&e.sourcePC==0x2A3D68;
        if(valid)s->managerEntered=true;break;
    case Notice::beginTransition7C:
        if(t.phase==Phase::entry)valid=s->managerEntered&&!s->opening&&t.lifecycle==Life::waitPageTransition&&e.sourcePC==0x2A3D8C;
        else valid=t.phase==Phase::exit&&s->exitAccepted()&&!s->closing&&
            ((s->state.backAccepted&&t.lifecycle==Life::waitScreenExit&&e.sourcePC==0x2A3DFC)||
             (s->state.launchAccepted&&t.lifecycle==Life::waitCourseExit&&e.sourcePC==0x2A3E28));
        if(valid&&!apertureOkay(s->aperture.beginTransition7C(t.phase)))s->fault(S::aperture,e.sourcePC);break;
    case Notice::pageAuxiliary1C:
        if(e.sourcePC==0x2A3DA4)valid=s->managerEntered&&!s->entryAuxiliary&&t.lifecycle==Life::waitPageTransition&&t.phase==Phase::entry;
        else valid=(e.sourcePC==0x2A3E8C&&s->closed&&s->state.backAccepted&&t.lifecycle==Life::waitScreenExit&&t.phase==Phase::exit)||
            (e.sourcePC==0x2A3E40&&s->state.launchAccepted&&t.lifecycle==Life::waitCourseExit&&t.phase==Phase::exit);
        if(valid){s->state.inputContainerShown=false;if(e.sourcePC==0x2A3DA4)s->entryAuxiliary=true;}break;
    case Notice::pageAuxiliary24:
        valid=s->entryAuxiliary&&!s->entryCompleted&&s->opened&&e.sourcePC==0x2A3EE0&&t.lifecycle==Life::waitPageTransition&&
            t.phase==Phase::entry&&s->aperture.pollCompletion({6,1},Phase::entry)==Completion::complete;
        if(valid){s->entryCompleted=true;s->state.inputContainerShown=true;}break;
    case Notice::previousPage94:valid=false;break;
    }
    if(!valid)s->fault(S::transition,e.sourcePC);s->inOperation=false;
    if(s->state.faulted){d=s->faultDetail;return d.status;}return result(d,S::ok);
}
Completion OriginalUiCourseSession50KG::pollCompletion(game::OriginalUiTransitionRoute50KG route,Phase phase)const noexcept{
    const auto* s=storage_.get();if(!s||!s->operational()||!s->currentPage()||!s->lightCurrent()||!s->profileCurrent()||
        route.screen!=6||route.page!=1||s->transition().phase!=phase)return Completion::unavailable;
    if(phase==Phase::entry&&!s->opening)return Completion::pending;
    if(phase==Phase::exit&&(!s->exitAccepted()||!s->closing))return Completion::unavailable;
    return s->aperture.pollCompletion(route,phase);
}
S OriginalUiCourseSession50KG::input(const OriginalUiMainSessionInput50KG& in,D& d)noexcept{
    OriginalUiCourseSessionInputReceipt50KG receipt;return input(in,receipt,d);
}
S OriginalUiCourseSession50KG::input(const OriginalUiMainSessionInput50KG& in,OriginalUiCourseSessionInputReceipt50KG& out,D& d)noexcept{
    d={};out={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational())return result(d,s->state.quitRequested?S::quit:S::faulted);
    if(in.controller>1||in.originalInput>15)return result(d,S::invalidInput);
    if(s->inOperation||s->rendering||s->updatePrepared||!s->currentPage()||!s->profileCurrent()||!s->lightCurrent())return result(d,S::unsupportedState);
    s->state.backProposalPending=false;
    if(!s->idleInput()||in.popupActive||in.sourceInputBlocked||in.controller!=0)return result(d,S::noChange);
    out.containerReached=true;out.pageCallbackRan=true;out.pageAccepted=true; // source1DF9A4
    if(in.originalInput==2||in.originalInput==3){
        const bool down=in.originalInput==3;const auto pc=down?0x1DF8D0u:0x1DF734u;
        game::OriginalUiCourseWidgetProfile50KG profile;
        if(!s->widgetProfile(profile))return result(d,S::profile);
        if(!s->effectReady(down?4u:3u))return result(d,S::audio,pc);
        s->cpuDrain();s->inOperation=true;
        if(!s->widgets().moveRow(down,profile,s->hooks(),d.widget))s->fault(S::widget,down?0x1DF810:0x1DF674);
        else {s->effect(down?4u:3u,pc);out.selectionChanged=true;}
        s->inOperation=false;if(s->state.faulted){d=s->faultDetail;return d.status;}
        out.status=S::ok;return result(d,S::ok,down?0x1DF810:0x1DF674);
    }
    if(in.originalInput<=1){
        const bool forward=in.originalInput==1;
        const auto* row=s->widgets().navigation();
        if(!row)return result(d,S::widget);
        // Event-type owners remain unavailable. Visiting that authored row is
        // legitimate, but Left/Right must not silently change it or the course.
        if(row->controllerGroup[0]==2){out.status=S::unavailableDestination;return result(d,out.status,forward?0x1DF4A8:0x1DF1F0);}
        if(row->controllerGroup[0]==3){
            game::OriginalUiCourseWidgetProfile50KG profile;game::OriginalUiCourseLapMove50KG move;
            const auto* controller=s->widgets().controller();
            if(!controller||!s->widgetProfile(profile))return result(d,S::profile);
            if(controller->moveLaps(*row,s->state.laps14EC,forward,profile,move)!=game::OriginalUiCourseWidgetControllerStatus50KG::ok)
                return result(d,S::unsupportedState,forward?0x1DF5C4:0x1DF310);
            if(!s->effectReady(move.effectId))return result(d,S::audio,move.audioPC);
            s->cpuDrain();s->inOperation=true;
            // Page+14EC changes now. Remembered+704 and packed1125C change
            // only when the existing authentic Back/Confirm owner commits.
            s->state.laps14EC=move.laps14EC;s->state.repeatRight14D4=move.repeatRight14D4;s->state.repeatLeft14D8=move.repeatLeft14D8;
            s->effect(move.effectId,move.audioPC);out.selectionChanged=true;s->inOperation=false;
            if(s->state.faulted){d=s->faultDetail;return d.status;}
            out.status=S::ok;return result(d,S::ok,move.sourcePC);
        }
        game::OriginalUiCourseRememberedProfile50KG profile;game::OriginalUiCourseMove50KG move;
        const auto* navigation=s->widgets().navigation();game::OriginalUiCourseWidgetProfile50KG widgetProfile;
        if(!navigation||!s->cold.profile->courseSnapshot(profile)||!s->widgetProfile(widgetProfile))return result(d,S::profile);
        const auto before=*navigation;const auto stamp=s->cold.profile->stamp();
        const auto planned=game::planOriginalUiCourseMove50KG(s->selection,before,profile,forward,move);
        if(planned==game::OriginalUiCourseSelectionStatus50KG::noChange)return result(d,S::noChange);
        if(planned!=game::OriginalUiCourseSelectionStatus50KG::ok||move.courseSlot14E4>13u||
           !assets::originalUiCourseBackgroundName50KG(move.selectedControl.planet40)||
           s->cold.resources->backgroundImage(move.selectedControl.planet40)==assets::unresolvedMaterialTexture50EA||
           stamp.generation==std::numeric_limits<std::uint64_t>::max())return result(d,S::unsupportedState,move.sourcePC);
        if(!s->effectReady(forward?4u:3u))return result(d,S::audio,forward?0x1DF480:0x1DF1C8);
        // Every image/model was genuinely prepared and published at cold bind.
        // CPU packets own copied vertices; drain before model reset, keep the
        // immutable GPU bank and all resources through the whole menu lifetime.
        s->cpuDrain();s->inOperation=true;
        if(!s->widgets().moveCourse(forward,widgetProfile,s->hooks(),d.widget))s->fault(S::widget,move.sourcePC);
        else if(s->widgets().navigation()->groups[1].selectedSlot!=move.courseSlot14E4||
            !s->cold.profile->commitCourseMove(s->selection,before,forward,move,stamp))s->fault(S::profile,move.planetProfileChange.sourcePC);
        else{
            s->liveStamp=s->cold.profile->stamp();s->state.courseSlot14E4=move.courseSlot14E4;
            s->state.selectedPlanet40=move.selectedControl.planet40;s->state.selectedCourse44=move.selectedControl.course44;
            s->state.deferred14D0=move.deferred14D0;s->selectionRefreshCompleted=false;
            s->effect(forward?4u:3u,forward?0x1DF480:0x1DF1C8);
            out.selectionChanged=true;
        }
        s->inOperation=false;if(s->state.faulted){d.status=s->faultDetail.status;d.sourcePC=s->faultDetail.sourcePC;return d.status;}
        out.status=S::ok;return result(d,S::ok,move.sourcePC);
    }
    if(in.originalInput!=6&&in.originalInput!=4){out.status=in.originalInput>=2&&in.originalInput<=3?S::unavailableDestination:S::noChange;return result(d,out.status,0x1DEB90);}
    const bool confirm=in.originalInput==4;
    // Do not change the selected model/background while its real deferred
    // publication is pending. A fresh Confirm edge can retry after update3.
    if(confirm&&s->state.deferred14D0)return result(d,S::noChange,0x1DFDF0);
    if(!s->effectReady(confirm?1u:5u))return result(d,S::audio,confirm?0x1DEF18:0x1DF0E0);
    game::OriginalUiCourseContext50KG context;if(!s->context(context))return result(d,S::profile);
    game::OriginalUiRouteDecision50KG decision;
    const auto status=game::dispatchOriginalUiCourse50KG(0,confirm?game::OriginalUiRouteInput50KG::activate:game::OriginalUiRouteInput50KG::back,context,decision);
    const auto blocker=confirm?game::OriginalUiRouteBlocker50KG::courseLaunchOwner:game::OriginalUiRouteBlocker50KG::destinationOwner;
    if(status!=game::OriginalUiRouteStatus50KG::unavailable||decision.admitted||!decision.profileProposalComplete||
        decision.blockers!=static_cast<std::uint32_t>(blocker)||
        (confirm?(decision.request.kind!=game::OriginalUiRouteKind50KG::courseLaunch||decision.profileChanges.count!=8):
                 (decision.request.targetScreen!=5||decision.request.targetPage!=1)))return result(d,S::unavailableDestination,decision.inputPC);
    if(s->proposalSerial==std::numeric_limits<std::uint64_t>::max())return result(d,S::unsupportedState);
    ++s->proposalSerial;s->pendingContext=context;s->pendingDecision=decision;s->pendingStamp=s->cold.profile->stamp();
    s->pendingLifecycle=s->transition();s->state.backProposalPending=true;
    out.decision=decision;out.routeProposal={this,s->proposalSerial,decision,s->pendingStamp};out.status=confirm?S::confirmProposed:S::backProposed;
    return result(d,out.status,confirm?0x1DED58:0x1DF020);
}
bool OriginalUiCourseSession50KG::routeProposalCurrent(const OriginalUiCourseRouteProposal50KG& p)const noexcept{
    const auto* s=storage_.get();if(!s||!s->operational()||s->inOperation||s->rendering||s->updatePrepared||
        !s->state.backProposalPending||p.source!=this||!p.serial||p.serial!=s->proposalSerial||!s->idleInput()||
        !s->processCurrent()||!s->lightCurrent()||!s->profileCurrent()||!sameStamp(p.profileStamp,s->pendingStamp)||
        !sameStamp(s->cold.profile->stamp(),s->pendingStamp)||!sameLife(s->transition(),s->pendingLifecycle)||!sameDecision(p.decision,s->pendingDecision))return false;
    if(!s->effectReady(s->pendingDecision.request.kind==game::OriginalUiRouteKind50KG::courseLaunch?1u:5u))return false;
    game::OriginalUiCourseContext50KG context;game::OriginalUiRouteDecision50KG current;
    return s->context(context)&&game::dispatchOriginalUiCourse50KG(0,
        s->pendingDecision.request.kind==game::OriginalUiRouteKind50KG::courseLaunch?game::OriginalUiRouteInput50KG::activate:game::OriginalUiRouteInput50KG::back,context,current)==
        game::OriginalUiRouteStatus50KG::unavailable&&sameDecision(current,s->pendingDecision);
}
bool OriginalUiCourseSession50KG::routeContext(const OriginalUiCourseRouteProposal50KG& p,game::OriginalUiCourseContext50KG& out)const noexcept{
    if(!routeProposalCurrent(p))return false;out=storage_->pendingContext;return true;
}
S OriginalUiCourseSession50KG::notifyRouteAccepted(const OriginalUiCourseRouteProposal50KG& p,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||s->inOperation||s->rendering||s->updatePrepared||!s->state.backProposalPending||s->exitAccepted()||
        p.source!=this||p.serial!=s->proposalSerial||!sameDecision(p.decision,s->pendingDecision)||
        !sameStamp(p.profileStamp,s->pendingStamp)||!s->processCurrent()||!s->lightCurrent())return result(d,S::invalidInput);
    const bool launch=p.decision.request.kind==game::OriginalUiRouteKind50KG::courseLaunch;
    if(!s->effectReady(launch?1u:5u))return result(d,S::audio,launch?0x1DEF18:0x1DF0E0);
    auto expected=s->pendingLifecycle;if(!launch)expected.requested={5,1};
    expected.lifecycle=launch?Life::requestCourseExit:Life::requestScreenExit;
    const auto stamp=s->cold.profile->stamp();game::OriginalUiCourseRememberedProfile50KG profile;
    if(!sameLife(s->transition(),expected)||s->pendingStamp.generation==std::numeric_limits<std::uint64_t>::max()||
        stamp.generation!=s->pendingStamp.generation+1||stamp.mode34!=0||!s->cold.profile->courseSnapshot(profile)||
        !bounded(profile,*s->cold.commonProfile)||!s->testProfileCurrent(profile)||profile.course700!=s->pendingContext.courseSlot14E4||
        profile.laps704!=s->pendingContext.laps14EC||profile.entrants708!=s->pendingContext.entrants14F0||profile.subtype70C!=s->pendingContext.subtypeSlot14F4)
        return result(d,S::profile,0x1DEFCC);
    s->cpuDrain();s->liveStamp=stamp;s->state.backProposalPending=false;s->state.backAccepted=!launch;s->state.launchAccepted=launch;
    if(!s->effect(launch?1u:5u,launch?0x1DEF18:0x1DF0E0)){d=s->faultDetail;return d.status;}
    return result(d,S::ok,launch?0x1DEF18:0x1DF0E0);
}
S OriginalUiCourseSession50KG::completeSelectionRefresh(D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||s->inOperation||s->rendering||s->updatePrepared||!s->currentPage()||
       !s->profileCurrent()||!s->lightCurrent()||s->state.deferred14D0!=1||s->selectionRefreshCompleted)
        return result(d,S::unsupportedState,0x1DFE18);
    s->selectionRefreshCompleted=true;return result(d,S::ok,0x1DFE58);
}
S OriginalUiCourseSession50KG::prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG& in,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||!s->currentPage()||s->inOperation||s->rendering||s->updatePrepared)return result(d,S::unsupportedState);
    if(!std::isfinite(in.elapsedSeconds)||in.elapsedSeconds<0||in.elapsedSeconds>1||(in.paused&&in.elapsedSeconds!=0))return result(d,S::invalidInput);
    for(bool held:in.debugChordHeld)if(held)return result(d,S::unsupportedState);
    if(!s->processCurrent()||!s->profileCurrent()||!s->lightCurrent()||!s->popupCurrent()||s->cold.application->uiObject3D8)return result(d,S::unsupportedState);
    s->cpuDrain();s->state.backProposalPending=false;s->paused=in.paused;s->inOperation=true;
    // The same shared transport service as Main/Rider, exactly once before
    // the manager tick. Only the application advances PCM for accepted time.
    if(s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled){
        s->cold.audio->pause(in.paused);
        if(!s->cold.audio->update(in.elapsedSeconds)){++s->state.audio.failed;s->fault(S::audio,0x234440);}
    }
    s->inOperation=false;if(s->state.faulted){d=s->faultDetail;return d.status;}
    s->preparedDelta=in.elapsedSeconds;s->updatePrepared=true;return result(d,S::ok);
}
S OriginalUiCourseSession50KG::update(const OriginalUiMainSessionUpdate50KG& in,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||!s->updatePrepared||in.elapsedSeconds!=s->preparedDelta||in.paused!=s->paused||
        !s->profileCurrent()||!s->lightCurrent()||!s->popupCurrent()||s->rendering||s->inOperation)return result(d,S::unsupportedState);
    s->updatePrepared=false;s->inOperation=true;
    // Source1DFDF0: refresh on the third actual update, not a wall-time guess.
    // Native frontend performs a real GL finish and retained-bank rebind.
    if(s->state.deferred14D0){
        if(s->state.deferred14D0==1&&!s->selectionRefreshCompleted){s->inOperation=false;return result(d,S::unsupportedState,0x1DFE18);}
        --s->state.deferred14D0;if(!s->state.deferred14D0)s->selectionRefreshCompleted=false;
    }
    const auto saved=s->aperture.saveCourseUpdateDelta38(in.elapsedSeconds);
    if(!apertureOkay(saved)){s->faultDetail.aperture.status=saved;s->fault(S::aperture,0x1E02BC);}
    const auto* ordinary=s->widgets().order(false);const auto* top=s->widgets().order(true);
    if(!ordinary||!top)s->fault(S::widget,0x2A6BB0);
    else if(s->operational()&&s->state.pageShown){s->updateWidgets(*ordinary,in.elapsedSeconds);if(s->operational())s->updateWidgets(*top,in.elapsedSeconds);}
    ++s->state.updates;s->inOperation=false;if(s->state.faulted){d=s->faultDetail;return d.status;}return result(d,S::ok);
}
S OriginalUiCourseSession50KG::render(const OriginalUiCourseDrawClock50KG& clock,D& d)noexcept{
    d={};auto* s=storage_.get();if(!s)return result(d,S::notBound);
    if(!s->operational()||s->inOperation||s->rendering||s->updatePrepared||!clock.known||(s->paused&&clock.advanceAnimations)||
        !s->currentPage()||!s->profileCurrent()||!s->lightCurrent()||!s->state.cameraActive||!s->state.pageShown||
        !s->popupCurrent()||!s->aperture.snapshot().hasUpdateDelta)return result(d,S::unsupportedState);
    OriginalUiCourseProfileColors50KG colors;if(!s->cold.profile->coursePlanetColors(s->state.selectedPlanet40,colors))return result(d,S::profile);
    s->cpuDrain();s->state.backProposalPending=false;s->inOperation=true;s->rendering=true;
    bool okay=s->collector.begin(clock,s->renderContext,s->lighting,d.draw,s->state.selectedPlanet40)&&s->collector.projectOrdinaryWidgets(true,d.draw);
    game::OriginalUiCoursePageDrawDiagnostic50KG pageDiagnostic;
    if(okay&&!s->page.setLapCount(s->state.laps14EC,pageDiagnostic)){s->fault(S::widget,pageDiagnostic.sourcePC);okay=false;}
    if(okay&&!s->page.applyPlanetPalette(s->widgets(),colors.primary79C,colors.secondary7A0,pageDiagnostic)){s->fault(S::widget,pageDiagnostic.sourcePC);okay=false;}
    if(okay)okay=s->collector.projectPage(s->page,d.draw);
    if(okay){const auto status=s->aperture.beginCourseEntryAtRender48(s->transition().phase);
        if(!apertureOkay(status)){s->faultDetail.aperture.status=status;s->fault(S::aperture,0x1DEB60);okay=false;}}
    if(okay&&s->operational()){const auto status=s->aperture.render48(s->transition().lifecycle);
        if(!apertureOkay(status)){s->faultDetail.aperture.status=status;s->fault(S::aperture,0x1DEB6C);okay=false;}}
    if(okay&&s->operational())okay=s->collector.projectPrompts(s->state.promptMask42C,d.draw)&&s->collector.projectOnTopWidgets(true,d.draw)&&s->collector.finish(d.draw);
    s->rendering=false;s->inOperation=false;
    if(!okay||s->state.faulted){if(!s->state.faulted){s->faultDetail.draw=d.draw;s->fault(S::draw,d.draw.sourcePC);}s->cpuDrain();d=s->faultDetail;return d.status;}
    s->state.framePublished=true;++s->state.renders;return result(d,S::ok);
}
S OriginalUiCourseSession50KG::prepareBank(D& d)noexcept{
    d={};if(!storage_)return result(d,S::notBound);storage_->cpuDrain();if(storage_->collector.bank())return result(d,S::ok);
    return result(d,storage_->collector.prepareBank(d.draw)?S::ok:S::draw);
}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiCourseSession50KG::bank()const noexcept{return storage_?storage_->collector.bank():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiCourseSession50KG::samplers()const noexcept{return storage_?storage_->collector.samplers():nullptr;}
S OriginalUiCourseSession50KG::bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB> gpu,D& d)noexcept{
    d={};if(!storage_)return result(d,S::notBound);storage_->cpuDrain();if(!storage_->collector.bindPublishedBank(std::move(gpu),d.draw))return result(d,S::draw);
    storage_->published=true;return result(d,S::ok);
}
const OriginalUiNativeDraw50KG* OriginalUiCourseSession50KG::draw()const noexcept{return storage_&&storage_->state.framePublished?storage_->collector.draw():nullptr;}
OriginalUiCourseSessionSnapshot50KG OriginalUiCourseSession50KG::snapshot()const noexcept{
    if(!storage_)return {};auto result=storage_->state;result.transition=storage_->transition();result.aperture=storage_->aperture.snapshot();
    if(const auto* n=storage_->cold.resources->widgets()->navigation())result.controllerGroup=n->controllerGroup[0];
    result.popupEnabled438=storage_->cold.resources->widgets()->popupEnabled438();return result;
}
void OriginalUiCourseSession50KG::drain()noexcept{if(storage_)storage_->cpuDrain();}
void OriginalUiCourseSession50KG::requestQuit()noexcept{if(storage_){storage_->state.quitRequested=true;storage_->cpuDrain();}}
}

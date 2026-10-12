// Source-reviewed bounded Course integration; runtime acceptance is pending.
#include "original_ui_frontend50kg.h"
#include "../../../src/assets/original_ui_course_background50kg.h"
#include <new>
#include <utility>

namespace rr::platform {
namespace {
using Status=OriginalUiFrontendStatus50KG;
using Diagnostic=OriginalUiFrontendDiagnostic50KG;
using Route=game::OriginalUiTransitionRoute50KG;
using Life=game::OriginalUiTransitionLifecycle50KG;
using Phase=game::OriginalUiTransitionPhase50KG;
using Transition=game::OriginalUiTransitionStatus50KG;
using Admission=game::OriginalUiTransitionAdmission50KG;
using MainStatus=OriginalUiMainSessionStatus50KG;
using OptionsStatus=OriginalUiOptionsSessionStatus50KG;
using RiderStatus=OriginalUiRiderSessionStatus50KG;
using CourseStatus=OriginalUiCourseSessionStatus50KG;
// Bounded Rider19 -> Course -> Back/native race, with the shared audio owner.
// Real archive, profile, popup visibility and resource predicates remain required.
constexpr bool boundedCourseAdmissionEnabled=true;
Status result(Diagnostic& d,Status s,std::uint32_t pc=0) noexcept {d.status=s;d.sourcePC=pc;return s;}
bool same(Route a,Route b) noexcept {return a.screen==b.screen&&a.page==b.page;}
bool archive(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,assets::OriginalUiArchiveId50KG id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool sameProfile(const OriginalUiMainProfile50KG& a,const OriginalUiMainProfile50KG& b) noexcept {
    return a.known==b.known&&a.mode34==b.mode34&&a.previousMode38==b.previousMode38&&a.savedSlot6E0==b.savedSlot6E0&&
        a.availability11018==b.availability11018&&a.flags11054==b.flags11054&&a.playerBits1125C==b.playerBits1125C;
}
bool sameMainDecision(const game::OriginalUiMainDecision50KG& a,const game::OriginalUiMainDecision50KG& b) noexcept {
    return a.action==b.action&&a.selectedSlot==b.selectedSlot&&a.originalGameMode==b.originalGameMode&&
        a.originalPlayerBits==b.originalPlayerBits&&a.routeArgument==b.routeArgument&&a.targetScreen==b.targetScreen&&a.sourcePC==b.sourcePC;
}
bool sameRouteProposal(const game::OriginalUiRouteDecision50KG& a,const game::OriginalUiRouteDecision50KG& b) noexcept {
    const auto& x=a.request;const auto& y=b.request;
    if(a.profileProposalComplete!=b.profileProposalComplete||a.selectedSlot!=b.selectedSlot||a.selectedRider!=b.selectedRider||
       a.inputPC!=b.inputPC||x.kind!=y.kind||x.sourceScreen!=y.sourceScreen||x.sourcePage!=y.sourcePage||
       x.targetScreen!=y.targetScreen||x.targetPage!=y.targetPage||x.sourcePC!=y.sourcePC||
       a.profileChanges.count!=b.profileChanges.count||a.profileChanges.count>a.profileChanges.values.size())return false;
    for(std::size_t i=0;i<a.profileChanges.count;++i){const auto& p=a.profileChanges.values[i];const auto& q=b.profileChanges.values[i];
        if(p.field!=q.field||p.operation!=q.operation||p.value!=q.value||p.replaceMask!=q.replaceMask||p.sourcePC!=q.sourcePC)return false;}
    return true;
}
struct Operation {
    bool& flag;explicit Operation(bool& f) noexcept:flag(f){flag=true;}
    ~Operation(){flag=false;}
};
bool optionsBackShape(const game::OriginalUiRouteDecision50KG& d) noexcept {
    const auto& r=d.request;
    if(r.kind!=game::OriginalUiRouteKind50KG::changeScreen||r.sourceScreen!=12||r.sourcePage!=1||
        r.targetScreen!=4||r.targetPage!=1||r.sourcePC!=0x1F99A4||d.inputPC!=0x1F9640||
        !d.profileProposalComplete||d.selectedSlot<1||d.selectedSlot>6||d.selectedRider!=-1||d.profileChanges.count!=1)return false;
    const auto& p=d.profileChanges.values[0];
    return p.field==game::OriginalUiProfileField50KG::optionsSlot6E4&&p.operation==game::OriginalUiProfileWrite50KG::word&&
        p.value==d.selectedSlot&&p.replaceMask==0xFFFFFFFFu&&p.sourcePC==0x1F9994;
}
bool optionsPageShape(const game::OriginalUiRouteDecision50KG& d) noexcept {
    if(game::originalUiGameOptionsAcceptShape50KG(d))return true;
    const auto& r=d.request;
    if(r.kind!=game::OriginalUiRouteKind50KG::changePage||r.sourceScreen!=12||r.targetScreen!=12||
        !d.profileProposalComplete||d.selectedRider!=-1||d.profileChanges.count!=1)return false;
    const auto& p=d.profileChanges.values[0];
    if(p.operation!=game::OriginalUiProfileWrite50KG::word||p.value!=d.selectedSlot||p.replaceMask!=0xFFFFFFFFu)return false;
    if(r.sourcePage==1&&r.targetPage==4)
        return d.inputPC==0x1F9640&&d.selectedSlot==3&&r.sourcePC==0x1F9898&&
            p.field==game::OriginalUiProfileField50KG::optionsSlot6E4&&p.sourcePC==0x1F9940;
    // Cancel's one store remains distinct from the source Accept transaction.
    return r.sourcePage==4&&r.targetPage==1&&d.inputPC==0x1F50F0&&d.selectedSlot>=1&&d.selectedSlot<=8&&
        r.sourcePC==0x1F51D8&&p.field==game::OriginalUiProfileField50KG::optionsGroup6E8&&p.sourcePC==0x1F51C8;
}
}
struct OriginalUiFrontend50KG::Storage {
    struct Manager {
        Storage* owner=nullptr;
        // Cold screen identity, never used as a replacement for the manager's
        // authoritative current/requested page after initial activation.
        Route initialRoute{};
        std::shared_ptr<OriginalUiMainDrawResources50KG> mainResources;
        std::shared_ptr<OriginalUiOptionsDrawResources50KG> optionsResources;
        std::shared_ptr<OriginalUiRiderDrawResources50KG> riderResources;
        std::shared_ptr<OriginalUiCourseDrawResources50KG> courseResources;
        std::shared_ptr<OriginalUiCourseHologramDraw50KG> courseHolograms;
        std::shared_ptr<const assets::OriginalUiImageBank50KG> publishedCourseBank;
        std::shared_ptr<assets::OriginalUiRiderStatAssets50KG> riderStats;
        std::shared_ptr<assets::OriginalUiRiderBinderAssets50KG> riderBinder;
        std::shared_ptr<const assets::OriginalUiImageBank50KG> publishedRiderBank;
        std::vector<MaterialTextureSampler50EB> gpuSamplers;
        std::shared_ptr<MaterialTextureGl50EB> gpu;
        std::uint64_t publication=0;
        std::shared_ptr<game::OriginalUiTransitionOwner50KG> transition;
        std::unique_ptr<OriginalUiMainSession50KG> main;
        std::unique_ptr<OriginalUiOptionsSession50KG> options;
        std::unique_ptr<OriginalUiRiderSession50KG> rider;
        std::unique_ptr<OriginalUiCourseSession50KG> course;
        bool riderRetired=false;
        Route currentRoute() const noexcept {
            return transition&&transition->snapshot().bound?transition->snapshot().current:initialRoute;
        }
        bool ownsRoute(Route route) const noexcept {
            return route.screen==initialRoute.screen&&
                (main?same(route,{4,1}):options?options->ownsPreparedPage(route.page):rider?rider->ownsPreparedPage(route.page):course&&course->ownsPreparedPage(route.page));
        }
        const assets::MaterialTextureAssets50EA* textures() const noexcept {
            if(course){const auto bank=publishedCourseBank?publishedCourseBank:course->bank();return bank&&bank->images()?bank->images()->textures():nullptr;}
            if(rider){const auto bank=publishedRiderBank?publishedRiderBank:rider->bank();return bank&&bank->images()?bank->images()->textures():nullptr;}
            return mainResources?mainResources->textures():(optionsResources?optionsResources->textures():nullptr);
        }
        bool prepared() const noexcept {
            if(!owner||!transition||!gpu||!publication||!textures()||!gpu->initialized()||
                !gpu->validBorrow(owner->cold.textureRequest.context,owner->cold.textureRequest.isCurrent,publication))return false;
            if(same(initialRoute,{4,1})&&main&&mainResources&&!options&&!optionsResources){
                const auto s=main->snapshot();return s.bound&&!s.faulted&&!s.quitRequested&&mainResources->loaded()&&
                    main->externalLifecycleOwner()==transition.get()&&mainResources->exitText()&&mainResources->exitText()->prepared();
            }
            if(same(initialRoute,{12,1})&&options&&optionsResources&&!main&&!mainResources){
                const auto s=options->snapshot();return s.bound&&!s.faulted&&!s.quitRequested&&optionsResources->loaded()&&
                    options->externalLifecycleOwner()==transition.get()&&options->ownsPreparedPage(1)&&
                    optionsResources->exitText()&&optionsResources->exitText()->prepared();
            }
            if(same(initialRoute,{5,1})&&rider&&riderResources&&riderStats&&riderBinder&&!main&&!options){
                const auto s=rider->snapshot();return s.bound&&!s.faulted&&!s.quitRequested&&riderResources->loaded()&&publishedRiderBank&&
                    rider->externalLifecycleOwner()==transition.get()&&rider->ownsPreparedPage(1)&&
                    rider->canEnterMode(rider->intendedMode())&&riderResources->exitText(0)&&riderResources->exitText(0)->prepared();
            }
            if(same(initialRoute,{6,1})&&course&&courseResources&&courseHolograms&&!main&&!options&&!rider){
                const auto s=course->snapshot();return s.bound&&!s.faulted&&!s.quitRequested&&publishedCourseBank&&
                    courseResources->loaded()&&courseHolograms->loaded()&&course->externalLifecycleOwner()==transition.get()&&
                    course->ownsPreparedPage(1)&&course->canEnterMode(0)&&course->intendedMode()==0&&
                    courseResources->exitText()&&courseResources->exitText()->prepared();
            }
            return false;
        }
        game::OriginalUiApertureSnapshot50KG aperture() const noexcept {return main?main->snapshot().aperture:options?options->snapshot().aperture:rider?rider->snapshot().aperture:course->snapshot().aperture;}
        const OriginalUiNativeDraw50KG* draw() const noexcept {return main?main->draw():(options?options->draw():course?course->draw():nullptr);}
    };
    OriginalUiFrontendCold50KG cold;
    assets::OriginalUiEnglish50KG english;
    // Callback contexts are Manager objects at stable addresses, not snapshots.
    // Manager owns no parent shared_ptr, so no cross-session ownership cycle.
    std::unique_ptr<Manager> active,pending,retiring;
    Manager* activating=nullptr;
    Manager* bank=nullptr;
    VehicleWindowGl50DS* renderer=nullptr;
    bool prepared=false,activated=false,faulted=false,quit=false,inOperation=false,closing=false;
    bool queued=false,frameRendered=false,frameSubmitted=false;
    bool courseLaunchReady=false,courseLaunchTaken=false;
    OriginalUiRiderProfileOwner50KG::CourseRaceSelection launchSelection{};
    // This coordinator's current UI-manager lease on the shared APPLICATION
    // object. Cold/pending resource owners never enter or release it.
    bool lightManagerEntered=false;
    const assets::OriginalUiArchiveIdentity50KG* lightManagerUi=nullptr;
    std::uint64_t lightManagerGeneration=0;
    game::OriginalUiApplicationRequest50KG queue{};
    OriginalUiMainRouteProposal50KG mainProposal{};
    OriginalUiOptionsRouteProposal50KG optionsProposal{};
    OriginalUiRiderRouteProposal50KG riderProposal{};
    OriginalUiCourseRouteProposal50KG courseProposal{};
    OriginalUiMainProfile50KG mainProfileProposal{};
    std::uint64_t updates=0,renders=0,submissions=0,switches=0,renderedUpdate=0,submittedUpdate=0;
    Diagnostic failure{};
    Status fault(Diagnostic& d,Status s,std::uint32_t pc=0) noexcept {
        if(!faulted){result(d,s,pc);failure=d;faulted=true;}
        frameRendered=false;frameSubmitted=false;d=failure;return d.status;
    }
    bool ownsLightManager() const noexcept {
        return lightManagerEntered&&cold.lightManager&&lightManagerUi&&lightManagerGeneration&&
            cold.lightManager->activeUiIdentity()==lightManagerUi&&
            cold.lightManager->managerGeneration()==lightManagerGeneration;
    }
    bool enterLightManager(const assets::OriginalUiArchive50KG& source,Diagnostic& d) noexcept {
        if(lightManagerEntered||!cold.lightManager){
            d.lightManager={game::OriginalUiLightManagerLifetimeError50KG::phase,0x1CCD88};return false;
        }
        if(!cold.lightManager->beginUiManager(source,d.lightManager))return false;
        lightManagerUi=source.identity();lightManagerGeneration=cold.lightManager->managerGeneration();
        lightManagerEntered=true;return true;
    }
    bool releaseLightManager(Diagnostic& d) noexcept {
        if(!lightManagerEntered)return true;
        const bool owned=ownsLightManager();
        const bool released=owned&&cold.lightManager->releaseUiManager();
        // If external work broke the lease, do not release another owner's
        // manager or turn unknown history into a fresh absent-cache claim.
        // Native consumers have already drained before this helper is reached.
        if(!released&&cold.lightManager)cold.lightManager->invalidateForUntrackedScene();
        lightManagerEntered=false;lightManagerUi=nullptr;lightManagerGeneration=0;
        if(!released){d.lightManager={game::OriginalUiLightManagerLifetimeError50KG::phase,0x148110};return false;}
        return true;
    }
    bool processReady() const noexcept {
        return cold.lightManager&&cold.lightManager->cacheState()==game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction&&
            (!lightManagerEntered||ownsLightManager())&&
            cold.profile&&cold.profile->known&&cold.profile->savedSlot6E0>=1&&cold.profile->savedSlot6E0<=5&&
            cold.optionsProfile&&cold.optionsProfile->known&&cold.optionsProfile->savedSlot6E4>=1&&cold.optionsProfile->savedSlot6E4<=6&&
            (!cold.gameSettingsProfile||game::validOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile))&&
            cold.application&&cold.application->known&&!(cold.application->globalFlags228&4u)&&cold.application->transportE00==-1&&
            (cold.audioMode==OriginalUiMainAudioMode50KG::muted?!cold.audio:(cold.audio&&cold.audio->ready()));
    }
    bool riderInputsReady() const noexcept {
        using Id=assets::OriginalUiArchiveId50KG;
        return (cold.audioMode==OriginalUiMainAudioMode50KG::muted?!cold.audio:
                cold.audioMode==OriginalUiMainAudioMode50KG::enabled&&cold.audio&&cold.audio->riderMenuReady())&&
            archive(cold.rider,Id::rider)&&archive(cold.backgrounds,Id::backgrounds)&&archive(cold.podTextures,Id::podTextures)&&
            cold.riderIpumCache&&cold.riderIpumCache->loaded()&&cold.riderProfile&&cold.riderProfile->initialized()&&
            cold.riderProfile->commonProfile()==cold.profile.get();
    }
    bool courseInputsReady() const noexcept {
        using Id=assets::OriginalUiArchiveId50KG;
        if(!boundedCourseAdmissionEnabled||
           (cold.audioMode==OriginalUiMainAudioMode50KG::muted?bool(cold.audio):
            cold.audioMode!=OriginalUiMainAudioMode50KG::enabled||!cold.audio||!cold.audio->courseMenuReady())||
           !archive(cold.course,Id::course)||!archive(cold.backgrounds,Id::backgrounds)||!cold.riderProfile||
           !cold.riderProfile->initialized()||cold.riderProfile->commonProfile()!=cold.profile.get()||
           !cold.profile||!cold.profile->known||cold.profile->mode34!=0||
           (cold.profile->playerBits1125C&0x60)!=0x20)return false;
        game::OriginalUiCourseRememberedProfile50KG p;
        return cold.riderProfile->courseSnapshot(p)&&p.ownerReady&&p.mode34==0&&p.course700>=1&&p.course700<=4&&p.subtype70C==1&&
            p.laps704==3&&p.entrants708==8&&!p.cardFlag100BC&&assets::originalUiCourseBackgroundName50KG(p.packed1125E>>5)&&
            p.planet0Primary79C==0xC2A684&&p.planet0Secondary7A0==0x80674A;
    }
    bool sourceCurrent() const noexcept {
        return activated&&ownsLightManager()&&processReady()&&renderer&&renderer->initialized()&&bank==active.get()&&active&&active->prepared()&&
            active->transition->snapshot().bound&&active->ownsRoute(active->transition->snapshot().current);
    }
    bool destination(Route route) const noexcept {
        return pending&&same(pending->initialRoute,route)&&pending->prepared()&&!pending->transition->snapshot().bound&&processReady();
    }
    bool sourceProposalCurrent() const noexcept {
        return active&&(active->main?active->main->routeProposalCurrent(mainProposal):active->options?
            active->options->routeProposalCurrent(optionsProposal):active->rider?active->rider->routeProposalCurrent(riderProposal):
            active->course&&active->course->routeProposalCurrent(courseProposal));
    }
    static bool admit(void* p,const game::OriginalUiScreenFactory50KG& screen,
        const game::OriginalUiPageFactory50KG& page,Admission kind) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        if(s.faulted||s.quit||s.closing||!s.processReady())return false;
        if(kind==Admission::initialPage)
            return (&m==s.activating)&&m.prepared()&&!m.transition->snapshot().bound&&
                &screen==game::findOriginalUiScreenFactory50KG(m.initialRoute.screen)&&
                &page==game::findOriginalUiBoundedPageFactory50KG(m.initialRoute.screen,m.initialRoute.page);
        if(&m!=s.active.get()||!s.sourceCurrent()||!s.sourceProposalCurrent())return false;
        if(kind==Admission::page){
            if(!m.options||s.pending||!optionsPageShape(s.optionsProposal.decision))return false;
            const auto& r=s.optionsProposal.decision.request;
            return same(m.currentRoute(),{r.sourceScreen,r.sourcePage})&&m.ownsRoute({r.targetScreen,r.targetPage})&&
                &screen==game::findOriginalUiScreenFactory50KG(r.targetScreen)&&
                &page==game::findOriginalUiBoundedPageFactory50KG(r.targetScreen,r.targetPage);
        }
        if(kind!=Admission::screen)return false;
        const Route target=m.main?Route{static_cast<std::uint8_t>(s.mainProposal.decision.targetScreen),
            static_cast<std::uint8_t>(s.mainProposal.decision.routeArgument)}:m.rider?
            Route{s.riderProposal.decision.request.targetScreen,s.riderProposal.decision.request.targetPage}:m.course?
            Route{s.courseProposal.decision.request.targetScreen,s.courseProposal.decision.request.targetPage}:Route{4,1};
        return s.destination(target)&& &screen==game::findOriginalUiScreenFactory50KG(target.screen)&&
            &page==game::findOriginalUiBoundedPageFactory50KG(target.screen,target.page);
    }
    static void notify(void* p,const game::OriginalUiTransitionNotification50KG& event) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        if(s.faulted)return;
        Diagnostic d;
        const auto t=m.transition->snapshot();
        // requestedPage90 is delivered BEFORE current-page publication. Every
        // other notice belongs to the actual current page, including previous94.
        const auto expected=event.kind==game::OriginalUiTransitionNotificationKind50KG::requestedPage90?t.requested:t.current;
        if((&m!=s.active.get()&& &m!=s.activating)||!t.bound||!same(event.route,expected)||
            event.lifecycle!=t.lifecycle||event.phase!=t.phase||!m.ownsRoute(event.route)){
            s.fault(d,Status::transition,event.sourcePC);return;
        }
        const bool okay=m.main?m.main->notifyTransition(event,d.main)==MainStatus::ok:m.options?
            m.options->notifyTransition(event,d.options)==OptionsStatus::ok:m.rider?m.rider->notifyTransition(event,d.rider)==RiderStatus::ok:
            m.course&&m.course->notifyTransition(event,d.course)==CourseStatus::ok;
        if(!okay)s.fault(d,Status::session,event.sourcePC);
    }
    static game::OriginalUiTransitionCompletion50KG poll(void* p,Route route,Phase phase) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        const auto t=m.transition->snapshot();
        if(s.faulted||s.quit||&m!=s.active.get()||!s.sourceCurrent()||!same(route,t.current)||phase!=t.phase)
            return game::OriginalUiTransitionCompletion50KG::unavailable;
        return m.main?m.main->pollCompletion(route,phase):m.options?m.options->pollCompletion(route,phase):m.rider?m.rider->pollCompletion(route,phase):m.course->pollCompletion(route,phase);
    }
    static bool commitMain(void* p,const game::OriginalUiMainDecision50KG& decision) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        if(s.faulted||s.quit||&m!=s.active.get()||!m.main||!s.sourceCurrent()||!s.processReady()||
           !s.riderInputsReady()||!s.destination({5,1})||!s.pending->rider||
           !m.main->routeProposalCurrent(s.mainProposal)||!sameMainDecision(decision,s.mainProposal.decision)||
           !sameProfile(*s.cold.profile,s.mainProfileProposal)||decision.selectedSlot<1||decision.selectedSlot>3||
           decision.action!=game::OriginalUiMainAction50KG::changeScreen||decision.targetScreen!=5||decision.routeArgument!=1||
           decision.originalGameMode!=static_cast<std::int8_t>(decision.selectedSlot-1)||
           decision.originalPlayerBits!=(decision.selectedSlot==2?0x40:0x20)||
           !s.pending->rider->canEnterMode(static_cast<std::uint32_t>(decision.originalGameMode))||
           s.pending->rider->intendedMode()!=static_cast<std::uint32_t>(decision.originalGameMode))return false;
        auto& profile=*s.cold.profile;const auto mode=static_cast<std::uint32_t>(decision.originalGameMode);
        if(profile.mode34!=mode){profile.previousMode38=profile.mode34;profile.mode34=mode;}
        profile.playerBits1125C=static_cast<std::uint8_t>((profile.playerBits1125C&~0x60u)|decision.originalPlayerBits);
        return true;
    }
    bool courseLaunchProposal(OriginalUiRiderProfileOwner50KG::CourseRaceSelection& selection)const noexcept {
        if(!cold.courseRaceDestination||!active||!active->course||pending||
           !active->course->routeProposalCurrent(courseProposal)||
           courseProposal.decision.request.kind!=game::OriginalUiRouteKind50KG::courseLaunch)return false;
        game::OriginalUiCourseContext50KG c;std::uint8_t rider=0;std::uint32_t comparison=0;
        if(!active->course->routeContext(courseProposal,c)||!cold.riderProfile->riderConfirmSnapshot(rider,comparison)||
           c.mode34!=0||c.selectedControlWord44!=8||c.courseSlot14E4!=4||c.subtypeSlot14F4!=1||
           c.laps14EC!=3||c.entrants14F0!=8||(rider>>3)!=19||comparison!=19||
           ((c.packed1125C>>5)&3)!=1)return false;
        selection={8,1,19,1,3,8,static_cast<std::uint16_t>(3u|(1u<<5)|(7u<<7)|(19u<<11))};
        return cold.courseRaceDestination->admits(selection);
    }
    static bool admitCourse(void* p) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        OriginalUiRiderProfileOwner50KG::CourseRaceSelection selection;
        return !s.faulted&&!s.quit&&!s.closing&& &m==s.active.get()&&s.sourceCurrent()&&s.courseLaunchProposal(selection);
    }
    static bool commitOptions(void* p,const game::OriginalUiRouteDecision50KG& decision) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        if(m.course){
            const auto& proposed=s.courseProposal.decision;
            if(proposed.request.kind==game::OriginalUiRouteKind50KG::courseLaunch){
                OriginalUiRiderProfileOwner50KG::CourseRaceSelection selection;
                game::OriginalUiCourseContext50KG context;
                if(s.faulted||s.quit||&m!=s.active.get()||!s.sourceCurrent()||!s.courseLaunchProposal(selection)||
                   proposed.admitted||proposed.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::courseLaunchOwner)||
                   !decision.admitted||decision.blockers||decision.action!=game::OriginalUiRouteAction50KG::request||
                   !sameRouteProposal(decision,proposed)||!m.course->routeContext(s.courseProposal,context)||
                   !s.cold.riderProfile->commitCourseLaunch(decision,context,s.courseProposal.profileStamp))return false;
                s.cold.application->courseUi474=0;s.cold.application->courseUi478=0;
                s.launchSelection=selection;return true;
            }
            if(s.faulted||s.quit||&m!=s.active.get()||!s.sourceCurrent()||!s.courseInputsReady()||!s.riderInputsReady()||
               !m.course->routeProposalCurrent(s.courseProposal)||!s.destination({5,1})||!s.pending->rider||
               !s.pending->rider->canEnterMode(0)||s.pending->rider->intendedMode()!=0||
               !decision.admitted||decision.blockers||decision.action!=game::OriginalUiRouteAction50KG::request||
               proposed.admitted||proposed.action!=game::OriginalUiRouteAction50KG::unavailable||
               proposed.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)||
               !sameRouteProposal(decision,proposed)||decision.request.sourceScreen!=6||decision.request.sourcePage!=1||
               decision.request.targetScreen!=5||decision.request.targetPage!=1||decision.profileChanges.count!=4)return false;
            game::OriginalUiCourseContext50KG context;
            return m.course->routeContext(s.courseProposal,context)&&
                s.cold.riderProfile->commitCourseBack(decision,context,s.courseProposal.profileStamp);
        }
        if(m.rider){
            const auto& proposed=s.riderProposal.decision;
            if(s.faulted||s.quit||&m!=s.active.get()||!s.sourceCurrent()||!s.processReady()||!s.riderInputsReady()||
               !m.rider->routeProposalCurrent(s.riderProposal)||
               !decision.admitted||decision.blockers||decision.action!=game::OriginalUiRouteAction50KG::request||
               proposed.admitted||proposed.action!=game::OriginalUiRouteAction50KG::unavailable||
               proposed.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)||
               !sameRouteProposal(decision,proposed)||decision.request.sourceScreen!=5||decision.request.sourcePage!=1||
               decision.request.targetPage!=1)return false;
            if(decision.request.targetScreen==4&&s.destination({4,1}))
                return s.cold.riderProfile->commitBack(decision,s.riderProposal.profileStamp);
            if(decision.request.targetScreen!=6||!s.courseInputsReady()||!s.destination({6,1})||!s.pending->course||
               !s.pending->course->canEnterMode(0)||decision.selectedRider!=19||decision.profileChanges.count!=3||
               decision.request.sourcePC!=0x1EA604)return false;
            return s.cold.riderProfile->commitRiderConfirmToCourse(decision,s.riderProposal.profileStamp);
        }
        // Exact private typed proposal was revalidated after destination admission.
        // No generic profile patch interpreter and no arbitrary admitted flags.
        if(s.faulted||s.quit||&m!=s.active.get()||!m.options||!s.sourceCurrent()||!s.processReady()||
            !m.options->routeProposalCurrent(s.optionsProposal)||
            !decision.admitted||decision.blockers||decision.action!=game::OriginalUiRouteAction50KG::request||
            decision.selectedSlot!=s.optionsProposal.decision.selectedSlot)return false;
        const auto& proposed=s.optionsProposal.decision;
        if(optionsBackShape(decision)){
            if(!optionsBackShape(proposed)||!s.destination({4,1}))return false;
            s.cold.optionsProfile->savedSlot6E4=decision.profileChanges.values[0].value;
        }else if(optionsPageShape(decision)){
            if(!optionsPageShape(proposed)||s.pending||
                decision.request.sourcePage!=proposed.request.sourcePage||decision.request.targetPage!=proposed.request.targetPage||
                decision.request.sourcePC!=proposed.request.sourcePC||
                !m.ownsRoute({decision.request.targetScreen,decision.request.targetPage}))return false;
            if(game::originalUiGameOptionsAcceptShape50KG(decision))
                return m.options->commitGameOptionsAccept(s.optionsProposal,decision);
            if(decision.request.sourcePage==4){
                if(!s.cold.gameSettingsProfile)return false;
                s.cold.gameSettingsProfile->savedGroup6E8=decision.profileChanges.values[0].value;
            }else s.cold.optionsProfile->savedSlot6E4=decision.profileChanges.values[0].value;
        }else return false;
        return true;
    }
    static bool enqueue(void* p,const game::OriginalUiApplicationRequest50KG& q) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        if(q.flags==9){
            const auto t=m.transition->snapshot();const auto a=m.aperture();
            OriginalUiRiderProfileOwner50KG::CourseRaceSelection actual;
            if(s.faulted||s.quit||s.queued||&m!=s.active.get()||!s.sourceCurrent()||!m.course||s.pending||
               !m.course->snapshot().launchAccepted||q.screen||q.page||q.sourcePC!=0x2A3F98||
               t.lifecycle!=Life::queueCourse||t.phase!=Phase::none||t.applicationRequestQueued||
               a.state!=game::OriginalUiApertureState50KG::closed||a.inputEnabled268||
               !s.cold.riderProfile->courseRaceSnapshot(actual)||!s.cold.courseRaceDestination||
               !s.cold.courseRaceDestination->admits(actual))return false;
            s.queue=q;s.queued=true;return true;
        }
        if(s.faulted||s.quit||s.queued||&m!=s.active.get()||!s.sourceCurrent()||q.flags!=2||q.sourcePC!=0x2A3F70||
            q.screen>255||q.page!=1||!s.destination({static_cast<std::uint8_t>(q.screen),1}))return false;
        const auto t=m.transition->snapshot();const auto a=m.aperture();
        if(t.lifecycle!=Life::queueScreen||t.phase!=Phase::none||t.applicationRequestQueued||
            t.requested.screen!=q.screen||t.requested.page!=q.page||a.state!=game::OriginalUiApertureState50KG::closed||a.inputEnabled268)return false;
        s.queue=q;s.queued=true;return true; // No switch, binding, teardown or GL.
    }
    game::OriginalUiTransitionCallbacks50KG callbacks(Manager& m) noexcept {
        game::OriginalUiTransitionCallbacks50KG c;c.context=&m;c.admitDestination=&admit;c.notify=&notify;
        c.pollCompletion=&poll;c.admitCourseLaunch=&admitCourse;c.commitMainSelection=&commitMain;c.commitRouteSelection=&commitOptions;c.queueApplication=&enqueue;return c;
    }
    bool build(std::unique_ptr<Manager>& out,Route route,Diagnostic& d,std::uint32_t prospectiveMode=0) noexcept {
        if(out){result(d,Status::invalidInput);return false;}
        try {
            out=std::make_unique<Manager>();auto& m=*out;m.owner=this;m.initialRoute=route;
            m.transition=std::make_shared<game::OriginalUiTransitionOwner50KG>();
            const std::vector<MaterialTextureSampler50EB>* samplers=nullptr;
            if(same(route,{4,1})){
                OriginalUiMainColdInput50KG input;input.appStart=cold.appStart;input.mainMenu=cold.mainMenu;
                input.fonts=cold.fonts;input.controllerPopup=cold.controllerPopup;input.overlays=cold.overlays;
                input.english=&english;input.runtime=cold.runtime;input.video=cold.video;
                m.mainResources=std::make_shared<OriginalUiMainDrawResources50KG>();
                if(!m.mainResources->load(input,d.main.draw)){result(d,Status::resource);return false;}
                samplers=m.mainResources->samplers();
            }else if(same(route,{12,1})){
                OriginalUiOptionsColdInput50KG input;input.appStart=cold.appStart;input.options=cold.options;
                input.fonts=cold.fonts;input.controllerPopup=cold.controllerPopup;input.overlays=cold.overlays;
                input.english=&english;input.runtime=cold.runtime;
                input.video={cold.video.lifetime,cold.video.width,cold.video.height,cold.video.colorBits,cold.video.packetZMaximum,cold.video.zBufferFormat};
                m.optionsResources=std::make_shared<OriginalUiOptionsDrawResources50KG>();
                if(!m.optionsResources->load(input,d.optionsResources)){result(d,Status::resource);return false;}
                samplers=m.optionsResources->samplers();
            }else if(same(route,{5,1})){
                if(!riderInputsReady()||prospectiveMode>2){result(d,Status::unavailableDestination);return false;}
                OriginalUiRiderColdInput50KG input;input.appStart=cold.appStart;input.rider=cold.rider;input.fonts=cold.fonts;
                input.backgrounds=cold.backgrounds;input.overlays=cold.overlays;input.english=&english;input.runtime=cold.runtime;
                input.video={cold.video.lifetime,cold.video.width,cold.video.height,cold.video.colorBits,cold.video.packetZMaximum,cold.video.zBufferFormat};
                m.riderResources=std::make_shared<OriginalUiRiderDrawResources50KG>();
                m.riderStats=std::make_shared<assets::OriginalUiRiderStatAssets50KG>();
                m.riderBinder=std::make_shared<assets::OriginalUiRiderBinderAssets50KG>();
                if(!m.riderResources->load(input,d.riderResources)||
                   !m.riderStats->load(cold.rider,cold.fonts,cold.appStart,english,cold.runtime,d.riderStats)||
                   !m.riderBinder->load(cold.rider,cold.appStart,cold.runtime,d.riderBinder)){result(d,Status::resource);return false;}
                OriginalUiRiderSessionCold50KG session;session.resources=m.riderResources;session.statAssets=m.riderStats;
                session.binderAssets=m.riderBinder;session.riderArchive=cold.rider;session.appStart=cold.appStart;
                session.podTextures=cold.podTextures;session.ipumCache=cold.riderIpumCache;session.materialRuntime=cold.runtime;
                session.english=&english;session.profile=cold.riderProfile;session.commonProfile=cold.profile;session.application=cold.application;
                session.lightManager=cold.lightManager;session.externalTransition=m.transition;session.audioMode=cold.audioMode;session.audio=cold.audio;
                const bool fromCourse=active&&active->course&&same(active->currentRoute(),{6,1});
                if(fromCourse&&(!courseInputsReady()||prospectiveMode!=0)){result(d,Status::unavailableDestination);return false;}
                session.prospectiveMode=prospectiveMode;session.previousScreen=fromCourse?6:4;
                session.predecessorResources=active&&active->main?active->mainResources.get():nullptr;
                session.predecessorCourseResources=fromCourse?active->courseResources.get():nullptr;
                session.predecessorCourseHolograms=fromCourse?active->courseHolograms.get():nullptr;
                m.rider=std::make_unique<OriginalUiRiderSession50KG>();
                if(m.rider->bind(session,d.rider)!=RiderStatus::ok||!m.rider->canEnterMode(prospectiveMode)||
                   m.rider->prepareBank(d.rider)!=RiderStatus::ok){result(d,Status::session);return false;}
                samplers=m.rider->samplers();
                m.publishedRiderBank=m.rider->bank();
            }else if(same(route,{6,1})){
                if(!courseInputsReady()||prospectiveMode!=0||!active||!active->rider||!same(active->currentRoute(),{5,1})){
                    result(d,Status::unavailableDestination);return false;}
                OriginalUiCourseColdInput50KG input;input.appStart=cold.appStart;input.course=cold.course;
                input.fonts=cold.fonts;input.backgrounds=cold.backgrounds;input.controllerPopup=cold.controllerPopup;
                input.overlays=cold.overlays;input.english=&english;input.runtime=cold.runtime;
                input.video={cold.video.lifetime,cold.video.width,cold.video.height,cold.video.colorBits,cold.video.packetZMaximum,cold.video.zBufferFormat};
                m.courseResources=std::make_shared<OriginalUiCourseDrawResources50KG>();
                m.courseHolograms=std::make_shared<OriginalUiCourseHologramDraw50KG>();
                if(!m.courseResources->load(input,d.courseResources)||
                   !m.courseHolograms->load(cold.appStart,cold.course,cold.runtime,d.courseHolograms)){result(d,Status::resource);return false;}
                OriginalUiCourseSessionCold50KG session;session.resources=m.courseResources;session.holograms=m.courseHolograms;
                session.courseArchive=cold.course;session.english=&english;session.profile=cold.riderProfile;
                session.commonProfile=cold.profile;session.application=cold.application;session.lightManager=cold.lightManager;
                session.externalTransition=m.transition;session.audioMode=cold.audioMode;session.audio=cold.audio;session.previousScreen=5;
                m.course=std::make_unique<OriginalUiCourseSession50KG>();
                // bind prepares this immutable combined bank exactly once.
                if(m.course->bind(session,d.course)!=CourseStatus::ok||!m.course->bank()){result(d,Status::session);return false;}
                m.publishedCourseBank=m.course->bank();samplers=m.course->samplers();
            }else {result(d,Status::unavailableDestination);return false;}
            if(!samplers||!m.textures()){result(d,Status::resource);return false;}
            m.gpuSamplers=*samplers;
            m.gpu=std::make_shared<MaterialTextureGl50EB>();auto request=cold.textureRequest;
            request.samplers=samplers->data();request.samplerCount=static_cast<std::uint32_t>(samplers->size());
            if(!m.gpu->create(request,*m.textures(),d.gpu)){result(d,Status::gpu);return false;}
            m.publication=m.gpu->publicationToken();
            if(m.mainResources){
                OriginalUiMainSessionCold50KG input;input.resources=m.mainResources;input.gpu=m.gpu;input.mainArchive=cold.mainMenu;
                input.profile=cold.profile;input.application=cold.application;input.audioMode=cold.audioMode;input.audio=cold.audio;
                input.externalTransition=m.transition;m.main=std::make_unique<OriginalUiMainSession50KG>();
                if(m.main->bind(input,d.main)!=MainStatus::ok){result(d,Status::session);return false;}
            }else if(m.optionsResources){
                OriginalUiOptionsSessionCold50KG input;input.resources=m.optionsResources;input.gpu=m.gpu;input.optionsArchive=cold.options;
                input.profile=cold.optionsProfile;input.commonProfile=cold.profile;input.application=cold.application;
                input.gameSettingsProfile=cold.gameSettingsProfile;input.soundMode=cold.soundMode;
                input.audioMode=cold.audioMode;input.audio=cold.audio;input.externalTransition=m.transition;
                m.options=std::make_unique<OriginalUiOptionsSession50KG>();
                if(m.options->bind(input,d.options)!=OptionsStatus::ok){result(d,Status::session);return false;}
            }else if(m.course){
                if(m.course->bindPublishedBank(m.gpu,d.course)!=CourseStatus::ok){result(d,Status::session);return false;}
            }else if(!m.rider||m.rider->bindPublishedBank(m.gpu,d.rider)!=RiderStatus::ok){
                result(d,Status::session);return false;
            }
            if(!m.prepared()){result(d,Status::resource);return false;}return true;
        }catch(const std::bad_alloc&){result(d,Status::allocation);return false;}
        catch(...){result(d,Status::resource);return false;}
    }
    bool retireRider(Manager& m,Diagnostic& d) noexcept {
        if(!m.rider||m.riderRetired)return true;
        // The private session path obtains actual renderer retirement and
        // binds it to this collector's attempted generation/event frontier.
        // All dynamic releases and retirement consumption precede reset().
        if(m.rider->teardownSource(renderer,d.renderer,d.rider)!=RiderStatus::ok){result(d,Status::cleanup);return false;}
        m.riderRetired=true;return true;
    }
    bool refreshRiderBank(Manager& m,Diagnostic& d) noexcept {
        if(!m.rider)return true;
        if(m.rider->prepareBank(d.rider)!=RiderStatus::ok){result(d,Status::session);return false;}
        const auto next=m.rider->bank();const auto* samplers=m.rider->samplers();
        const auto* textures=next&&next->images()?next->images()->textures():nullptr;
        if(!textures||!samplers||!m.gpu||!m.gpu->initialized()||m.gpu->publicationToken()!=m.publication||
           textures->textureCount()!=m.gpu->textureCount()||samplers->size()!=m.gpuSamplers.size()||samplers->size()!=m.gpu->textureCount()){
            result(d,Status::resource);return false;
        }
        // Initial Rider19 cold admission preloads every real image. This
        // bounded screen never replaces a GPU image with an unadmitted rider.
        // Reuse requires exact canonical identities AND actual sampler policy.
        for(std::uint32_t i=0;i<m.gpu->textureCount();++i){const auto* gpu=m.gpu->asset(i);const auto* cpu=textures->texture(i);
            if(!gpu||!cpu||gpu->identity!=cpu->identity){result(d,Status::resource);return false;}
            const auto& a=(*samplers)[i];const auto& b=m.gpuSamplers[i];
            if(a.minification!=b.minification||a.magnification!=b.magnification||a.s!=b.s||a.t!=b.t){result(d,Status::resource);return false;}
        }
        if(m.rider->bindPublishedBank(m.gpu,d.rider)!=RiderStatus::ok){result(d,Status::session);return false;}
        if(renderer&&bank==&m&&next!=m.publishedRiderBank){
            if(!renderer->finishSubmittedWork(d.renderer)||!d.renderer.submittedWorkCompleted||
               !renderer->bindOriginalUiTextures(*textures,*m.gpu,d.renderer)){result(d,Status::renderer);return false;}
        }
        m.publishedRiderBank=next;return true;
    }
    bool verifyCourseBank(const Manager& m,Diagnostic& d) const noexcept {
        if(!m.course)return true;
        const auto current=m.course->bank();const auto* samplers=m.course->samplers();
        const auto* textures=current&&current->images()?current->images()->textures():nullptr;
        if(!current||current!=m.publishedCourseBank||!textures||!samplers||!m.gpu||!m.gpu->initialized()||
           m.gpu->publicationToken()!=m.publication||textures->textureCount()!=m.gpu->textureCount()||
           samplers->size()!=m.gpuSamplers.size()||samplers->size()!=m.gpu->textureCount()){result(d,Status::resource);return false;}
        for(std::uint32_t i=0;i<m.gpu->textureCount();++i){const auto* a=m.gpu->asset(i);const auto* b=textures->texture(i);
            if(!a||!b||a->identity!=b->identity){result(d,Status::resource);return false;}
            const auto& x=(*samplers)[i];const auto& y=m.gpuSamplers[i];
            if(x.minification!=y.minification||x.magnification!=y.magnification||x.s!=y.s||x.t!=y.t){result(d,Status::resource);return false;}}
        return true;
    }
    bool release(std::unique_ptr<Manager>& m,Diagnostic& d) noexcept {
        if(!m)return true;
        if(!retireRider(*m,d))return false;
        m->main.reset();m->options.reset();m->rider.reset();m->course.reset(); // No further callback can reach them.
        if(m->gpu&&!m->gpu->close(d.gpu))return false;
        m.reset();return true;
    }
    bool prepareDestination(Route target,OriginalUiFrontendInputReceipt50KG& receipt,Diagnostic& d,std::uint32_t prospectiveMode=0) noexcept {
        receipt.coldInterval.measured=true;receipt.coldInterval.begin=std::chrono::steady_clock::now();
        const bool okay=build(pending,target,d,prospectiveMode);
        if(!okay){
            d.preparationStatus=d.status; // Preserve actual failed cold stage before public unavailable mapping.
            Diagnostic cleanup;
            if(!release(pending,cleanup)){d.gpu=cleanup.gpu;fault(d,Status::cleanup);}
        }
        receipt.coldInterval.end=std::chrono::steady_clock::now();
        const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(receipt.coldInterval.end-receipt.coldInterval.begin).count();
        receipt.coldInterval.nanoseconds=elapsed>0?static_cast<std::uint64_t>(elapsed):0;
        return okay;
    }
};
OriginalUiFrontend50KG::OriginalUiFrontend50KG() noexcept=default;
OriginalUiFrontend50KG::~OriginalUiFrontend50KG() noexcept {
    // Explicit close is still required for GPU cleanup. Destruction without a
    // completed close cannot certify a reusable process light lifetime.
    if(storage_&&storage_->lightManagerEntered&&storage_->cold.lightManager)
        storage_->cold.lightManager->invalidateForUntrackedScene();
    if(storage_)for(const auto* manager:{storage_->retiring.get(),storage_->pending.get(),storage_->active.get()})
        if(manager&&manager->rider&&manager->rider->sourceTeardownPending()){
            // Terminal misuse after omitted/failed explicit close. Keep the
            // Manager callback context, session, ledger and every cold/GPU/
            // application owner together. This leaks deliberately; it does
            // not report cleanup success and cannot replace a close retry.
            storage_.release();break;
        }
}
Status OriginalUiFrontend50KG::prepare(const OriginalUiFrontendCold50KG& cold,Diagnostic& d) noexcept {
    d={};if(storage_)return result(d,Status::alreadyPrepared);
    using Id=assets::OriginalUiArchiveId50KG;
    if(!archive(cold.appStart,Id::appStart)||!archive(cold.mainMenu,Id::mainMenu)||!archive(cold.options,Id::options)||
        !archive(cold.fonts,Id::fonts)||!archive(cold.controllerPopup,Id::controllerPopup)||!archive(cold.overlays,Id::overlays)||
        !archive(cold.localization,Id::localization))return result(d,Status::resource);
    if(!cold.lightManager||
        cold.lightManager->cacheState()!=game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction||
        cold.lightManager->activeUiIdentity()||
        !cold.video.lifetime||!cold.textureRequest.context||!cold.textureRequest.isCurrent||
        !cold.textureRequest.isCurrent(cold.textureRequest.context)||!cold.textureRequest.maximumGpuBytes||
        cold.textureRequest.samplers||cold.textureRequest.samplerCount||
        (cold.audioMode!=OriginalUiMainAudioMode50KG::enabled&&cold.audioMode!=OriginalUiMainAudioMode50KG::muted))return result(d,Status::invalidInput);
    try {
        storage_=std::make_unique<Storage>();auto& s=*storage_;Operation operation(s.inOperation);s.cold=cold;
        if(!s.processReady())return s.fault(d,Status::invalidInput);
        if(!assets::readOriginalUiEnglish50KG(*cold.localization,s.english,d.localization))return s.fault(d,Status::localization);
        if(!s.build(s.active,{4,1},d))return s.fault(d,d.status);
        s.prepared=true;return result(d,Status::ok);
    }catch(const std::bad_alloc&){if(storage_)return storage_->fault(d,Status::allocation);return result(d,Status::allocation);}
    catch(...){if(storage_)return storage_->fault(d,Status::resource);return result(d,Status::resource);}
}
Status OriginalUiFrontend50KG::configureRendererRequest(VehicleWindowRequest50DS& request,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notPrepared);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->prepared)return result(d,Status::notPrepared);
    if(s->activated||s->closing||s->quit)return result(d,Status::busy);
    if(request.context!=s->cold.textureRequest.context||request.isCurrent!=s->cold.textureRequest.isCurrent||!s->active->prepared())return result(d,Status::invalidInput);
    request.enableOriginalUi=true;request.originalUiCpuTextures=s->active->textures();request.originalUiTextures=s->active->gpu.get();
    // Real renderer source target is created only when all optional original
    // inputs exist. This is capability allocation, not destination admission.
    request.enableOriginalUiSourceRgba=s->riderInputsReady();
    return result(d,Status::ok);
}
Status OriginalUiFrontend50KG::activate(VehicleWindowGl50DS& renderer,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notPrepared);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->prepared)return result(d,Status::notPrepared);
    if(s->activated||s->renderer||s->closing||s->quit)return result(d,Status::busy);
    Operation operation(s->inOperation);s->renderer=&renderer;
    if(!s->active->prepared()||!renderer.bindOriginalUiTextures(*s->active->textures(),*s->active->gpu,d.renderer))return s->fault(d,Status::renderer);
    if(!s->enterLightManager(*s->cold.mainMenu,d))return s->fault(d,Status::session,d.lightManager.sourcePC);
    s->bank=s->active.get();s->activating=s->active.get();
    d.transition=s->active->transition->bindInitialPage(4,1,s->callbacks(*s->active));s->activating=nullptr;
    if(d.transition!=Transition::ok||s->faulted)return s->fault(d,Status::transition,0x2A3C10);
    s->activated=true;return result(d,Status::ok);
}
Status OriginalUiFrontend50KG::input(const OriginalUiMainSessionInput50KG& input,
    OriginalUiFrontendInputReceipt50KG& receipt,Diagnostic& d) noexcept {
    d={};receipt={};auto* s=storage_.get();if(!s)return result(d,Status::notActive);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->activated)return result(d,Status::notActive);
    if(s->quit||s->closing)return result(d,Status::quit);
    // Do not discard an accepted source frame, especially the unique +440
    // endpoint. Its actual renderer submission precedes any next source tick.
    if(s->queued||s->updates!=s->submittedUpdate)return result(d,Status::busy);
    Operation operation(s->inOperation);s->frameRendered=false;s->frameSubmitted=false;
    if(!s->sourceCurrent()||!s->processReady())return s->fault(d,Status::session);
    receipt.source=s->active->currentRoute();s->mainProposal={};s->optionsProposal={};s->riderProposal={};s->courseProposal={};
    if(s->active->main){
        const auto status=s->active->main->input(input,receipt.main,d.main);
        if(status==MainStatus::routeProposed){
            s->mainProposal=receipt.main.routeProposal;const auto& decision=s->mainProposal.decision;
            const bool options=decision.selectedSlot==4&&decision.targetScreen==12&&decision.originalGameMode==-1&&!decision.originalPlayerBits;
            const bool rider=decision.selectedSlot>=1&&decision.selectedSlot<=3&&decision.targetScreen==5&&
                decision.originalGameMode==static_cast<std::int8_t>(decision.selectedSlot-1)&&
                decision.originalPlayerBits==(decision.selectedSlot==2?0x40:0x20)&&s->riderInputsReady()&&
                s->renderer->originalUiSourceRgbaAvailable();
            if(decision.action!=game::OriginalUiMainAction50KG::changeScreen||decision.routeArgument!=1||(!options&&!rider))
                return result(d,receipt.status=Status::unavailableDestination);
            if(s->pending||!s->active->main->routeProposalCurrent(s->mainProposal))return s->fault(d,Status::transition);
            s->mainProfileProposal=*s->cold.profile;
            if(!s->prepareDestination({static_cast<std::uint8_t>(decision.targetScreen),1},receipt,d,
                rider?static_cast<std::uint32_t>(decision.originalGameMode):0))return s->faulted?d.status:result(d,receipt.status=Status::unavailableDestination);
            if(!s->active->main->routeProposalCurrent(s->mainProposal))return s->fault(d,Status::transition);
            d.transition=s->active->transition->requestMain(decision);
            if(d.transition!=Transition::ok)return s->fault(d,Status::transition,0x1E6160);
            s->cold.profile->savedSlot6E0=decision.selectedSlot;
            if(s->active->main->notifyRouteAccepted(s->mainProposal,d.main)!=MainStatus::ok)return s->fault(d,Status::session,0x1E61B4);
        }else {
            const auto mapped=status==MainStatus::ok?Status::ok:status==MainStatus::noChange?Status::noChange:
                status==MainStatus::unavailableDestination?Status::unavailableDestination:status==MainStatus::secondControllerRequired?Status::secondControllerRequired:Status::session;
            if(mapped==Status::session)return s->fault(d,mapped);return result(d,receipt.status=mapped);
        }
    }else if(s->active->options){
        const auto status=s->active->options->input(input,receipt.options,d.options);
        if(status==OptionsStatus::backProposed||status==OptionsStatus::pageProposed){
            s->optionsProposal=receipt.options.routeProposal;
            const auto& proposal=s->optionsProposal.decision;
            const bool pageChange=status==OptionsStatus::pageProposed;
            if(!(pageChange?optionsPageShape(proposal):optionsBackShape(proposal))||s->pending||
                !s->active->options->routeProposalCurrent(s->optionsProposal))return s->fault(d,Status::transition);
            if(pageChange){
                if(!s->active->ownsRoute({proposal.request.targetScreen,proposal.request.targetPage}))
                    return result(d,receipt.status=Status::unavailableDestination);
            }else if(!s->prepareDestination({4,1},receipt,d))return s->faulted?d.status:result(d,receipt.status=Status::unavailableDestination);
            if(!s->active->options->routeProposalCurrent(s->optionsProposal))return s->fault(d,Status::transition);
            auto admitted=s->optionsProposal.decision;
            // Concrete destination now exists. Only its previously missing
            // destinationOwner blocker is discharged; no other flag is ignored.
            if(admitted.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)||admitted.admitted||
                admitted.action!=game::OriginalUiRouteAction50KG::unavailable)return s->fault(d,Status::transition);
            admitted.blockers=0;admitted.admitted=true;admitted.action=game::OriginalUiRouteAction50KG::request;
            d.transition=s->active->transition->requestRouteDecision(admitted);
            if(d.transition!=Transition::ok)return s->fault(d,Status::transition,proposal.request.sourcePC);
            if(s->active->options->notifyRouteAccepted(s->optionsProposal,d.options)!=OptionsStatus::ok)return s->fault(d,Status::session,d.options.sourcePC);
        }else {
            const auto mapped=status==OptionsStatus::ok?Status::ok:status==OptionsStatus::noChange?Status::noChange:
                status==OptionsStatus::unavailableDestination?Status::unavailableDestination:status==OptionsStatus::secondControllerRequired?Status::secondControllerRequired:Status::session;
            if(mapped==Status::session)return s->fault(d,mapped);return result(d,receipt.status=mapped);
        }
    }else if(s->active->rider){
        const auto status=s->active->rider->input(input,receipt.rider,d.rider);
        if(status==RiderStatus::backProposed||status==RiderStatus::confirmProposed){
            const bool confirm=status==RiderStatus::confirmProposed;
            s->riderProposal=receipt.rider.routeProposal;const auto& proposal=s->riderProposal.decision;
            if(s->pending||!s->active->rider->routeProposalCurrent(s->riderProposal)||
               proposal.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||proposal.request.sourceScreen!=5||
               proposal.request.sourcePage!=1||proposal.request.targetScreen!=(confirm?6:4)||proposal.request.targetPage!=1)
                return s->fault(d,Status::transition);
            if(confirm&&(!s->courseInputsReady()||proposal.selectedRider!=19||proposal.profileChanges.count!=3||
               proposal.request.sourcePC!=0x1EA604))return result(d,receipt.status=Status::unavailableDestination);
            if(!s->prepareDestination({static_cast<std::uint8_t>(confirm?6:4),1},receipt,d))return s->faulted?d.status:result(d,receipt.status=Status::unavailableDestination);
            if(!s->active->rider->routeProposalCurrent(s->riderProposal))return s->fault(d,Status::transition);
            auto admitted=proposal;
            if(admitted.admitted||admitted.action!=game::OriginalUiRouteAction50KG::unavailable||
               admitted.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner))return s->fault(d,Status::transition);
            admitted.blockers=0;admitted.admitted=true;admitted.action=game::OriginalUiRouteAction50KG::request;
            d.transition=s->active->transition->requestRouteDecision(admitted);
            if(d.transition!=Transition::ok||s->active->rider->notifyRouteAccepted(s->riderProposal,d.rider)!=RiderStatus::ok)
                return s->fault(d,Status::transition,proposal.request.sourcePC);
        }else{
            const auto mapped=status==RiderStatus::ok?Status::ok:status==RiderStatus::noChange?Status::noChange:
                status==RiderStatus::unavailableDestination?Status::unavailableDestination:Status::session;
            if(mapped==Status::session)return s->fault(d,mapped);return result(d,receipt.status=mapped);
        }
    }else if(s->active->course){
        const auto status=s->active->course->input(input,receipt.course,d.course);
        if(status==CourseStatus::confirmProposed){
            s->courseProposal=receipt.course.routeProposal;
            OriginalUiRiderProfileOwner50KG::CourseRaceSelection selection;
            if(!s->courseLaunchProposal(selection))return result(d,receipt.status=Status::unavailableDestination,0x1DED58);
            auto admitted=s->courseProposal.decision;
            if(admitted.admitted||admitted.action!=game::OriginalUiRouteAction50KG::unavailable||
               admitted.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::courseLaunchOwner))
                return s->fault(d,Status::transition);
            admitted.blockers=0;admitted.admitted=true;admitted.action=game::OriginalUiRouteAction50KG::request;
            d.transition=s->active->transition->requestRouteDecision(admitted);
            if(d.transition!=Transition::ok||s->active->course->notifyRouteAccepted(s->courseProposal,d.course)!=CourseStatus::ok)
                return s->fault(d,Status::transition,0x1DED58);
        }else if(status==CourseStatus::backProposed){
            s->courseProposal=receipt.course.routeProposal;const auto& proposal=s->courseProposal.decision;
            if(s->pending||!s->active->course->routeProposalCurrent(s->courseProposal)||
               proposal.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||proposal.request.sourceScreen!=6||
               proposal.request.sourcePage!=1||proposal.request.targetScreen!=5||proposal.request.targetPage!=1||
               proposal.profileChanges.count!=4)return s->fault(d,Status::transition);
            if(!s->courseInputsReady()||!s->riderInputsReady()||!s->renderer->originalUiSourceRgbaAvailable())
                return result(d,receipt.status=Status::unavailableDestination);
            if(!s->prepareDestination({5,1},receipt,d,0))return s->faulted?d.status:result(d,receipt.status=Status::unavailableDestination);
            if(!s->active->course->routeProposalCurrent(s->courseProposal))return s->fault(d,Status::transition);
            auto admitted=proposal;
            if(admitted.admitted||admitted.action!=game::OriginalUiRouteAction50KG::unavailable||
               admitted.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner))return s->fault(d,Status::transition);
            admitted.blockers=0;admitted.admitted=true;admitted.action=game::OriginalUiRouteAction50KG::request;
            d.transition=s->active->transition->requestRouteDecision(admitted);
            if(d.transition!=Transition::ok||s->active->course->notifyRouteAccepted(s->courseProposal,d.course)!=CourseStatus::ok)
                return s->fault(d,Status::transition,proposal.request.sourcePC);
        }else{
            const auto mapped=status==CourseStatus::ok?Status::ok:status==CourseStatus::noChange?Status::noChange:
                status==CourseStatus::unavailableDestination?Status::unavailableDestination:Status::session;
            if(mapped==Status::session)return s->fault(d,mapped);return result(d,receipt.status=mapped);
        }
    }else return s->fault(d,Status::session);
    receipt.routeAccepted=true;receipt.requested=s->active->transition->snapshot().requested;
    s->mainProposal={};s->optionsProposal={};s->riderProposal={};s->courseProposal={};return result(d,receipt.status=Status::routeAccepted);
}
Status OriginalUiFrontend50KG::update(const OriginalUiMainSessionUpdate50KG& input,
    OriginalUiFrontendUpdateReceipt50KG& receipt,Diagnostic& d) noexcept {
    d={};receipt={};auto* s=storage_.get();if(!s)return result(d,Status::notActive);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->activated)return result(d,Status::notActive);
    if(s->quit||s->closing)return result(d,Status::quit);
    // Do not discard an accepted source frame, especially the unique +440
    // endpoint. Its actual renderer submission precedes any next source tick.
    if(s->queued||s->updates!=s->submittedUpdate)return result(d,Status::busy);
    Operation operation(s->inOperation);s->frameRendered=false;s->frameSubmitted=false;
    if(!s->sourceCurrent()||!s->processReady())return s->fault(d,Status::session);
    auto& m=*s->active;
    if(m.course&&m.course->snapshot().deferred14D0==1){
        // Native equivalent of1DFE18..1DFE78: all Course images/materials are
        // already resident in the exact immutable bank. Retire submitted GPU
        // work and rebind that real bank; do not fake original DMA completion,
        // replace images, reconstruct the manager or emulate video registers.
        if(!s->renderer||s->bank!=&m||!s->verifyCourseBank(m,d)||
           !s->renderer->finishSubmittedWork(d.renderer)||!d.renderer.submittedWorkCompleted||
           !s->renderer->bindOriginalUiTextures(*m.textures(),*m.gpu,d.renderer))return s->fault(d,Status::renderer,0x1DFE18);
        if(m.course->completeSelectionRefresh(d.course)!=CourseStatus::ok)return s->fault(d,Status::session,0x1DFE58);
    }
    const bool prepared=m.main?m.main->prepareUpdateAudio(input,d.main)==MainStatus::ok:m.options?
        m.options->prepareUpdateAudio(input,d.options)==OptionsStatus::ok:m.rider?m.rider->prepareUpdateAudio(input,d.rider)==RiderStatus::ok:
        m.course&&m.course->prepareUpdateAudio(input,d.course)==CourseStatus::ok;
    if(!prepared)return s->fault(d,Status::session,0x234440);
    d.transition=m.transition->tick();
    if(s->faulted)return s->fault(d,Status::session);
    if(d.transition!=Transition::ok&&d.transition!=Transition::noChange&&d.transition!=Transition::pending&&d.transition!=Transition::queued)
        return s->fault(d,Status::transition,0x2A3C80);
    const bool updated=m.main?m.main->update(input,d.main)==MainStatus::ok:m.options?
        m.options->update(input,d.options)==OptionsStatus::ok:m.rider?m.rider->update(input,d.rider)==RiderStatus::ok:
        m.course&&m.course->update(input,d.course)==CourseStatus::ok;
    if(!updated)return s->fault(d,Status::session);
    if(!s->refreshRiderBank(m,d)||!s->verifyCourseBank(m,d))return s->fault(d,d.status);
    ++s->updates;receipt.accepted=true;receipt.sequence=s->updates;receipt.update=input;receipt.source=m.currentRoute();
    return result(d,Status::ok);
}
Status OriginalUiFrontend50KG::render(const OriginalUiMainFrameClock50KG& clock,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notActive);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->activated)return result(d,Status::notActive);
    if(s->quit||s->closing)return result(d,Status::quit);
    if(!s->updates||s->renderedUpdate==s->updates)return result(d,Status::invalidInput);
    Operation operation(s->inOperation);s->frameRendered=false;s->frameSubmitted=false;
    if(!s->sourceCurrent())return s->fault(d,Status::session);
    auto& m=*s->active;const bool rendered=m.main?m.main->render(clock,d.main)==MainStatus::ok:m.options?
        m.options->render({clock.known,clock.advanceAnimations,clock.milliseconds},d.options)==OptionsStatus::ok:
        m.rider?m.rider->render({clock.known,clock.advanceAnimations,clock.milliseconds},d.rider)==RiderStatus::ok:
        m.course&&m.course->render({clock.known,clock.advanceAnimations,clock.milliseconds},d.course)==CourseStatus::ok;
    if(!rendered||(m.rider?!m.rider->sourceFrame():!m.draw()))return s->fault(d,Status::session);
    s->renderedUpdate=s->updates;++s->renders;s->frameRendered=true;return result(d,Status::ok);
}
Status OriginalUiFrontend50KG::submit(VehicleWindowGl50DS& renderer,std::int32_t width,std::int32_t height,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notActive);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->activated)return result(d,Status::notActive);
    if(s->quit||s->closing)return result(d,Status::quit);
    if(&renderer!=s->renderer||s->bank!=s->active.get()||!s->frameRendered||s->frameSubmitted||s->renderedUpdate!=s->updates)
        return result(d,Status::invalidInput);
    Operation operation(s->inOperation);
    if(!s->sourceCurrent())return s->fault(d,Status::session);
    if(s->active->rider){const auto* frame=s->active->rider->sourceFrame();
        if(!frame||!renderer.submitOriginalUiSourceRgba(*frame,width,height,d.renderer)||!d.renderer.originalUiAccepted)return s->fault(d,Status::renderer);
    }else{const auto* draw=s->active->draw();
        if(!draw||!renderer.submitOriginalUi(*draw,width,height,d.renderer)||!d.renderer.originalUiAccepted)return s->fault(d,Status::renderer);
    }
    s->frameSubmitted=true;s->submittedUpdate=s->updates;++s->submissions;return result(d,Status::ok);
}
Status OriginalUiFrontend50KG::commitDeferred(VehicleWindowGl50DS& renderer,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notActive);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->activated)return result(d,Status::notActive);
    if(s->quit||s->closing)return result(d,Status::quit);if(!s->queued)return result(d,Status::noChange);
    if(&renderer!=s->renderer||s->bank!=s->active.get()||!s->frameSubmitted||s->submittedUpdate!=s->updates||s->retiring)
        return result(d,Status::invalidInput);
    Operation operation(s->inOperation);
    const auto old=s->active->transition->snapshot();const Route target{static_cast<std::uint8_t>(s->queue.screen),static_cast<std::uint8_t>(s->queue.page)};
    if(s->queue.flags==9){
        OriginalUiRiderProfileOwner50KG::CourseRaceSelection actual;
        if(!old.applicationRequestQueued||old.lifecycle!=Life::queueCourse||!s->active->course||s->pending||
           !s->active->course->snapshot().launchAccepted||!s->ownsLightManager()||
           !s->cold.riderProfile->courseRaceSnapshot(actual)||!s->cold.courseRaceDestination||
           !s->cold.courseRaceDestination->admits(actual))return s->fault(d,Status::transition,0x2A3F98);
        if(!renderer.finishSubmittedWork(d.renderer)||!d.renderer.submittedWorkCompleted)return s->fault(d,Status::renderer);
        s->active->course->drain();
        if(!s->releaseLightManager(d))return s->fault(d,Status::cleanup,d.lightManager.sourcePC);
        // Keep immutable texture/model owners pinned because the same renderer
        // retains their bank pointers. close() retires them after the race.
        s->launchSelection=actual;s->courseLaunchReady=true;s->activated=false;
        s->queued=false;s->frameRendered=false;s->frameSubmitted=false;
        return result(d,Status::courseLaunchReady,0x2A3F98);
    }
    if(!old.applicationRequestQueued||old.lifecycle!=Life::queueScreen||!same(old.requested,target)||!s->destination(target))return s->fault(d,Status::transition);
    if(!s->ownsLightManager())return s->fault(d,Status::session,0x148110);
    const auto* destinationArchive=s->pending->main?s->cold.mainMenu.get():
        (s->pending->options?s->cold.options.get():s->pending->rider?s->cold.rider.get():s->pending->course?s->cold.course.get():nullptr);
    if(!destinationArchive)return s->fault(d,Status::unavailableDestination,0x1CCD88);
    if(!renderer.finishSubmittedWork(d.renderer)||!d.renderer.submittedWorkCompleted)return s->fault(d,Status::renderer);
    if(!s->retireRider(*s->active,d))return s->fault(d,Status::cleanup);
    if(!renderer.bindOriginalUiTextures(*s->pending->textures(),*s->pending->gpu,d.renderer))return s->fault(d,Status::renderer);
    // All outgoing source submissions have completed. Switch the shared light
    // manager BEFORE incoming page callbacks, while retaining old cold/GPU
    // resources for the existing retirement boundary. Page-only Options
    // changes never call this function and keep the same manager generation.
    if(!s->releaseLightManager(d))return s->fault(d,Status::cleanup,d.lightManager.sourcePC);
    if(!s->enterLightManager(*destinationArchive,d))return s->fault(d,Status::session,d.lightManager.sourcePC);
    s->bank=s->pending.get();s->activating=s->pending.get();
    d.transition=s->pending->transition->bindInitialPage(target.screen,target.page,s->callbacks(*s->pending));s->activating=nullptr;
    if(d.transition!=Transition::ok||s->faulted)return s->fault(d,Status::transition,0x2A3C10);
    s->retiring=std::move(s->active);s->active=std::move(s->pending);
    s->queued=false;s->queue={};s->frameRendered=false;s->frameSubmitted=false;
    s->renderedUpdate=s->updates; // Fresh destination needs its own real update.
    ++s->switches;
    if(!s->release(s->retiring,d))return s->fault(d,Status::cleanup);
    return result(d,Status::ok);
}
Status OriginalUiFrontend50KG::close(Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::noChange);
    if(s->inOperation)return result(d,Status::busy);
    {
        Operation operation(s->inOperation);s->closing=true;s->frameRendered=false;s->frameSubmitted=false;
        if(s->renderer){
            // Source owner retirement must occur while the exact live renderer
            // context exists. Failed retirement retains every owner for retry.
            for(auto* manager:{s->retiring.get(),s->pending.get(),s->active.get()})
                if(manager&&!s->retireRider(*manager,d))return result(d,Status::cleanup);
            // Retain the exact borrowed object even if failed close ended
            // resource ownership. An idempotent successful retry clears it.
            if(!s->renderer->close(d.renderer))return result(d,Status::cleanup);
            s->renderer=nullptr;s->bank=nullptr;
        }
        if(!s->release(s->retiring,d))return result(d,Status::cleanup);
        if(!s->release(s->pending,d))return result(d,Status::cleanup);
        if(!s->release(s->active,d))return result(d,Status::cleanup);
        // Cleanup retries retain this lease until every renderer/session/GPU
        // consumer is gone. Per-manager release above must not touch it.
        if(!s->releaseLightManager(d))return result(d,Status::cleanup,d.lightManager.sourcePC);
    }
    storage_.reset();return result(d,Status::ok);
}
bool OriginalUiFrontend50KG::takeCourseLaunch(OriginalUiRiderProfileOwner50KG::CourseRaceSelection& out)noexcept{
    auto* s=storage_.get();if(!s||s->faulted||s->quit||s->closing||s->inOperation||!s->courseLaunchReady||s->courseLaunchTaken)return false;
    out=s->launchSelection;s->courseLaunchTaken=true;return true;
}
bool OriginalUiFrontend50KG::borrowsRenderer(const VehicleWindowGl50DS& renderer) const noexcept {
    return storage_&&storage_->renderer==&renderer;
}
void OriginalUiFrontend50KG::requestQuit() noexcept {
    auto* s=storage_.get();if(!s||s->inOperation)return;
    s->quit=true;s->frameRendered=false;s->frameSubmitted=false;
    if(s->active){if(s->active->main)s->active->main->requestQuit();if(s->active->options)s->active->options->requestQuit();
        if(s->active->rider)s->active->rider->requestQuit();if(s->active->course)s->active->course->requestQuit();}
}
OriginalUiFrontendSnapshot50KG OriginalUiFrontend50KG::snapshot() const noexcept {
    OriginalUiFrontendSnapshot50KG out;const auto* s=storage_.get();if(!s)return out;
    out.prepared=s->prepared;out.active=s->activated;out.faulted=s->faulted;out.quitRequested=s->quit;
    out.courseLaunchReady=s->courseLaunchReady;out.deferredQueue=s->queued;out.frameSubmitted=s->frameSubmitted;out.queuedRequest=s->queue;
    out.updates=s->updates;out.renders=s->renders;out.submissions=s->submissions;out.switches=s->switches;
    if(s->cold.lightManager){out.lightCacheState=s->cold.lightManager->cacheState();
        out.lightManagerGeneration=s->cold.lightManager->managerGeneration();}
    out.lightManagerEntered=s->ownsLightManager();
    if(s->active){out.route=s->active->currentRoute();if(s->active->transition)out.transition=s->active->transition->snapshot();
        if(s->active->main)out.main=s->active->main->snapshot();if(s->active->options)out.options=s->active->options->snapshot();
        if(s->active->rider)out.rider=s->active->rider->snapshot();if(s->active->course)out.course=s->active->course->snapshot();}
    if(s->pending)out.preparedDestination=s->pending->initialRoute;return out;
}
const OriginalUiMainSession50KG* OriginalUiFrontend50KG::mainSession() const noexcept {return storage_&&storage_->active?storage_->active->main.get():nullptr;}
const OriginalUiOptionsSession50KG* OriginalUiFrontend50KG::optionsSession() const noexcept {return storage_&&storage_->active?storage_->active->options.get():nullptr;}
const OriginalUiRiderSession50KG* OriginalUiFrontend50KG::riderSession() const noexcept {return storage_&&storage_->active?storage_->active->rider.get():nullptr;}
const OriginalUiCourseSession50KG* OriginalUiFrontend50KG::courseSession() const noexcept {return storage_&&storage_->active?storage_->active->course.get():nullptr;}
const OriginalUiCourseDrawResources50KG* OriginalUiFrontend50KG::courseResources() const noexcept {return storage_&&storage_->active?storage_->active->courseResources.get():nullptr;}
const OriginalUiMainDrawResources50KG* OriginalUiFrontend50KG::mainResources() const noexcept {return storage_&&storage_->active?storage_->active->mainResources.get():nullptr;}
const OriginalUiOptionsDrawResources50KG* OriginalUiFrontend50KG::optionsResources() const noexcept {return storage_&&storage_->active?storage_->active->optionsResources.get():nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiFrontend50KG::textures() const noexcept {return storage_&&storage_->active?storage_->active->textures():nullptr;}
const MaterialTextureGl50EB* OriginalUiFrontend50KG::gpu() const noexcept {return storage_&&storage_->active?storage_->active->gpu.get():nullptr;}
const OriginalUiNativeDraw50KG* OriginalUiFrontend50KG::draw() const noexcept {
    const auto* s=storage_.get();return s&&!s->inOperation&&!s->faulted&&!s->quit&&!s->closing&&s->frameRendered&&s->active?s->active->draw():nullptr;
}
}

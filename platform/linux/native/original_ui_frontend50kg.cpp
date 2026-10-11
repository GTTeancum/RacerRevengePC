#include "original_ui_frontend50kg.h"
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
Status result(Diagnostic& d,Status s,std::uint32_t pc=0) noexcept {d.status=s;d.sourcePC=pc;return s;}
bool same(Route a,Route b) noexcept {return a.screen==b.screen&&a.page==b.page;}
bool archive(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,assets::OriginalUiArchiveId50KG id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
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
    const auto& r=d.request;
    if(r.kind!=game::OriginalUiRouteKind50KG::changePage||r.sourceScreen!=12||r.targetScreen!=12||
        !d.profileProposalComplete||d.selectedRider!=-1||d.profileChanges.count!=1)return false;
    const auto& p=d.profileChanges.values[0];
    if(p.operation!=game::OriginalUiProfileWrite50KG::word||p.value!=d.selectedSlot||p.replaceMask!=0xFFFFFFFFu)return false;
    if(r.sourcePage==1&&r.targetPage==4)
        return d.inputPC==0x1F9640&&d.selectedSlot==3&&r.sourcePC==0x1F9898&&
            p.field==game::OriginalUiProfileField50KG::optionsSlot6E4&&p.sourcePC==0x1F9940;
    // Cancel only. Accept's distinct profile/apply transaction is unavailable.
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
        std::shared_ptr<MaterialTextureGl50EB> gpu;
        std::uint64_t publication=0;
        std::shared_ptr<game::OriginalUiTransitionOwner50KG> transition;
        std::unique_ptr<OriginalUiMainSession50KG> main;
        std::unique_ptr<OriginalUiOptionsSession50KG> options;
        Route currentRoute() const noexcept {
            return transition&&transition->snapshot().bound?transition->snapshot().current:initialRoute;
        }
        bool ownsRoute(Route route) const noexcept {
            return route.screen==initialRoute.screen&&
                (main?same(route,{4,1}):(options&&options->ownsPreparedPage(route.page)));
        }
        const assets::MaterialTextureAssets50EA* textures() const noexcept {
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
            return false;
        }
        game::OriginalUiApertureSnapshot50KG aperture() const noexcept {return main?main->snapshot().aperture:options->snapshot().aperture;}
        const OriginalUiNativeDraw50KG* draw() const noexcept {return main?main->draw():(options?options->draw():nullptr);}
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
    game::OriginalUiApplicationRequest50KG queue{};
    OriginalUiMainRouteProposal50KG mainProposal{};
    OriginalUiOptionsRouteProposal50KG optionsProposal{};
    std::uint64_t updates=0,renders=0,submissions=0,switches=0,renderedUpdate=0,submittedUpdate=0;
    Diagnostic failure{};
    Status fault(Diagnostic& d,Status s,std::uint32_t pc=0) noexcept {
        if(!faulted){result(d,s,pc);failure=d;faulted=true;}
        frameRendered=false;frameSubmitted=false;d=failure;return d.status;
    }
    bool processReady() const noexcept {
        return cold.profile&&cold.profile->known&&cold.profile->savedSlot6E0>=1&&cold.profile->savedSlot6E0<=5&&
            cold.optionsProfile&&cold.optionsProfile->known&&cold.optionsProfile->savedSlot6E4>=1&&cold.optionsProfile->savedSlot6E4<=6&&
            (!cold.gameSettingsProfile||game::validOriginalUiGameSettingsProfile50KG(*cold.gameSettingsProfile))&&
            cold.application&&cold.application->known&&!(cold.application->globalFlags228&4u)&&cold.application->transportE00==-1&&
            (cold.audioMode==OriginalUiMainAudioMode50KG::muted?!cold.audio:(cold.audio&&cold.audio->ready()));
    }
    bool sourceCurrent() const noexcept {
        return activated&&renderer&&renderer->initialized()&&bank==active.get()&&active&&active->prepared()&&
            active->transition->snapshot().bound&&active->ownsRoute(active->transition->snapshot().current);
    }
    bool destination(Route route) const noexcept {
        return pending&&same(pending->initialRoute,route)&&pending->prepared()&&!pending->transition->snapshot().bound&&processReady();
    }
    bool sourceProposalCurrent() const noexcept {
        return active&&(active->main?active->main->routeProposalCurrent(mainProposal):active->options->routeProposalCurrent(optionsProposal));
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
        const Route target{static_cast<std::uint8_t>(m.currentRoute().screen==4?12:4),1};
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
        const bool okay=m.main?m.main->notifyTransition(event,d.main)==MainStatus::ok:
            m.options->notifyTransition(event,d.options)==OptionsStatus::ok;
        if(!okay)s.fault(d,Status::session,event.sourcePC);
    }
    static game::OriginalUiTransitionCompletion50KG poll(void* p,Route route,Phase phase) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        const auto t=m.transition->snapshot();
        if(s.faulted||s.quit||&m!=s.active.get()||!s.sourceCurrent()||!same(route,t.current)||phase!=t.phase)
            return game::OriginalUiTransitionCompletion50KG::unavailable;
        return m.main?m.main->pollCompletion(route,phase):m.options->pollCompletion(route,phase);
    }
    static bool commitOptions(void* p,const game::OriginalUiRouteDecision50KG& decision) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
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
                !m.ownsRoute({decision.request.targetScreen,decision.request.targetPage}))return false;
            if(decision.request.sourcePage==4){
                if(!s.cold.gameSettingsProfile)return false;
                s.cold.gameSettingsProfile->savedGroup6E8=decision.profileChanges.values[0].value;
            }else s.cold.optionsProfile->savedSlot6E4=decision.profileChanges.values[0].value;
        }else return false;
        return true;
    }
    static bool enqueue(void* p,const game::OriginalUiApplicationRequest50KG& q) noexcept {
        auto& m=*static_cast<Manager*>(p);auto& s=*m.owner;
        if(s.faulted||s.quit||s.queued||&m!=s.active.get()||!s.sourceCurrent()||q.flags!=2||q.sourcePC!=0x2A3F70||
            q.screen>255||q.page!=1||!s.destination({static_cast<std::uint8_t>(q.screen),1}))return false;
        const auto t=m.transition->snapshot();const auto a=m.aperture();
        if(t.lifecycle!=Life::queueScreen||t.phase!=Phase::none||t.applicationRequestQueued||
            t.requested.screen!=q.screen||t.requested.page!=q.page||a.state!=game::OriginalUiApertureState50KG::closed||a.inputEnabled268)return false;
        s.queue=q;s.queued=true;return true; // No switch, binding, teardown or GL.
    }
    game::OriginalUiTransitionCallbacks50KG callbacks(Manager& m) noexcept {
        game::OriginalUiTransitionCallbacks50KG c;c.context=&m;c.admitDestination=&admit;c.notify=&notify;
        c.pollCompletion=&poll;c.commitRouteSelection=&commitOptions;c.queueApplication=&enqueue;return c;
    }
    bool build(std::unique_ptr<Manager>& out,Route route,Diagnostic& d) noexcept {
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
            }else {result(d,Status::unavailableDestination);return false;}
            if(!samplers||!m.textures()){result(d,Status::resource);return false;}
            m.gpu=std::make_shared<MaterialTextureGl50EB>();auto request=cold.textureRequest;
            request.samplers=samplers->data();request.samplerCount=static_cast<std::uint32_t>(samplers->size());
            if(!m.gpu->create(request,*m.textures(),d.gpu)){result(d,Status::gpu);return false;}
            m.publication=m.gpu->publicationToken();
            if(m.mainResources){
                OriginalUiMainSessionCold50KG input;input.resources=m.mainResources;input.gpu=m.gpu;input.mainArchive=cold.mainMenu;
                input.profile=cold.profile;input.application=cold.application;input.audioMode=cold.audioMode;input.audio=cold.audio;
                input.externalTransition=m.transition;m.main=std::make_unique<OriginalUiMainSession50KG>();
                if(m.main->bind(input,d.main)!=MainStatus::ok){result(d,Status::session);return false;}
            }else {
                OriginalUiOptionsSessionCold50KG input;input.resources=m.optionsResources;input.gpu=m.gpu;input.optionsArchive=cold.options;
                input.profile=cold.optionsProfile;input.commonProfile=cold.profile;input.application=cold.application;
                input.gameSettingsProfile=cold.gameSettingsProfile;
                input.audioMode=cold.audioMode;input.audio=cold.audio;input.externalTransition=m.transition;
                m.options=std::make_unique<OriginalUiOptionsSession50KG>();
                if(m.options->bind(input,d.options)!=OptionsStatus::ok){result(d,Status::session);return false;}
            }
            if(!m.prepared()){result(d,Status::resource);return false;}return true;
        }catch(const std::bad_alloc&){result(d,Status::allocation);return false;}
        catch(...){result(d,Status::resource);return false;}
    }
    bool release(std::unique_ptr<Manager>& m,Diagnostic& d) noexcept {
        if(!m)return true;
        m->main.reset();m->options.reset(); // No further callback can reach them.
        if(m->gpu&&!m->gpu->close(d.gpu))return false;
        m.reset();return true;
    }
    bool prepareDestination(Route target,OriginalUiFrontendInputReceipt50KG& receipt,Diagnostic& d) noexcept {
        receipt.coldInterval.measured=true;receipt.coldInterval.begin=std::chrono::steady_clock::now();
        const bool okay=build(pending,target,d);
        if(!okay){
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
OriginalUiFrontend50KG::~OriginalUiFrontend50KG() noexcept=default;
Status OriginalUiFrontend50KG::prepare(const OriginalUiFrontendCold50KG& cold,Diagnostic& d) noexcept {
    d={};if(storage_)return result(d,Status::alreadyPrepared);
    using Id=assets::OriginalUiArchiveId50KG;
    if(!archive(cold.appStart,Id::appStart)||!archive(cold.mainMenu,Id::mainMenu)||!archive(cold.options,Id::options)||
        !archive(cold.fonts,Id::fonts)||!archive(cold.controllerPopup,Id::controllerPopup)||!archive(cold.overlays,Id::overlays)||
        !archive(cold.localization,Id::localization))return result(d,Status::resource);
    if(!cold.video.lifetime||!cold.textureRequest.context||!cold.textureRequest.isCurrent||
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
    return result(d,Status::ok);
}
Status OriginalUiFrontend50KG::activate(VehicleWindowGl50DS& renderer,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notPrepared);
    if(s->inOperation)return result(d,Status::busy);if(s->faulted){d=s->failure;return d.status;}
    if(!s->prepared)return result(d,Status::notPrepared);
    if(s->activated||s->renderer||s->closing||s->quit)return result(d,Status::busy);
    Operation operation(s->inOperation);s->renderer=&renderer;
    if(!s->active->prepared()||!renderer.bindOriginalUiTextures(*s->active->textures(),*s->active->gpu,d.renderer))return s->fault(d,Status::renderer);
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
    receipt.source=s->active->currentRoute();s->mainProposal={};s->optionsProposal={};
    if(s->active->main){
        const auto status=s->active->main->input(input,receipt.main,d.main);
        if(status==MainStatus::routeProposed){
            s->mainProposal=receipt.main.routeProposal;const auto& decision=s->mainProposal.decision;
            if(decision.action!=game::OriginalUiMainAction50KG::changeScreen||decision.selectedSlot!=4||decision.targetScreen!=12||
                decision.routeArgument!=1||decision.originalGameMode!=-1||decision.originalPlayerBits)
                return result(d,receipt.status=Status::unavailableDestination);
            if(s->pending||!s->active->main->routeProposalCurrent(s->mainProposal))return s->fault(d,Status::transition);
            if(!s->prepareDestination({12,1},receipt,d))return s->faulted?d.status:result(d,receipt.status=Status::unavailableDestination);
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
    }else {
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
    }
    receipt.routeAccepted=true;receipt.requested=s->active->transition->snapshot().requested;
    s->mainProposal={};s->optionsProposal={};return result(d,receipt.status=Status::routeAccepted);
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
    const bool prepared=m.main?m.main->prepareUpdateAudio(input,d.main)==MainStatus::ok:m.options->prepareUpdateAudio(input,d.options)==OptionsStatus::ok;
    if(!prepared)return s->fault(d,Status::session,0x234440);
    d.transition=m.transition->tick();
    if(s->faulted)return s->fault(d,Status::session);
    if(d.transition!=Transition::ok&&d.transition!=Transition::noChange&&d.transition!=Transition::pending&&d.transition!=Transition::queued)
        return s->fault(d,Status::transition,0x2A3C80);
    const bool updated=m.main?m.main->update(input,d.main)==MainStatus::ok:m.options->update(input,d.options)==OptionsStatus::ok;
    if(!updated)return s->fault(d,Status::session);
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
    auto& m=*s->active;const bool rendered=m.main?m.main->render(clock,d.main)==MainStatus::ok:
        m.options->render({clock.known,clock.advanceAnimations,clock.milliseconds},d.options)==OptionsStatus::ok;
    if(!rendered||!m.draw())return s->fault(d,Status::session);
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
    const auto* draw=s->active->draw();
    if(!draw||!renderer.submitOriginalUi(*draw,width,height,d.renderer)||!d.renderer.originalUiAccepted)return s->fault(d,Status::renderer);
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
    if(!old.applicationRequestQueued||old.lifecycle!=Life::queueScreen||!same(old.requested,target)||!s->destination(target))return s->fault(d,Status::transition);
    if(!renderer.finishSubmittedWork(d.renderer)||!d.renderer.submittedWorkCompleted)return s->fault(d,Status::renderer);
    if(!renderer.bindOriginalUiTextures(*s->pending->textures(),*s->pending->gpu,d.renderer))return s->fault(d,Status::renderer);
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
            // Retain the exact borrowed object even if failed close ended
            // resource ownership. An idempotent successful retry clears it.
            if(!s->renderer->close(d.renderer))return result(d,Status::cleanup);
            s->renderer=nullptr;s->bank=nullptr;
        }
        if(!s->release(s->retiring,d))return result(d,Status::cleanup);
        if(!s->release(s->pending,d))return result(d,Status::cleanup);
        if(!s->release(s->active,d))return result(d,Status::cleanup);
    }
    storage_.reset();return result(d,Status::ok);
}
void OriginalUiFrontend50KG::requestQuit() noexcept {
    auto* s=storage_.get();if(!s||s->inOperation)return;
    s->quit=true;s->frameRendered=false;s->frameSubmitted=false;
    if(s->active){if(s->active->main)s->active->main->requestQuit();if(s->active->options)s->active->options->requestQuit();}
}
OriginalUiFrontendSnapshot50KG OriginalUiFrontend50KG::snapshot() const noexcept {
    OriginalUiFrontendSnapshot50KG out;const auto* s=storage_.get();if(!s)return out;
    out.prepared=s->prepared;out.active=s->activated;out.faulted=s->faulted;out.quitRequested=s->quit;
    out.deferredQueue=s->queued;out.frameSubmitted=s->frameSubmitted;out.queuedRequest=s->queue;
    out.updates=s->updates;out.renders=s->renders;out.submissions=s->submissions;out.switches=s->switches;
    if(s->active){out.route=s->active->currentRoute();if(s->active->transition)out.transition=s->active->transition->snapshot();
        if(s->active->main)out.main=s->active->main->snapshot();if(s->active->options)out.options=s->active->options->snapshot();}
    if(s->pending)out.preparedDestination=s->pending->initialRoute;return out;
}
const OriginalUiMainSession50KG* OriginalUiFrontend50KG::mainSession() const noexcept {return storage_&&storage_->active?storage_->active->main.get():nullptr;}
const OriginalUiOptionsSession50KG* OriginalUiFrontend50KG::optionsSession() const noexcept {return storage_&&storage_->active?storage_->active->options.get():nullptr;}
const OriginalUiMainDrawResources50KG* OriginalUiFrontend50KG::mainResources() const noexcept {return storage_&&storage_->active?storage_->active->mainResources.get():nullptr;}
const OriginalUiOptionsDrawResources50KG* OriginalUiFrontend50KG::optionsResources() const noexcept {return storage_&&storage_->active?storage_->active->optionsResources.get():nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiFrontend50KG::textures() const noexcept {return storage_&&storage_->active?storage_->active->textures():nullptr;}
const MaterialTextureGl50EB* OriginalUiFrontend50KG::gpu() const noexcept {return storage_&&storage_->active?storage_->active->gpu.get():nullptr;}
const OriginalUiNativeDraw50KG* OriginalUiFrontend50KG::draw() const noexcept {
    const auto* s=storage_.get();return s&&!s->inOperation&&!s->faulted&&!s->quit&&!s->closing&&s->frameRendered&&s->active?s->active->draw():nullptr;
}
}

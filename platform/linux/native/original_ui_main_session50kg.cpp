#include "original_ui_main_session50kg.h"
#include <cmath>
#include <new>
#include <utility>

namespace rr::platform {
namespace {
using Status=OriginalUiMainSessionStatus50KG;
using WidgetStatus=game::OriginalUiMainWidgetStatus50KG;
using Phase=game::OriginalUiMainWidgetPhase50KG;
using Notice=game::OriginalUiTransitionNotificationKind50KG;
using Hook=game::OriginalUiApertureHookKind50KG;
using TransitionStatus=game::OriginalUiTransitionStatus50KG;
using ApertureStatus=game::OriginalUiApertureStatus50KG;
bool widgetOkay(WidgetStatus s) noexcept {
    return s==WidgetStatus::ok || s==WidgetStatus::noChange || s==WidgetStatus::hidden || s==WidgetStatus::lockedOut;
}
bool apertureOkay(ApertureStatus s) noexcept {return s==ApertureStatus::ok || s==ApertureStatus::noChange;}
Status result(OriginalUiMainSessionDiagnostic50KG& d,Status s,std::uint32_t pc=0) noexcept {
    d.status=s;d.sourcePC=pc;return s;
}
}
void resetOriginalUiMainProfileFields50KG(OriginalUiMainProfile50KG& p) noexcept {
    p.savedSlot6E0=1;
    p.availability11018=static_cast<std::uint8_t>((p.availability11018&0x80u)|4u);
    // Does not mark an otherwise unknown imported profile as wholly known.
}
struct OriginalUiMainSession50KG::Storage {
    OriginalUiMainSessionCold50KG cold;
    OriginalUiMainDraw50KG collector;
    game::OriginalUiTransitionOwner50KG transition;
    game::OriginalUiApertureOwner50KG aperture;
    game::OriginalUiMainSceneOwner50KG scene;
    game::OriginalUiMainFireOwner50KG fire;
    game::OriginalUiMainCameraPreparation50KG camera;
    game::OriginalUiMainParticleRenderState50KG particleRenderState;
    game::OriginalUiMainParticleLighting50KG lighting;
    OriginalUiMainSessionSnapshot50KG state;
    OriginalUiMainSessionDiagnostic50KG callbackFault;
    bool dependenciesPrepared=false,rendering=false,inOperation=false,paused=false;
    // Main1E5D10/1E6290's source-only transient debug-message state. The input
    // callbacks supported here cannot start the omitted debug editor.
    float messageTime744=0;
    bool messageActive74C=false;
    std::array<char,64> message704{};

    game::OriginalUiMainWidgetManager50KG& manager() noexcept {return *cold.resources->manager();}
    void fault(Status s,std::uint32_t pc) noexcept {
        if(!state.faulted){state.faulted=true;callbackFault.status=s;callbackFault.sourcePC=pc;}
    }
    bool operational() const noexcept {
        return dependenciesPrepared && !state.faulted && !state.quitRequested;
    }
    game::OriginalUiMainWidgetPhaseInput50KG phaseInput() const noexcept {
        return {cold.profile->known,cold.profile->savedSlot6E0,cold.profile->availability11018};
    }
    void widgetPhase(Phase p,std::uint32_t pc) noexcept {
        if(!widgetOkay(manager().applyMainWidgetPhase(p,phaseInput(),callbackFault.widget)))fault(Status::widget,pc);
    }
    bool audioEffect(std::uint32_t id) noexcept {
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return true;}
        if(!cold.audio || !cold.audio->ready() || !cold.audio->playMenuEffect(id)){
            ++state.audio.failed;fault(Status::audio,0x292360);return false;
        }
        ++state.audio.submitted;return true;
    }
    void startMusic() noexcept {
        // Main1E68A4..D0 only starts menu loop9 outside these original modes.
        if(cold.application->musicState44==4 || cold.application->musicState48==2){
            ++state.audio.suppressedBySourceGate;return;
        }
        if(cold.audioMode==OriginalUiMainAudioMode50KG::muted){++state.audio.suppressedByMutedMode;return;}
        if(!cold.audio || !cold.audio->ready() || !cold.audio->startMainLoop9()){
            ++state.audio.failed;fault(Status::audio,0x234900);return;
        }
        ++state.audio.submitted;
    }
    static bool apertureAvailable(void* p) noexcept {return static_cast<Storage*>(p)->operational();}
    static bool admit(void* p,const game::OriginalUiScreenFactory50KG& s,
        const game::OriginalUiPageFactory50KG& page,game::OriginalUiTransitionAdmission50KG kind) noexcept;
    static game::OriginalUiTransitionCompletion50KG poll(void* p,game::OriginalUiTransitionRoute50KG route,
        game::OriginalUiTransitionPhase50KG phase) noexcept {
        auto& self=*static_cast<Storage*>(p);
        if(!self.operational())return game::OriginalUiTransitionCompletion50KG::unavailable;
        return self.aperture.pollCompletion(route,phase);
    }
    static void notify(void* p,const game::OriginalUiTransitionNotification50KG& event) noexcept {
        auto& s=*static_cast<Storage*>(p);
        if(!s.operational())return;
        if(event.route.screen!=4 || event.route.page!=1){s.fault(Status::unsupportedState,event.sourcePC);return;}
        switch(event.kind) {
        case Notice::requestedPage90:
            s.widgetPhase(Phase::requestedPage90,event.sourcePC);s.state.mainPageShown=true;break;
        case Notice::previousPage94:
            s.widgetPhase(Phase::previousPage94,event.sourcePC);s.state.mainPageShown=false;break;
        case Notice::pageEntry74: {
            auto& profile=*s.cold.profile;
            if(profile.mode34!=4){profile.previousMode38=profile.mode34;profile.mode34=4;}
            s.state.promptMask42C=0;
            s.widgetPhase(Phase::entry74,event.sourcePC);
            s.message704.fill(0);s.messageTime744=0;s.messageActive74C=false;s.state.debugChord748=false;
            // Actual Main has no PageInfo movie/background-video path, so
            // optional video object+4BC is absent. The actual global UI field
            // is retained by the shared application subset, not guessed audio.
            s.cold.application->uiObject3D8=0;
            break;
        }
        case Notice::managerEntry84:
            // Exact Main vtable target2A4330 consists solely of return.
            break;
        case Notice::beginTransition7C:
            if(!apertureOkay(s.aperture.beginTransition7C(event.phase)))s.fault(Status::aperture,event.sourcePC);
            break;
        case Notice::pageAuxiliary1C:s.state.inputContainerShown=false;break;
        case Notice::pageAuxiliary24:s.state.inputContainerShown=true;break;
        }
    }
    static void apertureHook(void* p,const game::OriginalUiApertureHook50KG& event) noexcept {
        auto& s=*static_cast<Storage*>(p);
        if(!s.operational())return;
        switch(event.kind){
        case Hook::openingAC:s.widgetPhase(Phase::openingAC,event.callPC);if(!s.state.faulted)s.startMusic();break;
        case Hook::openedB0:s.state.promptMask42C=1;s.widgetPhase(Phase::openedB0,event.callPC);break;
        case Hook::closingB4:case Hook::closingThresholdB8:case Hook::closedBC:case Hook::screenExitObject440:
            // Entry-only aperture rejects exit BEFORE its first write/hook.
            s.fault(Status::unsupportedState,event.callPC);break;
        }
    }
    static void overlay(void* p,std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rectangle) noexcept {
        auto& s=*static_cast<Storage*>(p);
        if(!s.operational())return;
        if(!s.rendering || !s.collector.projectAperture(ordinal,rectangle,s.callbackFault.draw))s.fault(Status::draw,0x1D798C);
    }
    bool updateWidgets(const std::vector<std::uint16_t>& ids) noexcept {
        for(auto id:ids)if(manager().widgetState(id).kind==game::OriginalUiMainWidgetKind50KG::text)
            if(!widgetOkay(manager().updateText(id,true,callbackFault.widget))){fault(Status::widget,0x141ED0);return false;}
        return true;
    }
};
bool OriginalUiMainSession50KG::Storage::admit(void* p,const game::OriginalUiScreenFactory50KG& s,
    const game::OriginalUiPageFactory50KG& page,game::OriginalUiTransitionAdmission50KG kind) noexcept {
    auto& self=*static_cast<Storage*>(p);
    // Exact factory identities prevent a route-number-only readiness claim.
    return self.operational() && kind==game::OriginalUiTransitionAdmission50KG::initialPage &&
        &s==game::findOriginalUiScreenFactory50KG(4) && &page==game::findOriginalUiBoundedPageFactory50KG(4,1);
}
OriginalUiMainSession50KG::OriginalUiMainSession50KG() noexcept=default;
OriginalUiMainSession50KG::~OriginalUiMainSession50KG()=default;
Status OriginalUiMainSession50KG::bind(const OriginalUiMainSessionCold50KG& cold,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};if(storage_)return result(d,Status::alreadyBound);
    if(!cold.resources || !cold.resources->loaded() || !cold.resources->manager() || !cold.gpu ||
        !cold.mainArchive || !cold.mainArchive->loaded() || !cold.mainArchive->identity() ||
        cold.mainArchive->identity()->id!=assets::OriginalUiArchiveId50KG::mainMenu)
        return result(d,Status::resource);
    if(!cold.profile || !cold.profile->known || cold.profile->savedSlot6E0<1 || cold.profile->savedSlot6E0>5 ||
        !cold.application || !cold.application->known || (cold.application->globalFlags228&4u) ||
        cold.application->transportE00!=-1)return result(d,Status::unsupportedState);
    if(cold.audioMode!=OriginalUiMainAudioMode50KG::enabled && cold.audioMode!=OriginalUiMainAudioMode50KG::muted)
        return result(d,Status::invalidInput);
    if(cold.audioMode==OriginalUiMainAudioMode50KG::enabled && (!cold.audio || !cold.audio->ready()))return result(d,Status::audio);
    // Requiring nullptr for deliberate mute prevents accidental second writer
    // ownership and makes suppressed audio receipts unambiguous.
    if(cold.audioMode==OriginalUiMainAudioMode50KG::muted && cold.audio)return result(d,Status::invalidInput);
    try {
        auto next=std::make_unique<Storage>();next->cold=cold;next->state.audioMode=cold.audioMode;
        if(!next->collector.bind(cold.resources,cold.gpu,d.draw))return result(d,Status::draw);
        assets::OriginalUiMainCameraConfig50KG config;assets::OriginalUiPageDiagnostic50KG parse;
        if(!assets::readOriginalUiMainCameraConfig50KG(*cold.resources->manager()->uiDat(),config,parse) ||
            game::prepareOriginalUiMainCamera50KG(config,next->camera)!=game::OriginalUiMainCameraStatus50KG::ok)
            return result(d,Status::camera);
        if(!next->scene.prepare(*cold.mainArchive,d.scene))return result(d,Status::scene);
        if(next->fire.initialize(next->scene)!=game::OriginalUiMainFireStatus50KG::complete)return result(d,Status::particle);
        if(game::prepareOriginalUiMainParticleLighting50KG(next->scene,next->lighting)!=game::OriginalUiMainParticleLightingStatus50KG::complete)
            return result(d,Status::scene);
        if(game::prepareOriginalUiMainParticleRenderState50KG(next->scene,next->camera,next->lighting,next->particleRenderState)!=
            game::OriginalUiMainParticleRenderStateStatus50KG::complete)return result(d,Status::camera);
        const auto* page=cold.resources->manager()->page(1);
        if(!page)return result(d,Status::resource);
        if(page->useVideoAsTexture || page->background!="*INVALID*")
            return result(d,Status::unsupportedState,0x1D63B0);
        std::uint32_t packed=0;
        for(std::size_t i=0;i<3;++i){std::int32_t channel=0;
            if(!game::convertOriginalUiApertureCvtWS50KG(page->backgroundColor[i],channel) || channel<0 || channel>255)
                return result(d,Status::unsupportedState,0x1D6D24);
            packed|=static_cast<std::uint32_t>(channel)<<(16u-8u*static_cast<unsigned>(i));
        }
        next->state.packedBackground430=packed;
        next->dependenciesPrepared=true;
        game::OriginalUiApertureCallbacks50KG a;
        a.context=next.get();a.available=&Storage::apertureAvailable;a.hook=&Storage::apertureHook;a.submitOverlay=&Storage::overlay;
        if(next->aperture.bindMainEntryOnly(cold.mainArchive,a,d.aperture)!=ApertureStatus::ok)return result(d,Status::aperture);
        game::OriginalUiTransitionCallbacks50KG t;
        t.context=next.get();t.admitDestination=&Storage::admit;t.notify=&Storage::notify;t.pollCompletion=&Storage::poll;
        // Fixed-address private callback storage survives publication. No
        // fake queue, launch, selection commit or destination hook is supplied.
        const auto bound=next->transition.bindInitialMain(t);
        if(bound!=TransitionStatus::ok)return result(d,Status::transition);
        if(next->state.faulted){
            // Initial source callbacks may already have changed shared widget
            // or profile state. Retain the faulted owner, forbid retry, and
            // require fresh resources instead of implying atomic rollback.
            d=next->callbackFault;storage_=std::move(next);return d.status;
        }
        next->state.bound=true;storage_=std::move(next);return result(d,Status::ok);
    }catch(const std::bad_alloc&){return result(d,Status::allocation);}catch(...){return result(d,Status::resource);}
}
Status OriginalUiMainSession50KG::input(const OriginalUiMainSessionInput50KG& in,
    OriginalUiMainSessionInputReceipt50KG& receipt,OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};receipt={};auto* s=storage_.get();
    if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);
    if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation || in.controller>1 || in.originalInput>15)return result(d,Status::invalidInput);
    receipt.containerReached=s->state.mainPageShown && s->state.inputContainerShown;
    if(!receipt.containerReached || !s->aperture.snapshot().inputEnabled268)return result(d,Status::noChange);
    if(s->cold.application->transportE00!=-1)return result(d,Status::unsupportedState,0x267BE0);
    s->inOperation=true;
    receipt.pageCallbackRan=true;receipt.pageAccepted=true; //1E6284 unconditional1
    game::OriginalUiMainContext50KG context;
    context.popupActive=in.popupActive;context.sourceInputBlocked=in.sourceInputBlocked || s->state.debugChord748;
    context.secondControllerPresent=in.secondControllerPresent;
    // No destination owner is available yet; original labels stay selectable.
    Status status=Status::noChange;
    if(in.originalInput==2 || in.originalInput==3 || in.originalInput==4){
        const auto ws=s->manager().dispatchMain(in.controller,static_cast<game::OriginalUiMainInput50KG>(in.originalInput),
            context,receipt.decision,d.widget);
        if(ws==WidgetStatus::unavailable){
            status=receipt.decision.action==game::OriginalUiMainAction50KG::secondControllerRequired?
                Status::secondControllerRequired:Status::unavailableDestination;
        }else if(!widgetOkay(ws)){s->fault(Status::widget,0x1E5E00);status=Status::widget;}
        else if(receipt.decision.action==game::OriginalUiMainAction50KG::changedSelection){
            status=s->audioEffect(in.originalInput==2?3:4)?Status::ok:Status::audio;
            // Source267BE0 is reached here; admitted transportE00=-1 takes
            // the exact no-work return. Never rename it an idle reset.
        }
    }
    // Snapshot-selected delivery occurs AFTER the actual page callback, even
    // if its own popup/controller/sourceInputBlocked gates suppressed action.
    game::OriginalUiMainMovementDelivery50KG delivery{true,true,true};
    const auto movement=s->manager().dispatchMainMovement(in.controller,
        static_cast<assets::OriginalUiMovementInput50KG>(in.originalInput),delivery,receipt.movement,d.widget);
    if(!widgetOkay(movement)){s->fault(Status::widget,0x2A73F0);status=Status::widget;}
    s->inOperation=false;receipt.status=status;return result(d,status);
}
Status OriginalUiMainSession50KG::update(const OriginalUiMainSessionUpdate50KG& in,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    if(s->state.faulted)return result(d,Status::faulted);
    if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation || !std::isfinite(in.elapsedSeconds) || in.elapsedSeconds<0 || in.elapsedSeconds>1 ||
        (in.paused && in.elapsedSeconds!=0))
        return result(d,Status::invalidInput);
    if((s->cold.application->globalFlags228&4u) || s->cold.application->transportE00!=-1)
        return result(d,Status::unsupportedState);
    s->inOperation=true;s->paused=in.paused;
    if(s->cold.audioMode==OriginalUiMainAudioMode50KG::enabled){
        s->cold.audio->pause(in.paused);
        if(!s->cold.audio->update(in.elapsedSeconds)){s->fault(Status::audio,0x234440);}
    }
    // Manager switch runs before child traversal. Each call advances only one
    // switch case; opening geometry itself advances only from render48.
    const auto transitioned=s->transition.tick();
    if(transitioned!=TransitionStatus::ok && transitioned!=TransitionStatus::noChange &&
        transitioned!=TransitionStatus::pending)s->fault(Status::transition,0x2A3C80);
    const auto* order=s->manager().managerWidgetOrder();
    if(!s->state.faulted)s->updateWidgets(order->ordinaryWidgets);
    // Actual Main entity1E6290 precedes its page-owned input container.
    if(!s->state.faulted && s->state.mainPageShown){
        s->state.debugChord748=in.debugChordHeld[0] && in.debugChordHeld[1] &&
            in.debugChordHeld[2] && in.debugChordHeld[3] && (s->cold.profile->flags11054&1u);
        if(s->messageActive74C)s->messageTime744-=in.elapsedSeconds;
        if(s->messageTime744<=0){s->message704.fill(0);s->messageActive74C=false;s->messageTime744=-1;}
        if(s->aperture.saveMainUpdateDelta38(in.elapsedSeconds)!=ApertureStatus::ok)s->fault(Status::aperture,0x1E63F8);
    }
    if(!s->state.faulted)s->updateWidgets(order->onTopWidgets);
    // 1CF410 appends these actual Main scene subscribers AFTER Pod children:
    // particle manager(-1), borrowed stationary camera(-1), emitter(0x402).
    if(!s->state.faulted){
        s->state.lastParticleAdvance=s->fire.advance(in.elapsedSeconds,in.paused);
        if(!s->state.lastParticleAdvance.okay())s->fault(Status::particle,0x160E40);
    }
    // This bounded camera has no source pose animation or debug override.
    // Pose is invariant. Source render fog changes its clipping planes; the
    // typed particleRenderState is that SAME camera's later render boundary,
    // not another entity. Unconsumed motion metrics are not claimed recovered.
    if(!s->state.faulted){
        s->state.lastFireEmit=s->fire.emit(in.elapsedSeconds,in.paused,
            s->cold.application->rawRandom308030,s->cold.application->fireSerial433970);
        if(!s->state.lastFireEmit.okay())s->fault(Status::particle,0x15AEA0);
    }
    s->inOperation=false;
    if(s->state.faulted){d=s->callbackFault;return d.status;}
    ++s->state.updateCount;return result(d,Status::ok);
}
Status OriginalUiMainSession50KG::render(const OriginalUiMainFrameClock50KG& clock,
    OriginalUiMainSessionDiagnostic50KG& d) noexcept {
    d={};auto* s=storage_.get();if(!s)return result(d,Status::notBound);
    s->state.framePublished=false;
    if(s->state.faulted)return result(d,Status::faulted);
    if(s->state.quitRequested)return result(d,Status::quit);
    if(s->inOperation || !s->state.updateCount || (s->paused && clock.advanceAnimations))return result(d,Status::invalidInput);
    // Source scene fog/proxy follows manager UI fog and sets camera far4000.
    // The cold typed boundary preserves that exact render-time matrix/color;
    // it does not replace the manager's immutable authored pose or page camera.
    s->state.framePublished=false;s->inOperation=true;s->rendering=true;
    bool okay=s->collector.begin(clock,d.draw) && s->collector.projectOrdinaryWidgets(true,d.draw) &&
        s->collector.projectMainPage(s->state.mainPageShown,d.draw);
    if(okay)okay=apertureOkay(s->aperture.render48(s->transition.snapshot().lifecycle));
    okay=okay && !s->state.faulted;
    if(okay)okay=s->collector.projectMainBasePrompts(s->state.promptMask42C,d.draw) &&
        s->collector.projectOnTopWidgets(true,d.draw) &&
        s->collector.projectMainParticles(s->fire,s->scene,s->particleRenderState,d.draw) && s->collector.drain(d.draw);
    s->rendering=false;s->inOperation=false;
    if(!okay){if(s->state.faulted){d=s->callbackFault;return d.status;}return result(d,Status::draw);}
    ++s->state.renderCount;s->state.framePublished=true;return result(d,Status::ok);
}
void OriginalUiMainSession50KG::requestQuit() noexcept {
    if(!storage_ || storage_->inOperation)return;
    storage_->state.quitRequested=true;storage_->state.framePublished=false;
    if(storage_->cold.audioMode==OriginalUiMainAudioMode50KG::enabled && storage_->cold.audio)storage_->cold.audio->pause(true);
}
OriginalUiMainSessionSnapshot50KG OriginalUiMainSession50KG::snapshot() const noexcept {
    if(!storage_)return {};
    auto snapshot=storage_->state;snapshot.transition=storage_->transition.snapshot();snapshot.aperture=storage_->aperture.snapshot();return snapshot;
}
const OriginalUiNativeDraw50KG* OriginalUiMainSession50KG::draw() const noexcept {return storage_ && !storage_->state.faulted && !storage_->state.quitRequested && storage_->state.framePublished?storage_->collector.draw():nullptr;}
const game::OriginalUiMainCameraPreparation50KG* OriginalUiMainSession50KG::camera() const noexcept {return storage_?&storage_->camera:nullptr;}
const game::OriginalUiMainParticleCamera50KG* OriginalUiMainSession50KG::particleCamera() const noexcept {return storage_?&storage_->particleRenderState.particleCamera:nullptr;}
const game::OriginalUiMainParticleRenderState50KG* OriginalUiMainSession50KG::particleRenderState() const noexcept {return storage_?&storage_->particleRenderState:nullptr;}
const game::OriginalUiMainSceneOwner50KG* OriginalUiMainSession50KG::scene() const noexcept {return storage_?&storage_->scene:nullptr;}
const game::OriginalUiMainFireOwner50KG* OriginalUiMainSession50KG::fire() const noexcept {return storage_?&storage_->fire:nullptr;}
const game::OriginalUiMainParticleLighting50KG* OriginalUiMainSession50KG::lighting() const noexcept {return storage_?&storage_->lighting:nullptr;}
}

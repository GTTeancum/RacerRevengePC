#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "original_race_display_lifetime50la.h"
#include "original_race_hud_scope50la.h"
#include "original_position_stats_adapter50la.h"
#include "ra_cold_aggregate50la.h"
#include "../../src/game/original_pause_storage50la.h"
#include "../../platform/linux/native/original_video_blocking_native.h"
#include <limits>

namespace original_main_entry50KG {
struct NativeResultsInputContinuation50LA {
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race;
    std::shared_ptr<original_camera_session50KX::Session> camera;
    std::shared_ptr<Video> video;
    std::shared_ptr<RaceDisplayLifetime50LA> display;
    std::shared_ptr<original_race_hud_scope50LA::Registration> scope;
    rr::game::OriginalResults10450LA* results=nullptr;
    rr::platform::OriginalVideoBlockingNativeBackend* backend=nullptr;
    original_camera_session50KX::DisplayInputs* inputs=nullptr;
    rr::platform::OriginalUiRiderProfileStamp50KG profile{};
    rr::game::OriginalResultsBlockingFade2AED60 fade;
    std::chrono::steady_clock::time_point epoch;
    std::uint64_t root=0,generation=0,frame=0,publication=0;
    std::uint32_t sourceCallPC=0;
    bool failed=false;
};
#include "original_results_hall_transfer50la.inl"
bool Application::resultsDonePending50LA()const noexcept {
    return garageAudioOwnerThread_==std::this_thread::get_id()&&bool(resultsInputContinuation50LA_);
}
bool Application::resultsInputContinuationCurrent50LA()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto& p=resultsInputContinuation50LA_;
    if(!p||p->failed||sourceRace28COwner_!=p->race||videoOwner_!=p->video||
       raceDisplayOwner50LA_.lock()!=p->display||!p->display->canRetain()||
       p->display->application_!=this||p->display->video_!=p->video||
       nativeTaRootEntered_||nativeTaRootAborted_||nativeTaRootRace_!=p->race.get()||
       nativeTaRootPublication_!=p->root||nativeTaRootGeneration_!=p->generation||
       sourceUpdateRenderBypass228Bit3_||sourceDebugCamera228Bit2_||sourceApplicationMode22C_!=0||
       p->race->results104_.get()!=p->results||!p->camera->ready()||p->camera->abandoned_||
       !p->scope||p->race->endHud_.lock()!=p->scope||!p->scope->endStateCurrent(*p->race,*p->camera)||
       p->camera->generation_!=p->generation||p->camera->completedFrame_!=p->frame||
       p->camera->publicationCount_!=p->publication||p->camera->actual_.applicationClock.epoch!=p->epoch||
       !p->race->profile_||p->race->profile_->stamp().generation!=p->profile.generation||
       p->race->profile_->stamp().mode34!=p->profile.mode34||
       !p->camera->displayInputs_||p->camera->displayInputs_!=p->inputs||
       &p->camera->displayInputs_->renderer!=&p->display->renderer_||
       !p->video->host.retainedResultsPollCurrent50LA(*p->results))return false;
    rr::game::OriginalResults104Diagnostic50LA d;
    // Deactivation does not detach the actual same sibling. Only THIS
    // retained transaction may validate custody without fresh active admission.
    return resultsPublishedRoot50LA(*p->race,d,false);
}
bool Application::suspendResultsBlockingInput50LA(rr::game::OriginalResults10450LA& results,
    rr::game::OriginalRaceConstructor50LA& race,const original_camera_session50KX::Session& observed,
    rr::platform::GameHost50ER& host,ResultsBlockingInputOrigin50LA origin)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||resultsInputContinuation50LA_||
       (origin!=ResultsBlockingInputOrigin50LA::summary&&origin!=ResultsBlockingInputOrigin50LA::tournament&&
        origin!=ResultsBlockingInputOrigin50LA::nameDone&&origin!=ResultsBlockingInputOrigin50LA::hall)||
       sourceRace28COwner_.get()!=&race||race.results104_.get()!=&results||!race.world74_||
       !videoOwner_||&videoOwner_->host!=&host||nativeTaRootEntered_||nativeTaRootAborted_||
       !nativeTaRootPublication_||nativeTaRootRace_!=&race)return false;
    try {
        const auto display=raceDisplayOwner50LA_.lock();
        const auto scope=race.endHud_.lock();
        ra_cold_aggregate50LA::Diagnostic wd;
        auto camera=race.world74_->borrowRuntimeSourceCamera(race.world74_,wd);
        rr::game::OriginalResults104Diagnostic50LA d;
        if(!display||!display->canRetain()||display->application_!=this||display->video_!=videoOwner_||
           !camera||camera.get()!=&observed||!camera->displayInputs_||camera->abandoned_||
           &camera->displayInputs_->renderer!=&display->renderer_||
           !scope||!scope->endStateCurrent(race,*camera)||
           camera->pendingDisplayWork()||camera->blockingVideoPending()||camera->asyncVideoPending()||
           !resultsPublishedRoot50LA(race,d))return false;
        auto p=std::make_shared<NativeResultsInputContinuation50LA>();
        p->race=sourceRace28COwner_;p->video=videoOwner_;p->display=display;p->scope=scope;p->camera=std::move(camera);
        p->results=&results;p->root=nativeTaRootPublication_;p->generation=nativeTaRootGeneration_;
        p->frame=p->camera->completedFrame_;p->publication=p->camera->publicationCount_;
        p->epoch=p->camera->actual_.applicationClock.epoch;
        p->inputs=p->camera->displayInputs_;p->profile=race.profile_->stamp();
        p->sourceCallPC=static_cast<std::uint32_t>(origin);
        // Last fallible custody transfer, after allocating all owning pins.
        if(!host.retainResultsPoll50LA(results))return false;
        resultsInputContinuation50LA_=std::move(p);return true;
    }catch(...){return false;}
}
std::uint32_t Application::resultsBlockingInputReturnPC50LA(const rr::game::OriginalResults10450LA& owner)const noexcept {
    return resultsInputContinuationCurrent50LA()&&resultsInputContinuation50LA_->results==&owner&&
        resultsInputContinuation50LA_->fade.complete()?resultsInputContinuation50LA_->sourceCallPC:0u;
}
bool Application::advanceResultsDone50LA(rr::platform::OriginalVideoBlockingNativeBackend& backend,
    rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {
    using S=rr::game::OriginalResults104Diagnostic50LA::Status;
    d={};d.status=S::owner;d.sourcePC=0x2aed60;d.operation="Results retained fade owner changed";
    if(!resultsInputContinuationCurrent50LA())return false;
    const auto p=resultsInputContinuation50LA_;
    d.sourcePC=p->sourceCallPC;
    if(&backend.renderer_!=&p->display->renderer_||&backend.host_!=&p->video->host||
       backend.use_!=rr::platform::OriginalVideoNativeUse::blockingSourceLoops)return false;
    if(p->backend&&p->backend!=&backend)return false;
    if(!p->backend){
        const auto phase=backend.phase();
        if(phase!=rr::platform::OriginalVideoNativePhase::idle&&phase!=rr::platform::OriginalVideoNativePhase::retired)
            return false;
        p->backend=&backend;
    }
    try {
        auto& display=*p->camera->displayInputs_;
        const auto r=p->fade.advance(display.fadeManagerA8,display.currentApplicationMode22C(),
            display.video,p->camera->actual_.applicationClock,&backend);
        if(!resultsInputContinuationCurrent50LA()){p->failed=true;return false;}
        if(r.status==rr::game::OriginalCameraStartStatus::videoClockWait){
            d.status=S::incomplete;d.sourcePC=r.sourcePC;d.operation="Results real blocking presentation/clock pending";
            return true;
        }
        if(r.status!=rr::game::OriginalCameraStartStatus::complete){p->failed=true;return false;}
        const auto scope=p->race->endHud_.lock();
        if(!scope||!scope->pressResults1C8C30(*p->race,p->video->host,d)){
            p->failed=true;return false;
        }
        if(p->sourceCallPC==0x1ca678u){
            const auto hall=resultsHallTransfer50LA_;
            if(!hall||hall->phase!=NativeResultsHallTransfer50LA::Phase::queued||hall->race!=p->race){
                p->failed=true;return false;
            }
        }
        if(!resultsInputContinuationCurrent50LA()||!p->video->host.releaseResultsPoll50LA(*p->results)){
            p->failed=true;return false;
        }
        if(p->sourceCallPC==0x1ca678u)
            resultsHallTransfer50LA_->phase=NativeResultsHallTransfer50LA::Phase::pollReturned;
        resultsInputContinuation50LA_.reset();return true;
    }catch(...){p->failed=true;return false;}
}
bool Application::cancelResultsDone50LA(rr::platform::OriginalVideoBlockingNativeBackend* backend)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=resultsInputContinuation50LA_;if(!p)return true;
    // Called AFTER the real backend cancel/retire, before Camera/World release.
    if(!backend||(p->backend&&p->backend!=backend))return false;
    const auto phase=backend->phase();
    if(phase!=rr::platform::OriginalVideoNativePhase::cancelled&&phase!=rr::platform::OriginalVideoNativePhase::retired&&
       phase!=rr::platform::OriginalVideoNativePhase::idle)
        return false;
    if(!p->video->host.releaseResultsPoll50LA(*p->results))return false;
    resultsInputContinuation50LA_.reset();return true;
}
bool Application::pressConsumedResults1C8C30(rr::platform::GameHost50ER& polledHost,
    rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {
    d={};const auto reject=[&](std::uint32_t pc,const char* why)noexcept {
        d.status=rr::game::OriginalResults104Diagnostic50LA::Status::owner;
        d.sourcePC=pc;d.operation=why;return false;
    };
    //Immutable Application fence BEFORE its mutable Race/Video/Display slots.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject(0x1c8c30,"Results input requires actual Application construction thread");
    const auto race=sourceRace28COwner_;const auto video=videoOwner_;
    const auto display=raceDisplayOwner50LA_.lock();
    if(!race||race->resultsFeedbackThread_!=std::this_thread::get_id()||!video||!display||
       display->application_!=this||display->video_!=video||!display->canRetain()||
       race->loadDisplay_!=display||&video->host!=&polledHost||nativeTaRootRace_!=race.get()||nativeTaRootEntered_||
       !nativeTaRootPublication_||nativeTaRootAborted_||sourceUpdateRenderBypass228Bit3_||sourceDebugCamera228Bit2_||sourceApplicationMode22C_!=0)
        return reject(0x1c8c30,"Results input requires same completed idle Race/Video/Display root");
    if(!resultsPublishedRoot50LA(*race,d))return false;
    //140C90 invokes128080 BEFORE140CE0 writes mode2/23C. Input must not
    //pretend to be an active update. This native route requires a prior real
    //completed Root only for custody; it never opens a Root or changes time.
    // Effects-free supported-domain check includes the actual earlier process
    // owners. Their reached concrete callbacks are evaluated again PER event.
    if(!passResultsProcessPredecessors50LA(*race,nullptr,d))return false;
    //Do not steal an active Pause/unknown earlier child's native controls.
    //This is an effects-free concrete-owner domain check, not source callback
    //execution; the real1A87A0 reads and mandatory tail remain per-event.
    if(!race->resultsInputPredecessorTopology50LA(d))return false;
    //The same normal category2 bridge is17CBC0 ->2376F0 ->140EF0.
    //No earlier root sibling is silently skipped, and no child ownMask test
    //is added to142350. Concrete Race/Pause work is interleaved PER event.
    auto& host=video->host;
    if(!host.ownerThread()||!host.created()||!host.hasWindow()||
       !host.resultsInput50LA_.fixedUiCurrent()||!host.resultsInput50LA_.edgesOpen_||
       !host.resultsPollDestination50LA_)
        return reject(0x1c8c30,"Results fresh successful Window poll already retired or absent");
    const auto actual= race->endHud_.lock();
    if(!actual)return reject(0x1c8c30,"Results actual lexical Camera/Bank/audio lifetime absent");
    //The actual Loop invokes once here, before native menus/driving. The
    //same poll's claimed UI controls are then removed from its exact output
    //destination; failure before claim leaves other clients' edges untouched.
    return actual->pressResults1C8C30(*race,host,d);
}
}

namespace original_race_hud_scope50LA {
bool Registration::pressResults1C8C30(rr::game::OriginalRaceConstructor50LA& race,
    rr::platform::GameHost50ER& host,rr::game::OriginalResults104Diagnostic50LA& d)noexcept {
    d={};d.status=rr::game::OriginalResults104Diagnostic50LA::Status::owner;d.sourcePC=0x1c8c30;
    d.operation="Results same-Race synchronous Camera/Bank/audio borrow unavailable";
    if(std::this_thread::get_id()!=thread_)return false;
    if(!active_||!published_||!hud_||!manager_||!bank_||!camera_)return false;
    const auto retained=race_.lock();
    if(retained.get()!=&race||application_->race28C()!=&race||
       application_->entities()!=race.application_||!race.counted_||race.rolledBack_||
       race.entity_.sessionGeneration!=generation_||race.endHud_.lock().get()!=this||
       !race.world74_||!race.results104_)return false;
    try {
        const auto world=race.world74_;ra_cold_aggregate50LA::Diagnostic wd;
        const auto camera=world->borrowRuntimeSourceCamera(world,wd);
        if(!camera||camera.get()!=camera_||&camera->actual_.bank!=bank_||
           camera->actual_.raceHud!=hud_||camera->generation_!=generation_||
           camera->actual_.applicationEntities!=race.application_)return false;
        original_position_stats_adapter50LA::Owner observer;
        original_position_stats_adapter50LA::Owner::Diagnostic observed;
        if(!observer.bind(*camera,*manager_,observed))return false;
        //Same lifetime pin and source-loaded Operation/Bank checks as the end
        //prefix. No owning Race cycle, copied rows or manufactured audio owner.
        return race.results104_->pressHostInput1C8C30(*application_,race,observer,camera->actual_.cycleAudio,host,d);
    }catch(...){return false;}
}
}

#include "original_camera_session50kx.h"
#include "original_race_world_join50la.h"
#include "original_race_display_lifetime50la.h"
#include "ra_cold_aggregate50la.h"
#include <thread>
#include <utility>
#include <cfenv>

namespace rr::game {
bool OriginalRaceConstructor50LA::nativeCameraAdapter34LifetimeCurrent(
    const original_camera_session50KX::Session& session)const noexcept {
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!counted_||rolledBack_)return false;
    if(!world74_&&taWorld74_){
        // TA: the unique Application-issued course Session of THIS race.
        const auto& issued=session.nativeTaIssuance_;
        if(!issued||issued->course!=OriginalCameraStartCourse::ta||!issued->issuer||
           issued->issuer->race28C()!=this||issued->generation!=entity_.sessionGeneration)return false;
        return issued->issuer->issuedCourseCameraSession50LA().get()==&session;
    }
    if(!world74_||!world74_->runtimeQueries_)return false;
    const auto& runtime=*world74_->runtimeQueries_;
    // Teardown compares actual unique ownership even after prepare has become
    // terminal. It must not require the live admission it is trying to revoke.
    return runtime.camera_&&runtime.camera_->session.get()==&session&&
        runtime.camera_->generation==entity_.sessionGeneration;
}
bool OriginalRaceConstructor50LA::ownsNativeCameraAdapter34(
    const original_camera_session50KX::Session& session)const noexcept {
    if(!nativeCameraAdapter34LifetimeCurrent(session))return false;
    if(!world74_)return taWorldCounted_&&taWorldPrefixComplete_&&!session.closed_&&!session.abandoned_;
    const auto& runtime=*world74_->runtimeQueries_;
    // The actual RA world owns this unique Session; an equal generation or an
    // Application-issued TA Session is not interchangeable with that owner.
    return !world74_->terminal_&&!world74_->closed_&&!runtime.terminal_&&!runtime.closed_&&
        !runtime.cameraPrepareTerminal_&&runtime.cameraBankCurrent()&&runtime.camera_->display;
}
bool OriginalRaceConstructor50LA::raceCameraCourse34(OriginalCameraStartCourse course)const noexcept {
    if(world74_)return course==OriginalCameraStartCourse::ra;
    if(taWorld74_)return course==OriginalCameraStartCourse::ta;
    return false;
}
std::shared_ptr<original_camera_session50KX::Session> OriginalRaceConstructor50LA::borrowRaceCamera34()const noexcept {
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!counted_||rolledBack_)return {};
    if(world74_){
        const auto world=world74_;ra_cold_aggregate50LA::Diagnostic wd;
        return world->borrowRuntimeSourceCamera(world,wd);
    }
    if(!taWorld74_||!cameraNativeAdapter34_)return {};
    const auto& issued=cameraNativeAdapter34_->nativeTaIssuance_;
    if(!issued||!issued->issuer)return {};
    auto held=issued->issuer->issuedCourseCameraSession50LA();
    if(held.get()!=cameraNativeAdapter34_||!ownsNativeCameraAdapter34(*held))return {};
    return held;
}
}

namespace original_main_entry50KG {
bool Application::continueConsumedRaCamera1A9240(
    rr::game::OriginalRaceConstructor50LA& race,std::uint32_t& pc)const noexcept {
    using Step=rr::game::OriginalRaceConstructor50LA::CameraInitStep;
    using Camera=rr::game::OriginalFrontendCameraCold50LA;
    pc=0x1a6c88;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       race.resultsFeedbackThread_!=std::this_thread::get_id())return false;
    if(sourceRace28COwner_.get()!=&race||raceConstructorColdOwner50LA_.lock().get()!=&race||
       race.application_!=entitiesOwner_||race.profile_!=riderProfile||!riderProfile||
       !race.counted_||race.rolledBack_||!race.nativeIntroWorldEntity()||!race.anyWorld()||
       race.worldInitStep_!=rr::game::OriginalRaceConstructor50LA::WorldInitStep::camera||
       !entitiesOwner_||entitiesOwner_->attachmentLock423F58()||
       (race.flags1B0KnownMask_&3u)!=3u)return false;
    // Exact1A6C8C a1. Only the actual one-view branch has a native backend;
    // zero/debug and multi-view branches do not borrow Session's one-view value.
    if((race.flags1B0KnownValue_&3u)!=1u){pc=0x1a9240;return false;}
    if(race.cameraInitStep_==Step::nativeRetiring||race.cameraInitStep_==Step::retired)return false;
    if(race.cameraInitStep_==Step::unreached){
        race.field8C_=0; //1A9284, before the reached Application2E0 read.
        race.cameraInitStep_=Step::pause;
    }
    if(race.cameraInitStep_==Step::pause){
        pc=0x1a9290;
        if(!sourceUseProfile2E0_)return false;
        if(*sourceUseProfile2E0_){
            if(!animationClockOwner_||!animationClockOwner_->applicationAlive_||
               animationClockOwner_->updateInProgress_)return false;
            animationClockOwner_->value_.pause11C=0; //1A92A0 branch delay store.
        }
        race.cameraInitStep_=Step::construct;
    }
    if(race.cameraInitStep_==Step::construct){
        pc=0x1a9390;
        if(race.camera34_)return false;
        std::shared_ptr<Camera> constructed;
        // Existing actual counted236180(a1=1), same Application Entity registry.
        // Native allocation rejection does not pretend to call2361C0(nullptr).
        const auto made=Camera::construct(this,entitiesOwner_,constructed);
        if(!made.okay()){pc=made.sourcePC;return false;}
        race.camera34_=std::move(constructed); //1A93AC BEFORE initializer entry.
        race.cameraInitStep_=Step::viewportPrefix;
    }
    const auto camera=race.camera34_;
    if(!camera||camera->issuer_!=this||camera->entities_!=entitiesOwner_||
       !camera->onAffinity()||!camera->actualEntitiesAlive())return false;
    if(race.cameraInitStep_==Step::viewportPrefix){
        pc=0x1a93b0;
        // Existing exact640,448,3FAAAAAB scalar stores through1149D4. A later
        // failure retains the already published Camera34/count and every store.
        const auto prefix=camera->stageViewportPrefix1149D4(this,entitiesOwner_);
        if(!prefix.okay()){pc=prefix.sourcePC;return false;}
        race.cameraInitStep_=Step::nativePending;
    }
    if(camera->phase_!=Camera::Phase::beforeViewport114A00)return false;
    // Native cold staging may now construct the actual retained world Session.
    // This return is only this bounded prefix, NOT2361C0 or1A9240 completion.
    pc=race.cameraInitStep_==Step::nativeReady?0x1a6c90u:0x114a00u;
    return race.cameraInitStep_==Step::nativePending||race.cameraInitStep_==Step::nativeReady;
}

bool Application::consumedRaAll8CameraCustodyNative34Current(
    const original_camera_session50KX::Session& session)const noexcept {
    using Step=rr::game::OriginalRaceConstructor50LA::CameraInitStep;
    // The ordinary ALL8 factory supplies no Career/TA issuance. Derive custody
    // from its actual world-owned unique Session, never from a matching key or
    // a synthetic completion token. This function performs no source writes.
    // Music10C is a separate owner: its presence or lexical provider revocation
    // must not revoke camera custody or strand an active Pause during cleanup.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!session.onOwnerThread())return false;
    try {
        const auto& race=sourceRace28COwner_;
        if(!race||raceConstructorColdOwner50LA_.lock()!=race||race->application_!=entitiesOwner_||
           !riderProfile||race->profile_!=riderProfile||!race->nativeIntroWorldEntity()||
           !race->ownsNativeCameraAdapter34(session)||
           race->worldInitStep_!=rr::game::OriginalRaceConstructor50LA::WorldInitStep::camera||
           race->cameraInitStep_!=Step::nativePending||race->cameraNativeAdapter34_||
           (race->flags1B0KnownMask_&3u)!=3u||(race->flags1B0KnownValue_&3u)!=1u||race->field8C_||
           session.nativeTaIssuance_||session.generation_!=race->entity_.sessionGeneration||
           session.actual_.applicationEntities!=entitiesOwner_||
           !session.actual_.applicationAnimation.sameOwner(animationClock())||
           session.actual_.humanCount34||session.actual_.bank.count()!=8u||
           session.closed_||session.abandoned_||!session.coldBound_||!session.readsBound_||
           session.frameStage_==original_camera_session50KX::Session::FrameStage::failed||
           !session.displayInputs_||!session.identitiesCurrent())return false;
        // Preserve the actual legacy ALL8 countdown borrow. It is not the
        // source constructor's Race94, and neither value is copied or changed.
        // A future canonical timing join must explicitly revise this boundary.
        if(&session.actual_.race94==&race->field94_)return false;
        const auto& camera=race->camera34_;
        if(!camera||camera->issuer_!=this||camera->entities_!=entitiesOwner_||
           camera->phase_!=rr::game::OriginalFrontendCameraCold50LA::Phase::beforeViewport114A00||
           !camera->coldRetirementPreflight(true).okay())return false;
        const auto display=raceDisplayOwner50LA_.lock();
        if(!display||display!=race->loadDisplay_||display->application_!=this||
           !display->gameplayDisplayIdentityCurrent50LA()||!display->video_->host.current()||
           &session.displayInputs_->renderer!=&display->renderer_||
           &session.displayInputs_->rendererDispatch!=&display->dispatch_)return false;
        // Observe the real completed native selection, not a fabricated return
        // from2361C0/1A9240. The counted Camera34 remains at its source boundary.
        return session.coldViewport_.complete()&&session.coldCameraSelected26F940_&&
            session.viewport_.prepared&&session.viewport_.matricesReady&&
            session.viewport_.sessionGeneration==session.generation_&&
            session.viewport_.sceneKey==session.key(session.scene_)&&
            session.display_.rendererCurrentScene0==&session.viewport_&&
            session.selectedGameplayDisplay_.lock()==display&&display->selectedGameplayCamera0_==&session&&
            display->gameplaySelectionRevision_&&display->gameplaySelectionRevision_!=UINT64_MAX;
    }catch(...){return false;}
}

bool Application::consumedTaCamera34Engaged(const original_camera_session50KX::Session& session,
    bool requireBound)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!session.onOwnerThread())return false;
    const auto& race=sourceRace28COwner_;const auto& issued=session.nativeTaIssuance_;
    if(!race||race->world74_||!race->taWorld74_||!race->camera34_||
       !issued||issued->issuer!=this||issued->course!=rr::game::OriginalCameraStartCourse::ta||
       &session.actual_.race94!=&race->field94_||session.actual_.humanCount34!=1u)return false;
    // The canonical end route also needs this race's live native Root;
    // without it TA keeps the previous end policy (never a stalled boundary).
    return !requireBound||(race->cameraNativeAdapter34_==&session&&!race->endHud_.expired()&&
        nativeTaRootEntered_&&!nativeTaRootAborted_&&nativeTaRootRace_==race.get());
}

bool Application::bindConsumedRaCameraNative34(original_camera_session50KX::Session& session)const noexcept {
    using Step=rr::game::OriginalRaceConstructor50LA::CameraInitStep;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       !session.onOwnerThread())return false;
    try {
    const auto race=sourceRace28COwner_;const auto& issued=session.nativeTaIssuance_;
    if(!race||raceConstructorColdOwner50LA_.lock()!=race||race->application_!=entitiesOwner_||
       race->profile_!=riderProfile||!race->ownsNativeCameraAdapter34(session)||
       !issued||issued->issuer!=this||!race->raceCameraCourse34(issued->course)||
       issued->generation!=race->entity_.sessionGeneration||session.generation_!=issued->generation||
       session.actual_.applicationEntities!=entitiesOwner_||
       !session.actual_.applicationAnimation.sameOwner(animationClock())||
       session.closed_||session.abandoned_||!session.displayInputs_||!session.readsBound_||
       session.frameStage_==original_camera_session50KX::Session::FrameStage::failed||
       !session.identitiesCurrent()||session.actual_.humanCount34!=1u||
       &session.actual_.race94!=&race->field94_||
       (race->flags1B0KnownMask_&3u)!=3u||(race->flags1B0KnownValue_&3u)!=1u||race->field8C_!=0u||
       (race->cameraInitStep_!=Step::nativePending&&race->cameraInitStep_!=Step::nativeReady)||
       (race->cameraNativeAdapter34_&&race->cameraNativeAdapter34_!=&session))return false;
    const auto& camera=race->camera34_;
    if(!camera||camera->issuer_!=this||camera->entities_!=entitiesOwner_||
       camera->phase_!=rr::game::OriginalFrontendCameraCold50LA::Phase::beforeViewport114A00||
       !camera->coldRetirementPreflight(true).okay())return false;
    const auto display=raceDisplayOwner50LA_.lock();
    if(!display||display!=race->loadDisplay_||display->application_!=this||
       !display->gameplayDisplayIdentityCurrent50LA()||!display->video_->host.current()||
       &session.displayInputs_->renderer!=&display->renderer_||
       &session.displayInputs_->rendererDispatch!=&display->dispatch_)return false;
    race->cameraNativeAdapter34_=&session;
    session.raceMode8C_=static_cast<std::int32_t>(race->field8C_); //One-way native view.
    return true;
    }catch(...){return false;}
}

bool Application::completeConsumedRaCameraNative34(original_camera_session50KX::Session& session)const noexcept {
    if(!bindConsumedRaCameraNative34(session))return false;
    const auto race=sourceRace28COwner_;const auto display=race->loadDisplay_;
    const auto selected=session.selectedGameplayDisplay_.lock();
    // Reached native services, not a supplied readiness flag: existing cold
    // viewport owns both renderer drains and matrix work; the real selector has
    // published this exact Session on the same initialized Windows/Linux Display.
    if(!session.coldViewport_.complete()||!session.coldCameraSelected26F940_||
       session.coldPhase_!=1u||!session.viewport_.prepared||!session.viewport_.matricesReady||
       session.viewport_.sessionGeneration!=session.generation_||
       session.viewport_.sceneKey!=session.key(session.scene_)||
       session.display_.rendererCurrentScene0!=&session.viewport_||
       selected!=display||display->selectedGameplayCamera0_!=&session||
       !display->gameplaySelectionRevision_||display->gameplaySelectionRevision_==UINT64_MAX||
       session.viewport_.fullWidth204!=640||session.viewport_.fullHeight208!=448||
       session.displayInputs_->rendererWidth4!=640||session.displayInputs_->rendererHeight8!=448)return false;
    // Native backend completion only. The counted owner's source phase stays
    // before114A00: no fake243CB4 buffer count, PS2 callbacks, or copied second
    // camera table. All ongoing viewport/pose resources are this SAME Session.
    race->cameraInitStep_=rr::game::OriginalRaceConstructor50LA::CameraInitStep::nativeReady;
    return true;
}

bool Application::revokeConsumedRaCameraNative34(original_camera_session50KX::Session& session)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       !session.onOwnerThread())return false;
    const auto race=sourceRace28COwner_;
    if(!race||race->application_!=entitiesOwner_)return false;
    if(!race->cameraNativeAdapter34_)return true;
    if(race->cameraNativeAdapter34_!=&session||!race->nativeCameraAdapter34LifetimeCurrent(session))return false;
    // Before the first fallible Session close operation. This revokes only a
    // lexical native binding, not the source Camera pointer/count or destructor.
    race->cameraNativeAdapter34_=nullptr;
    race->cameraInitStep_=rr::game::OriginalRaceConstructor50LA::CameraInitStep::nativeRetiring;
    return true;
}

bool Application::retireConsumedRaCameraNative34(
    rr::game::OriginalRaceConstructor50LA& race,std::uint32_t& pc)const noexcept {
    pc=0x1a93ac;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       race.resultsFeedbackThread_!=std::this_thread::get_id())return false;
    if(raceConstructorColdOwner50LA_.lock().get()!=&race||race.application_!=entitiesOwner_||
       race.world74_||race.cameraNativeAdapter34_)return false;
    if(race.loadingCamera34_.entered)return abortConsumedRaLoadingCameraNative34(race,pc);
    if(!race.camera34_)return true;
    // Actual world/session cleanup must already have invalidated the native
    // selection. Do not erase a live global selection to force count retirement.
    if(race.loadDisplay_&&(race.loadDisplay_->selectedGameplayCamera0_||
       race.loadDisplay_->sourceSelectedCamera0_==race.camera34_))return false;
    auto& camera=*race.camera34_;
    if(camera.phase_!=rr::game::OriginalFrontendCameraCold50LA::Phase::constructedCold&&
       camera.phase_!=rr::game::OriginalFrontendCameraCold50LA::Phase::beforeViewport114A00)return false;
    // The existing native abort kernel means never published to source Camera
    // lists/Display/observer, NOT never retained in Race34. Its strict topology,
    // references and registration preflight remain unchanged. No1155C0 claim.
    const auto retired=camera.abortUnpublished236180(this,entitiesOwner_);
    if(!retired.okay()){pc=retired.sourcePC;return false;}
    race.camera34_.reset(); //Native slot withdrawal after actual count retirement.
    race.cameraInitStep_=rr::game::OriginalRaceConstructor50LA::CameraInitStep::retired;
    return true;
}
}

namespace original_camera_session50KX {
bool Session::bindCountedRaceCameraNative34()noexcept {
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_)return true;
    if(nativeTaIssuance_->course==OriginalCameraStartCourse::ta){
        if(!nativeTaIssuance_->issuer||!nativeTaIssuance_->issuer->consumedTaCamera34Engaged(*this,false))return true;
    }else if(nativeTaIssuance_->course!=OriginalCameraStartCourse::ra)return true;
    std::uint32_t pause=0;
    return nativeTaIssuance_->issuer&&actual_.applicationAnimation.readPause11C(pause)&&
        nativeTaIssuance_->issuer->bindConsumedRaCameraNative34(*this);
}
bool Session::completeCountedRaceCameraNative34()noexcept {
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_)return true;
    if(nativeTaIssuance_->course==OriginalCameraStartCourse::ta){
        if(!nativeTaIssuance_->issuer||!nativeTaIssuance_->issuer->consumedTaCamera34Engaged(*this,false))return true;
    }else if(nativeTaIssuance_->course!=OriginalCameraStartCourse::ra)return true;
    std::uint32_t pause=0;
    return nativeTaIssuance_->issuer&&actual_.applicationAnimation.readPause11C(pause)&&
        nativeTaIssuance_->issuer->completeConsumedRaCameraNative34(*this);
}
bool Session::revokeCountedRaceCameraNative34()noexcept {
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_)return true;
    // TA: only a consumed (tournament) race can hold this Session as its
    // Camera34 adapter. Single Event/practice TA has no Race28C: nothing bound.
    if(nativeTaIssuance_->course==OriginalCameraStartCourse::ta){
        if(!nativeTaIssuance_->issuer||!nativeTaIssuance_->issuer->race28C())return true;
    }else if(nativeTaIssuance_->course!=OriginalCameraStartCourse::ra)return true;
    std::uint32_t pause=0;
    return nativeTaIssuance_->issuer&&actual_.applicationAnimation.readPause11C(pause)&&
        nativeTaIssuance_->issuer->revokeConsumedRaCameraNative34(*this);
}
}

#include "original_race_loading_camera50la.inl"

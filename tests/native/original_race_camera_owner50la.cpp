#include "original_camera_session50kx.h"
#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "original_race_display_lifetime50la.h"
#include "common_race_world50cr.h"
#include <thread>
#include <utility>
#include <cfenv>
#include <cstdio>

namespace rr::game {
// Common rewrite (R2): one rule for every course. The race's Camera34 adapter is
// the unique Application-issued course Session of THIS race, issued for this
// race's one World at race+74 (no RA world-owned Session, no per-course world).
bool OriginalRaceConstructor50LA::nativeCameraAdapter34LifetimeCurrent(
    const original_camera_session50KX::Session& session)const noexcept {
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!counted_||rolledBack_||!world74_)return false;
    // Teardown compares actual unique ownership even after prepare has become
    // terminal. It must not require the live admission it is trying to revoke.
    const auto& issued=session.nativeTaIssuance_;
    if(!issued||!issued->issuer||issued->race!=this||issued->world!=world74_.get()||
       issued->issuer->race28C()!=this||issued->generation!=entity_.sessionGeneration)return false;
    return issued->issuer->issuedCourseCameraSession50LA().get()==&session;
}
bool OriginalRaceConstructor50LA::ownsNativeCameraAdapter34(
    const original_camera_session50KX::Session& session)const noexcept {
    if(!nativeCameraAdapter34LifetimeCurrent(session))return false;
    // The counted prefix of this race's one World.
    return world74_->counted&&world74_->prefixComplete&&!session.closed_&&!session.abandoned_;
}
std::shared_ptr<original_camera_session50KX::Session> OriginalRaceConstructor50LA::borrowRaceCamera34()const noexcept {
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!counted_||rolledBack_)return {};
    if(!world74_||!cameraNativeAdapter34_)return {};
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
       !race.counted_||race.rolledBack_||!race.world74_||!common_race_world50CR::introWorldEntity(race)||
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
    // The ALL8 spectator race (humans34 0) is issued the same Application
    // course Session as every race. Derive custody from that actual unique
    // Session of this race's one World, never from a matching key or a
    // synthetic completion token. This function performs no source writes.
    // Music10C is a separate owner: its presence or lexical provider revocation
    // must not revoke camera custody or strand an active Pause during cleanup.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!session.onOwnerThread())return false;
    try {
        const auto& race=sourceRace28COwner_;
        if(!race||raceConstructorColdOwner50LA_.lock()!=race||race->application_!=entitiesOwner_||
           !riderProfile||race->profile_!=riderProfile||!common_race_world50CR::introWorldEntity(*race)||
           !race->ownsNativeCameraAdapter34(session)||
           race->worldInitStep_!=rr::game::OriginalRaceConstructor50LA::WorldInitStep::camera||
           race->cameraInitStep_!=Step::nativePending||race->cameraNativeAdapter34_||
           (race->flags1B0KnownMask_&3u)!=3u||(race->flags1B0KnownValue_&3u)!=1u||race->field8C_||
           !session.nativeTaIssuance_||session.nativeTaIssuance_->issuer!=this||
           session.generation_!=race->entity_.sessionGeneration||
           session.actual_.applicationEntities!=entitiesOwner_||
           !session.actual_.applicationAnimation.sameOwner(animationClock())||
           // The ALL8 field is the manager's own count (1AD520 limit 8), not a fixed eight.
           session.actual_.humanCount34||!session.actual_.bank.count()||session.actual_.bank.count()>8u||
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
    // Every consumed course race: one race class in the original, every course.
    const bool sameWorld=race&&issued&&issued->race==race.get()&&issued->world==race->world74_.get();
    if(!race||!race->world74_||!race->camera34_||
       !issued||issued->issuer!=this||!sameWorld||
       &session.actual_.race94!=&race->field94_||session.actual_.humanCount34!=1u){
        if(!requireBound){static int reported=0;if(reported++<2)
            std::fprintf(stderr,"common_race_camera34 engaged=0 race=%d world74=%d camera34=%d issued=%d same_world=%d race94=%d humans=%u\n",
                int(bool(race)),race?int(bool(race->world74_)):-1,race?int(bool(race->camera34_)):-1,
                int(bool(issued)),int(sameWorld),
                race?int(&session.actual_.race94==&race->field94_):-1,session.actual_.humanCount34);}
        return false;
    }
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
    // Read-only refusal diagnostic (one line per distinct gate).
    const auto refused=[](int gate)noexcept{static int last=0;
        if(gate!=last){last=gate;std::fprintf(stderr,"camera34_bind_refused gate=%d\n",gate);}return false;};
    if(!race||raceConstructorColdOwner50LA_.lock()!=race||race->application_!=entitiesOwner_)return refused(1);
    if(race->profile_!=riderProfile||!race->ownsNativeCameraAdapter34(session))return refused(2);
    if(!issued||issued->issuer!=this||issued->race!=race.get()||issued->world!=race->world74_.get()||
       issued->generation!=race->entity_.sessionGeneration||session.generation_!=issued->generation)return refused(3);
    if(session.closed_||session.abandoned_||!session.displayInputs_)return refused(4);
    if(!session.readsBound_||!session.identitiesCurrent())return refused(5);
    if(session.actual_.humanCount34!=1u||&session.actual_.race94!=&race->field94_)return refused(6);
    if((race->flags1B0KnownMask_&3u)!=3u||(race->flags1B0KnownValue_&3u)!=1u||race->field8C_!=0u)return refused(7);
    if(race->cameraInitStep_!=Step::nativePending&&race->cameraInitStep_!=Step::nativeReady)return refused(8);
    if(!race||raceConstructorColdOwner50LA_.lock()!=race||race->application_!=entitiesOwner_||
       race->profile_!=riderProfile||!race->ownsNativeCameraAdapter34(session)||
       !issued||issued->issuer!=this||issued->race!=race.get()||issued->world!=race->world74_.get()||
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
       !camera->coldRetirementPreflight(true).okay())return refused(9);
    const auto display=raceDisplayOwner50LA_.lock();
    if(!display||display!=race->loadDisplay_||display->application_!=this)return refused(10);
    // The race display's creation marks the start of gameplay camera custody
    // (source Camera0 no longer the known frontend/loading value). When the
    // race camera starts before the display content exists (display brought
    // up, not created), that same transition happens here, once.
    if(!display->created_&&!display->attempted_&&display->renderer_.broughtUp()&&
       display->sourceCurrentCamera0Known_&&!display->sourceSelectedCamera0_){
        display->sourceCurrentCamera0Known_=false;
        display->invalidateGameplayCameraObservation50LA();
        std::fprintf(stderr,"race_display_gameplay_camera_start before_create=1\n");
    }
    if(!display->gameplayDisplayIdentityCurrent50LA()){
        std::fprintf(stderr,"camera34_display_identity created=%d attempted=%d up=%d known=%d selected=%d\n",
            int(display->created_),int(display->attempted_),int(display->renderer_.broughtUp()),
            int(display->sourceCurrentCamera0Known_),int(bool(display->sourceSelectedCamera0_)));
        return refused(11);
    }
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
       race.cameraNativeAdapter34_)return false;
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
// Every issued course session binds its race's Camera34 once engaged (one rule).
bool Session::bindCountedRaceCameraNative34()noexcept {
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_)return true;
    if(!nativeTaIssuance_->issuer||!nativeTaIssuance_->issuer->consumedTaCamera34Engaged(*this,false))return true;
    std::uint32_t pause=0;
    return actual_.applicationAnimation.readPause11C(pause)&&
        nativeTaIssuance_->issuer->bindConsumedRaCameraNative34(*this);
}
bool Session::completeCountedRaceCameraNative34()noexcept {
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_)return true;
    if(!nativeTaIssuance_->issuer||!nativeTaIssuance_->issuer->consumedTaCamera34Engaged(*this,false))return true;
    std::uint32_t pause=0;
    return actual_.applicationAnimation.readPause11C(pause)&&
        nativeTaIssuance_->issuer->completeConsumedRaCameraNative34(*this);
}
bool Session::revokeCountedRaceCameraNative34()noexcept {
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_)return true;
    // Only a consumed race can hold this Session as its Camera34 adapter.
    // A launch without Race28C has nothing bound.
    if(!nativeTaIssuance_->issuer||!nativeTaIssuance_->issuer->race28C())return true;
    std::uint32_t pause=0;
    return actual_.applicationAnimation.readPause11C(pause)&&
        nativeTaIssuance_->issuer->revokeConsumedRaCameraNative34(*this);
}
}

#include "original_race_loading_camera50la.inl"

#include "original_race_display_lifetime50la.h"
#include "original_race_constructor50la.h"
#include "original_race_world_join50la.h"
#include "original_camera_session50kx.h"
#include <bit>
#include <exception>
#include <limits>
#include <thread>

namespace original_main_entry50KG {
bool RaceDisplayLifetime50LA::gameplayDisplayIdentityCurrent50LA()const noexcept {
    // The race display exists from boot in the original: brought up and not
    // yet created (the race load before its single population) is the same
    // display as the created one.
    if(!ownerThread()||retiring_||frontendCreated_||!video_||!application_||
       !(created_?renderer_.initialized():(!attempted_&&renderer_.broughtUp()))||
       !video_->originalMain||!video_->host.created()||
       renderer_.sessionDisplayOwner50LA_!=this||video_->host.sessionDisplayRenderer50LA_!=&renderer_||
       sourceCurrentCamera0Known_||sourceSelectedCamera0_)return false;
    std::uint32_t ignoredPause=0;
    if(!applicationLifetime_.readPause11C(ignoredPause))return false;
    const auto issued=application_->raceDisplayOwner50LA_.lock();
    const auto& issuedVideo=application_->videoOwner_;
    return issued&&issued.get()==this&&
        !issued.owner_before(weak_from_this())&&!weak_from_this().owner_before(issued)&&
        application_->raceDisplayGeneration50LA_==generation_&&issuedVideo&&
        issuedVideo==video_&&!issuedVideo.owner_before(video_)&&!video_.owner_before(issuedVideo)&&
        applicationLifetime_.sameOwner(application_->animationClock())&&
        application_->entitiesOwner_&&application_->entitiesOwner_->application10();
}

void RaceDisplayLifetime50LA::observeGameplayCamera26F940(
    original_camera_session50KX::Session& camera)noexcept {
    // Called at the reached existing26F940 producer, before coldPhase advances.
    // Existing standalone/legacy preparation remains unchanged. Unsupported
    // owners get no F8 authority, NOT a new failure of their original route.
    if(!camera.displayInputs_)return;
    auto& renderer=camera.displayInputs_->renderer;
    auto* display=const_cast<RaceDisplayLifetime50LA*>(
        static_cast<const RaceDisplayLifetime50LA*>(renderer.sessionDisplayOwner50LA_));
    if(!display)return;
    if(!display->ownerThread()||&display->renderer_!=&renderer)std::terminate();
    // Even a subsequently rejected producer supersedes an older observation.
    // The retained frontend pointer may not be discarded as an invented ref0.
    display->invalidateGameplayCameraObservation50LA();
    display->sourceCurrentCamera0Known_=false;
    if(camera.result_.start.status!=rr::game::OriginalCameraStartStatus::complete||
       camera.result_.start.sourcePC!=0x26f998||!display->gameplayDisplayIdentityCurrent50LA()||
       display->gameplaySelectionRevision_==std::numeric_limits<std::uint64_t>::max()||
       &camera.displayInputs_->rendererDispatch!=&display->dispatch_||
       camera.display_.rendererDispatch!=&display->dispatch_||
       camera.actual_.applicationEntities!=display->application_->entitiesOwner_||
       !camera.actual_.applicationAnimation.sameOwner(display->applicationLifetime_)||
       !camera.generation_||camera.display_.sessionGeneration!=camera.generation_||
       camera.display_.raceKey!=camera.key(camera)||
       camera.display_.rendererCurrentScene0!=&camera.viewport_||
       !camera.viewport_.prepared||camera.viewport_.sessionGeneration!=camera.generation_||
       camera.viewport_.sceneKey!=camera.key(camera.scene_)||camera.closed_||camera.abandoned_||
       camera.viewport_.fullWidth204!=camera.displayInputs_->rendererWidth4||
       camera.viewport_.fullHeight208!=camera.displayInputs_->rendererHeight8)return;
    const auto previous=camera.selectedGameplayDisplay_.lock();
    if(previous&&previous.get()!=display){
        if(previous->selectedGameplayCamera0_==&camera)previous->invalidateGameplayCameraObservation50LA();
        return; // A Session's real DisplayInputs cannot be rebound.
    }
    camera.selectedGameplayDisplay_=display->weak_from_this();
    display->selectedGameplayCamera0_=&camera;
}

bool RaceDisplayLifetime50LA::readGameplayViewport115990(
    const original_camera_session50KX::Session& camera,
    std::array<std::int32_t,4>& rectangle,std::uint64_t& revision)const noexcept {
    // The caller holds the actual world's counted Camera borrow. We compare
    // identity BEFORE accessing the supplied live Session, never dereference
    // selectedGameplayCamera0_ as a free-standing/remembered raw owner.
    if(!gameplayDisplayIdentityCurrent50LA()||selectedGameplayCamera0_!=&camera||
       !gameplaySelectionRevision_||gameplaySelectionRevision_==std::numeric_limits<std::uint64_t>::max())return false;
    const auto selectedDisplay=camera.selectedGameplayDisplay_.lock();
    if(!selectedDisplay||selectedDisplay.get()!=this||
       !selectedDisplay->applicationLifetime_.sameOwner(applicationLifetime_)||
       !camera.coldBound_||camera.closed_||camera.abandoned_||!camera.displayInputs_||
       (camera.frameStage_!=original_camera_session50KX::Session::FrameStage::idle&&
        camera.frameStage_!=original_camera_session50KX::Session::FrameStage::published)||
       &camera.displayInputs_->renderer!=&renderer_||
       &camera.displayInputs_->rendererDispatch!=&dispatch_||camera.display_.rendererDispatch!=&dispatch_||
       camera.actual_.applicationEntities!=application_->entitiesOwner_||
       !camera.actual_.applicationAnimation.sameOwner(applicationLifetime_)||
       camera.display_.sessionGeneration!=camera.generation_||camera.display_.raceKey!=camera.key(camera)||
       camera.display_.rendererCurrentScene0!=&camera.viewport_||!camera.viewport_.prepared||
       camera.viewport_.sessionGeneration!=camera.generation_||camera.viewport_.sceneKey!=camera.key(camera.scene_)||
       camera.display_.cameraCount264!=1||camera.display_.applicationCameras254[0]!=camera.viewport_.sceneKey||
       camera.viewport_.fullWidth204!=camera.displayInputs_->rendererWidth4||
       camera.viewport_.fullHeight208!=camera.displayInputs_->rendererHeight8)return false;
    const auto& v=camera.viewport_;
    //115990 stores left,right,top,bottom in that order. ADDU is modulo32,
    // followed by signed CVT.S.W in198CF0; avoid native signed-overflow UB.
    std::array<std::int32_t,4> next;
    next[0]=v.x1E0;
    next[2]=std::bit_cast<std::int32_t>(std::uint32_t(v.x1E0)+std::uint32_t(v.width1E8));
    next[1]=v.y1E4;
    next[3]=std::bit_cast<std::int32_t>(std::uint32_t(v.y1E4)+std::uint32_t(v.height1EC));
    rectangle=next;revision=gameplaySelectionRevision_;return true;
}

bool Application::layoutConsumedF81994B0(rr::game::OriginalRaceF8Diagnostic50LA& d)const noexcept {
    d={};const auto reject=[&](std::uint32_t pc,const char* why)noexcept {
        d.sourcePC=pc;d.operation=why;return false;
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject(0x1994b0,"F8 layout requires the Application owner thread");
    const auto race=sourceRace28COwner_;
    const auto display=raceDisplayOwner50LA_.lock();
    if(!race||race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||
       !race->world74_||!display||race->loadDisplay_!=display||display->application_!=this||
       display->generation_!=raceDisplayGeneration50LA_||!race->f8Attached_||!race->f8Owner_||
       race->f8Source_!=race->f8Owner_.get()||
       !race->f8Owner_->belongsTo(entitiesOwner_,race->entity_.sessionGeneration))
        return reject(0x1994c0,"F8 layout requires the same consumed Race/World/Application Display");
    const auto* f8=race->f8Owner_->entity();
    if(!f8||f8->parent1C!=&race->entity_||f8->mask20!=0xffffffffu||f8->firstChild18||f8->descendants24)
        return reject(0x1994b0,"F8 layout requires its actual attached empty child");
    // 1994B0 for every race (B-C06): the Application-issued Session bound as
    // the race's Camera34, issued for this race and its one World. The
    // synchronous borrow keeps it alive; close/abandon reject while held.
    const auto* world=race->world74_.get();
    const auto camera=race->borrowRaceCamera34();
    if(!camera||camera->generation_!=race->entity_.sessionGeneration||
       camera->actual_.applicationEntities!=entitiesOwner_||camera->actual_.humanCount34!=1||
       !camera->nativeTaIssuance_||camera->nativeTaIssuance_->issuer!=this||
       camera->nativeTaIssuance_->generation!=camera->generation_||
       !race->raceCamera34Issued(*camera))
        return reject(0x1994c8,"F8 requires the actual retained one-human course Camera/issuer");
    std::array<std::int32_t,4> rectangle{},after{};std::uint64_t revision=0,afterRevision=0;
    if(!display->readGameplayViewport115990(*camera,rectangle,revision))
        return reject(0x115990,"F8 requires this Display's actual current gameplay Camera");
    // Deliberately uncalled by the live root. This is CPU layout only: no
    // draw, queue, GL, renderer service, acknowledgment or end/input mutation.
    if(!race->f8Owner_->layout198CF0(rectangle,d))return false;
    if(sourceRace28COwner_!=race||race->world74_.get()!=world||race->loadDisplay_!=display||
       raceDisplayOwner50LA_.lock()!=display||raceDisplayGeneration50LA_!=display->generation_||
       display->application_!=this||camera->generation_!=race->entity_.sessionGeneration||
       !camera->nativeTaIssuance_||camera->nativeTaIssuance_->issuer!=this||
       camera->nativeTaIssuance_->generation!=camera->generation_||
       !race->raceCamera34Issued(*camera)||
       !display->readGameplayViewport115990(*camera,after,afterRevision)||
       afterRevision!=revision||after!=rectangle)
        return reject(0x1994e8,"F8 owner/selection changed after CPU stores; effects retained");
    return true;
}
}

namespace original_camera_session50KX {
void Session::observeSelectedGameplayCamera26F940()noexcept {
    original_main_entry50KG::RaceDisplayLifetime50LA::observeGameplayCamera26F940(*this);
}
void Session::invalidateSelectedGameplayCamera50LA()noexcept {
    const auto display=selectedGameplayDisplay_.lock();
    if(display&&!display->ownerThread())std::terminate();
    if(display&&display->selectedGameplayCamera0_==this)
        display->invalidateGameplayCameraObservation50LA();
    selectedGameplayDisplay_.reset();
    // Native observation invalidation ONLY. Do not clear display_'s original
    // selected pointer, set known-null, call115690, or claim renderer retirement.
}
}

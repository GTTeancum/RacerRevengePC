#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"

namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::lateSceneBorrowFailureCurrent()const noexcept {
    // Historical failed-call identity, not the Camera's mutable live phase.
    // Retaining this receipt does not authorize Scene or Page count release.
    if(!failedLateBorrow1B4D24_||ownerThread_!=std::this_thread::get_id()||!issuer_||!grant_||
       failedLateBorrowGeneration_!=grant_->generation_||failedLateBorrowEpoch_!=grant_->epoch_||
       grant_->issuer()!=issuer_||!authority_||grant_->application().get()!=authority_.get()||
       issuer_->entities().get()!=authority_.get()||
       authority_.owner_before(grant_->application())||grant_->application().owner_before(authority_)||
       authority_.owner_before(issuer_->entities())||issuer_->entities().owner_before(authority_)||
       managerAttached1D1C40_||!scene4C0_||sceneBacking_.get()!=scene4C0_||retiredSceneBacking_||
       !scenePageBorrowCaptured_||scenePageAttachCursor_!=10||cameraHandoffPhase_!=3||
       sceneLateBorrowCaptured_||sceneLateAttachCursor_||sceneLateRetiredCount_||
       activationPhase_!=ActivationPhase::notActivated||entryPhase_!=EntryPhase::notEntered||
       loadReturnPhase_!=LoadReturnPhase::forestPending||updatePublication_||drawPublication_||inputSerial_||
       !entityLive_||entityRefReleased_||entityCountReleased_||stage_!=Stage::presentationLoaded)
        return false;
    for(const auto* child:sceneLateChildren_)if(child)return false;
    const auto& session=issuer_->garageCustodyOwner_;
    return session&&session->page_.get()==this&&session->podScene_==scene4C0_&&session->failed_&&
        session->entryManagerGeneration_==failedLateBorrowGeneration_&&session->grant_==grant_&&
        grant_->garagePageCurrent(this,entity_);
}
bool OriginalUiGaragePageOwner50KG::preflightFailedLateBorrowCamera231310(Diagnostic& d)const noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    d={};
    if(!lateSceneBorrowFailureCurrent()||!currentConstructor()||sceneInitialRetiredCount_||
       cameraRetirementStep_||payloadRetirementStep_||baseRetirementStep_||
       !sceneAttachmentPrefixCurrent1B48A0()||!scene4C0_->sourceLoadReturned1FB59C()||
       !sharedCamera_||shared_.camera4BC!=sharedCamera_.get()||!managerSearch_||!managerSearch_->current()||
       managerSearch_->camera_||managerSearch_->cameraObserver_||
       !sharedCameraDisplay_||!sharedCameraDisplayPinned_||sharedCameraDisplay_->application_!=issuer_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||!sharedCameraDisplay_->sourceCurrentCamera0Known_||
       sharedCameraDisplay_->sourceSelectedCamera0_!=sharedCamera_||
       sharedCameraDisplay_->localizationServiceActive_||
       sharedCameraDisplay_->dispatch_.pendingCallbackDC||sharedCameraDisplay_->dispatch_.count54||
       sharedCameraDisplay_->dispatch_.count60||sharedCamera_->failedLoaderRetirementStep_||
       sharedCamera_->phase_!=Camera::Phase::pageAttached1FB620||
       !sharedCamera_->nativeRegistrationIdentityCurrent()||sharedCamera_->entity_.parent1C!=&entity_||
       sharedCamera_->entity_.next14||sharedCamera_->entity_.firstChild18||
       !grant_->garagePageCameraCurrent(*this,*sharedCamera_)||!cameraRegistrationCurrent1FB608())
        return fail(d,Status::childRetirementPending,0x231310);
    d.sourcePC=0x231310;return true;
}
bool OriginalUiGaragePageOwner50KG::retireFailedLateBorrowCamera231310(Diagnostic& d)noexcept {
    d={};
    if(!lateSceneBorrowFailureCurrent()||sceneInitialRetiredCount_||
       payloadRetirementStep_||baseRetirementStep_)
        return fail(d,Status::childRetirementPending,0x1B4D24);
    if(!cameraRetirementStep_&&!preflightFailedLateBorrowCamera231310(d))return false;
    // The receiver really is the final attached Page child. Use its exact
    // 142590 -> 231310 -> 1155C0 -> 1413E0 continuation, including the real
    // Scene observer280920. Do NOT unregister1148D0 or remove the Search row.
    if(!retireCameraChild231310(d))return false;
    if(cameraRetirementStep_!=6||!sharedCamera_||
       sharedCamera_->phase_!=game::OriginalFrontendCameraCold50LA::Phase::released||
       sharedCamera_->enrolled_||sharedCamera_->entity_.reference00)
        return fail(d,Status::childRetirementPending,0x1413E0);
    // Detached late owners and the ten real initial children remain retained.
    // In particular child9 cannot retire while emitter model/pool borrows live.
    d.sourceEffectsCommitted=true;d.sourcePC=0x231358;return true;
}
}

#include "original_ui_garage_page_owner50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"

namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::preflightFailedSharedCamera231310(Diagnostic& d)const noexcept {
    return preflightFailedManagerCamera231310(false,d);
}
bool OriginalUiGaragePageOwner50KG::preflightFailedPartialCamera231310(Diagnostic& d)const noexcept {
    return preflightFailedManagerCamera231310(true,d);
}
bool OriginalUiGaragePageOwner50KG::preflightFailedManagerCamera231310(bool partialScene,Diagnostic& d)const noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    d={};
    if(!(partialScene?partialSceneAttachmentFailureCurrent():sharedPromptFailurePrefixCurrent())||!sharedCamera_||shared_.camera4BC!=sharedCamera_.get()||
       !managerSearch_||!managerSearch_->current()||!sharedCameraDisplay_||!sharedCameraDisplayPinned_||
       sharedViewportDrainPhase_!=2||sharedCameraCallbackCount_!=2||cameraRetirementStep_||
       sharedCamera_->failedLoaderRetirementStep_||sharedCamera_->phase_!=Camera::Phase::poseSet114F90||
       sharedCamera_->issuer_!=issuer_||sharedCamera_->entities_!=authority_||
       sharedCameraDisplay_->application_!=issuer_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||!sharedCameraDisplay_->canRetain()||
       !sharedCameraDisplay_->sourceCurrentCamera0Known_||
       sharedCameraDisplay_->sourceSelectedCamera0_!=sharedCamera_||
       sharedCameraDisplay_->localizationServiceActive_||
       sharedCameraDisplay_->dispatch_.pendingCallbackDC||sharedCameraDisplay_->dispatch_.count54||
       sharedCameraDisplay_->dispatch_.count60)
        return fail(d,Status::childRetirementPending,0x231310);
    // The original live unregister is required:1155C0 alone frees observer34
    // without removing its row from the still-live manager Search list.
    if(managerSearch_->camera_!=sharedCamera_||!managerSearch_->cameraRegistrationCurrent()||
       sharedCamera_->fields_.parent38!=&managerSearch_->node_)
        return fail(d,Status::childRetirementPending,0x1148D0);
    d.camera=sharedCamera_->preflightFailedLoaderEntity104970();
    if(!d.camera.okay())return fail(d,Status::childRetirementPending,d.camera.sourcePC);
    if(!sharedCameraDisplay_->preflightFailedLoaderCamera231310(*sharedCamera_,d.renderer))
        return fail(d,Status::rendererRetirementPending,0x231310);
    d.sourcePC=0x231310;return true;
}

bool OriginalUiGaragePageOwner50KG::retireFailedSharedCamera231310(Diagnostic& d)noexcept {
    return retireFailedManagerCamera231310(false,d);
}
bool OriginalUiGaragePageOwner50KG::retireFailedPartialCamera231310(Diagnostic& d)noexcept {
    return retireFailedManagerCamera231310(true,d);
}
bool OriginalUiGaragePageOwner50KG::retireFailedManagerCamera231310(bool partialScene,Diagnostic& d)noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    d={};
    if(!(partialScene?partialSceneAttachmentFailureCurrent():sharedPromptFailurePrefixCurrent())||!sharedCamera_||shared_.camera4BC!=sharedCamera_.get()||
       !managerSearch_||!managerSearch_->current()||!sharedCameraDisplay_||!sharedCameraDisplayPinned_||
       sharedCamera_->issuer_!=issuer_||sharedCamera_->entities_!=authority_||
       sharedCameraDisplay_->application_!=issuer_||
       !sharedCameraDisplay_->frontendOwnerCurrent50KG()||
       !sharedCameraDisplay_->sourceCurrentCamera0Known_||cameraRetirementStep_)
        return fail(d,Status::childRetirementPending,0x231310);
    auto& camera=*sharedCamera_;
    auto& step=camera.failedLoaderRetirementStep_;
    d.sourceEffectsCommitted=step!=0;
    if(!step){
        if(!preflightFailedManagerCamera231310(partialScene,d))return false;
        OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!managerSearch_->unregisterCamera1148D0(sharedCamera_,search))
            return fail(d,Status::childRetirementPending,search.sourcePC);
        step=1;d.sourceEffectsCommitted=true; //Actual114910 return, before ref0.
    }
    //1148D0 deliberately leaves Camera.parent38 unchanged. No Scene observer
    // is fabricated and no dying-source field is cleared to pass a guard.
    if(managerSearch_->camera_||managerSearch_->cameraObserver_||camera.fields_.registration34||
       camera.fields_.parent38!=&managerSearch_->node_)
        return fail(d,Status::childRetirementPending,0x114910);
    if(step==1){
        d.camera=camera.beginFailedLoaderRetirement104970();
        if(!d.camera.okay())return fail(d,Status::childRetirementPending,d.camera.sourcePC);
    }
    if(step>=2&&step<=5&&!camera.failedLoaderRef0Current1155C0())
        return fail(d,Status::childRetirementPending,0x1155C0);
    if(step==2){
        //1155EC skips280920 because11490C actually nulled34 after releasing
        //the real observer.1144F4 initialized224; no reached producer changed it.
        if(camera.fields_.resource224)return fail(d,Status::childRetirementPending,0x13D9F0);
        step=3;
    }
    if(step==3){
        if(!camera.retireFailedLoaderApplicationSlots115620())
            return fail(d,Status::childRetirementPending,0x115620);
        step=4;
    }
    if(step==4){
        //115684 compare,115690 ->26F940(nullptr). This is the same real retained
        //Display source field as the normal destructor, not a renderer release.
        if(!sharedCameraDisplay_->retireFailedLoaderSelectedCamera115690(camera))
            return fail(d,Status::rendererRetirementPending,0x115690);
        step=5;
    }
    if(step==5){
        if(sharedCameraDisplay_->sourceSelectedCamera0_.get()==&camera)
            return fail(d,Status::rendererRetirementPending,0x115690);
        d.camera=camera.finishFailedLoaderRetirement1413E0();
        if(!d.camera.okay())return fail(d,Status::childRetirementPending,d.camera.sourcePC);
    }
    if(step!=6||camera.phase_!=Camera::Phase::released||camera.enrolled_||camera.entity_.reference00)
        return fail(d,Status::childRetirementPending,0x1413E0);
    //Native backing and Display pin remain with the actual Page owner. Its
    //later teardown may release them only after this genuine final count step.
    d.sourceEffectsCommitted=true;d.sourcePC=0x231358;return true;
}
}

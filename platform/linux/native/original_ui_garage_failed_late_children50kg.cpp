#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"

namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::retireFailedLateBorrowInitialChildren142590(Diagnostic& d)noexcept {
    d={};
    if(!lateSceneBorrowFailureCurrent()||!currentConstructor()||
       sceneInitialRetiredCount_>scenePageChildren_.size()||
       payloadRetirementStep_||baseRetirementStep_||!scene4C0_->sceneEmitters_||
       !scene4C0_->sceneParticles_||!scene4C0_->toyFields_)
        return fail(d,Status::childRetirementPending,0x1B4D24);
    if(!sceneInitialRetiredCount_){
        if(!retireFailedLateBorrowCamera231310(d))return false;
        // Native failed-constructor ownership unwind, not a fabricated late
        // Page callback. This exact list was never attached at1B4D24. Its
        // seven actual children retire tail-first via15A720, including their
        // real model pins and numeric pool references;27DCD0 then frees the
        // wrappers/pointer arrays after list ref0 and before final1413E0.
        assets::OriginalStandardMaterialDiagnostic50LA child;
        if(!scene4C0_->retireSceneParticleList27DCD0(child)){
            d.loadResources=child;
            return fail(d,Status::childRetirementPending,child.sourcePC);
        }
    }
    // The normal child9 destructor independently rejects any live emitter or
    // pool borrow. Every actual callback retains its own resumable cursor,
    // and sceneInitialRetiredCount_ advances only after its genuine return.
    if(!retireInitialSceneChildren142590(d))return false;
    // Detached material-Sound lists, Router, Page and Scene remain retained.
    // No full Page retirement or normal late-child receipt is minted here.
    d.sourceEffectsCommitted=true;d.sourcePC=0x1425B8;return true;
}
bool OriginalUiGaragePageOwner50KG::retireFailedLateBorrowDetachedAudio271470(Diagnostic& d)noexcept {
    d={};
    if(!lateSceneBorrowFailureCurrent()||!currentConstructor()||
       sceneInitialRetiredCount_!=scenePageChildren_.size()||cameraRetirementStep_!=6||
       payloadRetirementStep_||baseRetirementStep_)
        return fail(d,Status::childRetirementPending,0x271470);
    // Neither list acquired a late Page link. Release the actual detached
    // drive-on/collision configuration owners, preserving Sound/Group custody.
    assets::OriginalStandardMaterialDiagnostic50LA child;
    for(const auto ordinal:{std::size_t{2},std::size_t{1}}){
        if(!scene4C0_->retireLateBorrowMaterialSoundList(*this,ordinal,child)){
            d.loadResources=child;
            return fail(d,Status::childRetirementPending,child.sourcePC);
        }
    }
    if(!grant_->garageLateBorrowAudioListsRetired(*this,*scene4C0_))
        return fail(d,Status::childRetirementPending,0x1413E0);
    // Router, Page and Scene are still live. The terminal caller must perform
    // their distinct real ref0/payload/base/count stages, never a normal entry.
    d.sourceEffectsCommitted=true;d.sourcePC=0x1413E0;return true;
}

}

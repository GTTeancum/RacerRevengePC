#pragma once
#include "original_race_display_lifetime50la.h"
#include "original_race_world_join50la.h"
#include "original_pause_entity50la.h"

namespace rr::game {
// Connected Pause100/Results104 forest and retirement bodies are out of line
// in original_race_results_parent50la.cpp. Legacy Pause-only callers remain valid.

}
namespace original_main_entry50KG {
inline bool Application::attachNativeConsumedRacePause(rr::game::OriginalRaceConstructor50LA& owner,
    const std::shared_ptr<original_camera_session50KX::Session>& camera)const noexcept {
    using P=rr::game::OriginalPauseStorage50LA;
    if(sourceRace28COwner_.get()!=&owner||owner.application_!=entitiesOwner_||owner.profile_!=riderProfile||
       !camera||!owner.world74_||
       (owner.results104_&&(!owner.results104_->loadedDisabled()||
        !owner.results104_->belongsTo(entitiesOwner_,owner.entity_.sessionGeneration)))||!owner.pause100_||pauseColdOwner50LA_.lock()!=owner.pause100_||
       owner.pause100_->stage_!=P::Stage::derivedResources||!owner.pause100_->entity_||
       !owner.pause100_->entity_->disabledAfterLoad||owner.pause100_->references_.failed()||
       owner.pause100_->references_.associatedCount()!=rr::assets::OriginalPausePrimitiveReferences50LA::count||
       owner.pause100_->domain_!=standardMaterialDomainOwner_||owner.pause100_->context_!=materialContextOwner_||
       !materialContextOwner_||!materialContextOwner_->live()||!owner.loadDisplay_||
       raceDisplayOwner50LA_.lock()!=owner.loadDisplay_||!RaceDisplayLifetime50LA::uploadBindingLive(*owner.loadDisplay_))return false;
    // The race's one World holds its loaded common Track (every course), and the
    // camera is the race's own issued Camera34.
    if(!owner.world74_->trackData())return false;
    const auto actualCamera=owner.borrowRaceCamera34();
    if(!actualCamera||actualCamera.get()!=camera.get())return false;
    // Actual prepared native camera/bank and loaded draw resources admit this
    // native route. They are not a receipt for unrepresented original slot30.
    return owner.attachNativePauseParent();
}
}

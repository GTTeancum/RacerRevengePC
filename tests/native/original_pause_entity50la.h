#pragma once
#include "original_ui_main_entry50kg.h"
#include "../../src/game/original_pause_storage50la.h"

namespace original_main_entry50KG {
inline std::shared_ptr<rr::game::OriginalPauseStorage50LA> Application::constructPauseCold1A8AB0(
    std::uint64_t actualSessionGeneration)const noexcept {
    using Pause=rr::game::OriginalPauseStorage50LA;
    if(!actualSessionGeneration||!pauseColdOwner50LA_.expired()||!entitiesOwner_||
       !entitiesOwner_->application10()||!materialContextOwner_||!materialContextOwner_->live()||
       materialContextOwner_->materialApplication()!=materialApplication||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return {};
    try {
        auto owner=std::shared_ptr<Pause>(new Pause(entitiesOwner_,actualSessionGeneration));
        if(owner->stage_!=Pause::Stage::fresh||!owner->entity_||!owner->entity_->detached())return {};
        pauseColdOwner50LA_=owner;return owner;
    }catch(...){return {};}
}
inline bool Application::finishPauseEntityCold1A8ADC(rr::game::OriginalPauseStorage50LA& owner)const noexcept {
    using Pause=rr::game::OriginalPauseStorage50LA;
    const auto retained=pauseColdOwner50LA_.lock();
    if(retained.get()!=&owner||owner.stage_!=Pause::Stage::derivedResources||
       !owner.entity_||owner.entity_->application!=entitiesOwner_||
       owner.domain_!=standardMaterialDomainOwner_||owner.context_!=materialContextOwner_||
       !owner.context_||!owner.context_->live()||owner.references_.failed()||
       owner.references_.associatedCount()!=rr::assets::OriginalPausePrimitiveReferences50LA::count)
        return false;
    // Source1A8AC8 full load precedes this explicit caller-side slot1C boundary.
    // It is only the detached concrete disable, never proof of whole race load.
    return owner.entity_->disable1A8ADC();
}
}

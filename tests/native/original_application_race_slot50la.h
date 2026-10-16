#pragma once
#include "original_race_constructor50la.h"

namespace original_main_entry50KG {
inline bool Application::publishConsumedRace28C(
    const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& owner)const noexcept {
    if(!owner||sourceRace28COwner_||raceConstructorColdOwner50LA_.lock()!=owner||
       owner->application_!=entitiesOwner_||owner->profile_!=riderProfile||
       !owner->canPublish28C())return false;
    //17D680 (alternate17C17C): publish the SAME successful constructor result
    // BEFORE its slot30 load. This does not attach the later sibling chain.
    sourceRace28COwner_=owner;
    return true;
}
inline bool Application::clearNativeRetiredRace28C(rr::game::OriginalRaceConstructor50LA& owner)const noexcept {
    if(sourceRace28COwner_&&sourceRace28COwner_.get()!=&owner)return false;
    if(!owner.rolledBack_||owner.entity_.reference00||owner.entity_.parent1C||
       owner.entity_.previous10||owner.entity_.next14||owner.entity_.firstChild18||
       owner.pause100_||owner.nativeRootAttached_||owner.world74_||owner.raceAudio_||owner.raceGroupCount_||owner.loadDisplay_||
       owner.loadFifo_||owner.resourceContext_||owner.resourceDomain_||owner.profile_)return false;
    //17D904 clears the source slot AFTER release. Here only the explicitly
    // completed native retirement authorizes that store; no1A6840 claim.
    sourceRace28COwner_.reset();
    return true;
}
}

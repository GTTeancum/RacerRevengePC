#pragma once
#include "original_application_race_slot50la.h"
#include "original_native_collector_fifo50la.h"
#include "../../src/game/original_racer_model_view50la.h"

namespace original_main_entry50KG {
inline bool Application::beginConsumedRaceLoad1A6AA0(rr::game::OriginalRaceConstructor50LA& owner,
    rr::game::OriginalRacerModelRenderGlobals50LA& actualModelGlobals,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=rr::assets::OriginalStandardMaterialStatus50LA;
    d={};d.sourcePC=0x1a6aa0;
    const auto display=raceDisplayOwner50LA_.lock();
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
    if(sourceRace28COwner_.get()!=&owner||raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!owner.detachedPrefix()||!owner.constructorSuffix_||
       owner.loadPrefixAttempted_||owner.resourcesAttempted_||owner.raceGroupsAttempted_||
       !riderProfile||!riderProfile->courseRaceSnapshot(selected)||
       !display||display->application_!=this||display->video_!=videoOwner_||
       !RaceDisplayLifetime50LA::uploadBindingLive(*display)||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       nativeCollectorFifoOwner_){d.status=S::phase;return false;}
    // Allocate unpublished native backing first. No original state changes
    // occur if allocation fails; this is not a guest allocation/DMA pool.
    std::shared_ptr<NativeCollectorFifo50LA> fifo;
    try {fifo=std::shared_ptr<NativeCollectorFifo50LA>(new NativeCollectorFifo50LA(
        *this,standardMaterialDomainOwner_->materials382B38()));}
    catch(...){d.status=S::allocation;d.sourcePC=0x1a6b30;return false;}
    owner.loadPrefixAttempted_=true;
    sourceApplicationMode22C_=1; //1A6AC8, SAME Application scalar.
    display->beginRaceClear1A6AD8(); //141C20 itself is an identity-return leaf.
    actualModelGlobals.setRaceDistance2410E0(selected.subtype); //1A6B0C or1A6B24.
    sourceSecondMapDistance382FA0_=actualModelGlobals.secondMapDistance382FA0(); //Same reached process store, preserved across later Garage entries.
    nativeCollectorFifoOwner_=fifo; //1A6B40->24B6E0->29C270, actual443848 slot.
    owner.loadDisplay_=display;owner.loadFifo_=std::move(fifo);
    owner.loadPrefixReached_=true;d.sourceEffectsCommitted=true;d.sourcePC=0x1a6b4c;return true;
}
inline bool Application::rollbackNativeRaceLoadPrefix(rr::game::OriginalRaceConstructor50LA& owner,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=rr::assets::OriginalStandardMaterialStatus50LA;
    d={};d.sourcePC=0x1a6aa0;
    if(raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||owner.world74_||owner.raceAudio_||owner.raceGroupCount_||
       !owner.detachedPrefix(true)) {d.status=S::phase;d.field="detached Race load-prefix retirement";return false;}
    if(!owner.loadPrefixReached_)return !owner.loadDisplay_&&!owner.loadFifo_;
    if(!owner.loadDisplay_||!owner.loadFifo_||nativeCollectorFifoOwner_!=owner.loadFifo_||
       owner.loadDisplay_->application_!=this||!owner.loadDisplay_->ownerThread()){
        d.status=S::phase;return false;
    }
    // Only the actual acquired empty FIFO retires. This checks its real queue,
    // canonical pins and thread; it never fabricates submission/drain completion.
    if(!retireNativeCollector1A6948(d))return false;
    owner.loadFifo_.reset();owner.loadDisplay_.reset();owner.loadPrefixReached_=false;
    // Reached Application22C/renderer2C/distance writes are NOT rolled back.
    // This is explicit native failure cleanup, not normal1A6840 completion.
    return true;
}
}

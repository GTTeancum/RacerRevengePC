#pragma once
#include "original_race_world_join50la.h"
#include "original_race_load_prefix50la.h"
#include "original_race_pause_parent50la.h"

namespace rr::game {
inline bool OriginalRaceConstructor50LA::retireNativeWorldAndGroups(std::uint32_t& pc,const char*& gate)noexcept {
    pc=0;
    // The scheduler must actually detach this owner first. Only the exact
    // represented six-child chain is admitted; unknown children are not skipped.
    gate="race detached identity/flags/children";
    if(!counted_||rolledBack_||!application_||application_->attachmentLock423F58()||
       entity_.reference00!=1||entity_.parent1C||entity_.previous10||entity_.next14||
       entity_.mask20!=(nativeRootMaskReached_?0xffffffffu:0u)||(entity_.flags29&15u)!=3u||
       !application_->entityAuthority_.canRetire(entity_)||raceGroupCount_>6||
       raceGroupsAttached_>raceGroupCount_||
       entity_.firstChild18!=(raceGroupsAttached_?raceGroups_[0]:nullptr))return false;
    if(raceAudio_&&(raceGroupCount_||raceAudio_->coldStarted_||world74_)){
        auto& audio=*raceAudio_;
        gate="race audio canonical identity/epoch/thread";
        if(audio.coldApplication_!=application_||audio.coldSession_!=entity_.sessionGeneration||
           audio.coldThread_!=std::this_thread::get_id()||!audio.coldMixer_||
           audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC())return false;
        for(std::size_t i=0;i<raceGroupCount_;++i){
            const auto* child=raceGroups_[i];
            gate="race audio child authority/reference";
            if(!child||child!=&audio.coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity||
               child->reference00!=1||child->firstChild18||
               !application_->entityAuthority_.canRetire(*child))return false;
            if(i<raceGroupsAttached_){
                gate="race audio attached child chain";
                if(child->parent1C!=&entity_||child->previous10!=(i?raceGroups_[i-1]:nullptr)||
                   child->next14!=(i+1<raceGroupsAttached_?raceGroups_[i+1]:nullptr))return false;
            }else if(child->parent1C||child->previous10||child->next14){gate="race audio detached child links";return false;}
        }
        // Never stop live voices to manufacture empty group admission. Partial
        // provider preparation is handled by the same actual membership ledger.
        gate="native stopped voice membership retirement";
        if(audio.mixer_&&!audio.retireNativeVoiceBindings())return false;
    }else if(raceGroupCount_||world74_){gate="missing audio owner with acquired race children";return false;}
    if(world74_){
        // Actual aggregate borrows, renderer publication, runtime topology and
        // resource gates remain authoritative. Failure retains this same owner.
        gate="actual aggregate native loaded-world retirement";
        if(!world74_->closeNativeLoadedWorld())return false;
        world74_.reset();
    }
    // This existing native unwind kernel performs exact canonical detach and
    // actual acquired empty-group release, with retryable partial progress.
    gate="six race group detach/release";
    return rollbackNativeRaceGroupsFailure(pc);
}
}
namespace original_main_entry50KG {
inline bool Application::closeNativeConsumedRace(rr::game::OriginalRaceConstructor50LA& owner,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=rr::assets::OriginalStandardMaterialStatus50LA;
    d={};
    // Keep the actual slot-backed owner alive even if the caller borrowed it.
    const auto publishedOwner=sourceRace28COwner_;
    if((publishedOwner&&publishedOwner.get()!=&owner)||
       raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_){
        d.status=S::phase;d.field="consumed race actual Application identity";return false;
    }
    if(owner.rolledBack_){
        if(!clearNativeRetiredRace28C(owner)){d.status=S::phase;d.field="clear actually retired Application28C";return false;}
        return true;
    }
    if(owner.profile_!=riderProfile||
       (owner.loadDisplay_&&!owner.loadDisplay_->ownerThread())){d.status=S::phase;d.field="consumed race profile/display thread";return false;}
    std::uint32_t pc=0;
    const char* gate=nullptr;
    if(!owner.retireNativePauseParent(gate)){d.status=S::phase;d.field=gate;return false;}
    if(!owner.retireNativeWorldAndGroups(pc,gate)){d.status=S::phase;d.sourcePC=pc;d.field=gate;return false;}
    // Reuse real FIFO retirement and backing release. These kernels are native
    // ownership cleanup; this entry does not certify original1A6840 callbacks.
    if(!rollbackNativeRaceLoadPrefix(owner,d)){
        if(!d.field)d.field="race load FIFO/display native retirement";return false;
    }
    if(!owner.rollbackNativeConstructionFailure(pc)){d.status=S::phase;d.sourcePC=pc;d.field="race constructor native backing/count retirement";return false;}
    if(!clearNativeRetiredRace28C(owner)){d.status=S::phase;d.field="clear actually retired Application28C";return false;}
    d={};return true;
}
}

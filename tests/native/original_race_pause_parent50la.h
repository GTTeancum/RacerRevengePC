#pragma once
#include "original_race_display_lifetime50la.h"
#include "original_race_world_join50la.h"
#include "original_pause_entity50la.h"

namespace rr::game {
inline bool OriginalRaceConstructor50LA::canRetainPause100()const noexcept {
    return counted_&&!rolledBack_&&!pause100_&&world74_&&world74_->worldColdConstructed_&&
        world74_->worldApplication_==application_&&world74_->worldAudio_==raceAudio_&&
        world74_->worldEntity_.sessionGeneration==entity_.sessionGeneration&&
        loadPrefixReached_&&resourcesPrepared_&&raceGroupsAttached_==6;
}
inline bool OriginalRaceConstructor50LA::attachNativePauseParent()noexcept {
    if(!pause100_||!pause100_->entity_||!counted_||rolledBack_||nativeRootAttached_||
       nativeRootMaskReached_||!application_||application_->attachmentLock423F58()||
       !application_->application10()||entity_.parent1C||entity_.previous10||entity_.next14||
       entity_.mask20||raceGroupCount_!=6||raceGroupsAttached_!=6||
       !application_->entityAuthority_.canRetire(entity_))return false;
    auto& p=*pause100_->entity_;
    if(p.application!=application_||p.component.sessionGeneration!=entity_.sessionGeneration||
       !p.detached()||!p.disabledAfterLoad||p.retainedAttachmentMask20||
       !application_->entityAuthority_.canRetire(p.component))return false;
    const auto lock=application_->attachmentLock423F58();
    //1A7818 builds the actual sibling chain before17D6A0 attaches it.
    if(application_->entityAuthority_.appendSibling(entity_,p.component,0x1402u,lock).status!=
       OriginalWreckComponentStatus50LA::complete)return false;
    p.retainedAttachmentMask20=0x1402u;
    if(application_->entityAuthority_.attachChildren(*application_->application10(),entity_,0xffffffffu,lock).status!=
       OriginalWreckComponentStatus50LA::complete)return false;
    nativeRootMaskReached_=nativeRootAttached_=true;return true;
}
inline bool OriginalRaceConstructor50LA::retireNativePauseParent()noexcept {
    if(!pause100_)return !nativeRootAttached_;
    if(!application_||application_->attachmentLock423F58()||!pause100_->entity_||
       pause100_->materialAdmissionBorrows_)return false;
    auto& p=*pause100_->entity_;auto& e=p.component;
    if(p.application!=application_||e.sessionGeneration!=entity_.sessionGeneration||
       e.firstChild18||e.descendants24||e.mask20!=p.retainedAttachmentMask20||
       (e.flags29&15u)!=(p.disabledAfterLoad?0u:3u)||
       !application_->entityAuthority_.canRetire(e)||
       !application_->entityAuthority_.canRetire(entity_)||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view()))return false;
    if(nativeRootAttached_){
        if(entity_.parent1C!=application_->application10()||entity_.next14!=&e||
           e.parent1C!=entity_.parent1C||e.previous10!=&entity_||e.next14||
           entity_.mask20!=0xffffffffu||!nativeRootMaskReached_)return false;
        //17D8E8 detaches race first; its concrete child cleanup then detaches Pause.
        if(application_->entityAuthority_.detach(entity_,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
        nativeRootAttached_=false;
    }else if(entity_.parent1C||entity_.previous10)return false;
    // Also admits the acquired sibling-only state if root attachment failed.
    if(e.parent1C||e.previous10||e.next14){
        if(e.next14)return false;
        if(application_->entityAuthority_.detach(e,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
    }
    if(!p.detached())return false;
    pause100_->closeCold();pause100_.reset();return true;
}
}
namespace original_main_entry50KG {
inline std::shared_ptr<rr::game::OriginalPauseStorage50LA> Application::constructConsumedRacePause1A8AB0(
    rr::game::OriginalRaceConstructor50LA& owner)const noexcept {
    if(sourceRace28COwner_.get()!=&owner||owner.application_!=entitiesOwner_||owner.profile_!=riderProfile||
       !owner.canRetainPause100())return {};
    auto pause=constructPauseCold1A8AB0(owner.entity_.sessionGeneration);
    if(!pause)return {};
    owner.pause100_=pause; //Actual1A8AB8 publication before caller invokes Pause load.
    return pause;
}
inline bool Application::attachNativeConsumedRacePause(rr::game::OriginalRaceConstructor50LA& owner,
    const std::shared_ptr<original_camera_session50KX::Session>& camera)const noexcept {
    using P=rr::game::OriginalPauseStorage50LA;
    if(sourceRace28COwner_.get()!=&owner||owner.application_!=entitiesOwner_||owner.profile_!=riderProfile||
       !camera||!owner.world74_||!owner.pause100_||pauseColdOwner50LA_.lock()!=owner.pause100_||
       owner.pause100_->stage_!=P::Stage::derivedResources||!owner.pause100_->entity_||
       !owner.pause100_->entity_->disabledAfterLoad||owner.pause100_->references_.failed()||
       owner.pause100_->references_.associatedCount()!=rr::assets::OriginalPausePrimitiveReferences50LA::count||
       owner.pause100_->domain_!=standardMaterialDomainOwner_||owner.pause100_->context_!=materialContextOwner_||
       !materialContextOwner_||!materialContextOwner_->live()||!owner.loadDisplay_||
       raceDisplayOwner50LA_.lock()!=owner.loadDisplay_||!RaceDisplayLifetime50LA::uploadBindingLive(*owner.loadDisplay_))return false;
    const auto prefix=owner.world74_->completedPrefixes();
    if(!prefix.staticWorldDrawPlan||!prefix.staticWorldImages||!prefix.staticModelObservers||
       !prefix.runtimeQueryWorldOwners||!prefix.trackParticleManager)return false;
    ra_cold_aggregate50LA::Diagnostic d;
    const auto actualCamera=owner.world74_->borrowRuntimeSourceCamera(owner.world74_,d);
    if(!actualCamera||actualCamera.get()!=camera.get())return false;
    // Actual prepared native camera/bank and loaded draw resources admit this
    // native route. They are not a receipt for unrepresented original slot30.
    return owner.attachNativePauseParent();
}
}

#include "terrain_interactive_publication50du.h"
#include "original_consumed_ra_stage50du.h"
#include "original_consumed_ta_stage50du.h"
#include "original_race_constructor50la.h"
#include "original_race_hud_scope50la.h"
#include "original_position_stats_adapter50la.h"
#include "original_race_display_lifetime50la.h"
#include "ra_cold_aggregate50la.h"
#include "../../src/game/original_pause_storage50la.h"

namespace original_main_entry50KG {
bool Application::publishNativeHumanResultsRoot50LA(::interactive_publication50DU::Context& actual,
    rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {
    d={};const auto reject=[&](const char* why)noexcept {
        d.status=rr::game::OriginalResults104Diagnostic50LA::Status::owner;
        d.sourcePC=0;d.operation=why;return false;
    };
    // Native accepted-world policy, NOT a receipt for full original slot30 or
    //17D6A0. Only the actual Publication calls after its successful HUD bind.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject("Native human root requires Application owner thread");
    if(&actual.originalMainApplication!=this||(!actual.sourceConsumedIntroActivation&&!actual.consumedTaStage.race())||
       !actual.humanCourseEnabled)return reject("Native human root requires an actual accepted human race Publication");
    try {
        // The same root for every race (1A8A40/17D6A0 are common). RA keeps
        // its world join; TA uses its own counted world and camera (the
        // other courses join once their race camera link exists).
        const bool raRace=bool(actual.consumedRaStage.race());
        const auto& selected=raRace?actual.consumedRaStage.race():actual.consumedTaStage.race();
        const auto race=sourceRace28COwner_;
        const auto camera=actual.sourceCameraSession;const auto display=raceDisplayOwner50LA_.lock();
        const bool worldCurrent=raRace?actual.raWorld&&race&&race->world74_==actual.raWorld:
            race&&!race->world74_&&actual.taOriginalWorld&&race->taWorld74_==actual.taOriginalWorld;
        if(!race||race!=selected||raceConstructorColdOwner50LA_.lock()!=race||
           race->resultsFeedbackThread_!=std::this_thread::get_id()||race->profile_!=riderProfile||
           !riderProfile||race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||
           !worldCurrent||!camera||
           race->cameraNativeAdapter34_!=camera.get()||!race->ownsNativeCameraAdapter34(*camera)||
           race->cameraInitStep_!=rr::game::OriginalRaceConstructor50LA::CameraInitStep::nativeReady||
           !display||display!=race->loadDisplay_||display->application_!=this||display->video_!=videoOwner_||
           &display->renderer_!=&actual.renderer||!display->gameplayDisplayIdentityCurrent50LA()||
           !RaceDisplayLifetime50LA::uploadBindingLive(*display)||
           display->selectedGameplayCamera0_!=camera.get()||camera->selectedGameplayDisplay_.lock()!=display)
            return reject("Native human root actual Race/Profile/world/selected Camera/Display custody absent");
        if(raRace){
            // RA: the camera is the world join's own retained Session.
            ra_cold_aggregate50LA::Diagnostic worldDiagnostic;
            const auto borrowed=actual.raWorld->borrowRuntimeSourceCamera(actual.raWorld,worldDiagnostic);
            const auto prefix=actual.raWorld->completedPrefixes();
            if(!borrowed||borrowed.get()!=camera.get()||!prefix.staticWorldDrawPlan||!prefix.staticWorldImages||
               !prefix.staticModelObservers||!prefix.runtimeQueryWorldOwners||!prefix.trackParticleManager)
                return reject("Native human root requires the RA world's own prepared Camera");
        }
        const auto course=raRace?rr::game::OriginalCameraStartCourse::ra:rr::game::OriginalCameraStartCourse::ta;
        if(camera->closed_||camera->abandoned_||!camera->coldBound_||!camera->readsBound_||
           !camera->nativeTaIssuance_||camera->nativeTaIssuance_->issuer!=this||
           camera->nativeTaIssuance_->course!=course||
           camera->nativeTaIssuance_->generation!=race->entity_.sessionGeneration||
           camera->generation_!=race->entity_.sessionGeneration||camera->actual_.applicationEntities!=entitiesOwner_||
           &camera->actual_.bank!=&actual.racerBank||&camera->actual_.all8Manager!=&actual.originalAiSession.manager||
           camera->actual_.raceHud!=&actual.sourceRaceHud||camera->actual_.humanCount34!=1u||
           !camera->viewport_.prepared||camera->viewport_.sessionGeneration!=camera->generation_||
           !actual.sourceRaceHud.loaded()||actual.sourceRaceHud.viewCount()!=1u)
            return reject("Native human root requires prepared same-Operation Camera/Bank/HUD owners");
        const auto hud=race->endHud_.lock();
        if(!hud||hud->thread_!=std::this_thread::get_id()||!hud->active_||!hud->published_||
           hud->application_!=this||hud->race_.lock()!=race||hud->camera_!=camera.get()||
           hud->bank_!=&actual.racerBank||hud->manager_!=&actual.originalAiSession||
           hud->hud_!=&actual.sourceRaceHud||hud->generation_!=race->entity_.sessionGeneration)
            return reject("Native human root actual lexical successful HUD publication absent");
        using P=rr::game::OriginalPauseStorage50LA;
        const auto& pause=race->pause100_;
        if(!pause||pauseColdOwner50LA_.lock()!=pause||pause->stage_!=P::Stage::derivedResources||
           !pause->entity_||!pause->entity_->disabledAfterLoad||pause->references_.failed()||
           pause->references_.associatedCount()!=rr::assets::OriginalPausePrimitiveReferences50LA::count||
           pause->domain_!=standardMaterialDomainOwner_||pause->context_!=materialContextOwner_||
           !materialContextOwner_||!materialContextOwner_->live()||!race->results104_||
           !race->results104_->loadedDisabled())
            return reject("Native human root needs actual loaded disabled Pause/Results resources");
        original_position_stats_adapter50LA::Owner observer;
        original_position_stats_adapter50LA::Owner::Diagnostic observed;
        if(!observer.bind(*camera,actual.originalAiSession,observed)||
           !race->results104_->endDispatchOwnerCurrent1AB520(*race,observer))
            return reject("Native human root Results load-issued Operation/Bank identity differs");
        // Append to the real existing root, never reorder/delete Light, Aural,
        //dispatcher or dialogs to satisfy an input guard. No source callbacks
        //are run by this effects-free topology/availability preflight.
        if(!passResultsProcessPredecessors50LA(*race,nullptr,d))return false;
        if(!race->attachNativeHumanResultsRoot50LA())
            return reject("Native human root splice rejected; retain reached prefix for ordered unwind");
        d.status=rr::game::OriginalResults104Diagnostic50LA::Status::complete;
        d.operation="Native accepted-world human root published; original load/end/update/draw still separate";
        return true;
    }catch(...){return reject("Native human root same-owner borrow unavailable");}
}
}

namespace rr::game {
bool OriginalRaceConstructor50LA::attachNativeHumanResultsRoot50LA()noexcept {
    if(resultsFeedbackThread_!=std::this_thread::get_id()||
       nativeHumanRootStep_!=NativeHumanRootStep::unreached||nativeRootAttached_||nativeRootMaskReached_||
       !counted_||rolledBack_||!application_||application_->attachmentLock423F58()||
       !application_->application10()||entity_.parent1C||entity_.previous10||entity_.next14||entity_.mask20||
       !pause100_||!pause100_->entity_||!results104_||!results104_->loadedDisabled()||
       !results104_->belongsTo(application_,entity_.sessionGeneration)||!results104_->retirementCurrent()||
       !f8Owner_||f8Source_!=f8Owner_.get()||!f8Attached_||!f8Owner_->loadedCold()||
       !f8Owner_->belongsTo(application_,entity_.sessionGeneration)||f8Owner_->nativeBarBorrowed()||
       raceGroupCount_!=6||raceGroupsAttached_!=6||entity_.firstChild18!=raceGroups_[0]||
       !application_->entityAuthority_.canRetire(entity_))return false;
    auto& p=*pause100_->entity_;auto* r=results104_->entity();auto* f=f8Owner_->entity();
    if(p.application!=application_||p.component.sessionGeneration!=entity_.sessionGeneration||
       !p.detached()||!p.disabledAfterLoad||p.retainedAttachmentMask20||
       !application_->entityAuthority_.canRetire(p.component)||!r||r->parent1C||r->previous10||r->next14||
       r->mask20!=0xffffffdfu||!f||f->parent1C!=&entity_||f->previous10!=raceGroups_[5]||f->next14||
       f->firstChild18||f->descendants24||f->mask20!=0xffffffffu)return false;
    for(std::size_t i=0;i<6;++i){const auto* c=raceGroups_[i];
        if(!c||c->parent1C!=&entity_||c->previous10!=(i?raceGroups_[i-1]:nullptr)||
           c->next14!=(i+1<6?raceGroups_[i+1]:f))return false;
    }
    // Exact source chain/masks:1A7818 Pause then1A7830 Results. The decision
    //to publish NOW is explicitly native accepted-world policy, not17D6A0.
    nativeHumanRootStep_=NativeHumanRootStep::appending;
    auto& authority=application_->entityAuthority_;
    if(authority.appendSibling(entity_,p.component,0x1402u,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
    p.retainedAttachmentMask20=0x1402u;
    if(authority.appendSibling(entity_,*r,0xfffffff7u,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
    if(!results104_->attachedMask(0xfffffff7u))return false;
    if(authority.attachChildren(*application_->application10(),entity_,0xffffffffu,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
    nativeRootMaskReached_=nativeRootAttached_=true;
    nativeHumanRootStep_=NativeHumanRootStep::published;return true;
}

bool OriginalRaceConstructor50LA::detachNativeHumanResultsRoot50LA(const char*& gate)noexcept {
    gate="native human root retirement affinity";
    if(resultsFeedbackThread_!=std::this_thread::get_id())return false;
    // Preserve existing spectator/legacy behavior exactly. No human prefix
    //was acquired, so the existing Pause cleanup remains its sole detacher.
    if(nativeHumanRootStep_==NativeHumanRootStep::unreached)return true;
    if(nativeHumanRootStep_==NativeHumanRootStep::detached)
        return !nativeRootAttached_&&!entity_.parent1C&&!entity_.previous10&&!entity_.next14;
    gate="native human root live borrows/actual forest";
    if(!counted_||rolledBack_||!application_||application_->attachmentLock423F58()||
       !endHud_.expired()||!resultsFeedback_.expired()||
       (f8Owner_&&f8Owner_->nativeBarBorrowed())||
       !application_->entityAuthority_.canRetire(entity_)||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view()))return false;
    auto* p=pause100_?pause100_->entity_.get():nullptr;
    auto* r=results104_?results104_->entity():nullptr;
    gate="native human root reached owned UI prefix";
    if(!p||!r||!results104_->retirementCurrent()||
       !results104_->belongsTo(application_,entity_.sessionGeneration)||
       p->application!=application_||p->component.sessionGeneration!=entity_.sessionGeneration||
       p->component.mask20!=p->retainedAttachmentMask20||
       !application_->entityAuthority_.canRetire(p->component))return false;
    auto& pe=p->component;auto* root=application_->application10();
    if(nativeHumanRootStep_==NativeHumanRootStep::published){
        if(!nativeRootAttached_||!nativeRootMaskReached_||entity_.parent1C!=root||
           entity_.mask20!=0xffffffffu||entity_.next14!=&pe||pe.parent1C!=root||
           pe.previous10!=&entity_||pe.next14!=r||r->parent1C!=root||r->previous10!=&pe||r->next14)
            return false;
    }else{
        // Failed append forms: no append, Race->Pause, Race->Pause->Results.
        // Retained masks distinguish the exact reached steps; never detach an
        //arbitrary caller-provided sibling chain or reinterpret cold absence.
        if(nativeRootAttached_||nativeRootMaskReached_||entity_.parent1C||entity_.previous10||entity_.mask20||
           pe.parent1C||r->parent1C||r->next14)return false;
        if(!entity_.next14){
            if(pe.previous10||pe.next14||r->previous10||p->retainedAttachmentMask20||r->mask20!=0xffffffdfu)return false;
        }else if(entity_.next14!=&pe||pe.previous10!=&entity_||p->retainedAttachmentMask20!=0x1402u||
                 (pe.next14&&pe.next14!=r)||
                 (pe.next14==r?r->previous10!=&pe:r->previous10!=nullptr))return false;
    }
    gate="detach actual native human Race before F8 child release";
    //141A70 also supports the parentless sibling-only partial prefix. It
    //leaves both UI owners, child F8, masks and reference counts intact.
    if(entity_.parent1C||entity_.previous10||entity_.next14)
        if(application_->entityAuthority_.detach(entity_,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
    nativeRootAttached_=false;nativeHumanRootStep_=NativeHumanRootStep::detached;
    return true;
}
}

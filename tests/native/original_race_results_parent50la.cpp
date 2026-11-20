#include "original_race_constructor50la.h"
#include "../../src/game/original_pause_storage50la.h"
#include "original_race_world_join50la.h"
namespace rr::game {
bool OriginalRaceConstructor50LA::canRetainPause100()const noexcept {
    // 1A8AB0 runs inside every race's load: the race's one World (the same
    // 1A6BC0 step on every course), counted, prefix complete, same generation.
    const bool world=nativeIntroWorldEntity()!=nullptr;
    return counted_&&!rolledBack_&&!pause100_&&!results104_&&world&&
        loadPrefixReached_&&resourcesPrepared_&&raceGroupsAttached_==6;
}
bool OriginalRaceConstructor50LA::attachNativePauseParent()noexcept {
    //This is the spectator live Pause callback forest, not human intro/root
    //publication. Cold F8 has no live scheduler/renderer join; do not admit it.
    if(!pause100_||!pause100_->entity_||!counted_||rolledBack_||nativeRootAttached_||
       nativeRootMaskReached_||f8Owner_||f8Attached_||!application_||application_->attachmentLock423F58()||
       !application_->application10()||entity_.parent1C||entity_.previous10||entity_.next14||
       entity_.mask20||raceGroupCount_!=6||raceGroupsAttached_!=6||
       !application_->entityAuthority_.canRetire(entity_))return false;
    auto& p=*pause100_->entity_;
    if(p.application!=application_||p.component.sessionGeneration!=entity_.sessionGeneration||
       !p.detached()||!p.disabledAfterLoad||p.retainedAttachmentMask20||
       !application_->entityAuthority_.canRetire(p.component))return false;
    auto* r=results104_?results104_->entity():nullptr;
    if(results104_&&(!r||!results104_->loadedDisabled()||
       !results104_->belongsTo(application_,entity_.sessionGeneration)||
       !results104_->retirementCurrent()||r->parent1C||r->previous10||r->next14||
       r->mask20!=0xffffffdfu))return false;
    const auto lock=application_->attachmentLock423F58();
    //1A7818 then1A7830 construct the real sibling chain, before17D6A0
    //attaches its head. Neither ScreenObjects nor a second Pause are Entities.
    if(application_->entityAuthority_.appendSibling(entity_,p.component,0x1402u,lock).status!=
       OriginalWreckComponentStatus50LA::complete)return false;
    p.retainedAttachmentMask20=0x1402u;
    if(r){
        if(application_->entityAuthority_.appendSibling(entity_,*r,0xfffffff7u,lock).status!=
           OriginalWreckComponentStatus50LA::complete)return false;
        if(!results104_->attachedMask(0xfffffff7u))return false;
    }
    if(application_->entityAuthority_.attachChildren(*application_->application10(),entity_,0xffffffffu,lock).status!=
       OriginalWreckComponentStatus50LA::complete)return false;
    nativeRootMaskReached_=nativeRootAttached_=true;return true;
}
bool OriginalRaceConstructor50LA::retireNativePauseParent(const char*& gate)noexcept {
    gate="Race UI absent with native root attachment";
    if(!pause100_&&!results104_)return !nativeRootAttached_;
    gate="Race UI retirement Application/lock/forest";
    if(!application_||application_->attachmentLock423F58()||
       !application_->entityAuthority_.canRetire(entity_)||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view()))return false;
    auto* p=pause100_?pause100_->entity_.get():nullptr;
    auto* r=results104_?results104_->entity():nullptr;
    if(pause100_){
        gate="Pause canonical identity/flags/mask/resource borrows";
        if(!p||pause100_->materialAdmissionBorrows_||p->application!=application_||
           p->component.sessionGeneration!=entity_.sessionGeneration||
           p->component.firstChild18||p->component.descendants24||
           p->component.mask20!=p->retainedAttachmentMask20||
           (p->component.flags29&15u)!=(p->disabledAfterLoad?0u:3u)||
           !application_->entityAuthority_.canRetire(p->component))return false;
    }
    if(results104_){
        gate="Results canonical known-prefix/identity/flags/mask";
        if(!results104_->retirementCurrent()){
            // Only this same Race may resume its already-detached post104970
            // resource destructor. entity() intentionally remains null; do
            // not re-enroll, re-detach, or expose it to any live traversal.
            if(r||!results104_->detachedRetirementCurrent(*this))return false;
        }else if(r&&!results104_->belongsTo(application_,entity_.sessionGeneration))return false;
    }
    if(!nativeRootAttached_){
        // Only the two reached partial forms are admitted: detached cold UI,
        // or the exact sibling-only append prefix. A parented remainder can
        // exist solely after this owner's earlier successful root attachment.
        gate="Race UI partial-prefix attachment provenance";
        if(entity_.parent1C||entity_.previous10)return false;
        if(p){const auto& e=p->component;
            if(e.parent1C){if(!nativeRootMaskReached_||e.parent1C!=application_->application10())return false;}
            else if(e.previous10&&e.previous10!=&entity_)return false;
            if(e.previous10==&entity_&&entity_.next14!=&e)return false;
        }
        if(r){
            if(r->parent1C){if(!nativeRootMaskReached_||r->parent1C!=application_->application10())return false;}
            else if(r->previous10&&r->previous10!=&entity_&&(!p||r->previous10!=&p->component))return false;
        }
        if(entity_.next14&&entity_.next14!=(p?&p->component:r))return false;
    }
    if(nativeRootAttached_){
        gate="Race exact Pause100/Results104 sibling chain";
        if(!p||entity_.parent1C!=application_->application10()||entity_.next14!=&p->component||
           p->component.parent1C!=entity_.parent1C||p->component.previous10!=&entity_||
           p->component.next14!=r||entity_.mask20!=0xffffffffu||!nativeRootMaskReached_||
           (r&&(r->parent1C!=entity_.parent1C||r->previous10!=&p->component||r->next14)))return false;
        //17D8E8 removes the actual Race first. The known UI siblings remain
        //on their real parent until their individual1A695C/1A6988 calls.
        gate="detach actual Race root";
        if(application_->entityAuthority_.detach(entity_,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
        nativeRootAttached_=false;
    }else if(entity_.parent1C||entity_.previous10){gate="Race root retains parent/previous";return false;}
    //1A6950..6978: Pause100 detach/release/store-null BEFORE Results104.
    //Also drains the acquired sibling-only prefix if parent attachment failed.
    if(p){
        auto& e=p->component;
        gate="Pause unexpected sibling/parent";
        if((e.next14&&e.next14!=r)||(e.parent1C&&e.parent1C!=application_->application10())||
           (r&&e.next14==r&&(r->previous10!=&e||r->parent1C!=e.parent1C)))return false;
        if(e.parent1C||e.previous10||e.next14){
            gate="detach actual Pause100";
            if(application_->entityAuthority_.detach(e,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
        }
        gate="Pause detached prefix";if(!p->detached())return false;
        pause100_->closeCold();pause100_.reset();
    }
    //1A697C..69A4: then Results104. Partial HOST construction/load cleanup
    //drains only known custody, never pretending1BEC40 could read unwritten slots.
    if(results104_){
        gate="Results unexpected sibling/parent";
        if(r&&(r->next14||(r->parent1C&&r->parent1C!=application_->application10())))return false;
        if(r&&(r->parent1C||r->previous10||r->next14)){
            gate="detach actual Results104";
            if(application_->entityAuthority_.detach(*r,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
        }
        gate="Results detached known-prefix retirement";
        OriginalResults104Diagnostic50LA d;
        if(!results104_->closeDetached(d))return false;
        results104_.reset();
    }
    gate="Race retains unexpected UI sibling";return !entity_.next14;
}
}

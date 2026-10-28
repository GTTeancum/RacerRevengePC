#include "original_race_constructor50la.h"
namespace rr::game {
bool OriginalRaceConstructor50LA::attachF8Cold1AAF54()noexcept {
    if(!f8Owner_||f8Source_!=f8Owner_.get()||f8Attached_||!f8Owner_->loadedCold()||
       !f8Owner_->belongsTo(application_,entity_.sessionGeneration)||!counted_||rolledBack_||
       nativeRootAttached_||nativeRootMaskReached_||!application_||application_->attachmentLock423F58()||
       entity_.parent1C||entity_.previous10||entity_.next14||entity_.mask20||
       raceGroupCount_!=6||raceGroupsAttached_!=6||entity_.firstChild18!=raceGroups_[0]||
       !application_->entityAuthority_.canRetire(entity_))return false;
    //The retained cold Race has exactly its six real audio children. Keep
    //their identities/order, append this later source child after them; do not
    //invent absent HUD/camera children or certify the whole1AAD40 dispatcher.
    for(std::size_t i=0;i<6;++i){const auto* c=raceGroups_[i];
        if(!c||c->parent1C!=&entity_||c->previous10!=(i?raceGroups_[i-1]:nullptr)||
           c->next14!=(i+1<6?raceGroups_[i+1]:nullptr))return false;
    }
    auto* f=f8Owner_->entity();
    if(!f||f->parent1C||f->previous10||f->next14||f->firstChild18||f->descendants24||f->mask20||
       !application_->entityAuthority_.canRetire(*f))return false;
    if(application_->entityAuthority_.attachChildren(entity_,*f,0xffffffffu,
            application_->attachmentLock423F58()).status!=OriginalWreckComponentStatus50LA::complete)return false;
    f8Attached_=true;return f8Owner_->attachedMask(0xffffffffu);
}
bool OriginalRaceConstructor50LA::retireF8Cold(const char*& gate)noexcept {
    if(!f8Owner_)return !f8Attached_;
    gate="F8 cold Race/Application ownership";
    if(f8Owner_->nativeBarBorrowed()){gate="F8 actual native Frame borrow blocks child detach";return false;}
    if(!counted_||rolledBack_||!application_||application_->attachmentLock423F58()||
       nativeRootAttached_||entity_.parent1C||entity_.previous10||entity_.next14||
       !application_->entityAuthority_.canRetire(entity_))return false;
    auto* f=f8Owner_->entity();
    if(f&&!f8Owner_->belongsTo(application_,entity_.sessionGeneration))return false;
    if(f8Attached_){
        gate="F8 exact cold child tail before Race destruction";
        if(f8Source_!=f8Owner_.get()||!f||raceGroupCount_!=6||raceGroupsAttached_!=6||
           entity_.firstChild18!=raceGroups_[0]||f->parent1C!=&entity_||
           f->previous10!=raceGroups_[5]||f->next14||f->firstChild18||f->descendants24||
           f->mask20!=0xffffffffu||!raceGroups_[5]||raceGroups_[5]->next14!=f)return false;
        for(std::size_t i=0;i<6;++i){const auto* c=raceGroups_[i];
            if(!c||c->parent1C!=&entity_||c->previous10!=(i?raceGroups_[i-1]:nullptr)||
               c->next14!=(i+1<6?raceGroups_[i+1]:f))return false;
        }
        //Known actual tail release is first in the represented1424E0 reverse
        //child order. Remaining existing HOST world/group cleanup stays intact.
        if(application_->entityAuthority_.detach(*f,0).status!=OriginalWreckComponentStatus50LA::complete)return false;
        f8Attached_=false;raceMaskRecomputed_=true;
    }else if(f&&(f->parent1C||f->previous10||f->next14))return false;
    gate="F8 known-acquired resource/reference/Entity retirement";
    OriginalRaceF8Diagnostic50LA d;if(!f8Owner_->closeDetached(d))return false;
    f8Owner_.reset(); //Native custody only; do not claim original F8 store-null.
    return true;
}
}

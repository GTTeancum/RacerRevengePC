#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "../../src/game/original_results104_50la.h"
#include "../../src/game/original_pause_storage50la.h"
#include <bit>

namespace original_main_entry50KG {
bool Application::updateConsumedResults1C8AE0(rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {
    d={};const auto reject=[&](std::uint32_t pc,const char* operation)noexcept {
        d.status=rr::game::OriginalResults104Diagnostic50LA::Status::owner;
        d.sourcePC=pc;d.operation=operation;return false;
    };
    // Existing immutable Application affinity comes BEFORE its mutable Race
    // slot. Checking Race affinity only after reading that slot is too late.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject(0x140ce4,"Results update requires its Application construction thread");
    const auto race=sourceRace28COwner_;
    if(!race||race->resultsFeedbackThread_!=std::this_thread::get_id())
        return reject(0x140ce4,"Results update requires its construction-thread Race owner");
    // Same actual retained140CE0/CE4 timing owner already used by the root;
    // no caller delta, raw250, second clock, timer0 or fixed-step substitute.
    if(!entitiesOwner_||nativeTaRootRace_!=race.get()||!nativeTaRootEntered_||nativeTaRootAborted_||
       sourceUpdateRenderBypass228Bit3_||sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||
       !nativeTaRootController_||race->nativeIntroControllerSlot48()!=nativeTaRootController_||
       race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||!race->results104_||
       race->entity_.sessionGeneration!=nativeTaRootGeneration_||
       !race->results104_->belongsTo(entitiesOwner_,nativeTaRootGeneration_))
        return reject(0x140ce4,"Results update requires same active consumed Root/Race/Results");
    original_ui_source_timing50LH::Sample sample;std::uint64_t publication=0;
    if(!nativeTaRootTiming_.readPrepared(sample,publication)||publication!=nativeTaRootPublication_||
       std::bit_cast<std::uint32_t>(sample.updateSeconds)!=
           std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_))
        return reject(0x140ce4,"Results update requires actual same-Root capped23C publication");
    //1A7818/1A7830 append Race -> Pause100 -> Results104 as SIBLINGS,
    // then17D6A0 attaches their head to actual Application10. Results is NOT
    // an F8-like Race child. A merely equal/null parent grants no membership.
    if(!resultsPublishedRoot50LA(*race,d))return false;
    // The ordered native suffix calls here only after its real predecessor
    // callbacks return. The exact timer prefix must reach its mandatory base
    // call before the separately labeled native device availability join.
    if(race->results104_->updateCpuPrefix1C8AE0(*sourceApplicationElapsed23C_,publication,d))return true;
    if(d.status!=rr::game::OriginalResults104Diagnostic50LA::Status::incomplete||d.sourcePC!=0x1c8c10)return false;
    if(!videoOwner_)return reject(0x1b9fe0,"Results actual owned Video host absent");
    return race->results104_->updateBaseInput1B9FE0(*race,videoOwner_->host,d);
}

bool Application::resultsPublishedRoot50LA(const rr::game::OriginalRaceConstructor50LA& owner,
    rr::game::OriginalResults104Diagnostic50LA& d,bool requireUpdateEligible)const noexcept {
    const auto reject=[&](std::uint32_t pc,const char* why)noexcept {
        d.status=rr::game::OriginalResults104Diagnostic50LA::Status::owner;
        d.sourcePC=pc;d.operation=why;return false;
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject(0x141f00,"Results root requires actual Application thread");
    const auto* race=&owner;
    if(race->resultsFeedbackThread_!=std::this_thread::get_id()||!entitiesOwner_||
       sourceRace28COwner_.get()!=race||!riderProfile||race->profile_!=riderProfile||!race->results104_||!race->counted_||race->rolledBack_||
       race->application_!=entitiesOwner_||race->entity_.sessionGeneration!=nativeTaRootGeneration_)
        return reject(0x141f00,"Results root current Race/Application owner absent");
    auto* root=entitiesOwner_->application10();
    const auto* pause=race->pause100_?race->pause100_->entity_.get():nullptr;
    const auto* results=race->results104_->entity();
    if(!root||!race->nativeRootAttached_||!race->nativeRootMaskReached_||
       entitiesOwner_->attachmentLock423F58()||!pause||!pause->initialized||pause->releasing||
       pause->application!=entitiesOwner_||pause->component.sessionGeneration!=nativeTaRootGeneration_||
       !results||race->entity_.parent1C!=root||race->entity_.mask20!=0xffffffffu||
       race->entity_.next14!=&pause->component||pause->component.parent1C!=root||
       pause->component.previous10!=&race->entity_||pause->component.next14!=results||
       pause->component.mask20!=0x1402u||pause->retainedAttachmentMask20!=0x1402u||
       results->parent1C!=root||results->previous10!=&pause->component||!resultsRequestTailCurrent50LA(*race)||
       results->mask20!=0xfffffff7u||(requireUpdateEligible&&(results->flags29&13u)!=1u)||
       results->firstChild18||results->descendants24)
        return reject(0x141f00,"Results actual published Application-root sibling update admission absent");
    const auto authority=entitiesOwner_->entityAuthority_.view();
    if(!contains(authority,root)||!contains(authority,&race->entity_)||
       !contains(authority,&pause->component)||!contains(authority,results)||
       !rr::game::original_entity_chain_detail50LA::forest(authority))
        return reject(0x141f00,"Results enrolled current Application-root forest membership absent");
    return true;
}

}

#pragma once
#include "../../src/game/original_wreck_racer_storage50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_racer_ember_owner50la.h"
#include <algorithm>
#include <exception>

namespace ra_cold_roster50LA {
class WreckBacking;
// Private native ownership around reached per-piece effect calls. This owns
// no particle manager, racer, model, global random state or alternative clock.
class NativeWreckEffectChildren final {
    friend class WreckBacking;
    using G=rr::game::OriginalWreckComponent50LA;
    using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
    std::array<std::vector<std::uint8_t>,10> bytes_; //smoke,fire,dust,splash; stable borrowed sources
    rr::game::OriginalWreckEffectResourceContext50LA context_;
    const rr::game::OriginalWreckEffectResourceContext50LA* sourceContext_=nullptr;
    G* sourceParent_=nullptr;
    G* const* sourceParentField_=nullptr;
    rr::game::OriginalTrackParticleCold50LA* sourceManager4A0_=nullptr;
    std::array<G*,rr::game::maxOriginalWreckComponents50LA> forestStorage_{};
    rr::game::OriginalWreckComponentDomain50LA forest_{};
    struct Occurrence {
        //Native progress for the bounded initial child of a later source occurrence.
        bool freshInitialChildAttempted=false,freshInitialChildComplete=false,freshInitialChildSmoke=false;
        bool freshInitialReaderAttempted=false,freshInitialReaderComplete=false;
        bool freshInitialCallerAttempted=false,freshInitialCallerComplete=false;
        bool freshSuccessorAttempted=false,freshSuccessorComplete=false;
        bool freshSuccessorReaderAttempted=false,freshSuccessorReaderComplete=false;
        bool freshSuccessorCallerAttempted=false,freshSuccessorCallerComplete=false;
        bool freshFirstPairStopAttempted=false,freshFirstPairStopped=false;
        bool freshRemainingPairsAttempted=false,freshRemainingPairsComplete=false;
        bool freshPositiveDustAttempted=false,freshPositiveDustComplete=false;
        bool freshPositiveDustReaderAttempted=false,freshPositiveDustReaderComplete=false;
        bool freshPositiveSplashAttempted=false,freshPositiveSplashComplete=false;
        bool freshPositiveSplashReaderAttempted=false,freshPositiveSplashReaderComplete=false;
        bool freshPieceAppendAttempted=false,freshPieceAppendComplete=false;
        bool freshWaterAttempted=false,freshWaterComplete=false;
        bool freshPieceCursorAttempted=false,freshPieceCursorComplete=false;
        std::vector<std::unique_ptr<rr::game::OriginalWreckSmokeOwner50LA>> smoke;
        std::vector<std::unique_ptr<rr::game::OriginalWreckFireOwner50LA>> fire;
        std::unique_ptr<rr::game::OriginalWreckDustOwner50LA> dust;
        std::unique_ptr<rr::game::OriginalWreckSplashOwner50LA> splash;
    };
    struct CollectionEffects {
        std::vector<std::unique_ptr<Occurrence>> occurrences;
        std::unique_ptr<rr::game::OriginalWreckGroupSparksOwner50LA> sparks;
        std::unique_ptr<rr::game::OriginalWreckGroupExplosionOwner50LA> explosion;
        bool groupAttempted=false,deactivateAttempted=false,deactivated=false,retired=false;
        std::size_t completed=0;
    };
    std::vector<std::unique_ptr<CollectionEffects>> collections_;
    std::unique_ptr<rr::game::OriginalWreckGroupExplosionOwner50LA> racerExplosion814_;
    std::unique_ptr<rr::game::OriginalWreckGroupSparksOwner50LA> racerSparks810_;
    bool racerTailAttempted_=false,racerTailComplete_=false;
    std::array<bool,2> racerTailPublished_{}; //814 then810, actual source stores only
    std::array<std::unique_ptr<rr::game::OriginalRacerEmberOwner50LA>,10> embers818_;
    std::size_t emberPublished_=0;
    bool emberAttempted_=false,emberComplete_=false;
    std::uint32_t nextControlId_=1;
    bool attempted_=false,externalRetired_=false,terminal_=false,freshInputsOnly_=false;
    bool freshSplashAttempted_=false,freshSplashComplete_=false;
    bool freshPieceCursorAttempted_=false,freshPieceCursorComplete_=false;
    bool freshWaterAttempted_=false,freshWaterComplete_=false;
    bool freshPieceAppendAttempted_=false,freshPieceAppendComplete_=false;
    bool freshGroupConstructorAttempted_=false,freshGroupConstructorComplete_=false;
    bool freshGroupPrimaryAttempted_=false,freshGroupPrimaryComplete_=false;
    bool freshGroupContextAttempted_=false,freshGroupContextComplete_=false;
    bool freshGroupSparksAttempted_=false,freshGroupSparksComplete_=false;
    bool freshGroupSparksSetupAttempted_=false,freshGroupSparksSetupComplete_=false;
    bool freshGroupSparksConfigAttempted_=false,freshGroupSparksConfigComplete_=false;
    bool freshSplashReadAttempted_=false,freshSplashReadComplete_=false;
    bool freshDustAttempted_=false,freshDustComplete_=false;
    bool freshDustReadAttempted_=false,freshDustReadComplete_=false;
    bool freshRemainingPairsAttempted_=false,freshRemainingPairsComplete_=false;
    bool freshFirstPairStopAttempted_=false,freshFirstPairStopped_=false;
    bool freshFireAttempted_=false,freshFireComplete_=false,freshFireReadAttempted_=false,freshFireReadComplete_=false;
    bool freshSmokeAttempted_=false,freshSmokeComplete_=false,freshSmokeReadAttempted_=false,freshSmokeReadComplete_=false;
    std::uint32_t pc_=0x166390;
    bool bindFreshInputContext(rr::game::OriginalRacerEntityCold50LA& racer,
        rr::game::OriginalTrackParticleCold50LA& manager,G* const* actualParent7FC)noexcept {
        if(attempted_||terminal_||externalRetired_||freshInputsOnly_||!racer.prepared()||
           !racer.postOverlayConstructed210A20()||!actualParent7FC||*actualParent7FC!=&racer.particle7FC||
           !manager.prepared()||!manager.particles()||racer.racer.reference00!=1u||
           racer.particle7FC.reference00!=1u||racer.particle7FC.parent1C!=&racer.racer||
           racer.particle7FC.firstChild18||
           racer.particle7FC.sessionGeneration!=racer.racer.sessionGeneration)return false;
        //Native typed context for the SAME actual210770 t2/Track4A0 argument.
        //No emitter resources, constructors, pool, alternate clock or future arrays.
        freshInputsOnly_=true;attempted_=true;sourceManager4A0_=&manager;
        context_.particleManager=manager.particles();sourceContext_=&context_;
        sourceParentField_=actualParent7FC;sourceParent_=*actualParent7FC;
        forestStorage_[0]=&racer.racer;forestStorage_[1]=sourceParent_;
        forest_={racer.racer.sessionGeneration,forestStorage_.data(),2};pc_=0x2cb618u;
        return true;
    }
    bool borrowFreshSharedInputs2CB618(rr::game::OriginalRacerEntityCold50LA& racer,
        rr::game::OriginalTrackParticleCold50LA& manager,G* const* actualParent7FC,
        rr::game::OriginalWreckPieceEffectInputs50LA& result)const noexcept {
        result={};
        //terminal_ is a sticky legacy-route exclusion after fresh Smoke, not
        //a failure verdict. Require the actual completed first-piece cursor.
        if(!attempted_||!freshInputsOnly_||externalRetired_||
           !freshPieceCursorAttempted_||!freshPieceCursorComplete_||pc_!=0x22c0a0u||
           !racer.prepared()||!racer.postOverlayConstructed210A20()||
           sourceManager4A0_!=&manager||!manager.prepared()||!manager.particles()||
           context_.particleManager!=manager.particles()||sourceContext_!=&context_||
           !actualParent7FC||sourceParentField_!=actualParent7FC||
           *actualParent7FC!=&racer.particle7FC||sourceParent_!=&racer.particle7FC||
           forestStorage_[0]!=&racer.racer||forestStorage_[1]!=sourceParent_||
           !canRetireFreshInputContext())return false;
        //Same incoming Track4A0 and actual7FC field for every piece. This is
        //a read-through borrow, not another forest/domain or child-stage grant.
        result={&sourceContext_,sourceParentField_};return true;
    }
    bool freshSmokeLayout(rr::game::OriginalWreckSmokeOwner50LA*& result)const noexcept {
        result=nullptr;
        if(collections_.size()>1u)return false;
        if(collections_.empty())return true;
        const auto& c=collections_[0];if(!c||c->groupAttempted||c->deactivateAttempted||c->completed||
            (c->sparks&&!freshGroupSparksAttempted_)||c->explosion||c->occurrences.size()>2u)return false;
        if(c->occurrences.empty())return true;
        if(c->occurrences.size()==2u){
            const auto& second=c->occurrences[1];
            if(!freshPieceCursorComplete_||!second||!second->freshInitialChildAttempted||
               second->fire.size()>512u||second->smoke.size()>512u||
               second->fire.size()>second->smoke.size()||second->smoke.size()>second->fire.size()+1u||
               (!second->freshInitialChildSmoke&&(!second->smoke.empty()||!second->fire.empty())))return false;
            if(second->freshInitialChildSmoke&&second->dust&&
               (!second->freshPositiveDustAttempted||!second->freshFirstPairStopped||
                (second->dust->appended_&&!second->freshPositiveDustReaderAttempted)))return false;
            if(second->freshPositiveDustComplete&&(!second->freshInitialChildSmoke||!second->dust))return false;
            if(second->freshInitialChildSmoke&&second->splash&&
               (!second->freshPositiveSplashAttempted||!second->freshPositiveDustReaderComplete||
                (second->splash->appended_&&!second->freshPositiveSplashReaderAttempted)))return false;
            if(second->freshPositiveSplashComplete&&(!second->freshInitialChildSmoke||!second->splash))return false;
            if((!second->fire.empty()||second->splash)&&
               (!second->freshSuccessorAttempted||!second->freshInitialCallerComplete))return false;
            if((second->smoke.size()>1u||second->fire.size()>1u)&&
               (!second->freshInitialChildSmoke||!second->freshRemainingPairsAttempted||!second->freshFirstPairStopped))return false;
            for(const auto& owner:second->fire)if(!owner)return false;
            for(const auto& owner:second->smoke)if(!owner)return false;
            if(second->freshSuccessorComplete&&
               (second->freshInitialChildSmoke?second->fire.empty():!second->splash))return false;
            if(!second->smoke.empty()&&!second->smoke[0])return false;
            if(second->freshInitialChildComplete&&
               (second->freshInitialChildSmoke?second->smoke.empty():!second->dust))return false;
        }
        const auto& o=c->occurrences[0];if(!o||o->fire.size()>512u||o->smoke.size()>512u||o->fire.size()>o->smoke.size()||o->smoke.size()>o->fire.size()+1u)return false;
        for(const auto& value:o->smoke)if(!value)return false;
        for(const auto& value:o->fire)if(!value)return false;
        if(!o->smoke.empty())result=o->smoke[0].get();return true;
    }
    rr::game::OriginalWreckFireOwner50LA* freshFireOwner()const noexcept {
        if(collections_.size()!=1u||!collections_[0]||collections_[0]->occurrences.size()!=1u||
           !collections_[0]->occurrences[0]||collections_[0]->occurrences[0]->fire.size()!=1u)return nullptr;
        return collections_[0]->occurrences[0]->fire[0].get();
    }
    bool canRetireFreshInputContext()const noexcept {
        if(!attempted_)return true;
        if(!freshInputsOnly_||racerTailAttempted_||emberAttempted_||racerExplosion814_||racerSparks810_)return false;
        for(const auto& owner:embers818_)if(owner)return false;
        rr::game::OriginalWreckSmokeOwner50LA* firstSmoke=nullptr;
        if(!freshSmokeLayout(firstSmoke)||(!freshSmokeAttempted_&&!freshDustAttempted_&&!collections_.empty()))return false;
        const Occurrence* occurrence=nullptr;
        if(!collections_.empty()&&!collections_[0]->occurrences.empty())occurrence=collections_[0]->occurrences[0].get();
        const Occurrence* secondOccurrence=collections_.size()==1u&&collections_[0]->occurrences.size()==2u?
            collections_[0]->occurrences[1].get():nullptr;
        if(secondOccurrence){
            if(!freshPieceCursorComplete_||!freshSplashReadComplete_||!occurrence||!occurrence->splash)return false;
            const bool linked=(!secondOccurrence->smoke.empty()&&secondOccurrence->smoke[0]->appended_)||
                (secondOccurrence->dust&&secondOccurrence->dust->appended_);
            if(linked&&(!secondOccurrence->freshInitialCallerAttempted||
                        !secondOccurrence->freshInitialReaderComplete))return false;
            const bool successorLinked=(!secondOccurrence->fire.empty()&&secondOccurrence->fire[0]->appended_)||
                (secondOccurrence->splash&&secondOccurrence->splash->appended_);
            if(successorLinked&&(!secondOccurrence->freshSuccessorCallerAttempted||
                                 !secondOccurrence->freshSuccessorReaderComplete))return false;
        }
        if(occurrence&&!occurrence->fire.empty()&&(!freshFireAttempted_||!freshSmokeReadComplete_))return false;
        if(occurrence&&occurrence->dust&&!freshDustAttempted_)return false;
        if(occurrence&&occurrence->splash&&!freshSplashAttempted_)return false;
        const auto* groupSparks=collections_.empty()?nullptr:collections_[0]->sparks.get();
        if(groupSparks&&(!freshGroupSparksAttempted_||!freshGroupContextComplete_))return false;
        if(externalRetired_){
            if(groupSparks&&!groupSparks->retired_)return false;
            for(const auto* current:{occurrence,secondOccurrence})if(current){
                for(const auto& o:current->smoke)if(!o->retired_)return false;
                for(const auto& o:current->fire)if(!o->retired_)return false;
                if(current->dust&&!current->dust->retired_)return false;
                if(current->splash&&!current->splash->retired_)return false;
            }
            return true;
        }
        if(!retirementContextCurrent()||sourceContext_!=&context_||forestStorage_[1]!=sourceParent_||
           !forestStorage_[0]||forestStorage_[0]->reference00!=1u||sourceParent_->reference00!=1u||
           sourceParent_->parent1C!=forestStorage_[0])return false;
        //Scratch identity view only: persistent source160/164 remain the sole
        //piece pointer arrays; typed unique_ptr owners retain actual lifetimes.
        std::array<const G*,rr::game::maxOriginalWreckComponents50LA> live{};
        std::array<bool,rr::game::maxOriginalWreckComponents50LA> linked{};
        std::size_t liveCount=0;bool ended=false,unlinked=false;
        const std::uint32_t* actualCount=nullptr;const std::uint32_t* actualLock=nullptr;
        auto admit=[&](const auto& pointer)->bool {
            const auto& owner=*pointer;const auto& c=owner.component_;
            if(owner.retired_){ended=true;return !c.reference00&&!c.nativeEffectControlId&&
                c.kind==(owner.initialized_?rr::game::OriginalWreckComponentKind50LA::entity141300:
                    rr::game::OriginalWreckComponentKind50LA::unbound);}
            if(!owner.initialized_){ended=true;return owner.preflightFreshUnappended();}
            if(ended||unlinked||liveCount+2u>=forestStorage_.size())return false;
            const auto& binding=owner.binding_;
            if(binding.domain!=&forest_||binding.domainStorage!=forestStorage_.data()||
               binding.domainCapacity!=forestStorage_.size()||binding.particleParent!=sourceParent_||
               (actualCount&&actualCount!=binding.actualObjectCount423F60)||
               (actualLock&&actualLock!=binding.actualAttachmentLock423F58))return false;
            actualCount=binding.actualObjectCount423F60;actualLock=binding.actualAttachmentLock423F58;
            live[liveCount]=&c;linked[liveCount]=owner.appended_;++liveCount;
            unlinked=!owner.appended_;return true;
        };
        if(occurrence)for(std::size_t i=0;i<occurrence->smoke.size();++i){
            if(!admit(occurrence->smoke[i]))return false;
            if(i<occurrence->fire.size()&&!admit(occurrence->fire[i]))return false;
        }
        if(occurrence&&occurrence->dust&&!admit(occurrence->dust))return false;
        if(occurrence&&occurrence->splash&&!admit(occurrence->splash))return false;
        if(secondOccurrence){
            for(std::size_t i=0;i<secondOccurrence->smoke.size();++i){
                if(!admit(secondOccurrence->smoke[i]))return false;
                if(i<secondOccurrence->fire.size()&&!admit(secondOccurrence->fire[i]))return false;
            }
            if(secondOccurrence->dust&&!admit(secondOccurrence->dust))return false;
            if(secondOccurrence->splash&&!admit(secondOccurrence->splash))return false;
        }
        if(groupSparks&&!admit(collections_[0]->sparks))return false;
        if(forest_.count!=liveCount+2u||
           sourceParent_->firstChild18!=(liveCount&&linked[0]?live[0]:nullptr)||
           (liveCount&&(!actualCount||*actualCount<liveCount)))return false;
        std::size_t index=0;
        auto check=[&](const auto& pointer)->bool {
            const auto& owner=*pointer;if(owner.retired_||!owner.initialized_)return true;
            const auto* previous=linked[index]&&index?live[index-1u]:nullptr;
            const auto* next=linked[index]&&index+1u<liveCount&&linked[index+1u]?live[index+1u]:nullptr;
            const bool okay=forestStorage_[index+2u]==&owner.component_&&
                owner.preflightFreshChainMember(index+2u,previous,next);
            ++index;return okay;
        };
        if(occurrence)for(std::size_t i=0;i<occurrence->smoke.size();++i){
            if(!check(occurrence->smoke[i]))return false;
            if(i<occurrence->fire.size()&&!check(occurrence->fire[i]))return false;
        }
        if(occurrence&&occurrence->dust&&!check(occurrence->dust))return false;
        if(occurrence&&occurrence->splash&&!check(occurrence->splash))return false;
        if(secondOccurrence){
            for(std::size_t i=0;i<secondOccurrence->smoke.size();++i){
                if(!check(secondOccurrence->smoke[i]))return false;
                if(i<secondOccurrence->fire.size()&&!check(secondOccurrence->fire[i]))return false;
            }
            if(secondOccurrence->dust&&!check(secondOccurrence->dust))return false;
            if(secondOccurrence->splash&&!check(secondOccurrence->splash))return false;
        }
        if(groupSparks&&!check(collections_[0]->sparks))return false;
        return index==liveCount;
    }
    bool retireFreshExternalPrefix()noexcept {
        if(!canRetireFreshInputContext())return false;
        if(collections_.empty())return true;
        auto& sparks=collections_[0]->sparks;
        if(sparks&&!sparks->retired_&&!sparks->retireFreshUnappended(true))return false;
        for(std::size_t n=collections_[0]->occurrences.size();n;--n){
            auto& occurrence=*collections_[0]->occurrences[n-1u];
            if(occurrence.splash&&!occurrence.splash->retired_&&!occurrence.splash->retireFreshUnappended(true))return false;
            if(occurrence.dust&&!occurrence.dust->retired_&&!occurrence.dust->retireFreshUnappended(true))return false;
            for(std::size_t count=occurrence.smoke.size();count;--count){const auto i=count-1u;
                if(i<occurrence.fire.size()&&!occurrence.fire[i]->retired_&&
                   !occurrence.fire[i]->retireFreshUnappended(true))return false;
                if(!occurrence.smoke[i]->retired_&&!occurrence.smoke[i]->retireFreshUnappended(true))return false;
            }
        }
        return true;
    }
    bool retireFreshInputContext()noexcept {
        if(!canRetireFreshInputContext())return false;
        if(!collections_.empty()&&collections_[0]->sparks&&!collections_[0]->sparks->retired_)return false;
        if(!collections_.empty())for(const auto& pointer:collections_[0]->occurrences){
            const auto& occurrence=*pointer;
            for(const auto& owner:occurrence.smoke)if(!owner->retired_)return false;
            for(const auto& owner:occurrence.fire)if(!owner->retired_)return false;
            if(occurrence.dust&&!occurrence.dust->retired_)return false;
            if(occurrence.splash&&!occurrence.splash->retired_)return false;
        }
        if(attempted_)externalRetired_=true;
        return true;
    }
    bool constructFreshSecondPositiveSplash(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::array<std::uint32_t,4>* actual433F50,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb854u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialChildSmoke||!occurrence.freshPositiveDustReaderComplete||
           occurrence.freshPositiveSplashAttempted||!occurrence.dust||!occurrence.dust->appended_||occurrence.splash)return false;
        const auto priorId=occurrence.dust->binding_.controlOwnerId;
        if(priorId==std::numeric_limits<std::uint32_t>::max())return false;
        occurrence.freshPositiveSplashAttempted=true;
        try {
            occurrence.splash=std::make_unique<rr::game::OriginalWreckSplashOwner50LA>();
            auto& splash=*occurrence.splash;
            const auto result=owner.constructFreshSplash2CB854(splash,
                binding<rr::game::OriginalWreckSplashBinding50LA>(splash,app,priorId+1u),
                actual433F50,actualDirectory36B530,1u);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x19ddc8u)return false;
            occurrence.freshPositiveSplashComplete=true;return true;
        }catch(...){return false;}
    }
    bool constructFreshSecondPositiveDust(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::string* actualDirectory36B530,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb7e0u;
        std::size_t index=0;std::uint32_t count=0,currentPC=0;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u||
           !owner.freshEffectCursor(index,count,currentPC,1u)||!count||index!=count||currentPC!=pc)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialChildSmoke||!occurrence.freshFirstPairStopped||
           occurrence.freshPositiveDustAttempted||occurrence.dust||occurrence.splash||
           occurrence.smoke.size()!=count||occurrence.fire.size()!=count||
           !occurrence.fire.back()->appended_)return false;
        const auto priorId=occurrence.fire.back()->binding_.controlOwnerId;
        if(priorId==std::numeric_limits<std::uint32_t>::max())return false;
        occurrence.freshPositiveDustAttempted=true;
        try {
            occurrence.dust=std::make_unique<rr::game::OriginalWreckDustOwner50LA>();
            auto& dust=*occurrence.dust;
            const auto result=owner.constructFreshDust2CB7E0(dust,
                binding<rr::game::OriginalWreckDustBinding50LA>(dust,app,priorId+1u),actualDirectory36B530,1u);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x19a7d0u)return false;
            occurrence.freshPositiveDustComplete=true;return true;
        }catch(...){return false;}
    }
    bool constructFreshSecondSuccessorChild(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::array<std::uint32_t,4>* actual433F50,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if((pc!=0x2cb720u&&pc!=0x2cb854u)||!freshInputsOnly_||externalRetired_||pc_!=pc||
           !canRetireFreshInputContext()||collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialCallerComplete||occurrence.freshSuccessorAttempted||
           !occurrence.fire.empty()||occurrence.splash)return false;
        std::uint32_t initialId=0;
        if(occurrence.freshInitialChildSmoke){
            if(pc!=0x2cb720u||occurrence.smoke.size()!=1u||!occurrence.smoke[0]->appended_)return false;
            initialId=occurrence.smoke[0]->binding_.controlOwnerId;
        }else{
            if(pc!=0x2cb854u||!occurrence.dust||!occurrence.dust->appended_)return false;
            initialId=occurrence.dust->binding_.controlOwnerId;
        }
        if(initialId==std::numeric_limits<std::uint32_t>::max())return false;
        occurrence.freshSuccessorAttempted=true;
        try {
            rr::game::OriginalWreckConstructorPrefixResult50LA result;
            if(occurrence.freshInitialChildSmoke){
                occurrence.fire.push_back(std::make_unique<rr::game::OriginalWreckFireOwner50LA>());
                auto& fire=*occurrence.fire[0];
                result=owner.constructFreshFire2CB720(fire,
                    binding<rr::game::OriginalWreckFireBinding50LA>(fire,app,initialId+1u),actualDirectory36B530,1u);
            }else{
                occurrence.splash=std::make_unique<rr::game::OriginalWreckSplashOwner50LA>();
                auto& splash=*occurrence.splash;
                result=owner.constructFreshSplash2CB854(splash,
                    binding<rr::game::OriginalWreckSplashBinding50LA>(splash,app,initialId+1u),
                    actual433F50,actualDirectory36B530,1u);
            }
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=(occurrence.freshInitialChildSmoke?0x15e024u:0x19ddc8u))return false;
            occurrence.freshSuccessorComplete=true;return true;
        }catch(...){return false;}
    }
    bool completeFreshSecondInitialChild(rr::game::OriginalWreckRacerStorage50LA& owner,
        std::uint32_t& pc)noexcept {
        pc=0x19a7f8u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialReaderComplete||occurrence.freshInitialCallerAttempted)return false;
        occurrence.freshInitialCallerAttempted=true;
        const auto result=owner.completeFreshSecondInitialChild19A7F8();
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(occurrence.freshInitialChildSmoke){
            if(result.status!=S::effectFireRequired||pc!=0x2cb720u)return false;
        }else if(result.status!=S::stopped||pc!=0x2cb854u)return false;
        occurrence.freshInitialCallerComplete=true;return true;
    }
    bool completeFreshSecondSuccessorChild(rr::game::OriginalWreckRacerStorage50LA& owner,
        std::uint32_t& pc)noexcept {
        if((pc!=0x15e0b0u&&pc!=0x19de7cu)||!freshInputsOnly_||externalRetired_||pc_!=pc||
           !canRetireFreshInputContext()||collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshSuccessorReaderComplete||occurrence.freshSuccessorCallerAttempted||
           pc!=(occurrence.freshInitialChildSmoke?0x15e0b0u:0x19de7cu))return false;
        occurrence.freshSuccessorCallerAttempted=true;
        const auto result=owner.completeFreshSecondSuccessorChild();
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(result.status!=rr::game::OriginalWreckConstructorPrefixStatus50LA::stopped||
           pc!=(occurrence.freshInitialChildSmoke?0x2cb778u:0x2cb8c4u))return false;
        occurrence.freshSuccessorCallerComplete=true;return true;
    }
    bool readFreshSecondSuccessorChild(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(pc!=0x15e024u&&pc!=0x19ddc8u)return false;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!provider.sealed()||
           !canRetireFreshInputContext()||collections_.size()!=1u||
           collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshSuccessorComplete||occurrence.freshSuccessorReaderAttempted)return false;
        const char* filename=nullptr;
        auto select=[&](const auto& pointer){
            if(!pointer||!pointer->freshFilenameKnown_||
               pointer->freshFilenameLength_>=pointer->freshFilename78_.size()||
               pointer->freshFilename78_[pointer->freshFilenameLength_])return false;
            filename=pointer->freshFilename78_.data();return true;
        };
        if(occurrence.freshInitialChildSmoke){
            if(pc!=0x15e024u||occurrence.fire.size()!=1u||!select(occurrence.fire[0]))return false;
        }else if(pc!=0x19ddc8u||!select(occurrence.splash))return false;
        occurrence.freshSuccessorReaderAttempted=true;pc=pc_=0x15ae2cu;
        try {
            const auto member=occurrence.freshInitialChildSmoke?1u:3u;
            //Every reached native Config lookup uses this actual filename.
            //Existing first-owner byte borrows never reallocate or change identity.
            if(!freshReadMember(provider,filename,bytes_[member]))return false;
            if(occurrence.freshInitialChildSmoke){
                if(context_.podFireEmt&&context_.podFireEmt!=&bytes_[1])return false;
                context_.podFireEmt=&bytes_[1];
            }else{
                if(context_.podWreckSplashEmt&&context_.podWreckSplashEmt!=&bytes_[3])return false;
                context_.podWreckSplashEmt=&bytes_[3];
            }
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readFreshSecondSuccessorChild(bytes_[member],diagnostic);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=(occurrence.freshInitialChildSmoke?0x15e0b0u:0x19de7cu))return false;
            occurrence.freshSuccessorReaderComplete=true;return true;
        }catch(...){return false;}
    }
    bool readFreshSecondInitialChild(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x19a7d0u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!provider.sealed()||
           !canRetireFreshInputContext()||collections_.size()!=1u||
           collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialChildComplete||occurrence.freshInitialReaderAttempted)return false;
        const char* filename=nullptr;
        auto select=[&](const auto& pointer){
            if(!pointer||!pointer->freshFilenameKnown_||
               pointer->freshFilenameLength_>=pointer->freshFilename78_.size()||
               pointer->freshFilename78_[pointer->freshFilenameLength_])return false;
            filename=pointer->freshFilename78_.data();return true;
        };
        if(occurrence.freshInitialChildSmoke){
            if(occurrence.smoke.size()!=1u||!select(occurrence.smoke[0]))return false;
        }else if(!select(occurrence.dust))return false;
        occurrence.freshInitialReaderAttempted=true;pc=pc_=0x15ae2cu;
        try {
            const auto member=occurrence.freshInitialChildSmoke?0u:2u;
            //Every reached native Config lookup uses this actual filename.
            //Existing first-owner byte borrows never reallocate or change identity.
            if(!freshReadMember(provider,filename,bytes_[member]))return false;
            if(occurrence.freshInitialChildSmoke){
                if(context_.podWreckSmokeEmt&&context_.podWreckSmokeEmt!=&bytes_[0])return false;
                context_.podWreckSmokeEmt=&bytes_[0];
            }else{
                if(context_.podWreckDustEmt&&context_.podWreckDustEmt!=&bytes_[2])return false;
                context_.podWreckDustEmt=&bytes_[2];
            }
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readFreshSecondInitialChild19A7D0(bytes_[member],diagnostic);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x19a7f8u)return false;
            occurrence.freshInitialReaderComplete=true;return true;
        }catch(...){return false;}
    }
    bool constructFreshSecondInitialChild(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::string* actualDirectory36B530,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(!freshInputsOnly_||externalRetired_||!freshPieceCursorComplete_||pc_!=0x22c0a0u||
           (pc!=0x2cb6ccu&&pc!=0x2cb7e4u)||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=1u)return false;
        auto& collection=*collections_[0];const auto& first=*collection.occurrences[0];
        if(!first.dust||!first.splash||!first.dust->appended_||!first.splash->appended_||
           first.smoke.size()!=first.fire.size()||nextControlId_>std::numeric_limits<std::uint32_t>::max()-2u||
           first.smoke.size()>(std::numeric_limits<std::uint32_t>::max()-nextControlId_-2u)/2u)return false;
        const auto id=nextControlId_+2u*static_cast<std::uint32_t>(first.smoke.size())+2u;
        const bool smokeBranch=pc==0x2cb6ccu;
        try {
            auto next=std::make_unique<Occurrence>();
            next->freshInitialChildAttempted=true;next->freshInitialChildSmoke=smokeBranch;
            collection.occurrences.push_back(std::move(next));
            auto& occurrence=*collection.occurrences[1];
            rr::game::OriginalWreckConstructorPrefixResult50LA result;
            if(smokeBranch){
                occurrence.smoke.push_back(std::make_unique<rr::game::OriginalWreckSmokeOwner50LA>());
                auto& smoke=*occurrence.smoke[0];
                result=owner.constructFreshSmoke2CB6CC(smoke,
                    binding<rr::game::OriginalWreckSmokeBinding50LA>(smoke,app,id),actualDirectory36B530,1u);
            }else{
                occurrence.dust=std::make_unique<rr::game::OriginalWreckDustOwner50LA>();
                auto& dust=*occurrence.dust;
                result=owner.constructFreshDust2CB7E0(dust,
                    binding<rr::game::OriginalWreckDustBinding50LA>(dust,app,id),actualDirectory36B530,1u);
            }
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x19a7d0u)return false;
            occurrence.freshInitialChildComplete=true;return true;
        }catch(...){return false;}
    }
    bool constructFreshSplash(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::array<std::uint32_t,4>* actual433F50,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        pc=0x2cb854u;
        if(!freshInputsOnly_||!freshDustReadComplete_||freshSplashAttempted_||externalRetired_||
           pc_!=0x2cb854u||!canRetireFreshInputContext()||collections_.size()!=1u||!collections_[0]||
           collections_[0]->occurrences.size()!=1u||!collections_[0]->occurrences[0])return false;
        auto& occurrence=*collections_[0]->occurrences[0];
        if(!occurrence.dust||!occurrence.dust->appended_||occurrence.splash||
           occurrence.smoke.size()!=occurrence.fire.size()||
           occurrence.smoke.size()>(std::numeric_limits<std::uint32_t>::max()-nextControlId_-1u)/2u)return false;
        freshSplashAttempted_=true;
        try {
            occurrence.splash=std::make_unique<rr::game::OriginalWreckSplashOwner50LA>();
            auto& splash=*occurrence.splash;
            const auto result=owner.constructFreshSplash2CB854(splash,
                binding<rr::game::OriginalWreckSplashBinding50LA>(splash,app,
                    nextControlId_+2u*static_cast<std::uint32_t>(occurrence.smoke.size())+1u),
                actual433F50,actualDirectory36B530);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x19ddc8u)return false;
            freshSplashComplete_=true;return true;
        }catch(...){return false;}
    }
    bool beginFreshGroupSparksConfig(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        pc=0x15f2f0u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!freshGroupSparksSetupComplete_||
           freshGroupSparksConfigAttempted_||!canRetireFreshInputContext()||collections_.size()!=1u||!collections_[0]->sparks)return false;
        freshGroupSparksConfigAttempted_=true;
        const bool done=owner.beginFreshGroupSparksConfig15F2F0(pc);pc_=pc;
        if(!done||pc!=0x15ae2cu)return false;
        freshGroupSparksConfigComplete_=true;return true; //Constructor only, not Config load/reader complete.
    }
    bool setupFreshGroupSparks(rr::game::OriginalWreckRacerStorage50LA& owner,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        pc=0x2cc0d4u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!freshGroupSparksComplete_||
           freshGroupSparksSetupAttempted_||!canRetireFreshInputContext()||collections_.size()!=1u)return false;
        freshGroupSparksSetupAttempted_=true;
        const bool done=owner.setupFreshGroupSparks2CC0D4(actualDirectory36B530,pc);pc_=pc;
        if(!done||(pc!=0x15f2f0u&&pc!=0x2cc124u))return false;
        freshGroupSparksSetupComplete_=true;return true;
    }
    bool constructFreshGroupSparks(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,std::uint32_t& pc)noexcept {
        pc=0x2cc0b4u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!freshGroupContextComplete_||freshGroupSparksAttempted_||
           !canRetireFreshInputContext()||collections_.size()!=1u||collections_[0]->sparks||collections_[0]->explosion)return false;
        //Native control identity follows the last actual piece child in this SAME forest.
        if(forest_.count<3u||!forestStorage_[forest_.count-1u]||
           !forestStorage_[forest_.count-1u]->nativeEffectControlId)return false;
        const auto previousId=*forestStorage_[forest_.count-1u]->nativeEffectControlId;
        if(previousId==std::numeric_limits<std::uint32_t>::max())return false;
        if(!owner.beginFreshGroupSparks2CC0B4(pc)){pc_=pc;return false;}
        freshGroupSparksAttempted_=true;pc_=pc;
        auto& child=collections_[0]->sparks;
        //Host typed allocation is not guest sizeof0x240. Its failure follows the actual NULL arm.
        try {child=std::make_unique<rr::game::OriginalWreckGroupSparksOwner50LA>();}catch(...){child.reset();}
        bool done=false;
        if(child){const auto b=binding<rr::game::OriginalWreckGroupSparksBinding50LA>(*child,app,previousId+1u);
            done=owner.finishFreshGroupSparks2CC0BC(child.get(),&b,pc);
        }else done=owner.finishFreshGroupSparks2CC0BC(nullptr,nullptr,pc);
        pc_=pc;if(!done||pc!=0x2cc0d4u)return false;
        freshGroupSparksComplete_=true;return true;
    }
    bool consumeFreshGroupContext(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        pc=0x2cc088u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!freshGroupPrimaryComplete_||
           freshGroupContextAttempted_||!canRetireFreshInputContext()||
           !sourceManager4A0_||!sourceManager4A0_->prepared()||
           sourceContext_!=&context_||context_.particleManager!=sourceManager4A0_->particles()||
           !sourceParentField_||*sourceParentField_!=sourceParent_)return false;
        freshGroupContextAttempted_=true;
        const bool done=owner.consumeFreshGroupContext2CC088({&sourceContext_,sourceParentField_},pc);
        pc_=pc;
        if(!done||pc!=0x2cc0b4u)return false;
        freshGroupContextComplete_=true;return true;
    }
    bool bindFreshGroupPrimary(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        pc=0x22c298u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!freshGroupConstructorComplete_||
           freshGroupPrimaryAttempted_||!canRetireFreshInputContext())return false;
        freshGroupPrimaryAttempted_=true;
        const bool done=owner.bindFreshGroupPrimary22C298(pc);
        pc_=pc;
        if(!done||pc!=0x2cc088u)return false;
        freshGroupPrimaryComplete_=true;return true;
    }
    bool constructFreshGroup(rr::game::OriginalWreckRacerStorage50LA& owner,
        const std::array<std::uint32_t,4>& actual4441F0,std::uint32_t& pc)noexcept {
        pc=0x22c278u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||freshGroupConstructorAttempted_||
           !freshPieceCursorComplete_||!canRetireFreshInputContext()||collections_.size()!=1u)return false;
        const auto count=collections_[0]->occurrences.size();
        if(count!=1u&&(count!=2u||!collections_[0]->occurrences[1]->freshPieceCursorComplete))return false;
        freshGroupConstructorAttempted_=true;
        const bool done=owner.constructFreshGroup22C278(actual4441F0,pc);
        pc_=pc;
        if(!done||pc!=0x22c298u)return false;
        freshGroupConstructorComplete_=true;return true;
    }
    bool advanceFreshSecondPieceCursor(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x22c25cu;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshWaterComplete||occurrence.freshPieceCursorAttempted)return false;
        occurrence.freshPieceCursorAttempted=true;
        const auto result=owner.advanceFreshPieceCursor22C25C(1u);
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(!((result.status==S::nextPieceRequired&&pc==0x22c0a0u)||
             (result.status==S::groupConstructorRequired&&pc==0x22c278u)))return false;
        occurrence.freshPieceCursorComplete=true;return true;
    }
    bool advanceFreshPieceCursor(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x22c25cu;
        if(!freshInputsOnly_||!freshWaterComplete_||freshPieceCursorAttempted_||externalRetired_||
           pc_!=0x22c25cu||!canRetireFreshInputContext())return false;
        freshPieceCursorAttempted_=true;
        const auto result=owner.advanceFreshPieceCursor22C25C();
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(!((result.status==S::nextPieceRequired&&pc==0x22c0a0u)||
             (result.status==S::groupConstructorRequired&&pc==0x22c278u)))return false;
        freshPieceCursorComplete_=true;return true;
    }
    bool applyFreshSecondWater(rr::game::OriginalWreckRacerStorage50LA& owner,
        const std::uint32_t& actualNamedWaterBit,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x22c21cu;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshPieceAppendComplete||occurrence.freshWaterAttempted)return false;
        occurrence.freshWaterAttempted=true;
        const auto result=owner.applyFreshCollectionWater22C21C(actualNamedWaterBit,1u);
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(result.status!=S::stopped||pc!=0x22c25cu)return false;
        occurrence.freshWaterComplete=true;return true;
    }
    bool applyFreshWater(rr::game::OriginalWreckRacerStorage50LA& owner,
        const std::uint32_t& actualNamedWaterBit,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x22c21cu;
        if(!freshInputsOnly_||!freshPieceAppendComplete_||freshWaterAttempted_||externalRetired_||
           pc_!=0x22c21cu||!canRetireFreshInputContext())return false;
        freshWaterAttempted_=true;
        const auto result=owner.applyFreshCollectionWater22C21C(actualNamedWaterBit);
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(result.status!=S::stopped||pc!=0x22c25cu)return false;
        freshWaterComplete_=true;return true;
    }
    bool appendFreshSecondPiece(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb8c4u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!freshPieceCursorComplete_||
           !canRetireFreshInputContext()||collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(occurrence.freshPieceAppendAttempted||!occurrence.dust||!occurrence.splash||
           !occurrence.dust->appended_||!occurrence.splash->appended_||
           !occurrence.splash->freshReaderComplete_||occurrence.splash->sourceBoundary()!=0x2cb8c4u||
           (occurrence.freshInitialChildSmoke?!occurrence.freshPositiveSplashReaderComplete:
               !occurrence.freshSuccessorCallerComplete))return false;
        occurrence.freshPieceAppendAttempted=true;
        const auto result=owner.appendFreshOccurrencePiece22C1B8(1u);
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(result.status!=S::collectionFiltersRequired||pc!=0x22c21cu)return false;
        occurrence.freshPieceAppendComplete=true;return true;
    }
    bool appendFreshPiece(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb8c4u;
        if(!freshInputsOnly_||!freshSplashReadComplete_||freshPieceAppendAttempted_||externalRetired_||
           pc_!=0x2cb8c4u||!canRetireFreshInputContext())return false;
        freshPieceAppendAttempted_=true;
        const auto result=owner.appendFreshOccurrencePiece22C1B8();
        pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(result.status!=S::collectionFiltersRequired||pc!=0x22c21cu)return false;
        freshPieceAppendComplete_=true;return true;
    }
    bool readStopAppendFreshSecondPositiveSplash(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        pc=0x19ddc8u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!provider.sealed()||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialChildSmoke||!occurrence.freshPositiveSplashComplete||
           occurrence.freshPositiveSplashReaderAttempted)return false;
        auto* splash=occurrence.splash.get();
        if(!splash||!splash->freshFilenameKnown_||splash->freshFilenameLength_>=splash->freshFilename78_.size()||
           splash->freshFilename78_[splash->freshFilenameLength_])return false;
        occurrence.freshPositiveSplashReaderAttempted=true;pc=pc_=0x15ae2cu;
        try {
            if(!freshReadMember(provider,splash->freshFilename78_.data(),bytes_[3])||
               context_.podWreckSplashEmt!=&bytes_[3])return false;
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readStopAppendFreshSplash19DDC8(*splash,bytes_[3],diagnostic,1u);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=rr::game::OriginalWreckConstructorPrefixStatus50LA::stopped||pc!=0x2cb8c4u)return false;
            occurrence.freshPositiveSplashReaderComplete=true;return true;
        }catch(...){return false;}
    }
    bool readStopAppendFreshSplash(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        pc=0x19ddc8u;
        if(!freshInputsOnly_||!freshSplashComplete_||freshSplashReadAttempted_||externalRetired_||
           !provider.sealed()||!canRetireFreshInputContext()||collections_.size()!=1u||
           !collections_[0]||collections_[0]->occurrences.size()!=1u||!collections_[0]->occurrences[0])return false;
        auto* splash=collections_[0]->occurrences[0]->splash.get();
        if(!splash||!splash->freshFilenameKnown_||splash->freshFilenameLength_>=splash->freshFilename78_.size()||
           splash->freshFilename78_[splash->freshFilenameLength_])return false;
        freshSplashReadAttempted_=true;pc=pc_=0x15ae2cu;
        try {
            if(!freshReadMember(provider,splash->freshFilename78_.data(),bytes_[3]))return false;
            context_.podWreckSplashEmt=&bytes_[3];
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readStopAppendFreshSplash19DDC8(*splash,bytes_[3],diagnostic);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=rr::game::OriginalWreckConstructorPrefixStatus50LA::stopped||pc!=0x2cb8c4u)return false;
            freshSplashReadComplete_=true;return true;
        }catch(...){return false;}
    }
    bool readStopAppendFreshSecondPositiveDust(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        pc=0x19a7d0u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!provider.sealed()||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialChildSmoke||!occurrence.freshPositiveDustComplete||
           occurrence.freshPositiveDustReaderAttempted)return false;
        auto* dust=occurrence.dust.get();
        if(!dust||!dust->freshFilenameKnown_||dust->freshFilenameLength_>=dust->freshFilename78_.size()||
           dust->freshFilename78_[dust->freshFilenameLength_])return false;
        occurrence.freshPositiveDustReaderAttempted=true;pc=pc_=0x15ae2cu;
        try {
            if(!freshReadMember(provider,dust->freshFilename78_.data(),bytes_[2])||
               context_.podWreckDustEmt!=&bytes_[2])return false;
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readStopAppendFreshDust19A7D0(*dust,bytes_[2],diagnostic,1u);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=rr::game::OriginalWreckConstructorPrefixStatus50LA::stopped||pc!=0x2cb854u)return false;
            occurrence.freshPositiveDustReaderComplete=true;return true;
        }catch(...){return false;}
    }
    bool readStopAppendFreshDust(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        pc=0x19a7d0u;
        if(!freshInputsOnly_||!freshDustComplete_||freshDustReadAttempted_||externalRetired_||
           !provider.sealed()||!canRetireFreshInputContext()||collections_.size()!=1u||
           !collections_[0]||collections_[0]->occurrences.size()!=1u||!collections_[0]->occurrences[0])return false;
        auto* dust=collections_[0]->occurrences[0]->dust.get();
        if(!dust||!dust->freshFilenameKnown_||dust->freshFilenameLength_>=dust->freshFilename78_.size()||
           dust->freshFilename78_[dust->freshFilenameLength_])return false;
        freshDustReadAttempted_=true;pc=pc_=0x15ae2cu;
        try {
            if(!freshReadMember(provider,dust->freshFilename78_.data(),bytes_[2]))return false;
            context_.podWreckDustEmt=&bytes_[2];
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readStopAppendFreshDust19A7D0(*dust,bytes_[2],diagnostic);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=rr::game::OriginalWreckConstructorPrefixStatus50LA::stopped||pc!=0x2cb854u)return false;
            freshDustReadComplete_=true;return true;
        }catch(...){return false;}
    }
    bool constructFreshDust(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::string* actualDirectory36B530,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;
        if(!freshInputsOnly_||freshDustAttempted_||externalRetired_||
           (pc!=0x2cb7e0u&&pc!=0x2cb7e4u)||!canRetireFreshInputContext())return false;
        freshDustAttempted_=true;
        try {
            if(collections_.empty())collections_.push_back(std::make_unique<CollectionEffects>());
            auto& collection=*collections_[0];
            if(collection.occurrences.empty())collection.occurrences.push_back(std::make_unique<Occurrence>());
            auto& occurrence=*collection.occurrences[0];
            if(occurrence.dust||occurrence.smoke.size()!=occurrence.fire.size()||
               occurrence.smoke.size()>(std::numeric_limits<std::uint32_t>::max()-nextControlId_)/2u)return false;
            occurrence.dust=std::make_unique<rr::game::OriginalWreckDustOwner50LA>();
            auto& dust=*occurrence.dust;
            const auto result=owner.constructFreshDust2CB7E0(dust,
                binding<rr::game::OriginalWreckDustBinding50LA>(dust,app,
                    nextControlId_+2u*static_cast<std::uint32_t>(occurrence.smoke.size())),actualDirectory36B530);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x19a7d0u)return false;
            freshDustComplete_=true;return true; //PreConfig; Dust is not appended or registered.
        }catch(...){return false;}
    }
    bool freshReadMember(rr::assets::OriginalStandardArchiveProvider50LA& provider,
        const char* actualFilename,std::vector<std::uint8_t>& bytes) {
        //Each reached Config transport authenticates its actual filename anew.
        //Already borrowed byte vectors never reallocate or silently select another member.
        rr::assets::OriginalStandardArchiveProvider50LA::MemberView member;
        if(!actualFilename||provider.lookup132820(actualFilename,member)!=
           rr::assets::OriginalStandardArchiveProvider50LA::Lookup::present||
           !member.data||!member.size||member.size>65536u)return false;
        if(bytes.empty()){bytes.assign(member.data,member.data+member.size);return true;}
        return bytes.size()==member.size&&std::equal(bytes.begin(),bytes.end(),member.data);
    }
    bool continueFreshSecondPairs(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb6ccu;
        std::size_t index=0;std::uint32_t count=0,currentPC=0;
        if(!freshInputsOnly_||externalRetired_||
           !provider.sealed()||
           !canRetireFreshInputContext()||!owner.freshEffectCursor(index,count,currentPC,1u)||
           index!=1u||index>=count||currentPC!=0x2cb6ccu||pc_!=currentPC||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialChildSmoke||!occurrence.freshFirstPairStopped||
           occurrence.freshRemainingPairsAttempted||occurrence.smoke.size()!=1u||occurrence.fire.size()!=1u)return false;
        const auto sourceBaseId=occurrence.smoke[0]->binding_.controlOwnerId;
        if(sourceBaseId==std::numeric_limits<std::uint32_t>::max())return false;
        occurrence.freshRemainingPairsAttempted=true;
        try {
            while(index<count){
                if(occurrence.smoke.size()!=index||occurrence.fire.size()!=index||
                   !canRetireFreshInputContext()||index>(std::numeric_limits<std::uint32_t>::max()-sourceBaseId-1u)/2u)return false;
                pc=pc_=0x2cb6ccu;
                occurrence.smoke.push_back(std::make_unique<rr::game::OriginalWreckSmokeOwner50LA>());
                auto& smoke=*occurrence.smoke.back();
                auto result=owner.constructFreshSmoke2CB6CC(smoke,
                    binding<rr::game::OriginalWreckSmokeBinding50LA>(smoke,app,sourceBaseId+2u*static_cast<std::uint32_t>(index)),
                    actualDirectory36B530,1u);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::stopped||pc!=0x19a7d0u)return false;
                pc=pc_=0x15ae2cu;
                if(!smoke.freshFilenameKnown_||smoke.freshFilenameLength_>=smoke.freshFilename78_.size()||
                   smoke.freshFilename78_[smoke.freshFilenameLength_]||
                   !freshReadMember(provider,smoke.freshFilename78_.data(),bytes_[0]))return false;
                if(context_.podWreckSmokeEmt!=&bytes_[0])return false;
                context_.podWreckSmokeEmt=&bytes_[0];
                rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
                result=owner.readAppendFreshSmoke19A7D0(smoke,bytes_[0],diagnostic,1u);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::effectFireRequired||pc!=0x2cb720u)return false;
                occurrence.fire.push_back(std::make_unique<rr::game::OriginalWreckFireOwner50LA>());
                auto& fire=*occurrence.fire.back();
                result=owner.constructFreshFire2CB720(fire,
                    binding<rr::game::OriginalWreckFireBinding50LA>(fire,app,sourceBaseId+2u*static_cast<std::uint32_t>(index)+1u),
                    actualDirectory36B530,1u);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::stopped||pc!=0x15e024u)return false;
                pc=pc_=0x15ae2cu;
                if(!fire.freshFilenameKnown_||fire.freshFilenameLength_>=fire.freshFilename78_.size()||
                   fire.freshFilename78_[fire.freshFilenameLength_]||
                   !freshReadMember(provider,fire.freshFilename78_.data(),bytes_[1]))return false;
                if(context_.podFireEmt!=&bytes_[1])return false;
                context_.podFireEmt=&bytes_[1];
                result=owner.readAppendFreshFire15E024(fire,bytes_[1],diagnostic,1u);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::stopped||pc!=0x2cb778u)return false;
                result=owner.stopFreshPair2CB778(1u);pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(!((result.status==S::effectNextPairRequired&&pc==0x2cb6ccu)||
                     (result.status==S::effectDustRequired&&pc==0x2cb7e0u)))return false;
                std::size_t reached=0;std::uint32_t actualCount=0,reachedPC=0;
                if(!owner.freshEffectCursor(reached,actualCount,reachedPC,1u)||reached!=index+1u||
                   actualCount!=count||reachedPC!=pc)return false;
                index=reached; //Only the source Stop/loop updates this read-through cursor.
            }
            if(pc!=0x2cb7e0u||!canRetireFreshInputContext())return false;
            occurrence.freshRemainingPairsComplete=true;return true;
        }catch(...){return false;}
    }
    bool continueFreshPairs(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,
        const std::string* actualDirectory36B530,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb6ccu;
        std::size_t index=0;std::uint32_t count=0,currentPC=0;
        if(!freshInputsOnly_||!freshFirstPairStopped_||freshRemainingPairsAttempted_||externalRetired_||
           !provider.sealed()||nextControlId_==std::numeric_limits<std::uint32_t>::max()||
           !canRetireFreshInputContext()||!owner.freshEffectCursor(index,count,currentPC)||
           index!=1u||index>=count||currentPC!=0x2cb6ccu||pc_!=currentPC||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=1u)return false;
        freshRemainingPairsAttempted_=true;
        auto& occurrence=*collections_[0]->occurrences[0];
        try {
            while(index<count){
                if(occurrence.smoke.size()!=index||occurrence.fire.size()!=index||
                   !canRetireFreshInputContext()||index>(std::numeric_limits<std::uint32_t>::max()-nextControlId_-1u)/2u)return false;
                pc=pc_=0x2cb6ccu;
                occurrence.smoke.push_back(std::make_unique<rr::game::OriginalWreckSmokeOwner50LA>());
                auto& smoke=*occurrence.smoke.back();
                auto result=owner.constructFreshSmoke2CB6CC(smoke,
                    binding<rr::game::OriginalWreckSmokeBinding50LA>(smoke,app,nextControlId_+2u*static_cast<std::uint32_t>(index)),
                    actualDirectory36B530);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::stopped||pc!=0x19a7d0u)return false;
                pc=pc_=0x15ae2cu;
                if(!smoke.freshFilenameKnown_||smoke.freshFilenameLength_>=smoke.freshFilename78_.size()||
                   smoke.freshFilename78_[smoke.freshFilenameLength_]||
                   !freshReadMember(provider,smoke.freshFilename78_.data(),bytes_[0]))return false;
                context_.podWreckSmokeEmt=&bytes_[0];
                rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
                result=owner.readAppendFreshSmoke19A7D0(smoke,bytes_[0],diagnostic);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::effectFireRequired||pc!=0x2cb720u)return false;
                occurrence.fire.push_back(std::make_unique<rr::game::OriginalWreckFireOwner50LA>());
                auto& fire=*occurrence.fire.back();
                result=owner.constructFreshFire2CB720(fire,
                    binding<rr::game::OriginalWreckFireBinding50LA>(fire,app,nextControlId_+2u*static_cast<std::uint32_t>(index)+1u),
                    actualDirectory36B530);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::stopped||pc!=0x15e024u)return false;
                pc=pc_=0x15ae2cu;
                if(!fire.freshFilenameKnown_||fire.freshFilenameLength_>=fire.freshFilename78_.size()||
                   fire.freshFilename78_[fire.freshFilenameLength_]||
                   !freshReadMember(provider,fire.freshFilename78_.data(),bytes_[1]))return false;
                context_.podFireEmt=&bytes_[1];
                result=owner.readAppendFreshFire15E024(fire,bytes_[1],diagnostic);
                pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(result.status!=S::stopped||pc!=0x2cb778u)return false;
                result=owner.stopFreshPair2CB778();pc=pc_=result.sourcePC?result.sourcePC:pc;
                if(!((result.status==S::effectNextPairRequired&&pc==0x2cb6ccu)||
                     (result.status==S::effectDustRequired&&pc==0x2cb7e0u)))return false;
                std::size_t reached=0;std::uint32_t actualCount=0,reachedPC=0;
                if(!owner.freshEffectCursor(reached,actualCount,reachedPC)||reached!=index+1u||
                   actualCount!=count||reachedPC!=pc)return false;
                index=reached; //Only the source Stop/loop updates this read-through cursor.
            }
            if(pc!=0x2cb7e0u||!canRetireFreshInputContext())return false;
            freshRemainingPairsComplete_=true;return true;
        }catch(...){return false;}
    }
    bool stopFreshSecondFirstPair(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb778u;
        if(!freshInputsOnly_||externalRetired_||pc_!=pc||!canRetireFreshInputContext()||
           collections_.size()!=1u||collections_[0]->occurrences.size()!=2u)return false;
        auto& occurrence=*collections_[0]->occurrences[1];
        if(!occurrence.freshInitialChildSmoke||!occurrence.freshSuccessorCallerComplete||
           occurrence.freshFirstPairStopAttempted||occurrence.smoke.size()!=1u||occurrence.fire.size()!=1u)return false;
        occurrence.freshFirstPairStopAttempted=true;
        const auto result=owner.stopFreshFirstPair2CB778(1u);pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(!((result.status==S::effectNextPairRequired&&pc==0x2cb6ccu)||
             (result.status==S::effectDustRequired&&pc==0x2cb7e0u)))return false;
        occurrence.freshFirstPairStopped=true;return true;
    }
    bool stopFreshFirstPair(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb778u;
        if(!freshInputsOnly_||!freshFireReadComplete_||freshFirstPairStopAttempted_||externalRetired_||
           pc_!=0x2cb778u||!canRetireFreshInputContext())return false;
        freshFirstPairStopAttempted_=true;
        const auto result=owner.stopFreshFirstPair2CB778();pc=pc_=result.sourcePC?result.sourcePC:pc;
        if(!((result.status==S::effectNextPairRequired&&pc==0x2cb6ccu)||
             (result.status==S::effectDustRequired&&pc==0x2cb7e0u)))return false;
        freshFirstPairStopped_=true;return true;
    }
    bool readAppendFreshFire(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        pc=0x15e024u;auto* fire=freshFireOwner();
        if(!freshInputsOnly_||!freshFireComplete_||freshFireReadAttempted_||externalRetired_||
           !provider.sealed()||!fire||!canRetireFreshInputContext()||
           !fire->freshFilenameKnown_||fire->freshFilenameLength_>=fire->freshFilename78_.size()||
           fire->freshFilename78_[fire->freshFilenameLength_])return false;
        freshFireReadAttempted_=true;pc=pc_=0x15ae2cu;
        try {
            //Actual copied+appended filename selects the same retained provider.
            if(!readMember(provider,fire->freshFilename78_.data(),bytes_[1]))return false;
            context_.podFireEmt=&bytes_[1];
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readAppendFreshFire15E024(*fire,bytes_[1],diagnostic);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=rr::game::OriginalWreckConstructorPrefixStatus50LA::stopped||pc!=0x2cb778u)return false;
            freshFireReadComplete_=true;return true;
        }catch(...){return false;}
    }
    bool constructFreshFire(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::string* actualDirectory36B530,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb720u;
        rr::game::OriginalWreckSmokeOwner50LA* smoke=nullptr;
        if(!freshInputsOnly_||!freshSmokeReadComplete_||freshFireAttempted_||externalRetired_||
           !freshSmokeLayout(smoke)||!smoke||!smoke->appended_||freshFireOwner()||
           !canRetireFreshInputContext()||nextControlId_==std::numeric_limits<std::uint32_t>::max())return false;
        freshFireAttempted_=true;
        try {
            auto& occurrence=*collections_[0]->occurrences[0];
            occurrence.fire.push_back(std::make_unique<rr::game::OriginalWreckFireOwner50LA>());
            auto& fire=*occurrence.fire[0];
            const auto result=owner.constructFreshFire2CB720(fire,
                binding<rr::game::OriginalWreckFireBinding50LA>(fire,app,nextControlId_+1u),actualDirectory36B530);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x15e024u)return false;
            freshFireComplete_=true;return true; //Pre Config, unappended and unregistered.
        }catch(...){return false;}
    }
    bool readAppendFreshSmoke(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        pc=0x19a7d0u;rr::game::OriginalWreckSmokeOwner50LA* smoke=nullptr;
        if(!freshInputsOnly_||!freshSmokeComplete_||freshSmokeReadAttempted_||externalRetired_||
           !provider.sealed()||!freshSmokeLayout(smoke)||!smoke||!canRetireFreshInputContext()||
           !smoke->freshFilenameKnown_||smoke->freshFilenameLength_>=smoke->freshFilename78_.size()||
           smoke->freshFilename78_[smoke->freshFilenameLength_])return false;
        freshSmokeReadAttempted_=true;pc=pc_=0x15ae2cu;
        try {
            //Actual copied+appended filename78 selects the SAME retained provider.
            //Native lookup refusal is NOT a fabricated Config load-failure result.
            if(!readMember(provider,smoke->freshFilename78_.data(),bytes_[0]))return false;
            context_.podWreckSmokeEmt=&bytes_[0];
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            const auto result=owner.readAppendFreshSmoke19A7D0(*smoke,bytes_[0],diagnostic);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=rr::game::OriginalWreckConstructorPrefixStatus50LA::effectFireRequired||pc!=0x2cb720u)return false;
            freshSmokeReadComplete_=true;return true;
        }catch(...){return false;}
    }
    bool constructFreshSmoke(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,const std::string* actualDirectory36B530,
        std::uint32_t& pc)noexcept {
        using S=rr::game::OriginalWreckConstructorPrefixStatus50LA;pc=0x2cb6ccu;
        if(!freshInputsOnly_||freshSmokeAttempted_||terminal_||externalRetired_||
           !collections_.empty()||!canRetireFreshInputContext())return false;
        freshSmokeAttempted_=true;terminal_=true;
        try {
            collections_.push_back(std::make_unique<CollectionEffects>());
            auto& selected=*collections_[0];selected.occurrences.push_back(std::make_unique<Occurrence>());
            auto& occurrence=*selected.occurrences[0];occurrence.smoke.push_back(std::make_unique<rr::game::OriginalWreckSmokeOwner50LA>());
            auto& smoke=*occurrence.smoke[0];
            const auto result=owner.constructFreshSmoke2CB6CC(smoke,
                binding<rr::game::OriginalWreckSmokeBinding50LA>(smoke,app,nextControlId_),actualDirectory36B530);
            pc=pc_=result.sourcePC?result.sourcePC:pc;
            if(result.status!=S::stopped||pc!=0x19a7d0u)return false;
            freshSmokeComplete_=true;return true; //Before Config15AE00, no append or manager row.
        }catch(...){return false;}
    }
    bool readMember(rr::assets::OriginalStandardArchiveProvider50LA& provider,
        const char* name,std::vector<std::uint8_t>& out) {
        if(!out.empty())return true; //same retained immutable member, no replay/reallocation
        rr::assets::OriginalStandardArchiveProvider50LA::MemberView member;
        if(provider.lookup132820(name,member)!=rr::assets::OriginalStandardArchiveProvider50LA::Lookup::present||
           !member.data||!member.size||member.size>65536u)return false;
        // Same authenticated retained provider resolution, no disc reopen or
        // replacement archive. Per-owner cold readers validate the real EMT.
        out.assign(member.data,member.data+member.size);return true;
    }
    template<class Binding,class Child>
    Binding binding(Child& child,rr::game::OriginalApplicationEntities50LA& app,std::uint32_t id)noexcept {
        Binding b;b.domain=&forest_;b.domainStorage=forestStorage_.data();b.domainCapacity=forestStorage_.size();
        b.particleParent=sourceParent_;b.actualObjectCount423F60=&app.objectCount423F60();
        b.actualAttachmentLock423F58=&app.attachmentLock423F58();
        b.componentKey=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&child.component()));
        // Native identity within this actual fresh external forest only. Not
        // guest pointer bits, a global dispatch registration, or another pool.
        b.controlOwnerId=id;return b;
    }
    bool load(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,rr::game::OriginalRacerEntityCold50LA& racer,
        rr::game::OriginalTrackParticleCold50LA& manager,G* const* parent7FC,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,const std::uint32_t& water,
        std::size_t pieceIndex,std::uint32_t& pc)noexcept {
        using namespace rr::game;
        pc=pc_;
        const auto ci=owner.nativeCollectionOrdinal();
        if(freshInputsOnly_||terminal_||externalRetired_||ci>collections_.size()||!racer.prepared()||!racer.postOverlayConstructed210A20()||
           !parent7FC||*parent7FC!=&racer.particle7FC||!manager.prepared()||!manager.particles()||
           !provider.sealed()||app.attachmentLock423F58()||racer.racer.reference00!=1u||
           racer.racer.parent1C||racer.racer.previous10||racer.racer.next14||
           racer.particle7FC.reference00!=1u||racer.particle7FC.kind!=OriginalWreckComponentKind50LA::entity141300||
           racer.particle7FC.parent1C!=&racer.racer||(!attempted_&&racer.particle7FC.firstChild18)||
           racer.particle7FC.sessionGeneration!=racer.racer.sessionGeneration||
           racer.racer.nativeEffectControlId||racer.particle7FC.nativeEffectControlId)return false;
        if(attempted_&&(sourceManager4A0_!=&manager||sourceParentField_!=parent7FC||
            sourceParent_!=*parent7FC||context_.particleManager!=manager.particles()))return false;
        if(ci<collections_.size()){
            const auto& selected=*collections_[ci];
            if(ci+1u!=collections_.size()||selected.groupAttempted||selected.retired||pieceIndex!=selected.completed)return false;
        }else if(pieceIndex||(ci&&(!collections_[ci-1u]->deactivated||collections_[ci-1u]->retired)))return false;
        terminal_=true;
        if(!attempted_){
        attempted_=true;sourceManager4A0_=&manager;
        context_.particleManager=manager.particles();sourceContext_=&context_;
        sourceParentField_=parent7FC;sourceParent_=*parent7FC;
        forestStorage_[0]=&racer.racer;forestStorage_[1]=sourceParent_;
        forest_={racer.racer.sessionGeneration,forestStorage_.data(),2};
        }
        auto step=[&](OriginalWreckConstructorPrefixResult50LA r,S wanted){pc=pc_=r.sourcePC;return r.status==wanted;};
        try {
            if(ci==collections_.size())collections_.push_back(std::make_unique<CollectionEffects>());
            auto& selected=*collections_[ci];auto& occurrences_=selected.occurrences;
            occurrences_.push_back(std::make_unique<Occurrence>());
            auto& occurrence=*occurrences_.back();
            auto& smoke_=occurrence.smoke;auto& fire_=occurrence.fire;
            auto& dust_=occurrence.dust;auto& splash_=occurrence.splash;
            if(!step(owner.constructOccurrenceMutualAndDerived(ci,pieceIndex),S::effectInputsRequired))return false;
            if(!step(owner.bindOccurrenceEffectConstructorInputs(ci,pieceIndex,{&sourceContext_,sourceParentField_}),S::effectConstructorRequired))return false;
            const auto* collection=owner.collection(ci);const auto* piece=collection?collection->piece(pieceIndex):nullptr;
            if(!piece||!piece->effectCountCommitted())return false;
            const auto count=piece->effectCount174();
            if(forest_.count>forestStorage_.size()||forestStorage_.size()-forest_.count<2u||
               count>(forestStorage_.size()-forest_.count-2u)/2u)return false;
            const auto controlBase=nextControlId_;nextControlId_+=2u*count+2u;
            const auto arrays=owner.constructOccurrencePositiveEffectArrays(ci,pieceIndex);pc=pc_=arrays.sourcePC;
            if(arrays.status!=(count?S::effectChildrenRequired:S::effectDustRequired))return false;
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            if(count){
                if(!readMember(provider,"particle\\podWreckSmoke.emt",bytes_[0])||
                   !readMember(provider,"particle\\podFire.emt",bytes_[1]))return false;
                context_.podWreckSmokeEmt=&bytes_[0];context_.podFireEmt=&bytes_[1];
                smoke_.resize(count);fire_.resize(count);
                for(std::size_t i=0;i<count;++i){
                    smoke_[i]=std::make_unique<OriginalWreckSmokeOwner50LA>();
                    if(!smoke_[i]->prepareCold(context_,diagnostic)||
                       !step(owner.constructOccurrencePositiveSmoke(ci,pieceIndex,*smoke_[i],
                           binding<OriginalWreckSmokeBinding50LA>(*smoke_[i],app,controlBase+static_cast<std::uint32_t>(2u*i))),S::effectFireRequired))return false;
                    fire_[i]=std::make_unique<OriginalWreckFireOwner50LA>();
                    if(!fire_[i]->prepareCold(context_,diagnostic)||
                       !step(owner.constructOccurrencePositiveFireAndStops(ci,pieceIndex,*smoke_[i],*fire_[i],
                           binding<OriginalWreckFireBinding50LA>(*fire_[i],app,controlBase+static_cast<std::uint32_t>(2u*i+1u))),
                           i+1u<count?S::effectNextPairRequired:S::effectDustRequired))return false;
                }
            }
            if(!readMember(provider,"particle\\podWreckDust.emt",bytes_[2]))return false;
            context_.podWreckDustEmt=&bytes_[2];dust_=std::make_unique<OriginalWreckDustOwner50LA>();
            if(!dust_->prepareCold(context_,diagnostic)||
               !step(owner.constructOccurrencePositiveDust(ci,pieceIndex,*dust_,
                   binding<OriginalWreckDustBinding50LA>(*dust_,app,controlBase+2u*count)),S::effectSplashRequired))return false;
            if(!readMember(provider,"particle\\podWreckSplash.emt",bytes_[3]))return false;
            context_.podWreckSplashEmt=&bytes_[3];splash_=std::make_unique<OriginalWreckSplashOwner50LA>();
            if(!splash_->prepareCold(context_,diagnostic)||
               !step(owner.constructOccurrencePositiveSplash(ci,pieceIndex,*dust_,*splash_,
                   binding<OriginalWreckSplashBinding50LA>(*splash_,app,controlBase+2u*count+1u)),S::effectReturnRequired))return false;
            if(!step(owner.appendOccurrencePiece22C1B8(ci,pieceIndex),S::collectionFiltersRequired))return false;
            const auto result=owner.applyOccurrenceCollectionWaterCategories(ci,pieceIndex,water);pc=pc_=result.sourcePC;
            if(result.status!=S::nextPieceRequired&&result.status!=S::groupConstructorRequired)return false;
            ++selected.completed;terminal_=false;return true;
        }catch(...){return false;} // Actual constructor/manager/array prefixes remain owned.
    }
    bool loadGroup(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,std::uint32_t& pc)noexcept {
        const auto ci=owner.nativeCollectionOrdinal();
        if(ci>=collections_.size()||ci+1u!=collections_.size())return false;
        auto& selected=*collections_[ci];auto& sparks_=selected.sparks;auto& explosion_=selected.explosion;
        auto& groupAttempted_=selected.groupAttempted;
        if(!attempted_||terminal_||externalRetired_||groupAttempted_||!sourceManager4A0_||
           !sourceManager4A0_->prepared()||sourceManager4A0_->particles()!=context_.particleManager||
           !sourceParentField_||*sourceParentField_!=sourceParent_||!provider.sealed())return false;
        groupAttempted_=true;terminal_=true;pc=pc_=0x2cc0b4u;
        if(forest_.count>forestStorage_.size()||forestStorage_.size()-forest_.count<2u)return false;
        const auto controlBase=nextControlId_;nextControlId_+=2u;
        try {
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            if(!readMember(provider,"particle\\podWreckSparks.emt",bytes_[4]))return false;
            context_.podWreckSparksEmt=&bytes_[4];sparks_=std::make_unique<rr::game::OriginalWreckGroupSparksOwner50LA>();
            if(!sparks_->prepareCold(context_,diagnostic)||
               !owner.constructNativeGroupEffect(*sparks_,binding<rr::game::OriginalWreckGroupSparksBinding50LA>(*sparks_,app,controlBase),false,pc))return false;
            pc_=pc;
            if(!readMember(provider,"particle\\podExplosion.emt",bytes_[5]))return false;
            context_.podExplosionEmt=&bytes_[5];explosion_=std::make_unique<rr::game::OriginalWreckGroupExplosionOwner50LA>();
            if(!explosion_->prepareCold(context_,diagnostic)||
               !owner.constructNativeGroupEffect(*explosion_,binding<rr::game::OriginalWreckGroupExplosionBinding50LA>(*explosion_,app,controlBase+1u),true,pc))return false;
            pc_=pc;
            if(!owner.appendNativeGroup22C2C0(pc))return false;
            pc_=pc;return true; //no later piece or group retry; virtual18 is still required
        }catch(...){return false;}
    }
    bool deactivateLoad22CE10(rr::game::OriginalWreckRacerStorage50LA& owner,std::uint32_t& pc)noexcept {
        const auto ci=owner.nativeCollectionOrdinal();
        if(ci>=collections_.size()||ci+1u!=collections_.size())return false;
        auto& selected=*collections_[ci];auto& sparks_=selected.sparks;auto& explosion_=selected.explosion;
        auto& occurrences_=selected.occurrences;auto& deactivateAttempted_=selected.deactivateAttempted;
        if(!selected.groupAttempted||deactivateAttempted_||externalRetired_||pc_!=0x22c2c8u||
           !sparks_||!explosion_||occurrences_.empty()||occurrences_.size()!=selected.completed||
           !occurrences_[0]||!occurrences_[0]->splash||!sourceManager4A0_||!sourceManager4A0_->prepared()||
           sourceManager4A0_->particles()!=context_.particleManager||!sourceParentField_||
           *sourceParentField_!=sourceParent_)return false;
        deactivateAttempted_=true;
        if(!owner.beginNativeLoadDeactivate22CE10(pc))return false;
        if(!owner.deactivateNativeGroup2CC9C0(*sparks_,*explosion_,*occurrences_[0]->splash,pc))return false;
        for(std::size_t i=0;i<occurrences_.size();++i){auto& p=*occurrences_[i];
            if(!p.dust||!p.splash||!owner.deactivateNativePiece2CBEB0(i,p.smoke,p.fire,*p.dust,*p.splash,pc)||
               !owner.finishNativePieceLoadDeactivate(i,pc))return false;
        }
        if(!owner.finishNativeCollectionLoad22CF18(pc))return false;
        selected.deactivated=true;terminal_=false;pc_=pc;return true;
    }
    template<class Publish> bool loadRacer814810(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,
        rr::assets::OriginalStandardArchiveProvider50LA& provider,Publish&& publish,std::uint32_t& pc)noexcept {
        pc=0x212b50u;
        if(racerTailAttempted_||terminal_||externalRetired_||!owner.nativeCollectionsReached212B50()||
           collections_.size()!=owner.nativeCollectionOrdinal()+1u||!retirementContextCurrent()||
           !provider.sealed()||app.attachmentLock423F58())return false;
        for(const auto& c:collections_)if(!c||!c->deactivated||c->retired)return false;
        racerTailAttempted_=true;terminal_=true;
        try {
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            pc=pc_=0x212b5cu;
            if(forest_.count>=forestStorage_.size())return false;
            if(!readMember(provider,"particle\\podExplWreckExplosion.emt",bytes_[6]))return false;
            context_.podExplWreckExplosionEmt=&bytes_[6];
            racerExplosion814_=std::make_unique<rr::game::OriginalWreckGroupExplosionOwner50LA>();
            auto& explosion=*racerExplosion814_;
            if(!explosion.prepareNativeRacerCold(context_,diagnostic)||
               !explosion.constructSource(binding<rr::game::OriginalWreckGroupExplosionBinding50LA>(explosion,app,nextControlId_++)))return false;
            pc=pc_=0x212b78u;
            if(!publish(true,&explosion.component_))return false;racerTailPublished_[0]=true;
            const bool explosionDone=explosion.setupAppendStopDisableNativeRacer();pc=pc_=explosion.sourceBoundary();
            if(!explosionDone)return false;
            pc=pc_=0x212bd0u;
            if(forest_.count>=forestStorage_.size())return false;
            if(!readMember(provider,"particle\\podExplWreckSparks.emt",bytes_[7]))return false;
            context_.podExplWreckSparksEmt=&bytes_[7];
            racerSparks810_=std::make_unique<rr::game::OriginalWreckGroupSparksOwner50LA>();
            auto& sparks=*racerSparks810_;
            if(!sparks.prepareNativeRacerCold(context_,diagnostic)||
               !sparks.constructSource(binding<rr::game::OriginalWreckGroupSparksBinding50LA>(sparks,app,nextControlId_++)))return false;
            pc=pc_=0x212becu;
            if(!publish(false,&sparks.component_))return false;racerTailPublished_[1]=true;
            const bool sparksDone=sparks.setupAppendStopDisableNativeRacer();pc=pc_=sparks.sourceBoundary();
            if(!sparksDone)return false;
            racerTailComplete_=true;return true; //212C44 actual818 ember allocation remains required
        }catch(...){return false;}
    }
    template<class Publish> bool loadRacer818(rr::game::OriginalWreckRacerStorage50LA& owner,
        rr::game::OriginalApplicationEntities50LA& app,rr::assets::OriginalStandardArchiveProvider50LA& provider,
        const std::string& directory,std::uint32_t& actualRaw,Publish&& publish,std::uint32_t& pc)noexcept{
        pc=pc_;
        if(!racerTailComplete_||pc_!=0x212c44u||emberAttempted_||externalRetired_||
           !owner.nativeCollectionsReached212B50()||!retirementContextCurrent()||!provider.sealed()||
           app.attachmentLock423F58()||directory.size()>230u||std::fegetround()!=FE_TONEAREST)return false;
        emberAttempted_=true;terminal_=true;
        try{
            rr::assets::OriginalUiPageDiagnostic50KG diagnostic;
            for(std::size_t i=0;i<embers818_.size();++i){
                pc=pc_=i?0x212ce0u:0x212c44u;
                // Sequential native admission, not ten speculative guest owners.
                if(forest_.count>=forestStorage_.size())return false;
                embers818_[i]=std::make_unique<rr::game::OriginalRacerEmberOwner50LA>();
                auto& child=*embers818_[i];
                if(!i){
                    if(!readMember(provider,(directory+"podEmberFire.emt").c_str(),bytes_[8])||
                       !readMember(provider,(directory+"podEmberSmoke.emt").c_str(),bytes_[9])||
                       !child.prepareCold(bytes_[8],bytes_[9],diagnostic))return false;
                }else if(!embers818_[0]||!child.prepareClone(*embers818_[0]))return false;
                const auto id=nextControlId_;nextControlId_+=2;
                if(!child.construct19BA50(binding<rr::game::OriginalWreckSmokeBinding50LA>(child,app,id)))return false;
                pc=pc_=i?0x212cfcu:0x212c74u;
                if(!publish(i,&child.component_))return false;++emberPublished_;
                const bool loaded=i?child.loadClone(*embers818_[0],id+1,actualRaw):
                    child.loadFirst(*context_.particleManager,directory,id+1,actualRaw);
                pc=pc_=child.pc_;if(!loaded)return false;
                const bool ended=child.appendStopDisable();pc=pc_=child.pc_;if(!ended)return false;
            }
            emberComplete_=true;pc=pc_=0x212dc8u;return true;
        }catch(...){return false;}
    }
    bool emberAliasesCurrent(rr::game::OriginalWreckComponent50LA* const* slots,std::size_t published)const noexcept{
        if(!slots||published!=emberPublished_||published>embers818_.size())return false;
        for(std::size_t i=0;i<published;++i)
            if(!embers818_[i]||slots[i]!=&embers818_[i]->component_)return false;
        // Source array constructor wrote actual NULLs for every unused slot.
        for(std::size_t i=published;i<embers818_.size();++i)if(slots[i])return false;
        return true;
    }
    bool retireEmbers()noexcept{
        for(std::size_t i=embers818_.size();i>0;--i)
            if(embers818_[i-1]&&!embers818_[i-1]->retireTail())return false;
        return true;
    }
    bool racerTailAliasesCurrent(const std::optional<G*>& explosion,const std::optional<G*>& sparks)const noexcept {
        const std::optional<G*>* aliases[]={&explosion,&sparks};
        const G* owners[]={racerExplosion814_?&racerExplosion814_->component():nullptr,
            racerSparks810_?&racerSparks810_->component():nullptr};
        for(unsigned i=0;i<2u;++i){
            if(racerTailPublished_[i]){if(!aliases[i]->has_value()||!owners[i]||aliases[i]->value()!=owners[i])return false;}
            else if(aliases[i]->has_value())return false;
        }
        return true; //identity only; retained source aliases remain history after actual drops
    }
    bool retireRacerTail()noexcept {
        using K=rr::game::OriginalWreckComponentKind50LA;
        auto retire=[](auto& owned){
            if(!owned)return true;const auto& c=owned->component();
            if(!c.reference00&&c.kind==K::unbound)return !c.parent1C&&!c.previous10&&!c.next14&&!c.firstChild18;
            return owned->retireTail1424E0();
        };
        return retireEmbers()&&retire(racerSparks810_)&&retire(racerExplosion814_);
    }
    bool retirementContextCurrent()const noexcept {
        return sourceManager4A0_&&sourceManager4A0_->prepared()&&
            sourceManager4A0_->particles()==context_.particleManager&&sourceParent_&&
            sourceParentField_&&*sourceParentField_==sourceParent_;
    }
    bool retireCollectionExternal(std::size_t ci)noexcept {
        using K=rr::game::OriginalWreckComponentKind50LA;
        if(ci>=collections_.size()||!retirementContextCurrent())return false;
        auto& selected=*collections_[ci];if(selected.retired)return true;
        for(std::size_t n=ci+1u;n<collections_.size();++n)if(!collections_[n]->retired)return false;
        auto retire=[](auto& owned){
            if(!owned)return true;
            const auto& c=owned->component();
            if(!c.reference00&&c.kind==K::unbound)return !c.parent1C&&!c.previous10&&!c.next14&&!c.firstChild18;
            return owned->retireTail1424E0();
        };
        if(!retire(selected.explosion)||!retire(selected.sparks))return false;
        for(std::size_t n=selected.occurrences.size();n>0;--n){auto& occurrence=*selected.occurrences[n-1u];
            if(!retire(occurrence.splash)||!retire(occurrence.dust))return false;
            for(std::size_t i=std::max(occurrence.smoke.size(),occurrence.fire.size());i>0;--i){
                if(i<=occurrence.fire.size()&&!retire(occurrence.fire[i-1u]))return false;
                if(i<=occurrence.smoke.size()&&!retire(occurrence.smoke[i-1u]))return false;
            }
        }
        selected.retired=true;return true;
    }
    bool retireDetachedTail(rr::game::OriginalWreckRacerStorage50LA& owner)noexcept {
        if(!attempted_||externalRetired_)return true;
        //Only unpublished-to-Racer trailing collections cancel their own7FC
        //suffix first. Earlier attached collection effects remain borrowed live.
        for(std::size_t n=collections_.size();n>0;--n){
            if(owner.nativeCollectionAppendedToRacer(n-1u))break;
            if(!retireCollectionExternal(n-1u))return false;
        }
        return true;
    }
    bool retireExternal()noexcept {
        if(!attempted_||externalRetired_)return true;
        if(!retirementContextCurrent())return false;
        if(!retireRacerTail())return false;
        for(std::size_t n=collections_.size();n>0;--n)if(!retireCollectionExternal(n-1u))return false;
        if(forest_.count!=2u||sourceParent_->firstChild18)return false;
        externalRetired_=true;return true;
    }

public:
    NativeWreckEffectChildren()=default;
    ~NativeWreckEffectChildren(){if(attempted_&&!externalRetired_)std::terminate();}
    NativeWreckEffectChildren(const NativeWreckEffectChildren&)=delete;
    NativeWreckEffectChildren& operator=(const NativeWreckEffectChildren&)=delete;
};
}

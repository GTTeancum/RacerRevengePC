#pragma once
#include "original_racer_model_scene50la.h"
#include <exception>
#include <memory>
#include <optional>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
// Source2CB5A0 arrays only. External particle children are owned by their
//actual forest; allocating these pointer slots does not construct a child.
class OriginalWreckEffectArrays50LA final {
    friend class OriginalWreckRacerStorage50LA;
    using O=OriginalRacerSceneObject50LA;
    std::unique_ptr<OriginalWreckComponent50LA*[]> smoke160_,fire164_;
    std::unique_ptr<const O*[]> anchors170_;
    std::size_t count_=0,anchorsWritten_=0,smokeWritten_=0,fireWritten_=0,pairsStopped_=0;
    // Same actual external forest for every source loop iteration. These are
    // native borrows only;2CB500 never retires that forest or its children.
    OriginalWreckComponentDomain50LA* externalDomain_=nullptr;
    OriginalWreckComponent50LA** externalStorage_=nullptr;
    std::size_t externalCapacity_=0;
    bool attempted_=false,retired_=false;
    std::uint32_t boundary_=0;
    //Only previously absent source aliases. SAME allocations remain authoritative.
    //Unknown until the actual fresh2CB4B0 stores, then retained after free.
    std::optional<OriginalWreckComponent50LA**> source160_,source164_;
    std::optional<const O**> source170_;
    bool freshRoute_=false;
    //Source s1 byte-offset is distinct from the existing pairsStopped_ (s0).
    //Unknown before actual loop initialization2CB6C8; retained after free.
    std::optional<std::uint32_t> freshPairByteOffset_;
    std::optional<std::size_t> externalFirstSlot_; //Native extent of SAME counted forest, never a source pointer copy.
    const OriginalRacerModelSceneOwner50LA* freshModel_=nullptr;
    decltype(SceneGraph49P{}.nodes) freshGraph_=nullptr;
    bool freshAliasesCurrent()const noexcept {
        return freshRoute_&&!retired_&&source160_&&source164_&&source170_&&
            *source160_==smoke160_.get()&&*source164_==fire164_.get()&&*source170_==anchors170_.get();
    }
    //After all external children retire, a later occurrence's native slice
    //starts above domain.count. Authenticate the entire earlier SOURCE slice;
    //do not relax the lower bound based on a receipt or a copied pointer list.
    bool retiredPriorSliceCurrent(const OriginalWreckEffectArrays50LA* prior,
        const OriginalWreckComponent50LA* priorDust,const OriginalWreckComponent50LA* priorSplash)const noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!prior||prior==this||!priorDust||!priorSplash||!externalFirstSlot_||
           !externalDomain_||!externalStorage_||externalDomain_->objects!=externalStorage_||
           externalDomain_->count!=2u||externalCapacity_<2u||
           !externalStorage_[0]||!externalStorage_[1]||
           externalStorage_[0]->reference00!=1u||externalStorage_[1]->reference00!=1u||
           externalStorage_[0]->sessionGeneration!=externalDomain_->sessionGeneration||
           externalStorage_[1]->sessionGeneration!=externalDomain_->sessionGeneration||
           externalStorage_[1]->parent1C!=externalStorage_[0]||externalStorage_[1]->firstChild18||
           !prior->preflightFreshArrays(nullptr,nullptr,priorDust,priorSplash))return false;
        const auto written=prior->smokeWritten_+prior->fireWritten_;
        if(prior->smokeWritten_!=prior->fireWritten_||
           *externalFirstSlot_!=2u+written+2u||*externalFirstSlot_>=externalCapacity_)return false;
        if(written&&(!prior->externalFirstSlot_||*prior->externalFirstSlot_!=2u||
           prior->externalDomain_!=externalDomain_||prior->externalStorage_!=externalStorage_||
           prior->externalCapacity_!=externalCapacity_))return false;
        auto retired=[&](const OriginalWreckComponent50LA* c,std::size_t slot){
            return c&&slot<externalCapacity_&&externalStorage_[slot]==c&&!c->reference00&&
                c->kind==K::entity141300&&!c->nativeEffectControlId&&
                c->sessionGeneration==externalDomain_->sessionGeneration&&c->parent1C==externalStorage_[1];
        };
        for(std::size_t n=0;n<written;++n)
            if(!retired((n&1u)?prior->fire164_[n/2u]:prior->smoke160_[n/2u],2u+n))return false;
        if(!retired(priorDust,2u+written)||!retired(priorSplash,3u+written))return false;
        for(std::size_t n=0;n<smokeWritten_+fireWritten_;++n){
            const auto* c=(n&1u)?fire164_[n/2u]:smoke160_[n/2u];
            if(!c||c->reference00||c->kind!=K::entity141300||c->nativeEffectControlId)return false;
        }
        return true;
    }
    //Read the later occurrence's actual source160/164 arrays, not a supplied
    //span/count or duplicate pointer list. Counted rows form one live prefix;
    //actual tail retirement may leave a precise retired suffix in native slots.
    // Storage authenticates the typed sparks owner. This gate admits only
    // its ONE constructor-only, unappended tail after the actual last Splash.
    static bool freshGroupSparksSuffixCurrent(const OriginalWreckComponent50LA* sparks,
        const OriginalWreckComponent50LA* splash,const OriginalWreckComponentDomain50LA* domain=nullptr,
        OriginalWreckComponent50LA* const* storage=nullptr,std::size_t capacity=0)noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!sparks||!splash||sparks==splash||sparks->reference00!=1u||sparks->kind!=K::effect15F100||
           !sparks->nativeEffectControlId||!*sparks->nativeEffectControlId||sparks->parent1C||
           sparks->previous10||sparks->next14||sparks->firstChild18||
           splash->reference00!=1u||splash->kind!=K::effect19DBE0||!splash->nativeEffectControlId||
           !*splash->nativeEffectControlId||!splash->parent1C||splash->next14||splash->firstChild18||
           sparks->sessionGeneration!=splash->sessionGeneration)return false;
        if(!domain)return !storage&&!capacity; // Zero-pair arrays own no forest borrow; Storage proves it.
        return storage&&domain->objects==storage&&domain->count>=2u&&domain->count<=capacity&&
            sparks->sessionGeneration==domain->sessionGeneration&&
            storage[domain->count-1u]==sparks&&storage[domain->count-2u]==splash;
    }
    bool preflightFollowingFreshPairs(const OriginalWreckEffectArrays50LA& later,
        const OriginalWreckComponent50LA* priorDust,const OriginalWreckComponent50LA* priorSplash,
        std::size_t& live,const OriginalWreckComponent50LA* actualLaterDust=nullptr,
        const OriginalWreckComponent50LA* actualLaterSplash=nullptr,
        const OriginalWreckComponent50LA* actualGroupSparks=nullptr)const noexcept {
        using K=OriginalWreckComponentKind50LA;live=0;
        if(&later==this||!priorDust||!priorSplash||!later.freshAliasesCurrent()||
           !later.attempted_||!later.count_||later.count_>512u||
           later.smokeWritten_>later.count_||later.fireWritten_>later.smokeWritten_||
           later.smokeWritten_>later.fireWritten_+1u||later.pairsStopped_>later.fireWritten_||
           later.fireWritten_>later.pairsStopped_+1u||later.smokeWritten_>later.pairsStopped_+1u||
           !later.freshPairByteOffset_||*later.freshPairByteOffset_!=4u*later.pairsStopped_||
           later.anchorsWritten_!=later.count_)return false;
        if(actualGroupSparks&&(later.pairsStopped_!=later.count_||
           !freshGroupSparksSuffixCurrent(actualGroupSparks,actualLaterSplash,later.externalDomain_,
               later.externalStorage_,later.externalCapacity_)))return false;
        const auto written=later.smokeWritten_+later.fireWritten_;
        if(!written)return !later.externalFirstSlot_&&!later.externalDomain_&&!later.externalStorage_&&!later.externalCapacity_;
        const auto begin=4u+smokeWritten_+fireWritten_;
        if(!later.externalFirstSlot_||*later.externalFirstSlot_!=begin||!later.externalDomain_||
           !later.externalStorage_||later.externalDomain_->objects!=later.externalStorage_||
           later.externalDomain_->count>later.externalCapacity_||begin>=later.externalCapacity_||
           written>later.externalCapacity_-begin||!later.smoke160_||!later.fire164_||
           later.externalStorage_[begin-2u]!=priorDust||later.externalStorage_[begin-1u]!=priorSplash)return false;
        const auto* parent=priorSplash->parent1C;
        if(!parent||parent->reference00!=1u||parent->sessionGeneration!=later.externalDomain_->sessionGeneration||
           priorDust->parent1C!=parent)return false;
        if(smokeWritten_+fireWritten_){
            if(!externalFirstSlot_||*externalFirstSlot_!=2u||externalDomain_!=later.externalDomain_||
               externalStorage_!=later.externalStorage_||externalCapacity_!=later.externalCapacity_)return false;
        }else if(externalFirstSlot_||externalDomain_||externalStorage_||externalCapacity_)return false;
        bool ended=false,unlinked=false;
        const OriginalWreckComponent50LA* previous=priorSplash;
        for(std::size_t n=0;n<written;++n){
            const auto* e=(n&1u)?later.fire164_[n/2u]:later.smoke160_[n/2u];
            if(!e||later.externalStorage_[begin+n]!=e||e->sessionGeneration!=later.externalDomain_->sessionGeneration||e->firstChild18)return false;
            if(e->reference00==1u){
                if(ended||unlinked||e->kind!=((n&1u)?K::effect15DE00:K::effect19A5E0)||!e->nativeEffectControlId)return false;
                if(e->parent1C){
                    if(e->parent1C!=parent||e->previous10!=previous||previous->next14!=e)return false;
                    previous=e;
                }else{
                    if(n+1u!=written||e->previous10||e->next14||previous->next14)return false;
                    unlinked=true;
                }
                ++live;
            }else{
                if(e->reference00||e->kind!=K::entity141300||e->nativeEffectControlId||e->next14)return false;
                //Source links survive retirement. Compare exact retained slots,
                //never follow a historical previous link as a live object.
                const auto* oldPrevious=n?later.externalStorage_[begin+n-1u]:priorSplash;
                if(e->parent1C?(e->parent1C!=parent||e->previous10!=oldPrevious):(e->previous10!=nullptr||n+1u!=written))return false;
                ended=true;
            }
        }
        //One actual source168 suffix only, after every positive pair Stop.
        //Storage authenticates the typed Dust owner; no span/count grant here.
        if(actualLaterDust){
            const auto slot=begin+written;
            if(later.pairsStopped_!=later.count_||later.smokeWritten_!=later.count_||
               later.fireWritten_!=later.count_||slot>=later.externalCapacity_||
               later.externalStorage_[slot]!=actualLaterDust||
               actualLaterDust->sessionGeneration!=later.externalDomain_->sessionGeneration||
               actualLaterDust->next14!=(actualLaterSplash&&actualLaterSplash->reference00==1u&&actualLaterSplash->parent1C?actualLaterSplash:nullptr)||
               actualLaterDust->firstChild18)return false;
            const auto* lastPair=later.externalStorage_[slot-1u];
            if(actualLaterDust->parent1C){
                if(actualLaterDust->parent1C!=parent||actualLaterDust->previous10!=lastPair)return false;
            }else if(actualLaterDust->previous10)return false;
            if(actualLaterDust->reference00==1u){
                if(ended||unlinked||actualLaterDust->kind!=K::effect19A5E0||!actualLaterDust->nativeEffectControlId)return false;
                if(actualLaterDust->parent1C){
                    if(previous!=lastPair||lastPair->next14!=actualLaterDust)return false;
                    previous=actualLaterDust;
                }else if(lastPair->next14)return false;
                ++live;
            }else if(actualLaterDust->reference00||actualLaterDust->kind!=K::entity141300||
                actualLaterDust->nativeEffectControlId)return false;
        }
        //Exact typed native Splash suffix: source16C may still be NULL if
        //19DBE0 stopped before250. Storage alone proves that partial owner.
        if(actualLaterSplash){
            const auto slot=begin+written+1u;
            if(!actualLaterDust||!actualLaterDust->parent1C||slot>=later.externalCapacity_||
               later.externalStorage_[slot]!=actualLaterSplash||
               actualLaterSplash->sessionGeneration!=later.externalDomain_->sessionGeneration||
               actualLaterSplash->next14||actualLaterSplash->firstChild18)return false;
            if(actualLaterSplash->parent1C){
                if(actualLaterSplash->parent1C!=parent||actualLaterSplash->previous10!=actualLaterDust)return false;
            }else if(actualLaterSplash->previous10)return false;
            if(actualLaterSplash->reference00==1u){
                if(actualLaterDust->reference00!=1u||ended||unlinked||
                   actualLaterSplash->kind!=K::effect19DBE0||!actualLaterSplash->nativeEffectControlId)return false;
                if(actualLaterSplash->parent1C){
                    if(previous!=actualLaterDust||actualLaterDust->next14!=actualLaterSplash)return false;
                    previous=actualLaterSplash;
                }else if(actualLaterDust->next14)return false;
                ++live;
            }else if(actualLaterSplash->reference00||actualLaterSplash->kind!=K::entity141300||
                actualLaterSplash->nativeEffectControlId)return false;
        }
        if(previous->next14)return false;
        if(live){
            if(priorDust->reference00!=1u||priorSplash->reference00!=1u||
               later.externalDomain_->count!=begin+live+(actualGroupSparks?1u:0u))return false;
        }else if(later.externalDomain_->count>begin)return false;
        return true;
    }
    bool preflightFreshArrays(const OriginalWreckComponent50LA* actualSmoke=nullptr,
        const OriginalWreckComponent50LA* actualFire=nullptr,
        const OriginalWreckComponent50LA* actualDust=nullptr,
        const OriginalWreckComponent50LA* actualSplash=nullptr,
        const OriginalWreckComponent50LA* actualFollowingOccurrenceChild=nullptr,
        const OriginalWreckEffectArrays50LA* actualPriorArrays=nullptr,
        const OriginalWreckComponent50LA* actualPriorDust=nullptr,
        const OriginalWreckComponent50LA* actualPriorSplash=nullptr,
        const OriginalWreckComponent50LA* actualFollowingOccurrenceTail=nullptr,
        const OriginalWreckEffectArrays50LA* actualFollowingPairs=nullptr,
        const OriginalWreckComponent50LA* actualFollowingPairsDust=nullptr,
        const OriginalWreckComponent50LA* actualFollowingPairsSplash=nullptr,
        const OriginalWreckComponent50LA* actualGroupSparks=nullptr)const noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!freshAliasesCurrent()||pairsStopped_>count_||smokeWritten_>count_||fireWritten_>smokeWritten_||
           smokeWritten_>fireWritten_+1u||pairsStopped_>fireWritten_||fireWritten_>pairsStopped_+1u||
           smokeWritten_>pairsStopped_+1u)return false;
        if(freshPairByteOffset_&&*freshPairByteOffset_!=4u*pairsStopped_)return false;
        if((pairsStopped_||smokeWritten_||fireWritten_)&&(!freshPairByteOffset_||anchorsWritten_!=count_))return false;
        if(actualSmoke&&(!smokeWritten_||!smoke160_||smoke160_[smokeWritten_-1u]!=actualSmoke))return false;
        if(actualFire&&(!fireWritten_||!fire164_||fire164_[fireWritten_-1u]!=actualFire))return false;
        std::size_t followingPairLive=0;
        if(actualFollowingPairs&&(actualFollowingOccurrenceChild||actualFollowingOccurrenceTail||
           !preflightFollowingFreshPairs(*actualFollowingPairs,actualDust,actualSplash,followingPairLive,actualFollowingPairsDust,actualFollowingPairsSplash,actualGroupSparks)))return false;
        if((actualFollowingPairsDust||actualFollowingPairsSplash)&&!actualFollowingPairs)return false;
        const auto written=smokeWritten_+fireWritten_;
        if(actualGroupSparks){
            const auto* lastSplash=actualFollowingPairs?actualFollowingPairsSplash:
                (actualFollowingOccurrenceTail?actualFollowingOccurrenceTail:actualSplash);
            if(pairsStopped_!=count_||smokeWritten_!=count_||fireWritten_!=count_||
               (actualFollowingOccurrenceChild&&!actualFollowingOccurrenceTail)||
               !freshGroupSparksSuffixCurrent(actualGroupSparks,lastSplash,externalDomain_,externalStorage_,externalCapacity_))return false;
        }
        const std::size_t dustLive=actualDust&&actualDust->reference00==1u?1u:0u;
        const std::size_t splashLive=actualSplash&&actualSplash->reference00==1u?1u:0u;
        if(actualSplash&&!actualDust)return false;
        if(actualFollowingOccurrenceChild&&(!actualDust||!actualSplash||!dustLive||!splashLive||
           actualFollowingOccurrenceChild->reference00!=1u||
           actualFollowingOccurrenceChild->kind!=K::effect19A5E0||
           !actualFollowingOccurrenceChild->nativeEffectControlId||
           actualFollowingOccurrenceChild->sessionGeneration!=actualSplash->sessionGeneration||
           actualFollowingOccurrenceChild->next14!=(actualFollowingOccurrenceTail&&actualFollowingOccurrenceTail->parent1C?actualFollowingOccurrenceTail:nullptr)||
           actualFollowingOccurrenceChild->firstChild18))return false;
        if(actualFollowingOccurrenceTail&&(!actualFollowingOccurrenceChild||
           !actualFollowingOccurrenceChild->parent1C||actualFollowingOccurrenceTail->reference00!=1u||
           (actualFollowingOccurrenceTail->kind!=K::effect15DE00&&actualFollowingOccurrenceTail->kind!=K::effect19DBE0)||
           !actualFollowingOccurrenceTail->nativeEffectControlId||
           actualFollowingOccurrenceTail->sessionGeneration!=actualFollowingOccurrenceChild->sessionGeneration||
           actualFollowingOccurrenceTail->next14||actualFollowingOccurrenceTail->firstChild18))return false;
        if(actualFollowingOccurrenceTail){
            if(actualFollowingOccurrenceTail->parent1C){
                if(actualFollowingOccurrenceTail->parent1C!=actualFollowingOccurrenceChild->parent1C||
                   actualFollowingOccurrenceTail->previous10!=actualFollowingOccurrenceChild||
                   actualFollowingOccurrenceChild->next14!=actualFollowingOccurrenceTail)return false;
            }else if(actualFollowingOccurrenceTail->previous10||actualFollowingOccurrenceChild->next14)return false;
        }
        if(actualFollowingOccurrenceChild){
            //Exactly one later occurrence child: either its original unlinked
            //constructor cut or the real append after this actual Splash tail.
            if(actualFollowingOccurrenceChild->parent1C){
                if(!actualSplash->parent1C||
                   actualFollowingOccurrenceChild->parent1C!=actualSplash->parent1C||
                   actualFollowingOccurrenceChild->previous10!=actualSplash||
                   actualSplash->next14!=actualFollowingOccurrenceChild)return false;
            }else if(actualFollowingOccurrenceChild->previous10||actualSplash->next14)return false;
        }
        if(actualDust&&attempted_&&!written)return false;
        if(written){
            if(!externalFirstSlot_||!externalDomain_||!externalStorage_||!externalCapacity_||
               externalDomain_->objects!=externalStorage_||externalDomain_->count>externalCapacity_||
               *externalFirstSlot_>externalCapacity_||written>externalCapacity_-*externalFirstSlot_||
               (externalDomain_->count<*externalFirstSlot_&&
                !retiredPriorSliceCurrent(actualPriorArrays,actualPriorDust,actualPriorSplash))||externalDomain_->count>*externalFirstSlot_+written+dustLive+splashLive+(actualFollowingOccurrenceChild?1u:0u)+(actualFollowingOccurrenceTail?1u:0u)+followingPairLive+(actualGroupSparks?1u:0u))return false;
            if(actualGroupSparks&&!actualFollowingPairs&&!actualFollowingOccurrenceChild&&
               externalDomain_->count!=*externalFirstSlot_+written+2u+1u)return false;
            if(actualFollowingOccurrenceChild){
                const auto slot=*externalFirstSlot_+written+dustLive+splashLive;
                if(slot>=externalCapacity_||externalDomain_->count!=slot+1u+(actualFollowingOccurrenceTail?1u:0u)+(actualGroupSparks?1u:0u)||
                   externalStorage_[slot]!=actualFollowingOccurrenceChild||
                   (actualFollowingOccurrenceTail&&(slot+1u>=externalCapacity_||externalStorage_[slot+1u]!=actualFollowingOccurrenceTail)))return false;
            }
            if(actualDust){
                const auto slot=*externalFirstSlot_+written;
                if(slot>=externalCapacity_||externalStorage_[slot]!=actualDust)return false;
                if(dustLive){if(slot>=externalDomain_->count||actualDust->kind!=K::effect19A5E0||
                    !actualDust->nativeEffectControlId)return false;}
                else if(actualDust->reference00||actualDust->kind!=K::entity141300||
                    actualDust->nativeEffectControlId||slot<externalDomain_->count)return false;
            }
            if(actualSplash){
                const auto slot=*externalFirstSlot_+written+1u;
                if(slot>=externalCapacity_||externalStorage_[slot]!=actualSplash)return false;
                if(splashLive){if(slot>=externalDomain_->count||actualSplash->kind!=K::effect19DBE0||
                    !actualSplash->nativeEffectControlId)return false;}
                else if(actualSplash->reference00||actualSplash->kind!=K::entity141300||
                    actualSplash->nativeEffectControlId||slot<externalDomain_->count)return false;
            }
            //Only published slots are read. Forest storage outlives its decremented count.
            //Retired identities are compared in-place; no erased slot or owner reconstruction.
            for(std::size_t n=0;n<written;++n){
                const auto* c=(n&1u)?fire164_[n/2u]:smoke160_[n/2u];
                const auto slot=*externalFirstSlot_+n;
                if(!c||externalStorage_[slot]!=c)return false;
                if(c->reference00==1u){
                    if(slot>=externalDomain_->count||c->kind!=((n&1u)?K::effect15DE00:K::effect19A5E0)||
                       !c->nativeEffectControlId)return false;
                }else if(c->reference00||c->kind!=K::entity141300||c->nativeEffectControlId||slot<externalDomain_->count)return false;
            }
        }else if(externalFirstSlot_||externalDomain_||externalStorage_||externalCapacity_)return false;
        if(!attempted_)return !count_&&!anchorsWritten_&&!smoke160_&&!fire164_&&!anchors170_&&!freshModel_&&!freshGraph_;
        return count_&&count_<=512u&&anchorsWritten_<=count_&&freshModel_&&freshGraph_&&
            (!fire164_||smoke160_)&&(!anchors170_||fire164_)&&(!anchorsWritten_||anchors170_);
    }
    bool allocateFreshAndFind2CB634(std::uint32_t count,const OriginalRacerModelSceneOwner50LA& model,
        const SceneGraph49P& graph)noexcept {
        if(!preflightFreshArrays()||attempted_||!count||count>512u||model.released()||!model.model()||!graph.nodes)return false;
        attempted_=true;count_=count;freshModel_=&model;freshGraph_=graph.nodes;
        try {
            boundary_=0x2cb634u;smoke160_=std::make_unique<OriginalWreckComponent50LA*[]>(count_);
            source160_=smoke160_.get(); //2CB63C
            boundary_=0x2cb644u;fire164_=std::make_unique<OriginalWreckComponent50LA*[]>(count_);
            source164_=fire164_.get(); //2CB64C
            boundary_=0x2cb660u;anchors170_=std::make_unique<const O*[]>(count_);
            source170_=anchors170_.get(); //2CB668
        }catch(...){return false;} //Native failure cancellation, not guest malloc equivalence.
        for(std::size_t i=0;i<count_;++i){
            const O* found=nullptr;boundary_=0x2cb694u;
            if(!findDamage28C590(model,graph,found))return false;
            anchors170_[i]=found;++anchorsWritten_; //2CB6AC; known miss is NULL.
        }
        pairsStopped_=0u; //Actual s0 initialization2CB6C0 after anchor loop.
        freshPairByteOffset_=0u; //Actual distinct s1 initialization2CB6C8.
        boundary_=0x2cb6ccu;return true;
    }
    bool releaseFresh2CB500(const OriginalWreckComponent50LA* actualSmoke=nullptr,
        const OriginalWreckComponent50LA* actualFire=nullptr,
        const OriginalWreckComponent50LA* actualDust=nullptr,
        const OriginalWreckComponent50LA* actualSplash=nullptr,
        const OriginalWreckEffectArrays50LA* actualPriorArrays=nullptr,
        const OriginalWreckComponent50LA* actualPriorDust=nullptr,
        const OriginalWreckComponent50LA* actualPriorSplash=nullptr)noexcept {
        if(!preflightFreshArrays(actualSmoke,actualFire,actualDust,actualSplash,nullptr,actualPriorArrays,actualPriorDust,actualPriorSplash)||
           (actualDust&&actualDust->reference00)||(actualSplash&&actualSplash->reference00))return false;
        //Whole wrapper preflight/retirement must have retired EVERY actual child,
        //not merely the current typed owner borrows, before any array is freed.
        for(std::size_t i=0;i<smokeWritten_;++i)if(smoke160_[i]->reference00)return false;
        for(std::size_t i=0;i<fireWritten_;++i)if(fire164_[i]->reference00)return false;
        //Derived destructor frees arrays BEFORE base165BD0's children/model.
        //Unwritten slots are never read; external pointees are not owned here.
        smoke160_.reset(); //2CB534
        fire164_.reset(); //2CB548
        anchors170_.reset(); //2CB55C
        retired_=true; //Source160/164/170 and written cursors remain history.
        return true;
    }
    static bool damageName(const std::string& name)noexcept {
        constexpr char expected[]="damage";
        if(name.size()!=6)return false;
        for(std::size_t i=0;i<6;++i){auto c=name[i];if(c>='A'&&c<='Z')c=char(c+32);
            if(c!=expected[i])return false;}return true;
    }
    static bool findDamage28C590(const OriginalRacerModelSceneOwner50LA& model,
        const SceneGraph49P& graph,const O*& found)noexcept {
        found=nullptr;
        if(model.released()||!model.model()||!model.sourceRowCount()||!graph.nodes)return false;
        std::size_t remaining=model.sourceRowCount()+1u;
        auto walk=[&](auto&& self,SceneNodeIndex49P index)->bool {
            if(index==SceneNodeIndex49P::none)return true;
            const auto n=static_cast<std::uint32_t>(index);
            if(!remaining--||n>=graph.count)return false;
            const O* object=model.model()->sceneIndex==index?model.model():nullptr;
            for(std::uint32_t i=0;!object&&i<model.sourceRowCount();++i){
                const auto* row=model.sourceRow(i);if(row&&row->sceneIndex==index)object=row;}
            if(!object||!object->constructed||object->released)return false;
            if(damageName(object->name38)){found=object;return true;}
            if(!self(self,graph.nodes[n].nextSibling188))return false;
            if(found)return true;
            return self(self,graph.nodes[n].firstChild184);
        };
        return walk(walk,model.model()->sceneIndex);
    }
    bool allocateAndFind2CB634(std::uint32_t count,const OriginalRacerModelSceneOwner50LA& model,
        const SceneGraph49P& graph)noexcept {
        if(freshRoute_||attempted_||retired_||!count||count>512)return false;
        attempted_=true;count_=count;
        try {
            boundary_=0x2cb634u;smoke160_=std::make_unique<OriginalWreckComponent50LA*[]>(count_);
            boundary_=0x2cb644u;fire164_=std::make_unique<OriginalWreckComponent50LA*[]>(count_);
            // This fresh piece's2CB4B0 initialized170=NULL, so allocation is
            //genuinely reached. No existing170 array is silently replaced.
            boundary_=0x2cb660u;anchors170_=std::make_unique<const O*[]>(count_);
        }catch(...){return false;}
        //3C2670 is exactly "damage", with no integer conversion. Source
        //passes the loop index to sprintf but resolves the same name each time.
        for(std::size_t i=0;i<count_;++i){
            const O* found=nullptr;boundary_=0x2cb694u;
            if(!findDamage28C590(model,graph,found))return false;
            anchors170_[i]=found;++anchorsWritten_; //2CB6AC; genuine miss=NULL
        }
        boundary_=0x2cb6ccu;return true; //before first external child allocation
    }
    bool release2CB500()noexcept {
        if(freshRoute_||!attempted_||retired_)return false;
        //2CB534/548/55C free pointer arrays without reading/releasing pointees.
        //External children are separately owned; this owner never disposes them.
        smoke160_.reset();fire164_.reset();anchors170_.reset();
        externalDomain_=nullptr;externalStorage_=nullptr;
        retired_=true;boundary_=0x2cb568u;return true;
    }
public:
    OriginalWreckEffectArrays50LA()=default;
    OriginalWreckEffectArrays50LA(const OriginalWreckEffectArrays50LA&)=delete;
    OriginalWreckEffectArrays50LA& operator=(const OriginalWreckEffectArrays50LA&)=delete;
    ~OriginalWreckEffectArrays50LA(){if(smoke160_||fire164_||anchors170_)std::terminate();}
    bool attempted()const noexcept{return attempted_;}
    bool retired()const noexcept{return retired_;}
    std::size_t count()const noexcept{return count_;}
    std::size_t anchorsWritten()const noexcept{return anchorsWritten_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    // Never expose uninitialized smoke/fire entries as source NULL children.
};
}

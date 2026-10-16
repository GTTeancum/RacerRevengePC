#pragma once
#include "original_racer_main_body_owner50la.h"
#include <memory>

namespace rr::game {
// Concrete344 Entity plus the separately allocated282750 target array.
// Acceleration38 and force40.X/Z borrow the existing canonical numerical
// fields; no duplicate gravity scalar, force model, or physical body exists.
// The native2827D0 leaf does not retain its transient40.Y cache. That lane
// stays explicitly unrepresented here, rather than becoming stale owner data.
class OriginalRacerGravityOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    using Target=const OriginalRacerMainBodyOwner50LA*;
    enum class Stage {allocated,constructed,arrayAllocated,appended,targetBound,released};
    C entity_;
    OriginalApplicationEntities50LA* application_=nullptr;
    MainBodyState49K* numeric_=nullptr;
    MainBodyParameters49K* parameters_=nullptr;
    std::unique_ptr<Target[]> targets50_;
    std::uint32_t count30_=0,capacity34_=0;
    float force4CW_=0.f; //Unchanged by2827D0; separate from body force180.W.
    Stage stage_=Stage::allocated;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    R construct282670(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        MainBodyState49K& main,MainBodyParameters49K& parameters)noexcept {
        if(stage_!=Stage::allocated||application_||numeric_||targets50_)
            return {S::owner,0x282670u};
        application_=&app;numeric_=&main;parameters_=&parameters;
        const auto r=initializeOriginalWreckComponent50LA(entity_,K::gravity282670,
            generation,key(this),true,app.objectCount423F60());
        if(r.status!=S::complete)return r;
        count30_=capacity34_=0;
        constexpr std::uint32_t acceleration=0x4200cccdu;
        std::memcpy(&main.selectedGravity,&acceleration,4);
        const auto& source=app.gravityOrigin4431C0_;
        std::memcpy(&parameters.gravityForceX,source.data(),4);
        std::memcpy(&parameters.gravityForceZ,source.data()+2,4);
        std::memcpy(&force4CW_,source.data()+3,4);
        targets50_.reset(); //2826BC, null array pointer before282750.
        stage_=Stage::constructed;return {S::complete,0x2826ccu};
    }
    R allocateOne282750()noexcept {
        if(stage_!=Stage::constructed||targets50_)return {S::owner,0x212024u};
        count30_=0;capacity34_=1; //28275C/760 precede allocation28276C.
        // Native pointer width replaces one source32-bit pointer. Deliberately
        //not value-initialized:282750 does not initialize unused array slots.
        try {targets50_=std::unique_ptr<Target[]>(new Target[1]);}
        catch(...){return {S::capacity,0x28276cu};}
        stage_=Stage::arrayAllocated;return {S::complete,0x212024u};
    }
    R append212034(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(stage_!=Stage::arrayAllocated||!application_||!targets50_)
            return {S::owner,0x212034u};
        const auto r=appendOriginalWreckComponent50LA(domain,racer,entity_,0u,
            application_->attachmentLock423F58());
        if(r.status==S::complete)stage_=Stage::appended;
        return r;
    }
    R bindTarget212048(const OriginalRacerMainBodyOwner50LA& body)noexcept {
        if(stage_!=Stage::appended||!targets50_||count30_!=0||capacity34_!=1||
           body.canonicalNumericAddress()!=numeric_||
           body.entity().sessionGeneration!=entity_.sessionGeneration||
           body.entity().parent1C!=entity_.parent1C||entity_.previous10!=&body.entity())
            return {S::owner,0x212048u};
        targets50_[count30_]=&body;++count30_; //2827B0/B4/B8/BC; no body retain.
        stage_=Stage::targetBound;return {S::complete,0x212048u};
    }
    bool appended()const noexcept{return stage_==Stage::appended||stage_==Stage::targetBound;}
    C* liveEntity()noexcept{return entity_.reference00&&stage_!=Stage::released?&entity_:nullptr;}
    bool preflightClose(const C* racer,const C* previous,const C* next)const noexcept {
        if(stage_==Stage::released||!entity_.reference00)return true;
        if(!application_||application_->attachmentLock423F58()||entity_.reference00!=1||
           entity_.firstChild18||entity_.next14!=next||count30_>1||capacity34_>1||
           (count30_&&(!targets50_||stage_!=Stage::targetBound)))return false;
        if(!appended())return !entity_.parent1C&&!entity_.previous10&&!next;
        return racer&&previous&&entity_.parent1C==racer&&entity_.previous10==previous&&
            previous->next14==&entity_&&entity_.mask20==0u&&
            (!next||(next->parent1C==racer&&next->previous10==&entity_));
    }
    R close142590(const C* racer,const C* previous)noexcept {
        if(!preflightClose(racer,previous,nullptr))return {S::topology,0x142590u};
        if(stage_==Stage::released)return {S::complete,0x282744u};
        if(entity_.reference00){
            if(entity_.previous10)entity_.previous10->next14=entity_.next14;
            if(entity_.parent1C&&entity_.parent1C->firstChild18==&entity_)
                entity_.parent1C->firstChild18=entity_.next14;
            --entity_.reference00; //104970 ->2826D0.
            //2826F8/282704 frees only the pointer array, without inspecting,
            //retaining, releasing or deleting its borrowed body entries.
            targets50_.reset();
            entity_.kind=K::entity141300;--application_->objectCount423F60();
        }
        //Canonical numeric fields and source count/links/alias stay unchanged.
        stage_=Stage::released;return {S::complete,0x282744u};
    }
public:
    OriginalRacerGravityOwner50LA()=default;
    OriginalRacerGravityOwner50LA(const OriginalRacerGravityOwner50LA&)=delete;
    OriginalRacerGravityOwner50LA& operator=(const OriginalRacerGravityOwner50LA&)=delete;
    ~OriginalRacerGravityOwner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
    // Address comparison only outside the retained owner's actual bank visit.
    const float* canonicalAccelerationAddress()const noexcept {
        return numeric_?&numeric_->selectedGravity:nullptr;
    }
    const OriginalRacerMainBodyOwner50LA* target50()const noexcept {
        return stage_==Stage::targetBound&&targets50_&&count30_==1?targets50_[0]:nullptr;
    }
};
}

#pragma once
#include "original_racer_main_body_owner50la.h"
#include <memory>
#include <cfenv>
#include <cmath>

namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {
// Concrete344 Entity plus the separately allocated282750 target array.
// Acceleration38 and force40.X/Z borrow the existing canonical numerical
// fields; no duplicate gravity scalar, force model, or physical body exists.
// The retained per-instance force44.Y cache is now initialized at the real
// constructor and written by the private actual-target2827D0 service.
class OriginalRacerGravityOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
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
    mutable float force44Y_=0.f; //Real per-owner cache, not another Bank body lane.
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
        std::memcpy(&force44Y_,source.data()+1,4);
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
    bool storeEndAcceleration1143E0(OriginalApplicationEntities50LA& application,
        const C& racer,const OriginalRacerMainBodyOwner50LA& body,
        MainBodyState49K& current,std::uint32_t f12Bits)const noexcept {
        if(stage_!=Stage::targetBound||application_!=&application||numeric_!=&current||
           entity_.reference00!=1||entity_.kind!=K::gravity282670||
           entity_.sessionGeneration!=racer.sessionGeneration||entity_.parent1C!=&racer||
           body.entity().parent1C!=&racer||body.entity().sessionGeneration!=racer.sessionGeneration||
           body.entity().reference00!=1||body.canonicalNumericAddress()!=&current||
           target50()!=&body||application.attachmentLock423F58())return false;
        //Actual table3D2E60+7C=1143E0. JR delay-slot SWC1 f12,+38;
        //no arithmetic, force accumulation, callback or49W phase transition.
        std::memcpy(&numeric_->selectedGravity,&f12Bits,4);return true;
    }
    //Private same-Bank-visit body of2827D0 for the actual single target
    //constructed by212048. No scheduler, scene resolution or engine application.
    bool accumulateTarget2827D0(OriginalApplicationEntities50LA& application,
        const C& racer,const OriginalRacerMainBodyOwner50LA& body,
        MainBodyState49K& current,MainBodyParameters49K& parameters,
        std::uint32_t& sourcePC)const noexcept {
        sourcePC=0x2827d0;
        if(stage_!=Stage::targetBound||application_!=&application||numeric_!=&current||
           parameters_!=&parameters||target50()!=&body||count30_!=1||capacity34_!=1||
           entity_.reference00!=1||entity_.kind!=K::gravity282670||
           entity_.sessionGeneration!=racer.sessionGeneration||entity_.parent1C!=&racer||
           body.entity().parent1C!=&racer||body.entity().sessionGeneration!=racer.sessionGeneration||
           body.entity().reference00!=1||body.canonicalNumericAddress()!=&current||
           application.attachmentLock423F58()||std::fegetround()!=FE_TONEAREST)return false;
        sourcePC=0x2827f0;
        if(!std::isfinite(parameters.body.mass)||!std::isfinite(current.selectedGravity))return false;
        volatile float negativeMass=-parameters.body.mass;
        volatile float y=negativeMass*current.selectedGravity;
        if(!std::isfinite(y))return false;
        sourcePC=0x2827fc;force44Y_=y; //Before any target force store.
        const float force[]={parameters.gravityForceX,force44Y_,parameters.gravityForceZ};
        auto& loads=current.components.root.loads.force;
        constexpr std::uint32_t stores[]={0x28281cu,0x28282cu,0x28283cu};
        for(unsigned k=0;k<3;++k){
            sourcePC=stores[k];
            if(!std::isfinite(force[k])||!std::isfinite(loads.v[k]))return false;
            volatile float sum=loads.v[k]+force[k];
            if(!std::isfinite(sum))return false;
            loads.v[k]=sum; //Earlier cache/XYZ stores survive a later rejection.
        }
        sourcePC=0x282858;return true;
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

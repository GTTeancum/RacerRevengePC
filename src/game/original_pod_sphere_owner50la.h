#pragma once
#include "original_application_entities50la.h"
#include "pod_sphere_constraint49l.h"

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
// Native Entity allocation for the SAME banked numeric joint. Numeric fields
// remain solely in Fixture.state.bodies.joint; this object never copies them.
// Only the retained chariot constructor may allocate, construct or append it.
class OriginalPodSphereOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    enum class Stage {allocated,base,suffix,initialized,appended,released};
    C entity_;
    PodSphereJoint49L* numeric_=nullptr;
    OriginalApplicationEntities50LA* application_=nullptr;
    Stage stage_=Stage::allocated;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    //3CF290+30=141C20, a literal identity return, with no extra stores.
    static C* initialize141C20(C& entity)noexcept{return &entity;}
    R construct1B6200(OriginalApplicationEntities50LA& app,
        std::uint64_t generation,PodSphereJoint49L& actual)noexcept {
        if(stage_!=Stage::allocated||application_||numeric_)return {S::owner,0x1b6200u};
        application_=&app;numeric_=&actual;
        auto result=initializeOriginalWreckComponent50LA(entity_,K::constraint1276E0,
            generation,key(this),true,app.objectCount423F60());
        if(result.status!=S::complete)return result;
        stage_=Stage::base;
        //1276E0 ->141300, then127704/70C. No type registry is called.
        actual.damping34=2.f;actual.stiffness30=1.f;
        entity_.kind=K::podSphere1B6200; //1B6220 concrete dispatch installation.
        if(!initializePodSphere49L(actual))return {S::owner,0x1b6214u};
        stage_=Stage::suffix;
        if(initialize141C20(entity_)!=&entity_)return {S::owner,0x212698u};
        stage_=Stage::initialized;
        return {S::complete,0x212698u};
    }
    R append2126A8(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(stage_!=Stage::initialized||!application_)return {S::owner,0x2126a8u};
        const auto result=appendOriginalWreckComponent50LA(domain,racer,entity_,0xffffffffu,
            application_->attachmentLock423F58());
        if(result.status==S::complete)stage_=Stage::appended;
        return result;
    }
    C* liveEntity()noexcept{return entity_.reference00&&stage_!=Stage::released?&entity_:nullptr;}
    bool preflightClose(const C* racer,const C* precedingComponent)const noexcept {
        if(stage_==Stage::released||!entity_.reference00)return true;
        if(!application_||application_->attachmentLock423F58()||entity_.reference00!=1u||
           entity_.firstChild18||entity_.next14)return false;
        if(stage_!=Stage::appended)return !entity_.parent1C&&!entity_.previous10;
        return racer&&precedingComponent&&entity_.parent1C==racer&&
            entity_.previous10==precedingComponent&&precedingComponent->next14==&entity_&&
            entity_.mask20==0xffffffffu;
    }
    R close142590(const C* racer,const C* precedingComponent)noexcept {
        if(!preflightClose(racer,precedingComponent))return {S::topology,0x142590u};
        if(stage_==Stage::released)return {S::complete,0x1b64ccu};
        if(entity_.reference00){
            //142590 repairs the current list, then104970 invokes
            //1B6470 ->127730 ->1413E0. Borrowed A/B need no release.
            if(entity_.previous10)entity_.previous10->next14=entity_.next14;
            if(entity_.parent1C&&entity_.parent1C->firstChild18==&entity_)
                entity_.parent1C->firstChild18=entity_.next14;
            --entity_.reference00;
            entity_.kind=K::constraint1276E0; //127754
            entity_.kind=K::entity141300; //141404
            --application_->objectCount423F60();
        }
        // No numeric zeroing, source alias nulling, mask recomputation or body
        // release is present in these destructors. Source stale links survive.
        stage_=Stage::released;
        return {S::complete,0x1b64ccu};
    }
public:
    OriginalPodSphereOwner50LA()=default;
    OriginalPodSphereOwner50LA(const OriginalPodSphereOwner50LA&)=delete;
    OriginalPodSphereOwner50LA& operator=(const OriginalPodSphereOwner50LA&)=delete;
    ~OriginalPodSphereOwner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
    // Address identity only. Dereference requires the owner's actual bank slot
    // visit; all slots share the bank's active canonical Fixture address.
    const PodSphereJoint49L* canonicalNumericAddress()const noexcept{return numeric_;}
};
}

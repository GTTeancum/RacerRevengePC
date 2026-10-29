#pragma once
#include "original_application_entities50la.h"
#include "pod_sphere_constraint49l.h"
#include <cstddef>
#include <optional>

namespace native_racer_model50LA {class Owner;}
namespace native_racer50HN {class Bank;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {
class NativeSecondaryContactOwner50LA;
class OriginalRacerMainBodyOwner50LA;
class OriginalRacerSecondaryBodyOwner50LA;
class OriginalRacerEntityCold50LA;
struct VehicleFrameState49W;
// Native Entity allocation for the SAME banked numeric joint. Numeric fields
// remain solely in Fixture.state.bodies.joint; this object never copies them.
// Only the retained chariot constructor may allocate, construct or append it.
class OriginalPodSphereOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    enum class Stage {allocated,base,suffix,initialized,appended,released};
    C entity_;
    PodSphereJoint49L* numeric_=nullptr;
    OriginalApplicationEntities50LA* application_=nullptr;
    Stage stage_=Stage::allocated;
    // Native lifetime associations only; never source44/48 pointer stores or
    // a fabricated35C Entity. Only the actual retained cold producer writes
    // these. Their shared arena address is NOT permission to visit a Bank slot.
    struct NativeBodies {
        const ::native_racer50HN::Bank* bank=nullptr;
        VehicleFrameState49W* frame=nullptr;
        const ::native_racer_model50LA::Owner* model=nullptr;
        const NativeSecondaryContactOwner50LA* a=nullptr;
        const OriginalRacerMainBodyOwner50LA* b=nullptr;
        const OriginalRacerEntityCold50LA* racer=nullptr;
        std::uint64_t generation=0,racerKey=0;
        std::size_t slot=0;
        bool published=false,revoked=false;
    } nativeBodies_;
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
    bool sourceRoute_=false,sourceConstructorComplete_=false;
    bool sourceAAttempted_=false,sourceAComplete_=false,sourceBAttempted_=false,sourceBComplete_=false;
    std::uint32_t sourcePC_=0;
    std::optional<const OriginalRacerSecondaryBodyOwner50LA*> sourceA44_;
    std::optional<const OriginalRacerMainBodyOwner50LA*> sourceB48_;
    const OriginalRacerSecondaryBodyOwner50LA* pendingSourceA_=nullptr;
    const OriginalRacerMainBodyOwner50LA* pendingSourceB_=nullptr;
    static void publishSourcePresence(void* context,bool isBodyB,bool present)noexcept {
        auto& owner=*static_cast<OriginalPodSphereOwner50LA*>(context);
        //Only typed borrow publication. Numeric stores and all input/global
        //storage belong to the source-ordered numerical method, not this hook.
        if(isBodyB)owner.sourceB48_=present?owner.pendingSourceB_:nullptr;
        else owner.sourceA44_=present?owner.pendingSourceA_:nullptr;
    }
    R constructSource1B6200(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        PodSphereJoint49L& actual,const std::array<std::uint32_t,4>& actual434390)noexcept {
        if(stage_!=Stage::allocated||application_||numeric_||sourceRoute_)return {S::owner,0x1b6200u};
        sourceRoute_=true;application_=&app;numeric_=&actual;sourcePC_=0x1276e0u;
        auto r=initializeOriginalWreckComponent50LA(entity_,K::constraint1276E0,generation,key(this),true,
            app.objectCount423F60());
        if(r.status!=S::complete)return r;
        stage_=Stage::base;actual.damping34=2.f;actual.stiffness30=1.f; //127704/70C.
        entity_.kind=K::podSphere1B6200; //1B6220.
        const PodSphereSourcePresenceHooks49L hooks{this,&publishSourcePresence};
        if(!initializePodSphereSourceOrdered49L(actual,actual434390.data(),sourcePC_,&hooks))
            return {S::owner,sourcePC_};
        sourceConstructorComplete_=true;stage_=Stage::suffix;
        return {S::complete,0x212688u}; //Alias publication/identity are caller stages.
    }
    bool initializeSource212698()noexcept {
        if(!sourceRoute_||!sourceConstructorComplete_||stage_!=Stage::suffix)return false;
        if(initialize141C20(entity_)!=&entity_)return false;
        stage_=Stage::initialized;sourcePC_=0x2126a0u;return true;
    }
    bool setupSourceA212700(const OriginalRacerSecondaryBodyOwner50LA& actualA,
        float inverseMass,const PodSphereMatrix49L& inverseInertia,const PodSphereVector49L& anchor)noexcept {
        if(!sourceRoute_||!sourceConstructorComplete_||stage_!=Stage::appended||!numeric_||sourceAAttempted_)
            return false;
        sourceAAttempted_=true;pendingSourceA_=&actualA;
        const PodSphereSourcePresenceHooks49L hooks{this,&publishSourcePresence};
        if(!setupPodSphereASourceOrdered49L(*numeric_,inverseMass,inverseInertia,anchor,sourcePC_,&hooks))return false;
        sourceAComplete_=true;sourcePC_=0x212708u;return true;
    }
    bool setupSourceB212718(const OriginalRacerMainBodyOwner50LA& actualB,
        const PodSphereVector49L& anchor,float length)noexcept {
        if(!sourceRoute_||stage_!=Stage::appended||!numeric_||!sourceAComplete_||sourceBAttempted_)return false;
        sourceBAttempted_=true;pendingSourceB_=&actualB;
        const PodSphereSourcePresenceHooks49L hooks{this,&publishSourcePresence};
        if(!setupPodSphereBSourceOrdered49L(*numeric_,true,anchor,length,sourcePC_,&hooks))return false;
        sourceBComplete_=true;sourcePC_=0x212720u;return true;
    }
    R append2126A8(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(stage_!=Stage::initialized||!application_)return {S::owner,0x2126a8u};
        const auto result=appendOriginalWreckComponent50LA(domain,racer,entity_,0xffffffffu,
            application_->attachmentLock423F58());
        if(result.status==S::complete)stage_=Stage::appended;
        return result;
    }
    C* liveEntity()noexcept{return entity_.reference00&&stage_!=Stage::released?&entity_:nullptr;}
    bool preflightClose(const C* racer,const C* precedingComponent,const C* followingComponent=nullptr)const noexcept {
        if(stage_==Stage::released||!entity_.reference00)return true;
        if(!application_||application_->attachmentLock423F58()||entity_.reference00!=1u||
           entity_.firstChild18||entity_.next14!=followingComponent||
           (followingComponent&&(followingComponent->previous10!=&entity_||followingComponent->parent1C!=racer)))return false;
        if(stage_!=Stage::appended)return !followingComponent&&!entity_.parent1C&&!entity_.previous10;
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

#pragma once
#include "../../src/game/original_application_entities50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/original_wreck_tuning50la.h"
#include <cfenv>
#include <cmath>
#include <optional>
#include <cstring>

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
//Actual21254C allocation. Every represented numerical lane belongs to the
//same canonical secondary Bank storage; absent source words alone live here.
class OriginalRacerSecondaryBodyOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class OriginalRacerQuery364Owner50LA;
    friend class OriginalRacerContact368Owner50LA;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;using R=OriginalWreckComponentResult50LA;
    using Words=std::array<std::uint32_t,4>;
    enum class Stage {fresh,base,constructed,released};
    Stage stage_=Stage::fresh;C entity_;
    OriginalApplicationEntities50LA* application_=nullptr;
    VehicleFrameState49W* state_=nullptr;
    VehicleFrameParameters49W* parameters_=nullptr;
    VehicleFrameBindings49W* bindings_=nullptr;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    std::optional<const OriginalRacerSceneObject50LA*> scene288_;
    std::optional<const void*> allocation28C_;
    std::optional<std::uint32_t> word30_,flags200_,awake208_,count264_,count268_,word284_;
    std::optional<Words> vector40_,vector50_,vectorF0_;
    std::optional<float> cachedAnglesE0W_;
    std::optional<BodyMatrix48G> inertia100_;
    bool initialized_=false,appended_=false,sceneBound_=false;
    bool massAttempted_=false,massComplete_=false,inertiaAttempted_=false,inertiaComplete_=false;
    std::uint32_t boundary_=0;
    static float word(std::uint32_t bits)noexcept {float f;std::memcpy(&f,&bits,4);return f;}
    static void copy(Vector3_48C& xyz,float& w,const Words& a)noexcept {
        std::memcpy(xyz.v,a.data(),12);std::memcpy(&w,a.data()+3,4);
    }
    R construct167920(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        VehicleFrameState49W& state,VehicleFrameParameters49W& parameters,
        VehicleFrameBindings49W& bindings,const Words& actual433AD0,
        const Words& actual433B80,const Words& actual382B60)noexcept {
        if(stage_!=Stage::fresh||application_||state_||entity_.reference00)return {S::owner,0x21254cu};
        //Caller must hold exact fresh source Bank slot; never adopt a warmed
        //legacy secondary body. This private call is sticky before141300.
        application_=&app;state_=&state;parameters_=&parameters;bindings_=&bindings;boundary_=0x164edcu;
        auto r=initializeOriginalWreckComponent50LA(entity_,K::body164ED0,generation,
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(this)),true,app.objectCount423F60());
        if(r.status!=S::complete)return r;
        stage_=Stage::base;
        auto& secondary=state.bodies.secondary;auto& root=secondary.root;auto& body=root.body;
        auto& p=parameters.bodies.secondary;auto& extra=state.secondaryExtra;auto& supplement=state.secondarySupplement;
        const Words vf0{{0,0,0,0x3f800000u}};
        //Actual inline1048E0/VF0 constructors164EFC..164FD4 BEFORE globals.
        vector40_=vf0;vector50_=vf0;
        copy(body.position,extra.position60W,vf0);
        copy(body.velocity,secondary.velocity70W,vf0);
        copy(root.localVelocity80,supplement.localVelocity80W,vf0);
        copy(root.previousVelocity90,supplement.previousVelocity90W,vf0);
        copy(root.worldAngularVelocityA0,extra.worldAngularVelocityA0W,vf0);
        copy(body.angularVelocity,secondary.angularVelocityB0W,vf0);
        copy(body.linearAcceleration,supplement.linearAccelerationC0W,vf0);
        copy(body.angularAcceleration,supplement.angularAccelerationD0W,vf0);
        cachedAnglesE0W_=1.f;copy(secondary.cachedAnglesE0,*cachedAnglesE0W_,vf0);vectorF0_=vf0;
        copy(root.loads.force,secondary.force180W,vf0);copy(root.loads.torque,secondary.torque190W,vf0);
        copy(root.positionCorrection1a0,extra.positionCorrection1A0W,vf0);
        copy(root.rotationCorrection1b0,extra.rotationCorrection1B0W,vf0);
        copy(root.accumulator1c0,extra.accumulator1C0W,vf0);
        copy(root.accumulator1d0,extra.accumulator1D0W,vf0);copy(p.centerOffset,p.centerOffsetW,vf0);
        secondary.enabled=true;awake208_=0;word30_=0; //164FEC,165008,165018.
        vector40_=actual433AD0;vector50_=actual433AD0;
        copy(body.position,extra.position60W,actual433AD0);
        copy(body.velocity,secondary.velocity70W,actual433AD0);
        copy(root.localVelocity80,supplement.localVelocity80W,actual433AD0);
        copy(root.worldAngularVelocityA0,extra.worldAngularVelocityA0W,actual433AD0);
        copy(body.angularVelocity,secondary.angularVelocityB0W,actual433AD0);
        copy(body.linearAcceleration,supplement.linearAccelerationC0W,actual433AD0);
        copy(body.angularAcceleration,supplement.angularAccelerationD0W,actual433AD0);
        copy(root.positionCorrection1a0,extra.positionCorrection1A0W,actual433AD0);
        copy(root.rotationCorrection1b0,extra.rotationCorrection1B0W,actual433AD0);
        copy(root.accumulator1c0,extra.accumulator1C0W,actual433AD0);
        copy(root.accumulator1d0,extra.accumulator1D0W,actual433AD0);
        for(unsigned i=0;i<4;++i)body.orientation.v[i]=word(actual382B60[i]); //1650F8..165128.
        copy(secondary.cachedAnglesE0,*cachedAnglesE0W_,actual433AD0);
        p.body.linearAccelerationLimitSquared=word(0x4963d240u);
        p.body.angularAccelerationLimitSquared=word(0x48742400u);
        p.body.linearVelocityLimitSquared=word(0x468e6200u);
        p.body.angularVelocityLimitSquared=word(0x441c4000u);
        p.body.mass=0.f;p.body.inverseMass=0.f;secondary.cachedSpeed34=0.f;word284_=0;
        //Source14D7F0 local ZERO matrices, then distinct source100/140 copies.
        inertia100_.emplace();std::memset(inertia100_->v,0,sizeof inertia100_->v);
        unsigned extraIndex=0;
        for(unsigned column=0;column<4;++column)for(unsigned row=0;row<4;++row){
            if(column<3&&row<3)p.body.inverseInertiaColumns[column].v[row]=0.f;
            else p.inverseInertiaExtra[extraIndex++]=0.f;
        }
        vectorF0_=actual433AD0;bindings.secondaryDomain.integrationMethod290=GraphBodyIntegrator49W::OriginalAdamsBashforth2;
        copy(p.centerOffset,p.centerOffsetW,actual433AD0);
        count264_=0;count268_=0;allocation28C_=nullptr;scene288_=nullptr;secondary.scenePresent=false;
        secondary.body20C=0;root.callbackState238=0;
        entity_.kind=K::body167920;flags200_=0;
        for(unsigned i=0;i<3;++i)p.body.translationEnabled[i]=true;
        for(unsigned i=0;i<3;++i)p.body.rotationEnabled[i]=true;
        //167990/99C genuinely precede the repeated writes in167A50.
        copy(root.loads.force,secondary.force180W,actual433B80);
        copy(root.loads.torque,secondary.torque190W,actual433B80);
        SecondaryActivationGlobals49V globals;
        std::memcpy(globals.vector433B80,actual433B80.data(),16);
        std::memcpy(globals.quaternion382B60,actual382B60.data(),16);
        resetSecondaryActivation49V(secondary,supplement,globals); //167A50→167AD0, same canonical lanes.
        state.bodies.secondaryDomain.body320=0; //1679B8 AFTER reset.
        //Routing projection of the real empty Entity, not guessed descendants.
        state.bodies.secondaryDomain.bodyChildCount=0;
        bindings.secondaryDomain.componentFlags24=entity_.descendants24;
        bindings.secondaryDomain.firstComponentChildPresent18=entity_.firstChild18!=nullptr;
        bindings.secondaryDomain.callbackMethod9C=GraphBodyCallback49W::Clear238;
        //No scene matrix/cache,phase,time864,processed identity slots or joint
        //writes occur.212564 slot30=141C20 returns identity, no extra effects.
        stage_=Stage::constructed;boundary_=0x212554u;return {S::complete,boundary_};
    }
    bool initialize212564()noexcept {
        if(stage_!=Stage::constructed||initialized_||boundary_!=0x212554u)return false;
        //3CE000+30=141C20: actual identity return, no source stores.
        const auto identity141C20=[](C& entity)noexcept{return &entity;};
        if(identity141C20(entity_)!=&entity_)return false;
        initialized_=true;boundary_=0x21256cu;return true;
    }
    R append212574(const OriginalWreckComponentDomain50LA& domain,OriginalRacerEntityCold50LA& actualRacer)noexcept {
        if(stage_!=Stage::constructed||!initialized_||boundary_!=0x21256cu||appended_||!actualRacer.prepared()||!actualRacer.racer.reference00||
           actualRacer.racer.sessionGeneration!=entity_.sessionGeneration)return {S::owner,0x212574u};
        auto r=appendOriginalWreckComponent50LA(domain,actualRacer.racer,entity_,2u,application_->attachmentLock423F58());
        if(r.status==S::complete){appended_=true;racer_=&actualRacer;
            state_->bodies.secondaryDomain.registrationMask=entity_.mask20;boundary_=0x21257cu;}
        return r;
    }
    R bindScene21257C(const OriginalRacerSceneObject50LA& actual19C)noexcept {
        if(stage_!=Stage::constructed||!appended_||sceneBound_||boundary_!=0x21257cu||
           !actual19C.constructed||actual19C.released||actual19C.entity().reference00!=1u||
           actual19C.entity().sessionGeneration!=entity_.sessionGeneration||
           actual19C.entity().parent1C!=&racer_->racer||entity_.previous10!=&actual19C.entity())return {S::owner,0x21257cu};
        auto& p=parameters_->bodies.secondary.body;
        p.linearAccelerationLimitSquared=word(0x501502f9u); //21258C
        p.angularAccelerationLimitSquared=word(0x501502f9u); //212594
        p.linearVelocityLimitSquared=word(0x501502f9u); //21259C
        p.angularVelocityLimitSquared=word(0x501502f9u); //2125A4
        state_->bodies.secondaryDomain.body320=1; //2125AC
        scene288_=&actual19C;state_->bodies.secondary.scenePresent=true; //2125BC→1652E8, no retain.
        sceneBound_=true;boundary_=0x2125c4u;return {S::complete,boundary_};
    }
    //Private caller contract: actual2125D0 direct retained-model refresh and
    //2125EC..212640 arithmetic have returned on the SAME Bank slot. These
    //methods do not adopt authored future values or attest that earlier work.
    bool applyMass212650(float mass)noexcept {
        if(stage_!=Stage::constructed||!appended_||!sceneBound_||massAttempted_||
           boundary_!=0x2125c4u||!scene288_.has_value()||!*scene288_||
           !sceneBindingCurrent(**scene288_)||!racer_||!racer_->racer.reference00||
           entity_.parent1C!=&racer_->racer||entity_.reference00!=1u)return false;
        massAttempted_=true;boundary_=0x165300u;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(mass))return false;
        if(mass>0.f){
            auto& body=parameters_->bodies.secondary.body;
            body.mass=mass; //165318 before reciprocal; failure retains mass.
            boundary_=0x16532cu;volatile float inverse=1.f/body.mass;
            if(!std::isfinite(inverse))return false;
            body.inverseMass=inverse; //165330, not reached on native refusal.
        } //Nonpositive source165310 branch preserves both existing words.
        massComplete_=true;boundary_=0x212658u;return true;
    }
    bool applyInertia212664(const BodyMatrix48G& input)noexcept {
        if(stage_!=Stage::constructed||!appended_||!sceneBound_||!massComplete_||inertiaAttempted_||
           boundary_!=0x212658u||!inertia100_.has_value()||!scene288_.has_value()||!*scene288_||
           !sceneBindingCurrent(**scene288_)||!racer_||!racer_->racer.reference00||
           entity_.parent1C!=&racer_->racer||entity_.reference00!=1u)return false;
        inertiaAttempted_=true;boundary_=0x165340u;
        if(std::fegetround()!=FE_TONEAREST)return false;
        //Actual eight pair copies precede14E910. Do not finite-prevalidate or
        //rollback100; inverse arithmetic refusal preserves previous140.
        for(unsigned i=0;i<16;i+=2){std::uint32_t pair[2];
            std::memcpy(pair,input.v+i,8);std::memcpy(inertia100_->v+i,pair,8);}
        boundary_=0x16537cu;
        original_wreck_tuning_detail50LA::Math math;
        const auto inverse=math.inverse(*inertia100_);
        if(!math.valid)return false;
        auto& parameters=parameters_->bodies.secondary;
        constexpr unsigned extra[]={3,7,11,12,13,14,15};
        for(unsigned i=0;i<16;++i){
            if(i%4u<3u&&i/4u<3u)parameters.body.inverseInertiaColumns[i/4u].v[i%4u]=inverse.v[i];
            else for(unsigned j=0;j<7;++j)if(extra[j]==i)parameters.inverseInertiaExtra[j]=inverse.v[i];
        }
        //Singular finite input follows actual ZERO inverse return; no guessed
        //identity or1/diagonal substitute. Source14E910 v15 remainsZERO.
        inertiaComplete_=true;boundary_=0x21266cu;return true;
    }
    C* liveEntity()noexcept{return entity_.reference00?&entity_:nullptr;}
    bool appended()const noexcept{return appended_;}
    bool retired()const noexcept{return stage_==Stage::released;}
    const SecondaryBodyState49M* canonicalNumericAddress()const noexcept{return state_?&state_->bodies.secondary:nullptr;}
    bool sceneBindingCurrent(const OriginalRacerSceneObject50LA& scene)const noexcept {
        return !retired()&&sceneBound_&&scene288_.has_value()&&*scene288_==&scene&&scene.constructed&&!scene.released&&
            scene.entity().reference00&&scene.entity().sessionGeneration==entity_.sessionGeneration;
    }
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(retired())return true;
        if(stage_!=Stage::constructed||!application_||application_->attachmentLock423F58()||entity_.reference00!=1u||
           entity_.firstChild18||entity_.next14!=next||!allocation28C_.has_value()||*allocation28C_)return false;
        if(!appended_)return !parent&&!previous&&!next&&!entity_.parent1C&&!entity_.previous10;
        return racer_&&parent==&racer_->racer&&parent->reference00&&previous&&entity_.parent1C==parent&&
            entity_.previous10==previous&&previous->next14==&entity_&&entity_.mask20==2u&&
            (!next||(next->parent1C==parent&&next->previous10==&entity_));
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(retired())return {S::complete,0x167a2cu};
        if(!preflightClose142590(parent,previous,nullptr))return {S::topology,0x142590u};
        if(entity_.previous10)entity_.previous10->next14=entity_.next14;
        if(entity_.parent1C&&entity_.parent1C->firstChild18==&entity_)entity_.parent1C->firstChild18=entity_.next14;
        --entity_.reference00; //142590→104970→1679D0.
        entity_.kind=K::body164ED0; //165284; genuine28CNULL skips2E8AE0.
        entity_.kind=K::entity141300;--application_->objectCount423F60(); //1413E0.
        //No clear/free of borrowed288, numeric body or historical source35C.
        stage_=Stage::released;boundary_=0x167a2cu;return {S::complete,boundary_};
    }
public:
    OriginalRacerSecondaryBodyOwner50LA()=default;
    OriginalRacerSecondaryBodyOwner50LA(const OriginalRacerSecondaryBodyOwner50LA&)=delete;
    OriginalRacerSecondaryBodyOwner50LA& operator=(const OriginalRacerSecondaryBodyOwner50LA&)=delete;
    ~OriginalRacerSecondaryBodyOwner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
};
}

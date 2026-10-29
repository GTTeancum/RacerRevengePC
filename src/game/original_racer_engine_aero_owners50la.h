#pragma once
#include "original_racer_main_body_owner50la.h"
#include "native_racer_aero_owner50la.h"
#include <array>
#include <bit>
#include <optional>

namespace rr::game {
namespace original_racer_engine_aero_detail50LA {
using C=OriginalWreckComponent50LA;
inline bool tail(const C& c,const OriginalApplicationEntities50LA* app,bool appended,
    const C* parent,const C* previous,const C* next)noexcept {
    if(!app||app->attachmentLock423F58()||c.reference00!=1u||c.firstChild18||c.next14!=next)return false;
    if(!appended)return !parent&&!previous&&!next&&!c.parent1C&&!c.previous10;
    return parent&&c.parent1C==parent&&c.previous10==previous&&
        (previous?previous->next14==&c:parent->firstChild18==&c)&&
        (!next||(next->parent1C==parent&&next->previous10==&c));
}
inline void drop142590(C& c)noexcept {
    if(c.previous10)c.previous10->next14=c.next14;
    if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
    --c.reference00; //104970; dispatch-specific destruction follows.
}
inline float word(std::uint32_t w)noexcept{return std::bit_cast<float>(w);}
}
//Actual34C2E7510 counted owner. ALL existing numeric engine fields stay in the
//same main-body Bank lane; only absent W words/body30/source lifetime live here.
class OriginalRacerEngine34COwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;using R=OriginalWreckComponentResult50LA;
    C entity_;
    OriginalApplicationEntities50LA* app_=nullptr;
    const OriginalRacerMainBodyOwner50LA* expectedBody_=nullptr;
    const OriginalRacerMainBodyOwner50LA* body30_=nullptr;
    MainBodyState49K* main_=nullptr;MainBodyParameters49K* parameters_=nullptr;
    std::array<std::optional<std::uint32_t>,6> vectorW_{}; //80,90,A0,B0,C0,D0 only.
    std::optional<std::uint32_t> sourceWord5C_; //Absent from EngineParameters48D; no future AI receipt.
    bool constructed_=false,bound_=false,appended_=false,released_=false;
    R construct2E7510(OriginalApplicationEntities50LA& app,
        const OriginalRacerMainBodyOwner50LA& body,MainBodyState49K& main,
        MainBodyParameters49K& parameters,std::uint64_t generation,std::uint64_t key,
        const std::array<std::uint32_t,4>& actualOrigin446800)noexcept {
        if(constructed_||entity_.reference00||!generation||!key||body.entity().reference00!=1u||
           body.entity().sessionGeneration!=generation||body.canonicalNumericAddress()!=&main)
            return {S::owner,0x2120c8u};
        app_=&app;expectedBody_=&body;main_=&main;parameters_=&parameters;
        auto result=initializeOriginalWreckComponent50LA(entity_,K::engine2E7510,generation,key,true,
            app.objectCount423F60());
        if(result.status!=S::complete)return result;
        auto& p=parameters.engine;auto& c=main.components;
        Vector3_48C* vectors[]={&p.torqueBasis[0],&p.torqueBasis[1],&c.engine.localThrust[0],
            &c.engine.localThrust[1],&c.engine.torque[0],&c.engine.torque[1]};
        for(unsigned i=0;i<6;++i){*vectors[i]={{0,0,0}};vectorW_[i]=0x3f800000u;} //VF0 stores
        body30_=nullptr;main.selectedEngineFlags.engine34=0;main.selectedEngineFlags.engine38=0;
        c.feedback.engine3C=1;
        p.maxThrust=0;p.boostThrust=0;p.repairThrust=0;p.rampUpFilter=0;p.rampDownFilter=0;
        p.rampDownSpeed=0;p.theoreticalTopSpeed=0;
        sourceWord5C_=0u; //Actual2E7590, between58 and60 stores.
        p.boostSpeed=0;p.repairSpeed=0;
        c.bindings.engineDrive[0]=0;c.bindings.engineDrive[1]=0;
        c.engine.smoothedDrive[0]=0;c.engine.smoothedDrive[1]=0;
        for(unsigned i=0;i<6;++i){for(unsigned k=0;k<3;++k)
            vectors[i]->v[k]=original_racer_engine_aero_detail50LA::word(actualOrigin446800[k]);
            vectorW_[i]=actualOrigin446800[3];} //2E75B8..2E75F4
        constructed_=true;return {S::complete,0x2120d0u};
    }
    R bind2E7670(const OriginalRacerMainBodyOwner50LA& actual340)noexcept {
        if(!constructed_||bound_||released_||&actual340!=expectedBody_||
           actual340.canonicalNumericAddress()!=main_)return {S::owner,0x2120d8u};
        body30_=&actual340;bound_=true; //2E7674; tail141C20 returns THIS, no extra effect.
        return {S::complete,0x2120e0u};
    }
    R append2120E8(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(!bound_||appended_||released_)return {S::owner,0x2120e8u};
        auto result=appendOriginalWreckComponent50LA(domain,racer,entity_,0u,app_->attachmentLock423F58());
        if(result.status==S::complete)appended_=true;return result;
    }
    C* liveEntity()noexcept{return entity_.reference00?&entity_:nullptr;}
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        return released_||(constructed_&&original_racer_engine_aero_detail50LA::tail(entity_,app_,appended_,parent,previous,next));
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(released_)return {S::complete,0x2e766cu};
        if(!preflightClose142590(parent,previous,nullptr))return {S::topology,0x142590u};
        original_racer_engine_aero_detail50LA::drop142590(entity_);
        //2E7610->1413E0 has no retained-body release or numeric reset.
        entity_.kind=K::entity141300;--app_->objectCount423F60();released_=true;
        return {S::complete,0x2e766cu};
    }
public:
    OriginalRacerEngine34COwner50LA()=default;
    OriginalRacerEngine34COwner50LA(const OriginalRacerEngine34COwner50LA&)=delete;
    ~OriginalRacerEngine34COwner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
};

//Actual35016CCD0 owner, composed around the SAME NativeRacerAeroOwner50LA
//reserved with FreshSourceTag (not a new flow facade or copied body parameters).
class OriginalRacerAero350Owner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;using R=OriginalWreckComponentResult50LA;
    C entity_;
    NativeRacerAeroOwner50LA* numeric_=nullptr;
    OriginalApplicationEntities50LA* app_=nullptr;
    const OriginalRacerMainBodyOwner50LA* body30_=nullptr;
    std::array<std::optional<float>,28> absentBase5C_{}; //Only offsets absent from canonical AeroParameters engaged.
    std::optional<float> field54_,field58_;
    std::optional<std::uint32_t> dampingW13C_;
    bool constructed_=false,bound_=false,appended_=false,released_=false;
    void zeroBaseCoefficient100430(unsigned index,AeroParameters48E& p)noexcept {
        switch(index){
        case 3:p.offset68=0;break;case 4:p.lift6c=0;break;case 7:p.drag78=0;break;
        case 10:p.pitch84=0;break;case 11:p.pitchDamping88=0;break;case 13:p.lateral90=0;break;
        case 21:p.rollDampingB0=0;break;case 23:p.timerYawB8=0;break;case 27:p.yawDampingC8=0;break;
        default:absentBase5C_[index]=0;break;
        }
    }
    R construct16CCD0(OriginalApplicationEntities50LA& app,NativeRacerAeroOwner50LA& numeric,
        std::uint64_t generation,std::uint64_t key,
        const std::array<std::uint32_t,4>& actualFlow3D6B40,
        const std::array<std::uint32_t,4>& actualDamping433BD0)noexcept {
        if(constructed_||entity_.reference00||!generation||!key||!numeric.sourceFresh_||numeric.sourceConstructorStarted_||
           numeric.sourceConstructorComplete_||numeric.published_||numeric.revoked_||
           !numeric.frame_||!numeric.parameters_||!numeric.body_||numeric.application_!=&app||
           numeric.generation_!=generation||numeric.body_->entity().reference00!=1u||
           numeric.body_->entity().sessionGeneration!=generation||
           numeric.body_->canonicalNumericAddress()!=&numeric.frame_->bodies.main)return {S::owner,0x212104u};
        numeric_=&numeric;app_=&app;numeric.sourceConstructorStarted_=true;
        auto r=initializeOriginalWreckComponent50LA(entity_,K::aeroBase100330,generation,key,true,
            app.objectCount423F60()); //10033C
        if(r.status!=S::complete)return r;
        auto& c=numeric.frame_->bodies.main.components;auto& p=numeric.parameters_->aero;
        numeric.flow40Words_={{0,0,0,0x3f800000u}}; //10035C VF0 before actual global copy.
        body30_=nullptr;numeric.flowEnabled34_=0;numeric.flow40Words_=actualFlow3D6B40;
        c.bindings.aeroAux50=0;field54_=0;field58_=0;
        for(unsigned i=0;i<28;++i)zeroBaseCoefficient100430(i,p);
        entity_.kind=K::aero16CCD0; //16CCEC dispatch replacement, same counted Entity.
        p.velocityDamping130={{0,0,0}};dampingW13C_=0x3f800000u;
        c.feedback.aeroD0=0;c.feedback.aeroD4=0;
        //NO16CCD0 write toD8: preserve actual constructor-unknown lane.
        c.aero.lowSpeedDc=false;c.feedback.aeroE0=1;
        c.bindings.aeroDrive[0]=0;c.bindings.aeroDrive[1]=0;c.bindings.aeroBrakeEC=0;
        c.bindings.aeroAuxF0=0;c.bindings.aeroAuxF4=0;
        p.driveDragF8=0;p.brakeDragFc=0;p.steering100=0;p.steeringRamp104=1;
        p.timerSteering108=0;p.timerLateral10c=0;p.timerYaw110=0;p.timerDrag114=0;
        p.tiltedLateral118=original_racer_engine_aero_detail50LA::word(0xc1a00000u);
        p.tiltedSteering11c=original_racer_engine_aero_detail50LA::word(0xc4fa0000u);
        p.tiltedYaw120=original_racer_engine_aero_detail50LA::word(0x43960000u);
        for(unsigned k=0;k<3;++k)p.velocityDamping130.v[k]=
            original_racer_engine_aero_detail50LA::word(actualDamping433BD0[k]);
        dampingW13C_=actualDamping433BD0[3]; //16CD68 complete source vector copy.
        c.aero.timer140=0;c.aero.timer144=0;
        constructed_=true;numeric.sourceConstructorComplete_=true;return {S::complete,0x21210cu};
    }
    R bind100420(const OriginalRacerMainBodyOwner50LA& actual340)noexcept {
        if(!constructed_||bound_||released_||!numeric_||numeric_->body_!=&actual340||
           actual340.canonicalNumericAddress()!=&numeric_->frame_->bodies.main)return {S::owner,0x21211cu};
        body30_=&actual340;bound_=true; //100424 then actual141C20 identity return.
        return {S::complete,0x212124u};
    }
    R append21212C(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(!bound_||appended_||released_)return {S::owner,0x21212cu};
        auto r=appendOriginalWreckComponent50LA(domain,racer,entity_,2u,app_->attachmentLock423F58());
        if(r.status==S::complete)appended_=true;return r;
    }
    C* liveEntity()noexcept{return entity_.reference00?&entity_:nullptr;}
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        return released_||(constructed_&&numeric_&&numeric_->sourceConstructorComplete_&&
            original_racer_engine_aero_detail50LA::tail(entity_,app_,appended_,parent,previous,next));
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(released_)return {S::complete,0x16d8dcu};
        if(!preflightClose142590(parent,previous,nullptr))return {S::topology,0x142590u};
        original_racer_engine_aero_detail50LA::drop142590(entity_);
        entity_.kind=K::aeroBase100330; //16D8A8->1003C0
        entity_.kind=K::entity141300;--app_->objectCount423F60(); //1003E8->1413E0
        numeric_->revoked_=true; //End native consumer authority AFTER actual source destruction.
        released_=true;return {S::complete,0x16d8dcu};
    }
public:
    OriginalRacerAero350Owner50LA()=default;
    OriginalRacerAero350Owner50LA(const OriginalRacerAero350Owner50LA&)=delete;
    ~OriginalRacerAero350Owner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
};
}

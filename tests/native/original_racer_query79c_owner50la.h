#pragma once
#include "../../src/game/original_collision_base_owner50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/trigger_sphere50in.h"
#include <array>
#include <bit>
#include <cstring>
#include <memory>

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
//Actual79C11C0B0 is shape5, NOT shape1. Owns its real shape/result lifetime;
//uses existing TriggerSphereShape numeric layout, not a camera query facade.
class OriginalRacerQuery79COwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;using S=OriginalWreckComponentStatus50LA;using R=OriginalWreckComponentResult50LA;
    OriginalCollisionBaseOwner50LA base_;
    struct Shape {
        TriggerSphereShape50IN numeric; //Native backing; only reached stores below grant knowledge.
        std::uint8_t sourceKnownMask=0; //1=center,2=radius,4=radiusSquared,8/16=scales,32=matrix.
        std::optional<const void*> owner60; //Unknown until actual11C180 NULL store.
    };
    struct Result {
        std::uint32_t capacity00=0,count04=0;
        std::optional<float> planeDistance08; //NEVER written by11C0B0.
        SpringVector4_48N center20{};
        std::array<SpringVector4_48N,3> normals30{},points60{};
    };
    std::unique_ptr<Shape> shape8C_;
    std::unique_ptr<Result> result90_;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    bool configured_=false,shapeAttempted_=false,shapeComplete_=false;
    std::uint32_t boundary_=0;
    R construct21247C(OriginalApplicationEntities50LA& app,std::uint64_t generation,std::uint64_t key,
        const SpringVector4_48N& actualSentinel3E02C0,std::uint8_t nativeFlags31,std::uint8_t nativeFlagsF8)noexcept {
        if(base_.phase_!=OriginalCollisionBaseOwner50LA::Phase::fresh||base_.entity_.reference00||shapeAttempted_)
            return {S::owner,0x212468u};
        base_.entity_.flags29=nativeFlags31;base_.flagsF8_=nativeFlagsF8;boundary_=0x21247cu;
        auto r=base_.construct11B190(app,generation,key,actualSentinel3E02C0);
        if(r.status==S::complete)boundary_=0x212484u;
        return r; //Caller publishes79C at212484 BEFORE mode/category/shape.
    }
    bool configure21248C(std::uint8_t& actualGlobal3E0730,std::uint32_t actualTrigger3E0728)noexcept {
        if(boundary_!=0x212484u||configured_||!base_.freshShapeNull())return false;
        //11B570(2): original bit8/bit2/bit4 RMW stores, no other flags changed.
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_&~8u); //11B594
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_|2u); //11B5C0
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_&~4u); //11B5E4
        actualGlobal3E0730=0; //11B5F4
        base_.categoryA4_=actualTrigger3E0728; //2124B0
        configured_=true;boundary_=0x2124b4u;return true;
    }
    bool shape11C0B0_2124B8(const std::array<std::uint32_t,4>& actual434630,float actualRadius)noexcept {
        if(!configured_||shapeAttempted_||boundary_!=0x2124b4u||!base_.freshShapeNull()||
           std::bit_cast<std::uint32_t>(actualRadius)!=0x447a0000u)return false; //Actual caller1000.f only.
        shapeAttempted_=true;
        SpringVector4_48N local;std::memcpy(&local,actual434630.data(),16); //11C0C8/E0 local copy.
        boundary_=0x11c0dcu;base_.shape8C_=nullptr;base_.result90_=nullptr; //GenuineNULL11B600 branch.
        boundary_=0x11c0e4u;
        try {shape8C_=std::make_unique<Shape>();}catch(...){return false;}
        shape8C_->numeric.localCenter={{0,0,0,1}};shape8C_->sourceKnownMask|=1u; //11C0F4 only source shape ctor vector.
        base_.shape8C_=shape8C_.get(); //11C0F8 BEFOREtag/result allocation.
        base_.shape88_=5u;boundary_=0x11c104u; //11C108 actual tag5.
        try {result90_=std::make_unique<Result>();}catch(...){return false;}
        auto& result=*result90_;const SpringVector4_48N vf0{{0,0,0,1}};
        result.center20=vf0; //11C11C
        for(auto& vector:result.normals30)vector=vf0; //11C134->2E8860,3x1048E0
        for(auto& vector:result.points60)vector=vf0; //11C150->2E8860,3x1048E0
        //1000*1000 is exactly representable; no finite refusal can erase the
        //preceding source allocation prefix. Other radii are not admitted.
        volatile float squared=actualRadius*actualRadius; //11C160
        result.capacity00=3u;result.count04=0u; //11C164/168;08 staysunknown.
        base_.result90_=result90_.get(); //11C16C only after result constructor.
        auto& shape=*shape8C_;shape.owner60=nullptr; //11C180 sourcea2=NULL.
        shape.numeric.localCenter=local; //11C18C actual434630.
        shape.numeric.radius50=actualRadius;shape.sourceKnownMask|=2u; //11C194
        shape.numeric.radiusSquared54=squared;shape.sourceKnownMask|=4u; //11C19C
        shape.numeric.radiusScale58=1.f;shape.sourceKnownMask|=8u; //11C1A4
        shape.numeric.centerScale5C=1.f;shape.sourceKnownMask|=16u; //11C1AC
        std::memset(shape.numeric.world.v,0,sizeof(shape.numeric.world.v)); //306478 at11C1B8
        shape.numeric.world.v[15]=1.f;shape.numeric.world.v[10]=1.f;
        shape.numeric.world.v[5]=1.f;shape.numeric.world.v[0]=1.f; //11C1C4/C8/CC/D0
        shape.sourceKnownMask|=32u;
        shapeComplete_=true;boundary_=0x2124c0u;return true;
    }
    R append2124D4(const OriginalWreckComponentDomain50LA& domain,OriginalRacerEntityCold50LA& actualRacer)noexcept {
        if(!shapeComplete_||!shape8C_||shape8C_->sourceKnownMask!=63u||
           boundary_!=0x2124c0u||base_.appended_||!actualRacer.prepared()||
           !actualRacer.racer.reference00||actualRacer.racer.sessionGeneration!=base_.entity_.sessionGeneration)
            return {S::owner,0x2124d4u};
        auto r=base_.append141960(domain,actualRacer.racer,0xffffffffu);
        if(r.status==S::complete){racer_=&actualRacer;boundary_=0x2124dcu;}return r;
    }
    C* liveEntity()noexcept{return base_.entity_.reference00?&base_.entity_:nullptr;}
    bool appended()const noexcept{return base_.appended_;}
    bool retired()const noexcept{return base_.phase_==OriginalCollisionBaseOwner50LA::Phase::released;}
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(retired())return true;
        if(!base_.preflightClose142590(parent,previous,next)||!base_.constructorNullListsCurrent()||
           !base_.owner9C_.has_value()||*base_.owner9C_||!base_.bodyA0_.has_value()||*base_.bodyA0_||
           (base_.appended_&&(!racer_||parent!=&racer_->racer||!racer_->racer.reference00||
             racer_->racer.sessionGeneration!=base_.entity_.sessionGeneration)))return false;
        for(const auto& callback:base_.callbacksE4_)if(!callback.has_value()||*callback)return false;
        if(!shape8C_)return !result90_&&base_.freshShapeNull();
        if(base_.shape88_!=5u||!base_.shape8C_.has_value()||*base_.shape8C_!=shape8C_.get()||
           !base_.result90_.has_value()||*base_.result90_!=result90_.get())return false;
        //Tag5 destructor owns no inner array/resource pointers. A genuinely
        //published shape with result allocation stopped/NULL is closable.
        return !result90_||(shapeComplete_&&shape8C_->sourceKnownMask==63u);
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(retired())return {S::complete,0x11b568u};
        if(!preflightClose142590(parent,previous,nullptr)||!base_.dropTailReference142590(parent,previous))
            return {S::topology,0x142590u};
        shape8C_.reset(); //Dispatchtable3888F0[tag5]=11B8E0, actualfree11B8E4.
        result90_.reset(); //11B8F8 ifnonnull; inlinevectors need no separatefrees.
        base_.shape8C_=nullptr;base_.result90_=nullptr; //11B900/904 actual sourceNULLstores.
        auto r=base_.retireBase1413E0AfterPayload();
        if(r.status==S::complete)boundary_=0x11b568u;return r;
    }
public:
    OriginalRacerQuery79COwner50LA()=default;
    OriginalRacerQuery79COwner50LA(const OriginalRacerQuery79COwner50LA&)=delete;
    OriginalRacerQuery79COwner50LA& operator=(const OriginalRacerQuery79COwner50LA&)=delete;
    ~OriginalRacerQuery79COwner50LA(){if(shape8C_||result90_)std::terminate();}
    const C& entity()const noexcept{return base_.entity();}
};
}

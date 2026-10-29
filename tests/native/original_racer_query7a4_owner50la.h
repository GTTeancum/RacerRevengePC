#pragma once
#include "../../src/game/original_collision_base_owner50la.h"
#include "../../src/game/original_wreck_type3_owners50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
//Actual counted7A4 constructor path. Type3 allocations below are canonical;
//inactive Fresh fixed arrays are NOT mirrored/adopted or granted readiness.
class OriginalRacerQuery7A4Owner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;using S=OriginalWreckComponentStatus50LA;using R=OriginalWreckComponentResult50LA;
    OriginalCollisionBaseOwner50LA base_;
    OriginalWreckType3Owners50LA payload_;
    OriginalWreckCategoryList50LA categories_;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    enum class SourceCallbackE4 {none,movement20EE10};
    SourceCallbackE4 callbackE4_=SourceCallbackE4::none;
    bool modeApplied_=false,receiverBound_=false,shapeAttempted_=false,shapeComplete_=false,categoryAttempted_=false,categoryComplete_=false;
    std::uint32_t boundary_=0;
    R construct2123DC(OriginalApplicationEntities50LA& app,std::uint64_t generation,std::uint64_t key,
        const SpringVector4_48N& actualSentinel3E02C0,std::uint8_t nativeFlags31,std::uint8_t nativeFlagsF8)noexcept {
        if(base_.phase_!=OriginalCollisionBaseOwner50LA::Phase::fresh||base_.entity_.reference00||payload_.attempted())
            return {S::owner,0x2123c8u};
        //Explicit native allocation carriers, not recovered original heap bits.
        base_.entity_.flags29=nativeFlags31;base_.flagsF8_=nativeFlagsF8;boundary_=0x2123dcu;
        auto r=base_.construct11B190(app,generation,key,actualSentinel3E02C0);
        if(r.status==S::complete)boundary_=0x2123e4u;
        return r; //Caller publishes actualRacer7A4 only at2123E4.
    }
    bool mode3_2123EC(std::uint8_t& actualGlobal3E0730)noexcept {
        if(boundary_!=0x2123e4u||modeApplied_||!base_.freshShapeNull())return false;
        if(!base_.setMode3_11B570(actualGlobal3E0730))return false;
        modeApplied_=true;boundary_=0x2123f4u;return true;
    }
    R append212408(const OriginalWreckComponentDomain50LA& domain,OriginalRacerEntityCold50LA& actualRacer)noexcept {
        if(!modeApplied_||boundary_!=0x2123f4u||base_.appended_||!actualRacer.prepared()||
           !actualRacer.racer.reference00||actualRacer.racer.sessionGeneration!=base_.entity_.sessionGeneration)
            return {S::owner,0x212408u};
        auto r=base_.append141960(domain,actualRacer.racer,0u);
        if(r.status==S::complete){racer_=&actualRacer;boundary_=0x212410u;}
        return r; //Actual append PRECEDES owner9C/E4/category/shape stores.
    }
    bool bindReceiver212424(const OriginalRacerEntityCold50LA& actualRacer,std::uint32_t actualPhysicsBody3E0718)noexcept {
        if(boundary_!=0x212410u||receiverBound_||!base_.appended_||racer_!=&actualRacer||
           !racerCurrent()||!base_.freshShapeNull())return false;
        base_.owner9C_=&actualRacer; //212424
        base_.callbacksE4_[0].reset();callbackE4_=SourceCallbackE4::movement20EE10; //212430 typed alternative.
        base_.categoryA4_=actualPhysicsBody3E0718; //21243C; no20EE10 execution.
        receiverBound_=true;boundary_=0x212440u;return true;
    }
    bool shape11C710_212444(const SpringVector4_48N& actualSentinel3E02C0)noexcept {
        if(boundary_!=0x212440u||!receiverBound_||shapeAttempted_||!racerCurrent()||!base_.freshShapeNull())return false;
        shapeAttempted_=true;boundary_=0x11c738u;
        //Actual11B600 on genuine constructorNULL; no prior native shape adoption.
        base_.shape8C_=nullptr;base_.result90_=nullptr; //11B900/904
        if(!payload_.allocate11C710(actualSentinel3E02C0,&base_.shape8C_,&base_.shape88_,&base_.result90_)){
            boundary_=payload_.sourceBoundary();return false;
        }
        shapeComplete_=true;boundary_=0x21244cu;return true;
    }
    bool mode0StaticCategory212450(std::uint32_t actualStaticObject3E0720)noexcept {
        if(boundary_!=0x21244cu||!shapeComplete_||categoryAttempted_||!racerCurrent())return false;
        categoryAttempted_=true;boundary_=0x212450u;
        if(!categories_.selectFreshMode0124160(base_.entity_,base_.listD8_,base_.countDC_,base_.modeD4_))return false;
        boundary_=0x124214u;
        if(!categories_.appendMode0124170(base_.listD8_,base_.countDC_,base_.modeD4_,actualStaticObject3E0720))return false;
        categoryComplete_=true;boundary_=0x212468u;return true;
    }
    bool racerCurrent()const noexcept {
        return racer_&&racer_->racer.reference00&&racer_->racer.sessionGeneration==base_.entity_.sessionGeneration&&
            (!base_.appended_||base_.entity_.parent1C==&racer_->racer);
    }
    bool receiverCurrent()const noexcept {
        if(!base_.bodyA0_.has_value()||*base_.bodyA0_)return false;
        for(std::size_t i=1;i<base_.callbacksE4_.size();++i)
            if(!base_.callbacksE4_[i].has_value()||*base_.callbacksE4_[i])return false;
        return receiverBound_?(racerCurrent()&&base_.owner9C_.has_value()&&*base_.owner9C_==racer_&&
            !base_.callbacksE4_[0].has_value()&&callbackE4_==SourceCallbackE4::movement20EE10):
            (base_.owner9C_.has_value()&&!*base_.owner9C_&&base_.callbacksE4_[0].has_value()&&
             !*base_.callbacksE4_[0]&&callbackE4_==SourceCallbackE4::none);
    }
    C* liveEntity()noexcept{return base_.entity_.reference00?&base_.entity_:nullptr;}
    bool appended()const noexcept{return base_.appended_;}
    bool retired()const noexcept{return base_.phase_==OriginalCollisionBaseOwner50LA::Phase::released;}
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(retired())return true;
        if(!base_.preflightClose142590(parent,previous,next)||!receiverCurrent()||
           (base_.appended_&&(!racerCurrent()||parent!=&racer_->racer))||
           !base_.membershipE0_.has_value()||*base_.membershipE0_||base_.listC0_||base_.listCC_||base_.countC4_||base_.countD0_)return false;
        if(categories_.mode0Route_){
            if(!categories_.matchesMode0(base_.listD8_,base_.countDC_,base_.modeD4_))return false;
        }else if(!base_.constructorNullListsCurrent())return false;
        if(!payload_.attempted())return base_.freshShapeNull();
        if(!payload_.canRelease11B600())return false; //Never fakeNULLs for partial shape/result arrays.
        if(!payload_.loaded())return base_.freshShapeNull()&&!payload_.shape8C_&&!payload_.result90_;
        return shapeComplete_&&base_.shape88_==3u&&base_.shape8C_.has_value()&&
            *base_.shape8C_==payload_.shape8C_.get()&&base_.result90_.has_value()&&*base_.result90_==payload_.result90_.get();
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(retired())return {S::complete,0x11b568u};
        if(!preflightClose142590(parent,previous,nullptr)||!base_.dropTailReference142590(parent,previous))
            return {S::topology,0x142590u};
        if(categories_.mode0Route_&&!categories_.releaseMode0AfterReferenceDrop11B51C(
            base_.entity_,base_.listD8_,base_.countDC_,base_.modeD4_))std::terminate();
        if(payload_.attempted()&&!payload_.release11B600())std::terminate();
        base_.shape8C_=nullptr;base_.result90_=nullptr; //Actual11B900/904 after11B600 payload disposal.
        auto r=categories_.mode0Route_?base_.retireBase1413E0AfterMode0Payload(categories_):base_.retireBase1413E0AfterPayload();
        //SourceRacer7A4/9C/E4/D8/DC/D4 history remains. No coldBound, query
        //membership, native fixed arrays, radius/length/hit validity granted.
        if(r.status==S::complete)boundary_=0x11b568u;return r;
    }
public:
    OriginalRacerQuery7A4Owner50LA()=default;
    OriginalRacerQuery7A4Owner50LA(const OriginalRacerQuery7A4Owner50LA&)=delete;
    OriginalRacerQuery7A4Owner50LA& operator=(const OriginalRacerQuery7A4Owner50LA&)=delete;
    const C& entity()const noexcept{return base_.entity();}
};
}

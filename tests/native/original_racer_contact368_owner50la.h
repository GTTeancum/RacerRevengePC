#pragma once
#include "original_racer_query364_owner50la.h"
#include "../../src/game/original_wreck_record_array50la.h"
#include "../../src/game/spring_model48k.h"
#include <array>
#include <bit>
#include <cstring>
#include <vector>

namespace rr::game {
class OriginalRacerDetector36COwner50LA;
//ONE actual295A40 owner at Racer368. Numeric settings are references to the
//same fresh Bank secondarySpring/secondaryResponse lanes, never WreckPhysical copies.
class OriginalRacerContact368Owner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class OriginalRacerDetector36COwner50LA;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    C entity_;
    OriginalApplicationEntities50LA* app_=nullptr;
    SpringGeneratedParameters48K* generated_=nullptr;
    SpringResponseParameters48L* response_=nullptr;
    const std::array<std::uint32_t,4>* origin4435A0_=nullptr; //Same App global, not a copied owner.
    const OriginalRacerSecondaryBodyOwner50LA* expectedBody_=nullptr;
    const OriginalRacerSecondaryBodyOwner50LA* body30_=nullptr;
    const OriginalRacerQuery364Owner50LA* query364_=nullptr;
    const OriginalRacerSceneObject50LA* scene19C_=nullptr;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    //Fields absent from existing Bank numeric settings, not parallel settings.
    //368+3C/+44 have no same-Bank secondary persistent state in current consumers.
    SecondaryBodyState49M* secondary_=nullptr;
    struct AbsentRecordState {
        std::uint32_t pending38=0,contact3C=0,upward40=0,other44=0,point74=0,generated78=0,currentCandidate130=0,pointCount7C=0;
        std::int32_t generatedCount84=0,generatedCapacity88=0;
        std::uint32_t lastMaterialD4=0;
    } state_;
    std::uint32_t mode34_=0;
    float customCenter100W_=0,worldCenter110W_=0,contactVelocity120W_=0,normalVelocityScaleAC_=0;
    OriginalWreckRecordArray50LA pointRecords_,generatedRecords_;
    std::optional<const SpringRecord48N*> source80_,source8C_; //Engaged only by ctor stores; stale afterfree.
    //Typed source scene40 alternate. Numeric SpringSceneId is never used as
    //a fake pointer. Only i<count7C has a source-written point/scene identity.
    std::array<const OriginalRacerSceneObject50LA*,60> pointScenes_{};
    std::array<std::uint8_t,60> pointFlagsKnownMask_{};
    std::vector<std::uint16_t> traversal_; //Immutable topology workspace, not source records.
    bool constructorAttempted_=false,constructed_=false,allocationAttempted_=false,arraysComplete_=false;
    bool pointsAttempted_=false,pointsComplete_=false,overridesApplied_=false,appended_=false,retired_=false;
    std::uint32_t boundary_=0;
    static float word(std::uint32_t value)noexcept{return std::bit_cast<float>(value);}
    static bool freshNumeric(const SpringGeneratedParameters48K& g,const SpringResponseParameters48L& r)noexcept {
        const auto zero=[](float v){return std::bit_cast<std::uint32_t>(v)==0u;};
        const auto& a=g.impact;
        for(auto v:a.customLocalCenter100.v)if(!zero(v))return false;
        for(auto v:a.worldCenter110.v)if(!zero(v))return false;
        for(auto v:a.contactVelocity120.v)if(!zero(v))return false;
        const float values[]={a.normalForceScale98,a.angularImpulseScaleB0,a.forceTorqueScaleB4,
            a.restitutionB8,a.restitutionBC,a.restitutionC0,a.positionCorrectionScaleCC,a.angularCorrectionScaleD0,
            g.frictionTorqueScaleC4,r.maxPenetration90,r.maxSpeed94,r.stiffness9C,r.dampingA0,r.impactStiffnessA4,r.impactDampingA8};
        for(auto v:values)if(!zero(v))return false;
        for(auto v:g.frictionByMaterial)if(!zero(v))return false;
        return !a.disableLeverArms5C&&!a.correctPenetration64&&!a.yawOnly6C&&!a.customCenter70&&
            !g.pointContacts48&&!g.springForce4C&&!g.friction50&&!g.impact54&&!g.otherSpring58&&!g.secondary68&&!r.secondaryAtLimit60;
    }
    //Reserve native traversal only against already-authentic type2 allocation.
    //No295A40/POINT/field publication occurs here; no warming/replay admitted.
    bool reservePointTraversal(const OriginalRacerQuery364Owner50LA& query)noexcept {
        if(constructorAttempted_||!traversal_.empty()||!query.payload_.loaded()||!query.sourceBindingCurrent()||
           !query.appended()||query.boundary_!=0x2127c8u)return false;
        const auto resource=query.payload_.resource();
        if(!resource.nodeCount||!resource.rayCount||resource.nodeCount>32767u||resource.rayCount>32768u)return false;
        try {traversal_.resize(resource.nodeCount+resource.rayCount);}catch(...){return false;}
        query364_=&query;return true;
    }
    R construct295A40(OriginalApplicationEntities50LA& app,const OriginalRacerSecondaryBodyOwner50LA& actualBody,
        SecondaryBodyState49M& actualSecondary,SpringGeneratedParameters48K& actualSecondarySpring,SpringResponseParameters48L& actualSecondaryResponse,
        std::uint64_t generation,std::uint64_t key,const std::array<std::uint32_t,4>& actual4435A0,
        std::uint8_t nativeFlags29)noexcept {
        if(constructorAttempted_||!query364_||!query364_->sourceBindingCurrent()||
           !freshNumeric(actualSecondarySpring,actualSecondaryResponse)||!generation||!key||
           actualBody.entity().reference00!=1u||actualBody.entity().sessionGeneration!=generation||
           actualBody.canonicalNumericAddress()!=&actualSecondary||
           query364_->body35C_!=&actualBody||query364_->entity().sessionGeneration!=generation)return {S::owner,0x2127dcu};
        constructorAttempted_=true;app_=&app;expectedBody_=&actualBody;
        secondary_=&actualSecondary;generated_=&actualSecondarySpring;response_=&actualSecondaryResponse;origin4435A0_=&actual4435A0;boundary_=0x295a4cu;
        //Native allocation carrier, not an assertion of retail heap upperbits.
        entity_.flags29=nativeFlags29;
        auto r=initializeOriginalWreckComponent50LA(entity_,K::contact295A40,generation,key,true,app.objectCount423F60());
        if(r.status!=S::complete)return r;
        if(!pointRecords_.selectSource()||!generatedRecords_.selectSource())std::terminate();
        auto& g=*generated_;auto& a=g.impact;auto& response=*response_;
        a.customLocalCenter100={{0,0,0}};customCenter100W_=1.f; //295A64
        a.worldCenter110={{0,0,0}};worldCenter110W_=1.f; //295A6C
        a.contactVelocity120={{0,0,0}};contactVelocity120W_=1.f; //295A74
        state_.pointCount7C=0;state_.generatedCount84=0;state_.generatedCapacity88=0;
        body30_=nullptr;source80_=nullptr;source8C_=nullptr;state_.currentCandidate130=0;
        mode34_=0;state_.contact3C=0;state_.pending38=0;
        a.customCenter70=false;g.pointContacts48=true;g.springForce4C=true;g.impact54=true;g.friction50=true;
        a.disableLeverArms5C=false;g.otherSpring58=false;response.secondaryAtLimit60=false;
        a.correctPenetration64=false;g.secondary68=false;a.yawOnly6C=false;
        state_.point74=0;state_.generated78=0;state_.upward40=0;state_.other44=0;
        response.maxPenetration90=0;response.maxSpeed94=word(0x461c4000u);
        response.stiffness9C=0;response.dampingA0=0;response.impactStiffnessA4=0;response.impactDampingA8=0;
        normalVelocityScaleAC_=1.f;a.angularImpulseScaleB0=1.f;a.forceTorqueScaleB4=1.f;g.frictionTorqueScaleC4=1.f;
        a.positionCorrectionScaleCC=word(0x3d4ccccdu);a.angularCorrectionScaleD0=0;a.normalForceScale98=1.f;
        std::memcpy(a.customLocalCenter100.v,actual4435A0.data(),12);std::memcpy(&customCenter100W_,actual4435A0.data()+3,4); //295B34
        std::memcpy(a.contactVelocity120.v,actual4435A0.data(),12);std::memcpy(&contactVelocity120W_,actual4435A0.data()+3,4); //295B44
        for(auto& f:g.frictionByMaterial)f=0; //295B54 x10
        state_.lastMaterialD4=0;a.restitutionB8=0;a.restitutionC0=0;a.restitutionBC=0;
        constructed_=true;boundary_=0x2127e4u;return {S::complete,boundary_};
    }
    bool allocate295C20()noexcept {
        if(!constructed_||allocationAttempted_||retired_||boundary_!=0x2127e4u)return false;
        allocationAttempted_=true;state_.generatedCapacity88=60; //295C2C
        boundary_=0x295c40u;if(!pointRecords_.allocate295C20(60u))return false;
        source80_=pointRecords_.data(); //295C48
        boundary_=0x295c58u;if(!generatedRecords_.allocate295C20(60u))return false;
        source8C_=generatedRecords_.data(); //295C60;141C20 identity return.
        arraysComplete_=true;boundary_=0x2127f4u;return true;
    }
    bool bind295C80(const OriginalRacerSecondaryBodyOwner50LA& actual35C)noexcept {
        if(!arraysComplete_||body30_||retired_||boundary_!=0x2127f4u||&actual35C!=expectedBody_||
           !actual35C.entity().reference00)return false;
        body30_=&actual35C;boundary_=0x212800u;return true; //295C84
    }
    bool sourceInputsCurrent()const noexcept {
        return query364_&&query364_->sourceBindingCurrent()&&query364_->payload_.loaded()&&query364_->appended()&&
            query364_->body35C_==expectedBody_&&expectedBody_&&expectedBody_->entity().reference00&&
            expectedBody_->canonicalNumericAddress()==secondary_&&
            (!racer_||(query364_->racer_==racer_&&query364_->scene19C_==scene19C_));
    }
    bool appendPoint295E00(const SpringVector4_48N& point,const OriginalRacerSceneObject50LA& actual19C,
        const std::array<std::uint32_t,4>& actual4435A0)noexcept {
        if(state_.pointCount7C>=60u)return true; //Original signed capacity check BEFORE dedup (60 for368).
        for(std::size_t i=0;i<state_.pointCount7C;++i){const auto& old=pointRecords_[i];
            if(old.localPoint10.v[0]==point.v[0]&&old.localPoint10.v[1]==point.v[1]&&
               old.localPoint10.v[2]==point.v[2]&&pointScenes_[i]==&actual19C)return true;}
        const auto i=state_.pointCount7C;auto& record=pointRecords_[i];
        pointScenes_[i]=&actual19C; //295EFC typed source scene40, no numeric pointer surrogate.
        std::memcpy(&record.localPoint10,&point,sizeof(point)); //295F24 preserves all four endpoint words.
        record.flags50=static_cast<std::uint8_t>((record.flags50&~1u)|1u);pointFlagsKnownMask_[i]=1u; //295F4C
        record.margin48=0.f; //295F68
        std::memcpy(&record.normal20,actual4435A0.data(),16); //295F88
        std::memcpy(&record.surfacePoint30,actual4435A0.data(),16); //295FAC
        ++state_.pointCount7C;return true; //295FB8;field00/other44/material4C remain unwritten.
    }
    bool producePoints296190(const OriginalRacerQuery364Owner50LA& actual364,
        const OriginalRacerEntityCold50LA& actualRacer,const OriginalRacerSceneObject50LA& actual19C,
        const std::array<std::uint32_t,4>& actual4435A0)noexcept {
        if(!arraysComplete_||!body30_||pointsAttempted_||retired_||boundary_!=0x212800u||
           &actual364!=query364_||&actual4435A0!=origin4435A0_||!sourceInputsCurrent()||actual364.racer_!=&actualRacer||
           actual364.scene19C_!=&actual19C||
           actual364.body35C_!=body30_||actual364.boundary_!=0x2127c8u)return false;
        racer_=&actualRacer;scene19C_=&actual19C;pointsAttempted_=true;boundary_=0x296308u;
        const auto resource=actual364.payload_.resource();
        if(traversal_.size()<resource.nodeCount+resource.rayCount)return false;
        std::size_t top=0,remaining=resource.nodeCount+resource.rayCount;
        traversal_[top++]=resource.root;
        while(top){
            if(!remaining--)return false;
            const auto tag=traversal_[--top];
            if(tag&type2Leaf49X){const auto i=static_cast<std::size_t>(tag&~type2Leaf49X);
                if(i>=resource.rayCount)return false;
                boundary_=0x295e00u;appendPoint295E00(resource.rays[i].second,actual19C,actual4435A0);
            }else{
                boundary_=0x295fd0u;
                if(tag>=resource.nodeCount||top+2u>traversal_.size())return false;
                traversal_[top++]=resource.nodes[tag].children[1];
                traversal_[top++]=resource.nodes[tag].children[0];
            }
        }
        pointsComplete_=true;boundary_=0x212814u;return true;
    }
    bool applyCaller212814()noexcept {
        if(!pointsComplete_||overridesApplied_||retired_||boundary_!=0x212814u||!sourceInputsCurrent())return false;
        auto& g=*generated_;auto& a=g.impact;
        mode34_=1; //212828
        a.correctPenetration64=true; //212834
        a.yawOnly6C=false; //21283C
        g.springForce4C=false; //212844
        g.impact54=true; //21284C
        g.friction50=false; //212854
        a.disableLeverArms5C=false; //21285C
        a.angularImpulseScaleB0=0; //212864
        a.forceTorqueScaleB4=0; //21286C
        a.positionCorrectionScaleCC=word(0x3ccccccdu); //212874
        a.angularCorrectionScaleD0=0; //21287C
        a.normalForceScale98=word(0x3f800000u); //212884
        overridesApplied_=true;boundary_=0x212888u;return true;
    }
    R append21288C(const OriginalWreckComponentDomain50LA& domain,C& actualRacer)noexcept {
        if(!overridesApplied_||retired_||appended_||boundary_!=0x212888u||!sourceInputsCurrent()||
           !racer_||&actualRacer!=&racer_->racer)return {S::owner,0x21288cu};
        auto r=appendOriginalWreckComponent50LA(domain,actualRacer,entity_,2u,app_->attachmentLock423F58());
        if(r.status==S::complete){appended_=true;boundary_=0x212894u;}return r;
    }
    C* liveEntity()noexcept{return entity_.reference00?&entity_:nullptr;}
    bool appended()const noexcept{return appended_;}
    bool retired()const noexcept{return retired_;}
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(retired_)return true;
        if(!constructed_||!app_||app_->attachmentLock423F58()||entity_.reference00!=1u||entity_.firstChild18||
           entity_.next14!=next||!source80_.has_value()||!source8C_.has_value()||
           *source80_!=pointRecords_.data()||*source8C_!=generatedRecords_.data()||!sourceInputsCurrent())return false;
        if(!appended_)return !parent&&!previous&&!next&&!entity_.parent1C&&!entity_.previous10;
        return racer_&&parent==&racer_->racer&&entity_.parent1C==parent&&entity_.previous10==previous&&
            (previous?previous->next14==&entity_:parent->firstChild18==&entity_)&&
            (!next||(next->parent1C==parent&&next->previous10==&entity_));
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(retired_)return {S::complete,0x295c18u};
        if(!preflightClose142590(parent,previous,nullptr))return {S::topology,0x142590u};
        if(entity_.previous10)entity_.previous10->next14=entity_.next14;
        if(entity_.parent1C&&entity_.parent1C->firstChild18==&entity_)entity_.parent1C->firstChild18=entity_.next14;
        --entity_.reference00; //104970 BEFORE295B90 and its two arrayfrees.
        if(!pointRecords_.release295B90()||!generatedRecords_.release295B90())std::terminate();
        entity_.kind=K::entity141300;--app_->objectCount423F60(); //295BE4->1413E0
        retired_=true;boundary_=0x295c18u;
        //source80/8C, body30, scene40, counts/settings and Racer368 remain
        //historical. No payload or numeric alias is zeroed to fake freshness.
        return {S::complete,boundary_};
    }
public:
    OriginalRacerContact368Owner50LA()=default;
    OriginalRacerContact368Owner50LA(const OriginalRacerContact368Owner50LA&)=delete;
    OriginalRacerContact368Owner50LA& operator=(const OriginalRacerContact368Owner50LA&)=delete;
    ~OriginalRacerContact368Owner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
};
}

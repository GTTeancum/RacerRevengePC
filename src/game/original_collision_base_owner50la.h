#pragma once
#include "original_collision_identity_list50la.h"
#include "original_application_entities50la.h"
#include "original_wreck_category_list50la.h"
#include "spring_records48n.h"
#include <array>
#include <optional>
#include <exception>

namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {
class OriginalWorldTriggerInstance50LA;
class OriginalRacerQuery364Owner50LA;
class OriginalRacerQuery690Owner50LA;
class OriginalRacerQuery694Owner50LA;
class OriginalRacerQuery7A4Owner50LA;
class OriginalRacerQuery79COwner50LA;
class OriginalRacerTerrainQueryOwner50LA;
// ONE counted11B190 Entity+8, shared by concrete collision-derived owners.
// It is not a second query facade: only the concrete owner holds this object,
// and only its entity_ enters the actual Racer component domain. No shape,
// scene publication, collection membership, or completed derived ctor implied.
class OriginalCollisionBaseOwner50LA final {
    friend class OriginalWorldTriggerInstance50LA;
    friend class OriginalSourcePhysicsCollision50LA;
    friend class OriginalRacerQuery364Owner50LA;
    friend class OriginalRacerQuery690Owner50LA;
    friend class OriginalRacerQuery694Owner50LA;
    friend class OriginalRacerQuery7A4Owner50LA;
    friend class OriginalRacerQuery79COwner50LA;
    friend class OriginalRacerTerrainQueryOwner50LA;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    C entity_;
    OriginalApplicationEntities50LA* application_=nullptr;
    enum class Phase {fresh,entity,complete,released};
    Phase phase_=Phase::fresh;
    bool appended_=false;
    std::uint16_t field00_=0;
    std::uint8_t type02_=0xff,flagsF8_=0;
    //Native byte backing is not source knowledge.11B190 only writes low3
    //bits by RMW; upper5 allocation bits remain explicitly unknown.
    struct SourceByte03 {std::uint8_t value=0,knownMask=0;} flags03_;
    SpringVector4_48N copied40_{},copied50_{},cachedCenter60_{},previous70_{};
    std::optional<float> radius80_; //11B190 never writes80.
    float distance84_=0;
    std::uint32_t shape88_=0,current94_=0,field98_=0,categoryA4_=0;
    // Null source aliases have engaged optionals; fresh allocation is unknown.
    std::optional<const void*> shape8C_,result90_,owner9C_,bodyA0_,membershipE0_;
    std::array<std::optional<const void*>,4> callbacksE4_{};
    std::uint8_t fieldA8_=0,fieldA9_=0;
    std::optional<std::uint64_t> wordB0_; //Native typed file identity is held by concrete loader after11BB84.
    std::uint32_t acceptB8_=0,modeBC_=0,modeC8_=0,modeD4_=0;
    const void* listC0_=nullptr; //Actual typed identity-array address; never truncated to32-bit tokens.
    const std::uint32_t* listCC_=nullptr;
    const std::uint32_t* listD8_=nullptr;
    std::uint32_t countC4_=0,countD0_=0,countDC_=0,fieldF4_=0;
    //Every field below is a genuine store reached inside11B190. Registry
    //failure retains the preceding counted Entity prefix and rejects replay.
    R construct11B190(OriginalApplicationEntities50LA& application,
        std::uint64_t generation,std::uint64_t key,const SpringVector4_48N& actual3E02C0)noexcept {
        if(phase_!=Phase::fresh)return {S::owner,0x11b190u};
        application_=&application;
        field00_=0;type02_=0xff;
        flags03_.value=static_cast<std::uint8_t>(flags03_.value&~1u);flags03_.knownMask|=1u; //11B1D8
        flags03_.value=static_cast<std::uint8_t>(flags03_.value&~2u);flags03_.knownMask|=2u; //11B1EC
        flags03_.value=static_cast<std::uint8_t>(flags03_.value&~4u);flags03_.knownMask|=4u; //11B200
        const auto result=initializeOriginalWreckComponent50LA(entity_,K::collision11B190,
            generation,key,true,application.objectCount423F60()); //11B208
        if(result.status!=S::complete)return result;
        phase_=Phase::entity;
        const SpringVector4_48N vf0{{0,0,0,1}};
        copied40_=vf0;copied50_=vf0;cachedCenter60_=vf0;previous70_=vf0;
        wordB0_=~std::uint64_t{0}; //11B26C
        const auto type=application.types383408().collisionObject11B270();
        if(!type.okay())return {S::owner,type.sourcePC};
        type02_=type.type;shape88_=0;shape8C_=nullptr;result90_=nullptr;
        flagsF8_=static_cast<std::uint8_t>((flagsF8_&0xc8u)|6u);
        for(auto& callback:callbacksE4_)callback=nullptr;
        acceptB8_=0xffffffffu;modeBC_=modeC8_=modeD4_=1u;
        listC0_=nullptr;listCC_=nullptr;countC4_=0;countD0_=0;listD8_=nullptr;countDC_=0;
        current94_=0;field98_=0;owner9C_=nullptr;bodyA0_=nullptr;categoryA4_=0;
        fieldA8_=0;fieldA9_=0;fieldF4_=0;
        copied40_=actual3E02C0;copied50_=actual3E02C0;distance84_=0;
        previous70_={{0,-10000.f,0,1}};membershipE0_=nullptr;
        phase_=Phase::complete;return {S::complete,0x11b498u};
    }
    bool setMode3_11B570(std::uint8_t& actualGlobal3E0730)noexcept {
        if(phase_!=Phase::complete)return false;
        flagsF8_=static_cast<std::uint8_t>(flagsF8_&~8u); //11B594, mode3 lacks bit8.
        flagsF8_=static_cast<std::uint8_t>(flagsF8_|2u); //11B5C0, mode3 has bit2.
        flagsF8_=static_cast<std::uint8_t>(flagsF8_&~4u); //11B5E4, mode3 lacks bit4.
        actualGlobal3E0730=0;return true;
    }
    bool setMode5_11B570(std::uint8_t& actualGlobal3E0730)noexcept {
        if(phase_!=Phase::complete)return false;
        flagsF8_=static_cast<std::uint8_t>(flagsF8_&~8u); //11B594
        flagsF8_=static_cast<std::uint8_t>(flagsF8_&~2u); //11B5C0
        flagsF8_=static_cast<std::uint8_t>(flagsF8_|4u); //11B5E4
        actualGlobal3E0730=0;return true; //11B5F4 tail identity, no retain.
    }
    bool freshShapeNull()const noexcept {
        return phase_==Phase::complete&&shape88_==0&&shape8C_.has_value()&&!*shape8C_&&
            result90_.has_value()&&!*result90_;
    }
    R append141960(const OriginalWreckComponentDomain50LA& domain,C& racer,std::uint32_t mask)noexcept {
        if(phase_!=Phase::complete||appended_||!application_)return {S::owner,0x141960u};
        auto r=appendOriginalWreckComponent50LA(domain,racer,entity_,mask,application_->attachmentLock423F58());
        if(r.status==S::complete)appended_=true;return r;
    }
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(phase_==Phase::released)return true;
        if(phase_!=Phase::complete||!application_||application_->attachmentLock423F58()||
           entity_.reference00!=1u||entity_.firstChild18||entity_.next14!=next)return false;
        if(!appended_)return !parent&&!previous&&!next&&!entity_.parent1C&&!entity_.previous10;
        return parent&&entity_.parent1C==parent&&entity_.previous10==previous&&
            (previous?previous->next14==&entity_:parent->firstChild18==&entity_)&&
            (!next||(next->parent1C==parent&&next->previous10==&entity_));
    }
    //Only genuine tail-first142590 destruction is joined, not arbitrary removal.
    //The concrete owner MUST preflight all payload/derived cleanup before this.
    bool dropTailReference142590(const C* parent,const C* previous)noexcept {
        if(!preflightClose142590(parent,previous,nullptr)||phase_==Phase::released)return false;
        if(entity_.previous10)entity_.previous10->next14=entity_.next14;
        if(entity_.parent1C&&entity_.parent1C->firstChild18==&entity_)
            entity_.parent1C->firstChild18=entity_.next14;
        --entity_.reference00;return true; //104970 before concrete destructor.
    }
    //Payload-specific owner must prove/dispose actual shape/result first.
    //No filter/membership mutator is exposed by this constructor-only class.
    bool constructorNullListsCurrent()const noexcept {
        return phase_==Phase::complete&&membershipE0_.has_value()&&!*membershipE0_&&
            !listC0_&&!listCC_&&!listD8_&&!countC4_&&!countD0_&&!countDC_;
    }
    R retireBase1413E0AfterPayload()noexcept {
        if(!constructorNullListsCurrent()||entity_.reference00||entity_.firstChild18||
           !shape8C_.has_value()||*shape8C_||!result90_.has_value()||*result90_||!application_)
            return {S::owner,0x1413e0u};
        entity_.kind=K::entity141300;--application_->objectCount423F60();
        phase_=Phase::released;return {S::complete,0x141438u};
    }
    //Dedicated348 post-free admission. Source11B51C deliberately left the
    //actualD8/DC/D4 fields stale; never zero them to satisfy ctor-null checks.
    R retireBase1413E0AfterMode0Payload(const OriginalWreckCategoryList50LA& category)noexcept {
        if(phase_!=Phase::complete||entity_.reference00||entity_.firstChild18||!application_||
           !membershipE0_.has_value()||*membershipE0_||listC0_||listCC_||countC4_||countD0_||
           !shape8C_.has_value()||*shape8C_||!result90_.has_value()||*result90_||
           !category.retiredMode0History(entity_,listD8_,countDC_,modeD4_))
            return {S::owner,0x1413e0u};
        entity_.kind=K::entity141300;--application_->objectCount423F60();
        phase_=Phase::released;return {S::complete,0x141438u};
    }
    //Same source post-D8-free phase for actual364 mode1 history.
    R retireBase1413E0AfterMode1Payload(const OriginalWreckCategoryList50LA& category)noexcept {
        if(phase_!=Phase::complete||entity_.reference00||entity_.firstChild18||!application_||
           !membershipE0_.has_value()||*membershipE0_||listC0_||listCC_||countC4_||countD0_||
           !shape8C_.has_value()||*shape8C_||!result90_.has_value()||*result90_||
           !category.retiredMode1History(entity_,listD8_,countDC_,modeD4_))
            return {S::owner,0x1413e0u};
        entity_.kind=K::entity141300;--application_->objectCount423F60();
        phase_=Phase::released;return {S::complete,0x141438u};
    }
    R retireBase1413E0AfterIdentityPayload(const OriginalCollisionIdentityList50LA& identities,
        const OriginalWreckCategoryList50LA* mode1Category=nullptr)noexcept {
        if(phase_!=Phase::complete||entity_.reference00||entity_.firstChild18||!application_||
           !membershipE0_.has_value()||*membershipE0_||listCC_||countD0_||
           !shape8C_.has_value()||*shape8C_||!result90_.has_value()||*result90_||
           !identities.retiredHistory(entity_,listC0_,countC4_))return {S::owner,0x1413e0u};
        if(mode1Category){
            if(!mode1Category->retiredMode1History(entity_,listD8_,countDC_,modeD4_))return {S::owner,0x1413e0u};
        }else if(listD8_||countDC_)return {S::owner,0x1413e0u};
        entity_.kind=K::entity141300;--application_->objectCount423F60();
        phase_=Phase::released;return {S::complete,0x141438u};
    }
public:
    OriginalCollisionBaseOwner50LA()=default;
    OriginalCollisionBaseOwner50LA(const OriginalCollisionBaseOwner50LA&)=delete;
    OriginalCollisionBaseOwner50LA& operator=(const OriginalCollisionBaseOwner50LA&)=delete;
    ~OriginalCollisionBaseOwner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
};
}

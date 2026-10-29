#pragma once
#include "original_racer_contact354_owner50la.h"
#include <array>
#include <cstring>
#include <optional>

namespace rr::game {
//Actual295290 at Racer358, bounded actual2953A0 a3=t0=t2=0 branch.
//It borrows the SAME354 and690; no private detector query or pair array exists.
class OriginalRacerDetector358Owner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;using R=OriginalWreckComponentResult50LA;
    C entity_;
    OriginalApplicationEntities50LA* app_=nullptr;
    OriginalRacerContact354Owner50LA* contact30_=nullptr;
    const OriginalWreckTerrainProvider50LA* terrain34_=nullptr;
    OriginalRacerQuery690Owner50LA* query38_=nullptr;
    SpringVector4_48N center40_{},half50_{};
    SpringVector4_48N* pairs60_=nullptr;
    std::uint32_t enabled64_=0,queryEnabled68_=0,reset6C_=0;
    const std::optional<std::uint32_t>* countAddress70_=nullptr;
    const std::optional<const void*>* listAddress74_=nullptr;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    const std::optional<const OriginalWreckTerrainProvider50LA*>* terrain330Address_=nullptr;
    const std::array<std::uint32_t,4>* setupOrigin434630_=nullptr; //Native exact input borrow, not source field.
    bool attempted_=false,constructed_=false,setup_=false,addressesBound_=false,queryBound_=false,appended_=false,retired_=false;
    std::uint32_t boundary_=0;
    R construct295290(OriginalApplicationEntities50LA& app,std::uint64_t generation,std::uint64_t key,
        const std::array<std::uint32_t,4>& actual443550,std::uint8_t nativeFlags29)noexcept {
        if(attempted_||entity_.reference00||!generation||!key)return {S::owner,0x21235cu};
        attempted_=true;app_=&app;boundary_=0x29529cu;entity_.flags29=nativeFlags29;
        auto r=initializeOriginalWreckComponent50LA(entity_,K::detector295290,generation,key,true,app.objectCount423F60());
        if(r.status!=S::complete)return r;
        center40_={{0,0,0,1}};half50_={{0,0,0,1}}; //2952B4/BC
        enabled64_=0;queryEnabled68_=1;reset6C_=1;
        contact30_=nullptr;terrain34_=nullptr;query38_=nullptr;pairs60_=nullptr;
        countAddress70_=nullptr;listAddress74_=nullptr;
        std::memcpy(&center40_,actual443550.data(),16); //2952F4, NOT434630/4435A0.
        std::memcpy(&half50_,actual443550.data(),16); //295300
        constructed_=true;boundary_=0x212364u;return {S::complete,boundary_};
    }
    bool setup2953A0(OriginalRacerContact354Owner50LA& contact,const OriginalRacerEntityCold50LA& actualRacer,
        const std::optional<const OriginalWreckTerrainProvider50LA*>& actual330,
        const std::array<std::uint32_t,4>& actual434630)noexcept {
        if(!constructed_||setup_||retired_||boundary_!=0x212364u||!actual330.has_value()||
           !contact.appended_||contact.retired_||contact.entity_.reference00!=1u||
           contact.entity_.sessionGeneration!=entity_.sessionGeneration||contact.racer_!=&actualRacer||
           !contact.sourceInputsCurrent()||contact.app_!=app_||!contact.generated_)return false;
        //2953B8/CC copies434630 to a LOCAL. This null-pair branch does not
        //write center40/half50 or allocate a query/pair buffer from that value.
        const auto local434630=actual434630;(void)local434630;
        setupOrigin434630_=&actual434630;terrain330Address_=&actual330;racer_=&actualRacer;
        contact30_=&contact; //2953D0
        terrain34_=*actual330; //2953D4: genuine NULL is allowed, unknown is not.
        if(!terrain34_)contact.generated_->pointContacts48=false; //2953E8 actual same Bank354 field.
        //a3=t0=0:2953F4 goes directly2955C8->141C20 identity return.
        setup_=true;boundary_=0x21238cu;return true;
    }
    bool bindAddresses21239C(const std::optional<std::uint32_t>& actual384,
        const std::optional<const void*>& actual388)noexcept {
        if(!setup_||addressesBound_||retired_||boundary_!=0x21238cu)return false;
        enabled64_=1; //21239C
        countAddress70_=&actual384; //2123A4, address only: no current-count read.
        listAddress74_=&actual388; //2123A8, preserves actual388 unknown contents.
        addressesBound_=true;boundary_=0x2123acu;return true;
    }
    bool inputsCurrent()const noexcept {
        return setup_&&contact30_&&racer_&&contact30_->racer_==racer_&&contact30_->app_==app_&&
            contact30_->entity_.reference00==1u&&!contact30_->retired_&&contact30_->sourceInputsCurrent()&&
            terrain330Address_&&terrain330Address_->has_value()&&**terrain330Address_==terrain34_;
    }
    bool bindQuery2955F0(OriginalRacerQuery690Owner50LA& actual690)noexcept {
        if(!addressesBound_||queryBound_||retired_||boundary_!=0x2123acu||!inputsCurrent()||
           contact30_->query690_!=&actual690||!contact30_->body30_||actual690.racer_!=racer_||
           !actual690.canBindContact2955F0(contact30_,contact30_->entity_,*contact30_->body30_))return false;
        query38_=&actual690; //2955F0 BEFORE writes to actual6909C/A0/E4/BC.
        actual690.bindContact2955F0(contact30_,contact30_->entity_,*contact30_->body30_);
        queryBound_=true;boundary_=0x2123b8u;return true;
    }
    R append2123C0(const OriginalWreckComponentDomain50LA& domain,C& actualRacer)noexcept {
        if(!queryBound_||appended_||retired_||boundary_!=0x2123b8u||!inputsCurrent()||&actualRacer!=&racer_->racer)
            return {S::owner,0x2123c0u};
        auto r=appendOriginalWreckComponent50LA(domain,actualRacer,entity_,2u,app_->attachmentLock423F58());
        if(r.status==S::complete){appended_=true;boundary_=0x2123c8u;}return r;
    }
    C* liveEntity()noexcept{return entity_.reference00?&entity_:nullptr;}
    bool appended()const noexcept{return appended_;}
    bool retired()const noexcept{return retired_;}
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(retired_)return true;
        if(!constructed_||!app_||app_->attachmentLock423F58()||entity_.reference00!=1u||entity_.firstChild18||
           entity_.next14!=next||pairs60_||(setup_&&!inputsCurrent())||
           (queryBound_&&(!query38_||query38_!=contact30_->query690_||
                !query38_->contactReceiverHistoryCurrent(false))))return false;
        if(!appended_)return !parent&&!previous&&!next&&!entity_.parent1C&&!entity_.previous10;
        return racer_&&parent==&racer_->racer&&entity_.parent1C==parent&&entity_.previous10==previous&&
            (previous?previous->next14==&entity_:parent->firstChild18==&entity_)&&
            (!next||(next->parent1C==parent&&next->previous10==&entity_));
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(retired_)return {S::complete,0x29539cu};
        if(!preflightClose142590(parent,previous,nullptr))return {S::topology,0x142590u};
        if(entity_.previous10)entity_.previous10->next14=entity_.next14;
        if(entity_.parent1C&&entity_.parent1C->firstChild18==&entity_)entity_.parent1C->firstChild18=entity_.next14;
        --entity_.reference00; //142590/104970 BEFORE295320.
        //Actual60 remains genuinelyNULL:29534C skips pair array free.
        entity_.kind=K::entity141300;--app_->objectCount423F60(); //295368->1413E0
        retired_=true;boundary_=0x29539cu;
        //No release of borrowed354/690/330; no query callback or source
        //30/34/38/70/74/Racer358 alias is cleared by this destructor.
        return {S::complete,boundary_};
    }
public:
    OriginalRacerDetector358Owner50LA()=default;
    OriginalRacerDetector358Owner50LA(const OriginalRacerDetector358Owner50LA&)=delete;
    OriginalRacerDetector358Owner50LA& operator=(const OriginalRacerDetector358Owner50LA&)=delete;
    ~OriginalRacerDetector358Owner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
};
}

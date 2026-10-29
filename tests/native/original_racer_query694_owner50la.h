#pragma once
#include "native_racer_collision50hw_fixture.h"
#include "../../src/game/original_collision_base_owner50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include "../../src/game/original_wreck_type0_owners50la.h"
#include <cstring>

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
//Actual counted11B190 at Racer694. This does not adopt or complete the
//separate native694 reservation, its collection membership, or coldBound.
class OriginalRacerQuery694Owner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    OriginalCollisionBaseOwner50LA base_;
    OriginalCollisionIdentityList50LA identitiesC0_;
    OriginalWreckType0Owners50LA payload_;
    const native_racer_collision50HW::Owner* collision_=nullptr;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    const OriginalRacerMainBodyOwner50LA* body340_=nullptr;
    const OriginalRacerSceneObject50LA* scene190_=nullptr;
    std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver_;
    rr::assets::OriginalStandardModelFileId50LA file_{};
    std::optional<rr::assets::OriginalStandardModelFileId50LA> sourceFileB0_;
    std::string member_;
    bool configured_=false,setupAttempted_=false,setupComplete_=false,ownersBound_=false;
    std::uint32_t boundary_=0;
    //Immutable same-owner admission, not execution of source132820.
    bool reserveRetainedFile(const native_racer_collision50HW::Owner& collision,
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        if(collision_||!collision.prepared()||!resolver||payload_.attempted()||base_.entity_.reference00)return false;
        const auto& raw=collision.storage_->raw[2];const auto& target=collision.mainTarget();
        if(raw.size()<84u||target.mesh.nodes.empty()||target.mesh.faces.empty()||target.mesh.vertices.empty()||
           target.radii.size()!=target.mesh.nodes.size()||target.mesh.reverse_mode!=0u||
           OriginalWreckType0Owners50LA::word(raw,68)!=target.mesh.vertices.size()||
           OriginalWreckType0Owners50LA::word(raw,72)!=target.mesh.nodes.size()||
           OriginalWreckType0Owners50LA::word(raw,76)!=target.mesh.faces.size()||
           (OriginalWreckType0Owners50LA::word(raw,80)&&!target.mesh.vertex_normals.empty()))return false;
        try {
            const auto name=collision.binding().memberPrefix+"podboxf.col";
            if(!resolver->bindModelMember(name,rr::assets::OriginalStandardModelFileKind50LA::colType0,
                raw,file_,diagnostic))return false;
            member_=name;
        }catch(...){return false;}
        collision_=&collision;resolver_=std::move(resolver);return true;
    }
    R construct2121F8(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        std::uint64_t key,const SpringVector4_48N& actualSentinel3E02C0,
        std::uint8_t nativeFlags31,std::uint8_t nativeFlagsF8)noexcept {
        if(!collision_||!resolver_||base_.phase_!=OriginalCollisionBaseOwner50LA::Phase::fresh||
           base_.entity_.reference00||payload_.attempted())return {S::owner,0x2121e4u};
        //Caller-admitted native allocation carriers, NOT recovered heap zeros.
        base_.entity_.flags29=nativeFlags31;base_.flagsF8_=nativeFlagsF8;
        boundary_=0x2121f8u;
        auto r=base_.construct11B190(app,generation,key,actualSentinel3E02C0);
        if(r.status==S::complete)boundary_=0x212200u;
        return r; //Caller publishes source694 only at212200.
    }
    bool configure212208(std::uint8_t& actualGlobal3E0730,std::uint32_t actualPhysicsBody3E0718,
        std::uint32_t actualCategory3B14B0)noexcept {
        if(boundary_!=0x212200u||configured_||!base_.freshShapeNull())return false;
        //11B570(5): independent actual RMW stores, preserving all other bits.
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_&~8u); //11B594
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_&~2u); //11B5C0
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_|4u); //11B5E4
        actualGlobal3E0730=0; //11B5F4; tail141C20 is identity return.
        base_.categoryA4_=actualPhysicsBody3E0718|actualCategory3B14B0; //212228
        base_.modeBC_=1u; //212230->123EA0
        configured_=true;boundary_=0x212238u;return true;
    }
    bool setup212240(const OriginalRacerEntityCold50LA& actualRacer,
        const OriginalRacerSceneObject50LA& actual190,const std::string& actualRequest840,
        const std::string& actualDirectory36B0F0,std::array<std::uint8_t,256>& actualName3E0730,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        if(!configured_||setupAttempted_||!base_.freshShapeNull()||!collision_||!resolver_||
           !actualRacer.prepared()||!actualRacer.racer.reference00||
           actualRacer.racer.sessionGeneration!=base_.entity_.sessionGeneration||
           actualRacer.scene190!=&actual190||!actualRacer.body340||
           actualRacer.body340->scene288()!=&actual190||!actualRacer.body340->entity().reference00||
           actualRacer.body340->entity().sessionGeneration!=base_.entity_.sessionGeneration||
           !actual190.constructed||actual190.released||!actual190.entity().reference00||
           actual190.entity().sessionGeneration!=base_.entity_.sessionGeneration)return false;
        //Exact2121DC full request. Only proved empty-directory/already-.col
        //branch admitted, never a synthetic stem or native cache path.
        if(!actualDirectory36B0F0.empty()||actualRequest840!=member_||actualRequest840.empty()||
           actualRequest840.size()>=actualName3E0730.size())return false;
        racer_=&actualRacer;body340_=actualRacer.body340;scene190_=&actual190;
        setupAttempted_=true;boundary_=0x11b9fcu;
        std::memcpy(actualName3E0730.data(),actualRequest840.data(),actualRequest840.size());
        actualName3E0730[actualRequest840.size()]=0;
        rr::assets::OriginalStandardModelFileView50LA file;boundary_=0x11ba2cu;
        if(!resolver_->lookupModel132820(actualRequest840,rr::assets::OriginalStandardModelFileKind50LA::colType0,
            file,diagnostic)||file.id.resolver!=file_.resolver||file.id.row!=file_.row||
           !file.bytes||*file.bytes!=collision_->storage_->raw[2]||file.animation1536F0||file.geometry240110)return false;
        if(file.collision125014){
            //11BA64->1254A0 cached branch does NOT execute source11BB84.
            base_.shape88_=0; //1254B8: actual resource000 tag is type0.
        }else{
            sourceFileB0_=file.id;base_.wordB0_.reset(); //11BB84 typed handle alternative.
            //124F9C invokes11B600 on the genuine NULL constructor payload.
            base_.shape8C_=nullptr;base_.result90_=nullptr; //11B900/904
            base_.shape88_=0; //124FB8 reads type0 from the retained COL.
        }
        const auto& target=collision_->mainTarget();
        const bool loaded=payload_.loadRacer11B920(file,collision_->storage_->raw[2],target.mesh,
            target.radii,actual190,resolver_,base_.shape8C_,diagnostic);
        if(!loaded){if(payload_.attempted())boundary_=payload_.sourceBoundary();return false;}
        //11B920 has no11BBE8 result allocation.90 remains genuinely NULL.
        setupComplete_=true;boundary_=0x212248u;return true;
    }
    bool sourceBindingCurrent()const noexcept {
        return racer_&&body340_&&scene190_&&racer_->racer.reference00&&
            racer_->racer.sessionGeneration==base_.entity_.sessionGeneration&&
            racer_->scene190==scene190_&&racer_->body340==body340_&&
            body340_->entity().reference00&&body340_->scene288()==scene190_&&
            body340_->entity().sessionGeneration==base_.entity_.sessionGeneration&&
            scene190_->constructed&&!scene190_->released&&scene190_->entity().reference00&&
            scene190_->entity().sessionGeneration==base_.entity_.sessionGeneration;
    }
    bool bindOwners21224C(const OriginalRacerEntityCold50LA& actualRacer,
        const OriginalRacerMainBodyOwner50LA& actual340)noexcept {
        if(boundary_!=0x212248u||!setupComplete_||ownersBound_||!sourceBindingCurrent()||
           &actualRacer!=racer_||&actual340!=body340_)return false;
        base_.owner9C_=&actualRacer; //21224C, actual source object not Entity+8.
        base_.bodyA0_=&actual340; //212258
        ownersBound_=true;boundary_=0x21225cu;return true;
    }
    R append212270(const OriginalWreckComponentDomain50LA& domain,C& actualRacer)noexcept {
        if(!ownersBound_||boundary_!=0x21225cu||!sourceBindingCurrent()||
           &actualRacer!=&racer_->racer||!base_.owner9C_||*base_.owner9C_!=racer_||
           !base_.bodyA0_||*base_.bodyA0_!=body340_)return {S::owner,0x212270u};
        auto r=base_.append141960(domain,actualRacer,0x402u);
        if(r.status==S::complete)boundary_=0x212278u;
        return r;
    }
    C* liveEntity()noexcept{return base_.entity_.reference00?&base_.entity_:nullptr;}
    bool appended()const noexcept{return base_.appended_;}
    bool retired()const noexcept{return base_.phase_==OriginalCollisionBaseOwner50LA::Phase::released;}
    bool appendIdentity123EB0(const OriginalCollisionBaseOwner50LA& other,const C& otherEntity)noexcept {
        if(!base_.appended_||!sourceBindingCurrent()||!setupComplete_||retired()||
           &other.entity()!=&otherEntity||otherEntity.parent1C!=base_.entity_.parent1C)return false;
        return identitiesC0_.append123EB0(base_.entity_,base_.listC0_,base_.countC4_,other,otherEntity);
    }
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(retired())return true;
        if(!base_.preflightClose142590(parent,previous,next)||!base_.membershipE0_.has_value()||*base_.membershipE0_||
           base_.listCC_||base_.countD0_||base_.listD8_||base_.countDC_||
           !identitiesC0_.matches(base_.entity_,base_.listC0_,base_.countC4_)||
           (racer_&&!sourceBindingCurrent())||(base_.appended_&&(!racer_||parent!=&racer_->racer))||
           !base_.result90_.has_value()||*base_.result90_)return false;
        if(!base_.owner9C_.has_value()||!base_.bodyA0_.has_value()||
           (ownersBound_?(*base_.owner9C_!=racer_||*base_.bodyA0_!=body340_):
                         (*base_.owner9C_||*base_.bodyA0_)))return false;
        if(!payload_.attempted())return base_.freshShapeNull()&&!payload_.shape8C_&&!payload_.loading_;
        //Incomplete allocation/publication remains retained, including a live
        //ref60 resource whose cache publication failed. No guessed unwind.
        const auto* resource=payload_.resource();
        return setupComplete_&&payload_.loaded()&&!payload_.retired()&&sourceBindingCurrent()&&
            payload_.shape8C_&&base_.shape88_==0u&&base_.shape8C_.has_value()&&
            *base_.shape8C_==payload_.shape8C_.get()&&payload_.shape8C_->racerModel110==scene190_&&
            !payload_.shape8C_->model110.value&&resource&&resource->live()&&
            resource->fileId().resolver==resolver_.get()&&resource->fileId().row==file_.row;
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(retired())return {S::complete,0x11b568u};
        if(!preflightClose142590(parent,previous,nullptr)||!base_.dropTailReference142590(parent,previous))
            return {S::topology,0x142590u};
        if(!identitiesC0_.releaseAfterReferenceDrop11B4F4(base_.entity_,base_.listC0_,base_.countC4_))std::terminate();
        if(payload_.loaded()&&!payload_.release11B600())std::terminate();
        base_.shape8C_=nullptr;base_.result90_=nullptr; //Actual11B900/904, after payload disposal.
        auto r=base_.retireBase1413E0AfterIdentityPayload(identitiesC0_);
        //Actual9C/A0, Racer694, model110 and fileB0 history is not cleared.
        if(r.status==S::complete)boundary_=0x11b568u;
        return r;
    }
public:
    OriginalRacerQuery694Owner50LA()=default;
    OriginalRacerQuery694Owner50LA(const OriginalRacerQuery694Owner50LA&)=delete;
    OriginalRacerQuery694Owner50LA& operator=(const OriginalRacerQuery694Owner50LA&)=delete;
    const C& entity()const noexcept{return base_.entity();}
};
}

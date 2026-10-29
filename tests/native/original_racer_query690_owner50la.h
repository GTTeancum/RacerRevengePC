#pragma once
#include "native_racer_collision50hw_fixture.h"
#include "../../src/game/original_collision_base_owner50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include "../../src/game/original_wreck_type2_owners50la.h"
#include "../../src/game/type2_bounds50by.h"
#include "../../src/assets/original_standard_texture_resource50la.h"
#include <cstring>

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
class OriginalRacerContact354Owner50LA;
class OriginalRacerDetector358Owner50LA;
//Private source component: actual11B190 object at Racer690, containing its
//ONE Entity+8. The fresh constructor caller reaches212148 in order; existing
//warm fixture query fields confer no authority to adopt/complete this owner.
class OriginalRacerQuery690Owner50LA final {
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend class OriginalRacerContact354Owner50LA;
    friend class ::native_racer_model50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    OriginalCollisionBaseOwner50LA base_;
    OriginalCollisionIdentityList50LA identitiesC0_;
    OriginalWreckType2Owners50LA payload_;
    const native_racer_collision50HW::Owner* collision_=nullptr;
    const OriginalRacerSceneObject50LA* scene190_=nullptr;
    const OriginalRacerEntityCold50LA* racer_=nullptr;
    const OriginalRacerMainBodyOwner50LA* body340_=nullptr;
    std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver_;
    rr::assets::OriginalStandardModelFileId50LA file_{};
    std::optional<rr::assets::OriginalStandardModelFileId50LA> sourceFileB0_; //Historical closed handle identity.
    std::string member_; //Retained full COL member; no allocation in reached setup.
    bool configured_=false,setupAttempted_=false,setupComplete_=false,boundsPublished_=false;
    std::uint32_t boundary_=0;
    friend class OriginalRacerDetector358Owner50LA;
    enum class SourceCallbackE4 { none, contact294BB0 };
    SourceCallbackE4 sourceCallbackE4_=SourceCallbackE4::none;
    const OriginalRacerContact354Owner50LA* contactReceiver9C_=nullptr;
    //Native lifetime metadata pin, NOT dereferencing historical source9C.
    //Model must retain the SAME unique354 owner through this query's close.
    const C* contactEntity9C_=nullptr;
    bool canBindContact2955F0(const OriginalRacerContact354Owner50LA* receiver,
        const C& contact,const OriginalRacerMainBodyOwner50LA& body)const noexcept {
        if(!receiver||contactReceiver9C_||contactEntity9C_||sourceCallbackE4_!=SourceCallbackE4::none||
           !setupComplete_||!payload_.loaded()||!base_.appended_||boundary_!=0x2121d0u||
           !sourceBindingCurrent()||&body!=body340_||contact.reference00!=1u||
           contact.kind!=OriginalWreckComponentKind50LA::contact295A40||
           contact.sessionGeneration!=base_.entity_.sessionGeneration||!contact.objectKey||
           !base_.owner9C_.has_value()||*base_.owner9C_||!base_.bodyA0_.has_value()||*base_.bodyA0_)return false;
        for(const auto& callback:base_.callbacksE4_)if(!callback.has_value()||*callback)return false;
        return true;
    }
    void bindContact2955F0(const OriginalRacerContact354Owner50LA* receiver,
        const C& contact,const OriginalRacerMainBodyOwner50LA& body)noexcept {
        //Caller completed ALL admission before detector38's actual2955F0 store.
        base_.owner9C_=receiver;contactReceiver9C_=receiver;contactEntity9C_=&contact; //295604
        base_.bodyA0_=&body; //295614, actual354.body30 (same retained mainbody).
        base_.callbacksE4_[0].reset();sourceCallbackE4_=SourceCallbackE4::contact294BB0; //29561C typed alternative.
        base_.modeBC_=1u; //295624 tail123EA0, no callback dispatch.
        //Keep boundary2121D0: it denotes this query's own earlier constructor
        //caller completion, not a claim that later detector registration did not occur.
    }
    bool contactReceiverHistoryCurrent(bool actualRetirement)const noexcept {
        for(std::size_t i=1;i<base_.callbacksE4_.size();++i)
            if(!base_.callbacksE4_[i].has_value()||*base_.callbacksE4_[i])return false;
        if(!contactReceiver9C_){
            return !contactEntity9C_&&sourceCallbackE4_==SourceCallbackE4::none&&
                base_.owner9C_.has_value()&&!*base_.owner9C_&&base_.bodyA0_.has_value()&&!*base_.bodyA0_&&
                base_.callbacksE4_[0].has_value()&&!*base_.callbacksE4_[0];
        }
        //Only compare the historical source9C pointer. Native Entity metadata
        //remains owned until query close; it is NOT a dispatch/receiver lease.
        if(!contactEntity9C_||!contactEntity9C_->objectKey||
           contactEntity9C_->sessionGeneration!=base_.entity_.sessionGeneration||
           !base_.owner9C_.has_value()||*base_.owner9C_!=contactReceiver9C_||
           !base_.bodyA0_.has_value()||*base_.bodyA0_!=body340_||base_.callbacksE4_[0].has_value()||
           sourceCallbackE4_!=SourceCallbackE4::contact294BB0||base_.modeBC_!=1u)return false;
        if(actualRetirement)return contactEntity9C_->reference00==0u&&
            contactEntity9C_->kind==OriginalWreckComponentKind50LA::entity141300;
        return (contactEntity9C_->reference00==1u&&
                contactEntity9C_->kind==OriginalWreckComponentKind50LA::contact295A40)||
               (contactEntity9C_->reference00==0u&&
                contactEntity9C_->kind==OriginalWreckComponentKind50LA::entity141300);
    }
    //Pre-source immutable file namespace admission, not a132820 execution.
    //The resolver compares SAME sealed original archive occurrence and bytes.
    bool reserveRetainedFile(const native_racer_collision50HW::Owner& collision,
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        if(collision_||!collision.prepared()||!resolver||payload_.attempted()||base_.entity_.reference00)return false;
        const auto& raw=collision.storage_->raw[0];
        //Existing source allocation owner supports the actual tree-bearing
        //COL branch. Single-ray/no-node input remains a bounded rejection.
        const auto& view=collision.main();
        if(!view.nodes||!view.rays||!view.nodeCount||view.root||view.rayCount!=view.nodeCount+1)return false;
        try {
            const auto name=collision.binding().memberPrefix+"podboxm.col";
            if(!resolver->bindModelMember(name,rr::assets::OriginalStandardModelFileKind50LA::colType2,
                raw,file_,diagnostic))return false;
            member_=name;
        }catch(...){return false;}
        collision_=&collision;resolver_=std::move(resolver);return true;
    }
    R construct21215C(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        std::uint64_t key,const SpringVector4_48N& actualSentinel3E02C0,
        std::uint8_t nativeFlags31,std::uint8_t nativeFlagsF8)noexcept {
        if(!collision_||!resolver_||base_.phase_!=OriginalCollisionBaseOwner50LA::Phase::fresh||
           base_.entity_.reference00||payload_.attempted())return {S::owner,0x212148u};
        //Explicit SAME native query/filter allocation carriers, not recovered
        //retail heap bytes.141300/11B190 overwrite only their actual source
        //bits and preserve the rest of these caller-admitted native values.
        //Canonical flags03 knownMask remains independent and unchanged here.
        base_.entity_.flags29=nativeFlags31;base_.flagsF8_=nativeFlagsF8;
        boundary_=0x21215cu;
        auto r=base_.construct11B190(app,generation,key,actualSentinel3E02C0);
        if(r.status==S::complete)boundary_=0x212164u;
        return r; //Caller now publishes actual source690=this, not future query.
    }
    bool configure21216C(std::uint8_t& actualGlobal3E0730,std::uint32_t actualPhysicsBody3E0718)noexcept {
        if(boundary_!=0x212164u||configured_||!base_.freshShapeNull())return false;
        if(!base_.setMode3_11B570(actualGlobal3E0730))return false;
        base_.categoryA4_=actualPhysicsBody3E0718; //212180
        base_.modeBC_=1u; //212188->123EA0, no list content fabricated.
        configured_=true;boundary_=0x212190u;return true;
    }
    bool setup21219C(const OriginalRacerEntityCold50LA& actualRacer,
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
        //Bounded empty-global-directory source branch11B9AC. A nonempty
        //directory is not silently ignored or normalized into a different file.
        //212140 passes the FULL "%spodboxm.col" request.11B9FC copies the
        //original argument, NOT normalized local stack40 or a stem. Admit the
        //already-.col, exact selected member branch; do not synthesize its input.
        if(!actualDirectory36B0F0.empty()||actualRequest840!=member_||actualRequest840.empty()||
           actualRequest840.size()>=actualName3E0730.size())return false;
        racer_=&actualRacer;body340_=actualRacer.body340;scene190_=&actual190;
        setupAttempted_=true;boundary_=0x11b9fcu;
        std::memcpy(actualName3E0730.data(),actualRequest840.data(),actualRequest840.size());
        actualName3E0730[actualRequest840.size()]=0; //Actual strcpy, suffix not zero-filled.
        rr::assets::OriginalStandardModelFileView50LA file;
        boundary_=0x11ba2cu;
        if(!resolver_->lookupModel132820(actualRequest840,rr::assets::OriginalStandardModelFileKind50LA::colType2,
            file,diagnostic)||file.id.resolver!=file_.resolver||file.id.row!=file_.row||
           !file.bytes||*file.bytes!=collision_->storage_->raw[0]||file.collision125014||
           file.animation1536F0||file.geometry240110)return false;
        //No source resource cache association is published for type2. The
        //same retained COL feeds source126640 allocations, not a new decode.
        sourceFileB0_=file.id;base_.wordB0_.reset(); //11BB84 typed actual handle alternative.
        //Retained bytes already passed original COL magic/version/type validation.
        base_.shape88_=2u; //124FB8 reads tag BEFORE1253DC allocation.
        if(!payload_.allocateRacerShape1253DC(actual190)){boundary_=payload_.sourceBoundary();return false;}
        base_.shape8C_=payload_.shape8C_.get(); //1253E4/F0
        if(!payload_.loadShape126640(collision_->main())||!payload_.allocateResult11BBE8(0u)){
            boundary_=payload_.sourceBoundary();return false;
        }
        base_.result90_=payload_.result90_.get(); //Actual11BD18 publication.
        setupComplete_=true;boundary_=0x2121a4u;return true;
    }
    R append2121B8(const OriginalWreckComponentDomain50LA& actualDomain,C& actualRacer)noexcept {
        if(!setupComplete_||boundary_!=0x2121a4u||!sourceBindingCurrent()||
           &actualRacer!=&racer_->racer)return {S::owner,0x2121b8u};
        auto r=base_.append141960(actualDomain,actualRacer,0x402u);
        if(r.status==S::complete)boundary_=0x2121c0u;
        return r;
    }
    //Actual690 source shape matrix comes from126640's retained COL.1202E0
    //does NOT read native Type2StaticQuery.worldMatrix or current body pose.
    //No11F570 occurs between11BBC0 and2121C8. Both outputs are real Racer
    //optionalXYZ lanes owned by caller; halfW is never written by source.
    bool publishBounds2121C8(std::optional<std::array<float,3>>& actual310,
        std::optional<std::array<float,3>>& actual320)noexcept {
        if(boundary_!=0x2121c0u||!base_.appended_||boundsPublished_||actual310||actual320||
           !payload_.loaded()||!sourceBindingCurrent()||
           payload_.shape8C_->racerModel44!=scene190_)return false;
        const auto resource=payload_.resource();Type2BoundsInput50BY in{};
        in.resource=&resource;in.preparedWorld=resource.localMatrix;
        CollectionBoundsResult48S output{};
        //Bounded finite helper admission BEFORE publishing outputs. Rejection
        //does not claim original1202E0 returned or advance source boundary.
        if(prepareType2Bounds50BY(in,output)!=CollectionBoundsStatus48S::Complete)return false;
        actual310=std::array<float,3>{{output.center.v[0],output.center.v[1],output.center.v[2]}}; //120CD4 XYZ
        actual320=std::array<float,3>{{output.half.v[0],output.half.v[1],output.half.v[2]}}; //120DBC/E04/E48
        boundsPublished_=true;boundary_=0x2121d0u;return true;
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
    C* liveEntity()noexcept {
        return base_.entity_.reference00?&base_.entity_:nullptr;
    }
    bool appended()const noexcept{return base_.appended_;} //Historical actual141960, not readiness.
    bool retired()const noexcept{return base_.phase_==OriginalCollisionBaseOwner50LA::Phase::released;}
    bool appendIdentity123EB0(const OriginalCollisionBaseOwner50LA& other,const C& otherEntity)noexcept {
        if(!base_.appended_||!sourceBindingCurrent()||!setupComplete_||retired()||
           &other.entity()!=&otherEntity||otherEntity.parent1C!=base_.entity_.parent1C)return false;
        return identitiesC0_.append123EB0(base_.entity_,base_.listC0_,base_.countC4_,other,otherEntity);
    }
    bool preflightClose142590(const C* parent,const C* previous,const C* next)const noexcept {
        if(retired())return true;
        if(!contactReceiverHistoryCurrent(false)||
           !base_.preflightClose142590(parent,previous,next)||!base_.membershipE0_.has_value()||*base_.membershipE0_||
           base_.listCC_||base_.countD0_||base_.listD8_||base_.countDC_||
           !identitiesC0_.matches(base_.entity_,base_.listC0_,base_.countC4_)||
           (racer_&&!sourceBindingCurrent())||(base_.appended_&&(!racer_||parent!=&racer_->racer)))return false;
        if(!payload_.attempted()){
            //Constructor-only or failed resolver prefix: source8C/90 are
            //genuinely known NULL, not inferred from unpublished native storage.
            return base_.freshShapeNull()&&!payload_.shape8C_&&!payload_.result90_;
        }
        //All reached incomplete shape/result allocations retain backing and
        //reference1. Whole-graph preflight MUST reject before any unlink/drop.
        return setupComplete_&&payload_.loaded()&&!payload_.retired()&&sourceBindingCurrent()&&
            payload_.shape8C_&&payload_.result90_&&base_.shape88_==2u&&
            base_.shape8C_.has_value()&&*base_.shape8C_==payload_.shape8C_.get()&&
            base_.result90_.has_value()&&*base_.result90_==payload_.result90_.get()&&
            payload_.shape8C_->racerModel44==scene190_&&!payload_.shape8C_->model44.value;
    }
    R close142590(const C* parent,const C* previous)noexcept {
        if(base_.phase_==OriginalCollisionBaseOwner50LA::Phase::released)return {S::complete,0x11b568u};
        if(!contactReceiverHistoryCurrent(true)||
           !preflightClose142590(parent,previous,nullptr)||!base_.dropTailReference142590(parent,previous))
            return {S::topology,0x142590u};
        if(!identitiesC0_.releaseAfterReferenceDrop11B4F4(base_.entity_,base_.listC0_,base_.countC4_))std::terminate();
        if(payload_.loaded()){
            if(!payload_.releaseShapeResult11B600())std::terminate();
            base_.shape8C_=nullptr; //Actual SW zero11B900, AFTER shape/result frees.
            base_.result90_=nullptr; //Actual SW zero11B904; unlike D8 these are cleared.
        }
        auto r=base_.retireBase1413E0AfterIdentityPayload(identitiesC0_);
        //Source190,690,scene44 and sibling aliases remain historical. No
        //clearing other owners, world detach, or half-word zeroing invented.
        if(r.status==S::complete)boundary_=0x11b568u;
        return r;
    }
public:
    OriginalRacerQuery690Owner50LA()=default;
    OriginalRacerQuery690Owner50LA(const OriginalRacerQuery690Owner50LA&)=delete;
    OriginalRacerQuery690Owner50LA& operator=(const OriginalRacerQuery690Owner50LA&)=delete;
    const C& entity()const noexcept{return base_.entity();}
};
}

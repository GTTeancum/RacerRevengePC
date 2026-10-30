#pragma once
#include <type_traits>
#include <cmath>
#include <cfenv>
#include "original_source_physics_object50la.h"
#include "original_wreck_physical50la.h"
#include "original_source_physics_model50la.h"
namespace rr::game {
class OriginalSourcePhysicsCollision50LA;
//Conditional165ECC nullable330/167920 constructor only. Root must first reach
//the real165E90 callsite with a LIVE nested Model; this class issues no setup.
class OriginalSourcePhysicsBody50LA final {
    friend class OriginalSourcePhysicsGravity50LA;
    friend class OriginalSourcePhysicsCollision50LA;
    friend class OriginalSourcePhysicsContact50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_static_model_cold50LA::Owner;
    using Base=OriginalSourcePhysicsObject50LA;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    enum class Status : std::uint8_t {reached,bounded,owner};
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    struct SourceFields {
        std::optional<SpringVector4_48N> vector40,vector50,vectorF0;
        std::optional<std::uint32_t> word30,word284,method290,count264,flags200;
        std::optional<const void*> allocation28C,model288;
    };
    struct Storage {C entity;OriginalWreckPhysical50LA physical;SourceFields source;
        std::uint8_t flags29Known=0;std::uint32_t dispatch08=0;};
    const void* contactBodyBorrower_=nullptr;
    const void* directCollisionModelBorrower_=nullptr; //E4 Model argument lease; not a source A0 store.
    const void* collisionBodyBorrower_=nullptr; //Exact collisionA0 raw Body borrow.
    const void* gravityTargetBorrower_=nullptr; //Exact published gravity array slot.
    Base* base_=nullptr;
    OriginalSourcePhysicsModel50LA* modelBorrow_=nullptr;
    bool configEntered_=false,noOpReturned_=false,modelStored_=false;
    bool mode320Stored_=false,disableReturned_=false;
    std::unique_ptr<Storage> body_;
    OriginalWreckConstructorGlobalViews50LA globals_;
    bool attempted_=false,counted_=false,constructorReturned_=false,published_=false;
    bool nativeRetired_=false,nativeWithdrawn_=false;
    C* publishedIdentity_=nullptr;
    bool attachEntered_=false,attachReturned_=false,attached_=false,detachedNative_=false;
    std::optional<std::uint32_t> attachLock_;
    std::uint32_t attachPC_=0;
    std::uint32_t pc_=0;
    Result at(Status s,std::uint32_t pc)noexcept {pc_=pc;return {s,pc};}
    static bool bits(float value,std::uint32_t expected)noexcept {
        std::uint32_t raw;std::memcpy(&raw,&value,4);return raw==expected;
    }
    static bool vector(const Vector3_48C& xyz,float w,const void* raw)noexcept {
        return !std::memcmp(xyz.v,raw,12)&&!std::memcmp(&w,static_cast<const char*>(raw)+12,4);
    }
    struct SourceDimensionsMass1664A0 {
        bool entered=false,halfDoubledStored=false,lengthStored=false,inverseExtentStored=false;
        bool requestedMassReturned=false,massCallEntered=false,massStored=false,inverseMassStored=false;
        bool massCallReturned=false,returned=false;
        std::uint32_t pc=0,rawWeight=0,beforeMass=0,beforeInverseMass=0;
        std::array<std::uint32_t,4> halfBefore{},temporaryE0{},halfAfter{},halfReload{};
        std::uint8_t temporaryKnown=0;
        std::array<std::uint32_t,12> arithmetic{};
        std::uint16_t arithmeticKnown=0;
        std::optional<bool> lengthNonpositive,massNonpositive;
        std::optional<std::uint32_t> massReload;
        const void* actualHalfIdentity=nullptr;
    } dimensionsMass_;
    Result configureSourceDimensionsMass1664A0(OriginalSourcePhysicsModel50LA&,
        std::array<std::uint32_t,4>& actualHalf50,std::uint32_t rawWeight284)noexcept;
    bool dimensionsMassHistoryNative()const noexcept;
    struct SourceCenterOffset27B598 {
        bool entered=false,returned=false;
        const void* configurationOccurrence=nullptr;
        std::array<std::uint32_t,4> raw{};
        std::uint32_t pc=0;
    } sourceCenterOffset_;
    template<class ActualContact>
    Result copySourceCenterOffset27B598(const ActualContact& actualContact,const void* configurationOccurrence,
        const std::array<std::uint32_t,4>& rawDescriptor2B0)noexcept {
        if(sourceCenterOffset_.entered||nativeWithdrawn_||nativeRetired_||detachedNative_||!body_||
           !base_||!inertia_.returned||!preflightNative()||!modelBorrowCurrentNative()||
           !base_->sourceConfigurationTailReturnCurrentNative(actualContact,configurationOccurrence))
            return {Status::owner,0x27b598u};
        auto& h=sourceCenterOffset_;h.entered=true;h.configurationOccurrence=configurationOccurrence;h.raw=rawDescriptor2B0;
        //One actual SQ of all sixteen raw descriptor bytes, not a numeric XYZ setter.
        std::memcpy(body_->physical.parameters.centerOffset.v,h.raw.data(),12);
        std::memcpy(&body_->physical.parameters.centerOffsetW,h.raw.data()+3,4);
        h.returned=true;h.pc=0x27b59cu;return {Status::reached,h.pc};
    }
    bool sourceCenterOffsetHistoryNative()const noexcept {
        const auto& h=sourceCenterOffset_;
        if(!h.entered)return !h.returned&&!h.configurationOccurrence&&!h.pc;
        return h.configurationOccurrence&&h.returned&&h.pc==0x27b59cu&&base_&&
            base_->sourceConfigurationTail_.returned&&base_->sourceConfigurationTail_.occurrence==h.configurationOccurrence;
    }
    struct SourceInertia166558 {
        bool entered=false,zeroReturned=false,setterEntered=false,inverseEntered=false,inverseReturned=false,returned=false;
        std::uint32_t pc=0,rawInertia=0;
        const void* actualWorkingIdentity=nullptr;
        std::array<std::uint32_t,16> workingBefore{},zeroF0{},workingAfter{},body100{},inverseInput{},inverseLocal{},body140{};
        std::array<std::uint32_t,3> halfReload{},massReload{};
        std::uint8_t massReadMask=0,zeroCopyMask=0,diagonalMask=0;
        std::uint16_t zeroKnown=0,body100Mask=0,inverseLocalKnown=0,inverseOutputMask=0,body140Mask=0;
        std::array<std::uint32_t,18> arithmetic{};
        std::uint32_t arithmeticKnown=0;
        std::array<std::uint32_t,32> inverseRegisters{};
        std::uint32_t inverseRegisterKnown=0,inverseOperationCount=0;
        std::optional<std::uint32_t> inverseAccumulator;
        std::optional<bool> determinantZero;
    } inertia_;
    Result configureSourceInertia166558(const std::array<std::uint32_t,4>& actualHalf50,
        std::array<std::uint32_t,16>& actualWorking60,std::uint32_t rawInertia288)noexcept;
    bool inertiaHistoryNative()const noexcept;
    bool sourceInertiaReturned166618Native(const void* actualContact)const noexcept {
        return actualContact&&contactBodyBorrower_==actualContact&&inertia_.returned&&inertia_.pc==0x166618u&&
            !nativeWithdrawn_&&!nativeRetired_&&!detachedNative_&&attachReturned_&&body_&&
            preflightNative()&&modelBorrowCurrentNative();
    }
    float& inverseWordNative(unsigned index)noexcept {
        auto& q=body_->physical.parameters;
        if(index<12&&index%4<3)return q.body.inverseInertiaColumns[index/4].v[index%4];
        return q.inverseInertiaExtra[index<12?index/4:index-9];
    }
    bool constructorFieldsCurrentNative()const noexcept {
        const auto& p=body_->physical;const auto& b=p.body;const auto& r=b.root;
        const auto& q=p.parameters;const auto& s=body_->source;
        const auto origin=globals_.bodyVector433AD0,reset=globals_.reset.vector433B80;
        if(!sourceCenterOffsetHistoryNative()||!inertiaHistoryNative()||!dimensionsMassHistoryNative()||!p.bodyConstructorFieldsApplied||!s.vector40||!s.vector50||!s.vectorF0||
           std::memcmp(s.vector40->v,origin,16)||std::memcmp(s.vector50->v,origin,16)||
           std::memcmp(s.vectorF0->v,origin,16)||s.word30!=0||s.word284!=0||
           s.method290!=0x14ee40u||s.count264!=0||s.flags200!=0||
           !s.allocation28C||*s.allocation28C||!s.model288||
           (modelStored_?*s.model288!=static_cast<const void*>(&modelBorrow_->model_->model_):*s.model288!=nullptr))return false;
        if(b.enabled==disableReturned_||p.awake208||p.linkedCount268||
           p.body320!=(mode320Stored_?1u:0u)||b.body20C||r.callbackState238||
           b.scenePresent||p.model288!=SceneNodeIndex49P::none||p.modelGraphNodes||
           p.modelGraphCount||p.modelSceneDomain||!bits(q.body.mass,dimensionsMass_.massStored?dimensionsMass_.arithmetic[10]:0u)||
           !bits(q.body.inverseMass,dimensionsMass_.inverseMassStored?dimensionsMass_.arithmetic[11]:0u)||
           !bits(b.cachedSpeed34,0)||!bits(q.body.linearAccelerationLimitSquared,0x4963d240)||
           !bits(q.body.angularAccelerationLimitSquared,0x48742400)||
           !bits(q.body.linearVelocityLimitSquared,0x468e6200)||
           !bits(q.body.angularVelocityLimitSquared,0x441c4000))return false;
        if(!vector(r.body.position,p.extra.position60W,origin)||
           !vector(r.worldAngularVelocityA0,p.extra.worldAngularVelocityA0W,origin)||
           !vector(b.cachedAnglesE0,p.cachedAnglesE0W,origin)||
           !vector(r.positionCorrection1a0,p.extra.positionCorrection1A0W,origin)||
           !vector(r.rotationCorrection1b0,p.extra.rotationCorrection1B0W,origin)||
           !vector(r.accumulator1c0,p.extra.accumulator1C0W,origin)||
           !vector(r.accumulator1d0,p.extra.accumulator1D0W,origin)||
           !vector(q.centerOffset,q.centerOffsetW,sourceCenterOffset_.returned?static_cast<const void*>(sourceCenterOffset_.raw.data()):static_cast<const void*>(origin))||
           !vector(r.body.velocity,b.velocity70W,reset)||
           !vector(r.localVelocity80,p.supplement.localVelocity80W,reset)||
           !vector(r.previousVelocity90,p.supplement.previousVelocity90W,reset)||
           !vector(r.body.angularVelocity,b.angularVelocityB0W,reset)||
           !vector(r.body.linearAcceleration,p.supplement.linearAccelerationC0W,reset)||
           !vector(r.body.angularAcceleration,p.supplement.angularAccelerationD0W,reset)||
           !vector(r.loads.force,b.force180W,reset)||!vector(r.loads.torque,b.torque190W,reset))return false;
        for(unsigned i=0;i<16;++i)if(!bits(p.inertia100.v[i],(inertia_.body100Mask&(1u<<i))?inertia_.body100[i]:0u))return false;
        for(unsigned i=0;i<3;++i){
            if(!q.body.translationEnabled[i]||!q.body.rotationEnabled[i])return false;
            for(unsigned j=0;j<3;++j){const auto lane=4*i+j;
                if(!bits(q.body.inverseInertiaColumns[i].v[j],(inertia_.body140Mask&(1u<<lane))?inertia_.body140[lane]:0u))return false;}
        }
        constexpr unsigned extraLanes[7]={3,7,11,12,13,14,15};
        for(unsigned i=0;i<7;++i){const auto lane=extraLanes[i];
            if(!bits(q.inverseInertiaExtra[i],(inertia_.body140Mask&(1u<<lane))?inertia_.body140[lane]:0u))return false;}
        const Vector3_48C* history[]={&p.supplement.positionDerivative2B8,
            &p.supplement.linearAcceleration2D0,&p.supplement.angularAcceleration308,
            &r.body.previousPositionDerivative,&r.body.previousLinearAcceleration,
            &r.body.previousAngularAcceleration};
        for(const auto* v:history)for(float value:v->v)if(!bits(value,0))return false;
        return !std::memcmp(r.body.orientation.v,globals_.reset.quaternion382B60,16)&&
            !std::memcmp(r.body.orientationDerivative.v,globals_.reset.quaternion382B60,16)&&
            !std::memcmp(r.body.previousOrientationDerivative.v,globals_.reset.quaternion382B60,16);
    }
    //Structural borrowed identity only. Source admission below uses the full
    //live nested proof; later payload withdrawal cannot invalidate raw288
    //cleanup while exact Model/Entity lifetime tokens remain held.
    bool modelBorrowCurrentNative()const noexcept {
        if(!modelBorrow_)return !configEntered_&&!noOpReturned_&&!modelStored_&&!mode320Stored_&&!disableReturned_;
        const auto& n=*modelBorrow_;
        return configEntered_&&n.base_==base_&&n.bodyModelBorrower_==this&&n.model_&&
            n.published_&&n.constructorReturned_&&!n.ledgerWithdrawn_&&
            !n.modelRetired_&&!n.nativeWithdrawn_&&!n.sourceReferenceDropped_&&!n.sourceSceneReleased_&&
            !n.sourceOuterDeleted_&&n.countedModelEntityNative()&&
            n.model_->sourcePhysicsBodyBorrower_==this&&!n.model_->released_&&
            !n.model_->referenceReleased_&&n.model_->model_.constructed&&!n.model_->model_.released&&
            n.model_->model_.entity8.reference00==1&&
            n.model_->model_.entity8.kind==K::model2403A0&&
            n.model_->model_.entity8.sessionGeneration==base_->generation_&&
            n.model_->sourceConstructor_.entityCounted&&n.model_->sourceConstructor_.modelReturned&&
            n.model_->binding_.sessionGeneration==base_->generation_&&
            n.model_->binding_.actualObjectCount423F60==&base_->application_->objectCount423F60()&&
            n.model_->binding_.actualAttachmentLock423F58==&base_->application_->attachmentLock423F58()&&
            n.model_->binding_.actualTypes383408==&base_->application_->types383408()&&
            base_->object_->model14C==static_cast<const void*>(&n.model_->model_);
    }
    const OriginalRacerSceneObject50LA* sourceModel288Native()const noexcept {
        if(!base_||!base_->object_||!modelBorrowCurrentNative()||!modelStored_)return nullptr;
        return &modelBorrow_->model_->model_; //Exact raw source288, no graph/ready promotion.
    }
    Result copySourceModelWorld28B550(std::array<std::uint32_t,16>& output,
        std::uint16_t& known)noexcept {
        if(!preflightNative()||!modelBorrowCurrentNative()||!modelStored_||
           !disableReturned_||!modelBorrow_||!modelBorrow_->preflightNative())
            return {Status::owner,0x28b550u};
        const auto result=modelBorrow_->copySourceWorld28B550(output,known);
        // This getter does not rewrite the Body's earlier constructor/configPC.
        return {result.status==OriginalSourcePhysicsModel50LA::Status::reached?
            Status::reached:Status::bounded,result.sourcePC};
    }
    template<class ActualDirectCollision>
    Result copyDirectCollisionModelWorld28B550(const ActualDirectCollision& actualOccurrence,
        std::array<std::uint32_t,16>& output,std::uint16_t& known)noexcept {
        static_assert(std::is_same_v<ActualDirectCollision,OriginalSourcePhysicsCollision50LA>,
            "Only the actual typed source E4 occurrence may request this getter");
        //Dependent private admission is supplied only by the later genuine
        //positive E4 transform stage. A cleanup-valid Body lease is insufficient.
        if(directCollisionModelBorrower_!=&actualOccurrence||
           !actualOccurrence.sourceDirectWorldCallAdmissionNative(this)||!preflightNative()||!modelBorrowCurrentNative()||!modelStored_||!disableReturned_||
           !modelBorrow_||!modelBorrow_->preflightNative()||
           sourceModel288Native()!=&modelBorrow_->model_->model_)return {Status::owner,0x28b550u};
        const auto result=modelBorrow_->copyDirectCollisionWorld28B550(this,&actualOccurrence,output,known);
        return {result.status==OriginalSourcePhysicsModel50LA::Status::reached?
            Status::reached:Status::bounded,result.sourcePC};
    }
    Result configureSource165EF8(OriginalSourcePhysicsModel50LA& actualNested)noexcept {
        if(configEntered_||!preflightNative()||pc_!=0x165eec||!published_||
           actualNested.base_!=base_||!actualNested.preflightNative()||!actualNested.model_||
           !actualNested.published_||!actualNested.constructorReturned_||actualNested.ledgerWithdrawn_||
           actualNested.modelRetired_||actualNested.nativeWithdrawn_||actualNested.sourceReferenceDropped_||
           actualNested.sourceSceneReleased_||actualNested.sourceOuterDeleted_||
           !actualNested.countedModelEntityNative()||actualNested.bodyModelBorrower_||
           actualNested.model_->sourcePhysicsBodyBorrower_||actualNested.model_->released_||
           actualNested.model_->referenceReleased_||!actualNested.model_->model_.constructed||
           actualNested.model_->model_.released||actualNested.model_->model_.entity8.reference00!=1||
           base_->object_->model14C!=static_cast<const void*>(&actualNested.model_->model_))
            return {Status::owner,0x165ef8};
        //Native custody precedes even the NULLD0 dereference. It is not a
        //source retain, loader return, placement or caller-stage admission.
        configEntered_=true;modelBorrow_=&actualNested;
        actualNested.bodyModelBorrower_=this;actualNested.model_->sourcePhysicsBodyBorrower_=this;
        if(!body_)return at(Status::bounded,0x165ef0);
        noOpReturned_=true; //Actual slot30→141C20 has no source stores.
        //1652E0 skips a NULL argument. This qualified live-model branch has
        //the actual nonNULL BASE14C and therefore performs exactly1652E8.
        body_->source.model288=static_cast<const void*>(&actualNested.model_->model_);modelStored_=true;
        //No graph/cache adoption or retain accompanies this raw source borrow.
        body_->physical.body320=1;mode320Stored_=true; //165F1C.
        body_->physical.body.enabled=false; //1653C4 branch-delay store204=0.
        disableReturned_=true; //1653C0(a1=0) jumps165418, no208/linked writes.
        return at(Status::reached,0x165f34); //Before actual141960 attachment.
    }
    Result allocateSource165ECC(Base& actualBase)noexcept {
        if(attempted_||nativeWithdrawn_||!actualBase.preflightNative()||!actualBase.object_||
           actualBase.bodyBorrower_||actualBase.object_->bodyD0||
           actualBase.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return {Status::owner,0x165ecc};
        base_=&actualBase;attempted_=true;base_->bodyBorrower_=this;
        auto& app=*base_->application_;
        globals_.bodyVector433AD0=app.bodyOrigin433AD0_.data();
        globals_.pieceVector433B10=app.wreckAssetVectors_.zero433B10.data();
        globals_.reset.vector433B80=app.bodyReset433B80_.data();
        globals_.reset.quaternion382B60=app.bodyQuaternion382B60_.data();
        try{body_=std::make_unique<Storage>();}catch(...){body_.reset();}
        if(body_){
            const auto result=initializeOriginalWreckComponent50LA(body_->entity,K::entity141300,
                base_->generation_,Base::key(&body_->entity),true,app.objectCount423F60());
            if(result.status!=OriginalWreckComponentStatus50LA::complete)
                return at(Status::bounded,0x141300);
            counted_=true;body_->flags29Known=0x0f;
            body_->dispatch08=0x3cded0u; //164ED0 after141300.
            initializeSourceBody16792050LA(body_->physical,body_->source,body_->entity,globals_);
            //Final dispatch witness; entity.kind changed at exact167948 in the leaf.
            body_->dispatch08=0x3ce000u;constructorReturned_=true;
        }
        publishedIdentity_=body_?&body_->entity:nullptr;
        base_->object_->bodyD0=publishedIdentity_;published_=true; //165EE8 evenNULL.
        return at(Status::reached,0x165eec); //No virtual30/Model288 binding or attachment.
    }
    //Conditional original 165F3C; caller supplies the same-App authority.
    template<class ActualAttach>
    Result attachSourceBody141960(ActualAttach&& attach)noexcept {
        if(attachEntered_||nativeWithdrawn_||!(disableReturned_&&modelStored_)||!preflightNative()||
           !body_||!counted_||!published_||!base_||!base_->object_||base_->bodyChild141960_)
            return {Status::owner,0x165f3c};
        auto& child=body_->entity;auto& parent=base_->object_->entity;
        if(child.parent1C||child.previous10||child.next14||child.firstChild18||child.mask20||child.descendants24)
            return {Status::owner,0x165f3c};
        attachEntered_=true;attachLock_=base_->application_->attachmentLock423F58();attachPC_=0x141970;
        if(!*attachLock_){
            const auto result=attach(parent,child,0xffffffffu,*attachLock_);
            if(result.status!=OriginalWreckComponentStatus50LA::complete){
                attachPC_=result.sourcePC;return {Status::bounded,attachPC_};
            }
            base_->bodyChild141960_=&child;attached_=true;
        }
        //A real nonzero lock returns NULL before touching the child. Caller
        //ignores that result; it is not a native callback refusal.
        attachReturned_=true;attachPC_=0x165f44;return {Status::reached,attachPC_};
    }
    bool attachmentHistoryNative()const noexcept {
        if(!attachEntered_)return !attachReturned_&&!attached_&&!detachedNative_&&!attachLock_&&!attachPC_&&
            (!base_||!base_->bodyChild141960_);
        if(!base_||!base_->object_||!body_||!(disableReturned_&&modelStored_)||!attachLock_||
           (detachedNative_&&!attached_)||(*attachLock_&&attached_))return false;
        if(attachReturned_&&((*attachLock_?attached_:!attached_)||attachPC_!=0x165f44))return false;
        const auto& child=body_->entity;const bool live=attached_&&!detachedNative_;
        if(base_->bodyChild141960_!=(live?&child:nullptr)||!base_->sourceChildChainCurrentNative()||
           child.parent1C!=(live?&base_->object_->entity:nullptr)||
           (!live&&(child.previous10||child.next14))||child.firstChild18||child.descendants24||
           child.mask20!=(attached_?0xffffffffu:0u))return false;
        return true;
    }
    template<class ActualDetach>
    bool detachSourceBodyNative(ActualDetach&& detach)noexcept {
        if(!attempted_||nativeWithdrawn_||!attached_||detachedNative_)return true;
        if(gravityTargetBorrower_||collisionBodyBorrower_||directCollisionModelBorrower_||contactBodyBorrower_||!base_||!base_->application_||base_->application_->attachmentLock423F58()||!preflightNative())return false;
        const auto result=detach(body_->entity,0u);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        base_->bodyChild141960_=nullptr;detachedNative_=true;return true;
    }
    C* countedEntityNative()const noexcept {
        return body_&&counted_&&!nativeRetired_?&body_->entity:nullptr;
    }
    bool preflightNative()const noexcept {
        if(!attempted_)return !base_&&!body_&&!counted_&&!published_&&!nativeWithdrawn_;
        if(nativeWithdrawn_)return !base_&&!body_&&!modelBorrow_&&!gravityTargetBorrower_&&!collisionBodyBorrower_&&!directCollisionModelBorrower_&&!contactBodyBorrower_&&(!counted_||nativeRetired_);
        if(!attachmentHistoryNative())return false;
        if(!base_||base_->bodyBorrower_!=this||!base_->preflightNative()||!base_->object_)return false;
        auto& app=*base_->application_;
        if(globals_.bodyVector433AD0!=app.bodyOrigin433AD0_.data()||
           globals_.pieceVector433B10!=app.wreckAssetVectors_.zero433B10.data()||
           globals_.reset.vector433B80!=app.bodyReset433B80_.data()||
           globals_.reset.quaternion382B60!=app.bodyQuaternion382B60_.data())return false;
        if(published_&&(!base_->object_->bodyD0||*base_->object_->bodyD0!=publishedIdentity_))return false;
        if(!published_&&base_->object_->bodyD0)return false;
        if(!body_)return !counted_&&!constructorReturned_&&!nativeRetired_&&published_&&!publishedIdentity_&&
            modelBorrowCurrentNative()&&(!configEntered_||(!noOpReturned_&&!modelStored_&&
                !mode320Stored_&&!disableReturned_&&pc_==0x165ef0));
        if(!modelBorrowCurrentNative())return false;
        if(configEntered_&&(noOpReturned_!=modelStored_||modelStored_!=mode320Stored_||
           mode320Stored_!=disableReturned_||!disableReturned_||pc_!=0x165f34))return false;
        const auto& e=body_->entity;
        if(!counted_)return !constructorReturned_&&!published_&&!nativeRetired_&&
            !e.reference00&&e.kind==K::unbound&&!body_->dispatch08&&!body_->flags29Known;
        return !nativeRetired_&&constructorReturned_&&published_&&publishedIdentity_==&e&&
            e.reference00==1&&e.kind==K::body167920&&e.objectKey==Base::key(&e)&&
            e.sessionGeneration==base_->generation_&&
            (attachEntered_||(!e.previous10&&!e.next14&&!e.parent1C&&!e.mask20))&&!e.firstChild18&&!e.descendants24&&!e.nativeFlags29&&e.category28==0xff&&
            (e.flags29&0x0f)==3&&body_->flags29Known==0x0f&&body_->dispatch08==0x3ce000&&
            app.objectCount423F60()>=2&&constructorFieldsCurrentNative();
    }
    template<class Retire>
    bool withdrawNative(Retire&& retire)noexcept {
        if(!attempted_||nativeWithdrawn_)return true;
        if(gravityTargetBorrower_||collisionBodyBorrower_||directCollisionModelBorrower_||contactBodyBorrower_||(attached_&&!detachedNative_)||!preflightNative())return false;
        if(counted_&&!nativeRetired_){
            if(!retire(body_->entity))return false;
            body_->entity.reference00=0;--base_->application_->objectCount423F60();nativeRetired_=true;
        }
        //No further refusal after exact ledger callback. Unpin before freeing
        //the body; model288 remains historical, never a source release/NULL.
        if(modelBorrow_){modelBorrow_->model_->sourcePhysicsBodyBorrower_=nullptr;
            modelBorrow_->bodyModelBorrower_=nullptr;modelBorrow_=nullptr;}
        if(published_){base_->object_->bodyD0=nullptr;base_->object_->bodyD0NativeWithdrawn=true;}
        if(base_->sourceInverseExtent118_.occurrence==this)base_->sourceInverseExtent118_.nativeWithdrawn=true;
        body_.reset();base_->bodyBorrower_=nullptr;base_=nullptr;globals_={};nativeWithdrawn_=true;
        return true; //Native cancellation, never1679D0/source destructor or setup return.
    }
public:
    OriginalSourcePhysicsBody50LA()=default;
    OriginalSourcePhysicsBody50LA(const OriginalSourcePhysicsBody50LA&)=delete;
    OriginalSourcePhysicsBody50LA& operator=(const OriginalSourcePhysicsBody50LA&)=delete;
    ~OriginalSourcePhysicsBody50LA(){if(attempted_&&!nativeWithdrawn_)std::terminate();}
};
}

#include "original_source_physics_dimensions_mass50la.inl"

#include "original_source_physics_inertia50la.inl"

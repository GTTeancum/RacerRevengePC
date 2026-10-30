#pragma once
#include "original_source_physics_body50la.h"
#include "original_wreck_forces50la.h"
namespace rr::game {
//Private isolated nullable60/282670 constructor. The real165F3C attachment
//must return before any root issues this leaf; no such issuer is added here.
class OriginalSourcePhysicsGravity50LA final {
    friend class OriginalSourcePhysicsCollision50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_static_model_cold50LA::Owner;
    using Base=OriginalSourcePhysicsObject50LA;
    using Body=OriginalSourcePhysicsBody50LA;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    enum class Status : std::uint8_t {reached,bounded,owner};
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    struct Storage {
        C entity;OriginalWreckGravity50LA gravity;
        std::uint32_t dispatch08=0;std::uint8_t flags29Known=0;
        //Actual source50 allocation alias, distinct from future slot payload.
        //Constructor publishesNULL; no fixed native target backing is proof
        //that282750 has allocated an array or282790 has written its entry.
        std::optional<const void*> array50;
    };
    Base* base_=nullptr;
    Body* targetOwner_=nullptr; //Exact native raw target borrow, no source retain.
    std::unique_ptr<OriginalWreckPhysical50LA*[]> array_;
    bool targetEntered_=false,reserveCountStored_=false,reserveCapacityStored_=false;
    bool arrayAllocationAttempted_=false,arrayPublished_=false,reserveReturned_=false;
    bool targetCallEntered_=false,targetStored_=false,targetCountStored_=false,targetReturned_=false;
    std::unique_ptr<Storage> gravity_;
    const std::array<std::uint32_t,4>* source4431C0_=nullptr;
    bool attempted_=false,counted_=false,constructorReturned_=false,published_=false;
    bool nativeRetired_=false,nativeWithdrawn_=false;
    C* publishedIdentity_=nullptr;
    bool attachEntered_=false,attachReturned_=false,attached_=false,detachedNative_=false;
    std::optional<std::uint32_t> attachLock_;
    std::uint32_t attachPC_=0;
    std::uint32_t pc_=0;
    Result at(Status s,std::uint32_t pc)noexcept {pc_=pc;return {s,pc};}
    Result allocateSource165F44(Base& actualBase,const Body& actualBody)noexcept {
        if(attempted_||nativeWithdrawn_||!actualBase.preflightNative()||!actualBase.object_||
           actualBase.gravityBorrower_||actualBase.object_->gravityD4||
           actualBody.base_!=&actualBase||!actualBody.attachReturned_||actualBody.detachedNative_||
           actualBody.nativeWithdrawn_||!actualBody.preflightNative()||
           !actualBody.body_||!actualBody.disableReturned_||!actualBody.counted_||
           actualBody.nativeRetired_||actualBody.nativeWithdrawn_||
           !actualBase.object_->bodyD0||*actualBase.object_->bodyD0!=&actualBody.body_->entity||
           actualBase.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return {Status::owner,0x165f44};
        //The body argument authenticates the exact owner, NOT an attachment
        //return. This isolated leaf has no caller; root must supply that
        //missing genuine predecessor before integration into165E90.
        base_=&actualBase;attempted_=true;base_->gravityBorrower_=this;
        source4431C0_=&base_->application_->gravityOrigin4431C0_;
        try{gravity_=std::make_unique<Storage>();}catch(...){gravity_.reset();}
        if(gravity_){
            auto& s=*gravity_;
            const auto r=initializeOriginalWreckComponent50LA(s.entity,K::entity141300,
                base_->generation_,Base::key(&s.entity),true,base_->application_->objectCount423F60());
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return at(Status::bounded,0x141300);
            counted_=true;s.flags29Known=0x0f;
            s.dispatch08=0x3d2e60;s.entity.kind=K::gravity282670; //28268C.
            //Existing exact constructor: vf0→capacity0→count0→4200CCCD→
            //actual4431C0 full16-byte copy→NULL50. No binding/force math.
            if(!constructOriginalWreckGravity282670(s.gravity,s.entity,source4431C0_->data()))
                return at(Status::bounded,0x282694);
            s.array50=nullptr;constructorReturned_=true;
        }
        publishedIdentity_=gravity_?&gravity_->entity:nullptr;
        base_->object_->gravityD4=publishedIdentity_;published_=true; //165F60 evenNULL.
        return at(Status::reached,0x165f64); //Before first vtable read165F68.
    }
    Result configureTargetSource165F70(Body& actualBody)noexcept {
        if(targetEntered_||!preflightNative()||!published_||pc_!=0x165f64||
           actualBody.base_!=base_||!actualBody.attachReturned_||actualBody.detachedNative_||
           actualBody.nativeWithdrawn_||!actualBody.preflightNative()||!actualBody.body_||
           !actualBody.disableReturned_||!actualBody.counted_||actualBody.nativeRetired_||
           actualBody.nativeWithdrawn_||actualBody.gravityTargetBorrower_||
           !base_->object_->bodyD0||*base_->object_->bodyD0!=&actualBody.body_->entity)
            return {Status::owner,0x165f70};
        targetEntered_=true;
        if(!gravity_)return at(Status::bounded,0x165f68); //First actual vtable read.
        auto& s=*gravity_;
        s.gravity.count30=0;reserveCountStored_=true; //28275C before capacity/allocation.
        s.gravity.capacity34=1;reserveCapacityStored_=true; //282760 actual a1=1.
        arrayAllocationAttempted_=true;
        //Exactly one uninitialized pointer slot. No identity target/default
        //payload is supplied when original nullable allocation fails.
        try{array_=std::unique_ptr<OriginalWreckPhysical50LA*[]>(new OriginalWreckPhysical50LA*[1]);}
        catch(...){array_.reset();}
        s.array50=static_cast<const void*>(array_.get());arrayPublished_=true; //282774 evenNULL.
        reserveReturned_=true; //Actual282784, includingNULL allocation.
        targetCallEntered_=true; //165F84 virtual78→282790, actual count0<capacity1.
        if(!array_)return at(Status::bounded,0x2827b0); //Actual destination store; no count increment.
        //Native pin before source slot mutation; no reference increment.
        targetOwner_=&actualBody;actualBody.gravityTargetBorrower_=this;
        array_[0]=&actualBody.body_->physical;targetStored_=true; //2827B0.
        s.gravity.target50=array_[0]; //Checked native projection of the SAME slot.
        s.gravity.count30=1;targetCountStored_=true; //2827BC follows pointer store.
        targetReturned_=true;
        return at(Status::reached,0x165f8c); //Before gravity141960 at165F94.
    }
    bool targetHistoryCurrentNative()const noexcept {
        if(!targetEntered_)return !reserveCountStored_&&!reserveCapacityStored_&&!arrayAllocationAttempted_&&
            !arrayPublished_&&!reserveReturned_&&!targetCallEntered_&&!targetStored_&&!targetCountStored_&&
            !targetReturned_&&!array_&&!targetOwner_;
        if(!gravity_)return pc_==0x165f68&&!reserveCountStored_&&!reserveCapacityStored_&&
            !arrayAllocationAttempted_&&!arrayPublished_&&!reserveReturned_&&!targetCallEntered_&&
            !targetStored_&&!targetCountStored_&&!targetReturned_&&!array_&&!targetOwner_;
        if(!reserveCountStored_||!reserveCapacityStored_||!arrayAllocationAttempted_||
           !arrayPublished_||!reserveReturned_||!targetCallEntered_||
           gravity_->array50!=std::optional<const void*>(static_cast<const void*>(array_.get())))return false;
        if(!array_)return pc_==0x2827b0&&!targetStored_&&!targetCountStored_&&!targetReturned_&&!targetOwner_&&
            !gravity_->gravity.target50;
        if(!targetStored_||!targetCountStored_||!targetReturned_||pc_!=0x165f8c||!targetOwner_||
           targetOwner_->base_!=base_||targetOwner_->gravityTargetBorrower_!=this||
           !targetOwner_->preflightNative()||!targetOwner_->body_||!targetOwner_->counted_||
           targetOwner_->nativeRetired_||targetOwner_->nativeWithdrawn_)return false;
        return array_[0]==&targetOwner_->body_->physical&&gravity_->gravity.target50==array_[0];
    }
    //Conditional original 165F94; caller supplies the same-App authority.
    template<class ActualAttach>
    Result attachSourceGravity141960(ActualAttach&& attach)noexcept {
        if(attachEntered_||nativeWithdrawn_||!(targetReturned_&&targetCountStored_)||!preflightNative()||
           !gravity_||!counted_||!published_||!base_||!base_->object_||base_->gravityChild141960_)
            return {Status::owner,0x165f94};
        auto& child=gravity_->entity;auto& parent=base_->object_->entity;
        if(child.parent1C||child.previous10||child.next14||child.firstChild18||child.mask20||child.descendants24)
            return {Status::owner,0x165f94};
        attachEntered_=true;attachLock_=base_->application_->attachmentLock423F58();attachPC_=0x141970;
        if(!*attachLock_){
            const auto result=attach(parent,child,0u,*attachLock_);
            if(result.status!=OriginalWreckComponentStatus50LA::complete){
                attachPC_=result.sourcePC;return {Status::bounded,attachPC_};
            }
            base_->gravityChild141960_=&child;attached_=true;
        }
        //A real nonzero lock returns NULL before touching the child. Caller
        //ignores that result; it is not a native callback refusal.
        attachReturned_=true;attachPC_=0x165f9c;return {Status::reached,attachPC_};
    }
    bool attachmentHistoryNative()const noexcept {
        if(!attachEntered_)return !attachReturned_&&!attached_&&!detachedNative_&&!attachLock_&&!attachPC_&&
            (!base_||!base_->gravityChild141960_);
        if(!base_||!base_->object_||!gravity_||!(targetReturned_&&targetCountStored_)||!attachLock_||
           (detachedNative_&&!attached_)||(*attachLock_&&attached_))return false;
        if(attachReturned_&&((*attachLock_?attached_:!attached_)||attachPC_!=0x165f9c))return false;
        const auto& child=gravity_->entity;const bool live=attached_&&!detachedNative_;
        if(base_->gravityChild141960_!=(live?&child:nullptr)||!base_->sourceChildChainCurrentNative()||
           child.parent1C!=(live?&base_->object_->entity:nullptr)||
           (!live&&(child.previous10||child.next14))||child.firstChild18||child.descendants24||
           child.mask20!=(attached_?0u:0u))return false;
        return true;
    }
    template<class ActualDetach>
    bool detachSourceGravityNative(ActualDetach&& detach)noexcept {
        if(!attempted_||nativeWithdrawn_||!attached_||detachedNative_)return true;
        if(!base_||!base_->application_||base_->application_->attachmentLock423F58()||!preflightNative())return false;
        const auto result=detach(gravity_->entity,0u);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        base_->gravityChild141960_=nullptr;detachedNative_=true;return true;
    }
    C* countedEntityNative()const noexcept {
        return gravity_&&counted_&&!nativeRetired_?&gravity_->entity:nullptr;
    }
    bool preflightNative()const noexcept {
        if(!attempted_)return !base_&&!gravity_&&!counted_&&!published_&&!nativeWithdrawn_;
        if(nativeWithdrawn_)return !base_&&!gravity_&&!source4431C0_&&!array_&&!targetOwner_&&(!counted_||nativeRetired_);
        if(!attachmentHistoryNative())return false;
        if(!base_||base_->gravityBorrower_!=this||!base_->preflightNative()||!base_->object_||
           source4431C0_!=&base_->application_->gravityOrigin4431C0_)return false;
        if(published_&&(!base_->object_->gravityD4||*base_->object_->gravityD4!=publishedIdentity_))return false;
        if(!published_&&base_->object_->gravityD4)return false;
        if(!targetHistoryCurrentNative())return false;
        if(!gravity_)return !counted_&&!constructorReturned_&&!nativeRetired_&&published_&&!publishedIdentity_;
        const auto& s=*gravity_;const auto& e=s.entity;
        if(!counted_)return !constructorReturned_&&!published_&&!nativeRetired_&&
            !e.reference00&&e.kind==K::unbound&&!s.dispatch08&&!s.flags29Known;
        if(nativeRetired_||e.reference00!=1||e.kind!=K::gravity282670||
           e.sessionGeneration!=base_->generation_||e.objectKey!=Base::key(&e)||
           (!attachEntered_&&(e.previous10||e.next14||e.parent1C))||e.firstChild18||e.mask20||e.descendants24||
           e.nativeFlags29||e.category28!=0xff||(e.flags29&0x0f)!=3||s.flags29Known!=0x0f||
           s.dispatch08!=0x3d2e60||base_->application_->objectCount423F60()<2)return false;
        //Helper admission refusal has no source field writes; it is retained
        //as a counted unpublished constructor prefix, never source return.
        if(!constructorReturned_)return !published_&&!s.array50&&pc_==0x282694;
        std::uint32_t acceleration;std::memcpy(&acceleration,&s.gravity.acceleration38,4);
        return published_&&publishedIdentity_==&e&&(targetEntered_||pc_==0x165f64)&&
            s.gravity.count30==(targetCountStored_?1u:0u)&&
            s.gravity.capacity34==(reserveCapacityStored_?1u:0u)&&acceleration==0x4200cccd&&
            !std::memcmp(s.gravity.force40.v,source4431C0_->data(),16)&&s.array50&&
            (arrayPublished_||(!*s.array50&&!s.gravity.target50));
    }
    template<class Retire>
    bool withdrawNative(Retire&& retire)noexcept {
        if(!attempted_||nativeWithdrawn_)return true;
        if((attached_&&!detachedNative_)||!preflightNative())return false;
        if(counted_&&!nativeRetired_){
            if(!retire(gravity_->entity))return false;
            gravity_->entity.reference00=0;--base_->application_->objectCount423F60();nativeRetired_=true;
        }
        //No refusal remains after callback; withdraw array/checked projection
        //before releasing the Body lifetime token. This is native cleanup,
        //not a source282750/282790 replay or source target50=NULL store.
        array_.reset();if(gravity_)gravity_->gravity.target50=nullptr;
        if(targetOwner_){targetOwner_->gravityTargetBorrower_=nullptr;targetOwner_=nullptr;}
        if(published_){base_->object_->gravityD4=nullptr;base_->object_->gravityD4NativeWithdrawn=true;}
        gravity_.reset();base_->gravityBorrower_=nullptr;base_=nullptr;source4431C0_=nullptr;
        nativeWithdrawn_=true;return true; //Native cancellation, not2826D0/source return.
    }
public:
    OriginalSourcePhysicsGravity50LA()=default;
    OriginalSourcePhysicsGravity50LA(const OriginalSourcePhysicsGravity50LA&)=delete;
    OriginalSourcePhysicsGravity50LA& operator=(const OriginalSourcePhysicsGravity50LA&)=delete;
    ~OriginalSourcePhysicsGravity50LA(){if(attempted_&&!nativeWithdrawn_)std::terminate();}
};
}

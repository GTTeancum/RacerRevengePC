#pragma once
#include <limits>
namespace ra_pod_scene50LA {
inline bool Owner::reserveSourceTriggerInstanceNative()noexcept {
    return worldApplication_&&worldApplication_->application10()&&worldEntity_.reference00==1&&
        worldEntity_.sessionGeneration&&!worldApplication_->attachmentLock423F58()&&
        worldApplication_->objectCount423F60()<=std::numeric_limits<std::uint32_t>::max()-2&&
        worldApplication_->entityAuthority_.enrollmentSerial_<=std::numeric_limits<std::uint64_t>::max()-2&&
        worldApplication_->entityAuthority_.reserveAdditional(2);
}
inline bool Owner::constructSourceTriggerInstance2DBFE0(NativeTriggerDescriptor& d,
    const std::array<std::uint32_t,4>& actualOrigin,std::uint32_t& pc)noexcept {
    if(!d.sourceFields||d.sourceInstanceAttempted||d.sourceInstance||d.trigger00||!worldApplication_)return false;
    d.sourceInstanceAttempted=true;pc=0x2dbfe0;
    try{d.sourceInstance.reset(new g::OriginalWorldTriggerInstance50LA);}catch(...){d.sourceInstance.reset();}
    if(d.sourceInstance){auto& t=*d.sourceInstance;
        if(!t.construct2A0290(*worldApplication_,worldEntity_.sessionGeneration,key(&t.entity_),actualOrigin,pc))return false;
        if(!worldApplication_->entityAuthority_.enroll(t.entity_)){pc=0x2dbffc;return false;}t.enrolled_=true;
    }
    pc=0x2dbffc;d.trigger00=d.sourceInstance?&d.sourceInstance->entity_:nullptr;d.sourceInstancePublished=true;
    return true;
}
inline bool Owner::setupSourceTriggerInstance2DC01C(NativeTriggerDescriptor& d,
    const std::array<std::uint32_t,4>& actualSentinel,std::uint32_t actualCategory,std::uint32_t& pc)noexcept {
    if(!d.sourceFields||!d.sourceFields->geometry64||!d.sourceInstancePublished)return false;
    const auto& f=*d.sourceFields;const auto& v=*f.geometry64;
    //2A04B0 stack copies and literal no-op141C20 execute even for source NULL.
    d.sourceInstanceStackVectors=std::array<std::array<std::uint32_t,4>,4>{{v.position00,v.box10,v.look20,v.up30}};
    d.sourceInstanceNoOpReturned=true;
    if(!d.sourceInstance){pc=0x2a0514;return false;}
    auto& t=*d.sourceInstance;
    const bool okay=t.setup2A04B0(v.position00,v.box10,v.look20,v.up30,v.radius4C,d.actionCount04,f.random10,
        actualSentinel,actualCategory,worldApplication_->collisionGlobalValid3E0730_,pc);
    //Native authority enrollment after the reached source prefix; capacity was
    //reserved before the source interval. Registry refusal can leave a real
    //counted-but-incomplete 11B190 object, which is enrolled/retained too.
    if(auto* c=t.colliderEntity();c&&c->reference00&&!t.colliderEnrolled_){
        if(!worldApplication_->entityAuthority_.enroll(*c))return false;t.colliderEnrolled_=true;}
    return okay;
}
inline std::size_t Owner::sourceTriggerInstanceCountNative()const noexcept {
    std::size_t result=0;if(nativeTriggerList3D0_&&nativeTriggerList3D0_->sourceMode)
        for(const auto& d:nativeTriggerList3D0_->descriptorOwners)if(d&&d->sourceInstance)result+=d->sourceInstance->countedEntities();
    return result;
}
inline bool Owner::preflightSourceTriggerInstanceNative(const NativeTriggerDescriptor& d)const noexcept {
    if(!d.sourceInstanceAttempted)return !d.sourceInstance&&!d.sourceInstancePublished&&!d.trigger00&&
        !d.sourceInstanceStackVectors&&!d.sourceInstanceNoOpReturned;
    if(d.sourceInstanceNoOpReturned!=d.sourceInstanceStackVectors.has_value())return false;
    if(!d.sourceInstance)return !d.trigger00;
    const auto& t=*d.sourceInstance;
    if(!worldApplication_||t.application_!=worldApplication_.get()||!t.preflightNative()||
       (d.sourceInstancePublished?d.trigger00!=&t.entity_:d.trigger00!=nullptr))return false;
    const auto& a=worldApplication_->entityAuthority_;
    const auto check=[&](const Entity& e,bool enrolled)noexcept {
        if(e.reference00&&(e.reference00!=1||e.sessionGeneration!=worldEntity_.sessionGeneration||e.objectKey!=key(&e)))return false;
        if(enrolled)return a.canRetire(e);
        for(std::size_t i=0;i<a.count_;++i)if(a.entries_[i].node==&e)return false;
        return true;
    };
    if(!check(t.entity_,t.enrolled_))return false;
    if(const auto* c=t.colliderEntity())return check(*c,t.colliderEnrolled_);
    return !t.colliderEnrolled_;
}
inline bool Owner::retireSourceTriggerInstanceNative(NativeTriggerDescriptor& d)noexcept {
    if(!preflightSourceTriggerInstanceNative(d))return false;
    if(!d.sourceInstance)return true;
    auto& t=*d.sourceInstance;
    //Direct disposal also obeys newest observer leases before Entity authority.
    if(!t.withdrawServiceMembershipNative()||!detachSourceTriggerRegistrationNative(d))return false;
    auto& a=worldApplication_->entityAuthority_;
    if(t.colliderEnrolled_){if(!a.retire(*t.colliderEntity()))return false;t.colliderEnrolled_=false;}
    if(t.enrolled_){if(!a.retire(t.entity_))return false;t.enrolled_=false;}
    //After the read-only preflight and exact membership withdrawals above,
    //remaining native backing disposal/count decrement is infallible. This
    //does not claim source142590/2A0420 or restore unknown source aliases.
    d.trigger00=nullptr; //Native knowledge withdrawn, NOT a source NULL store.
    d.sourceInstancePublished=false;t.closeNativeAfterAuthority();d.sourceInstance.reset();return true;
}
} //namespace ra_pod_scene50LA

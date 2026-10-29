#pragma once
#include <limits>
namespace ra_pod_scene50LA {
bool Owner::reserveSourceAmbientListNative()noexcept {
    return worldColdConstructed_&&worldApplication_&&worldApplication_->application10()&&
        worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&!worldApplication_->attachmentLock423F58()&&
        worldApplication_->objectCount423F60()<std::numeric_limits<std::uint32_t>::max()&&
        worldApplication_->entityAuthority_.enrollmentSerial_<std::numeric_limits<std::uint64_t>::max()&&
        worldApplication_->entityAuthority_.reserveAdditional(1);
}
bool Owner::constructSourceAmbientList2792E0(Entity& e,bool& counted,bool& enrolled,std::uint32_t& pc)noexcept {
    pc=0x2792ec;if(!worldApplication_||counted||enrolled)return false;
    auto& authority=worldApplication_->entityAuthority_;
    if(!authority.canEnroll(e,key(&e)))return false;
    const auto r=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::entity141300,
        worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());
    counted=e.reference00!=0;
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete){pc=r.sourcePC;return false;}
    pc=0x2792f4;if(!authority.enroll(e))return false;enrolled=true;return true;
}
bool Owner::preflightSourceAmbientListCountNative(const Entity& e,bool counted,bool enrolled)const noexcept {
    if(!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto& a=worldApplication_->entityAuthority_;
    if(counted){
        if(e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||e.objectKey!=key(&e)||
           e.sessionGeneration!=worldEntity_.sessionGeneration||e.category28!=0xff||(e.flags29&15u)!=3u||
           e.nativeFlags29||e.parent1C||e.firstChild18||e.previous10||e.next14||e.mask20||e.descendants24||
           !worldApplication_->objectCount423F60())return false;
    }else if(e.reference00)return false;
    if(enrolled)return counted&&a.canRetire(e);
    for(std::size_t i=0;i<a.count_;++i)if(a.entries_[i].node==&e)return false;
    return true;
}
bool Owner::releaseSourceAmbientListCountNative(Entity& e,bool& counted,bool& enrolled)noexcept {
    if(!preflightSourceAmbientListCountNative(e,counted,enrolled))return false;
    if(enrolled){if(!worldApplication_->entityAuthority_.retire(e))return false;enrolled=false;}
    if(counted){--e.reference00;--worldApplication_->objectCount423F60();counted=false;}
    return true; //Native exact-reference cleanup; not a guest142590/destructor receipt.
}
bool Owner::destroySourceEmptyAmbient279320(Entity& e,bool& counted,bool& enrolled,
    std::optional<std::uint32_t>& vtable,std::uint32_t& pc)noexcept {
    //All refusal gates precede actual104970. No callbacks/allocations between
    //this exact authority withdrawal and the now-infallible empty destructor.
    if(!counted||!enrolled||!preflightSourceAmbientListCountNative(e,counted,enrolled))return false;
    if(!worldApplication_->entityAuthority_.retire(e))return false;enrolled=false;
    pc=0x104980;--e.reference00; //142590 ->104970, final reference.
    pc=0x279344;vtable=0x3d2980; //Actual derived vtable; rows34 is NULL, no array destructor.
    pc=0x141404;vtable=0x3cd240; //Actual base vtable before count decrement.
    pc=0x141420;--worldApplication_->objectCount423F60();counted=false;
    pc=0x279380;return true; //Caller frees exact outer then sourceNULL2714D0.
}
}

#pragma once
#include <limits>
namespace ra_pod_scene50LA {
bool Owner::reserveSourceMaterialSoundListNative()noexcept {
    return worldColdConstructed_&&worldApplication_&&worldApplication_->application10()&&
        worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&!worldApplication_->attachmentLock423F58()&&
        worldApplication_->objectCount423F60()<std::numeric_limits<std::uint32_t>::max()&&
        worldApplication_->entityAuthority_.enrollmentSerial_<std::numeric_limits<std::uint64_t>::max()&&
        worldApplication_->entityAuthority_.reserveAdditional(1);
}
bool Owner::constructSourceMaterialSoundList279AA0(Entity& e,bool drive,bool& counted,bool& enrolled,std::uint32_t& pc)noexcept {
    pc=drive?0x279d9c:0x279aac;if(!worldApplication_||counted||enrolled)return false;
    auto& authority=worldApplication_->entityAuthority_;
    if(!authority.canEnroll(e,key(&e)))return false;
    const auto r=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::entity141300,
        worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());
    counted=e.reference00!=0;
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete){pc=r.sourcePC;return false;}
    pc=drive?0x279da4:0x279ab4;if(!authority.enroll(e))return false;enrolled=true;return true;
}
bool Owner::preflightSourceMaterialSoundListCountNative(const Entity& e,bool counted,bool enrolled)const noexcept {
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
bool Owner::releaseSourceMaterialSoundListCountNative(Entity& e,bool& counted,bool& enrolled)noexcept {
    if(!preflightSourceMaterialSoundListCountNative(e,counted,enrolled))return false;
    if(enrolled){if(!worldApplication_->entityAuthority_.retire(e))return false;enrolled=false;}
    if(counted){--e.reference00;--worldApplication_->objectCount423F60();counted=false;}
    return true; //Native exact-reference cleanup; not a guest142590/destructor receipt.
}
}

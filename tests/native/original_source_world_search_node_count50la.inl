#pragma once
#include <limits>
namespace ra_pod_scene50LA {
bool Owner::reserveSourceSearchNodesNative()noexcept {
    return worldColdConstructed_&&worldApplication_&&worldApplication_->application10()&&
        worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&!worldApplication_->attachmentLock423F58()&&
        worldApplication_->objectCount423F60()<=std::numeric_limits<std::uint32_t>::max()-20u&&
        worldApplication_->entityAuthority_.enrollmentSerial_<=std::numeric_limits<std::uint64_t>::max()-20u&&
        worldApplication_->entityAuthority_.reserveAdditional(20);
}
bool Owner::constructSourceSearchNode27F120(SourceSearchNode27F120& n,std::uint32_t& pc)noexcept {
    pc=0x27f120;if(!n.owner||!n.attempted||n.counted||n.enrolled||n.constructorMask||!worldApplication_)return false;
    auto& z=*n.owner;auto& c=z.entity;auto& a=worldApplication_->entityAuthority_;
    if(!a.canEnroll(c,key(&c)))return false; //Native admission before actual Entity effects.
    const auto result=g::initializeOriginalWreckComponent50LA(c,g::OriginalWreckComponentKind50LA::searchNode27F120,
        worldEntity_.sessionGeneration,key(&c),true,worldApplication_->objectCount423F60());
    n.counted=c.reference00!=0;if(result.status!=g::OriginalWreckComponentStatus50LA::complete){pc=result.sourcePC;return false;}
    pc=0x27f13c;n.vtable08=0x3d2dd0;n.constructorMask|=1;
    pc=0x27f140;z.portalLinks30.reset();n.constructorMask|=2;
    pc=0x27f144;z.nativeList34=nullptr;n.constructorMask|=4;
    pc=0x27f148;z.nativeList38=nullptr;n.constructorMask|=8;
    pc=0x27f14c;z.ordinal48=0;n.constructorMask|=16;
    pc=0x27f150;z.exclusions3C.reset();n.pointer3C=nullptr;n.constructorMask|=32;
    pc=0x27f154;z.exclusionCount40=0;n.constructorMask|=64;
    pc=0x27f158;n.pointer44=nullptr;n.constructorMask|=128;
    pc=0x27f15c;n.pointerA0=nullptr;n.constructorMask|=256;
    z.constructed=true;
    if(!a.enroll(c))return false;n.enrolled=true;pc=0x27f170;return true;
}
bool Owner::preflightSourceSearchNodeNative(const SourceSearchNode27F120& n,std::size_t index,bool collision)const noexcept {
    if(index>=10||(n.constructorMask&~511u))return false;
    if(n.withdrawn)return !n.owner&&!n.counted&&!n.enrolled;
    if(!n.owner)return !n.counted&&!n.enrolled&&!n.constructorMask&&!n.vtable08&&!n.ordinalKnown&&
        !n.pointer3C&&!n.pointer44&&!n.pointerA0&&!n.exclusionAttempted&&!n.exclusionAllocationReturned&&!n.exclusionReturned&&!n.exclusionProduced;
    if(!n.attempted||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto& z=*n.owner;const auto& c=z.entity;const auto& a=worldApplication_->entityAuthority_;
    if(!sourceMethodNodeAttachmentCurrent(z,collision,index)||!sourceListMethodAliasesCurrent(z,collision,index)||
       !sourcePortalWorldLinksCurrent(z,collision,index)||z.collectionBacking||z.exclusionSource||z.exclusionsComplete)return false;
    if(n.counted){
        if(c.reference00!=1||c.kind!=g::OriginalWreckComponentKind50LA::searchNode27F120||c.objectKey!=key(&c)||
           c.sessionGeneration!=worldEntity_.sessionGeneration||c.category28!=0xff||(c.flags29&15u)!=3u||
           c.nativeFlags29||c.firstChild18||c.descendants24||
           !worldApplication_->objectCount423F60())return false;
    }else if(c.reference00||z.constructed)return false;
    if(n.enrolled){if(!n.counted||!a.canRetire(c))return false;}
    else for(std::size_t j=0;j<a.count_;++j)if(a.entries_[j].node==&c)return false;
    if(z.constructed){
        if(n.constructorMask!=511||!n.vtable08||*n.vtable08!=0x3d2dd0||!n.pointer44||*n.pointer44||
           !n.pointerA0||*n.pointerA0||!n.pointer3C||*n.pointer3C!=z.exclusions3C.get()||!z.exclusionCount40||
           z.ordinal48!=(n.ordinalKnown?index:0))return false;
    }else if(n.constructorMask||n.vtable08||n.pointer3C||n.pointer44||n.pointerA0||z.exclusionCount40||z.exclusions3C)return false;
    if(n.exclusionAttempted){
        if(!z.constructed||!n.ordinalKnown||!z.exclusionCount40||*z.exclusionCount40>128||
           n.exclusionProduced>*z.exclusionCount40||
           (n.exclusionProduced&&!z.exclusions3C)||
           (n.exclusionReturned&&n.exclusionProduced!=*z.exclusionCount40))return false;
        if(!n.exclusionAllocationReturned&&(z.exclusions3C||n.exclusionProduced||n.exclusionReturned))return false;
    }else if(n.exclusionAllocationReturned||n.exclusionProduced||n.exclusionReturned||z.exclusions3C||
        (z.exclusionCount40&&*z.exclusionCount40))return false;
    return true;
}
bool Owner::releaseSourceSearchNodeNative(SourceSearchNode27F120& n)noexcept {
    //Whole20-node/array/authority preflight and combined count floor precede this helper.
    if(n.withdrawn)return true;
    if(n.owner){auto& z=*n.owner;auto& c=z.entity;
        if(z.portalLinks30||z.attached||z.nativeList34||z.nativeList38||c.parent1C||c.previous10||c.next14)return false;
        //No attachment or raw Portal/neighbor/List borrower may survive authority withdrawal.
        for(const auto& f:sourceMethodNodeAttachments_)if(f.attempted&&!f.withdrawn&&f.identity==&z)return false;
        if(n.enrolled){if(!worldApplication_->entityAuthority_.retire(c))return false;n.enrolled=false;}
        //No fallible operation remains: plain exclusion backing before Entity count disposal.
        z.exclusions3C.reset();
        if(n.counted){--c.reference00;--worldApplication_->objectCount423F60();n.counted=false;}
        n.withdrawn=true;n.owner.reset();
    }else n.withdrawn=true;
    return true; //Native withdrawal, not original NULL3C/40 or virtual destructor execution.
}
}

#pragma once
#include <limits>
namespace ra_pod_scene50LA {
inline bool Owner::reserveSourceToyAuthorityNative()noexcept {
    return !sourceToyAttempted_&&!nativeEmptyToyList3C8_&&worldApplication_&&worldApplication_->application10()&&
        !worldApplication_->attachmentLock423F58()&&worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&
        worldApplication_->objectCount423F60()!=std::numeric_limits<std::uint32_t>::max()&&
        worldApplication_->entityAuthority_.reserveAdditional(1);
}
inline bool Owner::constructSourceToy2D9B40(std::uint32_t& pc)noexcept {
    pc=0x2d9b4c;
    if(!sourceToyAttempted_||!nativeEmptyToyList3C8_||!nativeEmptyToyList3C8_->sourceMode||!worldApplication_)return false;
    auto& list=*nativeEmptyToyList3C8_;auto& authority=worldApplication_->entityAuthority_;
    if(list.sourceCounted||list.sourceEnrolled||!authority.canEnroll(list.entity,key(&list.entity)))return false;
    const auto r=g::initializeOriginalWreckComponent50LA(list.entity,g::OriginalWreckComponentKind50LA::toyList2D9B40,
        worldEntity_.sessionGeneration,key(&list.entity),true,worldApplication_->objectCount423F60());
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete){pc=r.sourcePC;return false;}
    list.sourceCounted=true; //Retain actual count/ref if subsequent native enrollment refuses.
    if(!authority.enroll(list.entity)){pc=0x2d9b54;return false;}list.sourceEnrolled=true;
    pc=0x2d9b5c;list.sourceVtable08=0x3d5390;list.sourceVtableKnown=true;
    pc=0x2d9b60;list.count30=0;list.sourceCountKnown=true;
    pc=0x2d9b64;list.sourceArray34=nullptr;list.sourceArrayKnown=true;return true;
}
inline bool Owner::canRetireSourceToyNative()const noexcept {
    if(!nativeEmptyToyList3C8_)return true;
    const auto& list=*nativeEmptyToyList3C8_;const auto& e=list.entity;
    if(!list.sourceMode||!sourceToyAttempted_||!worldApplication_||worldApplication_->attachmentLock423F58()||
       (list.sourceCountKnown&&list.count30)||(list.sourceArrayKnown&&list.sourceArray34))return false;
    if(!list.sourceCounted)return !list.sourceEnrolled&&!e.reference00;
    if(e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||e.mask20||e.descendants24||
       e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration||
       !worldApplication_->objectCount423F60())return false;
    const auto& authority=worldApplication_->entityAuthority_;
    if(list.sourceEnrolled)return authority.canRetire(e);
    //Only the exact failed native enrollment prefix, not a forged registered Entity.
    for(std::size_t n=0;n<authority.count_;++n)if(authority.entries_[n].node==&e)return false;
    return true;
}
inline bool Owner::retireSourceToyCountNative(std::uint32_t& pc,bool sourceDestructor)noexcept {
    if(!canRetireSourceToyNative())return false;
    if(!nativeEmptyToyList3C8_||!nativeEmptyToyList3C8_->sourceCounted)return true;
    auto& list=*nativeEmptyToyList3C8_;
    //Native authority withdrawal must succeed before source reference/count
    //changes. It is not itself the original142590/104970/1413E0 call.
    if(list.sourceEnrolled&&!worldApplication_->entityAuthority_.retire(list.entity))return false;
    list.sourceEnrolled=false;
    pc=sourceDestructor?0x104980:0;--list.entity.reference00;
    if(sourceDestructor){pc=0x2d9bac;list.sourceVtable08=0x3d5390;
        pc=0x141404;list.sourceVtable08=0x3cd240;}
    pc=sourceDestructor?0x141418:0;--worldApplication_->objectCount423F60();list.sourceCounted=false;
    return true;
}
} //namespace ra_pod_scene50LA

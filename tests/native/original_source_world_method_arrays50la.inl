#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldMethodArraysNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||!preflightSourceListMethodsNative())return false;
    NativeListMethodCustody* const* backings[4]={nativeMethods4C8_.get(),nativeMethods4CC_.get(),nativeMethods4D0_.get(),nativeMethods4D4_.get()};
    if(sourceMethodArraysPhase_==SourceMethodArraysPhase::unreached){
        if(sourceMethodArrayAllocationMask_||sourceMethodS0_)return false;
        for(unsigned k=0;k<4;++k){if(backings[k]||sourceMethodArrays_[k])return false;
            for(const auto& selector:sourceMethodSelectors_[k])if(selector)return false;}
        for(auto m:sourceMethodSlotNullMask_)if(m)return false;
        for(auto m:sourceMethodReturnedMask_)if(m)return false;
        return preflightSourceMethodNodeAttachmentsNative(nullptr); //No method attachment attempt exists, including earlier partial container construction.
    }
    if(sourceMethodArrayAllocationMask_&~15u)return false;
    if(sourceMethodArraysPhase_==SourceMethodArraysPhase::nativeClosed){
        for(unsigned k=0;k<4;++k)if(backings[k]||sourceMethodArrays_[k])return false;
        return preflightSourceMethodNodeAttachmentsNative(nullptr); //Withdrawn historical node identities are not dereferenced.
    }
    if(!sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceTopologyContainer_||
       !preflightSourceMethodNodeAttachmentsNative(sourceTopologyContainer_.get()))return false;
    bool missing=false;
    for(unsigned k=0;k<4;++k){const bool allocated=sourceMethodArrayAllocationMask_&(1u<<k);
        if(!allocated){missing=true;if(backings[k]||sourceMethodArrays_[k])return false;}
        else if(missing||!sourceMethodArrays_[k]||*sourceMethodArrays_[k]!=backings[k])return false;
    }
    if(sourceMethodS0_&&(*sourceMethodS0_>10||sourceMethodArrayAllocationMask_!=15))return false;
    for(std::size_t row=0;row<10;++row){
        const auto mask=sourceMethodSlotNullMask_[row],returned=sourceMethodReturnedMask_[row];
        if((mask&~15u)||(returned&~15u))return false;
        if((mask||returned)&&(!sourceMethodS0_||row>*sourceMethodS0_))return false;
        if(sourceMethodS0_&&row<*sourceMethodS0_&&(mask!=15||returned!=15))return false;
        bool missingNull=false,missingReturn=false;
        for(unsigned k=0;k<4;++k){const auto& r=sourceListMethods_[k][row];
            const bool known=mask&(1u<<k),done=returned&(1u<<k);
            if(!known)missingNull=true;else if(missingNull||!backings[k])return false;
            if(!done)missingReturn=true;else if(missingReturn||!r.aliasRetained)return false;
            if(sourceMethodSelectors_[k][row]&&(!sourceMethodS0_||row>*sourceMethodS0_||mask!=15))return false;
            if(r.attempted&&(!sourceMethodSelectors_[k][row]||*sourceMethodSelectors_[k][row]!=1))return false;
            if(r.arrayPublished&&!known)return false;
            if(r.withdrawn)continue; //Historical pointer slots may dangle after actual owner disposal.
            if(known){const auto* pointer=backings[k][row];
                if(r.arrayPublished){if(!sourceListMethodArrayCurrent(k,row,pointer))return false;}
                else if(pointer)return false;
            }
            if(done&&!r.aliasPublished)return false;
        }
    }
    if(sourceMethodArraysPhase_==SourceMethodArraysPhase::returned)
        return sourceMethodS0_&&*sourceMethodS0_==10&&sourceMethodArrayAllocationMask_==15;
    return true;
}
bool Owner::sourceWorldMethodArraysInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTopologyPhase_==SourceTopologyPhase::portalsReturned&&sourceTopologyPC_==0x272c90&&sourceTrackPC_==0x272c90&&
        sourcePortalGraphPhase_==SourcePortalGraphPhase::returned&&sourcePortalGraphPC_==0x272c90&&
        sourcePortalParameterFreeReached_&&sourcePortalParameterNullStored_&&sourcePortal3A0_&&!*sourcePortal3A0_&&
        sourceTopologyMode_&&!sourceTopologyWithdrawn_&&sourceTopologyContainer_&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&worldZones34C_&&*worldZones34C_==10&&
        sourceZonesPhase_==SourceZonesPhase::returned&&sourceZonesLoaded_==10&&sourceZones350_&&
        worldZoneInfo350_&&*worldZoneInfo350_==sourceZones350_.get()&&
        preflightSourceWorldMethodArraysNative()&&preflightSourceWorldTopologyCountNative();
}
bool Owner::abortSourceWorldMethodArraysNative(std::uint32_t& pc)noexcept {
    if(!preflightSourceWorldTopologyCountNative())return false;
    if(sourceMethodArraysPhase_==SourceMethodArraysPhase::unreached||sourceMethodArraysPhase_==SourceMethodArraysPhase::nativeClosed)return true;
    if(!retireSourceListMethodsNative(pc)||!detachSourceMethodNodesNative(sourceTopologyContainer_.get(),pc))return false;
    //No raw aliases/owned List references remain; never read an unknown or historical slot while disposing arrays.
    for(auto& pointer:sourceMethodArrays_)pointer.reset();
    nativeMethods4D4_.reset();nativeMethods4D0_.reset();nativeMethods4CC_.reset();nativeMethods4C8_.reset();
    sourceMethodArraysPhase_=SourceMethodArraysPhase::nativeClosed;return true;
}
}

#pragma once
#include <new>
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceWorldMethodArrays272C90(std::uint32_t& pc)noexcept {
    pc=sourceMethodArraysPC_;
    if(sourceMethodArraysPhase_!=SourceMethodArraysPhase::unreached||!sourceWorldMethodArraysInvocationCurrent())return false;
    sourceMethodArraysPhase_=SourceMethodArraysPhase::entered;
    const auto at=[&](std::uint32_t value){pc=sourceMethodArraysPC_=value;};
    const auto stop=[&](std::uint32_t value){at(value);sourceMethodArraysPhase_=SourceMethodArraysPhase::bounded;return false;};
    std::unique_ptr<NativeListMethodCustody*[]>* backings[4]={&nativeMethods4C8_,&nativeMethods4CC_,&nativeMethods4D0_,&nativeMethods4D4_};
    at(0x272c90);if(!worldZones34C_)return stop(pc);
    at(0x272c98);if(*worldZones34C_!=10)return stop(pc); //Exact authored branch, no single-zone quadtree adoption.
    constexpr std::uint32_t countPC[]{0x272e38,0x272e48,0x272e58,0x272e68};
    constexpr std::uint32_t allocPC[]{0x272e3c,0x272e4c,0x272e5c,0x272e6c};
    constexpr std::uint32_t publishPC[]{0x272e44,0x272e54,0x272e64,0x272e74};
    for(unsigned kind=0;kind<4;++kind){
        at(countPC[kind]);if(!worldZones34C_||*worldZones34C_!=10)return stop(pc);
        at(allocPC[kind]);backings[kind]->reset(new(std::nothrow) NativeListMethodCustody*[10]);
        sourceMethodArrayAllocationMask_|=1u<<kind;
        at(publishPC[kind]);sourceMethodArrays_[kind]=backings[kind]->get(); //Actual NULL is published; no slot initialized here.
    }
    at(0x272e7c);sourceMethodS0_=0;
    for(;;){
        at(0x272f18);if(!worldZones34C_)return stop(pc);
        at(0x272f1c);if(static_cast<std::int32_t>(*sourceMethodS0_)>=static_cast<std::int32_t>(*worldZones34C_))break;
        const auto index=static_cast<std::size_t>(*sourceMethodS0_);if(index>=10)return stop(pc);
        at(0x272e80);if(!sourceVisibility4C0_)return stop(pc);auto* visibility=*sourceVisibility4C0_;
        at(0x272e88);if(!sourceContainer4BC_)return stop(pc);auto* parent=*sourceContainer4BC_;
        at(0x272e90);if(!visibility||index>=sourceVisibilityPublished_||visibility[index]!=sourceSearchNodes_[0][index].owner.get())return stop(pc);
        at(0x272e94);if(!attachSourceMethodNode141960(sourceSearchNodes_[0][index],false,index,parent,pc))return stop(pc);
        at(0x272e9c);if(!sourceCollision4C4_)return stop(pc);auto* collision=*sourceCollision4C4_;
        at(0x272ea4);if(!sourceContainer4BC_)return stop(pc);parent=*sourceContainer4BC_;
        at(0x272eac);if(!collision||index>=sourceCollisionPublished_||collision[index]!=sourceSearchNodes_[1][index].owner.get())return stop(pc);
        at(0x272eb0);if(!attachSourceMethodNode141960(sourceSearchNodes_[1][index],true,index,parent,pc))return stop(pc);
        constexpr std::uint32_t arrayPC[]{0x272eb8,0x272ecc,0x272ed8,0x272ee4};
        constexpr std::uint32_t nullPC[]{0x272ec8,0x272ed4,0x272ee0,0x272eec};
        for(unsigned kind=0;kind<4;++kind){
            at(arrayPC[kind]);if(!sourceMethodArrays_[kind])return stop(pc);auto* slots=*sourceMethodArrays_[kind];
            at(nullPC[kind]);if(!slots)return stop(pc);slots[index]=nullptr;sourceMethodSlotNullMask_[index]|=1u<<kind;
        }
        constexpr std::uint32_t zonePC[]{0x27319c,0x2733c8,0x27362c,0x273858};
        constexpr std::uint32_t selectorPC[]{0x2731b4,0x2733dc,0x273644,0x27386c};
        constexpr unsigned selectorIndex[]{2,3,0,1};
        constexpr std::uint32_t methodArrayPC[]{0x2731e0,0x273408,0x273670,0x273898};
        constexpr std::uint32_t methodStorePC[]{0x2731ec,0x273414,0x27367c,0x2738a4};
        constexpr std::uint32_t nodeArrayPC[]{0x2731f0,0x273418,0x273680,0x2738a8};
        constexpr std::uint32_t methodReloadPC[]{0x2731f4,0x27341c,0x273684,0x2738ac};
        constexpr std::uint32_t methodReadPC[]{0x273200,0x273428,0x273690,0x2738b8};
        constexpr std::uint32_t nodeReadPC[]{0x273208,0x273430,0x273698,0x2738c0};
        for(unsigned kind=0;kind<4;++kind){
            if(kind==0)at(0x272ef8);else if(kind==2)at(0x272f0c); //Distinct actual v104/v108 frames, no later selector read early.
            at(zonePC[kind]);if(!worldZoneInfo350_||!sourceZones350_||*worldZoneInfo350_!=sourceZones350_.get())return stop(pc);
            at(selectorPC[kind]);sourceMethodSelectors_[kind][index]=sourceZones350_[index].search110[selectorIndex[kind]];
            if(*sourceMethodSelectors_[kind][index]!=1)return stop(pc); //Other real selector branches remain unjoined.
            if(!allocateSourceListMethod27E260(kind,index,pc))return stop(pc);
            NativeListMethodCustody* returned=nullptr;if(!sourceListMethodReturned(kind,index,returned))return stop(pc);
            at(methodArrayPC[kind]);if(!sourceMethodArrays_[kind])return stop(pc);auto* slots=*sourceMethodArrays_[kind];
            at(methodStorePC[kind]);if(!slots)return stop(pc);slots[index]=returned;sourceListMethods_[kind][index].arrayPublished=true;
            at(nodeArrayPC[kind]);const auto& nodeArray=(kind&1u)?sourceCollision4C4_:sourceVisibility4C0_;
            if(!nodeArray)return stop(pc);auto* nodes=*nodeArray;
            at(methodReloadPC[kind]);if(!sourceMethodArrays_[kind])return stop(pc);slots=*sourceMethodArrays_[kind];
            at(methodReadPC[kind]);if(!slots)return stop(pc);if(!sourceListMethodArrayCurrent(kind,index,slots[index]))return stop(pc);
            at(nodeReadPC[kind]);if(!nodes)return stop(pc);auto* actualNode=nodes[index];
            if(!aliasSourceListMethod27F420(kind,index,actualNode,pc))return stop(pc);
            sourceMethodReturnedMask_[index]|=1u<<kind;
        }
        at(0x272f14);++*sourceMethodS0_;
    }
    sourceMethodArraysPhase_=SourceMethodArraysPhase::returned;at(0x272f28);return true;
}
}

#pragma once
#include <bit>
#include <new>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldSearchNodesNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceSearchNodesPhase_==SourceSearchNodesPhase::unreached){
        if(sourceSearchNodeS0_||sourceSearchZonesIdentity_||sourceSearchExclusionCount_||sourceSearchExclusionArg_||
           sourceSearchHelperA2_||sourceSearchHelperA3_||sourceSearchHelperSourceOffset_||sourceSearchLoadedWord_||sourceSearchStorePending_)return false;
        for(const auto& rows:sourceSearchNodes_)for(const auto& n:rows)
            if(n.owner||n.attempted||n.counted||n.enrolled||n.published||n.ordinalKnown||n.withdrawn||n.constructorMask||n.vtable08||
               n.pointer3C||n.pointer44||n.pointerA0||n.exclusionAttempted||n.exclusionAllocationReturned||n.exclusionReturned||n.exclusionProduced)return false;
        return true; //No access to a legacy or unrelated topology's slots.
    }
    if(!sourceSearchNodeS0_||*sourceSearchNodeS0_>10||!sourceSearchZonesIdentity_)return false;
    if(sourceSearchNodesPhase_==SourceSearchNodesPhase::nativeClosed){
        for(const auto& rows:sourceSearchNodes_)for(const auto& n:rows)if(!n.withdrawn||n.owner||n.counted||n.enrolled)return false;
        return true; //Historical Zone and slot pointers are not dereferenced.
    }
    if(!sourceTopologyMode_||sourceTopologyWithdrawn_||topologyAttempted_||topologyConstructed_||topologyClosed_||
       !worldApplication_||worldEntity_.reference00!=1||!worldEntity_.sessionGeneration||
       !sourceVisibility4C0_||*sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get()||
       !sourceCollision4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
       !sourceZones350_||sourceSearchZonesIdentity_!=sourceZones350_.get()||
       !worldZoneInfo350_||*worldZoneInfo350_!=sourceZones350_.get()||sourceZonesLoaded_!=10||
       sourceZonesPhase_!=SourceZonesPhase::returned||!preflightSourceWorldZonesNative()||
       nativeWreckZoneBorrower_||constructorRacerWorldBorrows_)return false;
    std::size_t published[2]={0,0};
    for(std::size_t kind=0;kind<2;++kind)for(std::size_t i=0;i<10;++i){const auto& n=sourceSearchNodes_[kind][i];
        if(!preflightSourceSearchNodeNative(n,i,kind!=0))return false;
        if(n.published){++published[kind];if(!n.withdrawn){
            const auto* slots=kind?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
            if(!slots||slots[i]!=n.owner.get())return false;
        }}
        if(i>*sourceSearchNodeS0_&&(n.attempted||n.published||n.ordinalKnown||n.constructorMask||n.exclusionAttempted))return false;
        if(kind&&n.exclusionAttempted)return false;
        if(i<*sourceSearchNodeS0_&&(!n.published||!n.ordinalKnown||n.constructorMask!=511))return false;
        if(n.exclusionReturned&&!n.withdrawn){const auto& z=sourceZones350_[i];
            if(!n.owner||!n.owner->exclusionCount40||z.exclusionCount124<0||
               *n.owner->exclusionCount40!=static_cast<std::uint32_t>(z.exclusionCount124)||
               n.exclusionProduced!=static_cast<std::size_t>(z.exclusionCount124))return false;}
    }
    if(published[0]!=sourceVisibilityPublished_||published[1]!=sourceCollisionPublished_||
       sourceVisibilityPublished_>10||sourceCollisionPublished_>10)return false;
    if(sourceSearchHelperA2_||sourceSearchHelperA3_||sourceSearchHelperSourceOffset_){
        if(!sourceSearchHelperA2_||!sourceSearchHelperA3_||!sourceSearchHelperSourceOffset_||
           !sourceSearchExclusionCount_||*sourceSearchExclusionCount_<=0||*sourceSearchExclusionCount_>128||
           *sourceSearchHelperA2_>static_cast<std::uint32_t>(*sourceSearchExclusionCount_)||
           *sourceSearchHelperA3_!=4u**sourceSearchHelperA2_||
           *sourceSearchHelperSourceOffset_!=*sourceSearchHelperA3_)return false;
    }
    if(sourceSearchStorePending_&&(!sourceSearchLoadedWord_||!sourceSearchHelperA2_||!*sourceSearchHelperA2_))return false;
    if(sourceSearchNodesPhase_==SourceSearchNodesPhase::returned){
        if(*sourceSearchNodeS0_!=10||sourceVisibilityPublished_!=10||sourceCollisionPublished_!=10||sourceSearchStorePending_)return false;
        for(std::size_t i=0;i<10;++i){const auto& z=sourceZones350_[i];const auto& v=sourceSearchNodes_[0][i];
            if(z.exclusionCount124>0&&!v.exclusionReturned)return false;}
    }
    return true;
}
bool Owner::sourceWorldSearchNodesCounted(std::size_t& count)const noexcept {
    count=0;if(!preflightSourceWorldSearchNodesNative())return false;
    for(const auto& rows:sourceSearchNodes_)for(const auto& n:rows)if(n.counted)++count;
    return true;
}
bool Owner::abortSourceWorldSearchNodesNative(std::uint32_t& pc)noexcept {
    if(sourcePortalGraphPhase_!=SourcePortalGraphPhase::unreached&&sourcePortalGraphPhase_!=SourcePortalGraphPhase::nativeClosed)return false;
    if(!preflightSourceWorldTopologyCountNative())return false;
    if(sourceSearchNodesPhase_==SourceSearchNodesPhase::unreached||sourceSearchNodesPhase_==SourceSearchNodesPhase::nativeClosed)return true;
    //All20 authorities, source slots, exclusion identities and the combined count
    //floor were checked before the first deletion. No callbacks are invoked.
    for(std::size_t i=10;i>0;--i)for(std::size_t kind=2;kind>0;--kind)
        if(!releaseSourceSearchNodeNative(sourceSearchNodes_[kind-1][i-1])){pc=0x27f180;return false;}
    //Keep actual published-slot bytes/count/constructor/cursor history. They are
    //unusable after withdrawal and parent frees the arrays only after this returns.
    sourceSearchNodesPhase_=SourceSearchNodesPhase::nativeClosed;return true;
}
bool Owner::continueSourceWorldSearchNodes272A50(std::uint32_t& pc)noexcept {
    pc=sourceSearchNodesPC_;
    if(sourceSearchNodesPhase_!=SourceSearchNodesPhase::unreached||!preflightSourceWorldSearchNodesNative()||
       !sourceWorldSearchNodesInvocationCurrent()||!reserveSourceSearchNodesNative())return false;
    //Capacity/authority admission is native and precedes actual loop entry.
    sourceSearchNodesPhase_=SourceSearchNodesPhase::entered;sourceSearchZonesIdentity_=sourceZones350_.get();
    const auto at=[&](std::uint32_t p){pc=sourceSearchNodesPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceSearchNodesPhase_=SourceSearchNodesPhase::bounded;return false;};
    at(0x272a54);sourceSearchNodeS0_=0; //Actual unconditional branch delay.
    for(;;){
        at(0x272b18);if(!worldZones34C_)return stop(pc);const auto count=*worldZones34C_;
        at(0x272b1c);if(static_cast<std::int32_t>(*sourceSearchNodeS0_)>=static_cast<std::int32_t>(count))break;
        const auto i=static_cast<std::size_t>(*sourceSearchNodeS0_);if(i>=10)return stop(0x272a58);
        auto& v=sourceSearchNodes_[0][i];auto& c=sourceSearchNodes_[1][i];
        at(0x272a58);v.attempted=true;v.owner.reset(new(std::nothrow) SearchNode);
        if(v.owner&&!constructSourceSearchNode27F120(v,pc))return stop(pc);
        at(0x272a8c);if(!sourceVisibilitySlots4C0_)return stop(pc);
        sourceVisibilitySlots4C0_[i]=v.owner.get();v.published=true;++sourceVisibilityPublished_;
        //The preceding store is the272A88 malloc delay, not after its result.
        at(0x272a88);c.attempted=true;c.owner.reset(new(std::nothrow) SearchNode);
        if(c.owner&&!constructSourceSearchNode27F120(c,pc))return stop(pc);
        at(0x272ac0);if(!sourceCollisionSlots4C4_)return stop(pc);
        sourceCollisionSlots4C4_[i]=c.owner.get();c.published=true;++sourceCollisionPublished_;
        at(0x272ad4);if(!v.owner)return stop(pc);v.owner->ordinal48=static_cast<std::uint32_t>(i);v.ordinalKnown=true;
        at(0x272ae4);if(!c.owner)return stop(pc);c.owner->ordinal48=static_cast<std::uint32_t>(i);c.ordinalKnown=true;
        at(0x272af0);if(!sourceZones350_||sourceSearchZonesIdentity_!=sourceZones350_.get())return stop(pc);
        const auto& z=sourceZones350_[i];if(!z.constructed||!z.loaded)return stop(pc);
        sourceSearchExclusionCount_=z.exclusionCount124;
        if(*sourceSearchExclusionCount_<=0){at(0x272af8);++*sourceSearchNodeS0_;continue;}
        at(0x272afc);sourceSearchExclusionArg_=z.exclusions120.get();
        at(0x27f518);if(!v.pointer3C||*v.pointer3C!=v.owner->exclusions3C.get()||v.owner->exclusions3C)return stop(pc);
        //This genuine fresh constructor has no prior3C owner to free at27F528.
        if(*sourceSearchExclusionCount_>128)return stop(0x27f530);
        const auto size=static_cast<std::uint32_t>(*sourceSearchExclusionCount_);
        at(0x27f538);v.exclusionAttempted=true;v.owner->exclusionCount40=size; //JAL delay before allocation.
        at(0x27f534);v.owner->exclusions3C.reset(new(std::nothrow) std::uint32_t[size]);
        at(0x27f540);v.pointer3C=v.owner->exclusions3C.get();v.exclusionAllocationReturned=true;
        at(0x27f548);sourceSearchHelperA2_=0;at(0x27f54c);sourceSearchHelperA3_=0;sourceSearchHelperSourceOffset_=0;
        for(std::uint32_t j=0;j<size;++j){
            at(0x27f550); //Actual nullable destination pointer load does not dereference it.
            at(0x27f554);if(!sourceSearchExclusionArg_||z.exclusions120.get()!=sourceSearchExclusionArg_||j>=z.exclusionsProduced)return stop(pc);
            sourceSearchLoadedWord_=std::bit_cast<std::uint32_t>(sourceSearchExclusionArg_[j]);
            at(0x27f558);++*sourceSearchHelperA2_;
            at(0x27f564);*sourceSearchHelperSourceOffset_+=4;
            at(0x27f568);*sourceSearchHelperA3_+=4;sourceSearchStorePending_=true;
            at(0x27f570);if(!v.owner->exclusions3C)return stop(pc);
            v.owner->exclusions3C[j]=*sourceSearchLoadedWord_;++v.exclusionProduced;sourceSearchStorePending_=false;
        }
        at(0x27f58c);v.exclusionReturned=true;
        at(0x272b10);++*sourceSearchNodeS0_;
    }
    sourceSearchNodesPhase_=SourceSearchNodesPhase::returned;at(0x272b28);return true; //Before actual portal358 load.
}
}

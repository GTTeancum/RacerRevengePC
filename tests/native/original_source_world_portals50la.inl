#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldPortalsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourcePortalsPhase_==SourcePortalsPhase::unreached)
        return !nativePortalParameters3A0_||!nativePortalParameters3A0_->sourceMode;
    if(!nativePortalParameters3A0_)return !sourcePortal3A0_;
    const auto& role=*nativePortalParameters3A0_;
    if(!role.sourceMode||!role.sourcePublished||role.rows||role.count||role.retired||
       role.sourceApplication!=worldApplication_.get()||!worldApplication_||
       role.sourceGeneration!=worldEntity_.sessionGeneration||!role.sourceGeneration||
       role.sourceThread!=std::this_thread::get_id()||!sourcePortal3A0_||
       *sourcePortal3A0_!=role.sourceArray.get()||sourcePortalsLoaded_>10||
       sourcePortalsConstructed_!=((role.sourceArray||role.sourceReleased)?10u:0u)||
       (sourcePortalsLoaded_&&!role.sourceArray&&!role.sourceReleased)||topologyAttempted_||nativePortalNext_||
       nativeMethods4C8_||nativeMethods4CC_||nativeMethods4D0_||nativeMethods4D4_||
       nativeMethodRowsInitialized_||nativeMethodZoneNext_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    if(role.sourceReleased&&sourcePortalGraphPhase_==SourcePortalGraphPhase::unreached)return false;
    if(sourcePortalGraphPhase_!=SourcePortalGraphPhase::unreached)return preflightSourceWorldPortalGraphNative();
    for(const auto& p:nativePortals_)if(p)return false;
    return true;
}
inline bool Owner::abortSourceWorldPortalsNative(std::uint32_t& pc)noexcept {
    pc=0;if(sourcePortalGraphPhase_!=SourcePortalGraphPhase::unreached&&sourcePortalGraphPhase_!=SourcePortalGraphPhase::nativeClosed)return false;
    if(!preflightSourceWorldPortalsNative())return false;
    if(sourcePortalsPhase_==SourcePortalsPhase::unreached)return true; //Legacy role unaffected.
    //Native pointer-knowledge withdrawal before array backing release. Actual
    //2E8990 destructor callback is NULL; no guest element destructor/count.
    sourcePortal3A0_.reset();nativePortalParameters3A0_.reset();
    sourcePortalsPhase_=SourcePortalsPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldPortals270BF8(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourcePortalsPhase_!=SourcePortalsPhase::unreached||!sourceWorldPortalsInvocationCurrent()||
       !sourceWorldConfig_||nativePortalParameters3A0_||!sourcePortal3A0_||*sourcePortal3A0_||
       !worldApplication_||!worldEntity_.sessionGeneration||!worldPortals358_||*worldPortals358_!=10||
       topologyAttempted_||nativePortalNext_||nativeMethods4C8_||nativeMethods4CC_||
       nativeMethods4D0_||nativeMethods4D4_||nativeMethodRowsInitialized_||nativeMethodZoneNext_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    for(const auto& p:nativePortals_)if(p)return false;
    std::array<std::uint32_t,4> origin,unusedForward;
    //Explicit native input admission before source interval: same actual
    //Application immutable global vectors; no guessed zero or warm owner.
    if(!copySourceWorldStartDefaults(origin,unusedForward))return false;
    nativePortalParameters3A0_.emplace(); //Native wrapper only, NOT publication3A0.
    auto& role=*nativePortalParameters3A0_;role.sourceMode=true;
    role.sourceApplication=worldApplication_.get();role.sourceGeneration=worldEntity_.sessionGeneration;
    role.sourceThread=std::this_thread::get_id();role.sourceOrigin=origin;
    sourcePortalsPhase_=SourcePortalsPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourcePortalsPC_=at;sourcePortalsPhase_=SourcePortalsPhase::bounded;return false;};
    pc=0x270bfc; //Actual delay-slot read of same World358=10.
    pc=0x270c54; //Actual count*0x50+0x10 array-cookie allocation, typed layout differs.
    try{role.sourceArray.reset(new SourcePortalParameter276590[10]);}catch(...){role.sourceArray.reset();}
    sourcePortalsAllocationReturned_=true;
    if(role.sourceArray)for(std::size_t i=0;i<10;++i){auto& row=role.sourceArray[i];
        pc=0x2765a4;row.vtable48=0x3d2b80;
        //2E8860 callback1048E0 DOES store full VUvf0 before later global copies.
        for(unsigned k=0;k<4;++k){pc=0x1048e0;row.vertices00[k]={{0,0,0,0x3f800000u}};}
        for(unsigned k=0;k<4;++k){pc=0x2765dc;row.vertices00[k]=role.sourceOrigin;}
        pc=0x2765e8;row.connect40=0;pc=0x2765ec;row.connect44=0;
        ++sourcePortalsConstructed_;
    }
    pc=0x270c78;sourcePortal3A0_=role.sourceArray.get();role.sourcePublished=true;
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;
    for(std::size_t i=0;i<10;++i){pc=0x270c9c;if(!role.sourceArray)return stop(pc);
        auto& row=role.sourceArray[i];bool returned=false;
        pc=0x27663c;if(!config.selectSourcePortal157D00(static_cast<std::uint32_t>(i),returned,d))
            return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
        //Source ignores selection v0. Exact all10 RA sections exist; if a real
        //missing selection occurs, retain prior Config and stop BEFORE getter,
        //not a fabricated loader0/row skip or generic previous-block authority.
        if(!returned)return stop(0x276658);
        auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
            if(!config.readSourcePortalField158240(key,value,found,copy,d)){
                pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
        source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
        static constexpr const char* keys[]={"vertex1","vertex2","vertex3","vertex4"};
        static constexpr std::uint32_t calls[]={0x276658,0x276674,0x276690,0x2766ac};
        for(unsigned k=0;k<4;++k){pc=calls[k];
            if(!read.vector(keys[k],role.sourceOrigin,&row.vertices00[k],returned))return stop(pc);}
        pc=0x2766c4;if(!read.integer("connect1",0,&row.connect40,returned))return stop(pc);
        pc=0x2766dc;if(!read.integer("connect2",0,&row.connect44,returned))return stop(pc);
        sourcePortalsLoaded_=i+1; //Actual2766F0 return1; no runtimePortal230450 grant.
    }
    sourcePortalsPhase_=SourcePortalsPhase::returned;pc=sourcePortalsPC_=0x270cc4;return true;
    //Before actual next physics3D4 allocation.
}
} //namespace ra_cold_aggregate50LA

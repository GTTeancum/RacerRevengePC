#pragma once
#include "original_source_world_light_read50la.inl"
namespace source_world_zone50LA {
// Exact stack-only29FF50/29FFA0 token custody. No strtok process-global state.
struct Tokens {
    char* token00=nullptr;char* next04=nullptr;
    explicit Tokens(char* text)noexcept:next04(text&&*text?text:nullptr){}
    char* next()noexcept {
        if(!next04)return nullptr;token00=next04;char* p=next04;
        while(*p&&*p!=','&&*p!='\n')++p;
        if(!*p)next04=nullptr;
        else{*p++=0;while(*p==','||*p=='\n')++p;next04=*p?p:nullptr;}
        return token00;
    }
};
}
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldZonesNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceZonesPhase_==SourceZonesPhase::unreached)return !sourceZones350_;
    if(!sourceZones350_)return !worldZoneInfo350_||*worldZoneInfo350_==nullptr;
    if(sourceZonesApplication_!=worldApplication_.get()||!worldApplication_||
       sourceZonesGeneration_!=worldEntity_.sessionGeneration||!sourceZonesGeneration_||
       !sourceZonesPublished_||!worldZoneInfo350_||*worldZoneInfo350_!=sourceZones350_.get()||
       sourceZonesConstructed_!=10||sourceZonesLoaded_>10||topologyAttempted_||
       nativeZoneTopologyPrefix272A48_||nativeZoneExclusionsNext_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    for(std::size_t i=0;i<10;++i){const auto& z=sourceZones350_[i];
        if(!z.constructed||z.exclusionCount124<0||z.exclusionCount124>128||
           z.exclusionsProduced>static_cast<std::size_t>(z.exclusionCount124)||
           (z.exclusionsProduced&&!z.exclusions120)||
           (z.exclusions120&&!z.exclusionAllocationReturned))return false;}
    return true;
}
inline bool Owner::abortSourceWorldZonesNative(std::uint32_t& pc)noexcept {
    if(sourceSearchNodesPhase_!=SourceSearchNodesPhase::unreached&&sourceSearchNodesPhase_!=SourceSearchNodesPhase::nativeClosed)return false;
    pc=0;if(!preflightSourceWorldZonesNative())return false;
    if(sourceZonesPhase_==SourceZonesPhase::unreached)return true;
    //Native withdrawal, never a source NULL350/reset/destructor receipt.
    worldZoneInfo350_.reset();
    if(sourceZones350_)for(std::size_t i=sourceZonesConstructed_;i>0;--i)
        sourceZones350_[i-1].exclusions120.reset();
    sourceZones350_.reset();sourceZonesPhase_=SourceZonesPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldZones270B58(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceZonesPhase_!=SourceZonesPhase::unreached||!sourceWorldZonesInvocationCurrent()||
       !sourceWorldConfig_||sourceZones350_||!worldZoneInfo350_||*worldZoneInfo350_||
       !worldApplication_||!worldEntity_.sessionGeneration||!worldZones34C_||*worldZones34C_!=10||
       topologyAttempted_||nativeZoneTopologyPrefix272A48_||nativeZoneExclusionsNext_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    sourceZonesPhase_=SourceZonesPhase::entered;sourceZonesApplication_=worldApplication_.get();
    sourceZonesGeneration_=worldEntity_.sessionGeneration;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceZonesPC_=at;sourceZonesPhase_=SourceZonesPhase::bounded;return false;};
    pc=0x270b58; //Actual same World34C=10 read; positive branch only.
    pc=0x270b78; //Actual requested10*0x160+0x10 cookie bytes; typed native layout differs.
    try{sourceZones350_.reset(new SourceZone275C50[10]);}catch(...){sourceZones350_.reset();}
    sourceZonesArrayReturned_=true;
    if(sourceZones350_)for(std::size_t i=0;i<10;++i){auto& z=sourceZones350_[i];
        pc=0x275c6c;z.vtable15C=0x3d2b90;
        pc=0x27844c;z.fogVtable144=0x3d2aa0;
        pc=0x278450;z.fogColor12C.fill(0);
        pc=0x27845c;z.fogVisibility13C=0x3f800000;
        pc=0x278464;z.fogHaziness140=0x3f000000;
        pc=0x275c84;z.index00=0;
        //Actual immediate260!=4 skips8B7C/442FF0 assertion entirely.
        pc=0x275d20;std::memcpy(z.name04.data(),"StaticModels",12);
        pc=0x275d2c;z.name04[12]=0;z.nameKnown=13;
        pc=0x275d34;z.staticCount108=0;
        pc=0x275d38;z.search110[0]=2;pc=0x275d3c;z.search110[2]=2;
        pc=0x275d40;z.search110[1]=2;pc=0x275d44;z.search110[3]=2;
        pc=0x275d48;z.exclusions120.reset();pc=0x275d4c;z.exclusionCount124=0;
        pc=0x275d50;z.reverb128=0;
        pc=0x275d6c;z.flags148=1;z.knownFlags148=1;
        pc=0x275d8c;z.flags148|=2;z.knownFlags148|=2;
        pc=0x275d9c;z.flags148|=4;z.knownFlags148|=4;
        z.constructed=true;++sourceZonesConstructed_;
    }
    pc=0x270ba0;worldZoneInfo350_=sourceZones350_.get();sourceZonesPublished_=true;
    for(std::size_t i=0;i<10;++i){pc=0x270bd0;if(!sourceZones350_)return stop(pc);
        if(!loadSourceWorldZone275E30(i,pc))return stop(pc);
        sourceZonesLoaded_=i+1; //Original loader return value ignored, not a render-ready grant.
    }
    sourceZonesPhase_=SourceZonesPhase::returned;pc=sourceZonesPC_=0x270bf8;return true;
    //STOP before270BFC delay-slot read of actual portalcount358.
}
} //namespace ra_cold_aggregate50LA

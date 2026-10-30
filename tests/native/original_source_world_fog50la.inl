#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldFogNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceFogPhase_==SourceFogPhase::unreached)
        return !nativeFogDescriptor3AC_||!nativeFogDescriptor3AC_->sourceMode;
    if(!nativeFogDescriptor3AC_)return !sourceFog3AC_||*sourceFog3AC_==nullptr;
    const auto& f=*nativeFogDescriptor3AC_;
    return f.sourceMode&&f.sourceConstructed&&f.sourcePublished&&
        f.sourceApplication==worldApplication_.get()&&worldApplication_&&
        f.sourceGeneration==worldEntity_.sessionGeneration&&f.sourceGeneration&&
        f.sourceThread==std::this_thread::get_id()&&!f.source&&!f.block&&
        sourceFog3AC_&&*sourceFog3AC_==&f&&
        !runtimeCameraBorrows_&&!runtimeWorldBorrows_&&!constructorRacerWorldBorrows_&&
        !nativeFog3EC_&&!nativeFogOff3F4_&&!nativeFogOn3F0_;
}
inline bool Owner::abortSourceWorldFogNative(std::uint32_t& pc)noexcept {
    pc=0;if(sourceFogTransferBorrowers_||(sourceFogTransfer_&&!sourceFogTransfer_->nativeCancelled)||
        !preflightSourceWorldFogNative())return false;
    if(sourceFogPhase_==SourceFogPhase::unreached)return true; //Legacy backing untouched.
    //No Entity, external descriptor borrower or dynamic suballocation exists.
    //Withdraw source-pointer knowledge before freeing, NOT270B54 NULLstore.
    sourceFog3AC_.reset();nativeFogDescriptor3AC_.reset();
    sourceFogPhase_=SourceFogPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldFog270B14(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceFogPhase_!=SourceFogPhase::unreached||!sourceWorldFogInvocationCurrent()||
       !sourceWorldConfig_||nativeFogDescriptor3AC_||!sourceFog3AC_||*sourceFog3AC_||
       !worldApplication_||!worldEntity_.sessionGeneration||
       nativeFogLoadAttempted_||nativeFogLoadComplete_||nativeFogAttempted_||nativeFogRetired_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||
       nativeFog3EC_||nativeFogOff3F4_||nativeFogOn3F0_)return false;
    sourceFogPhase_=SourceFogPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept {
        pc=sourceFogPC_=at;sourceFogPhase_=SourceFogPhase::bounded;return false;};
    pc=0x270b14; //Actual requested source size0x1C; typed native layout differs.
    try{nativeFogDescriptor3AC_.reset(new NativeFogDescriptor3AC);}catch(...){nativeFogDescriptor3AC_.reset();}
    if(nativeFogDescriptor3AC_){auto& f=*nativeFogDescriptor3AC_;
        f.sourceMode=true;f.sourceApplication=worldApplication_.get();
        f.sourceGeneration=worldEntity_.sessionGeneration;f.sourceThread=std::this_thread::get_id();
        pc=0x27844c;f.sourceVtable18=0x3d2aa0;
        pc=0x278450;f.sourceColor00.fill(0); //Actual memset of full16bytes.
        pc=0x27845c;f.sourceVisibility10=0x3f800000;
        pc=0x278464;f.sourceHaziness14=0x3f000000;
        f.sourceConstructed=true;
    }
    pc=0x270b2c;sourceFog3AC_=nativeFogDescriptor3AC_.get();
    if(!nativeFogDescriptor3AC_)return stop(0x270b34); //First NULL vtable18 read.
    auto& f=*nativeFogDescriptor3AC_;f.sourcePublished=true;
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x278590;if(!config.selectSourceFog157D00(returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned){
        //Actual source loader0 returns before field getters. Caller frees the
        //plain0x1C payload directly (no virtual destructor, no Entity count).
        if(!preflightSourceWorldFogNative())return stop(0x270b4c);
        pc=0x270b4c;nativeFogDescriptor3AC_.reset();
        pc=0x270b54;sourceFog3AC_=nullptr;
        sourceFogPhase_=SourceFogPhase::returnedZero;pc=sourceFogPC_=0x270b58;return true;
    }
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept {
        if(!config.readSourceFogField158240(key,value,found,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    std::array<char,256> text;pc=0x2785c0;
    if(!read.text("colorrgb","undef",text.data(),text.size(),returned))return stop(pc);
    std::array<std::uint32_t,4> rgba;pc=0x2785cc;if(!read.color(text.data(),rgba))return stop(pc);
    static constexpr std::uint32_t stores[]={0x2785e8,0x2785f8,0x278600,0x27860c};
    for(unsigned i=0;i<4;++i){pc=stores[i];f.sourceColor00[i]=rgba[i];}
    //The finalcolor store is the delay slot of the Visibility getter call.
    //Original302E10→30AB18→309C18 accepts leading '.' with no integer digits.
    //Only the two actual Fog lexemes are lowered via equivalent rationals;
    //no generic parser policy or host floating arithmetic is introduced.
    auto number=[&](const char* key,std::string_view exact,const char* rational,
        std::uint32_t fallback,std::uint32_t& output)noexcept {
        std::string_view value;bool found=false;std::uint32_t copy=0;
        if(!fetch(key,value,found,copy))return false;
        if(found){pc=0x158bac;if(value!=exact)return false;
            std::uint32_t bits;if(!source_world_light50LA::Reader<decltype(fetch)>::numberToken(rational,bits))return false;
            pc=0x158bc4;output=bits;
        }else{pc=0x158bdc;output=fallback;}
        pc=0x158be4;return true;
    };
    pc=0x278608;if(!number("visibility",".9","0.9",0x3f800000,f.sourceVisibility10))return stop(pc);
    pc=0x278624;if(!number("haziness",".5","0.5",0x3f000000,f.sourceHaziness14))return stop(pc);
    sourceFogPhase_=SourceFogPhase::returnedOne;pc=sourceFogPC_=0x270b58;return true;
}
} //namespace ra_cold_aggregate50LA

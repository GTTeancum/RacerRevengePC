#pragma once
#include "original_source_world_light_read50la.inl"
#include "../../src/game/original_smoke_numeric50rc.h"
#include <cfenv>
#include <new>
namespace ra_cold_aggregate50LA {
inline bool Owner::loadSourceWorldSunFlare1B46E0(std::uint32_t& pc)noexcept {
    pc=0;std::array<std::uint32_t,4> origin,forward,up;
    if(sourceSunFlarePhase_!=SourceSunFlarePhase::unreached||!sourceWorldSunFlareInvocationCurrent()||
       nativeSunFlares_||!nativeSunFlarePointers4A4_||*nativeSunFlarePointers4A4_||
       !worldZones34C_||*worldZones34C_!=10||!copySourceWorldStartDefaults(origin,forward)||
       !copySourceWorldUpDefault(up)||!reserveSourceSunFlareNative())return false;
    nativeSunFlares_.emplace();auto& domain=*nativeSunFlares_;
    domain.sourceMode=true;domain.sourceThread=nativeScbThread_;domain.sourceApplication=worldApplication_.get();domain.generation=worldEntity_.sessionGeneration;
    sourceSunFlareStack_.emplace();auto& stack=*sourceSunFlareStack_;
    sourceSunFlarePhase_=SourceSunFlarePhase::nativeAdmitted;
    auto stop=[&](std::uint32_t at){pc=sourceSunFlarePC_=at;sourceSunFlarePhase_=SourceSunFlarePhase::bounded;return false;};
    auto& config=*sourceWorldObjectConfig_;a::OriginalStandardTextureDiagnostic50LA d;
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy){
        if(!config.readSourceSunFlareField158240(key,value,found,copy,d)){
            pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;
    };
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    //Direct optional source output stores, with no fallible native work after
    //the actual integer/default conversion and before its reached store.
    auto getInteger=[&](const char* key,std::int32_t fallback,std::optional<std::int32_t>& output){
        std::string_view text;bool found=false;std::uint32_t copy=0;
        if(!fetch(key,text,found,copy))return false;
        std::int32_t value=fallback;
        if(found){pc=0x158a34;if(!decltype(read)::integerToken(text,value))return false;
            pc=text=="T"?0x158968:text=="F"?0x158990:0x158a44;}
        else pc=0x158b60;
        output=value;return true;
    };
    auto getFloat=[&](const char* key,std::optional<std::uint32_t>& output){
        std::string_view text;bool found=false;std::uint32_t copy=0;
        if(!fetch(key,text,found,copy))return false;
        if(!found){pc=0x158ab4;return true;} //Actual158A70 returns0, does NOT store fallback.
        pc=0x158a94;std::uint32_t value;if(!decltype(read)::numberToken(text,value))return false;
        pc=0x158aac;output=value;return true;
    };
    sourceSunFlarePhase_=SourceSunFlarePhase::entered;
    pc=0x1b46e8;sourceSunFlareModeA1_=1;
    pc=0x1b46f8;std::uint8_t profileByte=0;
    //Read the SAME current shared byte anew, not a snapshot/replayed1B44E8.
    if(!sourceLoadApplication_->profile||sourceLoadRace_->profile_!=sourceLoadApplication_->riderProfile||
       !sourceLoadApplication_->riderProfile||!sourceLoadApplication_->riderProfile->readWorldProfileByte1B44E8(
            *sourceLoadApplication_->profile,profileByte))return stop(pc);
    sourceSunFlareProfileByte_=profileByte;
    if(((profileByte>>5u)&3u)==1u){pc=0x1b470c;sourceSunFlareModeA1_=0;}
    pc=0x271df0;stack.saturation4A8=0;
    pc=0x271dfc;stack.direction400=std::array<std::uint32_t,4>{{0,0,0,0x3f800000}};
    pc=0x271e04;stack.position410=std::array<std::uint32_t,4>{{0,0,0,0x3f800000}};
    pc=0x271e0c;stack.target420=std::array<std::uint32_t,4>{{0,0,0,0x3f800000}};
    pc=0x271e24;stack.down430=std::array<std::uint32_t,4>{{0,0xbf800000,0,0x3f800000}};
    bool returned=false;pc=0x271e30;
    if(!config.selectSourceSunFlareSection157D00(0,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x271e48); //Ignored return, unsupported retained-block getter.
    pc=0x271e48;if(!getInteger("numberofsunflares",0,stack.count4B4))return stop(pc);
    if(!stack.count4B4||*stack.count4B4!=1)return stop(0x271e54);
    pc=0x271e68;domain.count4A8=1;
    pc=0x271e6c;sourceSunFlareLimit_=1;
    pc=0x271e70;sourceSunFlareIndex_=0;
    pc=0x271e74;domain.pointerAllocationAttempted=true;
    domain.pointerBacking.reset(new(std::nothrow) NativeSunFlare*[1]); //Actual slot UNKNOWN.
    pc=0x271e80;nativeSunFlarePointers4A4_=domain.pointerBacking.get();domain.sourcePointerPublished=true;
    pc=0x271ecc;returned=false;
    if(!config.selectSourceSunFlareSection157D00(1,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x271ed4); //Actual skip leaves unknown slot; cohort stops before loop.
    bool found=false;std::string_view value;std::uint32_t copy=0;
    pc=0x271eec;if(!fetch("texturemap",value,found,copy))return stop(pc);
    if(!found)return stop(0x271ef4);
    pc=copy;if(value.size()>=stack.texture70.size())return stop(pc);
    std::memcpy(stack.texture70.data(),value.data(),value.size());stack.texture70[value.size()]=0;stack.textureKnown=true;
    pc=0x271f0c;if(!read.text("retrievematerial","flarezretrieve",stack.retrieve180.data(),stack.retrieve180.size(),found))return stop(pc);
    stack.retrieveKnown=true;
    pc=0x271f20;if(!fetch("saturation",value,found,copy))return stop(pc);
    if(found){
        //Exact authored lowercase t follows genuine158920 case-insensitive
        //boolean path. No shared parser policy is broadened here.
        if(value!="t")return stop(0x158920);
        pc=0x158968;stack.saturation4A8=1;
    }
    pc=0x271f34;if(!getFloat("minscale",stack.min4AC))return stop(pc);
    pc=0x271f48;if(!getFloat("maxscale",stack.max4B0))return stop(pc);
    pc=0x271f60;if(!getInteger("width",128,stack.width4B8))return stop(pc);
    pc=0x271f78;if(!getInteger("height",128,stack.height4BC))return stop(pc);
    pc=0x271f98;if(!read.text("vertexcolor","128,128,128",stack.color310.data(),stack.color310.size(),found))return stop(pc);
    stack.colorTextKnown=true;std::array<std::uint32_t,4> color;
    pc=0x271fa4;if(!read.color(stack.color310.data(),color))return stop(pc);
    pc=0x271fd0;stack.color3F0=color;
    pc=0x271fe0;if(!read.vector("lightdirection",origin,&*stack.direction400,found))return stop(pc);
    if(!found){
        pc=0x272004;if(!read.vector("position",up,&*stack.position410,found))return stop(pc);
        pc=0x272020;if(!read.vector("targetposition",origin,&*stack.target420,found))return stop(pc);
        //Explicit existing native finite/nearest adaptation, NOT EE-bit-exact.
        //Compute the first two SUB.S before first store, as real instructions.
        if(std::fegetround()!=FE_TONEAREST)return stop(0x272044);
        g::original_smoke_numeric_detail50RC::Math m;
        auto f=[](std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;};
        auto bits=[](float value){std::uint32_t result;std::memcpy(&result,&value,4);return result;};
        const float x=m.sub(f((*stack.target420)[0]),f((*stack.position410)[0]),0x272044);
        if(m.badPC)return stop(m.badPC);
        const float y=m.sub(f((*stack.target420)[1]),f((*stack.position410)[1]),0x272048);
        if(m.badPC)return stop(m.badPC);
        pc=0x27204c;stack.difference450[0]=bits(x);
        const float z=m.sub(f((*stack.target420)[2]),f((*stack.position410)[2]),0x272050);
        if(m.badPC)return stop(m.badPC);
        pc=0x272054;stack.difference450[1]=bits(y);
        pc=0x272058;stack.difference450[2]=bits(z);
        pc=0x27205c;stack.difference450[3]=0x3f800000;
        pc=0x272064;stack.direction400=std::array<std::uint32_t,4>{{*stack.difference450[0],*stack.difference450[1],*stack.difference450[2],*stack.difference450[3]}};
    }
    if(!constructSourceSunFlare2C5730(pc))return stop(pc);
    pc=0x2720a0;stack.callMode0=*sourceSunFlareModeA1_;
    //Actual argument loads precede direct2C5A10. An unknown required scalar
    //would stop at its first load, not be replaced with a native default.
    if(!stack.min4AC)return stop(0x2720a8);
    if(!stack.max4B0)return stop(0x2720ac);
    if(!stack.width4B8)return stop(0x2720b0);
    if(!stack.height4BC)return stop(0x2720b4);
    pc=0x2c5a68;stack.calleeColor80=stack.color3F0;
    pc=0x2c5a6c;if(!domain.owner)return stop(pc);
    auto& o=*domain.owner;o.zones30=*worldZones34C_;o.sourceZonesStored=true;
    pc=0x2c5a80;if(std::fegetround()!=FE_TONEAREST)return stop(pc);
    g::original_smoke_numeric_detail50RC::Math m;
    auto f=[](std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;};
    const std::array<float,3> input{{f((*stack.direction400)[0]),f((*stack.direction400)[1]),f((*stack.direction400)[2])}};
    const auto normal=g::original_smoke_numeric_detail50RC::normalize13E3F0(input,m);
    if(m.badPC)return stop(m.badPC);
    pc=0x2c5a94;o.direction50={{normal[0],normal[1],normal[2],1.f}};o.sourceDirectionStored=true;
    pc=0x2c5a98;o.minScale34=f(*stack.min4AC);o.sourceMinStored=true;
    pc=0x2c5a9c;o.maxScale38=f(*stack.max4B0);o.sourceMaxStored=true;
    if(!constructSourceSunFlareSprite2E5E60(*stack.callMode0,pc))return stop(pc);
    sourceSunFlarePhase_=SourceSunFlarePhase::materialPrefix;sourceSunFlarePC_=pc=0x2c5ae0;
    return true; //BEFORE first real material load; no vC0/vA4 return claim.
}
inline bool Owner::preflightSourceWorldSunFlareNative()const noexcept {
    if(sourceSunFlarePhase_==SourceSunFlarePhase::unreached)return true;
    if(nativeScbThread_!=std::this_thread::get_id()||!nativeSunFlares_||!nativeSunFlares_->sourceMode)return false;
    return canRetireSourceSunFlareNative();
}
inline bool Owner::abortSourceWorldSunFlareNative(std::uint32_t& pc)noexcept {
    pc=sourceSunFlarePC_;if(sourceSunFlarePhase_==SourceSunFlarePhase::unreached)return true;
    if(!preflightSourceWorldSunFlareNative()||!retireSourceSunFlareNative())return false;
    sourceSunFlarePhase_=SourceSunFlarePhase::nativeClosed;return true;
}
}

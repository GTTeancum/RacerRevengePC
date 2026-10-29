#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::loadSourceWorldLights270AA8(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceLightsPhase_!=SourceLightsPhase::unreached||!sourceWorldLightsInvocationCurrent()||
       !sourceWorldConfig_||!worldZones34C_||*worldZones34C_!=10u)return false;
    sourceLightsPhase_=SourceLightsPhase::entered;
    const auto stop=[&](std::uint32_t p)noexcept{pc=sourceLightsPC_=p;sourceLightsPhase_=SourceLightsPhase::bounded;return false;};
    if(!allocateSourceLightList270AA8(pc))return stop(pc);
    pc=sourceLightsPC_=0x270ac4;sourceLight3A8_=nativeLightList3A8_.get();
    if(!nativeLightList3A8_){sourceLightsNullPublished_=true;return stop(0x270acc);}
    auto& l=*nativeLightList3A8_;l.sourcePublished=true;
    pc=0x27825c;l.zoneCount34=*worldZones34C_; //ActualWorld34C is10; parentpredicate proves priorgetter.
    pc=0x278268;try{l.colors3C.reset(new NativeLightList3A8::Color[10]);}catch(...){l.colors3C.reset();}
    pc=0x278270;l.sourceZoneAllocationReturned=true;
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x2782b8;if(!config.selectSourceLights157D00(returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned){if(!retireSourceLightList142590(pc))return stop(pc);
        pc=0x270b0c;sourceLight3A8_=nullptr;sourceLightsPhase_=SourceLightsPhase::returnedZero;
        pc=sourceLightsPC_=0x270b14;return true;}
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
        if(!config.readSourceLightField158240(key,value,found,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    std::int32_t count=0;pc=0x2782e0;
    if(!read.integer("numberoflights",0,&count,returned))return stop(pc);
    l.count30=static_cast<std::uint32_t>(count);
    if(count!=185)return stop(0x2782e8);
    pc=0x2782f8;try{l.wrappers38.reset(new NativeLightWrapper277C10[185]);}catch(...){l.wrappers38.reset();}
    l.sourceArrayAllocationReturned=true;
    if(l.wrappers38){for(std::size_t i=0;i<185;++i){auto& w=l.wrappers38[i];
        pc=0x277c18; //Exactwrapper semantic vtable3D2B30.
        pc=0x277c1c;w.live00=nullptr;pc=0x277c20;w.sourceDescriptor04.reset();
        w.sourceConstructed=true;++l.sourceWrappersConstructed;}}
    pc=0x278320; //Actual38publication after complete cookie-array constructors.
    static constexpr const char* keys[]={"directionallightcolorforzone1","directionallightcolorforzone2",
        "directionallightcolorforzone3","directionallightcolorforzone4","directionallightcolorforzone5",
        "directionallightcolorforzone6","directionallightcolorforzone7","directionallightcolorforzone8",
        "directionallightcolorforzone9","directionallightcolorforzone10"};
    for(std::size_t i=0;i<10;++i){std::array<char,64> text;pc=0x278360;
        if(!read.text(keys[i],"undef",text.data(),text.size(),returned))return stop(pc);
        if(returned){std::array<std::uint32_t,4> rgba;pc=0x278374;if(!read.color(text.data(),rgba))return stop(pc);
            static constexpr std::uint32_t stores[]={0x278388,0x278390,0x278398,0x2783a4};
            for(unsigned k=0;k<4;++k){pc=stores[k];if(!l.colors3C)return stop(pc);
                float actual;std::memcpy(&actual,&rgba[k],4);l.colors3C[i].lanes[k]=actual;}}
        else{pc=0x2783b4;if(!l.colors3C)return stop(pc);l.colors3C[i].lanes[0]=-256.f;}
        l.colorsLoaded=i+1;
    }
    for(std::size_t i=0;i<185;++i){pc=0x2783e8;if(!l.wrappers38)return stop(pc);
        if(!loadSourceLightDescriptor277390(i,pc))return stop(pc);
        //Original2783F0/277D5C return values are ignored; this bool means only
        //the exact typed call returned, not an invented descriptor readiness.
        l.loaded=i+1;
    }
    sourceLightsPhase_=SourceLightsPhase::returnedOne;pc=sourceLightsPC_=0x270b14;return true;
}
inline bool Owner::abortSourceWorldLightsNative(std::uint32_t& pc)noexcept {
    pc=0;if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceLightsPhase_==SourceLightsPhase::nativeClosed)return !nativeLightList3A8_;
    if(sourceLightsPhase_==SourceLightsPhase::unreached)return !nativeLightList3A8_||!nativeLightList3A8_->sourceMode;
    if(!abortSourceLightListNative(pc))return false;
    sourceLightsPhase_=SourceLightsPhase::nativeClosed;return true;
}
} //namespace ra_cold_aggregate50LA

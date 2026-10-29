#pragma once
#include "../../platform/linux/native/original_ui_garage_scene_setup50kg.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldInstanceMasterNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceInstancePhase_==SourceInstancePhase::unreached)
        return !nativeEmptyInstances3DC_||!nativeEmptyInstances3DC_->sourceMode;
    if(!nativeEmptyInstances3DC_)return sourceInstancePhase_==SourceInstancePhase::nativeClosed;
    return nativeEmptyInstances3DC_->sourceMode&&
        !runtimeCameraBorrows_&&!runtimeWorldBorrows_&&!constructorRacerWorldBorrows_&&
        canRetireNativeInstanceMaster();
}
inline bool Owner::abortSourceWorldInstanceMasterNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldInstanceMasterNative())return false;
    if(sourceInstancePhase_==SourceInstancePhase::unreached)return true;
    //Existing same-domain cleanup releases only OUR exact current-global
    //backing, or an unpublished attempt. If displaced, Application history
    //retains its backing; neither restore prior globals nor free a foreign one.
    //This is native withdrawal, not a claimed2BFD40 source destructor return.
    if(!retireNativeInstanceMaster())return false;
    sourceInstancePhase_=SourceInstancePhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldInstanceMaster270FD0(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceInstancePhase_!=SourceInstancePhase::unreached||!sourceWorldInstanceMasterInvocationCurrent()||
       !sourceLoadApplication_||!sourceLoadApplication_->state||!sourceWorldConfig_||nativeEmptyInstances3DC_||
       !nativeInstanceDescriptors3DC_||*nativeInstanceDescriptors3DC_||
       !worldApplication_||!worldEntity_.sessionGeneration||!worldEntity_.reference00||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    using Master=rr::platform::OriginalUiGarageInstanceMaster50KG;
    struct NativeAttempt final {std::unique_ptr<Master> actual;};
    auto application=sourceLoadApplication_->state;
    const auto previous=application->instanceMaster444078;
    std::shared_ptr<NativeAttempt> attempt;
    //Native preadmission: control block and displaced-owner history capacity.
    //Refusal has PC0 and is NOT a guest mallocNULL. No source effects yet.
    try{
        attempt=std::make_shared<NativeAttempt>();
        if(previous){
            if(application->overwrittenInstanceMasters.size()==application->overwrittenInstanceMasters.max_size())return false;
            application->overwrittenInstanceMasters.reserve(application->overwrittenInstanceMasters.size()+1);
        }
    }catch(...){return false;}
    nativeEmptyInstances3DC_.emplace();auto& role=*nativeEmptyInstances3DC_;
    role.sourceMode=true;role.application=application;role.sourceWorld=&worldEntity_;
    role.sourceGeneration=worldEntity_.sessionGeneration;role.sourceThread=std::this_thread::get_id();
    sourceInstancePhase_=SourceInstancePhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceInstancePC_=at;sourceInstancePhase_=SourceInstancePhase::bounded;return false;};
    pc=0x270fe8;
    //Actual nullable outer allocation. SourceTag executes2BFEA0; its inner
    //mallocNULL is a normal constructor prefix, NOT an outer allocation failure.
    try{attempt->actual.reset(new Master(Master::SourceTag{}));}catch(...){attempt->actual.reset();}
    role.sourceOuterReturned=true;
    if(attempt->actual){
        //Aliasing constructor allocates no control block after source effects.
        role.master=std::shared_ptr<Master>(attempt,attempt->actual.get());
        pc=0x2bfd24;
        if(application->instanceMaster444078!=previous)return stop(pc);
        if(previous)application->overwrittenInstanceMasters.push_back(previous); //Reserved; shared copy is noexcept.
        application->instanceMaster444078=role.master;role.published=true;role.sourceConstructorPublished=true;
        pc=0x2bfd28;role.master->count10_=0;role.master->sourceCount10Known_=true;
    }
    pc=0x271004;
    if(!role.master){
        //OuterNULL skips ctor/global2BFD24, but caller STILL writesNULL444078.
        if(application->instanceMaster444078!=previous)return stop(pc);
        if(previous)application->overwrittenInstanceMasters.push_back(previous);
    }else if(application->instanceMaster444078!=role.master)return stop(pc);
    application->instanceMaster444078=role.master;role.published=true;role.sourceCallerPublished=true;
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x271010;
    if(!config.selectSourceInstanceMasters157D00(returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    //Actual v0 ignored. Missing selection retains the live Portal10 block;
    //the next158B30 performs real lookup there before the default0 store.
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
        if(!config.readSourceInstanceMasterField158240(key,value,found,copy,d)){
            pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    pc=0x271028;
    //Use a separate native scratch only until the actual getter's destination
    //store; prior World3E0 remains untouched on any earlier failed lookup.
    std::int32_t count=0;
    if(!read.integer("numberofinstancemasters",0,&count,returned))return stop(pc);
    nativeInstanceCount3E0_=count;
    if(count!=0)return stop(0x271048); //No positive descriptor array/record claim.
    //Zero branch never allocates/writes3DC and never dereferences raw records00.
    role.complete=true;sourceInstancePhase_=SourceInstancePhase::returned;
    pc=sourceInstancePC_=0x270cfc;return true; //True v84 return, before v88.
}
} //namespace ra_cold_aggregate50LA

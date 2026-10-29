#pragma once
#include <cstdio>
namespace ra_cold_aggregate50LA {
inline bool Owner::closeSourceSceneConfig271C10(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||sourceSceneConfigCloseAttempted_||
       sourceTrackStep_!=SourceTrackStep::sceneInfoReturned||sourceTrackPC_!=0x1aa08c||
       sourceSceneInfoPhase_!=SourceSceneInfoPhase::returnedOne||
       !preflightSourceWorldControllersNative()||!preflightSourceWorldCollisionNative()||
       !preflightSourceWorldStaticModelsNative()||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||sourceLoadApplication_->entities()!=worldApplication_||
       !sourceWorldConfig_||!sourceWorldConfig_->source_||!sourceTrackFile_||
       sourceWorldConfig_->file_!=sourceTrackFile_||!sourceConfig34Published_||
       !worldParser34_||*worldParser34_!=sourceWorldConfig_->source_.get()||
       !sourceWorldConfig_->sourceWorldTailEnvironmentCurrent()||
       !sourceTrackFile_->current(*sourceWorldConfig_,sourceWorldConfig_->source_->file00))return false;
    sourceSceneConfigCloseAttempted_=true;pc=0x271c2c;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceWorldConfig_->close1573B0(d)){
        pc=d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_;return false;
    }
    pc=0x271c34;worldParser34_=nullptr;sourceSceneConfigCloseReturned_=true;
    //The source Config allocation is gone, but the exact wrapper remains the
    //historical Static/Collision/Environment argument identity. Withdraw its
    //now-closed native File occurrence BEFORE another Config uses this root.
    pc=0x1aa0a0;
    if(!sourceWorldConfig_->abortNative(d)){if(d.sourcePC)pc=d.sourcePC;return false;}
    sourceSceneConfigNativeWithdrawn_=true;
    sourceTrackStep_=SourceTrackStep::sceneConfigClosed;
    pc=sourceTrackPC_=0x1aa0a0;return true; //Actual v9C return; caller next dispatches vA0.
}
inline bool Owner::writeSourceObjectDiagnostic(bool missing,std::uint32_t& pc)noexcept {
    pc=missing?0x1b4400:0x1b4480;
    if(sourceObjectLogAttempted_)return false;
    std::array<char,1024> output;std::size_t size=0;
    const auto append=[&](const char* s)noexcept{while(*s){if(size==output.size())return false;output[size++]=*s++;}return true;};
    const auto actualString=[&](const auto& text)noexcept{
        for(const auto& b:text){if(!b)return false;if(!*b)return true;
            if(size==output.size())return false;output[size++]=static_cast<char>(*b);}return false;};
    if(missing){
        if(!sourceObjectToyPathKnown_||!append("Toy file "))return false;
        for(std::size_t i=0;i+1<sourceObjectToyPathKnown_;++i){if(size==output.size())return false;output[size++]=sourceObjectToyPath_[i];}
        if(!append(" NOT FOUND!!"))return false;
    }else if(!append("\n\n***** Begin Load for Scene Toys ")||!actualString(worldText244_)||
        !append(" (")||!actualString(worldText140_)||!append(".toy) *****\n\n"))return false;
    //Explicit native diagnostic adapter for source1336C0. One latched write,
    //retained short/error result, no source-host-I/O fiction, no flush/replay.
    sourceObjectLogAttempted_=true;sourceObjectLogMissing_=missing;sourceObjectLogRequested_=size;
    sourceObjectLogWritten_=std::fwrite(output.data(),1,size,stdout);
    sourceObjectLogNativeError_=sourceObjectLogWritten_!=size||std::ferror(stdout)!=0;return true;
}
inline bool Owner::loadSourceWorldObjects1B4330(std::uint32_t& pc)noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||sourceObjectPhase_!=SourceObjectPhase::unreached||
       sourceTrackStep_!=SourceTrackStep::sceneConfigClosed||sourceTrackPC_!=0x1aa0a0||
       !sourceSceneConfigCloseReturned_||!sourceSceneConfigNativeWithdrawn_||
       !sourceWorldConfig_||!sourceWorldConfig_->sourceClosed_||!sourceWorldConfig_->nativeClosed_||sourceWorldConfig_->source_||
       sourceWorldObjectConfig_||!worldParser34_||*worldParser34_||!sourceTrackFile_||sourceTrackFile_->occurrence_||
       sourceTrackFile_->handle_!=-1||sourceTrackFile_->nativeClosed_||!sourceTrackFile_->joined_||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!sourceLoadApplication_->riderProfile||
       sourceLoadRace_->profile_!=sourceLoadApplication_->riderProfile||
       !preflightSourceWorldControllersNative()||!preflightSourceWorldCollisionNative()||
       !preflightSourceWorldStaticModelsNative())return false;
    const auto exact=[](const auto& source,const char* text)noexcept{
        for(std::size_t i=0;i<source.size();++i){if(!source[i]||*source[i]!=std::uint8_t(text[i]))return false;
            if(!text[i])return true;}return false;};
    if(!exact(worldText3C_,"tracks\\")||!exact(worldText140_,"ra"))return false;
    sourceObjectPhase_=SourceObjectPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceObjectPC_=at;sourceObjectPhase_=SourceObjectPhase::bounded;return false;};
    //Actual1B4358 formatter consumes the known sameWorld directory and stem.
    //Only logical bytes+NUL are produced; no source stack-tail zero fill.
    pc=0x1b4358;std::size_t at=0;
    for(const auto& byte:worldText3C_){if(!*byte)break;sourceObjectToyPath_[at++]=static_cast<char>(*byte);}
    for(const auto& byte:worldText140_){if(!*byte)break;sourceObjectToyPath_[at++]=static_cast<char>(*byte);}
    for(const char* s=".toy";*s;++s)sourceObjectToyPath_[at++]=*s;
    sourceObjectToyPath_[at]=0;sourceObjectToyPathKnown_=at+1;
    pc=0x1b4364;std::memcpy(sourceObjectBinaryPath_.data(),sourceObjectToyPath_.data(),sourceObjectToyPathKnown_);
    sourceObjectBinaryPathKnown_=sourceObjectToyPathKnown_;
    pc=0x1b4370;std::size_t dot=0;for(std::size_t i=0;i<at;++i)if(sourceObjectBinaryPath_[i]=='.')dot=i;
    if(dot!=9)return stop(0x1b4388); //Exact qualified tracks\\ra.toy last-dot branch.
    pc=0x1b4388;std::memcpy(sourceObjectBinaryPath_.data()+dot,".tob",5);
    //Native wrapper admission is distinct from actual nullable0x4D0 allocation.
    try{sourceWorldObjectConfig_.reset(new Config);}catch(...){return stop(0x1b43ac);}
    if(!sourceWorldObjectConfig_->bindNative(sourceTrackFile_))return stop(0x1b43ac);
    if(!sourceWorldObjectConfig_->allocate1B43AC())return stop(0x1b43ac);
    pc=0x1b43c4;worldParser34_=sourceWorldObjectConfig_->source_.get();sourceObjectConfig34Published_=true;
    sourceObjectPhase_=SourceObjectPhase::allocated;
    a::OriginalStandardTextureDiagnostic50LA d;
    pc=0x1b43d0;auto returned=sourceWorldObjectConfig_->open1577D0(sourceObjectBinaryPath_.data(),d);
    const auto ioPC=[&]()noexcept{return d.sourcePC?d.sourcePC:sourceWorldObjectConfig_->sourcePC_;};
    if(returned==Config::Return::bounded)return stop(ioPC());
    if(returned==Config::Return::zero){
        pc=0x1b43e8;returned=sourceWorldObjectConfig_->open1577D0(sourceObjectToyPath_.data(),d);
        if(returned==Config::Return::bounded)return stop(ioPC());
        if(returned==Config::Return::one)return stop(0x1b4424); //Actual443318 export-global branch unjoined.
        if(!writeSourceObjectDiagnostic(true,pc))return stop(pc);
        pc=0x1b440c;if(!sourceWorldObjectConfig_->close1573B0(d))return stop(ioPC());
        pc=0x1b4414;worldParser34_=nullptr;
        sourceObjectReturnValue_=0;sourceObjectPhase_=SourceObjectPhase::returnedZero;
        sourceTrackStep_=SourceTrackStep::objectsReturned;pc=sourceTrackPC_=sourceObjectPC_=0x1aa0b4;return true;
    }
    sourceObjectPhase_=SourceObjectPhase::opened;
    if(!writeSourceObjectDiagnostic(false,pc))return stop(pc);
    bool selected=false,valueReturned=false;
    pc=0x1b4490;if(!sourceWorldObjectConfig_->selectSourceObjectSection157D00("PodRacer",selected,d))return stop(ioPC());
    if(selected){
        pc=0x1b44b0;if(!worldTrackEmitsDust51C_)return stop(pc); //Require genuine1B28DC constructor-produced lane, never invent default knowledge.
        if(!sourceWorldObjectConfig_->readSourceObjectInteger158B30("TrackEmitsDust",0,*worldTrackEmitsDust51C_,valueReturned,d))return stop(ioPC());
        pc=0x1b44c8;if(!sourceWorldObjectConfig_->readSourceObjectInteger158B30("IPUCacheSize",3,worldMode524_,valueReturned,d))return stop(ioPC());
    }
    //Actual Application2DC/profile identity, not a Course receipt or snapshot.
    pc=0x1b44e8;std::uint8_t profileByte=0;
    if(!sourceLoadApplication_->profile||sourceLoadRace_->profile_!=sourceLoadApplication_->riderProfile||
       !sourceLoadApplication_->riderProfile->readWorldProfileByte1B44E8(*sourceLoadApplication_->profile,profileByte))return stop(pc);
    sourceObjectProfileByte1125C_=profileByte;
    if(((profileByte>>5u)&3u)==1u){
        pc=0x1b4504;if(!sourceWorldObjectConfig_->selectSourceObjectSection157D00("Rain",selected,d))return stop(ioPC());
        if(selected)return stop(0x1b4524); //No unrepresented rain constructor/NULL producer.
    }
    pc=0x1b45a0;if(!sourceWorldObjectConfig_->selectSourceObjectSection157D00("321_Anims",selected,d))return stop(ioPC());
    if(selected)return stop(0x1b45c0); //No inferred animation-array ownership.
    //Actual absent selector0 takes1B45A8, whose delay1B45AC setsv0=1.
    sourceObjectReturnValue_=1;sourceObjectPhase_=SourceObjectPhase::returnedOne;
    sourceTrackStep_=SourceTrackStep::objectsReturned;pc=sourceTrackPC_=sourceObjectPC_=0x1aa0b4;return true;
}
} //namespace ra_cold_aggregate50LA

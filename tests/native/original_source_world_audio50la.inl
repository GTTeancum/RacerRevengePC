#pragma once
#include <cstdio>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldAudioNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceAudioPhase_==SourceAudioPhase::unreached||sourceAudioPhase_==SourceAudioPhase::nativeClosed)
        return !sourceWorldAudioConfig_;
    if(nativeAub38_||(nativeEmptyAmbient3B4_&&(!nativeEmptyAmbient3B4_->sourceMode||!preflightSourceWorldAmbientNative()))||(nativeSoulTreeSounds3B8_&&(!nativeSoulTreeSounds3B8_->sourceMode||!preflightSourceWorldSoulTreeSoundsNative()))||
       (nativeMaterialSounds3BC_[0]&&!nativeMaterialSounds3BC_[0]->sourceMode)||
       (nativeMaterialSounds3BC_[1]&&!nativeMaterialSounds3BC_[1]->sourceMode)||
       ((nativeMaterialSounds3BC_[0]||nativeMaterialSounds3BC_[1])&&!preflightSourceWorldMaterialSoundsNative()))return false;
    if(sourceAudioConfig38Published_){
        if(!sourceWorldAudioConfig_||!sourceAudioParser38_)return false;
        if(sourceAudioCloseReturned_){
            //Actual272954 clears34, not38. The retained38 value is a historical
            //tombstone only; no freed Config backing is dereferenced or current.
            if(!sourceAudioRetiredAllocation_||*sourceAudioParser38_!=sourceAudioRetiredAllocation_||
               !sourceWorldAudioConfig_->sourceClosed_||sourceWorldAudioConfig_->source_)return false;
        }else if(*sourceAudioParser38_!=sourceWorldAudioConfig_->source_.get())return false;
    }
    return !sourceWorldAudioConfig_||!sourceWorldAudioConfig_->file_||
        sourceWorldAudioConfig_->file_==sourceTrackFile_;
}
bool Owner::abortSourceWorldAudioNative(std::uint32_t& pc)noexcept {
    if(sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached&&sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::nativeClosed)return false;
    if(sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached&&sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::nativeClosed)return false;
    if(sourceAmbientPhase_!=SourceAmbientPhase::unreached&&sourceAmbientPhase_!=SourceAmbientPhase::nativeClosed)return false; //Withdraw historical child Config borrow first.
    if(!preflightSourceWorldAudioNative())return false;
    if(sourceAudioPhase_==SourceAudioPhase::unreached||sourceAudioPhase_==SourceAudioPhase::nativeClosed)return true;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(sourceWorldAudioConfig_&&!sourceWorldAudioConfig_->abortNative(d)){if(d.sourcePC)pc=d.sourcePC;return false;}
    if(sourceAudioConfig38Published_)sourceAudioParser38_.reset(); //Native unknown, not guest NULL.
    sourceWorldAudioConfig_.reset();sourceAudioPhase_=SourceAudioPhase::nativeClosed;return true;
}
bool Owner::writeSourceAudioDiagnostic(bool missing,std::uint32_t& pc)noexcept {
    pc=missing?0x1b47e8:0x1b4868;
    if(sourceAudioLogAttempted_)return false;
    std::array<char,1024> output;std::size_t size=0;
    const auto append=[&](const char* s)noexcept{while(*s){if(size==output.size())return false;output[size++]=*s++;}return true;};
    const auto actualString=[&](const auto& text)noexcept{
        for(const auto& b:text){if(!b)return false;if(!*b)return true;
            if(size==output.size())return false;output[size++]=static_cast<char>(*b);}return false;};
    if(missing){
        if(!sourceAudioTextPathKnown_||!append("Audio file "))return false;
        for(std::size_t i=0;i+1<sourceAudioTextPathKnown_;++i){if(size==output.size())return false;output[size++]=sourceAudioTextPath_[i];}
        if(!append(" NOT FOUND!!"))return false;
    }else if(!append("\n\n***** Begin Load for Scene Audio ")||!actualString(worldText244_)||
        !append(" (")||!actualString(worldText140_)||!append(".aud) *****\n\n"))return false;
    //Explicit native1336C0 diagnostic adaptation: one latched stdout write;
    //short/error retained, ignored integer result does not create gameplay failure.
    sourceAudioLogAttempted_=true;sourceAudioLogMissing_=missing;sourceAudioLogRequested_=size;
    sourceAudioLogWritten_=std::fwrite(output.data(),1,size,stdout);
    sourceAudioLogNativeError_=sourceAudioLogWritten_!=size||std::ferror(stdout)!=0;return true;
}
bool Owner::loadSourceWorldAudio1B4720(std::uint32_t& pc)noexcept {
    using Config=rr::platform::OriginalWorldSourceConfig50LA;
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceAudioPhase_!=SourceAudioPhase::unreached||sourceTrackStep_!=SourceTrackStep::objectConfigClosed||
       sourceTrackPC_!=0x1aa0e8||!sourceObjectConfigCloseReturned_||!sourceObjectConfigNativeWithdrawn_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->sourceClosed_||!sourceWorldObjectConfig_->nativeClosed_||
       sourceWorldObjectConfig_->source_||sourceWorldAudioConfig_||nativeAub38_||
       !worldParser34_||*worldParser34_||!sourceAudioParser38_||*sourceAudioParser38_||
       !sourceTrackFile_||sourceTrackFile_->occurrence_||sourceTrackFile_->handle_!=-1||
       sourceTrackFile_->nativeClosed_||!sourceTrackFile_->joined_||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!preflightSourceWorldAudioNative())return false;
    const auto exact=[](const auto& source,const char* text)noexcept{
        for(std::size_t i=0;i<source.size();++i){if(!source[i]||*source[i]!=std::uint8_t(text[i]))return false;
            if(!text[i])return true;}return false;};
    if(!exact(worldText3C_,"tracks\\")||!exact(worldText140_,"ra"))return false;
    sourceAudioPhase_=SourceAudioPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceAudioPC_=at;sourceAudioPhase_=SourceAudioPhase::bounded;return false;};
    //Actual formatter/copy/last-dot branch; only produced bytes+NUL are known.
    pc=0x1b4740;std::size_t at=0;
    for(const auto& byte:worldText3C_){if(!*byte)break;sourceAudioTextPath_[at++]=static_cast<char>(*byte);}
    for(const auto& byte:worldText140_){if(!*byte)break;sourceAudioTextPath_[at++]=static_cast<char>(*byte);}
    for(const char* s=".aud";*s;++s)sourceAudioTextPath_[at++]=*s;
    sourceAudioTextPath_[at]=0;sourceAudioTextPathKnown_=at+1;
    pc=0x1b474c;std::memcpy(sourceAudioBinaryPath_.data(),sourceAudioTextPath_.data(),sourceAudioTextPathKnown_);
    sourceAudioBinaryPathKnown_=sourceAudioTextPathKnown_;
    pc=0x1b4758;std::size_t dot=0;for(std::size_t i=0;i<at;++i)if(sourceAudioBinaryPath_[i]=='.')dot=i;
    if(dot!=9)return stop(0x1b4770);
    pc=0x1b4770;std::memcpy(sourceAudioBinaryPath_.data()+dot,".aub",5);
    //Native wrapper admission is not the guest nullable4D0 allocation.
    try{sourceWorldAudioConfig_.reset(new Config);}catch(...){return stop(0x1b4794);}
    if(!sourceWorldAudioConfig_->bindNative(sourceTrackFile_))return stop(0x1b4794);
    if(!sourceWorldAudioConfig_->allocate1B4794())return stop(0x1b4794);
    pc=0x1b47ac;sourceAudioParser38_=sourceWorldAudioConfig_->source_.get();sourceAudioConfig38Published_=true;
    sourceAudioPhase_=SourceAudioPhase::allocated;
    a::OriginalStandardTextureDiagnostic50LA d;
    pc=0x1b47b8;auto returned=sourceWorldAudioConfig_->open1577D0(sourceAudioBinaryPath_.data(),d);
    const auto ioPC=[&]()noexcept{return d.sourcePC?d.sourcePC:sourceWorldAudioConfig_->sourcePC_;};
    if(returned==Config::Return::bounded)return stop(ioPC());
    if(returned==Config::Return::zero){
        pc=0x1b47d0;returned=sourceWorldAudioConfig_->open1577D0(sourceAudioTextPath_.data(),d);
        if(returned==Config::Return::bounded)return stop(ioPC());
        if(returned==Config::Return::one)return stop(0x1b4808); //Export/global and actual World34 dereference unjoined.
        if(!writeSourceAudioDiagnostic(true,pc))return stop(pc);
        pc=0x1b47f4;if(!sourceWorldAudioConfig_->close1573B0(d))return stop(ioPC());
        pc=0x1b47fc;sourceAudioParser38_=nullptr;
        sourceAudioReturnValue_=0;sourceAudioPhase_=SourceAudioPhase::returnedZero;
    }else{
        sourceAudioPhase_=SourceAudioPhase::opened;
        if(!sourceWorldAudioConfig_->sourceAudioConfigCurrent())return stop(0x1b485c);
        if(!writeSourceAudioDiagnostic(false,pc))return stop(pc);
        pc=0x1b4870;sourceAudioReturnValue_=1;sourceAudioPhase_=SourceAudioPhase::returnedOne;
    }
    sourceTrackStep_=SourceTrackStep::audioReturned;pc=sourceTrackPC_=sourceAudioPC_=0x1aa0fc;
    return true; //Only actual vE4 return; no audio Entity or later readiness.
}
}

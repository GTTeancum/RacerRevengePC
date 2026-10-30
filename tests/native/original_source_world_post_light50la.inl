#pragma once
//Actual ordered1AA150 return ->1AA178 ->1AA18C. No runtime readiness.
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceFogAndParticle1AA158(std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourcePostLightPhase_!=SourcePostLightPhase::unreached||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->world74_.get()!=this||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldApplication_||
       sourceLightPopulationPhase_!=SourceLightPopulationPhase::returned||sourceLightPopulationPC_!=0x273c28||
       !sourceLightPopulationReturnV0_||*sourceLightPopulationReturnV0_!=1||
       !preflightSourceLightPopulationNative()||!sourceEnvironmentPublished_||!nativeEnvironment3A4_||
       *nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||
       (sourceEnvironment3A4_?sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne:
                               sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedZero)||
       sourceFogTransferBorrowers_||sourceParticleInvocationBorrowers_||sourceFogTransfer_||sourceParticleConfig_||
       trackParticles_||nativeParticleManagerAttempted_||nativeParticleManager4A0_||
       runtimeQueries_||runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    const auto at=[&](std::uint32_t value){pc=sourceTrackPC_=sourcePostLightPC_=value;};
    const auto stop=[&](std::uint32_t value){at(value);sourcePostLightPhase_=SourcePostLightPhase::bounded;return false;};
    //The actual Race strong pin and private token carry this same World/App/
    //Environment occurrence. A known NULL Environment is valid for fog itself.
    at(0x1aa158);sourcePostLightPhase_=SourcePostLightPhase::fogEntered;
    bool fogReturned=false;
    {
        g::OriginalWorldFogInvocation50LA invocation(sourceLoadRace_->world74_,sourceEnvironment3A4_.get(),
            sourceLoadApplication_,worldEntity_.sessionGeneration,sourceFogTransferBorrowers_,&Owner::releaseSourceFogInvocationNative);
        at(0x1aa178);fogReturned=beginSourceFog273C30(invocation,pc);
        sourcePostLightPC_=sourceTrackPC_=pc;
        if(fogReturned&&(!sourceFogTransfer_||!sourceFogTransfer_->returned||sourceFogTransfer_->nativeCancelled||pc!=0x273f94))fogReturned=false;
    } //Bounded partial resources cancel; successful outputs remain World-owned.
    if(!fogReturned)return stop(pc);
    sourceFogCallReturnV0_=1; //Both represented273C60 and273F78 return1.
    sourcePostLightPhase_=SourcePostLightPhase::fogReturned;
    at(0x1aa180);at(0x1aa18c);sourcePostLightPhase_=SourcePostLightPhase::particleEntered;
    at(0x1b4dcc);
    if(!sourceEnvironment3A4_){
        at(0x1b4ddc);sourceParticleStageReturnV0_=1;
    }else{
        if(!sourceWorldConfig_)return stop(0x1b4de0);
        const char* name=nullptr;std::size_t length=0;
        if(!sourceEnvironment3A4_->readSourceParticleName2771F4(worldEntity_,*sourceWorldConfig_,name,length))return stop(0x1b4de0);
        at(0x1b4de8);sourceParticleSelectorNone_=length==4&&std::memcmp(name,"none",4)==0;
        if(*sourceParticleSelectorNone_){
            //Source diagnostic return ignored. Retain partial host write/error,
            //never retry it or confuse the genuine zero return with refusal.
            static constexpr char text[]="No particle texture map specified in scene file.\n";
            at(0x1b4dfc);sourceParticleStageLogAttempted_=true;
            sourceParticleStageLogWritten_=std::fwrite(text,1,sizeof(text)-1,stdout);
            sourceParticleStageLogError_=sourceParticleStageLogWritten_!=sizeof(text)-1||std::ferror(stdout)!=0;
            at(0x1b4e08);sourceParticleStageReturnV0_=0;
        }else{
            at(0x1b4e10);sourceParticleVisibilityWasNull_=worldApplication_->visibility443A98_==nullptr;
            if(*sourceParticleVisibilityWasNull_){
                at(0x1b4e20);useEnvironment35C_=1;sourceParticleEnvironment35CWritten_=true;
                at(0x1b4e2c);sourceParticlePredecessorEntered_=true;
                if(!constructSourceVisibilityEnvironment272FC0(pc))return stop(pc);
                if(pc!=0x273174||!sourceVisibilityEnvironmentReturned_||!sourceVisibilityEnvironmentReturnV0_||
                   *sourceVisibilityEnvironmentReturnV0_!=1||!preflightSourceVisibilityEnvironmentNative())return stop(pc);
                sourceParticlePredecessorReturned_=true;
                at(0x1b4e34);if(!continueSourceParticleManager1B4E34(pc))return stop(pc);
            }
            else {
                //A preexisting raw global is not a source-owner lease. This
                //fresh constructor route never adopts native prewarm state.
                return stop(0x1b4e10);
            }
        }
    }
    at(0x1b4eac);sourcePostLightPhase_=SourcePostLightPhase::particleReturned;
    at(0x1aa194);return true; //Ignored v0; stop before Application330/control branch.
}
}

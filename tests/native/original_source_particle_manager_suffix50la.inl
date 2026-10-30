#pragma once
//Concrete source1B4E34 suffix; no native prewarm adoption or generic v0 injection.
namespace ra_cold_aggregate50LA {
bool Owner::sourceParticleStageCountedNative(std::size_t& count)const noexcept {
    count=0;
    if(!sourceParticleAllocationAttempted_)return !sourceParticlePendingAllocation_&&!sourceParticle4A0_&&
        !sourceParticleAllocationReturned_&&!sourceParticleAllocationNonNull_&&!sourceParticleConstructorReturned_&&!sourceParticlePublished4A0_&&
        !sourceParticleLoadEntered_&&!sourceParticleLoadReturned_&&!sourceParticleReturnedSelf_&&!sourceParticleDepthBefore_&&!sourceParticleDepthAfter_;
    if(nativeScbThread_!=std::this_thread::get_id()||!worldApplication_||worldApplication_->attachmentLock423F58()||
       !sourceParticleAllocationReturned_||!sourceParticleAllocationNonNull_||sourceParticlePendingAllocation_&&trackParticles_)return false;
    if(sourceParticleStageNativeClosed_)return !sourceParticlePendingAllocation_&&!trackParticles_&&
        (!sourceParticlePublished4A0_||sourceParticle4A0NativeWithdrawn_&&!nativeParticleManager4A0_);
    if(sourceParticle4A0NativeWithdrawn_||sourceParticlePublished4A0_!=bool(sourceParticle4A0_)||
       *sourceParticleAllocationNonNull_!=bool(sourceParticlePendingAllocation_||trackParticles_))return false;
    if(sourceParticlePublished4A0_&&(*sourceParticle4A0_!=trackParticles_.get()||nativeParticleManager4A0_!=*sourceParticle4A0_))return false;
    if(sourceParticlePendingAllocation_){const auto& pending=*sourceParticlePendingAllocation_;
        if(pending.application_||pending.attempted_||pending.entityConstructed_||pending.constructorReturned_||
           sourceParticleConstructorReturned_||sourceParticlePublished4A0_||sourceParticleLoadEntered_)return false;
        return true; //Actual allocation held; native shared-control capacity refused.
    }
    if(!trackParticles_)return !sourceParticleConstructorReturned_&&!sourceParticleLoadEntered_&&
        !sourceParticleLoadReturned_&&!sourceParticleReturnedSelf_;
    const auto& p=*trackParticles_;const auto& e=p.component_;
    if(!p.attempted_)return !p.closed_&&!p.application_&&!p.generation_&&!p.entityConstructed_&&!p.constructorReturned_&&
        !p.sourceEntityEnrolled_&&!e.reference00&&!sourceParticleConstructorReturned_&&!sourceParticlePublished4A0_&&
        !sourceParticleLoadEntered_&&!sourceParticleLoadReturned_&&!sourceParticleReturnedSelf_;
    if(!p.sourceEntityCountedNative(worldApplication_.get(),worldEntity_.sessionGeneration,count))return false;
    if(bool(sourceParticleDepthBefore_)!=p.depthGlobalWritten_||bool(sourceParticleDepthAfter_)!=p.depthGlobalWritten_||
       (p.depthGlobalWritten_&&(sourceParticleDepthBefore_!=p.depthGlobalBefore_||sourceParticleDepthAfter_!=p.depthGlobalAfter_||
        p.depthWriteOrigin_!=g::OriginalTrackParticleCold50LA::DepthWriteOrigin::source160E78)))return false;
    if(sourceParticleConstructorReturned_&&(!p.constructorReturned_||!p.sourceEntityEnrolled_))return false;
    if(sourceParticlePublished4A0_&&!sourceParticleConstructorReturned_)return false;
    if(sourceParticleLoadEntered_&&(!sourceParticlePublished4A0_||!sourceParticleConstructorReturned_))return false;
    if(sourceParticleLoadReturned_!=p.sourceLoadReturned160E00_||
       (sourceParticleLoadReturned_?(sourceParticleReturnedSelf_!=&p):sourceParticleReturnedSelf_!=nullptr))return false;
    return true;
}
bool Owner::abortSourceParticleStageNative(std::uint32_t& pc)noexcept {
    if(!sourceParticleAllocationAttempted_)return !sourceParticlePendingAllocation_;
    if(!preflightSourceWorldParticleCountNative())return false;
    if(nativeScbThread_!=std::this_thread::get_id()||sourceParticleInvocationBorrowers_||sourceParticleConfig_||
       sourceParticleConfigInvocation_)return false;
    if(sourceParticleStageNativeClosed_)return !sourceParticlePendingAllocation_&&!trackParticles_;
    if(sourceParticlePendingAllocation_&&trackParticles_)return false;
    if(trackParticles_&&(trackParticles_.use_count()!=1||trackParticles_->sourceConfigInvocationBorrowers_||trackParticles_->sourceSmokerManagerBorrowers_))return false;
    if(sourceParticlePublished4A0_&&(!sourceParticle4A0_||nativeParticleManager4A0_!=*sourceParticle4A0_||
       *sourceParticle4A0_!=trackParticles_.get()))return false;
    //No source zero-return branch is claimed. Close the actual partial owner,
    //withdraw World borrow natively, and retain original publication/depth history.
    if(trackParticles_&&!trackParticles_->close()){pc=sourceParticleStagePC_;return false;}
    if(sourceParticlePendingAllocation_&&!sourceParticlePendingAllocation_->close()){pc=sourceParticleStagePC_;return false;}
    if(sourceParticlePublished4A0_){nativeParticleManager4A0_=nullptr;sourceParticle4A0NativeWithdrawn_=true;}
    trackParticles_.reset();sourceParticlePendingAllocation_.reset();sourceParticleStageNativeClosed_=true;
    return true;
}
bool Owner::continueSourceParticleManager1B4E34(std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourcePostLightPhase_!=SourcePostLightPhase::particleEntered||sourcePostLightPC_!=0x1b4e34||
       !sourceParticlePredecessorEntered_||!sourceParticlePredecessorReturned_||
       !sourceVisibilityEnvironmentReturned_||!sourceVisibilityEnvironmentReturnV0_||*sourceVisibilityEnvironmentReturnV0_!=1||
       !preflightSourceVisibilityEnvironmentNative()||sourceVisibilityEnvironment_.withdrawn||
       sourceVisibilityEnvironment_.nativeNodeDetached||sourceVisibilityEnvironment_.nativeAliasesWithdrawn||
       sourceVisibilityEnvironment_.nativeConstructorReleased||!sourceVisibilityEnvironment_.tree||!sourceVisibilityEnvironment_.node.owner||
       !sourceParticleSelectorNone_||*sourceParticleSelectorNone_||
       !sourceParticleEnvironment35CWritten_||useEnvironment35C_!=1||
       !sourceParticleVisibilityWasNull_||!*sourceParticleVisibilityWasNull_||
       !sourceLoadRace_||sourceLoadRace_->world74_.get()!=this||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldApplication_||
       !sourceEnvironment3A4_||!nativeEnvironment3A4_||*nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||
       sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne||!sourceWorldConfig_||
       !sourceFogCallReturnV0_||*sourceFogCallReturnV0_!=1||!sourceFogTransfer_||!sourceFogTransfer_->returned||
       sourceFogTransfer_->nativeCancelled||sourceParticleAllocationAttempted_||sourceParticlePendingAllocation_||
       trackParticles_||nativeParticleManager4A0_||nativeParticleManagerAttempted_||
       sourceParticleInvocationBorrowers_||sourceParticleConfig_||sourceParticleConfigInvocation_)return false;
    //The exact completed source272FC0 owner is checked above, not a raw global.
    const auto at=[&](std::uint32_t value){pc=sourceTrackPC_=sourcePostLightPC_=sourceParticleStagePC_=value;};
    const auto stop=[&](std::uint32_t value){at(value);return false;};
    at(0x1b4e38);sourceParticleAllocationAttempted_=true;
    sourceParticlePendingAllocation_.reset(new(std::nothrow) g::OriginalTrackParticleCold50LA);
    sourceParticleAllocationNonNull_=bool(sourceParticlePendingAllocation_);sourceParticleAllocationReturned_=true;
    if(sourceParticlePendingAllocation_){
        //Shared-control storage is native capacity, not source1D0 allocation.
        //unique_ptr promotion preserves ownership on control-block refusal.
        try{trackParticles_=std::shared_ptr<g::OriginalTrackParticleCold50LA>(std::move(sourceParticlePendingAllocation_));}
        catch(...){return stop(0x1b4e48);}
        at(0x1b4e4c);g::OriginalTrackParticleColdDiagnostic50LA d;
        if(!trackParticles_->constructSource160B10(worldApplication_,worldEntity_.sessionGeneration,d))return stop(d.sourcePC);
        sourceParticleConstructorReturned_=true;
    }
    //Actual unconditional source store, including a returned NULL malloc.
    at(0x1b4e54);nativeParticleManager4A0_=trackParticles_.get();sourceParticle4A0_=trackParticles_.get();sourceParticlePublished4A0_=true;
    at(0x1b4e64);if(!trackParticles_)return stop(pc); //NULL vtable dereference, NOT loader return0.
    const char* name=nullptr;std::size_t length=0;
    if(!sourceEnvironment3A4_->readSourceParticleName2771F4(worldEntity_,*sourceWorldConfig_,name,length))return stop(0x1b4e70);
    //Captured Environment is strongly protected by the exact World pin for the
    //whole virtual160E00 occurrence; actual caller's fixed a2 is5000.
    bool returned=false;
    {
        g::OriginalWorldParticleInvocation50LA invocation(sourceLoadRace_->world74_,trackParticles_,sourceEnvironment3A4_.get(),
            sourceLoadApplication_,worldEntity_.sessionGeneration,trackParticles_->generation_,
            sourceParticleInvocationBorrowers_,trackParticles_->sourceConfigInvocationBorrowers_,&Owner::releaseSourceParticleInvocationNative);
        at(0x1b4e6c);sourceParticleLoadEntered_=true;
        //These preexisting segment helpers deliberately returnfalse at their
        //documented suspension boundary. Only their exact committed PC/state
        //permits the next segment, never a native refusal at any other point.
        trackParticles_->beginSourceLoad160E00(pc);
        if(trackParticles_->depthGlobalWritten_){sourceParticleDepthBefore_=trackParticles_->depthGlobalBefore_;
            sourceParticleDepthAfter_=trackParticles_->depthGlobalAfter_;} //Retained even after native owner cancellation.
        if(pc==0x160f04&&trackParticles_->sourceLoadPC_==pc&&trackParticles_->sourceScratchCleared_){
            trackParticles_->continueSourcePool160F04(pc);
            if(pc==0x160f60&&trackParticles_->sourceLoadPC_==pc&&trackParticles_->sourcePoolAttempted_&&
               trackParticles_->sourceParticlePointersPublished_)
                returned=beginSourceParticleConfig1617A0(invocation,pc);
        }
        if(returned&&(!trackParticles_->sourceLoadReturned160E00_||pc!=0x161174))returned=false;
        if(returned){sourceParticleLoadReturned_=true;sourceParticleReturnedSelf_=trackParticles_.get();}
    } //Config native backing retires after actual stack close; pool/material remain.
    if(!returned)return stop(pc); //No synthetic1B4E7C diagnostic/destructor.
    if(sourceParticleReturnedSelf_!=trackParticles_.get()||*sourceParticle4A0_!=sourceParticleReturnedSelf_)return stop(0x1b4e74);
    at(0x1b4e74);at(0x1b4e78);sourceParticleStageReturnV0_=1; //Taken BNE delay store.
    at(0x1b4eac);return true; //Caller resumes1AA194; no World/renderer readiness.
}
}

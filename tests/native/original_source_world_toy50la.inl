#pragma once
#include <cstdio>
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldToyNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceToyPhase_==SourceToyPhase::unreached)return !sourceToyAttempted_;
    if(sourceToyPhase_==SourceToyPhase::nativeClosed)return !nativeEmptyToyList3C8_&&!sourceToy3C8_;
    if(!sourceToyAttempted_||runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    if(sourceToyPublished_){if(!sourceToy3C8_||*sourceToy3C8_!=nativeEmptyToyList3C8_.get())return false;}
    else if(!sourceToy3C8_||*sourceToy3C8_)return false; //Retained allocation before271414, reset NULL untouched.
    return canRetireSourceToyNative();
}
inline bool Owner::abortSourceWorldToyNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldToyNative())return false;
    if(sourceToyPhase_==SourceToyPhase::unreached||sourceToyPhase_==SourceToyPhase::nativeClosed)return true;
    //Exact count/authority cleanup can refuse without mutation. Once it
    //succeeds only non-failing plain destruction remains on this owner thread.
    if(!retireSourceToyCountNative(pc))return false;
    sourceToy3C8_.reset();nativeEmptyToyList3C8_.reset();
    sourceToyPhase_=SourceToyPhase::nativeClosed;pc=0;return true;
}
inline bool Owner::loadSourceWorldToy2713E0(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceToyPhase_!=SourceToyPhase::unreached||!sourceWorldToyInvocationCurrent()||
       !sourceToy3C8_||*sourceToy3C8_||nativeEmptyToyList3C8_||!sourceWorldObjectConfig_||
       !reserveSourceToyAuthorityNative())return false;
    //Authority capacity reservation above is native preadmission, not a guest
    //malloc result or early counted constructor. All actual allocations follow.
    sourceToyAttempted_=true;sourceToyPhase_=SourceToyPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceToyPC_=at;sourceToyPhase_=SourceToyPhase::bounded;return false;};
    pc=0x2713f8;try{nativeEmptyToyList3C8_.reset(new NativeEmptyToyList3C8);}catch(...){nativeEmptyToyList3C8_.reset();}
    if(nativeEmptyToyList3C8_){nativeEmptyToyList3C8_->sourceMode=true;
        if(!constructSourceToy2D9B40(pc))return stop(pc);}
    pc=0x271414;sourceToy3C8_=nativeEmptyToyList3C8_.get();sourceToyPublished_=true;
    pc=0x27141c;if(!nativeEmptyToyList3C8_)return stop(pc);
    auto& config=*sourceWorldObjectConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x2d9c94;if(!config.selectSourceToyAnimations157D00(returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(returned)return stop(0x2d9cb8); //No unsupported toy record/factory authority.
    if(!config.sourceToyAnimationsMissingCurrent())return stop(0x2d9ca4);
    //Explicit native diagnostic adaptation for reached1336C0, not a guest
    //stdio emulation/no-op. One call, retained short/error result; caller ignores it.
    static constexpr char diagnostic[]="No Animations defined.\n";
    pc=0x2d9ca8;if(sourceToyLogAttempted_)return stop(pc);
    sourceToyLogAttempted_=true;sourceToyLogBytes_=std::fwrite(diagnostic,1,sizeof(diagnostic)-1,stdout);
    sourceToyLogError_=std::ferror(stdout)!=0;
    pc=0x2d9cb4;sourceToyLoaderReturnedZero_=true;
    auto& list=*nativeEmptyToyList3C8_;pc=0x271438;
    if(!list.sourceCountKnown||list.count30||!list.sourceArrayKnown||list.sourceArray34||
       !list.sourceVtableKnown||list.sourceVtable08!=0x3d5390||!list.sourceCounted||!list.sourceEnrolled||
       !preflightSourceWorldToyNative())return stop(pc);
    pc=0x27144c;if(!retireSourceToyCountNative(pc,true))return stop(pc);
    //2D9BB4 sees actualNULL34, skips toy array loop.1413E0 was reached;
    //destructor +1 frees this actual owned object before caller NULL store.
    pc=0x2d9c38;nativeEmptyToyList3C8_.reset();
    pc=0x271454;sourceToy3C8_=nullptr;
    sourceToyPhase_=SourceToyPhase::returned;pc=sourceToyPC_=0x271d3c;return true;
}
} //namespace ra_cold_aggregate50LA

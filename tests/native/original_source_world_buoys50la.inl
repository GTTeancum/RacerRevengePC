#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldBuoysNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceBuoysPhase_==SourceBuoysPhase::unreached)return true; //No legacy scalar adoption/withdrawal.
    for(const auto& v:sourceBuoyStack260To280_)
        if(v&&*v!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;
    if(nativeBuoyCount538_&&*nativeBuoyCount538_!=0)return false;
    if(sourceBuoysPhase_==SourceBuoysPhase::returned&&
       (!nativeBuoyCount538_||!sourceBuoyStack260To280_[0]||!sourceBuoyStack260To280_[1]||!sourceBuoyStack260To280_[2]))return false;
    return true;
}
bool Owner::abortSourceWorldBuoysNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldBuoysNative())return false;
    if(sourceBuoysPhase_==SourceBuoysPhase::unreached||sourceBuoysPhase_==SourceBuoysPhase::nativeClosed)return true;
    //No allocated payload/borrow exists on this negative branch. Preserve all
    //actual stack/default538 history, including failures after selection.
    sourceBuoysPhase_=SourceBuoysPhase::nativeClosed;return true;
}
bool Owner::loadSourceWorldBuoys1B4890(std::uint32_t& pc)noexcept {
    pc=0;if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceBuoysPhase_!=SourceBuoysPhase::unreached||nativeBuoyCount538_||
       !sourceWorldObjectConfig_||!sourceWorldBuoysInvocationCurrent())return false;
    const auto at=[&](std::uint32_t p){pc=sourceBuoysPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceBuoysPhase_=SourceBuoysPhase::bounded;return false;};
    sourceBuoysPhase_=SourceBuoysPhase::entered;
    //Actual vC8 target1B4890 is a tail jump, not a second owner/loader.
    at(0x1b50ac);sourceBuoyStack260To280_[0]=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}};
    at(0x1b50b4);sourceBuoyStack260To280_[1]=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}};
    at(0x1b50bc);sourceBuoyStack260To280_[2]=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}};
    auto& config=*sourceWorldObjectConfig_;a::OriginalStandardTextureDiagnostic50LA diagnostic;bool returned=false;
    at(0x1b50c8);if(!config.selectSourceBuoys157D00(returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
    if(returned)return stop(0x1b50d0); //Not the authenticated missing-selection cohort.
    at(0x1b50e0);if(!config.readSourceBuoyCount158B30(nativeBuoyCount538_,returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
    if(returned||!nativeBuoyCount538_)return stop(0x1b50e8);
    at(0x1b50ec);if(*nativeBuoyCount538_!=0)return stop(pc);
    //BLEZ takes1B5320. No allocation or53C read/write is reached. No explicit
    //v0=1 exists in the epilogue; caller ignores its inherited return register.
    at(0x271da0);sourceBuoysPhase_=SourceBuoysPhase::returned;return true;
}
}

#pragma once
#include "original_source_world_light_read50la.inl"
#include "original_source_world_cscamera_history50la.h"
#include <new>
namespace ra_cold_aggregate50LA {
inline bool Owner::loadSourceWorldCsCameras272380(std::uint32_t& pc)noexcept {
    pc=0;
    if(sourceCsCameraPhase_!=SourceCsCameraPhase::unreached||
       !sourceWorldCsCamerasInvocationCurrent()||nativeCsCameraTable3E4_||
       !nativeCsCameraZones3E8_||*nativeCsCameraZones3E8_||
       !nativeCsCameraCount3E4_||*nativeCsCameraCount3E4_!=0||
       !worldZones34C_||*worldZones34C_!=10)return false;
    // Native ledger admission, not a source allocation or constructor. The
    // only source allocation below is the actual seven-word row array.
    nativeCsCameraTable3E4_.emplace();auto& table=*nativeCsCameraTable3E4_;
    table.sourceMode=true;table.sourceApplication=worldApplication_.get();
    table.sourceGeneration=worldEntity_.sessionGeneration;
    sourceCsCameraPhase_=SourceCsCameraPhase::nativeAdmitted;
    auto fail=[&](std::uint32_t at){pc=sourceCsCameraPC_=at;sourceCsCameraPhase_=SourceCsCameraPhase::bounded;return false;};
    auto& config=*sourceWorldObjectConfig_;a::OriginalStandardTextureDiagnostic50LA d;
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy){
        if(!config.readSourceCsCameraField158240(key,value,found,copy,d)){
            pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;
        }return true;
    };
    // Output address may be genuinely NULL. Perform the actual lookup and
    // bounded strtol/default lowering BEFORE testing it at the output store.
    auto integer=[&](const char* key,std::int32_t fallback,std::optional<std::uint32_t>* row,
                     std::optional<std::int32_t>* stack){
        std::string_view value;bool found=false;std::uint32_t copy=0;
        if(!fetch(key,value,found,copy))return false;
        std::int32_t next=fallback;
        if(found){pc=0x158a34;if(!source_world_light50LA::Reader<decltype(fetch)>::integerToken(value,next))return false;
            pc=value=="T"?0x158968:value=="F"?0x158990:0x158a44;}
        else pc=0x158b60;
        if(stack)*stack=next;
        else {if(!row)return false;*row=static_cast<std::uint32_t>(next);}
        pc=0x158b68;return true;
    };
    bool returned=false;sourceCsCameraPhase_=SourceCsCameraPhase::entered;
    pc=0x2723a0;
    if(!config.selectSourceCsCameraSection157D00(0,returned,d))return fail(d.sourcePC?d.sourcePC:config.sourcePC_);
    // Source ignores selector0. This exact authored cohort supports the
    // positive selection only; a negative is an unjoined getter, not return0.
    if(!returned)return fail(0x2723b8);
    pc=0x2723b8;
    if(!integer("numberofcameras",0,nullptr,&sourceCsCameraStack8C_))return fail(pc);
    pc=0x2723c4;
    if(!sourceCsCameraStack8C_||*sourceCsCameraStack8C_!=7)return fail(pc);
    // Actual reset-known NULL chooses fresh branch; no append/realloc waiver.
    pc=0x2723fc;nativeCsCameraCount3E4_=7;
    pc=0x272404;sourceCsCameraLimit_=7;
    pc=0x272408;sourceCsCameraIndex_=0;
    pc=0x27240c;table.sourceAllocationAttempted=true;
    // Semantic seven-word backing; optional tags represent unwritten source
    // words, not initialized guest values. No camera/Entity/Session is made.
    table.sourceRows.reset(new(std::nothrow) NativeCsCameraRows);
    pc=0x272414;nativeCsCameraZones3E8_=table.sourceRows.get();table.sourcePublished=true;
    while(*sourceCsCameraIndex_<*sourceCsCameraLimit_){
        const auto index=*sourceCsCameraIndex_;
        pc=0x272424;sourceCsCameraSectionNumber_=index+1;
        // Exact bounded sprintf("CSCamera%d",1..7), retained in the genuine
        // Config name selection; no allocation-prone native string temporary.
        pc=0x272438;returned=false;
        if(!config.selectSourceCsCameraSection157D00(*sourceCsCameraSectionNumber_,returned,d))
            return fail(d.sourcePC?d.sourcePC:config.sourcePC_);
        if(!returned)return fail(0x272458);
        pc=0x272458;
        auto* output=table.sourceRows?&(*table.sourceRows)[index]:nullptr;
        if(!integer("initialzone",1,output,nullptr))return fail(pc);
        // The actual output is already committed. Unknown process history
        // stops BEFORE reading the first-use guard, never rolls this row back.
        if(!source_world_cscamera50LA::History::enter(static_cast<std::int32_t>(*worldZones34C_),
            static_cast<std::int32_t>(**output),pc))return fail(pc);
        pc=0x2724e8;++*sourceCsCameraIndex_;table.next=*sourceCsCameraIndex_;
    }
    sourceCsCameraPhase_=SourceCsCameraPhase::returned;
    pc=sourceCsCameraPC_=0x271d6c;return true; //vB8 returned; NOT whole vA4.
}
inline bool Owner::preflightSourceWorldCsCamerasNative()const noexcept {
    if(sourceCsCameraPhase_==SourceCsCameraPhase::unreached)return true; //Leave legacy owner untouched.
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceCsCameraPhase_==SourceCsCameraPhase::nativeClosed)
        return !nativeCsCameraTable3E4_&&!nativeCsCameraZones3E8_;
    if(!nativeCsCameraTable3E4_)return false;const auto& table=*nativeCsCameraTable3E4_;
    if(!table.sourceMode||table.session||table.authoring||table.next>7||
       table.sourceApplication!=worldApplication_.get()||table.sourceGeneration!=worldEntity_.sessionGeneration)return false;
    for(const auto& legacy:table.zones)if(legacy)return false;
    if(table.sourceRows&&!table.sourceAllocationAttempted)return false;
    if(table.sourcePublished){
        if(!table.sourceAllocationAttempted||!nativeCsCameraZones3E8_||
           *nativeCsCameraZones3E8_!=table.sourceRows.get()||
           !nativeCsCameraCount3E4_||*nativeCsCameraCount3E4_!=7)return false;
    }else if(!nativeCsCameraZones3E8_||*nativeCsCameraZones3E8_||table.sourceRows)return false;
    // No source camera controller or other borrower is admitted by this cohort.
    // Same-role legacy consumers reject sourceMode before borrowing inline rows.
    return true;
}
inline bool Owner::abortSourceWorldCsCamerasNative(std::uint32_t& pc)noexcept {
    pc=sourceCsCameraPC_;
    if(sourceCsCameraPhase_==SourceCsCameraPhase::unreached)return true;
    if(!preflightSourceWorldCsCamerasNative())return false;
    if(sourceCsCameraPhase_==SourceCsCameraPhase::nativeClosed)return true;
    // All remaining operations are non-failing plain-storage destruction on
    // the admitted owner thread. No source free/reset/NULL store is claimed.
    nativeCsCameraZones3E8_.reset();
    nativeCsCameraTable3E4_->sourceRows.reset();nativeCsCameraTable3E4_.reset();
    sourceCsCameraPhase_=SourceCsCameraPhase::nativeClosed;return true;
}
}

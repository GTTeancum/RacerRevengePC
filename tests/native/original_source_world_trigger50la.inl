#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldTriggerNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceTriggerPhase_==SourceTriggerPhase::unreached)return !sourceTriggerAttempted_;
    if(sourceTriggerPhase_==SourceTriggerPhase::nativeClosed)return !sourceTrigger3D0_&&!nativeTriggerList3D0_;
    if(!sourceTriggerAttempted_||(sourceTriggerConfigArg_&&sourceTriggerConfigArg_!=sourceWorldObjectConfig_.get())||runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    if(sourceTriggerPublished_){if(!sourceTrigger3D0_||*sourceTrigger3D0_!=nativeTriggerList3D0_.get())return false;}
    else if(sourceTrigger3D0_)return false; //No reset producer: still unknown before1B502C.
    return canRetireSourceTriggerNative();
}
inline bool Owner::abortSourceWorldTriggerNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldTriggerNative())return false;
    if(sourceTriggerPhase_==SourceTriggerPhase::unreached||sourceTriggerPhase_==SourceTriggerPhase::nativeClosed)return true;
    if(!retireSourceTriggerCountNative(pc))return false;
    sourceTrigger3D0_.reset();nativeTriggerList3D0_.reset();sourceTriggerConfigArg_=nullptr;
    sourceTriggerPhase_=SourceTriggerPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldTriggers1B5000(std::uint32_t& pc)noexcept {
    pc=0;std::array<std::uint32_t,4> origin;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||sourceTriggerPhase_!=SourceTriggerPhase::unreached||
       !sourceWorldTriggerInvocationCurrent()||sourceTrigger3D0_||!sourceWorldObjectConfig_||
       !copySourceTriggerOrigin(origin)||!reserveSourceTriggerAuthorityNative())return false;
    sourceTriggerAttempted_=true;sourceTriggerPhase_=SourceTriggerPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceTriggerPC_=at;sourceTriggerPhase_=SourceTriggerPhase::bounded;return false;};
    pc=0x1b5010;try{nativeTriggerList3D0_.reset(new NativeTriggerList3D0);}catch(...){nativeTriggerList3D0_.reset();}
    if(nativeTriggerList3D0_){nativeTriggerList3D0_->sourceMode=true;if(!constructSourceTriggerList2DC320(pc))return stop(pc);}
    pc=0x1b502c;sourceTrigger3D0_=nativeTriggerList3D0_.get();sourceTriggerPublished_=true;
    pc=0x1b5038;sourceTriggerFactoryVtable_=0x3cf090; //Actual retained stack helper, no factory call yet.
    pc=0x1b5040;sourceTriggerConfigArg_=sourceWorldObjectConfig_.get();
    pc=0x1b5044;if(!nativeTriggerList3D0_)return stop(pc);
    auto& l=*nativeTriggerList3D0_;auto& config=*sourceWorldObjectConfig_;
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!config.readSourceTriggerField158240(key,value,present,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    auto integer=[&](const char* key,std::int32_t fallback,std::int32_t& out)noexcept {
        std::string_view value;std::uint32_t copy=0;bool present=false;if(!fetch(key,value,present,copy))return false;
        if(!present){pc=0x158b60;out=fallback;pc=0x158b68;return true;}
        //Narrow actual authored lexemes, not a new generic bool/atoi policy.
        if(value=="TRUE"){pc=0x158968;out=1;return true;}
        if(value=="100.000000"){pc=0x158a34;pc=0x158a44;out=100;return true;}
        std::int32_t n;pc=0x158a34;
        if(!source_world_light50LA::Reader<decltype(fetch)>::integerToken(value,n))return false;
        pc=value=="T"?0x158968:value=="F"?0x158990:0x158a44;out=n;return true;
    };
    pc=0x2dc478;sourceTriggerListSuccessCount_=0; //Actual selector call delay slot.
    pc=0x2dc474;if(!config.selectSourceTriggerSection157D00(0,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x2dc484); //Missing diagnostic/false loader branch outside this positive authored cohort.
    std::int32_t number;pc=0x2dc4a8;if(!integer("numberoftriggers",0,number))return stop(pc);
    if(number!=2)return stop(0x2dc4b0);
    pc=0x2dc4f4;l.count30=2;l.sourceCountKnown=true;
    pc=0x2dc4f8;sourceTriggerListLimit_=l.count30;
    pc=0x2dc4fc;sourceTriggerListStartIndex_=0;
    pc=0x2dc500;try{l.descriptors34.reset(new NativeTriggerDescriptor*[2]);}catch(...){l.descriptors34.reset();}
    pc=0x2dc508;l.sourceArrayAllocationReturned=true;l.sourceArrayKnown=true;
    pc=0x2dc514;sourceTriggerRow_=*sourceTriggerListStartIndex_;
    pc=0x2dc518;sourceTriggerRawOffset_=*sourceTriggerListStartIndex_<<2;
    return loadSourceWorldTriggerDescriptor2DC53C(pc);

}

inline bool Owner::loadSourceWorldTriggerDescriptor2DC53C(std::uint32_t& pc)noexcept {
    std::array<std::uint32_t,4> origin;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       (sourceTriggerPhase_!=SourceTriggerPhase::entered&&sourceTriggerPhase_!=SourceTriggerPhase::rowEntered)||
       !nativeTriggerList3D0_||!sourceWorldObjectConfig_||sourceTriggerRow_>=2||
       !sourceTriggerListStartIndex_||*sourceTriggerListStartIndex_!=0||
       !sourceTriggerListLimit_||*sourceTriggerListLimit_!=2||!sourceTriggerRawOffset_||
       *sourceTriggerRawOffset_!=sourceTriggerRow_*4||!copySourceTriggerOrigin(origin))return false;
    auto& l=*nativeTriggerList3D0_;auto& config=*sourceWorldObjectConfig_;
    if(l.descriptorSlotsPublished!=sourceTriggerRow_||l.descriptorOwners[sourceTriggerRow_])return false;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceTriggerPC_=at;sourceTriggerPhase_=SourceTriggerPhase::bounded;return false;};
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!config.readSourceTriggerField158240(key,value,present,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    auto integer=[&](const char* key,std::int32_t fallback,std::int32_t& out)noexcept {
        std::string_view value;std::uint32_t copy=0;bool present=false;if(!fetch(key,value,present,copy))return false;
        if(!present){pc=0x158b60;out=fallback;pc=0x158b68;return true;}
        if(value=="TRUE"){pc=0x158968;out=1;return true;}
        if(value=="100.000000"){pc=0x158a34;pc=0x158a44;out=100;return true;}
        std::int32_t n;pc=0x158a34;
        if(!source_world_light50LA::Reader<decltype(fetch)>::integerToken(value,n))return false;
        pc=value=="T"?0x158968:value=="F"?0x158990:0x158a44;out=n;return true;
    };
    pc=0x2dc520;sourceTriggerSectionNumber_=static_cast<std::uint32_t>(sourceTriggerRow_+1)-*sourceTriggerListStartIndex_;
    //Exact real loop args prove format Trigger%d =>Trigger1/Trigger2.
    if(*sourceTriggerSectionNumber_!=sourceTriggerRow_+1)return stop(0x2dc530);
    pc=0x2dc53c;if(!config.selectSourceTriggerSection157D00(static_cast<std::uint32_t>(1+2*sourceTriggerRow_),returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x2dc54c); //No synthetic NULLslot on missing source section.
    pc=0x2dc568;auto& owned=l.descriptorOwners[sourceTriggerRow_];
    try{owned.reset(new NativeTriggerDescriptor);}catch(...){owned.reset();}
    if(owned){owned->sourceFields.emplace();auto& f=*owned->sourceFields;
        pc=0x2dc580;f.vtable6C=0x3d52c0;f.known|=1;
        pc=0x2dc584;owned->trigger00=nullptr;f.known|=2;
        pc=0x2dc588;f.sourceGeometry64=nullptr;f.known|=4;
        pc=0x2dc58c;owned->actionCount04=0;f.known|=8;
        pc=0x2dc594;owned->actions68.reset();f.known|=16;
        pc=0x2dc598;f.enabled0C=1;f.known|=32;
    }
    pc=0x2dc5b0;if(!l.descriptors34)return stop(pc); //Retain allocated, unpublishable descriptor.
    l.descriptors34[sourceTriggerRow_]=owned.get();l.descriptorSlotsPublished=sourceTriggerRow_+1;
    pc=0x2dc5c0;if(!owned)return stop(pc);
    auto& f=*owned->sourceFields;
    //Actual new descriptor virtual call creates a NEW callee frame. Only its
    //native admission/stack knowledge is reset; prior row resources and
    //per-row diagnostic/return history remain owned and untouched.
    pc=0x2dc5c8;
    sourceTriggerParentKnown_=false;sourceTriggerEulerAttempted_=false;
    sourceTriggerEulerReturned_=false;sourceTriggerRadiusReturned_=false;
    sourceTriggerActionSuccessCount_.reset();sourceTriggerActionLoopIndex_.reset();sourceTriggerDescriptorReturn_.reset();
    sourceTriggerActionSectionKnown_=false;sourceTriggerActionLogAttempted_=false;
    sourceTriggerActionLogBytes_=0;sourceTriggerActionLogError_=false;
    pc=0x2dbdec;sourceTriggerActionType_=0; //Actual retained stack398 before Name getter.
    pc=0x2dbdf4;sourceTriggerActionSuccessCount_=0; //Actual Name-call delay slot s5=0.
    pc=0x2dbdf0;if(!read.text("name","",f.name14.data(),f.name14.size(),found))return stop(pc);f.nameKnown=true;
    pc=0x2dbe08;if(!integer("numberofactions",0,owned->actionCount04))return stop(pc);f.known|=8;
    pc=0x2dbe20;if(!integer("zone",1,f.zone08))return stop(pc);f.known|=64;
    pc=0x2dbe40;if(!read.text("ignoretrigger","",f.ignore3C.data(),f.ignore3C.size(),found))return stop(pc);f.ignoreKnown=true;
    pc=0x2dbe58;if(!integer("startenabled",1,f.enabled0C))return stop(pc);f.known|=32;
    pc=0x2dbe78;if(!read.text("parent","",sourceTriggerParentName_.data(),sourceTriggerParentName_.size(),found))return stop(pc);sourceTriggerParentKnown_=true;
    //Actual Zone decrement store is RandomTriggerPercent's call delay slot.
    const std::uint32_t zone=static_cast<std::uint32_t>(f.zone08)-1u;
    pc=0x2dbea0;std::memcpy(&f.zone08,&zone,4);
    pc=0x2dbe9c;if(!integer("randomtriggerpercent",100,f.random10))return stop(pc);f.known|=128;
    pc=0x2dbea4;try{f.geometry64.reset(new SourceTriggerGeometry2DBEB4);}catch(...){f.geometry64.reset();}
    f.geometryAllocationReturned=true;
    if(f.geometry64){auto& g=*f.geometry64;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000}}; //Architectural VF0, not a global default.
        pc=0x2dbeb4;g.position00=vf0;pc=0x2dbebc;g.box10=vf0;
        pc=0x2dbec4;g.look20=vf0;pc=0x2dbecc;g.up30=vf0;
        //Read same actual immutable startup owner; no global-null/BSS guess.
        pc=0x2dbed8;g.position00=origin;pc=0x2dbee4;g.box10=origin;
        pc=0x2dbef0;g.look20=origin;pc=0x2dbefc;g.up30=origin;
        pc=0x2dbf00;g.yaw40=0;pc=0x2dbf04;g.pitch44=0;pc=0x2dbf08;g.roll48=0;pc=0x2dbf0c;g.radius4C=0;
        f.geometryConstructed=true;
    }
    pc=0x2dbf10;f.sourceGeometry64=f.geometry64.get();f.geometryPublished=true; //Actual descriptor64 nullable publication.
    auto* g=*f.sourceGeometry64;pc=0x2dbf2c;
    if(!read.vector("position",origin,g?&g->position00:nullptr,found))return stop(pc);
    pc=0x2dbf48;if(!read.vector("box",origin,g?&g->box10:nullptr,found))return stop(pc);
    //Position actual present guarantees any NULL geometry has already stopped
    //at158C7C; do not manufacture defaults to bypass that first source store.
    if(!g)return stop(0x2dbf50);
    pc=0x2dbf60;if(!read.number("yaw",0,g->yaw40,found))return stop(pc);
    pc=0x2dbf78;if(!read.number("pitch",0,g->pitch44,found))return stop(pc);
    pc=0x2dbf90;if(!read.number("roll",0,g->roll48,found))return stop(pc);
    sourceTriggerPhase_=SourceTriggerPhase::prefixComplete;pc=sourceTriggerPC_=0x2dbf98;return true;
}
} //namespace ra_cold_aggregate50LA

#pragma once
#include "ra_static_model_cold50la.h"
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldStaticModelsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceStaticPhase_==SourceStaticPhase::unreached)return !sourceStaticOwner_;
    if(!sourceStaticOwner_)return sourceStaticPhase_==SourceStaticPhase::nativeClosed;
    if(!sourceStaticOwner_->sourceMode_||sourceStaticOwner_->application_!=worldApplication_||
       sourceStaticOwner_->generation_!=worldEntity_.sessionGeneration||!sourceStaticOwner_->canCloseSourceNative()||
       world_||runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||
       (sourceStaticPhysicsArg_&&sourceStaticPhysicsArg_!=worldPhysics3D4_.get())||
       (sourceStaticConfigArg_&&sourceStaticConfigArg_!=sourceWorldConfig_.get())||
       (sourceStaticZoneArg_&&(!sourceZones350_||sourceStaticZoneArgIndex_>=10||
           sourceStaticZoneArg_!=&sourceZones350_[sourceStaticZoneArgIndex_])))return false;
    if(sourceStaticPhase_==SourceStaticPhase::nativeAdmitted)
        return !sourceModelOwner3C4_&&!sourceModelParents3C4_&&!modelParents3C4Attempted_;
    return sourceModelOwner3C4_==sourceStaticOwner_.get()&&modelParents3C4Current(sourceStaticOwner_.get(),false);
}
inline bool Owner::abortSourceWorldStaticModelsNative(std::uint32_t& pc)noexcept {
    //A later actual271A00 frame retains typed native stack identities. Its
    //withdrawal must precede any raw group/descriptor owner destruction.
    if(sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::unreached&&
       sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::nativeClosed){pc=0;return false;}
    pc=0;if(!preflightSourceWorldStaticModelsNative())return false;
    if(!sourceStaticOwner_)return true;
    if(sourceModelOwner3C4_){
        //Native withdrawal before any slot/group/count release. Unlike legacy
        //withdrawModelParents3C4, do NOT manufacture guestNULL3C4 knowledge.
        sourceModels3C4_.reset();sourceModelParents3C4_.reset();
        sourceStaticOwner_->sourceWorld3C4Borrower_=nullptr;sourceModelOwner3C4_=nullptr;
        sourceModelParentsPublished3C4_=0;modelParents3C4Complete_=false;
    }
    if(!sourceStaticOwner_->closeSourceNative())std::terminate(); //Immutable preflight; no callback since.
    sourceStaticOwner_.reset();sourceStaticPhysicsArg_=nullptr;sourceStaticConfigArg_=nullptr;sourceStaticZoneArg_=nullptr;
    sourceStaticPhase_=SourceStaticPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldStaticModels2710F0(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceStaticPhase_!=SourceStaticPhase::unreached||!sourceWorldStaticModelsInvocationCurrent()||
       world_||sourceStaticOwner_||!sourceWorldConfig_||!worldApplication_||!worldApplication_->application10()||
       !sourceModels3C4_||*sourceModels3C4_||sourceModelParents3C4_||sourceModelOwner3C4_||modelParents3C4Attempted_||
       !worldZones34C_||*worldZones34C_!=10||!sourceZones350_||sourceZonesLoaded_!=10||
       !worldZoneInfo350_||*worldZoneInfo350_!=sourceZones350_.get()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    const auto& zone=sourceZones350_[0];
    if(!zone.loaded||zone.index00!=0||zone.staticCount108!=467||
       !(zone.knownFlags148&4)||zone.nameKnown!=6||std::memcmp(zone.name04.data(),"Zone1",6)!=0||
       (zone.flags148&4))return false;
    std::array<std::uint32_t,4> origin,forward,up;
    if(!copySourceWorldStartDefaults(origin,forward)||!copySourceWorldUpDefault(up))return false;
    if(worldApplication_->attachmentLock423F58()||worldApplication_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
    //Native owner/ledger admission before original271124. No ready source_,
    //materials, world metadata, geometry, Scene or renderer is substituted.
    try{sourceStaticOwner_.reset(new ra_static_model_cold50LA::Owner);}catch(...){return false;}
    sourceStaticPhase_=SourceStaticPhase::nativeAdmitted;
    if(!sourceStaticOwner_->admitSourceNative(worldApplication_,worldEntity_.sessionGeneration)){
        //Initial native admission may reject BEFORE sourceMode/App binding.
        //That untouched default Owner has no guest fields/resources/counts.
        //Dispose it separately; never leave an unabortable retained wrapper.
        if(!sourceStaticOwner_->sourceMode_){sourceStaticOwner_.reset();sourceStaticPhase_=SourceStaticPhase::nativeClosed;}
        return false; //Reserve failure AFTER binding stays retained/abortable.
    }
    auto& owner=*sourceStaticOwner_;using Parent=ra_static_model_cold50LA::Owner::Parent;
    using Wrapper=ra_static_model_cold50LA::Owner::StandardWrapper;
    sourceStaticPhase_=SourceStaticPhase::entered;modelParents3C4Attempted_=true;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceStaticPC_=modelParents3C4PC_=at;sourceStaticPhase_=SourceStaticPhase::bounded;return false;};
    pc=0x271114;sourceStaticFactoryVtable_=0x3d2630;
    pc=0x271120;sourceStaticSucceededZones_=0;
    pc=0x271124;try{sourceModelParents3C4_.reset(new Entity*[10]);}catch(...){sourceModelParents3C4_.reset();}
    pc=0x27112c;sourceModels3C4_=sourceModelParents3C4_.get();
    sourceModelOwner3C4_=&owner;owner.sourceWorld3C4Borrower_=this;
    pc=0x271130;sourceStaticZoneIndex_=0;sourceStaticZonePointerOffset_=0;sourceStaticZoneByteOffset_=0;
    pc=0x271140;std::unique_ptr<Parent> pending;
    try{pending.reset(new Parent);}catch(...){pending.reset();}owner.sourceOuterReturned_=true;owner.sourceGroupOuterReturned_[0]=true;
    if(pending){pending->sourceMode=true;pending->zone=1;owner.parents_.push_back(std::move(pending));
        auto& p=*owner.parents_[0];pc=0x27b61c;
        const auto r=g::initializeOriginalWreckComponent50LA(p.entity,g::OriginalWreckComponentKind50LA::staticModels27B610,
            owner.generation_,owner.key(&p.entity),true,worldApplication_->objectCount423F60());
        if(r.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(r.sourcePC);
        p.constructed=true;pc=0x27b62c;p.sourceVtable08=0x3d2640;
        pc=0x27b630;p.count30=0;p.sourceCountKnown=true;pc=0x27b634;p.sourceEntries34.reset();
    }
    pc=0x27116c;if(!sourceModelParents3C4_)return stop(pc); //Counted orphan retained, not erased.
    sourceModelParents3C4_[0]=owner.parents_.empty()?nullptr:&owner.parents_[0]->entity;
    sourceModelParentsPublished3C4_=1;
    pc=0x271178;sourceStaticPhysicsArg_=worldPhysics3D4_.get();
    pc=0x27117c;sourceStaticConfigArg_=sourceWorldConfig_.get();
    pc=0x271188;if(owner.parents_.empty())return stop(pc);
    pc=0x271194;sourceStaticZoneArg_=&zone;sourceStaticZoneArgIndex_=0;
    auto& p=*owner.parents_[0];
    pc=0x27b780;p.count30=static_cast<std::uint32_t>(zone.staticCount108);
    pc=0x27b78c;sourceStaticPhysicsCount_=0;
    pc=0x27b79c;try{p.sourceEntries34.reset(new Wrapper*[p.count30]);}catch(...){p.sourceEntries34.reset();}
    pc=0x27b7a4;p.sourceArrayReturned=true;
    pc=0x27b7a8;sourceStaticModelIndex_=0;sourceStaticSlotByteOffset_=0;
    //Actual known bit2 chooses%s-Model%d. Fixed buffers avoid host allocation;
    //same source name/ordinal were authenticated above, not caller-injected text.
    std::array<char,260> section;pc=0x27b7f4;
    std::memcpy(section.data(),zone.name04.data(),5);std::memcpy(section.data()+5,"-Model1",8);
    if(std::strcmp(section.data(),"Zone1-Model1")!=0)return stop(0x27b804);
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x27b804;if(!config.selectSourceFirstStaticModel157D00(returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x27b81c); //Before unjoined logging/skip; source slot remains UNKNOWN.
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
        if(!config.readSourceFirstStaticModelField158240(key,value,found,copy,d)){
            pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};std::array<char,260> type;
    pc=0x27b844;if(!read.text("statictype","Standard",type.data(),type.size(),returned))return stop(pc);
    pc=0x27bae0;if(std::strcmp(type.data(),"Standard")!=0)return stop(0x27bb10);
    const std::uint32_t sourceType=1;sourceStaticResolvedType_=sourceType; //Actual27BB0C/27B860, not wrapper00 yet.
    pc=0x27b9f4;try{p.sourceFirstWrapper.reset(new Wrapper);}catch(...){p.sourceFirstWrapper.reset();}
    if(p.sourceFirstWrapper){auto& w=*p.sourceFirstWrapper;w.sourceMode=true;
        pc=0x27ba0c;w.sourceVtable04=0x3d2710;pc=0x27ba18;w.type00=0;
        pc=0x27ba1c;w.sourceVtable04=0x3d26f0;w.dispatch=Wrapper::Dispatch::standard3D26F0;
        pc=0x27ba20;w.model8=nullptr;pc=0x27ba24;w.descriptor10.reset(); //0C remains UNKNOWN.
    }
    pc=0x27ba2c;if(!p.sourceEntries34)return stop(pc);
    p.sourceEntries34[0]=p.sourceFirstWrapper.get();p.sourceEntriesKnown=1;
    pc=0x27bac4;if(!p.sourceFirstWrapper)return stop(pc);
    p.sourceFirstWrapper->type00=sourceType;p.sourceFirstWrapper->sourceTypeKnown=true;
    pc=0x27b894;
    if(!loadSourceFirstStaticDescriptor27AB60(pc))return stop(pc);
    //Descriptor's true v0C return is not the later model initializer27ABC0.
    //Bound before loop advances to the next source model/section.
    owner.sourceFirstDescriptorReturned_=true;sourceStaticPhase_=SourceStaticPhase::prefixComplete;
    pc=sourceStaticPC_=modelParents3C4PC_=0x27b89c;return true;
}
inline bool Owner::loadSourceFirstStaticDescriptor27AB60(std::uint32_t& pc)noexcept {
    return loadSourceStaticDescriptor27AB60(0,0,true,pc);
}
inline bool Owner::loadSourceStaticDescriptor27AB60(std::size_t zoneIndex,std::size_t modelIndex,
    bool initialFirst,std::uint32_t& pc)noexcept {
    using Descriptor=ra_static_model_cold50LA::Owner::Descriptor;
    auto& owner=*sourceStaticOwner_;auto& p=*owner.parents_[zoneIndex];
    auto& w=initialFirst?*p.sourceFirstWrapper:*owner.sourceWrapperLedger_[zoneIndex][modelIndex];
    if(p.sourcePendingDescriptor||w.descriptor10||w.sourceDescriptorAllocationReturned||w.sourceDescriptorPublished){pc=0x27ab60;return false;}
    std::array<std::uint32_t,4> origin,forward,up;
    //These are SAME startup-owned readonly globals. Caller already admitted
    //them before source interval; reads here do not create replacement defaults.
    if(!copySourceWorldStartDefaults(origin,forward)||!copySourceWorldUpDefault(up)){pc=0x27ab60;return false;}
    pc=0x27ab78;
    try{p.sourcePendingDescriptor.reset(new Descriptor);}catch(...){p.sourcePendingDescriptor.reset();}
    w.sourceDescriptorAllocationReturned=true;
    if(p.sourcePendingDescriptor){
        p.sourcePendingDescriptor->sourceFields.emplace();auto& f=*p.sourcePendingDescriptor->sourceFields;
        pc=0x27a11c;f.vtable170=0x3d2760;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        pc=0x27a124;f.position110=vf0;f.vectorKnown|=1;
        pc=0x27a12c;f.look120=vf0;f.vectorKnown|=2;
        pc=0x27a134;f.up130=vf0;f.vectorKnown|=4;
        pc=0x27a13c;f.smokeOffset150=vf0;f.vectorKnown|=8;
        pc=0x27a144;f.smokeVelocity160=vf0;f.vectorKnown|=16;
        //Actual constant260!=4 skips both constructor assertion guards.
        pc=0x27a1f0;std::memcpy(f.name00.data(),"none",4);
        pc=0x27a1fc;f.name00[4]=0;f.name00Known=5;
        pc=0x27a210;f.position110=origin;pc=0x27a21c;f.look120=forward;
        pc=0x27a228;f.up130=up;
        pc=0x27a22c;f.useLighting140=1;pc=0x27a230;f.dynamic144=0;
        pc=0x27a234;f.shadow148=1;pc=0x27a238;f.smoker14C=0;
        pc=0x27a244;f.smokeOffset150=origin;pc=0x27a250;f.smokeVelocity160=origin;
        pc=0x27a400;f.vtable170=0x3d2750;
        pc=0x27a4ac;std::memcpy(f.name180.data(),"none",4);
        pc=0x27a4b8;f.name180[4]=0;f.name180Known=5;f.constructed=true;
    }
    pc=0x27ab90;w.descriptor10=std::move(p.sourcePendingDescriptor);w.sourceDescriptorPublished=true; //Actual NULL included.
    pc=0x27ab98;if(!w.descriptor10)return false;
    auto& f=*w.descriptor10->sourceFields;auto& config=*sourceWorldConfig_;
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
        if(!(initialFirst?config.readSourceFirstStaticModelField158240(key,value,found,copy,d):
             config.readSourceStaticModelField158240(key,value,found,copy,d))){
            pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};std::array<char,260> slt;
    pc=0x27a514;if(!read.text("slt","none",slt.data(),slt.size(),returned))return false;
    pc=0x27a524;std::size_t count=0;while(slt[count]&&slt[count]!='.')++count;
    if(count>50)count=50;
    pc=0x27a550;std::memcpy(f.name00.data(),slt.data(),count);
    pc=0x27a55c;f.name00[count]=0;f.name00Known=count+1;
    pc=0x27a574;if(!read.vector("position",origin,&f.position110,returned))return false;
    pc=0x27a590;if(!read.vector("lookvector",forward,&f.look120,returned))return false;
    pc=0x27a5ac;if(!read.vector("upvector",up,&f.up130,returned))return false;
    std::int32_t value;
    pc=0x27a5c4;if(!read.integer("uselighting",1,&value,returned))return false;f.useLighting140=value;
    pc=0x27a5dc;if(!read.integer("usedynamiclighting",0,&value,returned))return false;f.dynamic144=value;
    pc=0x27a5f4;if(!read.integer("issmoker",0,&value,returned))return false;f.smoker14C=value;
    pc=0x27a610;if(!read.vector("smokeoffset",origin,&f.smokeOffset150,returned))return false;
    pc=0x27a62c;if(!read.vector("smokevelocity",origin,&f.smokeVelocity160,returned))return false;
    f.loaded=true;pc=0x27b89c;return true;
}
} //namespace ra_cold_aggregate50LA

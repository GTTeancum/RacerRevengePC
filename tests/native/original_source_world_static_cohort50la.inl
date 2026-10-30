#pragma once
#include <cstdio>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceWorldStaticModels27B89C(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceStaticPhase_!=SourceStaticPhase::prefixComplete||!sourceWorldStaticCohortInvocationCurrent()||
       !preflightSourceWorldStaticModelsNative()||!sourceStaticOwner_||!sourceWorldConfig_||
       sourceStaticZoneIndex_||sourceStaticModelIndex_||sourceStaticPhysicsCount_||sourceStaticSucceededZones_||
       sourceStaticZonePointerOffset_||sourceStaticZoneByteOffset_||sourceStaticSlotByteOffset_||
       !sourceStaticResolvedType_||*sourceStaticResolvedType_!=1||
       !sourceStaticFactoryVtable_||*sourceStaticFactoryVtable_!=0x3d2630||
       !sourceModelParents3C4_||sourceModelParentsPublished3C4_!=1||!sourceZones350_||sourceZonesLoaded_!=10)return false;
    auto& owner=*sourceStaticOwner_;
    if(owner.sourceLedgerAttempted_||!owner.sourceFirstDescriptorReturned_||owner.parents_.size()!=1||
       !owner.parents_[0]||!owner.parents_[0]->sourceFirstWrapper||
       !owner.parents_[0]->sourceFirstWrapper->descriptor10||
       !owner.parents_[0]->sourceFirstWrapper->descriptor10->sourceFields||
       !owner.parents_[0]->sourceFirstWrapper->descriptor10->sourceFields->loaded)return false;
    static constexpr std::uint32_t counts[10]={467,95,33,72,23,247,33,122,104,65};
    static constexpr const char* names[10]={"Zone1","Zone2","Zone3","Zone4","Zone5","Zone6","Zone7","Zone8","Zone9","Zone10"};
    for(std::size_t z=0;z<10;++z){const auto& zone=sourceZones350_[z];
        if(!zone.loaded||zone.index00!=static_cast<std::int32_t>(z)||zone.staticCount108!=static_cast<std::int32_t>(counts[z])||
           !(zone.knownFlags148&4)||(zone.flags148&4)||zone.nameKnown!=std::strlen(names[z])+1||
           std::memcmp(zone.name04.data(),names[z],zone.nameKnown)!=0)return false;
    }
    if(worldApplication_->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-9u)return false;
    //Pure native ledger admission at the retained first-descriptor boundary.
    //These empty unique_ptrs are NOT raw source34 NULL publications. If any
    //resize fails, earlier ledger allocations stay owned and source stays27B89C.
    owner.sourceLedgerAttempted_=true;
    try{for(std::size_t z=0;z<10;++z){owner.sourceZoneCounts_[z]=counts[z];owner.sourceWrapperLedger_[z].resize(counts[z]);}}
    catch(...){return false;}
    owner.sourceWrapperLedger_[0][0]=std::move(owner.parents_[0]->sourceFirstWrapper);
    owner.sourceFirstTransferred_=true;owner.sourceLedgerPrepared_=true;
    sourceStaticPhase_=SourceStaticPhase::continuing;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceStaticPC_=modelParents3C4PC_=at;sourceStaticPhase_=SourceStaticPhase::bounded;return false;};
    using Parent=ra_static_model_cold50LA::Owner::Parent;using Wrapper=ra_static_model_cold50LA::Owner::StandardWrapper;
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;
    //Native bounded formatter for actual source%s-Model%d. Its output is
    //qualified against real current Zone04/index/ordinal before Config selects.
    const auto formatName=[](char* out,const char* zone,std::uint32_t ordinal)noexcept {
        std::size_t at=0;while(*zone)out[at++]=*zone++;
        const char* sep="-Model";while(*sep)out[at++]=*sep++;
        char digits[10];std::size_t n=0;do{digits[n++]=char('0'+ordinal%10);ordinal/=10;}while(ordinal);
        while(n)out[at++]=digits[--n];out[at]=0;
    };
    const auto loadRow=[&]()noexcept {
        const auto zi=sourceStaticZoneIndex_,mi=sourceStaticModelIndex_;
        if(zi>=10||mi>=owner.sourceZoneCounts_[zi]){pc=0x27b7b4;return false;}
        auto& p=*owner.parents_[zi];const auto& zone=sourceZones350_[zi];
        if(zone.nameKnown!=std::strlen(names[zi])+1||std::memcmp(zone.name04.data(),names[zi],zone.nameKnown)!=0||
           !(zone.knownFlags148&4)||(zone.flags148&4)){pc=0x27b7b4;return false;}
        std::array<char,260> section,expected;pc=0x27b7f4;formatName(section.data(),zone.name04.data(),mi+1);
        formatName(expected.data(),names[zi],mi+1);
        if(std::strcmp(section.data(),expected.data())!=0){pc=0x27b804;return false;}
        bool returned=false;pc=0x27b804;
        if(!config.selectSourceStaticModel157D00(zi,mi,returned,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}
        if(!returned){pc=0x27b81c;return false;} //Actual missing-log/skip remains unjoined; slot UNKNOWN.
        auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
            if(!config.readSourceStaticModelField158240(key,value,found,copy,d)){
                pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
        source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};std::array<char,260> type;
        pc=0x27b844;if(!read.text("statictype","Standard",type.data(),type.size(),returned))return false;
        pc=0x27bae0;
        if(std::strcmp(type.data(),"Standard")==0){pc=0x27bb0c;sourceStaticResolvedType_=1;}
        else {pc=0x27bb18;if(std::strcmp(type.data(),"PhysicsObject")!=0){pc=0x27bb2c;return false;}
            pc=0x27bb2c;sourceStaticResolvedType_=2;}
        pc=0x27b860; //Actual resolved type stack lane, not yet wrapper00.
        if(*sourceStaticResolvedType_==2){
            if(!constructSourcePhysicsWrapper27BA3C(zi,mi,pc)||!loadSourcePhysicsDescriptor27B370(zi,mi,pc))return false;
            pc=0x27b89c;return true;
        }
        auto& owned=owner.sourceWrapperLedger_[zi][mi];
        pc=0x27b9f4;if(owned)return false;
        pc=0x27b9f4;try{owned.reset(new Wrapper);}catch(...){owned.reset();}
        if(owned){auto& w=*owned;w.sourceMode=true;
            pc=0x27ba0c;w.sourceVtable04=0x3d2710;pc=0x27ba18;w.type00=0;
            pc=0x27ba1c;w.sourceVtable04=0x3d26f0;w.dispatch=Wrapper::Dispatch::standard3D26F0;
            pc=0x27ba20;w.model8=nullptr;pc=0x27ba24;w.descriptor10.reset();
        }
        pc=0x27ba2c;if(!p.sourceEntries34)return false;
        p.sourceEntries34[mi]=owned.get();++p.sourceEntriesKnown;
        pc=0x27bac4;if(!owned)return false;owned->type00=*sourceStaticResolvedType_;owned->sourceTypeKnown=true;
        pc=0x27b894;return loadSourceStaticDescriptor27AB60(zi,mi,false,pc);
    };
    for(;;){
        auto& p=*owner.parents_[sourceStaticZoneIndex_];const auto& zone=sourceZones350_[sourceStaticZoneIndex_];
        pc=0x27b89c;if(!sourceStaticResolvedType_||(*sourceStaticResolvedType_!=1&&*sourceStaticResolvedType_!=2))return stop(pc);
        if(*sourceStaticResolvedType_==2){
            auto& w=owner.sourceWrapperLedger_[sourceStaticZoneIndex_][sourceStaticModelIndex_];
            if(!w||!owner.sourcePhysicsWrapperCurrentNative(*w)||!w->sourcePhysics.descriptorReadReturned||
               w->sourcePhysics.countedInParent||sourceStaticPhysicsCount_>=p.count30)return stop(0x27b8ac);
            pc=0x27b8ac;++sourceStaticPhysicsCount_;++owner.sourcePhysicsCounts_[sourceStaticZoneIndex_];
            w->sourcePhysics.countedInParent=true; //Integer S1 count only, no Entity retain/count.
        }
        pc=0x27b8b0;sourceStaticSlotByteOffset_+=4;
        pc=0x27b8b4;++sourceStaticModelIndex_;
        pc=0x27b8b8;
        if(sourceStaticModelIndex_<p.count30){if(!loadRow())return stop(pc);continue;}
        if(sourceStaticModelIndex_!=p.count30)return stop(0x27b8b8);
        //Explicit native diagnostic adapter for original1336C0, NOT a source
        //stdio no-op. Format3BB370 is exact; actual loop counter and Zone04
        //supply arguments. One latched fwrite; short/error return is ignored
        //just as source27B8D4 ignores v0. No retry repeats an output attempt.
        pc=0x27b8c8;
        if(zone.nameKnown!=std::strlen(names[sourceStaticZoneIndex_])+1||
           std::memcmp(zone.name04.data(),names[sourceStaticZoneIndex_],zone.nameKnown)!=0)return stop(pc);
        std::array<char,320> message;std::size_t length=0;
        const auto append=[&](const char* text)noexcept{while(*text)message[length++]=*text++;};
        append("Read ");char digits[10];std::size_t nd=0;auto number=sourceStaticModelIndex_;
        do{digits[nd++]=char('0'+number%10);number/=10;}while(number);
        while(nd)message[length++]=digits[--nd];append(" static models from ");append(zone.name04.data());append(".\n");
        const auto zi=sourceStaticZoneIndex_;
        pc=0x27b8d4;if(owner.sourceLogAttempted_[zi])return stop(pc);
        owner.sourceLogAttempted_[zi]=true;
        owner.sourceLogWritten_[zi]=std::fwrite(message.data(),1,length,stdout);
        pc=0x27b8dc;
        if(sourceStaticPhysicsCount_){
            if(!worldPhysics3D4_||!sourcePhysics3D4_||*sourcePhysics3D4_!=worldPhysics3D4_.get()||
               sourceStaticPhysicsArg_!=worldPhysics3D4_.get()||zone.index00!=static_cast<std::int32_t>(zi))return stop(0x27b8ec);
            bool returned=false;std::uint32_t ignoredV0=0;pc=0x27b8ec;
            if(!worldPhysics3D4_->reserveSource27C1D0(worldEntity_,static_cast<std::uint32_t>(zone.index00),
                static_cast<std::int32_t>(sourceStaticPhysicsCount_),returned,ignoredV0,pc)||!returned)return stop(pc);
            //Original ignores27C1D0 V0, including actual0. A native refusal
            //above does not fabricate either that return or parent success.
            pc=0x27b8f4;
        }
        //True27B740 return1, then actual outer branch-delay success increment.
        pc=0x27119c;++sourceStaticSucceededZones_;
        pc=0x2711d0;sourceStaticZonePointerOffset_+=4;
        pc=0x2711d4;sourceStaticZoneByteOffset_+=0x160;
        pc=0x2711d8;++sourceStaticZoneIndex_;
        pc=0x2711e0;
        if(sourceStaticZoneIndex_==10){
            if(sourceStaticSucceededZones_!=10)return stop(0x2711f0);
            //Positive successes preserve source3C4 array (no271200NULL store).
            owner.sourceAllDescriptorsReturned_=true;sourceStaticPhase_=SourceStaticPhase::returned;
            pc=sourceStaticPC_=modelParents3C4PC_=0x270d0c;return true;
        }
        const auto next=sourceStaticZoneIndex_;pc=0x271140;std::unique_ptr<Parent> pending;
        try{pending.reset(new Parent);}catch(...){pending.reset();}owner.sourceGroupOuterReturned_[next]=true;
        owner.parents_.push_back(std::move(pending)); //Native capacity10 was admitted before first source allocation.
        if(owner.parents_[next]){auto& parent=*owner.parents_[next];parent.sourceMode=true;parent.zone=next+1;
            pc=0x27b61c;const auto r=g::initializeOriginalWreckComponent50LA(parent.entity,g::OriginalWreckComponentKind50LA::staticModels27B610,
                owner.generation_,owner.key(&parent.entity),true,worldApplication_->objectCount423F60());
            if(r.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(r.sourcePC);
            parent.constructed=true;pc=0x27b62c;parent.sourceVtable08=0x3d2640;
            pc=0x27b630;parent.count30=0;parent.sourceCountKnown=true;pc=0x27b634;parent.sourceEntries34.reset();
        }
        pc=0x27116c;sourceModelParents3C4_[next]=owner.parents_[next]?&owner.parents_[next]->entity:nullptr;
        ++sourceModelParentsPublished3C4_;
        pc=0x271178;sourceStaticPhysicsArg_=worldPhysics3D4_.get();pc=0x27117c;sourceStaticConfigArg_=sourceWorldConfig_.get();
        pc=0x271188;if(!owner.parents_[next])return stop(pc);
        pc=0x271194;sourceStaticZoneArg_=&sourceZones350_[next];sourceStaticZoneArgIndex_=next;
        auto& parent=*owner.parents_[next];pc=0x27b780;parent.count30=static_cast<std::uint32_t>(sourceZones350_[next].staticCount108);
        if(parent.count30!=owner.sourceZoneCounts_[next])return stop(0x27b784);
        pc=0x27b78c;sourceStaticPhysicsCount_=0;sourceStaticPhysicsCountZone_=next;
        pc=0x27b79c;try{parent.sourceEntries34.reset(new Wrapper*[parent.count30]);}catch(...){parent.sourceEntries34.reset();}
        pc=0x27b7a4;parent.sourceArrayReturned=true;
        pc=0x27b7a8;sourceStaticModelIndex_=0;sourceStaticSlotByteOffset_=0;
        if(!loadRow())return stop(pc);
    }
}
} //namespace ra_cold_aggregate50LA

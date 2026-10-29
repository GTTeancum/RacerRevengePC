#pragma once
#include "original_source_world_light_read50la.inl"
#include <cstdio>
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldCollisionNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceCollisionPhase_==SourceCollisionPhase::unreached)return true; //Legacy backing belongs to its own closer.
    if(!worldCollisionObjects3CC_)return sourceCollisionPhase_==SourceCollisionPhase::nativeClosed;
    if(!worldCollisionObjects3CC_->sourceMode_||runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_||
       sourceCollisionZoneArgIndex_>10||(sourceCollisionZoneArgIndex_<10&&
       (!sourceZones350_||sourceCollisionZoneArg_!=&sourceZones350_[sourceCollisionZoneArgIndex_]))||
       (sourceCollisionConfigArg_&&sourceCollisionConfigArg_!=sourceWorldConfig_.get()))return false;
    std::uint32_t pc=0;return canReleaseWorldCollisionObjects27BB80(pc);
}
inline bool Owner::abortSourceWorldCollisionNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldCollisionNative())return false;
    if(sourceCollisionPhase_==SourceCollisionPhase::unreached||sourceCollisionPhase_==SourceCollisionPhase::nativeClosed)return true;
    if(!releaseWorldCollisionObjects27BB80(pc))return false;
    sourceCollisionConfigArg_=nullptr;sourceCollisionZoneArg_=nullptr;sourceCollisionZoneArgIndex_=10;
    sourceCollisionPhase_=SourceCollisionPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldCollisionObjects1B4EC0(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||sourceCollisionPhase_!=SourceCollisionPhase::unreached||
       !sourceWorldCollisionInvocationCurrent()||worldCollisionObjects3CC_||sourceCollisionObjects3CC_||sourceCollisionObjectsAttempted_||worldCollisionResources_||
       nativeCollisionModelBorrow_||!worldApplication_||!worldApplication_->application10()||!sourceWorldConfig_||
       !worldZones34C_||*worldZones34C_!=10||!sourceZones350_||sourceZonesLoaded_!=10||
       !worldZoneInfo350_||*worldZoneInfo350_!=sourceZones350_.get()||
       runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_)return false;
    static constexpr std::uint32_t counts[10]={467,95,33,72,23,247,33,122,104,65};
    static constexpr const char* names[10]={"Zone1","Zone2","Zone3","Zone4","Zone5","Zone6","Zone7","Zone8","Zone9","Zone10"};
    for(std::size_t z=0;z<10;++z){const auto& in=sourceZones350_[z];
        if(!in.loaded||in.index00!=static_cast<std::int32_t>(z)||!in.collisionCount10C||
           *in.collisionCount10C!=static_cast<std::int32_t>(counts[z])||(in.knownFlags148&5)!=5||(in.flags148&5)||
           in.nameKnown!=std::strlen(names[z])+1||std::memcmp(in.name04.data(),names[z],in.nameKnown))return false;
    }
    std::array<std::uint32_t,4> origin,forward,up;
    if(!copySourceWorldStartDefaults(origin,forward)||!copySourceWorldUpDefault(up)||!reserveSourceCollisionAuthorityNative())return false;
    using Objects=g::OriginalWorldCollisionObjects50LA;using Zone=Objects::Zone;using Wrapper=Objects::Wrapper;
    //Only native ledger storage is preadmitted; no actual source pointer slot
    //is zeroed here. Allocation failure is native refusal before1B4EF4.
    try{worldCollisionObjects3CC_.reset(new Objects);}catch(...){return false;}
    auto& owner=*worldCollisionObjects3CC_;owner.sourceMode_=true;sourceCollisionPhase_=SourceCollisionPhase::nativeAdmitted;
    try{for(std::size_t z=0;z<10;++z)owner.sourceWrappers_[z].resize(counts[z]);}catch(...){return false;}
    sourceCollisionPhase_=SourceCollisionPhase::entered;sourceCollisionObjectsAttempted_=true;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceCollisionPC_=at;sourceCollisionPhase_=SourceCollisionPhase::bounded;return false;};
    pc=0x1b4ef4;try{owner.sourceArray3CC_.reset(new Entity*[10]);}catch(...){owner.sourceArray3CC_.reset();}
    pc=0x1b4efc;owner.sourceArrayReturned_=true;sourceCollisionObjects3CC_=owner.sourceArray3CC_.get();
    sourceCollisionZoneIndex_=0;sourceCollisionSucceeded_=0;
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;
    const auto format=[](char* out,const char* zone,std::uint32_t ordinal)noexcept{
        std::size_t n=0;while(*zone)out[n++]=*zone++;const char* sep="-Model";while(*sep)out[n++]=*sep++;
        char digits[10];std::size_t k=0;do{digits[k++]=char('0'+ordinal%10);ordinal/=10;}while(ordinal);
        while(k)out[n++]=digits[--k];out[n]=0;
    };
    for(std::size_t z=0;z<10;++z){sourceCollisionZoneIndex_=z;
        pc=0x1b4f10;try{owner.sourceZones_[z].reset(new Zone);}catch(...){owner.sourceZones_[z].reset();}
        owner.sourceGroupAllocationReturned_[z]=true;
        if(owner.sourceZones_[z]){owner.sourceZones_[z]->sourceMode=true;
            if(!constructSourceCollisionGroup27BB40(z,pc))return stop(pc);}
        pc=0x1b4f3c;if(!owner.sourceArray3CC_)return stop(pc); //Counted orphan survives NULL outer array.
        owner.sourceArray3CC_[z]=owner.sourceZones_[z]?&owner.sourceZones_[z]->entity:nullptr;++owner.sourceArrayKnown_;
        pc=0x1b4f54;if(!owner.sourceZones_[z])return stop(pc);
        sourceCollisionConfigArg_=&config;sourceCollisionZoneArg_=&sourceZones350_[z];sourceCollisionZoneArgIndex_=z;
        auto& group=*owner.sourceZones_[z];const auto& in=sourceZones350_[z];
        pc=0x27bcd0;group.count30=static_cast<std::uint32_t>(*in.collisionCount10C);
        pc=0x27bcec;try{group.sourceEntries34.reset(new Wrapper*[group.count30]);}catch(...){group.sourceEntries34.reset();}
        pc=0x27bcf4;group.sourceArrayReturned=true;
        for(std::size_t n=0;n<group.count30;++n){sourceCollisionObjectIndex_=n;
            std::array<char,260> section,expected;format(section.data(),in.name04.data(),n+1);format(expected.data(),names[z],n+1);
            if(std::strcmp(section.data(),expected.data()))return stop(0x27bdac);
            bool returned=false;pc=0x27bdac;
            if(!config.selectSourceCollisionDescriptor157D00(z,n,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
            if(!returned)return stop(0x27bdc4); //Before missing log/skip; slot stays UNKNOWN, not NULL.
            auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
                if(!config.readSourceCollisionDescriptorField158240(key,value,found,copy,d)){
                    pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
            source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};std::array<char,260> type;
            pc=0x27bdec;if(!read.text("statictype","Standard",type.data(),type.size(),returned))return stop(pc);
            pc=0x27bfb0;if(std::strcmp(type.data(),"Standard"))return stop(0x27bfe0);
            sourceCollisionResolvedType_=1; //Real resolver result, not a metadata classification.
            auto& owned=owner.sourceWrappers_[z][n];pc=0x1b58fc;
            try{owned.reset(new Wrapper);}catch(...){owned.reset();}
            if(owned){auto& w=*owned;w.sourceMode=true;
                pc=0x27afa8;w.sourceVtable0C=0x3d26e0;pc=0x27afac;w.type00=0;
                pc=0x27afb0;w.collision04=nullptr;pc=0x27afb4;w.descriptor08.reset();
                pc=0x1b591c;w.sourceVtable0C=0x3cf0f0;
            }
            pc=0x1b5920;if(!group.sourceEntries34)return stop(pc); //Acquired but unpublishable Wrapper retained.
            group.sourceEntries34[n]=owned.get();++group.sourceEntriesKnown;
            pc=0x1b592c;if(!owned)return stop(pc);owned->type00=sourceCollisionResolvedType_;owned->sourceTypeKnown=true;
            pc=0x27be3c;if(!loadSourceCollisionDescriptor1B5510(z,n,pc))return stop(pc);
        }
        sourceCollisionObjectIndex_=group.count30;
        //Actual1336C0 diagnostic, represented by a single latched native fwrite.
        //Short/error return is retained and ignored, never replayed on refusal.
        std::array<char,320> message;std::size_t length=0;
        const auto append=[&](const char* s)noexcept{while(*s)message[length++]=*s++;};
        append("Read ");char digits[10];std::size_t k=0;auto count=group.count30;
        do{digits[k++]=char('0'+count%10);count/=10;}while(count);while(k)message[length++]=digits[--k];
        append(" collision objects from ");append(in.name04.data());append(".\n");
        pc=0x27be6c;if(owner.sourceLogAttempted_[z])return stop(pc);owner.sourceLogAttempted_[z]=true;
        owner.sourceLogWritten_[z]=std::fwrite(message.data(),1,length,stdout);
        pc=0x1b4f68;++sourceCollisionSucceeded_; //27BCA0 returned1; no loader-zero destructor path reached.
    }
    if(sourceCollisionSucceeded_!=10)return stop(0x1b4fc0);
    //All ten positive successes preserve raw3CC. Outer1B4FEC does not synthesize
    //v0=1; caller ignores its return. This is actual v8C return only.
    sourceCollisionZoneIndex_=10;sourceCollisionPhase_=SourceCollisionPhase::returned;
    pc=sourceCollisionPC_=0x270d1c;return true;
}
inline bool Owner::loadSourceCollisionDescriptor1B5510(std::size_t z,std::size_t n,std::uint32_t& pc)noexcept {
    using Descriptor=g::OriginalWorldCollisionObjects50LA::Descriptor;
    auto& w=*worldCollisionObjects3CC_->sourceWrappers_[z][n];
    std::array<std::uint32_t,4> origin,forward,up;
    if(!copySourceWorldStartDefaults(origin,forward)||!copySourceWorldUpDefault(up)){pc=0x1b5510;return false;}
    pc=0x1b552c;std::unique_ptr<Descriptor> pending;
    try{pending.reset(new Descriptor);}catch(...){pending.reset();}
    if(pending){pending->sourceFields.emplace();auto& f=*pending->sourceFields;
        pc=0x27a11c;f.vtable170=0x3d2760;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        pc=0x27a124;f.position110=vf0;f.vectorKnown|=1;
        pc=0x27a12c;f.look120=vf0;f.vectorKnown|=2;
        pc=0x27a134;f.up130=vf0;f.vectorKnown|=4;
        pc=0x27a13c;f.smokeOffset150=vf0;f.vectorKnown|=8;
        pc=0x27a144;f.smokeVelocity160=vf0;f.vectorKnown|=16;
        //Actual constant260!=4 skips both constructor assertion guards.
        pc=0x27a1f0;std::memcpy(f.name00.data(),"none",4);
        pc=0x27a1fc;f.name00[4]=0;f.nameKnown=5;
        pc=0x27a210;f.position110=origin;pc=0x27a21c;f.look120=forward;
        pc=0x27a228;f.up130=up;
        pc=0x27a22c;f.useLighting140=1;pc=0x27a230;f.dynamic144=0;
        pc=0x27a234;f.shadow148=1;pc=0x27a238;f.smoker14C=0;
        pc=0x27a244;f.smokeOffset150=origin;pc=0x27a250;f.smokeVelocity160=origin;
        pc=0x27a85c;f.vtable170=0x3d2730;
        pc=0x27a860;f.field180=0;pc=0x27a864;f.field184=0;
        pc=0x1b5550;f.vtable170=0x3cf100;
        pc=0x1b5554;f.field180=0;pc=0x1b5558;f.field184=0;f.constructed=true;
    }
    pc=0x1b555c;w.descriptor08=std::move(pending);w.sourceDescriptorPublished=true;
    pc=0x1b5564;if(!w.descriptor08)return false;
    auto& f=*w.descriptor08->sourceFields;auto& config=*sourceWorldConfig_;
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
        if(!config.readSourceCollisionDescriptorField158240(key,value,found,copy,d)){
            pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};std::array<char,260> name;
    pc=0x27a2a4;if(!read.text("slt","none",name.data(),name.size(),returned))return false;
    pc=0x27a2b4;std::size_t count=0;while(name[count]&&name[count]!='.')++count;if(count>50)count=50;
    pc=0x27a2e0;std::memcpy(f.name00.data(),name.data(),count);
    pc=0x27a2ec;f.name00[count]=0;f.nameKnown=count+1;
    pc=0x27a304;if(!read.vector("position",origin,&f.position110,returned))return false;
    pc=0x27a320;if(!read.vector("lookvector",forward,&f.look120,returned))return false;
    pc=0x27a33c;if(!read.vector("upvector",up,&f.up130,returned))return false;
    std::int32_t value;
    pc=0x27a354;if(!read.integer("uselighting",1,&value,returned))return false;f.useLighting140=value;
    pc=0x27a36c;if(!read.integer("usedynamiclighting",0,&value,returned))return false;f.dynamic144=value;
    pc=0x27a384;if(!read.integer("issmoker",0,&value,returned))return false;f.smoker14C=value;
    pc=0x27a3a0;if(!read.vector("smokeoffset",origin,&f.smokeOffset150,returned))return false;
    pc=0x27a3bc;if(!read.vector("smokevelocity",origin,&f.smokeVelocity160,returned))return false;
    //Derived1B5430 repeats SLT on genuine absent COL, not a native alias to
    //the earlier lookup. All actual source reads and overwrite ordering survive.
    pc=0x1b5468;if(!read.text("col","none",name.data(),name.size(),returned))return false;
    if(!returned){pc=0x1b5490;if(!read.text("slt","none",name.data(),name.size(),returned))return false;}
    pc=0x1b54a0;count=0;while(name[count]&&name[count]!='.')++count;if(count>50)count=50;
    pc=0x1b54cc;std::memcpy(f.name00.data(),name.data(),count);
    pc=0x1b54d8;f.name00[count]=0;f.nameKnown=count+1;
    pc=0x1b54ec;if(!read.integer("materialid",1,&value,returned))return false;f.field184=value;
    f.loaded=true;pc=0x27be44;return true;
}
} //namespace ra_cold_aggregate50LA

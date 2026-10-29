#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldTerrainNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceTerrainPhase_==SourceTerrainPhase::unreached)
        return !nativeTerrain3B0_||!nativeTerrain3B0_->sourceMode;
    if(sourceTerrainPhase_==SourceTerrainPhase::nativeClosed)return !nativeTerrain3B0_&&!sourceTerrain3B0_;
    if(!nativeTerrain3B0_)return false;
    const auto& t=*nativeTerrain3B0_;
    return t.sourceMode&&t.sourceApplication==worldApplication_.get()&&worldApplication_&&
        t.sourceGeneration==worldEntity_.sessionGeneration&&t.sourceGeneration&&
        t.sourceConfig==sourceWorldObjectConfig_.get()&&t.sourceAllocationReturned&&t.sourcePublished&&
        sourceTerrain3B0_&&*sourceTerrain3B0_==t.sourcePayload.get()&&
        (!t.sourcePayload||(t.sourceVtableKnown&&t.sourceCountKnown&&t.sourceRowsConstructed==8))&&
        !t.count00&&!t.mappingsComplete&&!runtimeCameraBorrows_&&!runtimeWorldBorrows_&&
        !constructorRacerWorldBorrows_&&!nativeCollisionModelBorrow_&&!worldCollisionResources_;
}
inline bool Owner::abortSourceWorldTerrainNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldTerrainNative())return false;
    if(sourceTerrainPhase_==SourceTerrainPhase::unreached||sourceTerrainPhase_==SourceTerrainPhase::nativeClosed)return true;
    // All payload rows are plain uncounted scalars, no fallible destructor or
    // external registration. Withdrawal and destruction cannot refuse after
    // this same-thread preflight. Existing descriptor180 writes are NOT undone.
    sourceTerrain3B0_.reset();nativeTerrain3B0_.reset();
    sourceTerrainPhase_=SourceTerrainPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldTerrain271370(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceTerrainPhase_!=SourceTerrainPhase::unreached||!sourceWorldTerrainInvocationCurrent()||
       nativeTerrain3B0_||!sourceTerrain3B0_||*sourceTerrain3B0_||!sourceWorldObjectConfig_||
       !worldApplication_||!worldEntity_.sessionGeneration||!preflightSourceWorldCollisionNative()||
       sourceCollisionPhase_!=SourceCollisionPhase::returned||!worldCollisionObjects3CC_||
       !worldZones34C_||*worldZones34C_!=10||runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    // Native inline ledger admission is allocation-free and has no source
    // publication. Only the following new SourceTerrain represents malloc C8.
    nativeTerrain3B0_.emplace();auto& t=*nativeTerrain3B0_;
    t.sourceMode=true;t.sourceApplication=worldApplication_.get();t.sourceGeneration=worldEntity_.sessionGeneration;
    t.sourceConfig=sourceWorldObjectConfig_.get();sourceTerrainPhase_=SourceTerrainPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept {pc=sourceTerrainPC_=at;sourceTerrainPhase_=SourceTerrainPhase::bounded;return false;};
    pc=0x271388;try{t.sourcePayload.reset(new SourceTerrain276830);}catch(...){t.sourcePayload.reset();}
    t.sourceAllocationReturned=true;
    if(t.sourcePayload){auto& p=*t.sourcePayload;
        pc=0x276844;p.vtableC4=0x3d2b60;t.sourceVtableKnown=true;
        for(auto& r:p.rows04){pc=0x276708;r.vtable14=0x3d2b70;
            pc=0x276710;r.friction00=0x3f800000;pc=0x276714;r.drag04=0x3f800000;
            pc=0x276718;r.traction08=0x3f800000;pc=0x276720;r.dust0C=1;
            pc=0x276724;r.dirt10=1;++t.sourceRowsConstructed;}
        pc=0x276868;p.count00=0;t.sourceCountKnown=true;
    }
    pc=0x2713a0;sourceTerrain3B0_=t.sourcePayload.get();t.sourcePublished=true;
    if(!t.sourcePayload)return stop(0x2713a8);
    auto& p=*t.sourcePayload;auto& config=*sourceWorldObjectConfig_;
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!config.readSourceTerrainField158240(key,value,present,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    for(std::uint32_t i=0;i<8;++i){pc=0x2768c0;
        if(!config.selectSourceTerrainSection157D00(i,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
        // Source would skip a missing row, not fail the loader. This exact
        // authored cohort grants only all-present; refuse before that unjoined
        // continuation rather than inventing a successful/default row load.
        if(!returned)return stop(0x2768c8);
        auto& r=p.rows04[i];
        pc=0x276764;if(!read.number("surfacefriction",0x3f800000,r.friction00,found))return stop(pc);
        pc=0x27678c;if(!read.number("surfacedrag",0x3f800000,r.drag04,found))return stop(pc);
        pc=0x2767b4;if(!read.number("surfacetraction",0x3f800000,r.traction08,found))return stop(pc);
        pc=0x2767d8;if(!read.integer("generatedust",1,&r.dust0C,found))return stop(pc);
        pc=0x2767fc;if(!read.integer("generatedirt",1,&r.dirt10,found))return stop(pc);
        // 276730's zero return for missing fields is ignored by276880.
        pc=0x2768fc;++t.sourceRowsSelected;
    }
    pc=0x276924;p.count00+=static_cast<std::uint32_t>(t.sourceRowsSelected);t.sourceLoaderReturned=true;
    // Actual positive276880 return reaches World vBC272520. No legacy
    // mappingsComplete or native metadata owner is used as its input.
    pc=0x272550;if(!config.selectSourceTerrainSection157D00(8,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x272558); // Ignored source0, retained-block getter not joined for this unexpected tuple.
    pc=0x272568;if(!read.integer("numcollisionobjects",0,&t.sourceMappingCount,found))return stop(pc);
    t.sourceMappingCountKnown=true;
    if(t.sourceMappingCount!=9)return stop(0x272698);
    for(std::uint32_t i=0;i<9;++i){pc=0x272590;
        if(!config.selectSourceTerrainSection157D00(9+i,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
        if(!returned)return stop(0x272598);
        pc=0x2725b0;if(!read.text("obj","",t.sourceObjectName.data(),t.sourceObjectName.size(),found))return stop(pc);
        t.sourceObjectNameKnown=true;
        const std::string_view name(t.sourceObjectName.data());pc=0x2725bc;const auto dot=name.rfind('.');
        if(dot!=std::string_view::npos){pc=0x2725e0;std::memset(t.sourceObjectName.data()+dot,0,name.size()-dot);}
        pc=0x2725f8;if(!read.integer("terrainzone",0,&t.sourceTerrainIndex,found))return stop(pc);
        t.sourceTerrainIndexKnown=true;
        pc=0x27260c;const auto decremented=static_cast<std::uint32_t>(t.sourceTerrainIndex)-1u;
        pc=0x272614;std::memcpy(&t.sourceTerrainIndex,&decremented,4);
        auto& owner=*worldCollisionObjects3CC_;
        for(std::size_t z=0;z<10;++z){pc=0x272658;
            if(!sourceCollisionObjects3CC_||!*sourceCollisionObjects3CC_||
               *sourceCollisionObjects3CC_!=owner.sourceArray3CC_.get()||z>=owner.sourceArrayKnown_||
               !owner.sourceZones_[z]||owner.sourceArray3CC_[z]!=&owner.sourceZones_[z]->entity)return stop(pc);
            auto& group=*owner.sourceZones_[z];pc=0x272664;
            if(!group.sourceCountKnown||!group.sourceArrayReturned||group.sourceEntriesKnown!=group.count30)return stop(pc);
            for(std::size_t n=0;n<group.count30;++n){pc=0x272624;
                if(!group.sourceEntries34||n>=owner.sourceWrappers_[z].size())return stop(pc);
                auto* w=group.sourceEntries34[n];pc=0x27262c;
                if(!w||w!=owner.sourceWrappers_[z][n].get()||!w->sourceMode||!w->sourceDescriptorPublished)return stop(pc);
                pc=0x272630;if(!w->descriptor08||!w->descriptor08->sourceFields)return stop(pc);
                auto& desc=*w->descriptor08->sourceFields;
                if(!desc.loaded||!desc.nameKnown||desc.nameKnown>desc.name00.size()||desc.name00[desc.nameKnown-1]!=0)return stop(pc);
                pc=0x272634;const std::string_view actual(desc.name00.data());const std::string_view requested(t.sourceObjectName.data());
                // Exact authored names are ASCII;308F00 is case-insensitive.
                bool equal=actual.size()==requested.size();
                for(std::size_t c=0;equal&&c<actual.size();++c){auto lower=[](unsigned char v)noexcept{return v>='A'&&v<='Z'?unsigned(v+32):unsigned(v);};
                    if(static_cast<unsigned char>(actual[c])>=128||static_cast<unsigned char>(requested[c])>=128)return stop(pc);
                    equal=lower(actual[c])==lower(requested[c]);}
                if(equal){pc=0x27264c;desc.field180=decremented;}
            }
        }
        ++t.sourceMappingsCompleted;
    }
    sourceTerrainPhase_=SourceTerrainPhase::returned;pc=sourceTerrainPC_=0x271d2c;return true;
}
} //namespace ra_cold_aggregate50LA

// Inside rr::assets after Resource definition and resolver storage declarations.
struct OriginalStandardTextureResource50LA::LoadingPstNative50LA {
    struct AlignedDelete {void operator()(std::uint8_t* p)const noexcept {if(p)::operator delete[](p,std::align_val_t{64});}};
    struct Palette {
        std::uint32_t count04=0;
        std::unique_ptr<std::uint8_t[]> data08;
        bool pointer08Published=false,constructed=false;
    };
    struct MipDefaults {
        const void* pointer100=nullptr;const void* pointer118=nullptr;
        std::int32_t width104=-1,height110=-1,stride108=-1,logWidth10C=-1,logHeight114=-1;
        std::int32_t bytes11C=-1,field120=-1,width12C=-1,height128=-1;
        std::uint16_t width124=0xffff;std::uint8_t flags130=0,known130=3;
    };
    std::array<MipDefaults,5> constructorMipWrites{}; // Historical write ledger, not a second live mip lane.
    std::array<std::int32_t,3> sourceF4EcF0{};
    bool baseFieldsWritten=false;
    std::unique_ptr<Palette> pendingPalette;
    const Palette* source204=nullptr;
    const Palette* source208=nullptr;
    bool publication204=false,publication208=false;
    std::unique_ptr<std::uint8_t[],AlignedDelete> mip;
    const std::uint8_t* source100=nullptr;
    std::uint32_t header[4]{};
    std::uint32_t version=0,kind=0,count=0;
    std::uint32_t allocationBytes=0;
    std::int32_t width104=0,height110=0,logWidth10C=0,logHeight114=0,blocks108=0,width12C=0,height128=0;
    std::uint16_t width124=0;
    bool attempted=false,mipPublished=false,counterWritten=false,dimensionsWritten=false,metadataWritten=false;
    bool paletteRead=false,pixelsRead=false,nativeClosed=false;
    std::uint32_t lastPC=0;
};
bool OriginalStandardTextureResource50LA::admitLoadingPstNative(D& d)noexcept try {
    d={};if(!loadingPstFile_||(bool(loadingPstFile_->root_)==bool(loadingPstFile_->raRoot_))||loadingPstNative_||loadingPstFc_||registered_||constructed_)
        return fail(d,E::phase,0x24DEF8);
    // Host wrapper admission only; neither allocation is original Palette/FC.
    loadingPstNative_.reset(new LoadingPstNative50LA);
    loadingPstFc_.reset(new OriginalLoadingTextureFc50LA);return true;
}catch(...){return fail(d,E::allocation,0x24DEF8);}
bool OriginalStandardTextureResource50LA::initializeLoadingPstFields24E1E0(D& d)noexcept {
    d={};if(!loadingPstNative_||!loadingPstFc_||!registered_||!loadingPstFc_->nullConstructorReached_||loadingPstNative_->baseFieldsWritten)
        return fail(d,E::phase,0x24E1E0);
    auto& p=*loadingPstNative_;bitsPerPixelE8_=-1; //24E1F0.
    for(std::size_t i=0;i<5;++i){p.constructorMipWrites[i]={};levels_[i]={};}
    p.source100=nullptr;p.width104=p.height110=p.blocks108=p.logWidth10C=p.logHeight114=-1;
    p.width12C=p.height128=-1;p.width124=0xffff;
    p.source204=nullptr;p.publication204=true; //24E2EC.
    p.source208=nullptr;p.publication208=true; //24E2F4.
    p.sourceF4EcF0[0]=-1;p.sourceF4EcF0[1]=-1;p.sourceF4EcF0[2]=-1; //24E2FC/304/30C.
    flagsF8_=static_cast<std::uint8_t>(flagsF8_&~1u);flagsF8KnownMask_|=1; //24E320.
    flagsF8_=static_cast<std::uint8_t>(flagsF8_&~2u);flagsF8KnownMask_|=2; //24E334.
    flagsF8_=static_cast<std::uint8_t>(flagsF8_&~4u);flagsF8KnownMask_|=4; //24E348.
    flagsF8_=static_cast<std::uint8_t>(flagsF8_&~8u);flagsF8KnownMask_|=8; //24E35C.
    p.baseFieldsWritten=true;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResource50LA::constructLoadingPst24E880(D& d)noexcept {
    d={};if(!loadingPstNative_||!loadingPstFc_||!loadingPstFile_||!resolver_||!registered_||constructed_||
       !resolver_->loadingPstFileCurrent(*loadingPstFile_,true)||loadingPstFile_->boundResource_!=this)
        return fail(d,E::phase,0x24E880);
    const bool ra=bool(loadingPstFile_->raRoot_);
    if(bool(loadingPstFile_->root_)==ra||ra!=bool(firstUiSource_.loadingRa))return fail(d,E::source,0x24E880);
    auto& p=*loadingPstNative_;if(!p.baseFieldsWritten||p.attempted||p.nativeClosed)return fail(d,E::phase,0x24E880);p.attempted=true;
    const auto read=[&](void* dst,std::size_t n,std::uint32_t pc)noexcept {
        p.lastPC=pc;std::size_t done=0;
        if(!resolver_->readLoadingPst12FB30(*loadingPstFile_,dst,n,done,d))return false;
        return done==n||fail(d,E::source,pc+8);
    };
    if(!read(&p.version,4,0x24E8B8))return false;
    if(!OriginalTextureAllocationHistory50LA::assertionHistoryCurrent())return fail(d,E::phase,0x24E8C0);
    if(!OriginalTextureAllocationHistory50LA::version24E900(p.version))return fail(d,E::source,0x24E900);
    if(p.version!=1)return fail(d,E::source,0x24E940);
    if(!read(&p.kind,4,0x24E958))return false;
    if(p.kind!=(ra?1u:0u))return fail(d,E::unsupported,0x24E960);
    formatAC_=ra?8:4;bitsPerPixelE8_=ra?8:4; //24E968 then24E974.
    if(!read(&p.count,4,0x24E98C))return false;
    if(p.count!=1)return fail(d,E::unsupported,0x24E994);
    const std::size_t paletteBytes=ra?1024:64;
    p.lastPC=ra?0x24EA04:0x24E9C4;p.pendingPalette.reset(new(std::nothrow) LoadingPstNative50LA::Palette);
    if(p.pendingPalette){
        auto& palette=*p.pendingPalette;palette.count04=0;palette.pointer08Published=true; //11A51C/524.
        palette.count04=ra?256:16; //11A5E4 or11A534.
        try {palette.data08.reset(new std::uint8_t[paletteBytes]);}catch(...){palette.data08.reset();}
        palette.pointer08Published=true; //11A550, including actualNULL.
        if(!palette.data08){p.lastPC=ra?0x11A610:0x11A560;return fail(d,E::allocation,p.lastPC);}
        std::memset(palette.data08.get(),0,paletteBytes);palette.constructed=true;
    }
    if(ra){p.source208=p.pendingPalette.get();p.publication208=true;} //24EA1C, even outerNULL.
    else {p.source204=p.pendingPalette.get();p.publication204=true;} //24E9DC.
    if(!p.pendingPalette){p.lastPC=ra?0x11A630:0x11A580;return fail(d,E::allocation,p.lastPC);}
    if(!read(p.pendingPalette->data08.get(),paletteBytes,ra?0x24EA24:0x24E9E4))return false;p.paletteRead=true;
    p.lastPC=ra?0x24EA30:0x24E9F0;if(!loadingPstFc_->construct18_257750(p.lastPC))return fail(d,p.lastPC==0x257750?E::phase:E::allocation,p.lastPC);
    if(!read(p.header,16,0x24EB20))return false;
    // Exact real source families only; dimensions come from the actual read.
    if(p.header[0]!=(ra?448u:256u)||p.header[1]!=(ra?640u:256u)||p.header[2]||p.header[3])
        return fail(d,E::source,ra?0x24EDFC:0x24EDA8);
    p.allocationBytes=ra?286720:32768;p.lastPC=0x2505C8;
    try {p.mip.reset(static_cast<std::uint8_t*>(::operator new[](p.allocationBytes,std::align_val_t{64})));}
    catch(...){p.mip.reset();}
    p.source100=p.mip.get();p.mipPublished=true; //2505D0, includingNULL.
    p.lastPC=0x2505D8;
    if(!OriginalTextureAllocationHistory50LA::add2505E8(p.allocationBytes))return fail(d,E::phase,p.lastPC);
    p.counterWritten=true;
    p.width104=static_cast<std::int32_t>(p.header[1]);p.height110=static_cast<std::int32_t>(p.header[0]);
    p.width124=static_cast<std::uint16_t>(p.width104);p.dimensionsWritten=true; //2505EC/F0/F8.
    // Source __ieee754_log2FF788 power-of-two f==0 branch2FF8F8:
    // log(256)=fl(8*ln2_hi+8*ln2_lo)=8*fl(ln2_hi+ln2_lo).
    // Both multiplications are exact binary exponent shifts. Division by the
    // identically computed log(2) is exactly8; fixdfsi3102A0 therefore yields8.
    // This exact tuple needs no host libm/rounding approximation.
    // For RA, the retained polynomial branch proves9<log2(640)<10
    // and8<log2(448)<9 including software rounding; see pinned math note.
    p.logWidth10C=ra?9:8;p.logHeight114=8; //250628 then250660.
    p.blocks108=ra?10:4; //max(2,width/64), rounded to an even stride2506B0..D8.
    p.width12C=p.width104;p.height128=p.height110;p.metadataWritten=true; //25070C/710.
    if(!OriginalTextureAllocationHistory50LA::allocation250758(p.source100!=nullptr))return fail(d,E::allocation,0x250758);
    if(!p.source100)return fail(d,E::allocation,0x250790);
    p.lastPC=ra?0x24EE38:0x24EDE4;std::size_t done=0;
    if(!resolver_->readLoadingPst12FF70(*loadingPstFile_,p.mip.get(),p.allocationBytes,done,d))return false;
    if(done!=p.allocationBytes)return fail(d,E::source,ra?0x24EE40:0x24EDEC);p.pixelsRead=true;
    if(ra){
        // Source382FC8 zero goes straight to24F128; nonzero rejects640 from
        // the conversion width set and reaches the SAME store. No guessed
        // current global value, transfer conversion or native upload.
        levels_[0].flags30=static_cast<std::uint8_t>(levels_[0].flags30&~4u);
        levels_[0].flags30KnownMask|=4; //24F144.
    }
    if(!OriginalTextureAllocationHistory50LA::width24F26C(p.width104))return fail(d,E::source,0x24F26C);
    auto& level=levels_[0];level.packedIndices100=p.source100;level.packedBytes=p.allocationBytes;
    level.linearSourceIndices=p.source100;level.linearSourceBytes=p.allocationBytes;
    level.width104=p.width104;level.height110=p.height110;level.sourceHeaderOffset=ra?1036:76;level.sourceIndicesOffset=ra?1052:92;
    level.sourceMipOrdinal=0;level.storage=ra?OriginalStandardTextureLevelStorage50LA::pstLoadingLinear8:OriginalStandardTextureLevelStorage50LA::pstPacked4;
    width14_=static_cast<std::int32_t>(p.header[1]);height18_=static_cast<std::int32_t>(p.header[0]); //24EB94/9C.
    levelCountB0_=1; //24EBC8; table flags2 and count1.
    p.lastPC=0x24EBD0;constructed_=true;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResource50LA::preflightLoadingPstNative()const noexcept {
    if(!loadingPstNative_)return true;
    if(references00_!=1)return false;
    if(registered_){
        if(!construction_||!construction_->textureLifetime_)return false;
        const auto& manager=*construction_->textureLifetime_;
        if(lifetimeLink_.owner_!=&manager||!manager.count0C_||lifetimeLink_.resource_!=this)return false;
        if(lifetimeLink_.previousA4_){
            if(lifetimeLink_.previousA4_->owner_!=&manager||lifetimeLink_.previousA4_->nextA0_!=&lifetimeLink_)return false;
        }else if(manager.head00_!=&lifetimeLink_)return false;
        if(lifetimeLink_.nextA0_){
            if(lifetimeLink_.nextA0_->owner_!=&manager||lifetimeLink_.nextA0_->previousA4_!=&lifetimeLink_)return false;
        }else if(manager.tail04_!=&lifetimeLink_)return false;
    }else if(lifetimeLink_.owner_||lifetimeLink_.nextA0_||lifetimeLink_.previousA4_)return false;
    if(loadingPstFile_&&loadingPstFile_->associated_&&!loadingPstFile_->nativeAssociationWithdrawn_&&
       (!resolver_||(!resolver_->canClearLoadingPst1327D0(*loadingPstFile_,*this)&&
        !resolver_->canWithdrawLoadingRaAssociationNative(*loadingPstFile_,*this))))return false;
    return !loadingPstFc_?!loadingPstNative_->attempted:loadingPstFc_->preflightNativeClose();
}

bool OriginalStandardTextureResource50LA::closeLoadingPstNative(D& d)noexcept {
    d={};if(!loadingPstNative_)return true;auto& p=*loadingPstNative_;if(p.nativeClosed)return true;
    if(loadingPstFc_&&!loadingPstFc_->preflightNativeClose())return fail(d,E::phase,0);
    if(!loadingPstFc_&&p.attempted)return fail(d,E::phase,0);
    // Exact pending source file is closed by Table before resource disposal.
    if(loadingPstFile_&&loadingPstFile_->handle_!=-1)return fail(d,E::phase,0);
    if(loadingPstFile_&&loadingPstFile_->raRoot_&&loadingPstFile_->associated_&&
       !loadingPstFile_->nativeAssociationWithdrawn_&&resolver_&&
       !resolver_->canClearLoadingPst1327D0(*loadingPstFile_,*this)){
        if(!resolver_->withdrawLoadingRaAssociationNative(*loadingPstFile_,*this,d))return false;
    }
    if(p.counterWritten)OriginalTextureAllocationHistory50LA::invalidateNativeCounterCleanup();
    levels_[0]={};constructed_=false; // Withdraw native public views before freeing their backing.
    p.mip.reset();p.pendingPalette.reset();
    if(loadingPstFc_&&!loadingPstFc_->closeNative())return fail(d,E::phase,0);
    p.nativeClosed=true;return true; // Historical source pointers remain ledger-only.
}

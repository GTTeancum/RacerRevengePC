//Included inside ra_cold_aggregate50LA. Only the real first water factory
//can enter this success-only allocation/base-construction occurrence.
bool Owner::sourceWaterProviderCountedNative(std::size_t& count)const noexcept {
    if(!preflightSourceWaterProviderMembersNative())return false;
    count=0;const auto& h=sourceWaterProvider274720_;
    if(h.zeroWordsWritten>h.zeroWords.size()||h.flags33Known>7u||
       (h.flags33Bits&~h.flags33Known))return false;
    for(std::size_t n=0;n<h.zeroWords.size();++n)
        if(n<h.zeroWordsWritten?(h.zeroWords[n]!=0u):bool(h.zeroWords[n]))return false;
    if(!h.allocationAttempted)return !sourceWaterProvider_&&!h.allocationReturned&&!h.nativeAllocationRefused&&
        !h.constructorEntered&&!h.baseEntered&&!h.countAcquired&&!h.countRetiredNative&&
        !h.typeCallEntered&&!h.typeCallReturned&&!h.searchableReturned&&!h.waterBaseEntered&&
        !h.formatCallEntered&&!h.nativeWithdrawn&&!h.countBefore&&!h.vtable08&&!h.query30&&!h.type32&&
        !h.typeReturned&&!h.flags33Known&&!h.flags33Bits&&!h.zeroWordsWritten;
    if(!sourceWaterFactory274570_.providerAllocationReached||!sourceWaterFactory274570_.visibilityOwnerValidated||
       sourceWaterFactory274570_.existsV0!=1u||sourcePostSkyTail_.water[0].returnV0||
       h.allocationReturned==h.nativeAllocationRefused||
       (h.constructorEntered&&!h.allocationReturned)||(h.baseEntered&&!h.constructorEntered)||
       (h.countAcquired&&!h.baseEntered)||bool(h.countBefore)!=h.countAcquired||
       (h.typeCallEntered&&!h.countAcquired)||h.typeCallReturned!=bool(h.typeReturned)||
       (h.typeCallReturned&&!h.typeCallEntered)||h.searchableReturned!=h.typeCallReturned||
       h.waterBaseEntered!=h.searchableReturned||
       (h.formatCallEntered&&(!h.waterBaseEntered||h.zeroWordsWritten!=h.zeroWords.size()))||
       (h.zeroWordsWritten&&!h.waterBaseEntered)||h.countRetiredNative!=(h.nativeWithdrawn&&h.countAcquired))return false;
    if(h.nativeAllocationRefused&&(h.constructorEntered||h.pc!=0x274720u))return false;
    if(h.countAcquired){
        //No intervening fallible step between these fixed stores. A refused
        //type lookup keeps its earlier FF category and fully known low3=0.
        if(h.query30!=0u||!h.typeCallEntered||h.flags33Known!=7u||
           h.vtable08!=(sourceWaterProviderMembers_.baseReturned?0x3d46e0u:h.waterBaseEntered?0x3d45d0u:0x3ccee0u)||
           h.type32!=(h.typeCallReturned?h.typeReturned:std::optional<std::uint8_t>(0xffu))||
           h.flags33Bits!=(h.waterBaseEntered?3u:0u))return false;
    }else if(h.vtable08||h.query30||h.type32||h.flags33Known||h.flags33Bits||h.typeCallEntered)return false;
    if(h.formatCallEntered&&!sourceWaterProviderMembers_.entered&&h.pc!=0x308c90u)return false;
    if(h.nativeWithdrawn)return !sourceWaterProvider_;
    if(!worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       worldApplication_->attachmentLock423F58()||bool(sourceWaterProvider_)!=h.allocationReturned)return false;
    if(!sourceWaterProvider_)return !h.countAcquired;
    const auto& e=sourceWaterProvider_->entity;const auto& authority=worldApplication_->entityAuthority_;
    if(e.parent1C||e.previous10||e.next14||e.firstChild18||e.mask20||e.descendants24||e.nativeFlags29)return false;
    if(h.countAcquired){
        if(e.reference00!=1u||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||
           e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration||
           e.category28!=0xffu||(e.flags29&15u)!=3u||!worldApplication_->objectCount423F60()||
           !authority.canRetire(e))return false;
        count=1;
    }else{
        if(e.reference00||e.kind!=g::OriginalWreckComponentKind50LA::unbound||e.objectKey||e.sessionGeneration)return false;
        for(std::size_t i=0;i<authority.count_;++i)if(authority.entries_[i].node==&e)return false;
    }
    return true;
}
bool Owner::withdrawSourceWaterProviderNative()noexcept {
    auto& h=sourceWaterProvider274720_;std::size_t count=0;
    if(!sourceWaterProviderCountedNative(count))return false;
    if(!h.allocationAttempted||h.nativeWithdrawn)return true;
    //The public close path reaches this before its later combined floor.
    //Validate all still-owned source counts before decrementing any one.
    if(h.countAcquired&&!preflightSourceWorldParticleCountNative())return false;
    //Withdraw the exact native World400 publication before freeing backing.
    //Unknown knowledge is not a source NULL store or unfinished destructor.
    auto& continuation=sourceWaterProviderInitializer_;
    if(continuation.world400Published&&!continuation.world400NativeWithdrawn){
        if(!sourceWaterProvider_||!worldWater400_||
           *worldWater400_!=&sourceWaterProvider_->entity)return false;
        worldWater400_.reset();continuation.world400NativeWithdrawn=true;
        continuation.lightPin.reset();
    }
    //No provider globals443B80/443CE8, callback or graph alias are published
    //by this prefix. Retire the exact count once; process scalar/guard effects stay.
    if(h.countAcquired){auto& e=sourceWaterProvider_->entity;
        if(!worldApplication_->entityAuthority_.retire(e))return false;
        --e.reference00;--worldApplication_->objectCount423F60();h.countRetiredNative=true;
    }
    sourceWaterProvider_.reset();h.nativeWithdrawn=true;return true;
}
bool Owner::continueSourceWaterProvider274720(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterProvider274720_;const auto& factory=sourceWaterFactory274570_;
    if(pc!=0x274720u||sourcePostParticleModelsPC_!=pc||h.allocationAttempted||
       nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourcePostParticleModelsPhase_!=SourcePostParticleModelsPhase::entered||
       !factory.providerAllocationReached||!factory.visibilityOwnerValidated||factory.existsV0!=1u||
       !factory.arguments||!factory.numericLayout||!sourceLoadRace_||!sourceLoadApplication_||!worldApplication_||
       sourceLoadRace_->world74_.get()!=this||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||worldApplication_->attachmentLock423F58()||
       !preflightSourcePostParticleModelsNative()||!preflightSourceWorldParticleCountNative())return false;
    const auto at=[&](std::uint32_t p)noexcept{pc=sourcePostParticleModelsPC_=h.pc=p;};
    at(0x274720);h.allocationAttempted=true;
    //Native typed backing for the actual0x4B0 object role. Success alone is
    //admitted. Host failure is NOT source2E8D90's new-handler/throw/NULL path.
    sourceWaterProvider_.reset(new(std::nothrow) SourceWaterProviderOwner);
    if(!sourceWaterProvider_){h.nativeAllocationRefused=true;return false;}
    h.allocationReturned=true;
    at(0x274728);at(0x27472c);at(0x274730); //Actual a1=1.
    at(0x274734);at(0x274738); //A2/A3 are the SAME retained factory arguments.
    at(0x27473c);at(0x274740);at(0x2b8370);h.constructorEntered=true;
    at(0x2b8388);at(0x2b838c);at(0x2b8390);at(0x2b0800);h.baseEntered=true;
    at(0x2b080c);at(0x2b0810);at(0x27e130);at(0x27e13c);at(0x27e140);at(0x141300);
    auto& e=sourceWaterProvider_->entity;auto& authority=worldApplication_->entityAuthority_;
    auto& count=worldApplication_->objectCount423F60();
    if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
       !authority.canEnroll(e,key(&e)))return false;
    h.countBefore=count;
    if(g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::entity141300,
       worldEntity_.sessionGeneration,key(&e),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
       !authority.enroll(e))std::terminate(); //Admitted exact same-owner bookkeeping must not partially escape.
    at(0x1413c0);h.countAcquired=true;h.vtable08=0x3cd240u;at(0x1413d4);
    at(0x27e154);h.query30=0;at(0x27e168);h.type32=0xffu;
    at(0x27e188);h.flags33Known=1;at(0x27e19c);h.flags33Known=3;
    at(0x27e1b0);h.flags33Known=7;at(0x27e1b8);h.vtable08=0x3ccee0u;
    at(0x27e1bc);h.typeCallEntered=true;
    const auto type=worldApplication_->types383408_.searchableGameObject27E1BC();
    if(!type.okay()){at(type.sourcePC);return false;}
    h.typeCallReturned=true;h.typeReturned=type.type;
    at(0x27e1f8);h.type32=type.type;at(0x27e218);h.flags33Bits=1;
    at(0x27e22c);at(0x27e240);at(0x27e24c);at(0x27e250);h.searchableReturned=true;
    at(0x2b0814);at(0x2b082c);h.vtable08=0x3d45d0u;h.waterBaseEntered=true;
    at(0x2b084c);h.flags33Bits=1;at(0x2b0860);h.flags33Bits=3;
    //Ordered zero stores, including the two loop iterations and final+80.
    //Offsets:38,40,44,48,5C,50,54,C8,D0,4C,68,70,74,78,7C,80,6C,98,9C,A0,A4,CC.
    static constexpr std::array<std::uint32_t,22> stores{{
        0x2b0868,0x2b0870,0x2b0878,0x2b0880,0x2b0888,0x2b0890,0x2b0898,
        0x2b08a0,0x2b08a8,0x2b08b0,0x2b08b8,0x2b08cc,0x2b08e0,0x2b08cc,
        0x2b08e0,0x2b090c,0x2b0924,0x2b093c,0x2b0944,0x2b094c,0x2b0954,0x2b0960}};
    for(std::size_t n=0;n<stores.size();++n){
        if(n+1==stores.size())at(0x2b095c); //JAL's delay slot still storesCC.
        at(stores[n]);h.zeroWords[n]=0;++h.zeroWordsWritten;
    }
    h.formatCallEntered=true;at(0x308c90);
    return continueSourceWaterProviderFormat308C90(pc);
}
#include "original_source_world_water_provider_members50la.inl"

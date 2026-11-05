bool Owner::preflightNativeWaterZoneElements()const noexcept {
    const auto& n=sourceWaterConfig_.nativeWaterZoneElements;
    const auto& c=sourceWaterConfig_.nativeWaterArrayCaller;
    const auto& a=sourceWaterConfig_.nativeWaterArrayBacking;
    const auto& e=sourceWaterConfig_.nativeWaterAllocationEntry;
    const auto noLast=[&]()noexcept {
        return !n.lastElementOffset&&!n.lastOrdinal&&!n.lastFlagRead&&!n.lastVtable&&
            !n.lastSaveMask&&!n.lastRestoreMask&&!n.lastFrameReleased;
    };
    if(!n.attempted)return n.pc==0u&&!n.wrapperAlreadyReturned&&!n.wrapperReturnProjected&&!n.d0Published&&
        !n.completed&&!n.bodiesEntered&&!n.baseReturns&&!n.constructorReturns&&!n.callsIssuedHere&&!n.repeatComparisons&&
        !n.count&&!n.wrapperOrdinal&&!n.nextElementOffset&&noLast()&&!n.wrapperStackReleased&&!n.wrapperRestoreMask&&
        (!sourceWaterProvider_||(!sourceWaterProvider_->nativeArrayD0&&
         !sourceWaterProvider_->nativeArrayBacking.zoneElementsCompleted&&!sourceWaterProvider_->nativeArrayBacking.zoneCompletedVtable));
    if(!a.nativeAllocated||a.role!=NativeWaterArrayRole::zoneRaw||a.epoch!=1u||!c.attempted||
       c.role!=NativeWaterArrayRole::zoneRaw||c.cookieWrites!=2u||c.elementOffset!=16u||
       !n.count||n.count!=c.zoneCount||*n.count>nativeWaterArrayMaxCount||n.pc!=0x2b9dd4u||!n.d0Published||
       n.completed!=*n.count||n.wrapperOrdinal!=n.count||n.nextElementOffset!=16u+*n.count*36u||
       !sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||sourceWaterConfigOwner_->wakeSelection_.returned||
       (e.pc!=0x3059ecu&&e.pc!=0x305ac4u)||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_))||
       (sourceWaterProvider274720_.nativeWithdrawn&&sourceWaterProvider_))return false;
    if(*n.count){
        if(c.pc!=0x2461e0u||!c.constructorCallIssued||c.wrapperReturned||n.wrapperAlreadyReturned||
           !n.wrapperReturnProjected||!n.wrapperStackReleased||n.wrapperRestoreMask!=1023u||
           n.bodiesEntered!=*n.count||n.baseReturns!=*n.count||n.constructorReturns!=*n.count||
           n.callsIssuedHere!=*n.count-1u||n.repeatComparisons!=*n.count||
           n.lastOrdinal!=*n.count-1u||n.lastElementOffset!=16u+(*n.count-1u)*36u||
           n.lastFlagRead!=0u||n.lastVtable!=NativeWaterZoneVtable::derived3D20C0||
           n.lastSaveMask!=3u||n.lastRestoreMask!=3u||!n.lastFrameReleased)return false;
    }else if(c.pc!=0x2b9dd0u||!c.wrapperReturned||c.constructorCallIssued||!n.wrapperAlreadyReturned||
        n.wrapperReturnProjected||n.wrapperStackReleased||n.wrapperRestoreMask||n.bodiesEntered||n.baseReturns||
        n.constructorReturns||n.callsIssuedHere||n.repeatComparisons||!noLast())return false;
    if(sourceWaterProvider274720_.nativeWithdrawn)return true; //History only; backing and field view retired.
    const auto& b=sourceWaterProvider_->nativeArrayBacking;const auto& view=sourceWaterProvider_->nativeArrayD0;
    if(!view||view->role!=NativeWaterArrayRole::zoneRaw||view->epoch!=1u||view->offset!=16u||!b.payload||
       b.effectiveBytes!=16u+*n.count*36u||b.zoneElementsCompleted!=n.completed||
       b.zoneCompletedVtable!=(*n.count?std::optional<NativeWaterZoneVtable>(NativeWaterZoneVtable::derived3D20C0):std::nullopt))return false;
    //Only completed element bytes are read. Cookie padding8..15 is unknown.
    const auto* bytes=static_cast<const unsigned char*>(b.payload.get());
    static constexpr std::array<std::uint32_t,9> words{{0xffffffffu,0u,0x3d20c0u,0x00ff0000u,
        0u,0u,0x3f800000u,0x3f800000u,0u}};
    for(std::uint32_t i=0;i<n.completed;++i)for(std::size_t w=0;w<words.size();++w)
        for(std::uint32_t j=0;j<4u;++j)
            if(bytes[16u+std::size_t(i)*36u+w*4u+j]!=((words[w]>>(j*8u))&0xffu))return false;
    return true;
}
Owner::WaterZoneElementsResult Owner::constructWaterZoneElementsNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterZoneElementsResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& n=sourceWaterConfig_.nativeWaterZoneElements;const auto& c=sourceWaterConfig_.nativeWaterArrayCaller;
    const auto& a=sourceWaterConfig_.nativeWaterArrayBacking;const auto& e=sourceWaterConfig_.nativeWaterAllocationEntry;
    auto& backing=sourceWaterProvider_->nativeArrayBacking;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;result.allocatorStop=e.pc;
    if(!a.nativeAllocated||a.nativeRetired||a.role!=NativeWaterArrayRole::zoneRaw||a.epoch!=1u||
       !c.attempted||!c.zoneCount||c.cookieWrites!=2u||c.elementOffset!=16u||!backing.payload||
       (*c.zoneCount?(c.pc!=0x2461e0u||!c.constructorCallIssued||c.wrapperReturned):
                      (c.pc!=0x2b9dd0u||!c.wrapperReturned||c.constructorCallIssued)))return result;
    const auto finish=[&](WaterZoneElementsResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.completed=n.completed;result.d0Published=n.d0Published;return result;
    };
    if(n.attempted)return finish(WaterZoneElementsResult::Status::alreadyAttempted);
    n.attempted=true;n.count=c.zoneCount;n.pc=c.pc;n.wrapperOrdinal=0u;n.nextElementOffset=16u;
    n.wrapperAlreadyReturned=c.wrapperReturned;
    auto* raw=static_cast<unsigned char*>(backing.payload.get());
    const auto word=[&](std::uint32_t at,std::uint32_t value)noexcept {
        for(std::uint32_t j=0;j<4u;++j)raw[at+j]=static_cast<unsigned char>(value>>(j*8u));
    };
    const auto half=[&](std::uint32_t at,std::uint16_t value)noexcept {
        raw[at]=static_cast<unsigned char>(value);raw[at+1u]=static_cast<unsigned char>(value>>8u);
    };
    while(*n.wrapperOrdinal<*n.count){
        if(*n.wrapperOrdinal){
            n.pc=0x2e8a18;++n.callsIssuedHere;n.pc=0x2e8a1c; //Next JALR/delay; first call belongs to predecessor.
        }
        n.pc=0x2461e0;++n.bodiesEntered;n.lastOrdinal=n.wrapperOrdinal;n.lastElementOffset=n.nextElementOffset;
        const auto offset=*n.lastElementOffset;
        n.lastSaveMask=0u;n.lastRestoreMask=0u;n.lastFrameReleased=false;
        n.lastFlagRead.reset();n.lastVtable.reset(); //Fresh constructor-local knownness, not payload initialization.
        n.pc=0x2461e4;n.lastSaveMask|=1u; //SQ ra+10 in32-byte frame.
        n.pc=0x2461e8;n.lastSaveMask|=2u; //SQ s0+0.
        n.pc=0x2461ec;n.pc=0x2461f0; //Call base; delay retains this element role in s0.
        n.pc=0x299aa0;n.pc=0x299aa4;n.pc=0x299aa8;
        word(offset+8u,0x3d36b0u);n.lastVtable=NativeWaterZoneVtable::base3D36B0;
        n.pc=0x299aac;n.pc=0x299ab0;word(offset,0xffffffffu);
        n.pc=0x299ab4;word(offset+4u,0u);
        n.pc=0x299ab8;n.pc=0x299abc;++n.baseReturns; //Return same symbolic element, never a host vptr.
        n.pc=0x2461f4;n.pc=0x2461f8;n.pc=0x2461fc;
        word(offset+8u,0x3d20c0u);n.lastVtable=NativeWaterZoneVtable::derived3D20C0;
        n.pc=0x246200;raw[offset+12u]=0u;
        n.pc=0x246204;n.pc=0x246208;raw[offset+13u]=0u;
        n.pc=0x24620c;raw[offset+14u]=255u;
        n.pc=0x246210;raw[offset+15u]=0u;
        n.pc=0x246214;n.pc=0x246218;word(offset+24u,0x3f800000u);
        n.pc=0x24621c;word(offset+28u,0x3f800000u);
        n.pc=0x246220;word(offset+16u,0u);
        n.pc=0x246224;n.lastFlagRead=raw[offset+15u]; //This byte was just written; no unknown payload read.
        n.pc=0x246228;n.pc=0x24622c;
        const auto cleared=std::uint32_t(*n.lastFlagRead)&0xfffffffeu;const std::uint32_t inserted=0u&1u;
        n.pc=0x246230;n.pc=0x246234;n.pc=0x246238;n.pc=0x24623c;
        raw[offset+15u]=static_cast<unsigned char>(cleared|inserted);
        n.pc=0x246240;word(offset+20u,0u);
        n.pc=0x246244;half(offset+32u,0u);
        n.pc=0x246248;half(offset+34u,0u);
        n.pc=0x24624c;n.lastRestoreMask|=1u;n.pc=0x246250;n.lastRestoreMask|=2u;
        n.pc=0x246254;n.pc=0x246258;n.lastFrameReleased=true;++n.constructorReturns;
        ++n.completed;backing.zoneElementsCompleted=n.completed;
        backing.zoneCompletedVtable=NativeWaterZoneVtable::derived3D20C0;
        n.pc=0x2e8a20;n.pc=0x2e8a24;n.nextElementOffset=*n.nextElementOffset+36u;
        n.pc=0x2e8a28;const auto next=std::uint32_t(*n.wrapperOrdinal+1u);
        n.pc=0x2e8a2c;n.wrapperOrdinal=next;
        n.pc=0x2e8a30;n.pc=0x2e8a34;++n.repeatComparisons;
        n.pc=0x2e8a38;n.pc=0x2e8a3c; //Unsigned compare and a1=1 delay; next loop body only if below count.
    }
    if(!n.wrapperAlreadyReturned){
        n.pc=0x2e8a40;n.pc=0x2e8a44;n.pc=0x2e8a48;n.pc=0x2e8a4c; //Completed ordinal==count, return view raw+16.
        static constexpr std::array<std::uint32_t,10> restorePC{{0x2e8aacu,0x2e8ab0u,0x2e8ab4u,0x2e8ab8u,0x2e8abcu,
            0x2e8ac0u,0x2e8ac4u,0x2e8ac8u,0x2e8accu,0x2e8ad0u}};
        for(std::size_t i=0;i<restorePC.size();++i){n.pc=restorePC[i];n.wrapperRestoreMask|=std::uint16_t(1u<<i);}
        n.pc=0x2e8ad4;n.pc=0x2e8ad8;n.wrapperStackReleased=true;n.wrapperReturnProjected=true;
    }
    n.pc=0x2b9dd0;
    sourceWaterProvider_->nativeArrayD0=NativeWaterArrayView{NativeWaterArrayRole::zoneRaw,*a.epoch,16u};
    n.d0Published=true;n.pc=0x2b9dd4;
    return finish(WaterZoneElementsResult::Status::nativeCompleted); //Before fresh NumZones read / distinct4A4 allocation.
}

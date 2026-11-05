bool Owner::preflightNativeWaterArrayCaller()const noexcept {
    if(!preflightNativeWaterArrayFormat())return false;
    if(!preflightNativeWaterZoneElements())return false;
    const auto& n=sourceWaterConfig_.nativeWaterArrayCaller;
    const auto& a=sourceWaterConfig_.nativeWaterArrayBacking;
    const auto& e=sourceWaterConfig_.nativeWaterAllocationEntry;
    const auto noFormat=[&]()noexcept {
        return !n.formatCallIssued&&!n.formatStackOffset&&!n.formatLiteral&&!n.formatIndex&&!n.formatReturnPC;
    };
    const auto noZone=[&]()noexcept {
        return !n.wrapperEntered&&!n.wrapperReturned&&!n.wrapperFrame&&!n.wrapperStackReleased&&
            !n.wrapperSaveMask&&!n.wrapperRestoreMask&&!n.zoneConstructor&&!n.zoneDestructor&&!n.zoneSize&&
            !n.zoneCount&&!n.zoneCountRegister&&!n.cookieWrites&&!n.unwindStoreMask&&!n.elementOffset&&
            !n.unwindSize&&!n.unwindCount&&!n.unwindDestructor&&!n.initialOrdinalSlot&&
            !n.constructorCallIssued&&!n.constructorFlag;
    };
    const auto noProviderEffects=[&]()noexcept {
        return !sourceWaterProvider_||(!sourceWaterProvider_->nativeArrayC8&&!sourceWaterProvider_->nativeArray68&&
            !sourceWaterProvider_->nativeArrayBacking.zoneCookieSize&&!sourceWaterProvider_->nativeArrayBacking.zoneCookieCount);
    };
    if(!n.attempted)return n.pc==0u&&!n.nativeViewsRetired&&!n.pointerPublished&&!n.role&&!n.ordinal&&
        !n.loopCountRegister&&!n.loopCount&&!n.loopTaken&&noFormat()&&noZone()&&noProviderEffects();
    if(!a.nativeAllocated||!a.role||n.role!=a.role||a.epoch!=1u||!a.nativeResumePC||
       !sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||sourceWaterConfigOwner_->wakeSelection_.returned||
       (e.pc!=0x3059ecu&&e.pc!=0x305ac4u)||n.nativeViewsRetired!=sourceWaterProvider274720_.nativeWithdrawn||
       (!n.nativeViewsRetired&&(!sourceWaterProvider_||!worldApplication_))||
       (n.nativeViewsRetired&&sourceWaterProvider_))return false;
    const bool zones=*n.role==NativeWaterArrayRole::zoneRaw;
    if(!zones){
        const bool signature=*n.role==NativeWaterArrayRole::signaturePointers;
        if(!n.pointerPublished||n.ordinal!=0u||!noZone()||a.nativeResumePC!=(signature?0x2b98dcu:0x2b9c58u))return false;
        if(!n.nativeViewsRetired){
            const auto& field=signature?sourceWaterProvider_->nativeArrayC8:sourceWaterProvider_->nativeArray68;
            const auto& other=signature?sourceWaterProvider_->nativeArray68:sourceWaterProvider_->nativeArrayC8;
            if(!field||other||field->role!=*n.role||field->epoch!=1u||field->offset||
               sourceWaterProvider_->nativeArrayBacking.zoneCookieSize||sourceWaterProvider_->nativeArrayBacking.zoneCookieCount)return false;
            if(n.loopCount&&(signature?sourceWaterProvider_->signatureWaves6C:sourceWaterProvider_->numPools64)!=n.loopCount)return false;
        }
        if(!n.loopCount)return !n.loopCountRegister&&!n.loopTaken&&noFormat()&&n.pc==(signature?0x2b9bc0u:0x2b9d78u);
        const auto countRegister=std::uint64_t(*n.loopCount)|((*n.loopCount&0x80000000u)?0xffffffff00000000ull:0ull);
        if(n.loopCountRegister!=countRegister||n.loopTaken!=(countRegister>0u))return false;
        if(!*n.loopTaken)return noFormat()&&n.pc==(signature?0x2b9bd0u:0x2b9d88u);
        return n.formatCallIssued&&n.formatStackOffset==0x710u&&n.formatIndex==1u&&
            n.formatLiteral==(signature?0x3c0af0u:0x3c0c20u)&&
            n.formatReturnPC==(signature?0x2b98fcu:0x2b9c78u)&&n.pc==0x308c90u;
    }
    if(n.pointerPublished||n.loopCount||n.loopCountRegister||n.loopTaken||!noFormat()||a.nativeResumePC!=0x2b9db0u||
       !n.wrapperEntered||!n.wrapperFrame||n.wrapperSaveMask!=1023u||n.zoneConstructor!=0x2461e0u||
       n.zoneDestructor!=0x246260u||n.zoneSize!=36u||!n.zoneCount||n.zoneCount!=e.zoneCount||
       n.zoneCountRegister!=e.zoneCountRegister||n.cookieWrites!=2u||n.elementOffset!=16u||
       n.unwindStoreMask!=63u||n.unwindSize!=36u||n.unwindCount!=n.zoneCount||
       n.unwindDestructor!=0x246260u||n.initialOrdinalSlot!=n.zoneCount||n.ordinal!=0u||n.constructorFlag!=1u)return false;
    if(!n.nativeViewsRetired){
        const auto& b=sourceWaterProvider_->nativeArrayBacking;
        if(sourceWaterProvider_->nativeArrayC8||sourceWaterProvider_->nativeArray68||!b.payload||b.effectiveBytes<16u||
           b.zoneCookieSize!=36u||b.zoneCookieCount!=n.zoneCount)return false;
        const auto* bytes=static_cast<const unsigned char*>(b.payload.get());
        for(std::uint32_t i=0;i<4u;++i)
            if(bytes[i]!=((36u>>(i*8u))&0xffu)||bytes[4u+i]!=((*n.zoneCount>>(i*8u))&0xffu))return false;
        //This caller layer reads only cookie bytes0..7. Padding8..15 stays unknown;
        //the separate zone-element descendant owns any later element knownness.
    }
    if(*n.zoneCount)return n.constructorCallIssued&&!n.wrapperReturned&&!n.wrapperStackReleased&&
        !n.wrapperRestoreMask&&n.pc==0x2461e0u;
    return !n.constructorCallIssued&&n.wrapperReturned&&n.wrapperStackReleased&&
        n.wrapperRestoreMask==1023u&&n.pc==0x2b9dd0u;
}
Owner::WaterArrayCallerResult Owner::resumeWaterArrayCallerNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArrayCallerResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& n=sourceWaterConfig_.nativeWaterArrayCaller;const auto& a=sourceWaterConfig_.nativeWaterArrayBacking;
    const auto& e=sourceWaterConfig_.nativeWaterAllocationEntry;auto& backing=sourceWaterProvider_->nativeArrayBacking;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;result.allocatorStop=e.pc;
    if(!a.nativeAllocated||a.nativeRetired||!a.role||a.epoch!=1u||!a.nativeResumePC||!backing.payload)return result;
    const bool zones=*a.role==NativeWaterArrayRole::zoneRaw;
    if(zones&&(!e.zoneCount||!e.zoneCountRegister||backing.effectiveBytes<16u))return result;
    const auto finish=[&](WaterArrayCallerResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.pointerPublished=n.pointerPublished;
        result.formatCallIssued=n.formatCallIssued;result.constructorCallIssued=n.constructorCallIssued;
        result.wrapperReturned=n.wrapperReturned;return result;
    };
    if(n.attempted)return finish(WaterArrayCallerResult::Status::alreadyAttempted);
    n.attempted=true;n.role=a.role;n.pc=*a.nativeResumePC;
    if(!zones){
        const bool signature=*n.role==NativeWaterArrayRole::signaturePointers;
        auto& field=signature?sourceWaterProvider_->nativeArrayC8:sourceWaterProvider_->nativeArray68;
        field=NativeWaterArrayView{*n.role,*a.epoch,0u};n.pointerPublished=true; //SW +C8 or+68, semantic role only.
        n.pc=signature?0x2b98e0u:0x2b9c5cu;
        n.pc=signature?0x2b98e4u:0x2b9c60u;n.ordinal=0u; //POR s0,zero,zero in jump delay.
        n.pc=signature?0x2b9bc0u:0x2b9d78u;
        const auto count=signature?sourceWaterProvider_->signatureWaves6C:sourceWaterProvider_->numPools64;
        if(!count)return finish(WaterArrayCallerResult::Status::nativeAdmissionStopped);
        n.loopCount=count;n.loopCountRegister=std::uint64_t(*count)|((*count&0x80000000u)?0xffffffff00000000ull:0ull);
        n.pc=signature?0x2b9bc4u:0x2b9d7cu;n.loopTaken=*n.ordinal<*n.loopCountRegister;
        n.pc=signature?0x2b9bc8u:0x2b9d80u;n.pc=signature?0x2b9bccu:0x2b9d84u; //NOP delay.
        if(!*n.loopTaken){n.pc=signature?0x2b9bd0u:0x2b9d88u;return finish(WaterArrayCallerResult::Status::nativeContinued);}
        n.pc=signature?0x2b98e8u:0x2b9c64u;
        n.pc=signature?0x2b98ecu:0x2b9c68u;n.formatIndex=1u;
        n.pc=signature?0x2b98f0u:0x2b9c6cu;n.formatStackOffset=0x710u; //Symbolic caller-stack destination, contents unknown.
        n.pc=signature?0x2b98f4u:0x2b9c70u;n.formatCallIssued=true;
        n.formatReturnPC=signature?0x2b98fcu:0x2b9c78u;
        n.pc=signature?0x2b98f8u:0x2b9c74u;n.formatLiteral=signature?0x3c0af0u:0x3c0c20u;
        n.pc=0x308c90;return finish(WaterArrayCallerResult::Status::nativeContinued); //No formatter body/output/return.
    }
    n.pc=0x2b9db0; //a0 is the exact private raw-backing role, not an EE allocation result.
    n.pc=0x2b9db4;n.pc=0x2b9db8;n.zoneConstructor=0x2461e0u;
    n.pc=0x2b9dbc;n.pc=0x2b9dc0;n.zoneDestructor=0x246260u;
    n.pc=0x2b9dc4;n.zoneSize=36u;
    n.pc=0x2b9dc8;n.wrapperEntered=true;
    n.pc=0x2b9dcc;n.zoneCount=e.zoneCount;n.zoneCountRegister=e.zoneCountRegister; //Retained caller s0, no new LW.
    n.pc=0x2e8990;n.wrapperFrame=true; //192-byte symbolic frame, saved register bits remain unknown.
    static constexpr std::array<std::uint32_t,10> savePC{{0x2e8994u,0x2e8998u,0x2e899cu,0x2e89a0u,0x2e89a4u,
        0x2e89a8u,0x2e89acu,0x2e89b0u,0x2e89b4u,0x2e89b8u}};
    for(std::size_t i=0;i<savePC.size();++i){n.pc=savePC[i];n.wrapperSaveMask|=std::uint16_t(1u<<i);}
    n.pc=0x2e89bc;n.pc=0x2e89c0;n.pc=0x2e89c4;
    n.pc=0x2e89c8;n.pc=0x2e89cc; //Native-success raw role is nonnull; preserve source branch delay.
    auto* bytes=static_cast<unsigned char*>(backing.payload.get());
    n.pc=0x2e89d0;for(std::uint32_t i=0;i<4u;++i)bytes[i]=static_cast<unsigned char>(36u>>(i*8u));
    backing.zoneCookieSize=36u;++n.cookieWrites;
    n.pc=0x2e89d4;for(std::uint32_t i=0;i<4u;++i)bytes[4u+i]=static_cast<unsigned char>(*n.zoneCount>>(i*8u));
    backing.zoneCookieCount=n.zoneCount;++n.cookieWrites;
    n.pc=0x2e89d8;n.pc=0x2e89dc;n.elementOffset=16u; //Fixed nonnull constructor; count0 gives an end view only.
    n.pc=0x2e89e0;n.pc=0x2e89e4;n.unwindStoreMask|=1u; //A0=element view.
    n.pc=0x2e89e8;n.pc=0x2e89ec;n.unwindSize=36u;n.unwindStoreMask|=2u;
    n.pc=0x2e89f0;n.pc=0x2e89f4;n.unwindCount=n.zoneCount;n.unwindStoreMask|=4u;
    n.pc=0x2e89f8;n.unwindDestructor=n.zoneDestructor;n.unwindStoreMask|=8u;
    n.pc=0x2e89fc;n.pc=0x2e8a00;n.pc=0x2e8a04;
    n.pc=0x2e8a08;n.initialOrdinalSlot=n.zoneCount;n.unwindStoreMask|=16u;
    n.pc=0x2e8a0c;n.pc=0x2e8a10;n.ordinal=0u;n.unwindStoreMask|=32u;
    n.pc=0x2e8a30;n.pc=0x2e8a34;
    const bool construct=*n.ordinal<*n.zoneCountRegister;
    n.pc=0x2e8a38;n.pc=0x2e8a3c;n.constructorFlag=1u; //Actual delay on both arms.
    if(construct){
        n.pc=0x2e8a18;n.constructorCallIssued=true;n.pc=0x2e8a1c;
        n.pc=0x2461e0;return finish(WaterArrayCallerResult::Status::nativeContinued); //First element body not entered.
    }
    n.pc=0x2e8a40;n.pc=0x2e8a44;n.pc=0x2e8a48;n.pc=0x2e8a4c; //Zero count: return element-base view in delay.
    static constexpr std::array<std::uint32_t,10> restorePC{{0x2e8aacu,0x2e8ab0u,0x2e8ab4u,0x2e8ab8u,0x2e8abcu,
        0x2e8ac0u,0x2e8ac4u,0x2e8ac8u,0x2e8accu,0x2e8ad0u}};
    for(std::size_t i=0;i<restorePC.size();++i){n.pc=restorePC[i];n.wrapperRestoreMask|=std::uint16_t(1u<<i);}
    n.pc=0x2e8ad4;n.pc=0x2e8ad8;n.wrapperStackReleased=true;n.wrapperReturned=true;
    n.pc=0x2b9dd0;return finish(WaterArrayCallerResult::Status::nativeContinued); //Before D0 store; no elements constructed.
}

#include "original_source_world_water_zone_elements50la.inl"

#include "original_source_world_water_array_format50la.inl"

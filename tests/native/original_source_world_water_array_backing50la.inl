bool Owner::preflightNativeWaterArrayBacking()const noexcept {
    if(!preflightNativeWaterArrayCaller())return false;
    const auto& n=sourceWaterConfig_.nativeWaterArrayBacking;
    const auto& e=sourceWaterConfig_.nativeWaterAllocationEntry;
    const auto& p=sourceWaterConfig_.nativeWaterArrayPrefix;
    using Refusal=WaterArrayBackingRefusal;using G=a::OriginalConfigProcessGlobals50LA;
    const auto backingPristine=[&]()noexcept {
        if(!sourceWaterProvider_)return true;
        const auto& b=sourceWaterProvider_->nativeArrayBacking;
        return !b.payload&&!b.slots&&!b.role&&!b.epoch&&!b.originalCount&&!b.requestedBytes&&
            !b.effectiveBytes&&!b.slotCapacity&&!b.zoneCookieSize&&!b.zoneCookieCount&&
            !b.zoneElementsCompleted&&!b.zoneCompletedVtable;
    };
    const auto noAllocation=[&]()noexcept {
        return !n.sidecarAttempted&&!n.sidecarPrepared&&!n.temporarySidecarsRetired&&!n.payloadAttempted&&
            !n.nativeAllocated&&!n.nativeRetired&&!n.epoch&&!n.nativeResumePC&&backingPristine();
    };
    if(!n.attempted)return !n.boundsAdmitted&&n.refusal==Refusal::none&&!n.role&&!n.count&&!n.requestedBytes&&
        !n.effectiveBytes&&!n.slotCapacity&&!n.nativeAdmissionReent&&noAllocation();
    if(!e.attempted||!e.lockReturned||(e.pc!=0x3059ecu&&e.pc!=0x305ac4u)||!e.callerReturnPC||
       !e.requestedBytes||!e.effectiveBytes||!n.role||!n.count||n.requestedBytes!=e.requestedBytes||
       n.effectiveBytes!=e.effectiveBytes||!sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||
       sourceWaterConfigOwner_->wakeSelection_.returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    const bool zones=*n.role==NativeWaterArrayRole::zoneRaw;
    if(zones!=e.zones)return false;
    if(zones){if(n.count!=e.zoneCount)return false;}
    else if(!p.signatureArm||n.count!=p.countWord||
        *n.role!=(*p.signatureArm?NativeWaterArrayRole::signaturePointers:NativeWaterArrayRole::poolPointers))return false;
    const auto expected=zones?std::uint64_t(*n.count)*36u+16u:(std::uint64_t(*n.count)+1u)*4u;
    const auto boundsRefusal=*n.count>nativeWaterArrayMaxCount?Refusal::countCapacity:
        expected>std::numeric_limits<std::uint32_t>::max()||expected!=*n.requestedBytes||
        *n.effectiveBytes!=*n.requestedBytes?Refusal::requestMismatch:
        expected>nativeWaterArrayMaxBytes||expected>std::numeric_limits<std::size_t>::max()?Refusal::byteCapacity:Refusal::none;
    if(boundsRefusal!=Refusal::none)return !n.boundsAdmitted&&n.refusal==boundsRefusal&&!n.slotCapacity&&
        !n.nativeAdmissionReent&&noAllocation();
    if(!n.boundsAdmitted||n.slotCapacity!=(zones?0u:*n.count+1u))return false;
    if(n.refusal==Refusal::reentrancyUnknown)return !n.nativeAdmissionReent&&noAllocation();
    if(n.refusal==Refusal::reentrancyUnsupported)return n.nativeAdmissionReent&&
        *n.nativeAdmissionReent!=G::StdioIdentity::reent385160&&noAllocation();
    if(n.nativeAdmissionReent!=G::StdioIdentity::reent385160||n.sidecarAttempted!=!zones)return false;
    if(n.refusal==Refusal::sidecarAllocation)return !zones&&!n.sidecarPrepared&&!n.temporarySidecarsRetired&&
        !n.payloadAttempted&&!n.nativeAllocated&&!n.nativeRetired&&!n.epoch&&!n.nativeResumePC&&backingPristine();
    if(n.sidecarPrepared!=!zones||!n.payloadAttempted)return false;
    if(n.refusal==Refusal::payloadAllocation)return n.temporarySidecarsRetired==!zones&&
        !n.nativeAllocated&&!n.nativeRetired&&!n.epoch&&!n.nativeResumePC&&backingPristine();
    if(n.refusal!=Refusal::none||n.temporarySidecarsRetired||!n.nativeAllocated||n.epoch!=1u||
       n.nativeResumePC!=e.callerReturnPC||n.nativeRetired!=sourceWaterProvider274720_.nativeWithdrawn)return false;
    if(n.nativeRetired)return !sourceWaterProvider_; //Historical role survives, never host backing access.
    const auto& b=sourceWaterProvider_->nativeArrayBacking;
    if(!b.payload||bool(b.slots)!=!zones||b.role!=n.role||b.epoch!=*n.epoch||b.originalCount!=*n.count||
       b.requestedBytes!=*n.requestedBytes||b.effectiveBytes!=*n.effectiveBytes||b.slotCapacity!=*n.slotCapacity)return false;
    for(std::uint32_t i=0;i<b.slotCapacity;++i)
        if(b.slots[i].known||b.slots[i].role||b.slots[i].offset)return false;
    return true; //This backing layer reads no payload bytes; descendants validate their established bytes.
}
Owner::WaterArrayBackingResult Owner::allocateWaterArrayBackingNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArrayBackingResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& n=sourceWaterConfig_.nativeWaterArrayBacking;const auto& e=sourceWaterConfig_.nativeWaterAllocationEntry;
    const auto& p=sourceWaterConfig_.nativeWaterArrayPrefix;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;
    if(!e.attempted||!e.lockReturned||(e.pc!=0x3059ecu&&e.pc!=0x305ac4u)||
       !e.requestedBytes||!e.effectiveBytes||!e.callerReturnPC||
       (e.zones?!e.zoneCount:(!p.countWord||!p.signatureArm)))return result;
    result.allocatorStop=e.pc;
    const auto finish=[&](WaterArrayBackingResult::Status status)noexcept {
        result.status=status;result.refusal=n.refusal;result.nativeResumePC=n.nativeResumePC;
        result.requestedBytes=n.requestedBytes;result.effectiveBytes=n.effectiveBytes;
        result.nativeAllocated=n.nativeAllocated;return result;
    };
    if(n.attempted)return finish(WaterArrayBackingResult::Status::alreadyAttempted);
    using Refusal=WaterArrayBackingRefusal;using G=a::OriginalConfigProcessGlobals50LA;
    n.attempted=true;n.role=e.zones?NativeWaterArrayRole::zoneRaw:
        (*p.signatureArm?NativeWaterArrayRole::signaturePointers:NativeWaterArrayRole::poolPointers);
    n.count=e.zones?e.zoneCount:p.countWord;n.requestedBytes=e.requestedBytes;n.effectiveBytes=e.effectiveBytes;
    const auto refuse=[&](Refusal reason)noexcept {
        n.refusal=reason;return finish(WaterArrayBackingResult::Status::nativeAdmissionStopped);
    };
    //Explicit native capacity/aperture, NOT original range/overflow validation.
    if(*n.count>nativeWaterArrayMaxCount)return refuse(Refusal::countCapacity);
    const auto expected=e.zones?std::uint64_t(*n.count)*36u+16u:(std::uint64_t(*n.count)+1u)*4u;
    if(expected>std::numeric_limits<std::uint32_t>::max()||expected!=*n.requestedBytes||
       *n.effectiveBytes!=*n.requestedBytes)return refuse(Refusal::requestMismatch);
    if(expected>nativeWaterArrayMaxBytes||expected>std::numeric_limits<std::size_t>::max())return refuse(Refusal::byteCapacity);
    n.boundsAdmitted=true;n.slotCapacity=e.zones?0u:*n.count+1u;
    //Fresh NATIVE admission only. Do not replay or replace source30570C capture.
    if(!G::reentrancyCurrentNative())return refuse(Refusal::reentrancyUnknown);
    n.nativeAdmissionReent=G::state().reent38544C;
    if(*n.nativeAdmissionReent!=G::StdioIdentity::reent385160)return refuse(Refusal::reentrancyUnsupported);
    std::unique_ptr<NativeWaterArraySlot[]> slots;
    if(!e.zones){
        n.sidecarAttempted=true;
        slots.reset(new(std::nothrow) NativeWaterArraySlot[*n.slotCapacity]);
        if(!slots)return refuse(Refusal::sidecarAllocation);
        n.sidecarPrepared=true; //All semantic slots UNKNOWN, not source NULL.
    }
    n.payloadAttempted=true;
    void* const payload=std::malloc(static_cast<std::size_t>(*n.effectiveBytes));
    if(!payload){
        slots.reset();n.temporarySidecarsRetired=n.sidecarPrepared;
        return refuse(Refusal::payloadAllocation); //Native refusal, never guest NULL/errno/new_handler.
    }
    //All fallible preparation finished. Transfer private native custody once.
    //Neither source field stores nor original allocator/frame returns occur.
    auto& backing=sourceWaterProvider_->nativeArrayBacking;
    backing.payload.reset(payload);backing.slots=std::move(slots);backing.role=n.role;
    backing.epoch=1u;backing.originalCount=*n.count;backing.requestedBytes=*n.requestedBytes;
    backing.effectiveBytes=*n.effectiveBytes;backing.slotCapacity=*n.slotCapacity;
    n.epoch=backing.epoch;n.nativeAllocated=true;n.nativeResumePC=e.callerReturnPC;
    return finish(WaterArrayBackingResult::Status::nativeAllocated);
}

#include "original_source_world_water_array_caller50la.inl"

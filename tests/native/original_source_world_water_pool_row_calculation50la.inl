bool Owner::preflightNativeWaterPoolRowCalculation()const noexcept {
    using S=WaterPoolRowCalculationStop;using G=a::OriginalConfigProcessGlobals50LA;
    using A=g::OriginalWaterRowAssertion50LA;
    const auto& n=sourceWaterConfig_.nativeWaterPoolRowCalculation;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRowBacking;
    const auto& input=sourceWaterConfig_.nativeWaterPoolRowInput;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto* topology=sourceWaterProvider_&&sourceWaterProvider_->nativePool&&sourceWaterProvider_->nativePool->nativeData?
        sourceWaterProvider_->nativePool->nativeData->nativeTopology.get():nullptr;
    const auto* object=topology&&topology->firstRecord?topology->firstRecord->firstInner.get():nullptr;
    const auto* rows=object?object->rows.get():nullptr;
    const auto* row=rows?rows->firstRow.get():nullptr;
    const auto noOutput=[&]()noexcept {
        return !n.outputAttempted&&!n.allocated&&!n.aliasStored&&!n.outputIdentity&&!n.assertionAttempted&&
            n.assertion==A{}&&!n.numericAttempted&&!n.numeric.prepared&&n.numeric.pc==0x299420u;
    };
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.sourceStopPC&&!n.sourceProduct&&!n.sourceReent&&!n.nativeReent&&
        !n.rowIdentity&&noOutput()&&(!row||(!row->output10&&!row->storage.outputIdentity()));
    if(input.stop!=WaterPoolRowInputStop::calculation299A38||!b.count||!n.rowIdentity||
       n.rowIdentity!=input.rowIdentity||n.rowIdentity!=b.ownerIdentity)return false;
    if(n.sourceReent!=G::StdioIdentity::reent385160)return n.stop==S::sourceReentrancy&&n.pc==0x299448u&&
        n.sourceStopPC==0x302e68u&&!n.sourceProduct&&!n.nativeReent&&noOutput();
    if(n.sourceStopPC!=0x305990u||n.sourceProduct!=*b.count*16u)return false;
    if(n.stop==S::nativeReentrancy)return n.pc==0x299448u&&n.nativeReent!=G::StdioIdentity::reent385160&&noOutput();
    if(n.nativeReent!=G::StdioIdentity::reent385160||!n.outputAttempted)return false;
    if(b.retired){if(row||!f.file||!f.nativeWithdrawn||!f.file->nativeClosed())return false;}
    else if(!row||row!=n.rowIdentity||row->storage.outputIdentity()!=n.outputIdentity||
            row->output10!=(n.aliasStored?n.outputIdentity:nullptr))return false;
    if(n.stop==S::outputAllocation)return n.pc==0x299448u&&!n.allocated&&!n.aliasStored&&!n.outputIdentity&&
        !n.assertionAttempted&&n.assertion==A{}&&!n.numericAttempted&&!n.numeric.prepared&&n.numeric.pc==0x299420u;
    if(!n.allocated||!n.aliasStored||!n.outputIdentity||!n.assertionAttempted)return false;
    if(n.assertion.status==A::Status::invalidGlobals){
        A expected;expected.status=A::Status::invalidGlobals;expected.pc=0x299454u;
        return n.stop==S::assertion&&n.pc==0x299454u&&n.assertion==expected&&
            !n.numericAttempted&&!n.numeric.prepared&&n.numeric.pc==0x299420u;
    }
    if(n.assertion.status!=A::Status::accepted||!n.assertion.guardBefore||*n.assertion.guardBefore>1u)return false;
    g::OriginalWaterRowGlobals50LA before;
    before.guard3D6644=*n.assertion.guardBefore;before.assertion443638=*n.assertion.guardBefore;
    if(n.assertion!=g::admitOriginalWaterRowOutput50LA(before,n.outputIdentity)||!n.numericAttempted)return false;
    if(n.numeric.prepared){
        if(n.stop!=S::xyzPrepared299780||n.pc!=0x299780u||n.numeric.pc!=0x299780u||*b.count<2u)return false;
    }else if(n.stop!=S::numeric||n.pc!=n.numeric.pc)return false;
    if(!b.retired)for(std::uint32_t i=0;i<*b.count;++i)
        if(row->storage.outputXYZ(i).has_value()!=n.numeric.prepared)return false;
    return true;
}

Owner::WaterPoolRowCalculationResult Owner::calculateFirstWaterPoolRowNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolRowCalculationResult;using S=WaterPoolRowCalculationStop;using G=a::OriginalConfigProcessGlobals50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolRowCalculation;
    const auto& input=sourceWaterConfig_.nativeWaterPoolRowInput;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRowBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.sourceStopPC=n.sourceStopPC;
        result.allocated=n.allocated;result.prepared=n.numeric.prepared;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       input.stop!=WaterPoolRowInputStop::calculation299A38||b.retired||!worldApplication_||!f.file||f.file->nativeClosed()||
       f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& row=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord->firstInner->rows->firstRow;
    n.attempted=true;n.rowIdentity=&row;n.pc=0x299448u;n.stop=S::sourceReentrancy;n.sourceStopPC=0x302e68u;
    //299448 calls302E58 with N and16, whose wrapper reads reent38544C.
    //302E88 MULT/MFLO forms N*16;302E9C calls305990 directly, not305700.
    if(G::reentrancyCurrentNative())n.sourceReent=G::state().reent38544C;
    if(n.sourceReent!=G::StdioIdentity::reent385160)return finish(R::Status::nativeStopped);
    n.sourceProduct=*b.count*16u;n.sourceStopPC=0x305990u; //Before allocator body; no source return.
    n.stop=S::nativeReentrancy;
    if(G::reentrancyCurrentNative())n.nativeReent=G::state().reent38544C;
    if(n.nativeReent!=G::StdioIdentity::reent385160)return finish(R::Status::nativeStopped);
    n.stop=S::outputAllocation;n.outputAttempted=true;
    if(!row.storage.allocateOutput())return finish(R::Status::nativeStopped);
    n.allocated=true;n.outputIdentity=row.storage.outputIdentity();row.output10=n.outputIdentity;n.aliasStored=true;
    //Native helper zeroed only requested16*N bytes. The original allocator's
    //chunk-header-derived zero extent and its return remain unestablished.
    n.stop=S::assertion;n.assertionAttempted=true;
    n.assertion=g::admitOriginalWaterRowOutput50LA(worldApplication_->sourceWaterRowGlobals_,n.outputIdentity);
    n.pc=n.assertion.pc;
    if(n.assertion.status!=g::OriginalWaterRowAssertion50LA::Status::accepted)return finish(R::Status::nativeStopped);
    n.stop=S::numeric;n.numericAttempted=true;n.numeric=row.storage.calculateXYZ();n.pc=n.numeric.pc;
    if(!n.numeric.prepared)return finish(R::Status::nativeStopped);
    n.stop=S::xyzPrepared299780;
    return finish(R::Status::nativeXYZPrepared); //XYZ only, before source epilogue; no fullW/loader return claim.
}

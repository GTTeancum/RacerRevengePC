//Explicit native-only continuation of paired success-side backing ownership.
//This NEVER executes or completes304900, and never advances original pc_.
bool OriginalWaterSourceConfig50LA::retireWakeBackingNativeOnly()noexcept {
    auto& n=nativeWakeRetirement_;const auto& w=wakeSelection_;
    if(n.attempted||nativeClosed_||!borrowed_||thread_!=std::this_thread::get_id()||
       pc_!=0x30492cu||!w.heapHeaderBoundary||!w.lockReturned||!w.nonnullFreeEntered||
       !w.seekReturned||w.returned||w.nullChildBoundary||sectionReturn_!=true||
       w.childIdentity!=&sectionTableRole_||w.dataIdentity!=&sectionDataRole_||
       !sectionTable_||!sectionData_||!sectionTableAllocation_.returned||!sectionDataAllocation_.returned||
       !preflightNative(waterReceiver_))return false;
    //Every historical getter owns copies, not borrowed table/data spans.
    //Config is private/nonmovable; only this synchronous occurrence may read
    //these buffers. No raw pointer is exported by the explicit Owner entry.
    for(const auto& f:floatGetters_)if(!f.returned)return false;
    for(const auto& f:integerGetters_)if(!f.returned)return false;
    if(!shaderGetter_.returned)return false;
    n.attempted=true;
    using G=OriginalConfigProcessGlobals50LA;
    //A fresh native admission check, NOT replay of the first source wrapper.
    //w.freeReent remains its original historical capture at305734.
    if(!G::reentrancyCurrentNative())return false;
    n.dataAdmissionReent=G::state().reent38544C;
    if(*n.dataAdmissionReent!=G::StdioIdentity::reent385160)return false;
    sectionData_.reset();n.dataRetired=true;
    //Original call order is data157FB0, then table157FB8/BC. Crossing the
    //unknown first free is explicitly native adaptation, not source return.
    //Keep semantic child24 and count20 unchanged while table remains live.
    n.tableGateReached=true;
    if(!G::reentrancyCurrentNative())return false;
    n.tableAdmissionReent=G::state().reent38544C;
    if(*n.tableAdmissionReent!=G::StdioIdentity::reent385160)return false;
    sectionTable_.reset();n.tableRetired=true;
    //Native projection of157FC0/157FC4 only after BOTH native retirements.
    source_.selected20=0;n.countCleared=true;
    source_.child24=nullptr;n.childCleared=true;
    n.adaptedResumePC=0x157fe8u; //Before line08/new loading. Not entered here.
    return true;
}
bool OriginalWaterSourceConfig50LA::preflightWakeRetirementNative()const noexcept {
    const auto& n=nativeWakeRetirement_;const auto& w=wakeSelection_;
    if(sectionTableRole_.owner!=this||sectionTableRole_.epoch!=1u||
       sectionDataRole_.owner!=this||sectionDataRole_.epoch!=2u)return false;
    if(!n.attempted)return !n.dataRetired&&!n.tableGateReached&&!n.tableRetired&&!n.countCleared&&
        !n.childCleared&&!n.dataAdmissionReent&&!n.tableAdmissionReent&&!n.adaptedResumePC;
    if(pc_!=0x30492cu||!w.heapHeaderBoundary||!w.lockReturned||!w.nonnullFreeEntered||
       !w.freeCallIssued||!w.seekReturned||w.returned||w.nullChildBoundary||sectionReturn_!=true||
       w.childIdentity!=&sectionTableRole_||w.dataIdentity!=&sectionDataRole_||
       !sectionTableAllocation_.returned||!sectionDataAllocation_.returned||!shaderGetter_.returned)return false;
    for(const auto& f:floatGetters_)if(!f.returned)return false;
    for(const auto& f:integerGetters_)if(!f.returned)return false;
    using G=OriginalConfigProcessGlobals50LA;
    if(n.dataRetired!=(n.dataAdmissionReent==G::StdioIdentity::reent385160)||
       n.tableGateReached!=n.dataRetired||
       n.tableRetired!=(n.tableAdmissionReent==G::StdioIdentity::reent385160)||
       (n.tableAdmissionReent&&!n.tableGateReached)||
       (n.tableRetired&&!n.dataRetired)||n.countCleared!=n.tableRetired||
       n.childCleared!=n.tableRetired||bool(n.adaptedResumePC)!=n.tableRetired||
       (n.adaptedResumePC&&*n.adaptedResumePC!=0x157fe8u))return false;
    //Native withdrawal may later release the stream; the historical native
    //gate values need not equal a subsequently changed process reentrancy.
    return bool(sectionData_)==!n.dataRetired&&bool(sectionTable_)==!n.tableRetired&&
        source_.selected20==currentSectionCountNative()&&source_.child24==currentSectionChildNative();
}

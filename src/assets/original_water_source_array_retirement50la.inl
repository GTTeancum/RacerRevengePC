//Separate native ownership operation AFTER the new selection's heap stop.
//This is not304900 execution, a source free return or an original heap receipt.
bool OriginalWaterSourceConfig50LA::retireArraySectionBackingNativeOnly()noexcept {
    auto& n=nativeArrayRetirement_;const auto& s=nativeArraySelection_;auto& w=nativeWakeLoad_;
    if(n.attempted||nativeClosed_||!borrowed_||thread_!=std::this_thread::get_id()||
       pc_!=0x30492cu||s.pc!=0x30492cu||!s.heapHeaderBoundary||!s.lockReturned||!s.nonnullFreeEntered||
       !s.seekReturned||s.returned||!s.index||*s.index<0||s.childIdentity!=&wakeTableRole_||
       s.dataIdentity!=&wakeDataRole_||!w.nativeLoaded||!w.table||!w.data||
       !w.tableAllocation.returned||!w.dataAllocation.returned||nativeWakeGetters_.completed!=5u||
       nativeWakeGetters_.active||!preflightNative(waterReceiver_))return false;
    //Wake getters retain copies, and rows/offsets remain private historical
    //metadata. Completed getter scope cannot consume retired backing; no raw
    //table/data borrow escapes this synchronous, nonmovable Config owner.
    n.attempted=true;
    using G=OriginalConfigProcessGlobals50LA;
    if(!G::reentrancyCurrentNative())return false;
    n.dataAdmissionReent=G::state().reent38544C;
    if(*n.dataAdmissionReent!=G::StdioIdentity::reent385160)return false;
    w.data.reset();n.dataRetired=true; //Native std::free only; s.freeReent stays historical.
    n.tableGateReached=true;
    //Separate fresh admission for table retirement, in the original paired
    //data-before-table order. This does not issue the source157FB8 call.
    if(!G::reentrancyCurrentNative())return false;
    n.tableAdmissionReent=G::state().reent38544C;
    if(*n.tableAdmissionReent!=G::StdioIdentity::reent385160)return false;
    w.table.reset();n.tableRetired=true;
    //Native projection of157FC0/C4 only after BOTH native payloads retire.
    source_.selected20=0;n.countCleared=true;
    source_.child24=nullptr;n.childCleared=true;
    n.adaptedResumePC=0x157fe8u; //Before line08/new name/read/alloc; not entered here.
    return true;
}
bool OriginalWaterSourceConfig50LA::preflightArrayRetirementNative()const noexcept {
    const auto& n=nativeArrayRetirement_;const auto& s=nativeArraySelection_;const auto& w=nativeWakeLoad_;
    if(!n.attempted)return !n.dataRetired&&!n.tableGateReached&&!n.tableRetired&&!n.countCleared&&
        !n.childCleared&&!n.dataAdmissionReent&&!n.tableAdmissionReent&&!n.adaptedResumePC;
    if(pc_!=0x30492cu||s.pc!=0x30492cu||!s.heapHeaderBoundary||!s.lockReturned||!s.nonnullFreeEntered||
       !s.freeCallIssued||!s.seekReturned||s.returned||!s.index||*s.index<0||
       s.childIdentity!=&wakeTableRole_||s.dataIdentity!=&wakeDataRole_||
       !w.nativeLoaded||!w.tableAllocation.returned||!w.dataAllocation.returned||
       w.pointerZeroRole!=&wakeDataRole_||nativeWakeGetters_.completed!=5u||nativeWakeGetters_.active)return false;
    using G=OriginalConfigProcessGlobals50LA;
    if(n.dataRetired!=(n.dataAdmissionReent==G::StdioIdentity::reent385160)||
       n.tableGateReached!=n.dataRetired||n.tableRetired!=(n.tableAdmissionReent==G::StdioIdentity::reent385160)||
       (n.tableAdmissionReent&&!n.tableGateReached)||(n.tableRetired&&!n.dataRetired)||
       n.countCleared!=n.tableRetired||n.childCleared!=n.tableRetired||
       bool(n.adaptedResumePC)!=n.tableRetired||(n.adaptedResumePC&&*n.adaptedResumePC!=0x157fe8u))return false;
    //Historical gate captures need not equal later process globals. Metadata
    //roles3/4 and completed load/getter histories survive native withdrawal.
    return bool(w.data)==!n.dataRetired&&bool(w.table)==!n.tableRetired&&
        source_.selected20==currentSectionCountNative()&&source_.child24==currentSectionChildNative();
}

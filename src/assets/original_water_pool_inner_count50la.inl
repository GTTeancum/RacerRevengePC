bool OriginalWaterPoolSourceFile50LA::readInnerCount12FB30(
    const void* data,const void* object,std::uint64_t epoch,D& d)noexcept {
    d={};auto& n=innerCount_;auto& r=n.read;
    if(r.entered||!data||!object||
       data!=firstTopologyElement_.dataIdentity||epoch!=firstTopologyElement_.dataEpoch||
       !firstTopologyElement_.read.resultAdmitted||!firstTopologyElement_.read.afterObserved||nativeClosed_||nativeCloseAttempted_||
       sourceCloseAttempted_||!preflightNative(poolIdentity_))return fail(d,E::phase,0x2998f8u);
    const ServiceObservation* prior=&firstTopologyElement_.read.after;
    n.dataIdentity=data;n.objectIdentity=object;n.dataEpoch=epoch;
    r.entered=true;r.pc=0x299908u;
    try {r.before.devices.resize(std::size_t(deviceCount_));r.after.devices.resize(std::size_t(deviceCount_));}
    catch(...){fail(d,E::allocation,r.pc);r.admissionDiagnostic=d;return false;}
    r.prepared=true;
    const auto reject=[&](E why,std::uint32_t pc)noexcept {
        fail(d,why,pc);r.admissionDiagnostic=d;r.pc=pc;return false;
    };
    r.beforeObserved=observeServiceNative(r.before);
    if(!r.beforeObserved||r.before.memberCursor10!=prior->memberCursor10)return reject(E::phase,r.pc);
    if(r.before.memberCursor10>selectedLength_||
       selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max()))
        return reject(E::unsupported,r.pc);
    r.clampedAmount=std::min<std::size_t>(4u,selectedLength_-r.before.memberCursor10);
    r.attempted=true;r.pc=0x12fb30u;
    //The299790 constructor zeroed object+4. A short count read retains
    //that known zero suffix; only the copied prefix is committed by the owner.
    r.nativeReturned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,nativeHandle_,r.bytes.data(),4u,r.copied,d);
    r.helperDiagnostic=d;r.afterObserved=observeServiceNative(r.after);
    if(!r.nativeReturned){r.pc=d.sourcePC?d.sourcePC:0x12fb30u;return false;}
    if(!r.afterObserved||r.copied!=r.clampedAmount||r.after.memberCursor10!=r.before.memberCursor10+r.copied)
        return reject(E::phase,0x12fb30u);
    r.sourceItems=r.copied?1:0;r.resultAdmitted=true;r.pc=0x299910u;return true;
}

bool OriginalWaterPoolSourceFile50LA::innerCountHistoryNative()const noexcept {
    const auto& n=innerCount_;const auto& r=n.read;
    if(!r.entered)return !n.dataIdentity&&!n.objectIdentity&&!n.dataEpoch&&!r.prepared&&
        !r.beforeObserved&&!r.attempted&&!r.nativeReturned&&!r.afterObserved&&!r.resultAdmitted&&
        !r.copied&&!r.clampedAmount&&!r.ordinal&&!r.pc&&!r.sourceItems&&
        r.before.devices.empty()&&r.after.devices.empty()&&r.bytes==std::array<std::uint8_t,4>{};
    if(!n.dataIdentity||!n.objectIdentity||
       n.dataEpoch!=1u||n.dataIdentity!=firstTopologyElement_.dataIdentity||
       n.dataEpoch!=firstTopologyElement_.dataEpoch||!firstTopologyElement_.read.resultAdmitted||!firstTopologyElement_.read.afterObserved||
       sourceCloseAttempted_||r.ordinal||r.copied>4u||r.copied>r.clampedAmount||r.clampedAmount>4u)return false;
    const ServiceObservation* prior=&firstTopologyElement_.read.after;
    for(std::size_t i=r.copied;i<4u;++i)if(r.bytes[i])return false;
    if(r.prepared){
        if(r.before.devices.size()!=std::size_t(deviceCount_)||r.after.devices.size()!=std::size_t(deviceCount_))return false;
    }else if(r.before.devices.size()>std::size_t(deviceCount_)||r.after.devices.size()>std::size_t(deviceCount_)||
             r.beforeObserved||r.attempted)return false;
    if(r.beforeObserved&&(!r.prepared||r.before.memberCursor10!=prior->memberCursor10))return false;
    if(!r.attempted)return !r.nativeReturned&&!r.afterObserved&&!r.resultAdmitted&&!r.sourceItems&&
        !r.copied&&!r.clampedAmount&&r.pc==0x299908u;
    if(!r.prepared||!r.beforeObserved||r.before.memberCursor10>selectedLength_||
       selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max())||
       r.clampedAmount!=std::min<std::size_t>(4u,selectedLength_-r.before.memberCursor10))return false;
    const bool admitted=r.nativeReturned&&r.afterObserved&&r.copied==r.clampedAmount&&
        r.after.memberCursor10==r.before.memberCursor10+r.copied;
    if(r.resultAdmitted!=admitted)return false;
    if(admitted)return r.pc==0x299910u&&r.sourceItems==(r.copied?1:0);
    if(r.sourceItems||r.pc!=(r.nativeReturned?0x12fb30u:r.helperDiagnostic.sourcePC?r.helperDiagnostic.sourcePC:0x12fb30u))return false;
    return r.nativeReturned||!r.afterObserved||r.after.memberCursor10==r.before.memberCursor10;
}

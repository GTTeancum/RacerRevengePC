//Included inside rr::assets with the existing file-custody implementation.
bool OriginalWaterPoolSourceFile50LA::beginIndexReaderNative(const void* data,const void* array,
    std::uint64_t epoch,std::uint32_t count,D& d)noexcept {
    d={};auto& n=indexReader_;
    if(n.entered||!data||!array||epoch!=1u||count>16384u||data!=topologyRead_.dataIdentity||
       epoch!=topologyRead_.dataEpoch||!topologyRead_.resultAdmitted||!topologyRead_.afterObserved||
       nativeClosed_||nativeCloseAttempted_||sourceCloseAttempted_||!preflightNative(poolIdentity_))
        return fail(d,E::phase,0x2babf4u);
    std::uint32_t actual=0;
    for(std::size_t i=0;i<4u;++i)actual|=std::uint32_t(topologyRead_.bytes[i])<<(i*8u);
    if(count!=actual)return fail(d,E::phase,0x2babf4u);
    n.entered=true;n.dataIdentity=data;n.arrayIdentity=array;n.dataEpoch=epoch;n.count=count;
    try {n.reads.resize(count?count-1u:0u);}
    catch(...){return fail(d,E::allocation,0x2babf4u);}
    n.prepared=true;return true;
}

bool OriginalWaterPoolSourceFile50LA::readIndex12FB30(std::uint32_t ordinal,D& d)noexcept {
    d={};auto& n=indexReader_;
    if(!n.prepared||ordinal>=n.reads.size()||n.reads[ordinal].entered||nativeClosed_||
       nativeCloseAttempted_||sourceCloseAttempted_||nativeHandle_<0||!preflightNative(poolIdentity_))
        return fail(d,E::phase,0x2bac04u);
    const ServiceObservation* prior=&topologyRead_.after;
    for(std::uint32_t i=0;i<ordinal;++i){
        if(!n.reads[i].resultAdmitted||!n.reads[i].afterObserved)return fail(d,E::phase,0x2bac04u);
        prior=&n.reads[i].after;
    }
    auto& r=n.reads[ordinal];r.entered=true;r.ordinal=ordinal;r.pc=0x2bac18u;
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
    //2BAC18/1C: same saved signed handle; one four-byte item, offset4*ordinal.
    r.attempted=true;r.pc=0x12fb30u;
    r.nativeReturned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,nativeHandle_,
        r.bytes.data(),4u,r.copied,d);
    r.helperDiagnostic=d;r.afterObserved=observeServiceNative(r.after);
    if(!r.nativeReturned){r.pc=d.sourcePC?d.sourcePC:0x12fb30u;return false;}
    if(!r.afterObserved||r.copied!=r.clampedAmount||r.after.memberCursor10!=r.before.memberCursor10+r.copied)
        return reject(E::phase,0x12fb30u);
    r.sourceItems=r.copied?1:0;r.resultAdmitted=true;r.pc=0x2bac20u;return true;
}

bool OriginalWaterPoolSourceFile50LA::indexReadHistoryNative()const noexcept {
    const auto& n=indexReader_;
    if(!n.entered)return !n.prepared&&!n.dataIdentity&&!n.arrayIdentity&&!n.dataEpoch&&!n.count&&n.reads.empty();
    if(!n.dataIdentity||!n.arrayIdentity||n.dataEpoch!=1u||n.count>16384u||
       n.dataIdentity!=topologyRead_.dataIdentity||n.dataEpoch!=topologyRead_.dataEpoch||
       !topologyRead_.resultAdmitted||!topologyRead_.afterObserved||sourceCloseAttempted_)return false;
    std::uint32_t count=0;for(std::size_t i=0;i<4u;++i)count|=std::uint32_t(topologyRead_.bytes[i])<<(8u*i);
    if(count!=n.count)return false;
    if(!n.prepared)return n.reads.empty();
    if(n.reads.size()!=(count?count-1u:0u))return false;
    const ServiceObservation* prior=&topologyRead_.after;bool stopped=false;
    for(std::size_t i=0;i<n.reads.size();++i){const auto& r=n.reads[i];
        if(!r.entered){
            stopped=true;
            if(r.prepared||r.beforeObserved||r.attempted||r.nativeReturned||r.afterObserved||r.resultAdmitted||
               r.copied||r.clampedAmount||r.ordinal||r.pc||r.sourceItems||!r.before.devices.empty()||
               !r.after.devices.empty()||r.bytes!=std::array<std::uint8_t,4>{})return false;
            continue;
        }
        if(stopped||r.ordinal!=i||r.copied>4u||r.copied>r.clampedAmount||r.clampedAmount>4u)return false;
        if(r.prepared){
            if(r.before.devices.size()!=std::size_t(deviceCount_)||r.after.devices.size()!=std::size_t(deviceCount_))return false;
        }else if(r.before.devices.size()>std::size_t(deviceCount_)||r.after.devices.size()>std::size_t(deviceCount_)||
                 r.beforeObserved||r.attempted)return false;
        if(r.beforeObserved&&(!r.prepared||r.before.memberCursor10!=prior->memberCursor10))return false;
        if(!r.attempted){
            if(r.nativeReturned||r.afterObserved||r.resultAdmitted||r.sourceItems||r.copied||r.clampedAmount||r.pc!=0x2bac18u)return false;
            stopped=true;continue;
        }
        if(!r.prepared||!r.beforeObserved||r.before.memberCursor10>selectedLength_||
           selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max())||
           r.clampedAmount!=std::min<std::size_t>(4u,selectedLength_-r.before.memberCursor10))return false;
        const bool admitted=r.nativeReturned&&r.afterObserved&&r.copied==r.clampedAmount&&
            r.after.memberCursor10==r.before.memberCursor10+r.copied;
        if(r.resultAdmitted!=admitted)return false;
        if(admitted){if(r.pc!=0x2bac20u||r.sourceItems!=(r.copied?1:0))return false;prior=&r.after;}
        else {
            if(r.sourceItems||r.pc!=(r.nativeReturned?0x12fb30u:r.helperDiagnostic.sourcePC?r.helperDiagnostic.sourcePC:0x12fb30u))return false;
            if(!r.nativeReturned&&r.afterObserved&&r.after.memberCursor10!=r.before.memberCursor10)return false;
            stopped=true;
        }
    }
    return true; //Opaque role history only: never calls aggregate preflights.
}

bool OriginalWaterPoolSourceFile50LA::prepareRowInputsNative(
    const void* data,const void* object,const void* rows,const void* row,std::uint64_t epoch,std::uint32_t count,D& d)noexcept {
    d={};auto& n=rowInputs_;const auto& prior=innerRowCount_;
    if(n.entered||!row||count>16384u||data!=prior.dataIdentity||object!=prior.objectIdentity||
       rows!=prior.rowsIdentity||epoch!=prior.dataEpoch||!prior.read.resultAdmitted||prior.read.copied!=4u||
       nativeClosed_||nativeCloseAttempted_||sourceCloseAttempted_||!preflightNative(poolIdentity_))return fail(d,E::phase,0x2999ccu);
    std::uint32_t actual=0;for(unsigned i=0;i<4u;++i)actual|=std::uint32_t(prior.read.bytes[i])<<(8u*i);
    if(actual!=count)return fail(d,E::phase,0x2999ccu);
    n.entered=true;n.dataIdentity=data;n.objectIdentity=object;n.rowsIdentity=rows;n.rowIdentity=row;
    n.dataEpoch=epoch;n.count=count;
    try {n.normals.resize(count);}catch(...){return fail(d,E::allocation,0x2999ccu);}
    n.prepared=true;return true;
}

bool OriginalWaterPoolSourceFile50LA::readRowInput12FB30(bool normal,std::uint32_t ordinal,D& d)noexcept {
    d={};auto& n=rowInputs_;
    const std::uint32_t callPC=normal?0x299a0cu:0x2999e0u;
    if(!n.prepared||nativeClosed_||nativeCloseAttempted_||sourceCloseAttempted_||
       (!normal&&ordinal)||(normal&&ordinal>=n.count)||!preflightNative(poolIdentity_))return fail(d,E::phase,callPC);
    auto& r=normal?n.normals[ordinal]:n.vertices;
    if(r.entered)return fail(d,E::phase,callPC);
    const ServiceObservation* prior=&innerRowCount_.read.after;
    if(normal){
        const auto& previous=ordinal?n.normals[ordinal-1u]:n.vertices;
        if(!previous.resultAdmitted||!previous.afterObserved)return fail(d,E::phase,callPC);
        prior=&previous.after;
    }
    const std::size_t request=normal?12u:std::size_t(n.count)*32u;
    const auto returnItems=normal?1u:n.count;
    r.entered=true;r.pc=callPC;
    try {r.bytes.resize(request);r.before.devices.resize(std::size_t(deviceCount_));r.after.devices.resize(std::size_t(deviceCount_));}
    catch(...){fail(d,E::allocation,callPC);r.admissionDiagnostic=d;return false;}
    r.prepared=true;
    const auto reject=[&](E why,std::uint32_t pc)noexcept {fail(d,why,pc);r.admissionDiagnostic=d;r.pc=pc;return false;};
    r.beforeObserved=observeServiceNative(r.before);
    if(!r.beforeObserved||r.before.memberCursor10!=prior->memberCursor10)return reject(E::phase,callPC);
    if(r.before.memberCursor10>selectedLength_||selectedLength_-r.before.memberCursor10>
       std::uint32_t(std::numeric_limits<std::int32_t>::max()))return reject(E::unsupported,callPC);
    r.clampedAmount=std::min<std::size_t>(request,selectedLength_-r.before.memberCursor10);
    r.attempted=true;r.pc=0x12fb30u;
    r.nativeReturned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,nativeHandle_,r.bytes.data(),request,r.copied,d);
    r.helperDiagnostic=d;r.afterObserved=observeServiceNative(r.after);
    if(!r.nativeReturned){r.pc=d.sourcePC?d.sourcePC:0x12fb30u;return false;}
    if(!r.afterObserved||r.copied!=r.clampedAmount||r.after.memberCursor10!=r.before.memberCursor10+r.copied)
        return reject(E::phase,0x12fb30u);
    //12FF30 returns requested itemCount for any positive exact clamped transfer,
    //even if fewer than one whole item was copied. Byte knownness is separate.
    r.sourceItems=r.copied?std::int32_t(returnItems):0;r.resultAdmitted=true;r.pc=callPC+8u;return true;
}

bool OriginalWaterPoolSourceFile50LA::rowInputsHistoryNative()const noexcept {
    const auto& n=rowInputs_;const auto& p=innerRowCount_;
    const auto virgin=[](const RowInputRead& r)noexcept {
        return !r.entered&&!r.prepared&&!r.beforeObserved&&!r.attempted&&!r.nativeReturned&&!r.afterObserved&&
            !r.resultAdmitted&&!r.copied&&!r.clampedAmount&&!r.pc&&!r.sourceItems&&r.bytes.empty()&&
            r.before.devices.empty()&&r.after.devices.empty();
    };
    if(!n.entered)return !n.prepared&&!n.dataIdentity&&!n.objectIdentity&&!n.rowsIdentity&&!n.rowIdentity&&
        !n.dataEpoch&&!n.count&&virgin(n.vertices)&&n.normals.empty();
    if(!n.rowIdentity||n.dataIdentity!=p.dataIdentity||n.objectIdentity!=p.objectIdentity||n.rowsIdentity!=p.rowsIdentity||
       n.dataEpoch!=p.dataEpoch||!p.read.resultAdmitted||!p.read.afterObserved||p.read.copied!=4u||
       n.count>16384u||sourceCloseAttempted_)return false;
    std::uint32_t actual=0;for(unsigned i=0;i<4u;++i)actual|=std::uint32_t(p.read.bytes[i])<<(8u*i);
    if(actual!=n.count)return false;
    if(!n.prepared)return n.normals.empty()&&virgin(n.vertices);
    if(n.normals.size()!=n.count)return false;
    const auto check=[&](const RowInputRead& r,const ServiceObservation& prior,std::size_t request,
                         std::int32_t items,std::uint32_t callPC)noexcept {
        if(!r.entered)return virgin(r);
        if(r.copied>request||r.copied>r.clampedAmount||r.clampedAmount>request||r.bytes.size()>request)return false;
        for(std::size_t i=r.copied;i<r.bytes.size();++i)if(r.bytes[i])return false;
        if(r.prepared){
            if(r.bytes.size()!=request||r.before.devices.size()!=std::size_t(deviceCount_)||
               r.after.devices.size()!=std::size_t(deviceCount_))return false;
        }else if(r.before.devices.size()>std::size_t(deviceCount_)||r.after.devices.size()>std::size_t(deviceCount_)||
                 r.beforeObserved||r.attempted)return false;
        if(r.beforeObserved&&(!r.prepared||r.before.memberCursor10!=prior.memberCursor10))return false;
        if(!r.attempted)return !r.nativeReturned&&!r.afterObserved&&!r.resultAdmitted&&!r.sourceItems&&
            !r.copied&&!r.clampedAmount&&r.pc==callPC;
        if(!r.prepared||!r.beforeObserved||r.before.memberCursor10>selectedLength_||
           selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max())||
           r.clampedAmount!=std::min<std::size_t>(request,selectedLength_-r.before.memberCursor10))return false;
        const bool admitted=r.nativeReturned&&r.afterObserved&&r.copied==r.clampedAmount&&
            r.after.memberCursor10==r.before.memberCursor10+r.copied;
        if(r.resultAdmitted!=admitted)return false;
        if(admitted)return r.pc==callPC+8u&&r.sourceItems==(r.copied?items:0);
        if(r.sourceItems||r.pc!=(r.nativeReturned?0x12fb30u:r.helperDiagnostic.sourcePC?r.helperDiagnostic.sourcePC:0x12fb30u))return false;
        //A failed block read can copy bytes and change device state without
        //advancing the member cursor. Do not discard either actual observation.
        return r.nativeReturned||!r.afterObserved||r.after.memberCursor10==r.before.memberCursor10;
    };
    if(!check(n.vertices,p.read.after,std::size_t(n.count)*32u,std::int32_t(n.count),0x2999e0u))return false;
    const RowInputRead* previous=&n.vertices;
    for(const auto& r:n.normals){
        if(r.entered&&(!previous->resultAdmitted||!previous->afterObserved))return false;
        if(!check(r,previous->after,12u,1,0x299a0cu))return false;
        previous=&r;
    }
    return true;
}

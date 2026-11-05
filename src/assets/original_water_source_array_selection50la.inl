//Post-formatter157D00 on the SAME Config, with an explicitly adapted PC.
//No old Wake selection/free receipt is reused by this occurrence.
bool OriginalWaterSourceConfig50LA::selectArraySectionNativeOnly(const char* query,std::size_t known,D& d)noexcept {
    d={};auto& n=nativeArraySelection_;const auto& w=nativeWakeLoad_;
    if(n.attempted||nativeClosed_||!borrowed_||!query||(known!=7u&&known!=17u)||
       !w.nativeLoaded||nativeWakeGetters_.completed!=5u||pc_!=0x30492cu||wakeSelection_.returned||
       !textReturned_||!openReturned_||!*openReturned_||source_.opened0C!=1u||source_.mode10||source_.binary08||
       !preflightNative(waterReceiver_))return fail(d,E::phase,0x157d00);
    using Phase=NativeArraySelection::Phase;
    n.attempted=true;n.phase=Phase::lookup;n.pc=0x157d00;n.frame=true; //2160-byte symbolic source frame.
    n.pc=0x157d04;n.saveMask|=32u;n.pc=0x157d08;n.saveMask|=16u;n.pc=0x157d0c;n.saveMask|=8u;
    n.pc=0x157d10;n.saveMask|=4u;n.pc=0x157d14;n.saveMask|=2u;n.pc=0x157d18;n.saveMask|=1u;
    n.pc=0x157d1c;n.pc=0x157d20;n.queryIdentity=query;
    //Private observation of already-known caller bytes, not a309230 copy or name28 mutation.
    for(std::size_t i=0;i<known;++i)n.query.put(i,static_cast<std::uint8_t>(query[i]));
    n.queryKnown=known;n.pc=0x157d24;n.pc=0x157d28;
    n.pc=0x157da0;n.pc=0x157da4;n.pc=0x157da8;n.pc=0x157dc0;n.pc=0x157dc4;
    std::int32_t index=-1;
    if(!lookupText157AF0(n.query,index,&n.pc))return false;
    n.pc=0x157dc8;n.index=index;n.pc=0x157dcc;n.pc=0x157dd0;n.pc=0x157dd4;
    if(index==-1){
        n.pc=0x157dd8;n.pc=0x157ddc;n.returned=false;
        n.pc=0x15821c;n.restoreMask|=32u;n.pc=0x158220;n.restoreMask|=16u;
        n.pc=0x158224;n.restoreMask|=8u;n.pc=0x158228;n.restoreMask|=4u;
        n.pc=0x15822c;n.restoreMask|=2u;n.pc=0x158230;n.restoreMask|=1u;
        n.pc=0x158234;n.pc=0x158238;n.stackReleased=true;n.phase=Phase::missingReturned;
        return true; //Only THIS adapted occurrence returned false; current Wake is untouched.
    }
    n.pc=0x157de0;n.pc=0x157de4;n.pc=0x157de8;
    n.pc=0x157f60;n.pc=0x157f64;n.pc=0x157f68;n.pc=0x157f6c;
    n.pc=0x157f70;n.pc=0x157f74;n.pc=0x157f78;n.pc=0x157f7c;n.pc=0x157f80;
    n.phase=Phase::position;n.pc=0x157f84;
    if(index<0||std::size_t(index)>=textTable_.rowExtent||!textTable_.rows[index].positionKnown)return false;
    n.position=textTable_.rows[index].position00;
    n.phase=Phase::seek;n.pc=0x157f88;n.seekAttempted=true;
    n.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    if(*n.position>std::numeric_limits<std::size_t>::max())return fail(d,E::unsupported,n.pc);
    n.pc=0x157f8c;
    const bool returned=resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,
        static_cast<std::size_t>(*n.position),d);
    n.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=n.cursor;
    if(!returned){n.pc=d.sourcePC?d.sourcePC:0x157f88u;return false;}
    n.seekReturned=true;n.phase=Phase::child;n.pc=0x157f90;
    n.childObserved=true;n.childIdentity=source_.child24;
    //This aperture has current successful Wake backing, including its count-zero table.
    if(n.childIdentity!=&wakeTableRole_||!w.tableAllocation.returned||!w.table)return false;
    n.pc=0x157f94; //Nonnull BEQL does NOT execute its157F98 delay instruction.
    n.pc=0x157f9c;n.pc=0x157fa0;n.phase=Phase::data;n.pc=0x157fa4;
    if(!w.dataAllocation.returned||!w.data||!w.offsets||!w.pointersWritten||w.offsets[0]!=0u||
       w.pointerZeroRole!=&wakeDataRole_)return false;
    n.dataIdentity=w.pointerZeroRole;n.dataObserved=true;
    n.pc=0x157fa8;n.pc=0x157fac;n.pc=0x157fb0;n.freeCallIssued=true;n.pc=0x157fb4;
    n.phase=Phase::freeReent;n.pc=0x305728;n.pc=0x30572c;n.pc=0x305730;n.pc=0x305734;
    using G=OriginalConfigProcessGlobals50LA;
    if(!G::reentrancyCurrentNative())return false;
    n.freeReent=G::state().reent38544C;
    if(*n.freeReent!=G::StdioIdentity::reent385160)return false;
    n.pc=0x304900;n.pc=0x304904;n.pc=0x304908;n.pc=0x30490c;n.pc=0x304910;n.pc=0x304914;n.pc=0x304918;
    n.nonnullFreeEntered=true;n.pc=0x30491c;n.pc=0x304920;n.pc=0x306538;n.pc=0x30653c;
    n.lockReturned=true; //Actual no-op JR/NOP, not authority over a native or EE heap.
    n.pc=0x304924;n.pc=0x304928;n.pc=0x30492c;n.heapHeaderBoundary=true;n.phase=Phase::heap;
    //Stop BEFORE header LW(payload-4). No free return, native retirement, source
    //count/child/name stores or new loading. Current Wake epochs3/4 stay owned.
    return fail(d,E::unsupported,n.pc);
}
bool OriginalWaterSourceConfig50LA::preflightArraySelectionNative()const noexcept {
    const auto& n=nativeArraySelection_;const auto& w=nativeWakeLoad_;
    using Phase=NativeArraySelection::Phase;using G=OriginalConfigProcessGlobals50LA;
    const auto noFree=[&]()noexcept {return !n.dataObserved&&!n.dataIdentity&&!n.freeCallIssued&&!n.freeReent&&
        !n.nonnullFreeEntered&&!n.lockReturned&&!n.heapHeaderBoundary;};
    const auto noSeek=[&]()noexcept {return !n.position&&!n.cursor&&!n.seekAttempted&&!n.seekReturned&&
        !n.childObserved&&!n.childIdentity&&noFree();};
    if(!n.attempted){
        if(n.phase!=Phase::unentered||n.pc||n.frame||n.stackReleased||n.saveMask||n.restoreMask||
           n.queryKnown||n.queryIdentity||n.index||n.returned||!noSeek())return false;
        for(const auto known:n.query.known)if(known)return false;return true;
    }
    if(!n.frame||n.saveMask!=63u||!n.queryIdentity||(n.queryKnown!=7u&&n.queryKnown!=17u)||
       !w.nativeLoaded||nativeWakeGetters_.completed!=5u||pc_!=0x30492cu||wakeSelection_.returned||
       !textReturned_||!openReturned_||!*openReturned_||source_.opened0C!=1u||source_.binary08||source_.mode10)return false;
    constexpr char signature[]="Signature Wave 1",pool[]="Pool 1";
    const auto* query=n.queryKnown==17u?signature:pool;
    for(std::size_t i=0;i<n.query.value.size();++i){
        if(n.query.known[i]!=(i<n.queryKnown))return false;
        if(n.query.known[i]&&n.query.value[i]!=static_cast<std::uint8_t>(query[i]))return false;
    }
    if(!n.index)return n.phase==Phase::lookup&&!n.returned&&!n.stackReleased&&!n.restoreMask&&noSeek();
    if(*n.index==-1)return n.phase==Phase::missingReturned&&n.returned==false&&n.pc==0x158238u&&
        n.stackReleased&&n.restoreMask==63u&&noSeek();
    if(*n.index<0||std::size_t(*n.index)>=textTable_.rowExtent||n.returned||n.stackReleased||n.restoreMask)return false;
    const auto& row=textTable_.rows[*n.index];
    if(!n.position)return n.phase==Phase::position&&n.pc==0x157f84u&&noSeek();
    if(!row.positionKnown||*n.position!=row.position00||!n.seekAttempted||!n.cursor)return false;
    if(!n.seekReturned)return n.phase==Phase::seek&&!n.childObserved&&!n.childIdentity&&noFree();
    if(!n.childObserved||n.childIdentity!=&wakeTableRole_||source_.child24!=currentSectionChildNative()||
       !w.tableAllocation.returned||bool(w.table)==nativeArrayRetirement_.tableRetired)return false;
    if(!n.dataObserved)return (n.phase==Phase::child||n.phase==Phase::data)&&noFree()&&
        n.pc==(n.phase==Phase::child?0x157f90u:0x157fa4u);
    if(n.dataIdentity!=&wakeDataRole_||w.pointerZeroRole!=n.dataIdentity||!w.dataAllocation.returned||
       bool(w.data)==nativeArrayRetirement_.dataRetired||!w.offsets||!w.pointersWritten||w.offsets[0]||!n.freeCallIssued)return false;
    if(!n.freeReent||*n.freeReent!=G::StdioIdentity::reent385160)return n.phase==Phase::freeReent&&
        n.pc==0x305734u&&!n.nonnullFreeEntered&&!n.lockReturned&&!n.heapHeaderBoundary;
    return n.phase==Phase::heap&&n.nonnullFreeEntered&&n.lockReturned&&n.heapHeaderBoundary&&n.pc==0x30492cu;
}

//SECOND157D00 occurrence. Current General Information custody stays intact
//until authentic source retirement can be represented; native free is not it.
bool OriginalWaterSourceConfig50LA::selectWakeFroth157D00(D& d)noexcept {
    d={};auto& w=wakeSelection_;
    if(w.attempted||nativeClosed_||!borrowed_||!textReturned_||!openReturned_||!*openReturned_||
       !sectionReturn_||!integerGetters_[11].returned||source_.opened0C!=1u||source_.mode10||source_.binary08||
       !preflightNative(waterReceiver_))return fail(d,E::phase,0x157d00);
    w.attempted=true;pc_=0x157d1c;pc_=0x157da0;
    constexpr char name[]="Wake Froth"; //Actual caller literal3C0A88.
    for(std::size_t i=0;i<sizeof(name);++i)w.query.put(i,static_cast<std::uint8_t>(name[i]));
    w.queryReady=true;pc_=0x157dc0;std::int32_t index=-1;
    if(!lookupText157AF0(w.query,index))return false;
    w.index=index;
    if(index==-1){
        //No seek, free, count/child/name write or new selection is implied.
        pc_=0x157dd8;pc_=0x157ddc;w.returned=false;pc_=0x158238;return true;
    }
    pc_=0x157f84;
    if(index<0||std::size_t(index)>=textTable_.rowExtent||!textTable_.rows[index].positionKnown)return false;
    w.position=textTable_.rows[index].position00;
    pc_=0x157f88;w.seekAttempted=true;
    w.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    if(*w.position>std::numeric_limits<std::size_t>::max())return fail(d,E::unsupported,pc_);
    const bool returned=resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,
        static_cast<std::size_t>(*w.position),d);
    w.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=w.cursor;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    w.seekReturned=true;
    pc_=0x157f90;w.childObserved=true;w.childIdentity=source_.child24;
    if(!w.childIdentity){
        //157F94 branch-likely skips BOTH frees AND count/child clearing.
        //First General Information was missing. New loading is not joined yet.
        pc_=0x157f94;pc_=0x157f98;pc_=0x157fcc;pc_=0x157fe8;
        w.nullChildBoundary=true;return false; //Before row.line08/new section body.
    }
    pc_=0x157f9c;pc_=0x157fa0;pc_=0x157fa4;
    if(w.childIdentity!=&sectionTableRole_||!sectionTable_||!sectionTableAllocation_.returned||
       !sectionDataAllocation_.returned||!sectionData_||!sectionOffsets_||
       !sectionPointersWritten_||sectionOffsets_[0]!=0u)return false;
    //Semantic child[0] is the SAME live data allocation, even at count0.
    //This retains opaque allocation-role identity, never casts host to EE bits.
    w.dataIdentity=&sectionDataRole_;w.dataObserved=true;
    pc_=0x157fa8;pc_=0x157fac;pc_=0x157fb0;w.freeCallIssued=true;pc_=0x157fb4;
    pc_=0x305728;pc_=0x30572c;pc_=0x305730;pc_=0x305734;
    using G=OriginalConfigProcessGlobals50LA;
    if(!G::reentrancyCurrentNative())return false;
    w.freeReent=G::state().reent38544C;
    if(*w.freeReent!=G::StdioIdentity::reent385160)return false; //Native supported-reent boundary.
    pc_=0x304900;pc_=0x30490c;pc_=0x304910;pc_=0x304914;pc_=0x304918;
    w.nonnullFreeEntered=true; //Nonnull role is established by the prior success-only allocation.
    pc_=0x30491c;pc_=0x304920;pc_=0x306538;pc_=0x30653c;w.lockReturned=true; //Actual JR/NOP only.
    pc_=0x304924;pc_=0x304928;pc_=0x30492c;w.heapHeaderBoundary=true;
    //First unjoined LW reads original payload-4 chunk metadata. Native backing
    //has no EE header/free-list/current heap authority. Do NOT std::free, clear
    //count/child, claim either free returned, or fabricate a Wake selection.
    return fail(d,E::unsupported,pc_);
}
bool OriginalWaterSourceConfig50LA::preflightWakeSelectionNative()const noexcept {
    const auto& w=wakeSelection_;
    const auto noFree=[&]()noexcept {
        return !w.dataObserved&&!w.dataIdentity&&!w.freeCallIssued&&!w.freeReent&&!w.nonnullFreeEntered&&
            !w.lockReturned&&!w.heapHeaderBoundary;
    };
    if(!w.attempted){
        if(w.queryReady||w.index||w.position||w.cursor||w.seekAttempted||w.seekReturned||w.returned||
           w.childObserved||w.childIdentity||w.nullChildBoundary||!noFree())return false;
        for(const auto known:w.query.known)if(known)return false;
        return true;
    }
    if(!w.queryReady||!sectionReturn_||!integerGetters_[11].returned||!textReturned_||
       !openReturned_||!*openReturned_||source_.opened0C!=1u||source_.binary08||source_.mode10)return false;
    constexpr char name[]="Wake Froth";
    for(std::size_t i=0;i<w.query.value.size();++i){
        if(w.query.known[i]!=(i<sizeof(name)))return false;
        if(w.query.known[i]&&w.query.value[i]!=static_cast<std::uint8_t>(name[i]))return false;
    }
    if(w.returned&&(*w.returned||w.index!=-1))return false;
    if(!w.index||*w.index==-1){
        if(w.position||w.cursor||w.seekAttempted||w.seekReturned||w.childObserved||w.childIdentity||
           w.nullChildBoundary||!noFree())return false;
        return w.index?w.returned==false:!w.returned;
    }
    if(*w.index<0||std::size_t(*w.index)>=textTable_.rowExtent||w.returned)return false;
    const auto& row=textTable_.rows[*w.index];
    if(w.position&&(!row.positionKnown||w.position!=row.position00))return false;
    if(!w.position)return !w.seekAttempted&&!w.cursor&&!w.seekReturned&&!w.childObserved&&!w.childIdentity&&
        !w.nullChildBoundary&&noFree()&&pc_==0x157f84u;
    if(!w.seekAttempted||!w.cursor)return false;
    if(w.seekReturned&&!w.seekAttempted)return false;
    if(!w.seekReturned)return !w.childObserved&&!w.childIdentity&&!w.nullChildBoundary&&noFree();
    if(!w.childObserved||w.childIdentity!=(sectionTableAllocation_.returned?&sectionTableRole_:nullptr)||
       source_.child24!=currentSectionChildNative())return false;
    if(!w.childIdentity)return w.nullChildBoundary&&sectionReturn_==false&&noFree()&&pc_==0x157fe8u;
    if(w.nullChildBoundary)return false;
    if(!w.dataObserved)return noFree()&&pc_==0x157fa4u;
    if(!w.dataIdentity||w.dataIdentity!=&sectionDataRole_||!sectionDataAllocation_.returned||
       !sectionTableAllocation_.returned||!sectionOffsets_||!sectionPointersWritten_||sectionOffsets_[0]||
       !w.freeCallIssued)return false;
    if(!w.freeReent||*w.freeReent!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160)
        return !w.nonnullFreeEntered&&!w.lockReturned&&!w.heapHeaderBoundary&&pc_==0x305734u;
    return w.nonnullFreeEntered&&w.lockReturned&&w.heapHeaderBoundary&&pc_==0x30492cu;
}

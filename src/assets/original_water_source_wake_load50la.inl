//Body157FE8..158238 on an EXPLICIT native-adapted path only. Original pc_
//remains30492C; no source free return or original selection return is issued.
bool OriginalWaterSourceConfig50LA::allocateWakeBackingNative(bool table)noexcept {
    auto& n=nativeWakeLoad_;auto& a=table?n.tableAllocation:n.dataAllocation;
    if(a.attempted)return false;
    a.attempted=true;a.requested=n.counted<<(table?2u:10u);
    a.nativeBytes=std::max<std::size_t>(8u,a.requested);
    if(table&&n.counted>0xffffffffu/1024u){n.sizeBounded=true;n.pc=0x158144;return false;}
    using G=OriginalConfigProcessGlobals50LA;
    n.pc=0x30570c;
    if(!G::reentrancyCurrentNative()){a.stop=NativeAllocationStop::reentUnknown;return false;}
    a.reent=G::state().reent38544C;
    if(*a.reent!=G::StdioIdentity::reent385160){a.stop=NativeAllocationStop::reentUnknown;return false;}
    n.pc=0x305990;
    if(table){
        std::unique_ptr<std::uint32_t[]> offsets;std::unique_ptr<Text::Bytes[]> rows;
        try{
            offsets.reset(new std::uint32_t[std::max<std::uint32_t>(1u,n.counted)]);
            if(n.counted)rows.reset(new Text::Bytes[n.counted]);
        }catch(const std::bad_alloc&){a.stop=NativeAllocationStop::sidecarRefused;return false;}
        n.offsets=std::move(offsets);n.rows=std::move(rows);n.capacity=n.counted;
    }
    void* const storage=std::malloc(a.nativeBytes);
    if(!storage){a.stop=NativeAllocationStop::backingRefused;return false;}
    //Success-only native backing, NOT an original malloc/address/errno result.
    if(table){n.table.reset(storage);source_.child24=&wakeTableRole_;n.pc=0x15814c;}
    else{n.data.reset(storage);n.pc=0x15815c;}
    a.returned=true;return true;
}
bool OriginalWaterSourceConfig50LA::loadWakeFrothNativeOnly(D& d)noexcept {
    d={};auto& n=nativeWakeLoad_;const auto& w=wakeSelection_;
    if(n.attempted||nativeClosed_||!borrowed_||thread_!=std::this_thread::get_id()||pc_!=0x30492cu||
       !nativeWakeRetirement_.tableRetired||nativeWakeRetirement_.adaptedResumePC!=0x157fe8u||
       !w.index||*w.index<0||std::size_t(*w.index)>=textTable_.rowExtent||!w.position||
       !w.seekReturned||!w.heapHeaderBoundary||w.returned||source_.selected20||source_.child24||
       !preflightNative(waterReceiver_))return false;
    n.attempted=true;n.phase=WakeLoadPhase::span;n.pc=0x157fe8;
    const auto& row=textTable_.rows[*w.index];
    if(!row.lineKnown)return false;n.span=row.line08;
    n.phase=WakeLoadPhase::copyName;
    constexpr char name[]="Wake Froth";
    n.pc=0x15808c;for(std::size_t i=0;i<sizeof(name)-1u;++i)source_.name28[i]=name[i];
    n.pc=0x158098;source_.name28[sizeof(name)-1u]=0;n.nameCopied=true;
    //Only indices0..10 change. General Information's known11..19 tail remains.
    const auto span=(*n.span&0x80000000u)?0u:*n.span;
    while(n.scanned[0]<span){
        n.phase=WakeLoadPhase::countRead;n.pc=0x1580b4;++n.readCalls[0];
        const bool returned=readTextLine12EE90(n.input,n.read,d,&n.pc);
        n.cursor=n.read.after;if(!returned)return false;
        n.phase=WakeLoadPhase::countSplit;n.pc=0x1580c4;
        const auto split=Text::split159440(n.input,n.line);n.pc=split.pc;
        if(split.stop!=Text::Stop::complete)return false;
        const auto first=n.line.value[0];
        if(first&&first!=';'){n.pc=0x1580ec;++source_.selected20;++n.counted;}
        ++n.scanned[0];
    }
    //Original reloads this SAME text row position for its second seek.
    n.phase=WakeLoadPhase::secondSeek;n.pc=0x15812c;
    if(!row.positionKnown)return false;n.seekPosition=row.position00;
    n.pc=0x158130;n.seekAttempted=true;
    n.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    if(*n.seekPosition>std::numeric_limits<std::size_t>::max())return fail(d,E::unsupported,n.pc);
    const bool sought=resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,
        static_cast<std::size_t>(*n.seekPosition),d);
    n.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=n.cursor;
    if(!sought){n.pc=d.sourcePC?d.sourcePC:n.pc;return false;}n.seekReturned=true;
    n.phase=WakeLoadPhase::tableAllocation;n.pc=0x158144;
    if(!allocateWakeBackingNative(true))return false;
    n.phase=WakeLoadPhase::dataAllocation;n.pc=0x158154;
    if(!allocateWakeBackingNative(false))return false;
    n.phase=WakeLoadPhase::pointers;n.pc=0x158168;
    //Semantic pointer[0] is epoch4's data, even for count0. Raw EE pointer bits
    //are unknown. Positive slots retain original1024-byte relative offsets.
    n.offsets[0]=0;n.pointerZeroRole=&wakeDataRole_;n.pointersWritten=1;
    for(std::uint32_t i=1;i<n.counted;++i){n.offsets[i]=n.offsets[i-1]+1024u;++n.pointersWritten;}
    while(n.scanned[1]<span){
        n.phase=WakeLoadPhase::storeRead;n.pc=0x1581b4;++n.readCalls[1];
        const bool returned=readTextLine12EE90(n.input,n.read,d,&n.pc);
        n.cursor=n.read.after;if(!returned)return false;
        n.phase=WakeLoadPhase::storeSplit;n.pc=0x1581c4;
        const auto split=Text::split159440(n.input,n.line);n.pc=split.pc;
        if(split.stop!=Text::Stop::complete)return false;
        const auto first=n.line.value[0];
        if(first&&first!=';'){
            n.phase=WakeLoadPhase::copyLine;n.pc=0x1581f0;
            if(n.stored>=n.capacity||n.stored>=n.pointersWritten||!n.rows)return false;
            n.pc=0x1581f4;std::size_t length=0;
            for(;length<n.line.value.size();++length){
                if(!n.line.known[length])return false;if(!n.line.value[length])break;
            }
            if(length==n.line.value.size())return false;
            auto& stored=n.rows[n.stored];
            auto* raw=static_cast<std::uint8_t*>(n.data.get())+n.offsets[n.stored];
            //Bounded byte-result copy, not EE alignment/word-overread evidence.
            for(std::size_t i=0;i<=length;++i){stored.put(i,n.line.value[i]);raw[i]=n.line.value[i];}
            ++n.stored;
        }
        ++n.scanned[1];
    }
    n.pc=0x158214;source_.field428=0;n.pc=0x158218;
    n.nativeLoaded=true;n.phase=WakeLoadPhase::complete;n.pc=0x158238;
    return true; //Native body complete ONLY; wakeSelection_.returned stays absent.
}
bool OriginalWaterSourceConfig50LA::preflightWakeLoadNative()const noexcept {
    const auto& n=nativeWakeLoad_;const auto& first=n.tableAllocation;const auto& second=n.dataAllocation;
    if(wakeTableRole_.owner!=this||wakeTableRole_.epoch!=3u||
       wakeDataRole_.owner!=this||wakeDataRole_.epoch!=4u)return false;
    if(!n.attempted){
        if(n.phase!=WakeLoadPhase::unentered||n.pc!=0x157fe8u||n.nameCopied||n.seekAttempted||n.seekReturned||
           n.sizeBounded||n.nativeLoaded||n.span||n.seekPosition||n.cursor||n.readCalls[0]||n.readCalls[1]||
           n.scanned[0]||n.scanned[1]||n.counted||n.stored||n.pointersWritten||n.capacity||n.pointerZeroRole||n.read.attempted||
           n.table||n.data||n.offsets||n.rows)return false;
        for(const auto* a:{&first,&second})if(a->attempted||a->returned||a->requested||a->nativeBytes||a->reent||
            a->stop!=NativeAllocationStop::none)return false;
        for(const auto known:n.input.known)if(known)return false;
        for(const auto known:n.line.known)if(known)return false;
        return true;
    }
    const auto& w=wakeSelection_;
    if(pc_!=0x30492cu||!nativeWakeRetirement_.tableRetired||nativeWakeRetirement_.adaptedResumePC!=0x157fe8u||
       !w.heapHeaderBoundary||w.returned||!w.index||*w.index<0||std::size_t(*w.index)>=textTable_.rowExtent||
       !w.position||!w.seekReturned||n.phase==WakeLoadPhase::unentered||
       n.nativeLoaded!=(n.phase==WakeLoadPhase::complete)||source_.selected20!=currentSectionCountNative()||
       source_.child24!=currentSectionChildNative()||n.counted>n.scanned[0]||
       n.stored>n.scanned[1]||n.stored>n.counted||n.seekReturned>n.seekAttempted)return false;
    const auto& row=textTable_.rows[*w.index];
    if(n.span&&(!row.lineKnown||*n.span!=row.line08))return false;
    if(!n.span){
        for(const auto* a:{&first,&second})if(a->attempted||a->returned||a->requested||a->nativeBytes||a->reent||
            a->stop!=NativeAllocationStop::none)return false;
        return n.phase==WakeLoadPhase::span&&n.pc==0x157fe8u&&!n.nameCopied&&!n.seekAttempted&&!n.seekPosition&&
            !n.cursor&&!n.readCalls[0]&&!n.readCalls[1]&&!n.scanned[0]&&!n.scanned[1]&&!n.counted&&!n.stored&&
            !n.pointersWritten&&!n.capacity&&!n.pointerZeroRole&&!n.read.attempted&&!n.sizeBounded&&
            !n.table&&!n.data&&!n.offsets&&!n.rows;
    }
    if(!n.nameCopied)return false; //Constant ten-byte copy has no native failure point.
    constexpr char name[]="Wake Froth";constexpr char oldName[]="General Information";
    if(!nativeArrayLoad_.nameCopied)
        for(std::size_t i=0;i<sizeof(oldName);++i)
            if(source_.name28[i]!=(i<sizeof(name)?name[i]:oldName[i]))return false;
    const auto span=(*n.span&0x80000000u)?0u:*n.span;
    for(std::size_t pass=0;pass<2;++pass)
        if(n.scanned[pass]>span||n.readCalls[pass]<n.scanned[pass]||n.readCalls[pass]-n.scanned[pass]>1u)return false;
    if(n.seekPosition&&(!row.positionKnown||*n.seekPosition!=row.position00||n.scanned[0]!=span))return false;
    if(n.seekAttempted&&(!n.seekPosition||!n.cursor))return false;
    if(n.readCalls[1]&&!n.seekReturned)return false;
    if(n.readCalls[0]||n.readCalls[1]){
        const auto& read=n.read;
        if(!read.attempted||!read.before||!read.after||read.requested>1022u||read.copied>read.requested||
           (read.present&&(!read.returned||!read.transportReturned||!read.copied))||
           (read.returned&&read.present&&read.copied!=read.requested)||
           (read.returned&&!read.present&&(read.requested||read.copied||read.transportReturned||read.after!=read.before))||
           *read.after<*read.before||*read.after-*read.before>read.requested)return false;
        if(!read.returned&&n.phase!=WakeLoadPhase::countRead&&n.phase!=WakeLoadPhase::storeRead)return false;
        if((n.phase==WakeLoadPhase::countRead||n.phase==WakeLoadPhase::countSplit||
            n.phase==WakeLoadPhase::storeRead||n.phase==WakeLoadPhase::storeSplit||n.phase==WakeLoadPhase::copyLine)&&
           n.cursor!=read.after)return false;
    }else if(n.read.attempted)return false;
    //Completed allocations remain historical after explicit descendant native retirement.
    if(bool(n.table)!=(first.returned&&!nativeArrayRetirement_.tableRetired)||
       bool(n.data)!=(second.returned&&!nativeArrayRetirement_.dataRetired)||
       (second.attempted&&!first.returned)||(first.attempted&&!n.seekReturned)||
       (first.returned&&(!n.offsets||n.capacity!=n.counted||bool(n.rows)!=bool(n.counted))))return false;
    for(const auto* a:{&first,&second}){
        const bool table=a==&first;
        if(!a->attempted){if(a->returned||a->requested||a->nativeBytes||a->reent||a->stop!=NativeAllocationStop::none)return false;continue;}
        if(a->requested!=(n.counted<<(table?2u:10u))||a->nativeBytes!=std::max<std::size_t>(8u,a->requested))return false;
        if(a->returned&&(!a->reent||*a->reent!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160||
           a->stop!=NativeAllocationStop::none))return false;
        if(!a->returned&&!n.sizeBounded&&a->stop==NativeAllocationStop::none)return false;
        if(a->reent&&*a->reent!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160&&
           a->stop!=NativeAllocationStop::reentUnknown)return false;
    }
    if(n.sizeBounded&&(!first.attempted||first.returned||second.attempted||n.counted<=0xffffffffu/1024u||
       n.phase!=WakeLoadPhase::tableAllocation||n.pc!=0x158144u))return false;
    if(n.offsets){if(n.capacity!=n.counted||bool(n.rows)!=bool(n.capacity))return false;}
    else if(n.rows||n.capacity||n.pointersWritten)return false;
    if(second.returned){
        if(n.pointerZeroRole!=&wakeDataRole_||n.pointersWritten!=std::max<std::uint32_t>(1u,n.counted))return false;
        for(std::uint32_t i=0;i<n.pointersWritten;++i)if(n.offsets[i]!=i*1024u)return false;
    }else if(n.pointerZeroRole||n.pointersWritten||n.stored)return false;
    for(std::uint32_t i=0;i<n.stored;++i){
        const auto& stored=n.rows[i];bool terminated=false;
        for(std::size_t j=0;j<stored.value.size();++j){
            if(!stored.known[j])return false;if(!stored.value[j]){terminated=true;break;}
        }
        if(!terminated)return false;
    }
    if(n.nativeLoaded&&(!second.returned||!n.seekReturned||n.scanned[0]!=span||n.scanned[1]!=span||
       n.readCalls!=n.scanned||n.stored!=n.counted||n.pc!=0x158238u))return false;
    return true;
}

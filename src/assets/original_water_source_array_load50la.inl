//Body157FE8..158238 on an EXPLICIT native-adapted path only. Original pc_
//remains30492C; no source free return or original selection return is issued.
//Only the exact first formatter outputs are admitted. This projects a length
//value across every alignment branch, never reads unknown native tail bytes or
//claims concrete EE wide-load values/alignment. See the pinned bounded proof.
bool OriginalWaterSourceConfig50LA::projectArrayNameLengthNative()noexcept {
    auto& n=nativeArrayLoad_;const auto& w=nativeArraySelection_;n.pc=0x3093a8;
    if(n.nameLengthProjected||!w.queryIdentity||(w.queryKnown!=7u&&w.queryKnown!=17u))return false;
    constexpr char signature[]="Signature Wave 1",pool[]="Pool 1";
    const auto* expected=w.queryKnown==17u?signature:pool;
    for(std::size_t i=0;i<w.queryKnown;++i)
        if(!w.query.known[i]||w.query.value[i]!=static_cast<std::uint8_t>(expected[i]))return false;
    //Known nonzero ASCII precedes a known NUL. Byte scan returns6/16. LD8's
    //borrow cannot precede that NUL; PSUBB's per-lane marker is likewise forced.
    //Unknown later bytes cannot clear the marker, and final byte scan stops at
    //this first NUL. Maximum wide reads stay inside loader frame+710..730.
    n.nameLength=static_cast<std::uint32_t>(w.queryKnown-1u);
    n.strlenReadWindowMax=w.queryKnown==17u?32u:16u;n.nameLengthProjected=true;
    n.pc=0x3094dc;return true;
}
bool OriginalWaterSourceConfig50LA::allocateArraySectionBackingNative(bool table)noexcept {
    auto& n=nativeArrayLoad_;auto& a=table?n.tableAllocation:n.dataAllocation;
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
    if(table){n.table.reset(storage);source_.child24=&arrayTableRole_;n.pc=0x15814c;}
    else{n.data.reset(storage);n.pc=0x15815c;}
    a.returned=true;return true;
}
bool OriginalWaterSourceConfig50LA::loadArraySectionNativeOnly(D& d)noexcept {
    d={};auto& n=nativeArrayLoad_;const auto& w=nativeArraySelection_;
    if(n.attempted||nativeClosed_||!borrowed_||thread_!=std::this_thread::get_id()||pc_!=0x30492cu||
       !nativeArrayRetirement_.tableRetired||nativeArrayRetirement_.adaptedResumePC!=0x157fe8u||
       !w.index||*w.index<0||std::size_t(*w.index)>=textTable_.rowExtent||!w.position||
       !w.seekReturned||!w.heapHeaderBoundary||w.returned||source_.selected20||source_.child24||
       !preflightNative(waterReceiver_))return false;
    n.attempted=true;n.phase=ArrayLoadPhase::span;n.pc=0x157fe8;
    const auto& row=textTable_.rows[*w.index];
    if(!row.lineKnown)return false;n.span=row.line08;
    n.phase=ArrayLoadPhase::nameLength;n.pc=0x158064;
    if(!projectArrayNameLengthNative())return false;
    n.pc=0x15806c;n.pc=0x158070;n.pc=0x158074;n.pc=0x158078;
    //Length6/16 is below the original signed1023 cap, so15807C is skipped.
    n.pc=0x158080;n.phase=ArrayLoadPhase::copyName;n.pc=0x158084;n.pc=0x158088;
    n.pc=0x15808c;n.strncpyReadQuantumMax=*n.nameLength==16u?8u:1u;
    //caller+710 minus Config+28(caller+268) is4A8, so both addresses cannot
    //be16-aligned. Byte/LD8 paths read only these known6/16 source bytes.
    n.pc=0x309980;
    for(std::size_t i=0;i<*n.nameLength;++i)source_.name28[i]=static_cast<char>(w.query.value[i]);
    n.pc=0x309b38;n.pc=0x158094;n.pc=0x158098;
    source_.name28[*n.nameLength]=0;n.nameCopied=true;
    //Known old Wake/General tail survives; unknown bytes20+ are never touched.
    const auto span=(*n.span&0x80000000u)?0u:*n.span;
    while(n.scanned[0]<span){
        n.phase=ArrayLoadPhase::countRead;n.pc=0x1580b4;++n.readCalls[0];
        const bool returned=readTextLine12EE90(n.input,n.read,d,&n.pc);
        n.cursor=n.read.after;if(!returned)return false;
        n.phase=ArrayLoadPhase::countSplit;n.pc=0x1580c4;
        const auto split=Text::split159440(n.input,n.line);n.pc=split.pc;
        if(split.stop!=Text::Stop::complete)return false;
        const auto first=n.line.value[0];
        if(first&&first!=';'){n.pc=0x1580ec;++source_.selected20;++n.counted;}
        ++n.scanned[0];
    }
    //Original reloads this SAME text row position for its second seek.
    n.phase=ArrayLoadPhase::secondSeek;n.pc=0x15812c;
    if(!row.positionKnown)return false;n.seekPosition=row.position00;
    n.pc=0x158130;n.seekAttempted=true;
    n.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    if(*n.seekPosition>std::numeric_limits<std::size_t>::max())return fail(d,E::unsupported,n.pc);
    const bool sought=resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,
        static_cast<std::size_t>(*n.seekPosition),d);
    n.cursor=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=n.cursor;
    if(!sought){n.pc=d.sourcePC?d.sourcePC:n.pc;return false;}n.seekReturned=true;
    n.phase=ArrayLoadPhase::tableAllocation;n.pc=0x158144;
    if(!allocateArraySectionBackingNative(true))return false;
    n.phase=ArrayLoadPhase::dataAllocation;n.pc=0x158154;
    if(!allocateArraySectionBackingNative(false))return false;
    n.phase=ArrayLoadPhase::pointers;n.pc=0x158168;
    //Semantic pointer[0] is epoch6's data, even for count0. Raw EE pointer bits
    //are unknown. Positive slots retain original1024-byte relative offsets.
    n.offsets[0]=0;n.pointerZeroRole=&arrayDataRole_;n.pointersWritten=1;
    for(std::uint32_t i=1;i<n.counted;++i){n.offsets[i]=n.offsets[i-1]+1024u;++n.pointersWritten;}
    while(n.scanned[1]<span){
        n.phase=ArrayLoadPhase::storeRead;n.pc=0x1581b4;++n.readCalls[1];
        const bool returned=readTextLine12EE90(n.input,n.read,d,&n.pc);
        n.cursor=n.read.after;if(!returned)return false;
        n.phase=ArrayLoadPhase::storeSplit;n.pc=0x1581c4;
        const auto split=Text::split159440(n.input,n.line);n.pc=split.pc;
        if(split.stop!=Text::Stop::complete)return false;
        const auto first=n.line.value[0];
        if(first&&first!=';'){
            n.phase=ArrayLoadPhase::copyLine;n.pc=0x1581f0;
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
    n.pc=0x15821c;n.selectionRestoreMask|=32u;n.pc=0x158220;n.selectionRestoreMask|=16u;
    n.pc=0x158224;n.selectionRestoreMask|=8u;n.pc=0x158228;n.selectionRestoreMask|=4u;
    n.pc=0x15822c;n.selectionRestoreMask|=2u;n.pc=0x158230;n.selectionRestoreMask|=1u;
    n.pc=0x158234;n.pc=0x158238;n.selectionStackReleased=true;
    n.nativeLoaded=true;n.phase=ArrayLoadPhase::complete;
    return true; //Adapted body/value1 ONLY; old selection stop/returned stay unchanged.
}
bool OriginalWaterSourceConfig50LA::preflightArrayLoadNative()const noexcept {
    const auto& n=nativeArrayLoad_;const auto& first=n.tableAllocation;const auto& second=n.dataAllocation;
    if(arrayTableRole_.owner!=this||arrayTableRole_.epoch!=5u||
       arrayDataRole_.owner!=this||arrayDataRole_.epoch!=6u)return false;
    if(!n.attempted){
        if(n.phase!=ArrayLoadPhase::unentered||n.pc!=0x157fe8u||n.nameCopied||n.seekAttempted||n.seekReturned||
           n.sizeBounded||n.nativeLoaded||n.nameLengthProjected||n.nameLength||n.strlenReadWindowMax||
           n.strncpyReadQuantumMax||n.selectionRestoreMask||n.selectionStackReleased||n.span||n.seekPosition||n.cursor||n.readCalls[0]||n.readCalls[1]||
           n.scanned[0]||n.scanned[1]||n.counted||n.stored||n.pointersWritten||n.capacity||n.pointerZeroRole||n.read.attempted||
           n.table||n.data||n.offsets||n.rows)return false;
        for(const auto* a:{&first,&second})if(a->attempted||a->returned||a->requested||a->nativeBytes||a->reent||
            a->stop!=NativeAllocationStop::none)return false;
        for(const auto known:n.input.known)if(known)return false;
        for(const auto known:n.line.known)if(known)return false;
        return true;
    }
    const auto& w=nativeArraySelection_;
    if(pc_!=0x30492cu||!nativeArrayRetirement_.tableRetired||nativeArrayRetirement_.adaptedResumePC!=0x157fe8u||
       !w.heapHeaderBoundary||!w.frame||w.saveMask!=63u||w.restoreMask||w.stackReleased||w.returned||!w.index||*w.index<0||std::size_t(*w.index)>=textTable_.rowExtent||
       !w.position||!w.seekReturned||n.phase==ArrayLoadPhase::unentered||
       n.nativeLoaded!=(n.phase==ArrayLoadPhase::complete)||source_.selected20!=currentSectionCountNative()||
       source_.child24!=currentSectionChildNative()||n.counted>n.scanned[0]||
       n.stored>n.scanned[1]||n.stored>n.counted||n.seekReturned>n.seekAttempted)return false;
    const auto& row=textTable_.rows[*w.index];
    if(n.span&&(!row.lineKnown||*n.span!=row.line08))return false;
    if(!n.span){
        for(const auto* a:{&first,&second})if(a->attempted||a->returned||a->requested||a->nativeBytes||a->reent||
            a->stop!=NativeAllocationStop::none)return false;
        return n.phase==ArrayLoadPhase::span&&n.pc==0x157fe8u&&!n.nameLengthProjected&&!n.nameLength&&
            !n.strlenReadWindowMax&&!n.strncpyReadQuantumMax&&!n.selectionRestoreMask&&!n.selectionStackReleased&&
            !n.nameCopied&&!n.seekAttempted&&!n.seekPosition&&
            !n.cursor&&!n.readCalls[0]&&!n.readCalls[1]&&!n.scanned[0]&&!n.scanned[1]&&!n.counted&&!n.stored&&
            !n.pointersWritten&&!n.capacity&&!n.pointerZeroRole&&!n.read.attempted&&!n.sizeBounded&&
            !n.table&&!n.data&&!n.offsets&&!n.rows;
    }
    if(!n.nameLengthProjected){
        for(const auto* a:{&first,&second})if(a->attempted||a->returned||a->requested||a->nativeBytes||a->reent||
            a->stop!=NativeAllocationStop::none)return false;
        return n.phase==ArrayLoadPhase::nameLength&&n.pc==0x3093a8u&&!n.nameLength&&!n.strlenReadWindowMax&&
            !n.strncpyReadQuantumMax&&!n.nameCopied&&!n.seekAttempted&&!n.seekPosition&&!n.cursor&&
            !n.readCalls[0]&&!n.readCalls[1]&&!n.scanned[0]&&!n.scanned[1]&&!n.counted&&!n.stored&&
            !n.pointersWritten&&!n.capacity&&!n.pointerZeroRole&&!n.read.attempted&&!n.sizeBounded&&
            !n.table&&!n.data&&!n.offsets&&!n.rows&&!n.selectionRestoreMask&&!n.selectionStackReleased;
    }
    if((w.queryKnown!=7u&&w.queryKnown!=17u)||n.nameLength!=w.queryKnown-1u||
       n.strlenReadWindowMax!=(w.queryKnown==17u?32u:16u)||
       n.strncpyReadQuantumMax!=(w.queryKnown==17u?8u:1u)||!n.nameCopied)return false;
    constexpr char wakeName[]="Wake Froth";constexpr char oldName[]="General Information";
    for(std::size_t i=0;i<sizeof(oldName);++i){
        if(i<w.queryKnown&&!w.query.known[i])return false;
        const auto expected=i<w.queryKnown?static_cast<char>(w.query.value[i]):
            i<sizeof(wakeName)?wakeName[i]:oldName[i];
        if(source_.name28[i]!=expected)return false;
    }
    if(n.selectionStackReleased!=n.nativeLoaded||n.selectionRestoreMask!=(n.nativeLoaded?63u:0u))return false;
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
        if(!read.returned&&n.phase!=ArrayLoadPhase::countRead&&n.phase!=ArrayLoadPhase::storeRead)return false;
        if((n.phase==ArrayLoadPhase::countRead||n.phase==ArrayLoadPhase::countSplit||
            n.phase==ArrayLoadPhase::storeRead||n.phase==ArrayLoadPhase::storeSplit||n.phase==ArrayLoadPhase::copyLine)&&
           n.cursor!=read.after)return false;
    }else if(n.read.attempted)return false;
    if(bool(n.table)!=first.returned||bool(n.data)!=second.returned||
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
       n.phase!=ArrayLoadPhase::tableAllocation||n.pc!=0x158144u))return false;
    if(n.offsets){if(n.capacity!=n.counted||bool(n.rows)!=bool(n.capacity))return false;}
    else if(n.rows||n.capacity||n.pointersWritten)return false;
    if(second.returned){
        if(n.pointerZeroRole!=&arrayDataRole_||n.pointersWritten!=std::max<std::uint32_t>(1u,n.counted))return false;
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

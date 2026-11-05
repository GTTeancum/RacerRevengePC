//The first section-selection occurrence within the SAME Water stack Config.
bool OriginalWaterSourceConfig50LA::seekSection12F640(D& d)noexcept {
    ++sectionSeeksAttempted_;
    if(!sectionPosition_||!resolver_||handle_<0)return fail(d,E::phase,pc_);
    sectionCursor_=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    if(*sectionPosition_>std::numeric_limits<std::size_t>::max())return fail(d,E::unsupported,pc_);
    const bool returned=resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,
        static_cast<std::size_t>(*sectionPosition_),d);
    sectionCursor_=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=sectionCursor_;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    ++sectionSeeksReturned_;return true;
}
bool OriginalWaterSourceConfig50LA::allocateSectionBackingNative(bool table)noexcept {
    auto& a=table?sectionTableAllocation_:sectionDataAllocation_;
    if(a.attempted)return false;
    a.attempted=true;a.requested=source_.selected20<<(table?2u:10u);
    //305990 rounds a zero request to chunk16 and returns chunk+8. Minimum
    //eight native bytes support the source's unconditional child[0] even when
    //selected20==0. The ORIGINAL request stays zero, not a fabricated count1.
    a.nativeBytes=std::max<std::size_t>(8u,a.requested);
    if(table&&source_.selected20>0xffffffffu/1024u){
        sectionSizeBounded_=true;pc_=0x158144;return false; //Native aperture, not source overflow rejection.
    }
    using G=OriginalConfigProcessGlobals50LA;
    pc_=0x30570c;
    if(!G::reentrancyCurrentNative()){a.stop=NativeAllocationStop::reentUnknown;return false;}
    a.reent=G::state().reent38544C;
    if(*a.reent!=G::StdioIdentity::reent385160){a.stop=NativeAllocationStop::reentUnknown;return false;}
    pc_=0x305990;
    if(table){
        //Sidecars are private native pointer identities/knownness, not guessed
        //EE pointer bits. Prepare them before publishing the actual child24.
        std::unique_ptr<std::uint32_t[]> offsets;std::unique_ptr<Text::Bytes[]> rows;
        try{
            offsets.reset(new std::uint32_t[std::max<std::uint32_t>(1u,source_.selected20)]);
            if(source_.selected20)rows.reset(new Text::Bytes[source_.selected20]);
        }catch(const std::bad_alloc&){a.stop=NativeAllocationStop::sidecarRefused;return false;}
        sectionOffsets_=std::move(offsets);sectionRows_=std::move(rows);sectionCapacity_=source_.selected20;
    }
    void* const storage=std::malloc(a.nativeBytes);
    if(!storage){a.stop=NativeAllocationStop::backingRefused;return false;}
    //A native success-side storage projection, never an EE heap/free-list,
    //NULL, errno or allocator-failure receipt. No fallible work follows here.
    if(table){sectionTable_.reset(storage);source_.child24=&sectionTableRole_;pc_=0x15814c;}
    else{sectionData_.reset(storage);pc_=0x15815c;}
    a.returned=true;return true;
}
bool OriginalWaterSourceConfig50LA::selectGeneralInformation157D00(D& d)noexcept {
    d={};if(sectionAttempted_||nativeClosed_||!borrowed_||!textReturned_||
       !openReturned_||!*openReturned_||source_.opened0C!=1u||source_.mode10||source_.binary08||
       source_.selected20||source_.child24||!preflightNative(waterReceiver_))return fail(d,E::phase,0x157d00);
    sectionAttempted_=true;sectionPhase_=SectionPhase::lookup;pc_=0x157d1c;
    //opened0C=1 and mode10=0 genuinely skip the assertion/flush arms.
    constexpr char name[]="General Information"; //Actual3C07A0 caller literal.
    for(std::size_t i=0;i<sizeof(name);++i)sectionQuery_.put(i,static_cast<std::uint8_t>(name[i]));
    pc_=0x157dc0;std::int32_t index=-1;
    if(!lookupText157AF0(sectionQuery_,index))return false;
    sectionIndex_=index;
    if(index==-1){
        //157DD8/DC: missing selection does NOT clear child/count/name.
        sectionReturn_=false;sectionPhase_=SectionPhase::returned;pc_=0x158238;return true;
    }
    pc_=0x157f84;
    if(index<0||std::size_t(index)>=textTable_.rowExtent||!textTable_.rows[index].positionKnown)return false;
    const auto& row=textTable_.rows[index];sectionPosition_=row.position00;
    sectionPhase_=SectionPhase::firstSeek;pc_=0x157f88;if(!seekSection12F640(d))return false;
    //This is the first selection on this fresh Config. Actual NULL child24
    //takes157F94/98: no source free, count reset or fabricated child retirement.
    pc_=0x157fe8;if(!row.lineKnown)return false;sectionSpan_=row.line08;
    sectionPhase_=SectionPhase::copyName;
    //s3=1024 skips the literal impossible==4 assertion. strlen19<1023, then
    //15808C copies exactly19 bytes and158098 appends NUL; tail stays unknown.
    pc_=0x15808c;for(std::size_t i=0;i<sizeof(name)-1u;++i)source_.name28[i]=name[i];
    pc_=0x158098;source_.name28[sizeof(name)-1u]=0;sectionNameCopied_=true;
    const auto span=(*sectionSpan_&0x80000000u)?0u:*sectionSpan_; //Signed loop comparison.
    while(sectionScanned_[0]<span){
        sectionPhase_=SectionPhase::countRead;pc_=0x1580b4;++sectionReadCalls_[0];
        const bool returned=readTextLine12EE90(sectionInput_,sectionRead_,d);
        sectionCursor_=sectionRead_.after;if(!returned)return false;
        //Original ignores the reader's v0, including its real EOF arm.
        sectionPhase_=SectionPhase::countSplit;pc_=0x1580c4;
        const auto split=Text::split159440(sectionInput_,sectionLine_);pc_=split.pc;
        if(split.stop!=Text::Stop::complete)return false;
        const auto first=sectionLine_.value[0];
        if(first&&first!=';'){pc_=0x1580ec;++source_.selected20;++sectionCounted_;}
        ++sectionScanned_[0];
    }
    sectionPhase_=SectionPhase::secondSeek;pc_=0x158130;if(!seekSection12F640(d))return false;
    //15813C's self-store leaves the counted value unchanged.
    sectionPhase_=SectionPhase::tableAllocation;pc_=0x158144;
    if(!allocateSectionBackingNative(true))return false;
    sectionPhase_=SectionPhase::dataAllocation;pc_=0x158154;
    if(!allocateSectionBackingNative(false))return false;
    sectionPhase_=SectionPhase::pointers;pc_=0x158168;
    //Semantic pointer[0]=data, then pointers[i]=pointers[i-1]+1024. Raw32-bit
    //slots have no fabricated EE address bytes. This store occurs even at0.
    sectionOffsets_[0]=0;sectionPointersWritten_=1;
    for(std::uint32_t i=1;i<source_.selected20;++i){
        sectionOffsets_[i]=sectionOffsets_[i-1]+1024u;++sectionPointersWritten_;
    }
    while(sectionScanned_[1]<span){
        sectionPhase_=SectionPhase::storeRead;pc_=0x1581b4;++sectionReadCalls_[1];
        const bool returned=readTextLine12EE90(sectionInput_,sectionRead_,d);
        sectionCursor_=sectionRead_.after;if(!returned)return false;
        sectionPhase_=SectionPhase::storeSplit;pc_=0x1581c4;
        const auto split=Text::split159440(sectionInput_,sectionLine_);pc_=split.pc;
        if(split.stop!=Text::Stop::complete)return false;
        const auto first=sectionLine_.value[0];
        if(first&&first!=';'){
            sectionPhase_=SectionPhase::copyLine;pc_=0x1581f0;
            if(sectionStored_>=sectionCapacity_||sectionStored_>=sectionPointersWritten_||!sectionRows_)return false;
            pc_=0x1581f4;std::size_t n=0;
            for(;n<sectionLine_.value.size();++n){
                if(!sectionLine_.known[n])return false;if(!sectionLine_.value[n])break;
            }
            if(n==sectionLine_.value.size())return false;
            auto& stored=sectionRows_[sectionStored_];
            auto* raw=static_cast<std::uint8_t*>(sectionData_.get())+sectionOffsets_[sectionStored_];
            for(std::size_t i=0;i<=n;++i){stored.put(i,sectionLine_.value[i]);raw[i]=sectionLine_.value[i];}
            ++sectionStored_;
        }
        ++sectionScanned_[1];
    }
    pc_=0x158214;source_.field428=0;pc_=0x158218;sectionReturn_=true;
    sectionPhase_=SectionPhase::returned;pc_=0x158238;return true;
}
bool OriginalWaterSourceConfig50LA::preflightSectionNative()const noexcept {
    const auto& first=sectionTableAllocation_;const auto& second=sectionDataAllocation_;
    if(!sectionAttempted_){
        return sectionPhase_==SectionPhase::unentered&&!sectionNameCopied_&&!sectionReturn_&&!sectionIndex_&&
            !sectionPosition_&&!sectionSpan_&&!sectionCursor_&&!sectionSeeksAttempted_&&!sectionSeeksReturned_&&
            !sectionReadCalls_[0]&&!sectionReadCalls_[1]&&!sectionScanned_[0]&&!sectionScanned_[1]&&
            !sectionCounted_&&!sectionStored_&&!sectionPointersWritten_&&!sectionRead_.attempted&&
            !sectionTable_&&!sectionData_&&!sectionOffsets_&&!sectionRows_&&!sectionCapacity_&&
            !first.attempted&&!second.attempted&&!sectionSizeBounded_&&!source_.name28[0];
    }
    if(!textReturned_||!openReturned_||!*openReturned_||source_.binary08||source_.mode10||
       sectionPhase_==SectionPhase::unentered||bool(sectionReturn_)!=(sectionPhase_==SectionPhase::returned)||
       source_.selected20!=currentSectionCountNative()||source_.child24!=currentSectionChildNative()||
       sectionSeeksReturned_>sectionSeeksAttempted_||sectionSeeksAttempted_>2u||
       sectionSeeksAttempted_-sectionSeeksReturned_>1u||sectionCounted_>sectionScanned_[0]||
       sectionStored_>sectionScanned_[1]||sectionStored_>sectionCounted_)return false;
    if(!sectionIndex_){
        if(sectionPhase_!=SectionPhase::lookup||sectionPosition_||sectionSeeksAttempted_||sectionReturn_)return false;
    }else if(*sectionIndex_==-1){
        if(sectionReturn_!=false||sectionPosition_||sectionSpan_||sectionSeeksAttempted_||sectionNameCopied_)return false;
    }else{
        if(*sectionIndex_<0||std::size_t(*sectionIndex_)>=textTable_.rowExtent)return false;
        const auto& row=textTable_.rows[*sectionIndex_];
        if(sectionPosition_&&(!row.positionKnown||*sectionPosition_!=row.position00))return false;
        if(sectionSpan_&&(!row.lineKnown||*sectionSpan_!=row.line08||sectionSeeksReturned_<1u))return false;
        if(sectionReturn_&&!*sectionReturn_)return false;
    }
    if(sectionSeeksAttempted_&&(!sectionPosition_||!sectionCursor_))return false;
    if(sectionNameCopied_){
        constexpr char name[]="General Information";
        if(!sectionSpan_)return false;
        if(!nativeWakeLoad_.nameCopied)
            for(std::size_t i=0;i<sizeof(name);++i)if(source_.name28[i]!=name[i])return false;
    }else if(source_.name28[0])return false;
    const auto span=sectionSpan_&&!(*sectionSpan_&0x80000000u)?*sectionSpan_:0u;
    for(std::size_t pass=0;pass<2;++pass){
        if(sectionScanned_[pass]>span||sectionReadCalls_[pass]<sectionScanned_[pass]||
           sectionReadCalls_[pass]-sectionScanned_[pass]>1u)return false;
    }
    if(sectionSeeksAttempted_==2u&&(!sectionNameCopied_||sectionScanned_[0]!=span))return false;
    if(sectionReadCalls_[1]&&sectionSeeksReturned_!=2u)return false;
    if(sectionReadCalls_[0]||sectionReadCalls_[1]){
        const auto& read=sectionRead_;
        if(!read.attempted||!read.before||!read.after||read.requested>1022u||read.copied>read.requested||
           (read.transportReturned&&(!read.requested||read.copied!=read.requested))||
           (read.present&&(!read.returned||!read.transportReturned))||
           (read.returned&&!read.present&&(read.requested||read.copied||read.transportReturned))||
           (!read.returned&&read.after!=read.before))return false;
        if(read.returned&&read.present&&(*read.after<=*read.before||*read.after>*read.before+read.requested))return false;
        if(read.returned&&!read.present&&read.after!=read.before)return false;
        if(!read.returned&&sectionPhase_!=SectionPhase::countRead&&sectionPhase_!=SectionPhase::storeRead)return false;
        //The latest read may precede the second seek. sectionCursor_ owns the
        //latest operation's cursor, not a reinterpreted earlier-read receipt.
        if((sectionPhase_==SectionPhase::countRead||sectionPhase_==SectionPhase::countSplit||
            sectionPhase_==SectionPhase::storeRead||sectionPhase_==SectionPhase::storeSplit||
            sectionPhase_==SectionPhase::copyLine)&&sectionCursor_!=read.after)return false;
    }else if(sectionRead_.attempted)return false;
    if(bool(sectionTable_)!=(first.returned&&!nativeWakeRetirement_.tableRetired)||
       bool(sectionData_)!=(second.returned&&!nativeWakeRetirement_.dataRetired)||
       (second.attempted&&!first.returned)||(first.attempted&&sectionSeeksReturned_!=2u)||
       (first.returned&&(!sectionOffsets_||sectionCapacity_!=sectionCounted_||
            bool(sectionRows_)!=bool(sectionCounted_))))return false;
    for(const auto* a:{&first,&second}){
        const bool table=a==&first;
        if(!a->attempted){if(a->returned||a->requested||a->nativeBytes||a->reent||a->stop!=NativeAllocationStop::none)return false;continue;}
        if(a->requested!=(sectionCounted_<<(table?2u:10u))||
           a->nativeBytes!=std::max<std::size_t>(8u,a->requested))return false;
        if(a->returned&&(!a->reent||*a->reent!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160||
           a->stop!=NativeAllocationStop::none))return false;
        if(!a->returned&&!sectionSizeBounded_&&a->stop==NativeAllocationStop::none)return false;
        if(a->reent&&*a->reent!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160&&
           a->stop!=NativeAllocationStop::reentUnknown)return false;
    }
    if(sectionSizeBounded_&&(!first.attempted||first.returned||second.attempted||sectionCounted_<=0xffffffffu/1024u||
       sectionPhase_!=SectionPhase::tableAllocation||pc_!=0x158144u))return false;
    if(sectionOffsets_){
        if(sectionCapacity_!=sectionCounted_||bool(sectionRows_)!=bool(sectionCapacity_))return false;
    }else if(sectionRows_||sectionCapacity_||sectionPointersWritten_)return false;
    if(second.returned){
        if(sectionPointersWritten_!=std::max<std::uint32_t>(1u,sectionCounted_))return false;
        for(std::uint32_t i=0;i<sectionPointersWritten_;++i)if(sectionOffsets_[i]!=i*1024u)return false;
    }else if(sectionPointersWritten_||sectionStored_)return false;
    for(std::uint32_t i=0;i<sectionStored_;++i){
        const auto& row=sectionRows_[i];bool terminated=false;
        for(std::size_t j=0;j<row.value.size();++j){if(!row.known[j])return false;if(!row.value[j]){terminated=true;break;}}
        if(!terminated)return false;
    }
    if(sectionReturn_&&*sectionReturn_){
        if(!second.returned||sectionSeeksReturned_!=2u||sectionScanned_[0]!=span||sectionScanned_[1]!=span||
           sectionReadCalls_!=sectionScanned_||sectionStored_!=sectionCounted_)return false;
    }
    return true;
}

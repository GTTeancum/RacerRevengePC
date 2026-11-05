//Private helpers of the SAME Water Config. No Particle/parser occurrence reuse.
bool OriginalWaterSourceConfig50LA::readTextLine12EE90(D& d)noexcept {
    d={};if(!borrowed_||handle_<0||!resolver_||
       !resolver_->sourceBankMemberCurrent(selectedRoot_,this,handle_))return fail(d,E::phase,0x12ee90);
    textRead_={};auto& h=textRead_;h.attempted=true;++textReadCalls_;pc_=0x12ee90;
    auto& member=resolver_->freshFileCapacity_->members150[handle_];
    h.before=member.cursor10;h.after=member.cursor10;
    if(!lineAssertions12EE90(handle_,resolver_->freshFileCapacity_->memberCount154,d)){
        pc_=d.sourcePC;return false;}
    const auto remaining=member.length14-member.cursor10; //Original SUBU, then signed BGTZ.
    if(!remaining||(remaining&0x80000000u)){
        pc_=0x12efbc;textInput_.put(0,0);h.returned=true;h.present=false;
        expectedCursor_=h.after;pc_=0x157768;return true;
    }
    h.requested=std::min<std::uint32_t>(remaining,1022u);pc_=0x130560;
    h.transportReturned=resolver_->readSourceBankMember130560(selectedRoot_,this,handle_,
        textInput_.value.data(),h.requested,h.copied,false,d);
    //The source stack buffer persists. Never erase old known suffix bytes.
    for(std::size_t i=0;i<h.copied;++i)textInput_.known[i]=true;
    h.after=member.cursor10;expectedCursor_=h.after;
    if(!h.transportReturned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    //The immutable numeric backend's returned positive call owns its whole
    //requested span. This is a byte result, not a12FB30 fread item count.
    if(h.copied!=h.requested){pc_=0x12f1b4;return fail(d,E::unsupported,pc_);}
    auto cursor=member.cursor10;
    const auto suffix=Text::lineSuffix12F1B4(textInput_,h.requested,h.copied!=0,cursor);
    pc_=suffix.pc;if(suffix.stop!=Text::Stop::complete)return false;
    member.cursor10=cursor;h.after=cursor;expectedCursor_=h.after;
    h.present=h.copied!=0;h.returned=true;pc_=0x157768;return true;
}
bool OriginalWaterSourceConfig50LA::lookupText157AF0(std::int32_t& out)noexcept {
    pc_=0x157c44;const auto& table=textTable_;
    if(table.sourceCleaned||table.allocationPending||table.allocationFault)return false;
    if(!table.count14||(table.count14&0x80000000u)){out=-1;pc_=0x157c90;return true;}
    using G=OriginalConfigProcessGlobals50LA;const G::WaterSprintfAccess key;
    for(std::uint32_t index=0;index<table.count14;++index){
        pc_=0x157c50;
        if(!table.rows||!table.sourceStorage||table.rows!=table.owned.get()||
           table.orphan||table.orphanStorage||index>=table.rowExtent)return false;
        const auto& row=table.rows[index].name0C;bool equal=false;
        for(std::size_t i=0;;++i){
            pc_=i?0x308f60u:0x308f08u;
            if(i>=textName_.value.size()||!textName_.known[i])return false;
            const auto left=textName_.value[i];pc_=left?0x308f18u:0x308f80u;
            if(!row.known[i])return false;const auto right=row.value[i];std::int32_t a=0,b=0;
            if(left){
                pc_=0x308f34;if(!G::waterFold308F00(key,left,a,pc_))return false;
                pc_=0x308f3c;if(!G::waterFold308F00(key,right,b,pc_))return false;
                if(a==b)continue;
            }
            pc_=0x308f90;if(!G::waterFold308F00(key,left,a,pc_))return false;
            pc_=0x308f94;if(!G::waterFold308F00(key,right,b,pc_))return false;
            equal=a==b;break;
        }
        if(equal){out=static_cast<std::int32_t>(index);pc_=0x157c70;return true;}
    }
    out=-1;pc_=0x157c90;return true;
}
bool OriginalWaterSourceConfig50LA::allocateTextBackingNative()noexcept {
    auto& table=textTable_;
    if(!textAllocation_||!table.allocationPending||table.allocationFault||table.sourceCleaned||
       table.orphan||table.orphanStorage||table.rows!=table.owned.get()||
       (bool(table.rows)!=bool(table.sourceStorage))||!table.pendingCapacity||
       table.pendingCapacity>0xffffffffu/0x418u||
       table.pendingCapacity>std::numeric_limits<std::size_t>::max()/sizeof(Text::Row))return false;
    if(table.pendingReallocation){
        if(!table.rows||table.rowExtent>table.pendingCapacity||table.capacity18!=table.pendingCapacity)return false;
    }else if(table.rows||table.rowExtent||table.pendingCapacity!=10u)return false;
    ++textAllocationsAttempted_;textAllocationReent_.reset();textAllocationStop_=NativeAllocationStop::none;
    //Native success-only allocator projection. Neither admission nor host
    //allocation is an execution receipt for EE arena385468, sbrk or errno.
    using G=OriginalConfigProcessGlobals50LA;
    pc_=table.pendingReallocation?0x3080d0u:0x30570cu;
    if(!G::reentrancyCurrentNative()){
        textAllocationStop_=NativeAllocationStop::reentUnknown;return false;}
    textAllocationReent_=G::state().reent38544C;
    if(*textAllocationReent_!=G::StdioIdentity::reent385160){
        textAllocationStop_=NativeAllocationStop::reentUnknown;return false;}
    pc_=table.pendingReallocation?0x3080f0u:0x305990u;
    std::unique_ptr<Text::Row[]> replacement;
    try{replacement.reset(new Text::Row[table.pendingCapacity]);}
    catch(const std::bad_alloc&){textAllocationStop_=NativeAllocationStop::sidecarRefused;return false;}
    for(std::size_t i=0;i<table.rowExtent;++i)Text::copyKnownRow(table.rows[i],replacement[i]);
    const auto bytes=std::size_t(table.pendingCapacity)*0x418u;
    void* const allocation=table.pendingReallocation?std::realloc(table.sourceStorage,bytes):std::malloc(bytes);
    if(!allocation){
        //Host refusal is NOT original NULL, errno or orphan-pointer behavior.
        //The original pending capacity store and old source pointer survive.
        textAllocationStop_=NativeAllocationStop::backingRefused;return false;
    }
    table.sourceStorage=allocation;table.owned=std::move(replacement);table.rows=table.owned.get();
    table.rowExtent=table.pendingCapacity;source_.text1C=table.sourceStorage; //1575D8/1575E8.
    if(!table.pendingReallocation){table.capacity18=10;source_.index18=10;} //1575F0 AFTER pointer.
    table.allocationPending=false;++textAllocationsReturned_;pc_=0x1575f8;return true;
}
bool OriginalWaterSourceConfig50LA::parseText157500(D& d)noexcept {
    if(textEntered_||!textCallReached_||!seekReturned_||pc_!=0x157500u||source_.binary08||
       source_.opened0C!=1u||source_.mode10||source_.count14||source_.index18||source_.text1C)return false;
    textEntered_=true;textLineOrdinal_=0;
    for(;;){
        textPhase_=TextPhase::reading;pc_=0x157760;
        if(!readTextLine12EE90(d))return false;
        if(!textRead_.present){
            textPhase_=TextPhase::finishLast;
            const auto final=Text::finishLastRow157770(textTable_,textLineOrdinal_);syncTextFields();pc_=final.pc;
            if(final.stop!=Text::Stop::complete)return false;
            pc_=0x1577c0;textPhase_=TextPhase::returned;textReturned_=true;return true;
        }
        textLookup_.reset();textTell_.reset();textPhase_=TextPhase::splitting;
        auto step=Text::split159440(textInput_,textLine_);pc_=step.pc;
        if(step.stop!=Text::Stop::complete)return false;
        if(textLine_.value[0]=='['){
            textPhase_=TextPhase::section;step=Text::sectionName157548(textLine_,textName_);pc_=step.pc;
            if(step.stop!=Text::Stop::complete)return false;
            textPhase_=TextPhase::lookup;std::int32_t found=-1;
            if(!lookupText157AF0(found))return false;textLookup_=found;
            if(found!=-1){textPhase_=TextPhase::duplicate;pc_=0x1576d0;return false;}
            textPhase_=TextPhase::allocation;Text::AllocationRequest request;
            step=Text::prepareNewRow15758C(textTable_,request);syncTextFields();pc_=step.pc;
            if(step.stop==Text::Stop::needsAllocation){
                textAllocation_=request;if(!allocateTextBackingNative())return false;
            }else if(step.stop!=Text::Stop::complete)return false;
            textPhase_=TextPhase::copyName;step=Text::copyRowName15761C(textTable_,textName_);pc_=step.pc;
            if(step.stop!=Text::Stop::complete)return false;
            textPhase_=TextPhase::tell;pc_=0x15762c;std::size_t position=0;
            if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d)){
                pc_=d.sourcePC?d.sourcePC:pc_;return false;}
            if(position>0xffffffffULL){pc_=0x12f630;return fail(d,E::unsupported,pc_);}
            textTell_=position<=0x7fffffffULL?std::uint64_t(position):(0xffffffff00000000ULL|position);
            textPhase_=TextPhase::finishRow;step=Text::finishRow157654(textTable_,*textTell_,textLineOrdinal_);
            syncTextFields();pc_=step.pc;if(step.stop!=Text::Stop::complete)return false;
        }
        ++textLineOrdinal_;++textCompletedLines_; //157544 delay or157748; low32 wrap.
    }
}
bool OriginalWaterSourceConfig50LA::preflightTextNative()const noexcept {
    const auto& t=textTable_;const auto& read=textRead_;
    if(t.orphan||t.orphanStorage||t.allocationFault||t.sourceCleaned||t.rows!=t.owned.get()||
       bool(t.rows)!=bool(t.sourceStorage)||t.count14>t.capacity18||t.count14>t.rowExtent||
       (t.rows&&(!t.rowExtent||t.rowExtent>t.capacity18))||(!t.rows&&t.rowExtent))return false;
    if(!textEntered_)return textPhase_==TextPhase::unentered&&!textReturned_&&!textCompletedLines_&&
        !textReadCalls_&&!textLineOrdinal_&&!read.attempted&&!read.returned&&!read.before&&!read.after&&
        !read.requested&&!read.copied&&!read.transportReturned&&!read.present&&!textLookup_&&!textTell_&&
        !textAllocationsAttempted_&&!textAllocationsReturned_&&!textAllocation_&&!textAllocationReent_&&
        textAllocationStop_==NativeAllocationStop::none&&!t.count14&&!t.capacity18&&!t.rows&&!t.allocationPending;
    if(!textCallReached_||!seekReturned_||source_.binary08||textPhase_==TextPhase::unentered||
       textReturned_!=(textPhase_==TextPhase::returned)||textLineOrdinal_!=std::uint32_t(textCompletedLines_)||
       textReadCalls_!=textCompletedLines_+1u||!read.attempted||!read.before||!read.after||
       read.requested>1022u||read.copied>read.requested||
       (read.transportReturned&&(!read.requested||read.copied!=read.requested))||
       (read.present&&(!read.returned||!read.transportReturned))||
       (read.returned&&!read.present&&(read.requested||read.copied||read.transportReturned))||
       (!read.returned&&read.after!=read.before))return false;
    if(read.returned&&read.present&&(*read.after<=*read.before||
       *read.after>*read.before+read.requested))return false;
    if(read.returned&&!read.present&&read.after!=read.before)return false;
    if(textPhase_!=TextPhase::reading&&!read.returned)return false;
    if((textPhase_==TextPhase::finishLast||textReturned_)&&read.present)return false;
    if(textPhase_!=TextPhase::reading&&textPhase_!=TextPhase::finishLast&&!textReturned_&&!read.present)return false;
    if(textReturned_&&(!openReturned_||!*openReturned_))return false;
    if(t.rowNameCopied&&(textPhase_!=TextPhase::tell&&textPhase_!=TextPhase::finishRow))return false;
    if((textPhase_==TextPhase::copyName||textPhase_==TextPhase::tell||textPhase_==TextPhase::finishRow)&&textLookup_!=-1)return false;
    if(textPhase_==TextPhase::duplicate&&(!textLookup_||*textLookup_<0||pc_!=0x1576d0u))return false;
    if(textAllocationsReturned_>textAllocationsAttempted_||
       textAllocationsAttempted_-textAllocationsReturned_>1u||
       bool(textAllocation_)!=bool(textAllocationsAttempted_)||
       (textAllocationStop_!=NativeAllocationStop::none&&textAllocationsAttempted_==textAllocationsReturned_))return false;
    if(textAllocation_){
        const auto& a=*textAllocation_;
        if(a.bytes%0x418u||!a.bytes||a.callPc!=(a.reallocate?0x1575ccu:0x1575e0u)||
           (a.reallocate&&a.bytes/0x418u<20u)||(!a.reallocate&&a.bytes!=0x28f0u))return false;
        if(textAllocationsAttempted_!=textAllocationsReturned_){
            if(textPhase_!=TextPhase::allocation||!t.allocationPending||
               t.pendingReallocation!=a.reallocate||t.pendingCapacity!=a.bytes/0x418u||
               textAllocationStop_==NativeAllocationStop::none)return false;
        }else if(!textAllocationReent_||textAllocationStop_!=NativeAllocationStop::none)return false;
    }
    if(textAllocationReent_&&*textAllocationReent_!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160&&
       textAllocationStop_!=NativeAllocationStop::reentUnknown)return false;
    if(t.allocationPending){
        if(textPhase_!=TextPhase::allocation||!t.pendingCapacity||
           t.pendingCapacity>0xffffffffu/0x418u)return false;
        if(t.pendingReallocation){if(!t.rows||t.capacity18!=t.pendingCapacity||t.rowExtent+10u!=t.capacity18)return false;}
        else if(t.rows||t.capacity18||t.pendingCapacity!=10u)return false;
    }else if(t.rows&&t.capacity18!=t.rowExtent)return false;
    for(std::size_t i=0;i<t.count14;++i){
        const auto& row=t.rows[i];if(!row.positionKnown||!row.lineKnown)return false;
        bool terminated=false;for(std::size_t j=0;j<row.name0C.value.size();++j){
            if(!row.name0C.known[j])return false;if(!row.name0C.value[j]){terminated=true;break;}}
        if(!terminated)return false;
    }
    return true;
}

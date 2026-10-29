// Included within OriginalWorldSourceConfig50LA. Private authentic text branch.
struct TextFree {void operator()(void* p)const noexcept {std::free(p);}};
std::unique_ptr<void,TextFree> textChildTable_,textChildData_;
std::unique_ptr<std::uint32_t[]> textChildOffsets_,textChildLengths_;
std::unique_ptr<bool[]> textChildFilled_;
std::uint32_t textChildCapacity_=0,textPointersWritten_=0;
bool textSelectBounded_=false,textSelectionReturned_=false;
std::uint32_t textScanLine_=0,textStoredLines_=0;
File::Line12EE90 textSelectionInput_;
Text::Bytes textSelectionBytes_,textSelectionLine_;

bool textConfigCurrent()const noexcept {
    return thread_==std::this_thread::get_id()&&source_&&file_&&!sourceClosed_&&!nativeClosed_&&
        textReturned_&&returned_==Return::one&&source_->binary08==0&&source_->opened0C==1&&
        source_->mode10==0&&source_->text1C==textTable_.sourceStorage&&
        source_->count14==textTable_.count14&&source_->index18==textTable_.capacity18&&
        source_->child24==textChildTable_.get()&&file_->current(*this,source_->file00);
}
bool retireTextChild157F90()noexcept {
    if(source_->child24!=textChildTable_.get())return false;
    if(!source_->child24)return true; // Original skips count20 reset on NULL.
    // Original first loads child[0]. If unconditional store was never reached,
    // do not manufacture NULL or a source free for the second allocation.
    if(!textPointersWritten_){sourcePC_=0x157FA4u;return false;}
    if(textChildData_)textChildData_.reset(); //305728(child[0]); NULL skips.
    textChildTable_.reset(); //305728(child).
    source_->selected20=0;source_->child24=nullptr; //157FC0/4.
    textChildOffsets_.reset();textChildLengths_.reset();textChildFilled_.reset();
    textChildCapacity_=0;textPointersWritten_=0;return true;
}
bool readTextSelectionLine(std::uint32_t readPc,std::uint32_t splitPc,D& d)noexcept {
    sourcePC_=readPc;
    if(!file_->read12EE90(*this,source_->file00,textSelectionInput_,d))return false;
    // Both original fixed-span passes IGNORE reader v0. A genuine EOF stores
    // NUL at0 and still reaches split; native bounded failure does not.
    for(std::size_t i=0;i<1024;++i){
        textSelectionBytes_.known[i]=textSelectionInput_.known[i];
        if(textSelectionInput_.known[i])
            textSelectionBytes_.value[i]=static_cast<std::uint8_t>(textSelectionInput_.bytes[i]);
    }
    sourcePC_=splitPc;
    const auto split=Text::split159440(textSelectionBytes_,textSelectionLine_);
    if(split.stop!=Text::Stop::complete){sourcePC_=split.pc;return false;}
    return true;
}
bool selectTextSection157D00(bool environment,bool& sourceReturned,D& d)noexcept {
    d={};sourceReturned=false;sourcePC_=0x157D1Cu;
    if(!textConfigCurrent()||cohort_!=Cohort::sceneScb||textSelectBounded_||
       (environment?sectionAttempts_!=2:sectionAttempts_>=2)||
       (sectionAttempts_&&!sectionReturned_))return false;
    ++sectionAttempts_;sectionReturned_=false;textSelectionReturned_=false;
    // Latch prevents re-entry after any partial failure. Clear only on an
    // actual selector return (including original missing-section return0).
    textSelectBounded_=true;
    const std::string_view wanted=environment?"Environment":"SceneInfo";
    Text::Bytes query;
    for(std::size_t i=0;i<wanted.size();++i)query.put(i,static_cast<std::uint8_t>(wanted[i]));
    query.put(wanted.size(),0);
    const auto found=Text::lookupText157C44(textTable_,query);sourcePC_=found.pc;
    if(found.stop!=Text::Stop::complete)return false;
    if(found.index<0){sourcePC_=0x15821Cu;textSelectBounded_=false;return true;}
    const auto index=static_cast<std::uint32_t>(found.index);
    if(index>=textTable_.rowExtent)return false;
    const auto& row=textTable_.rows[index];
    sourcePC_=0x157F84u;if(!row.positionKnown)return false;
    // Source LD feeds original seek. Existing native seek admits only offsets
    // within the owned member; do not truncate a sign-extended negative tell.
    if(row.position00>std::numeric_limits<std::size_t>::max())return false;
    sourcePC_=0x157F88u;
    if(!file_->seek12F640(*this,source_->file00,static_cast<std::size_t>(row.position00),d))return false;
    if(!retireTextChild157F90())return false;
    sourcePC_=0x157FE8u;if(!row.lineKnown)return false;
    const auto span=row.line08;
    for(std::size_t i=0;i<wanted.size();++i)source_->name28[i]=wanted[i];
    source_->name28[wanted.size()]=0; //15808C/98, trusted actual caller name.
    textScanLine_=0;
    while((span&0x80000000u)==0&&textScanLine_<span){
        if(!readTextSelectionLine(0x1580B4u,0x1580C4u,d))return false;
        const auto first=textSelectionLine_.value[0];
        if(first&&first!=';')++source_->selected20; //1580EC.
        ++textScanLine_;
    }
    sourcePC_=0x158130u;
    if(!file_->seek12F640(*this,source_->file00,static_cast<std::size_t>(row.position00),d))return false;
    const auto count=source_->selected20;
    // Native sidecar preadmission, not a source allocator result or rollback.
    if(count>0xffffffffu/1024u){sourcePC_=0x158144u;return false;}
    try {
        textChildOffsets_.reset(new std::uint32_t[count]);
        textChildLengths_.reset(new std::uint32_t[count]);
        textChildFilled_.reset(new bool[count]{});
    }catch(const std::bad_alloc&){sourcePC_=0x158144u;return false;}
    textChildCapacity_=count;textPointersWritten_=0;
    sourcePC_=0x158144u;textChildTable_.reset(std::malloc(std::size_t(count)*4u));
    source_->child24=textChildTable_.get(); //15814C, including NULL.
    sourcePC_=0x158154u;textChildData_.reset(std::malloc(std::size_t(count)*1024u));
    sourcePC_=0x158168u;
    // Original unconditionally writes child[0] even for count0. Retain both
    // actual allocation outcomes, but never emulate an out-of-bounds host write.
    if(!textChildTable_||!count)return false;
    textChildOffsets_[0]=0;textPointersWritten_=1;
    for(std::uint32_t i=1;i<count;++i){
        textChildOffsets_[i]=textChildOffsets_[i-1]+1024u;++textPointersWritten_;
    }
    // Raw source-size pointer-table backing is separate from host64 pointer
    // semantics. Sidecar offsets represent produced pointer lanes; no guessed
    // EE address bits are stored/exposed in the raw32-bit pointer slots.
    textScanLine_=0;textStoredLines_=0;
    while((span&0x80000000u)==0&&textScanLine_<span){
        if(!readTextSelectionLine(0x1581B4u,0x1581C4u,d))return false;
        const auto first=textSelectionLine_.value[0];
        if(first&&first!=';'){
            sourcePC_=0x1581F4u;
            if(textStoredLines_>=count||textStoredLines_>=textPointersWritten_||!textChildData_)return false;
            std::size_t n=0;
            while(n<1024){
                if(!textSelectionLine_.known[n])return false;
                if(!textSelectionLine_.value[n])break;++n;
            }
            if(n==1024)return false;
            auto* destination=static_cast<std::uint8_t*>(textChildData_.get())+textChildOffsets_[textStoredLines_];
            for(std::size_t i=0;i<=n;++i)destination[i]=textSelectionLine_.value[i];
            textChildLengths_[textStoredLines_]=static_cast<std::uint32_t>(n);
            textChildFilled_[textStoredLines_]=true;++textStoredLines_;
        }
        ++textScanLine_;
    }
    source_->field428=0; //158214.
    sourcePC_=0x15821Cu;textSelectionReturned_=true;textSelectBounded_=false;
    sectionReturned_=true;sourceReturned=true;return true;
}
